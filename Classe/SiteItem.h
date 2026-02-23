#pragma once
#include <QString>

struct SiteItem
{
	QString name;
	QString url;

	QString username;
	QString password;

	bool isHooked = false;
};