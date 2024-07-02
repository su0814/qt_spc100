#ifndef OUTPUT_BLOCK_H
#define OUTPUT_BLOCK_H

#include "base_rect_class.h"
#include "config/data_config.h"
#include "connect_point.h"
#include "logic_element.h"
#include "logic_view_config.h"
#include <QGraphicsRectItem>
#include <QObject>
#include <QTimer>

class output_block : public base_rect_class {
    Q_OBJECT
public:
    explicit output_block(QPointF pos, config_block_data_t data, uint32_t uid, QWidget* uiparent,
                          QGraphicsItem* parent = nullptr);
    explicit output_block(QJsonObject rootObject, QWidget* uiparent, QGraphicsItem* parent = nullptr);
    explicit output_block(QPointF pos, uint32_t uid, QJsonObject rootObject, QWidget* uiparent,
                          QGraphicsItem* parent = nullptr);
    ~output_block();
    enum { Type = QGraphicsItem::UserType + BLOCK_TYPE_OUTPUTBLOCK };  // 自定义类型
    int type() const override
    {
        return Type;
    }

private:
    static const int defaultWidth  = CONDITION_BLOCK_WIDTH;
    static const int defaultHeight = CONDITION_BLOCK_HEIGHT;
    QTimer           update_timer;

protected:
    /* 虚函数自定义实现 */
    void config_block_data_update(void) override;
    void action_delete_callback(void) override;
    void action_set_callback(void) override;
    void block_project_prase(QJsonObject rootObject, bool copy = false, QPointF pos = QPoint(0, 0),
                             uint32_t uid = 0) override;

public:
    QJsonObject block_project_info(void) override;
    QJsonObject block_param_info(void) override;
    void        block_param_prase(QJsonObject rootObject) override;

private:
    void self_init(void);
    void display_name_update(void);
    void resource_setenable(bool isenable);
    void logic_function_update(void);
    void tooltip_update(void);
    /* 右键菜单 */
    void repeater_right_menu(void);
signals:

public slots:
    void update_state_slot(void);
};

#endif  // OUTPUT_BLOCK_H
