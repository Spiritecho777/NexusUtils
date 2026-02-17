#pragma once
#include <QWidget>
#include <QListWidget>

#include "../Classe/SiteItem.h"
#include "../Classe/SiteManager.h"
#include "BrowserWindow.h"

class Window : public QWidget
{
	Q_OBJECT

public:
	explicit Window(QWidget *parent = nullptr);

private slots:
	void onAddSite();
	void onAbout();
	void onOpenSite(SiteItem site);
	void onAddCreds(SiteItem site);
	void onDeleteSite(SiteItem site);

private :
	QListWidget* listSite;
	SiteManager siteManager;
	BrowserWindow* browserWindow = nullptr;

	void updateSiteList();
	void addSiteItem(const SiteItem &site);
};