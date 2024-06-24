#include "config_menu.h"
#include "mainwindow.h"
#include <QDebug>
#include <QDrag>
#include <QMimeData>

config_menu::config_menu(QWidget* uparent, QWidget* parent)
    : QTreeWidget(parent)
{

    mainwindow = ( MainWindow* )uparent;
    load_menu();
    //设置可拖拽性质
    this->setDragDropMode(QAbstractItemView::DragDrop);
    setDragDropMode(QAbstractItemView::DragDrop);
    setAcceptDrops(true);
    this->setHeaderHidden(true);
    setIndentation(14);
    this->setStyleSheet("QTreeWidget::branch:closed:has-children {image: "
                        "url(:/new/photo/photo/menu_close.png);}QTreeWidget::branch:open:has-children {image: "
                        "url(:/new/photo/photo/menu_open.png);}"
                        "QTreeWidget { font-size: 12px; }");
    setIconSize(QSize(50, 50));
    setItemDelegate(new CustomItemDelegate);
    expandAll();
}

void config_menu::load_menu()
{
    QFile inputFile("://configphoto/config_menu.json");
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
            config_element* parentItem;
            if (!item) {
                parentItem = new config_element(jsonObject, this);
            } else {
                parentItem = new config_element(jsonObject, item);
            }
            parentItem->setText(0, key.remove(0, 1));
            json_menu_parse(jsonObject, parentItem);
        }
    }
}

void config_menu::startDrag(Qt::DropActions supportedActions)
{
    Q_UNUSED(supportedActions);
    QTreeWidgetItem* item = currentItem();
    if (item) {
        config_element* element = dynamic_cast<config_element*>(item);
        if (element->config_data()->type == KEY_TYPE_MENU) {
            return;
        }
        QDrag*           drag     = new QDrag(this);
        QMimeData*       mimeData = new QMimeData;
        element_data_t*  s_data   = element->config_data();
        element_data_t** data     = &s_data;
        QByteArray       byteArray(reinterpret_cast<const char*>(data), sizeof(element_data_t*));
        mimeData->setData("ruohui/config", byteArray);
        drag->setMimeData(mimeData);
        QPixmap pixmap(200, 20);
        pixmap.fill(Qt::transparent);
        QPainter painter(&pixmap);
        painter.drawPixmap(0, 0, element->icon(0).pixmap(20, 20));                             // 绘制图标
        painter.drawText(25, 0, 175, 20, Qt::AlignLeft | Qt::AlignVCenter, element->text(0));  // 设置文本位置和内容
        painter.end();
        drag->setPixmap(pixmap);
        drag->exec(Qt::MoveAction);
    }
}

void config_menu::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasFormat("ruohui/config")) {
        event->accept();
    }
}

void config_menu::dragMoveEvent(QDragMoveEvent* event)
{
    if (event->mimeData()->hasFormat("ruohui/config")) {
        event->accept();
    }
}

void config_menu::dragLeaveEvent(QDragLeaveEvent* event)
{
    event->accept();
}

void config_menu::dropEvent(QDropEvent* event)
{
    if (event->mimeData()->hasFormat("ruohui/config")) {
        event->accept();
    }
}
