#include "mainwindow.h"
#include "QDateTime"
#include "QTime"
#include "md5.h"
#include "qdebug.h"
#include "qfiledialog.h"
#include "qmessagebox.h"
#include "stdio.h"
#include "ui_mainwindow.h"
#include <windows.h>
MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui_init();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::ui_init()
{
    my_serial = new my_serialport;
    transport = new transportcrc;
    ui->serial_port_comboBox->installEventFilter(this);
    serial_search();

    connect(my_serial->my_serial, SIGNAL(errorOccurred(QSerialPort::SerialPortError)), this,
            SLOT(serial_error_callback(QSerialPort::SerialPortError)));
    connect(my_serial->my_serial, SIGNAL(readyRead()), this, SLOT(serial_data_proc()));
    connect(my_serial->transport, SIGNAL(signal_onFrameDetected(uint8_t*, int32_t)), this,
            SLOT(cmd_callback(uint8_t*, int32_t)));

    ui->start_upgrade_pushButton->setEnabled(false);
    ui->select_fw_pushButton->setEnabled(false);
    iap_info.upgrade_mode = UPGRADE_SYNCHRONOUS;
    module_name_list << "spc100";
    upgrade_id_list << "COMMON"
                    << "A"
                    << "B";
    PT_INIT(&pt_upgrade);
}

void MainWindow::serial_switch_ctrl()
{

    QSerialPort::DataBits databits = QSerialPort::Data8;
    QSerialPort::StopBits stopbits = QSerialPort::OneStop;
    QSerialPort::Parity   parity   = QSerialPort::NoParity;
    qint32                baudrate = (ui->serial_baudrate_comboBox->currentText().toUInt());
    if (ui->serial_switch_pushButton->text() == "打开串口") {
        if (my_serial->portname_list.length() == 0) {
            ui->serial_log->append(TEXT_COLOR_RED(QString("未检测到可用端口"), TEXT_SIZE_MEDIUM));
            return;
        }

        /* serial databit set */
        switch (ui->serial_databit_comboBox->currentText().toUInt()) {
        case 5:
            databits = QSerialPort::Data5;
            break;
        case 6:
            databits = QSerialPort::Data6;
            break;
        case 7:
            databits = QSerialPort::Data7;
            break;
        case 8:
            databits = QSerialPort::Data8;
            break;
        }
        /* serial parity set */
        switch (ui->serial_parity_comboBox->currentIndex()) {
        case 0:
            parity = QSerialPort::NoParity;
            break;
        case 1:
            parity = QSerialPort::EvenParity;
            break;
        case 2:
            parity = QSerialPort::OddParity;
            break;
        }
        /* serial dstopbit set */
        switch (ui->serial_stopbit_comboBox->currentIndex()) {
        case 0:
            stopbits = QSerialPort::OneStop;
            break;
        case 1:
            stopbits = QSerialPort::OneAndHalfStop;
            break;
        case 2:
            stopbits = QSerialPort::TwoStop;
            break;
        }
        int i   = ui->serial_port_comboBox->currentIndex();
        int ret = my_serial->open_port(my_serial->portname_list[i].portName(), baudrate, databits, parity, stopbits);
        if (ret != 0) {
            ui->serial_log->append(TEXT_COLOR_RED("打开串口失败", TEXT_SIZE_MEDIUM));
            return;
        }
        ui->serial_switch_pushButton->setStyleSheet("background-color: rgb(200, 50, 0)");
        ui->serial_switch_pushButton->setText("关闭串口");
        ui->serial_log->append(TEXT_COLOR_GREEN(QString("成功打开串口"), TEXT_SIZE_MEDIUM));
        ui->serial_port_comboBox->setEnabled(false);
        ui->serial_baudrate_comboBox->setEnabled(false);
        ui->serial_databit_comboBox->setEnabled(false);
        ui->serial_parity_comboBox->setEnabled(false);
        ui->serial_stopbit_comboBox->setEnabled(false);
        ui->select_fw_pushButton->setEnabled(true);
    } else {
        ui->serial_switch_pushButton->setStyleSheet("background-color: rgb(100, 200, 50)");
        ui->serial_switch_pushButton->setText("打开串口");
        ui->serial_log->append(TEXT_COLOR_GREEN(QString("成功关闭串口"), TEXT_SIZE_MEDIUM));
        my_serial->close_port();
        ui->serial_port_comboBox->setEnabled(true);
        ui->serial_baudrate_comboBox->setEnabled(true);
        ui->serial_databit_comboBox->setEnabled(true);
        ui->serial_parity_comboBox->setEnabled(true);
        ui->serial_stopbit_comboBox->setEnabled(true);
        ui->select_fw_pushButton->setEnabled(false);
        ui->start_upgrade_pushButton->setEnabled(false);
        ui->upgrade_log->clear();
        ui->upgrade_progressBar->setValue(0);
    }
}

void MainWindow::serial_search()
{
    ui->serial_port_comboBox->clear();
    my_serial->serch_port();
    int len = my_serial->portname_list.length();
    if (len > 0) {
        for (uint8_t i = 0; i < len; i++) {
            qDebug() << my_serial->portname_list[i].portName();
            ui->serial_port_comboBox->addItem(my_serial->portname_list[i].portName() + " #"
                                              + my_serial->portname_list[i].description());
        }
    }
}

void MainWindow::serial_data_proc()
{

    QByteArray tmp_buf;
    while (my_serial->my_serial->bytesAvailable()) {
        tmp_buf.append(my_serial->my_serial->readAll());
    }
    foreach (uint8_t byte, tmp_buf) {
        my_serial->data_decrypt(&byte, 1);
        my_serial->transport->receiveByte(byte);
    }
}

void MainWindow::cmd_callback(uint8_t* frame, int32_t length)
{

    boot_cmd_response(frame, length);
}

void MainWindow::my_message_box(QString title, QString text)
{
    static bool isshow_flag = false;
    if (!isshow_flag) {
        isshow_flag = true;
        QMessageBox box;
        box.setStyleSheet("QLabel{"
                          "min-width: 300px;"
                          "min-height: 300px; "
                          "font-size:20px;"
                          "}"
                          "QPushButton {"
                          "background-color:#89AFDE;"
                          " border-style: outset;"
                          " border-width: 10px;"
                          " border-radius: 20px;"
                          " border-color: beige;"
                          " font: bold 15px;"
                          " min-width: 15em;"
                          " min-height: 5em;"
                          "}"
                          "");
        box.setText(text);
        box.setWindowTitle(title);
        box.setWindowIcon(QIcon(":/new/photo/photo/logo.png"));
        box.setIconPixmap(QPixmap(":/new/photo/photo/logo.ico"));
        box.exec();
        isshow_flag = false;
    }
}

int MainWindow::serial_error_callback(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError || error == QSerialPort::PermissionError) {
        ui->serial_log->append(TEXT_COLOR_RED("串口断开，请检查串口！！！！！！", TEXT_SIZE_MEDIUM));
        ui->serial_switch_pushButton->click();
        iap_info.status = IAP_DOWNLOAD_END;
        ui->upgrade_log->clear();
        ui->upgrade_progressBar->setValue(0);
        serial_search();
        my_message_box("串口警告", "串口异常断开，请检查串口状态！");
    }
    return 0;
}

void MainWindow::boot_cmd_response(uint8_t* frame, int32_t length)
{
    uint8_t cmd    = frame[2];
    uint8_t bootid = frame[0];
    if (bootid >= BOOT_SYNC_ID_MAX) {
        return;
    }
    switch (cmd) {
    case CMD_BL_DATA_TRANSFER:
        switch (frame[3]) {
        case SUB_BL_EOT_ACK:
        case SUB_BL_SOH_ACK:
            iap_info.ack_nack[bootid]   = frame[3];
            iap_info.error_code[bootid] = ( int8_t )frame[6];
            break;

        case SUB_BL_STX_ACK:
            iap_info.ack_nack[bootid]   = frame[3];
            iap_info.error_code[bootid] = ( int8_t )frame[6];
            iap_info.reqpackseq[bootid] = frame[7] | (frame[8] << 8) | (frame[9] << 16) | (frame[10] << 24);
            break;
        default:
            break;
        }
        break;
    case CMD_BL_STATUS:
        if (frame[3] == SUB_BL_STS_START) {
            fflush(stdout);
            iap_info.upgrade_mode          = frame[6];
            iap_info.bl_start_flag[bootid] = 1;
            ui->upgrade_log->append(
                TEXT_COLOR_GREEN(upgrade_id_list[bootid] + QString(" :bootloader等待文件传输..."), TEXT_SIZE_MEDIUM));
        } else if (frame[3] == SUB_BL_STS_APP_RUN) {
            qDebug() << length;
            qDebug() << frame[6];
            if (length < 7)
                break;
            switch (( int8_t )frame[6]) {
            case 0:
                ui->upgrade_log->append(
                    TEXT_COLOR_GREEN(upgrade_id_list[bootid] + QString(":app启动成功"), TEXT_SIZE_MEDIUM));
                if (iap_info.status == IAP_DOWNLOAD_IDLE)
                    ui->upgrade_log->append(TEXT_COLOR_GREEN(
                        upgrade_id_list[bootid] + QString(":错过了bootloader启动时间点，请重新下载或重新上电"),
                        TEXT_SIZE_MEDIUM));
                iap_info.app_start_flag[bootid] = 1;
                // boot_status = SUB_BL_STS_APP_RUN;
                break;
            case -1:
                boot_status = SUB_BL_STS_APP_RUN_FAIL;
                ui->upgrade_log->append(
                    TEXT_COLOR_RED(upgrade_id_list[bootid] + QString(":Error:文件码错误！"), TEXT_SIZE_LARGE));
                break;
            case -2:
                boot_status = SUB_BL_STS_APP_RUN_FAIL;
                ui->upgrade_log->append(
                    TEXT_COLOR_RED(upgrade_id_list[bootid] + QString(":Error:MSP码错误！"), TEXT_SIZE_LARGE));
                break;
            case -3:
                boot_status = SUB_BL_STS_APP_RUN_FAIL;
                ui->upgrade_log->append(
                    TEXT_COLOR_RED(upgrade_id_list[bootid] + QString(":Error:一级密钥错误！"), TEXT_SIZE_LARGE));
                break;
            case -4:
                boot_status = SUB_BL_STS_APP_RUN_FAIL;
                ui->upgrade_log->append(
                    TEXT_COLOR_RED(upgrade_id_list[bootid] + QString(":Error:文件名错误！"), TEXT_SIZE_LARGE));
                break;
            case -5:
                boot_status = SUB_BL_STS_APP_RUN_FAIL;
                ui->upgrade_log->append(
                    TEXT_COLOR_RED(upgrade_id_list[bootid] + QString(":Error:二级密钥错误！"), TEXT_SIZE_LARGE));
                break;
            case -6:
                boot_status = SUB_BL_STS_APP_RUN_FAIL;
                ui->upgrade_log->append(
                    TEXT_COLOR_RED(upgrade_id_list[bootid] + QString(":Error:三级密钥错误！"), TEXT_SIZE_LARGE));
                break;
            default:
                break;
            }
        }
        break;
    default:
        break;
    }
}

int MainWindow::firmware_info_encrypt_decrypt(firmware_info_t* firmware_info, key_crypt_e key)
{
    if (firmware_info->magic == VALID_MAGIC_ENCRYPT && key == KEY_ENCRYPT) {
        return -1;  //已经加过密，无需加密
    } else if (firmware_info->magic == VALID_MAGIC && key == KEY_DECRYPT) {
        return -2;  //已经解过密，无需解密
    }

    uint8_t* info_addr = ( uint8_t* )firmware_info;
    for (uint32_t i = 0; i < sizeof(firmware_info_t); i++) {
        if ((info_addr[i] != 0) && (info_addr[i] != ENCRYPTED_CODE_BYTE)) {
            info_addr[i] ^= ENCRYPTED_CODE_BYTE;
        }
    }
    return 0;
}

int MainWindow::upgrade_ack_soh_result_phase(uint8_t* retry_cnt, uint8_t* bootid)
{
    uint8_t retry = *retry_cnt;
    uint8_t ret   = 0;

    if (iap_info.upgrade_mode == UPGRADE_SYNCHRONOUS) {
        if (iap_info.ack_nack[BOOT_SYNC_ID_A] != SUB_BL_SOH_ACK
            || iap_info.ack_nack[BOOT_SYNC_ID_B] != SUB_BL_SOH_ACK) {
            if (iap_info.ack_nack[BOOT_SYNC_ID_A] != SUB_BL_SOH_ACK
                && iap_info.ack_nack[BOOT_SYNC_ID_B] != SUB_BL_SOH_ACK) {
                ui->upgrade_log->append(
                    TEXT_COLOR_RED(QString("Warning: Wait all  SOH ACK timeout!, retry: " + QString::number(retry)),
                                   TEXT_SIZE_MEDIUM));
            } else if (iap_info.ack_nack[BOOT_SYNC_ID_B] != SUB_BL_SOH_ACK) {
                *bootid = BOOT_SYNC_ID_B;
                ui->upgrade_log->append(TEXT_COLOR_RED(
                    "Warning: Wait B  SOH ACK timeout!, retry: " + QString::number(retry), TEXT_SIZE_MEDIUM));
            } else if (iap_info.ack_nack[BOOT_SYNC_ID_A] != SUB_BL_SOH_ACK) {
                *bootid = BOOT_SYNC_ID_A;
                ui->upgrade_log->append(TEXT_COLOR_RED(
                    QString("Warning: Wait A  SOH ACK timeout!, retry: " + QString::number(retry)), TEXT_SIZE_MEDIUM));
            }
            if (++retry >= 5) {
                iap_info.status = IAP_DOWNLOAD_END;
                ui->upgrade_log->append(TEXT_COLOR_RED(QString("retry SOH fail"), TEXT_SIZE_MEDIUM));
                ret = -1;
            }

        } else {
            for (uint8_t i = BOOT_SYNC_ID_A; i <= BOOT_SYNC_ID_B; i++) {
                switch (iap_info.error_code[i]) {
                case 0:
                    if (iap_info.status != IAP_DOWNLOAD_END) {
                        iap_info.status = IAP_DOWNLOADING;
                    }
                    break;
                case -1:
                    iap_info.status = IAP_DOWNLOAD_END;
                    ui->upgrade_log->append(
                        upgrade_id_list[i]
                        + TEXT_COLOR_RED("-Error: 下载的固件与板子不匹配，请检查！", TEXT_SIZE_LARGE));
                    break;
                case -2:
                    iap_info.status = IAP_DOWNLOAD_END;
                    ui->upgrade_log->append(TEXT_COLOR_RED(
                        upgrade_id_list[i] + "-Error: 下载的固件大小超过预定义内存范围！", TEXT_SIZE_LARGE));
                    break;
                case -3:
                    iap_info.status = IAP_DOWNLOAD_END;
                    ui->upgrade_log->append(
                        TEXT_COLOR_RED(upgrade_id_list[i] + "-Error: 当前版本较高，无需更新！", TEXT_SIZE_LARGE));
                    break;
                default:
                    iap_info.status = IAP_DOWNLOAD_END;
                    ui->upgrade_log->append(TEXT_COLOR_RED(upgrade_id_list[i] + "Error: 未知错误！", TEXT_SIZE_LARGE));
                    break;
                }
            }
            ret = -1;
        }
    } else {
        if (iap_info.ack_nack[BOOT_SYNC_ID_COMMON] != SUB_BL_SOH_ACK) {
            if (++retry >= 5) {
                iap_info.status = IAP_DOWNLOAD_END;
                ui->upgrade_log->append(TEXT_COLOR_RED(
                    upgrade_id_list[BOOT_SYNC_ID_COMMON] + QString(" retry SOH fail"), TEXT_SIZE_MEDIUM));
                ret = -1;
            }
            ui->upgrade_log->append(
                TEXT_COLOR_RED(upgrade_id_list[BOOT_SYNC_ID_COMMON]
                                   + QString(" Warning: Wait SOH ACK timeout!, retry: " + QString::number(retry)),
                               TEXT_SIZE_MEDIUM));
        } else {
            switch (iap_info.error_code[BOOT_SYNC_ID_COMMON]) {
            case 0:
                iap_info.status = IAP_DOWNLOADING;
                break;
            case -1:
                iap_info.status = IAP_DOWNLOAD_END;
                ui->upgrade_log->append(
                    TEXT_COLOR_RED(upgrade_id_list[BOOT_SYNC_ID_COMMON] + "-Error: 下载的固件与板子不匹配，请检查！",
                                   TEXT_SIZE_LARGE));
                break;
            case -2:
                iap_info.status = IAP_DOWNLOAD_END;
                ui->upgrade_log->append(
                    TEXT_COLOR_RED(upgrade_id_list[BOOT_SYNC_ID_COMMON] + "-Error: 下载的固件大小超过预定义内存范围！",
                                   TEXT_SIZE_LARGE));
                break;
            case -3:
                iap_info.status = IAP_DOWNLOAD_END;
                ui->upgrade_log->append(TEXT_COLOR_RED(
                    upgrade_id_list[BOOT_SYNC_ID_COMMON] + "-Error: 当前已是最新版本，无需更新！", TEXT_SIZE_LARGE));
                break;
            default:
                iap_info.status = IAP_DOWNLOAD_END;
                ui->upgrade_log->append(TEXT_COLOR_RED(
                    upgrade_id_list[BOOT_SYNC_ID_COMMON] + QString("-Error: 未知错误！"), TEXT_SIZE_LARGE));
                break;
            }
            ret = -1;
        }
    }
    *retry_cnt = retry;
    return ret;
}

int MainWindow::upgrade_ack_stx_result_phase(uint8_t* retry_cnt, uint8_t* bootid)
{
    uint8_t retry = *retry_cnt;
    uint8_t ret   = 0;
    if (iap_info.upgrade_mode == UPGRADE_SYNCHRONOUS) {
        /* 目标未反馈，重试 */
        if (iap_info.ack_nack[BOOT_SYNC_ID_A] != SUB_BL_STX_ACK
            || iap_info.ack_nack[BOOT_SYNC_ID_B] != SUB_BL_STX_ACK) {
            if (++retry < 10) {
                if (iap_info.ack_nack[BOOT_SYNC_ID_A] != SUB_BL_STX_ACK
                    && iap_info.ack_nack[BOOT_SYNC_ID_B] != SUB_BL_STX_ACK) {
                    ui->upgrade_log->append(
                        TEXT_COLOR_RED(QString("E-Warning: 等待目标反馈超时！重新发送..." + QString::number(retry)),
                                       TEXT_SIZE_MEDIUM));
                } else if (iap_info.ack_nack[BOOT_SYNC_ID_B] != SUB_BL_STX_ACK) {
                    *bootid = BOOT_SYNC_ID_B;
                    ui->upgrade_log->append(TEXT_COLOR_RED(
                        "B-Warning: 等待目标反馈超时！重新发送..." + QString::number(retry), TEXT_SIZE_MEDIUM));
                } else if (iap_info.ack_nack[BOOT_SYNC_ID_A] != SUB_BL_STX_ACK) {
                    *bootid = BOOT_SYNC_ID_A;
                    ui->upgrade_log->append(TEXT_COLOR_RED(
                        "A-Warning: 等待目标反馈超时！重新发送..." + QString::number(retry), TEXT_SIZE_MEDIUM));
                }
            } else {
                iap_info.status = IAP_DOWNLOAD_END;
                ui->upgrade_log->append(TEXT_COLOR_RED(QString("SYNC-Error: 重发失败！"), TEXT_SIZE_MEDIUM));
                ret = -1;
            }
        } else {
            retry = 0;
            for (uint8_t i = BOOT_SYNC_ID_A; i <= BOOT_SYNC_ID_B; i++) {
                if (iap_info.error_code[i] == -1) {
                    ui->upgrade_log->append(TEXT_COLOR_BLUE(
                        upgrade_id_list[i] + "-Warning: 目标未收到正确的包!seq = " + QString::number(iap_info.packseq)
                            + ", reqseq = " + QString::number(iap_info.reqpackseq[i]),
                        TEXT_SIZE_MEDIUM));
                    /* 目标请求的包不正确，且误差很严重 */
                    if (iap_info.reqpackseq[i] < iap_info.packseq || iap_info.reqpackseq[i] > iap_info.packseq + 1) {
                        ui->upgrade_log->append(TEXT_COLOR_RED(upgrade_id_list[i]
                                                                   + "Error: 目标在请求错误的包：reqseq = "
                                                                   + QString::number(iap_info.reqpackseq[i]),
                                                               TEXT_SIZE_MEDIUM));
                        iap_info.status = IAP_DOWNLOAD_END;
                        ret             = -1;
                        ;
                    }
                    /* 本次发送的包没有被收到，重新发送本次数据包 */
                    /* 其实这个条件并不会被触发，因为目标如果没收到，就不会有这次NAK回复了 */
                    if (iap_info.reqpackseq[i] == iap_info.packseq) {
                        ui->upgrade_log->append(TEXT_COLOR_BLUE(
                            upgrade_id_list[i] + "重新发送数据包... seq = " + QString::number(iap_info.packseq)
                                + ", reqseq = " + QString::number(iap_info.reqpackseq[i]),
                            TEXT_SIZE_MEDIUM));
                    }
                    /* 目标收到了重复的包 */
                    /* 这可能是由于目标的ACK包没被PC收到，于是PC重新发包导致的 */
                    if (iap_info.reqpackseq[i] == (iap_info.packseq + 1)) {
                        ui->upgrade_log->append(
                            TEXT_COLOR_BLUE(upgrade_id_list[i] + "目标收到了重复的包!", TEXT_SIZE_MEDIUM));
                    }
                }
            }
            if (iap_info.error_code[BOOT_SYNC_ID_A] == 0 && iap_info.error_code[BOOT_SYNC_ID_B] == 0) {
                ++iap_info.packseq;
                iap_info.write_size += iap_info.packlen;
                ui->upgrade_progressBar->setValue(iap_info.write_size);
                if (iap_info.write_size >= iap_info.fw_size) {
                    iap_info.status = IAP_DOWNLOAD_FINISH;
                    ui->upgrade_log->append(TEXT_COLOR_GREEN(QString("Download finish!"), TEXT_SIZE_MEDIUM));
                    ret = -1;
                }
            }
        }
    } else {
        /* 目标未反馈，重试 */
        if (iap_info.ack_nack[BOOT_SYNC_ID_COMMON] != SUB_BL_STX_ACK) {
            if (++retry < 10) {
                ui->upgrade_log->append(
                    TEXT_COLOR_BLUE(QString("C-Warning: 等待目标反馈超时！重新发送..."), TEXT_SIZE_MEDIUM));
            } else {
                iap_info.status = IAP_DOWNLOAD_END;
                ui->upgrade_log->append(TEXT_COLOR_RED(QString("C-Error: 重发失败！"), TEXT_SIZE_MEDIUM));
                ret = -1;
            }
        } else {
            retry = 0;
            /* 目标未收到正确的包 */
            if (iap_info.error_code[BOOT_SYNC_ID_COMMON] == -1) {
                ui->upgrade_log->append(TEXT_COLOR_BLUE(
                    upgrade_id_list[BOOT_SYNC_ID_COMMON]
                        + QString("Warning: 目标未收到正确的包!seq = " + QString::number(iap_info.packseq)
                                  + ", reqseq = " + QString::number(iap_info.reqpackseq[BOOT_SYNC_ID_COMMON])),
                    TEXT_SIZE_MEDIUM));
                /* 目标请求的包不正确，且误差很严重 */
                if (iap_info.reqpackseq[BOOT_SYNC_ID_COMMON] < iap_info.packseq
                    || iap_info.reqpackseq[BOOT_SYNC_ID_COMMON] > iap_info.packseq + 1) {
                    ui->upgrade_log->append(TEXT_COLOR_RED(
                        upgrade_id_list[BOOT_SYNC_ID_COMMON] + QString("-Error: 目标在请求错误的包：reqseq = ")
                            + QString::number(iap_info.reqpackseq[BOOT_SYNC_ID_COMMON]),
                        TEXT_SIZE_MEDIUM));
                    iap_info.status = IAP_DOWNLOAD_END;
                    ret             = -1;
                }
                /* 本次发送的包没有被收到，重新发送本次数据包 */
                /* 其实这个条件并不会被触发，因为目标如果没收到，就不会有这次NAK回复了 */
                if (iap_info.reqpackseq[BOOT_SYNC_ID_COMMON] == iap_info.packseq) {
                    ui->upgrade_log->append(TEXT_COLOR_BLUE(
                        upgrade_id_list[BOOT_SYNC_ID_COMMON]
                            + QString("重新发送数据包... seq = " + QString::number(iap_info.packseq)
                                      + ", reqseq = " + QString::number(iap_info.reqpackseq[BOOT_SYNC_ID_COMMON])),
                        TEXT_SIZE_MEDIUM));
                }
                /* 目标收到了重复的包 */
                /* 这可能是由于目标的ACK包没被PC收到，于是PC重新发包导致的 */
                if (iap_info.reqpackseq[BOOT_SYNC_ID_COMMON] == (iap_info.packseq + 1)) {
                    ui->upgrade_log->append(TEXT_COLOR_BLUE(
                        upgrade_id_list[BOOT_SYNC_ID_COMMON] + QString("目标收到了重复的包!"), TEXT_SIZE_MEDIUM));
                }
            }
            ++iap_info.packseq;
            iap_info.write_size += iap_info.packlen;
            ui->upgrade_progressBar->setValue(iap_info.write_size);
            if (iap_info.write_size >= iap_info.fw_size) {
                iap_info.status = IAP_DOWNLOAD_FINISH;
                ui->upgrade_log->append(TEXT_COLOR_GREEN(QString("Download finish!"), TEXT_SIZE_MEDIUM));
                ret = -1;
            }
        }
    }
    *retry_cnt = retry;
    return ret;
}

int MainWindow::upgrade_ack_eot_result_phase(qint64 starttime)
{
    bool   result  = false;
    qint64 endtime = 0;
    if (iap_info.upgrade_mode == UPGRADE_SYNCHRONOUS) {
        result = (iap_info.ack_nack[BOOT_SYNC_ID_A] == SUB_BL_EOT_ACK
                  && iap_info.ack_nack[BOOT_SYNC_ID_B] == SUB_BL_EOT_ACK);
    } else {
        result = (iap_info.ack_nack[BOOT_SYNC_ID_COMMON] == SUB_BL_EOT_ACK);
    }

    if (result) {
        for (uint8_t i = BOOT_SYNC_ID_COMMON; i < BOOT_SYNC_ID_MAX; i++) {
            if (iap_info.upgrade_mode == UPGRADE_SYNCHRONOUS && i == BOOT_SYNC_ID_COMMON) {
                continue;
            } else if (iap_info.upgrade_mode == UPGRADE_INDEPENDENT && i != BOOT_SYNC_ID_COMMON) {
                continue;
            }
            switch (iap_info.error_code[i]) {
            case 0:
                if (iap_info.status != IAP_DOWNLOAD_END) {
                    iap_info.status = IAP_DOWNLOAD_WAIT_APP_BOOT;
                }
                if ((i == BOOT_SYNC_ID_B || i == BOOT_SYNC_ID_COMMON)
                    && iap_info.status == IAP_DOWNLOAD_WAIT_APP_BOOT) {
                    ui->upgrade_log->append(TEXT_COLOR_GREEN(QString("Download success!"), TEXT_SIZE_MEDIUM));
                    endtime = QDateTime::currentMSecsSinceEpoch();
                    ui->upgrade_log->append(TEXT_COLOR_GREEN(
                        QString("Total tim: " + QString::number(endtime - starttime) + "ms " + ", Average speed:"
                                + QString::number(((iap_info.fw_size) / (endtime - starttime))) + "KB/s"),
                        TEXT_SIZE_MEDIUM));
                }

                break;
            case -1:
                iap_info.status = IAP_DOWNLOAD_END;
                ui->upgrade_log->append(
                    TEXT_COLOR_RED(upgrade_id_list[i] + QString(" 下载丢包，接收固件不完整"), TEXT_SIZE_MEDIUM));
                break;
            case -2:
                iap_info.status = IAP_DOWNLOAD_END;
                ui->upgrade_log->append(
                    TEXT_COLOR_RED(upgrade_id_list[i] + QString(" MD5 verify fail!"), TEXT_SIZE_MEDIUM));
                break;
            case -3:
                iap_info.status = IAP_DOWNLOAD_END;
                ui->upgrade_log->append(TEXT_COLOR_RED(upgrade_id_list[i] + QString(" Copy fail"), TEXT_SIZE_MEDIUM));
                break;
            default:
                iap_info.status = IAP_DOWNLOAD_END;
                ui->upgrade_log->append(TEXT_COLOR_RED(upgrade_id_list[i] + QString(" 未知错误"), TEXT_SIZE_MEDIUM));
                break;
            }
        }
    } else {
        iap_info.status = IAP_DOWNLOAD_END;
        ui->upgrade_log->append(TEXT_COLOR_RED("错误结束", TEXT_SIZE_MEDIUM));
    }
    return 0;
}

int MainWindow::boot_upgrade_thread()
{
#define PACK_LEN 512
    static struct pt* pt = &pt_upgrade;
    static uint16_t   payloadlen;
    static uint8_t    frame[TRANS_BUFF_LEN];
    static uint8_t    retry;
    static qint64     start_time;
    static qint64     starttime = QDateTime::currentMSecsSinceEpoch();  //用于计算升级用时
    static uint8_t    bootid    = BOOT_SYNC_ID_COMMON;
    PT_BEGIN(pt);
    PT_YIELD_FLAG = PT_YIELD_FLAG;
    starttime     = QDateTime::currentMSecsSinceEpoch();
    ui->upgrade_progressBar->setMaximum(iap_info.fw_size);
    if (iap_info.status == IAP_DOWNLOAD_IDLE) {
        ui->upgrade_log->append(TEXT_COLOR_GREEN(QString("Downloading file size = " + QString::number(iap_info.fw_size)
                                                         + ",pack len = " + QString::number(PACK_LEN)),
                                                 TEXT_SIZE_MEDIUM));
        retry = 0;
        while (1) {
            payloadlen = 4 + 1 + strlen(app_tail.fw_name) + 1 + strlen(app_tail.fw_version);
            ui->upgrade_log->append(
                TEXT_COLOR_GREEN(QString("SOH: " + QString::number(iap_info.packseq)), TEXT_SIZE_MEDIUM));
            frame[0]  = 0;
            frame[1]  = CMD_TYPE_BL;
            frame[2]  = CMD_BL_DATA_TRANSFER;
            frame[3]  = SUB_BL_DT_SOH;
            frame[4]  = payloadlen & 0x00ff;
            frame[5]  = (payloadlen >> 8) & 0x00ff;
            frame[6]  = iap_info.fw_size & 0x000000ff;
            frame[7]  = (iap_info.fw_size >> 8) & 0x000000ff;
            frame[8]  = (iap_info.fw_size >> 16) & 0x000000ff;
            frame[9]  = (iap_info.fw_size >> 24) & 0x000000ff;
            frame[10] = strlen(app_tail.fw_name);
            memset(&(frame[11]), '\0', strlen(app_tail.fw_name));
            strncpy(( char* )&(frame[11]), app_tail.fw_name, strlen(app_tail.fw_name)); /* 固件名称 */
            frame[11 + strlen(app_tail.fw_name)] = strlen(app_tail.fw_version);
            memset(&(frame[12 + strlen(app_tail.fw_name)]), '\0', strlen(app_tail.fw_version));
            strncpy(( char* )&(frame[12 + strlen(app_tail.fw_name)]), app_tail.fw_version,
                    strlen(app_tail.fw_version)); /* 固件名称 */
            iap_info.packseq = 0;
            memset(( uint8_t* )iap_info.ack_nack, 0, sizeof(iap_info.ack_nack));
            memset(( uint8_t* )iap_info.error_code, 0, sizeof(iap_info.error_code));
            my_serial->port_sendframe(frame, payloadlen + 6);
            start_time = QDateTime::currentMSecsSinceEpoch();
            if (iap_info.upgrade_mode == UPGRADE_SYNCHRONOUS) {
                PT_WAIT_UNTIL(pt, (iap_info.ack_nack[BOOT_SYNC_ID_A] == SUB_BL_SOH_ACK
                                   && iap_info.ack_nack[BOOT_SYNC_ID_B] == SUB_BL_SOH_ACK)
                                      || (QDateTime::currentMSecsSinceEpoch() - start_time > 3000));
            } else {
                PT_WAIT_UNTIL(pt, (iap_info.ack_nack[BOOT_SYNC_ID_COMMON] == SUB_BL_SOH_ACK)
                                      || (QDateTime::currentMSecsSinceEpoch() - start_time > 1500));
            }
            //            my_serial->port_sendframe(frame, payloadlen + 6);
            //            start_time = QDateTime::currentMSecsSinceEpoch();
            //            if (iap_info.upgrade_mode == UPGRADE_SYNCHRONOUS) {
            //                PT_WAIT_UNTIL(pt, (iap_info.ack_nack[BOOT_SYNC_ID_A] == SUB_BL_SOH_ACK
            //                                   && iap_info.ack_nack[BOOT_SYNC_ID_B] == SUB_BL_SOH_ACK)
            //                                      || (QDateTime::currentMSecsSinceEpoch() - start_time > 3000));
            //            } else {
            //                PT_WAIT_UNTIL(pt, (iap_info.ack_nack[BOOT_SYNC_ID_COMMON] == SUB_BL_SOH_ACK)
            //                                      || (QDateTime::currentMSecsSinceEpoch() - start_time > 1000));
            //            }
            bootid = BOOT_SYNC_ID_COMMON;
            if (upgrade_ack_soh_result_phase(&retry, &bootid) != 0) {
                break;
            }
        }
    }

    if (iap_info.status == IAP_DOWNLOADING) {
        retry = 0;
        do {
            ui->upgrade_log->append(
                TEXT_COLOR_GREEN(QString("发送数据包... seq:" + QString::number(iap_info.packseq)), TEXT_SIZE_MEDIUM));
            frame[0] = 0;
            frame[1] = CMD_TYPE_BL;
            frame[2] = CMD_BL_DATA_TRANSFER;
            frame[3] = SUB_BL_DT_STX;
            frame[6] = iap_info.packseq & 0x000000ff;
            frame[7] = (iap_info.packseq >> 8) & 0x000000ff;
            frame[8] = (iap_info.packseq >> 16) & 0x000000ff;
            frame[9] = (iap_info.packseq >> 24) & 0x000000ff;
            if (iap_info.write_size + PACK_LEN <= iap_info.fw_size) {
                iap_info.packlen = PACK_LEN;
            } else {
                iap_info.packlen = iap_info.fw_size - iap_info.write_size;
            }
            memcpy(&frame[10], iap_info.filebuf + iap_info.write_size, iap_info.packlen);
            payloadlen = 4 + iap_info.packlen; /* 固件 + 4字节的seq */
            frame[4]   = payloadlen & 0x00ff;
            frame[5]   = (payloadlen >> 8) & 0x00ff;
            memset(( uint8_t* )iap_info.ack_nack, 0, sizeof(iap_info.ack_nack));
            memset(( uint8_t* )iap_info.error_code, 0, sizeof(iap_info.error_code));
            /* 发送 frame = payload + 6字节的控制串 */
            my_serial->port_sendframe(frame, payloadlen + 6);
            /* 等待目标返回ACK */
            start_time = QDateTime::currentMSecsSinceEpoch();
            if (iap_info.upgrade_mode == UPGRADE_SYNCHRONOUS) {
                PT_WAIT_UNTIL(pt, (iap_info.ack_nack[BOOT_SYNC_ID_A] == SUB_BL_STX_ACK
                                   && iap_info.ack_nack[BOOT_SYNC_ID_B] == SUB_BL_STX_ACK)
                                      || (QDateTime::currentMSecsSinceEpoch() - start_time > 3000));
            } else {
                PT_WAIT_UNTIL(pt, (iap_info.ack_nack[BOOT_SYNC_ID_COMMON] == SUB_BL_STX_ACK)
                                      || (QDateTime::currentMSecsSinceEpoch() - start_time > 1000));
            }
            if (upgrade_ack_stx_result_phase(&retry, &bootid) != 0) {
                break;
            }
        } while (1);
    }

    if (iap_info.status == IAP_DOWNLOAD_FINISH) {
        payloadlen = 16;
        frame[0]   = 0;
        frame[1]   = CMD_TYPE_BL;
        frame[2]   = CMD_BL_DATA_TRANSFER;
        frame[3]   = SUB_BL_DT_EOT;
        frame[4]   = payloadlen;
        frame[5]   = 0;
        memcpy(&frame[6], iap_info.md5, 16);
        memset(( uint8_t* )iap_info.ack_nack, 0, sizeof(iap_info.ack_nack));
        memset(( uint8_t* )iap_info.error_code, 0, sizeof(iap_info.error_code));
        memset(( uint8_t* )iap_info.app_start_flag, 0, sizeof(iap_info.app_start_flag));
        my_serial->port_sendframe(frame, payloadlen + 6);
        start_time = QDateTime::currentMSecsSinceEpoch();
        if (iap_info.upgrade_mode == UPGRADE_SYNCHRONOUS) {
            PT_WAIT_UNTIL(pt, (iap_info.ack_nack[BOOT_SYNC_ID_A] == SUB_BL_EOT_ACK
                               && iap_info.ack_nack[BOOT_SYNC_ID_B] == SUB_BL_EOT_ACK)
                                  || (QDateTime::currentMSecsSinceEpoch() - start_time > 3000));
        } else {
            PT_WAIT_UNTIL(pt, (iap_info.ack_nack[BOOT_SYNC_ID_COMMON] == SUB_BL_EOT_ACK)
                                  || (QDateTime::currentMSecsSinceEpoch() - start_time > 1000));
        }
        upgrade_ack_eot_result_phase(starttime);
    }

    if (iap_info.status == IAP_DOWNLOAD_WAIT_APP_BOOT) {
        boot_status = SUB_BL_STS_START;
        start_time  = QDateTime::currentMSecsSinceEpoch();
        if (iap_info.upgrade_mode == UPGRADE_SYNCHRONOUS) {
            PT_WAIT_UNTIL(pt,
                          (iap_info.app_start_flag[BOOT_SYNC_ID_A] == 1 && iap_info.app_start_flag[BOOT_SYNC_ID_B] == 1)
                              || (QDateTime::currentMSecsSinceEpoch() - start_time > 3000));
        } else {
            PT_WAIT_UNTIL(pt, (iap_info.app_start_flag[BOOT_SYNC_ID_COMMON] == 1)
                                  || (QDateTime::currentMSecsSinceEpoch() - start_time > 1000));
        }
        if ((iap_info.upgrade_mode == UPGRADE_SYNCHRONOUS)) {
            if ((iap_info.app_start_flag[BOOT_SYNC_ID_A] == 1 && iap_info.app_start_flag[BOOT_SYNC_ID_B] == 1)) {
                boot_status = SUB_BL_STS_APP_RUN;
            }
        } else {
            if ((iap_info.app_start_flag[BOOT_SYNC_ID_COMMON] == 1)) {
                boot_status = SUB_BL_STS_APP_RUN;
            }
        }
        iap_info.status = IAP_DOWNLOAD_END;
        if (boot_status == SUB_BL_STS_APP_RUN) {
            ui->upgrade_log->append(TEXT_COLOR_GREEN(QString("APP successfully boot!"), TEXT_SIZE_MEDIUM));
            ui->upgrade_log->append(TEXT_COLOR_BLUE(QString("固件升级成功^ v ^"), TEXT_SIZE_MEDIUM));
        } else {
            ui->upgrade_log->append(TEXT_COLOR_RED(QString("APP boot fail!"), TEXT_SIZE_MEDIUM));
        }
        iap_info.status = IAP_DOWNLOAD_END;
    }
    PT_END(pt);
}

bool MainWindow::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == ui->serial_port_comboBox) {
        if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->buttons() & Qt::LeftButton) {
                serial_search();
            }
        }
    }
    return QWidget::eventFilter(watched, event);
}

void MainWindow::on_serial_switch_pushButton_clicked()
{
    serial_switch_ctrl();
}

void MainWindow::on_select_fw_pushButton_clicked()
{
    QString curPath  = QDir::currentPath();                     //获取系统当前目录
    QString dlgTitle = "打开一个*-app-*.bin或*_APP_*.bin文件";  //对话框标题
    QString filter   = "升级固件(*app*.bin *APP*.bin)";         //文件过滤器
    QString filename = QFileDialog::getOpenFileName(this, dlgTitle, curPath, filter);
    if (filename == "") {
        return;
    }
    int ret = 0;
    while (1) {
        int a = filename.indexOf("/", ret + 1);
        if (a >= 0)
            ret = a;
        else
            break;
    }
    firmware_filename = filename.mid(ret + 1);
    if (!firmware_filename.contains(module_name_list[ui->select_module_comboBox->currentIndex()], Qt::CaseSensitive)) {
        firmware_filename.clear();
        my_message_box("升级警告", "升级文件与所选升级模块不匹配，请检查！");
        return;
    }
    firmware_pathname = filename;
    ui->firmware_name_lineedit->setText(firmware_filename);
    ui->start_upgrade_pushButton->setEnabled(true);
}

void MainWindow::on_start_upgrade_pushButton_clicked()
{
    QStringList upgade_name_list;
    upgade_name_list << "独立升级"
                     << "同步升级";
    QFile qfile(firmware_pathname);
    if (!qfile.exists()) {
        ui->firmware_name_lineedit->clear();
        ui->start_upgrade_pushButton->setEnabled(false);
        my_message_box("升级警告", "升级文件不存在，请检查！");
        return;
    }
    uint32_t   file_size = 0;
    QByteArray filebuffer;
    memset(&iap_info, 0, sizeof(iap_info));
    qfile.open(QIODevice::ReadOnly);
    ui->upgrade_log->setText(TEXT_COLOR_GREEN(QString("固件名称: " + firmware_filename), TEXT_SIZE_MEDIUM));
    ui->upgrade_log->append(TEXT_COLOR_GREEN(QString("固件大小: " + QString::number(qfile.size())), TEXT_SIZE_MEDIUM));

    file_size        = qfile.size();
    filebuffer       = qfile.readAll();
    iap_info.filebuf = filebuffer.data();

    if (iap_info.filebuf == NULL) {
        ui->upgrade_log->append(TEXT_COLOR_RED(QString("ERROR: 文件空间错误"), TEXT_SIZE_LARGE));
        qfile.close();
        return;
    }

    memcpy(&app_tail, iap_info.filebuf + file_size - sizeof(firmware_info_t), sizeof(firmware_info_t));
    firmware_info_encrypt_decrypt(&app_tail, KEY_DECRYPT);
    unsigned char app_tail_md5[16];
    mbedtls_md5(( const unsigned char* )&app_tail, sizeof(app_tail) - 16, app_tail_md5);
    if (memcmp(app_tail.fw_info_md5, app_tail_md5, 16)) {
        ui->upgrade_log->append(TEXT_COLOR_RED(QString("ERROR: 密钥1校验失败"), TEXT_SIZE_LARGE));
        return;
    }
    if (app_tail.magic != VALID_MAGIC) {
        ui->upgrade_log->append(
            TEXT_COLOR_RED(QString("ERROR: 文件码错误，请检查固件是否于正规渠道获取！"), TEXT_SIZE_LARGE));
        qfile.close();
        return;
    }
    QString file_name_check(app_tail.fw_name);
    if (!file_name_check.contains(module_name_list[ui->select_module_comboBox->currentIndex()], Qt::CaseSensitive)) {
        ui->upgrade_log->append(
            TEXT_COLOR_RED(QString("ERROR: 文件验证失败，升级文件与所选升级模块不匹配！"), TEXT_SIZE_LARGE));
        qfile.close();
        return;
    }
    iap_info.fw_size = app_tail.fw_length;
    qfile.close();
    QByteArray temp(app_tail.fw_name);
    QString    str1(temp);
    ui->upgrade_log->append(TEXT_COLOR_GREEN(QString("frimware name = " + str1), TEXT_SIZE_MEDIUM));
    ui->upgrade_log->append(
        TEXT_COLOR_GREEN(QString("frimware size = " + QString::number(app_tail.fw_length)), TEXT_SIZE_LARGE));

    mbedtls_md5(( const unsigned char* )iap_info.filebuf, iap_info.fw_size, iap_info.md5);
    if (memcmp(iap_info.md5, app_tail.fw_md5, 16)) {
        ui->upgrade_log->append(TEXT_COLOR_RED(QString("ERROR: 密钥2校验失败"), TEXT_SIZE_LARGE));
        return;
    }
    iap_info.upgrade_mode = UPGRADE_SYNCHRONOUS;
    memset(iap_info.bl_start_flag, 0, sizeof(iap_info.bl_start_flag));
    unsigned char cmd[21] = { 0xaa, 0xaa, 0x00, 0x20, 0x21, 0x00, 0x09, 0x00, 0x0d, 0x74, 0x6F,
                              0x5F, 0x62, 0x6F, 0x6F, 0x74, 0x0d, 0xCD, 0xC8, 0x55, 0x55 };
    my_serial->data_decrypt(cmd, 21);
    my_serial->port_senddata(cmd, 21);
    ui->upgrade_log->append(TEXT_COLOR_GREEN(QString("发送复位指令"), TEXT_SIZE_MEDIUM));
    ui->upgrade_log->append(TEXT_COLOR_GREEN(QString("等待BootLoader的启动信号····"), TEXT_SIZE_MEDIUM));
    ui->upgrade_log->append(TEXT_COLOR_GREEN(QString("若目标板无自动reset功能，请保持此程序等待，然后直接给目标板"
                                                     "断电重连！,若想退出本次下载，请点击“退出下载”按钮"),
                                             TEXT_SIZE_MEDIUM));
    ui->start_upgrade_pushButton->setEnabled(false);
    ui->select_fw_pushButton->setEnabled(false);
    ui->quit_upgrade_pushButton->setEnabled(true);
    upgrade_quit_flag = DISABLE_FLAG;
    PT_INIT(&pt_upgrade);
    while (boot_status != SUB_BL_STS_WAITING) {
        QApplication::processEvents();           //如果无此函数  则主程序卡死  程序无响应
        if (upgrade_quit_flag == ENABLE_FLAG) {  //退出升级模式
            break;
        }
        if (iap_info.bl_start_flag[BOOT_SYNC_ID_COMMON] == 1
            || (iap_info.bl_start_flag[BOOT_SYNC_ID_A] == 1 && iap_info.bl_start_flag[BOOT_SYNC_ID_B] == 1)) {
            boot_status = SUB_BL_STS_WAITING;
        }
    }

    ui->quit_upgrade_pushButton->setEnabled(false);

    ui->quit_upgrade_pushButton->setEnabled(false);
    if (upgrade_quit_flag == ENABLE_FLAG) {
        upgrade_quit_flag = DISABLE_FLAG;
        ui->upgrade_log->append(TEXT_COLOR_RED(
            QString("本次升级已退出，若要进行升级请重新选择固件并点击“开始升级”按钮"), TEXT_SIZE_MEDIUM));
    } else {
        ui->start_upgrade_pushButton->setEnabled(false);
        ui->serial_switch_pushButton->setEnabled(false);
        ui->select_fw_pushButton->setEnabled(false);
        ui->upgrade_log->append(
            TEXT_COLOR_BLUE("升级模式：" + upgade_name_list[iap_info.upgrade_mode], TEXT_SIZE_MEDIUM));
        while (iap_info.status != IAP_DOWNLOAD_END) {
            boot_upgrade_thread();
            QApplication::processEvents();  //如果无此函数  则主程序卡死  程序无响应
        }
    }
    ui->start_upgrade_pushButton->setEnabled(true);
    ui->serial_switch_pushButton->setEnabled(true);
    ui->select_fw_pushButton->setEnabled(true);
}

void MainWindow::on_quit_upgrade_pushButton_clicked()
{
    upgrade_quit_flag = ENABLE_FLAG;
}
