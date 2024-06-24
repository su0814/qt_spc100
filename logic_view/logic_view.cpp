#include "logic_view.h"
#include "config/def.h"
#include "logic_undocommand.h"
#include "mainwindow.h"
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
    // setGeometry(0, 0, MAX_SCENE_SIDE_LENGTH, MAX_SCENE_SIDE_LENGTH);
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
    // setDragMode(QGraphicsView::ScrollHandDrag);
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
    verticalScrollBar->setMinimum(0);
    verticalScrollBar->setMaximum(MAX_SCENE_SIDE_LENGTH);
    verticalScrollBar->setValue(MAX_SCENE_SIDE_LENGTH / 2);
    QScrollBar* horizontalScrollBar = this->horizontalScrollBar();
    horizontalScrollBar->setMinimum(0);
    horizontalScrollBar->setMaximum(MAX_SCENE_SIDE_LENGTH);
    horizontalScrollBar->setValue(MAX_SCENE_SIDE_LENGTH / 2);
    // 设置视图背景为网格样式
    //    QPixmap gridPixmap(10, 10);
    //    gridPixmap.fill(QColor(220, 220, 220));
    //    QPainter painter(&gridPixmap);
    //    painter.setPen(Qt::lightGray);
    //    painter.drawRect(0, 0, 10, 10);
    //    setBackgroundBrush(QBrush(gridPixmap));
    QPixmap gridPixmap(10, 10);
    gridPixmap.fill(QColor(220, 220, 220));
    QPainter painter(&gridPixmap);
    painter.fillRect(0, 0, 1, 1, Qt::black);
    setBackgroundBrush(QBrush(gridPixmap));
    m_undostack = new QUndoStack(this);
    //    m_undoaction = m_undostack->createUndoAction(this, "Undo");
    //    m_undoaction->setShortcut(QKeySequence::Undo);
    //    m_redoaction = m_undostack->createRedoAction(this, "Redo");
    //    m_redoaction->setShortcut(QKeySequence::Redo);
    //    this->addAction(m_undoaction);
    //    this->addAction(m_redoaction);
}

/**
 * @brief 生成工程信息，所有的块的工程信息都会在这里生成
 * @return 工程信息
 */
QJsonObject logic_view::logic_view_project_info()
{
    QJsonObject rootObject;
    QJsonObject inputObject;
    for (int i = 0; i < input_block_list.size(); i++) {
        inputObject["inputblock" + QString::number(i)] = input_block_list[i]->block_project_info();
    }
    QJsonObject outputObject;
    for (int i = 0; i < output_block_list.size(); i++) {
        outputObject["outputblock" + QString::number(i)] = output_block_list[i]->block_project_info();
    }
    QJsonObject baselogicObject;
    for (int i = 0; i < base_logic_block_list.size(); i++) {
        baselogicObject["baselogicblock" + QString::number(i)] = base_logic_block_list[i]->block_project_info();
    }
    QJsonObject applylogicObject;
    for (int i = 0; i < apply_logic_block_list.size(); i++) {
        applylogicObject["applylogicblock" + QString::number(i)] = apply_logic_block_list[i]->block_project_info();
    }
    QJsonObject delaylogicObject;
    for (int i = 0; i < delay_counter_block_list.size(); i++) {
        delaylogicObject["delaylogicblock" + QString::number(i)] = delay_counter_block_list[i]->block_project_info();
    }
    QJsonObject speedlogicObject;
    for (int i = 0; i < speed_logic_block_list.size(); i++) {
        speedlogicObject["speedlogicblock" + QString::number(i)] = speed_logic_block_list[i]->block_project_info();
    }
    QJsonObject lineObject;
    for (int i = 0; i < connection_line_list.size(); i++) {
        lineObject["line" + QString::number(i)] = connection_line_list[i]->connect_line_project_info();
    }
    rootObject["inputblock"]      = inputObject;
    rootObject["outputblock"]     = outputObject;
    rootObject["applylogicblock"] = applylogicObject;
    rootObject["delaylogicblock"] = delaylogicObject;
    rootObject["speedlogicblock"] = speedlogicObject;
    rootObject["baselogicblock"]  = baselogicObject;
    rootObject["line"]            = lineObject;
    return rootObject;
}

/**
 * @brief 工程信息解析，将在这里重建图形化
 * @param project 工程信息
 * @return
 */
bool logic_view::logic_view_project_parse(QJsonObject project)
{
    QJsonObject inputObject  = project["inputblock"].toObject();
    QStringList inputkeys    = inputObject.keys();
    QJsonObject outputObject = project["outputblock"].toObject();
    QStringList outputkeys   = outputObject.keys();
    QJsonObject baseObject   = project["baselogicblock"].toObject();
    QStringList basekeys     = baseObject.keys();
    QJsonObject applyObject  = project["applylogicblock"].toObject();
    QStringList applykeys    = applyObject.keys();
    QJsonObject delayObject  = project["delaylogicblock"].toObject();
    QStringList delaykeys    = delayObject.keys();
    QJsonObject speedObject  = project["speedlogicblock"].toObject();
    QStringList speedkeys    = speedObject.keys();
    QJsonObject lineObject   = project["line"].toObject();
    QStringList linekeys     = lineObject.keys();
    for (int i = 0; i < inputkeys.size(); i++) {
        input_block* input = new input_block(inputObject["inputblock" + QString::number(i)].toObject(), mparent);
        scene()->addItem(input);
    }
    for (int i = 0; i < outputkeys.size(); i++) {
        output_block* output = new output_block(outputObject["outputblock" + QString::number(i)].toObject(), mparent);
        scene()->addItem(output);
    }
    for (int i = 0; i < basekeys.size(); i++) {
        base_logic_block* logic =
            new base_logic_block(baseObject["baselogicblock" + QString::number(i)].toObject(), mparent);
        scene()->addItem(logic);
    }

    for (int i = 0; i < applykeys.size(); i++) {
        apply_logic_block* logic =
            new apply_logic_block(applyObject["applylogicblock" + QString::number(i)].toObject(), mparent);
        scene()->addItem(logic);
    }

    for (int i = 0; i < delaykeys.size(); i++) {
        delay_counter_logic_block* logic =
            new delay_counter_logic_block(delayObject["delaylogicblock" + QString::number(i)].toObject(), mparent);
        scene()->addItem(logic);
    }
    for (int i = 0; i < speedkeys.size(); i++) {
        speed_logic_block* logic =
            new speed_logic_block(speedObject["speedlogicblock" + QString::number(i)].toObject(), mparent);
        scene()->addItem(logic);
    }

    for (int i = 0; i < linekeys.size(); i++) {
        connection_line* line    = new connection_line(mparent);
        bool             success = false;
        scene()->addItem(line);
        success = line->connect_line_project_parse(lineObject["line" + QString::number(i)].toObject());
        if (success) {
            connection_line_list.append(line);
        } else {
            scene()->removeItem(line);
        }
    }
    return false;
}

/**
 * @brief 图形化编程复位
 */
void logic_view::logic_view_reset()
{
    m_undostack->clear();
    QList<QGraphicsItem*> allBlocks = my_scene->items();
    foreach (QGraphicsItem* item, allBlocks) {
        my_scene->removeItem(item);
        delete item;
    }
    mainwindow->logic_menu_class->logic_menu_reset();
    connection_line_list.clear();
    input_block_list.clear();
    output_block_list.clear();
    base_logic_block_list.clear();
    rs_trigger_logic_block_list.clear();
    apply_logic_block_list.clear();
    apply_reset_block_list.clear();
    apply_extern_device_monitor_list.clear();
    apply_edge_detected_list.clear();
    apply_freq_monitor_list.clear();
    delay_counter_block_list.clear();
    delay_on_list.clear();
    delay_off_list.clear();
    delay_adjust_on_list.clear();
    delay_adjust_off_list.clear();
    counter_event_list.clear();
    counter_logging_list.clear();
    speed_logic_block_list.clear();
    speed_cross_check_list.clear();
    speed_monitor_list.clear();
    speed_motionless_list.clear();
    speed_value_compairsons_list.clear();
    probe_line      = nullptr;
    draw_line_state = DRAW_LINE_STATE_IDLE;
    selecteditems.clear();
    selectedlines.clear();
}

/**
 * @brief 画连接线的回调，在鼠标点击和松开事件内调用
 * @param block 要操作的连接点
 */
void logic_view::draw_line_both_block(connect_point* block)
{
    bool                  is_success = true;
    static connect_point* last_block = nullptr;
    switch (draw_line_state) {
    case DRAW_LINE_STATE_IDLE:
        if (block) {
            if (!block->allow_connect()) {
                mainwindow->dispaly_status_message("连接点已被连接，无法重复连接", 3000);
            } else if (!block->get_valid_state()) {
                mainwindow->dispaly_status_message("连接点无效，不可连接", 3000);
            } else {
                last_block = block;
                probe_line = new connection_line(mparent);
                my_scene->addItem(probe_line);
                probe_line->set_start_point_block(last_block);
                draw_line_state = DRAW_LINE_STATE_ING;
            }
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
            } else if (!block->allow_connect()) {
                mainwindow->dispaly_status_message("连接点已被连接，无法重复连接", 3000);
                is_success = false;
            } else if (block->get_io_type() == last_block->get_io_type()) {
                mainwindow->dispaly_status_message("输入与输入或输出与输出无法连接", 3000);
                is_success = false;
            } else if (block->get_data_type() != last_block->get_data_type()) {
                mainwindow->dispaly_status_message("输入点的数据类型不一致", 3000);
                is_success = false;
            } else if (block->parents_coincide_detect(&last_block->self_attribute)) {
                mainwindow->dispaly_status_message("禁止连接父节点，将会形成逻辑死循环", 3000);
                is_success = false;
            } else if (!block->get_valid_state()) {
                mainwindow->dispaly_status_message("连接点无效，不可连接", 3000);
                is_success = false;
            }
            if (is_success == false) {
                draw_line_delete(probe_line);
                last_block = nullptr;  //删除第一个选中的块以及删除线
            } else {
                probe_line->set_end_point_block(block);
                connection_line_list.append(probe_line);
                m_undostack->push(new linecommand(this, probe_line, mparent));
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
void logic_view::draw_line_delete(connection_line* line)
{
    if (line) {
        delete line;
        line = nullptr;
    }
}

int logic_view::get_idle_block_uid()
{
    int id     = 1;
    int maxnum = input_block_list.size() + output_block_list.size() + base_logic_block_list.size()
                 + apply_logic_block_list.size() + delay_counter_block_list.size() + speed_logic_block_list.size() + 1;
    QList<QGraphicsItem*> allBlocks = scene()->items();
    for (int i = 1; i <= maxnum; i++) {
        bool id_exist = false;
        foreach (QGraphicsItem* item, allBlocks) {
            if (item->type() >= QGraphicsItem::UserType + BLOCK_TYPE_INPUTBLOCK) {
                base_rect_class* base = dynamic_cast<base_rect_class*>(item);
                if (base) {
                    if (i == base->get_attribute()->uid) {
                        id_exist = true;
                        break;
                    }
                }
            }
        }
        if (id_exist) {
            continue;
        }
        id = i;
        break;
    }
    return id;
}

void logic_view::lines_path_update()
{
    foreach (connection_line* line, connection_line_list) {
        if (line) {
            line->line_path_update();
        }
    }
}

/**
 * @brief 创建块
 * @param tool_info
 * @param pos
 */
void logic_view::creat_logic_block(config_block_data_t* data, QPointF pos)
{
    m_undostack->push(new addcommand(this, data, pos, mparent));
}

/**
 * @brief 检测当前所有的块是否有故障
 * @return true-有故障 false-无故障
 */
bool logic_view::blocks_error_detect()
{
    for (int i = 0; i < input_block_list.size(); i++) {
        if (input_block_list[i]->error_is_exist()) {
            return true;
        }
    }
    for (int i = 0; i < output_block_list.size(); i++) {
        if (output_block_list[i]->error_is_exist()) {
            return true;
        }
    }
    for (int i = 0; i < base_logic_block_list.size(); i++) {
        if (base_logic_block_list[i]->error_is_exist()) {
            return true;
        }
    }
    for (int i = 0; i < apply_logic_block_list.size(); i++) {
        if (apply_logic_block_list[i]->error_is_exist()) {
            return true;
        }
    }
    for (int i = 0; i < delay_counter_block_list.size(); i++) {
        if (delay_counter_block_list[i]->error_is_exist()) {
            return true;
        }
    }
    for (int i = 0; i < speed_logic_block_list.size(); i++) {
        if (speed_logic_block_list[i]->error_is_exist()) {
            return true;
        }
    }
    return false;
}

void logic_view::set_block_focus_ctrl(QPointF pos)
{
    QList<QGraphicsItem*> allBlocks = scene()->items();
    foreach (QGraphicsItem* item, allBlocks) {
        if (item->type() >= QGraphicsItem::UserType + BLOCK_TYPE_INPUTBLOCK) {
            base_rect_class* base = dynamic_cast<base_rect_class*>(item);
            if (base) {
                if (base->sceneBoundingRect().contains(pos)) {
                    if (selecteditems.contains(item)) {
                        selecteditems.removeOne(item);
                        base->set_focus(false);
                    } else {
                        selecteditems.append(item);
                        base->set_focus(true);
                    }
                }
            }
        }
    }
    foreach (connection_line* line, connection_line_list) {
        if (line) {
            if (line->sceneBoundingRect().contains(pos)) {
                if (line->get_focus()) {
                    if (selectedlines.contains(line)) {
                        selectedlines.removeOne(line);
                        line->set_focus(false);
                    }
                    line->set_focus(false);
                } else {
                    selectedlines.append(line);
                    line->set_focus(true);
                }
            }
        }
    }
}

void logic_view::set_block_focus(QPointF pos)
{
    QGraphicsItem* item = scene()->itemAt(pos, QTransform());
    if (selecteditems_contains(item) || selectedlines.contains(item)) {
        return;
    }
    selecteditems.clear();
    selectedlines.clear();
    QList<QGraphicsItem*> allBlocks = scene()->items();
    foreach (QGraphicsItem* item, allBlocks) {
        if (item->type() >= QGraphicsItem::UserType + BLOCK_TYPE_INPUTBLOCK) {
            base_rect_class* base = dynamic_cast<base_rect_class*>(item);
            if (base) {
                if (base->sceneBoundingRect().contains(pos)) {
                    selecteditems.append(item);
                    base->set_focus(true);
                } else {
                    base->set_focus(false);
                }
            }
        }
    }
    foreach (connection_line* line, connection_line_list) {
        if (line) {
            if (line->shape().contains(pos)) {
                selectedlines.append(line);
                line->set_focus(true);
            } else {
                line->set_focus(false);
            }
        }
    }
}

void logic_view::set_block_focus(QRectF rect)
{
    selecteditems.clear();
    selectedlines.clear();
    QList<QGraphicsItem*> allBlocks = scene()->items();
    foreach (QGraphicsItem* item, allBlocks) {
        if (item->type() >= QGraphicsItem::UserType + BLOCK_TYPE_INPUTBLOCK) {
            base_rect_class* base = dynamic_cast<base_rect_class*>(item);
            if (base) {
                if (rect.contains(base->sceneBoundingRect())) {
                    selecteditems.append(item);
                    base->set_focus(true);
                } else {
                    base->set_focus(false);
                }
            }
        }
    }
}

bool logic_view::selecteditems_contains(QGraphicsItem* item)
{
    if (item && item->type() != QGraphicsItem::UserType + BLOCK_TYPE_CONNECT) {
        if (selecteditems.contains(item) || selecteditems.contains(item->parentItem())) {
            return true;
        }
    }
    return false;
}

void logic_view::selecteditems_movepos_start(QPoint pos)
{
    if (selecteditems.isEmpty()) {
        return;
    }
    foreach (QGraphicsItem* item, selecteditems) {
        if (item) {
            base_rect_class* base = dynamic_cast<base_rect_class*>(item);
            if (base) {
                base->movepos_start(pos);
            }
        }
    }
}

void logic_view::selecteditems_movepos_moving(QPoint pos)
{
    if (selecteditems.isEmpty()) {
        return;
    }
    foreach (QGraphicsItem* item, selecteditems) {
        if (item) {
            base_rect_class* base = dynamic_cast<base_rect_class*>(item);
            if (base) {
                base->movepos_moving(pos, selecteditems);
            }
        }
    }
}

void logic_view::selecteditems_movepos_end()
{
    if (selecteditems.isEmpty()) {
        return;
    }
    bool error = false;
    foreach (QGraphicsItem* item, selecteditems) {
        if (item) {
            base_rect_class* base = dynamic_cast<base_rect_class*>(item);
            if (base) {
                if (base->movepos_iserror(selecteditems)) {
                    error = true;
                    break;
                }
            }
        }
    }
    foreach (QGraphicsItem* item, selecteditems) {
        if (item) {
            base_rect_class* base = dynamic_cast<base_rect_class*>(item);
            if (base) {
                if (error) {
                    base->movepos_cancle();
                } else {
                    base->movepos_end();
                }
            }
        }
    }
    if (!error) {
        m_undostack->push(new movecommand(this, selecteditems));
    }
}

void logic_view::selecteditems_delete()
{
    if (selecteditems.isEmpty() && selectedlines.isEmpty()) {
        return;
    }
    m_undostack->push(new deletecommand(this, selecteditems, selectedlines, mparent));
    selecteditems.clear();
    selectedlines.clear();
}

void logic_view::selecteditems_clear()
{
    foreach (QGraphicsItem* item, selecteditems) {
        if (item) {
            base_rect_class* base = dynamic_cast<base_rect_class*>(item);
            if (base) {
                base->set_focus(false);
            }
        }
    }
    selecteditems.clear();
    selectedlines.clear();
}

void logic_view::key_copy_action_callback()
{
    if (selecteditems.isEmpty()) {
        return;
    }
    block_copy_data.clear();
    line_copy_data.clear();
    int x = selecteditems[0]->pos().x(), y = selecteditems[0]->pos().y();
    foreach (QGraphicsItem* item, selecteditems) {
        if (item) {
            base_rect_class* base = dynamic_cast<base_rect_class*>(item);
            if (base) {
                if (base->pos().x() < x) {
                    x = base->pos().x();
                }
                if (base->pos().y() < y) {
                    y = base->pos().y();
                }
            }
        }
    }
    QPoint origin_pos(x, y);
    foreach (QGraphicsItem* item, selecteditems) {
        if (item) {
            base_rect_class* base = dynamic_cast<base_rect_class*>(item);
            if (base) {
                block_copy_data_t block_data;
                block_data.type       = base->type();
                block_data.width      = base->sceneBoundingRect().width();
                block_data.height     = base->sceneBoundingRect().height();
                block_data.block_info = base->block_project_info();
                block_data.block_data = *base->get_config_block_data();
                block_data.offset_pos = (base->pos() - origin_pos);
                block_copy_data.append(block_data);
            }
        }
    }
    foreach (connection_line* line, connection_line_list) {
        if (line) {
            if (selecteditems.contains(line->get_start_point()->parentItem())
                && selecteditems.contains(line->get_end_point()->parentItem())) {
                line_copy_data_t line_data;
                line_data.input_block_id    = selecteditems.indexOf(line->get_start_point()->parentItem());
                line_data.input_point_id    = line->get_start_point()->connect_point_id;
                line_data.input_point_type  = line->get_start_point()->get_io_type();
                line_data.output_block_id   = selecteditems.indexOf(line->get_end_point()->parentItem());
                line_data.output_point_id   = line->get_end_point()->connect_point_id;
                line_data.output_point_type = line->get_end_point()->get_io_type();
                line_copy_data.append(line_data);
            }
        }
    }
}

void logic_view::key_paste_action_callback()
{
    if (block_copy_data.isEmpty()) {
        return;
    }
    int     x = qRound(cursor_pos.x() / 10) * 10;
    int     y = qRound(cursor_pos.y() / 10) * 10;
    QPointF origin_pos(x, y);
    /* 碰撞检测及块数量上限检测 */
    QList<QGraphicsItem*> allBlocks = scene()->items();
    foreach (block_copy_data_t data, block_copy_data) {
        logic_element* source_item = mainwindow->logic_menu_class->get_function_item(data.block_data);
        if (source_item) {
            if (source_item->isDisabled()) {
                mainwindow->my_message_box("\"" + source_item->get_config_data()->source_name + "\"可放置数量已达上限",
                                           MESSAGE_TYPE_ERROR);
                return;
            }
        }
        QPointF point = origin_pos + data.offset_pos;
        QRect   rect(point.x(), point.y(), data.width, data.height);
        foreach (QGraphicsItem* item, allBlocks) {
            if (item->type() >= QGraphicsItem::UserType + BLOCK_TYPE_INPUTBLOCK) {
                base_rect_class* base = dynamic_cast<base_rect_class*>(item);
                if (base) {
                    if (base->collison_detect(rect)) {
                        mainwindow->my_message_box("此处不可放置块", MESSAGE_TYPE_ERROR);
                        return;
                    }
                }
            }
        }
    }
    /* 粘贴 */
    m_undostack->push(new pastecommand(this, origin_pos, block_copy_data, line_copy_data, mparent));
}

/* user slots */

void logic_view::item_delete_slot()
{
    selecteditems_delete();
}

void logic_view::item_contexmenu_slot(QGraphicsItem* item)
{
    if (selecteditems_contains(item) || selectedlines.contains(item)) {
        return;
    }
    foreach (QGraphicsItem* item, selecteditems) {
        base_rect_class* base = dynamic_cast<base_rect_class*>(item);
        if (base) {
            base->set_focus(false);
        }
    }
    foreach (QGraphicsItem* item, selectedlines) {
        connection_line* line = dynamic_cast<connection_line*>(item);
        if (line) {
            line->set_focus(false);
        }
    }
    selecteditems.clear();
    selectedlines.clear();
    if (item->type() == QGraphicsItem::UserType + BLOCK_TYPE_LINE) {
        connection_line* line = dynamic_cast<connection_line*>(item);
        if (line) {
            line->set_focus(true);
        }
        selectedlines.append(item);
    } else {
        base_rect_class* base = dynamic_cast<base_rect_class*>(item);
        if (base) {
            base->set_focus(true);
        }
        selecteditems.append(item);
    }
}

void logic_view::item_param_change_slot(QGraphicsItem* item)
{
    m_undostack->push(new paramchangecommand(this, item));
}

/* sys slots */
/**
 * @brief 拖拽接收事件
 * @param event
 */
void logic_view::dragEnterEvent(QDragEnterEvent* event)
{
    if (mainwindow->project_debug_class->get_debug_state() == DEBUG_STATE_IDLE) {
        if (event->mimeData()->hasFormat("ruohui/logic")) {
            event->acceptProposedAction();
            QByteArray           byteArray   = event->mimeData()->data("ruohui/logic");
            config_block_data_t* config_data = *reinterpret_cast<config_block_data_t* const*>(byteArray.constData());
            /* 根据拖拽的块不同设置虚拟块的大小 */
            if (config_data->config_param_data.model_iotype == MODEL_TYPE_LOGIC) {
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
}

/**
 * @brief 拖拽移动事件
 * @param event
 */
void logic_view::dragMoveEvent(QDragMoveEvent* event)
{
    if (mainwindow->project_debug_class->get_debug_state() == DEBUG_STATE_IDLE) {
        if (event->mimeData()->hasFormat("ruohui/logic")) {
            QPointF pos = mapToScene(event->pos());  // 获取全局位置
            if (drop_tool_info.probe_rect != nullptr) {
                drop_tool_info.probe_rect->setPos(pos.x() - drop_tool_info.width / 2,
                                                  pos.y() - drop_tool_info.height / 2);
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
                if (item->type() >= QGraphicsItem::UserType + BLOCK_TYPE_INPUTBLOCK) {
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
}

/**
 * @brief 拖拽放弃事件
 * @param event
 */
void logic_view::dragLeaveEvent(QDragLeaveEvent* event)
{
    Q_UNUSED(event);
    if (mainwindow->project_debug_class->get_debug_state() == DEBUG_STATE_IDLE) {
        if (drop_tool_info.probe_rect != nullptr) {
            my_scene->removeItem(drop_tool_info.probe_rect);
            drop_tool_info.probe_rect = nullptr;
            drop_tool_info.is_valid   = false;
        }
    }
}

/**
 * @brief 拖拽完成事件
 * @param event
 */
void logic_view::dropEvent(QDropEvent* event)
{
    if (mainwindow->project_debug_class->get_debug_state() == DEBUG_STATE_IDLE) {
        if (event->mimeData()->hasFormat("ruohui/logic")) {
            if (drop_tool_info.is_valid) {
                QByteArray           byteArray = event->mimeData()->data("ruohui/logic");
                config_block_data_t* config_data =
                    *reinterpret_cast<config_block_data_t* const*>(byteArray.constData());
                QPointF pos = mapToScene(event->pos());  // 获取全局位置
                creat_logic_block(config_data, pos);
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
}

/**
 * @brief 鼠标滚轮事件
 * @param event
 */
void logic_view::wheelEvent(QWheelEvent* event)
{
    int delta = event->delta();                      //获取鼠标滑轮滚动的距离
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
    if (mainwindow->project_debug_class->get_debug_state() == DEBUG_STATE_IDLE) {
        if (event->button() == Qt::LeftButton && draw_line_state == DRAW_LINE_STATE_IDLE) {
            // 将鼠标事件的位置从视图坐标系转换为场景坐标系
            QPointF scenePos   = mapToScene(event->pos());
            rubberbandstartpos = event->pos();
            // 在场景中查找图形项
            QGraphicsItem* item = scene()->itemAt(scenePos, QTransform());
            if (item) {
                if ((item->type() == QGraphicsItem::UserType + BLOCK_TYPE_CONNECT)) {
                    connect_point* otherBlock = dynamic_cast<connect_point*>(item);
                    draw_line_both_block(otherBlock);
                }
            } else {
                /* 创建多选框 */
                selecteditems.clear();
                selectedlines.clear();
                rubberband = new QRubberBand(QRubberBand::Rectangle, this);
                rubberband->setGeometry(QRect(rubberbandstartpos, QSize()));
                rubberband->show();
            }
            /* 通过位置选模块 */
            if (event->modifiers() & Qt::ControlModifier) {
                set_block_focus_ctrl(scenePos);
            } else {
                set_block_focus(scenePos);
                selecteditems_movepos_start(event->pos());
            }
        }
    }
    QGraphicsView::mousePressEvent(event);
}

void logic_view::mouseReleaseEvent(QMouseEvent* event)
{
    if (mainwindow->project_debug_class->get_debug_state() == DEBUG_STATE_IDLE) {
        if (event->button() == Qt::LeftButton) {
            QPointF scenePos = mapToScene(event->pos());
            if (draw_line_state == DRAW_LINE_STATE_ING && probe_line != nullptr) {
                QGraphicsItem* item       = scene()->itemAt(scenePos, QTransform());
                connect_point* otherBlock = dynamic_cast<connect_point*>(item);
                if (item && item->type() == QGraphicsItem::UserType + BLOCK_TYPE_CONNECT) {
                    draw_line_both_block(otherBlock);
                } else {
                    draw_line_both_block(nullptr);
                }
            }
            if (rubberband) {
                QRectF rubberBandRect = mapToScene(rubberband->geometry()).boundingRect();
                set_block_focus(rubberBandRect);
                delete rubberband;
                rubberband = nullptr;
            } else {
                selecteditems_movepos_end();
            }
        }
    }
    QGraphicsView::mouseReleaseEvent(event);
}

void logic_view::mouseMoveEvent(QMouseEvent* event)
{
    if (mainwindow->project_debug_class->get_debug_state() == DEBUG_STATE_IDLE) {
        cursor_pos = mapToScene(event->pos());
        if (event->buttons() & Qt::LeftButton) {
            if (draw_line_state == DRAW_LINE_STATE_ING && probe_line != nullptr) {
                probe_line->set_end_point(mapToScene(event->pos()));
            }
            if (rubberband) {
                rubberband->setGeometry(QRect(rubberbandstartpos, event->pos()).normalized());
            } else {
                selecteditems_movepos_moving(event->pos());
            }
        }
    }
    QGraphicsView::mouseMoveEvent(event);
}

void logic_view::keyPressEvent(QKeyEvent* event)
{
    if (mainwindow->project_debug_class->get_debug_state() == DEBUG_STATE_IDLE) {
        if (event->key() == Qt::Key_Delete) {
            selecteditems_delete();
        }
        if (event->matches(QKeySequence::Copy)) {
            key_copy_action_callback();
        } else if (event->matches(QKeySequence::Paste)) {
            key_paste_action_callback();
        } else if (event->matches(QKeySequence::Undo)) {
            if (m_undostack->canUndo()) {
                selecteditems_clear();
                m_undostack->undo();
            }
        } else if (event->matches(QKeySequence::Redo)) {
            if (m_undostack->canRedo()) {
                selecteditems_clear();
                m_undostack->redo();
            }
        }
    }
    QGraphicsView::keyPressEvent(event);
}
