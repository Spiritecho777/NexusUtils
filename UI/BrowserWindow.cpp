#include "BrowserWindow.h"
#include "../Popup/CustomWebPage.h"
#include "../Popup/TabView.h"
#include <QVBoxLayout>
#include <QWebEngineView>
#include <QCloseEvent>

BrowserWindow::BrowserWindow(QWidget* parent)
    : QWidget(parent)
{
	setAttribute(Qt::WA_DeleteOnClose);

    setWindowTitle("Nexus Browser");
    resize(1200, 720);

    m_tabs = new QTabWidget(this);
    m_tabs->setTabsClosable(true);
    m_tabs->setMovable(true);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_tabs);
    setLayout(layout);

    connect(m_tabs, &QTabWidget::tabCloseRequested, this, [this](int index) {
        QWidget* w = m_tabs->widget(index);
        m_tabs->removeTab(index);
        w->deleteLater();

        if (m_tabs->count() == 0)
            this->close();
        });
}

void BrowserWindow::openSite(const SiteItem& site)
{
    auto* tab = new TabView(site); 
    
    int index = m_tabs->addTab(tab, site.name); 
    m_tabs->setCurrentIndex(index); 
    
    // Titre dynamique 
    connect(tab->view(), &QWebEngineView::titleChanged, this, [this, tab](const QString& title) 
        { 
            int idx = m_tabs->indexOf(tab); 
            if (idx >= 0) m_tabs->setTabText(idx, title); 
        });
}

void BrowserWindow::closeEvent(QCloseEvent* event)
{
    // Ferme tous les onglets
    while (m_tabs->count() > 0) {
        QWidget* w = m_tabs->widget(0);
		m_tabs->removeTab(0);
        w->deleteLater();
    }
    m_tabs->clear();
    event->accept();
}
