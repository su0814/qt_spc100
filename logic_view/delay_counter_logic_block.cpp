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

delay_counter_logic_block::delay_counter_logic_block(QPointF pos, uint32_t uid, QJsonObject rootObject,
                                                     QWidget* uiparent, QGraphicsItem* parent)
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

void delay_counter_logic_block::self_init()
{
    mainwindow->logic_view_class->delay_counter_block_list.append(this);
    attribute_data.parent_id.clear();
    set_display_name(config_block_data.source_name);
    set_display_pixmap(config_block_data.pixmap);
    set_display_uid(attribute_data.uid);
    attribute_data.function_name = "delaycounterlogic" + QString::number(attribute_data.uid) + "_func()";
    set_right_menu_action(ACTION_SET | ACTION_DELETE);
    QStringList iname;
    QStringList oname;
    switch (config_block_data.config_param_data.model_id) {
    case MODEL_ID_LOGIC_DELAY_OFF:
        set_input_mask(0x01);
        set_output_mask(0x01);
        mainwindow->logic_view_class->delay_off_list.append(this);
        on_off_setting_dialog = new delay_on_off_setting(this);
        if (mainwindow->logic_view_class->delay_off_list.size() >= LOGIC_BLOCK_MAX_NUM) {
            resource_setenable(false);
        }
        break;
    case MODEL_ID_LOGIC_DELAY_ON:
        set_input_mask(0x01);
        set_output_mask(0x01);
        mainwindow->logic_view_class->delay_on_list.append(this);
        on_off_setting_dialog = new delay_on_off_setting(this);
        if (mainwindow->logic_view_class->delay_on_list.size() >= LOGIC_BLOCK_MAX_NUM) {
            resource_setenable(false);
        }
        break;
    case MODEL_ID_LOGIC_DELAY_ADJUST_OFF:
        set_input_mask(0x03);
        set_output_mask(0x01);
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
        set_input_mask(0x03);
        set_output_mask(0x01);
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
    case MODEL_ID_LOGIC_COUNTER_EVENT:
        set_input_mask(0x0f);
        set_output_mask(0x03);
        iname.clear();
        iname.append("向上");
        iname.append("向下");
        iname.append("重置装载值");
        iname.append("重置为0");
        set_sys_inputpoint_labels(iname);
        oname.clear();
        oname.append("上溢");
        oname.append("下溢");
        set_sys_outputpoint_labels(oname);
        mainwindow->logic_view_class->counter_event_list.append(this);
        counter_event_setting_dialog = new counter_event_setting(this);
        attribute_data.function_name = "delaycounterlogic" + QString::number(attribute_data.uid) + "_func(outputid)";
        if (mainwindow->logic_view_class->counter_event_list.size() >= LOGIC_BLOCK_MAX_NUM) {
            resource_setenable(false);
        }
        break;
    case MODEL_ID_LOGIC_COUNTER_LOGGING:
        set_input_mask(0x01);
        mainwindow->logic_view_class->counter_logging_list.append(this);
        counter_logging_setting_dialog = new counter_logging_setting(this);
        if (mainwindow->logic_view_class->counter_logging_list.size() >= 2) {
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
    case MODEL_ID_LOGIC_DELAY_OFF:
    case MODEL_ID_LOGIC_DELAY_ON:
        rootObject["onoffdelay"] = delay_on_off_param.on_off_delay_time;
        break;
    case MODEL_ID_LOGIC_DELAY_ADJUST_OFF:
    case MODEL_ID_LOGIC_DELAY_ADJUST_ON:
        for (int i = 0; i < 4; i++) {
            rootObject["adjustonoffdelay" + QString::number(i)] = delay_adjust_on_off_param.adjust_on_off_delay_time[i];
        }
        break;
    case MODEL_ID_LOGIC_COUNTER_EVENT:
        for (int i = 0; i < EVENT_COUNTER_PARAM_NUM; i++) {
            rootObject["event_counter_param" + QString::number(i)] = counter_event_param.event_counter_param[i];
        }
        break;
    case MODEL_ID_LOGIC_COUNTER_LOGGING:
        for (int i = 0; i < 8; i++) {
            rootObject["logedge" + QString::number(i)] = counter_logging_param.log_edge[i];
            rootObject["logtext" + QString::number(i)] = counter_logging_param.log_text[i];
        }
        break;
    default:
        break;
    }
    return rootObject;
}

void delay_counter_logic_block::block_project_prase(QJsonObject rootObject, bool copy, QPointF pos, uint32_t uid)
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
    case MODEL_ID_LOGIC_DELAY_OFF:
    case MODEL_ID_LOGIC_DELAY_ON:
        delay_on_off_param.on_off_delay_time = rootObject["onoffdelay"].toInt();
        break;
    case MODEL_ID_LOGIC_DELAY_ADJUST_OFF:
    case MODEL_ID_LOGIC_DELAY_ADJUST_ON:
        for (int i = 0; i < 4; i++) {
            delay_adjust_on_off_param.adjust_on_off_delay_time[i] =
                rootObject["adjustonoffdelay" + QString::number(i)].toInt();
        }
        break;
    case MODEL_ID_LOGIC_COUNTER_EVENT:
        for (int i = 0; i < EVENT_COUNTER_PARAM_NUM; i++) {
            counter_event_param.event_counter_param[i] = rootObject["event_counter_param" + QString::number(i)].toInt();
        }
        break;
    case MODEL_ID_LOGIC_COUNTER_LOGGING:
        for (int i = 0; i < 8; i++) {
            counter_logging_param.log_edge[i] = rootObject["logedge" + QString::number(i)].toInt();
            counter_logging_param.log_text[i] = rootObject["logtext" + QString::number(i)].toString();
        }
        break;
    default:
        break;
    }
}

QJsonObject delay_counter_logic_block::block_param_info()
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
    case MODEL_ID_LOGIC_DELAY_OFF:
    case MODEL_ID_LOGIC_DELAY_ON:
        rootObject["onoffdelay"] = delay_on_off_param.on_off_delay_time;
        break;
    case MODEL_ID_LOGIC_DELAY_ADJUST_OFF:
    case MODEL_ID_LOGIC_DELAY_ADJUST_ON:
        for (int i = 0; i < 4; i++) {
            rootObject["adjustonoffdelay" + QString::number(i)] = delay_adjust_on_off_param.adjust_on_off_delay_time[i];
        }
        break;
    case MODEL_ID_LOGIC_COUNTER_EVENT:
        for (int i = 0; i < EVENT_COUNTER_PARAM_NUM; i++) {
            rootObject["event_counter_param" + QString::number(i)] = counter_event_param.event_counter_param[i];
        }
        break;
    case MODEL_ID_LOGIC_COUNTER_LOGGING:
        for (int i = 0; i < 8; i++) {
            rootObject["logedge" + QString::number(i)] = counter_logging_param.log_edge[i];
            rootObject["logtext" + QString::number(i)] = counter_logging_param.log_text[i];
        }
        break;
    default:
        break;
    }
    return rootObject;
}

void delay_counter_logic_block::block_param_prase(QJsonObject rootObject)
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
    case MODEL_ID_LOGIC_DELAY_OFF:
    case MODEL_ID_LOGIC_DELAY_ON:
        delay_on_off_param.on_off_delay_time = rootObject["onoffdelay"].toInt();
        break;
    case MODEL_ID_LOGIC_DELAY_ADJUST_OFF:
    case MODEL_ID_LOGIC_DELAY_ADJUST_ON:
        for (int i = 0; i < 4; i++) {
            delay_adjust_on_off_param.adjust_on_off_delay_time[i] =
                rootObject["adjustonoffdelay" + QString::number(i)].toInt();
        }
        break;
    case MODEL_ID_LOGIC_COUNTER_EVENT:
        for (int i = 0; i < EVENT_COUNTER_PARAM_NUM; i++) {
            counter_event_param.event_counter_param[i] = rootObject["event_counter_param" + QString::number(i)].toInt();
        }
        break;
    case MODEL_ID_LOGIC_COUNTER_LOGGING:
        for (int i = 0; i < 8; i++) {
            counter_logging_param.log_edge[i] = rootObject["logedge" + QString::number(i)].toInt();
            counter_logging_param.log_text[i] = rootObject["logtext" + QString::number(i)].toString();
        }
        break;
    default:
        break;
    }
}

void delay_counter_logic_block::debug_data_parse(uint8_t res)
{
    switch (config_block_data.config_param_data.model_id) {
    case MODEL_ID_LOGIC_DELAY_OFF:
    case MODEL_ID_LOGIC_DELAY_ON:
    case MODEL_ID_LOGIC_DELAY_ADJUST_OFF:
    case MODEL_ID_LOGIC_DELAY_ADJUST_ON:
    case MODEL_ID_LOGIC_COUNTER_EVENT:
    case MODEL_ID_LOGIC_COUNTER_LOGGING:
        for (int i = 0; i < MAX_CONNECT_POINT_NUM; i++) {
            if (get_output_point_mask() & (0x01 << i)) {
                output_point_list[i]->send_debug_data((res >> i) & 0x01);
            }
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
    case MODEL_ID_LOGIC_COUNTER_EVENT:
        mainwindow->logic_view_class->counter_event_list.removeOne(this);
        break;
    case MODEL_ID_LOGIC_COUNTER_LOGGING:
        mainwindow->logic_view_class->counter_logging_list.removeOne(this);
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
    case MODEL_ID_LOGIC_COUNTER_EVENT:
        counter_event_setting_dialog->setting_exec();
        break;
    case MODEL_ID_LOGIC_COUNTER_LOGGING:
        counter_logging_setting_dialog->setting_exec();
        break;
    default:
        break;
    }
}

void delay_counter_logic_block::resource_setenable(bool isenable)
{
    logic_element* source_item = mainwindow->logic_menu_class->item_exist_detect(config_block_data);
    if (source_item != nullptr) {
        source_item->setDisabled(!isenable);
    }
}

void delay_counter_logic_block::tooltip_update()
{
    QStringList mode;
    mode.append("自动");
    mode.append("手动");
    QStringList edge;
    edge.append("上升沿");
    edge.append("下降沿");
    edge.append("上升沿+下降沿");
    QString tooltip;
    tooltip += "uid: " + QString::number(attribute_data.uid);
    tooltip += "\r\n" + config_block_data.source_name;
    switch (config_block_data.config_param_data.model_id) {
    case MODEL_ID_LOGIC_DELAY_OFF:
    case MODEL_ID_LOGIC_DELAY_ON:
        tooltip += "\r\n延迟时间: " + QString::number(delay_on_off_param.on_off_delay_time) + " ms";
        break;
    case MODEL_ID_LOGIC_DELAY_ADJUST_OFF:
    case MODEL_ID_LOGIC_DELAY_ADJUST_ON:
        for (int i = 0; i < 4; i++) {
            tooltip += "\r\n延迟时间" + QString::number(i) + ": "
                       + QString::number(delay_adjust_on_off_param.adjust_on_off_delay_time[i]) + " ms";
        }
        break;
    case MODEL_ID_LOGIC_COUNTER_EVENT:
        tooltip += "计数值上溢后重置为0模式：" + mode[counter_event_param.event_counter_param[UPOVER_RESET_MODE]];
        tooltip += "计数值下溢后重置重载值模式：" + mode[counter_event_param.event_counter_param[DOWNOVER_RESET_MODE]];
        tooltip += "上溢溢出值：" + QString::number(counter_event_param.event_counter_param[UPOVER_VALUE]);
        tooltip += "重载值：" + QString::number(counter_event_param.event_counter_param[START_VALUE]);
        tooltip += "重置为0的最小脉冲时间"
                   + QString::number(counter_event_param.event_counter_param[RESET_ZERO_PULSE_TIME]) + "ms";
        tooltip += "重置重载值的最小脉冲时间"
                   + QString::number(counter_event_param.event_counter_param[RESET_START_PULSE_TIME]) + "ms";
        tooltip +=
            "自动重置输出保持时间" + QString::number(counter_event_param.event_counter_param[RESET_KEEP_TIME]) + "ms";
        break;
    case MODEL_ID_LOGIC_COUNTER_LOGGING:
        for (int i = 0; i < MAX_CONNECT_POINT_NUM; i++) {
            if (get_input_point_mask() & (0x01 << i)) {
                tooltip += "\r\n输入" + QString::number(i) + ": " + edge[counter_logging_param.log_edge[i] - 1] + ","
                           + counter_logging_param.log_text[i];
            }
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
                 + mainwindow->logic_view_class->output_block_list.size()
                 + mainwindow->logic_view_class->base_logic_block_list.size()
                 + mainwindow->logic_view_class->apply_logic_block_list.size()
                 + mainwindow->logic_view_class->delay_counter_block_list.indexOf(this);
    QString temp_logic_function = "\r\nreturn set_emu_data(" + QString::number(emu_id) + ",";
    switch (config_block_data.config_param_data.model_id) {
    case MODEL_ID_LOGIC_DELAY_OFF:
        temp_logic_function += "delay_logic_off("
                               + QString::number(mainwindow->logic_view_class->delay_off_list.indexOf(this)) + ","
                               + QString::number(delay_on_off_param.on_off_delay_time) + ","
                               + input_point_list[0]->parent_attribute.function_name + "))";
        break;
    case MODEL_ID_LOGIC_DELAY_ON:
        temp_logic_function += "delay_logic_on("
                               + QString::number(mainwindow->logic_view_class->delay_on_list.indexOf(this)) + ","
                               + QString::number(delay_on_off_param.on_off_delay_time) + ","
                               + input_point_list[0]->parent_attribute.function_name + "))";
        break;
    case MODEL_ID_LOGIC_DELAY_ADJUST_OFF:
        temp_logic_function +=
            "delay_adjust_off(" + QString::number(mainwindow->logic_view_class->delay_adjust_off_list.indexOf(this));
        for (int i = 0; i < 4; i++) {
            temp_logic_function += "," + QString::number(delay_adjust_on_off_param.adjust_on_off_delay_time[i]);
        }
        for (int i = 0; i < 5; i++) {
            if (get_input_point_mask() & (0x01 << i)) {
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
            temp_logic_function += "," + QString::number(delay_adjust_on_off_param.adjust_on_off_delay_time[i]);
        }
        for (int i = 0; i < 5; i++) {
            if (get_input_point_mask() & (0x01 << i)) {
                temp_logic_function += "," + input_point_list[i]->parent_attribute.function_name;
            } else {
                temp_logic_function += ",false";
            }
        }
        temp_logic_function += "))";
        break;
    case MODEL_ID_LOGIC_COUNTER_EVENT:
        temp_logic_function = "\r\nreturn counter_event(" + QString::number(emu_id) + ","
                              + QString::number(mainwindow->logic_view_class->counter_event_list.indexOf(this)) + ",";
        for (int i = 0; i < MAX_CONNECT_POINT_NUM; i++) {
            if (get_input_point_mask() & (0x01 << i)) {
                temp_logic_function += input_point_list[i]->parent_attribute.function_name + ",";
            }
        }
        for (int i = 0; i < EVENT_COUNTER_PARAM_NUM; i++) {
            temp_logic_function += QString::number(counter_event_param.event_counter_param[i]) + ",";
        }
        temp_logic_function += "outputid)";
        for (int i = 0; i < MAX_CONNECT_POINT_NUM; i++) {
            if (get_output_point_mask() & (0x01 << i)) {
                output_point_list[i]->self_attribute.function_name =
                    "delaycounterlogic" + QString::number(attribute_data.uid) + "_func(" + QString::number(i) + ")";
            }
        }
        break;
    case MODEL_ID_LOGIC_COUNTER_LOGGING:
        temp_logic_function =
            "\r\n counter_logging(" + QString::number(mainwindow->logic_view_class->counter_logging_list.indexOf(this));
        for (int i = 0; i < MAX_CONNECT_POINT_NUM; i++) {
            if (get_input_point_mask() & (0x01 << i)) {
                temp_logic_function += "," + input_point_list[i]->parent_attribute.function_name;
            } else {
                temp_logic_function += ",false";
            }
        }
        for (int i = 0; i < 8; i++) {
            temp_logic_function += "," + QString::number(counter_logging_param.log_edge[i]);
        }
        for (int i = 0; i < MAX_CONNECT_POINT_NUM; i++) {
            if (get_input_point_mask() & (0x01 << i)) {
                if (counter_logging_param.log_text[i].isEmpty()) {
                    temp_logic_function +=
                        ",\"日志记录" + QString::number(attribute_data.uid) + ": 输入" + QString::number(i) + "触发\"";
                } else {
                    temp_logic_function += ",\"" + counter_logging_param.log_text[i] + "\"";
                }
            } else {
                temp_logic_function += ",\"\"";
            }
        }
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
void delay_counter_logic_block::update_state_slot()
{
    error_detect();
    logic_function_update();
    tooltip_update();
}
