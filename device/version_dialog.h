#ifndef VERSION_DIALOG_H
#define VERSION_DIALOG_H
#define FIRMWARE_VER_SIZE 64
#include "config/data_config.h"
#include <QDialog>
#include <QTimer>
#pragma pack(1)
typedef struct {
    /*hardware info*/
    uint8_t mcu_state;  //确定mcu是A还是B
    /*runtime info*/
    uint64_t run_time_ms;  //软件运行时间ms
    /*version info*/
    char coreboard_hardware_version[8];        //核心板版本
    char bottomboard_hardware_version[8];      //底板版本
    char firmware_version[FIRMWARE_VER_SIZE];  //固件版本
    char bootloader_version[FIRMWARE_VER_SIZE];
} module_info_t;

#pragma pack()

namespace Ui {
class version_Dialog;
}
class MainWindow;
class version_Dialog : public QDialog {
    Q_OBJECT

public:
    explicit version_Dialog(QWidget* parent = 0);
    ~version_Dialog();
    MainWindow* mainwindow = nullptr;

public:
    void version_display(uint8_t* frame, int32_t length);

private:
    Ui::version_Dialog* ui;
    QTimer              version_read_wait_timer;
    bool                version_read_success = false;

private slots:
    void device_line_status_change_slots(device_line_status_e status);
    void dialog_show_slot(void);
    void read_version_result_check_slot(void);
};

#endif  // VERSION_DIALOG_H
