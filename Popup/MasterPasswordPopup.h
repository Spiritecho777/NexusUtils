#pragma once

#include <QDialog>
#include <QString>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

class MasterPasswordPopup : public QDialog
{
	Q_OBJECT

public:
	explicit MasterPasswordPopup(QWidget* parent = nullptr);
	
	QString password() const;

private:
	QLineEdit* edit;
	QLabel* labelWrn;
};