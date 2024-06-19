#include "project_report.h"
#include "config/data_config.h"
#include "graphical_config/config_param.h"
#include "mainwindow.h"
#include <QFileDialog>
#include <QFileInfo>
#include <QFormLayout>
#include <QObject>
#include <QPdfWriter>
#include <QProgressDialog>
#include <QtPrintSupport/QPrinter>
#include <windows.h>
#define ELEMENT_POINT_WIDTH     (14)
#define ELEMENT_POINT_HALFWIDTH (ELEMENT_POINT_WIDTH / 2)
#define LEFT_ELEMENT_X          (110)
#define RIGHT_ELEMENT_X         (430)
#define LEFT_POWER_X            (50)
#define RIGHT_POWER_X           (500)
#define ELEMENT_DISTANCE        (80)
#define ELEMENT_X               (200)
#define ELEMENT_PIXMAP_X_OFFSET (30)
#define ELEMENT_HALFDISTANCE    (ELEMENT_DISTANCE / 2)
#define LINE_WIDTH              (3)
#define LINE_HALFWIDTH          (0)
project_report::project_report(QWidget* mparent)
{
    mainwindow = ( MainWindow* )mparent;
}

void project_report::project_report_clear(QGridLayout* layout)
{
    QLayoutItem* child;
    while ((child = layout->takeAt(0)) != nullptr) {
        if (child->widget()) {
            delete child->widget();
        }
        delete child;
    }
}

void project_report::project_info_creat(QGridLayout* layout, int* line)
{
    QGridLayout* clayout = new QGridLayout;
    int          cline   = 0;
    int          mline   = *line;
    QFont        font;
    font.setBold(true);
    font.setFamily("Arial");
    font.setPixelSize(15);
    QLabel* project_title = new QLabel("1 项目信息");
    project_title->setFont(font);
    project_title->setStyleSheet("color: rgb(0,127,195);");
    title_row.append(mline);
    layout->addWidget(project_title, mline++, 0);
    QStringList title_label;
    title_label << "项目名称："
                << "作者："
                << "公司："
                << "项目保存路径："
                << "最后修改时间："
                << "当前工程校验码："
                << "设备工程校验码：";
    font.setPixelSize(11);
    for (int i = 0; i < title_label.size(); i++) {
        QLabel* title = new QLabel(title_label[i]);
        title->setFont(font);
        clayout->addWidget(title, cline + i, 0);
    }
    QList<QLineEdit*> info_lineedit;
    info_lineedit.append(mainwindow->ui->lineEdit_project_name);
    info_lineedit.append(mainwindow->ui->lineEdit_author_name);
    info_lineedit.append(mainwindow->ui->lineEdit_company_name);
    info_lineedit.append(mainwindow->ui->lineEdit_path_name);
    info_lineedit.append(mainwindow->ui->lineEdit_save_time);
    info_lineedit.append(mainwindow->ui->lineEdit_current_project_checknum);
    info_lineedit.append(mainwindow->ui->lineEdit_device_project_checknum);
    font.setBold(false);
    for (int i = 0; i < info_lineedit.size(); i++) {
        QLabel* info = new QLabel(info_lineedit[i]->text());
        info->setFont(font);
        clayout->addWidget(info, cline++, 1);
    }
    layout->addLayout(clayout, mline++, 0);
    *line = mline;
}

void project_report::config_overview_creat(QGridLayout* layout, int* line)
{
    int   mline = *line;
    QFont font;
    font.setBold(true);
    font.setFamily("Arial");
    font.setPixelSize(15);
    /* 标题2.1 */
    QLabel* project_title = new QLabel("2 配置概览");
    project_title->setFont(font);
    project_title->setStyleSheet("color: rgb(0,127,195);");
    title_row.append(mline);
    layout->addWidget(project_title, mline++, 0);
    font.setPixelSize(13);
    /* 概览图 */
    mainwindow->config_view_class->config_photo_svg->set_base_data_enable(false);
    int    left_config_width  = LABEL_WIDTH - 12;
    int    right_config_width = LABEL_WIDTH - 9;
    QRectF spc_rect           = mainwindow->config_view_class->config_photo_svg->sceneBoundingRect().adjusted(
        -left_config_width, 0, right_config_width, 0);
    QLabel* spc100_image = new QLabel;
    QPixmap spc100_pixmap(spc_rect.width(), spc_rect.height());
    spc100_pixmap.fill(Qt::transparent);
    QPainter spc100_painter(&spc100_pixmap);
    QRect    target_rect(0, 0, spc_rect.width(), spc_rect.height());
    mainwindow->config_view_class->scene()->render(&spc100_painter, target_rect, spc_rect);
    spc100_image->setPixmap(spc100_pixmap);
    layout->addWidget(spc100_image, mline++, 0, 1, 3);
    mainwindow->config_view_class->config_photo_svg->set_base_data_enable(true);
    QLabel* title1 = new QLabel("2.1 输入配置");
    title1->setFont(font);
    title1->setStyleSheet("color: rgb(0,127,195);");
    title_row.append(mline);
    layout->addWidget(title1, mline++, 0);
    /* 表格 */
    QStringList col_title;
    col_title << "输入类型"
              << "输入标识"
              << "元件类型"
              << "用户标签"
              << "参数";
    QTableWidget* input_table = new QTableWidget;
    input_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    input_table->setSelectionMode(QAbstractItemView::NoSelection);
    input_table->verticalHeader()->setVisible(false);
    input_table->setFocusPolicy(Qt::NoFocus);
    input_table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    input_table->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    input_table->horizontalHeader()->setSectionsClickable(false);
    input_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    input_table->setStyleSheet("QHeaderView::section:horizontal {background: rgba(128,128,128,100); }"
                               "QTableWidget { background-color: transparent; gridline-color: black; } ");
    for (int i = 0; i < col_title.size(); i++) {
        input_table->insertColumn(input_table->columnCount());
        QTableWidgetItem* item = new QTableWidgetItem(col_title[i]);
        item->setFont(QFont("Arial", 11, QFont::Bold));
        input_table->setHorizontalHeaderItem(i, item);
    }
    /* ai */
    QList<config_param*> a_ai_list    = mainwindow->config_view_class->config_photo_svg->a_ai_config_list;
    QList<config_param*> b_ai_list    = mainwindow->config_view_class->config_photo_svg->b_ai_config_list;
    int                  ai_start_row = input_table->rowCount();
    for (int i = 0; i < a_ai_list.size(); i++) {
        int a_row = 0, b_row = 0;
        if (!a_ai_list[i]->get_block_data().is_used && !b_ai_list[i]->get_block_data().is_used) {
            continue;
        }
        if (a_ai_list[i]->get_block_data().is_used) {
            a_row = input_table->rowCount();
            input_table->insertRow(a_row);
            input_table->setItem(a_row, 1, new QTableWidgetItem(a_ai_list[i]->get_block_data().source_name + ".A"));
        }
        if (b_ai_list[i]->get_block_data().is_used) {
            b_row = input_table->rowCount();
            input_table->insertRow(input_table->rowCount());
            input_table->setItem(b_row, 1, new QTableWidgetItem(b_ai_list[i]->get_block_data().source_name + ".B"));
        }
        if (a_ai_list[i]->get_block_data().is_used) {
            if (a_ai_list[i]->get_block_data().safe_level == SAFE_LEVEL_CAT3) {
                input_table->setSpan(a_row, 2, 2, 1);
                input_table->setSpan(a_row, 3, 2, 1);
                input_table->setSpan(a_row, 4, 2, 1);
                input_table->setItem(a_row, 2, new QTableWidgetItem("双通道"));
                input_table->setItem(a_row, 3,
                                     new QTableWidgetItem(a_ai_list[i]->get_block_data().name.isEmpty() ?
                                                              "无" :
                                                              a_ai_list[i]->get_block_data().name));
                module_param_t* safe_param = mainwindow->config_view_class->get_module_param();
                QString         param      = "采样间隔：" + QString::number(safe_param->sai_sample_interval)
                                + "ms\r\n互检满量程占比： " + QString ::number(safe_param->sai_allow_dif[0])
                                + "%\r\n互检实时数据占比：" + QString::number(safe_param->sai_allow_dif[1]) + "%";
                input_table->setItem(a_row, 4, new QTableWidgetItem(param));
            } else {
                input_table->setItem(a_row, 2, new QTableWidgetItem("单通道"));
                input_table->setItem(a_row, 3,
                                     new QTableWidgetItem(a_ai_list[i]->get_block_data().name.isEmpty() ?
                                                              "无" :
                                                              a_ai_list[i]->get_block_data().name));
                module_param_t* safe_param = mainwindow->config_view_class->get_module_param();
                QString         param = "采样间隔：" + QString::number(safe_param->sai_sample_interval) + "ms";
                input_table->setItem(a_row, 4, new QTableWidgetItem(param));
            }
        }
        if (b_ai_list[i]->get_block_data().is_used) {
            if (b_ai_list[i]->get_block_data().safe_level == SAFE_LEVEL_CAT2) {
                input_table->setItem(b_row, 2, new QTableWidgetItem("单通道"));
                input_table->setItem(b_row, 3,
                                     new QTableWidgetItem(b_ai_list[i]->get_block_data().name.isEmpty() ?
                                                              "无" :
                                                              b_ai_list[i]->get_block_data().name));
                module_param_t* safe_param = mainwindow->config_view_class->get_module_param();
                QString         param = "采样间隔：" + QString::number(safe_param->sai_sample_interval) + "ms";
                input_table->setItem(b_row, 4, new QTableWidgetItem(param));
            }
        }
    }
    int ai_end_row = input_table->rowCount();
    if (ai_end_row > ai_start_row) {
        input_table->setSpan(ai_start_row, 0, ai_end_row - ai_start_row, 1);
        input_table->setItem(ai_start_row, 0, new QTableWidgetItem("AI"));
    }
    /*DI*/
    QList<config_param*> a_di_list    = mainwindow->config_view_class->config_photo_svg->a_di_config_list;
    QList<config_param*> b_di_list    = mainwindow->config_view_class->config_photo_svg->b_di_config_list;
    int                  di_start_row = input_table->rowCount();
    for (int i = 0; i < a_di_list.size(); i++) {
        int         a_row = 0, b_row = 0;
        QStringList ns;
        ns << ""
           << "常闭"
           << "常开";
        if (!a_di_list[i]->get_block_data().is_used && !b_di_list[i]->get_block_data().is_used) {
            continue;
        }
        if (a_di_list[i]->get_block_data().is_used) {
            a_row = input_table->rowCount();
            input_table->insertRow(a_row);
            input_table->setItem(a_row, 1, new QTableWidgetItem(a_di_list[i]->get_block_data().source_name + ".A"));
        }
        if (b_di_list[i]->get_block_data().is_used) {
            b_row = input_table->rowCount();
            input_table->insertRow(input_table->rowCount());
            input_table->setItem(b_row, 1, new QTableWidgetItem(b_di_list[i]->get_block_data().source_name + ".B"));
        }
        if (a_di_list[i]->get_block_data().is_used) {
            if (a_di_list[i]->get_block_data().safe_level == SAFE_LEVEL_CAT3) {
                input_table->setSpan(a_row, 2, 2, 1);
                input_table->setSpan(a_row, 3, 2, 1);
                input_table->setSpan(a_row, 4, 2, 1);
                input_table->setItem(
                    a_row, 2,
                    new QTableWidgetItem("双通道" + ns[a_di_list[i]->get_block_data().config_param_data.normal_state]));
                input_table->setItem(a_row, 3,
                                     new QTableWidgetItem(a_di_list[i]->get_block_data().name.isEmpty() ?
                                                              "无" :
                                                              a_di_list[i]->get_block_data().name));
                input_table->setItem(a_row, 4, new QTableWidgetItem("无"));
            } else {
                input_table->setItem(
                    a_row, 2,
                    new QTableWidgetItem("单通道" + ns[a_di_list[i]->get_block_data().config_param_data.normal_state]));
                input_table->setItem(a_row, 3,
                                     new QTableWidgetItem(a_di_list[i]->get_block_data().name.isEmpty() ?
                                                              "无" :
                                                              a_di_list[i]->get_block_data().name));
                input_table->setItem(a_row, 4, new QTableWidgetItem("无"));
            }
        }
        if (b_di_list[i]->get_block_data().is_used) {
            if (b_di_list[i]->get_block_data().safe_level == SAFE_LEVEL_CAT2) {
                input_table->setItem(
                    b_row, 2,
                    new QTableWidgetItem("单通道" + ns[a_di_list[i]->get_block_data().config_param_data.normal_state]));
                input_table->setItem(b_row, 3,
                                     new QTableWidgetItem(b_di_list[i]->get_block_data().name.isEmpty() ?
                                                              "无" :
                                                              b_di_list[i]->get_block_data().name));
                input_table->setItem(b_row, 4, new QTableWidgetItem("无"));
            }
        }
    }
    int di_end_row = input_table->rowCount();
    if (di_end_row > di_start_row) {
        input_table->setSpan(di_start_row, 0, di_end_row - di_start_row, 1);
        input_table->setItem(di_start_row, 0, new QTableWidgetItem("DI"));
    }
    /* encode */
    int                  qep_start_row = input_table->rowCount();
    QList<config_param*> qep_list      = mainwindow->config_view_class->config_photo_svg->qep_config_list;
    module_param_t*      safe_param    = mainwindow->config_view_class->get_module_param();
    uint16_t qep_pulse_num[4] = { safe_param->qep1_pulse_num, safe_param->qep2_pulse_num, safe_param->piqep1_pulse_num,
                                  safe_param->piqep2_pulse_num };
    uint16_t qep_distance_num[4] = { safe_param->qep1_distance, safe_param->qep2_distance, safe_param->piqep1_distance,
                                     safe_param->piqep2_distance };
    uint8_t  qep_sample[4]       = { safe_param->sqep_sample_interval, safe_param->sqep_sample_interval,
                              safe_param->pi_sqep_sample_interval, safe_param->pi_sqep_sample_interval };
    uint8_t  qep_full[4]         = { safe_param->sqep_allow_dif[0], safe_param->sqep_allow_dif[0],
                            safe_param->pi_qep_allow_dif[0], safe_param->pi_qep_allow_dif[0] };
    uint8_t  qep_actual[4]       = { safe_param->sqep_allow_dif[1], safe_param->sqep_allow_dif[1],
                              safe_param->pi_qep_allow_dif[1], safe_param->pi_qep_allow_dif[1] };
    for (int i = 0; i < qep_list.size(); i++) {
        if (qep_list[i]->get_block_data().is_used) {
            int row = input_table->rowCount();
            input_table->insertRow(input_table->rowCount());
            input_table->setItem(row, 1, new QTableWidgetItem(qep_list[i]->get_block_data().source_name));
            input_table->setItem(row, 2, new QTableWidgetItem(qep_list[i]->get_block_data().type_name));
            input_table->setItem(row, 3,
                                 new QTableWidgetItem(qep_list[i]->get_block_data().name.isEmpty() ?
                                                          "无" :
                                                          qep_list[i]->get_block_data().name));

            QString param = "采样间隔：" + QString::number(qep_sample[i]) + "ms\r\n互检满量程占比"
                            + QString::number(qep_full[i]) + "%\r\n互检实时数据占比" + QString::number(qep_actual[i])
                            + "%\r\n输出轴每转一圈，编码器输出" + QString::number(qep_pulse_num[i])
                            + "个脉冲\r\n输出轴每转一圈，机器运动" + QString::number(qep_distance_num[i]) + "mm";
            input_table->setItem(row, 4, new QTableWidgetItem(param));
        }
    }

    int qep_end_row = input_table->rowCount();
    if (qep_end_row > qep_start_row) {
        input_table->setSpan(qep_start_row, 0, qep_end_row - qep_start_row, 1);
        input_table->setItem(qep_start_row, 0, new QTableWidgetItem("QEP"));
    }
    /* pi */
    QList<config_param*> a_pi_list    = mainwindow->config_view_class->config_photo_svg->a_pi_config_list;
    QList<config_param*> b_pi_list    = mainwindow->config_view_class->config_photo_svg->b_pi_config_list;
    int                  pi_start_row = input_table->rowCount();
    for (int i = 0; i < a_pi_list.size(); i++) {
        int a_row = 0, b_row = 0;
        if (!a_pi_list[i]->get_block_data().is_used && !b_pi_list[i]->get_block_data().is_used) {
            continue;
        }
        if (a_pi_list[i]->get_block_data().is_used) {
            a_row = input_table->rowCount();
            input_table->insertRow(a_row);
            input_table->setItem(a_row, 1, new QTableWidgetItem(a_pi_list[i]->get_block_data().source_name + ".A"));
        }
        if (b_pi_list[i]->get_block_data().is_used) {
            b_row = input_table->rowCount();
            input_table->insertRow(input_table->rowCount());
            input_table->setItem(b_row, 1, new QTableWidgetItem(b_pi_list[i]->get_block_data().source_name + ".B"));
        }
        if (a_pi_list[i]->get_block_data().is_used) {
            if (a_pi_list[i]->get_block_data().safe_level == SAFE_LEVEL_CAT3) {
                input_table->setSpan(a_row, 2, 2, 1);
                input_table->setSpan(a_row, 3, 2, 1);
                input_table->setSpan(a_row, 4, 2, 1);
                input_table->setItem(a_row, 2, new QTableWidgetItem("双通道"));
                input_table->setItem(a_row, 3,
                                     new QTableWidgetItem(a_pi_list[i]->get_block_data().name.isEmpty() ?
                                                              "无" :
                                                              a_pi_list[i]->get_block_data().name));
                module_param_t* safe_param = mainwindow->config_view_class->get_module_param();
                QString         param      = "采样间隔：" + QString::number(safe_param->spi_sample_interval)
                                + "ms\r\n互检满量程占比： " + QString ::number(safe_param->spi_allow_dif[0])
                                + "%\r\n互检实时数据占比：" + QString::number(safe_param->spi_allow_dif[1]) + "%";
                input_table->setItem(a_row, 4, new QTableWidgetItem(param));
            } else {
                input_table->setItem(a_row, 2, new QTableWidgetItem("单通道"));
                input_table->setItem(a_row, 3,
                                     new QTableWidgetItem(a_pi_list[i]->get_block_data().name.isEmpty() ?
                                                              "无" :
                                                              a_pi_list[i]->get_block_data().name));
                module_param_t* safe_param = mainwindow->config_view_class->get_module_param();
                QString         param = "采样间隔：" + QString::number(safe_param->spi_sample_interval) + "ms";
                input_table->setItem(a_row, 4, new QTableWidgetItem(param));
            }
        }
        if (b_pi_list[i]->get_block_data().is_used) {
            if (b_pi_list[i]->get_block_data().safe_level == SAFE_LEVEL_CAT2) {
                input_table->setItem(b_row, 2, new QTableWidgetItem("单通道"));
                input_table->setItem(b_row, 3,
                                     new QTableWidgetItem(b_pi_list[i]->get_block_data().name.isEmpty() ?
                                                              "无" :
                                                              b_pi_list[i]->get_block_data().name));
                module_param_t* safe_param = mainwindow->config_view_class->get_module_param();
                QString         param = "采样间隔：" + QString::number(safe_param->spi_sample_interval) + "ms";
                input_table->setItem(b_row, 4, new QTableWidgetItem(param));
            }
        }
    }
    int pi_end_row = input_table->rowCount();
    if (pi_end_row > pi_start_row) {
        input_table->setSpan(pi_start_row, 0, pi_end_row - pi_start_row, 1);
        input_table->setItem(pi_start_row, 0, new QTableWidgetItem("PI"));
    }
    /* input table set */
    input_table->resizeColumnsToContents();
    input_table->resizeRowsToContents();
    input_table->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    input_table->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    int totalWidth = input_table->verticalHeader()->width();
    for (int i = 0; i < input_table->columnCount(); i++) {
        totalWidth += input_table->columnWidth(i);
    }
    int totalHeight = input_table->horizontalHeader()->height();
    for (int i = 0; i < input_table->rowCount(); i++) {
        totalHeight += input_table->rowHeight(i);
    }
    // input_table->setFixedSize(totalWidth + 2, totalHeight);
    if (input_table->rowCount() > 1) {
        input_table->setFixedWidth(totalWidth);
        input_table->setFixedHeight(totalHeight + 2);
        layout->addWidget(input_table, mline++, 0, 1, 3);
    }
    /* 标题2.2 */
    font.setPixelSize(13);
    QLabel* title2 = new QLabel("2.2 输出配置");
    title2->setFont(font);
    title2->setStyleSheet("color: rgb(0,127,195);");
    title_row.append(mline);
    layout->addWidget(title2, mline++, 0);
    /* 输出表格 */
    QStringList col_title2;
    col_title2 << "输出类型"
               << "接口标识"
               << "元件类型"
               << "用户标签"
               << "参数";
    QTableWidget* output_table = new QTableWidget;
    output_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    output_table->setSelectionMode(QAbstractItemView::NoSelection);
    output_table->verticalHeader()->setVisible(false);
    output_table->setFocusPolicy(Qt::NoFocus);
    output_table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    output_table->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    output_table->horizontalHeader()->setSectionsClickable(false);
    output_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    output_table->setStyleSheet("QHeaderView::section:horizontal {background: rgba(128,128,128,100); }"
                                "QTableWidget { background-color: transparent; gridline-color: black; } ");
    for (int i = 0; i < col_title2.size(); i++) {
        output_table->insertColumn(output_table->columnCount());
        QTableWidgetItem* item = new QTableWidgetItem(col_title2[i]);
        item->setFont(QFont("Arial", 11, QFont::Bold));
        output_table->setHorizontalHeaderItem(i, item);
    }
    /* relay */
    QList<config_param*> a_relay_list    = mainwindow->config_view_class->config_photo_svg->a_relay_config_list;
    QList<config_param*> b_relay_list    = mainwindow->config_view_class->config_photo_svg->b_relay_config_list;
    int                  relay_start_row = output_table->rowCount();
    for (int i = 0; i < a_relay_list.size(); i++) {
        int a_row = 0, b_row = 0;
        if (!a_relay_list[i]->get_block_data().is_used && !b_relay_list[i]->get_block_data().is_used) {
            continue;
        }
        if (a_relay_list[i]->get_block_data().is_used) {
            a_row = output_table->rowCount();
            output_table->insertRow(a_row);
            output_table->setItem(a_row, 1, new QTableWidgetItem(a_relay_list[i]->get_block_data().source_name + ".A"));
        }
        if (b_relay_list[i]->get_block_data().is_used) {
            b_row = output_table->rowCount();
            output_table->insertRow(output_table->rowCount());
            output_table->setItem(b_row, 1,
                                  new QTableWidgetItem(b_relay_list[i]->get_block_data().source_name
                                                       + ".B"
                                                         ""));
        }
        if (a_relay_list[i]->get_block_data().is_used) {
            if (a_relay_list[i]->get_block_data().safe_level == SAFE_LEVEL_CAT3) {
                output_table->setSpan(a_row, 2, 2, 1);
                output_table->setSpan(a_row, 3, 2, 1);
                output_table->setSpan(a_row, 4, 2, 1);
                output_table->setItem(a_row, 2, new QTableWidgetItem(a_relay_list[i]->get_block_data().type_name));
                output_table->setItem(a_row, 3,
                                      new QTableWidgetItem(a_relay_list[i]->get_block_data().name.isEmpty() ?
                                                               "无" :
                                                               a_relay_list[i]->get_block_data().name));
                output_table->setItem(a_row, 4, new QTableWidgetItem("无"));
            } else {
                output_table->setItem(a_row, 2, new QTableWidgetItem(a_relay_list[i]->get_block_data().type_name));
                output_table->setItem(a_row, 3,
                                      new QTableWidgetItem(a_relay_list[i]->get_block_data().name.isEmpty() ?
                                                               "无" :
                                                               a_relay_list[i]->get_block_data().name));
                output_table->setItem(a_row, 4, new QTableWidgetItem("无"));
            }
        }
        if (b_relay_list[i]->get_block_data().is_used) {
            if (b_relay_list[i]->get_block_data().safe_level == SAFE_LEVEL_CAT2) {
                output_table->setItem(b_row, 2, new QTableWidgetItem(a_relay_list[i]->get_block_data().type_name));
                output_table->setItem(b_row, 3,
                                      new QTableWidgetItem(b_relay_list[i]->get_block_data().name.isEmpty() ?
                                                               "无" :
                                                               b_relay_list[i]->get_block_data().name));
                output_table->setItem(b_row, 4, new QTableWidgetItem("无"));
            }
        }
    }
    int relay_end_row = output_table->rowCount();
    if (relay_end_row > relay_start_row) {
        output_table->setSpan(relay_start_row, 0, relay_end_row - relay_start_row, 1);
        output_table->setItem(relay_start_row, 0, new QTableWidgetItem("Relay"));
    }
    /* mos */
    QList<config_param*> mos_list      = mainwindow->config_view_class->config_photo_svg->mos_config_list;
    int                  mos_start_row = output_table->rowCount();
    for (int i = 0; i < mos_list.size(); i++) {
        int row = output_table->rowCount();
        if (mos_list[i]->get_block_data().is_used) {
            output_table->insertRow(output_table->rowCount());
            output_table->setItem(row, 1, new QTableWidgetItem(mos_list[i]->get_block_data().source_name));
            output_table->setItem(row, 2, new QTableWidgetItem(mos_list[i]->get_block_data().type_name));
            output_table->setItem(row, 3,
                                  new QTableWidgetItem(mos_list[i]->get_block_data().name.isEmpty() ?
                                                           "无" :
                                                           mos_list[i]->get_block_data().name));
            output_table->setItem(row, 4, new QTableWidgetItem("无"));
        }
    }
    int mos_end_row = output_table->rowCount();
    if (mos_end_row > mos_start_row) {
        output_table->setSpan(mos_start_row, 0, mos_end_row - mos_start_row, 1);
        output_table->setItem(mos_start_row, 0, new QTableWidgetItem("MOS"));
    }
    /* output table set */
    output_table->resizeColumnsToContents();
    output_table->resizeRowsToContents();
    output_table->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    output_table->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    int totalWidth1 = output_table->verticalHeader()->width();
    for (int i = 0; i < output_table->columnCount(); ++i) {
        totalWidth1 += output_table->columnWidth(i);
    }
    int totalHeight1 = output_table->horizontalHeader()->height();
    for (int i = 0; i < output_table->rowCount(); ++i) {
        totalHeight1 += output_table->rowHeight(i);
    }
    if (output_table->rowCount() > 1) {
        output_table->setFixedWidth(totalWidth1);
        output_table->setFixedHeight(totalHeight1 + 2);
        layout->addWidget(output_table, mline++, 0, 1, 3);
    }
    *line = mline;
}

void project_report::safety_param_creat(QGridLayout* layout, int* line)
{
    int   mline = *line;
    QFont font;
    font.setBold(true);
    font.setFamily("Arial");
    font.setPixelSize(15);
    /* 标题 */
    QLabel* project_title = new QLabel("3 系统安全参数");
    project_title->setFont(font);
    project_title->setStyleSheet("color: rgb(0,127,195);");
    title_row.append(mline);
    layout->addWidget(project_title, mline++, 0);
    module_param_t* safe_param = mainwindow->config_view_class->get_module_param();
    /* 表格 */
    QTableWidget* table = new QTableWidget;
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionMode(QAbstractItemView::NoSelection);
    table->verticalHeader()->setVisible(false);
    table->horizontalHeader()->setVisible(false);
    table->setFocusPolicy(Qt::NoFocus);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    table->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionsClickable(false);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    table->setStyleSheet("QTableWidget { border: none; gridline-color: black; }");
    table->setColumnCount(2);
    /* 同步参数 */
    int row = 0;
    table->insertRow(table->rowCount());
    table->setSpan(row, 0, 1, 2);
    QTableWidgetItem* syncitem = new QTableWidgetItem("MCU同步互检");
    syncitem->setFont(QFont("Arial", 11, QFont::Bold));
    syncitem->setBackgroundColor(QColor(128, 128, 128, 100));
    table->setItem(row++, 0, syncitem);
    table->insertRow(table->rowCount());
    table->setItem(row, 0, new QTableWidgetItem("同步互检间隔"));
    table->setItem(row++, 1,
                   new QTableWidgetItem(QString::number(safe_param->check_factor)
                                        + " * 25 = " + QString::number(safe_param->check_factor * 25) + "ms"));
    /* can参数 */
    table->insertRow(table->rowCount());
    table->setSpan(row, 0, 1, 2);
    QTableWidgetItem* canitem = new QTableWidgetItem("CANopen参数");
    canitem->setFont(QFont("Arial", 11, QFont::Bold));
    canitem->setBackgroundColor(QColor(128, 128, 128, 100));
    table->setItem(row++, 0, canitem);
    table->insertRow(table->rowCount());
    table->setItem(row, 0, new QTableWidgetItem("波特率"));
    table->setItem(row++, 1, new QTableWidgetItem(QString::number(safe_param->can_baudrate)));
    table->insertRow(table->rowCount());
    table->setItem(row, 0, new QTableWidgetItem("从机A节点ID"));
    table->setItem(row++, 1, new QTableWidgetItem(QString::number(safe_param->can_slave_nodeID_A)));
    table->insertRow(table->rowCount());
    table->setItem(row, 0, new QTableWidgetItem("从机B节点ID"));
    table->setItem(row++, 1, new QTableWidgetItem(QString::number(safe_param->can_slave_nodeID_B)));
    table->insertRow(table->rowCount());
    table->setItem(row, 0, new QTableWidgetItem("主机节点ID"));
    table->setItem(row++, 1, new QTableWidgetItem(QString::number(safe_param->can_master_nodeID)));
    table->insertRow(table->rowCount());
    table->setItem(row, 0, new QTableWidgetItem("主机心跳间隔"));
    table->setItem(row++, 1, new QTableWidgetItem(QString::number(safe_param->can_hb_consumer_gap) + "ms"));
    table->insertRow(table->rowCount());
    table->setItem(row, 0, new QTableWidgetItem("从机心跳间隔"));
    table->setItem(row++, 1, new QTableWidgetItem(QString::number(safe_param->can_hb_producer_gap) + "ms"));
    table->insertRow(table->rowCount());
    table->setItem(row, 0, new QTableWidgetItem("PDO数据发送间隔"));
    table->setItem(row++, 1, new QTableWidgetItem(QString::number(safe_param->can_pdo_time_gap) + "ms"));
    table->resizeColumnsToContents();
    table->resizeRowsToContents();
    int totalWidth = table->verticalHeader()->width();
    for (int i = 0; i < table->columnCount(); ++i) {
        totalWidth += table->columnWidth(i);
    }
    int totalHeight = table->horizontalHeader()->height();
    for (int i = 0; i < table->rowCount(); ++i) {
        totalHeight += table->rowHeight(i);
    }
    layout->addWidget(table, mline++, 0);
    table->setFixedWidth(totalWidth + 2);
    table->setFixedHeight(totalHeight);
    *line = mline;
}
#define AI_MAP_HEIGHT    (ELEMENT_DISTANCE * 4)
#define DI_MAP_HEIGHT    (ELEMENT_DISTANCE * 8)
#define PI_MAP_HEIGHT    (ELEMENT_DISTANCE * 4)
#define QEP_MAP_HEIGHT   (ELEMENT_DISTANCE * 3)
#define RELAY_MAP_HEIGHT (ELEMENT_DISTANCE * 8)
#define MOS_MAP_HEIGHT   (ELEMENT_DISTANCE * 4)
#define MAP_WIDTH        (600)
#define POINT_LEFT_X     (ELEMENT_X - ELEMENT_POINT_HALFWIDTH)
#define POINT_RIGHT_X    (ELEMENT_X + ELEMENT_POINT_HALFWIDTH)
#define ELEMENT_PIXMAP_X (ELEMENT_X + 30)
#define POWER_X          (ELEMENT_PIXMAP_X + ELEMENT_DISTANCE + 30)

QPixmap project_report::wiring_basemap_baseinput_creat(safe_level_e safety_level, QPixmap element_pixmap, QString name,
                                                       bool used, bool power, source_mcu_e channel)
{
    QString ch = "A";
    if (channel == SOURCE_MCU_B) {
        ch = "B";
    }
    QPixmap    power_map("://configphoto/24V.ico");
    QTransform transform;
    transform.rotate(90);
    power_map = power_map.transformed(transform);
    QPixmap pixmap(MAP_WIDTH, ELEMENT_DISTANCE * 2);
    if (safety_level == SAFE_LEVEL_CAT2) {
        pixmap = pixmap.scaled(MAP_WIDTH, ELEMENT_DISTANCE);
    }
    int text_width = name.size() * 10;
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setPen(QPen(Qt::black, 3));
    painter.setBrush(QBrush(QColor(127, 255, 212)));
    if (safety_level == SAFE_LEVEL_CAT2) {
        painter.drawEllipse(
            QRect(POINT_LEFT_X, 40 - ELEMENT_POINT_HALFWIDTH, ELEMENT_POINT_WIDTH, ELEMENT_POINT_WIDTH));
        painter.drawText(
            QRect(POINT_LEFT_X - text_width, 40 - ELEMENT_POINT_HALFWIDTH, text_width, ELEMENT_POINT_WIDTH),
            name + "." + ch);
        if (used) {
            painter.setPen(Qt::NoPen);
            painter.setBrush(QBrush(QColor(128, 128, 128, 50)));
            painter.drawRect(QRect(ELEMENT_X, 10, pixmap.width() / 2, 60));
            painter.setPen(QPen(Qt::black, LINE_WIDTH));
            painter.drawPixmap(ELEMENT_PIXMAP_X, 0, ELEMENT_DISTANCE, ELEMENT_DISTANCE, element_pixmap);
            painter.drawLine(QPoint(POINT_RIGHT_X, 40 - LINE_HALFWIDTH), QPoint(ELEMENT_PIXMAP_X, 40 - LINE_HALFWIDTH));
            /* 画电源 */
            if (power) {
                painter.drawPixmap(POWER_X, 20, ELEMENT_DISTANCE / 2, ELEMENT_DISTANCE / 2, power_map);
                painter.drawLine(QPoint(ELEMENT_PIXMAP_X + ELEMENT_DISTANCE, 40 - LINE_HALFWIDTH),
                                 QPoint(POWER_X, 40 - LINE_HALFWIDTH));
            }
        }
    } else {
        painter.drawEllipse(
            QRect(POINT_LEFT_X, 60 - ELEMENT_POINT_HALFWIDTH, ELEMENT_POINT_WIDTH, ELEMENT_POINT_WIDTH));
        painter.drawEllipse(
            QRect(POINT_LEFT_X, 100 - ELEMENT_POINT_HALFWIDTH, ELEMENT_POINT_WIDTH, ELEMENT_POINT_WIDTH));
        painter.drawText(
            QRect(POINT_LEFT_X - text_width, 60 - ELEMENT_POINT_HALFWIDTH, text_width, ELEMENT_POINT_WIDTH),
            name + ".A");
        painter.drawText(
            QRect(POINT_LEFT_X - text_width, 100 - ELEMENT_POINT_HALFWIDTH, text_width, ELEMENT_POINT_WIDTH),
            name + ".B");
        if (used) {
            painter.setPen(Qt::NoPen);
            painter.setBrush(QBrush(QColor(128, 128, 128, 50)));
            painter.drawRect(QRect(ELEMENT_X, 10, pixmap.width() / 2, 140));
            painter.drawPixmap(ELEMENT_PIXMAP_X, 40, ELEMENT_DISTANCE, ELEMENT_DISTANCE, element_pixmap);
            painter.setPen(QPen(Qt::black, LINE_WIDTH));
            painter.drawLine(QPoint(POINT_RIGHT_X, 60 - LINE_HALFWIDTH), QPoint(ELEMENT_PIXMAP_X, 60 - LINE_HALFWIDTH));
            painter.drawLine(QPoint(POINT_RIGHT_X, 100 - LINE_HALFWIDTH),
                             QPoint(ELEMENT_PIXMAP_X, 100 - LINE_HALFWIDTH));
            /* 画电源 */
            if (power) {
                painter.drawPixmap(POWER_X, 40, ELEMENT_DISTANCE / 2, ELEMENT_DISTANCE / 2, power_map);
                painter.drawPixmap(POWER_X, 80, ELEMENT_DISTANCE / 2, ELEMENT_DISTANCE / 2, power_map);
                painter.drawLine(QPoint(ELEMENT_PIXMAP_X + ELEMENT_DISTANCE, 60 - LINE_HALFWIDTH),
                                 QPoint(POWER_X, 60 - LINE_HALFWIDTH));
                painter.drawLine(QPoint(ELEMENT_PIXMAP_X + ELEMENT_DISTANCE, 100 - LINE_HALFWIDTH),
                                 QPoint(POWER_X, 100 - LINE_HALFWIDTH));
            }
        }
    }
    return pixmap;
}

QPixmap project_report::wiring_basemap_qep_creat(safe_level_e safety_level, QPixmap element_pixmap, QString name,
                                                 bool used)
{
    QPixmap pixmap(MAP_WIDTH, 240);
    if (safety_level == SAFE_LEVEL_CAT2) {
        pixmap = pixmap.scaled(MAP_WIDTH, 120);
    }
    int text_width  = name.size() * 10;
    int x[8]        = { 30, 50, 70, 90, 150, 170, 190, 210 };
    int cat2_center = (x[1] + x[2]) / 2;
    int cat3_center = (x[3] + x[4]) / 2;
    int cat3_up     = cat3_center - 20;
    int cat3_down   = cat3_center + 20;
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setPen(QPen(Qt::black, LINE_WIDTH));
    painter.setBrush(QBrush(QColor(127, 255, 212)));
    if (safety_level == SAFE_LEVEL_CAT2) {
        painter.drawEllipse(
            QRect(POINT_LEFT_X, x[0] - ELEMENT_POINT_HALFWIDTH, ELEMENT_POINT_WIDTH, ELEMENT_POINT_WIDTH));
        painter.drawEllipse(
            QRect(POINT_LEFT_X, x[1] - ELEMENT_POINT_HALFWIDTH, ELEMENT_POINT_WIDTH, ELEMENT_POINT_WIDTH));
        painter.drawEllipse(
            QRect(POINT_LEFT_X, x[2] - ELEMENT_POINT_HALFWIDTH, ELEMENT_POINT_WIDTH, ELEMENT_POINT_WIDTH));
        painter.drawEllipse(
            QRect(POINT_LEFT_X, x[3] - ELEMENT_POINT_HALFWIDTH, ELEMENT_POINT_WIDTH, ELEMENT_POINT_WIDTH));
        painter.drawText(
            QRect(POINT_LEFT_X - text_width, x[0] - ELEMENT_POINT_HALFWIDTH, text_width, ELEMENT_POINT_WIDTH),
            name + ".A+");
        painter.drawText(
            QRect(POINT_LEFT_X - text_width, x[1] - ELEMENT_POINT_HALFWIDTH, text_width, ELEMENT_POINT_WIDTH),
            name + ".A-");
        painter.drawText(
            QRect(POINT_LEFT_X - text_width, x[2] - ELEMENT_POINT_HALFWIDTH, text_width, ELEMENT_POINT_WIDTH),
            name + ".B+");
        painter.drawText(
            QRect(POINT_LEFT_X - text_width, x[3] - ELEMENT_POINT_HALFWIDTH, text_width, ELEMENT_POINT_WIDTH),
            name + ".B-");
        if (used) {
            painter.setPen(Qt::NoPen);
            painter.setBrush(QBrush(QColor(128, 128, 128, 50)));
            painter.drawRect(QRect(ELEMENT_X, 10, pixmap.width() / 2, 100));
            painter.setPen(QPen(Qt::black, LINE_WIDTH));
            painter.drawPixmap(ELEMENT_PIXMAP_X, cat2_center - ELEMENT_HALFDISTANCE, ELEMENT_DISTANCE, ELEMENT_DISTANCE,
                               element_pixmap);
            painter.drawLine(QPoint(POINT_RIGHT_X, x[0] - LINE_HALFWIDTH),
                             QPoint(ELEMENT_PIXMAP_X, x[0] - LINE_HALFWIDTH));
            painter.drawLine(QPoint(POINT_RIGHT_X, x[1] - LINE_HALFWIDTH),
                             QPoint(ELEMENT_PIXMAP_X, x[1] - LINE_HALFWIDTH));
            painter.drawLine(QPoint(POINT_RIGHT_X, x[2] - LINE_HALFWIDTH),
                             QPoint(ELEMENT_PIXMAP_X, x[2] - LINE_HALFWIDTH));
            painter.drawLine(QPoint(POINT_RIGHT_X, x[3] - LINE_HALFWIDTH),
                             QPoint(ELEMENT_PIXMAP_X, x[3] - LINE_HALFWIDTH));
            painter.drawLine(QPoint(ELEMENT_PIXMAP_X, x[0] - LINE_HALFWIDTH),
                             QPoint(ELEMENT_PIXMAP_X, x[3] - LINE_HALFWIDTH));
            // painter.drawLine(QPoint(ELEMENT_PIXMAP_X - 10, cat2_center), QPoint(ELEMENT_PIXMAP_X, cat2_center));
            painter.drawLine(QPoint(ELEMENT_PIXMAP_X, cat2_center - 10), QPoint(ELEMENT_PIXMAP_X, cat2_center + 5));
        }
    } else {
        painter.drawEllipse(
            QRect(POINT_LEFT_X, x[0] - ELEMENT_POINT_HALFWIDTH, ELEMENT_POINT_WIDTH, ELEMENT_POINT_WIDTH));
        painter.drawEllipse(
            QRect(POINT_LEFT_X, x[1] - ELEMENT_POINT_HALFWIDTH, ELEMENT_POINT_WIDTH, ELEMENT_POINT_WIDTH));
        painter.drawEllipse(
            QRect(POINT_LEFT_X, x[2] - ELEMENT_POINT_HALFWIDTH, ELEMENT_POINT_WIDTH, ELEMENT_POINT_WIDTH));
        painter.drawEllipse(
            QRect(POINT_LEFT_X, x[3] - ELEMENT_POINT_HALFWIDTH, ELEMENT_POINT_WIDTH, ELEMENT_POINT_WIDTH));
        painter.drawEllipse(
            QRect(POINT_LEFT_X, x[4] - ELEMENT_POINT_HALFWIDTH, ELEMENT_POINT_WIDTH, ELEMENT_POINT_WIDTH));
        painter.drawEllipse(
            QRect(POINT_LEFT_X, x[5] - ELEMENT_POINT_HALFWIDTH, ELEMENT_POINT_WIDTH, ELEMENT_POINT_WIDTH));
        painter.drawEllipse(
            QRect(POINT_LEFT_X, x[6] - ELEMENT_POINT_HALFWIDTH, ELEMENT_POINT_WIDTH, ELEMENT_POINT_WIDTH));
        painter.drawEllipse(
            QRect(POINT_LEFT_X, x[7] - ELEMENT_POINT_HALFWIDTH, ELEMENT_POINT_WIDTH, ELEMENT_POINT_WIDTH));
        painter.drawText(
            QRect(POINT_LEFT_X - text_width, x[0] - ELEMENT_POINT_HALFWIDTH, text_width, ELEMENT_POINT_WIDTH),
            name + "1.A+");
        painter.drawText(
            QRect(POINT_LEFT_X - text_width, x[1] - ELEMENT_POINT_HALFWIDTH, text_width, ELEMENT_POINT_WIDTH),
            name + "1.A-");
        painter.drawText(
            QRect(POINT_LEFT_X - text_width, x[2] - ELEMENT_POINT_HALFWIDTH, text_width, ELEMENT_POINT_WIDTH),
            name + "1.B+");
        painter.drawText(
            QRect(POINT_LEFT_X - text_width, x[3] - ELEMENT_POINT_HALFWIDTH, text_width, ELEMENT_POINT_WIDTH),
            name + "1.B-");
        painter.drawText(
            QRect(POINT_LEFT_X - text_width, x[4] - ELEMENT_POINT_HALFWIDTH, text_width, ELEMENT_POINT_WIDTH),
            name + "2.A+");
        painter.drawText(
            QRect(POINT_LEFT_X - text_width, x[5] - ELEMENT_POINT_HALFWIDTH, text_width, ELEMENT_POINT_WIDTH),
            name + "2.A-");
        painter.drawText(
            QRect(POINT_LEFT_X - text_width, x[6] - ELEMENT_POINT_HALFWIDTH, text_width, ELEMENT_POINT_WIDTH),
            name + "2.B+");
        painter.drawText(
            QRect(POINT_LEFT_X - text_width, x[7] - ELEMENT_POINT_HALFWIDTH, text_width, ELEMENT_POINT_WIDTH),
            name + "2.B-");
        if (used) {
            painter.setPen(Qt::NoPen);
            painter.setBrush(QBrush(QColor(128, 128, 128, 50)));
            painter.drawRect(QRect(ELEMENT_X, 10, pixmap.width() / 2, 220));
            painter.drawPixmap(ELEMENT_PIXMAP_X, cat3_center - ELEMENT_HALFDISTANCE, ELEMENT_DISTANCE, ELEMENT_DISTANCE,
                               element_pixmap);
            painter.setPen(QPen(Qt::black, LINE_WIDTH));
            painter.drawLine(QPoint(POINT_RIGHT_X, x[0] - LINE_HALFWIDTH),
                             QPoint(ELEMENT_PIXMAP_X - 10, x[0] - LINE_HALFWIDTH));
            painter.drawLine(QPoint(POINT_RIGHT_X, x[1] - LINE_HALFWIDTH),
                             QPoint(ELEMENT_PIXMAP_X - 10, x[1] - LINE_HALFWIDTH));
            painter.drawLine(QPoint(POINT_RIGHT_X, x[2] - LINE_HALFWIDTH),
                             QPoint(ELEMENT_PIXMAP_X - 10, x[2] - LINE_HALFWIDTH));
            painter.drawLine(QPoint(POINT_RIGHT_X, x[3] - LINE_HALFWIDTH),
                             QPoint(ELEMENT_PIXMAP_X - 10, x[3] - LINE_HALFWIDTH));
            painter.drawLine(QPoint(ELEMENT_PIXMAP_X - 10, x[0] - LINE_HALFWIDTH),
                             QPoint(ELEMENT_PIXMAP_X - 10, cat3_up - LINE_HALFWIDTH));
            painter.drawLine(QPoint(ELEMENT_PIXMAP_X - 10, cat3_up - LINE_HALFWIDTH),
                             QPoint(ELEMENT_PIXMAP_X, cat3_up - LINE_HALFWIDTH));
            painter.drawLine(QPoint(POINT_RIGHT_X, x[4] - LINE_HALFWIDTH),
                             QPoint(ELEMENT_PIXMAP_X - 10, x[4] - LINE_HALFWIDTH));
            painter.drawLine(QPoint(POINT_RIGHT_X, x[5] - LINE_HALFWIDTH),
                             QPoint(ELEMENT_PIXMAP_X - 10, x[5] - LINE_HALFWIDTH));
            painter.drawLine(QPoint(POINT_RIGHT_X, x[6] - LINE_HALFWIDTH),
                             QPoint(ELEMENT_PIXMAP_X - 10, x[6] - LINE_HALFWIDTH));
            painter.drawLine(QPoint(POINT_RIGHT_X, x[7] - LINE_HALFWIDTH),
                             QPoint(ELEMENT_PIXMAP_X - 10, x[7] - LINE_HALFWIDTH));
            painter.drawLine(QPoint(ELEMENT_PIXMAP_X - 10, cat3_down - LINE_HALFWIDTH),
                             QPoint(ELEMENT_PIXMAP_X - 10, x[7] - LINE_HALFWIDTH));
            painter.drawLine(QPoint(ELEMENT_PIXMAP_X - 10, cat3_down - LINE_HALFWIDTH),
                             QPoint(ELEMENT_PIXMAP_X, cat3_down - LINE_HALFWIDTH));
        }
    }
    return pixmap;
}

QPixmap project_report::wiring_basemap_relay_creat(safe_level_e safety_level, QString name, int contactors_num,
                                                   source_mcu_e channel, bool used)
{
    QString ch = ".A";
    if (channel == SOURCE_MCU_B) {
        ch = ".B";
    }
    QPixmap    power_map("://configphoto/24V.ico");
    QTransform transform;
    transform.rotate(90);
    power_map = power_map.transformed(transform);
    QPixmap gnd_map("://configphoto/GND.ico");
    gnd_map = gnd_map.transformed(transform);
    QPixmap contactors_map("://configphoto/single_contactors_relay.ico");
    QPixmap pixmap(MAP_WIDTH, ELEMENT_DISTANCE * 4);
    if (safety_level == SAFE_LEVEL_CAT2) {
        pixmap = pixmap.scaled(MAP_WIDTH, ELEMENT_DISTANCE * 2);
    }
    int y[4]       = { 40, 120, 200, 280 };
    int text_width = name.size() * 10;
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setPen(QPen(Qt::black, LINE_WIDTH));
    painter.setBrush(QBrush(QColor(255, 0, 255)));
    if (safety_level == SAFE_LEVEL_CAT2) {
        painter.drawEllipse(
            QRect(POINT_LEFT_X, y[0] - ELEMENT_POINT_HALFWIDTH, ELEMENT_POINT_WIDTH, ELEMENT_POINT_WIDTH));
        painter.drawEllipse(
            QRect(POINT_LEFT_X, y[1] - ELEMENT_POINT_HALFWIDTH, ELEMENT_POINT_WIDTH, ELEMENT_POINT_WIDTH));
        painter.drawText(
            QRect(POINT_LEFT_X - text_width, y[0] - ELEMENT_POINT_HALFWIDTH, text_width, ELEMENT_POINT_WIDTH),
            name + "-" + ch);
        painter.drawText(
            QRect(POINT_LEFT_X - text_width, y[1] - ELEMENT_POINT_HALFWIDTH, text_width, ELEMENT_POINT_WIDTH),
            name + "+" + ch);
        if (used) {
            painter.setPen(Qt::NoPen);
            painter.setBrush(QBrush(QColor(128, 128, 128, 50)));
            painter.drawRect(QRect(ELEMENT_X, 10, pixmap.width() / 2, 140));
            painter.setPen(QPen(Qt::black, LINE_WIDTH));
            /* 画负极接触器 */
            painter.drawPixmap(ELEMENT_PIXMAP_X, y[0] - ELEMENT_HALFDISTANCE, ELEMENT_DISTANCE, ELEMENT_DISTANCE,
                               contactors_map);
            painter.drawPixmap(POWER_X, y[0] - ELEMENT_HALFDISTANCE / 2, ELEMENT_HALFDISTANCE, ELEMENT_HALFDISTANCE,
                               gnd_map);
            painter.drawLine(QPoint(POINT_RIGHT_X, y[0] - LINE_HALFWIDTH),
                             QPoint(ELEMENT_PIXMAP_X, y[0] - LINE_HALFWIDTH));
            painter.drawLine(QPoint(ELEMENT_PIXMAP_X + ELEMENT_DISTANCE, y[0] - LINE_HALFWIDTH),
                             QPoint(POWER_X, y[0] - LINE_HALFWIDTH));
            /* 画正极电源 */
            painter.drawPixmap(ELEMENT_PIXMAP_X, y[1] - ELEMENT_HALFDISTANCE / 2, ELEMENT_HALFDISTANCE,
                               ELEMENT_HALFDISTANCE, power_map);
            painter.drawLine(QPoint(POINT_RIGHT_X, y[1] - LINE_HALFWIDTH),
                             QPoint(ELEMENT_PIXMAP_X, y[1] - LINE_HALFWIDTH));
        }
    } else {
        painter.drawEllipse(
            QRect(POINT_LEFT_X, y[0] - ELEMENT_POINT_HALFWIDTH, ELEMENT_POINT_WIDTH, ELEMENT_POINT_WIDTH));
        painter.drawEllipse(
            QRect(POINT_LEFT_X, y[1] - ELEMENT_POINT_HALFWIDTH, ELEMENT_POINT_WIDTH, ELEMENT_POINT_WIDTH));
        painter.drawEllipse(
            QRect(POINT_LEFT_X, y[2] - ELEMENT_POINT_HALFWIDTH, ELEMENT_POINT_WIDTH, ELEMENT_POINT_WIDTH));
        painter.drawEllipse(
            QRect(POINT_LEFT_X, y[3] - ELEMENT_POINT_HALFWIDTH, ELEMENT_POINT_WIDTH, ELEMENT_POINT_WIDTH));
        painter.drawText(
            QRect(POINT_LEFT_X - text_width, y[0] - ELEMENT_POINT_HALFWIDTH, text_width, ELEMENT_POINT_WIDTH),
            name + "-" + ".A");
        painter.drawText(
            QRect(POINT_LEFT_X - text_width, y[1] - ELEMENT_POINT_HALFWIDTH, text_width, ELEMENT_POINT_WIDTH),
            name + "+" + ".A");
        painter.drawText(
            QRect(POINT_LEFT_X - text_width, y[2] - ELEMENT_POINT_HALFWIDTH, text_width, ELEMENT_POINT_WIDTH),
            name + "-" + ".B");
        painter.drawText(
            QRect(POINT_LEFT_X - text_width, y[3] - ELEMENT_POINT_HALFWIDTH, text_width, ELEMENT_POINT_WIDTH),
            name + "+" + ".B");

        if (used) {
            painter.setPen(Qt::NoPen);
            painter.setBrush(QBrush(QColor(128, 128, 128, 50)));
            painter.drawRect(QRect(ELEMENT_X, 10, pixmap.width() / 2, 300));
            painter.setPen(QPen(Qt::black, LINE_WIDTH));
            if (contactors_num == 2) {
                /* 画负极接触器 */
                painter.drawPixmap(ELEMENT_PIXMAP_X, y[0] - ELEMENT_HALFDISTANCE, ELEMENT_DISTANCE, ELEMENT_DISTANCE,
                                   contactors_map);
                painter.drawPixmap(POWER_X, y[0] - ELEMENT_HALFDISTANCE / 2, ELEMENT_HALFDISTANCE, ELEMENT_HALFDISTANCE,
                                   gnd_map);
                painter.drawLine(QPoint(POINT_RIGHT_X, y[0] - LINE_HALFWIDTH),
                                 QPoint(ELEMENT_PIXMAP_X, y[0] - LINE_HALFWIDTH));
                painter.drawLine(QPoint(ELEMENT_PIXMAP_X + ELEMENT_DISTANCE, y[0] - LINE_HALFWIDTH),
                                 QPoint(POWER_X, y[0] - LINE_HALFWIDTH));
                /* 画正极电源 */
                painter.drawPixmap(ELEMENT_PIXMAP_X, y[1] - ELEMENT_HALFDISTANCE / 2, ELEMENT_HALFDISTANCE,
                                   ELEMENT_HALFDISTANCE, power_map);
                painter.drawLine(QPoint(POINT_RIGHT_X, y[1] - LINE_HALFWIDTH),
                                 QPoint(ELEMENT_PIXMAP_X, y[1] - LINE_HALFWIDTH));
                /* 画负极接触器 */
                painter.drawPixmap(ELEMENT_PIXMAP_X, y[2] - ELEMENT_HALFDISTANCE, ELEMENT_DISTANCE, ELEMENT_DISTANCE,
                                   contactors_map);
                painter.drawPixmap(POWER_X, y[2] - ELEMENT_HALFDISTANCE / 2, ELEMENT_HALFDISTANCE, ELEMENT_HALFDISTANCE,
                                   gnd_map);
                painter.drawLine(QPoint(POINT_RIGHT_X, y[2] - LINE_HALFWIDTH),
                                 QPoint(ELEMENT_PIXMAP_X, y[2] - LINE_HALFWIDTH));
                painter.drawLine(QPoint(ELEMENT_PIXMAP_X + ELEMENT_DISTANCE, y[2] - LINE_HALFWIDTH),
                                 QPoint(POWER_X, y[2] - LINE_HALFWIDTH));
                /* 画正极电源 */
                painter.drawPixmap(ELEMENT_PIXMAP_X, y[3] - ELEMENT_HALFDISTANCE / 2, ELEMENT_HALFDISTANCE,
                                   ELEMENT_HALFDISTANCE, power_map);
                painter.drawLine(QPoint(POINT_RIGHT_X, y[3] - LINE_HALFWIDTH),
                                 QPoint(ELEMENT_PIXMAP_X, y[3] - LINE_HALFWIDTH));
            } else {
                /* 画负极接触器 */
                painter.drawPixmap(ELEMENT_PIXMAP_X, y[0] - ELEMENT_HALFDISTANCE, ELEMENT_DISTANCE, ELEMENT_DISTANCE,
                                   contactors_map);
                painter.drawPixmap(POWER_X, y[0] - ELEMENT_HALFDISTANCE / 2, ELEMENT_HALFDISTANCE, ELEMENT_HALFDISTANCE,
                                   gnd_map);
                painter.drawLine(QPoint(POINT_RIGHT_X, y[0] - LINE_HALFWIDTH),
                                 QPoint(ELEMENT_PIXMAP_X, y[0] - LINE_HALFWIDTH));
                painter.drawLine(QPoint(ELEMENT_PIXMAP_X + ELEMENT_DISTANCE, y[0] - LINE_HALFWIDTH),
                                 QPoint(POWER_X, y[0] - LINE_HALFWIDTH));
                /* 画续接线 */
                painter.drawLine(QPoint(POINT_RIGHT_X, y[1] - LINE_HALFWIDTH),
                                 QPoint(ELEMENT_PIXMAP_X, y[1] - LINE_HALFWIDTH));
                painter.drawLine(QPoint(POINT_RIGHT_X, y[2] - LINE_HALFWIDTH),
                                 QPoint(ELEMENT_PIXMAP_X, y[2] - LINE_HALFWIDTH));
                painter.drawLine(QPoint(ELEMENT_PIXMAP_X, y[1] - LINE_HALFWIDTH),
                                 QPoint(ELEMENT_PIXMAP_X, y[2] - LINE_HALFWIDTH));
                /* 画正极电源 */
                painter.drawPixmap(ELEMENT_PIXMAP_X, y[3] - ELEMENT_HALFDISTANCE / 2, ELEMENT_HALFDISTANCE,
                                   ELEMENT_HALFDISTANCE, power_map);
                painter.drawLine(QPoint(POINT_RIGHT_X, y[3] - LINE_HALFWIDTH),
                                 QPoint(ELEMENT_PIXMAP_X, y[3] - LINE_HALFWIDTH));
            }
        }
    }
    return pixmap;
}

QPixmap project_report::wiring_basemap_mos_creat(QString name, QString pixmap, bool used)
{
    Q_UNUSED(pixmap);
    bool    draw_gnd = true;
    QString map      = "://configphoto/single_contactors_relay.ico";
    //    if (pixmap == "://configphoto/double_contactors_relay.ico"
    //        || pixmap == "://configphoto/single_contactors_relay.ico") {
    //        draw_gnd = true;
    //        map      = "://configphoto/single_contactors_relay.ico";
    //    }
    QPixmap    epixmap(map);
    QTransform transform;
    transform.rotate(90);
    QPixmap gnd_map("://configphoto/GND.ico");
    gnd_map = gnd_map.transformed(transform);
    QPixmap mpixmap(MAP_WIDTH, 80);
    mpixmap.fill(Qt::transparent);
    QPainter painter(&mpixmap);
    painter.setPen(QPen(Qt::black, LINE_WIDTH));
    painter.setBrush(QBrush(QColor(255, 0, 255)));
    painter.drawEllipse(QRect(POINT_LEFT_X, 40 - ELEMENT_POINT_HALFWIDTH, ELEMENT_POINT_WIDTH, ELEMENT_POINT_WIDTH));
    painter.drawText(QRect(POINT_LEFT_X - 30, 40 - ELEMENT_POINT_HALFWIDTH, 30, ELEMENT_POINT_WIDTH), name);
    if (used) {
        painter.setPen(Qt::NoPen);
        painter.setBrush(QBrush(QColor(128, 128, 128, 50)));
        painter.drawRect(QRect(ELEMENT_X, 10, mpixmap.width() / 2, 60));
        painter.setPen(QPen(Qt::black, LINE_WIDTH));
        painter.drawPixmap(ELEMENT_PIXMAP_X, 40 - ELEMENT_HALFDISTANCE, ELEMENT_DISTANCE, ELEMENT_DISTANCE, epixmap);
        painter.drawLine(QPoint(POINT_RIGHT_X, 40 - LINE_HALFWIDTH), QPoint(ELEMENT_PIXMAP_X, 40 - LINE_HALFWIDTH));
        if (draw_gnd) {
            painter.drawPixmap(POWER_X, 40 - ELEMENT_HALFDISTANCE / 2, ELEMENT_HALFDISTANCE, ELEMENT_HALFDISTANCE,
                               gnd_map);
            painter.drawLine(QPoint(ELEMENT_PIXMAP_X + ELEMENT_DISTANCE, 40 - LINE_HALFWIDTH),
                             QPoint(POWER_X, 40 - LINE_HALFWIDTH));
        }
    }
    return mpixmap;
}

void project_report::wiring_diagram_creat(QGridLayout* layout, int* line)
{
#define ELEMENT_MAP_X  (230)
#define ELEMENT_LINE_X (ELEMENT_X + ELEMENT_POINT_HALFWIDTH)

    module_param_t* safe_param = mainwindow->config_view_class->get_module_param();
    int             mline      = *line;
    QFont           font;
    font.setBold(true);
    font.setFamily("Arial");
    font.setPixelSize(15);
    /* 标题 */
    QLabel* project_title = new QLabel("4 接线参考图");
    project_title->setFont(font);
    project_title->setStyleSheet("color: rgb(0,127,195);");
    title_row.append(mline);
    layout->addWidget(project_title, mline++, 0);
    font.setPixelSize(12);
    QLabel* tips = new QLabel("接线图仅供参考，实际接线请分析实际使用的元器件类型");
    tips->setFont(font);
    layout->addWidget(tips, mline++, 0);
    /* AI */
    font.setPixelSize(13);
    QLabel* ai_title = new QLabel("4.1 AI接线图");
    ai_title->setFont(font);
    ai_title->setStyleSheet("color: rgb(0,127,195);");
    title_row.append(mline);
    layout->addWidget(ai_title, mline++, 0);
    QLabel* ai_label = new QLabel;
    QPixmap ai_pixmap(MAP_WIDTH, AI_MAP_HEIGHT);
    ai_pixmap.fill(Qt::transparent);
    QPainter aipainter(&ai_pixmap);
    aipainter.setPen(QPen(Qt::black, 3));
    aipainter.drawRect(QRect(0, 0, 200, AI_MAP_HEIGHT));
    QList<config_param*> a_ai_list = mainwindow->config_view_class->config_photo_svg->a_ai_config_list;
    QList<config_param*> b_ai_list = mainwindow->config_view_class->config_photo_svg->b_ai_config_list;
    for (int i = 0; i < a_ai_list.size(); i++) {
        if (((safe_param->work_state.work_state_byte >> (4 + i)) & 0x01) == 0) {
            if (a_ai_list[i]->get_block_data().is_used) {
                QPixmap pixmap(a_ai_list[i]->get_block_data().pixmap);
                if (a_ai_list[i]->get_block_data().safe_level == SAFE_LEVEL_CAT3) {
                    aipainter.drawPixmap(
                        0, i * 160, MAP_WIDTH, 160,
                        wiring_basemap_baseinput_creat(SAFE_LEVEL_CAT3, pixmap, "AI" + QString::number(i + 1)));
                } else {
                    aipainter.drawPixmap(
                        0, i * 160, MAP_WIDTH, 80,
                        wiring_basemap_baseinput_creat(SAFE_LEVEL_CAT2, pixmap, "AI" + QString::number(i + 1)));
                }
            } else {
                QPixmap pixmap(a_ai_list[i]->get_block_data().pixmap);
                aipainter.drawPixmap(
                    0, i * 160, MAP_WIDTH, 80,
                    wiring_basemap_baseinput_creat(SAFE_LEVEL_CAT2, pixmap, "AI" + QString::number(i + 1), false));
            }
            if (b_ai_list[i]->get_block_data().is_used) {
                QPixmap pixmap(b_ai_list[i]->get_block_data().pixmap);
                if (b_ai_list[i]->get_block_data().safe_level == SAFE_LEVEL_CAT2) {
                    aipainter.drawPixmap(0, 80 + i * 160, MAP_WIDTH, 80,
                                         wiring_basemap_baseinput_creat(SAFE_LEVEL_CAT2, pixmap,
                                                                        "AI" + QString::number(i + 1), true, false,
                                                                        SOURCE_MCU_B));
                }
            } else {
                QPixmap pixmap(b_ai_list[i]->get_block_data().pixmap);
                aipainter.drawPixmap(0, 80 + i * 160, MAP_WIDTH, 80,
                                     wiring_basemap_baseinput_creat(SAFE_LEVEL_CAT2, pixmap,
                                                                    "AI" + QString::number(i + 1), false, false,
                                                                    SOURCE_MCU_B));
            }
        }
    }
    QList<config_param*> a_di_list = mainwindow->config_view_class->config_photo_svg->a_di_config_list;
    QList<config_param*> b_di_list = mainwindow->config_view_class->config_photo_svg->b_di_config_list;
    for (int i = 0; i < 2; i++) {
        QString name = "DI" + QString::number(i + 9) + "(AI" + QString::number(i + 1) + ")";
        if (((safe_param->work_state.work_state_byte >> (4 + i)) & 0x01) != 0) {
            if (a_di_list[8 + i]->get_block_data().is_used) {
                QPixmap pixmap(a_di_list[8 + i]->get_block_data().pixmap);
                if (a_di_list[8 + i]->get_block_data().safe_level == SAFE_LEVEL_CAT3) {
                    aipainter.drawPixmap(0, i * 160, MAP_WIDTH, 160,
                                         wiring_basemap_baseinput_creat(SAFE_LEVEL_CAT3, pixmap, name, true, true));
                } else {
                    aipainter.drawPixmap(0, i * 160, MAP_WIDTH, 80,
                                         wiring_basemap_baseinput_creat(SAFE_LEVEL_CAT2, pixmap, name, true, true));
                }
            } else {
                QPixmap pixmap(a_di_list[8 + i]->get_block_data().pixmap);
                aipainter.drawPixmap(0, i * 160, MAP_WIDTH, 80,
                                     wiring_basemap_baseinput_creat(SAFE_LEVEL_CAT2, pixmap, name, false));
            }
            if (b_di_list[8 + i]->get_block_data().is_used) {
                if (b_di_list[8 + i]->get_block_data().safe_level == SAFE_LEVEL_CAT2) {
                    QPixmap pixmap(b_di_list[8 + i]->get_block_data().pixmap);
                    aipainter.drawPixmap(
                        0, 80 + i * 160, MAP_WIDTH, 80,
                        wiring_basemap_baseinput_creat(SAFE_LEVEL_CAT2, pixmap, name, true, true, SOURCE_MCU_B));
                }
            } else {
                QPixmap pixmap(b_di_list[8 + i]->get_block_data().pixmap);
                aipainter.drawPixmap(
                    0, 80 + i * 160, MAP_WIDTH, 80,
                    wiring_basemap_baseinput_creat(SAFE_LEVEL_CAT2, pixmap, name, false, false, SOURCE_MCU_B));
            }
        }
    }
    ai_label->setPixmap(ai_pixmap);
    layout->addWidget(ai_label, mline++, 0, 1, 3);
    /* DI */
    QLabel* di_title = new QLabel("4.2 DI接线图");
    di_title->setFont(font);
    di_title->setStyleSheet("color: rgb(0,127,195);");
    title_row.append(mline);
    layout->addWidget(di_title, mline++, 0);
    font.setPixelSize(11);
    QLabel* di1_title = new QLabel("4.2.1 DI1/DI2/DI3/DI4");
    di1_title->setFont(font);
    di1_title->setStyleSheet("color: rgb(0,127,195);");
    title_row.append(mline);
    layout->addWidget(di1_title, mline++, 0);
    QLabel* diup_label = new QLabel;
    QPixmap diup_pixmap(MAP_WIDTH, DI_MAP_HEIGHT);
    diup_pixmap.fill(Qt::transparent);
    QPainter diuppainter(&diup_pixmap);
    diuppainter.setPen(QPen(Qt::black, 3));
    diuppainter.drawRect(QRect(0, 0, 200, DI_MAP_HEIGHT));
    for (int i = 0; i < 4; i++) {
        QString name = "DI" + QString::number(i + 1);
        if (a_di_list[i]->get_block_data().is_used) {
            QPixmap pixmap(a_di_list[i]->get_block_data().pixmap);
            if (a_di_list[i]->get_block_data().safe_level == SAFE_LEVEL_CAT3) {
                diuppainter.drawPixmap(0, i * 160, MAP_WIDTH, 160,
                                       wiring_basemap_baseinput_creat(SAFE_LEVEL_CAT3, pixmap, name, true, true));
            } else {
                diuppainter.drawPixmap(0, i * 160, MAP_WIDTH, 80,
                                       wiring_basemap_baseinput_creat(SAFE_LEVEL_CAT2, pixmap, name, true, true));
            }
        } else {
            QPixmap pixmap(a_di_list[i]->get_block_data().pixmap);
            diuppainter.drawPixmap(0, i * 160, MAP_WIDTH, 80,
                                   wiring_basemap_baseinput_creat(SAFE_LEVEL_CAT2, pixmap, name, false));
        }
        if (b_di_list[i]->get_block_data().is_used) {
            if (b_di_list[i]->get_block_data().safe_level == SAFE_LEVEL_CAT2) {
                QPixmap pixmap(b_di_list[i]->get_block_data().pixmap);
                diuppainter.drawPixmap(
                    0, 80 + i * 160, MAP_WIDTH, 80,
                    wiring_basemap_baseinput_creat(SAFE_LEVEL_CAT2, pixmap, name, true, true, SOURCE_MCU_B));
            }
        } else {
            QPixmap pixmap(b_di_list[i]->get_block_data().pixmap);
            diuppainter.drawPixmap(
                0, 80 + i * 160, MAP_WIDTH, 80,
                wiring_basemap_baseinput_creat(SAFE_LEVEL_CAT2, pixmap, name, false, false, SOURCE_MCU_B));
        }
    }
    diup_label->setPixmap(diup_pixmap);
    layout->addWidget(diup_label, mline++, 0, 1, 3);
    QLabel* di2_title = new QLabel("4.2.1 DI5/DI6/DI7/DI8");
    di2_title->setFont(font);
    di2_title->setStyleSheet("color: rgb(0,127,195);");
    title_row.append(mline);
    layout->addWidget(di2_title, mline++, 0);
    QLabel* didown_label = new QLabel;
    QPixmap didwon_pixmap(MAP_WIDTH, DI_MAP_HEIGHT);
    didwon_pixmap.fill(Qt::transparent);
    QPainter didownpainter(&didwon_pixmap);
    didownpainter.setPen(QPen(Qt::black, 3));
    didownpainter.drawRect(QRect(0, 0, 200, DI_MAP_HEIGHT));
    for (int i = 0; i < 4; i++) {
        QString name = "DI" + QString::number(i + 5);
        if (a_di_list[i + 5]->get_block_data().is_used) {
            QPixmap pixmap(a_di_list[i + 5]->get_block_data().pixmap);
            if (a_di_list[i + 5]->get_block_data().safe_level == SAFE_LEVEL_CAT3) {
                didownpainter.drawPixmap(0, i * 160, MAP_WIDTH, 160,
                                         wiring_basemap_baseinput_creat(SAFE_LEVEL_CAT3, pixmap, name, true, true));
            } else {
                didownpainter.drawPixmap(0, i * 160, MAP_WIDTH, 80,
                                         wiring_basemap_baseinput_creat(SAFE_LEVEL_CAT2, pixmap, name, true, true));
            }
        } else {
            QPixmap pixmap(a_di_list[i + 5]->get_block_data().pixmap);
            didownpainter.drawPixmap(0, i * 160, MAP_WIDTH, 80,
                                     wiring_basemap_baseinput_creat(SAFE_LEVEL_CAT2, pixmap, name, false));
        }
        if (b_di_list[i + 5]->get_block_data().is_used) {
            if (b_di_list[i + 5]->get_block_data().safe_level == SAFE_LEVEL_CAT2) {
                QPixmap pixmap(b_di_list[i + 5]->get_block_data().pixmap);
                didownpainter.drawPixmap(
                    0, 80 + i * 160, MAP_WIDTH, 80,
                    wiring_basemap_baseinput_creat(SAFE_LEVEL_CAT2, pixmap, name, true, true, SOURCE_MCU_B));
            }
        } else {
            QPixmap pixmap(b_di_list[i + 5]->get_block_data().pixmap);
            didownpainter.drawPixmap(
                0, 80 + i * 160, MAP_WIDTH, 80,
                wiring_basemap_baseinput_creat(SAFE_LEVEL_CAT2, pixmap, name, false, false, SOURCE_MCU_B));
        }
    }
    didown_label->setPixmap(didwon_pixmap);
    layout->addWidget(didown_label, mline++, 0, 1, 3);
    /* pi */
    font.setPixelSize(13);
    QLabel* pi_title = new QLabel("4.3 PI接线图");
    pi_title->setFont(font);
    pi_title->setStyleSheet("color: rgb(0,127,195);");
    title_row.append(mline);
    layout->addWidget(pi_title, mline++, 0);
    QLabel* pi_label = new QLabel;
    QPixmap pi_pixmap(MAP_WIDTH, PI_MAP_HEIGHT);
    pi_pixmap.fill(Qt::transparent);
    QList<config_param*> a_pi_list = mainwindow->config_view_class->config_photo_svg->a_pi_config_list;
    QList<config_param*> b_pi_list = mainwindow->config_view_class->config_photo_svg->b_pi_config_list;
    QList<config_param*> qep_list  = mainwindow->config_view_class->config_photo_svg->qep_config_list;
    if (safe_param->work_state.work_state_bit.pi1_work_state_bit == PI_AF_QEP) {
        pi_pixmap.scaled(MAP_WIDTH, QEP_MAP_HEIGHT);
        QPainter pipainter(&pi_pixmap);
        pipainter.setPen(QPen(Qt::black, 3));
        pipainter.drawRect(QRect(0, 0, 200, QEP_MAP_HEIGHT));
        QString name = "PIQEP";
        if (qep_list[2]->get_block_data().is_used) {
            QPixmap pixmap(qep_list[2]->get_block_data().pixmap);
            if (qep_list[2]->get_block_data().safe_level == SAFE_LEVEL_CAT3) {
                pipainter.drawPixmap(0, 0, MAP_WIDTH, 240, wiring_basemap_qep_creat(SAFE_LEVEL_CAT3, pixmap, name));
            } else {
                name = "PIQEP1";
                pipainter.drawPixmap(0, 0, MAP_WIDTH, 120, wiring_basemap_qep_creat(SAFE_LEVEL_CAT2, pixmap, name));
            }
        } else {
            QPixmap pixmap(qep_list[2]->get_block_data().pixmap);
            name = "PIQEP1";
            pipainter.drawPixmap(0, 0, MAP_WIDTH, 120, wiring_basemap_qep_creat(SAFE_LEVEL_CAT2, pixmap, name, false));
        }
        if (qep_list[3]->get_block_data().is_used) {
            QPixmap pixmap(qep_list[3]->get_block_data().pixmap);
            if (qep_list[3]->get_block_data().safe_level == SAFE_LEVEL_CAT2) {
                name = "PIQEP2";
                pipainter.drawPixmap(0, 120, MAP_WIDTH, 120, wiring_basemap_qep_creat(SAFE_LEVEL_CAT2, pixmap, name));
            }
        } else {
            QPixmap pixmap(qep_list[3]->get_block_data().pixmap);
            name = "PIQEP2";
            pipainter.drawPixmap(0, 120, MAP_WIDTH, 120,
                                 wiring_basemap_qep_creat(SAFE_LEVEL_CAT2, pixmap, name, false));
        }
    } else {
        QPainter pipainter(&pi_pixmap);
        pipainter.setPen(QPen(Qt::black, 3));
        pipainter.drawRect(QRect(0, 0, 200, PI_MAP_HEIGHT));
        for (int i = 0; i < a_pi_list.size(); i++) {
            QString name = "PI" + QString::number(i + 1);
            if (((safe_param->work_state.work_state_byte >> (2 * i)) & 0x03) == PI_AF_PI) {
                if (a_pi_list[i]->get_block_data().is_used) {
                    QPixmap pixmap(a_pi_list[i]->get_block_data().pixmap);
                    if (a_pi_list[i]->get_block_data().safe_level == SAFE_LEVEL_CAT3) {
                        pipainter.drawPixmap(0, i * 160, MAP_WIDTH, 160,
                                             wiring_basemap_baseinput_creat(SAFE_LEVEL_CAT3, pixmap, name));
                    } else {
                        pipainter.drawPixmap(0, i * 160, MAP_WIDTH, 80,
                                             wiring_basemap_baseinput_creat(SAFE_LEVEL_CAT2, pixmap, name));
                    }
                } else {
                    QPixmap pixmap(a_pi_list[i]->get_block_data().pixmap);
                    pipainter.drawPixmap(0, i * 160, MAP_WIDTH, 80,
                                         wiring_basemap_baseinput_creat(SAFE_LEVEL_CAT2, pixmap, name, false));
                }
                if (b_pi_list[i]->get_block_data().is_used) {
                    QPixmap pixmap(b_ai_list[i]->get_block_data().pixmap);
                    if (b_pi_list[i]->get_block_data().safe_level == SAFE_LEVEL_CAT2) {
                        pipainter.drawPixmap(
                            0, 80 + i * 160, MAP_WIDTH, 80,
                            wiring_basemap_baseinput_creat(SAFE_LEVEL_CAT2, pixmap, name, true, false, SOURCE_MCU_B));
                    }
                } else {
                    QPixmap pixmap(b_pi_list[i]->get_block_data().pixmap);
                    pipainter.drawPixmap(
                        0, 80 + i * 160, MAP_WIDTH, 80,
                        wiring_basemap_baseinput_creat(SAFE_LEVEL_CAT2, pixmap, name, false, false, SOURCE_MCU_B));
                }
            } else if (((safe_param->work_state.work_state_byte >> (2 * i)) & 0x03) == PI_AF_DI) {
                QString name = "DI" + QString::number(i + 11) + "(PI" + QString::number(i + 1) + ")";
                if (a_di_list[10 + i]->get_block_data().is_used) {
                    QPixmap pixmap(a_di_list[10 + i]->get_block_data().pixmap);
                    if (a_di_list[10 + i]->get_block_data().safe_level == SAFE_LEVEL_CAT3) {
                        pipainter.drawPixmap(0, i * 160, MAP_WIDTH, 160,
                                             wiring_basemap_baseinput_creat(SAFE_LEVEL_CAT3, pixmap, name, true, true));
                    } else {
                        pipainter.drawPixmap(0, i * 160, MAP_WIDTH, 80,
                                             wiring_basemap_baseinput_creat(SAFE_LEVEL_CAT2, pixmap, name, true, true));
                    }
                } else {
                    QPixmap pixmap(a_di_list[10 + i]->get_block_data().pixmap);
                    pipainter.drawPixmap(0, i * 160, MAP_WIDTH, 80,
                                         wiring_basemap_baseinput_creat(SAFE_LEVEL_CAT2, pixmap, name, false));
                }
                if (b_di_list[10 + i]->get_block_data().is_used) {
                    if (b_di_list[10 + i]->get_block_data().safe_level == SAFE_LEVEL_CAT2) {
                        QPixmap pixmap(b_di_list[10 + i]->get_block_data().pixmap);
                        pipainter.drawPixmap(
                            0, 80 + i * 160, MAP_WIDTH, 80,
                            wiring_basemap_baseinput_creat(SAFE_LEVEL_CAT2, pixmap, name, true, true, SOURCE_MCU_B));
                    }
                } else {
                    QPixmap pixmap(b_di_list[10 + i]->get_block_data().pixmap);
                    pipainter.drawPixmap(
                        0, 80 + i * 160, MAP_WIDTH, 80,
                        wiring_basemap_baseinput_creat(SAFE_LEVEL_CAT2, pixmap, name, false, false, SOURCE_MCU_B));
                }
            }
        }
    }
    pi_label->setPixmap(pi_pixmap);
    layout->addWidget(pi_label, mline++, 0, 1, 3);
    /* qep */
    QLabel* qep_title = new QLabel("4.4 QEP接线图");
    qep_title->setFont(font);
    qep_title->setStyleSheet("color: rgb(0,127,195);");
    title_row.append(mline);
    layout->addWidget(qep_title, mline++, 0);
    QLabel* qep_label = new QLabel;
    QPixmap qep_pixmap(MAP_WIDTH, QEP_MAP_HEIGHT);
    qep_pixmap.fill(Qt::transparent);
    QPainter qeppainter(&qep_pixmap);
    qeppainter.setPen(QPen(Qt::black, 3));
    qeppainter.drawRect(QRect(0, 0, 200, QEP_MAP_HEIGHT));
    QString name = "QEP";
    if (qep_list[0]->get_block_data().is_used) {
        QPixmap pixmap(qep_list[0]->get_block_data().pixmap);
        if (qep_list[0]->get_block_data().safe_level == SAFE_LEVEL_CAT3) {
            qeppainter.drawPixmap(0, 0, MAP_WIDTH, 240, wiring_basemap_qep_creat(SAFE_LEVEL_CAT3, pixmap, name));
        } else {
            name = "QEP1";
            qeppainter.drawPixmap(0, 0, MAP_WIDTH, 120, wiring_basemap_qep_creat(SAFE_LEVEL_CAT2, pixmap, name));
        }
    } else {
        QPixmap pixmap(qep_list[0]->get_block_data().pixmap);
        name = "QEP1";
        qeppainter.drawPixmap(0, 0, MAP_WIDTH, 120, wiring_basemap_qep_creat(SAFE_LEVEL_CAT2, pixmap, name, false));
    }
    if (qep_list[1]->get_block_data().is_used) {
        QPixmap pixmap(qep_list[1]->get_block_data().pixmap);
        if (qep_list[1]->get_block_data().safe_level == SAFE_LEVEL_CAT2) {
            name = "QEP2";
            qeppainter.drawPixmap(0, 120, MAP_WIDTH, 120, wiring_basemap_qep_creat(SAFE_LEVEL_CAT2, pixmap, name));
        }
    } else {
        QPixmap pixmap(qep_list[1]->get_block_data().pixmap);
        name = "QEP2";
        qeppainter.drawPixmap(0, 120, MAP_WIDTH, 120, wiring_basemap_qep_creat(SAFE_LEVEL_CAT2, pixmap, name, false));
    }
    qep_label->setPixmap(qep_pixmap);
    layout->addWidget(qep_label, mline++, 0, 1, 3);
    /* relay */
    QLabel* relay_title = new QLabel("4.5 Relay接线图");
    relay_title->setFont(font);
    relay_title->setStyleSheet("color: rgb(0,127,195);");
    title_row.append(mline);
    layout->addWidget(relay_title, mline++, 0);
    QLabel* relay_label = new QLabel;
    QPixmap relay_pixmap(MAP_WIDTH, RELAY_MAP_HEIGHT);
    relay_pixmap.fill(Qt::transparent);
    QPainter relaypainter(&relay_pixmap);
    relaypainter.setPen(QPen(Qt::black, 3));
    relaypainter.drawRect(QRect(0, 0, 200, RELAY_MAP_HEIGHT));
    QList<config_param*> a_relay_list = mainwindow->config_view_class->config_photo_svg->a_relay_config_list;
    QList<config_param*> b_relay_list = mainwindow->config_view_class->config_photo_svg->b_relay_config_list;
    for (int i = 0; i < a_relay_list.size(); i++) {
        QString name = "Relay" + QString::number(i + 1);
        if (a_relay_list[i]->get_block_data().is_used) {
            if (a_relay_list[i]->get_block_data().safe_level == SAFE_LEVEL_CAT3) {
                int num = a_relay_list[i]->get_user_data().contactors_num;
                relaypainter.drawPixmap(0, i * 320, MAP_WIDTH, 320,
                                        wiring_basemap_relay_creat(SAFE_LEVEL_CAT3, name, num));
            } else {
                relaypainter.drawPixmap(0, i * 320, MAP_WIDTH, 160,
                                        wiring_basemap_relay_creat(SAFE_LEVEL_CAT2, name, 0));
            }
        } else {
            relaypainter.drawPixmap(0, i * 320, MAP_WIDTH, 160,
                                    wiring_basemap_relay_creat(SAFE_LEVEL_CAT2, name, 0, SOURCE_MCU_A, false));
        }
        if (b_relay_list[i]->get_block_data().is_used) {
            if (b_relay_list[i]->get_block_data().safe_level == SAFE_LEVEL_CAT2) {
                relaypainter.drawPixmap(0, i * 320 + 160, MAP_WIDTH, 160,
                                        wiring_basemap_relay_creat(SAFE_LEVEL_CAT2, name, 0, SOURCE_MCU_B, true));
            }
        } else {
            relaypainter.drawPixmap(0, i * 320 + 160, MAP_WIDTH, 160,
                                    wiring_basemap_relay_creat(SAFE_LEVEL_CAT2, name, 0, SOURCE_MCU_B, false));
        }
    }
    relay_label->setPixmap(relay_pixmap);
    layout->addWidget(relay_label, mline++, 0, 1, 3);
    /* mos */
    QLabel* mos_title = new QLabel("4.5 MOS接线图");
    mos_title->setFont(font);
    mos_title->setStyleSheet("color: rgb(0,127,195);");
    title_row.append(mline);
    layout->addWidget(mos_title, mline++, 0);
    QLabel* mos_label = new QLabel;
    QPixmap mos_pixmap(MAP_WIDTH, MOS_MAP_HEIGHT);
    mos_pixmap.fill(Qt::transparent);
    QPainter mospainter(&mos_pixmap);
    mospainter.setPen(QPen(Qt::black, 3));
    mospainter.drawRect(QRect(0, 0, 200, MOS_MAP_HEIGHT));
    QList<config_param*> mos_list = mainwindow->config_view_class->config_photo_svg->mos_config_list;
    for (int i = 0; i < mos_list.size(); i++) {
        QString name   = "MOS" + QString::number(i + 1);
        QString pixmap = mos_list[i]->get_block_data().pixmap;
        if (mos_list[i]->get_block_data().is_used) {
            mospainter.drawPixmap(0, i * 80, MAP_WIDTH, 80, wiring_basemap_mos_creat(name, pixmap));
        } else {
            mospainter.drawPixmap(0, i * 80, MAP_WIDTH, 80, wiring_basemap_mos_creat(name, pixmap, false));
        }
    }
    mos_label->setPixmap(mos_pixmap);
    layout->addWidget(mos_label, mline++, 0, 1, 3);
    /* 设置 */
    *line = mline;
}

void project_report::project_report_update()
{
    QLayout*     baseLayout = mainwindow->ui->scrollAreaWidgetContents_report->layout();
    QGridLayout* layout     = dynamic_cast<QGridLayout*>(baseLayout);
    if (layout) {
        project_report_clear(layout);
        title_row.clear();
        int          line   = 0;
        QSpacerItem* spacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        layout->addItem(spacer, 0, 2);
        project_info_creat(layout, &line);
        config_overview_creat(layout, &line);
        safety_param_creat(layout, &line);
        wiring_diagram_creat(layout, &line);
        QSpacerItem* verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
        layout->addItem(verticalSpacer, line, 0);
    }
}

void project_report::project_report_save()
{
    bool      success               = true;
    QDateTime currentDateTime       = QDateTime::currentDateTime();
    QString   currentDateTimeString = currentDateTime.toString("yyyy-MM-dd-hh-mm-ss");
    QString   DateTimeString        = currentDateTime.toString("yyyy-MM-dd hh:mm:ss");
    QString   fileName =
        QFileDialog::getSaveFileName(mainwindow, QString("保存文件"), QString("SPC100-Report-" + currentDateTimeString),
                                     QString("PDF 文件 (*.pdf)"));
    if (!fileName.isEmpty()) {
        QFile pdfFile(fileName);
        if (!pdfFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            mainwindow->my_message_box("打开或创建PDF文件失败", MESSAGE_TYPE_ERROR);
        } else {
            QLayout*     baseLayout = mainwindow->ui->scrollAreaWidgetContents_report->layout();
            QGridLayout* layout     = dynamic_cast<QGridLayout*>(baseLayout);
            QList<int>   title_y;
            QList<int>   title_height;
            for (int i = 0; i < title_row.size(); i++) {
                title_y.append(layout->cellRect(title_row[i], 0).y());
            }
            for (int i = 1; i < title_y.size(); i++) {
                title_height.append(title_y[i] - title_y[i - 1]);
            }
            title_height.append(mainwindow->ui->scrollAreaWidgetContents_report->height()
                                - title_y[title_y.size() - 1]);
            QPrinter printer(QPrinter::HighResolution);
            printer.setResolution(700);
            printer.setPageSize(QPagedPaintDevice::A4);
            printer.setOutputFormat(QPrinter::PdfFormat);
            printer.setOutputFileName(fileName);
            QPainter painter(&printer);
            painter.setRenderHint(QPainter::HighQualityAntialiasing);
            painter.setPen(Qt::white);
            painter.setBrush(QBrush(Qt::white));
            int scaleFactor = qRound(printer.pageRect().width() / 700.0);
            int pageHeight  = printer.pageRect().height();
            painter.scale(scaleFactor, scaleFactor);
            painter.drawImage(100, 100, QImage(":/new/photo/photo/logo.png").scaled(500, 500));
            painter.setFont(QFont("Arial", 2));
            painter.setPen(QPen(QColor(0, 127, 195)));
            QTextOption textOption;
            textOption.setAlignment(Qt::AlignCenter);
            painter.drawText(QRect(100, 600, 500, 50), "SPC1OO配置报告", textOption);
            painter.setFont(QFont("Arial", 1));
            painter.drawText(QRect(100, 650, 500, 50), DateTimeString, textOption);
            printer.newPage();
            painter.resetTransform();
            painter.scale(scaleFactor, scaleFactor);
            int remainder = pageHeight;

            for (int i = 0; i < title_height.size(); i++) {
                if (remainder < title_height[i] * scaleFactor) {
                    if (!printer.newPage()) {
                        mainwindow->my_message_box("文件写入失败", MESSAGE_TYPE_ERROR);
                        success = false;
                        break;
                    }
                    painter.resetTransform();
                    painter.scale(scaleFactor, scaleFactor);
                    remainder = pageHeight;
                }
                mainwindow->ui->scrollAreaWidgetContents_report->render(
                    &painter, QPoint(0, ((pageHeight - remainder) / scaleFactor)),
                    QRegion(0, title_y[i], 700, title_height[i]));
                remainder -= (title_height[i] * scaleFactor);
            }
            painter.end();
            if (success) {
                mainwindow->my_message_box("报告保存成功", MESSAGE_TYPE_INFO);
            }
        }
    }
}
