#include "project_transmit.h"
#include "QDateTime"
#include "algorithm/MD5/md5.h"
#include "mainwindow.h"
#include "qdebug.h"
#include "qfiledialog.h"
#include <QElapsedTimer>
#include <QFont>
#include <QFontDialog>
#include <QFormLayout>
#include <QInputDialog>
#include <cstdio>
project_transmit::project_transmit(QWidget* mparent, QWidget* parent)
    : QWidget(parent)
{
    ui                           = MainWindow::my_ui->ui;
    mainwindow                   = ( MainWindow* )mparent;
    project_transmit_info.status = PROJECT_DOWNLOAD_IDLE;
    PT_INIT(&pt_download);
    sync_id_list << "COMMON"
                 << "A"
                 << "B";
    init();
}

void project_transmit::init()
{
    project_rw_dialog.setWindowTitle("设备工程");
    project_rw_dialog.setWindowFlags(Qt::Tool | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
    project_rw_dialog.setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    project_rw_dialog.setWindowFlag(Qt::MSWindowsFixedSizeDialogHint);
    pro_progress.setMaximum(0);
    pro_progress.setMinimumWidth(200);
    QFormLayout* layout = new QFormLayout(&project_rw_dialog);
    layout->addRow(&pro_progress);
    layout->addRow(&pro_rw_log);
    layout->setContentsMargins(10, 10, 10, 10);
    project_rw_dialog.setStyleSheet("QDialog { background-color: rgb(210,230,255); }");
}

/************************* 文件下载代码区 *************************/
/**
 * @brief lua::download_ack_soh_result_phase 下载起始包反馈结果解析
 * @param retry_cnt 重发次数
 * @return 解析结果 0-正常 <0-有错误
 */
int project_transmit::download_ack_soh_result_phase(uint8_t* retry_cnt)
{
    bool    is_repeat = false;
    uint8_t retry     = *retry_cnt;
    uint8_t ret       = 0;

    if (LUA_SYNC_DOWNLOAD) {  //双MCU同步升级
        if (project_transmit_info.ack[SYNC_ID_A] != SUB_PUBLIC_FILE_DOWNLOAD_SOH_ACK
            || project_transmit_info.ack[SYNC_ID_B] != SUB_PUBLIC_FILE_DOWNLOAD_SOH_ACK) {  //有节点没反馈
            if (project_transmit_info.ack[SYNC_ID_A] != SUB_PUBLIC_FILE_DOWNLOAD_SOH_ACK
                && project_transmit_info.ack[SYNC_ID_B] != SUB_PUBLIC_FILE_DOWNLOAD_SOH_ACK) {  //所有节点都没反馈
                pro_rw_log.append(TEXT_COLOR_RED(
                    "Warning: Wait all  SOH ACK timeout!, retry: " + QString::number(retry), TEXT_SIZE_MEDIUM));
            } else if (project_transmit_info.ack[SYNC_ID_B] != SUB_PUBLIC_FILE_DOWNLOAD_SOH_ACK) {  // B节点未反馈
                pro_rw_log.append(TEXT_COLOR_RED("Warning: Wait B  SOH ACK timeout!, retry: " + QString::number(retry),
                                                 TEXT_SIZE_MEDIUM));
            } else if (project_transmit_info.ack[SYNC_ID_A] != SUB_PUBLIC_FILE_DOWNLOAD_SOH_ACK) {  // A节点未反馈
                pro_rw_log.append(TEXT_COLOR_RED(
                    QString("Warning: Wait A  SOH ACK timeout!, retry: " + QString::number(retry)), TEXT_SIZE_MEDIUM));
            }
            if (++retry >= 5) {  //重发次数达到阈值
                project_transmit_info.status = PROJECT_DOWNLOAD_FAIL;
                pro_rw_log.append(TEXT_COLOR_RED(QString("retry SOH fail"), TEXT_SIZE_MEDIUM));
                ret = -1;
            }

        } else {
            for (uint8_t i = SYNC_ID_A; i <= SYNC_ID_B; i++) {  //解析节点反馈结果是异常/正常
                switch (project_transmit_info.error_code[i]) {
                case 0:
                    if (project_transmit_info.status != PROJECT_DOWNLOAD_FAIL) {
                        project_transmit_info.download_progress = PROJECT_DOWNLOAD_STX;
                    }
                    break;
                case -1:
                    project_transmit_info.status = PROJECT_DOWNLOAD_FAIL;
                    if (!is_repeat) {
                        mainwindow->my_message_box("设备工程与当前工程一致，无需重复写入", MESSAGE_TYPE_WARNING);
                        pro_rw_log.append(TEXT_COLOR_RED(sync_id_list[i] + "工程重复写入！", TEXT_SIZE_LARGE));
                        is_repeat = true;
                    }
                    break;
                case -2:
                    project_transmit_info.status = PROJECT_DOWNLOAD_FAIL;
                    pro_rw_log.append(TEXT_COLOR_RED(sync_id_list[i] + "-Error: 文件大小不符合！", TEXT_SIZE_LARGE));
                    break;
                default:
                    project_transmit_info.status = PROJECT_DOWNLOAD_FAIL;
                    pro_rw_log.append(TEXT_COLOR_RED(sync_id_list[i] + "Error: 未知错误！", TEXT_SIZE_LARGE));
                    break;
                }
            }
            ret = -1;
        }
    } else {  //单MCU升级 相同检测逻辑
        if (project_transmit_info.ack[SYNC_ID_COMMON] != SUB_PUBLIC_FILE_DOWNLOAD_SOH_ACK) {
            if (++retry >= 5) {
                project_transmit_info.status = PROJECT_DOWNLOAD_FAIL;
                pro_rw_log.append(
                    TEXT_COLOR_RED(sync_id_list[SYNC_ID_COMMON] + QString(" retry SOH fail"), TEXT_SIZE_MEDIUM));
                ret = -1;
            }
            pro_rw_log.append(TEXT_COLOR_RED(sync_id_list[SYNC_ID_COMMON]
                                                 + " Warning: Wait SOH ACK timeout!, retry: " + QString::number(retry),
                                             TEXT_SIZE_MEDIUM));
        } else {
            switch (project_transmit_info.error_code[SYNC_ID_COMMON]) {
            case 0:
                project_transmit_info.download_progress = PROJECT_DOWNLOAD_STX;
                break;
            case -1:
                project_transmit_info.status = PROJECT_DOWNLOAD_FAIL;
                pro_rw_log.append(
                    TEXT_COLOR_RED(sync_id_list[SYNC_ID_COMMON] + "-Error: 文件名过长，请检查！", TEXT_SIZE_LARGE));
                break;
            case -2:
                project_transmit_info.status = PROJECT_DOWNLOAD_FAIL;
                pro_rw_log.append(
                    TEXT_COLOR_RED(sync_id_list[SYNC_ID_COMMON] + "-Error: 文件大小不符合！", TEXT_SIZE_LARGE));
                break;
            default:
                project_transmit_info.status = PROJECT_DOWNLOAD_FAIL;
                pro_rw_log.append(
                    TEXT_COLOR_RED(sync_id_list[SYNC_ID_COMMON] + QString("-Error: 未知错误！"), TEXT_SIZE_LARGE));
                break;
            }
            ret = -1;
        }
    }
    *retry_cnt = retry;
    return ret;
}

/**
 * @brief lua::download_ack_stx_result_phase 下载数据包反馈结果解析
 * @param retry_cnt 同一数据包重发次数
 * @return 解析结果 0-正常 <0-有错误
 */
int project_transmit::download_ack_stx_result_phase(uint8_t* retry_cnt)
{
    uint8_t retry               = *retry_cnt;
    uint8_t ret                 = 0;
    bool    repeat[SYNC_ID_MAX] = { false, false, false };
    if (LUA_SYNC_DOWNLOAD) {
        /* 目标未反馈，重试 */
        if (project_transmit_info.ack[SYNC_ID_A] != SUB_PUBLIC_FILE_DOWNLOAD_STX_ACK
            || project_transmit_info.ack[SYNC_ID_B] != SUB_PUBLIC_FILE_DOWNLOAD_STX_ACK) {
            if (++retry < 10) {
                if (project_transmit_info.ack[SYNC_ID_B] != SUB_PUBLIC_FILE_DOWNLOAD_STX_ACK) {
                    pro_rw_log.append(TEXT_COLOR_RED(
                        "B-Warning: 等待目标反馈超时！重新发送..." + QString::number(retry), TEXT_SIZE_MEDIUM));
                } else if (project_transmit_info.ack[SYNC_ID_A] != SUB_PUBLIC_FILE_DOWNLOAD_STX_ACK) {
                    pro_rw_log.append(TEXT_COLOR_RED(
                        "A-Warning: 等待目标反馈超时！重新发送..." + QString::number(retry), TEXT_SIZE_MEDIUM));
                }
            } else {
                project_transmit_info.status = PROJECT_DOWNLOAD_FAIL;
                pro_rw_log.append(TEXT_COLOR_RED("SYNC-Error: 重发失败！", TEXT_SIZE_MEDIUM));
                ret = -1;
            }
        } else {
            retry = 0;
            for (uint8_t i = SYNC_ID_A; i <= SYNC_ID_B; i++) {
                if (project_transmit_info.error_code[i] == -1) {
                    pro_rw_log.append(
                        TEXT_COLOR_BLUE(sync_id_list[i] + "-Warning: 目标未收到正确的包!seq = "
                                            + QString::number(project_transmit_info.packseq)
                                            + ", reqseq = " + QString::number(project_transmit_info.reqpackseq[i]),
                                        TEXT_SIZE_MEDIUM));
                    /* 目标请求的包不正确，且误差很严重 */
                    if (project_transmit_info.reqpackseq[i] < project_transmit_info.packseq
                        || project_transmit_info.reqpackseq[i] > project_transmit_info.packseq + 1) {
                        pro_rw_log.append(TEXT_COLOR_RED(sync_id_list[i] + "Error: 目标在请求错误的包：reqseq = "
                                                             + QString::number(project_transmit_info.reqpackseq[i]),
                                                         TEXT_SIZE_MEDIUM));
                        project_transmit_info.status = PROJECT_DOWNLOAD_FAIL;
                        ret                          = -1;
                    }
                    /* 本次发送的包没有被收到，重新发送本次数据包 */
                    /* 其实这个条件并不会被触发，因为目标如果没收到，就不会有这次NAK回复了 */
                    if (project_transmit_info.reqpackseq[i] == project_transmit_info.packseq) {
                        pro_rw_log.append(
                            TEXT_COLOR_BLUE(sync_id_list[i] + "重新发送数据包... seq = "
                                                + QString::number(project_transmit_info.packseq)
                                                + ", reqseq = " + QString::number(project_transmit_info.reqpackseq[i]),
                                            TEXT_SIZE_MEDIUM));
                    }
                    /* 目标收到了重复的包 */
                    /* 这可能是由于目标的ACK包没被PC收到，于是PC重新发包导致的 */
                    if (project_transmit_info.reqpackseq[i] == (project_transmit_info.packseq + 1)) {
                        pro_rw_log.append(TEXT_COLOR_BLUE(sync_id_list[i] + "目标收到了重复的包!", TEXT_SIZE_MEDIUM));
                        repeat[i] = true;
                    } else {
                        project_transmit_info.download_progress = PROJECT_DOWNLOAD_SOH;
                    }
                } else if (project_transmit_info.error_code[i] == -2) {
                    pro_rw_log.append(TEXT_COLOR_RED(sync_id_list[i] + "-ERROR: 文件续传出错", TEXT_SIZE_MEDIUM));
                    ret = -2;
                } else if (project_transmit_info.error_code[i] == 0) {
                    pro_rw_log.append(TEXT_COLOR_GREEN(sync_id_list[i] + " receive success", TEXT_SIZE_MEDIUM));
                } else {
                    pro_rw_log.append(TEXT_COLOR_RED(sync_id_list[i] + "-ERROR: 未知错误", TEXT_SIZE_MEDIUM));
                    ret = -2;
                }
            }
            if ((project_transmit_info.error_code[SYNC_ID_A] == 0 && project_transmit_info.error_code[SYNC_ID_B] == 0)
                || (repeat[SYNC_ID_A] && repeat[SYNC_ID_B])) {
                ++project_transmit_info.packseq;
                project_transmit_info.write_size += project_transmit_info.packlen;
                pro_progress.setValue(project_transmit_info.write_size);
                if (project_transmit_info.write_size >= project_transmit_info.file_size) {
                    project_transmit_info.download_progress = PROJECT_DOWNLOAD_EOT;
                    pro_rw_log.append(TEXT_COLOR_GREEN(QString("Download finish!"), TEXT_SIZE_MEDIUM));
                    ret = -1;
                }
            }
        }
    } else {
        /* 目标未反馈，重试 */
        if (project_transmit_info.ack[SYNC_ID_COMMON] != SUB_PUBLIC_FILE_DOWNLOAD_STX_ACK) {
            if (++retry < 10) {
                pro_rw_log.append(TEXT_COLOR_BLUE("C-Warning: 等待目标反馈超时！重新发送...", TEXT_SIZE_MEDIUM));
            } else {
                project_transmit_info.status = PROJECT_DOWNLOAD_FAIL;
                pro_rw_log.append(TEXT_COLOR_RED(QString("C-Error: 重发失败！"), TEXT_SIZE_MEDIUM));
                ret = -1;
            }
        } else {
            retry = 0;
            /* 目标未收到正确的包 */
            if (project_transmit_info.error_code[SYNC_ID_COMMON] == -1) {
                pro_rw_log.append(TEXT_COLOR_BLUE(
                    sync_id_list[SYNC_ID_COMMON]
                        + QString("Warning: 目标未收到正确的包!seq = " + QString::number(project_transmit_info.packseq)
                                  + ", reqseq = " + QString::number(project_transmit_info.reqpackseq[SYNC_ID_COMMON])),
                    TEXT_SIZE_MEDIUM));
                /* 目标请求的包不正确，且误差很严重 */
                if (project_transmit_info.reqpackseq[SYNC_ID_COMMON] < project_transmit_info.packseq
                    || project_transmit_info.reqpackseq[SYNC_ID_COMMON] > project_transmit_info.packseq + 1) {
                    pro_rw_log.append(
                        TEXT_COLOR_RED(sync_id_list[SYNC_ID_COMMON] + QString("-Error: 目标在请求错误的包：reqseq = ")
                                           + QString::number(project_transmit_info.reqpackseq[SYNC_ID_COMMON]),
                                       TEXT_SIZE_MEDIUM));
                    project_transmit_info.status = PROJECT_DOWNLOAD_FAIL;
                    ret                          = -1;
                }
                /* 本次发送的包没有被收到，重新发送本次数据包 */
                /* 其实这个条件并不会被触发，因为目标如果没收到，就不会有这次NAK回复了 */
                if (project_transmit_info.reqpackseq[SYNC_ID_COMMON] == project_transmit_info.packseq) {
                    pro_rw_log.append(TEXT_COLOR_BLUE(
                        sync_id_list[SYNC_ID_COMMON]
                            + "重新发送数据包... seq = " + QString::number(project_transmit_info.packseq)
                            + ", reqseq = " + QString::number(project_transmit_info.reqpackseq[SYNC_ID_COMMON]),
                        TEXT_SIZE_MEDIUM));
                }
                /* 目标收到了重复的包 */
                /* 这可能是由于目标的ACK包没被PC收到，于是PC重新发包导致的 */
                if (project_transmit_info.reqpackseq[SYNC_ID_COMMON] == (project_transmit_info.packseq + 1)) {
                    pro_rw_log.append(TEXT_COLOR_BLUE(sync_id_list[SYNC_ID_COMMON] + QString("目标收到了重复的包!"),
                                                      TEXT_SIZE_MEDIUM));
                }
            } else if (project_transmit_info.error_code[SYNC_ID_COMMON] == -2) {
                pro_rw_log.append(TEXT_COLOR_RED("COMMON-ERROR: 文件续传出错", TEXT_SIZE_MEDIUM));
            } else if (project_transmit_info.error_code[SYNC_ID_COMMON] == 0) {
                pro_rw_log.append(TEXT_COLOR_GREEN("receive success", TEXT_SIZE_MEDIUM));
            } else {
                pro_rw_log.append(TEXT_COLOR_RED("COMMON-ERROR: 未知错误", TEXT_SIZE_MEDIUM));
            }
            ++project_transmit_info.packseq;
            project_transmit_info.write_size += project_transmit_info.packlen;
            if (project_transmit_info.write_size >= project_transmit_info.file_size) {
                project_transmit_info.download_progress = PROJECT_DOWNLOAD_EOT;
                pro_rw_log.append(TEXT_COLOR_GREEN(QString("Download finish!"), TEXT_SIZE_MEDIUM));
                ret = -1;
            }
        }
    }
    *retry_cnt = retry;
    return ret;
}

/**
 * @brief lua::download_ack_eot_result_phase 下载结束包反馈结果解析
 * @param starttime 开始下载的时间，用于计算整个下载过程使用了多久时间
 * @return 解析结果 0-正常 <0-有错误
 */
int project_transmit::download_ack_eot_result_phase(qint64 starttime)
{
    bool   result  = false;
    qint64 endtime = 0;
    if (LUA_SYNC_DOWNLOAD) {
        result = (project_transmit_info.ack[SYNC_ID_A] == SUB_PUBLIC_FILE_DOWNLOAD_EOT_ACK
                  && project_transmit_info.ack[SYNC_ID_B] == SUB_PUBLIC_FILE_DOWNLOAD_EOT_ACK);
    } else {
        result = (project_transmit_info.ack[SYNC_ID_COMMON] == SUB_PUBLIC_FILE_DOWNLOAD_EOT_ACK);
    }
    if (result) {
        for (uint8_t i = SYNC_ID_COMMON; i < SYNC_ID_MAX; i++) {
            if (LUA_SYNC_DOWNLOAD && (i == SYNC_ID_COMMON)) {
                continue;
            } else if (!LUA_SYNC_DOWNLOAD && (i != SYNC_ID_COMMON)) {
                continue;
            }
            switch (project_transmit_info.error_code[i]) {
            case 0:
                if (project_transmit_info.status != PROJECT_DOWNLOAD_FAIL) {
                    project_transmit_info.status = PROJECT_DOWNLOAD_SUCCESS;
                }
                if ((i == SYNC_ID_B || i == SYNC_ID_COMMON)
                    && project_transmit_info.status == PROJECT_DOWNLOAD_SUCCESS) {
                    pro_rw_log.append(TEXT_COLOR_GREEN(QString("Download success!"), TEXT_SIZE_OVER));
                    endtime = QDateTime::currentMSecsSinceEpoch();
                    pro_rw_log.append(TEXT_COLOR_GREEN(
                        QString("Total tim: " + QString::number(endtime - starttime) + "ms " + ", Average speed:"
                                + QString::number(((project_transmit_info.file_size) / (endtime - starttime)))
                                + "KB/s"),
                        TEXT_SIZE_MEDIUM));
                }
                break;
            case -1:
                project_transmit_info.status = PROJECT_DOWNLOAD_FAIL;
                pro_rw_log.append(
                    TEXT_COLOR_RED(sync_id_list[i] + QString(" 下载丢包，接收文件不完整"), TEXT_SIZE_MEDIUM));
                break;
            case -2:
                project_transmit_info.status = PROJECT_DOWNLOAD_FAIL;
                pro_rw_log.append(TEXT_COLOR_RED(sync_id_list[i] + QString(" MD5 verify fail!"), TEXT_SIZE_MEDIUM));
                break;
            default:
                project_transmit_info.status = PROJECT_DOWNLOAD_FAIL;
                pro_rw_log.append(TEXT_COLOR_RED(sync_id_list[i] + QString(" 未知错误"), TEXT_SIZE_MEDIUM));
                break;
            }
        }
    } else {
        project_transmit_info.status = PROJECT_DOWNLOAD_FAIL;
        pro_rw_log.append(TEXT_COLOR_RED("错误结束", TEXT_SIZE_MEDIUM));
    }
    return 0;
}

/**
 * @brief project_transmit_thread 文件下载线程，使用PT，在while1内运行
 * @return
 */
int project_transmit::project_transmit_thread()
{
#define PACK_LEN 480
    static struct pt* pt = &pt_download;
    static uint16_t   payloadlen;
    static uint8_t    frame[TRANS_BUFF_LEN];
    static uint8_t    retry;
    static qint64     wait_tick;
    static qint64     starttime = QDateTime::currentMSecsSinceEpoch();  //用于计算升级用时
    PT_BEGIN(pt);
    PT_YIELD_FLAG = PT_YIELD_FLAG;
    starttime     = QDateTime::currentMSecsSinceEpoch();
    pro_progress.setValue(0);
    pro_progress.setMaximum(project_transmit_info.file_size);
    if (project_transmit_info.download_progress == PROJECT_DOWNLOAD_SOH) {
        retry = 0;
        pro_rw_log.append(TEXT_COLOR_GREEN(" START SOH", TEXT_SIZE_MEDIUM));
        while (1) {
            payloadlen = sizeof(transmit_project_info);
            memcpy(frame, ( char* )&transmit_project_info, sizeof(transmit_project_info));
            project_transmit_info.packseq    = 0;
            project_transmit_info.write_size = 0;
            memset(( uint8_t* )project_transmit_info.ack, 0, sizeof(project_transmit_info.ack));
            memset(( uint8_t* )project_transmit_info.error_code, 0, sizeof(project_transmit_info.error_code));
            mainwindow->my_serial->port_cmd_sendframe(0, CMD_TYPE_PROJECT, CMD_PUBLIC_FILE_DOWNLOAD,
                                                      SUB_PUBLIC_FILE_DOWNLOAD_SOH, frame, payloadlen);
            wait_tick = QDateTime::currentMSecsSinceEpoch();
            if (LUA_SYNC_DOWNLOAD) {
                PT_WAIT_UNTIL(pt, (project_transmit_info.ack[SYNC_ID_A] == SUB_PUBLIC_FILE_DOWNLOAD_SOH_ACK
                                   && project_transmit_info.ack[SYNC_ID_B] == SUB_PUBLIC_FILE_DOWNLOAD_SOH_ACK)
                                      || (QDateTime::currentMSecsSinceEpoch() - wait_tick > 2000));
            } else {
                PT_WAIT_UNTIL(pt, (project_transmit_info.ack[SYNC_ID_COMMON] == SUB_PUBLIC_FILE_DOWNLOAD_SOH_ACK)
                                      || (QDateTime::currentMSecsSinceEpoch() - wait_tick > 1500));
            }

            if (download_ack_soh_result_phase(&retry) != 0) {
                break;
            }
        }
    }

    if (project_transmit_info.download_progress == PROJECT_DOWNLOAD_STX) {
        retry = 0;
        do {
            pro_rw_log.append(TEXT_COLOR_GREEN(
                QString("发送文件... seq:" + QString::number(project_transmit_info.packseq)), TEXT_SIZE_MEDIUM));
            frame[0] = project_transmit_info.packseq & 0x000000ff;
            frame[1] = (project_transmit_info.packseq >> 8) & 0x000000ff;
            frame[2] = (project_transmit_info.packseq >> 16) & 0x000000ff;
            frame[3] = (project_transmit_info.packseq >> 24) & 0x000000ff;
            if (project_transmit_info.write_size + PACK_LEN <= project_transmit_info.file_size) {
                project_transmit_info.packlen = PACK_LEN;
            } else {
                project_transmit_info.packlen = project_transmit_info.file_size - project_transmit_info.write_size;
            }
            memcpy(&frame[4], project_transmit_info.file_buf + project_transmit_info.write_size,
                   project_transmit_info.packlen);
            payloadlen = 4 + project_transmit_info.packlen; /* 固件 + 4字节的seq */
            memset(( uint8_t* )project_transmit_info.ack, 0, sizeof(project_transmit_info.ack));
            memset(( uint8_t* )project_transmit_info.error_code, 0, sizeof(project_transmit_info.error_code));
            mainwindow->my_serial->port_cmd_sendframe(0, CMD_TYPE_PROJECT, CMD_PUBLIC_FILE_DOWNLOAD,
                                                      SUB_PUBLIC_FILE_DOWNLOAD_STX, frame, payloadlen);
            /* 等待目标返回ACK */
            wait_tick = QDateTime::currentMSecsSinceEpoch();
            if (LUA_SYNC_DOWNLOAD) {
                PT_WAIT_UNTIL(pt, (project_transmit_info.ack[SYNC_ID_A] == SUB_PUBLIC_FILE_DOWNLOAD_STX_ACK
                                   && project_transmit_info.ack[SYNC_ID_B] == SUB_PUBLIC_FILE_DOWNLOAD_STX_ACK)
                                      || (QDateTime::currentMSecsSinceEpoch() - wait_tick > 3000));
            } else {
                PT_WAIT_UNTIL(pt, (project_transmit_info.ack[SYNC_ID_COMMON] == SUB_PUBLIC_FILE_DOWNLOAD_STX_ACK)
                                      || (QDateTime::currentMSecsSinceEpoch() - wait_tick > 1000));
            }

            if (download_ack_stx_result_phase(&retry) != 0) {
                break;
            }
        } while (1);
    }
    if (project_transmit_info.download_progress == PROJECT_DOWNLOAD_EOT) {
        pro_rw_log.append(TEXT_COLOR_GREEN("START EOT", TEXT_SIZE_MEDIUM));
        memset(( uint8_t* )project_transmit_info.ack, 0, sizeof(project_transmit_info.ack));
        memset(( uint8_t* )project_transmit_info.error_code, 0, sizeof(project_transmit_info.error_code));
        mainwindow->my_serial->port_cmd_sendframe(0, CMD_TYPE_PROJECT, CMD_PUBLIC_FILE_DOWNLOAD,
                                                  SUB_PUBLIC_FILE_DOWNLOAD_EOT, NULL, 0);
        wait_tick = QDateTime::currentMSecsSinceEpoch();
        if (LUA_SYNC_DOWNLOAD) {
            PT_WAIT_UNTIL(pt, (project_transmit_info.ack[SYNC_ID_A] == SUB_PUBLIC_FILE_DOWNLOAD_EOT_ACK
                               && project_transmit_info.ack[SYNC_ID_B] == SUB_PUBLIC_FILE_DOWNLOAD_EOT_ACK)
                                  || (QDateTime::currentMSecsSinceEpoch() - wait_tick > 3000));
        } else {
            PT_WAIT_UNTIL(pt, (project_transmit_info.ack[SYNC_ID_COMMON] == SUB_PUBLIC_FILE_DOWNLOAD_EOT_ACK)
                                  || (QDateTime::currentMSecsSinceEpoch() - wait_tick > 1000));
        }
        download_ack_eot_result_phase(starttime);
    }
    PT_END(pt);
}

/**
 * @brief 下载工程到设备
 * @param file
 * @param project_file
 */
bool project_transmit::transmit_from_project(QByteArray* file, project_info_t project_file)
{
    project_rw_dialog.setWindowTitle("传输到设备");
    connect(&project_rw_dialog, &my_dialog::dialog_start, [&]() {
        pro_rw_log.clear();
        project_transmit_info.file_size         = file->size();
        project_transmit_info.file_buf          = file->data();
        transmit_project_info                   = project_file;
        project_transmit_info.status            = PROJECT_DOWNLOAD_DOWNLOADING;
        project_transmit_info.download_progress = PROJECT_DOWNLOAD_SOH;
        PT_INIT(&pt_download);
        while (project_transmit_info.status == PROJECT_DOWNLOAD_DOWNLOADING) {
            project_transmit_thread();
            QApplication::processEvents();
        }
        // run lua
        pro_rw_log.append(TEXT_COLOR_BLUE("结束下载", TEXT_SIZE_MEDIUM));
        if (project_transmit_info.status == PROJECT_DOWNLOAD_SUCCESS) {
            run_usercode();
        } else {
            mainwindow->my_message_box("传输到设备失败", MESSAGE_TYPE_ERROR);
        }
        if (project_rw_dialog.isVisible()) {
            project_rw_dialog.close();
        }
    });
    project_rw_dialog.dialog_exec();
    disconnect(&project_rw_dialog, &my_dialog::dialog_start, nullptr, nullptr);
    if (project_transmit_info.status == PROJECT_DOWNLOAD_SUCCESS) {
        return true;
    }
    return false;
}

/************************* 文件回读代码区 *************************/
/**
 * @brief 工程回读起始包反馈信息处理函数
 * @param frame
 * @param length
 */
void project_transmit::readback_ack_soh_prase(uint8_t* frame, int32_t length)
{
    length                   = length;
    readback_info.ack        = SUB_PUBLIC_FILE_READBACK_SOH_ACK;
    readback_info.error_code = frame[10];
    if (readback_info.error_code == 0) {
        memcpy(( char* )&read_project_info, &frame[11], sizeof(read_project_info));
        readback_info.file_size =
            read_project_info.param_size + read_project_info.project_size + read_project_info.usercode_size;
        pro_progress.setValue(0);
        pro_progress.setMaximum(readback_info.file_size);
    } else {
        switch (readback_info.error_code) {
        case ( uint8_t )-1:
            pro_rw_log.append(TEXT_COLOR_RED("设备内无有效工程", TEXT_SIZE_MEDIUM));
            break;
        case ( uint8_t )-2:
            mainwindow->my_message_box("设备工程与当前工程相同，无需重复读取", MESSAGE_TYPE_WARNING);
            break;
        }
        readback_info.status = READBACK_STATUS_FAIL;
    }
}

/**
 * @brief 工程回读数据包反馈信息处理函数
 * @param frame
 * @param length
 */
void project_transmit::readback_ack_stx_prase(uint8_t* frame, int32_t length)
{

    readback_info.ack        = SUB_PUBLIC_FILE_READBACK_STX_ACK;
    readback_info.error_code = frame[10];
    if (readback_info.error_code == 0) {
        uint32_t recpackseq = frame[11] | frame[12] << 8 | frame[13] << 16 | frame[14] << 24;
        if (recpackseq == readback_info.packseq) {
            readback_info.project_file.append(( char* )(frame + 23), length - 23);
            readback_info.read_size = readback_info.project_file.size();
            readback_info.packseq++;
            pro_progress.setValue(readback_info.project_file.size());
        }
    } else {
        switch (readback_info.error_code) {
        case ( uint8_t )-1:
            pro_rw_log.append(TEXT_COLOR_RED("设备内无有效工程", TEXT_SIZE_MEDIUM));
            break;
        case ( uint8_t )-2:
            pro_rw_log.append(TEXT_COLOR_RED("读取超过文件范围", TEXT_SIZE_MEDIUM));
            break;
        case ( uint8_t )-3:
            pro_rw_log.append(TEXT_COLOR_RED("读取数据包过大", TEXT_SIZE_MEDIUM));
            break;
        }
        readback_info.status = READBACK_STATUS_FAIL;
    }
}

/**
 * @brief 工程回读协程
 * @return
 */
int project_transmit::readback_file_thread()
{
#define PACKLEN (128)
    static uint8_t frame[128];
    static uint8_t retry;
    static qint64  wait_tick;
    uint32_t       packlen = 0;
    PT_BEGIN(&pt_readback);
    PT_YIELD_FLAG = PT_YIELD_FLAG;
    if (readback_info.status == READBACK_STATUS_SOH) {
        pro_rw_log.append(TEXT_COLOR_GREEN("Connect device ...... ", TEXT_SIZE_MEDIUM));
        retry = 0;
        while (1) {
            readback_info.ack        = 0;
            readback_info.error_code = 0;
            mainwindow->my_serial->port_cmd_sendframe(0, CMD_TYPE_PROJECT, CMD_PUBLIC_FILE_READBACK,
                                                      SUB_PUBLIC_FILE_READBACK_SOH, NULL, 0);
            wait_tick = QDateTime::currentMSecsSinceEpoch();
            PT_WAIT_UNTIL(&pt_readback, (readback_info.ack == SUB_PUBLIC_FILE_READBACK_SOH_ACK)
                                            || (QDateTime::currentMSecsSinceEpoch() - wait_tick >= 1000));
            if (readback_info.ack == SUB_PUBLIC_FILE_READBACK_SOH_ACK) {
                if (readback_info.error_code != 0) {
                    readback_info.status = READBACK_STATUS_FAIL;
                } else {
                    readback_info.status = READBACK_STATUS_STX;
                }
                break;
            } else {
                if (++retry > 3) {
                    readback_info.status = READBACK_STATUS_FAIL;
                    pro_rw_log.append(TEXT_COLOR_RED("SOH no response ", TEXT_SIZE_MEDIUM));
                    break;
                }
            }
        }
    }
    if (readback_info.status == READBACK_STATUS_STX) {
        retry = 0;
        while (1) {
            pro_rw_log.append(
                TEXT_COLOR_GREEN("读取工程文件包:" + QString::number(readback_info.packseq), TEXT_SIZE_MEDIUM));
            packlen = readback_info.file_size - readback_info.read_size >= PACKLEN ?
                          PACKLEN :
                          readback_info.file_size - readback_info.read_size;
            frame[0]                 = readback_info.packseq;
            frame[1]                 = readback_info.packseq >> 8;
            frame[2]                 = readback_info.packseq >> 16;
            frame[3]                 = readback_info.packseq >> 24;
            frame[4]                 = readback_info.read_size;
            frame[5]                 = readback_info.read_size >> 8;
            frame[6]                 = readback_info.read_size >> 16;
            frame[7]                 = readback_info.read_size >> 24;
            frame[8]                 = packlen;
            frame[9]                 = packlen >> 8;
            frame[10]                = packlen >> 16;
            frame[11]                = packlen >> 24;
            readback_info.ack        = 0;
            readback_info.error_code = 0;
            mainwindow->my_serial->port_cmd_sendframe(0, CMD_TYPE_PROJECT, CMD_PUBLIC_FILE_READBACK,
                                                      SUB_PUBLIC_FILE_READBACK_STX, frame, 12);
            wait_tick = QDateTime::currentMSecsSinceEpoch();
            PT_WAIT_UNTIL(&pt_readback, (readback_info.ack == SUB_PUBLIC_FILE_READBACK_STX_ACK)
                                            || (QDateTime::currentMSecsSinceEpoch() - wait_tick >= 1000));
            if (readback_info.ack == SUB_PUBLIC_FILE_READBACK_STX_ACK) {
                if (readback_info.project_file.size() >= ( int )readback_info.file_size) {
                    readback_info.status = READBACK_STATUS_EOT;
                    break;
                }
            } else {
                if (++retry > 3) {
                    readback_info.status = READBACK_STATUS_FAIL;
                    pro_rw_log.append(TEXT_COLOR_RED("stx no response ", TEXT_SIZE_MEDIUM));
                    break;
                }
            }
        }
    }
    if (readback_info.status == READBACK_STATUS_EOT) {
        uint8_t md5[16];
        mbedtls_md5(( unsigned char* )readback_info.project_file.data(), readback_info.project_file.size(), md5);
        if (memcmp(md5, read_project_info.md5, 16) == 0) {
            readback_info.status = READBACK_STATUS_SUCCESS;
        } else {
            readback_info.status = READBACK_STATUS_FAIL;
        }
    }

    PT_END(&pt_readback);
}

/**
 * @brief 从设备读取工程
 * @param project_file
 * @return
 */
bool project_transmit::readback_project_file(project_info_t project_file)
{
    project_rw_dialog.setWindowTitle("从设备读取");
    connect(&project_rw_dialog, &my_dialog::dialog_start, [&]() {
        pro_rw_log.clear();
        readback_info.packseq   = 0;
        readback_info.read_size = 0;
        readback_info.file_size = 0;
        readback_info.project_file.clear();
        readback_info.status  = READBACK_STATUS_SOH;
        transmit_project_info = project_file;
        PT_INIT(&pt_readback);
        while (readback_info.status != READBACK_STATUS_SUCCESS && readback_info.status != READBACK_STATUS_FAIL) {
            readback_file_thread();
            QApplication::processEvents();
        }
        pro_rw_log.append(TEXT_COLOR_BLUE("结束读取", TEXT_SIZE_MEDIUM));

        if (readback_info.status == READBACK_STATUS_FAIL) {
            mainwindow->my_message_box("从设备读取失败", MESSAGE_TYPE_ERROR);
        }
        if (project_rw_dialog.isVisible()) {
            project_rw_dialog.close();
        }
    });
    project_rw_dialog.dialog_exec();
    disconnect(&project_rw_dialog, &my_dialog::dialog_start, nullptr, nullptr);
    if (readback_info.status == READBACK_STATUS_SUCCESS) {
        return true;
    }
    return false;
}

/**
 * @brief 向设备发送运行用户代码的指令
 */
void project_transmit::run_usercode()
{
    mainwindow->my_serial->port_cmd_sendframe(0, CMD_TYPE_PROJECT, CMD_PROJECT_USERCODE, SUB_PROJECT_USERCODE_RUN, NULL,
                                              0);
}

/**
 * @brief 相关指令数据包解析回调
 * @param frame
 * @param length
 */
void project_transmit::transmit_cmd_response(uint8_t* frame, int32_t length)
{
    uint8_t cmd    = frame[6];
    uint8_t bootid = frame[0];
    if (bootid >= SYNC_ID_MAX) {
        return;
    }
    switch (cmd) {
    case CMD_PUBLIC_FILE_DOWNLOAD:
        switch (frame[7]) {
        case SUB_PUBLIC_FILE_DOWNLOAD_EOT_ACK:
        case SUB_PUBLIC_FILE_DOWNLOAD_SOH_ACK:
            project_transmit_info.ack[bootid]        = frame[7];
            project_transmit_info.error_code[bootid] = ( int8_t )frame[10];
            break;

        case SUB_PUBLIC_FILE_DOWNLOAD_STX_ACK:
            project_transmit_info.ack[bootid]        = frame[7];
            project_transmit_info.error_code[bootid] = ( int8_t )frame[10];
            project_transmit_info.reqpackseq[bootid] =
                frame[11] | (frame[12] << 8) | (frame[13] << 16) | (frame[14] << 24);
            break;
        default:
            break;
        }
        break;
    case CMD_PUBLIC_FILE_READBACK:
        switch (frame[7]) {
        case SUB_PUBLIC_FILE_READBACK_SOH_ACK:
            readback_ack_soh_prase(frame, length);
            break;
        case SUB_PUBLIC_FILE_DOWNLOAD_STX_ACK:
            readback_ack_stx_prase(frame, length);
            break;
        default:
            break;
        }
        break;
    }
}
