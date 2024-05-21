#ifndef DEVICE_FAULT_H
#define DEVICE_FAULT_H

#include "QTreeWidget"
#include "status.h"
#include <QDialog>
typedef enum {
    /* 电源错误 */
    FAULT_ERROR_POWER = 0X0000,
    FAULT_ERROR_POWER_24V_OVER,       // 24V过压
    FAULT_ERROR_POWER_24V_LOW,        // 24V欠压
    FAULT_ERROR_POWER_24V_DIF,        // 24V电压 偏差大
    FAULT_ERROR_POWER_5V_OVER,        // 5V过压
    FAULT_ERROR_POWER_5V_LOW,         // 5V欠压
    FAULT_ERROR_POWER_5V_DIF,         // 5V电压 偏差大
    FAULT_ERROR_POWER_PAIR_3V3_OVER,  // PAIR 3.3V过压
    FAULT_ERROR_POWER_PAIR_3V3_LOW,   // PAIR 3.3V欠压
    FAULT_ERROR_POWER_PAIR_3V3_OFF,   // PAIR 3.3V断电
    FAULT_ERROR_POWER_24A_OVER,       // 24V电流过大
    FAULT_ERROR_POWER_24A_DIF,        // 24V电流 偏差大

    /* 元件错误 */
    FAULT_ERROR_ELEMENT = 0X0100,
    FAULT_ERROR_ELEMENT_FRAM,     // FRAM自检失败
    FAULT_ERROR_ELEMENT_FLASH,    // FLASH自检失败
    FAULT_ERROR_ELEMENT_SDRAM,    // SDRAM自检失败
    FAULT_ERROR_ELEMENT_CPU,      // 寄存器自检失败
    FAULT_ERROR_ELEMENT_CLOCK,    // 时钟自检失败
    FAULT_ERROR_ELEMENT_COUNTER,  // 计数器自检失败
    FAULT_ERROR_ELEMENT_STACK,    // 堆栈溢出
    FAULT_ERROR_ELEMENT_RAM,      // RAM自检失败

    /* 版本错误 */
    FAULT_ERROR_VERSION = 0X0200,
    FAULT_ERROR_VERSION_FIRMWARE,
    FAULT_ERROR_VERSION_BOOTLOADER,
    FAULT_ERROR_VERSION_PARAM,
    FAULT_ERROR_VERSION_PROJECT,
    FAULT_ERROR_VERSION_PROJECT_INVALID,

    /* 通讯错误 */
    FAULT_ERROR_COMM = 0X0300,
    FAULT_ERROR_COMM_SYNC,
    FAULT_ERROR_COMM_CAN,

    /* 输出警告 */
    FAULT_WARNING_OUTPUT = 0X1000,
    FAULT_WARNING_OUTPUT_MOS1,
    FAULT_WARNING_OUTPUT_MOS2,
    FAULT_WARNING_OUTPUT_MOS3,
    FAULT_WARNING_OUTPUT_MOS4,
    FAULT_WARNING_OUTPUT_RELAY1,
    FAULT_WARNING_OUTPUT_RELAY2,

    /* 输入警告 */
    FAULT_WARNING_INPUT = 0X1100,
    FAULT_WARNING_INPUT_DI1_DIF,
    FAULT_WARNING_INPUT_DI2_DIF,
    FAULT_WARNING_INPUT_DI3_DIF,
    FAULT_WARNING_INPUT_DI4_DIF,
    FAULT_WARNING_INPUT_DI5_DIF,
    FAULT_WARNING_INPUT_DI6_DIF,
    FAULT_WARNING_INPUT_DI7_DIF,
    FAULT_WARNING_INPUT_DI8_DIF,
    FAULT_WARNING_INPUT_DI9_DIF,
    FAULT_WARNING_INPUT_DI10_DIF,
    FAULT_WARNING_INPUT_DI11_DIF,
    FAULT_WARNING_INPUT_DI12_DIF,
    // AI错误
    FAULT_WARNING_INPUT_AI = 0X1200,
    FAULT_WARNING_INPUT_AI1_OVER,
    FAULT_WARNING_INPUT_AI2_OVER,
    FAULT_WARNING_INPUT_AI1_DIF,
    FAULT_WARNING_INPUT_AI2_DIF,

    // QEP错误
    FAULT_WARNING_INPUT_QEP = 0X1300,
    FAULT_WARNING_INPUT_QEP1_DIF,
    FAULT_WARNING_INPUT_QEP2_DIF,
    FAULT_WARNING_INPUT_PIQEP1_DIF,
    FAULT_WARNING_INPUT_PIQEP2_DIF,
    FAULT_WARNING_INPUT_PIQEP2_LP,
    FAULT_WARNING_INPUT_QEP12_DIF,
    FAULT_WARNING_INPUT_PIQEP12_DIF,
    // PI错误
    FAULT_WARNING_INPUT_PI = 0x1400,
    FAULT_WARNING_INPUT_PI1_DIF,
    FAULT_WARNING_INPUT_PI2_DIF,
} error_code_e;

namespace Ui {
class device_fault;
}

class device_fault : public QDialog {
    Q_OBJECT

public:
    explicit device_fault(QWidget* parent = 0);
    ~device_fault();

public:
    void set_module_fault(module_fault_t a_module_fault, module_fault_t b_module_fault);

private:
    void self_init(void);

private:
    QList<QTreeWidgetItem*> power_error_items;
    QList<QTreeWidgetItem*> element_error_items;
    QList<QTreeWidgetItem*> version_error_items;
    QList<QTreeWidgetItem*> comm_error_items;
    QList<QTreeWidgetItem*> output_warning_items;
    QList<QTreeWidgetItem*> input_warning_items;

private:
    Ui::device_fault* ui;
signals:
    void send_fault_state(bool error, bool warning);
};

#endif  // DEVICE_FAULT_H
