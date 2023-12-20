#ifndef CONNECT_BLOCK_H
#define CONNECT_BLOCK_H

//#include "connect_line.h"
#include "config/data_config.h"
#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QObject>
#include <qgraphicsitem.h>

typedef enum {
    CONNECT_STATE_IDLE = 0,
    CONNECT_STATE_LINED,
} connect_state_e;

class connect_line;
class connect_block : public QObject, public QGraphicsRectItem {
    Q_OBJECT
public:
    connect_block(int x, int y, connect_point_type_e type, uint8_t id, block_attribute_t* attribute,
                  QGraphicsItem* parent);
    virtual ~connect_block();
    enum { Type = QGraphicsItem::UserType + BLOCK_TYPE_CONNECT };  // 自定义类型
    int type() const override
    {
        return Type;
    }

protected:
    //    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    //    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    //    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

public:
    block_attribute_t* self_block_attribute;
    block_attribute_t  parent_block_attribute;
    uint8_t            self_id;

public:
    void                 position_change(void);
    void                 connect_line_delete(void);
    void                 connect_line_creat();
    bool                 connect_is_created(void);
    connect_point_type_e get_connect_type(void);
    void                 send_block_attribute(void);
    bool                 parents_coincide_detect(QList<uint32_t> other_parent);
    uint16_t             get_connect_num(void);

private:
    static const int     defaultWidth;
    static const int     defaultHeight;
    connect_point_type_e connect_type;
    uint16_t             connect_num = 0;
signals:
    void position_change_signal(void);
    void item_deleted(void);
    void send_block_attribute_signal(block_attribute_t* block_attribute);
public slots:
    void input_point_receive_info(block_attribute_t* block_attribute);
};
#endif  // CONNECT_BLOCK_H
