#pragma once
#include <QWidget>
#include <QListWidget>

#include "../Classe/SiteItem.h"
#include "../Classe/SiteManager.h"

class NexusWindow : public QWidget
{
	Q_OBJECT

public:
	explicit NexusWindow(QWidget *parent = nullptr);

private slots:
	void onAddSite();
	void onAbout();
	void onOpenSite(SiteItem site);
	void onAddCreds(SiteItem site);
	void onDeleteSite(SiteItem site);

private :
	QListWidget* listSite;
	SiteManager siteManager;

	void updateSiteList();
	void addSiteItem(const SiteItem &site);
};