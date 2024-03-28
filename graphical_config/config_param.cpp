#include "config_param.h"
#include "mainwindow.h"
#include <QBrush>
#include <QColor>
#include <QDebug>
#include <QFormLayout>
#include <QGraphicsSceneMouseEvent>
#include <QLayout>
config_param::config_param(int x, int y, int w, int h, config_tool_e type, QWidget* mparent, QGraphicsItem* parent)
    : QGraphicsRectItem(x, y, w, h, parent)
{
    ui         = MainWindow::my_ui->ui;
    mainwindow = ( MainWindow* )mparent;
    tool_type  = type;
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemIsFocusable, true);
    QBrush brush(QColor(128, 128, 128, 128));  //块填充色
    setBrush(brush);
    QPen pen(QColor(50, 50, 50));
    pen.setWidth(0);
    this->setPen(pen);
}

/**
 * @brief 获取当前配置快的类型
 * @return
 */
config_tool_e config_param::get_type()
{
    return tool_type;
}

/* sys event */
void config_param::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        mainwindow->safety_param_dialog_class->exec_from_id(tool_type);
    }
    QGraphicsRectItem::mousePressEvent(event);
}
