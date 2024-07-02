#include "apply_logic_block.h"
#include "mainwindow.h"
#include <QDebug>
apply_logic_block::apply_logic_block(QPointF pos, config_block_data_t data, uint32_t uid, QWidget* uiparent,
                                     QGraphicsItem* parent)
    : base_rect_class(0, 0, defaultWidth, defaultHeight, uiparent, parent)
{
    attribute_data.uid = uid;
    setPos(pos.x() - defaultWidth / 2, pos.y() - defaultHeight / 2);
    config_block_data = data;
    self_init();
}

apply_logic_block::apply_logic_block(QJsonObject rootObject, QWidget* uiparent, QGraphicsItem* parent)
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
    set_input_mask(rootObject["inmask"].toInt());
    set_output_mask(rootObject["outmask"].toInt());
    set_user_inputpoint_labels(inlabels);
    set_user_outputpoint_labels(outlabels);
    set_input_reverse_data(rootObject["reverse"].toInt());
}

apply_logic_block::apply_logic_block(QPointF pos, uint32_t uid, QJsonObject rootObject, QWidget* uiparent,
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
    set_input_mask(rootObject["inmask"].toInt());
    set_output_mask(rootObject["outmask"].toInt());
    set_user_inputpoint_labels(inlabels);
    set_user_outputpoint_labels(outlabels);
    set_input_reverse_data(rootObject["reverse"].toInt());
}

void apply_logic_block::self_init()
{
    mainwindow->logic_view_class->apply_logic_block_list.append(this);
    attribute_data.parent_id.clear();
    set_display_name(config_block_data.source_name);
    set_display_pixmap(config_block_data.pixmap);
    set_display_uid(attribute_data.uid);
    attribute_data.function_name = "applylogic" + QString::number(attribute_data.uid) + "_func()";
    set_right_menu_action(ACTION_SET | ACTION_DELETE);
    QStringList iname;
    QStringList oname;
    switch (config_block_data.config_param_data.model_id) {
    case MODEL_ID_LOGIC_APPLICATION_RESET:
        set_input_mask(0x03);
        set_output_mask(0x03);
        iname.clear();
        iname.append("复位");
        for (int i = 0; i < 7; i++) {
            iname.append("释放" + QString::number(i + 1));
        }
        set_sys_inputpoint_labels(iname);
        oname.clear();
        oname.append("启用");
        oname.append("要求复位");
        set_sys_outputpoint_labels(oname);
        attribute_data.function_name = "applylogic" + QString::number(attribute_data.uid) + "_func(outputid)";
        reset_setting_dialog         = new apply_reset_setting(this);
        mainwindow->logic_view_class->apply_reset_block_list.append(this);
        if (mainwindow->logic_view_class->apply_reset_block_list.size() >= LOGIC_BLOCK_MAX_NUM) {
            resource_setenable(false);
        }
        break;
    case MODEL_ID_LOGIC_APPLICATION_EDMONITOR:
        set_input_mask(0x03);
        set_output_mask(0x03);
        iname.clear();
        iname.append("反馈信号");
        iname.append("控制");
        set_sys_inputpoint_labels(iname);
        oname.clear();
        oname.append("设备错误");
        oname.append("输出");
        set_sys_outputpoint_labels(oname);
        attribute_data.function_name          = "applylogic" + QString::number(attribute_data.uid) + "_func(outputid)";
        extern_device_monitors_setting_dialog = new apply_extern_device_monitors_setting(this);
        mainwindow->logic_view_class->apply_extern_device_monitor_list.append(this);
        if (mainwindow->logic_view_class->apply_extern_device_monitor_list.size() >= LOGIC_BLOCK_MAX_NUM) {
            resource_setenable(false);
        }
        break;
    case MODEL_ID_LOGIC_APPLICATION_FREQ_DETECT:
        set_input_mask(0x01);
        set_output_mask(0x07);
        iname.clear();
        iname.append("频率1");
        iname.append("频率2");
        set_sys_inputpoint_labels(iname);
        oname.clear();
        oname.append("输出1");
        oname.append("频率1错误");
        oname.append("频率1持续高");
        oname.append("输出2");
        oname.append("频率2错误");
        oname.append("频率2持续高");
        oname.append("故障");
        set_sys_outputpoint_labels(oname);
        attribute_data.function_name = "applylogic" + QString::number(attribute_data.uid) + "_func(outputid,fault)";
        freq_monitor_setting_dialog  = new apply_freq_monitor_setting(this);
        mainwindow->logic_view_class->apply_freq_monitor_list.append(this);
        if (mainwindow->logic_view_class->apply_freq_monitor_list.size() >= LOGIC_BLOCK_MAX_NUM) {
            resource_setenable(false);
        }
        break;
    case MODEL_ID_LOGIC_APPLICATION_EDGE_DETECT:
        set_input_mask(0x01);
        set_output_mask(0x01);
        edge_detected_setting_dialog = new apply_edge_detected_setting(this);
        mainwindow->logic_view_class->apply_edge_detected_list.append(this);
        if (mainwindow->logic_view_class->apply_edge_detected_list.size() >= LOGIC_BLOCK_MAX_NUM) {
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

QJsonObject apply_logic_block::block_project_info()
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
    rootObject["inmask"]  = get_input_point_mask();
    rootObject["outmask"] = get_output_point_mask();
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
    case MODEL_ID_LOGIC_APPLICATION_RESET:
        rootObject["resetpulsetime"] = apply_reset_param.min_reset_pulse_time;
        break;
    case MODEL_ID_LOGIC_APPLICATION_EDMONITOR:
        rootObject["feedbackdelay"] = apply_edm_param.max_feedback_delay;
        break;
    case MODEL_ID_LOGIC_APPLICATION_FREQ_DETECT:
        rootObject["freq2enable"] = apply_freq_monitor_param.freq_enable;
        rootObject["freqfault"]   = apply_freq_monitor_param.fault_output;
        for (int i = 0; i < 8; i++) {
            rootObject["freqparam" + QString::number(i)] = apply_freq_monitor_param.freq_param[i];
        }
        break;
    case MODEL_ID_LOGIC_APPLICATION_EDGE_DETECT:
        rootObject["edgemode"] = apply_edge_detect_param.edge_detected_mode;
        break;
    default:
        break;
    }
    return rootObject;
}

void apply_logic_block::block_project_prase(QJsonObject rootObject, bool copy, QPointF pos, uint32_t uid)
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
    case MODEL_ID_LOGIC_APPLICATION_RESET:
        apply_reset_param.min_reset_pulse_time = rootObject["resetpulsetime"].toInt();
        break;
    case MODEL_ID_LOGIC_APPLICATION_EDMONITOR:
        apply_edm_param.max_feedback_delay = rootObject["feedbackdelay"].toInt();
        break;
    case MODEL_ID_LOGIC_APPLICATION_FREQ_DETECT:
        apply_freq_monitor_param.freq_enable  = rootObject["freq2enable"].toBool();
        apply_freq_monitor_param.fault_output = rootObject["freqfault"].toBool();
        for (int i = 0; i < 8; i++) {
            apply_freq_monitor_param.freq_param[i] = rootObject["freqparam" + QString::number(i)].toInt();
        }
        break;
    case MODEL_ID_LOGIC_APPLICATION_EDGE_DETECT:
        apply_edge_detect_param.edge_detected_mode = rootObject["edgemode"].toInt();
        break;
    default:
        break;
    }
}

QJsonObject apply_logic_block::block_param_info()
{
    QJsonObject rootObject;
    rootObject["inmask"]  = get_input_point_mask();
    rootObject["outmask"] = get_output_point_mask();
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
    case MODEL_ID_LOGIC_APPLICATION_RESET:
        rootObject["resetpulsetime"] = apply_reset_param.min_reset_pulse_time;
        break;
    case MODEL_ID_LOGIC_APPLICATION_EDMONITOR:
        rootObject["feedbackdelay"] = apply_edm_param.max_feedback_delay;
        break;
    case MODEL_ID_LOGIC_APPLICATION_FREQ_DETECT:
        rootObject["freq2enable"] = apply_freq_monitor_param.freq_enable;
        rootObject["freqfault"]   = apply_freq_monitor_param.fault_output;
        for (int i = 0; i < 8; i++) {
            rootObject["freqparam" + QString::number(i)] = apply_freq_monitor_param.freq_param[i];
        }
        break;
    case MODEL_ID_LOGIC_APPLICATION_EDGE_DETECT:
        rootObject["edgemode"] = apply_edge_detect_param.edge_detected_mode;
        break;
    default:
        break;
    }
    return rootObject;
}

void apply_logic_block::block_param_prase(QJsonObject rootObject)
{
    QStringList inlabels, outlabels;
    for (int i = 0; i < MAX_CONNECT_POINT_NUM; i++) {
        inlabels.append(rootObject["ilabel" + QString::number(i)].toString());
    }
    for (int i = 0; i < MAX_CONNECT_POINT_NUM; i++) {
        outlabels.append(rootObject["olabel" + QString::number(i)].toString());
    }
    set_input_mask(rootObject["inmask"].toInt());
    set_output_mask(rootObject["outmask"].toInt());
    set_user_inputpoint_labels(inlabels);
    set_user_outputpoint_labels(outlabels);
    set_input_reverse_data(rootObject["reverse"].toInt());
    switch (config_block_data.config_param_data.model_id) {
    case MODEL_ID_LOGIC_APPLICATION_RESET:
        apply_reset_param.min_reset_pulse_time = rootObject["resetpulsetime"].toInt();
        break;
    case MODEL_ID_LOGIC_APPLICATION_EDMONITOR:
        apply_edm_param.max_feedback_delay = rootObject["feedbackdelay"].toInt();
        break;
    case MODEL_ID_LOGIC_APPLICATION_FREQ_DETECT:
        apply_freq_monitor_param.freq_enable  = rootObject["freq2enable"].toBool();
        apply_freq_monitor_param.fault_output = rootObject["freqfault"].toBool();
        for (int i = 0; i < 8; i++) {
            apply_freq_monitor_param.freq_param[i] = rootObject["freqparam" + QString::number(i)].toInt();
        }
        break;
    case MODEL_ID_LOGIC_APPLICATION_EDGE_DETECT:
        apply_edge_detect_param.edge_detected_mode = rootObject["edgemode"].toInt();
        break;
    default:
        break;
    }
}

void apply_logic_block::debug_data_parse(uint8_t res)
{
    switch (config_block_data.config_param_data.model_id) {
    case MODEL_ID_LOGIC_APPLICATION_RESET:
    case MODEL_ID_LOGIC_APPLICATION_EDMONITOR:
    case MODEL_ID_LOGIC_APPLICATION_EDGE_DETECT:
    case MODEL_ID_LOGIC_APPLICATION_FREQ_DETECT:
        for (int i = 0; i < MAX_CONNECT_POINT_NUM; i++) {
            if (get_output_point_mask() & (0x01 << i)) {
                output_point_list[i]->send_debug_data((res >> i) & 0x01);
            }
        }
        break;
        break;
    default:
        break;
    }
}

void apply_logic_block::action_delete_callback()
{
    if (mainwindow->logic_view_class->draw_line_state != DRAW_LINE_STATE_IDLE) {
        return;  //连线状态下不可删除
    }
    update_timer.stop();
    mainwindow->logic_view_class->apply_logic_block_list.removeOne(this);
    switch (config_block_data.config_param_data.model_id) {
    case MODEL_ID_LOGIC_APPLICATION_RESET:
        mainwindow->logic_view_class->apply_reset_block_list.removeOne(this);
        break;
    case MODEL_ID_LOGIC_APPLICATION_EDMONITOR:
        mainwindow->logic_view_class->apply_extern_device_monitor_list.removeOne(this);
        break;
    case MODEL_ID_LOGIC_APPLICATION_FREQ_DETECT:
        mainwindow->logic_view_class->apply_freq_monitor_list.removeOne(this);
        break;
    case MODEL_ID_LOGIC_APPLICATION_EDGE_DETECT:
        mainwindow->logic_view_class->apply_edge_detected_list.removeOne(this);
        break;
    default:
        break;
    }
    resource_setenable(true);
    scene()->removeItem(this);
    delete this;
}

void apply_logic_block::action_set_callback()
{
    switch (config_block_data.config_param_data.model_id) {
    case MODEL_ID_LOGIC_APPLICATION_RESET:
        reset_setting_dialog->setting_exec();
        break;
    case MODEL_ID_LOGIC_APPLICATION_EDMONITOR:
        extern_device_monitors_setting_dialog->setting_exec();
        break;
    case MODEL_ID_LOGIC_APPLICATION_FREQ_DETECT:
        freq_monitor_setting_dialog->setting_exec();
        break;
    case MODEL_ID_LOGIC_APPLICATION_EDGE_DETECT:
        edge_detected_setting_dialog->setting_exec();
        break;
    default:
        break;
    }
}

void apply_logic_block::resource_setenable(bool isenable)
{
    logic_element* source_item = mainwindow->logic_menu_class->item_exist_detect(config_block_data);
    if (source_item != nullptr) {
        source_item->setDisabled(!isenable);
    }
}

void apply_logic_block::tooltip_update()
{
    QString tooltip;
    tooltip += "uid: " + QString::number(attribute_data.uid);
    tooltip += "\r\n" + config_block_data.source_name;
    QStringList edge;
    edge.append("上升沿");
    edge.append("下降沿");
    edge.append("上升沿+下降沿");
    switch (config_block_data.config_param_data.model_id) {
    case MODEL_ID_LOGIC_APPLICATION_RESET:
        tooltip += "\r\n最小复位脉冲时间: " + QString::number(apply_reset_param.min_reset_pulse_time) + " ms";
        break;
    case MODEL_ID_LOGIC_APPLICATION_EDMONITOR:
        tooltip += "\r\n最大反馈延时: " + QString::number(apply_edm_param.max_feedback_delay) + " ms";
        break;
    case MODEL_ID_LOGIC_APPLICATION_FREQ_DETECT:
        tooltip += "\r\n频率1:";
        tooltip += "\r\n最小周期时间: " + QString::number(apply_freq_monitor_param.freq_param[0]) + " ms";
        tooltip += "\r\n最大周期时间: " + QString::number(apply_freq_monitor_param.freq_param[1]) + " ms";
        tooltip += "\r\n脉冲时间: " + QString::number(apply_freq_monitor_param.freq_param[2]) + " ms";
        tooltip += "\r\n脉冲时间容差: " + QString::number(apply_freq_monitor_param.freq_param[3]) + " ms";
        if (apply_freq_monitor_param.freq_enable) {
            tooltip += "\r\n频率2:";
            tooltip += "\r\n最小周期时间: " + QString::number(apply_freq_monitor_param.freq_param[4]) + " ms";
            tooltip += "\r\n最大周期时间: " + QString::number(apply_freq_monitor_param.freq_param[5]) + " ms";
            tooltip += "\r\n脉冲时间: " + QString::number(apply_freq_monitor_param.freq_param[6]) + " ms";
            tooltip += "\r\n脉冲时间容差: " + QString::number(apply_freq_monitor_param.freq_param[7]) + " ms";
        }
        break;
    case MODEL_ID_LOGIC_APPLICATION_EDGE_DETECT:
        tooltip += "\r\n检测的边沿: " + edge[apply_edge_detect_param.edge_detected_mode];
        break;
    default:
        break;
    }
    setToolTip(tooltip);
}

void apply_logic_block::logic_function_update()
{
    if (error) {
        attribute_data.logic_function.clear();
        return;
    }
    QString temp_logic_function = "";
    int     emu_id              = mainwindow->logic_view_class->input_block_list.size()
                 + mainwindow->logic_view_class->output_block_list.size()
                 + mainwindow->logic_view_class->base_logic_block_list.size()
                 + mainwindow->logic_view_class->apply_logic_block_list.indexOf(this);
    switch (config_block_data.config_param_data.model_id) {
    case MODEL_ID_LOGIC_APPLICATION_RESET:
        temp_logic_function += "return apply_logic_reset(" + QString::number(emu_id) + ","
                               + QString::number(mainwindow->logic_view_class->apply_reset_block_list.indexOf(this))
                               + "," + QString::number(apply_reset_param.min_reset_pulse_time) + ",";
        for (int i = 0; i < MAX_CONNECT_POINT_NUM; i++) {
            if (get_input_point_mask() & (0X01 << i)) {
                temp_logic_function += input_point_list[i]->parent_attribute.function_name + ",";
            } else {
                temp_logic_function += "true,";
            }
        }
        temp_logic_function += "outputid)";
        output_point_list[0]->self_attribute.function_name =
            "applylogic" + QString::number(attribute_data.uid) + "_func(0)";
        output_point_list[1]->self_attribute.function_name =
            "applylogic" + QString::number(attribute_data.uid) + "_func(1)";
        break;
    case MODEL_ID_LOGIC_APPLICATION_EDMONITOR:
        temp_logic_function +=
            "return apply_logic_external_device_monitorin(" + QString::number(emu_id) + ","
            + QString::number(mainwindow->logic_view_class->apply_extern_device_monitor_list.indexOf(this)) + ","
            + QString::number(apply_edm_param.max_feedback_delay) + ",";
        for (int i = 0; i < 2; i++) {
            temp_logic_function += input_point_list[i]->parent_attribute.function_name + ",";
        }
        temp_logic_function += "outputid)";
        output_point_list[0]->self_attribute.function_name =
            "applylogic" + QString::number(attribute_data.uid) + "_func(0)";
        output_point_list[1]->self_attribute.function_name =
            "applylogic" + QString::number(attribute_data.uid) + "_func(1)";
        break;
    case MODEL_ID_LOGIC_APPLICATION_FREQ_DETECT:
        temp_logic_function += "return apply_logic_freq_monitor(" + QString::number(emu_id) + ","
                               + QString::number(mainwindow->logic_view_class->apply_freq_monitor_list.indexOf(this))
                               + ",";
        if (apply_freq_monitor_param.freq_enable) {
            temp_logic_function += "true," + input_point_list[0]->parent_attribute.function_name + ","
                                   + input_point_list[1]->parent_attribute.function_name + ",";
        } else {
            temp_logic_function += "false," + input_point_list[0]->parent_attribute.function_name + ",false,";
        }
        for (int i = 0; i < 8; i++) {
            temp_logic_function += QString::number(apply_freq_monitor_param.freq_param[i]) + ",";
        }
        temp_logic_function += "outputid,fault)";
        for (int i = 0; i < MAX_CONNECT_POINT_NUM; i++) {
            if (get_output_point_mask() & (0x01 << i)) {
                if (apply_freq_monitor_param.fault_output && i == 6) {
                    output_point_list[i]->self_attribute.function_name =
                        "applylogic" + QString::number(attribute_data.uid) + "_func(0,true)";
                } else {
                    output_point_list[i]->self_attribute.function_name =
                        "applylogic" + QString::number(attribute_data.uid) + "_func(" + QString::number(i) + ",false)";
                }
            }
        }
        break;
    case MODEL_ID_LOGIC_APPLICATION_EDGE_DETECT:
        temp_logic_function += "return apply_logic_edge_detected(" + QString::number(emu_id) + ","
                               + QString::number(mainwindow->logic_view_class->apply_edge_detected_list.indexOf(this))
                               + "," + QString::number(apply_edge_detect_param.edge_detected_mode) + ","
                               + input_point_list[0]->parent_attribute.function_name + ")";
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
void apply_logic_block::update_state_slot()
{
    error_detect();
    logic_function_update();
    tooltip_update();
}
