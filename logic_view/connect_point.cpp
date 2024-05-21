#include "connect_point.h"
#include <QDebug>

const int connect_point::defaultWidth  = CONNECT_POINT_WIDTH;
const int connect_point::defaultHeight = CONNECT_POINT_HEIGHT;
connect_point::connect_point(int x, int y, connect_point_type_e type, uint8_t id, attribute_t* attribute,
                             QGraphicsItem* parent)
    : QGraphicsRectItem(0, 0, defaultWidth, defaultHeight, parent)
{
    setPos(x, y);
    QBrush brush(QColor(150, 150, 230));
    this->setBrush(brush);
    this->setCursor(Qt::ArrowCursor);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setAcceptedMouseButtons(Qt::LeftButton);
    connect_type     = type;
    self_attribute   = *attribute;
    connect_point_id = id;
    label_text       = new QGraphicsTextItem(this);
}

/**
 * @brief 连接点析构函数
 */
connect_point::~connect_point()
{
    emit item_deleted();
}

void connect_point::set_label(QString label)
{
    label_text->setPlainText(label);
    qreal width  = label_text->boundingRect().width();
    qreal height = label_text->boundingRect().height();
    if (connect_type == CONNECT_POINT_TYPE_OUTPUT) {
        label_text->setPos(CONNECT_POINT_WIDTH, -height / 2);
    } else {
        label_text->setPos(-width, -height / 2);
    }
}

void connect_point::set_id(int i)
{
    connect_point_id = i;
}

void connect_point::set_attribute(attribute_t* attribute)
{
    self_attribute = *attribute;
}

void connect_point::set_label_visible(bool state)
{
    label_text->setVisible(state);
}

/**
 * @brief 坐标位置变动函数
 */
void connect_point::position_change()
{
    emit position_change_signal();
}

/**
 * @brief 仿真数据发送函数，发送给连接的另一端
 * @param res 仿真数据
 */
void connect_point::send_debug_data(uint8_t res)
{
    if (res) {
        QBrush brush(QColor(0, 200, 0));
        this->setBrush(brush);
    } else {
        QBrush brush(QColor(0, 0, 0));
        this->setBrush(brush);
    }
    emit send_debug_data_signal(res);
}

void connect_point::set_focus(bool state)
{
    emit send_focus_state_signal(state);
}

/**
 * @brief 连接线删除回调函数
 */
void connect_point::connect_line_delete()
{
    if (connect_num > 0) {
        connect_num--;  //连接线数量
    }
}

/**
 * @brief 连接线创建回调函数
 */
void connect_point::connect_line_creat()
{
    connect_num++;
}

/**
 * @brief 连接点当前是否有连接线存在
 * @return
 */
bool connect_point::connect_is_created()
{
    if (connect_num > 0) {
        return true;
    }
    return false;
}

bool connect_point::allow_connect()
{
    if (connect_type == CONNECT_POINT_TYPE_INPUT && connect_num > 0) {
        return false;
    }
    return true;
}

/**
 * @brief 获取当前连接点的连接线的数量
 * @return 连接线数量
 */
uint16_t connect_point::get_connect_num()
{
    return connect_num;
}

/**
 * @brief 获取连接点的类型
 * @return 连接点类型
 */
connect_point_type_e connect_point::get_connect_type()
{
    return connect_type;
}

/**
 * @brief 父节点重复检测
 * @param otherblock 要检测的块的属性
 * @return 是否重复
 */
bool connect_point::parents_coincide_detect(attribute_t* otherblock)
{
    /* 看自身类型
     * 输入类型：检测对方父节点列表内是否有自己
     * 输出类型：检测自身父节点列表内是否有对方
     */
    QList<uint32_t> parent_id = otherblock->parent_id;
    uint32_t        probe_id  = self_attribute.uid;
    if (connect_type == CONNECT_POINT_TYPE_OUTPUT) {
        parent_id = self_attribute.parent_id;
        probe_id  = otherblock->uid;
    }
    return (parent_id.contains(probe_id));
}

/* user signal */
/**
 * @brief 发送父块的属性
 */
void connect_point::send_block_attribute(void)
{
    emit send_block_attribute_signal(&self_attribute);
}

/* user slots */
/**
 * @brief 接收其他块发来的属性信息
 * @param block_attribute 块属性信息
 */
void connect_point::input_point_receive_info(attribute_t* block_attribute)
{
    parent_attribute = *block_attribute;
}

/**
 * @brief 接收其他块发送来的仿真数据
 * @param res 仿真数据
 */
void connect_point::receive_debug_data_slot(uint8_t res)
{
    if (res) {
        QBrush brush(QColor(0, 255, 0));
        this->setBrush(brush);
    } else {
        QBrush brush(QColor(150, 150, 230));
        this->setBrush(brush);
    }
}