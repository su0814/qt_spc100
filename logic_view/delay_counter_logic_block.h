#ifndef DELAY_COUNTER_LOGIC_BLOCK_H
#define DELAY_COUNTER_LOGIC_BLOCK_H
#include "base_rect_class.h"
#include "logic_setting/delay_adjust_on_off_setting.h"
#include "logic_setting/delay_on_off_setting.h"
#include <QTimer>
class delay_counter_logic_block : public base_rect_class {
    Q_OBJECT
public:
    delay_counter_logic_block(QPointF pos, config_block_data_t data, uint32_t uid, QWidget* uiparent,
                              QGraphicsItem* parent = nullptr);
    delay_counter_logic_block(QJsonObject rootObject, QWidget* uiparent, QGraphicsItem* parent = nullptr);
    enum { Type = QGraphicsItem::UserType + BLOCK_TYPE_DELAY_COUNTER };  // 自定义类型
    int type() const override
    {
        return Type;
    }

protected:
    void action_set_callback(void) override;
    void action_delete_callback(void) override;
    void debug_data_parse(uint8_t res) override;

public:
    QJsonObject block_project_info(void) override;

private:
    static const int             defaultWidth  = LOGIC_BLOCK_WIDTH;
    static const int             defaultHeight = LOGIC_BLOCK_HEIGHT;
    QTimer                       update_timer;
    delay_on_off_setting*        on_off_setting_dialog        = nullptr;
    delay_adjust_on_off_setting* adjust_on_off_setting_dialog = nullptr;

public:
    int on_off_delay_time           = 0;
    int adjust_on_off_delay_time[4] = { 0 };

private:
    void self_init(void);
    void logic_function_update(void);
    void resource_setenable(bool isenable);
    void tooltip_update(void);
public slots:
    void update_state_slot(void);
};

#endif  // DELAY_COUNTER_LOGIC_BLOCK_H
