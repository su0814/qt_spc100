#ifndef CONNECT_POINT_H
#define CONNECT_POINT_H
#include "config/data_config.h"
#include "logic_view_config.h"
#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QObject>

class connect_point : public QObject, public QGraphicsRectItem {
    Q_OBJECT
public:
    explicit connect_point(int x, int y, connect_point_type_e type, uint8_t id, attribute_t* attribute,
                           QGraphicsItem* parent = nullptr);
    ~connect_point();
    enum { Type = QGraphicsItem::UserType + BLOCK_TYPE_CONNECT };  // 自定义类型
    int type() const override
    {
        return Type;
    }

public:
    void                 position_change(void);
    void                 send_debug_data(uint8_t res);
    void                 set_focus(bool state);
    void                 connect_line_delete(void);
    void                 connect_line_creat();
    bool                 connect_is_created(void);
    bool                 allow_connect(void);
    uint16_t             get_connect_num(void);
    connect_point_type_e get_connect_type(void);
    bool                 parents_coincide_detect(attribute_t* otherblock);
    void                 send_block_attribute(void);
    attribute_t          self_attribute;
    attribute_t          parent_attribute;
    int                  connect_point_id;
    void                 set_label(QString label);
    void                 set_id(int i);
    void                 set_attribute(attribute_t* attribute);
    void                 set_label_visible(bool state);

private:
    static const int     defaultWidth;
    static const int     defaultHeight;
    connect_point_type_e connect_type;
    uint16_t             connect_num = 0;
    QGraphicsTextItem*   label_text;

signals:
    void item_deleted(void);
    void position_change_signal(void);
    void send_debug_data_signal(int res);
    void send_focus_state_signal(bool state);
    void send_block_attribute_signal(attribute_t* block_attribute);
public slots:
    void input_point_receive_info(attribute_t* block_attribute);
    void receive_debug_data_slot(uint8_t res);
};

#endif  // CONNECT_POINT_H
