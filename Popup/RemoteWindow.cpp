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
	auto* page = new CustomWebPage(site, m_view);
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

	m_isHooked = site.isHooked;
	
    qDebug() << "RemoteWindow: isHooked=" << m_isHooked;

	if(m_isHooked) {
		KeyboardHook::startHook(this);
	}

	connect(page, &CustomWebPage::windowCloseRequested, this, &RemoteWindow::close);

	connect(m_view, &QWebEngineView::loadFinished, this, [this](bool ok) {
		if (!ok) return;
		injectCredentials();

#ifdef WIN32
        //PATCH pour les touches                      ]={  }==  
        m_view->page()->runJavaScript(R"(
        (function() {
            window.addEventListener('keydown', function(e) {
                if (!e.ctrlKey || !e.altKey) return;
                var remap = {
                    'û': { key: '{', keyCode: 219, code: 'Minus'        }, 
                    '»': { key: '=', keyCode: 187, code: 'Equal'        },
                    'º': { key: '/', keyCode: 186, code: 'BracketRight' }
                };
                var mapped = remap[e.key];
                if (mapped) {
                    e.preventDefault();
                    e.stopImmediatePropagation();
                    setTimeout(function() {
                        document.activeElement.dispatchEvent(new KeyboardEvent('keydown', {
                            key:      mapped.key,
                            code:     mapped.code,
                            keyCode:  mapped.keyCode,
                            which:    mapped.keyCode,
                            ctrlKey:  true,
                            altKey:   true,
                            bubbles:  true,
                            cancelable: true
                        }));
                    },0);
                }
            }, true);
        })();
        )");
		//PATCH
#endif
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
#ifdef WIN32
    if (!m_pressedKeys.contains(vkCode))
        m_pressedKeys.append(vkCode);

    bool isAltGr = m_pressedKeys.contains(0xA5);
    bool isAlt = !isAltGr && (m_pressedKeys.contains(0xA4) || m_pressedKeys.contains(0x12));

    if (vkCode == 0x12 || vkCode == 0xA4 || vkCode == 0xA5)
        return true;

    if (vkCode == VK_LWIN || vkCode == VK_RWIN)
    {
        m_view->page()->runJavaScript(R"(
            var canvas = document.querySelector('canvas');
            if (canvas) canvas.focus();
        )");
        m_view->setFocus();
        QWidget* target = m_view->focusProxy() ? m_view->focusProxy() : m_view;
        target->setFocus();
        QKeyEvent event(QEvent::KeyPress, Qt::Key_Meta, Qt::MetaModifier, 0, vkCode, 0);
        QApplication::sendEvent(target, &event);
        return true;
    }

    if (isAltGr)
    {
        //DEBUG
        m_view->page()->runJavaScript(R"(
            if (!window._altgrDebug) {
                window._altgrDebug = true;
                window.addEventListener('keydown', function(e) {
                    console.log('[JS keydown] key=' + e.key + ' code=' + e.code + ' keyCode=' + e.keyCode + ' ctrlKey=' + e.ctrlKey + ' altKey=' + e.altKey);
                }, true);
            }
        )");
        //DEBUG

        m_view->page()->runJavaScript(R"(
            var canvas = document.querySelector('canvas');
            if (canvas) canvas.focus();
        )");
        m_view->setFocus();
        QWidget* target = m_view->focusProxy() ? m_view->focusProxy() : m_view;
        target->setFocus();

        QKeyEvent altGrPress(QEvent::KeyPress, Qt::Key_AltGr, Qt::AltModifier | Qt::ControlModifier, 0, 0xA5, 0, QString());
        QApplication::sendEvent(target, &altGrPress);

        QKeyEvent press(QEvent::KeyPress, Qt::Key(vkCode), Qt::AltModifier | Qt::ControlModifier, 0, vkCode, 0, QString());
        QApplication::sendEvent(target, &press);

		m_altGrKeys.insert(vkCode);
        return true;
    }

    if (isAlt)
    {
        m_view->page()->runJavaScript(R"(
            var canvas = document.querySelector('canvas');
            if (canvas) canvas.focus();
        )");

        m_view->setFocus();
        QWidget* target = m_view->focusProxy() ? m_view->focusProxy() : m_view;
        target->setFocus();

        QKeyEvent altPress(QEvent::KeyPress, Qt::Key_Alt, Qt::AltModifier, 0, 0xA4, 0, QString());
        QApplication::sendEvent(target, &altPress);

        QKeyEvent Press(QEvent::KeyPress, Qt::Key_Tab, Qt::AltModifier, 0, vkCode, 0, QString());
        QApplication::sendEvent(target, &Press);
            
        return true;
    }
#endif

#ifdef __linux__
    if (!m_pressedKeys.contains(vkCode))
		m_pressedKeys.append(vkCode);

	bool isAlt = m_pressedKeys.contains(64) || m_pressedKeys.contains(108);

    if (vkCode == 64 || vkCode == 108)
		return true;

    if (vkCode == 133 || vkCode == 134)
    {
        m_view->setFocus();
        QWidget* target = m_view->focusProxy() ? m_view->focusProxy() : m_view;
		target->setFocus();
        QKeyEvent event(QEvent::KeyPress, Qt::Key_Meta, Qt::MetaModifier, 0, vkCode, 0);
        QApplication::sendEvent(target, &event);
        return true;
    }

    if (isAlt)
    {
        m_view->setFocus();
        QWidget* target = m_view->focusProxy() ? m_view->focusProxy() : m_view;
        target->setFocus();

        QKeyEvent altPress(QEvent::KeyPress, Qt::Key_Alt, Qt::AltModifier, 0, 64, 0, QString());
        QApplication::sendEvent(target, &altPress);

        QKeyEvent Press(QEvent::KeyPress, Qt::Key_Tab, Qt::AltModifier, 0, vkCode, 0, QString());
        QApplication::sendEvent(target, &Press);

        return true;
    }
#endif

    return false;
}

bool RemoteWindow::keyboardHookKeyUp(int vkCode)
{
#ifdef WIN32
    m_pressedKeys.removeAll(vkCode);

    QWidget* target = m_view->focusProxy() ? m_view->focusProxy() : m_view;

    if (vkCode == VK_LWIN || vkCode == VK_RWIN)
    {
        QKeyEvent event(QEvent::KeyRelease, Qt::Key_Meta, Qt::MetaModifier, 0, vkCode, 0);
        QApplication::sendEvent(target, &event);
        return true;
    }

    if (vkCode == 0x12 || vkCode == 0xA4)
    {
        QKeyEvent event(QEvent::KeyRelease, Qt::Key_Alt, Qt::AltModifier, 0, vkCode, 0);
        QApplication::sendEvent(target, &event);
        return true;
    }

    if (m_altGrKeys.contains(vkCode))
    {
        m_altGrKeys.remove(vkCode);

        QWidget* target = m_view->focusProxy() ? m_view->focusProxy() : m_view;
        QKeyEvent release(QEvent::KeyRelease, Qt::Key(vkCode), Qt::AltModifier | Qt::ControlModifier, 0, vkCode, 0, QString());
        QApplication::sendEvent(target, &release);

        QKeyEvent altGrRelease(QEvent::KeyRelease, Qt::Key_AltGr, Qt::NoModifier, 0, 0xA5, 0, QString());
        QApplication::sendEvent(target, &altGrRelease);
        return true;
    }
#endif

#ifdef __linux__
    m_pressedKeys.removeAll(vkCode);

    QWidget* target = m_view->focusProxy() ? m_view->focusProxy() : m_view;

    if (vkCode == 133 || vkCode == 134)
    {
        QKeyEvent event(QEvent::KeyRelease, Qt::Key_Meta, Qt::MetaModifier, 0, vkCode, 0);
        QApplication::sendEvent(target, &event);
        return true;
    }

    if (vkCode == 64 || vkCode == 108)
    {
        QKeyEvent event(QEvent::KeyRelease, Qt::Key_Alt, Qt::AltModifier, 0, vkCode, 0);
        QApplication::sendEvent(target, &event);
        return true;
    }
#endif

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
