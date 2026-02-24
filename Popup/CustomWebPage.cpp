#include "CustomWebPage.h"
#include "RemoteWindow.h"
#include "../Classe/SiteItem.h"
#include <QWebEngineSettings>
#include <QDebug>

CustomWebPage::CustomWebPage(QObject* parent) : QWebEnginePage(parent)
{
	QWebEngineSettings* s = settings();

	s->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
	s->setAttribute(QWebEngineSettings::JavascriptCanOpenWindows, true);
	s->setAttribute(QWebEngineSettings::AllowWindowActivationFromJavaScript, true);
}

QWebEnginePage* CustomWebPage::createWindow(WebWindowType type)
{
	SiteItem popupSite;
	popupSite.name = "Popup";
	popupSite.url = "";
	popupSite.isHooked = false;

	auto* popup = new RemoteWindow(popupSite);
	popup->show();

	return popup->view()->page();
}