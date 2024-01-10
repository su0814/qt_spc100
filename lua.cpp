#include "lua.h"
#include "QDateTime"
#include "mainwindow.h"
#include "md5.h"
#include "qdebug.h"
#include "qfiledialog.h"
#include <QElapsedTimer>
#include <QFont>
#include <QFontDialog>
#include <QInputDialog>
lua::lua(QWidget* parent)
    : QWidget(parent)
{
    ui                       = MainWindow::my_ui->ui;
    mainwindow               = ( MainWindow* )parent;
    lua_download_info.status = LUA_DOWNLOAD_IDLE;
    PT_INIT(&pt_download);
    sync_id_list << "COMMON"
                 << "A"
                 << "B";
}

/************************* 文件下载代码区 *************************/
/**
 * @brief lua::download_ack_soh_result_phase 下载起始包反馈结果解析
 * @param retry_cnt 重发次数
 * @return 解析结果 0-正常 <0-有错误
 */
int lua::download_ack_soh_result_phase(uint8_t* retry_cnt)
{

    uint8_t retry = *retry_cnt;
    uint8_t ret   = 0;

    if (LUA_SYNC_DOWNLOAD) {  //双MCU同步升级
        if (lua_download_info.ack[SYNC_ID_A] != SUB_PUBLIC_FILE_DOWNLOAD_SOH_ACK
            || lua_download_info.ack[SYNC_ID_B] != SUB_PUBLIC_FILE_DOWNLOAD_SOH_ACK) {  //有节点没反馈
            if (lua_download_info.ack[SYNC_ID_A] != SUB_PUBLIC_FILE_DOWNLOAD_SOH_ACK
                && lua_download_info.ack[SYNC_ID_B] != SUB_PUBLIC_FILE_DOWNLOAD_SOH_ACK) {  //所有节点都没反馈
                ui->lua_downloadlog_textBrowser->append(TEXT_COLOR_RED(
                    "Warning: Wait all  SOH ACK timeout!, retry: " + QString::number(retry), TEXT_SIZE_MEDIUM));
            } else if (lua_download_info.ack[SYNC_ID_B] != SUB_PUBLIC_FILE_DOWNLOAD_SOH_ACK) {  // B节点未反馈
                ui->lua_downloadlog_textBrowser->append(TEXT_COLOR_RED(
                    "Warning: Wait B  SOH ACK timeout!, retry: " + QString::number(retry), TEXT_SIZE_MEDIUM));
            } else if (lua_download_info.ack[SYNC_ID_A] != SUB_PUBLIC_FILE_DOWNLOAD_SOH_ACK) {  // A节点未反馈
                ui->lua_downloadlog_textBrowser->append(TEXT_COLOR_RED(
                    QString("Warning: Wait A  SOH ACK timeout!, retry: " + QString::number(retry)), TEXT_SIZE_MEDIUM));
            }
            if (++retry >= 5) {  //重发次数达到阈值
                lua_download_info.status = LUA_DOWNLOAD_END;
                ui->lua_downloadlog_textBrowser->append(TEXT_COLOR_RED(QString("retry SOH fail"), TEXT_SIZE_MEDIUM));
                ret = -1;
            }

        } else {
            for (uint8_t i = SYNC_ID_A; i <= SYNC_ID_B; i++) {  //解析节点反馈结果是异常/正常
                switch (lua_download_info.error_code[i]) {
                case 0:
                    if (lua_download_info.status != LUA_DOWNLOAD_END) {
                        lua_download_info.download_progress = LUA_DOWNLOAD_STX;
                    }
                    break;
                case -1:
                    lua_download_info.status = LUA_DOWNLOAD_END;
                    ui->lua_downloadlog_textBrowser->append(
                        sync_id_list[i] + TEXT_COLOR_RED("-Error: 文件名过长，请检查！", TEXT_SIZE_LARGE));
                    break;
                case -2:
                    lua_download_info.status = LUA_DOWNLOAD_END;
                    ui->lua_downloadlog_textBrowser->append(
                        TEXT_COLOR_RED(sync_id_list[i] + "-Error: 文件大小不符合！", TEXT_SIZE_LARGE));
                    break;
                default:
                    lua_download_info.status = LUA_DOWNLOAD_END;
                    ui->lua_downloadlog_textBrowser->append(
                        TEXT_COLOR_RED(sync_id_list[i] + "Error: 未知错误！", TEXT_SIZE_LARGE));
                    break;
                }
            }
            ret = -1;
        }
    } else {  //单MCU升级 相同检测逻辑
        if (lua_download_info.ack[SYNC_ID_COMMON] != SUB_PUBLIC_FILE_DOWNLOAD_SOH_ACK) {
            if (++retry >= 5) {
                lua_download_info.status = LUA_DOWNLOAD_END;
                ui->lua_downloadlog_textBrowser->append(
                    TEXT_COLOR_RED(sync_id_list[SYNC_ID_COMMON] + QString(" retry SOH fail"), TEXT_SIZE_MEDIUM));
                ret = -1;
            }
            ui->lua_downloadlog_textBrowser->append(TEXT_COLOR_RED(
                sync_id_list[SYNC_ID_COMMON] + " Warning: Wait SOH ACK timeout!, retry: " + QString::number(retry),
                TEXT_SIZE_MEDIUM));
        } else {
            switch (lua_download_info.error_code[SYNC_ID_COMMON]) {
            case 0:
                lua_download_info.download_progress = LUA_DOWNLOAD_STX;
                break;
            case -1:
                lua_download_info.status = LUA_DOWNLOAD_END;
                ui->lua_downloadlog_textBrowser->append(
                    TEXT_COLOR_RED(sync_id_list[SYNC_ID_COMMON] + "-Error: 文件名过长，请检查！", TEXT_SIZE_LARGE));
                break;
            case -2:
                lua_download_info.status = LUA_DOWNLOAD_END;
                ui->lua_downloadlog_textBrowser->append(
                    TEXT_COLOR_RED(sync_id_list[SYNC_ID_COMMON] + "-Error: 文件大小不符合！", TEXT_SIZE_LARGE));
                break;
            default:
                lua_download_info.status = LUA_DOWNLOAD_END;
                ui->lua_downloadlog_textBrowser->append(
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
int lua::download_ack_stx_result_phase(uint8_t* retry_cnt)
{
    uint8_t retry               = *retry_cnt;
    uint8_t ret                 = 0;
    bool    repeat[SYNC_ID_MAX] = { false, false, false };
    if (LUA_SYNC_DOWNLOAD) {
        /* 目标未反馈，重试 */
        if (lua_download_info.ack[SYNC_ID_A] != SUB_PUBLIC_FILE_DOWNLOAD_STX_ACK
            || lua_download_info.ack[SYNC_ID_B] != SUB_PUBLIC_FILE_DOWNLOAD_STX_ACK) {
            if (++retry < 10) {
                if (lua_download_info.ack[SYNC_ID_B] != SUB_PUBLIC_FILE_DOWNLOAD_STX_ACK) {
                    ui->lua_downloadlog_textBrowser->append(TEXT_COLOR_RED(
                        "B-Warning: 等待目标反馈超时！重新发送..." + QString::number(retry), TEXT_SIZE_MEDIUM));
                } else if (lua_download_info.ack[SYNC_ID_A] != SUB_PUBLIC_FILE_DOWNLOAD_STX_ACK) {
                    ui->lua_downloadlog_textBrowser->append(TEXT_COLOR_RED(
                        "A-Warning: 等待目标反馈超时！重新发送..." + QString::number(retry), TEXT_SIZE_MEDIUM));
                }
            } else {
                lua_download_info.status = LUA_DOWNLOAD_END;
                ui->lua_downloadlog_textBrowser->append(TEXT_COLOR_RED("SYNC-Error: 重发失败！", TEXT_SIZE_MEDIUM));
                ret = -1;
            }
        } else {
            retry = 0;
            for (uint8_t i = SYNC_ID_A; i <= SYNC_ID_B; i++) {
                if (lua_download_info.error_code[i] == -1) {
                    ui->lua_downloadlog_textBrowser->append(
                        TEXT_COLOR_BLUE(sync_id_list[i] + "-Warning: 目标未收到正确的包!seq = "
                                            + QString::number(lua_download_info.packseq)
                                            + ", reqseq = " + QString::number(lua_download_info.reqpackseq[i]),
                                        TEXT_SIZE_MEDIUM));
                    /* 目标请求的包不正确，且误差很严重 */
                    if (lua_download_info.reqpackseq[i] < lua_download_info.packseq
                        || lua_download_info.reqpackseq[i] > lua_download_info.packseq + 1) {
                        ui->lua_downloadlog_textBrowser->append(
                            TEXT_COLOR_RED(sync_id_list[i] + "Error: 目标在请求错误的包：reqseq = "
                                               + QString::number(lua_download_info.reqpackseq[i]),
                                           TEXT_SIZE_MEDIUM));
                        lua_download_info.status = LUA_DOWNLOAD_END;
                        ret                      = -1;
                    }
                    /* 本次发送的包没有被收到，重新发送本次数据包 */
                    /* 其实这个条件并不会被触发，因为目标如果没收到，就不会有这次NAK回复了 */
                    if (lua_download_info.reqpackseq[i] == lua_download_info.packseq) {
                        ui->lua_downloadlog_textBrowser->append(TEXT_COLOR_BLUE(
                            sync_id_list[i] + "重新发送数据包... seq = " + QString::number(lua_download_info.packseq)
                                + ", reqseq = " + QString::number(lua_download_info.reqpackseq[i]),
                            TEXT_SIZE_MEDIUM));
                    }
                    /* 目标收到了重复的包 */
                    /* 这可能是由于目标的ACK包没被PC收到，于是PC重新发包导致的 */
                    if (lua_download_info.reqpackseq[i] == (lua_download_info.packseq + 1)) {
                        ui->lua_downloadlog_textBrowser->append(
                            TEXT_COLOR_BLUE(sync_id_list[i] + "目标收到了重复的包!", TEXT_SIZE_MEDIUM));
                        repeat[i] = true;
                    } else {
                        lua_download_info.download_progress = LUA_DOWNLOAD_SOH;
                    }
                } else if (lua_download_info.error_code[i] == -2) {
                    ui->lua_downloadlog_textBrowser->append(
                        TEXT_COLOR_RED(sync_id_list[i] + "-ERROR: 文件续传出错", TEXT_SIZE_MEDIUM));
                } else if (lua_download_info.error_code[i] == 0) {
                    ui->lua_downloadlog_textBrowser->append(
                        TEXT_COLOR_GREEN(sync_id_list[i] + " receive success", TEXT_SIZE_MEDIUM));
                } else {
                    ui->lua_downloadlog_textBrowser->append(
                        TEXT_COLOR_RED(sync_id_list[i] + "-ERROR: 未知错误", TEXT_SIZE_MEDIUM));
                }
            }
            if ((lua_download_info.error_code[SYNC_ID_A] == 0 && lua_download_info.error_code[SYNC_ID_B] == 0)
                || (repeat[SYNC_ID_A] && repeat[SYNC_ID_B])) {
                ++lua_download_info.packseq;
                lua_download_info.write_size += lua_download_info.packlen;
                ui->lua_download_progressBar->setValue(lua_download_info.write_size);
                ui->upgrade_progressBar->setValue(lua_download_info.write_size);
                if (lua_download_info.write_size >= lua_download_info.file_size) {
                    lua_download_info.download_progress = LUA_DOWNLOAD_EOT;
                    ui->lua_downloadlog_textBrowser->append(
                        TEXT_COLOR_GREEN(QString("Download finish!"), TEXT_SIZE_MEDIUM));
                    ret = -1;
                }
            }
        }
    } else {
        /* 目标未反馈，重试 */
        if (lua_download_info.ack[SYNC_ID_COMMON] != SUB_PUBLIC_FILE_DOWNLOAD_STX_ACK) {
            if (++retry < 10) {
                ui->lua_downloadlog_textBrowser->append(
                    TEXT_COLOR_BLUE("C-Warning: 等待目标反馈超时！重新发送...", TEXT_SIZE_MEDIUM));
            } else {
                lua_download_info.status = LUA_DOWNLOAD_END;
                ui->lua_downloadlog_textBrowser->append(
                    TEXT_COLOR_RED(QString("C-Error: 重发失败！"), TEXT_SIZE_MEDIUM));
                ret = -1;
            }
        } else {
            retry = 0;
            /* 目标未收到正确的包 */
            if (lua_download_info.error_code[SYNC_ID_COMMON] == -1) {
                ui->lua_downloadlog_textBrowser->append(TEXT_COLOR_BLUE(
                    sync_id_list[SYNC_ID_COMMON]
                        + QString("Warning: 目标未收到正确的包!seq = " + QString::number(lua_download_info.packseq)
                                  + ", reqseq = " + QString::number(lua_download_info.reqpackseq[SYNC_ID_COMMON])),
                    TEXT_SIZE_MEDIUM));
                /* 目标请求的包不正确，且误差很严重 */
                if (lua_download_info.reqpackseq[SYNC_ID_COMMON] < lua_download_info.packseq
                    || lua_download_info.reqpackseq[SYNC_ID_COMMON] > lua_download_info.packseq + 1) {
                    ui->lua_downloadlog_textBrowser->append(
                        TEXT_COLOR_RED(sync_id_list[SYNC_ID_COMMON] + QString("-Error: 目标在请求错误的包：reqseq = ")
                                           + QString::number(lua_download_info.reqpackseq[SYNC_ID_COMMON]),
                                       TEXT_SIZE_MEDIUM));
                    lua_download_info.status = LUA_DOWNLOAD_END;
                    ret                      = -1;
                }
                /* 本次发送的包没有被收到，重新发送本次数据包 */
                /* 其实这个条件并不会被触发，因为目标如果没收到，就不会有这次NAK回复了 */
                if (lua_download_info.reqpackseq[SYNC_ID_COMMON] == lua_download_info.packseq) {
                    ui->lua_downloadlog_textBrowser->append(TEXT_COLOR_BLUE(
                        sync_id_list[SYNC_ID_COMMON]
                            + "重新发送数据包... seq = " + QString::number(lua_download_info.packseq)
                            + ", reqseq = " + QString::number(lua_download_info.reqpackseq[SYNC_ID_COMMON]),
                        TEXT_SIZE_MEDIUM));
                }
                /* 目标收到了重复的包 */
                /* 这可能是由于目标的ACK包没被PC收到，于是PC重新发包导致的 */
                if (lua_download_info.reqpackseq[SYNC_ID_COMMON] == (lua_download_info.packseq + 1)) {
                    ui->lua_downloadlog_textBrowser->append(TEXT_COLOR_BLUE(
                        sync_id_list[SYNC_ID_COMMON] + QString("目标收到了重复的包!"), TEXT_SIZE_MEDIUM));
                }
            } else if (lua_download_info.error_code[SYNC_ID_COMMON] == -2) {
                ui->lua_downloadlog_textBrowser->append(TEXT_COLOR_RED("COMMON-ERROR: 文件续传出错", TEXT_SIZE_MEDIUM));
            } else if (lua_download_info.error_code[SYNC_ID_COMMON] == 0) {
                ui->lua_downloadlog_textBrowser->append(TEXT_COLOR_GREEN("receive success", TEXT_SIZE_MEDIUM));
            } else {
                ui->lua_downloadlog_textBrowser->append(TEXT_COLOR_RED("COMMON-ERROR: 未知错误", TEXT_SIZE_MEDIUM));
            }
            ++lua_download_info.packseq;
            lua_download_info.write_size += lua_download_info.packlen;
            ui->upgrade_progressBar->setValue(lua_download_info.write_size);
            if (lua_download_info.write_size >= lua_download_info.file_size) {
                lua_download_info.download_progress = LUA_DOWNLOAD_EOT;
                ui->lua_downloadlog_textBrowser->append(
                    TEXT_COLOR_GREEN(QString("Download finish!"), TEXT_SIZE_MEDIUM));
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
int lua::download_ack_eot_result_phase(qint64 starttime)
{
    bool   result  = false;
    qint64 endtime = 0;
    if (LUA_SYNC_DOWNLOAD) {
        result = (lua_download_info.ack[SYNC_ID_A] == SUB_PUBLIC_FILE_DOWNLOAD_EOT_ACK
                  && lua_download_info.ack[SYNC_ID_B] == SUB_PUBLIC_FILE_DOWNLOAD_EOT_ACK);
    } else {
        result = (lua_download_info.ack[SYNC_ID_COMMON] == SUB_PUBLIC_FILE_DOWNLOAD_EOT_ACK);
    }
    if (result) {
        for (uint8_t i = SYNC_ID_COMMON; i < SYNC_ID_MAX; i++) {
            if (LUA_SYNC_DOWNLOAD && (i == SYNC_ID_COMMON)) {
                continue;
            } else if (!LUA_SYNC_DOWNLOAD && (i != SYNC_ID_COMMON)) {
                continue;
            }
            switch (lua_download_info.error_code[i]) {
            case 0:

                if (lua_download_info.status != LUA_DOWNLOAD_END) {
                    lua_download_info.status = LUA_DOWNLOAD_FINISH;
                }
                if ((i == SYNC_ID_B || i == SYNC_ID_COMMON) && lua_download_info.status == LUA_DOWNLOAD_FINISH) {
                    ui->lua_downloadlog_textBrowser->append(
                        TEXT_COLOR_GREEN(QString("Download success!"), TEXT_SIZE_OVER));
                    endtime = QDateTime::currentMSecsSinceEpoch();
                    ui->lua_downloadlog_textBrowser->append(TEXT_COLOR_GREEN(
                        QString("Total tim: " + QString::number(endtime - starttime) + "ms " + ", Average speed:"
                                + QString::number(((lua_download_info.file_size) / (endtime - starttime))) + "KB/s"),
                        TEXT_SIZE_MEDIUM));
                }
                break;
            case -1:
                lua_download_info.status = LUA_DOWNLOAD_END;
                ui->lua_downloadlog_textBrowser->append(
                    TEXT_COLOR_RED(sync_id_list[i] + QString(" 下载丢包，接收文件不完整"), TEXT_SIZE_MEDIUM));
                break;
            case -2:
                lua_download_info.status = LUA_DOWNLOAD_END;
                ui->lua_downloadlog_textBrowser->append(
                    TEXT_COLOR_RED(sync_id_list[i] + QString(" MD5 verify fail!"), TEXT_SIZE_MEDIUM));
                break;
            default:
                lua_download_info.status = LUA_DOWNLOAD_END;
                ui->lua_downloadlog_textBrowser->append(
                    TEXT_COLOR_RED(sync_id_list[i] + QString(" 未知错误"), TEXT_SIZE_MEDIUM));
                break;
            }
        }
    } else {
        lua_download_info.status = LUA_DOWNLOAD_END;
        ui->lua_downloadlog_textBrowser->append(TEXT_COLOR_RED("错误结束", TEXT_SIZE_MEDIUM));
    }
    return 0;
}

/**
 * @brief lua::lua_download_file_thread 文件下载线程，使用PT，在while1内运行
 * @return
 */
int lua::lua_download_file_thread()
{
#define PACK_LEN 480
    static struct pt* pt = &pt_download;
    static uint16_t   payloadlen;
    static uint8_t    frame[TRANS_BUFF_LEN];
    static uint8_t    retry;
    static qint64     wait_tick;
    static qint64     starttime = QDateTime::currentMSecsSinceEpoch();  //用于计算升级用时
    QByteArray        tmp_arr;
    char*             tmp_str;
    PT_BEGIN(pt);
    PT_YIELD_FLAG = PT_YIELD_FLAG;
    starttime     = QDateTime::currentMSecsSinceEpoch();
    ui->lua_download_progressBar->setValue(0);
    ui->lua_download_progressBar->setMaximum(lua_download_info.file_size);
    if (lua_download_info.download_progress == LUA_DOWNLOAD_SOH) {
        retry = 0;
        ui->lua_downloadlog_textBrowser->append(TEXT_COLOR_GREEN(" START SOH", TEXT_SIZE_MEDIUM));
        while (1) {
            payloadlen = sizeof(project_info);
            frame[0]   = 0;
            frame[1]   = CMD_TYPE_PROJECT;
            frame[2]   = CMD_PUBLIC_FILE_DOWNLOAD;
            frame[3]   = SUB_PUBLIC_FILE_DOWNLOAD_SOH;
            frame[4]   = payloadlen & 0x00ff;
            frame[5]   = (payloadlen >> 8) & 0x00ff;
            memcpy(&frame[6], ( char* )&project_info, sizeof(project_info));
            lua_download_info.packseq    = 0;
            lua_download_info.write_size = 0;
            memset(( uint8_t* )lua_download_info.ack, 0, sizeof(lua_download_info.ack));
            memset(( uint8_t* )lua_download_info.error_code, 0, sizeof(lua_download_info.error_code));
            mainwindow->my_serial->port_sendframe(frame, payloadlen + 6);
            wait_tick = QDateTime::currentMSecsSinceEpoch();
            if (LUA_SYNC_DOWNLOAD) {
                PT_WAIT_UNTIL(pt, (lua_download_info.ack[SYNC_ID_A] == SUB_PUBLIC_FILE_DOWNLOAD_SOH_ACK
                                   && lua_download_info.ack[SYNC_ID_B] == SUB_PUBLIC_FILE_DOWNLOAD_SOH_ACK)
                                      || (QDateTime::currentMSecsSinceEpoch() - wait_tick > 2000));
            } else {
                PT_WAIT_UNTIL(pt, (lua_download_info.ack[SYNC_ID_COMMON] == SUB_PUBLIC_FILE_DOWNLOAD_SOH_ACK)
                                      || (QDateTime::currentMSecsSinceEpoch() - wait_tick > 1500));
            }
            if (download_ack_soh_result_phase(&retry) != 0) {
                break;
            }
        }
    }

    if (lua_download_info.download_progress == LUA_DOWNLOAD_STX) {
        retry = 0;
        do {
            ui->lua_downloadlog_textBrowser->append(TEXT_COLOR_GREEN(
                QString("发送文件... seq:" + QString::number(lua_download_info.packseq)), TEXT_SIZE_MEDIUM));
            frame[0] = 0;
            frame[1] = CMD_TYPE_PROJECT;
            frame[2] = CMD_PUBLIC_FILE_DOWNLOAD;
            frame[3] = SUB_PUBLIC_FILE_DOWNLOAD_STX;
            frame[6] = lua_download_info.packseq & 0x000000ff;
            frame[7] = (lua_download_info.packseq >> 8) & 0x000000ff;
            frame[8] = (lua_download_info.packseq >> 16) & 0x000000ff;
            frame[9] = (lua_download_info.packseq >> 24) & 0x000000ff;
            if (lua_download_info.write_size + PACK_LEN <= lua_download_info.file_size) {
                lua_download_info.packlen = PACK_LEN;
            } else {
                lua_download_info.packlen = lua_download_info.file_size - lua_download_info.write_size;
            }
            memcpy(&frame[10], lua_download_info.file_buf + lua_download_info.write_size, lua_download_info.packlen);
            payloadlen = 4 + lua_download_info.packlen; /* 固件 + 4字节的seq */
            frame[4]   = payloadlen & 0x00ff;
            frame[5]   = (payloadlen >> 8) & 0x00ff;
            memset(( uint8_t* )lua_download_info.ack, 0, sizeof(lua_download_info.ack));
            memset(( uint8_t* )lua_download_info.error_code, 0, sizeof(lua_download_info.error_code));
            /* 发送 frame = payload + 6字节的控制串 */
            mainwindow->my_serial->port_sendframe(frame, payloadlen + 6);
            /* 等待目标返回ACK */
            wait_tick = QDateTime::currentMSecsSinceEpoch();
            if (LUA_SYNC_DOWNLOAD) {
                PT_WAIT_UNTIL(pt, (lua_download_info.ack[SYNC_ID_A] == SUB_PUBLIC_FILE_DOWNLOAD_STX_ACK
                                   && lua_download_info.ack[SYNC_ID_B] == SUB_PUBLIC_FILE_DOWNLOAD_STX_ACK)
                                      || (QDateTime::currentMSecsSinceEpoch() - wait_tick > 3000));
            } else {
                PT_WAIT_UNTIL(pt, (lua_download_info.ack[SYNC_ID_COMMON] == SUB_PUBLIC_FILE_DOWNLOAD_STX_ACK)
                                      || (QDateTime::currentMSecsSinceEpoch() - wait_tick > 1000));
            }
            if (download_ack_stx_result_phase(&retry) != 0) {
                break;
            }
        } while (1);
    }
    if (lua_download_info.download_progress == LUA_DOWNLOAD_EOT) {
        ui->lua_downloadlog_textBrowser->append(TEXT_COLOR_GREEN("START EOT", TEXT_SIZE_MEDIUM));
        payloadlen = 16;
        frame[0]   = 0;
        frame[1]   = CMD_TYPE_PROJECT;
        frame[2]   = CMD_PUBLIC_FILE_DOWNLOAD;
        frame[3]   = SUB_PUBLIC_FILE_DOWNLOAD_EOT;
        frame[4]   = payloadlen;
        frame[5]   = 0;
        memcpy(&frame[6], lua_download_info.md5, 16);
        memset(( uint8_t* )lua_download_info.ack, 0, sizeof(lua_download_info.ack));
        memset(( uint8_t* )lua_download_info.error_code, 0, sizeof(lua_download_info.error_code));
        mainwindow->my_serial->port_sendframe(frame, payloadlen + 6);
        wait_tick = QDateTime::currentMSecsSinceEpoch();
        if (LUA_SYNC_DOWNLOAD) {
            PT_WAIT_UNTIL(pt, (lua_download_info.ack[SYNC_ID_A] == SUB_PUBLIC_FILE_DOWNLOAD_EOT_ACK
                               && lua_download_info.ack[SYNC_ID_B] == SUB_PUBLIC_FILE_DOWNLOAD_EOT_ACK)
                                  || (QDateTime::currentMSecsSinceEpoch() - wait_tick > 3000));
        } else {
            PT_WAIT_UNTIL(pt, (lua_download_info.ack[SYNC_ID_COMMON] == SUB_PUBLIC_FILE_DOWNLOAD_EOT_ACK)
                                  || (QDateTime::currentMSecsSinceEpoch() - wait_tick > 1000));
        }
        download_ack_eot_result_phase(starttime);
    }
    PT_END(pt);
}

void lua::lua_download_from_project(QByteArray* file, project_info_t project_file)
{
    ui->lua_downloadlog_textBrowser->clear();
    lua_download_info.file_size         = file->size();
    lua_download_info.file_buf          = file->data();
    project_info                        = project_file;
    lua_download_info.status            = LUA_DOWNLOAD_DOWNLOADING;
    lua_download_info.download_progress = LUA_DOWNLOAD_SOH;
    PT_INIT(&pt_download);
    while (lua_download_info.status == LUA_DOWNLOAD_DOWNLOADING) {
        lua_download_file_thread();
        QApplication::processEvents();
    }
    // run lua
    lua_cmd_run();
    ui->lua_downloadlog_textBrowser->append(TEXT_COLOR_BLUE("结束下载", TEXT_SIZE_MEDIUM));
}

/************************* 文件回读代码区 *************************/
void lua::readback_ack_soh_prase(uint8_t* frame, int32_t length)
{
    readback_info.ack        = SUB_PUBLIC_FILE_READBACK_SOH_ACK;
    readback_info.error_code = frame[6];
    if (readback_info.error_code == 0) {
        memcpy(( char* )&project_info, &frame[7], sizeof(project_info));
        readback_info.file_size = project_info.param_size + project_info.project_size + project_info.usercode_size;
        ui->lua_download_progressBar->setValue(0);
        ui->lua_download_progressBar->setMaximum(readback_info.file_size);
    } else {
        ui->lua_downloadlog_textBrowser->append(TEXT_COLOR_RED("设备内无有效工程", TEXT_SIZE_MEDIUM));
    }
}

void lua::readback_ack_stx_prase(uint8_t* frame, int32_t length)
{

    readback_info.ack        = SUB_PUBLIC_FILE_READBACK_STX_ACK;
    readback_info.error_code = frame[6];
    if (readback_info.error_code == 0) {
        uint32_t recpackseq = frame[7] | frame[8] << 8 | frame[9] << 16 | frame[10] << 24;
        if (recpackseq == readback_info.packseq) {
            readback_info.project_file.append(( char* )(frame + 19), length - 19);
            readback_info.read_size = readback_info.project_file.size();
            readback_info.packseq++;
            ui->lua_download_progressBar->setValue(readback_info.project_file.size());
        }
    } else {
        switch (readback_info.error_code) {
        case ( uint8_t )-1:
            ui->lua_downloadlog_textBrowser->append(TEXT_COLOR_RED("设备内无有效工程", TEXT_SIZE_MEDIUM));
            break;
        case ( uint8_t )-2:
            ui->lua_downloadlog_textBrowser->append(TEXT_COLOR_RED("读取超过文件范围", TEXT_SIZE_MEDIUM));
            break;
        case ( uint8_t )-3:
            ui->lua_downloadlog_textBrowser->append(TEXT_COLOR_RED("读取数据包过大", TEXT_SIZE_MEDIUM));
            break;
        }
        readback_info.status = READBACK_STATUS_FAIL;
    }
}

int lua::readback_file_thread()
{
#define PACKLEN (128)
    static uint8_t frame[128];
    static uint8_t retry;
    static qint64  wait_tick;
    uint32_t       packlen = 0;
    PT_BEGIN(&pt_readback);
    PT_YIELD_FLAG = PT_YIELD_FLAG;
    if (readback_info.status == READBACK_STATUS_SOH) {
        ui->lua_downloadlog_textBrowser->append(TEXT_COLOR_GREEN("Connect device ...... ", TEXT_SIZE_MEDIUM));
        retry = 0;
        while (1) {
            frame[0]                 = 0;
            frame[1]                 = CMD_TYPE_PROJECT;
            frame[2]                 = CMD_PUBLIC_FILE_READBACK;
            frame[3]                 = SUB_PUBLIC_FILE_READBACK_SOH;
            frame[4]                 = 0;
            frame[5]                 = 0;
            readback_info.ack        = 0;
            readback_info.error_code = 0;
            mainwindow->my_serial->port_sendframe(frame, 6);
            wait_tick = QDateTime::currentMSecsSinceEpoch();
            PT_WAIT_UNTIL(&pt_readback, (readback_info.ack == SUB_PUBLIC_FILE_READBACK_SOH_ACK)
                                            || (QDateTime::currentMSecsSinceEpoch() - wait_tick >= 1000));
            if (readback_info.ack == SUB_PUBLIC_FILE_READBACK_SOH_ACK) {
                if (readback_info.error_code != 0) {
                    ui->lua_downloadlog_textBrowser->append(
                        TEXT_COLOR_GREEN("Device project invalid", TEXT_SIZE_MEDIUM));
                    readback_info.status = READBACK_STATUS_FAIL;
                } else {
                    readback_info.status = READBACK_STATUS_STX;
                }
                break;
            } else {
                if (++retry > 3) {
                    readback_info.status = READBACK_STATUS_FAIL;
                    ui->lua_downloadlog_textBrowser->append(TEXT_COLOR_RED("SOH no response ", TEXT_SIZE_MEDIUM));
                    break;
                }
            }
        }
    }
    if (readback_info.status == READBACK_STATUS_STX) {
        retry = 0;
        while (1) {
            ui->lua_downloadlog_textBrowser->append(
                TEXT_COLOR_GREEN("读取工程文件包:" + QString::number(readback_info.packseq), TEXT_SIZE_MEDIUM));
            packlen = readback_info.file_size - readback_info.read_size >= PACKLEN ?
                          PACKLEN :
                          readback_info.file_size - readback_info.read_size;
            frame[0]                 = 0;
            frame[1]                 = CMD_TYPE_PROJECT;
            frame[2]                 = CMD_PUBLIC_FILE_READBACK;
            frame[3]                 = SUB_PUBLIC_FILE_READBACK_STX;
            frame[4]                 = 12;
            frame[5]                 = 0;
            frame[6]                 = readback_info.packseq;
            frame[7]                 = readback_info.packseq >> 8;
            frame[8]                 = readback_info.packseq >> 16;
            frame[9]                 = readback_info.packseq >> 24;
            frame[10]                = readback_info.read_size;
            frame[11]                = readback_info.read_size >> 8;
            frame[12]                = readback_info.read_size >> 16;
            frame[13]                = readback_info.read_size >> 24;
            frame[14]                = packlen;
            frame[15]                = packlen >> 8;
            frame[16]                = packlen >> 16;
            frame[17]                = packlen >> 24;
            readback_info.ack        = 0;
            readback_info.error_code = 0;
            mainwindow->my_serial->port_sendframe(frame, 18);
            wait_tick = QDateTime::currentMSecsSinceEpoch();
            PT_WAIT_UNTIL(&pt_readback, (readback_info.ack == SUB_PUBLIC_FILE_READBACK_STX_ACK)
                                            || (QDateTime::currentMSecsSinceEpoch() - wait_tick >= 1000));
            if (readback_info.ack == SUB_PUBLIC_FILE_READBACK_STX_ACK) {
                if (readback_info.project_file.size() >= readback_info.file_size) {
                    readback_info.status = READBACK_STATUS_EOT;
                    break;
                }
            } else {
                if (++retry > 3) {
                    readback_info.status = READBACK_STATUS_FAIL;
                    ui->lua_downloadlog_textBrowser->append(TEXT_COLOR_RED("stx no response ", TEXT_SIZE_MEDIUM));
                    break;
                }
            }
        }
    }
    if (readback_info.status == READBACK_STATUS_EOT) {
        uint8_t md5[16];
        mbedtls_md5(( unsigned char* )readback_info.project_file.data(), readback_info.project_file.size(), md5);
        if (memcmp(md5, project_info.md5, 16) == 0) {
            readback_info.status = READBACK_STATUS_SUCCESS;
        } else {
            readback_info.status = READBACK_STATUS_FAIL;
        }
    }

    PT_END(&pt_readback);
}

bool lua::readback_project_file()
{
    ui->lua_downloadlog_textBrowser->clear();
    readback_info.packseq   = 0;
    readback_info.read_size = 0;
    readback_info.file_size = 0;
    readback_info.project_file.clear();
    readback_info.status = READBACK_STATUS_SOH;
    PT_INIT(&pt_readback);
    while (readback_info.status != READBACK_STATUS_SUCCESS && readback_info.status != READBACK_STATUS_FAIL) {
        readback_file_thread();
        QApplication::processEvents();
    }
    ui->lua_downloadlog_textBrowser->append(TEXT_COLOR_BLUE("结束读取", TEXT_SIZE_MEDIUM));
    if (readback_info.status == READBACK_STATUS_SUCCESS) {
        return true;
    }
    return false;
}

void lua::lua_log_display(uint8_t sub, uint8_t* frame, int32_t length)
{

    QDateTime current_date_time = QDateTime::currentDateTime();
    QString   current_tim       = current_date_time.toString("hh:mm:ss.zzz");
    QString   current_date      = current_date_time.toString("yyyy-MM-dd");

    char str[512] = { 0 };
    memcpy(str, &frame[6], length - 6);
    QString play_str = str;

    QStringList play_list = play_str.split(QRegExp("[\n]"), QString::SkipEmptyParts);
    for (uint16_t i = 0; i < play_list.length(); i++) {
        if (i == 0) {
            if (frame[6] != '[' && sub == CMD_REPORT_LOG) {
                ui->lua_log_textBrowser->moveCursor(QTextCursor::End);
                ui->lua_log_textBrowser->moveCursor(QTextCursor::PreviousCharacter);
                ui->lua_log_textBrowser->insertHtml(TEXT_COLOR_WHILE(play_list[i], TEXT_SIZE_MEDIUM));
                ui->lua_log_textBrowser->append(TEXT_COLOR_BLUE(
                    "[" + current_date + "  " + current_tim + "]" + "# MCU" + sync_id_list[frame[0]], TEXT_SIZE_LARGE));
                continue;
            } else {
                ui->lua_log_textBrowser->append(TEXT_COLOR_BLUE(
                    "[" + current_date + "  " + current_tim + "]" + "# MCU" + sync_id_list[frame[0]], TEXT_SIZE_LARGE));
            }
        }
        ui->lua_log_textBrowser->append(TEXT_COLOR_WHILE(play_list[i], TEXT_SIZE_MEDIUM));
    }
}

void lua::lua_log_display(char* str)
{
    QDateTime current_date_time = QDateTime::currentDateTime();
    QString   current_tim       = current_date_time.toString("hh:mm:ss.zzz");
    QString   current_date      = current_date_time.toString("yyyy-MM-dd");
    ui->lua_log_textBrowser->append(
        TEXT_COLOR_BLUE("[" + current_date + "  " + current_tim + "]" + "# INFO", TEXT_SIZE_LARGE));
    ui->lua_log_textBrowser->append(TEXT_COLOR_WHILE(str, TEXT_SIZE_MEDIUM));
}

void lua::lua_cmd_run()
{
    uint8_t cmd[6] = { 0, CMD_TYPE_PROJECT, CMD_PROJECT_USERCODE, SUB_PROJECT_USERCODE_RUN, 0X00, 0X00 };
    mainwindow->my_serial->port_sendframe(cmd, 6);
}

void lua::lua_log_save()
{
    QString curPath = QDir::currentPath();  //获取系统当前目录
    curPath += "/spc100.log";
    QFile file(curPath);
    if (file.open(QIODevice::Append | QIODevice::Text)) {

        char*      str;
        QString    line_str = ui->lua_log_textBrowser->toPlainText();
        QByteArray ch       = line_str.toUtf8();
        str                 = ch.data();
        file.write(str);
        file.close();
        ui->lua_log_textBrowser->append(TEXT_COLOR_YELLOW("log保存成功", TEXT_SIZE_MEDIUM));
    } else {
        ui->lua_log_textBrowser->append(TEXT_COLOR_RED("log保存失败", TEXT_SIZE_MEDIUM));
    }
}

void lua::lua_cmd_log(uint8_t id)
{
    uint16_t len = ui->log_a_size_spinBox->value();
    if (id == SYNC_ID_B) {
        len = ui->log_b_size_spinBox->value();
    }

    uint8_t cmd[8] = { id, CMD_TYPE_READ, CMD_READ_LOG, 0, 0X02, 0X00, ( uint8_t )len, (uint8_t)(len >> 8) };
    mainwindow->my_serial->port_sendframe(cmd, 8);
}

void lua::lua_serial_connect_callback()
{
    ui->get_a_log_pushButton->setEnabled(true);
    ui->get_b_log_pushButton->setEnabled(true);
}

void lua::lua_serial_disconnect_callback()
{
    ui->get_a_log_pushButton->setEnabled(false);
    ui->get_b_log_pushButton->setEnabled(false);
    lua_download_info.status = LUA_DOWNLOAD_DOWNLOADING;
}

void lua::lua_cmd_response(uint8_t* frame, int32_t length)
{
    uint8_t cmd    = frame[2];
    uint8_t bootid = frame[0];
    if (bootid >= SYNC_ID_MAX) {
        return;
    }
    switch (cmd) {
    case CMD_PUBLIC_FILE_DOWNLOAD:
        switch (frame[3]) {
        case SUB_PUBLIC_FILE_DOWNLOAD_EOT_ACK:
        case SUB_PUBLIC_FILE_DOWNLOAD_SOH_ACK:
            lua_download_info.ack[bootid]        = frame[3];
            lua_download_info.error_code[bootid] = ( int8_t )frame[6];
            break;

        case SUB_PUBLIC_FILE_DOWNLOAD_STX_ACK:
            lua_download_info.ack[bootid]        = frame[3];
            lua_download_info.error_code[bootid] = ( int8_t )frame[6];
            lua_download_info.reqpackseq[bootid] = frame[7] | (frame[8] << 8) | (frame[9] << 16) | (frame[10] << 24);
            break;
        default:
            break;
        }
        break;
    case CMD_PUBLIC_FILE_READBACK:
        switch (frame[3]) {
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

void lua::lua_cmd_report_response(uint8_t* frame, int32_t length)
{
    uint8_t cmd    = frame[2];
    uint8_t bootid = frame[0];
    if (bootid >= SYNC_ID_MAX) {
        return;
    }
    switch (cmd) {
    case CMD_REPORT_INFO:
    case CMD_REPORT_LOG:
        lua_log_display(cmd, frame, length);
        break;
    default:
        break;
    }
}
