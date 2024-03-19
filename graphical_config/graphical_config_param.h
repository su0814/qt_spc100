#ifndef GRAPHICAL_CONFIG_PARAM_H
#define GRAPHICAL_CONFIG_PARAM_H
#include "config/param_config.h"
#include "stdint.h"
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

enum {
    PI_AF_PI = 0,
    PI_AF_QEP,
    PI_AF_DI,
    AI_AF_AI = 0,
    AI_AF_DI,
};

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
    union {
        uint8_t safe_state_byte;
        struct {
            uint8_t output_safe_state_bit : 1;
            uint8_t reply1_safe_state_bit : 1;
            uint8_t reply2_safe_state_bit : 1;
            uint8_t smos1_safe_state_bit : 1;
            uint8_t smos2_safe_state_bit : 1;
            uint8_t smos3_safe_state_bit : 1;
            uint8_t smos4_safe_state_bit : 1;
            uint8_t safe_state_reserve_bit : 1;
        } safe_state_bit;
    } safe_state;
    uint8_t fault_code2_safe_state;
    uint16_t fault_code2_safe_state_delaytime;
    uint8_t sai_sample_interval;
    uint8_t sai_allow_dif[2]; //[0]-full scale Proportion of deviation [1]self data Proportion of deviation
    uint8_t spi_sample_interval;
    uint8_t spi_allow_dif[2];
    uint8_t pi_sqep_sample_interval;
    uint8_t pi_qep_allow_dif[2];
    uint8_t sqep_sample_interval;
    uint8_t sqep_allow_dif[2];
    uint8_t can_master_nodeID;
    uint8_t can_slave_nodeID_A;
    uint8_t can_slave_nodeID_B;
    uint16_t can_hb_consumer_gap;
    uint16_t can_hb_producer_gap;
    uint16_t check_factor;
    uint32_t can_baudrate;
    uint16_t can_pdo_time_gap;
    uint8_t qep12_cross_check;
    uint8_t piqep12_cross_check;
    uint16_t qep1_2_ratio;
    uint16_t piqep1_2_ratio;
    uint8_t qep_speed_allow_dif[2];
    uint8_t piqep_speed_allow_dif[2];
    uint8_t qep_speed_select[MAX_DECELERATE_NUM];
    uint8_t piqep_speed_select[MAX_DECELERATE_NUM];
    uint16_t decelerate_check_interval[MAX_DECELERATE_NUM];
    uint32_t decelerate_sublevel_threshold[MAX_DECELERATE_NUM][DECELERATE_SUBLEVEL_NUM];
    uint8_t md5[16]; // the md5 must Place on the tail
} module_param_t;
#pragma pack()

#endif // GRAPHICAL_CONFIG_PARAM_H
