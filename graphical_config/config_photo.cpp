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
    //    a_ai1 = new config_param(12, 137, 18, 31, CONFIG_TOOL_SYS, uiparent, this);
    //    a_ai2 = new config_param(12, 171, 18, 31, CONFIG_TOOL_SYS, uiparent, this);
    //    b_ai1 = new config_param(12, 205, 18, 31, CONFIG_TOOL_SYS, uiparent, this);
    //    b_ai2 = new config_param(12, 245, 18, 31, CONFIG_TOOL_SYS, uiparent, this);

    //    a_di1 = new config_param(12, 280, 18, 14, CONFIG_TOOL_SYS, uiparent, this);
    //    a_di2 = new config_param(12, 297, 18, 14, CONFIG_TOOL_SYS, uiparent, this);
    //    a_di3 = new config_param(12, 314, 18, 14, CONFIG_TOOL_SYS, uiparent, this);
    //    a_di4 = new config_param(12, 331, 18, 14, CONFIG_TOOL_SYS, uiparent, this);
    //    a_di5 = new config_param(12, 348, 18, 14, CONFIG_TOOL_SYS, uiparent, this);
    //    a_di6 = new config_param(12, 365, 18, 31, CONFIG_TOOL_SYS, uiparent, this);
    //    a_di7 = new config_param(12, 399, 18, 14, CONFIG_TOOL_SYS, uiparent, this);
    //    a_di8 = new config_param(12, 423, 18, 14, CONFIG_TOOL_SYS, uiparent, this);

    //    b_di1 = new config_param(12, 440, 18, 14, CONFIG_TOOL_SYS, uiparent, this);
    //    b_di2 = new config_param(12, 457, 18, 14, CONFIG_TOOL_SYS, uiparent, this);
    //    b_di3 = new config_param(12, 474, 18, 14, CONFIG_TOOL_SYS, uiparent, this);
    //    b_di4 = new config_param(12, 491, 18, 14, CONFIG_TOOL_SYS, uiparent, this);
    //    b_di5 = new config_param(12, 508, 18, 14, CONFIG_TOOL_SYS, uiparent, this);
    //    b_di6 = new config_param(12, 525, 18, 31, CONFIG_TOOL_SYS, uiparent, this);
    //    b_di7 = new config_param(12, 559, 18, 14, CONFIG_TOOL_SYS, uiparent, this);
    //    b_di8 = new config_param(12, 576, 18, 14, CONFIG_TOOL_SYS, uiparent, this);

    //    a_relay1 = new config_param(310, 10, 18, 31, CONFIG_TOOL_SYS, uiparent, this);
    //    a_relay2 = new config_param(310, 44, 18, 38, CONFIG_TOOL_SYS, uiparent, this);
    //    b_relay1 = new config_param(310, 85, 18, 31, CONFIG_TOOL_SYS, uiparent, this);
    //    b_relay2 = new config_param(310, 119, 18, 31, CONFIG_TOOL_SYS, uiparent, this);

    //    mos1 = new config_param(310, 153, 18, 31, CONFIG_TOOL_SYS, uiparent, this);
    //    mos2 = new config_param(310, 187, 18, 31, CONFIG_TOOL_SYS, uiparent, this);
    //    mos3 = new config_param(310, 245, 18, 31, CONFIG_TOOL_SYS, uiparent, this);
    //    mos4 = new config_param(310, 280, 18, 31, CONFIG_TOOL_SYS, uiparent, this);

    //    qep1 = new config_param(310, 314, 18, 65, CONFIG_TOOL_SYS, uiparent, this);
    //    qep2 = new config_param(310, 382, 18, 71, CONFIG_TOOL_SYS, uiparent, this);

    //    a_pi1 = new config_param(310, 457, 18, 31, CONFIG_TOOL_SYS, uiparent, this);
    //    a_pi2 = new config_param(310, 491, 18, 31, CONFIG_TOOL_SYS, uiparent, this);
    //    b_pi1 = new config_param(310, 525, 18, 31, CONFIG_TOOL_SYS, uiparent, this);
    //    b_pi2 = new config_param(310, 559, 18, 31, CONFIG_TOOL_SYS, uiparent, this);

    //    pi_qep1 = new config_param(310, 457, 18, 65, CONFIG_TOOL_SYS, uiparent, this);
    //    pi_qep2 = new config_param(310, 525, 18, 65, CONFIG_TOOL_SYS, uiparent, this);

    sys_config    = new config_param(39, 27, 23, 31, CONFIG_TOOL_SYS, uiparent, this);
    can_config    = new config_param(39, 69, 23, 59, CONFIG_TOOL_CAN, uiparent, this);
    ai_config     = new config_param(39, 135, 23, 141, CONFIG_TOOL_AI, uiparent, this);
    di_config     = new config_param(39, 280, 23, 307, CONFIG_TOOL_DI, uiparent, this);
    relay_config  = new config_param(278, 12, 25, 139, CONFIG_TOOL_RELAY, uiparent, this);
    mos_config    = new config_param(278, 156, 25, 152, CONFIG_TOOL_MOS, uiparent, this);
    encode_config = new config_param(278, 313, 25, 140, CONFIG_TOOL_ENCODE, uiparent, this);
    pi_config     = new config_param(278, 456, 25, 133, CONFIG_TOOL_PI, uiparent, this);
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
