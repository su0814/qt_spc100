#include "tool_block.h"
#include "QPainter"
#include <QApplication>
#include <QDebug>
#include <QScreen>
tool_block::tool_block(QString iconName, tool_type_e type, tool_id_e id, QWidget* parent)
    : QToolButton(parent)
{
    QSizePolicy sizePolicy = this->sizePolicy();
    sizePolicy.setHorizontalPolicy(QSizePolicy::Preferred);
    sizePolicy.setVerticalPolicy(QSizePolicy::Preferred);
    setSizePolicy(sizePolicy);
    setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    tool_info.tool_type = type;
    tool_info.tool_id   = id;
    setIcon(QIcon(iconName));
    /* 由于拖拽是阻塞式的  所以需要定时器定时监测是否拖拽到了软件之外 */
    drag_timer = new QTimer(this);
    connect(drag_timer, &QTimer::timeout, this, &drag_moveover_detect_slot);
}

void tool_block::mousePressEvent(QMouseEvent* event)
{
    if ((event->buttons() & Qt::LeftButton)) {
        Q_UNUSED(event);
        /* 设置拖拽属性 */
        drag                = new QDrag(this);
        QMimeData* mimeData = new QMimeData;
        QByteArray byteArray(reinterpret_cast<const char*>(&tool_info), sizeof(tool_info));
        mimeData->setData("ruohui/tool", byteArray);
        drag->setMimeData(mimeData);
        QPixmap pixmap = this->icon().pixmap(100, 100);
        drag->setPixmap(pixmap);                                            // 设置拖拽时的图标
        drag->setHotSpot(QPoint(pixmap.width() / 2, pixmap.height() / 2));  // 设置拖放的热点位置
        drag_timer->start(100);
        drag->exec(Qt::MoveAction);
        drag_timer->stop();
    }
}
void tool_block::set_name(QString Name)
{
    setText(Name);
}

/* user slots */
void tool_block::drag_moveover_detect_slot()
{

    if (!window()->geometry().contains(QCursor::pos()) && drag != nullptr) {
        drag->cancel();
        drag_timer->stop();
    }
}
