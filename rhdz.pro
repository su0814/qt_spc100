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
    param.cpp

HEADERS += \
        mainwindow.h \
    my_serialport.h \
    transportcrc.h \
    def.h \
    upgrade.h \
    lua.h \
    status.h \
    param.h

FORMS += \
        mainwindow.ui

RESOURCES += \
    photo.qrc

TEMPLATE = app
RC_ICONS = 02.ico
