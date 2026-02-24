//Fichiers systeme/bibliotheque
#include <QApplication>
#include <QLockFile>
#include <QDir>
#include <QMessageBox>
#include <QStandardPaths>
#include <QIcon>

//Fichiers local (du projet)
#include "NexusWindow.h"

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);

	a.setWindowIcon(QIcon("/icons/app_icon.png"));

	QString lockPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/NexusUtils.lock";

	QDir().mkpath(QFileInfo(lockPath).absolutePath());

	QLockFile lock(lockPath);
	lock.setStaleLockTime(0);

	if (!lock.tryLock()) {
		QMessageBox::warning(nullptr, "Instance déjà ouverte",
			"NexusUtils est déjà en cours d'exécution.");
		return 0;
	}

	NexusWindow w;
	w.show();

	return a.exec();
}