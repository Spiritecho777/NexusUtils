#include "MasterPasswordPopup.h"

#include <QVBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>

MasterPasswordPopup::MasterPasswordPopup(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("Mot de passe maître");

    auto* layout = new QVBoxLayout(this);

    edit = new QLineEdit(this);          // ← PAS local, membre de classe
    edit->setEchoMode(QLineEdit::Password);

    auto* btn = new QPushButton("Valider", this);

    labelWrn = new QLabel("Attention : Ceci va réinitialiser le fichier de données", this);
    labelWrn->setStyleSheet("color: red;");
    layout->addWidget(labelWrn);

    layout->addWidget(new QLabel("Entrez le mot de passe maître :", this));
    layout->addWidget(edit);
    layout->addWidget(btn);

    connect(btn, &QPushButton::clicked, this, [this]() {
        accept();
        });
}

QString MasterPasswordPopup::password() const
{
    return edit->text();   // ← FINI les pwd->text() qui n'existent pas
}
