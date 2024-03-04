#ifndef PARAM_H
#define PARAM_H

#include "ui_mainwindow.h"
#include <QJsonObject>
#include <QTimer>
#include <QWidget>
#define LUA_FILE_VER_SIZE 24
#define PARAM_IS_ACTIVE   0x8387
enum {
    SLV_DI1 = 0,
    SLV_DI2,
    SLV_DI3,
    SLV_DI4,
    SLV_DI5,
    SLV_DI6,
    SLV_DI7,
    SLV_DI8,
    SLV_RELAY1,
    SLV_RELAY2,
    SLV_AI1,
    SLV_AI2,
    SLV_NUM,
};

enum {
    SS_OUTPUT = 0,
    SS_RELAY1,
    SS_RELAY2,
    SS_SMOS1,
    SS_SMOS2,
    SS_SMOS3,
    SS_SMOS4,
    SS_NUM,
};

enum {
    PI_AF_PI = 0,
    PI_AF_QEP,
    PI_AF_DI,
    AI_AF_AI = 0,
    AI_AF_DI,
};

#pragma pack(1)
typedef struct {
    /*di safe level*/
    union {
        uint16_t di_slv_bytes;
        struct {
            uint16_t di1_slv_bit : 1;
            uint16_t di2_slv_bit : 1;
            uint16_t di3_slv_bit : 1;
            uint16_t di4_slv_bit : 1;
            uint16_t di5_slv_bit : 1;
            uint16_t di6_slv_bit : 1;
            uint16_t di7_slv_bit : 1;
            uint16_t di8_slv_bit : 1;
            uint16_t di_slv_reserve_bit : 8;
        } di_slv_bit;
    } di_slv;
    /*relay safe level*/
    union {
        uint8_t relay_slv_byte;
        struct {
            uint8_t relay1_slv_bit : 1;
            uint8_t relay2_slv_bit : 1;
            uint8_t relay_slv_reserve_bit : 6;
        } relay_slv_bit;
    } relay_slv;
    /*ai safe level*/
    union {
        uint8_t ai_slv_byte;
        struct {
            uint8_t ai1_slv_bit : 1;
            uint8_t ai2_slv_bit : 1;
            uint8_t ai_slv_reserve_bit : 6;
        } ai_slv_bit;
    } ai_slv;
    /*work state*/
    union {
        uint8_t work_state_byte;
        struct {
            uint8_t pi1_work_state_bit : 2;
            uint8_t pi2_work_state_bit : 2;
            uint8_t ai1_work_state_bit : 1;
            uint8_t ai2_work_state_bit : 1;
            uint8_t work_state_reserve_bit : 2;
        } work_state_bit;
    } work_state;
    union {
        uint8_t safe_state_byte;
        struct {
            uint8_t output_safe_state_bit : 1;
            uint8_t reply1_safe_state_bit : 1;
            uint8_t reply2_safe_state_bit : 1;
            uint8_t smos1_safe_state_bit : 1;
            uint8_t smos2_safe_state_bit : 1;
            uint8_t smos3_safe_state_bit : 1;
            uint8_t smos4_safe_state_bit : 1;
            uint8_t safe_state_reserve_bit : 1;
        } safe_state_bit;
    } safe_state;
    uint8_t  fault_code2_safe_state;
    uint16_t fault_code2_safe_state_delaytime;
    uint8_t  sai_sample_interval;
    uint8_t  sai_allow_dif[2];  //[0]-full scale Proportion of deviation [1]self data Proportion of deviation
    uint8_t  spi_sample_interval;
    uint8_t  spi_allow_dif[2];
    uint8_t  pi_sqep_sample_interval;
    uint8_t  pi_qep_allow_dif[2];
    uint8_t  sqep_sample_interval;
    uint8_t  sqep_allow_dif[2];
    uint8_t  speed_cross_check;
    uint8_t  speed_allow_dif[2];

    uint8_t  can_master_nodeID;
    uint8_t  can_slave_nodeID_A;
    uint8_t  can_slave_nodeID_B;
    uint16_t can_hb_consumer_gap;
    uint16_t can_hb_producer_gap;
    char     lua_file_ver[LUA_FILE_VER_SIZE];
    uint16_t check_factor;
    uint32_t can_baudrate;
    uint16_t can_pdo_time_gap;
    uint8_t  md5[16];  // the md5 must Place on the tail
} module_param_t;
#pragma pack()
/*************************CLASS*****************************/
class MainWindow;
class param : public QWidget {
    Q_OBJECT
public:
    explicit param(QWidget* mparent, QWidget* parent = nullptr);
    Ui::MainWindow* ui         = nullptr;
    MainWindow*     mainwindow = nullptr;
    int             a_slave_id = 81;
    int             b_slave_id = 82;
    int             master_id  = 1;

public:
    module_param_t module_param;
    void           param_ui_resize(uint32_t width, uint32_t height);
    void           param_ui_clear(void);
    QJsonObject    param_project_info(void);
    bool           param_project_parse(QJsonObject project);
    void           param_ss_set(int id, int state);
    uint8_t        param_ss_get(void);

private:
    void param_write_send_data(void);

private:
    QCheckBox* slv_cb[SLV_NUM];
    QCheckBox* ss_cb[SS_NUM];

    void param_ui_init(void);
    void param_ui_to_data(module_param_t* param);
    void param_display(module_param_t* param);

signals:
private slots:
    void ss_state_changed_slot(int index);
    void pi1_afstate_changed_slot(int index);
    void pi2_afstate_changed_slot(int index);
    void ai1_afstate_changed_slot(int index);
    void ai2_afstate_changed_slot(int index);
};

#endif  // PARAM_H
