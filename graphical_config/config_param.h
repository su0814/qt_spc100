#ifndef CONFIG_PARAM_H
#define CONFIG_PARAM_H
#include "config/data_config.h"
#include "graphical_config_param.h"
#include "safety_param_dialog.h"
#include "ui_mainwindow.h"
#include <QDialog>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsTextItem>
#include <QKeyEvent>
#include <QMetaType>
#include <QObject>
#define LABEL_WIDTH        (120)
#define VALUE_WIDTH        (60)
#define LABEL_WIDTH_OFFSET (20)
typedef enum {
    BRUSH_STATE_NORMAL = 0,
    BRUSH_STATE_DISPLAY,
    BRUSH_STATE_DATA_TRUE,
} brush_state_e;

class MainWindow;
class config_param : public QObject, public QGraphicsRectItem {
    Q_OBJECT
public:
    config_param(int x, int y, int w, int h, QWidget* mparent, QGraphicsItem* parent = nullptr);
    MainWindow* mainwindow = nullptr;
    /********** 变量 **********/
public:
    void                set_block_data(config_param_data_t data);
    config_block_data_t get_block_data(void);
    bool                display_detect(element_data_t config_data);
    void                display_hide();
    bool                bounding_range_detect(QPointF pos);
    void                drap_data_receive(element_data_t data);
    void                set_module_value(int value);
    QJsonObject         config_param_project_info(void);
    bool                config_param_project_parse(QJsonObject rootObject);
    void                data_reset(void);

private:
    config_block_data_t  config_block_data;
    config_user_data_t   config_user_data;
    element_data_t       element_data;
    QGraphicsRectItem*   label_rect;
    QGraphicsTextItem*   label_text;
    QGraphicsRectItem*   value_rect;
    QGraphicsTextItem*   value_text;
    QGraphicsPixmapItem* value_pixmap;
    void                 drap_data_parse(element_data_t data);
    void                 set_name(QString name);
    void                 set_brush_state(brush_state_e state);
    /********** 事件 **********/
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    bool sceneEventFilter(QGraphicsItem* watched, QEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

signals:
    void cat3_model_sync_drap_signal(element_data_t data);
    void cat3_model_sync_reset_signal(void);
    void cat3_model_sync_name_signal(QString name);
    void add_item_from_config_signal(config_block_data_t data);
    void remove_item_from_config_signal(config_block_data_t data);
    void name_change_signal(config_block_data_t data);
public slots:
    void cat3_model_sync_drap_slot(element_data_t data);
    void cat3_model_sync_reset_slot(void);
    void cat3_model_sync_name_slot(QString name);
};

#endif  // CONFIG_PARAM_H
