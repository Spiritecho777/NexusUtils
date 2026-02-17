#include "CredSitePopup.h"
#include <QVBoxLayout> 
#include <QHBoxLayout> 
#include <QLabel> 
#include <QMessageBox>

CredSitePopup::CredSitePopup(const SiteItem& site, QWidget* parent) : QDialog(parent), site(site)
{
	setWindowTitle("Ajouter des identifiants"); 
	setFixedSize(260, 150); 
	auto* layout = new QVBoxLayout(this); 

	// Identifiant 
	auto *rowId = new QHBoxLayout(); 
	rowId->addWidget(new QLabel("Identifiant :")); 
	editUser = new QLineEdit(this); 
	rowId->addWidget(editUser); 
	layout->addLayout(rowId); 
	
	// Mot de passe 
	auto *rowPwd = new QHBoxLayout(); 
	rowPwd->addWidget(new QLabel("Mot de passe :")); 
	editPassword = new QLineEdit(this); 
	editPassword->setEchoMode(QLineEdit::Password); 
	rowPwd->addWidget(editPassword); 
	layout->addLayout(rowPwd); 
	
	// Boutons 
	auto *buttons = new QHBoxLayout(); 
	QPushButton *btnCancel = new QPushButton("Annuler"); 
	QPushButton *btnAdd = new QPushButton("Ajouter"); 

	connect(btnCancel, &QPushButton::clicked, this, &CredSitePopup::onCancel); 
	connect(btnAdd, &QPushButton::clicked, this, &CredSitePopup::onAdd); 

	buttons->addWidget(btnCancel); 
	buttons->addStretch(); 
	buttons->addWidget(btnAdd); 

	layout->addLayout(buttons);
}

void CredSitePopup::onAdd() {
	QString identifiant = editUser->text();
	QString password = editPassword->text();

	site.username = identifiant;
	site.password = password;

	siteManager.addCredsSite(site);

	accept();
}

void CredSitePopup::onCancel() {
	reject();
}