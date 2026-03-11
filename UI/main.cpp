// Fichiers système/bibliothèque
#include <QApplication>
#include <QLockFile>
#include <QDir>
#include <QMessageBox>
#include <QStandardPaths>
#include <QIcon>

// Fichiers locaux
#include "NexusWindow.h"
#include "../Classe/CryptoUtils.h"
#include "../Popup/MasterPasswordPopup.h"

#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/conf.h>

int main(int argc, char* argv[])
{
    //DEBUG
    qputenv("QTWEBENGINE_REMOTE_DEBUGGING", "9222");
    //DEBUG

    qputenv("QTWEBENGINE_CHROMIUM_FLAGS",
        "--disable-features=IsolateOrigins,site-per-process,ClipboardUserActivation,RendererCodeIntegrity "
        "--enable-features=ClipboardContentSetting,ClipboardReading,ClipboardWrite "
        "--enable-experimental-web-platform-features "
        "--enabble-clipboard"
    );

    QApplication a(argc, argv);
	a.setApplicationName("NexusUtils");
    a.setWindowIcon(QIcon("/icons/app_icon.png"));

	OPENSSL_init_crypto(OPENSSL_INIT_LOAD_CONFIG, nullptr);
    RAND_poll();

    // Empêcher plusieurs instances
    QString lockPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/NexusUtils.lock";
    QDir().mkpath(QFileInfo(lockPath).absolutePath());

    QLockFile lock(lockPath);
    lock.setStaleLockTime(0);

    if (!lock.tryLock()) {
        QMessageBox::warning(nullptr, "Instance déjà ouverte",
            "NexusUtils est déjà en cours d'exécution.");
        return 0;
    }

    // ============================================================
    //  PROMPT MASTER PASSWORD AU LANCEMENT
    // ============================================================

    QByteArray stored = CryptoUtils::loadMasterKey();
    // stored = hash(32) + salt(16)

    if (!stored.isEmpty())
    {
        QByteArray expectedHash = stored.left(32);
        QByteArray salt = stored.mid(32);

        MasterPasswordPopup dlg(nullptr);
        dlg.setWindowTitle("Mot de passe maître requis");

        while (true)
        {
            if (dlg.exec() != QDialog::Accepted)
                return 0; // L'utilisateur annule → on quitte

            QString pwd = dlg.password();
            if (pwd.isEmpty())
                continue;

            QByteArray hash(32, 0);

            PKCS5_PBKDF2_HMAC(
                pwd.toUtf8().constData(), pwd.size(),
                reinterpret_cast<const unsigned char*>(salt.data()), salt.size(),
                100000,
                EVP_sha256(),
                32,
                reinterpret_cast<unsigned char*>(hash.data())
            );

            if (hash == expectedHash)
                break; // OK → on lance l'app

            QMessageBox::warning(nullptr, "Erreur", "Mot de passe incorrect.");
        }
    }

    // ============================================================
    //  LANCEMENT DE L'APPLICATION
    // ============================================================

    NexusWindow w;
    w.show();

    return a.exec();
}
