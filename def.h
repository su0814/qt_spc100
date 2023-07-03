#ifndef DEF_H
#define DEF_H
#include "stdio.h"
#include "stdlib.h"
#define ENCRYPTED_CODE_BYTE     (uint32_t)(0x80 + 0X2A)
#define ENCRYPTED_CODE_HALFWORD (uint32_t)((ENCRYPTED_CODE_BYTE << 8) | (ENCRYPTED_CODE_BYTE))
#define ENCRYPTED_CODE_WORD     (uint32_t)((ENCRYPTED_CODE_HALFWORD << 16) | ENCRYPTED_CODE_HALFWORD)
#define VALID_MAGIC_ENCRYPT     (uint32_t)(0X3A038A22 * 2 + 1)
#define VALID_MAGIC             0xDEADBEEF

#define UPGRADE_INDEPENDENT 0
#define UPGRADE_SYNCHRONOUS 1

enum {
    BOOT_SYNC_ID_COMMON = 0,
    BOOT_SYNC_ID_A,
    BOOT_SYNC_ID_B,
    BOOT_SYNC_ID_MAX,
};

enum {
    DISABLE_FLAG = 0,
    ENABLE_FLAG,
};

enum {
    CMD_TYPE_BL = 0x20,
    CMDEND,
};
// CMD_TYPE_BL option
enum {
    CMD_BL_STATUS = 0x20,
    CMD_BL_REBOOT,
    CMD_BL_RUN_APP,
    CMD_BL_DATA_TRANSFER,
};

// CMD_BL_STATUS option
enum {
    SUB_BL_STS_START = 0x00,
    SUB_BL_STS_APP_RUN,
    SUB_BL_STS_WAITING,
    SUB_BL_STS_APP_RUN_FAIL,
};
// CMD_BL_DATA_TRANSFER option
enum {
    SUB_BL_DT_SOH = 0x01,  // start of data packet
    SUB_BL_DT_STX = 0x02,  // Data packet transfer
    SUB_BL_DT_EOT = 0x04,  // End of transmission

    SUB_BL_SOH_ACK = 0x10,
    SUB_BL_STX_ACK = 0x11,
    SUB_BL_EOT_ACK = 0x12,

};
enum {
    IAP_DOWNLOAD_IDLE = 0,
    IAP_DOWNLOAD_START,
    IAP_DOWNLOADING,
    IAP_DOWNLOAD_FINISH,
    IAP_DOWNLOAD_WAIT_APP_BOOT,
    IAP_DOWNLOAD_END,
};
// CMDCTL option
enum {
    // ENC_CALI    = 0x50,  // encoder angle calibration
    ENC_UPGRADE = 0x51,  // encoder FW upgrade
};

typedef enum {
    KEY_ENCRYPT = 0,  //加密
    KEY_DECRYPT,      //解密
} key_crypt_e;

typedef struct {
    int           status;
    int           ack_nack[BOOT_SYNC_ID_MAX];
    signed char   error_code[BOOT_SYNC_ID_MAX];
    unsigned char upgrade_mode;
    int           reqpackseq[BOOT_SYNC_ID_MAX];
    unsigned char bl_start_flag[BOOT_SYNC_ID_MAX];
    unsigned char app_start_flag[BOOT_SYNC_ID_MAX];
    int           packseq;
    unsigned int  packlen;
    unsigned int  write_size;
    unsigned int  fw_size;
    int           fd_s;
    unsigned char md5[16];
    char*         filebuf;
} iap_info_t;
typedef struct {
    unsigned int  magic;  // magic number must be 0xdeadbeef, to make sure that the firmware_info partition is exist
    char          fw_version[64];  // must make sure that the bytes after '\0' are all 0x00, instead of 0xff
    char          fw_name[64];
    unsigned char fw_md5[16];
    unsigned int  fw_length;
    unsigned char fw_info_md5[16];  // the md5 of the items above must equal to this value, otherwise the firmware
                                    // config was considered incomplete.
} __attribute__((packed)) firmware_info_t;
#endif  // DEF_H
