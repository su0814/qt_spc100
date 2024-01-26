#include "logic_tools.h"
#include "mainwindow.h"
#include <QDebug>
logic_tools::logic_tools(QWidget* mparent, QWidget* parent)
    : QWidget(parent)
{
    ui         = MainWindow::my_ui->ui;
    mainwindow = ( MainWindow* )mparent;
    tools_ui_init();
}

void logic_tools::tools_ui_init()
{
    QVBoxLayout* layout_di = new QVBoxLayout(ui->page_condi_di);
    for (uint8_t i = TOOL_ID_DI1; i <= TOOL_ID_DI12; i++) {
        tool_block* tool =
            new tool_block(":/new/photo/photo/DI.png", TOOL_TYPE_CONDI_DI, ( tool_id_e )i, ui->page_condi_di);
        di_tools_list.append(tool);
        layout_di->addWidget(di_tools_list[i]);
        di_tools_list[i]->setVisible(false);
    }
    QSpacerItem* spacer_di = new QSpacerItem(0, 0, QSizePolicy::Preferred, QSizePolicy::Expanding);
    layout_di->addItem(spacer_di);
    ui->page_condi_di->setLayout(layout_di);

    QVBoxLayout* layout_ai = new QVBoxLayout(ui->page_condi_ai);
    for (uint8_t i = TOOL_ID_AI1; i <= TOOL_ID_AI2; i++) {
        tool_block* tool =
            new tool_block(":/new/photo/photo/AI.png", TOOL_TYPE_CONDI_AI, ( tool_id_e )i, ui->page_condi_ai);
        ai_tools_list.append(tool);
        layout_ai->addWidget(tool);
        ai_tools_list[i - TOOL_ID_AI1]->setVisible(false);
    }
    QSpacerItem* spacer_ai = new QSpacerItem(0, 0, QSizePolicy::Preferred, QSizePolicy::Expanding);
    layout_ai->addItem(spacer_ai);
    ui->page_condi_ai->setLayout(layout_ai);

    QVBoxLayout* layout_pi = new QVBoxLayout(ui->page_condi_pi);
    for (uint8_t i = TOOL_ID_PI1; i <= TOOL_ID_PI2; i++) {
        tool_block* tool =
            new tool_block(":/new/photo/photo/PI.png", TOOL_TYPE_CONDI_PI, ( tool_id_e )i, ui->page_condi_pi);
        pi_tools_list.append(tool);
        layout_pi->addWidget(tool);
        pi_tools_list[i - TOOL_ID_PI1]->setVisible(false);
    }
    QSpacerItem* spacer_pi = new QSpacerItem(0, 0, QSizePolicy::Preferred, QSizePolicy::Expanding);
    layout_pi->addItem(spacer_pi);
    ui->page_condi_pi->setLayout(layout_pi);

    QVBoxLayout* layout_qep = new QVBoxLayout(ui->page_condi_qep);
    for (uint8_t i = TOOL_ID_QEP1; i <= TOOL_ID_PI_QEP2; i++) {
        tool_block* tool =
            new tool_block(":/new/photo/photo/QEP.png", TOOL_TYPE_CONDI_QEP, ( tool_id_e )i, ui->page_condi_qep);
        qep_tools_list.append(tool);
        layout_qep->addWidget(tool);
        qep_tools_list[i - TOOL_ID_QEP1]->setVisible(false);
    }
    QSpacerItem* spacer_qep = new QSpacerItem(0, 0, QSizePolicy::Preferred, QSizePolicy::Expanding);
    layout_qep->addItem(spacer_qep);
    ui->page_condi_qep->setLayout(layout_qep);

    connect(ui->treeWidget_condi, &QTreeWidget::itemChanged, this, condi_tree_changed);

    /* logic tools */
    QVBoxLayout* layout_logic = new QVBoxLayout(ui->page_logic);
    tool_block*  tool_and =
        new tool_block(":/new/photo/photo/and.png", TOOL_TYPE_LOGIC_AND, TOOL_ID_LOGIC_AND, ui->page_logic);
    tool_and->set_name("AND");
    logic_tools_list.append(tool_and);
    layout_logic->addWidget(tool_and);

    tool_block* tool_or =
        new tool_block(":/new/photo/photo/OR.png", TOOL_TYPE_LOGIC_OR, TOOL_ID_LOGIC_OR, ui->page_logic);
    tool_or->set_name("OR");
    logic_tools_list.append(tool_or);
    layout_logic->addWidget(tool_or);

    tool_block* tool_not =
        new tool_block(":/new/photo/photo/LOGIC_NOT.png", TOOL_TYPE_LOGIC_NOT, TOOL_ID_LOGIC_NOT, ui->page_logic);
    tool_not->set_name("NOT");
    logic_tools_list.append(tool_not);
    layout_logic->addWidget(tool_not);

    tool_block* tool_sf =
        new tool_block(":/new/photo/photo/SF.png", TOOL_TYPE_LOGIC_SF, TOOL_ID_LOGIC_SF, ui->page_logic);
    tool_sf->set_name("SF");
    logic_tools_list.append(tool_sf);
    layout_logic->addWidget(tool_sf);

    tool_block* tool_exit =
        new tool_block(":/new/photo/photo/exit.png", TOOL_TYPE_LOGIC_EXIT, TOOL_ID_LOGIC_EXIT, ui->page_logic);
    tool_exit->set_name("EXIT");
    logic_tools_list.append(tool_exit);
    layout_logic->addWidget(tool_exit);

    QSpacerItem* spacer_logic = new QSpacerItem(0, 0, QSizePolicy::Preferred, QSizePolicy::Expanding);
    layout_logic->addItem(spacer_logic);
    ui->page_logic->setLayout(layout_logic);
}

void logic_tools::logic_tools_reset()
{
    for (int i = 0; i < di_tools_list.count(); i++) {
        di_tools_list[i]->setVisible(false);
    }
    for (int i = 0; i < ai_tools_list.count(); i++) {
        ai_tools_list[i]->setVisible(false);
    }
    for (int i = 0; i < pi_tools_list.count(); i++) {
        pi_tools_list[i]->setVisible(false);
    }
    for (int i = 0; i < qep_tools_list.count(); i++) {
        qep_tools_list[i]->setVisible(false);
    }
    logic_tools_list[TOOL_TYPE_LOGIC_EXIT - TOOL_TYPE_LOGIC_AND]->setEnabled(true);
}

void logic_tools::condi_tree_changed(QTreeWidgetItem* item, int column)
{
    if (item->parent() == nullptr) {
        return;
    }
    int        item_num   = ui->treeWidget_condi->indexOfTopLevelItem(item->parent());
    int        item_count = item->parent()->indexOfChild(item);
    QLineEdit* other_name = qobject_cast<QLineEdit*>(ui->treeWidget_condi->itemWidget(item, 1));
    QString    tool_name  = item->text(0) + "_" + other_name->text();
    switch (item_num) {
    case TOOL_DI_ITEM_NUM:
        di_tools_list[item_count]->set_name(tool_name);
        if (item->checkState(0) == Qt::Checked) {
            di_tools_list[item_count]->setVisible(true);
            mainwindow->project_report_class->input_resource_info.is_config[item_count] = true;
        } else {
            di_tools_list[item_count]->setVisible(false);
            mainwindow->project_report_class->input_resource_info.is_config[item_count] = false;
        }
        break;
    case TOOL_AI_ITEM_NUM:
        ai_tools_list[item_count]->set_name(tool_name);
        if (item->checkState(0) == Qt::Checked) {
            ai_tools_list[item_count]->setVisible(true);
            mainwindow->project_report_class->input_resource_info.is_config[INPUT_DI_RESOURCE_NUM + item_count] = true;
        } else {
            ai_tools_list[item_count]->setVisible(false);
            mainwindow->project_report_class->input_resource_info.is_config[INPUT_DI_RESOURCE_NUM + item_count] = false;
        }

        break;
    case TOOL_PI_ITEM_NUM:
        pi_tools_list[item_count]->set_name(tool_name);
        if (item->checkState(0) == Qt::Checked) {
            pi_tools_list[item_count]->setVisible(true);
            mainwindow->project_report_class->input_resource_info
                .is_config[INPUT_DI_RESOURCE_NUM + INPUT_AI_RESOURCE_NUM + item_count] = true;
        } else {
            pi_tools_list[item_count]->setVisible(false);
            mainwindow->project_report_class->input_resource_info
                .is_config[INPUT_DI_RESOURCE_NUM + INPUT_AI_RESOURCE_NUM + item_count] = false;
        }

        break;
    case TOOL_QEP_ITEM_NUM:
        qep_tools_list[item_count]->set_name(tool_name);
        if (item->checkState(0) == Qt::Checked) {
            qep_tools_list[item_count]->setVisible(true);
            mainwindow->project_report_class->input_resource_info
                .is_config[INPUT_DI_RESOURCE_NUM + INPUT_AI_RESOURCE_NUM + INPUT_PI_RESOURCE_NUM + item_count] = true;
        } else {
            qep_tools_list[item_count]->setVisible(false);
            mainwindow->project_report_class->input_resource_info
                .is_config[INPUT_DI_RESOURCE_NUM + INPUT_AI_RESOURCE_NUM + INPUT_PI_RESOURCE_NUM + item_count] = false;
        }
        break;
    default:
        break;
    }
}
