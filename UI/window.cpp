#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QWidget>

#include "window.h"
#include "../Classe/SiteItem.h"
#include "../Popup/AddSitePopup.h"
#include "../Popup/CredSitePopup.h"

Window::Window(QWidget* parent) : QWidget(parent)
{
	this->setWindowTitle("Nexus Utils");
	this->resize(450, 265);

	auto* layout = new QVBoxLayout(this);

	auto* topBar = new QHBoxLayout();
	QPushButton* btnAdd = new QPushButton("Ajouter un site", this);
	QPushButton* btnAbout = new QPushButton("?", this);

	connect(btnAdd, &QPushButton::clicked, this, &Window::onAddSite);
	connect(btnAbout, &QPushButton::clicked, this, &Window::onAbout);

	topBar->addWidget(btnAdd);
	topBar->addStretch();
	topBar->addWidget(btnAbout);

	layout->addLayout(topBar);

	listSite = new QListWidget(this);
	listSite->setFixedHeight(205);
	layout->addWidget(listSite);

	updateSiteList();
}

void Window::updateSiteList()
{
	listSite->clear();
	if (!siteManager.fileExists())
		return;

	for (const SiteItem& site : siteManager.getAllSites())
		addSiteItem(site);
}

void Window::addSiteItem(const SiteItem &site)
{
	QListWidgetItem* item = new QListWidgetItem(listSite);

	QWidget* row = new QWidget();
	auto* h = new QHBoxLayout(row);
	h->setContentsMargins(5, 5, 5, 5);

	QPushButton* btnOpen = new QPushButton(site.name);
	btnOpen->setFixedWidth(150);

	QPushButton* btnAddCreds = new QPushButton("🔑");
	QPushButton* btnDelete = new QPushButton("🗑️");

	h->addWidget(btnOpen);
	h->addWidget(btnAddCreds);
	h->addWidget(btnDelete);

	item->setSizeHint(row->sizeHint());
	listSite->setItemWidget(item, row);

	connect(btnOpen, &QPushButton::clicked, this, [this, site]() { onOpenSite(site); });
	connect(btnAddCreds, &QPushButton::clicked, this, [this, site]() { onAddCreds(site); });
	connect(btnDelete, &QPushButton::clicked, this, [this, site]() { onDeleteSite(site); });

}

void Window::onAddSite()
{
	AddSitePopup dlg(this);
	if (dlg.exec() == QDialog::Accepted)
		updateSiteList();	
}

void Window::onAbout()
{
	QMessageBox::about(this, "A propos", "Nexus Utils v1.0");
}

void Window::onOpenSite(SiteItem site)
{
	
}

void Window::onAddCreds(SiteItem site)
{
	CredSitePopup dlg(site, this);
	dlg.exec();
	updateSiteList();
}

void Window::onDeleteSite(SiteItem site)
{
	siteManager.deleteSite(site);
	updateSiteList();
}