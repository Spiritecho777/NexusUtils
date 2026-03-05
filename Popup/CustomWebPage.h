#pragma once
#include "../Classe/SiteItem.h"
#include <QWebEnginePage>

class CustomWebPage : public QWebEnginePage
{
	Q_OBJECT
public:
	explicit CustomWebPage(const SiteItem& site, QObject* parent = nullptr);

protected:
	QWebEnginePage* createWindow(WebWindowType type) override;

private:
	SiteItem m_site;
};
