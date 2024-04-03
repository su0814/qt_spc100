#include "config_menu.h"
#include "mainwindow.h"
#include <QDebug>

config_menu::config_menu(QWidget* parent)
{

    mainwindow = ( MainWindow* )parent;
    // mainwindow->ui->config_menu_treeWidget->setColumnCount(4);  //列数
    QHeaderView* header = mainwindow->ui->config_menu_treeWidget->header();
    header->setSectionResizeMode(QHeaderView::ResizeToContents);
    load_menu();
    mainwindow->ui->config_menu_treeWidget->setVisible(false);
}

void config_menu::load_menu()
{
    QFile inputFile("./config_menu.json");
    if (inputFile.open(QIODevice::ReadOnly)) {
        QByteArray jsonData = inputFile.readAll();
        inputFile.close();
        QJsonDocument jsonDoc    = QJsonDocument::fromJson(jsonData);
        QJsonObject   jsonObject = jsonDoc.object();
        json_menu_parse(jsonObject, nullptr);
    } else {
        mainwindow->my_message_box("获取不到菜单信息！", MESSAGE_TYPE_ERROR);
        mainwindow->close();
    }
}

void config_menu::json_menu_parse(QJsonObject object, QTreeWidgetItem* item)
{
    QStringList keys = object.keys();
    foreach (QString key, keys) {
        QJsonObject jsonObject = object[key].toObject();
        if (!jsonObject.isEmpty()) {
            QTreeWidgetItem* parentItem;
            if (!item) {
                parentItem = new QTreeWidgetItem(mainwindow->ui->config_menu_treeWidget);
            } else {
                parentItem = new QTreeWidgetItem(item);
            }
            parentItem->setText(0, key);
            int type = jsonObject["type"].toInt();
            switch (type) {
            case KEY_TYPE_MENU:
                break;
            case KEY_TYPE_ELEMENT:
                parentItem->setIcon(0, QIcon(":/new/photo/photo/logo.png"));
                break;
            }
            json_menu_parse(jsonObject, parentItem);
        }
    }
}
