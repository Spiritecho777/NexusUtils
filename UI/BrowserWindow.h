#pragma once
#include <QWidget>
#include <QTabWidget>
#include "../Classe/SiteItem.h"

class BrowserWindow : public QWidget
{
	Q_OBJECT

public:
	explicit BrowserWindow(QWidget* parent = nullptr);

	void openSite(const SiteItem& site);

protected:
	void closeEvent(QCloseEvent* event) override;

private:
	QTabWidget* m_tabs;
};