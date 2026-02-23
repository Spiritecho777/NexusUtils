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

	// Checbox Hook
	checkHook = new QCheckBox("Activer le hook clavier", this);
	layout->addWidget(checkHook);
	
	// Boutons 
	auto *buttons = new QHBoxLayout(); 
	QPushButton *btnCancel = new QPushButton("Annuler"); 
	QPushButton *btnAdd = new QPushButton("Ajouter"); 

	connect(btnCancel, &QPushButton::clicked, this, &CredSitePopup::onCancel); 
	connect(btnAdd, &QPushButton::clicked, this, &CredSitePopup::onAdd); 

	buttons->addWidget(btnAdd); 
	buttons->addStretch(); 
	buttons->addWidget(btnCancel); 

	layout->addLayout(buttons);

	editUser->setText(site.username);
	editPassword->setText(site.password);
	checkHook->setChecked(site.isHooked);
}

void CredSitePopup::onAdd() {
	QString identifiant = editUser->text();
	QString password = editPassword->text();
	bool eHook = checkHook->isChecked();

	site.username = identifiant;
	site.password = password;

	siteManager.addCredsSite(identifiant,password,eHook,site);

	accept();
}

void CredSitePopup::onCancel() {
	reject();
}