#include "SiteItem.h"
#include "SiteManager.h"


#pragma Traitement de données
bool SiteManager::fileExists() const
{
	return false;
}

QVector<SiteItem> SiteManager::getAllSites() const
{
	return {};
}
#pragma	endregion

#pragma Manipulation de données
void SiteManager::addSite(const SiteItem& site)
{
}

void SiteManager::addCredsSite(const SiteItem& site)
{
}

void SiteManager::deleteSite(const SiteItem& site)
{
}
#pragma endregion

#pragma Sécurité
void SiteManager::Chiffrement(const QString& data, const QString& key)
{
}
#pragma endregion
