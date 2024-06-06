#-------------------------------------------------
#
# Project created by QtCreator 2023-06-25T09:36:39
#
#-------------------------------------------------

QT       += core gui
QT       += serialport
QT       += svg
QT += printsupport
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = rhdz
exists (./.git) {
    APP_VERSION   = $$system(git describe --tag --dirty="-development" --long)
} else {
    APP_VERSION     = None
}

DEFINES += APP_VERSION=\"\\\"$$APP_VERSION\\\"\"


# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    config/data_config.cpp \
    graphical_config/config_view.cpp \
    graphical_config/config_photo.cpp \
    graphical_config/config_param.cpp \
    device/version_dialog.cpp \
    graphical_config/safety_param_dialog.cpp \
    graphical_config/config_menu.cpp \
    graphical_config/config_element.cpp \
    logic_view/logic_menu.cpp \
    logic_view/logic_element.cpp \
    logic_view/input_block.cpp \
    logic_view/connect_point.cpp \
    logic_view/connection_line.cpp \
    logic_view/output_block.cpp \
    logic_view/base_rect_class.cpp \
    logic_view/base_logic_block.cpp \
    graphical_config/navigate_menu.cpp \
    device/device_info.cpp \
    project/project_debug.cpp \
    project/project_management.cpp \
    logic_view/logic_view.cpp \
    algorithm/QAESEncryption/qaesencryption.cpp \
    algorithm/MD5/md5.c \
    device/mydevice.cpp \
    my_serialport/my_serialport.cpp \
    my_serialport/transportcrc.cpp \
    device/log_dialog.cpp \
    device/status.cpp \
    device/upgrade.cpp \
    project/usercode_dialog.cpp \
    project/about_prajna.cpp \
    method/my_dialog.cpp \
    project/project_transmit.cpp \
    device/device_fault.cpp \
    graphical_config/config_device_fault.cpp \
    logic_view/logic_setting/and_or_logic_setting.cpp \
    logic_view/logic_setting/not_xor_xnor_logic_setting.cpp \
    logic_view/logic_setting/rs_trigger_logic_setting.cpp \
    logic_view/logic_setting/nencode_logic_setting.cpp \
    logic_view/logic_setting/ndecoder_logic_setting.cpp \
    logic_view/apply_logic_block.cpp \
    logic_view/logic_setting/apply_reset_setting.cpp \
    logic_view/logic_setting/apply_extern_device_monitors_setting.cpp \
    logic_view/logic_setting/apply_edge_detected_setting.cpp \
    logic_view/delay_counter_logic_block.cpp \
    logic_view/logic_setting/delay_on_off_setting.cpp \
    logic_view/logic_setting/delay_adjust_on_off_setting.cpp \
    logic_view/logic_setting/apply_freq_monitor_setting.cpp \
    logic_view/logic_setting/speed_cross_check_setting.cpp \
    logic_view/logic_setting/speed_monitor_setting.cpp \
    logic_view/speed_logic_block.cpp \
    logic_view/logic_view_config.cpp \
    logic_view/logic_setting/counter_event_setting.cpp \
    logic_view/logic_setting/counter_logging_setting.cpp \
    logic_view/logic_setting/speed_motionless_setting.cpp \
    logic_view/logic_setting/speed_value_compairsons_setting.cpp \
    project/project_report.cpp

HEADERS += \
        mainwindow.h \
    config/data_config.h \
    config/param_config.h \
    graphical_config/config_view.h \
    graphical_config/config_photo.h \
    graphical_config/graphical_config_param.h \
    graphical_config/config_param.h \
    device/version_dialog.h \
    graphical_config/safety_param_dialog.h \
    graphical_config/config_menu.h \
    graphical_config/config_element.h \
    logic_view/logic_menu.h \
    logic_view/logic_element.h \
    logic_view/input_block.h \
    logic_view/logic_view_config.h \
    logic_view/connect_point.h \
    logic_view/connection_line.h \
    logic_view/output_block.h \
    logic_view/base_rect_class.h \
    logic_view/base_logic_block.h \
    graphical_config/navigate_menu.h \
    device/device_info.h \
    project/project_debug.h \
    project/project_management.h \
    logic_view/logic_view.h \
    algorithm/MD5/md5.h \
    algorithm/QAESEncryption/qaesencryption.h \
    device/mydevice.h \
    my_serialport/my_serialport.h \
    my_serialport/transportcrc.h \
    device/log_dialog.h \
    device/status.h \
    device/upgrade.h \
    project/usercode_dialog.h \
    project/about_prajna.h \
    method/my_dialog.h \
    project/project_transmit.h \
    config/def.h \
    device/device_fault.h \
    graphical_config/config_device_fault.h \
    logic_view/logic_setting/and_or_logic_setting.h \
    logic_view/logic_setting/not_xor_xnor_logic_setting.h \
    logic_view/logic_setting/rs_trigger_logic_setting.h \
    logic_view/logic_setting/nencode_logic_setting.h \
    logic_view/logic_setting/ndecoder_logic_setting.h \
    logic_view/apply_logic_block.h \
    logic_view/logic_setting/apply_reset_setting.h \
    logic_view/logic_setting/apply_extern_device_monitors_setting.h \
    logic_view/logic_setting/apply_edge_detected_setting.h \
    logic_view/delay_counter_logic_block.h \
    logic_view/logic_setting/delay_on_off_setting.h \
    logic_view/logic_setting/delay_adjust_on_off_setting.h \
    logic_view/logic_setting/apply_freq_monitor_setting.h \
    logic_view/logic_setting/speed_cross_check_setting.h \
    logic_view/logic_setting/speed_monitor_setting.h \
    logic_view/speed_logic_block.h \
    logic_view/logic_setting/counter_event_setting.h \
    logic_view/logic_setting/counter_logging_setting.h \
    logic_view/logic_setting/speed_motionless_setting.h \
    logic_view/logic_setting/speed_value_compairsons_setting.h \
    project/project_report.h

FORMS += \
        mainwindow.ui \
    device/version_dialog.ui \
    graphical_config/safety_param_dialog.ui \
    device/log_dialog.ui \
    project/usercode_dialog.ui \
    device/device_fault.ui \
    logic_view/logic_setting/and_or_logic_setting.ui \
    logic_view/logic_setting/not_xor_xnor_logic_setting.ui \
    logic_view/logic_setting/rs_trigger_logic_setting.ui \
    logic_view/logic_setting/nencode_logic_setting.ui \
    logic_view/logic_setting/ndecoder_logic_setting.ui \
    logic_view/logic_setting/apply_reset_setting.ui \
    logic_view/logic_setting/apply_extern_device_monitors_setting.ui \
    logic_view/logic_setting/apply_edge_detected_setting.ui \
    logic_view/logic_setting/delay_on_off_setting.ui \
    logic_view/logic_setting/delay_adjust_on_off_setting.ui \
    logic_view/logic_setting/apply_freq_monitor_setting.ui \
    logic_view/logic_setting/speed_cross_check_setting.ui \
    logic_view/logic_setting/speed_monitor_setting.ui \
    logic_view/logic_setting/counter_event_setting.ui \
    logic_view/logic_setting/counter_logging_setting.ui \
    logic_view/logic_setting/speed_motionless_setting.ui \
    logic_view/logic_setting/speed_value_compairsons_setting.ui

RESOURCES += \
    qrc/doc.qrc \
    qrc/photo.qrc \
    qrc/logic.qrc \
    qrc/config.qrc

TEMPLATE = app
RC_ICONS = SafetyProgram.ico

DISTFILES +=
