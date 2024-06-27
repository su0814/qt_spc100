#include "project_start_dialog.h"
#include "mainwindow.h"
#include "ui_project_start_dialog.h"
project_start_dialog::project_start_dialog(QWidget* mparent, QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::project_start_dialog)
{
    ui->setupUi(this);
    mainwindow = ( MainWindow* )mparent;
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

project_start_dialog::~project_start_dialog()
{
    delete ui;
}

void project_start_dialog::on_pushButton_new_pro_clicked()
{
    mainwindow->project_management_class->project_new_slot();
    close();
}

void project_start_dialog::on_pushButton_import_pro_clicked()
{
    if (mainwindow->project_management_class->project_import_slot() == 0) {
        close();
    }
}

void project_start_dialog::on_pushButton_skip_clicked()
{
    close();
}
