#-------------------------------------------------
#
# Project created by QtCreator 2023-06-25T09:36:39
#
#-------------------------------------------------

QT       += core gui
QT       += serialport
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = rhdz
exists (./.git) {
    APP_VERSION   = $$system(git describe --tag --dirty --long)
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
    my_serialport.cpp \
    transportcrc.cpp \
    md5.c \
    upgrade.cpp \
    lua.cpp \
    status.cpp \
    param.cpp \
    block_class/condition_block.cpp \
    block_class/connect_block.cpp \
    block_class/connect_line.cpp \
    block_class/logic_block.cpp \
    logic_view.cpp \
    list_condi/condition_view.cpp \
    logic_view/logic_tools.cpp \
    logic_view/tool_block.cpp \
    config/data_config.cpp \
    logic_view/coroutine_lua.cpp \
    project_management.cpp \
    QAESEncryption/qaesencryption.cpp \
    project_report.cpp \
    mydevice.cpp \
    project_debug.cpp

HEADERS += \
        mainwindow.h \
    my_serialport.h \
    transportcrc.h \
    def.h \
    upgrade.h \
    lua.h \
    status.h \
    param.h \
    block_class/condition_block.h \
    block_class/connect_block.h \
    block_class/connect_line.h \
    block_class/logic_block.h \
    config/data_config.h \
    config/param_config.h \
    logic_view.h \
    list_condi/condition_view.h \
    logic_view/logic_tools.h \
    logic_view/tool_block.h \
    logic_view/coroutine_lua.h \
    project_management.h \
    QAESEncryption/qaesencryption.h \
    project_report.h \
    mydevice.h \
    project_debug.h

FORMS += \
        mainwindow.ui

RESOURCES += \
    photo.qrc

TEMPLATE = app
RC_ICONS = 02.ico
