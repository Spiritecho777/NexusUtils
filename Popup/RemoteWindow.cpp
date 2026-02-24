#include "RemoteWindow.h"
#include "CustomWebPage.h"
#include "../Classe/KeyboardHook.h"
#include "../Classe/SiteItem.h"

#include <QWebEngineView>
#include <QVBoxLayout>

RemoteWindow::RemoteWindow(const SiteItem& site)	: QWidget(nullptr)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowTitle(site.name);
	resize(1200, 720);

	m_view = new QWebEngineView(this);
	auto* page = new CustomWebPage(m_view);
	m_view->setPage(page);

	if (!site.url.isEmpty())
		m_view->setUrl(QUrl(site.url));

	auto* layout = new QVBoxLayout(this);
	layout->addWidget(m_view);
	setLayout(layout);

	m_isHooked = site.isHooked;
	if(m_isHooked) {
		KeyboardHook::startHook(this);
	}
}

RemoteWindow::~RemoteWindow() 
{
	if(m_isHooked) {
		KeyboardHook::stopHook(this);
	}
}


bool RemoteWindow::keyboardHookKeyDown(int vkCode, int msg) 
{
	// Handle key down events here
	// Return true if the event is handled and should not be passed to other applications
	return false;
}

bool RemoteWindow::keyboardHookKeyUp(int vkCode) 
{
	// Handle key up events here
	// Return true if the event is handled and should not be passed to other applications
	return false;
}