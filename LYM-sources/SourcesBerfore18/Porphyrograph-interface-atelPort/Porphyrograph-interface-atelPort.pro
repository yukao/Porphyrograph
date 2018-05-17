#-------------------------------------------------
#
# Project created by QtCreator 2017-06-25T09:11:40
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Porphyrograph-interface-atelPort
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS __WINDOWS_MM__

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH = ../share/rtmidi-2.1.1

LIBS += ../share/rtmidi-2.1.1/build-rtmidi-qt-Desktop_Qt_5_9_0_MinGW_32bit-Release/release/librtmidi-qt.a \
    ../share/portmidi/release/libportmidi.a \
    "C:/Program Files (x86)/Windows Kits/10/Lib/10.0.15063.0/um/x86/Winmm.lib" \
    "C:/Program Files (x86)/Windows Kits/10/Lib/10.0.15063.0/um/x86/vfw32.lib" \
    "C:/Program Files (x86)/Windows Kits/10/Lib/10.0.15063.0/um/x86/ws2_32.lib"


SOURCES += \
    pg-interface-atelPort-main.cpp \
    pg-interface-atelPort-window.cpp \
    pg-interface-atelPort-comm.cpp \
    pg-interface-atelPort-display.cpp \
    pg-interface-atelPort-udp.cpp \
    pg-interface-atelPort-control.cpp \
    pg-interface-atelPort-data.cpp

HEADERS += \
    pg-interface-atelPort-comm.h \
    pg-interface-atelPort-display.h \
    pg-interface-atelPort-udp.h \
    pg-interface-atelPort-window.h \
    pg-interface-atelPort-main.h \
    pg-interface-atelPort-control.h \
    pg-interface-atelPort-data.h

FORMS +=

DISTFILES += \
    qss/porphyrograph-interface.qss
