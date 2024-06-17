#ifndef CONNECTION_LINE_H
#define CONNECTION_LINE_H

#include "config/data_config.h"
#include "logic_view/connect_point.h"
#include <QGraphicsLineItem>
#include <QMenu>
#include <QObject>
#define FIND_PATH_STEP (2)
typedef enum {
    DIRECTION_TRANSVERSE = 0,
    DIRECTION_TRANSVERSE_LEFT,
    DIRECTION_TRANSVERSE_RIGHT,
    DIRECTION_LONGITUDINAL,
    DIRECTION_LONGITUDINAL_UP,
    DIRECTION_LONGITUDINAL_DOWN,
    DIRECTION_IDLE,
} path_derection_e;

typedef struct {
    path_derection_e real_dir;
    bool             is_sucessful;
    QPointF          path_end_point;
    uint8_t          transverse_intersect_num;    //横向相交数量
    uint8_t          longitudinal_intersect_num;  //纵向相交数量
    bool             transversse_line1_is_none;   //横向线与纵向线是否无遮挡
    bool             transversse_line2_is_none;
    bool             longitudinal_line1_is_none;
    bool             longitudinal_line2_is_none;
} path_calc_t;
class MainWindow;
class connection_line : public QObject, public QGraphicsPathItem {
    Q_OBJECT
public:
    explicit connection_line(QWidget* uiparent, QGraphicsItem* parent = nullptr);
    ~connection_line();
    enum { Type = QGraphicsItem::UserType + BLOCK_TYPE_LINE };  // 自定义类型
    int type() const override
    {
        return Type;
    }
    MainWindow* mainwindow = nullptr;

public:
    void set_end_point_block(connect_point* endblock);
    void set_end_point(QPointF endpoint);
    void set_start_point_block(connect_point* startblock);
    /* project creat */
    QJsonObject connect_line_project_info(void);
    bool        connect_line_project_parse(QJsonObject project);
    /* info */
    connect_point* get_start_point();
    connect_point* get_end_point();

private:
    /* 路径计算 */
    /* 路径计算 */
    QPointF     probe_start_point;
    QPointF     probe_end_point;
    path_calc_t path_info;
    void        calc_path(void);
    void        rectangle_occlusion_calc(void);
    bool        check_line_intersects_rect(QLineF line);
    bool        check_point_intersects_rect(QPointF point);
    void        transverse_step_find_path(QPainterPath* path);
    void        longitudinal_step_find_path(QPainterPath* path);

private:
    connect_point* start_point_block = nullptr;
    connect_point* end_point_block   = nullptr;
    QPointF        start_point;
    QPointF        end_point;
    connect_point* send_block;
    connect_point* recv_block;
    connect_point* focus_block = nullptr;
    bool           focus_state = false;
    /* 自删 */
    QMenu    menu;
    QAction* deleteAction = nullptr;

protected:
    void         hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void         hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;
    void         contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;
    void         keyPressEvent(QKeyEvent* event) override;
    QPainterPath shape() const override
    {
        QPainterPathStroker stroker;
        stroker.setWidth(pen().widthF() + 2);  // 设置stroker的宽度为笔刷宽度
        return stroker.createStroke(path());
    }
signals:

public slots:
    void start_position_change_slot(void);
    void end_position_change_slot(void);
    void start_point_deleted_slot(void);
    void end_point_deleted_slot(void);
    void debug_data_prase_slot(bool res);
    void start_focus_slot(bool state);
    void end_focus_slot(bool state);
};

#endif  // CONNECTION_LINE_H
