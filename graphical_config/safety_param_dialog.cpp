#include "safety_param_dialog.h"
#include "ui_safety_param_dialog.h"

Safety_Param_Dialog::Safety_Param_Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Safety_Param_Dialog)
{
    ui->setupUi(this);
}

Safety_Param_Dialog::~Safety_Param_Dialog()
{
    delete ui;
}
