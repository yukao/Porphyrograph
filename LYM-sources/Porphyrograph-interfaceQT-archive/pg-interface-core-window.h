/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include <QMainWindow>
#include <QTime>

QT_BEGIN_NAMESPACE
class QCheckBox;
class QComboBox;
class QGroupBox;
class QLabel;
class QSpinBox;
class QSlider;
class QStackedWidget;
class QHBoxLayout;
class QGridLayout;
QT_END_NAMESPACE

class MainWindow;

#include "pg-interface-core-display.h"
#include "pg-interface-core-control.h"
#include "pg-interface-core-comm.h"
#include "pg-interface-core-udp.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

    GraphicalDisplays *tabDisplays[_NumberOfDisplays];
    GraphicalControllers *tabControllers[_NumberOfDisplays];
    QWidget *tabWindow[_NumberOfDisplays];

    QLabel *statusLabel;

    QTabWidget *tabbedWindow;

    QTimer *applicationTimerMidiUDP;

    //    Receiver *UDPreceiver;
    //    Sender   *UDPsender;

    MidiInOut *midiInOut;
    UDPInOut *udpInOut;

    DoubleSlider *lastPressedSlider;
    QTime lastTimePressedSlider;

    bool eventFilter(QObject* obj, QEvent* event);

    void registerMidiEventsToControllers( void );
    void propagateMidiEventsToControllers( DisplayType targetDisplay,
                                           int dataType,
                                           int indChannel,
                                           int controlIndex,
                                           int value );
    void propagateUDPEventsToControllers(char *OSCTag, char *firstArgt, char *secondArgt, float args[], int nbargts);

    void Controller_DisplayCxForInteractionFeedback(void);
    void reinitializeMidiLighting(DisplayType targetDisplay);
    void swithOffMidiLighting(DisplayType targetDisplay);
    int getCurrentTabWindow(void);
    int getLastTabWindow(void);

public slots:
    void setCurrentTabWindow(int index);

protected:
    void closeEvent (QCloseEvent *event);

private:
    DisplayType currentTabWindow;
    DisplayType lastTabWindow;
    void createStatusBar();
    void setWindowMyFlags();
};

extern MainWindow *pg_window;
extern QApplication *pg_app;

#endif
