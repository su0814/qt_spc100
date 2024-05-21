#ifndef STATUS_H
#define STATUS_H

#include "config/def.h"
#include "config/param_config.h"
#include "qtimer.h"
#include "qwidget.h"
#include "ui_mainwindow.h"
#include <QWidget>

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
} module_state_t;

typedef struct {
    union {
        uint16_t output_error_byte;
        struct {
            uint16_t do_mos1_bit : 1;
            uint16_t do_mos2_bit : 1;
            uint16_t do_mos3_bit : 1;
            uint16_t do_mos4_bit : 1;
            uint16_t do_rly1_bit : 1;
            uint16_t do_rly2_bit : 1;
            uint16_t reserve_bit : 8;
        } output_error_bit;
    } output_error_state;
    union {
        uint32_t input_error_byte;
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
            uint32_t qep12_dif_bit : 1;
            uint32_t pi_qep1_dif_bit : 1;
            uint32_t pi_qep2_dif_bit : 1;
            uint32_t pi_qep12_dif_bit : 1;
            uint32_t pi_qep_ch_dif_bit : 1;
            uint32_t pi1_dif_bit : 1;
            uint32_t pi2_dif_bit : 1;
            uint32_t reserve_bit : 7;
        } input_error_bit;
    } input_error_state;
} module_warning_t;

typedef struct {
    union {
        uint16_t power_error_byte;
        struct {
            uint16_t power_24v_over_bit : 1;
            uint16_t power_24v_low_bit : 1;
            uint16_t power_24v_dif_bit : 1;
            uint16_t power_5v_over_bit : 1;
            uint16_t power_5v_low_bit : 1;
            uint16_t power_5v_dif_bit : 1;
            uint16_t power_pair_3v3_over_bit : 1;
            uint16_t power_pair_3v3_low_bit : 1;
            uint16_t power_pair_3v3_off_bit : 1;
            uint16_t power_24a_over_bit : 1;
            uint16_t power_24a_dif_bit : 1;
            uint16_t reserve_bits : 5;
        } power_error_bit;
    } power_error_state;

    union {
        uint16_t element_error_byte;
        struct {
            uint16_t element_fram_bit : 1;
            uint16_t element_flash_bit : 1;
            uint16_t element_sdram_bit : 1;
            uint16_t element_cpu_bit : 1;
            uint16_t element_clock_bit : 1;
            uint16_t element_count_bit : 1;
            uint16_t element_stack_bit : 1;
            uint16_t element_ram_bit : 1;
            uint16_t reserve_bits : 8;
        } element_error_bit;
    } element_error_state;
    union {
        uint8_t version_error_byte;
        struct {
            uint8_t version_firmware_bit : 1;
            uint8_t version_bootloader_bit : 1;
            uint8_t version_param_bit : 1;
            uint8_t version_project_bit : 1;
            uint8_t version_project_invalid_bit : 1;
            uint8_t reserve_bits : 3;
        } version_error_bit;
    } version_error_state;
    union {
        uint8_t comm_error_byte;
        struct {
            uint8_t comm_sync_disconnect_bit : 1;
            uint8_t comm_can_disconnect_bit : 1;
            uint8_t reserve_bits : 6;
        } comm_error_bit;
    } comm_error_state;
} module_erro_t;
typedef struct {
    module_warning_t module_warning;
    module_erro_t    module_error;
} module_fault_t;

#pragma pack()
class MainWindow;
class status : public QWidget {
    Q_OBJECT
public:
    explicit status(QWidget* mparent, QWidget* parent = nullptr);
    Ui::MainWindow* ui         = nullptr;
    MainWindow*     mainwindow = nullptr;

private:
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

public:
    void read_status_switch(bool en);
    void type_status_response(uint8_t* frame, int32_t length);
    void status_serial_disconnect_callback(void);

private:
    void a_baseinfo_display(uint8_t* frame, int32_t length);
    void b_baseinfo_display(uint8_t* frame, int32_t length);
    void faultinfo_display(uint8_t* frame, int32_t length);
    void error_info_init(void);
    void set_led(QLabel* label, QString rgb_color);
private slots:
    void read_status_from_time_slot(void);
signals:

public slots:
};

#endif  // STATUS_H
