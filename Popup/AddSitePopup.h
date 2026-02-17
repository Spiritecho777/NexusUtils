#pragma once
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>

#include "../Classe/SiteManager.h"

class AddSitePopup : public QDialog
{
public:
	Q_OBJECT

public:
	explicit AddSitePopup(QWidget* parent = nullptr);

private slots :
	void onAdd();
	void onCancel();

private:
	QLineEdit* editName;
	QLineEdit* editUrl;
	
	SiteManager siteManager;
};