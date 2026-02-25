#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QWidget>
#include <openssl/rand.h>
#include <openssl/evp.h>

#include "NexusWindow.h"
#include "../Classe/SiteItem.h"
#include "../Popup/AddSitePopup.h"
#include "../Popup/CredSitePopup.h"
#include "../Popup/RemoteWindow.h"
#include "../Popup/MasterPasswordPopup.h"
#include "../Classe/CryptoUtils.h"


NexusWindow::NexusWindow(QWidget* parent) : QWidget(parent)
{
	this->setWindowTitle("Nexus Utils");
	this->resize(450, 265);

	auto* layout = new QVBoxLayout(this);

	auto* topBar = new QHBoxLayout();
	QPushButton* btnAdd = new QPushButton("Ajouter un site", this);
	QPushButton* btnMasterPassword = new QPushButton("🔒", this);
	QPushButton* btnAbout = new QPushButton("?", this);

	connect(btnAdd, &QPushButton::clicked, this, &NexusWindow::onAddSite);
	connect(btnMasterPassword, &QPushButton::clicked, this, &NexusWindow::onMasterPassword);
	connect(btnAbout, &QPushButton::clicked, this, &NexusWindow::onAbout);

	topBar->addWidget(btnAdd);
	topBar->addStretch();
	topBar->addWidget(btnMasterPassword);
	topBar->addWidget(btnAbout);

	layout->addLayout(topBar);

	listSite = new QListWidget(this);
	listSite->setFixedHeight(205);
	layout->addWidget(listSite);

	updateSiteList();
}

void NexusWindow::updateSiteList()
{
	listSite->clear();
	if (!siteManager.fileExists())
		return;

	for (const SiteItem& site : siteManager.getAllSites())
		addSiteItem(site);
}

void NexusWindow::addSiteItem(const SiteItem &site)
{
	QListWidgetItem* item = new QListWidgetItem(listSite);

	QWidget* row = new QWidget();
	auto* h = new QHBoxLayout(row);
	h->setContentsMargins(5, 5, 5, 5);

	QPushButton* btnOpen = new QPushButton(site.name);
	btnOpen->setFixedWidth(150);

#ifdef __linux__
	QPushButton* btnDelete = new QPushButton();
	btnDelete->setIcon(style()->standardIcon(QStyle::SP_TrashIcon));

	QPushButton* btnAddCreds = new QPushButton();
	btnAddCreds->setIcon(style()->standardIcon(QStyle::SP_FileDialogDetailedView));
#elif WIN32
	QPushButton* btnAddCreds = new QPushButton("🔑");
	QPushButton* btnDelete = new QPushButton("🗑️");
#endif

	h->addWidget(btnOpen);
	h->addWidget(btnAddCreds);
	h->addWidget(btnDelete);

	item->setSizeHint(row->sizeHint());
	listSite->setItemWidget(item, row);

	connect(btnOpen, &QPushButton::clicked, this, [this, site]() { onOpenSite(site); });
	connect(btnAddCreds, &QPushButton::clicked, this, [this, site]() { onAddCreds(site); });
	connect(btnDelete, &QPushButton::clicked, this, [this, site]() { onDeleteSite(site); });

}

void NexusWindow::onAddSite()
{
	AddSitePopup dlg(this);
	if (dlg.exec() == QDialog::Accepted)
		updateSiteList();	
}

void NexusWindow::onMasterPassword()
{
	QByteArray stored = CryptoUtils::loadMasterKey();

	MasterPasswordPopup dlg(this);

	if (dlg.exec() != QDialog::Accepted)
		return;

	QString pwd = dlg.password();

	// Désactivation → retour GUID
	if (pwd.isEmpty())
	{
		CryptoUtils::saveMasterKey({}, {});
		QMessageBox::information(this, "Info", "Mot de passe maître désactivé.");
		return;
	}

	// Activation / changement
	QByteArray salt(16, 0);
	RAND_bytes(reinterpret_cast<unsigned char*>(salt.data()), salt.size());

	QByteArray hash(32, 0);

	PKCS5_PBKDF2_HMAC(
		pwd.toUtf8().constData(), pwd.size(),
		reinterpret_cast<const unsigned char*>(salt.data()), salt.size(),
		100000,
		EVP_sha256(),
		32,
		reinterpret_cast<unsigned char*>(hash.data())
	);

	CryptoUtils::saveMasterKey(hash, salt);

	QMessageBox::information(this, "Info", "Mot de passe maître enregistré.");
}


void NexusWindow::onAbout()
{
	QString version = APP_VERSION;
	QMessageBox::about(this, "A propos", "NexusUtils version:" APP_VERSION);
}

void NexusWindow::onOpenSite(SiteItem site)
{
	if (!browser)
	{
		browser = new BrowserWindow;
		browser->show();

		connect(browser, &BrowserWindow::destroyed, this, [this]() {
			browser = nullptr;
		});
	}

	browser->openSite(site);
}

void NexusWindow::onAddCreds(SiteItem site)
{
	CredSitePopup dlg(site, this);
	dlg.exec();
	updateSiteList();
}

void NexusWindow::onDeleteSite(SiteItem site)
{
	siteManager.deleteSite(site);
	updateSiteList();
}