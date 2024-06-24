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
    enum { Type = QGraphicsItem::UserType + BLOCK_TYPE_SPEED };  // 自定义类型
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
    static const int                 defaultWidth  = LOGIC_BLOCK_WIDTH;
    static const int                 defaultHeight = LOGIC_BLOCK_HEIGHT;
    QTimer                           update_timer;
    speed_cross_check_setting*       cross_check_setting_dialog       = nullptr;
    speed_monitor_setting*           monitor_setting_dialog           = nullptr;
    speed_motionless_setting*        speed_motionless_setting_dialog  = nullptr;
    speed_value_compairsons_setting* value_compairsons_setting_dialog = nullptr;

public:
    /* 速度互检 */
    speed_crosscheck_param_t speed_crosscheck_param;
    /* 减速监控 */
    speed_decelerate_monitor_param_t speed_decelerate_monitor_param;
    /* 静止检测 */
    speed_motionless_monitor_param_t speed_motionless_monitor_param;
    /* 速度数值比较 */
    speed_value_compairsons_param_t speed_value_compairsons_param;

private:
    void self_init(void);
    void logic_function_update(void);
    void resource_setenable(bool isenable);
    void tooltip_update(void);
public slots:
    void update_state_slot(void);
};

#endif  // SPEED_LOGIC_BLOCK_H
