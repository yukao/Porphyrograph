#-------------------------------------------------
#
# Project created by QtCreator 2017-06-25T09:11:40
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Porphyrograph-interface-song
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
    pg-interface-song-main.cpp \
    pg-interface-song-window.cpp \
    pg-interface-song-comm.cpp \
    pg-interface-song-display.cpp \
    pg-interface-song-udp.cpp \
    pg-interface-song-control.cpp \
    pg-interface-song-data.cpp

HEADERS += \
    pg-interface-song-comm.h \
    pg-interface-song-display.h \
    pg-interface-song-udp.h \
    pg-interface-song-window.h \
    pg-interface-song-main.h \
    pg-interface-song-control.h \
    pg-interface-song-data.h \
    pg-interface-all_include.h

FORMS +=

DISTFILES += \
    qss/porphyrograph-interface.qss
