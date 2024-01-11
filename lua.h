#ifndef LUA_H
#define LUA_H

#include "PT/pt.h"
#include "config/data_config.h"
#include "def.h"
#include "qwidget.h"
#include "ui_mainwindow.h"
#include <QWidget>
#define LUA_SYNC_DOWNLOAD 1
typedef enum {
    LUA_DOWNLOAD_IDLE = 0,
    LUA_DOWNLOAD_DOWNLOADING,
    LUA_DOWNLOAD_SOH,
    LUA_DOWNLOAD_STX,
    LUA_DOWNLOAD_EOT,
    LUA_DOWNLOAD_FINISH,
    LUA_DOWNLOAD_END,
} lua_download_status_e;

typedef enum {
    READBACK_STATUS_IDLE = 0,
    READBACK_STATUS_SOH,
    READBACK_STATUS_STX,
    READBACK_STATUS_EOT,
    READBACK_STATUS_SUCCESS,
    READBACK_STATUS_FAIL,
} readback_status_e;

typedef struct {
    lua_download_status_e status;
    lua_download_status_e download_progress;
    int                   ack[SYNC_ID_MAX];
    signed char           error_code[SYNC_ID_MAX];
    unsigned int          file_size;
    char*                 file_buf;
    unsigned int          write_size;
    unsigned int          packlen;
    int                   reqpackseq[SYNC_ID_MAX];
    int                   packseq;
    unsigned char         md5[16];
} lua_download_info_t;

typedef struct {
    readback_status_e status;
    int               ack;
    int               packseq;
    uint8_t           error_code;
    uint32_t          file_size;
    uint32_t          read_size;
    QByteArray        project_file;
} readback_info_t;

class MainWindow;
class lua : public QWidget {
public:
    explicit lua(QWidget* parent = nullptr);
    Ui::MainWindow* ui         = nullptr;
    MainWindow*     mainwindow = nullptr;

public:
    void            lua_cmd_response(uint8_t* frame, int32_t length);
    void            lua_cmd_report_response(uint8_t* frame, int32_t length);
    void            lua_cmd_run(void);
    void            lua_log_save(void);
    void            lua_cmd_log(uint8_t id);
    void            lua_serial_disconnect_callback(void);
    void            lua_serial_connect_callback(void);
    void            lua_download_from_project(QByteArray* file, project_info_t project_file);
    bool            readback_project_file(project_info_t project_file);
    readback_info_t readback_info;
    project_info_t  transmit_project_info;
    project_info_t  read_project_info;
private slots:

private:
    QString             luafile_pathname;
    QString             luafile_filename;
    lua_download_info_t lua_download_info;

    struct pt   pt_download;
    struct pt   pt_readback;
    QStringList sync_id_list;

private:
    int  lua_download_file_thread(void);
    int  download_ack_soh_result_phase(uint8_t* retry_cnt);
    int  download_ack_stx_result_phase(uint8_t* retry_cnt);
    int  download_ack_eot_result_phase(qint64 starttime);
    void readback_ack_soh_prase(uint8_t* frame, int32_t length);
    void readback_ack_stx_prase(uint8_t* frame, int32_t length);
    void readback_ack_eot_prase(uint8_t* frame, int32_t length);
    int  readback_file_thread(void);
    void lua_log_display(uint8_t sub, uint8_t* frame, int32_t length);
    void lua_log_display(char* str);
};

#endif  // LUA_H
