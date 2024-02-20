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
logic_view::logic_view(QWidget* uiparent, QWidget* parent)
    : QGraphicsView(parent)
{
    ui         = MainWindow::my_ui->ui;
    mainwindow = ( MainWindow* )uiparent;
    mparent    = uiparent;
    init_ui();
}

logic_view::~logic_view()
{
    QList<QGraphicsItem*> allBlocks = my_scene->items();
    foreach (QGraphicsItem* item, allBlocks) {
        my_scene->removeItem(item);
    }
    delete my_scene;
}

void logic_view::init_ui()
{
    setGeometry(-100, -100, INFINITY, INFINITY);
    // 启用缩放
    setInteractive(true);
    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::SmoothPixmapTransform);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setDragMode(QGraphicsView::ScrollHandDrag);
    setOptimizationFlag(QGraphicsView::DontAdjustForAntialiasing, true);

    // 缩放设置
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);
    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::SmoothPixmapTransform);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setOptimizationFlag(QGraphicsView::DontAdjustForAntialiasing, true);
    setDragMode(QGraphicsView::ScrollHandDrag);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    // 缩放功能
    setMouseTracking(true);
    setInteractive(true);
    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::SmoothPixmapTransform);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setOptimizationFlag(QGraphicsView::DontAdjustForAntialiasing, true);
    setDragMode(QGraphicsView::ScrollHandDrag);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);
    //

    // 创建视图
    my_scene = new QGraphicsScene(this);
    my_scene->setSceneRect(-100, -100, INFINITY, INFINITY);
    setScene(my_scene);
    setFrameStyle(QFrame::NoFrame);

    // 设置视图背景为网格样式
    QPixmap gridPixmap(50, 50);
    gridPixmap.fill(Qt::white);
    QPainter painter(&gridPixmap);
    painter.setPen(Qt::lightGray);
    painter.drawRect(0, 0, 50, 50);
    setBackgroundBrush(QBrush(gridPixmap));
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
    ui->tableWidget_attribute->clearContents();
    ui->tableWidget_attribute->setRowCount(0);
}

/* 画块与块之间的连接线 */
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

void logic_view::draw_line_delete(connect_line* line)
{
    if (line) {
        delete line;
        line = nullptr;
    }
}

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
        // connect(logic, &logic_block::block_delete_signal, this, &condition_delete_slot);
    } else if (tool_info->tool_type >= TOOL_TYPE_CONDI_DI && tool_info->tool_type <= TOOL_TYPE_CONDI_QEP) {
        condition_block* condition = new condition_block(pos.x(), pos.y(), tool_info, block_id, mparent);
        my_scene->addItem(condition);
        condition_block_list.append(condition);
        block_id++;
    }
}

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

/* 事件 */
void logic_view::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasFormat("ruohui/tool")) {
        event->acceptProposedAction();
        QByteArray  byteArray = event->mimeData()->data("ruohui/tool");
        tool_info_t tool_info;
        memcpy(( char* )&tool_info, byteArray.constData(), byteArray.size());
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

void logic_view::dragMoveEvent(QDragMoveEvent* event)
{
    if (event->mimeData()->hasFormat("ruohui/tool")) {
        QPointF pos = mapToScene(event->pos());  // 获取全局位置
        if (drop_tool_info.probe_rect != nullptr) {
            drop_tool_info.probe_rect->setPos(pos.x() - drop_tool_info.width / 2, pos.y() - drop_tool_info.height / 2);
        }
        QRectF                currentRect = drop_tool_info.probe_rect->sceneBoundingRect();
        QList<QGraphicsItem*> allBlocks   = my_scene->items();
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

void logic_view::dragLeaveEvent(QDragLeaveEvent* event)
{
    if (drop_tool_info.probe_rect != nullptr) {
        my_scene->removeItem(drop_tool_info.probe_rect);
        drop_tool_info.probe_rect = nullptr;
        drop_tool_info.is_valid   = false;
    }
}

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
            mainwindow->dispaly_status_message("此处已有其他块，禁止在此处放置", 3000);
        }
        if (drop_tool_info.probe_rect != nullptr) {
            my_scene->removeItem(drop_tool_info.probe_rect);
            drop_tool_info.probe_rect = nullptr;
            drop_tool_info.is_valid   = false;
        }
    }
}

void logic_view::wheelEvent(QWheelEvent* event)
{
    int delta = event->delta();                      //获取鼠标滑轮滚动的距离
    if (event->modifiers() & Qt::ControlModifier) {  // ctrl+鼠标滑轮进行缩放
        qreal scaleFactor = 1.05;                    // 缩放因子--缩放的速度调节这里
        if (delta > 0) {
            scale(scaleFactor, scaleFactor);  // 向上滚动，放大视图
        } else {
            scale(1 / scaleFactor, 1 / scaleFactor);
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
    QGraphicsView::mouseMoveEvent(event);
}
