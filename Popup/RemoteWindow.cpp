#include "RemoteWindow.h"
#include "CustomWebPage.h"
#include "../Classe/KeyboardHook.h"
#include "../Classe/SiteItem.h"

#include <QWebEngineView>
#include <QVBoxLayout>
#include <QDebug>
#include <QEvent>
#include <QKeyEvent>
#include <QApplication>
#include <QTimer>"

#ifdef WIN32
#include <windows.h>
#endif


RemoteWindow::RemoteWindow(const SiteItem& site) : QWidget(nullptr), m_site(site)
{
	setAttribute(Qt::WA_DeleteOnClose);

	setAttribute(Qt::WA_TranslucentBackground);
	setStyleSheet("background: transparent;");

	setWindowTitle(site.name);
	resize(1200, 720);

	m_view = new QWebEngineView(this);
	auto* page = new CustomWebPage(m_view);
	m_view->setPage(page);

	m_view->setStyleSheet("background: white;");

	if (!site.url.isEmpty())
		m_view->setUrl(QUrl(site.url));

	connect(m_view, &QWebEngineView::titleChanged, this, [this](const QString& title) {
		if (!title.trimmed().isEmpty()) 
			this->setWindowTitle(title);
		});

	auto* layout = new QVBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->addWidget(m_view);
	setLayout(layout);

	//m_isHooked = site.isHooked;
	m_isHooked = true; // forcer le hook pour tous les sites
    qDebug() << "etat HOOKED = " << m_isHooked;
	if(m_isHooked) {
		KeyboardHook::startHook(this);
	}

	connect(page, &CustomWebPage::windowCloseRequested, this, &RemoteWindow::close);

	connect(m_view, &QWebEngineView::loadFinished, this, [this](bool ok) {
		if (!ok) return;
		injectCredentials();
	});
}

RemoteWindow::~RemoteWindow() 
{
	if(m_isHooked) {
		KeyboardHook::stopHook(this);
	}
}


bool RemoteWindow::keyboardHookKeyDown(int vkCode, int msg)
{
	if (vkCode == VK_LWIN || vkCode == VK_RWIN)
	{
		// 1. Forcer le focus sur le canvas via JS
		m_view->page()->runJavaScript(R"(
            var canvas = document.querySelector('canvas');
            if (canvas) canvas.focus();
        )");

		// 2. S'assurer que la vue a le focus
		m_view->setFocus();
		QWidget* target = m_view->focusProxy() ? m_view->focusProxy() : m_view;
		target->setFocus();

		// 3. Injecter le vrai VK code natif
		QKeyEvent event(
			QEvent::KeyPress,
			Qt::Key_Meta,
			Qt::MetaModifier,
			0,
			vkCode,  // VK_LWIN = 0x5B
			0
		);
		QApplication::sendEvent(target, &event);
		return true;
	}
	return false;
}


bool RemoteWindow::keyboardHookKeyUp(int vkCode)
{	
	if (vkCode == VK_LWIN || vkCode == VK_RWIN)
	{
		QWidget* target = m_view->focusProxy() ? m_view->focusProxy() : m_view;

		QKeyEvent event(
			QEvent::KeyRelease,
			Qt::Key_Meta,
			Qt::MetaModifier,
			0,
			vkCode,
			0
		);
		QApplication::sendEvent(target, &event);
		return true;
	}
	return false;
}

void RemoteWindow::injectCredentials()
{
	if (m_site.username.isEmpty() && m_site.password.isEmpty())
		return;

	QString login = m_site.username;
	QString password = m_site.password;

	login.replace("'", "\\'");
	password.replace("'", "\\'");

	QString script = QString(R"(
        (function() {
            var loginInput = document.querySelector('input[name=usermail], input[id=username]');
            if (loginInput) {
                loginInput.value = '%1';
            }

            var passwordInput = document.querySelector('input[type=password], input[name=password], input[id=password]');
            if (passwordInput) {
                passwordInput.value = '%2';
            }
        })();
    )").arg(login, password);

	m_view->page()->runJavaScript(script);
}
