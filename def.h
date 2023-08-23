#ifndef DEF_H
#define DEF_H
#include "stdio.h"
#include "stdlib.h"
#define UI_WIDTH             1200
#define ui_HEIGHT            650
#define AUTHORIZATION_PASSWD "rhdzkj8888"
/* about font format */
#define TEXT_SIZE_SMALL  "1.5"
#define TEXT_SIZE_MEDIUM "1.7"
#define TEXT_SIZE_LARGE  "1.9"
#define TEXT_SIZE_OVER   "2.5"
#define TEXT_COLOR_RED(STRING, SIZE)                                                     \
    ("<font style = 'font-size:" + QString(SIZE) + "rem; color:red;'>" + QString(STRING) \
     + "</font>"                                                                         \
       "<font color=black> </font>")
#define TEXT_COLOR_BLUE(STRING, SIZE)                                                     \
    ("<font style = 'font-size:" + QString(SIZE) + "rem; color:blue;'>" + QString(STRING) \
     + "</font>"                                                                          \
       "<font color=black> </font>")
#define TEXT_COLOR_GREEN(STRING, SIZE)                                                     \
    ("<font style = 'font-size:" + QString(SIZE) + "rem; color:green;'>" + QString(STRING) \
     + "</font>"                                                                           \
       "<font color=black> </font>")
#define TEXT_COLOR_GREY(STRING, SIZE)                                                     \
    ("<font style = 'font-size:" + QString(SIZE) + "rem; color:grey;'>" + QString(STRING) \
     + "</font>"                                                                          \
       "<font color=black> </font>")

#define TEXT_COLOR_WHILE(STRING, SIZE)                                                       \
    ("<font style = 'font-size:" + QString(SIZE) + "rem; color:#FFFFFF;'>" + QString(STRING) \
     + "</font>"                                                                             \
       "<font color=black> </font>")
#define TEXT_COLOR_YELLOW(STRING, SIZE)                                                      \
    ("<font style = 'font-size:" + QString(SIZE) + "rem; color:#FFFF00;'>" + QString(STRING) \
     + "</font>"                                                                             \
       "<font color=black> </font>")

/* about commnucation cmd */
/***************************命令类型指令集****************************/
enum {
    CMD_TYPE_BL = 0x20,
    CMD_TYPE_LUA,
    CMD_TYPE_SYNC,
    CMD_TYPE_READ,
    CMD_TYPE_WRITE,
    CMD_TYPE_REPORT,
    CMDEND,
};

/******************************命令指令集 ***********************/
enum {
    CMD_PUBLIC_FILE_DOWNLOAD = 0X80,
};  // public CMD option
enum {
    CMD_BL_STATUS = 0x20,
    CMD_BL_REBOOT,
    CMD_BL_RUN_APP,
};  // CMD_TYPE_BL OPTION

enum {
    CMD_LUA_CALL = 0X00,
};  // CMD_TYPE_LUA OPTION
enum {
    CMD_READ_STATUS = 0X00,
    CMD_READ_LOG,
    CMD_READ_PARAM,
};  // CMD_TYPE_READ OPTION
enum {
    CMD_REPORT_INFO = 0X00,
    CMD_REPORT_LOG,
    CMD_REPORT_PARAM,
};  // CMD_TYPE_REPORT OPTION

enum {
    CMD_WRITE_PARAM = 0X00,
};  // CMD_TYPE_WRITE OPTION
/************************************子命令指令集*****************************/
enum {
    SUB_PUBLIC_FILE_DOWNLOAD_SOH = 0x01,  // start of data packet
    SUB_PUBLIC_FILE_DOWNLOAD_STX = 0x02,  // Data packet transfer
    SUB_PUBLIC_FILE_DOWNLOAD_EOT = 0x03,  // End of transmission

    SUB_PUBLIC_FILE_DOWNLOAD_SOH_ACK = 0x10,
    SUB_PUBLIC_FILE_DOWNLOAD_STX_ACK = 0x11,
    SUB_PUBLIC_FILE_DOWNLOAD_EOT_ACK = 0x12,

};  // CMD_PUBLIC_FILE_DOWNLOAD option

enum {
    SUB_BL_STS_START = 0x00,
    SUB_BL_STS_APP_RUN,
    SUB_BL_STS_WAITING,
    SUB_BL_STS_APP_RUN_FAIL,
};  // CMD_BL_STATUS option

enum {
    SUB_LUA_CALL_RUN = 0,
    SUB_LUA_CALL_STOP,
};  // CMD_LUA_CALL OPTION

enum {
    SUB_READ_STATUS_BASE_SELF = 0X00,
    SUB_READ_STATUS_BASE_PAIR,
    SUB_READ_REPLY_BASE_SELF = 0X80,
    SUB_READ_REPLY_BASE_PAIR,
};  // CMD_STATUS_READ OPTION
enum {
    SUB_READ_PARAM_SS = 0,
    SUB_READ_PARAM_MODULE_INFO,
};
enum {
    SUB_REPORT_PARAM_SS = 0,
    SUB_REPORT_PARAM_MODULE_INFO,
    SUB_REPORT_PARAM_WRITE_ACK,
};
enum {
    SUB_WRITE_PARAM_SAFE = 0,
};
/******************************************/
enum {
    SYNC_ID_COMMON = 0,
    SYNC_ID_A,
    SYNC_ID_B,
    SYNC_ID_MAX,
};
/* about encrypt and decrypt */
typedef enum {
    KEY_ENCRYPT = 0,  //加密
    KEY_DECRYPT,      //解密
} key_crypt_e;

/* about struct */

#endif  // DEF_H
