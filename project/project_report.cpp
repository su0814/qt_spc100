#include "project_report.h"
#include "config/data_config.h"
#include "graphical_config/config_param.h"
#include "mainwindow.h"
#include <QFileDialog>
#include <QFileInfo>
#include <QObject>
#include <QPdfWriter>
#include <QtPrintSupport/QPrinter>
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
    input_table->setStyleSheet("QTableWidget { border: none; gridline-color: black; } "
                               "QTableWidget::item { border-right: 1px solid gray; border-bottom: 1px solid gray; }"
                               "QHeaderView::section:horizontal {background: rgba(128,128,128,100); }");
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
    int totalWidth = input_table->verticalHeader()->width();
    for (int i = 0; i < input_table->columnCount(); ++i) {
        totalWidth += input_table->columnWidth(i);
    }
    int totalHeight = input_table->horizontalHeader()->height();
    for (int i = 0; i < input_table->rowCount(); ++i) {
        totalHeight += input_table->rowHeight(i);
    }
    // input_table->setFixedSize(totalWidth + 2, totalHeight);
    if (input_table->rowCount() > 1) {
        layout->addWidget(input_table, mline++, 0, 1, 3);
        input_table->setFixedWidth(totalWidth + 2);
        input_table->setFixedHeight(totalHeight);
    }
    /* 标题2.2 */
    font.setPixelSize(13);
    QLabel* title2 = new QLabel("2.2 输出配置");
    title2->setFont(font);
    title2->setStyleSheet("color: rgb(0,127,195);");
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
    output_table->setStyleSheet("QTableWidget { border: none; gridline-color: black; } "
                                "QTableWidget::item { border-right: 1px solid gray; border-bottom: 1px solid gray; }"
                                "QHeaderView::section:horizontal {background: rgba(128,128,128,100); }");
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
    int totalWidth1 = output_table->verticalHeader()->width();
    for (int i = 0; i < output_table->columnCount(); ++i) {
        totalWidth1 += output_table->columnWidth(i);
    }
    int totalHeight1 = output_table->horizontalHeader()->height();
    for (int i = 0; i < output_table->rowCount(); ++i) {
        totalHeight1 += output_table->rowHeight(i);
    }
    if (output_table->rowCount() > 1) {
        layout->addWidget(output_table, mline++, 0, 1, 3);
        output_table->setFixedWidth(totalWidth1 + 2);
        output_table->setFixedHeight(totalHeight1);
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

void project_report::project_report_update()
{
    QLayout*     baseLayout = mainwindow->ui->scrollAreaWidgetContents_report->layout();
    QGridLayout* layout     = dynamic_cast<QGridLayout*>(baseLayout);
    if (layout) {
        project_report_clear(layout);
        int          line   = 0;
        QSpacerItem* spacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        layout->addItem(spacer, 0, 2);
        project_info_creat(layout, &line);
        config_overview_creat(layout, &line);
        safety_param_creat(layout, &line);
        QSpacerItem* verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
        layout->addItem(verticalSpacer, line, 0);
    }
}

void project_report::project_report_save()
{
    QDateTime currentDateTime       = QDateTime::currentDateTime();
    QString   currentDateTimeString = currentDateTime.toString("yyyy-MM-dd-hh-mm-ss");
    QString   fileName =
        QFileDialog::getSaveFileName(mainwindow, QString("保存文件"), QString("SPC100-Report-" + currentDateTimeString),
                                     QString("PDF 文件 (*.pdf)"));
    if (!fileName.isEmpty()) {
        QFile pdfFile(fileName);
        if (!pdfFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            mainwindow->my_message_box("打开或创建PDF文件失败", MESSAGE_TYPE_ERROR);
        } else {
            // project_report_update();
            QPrinter printer(QPrinter::HighResolution);
            printer.setResolution(700);
            printer.setPageSize(QPagedPaintDevice::A4);
            printer.setOutputFormat(QPrinter::PdfFormat);
            printer.setOutputFileName(fileName);
            QPainter painter(&printer);
            qreal    scaleFactor      = printer.pageRect().width() / qreal(700);
            int      pageHeight       = printer.pageRect().height();
            int      screenshotHeight = mainwindow->ui->scrollAreaWidgetContents_report->height() * scaleFactor;
            painter.scale(scaleFactor, scaleFactor);
            int yOffset = 0;
            while (yOffset < screenshotHeight) {
                mainwindow->ui->scrollAreaWidgetContents_report->render(
                    &painter, QPoint(0, 0), QRegion(0, yOffset / scaleFactor, 700, pageHeight / scaleFactor));

                yOffset += pageHeight;
                if (yOffset < screenshotHeight) {
                    if (!printer.newPage()) {
                        mainwindow->my_message_box("文件写入失败", MESSAGE_TYPE_ERROR);
                        break;
                    }
                    painter.resetTransform();  // 重置坐标变换
                    painter.scale(scaleFactor, scaleFactor);
                }
            }
            painter.end();
        }
    }
}
