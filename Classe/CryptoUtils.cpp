#include "CryptoUtils.h"

#include <QCryptographicHash>
#include <QRandomGenerator>
#include <QSysInfo>
#include <QStorageInfo>
#include <openssl/evp.h>
#include <openssl/rand.h>

static QString computeMachineGuid();

CryptoUtils::CryptoUtils()
{
    QString guid = computeMachineGuid(); 
    password = guid.toUtf8();
}

QByteArray CryptoUtils::deriveKey(const QByteArray& password, const QByteArray& salt) const
{
    QByteArray key;
    key.resize(keySize);

    PKCS5_PBKDF2_HMAC(
        password.constData(),
        password.size(),
        reinterpret_cast<const unsigned char*>(salt.constData()),
        salt.size(),
        iterations,
        EVP_sha256(),
        keySize,
        reinterpret_cast<unsigned char*>(key.data())
    );

    return key;
}

QByteArray CryptoUtils::encryptBytes(const QByteArray& plain) const
{
    // 1) Générer un IV (16 bytes)
    QByteArray iv(16, 0);
    RAND_bytes(reinterpret_cast<unsigned char*>(iv.data()), iv.size());

    // 2) Dériver la clé
    QByteArray key = deriveKey(password, iv);

    // 3) Préparer le contexte AES
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    QByteArray encrypted;
    encrypted.reserve(plain.size() + 32);

    // 4) Ajouter l’IV au début du fichier chiffré
    encrypted.append(iv);

    EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr,
        reinterpret_cast<const unsigned char*>(key.data()),
        reinterpret_cast<const unsigned char*>(iv.data()));

    QByteArray buffer;
    buffer.resize(plain.size() + 32);

    int outLen = 0;
    EVP_EncryptUpdate(ctx,
        reinterpret_cast<unsigned char*>(buffer.data()),
        &outLen,
        reinterpret_cast<const unsigned char*>(plain.data()),
        plain.size());

    encrypted.append(buffer.constData(), outLen);

    int finalLen = 0;
    EVP_EncryptFinal_ex(ctx,
        reinterpret_cast<unsigned char*>(buffer.data()),
        &finalLen);

    encrypted.append(buffer.constData(), finalLen);

    EVP_CIPHER_CTX_free(ctx);

    return encrypted;
}

QByteArray CryptoUtils::decryptBytes(const QByteArray& encrypted) const
{
    if (encrypted.size() < 16)
        return {};

    // 1) Extraire l’IV (salt)
    QByteArray iv = encrypted.left(16);
    QByteArray cipher = encrypted.mid(16);

    // 2) Dériver la clé
    QByteArray key = deriveKey(password, iv);

    // 3) Déchiffrer
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    QByteArray decrypted;
    decrypted.resize(cipher.size() + 32);

    EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr,
        reinterpret_cast<const unsigned char*>(key.data()),
        reinterpret_cast<const unsigned char*>(iv.data()));

    int outLen = 0;
    EVP_DecryptUpdate(ctx,
        reinterpret_cast<unsigned char*>(decrypted.data()),
        &outLen,
        reinterpret_cast<const unsigned char*>(cipher.data()),
        cipher.size());

    int finalLen = 0;
    EVP_DecryptFinal_ex(ctx,
        reinterpret_cast<unsigned char*>(decrypted.data()),
        &finalLen);

    EVP_CIPHER_CTX_free(ctx);

    decrypted.resize(outLen + finalLen);
    return decrypted;
}

static QString computeMachineGuid()
{
    // Username
    QString user = qEnvironmentVariable("USERNAME");
    if (user.isEmpty())
        user = qEnvironmentVariable("USER");

    // OS info
    QString osVersion = QSysInfo::prettyProductName();
    QString osArch = QSysInfo::currentCpuArchitecture();
    QString osDesc = QSysInfo::kernelType() + " " + QSysInfo::kernelVersion();

    // System drive
#ifdef _WIN32
    QString systemDrive = QString::fromUtf8(qgetenv("SystemDrive"));
    if (systemDrive.isEmpty())
        systemDrive = "C:/";
#else
    QString systemDrive = "/";
#endif

    QStorageInfo drive(systemDrive);

    QString driveFormat = drive.fileSystemType().isEmpty()
        ? "UnknownFormat"
        : QString::fromUtf8(drive.fileSystemType());

    QString driveLabel = drive.displayName().isEmpty()
        ? "UnknownLabel"
        : drive.displayName();

    QString driveName = drive.rootPath().isEmpty()
        ? "UnknownDrive"
        : drive.rootPath();

    // Build raw string (same logic as C#)
    QString raw = user + "|" +
        osVersion + "|" +
        osArch + "|" +
        osDesc + "|" +
        driveFormat + "|" +
        driveLabel + "|" +
        driveName;

    // SHA‑256
    QByteArray hash = QCryptographicHash::hash(raw.toUtf8(), QCryptographicHash::Sha256);

    return hash.toHex().toUpper();
}
