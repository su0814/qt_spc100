#ifndef UPGRADE_H
#define UPGRADE_H

#include "PT/pt.h"
#include "QProgressBar"
#include "def.h"
#include "qwidget.h"
#include "ui_mainwindow.h"
#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QTextBrowser>

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
    IAP_DOWNLOAD_SUCCESS,
    IAP_DOWNLOAD_FAIL,
};

#pragma pack(1)
typedef struct {
    int           pross_status;
    int           result_status;
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
    explicit upgrade(QWidget* mparent, QWidget* parent = 0);

public:
    Ui::MainWindow* ui                = nullptr;
    MainWindow*     mainwindow        = nullptr;
    uint8_t         upgrade_quit_flag = DISABLE_FLAG;
    iap_info_t      iap_info;

private:
    QStringList     upgrade_id_list;
    QString         firmware_pathname;
    QString         firmware_filename;
    struct pt       pt_upgrade;
    uint8_t         boot_status = SUB_BL_STS_START;
    firmware_info_t app_tail;
    /* 升级弹窗 */
    QDialog      upgrade_dialog;
    QPushButton  select_file_button;
    QPushButton  startupgrade_button;
    QPushButton  quit_upgrade_button;
    QProgressBar upgrade_progress;
    QTextBrowser upgrade_log;
    QLabel       tip;
    QLineEdit    file_path;

private:
    void init(void);
    int  boot_upgrade_thread(void);
    int  upgrade_ack_soh_result_phase(uint8_t* retry_cnt, uint8_t* bootid);
    int  upgrade_ack_stx_result_phase(uint8_t* retry_cnt, uint8_t* bootid);
    int  upgrade_ack_eot_result_phase(qint64 starttime);
    void start_upgrade(void);
    void select_upgrade_file(void);
    int  firmware_info_encrypt_decrypt(firmware_info_t* merge_firmware_info, key_crypt_e key);

public:
    void boot_cmd_response(uint8_t* frame, int32_t length);
    void upgrade_serial_connect_callback(void);
    void upgrade_serial_disconnect_callback(void);

private slots:
    void upgrade_slot(void);
    void upgrade_select_file_slot(void);
    void upgrade_start_slot(void);
    void upgrade_quit_slot(void);
};

#endif  // UPGRADE_H
