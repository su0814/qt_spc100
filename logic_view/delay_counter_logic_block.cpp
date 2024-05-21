#include "delay_counter_logic_block.h"
#include "mainwindow.h"
#include <QDebug>
delay_counter_logic_block::delay_counter_logic_block(QPointF pos, config_block_data_t data, uint32_t uid,
                                                     QWidget* uiparent, QGraphicsItem* parent)
    : base_rect_class(0, 0, defaultWidth, defaultHeight, uiparent, parent)
{
    attribute_data.uid = uid;
    setPos(pos.x() - defaultWidth / 2, pos.y() - defaultHeight / 2);
    config_block_data = data;
    self_init();
}

delay_counter_logic_block::delay_counter_logic_block(QJsonObject rootObject, QWidget* uiparent, QGraphicsItem* parent)
    : base_rect_class(0, 0, defaultWidth, defaultHeight, uiparent, parent)
{

    attribute_data.uid = rootObject["uid"].toInt();
    int x              = rootObject["x"].toInt();
    int y              = rootObject["y"].toInt();
    setPos(x, y);
    config_block_data.config_param_data.model_iotype = rootObject["miotype"].toInt();
    config_block_data.config_param_data.model_type   = rootObject["mtype"].toInt();
    config_block_data.config_param_data.model_id     = rootObject["mid"].toInt();
    config_block_data.source_name                    = rootObject["sname"].toString();
    config_block_data.pixmap                         = rootObject["pixmap"].toString();
    QStringList inlabels, outlabels;
    for (int i = 0; i < MAX_CONNECT_POINT_NUM; i++) {
        inlabels.append(rootObject["ilabel" + QString::number(i)].toString());
    }
    for (int i = 0; i < MAX_CONNECT_POINT_NUM; i++) {
        outlabels.append(rootObject["olabel" + QString::number(i)].toString());
    }
    self_init();
    set_input_num(rootObject["innum"].toInt());
    set_output_num(rootObject["outnum"].toInt());
    set_user_inputpoint_labels(inlabels);
    set_user_outputpoint_labels(outlabels);
    set_input_reverse_data(rootObject["reverse"].toInt());
    switch (config_block_data.config_param_data.model_id) {
    case MODEL_ID_LOGIC_DELAY_OFF:
    case MODEL_ID_LOGIC_DELAY_ON:
        on_off_delay_time = rootObject["onoffdelay"].toInt();
        break;
    case MODEL_ID_LOGIC_DELAY_ADJUST_OFF:
    case MODEL_ID_LOGIC_DELAY_ADJUST_ON:
        for (int i = 0; i < 4; i++) {
            adjust_on_off_delay_time[i] = rootObject["adjustonoffdelay" + QString::number(i)].toInt();
        }
        break;
    default:
        break;
    }
}

void delay_counter_logic_block::self_init()
{
    attribute_data.parent_id.clear();
    set_display_name(config_block_data.source_name);
    set_display_pixmap(config_block_data.pixmap);
    set_display_uid(attribute_data.uid);
    attribute_data.function_name = "delaycounterlogic" + QString::number(attribute_data.uid) + "_func()";
    set_right_menu_action(ACTION_SET | ACTION_DELETE);
    QStringList iname;
    switch (config_block_data.config_param_data.model_id) {
    case MODEL_ID_LOGIC_DELAY_OFF:
        set_input_num(1);
        set_output_num(1);
        mainwindow->logic_view_class->delay_off_list.append(this);
        on_off_setting_dialog = new delay_on_off_setting(this);
        if (mainwindow->logic_view_class->delay_off_list.size() >= LOGIC_BLOCK_MAX_NUM) {
            resource_setenable(false);
        }
        break;
    case MODEL_ID_LOGIC_DELAY_ON:
        set_input_num(1);
        set_output_num(1);
        mainwindow->logic_view_class->delay_on_list.append(this);
        on_off_setting_dialog = new delay_on_off_setting(this);
        if (mainwindow->logic_view_class->delay_on_list.size() >= LOGIC_BLOCK_MAX_NUM) {
            resource_setenable(false);
        }
        break;
    case MODEL_ID_LOGIC_DELAY_ADJUST_OFF:
        set_input_num(2);
        set_output_num(1);
        iname.clear();
        iname.append("控制");
        iname.append("延迟1");
        iname.append("延迟2");
        iname.append("延迟3");
        iname.append("延迟4");
        set_sys_inputpoint_labels(iname);
        mainwindow->logic_view_class->delay_adjust_off_list.append(this);
        adjust_on_off_setting_dialog = new delay_adjust_on_off_setting(this);
        if (mainwindow->logic_view_class->delay_adjust_off_list.size() >= LOGIC_BLOCK_MAX_NUM) {
            resource_setenable(false);
        }
        break;
    case MODEL_ID_LOGIC_DELAY_ADJUST_ON:
        set_input_num(2);
        set_output_num(1);
        iname.clear();
        iname.append("控制");
        iname.append("延迟1");
        iname.append("延迟2");
        iname.append("延迟3");
        iname.append("延迟4");
        set_sys_inputpoint_labels(iname);
        mainwindow->logic_view_class->delay_adjust_on_list.append(this);
        adjust_on_off_setting_dialog = new delay_adjust_on_off_setting(this);
        if (mainwindow->logic_view_class->delay_adjust_on_list.size() >= LOGIC_BLOCK_MAX_NUM) {
            resource_setenable(false);
        }
        break;
    default:
        break;
    }
    set_inputpoint_attribute(&attribute_data);
    set_outputpoint_attribute(&attribute_data);
    connect(&update_timer, &QTimer::timeout, this, update_state_slot);  //状态更新定时器及槽函数
    update_timer.start(BLOCK_DATA_REFRESH_TIME);
}

QJsonObject delay_counter_logic_block::block_project_info()
{
    QJsonObject rootObject;
    rootObject["x"]       = scenePos().x();
    rootObject["y"]       = scenePos().y();
    rootObject["uid"]     = ( int )attribute_data.uid;
    rootObject["miotype"] = config_block_data.config_param_data.model_iotype;
    rootObject["mtype"]   = config_block_data.config_param_data.model_type;
    rootObject["mid"]     = config_block_data.config_param_data.model_id;
    rootObject["sname"]   = config_block_data.source_name;
    rootObject["pixmap"]  = config_block_data.pixmap;
    rootObject["innum"]   = get_input_point_num();
    rootObject["outnum"]  = get_output_point_num();
    rootObject["reverse"] = get_input_reverse_data();
    QStringList inlabels  = get_user_inputpoint_labels();
    QStringList outlabels = get_user_outputpoint_labels();
    for (int i = 0; i < inlabels.size(); i++) {
        rootObject["ilabel" + QString::number(i)] = inlabels[i];
    }
    for (int i = 0; i < inlabels.size(); i++) {
        rootObject["olabel" + QString::number(i)] = outlabels[i];
    }
    switch (config_block_data.config_param_data.model_id) {
    case MODEL_ID_LOGIC_DELAY_OFF:
    case MODEL_ID_LOGIC_DELAY_ON:
        rootObject["onoffdelay"] = on_off_delay_time;
        break;
    case MODEL_ID_LOGIC_DELAY_ADJUST_OFF:
    case MODEL_ID_LOGIC_DELAY_ADJUST_ON:
        for (int i = 0; i < 4; i++) {
            rootObject["adjustonoffdelay" + QString::number(i)] = adjust_on_off_delay_time[i];
        }
        break;
    default:
        break;
    }
    return rootObject;
}

void delay_counter_logic_block::debug_data_parse(uint8_t res)
{
    switch (config_block_data.config_param_data.model_id) {
    case MODEL_ID_LOGIC_DELAY_OFF:
    case MODEL_ID_LOGIC_DELAY_ON:
    case MODEL_ID_LOGIC_DELAY_ADJUST_OFF:
    case MODEL_ID_LOGIC_DELAY_ADJUST_ON:
        for (int i = 0; i < get_output_point_num(); i++) {
            output_point_list[i]->send_debug_data((res >> i) & 0x01);
        }
        break;
    default:
        break;
    }
}

void delay_counter_logic_block::action_delete_callback()
{
    if (mainwindow->logic_view_class->draw_line_state != DRAW_LINE_STATE_IDLE) {
        return;  //连线状态下不可删除
    }
    update_timer.stop();
    mainwindow->logic_view_class->delay_counter_block_list.removeOne(this);
    switch (config_block_data.config_param_data.model_id) {
    case MODEL_ID_LOGIC_DELAY_OFF:
        mainwindow->logic_view_class->delay_off_list.removeOne(this);
        break;
    case MODEL_ID_LOGIC_DELAY_ON:
        mainwindow->logic_view_class->delay_on_list.removeOne(this);
        break;
    case MODEL_ID_LOGIC_DELAY_ADJUST_OFF:
        mainwindow->logic_view_class->delay_adjust_off_list.removeOne(this);
        break;
    case MODEL_ID_LOGIC_DELAY_ADJUST_ON:
        mainwindow->logic_view_class->delay_adjust_on_list.removeOne(this);
        break;
    default:
        break;
    }
    resource_setenable(true);
    scene()->removeItem(this);
    delete this;
}

void delay_counter_logic_block::action_set_callback()
{
    switch (config_block_data.config_param_data.model_id) {
    case MODEL_ID_LOGIC_DELAY_OFF:
    case MODEL_ID_LOGIC_DELAY_ON:
        on_off_setting_dialog->setting_exec();
        break;
    case MODEL_ID_LOGIC_DELAY_ADJUST_OFF:
    case MODEL_ID_LOGIC_DELAY_ADJUST_ON:
        adjust_on_off_setting_dialog->setting_exec();
        break;
    default:
        break;
    }
}

void delay_counter_logic_block::resource_setenable(bool isenable)
{
    logic_element* source_item = mainwindow->logic_menu_class->get_function_item(config_block_data);
    if (source_item != nullptr) {
        source_item->setDisabled(!isenable);
    }
}

void delay_counter_logic_block::tooltip_update()
{
    QString tooltip;
    tooltip += "uid: " + QString::number(attribute_data.uid);
    tooltip += "\r\n" + config_block_data.source_name;
    switch (config_block_data.config_param_data.model_id) {
    case MODEL_ID_LOGIC_DELAY_OFF:
    case MODEL_ID_LOGIC_DELAY_ON:
        tooltip += "\r\n延迟时间: " + QString::number(on_off_delay_time) + " ms";
        break;
    case MODEL_ID_LOGIC_DELAY_ADJUST_OFF:
    case MODEL_ID_LOGIC_DELAY_ADJUST_ON:
        for (int i = 0; i < 4; i++) {
            tooltip +=
                "\r\n延迟时间" + QString::number(i) + ": " + QString::number(adjust_on_off_delay_time[i]) + " ms";
        }
        break;
    default:
        break;
    }
    setToolTip(tooltip);
}

void delay_counter_logic_block::logic_function_update()
{
    if (error) {
        attribute_data.logic_function.clear();
        return;
    }
    int emu_id = mainwindow->logic_view_class->input_block_list.size()
                 + mainwindow->logic_view_class->base_logic_block_list.size()
                 + mainwindow->logic_view_class->apply_logic_block_list.size()
                 + mainwindow->logic_view_class->delay_counter_block_list.indexOf(this);
    QString temp_logic_function = "\r\nreturn set_emu_data(" + QString::number(emu_id) + ",";
    switch (config_block_data.config_param_data.model_id) {
    case MODEL_ID_LOGIC_DELAY_OFF:
        temp_logic_function +=
            "delay_logic_off(" + QString::number(mainwindow->logic_view_class->delay_off_list.indexOf(this)) + ","
            + QString::number(on_off_delay_time) + "," + input_point_list[0]->parent_attribute.function_name + "))";
        break;
    case MODEL_ID_LOGIC_DELAY_ON:
        temp_logic_function +=
            "delay_logic_on(" + QString::number(mainwindow->logic_view_class->delay_on_list.indexOf(this)) + ","
            + QString::number(on_off_delay_time) + "," + input_point_list[0]->parent_attribute.function_name + "))";
        break;
    case MODEL_ID_LOGIC_DELAY_ADJUST_OFF:
        temp_logic_function +=
            "delay_adjust_off(" + QString::number(mainwindow->logic_view_class->delay_adjust_off_list.indexOf(this));
        for (int i = 0; i < 4; i++) {
            temp_logic_function += "," + QString::number(adjust_on_off_delay_time[i]);
        }
        for (int i = 0; i < 5; i++) {
            if (i < get_input_point_num()) {
                temp_logic_function += "," + input_point_list[i]->parent_attribute.function_name;
            } else {
                temp_logic_function += ",false";
            }
        }
        temp_logic_function += "))";
        break;
    case MODEL_ID_LOGIC_DELAY_ADJUST_ON:
        temp_logic_function +=
            "delay_adjust_on(" + QString::number(mainwindow->logic_view_class->delay_adjust_on_list.indexOf(this));
        for (int i = 0; i < 4; i++) {
            temp_logic_function += "," + QString::number(adjust_on_off_delay_time[i]);
        }
        for (int i = 0; i < 5; i++) {
            if (i < get_input_point_num()) {
                temp_logic_function += "," + input_point_list[i]->parent_attribute.function_name;
            } else {
                temp_logic_function += ",false";
            }
        }
        temp_logic_function += "))";
        break;
    default:
        break;
    }
    if (temp_logic_function != attribute_data.logic_function) {
        attribute_data.logic_function = temp_logic_function;
    }
    send_attribute_data();
}

/* user slots */
void delay_counter_logic_block::update_state_slot()
{
    error_detect();
    logic_function_update();
    tooltip_update();
}