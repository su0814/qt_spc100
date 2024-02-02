#include "about_prajna.h"
#include "mainwindow.h"
#include <QCoreApplication>
#include <QDebug>
#include <QDesktopServices>
#include <QDialog>
#include <QTemporaryFile>
about_prajna::about_prajna(QWidget* mparent, QWidget* parent)
    : QWidget(parent)
{
    ui         = MainWindow::my_ui->ui;
    mainwindow = ( MainWindow* )mparent;
    connect(ui->action_about_prajna, &QAction::triggered, this, about_prajnasafe_message_slot);
    connect(ui->action_usermanual, &QAction::triggered, this, about_usermanual_file_slot);
    connect(ui->action_configtool_manual, &QAction::triggered, this, about_configtool_manual_file_slot);
}

/* user slots */
void about_prajna::about_prajnasafe_message_slot()
{
    QDialog dialog;
    dialog.setWindowTitle("关于若彗");
    QLabel* image_label = new QLabel();
    QPixmap pixmap(":/new/photo/photo/about_prajna.jpg");
    image_label->setPixmap(pixmap);
    dialog.setMinimumSize(pixmap.size());
    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(image_label);
    dialog.setLayout(layout);
    dialog.exec();
}

void about_prajna::about_configtool_manual_file_slot()
{
    QString cur_path = QCoreApplication::applicationDirPath() + "/SPC-ESW-015 Configure tool manual.pdf";
    QFile   efile(cur_path);
    if (!efile.exists()) {
        QFile resourceFile(":/doc/doc/SPC-ESW-015 Configure tool manual.pdf");
        if (resourceFile.open(QIODevice::ReadOnly)) {
            QFile saveFile(cur_path);
            if (saveFile.open(QIODevice::WriteOnly)) {
                saveFile.write(resourceFile.readAll());
                saveFile.close();
            }
            resourceFile.close();
        }
    }
    QDesktopServices::openUrl(QUrl::fromLocalFile(cur_path));
}

void about_prajna::about_usermanual_file_slot()
{
    QString cur_path = QCoreApplication::applicationDirPath() + "/SPC-PP-001 User Manual.pdf";
    QFile   resourceFile(":/doc/doc/SPC-PP-001 User Manual.pdf");
    QFile   efile(cur_path);
    if (!efile.exists()) {
        if (resourceFile.open(QIODevice::ReadOnly)) {
            QFile saveFile(cur_path);
            if (saveFile.open(QIODevice::WriteOnly)) {
                saveFile.write(resourceFile.readAll());
                saveFile.close();
            }
            resourceFile.close();
        }
    }
    QDesktopServices::openUrl(QUrl::fromLocalFile(cur_path));
}
