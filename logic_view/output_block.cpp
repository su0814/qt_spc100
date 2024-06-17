#include "output_block.h"
#include "mainwindow.h"

output_block::output_block(QPointF pos, config_block_data_t data, uint32_t uid, QWidget* uiparent,
                           QGraphicsItem* parent)
    : base_rect_class(0, 0, defaultWidth, defaultHeight, uiparent, parent)
{
    attribute_data.uid = uid;
    setPos(pos.x() - defaultWidth / 2, pos.y() - defaultHeight / 2);
    config_block_data = data;
    self_init();
}

output_block::output_block(QJsonObject rootObject, QWidget* uiparent, QGraphicsItem* parent)
    : base_rect_class(0, 0, defaultWidth, defaultHeight, uiparent, parent)
{
    block_project_prase(rootObject);
    self_init();
}

output_block::output_block(QPointF pos, uint32_t uid, QJsonObject rootObject, QWidget* uiparent, QGraphicsItem* parent)
    : base_rect_class(0, 0, defaultWidth, defaultHeight, uiparent, parent)
{
    block_project_prase(rootObject, true, pos, uid);
    self_init();
}

output_block::~output_block() {}

void output_block::self_init()
{
    mainwindow->logic_view_class->output_block_list.append(this);
    set_connect_point_labels_visible(false);
    config_block_data_update();
    resource_setenable(false);
    set_input_num(1);
    set_right_menu_action(ACTION_DELETE);
    switch (config_block_data.config_param_data.model_type) {
    case MODEL_OUTPUT_RELAY_MOS:
        break;
    case MODEL_OUTPUT_REPEATER:
        set_right_menu_action(ACTION_DELETE | ACTION_SET);
        mainwindow->logic_menu_class->set_output_repeat_disable(config_block_data.config_param_data.model_id, true);
        mainwindow->logic_menu_class->set_input_repeat_disable(config_block_data.config_param_data.model_id, false);
        break;
    default:
        break;
    }
    set_inputpoint_attribute(&attribute_data);
    tooltip_update();
    connect(&update_timer, &QTimer::timeout, this, update_state_slot);  //状态更新定时器及槽函数
    update_timer.start(BLOCK_DATA_REFRESH_TIME);
}

QJsonObject output_block::block_project_info()
{
    QJsonObject rootObject;
    rootObject["x"]       = scenePos().x();
    rootObject["y"]       = scenePos().y();
    rootObject["uid"]     = ( int )attribute_data.uid;
    rootObject["cat"]     = config_block_data.safe_level;
    rootObject["miotype"] = config_block_data.config_param_data.model_iotype;
    rootObject["mtype"]   = config_block_data.config_param_data.model_type;
    rootObject["mid"]     = config_block_data.config_param_data.model_id;
    rootObject["smcu"]    = config_block_data.config_param_data.source_mcu;
    rootObject["name"]    = config_block_data.name;
    return rootObject;
}

void output_block::block_project_prase(QJsonObject rootObject, bool copy, QPointF pos, uint32_t uid)
{
    if (copy) {
        attribute_data.uid = uid;
        setPos(pos);
    } else {
        attribute_data.uid = rootObject["uid"].toInt();
        int x              = rootObject["x"].toInt();
        int y              = rootObject["y"].toInt();
        setPos(x, y);
    }
    config_block_data.safe_level                     = ( safe_level_e )rootObject["cat"].toInt();
    config_block_data.config_param_data.model_iotype = rootObject["miotype"].toInt();
    config_block_data.config_param_data.model_type   = rootObject["mtype"].toInt();
    config_block_data.config_param_data.model_id     = rootObject["mid"].toInt();
    config_block_data.config_param_data.source_mcu   = ( source_mcu_e )rootObject["smcu"].toInt();
    switch (config_block_data.config_param_data.model_type) {
    case MODEL_OUTPUT_REPEATER:
        config_block_data.name = rootObject["name"].toString();
        mainwindow->logic_menu_class->set_output_repeat_name(config_block_data.config_param_data.model_id,
                                                             config_block_data.name);
        mainwindow->logic_menu_class->set_input_repeat_name(config_block_data.config_param_data.model_id,
                                                            config_block_data.name);
        break;
    }
}

void output_block::display_name_update()
{
    QString name;
    if (config_block_data.name.isEmpty()) {
        name = config_block_data.type_name + "." + config_block_data.source_name + config_block_data.suffix;
    } else {
        name = config_block_data.name + "." + config_block_data.source_name + config_block_data.suffix;
    }
    set_display_name(name);
}

void output_block::config_block_data_update()
{
    switch (config_block_data.config_param_data.model_type) {
    case MODEL_OUTPUT_RELAY_MOS:
        if (config_block_data.safe_level == SAFE_LEVEL_CAT2) {
            attribute_data.source_function = cat2_output[config_block_data.config_param_data.source_mcu]
                                                        [config_block_data.config_param_data.model_id];
        } else {
            attribute_data.source_function = cat3_output[config_block_data.config_param_data.model_type]
                                                        [config_block_data.config_param_data.model_id];
        }
        attribute_data.function_name = "output" + QString::number(attribute_data.uid) + "_func()";
        break;
    case MODEL_OUTPUT_REPEATER:
        attribute_data.function_name =
            "outrepeater" + QString::number(config_block_data.config_param_data.model_id) + "_func()";
        break;
    default:
        break;
    }
    display_name_update();
}

void output_block::resource_setenable(bool isenable)
{
    logic_element* source_item = mainwindow->logic_menu_class->item_exist_detect(config_block_data);
    if (source_item != nullptr) {
        source_item->setDisabled(!isenable);
    }
}

void output_block::action_delete_callback()
{
    if (mainwindow->logic_view_class->draw_line_state != DRAW_LINE_STATE_IDLE) {
        return;  //连线状态下不可删除
    }
    update_timer.stop();
    resource_setenable(true);
    mainwindow->logic_view_class->output_block_list.removeOne(this);
    switch (config_block_data.config_param_data.model_type) {
    case MODEL_OUTPUT_RELAY_MOS:
        break;
    case MODEL_OUTPUT_REPEATER:
        mainwindow->logic_menu_class->set_output_repeat_disable(config_block_data.config_param_data.model_id, false);
        mainwindow->logic_menu_class->set_input_repeat_disable(config_block_data.config_param_data.model_id, true);
        mainwindow->logic_menu_class->set_output_repeat_name(config_block_data.config_param_data.model_id, "");
        mainwindow->logic_menu_class->set_input_repeat_name(config_block_data.config_param_data.model_id, "");
        break;
    default:
        break;
    }
    scene()->removeItem(this);
    delete this;
}

void output_block::action_set_callback()
{
    switch (config_block_data.config_param_data.model_type) {
    case MODEL_OUTPUT_RELAY_MOS:
        break;
    case MODEL_OUTPUT_REPEATER:
        repeater_right_menu();
        break;
    default:
        break;
    }
}

void output_block::repeater_right_menu()
{
    QDialog     dialog;
    QFormLayout layout(&dialog);
    dialog.setWindowTitle("设置");
    dialog.setWindowFlags(Qt::Tool);
    dialog.setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    dialog.setWindowFlag(Qt::MSWindowsFixedSizeDialogHint);
    layout.setContentsMargins(10, 10, 10, 10);
    QLineEdit name;
    name.setMaxLength(15);
    name.setText(config_block_data.name);
    layout.addRow("标签:", &name);
    QPushButton  okButton("应用");
    QPushButton  cancleButton("取消");
    QHBoxLayout* hboxLayout1 = new QHBoxLayout;
    hboxLayout1->addWidget(&okButton);
    hboxLayout1->addWidget(&cancleButton);
    layout.addRow(hboxLayout1);
    QObject::connect(&okButton, &QPushButton::clicked, [&]() {
        mainwindow->logic_menu_class->set_output_repeat_name(config_block_data.config_param_data.model_id, name.text());
        mainwindow->logic_menu_class->set_input_repeat_name(config_block_data.config_param_data.model_id, name.text());
        dialog.close();
    });
    QObject::connect(&cancleButton, &QPushButton::clicked, [&]() { dialog.close(); });
    dialog.exec();
}

void output_block::tooltip_update()
{
    QString tooltip;
    tooltip += "uid: " + QString::number(attribute_data.uid);
    tooltip += "\r\n" + (config_block_data.type_name + "." + config_block_data.source_name + config_block_data.suffix);
    setToolTip(tooltip);
}

void output_block::logic_function_update()
{
    if (error) {
        attribute_data.logic_function.clear();
        return;
    }
    int emu_id = mainwindow->logic_view_class->input_block_list.size()
                 + mainwindow->logic_view_class->output_block_list.indexOf(this);
    QString temp_logic_function = " ";
    switch (config_block_data.config_param_data.model_type) {
    case MODEL_OUTPUT_RELAY_MOS:
        temp_logic_function += attribute_data.source_function + "((set_emu_data(" + QString::number(emu_id) + ","
                               + input_point_list[0]->parent_attribute.function_name + ")))";
        break;
    case MODEL_OUTPUT_REPEATER:
        temp_logic_function += " return set_emu_data(" + QString::number(emu_id) + ","
                               + input_point_list[0]->parent_attribute.function_name + ")";
        break;
    default:
        break;
    }
    if (temp_logic_function != attribute_data.logic_function) {
        attribute_data.logic_function = temp_logic_function;
    }
}

/* user slots */
void output_block::update_state_slot()
{
    error_detect();
    logic_function_update();
}
