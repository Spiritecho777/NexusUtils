#pragma once
#include <QVector>
#include "SiteItem.h"

class SiteManager
{
public:
	bool fileExists() const;
	QVector<SiteItem> getAllSites() const;
	void deleteSite(const SiteItem& site);
	void addSite(const SiteItem& site);
	void addCredsSite(const SiteItem& site);

private:
	void Chiffrement(const QString& data, const QString& key);
};