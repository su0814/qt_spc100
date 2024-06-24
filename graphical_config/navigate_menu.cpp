#include "navigate_menu.h"
#include "mainwindow.h"
#include <QDebug>
class menuCustomItemDelegate : public QStyledItemDelegate {
public:
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override
    {
        QStyledItemDelegate::paint(painter, option, index);
        QVariant data = index.data(Qt::UserRole);
        if (data.toString() == "menu") {
            // 在项目的图标的左边绘制一个绿点
            QRect dotRect(option.rect.left() - 17, option.rect.center().y() - 7, 14, 14);  // 绿点的位置和大小
            painter->setBrush(QColor(173, 216, 230, 50));
            painter->drawEllipse(dotRect);
        } else if (data.toString() == "element") {
            QRect dotRect(option.rect.left() - 20, option.rect.center().y() - 5, 10, 10);  // 绿点的位置和大小
            painter->setBrush(QColor(173, 216, 230));
            painter->drawEllipse(dotRect);
        }
    }
};
navigate_menu::navigate_menu(QWidget* uparent, QWidget* parent)
    : QTreeWidget(parent)
{
    mainwindow = ( MainWindow* )uparent;
    menu_init();
    connect(this, &QTreeWidget::itemClicked, this, &navigate_menu::on_items_clicked);
    expandAll();
}

void navigate_menu::menu_init()
{
    this->setHeaderHidden(true);
    this->setStyleSheet("QTreeWidget::branch:closed:has-children {image: "
                        "url(:/new/photo/photo/menu_close.png);}QTreeWidget::branch:open:has-children {image: "
                        "url(:/new/photo/photo/menu_open.png);}"
                        "QTreeWidget { font-size: 12px;border-radius: 10px;}"
                        "QTreeWidget::item { min-height: 20px; max-height: 30px;}"
                        "QTreeWidget::item { margin-bottom: 5px;}"
                        "QTreeWidget::item:selected { background-color: rgb(255,229,160);color: black; }");
    setIconSize(QSize(50, 50));
    setIndentation(20);
    setItemDelegate(new menuCustomItemDelegate);
    /* 硬件配置 */
    QTreeWidgetItem* hard_config = new QTreeWidgetItem(this);
    hard_config->setText(0, "模块配置");
    hard_config->setData(0, Qt::UserRole, "menu");
    QTreeWidgetItem* source_config = new QTreeWidgetItem(hard_config);
    source_config->setText(0, "资源配置");
    source_config->setData(0, Qt::UserRole, "element");
    menu_item_list.append(source_config);
    QTreeWidgetItem* parameter_config = new QTreeWidgetItem(hard_config);
    parameter_config->setText(0, "参数配置");
    parameter_config->setData(0, Qt::UserRole, "element");
    menu_item_list.append(parameter_config);
    /* 项目配置 */
    QTreeWidgetItem* project_set = new QTreeWidgetItem(this);
    project_set->setText(0, "项目设置");
    project_set->setData(0, Qt::UserRole, "menu");
    QTreeWidgetItem* project_baseinfo_set = new QTreeWidgetItem(project_set);
    project_baseinfo_set->setText(0, "项目信息");
    project_baseinfo_set->setData(0, Qt::UserRole, "element");
    menu_item_list.append(project_baseinfo_set);
    QTreeWidgetItem* project_device_info = new QTreeWidgetItem(project_set);
    project_device_info->setText(0, "设备信息");
    project_device_info->setData(0, Qt::UserRole, "element");
    menu_item_list.append(project_device_info);
    setCurrentItem(menu_item_list[0]);
}

/* user slots */
void navigate_menu::on_items_clicked(QTreeWidgetItem* item, int column)
{
    Q_UNUSED(column);
    if (menu_item_list.contains(item)) {
        int index = menu_item_list.indexOf(item);
        mainwindow->ui->stackedWidget_menu->setCurrentIndex(index);
    }
}
