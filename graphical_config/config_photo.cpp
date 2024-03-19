#include "config_photo.h"
#include "mainwindow.h"
#include "md5.h"
config_photo::config_photo(const QString& filePath, QWidget* uiparent)
    : QGraphicsSvgItem(filePath)
{
    ui         = MainWindow::my_ui->ui;
    mainwindow = ( MainWindow* )uiparent;
    setAcceptHoverEvents(true);
    init(uiparent);
}

/**
 * @brief 矢量图界面配置初始化
 */
void config_photo::init(QWidget* uiparent)
{
    sys_config    = new config_param(1, 17, 25, 29, CONFIG_TOOL_SYS, uiparent, &module_param, this);
    can_config    = new config_param(1, 56, 25, 55, CONFIG_TOOL_CAN, uiparent, &module_param, this);
    ai_config     = new config_param(1, 117, 25, 133, CONFIG_TOOL_AI, uiparent, &module_param, this);
    di_config     = new config_param(1, 253, 25, 287, CONFIG_TOOL_DI, uiparent, &module_param, this);
    relay_config  = new config_param(273, 2, 25, 130, CONFIG_TOOL_RELAY, uiparent, &module_param, this);
    mos_config    = new config_param(273, 137, 25, 143, CONFIG_TOOL_MOS, uiparent, &module_param, this);
    encode_config = new config_param(273, 284, 25, 131, CONFIG_TOOL_ENCODE, uiparent, &module_param, this);
    pi_config     = new config_param(273, 418, 25, 124, CONFIG_TOOL_PI, uiparent, &module_param, this);
    config_rect_list.append(sys_config);
    config_rect_list.append(can_config);
    config_rect_list.append(ai_config);
    config_rect_list.append(di_config);
    config_rect_list.append(relay_config);
    config_rect_list.append(mos_config);
    config_rect_list.append(encode_config);
    config_rect_list.append(pi_config);
    for (uint8_t i = 0; i < config_rect_list.count(); i++) {
        config_rect_list[i]->setVisible(false);
    }
    module_param_init();
}

/********** 安全配置参数部分 ***************/
void config_photo::module_param_init()
{
    module_param.can_master_nodeID                = ( uint8_t )0x01;
    module_param.can_slave_nodeID_A               = ( uint8_t )0x51;
    module_param.can_slave_nodeID_B               = ( uint8_t )0x52;
    module_param.can_hb_consumer_gap              = 0;
    module_param.can_hb_producer_gap              = 1000;
    module_param.ai_slv.ai_slv_byte               = 0x03;
    module_param.di_slv.di_slv_bytes              = 0xff;
    module_param.relay_slv.relay_slv_byte         = 0x03;
    module_param.work_state.work_state_byte       = 0x35;
    module_param.safe_state.safe_state_byte       = 0X7E;
    module_param.piqep12_cross_check              = 0;
    module_param.qep12_cross_check                = 0;
    module_param.qep1_2_ratio                     = 100;
    module_param.piqep1_2_ratio                   = 100;
    module_param.qep_speed_allow_dif[0]           = 1;
    module_param.qep_speed_allow_dif[1]           = 3;
    module_param.piqep_speed_allow_dif[0]         = 1;
    module_param.piqep_speed_allow_dif[1]         = 3;
    module_param.sai_allow_dif[0]                 = 1;
    module_param.sai_allow_dif[1]                 = 3;
    module_param.spi_allow_dif[0]                 = 1;
    module_param.spi_allow_dif[1]                 = 3;
    module_param.pi_qep_allow_dif[0]              = 1;
    module_param.pi_qep_allow_dif[1]              = 3;
    module_param.sqep_allow_dif[0]                = 1;
    module_param.sqep_allow_dif[1]                = 3;
    module_param.sai_sample_interval              = 10;
    module_param.spi_sample_interval              = 10;
    module_param.pi_sqep_sample_interval          = 10;
    module_param.sqep_sample_interval             = 10;
    module_param.fault_code2_safe_state           = 0;
    module_param.fault_code2_safe_state_delaytime = 0;
    /*lua software version*/
    module_param.check_factor     = 1;
    module_param.can_baudrate     = 1000000;
    module_param.can_pdo_time_gap = 1000;
    /* 减速曲线类 */
    for (int i = 0; i < MAX_DECELERATE_NUM; i++) {
        module_param.qep_speed_select[i]                 = SPEED_SELECT_NONE;
        module_param.piqep_speed_select[i]               = SPEED_SELECT_NONE;
        module_param.decelerate_check_interval[i]        = 100;
        module_param.decelerate_sublevel_threshold[i][0] = 3000;
        module_param.decelerate_sublevel_threshold[i][1] = 2000;
        module_param.decelerate_sublevel_threshold[i][2] = 1000;
    }
    mbedtls_md5(( const unsigned char* )&module_param, sizeof(module_param) - sizeof(module_param.md5),
                module_param.md5);
}

module_param_t config_photo::get_module_param()
{
    mbedtls_md5(( const unsigned char* )&module_param, sizeof(module_param) - sizeof(module_param.md5),
                module_param.md5);
    return module_param;
}

/**
 * @brief 判断鼠标是否在配置块的范围内
 * @param pos：鼠标位置
 */
void config_photo::config_rect_visible(QPointF pos)
{
    for (uint8_t i = 0; i < config_rect_list.count(); i++) {
        if (config_rect_list[i]->rect().contains(pos)) {
            config_rect_list[i]->setVisible(true);
        } else {
            config_rect_list[i]->setVisible(false);
        }
    }
}

QJsonObject config_photo::param_project_info()
{
    QJsonObject rootObject;
    rootObject["baudrate"]   = ( int )module_param.can_baudrate;
    rootObject["anodeid"]    = module_param.can_slave_nodeID_A;
    rootObject["bnodeid"]    = module_param.can_slave_nodeID_B;
    rootObject["mnodeid"]    = module_param.can_master_nodeID;
    rootObject["checkbt"]    = module_param.can_hb_consumer_gap;
    rootObject["sendbt"]     = module_param.can_hb_producer_gap;
    rootObject["pdopt"]      = module_param.can_pdo_time_gap;
    rootObject["saisi"]      = module_param.sai_sample_interval;
    rootObject["spisi"]      = module_param.spi_sample_interval;
    rootObject["piqepsi"]    = module_param.pi_sqep_sample_interval;
    rootObject["sqepsi"]     = module_param.sqep_sample_interval;
    rootObject["fcsta"]      = module_param.fault_code2_safe_state;
    rootObject["fcdt"]       = module_param.fault_code2_safe_state_delaytime;
    rootObject["saifull"]    = module_param.sai_allow_dif[0];
    rootObject["saiac"]      = module_param.sai_allow_dif[1];
    rootObject["spifull"]    = module_param.spi_allow_dif[0];
    rootObject["spiac"]      = module_param.spi_allow_dif[1];
    rootObject["piqepfull"]  = module_param.pi_qep_allow_dif[0];
    rootObject["piqepac"]    = module_param.pi_qep_allow_dif[1];
    rootObject["sqepfull"]   = module_param.sqep_allow_dif[0];
    rootObject["sqepac"]     = module_param.sqep_allow_dif[1];
    rootObject["crossc"]     = module_param.check_factor;
    rootObject["dilevel"]    = module_param.di_slv.di_slv_bytes;
    rootObject["ailevel"]    = module_param.ai_slv.ai_slv_byte;
    rootObject["relaylevel"] = module_param.relay_slv.relay_slv_byte;
    rootObject["af"]         = module_param.work_state.work_state_byte;
    rootObject["ss"]         = module_param.safe_state.safe_state_byte;
    /* 减速保护相关 */
    rootObject["qepratio"]       = module_param.qep1_2_ratio;
    rootObject["piqepratio"]     = module_param.piqep1_2_ratio;
    rootObject["qepcross"]       = module_param.qep12_cross_check;
    rootObject["piqepcross"]     = module_param.piqep12_cross_check;
    rootObject["qepspeedfull"]   = module_param.qep_speed_allow_dif[0];
    rootObject["qepspeedac"]     = module_param.qep_speed_allow_dif[1];
    rootObject["piqepspeedfull"] = module_param.piqep_speed_allow_dif[0];
    rootObject["piqepspeedac"]   = module_param.piqep_speed_allow_dif[1];

    for (int i = 0; i < MAX_DECELERATE_NUM; i++) {
        rootObject["qepspeedselect" + QString::number(i)]            = module_param.qep_speed_select[i];
        rootObject["piqepspeedselect" + QString::number(i)]          = module_param.piqep_speed_select[i];
        rootObject["decelerate_check_interval" + QString::number(i)] = module_param.decelerate_check_interval[i];
        rootObject["decelerate_sublevel1_" + QString::number(i)] =
            ( int )module_param.decelerate_sublevel_threshold[i][0];
        rootObject["decelerate_sublevel2_" + QString::number(i)] =
            ( int )module_param.decelerate_sublevel_threshold[i][1];
        rootObject["decelerate_sublevel3_" + QString::number(i)] =
            ( int )module_param.decelerate_sublevel_threshold[i][2];
    }
    return rootObject;
}

bool config_photo::param_project_parse(QJsonObject project)
{
    if (project.isEmpty()) {
        return false;
    }
    module_param.can_baudrate                     = project["baudrate"].toInt();
    module_param.can_slave_nodeID_A               = project["anodeid"].toInt();
    module_param.can_slave_nodeID_B               = project["bnodeid"].toInt();
    module_param.can_master_nodeID                = project["mnodeid"].toInt();
    module_param.can_hb_consumer_gap              = project["checkbt"].toInt();
    module_param.can_hb_producer_gap              = project["sendbt"].toInt();
    module_param.can_pdo_time_gap                 = project["pdopt"].toInt();
    module_param.sai_sample_interval              = project["saisi"].toInt();
    module_param.spi_sample_interval              = project["spisi"].toInt();
    module_param.pi_sqep_sample_interval          = project["piqepsi"].toInt();
    module_param.sqep_sample_interval             = project["sqepsi"].toInt();
    module_param.fault_code2_safe_state           = project["fcsta"].toInt();
    module_param.fault_code2_safe_state_delaytime = project["fcdt"].toInt();
    module_param.sai_allow_dif[0]                 = project["saifull"].toInt();
    module_param.sai_allow_dif[1]                 = project["saiac"].toInt();
    module_param.spi_allow_dif[0]                 = project["spifull"].toInt();
    module_param.spi_allow_dif[1]                 = project["spiac"].toInt();
    module_param.pi_qep_allow_dif[0]              = project["piqepfull"].toInt();
    module_param.pi_qep_allow_dif[1]              = project["piqepac"].toInt();
    module_param.sqep_allow_dif[0]                = project["sqepfull"].toInt();
    module_param.sqep_allow_dif[1]                = project["sqepac"].toInt();
    module_param.check_factor                     = project["crossc"].toInt();
    module_param.di_slv.di_slv_bytes              = project["dilevel"].toInt();
    module_param.ai_slv.ai_slv_byte               = project["ailevel"].toInt();
    module_param.relay_slv.relay_slv_byte         = project["relaylevel"].toInt();
    module_param.work_state.work_state_byte       = project["af"].toInt();
    module_param.safe_state.safe_state_byte       = project["ss"].toInt();
    /* 速度保护相关 */
    module_param.qep1_2_ratio             = project["qepratio"].toInt();
    module_param.piqep1_2_ratio           = project["piqepratio"].toInt();
    module_param.qep12_cross_check        = project["qepcross"].toInt();
    module_param.piqep12_cross_check      = project["piqepcross"].toInt();
    module_param.qep_speed_allow_dif[0]   = project["qepspeedfull"].toInt();
    module_param.qep_speed_allow_dif[1]   = project["qepspeedac"].toInt();
    module_param.piqep_speed_allow_dif[0] = project["piqepspeedfull"].toInt();
    module_param.piqep_speed_allow_dif[1] = project["piqepspeedac"].toInt();

    for (int i = 0; i < MAX_DECELERATE_NUM; i++) {
        module_param.qep_speed_select[i]          = project["qepspeedselect" + QString::number(i)].toInt();
        module_param.piqep_speed_select[i]        = project["piqepspeedselect" + QString::number(i)].toInt();
        module_param.decelerate_check_interval[i] = project["decelerate_check_interval" + QString::number(i)].toInt();
        module_param.decelerate_sublevel_threshold[i][0] =
            project["decelerate_sublevel1_" + QString::number(i)].toInt();
        module_param.decelerate_sublevel_threshold[i][1] =
            project["decelerate_sublevel2_" + QString::number(i)].toInt();
        module_param.decelerate_sublevel_threshold[i][2] =
            project["decelerate_sublevel3_" + QString::number(i)].toInt();
    }
    return true;
}

void config_photo::param_ss_set(uint8_t state)
{
    module_param.safe_state.safe_state_byte = (state << 1);
}

uint8_t config_photo::param_ss_get()
{
    return module_param.safe_state.safe_state_byte;
}

/* sys event */
void config_photo::hoverMoveEvent(QGraphicsSceneHoverEvent* event)
{
    QPointF pos = event->pos();
    config_rect_visible(pos);
}

void config_photo::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    Q_UNUSED(event);
    for (uint8_t i = 0; i < config_rect_list.count(); i++) {
        config_rect_list[i]->setVisible(false);
    }
}
