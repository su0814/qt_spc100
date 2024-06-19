#ifndef BASE_RECT_CLASS_H
#define BASE_RECT_CLASS_H
#include "config/data_config.h"
#include "graphical_config/graphical_config_param.h"
#include "logic_view/connect_point.h"
#include "logic_view/logic_view_config.h"
#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QJsonObject>
#include <QMenu>
#include <QObject>
#define MAX_CONNECT_POINT_NUM (8)
#define MIN_CONNECT_POINT_NUM (0)
class MainWindow;
class base_rect_class : public QObject, public QGraphicsRectItem {
    Q_OBJECT
public:
    explicit base_rect_class(qreal x, qreal y, qreal w, qreal h, QWidget* uiparent, QGraphicsItem* parent = nullptr);
    ~base_rect_class();

protected:
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;

private:
    QGraphicsTextItem*   display_name;
    QGraphicsPixmapItem* display_pixmap;
    QGraphicsTextItem*   display_uid;
    bool                 focus_state = false;
    QMenu                menu;
    QAction*             setaction       = nullptr;
    QAction*             deleteaction    = nullptr;
    int                  right_menu_mode = ACTION_NONE;
    QPointF              old_pos;
    QPointF              new_pos;
    QJsonObject          old_param;

protected:
    block_base_param_t  block_base_param;
    QStringList         sys_input_point_label;
    QStringList         sys_output_point_label;
    attribute_t         attribute_data;
    MainWindow*         mainwindow = nullptr;
    QString             error_info = "";
    config_block_data_t config_block_data;
    QGraphicsRectItem*  temp_rect = nullptr;
    QPointF             pos_offset;
    bool                error = false;

protected:
    bool block_collison_detect(QRectF rect);
    bool block_collison_detect(QRectF rect, QList<QGraphicsItem*> selections);
    void set_pen_state(block_state_e state);
    void set_brush_state(block_state_e state);
    void set_display_name(QString name);
    void set_display_pixmap(QString name);
    void set_display_uid(int id);
    void set_sys_inputpoint_labels(QStringList labels);
    void set_connect_point_labels_visible(bool state);
    void set_right_menu_action(int mode);
    void error_detect(void);
    int  get_input_connected_num(void);
    int  get_output_connected_num(void);
    void send_attribute_data(void);
    void set_outputpoint_attribute(attribute_t* attribute, int id);
    void set_outputpoint_attribute(attribute_t* attribute);
    void set_inputpoint_attribute(attribute_t* attribute, int id);
    void set_inputpoint_attribute(attribute_t* attribute);
    void set_inputpoint_data_type(connect_point_datatype_e type, int id);
    void set_outputpoint_data_type(connect_point_datatype_e type, int id);
    /* 自定义实现 */
    virtual void config_block_data_update(void) {}
    virtual void action_set_callback(void) {}
    virtual void debug_data_parse(uint8_t res);
    virtual void block_project_prase(QJsonObject rootObject, bool copy = false, QPointF pos = QPoint(0, 0),
                                     uint32_t uid = 0)
    {
        Q_UNUSED(rootObject)
        Q_UNUSED(copy)
        Q_UNUSED(pos)
        Q_UNUSED(uid)
    }

public:
    virtual QJsonObject block_project_info(void)
    {
        QJsonObject rootObject;
        return rootObject;
    }
    virtual void        action_delete_callback(void) {}
    virtual QJsonObject block_param_info(void)
    {
        QJsonObject rootObject;
        return rootObject;
    }
    virtual void block_param_prase(QJsonObject rootObject)
    {
        Q_UNUSED(rootObject)
    }

public:
    QList<connect_point*> output_point_list;
    QList<connect_point*> input_point_list;
    bool                  collison_detect(QRectF rect);
    void                  movepos_start(QPoint movepos);
    void                  movepos_moving(QPoint movepos, QList<QGraphicsItem*> selections);
    bool                  movepos_iserror(QList<QGraphicsItem*> selections);
    void                  movepos_cancle();
    void                  movepos_end();
    void                  set_focus(bool state);
    void                  set_current_pos(QPointF pos);
    bool                  get_focus_state(void);
    QPointF               get_old_pos(void);
    QPointF               get_new_pos(void);
    bool                  error_is_exist(void);
    void                  debug_data_set(uint8_t res);
    attribute_t*          get_attribute(void);
    config_block_data_t*  get_config_block_data(void);
    QStringList           get_sys_inputpoint_labels(void);
    QStringList           get_sys_outputpoint_labels(void);
    QStringList           get_user_inputpoint_labels(void);
    QStringList           get_user_outputpoint_labels(void);
    int                   get_input_point_num();
    int                   get_output_point_num();
    int                   get_input_reverse_data(void);
    void                  set_user_inputpoint_labels(QStringList labels);
    void                  set_user_outputpoint_labels(QStringList labels);
    void                  set_input_reverse_data(int data);
    bool                  set_input_num(int num);
    bool                  set_output_num(int num);
    void                  set_sys_outputpoint_labels(QStringList labels);
    block_base_param_t    get_block_base_param(void);
    void                  send_param_change_signal(void);
    void                  set_block_old_param(QJsonObject param);
    QJsonObject           get_block_old_param(void);
signals:
    void block_delete_signal(void);
    void block_contexmenu_signal(QGraphicsItem* item);
    void block_param_change_signal(QGraphicsItem* item);
public slots:
};

#endif  // BASE_RECT_CLASS_H
