#pragma once

#include <QWidget>
#include <QWebEngineView>
#include <QList>

#include "../Classe/SiteItem.h"
#include "../Classe/KeyboardHook.h"

class RemoteWindow : public QWidget
{
    Q_OBJECT

public:
    explicit RemoteWindow(const SiteItem& site);
	~RemoteWindow();

    bool keyboardHookKeyDown(int vkCode, int msg);
    bool keyboardHookKeyUp(int vkCode);

	bool isHooked() const { return m_isHooked; }
	QWebEngineView* view() const { return m_view; }

private:
    QWebEngineView* m_view = nullptr;
    bool m_isHooked;
	QList<int> m_pressedKeys;
    SiteItem m_site;
	void injectCredentials();
};
