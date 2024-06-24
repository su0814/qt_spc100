#include "device_fault.h"
#include "ui_device_fault.h"
#include <QDebug>
#include <QPainter>
#include <QStyledItemDelegate>
class faultItemDelegate : public QStyledItemDelegate {
public:
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override
    {
        QStyledItemDelegate::paint(painter, option, index);
        QVariant data = index.data(Qt::UserRole);
        QRect    aRect(5, option.rect.center().y() - 6, 12, 12);
        QRect    atRect(7, option.rect.center().y() - 6, 10, 10);
        QRect    bRect(25, option.rect.center().y() - 6, 12, 12);
        QRect    btRect(27, option.rect.center().y() - 6, 10, 10);

        if (data.toString().contains(QChar('A'))) {
            painter->setBrush(QColor(255, 0, 0));
            painter->drawEllipse(aRect);
        } else {
            painter->setBrush(QColor(128, 128, 128));
            painter->drawEllipse(aRect);
        }
        painter->drawText(atRect, "A");
        if (data.toString().contains(QChar('B'))) {
            painter->setBrush(QColor(255, 0, 0));
            painter->drawEllipse(bRect);
        } else {
            painter->setBrush(QColor(128, 128, 128));
            painter->drawEllipse(bRect);
        }
        painter->drawText(btRect, "B");
    }
};
device_fault::device_fault(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::device_fault)
{
    ui->setupUi(this);
    setWindowTitle("设备故障信息");
    ui->treeWidget_device_fault->setIconSize(QSize(20, 20));
    ui->treeWidget_device_fault->setItemDelegate(new faultItemDelegate);
    ui->treeWidget_device_fault->setIndentation(40);
    self_init();
}

device_fault::~device_fault()
{
    delete ui;
}

void device_fault::self_init()
{
    /* power error */
    QStringList power_error_str;
    power_error_str << "故障：24V过压(0001)"
                    << "故障：24V欠压(0002)"
                    << "故障：MCUA与MCUB之间24V电压差异大(0003)"
                    << "故障：5V过压(0004)"
                    << "故障：5V欠压(0005)"
                    << "故障：MCUA与MCUB之间5V电压差异大(0006)"
                    << "故障：3.3V过压(0007)"
                    << "故障：3.3V欠压(0008)"
                    << "故障：未检测到3.3V电源(0009)"
                    << "故障：24V过流(000A)"
                    << "故障：MCUA与MCUB之间24V电流差异大(000B)";
    for (int i = 0; i < power_error_str.size(); i++) {
        QTreeWidgetItem* item = new QTreeWidgetItem(ui->treeWidget_device_fault);
        item->setText(0, power_error_str[i]);
        item->setIcon(0, QIcon(":/new/photo/photo/fault_error.png"));
        item->setHidden(true);
        power_error_items.append(item);
    }
    /* element */
    QStringList element_error_str;
    element_error_str << "故障：Fram自检失败(0101)"
                      << "故障：Flash自检失败(0102)"
                      << "故障：SDram自检失败(0103)"
                      << "故障：寄存器自检失败(0104)"
                      << "故障：时钟自检失败(0105)"
                      << "故障：计数器自检失败(0106)"
                      << "故障：堆栈溢出(0107)"
                      << "故障：Ram自检失败(0108)";
    for (int i = 0; i < element_error_str.size(); i++) {
        QTreeWidgetItem* item = new QTreeWidgetItem(ui->treeWidget_device_fault);
        item->setText(0, element_error_str[i]);
        item->setIcon(0, QIcon(":/new/photo/photo/fault_error.png"));
        item->setHidden(true);
        element_error_items.append(item);
    }
    /* version */
    QStringList version_error_str;
    version_error_str << "故障：MCUA与MCUB的固件版本不一致(0201)"
                      << "故障：MCUA与MCUB的BOOTLOADER版本不一致(0202)"
                      << "故障：MCUA与MCUB的参数不一致(0203)"
                      << "故障：MCUA与MCUB的工程不一致(0204)"
                      << "故障：设备内无有效工程(0205)";
    for (int i = 0; i < version_error_str.size(); i++) {
        QTreeWidgetItem* item = new QTreeWidgetItem(ui->treeWidget_device_fault);
        item->setText(0, version_error_str[i]);
        item->setIcon(0, QIcon(":/new/photo/photo/fault_error.png"));
        item->setHidden(true);
        version_error_items.append(item);
    }
    /* comm */
    QStringList comm_error_str;
    comm_error_str << "故障：内部同步通讯断联(0301)"
                   << "故障：CAN通讯断联(0302)"
                   << "故障：用户代码运行状态不一致(0303)";
    for (int i = 0; i < comm_error_str.size(); i++) {
        QTreeWidgetItem* item = new QTreeWidgetItem(ui->treeWidget_device_fault);
        item->setText(0, comm_error_str[i]);
        item->setIcon(0, QIcon(":/new/photo/photo/fault_error.png"));
        item->setHidden(true);
        comm_error_items.append(item);
    }
    QStringList output_error_str;
    output_error_str << "警告：MOS1输出状态与实际状态不一致(1001)"
                     << "警告：MOS2输出状态与实际状态不一致(1002)"
                     << "警告：MOS3输出状态与实际状态不一致(1003)"
                     << "警告：MOS4输出状态与实际状态不一致(1004)"
                     << "警告：Relay1输出状态与实际状态不一致(1005)"
                     << "警告：Relay2输出状态与实际状态不一致(1006)";
    for (int i = 0; i < output_error_str.size(); i++) {
        QTreeWidgetItem* item = new QTreeWidgetItem(ui->treeWidget_device_fault);
        item->setText(0, output_error_str[i]);
        item->setIcon(0, QIcon(":/new/photo/photo/fault_warning.png"));
        item->setHidden(true);
        output_warning_items.append(item);
    }
    QStringList input_error_str;
    input_error_str << "警告：MCUA与MCUB的DI1状态不一致(1101)"
                    << "警告：MCUA与MCUB的DI2状态不一致(1102)"
                    << "警告：MCUA与MCUB的DI3状态不一致(1103)"
                    << "警告：MCUA与MCUB的DI4状态不一致(1104)"
                    << "警告：MCUA与MCUB的DI5状态不一致(1105)"
                    << "警告：MCUA与MCUB的DI6状态不一致(1106)"
                    << "警告：MCUA与MCUB的DI7状态不一致(1107)"
                    << "警告：MCUA与MCUB的DI8状态不一致(1108)"
                    << "警告：MCUA与MCUB的DI9状态不一致(1109)"
                    << "警告：MCUA与MCUB的DI10状态不一致(110A)"
                    << "警告：MCUA与MCUB的DI11状态不一致(110B)"
                    << "警告：MCUA与MCUB的DI12状态不一致(110C)"
                    << "警告：AI1过压(1201)"
                    << "警告：AI2过压(1202)"
                    << "警告：MCUA与MCUB的AI1数据差异大(1203)"
                    << "警告：MCUA与MCUB的AI2数据差异大(1204)"
                    << "警告：MCUA与MCUB的QEP1数据差异大(1301)"
                    << "警告：MCUA与MCUB的QEP2数据差异大(1302)"
                    << "警告：MCUA与MCUB的PIQEP1数据差异大(1303)"
                    << "警告：MCUA与MCUB的PIQEP2数据差异大(1304)"
                    << "警告：PIQEP缺相(1305)"
                    << "警告：QEP1与QEP2之间数据差异大(1306)"
                    << "警告：PIQEP1与PIQEP2之间数据差异大(1307)"
                    << "警告：MCUA与MCUB的PI1数据差异大(1401)"
                    << "警告：MCUA与MCUB的PI2数据差异大(1402)";
    for (int i = 0; i < input_error_str.size(); i++) {
        QTreeWidgetItem* item = new QTreeWidgetItem(ui->treeWidget_device_fault);
        item->setText(0, input_error_str[i]);
        item->setIcon(0, QIcon(":/new/photo/photo/fault_warning.png"));
        item->setHidden(true);
        input_warning_items.append(item);
    }
}

void device_fault::set_module_fault(module_fault_t a_module_fault, module_fault_t b_module_fault)
{
    bool error = false, warning = false;
    /* power error */
    for (int i = 0; i < power_error_items.size(); i++) {
        QString data = "";
        if (((a_module_fault.module_error.power_error_state.power_error_byte >> i) & 0x01) != 0) {
            data += "A";
        }
        if (((b_module_fault.module_error.power_error_state.power_error_byte >> i) & 0x01) != 0) {
            data += "B";
        }
        if (data.isEmpty()) {
            power_error_items[i]->setHidden(true);
        } else {
            power_error_items[i]->setHidden(false);
            error = true;
        }
        power_error_items[i]->setData(0, Qt::UserRole, data);
    }
    /* element */
    for (int i = 0; i < element_error_items.size(); i++) {
        QString data = "";
        if (((a_module_fault.module_error.element_error_state.element_error_byte >> i) & 0x01) != 0) {
            data += "A";
        }
        if (((b_module_fault.module_error.element_error_state.element_error_byte >> i) & 0x01) != 0) {
            data += "B";
        }
        if (data.isEmpty()) {
            element_error_items[i]->setHidden(true);
        } else {
            element_error_items[i]->setHidden(false);
            error = true;
        }
        element_error_items[i]->setData(0, Qt::UserRole, data);
    }
    /* version */
    for (int i = 0; i < version_error_items.size(); i++) {
        QString data = "";
        if (((a_module_fault.module_error.version_error_state.version_error_byte >> i) & 0x01) != 0) {
            data += "A";
        }
        if (((b_module_fault.module_error.version_error_state.version_error_byte >> i) & 0x01) != 0) {
            data += "B";
        }
        if (data.isEmpty()) {
            version_error_items[i]->setHidden(true);
        } else {
            version_error_items[i]->setHidden(false);
            error = true;
        }
        version_error_items[i]->setData(0, Qt::UserRole, data);
    }
    /* comm */
    for (int i = 0; i < comm_error_items.size(); i++) {
        QString data = "";
        if (((a_module_fault.module_error.comm_error_state.comm_error_byte >> i) & 0x01) != 0) {
            data += "A";
        }
        if (((b_module_fault.module_error.comm_error_state.comm_error_byte >> i) & 0x01) != 0) {
            data += "B";
        }
        if (data.isEmpty()) {
            comm_error_items[i]->setHidden(true);
        } else {
            comm_error_items[i]->setHidden(false);
            error = true;
        }
        comm_error_items[i]->setData(0, Qt::UserRole, data);
    }

    /* output */
    for (int i = 0; i < output_warning_items.size(); i++) {
        QString data = "";
        if (((a_module_fault.module_warning.output_error_state.output_error_byte >> i) & 0x01) != 0) {
            data += "A";
        }
        if (((b_module_fault.module_warning.output_error_state.output_error_byte >> i) & 0x01) != 0) {
            data += "B";
        }
        if (data.isEmpty()) {
            output_warning_items[i]->setHidden(true);
        } else {
            output_warning_items[i]->setHidden(false);
            warning = true;
        }
        output_warning_items[i]->setData(0, Qt::UserRole, data);
    }

    /* input */
    for (int i = 0; i < input_warning_items.size(); i++) {
        QString data = "";
        if (((a_module_fault.module_warning.input_error_state.input_error_byte >> i) & 0x01) != 0) {
            data += "A";
        }
        if (((b_module_fault.module_warning.input_error_state.input_error_byte >> i) & 0x01) != 0) {
            data += "B";
        }
        if (data.isEmpty()) {
            input_warning_items[i]->setHidden(true);
        } else {
            input_warning_items[i]->setHidden(false);
            warning = true;
        }
        input_warning_items[i]->setData(0, Qt::UserRole, data);
    }
    send_fault_state(error, warning);
}
