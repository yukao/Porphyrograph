#-------------------------------------------------
#
# Project created by QtCreator 2017-06-25T09:11:40
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Porphyrograph-interface-demo
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS __WINDOWS_MM__ demo

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH = ./RtMidi

LIBS += -lWS2_32 -lvfw32 -lWinMM

SOURCES += \
    pg-interface-core-main.cpp \
    pg-interface-core-window.cpp \
    pg-interface-core-comm.cpp \
    pg-interface-core-display.cpp \
    pg-interface-core-udp.cpp \
    pg-interface-core-control.cpp \
    pg-interface-core-data.cpp \
    RtMidi/RtMidi.cpp \
    RtMidi/rtmidi_c.cpp


HEADERS += \
    pg-interface-core-comm.h \
    pg-interface-core-display.h \
    pg-interface-core-udp.h \
    pg-interface-core-window.h \
    pg-interface-core-main.h \
    pg-interface-core-control.h \
    pg-interface-core-data.h \
    RtMidi/RtMidi.h \
    RtMidi/rtmidi_c.h

FORMS +=

DISTFILES += \
    qss/porphyrograph-interface.qss
