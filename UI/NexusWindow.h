#pragma once
#include <QWidget>
#include <QListWidget>

#include "../Classe/SiteItem.h"
#include "../Classe/SiteManager.h"
#include "BrowserWindow.h"

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
	void onMasterPassword();

private :
	QListWidget* listSite;
	SiteManager siteManager;
	BrowserWindow* browser = nullptr;

	void updateSiteList();
	void addSiteItem(const SiteItem &site);
};