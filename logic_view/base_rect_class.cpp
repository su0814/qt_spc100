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
        user_output_point_label.append("");
        user_input_point_label.append("");
    }
    set_input_num(0);
    set_output_num(0);
    set_pen_state(BLOCK_STATE_IDE);
    set_brush_state(BLOCK_STATE_IDE);
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
        QString str               = labels[i];
        user_input_point_label[i] = labels[i];
        if (str.isEmpty()) {
            input_point_list[i]->set_label(sys_input_point_label[i]);
        } else {
            input_point_list[i]->set_label(user_input_point_label[i]);
        }
    }
}

void base_rect_class::set_user_outputpoint_labels(QStringList labels)
{
    int num = labels.size() > MAX_CONNECT_POINT_NUM ? MAX_CONNECT_POINT_NUM : labels.size();
    for (int i = 0; i < num; i++) {
        QString str                = labels[i];
        user_output_point_label[i] = labels[i];
        if (str.isEmpty()) {
            output_point_list[i]->set_label(sys_output_point_label[i]);
        } else {
            output_point_list[i]->set_label(user_output_point_label[i]);
        }
    }
}

void base_rect_class::set_input_reverse_data(int data)
{
    attribute_data.input_reverse_data = data;
}

bool base_rect_class::set_input_num(int num)
{
    if (num < get_input_connected_num() || num > MAX_CONNECT_POINT_NUM) {
        return false;
    }
    QList<connect_point*> point_list;
    foreach (connect_point* point, input_point_list) {
        if (!point->connect_is_created()) {
            point_list.append(point);
            input_point_list.removeOne(point);
        }
    }
    input_point_list.append(point_list);
    for (int i = 0; i < input_point_list.size(); i++) {
        input_point_list[i]->set_label(sys_input_point_label[i]);
        if (i < num) {
            input_point_list[i]->setVisible(true);
        } else {
            input_point_list[i]->setVisible(false);
        }
        input_point_list[i]->setPos(-CONNECT_POINT_WIDTH, ((boundingRect().height() - 5) / (num)) * (i) + 5);
        input_point_list[i]->position_change();
    }
    input_point_num = num;
    return true;
}

bool base_rect_class::set_output_num(int num)
{
    if (num < get_output_connected_num() || num > MAX_CONNECT_POINT_NUM) {
        return false;
    }
    QList<connect_point*> point_list;
    foreach (connect_point* point, output_point_list) {
        if (!point->connect_is_created()) {
            point_list.append(point);
            output_point_list.removeOne(point);
        }
    }
    output_point_list.append(point_list);
    for (int i = 0; i < output_point_list.size(); i++) {
        output_point_list[i]->set_label(sys_output_point_label[i]);
        if (i < num) {
            output_point_list[i]->setVisible(true);
        } else {
            output_point_list[i]->setVisible(false);
        }
        output_point_list[i]->setPos(boundingRect().width(), ((boundingRect().height() - 5) / (num)) * (i) + 5);
        output_point_list[i]->position_change();
    }
    output_point_num = num;
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
    return attribute_data.input_reverse_data;
}

bool base_rect_class::get_focus_state()
{
    return focus_state;
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
    return user_input_point_label;
}

QStringList base_rect_class::get_user_outputpoint_labels()
{
    return user_output_point_label;
}

int base_rect_class::get_input_point_num()
{
    return input_point_num;
}

int base_rect_class::get_output_point_num()
{
    return output_point_num;
}

void base_rect_class::send_attribute_data()
{
    for (int i = 0; i < output_point_num; i++) {
        output_point_list[i]->send_block_attribute();
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
    //    for (int i = 0; i < output_point_num; i++) {
    //        if (!output_point_list[i]->connect_is_created()) {
    //            error = true;
    //            break;
    //        }
    //    }

    /* input detect */
    for (int i = 0; i < input_point_num; i++) {
        if (!input_point_list[i]->connect_is_created()) {
            error = true;
            break;
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
    if (res) {
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

        if (item->type() >= QGraphicsItem::UserType + BLOCK_TYPE_INPUTBLOCK
            && item->type() <= QGraphicsItem::UserType + BLOCK_TYPE_APPLYLOGIC) {
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
/* sys event */
void base_rect_class::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (temp_rect) {
        QPointF pos = mapToScene(event->pos()) + pos_offset;
        int     x   = qRound(pos.x() / 10) * 10;
        int     y   = qRound(pos.y() / 10) * 10;
        temp_rect->setPos(x - temp_rect->rect().width() / 2, y - temp_rect->rect().height() / 2);
        temp_rect->setVisible(true);
        if (block_collison_detect(temp_rect->sceneBoundingRect())) {
            temp_rect->setBrush(Qt::red);
        } else {
            temp_rect->setBrush(Qt::green);
        }
    }
}

void base_rect_class::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    setCursor(Qt::ArrowCursor);  // 设置鼠标样式为箭头
    QGraphicsRectItem::mouseReleaseEvent(event);
    if (temp_rect) {
        if (block_collison_detect(temp_rect->sceneBoundingRect())) {
            mainwindow->dispaly_status_message("此处已有其他块，禁止在此处放置", 3000);
        } else {
            setPos(temp_rect->scenePos());
            foreach (QGraphicsItem* child, childItems()) {
                connect_point* connectBlock = dynamic_cast<connect_point*>(child);
                if (connectBlock) {
                    connectBlock->position_change();
                }
            }
        }
        scene()->removeItem(temp_rect);
        temp_rect = nullptr;
    }
}

void base_rect_class::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        setCursor(Qt::ClosedHandCursor);  // 设置鼠标样式为手掌抓起
        // 记录块的原始位置
        temp_rect = new QGraphicsRectItem(rect());
        temp_rect->setVisible(false);
        temp_rect->setPos(pos());
        scene()->addItem(temp_rect);
        temp_rect->setOpacity(0.3);
        pos_offset = this->sceneBoundingRect().center() - mapToScene(event->pos());
    }
    // 调用父类的事件处理函数
    QGraphicsRectItem::mousePressEvent(event);
}

void base_rect_class::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Delete) {
        action_delete_callback();
        QGraphicsItem::keyPressEvent(event);
    }
}

void base_rect_class::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
    if (right_menu_mode != ACTION_NONE) {
        QAction* selectedItem = menu.exec(event->screenPos());
        if (selectedItem == nullptr) {
            return;
        }
        if (selectedItem == deleteaction) {
            action_delete_callback();
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
