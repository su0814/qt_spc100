#ifndef LOGIC_VIEW_CONFIG_H
#define LOGIC_VIEW_CONFIG_H
#include "QStyleOptionTab"
#include "config/data_config.h"
#include "graphical_config/graphical_config_param.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QList>
#include <QPainter>
#include <QProxyStyle>
#define LOGIC_BLOCK_MAX_NUM (16)
enum {
    ACTION_NONE   = 0,
    ACTION_SET    = 0x01,
    ACTION_DELETE = 0X02,
};
typedef enum {
    UPOVER_RESET_MODE = 0,
    DOWNOVER_RESET_MODE,
    UPOVER_VALUE,
    START_VALUE,
    RESET_ZERO_PULSE_TIME,
    RESET_START_PULSE_TIME,
    RESET_KEEP_TIME,
    EVENT_COUNTER_PARAM_NUM,
} event_counter_param_e;

typedef struct {
    QList<uint32_t>      parent_id;
    int                  uid;
    int                  order_id;
    QString              source_function = "";  //输入的源函数
    QString              logic_function  = "";  //输入的逻辑
    QString              function_name   = "";  //函数名称
    config_block_data_t* config_block_data;
} attribute_t;

typedef struct {
    uint32_t value = 0;
    uint8_t  dir   = 0;
} condition_right_set_t;

inline bool operator==(const condition_right_set_t& param1, const condition_right_set_t& param2)
{
    return ((param1.value == param2.value) && (param1.dir == param2.dir));
}

/* 复制信息 */
typedef struct {
    int                 type;
    int                 width;
    int                 height;
    QPointF             offset_pos;
    config_block_data_t block_data;
    QJsonObject         block_info;
} block_copy_data_t;

typedef struct {
    int                    input_block_id;
    int                    input_point_id;
    connect_point_iotype_e input_point_type;
    int                    output_block_id;
    int                    output_point_id;
    connect_point_iotype_e output_point_type;
} line_copy_data_t;

/* 块基本参数 */
typedef struct {
    int         input_reverse_data = 0;
    int         input_point_mask   = 0;
    int         output_point_mask  = 0;
    QStringList user_input_point_label;
    QStringList user_output_point_label;
} block_base_param_t;
inline bool operator==(const block_base_param_t& param1, const block_base_param_t& param2)
{
    return ((param1.input_point_mask == param2.input_point_mask)
            && (param1.output_point_mask == param2.output_point_mask)
            && (param1.user_input_point_label == param2.user_input_point_label)
            && (param1.user_output_point_label == param2.user_output_point_label)
            && (param1.input_reverse_data == param2.input_reverse_data));
}
/* 编码器块 */
typedef struct {
    bool nencode_fault_output = false;
} logic_base_encode_param_t;
inline bool operator==(const logic_base_encode_param_t& param1, const logic_base_encode_param_t& param2)
{
    return ((param1.nencode_fault_output == param2.nencode_fault_output));
}
/* 复位块 */
typedef struct {
    int min_reset_pulse_time = 100;
} apply_reset_param_t;
inline bool operator==(const apply_reset_param_t& param1, const apply_reset_param_t& param2)
{
    return ((param1.min_reset_pulse_time == param2.min_reset_pulse_time));
}
/* 外部设备监控块 */
typedef struct {
    int max_feedback_delay = 10;
} apply_edm_param_t;
inline bool operator==(const apply_edm_param_t& param1, const apply_edm_param_t& param2)
{
    return ((param1.max_feedback_delay == param2.max_feedback_delay));
}
/* 频率检测块 */
typedef struct {
    bool fault_output  = false;
    bool freq_enable   = false;
    int  freq_param[8] = { 450, 550, 250, 50, 450, 550, 250, 50 };
} apply_freq_monitor_param_t;
inline bool operator==(const apply_freq_monitor_param_t& param1, const apply_freq_monitor_param_t& param2)
{
    return ((param1.fault_output == param2.fault_output) && (param1.freq_enable == param2.freq_enable)
            && (memcmp(param1.freq_param, param2.freq_param, sizeof(param1.freq_param)) == 0));
}
/* 边沿检测块 */
typedef struct {
    int edge_detected_mode = 0;
} apply_edge_detect_param_t;
inline bool operator==(const apply_edge_detect_param_t& param1, const apply_edge_detect_param_t& param2)
{
    return ((param1.edge_detected_mode == param2.edge_detected_mode));
}
/* 断开/接通延时块 */
typedef struct {
    int on_off_delay_time = 0;
} delay_on_off_param_t;
inline bool operator==(const delay_on_off_param_t& param1, const delay_on_off_param_t& param2)
{
    return ((param1.on_off_delay_time == param2.on_off_delay_time));
}
/* 可调断开/接通延时块 */
typedef struct {
    int adjust_on_off_delay_time[4] = { 0 };
} delay_adjust_on_off_param_t;
inline bool operator==(const delay_adjust_on_off_param_t& param1, const delay_adjust_on_off_param_t& param2)
{
    return ((memcmp(param1.adjust_on_off_delay_time, param2.adjust_on_off_delay_time,
                    sizeof(param1.adjust_on_off_delay_time))
             == 0));
}
/* 事件计数块 */
typedef struct {
    int event_counter_param[EVENT_COUNTER_PARAM_NUM] = { 0, 0, 1000, 1000, 100, 100, 1 };
} counter_event_param_t;
inline bool operator==(const counter_event_param_t& param1, const counter_event_param_t& param2)
{
    return ((memcmp(param1.event_counter_param, param2.event_counter_param, sizeof(param1.event_counter_param)) == 0));
}
/* 日志记录块 */
typedef struct {
    QString log_text[8];
    int     log_edge[8] = { 1, 1, 1, 1, 1, 1, 1, 1 };
} counter_logging_param_t;
inline bool operator==(const counter_logging_param_t& param1, const counter_logging_param_t& param2)
{
    for (int i = 0; i < 8; i++) {
        if ((param1.log_text[i] != param2.log_text[i]) || (param1.log_edge[i] != param2.log_edge[i])) {
            return false;
        }
    }
    return true;
}
/* 速度互检块 */
typedef struct {
    int  encoder_output_mode           = 0;
    int  crosscheck_percentage[2]      = { 0 };
    int  error_keep_time               = 0;
    bool encoder_reliability_monitor   = false;
    int  reliability_monitor_max_time  = 1;
    int  reliability_monitor_min_speed = 1;
} speed_crosscheck_param_t;
inline bool operator==(const speed_crosscheck_param_t& param1, const speed_crosscheck_param_t& param2)
{
    return ((param1.encoder_output_mode == param2.encoder_output_mode)
            && (param1.crosscheck_percentage[0] == param2.crosscheck_percentage[0])
            && (param1.crosscheck_percentage[1] == param2.crosscheck_percentage[1])
            && (param1.error_keep_time == param2.error_keep_time)
            && (param1.encoder_reliability_monitor == param2.encoder_reliability_monitor)
            && (param1.reliability_monitor_max_time == param2.reliability_monitor_max_time)
            && (param1.reliability_monitor_min_speed == param2.reliability_monitor_min_speed));
}
/* 减速监控块 */
typedef struct {
    int ramp_num          = 1;
    int ramp_delay_time   = 0;
    int ramp_time[4]      = { 1000, 1000, 1000, 1000 };
    int ramp_speed[4]     = { 200, 200, 200, 200 };
    int ramp_max_speed[4] = { 500, 500, 500, 500 };
    int ramp_min_speed[4] = { 100, 100, 100, 100 };
} speed_decelerate_monitor_param_t;
inline bool operator==(const speed_decelerate_monitor_param_t& param1, const speed_decelerate_monitor_param_t& param2)
{
    return ((param1.ramp_num == param2.ramp_num) && (param1.ramp_delay_time == param2.ramp_delay_time)
            && (memcmp(param1.ramp_time, param2.ramp_time, sizeof(param1.ramp_time)) == 0)
            && (memcmp(param1.ramp_speed, param2.ramp_speed, sizeof(param1.ramp_speed)) == 0)
            && (memcmp(param1.ramp_max_speed, param2.ramp_max_speed, sizeof(param1.ramp_max_speed)) == 0)
            && (memcmp(param1.ramp_min_speed, param2.ramp_min_speed, sizeof(param1.ramp_min_speed)) == 0));
}
/* 静止检测块 */
typedef struct {
    int motionless_speed    = 1;
    int motionless_min_time = 0;
} speed_motionless_monitor_param_t;
inline bool operator==(const speed_motionless_monitor_param_t& param1, const speed_motionless_monitor_param_t& param2)
{
    return ((param1.motionless_speed == param2.motionless_speed)
            && (param1.motionless_min_time == param2.motionless_min_time));
}
/* 数值比较块 */
typedef struct {
    int calc_mode   = 0;
    int speed_value = 0;
    int min_time    = 0;
} speed_value_compairsons_param_t;
inline bool operator==(const speed_value_compairsons_param_t& param1, const speed_value_compairsons_param_t& param2)
{
    return ((param1.calc_mode == param2.calc_mode) && (param1.speed_value == param2.speed_value)
            && (param1.min_time == param2.min_time));
}
/* 自定义链接节点类型简称字符 */
extern const QStringList connect_point_data_type_label;
/* 自定义treewidget的item前面的点 */
class MyProxyStyle : public QProxyStyle {
public:
    using QProxyStyle::QProxyStyle;
    void drawControl(ControlElement element, const QStyleOption* option, QPainter* painter,
                     const QWidget* widget = nullptr) const override
    {
        if (element == CE_TabBarTabLabel) {
            // 在这里进行标签标题的绘制
            const QStyleOptionTab* tabOption = qstyleoption_cast<const QStyleOptionTab*>(option);
            if (tabOption) {
                QRect       allRect = tabOption->rect;
                QTextOption option;
                option.setAlignment(Qt::AlignCenter);
                painter->drawText(allRect, tabOption->text, option);
                return;
            }
        }
        QProxyStyle::drawControl(element, option, painter, widget);
    }
};
#endif  // LOGIC_VIEW_CONFIG_H
