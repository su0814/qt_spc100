#ifndef DATA_CONFIG_H
#define DATA_CONFIG_H
#include "param_config.h"
#include <QGraphicsRectItem>
#include <QString>
#include <QStringList>
#include <QTextBrowser>
#include <cstdint>

enum {
    TAB_LOGIC_PROJECT_OVERVIEW_ID = 0,
    TAB_LOGIC_GRAPHICAL_CODE_ID,
    TAB_LOGIC_REPORT_ID,
};

typedef enum {
    BLOCK_TYPE_LINE = 1,
    BLOCK_TYPE_CONNECT,
    BLOCK_TYPE_INPUTBLOCK,
    BLOCK_TYPE_OUTPUTBLOCK,
    BLOCK_TYPE_BASELOGIC,
    BLOCK_TYPE_APPLYLOGIC,
    BLOCK_TYPE_DELAY_COUNTER,
    BLOCK_TYPE_SPEED,

} block_type_e;

typedef enum {
    CONNECT_POINT_IOTYPE_INPUT = 0,
    CONNECT_POINT_IOTYPE_OUTPUT,
} connect_point_iotype_e;

typedef enum {
    CONNECT_POINT_DATATYPE_NONE    = 0X00,
    CONNECT_POINT_DATATYPE_ENCODER = 0X01,
} connect_point_datatype_e;

typedef enum {
    ACK_STATUS_IDLE = 0,
    ACK_STATUS_WAITING,
    ACK_STATUS_SUCCESS,
    ACK_STATUS_FAIL,
    ACK_STATUS_NORESPONSE,
} ack_status_e;

typedef enum {
    DEVICE_LINE_STATUS_OFF = 0,
    DEVICE_LINE_STATUS_ON,
} device_line_status_e;

typedef enum {
    BLOCK_STATE_IDE   = 0,
    BLOCK_STATE_DEBUG = 0X01,
    BLOCK_STATE_FOCUS = 0X02,
    BLOCK_STATE_ERROR = 0X04,
} block_state_e;

typedef enum {
    MESSAGE_TYPE_INFO = 0,
    MESSAGE_TYPE_WARNING,
    MESSAGE_TYPE_ERROR,
    MESSAGE_TYPE_QUESTION,
} messabe_type_e;

typedef struct {
    bool    is_new   = false;
    bool    is_valid = false;
    QString filepath = "";
    QString filename = "";
} project_management_info_t;

typedef struct {
    bool               is_valid   = false;
    int                width      = 0;
    int                height     = 0;
    QGraphicsRectItem* probe_rect = nullptr;
} drop_tool_info_t;

typedef struct {
    bool    responsed = false;
    uint8_t ack_code  = 0;
} ack_info_t;

typedef struct {
    ack_info_t   ack_info[2];
    ack_status_e ack_status;
    uint8_t      retry = 0;
} ack_enter_t;

#pragma pack(1)
typedef struct {
    uint32_t project_size;
    uint32_t usercode_size;
    uint32_t param_size;
    uint8_t  md5[16];
    char     date[30];
} project_info_t;
#pragma pack()

// 注册自定义结构体
Q_DECLARE_METATYPE(project_info_t)
extern const QStringList qep_resource;

extern const QList<QStringList>        cat3_source_function;
extern const QList<QList<QStringList>> cat2_source_function;

extern const QStringList        cat3_qep_dir_function;
extern const QList<QStringList> cat2_qep_dir_function;

extern const QList<QList<QStringList>> model_name;

extern const QList<QStringList> cat2_output;
extern const QList<QStringList> cat3_output;

/* project info json name */
extern const QString project_name;
extern const QString project_company_name;
extern const QString project_author_ver;
extern const QString project_path;
/* logic propgram view project json name */
extern const QString project_object_logic_programe;
extern const QString project_programe_logic;
extern const QString project_programe_condition;
extern const QString project_programe_line;
/* param */
extern const QString project_safety_param;
extern const QString project_config_photo;
/* color */
extern const QColor LED_COLOR_RED;
extern const QColor LED_COLOR_GREEN;
extern const QColor LED_COLOR_DARKRED;
extern const QColor LED_COLOR_DARKGREEN;
#endif  // DATA_CONFIG_H
