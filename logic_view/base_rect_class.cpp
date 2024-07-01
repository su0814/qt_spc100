#include "base_rect_class.h"
#include "mainwindow.h"
#include <QStringList>

base_rect_class::base_rect_class(qreal x, qreal y, qreal w, qreal h, QWidget* uiparent, QGraphicsItem* parent)
    : QGraphicsRectItem(x, y, w, h, parent)
{
    mainwindow = ( MainWindow* )uiparent;
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsFocusable);
    setFocus();
    display_name = new QGraphicsTextItem(this);
    display_name->setTextWidth(w);
    display_uid        = new QGraphicsTextItem(this);
    display_pixmap     = new QGraphicsPixmapItem(this);
    QTextOption option = display_name->document()->defaultTextOption();  //取出当前设置
    option.setAlignment(Qt::AlignHCenter);                   //对设置进行修改，增加居中对齐设置
    display_name->document()->setDefaultTextOption(option);  //重新设定
    attribute_data.config_block_data = &config_block_data;
    for (int i = 0; i < MAX_CONNECT_POINT_NUM; i++) {
        connect_point* opoint =
            new connect_point(boundingRect().width(), (boundingRect().height() / (MAX_CONNECT_POINT_NUM)) * (i),
                              CONNECT_POINT_IOTYPE_OUTPUT, i, &attribute_data, this);
        output_point_list.append(opoint);
        connect_point* ipoint =
            new connect_point(-CONNECT_POINT_WIDTH, (boundingRect().height() / (MAX_CONNECT_POINT_NUM)) * (i),
                              CONNECT_POINT_IOTYPE_INPUT, i, &attribute_data, this);
        input_point_list.append(ipoint);
        sys_input_point_label.append("输入" + QString::number(i + 1));
        input_point_list[i]->set_label(sys_input_point_label[i]);
        sys_output_point_label.append("输出" + QString::number(i + 1));
        output_point_list[i]->set_label(sys_output_point_label[i]);
        block_base_param.user_output_point_label.append("");
        block_base_param.user_input_point_label.append("");
    }
    set_input_mask(0);
    set_output_mask(0);
    set_pen_state(BLOCK_STATE_IDE);
    set_brush_state(BLOCK_STATE_ERROR);
    connect(this, block_delete_signal, mainwindow->logic_view_class, &logic_view::item_delete_slot);
    connect(this, block_contexmenu_signal, mainwindow->logic_view_class, &logic_view::item_contexmenu_slot);
    connect(this, block_param_change_signal, mainwindow->logic_view_class, &logic_view::item_param_change_slot);
}

base_rect_class::~base_rect_class() {}

void base_rect_class::set_pen_state(block_state_e state)
{
    QPen pen;
    switch (state) {
    case BLOCK_STATE_IDE:
        pen.setColor(QColor(128, 128, 128));
        pen.setWidth(1);
        this->setPen(pen);
        break;
    case BLOCK_STATE_DEBUG:
        pen.setColor(QColor(0, 200, 0));
        pen.setWidth(2);
        setPen(pen);
        break;
    case BLOCK_STATE_FOCUS:
        pen.setColor(QColor(0, 0, 255));
        pen.setWidth(2);
        this->setPen(pen);
        break;
    default:
        break;
    }
}

void base_rect_class::set_brush_state(block_state_e state)
{
    switch (state) {
    case BLOCK_STATE_IDE: {
        QBrush brush(QColor(240, 206, 14));
        this->setBrush(brush);
    } break;
    case BLOCK_STATE_ERROR: {
        QBrush brush(QColor(242, 129, 13));
        setBrush(brush);
    } break;
    default:
        break;
    }
}

void base_rect_class::set_focus(bool state)
{
    focus_state = state;
    if (mainwindow->project_debug_class->get_debug_state() == DEBUG_STATE_IDLE) {
        if (focus_state) {
            set_pen_state(BLOCK_STATE_FOCUS);
        } else {
            set_pen_state(BLOCK_STATE_IDE);
        }
        foreach (connect_point* item, output_point_list) {
            item->send_focus_state_signal(focus_state);
        }
        foreach (connect_point* item, input_point_list) {
            item->send_focus_state_signal(focus_state);
        }
    } else {
        set_pen_state(BLOCK_STATE_IDE);
    }
}

void base_rect_class::set_current_pos(QPointF pos)
{
    setPos(pos);
    //    foreach (QGraphicsItem* child, childItems()) {
    //        connect_point* connectBlock = dynamic_cast<connect_point*>(child);
    //        if (connectBlock) {
    //            connectBlock->position_change();
    //        }
    //    }
}

void base_rect_class::set_display_name(QString name)
{
    display_name->setPlainText(name);
    display_name->setPos(this->boundingRect().center().x() - display_name->boundingRect().center().x(),
                         this->boundingRect().center().y() - display_name->boundingRect().center().y());
}

void base_rect_class::set_display_pixmap(QString name)
{
    display_pixmap->setPixmap(QPixmap(name).scaled(40, 40));
    display_pixmap->setPos(this->boundingRect().center().x() - display_pixmap->boundingRect().center().x(),
                           this->boundingRect().center().y() - display_pixmap->boundingRect().center().y() - 30);
}

void base_rect_class::set_display_uid(int id)
{
    display_uid->setPlainText(QString::number(id));
    display_uid->setPos(this->boundingRect().center().x() - display_uid->boundingRect().center().x(),
                        this->boundingRect().center().y() - display_uid->boundingRect().center().y() - 60);
}

void base_rect_class::set_sys_inputpoint_labels(QStringList labels)
{
    int num = labels.size() > MAX_CONNECT_POINT_NUM ? MAX_CONNECT_POINT_NUM : labels.size();
    for (int i = 0; i < num; i++) {
        QString str = labels[i];
        if (str.isEmpty()) {
            sys_input_point_label[i] = "输入" + QString::number(i);
        } else {
            sys_input_point_label[i] = labels[i];
        }
        input_point_list[i]->set_label(sys_input_point_label[i]);
    }
}

void base_rect_class::set_sys_outputpoint_labels(QStringList labels)
{
    int num = labels.size() > MAX_CONNECT_POINT_NUM ? MAX_CONNECT_POINT_NUM : labels.size();
    for (int i = 0; i < num; i++) {
        QString str = labels[i];
        if (str.isEmpty()) {
            sys_output_point_label[i] = "输出" + QString::number(i);
        } else {
            sys_output_point_label[i] = labels[i];
        }
        output_point_list[i]->set_label(sys_output_point_label[i]);
    }
}

void base_rect_class::set_connect_point_labels_visible(bool state)
{
    for (int i = 0; i < MAX_CONNECT_POINT_NUM; i++) {
        output_point_list[i]->set_label_visible(state);
        input_point_list[i]->set_label_visible(state);
    }
}

void base_rect_class::set_user_inputpoint_labels(QStringList labels)
{
    int num = labels.size() > MAX_CONNECT_POINT_NUM ? MAX_CONNECT_POINT_NUM : labels.size();
    for (int i = 0; i < num; i++) {
        QString str                                = labels[i];
        block_base_param.user_input_point_label[i] = labels[i];
        if (str.isEmpty()) {
            input_point_list[i]->set_label(sys_input_point_label[i]);
        } else {
            input_point_list[i]->set_label(block_base_param.user_input_point_label[i]);
        }
    }
}

void base_rect_class::set_user_outputpoint_labels(QStringList labels)
{
    int num = labels.size() > MAX_CONNECT_POINT_NUM ? MAX_CONNECT_POINT_NUM : labels.size();
    for (int i = 0; i < num; i++) {
        QString str                                 = labels[i];
        block_base_param.user_output_point_label[i] = labels[i];
        if (str.isEmpty()) {
            output_point_list[i]->set_label(sys_output_point_label[i]);
        } else {
            output_point_list[i]->set_label(block_base_param.user_output_point_label[i]);
        }
    }
}

void base_rect_class::set_input_reverse_data(int data)
{
    block_base_param.input_reverse_data = data;
    for (int i = 0; i < 8; i++) {
        if (data & (0x01 << i)) {
            input_point_list[i]->set_reserve(true);
        } else {
            input_point_list[i]->set_reserve(false);
        }
    }
}

bool base_rect_class::set_input_mask(int mask)
{
    if (mask < 0 || mask > 0xff) {
        return false;
    }
    int num = 0;
    for (int i = 0; i < 8; i++) {
        if (mask & (0x01 << i)) {
            num++;
        } else {
            if (input_point_list[i]->connect_is_created()) {
                num++;
            }
        }
    }
    int count = 0;
    for (int i = 0; i < MAX_CONNECT_POINT_NUM; i++) {
        if (mask & (0x01 << i)) {
            input_point_list[i]->set_enable(true);
            input_point_list[i]->setPos(-CONNECT_POINT_WIDTH, ((boundingRect().height() - 5) / (num)) * (count) + 5);
            count++;
        } else {
            if (input_point_list[i]->connect_is_created()) {
                input_point_list[i]->setPos(-CONNECT_POINT_WIDTH,
                                            ((boundingRect().height() - 5) / (num)) * (count) + 5);
                count++;
            } else {
                input_point_list[i]->setPos(-CONNECT_POINT_WIDTH, (boundingRect().height() - CONNECT_POINT_WIDTH));
            }
            input_point_list[i]->set_enable(false);
        }
        input_point_list[i]->position_change();
    }
    block_base_param.input_point_mask = mask;
    return true;
}

bool base_rect_class::set_output_mask(int mask)
{
    if (mask < 0 || mask > 0xff) {
        return false;
    }
    int num = 0;
    for (int i = 0; i < 8; i++) {
        if (mask & (0x01 << i)) {
            num++;
        } else {
            if (output_point_list[i]->connect_is_created()) {
                num++;
            }
        }
    }
    int count = 0;
    for (int i = 0; i < MAX_CONNECT_POINT_NUM; i++) {
        if (mask & (0x01 << i)) {
            output_point_list[i]->set_enable(true);
            output_point_list[i]->setPos(boundingRect().width(), ((boundingRect().height() - 5) / (num)) * (count) + 5);
            count++;
        } else {
            if (output_point_list[i]->connect_is_created()) {
                output_point_list[i]->setPos(boundingRect().width(),
                                             ((boundingRect().height() - 5) / (num)) * (count) + 5);
                count++;
            } else {
                output_point_list[i]->setPos(boundingRect().width(), (boundingRect().height() - CONNECT_POINT_WIDTH));
            }
            output_point_list[i]->set_enable(false);
        }
        output_point_list[i]->position_change();
    }
    block_base_param.output_point_mask = mask;
    return true;
}

void base_rect_class::set_right_menu_action(int mode)
{
    static bool is_set = false;
    if (is_set || mode == ACTION_NONE) {
        return;
    }
    right_menu_mode = mode;
    if (mode & ACTION_SET) {
        setaction = new QAction(QIcon(":/new/photo/photo/setting_block.png"), "设置", this);
        menu.addAction(setaction);
    }
    if (mode & ACTION_DELETE) {
        deleteaction = new QAction(QIcon(":/new/photo/photo/delete_block.png"), "删除", this);
        menu.addAction(deleteaction);
    }
}

attribute_t* base_rect_class::get_attribute()
{
    return (&attribute_data);
}

int base_rect_class::get_input_reverse_data()
{
    return block_base_param.input_reverse_data;
}

bool base_rect_class::get_focus_state()
{
    return focus_state;
}

QPointF base_rect_class::get_old_pos()
{
    return old_pos;
}

QPointF base_rect_class::get_new_pos()
{
    return new_pos;
}

bool base_rect_class::error_is_exist()
{
    return error;
}

config_block_data_t* base_rect_class::get_config_block_data()
{
    return &config_block_data;
}

int base_rect_class::get_input_connected_num()
{
    int num = 0;
    foreach (connect_point* point, input_point_list) {
        if (point->connect_is_created()) {
            num++;
        }
    }
    return num;
}

int base_rect_class::get_output_connected_num()
{
    int num = 0;
    foreach (connect_point* point, output_point_list) {
        if (point->connect_is_created()) {
            num++;
        }
    }
    return num;
}

QStringList base_rect_class::get_sys_inputpoint_labels()
{
    return sys_input_point_label;
}

QStringList base_rect_class::get_sys_outputpoint_labels()
{
    return sys_output_point_label;
}

QStringList base_rect_class::get_user_inputpoint_labels()
{
    return block_base_param.user_input_point_label;
}

QStringList base_rect_class::get_user_outputpoint_labels()
{
    return block_base_param.user_output_point_label;
}

int base_rect_class::get_input_point_number()
{
    int num = 0;
    for (int i = 0; i < MAX_CONNECT_POINT_NUM; i++) {
        if (block_base_param.input_point_mask & (0x01 << i)) {
            num++;
        }
    }
    return num;
}

int base_rect_class::get_output_point_number()
{
    int num = 0;
    for (int i = 0; i < MAX_CONNECT_POINT_NUM; i++) {
        if (block_base_param.output_point_mask & (0x01 << i)) {
            num++;
        }
    }
    return num;
}

int base_rect_class::get_input_point_mask()
{
    return block_base_param.input_point_mask;
}

int base_rect_class::get_output_point_mask()
{
    return block_base_param.output_point_mask;
}

void base_rect_class::send_attribute_data()
{
    for (int i = 0; i < MAX_CONNECT_POINT_NUM; i++) {
        if (block_base_param.output_point_mask & (0x01 << i)) {
            output_point_list[i]->send_block_attribute();
        }
    }
}

void base_rect_class::set_outputpoint_attribute(attribute_t* attribute)
{
    foreach (connect_point* point, output_point_list) {
        point->set_attribute(attribute);
    }
}

void base_rect_class::set_outputpoint_attribute(attribute_t* attribute, int id)
{
    if (id > MAX_CONNECT_POINT_NUM || id < MIN_CONNECT_POINT_NUM) {
        return;
    }
    output_point_list[id]->set_attribute(attribute);
}

void base_rect_class::set_inputpoint_attribute(attribute_t* attribute)
{
    foreach (connect_point* point, input_point_list) {
        point->set_attribute(attribute);
    }
}

void base_rect_class::set_inputpoint_attribute(attribute_t* attribute, int id)
{
    if (id > MAX_CONNECT_POINT_NUM || id < MIN_CONNECT_POINT_NUM) {
        return;
    }
    input_point_list[id]->set_attribute(attribute);
}

void base_rect_class::set_inputpoint_data_type(connect_point_datatype_e type, int id)
{
    if (id > MAX_CONNECT_POINT_NUM || id < MIN_CONNECT_POINT_NUM) {
        return;
    }
    input_point_list[id]->set_data_type(type);
}

void base_rect_class::set_outputpoint_data_type(connect_point_datatype_e type, int id)
{
    if (id > MAX_CONNECT_POINT_NUM || id < MIN_CONNECT_POINT_NUM) {
        return;
    }
    output_point_list[id]->set_data_type(type);
}

void base_rect_class::error_detect()
{
    error = false;
    /* resource detect */
    if (config_block_data.config_param_data.model_iotype != MODEL_TYPE_LOGIC) {
        logic_element* source_item = mainwindow->logic_menu_class->item_exist_detect(config_block_data);
        if (source_item == nullptr) {
            error = true;
        } else {
            config_block_data = *source_item->get_config_data();
            config_block_data_update();
        }
    }

    /* output detect */
    for (int i = 0; i < MAX_CONNECT_POINT_NUM; i++) {
        if (!output_point_list[i]->get_valid_state()) {
            error = true;
            break;
        }
    }

    /* input detect */
    for (int i = 0; i < MAX_CONNECT_POINT_NUM; i++) {
        if (block_base_param.input_point_mask & (0x01 << i)) {
            if (!input_point_list[i]->connect_is_created()) {
                error = true;
                break;
            }
        } else {
            if (!input_point_list[i]->get_valid_state()) {
                error = true;
                break;
            }
        }
    }
    if (error) {
        set_brush_state(BLOCK_STATE_ERROR);
    } else {
        set_brush_state(BLOCK_STATE_IDE);
    }
}

/**
 * @brief 仿真模式下设置仿真数据
 * @param res 模块条件成立仿真数据
 */
void base_rect_class::debug_data_set(uint8_t res)
{
    if (mainwindow->project_debug_class->get_debug_state() == DEBUG_STATE_ING) {
        debug_data_parse(res);
    }
}

void base_rect_class::debug_data_parse(uint8_t res)
{
    foreach (connect_point* item, output_point_list) {
        item->send_debug_data(res);
    }
    if (res == 1) {
        set_pen_state(BLOCK_STATE_DEBUG);
    } else {
        set_pen_state(BLOCK_STATE_IDE);
    }
}

/**
 * @brief 快碰撞检测
 * @return
 */
bool base_rect_class::block_collison_detect(QRectF rect)
{
    // 获取当前块的边界矩形
    QRectF currentRect = rect;
    if ((currentRect.x() < SCENE_MARGIN_MIN || currentRect.x() > SCENE_MARGIN_MAX)
        || (currentRect.y() < SCENE_MARGIN_MIN || currentRect.y() > SCENE_MARGIN_MAX)) {
        return true;
    }
    // 遍历所有块
    QList<QGraphicsItem*> allBlocks = scene()->items();
    foreach (QGraphicsItem* item, allBlocks) {
        if (item == this || childItems().contains(item))
            continue;

        if (item->type() >= QGraphicsItem::UserType + BLOCK_TYPE_INPUTBLOCK) {
            // 获取其他块的边界矩形
            QRectF otherRect = item->sceneBoundingRect();
            // 判断是否与其他块及其周围一定距离范围内重叠
            if (currentRect.intersects(otherRect.adjusted(-BLOCK_SPCING_LEFT, -BLOCK_SPCING_TOP, BLOCK_SPCING_RIGHT,
                                                          BLOCK_SPCING_BOTTOM))) {
                return true;
            }
        }
    }
    return false;
}

bool base_rect_class::collison_detect(QRectF rect)
{
    // 获取当前块的边界矩形
    QRectF currentRect = rect;
    if ((currentRect.x() < SCENE_MARGIN_MIN || currentRect.x() > SCENE_MARGIN_MAX)
        || (currentRect.y() < SCENE_MARGIN_MIN || currentRect.y() > SCENE_MARGIN_MAX)) {
        return true;
    }
    if (currentRect.intersects(this->sceneBoundingRect().adjusted(-BLOCK_SPCING_LEFT, -BLOCK_SPCING_TOP,
                                                                  BLOCK_SPCING_RIGHT, BLOCK_SPCING_BOTTOM))) {
        return true;
    }
    return false;
}

bool base_rect_class::block_collison_detect(QRectF rect, QList<QGraphicsItem*> selections)
{
    // 获取当前块的边界矩形
    QRectF currentRect = rect;
    if ((currentRect.x() < SCENE_MARGIN_MIN || currentRect.x() > SCENE_MARGIN_MAX)
        || (currentRect.y() < SCENE_MARGIN_MIN || currentRect.y() > SCENE_MARGIN_MAX)) {
        return true;
    }
    // 遍历所有块
    QList<QGraphicsItem*> allBlocks = scene()->items();
    foreach (QGraphicsItem* item, allBlocks) {
        if (item == this || childItems().contains(item) || selections.contains(item))
            continue;

        if (item->type() >= QGraphicsItem::UserType + BLOCK_TYPE_INPUTBLOCK) {
            // 获取其他块的边界矩形
            QRectF otherRect = item->sceneBoundingRect();
            // 判断是否与其他块及其周围一定距离范围内重叠
            if (currentRect.intersects(otherRect.adjusted(-BLOCK_SPCING_LEFT, -BLOCK_SPCING_TOP, BLOCK_SPCING_RIGHT,
                                                          BLOCK_SPCING_BOTTOM))) {
                return true;
            }
        }
    }
    return false;
}

void base_rect_class::movepos_start(QPointF movepos)
{
    setCursor(Qt::ClosedHandCursor);  // 设置鼠标样式为手掌抓起
    // 记录块的原始位置
    temp_rect = new QGraphicsRectItem(rect());
    temp_rect->setVisible(false);
    temp_rect->setPos(this->pos());
    scene()->addItem(temp_rect);
    temp_rect->setOpacity(0.3);
    pos_offset = this->sceneBoundingRect().center() - mapToScene(movepos);
    old_pos    = temp_rect->scenePos();
}

void base_rect_class::movepos_moving(QPointF movepos, QList<QGraphicsItem*> selections)
{
    if (temp_rect) {
        QPointF pos = mapToScene(movepos) + pos_offset;
        int     x   = qRound(pos.x() / 10) * 10;
        int     y   = qRound(pos.y() / 10) * 10;
        temp_rect->setPos(x - temp_rect->rect().width() / 2, y - temp_rect->rect().height() / 2);
        temp_rect->setVisible(true);
        if (block_collison_detect(temp_rect->sceneBoundingRect(), selections)) {
            temp_rect->setBrush(Qt::red);
        } else {
            temp_rect->setBrush(Qt::green);
        }
    }
}

bool base_rect_class::movepos_iserror(QList<QGraphicsItem*> selections)
{
    /* 没移动或者目的坐标有其他块都认为失败 */
    if (temp_rect) {
        if (old_pos == temp_rect->scenePos() || block_collison_detect(temp_rect->sceneBoundingRect(), selections)) {
            return true;
        }
    } else {
        return true;
    }
    return false;
}

void base_rect_class::movepos_cancle()
{
    if (temp_rect) {
        scene()->removeItem(temp_rect);
        temp_rect = nullptr;
    }
}

void base_rect_class::movepos_end()
{
    if (temp_rect) {
        new_pos = temp_rect->scenePos();
        // set_current_pos(new_pos);
        movepos_cancle();
    }
}

block_base_param_t base_rect_class::get_block_base_param()
{
    return block_base_param;
}

void base_rect_class::set_block_old_param(QJsonObject param)
{
    old_param = param;
}

QJsonObject base_rect_class::get_block_old_param()
{
    return old_param;
}

void base_rect_class::send_param_change_signal()
{
    emit block_param_change_signal(this);
}
/* user slot */

/* sys event */
void base_rect_class::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    Q_UNUSED(event);
}

void base_rect_class::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    Q_UNUSED(event);
}

void base_rect_class::mousePressEvent(QGraphicsSceneMouseEvent* event)
{

    QGraphicsRectItem::mousePressEvent(event);
}

void base_rect_class::keyPressEvent(QKeyEvent* event)
{

    QGraphicsItem::keyPressEvent(event);
}

void base_rect_class::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
    emit block_contexmenu_signal(this);
    if (right_menu_mode != ACTION_NONE) {
        QAction* selectedItem = menu.exec(event->screenPos());
        if (selectedItem == nullptr) {
            return;
        }
        if (selectedItem == deleteaction) {
            emit block_delete_signal();

        } else if (selectedItem == setaction) {
            action_set_callback();
        }
    }
}

void base_rect_class::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        if (temp_rect) {
            scene()->removeItem(temp_rect);
            temp_rect = nullptr;
        }
        if (right_menu_mode & ACTION_SET) {
            action_set_callback();
        }
    }
}
