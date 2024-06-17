#ifndef SPEED_LOGIC_BLOCK_H
#define SPEED_LOGIC_BLOCK_H
#include "base_rect_class.h"
#include "logic_setting/speed_cross_check_setting.h"
#include "logic_setting/speed_monitor_setting.h"
#include "logic_setting/speed_motionless_setting.h"
#include "logic_setting/speed_value_compairsons_setting.h"
#include <QTimer>
class speed_logic_block : public base_rect_class {
    Q_OBJECT
public:
    speed_logic_block(QPointF pos, config_block_data_t data, uint32_t uid, QWidget* uiparent,
                      QGraphicsItem* parent = nullptr);
    speed_logic_block(QJsonObject rootObject, QWidget* uiparent, QGraphicsItem* parent = nullptr);
    explicit speed_logic_block(QPointF pos, uint32_t uid, QJsonObject rootObject, QWidget* uiparent,
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

private:
    static const int                 defaultWidth  = LOGIC_BLOCK_WIDTH;
    static const int                 defaultHeight = LOGIC_BLOCK_HEIGHT;
    QTimer                           update_timer;
    speed_cross_check_setting*       cross_check_setting_dialog       = nullptr;
    speed_monitor_setting*           monitor_setting_dialog           = nullptr;
    speed_motionless_setting*        speed_motionless_setting_dialog  = nullptr;
    speed_value_compairsons_setting* value_compairsons_setting_dialog = nullptr;

public:
    /* 速度互检 */
    int  encoder_output_mode           = 0;
    int  crosscheck_percentage[2]      = { 0 };
    int  error_keep_time               = 0;
    bool encoder_reliability_monitor   = false;
    int  reliability_monitor_max_time  = 1;
    int  reliability_monitor_min_speed = 1;
    /* 减速监控 */
    int ramp_num          = 1;
    int ramp_delay_time   = 0;
    int ramp_time[4]      = { 1000, 1000, 1000, 1000 };
    int ramp_speed[4]     = { 200, 200, 200, 200 };
    int ramp_max_speed[4] = { 500, 500, 500, 500 };
    int ramp_min_speed[4] = { 100, 100, 100, 100 };
    /* 静止检测 */
    int motionless_speed    = 1;
    int motionless_min_time = 0;
    /* 速度数值比较 */
    int calc_mode   = 0;
    int speed_value = 0;
    int min_time    = 0;

private:
    void self_init(void);
    void logic_function_update(void);
    void resource_setenable(bool isenable);
    void tooltip_update(void);
public slots:
    void update_state_slot(void);
};

#endif  // SPEED_LOGIC_BLOCK_H
