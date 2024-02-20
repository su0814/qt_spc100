#ifndef MY_DIALOG_H
#define MY_DIALOG_H

#include <QDialog>
#include <QObject>
#include <QTimer>
class my_dialog : public QDialog {
    Q_OBJECT
public:
    my_dialog(QWidget* parent = nullptr);

private:
    QTimer start_timer;

public:
    void dialog_exec(void);

signals:
    void dialog_start(void);

private slots:
    void start_timer_timeout_slot(void);
};

#endif  // MY_DIALOG_H
