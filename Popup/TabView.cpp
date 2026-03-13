#include "TabView.h"
#include "../Popup/CustomWebPage.h"
#include <QVBoxLayout>
#include <QTimer>

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

        QTimer::singleShot(800, this, [this]() {
            injectCredentials();
        });
    });
}

void TabView::injectCredentials()
{
    if (m_site.username.isEmpty() && m_site.password.isEmpty())
        return;

    QString login = m_site.username;
    QString password = m_site.password;

    login.replace("\\", "\\\\");
    login.replace("'", "\\'");
    login.replace("\n", "\\n");
    login.replace("\r", "\\r");

    password.replace("\\", "\\\\");
    password.replace("'", "\\'");
    password.replace("\n", "\\n");
    password.replace("\r", "\\r");

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
        (function retryFill(attempts) {
            if (attempts <= 0) return;
            
            var login    = '%1';
            var password = '%2';

            let loginInput = document.querySelector('%3');
            let passwordInput = document.querySelector('%4');

            if (loginInput) loginInput.value = login;
            if (passwordInput) passwordInput.value = password;

            if (!loginInput || !passwordInput)
                setTimeout(function() { retryFill(attempts - 1); }, 500);
        })(20);
    )").arg(login, password, loginSel, passwordSel);

    m_view->page()->runJavaScript(script);
}