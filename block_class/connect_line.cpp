#include "connect_line.h"
#include "QDebug"
#include "condition_block.h"
#include "logic_block.h"
#include "mainwindow.h"
#include "qgraphicsscene.h"
#include <QApplication>
#include <QWidget>
#include <windows.h>
/**
 * @brief 连接线构造函数
 * @param uiparent mainwindow，方便调用主程序元素
 * @param parent
 */
connect_line::connect_line(QWidget* uiparent, QGraphicsItem* parent)
    : QGraphicsPathItem(parent)
{
    ui         = MainWindow::my_ui->ui;
    mainwindow = ( MainWindow* )uiparent;
    setCursor(Qt::ArrowCursor);  // 设置鼠标样式为箭头
    setAcceptHoverEvents(true);
    deleteAction = new QAction("删除", this);
    deleteAction->setIcon(QIcon(":/new/photo/photo/delete.png"));
    menu.addAction(deleteAction);
    setFlag(QGraphicsItem::ItemIsFocusable);  //接受键盘事件
    setFocus();
}

/**
 * @brief 连接线析构函数
 */
connect_line::~connect_line()
{
    /* 从连接线列表内移除自己 */
    if (mainwindow->logic_view_class->line_list.contains(this)) {
        mainwindow->logic_view_class->line_list.removeOne(this);
    }
    if (start_point_block) {
        disconnect(start_point_block, connect_block::position_change_signal, this, start_position_change_slot);
        start_point_block->connect_line_delete();
    }
    if (end_point_block) {
        disconnect(end_point_block, connect_block::position_change_signal, this, start_position_change_slot);
        end_point_block->connect_line_delete();
        disconnect(send_block, connect_block::send_block_attribute_signal, recv_block,
                   connect_block::input_point_receive_info);
        disconnect(send_block, connect_block::send_debug_data_signal, recv_block,
                   connect_block::receive_debug_data_slot);
    }
    if (scene()) {
        if (scene()->items().contains(this)) {
            scene()->removeItem(this);
        }
    }
}

/**
 * @brief 生成连接线的工程信息
 * @return json格式的工程信息
 */
QJsonObject connect_line::connect_line_project_info()
{
    QJsonObject rootObject;
    rootObject["s_parentid"] = ( int )start_point_block->self_block_attribute->self_id;
    rootObject["s_selfid"]   = start_point_block->self_id;
    rootObject["s_type"]     = ( int )start_point_block->get_connect_type();

    rootObject["e_parentid"] = ( int )end_point_block->self_block_attribute->self_id;
    rootObject["e_selfid"]   = end_point_block->self_id;
    rootObject["e_type"]     = ( int )end_point_block->get_connect_type();
    return rootObject;
}

/**
 * @brief 连接线工程信息解析
 * @param project json格式的工程信息
 * @return 解析结果
 */
bool connect_line::connect_line_project_parse(QJsonObject project)
{
    connect_block*       s_block = nullptr;
    connect_block*       e_block = nullptr;
    int                  s_pid   = project["s_parentid"].toInt();
    int                  s_sid   = project["s_selfid"].toInt();
    connect_point_type_e s_type  = ( connect_point_type_e )project["s_type"].toInt();
    int                  e_pid   = project["e_parentid"].toInt();
    int                  e_sid   = project["e_selfid"].toInt();
    connect_point_type_e e_type  = ( connect_point_type_e )project["e_type"].toInt();

    QList<QGraphicsItem*> allBlocks = scene()->items();
    foreach (QGraphicsItem* item, allBlocks) {
        if (item->type() == QGraphicsItem::UserType + BLOCK_TYPE_CONDITION) {
            condition_block* condi = dynamic_cast<condition_block*>(item);
            if (condi->block_attribute.self_id == ( uint32_t )s_pid) {
                s_block = condi->output_point_list[s_sid];
            } else if (condi->block_attribute.self_id == ( uint32_t )e_pid) {
                e_block = condi->output_point_list[e_sid];
            }
        } else if (item->type() == QGraphicsItem::UserType + BLOCK_TYPE_LOGIC) {
            logic_block* logic = dynamic_cast<logic_block*>(item);
            if (logic->block_attribute.self_id == ( uint32_t )s_pid) {
                if (s_type == CONNECT_POINT_TYPE_OUTPUT) {
                    s_block = logic->output_point_list[s_sid];
                } else {
                    s_block = logic->input_point_list[s_sid];
                }
            } else if (logic->block_attribute.self_id == ( uint32_t )e_pid) {
                if (e_type == CONNECT_POINT_TYPE_OUTPUT) {
                    e_block = logic->output_point_list[e_sid];
                } else {
                    e_block = logic->input_point_list[e_sid];
                }
            }
        }
        if (s_block != nullptr && e_block != nullptr) {
            break;
        }
    }
    if (s_block == nullptr || e_block == nullptr) {
        return false;
    }
    set_start_point_block(s_block);
    set_end_point_block(e_block);
    return true;
}

/**
 * @brief 画线过程中设置结束点，当前画线未完成，结束点跟随鼠标指针移动
 * @param endpoint 结束点
 */
void connect_line::set_end_point(QPointF endpoint)
{
    //对坐标进行缩略，防止线终点在鼠标正下方触发事件
    qreal        x           = endpoint.x() > start_point.x() ? endpoint.x() - 2 : endpoint.x() + 2;
    qreal        y           = endpoint.y() > start_point.y() ? endpoint.y() - 2 : endpoint.y() + 2;
    QPointF      probe_point = QPointF(x, y);
    QPainterPath path;
    path.moveTo(start_point);
    path.lineTo(probe_point);
    setPath(path);
}

/**
 * @brief 设置起始连接点
 * @param startblock 起始点
 */
void connect_line ::set_start_point_block(connect_block* startblock)
{
    start_point_block = startblock;
    qreal x           = start_point_block->get_connect_type() == CONNECT_POINT_TYPE_OUTPUT ?
                  start_point_block->rect().x() + start_point_block->rect().width() :
                  start_point_block->rect().x();
    /* 记录起始点坐标，连接起始点位置变动和删除信号 */
    start_point =
        start_point_block->mapToScene(x, start_point_block->rect().y() + start_point_block->rect().height() / 2);
    connect(start_point_block, connect_block::position_change_signal, this, start_position_change_slot);
    connect(start_point_block, connect_block::item_deleted, this, start_point_deleted_slot);
    start_point_block->connect_line_creat();  //通知连接点连接线建立
}

/**
 * @brief 设置终点连接点
 * @param endblock 终点
 */
void connect_line::set_end_point_block(connect_block* endblock)
{
    end_point_block = endblock;
    qreal x         = end_point_block->get_connect_type() == CONNECT_POINT_TYPE_OUTPUT ?
                  end_point_block->rect().x() + end_point_block->rect().width() :
                  end_point_block->rect().x();
    end_point = end_point_block->mapToScene(x, end_point_block->rect().y() + end_point_block->rect().height() / 2);
    connect(end_point_block, connect_block::position_change_signal, this, end_position_change_slot);
    connect(end_point_block, connect_block::item_deleted, this, end_point_deleted_slot);
    end_point_block->connect_line_creat();  //通知连接点连接线建立
    calc_path();                            //路径计算
    /* 连接输出点属性发送信号和输入点的属性输入槽函数
     * 连接输出点仿真数据发送信号和输入点的仿真数据接收以及连接线仿真数据接收槽函数
     */
    send_block = start_point_block;
    recv_block = end_point_block;
    if (start_point_block->get_connect_type() == CONNECT_POINT_TYPE_INPUT) {
        send_block = end_point_block;
        recv_block = start_point_block;
    }
    connect(send_block, connect_block::send_block_attribute_signal, recv_block,
            connect_block::input_point_receive_info);
    connect(send_block, connect_block::send_debug_data_signal, recv_block, connect_block::receive_debug_data_slot);
    connect(send_block, connect_block::send_debug_data_signal, this, debug_data_prase_slot);

    connect(start_point_block, &connect_block::send_focus_state_signal, this, start_focus_slot);
    connect(end_point_block, &connect_block::send_focus_state_signal, this, end_focus_slot);
}

/**
 * @brief 路径计算
 */
void connect_line::calc_path()
{
    QPainterPath path;
    rectangle_occlusion_calc();  //先计算方向
    /* 哪边遇到的障碍物多就去哪边 */
    if (path_info.transverse_intersect_num >= path_info.longitudinal_intersect_num) {
        path_info.real_dir = DIRECTION_TRANSVERSE;
    } else {
        path_info.real_dir = DIRECTION_LONGITUDINAL;
    }
    /* 清除原路径 */
    path_info.is_sucessful = false;
    /* 起点和终点不能离自己的逻辑块太近，否则会触发点碰撞，所以要设置伪起点和伪终点  */
    /* 规划伪起点和伪终点，规定伪起点必须在伪终点左侧 */
    probe_start_point = start_point;
    probe_end_point   = end_point;
    if (start_point_block->get_connect_type() == CONNECT_POINT_TYPE_INPUT) {
        probe_start_point = end_point;
        probe_end_point   = start_point;
    }
    QPointF source_start = probe_start_point;
    QPointF source_end   = probe_end_point;
    /* 记录终点位置，最后进行连接 */
    path_info.path_end_point = probe_end_point;
    probe_start_point.setX(probe_start_point.x() + 13); /* 伪起点位置 */
    probe_end_point.setX(probe_end_point.x() - 13);     /* 伪终点位置 */
    path.moveTo(source_start);
    path.lineTo(probe_start_point);
    while (path_info.is_sucessful == false) {
        if (path_info.real_dir >= DIRECTION_LONGITUDINAL) {
            longitudinal_step_find_path(&path);
        } else {
            transverse_step_find_path(&path);
        }
    }
    /* 至少两条线(且至少有一条横线和一条纵线)无遮挡，且至少有一条与起点有关的判定为路线寻找完毕 */
    if ((path_info.transversse_line2_is_none && path_info.longitudinal_line1_is_none)) {
        path.lineTo(probe_start_point);
        path.lineTo(probe_start_point.x(), probe_end_point.y());
        path.lineTo(probe_end_point.x(), probe_end_point.y());
        path.lineTo(source_end.x(), source_end.y());
        path_info.is_sucessful = true;
        setPath(path);
    } else if ((path_info.transversse_line1_is_none && path_info.longitudinal_line2_is_none)) {
        path.lineTo(probe_start_point);
        path.lineTo(probe_end_point.x(), probe_start_point.y());
        path.lineTo(probe_end_point.x(), probe_end_point.y());
        path.lineTo(source_end.x(), source_end.y());
        path_info.is_sucessful = true;
        setPath(path);
    }
}

/**
 * @brief 判断一个点是否与方块周边相交，除4是为了可以让线路能够通过横向的窄路
 * @param point 点坐标
 * @return 是否相交
 */
bool connect_line::check_point_intersects_rect(QPointF point)
{
    QList<QGraphicsItem*> items = scene()->items();
    for (QGraphicsItem* item : items) {
        if (item->type() >= (QGraphicsItem::UserType + BLOCK_TYPE_LOGIC)
            && item->type() <= (QGraphicsItem::UserType + BLOCK_TYPE_CONDITION)) {
            if (item->sceneBoundingRect()
                    .adjusted(-BLOCK_SPCING / 2, -BLOCK_SPCING / 4, BLOCK_SPCING / 2, BLOCK_SPCING / 4)
                    .contains(point)) {
                return true;
            }
        }
    }
    return false;
}

/**
 * @brief 检测一条线是否与当前视图中某个块相交(检查直线与矩形的每条边是否相交)，
 * @param line 线
 * @return 是否相交
 */
bool connect_line::check_line_intersects_rect(QLineF line)
{
    QList<QGraphicsItem*> items = scene()->items();
    for (QGraphicsItem* item : items) {
        if (item->type() >= (QGraphicsItem::UserType + BLOCK_TYPE_LOGIC)
            && item->type() <= (QGraphicsItem::UserType + BLOCK_TYPE_CONDITION)) {
            if (item == start_point_block || item == end_point_block) {
                continue;
            }
            if (item) {
                QRectF rect =
                    item->sceneBoundingRect().adjusted(-BLOCK_SPCING / 2, -BLOCK_SPCING / 4, BLOCK_SPCING / 2,
                                                       BLOCK_SPCING / 4);  //除4是为了可以让线路能够通过横向的窄路
                if (rect.contains(line.p1()) || rect.contains(line.p2())) {
                    return true;
                }
                QPointF intersectionPoint;
                // 检查直线与矩形的每条边是否相交
                QLineF topLine(rect.topLeft(), rect.topRight());
                if (line.intersect(topLine, &intersectionPoint) == QLineF::BoundedIntersection)
                    return true;
                QLineF bottomLine(rect.bottomLeft(), rect.bottomRight());
                if (line.intersect(bottomLine, &intersectionPoint) == QLineF::BoundedIntersection)
                    return true;
                QLineF leftLine(rect.topLeft(), rect.bottomLeft());
                if (line.intersect(leftLine, &intersectionPoint) == QLineF::BoundedIntersection)
                    return true;
                QLineF rightLine(rect.topRight(), rect.bottomRight());
                if (line.intersect(rightLine, &intersectionPoint) == QLineF::BoundedIntersection)
                    return true;
            }
        }
    }
    return false;
}

/**
 * @brief 判断伪起点和伪终点矩形的遮挡情况
 */
void connect_line::rectangle_occlusion_calc()
{
    path_info.longitudinal_intersect_num = 0;
    path_info.transverse_intersect_num   = 0;
    QPointF rect_point1(probe_end_point.x(), probe_start_point.y());  //矩形的其他点,此点与起点组成横线
    QPointF rect_point2(probe_start_point.x(), probe_end_point.y());  //矩形的其他点,此点与起点组成纵线
    QLineF  transversse_line1(probe_start_point, rect_point1);        //与起点相连的横线
    QLineF  transversse_line2(rect_point2, probe_end_point);          //与终点相连的横线
    QLineF  longitudinal_line1(probe_start_point, rect_point2);       //与起点相连的纵线
    QLineF  longitudinal_line2(rect_point1, probe_end_point);         //与终点相连的纵线
    path_info.transversse_line1_is_none  = true;                      //横向线与纵向线是否无遮挡
    path_info.transversse_line2_is_none  = true;
    path_info.longitudinal_line1_is_none = true;
    path_info.longitudinal_line2_is_none = true;
    /* 判断四条线有几条线与其他块相交 */
    if (check_line_intersects_rect(transversse_line1)) {
        path_info.transverse_intersect_num++;
        path_info.transversse_line1_is_none = false;
    }
    if (check_line_intersects_rect(transversse_line2)) {
        path_info.transverse_intersect_num++;
        path_info.transversse_line2_is_none = false;
    }
    if (check_line_intersects_rect(longitudinal_line1)) {
        path_info.longitudinal_intersect_num++;
        path_info.longitudinal_line1_is_none = false;
    }
    if (check_line_intersects_rect(longitudinal_line2)) {
        path_info.longitudinal_intersect_num++;
        path_info.longitudinal_line2_is_none = false;
    }

    if ((path_info.transversse_line2_is_none && path_info.longitudinal_line1_is_none)
        || (path_info.transversse_line1_is_none && path_info.longitudinal_line2_is_none)) {
        path_info.is_sucessful = true;
    }
}

/**
 * @brief 横向路径寻找
 * @param path 路径
 */
void connect_line::transverse_step_find_path(QPainterPath* path)
{
    QPointF probe_point;  //用于探测下一个点有没有发生碰撞
    /* 纵向受阻，就先移动横向并检测纵向是否还有阻碍*/
    if (path_info.real_dir != DIRECTION_TRANSVERSE) {
        if (probe_end_point.y() > probe_start_point.y()) {
            probe_point = QPointF(probe_start_point.x(), probe_start_point.y() + FIND_PATH_STEP);
        } else {
            probe_point = QPointF(probe_start_point.x(), probe_start_point.y() - FIND_PATH_STEP);
        }
    }
    switch (path_info.real_dir) {
    case DIRECTION_TRANSVERSE_LEFT:
        /* 总的目标其实还是纵向，所以要检测纵向的下个探点现在还会不会被遮挡，如果没遮挡就继续回到纵向 */
        if (check_point_intersects_rect(probe_point)) {
            probe_start_point = QPointF(probe_start_point.x() - FIND_PATH_STEP, probe_start_point.y());
        } else {
            path->lineTo(probe_start_point);
            path_info.real_dir = DIRECTION_LONGITUDINAL;
        }
        break;
    case DIRECTION_TRANSVERSE_RIGHT:
        if (check_point_intersects_rect(probe_point)) {
            probe_start_point = QPointF(probe_start_point.x() + FIND_PATH_STEP, probe_start_point.y());
        } else {
            path->lineTo(probe_start_point);
            path_info.real_dir = DIRECTION_LONGITUDINAL;
        }
        break;
    case DIRECTION_TRANSVERSE:
        /* 横向运动，遇到障碍就临时换到纵向 */
        if (probe_end_point.x() > probe_start_point.x() + FIND_PATH_STEP) {
            probe_point = QPointF(probe_start_point.x() + FIND_PATH_STEP, probe_start_point.y());
        } else if (probe_end_point.x() < probe_start_point.x() - FIND_PATH_STEP) {
            probe_point = QPointF(probe_start_point.x() - FIND_PATH_STEP, probe_start_point.y());
        } else {
            path_info.real_dir = DIRECTION_LONGITUDINAL;
            path->lineTo(probe_start_point);
            return;
        }
        if (check_point_intersects_rect(probe_point)) {  //横向受阻
            if (abs(probe_start_point.y() - probe_end_point.y()) > LOGIC_BLOCK_HEIGHT * 1.1) {
                if (probe_end_point.y() > probe_start_point.y()) {
                    path_info.real_dir = DIRECTION_LONGITUDINAL_DOWN;
                } else {
                    path_info.real_dir = DIRECTION_LONGITUDINAL_UP;
                }
            } else {
                path_info.real_dir = DIRECTION_LONGITUDINAL_DOWN;
            }
            path->lineTo(probe_start_point);
        } else {
            probe_start_point = probe_point;
            rectangle_occlusion_calc();
        }
        break;
    default:
        break;
    }
}

/**
 * @brief 纵向路径寻找
 * @param path
 */
void connect_line::longitudinal_step_find_path(QPainterPath* path)
{

    QPointF probe_point;  //用于探测下一个点有没有发生碰撞
    /* 横向受阻，就先移动纵向并检测横向是否还有阻碍*/
    if (path_info.real_dir != DIRECTION_LONGITUDINAL) {
        if (probe_end_point.x() > probe_start_point.x()) {
            probe_point = QPointF(probe_start_point.x() + FIND_PATH_STEP, probe_start_point.y());
        } else {
            probe_point = QPointF(probe_start_point.x() - FIND_PATH_STEP, probe_start_point.y());
        }
    }

    switch (path_info.real_dir) {
    case DIRECTION_LONGITUDINAL_UP:
        if (check_point_intersects_rect(probe_point)) {
            probe_start_point = QPointF(probe_start_point.x(), probe_start_point.y() - FIND_PATH_STEP);
        } else {
            path->lineTo(probe_start_point);
            path_info.real_dir = DIRECTION_TRANSVERSE;
        }
        break;
    case DIRECTION_LONGITUDINAL_DOWN:
        if (check_point_intersects_rect(probe_point)) {
            probe_start_point = QPointF(probe_start_point.x(), probe_start_point.y() + FIND_PATH_STEP);
        } else {
            path->lineTo(probe_start_point);
            path_info.real_dir = DIRECTION_TRANSVERSE;
        }
        break;
    case DIRECTION_LONGITUDINAL:
        if (probe_end_point.y() > probe_start_point.y() + FIND_PATH_STEP) {
            probe_point = QPointF(probe_start_point.x(), probe_start_point.y() + FIND_PATH_STEP);
        } else if (probe_end_point.y() < probe_start_point.y() - FIND_PATH_STEP) {
            probe_point = QPointF(probe_start_point.x(), probe_start_point.y() - FIND_PATH_STEP);
        } else {
            path_info.real_dir = DIRECTION_TRANSVERSE;
            path->lineTo(probe_start_point);
            return;
        }
        if (check_point_intersects_rect(probe_point)) {  //纵向受阻
            if (abs(probe_start_point.x() - probe_end_point.x()) > LOGIC_BLOCK_WIDTH * 1.1) {
                if (probe_end_point.x() > probe_start_point.x()) {
                    path_info.real_dir = DIRECTION_TRANSVERSE_RIGHT;
                } else {
                    path_info.real_dir = DIRECTION_TRANSVERSE_LEFT;
                }
            } else {
                path_info.real_dir = DIRECTION_TRANSVERSE_RIGHT;
            }
            path->lineTo(probe_start_point);
        } else {
            probe_start_point = probe_point;
            rectangle_occlusion_calc();
        }
        break;
    default:
        break;
    }
}

/* 用户事件  */

void connect_line::start_position_change_slot(void)
{
    qreal x = start_point_block->get_connect_type() == CONNECT_POINT_TYPE_OUTPUT ?
                  start_point_block->rect().x() + start_point_block->rect().width() :
                  start_point_block->rect().x();

    start_point =
        start_point_block->mapToScene(x, start_point_block->rect().y() + start_point_block->rect().height() / 2);
    calc_path();
}

void connect_line::end_position_change_slot(void)
{
    qreal x = end_point_block->get_connect_type() == CONNECT_POINT_TYPE_OUTPUT ?
                  end_point_block->rect().x() + end_point_block->rect().width() :
                  end_point_block->rect().x();

    end_point = end_point_block->mapToScene(x, end_point_block->rect().y() + end_point_block->rect().height() / 2);
    calc_path();
}

void connect_line::start_point_deleted_slot()
{
    scene()->removeItem(this);
    delete this;
}

void connect_line::end_point_deleted_slot()
{
    scene()->removeItem(this);
    delete this;
}

void connect_line::debug_data_prase_slot(bool res)
{
    if (res) {
        QPen originalPen(Qt::green, 1);
        setPen(originalPen);
    } else {
        QPen originalPen(Qt::black, 1);
        setPen(originalPen);
    }
}

void connect_line::start_focus_slot(bool state)
{
    if (state) {
        QPen pen(QColor(0, 0, 255));
        pen.setWidth(2);
        setPen(pen);
        focus_block = start_point_block;
        focus_state = true;
    } else {
        if (focus_block == start_point_block) {
            QPen originalPen(Qt::black, 1);
            setPen(originalPen);
            focus_state = false;
        }
    }
}

void connect_line::end_focus_slot(bool state)
{
    if (state) {
        QPen pen(QColor(0, 0, 255));
        pen.setWidth(2);
        setPen(pen);
        focus_block = end_point_block;
        focus_state = true;
    } else {
        if (focus_block == end_point_block) {
            QPen originalPen(Qt::black, 1);
            setPen(originalPen);
            focus_state = false;
        }
    }
}

/* 系统事件 */

void connect_line::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
    QAction* selectedItem = menu.exec(event->screenPos());
    if (selectedItem == deleteAction) {
        delete this;
    }
    event->accept();
}

void connect_line::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    Q_UNUSED(event);
    if (end_point_block == nullptr) {
        return;
    }
    if (!focus_state) {
        QPen pen = this->pen();
        pen.setWidth(3);
        setPen(pen);
    }
}

void connect_line::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    Q_UNUSED(event);
    if (!focus_state) {
        QPen pen = this->pen();
        pen.setWidth(1);
        setPen(pen);
    }
}

void connect_line::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Delete) {
        if (end_point_block != nullptr) {
            delete this;
        }
    } else {
        QGraphicsItem::keyPressEvent(event);
    }
}
