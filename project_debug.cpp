#include "project_debug.h"
#include "mainwindow.h"
project_debug::project_debug(QWidget* parent)
    : QWidget(parent)
{
    ui         = MainWindow::my_ui->ui;
    mainwindow = ( MainWindow* )parent;
}
