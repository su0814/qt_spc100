#include "base_logic_block.h"
#include "mainwindow.h"
#include <QDebug>
base_logic_block::base_logic_block(QPointF pos, config_block_data_t data, uint32_t uid, QWidget* uiparent,
                                   QGraphicsItem* parent)
    : base_rect_class(0, 0, defaultWidth, defaultHeight, uiparent, parent)
{

    attribute_data.uid = uid;
    setPos(pos.x() - defaultWidth / 2, pos.y() - defaultHeight / 2);
    config_block_data = data;
    self_init();
}

base_logic_block::base_logic_block(QJsonObject rootObject, QWidget* uiparent, QGraphicsItem* parent)
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
base_logic_block::base_logic_block(QPointF pos, uint32_t uid, QJsonObject rootObject, QWidget* uiparent,
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
void base_logic_block::self_init()
{
    mainwindow->logic_view_class->base_logic_block_list.append(this);
    attribute_data.parent_id.clear();
    set_display_name(config_block_data.source_name);
    set_display_pixmap(config_block_data.pixmap);
    set_display_uid(attribute_data.uid);
    attribute_data.function_name = "baselogic" + QString::number(attribute_data.uid) + "_func()";
    set_right_menu_action(ACTION_SET | ACTION_DELETE);
    QStringList iname;
    QStringList oname;
    switch (config_block_data.config_param_data.model_id) {
    case MODEL_ID_LOGIC_BASE_AND:
    case MODEL_ID_LOGIC_BASE_OR:
        set_input_num(2);
        set_output_num(1);
        and_or_setting_dialog = new and_or_logic_setting(this);
        break;
    case MODEL_ID_LOGIC_BASE_NOT:
        set_input_num(1);
        set_output_num(1);
        not_xor_xnor_setting_dialog = new not_xor_xnor_logic_setting(this);
        break;
    case MODEL_ID_LOGIC_BASE_XNOR:
    case MODEL_ID_LOGIC_BASE_XOR:
        set_input_num(2);
        set_output_num(1);
        not_xor_xnor_setting_dialog = new not_xor_xnor_logic_setting(this);
        break;
    case MODEL_ID_LOGIC_BASE_RS:
        set_input_num(2);
        set_output_num(2);
        iname.clear();
        iname.append("设置");
        iname.append("复位");
        set_sys_inputpoint_labels(iname);
        oname.clear();
        oname.append("Q");
        oname.append("/Q");
        set_sys_outputpoint_labels(oname);
        rs_trigger_setting_dialog    = new rs_trigger_logic_setting(this);
        attribute_data.function_name = "baselogic" + QString::number(attribute_data.uid) + "_func(outputid)";
        mainwindow->logic_view_class->rs_trigger_logic_block_list.append(this);
        if (mainwindow->logic_view_class->rs_trigger_logic_block_list.size() >= LOGIC_BLOCK_MAX_NUM) {
            resource_setenable(false);
        }
        break;
    case MODEL_ID_LOGIC_BASE_ENCODER:
        set_input_num(2);
        set_output_num(1);
        oname.clear();
        oname.append("输出A");
        oname.append("输出B");
        oname.append("输出C");
        oname.append("故障");
        set_sys_outputpoint_labels(oname);
        nencode_setting_dialog       = new nencode_logic_setting(this);
        attribute_data.function_name = "baselogic" + QString::number(attribute_data.uid) + "_func(outputid,fault)";
        break;
    case MODEL_ID_LOGIC_BASE_DECODER:
        set_input_num(1);
        set_output_num(2);
        iname.clear();
        iname.append("输入A");
        iname.append("输入B");
        iname.append("输入C");
        set_sys_inputpoint_labels(iname);
        ndecoder_setting_dialog      = new ndecoder_logic_setting(this);
        attribute_data.function_name = "baselogic" + QString::number(attribute_data.uid) + "_func(outputid)";
        break;
    default:
        break;
    }
    set_inputpoint_attribute(&attribute_data);
    set_outputpoint_attribute(&attribute_data);
    connect(&update_timer, &QTimer::timeout, this, update_state_slot);  //状态更新定时器及槽函数
    update_timer.start(BLOCK_DATA_REFRESH_TIME);
}

QJsonObject base_logic_block::block_project_info()
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
    rootObject["encoder_fault"] = nencode_fault_output;
    return rootObject;
}

void base_logic_block::block_project_prase(QJsonObject rootObject, bool copy, QPointF pos, uint32_t uid)
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
    nencode_fault_output                             = rootObject["encoder_fault"].toBool();
}

void base_logic_block::debug_data_parse(uint8_t res)
{
    switch (config_block_data.config_param_data.model_id) {
    case MODEL_ID_LOGIC_BASE_AND:
    case MODEL_ID_LOGIC_BASE_OR:
    case MODEL_ID_LOGIC_BASE_NOT:
    case MODEL_ID_LOGIC_BASE_XNOR:
    case MODEL_ID_LOGIC_BASE_XOR:
    case MODEL_ID_LOGIC_BASE_RS:
    case MODEL_ID_LOGIC_BASE_DECODER:
        for (int i = 0; i < get_output_point_num(); i++) {
            output_point_list[i]->send_debug_data((res >> i) & 0x01);
        }
        break;
    case MODEL_ID_LOGIC_BASE_ENCODER:
        for (int i = 0; i < get_output_point_num(); i++) {
            if (nencode_fault_output && i == get_output_point_num() - 1) {
                output_point_list[i]->send_debug_data((res >> 7) & 0x01);
            } else {
                output_point_list[i]->send_debug_data((res >> i) & 0x01);
            }
        }
        break;
    default:
        break;
    }
}

void base_logic_block::action_delete_callback()
{
    if (mainwindow->logic_view_class->draw_line_state != DRAW_LINE_STATE_IDLE) {
        return;  //连线状态下不可删除
    }
    update_timer.stop();
    mainwindow->logic_view_class->base_logic_block_list.removeOne(this);
    if (config_block_data.config_param_data.model_id == MODEL_ID_LOGIC_BASE_RS) {
        mainwindow->logic_view_class->rs_trigger_logic_block_list.removeOne(this);
        resource_setenable(true);
    }
    scene()->removeItem(this);
    delete this;
}

void base_logic_block::action_set_callback()
{
    switch (config_block_data.config_param_data.model_id) {
    case MODEL_ID_LOGIC_BASE_AND:
    case MODEL_ID_LOGIC_BASE_OR:
        if (and_or_setting_dialog) {
            and_or_setting_dialog->setting_exec();
        }
        break;
    case MODEL_ID_LOGIC_BASE_NOT:
    case MODEL_ID_LOGIC_BASE_XNOR:
    case MODEL_ID_LOGIC_BASE_XOR:
        not_xor_xnor_setting_dialog->setting_exec();
        break;
    case MODEL_ID_LOGIC_BASE_RS:
        rs_trigger_setting_dialog->setting_exec();
        break;
    case MODEL_ID_LOGIC_BASE_ENCODER:
        nencode_setting_dialog->setting_exec();
        break;
    case MODEL_ID_LOGIC_BASE_DECODER:
        ndecoder_setting_dialog->setting_exec();
        break;
    default:
        break;
    }
}

void base_logic_block::resource_setenable(bool isenable)
{
    logic_element* source_item = mainwindow->logic_menu_class->get_function_item(config_block_data);
    if (source_item != nullptr) {
        source_item->setDisabled(!isenable);
    }
}

void base_logic_block::tooltip_update()
{
    QString tooltip;
    tooltip += "uid: " + QString::number(attribute_data.uid);
    tooltip += "\r\n" + config_block_data.source_name;
    QStringList inputlabels = get_sys_inputpoint_labels();
    QStringList reverse;
    int         reverse_data = get_input_reverse_data();
    reverse.append("不反向");
    reverse.append("反向");
    switch (config_block_data.config_param_data.model_id) {
    case MODEL_ID_LOGIC_BASE_AND:
    case MODEL_ID_LOGIC_BASE_OR:
    case MODEL_ID_LOGIC_BASE_RS:
        for (int i = 0; i < get_input_point_num(); i++) {
            tooltip += "\r\n" + inputlabels[i] + ": " + reverse[(reverse_data >> i) & 0x01];
        }
        break;
    case MODEL_ID_LOGIC_BASE_NOT:
    case MODEL_ID_LOGIC_BASE_XNOR:
    case MODEL_ID_LOGIC_BASE_XOR:
    case MODEL_ID_LOGIC_BASE_ENCODER:
    case MODEL_ID_LOGIC_BASE_DECODER:
        break;
    default:
        break;
    }
    setToolTip(tooltip);
}

void base_logic_block::logic_function_update()
{
    if (error) {
        attribute_data.logic_function.clear();
        return;
    }
    int emu_id = mainwindow->logic_view_class->input_block_list.size()
                 + mainwindow->logic_view_class->output_block_list.size()
                 + mainwindow->logic_view_class->base_logic_block_list.indexOf(this);
    QString     sub_str             = "\r\nreturn set_emu_data(" + QString::number(emu_id);
    QString     temp_logic_function = "";
    QStringList reverse;
    reverse << ""
            << " not ";
    int reverse_data = get_input_reverse_data();
    switch (config_block_data.config_param_data.model_id) {
    case MODEL_ID_LOGIC_BASE_AND:
        for (int i = 0; i < get_input_point_num(); i++) {
            if (i == 0) {
                temp_logic_function += "\r\nlocal result" + QString::number(i) + "= ("
                                       + reverse[(reverse_data >> i) & 0x01]
                                       + input_point_list[i]->parent_attribute.function_name + ")";
                sub_str += ",result" + QString::number(i);
            } else {
                temp_logic_function += "\r\nlocal result" + QString::number(i) + "=("
                                       + reverse[(reverse_data >> i) & 0x01]
                                       + input_point_list[i]->parent_attribute.function_name + ")";
                sub_str += " and result" + QString::number(i);
            }
        }
        temp_logic_function += sub_str + ")";
        break;
    case MODEL_ID_LOGIC_BASE_OR:
        for (int i = 0; i < get_input_point_num(); i++) {
            if (i == 0) {
                temp_logic_function += "\r\nlocal result" + QString::number(i) + "= ("
                                       + reverse[(reverse_data >> i) & 0x01]
                                       + input_point_list[i]->parent_attribute.function_name + ")";
                sub_str += ",result" + QString::number(i);
            } else {
                temp_logic_function += "\r\nlocal result" + QString::number(i) + "=("
                                       + reverse[(reverse_data >> i) & 0x01]
                                       + input_point_list[i]->parent_attribute.function_name + ")";
                sub_str += " or result" + QString::number(i);
            }
        }
        temp_logic_function += sub_str + ")";
        break;
    case MODEL_ID_LOGIC_BASE_NOT:
        temp_logic_function += "return set_emu_data(" + QString::number(emu_id) + ", not "
                               + input_point_list[0]->parent_attribute.function_name + ")";
        break;
    case MODEL_ID_LOGIC_BASE_XNOR:
        temp_logic_function += "return set_emu_data(" + QString::number(emu_id) + ",("
                               + input_point_list[0]->parent_attribute.function_name
                               + " == " + input_point_list[1]->parent_attribute.function_name + "))";
        break;
    case MODEL_ID_LOGIC_BASE_XOR:
        temp_logic_function += "return set_emu_data(" + QString::number(emu_id) + ",("
                               + input_point_list[0]->parent_attribute.function_name
                               + " ~= " + input_point_list[1]->parent_attribute.function_name + "))";
        break;
    case MODEL_ID_LOGIC_BASE_RS:
        temp_logic_function +=
            "return base_logic_rs_trigger(" + QString::number(emu_id) + ","
            + QString::number(mainwindow->logic_view_class->rs_trigger_logic_block_list.indexOf(this)) + ","
            + input_point_list[0]->parent_attribute.function_name + ","
            + input_point_list[1]->parent_attribute.function_name + ",outputid)";
        output_point_list[0]->self_attribute.function_name =
            "baselogic" + QString::number(attribute_data.uid) + "_func(0)";
        output_point_list[1]->self_attribute.function_name =
            "baselogic" + QString::number(attribute_data.uid) + "_func(1)";
        break;
    case MODEL_ID_LOGIC_BASE_ENCODER:
        temp_logic_function += "return base_logic_encoder(" + QString::number(emu_id) + ",";
        for (int i = 0; i < 8; i++) {
            if (i < get_input_point_num()) {
                temp_logic_function += input_point_list[i]->parent_attribute.function_name + ",";
            } else {
                temp_logic_function += "false,";
            }
        }
        temp_logic_function += "outputid,fault)";
        for (int i = 0; i < get_output_point_num(); i++) {
            if (nencode_fault_output && i == (get_output_point_num() - 1)) {
                output_point_list[i]->self_attribute.function_name =
                    "baselogic" + QString::number(attribute_data.uid) + "_func(0,true)";
            } else {
                output_point_list[i]->self_attribute.function_name =
                    "baselogic" + QString::number(attribute_data.uid) + "_func(" + QString::number(i) + ",false)";
            }
        }
        break;
    case MODEL_ID_LOGIC_BASE_DECODER:
        temp_logic_function += "return base_logic_decoder(" + QString::number(emu_id) + ",";
        for (int i = 0; i < 3; i++) {
            if (i < get_input_point_num()) {
                temp_logic_function += input_point_list[i]->parent_attribute.function_name + ",";
            } else {
                temp_logic_function += "false,";
            }
        }
        temp_logic_function += "outputid)";
        for (int i = 0; i < get_output_point_num(); i++) {
            output_point_list[i]->self_attribute.function_name =
                "baselogic" + QString::number(attribute_data.uid) + "_func(" + QString::number(i) + ")";
        }
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
void base_logic_block::update_state_slot()
{
    error_detect();
    logic_function_update();
    tooltip_update();
}
