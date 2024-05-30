#ifndef INPUT_BLOCK_H
#define INPUT_BLOCK_H
#include "base_rect_class.h"
#include "config/data_config.h"
#include "connect_point.h"
#include "graphical_config/graphical_config_param.h"
#include "logic_view_config.h"
#include <QGraphicsRectItem>
#include <QObject>
#include <QTimer>
class input_block : public base_rect_class {
    Q_OBJECT
public:
    explicit input_block(QPointF pos, config_block_data_t data, uint32_t uid, QWidget* uiparent,
                         QGraphicsItem* parent = nullptr);
    explicit input_block(QJsonObject rootObject, QWidget* uiparent, QGraphicsItem* parent = nullptr);
    enum { Type = QGraphicsItem::UserType + BLOCK_TYPE_INPUTBLOCK };  // 自定义类型
    int type() const override
    {
        return Type;
    }

protected:
    void config_block_data_update(void) override;
    void action_set_callback(void) override;
    void action_delete_callback(void) override;

public:
    QJsonObject block_project_info(void) override;

private:
    static const int      defaultWidth  = CONDITION_BLOCK_WIDTH;
    static const int      defaultHeight = CONDITION_BLOCK_HEIGHT;
    QTimer                update_timer;
    condition_right_set_t condition_right_set;

private:
    void self_init(void);
    void display_name_update(void);
    void logic_function_update(void);
    /* 右键菜单 */
    void ai_right_menu(void);
    void pi_right_menu(void);
    void qep_right_menu(void);
    void repeater_right_menu(void);
    void tooltip_update(void);
signals:
private slots:
    void update_state_slot(void);
};

#endif  // INPUT_BLOCK_H
