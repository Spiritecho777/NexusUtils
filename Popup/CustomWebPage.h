#pragma once

#include <QWebEnginePage>

class CustomWebPage : public QWebEnginePage
{
	Q_OBJECT
public:
	explicit CustomWebPage(QObject* parent = nullptr);

protected:
	QWebEnginePage* createWindow(WebWindowType type) override;
};
