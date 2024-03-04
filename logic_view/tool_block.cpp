#include "tool_block.h"
#include "QPainter"
#include <QApplication>
#include <QDebug>
#include <QScreen>
/**
 * @brief 图形化编程界面的可拖拽条件/逻辑的工具类的构造函数
 * @param iconName 图标
 * @param type 工具类型
 * @param id 类型ID
 * @param parent
 */
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
    connect(&drag_timer, &QTimer::timeout, this, &drag_moveover_detect_slot);
}

/**
 * @brief 设置工具显示字符
 * @param Name
 */
void tool_block::set_name(QString Name)
{
    setText(Name);
}

/* user slots */
/**
 * @brief 检测是否拖拽到了软件外，超出软件范围则拖拽取消
 */
void tool_block::drag_moveover_detect_slot()
{
    if (!window()->geometry().contains(QCursor::pos()) && drag != nullptr) {
        drag->cancel();
        drag_timer.stop();
    }
}

/* sys slots */
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
        drag_timer.start(100);
        drag->exec(Qt::MoveAction);
        drag_timer.stop();
    }
}
