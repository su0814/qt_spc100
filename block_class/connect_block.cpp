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

connect_block::~connect_block()
{
    emit item_deleted();
}

void connect_block::position_change()
{
    emit position_change_signal();
}

void connect_block::connect_line_delete()
{
    if (connect_num > 0) {
        connect_num--;
    }
}

void connect_block::connect_line_creat()
{
    connect_num++;
}

bool connect_block::connect_is_created()
{
    if (connect_type == CONNECT_POINT_TYPE_INPUT && connect_num > 0) {
        return true;
    }
    return false;
}

uint16_t connect_block::get_connect_num()
{
    return connect_num;
}

connect_point_type_e connect_block::get_connect_type()
{
    return connect_type;
}

bool connect_block::parents_coincide_detect(QList<uint32_t> other_parent)
{
    for (int i = 0; i < self_block_attribute->parent_id.count(); i++) {
        for (int j = 0; j < other_parent.count(); j++) {
            if (self_block_attribute->parent_id[i] == other_parent[j]) {
                return true;
            }
        }
    }
    return false;
}

/* user signal */
void connect_block::send_block_attribute(void)
{
    emit send_block_attribute_signal(self_block_attribute);
}

/* user slots */
void connect_block::input_point_receive_info(block_attribute_t* block_attribute)
{
    memcpy(( char* )&parent_block_attribute, ( char* )block_attribute, sizeof(block_attribute_t));
}
