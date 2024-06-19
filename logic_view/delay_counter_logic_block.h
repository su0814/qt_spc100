#ifndef DELAY_COUNTER_LOGIC_BLOCK_H
#define DELAY_COUNTER_LOGIC_BLOCK_H
#include "base_rect_class.h"
#include "logic_setting/counter_event_setting.h"
#include "logic_setting/counter_logging_setting.h"
#include "logic_setting/delay_adjust_on_off_setting.h"
#include "logic_setting/delay_on_off_setting.h"
#include <QTimer>

class delay_counter_logic_block : public base_rect_class {
    Q_OBJECT
public:
    delay_counter_logic_block(QPointF pos, config_block_data_t data, uint32_t uid, QWidget* uiparent,
                              QGraphicsItem* parent = nullptr);
    delay_counter_logic_block(QJsonObject rootObject, QWidget* uiparent, QGraphicsItem* parent = nullptr);
    explicit delay_counter_logic_block(QPointF pos, uint32_t uid, QJsonObject rootObject, QWidget* uiparent,
                                       QGraphicsItem* parent = nullptr);
    enum { Type = QGraphicsItem::UserType + BLOCK_TYPE_DELAY_COUNTER };  // 自定义类型
    int type() const override
    {
        return Type;
    }

protected:
    void action_set_callback(void) override;
    void action_delete_callback(void) override;
    void debug_data_parse(uint8_t res) override;
    void block_project_prase(QJsonObject rootObject, bool copy = false, QPointF pos = QPoint(0, 0),
                             uint32_t uid = 0) override;

public:
    QJsonObject block_project_info(void) override;
    QJsonObject block_param_info(void) override;
    void        block_param_prase(QJsonObject rootObject) override;

private:
    static const int             defaultWidth  = LOGIC_BLOCK_WIDTH;
    static const int             defaultHeight = LOGIC_BLOCK_HEIGHT;
    QTimer                       update_timer;
    delay_on_off_setting*        on_off_setting_dialog          = nullptr;
    delay_adjust_on_off_setting* adjust_on_off_setting_dialog   = nullptr;
    counter_event_setting*       counter_event_setting_dialog   = nullptr;
    counter_logging_setting*     counter_logging_setting_dialog = nullptr;

public:
    delay_on_off_param_t        delay_on_off_param;
    delay_adjust_on_off_param_t delay_adjust_on_off_param;
    counter_event_param_t       counter_event_param;
    counter_logging_param_t     counter_logging_param;

private:
    void self_init(void);
    void logic_function_update(void);
    void resource_setenable(bool isenable);
    void tooltip_update(void);
public slots:
    void update_state_slot(void);
};

#endif  // DELAY_COUNTER_LOGIC_BLOCK_H
