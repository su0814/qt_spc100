#include "config_param.h"
#include "mainwindow.h"
#include <QBrush>
#include <QColor>
#include <QDebug>
#include <QFormLayout>
#include <QGraphicsSceneMouseEvent>
#include <QLayout>

#define VALUE_PIXMAP_WIDTH (50)
config_param::config_param(int x, int y, int w, int h, QWidget* mparent, QGraphicsItem* parent)
    : QGraphicsRectItem(x, y, w, h, parent)
{
    mainwindow = ( MainWindow* )mparent;
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemIsFocusable, true);
    setCursor(Qt::ArrowCursor);

    setFlag(QGraphicsItem::ItemIsFocusable);
    QPen pen(QColor(50, 50, 50));
    pen.setWidth(0);
    this->setPen(pen);
    /* 标签显示 */

    label_rect = new QGraphicsRectItem(0, 0, LABEL_WIDTH, h, this);
    QFont font("Arial", 9);
    label_rect->setFlag(QGraphicsItem::ItemIsFocusable);
    label_text = new QGraphicsTextItem("unknow", label_rect);
    label_rect->setCursor(Qt::ArrowCursor);
    label_text->setTextWidth(LABEL_WIDTH);
    label_text->setFont(font);
    QTextOption option = label_text->document()->defaultTextOption();  //取出当前设置
    option.setAlignment(Qt::AlignHCenter);                 //对设置进行修改，增加居中对齐设置
    label_text->document()->setDefaultTextOption(option);  //重新设定
    label_text->setPos(label_rect->boundingRect().center() - label_text->boundingRect().center());
    /* 数值显示初始化 */
    value_rect = new QGraphicsRectItem(0, 0, VALUE_WIDTH, h, this);
    value_text = new QGraphicsTextItem("0", value_rect);
    value_rect->setCursor(Qt::ArrowCursor);
    value_text->setTextWidth(VALUE_WIDTH);
    value_text->setFont(font);
    value_pixmap = new QGraphicsPixmapItem(value_rect);
    value_rect->setVisible(false);
    option = value_text->document()->defaultTextOption();  //取出当前设置
    option.setAlignment(Qt::AlignHCenter);                 //对设置进行修改，增加居中对齐设置
    value_text->document()->setDefaultTextOption(option);  //重新设定
    value_text->setPos(value_rect->boundingRect().center() - value_text->boundingRect().center());
    data_reset();
    connect(this, remove_item_from_config_signal, mainwindow->logic_menu_class,
            &logic_menu::remove_item_from_config_slot);
    connect(this, name_change_signal, mainwindow->logic_menu_class, &logic_menu::name_change_slot);
    set_brush_state(BRUSH_STATE_NORMAL);
}

QJsonObject config_param::config_param_project_info()
{
    QJsonObject rootObject;
    rootObject["used"] = config_block_data.is_used;
    if (config_block_data.is_used) {
        rootObject["type"]     = element_data.type;
        rootObject["mtype"]    = element_data.model_type;
        rootObject["miotype"]  = element_data.model_iotype;
        rootObject["cat"]      = element_data.cat;
        rootObject["name"]     = element_data.name;
        rootObject["pixmap"]   = element_data.pixmap;
        rootObject["ns"]       = element_data.normal_state;
        rootObject["username"] = config_block_data.name;
    }
    return rootObject;
}

bool config_param::config_param_project_parse(QJsonObject rootObject)
{
    element_data_t data;
    bool           used = rootObject["used"].toBool();
    if (used) {
        setVisible(true);
        config_block_data.name = rootObject["username"].toString();
        data.type              = rootObject["type"].toInt();
        data.model_type        = rootObject["mtype"].toInt();
        data.model_iotype      = rootObject["miotype"].toInt();
        data.cat               = rootObject["cat"].toInt();
        data.name              = rootObject["name"].toString();
        data.pixmap            = rootObject["pixmap"].toString();
        data.normal_state      = rootObject["ns"].toInt();
        drap_data_parse(data);
        if (!config_block_data.name.isEmpty()) {
            set_name(config_block_data.name);
        }
    }
    return false;
}

/**
 * @brief 设置配置参数快的基本内容
 * @param data
 */
void config_param::set_block_data(config_param_data_t data)
{
    config_block_data.config_param_data = data;
    config_block_data.is_config         = true;
    switch (config_block_data.config_param_data.label_dir) {
    case LABEL_DIR_LEFT:
        label_rect->setPos(boundingRect().x() - LABEL_WIDTH, boundingRect().y());
        value_rect->setPos(boundingRect().x() - LABEL_WIDTH - VALUE_WIDTH, boundingRect().y());
        value_pixmap->setPos(value_rect->boundingRect().x() - VALUE_PIXMAP_WIDTH - 10,
                             value_rect->boundingRect().y() + value_rect->boundingRect().height() * 0.25);
        break;
    case LABEL_DIR_RIGHT:
        label_rect->setPos(boundingRect().x() + boundingRect().width(), boundingRect().y());
        value_rect->setPos(boundingRect().x() + boundingRect().width() + LABEL_WIDTH, boundingRect().y());
        value_pixmap->setPos(value_rect->boundingRect().x() + VALUE_WIDTH + 10,
                             value_rect->boundingRect().y() + value_rect->boundingRect().height() * 0.25);
        break;
    default:
        break;
    }
    if (data.cat3_model) {
        connect(this, cat3_model_sync_drap_signal, data.cat3_model, cat3_model_sync_drap_slot);
        connect(this, cat3_model_sync_reset_signal, data.cat3_model, cat3_model_sync_reset_slot);
        connect(this, cat3_model_sync_name_signal, data.cat3_model, cat3_model_sync_name_slot);
    }
    config_block_data.source_name = model_name[data.model_iotype][data.model_type][data.model_id];
    set_name(config_block_data.name);
}

void config_param::set_name(QString name)
{
    if (name.isEmpty()) {
        label_text->setPlainText(config_block_data.source_name + config_block_data.suffix);
    } else {
        label_text->setPlainText(name + "." + config_block_data.source_name + config_block_data.suffix);
    }

    label_text->setPos(label_rect->boundingRect().center() - label_text->boundingRect().center());
    QStringList cat;
    cat << "单通道"
        << "双通道";
    QStringList ns;
    ns << ""
       << "，常闭"
       << "，常开";
    setToolTip("<html><br>" + config_block_data.name + "<br>" + config_block_data.type_name + "，"
               + config_block_data.source_name + config_block_data.suffix + "<br>" + cat[config_block_data.safe_level]
               + ns[config_block_data.config_param_data.normal_state] + "</html>");
    label_rect->setToolTip(this->toolTip());
    emit name_change_signal(config_block_data);
}

void config_param::set_brush_state(brush_state_e state)
{
    switch (state) {
    case BRUSH_STATE_NORMAL: {
        QBrush brush(QColor(255, 245, 238));  //块填充色
        setBrush(brush);
        label_rect->setBrush(brush);
        value_rect->setBrush(brush);
    } break;
    case BRUSH_STATE_DISPLAY: {
        QBrush brush(QColor(0, 245, 0));  //块填充色
        setBrush(brush);
    } break;
    case BRUSH_STATE_DATA_TRUE: {
        QBrush brush(QColor(0, 245, 0));  //块填充色
        setBrush(brush);
        label_rect->setBrush(brush);
    } break;
    default:
        QBrush brush(QColor(230, 230, 250));  //块填充色
        setBrush(brush);
        break;
    }
}

void config_param::set_module_value(int value)
{
    if (!config_block_data.is_used) {
        return;
    }
    QString str = QString::number(abs(value));
    switch (config_block_data.config_param_data.model_iotype) {
    case MODEL_TYPE_INPUT:
        switch (config_block_data.config_param_data.model_type) {
        case MODEL_INPUT_AI: {
            qreal ai_v = value * 249.5 / 33579.0;
            str        = QString::number(ai_v, 'f', 2) + "V";
        } break;
        case MODEL_INPUT_PI:
            str += "p/s";
            break;
        case MODEL_INPUT_QEP:
            str += "p/s";
            if (value > 0) {
                value_pixmap->setPixmap(
                    QPixmap(":/new/photo/photo/encode_dir1.png")
                        .scaled(value_rect->boundingRect().height() / 2, value_rect->boundingRect().height() / 2));
            } else {
                value_pixmap->setPixmap(
                    QPixmap(":/new/photo/photo/encode_dir2.png")
                        .scaled(value_rect->boundingRect().height() / 2, value_rect->boundingRect().height() / 2));
            }
            break;
        case MODEL_INPUT_DI:
            if (value == 0) {
                set_brush_state(BRUSH_STATE_NORMAL);
            } else {
                set_brush_state(BRUSH_STATE_DATA_TRUE);
            }
            break;
        }
        break;
    case MODEL_TYPE_OUTPUT:
        switch (config_block_data.config_param_data.model_type) {
        case MODEL_OUTPUT_RELAY_MOS:
            if (value == 0) {
                set_brush_state(BRUSH_STATE_NORMAL);
            } else {
                set_brush_state(BRUSH_STATE_DATA_TRUE);
            }
            break;
        }
        break;
    }
    value_text->setPlainText(str);
}

config_block_data_t config_param::get_block_data()
{
    return config_block_data;
}

config_user_data_t config_param::get_user_data()
{
    return config_user_data;
}

void config_param::data_reset()
{
    label_rect->removeSceneEventFilter(this);
    disconnect(this, add_item_from_config_signal, mainwindow->logic_menu_class, &logic_menu::add_item_from_config_slot);
    emit remove_item_from_config_signal(config_block_data);
    if (config_block_data.is_used) {
        mainwindow->config_view_class->config_photo_svg->config_param_delete_update(
            mainwindow->config_view_class->get_module_param(), &config_block_data);
        mainwindow->config_view_class->config_photo_svg->module_param_update(
            mainwindow->config_view_class->get_module_param());
    }
    config_block_data.is_used       = false;
    config_block_data.safe_level    = SAFE_LEVEL_CAT3;
    config_block_data.name          = "";
    config_user_data.contactors_num = 1;
    set_name(config_block_data.name);
    display_hide();
}

/**
 * @brief 拖拽进元件时检测是否本块可用
 * @param config_data
 * @return
 */
bool config_param::display_detect(element_data_t config_data)
{
    if (config_block_data.is_used || this->isEnabled() == false) {
        return false;
    }
    if ((config_data.model_iotype != config_block_data.config_param_data.model_iotype)
        || config_data.model_type != config_block_data.config_param_data.model_type) {
        return false;
    }
    if (config_data.cat > config_block_data.safe_level) {
        return false;
    }
    setVisible(true);
    set_brush_state(BRUSH_STATE_DISPLAY);
    return true;
}

/**
 * @brief 隐藏块
 */
void config_param::display_hide()
{
    set_brush_state(BRUSH_STATE_NORMAL);
    if (config_block_data.is_used) {
        return;
    }
    setVisible(false);
    value_rect->setVisible(false);
    set_module_value(0);
}

/**
 * @brief 判断一个点是否在自己以及子块内
 * @param pos
 */
bool config_param::bounding_range_detect(QPointF pos)
{
    if (sceneBoundingRect().contains(pos) || label_rect->sceneBoundingRect().contains(pos)) {
        return true;
    }
    return false;
}

/**
 * @brief 接收拖拽数据
 * @param data
 */
void config_param::drap_data_receive(element_data_t data)
{
    if (display_detect(data) == false) {
        return;
    }
    /* cat变化 */
    emit cat3_model_sync_drap_signal(data);
    drap_data_parse(data);
}

/**
 * @brief 处理拖拽数据
 * @param data
 */
void config_param::drap_data_parse(element_data_t data)
{
    element_data = data;
    connect(this, add_item_from_config_signal, mainwindow->logic_menu_class, &logic_menu::add_item_from_config_slot);
    config_block_data.safe_level = ( safe_level_e )data.cat;
    label_rect->installSceneEventFilter(this);
    config_block_data.is_used = true;
    mainwindow->config_view_class->config_photo_svg->config_param_creat_update(
        mainwindow->config_view_class->get_module_param(), &config_block_data);
    QStringList suffix;
    if (config_block_data.config_param_data.model_iotype == MODEL_TYPE_INPUT
        && config_block_data.config_param_data.model_type == MODEL_INPUT_QEP) {
        suffix << ".D"
               << ".D"
               << "";
    } else {
        suffix << ".A"
               << ".B"
               << "";
    }

    if (config_block_data.safe_level == SAFE_LEVEL_CAT2) {
        config_block_data.suffix = suffix[config_block_data.config_param_data.source_mcu];
    } else {
        config_block_data.suffix = "";
    }
    config_block_data.type_name                      = data.name;
    config_block_data.config_param_data.normal_state = data.normal_state;
    config_block_data.pixmap                         = data.pixmap;
    set_name(data.name);
    value_rect->setVisible(true);
    emit add_item_from_config_signal(config_block_data);
}

/* user slots */
void config_param::cat3_model_sync_drap_slot(element_data_t data)
{
    if (config_block_data.is_used) {
        return;
    }
    config_block_data.safe_level = ( safe_level_e )data.cat;
    if (config_block_data.safe_level == SAFE_LEVEL_CAT3) {
        drap_data_parse(data);
    }
}
void config_param::cat3_model_sync_reset_slot()
{
    if (config_block_data.is_used && config_block_data.safe_level == SAFE_LEVEL_CAT2) {
        return;
    }
    data_reset();
}

void config_param::cat3_model_sync_name_slot(QString name)
{
    if (config_block_data.safe_level == SAFE_LEVEL_CAT3) {
        config_block_data.name = name;
        set_name(name);
    }
}

/* sys event */
void config_param::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsRectItem::mousePressEvent(event);
}

bool config_param::sceneEventFilter(QGraphicsItem* watched, QEvent* event)
{
    if (watched == label_rect) {
        if (event->type() == QEvent::GraphicsSceneMouseDoubleClick) {
            Safety_Param_Dialog dialog(mainwindow->config_view_class->get_module_param(), &config_block_data,
                                       &config_user_data);
            dialog.my_exec();
            if (!config_block_data.name.isEmpty()) {
                set_name(config_block_data.name);
                emit cat3_model_sync_name_signal(config_block_data.name);
            }
            return true;
        } else if (event->type() == QEvent::KeyPress) {
            QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
            if (keyEvent->key() == Qt::Key_Delete) {
                data_reset();
                emit cat3_model_sync_reset_signal();
                return true;
            }
        }
    }
    return QGraphicsRectItem::sceneEventFilter(watched, event);
}

void config_param::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Delete) {
        data_reset();

        emit cat3_model_sync_reset_signal();
    }
    QGraphicsItem::keyPressEvent(event);
}
