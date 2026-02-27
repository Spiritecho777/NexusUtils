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
    QList<int> m_pressedKeys;

    SiteItem m_site;

    bool m_isHooked;
    bool m_winPressed = false;

	void injectCredentials();
};
