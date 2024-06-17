#ifndef BASE_LOGIC_BLOCK_H
#define BASE_LOGIC_BLOCK_H
#include "base_rect_class.h"
#include "config/data_config.h"
#include "logic_setting/and_or_logic_setting.h"
#include "logic_setting/ndecoder_logic_setting.h"
#include "logic_setting/nencode_logic_setting.h"
#include "logic_setting/not_xor_xnor_logic_setting.h"
#include "logic_setting/rs_trigger_logic_setting.h"
#include <QTimer>
class base_logic_block : public base_rect_class {
    Q_OBJECT
public:
    base_logic_block(QPointF pos, config_block_data_t data, uint32_t uid, QWidget* uiparent,
                     QGraphicsItem* parent = nullptr);
    base_logic_block(QJsonObject rootObject, QWidget* uiparent, QGraphicsItem* parent = nullptr);
    explicit base_logic_block(QPointF pos, uint32_t uid, QJsonObject rootObject, QWidget* uiparent,
                              QGraphicsItem* parent = nullptr);
    enum { Type = QGraphicsItem::UserType + BLOCK_TYPE_BASELOGIC };  // 自定义类型
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
    static const int            defaultWidth  = LOGIC_BLOCK_WIDTH;
    static const int            defaultHeight = LOGIC_BLOCK_HEIGHT;
    QTimer                      update_timer;
    and_or_logic_setting*       and_or_setting_dialog       = nullptr;
    not_xor_xnor_logic_setting* not_xor_xnor_setting_dialog = nullptr;
    rs_trigger_logic_setting*   rs_trigger_setting_dialog   = nullptr;
    nencode_logic_setting*      nencode_setting_dialog      = nullptr;
    ndecoder_logic_setting*     ndecoder_setting_dialog     = nullptr;

public:
    bool nencode_fault_output = false;

private:
    void self_init(void);
    void logic_function_update(void);
    void resource_setenable(bool isenable);
    void tooltip_update(void);
public slots:
    void update_state_slot(void);
};

#endif  // BASE_LOGIC_BLOCK_H
