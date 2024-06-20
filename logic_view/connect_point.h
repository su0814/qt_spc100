#ifndef CONNECT_POINT_H
#define CONNECT_POINT_H
#include "config/data_config.h"
#include "logic_view_config.h"
#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QObject>

typedef enum {
    POINT_BRUSH_IDLE = 0,
    POINT_BRUSH_DEBUG,
    POINT_BRUSH_INVALID,
} point_brush_state_e;

class connect_point : public QObject, public QGraphicsRectItem {
    Q_OBJECT
public:
    explicit connect_point(int x, int y, connect_point_iotype_e type, uint8_t id, attribute_t* attribute,
                           QGraphicsItem* parent = nullptr);
    ~connect_point();
    enum { Type = QGraphicsItem::UserType + BLOCK_TYPE_CONNECT };  // 自定义类型
    int type() const override
    {
        return Type;
    }

public:
    void                     position_change(void);
    void                     send_debug_data(uint8_t res);
    void                     set_focus(bool state);
    void                     connect_line_delete(void);
    void                     connect_line_creat();
    bool                     connect_is_created(void);
    bool                     allow_connect(void);
    uint16_t                 get_connect_num(void);
    connect_point_iotype_e   get_io_type(void);
    connect_point_datatype_e get_data_type(void);
    bool                     parents_coincide_detect(attribute_t* otherblock);
    void                     send_block_attribute(void);
    attribute_t              self_attribute;
    attribute_t              parent_attribute;
    int                      connect_point_id;
    void                     set_label(QString label);
    void                     set_id(int i);
    void                     set_attribute(attribute_t* attribute);
    void                     set_label_visible(bool state);
    void                     set_data_type(connect_point_datatype_e type);
    void                     set_enable(bool state);

private:
    void set_brush_state(point_brush_state_e state);

private:
    static const int         defaultWidth;
    static const int         defaultHeight;
    connect_point_iotype_e   io_type;
    connect_point_datatype_e data_type   = CONNECT_POINT_DATATYPE_NONE;
    uint16_t                 connect_num = 0;
    QGraphicsTextItem*       label_text;
    QGraphicsTextItem*       label_data_type;
    bool                     enabel = false;

signals:
    void item_deleted(void);
    void position_change_signal(void);
    void send_debug_data_signal(int res);
    void send_focus_state_signal(bool state);
    void send_block_attribute_signal(attribute_t* block_attribute);
    void connect_point_update(void);
public slots:
    void input_point_receive_info(attribute_t* block_attribute);
    void receive_debug_data_slot(uint8_t res);
};

#endif  // CONNECT_POINT_H
