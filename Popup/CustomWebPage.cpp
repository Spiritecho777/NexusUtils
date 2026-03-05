#include "CustomWebPage.h"
#include "RemoteWindow.h"
#include "../Classe/SiteItem.h"
#include <QWebEngineSettings>
#include <QDebug>

CustomWebPage::CustomWebPage(const SiteItem& site, QObject* parent) : QWebEnginePage(parent), m_site(site)
{
	QWebEngineSettings* s = settings();

	s->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
	s->setAttribute(QWebEngineSettings::JavascriptCanOpenWindows, true);
	s->setAttribute(QWebEngineSettings::AllowWindowActivationFromJavaScript, true);
	s->setAttribute(QWebEngineSettings::JavascriptCanAccessClipboard, true); 
	s->setAttribute(QWebEngineSettings::JavascriptCanPaste, true);
	s->setAttribute(QWebEngineSettings::AllowRunningInsecureContent, true);
	s->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
}

QWebEnginePage* CustomWebPage::createWindow(WebWindowType type)
{
	auto* popup = new RemoteWindow(m_site);
	popup->show();

	return popup->view()->page();
}