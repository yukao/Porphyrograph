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

#ifndef _WIN32
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
        #include <sys/resource.h>
    #include <netdb.h>
    #include <unistd.h>
#else // !_WIN32
    //#define socklen_t int
    #include <winsock2.h>
    #include <Ws2tcpip.h>
    //#include <wspiapi.h>
#endif // _WIN32

#include "pg-interface-atelPort-display.h"
#include "pg-interface-atelPort-control.h"
#include "pg-interface-atelPort-comm.h"
#include "pg-interface-atelPort-window.h"
#include "pg-interface-atelPort-udp.h"
#include "pg-interface-atelPort-main.h"

#ifndef WIN32
#include <unistd.h>
#endif

#include <QtWidgets>
#include <QCloseEvent>
#include <QTime>

MainWindow::MainWindow() {
    /////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////
    // UDP AND MIDI INITIALIZATION
    //    UDPreceiver = new Receiver();
    //    UDPsender = new Sender();
    midiInOut = new MidiInOut();
    udpInOut = new UDPInOut();

    /////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////
    // CONFIGURATION TAB
    QGroupBox *UDPreceiverBox = new QGroupBox("Multicast Receiver",0);
    QVBoxLayout *mainLayoutReceiver = new QVBoxLayout;
    QLabel *statusLabelReceiver = new QLabel(tr("Listening for messages on port %2")
                                             .arg(udpInOut->IP_Servers[0]->Local_server_port));
    mainLayoutReceiver->addWidget(statusLabelReceiver);
    UDPreceiverBox->setLayout(mainLayoutReceiver);
    UDPreceiverBox->setMaximumHeight(80);

    QGroupBox *UDPsenderBox = new QGroupBox("Multicast Sender",0);
    QVBoxLayout *mainLayoutSender = new QVBoxLayout;
    QLabel *statusLabelSender = new QLabel(tr("Ready to multicast datagrams to %1 on port %2")
                                            .arg(QString::fromStdString(udpInOut->IP_Clients[0]->Remote_server_IP))
                                            .arg(udpInOut->IP_Clients[0]->Remote_server_port));
    mainLayoutSender->addWidget(statusLabelSender);
    UDPsenderBox->setLayout(mainLayoutSender);
    UDPsenderBox->setMaximumHeight(80);

    QGroupBox *midiInOutBox = new QGroupBox("MIDI In Out",0);
    QLabel *statusLabelMidi = new QLabel(tr("Midi In %1 ports & Out %2 ports").arg(midiInOut->indIn).arg(midiInOut->indOut));
    QVBoxLayout *mainLayoutMidi = new QVBoxLayout;
    mainLayoutMidi->addWidget(statusLabelMidi);
    midiInOutBox->setLayout(mainLayoutMidi);
    midiInOutBox->setMaximumHeight(80);

    QVBoxLayout *configurationTabLayout = new QVBoxLayout;
    configurationTabLayout->addWidget(UDPreceiverBox);
    configurationTabLayout->addWidget(UDPsenderBox);
    configurationTabLayout->addWidget(midiInOutBox);

    // Set layout in conifguration widget
    configurationTabWindow = new QWidget();
    configurationTabWindow->setLayout(configurationTabLayout);
    configurationTabWindow->setMaximumHeight(250);

    /////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////
    // LEFT PART OF THE INTERFACES THAT DISPLAYS THE VALUES
    /////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////
    // GRAPHICAL DISPLAY
    graphsTabDisplays = new GraphicalDisplays(tr("DISPLAY"),QString("textures/graphsIF-med.jpg"), _Graphs_display);
    tracksTabDisplays = new GraphicalDisplays(tr("DISPLAY"),QString("textures/tracksIF-med.jpg"), _Tracks_display);
    pathsTabDisplays = new GraphicalDisplays(tr("DISPLAY"),QString("textures/tracksIF-med.jpg"), _Paths_display);

    /////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////
    // RIGHT PART OF THE INTERFACES THAT DISPLAYS THE CONTROLLERS
    /////////////////////////////////////////////////////////////////////
    // GRAPHICAL SLIDERS
    graphsTabControllers = new GraphicalControllers(tr("CONTROLLER"),_Graphs_display);
    tracksTabControllers = new GraphicalControllers(tr("CONTROLLER"), _Tracks_display);
    pathsTabControllers = new GraphicalControllers(tr("CONTROLLER"), _Paths_display);

    lastPressedSlider = NULL;
    lastTimePressedSlider = QTime::currentTime();

    /////////////////////////////////////////////////////////
    // GRAPHS TAB
    QHBoxLayout *graphicalDisplayAndGraphsLayout = new QHBoxLayout;
    graphicalDisplayAndGraphsLayout->addWidget(graphsTabDisplays);
    graphicalDisplayAndGraphsLayout->addWidget(graphsTabControllers);

    // Set layout in graphs widget
    graphsTabWindow = new QWidget();
    graphsTabWindow->setLayout(graphicalDisplayAndGraphsLayout);
    graphsTabWindow->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    graphsTabWindow->setMinimumHeight(730);

    /////////////////////////////////////////////////////////
    // TRACKS TAB
    QHBoxLayout *graphicalDisplayAndTracksLayout = new QHBoxLayout;
    graphicalDisplayAndTracksLayout->addWidget(tracksTabDisplays);
    graphicalDisplayAndTracksLayout->addWidget(tracksTabControllers);

    // Set layout in tracks widget
    tracksTabWindow = new QWidget();
    tracksTabWindow->setLayout(graphicalDisplayAndTracksLayout);
    tracksTabWindow->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    tracksTabWindow->setMinimumHeight(730);

    /////////////////////////////////////////////////////////
    // PATHS TAB
    QHBoxLayout *graphicalDisplayAndpathsLayout = new QHBoxLayout;
    graphicalDisplayAndpathsLayout->addWidget(pathsTabDisplays);
    graphicalDisplayAndpathsLayout->addWidget(pathsTabControllers);

    // Set layout in paths widget
    pathsTabWindow = new QWidget();
    pathsTabWindow->setLayout(graphicalDisplayAndpathsLayout);
    pathsTabWindow->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    pathsTabWindow->setMinimumHeight(730);

    /////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////
    // FINAL INTERFACE: collects all tab windows
    /////////////////////////////////////////////////////////
    // Set layout in QTabWidget
    tabbedWindow = new QTabWidget();
    tabbedWindow->addTab(configurationTabWindow,tr("&CONF"));
    tabbedWindow->addTab(graphsTabWindow,tr("&GRAPHS"));
    tabbedWindow->addTab(tracksTabWindow,tr("&TRACKS"));
    tabbedWindow->addTab(pathsTabWindow,tr("&PATHS"));
    tabbedWindow->setCurrentIndex(0);

    connect(tabbedWindow, SIGNAL(currentChanged(int)), this, SLOT(updateCurrentTabWindow(int)));

    Controller_DisplayCxForInteractionFeedback();

    // Set tabbedWindow as the central layout of the main window
     setCentralWidget(tabbedWindow);

    createStatusBar();

    setWindowTitle(tr("porphyrograph"));
    setWindowMyFlags();

    /////////////////////////////////////////////////////////
    // TIMER (CREATED FIRST TO MAKE CONNECTIONS AFTERWARDS (MIDI & UDP)
    applicationTimerMidiUDP = new QTimer(this);

    connect(applicationTimerMidiUDP, SIGNAL(timeout()), midiInOut, SLOT(receiveMidiIn()));
    connect(applicationTimerMidiUDP, SIGNAL(timeout()), udpInOut, SLOT(receiveAndSendIPMessages()));

    applicationTimerMidiUDP->start(15);
}

/////////////////////////////////////////////////////////
// SLOTS
void MainWindow::updateCurrentTabWindow(int index) {
    if(DisplayType(index) != currentTabWindow
            && (currentTabWindow == _Graphs_display
                || currentTabWindow == _Tracks_display)) {
        lastTabWindow = currentTabWindow;
        currentTabWindow = DisplayType(index);
    }
}



/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
// VISUAL FEEDBACK OF BUTTONS (RIGHT PART) ON LABELS (LEFT PART)
/////////////////////////////////////////////////////////
void MainWindow::Controller_DisplayCxForInteractionFeedback(void) {
    /////////////////////////////////////////////////////////
    // CONNECTION BETWEEN BUTTONS AND LABELS FOR THE INTERACTION FEEDBACK ON LABELS
    for(unsigned int indMainButtonRow = 0 ; indMainButtonRow < _NbRows_mainButtons_graphs ; indMainButtonRow++ ) {
        for(unsigned int indMainButtonCol = 0 ; indMainButtonCol < graphs_mainButtonsData[indMainButtonRow].size() ; indMainButtonCol++ ) {
            // scenario-based value changes
            if(graphsTabControllers->mainButtons_graphs[indMainButtonRow][indMainButtonCol]
                    && indMainButtonRow < graphs_mainButtonsData.size()
                    && indMainButtonCol < graphs_mainButtonsData[indMainButtonRow].size() )
            {
                graphsTabControllers->mainButtons_graphs[indMainButtonRow][indMainButtonCol]->setAssociatedSingleLabel(
                    graphsTabDisplays->graphsButtonsLabels->labelRow[indMainButtonRow][indMainButtonCol]);
            }
        }
    }
    for(unsigned int indMainButtonRow = 0 ; indMainButtonRow < _NbRows_mainButtons_graphs ; indMainButtonRow++ ) {
        for(unsigned int indMainButtonCol = 0 ; indMainButtonCol < tracks_mainButtonsData[indMainButtonRow].size() ; indMainButtonCol++ ) {
            // scenario-based value changes
            if(tracksTabControllers->mainButtons_tracks[indMainButtonRow][indMainButtonCol]
                    && indMainButtonRow < tracks_mainButtonsData.size()
                    && indMainButtonCol < tracks_mainButtonsData[indMainButtonRow].size() ) {
                tracksTabControllers->mainButtons_tracks[indMainButtonRow][indMainButtonCol]->setAssociatedSingleLabel(
                    tracksTabDisplays->tracksButtonsLabels->labelRow[indMainButtonRow][indMainButtonCol]);
            }
        }
    }
    for(unsigned int indMainButtonRow = 0 ; indMainButtonRow < _NbRows_buttons_paths ; indMainButtonRow++ ) {
        for(unsigned int indMainButtonCol = 0 ; indMainButtonCol < paths_mainButtonsData[indMainButtonRow].size() ; indMainButtonCol++ ) {
            // scenario-based value changes
            if(pathsTabControllers->mainButtons_paths[indMainButtonRow][indMainButtonCol]
                    && indMainButtonRow < paths_mainButtonsData.size()
                    && indMainButtonCol < paths_mainButtonsData[indMainButtonRow].size() ) {
                pathsTabControllers->mainButtons_paths[indMainButtonRow][indMainButtonCol]->setAssociatedSingleLabel(
                    pathsTabDisplays->pathsButtonsLabels->labelRow[indMainButtonRow][indMainButtonCol]);
            }
        }
    }

    /////////////////////////////////////////////////////////
    // CONNECTION BETWEEN SLIDERS AND LED BARS FOR THE INTERACTION FEEDBACK ON SLIDERS
    for(unsigned int indMainSliderRow = 0 ; indMainSliderRow < _MaxGraphsInOutDataRows ; indMainSliderRow++ ) {
        for(unsigned int indMainSliderCol = 0 ; indMainSliderCol < graphs_mainSlidersData[indMainSliderRow].size() ; indMainSliderCol++ ) {
            // scenario-based value changes
            if(graphsTabControllers->verticalSliders_graphs[indMainSliderRow][indMainSliderCol]) {
                unsigned int indControllerDataRow
                        = graphsTabControllers->verticalSliders_graphs[indMainSliderRow][indMainSliderCol]->getIndLEDBarDataRow();
                unsigned int indControllerDataCol
                        = graphsTabControllers->verticalSliders_graphs[indMainSliderRow][indMainSliderCol]->getIndLEDBarDataCol();
                if(graphsTabControllers->verticalSliders_graphs[indMainSliderRow][indMainSliderCol]->getAssociatedDisplayType() == _Box ) {
                    if( indControllerDataRow < graphs_mainSlidersData.size()
                        && indControllerDataCol < graphs_mainSlidersData[indControllerDataRow].size() ) {
                        graphsTabControllers->verticalSliders_graphs[indMainSliderRow][indMainSliderCol]->setAssociatedBox(
                            graphsTabDisplays->graphsFlashGrid->getBox(indControllerDataRow + 1, indControllerDataCol + 1));
                    }
                }
                else {
                    if( indControllerDataRow < graphs_mainSlidersData.size()
                        && indControllerDataCol < graphs_mainSlidersData[indControllerDataRow].size()
                            && graphsTabDisplays->graphicalLEDBarRows[indControllerDataRow]) {
                        graphsTabControllers->verticalSliders_graphs[indMainSliderRow][indMainSliderCol]->setAssociatedLEDBar(
                            graphsTabDisplays->graphicalLEDBarRows[indControllerDataRow]->getLEDBar(indControllerDataCol));
                    }
                }
            }
        }
    }
    for(unsigned int indMainSliderRow = 0 ; indMainSliderRow < _MaxTracksInOutDataRows ; indMainSliderRow++ ) {
        for(unsigned int indMainSliderCol = 0 ; indMainSliderCol < tracks_mainSlidersData[indMainSliderRow].size() ; indMainSliderCol++ ) {
            // scenario-based value changes
            if(tracksTabControllers->verticalSliders_tracks[indMainSliderRow][indMainSliderCol]) {
                unsigned int indControllerDataRow
                        = tracksTabControllers->verticalSliders_tracks[indMainSliderRow][indMainSliderCol]->getIndLEDBarDataRow();
                unsigned int indControllerDataCol
                        = tracksTabControllers->verticalSliders_tracks[indMainSliderRow][indMainSliderCol]->getIndLEDBarDataCol();
                if( indControllerDataRow < tracks_mainSlidersData.size()
                    && indControllerDataCol < tracks_mainSlidersData[indControllerDataRow].size()
                        && tracksTabDisplays->graphicalLEDBarRows[indControllerDataRow]) {
                    tracksTabControllers->verticalSliders_tracks[indMainSliderRow][indMainSliderCol]->setAssociatedLEDBar(
                        tracksTabDisplays->graphicalLEDBarRows[indControllerDataRow]->getLEDBar(indControllerDataCol));
                }
            }
        }
    }
    for(unsigned int indMainSliderRow = 0 ; indMainSliderRow < _MaxPathsInOutDataRows ; indMainSliderRow++ ) {
        for(unsigned int indMainSliderCol = 0 ; indMainSliderCol < paths_mainSlidersData[indMainSliderRow].size() ; indMainSliderCol++ ) {
            // scenario-based value changes
            if(pathsTabControllers->verticalSliders_paths[indMainSliderRow][indMainSliderCol]) {
                unsigned int indControllerDataRow
                        = pathsTabControllers->verticalSliders_paths[indMainSliderRow][indMainSliderCol]->getIndLEDBarDataRow();
                unsigned int indControllerDataCol
                        = pathsTabControllers->verticalSliders_paths[indMainSliderRow][indMainSliderCol]->getIndLEDBarDataCol();
                if( indControllerDataRow < paths_mainSlidersData.size()
                    && indControllerDataCol < paths_mainSlidersData[indControllerDataRow].size()
                        && pathsTabDisplays->graphicalLEDBarRows[indControllerDataRow]) {
                    pathsTabControllers->verticalSliders_paths[indMainSliderRow][indMainSliderCol]->setAssociatedLEDBar(
                        pathsTabDisplays->graphicalLEDBarRows[indControllerDataRow]->getLEDBar(indControllerDataCol));
                }
            }
        }
    }
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"),10000);
}

void MainWindow::setWindowMyFlags(void) {
    Qt::WindowFlags flags = 0;
    flags |= Qt::FramelessWindowHint;
    setWindowFlags(flags);
    move(0,0);
    setMaximumHeight(768);
    setMinimumHeight(768);
    setMaximumWidth(1360);
    setMinimumWidth(1360);
}

void MainWindow::setCurrentTabWindow(DisplayType display) {
    switch( display ) {
    case _Config_display:
        currentTabWindow = display;
        tabbedWindow->setCurrentIndex(currentTabWindow);
        break;
    case _Graphs_display:
        currentTabWindow = display;
        tabbedWindow->setCurrentIndex(currentTabWindow);
        reinitializeMidiLighting(_Graphs_display);
        break;
    case _Tracks_display:
        currentTabWindow = display;
        tabbedWindow->setCurrentIndex(currentTabWindow);
        reinitializeMidiLighting(_Tracks_display);
        break;
    case _Paths_display:
        currentTabWindow = display;
        tabbedWindow->setCurrentIndex(currentTabWindow);
        break;
    default:
        break;
    }
}


void MainWindow::closeEvent (QCloseEvent *event)
{
    statusBar()->showMessage(tr("Quitting"));

    // clean connection abort
    //    UDPreceiver->cleanUp();
    //    UDPsender->cleanUp();

    midiInOut->midiCleanup();

    udpInOut->ReleaseUDP();

    // delay to read status bar info
#ifdef WIN32
    Sleep(1000);
#else
    usleep(1000 * 1000);   // usleep takes sleep time in us (1 millionth of a second)
#endif
    event->accept();
}
