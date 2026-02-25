#include "TabView.h"
#include "../Popup/CustomWebPage.h"
#include <QVBoxLayout>

TabView::TabView(const SiteItem& site, QWidget* parent)
    : QWidget(parent), m_site(site)
{
    m_view = new QWebEngineView(this);
    auto* page = new CustomWebPage(m_view);
    m_view->setPage(page);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_view);
    setLayout(layout);

    if (!site.url.isEmpty())
        m_view->setUrl(QUrl(site.url));

    // Injection des credentials
    connect(m_view, &QWebEngineView::loadFinished, this, [this](bool ok) {
        if (!ok) return;
        injectCredentials();
        });
}

void TabView::injectCredentials()
{
    if (m_site.username.isEmpty() && m_site.password.isEmpty())
        return;

    QString login = m_site.username;
    QString password = m_site.password;

    login.replace("'", "\\'");
    password.replace("'", "\\'");

    QString script = QString(R"(
        (function retryFill() {
            let loginInput = document.querySelector('input[name=usermail], input[id=username]');
            let passwordInput = document.querySelector('input[type=password], input[name=password], input[id=password]');

            if (loginInput) loginInput.value = '%1';
            if (passwordInput) passwordInput.value = '%2';

            if (!loginInput || !passwordInput)
                setTimeout(retryFill, 300);
        })();
    )").arg(login, password);

    m_view->page()->runJavaScript(script);
}
