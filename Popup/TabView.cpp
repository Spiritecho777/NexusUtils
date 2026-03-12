#include "TabView.h"
#include "../Popup/CustomWebPage.h"
#include <QVBoxLayout>

TabView::TabView(const SiteItem& site, QWidget* parent)
    : QWidget(parent), m_site(site)
{
    m_view = new QWebEngineView(this);
    auto* page = new CustomWebPage(site, m_view);
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

    static const QStringList loginSelectors = {
        "input[name=usermail]",
        "input[name=login]",
        "input[name=email]",
        "input[name=Email]",
        "input[name=username]",
        "input[id=username]",
        "input[id=login_form]",
        "input[id=login_email]",
        "input[id=email-field]",
        "input[data-mgmtautomationid=username]",
    };

    static const QStringList passwordSelectors = {
        "input[type=password]",
        "input[name=password]",
        "input[id=password]",
    };

    QString loginSel = loginSelectors.join(", ");
    QString passwordSel = passwordSelectors.join(", ");

    QString script = QString(R"(
        (function retryFill() {
            let loginInput = document.querySelector('%1');
            let passwordInput = document.querySelector('%2');

            if (loginInput) loginInput.value = '%3';
            if (passwordInput) passwordInput.value = '%4';

            if (!loginInput || !passwordInput)
                setTimeout(retryFill, 300);
        })();
    )").arg(loginSel, passwordSel, login, password);

    m_view->page()->runJavaScript(script);
}