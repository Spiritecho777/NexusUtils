#pragma once
#include <QVector>
#include <QString>
#include "SiteItem.h"

class SiteManager
{
public:
	SiteManager();
	bool fileExists() const;
	QVector<SiteItem> getAllSites() const;

	void deleteSite(const SiteItem& site);
	void addSite(const QString& name, const QString& url);
	void addCredsSite(const QString& user, const QString& password, const bool& eHook, const SiteItem& site);

private:
	QString appDirectory;
	QString filePathE;

	QByteArray loadDecrypted() const;
	void SaveDecrypted(const QByteArray& plain) const;
};