#include "project_report.h"
#include "list_condi/condition_view.h"
#include "mainwindow.h"
#include <QDebug>
#define REPORT_TABLE_COL_NUM    (8)
#define PROJECT_TABLE_ROW_START (0)
#define PROJECT_TABLE_ROW_NUM   (7)
#define INPUT_TABLE_ROW_START   (PROJECT_TABLE_ROW_START + PROJECT_TABLE_ROW_NUM)
#define INPUT_TABLE_ROW_NUM     (21)
#define OUTPUT_TABLE_ROW_START  (INPUT_TABLE_ROW_START + INPUT_TABLE_ROW_NUM)
#define OUTPUT_TABLE_ROW_NUM    (12)
#define COROUTINE_ROW_START     (OUTPUT_TABLE_ROW_START + OUTPUT_TABLE_ROW_NUM)
#define COROUTINE_ROW_NUM       (6)
#define SF_ROW_START            (COROUTINE_ROW_START + COROUTINE_ROW_NUM)
#define SF_ROW_NUM              (20)

#define REPORT_TABLE_ROW_NUM \
    (PROJECT_TABLE_ROW_NUM + INPUT_TABLE_ROW_NUM + OUTPUT_TABLE_ROW_NUM + COROUTINE_ROW_NUM + SF_ROW_NUM)

project_report::project_report(QWidget* parent)
    : QWidget(parent)
{
    ui         = MainWindow::my_ui->ui;
    mainwindow = ( MainWindow* )parent;
    ui->tableWidget_report->setColumnCount(8);
    ui->tableWidget_report->setRowCount(REPORT_TABLE_ROW_NUM);
    ui->tableWidget_report->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget_report->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    project_report_init();
    connect(ui->tabWidget_logic, &QTabWidget::currentChanged, this, tab_changeed_slot);
    update_timer.setSingleShot(true);
    connect(&update_timer, &QTimer::timeout, this, project_report_update_slot);
}

void project_report::project_report_init()
{
    ui->tableWidget_report->clearContents();
    project_table_init();
    device_input_table_init();
    device_output_table_init();
    coroutine_table_init();
    sf_table_init();
}

void project_report::sf_table_update()
{
    int num = mainwindow->logic_view_class->sf_used_inf.used_number;
    ui->tableWidget_report->item(SF_ROW_START, 0)
        ->setText("安全功能(" + QString::number(num) + "/" + QString::number(MAX_SF_NUM) + ")");

    for (int i = 0; i < MAX_SF_NUM; i++) {
        if (mainwindow->logic_view_class->sf_used_inf.sf_code[i].is_used) {
            ui->tableWidget_report->setRowHidden(SF_ROW_START + 1 + i, false);
            QTableWidgetItem* item1 = new QTableWidgetItem(mainwindow->logic_view_class->sf_used_inf.block_name[i]);
            ui->tableWidget_report->setItem(SF_ROW_START + 1 + i, 0, item1);

            QTableWidgetItem* item2 = new QTableWidgetItem(mainwindow->logic_view_class->sf_used_inf.sf_param[i].name);
            ui->tableWidget_report->setItem(SF_ROW_START + 1 + i, 1, item2);

            QTableWidgetItem* item3 = new QTableWidgetItem(
                "0x" + QString::number(mainwindow->logic_view_class->sf_used_inf.sf_param[i].sf_code, 16));
            ui->tableWidget_report->setItem(SF_ROW_START + 1 + i, 3, item3);

            QTableWidgetItem* item4 =
                new QTableWidgetItem(sf_type_str[mainwindow->logic_view_class->sf_used_inf.sf_param[i].sf_type]);
            ui->tableWidget_report->setItem(SF_ROW_START + 1 + i, 4, item4);

            QTableWidgetItem* item5 = new QTableWidgetItem(
                "0x" + QString::number(mainwindow->logic_view_class->sf_used_inf.sf_param[i].ss_code, 16));
            ui->tableWidget_report->setItem(SF_ROW_START + 1 + i, 5, item5);

            QTableWidgetItem* item6 = new QTableWidgetItem(
                QString::number(mainwindow->logic_view_class->sf_used_inf.sf_param[i].delay_time) + "ms");
            ui->tableWidget_report->setItem(SF_ROW_START + 1 + i, 6, item6);

            QTableWidgetItem* item7 = new QTableWidgetItem(
                QString::number(mainwindow->logic_view_class->sf_used_inf.sf_param[i].option_time) + "ms");
            ui->tableWidget_report->setItem(SF_ROW_START + 1 + i, 7, item7);

        } else {
            ui->tableWidget_report->setRowHidden(SF_ROW_START + 1 + i, true);
        }
    }
}

void project_report::coroutine_table_update()
{
    int coroutine_num = mainwindow->coroutine_lua_class->coroutine_name.count();
    ui->tableWidget_report->item(COROUTINE_ROW_START, 0)
        ->setText("线程使用(" + QString::number(coroutine_num) + "/" + QString::number(MAX_COROUTINE_NUM) + ")");
    for (int i = 0; i < coroutine_num; i++) {
        QTableWidgetItem* item1 = new QTableWidgetItem(mainwindow->coroutine_lua_class->coroutine_name[i]);
        ui->tableWidget_report->setItem(COROUTINE_ROW_START + 1 + i, 0, item1);
        float percentage =
            mainwindow->coroutine_lua_class->coroutine_code[i].size() * 100.0 / COROUTINE_CODE_MAX_LENGTH;
        QTableWidgetItem* item2 = new QTableWidgetItem("线程资源已使用: " + QString::number(percentage, 10, 2) + "%");
        ui->tableWidget_report->setItem(COROUTINE_ROW_START + 1 + i, 2, item2);
        ui->tableWidget_report->setRowHidden(COROUTINE_ROW_START + 1 + i, false);
    }
    for (int i = coroutine_num; i < MAX_COROUTINE_NUM; i++) {
        ui->tableWidget_report->setRowHidden(COROUTINE_ROW_START + 1 + i, true);
    }
}

void project_report::device_output_table_update()
{
    QStringList relevant;
    relevant << "not relevant"
             << "relevant";
    ui->tableWidget_report->item(OUTPUT_TABLE_ROW_START, 0)
        ->setText("输出资源分配(" + QString::number(mainwindow->condition_view_class->ss_info_list.count()) + "/"
                  + QString::number(MAX_SS_NUM) + ")");
    if (mainwindow->condition_view_class->ss_info_list.count() > 0) {
        ui->tableWidget_report->setRowHidden(OUTPUT_TABLE_ROW_START + 1, false);
    }
    for (int i = 0; i < mainwindow->condition_view_class->ss_info_list.count(); i++) {
        QTableWidgetItem* item1 =
            new QTableWidgetItem("0x" + QString::number(mainwindow->condition_view_class->ss_info_list[i].ss_code, 16));
        ui->tableWidget_report->setItem(OUTPUT_TABLE_ROW_START + 2 + i, 1, item1);
        ui->tableWidget_report->setRowHidden(OUTPUT_TABLE_ROW_START + 2 + i, false);
        for (int j = 0; j < 6; j++) {
            uint8_t           state = mainwindow->condition_view_class->ss_info_list[i].relevant_state;
            QTableWidgetItem* item2 = new QTableWidgetItem(relevant[(state >> j) & 0x01]);
            ui->tableWidget_report->setItem(OUTPUT_TABLE_ROW_START + 2 + i, 2 + j, item2);
        }
    }
    for (int i = mainwindow->condition_view_class->ss_info_list.count(); i < MAX_SS_NUM; i++) {
        ui->tableWidget_report->setRowHidden(OUTPUT_TABLE_ROW_START + 2 + i, true);
    }
}

void project_report::device_input_table_update()
{
    QStringList is_used;
    QStringList is_config;
    for (int i = 0; i < INPUT_RESOURCE_NUM; i++) {
        if (input_resource_info.is_config[i]) {
            is_config.append("已配置");
        } else {
            is_config.append("未配置");
        }
        if (input_resource_info.is_used[i]) {
            is_used.append("已使用");
        } else {
            is_used.append("未使用");
        }
    }
    for (int i = 0; i < is_config.count(); i++) {
        QTableWidgetItem* item1 = new QTableWidgetItem(is_config[i]);
        ui->tableWidget_report->setItem(INPUT_TABLE_ROW_START + 1 + i, 2, item1);
        QTableWidgetItem* item2 = new QTableWidgetItem(is_used[i]);
        ui->tableWidget_report->setItem(INPUT_TABLE_ROW_START + 1 + i, 4, item2);
        QTableWidgetItem* item3 =
            new QTableWidgetItem(mainwindow->condition_view_class->other_name_edit_list[i]->text());
        ui->tableWidget_report->setItem(INPUT_TABLE_ROW_START + 1 + i, 6, item3);
    }
}

void project_report::project_table_update()
{
    QStringList info;
    info.append(ui->lineEdit_projectname->text() == "" ? "待设置" : ui->lineEdit_projectname->text());
    info.append(ui->lineEdit_author_name->text() == "" ? "待设置" : ui->lineEdit_author_name->text());
    info.append(ui->lineEdit_company_name->text() == "" ? "待设置" : ui->lineEdit_company_name->text());
    info.append(ui->lineEdit_project_version->text() == "" ? "待设置" : ui->lineEdit_project_version->text());
    info.append(ui->lineEdit_project_path->text() == "" ? "项目未保存" : ui->lineEdit_project_path->text());
    if (ui->checkBox_advanced_program->isChecked()) {
        info.append("已启用");
    } else {
        info.append("未启用");
    }
    for (int i = 0; i < info.count(); i++) {

        QTableWidgetItem* item = new QTableWidgetItem(info[i]);
        ui->tableWidget_report->setItem(PROJECT_TABLE_ROW_START + 1 + i, 1, item);
    }
}

void project_report::coroutine_table_init()
{
    ui->tableWidget_report->setSpan(COROUTINE_ROW_START, 0, 1, REPORT_TABLE_COL_NUM);
    QTableWidgetItem* item = new QTableWidgetItem("\r\n 线程使用(0/5)\r\n");
    QFont             font("微软雅黑", 12);
    font.setBold(true);
    item->setFont(font);
    QColor textColor(50, 150, 255);
    item->setTextColor(textColor);
    ui->tableWidget_report->setItem(COROUTINE_ROW_START, 0, item);
    for (int i = 0; i < MAX_COROUTINE_NUM; i++) {
        ui->tableWidget_report->setRowHidden(COROUTINE_ROW_START + 1 + i, true);
        ui->tableWidget_report->setSpan(COROUTINE_ROW_START + 1 + i, 0, 1, 2);
        ui->tableWidget_report->setSpan(COROUTINE_ROW_START + 1 + i, 2, 1, REPORT_TABLE_COL_NUM);
    }
}

void project_report::sf_table_init()
{
    ui->tableWidget_report->setSpan(SF_ROW_START, 0, 1, REPORT_TABLE_COL_NUM);
    QTableWidgetItem* item = new QTableWidgetItem("\r\n 安全功能(0/19)\r\n");
    QFont             font("微软雅黑", 12);
    font.setBold(true);
    item->setFont(font);
    QColor textColor(50, 150, 255);
    item->setTextColor(textColor);
    ui->tableWidget_report->setItem(SF_ROW_START, 0, item);
    for (int i = 0; i < MAX_SF_NUM; i++) {
        ui->tableWidget_report->setRowHidden(SF_ROW_START + 1 + i, true);
        ui->tableWidget_report->setSpan(SF_ROW_START + 1 + i, 1, 1, 2);
    }
}

void project_report::device_output_table_init()
{
    ui->tableWidget_report->setSpan(OUTPUT_TABLE_ROW_START, 0, 1, REPORT_TABLE_COL_NUM);
    QTableWidgetItem* item = new QTableWidgetItem("\r\n 输出资源分配(0/10)\r\n");
    QFont             font("微软雅黑", 12);
    font.setBold(true);
    item->setFont(font);
    QColor textColor(50, 150, 255);
    item->setTextColor(textColor);
    ui->tableWidget_report->setItem(OUTPUT_TABLE_ROW_START, 0, item);

    QStringList topic;
    topic << "序号"
          << "Code"
          << "Relay1"
          << "Relay2"
          << "MOS1"
          << "MOS2"
          << "MOS3"
          << "MOS4";
    for (int i = 0; i < topic.count(); i++) {
        QTableWidgetItem* itemt = new QTableWidgetItem(topic[i]);
        QFont             fontt("微软雅黑", 10);
        fontt.setBold(true);
        itemt->setFont(font);
        QBrush brush(QColor(240, 240, 240));
        itemt->setBackground(brush);
        ui->tableWidget_report->setItem(OUTPUT_TABLE_ROW_START + 1, i, itemt);
    }
    ui->tableWidget_report->setRowHidden(OUTPUT_TABLE_ROW_START + 1, true);
    for (int i = 0; i < MAX_SS_NUM; i++) {
        QTableWidgetItem* itemt = new QTableWidgetItem(QString::number(i + 1) + "/" + QString::number(MAX_SS_NUM));
        ui->tableWidget_report->setItem(OUTPUT_TABLE_ROW_START + 2 + i, 0, itemt);
        ui->tableWidget_report->setRowHidden(OUTPUT_TABLE_ROW_START + 2 + i, true);
    }
}

void project_report::project_table_init()
{
    ui->tableWidget_report->setSpan(PROJECT_TABLE_ROW_START, 0, 1, REPORT_TABLE_COL_NUM);
    QTableWidgetItem* item = new QTableWidgetItem("\r\n 项目信息\r\n");
    QFont             font("微软雅黑", 12);
    font.setBold(true);
    item->setFont(font);
    QColor textColor(50, 150, 255);
    item->setTextColor(textColor);
    ui->tableWidget_report->setItem(PROJECT_TABLE_ROW_START, 0, item);
    QStringList project_topic_list;
    project_topic_list << "项目名称:"
                       << "作者:"
                       << "公司:"
                       << "项目版本标识:"
                       << "项目保存路径:"
                       << "高级编程:";
    for (uint8_t i = 0; i < project_topic_list.count(); i++) {
        QTableWidgetItem* itemp = new QTableWidgetItem(project_topic_list[i]);
        ui->tableWidget_report->setItem(i + 1, 0, itemp);
        ui->tableWidget_report->setSpan(PROJECT_TABLE_ROW_START + 1 + i, 1, 1, REPORT_TABLE_COL_NUM);
    }
}

void project_report::device_input_table_init()
{
    ui->tableWidget_report->setSpan(INPUT_TABLE_ROW_START, 0, 1, REPORT_TABLE_COL_NUM);
    QTableWidgetItem* item = new QTableWidgetItem("\r\n 输入资源分配\r\n");
    QFont             font("微软雅黑", 12);
    font.setBold(true);
    item->setFont(font);
    QColor textColor(50, 150, 255);
    item->setTextColor(textColor);
    ui->tableWidget_report->setItem(INPUT_TABLE_ROW_START, 0, item);
    for (int i = 0; i < di_resource.count(); i++) {
        QTableWidgetItem* itemd = new QTableWidgetItem(di_resource[i]);
        ui->tableWidget_report->setItem(INPUT_TABLE_ROW_START + i + 1, 0, itemd);
        ui->tableWidget_report->setSpan(INPUT_TABLE_ROW_START + 1 + i, 0, 1, 2);
        ui->tableWidget_report->setSpan(INPUT_TABLE_ROW_START + 1 + i, 2, 1, 2);
        ui->tableWidget_report->setSpan(INPUT_TABLE_ROW_START + 1 + i, 4, 1, 2);
        ui->tableWidget_report->setSpan(INPUT_TABLE_ROW_START + 1 + i, 6, 1, 2);
    }

    for (int i = 0; i < ai_resource.count(); i++) {
        QTableWidgetItem* itema = new QTableWidgetItem(ai_resource[i]);
        ui->tableWidget_report->setItem(INPUT_TABLE_ROW_START + i + 1 + di_resource.count(), 0, itema);
        ui->tableWidget_report->setSpan(INPUT_TABLE_ROW_START + 1 + i + di_resource.count(), 0, 1, 2);
        ui->tableWidget_report->setSpan(INPUT_TABLE_ROW_START + 1 + i + di_resource.count(), 2, 1, 2);
        ui->tableWidget_report->setSpan(INPUT_TABLE_ROW_START + 1 + i + di_resource.count(), 4, 1, 2);
        ui->tableWidget_report->setSpan(INPUT_TABLE_ROW_START + 1 + i + di_resource.count(), 6, 1, 2);
    }
    for (int i = 0; i < pi_resource.count(); i++) {
        QTableWidgetItem* itemp = new QTableWidgetItem(pi_resource[i]);
        ui->tableWidget_report->setItem(INPUT_TABLE_ROW_START + i + 1 + di_resource.count() + ai_resource.count(), 0,
                                        itemp);
        ui->tableWidget_report->setSpan(INPUT_TABLE_ROW_START + 1 + i + di_resource.count() + ai_resource.count(), 0, 1,
                                        2);
        ui->tableWidget_report->setSpan(INPUT_TABLE_ROW_START + 1 + i + di_resource.count() + ai_resource.count(), 2, 1,
                                        2);
        ui->tableWidget_report->setSpan(INPUT_TABLE_ROW_START + 1 + i + di_resource.count() + ai_resource.count(), 4, 1,
                                        2);
        ui->tableWidget_report->setSpan(INPUT_TABLE_ROW_START + 1 + i + di_resource.count() + ai_resource.count(), 6, 1,
                                        2);
    }

    for (int i = 0; i < qep_resource.count(); i++) {
        QTableWidgetItem* itemq = new QTableWidgetItem(qep_resource[i]);
        ui->tableWidget_report->setItem(
            INPUT_TABLE_ROW_START + i + 1 + di_resource.count() + ai_resource.count() + pi_resource.count(), 0, itemq);
        ui->tableWidget_report->setSpan(
            INPUT_TABLE_ROW_START + 1 + i + di_resource.count() + ai_resource.count() + pi_resource.count(), 0, 1, 2);
        ui->tableWidget_report->setSpan(
            INPUT_TABLE_ROW_START + 1 + i + di_resource.count() + ai_resource.count() + pi_resource.count(), 2, 1, 2);
        ui->tableWidget_report->setSpan(
            INPUT_TABLE_ROW_START + 1 + i + di_resource.count() + ai_resource.count() + pi_resource.count(), 4, 1, 2);
        ui->tableWidget_report->setSpan(
            INPUT_TABLE_ROW_START + 1 + i + di_resource.count() + ai_resource.count() + pi_resource.count(), 6, 1, 2);
    }
}

/* user slots */

void project_report::project_report_update_slot()
{
    ui->tableWidget_report->setUpdatesEnabled(false);
    project_table_update();
    device_input_table_update();
    device_output_table_update();
    coroutine_table_update();
    sf_table_update();
    sf_table_update();
    ui->tableWidget_report->setUpdatesEnabled(true);
}

void project_report::tab_changeed_slot(int index)
{
    if (index == ui->tabWidget_logic->indexOf(ui->tab_report)) {
        update_timer.start(100);
    }
}
