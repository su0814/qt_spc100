#ifndef GRAPHICAL_CONFIG_PARAM_H
#define GRAPHICAL_CONFIG_PARAM_H
#include "config/param_config.h"
#include "stdint.h"
#include <QDebug>
#include <QMetaType>
#include <QPainter>
#include <QProxyStyle>
#include <QString>
#include <QStyledItemDelegate>
#include <QTabWidget>
#include <QWidget>
class config_param;
typedef enum {
    CONFIG_TOOL_SYS = 0,
    CONFIG_TOOL_CAN,
    CONFIG_TOOL_AI,
    CONFIG_TOOL_DI,
    CONFIG_TOOL_RELAY,
    CONFIG_TOOL_MOS,
    CONFIG_TOOL_ENCODE,
    CONFIG_TOOL_PI,
} config_tool_e;

enum {
    SPEED_SELECT_NONE = 0,
    SPEED_SELECT_QEP1,
    SPEED_SELECT_QEP2,
    SPEED_SELECT_AVERAGE,
};

typedef enum {
    MODEL_INPUT_AI = 0,
    MODEL_INPUT_DI,
    MODEL_INPUT_PI,
    MODEL_INPUT_QEP,
    MODEL_INPUT_CONST,
    MODEL_INPUT_REPEATER,
    MODEL_OUTPUT_RELAY_MOS = 0,
    MODEL_OUTPUT_REPEATER,
    MODEL_LOGIC_BASE = 0,
    MODEL_LOGIC_APPLICATION,
    MODEL_LOGIC_DELAY_COUNTER,
    MODEL_LOGIC_SPEED,
} model_type_e;

typedef enum {
    MODEL_TYPE_INPUT = 0,
    MODEL_TYPE_OUTPUT,
    MODEL_TYPE_LOGIC,
} model_iotype_e;

enum {
    PI_AF_PI = 0,
    PI_AF_QEP,
    PI_AF_DI,
    AI_AF_AI = 0,
    AI_AF_DI,
};

enum {
    KEY_TYPE_MENU = 1,
    KEY_TYPE_ELEMENT,
};
/* SYS SOURCE */
typedef enum {
    SOURCE_MCU_A = 0,
    SOURCE_MCU_B,
    SOURCE_MCU_SAFE,
} source_mcu_e;

enum {
    NORMAL_STATE_NONE = 0,
    NORMAL_STATE_NC,
    NORMAL_STATE_NO,
};

typedef enum {
    MODEL_ID_AI1 = 0,
    MODEL_ID_AI2,
    MODEL_ID_DI1 = 0,
    MODEL_ID_DI2,
    MODEL_ID_DI3,
    MODEL_ID_DI4,
    MODEL_ID_DI5,
    MODEL_ID_DI6,
    MODEL_ID_DI7,
    MODEL_ID_DI8,
    MODEL_ID_DI9,
    MODEL_ID_DI10,
    MODEL_ID_DI11,
    MODEL_ID_DI12,
    MODEL_ID_PI1 = 0,
    MODEL_ID_PI2,
    MODEL_ID_QEP1 = 0,
    MODEL_ID_QEP2,
    MODEL_ID_PIQEP1,
    MODEL_ID_PIQEP2,
    MODEL_ID_TRUE = 0,
    MODEL_ID_FALSE,
    MODEL_ID_RELAY1 = 0,
    MODEL_ID_RELAY2,
    MODEL_ID_MOS1,
    MODEL_ID_MOS2,
    MODEL_ID_MOS3,
    MODEL_ID_MOS4,
    MODEL_ID_LOGIC_BASE_AND = 0,
    MODEL_ID_LOGIC_BASE_OR,
    MODEL_ID_LOGIC_BASE_NOT,
    MODEL_ID_LOGIC_BASE_XNOR,
    MODEL_ID_LOGIC_BASE_XOR,
    MODEL_ID_LOGIC_BASE_RS,
    MODEL_ID_LOGIC_BASE_ENCODER,
    MODEL_ID_LOGIC_BASE_DECODER,
    MODEL_ID_LOGIC_APPLICATION_RESET = 0,
    MODEL_ID_LOGIC_APPLICATION_EDMONITOR,
    MODEL_ID_LOGIC_APPLICATION_FREQ_DETECT,
    MODEL_ID_LOGIC_APPLICATION_EDGE_DETECT,
    MODEL_ID_LOGIC_APPLICATION_SLOPE_DOWN,
    MODEL_ID_LOGIC_DELAY_ON = 0,
    MODEL_ID_LOGIC_DELAY_OFF,
    MODEL_ID_LOGIC_DELAY_ADJUST_ON,
    MODEL_ID_LOGIC_DELAY_ADJUST_OFF,
    MODEL_ID_LOGIC_COUNTER_EVENT,
    MODEL_ID_LOGIC_COUNTER_LOGGING,
    MODEL_ID_LOGIC_SPEED_CROSS_CHECK = 0,
    MODEL_ID_LOGIC_SPEED_DECELERATE_MONITOR,
    MODEL_ID_LOGIC_SPEED_MOTIONLESS_MONITOR,
    MODEL_ID_LOGIC_SPEED_VALUE_COMPAIRSONS,
} model_id_e;

typedef enum {
    SAFE_LEVEL_CAT2 = 0,
    SAFE_LEVEL_CAT3,
} safe_level_e;

typedef enum {
    LABEL_DIR_LEFT = 0,
    LABEL_DIR_RIGHT,
} label_dir_e;

struct element_data_t {
    uint8_t type;
    int     model_type;
    int     model_iotype;
    uint8_t cat;
    QString name;
    QString pixmap;
    int     normal_state;
};

typedef struct {
    int           model_type   = MODEL_INPUT_AI;
    int           model_iotype = MODEL_TYPE_INPUT;
    int           model_id     = MODEL_ID_AI1;
    int           normal_state = NORMAL_STATE_NONE;
    source_mcu_e  source_mcu   = SOURCE_MCU_A;
    label_dir_e   label_dir    = LABEL_DIR_LEFT;
    config_param* cat3_model   = nullptr;
} config_param_data_t;

typedef struct {
    config_param_data_t config_param_data;
    safe_level_e        safe_level = SAFE_LEVEL_CAT3;
    bool                is_used    = false;
    bool                is_config;
    QString             name = "";    //用户命名
    QString             suffix;       //尾缀.A .B等
    QString             source_name;  // DI1-12等
    QString             type_name;    //单双通道某某
    QString             pixmap;
} config_block_data_t;

Q_DECLARE_METATYPE(element_data_t)
Q_DECLARE_METATYPE(config_block_data_t)

#pragma pack(1)
typedef struct {
    /*di safe level*/
    union {
        uint16_t di_slv_bytes;
        struct {
            uint16_t di1_slv_bit : 1;
            uint16_t di2_slv_bit : 1;
            uint16_t di3_slv_bit : 1;
            uint16_t di4_slv_bit : 1;
            uint16_t di5_slv_bit : 1;
            uint16_t di6_slv_bit : 1;
            uint16_t di7_slv_bit : 1;
            uint16_t di8_slv_bit : 1;
            uint16_t di_slv_reserve_bit : 8;
        } di_slv_bit;
    } di_slv;
    /*relay safe level*/
    union {
        uint8_t relay_slv_byte;
        struct {
            uint8_t relay1_slv_bit : 1;
            uint8_t relay2_slv_bit : 1;
            uint8_t relay_slv_reserve_bit : 6;
        } relay_slv_bit;
    } relay_slv;
    /*ai safe level*/
    union {
        uint8_t ai_slv_byte;
        struct {
            uint8_t ai1_slv_bit : 1;
            uint8_t ai2_slv_bit : 1;
            uint8_t ai_slv_reserve_bit : 6;
        } ai_slv_bit;
    } ai_slv;
    /*work state*/
    union {
        uint8_t work_state_byte;
        struct {
            uint8_t pi1_work_state_bit : 2;
            uint8_t pi2_work_state_bit : 2;
            uint8_t ai1_work_state_bit : 1;
            uint8_t ai2_work_state_bit : 1;
            uint8_t work_state_reserve_bit : 2;
        } work_state_bit;
    } work_state;
    uint8_t  sai_sample_interval;
    uint8_t  sai_allow_dif[2];  //[0]-full scale Proportion of deviation [1]self data Proportion of deviation
    uint8_t  spi_sample_interval;
    uint8_t  spi_allow_dif[2];
    uint8_t  pi_sqep_sample_interval;
    uint8_t  pi_qep_allow_dif[2];
    uint8_t  sqep_sample_interval;
    uint8_t  sqep_allow_dif[2];
    uint8_t  can_master_nodeID;
    uint8_t  can_slave_nodeID_A;
    uint8_t  can_slave_nodeID_B;
    uint16_t can_hb_consumer_gap;
    uint16_t can_hb_producer_gap;
    uint16_t check_factor;
    uint32_t can_baudrate;
    uint16_t can_pdo_time_gap;
    uint16_t qep1_pulse_num;
    uint16_t qep1_distance;
    uint16_t qep2_pulse_num;
    uint16_t qep2_distance;
    uint16_t piqep1_pulse_num;
    uint16_t piqep1_distance;
    uint16_t piqep2_pulse_num;
    uint16_t piqep2_distance;
    uint8_t  md5[16];  // the md5 must Place on the tail
} module_param_t;
#pragma pack()

class CustomItemDelegate : public QStyledItemDelegate {
public:
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override
    {
        QStyledItemDelegate::paint(painter, option, index);
        QVariant data = index.data(Qt::UserRole);
        if (data.isValid()) {
            // 在项目的图标的左边绘制一个绿点
            QRect dotRect(option.rect.left() - 20, option.rect.center().y() - 5, 10, 10);  // 绿点的位置和大小
            painter->setBrush(QColor(230, 130, 238));
            painter->drawEllipse(dotRect);
        }
    }
};

#endif  // GRAPHICAL_CONFIG_PARAM_H
