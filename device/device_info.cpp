#include "device_info.h"
#include <QStringList>
device_info::device_info() {}

void device_info::table_header_creat(QTableWidget* table, QString device_name)
{
    table->setSpan(0, 0, 1, 2);
    table->setSpan(0, 2, 2, 1);
    table->setSpan(0, 3, 2, 1);
    table->setSpan(0, 4, 1, 2);
    table->setSpan(0, 6, 2, 1);
    QFont font("Arial", 12);
    font.setBold(true);
    QBrush            brush(QColor(210, 230, 255, 128));
    QTableWidgetItem* item_device_name = new QTableWidgetItem(device_name);
    item_device_name->setFont(font);
    item_device_name->setTextAlignment(Qt::AlignCenter);
    item_device_name->setBackground(brush);
    table->setItem(0, 0, item_device_name);
    QTableWidgetItem* item_porttype = new QTableWidgetItem("接口类型");
    item_porttype->setFont(font);
    item_porttype->setTextAlignment(Qt::AlignCenter);
    item_porttype->setBackground(brush);
    table->setItem(1, 0, item_porttype);
    QTableWidgetItem* item_portlabel = new QTableWidgetItem("接口标识");
    item_portlabel->setFont(font);
    item_portlabel->setTextAlignment(Qt::AlignCenter);
    item_portlabel->setBackground(brush);
    table->setItem(1, 1, item_portlabel);
    QTableWidgetItem* item_resource_num = new QTableWidgetItem("资源数量");
    item_resource_num->setFont(font);
    item_resource_num->setTextAlignment(Qt::AlignCenter);
    item_resource_num->setBackground(brush);
    table->setItem(0, 2, item_resource_num);
    QTableWidgetItem* item_description = new QTableWidgetItem("接口说明");
    item_description->setFont(font);
    item_description->setTextAlignment(Qt::AlignCenter);
    item_description->setBackground(brush);
    table->setItem(0, 3, item_description);
    QTableWidgetItem* item_characteristic = new QTableWidgetItem("电气参数");
    item_characteristic->setFont(font);
    item_characteristic->setTextAlignment(Qt::AlignCenter);
    item_characteristic->setBackground(brush);
    table->setItem(0, 4, item_characteristic);
    QTableWidgetItem* item_characteristic_v = new QTableWidgetItem("电压");
    item_characteristic_v->setFont(font);
    item_characteristic_v->setTextAlignment(Qt::AlignCenter);
    item_characteristic_v->setBackground(brush);
    table->setItem(1, 4, item_characteristic_v);
    QTableWidgetItem* item_characteristic_c = new QTableWidgetItem("电流");
    item_characteristic_c->setFont(font);
    item_characteristic_c->setTextAlignment(Qt::AlignCenter);
    item_characteristic_c->setBackground(brush);
    table->setItem(1, 5, item_characteristic_c);
    QTableWidgetItem* item_d = new QTableWidgetItem("电气说明");
    item_d->setFont(font);
    item_d->setTextAlignment(Qt::AlignCenter);
    item_d->setBackground(brush);
    table->setItem(0, 6, item_d);
}

void device_info::spc100_info_table_creat(QTableWidget* table)
{
    table->clearContents();
    table->setFocusPolicy(Qt::NoFocus);
    table->setSelectionMode(QAbstractItemView::NoSelection);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    table->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    QFont font("Arial", 10);
    table->setFont(font);
    table->setColumnCount(7);
    table->setRowCount(13);
    table->setSpan(2, 0, 2, 1);
    table->setSpan(6, 0, 2, 1);
    table->setSpan(9, 0, 2, 1);
    table_header_creat(table, "SPC100");
    /* 列1 */
    QStringList porttype;
    porttype << "24VDC"
             << ""
             << "CAN"
             << "AI"
             << "DI"
             << ""
             << "Relay"
             << "MOS"
             << ""
             << "Encode"
             << "Pulse";
    for (int i = 0; i < porttype.size(); i++) {
        if (porttype[i] != "") {
            QTableWidgetItem* item = new QTableWidgetItem(porttype[i]);
            item->setTextAlignment(Qt::AlignCenter);
            table->setItem(i + 2, 0, item);
        }
    }
    /* 列2 */
    QStringList port_label;
    port_label << "A1"
               << "A2"
               << "CHA/B.H,\r\nCHA/B.L"
               << "CHA/B.1-,\r\nCHA/B.1+,\r\nCHA/B.2-,\r\nCHA/B.2+"
               << "CHA/B.1,\r\nCHA/B.2,\r\nCHA/B.3,\r\nCHA/B.4,\r\nCHA/B.5,\r\nCHA/B.7,\r\nCHA/B.8"
               << "CHA/B.6+,\r\nCHA/B.6-"
               << "CHA/B.1-,\r\nCHA/B.1+,\r\nCHA/B.2-,\r\nCHA/B.2+"
               << "CHA/B.G"
               << "CHA/B.1,\r\nCHA/B.2,\r\nCHA/B.3,\r\nCHA/B.4,"
               << "EN1/2.A+,\r\nEN1/2.A-,\r\nEN1/2.B+,\r\nEN1/2.B-"
               << "CHA/B.1+,\r\nCHA/B.1-,\r\nCHA/B.2+,\r\nCHA/B.2-";
    for (int i = 0; i < port_label.size(); i++) {
        QTableWidgetItem* item = new QTableWidgetItem(port_label[i]);
        table->setItem(i + 2, 1, item);
    }
    /* 列3 */
    QStringList resource_num;
    resource_num << "2"
                 << "1"
                 << "1*2"
                 << "2*2"
                 << "7*2"
                 << "1*2"
                 << "2*2"
                 << "4"
                 << "2*2"
                 << "1*2"
                 << "2*2";
    for (int i = 0; i < resource_num.size(); i++) {
        QTableWidgetItem* item = new QTableWidgetItem(resource_num[i]);
        table->setItem(i + 2, 2, item);
    }
    /* 列4 */
    QStringList description;
    description
        << "供电电源正接线端"
        << "供电电源接地端子(工作地)"
        << "-- A/B通道各提供1路CAN通信接口\r\n-- 支持安全协议开发"
        << "-- A/B通道各提供2路模拟量输入接口\r\n-- 差分输入方式\r\n-- 非隔离接口"
        << "-- A/B通道各提供7路数字量输入接口\r\n-- 接口采用光耦隔离"
        << "-- A/B通道各提供1路数字量差分输入接口\r\n-- 接口采用光耦隔离\r\n-- 支持差分可实现完全电气隔离"
        << "-- A/B通道各提供2路继电器输出接口\r\n-- 支持A/B两路拆分作为非安全应用"
        << "-- 提供4个工作地接线端子"
        << "-- A/B通道各提供2路MOS管输出接口\r\n-- 支持A/B两路拆分作为非安全应用"
        << "-- A/B通道各提供1路编码器接口\r\n-- 支持A/B两路拆分作为非安全应用\r\n-- 支持旋转编码器，编码器差分接口"
        << "-- A/B通道各提供2路脉冲输入接口\r\n-- 支持通道1和2组合使用作为正交编码器接口\r\n-- "
           "支持旋转编码器，编码器差分接口";
    for (int i = 0; i < description.size(); i++) {
        QTableWidgetItem* item = new QTableWidgetItem(description[i]);
        table->setItem(i + 2, 3, item);
    }
    /* 列5 */
    QStringList characteristic;
    characteristic << "18VDC-28VDC"
                   << "-"
                   << "5VDC"
                   << "<30VDC"
                   << "3.3VDC-30VDC"
                   << "20VDC-30VDC"
                   << "24VDC(accroding to power supply)"
                   << "-"
                   << "24VDC(accroding to power supply)"
                   << "3.3V or 5VDC"
                   << "5VDC";
    for (int i = 0; i < characteristic.size(); i++) {
        QTableWidgetItem* item = new QTableWidgetItem(characteristic[i]);
        table->setItem(i + 2, 4, item);
    }
    QStringList characteristic_v;
    characteristic_v << "-"
                     << "-"
                     << "-"
                     << "<1mA@30VDC"
                     << "<10mA@24VDC"
                     << "<10mA@24VDC"
                     << "<5A@250VAC or <3A@30VDC"
                     << "-"
                     << "<400mA@24VDC(per channel)"
                     << "<1mA@5VDC"
                     << "<10mA@24VDC";
    for (int i = 0; i < characteristic_v.size(); i++) {
        QTableWidgetItem* item = new QTableWidgetItem(characteristic_v[i]);
        table->setItem(i + 2, 5, item);
    }
    QStringList characteristic_d;
    characteristic_d << "24V供电"
                     << "-"
                     << "最大通信速率：1Mbps"
                     << "-"
                     << "反接保护"
                     << "反接保护"
                     << "触点材料：AgNi\r\n接触电阻：＜100mΩ"
                     << "-"
                     << "-"
                     << "Freq≤200KHz"
                     << "Freq≤200KHz";
    for (int i = 0; i < characteristic_d.size(); i++) {
        QTableWidgetItem* item = new QTableWidgetItem(characteristic_d[i]);
        table->setItem(i + 2, 6, item);
    }
}
