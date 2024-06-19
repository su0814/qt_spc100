#ifndef APPLY_LOGIC_BLOCK_H
#define APPLY_LOGIC_BLOCK_H
#include "base_rect_class.h"
#include "logic_setting/apply_edge_detected_setting.h"
#include "logic_setting/apply_extern_device_monitors_setting.h"
#include "logic_setting/apply_freq_monitor_setting.h"
#include "logic_setting/apply_reset_setting.h"
#include <QTimer>
class apply_logic_block : public base_rect_class {
    Q_OBJECT
public:
    apply_logic_block(QPointF pos, config_block_data_t data, uint32_t uid, QWidget* uiparent,
                      QGraphicsItem* parent = nullptr);
    apply_logic_block(QJsonObject rootObject, QWidget* uiparent, QGraphicsItem* parent = nullptr);
    explicit apply_logic_block(QPointF pos, uint32_t uid, QJsonObject rootObject, QWidget* uiparent,
                               QGraphicsItem* parent = nullptr);
    enum { Type = QGraphicsItem::UserType + BLOCK_TYPE_APPLYLOGIC };  // 自定义类型
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
    static const int                      defaultWidth  = LOGIC_BLOCK_WIDTH;
    static const int                      defaultHeight = LOGIC_BLOCK_HEIGHT;
    QTimer                                update_timer;
    apply_reset_setting*                  reset_setting_dialog                  = nullptr;
    apply_extern_device_monitors_setting* extern_device_monitors_setting_dialog = nullptr;
    apply_edge_detected_setting*          edge_detected_setting_dialog          = nullptr;
    apply_freq_monitor_setting*           freq_monitor_setting_dialog           = nullptr;

public:
    apply_reset_param_t        apply_reset_param;
    apply_edm_param_t          apply_edm_param;
    apply_edge_detect_param_t  apply_edge_detect_param;
    apply_freq_monitor_param_t apply_freq_monitor_param;

private:
    void self_init(void);
    void logic_function_update(void);
    void resource_setenable(bool isenable);
    void tooltip_update(void);
public slots:
    void update_state_slot(void);
};

#endif  // APPLY_LOGIC_BLOCK_H
