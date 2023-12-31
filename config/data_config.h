#ifndef DATA_CONFIG_H
#define DATA_CONFIG_H
#include "param_config.h"
#include <QGraphicsRectItem>
#include <QString>
#include <QStringList>
#include <cstdint>
typedef enum {
    BLOCK_TYPE_LINE = 1,
    BLOCK_TYPE_LOGIC,
    BLOCK_TYPE_CONDITION,
    BLOCK_TYPE_CONNECT,

} block_type_e;

typedef enum {
    CONNECT_POINT_TYPE_INPUT = 0,
    CONNECT_POINT_TYPE_OUTPUT,
} connect_point_type_e;

typedef enum {
    CALC_TYPE_ADD = 0,
    CALC_TYPE_SUBTRACT,
    CALC_TYPE_MULTIPLY,
    CALC_TYPE_DIVISION,
} calc_type_e;

typedef enum {
    TOOL_TYPE_CONDI_DI = 0,
    TOOL_TYPE_CONDI_AI,
    TOOL_TYPE_CONDI_PI,
    TOOL_TYPE_CONDI_QEP,
    TOOL_TYPE_LOGIC_AND = 0x80,
    TOOL_TYPE_LOGIC_OR,
    TOOL_TYPE_LOGIC_NOT,
    //    TOOL_TYPE_LOGIC_GT,
    //    TOOL_TYPE_LOGIC_LT,
    //    TOOL_TYPE_LOGIC_GTOET,
    //    TOOL_TYPE_LOGIC_LTOET,
    //    TOOL_TYPE_LOGIC_ET,
    TOOL_TYPE_LOGIC_SF,
    TOOL_TYPE_LOGIC_EXIT,
} tool_type_e;

typedef enum {
    TOOL_ID_DI1 = 0,
    TOOL_ID_DI2,
    TOOL_ID_DI3,
    TOOL_ID_DI4,
    TOOL_ID_DI5,
    TOOL_ID_DI6,
    TOOL_ID_DI7,
    TOOL_ID_DI8,
    TOOL_ID_DI9,
    TOOL_ID_DI10,
    TOOL_ID_DI11,
    TOOL_ID_DI12,
    TOOL_ID_AI1 = 0,
    TOOL_ID_AI2,
    TOOL_ID_PI1 = 0,
    TOOL_ID_PI2,
    TOOL_ID_QEP1 = 0,
    TOOL_ID_QEP2,
    TOOL_ID_PI_QEP1,
    TOOL_ID_PI_QEP2,
    TOOL_ID_LOGIC_AND  = 0,
    TOOL_ID_LOGIC_OR   = 0,
    TOOL_ID_LOGIC_NOT  = 0,
    TOOL_ID_LOGIC_SF   = 0,
    TOOL_ID_LOGIC_EXIT = 0,
} tool_id_e;

enum {
    SF_TYPE_ESTOP = 0,
    SF_TYPE_SPEED_MONITOR,
    SF_TYPE_RELAY_MONITOR,
    SF_TYPE_USER,
};

typedef struct {
    union {
        uint8_t value = 0;
        struct {
            uint16_t input1 : 1;
            uint16_t input2 : 1;
            uint16_t input3 : 1;
            uint16_t input4 : 1;
            uint16_t reserve_bits : 4;
        } error_bit;
    } input_error;
    union {
        uint8_t value = 0;
        struct {
            uint16_t output1 : 1;
            uint16_t reserve_bits : 7;
        } error_bit;
    } output_error;
    union {
        uint8_t value = 0;
        struct {
            uint8_t no_tool : 1;  //工具列表内不存在
            uint8_t no_ss : 1;
            uint8_t reserve_bits : 5;
        } error_bit;
    } other_error;
} block_error_t;

typedef struct {

    tool_type_e tool_type;
    tool_id_e   tool_id;
} tool_info_t;

typedef struct {
    QList<uint32_t> parent_id;
    uint32_t        self_id;
    QString         logic_string = "";
    tool_info_t     block_info;
    QString         other_name = "";
} block_attribute_t;

typedef struct {
    int calc_type;
} logic_param_t;

/* block right menu set */
typedef struct {
    bool is_reverse = false;
} condition_di_set_t;

typedef struct {
    int      calc_type_index = 0;
    uint32_t value           = 0;
} condition_ai_pi_qep_set_t;

/* sf info */
typedef struct {
    uint8_t code    = SF_USER_CODE;
    bool    is_used = false;
} sf_code_t;

typedef struct {
    QString  name;
    uint8_t  sf_code;
    uint8_t  sf_type     = SF_TYPE_USER;
    uint8_t  ss_code     = 0XFF;
    uint16_t delay_time  = 0;
    uint16_t option_time = 100;
} sf_param_t;

typedef struct {
    sf_code_t  sf_code[MAX_SF_NUM];
    sf_param_t sf_param[MAX_SF_NUM];
    QString    block_name[MAX_SF_NUM] = { "" };
    uint8_t    used_number            = 0;
} sf_used_info_t;

/* ss info */
typedef struct {
    uint8_t ss_code        = 0;
    uint8_t relevant_state = 0;
} ss_info_t;

typedef struct {
    bool    is_new   = false;
    bool    is_valid = false;
    QString filepath = "";
    QString filename = "";
} project_management_info_t;

/* report */
typedef struct {
    bool is_config[INPUT_RESOURCE_NUM] = { false };
    bool is_used[INPUT_RESOURCE_NUM]   = { false };
} input_resource_info_t;

typedef struct {
    tool_info_t        tool_info;
    bool               is_valid   = false;
    int                width      = 0;
    int                height     = 0;
    QGraphicsRectItem* probe_rect = nullptr;
} drop_tool_info_t;

extern const QStringList calc_str;

extern const QStringList        lua_di_func;
extern const QStringList        lua_ai_func;
extern const QStringList        lua_pi_func;
extern const QStringList        lua_qep_func;
extern const QStringList        lua_logic_keyword;
extern const QStringList        di_resource;
extern const QStringList        ai_resource;
extern const QStringList        pi_resource;
extern const QStringList        qep_resource;
extern const QList<QStringList> input_resource;
extern const QStringList        sf_type_str;

/* project info json name */
extern const QString project_name;
extern const QString project_lua_name;
extern const QString project_lua_ver;
extern const QString project_ref_time;
extern const QString project_path;
extern const QString project_advanced_program;
/* device source project json name */
extern const QString project_object_device;
extern const QString project_device_icheckstate;
extern const QString project_device_iothername;
extern const QString project_device_ossinfo;
/* logic propgram view project json name */
extern const QString project_object_logic_programe;
extern const QString project_programe_logic;
extern const QString project_programe_condition;
extern const QString project_programe_line;
/* coroutine lua */
extern const QString project_object_coroutine;
#endif  // DATA_CONFIG_H
