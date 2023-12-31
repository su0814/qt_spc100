#ifndef UPGRADE_H
#define UPGRADE_H

#include "PT/pt.h"
#include "def.h"
#include "qwidget.h"
#include "ui_mainwindow.h"

/* about upgrade */
#define ENCRYPTED_CODE_BYTE     (uint32_t)(0x80 + 0X2A)
#define ENCRYPTED_CODE_HALFWORD (uint32_t)((ENCRYPTED_CODE_BYTE << 8) | (ENCRYPTED_CODE_BYTE))
#define ENCRYPTED_CODE_WORD     (uint32_t)((ENCRYPTED_CODE_HALFWORD << 16) | ENCRYPTED_CODE_HALFWORD)
#define VALID_MAGIC_ENCRYPT     (uint32_t)(0X3A038A22 * 2 + 1)
#define VALID_MAGIC             0xDEADBEEF

#define UPGRADE_INDEPENDENT 0
#define UPGRADE_SYNCHRONOUS 1

enum {
    DISABLE_FLAG = 0,
    ENABLE_FLAG,
};

enum {
    IAP_DOWNLOAD_IDLE = 0,
    IAP_DOWNLOAD_START,
    IAP_DOWNLOADING,
    IAP_DOWNLOAD_FINISH,
    IAP_DOWNLOAD_WAIT_APP_BOOT,
    IAP_DOWNLOAD_END,
};

#pragma pack(1)
typedef struct {
    int           status;
    int           ack_nack[SYNC_ID_MAX];
    signed char   error_code[SYNC_ID_MAX];
    unsigned char upgrade_mode;
    int           reqpackseq[SYNC_ID_MAX];
    unsigned char bl_start_flag[SYNC_ID_MAX];
    unsigned char app_start_flag[SYNC_ID_MAX];
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
} firmware_info_t;
#pragma pack()
class MainWindow;
class upgrade : public QWidget {
public:
    explicit upgrade(QWidget* parent = 0);

public:
    Ui::MainWindow* ui                = nullptr;
    MainWindow*     mainwindow        = nullptr;
    uint8_t         upgrade_quit_flag = DISABLE_FLAG;
    iap_info_t      iap_info;

private:
    QStringList upgrade_id_list;
    QString     firmware_pathname;
    QString     firmware_filename;
    struct pt   pt_upgrade;
    uint8_t     boot_status = SUB_BL_STS_START;

    firmware_info_t app_tail;
    QStringList     module_name_list;

public:
    int  boot_upgrade_thread(void);
    int  upgrade_ack_soh_result_phase(uint8_t* retry_cnt, uint8_t* bootid);
    int  upgrade_ack_stx_result_phase(uint8_t* retry_cnt, uint8_t* bootid);
    int  upgrade_ack_eot_result_phase(qint64 starttime);
    void boot_cmd_response(uint8_t* frame, int32_t length);
    void start_upgrade(void);
    void select_upgrade_file(void);
    int  firmware_info_encrypt_decrypt(firmware_info_t* merge_firmware_info, key_crypt_e key);

    void upgrade_serial_connect_callback(void);
    void upgrade_serial_disconnect_callback(void);
    void upgrade_ui_resize(uint32_t width, uint32_t height);
};

#endif  // UPGRADE_H
