#ifndef DEF_H
#define DEF_H
#include "stdio.h"
#include "stdlib.h"
#define UI_WIDTH  1200
#define UI_HEIGHT 800

#define DESKTOP_BASE_WIDTH  1920
#define DESKTOP_BASE_HEIGHT 1080
#define SPC100_PHOTO_WIDTH  350
#define SPC100_PHOTO_HEIGHT 630

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
    CMD_TYPE_PROJECT,
    CMD_TYPE_DEVICE,
    CMDEND,
};

/******************************命令指令集 ***********************/
enum {
    CMD_PUBLIC_FILE_DOWNLOAD = 0X80,
    CMD_PUBLIC_FILE_READBACK,
};  // public CMD option
enum {
    CMD_BL_STATUS = 0x20,
    CMD_BL_REBOOT,
    CMD_BL_RUN_APP,
};  // CMD_TYPE_BL OPTION

enum {
    CMD_PROJECT_USERCODE = 0,
    CMD_PROJECT_INFO,
    CMD_PROJECT_DEBUG,
};  // CMD_TYPE_PROJECT option
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
    CMD_DEVICE_PASSWD = 0,
    CMD_DEVICE_HEARTBEAT,
};
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
    SUB_PUBLIC_FILE_READBACK_SOH = 0x01,  // start of data packet
    SUB_PUBLIC_FILE_READBACK_STX = 0x02,  // Data packet transfer
    SUB_PUBLIC_FILE_READBACK_EOT = 0x03,  // End of transmission

    SUB_PUBLIC_FILE_READBACK_SOH_ACK = 0x10,
    SUB_PUBLIC_FILE_READBACK_STX_ACK = 0x11,
    SUB_PUBLIC_FILE_READBACK_EOT_ACK = 0x12,

};  // CMD_PUBLIC_FILE_READBACK option

enum {
    SUB_BL_STS_START = 0x00,
    SUB_BL_STS_APP_RUN,
    SUB_BL_STS_WAITING,
    SUB_BL_STS_APP_RUN_FAIL,
};  // CMD_BL_STATUS option

enum {
    SUB_PROJECT_USERCODE_RUN = 0,
    SUB_PROJECT_USERCODE_STOP,
};  // CMD_PROJECT_USERCODE OPTION

enum {
    SUB_PROJECT_INFO_VERIFY     = 0,
    SUB_PROJECT_INFO_VERIFY_ACK = 0x80,
};  // CMD_PROJECT_INFO OPTION

enum {
    SUB_PROJECT_DEBUG_DATA     = 0,
    SUB_PROJECT_DEBUG_DATA_ACK = 0x80,
};  // CMD_PROJECT_INFO OPTION

enum {
    SUB_READ_STATUS_BASE_SELF = 0X00,
    SUB_READ_STATUS_BASE_PAIR,
    SUB_READ_STATUS_ERROR_SELF,
    SUB_READ_STATUS_ERROR_PAIR,
    SUB_READ_STATUS_VERSION,
    SUB_READ_REPLY_BASE_SELF = 0X80,
    SUB_READ_REPLY_BASE_PAIR,
    SUB_READ_REPLY_ERROR_SELF,
    SUB_READ_REPLY_ERROR_PAIR,
    SUB_READ_REPLY_VERSION,
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
    SUB_DEVICE_PASSWD_CHANGE = 0,
    SUB_DEVICE_PASSWD_VERIFY,
    SUB_DEVICE_PASSWD_CHANGE_ACK = 0X80,
    SUB_DEVICE_PASSWD_VERIFY_ACK,
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

typedef enum {
    USER_REGULAR = 0,
    USER_AUTHORIZED,
} user_permissions_e;
/* about struct */

#endif  // DEF_H
