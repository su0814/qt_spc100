#include "speed_logic_block.h"
#include "mainwindow.h"
#include <QDebug>
speed_logic_block::speed_logic_block(QPointF pos, config_block_data_t data, uint32_t uid, QWidget* uiparent,
                                     QGraphicsItem* parent)
    : base_rect_class(0, 0, defaultWidth, defaultHeight, uiparent, parent)
{
    attribute_data.uid = uid;
    setPos(pos.x() - defaultWidth / 2, pos.y() - defaultHeight / 2);
    config_block_data = data;
    self_init();
}

speed_logic_block::speed_logic_block(QJsonObject rootObject, QWidget* uiparent, QGraphicsItem* parent)
    : base_rect_class(0, 0, defaultWidth, defaultHeight, uiparent, parent)
{
    block_project_prase(rootObject);
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
}

speed_logic_block::speed_logic_block(QPointF pos, uint32_t uid, QJsonObject rootObject, QWidget* uiparent,
                                     QGraphicsItem* parent)
    : base_rect_class(0, 0, defaultWidth, defaultHeight, uiparent, parent)
{
    block_project_prase(rootObject, true, pos, uid);
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
}

void speed_logic_block::self_init()
{
    mainwindow->logic_view_class->speed_logic_block_list.append(this);
    attribute_data.parent_id.clear();
    set_display_name(config_block_data.source_name);
    set_display_pixmap(config_block_data.pixmap);
    set_display_uid(attribute_data.uid);
    attribute_data.function_name = "speedlogic" + QString::number(attribute_data.uid) + "_func()";
    set_right_menu_action(ACTION_SET | ACTION_DELETE);
    QStringList iname;
    QStringList oname;
    switch (config_block_data.config_param_data.model_id) {
    case MODEL_ID_LOGIC_SPEED_CROSS_CHECK:
        set_input_num(3);
        set_output_num(2);
        set_inputpoint_data_type(CONNECT_POINT_DATATYPE_ENCODER, 0);
        set_inputpoint_data_type(CONNECT_POINT_DATATYPE_ENCODER, 1);
        set_outputpoint_data_type(CONNECT_POINT_DATATYPE_ENCODER, 0);
        iname.clear();
        iname.append("编码器输入1");
        iname.append("编码器输入2");
        iname.append("复位");
        set_sys_inputpoint_labels(iname);
        oname.clear();
        oname.append("编码器输出1");
        oname.append("互检错误");
        oname.append("可靠性错误");
        set_sys_outputpoint_labels(oname);
        attribute_data.function_name = "speedlogic" + QString::number(attribute_data.uid) + "_func(outputid)";
        mainwindow->logic_view_class->speed_cross_check_list.append(this);
        cross_check_setting_dialog = new speed_cross_check_setting(this);
        if (mainwindow->logic_view_class->speed_cross_check_list.size() >= LOGIC_BLOCK_MAX_NUM) {
            resource_setenable(false);
        }
        break;
    case MODEL_ID_LOGIC_SPEED_DECELERATE_MONITOR:
        set_input_num(4);
        set_output_num(1);
        set_inputpoint_data_type(CONNECT_POINT_DATATYPE_ENCODER, 0);
        iname.clear();
        iname.append("编码器输入1");
        iname.append("减速开始");
        iname.append("复位");
        iname.append("减速选择1");
        iname.append("减速选择2");
        set_sys_inputpoint_labels(iname);
        oname.clear();
        oname.append("错误");
        set_sys_outputpoint_labels(oname);
        mainwindow->logic_view_class->speed_monitor_list.append(this);
        monitor_setting_dialog = new speed_monitor_setting(this);
        if (mainwindow->logic_view_class->speed_monitor_list.size() >= LOGIC_BLOCK_MAX_NUM) {
            resource_setenable(false);
        }
        break;
    case MODEL_ID_LOGIC_SPEED_MOTIONLESS_MONITOR:
        set_input_num(1);
        set_output_num(1);
        iname.clear();
        iname.append("编码器输入");
        set_sys_inputpoint_labels(iname);
        oname.clear();
        oname.append("静止");
        set_sys_outputpoint_labels(oname);
        set_inputpoint_data_type(CONNECT_POINT_DATATYPE_ENCODER, 0);
        mainwindow->logic_view_class->speed_motionless_list.append(this);
        speed_motionless_setting_dialog = new speed_motionless_setting(this);
        if (mainwindow->logic_view_class->speed_motionless_list.size() >= LOGIC_BLOCK_MAX_NUM) {
            resource_setenable(false);
        }
        break;
    case MODEL_ID_LOGIC_SPEED_VALUE_COMPAIRSONS:
        set_input_num(1);
        set_output_num(1);
        iname.clear();
        iname.append("编码器输入");
        set_sys_inputpoint_labels(iname);
        oname.clear();
        oname.append("成立");
        set_sys_outputpoint_labels(oname);
        set_inputpoint_data_type(CONNECT_POINT_DATATYPE_ENCODER, 0);
        mainwindow->logic_view_class->speed_value_compairsons_list.append(this);
        value_compairsons_setting_dialog = new speed_value_compairsons_setting(this);
        if (mainwindow->logic_view_class->speed_value_compairsons_list.size() >= LOGIC_BLOCK_MAX_NUM) {
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

QJsonObject speed_logic_block::block_project_info()
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
    case MODEL_ID_LOGIC_SPEED_CROSS_CHECK:
        rootObject["outputmode"]    = encoder_output_mode;
        rootObject["full"]          = crosscheck_percentage[0];
        rootObject["actual"]        = crosscheck_percentage[1];
        rootObject["errorkeeptime"] = error_keep_time;
        rootObject["reliability"]   = encoder_reliability_monitor;
        rootObject["maxtime"]       = reliability_monitor_max_time;
        rootObject["minspeed"]      = reliability_monitor_min_speed;
        break;
    case MODEL_ID_LOGIC_SPEED_DECELERATE_MONITOR:
        rootObject["rampnum"]       = ramp_num;
        rootObject["rampdelaytime"] = ramp_delay_time;
        for (int i = 0; i < 4; i++) {
            rootObject["ramptime" + QString::number(i)]  = ramp_time[i];
            rootObject["rampspeed" + QString::number(i)] = ramp_speed[i];
            rootObject["maxspeed" + QString::number(i)]  = ramp_max_speed[i];
            rootObject["minspeed" + QString::number(i)]  = ramp_min_speed[i];
        }
        break;
    case MODEL_ID_LOGIC_SPEED_MOTIONLESS_MONITOR:
        rootObject["motionlessspeed"]   = motionless_speed;
        rootObject["motionlessmintime"] = motionless_min_time;
        break;
    case MODEL_ID_LOGIC_SPEED_VALUE_COMPAIRSONS:
        rootObject["calcmode"]   = calc_mode;
        rootObject["speedvalue"] = speed_value;
        rootObject["mintime"]    = min_time;
        break;
    default:
        break;
    }
    return rootObject;
}

void speed_logic_block::block_project_prase(QJsonObject rootObject, bool copy, QPointF pos, uint32_t uid)
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
    config_block_data.config_param_data.model_iotype = rootObject["miotype"].toInt();
    config_block_data.config_param_data.model_type   = rootObject["mtype"].toInt();
    config_block_data.config_param_data.model_id     = rootObject["mid"].toInt();
    config_block_data.source_name                    = rootObject["sname"].toString();
    config_block_data.pixmap                         = rootObject["pixmap"].toString();
    switch (config_block_data.config_param_data.model_id) {
    case MODEL_ID_LOGIC_SPEED_CROSS_CHECK:
        encoder_output_mode           = rootObject["outputmode"].toInt();
        crosscheck_percentage[0]      = rootObject["full"].toInt();
        crosscheck_percentage[1]      = rootObject["actual"].toInt();
        error_keep_time               = rootObject["errorkeeptime"].toInt();
        encoder_reliability_monitor   = rootObject["reliability"].toBool();
        reliability_monitor_max_time  = rootObject["maxtime"].toInt();
        reliability_monitor_min_speed = rootObject["minspeed"].toInt();
        break;
    case MODEL_ID_LOGIC_SPEED_DECELERATE_MONITOR:
        ramp_num        = rootObject["rampnum"].toInt();
        ramp_delay_time = rootObject["rampdelaytime"].toInt();
        for (int i = 0; i < 4; i++) {
            ramp_time[i]      = rootObject["ramptime" + QString::number(i)].toInt();
            ramp_speed[i]     = rootObject["rampspeed" + QString::number(i)].toInt();
            ramp_max_speed[i] = rootObject["maxspeed" + QString::number(i)].toInt();
            ramp_min_speed[i] = rootObject["minspeed" + QString::number(i)].toInt();
        }
        break;
    case MODEL_ID_LOGIC_SPEED_MOTIONLESS_MONITOR:
        motionless_speed    = rootObject["motionlessspeed"].toInt();
        motionless_min_time = rootObject["motionlessmintime"].toInt();
        break;
    case MODEL_ID_LOGIC_SPEED_VALUE_COMPAIRSONS:
        calc_mode   = rootObject["calcmode"].toInt();
        speed_value = rootObject["speedvalue"].toInt();
        min_time    = rootObject["mintime"].toInt();
        break;
    default:
        break;
    }
}

void speed_logic_block::debug_data_parse(uint8_t res)
{
    switch (config_block_data.config_param_data.model_id) {
    case MODEL_ID_LOGIC_SPEED_CROSS_CHECK:
        for (int i = 1; i < 3; i++) {
            output_point_list[i]->send_debug_data((res >> i) & 0x01);
        }
    case MODEL_ID_LOGIC_SPEED_DECELERATE_MONITOR:
    case MODEL_ID_LOGIC_SPEED_MOTIONLESS_MONITOR:
    case MODEL_ID_LOGIC_SPEED_VALUE_COMPAIRSONS:
        output_point_list[0]->send_debug_data(res & 0x01);
        break;
    default:
        break;
    }
}

void speed_logic_block::action_delete_callback()
{
    if (mainwindow->logic_view_class->draw_line_state != DRAW_LINE_STATE_IDLE) {
        return;  //连线状态下不可删除
    }
    update_timer.stop();
    mainwindow->logic_view_class->speed_logic_block_list.removeOne(this);
    switch (config_block_data.config_param_data.model_id) {
    case MODEL_ID_LOGIC_SPEED_CROSS_CHECK:
        mainwindow->logic_view_class->speed_cross_check_list.removeOne(this);
        break;
    case MODEL_ID_LOGIC_SPEED_DECELERATE_MONITOR:
        mainwindow->logic_view_class->speed_monitor_list.removeOne(this);
        break;
    case MODEL_ID_LOGIC_SPEED_MOTIONLESS_MONITOR:
        mainwindow->logic_view_class->speed_motionless_list.removeOne(this);
        break;
    case MODEL_ID_LOGIC_SPEED_VALUE_COMPAIRSONS:
        mainwindow->logic_view_class->speed_value_compairsons_list.removeOne(this);
        break;
    }
    resource_setenable(true);
    scene()->removeItem(this);
    delete this;
}

void speed_logic_block::action_set_callback()
{
    switch (config_block_data.config_param_data.model_id) {
    case MODEL_ID_LOGIC_SPEED_CROSS_CHECK:
        cross_check_setting_dialog->setting_exec();
        break;
    case MODEL_ID_LOGIC_SPEED_DECELERATE_MONITOR:
        monitor_setting_dialog->setting_exec();
        break;
    case MODEL_ID_LOGIC_SPEED_MOTIONLESS_MONITOR:
        speed_motionless_setting_dialog->setting_exec();
        break;
    case MODEL_ID_LOGIC_SPEED_VALUE_COMPAIRSONS:
        value_compairsons_setting_dialog->setting_exec();
        break;
    default:
        break;
    }
}

void speed_logic_block::resource_setenable(bool isenable)
{
    logic_element* source_item = mainwindow->logic_menu_class->get_function_item(config_block_data);
    if (source_item != nullptr) {
        source_item->setDisabled(!isenable);
    }
}

void speed_logic_block::tooltip_update()
{
    QStringList output_mode_item;
    output_mode_item.append("编码器输入1");
    output_mode_item.append("编码器输入2");
    output_mode_item.append("编码器输入1与编码器输入2的最大值");
    output_mode_item.append("编码器输入1与编码器输入2的最小值");
    output_mode_item.append("编码器输入1与编码器输入2的平均值");
    QStringList calcmode;
    calcmode.append("大于");
    calcmode.append("小于");
    calcmode.append("等于");
    calcmode.append("大于等于");
    calcmode.append("小于等于");
    QString tooltip;
    tooltip += "uid: " + QString::number(attribute_data.uid);
    tooltip += "\r\n" + config_block_data.source_name;
    switch (config_block_data.config_param_data.model_id) {
    case MODEL_ID_LOGIC_SPEED_CROSS_CHECK:
        tooltip += "\r\n编码器输出模式: " + output_mode_item[encoder_output_mode];
        tooltip += "\r\n互检容差满量程占比: " + QString::number(crosscheck_percentage[0]) + "%";
        tooltip += "\r\n互检容差编码器输入1占比: " + QString::number(crosscheck_percentage[1]) + "%";
        tooltip += "\r\n互检容忍时间: " + QString::number(error_keep_time) + "ms";
        if (encoder_reliability_monitor) {
            tooltip += "\r\n编码器可靠性检测: ";
            tooltip += "\r\n 可靠性检测最大时间: " + QString::number(reliability_monitor_max_time) + "ms";
            tooltip += "\r\n 可靠性检测最小速度: " + QString::number(reliability_monitor_max_time) + "mm/s";
        }
        break;
    case MODEL_ID_LOGIC_SPEED_DECELERATE_MONITOR:
        tooltip += "\r\n减速开始延时时间: " + QString::number(ramp_delay_time) + "ms";
        for (int i = 0; i < ramp_num; i++) {
            tooltip += "\r\n减速检测" + QString::number(i + 1) + ": ";
            tooltip +=
                "\r\n 每" + QString::number(ramp_time[i]) + "ms速度下降" + QString::number(ramp_speed[i]) + "mm/s";
            tooltip += "\r\n 最大安全速度: " + QString::number(ramp_max_speed[i]) + "mm/s";
            tooltip += "\r\n 最小安全速度: " + QString::number(ramp_min_speed[i]) + "mm/s";
        }
        break;
    case MODEL_ID_LOGIC_SPEED_MOTIONLESS_MONITOR:
        tooltip += "\r\n静止速度阈值: " + QString::number(motionless_speed) + "mm/s";
        tooltip += "\r\n静止最小持续时间: " + QString::number(motionless_min_time) + "ms";
        break;
    case MODEL_ID_LOGIC_SPEED_VALUE_COMPAIRSONS:
        tooltip += "\r\n比较模式: " + calcmode[calc_mode];
        tooltip += "\r\n比较阈值: " + QString::number(speed_value) + "mm/s";
        break;
    default:
        break;
    }
    setToolTip(tooltip);
}

void speed_logic_block::logic_function_update()
{
    if (error) {
        attribute_data.logic_function.clear();
        return;
    }
    int emu_id = mainwindow->logic_view_class->input_block_list.size()
                 + mainwindow->logic_view_class->output_block_list.size()
                 + mainwindow->logic_view_class->base_logic_block_list.size()
                 + mainwindow->logic_view_class->apply_logic_block_list.size()
                 + mainwindow->logic_view_class->delay_counter_block_list.size()
                 + mainwindow->logic_view_class->speed_logic_block_list.indexOf(this);
    QString temp_logic_function = "return ";
    switch (config_block_data.config_param_data.model_id) {
    case MODEL_ID_LOGIC_SPEED_CROSS_CHECK:
        temp_logic_function += "speed_cross_check(" + QString::number(emu_id) + ","
                               + QString::number(mainwindow->logic_view_class->speed_cross_check_list.indexOf(this))
                               + ",";
        if (encoder_reliability_monitor) {
            temp_logic_function += "true,";
        } else {
            temp_logic_function += "false,";
        }
        temp_logic_function += QString::number(encoder_output_mode) + ",";
        temp_logic_function += QString::number(crosscheck_percentage[0]) + ",";
        temp_logic_function += QString::number(crosscheck_percentage[1]) + ",";
        temp_logic_function += QString::number(error_keep_time) + ",";
        temp_logic_function += QString::number(reliability_monitor_max_time) + ",";
        temp_logic_function += QString::number(reliability_monitor_min_speed) + ",";
        temp_logic_function += input_point_list[0]->parent_attribute.function_name + ",";
        temp_logic_function += input_point_list[1]->parent_attribute.function_name + ",";
        temp_logic_function +=
            QString::number(input_point_list[0]->parent_attribute.config_block_data->config_param_data.model_id) + ",";
        temp_logic_function += input_point_list[2]->parent_attribute.function_name + ",outputid)";
        for (int i = 0; i < get_output_point_num(); i++) {
            output_point_list[i]->self_attribute.function_name =
                "speedlogic" + QString::number(attribute_data.uid) + "_func(" + QString::number(i) + ")";
        }
        break;
    case MODEL_ID_LOGIC_SPEED_DECELERATE_MONITOR:
        temp_logic_function += "speed_monitor(" + QString::number(emu_id) + ","
                               + QString::number(mainwindow->logic_view_class->speed_monitor_list.indexOf(this)) + ",";
        for (int i = 0; i < 5; i++) {
            if (ramp_num <= 2 && i == 4) {
                temp_logic_function += "false,";
            } else {
                temp_logic_function += input_point_list[i]->parent_attribute.function_name + ",";
            }
        }
        temp_logic_function += QString::number(ramp_delay_time) + ",";
        temp_logic_function += QString::number(ramp_num);
        for (int i = 0; i < 4; i++) {
            temp_logic_function += "," + QString::number(ramp_time[i]);
        }
        for (int i = 0; i < 4; i++) {
            temp_logic_function += "," + QString::number(ramp_speed[i]);
        }
        for (int i = 0; i < 4; i++) {
            temp_logic_function += "," + QString::number(ramp_max_speed[i]);
        }
        for (int i = 0; i < 4; i++) {
            temp_logic_function += "," + QString::number(ramp_min_speed[i]);
        }
        temp_logic_function += ")";
        break;
    case MODEL_ID_LOGIC_SPEED_MOTIONLESS_MONITOR:
        temp_logic_function += "speed_motionless_monitor(" + QString::number(emu_id) + ","
                               + QString::number(mainwindow->logic_view_class->speed_monitor_list.indexOf(this)) + ",";
        temp_logic_function += input_point_list[0]->parent_attribute.function_name + ",";
        temp_logic_function += QString::number(motionless_speed) + ",";
        temp_logic_function += QString::number(motionless_min_time);
        temp_logic_function += ")";
        break;
    case MODEL_ID_LOGIC_SPEED_VALUE_COMPAIRSONS:
        temp_logic_function += "speed_value_compairson(" + QString::number(emu_id) + ","
                               + QString::number(mainwindow->logic_view_class->speed_monitor_list.indexOf(this)) + ",";
        temp_logic_function += input_point_list[0]->parent_attribute.function_name + ",";
        temp_logic_function += QString::number(speed_value) + ",";
        temp_logic_function += QString::number(calc_mode) + ",";
        temp_logic_function += QString::number(min_time);
        temp_logic_function += ")";
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
void speed_logic_block::update_state_slot()
{
    error_detect();
    logic_function_update();
    tooltip_update();
}
