#ifndef STATUS_H
#define STATUS_H

#include "config/param_config.h"
#include "def.h"
#include "qtimer.h"
#include "qwidget.h"
#include "ui_mainwindow.h"
#include <QWidget>

#define FIRMWARE_VER_SIZE 64

typedef enum {
    READ_BASE_SELF_STATUS = 0,
    READ_ERROR_SELF_STATUS,
    READ_BASE_PAIR_STATUS,
    READ_ERROR_PAIR_STATUS,
} read_status_e;

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

typedef struct {
    union {
        uint16_t power_error;
        struct {
            uint16_t power_24v_over_bit : 1;
            uint16_t power_24v_low_bit : 1;
            uint16_t power_24v_dif_bit : 1;
            uint16_t power_5v_over_bit : 1;
            uint16_t power_5v_low_bit : 1;
            uint16_t power_5v_dif_bit : 1;
            uint16_t power_mcua_3v3_over_bit : 1;
            uint16_t power_mcua_3v3_low_bit : 1;
            uint16_t power_mcua_3v3_off_bit : 1;
            uint16_t power_mcub_3v3_over_bit : 1;
            uint16_t power_mcub_3v3_low_bit : 1;
            uint16_t power_mcub_3v3_off_bit : 1;
            uint16_t power_24a_over_bit : 1;
            uint16_t power_24a_dif_bit : 1;
            uint16_t reserve_bits : 2;
        } power_error_bit;
    } power_error_state;
    union {
        uint8_t single_check_error;
        struct {
            uint8_t single_check_fram_bit : 1;
            uint8_t single_check_flash_bit : 1;
            uint8_t single_check_sdram_bit : 1;
            uint8_t single_check_firmware_bit : 1;
            uint8_t single_check_lua_ver_bit : 1;
            uint8_t single_check_param_bit : 1;
            uint8_t single_check_project_invalid_bit : 1;
            uint8_t reserve_bits : 1;
        } single_check_error_bit;
    } single_check_error_state;
    union {
        uint8_t cycle_check_error;
        struct {
            uint8_t cycle_check_com_bit : 1;
            uint8_t cycle_check_fram_bit : 1;
            uint8_t cycle_check_flash_bit : 1;
            uint8_t cycle_check_cpu_bit : 1;
            uint8_t cycle_check_clock_bit : 1;
            uint8_t cycle_check_counter_bit : 1;
            uint8_t cycle_check_stack_bit : 1;
            uint8_t cycle_check_ram_bit : 1;
        } cycle_check_error_bit;
    } cycle_check_error_state;
    union {
        uint8_t communication_error;
        struct {
            uint8_t communication_rs_disconnect_bit : 1;
            uint8_t communication_rs_unstable_bit : 1;
            uint8_t communication_can_disconnect_bit : 1;
            uint8_t communication_can_unstable_bit : 1;
            uint8_t reserve_bits : 4;
        } communication_error_bit;
    } communication_error_state;
    union {
        uint16_t output_error;
        struct {
            uint16_t do_mos_mcua1_bit : 1;
            uint16_t do_mos_mcua2_bit : 1;
            uint16_t do_mos_mcuap_bit : 1;
            uint16_t do_mos_mcub1_bit : 1;
            uint16_t do_mos_mcub2_bit : 1;
            uint16_t do_mos_mcubp_bit : 1;
            uint16_t do_mos_a1_dif_bit : 1;
            uint16_t do_mos_a2_dif_bit : 1;
            uint16_t do_mos_b1_dif_bit : 1;
            uint16_t do_mos_b2_dif_bit : 1;
            uint16_t do_rly_mcua1_bit : 1;
            uint16_t do_rly_mcua2_bit : 1;
            uint16_t do_rly_mcub1_bit : 1;
            uint16_t do_rly_mcub2_bit : 1;
            uint16_t do_rly1_dif_bit : 1;
            uint16_t do_rly2_dif_bit : 1;
        } output_error_bit;
    } output_error_state;
    union {
        uint32_t input_error;
        struct {
            uint32_t di1_dif_bit : 1;
            uint32_t di2_dif_bit : 1;
            uint32_t di3_dif_bit : 1;
            uint32_t di4_dif_bit : 1;
            uint32_t di5_dif_bit : 1;
            uint32_t di6_dif_bit : 1;
            uint32_t di7_dif_bit : 1;
            uint32_t di8_dif_bit : 1;
            uint32_t di9_dif_bit : 1;
            uint32_t di10_dif_bit : 1;
            uint32_t di11_dif_bit : 1;
            uint32_t di12_dif_bit : 1;
            uint32_t ai1_over_bit : 1;
            uint32_t ai2_over_bit : 1;
            uint32_t ai1_dif_bit : 1;
            uint32_t ai2_dif_bit : 1;
            uint32_t qep1_dif_bit : 1;
            uint32_t qep2_dif_bit : 1;
            uint32_t pi_qep1_dif_bit : 1;
            uint32_t pi_qep2_dif_bit : 1;
            uint32_t pi_qep_ch_dif_bit : 1;
            uint32_t pi1_dif_bit : 1;
            uint32_t pi2_dif_bit : 1;
            uint32_t reserve_bit : 9;
        } input_error_bit;
    } input_error_state;
} module_error_t;

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
class MainWindow;
class status : public QWidget {
    Q_OBJECT
public:
    explicit status(QWidget* mparent, QWidget* parent = nullptr);
    Ui::MainWindow* ui         = nullptr;
    MainWindow*     mainwindow = nullptr;

private:
    QTimer         version_read_wait_timer;
    QLineEdit*     label_lineedit[LABEL_NUM];
    QComboBox*     default_combox[LABEL_DI12 + 1];
    QLineEdit*     a_di_data_lineedit[LABEL_DI12 + 1];
    QLineEdit*     b_di_data_lineedit[LABEL_DI12 + 1];
    QComboBox*     di_default_combobox[LABEL_DI12 + 1];
    QTimer         read_state_timer;
    QList<QLabel*> a_power_error_ledlist;
    QList<QLabel*> b_power_error_ledlist;
    QList<QLabel*> a_singlecheck_error_ledlist;
    QList<QLabel*> b_singlecheck_error_ledlist;
    QList<QLabel*> a_cyclecheck_error_ledlist;
    QList<QLabel*> b_cyclecheck_error_ledlist;
    QList<QLabel*> a_output_error_ledlist;
    QList<QLabel*> b_output_error_ledlist;
    QList<QLabel*> a_input_error_ledlist;
    QList<QLabel*> b_input_error_ledlist;
    bool           version_read_success = false;
    QString        a_error_code_str     = "";
    QString        b_error_code_str     = "";

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
    void a_errorinfo_display(uint8_t* frame, int32_t length);
    void b_errorinfo_display(uint8_t* frame, int32_t length);
    void version_display(uint8_t* frame, int32_t length);
    void label_init(void);
    void error_info_init(void);
    void set_led(QLabel* label, QString rgb_color);
private slots:
    void read_status_from_time_slot(void);
    void read_version_result_check_slot(void);
    void read_version_slot(void);
signals:

public slots:
};

#endif  // STATUS_H
