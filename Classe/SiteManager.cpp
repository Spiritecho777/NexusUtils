#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QTextStream>

#include "SiteItem.h"
#include "SiteManager.h"
#include "CryptoUtils.h"


SiteManager::SiteManager()
{
	appDirectory = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/NexusUtilsData";
	
	QDir().mkpath(appDirectory);

	filePathE = appDirectory + "/sites.enc";

	if (!QFile::exists(filePathE))
	{
		QByteArray empty;
		SaveDecrypted(empty);
	}
}

#pragma Traitement de données
bool SiteManager::fileExists() const
{
	return QFile::exists(filePathE);
}

QVector<SiteItem> SiteManager::getAllSites() const
{
	QVector<SiteItem> sites;
	QByteArray decrypted = loadDecrypted();
	QTextStream stream(decrypted);

	while (!stream.atEnd())
	{
		QString line = stream.readLine();
		QStringList parts = line.split("||");

		if (parts.size() == 4)
		{
			SiteItem s;
			s.name = parts[0];
			s.url = parts[1];
			s.username = parts[2];
			s.password = parts[3];
			sites.append(s);
		}
	}
	return sites;
}
#pragma	endregion

#pragma Manipulation de données
void SiteManager::addSite(const QString& name, const QString& url)
{
	QByteArray decrypted = loadDecrypted();
	QString text = QString::fromUtf8(decrypted);

	if (!text.isEmpty() && !text.endsWith("\n"))
		text += "\n";

	text += name + "||" + url + "|| || ";

	SaveDecrypted(text.toUtf8());
}

void SiteManager::addCredsSite(const QString& user, const QString& password, const SiteItem& site)
{
	QByteArray decrypted = loadDecrypted();
	QString text = QString::fromUtf8(decrypted);

	QStringList lines = text.split("\n");

	for (int i = 0; i < lines.size(); ++i)
	{
		QStringList parts = lines[i].split("||");
		if (parts.size() == 4 && parts[0] == site.name)
		{
			parts[2] = user;
			parts[3] = password;
			lines[i] = parts.join("||");
		}
	}

	SaveDecrypted(lines.join("\n").toUtf8());
}

void SiteManager::deleteSite(const SiteItem& site)
{
	QByteArray decrypted = loadDecrypted();
	QString text = QString::fromUtf8(decrypted);

	QStringList lines = text.split("\n");
	QString prefix = site.name + "||";

	QStringList filtered;
	for (const QString& line : lines)
	{
		if (!line.startsWith(prefix))
			filtered.append(line);
	}

	SaveDecrypted(filtered.join("\n").toUtf8());
}
#pragma endregion

#pragma Sécurité
void SiteManager::SaveDecrypted(const QByteArray& plain) const
{
	CryptoUtils crypto;
	QByteArray encrypted = crypto.encryptBytes(plain);

	QFile file(filePathE);
	file.open(QIODevice::WriteOnly | QIODevice::Truncate);
	file.write(encrypted);
	file.close();
}

QByteArray SiteManager::loadDecrypted() const
{
	QFile file(filePathE);
	if(!file.open(QIODevice::ReadOnly))
		return {};

	QByteArray encrypted = file.readAll();
	file.close();

	CryptoUtils crypto;
	return crypto.decryptBytes(encrypted);
}
#pragma endregion
