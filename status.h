#ifndef STATUS_H
#define STATUS_H

#include "def.h"
#include "qtimer.h"
#include "qwidget.h"
#include "ui_mainwindow.h"
#include <QWidget>
typedef enum {
    STATE_RUN = 1,
    STATE_ERROR,
    STATE_SAFE,
} module_state_e;

enum {
    LABEL_DI1 = 0,
    LABEL_DI2,
    LABEL_DI3,
    LABEL_DI4,
    LABEL_DI5,
    LABEL_DI6,
    LABEL_DI7,
    LABEL_DI8,
    LABEL_DI9,
    LABEL_DI10,
    LABEL_DI11,
    LABEL_DI12,
    LABEL_AI1,
    LABEL_AI2,
    LABEL_PI_DIR1,
    LABEL_PI_DIR2,
    LABEL_PI_ENCCNT1,
    LABEL_PI_ENCCNT2,
    LABEL_PI_CH1CNT,
    LABEL_PI_CH2CNT,
    LABEL_RELAY1,
    LABEL_RELAY2,
    LABEL_SMOS1,
    LABEL_SMOS2,
    LABEL_SMOS3,
    LABEL_SMOS4,
    LABEL_QEP_DIR1,
    LABEL_QEP_CNT1,
    LABEL_QEP_DIR2,
    LABEL_QEP_CNT2,
    LABEL_NUM,
};

#pragma pack(1)
typedef struct {
    /*module state*/
    uint8_t module_state;  //本机运行状态
    /*voltage*/
    uint16_t voltage_24v;  // 24V电压
    uint16_t voltage_5v;   // 5V电压
    uint16_t voltage_3v3;  // 3.3V电压
    uint16_t current_24v;  // 24V电流
    /*ai*/
    uint16_t analog_value1;  // ai1
    uint16_t analog_value2;  // ai2
    /*di*/
    union {
        uint16_t di_s;
        struct {
            uint16_t bit1 : 1;
            uint16_t bit2 : 1;
            uint16_t bit3 : 1;
            uint16_t bit4 : 1;
            uint16_t bit5 : 1;
            uint16_t bit6 : 1;
            uint16_t bit7 : 1;
            uint16_t bit8 : 1;
            uint16_t bit9 : 1;
            uint16_t bit10 : 1;
            uint16_t bit11 : 1;
            uint16_t bit12 : 1;
            uint16_t reserve_bits : 4;
        } di_bit;
    } di_state;
    /*relay*/
    union {
        uint8_t relay_s;
        struct {
            uint8_t set_bit1 : 1;
            uint8_t set_bit2 : 1;
            uint8_t read_bit1 : 1;
            uint8_t read_bit2 : 1;
            uint8_t reserve_bits : 4;
        } relay_bit;
    } relay_state;
    /*mos*/
    union {
        uint8_t mos_s;
        struct {
            uint8_t set_bit1 : 1;
            uint8_t set_bit2 : 1;
            uint8_t mos_p_set_bit1 : 1;
            uint8_t mos_p_set_bit2 : 1;
            uint8_t mos_a_read_bit1 : 1;
            uint8_t mos_a_read_bit2 : 1;
            uint8_t mos_b_read_bit1 : 1;
            uint8_t mos_b_read_bit2 : 1;
        } mos_bit;
    } mos_state;
    /*qep & pi*/
    union {
        uint8_t dir_s;
        struct {
            uint8_t qep_dir_ma_read_bit : 1;
            uint8_t qep_dir_mb_read_bit : 1;
            uint8_t pi_dir_ma_read_bit : 1;
            uint8_t pi_dir_mb_read_bit : 1;
            uint8_t reserve_bit : 4;
        } dir_bit;
    } dir_state;
    uint32_t qep_ma_cnt;  // a channel qep计数
    uint32_t qep_mb_cnt;  // b channel qep计数
    uint32_t encoder_pi_ma_cnt;
    uint32_t encoder_pi_mb_cnt;
    uint32_t pi1_cnt;
    uint32_t pi2_cnt;
    uint8_t  lost_pkt_rate;
    uint32_t safe_state_en_flag;
} module_state_t;
#pragma pack()
class MainWindow;
class status : public QWidget {
    Q_OBJECT
public:
    explicit status(QWidget* parent = nullptr);
    Ui::MainWindow* ui         = nullptr;
    MainWindow*     mainwindow = nullptr;

private:
    QLineEdit* label_lineedit[LABEL_NUM];
    QComboBox* default_combox[LABEL_DI12 + 1];
    QLineEdit* a_di_data_lineedit[LABEL_DI12 + 1];
    QLineEdit* b_di_data_lineedit[LABEL_DI12 + 1];
    QComboBox* di_default_combobox[LABEL_DI12 + 1];
    QTimer*    read_state_timer = nullptr;

public:
    void read_status_switch(bool en);
    void type_status_response(uint8_t* frame, int32_t length);
    void label_clear(void);
    void label_save(void);
    void label_read(void);
    void status_serial_disconnect_callback(void);
    void status_serial_connect_callback(void);
    void status_ui_resize(uint32_t width, uint32_t height);

private:
    void a_baseinfo_display(uint8_t* frame, int32_t length);
    void b_baseinfo_display(uint8_t* frame, int32_t length);
    void label_init(void);
private slots:
    void read_status_thread(void);
signals:

public slots:
};

#endif  // STATUS_H
