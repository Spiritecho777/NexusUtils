#pragma once
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>

#include "../Classe/SiteItem.h"
#include "../Classe/SiteManager.h"

class CredSitePopup : public QDialog
{
public:
	Q_OBJECT

public:
	explicit CredSitePopup(const SiteItem& site, QWidget* parent = nullptr);

private slots:
	void onAdd();
	void onCancel();

private:
	SiteItem site;
	SiteManager siteManager;

	QLineEdit* editUser;
	QLineEdit* editPassword;
	QCheckBox* checkHook = nullptr;
	QLabel* labelWrn = nullptr;
};