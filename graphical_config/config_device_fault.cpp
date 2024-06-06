#include "config_device_fault.h"
#include <QDebug>

config_device_fault::config_device_fault(int x, int y, int w, int h, QGraphicsItem* parent)
    : QGraphicsRectItem(x, y, w, h, parent)
{
    setBrush(Qt::NoBrush);
    setPen(Qt::NoPen);
    warning_pixmap = new QGraphicsPixmapItem(this);
    warning_pixmap->setPixmap(QPixmap(":/new/photo/photo/fault_warning.png").scaled(25, 20));
    warning_pixmap->setPos(x + 25, y + 6);
    warning_pixmap->setVisible(false);
    error_pixmap = new QGraphicsPixmapItem(this);
    error_pixmap->setPixmap(QPixmap(":/new/photo/photo/fault_error.png").scaled(25, 20));
    error_pixmap->setPos(x + 64, y + 6);
    error_pixmap->setVisible(false);
    connect(&error_flash_timer, &QTimer::timeout, this, error_flash_slot);
    connect(&warning_flash_timer, &QTimer::timeout, this, warning_flash_slot);
    connect(&device_fault_dialog, &device_fault::send_fault_state, this, update_fault_state);
}

void config_device_fault::set_fault_info(module_fault_t a_fault_info, module_fault_t b_fault_info)
{
    device_fault_dialog.set_module_fault(a_fault_info, b_fault_info);
}

/* user slots */
void config_device_fault::update_fault_state(bool error, bool warning)
{
    static bool last_error = false, last_warning = false;
    if (last_error != error) {
        last_error = error;
        if (error) {
            error_pixmap->setVisible(true);
            error_flash_timer.start(1000);
        } else {
            error_flash_timer.stop();
            error_pixmap->setVisible(false);
        }
    }
    if (last_warning != warning) {
        last_warning = warning;
        if (warning) {
            warning_pixmap->setVisible(true);
            warning_flash_timer.start(1000);
        } else {
            warning_flash_timer.stop();
            warning_pixmap->setVisible(false);
        }
    }
}

void config_device_fault::error_flash_slot()
{
    if (error_pixmap->isVisible()) {
        error_pixmap->setVisible(false);
    } else {
        error_pixmap->setVisible(true);
    }
}

void config_device_fault::warning_flash_slot()
{
    if (warning_pixmap->isVisible()) {
        warning_pixmap->setVisible(false);
    } else {
        warning_pixmap->setVisible(true);
    }
}

/* sys event */
void config_device_fault::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    device_fault_dialog.show();
    QGraphicsRectItem::mousePressEvent(event);
}
