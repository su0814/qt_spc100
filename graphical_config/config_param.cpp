#include "config_param.h"
#include "mainwindow.h"
#include <QBrush>
#include <QColor>
#include <QDebug>
#include <QFormLayout>
#include <QGraphicsSceneMouseEvent>
#include <QLayout>
config_param::config_param(int x, int y, int w, int h, config_tool_e type, QWidget* mparent, module_param_t* param,
    QGraphicsItem* parent)
    : QGraphicsRectItem(x, y, w, h, parent)
{
    ui = MainWindow::my_ui->ui;
    mainwindow = (MainWindow*)mparent;
    module_param = param;
    tool_type = type;
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemIsFocusable, true);
    QBrush brush(QColor(128, 128, 128, 128)); //块填充色
    setBrush(brush);
}

/**
 * @brief 获取当前配置快的类型
 * @return
 */
config_tool_e config_param::get_type()
{
    return tool_type;
}

void config_param::sys_param_config_dialog()
{
    QDialog dialog;
    dialog.setWindowTitle("其他参数");
    QFormLayout* layout = new QFormLayout(&dialog);
    QComboBox fault2ss;
    fault2ss.addItem("不启用");
    fault2ss.addItem("启用");
    fault2ss.setCurrentIndex(module_param->fault_code2_safe_state);
    QSpinBox delaytime;
    delaytime.setMinimum(0);
    delaytime.setMaximum(30000);
    delaytime.setValue(module_param->fault_code2_safe_state_delaytime);
    QSpinBox cross_time;
    cross_time.setMinimum(0);
    cross_time.setMaximum(200);
    cross_time.setValue(module_param->check_factor);
    layout->addRow("硬件错误进入安全状态", &fault2ss);
    layout->addRow("硬件错误进入安全状态延迟时间", &delaytime);
    layout->addRow("设备互检间隔", &cross_time);
    QPushButton* okButton = new QPushButton("确认", &dialog);
    QPushButton* cancelButton = new QPushButton("取消", &dialog);
    QHBoxLayout buttonLayout;
    buttonLayout.addWidget(cancelButton);
    buttonLayout.addWidget(okButton);
    layout->addRow(&buttonLayout);
    QObject::connect(okButton, &QPushButton::clicked, [&]() {
        module_param->fault_code2_safe_state = fault2ss.currentIndex();
        module_param->fault_code2_safe_state_delaytime = delaytime.value();
        module_param->check_factor = cross_time.value();
        dialog.close();
    });
    QObject::connect(cancelButton, &QPushButton::clicked, [&]() { dialog.close(); });
    dialog.exec();
}

/**
 * @brief 关于CAN配置参数的弹窗
 */
void config_param::can_param_config_dialog()
{
    QDialog dialog;
    dialog.setWindowTitle("CANopen参数");
    QFormLayout* layout = new QFormLayout(&dialog);
    QComboBox can_baudrate;
    QSpinBox a_nodeid;
    QSpinBox b_nodeid;
    QSpinBox master_nodeid;
    QSpinBox master_heartbeat;
    QSpinBox slave_heartbeat;
    QSpinBox pdo_tim_gap;
    can_baudrate.addItem("250000");
    can_baudrate.addItem("500000");
    can_baudrate.addItem("1000000");
    can_baudrate.setCurrentText(QString::number(module_param->can_baudrate));
    a_nodeid.setMinimum(1);
    a_nodeid.setMaximum(127);
    a_nodeid.setValue(module_param->can_slave_nodeID_A);
    b_nodeid.setMinimum(1);
    b_nodeid.setMaximum(127);
    b_nodeid.setValue(module_param->can_slave_nodeID_B);
    master_nodeid.setMinimum(1);
    master_nodeid.setMaximum(127);
    master_nodeid.setValue(module_param->can_master_nodeID);
    master_heartbeat.setMinimum(0);
    master_heartbeat.setMaximum(30000);
    master_heartbeat.setValue(module_param->can_hb_consumer_gap);
    slave_heartbeat.setMinimum(0);
    slave_heartbeat.setMaximum(30000);
    slave_heartbeat.setValue(module_param->can_hb_producer_gap);
    pdo_tim_gap.setMinimum(0);
    pdo_tim_gap.setMaximum(30000);
    pdo_tim_gap.setValue(module_param->can_pdo_time_gap);
    QObject::connect(&a_nodeid, &QSpinBox::editingFinished, [&]() {
        int arg1 = a_nodeid.value();
        if (arg1 == b_nodeid.value() || arg1 == master_nodeid.value()) {
            a_nodeid.setValue(module_param->can_slave_nodeID_A);
        }
    });
    QObject::connect(&b_nodeid, &QSpinBox::editingFinished, [&]() {
        int arg1 = b_nodeid.value();
        if (arg1 == a_nodeid.value() || arg1 == master_nodeid.value()) {
            b_nodeid.setValue(module_param->can_slave_nodeID_B);
        }
    });
    QObject::connect(&master_nodeid, &QSpinBox::editingFinished, [&]() {
        int arg1 = master_nodeid.value();
        if (arg1 == b_nodeid.value() || arg1 == a_nodeid.value()) {
            master_nodeid.setValue(module_param->can_master_nodeID);
        }
    });

    QObject::connect(&master_heartbeat, &QSpinBox::editingFinished, [&]() {
        int arg1 = master_heartbeat.value();
        if (arg1 > 0 && arg1 < 100) {
            master_heartbeat.setValue(100);
        }
    });
    QObject::connect(&slave_heartbeat, &QSpinBox::editingFinished, [&]() {
        int arg1 = slave_heartbeat.value();
        if (arg1 > 0 && arg1 < 100) {
            slave_heartbeat.setValue(100);
        }
    });
    QObject::connect(&pdo_tim_gap, &QSpinBox::editingFinished, [&]() {
        int arg1 = pdo_tim_gap.value();
        if (arg1 > 0 && arg1 < 100) {
            pdo_tim_gap.setValue(100);
        }
    });

    layout->addRow("波特率", &can_baudrate);
    layout->addRow("从机A节点ID", &a_nodeid);
    layout->addRow("从机B节点ID", &b_nodeid);
    layout->addRow("主机节点ID", &master_nodeid);
    layout->addRow("主机心跳间隔", &master_heartbeat);
    layout->addRow("从机心跳间隔", &slave_heartbeat);
    layout->addRow("PDO数据发送间隔", &pdo_tim_gap);
    QPushButton* okButton = new QPushButton("确认", &dialog);
    QPushButton* cancelButton = new QPushButton("取消", &dialog);
    QHBoxLayout buttonLayout;
    buttonLayout.addWidget(cancelButton);
    buttonLayout.addWidget(okButton);
    layout->addRow(&buttonLayout);
    QObject::connect(okButton, &QPushButton::clicked, [&]() {
        module_param->can_baudrate = can_baudrate.currentText().toInt();
        module_param->can_slave_nodeID_A = a_nodeid.value();
        module_param->can_slave_nodeID_B = b_nodeid.value();
        module_param->can_master_nodeID = master_nodeid.value();
        module_param->can_hb_consumer_gap = master_heartbeat.value();
        module_param->can_hb_producer_gap = slave_heartbeat.value();
        module_param->can_pdo_time_gap = pdo_tim_gap.value();
        dialog.close();
    });
    QObject::connect(cancelButton, &QPushButton::clicked, [&]() { dialog.close(); });
    dialog.exec();
}

void config_param::ai_param_config_dialog()
{
    QDialog dialog;
    dialog.setWindowTitle("AI参数");
    QFormLayout* layout = new QFormLayout(&dialog);
    QComboBox ai1_af_combo;
    ai1_af_combo.addItem("AI");
    ai1_af_combo.addItem("DI");
    ai1_af_combo.setCurrentIndex(module_param->work_state.work_state_bit.ai1_work_state_bit);
    QComboBox ai2_af_combo;
    ai2_af_combo.addItem("AI");
    ai2_af_combo.addItem("DI");
    ai2_af_combo.setCurrentIndex(module_param->work_state.work_state_bit.ai2_work_state_bit);
    connect(&ai1_af_combo, SIGNAL(currentIndexChanged(int)), this, SLOT(ai1_afstate_changed_slot(int)));
    connect(&ai2_af_combo, SIGNAL(currentIndexChanged(int)), this, SLOT(ai2_afstate_changed_slot(int)));
    QSpinBox sampled_interval;
    sampled_interval.setMinimum(10);
    sampled_interval.setMaximum(200);
    sampled_interval.setValue(module_param->sai_sample_interval);
    QSpinBox ai_full;
    ai_full.setMinimum(1);
    ai_full.setMaximum(100);
    ai_full.setValue(module_param->sai_allow_dif[0]);
    QSpinBox ai_actual;
    ai_actual.setMinimum(1);
    ai_actual.setMaximum(100);
    ai_actual.setValue(module_param->sai_allow_dif[1]);
    QComboBox ai1_cat;
    ai1_cat.addItem("CAT2");
    ai1_cat.addItem("CAT3");
    ai1_cat.setCurrentIndex(module_param->ai_slv.ai_slv_bit.ai1_slv_bit);
    QComboBox ai2_cat;
    ai2_cat.addItem("CAT2");
    ai2_cat.addItem("CAT3");
    ai2_cat.setCurrentIndex(module_param->ai_slv.ai_slv_bit.ai2_slv_bit);
    layout->addRow("采样间隔", &sampled_interval);
    layout->addRow("AI1复用", &ai1_af_combo);
    layout->addRow("AI2复用", &ai2_af_combo);
    layout->addRow("AI1架构等级", &ai1_cat);
    layout->addRow("AI2架构等级", &ai2_cat);
    layout->addRow("互检满量程占比", &ai_full);
    layout->addRow("互检实时数据占比", &ai_actual);
    QPushButton* okButton = new QPushButton("确认", &dialog);
    QPushButton* cancelButton = new QPushButton("取消", &dialog);
    QHBoxLayout buttonLayout;
    buttonLayout.addWidget(cancelButton);
    buttonLayout.addWidget(okButton);
    layout->addRow(&buttonLayout);
    QObject::connect(okButton, &QPushButton::clicked, [&]() {
        module_param->ai_slv.ai_slv_bit.ai1_slv_bit = ai1_cat.currentIndex();
        module_param->ai_slv.ai_slv_bit.ai2_slv_bit = ai2_cat.currentIndex();
        module_param->sai_sample_interval = sampled_interval.value();
        module_param->sai_allow_dif[0] = ai_full.value();
        module_param->sai_allow_dif[1] = ai_actual.value();
        module_param->work_state.work_state_bit.ai1_work_state_bit = ai1_af_combo.currentIndex();
        module_param->work_state.work_state_bit.ai2_work_state_bit = ai2_af_combo.currentIndex();
        dialog.close();
    });
    QObject::connect(cancelButton, &QPushButton::clicked, [&]() { dialog.close(); });
    dialog.exec();
}

void config_param::di_param_config_dialog()
{
    QDialog dialog;
    dialog.setWindowTitle("DI参数");
    QFormLayout* layout = new QFormLayout(&dialog);
    QStringList items;
    items << "CAT2"
          << "CAT3";
    QComboBox di1_cat;
    di1_cat.addItems(items);
    di1_cat.setCurrentIndex(module_param->di_slv.di_slv_bit.di1_slv_bit);
    QComboBox di2_cat;
    di2_cat.addItems(items);
    di2_cat.setCurrentIndex(module_param->di_slv.di_slv_bit.di2_slv_bit);
    QComboBox di3_cat;
    di3_cat.addItems(items);
    di3_cat.setCurrentIndex(module_param->di_slv.di_slv_bit.di3_slv_bit);
    QComboBox di4_cat;
    di4_cat.addItems(items);
    di4_cat.setCurrentIndex(module_param->di_slv.di_slv_bit.di4_slv_bit);
    QComboBox di5_cat;
    di5_cat.addItems(items);
    di5_cat.setCurrentIndex(module_param->di_slv.di_slv_bit.di5_slv_bit);
    QComboBox di6_cat;
    di6_cat.addItems(items);
    di6_cat.setCurrentIndex(module_param->di_slv.di_slv_bit.di6_slv_bit);
    QComboBox di7_cat;
    di7_cat.addItems(items);
    di7_cat.setCurrentIndex(module_param->di_slv.di_slv_bit.di7_slv_bit);
    QComboBox di8_cat;
    di8_cat.addItems(items);
    di8_cat.setCurrentIndex(module_param->di_slv.di_slv_bit.di8_slv_bit);
    layout->addRow("DI1架构等级", &di1_cat);
    layout->addRow("DI2架构等级", &di2_cat);
    layout->addRow("DI3架构等级", &di3_cat);
    layout->addRow("DI4架构等级", &di4_cat);
    layout->addRow("DI5架构等级", &di5_cat);
    layout->addRow("DI6架构等级", &di6_cat);
    layout->addRow("DI7架构等级", &di7_cat);
    layout->addRow("DI8架构等级", &di8_cat);
    QPushButton* okButton = new QPushButton("确认", &dialog);
    QPushButton* cancelButton = new QPushButton("取消", &dialog);
    QHBoxLayout buttonLayout;
    buttonLayout.addWidget(cancelButton);
    buttonLayout.addWidget(okButton);
    layout->addRow(&buttonLayout);
    QObject::connect(okButton, &QPushButton::clicked, [&]() {
        module_param->di_slv.di_slv_bit.di1_slv_bit = di1_cat.currentIndex();
        module_param->di_slv.di_slv_bit.di2_slv_bit = di2_cat.currentIndex();
        module_param->di_slv.di_slv_bit.di3_slv_bit = di3_cat.currentIndex();
        module_param->di_slv.di_slv_bit.di4_slv_bit = di4_cat.currentIndex();
        module_param->di_slv.di_slv_bit.di5_slv_bit = di5_cat.currentIndex();
        module_param->di_slv.di_slv_bit.di6_slv_bit = di6_cat.currentIndex();
        module_param->di_slv.di_slv_bit.di7_slv_bit = di7_cat.currentIndex();
        module_param->di_slv.di_slv_bit.di8_slv_bit = di8_cat.currentIndex();
        dialog.close();
    });
    QObject::connect(cancelButton, &QPushButton::clicked, [&]() { dialog.close(); });
    dialog.exec();
}

void config_param::relay_param_config_dialog()
{
    QDialog dialog;
    dialog.setWindowTitle("继电器输出参数");
    QFormLayout* layout = new QFormLayout(&dialog);
    QStringList items;
    items << "CAT2"
          << "CAT3";
    QComboBox relay1_cat;
    relay1_cat.addItems(items);
    relay1_cat.setCurrentIndex(module_param->relay_slv.relay_slv_bit.relay1_slv_bit);
    QComboBox relay2_cat;
    relay2_cat.addItems(items);
    relay2_cat.setCurrentIndex(module_param->relay_slv.relay_slv_bit.relay2_slv_bit);
    layout->addRow("RELAY1架构等级", &relay1_cat);
    layout->addRow("RELAY2架构等级", &relay2_cat);
    QPushButton* okButton = new QPushButton("确认", &dialog);
    QPushButton* cancelButton = new QPushButton("取消", &dialog);
    QHBoxLayout buttonLayout;
    buttonLayout.addWidget(cancelButton);
    buttonLayout.addWidget(okButton);
    layout->addRow(&buttonLayout);
    QObject::connect(okButton, &QPushButton::clicked, [&]() {
        module_param->relay_slv.relay_slv_bit.relay1_slv_bit = relay1_cat.currentIndex();
        module_param->relay_slv.relay_slv_bit.relay2_slv_bit = relay2_cat.currentIndex();
        dialog.close();
    });
    QObject::connect(cancelButton, &QPushButton::clicked, [&]() { dialog.close(); });
    dialog.exec();
}

void config_param::mos_param_config_dialog()
{
    QDialog dialog;
    dialog.setWindowTitle("默认安全输出模式");
    QFormLayout* layout = new QFormLayout(&dialog);
    QStringList items;
    items << "不相关"
          << "相关";
    QComboBox relay1_relevant;
    relay1_relevant.addItems(items);
    relay1_relevant.setCurrentIndex(module_param->safe_state.safe_state_bit.reply1_safe_state_bit);
    QComboBox relay2_relevant;
    relay2_relevant.addItems(items);
    relay2_relevant.setCurrentIndex(module_param->safe_state.safe_state_bit.reply2_safe_state_bit);
    QComboBox mos1_relevant;
    mos1_relevant.addItems(items);
    mos1_relevant.setCurrentIndex(module_param->safe_state.safe_state_bit.smos1_safe_state_bit);
    QComboBox mos2_relevant;
    mos2_relevant.addItems(items);
    mos2_relevant.setCurrentIndex(module_param->safe_state.safe_state_bit.smos2_safe_state_bit);
    QComboBox mos3_relevant;
    mos3_relevant.addItems(items);
    mos3_relevant.setCurrentIndex(module_param->safe_state.safe_state_bit.smos3_safe_state_bit);
    QComboBox mos4_relevant;
    mos4_relevant.addItems(items);
    mos4_relevant.setCurrentIndex(module_param->safe_state.safe_state_bit.smos4_safe_state_bit);
    layout->addRow("RELAY1", &relay1_relevant);
    layout->addRow("RELAY2", &relay2_relevant);
    layout->addRow("MOS1", &mos1_relevant);
    layout->addRow("MOS2", &mos2_relevant);
    layout->addRow("MOS3", &mos3_relevant);
    layout->addRow("MOS4", &mos4_relevant);
    QPushButton* okButton = new QPushButton("确认", &dialog);
    QPushButton* cancelButton = new QPushButton("取消", &dialog);
    QHBoxLayout buttonLayout;
    buttonLayout.addWidget(cancelButton);
    buttonLayout.addWidget(okButton);
    layout->addRow(&buttonLayout);
    QObject::connect(okButton, &QPushButton::clicked, [&]() {
        module_param->safe_state.safe_state_bit.reply1_safe_state_bit = relay1_relevant.currentIndex();
        module_param->safe_state.safe_state_bit.reply2_safe_state_bit = relay2_relevant.currentIndex();
        module_param->safe_state.safe_state_bit.smos1_safe_state_bit = mos1_relevant.currentIndex();
        module_param->safe_state.safe_state_bit.smos2_safe_state_bit = mos2_relevant.currentIndex();
        module_param->safe_state.safe_state_bit.smos3_safe_state_bit = mos3_relevant.currentIndex();
        module_param->safe_state.safe_state_bit.smos4_safe_state_bit = mos4_relevant.currentIndex();
        mainwindow->condition_view_class->ss_default_set_state(module_param->safe_state.safe_state_byte);
        qDebug() << module_param->safe_state.safe_state_byte;
        dialog.close();
    });
    QObject::connect(cancelButton, &QPushButton::clicked, [&]() { dialog.close(); });
    dialog.exec();
}

void config_param::encode_param_config_dialog()
{
    QDialog dialog;
    dialog.setWindowTitle("ENCODE参数");
    QFormLayout* layout = new QFormLayout(&dialog);
    QSpinBox sample_interval;
    sample_interval.setMinimum(10);
    sample_interval.setMaximum(200);
    sample_interval.setValue(module_param->sqep_sample_interval);
    QSpinBox qep_full;
    qep_full.setMinimum(1);
    qep_full.setMaximum(200);
    qep_full.setValue(module_param->sqep_allow_dif[0]);
    QSpinBox qep_actual;
    qep_actual.setMinimum(1);
    qep_actual.setMaximum(200);
    qep_actual.setValue(module_param->sqep_allow_dif[1]);
    QSpinBox qep12ratio;
    qep12ratio.setMinimum(1);
    qep12ratio.setMaximum(10000);
    qep12ratio.setValue(module_param->qep1_2_ratio);
    QComboBox qep_cross;
    qep_cross.addItem("不启用");
    qep_cross.addItem("启用");
    qep_cross.setCurrentIndex(module_param->qep12_cross_check);
    QSpinBox qep_cross_full;
    qep_cross_full.setMinimum(1);
    qep_cross_full.setMaximum(100);
    qep_cross_full.setValue(module_param->qep_speed_allow_dif[0]);
    QSpinBox qep_cross_actual;
    qep_cross_actual.setMinimum(1);
    qep_cross_actual.setMaximum(100);
    qep_cross_actual.setValue(module_param->qep_speed_allow_dif[1]);
    QPushButton* setButton = new QPushButton("减速曲线设置", &dialog);
    QFrame* line = new QFrame;
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    layout->addRow("采样间隔", &sample_interval);
    layout->addRow("互检满量程占比", &qep_full);
    layout->addRow("互检实时数据占比", &qep_actual);
    layout->addRow(line);
    layout->addRow("QEP1/QEP2速度比", &qep12ratio);
    layout->addRow("QEP1/QEP2互检", &qep_cross);
    layout->addRow("QEP1/QEP2互检满量程占比", &qep_cross_full);
    layout->addRow("QEP1/QEP2互检实时数据占比", &qep_cross_actual);
    layout->addRow(setButton);
    QPushButton* okButton = new QPushButton("确认", &dialog);
    QPushButton* cancelButton = new QPushButton("取消", &dialog);
    QHBoxLayout buttonLayout;
    buttonLayout.addWidget(cancelButton);
    buttonLayout.addWidget(okButton);
    layout->addRow(&buttonLayout);
    QObject::connect(okButton, &QPushButton::clicked, [&]() {
        module_param->sqep_sample_interval = sample_interval.value();
        module_param->sqep_allow_dif[0] = qep_full.value();
        module_param->sqep_allow_dif[1] = qep_actual.value();
        module_param->qep1_2_ratio = qep12ratio.value();
        module_param->qep12_cross_check = qep_cross.currentIndex();
        module_param->qep_speed_allow_dif[0] = qep_cross_full.value();
        module_param->qep_speed_allow_dif[1] = qep_cross_actual.value();
        dialog.close();
    });
    QObject::connect(cancelButton, &QPushButton::clicked, [&]() { dialog.close(); });
    QObject::connect(setButton, &QPushButton::clicked, [&]() { deceleration_curve_config_dialog(); });
    dialog.exec();
}

void config_param::pi_param_config_dialog()
{
    QDialog dialog;
    dialog.setWindowTitle("PI参数");
    QFormLayout* layout = new QFormLayout(&dialog);
    QSpinBox pi_sample_interval;
    pi_sample_interval.setMinimum(10);
    pi_sample_interval.setMaximum(200);
    pi_sample_interval.setValue(module_param->spi_sample_interval);
    QSpinBox piqep_sample_interval;
    piqep_sample_interval.setMinimum(0);
    piqep_sample_interval.setMaximum(200);
    piqep_sample_interval.setValue(module_param->pi_sqep_sample_interval);
    QSpinBox pi_full;
    pi_full.setMinimum(1);
    pi_full.setMaximum(100);
    pi_full.setValue(module_param->spi_allow_dif[0]);
    QSpinBox pi_actual;
    pi_actual.setMinimum(1);
    pi_actual.setMaximum(100);
    pi_actual.setValue(module_param->spi_allow_dif[1]);
    QSpinBox piqep_full;
    piqep_full.setMinimum(1);
    piqep_full.setMaximum(100);
    piqep_full.setValue(module_param->pi_qep_allow_dif[0]);
    QSpinBox piqep_actual;
    piqep_actual.setMinimum(1);
    piqep_actual.setMaximum(100);
    piqep_actual.setValue(module_param->pi_qep_allow_dif[1]);
    QComboBox pi1_af;
    pi1_af.addItem("PI");
    pi1_af.addItem("QEP");
    pi1_af.addItem("DI");
    pi1_af.setCurrentIndex(module_param->work_state.work_state_bit.pi1_work_state_bit);
    pi1_af_combo = &pi1_af;
    connect(&pi1_af, SIGNAL(currentIndexChanged(int)), this, SLOT(pi1_afstate_changed_slot(int)));
    QComboBox pi2_af;
    pi2_af.addItem("PI");
    pi2_af.addItem("QEP");
    pi2_af.addItem("DI");
    pi2_af_combo = &pi2_af;
    connect(&pi2_af, SIGNAL(currentIndexChanged(int)), this, SLOT(pi2_afstate_changed_slot(int)));
    QSpinBox piqep_ratio;
    piqep_ratio.setMinimum(1);
    piqep_ratio.setMaximum(10000);
    piqep_ratio.setValue(module_param->piqep1_2_ratio);
    QComboBox qep_cross;
    qep_cross.addItem("不启用");
    qep_cross.addItem("启用");
    qep_cross.setCurrentIndex(module_param->piqep12_cross_check);
    connect(&qep_cross, SIGNAL(currentIndexChanged(int)), this, SLOT(piqep_crosscheck_slot(int)));
    QSpinBox qep_cross_full;
    qep_cross_full.setMinimum(1);
    qep_cross_full.setMaximum(100);
    qep_cross_full.setValue(module_param->piqep_speed_allow_dif[0]);
    QSpinBox qep_cross_actual;
    qep_cross_actual.setMinimum(1);
    qep_cross_actual.setMaximum(100);
    qep_cross_actual.setValue(module_param->piqep_speed_allow_dif[1]);
    pi2_af.setCurrentIndex(module_param->work_state.work_state_bit.pi2_work_state_bit);
    QPushButton* setButton = new QPushButton("减速曲线设置", &dialog);
    QFrame* line = new QFrame;
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    layout->addRow("PI采样间隔", &pi_sample_interval);
    layout->addRow("PI_QEP采样间隔", &piqep_sample_interval);
    layout->addRow("PI互检满量程占比", &pi_full);
    layout->addRow("PI互检实时数据占比", &pi_actual);
    layout->addRow("PI_QEP互检满量程占比", &piqep_full);
    layout->addRow("PI_QEP互检实时数据占比", &piqep_actual);
    layout->addRow("PI1复用", &pi1_af);
    layout->addRow("PI2复用", &pi2_af);
    layout->addRow(line);
    layout->addRow("PI_QEP1/PI_QEP2速度比", &piqep_ratio);
    layout->addRow("PI_QEP1/PI_QEP2互检", &qep_cross);
    layout->addRow("PI_QEP1/PI_QEP2互检满量程占比", &qep_cross_full);
    layout->addRow("PI_QEP1/PI_QEP2互检实时数据占比", &qep_cross_actual);
    layout->addRow(setButton);
    QPushButton* okButton = new QPushButton("确认", &dialog);
    QPushButton* cancelButton = new QPushButton("取消", &dialog);
    QHBoxLayout buttonLayout;
    buttonLayout.addWidget(cancelButton);
    buttonLayout.addWidget(okButton);
    layout->addRow(&buttonLayout);
    QObject::connect(okButton, &QPushButton::clicked, [&]() {
        module_param->spi_sample_interval = pi_sample_interval.value();
        module_param->pi_sqep_sample_interval = piqep_sample_interval.value();
        module_param->spi_allow_dif[0] = pi_full.value();
        module_param->spi_allow_dif[1] = pi_actual.value();
        module_param->pi_qep_allow_dif[0] = piqep_full.value();
        module_param->pi_qep_allow_dif[1] = piqep_actual.value();
        module_param->work_state.work_state_bit.pi1_work_state_bit = pi1_af.currentIndex();
        module_param->work_state.work_state_bit.pi2_work_state_bit = pi2_af.currentIndex();
        module_param->piqep1_2_ratio = piqep_ratio.value();
        module_param->piqep12_cross_check = qep_cross.currentIndex();
        module_param->piqep_speed_allow_dif[0] = qep_cross_full.value();
        module_param->piqep_speed_allow_dif[1] = qep_cross_actual.value();
        dialog.close();
    });
    QObject::connect(cancelButton, &QPushButton::clicked, [&]() { dialog.close(); });
    QObject::connect(setButton, &QPushButton::clicked, [&]() { deceleration_curve_config_dialog(); });
    dialog.exec();
}

void config_param::deceleration_curve_config_dialog()
{
    QDialog dialog;
    dialog.setWindowTitle("减速曲线参数");
    QFormLayout* layout = new QFormLayout(&dialog);
    QStringList qep_select_items;
    qep_select_items << "不使用"
                     << "QEP1"
                     << "QEP2"
                     << "平均值";
    QStringList piqep_select_items;
    piqep_select_items << "不使用"
                       << "PI_QEP1"
                       << "PI_QEP2"
                       << "平均值";
    QGroupBox curve_box[MAX_DECELERATE_NUM];
    QFormLayout* curve_layout[MAX_DECELERATE_NUM];
    QComboBox qep_sclect[MAX_DECELERATE_NUM];
    QComboBox piqep_sclect[MAX_DECELERATE_NUM];
    QSpinBox decelerate_check_interval[MAX_DECELERATE_NUM];
    QSpinBox decelerate_sublevel1[MAX_DECELERATE_NUM];
    QSpinBox decelerate_sublevel2[MAX_DECELERATE_NUM];
    QSpinBox decelerate_sublevel3[MAX_DECELERATE_NUM];
    for (int i = 0; i < MAX_DECELERATE_NUM; i++) {
        curve_box[i].setTitle("减速曲线" + QString::number(i + 1));
        curve_box[i].setStyleSheet("QGroupBox {"
                                   "border: 3px solid;"
                                   "border-radius: 9px;"
                                   "margin-top: 0.5em;"
                                   "}"
                                   "QGroupBox::title {"
                                   "subcontrol-origin: margin;"
                                   "subcontrol-position: top center;"
                                   "padding: 0 3px;"
                                   "font-weight: bold;"
                                   "}");
        qep_sclect[i].addItems(qep_select_items);
        qep_sclect[i].setCurrentIndex(module_param->qep_speed_select[i]);
        piqep_sclect[i].addItems(piqep_select_items);
        piqep_sclect[i].setCurrentIndex(module_param->piqep_speed_select[i]);
        connect(&piqep_sclect[i], SIGNAL(currentIndexChanged(int)), this, SLOT(param_piqepspeed_select_slot(int)));
        decelerate_check_interval[i].setMinimum(1);
        decelerate_check_interval[i].setMaximum(10000);
        decelerate_check_interval[i].setValue(module_param->decelerate_check_interval[i]);
        decelerate_sublevel1[i].setMinimum(0);
        decelerate_sublevel1[i].setMaximum(200000);
        decelerate_sublevel1[i].setValue(module_param->decelerate_sublevel_threshold[i][0]);
        decelerate_sublevel2[i].setMinimum(0);
        decelerate_sublevel2[i].setMaximum(200000);
        decelerate_sublevel2[i].setValue(module_param->decelerate_sublevel_threshold[i][1]);
        decelerate_sublevel3[i].setMinimum(0);
        decelerate_sublevel3[i].setMaximum(200000);
        decelerate_sublevel3[i].setValue(module_param->decelerate_sublevel_threshold[i][2]);
        curve_layout[i] = new QFormLayout(&curve_box[i]);
        curve_layout[i]->setContentsMargins(10, 10, 10, 10);
        curve_layout[i]->addRow("QEP速度选择", &qep_sclect[i]);
        curve_layout[i]->addRow("PI_QEP速度选择", &piqep_sclect[i]);
        curve_layout[i]->addRow("检测间隔", &decelerate_check_interval[i]);
        curve_layout[i]->addRow("第一段速度阈值", &decelerate_sublevel1[i]);
        curve_layout[i]->addRow("第二段速度阈值", &decelerate_sublevel2[i]);
        curve_layout[i]->addRow("第三段速度阈值", &decelerate_sublevel3[i]);
    }
    QHBoxLayout hboxlayout1;
    hboxlayout1.addWidget(&curve_box[0]);
    hboxlayout1.addWidget(&curve_box[1]);
    QHBoxLayout hboxlayout2;
    hboxlayout2.addWidget(&curve_box[2]);
    hboxlayout2.addWidget(&curve_box[3]);
    layout->addRow(&hboxlayout1);
    layout->addRow(&hboxlayout2);
    layout->addRow(&curve_box[4]);
    QPushButton* okButton = new QPushButton("确认", &dialog);
    QPushButton* cancelButton = new QPushButton("取消", &dialog);
    QHBoxLayout buttonLayout;
    buttonLayout.addWidget(cancelButton);
    buttonLayout.addWidget(okButton);
    layout->addRow(&buttonLayout);
    QObject::connect(okButton, &QPushButton::clicked, [&]() {
        for (int i = 0; i < MAX_DECELERATE_NUM; i++) {
            module_param->qep_speed_select[i] = qep_sclect[i].currentIndex();
            module_param->piqep_speed_select[i] = piqep_sclect[i].currentIndex();
            module_param->decelerate_check_interval[i] = decelerate_check_interval[i].value();
            module_param->decelerate_sublevel_threshold[i][0] = decelerate_sublevel1[i].value();
            module_param->decelerate_sublevel_threshold[i][1] = decelerate_sublevel2[i].value();
            module_param->decelerate_sublevel_threshold[i][2] = decelerate_sublevel3[i].value();
        }
        dialog.close();
    });
    QObject::connect(cancelButton, &QPushButton::clicked, [&]() { dialog.close(); });
    layout->setContentsMargins(10, 10, 10, 10);
    dialog.exec();
}

/* sys event */
void config_param::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        switch (tool_type) {
        case CONFIG_TOOL_SYS:
            sys_param_config_dialog();
            break;
        case CONFIG_TOOL_CAN:
            can_param_config_dialog();
            break;
        case CONFIG_TOOL_AI:
            ai_param_config_dialog();
            break;
        case CONFIG_TOOL_DI:
            di_param_config_dialog();
            break;
        case CONFIG_TOOL_RELAY:
            relay_param_config_dialog();
            break;
        case CONFIG_TOOL_MOS:
            mos_param_config_dialog();
            break;
        case CONFIG_TOOL_ENCODE:
            encode_param_config_dialog();
            break;
        case CONFIG_TOOL_PI:
            pi_param_config_dialog();
            break;
        }
    }
    QGraphicsRectItem::mousePressEvent(event);
}

/* user slots */
void config_param::pi1_afstate_changed_slot(int index)
{
    module_param->work_state.work_state_bit.pi1_work_state_bit = index;
    if (index != PI_AF_QEP) {
        module_param->piqep12_cross_check = 0;
        for (int i = 0; i < MAX_DECELERATE_NUM; i++) {
            module_param->piqep_speed_select[i] = SPEED_SELECT_NONE;
        }
    }
    switch (index) {
    case PI_AF_PI:
        mainwindow->condition_view_class->di_item[TOOL_ID_DI11]->setDisabled(true);
        mainwindow->condition_view_class->qep_item[TOOL_ID_PI_QEP1]->setDisabled(true);
        mainwindow->condition_view_class->pi_item[TOOL_ID_PI1]->setDisabled(false);
        mainwindow->condition_view_class->di_item[TOOL_ID_DI11]->setCheckState(0, Qt::Unchecked);
        mainwindow->condition_view_class->qep_item[TOOL_ID_PI_QEP1]->setCheckState(0, Qt::Unchecked);
        if (pi2_af_combo->currentIndex() == PI_AF_QEP) {
            pi2_af_combo->setCurrentIndex(index);
        }
        break;
    case PI_AF_QEP:
        mainwindow->condition_view_class->di_item[TOOL_ID_DI11]->setDisabled(true);
        mainwindow->condition_view_class->qep_item[TOOL_ID_PI_QEP1]->setDisabled(false);
        mainwindow->condition_view_class->pi_item[TOOL_ID_PI1]->setDisabled(true);
        mainwindow->condition_view_class->di_item[TOOL_ID_DI11]->setCheckState(0, Qt::Unchecked);
        mainwindow->condition_view_class->pi_item[TOOL_ID_PI1]->setCheckState(0, Qt::Unchecked);
        if (pi2_af_combo->currentIndex() != index) {
            pi2_af_combo->setCurrentIndex(index);
        }
        break;
    case PI_AF_DI:
        mainwindow->condition_view_class->di_item[TOOL_ID_DI11]->setDisabled(false);
        mainwindow->condition_view_class->qep_item[TOOL_ID_PI_QEP1]->setDisabled(true);
        mainwindow->condition_view_class->pi_item[TOOL_ID_PI1]->setDisabled(true);
        mainwindow->condition_view_class->di_item[TOOL_ID_DI11]->setCheckState(0, Qt::Unchecked);
        mainwindow->condition_view_class->pi_item[TOOL_ID_PI1]->setCheckState(0, Qt::Unchecked);
        if (pi2_af_combo->currentIndex() == PI_AF_QEP) {
            pi2_af_combo->setCurrentIndex(index);
        }
        break;
    }
}

void config_param::pi2_afstate_changed_slot(int index)
{
    module_param->work_state.work_state_bit.pi2_work_state_bit = index;
    if (index != PI_AF_QEP) {
        module_param->piqep12_cross_check = 0;
        for (int i = 0; i < MAX_DECELERATE_NUM; i++) {
            module_param->piqep_speed_select[i] = SPEED_SELECT_NONE;
        }
    }
    switch (index) {
    case PI_AF_PI:
        mainwindow->condition_view_class->di_item[TOOL_ID_DI12]->setDisabled(true);
        mainwindow->condition_view_class->qep_item[TOOL_ID_PI_QEP2]->setDisabled(true);
        mainwindow->condition_view_class->pi_item[TOOL_ID_PI2]->setDisabled(false);
        mainwindow->condition_view_class->di_item[TOOL_ID_DI12]->setCheckState(0, Qt::Unchecked);
        mainwindow->condition_view_class->qep_item[TOOL_ID_PI_QEP2]->setCheckState(0, Qt::Unchecked);
        if (pi1_af_combo->currentIndex() == PI_AF_QEP) {
            pi1_af_combo->setCurrentIndex(index);
        }

        break;
    case PI_AF_QEP:
        mainwindow->condition_view_class->di_item[TOOL_ID_DI12]->setDisabled(true);
        mainwindow->condition_view_class->qep_item[TOOL_ID_PI_QEP2]->setDisabled(false);
        mainwindow->condition_view_class->pi_item[TOOL_ID_PI2]->setDisabled(true);
        mainwindow->condition_view_class->di_item[TOOL_ID_DI12]->setCheckState(0, Qt::Unchecked);
        mainwindow->condition_view_class->pi_item[TOOL_ID_PI2]->setCheckState(0, Qt::Unchecked);
        if (pi1_af_combo->currentIndex() != index) {
            pi1_af_combo->setCurrentIndex(index);
        }
        break;
    case PI_AF_DI:
        mainwindow->condition_view_class->di_item[TOOL_ID_DI12]->setDisabled(false);
        mainwindow->condition_view_class->qep_item[TOOL_ID_PI_QEP2]->setDisabled(true);
        mainwindow->condition_view_class->pi_item[TOOL_ID_PI2]->setDisabled(true);
        mainwindow->condition_view_class->qep_item[TOOL_ID_PI_QEP2]->setCheckState(0, Qt::Unchecked);
        mainwindow->condition_view_class->pi_item[TOOL_ID_PI2]->setCheckState(0, Qt::Unchecked);
        if (pi1_af_combo->currentIndex() == PI_AF_QEP) {
            pi1_af_combo->setCurrentIndex(index);
        }
        break;
    }
}

void config_param::ai1_afstate_changed_slot(int index)
{
    switch (index) {
    case AI_AF_AI:
        mainwindow->condition_view_class->di_item[TOOL_ID_DI9]->setDisabled(true);
        mainwindow->condition_view_class->ai_item[TOOL_ID_AI1]->setDisabled(false);
        mainwindow->condition_view_class->di_item[TOOL_ID_DI9]->setCheckState(0, Qt::Unchecked);
        break;
    case AI_AF_DI:
        mainwindow->condition_view_class->di_item[TOOL_ID_DI9]->setDisabled(false);
        mainwindow->condition_view_class->ai_item[TOOL_ID_AI1]->setDisabled(true);
        mainwindow->condition_view_class->ai_item[TOOL_ID_AI1]->setCheckState(0, Qt::Unchecked);
        break;
    }
}

void config_param::ai2_afstate_changed_slot(int index)
{
    switch (index) {
    case AI_AF_AI:
        mainwindow->condition_view_class->di_item[TOOL_ID_DI10]->setDisabled(true);
        mainwindow->condition_view_class->ai_item[TOOL_ID_AI2]->setDisabled(false);
        mainwindow->condition_view_class->di_item[TOOL_ID_DI10]->setCheckState(0, Qt::Unchecked);
        break;
    case AI_AF_DI:
        mainwindow->condition_view_class->di_item[TOOL_ID_DI10]->setDisabled(false);
        mainwindow->condition_view_class->ai_item[TOOL_ID_AI2]->setDisabled(true);
        mainwindow->condition_view_class->ai_item[TOOL_ID_AI2]->setCheckState(0, Qt::Unchecked);
        break;
    }
}

void config_param::param_piqepspeed_select_slot(int index)
{
    QObject* senderObj = QObject::sender();
    QComboBox* comboBoxSender = qobject_cast<QComboBox*>(senderObj);
    if (comboBoxSender) {
        if (index != SPEED_SELECT_NONE) {
            if (module_param->work_state.work_state_bit.pi1_work_state_bit != PI_AF_QEP
                || module_param->work_state.work_state_bit.pi2_work_state_bit != PI_AF_QEP) {
                comboBoxSender->setCurrentIndex(SPEED_SELECT_NONE);
                mainwindow->my_message_box("操作失败", "启用功能需要PI复用为QEP", false);
                return;
            }
        }
    }
}

void config_param::piqep_crosscheck_slot(int index)
{
    QObject* senderObj = QObject::sender();
    QComboBox* comboBoxSender = qobject_cast<QComboBox*>(senderObj);
    if (index == 1) {
        if (module_param->work_state.work_state_bit.pi1_work_state_bit != PI_AF_QEP
            || module_param->work_state.work_state_bit.pi2_work_state_bit != PI_AF_QEP) {
            comboBoxSender->setCurrentIndex(0);
            mainwindow->my_message_box("操作失败", "启用功能需要PI复用为QEP", false);
            return;
        }
    }
}
