#ifndef CONFIG_PHOTO_H
#define CONFIG_PHOTO_H
#include "config/data_config.h"
#include "config_device_fault.h"
#include "config_param.h"
#include "device/status.h"
#include "graphical_config_param.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QJsonObject>
#include <QtSvg>
#include <QtWidgets>

class MainWindow;
class config_photo : public QGraphicsSvgItem {
public:
    config_photo(const QString& filePath, QWidget* uiparent);
    Ui::MainWindow* ui         = nullptr;
    MainWindow*     mainwindow = nullptr;

public:
    void display_available_source(element_data_t config_data);
    void hide_available_source(void);
    void drap_available_source(element_data_t config_data, QPointF pos);
    void config_param_creat_update(module_param_t* param, config_block_data_t* data);
    void config_param_delete_update(module_param_t* param, config_block_data_t* data);
    /* 安全参数 */
    void module_param_update(module_param_t* param);
    /* 数据信息 */
    void a_set_base_data(module_state_t data);
    void b_set_base_data(module_state_t data);
    void set_fault_info(module_fault_t a_fault_info, module_fault_t b_fault_info);
    /* 工程信息 */
    void        config_photo_reset(void);
    QJsonObject config_photo_project_info(void);
    bool        config_photo_project_parse(QJsonObject rootObject);

private:
    void init(QWidget* uiparent);
    void resource_led_init(void);
    void set_led_color(QGraphicsRectItem* item, const QColor& color);

public:
    /* config */
    QList<config_param*> a_ai_config_list;
    QList<config_param*> b_ai_config_list;
    QList<config_param*> a_di_config_list;
    QList<config_param*> b_di_config_list;
    QList<config_param*> a_relay_config_list;
    QList<config_param*> b_relay_config_list;
    QList<config_param*> a_pi_config_list;
    QList<config_param*> b_pi_config_list;
    QList<config_param*> mos_config_list;
    QList<config_param*> qep_config_list;
    QList<config_param*> all_config_list;
    /* led */
    QList<QGraphicsRectItem*> a_ai_led_list;
    QList<QGraphicsRectItem*> b_ai_led_list;
    QList<QGraphicsRectItem*> a_relay_led_list;
    QList<QGraphicsRectItem*> b_relay_led_list;
    QList<QGraphicsRectItem*> mos_led_list;
    QGraphicsRectItem*        a_error_led              = nullptr;
    QGraphicsRectItem*        b_error_led              = nullptr;
    config_device_fault*      config_device_fault_rect = nullptr;

protected:
    void hoverMoveEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;
};

#endif  // CONFIG_PHOTO_H
