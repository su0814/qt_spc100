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
    attribute_data.uid = rootObject["uid"].toInt();
    int x              = rootObject["x"].toInt();
    int y              = rootObject["y"].toInt();
    setPos(x, y);
    config_block_data.safe_level                     = ( safe_level_e )rootObject["cat"].toInt();
    config_block_data.config_param_data.model_iotype = rootObject["miotype"].toInt();
    config_block_data.config_param_data.model_type   = rootObject["mtype"].toInt();
    config_block_data.config_param_data.model_id     = rootObject["mid"].toInt();
    config_block_data.config_param_data.source_mcu   = ( source_mcu_e )rootObject["smcu"].toInt();
    condition_right_set.dir                          = rootObject["dir"].toInt();
    condition_right_set.value                        = rootObject["value"].toInt();
    self_init();
}

void input_block::self_init()
{
    set_connect_point_labels_visible(false);
    attribute_data.parent_id.clear();
    config_block_data_update();
    attribute_data.function_name = "input" + QString::number(attribute_data.uid) + "_func()";
    switch (config_block_data.config_param_data.model_type) {
    case MODEL_INPUT_DI:
        set_right_menu_action(ACTION_DELETE);
        break;
    case MODEL_INPUT_AI:
    case MODEL_INPUT_PI:
    case MODEL_INPUT_QEP:
        set_right_menu_action(ACTION_SET | ACTION_DELETE);
        break;
    }
    set_output_num(1);
    set_outputpoint_attribute(&attribute_data);
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
    return rootObject;
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
    if (config_block_data.safe_level == SAFE_LEVEL_CAT2) {
        attribute_data.source_function = cat2_source_function[config_block_data.config_param_data.source_mcu]
                                                             [config_block_data.config_param_data.model_type]
                                                             [config_block_data.config_param_data.model_id];
    } else {
        attribute_data.source_function = cat3_source_function[config_block_data.config_param_data.model_type]
                                                             [config_block_data.config_param_data.model_id];
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
    QString temp_logic_function = "return set_emu_data(" + QString::number(emu_id) + ",";
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
        QString dir_func;
        if (config_block_data.safe_level == SAFE_LEVEL_CAT2) {
            dir_func = cat2_qep_dir_function[config_block_data.config_param_data.source_mcu]
                                            [config_block_data.config_param_data.model_id];
            temp_logic_function += "(" + dir_func + "==" + QString::number(condition_right_set.dir) + " and "
                                   + attribute_data.source_function + ">" + QString::number(condition_right_set.value)
                                   + "))";
        } else {
            int id   = config_block_data.config_param_data.model_id / 2 * 2;
            int type = config_block_data.config_param_data.model_type;
            temp_logic_function +=
                "(" + cat3_qep_dir_function[id] + "==" + QString::number(condition_right_set.dir) + " and "
                + cat3_source_function[type][id] + ">" + QString::number(condition_right_set.value) + ")or("
                + cat3_qep_dir_function[id + 1] + "==" + QString::number(condition_right_set.dir) + " and "
                + cat3_source_function[type][id + 1] + ">" + QString::number(condition_right_set.value) + "))";
        }
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
        break;
    case MODEL_INPUT_AI:
        tooltip += "\r\n阈值: " + QString::number(condition_right_set.value) + " mV";
        break;
    case MODEL_INPUT_PI:
        tooltip += "\r\n阈值: " + QString::number(condition_right_set.value) + " pulse/s";
        break;
    case MODEL_INPUT_QEP:
        tooltip += "\r\n方向: " + QString::number(condition_right_set.dir);
        tooltip += "\r\n阈值: " + QString::number(condition_right_set.value);
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
