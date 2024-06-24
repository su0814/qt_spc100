#include "input_block.h"
#include "mainwindow.h"

input_block::input_block(QPointF pos, config_block_data_t data, uint32_t uid, QWidget* uiparent, QGraphicsItem* parent)
    : base_rect_class(0, 0, defaultWidth, defaultHeight, uiparent, parent)
{
    attribute_data.uid = uid;
    setPos(pos.x() - defaultWidth / 2, pos.y() - defaultHeight / 2);
    config_block_data = data;
    self_init();
}

input_block::input_block(QJsonObject rootObject, QWidget* uiparent, QGraphicsItem* parent)
    : base_rect_class(0, 0, defaultWidth, defaultHeight, uiparent, parent)
{
    block_project_prase(rootObject);
    self_init();
}

input_block::input_block(QPointF pos, uint32_t uid, QJsonObject rootObject, QWidget* uiparent, QGraphicsItem* parent)
    : base_rect_class(0, 0, defaultWidth, defaultHeight, uiparent, parent)
{
    block_project_prase(rootObject, true, pos, uid);
    self_init();
}

void input_block::self_init()
{
    mainwindow->logic_view_class->input_block_list.append(this);
    set_connect_point_labels_visible(false);
    attribute_data.parent_id.clear();
    config_block_data_update();
    set_right_menu_action(ACTION_DELETE);
    attribute_data.function_name = "input" + QString::number(attribute_data.uid) + "_func()";
    switch (config_block_data.config_param_data.model_type) {
    case MODEL_INPUT_DI:
    case MODEL_INPUT_CONST:
        break;
    case MODEL_INPUT_QEP:
        set_outputpoint_data_type(CONNECT_POINT_DATATYPE_ENCODER, 0);
        break;
    case MODEL_INPUT_AI:
    case MODEL_INPUT_PI:
        set_right_menu_action(ACTION_SET | ACTION_DELETE);
        break;
    case MODEL_INPUT_REPEATER:
        set_right_menu_action(ACTION_SET | ACTION_DELETE);
        attribute_data.function_name =
            "inputrepeater" + QString::number(config_block_data.config_param_data.model_id) + "_func()";
        break;
    }
    set_output_mask(0x01);
    set_outputpoint_attribute(&attribute_data);
    set_inputpoint_attribute(&attribute_data);
    connect(&update_timer, &QTimer::timeout, this, update_state_slot);  //状态更新定时器及槽函数
    update_timer.start(BLOCK_DATA_REFRESH_TIME);
}

QJsonObject input_block::block_project_info()
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
    rootObject["dir"]     = condition_right_set.dir;
    rootObject["value"]   = ( int )condition_right_set.value;
    switch (config_block_data.config_param_data.model_type) {
    case MODEL_INPUT_REPEATER:
        rootObject["name"] = config_block_data.name;
        break;
    }
    return rootObject;
}

void input_block::block_project_prase(QJsonObject rootObject, bool copy, QPointF pos, uint32_t uid)
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
    condition_right_set.dir                          = rootObject["dir"].toInt();
    condition_right_set.value                        = rootObject["value"].toInt();
    switch (config_block_data.config_param_data.model_type) {
    case MODEL_INPUT_REPEATER:
        config_block_data.name = rootObject["name"].toString();
        mainwindow->logic_menu_class->set_output_repeat_name(config_block_data.config_param_data.model_id,
                                                             config_block_data.name);
        mainwindow->logic_menu_class->set_input_repeat_name(config_block_data.config_param_data.model_id,
                                                            config_block_data.name);
        break;
    }
}

void input_block::display_name_update()
{
    QString name;
    if (config_block_data.name.isEmpty()) {
        name = config_block_data.type_name + "." + config_block_data.source_name + config_block_data.suffix;
    } else {
        name = config_block_data.name + "." + config_block_data.source_name + config_block_data.suffix;
    }
    set_display_name(name);
}

void input_block::config_block_data_update()
{
    switch (config_block_data.config_param_data.model_type) {
    case MODEL_INPUT_DI:
    case MODEL_INPUT_AI:
    case MODEL_INPUT_PI:
    case MODEL_INPUT_QEP:
    case MODEL_INPUT_CONST:
        if (config_block_data.safe_level == SAFE_LEVEL_CAT2) {
            attribute_data.source_function = cat2_source_function[config_block_data.config_param_data.source_mcu]
                                                                 [config_block_data.config_param_data.model_type]
                                                                 [config_block_data.config_param_data.model_id];
        } else {
            attribute_data.source_function = cat3_source_function[config_block_data.config_param_data.model_type]
                                                                 [config_block_data.config_param_data.model_id];
        }
        break;
    case MODEL_INPUT_REPEATER:
        break;
    }

    display_name_update();
}

void input_block::logic_function_update()
{
    if (error) {
        attribute_data.logic_function.clear();
        return;
    }
    int     emu_id              = mainwindow->logic_view_class->input_block_list.indexOf(this);
    QString temp_logic_function = " return set_emu_data(" + QString::number(emu_id) + ",";
    switch (config_block_data.config_param_data.model_type) {
    case MODEL_INPUT_DI:
        if (config_block_data.config_param_data.normal_state == NORMAL_STATE_NC) {
            temp_logic_function += attribute_data.source_function + "==false)";
        } else {
            temp_logic_function += attribute_data.source_function + "==true)";
        }
        break;
    case MODEL_INPUT_AI:
    case MODEL_INPUT_PI:
        temp_logic_function += attribute_data.source_function + ">" + QString::number(condition_right_set.value) + ")";
        break;
    case MODEL_INPUT_QEP:
        temp_logic_function = " return "
                              + cat3_source_function[config_block_data.config_param_data.model_type]
                                                    [config_block_data.config_param_data.model_id];
        break;
    case MODEL_INPUT_CONST:
        temp_logic_function += attribute_data.source_function + ")";
        break;
    case MODEL_INPUT_REPEATER:
        temp_logic_function +=
            "outrepeater" + QString::number(config_block_data.config_param_data.model_id) + "_func())";
        break;
    }
    if (temp_logic_function != attribute_data.logic_function) {
        attribute_data.logic_function = temp_logic_function;
    }
    send_attribute_data();
}

void input_block::tooltip_update()
{
    QString tooltip;
    tooltip += "uid: " + QString::number(attribute_data.uid);
    tooltip += "\r\n" + (config_block_data.type_name + "." + config_block_data.source_name + config_block_data.suffix);
    switch (config_block_data.config_param_data.model_type) {
    case MODEL_INPUT_DI:
    case MODEL_INPUT_REPEATER:
    case MODEL_INPUT_CONST:
        break;
    case MODEL_INPUT_AI:
        tooltip += "\r\n阈值: " + QString::number(condition_right_set.value) + " mV";
        break;
    case MODEL_INPUT_PI:
        tooltip += "\r\n阈值: " + QString::number(condition_right_set.value) + " pulse/s";
        break;
    case MODEL_INPUT_QEP:
        break;
    }
    setToolTip(tooltip);
}

void input_block::update_state_slot()
{
    error_detect();
    logic_function_update();
    tooltip_update();
}

void input_block::ai_right_menu()
{
    QDialog     dialog;
    QFormLayout layout(&dialog);
    dialog.setWindowTitle("设置");
    dialog.setWindowFlags(Qt::Tool);
    dialog.setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    dialog.setWindowFlag(Qt::MSWindowsFixedSizeDialogHint);
    layout.setContentsMargins(10, 10, 10, 10);
    QSpinBox     value;
    QLabel       unit;
    QHBoxLayout* hboxLayout = new QHBoxLayout;
    hboxLayout->addWidget(&value);
    hboxLayout->addWidget(&unit);
    value.setMinimum(0);
    value.setMaximum(30000);
    value.setValue(condition_right_set.value);
    unit.setText("mV");
    QPushButton  okButton("应用");
    QPushButton  cancleButton("取消");
    QHBoxLayout* hboxLayout1 = new QHBoxLayout;
    hboxLayout1->addWidget(&okButton);
    hboxLayout1->addWidget(&cancleButton);
    layout.addRow("阈值:", hboxLayout);
    layout.addRow(hboxLayout1);
    QObject::connect(&okButton, &QPushButton::clicked, [&]() {
        condition_right_set.value = value.value();
        dialog.close();
    });
    QObject::connect(&cancleButton, &QPushButton::clicked, [&]() { dialog.close(); });
    dialog.exec();
}
void input_block::pi_right_menu()
{
    QDialog     dialog;
    QFormLayout layout(&dialog);
    dialog.setWindowTitle("设置");
    dialog.setWindowFlags(Qt::Tool);
    dialog.setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    dialog.setWindowFlag(Qt::MSWindowsFixedSizeDialogHint);
    layout.setContentsMargins(10, 10, 10, 10);
    QSpinBox     value;
    QLabel       unit;
    QHBoxLayout* hboxLayout = new QHBoxLayout;
    hboxLayout->addWidget(&value);
    hboxLayout->addWidget(&unit);
    value.setMinimum(0);
    value.setMaximum(200000);
    value.setValue(condition_right_set.value);
    unit.setText("pulse/s");
    QPushButton  okButton("应用");
    QPushButton  cancleButton("取消");
    QHBoxLayout* hboxLayout1 = new QHBoxLayout;
    hboxLayout1->addWidget(&okButton);
    hboxLayout1->addWidget(&cancleButton);
    layout.addRow("阈值:", hboxLayout);
    layout.addRow(hboxLayout1);
    QObject::connect(&okButton, &QPushButton::clicked, [&]() {
        condition_right_set.value = value.value();
        dialog.close();
    });
    QObject::connect(&cancleButton, &QPushButton::clicked, [&]() { dialog.close(); });
    dialog.exec();
}

void input_block::qep_right_menu()
{
    QDialog     dialog;
    QFormLayout layout(&dialog);
    dialog.setWindowTitle("设置");
    dialog.setWindowFlags(Qt::Tool);
    dialog.setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    dialog.setWindowFlag(Qt::MSWindowsFixedSizeDialogHint);
    layout.setContentsMargins(10, 10, 10, 10);
    QComboBox dir;
    dir.addItem("0");
    dir.addItem("1");
    dir.setCurrentIndex(condition_right_set.dir);
    QSpinBox     value;
    QLabel       unit;
    QHBoxLayout* hboxLayout = new QHBoxLayout;
    hboxLayout->addWidget(&value);
    hboxLayout->addWidget(&unit);
    value.setMinimum(0);
    value.setMaximum(30000);
    value.setValue(condition_right_set.value);
    unit.setText("cm/s");
    QPushButton  okButton("应用");
    QPushButton  cancleButton("取消");
    QHBoxLayout* hboxLayout1 = new QHBoxLayout;
    hboxLayout1->addWidget(&okButton);
    hboxLayout1->addWidget(&cancleButton);
    layout.addRow("方向:", &dir);
    layout.addRow("阈值:", hboxLayout);
    layout.addRow(hboxLayout1);
    QObject::connect(&okButton, &QPushButton::clicked, [&]() {
        condition_right_set.dir   = dir.currentIndex();
        condition_right_set.value = value.value();
        dialog.close();
    });
    QObject::connect(&cancleButton, &QPushButton::clicked, [&]() { dialog.close(); });
    dialog.exec();
}

void input_block::repeater_right_menu()
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

void input_block::action_set_callback()
{
    switch (config_block_data.config_param_data.model_type) {
    case MODEL_INPUT_AI:
        ai_right_menu();
        break;
    case MODEL_INPUT_DI:
        break;
    case MODEL_INPUT_PI:
        pi_right_menu();
        break;
    case MODEL_INPUT_QEP:
        qep_right_menu();
        break;
    case MODEL_INPUT_REPEATER:
        if (!mainwindow->logic_menu_class->input_repeater_is_disable(config_block_data.config_param_data.model_id)) {
            repeater_right_menu();
        }
        break;
    }
}

void input_block::action_delete_callback()
{
    if (mainwindow->logic_view_class->draw_line_state != DRAW_LINE_STATE_IDLE) {
        return;  //连线状态下不可删除
    }
    update_timer.stop();
    mainwindow->logic_view_class->input_block_list.removeOne(this);
    scene()->removeItem(this);
    delete this;
}
