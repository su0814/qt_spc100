#include "my_dialog.h"

my_dialog::my_dialog(QWidget* parent)
    : QDialog(parent)
{
    start_timer.setSingleShot(true);
    connect(&start_timer, &QTimer::timeout, this, start_timer_timeout_slot);
}

void my_dialog::dialog_exec()
{
    start_timer.start(100);
    this->exec();
}

/* user slots */
void my_dialog::start_timer_timeout_slot()
{
    emit dialog_start();
}
