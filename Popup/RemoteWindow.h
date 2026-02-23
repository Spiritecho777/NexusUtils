#pragma once

#include <QWidget>
#include <QWebEngineView>

class RemoteWindow : public QWidget
{
    Q_OBJECT

public:
    explicit RemoteWindow(const QString& url, QWidget* parent = nullptr);

    bool keyboardHookKeyDown(int vkCode, int msg);
    bool keyboardHookKeyUp(int vkCode);

private:
    QWebEngineView* view;
};
