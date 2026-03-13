// Fichiers système/bibliothèque
#include <QApplication>
#include <QLockFile>
#include <QDir>
#include <QMessageBox>
#include <QStandardPaths>
#include <QIcon>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QTimer>
#include <QWebEngineProfile>
#include <QWebEngineDownloadRequest>
#include <QFileDialog>
#include <QFileInfo>

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
    //  TÉLÉCHARGEMENTS
    // ============================================================

    QObject::connect(
        QWebEngineProfile::defaultProfile(),
        &QWebEngineProfile::downloadRequested,
        [](QWebEngineDownloadRequest* download) {
            QString dir = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
            QString path = QFileDialog::getSaveFileName(
                nullptr,
                "Enregistrer sous",
                dir + "/" + download->downloadFileName()
            );
            if (path.isEmpty()) { download->cancel(); return; }
            download->setDownloadDirectory(QFileInfo(path).absolutePath());
            download->setDownloadFileName(QFileInfo(path).fileName());
            download->accept();
        }
    );

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

    QSystemTrayIcon* tray = new QSystemTrayIcon(&a);

    QIcon icon(":/Asset/Icone.png");
    tray->setIcon(icon);
    tray->setToolTip("NexusUtils");

    QMenu* trayMenu = new QMenu();
    QAction* actionShow = trayMenu->addAction("Ouvrir");
    QAction* actionQuit = trayMenu->addAction("Quitter");

    tray->setContextMenu(trayMenu);

    QObject::connect(actionShow, &QAction::triggered, [&w]() {
        w.show();
        w.raise();
        w.activateWindow();
    });

    QObject::connect(actionQuit, &QAction::triggered, &a, &QApplication::quit);

    QObject::connect(tray, &QSystemTrayIcon::activated, [&w](QSystemTrayIcon::ActivationReason reason) {
        if (reason == QSystemTrayIcon::DoubleClick) {
            w.show();
            w.raise();
            w.activateWindow();
        }
    });

    QTimer::singleShot(1000, [tray]() {
        tray->show();
    });

    return a.exec();
}
