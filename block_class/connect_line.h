#ifndef CONNECT_LINE_H
#define CONNECT_LINE_H

//#include "connect_block.h"

#include "logic_block.h"
#include <QGraphicsLineItem>
#include <QGraphicsSceneHoverEvent>
#include <QJsonObject>
#include <QObject>
#include <QPen>
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

class connect_block;
class connect_line : public QObject, public QGraphicsPathItem {
    Q_OBJECT
public:
    explicit connect_line(QGraphicsItem* parent = nullptr);
    virtual ~connect_line();
    enum { Type = QGraphicsItem::UserType + BLOCK_TYPE_LINE };  // 自定义类型
    int type() const override
    {
        return Type;
    }

public:
    void set_end_point_block(connect_block* endblock);
    void set_end_point(QPointF endpoint);
    void set_start_point_block(connect_block* startblock);
    /* 路径计算 */
    void calc_path(void);
    /* project creat */
    QJsonObject connect_line_project_info(void);
    bool        connect_line_project_parse(QJsonObject project);

private:
    /* 路径计算 */
    QPointF     probe_start_point;
    QPointF     probe_end_point;
    path_calc_t path_info;

    void rectangle_occlusion_calc(void);
    bool check_line_intersects_rect(QLineF line);
    bool check_point_intersects_rect(QPointF point);

    void transverse_step_find_path(QPainterPath* path);
    void longitudinal_step_find_path(QPainterPath* path);

private:
    connect_block* start_point_block = nullptr;
    connect_block* end_point_block   = nullptr;
    QPointF        start_point;
    QPointF        end_point;
    connect_block* send_block;
    connect_block* recv_block;
    /* 自删 */
    QMenu    menu;
    QAction* deleteAction = nullptr;

protected:
    void         hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void         hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;
    void         contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;
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
};

#endif  // CONNECT_LINE_H
