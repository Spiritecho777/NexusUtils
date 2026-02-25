#pragma once

#include <QWidget>
#include <QWebEngineView>
#include "../Classe/SiteItem.h"

class TabView : public QWidget
{
    Q_OBJECT

public:
    explicit TabView(const SiteItem& site, QWidget* parent = nullptr);

    QWebEngineView* view() const { return m_view; }

private:
    void injectCredentials();

    QWebEngineView* m_view = nullptr;
    SiteItem m_site;
};
