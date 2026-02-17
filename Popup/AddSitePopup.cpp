#include "AddSitePopup.h"
#include <QVBoxLayout> 
#include <QHBoxLayout> 
#include <QLabel> 
#include <QMessageBox>

AddSitePopup::AddSitePopup(QWidget* parent) : QDialog(parent)
{
	setWindowTitle("Ajouter un site"); 
	setFixedSize(260, 150); 
	auto* layout = new QVBoxLayout(this); 
	
	// Champ Nom 
	auto *rowName = new QHBoxLayout(); 
	rowName->addWidget(new QLabel("Nom du site :")); 
	editName = new QLineEdit(this); 
	rowName->addWidget(editName); 
	layout->addLayout(rowName); 
	
	// Champ URL 
	auto *rowUrl = new QHBoxLayout(); 
	rowUrl->addWidget(new QLabel("URL du site :")); 
	editUrl = new QLineEdit(this); 
	rowUrl->addWidget(editUrl); 
	layout->addLayout(rowUrl); 
	
	// Boutons 
	auto *buttons = new QHBoxLayout(); 
	QPushButton *btnCancel = new QPushButton("Annuler");
	QPushButton *btnAdd = new QPushButton("Ajouter"); 
	connect(btnCancel, &QPushButton::clicked, this, &AddSitePopup::onCancel); 
	connect(btnAdd, &QPushButton::clicked, this, &AddSitePopup::onAdd); 
	buttons->addWidget(btnCancel); 
	buttons->addStretch(); 
	buttons->addWidget(btnAdd); 
	layout->addLayout(buttons);
}

void AddSitePopup::onAdd()
{
	QString name = editName->text().trimmed(); 
	QString url = editUrl->text().trimmed(); 

	if (name.isEmpty() || url.isEmpty()) 
	{
		QMessageBox::warning(this, "Erreur", "Les champs sont obligatoires."); 
		return; 
	} 
	
	SiteItem site; 
	site.name = name;
	site.url = url; 

	siteManager.addSite(site); 
	accept();
}

void AddSitePopup::onCancel()
{
	reject();
}