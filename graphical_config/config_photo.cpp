#include "config_photo.h"
#include "algorithm/MD5/md5.h"
#include "mainwindow.h"
#define LED_WIDTH  (8.87)
#define LED_HEIGHT (6.04)
config_photo::config_photo(const QString& filePath, QWidget* uiparent)
    : QGraphicsSvgItem(filePath)
{
    ui         = MainWindow::my_ui->ui;
    mainwindow = ( MainWindow* )uiparent;
    setAcceptHoverEvents(true);
    setCursor(Qt::ArrowCursor);
    init(uiparent);
}

/**
 * @brief 矢量图界面配置初始化
 */
void config_photo::init(QWidget* uiparent)
{
    config_param_data_t config_param_data;
    int                 left_x = 12, right_x = 310;
    int                 block_width = 18;
    /* AI */
    int a_ai_height[2]             = { 31, 31 };
    int a_ai_y[2]                  = { 137, 171 };
    int b_ai_height[2]             = { 31, 31 };
    int b_ai_y[2]                  = { 205, 245 };
    config_param_data.model_type   = MODEL_INPUT_AI;
    config_param_data.model_iotype = MODEL_TYPE_INPUT;
    config_param_data.label_dir    = LABEL_DIR_LEFT;

    for (int i = 0; i < 2; i++) {
        config_param_data.model_id = i;
        a_ai_config_list.append(new config_param(left_x, a_ai_y[i], block_width, a_ai_height[i], uiparent, this));
        b_ai_config_list.append(new config_param(left_x, b_ai_y[i], block_width, b_ai_height[i], uiparent, this));
        config_param_data.source_mcu = SOURCE_MCU_A;
        config_param_data.cat3_model = b_ai_config_list[i];
        a_ai_config_list[i]->set_block_data(config_param_data);
        config_param_data.source_mcu = SOURCE_MCU_B;
        config_param_data.cat3_model = a_ai_config_list[i];
        b_ai_config_list[i]->set_block_data(config_param_data);
    }
    /* DI */
    int a_di_height[12]            = { 14, 14, 14, 14, 14, 31, 14, 14, 31, 31, 31, 31 };
    int a_di_y[12]                 = { 280, 297, 314, 331, 348, 365, 399, 423, 137, 171, 457, 491 };
    int b_di_height[12]            = { 14, 14, 14, 14, 14, 31, 14, 14, 31, 31, 31, 31 };
    int b_di_y[12]                 = { 440, 457, 474, 491, 508, 525, 559, 576, 205, 245, 525, 559 };
    config_param_data.model_type   = MODEL_INPUT_DI;
    config_param_data.model_iotype = MODEL_TYPE_INPUT;
    for (int i = 0; i < 12; i++) {
        if (i < 10) {
            config_param_data.label_dir = LABEL_DIR_LEFT;
            a_di_config_list.append(new config_param(left_x, a_di_y[i], block_width, a_di_height[i], uiparent, this));
            b_di_config_list.append(new config_param(left_x, b_di_y[i], block_width, b_di_height[i], uiparent, this));
        } else {
            config_param_data.label_dir = LABEL_DIR_RIGHT;
            a_di_config_list.append(new config_param(right_x, a_di_y[i], block_width, a_di_height[i], uiparent, this));
            b_di_config_list.append(new config_param(right_x, b_di_y[i], block_width, b_di_height[i], uiparent, this));
        }
        config_param_data.model_id   = i;
        config_param_data.source_mcu = SOURCE_MCU_A;
        config_param_data.cat3_model = b_di_config_list[i];
        a_di_config_list[i]->set_block_data(config_param_data);
        config_param_data.source_mcu = SOURCE_MCU_B;
        config_param_data.cat3_model = a_di_config_list[i];
        b_di_config_list[i]->set_block_data(config_param_data);
    }

    /* RELAY */
    int a_relay_height[2]          = { 31, 38 };
    int a_relay_y[2]               = { 10, 44 };
    int b_relay_height[2]          = { 31, 31 };
    int b_relay_y[2]               = { 85, 119 };
    config_param_data.label_dir    = LABEL_DIR_RIGHT;
    config_param_data.model_type   = MODEL_OUTPUT_RELAY_MOS;
    config_param_data.model_iotype = MODEL_TYPE_OUTPUT;
    for (int i = 0; i < 2; i++) {
        a_relay_config_list.append(
            new config_param(right_x, a_relay_y[i], block_width, a_relay_height[i], uiparent, this));
        b_relay_config_list.append(
            new config_param(right_x, b_relay_y[i], block_width, b_relay_height[i], uiparent, this));
        config_param_data.model_id   = i;
        config_param_data.source_mcu = SOURCE_MCU_A;
        config_param_data.cat3_model = b_relay_config_list[i];
        a_relay_config_list[i]->set_block_data(config_param_data);
        config_param_data.source_mcu = SOURCE_MCU_B;
        config_param_data.cat3_model = a_relay_config_list[i];
        b_relay_config_list[i]->set_block_data(config_param_data);
    }
    /* MOS */
    int mos_height[4]              = { 31, 31, 31, 31 };
    int mos_y[4]                   = { 153, 187, 245, 280 };
    config_param_data.model_iotype = MODEL_TYPE_OUTPUT;
    config_param_data.source_mcu   = SOURCE_MCU_SAFE;
    config_param_data.cat3_model   = nullptr;
    for (int i = 0; i < 4; i++) {
        mos_config_list.append(new config_param(right_x, mos_y[i], block_width, mos_height[i], uiparent, this));
        config_param_data.model_id = i + 2;
        mos_config_list[i]->set_block_data(config_param_data);
    }

    /* QEP */
    int qep_height[4]              = { 65, 71, 65, 65 };
    int qep_y[4]                   = { 314, 382, 457, 525 };
    config_param_data.model_type   = MODEL_INPUT_QEP;
    config_param_data.model_iotype = MODEL_TYPE_INPUT;
    for (int i = 0; i < 4; i += 2) {
        qep_config_list.append(new config_param(right_x, qep_y[i], block_width, qep_height[i], uiparent, this));
        qep_config_list.append(new config_param(right_x, qep_y[i + 1], block_width, qep_height[i + 1], uiparent, this));
        config_param_data.model_id   = i;
        config_param_data.cat3_model = qep_config_list[i + 1];
        config_param_data.source_mcu = SOURCE_MCU_A;
        qep_config_list[i]->set_block_data(config_param_data);
        config_param_data.model_id   = i + 1;
        config_param_data.cat3_model = qep_config_list[i];
        config_param_data.source_mcu = SOURCE_MCU_B;
        qep_config_list[i + 1]->set_block_data(config_param_data);
    }

    /* PI */
    int a_pi_height[2]             = { 31, 31 };
    int a_pi_y[2]                  = { 457, 491 };
    int b_pi_height[2]             = { 31, 31 };
    int b_pi_y[2]                  = { 525, 559 };
    config_param_data.model_type   = MODEL_INPUT_PI;
    config_param_data.model_iotype = MODEL_TYPE_INPUT;
    config_param_data.source_mcu   = SOURCE_MCU_A;
    for (int i = 0; i < 2; i++) {
        a_pi_config_list.append(new config_param(right_x, a_pi_y[i], block_width, a_pi_height[i], uiparent, this));
        b_pi_config_list.append(new config_param(right_x, b_pi_y[i], block_width, b_pi_height[i], uiparent, this));
        config_param_data.model_id   = i;
        config_param_data.source_mcu = SOURCE_MCU_A;
        config_param_data.cat3_model = b_pi_config_list[i];
        a_pi_config_list[i]->set_block_data(config_param_data);
        config_param_data.source_mcu = SOURCE_MCU_B;
        config_param_data.cat3_model = a_pi_config_list[i];
        b_pi_config_list[i]->set_block_data(config_param_data);
    }
    all_config_list.append(a_ai_config_list);
    all_config_list.append(b_ai_config_list);
    all_config_list.append(a_di_config_list);
    all_config_list.append(b_di_config_list);
    all_config_list.append(a_relay_config_list);
    all_config_list.append(b_relay_config_list);
    all_config_list.append(a_pi_config_list);
    all_config_list.append(b_pi_config_list);
    all_config_list.append(mos_config_list);
    all_config_list.append(qep_config_list);
    hide_available_source();
    /* fault */
    config_device_fault_rect = new config_device_fault(110, 600.8, 114, 32, this);
}

void config_photo::resource_led_init()
{
    qreal di_name_x[6] = { 136.79, 136.79, 124.54, 124.54, 112, 112 };
    qreal di_name_y[6] = { 253.3, 271.6, 253.3, 271.6, 253.3, 271.6 };
    QFont font("Arial", 6);
    font.setBold(true);
    for (int i = 0; i < 6; i++) {
        QGraphicsTextItem* name = new QGraphicsTextItem("." + QString::number(7 + i), this);
        name->setFont(font);
        name->setRotation(90);
        name->setPos(di_name_x[i], di_name_y[i]);
    }

    QPen  pen(Qt::NoPen);
    qreal a_di_x[12] = { 124.54, 124.54, 112, 112, 99.7, 99.7, 124.54, 124.54, 112, 112, 99.7, 99.7 };
    qreal a_di_y[12] = { 229.8, 245.3, 229.8, 245.3, 229.8, 245.3, 323.2, 338.7, 323.2, 338.7, 323.2, 338.7 };
    qreal b_di_x[12] = { 124.54, 124.54, 112, 112, 99.7, 99.7, 124.54, 124.54, 112, 112, 99.7, 99.7 };
    qreal b_di_y[12] = { 198.8, 214.3, 198.8, 214.3, 198.8, 214.3, 292.2, 307.7, 292.2, 307.7, 292.2, 307.7 };
    for (int i = 0; i < 12; i++) {
        QGraphicsRectItem* aled = new QGraphicsRectItem(a_di_x[i], a_di_y[i], LED_WIDTH, LED_HEIGHT, this);
        aled->setPen(pen);
        a_ai_led_list.append(aled);
        QGraphicsRectItem* bled = new QGraphicsRectItem(b_di_x[i], b_di_y[i], LED_WIDTH, LED_HEIGHT, this);
        bled->setPen(pen);
        b_ai_led_list.append(bled);
        set_led_color(aled, LED_COLOR_DARKGREEN);
        set_led_color(bled, LED_COLOR_DARKGREEN);
    }
    qreal a_relay_x[2] = { 187.09, 187.09 };
    qreal a_relay_y[2] = { 229.8, 245.3 };
    qreal b_relay_x[2] = { 187.09, 187.09 };
    qreal b_relay_y[2] = { 198.8, 214.3 };
    for (int i = 0; i < 2; i++) {
        QGraphicsRectItem* aled = new QGraphicsRectItem(a_relay_x[i], a_relay_y[i], LED_WIDTH, LED_HEIGHT, this);
        aled->setPen(pen);
        a_relay_led_list.append(aled);
        QGraphicsRectItem* bled = new QGraphicsRectItem(b_relay_x[i], b_relay_y[i], LED_WIDTH, LED_HEIGHT, this);
        bled->setPen(pen);
        b_relay_led_list.append(bled);
        set_led_color(aled, LED_COLOR_DARKGREEN);
        set_led_color(bled, LED_COLOR_DARKGREEN);
    }
    qreal mos_x[4] = { 162.07, 162.07, 162.07, 162.07 };
    qreal mos_y[4] = { 229.8, 245.3, 214.3, 198.8 };
    for (int i = 0; i < 4; i++) {
        QGraphicsRectItem* led = new QGraphicsRectItem(mos_x[i], mos_y[i], LED_WIDTH, LED_HEIGHT, this);
        led->setPen(pen);
        mos_led_list.append(led);
        set_led_color(led, LED_COLOR_DARKGREEN);
    }

    qreal a_error_x = 95.19;
    qreal a_error_y = 359.8;
    qreal b_error_x = 241.56;
    qreal b_error_y = 450.8;
    a_error_led     = new QGraphicsRectItem(a_error_x, a_error_y, LED_WIDTH, LED_HEIGHT, this);
    a_error_led->setPen(pen);
    b_error_led = new QGraphicsRectItem(b_error_x, b_error_y, LED_WIDTH, LED_HEIGHT, this);
    b_error_led->setPen(pen);
    set_led_color(a_error_led, LED_COLOR_DARKRED);
    set_led_color(b_error_led, LED_COLOR_DARKRED);
}

void config_photo::set_led_color(QGraphicsRectItem* item, const QColor& color)
{
    QBrush brush(color);
    item->setBrush(brush);
}

void config_photo::display_available_source(element_data_t config_data)
{
    foreach (config_param* param, all_config_list) {
        param->display_detect(config_data);
    }
}

void config_photo::hide_available_source()
{
    foreach (config_param* param, all_config_list) {
        param->display_hide();
    }
}

void config_photo::drap_available_source(element_data_t config_data, QPointF pos)
{
    foreach (config_param* param, all_config_list) {
        if (param->bounding_range_detect(pos)) {
            param->drap_data_receive(config_data);
        }
    }
}

void config_photo::config_param_creat_update(module_param_t* param, config_block_data_t* data)
{
    switch (data->config_param_data.model_iotype) {
    case MODEL_TYPE_INPUT:
        switch (data->config_param_data.model_type) {
        case MODEL_INPUT_AI:
            switch (data->config_param_data.model_id) {
            case MODEL_ID_AI1:
                param->work_state.work_state_bit.ai1_work_state_bit = AI_AF_AI;
                param->ai_slv.ai_slv_bit.ai1_slv_bit                = data->safe_level;
                a_di_config_list[MODEL_ID_DI9]->setEnabled(false);
                b_di_config_list[MODEL_ID_DI9]->setEnabled(false);
                break;
            case MODEL_ID_AI2:
                param->work_state.work_state_bit.ai2_work_state_bit = AI_AF_AI;
                param->ai_slv.ai_slv_bit.ai2_slv_bit                = data->safe_level;
                a_di_config_list[MODEL_ID_DI10]->setEnabled(false);
                b_di_config_list[MODEL_ID_DI10]->setEnabled(false);
                break;
            }
            break;
        case MODEL_INPUT_PI:
            switch (data->config_param_data.model_id) {
            case MODEL_ID_PI1:
                param->work_state.work_state_bit.pi1_work_state_bit = PI_AF_PI;
                a_di_config_list[MODEL_ID_DI11]->setEnabled(false);
                b_di_config_list[MODEL_ID_DI11]->setEnabled(false);
                qep_config_list[MODEL_ID_PIQEP1]->setEnabled(false);
                qep_config_list[MODEL_ID_PIQEP2]->setEnabled(false);
                break;
            case MODEL_ID_PI2:
                param->work_state.work_state_bit.pi2_work_state_bit = PI_AF_PI;
                a_di_config_list[MODEL_ID_DI12]->setEnabled(false);
                b_di_config_list[MODEL_ID_DI12]->setEnabled(false);
                qep_config_list[MODEL_ID_PIQEP1]->setEnabled(false);
                qep_config_list[MODEL_ID_PIQEP2]->setEnabled(false);
                break;
            }
            break;
        case MODEL_INPUT_DI:
            switch (data->config_param_data.model_id) {
            case MODEL_ID_DI1:
            case MODEL_ID_DI2:
            case MODEL_ID_DI3:
            case MODEL_ID_DI4:
            case MODEL_ID_DI5:
            case MODEL_ID_DI6:
            case MODEL_ID_DI7:
            case MODEL_ID_DI8:
                if (data->safe_level == SAFE_LEVEL_CAT2) {
                    param->di_slv.di_slv_bytes &= ~(0x01 << data->config_param_data.model_id);
                } else {
                    param->di_slv.di_slv_bytes |= (0x01 << data->config_param_data.model_id);
                }
                break;
            case MODEL_ID_DI9:
                param->work_state.work_state_bit.ai1_work_state_bit = AI_AF_DI;
                a_ai_config_list[MODEL_ID_AI1]->setEnabled(false);
                b_ai_config_list[MODEL_ID_AI1]->setEnabled(false);
                break;
            case MODEL_ID_DI10:
                param->work_state.work_state_bit.ai2_work_state_bit = AI_AF_DI;
                a_ai_config_list[MODEL_ID_AI2]->setEnabled(false);
                b_ai_config_list[MODEL_ID_AI2]->setEnabled(false);
                break;
            case MODEL_ID_DI11:
                param->work_state.work_state_bit.pi1_work_state_bit = PI_AF_DI;
                a_pi_config_list[MODEL_ID_PI1]->setEnabled(false);
                b_pi_config_list[MODEL_ID_PI1]->setEnabled(false);
                qep_config_list[MODEL_ID_PIQEP1]->setEnabled(false);
                qep_config_list[MODEL_ID_PIQEP2]->setEnabled(false);
                break;
            case MODEL_ID_DI12:
                param->work_state.work_state_bit.pi2_work_state_bit = PI_AF_DI;
                a_pi_config_list[MODEL_ID_PI2]->setEnabled(false);
                b_pi_config_list[MODEL_ID_PI2]->setEnabled(false);
                qep_config_list[MODEL_ID_PIQEP1]->setEnabled(false);
                qep_config_list[MODEL_ID_PIQEP2]->setEnabled(false);
                break;
            }
            break;
        case MODEL_INPUT_QEP:
            switch (data->config_param_data.model_id) {
            case MODEL_ID_PIQEP1:
            case MODEL_ID_PIQEP2:
                param->work_state.work_state_bit.pi1_work_state_bit = PI_AF_QEP;
                param->work_state.work_state_bit.pi2_work_state_bit = PI_AF_QEP;
                a_di_config_list[MODEL_ID_DI11]->setEnabled(false);
                b_di_config_list[MODEL_ID_DI11]->setEnabled(false);
                a_di_config_list[MODEL_ID_DI12]->setEnabled(false);
                b_di_config_list[MODEL_ID_DI12]->setEnabled(false);
                a_pi_config_list[MODEL_ID_PI1]->setEnabled(false);
                b_pi_config_list[MODEL_ID_PI1]->setEnabled(false);
                a_pi_config_list[MODEL_ID_PI2]->setEnabled(false);
                b_pi_config_list[MODEL_ID_PI2]->setEnabled(false);
                break;
            }
            break;
        default:
            break;
        }
        break;
    case MODEL_TYPE_OUTPUT:
        switch (data->config_param_data.model_id) {
        case MODEL_ID_RELAY1:
        case MODEL_ID_RELAY2:
            if (data->safe_level == SAFE_LEVEL_CAT2) {
                param->relay_slv.relay_slv_byte &= ~(0x01 << data->config_param_data.model_id);
            } else {
                param->relay_slv.relay_slv_byte |= (0x01 << data->config_param_data.model_id);
            }
            break;
        default:
            break;
        }
        break;
    }
}

void config_photo::config_param_delete_update(module_param_t* param, config_block_data_t* data)
{
    Q_UNUSED(param);
    switch (data->config_param_data.model_iotype) {
    case MODEL_TYPE_INPUT:
        switch (data->config_param_data.model_type) {
        case MODEL_INPUT_AI:
            switch (data->config_param_data.model_id) {
            case MODEL_ID_AI1:
                a_di_config_list[MODEL_ID_DI9]->setEnabled(true);
                b_di_config_list[MODEL_ID_DI9]->setEnabled(true);
                break;
            case MODEL_ID_AI2:
                a_di_config_list[MODEL_ID_DI10]->setEnabled(true);
                b_di_config_list[MODEL_ID_DI10]->setEnabled(true);
                break;
            }
            break;
        case MODEL_INPUT_PI:
            switch (data->config_param_data.model_id) {
            case MODEL_ID_PI1:
                a_di_config_list[MODEL_ID_DI11]->setEnabled(true);
                b_di_config_list[MODEL_ID_DI11]->setEnabled(true);
                qep_config_list[MODEL_ID_PIQEP1]->setEnabled(true);
                qep_config_list[MODEL_ID_PIQEP2]->setEnabled(true);
                break;
            case MODEL_ID_PI2:
                a_di_config_list[MODEL_ID_DI12]->setEnabled(true);
                b_di_config_list[MODEL_ID_DI12]->setEnabled(true);
                qep_config_list[MODEL_ID_PIQEP1]->setEnabled(true);
                qep_config_list[MODEL_ID_PIQEP2]->setEnabled(true);
                break;
            }
            break;
        case MODEL_INPUT_DI:
            switch (data->config_param_data.model_id) {
            case MODEL_ID_DI9:
                if (data->safe_level == SAFE_LEVEL_CAT3) {
                    a_ai_config_list[MODEL_ID_AI1]->setEnabled(true);
                    b_ai_config_list[MODEL_ID_AI1]->setEnabled(true);
                    param->work_state.work_state_bit.ai1_work_state_bit = AI_AF_AI;
                } else {
                    if (!data->config_param_data.cat3_model->get_block_data().is_used) {
                        a_ai_config_list[MODEL_ID_AI1]->setEnabled(true);
                        b_ai_config_list[MODEL_ID_AI1]->setEnabled(true);
                        param->work_state.work_state_bit.ai1_work_state_bit = AI_AF_AI;
                    }
                }

                break;
            case MODEL_ID_DI10:
                if (data->safe_level == SAFE_LEVEL_CAT3) {
                    a_ai_config_list[MODEL_ID_AI2]->setEnabled(true);
                    b_ai_config_list[MODEL_ID_AI2]->setEnabled(true);
                    param->work_state.work_state_bit.ai2_work_state_bit = AI_AF_AI;
                } else {
                    if (!data->config_param_data.cat3_model->get_block_data().is_used) {
                        a_ai_config_list[MODEL_ID_AI2]->setEnabled(true);
                        b_ai_config_list[MODEL_ID_AI2]->setEnabled(true);
                        param->work_state.work_state_bit.ai2_work_state_bit = AI_AF_AI;
                    }
                }
                break;
            case MODEL_ID_DI11:
                if (data->safe_level == SAFE_LEVEL_CAT3) {
                    a_pi_config_list[MODEL_ID_PI1]->setEnabled(true);
                    b_pi_config_list[MODEL_ID_PI1]->setEnabled(true);
                    qep_config_list[MODEL_ID_PIQEP1]->setEnabled(true);
                    qep_config_list[MODEL_ID_PIQEP2]->setEnabled(true);
                    param->work_state.work_state_bit.pi1_work_state_bit = PI_AF_PI;
                } else {
                    if (!data->config_param_data.cat3_model->get_block_data().is_used) {
                        a_pi_config_list[MODEL_ID_PI1]->setEnabled(true);
                        b_pi_config_list[MODEL_ID_PI1]->setEnabled(true);
                        qep_config_list[MODEL_ID_PIQEP1]->setEnabled(true);
                        qep_config_list[MODEL_ID_PIQEP2]->setEnabled(true);
                        param->work_state.work_state_bit.pi1_work_state_bit = PI_AF_PI;
                    }
                }
                break;
            case MODEL_ID_DI12:
                if (data->safe_level == SAFE_LEVEL_CAT3) {
                    a_pi_config_list[MODEL_ID_PI2]->setEnabled(true);
                    b_pi_config_list[MODEL_ID_PI2]->setEnabled(true);
                    qep_config_list[MODEL_ID_PIQEP1]->setEnabled(true);
                    qep_config_list[MODEL_ID_PIQEP2]->setEnabled(true);
                    param->work_state.work_state_bit.pi2_work_state_bit = PI_AF_PI;
                } else {
                    if (!data->config_param_data.cat3_model->get_block_data().is_used) {
                        a_pi_config_list[MODEL_ID_PI2]->setEnabled(true);
                        b_pi_config_list[MODEL_ID_PI2]->setEnabled(true);
                        qep_config_list[MODEL_ID_PIQEP1]->setEnabled(true);
                        qep_config_list[MODEL_ID_PIQEP2]->setEnabled(true);
                        param->work_state.work_state_bit.pi2_work_state_bit = PI_AF_PI;
                    }
                }

                break;
            }
            break;
        case MODEL_INPUT_QEP:
            switch (data->config_param_data.model_id) {
            case MODEL_ID_PIQEP1:
            case MODEL_ID_PIQEP2:
                if (data->safe_level == SAFE_LEVEL_CAT3) {
                    a_di_config_list[MODEL_ID_DI11]->setEnabled(true);
                    b_di_config_list[MODEL_ID_DI11]->setEnabled(true);
                    a_di_config_list[MODEL_ID_DI12]->setEnabled(true);
                    b_di_config_list[MODEL_ID_DI12]->setEnabled(true);
                    a_pi_config_list[MODEL_ID_PI1]->setEnabled(true);
                    b_pi_config_list[MODEL_ID_PI1]->setEnabled(true);
                    a_pi_config_list[MODEL_ID_PI2]->setEnabled(true);
                    b_pi_config_list[MODEL_ID_PI2]->setEnabled(true);
                    param->work_state.work_state_bit.pi1_work_state_bit = PI_AF_PI;
                    param->work_state.work_state_bit.pi2_work_state_bit = PI_AF_PI;
                } else {
                    if (!data->config_param_data.cat3_model->get_block_data().is_used) {
                        a_di_config_list[MODEL_ID_DI11]->setEnabled(true);
                        b_di_config_list[MODEL_ID_DI11]->setEnabled(true);
                        a_di_config_list[MODEL_ID_DI12]->setEnabled(true);
                        b_di_config_list[MODEL_ID_DI12]->setEnabled(true);
                        a_pi_config_list[MODEL_ID_PI1]->setEnabled(true);
                        b_pi_config_list[MODEL_ID_PI1]->setEnabled(true);
                        a_pi_config_list[MODEL_ID_PI2]->setEnabled(true);
                        b_pi_config_list[MODEL_ID_PI2]->setEnabled(true);
                        param->work_state.work_state_bit.pi1_work_state_bit = PI_AF_PI;
                        param->work_state.work_state_bit.pi2_work_state_bit = PI_AF_PI;
                    }
                }

                break;
            }
            break;
        }
        break;
    }
}
void config_photo::module_param_update(module_param_t* param)
{
    bool reset = true;
    /* AI */
    for (int i = 0; i < a_ai_config_list.size(); i++) {
        if ((a_ai_config_list[i]->get_block_data().is_used
             && a_ai_config_list[i]->get_block_data().safe_level == SAFE_LEVEL_CAT3)
            || (b_ai_config_list[i]->get_block_data().is_used
                && b_ai_config_list[i]->get_block_data().safe_level == SAFE_LEVEL_CAT3)) {
            reset = false;
        }
    }
    if (reset) {
        param->sai_sample_interval = 10;
        param->sai_allow_dif[0]    = 1;
        param->sai_allow_dif[1]    = 3;
    }
    /* PI */
    reset = true;
    for (int i = 0; i < a_pi_config_list.size(); i++) {
        if ((a_pi_config_list[i]->get_block_data().is_used
             && a_pi_config_list[i]->get_block_data().safe_level == SAFE_LEVEL_CAT3)
            || (b_pi_config_list[i]->get_block_data().is_used
                && b_pi_config_list[i]->get_block_data().safe_level == SAFE_LEVEL_CAT3)) {
            reset = false;
        }
    }
    if (reset) {
        param->spi_sample_interval = 10;
        param->spi_allow_dif[0]    = 1;
        param->spi_allow_dif[1]    = 3;
    }
    /* QEP */
    reset = true;
    for (int i = 0; i < qep_config_list.size() / 2; i++) {
        if ((qep_config_list[i]->get_block_data().is_used
             && qep_config_list[i]->get_block_data().safe_level == SAFE_LEVEL_CAT3)
            || (qep_config_list[i]->get_block_data().is_used
                && qep_config_list[i]->get_block_data().safe_level == SAFE_LEVEL_CAT3)) {}
        if ((qep_config_list[i]->get_block_data().is_used) || (qep_config_list[i]->get_block_data().is_used)) {
            reset = false;
        }
    }
    if (reset) {
        param->sqep_sample_interval = 10;
        param->sqep_allow_dif[0]    = 1;
        param->sqep_allow_dif[1]    = 3;
    }
    /* PIQEP */
    reset = true;
    for (int i = 2; i < qep_config_list.size(); i++) {
        if ((qep_config_list[i]->get_block_data().is_used
             && qep_config_list[i]->get_block_data().safe_level == SAFE_LEVEL_CAT3)
            || (qep_config_list[i]->get_block_data().is_used
                && qep_config_list[i]->get_block_data().safe_level == SAFE_LEVEL_CAT3)) {}
        if ((qep_config_list[i]->get_block_data().is_used) || (qep_config_list[i]->get_block_data().is_used)) {
            reset = false;
        }
    }
    if (reset) {
        param->pi_sqep_sample_interval = 10;
        param->pi_qep_allow_dif[0]     = 1;
        param->pi_qep_allow_dif[1]     = 3;
    }
}

void config_photo::set_base_data_enable(bool state)
{
    base_data_enable = state;
    if (!base_data_enable) {
        for (int i = 0; i < all_config_list.size(); i++) {
            all_config_list[i]->set_module_value(0);
        }
    }
}

void config_photo::a_set_base_data(module_state_t data)
{
    if (!base_data_enable) {
        return;
    }
    /* ai */
    a_ai_config_list[0]->set_module_value(data.analog_value1);
    a_ai_config_list[1]->set_module_value(data.analog_value2);
    /* di */
    for (int i = 0; i < a_di_config_list.size(); i++) {
        a_di_config_list[i]->set_module_value((data.di_state.di_s >> i) & 0x01);
    }
    /* relay */
    a_relay_config_list[0]->set_module_value(data.relay_state.relay_bit.read_bit1);
    a_relay_config_list[1]->set_module_value(data.relay_state.relay_bit.read_bit2);
    /* mos */
    mos_config_list[0]->set_module_value(data.mos_state.mos_bit.mos_a_read_bit1);
    mos_config_list[1]->set_module_value(data.mos_state.mos_bit.mos_a_read_bit2);
    /* qep */
    if (data.dir_state.dir_bit.qep_dir_ma_read_bit) {
        qep_config_list[0]->set_module_value(data.qep_ma_cnt);
    } else {
        qep_config_list[0]->set_module_value(-data.qep_ma_cnt);
    }
    if (data.dir_state.dir_bit.qep_dir_mb_read_bit) {
        qep_config_list[1]->set_module_value(data.qep_mb_cnt);
    } else {
        qep_config_list[1]->set_module_value(-data.qep_mb_cnt);
    }
    if (data.dir_state.dir_bit.pi_dir_ma_read_bit) {
        qep_config_list[2]->set_module_value(data.encoder_pi_ma_cnt);
    } else {
        qep_config_list[2]->set_module_value(-data.encoder_pi_ma_cnt);
    }
    if (data.dir_state.dir_bit.pi_dir_mb_read_bit) {
        qep_config_list[3]->set_module_value(data.encoder_pi_mb_cnt);
    } else {
        qep_config_list[3]->set_module_value(-data.encoder_pi_mb_cnt);
    }
    /* pi */
    a_pi_config_list[0]->set_module_value(data.pi1_cnt);
    a_pi_config_list[1]->set_module_value(data.pi2_cnt);
}

void config_photo::b_set_base_data(module_state_t data)
{
    if (!base_data_enable) {
        return;
    }
    /* ai */
    b_ai_config_list[0]->set_module_value(data.analog_value1);
    b_ai_config_list[1]->set_module_value(data.analog_value2);
    /* di */
    for (int i = 0; i < a_di_config_list.size(); i++) {
        b_di_config_list[i]->set_module_value((data.di_state.di_s >> i) & 0x01);
    }
    /* relay */
    b_relay_config_list[0]->set_module_value(data.relay_state.relay_bit.read_bit1);
    b_relay_config_list[1]->set_module_value(data.relay_state.relay_bit.read_bit2);
    /* mos */
    mos_config_list[2]->set_module_value(data.mos_state.mos_bit.mos_b_read_bit1);
    mos_config_list[3]->set_module_value(data.mos_state.mos_bit.mos_b_read_bit2);
    /* qep */
    //    qep_config_list[0]->set_module_value(data.qep_ma_cnt);
    //    qep_config_list[1]->set_module_value(data.qep_mb_cnt);
    //    qep_config_list[2]->set_module_value(data.encoder_pi_ma_cnt);
    //    qep_config_list[3]->set_module_value(data.encoder_pi_mb_cnt);
    /* pi */
    b_pi_config_list[0]->set_module_value(data.pi1_cnt);
    b_pi_config_list[1]->set_module_value(data.pi2_cnt);
}

void config_photo::set_fault_info(module_fault_t a_fault_info, module_fault_t b_fault_info)
{
    config_device_fault_rect->set_fault_info(a_fault_info, b_fault_info);
}

void config_photo::config_photo_reset()
{
    for (int i = 0; i < all_config_list.size(); i++) {
        all_config_list[i]->data_reset();
    }
}

QJsonObject config_photo::config_photo_project_info()
{
    QJsonObject rootObject;
    for (int i = 0; i < all_config_list.size(); i++) {
        rootObject["param" + QString::number(i)] = all_config_list[i]->config_param_project_info();
    }
    return rootObject;
}

bool config_photo::config_photo_project_parse(QJsonObject rootObject)
{
    for (int i = 0; i < all_config_list.size(); i++) {
        all_config_list[i]->config_param_project_parse(rootObject["param" + QString::number(i)].toObject());
    }
    return false;
}

/* sys event */
void config_photo::hoverMoveEvent(QGraphicsSceneHoverEvent* event)
{
    Q_UNUSED(event);
}

void config_photo::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    Q_UNUSED(event);
}
