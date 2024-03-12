#include "logic_view.h"
#include "block_class/connect_block.h"
#include "mainwindow.h"
#include "project_report.h"
#include <QDebug>
#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMimeData>
#include <QScrollBar>
#include <QVBoxLayout>
#include <climits>
/**
 * @brief 图形化编程构造函数
 * @param uiparent
 * @param parent
 */
logic_view::logic_view(QWidget* uiparent, QWidget* parent)
    : QGraphicsView(parent)
{
    ui         = MainWindow::my_ui->ui;
    mainwindow = ( MainWindow* )uiparent;
    mparent    = uiparent;
    init_ui();
}

/**
 * @brief logic_view::~logic_view
 */
logic_view::~logic_view()
{
    QList<QGraphicsItem*> allBlocks = my_scene->items();
    foreach (QGraphicsItem* item, allBlocks) {
        my_scene->removeItem(item);
    }
    delete my_scene;
}

/**
 * @brief 初始化
 */
void logic_view::init_ui()
{
    // setGeometry(SCENE_POS_ORIGIN, SCENE_POS_ORIGIN, MAX_SCENE_SIDE_LENGTH / 10, MAX_SCENE_SIDE_LENGTH / 10);

    // 启用缩放
    setInteractive(true);                            //启用交互模式
    setRenderHint(QPainter::Antialiasing);           //启用抗锯齿
    setRenderHint(QPainter::SmoothPixmapTransform);  //启用平滑的像素变换功能
    /* 图形视图的变换锚点被设置为鼠标下方， 平移、缩放、旋转等变换操作时，变换的中心点将根据鼠标的位置而动态改变 */
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);
    /* 图形项发生变化时，整个视口将被完全更新，重新绘制所有的图形项 */
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    /* 图形视图的拖拽模式被设置为滚动手势拖拽，使用鼠标或触摸手势来拖拽视图，实现滚动和平移的效果 */
    setDragMode(QGraphicsView::ScrollHandDrag);
    /* 不使用抗锯齿 */
    setOptimizationFlag(QGraphicsView::DontAdjustForAntialiasing, true);

    // 缩放设置
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);
    // 开启鼠标追踪。视图将能够跟踪鼠标的移动，无论鼠标是否按下任何按钮
    setMouseTracking(false);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    // 创建视图
    my_scene = new QGraphicsScene(this);
    my_scene->setSceneRect(SCENE_POS_ORIGIN, SCENE_POS_ORIGIN, MAX_SCENE_SIDE_LENGTH, MAX_SCENE_SIDE_LENGTH);
    setScene(my_scene);
    // setFrameStyle(QFrame::NoFrame);
    /* 镜头移到左上角 */
    QScrollBar* verticalScrollBar = this->verticalScrollBar();
    verticalScrollBar->setValue(-100);
    verticalScrollBar->setStyleSheet("QScrollBar { width: 25px; }");
    QScrollBar* horizontalScrollBar = this->horizontalScrollBar();
    horizontalScrollBar->setValue(-100);
    horizontalScrollBar->setStyleSheet("QScrollBar { height: 25px; }");
    // 设置视图背景为网格样式
    //    QPixmap gridPixmap(10, 10);
    //    gridPixmap.fill(QColor(220, 220, 220));
    //    QPainter painter(&gridPixmap);
    //    painter.setPen(Qt::lightGray);
    //    painter.drawRect(0, 0, 10, 10);
    //    setBackgroundBrush(QBrush(gridPixmap));
    QPixmap gridPixmap(15, 15);
    gridPixmap.fill(QColor(220, 220, 220));
    QPainter painter(&gridPixmap);
    painter.fillRect(0, 0, 1, 1, Qt::black);
    setBackgroundBrush(QBrush(gridPixmap));
    /* 初始化SF信息容器 */
    for (uint8_t i = 0; i < MAX_SF_NUM; i++) {
        sf_used_inf.sf_code[i].code    = SF_USER_CODE + i;
        sf_used_inf.sf_code[i].is_used = false;
    }
    /* 属性显示表格的设置 */
    ui->tableWidget_attribute->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    QString styleSheet = "QHeaderView::section { background-color: transparent; }";
    styleSheet += "QHeaderView::section:hover { background-color: transparent; }";
    ui->tableWidget_attribute->horizontalHeader()->setStyleSheet(styleSheet);
    ui->tableWidget_attribute->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    // 缩小垂直滚动条
    ui->tableWidget_attribute->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->tableWidget_attribute->verticalScrollBar()->setStyleSheet("QScrollBar { width: 10px; }");

    // 缩小水平滚动条
    ui->tableWidget_attribute->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->tableWidget_attribute->horizontalScrollBar()->setStyleSheet("QScrollBar { height: 10px; }");
    QTableWidgetItem* headerItem1 = new QTableWidgetItem("属性");
    QTableWidgetItem* headerItem2 = new QTableWidgetItem("值");
    ui->tableWidget_attribute->setHorizontalHeaderItem(0, headerItem1);
    ui->tableWidget_attribute->setHorizontalHeaderItem(1, headerItem2);
}

/**
 * @brief 生成工程信息，所有的块的工程信息都会在这里生成
 * @return 工程信息
 */
QJsonObject logic_view::logic_view_project_info()
{
    QJsonObject rootObject;
    int         condi_cnt = 0;
    int         logic_cnt = 0;
    int         line_cnt  = 0;
    QJsonObject logicObject;
    QJsonObject condiObject;
    QJsonObject lineObject;
    for (int i = 0; i < condition_block_list.count(); i++) {
        condition_block* condi                                = condition_block_list[i];
        condiObject["condition" + QString::number(condi_cnt)] = condi->condition_block_project_info();
        condi_cnt++;
    }
    for (int i = 0; i < logic_block_list.count(); i++) {
        logic_block* logic                                = logic_block_list[i];
        logicObject["logic" + QString::number(logic_cnt)] = logic->logic_block_project_info();
        logic_cnt++;
    }
    for (int i = 0; i < line_list.count(); i++) {
        connect_line* line                             = line_list[i];
        lineObject["line" + QString::number(line_cnt)] = line->connect_line_project_info();
        line_cnt++;
    }
    logicObject["number"]                  = logic_cnt;
    condiObject["number"]                  = condi_cnt;
    lineObject["number"]                   = line_cnt;
    rootObject["blockid"]                  = ( int )block_id;
    rootObject[project_programe_condition] = condiObject;
    rootObject[project_programe_logic]     = logicObject;
    rootObject[project_programe_line]      = lineObject;
    return rootObject;
}

/**
 * @brief 工程信息解析，将在这里重建图形化
 * @param project 工程信息
 * @return
 */
bool logic_view::logic_view_project_parse(QJsonObject project)
{
    QJsonObject logicObject = project[project_programe_logic].toObject();
    QJsonObject condiObject = project[project_programe_condition].toObject();
    QJsonObject lineObject  = project[project_programe_line].toObject();
    int         condi_num   = condiObject["number"].toInt();
    int         logic_num   = logicObject["number"].toInt();
    int         line_num    = lineObject["number"].toInt();

    for (int i = 0; i < condi_num; i++) {
        QJsonObject      condisub_obj = condiObject["condition" + QString::number(i)].toObject();
        condition_block* condition    = new condition_block(condisub_obj, mparent);
        my_scene->addItem(condition);
        condition_block_list.append(condition);
    }
    for (int i = 0; i < logic_num; i++) {
        QJsonObject  logicsub_obj = logicObject["logic" + QString::number(i)].toObject();
        logic_block* logic        = new logic_block(logicsub_obj, mparent);
        my_scene->addItem(logic);
        logic_block_list.append(logic);
    }
    for (int i = 0; i < line_num; i++) {
        QJsonObject   line_obj = lineObject["line" + QString::number(i)].toObject();
        connect_line* line     = new connect_line(mparent);
        my_scene->addItem(line);
        line->connect_line_project_parse(line_obj);
        line_list.append(line);
    }
    block_id = project["blockid"].toInt();
    return false;
}

/**
 * @brief 图形化编程复位
 */
void logic_view::logic_view_reset()
{
    QList<QGraphicsItem*> allBlocks = my_scene->items();
    foreach (QGraphicsItem* item, allBlocks) {
        my_scene->removeItem(item);
        delete item;
    }
    logic_block_list.clear();
    condition_block_list.clear();
    line_list.clear();
    attribute_display_id    = 0;
    probe_line              = nullptr;
    draw_line_state         = DRAW_LINE_STATE_IDLE;
    block_id                = 1;
    sf_used_inf.used_number = 0;
    for (uint8_t i = 0; i < MAX_SF_NUM; i++) {
        sf_used_inf.sf_code[i].code    = SF_USER_CODE + i;
        sf_used_inf.sf_code[i].is_used = false;
    }
    for (int i = 0; i < MAX_DECELERATE_NUM; i++) {
        sf_type_decelerate_isused[i] = false;
    }
    ui->tableWidget_attribute->clearContents();
    ui->tableWidget_attribute->setRowCount(0);
}

/**
 * @brief 画连接线的回调，在鼠标点击和松开事件内调用
 * @param block 要操作的连接点
 */
void logic_view::draw_line_both_block(connect_block* block)
{
    bool                  is_success = true;
    static connect_block* last_block = nullptr;
    switch (draw_line_state) {
    case DRAW_LINE_STATE_IDLE:
        if (block && !block->connect_is_created()) {
            last_block = block;
            probe_line = new connect_line(mparent);
            my_scene->addItem(probe_line);
            probe_line->set_start_point_block(last_block);
            draw_line_state = DRAW_LINE_STATE_ING;
        } else {
            mainwindow->dispaly_status_message("连接点已被连接，无法重复连接", 3000);
        }
        break;
    case DRAW_LINE_STATE_ING:
        //过滤 没选择另一个连接块  同一个逻辑块  自己连自己 以及同类型块
        if (block) {
            if (block == last_block) {
                mainwindow->dispaly_status_message("连接点无法连接自身", 3000);
                is_success = false;
            } else if (block->parentItem() == last_block->parentItem()) {
                mainwindow->dispaly_status_message("无法连接同一块的两个连接点", 3000);
                is_success = false;
            } else if (block->connect_is_created()) {
                mainwindow->dispaly_status_message("连接点已被连接，无法重复连接", 3000);
                is_success = false;
            } else if (block->get_connect_type() == last_block->get_connect_type()) {
                mainwindow->dispaly_status_message("输入与输入或输出与输出无法连接", 3000);
                is_success = false;
            } else if (block->parents_coincide_detect(last_block->self_block_attribute)) {
                mainwindow->dispaly_status_message("禁止连接父节点，将会形成逻辑死循环", 3000);
                is_success = false;
            }
            if (is_success == false) {
                draw_line_delete(probe_line);
                last_block = nullptr;  //删除第一个选中的块以及删除线
            } else {
                probe_line->set_end_point_block(block);
                line_list.append(probe_line);
                probe_line = nullptr;
                last_block = nullptr;
                mainwindow->dispaly_status_message("连接成功", 3000);
            }
        } else {
            draw_line_delete(probe_line);
            last_block = nullptr;  //删除第一个选中的块以及删除线
            mainwindow->dispaly_status_message("未正确选择终点", 3000);
        }
        draw_line_state = DRAW_LINE_STATE_IDLE;
        break;
    }
}

/**
 * @brief 删除连接线
 * @param line
 */
void logic_view::draw_line_delete(connect_line* line)
{
    if (line) {
        delete line;
        line = nullptr;
    }
}

/**
 * @brief 创建块
 * @param tool_info
 * @param pos
 */
void logic_view::creat_logic_block(tool_info_t* tool_info, QPointF pos)
{
    if (tool_info->tool_type >= TOOL_TYPE_LOGIC_AND && tool_info->tool_type <= TOOL_TYPE_LOGIC_EXIT) {
        if (tool_info->tool_type == TOOL_TYPE_LOGIC_SF && sf_used_inf.used_number >= MAX_SF_NUM) {
            mainwindow->my_message_box("创建失败", "SF数量已达上限值", false);
            return;
        }
        logic_block* logic = new logic_block(pos.x(), pos.y(), tool_info, block_id, mparent);
        my_scene->addItem(logic);
        logic_block_list.append(logic);
        block_id++;
    } else if (tool_info->tool_type >= TOOL_TYPE_CONDI_DI && tool_info->tool_type <= TOOL_TYPE_CONDI_BOOL) {
        condition_block* condition = new condition_block(pos.x(), pos.y(), tool_info, block_id, mparent);
        my_scene->addItem(condition);
        condition_block_list.append(condition);
        block_id++;
    }
}

/**
 * @brief 检测当前所有的块是否有故障
 * @return true-有故障 false-无故障
 */
bool logic_view::blocks_error_detect()
{
    bool exit_exist = false;
    if (condition_block_list.isEmpty() || logic_block_list.isEmpty()) {
        mainwindow->my_message_box("逻辑缺失", "图形化编程无可用逻辑，请认真编写!", false);
        return true;
    }
    foreach (condition_block* item, mainwindow->logic_view_class->condition_block_list) {
        block_error_t error;
        error = item->block_error;
        if (error.input_error.value != 0 || error.output_error.value != 0 || error.other_error.value != 0) {
            mainwindow->my_message_box("逻辑错误", "逻辑编程有错误，请检查", false);
            return true;
        }
    }
    foreach (logic_block* item, mainwindow->logic_view_class->logic_block_list) {
        block_error_t error;
        error = item->block_error;
        if (error.input_error.value != 0 || error.output_error.value != 0 || error.other_error.value != 0) {
            mainwindow->my_message_box("逻辑错误", "逻辑编程有错误，请检查", false);
            return true;
        }
        if (item->block_attribute.block_info.tool_type == TOOL_TYPE_LOGIC_EXIT) {
            exit_exist = true;
        }
    }
    if (exit_exist == false) {
        mainwindow->my_message_box("逻辑错误", "未设置EXIT模块", false);
        return true;
    }
    return false;
}

/* user slots */

/* sys slots */
/**
 * @brief 拖拽接收事件
 * @param event
 */
void logic_view::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasFormat("ruohui/tool")) {
        event->acceptProposedAction();
        QByteArray  byteArray = event->mimeData()->data("ruohui/tool");
        tool_info_t tool_info;
        memcpy(( char* )&tool_info, byteArray.constData(), byteArray.size());
        /* 根据拖拽的块不同设置虚拟块的大小 */
        if (tool_info.tool_type >= TOOL_TYPE_LOGIC_AND) {
            drop_tool_info.width  = LOGIC_BLOCK_WIDTH;
            drop_tool_info.height = LOGIC_BLOCK_HEIGHT;
        } else {
            drop_tool_info.width  = CONDITION_BLOCK_WIDTH;
            drop_tool_info.height = CONDITION_BLOCK_HEIGHT;
        }
        drop_tool_info.is_valid   = false;
        drop_tool_info.probe_rect = new QGraphicsRectItem(0, 0, drop_tool_info.width, drop_tool_info.height);
        drop_tool_info.probe_rect->setBrush(Qt::green);
        drop_tool_info.probe_rect->setOpacity(0.3);
        my_scene->addItem(drop_tool_info.probe_rect);
    }
}

/**
 * @brief 拖拽移动事件
 * @param event
 */
void logic_view::dragMoveEvent(QDragMoveEvent* event)
{
    if (event->mimeData()->hasFormat("ruohui/tool")) {
        QPointF pos = mapToScene(event->pos());  // 获取全局位置
        if (drop_tool_info.probe_rect != nullptr) {
            drop_tool_info.probe_rect->setPos(pos.x() - drop_tool_info.width / 2, pos.y() - drop_tool_info.height / 2);
        }
        QRectF currentRect = drop_tool_info.probe_rect->sceneBoundingRect();
        /* 视图边界不允许放置块 */
        if ((currentRect.x() < SCENE_MARGIN_MIN || currentRect.x() > SCENE_MARGIN_MAX)
            || (currentRect.y() < SCENE_MARGIN_MIN || currentRect.y() > SCENE_MARGIN_MAX)) {
            drop_tool_info.is_valid = false;
            drop_tool_info.probe_rect->setBrush(Qt::red);
            return;
        }
        /* 已有的块的附近不允许放置 */
        QList<QGraphicsItem*> allBlocks = my_scene->items();
        foreach (QGraphicsItem* item, allBlocks) {
            if (item->type() >= QGraphicsItem::UserType + BLOCK_TYPE_LOGIC) {
                QRectF otherRect = item->sceneBoundingRect();
                if (currentRect.intersects(
                        otherRect.adjusted(-BLOCK_SPCING, -BLOCK_SPCING, BLOCK_SPCING, BLOCK_SPCING))) {
                    drop_tool_info.is_valid = false;
                    drop_tool_info.probe_rect->setBrush(Qt::red);
                    return;
                }
            }
        }
        drop_tool_info.probe_rect->setBrush(Qt::green);
        drop_tool_info.is_valid = true;
        event->accept();
    }
}

/**
 * @brief 拖拽放弃事件
 * @param event
 */
void logic_view::dragLeaveEvent(QDragLeaveEvent* event)
{
    Q_UNUSED(event);
    if (drop_tool_info.probe_rect != nullptr) {
        my_scene->removeItem(drop_tool_info.probe_rect);
        drop_tool_info.probe_rect = nullptr;
        drop_tool_info.is_valid   = false;
    }
}

/**
 * @brief 拖拽完成事件
 * @param event
 */
void logic_view::dropEvent(QDropEvent* event)
{
    if (event->mimeData()->hasFormat("ruohui/tool")) {
        if (drop_tool_info.is_valid) {
            QByteArray  byteArray = event->mimeData()->data("ruohui/tool");
            tool_info_t tool_info;
            memcpy(( char* )&tool_info, byteArray.constData(), byteArray.size());
            QPointF pos = mapToScene(event->pos());  // 获取全局位置
            creat_logic_block(&tool_info, pos);
        } else {
            mainwindow->dispaly_status_message("禁止在此处放置", 3000);
        }
        if (drop_tool_info.probe_rect != nullptr) {
            my_scene->removeItem(drop_tool_info.probe_rect);
            drop_tool_info.probe_rect = nullptr;
            drop_tool_info.is_valid   = false;
        }
    }
}

/**
 * @brief 鼠标滚轮事件
 * @param event
 */
void logic_view::wheelEvent(QWheelEvent* event)
{
    int   delta    = event->delta();  //获取鼠标滑轮滚动的距离
    qreal maxscale = 2.0, minscale = 0.5;
    if (event->modifiers() & Qt::ControlModifier) {  // ctrl+鼠标滑轮进行缩放
        qreal scaleFactor  = 0.05;                   // 缩放因子--缩放的速度调节这里
        qreal currentscale = transform().m11();
        qreal targetscale  = (currentscale + scaleFactor) / currentscale;
        if (delta > 0 && currentscale < maxscale) {
            scale(targetscale, targetscale);  // 向上滚动，放大视图
        } else if (delta < 0 && currentscale > minscale) {
            scale(1 / targetscale, 1 / targetscale);
        }
        event->accept();
    } else {
        int         scrollStep        = delta / 8;  // 鼠标滑轮运动时界面上下移动的速度在这里计算
        QScrollBar* verticalScrollBar = this->verticalScrollBar();
        int         currentValue      = verticalScrollBar->value();
        int         newValue          = currentValue - scrollStep;
        verticalScrollBar->setValue(newValue);
        event->accept();
    }
}

void logic_view::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && draw_line_state == DRAW_LINE_STATE_IDLE) {
        // 将鼠标事件的位置从视图坐标系转换为场景坐标系
        QPointF scenePos = mapToScene(event->pos());
        // 在场景中查找图形项
        QGraphicsItem* item       = scene()->itemAt(scenePos, QTransform());
        connect_block* otherBlock = dynamic_cast<connect_block*>(item);
        if (item && item->type() == QGraphicsItem::UserType + BLOCK_TYPE_CONNECT) {
            draw_line_both_block(otherBlock);
        }
    }
    QGraphicsView::mousePressEvent(event);
}

void logic_view::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && (draw_line_state == DRAW_LINE_STATE_ING && probe_line != nullptr)) {
        // 将鼠标事件的位置从视图坐标系转换为场景坐标系
        QPointF scenePos = mapToScene(event->pos());
        // 在场景中查找图形项
        QGraphicsItem* item       = scene()->itemAt(scenePos, QTransform());
        connect_block* otherBlock = dynamic_cast<connect_block*>(item);
        if (item && item->type() == QGraphicsItem::UserType + BLOCK_TYPE_CONNECT) {
            draw_line_both_block(otherBlock);

        } else {
            draw_line_both_block(nullptr);
        }
    }
    QGraphicsView::mouseReleaseEvent(event);
}

void logic_view::mouseMoveEvent(QMouseEvent* event)
{
    if (draw_line_state == DRAW_LINE_STATE_ING && probe_line != nullptr) {
        probe_line->set_end_point(mapToScene(event->pos()));
    }
    // qDebug() << this->horizontalScrollBar()->value();
    QGraphicsView::mouseMoveEvent(event);
}
