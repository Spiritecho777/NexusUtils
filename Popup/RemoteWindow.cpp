#include "RemoteWindow.h"
#include "CustomWebPage.h"
#include "../Classe/KeyboardHook.h"
#include "../Classe/SiteItem.h"

#include <QWebEngineView>
#include <QVBoxLayout>
#include <QDebug>

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
    //qDebug() << "KeyDown vkCode =" << vkCode; // fait crasher linux

    return false;
}


bool RemoteWindow::keyboardHookKeyUp(int vkCode)
{
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
