#include "usercode_dialog.h"
#include "ui_usercode_dialog.h"

Usercode_Dialog::Usercode_Dialog(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::Usercode_Dialog)
{
    ui->setupUi(this);
}

Usercode_Dialog::~Usercode_Dialog()
{
    delete ui;
}

void Usercode_Dialog::set_usercode(QString code)
{
    ui->usercode_textBrowser->setPlainText(code);
}
