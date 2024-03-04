#include "connect_block.h"
#include "config/param_config.h"
#include "connect_line.h"
#include <QBrush>
#include <QDebug>
#include <QDialog>
#include <QFormLayout>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QInputDialog>
const int connect_block::defaultWidth  = CONNECT_POINT_WIDTH;
const int connect_block::defaultHeight = CONNECT_POINT_HEIGHT;
/**
 * @brief 块上连接点的构造函数
 * @param x 坐标x
 * @param y 坐标y
 * @param type 连接点类型
 * @param id 连接点类型ID
 * @param attribute 父类属性指针
 * @param parent 父指针
 */
connect_block::connect_block(int x, int y, connect_point_type_e type, uint8_t id, block_attribute_t* attribute,
                             QGraphicsItem* parent)
    : QGraphicsRectItem(x, y, defaultWidth, defaultHeight, parent)
{
    // setPos(x, y);
    QBrush brush(QColor(0, 0, 0));
    this->setBrush(brush);
    this->setCursor(Qt::ArrowCursor);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setAcceptHoverEvents(true);
    setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton);
    this->setParentItem(parent);
    connect_type         = type;
    self_block_attribute = attribute;
    self_id              = id;
}

/**
 * @brief 连接点析构函数
 */
connect_block::~connect_block()
{
    emit item_deleted();
}

/**
 * @brief 坐标位置变动函数
 */
void connect_block::position_change()
{
    emit position_change_signal();
}

/**
 * @brief 仿真数据发送函数，发送给连接的另一端
 * @param res 仿真数据
 */
void connect_block::send_debug_data(bool res)
{
    if (res) {
        QBrush brush(QColor(0, 255, 0));
        this->setBrush(brush);
    } else {
        QBrush brush(QColor(0, 0, 0));
        this->setBrush(brush);
    }
    emit send_debug_data_signal(res);
}

/**
 * @brief 连接线删除回调函数
 */
void connect_block::connect_line_delete()
{
    if (connect_num > 0) {
        connect_num--;  //连接线数量
    }
}

/**
 * @brief 连接线创建回调函数
 */
void connect_block::connect_line_creat()
{
    connect_num++;
}

/**
 * @brief 连接点当前是否有连接线存在
 * @return
 */
bool connect_block::connect_is_created()
{
    if (connect_type == CONNECT_POINT_TYPE_INPUT && connect_num > 0) {
        return true;
    }
    return false;
}

/**
 * @brief 获取当前连接点的连接线的数量
 * @return 连接线数量
 */
uint16_t connect_block::get_connect_num()
{
    return connect_num;
}

/**
 * @brief 获取连接点的类型
 * @return 连接点类型
 */
connect_point_type_e connect_block::get_connect_type()
{
    return connect_type;
}

/**
 * @brief 父节点重复检测
 * @param otherblock 要检测的块的属性
 * @return 是否重复
 */
bool connect_block::parents_coincide_detect(block_attribute_t* otherblock)
{
    /* 看自身类型
     * 输入类型：检测对方父节点列表内是否有自己
     * 输出类型：检测自身父节点列表内是否有对方
     */
    QList<uint32_t> parent_id = otherblock->parent_id;
    uint32_t        probe_id  = self_block_attribute->self_id;
    if (connect_type == CONNECT_POINT_TYPE_OUTPUT) {
        parent_id = self_block_attribute->parent_id;
        probe_id  = otherblock->self_id;
    }
    return (parent_id.contains(probe_id));
}

/* user signal */
/**
 * @brief 发送父块的属性
 */
void connect_block::send_block_attribute(void)
{
    emit send_block_attribute_signal(self_block_attribute);
}

/* user slots */
/**
 * @brief 接收其他块发来的属性信息
 * @param block_attribute 块属性信息
 */
void connect_block::input_point_receive_info(block_attribute_t* block_attribute)
{
    parent_block_attribute = *block_attribute;
}

/**
 * @brief 接收其他块发送来的仿真数据
 * @param res 仿真数据
 */
void connect_block::receive_debug_data_slot(bool res)
{
    if (res) {
        QBrush brush(QColor(0, 255, 0));
        this->setBrush(brush);
    } else {
        QBrush brush(QColor(0, 0, 0));
        this->setBrush(brush);
    }
}
