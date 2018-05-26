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

#include "pg-interface-song-display.h"
#include "pg-interface-song-control.h"
#include "pg-interface-song-comm.h"
#include "pg-interface-song-window.h"
#include "pg-interface-song-udp.h"
#include "pg-interface-song-main.h"

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
    tabWindow[_Config_display] = new QWidget();
    tabWindow[_Config_display]->setLayout(configurationTabLayout);
    tabWindow[_Config_display]->setMaximumHeight(250);

    /////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////
    // LEFT PART OF THE INTERFACES THAT DISPLAYS THE VALUES
    /////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////
    // GRAPHICAL DISPLAY
    tabDisplays[_Beat_display] = new GraphicalDisplays(_Beat_display, false);
    tabDisplays[_Beat_Pulse_display] = new GraphicalDisplays(_Beat_display, true);

    tabDisplays[_Mixer_display] = new GraphicalDisplays(_Mixer_display, false);
    tabDisplays[_Mixer_Pulse_display] = new GraphicalDisplays(_Mixer_display, true);

    tabDisplays[_FX_display] = new GraphicalDisplays(_FX_display, false);
    tabDisplays[_FX_Pulse_display] = new GraphicalDisplays(_FX_display, true);

    tabDisplays[_Paths_display] = new GraphicalDisplays(_Paths_display, false);
    tabDisplays[_Paths_Pulse_display] = new GraphicalDisplays(_Paths_display, true);

#ifdef TVW
    tabDisplays[_TVW_display] = new GraphicalDisplays(_TVW_display, false);
    tabDisplays[_TVW_Pulse_display] = new GraphicalDisplays(_TVW_display, true);
#endif

    /////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////
    // RIGHT PART OF THE INTERFACES THAT DISPLAYS THE CONTROLLERS
    /////////////////////////////////////////////////////////////////////
    // GRAPHICAL SLIDERS
    tabControllers[_Beat_display] = new GraphicalControllers(_Beat_display, false);
    tabControllers[_Beat_Pulse_display] = new GraphicalControllers(_Beat_display, true);

    tabControllers[_Mixer_display] = new GraphicalControllers(_Mixer_display, false);
    tabControllers[_Mixer_Pulse_display] = new GraphicalControllers(_Mixer_display, true);

    tabControllers[_FX_display] = new GraphicalControllers(_FX_display, false);
    tabControllers[_FX_Pulse_display] = new GraphicalControllers(_FX_display, true);

    tabControllers[_Paths_display] = new GraphicalControllers(_Paths_display, false);
    tabControllers[_Paths_Pulse_display] = new GraphicalControllers(_Paths_display, true);

#ifdef TVW
    tabControllers[_TVW_display] = new GraphicalControllers(_TVW_display, false);
    tabControllers[_TVW_Pulse_display] = new GraphicalControllers(_TVW_display, true);
#endif

    lastPressedSlider = NULL;
    lastTimePressedSlider = QTime::currentTime();

    /////////////////////////////////////////////////////////
    // Beat TAB
    QHBoxLayout *graphicalDisplayAndBeatLayout = new QHBoxLayout;
    graphicalDisplayAndBeatLayout->addWidget(tabDisplays[_Beat_display]);
    graphicalDisplayAndBeatLayout->addWidget(tabControllers[_Beat_display]);

    // Set layout in graphs widget
    tabWindow[_Beat_display] = new QWidget();
    tabWindow[_Beat_display]->setLayout(graphicalDisplayAndBeatLayout);
    tabWindow[_Beat_display]->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    tabWindow[_Beat_display]->setMinimumHeight(730);

    /////////////////////////////////////////////////////////
    // Beat PULSE TAB
    QHBoxLayout *graphicalDisplayAndPulseBeatLayout = new QHBoxLayout;
    graphicalDisplayAndPulseBeatLayout->addWidget(tabDisplays[_Beat_Pulse_display]);
    graphicalDisplayAndPulseBeatLayout->addWidget(tabControllers[_Beat_Pulse_display]);

    // Set layout in graphs widget
    tabWindow[_Beat_Pulse_display] = new QWidget();
    tabWindow[_Beat_Pulse_display]->setLayout(graphicalDisplayAndPulseBeatLayout);
    tabWindow[_Beat_Pulse_display]->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    tabWindow[_Beat_Pulse_display]->setMinimumHeight(730);

    /////////////////////////////////////////////////////////
    // Mixer TAB
    QHBoxLayout *graphicalDisplayAndMixerLayout = new QHBoxLayout;
    graphicalDisplayAndMixerLayout->addWidget(tabDisplays[_Mixer_display]);
    graphicalDisplayAndMixerLayout->addWidget(tabControllers[_Mixer_display]);

    // Set layout in Mixer widget
    tabWindow[_Mixer_display] = new QWidget();
    tabWindow[_Mixer_display]->setLayout(graphicalDisplayAndMixerLayout);
    tabWindow[_Mixer_display]->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    tabWindow[_Mixer_display]->setMinimumHeight(730);

    /////////////////////////////////////////////////////////
    // Mixer PULSE TAB
    QHBoxLayout *graphicalDisplayAndPulseMixerLayout = new QHBoxLayout;
    graphicalDisplayAndPulseMixerLayout->addWidget(tabDisplays[_Mixer_Pulse_display]);
    graphicalDisplayAndPulseMixerLayout->addWidget(tabControllers[_Mixer_Pulse_display]);

    // Set layout in Mixer widget
    tabWindow[_Mixer_Pulse_display] = new QWidget();
    tabWindow[_Mixer_Pulse_display]->setLayout(graphicalDisplayAndPulseMixerLayout);
    tabWindow[_Mixer_Pulse_display]->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    tabWindow[_Mixer_Pulse_display]->setMinimumHeight(730);

    /////////////////////////////////////////////////////////
    // FX TAB
    QHBoxLayout *graphicalDisplayAndFXLayout = new QHBoxLayout;
    graphicalDisplayAndFXLayout->addWidget(tabDisplays[_FX_display]);
    graphicalDisplayAndFXLayout->addWidget(tabControllers[_FX_display]);

    // Set layout in FX widget
    tabWindow[_FX_display] = new QWidget();
    tabWindow[_FX_display]->setLayout(graphicalDisplayAndFXLayout);
    tabWindow[_FX_display]->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    tabWindow[_FX_display]->setMinimumHeight(730);

    /////////////////////////////////////////////////////////
    // FX PULSE TAB
    QHBoxLayout *graphicalDisplayAndPulseFXLayout = new QHBoxLayout;
    graphicalDisplayAndPulseFXLayout->addWidget(tabDisplays[_FX_Pulse_display]);
    graphicalDisplayAndPulseFXLayout->addWidget(tabControllers[_FX_Pulse_display]);

    // Set layout in FX widget
    tabWindow[_FX_Pulse_display] = new QWidget();
    tabWindow[_FX_Pulse_display]->setLayout(graphicalDisplayAndPulseFXLayout);
    tabWindow[_FX_Pulse_display]->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    tabWindow[_FX_Pulse_display]->setMinimumHeight(730);

    /////////////////////////////////////////////////////////
    // PATHS TAB
    QHBoxLayout *graphicalDisplayAndpathsLayout = new QHBoxLayout;
    graphicalDisplayAndpathsLayout->addWidget(tabDisplays[_Paths_display]);
    graphicalDisplayAndpathsLayout->addWidget(tabControllers[_Paths_display]);

    // Set layout in paths widget
    tabWindow[_Paths_display] = new QWidget();
    tabWindow[_Paths_display]->setLayout(graphicalDisplayAndpathsLayout);
    tabWindow[_Paths_display]->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    tabWindow[_Paths_display]->setMinimumHeight(730);

    /////////////////////////////////////////////////////////
    // PATHS PULSE TAB
    QHBoxLayout *graphicalDisplayAndPulsePathsLayout = new QHBoxLayout;
    graphicalDisplayAndPulsePathsLayout->addWidget(tabDisplays[_Paths_Pulse_display]);
    graphicalDisplayAndPulsePathsLayout->addWidget(tabControllers[_Paths_Pulse_display]);

    // Set layout in paths widget
    tabWindow[_Paths_Pulse_display] = new QWidget();
    tabWindow[_Paths_Pulse_display]->setLayout(graphicalDisplayAndPulsePathsLayout);
    tabWindow[_Paths_Pulse_display]->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    tabWindow[_Paths_Pulse_display]->setMinimumHeight(730);

#ifdef TVW
    /////////////////////////////////////////////////////////
    // TVW TAB
    QHBoxLayout *graphicalDisplayAndTVWLayout = new QHBoxLayout;
    graphicalDisplayAndTVWLayout->addWidget(tabDisplays[_TVW_display]);
    graphicalDisplayAndTVWLayout->addWidget(tabControllers[_TVW_display]);

    // Set layout in TVW widget
    tabWindow[_TVW_display] = new QWidget();
    tabWindow[_TVW_display]->setLayout(graphicalDisplayAndTVWLayout);
    tabWindow[_TVW_display]->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    tabWindow[_TVW_display]->setMinimumHeight(730);

    /////////////////////////////////////////////////////////
    // TVW PULSE TAB
    QHBoxLayout *graphicalDisplayAndPulseTVWLayout = new QHBoxLayout;
    graphicalDisplayAndPulseTVWLayout->addWidget(tabDisplays[_TVW_Pulse_display]);
    graphicalDisplayAndPulseTVWLayout->addWidget(tabControllers[_TVW_Pulse_display]);

    // Set layout in TVW widget
    tabWindow[_TVW_Pulse_display] = new QWidget();
    tabWindow[_TVW_Pulse_display]->setLayout(graphicalDisplayAndPulseTVWLayout);
    tabWindow[_TVW_Pulse_display]->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    tabWindow[_TVW_Pulse_display]->setMinimumHeight(730);
#endif

    /////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////
    // FINAL INTERFACE: collects all tab windows
    /////////////////////////////////////////////////////////
    // Set layout in QTabWidget
    tabbedWindow = new QTabWidget;
    tabbedWindow->addTab(tabWindow[_Config_display],TabbedWindowTitles[_Config_display]);
    tabbedWindow->addTab(tabWindow[_Beat_display],TabbedWindowTitles[_Beat_display]);
    tabbedWindow->addTab(tabWindow[_Beat_Pulse_display],TabbedWindowTitles[_Beat_Pulse_display]);
    tabbedWindow->addTab(tabWindow[_Mixer_display],TabbedWindowTitles[_Mixer_display]);
    tabbedWindow->addTab(tabWindow[_Mixer_Pulse_display],TabbedWindowTitles[_Mixer_Pulse_display]);
    tabbedWindow->addTab(tabWindow[_FX_display],TabbedWindowTitles[_FX_display]);
    tabbedWindow->addTab(tabWindow[_FX_Pulse_display],TabbedWindowTitles[_FX_Pulse_display]);
    tabbedWindow->addTab(tabWindow[_Paths_display],TabbedWindowTitles[_Paths_display]);
    tabbedWindow->addTab(tabWindow[_Paths_Pulse_display],TabbedWindowTitles[_Paths_Pulse_display]);
#ifdef TVW
    tabbedWindow->addTab(tabWindow[_TVW_display],TabbedWindowTitles[_TVW_display]);
    tabbedWindow->addTab(tabWindow[_TVW_Pulse_display],TabbedWindowTitles[_TVW_Pulse_display]);
#endif

    tabbedWindow->setCurrentIndex(_Config_display);
    currentTabWindow = _Config_display;
    lastTabWindow = _Config_display;

    connect(tabbedWindow, SIGNAL(currentChanged(int)), this, SLOT(setCurrentTabWindow(int)));

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
void MainWindow::setCurrentTabWindow(int index) {
    if(DisplayType(index) != currentTabWindow) {
        lastTabWindow = currentTabWindow;
        currentTabWindow = DisplayType(index);
        tabbedWindow->setCurrentIndex(currentTabWindow);
        if( currentTabWindow >= _Beat_display && currentTabWindow < _NumberOfDisplays ) {
            reinitializeMidiLighting(currentTabWindow);
        }
    }
    // std::cout << "currentTabWindow " << currentTabWindow << std::endl;
}

int MainWindow::getCurrentTabWindow() {
    return currentTabWindow;
}
int MainWindow::getLastTabWindow() {
    return lastTabWindow;
}

/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
// VISUAL FEEDBACK OF BUTTONS (RIGHT PART) ON LABELS (LEFT PART)
/////////////////////////////////////////////////////////
void MainWindow::Controller_DisplayCxForInteractionFeedback(void) {
    /////////////////////////////////////////////////////////
    /// Beat TAB
    /////////////////////////////////////////////////////////
    // CONNECTION BETWEEN BUTTONS AND LABELS FOR THE INTERACTION FEEDBACK ON LABELS
    for(unsigned int indMainButtonRow = 0 ; indMainButtonRow < ButtonsBeatData.size() ; indMainButtonRow++ ) {
        for(unsigned int indMainButtonCol = 0 ; indMainButtonCol < ButtonsBeatData[indMainButtonRow].size() ; indMainButtonCol++ ) {
            // scenario-based value changes
            if(tabControllers[_Beat_display]->mainButtons[indMainButtonRow][indMainButtonCol]
                    && indMainButtonRow < ButtonsBeatData.size()
                    && indMainButtonCol < ButtonsBeatData[indMainButtonRow].size() )
            {
                tabControllers[_Beat_display]->mainButtons[indMainButtonRow][indMainButtonCol]->setAssociatedSingleLabel(
                    tabDisplays[_Beat_display]->graphicalLabelRows[indMainButtonRow]->getSingleLabel(indMainButtonCol));
            }
            // there are no pulse feedback on button values -> nothing to do for the pulsed tab
        }
    }
    /////////////////////////////////////////////////////////
    // CONNECTION BETWEEN SLIDERS AND LED BARS FOR THE INTERACTION FEEDBACK ON SLIDERS
    for(unsigned int indMainSliderRow = 0 ; indMainSliderRow < _MaxSlidersBeatData ; indMainSliderRow++ ) {
        for(unsigned int indMainSliderCol = 0 ; indMainSliderCol < SlidersBeatData[indMainSliderRow].size() ; indMainSliderCol++ ) {
            // scenario-based value changes
            if(tabControllers[_Beat_display]->verticalSliders[indMainSliderRow][indMainSliderCol]) {
                unsigned int indControllerDataRow
                        = tabControllers[_Beat_display]->verticalSliders[indMainSliderRow][indMainSliderCol]->getIndLEDBarDataRow();
                unsigned int indControllerDataCol
                        = tabControllers[_Beat_display]->verticalSliders[indMainSliderRow][indMainSliderCol]->getIndLEDBarDataCol();
                if(tabControllers[_Beat_display]->verticalSliders[indMainSliderRow][indMainSliderCol]->getAssociatedDisplayType() == _Box ) {
                    if( indControllerDataRow < SlidersBeatData.size()
                        && indControllerDataCol < SlidersBeatData[indControllerDataRow].size() ) {
                        tabControllers[_Beat_display]->verticalSliders[indMainSliderRow][indMainSliderCol]->setAssociatedBox(
                            tabDisplays[_Beat_display]->graphsFlashGrid->getBox(indControllerDataRow + 1, indControllerDataCol + 1));
                    }
                }
                else {
                    if( indControllerDataRow < SlidersBeatData.size()
                        && indControllerDataCol < SlidersBeatData[indControllerDataRow].size()
                            && tabDisplays[_Beat_display]->graphicalLEDBarRows[indControllerDataRow]) {
                        tabControllers[_Beat_display]->verticalSliders[indMainSliderRow][indMainSliderCol]->setAssociatedLEDBar(
                            tabDisplays[_Beat_display]->graphicalLEDBarRows[indControllerDataRow]->getLEDBar(indControllerDataCol));
                    }
                }
            }
            // scenario-based pulsed value changes
            if(tabControllers[_Beat_Pulse_display]->verticalSliders[indMainSliderRow][indMainSliderCol]) {
                unsigned int indControllerDataRow
                        = tabControllers[_Beat_Pulse_display]->verticalSliders[indMainSliderRow][indMainSliderCol]->getIndLEDBarDataRow();
                unsigned int indControllerDataCol
                        = tabControllers[_Beat_Pulse_display]->verticalSliders[indMainSliderRow][indMainSliderCol]->getIndLEDBarDataCol();
                if(tabControllers[_Beat_Pulse_display]->verticalSliders[indMainSliderRow][indMainSliderCol]->getAssociatedDisplayType() == _Box ) {
                    if( indControllerDataRow < SlidersBeatData.size()
                        && indControllerDataCol < SlidersBeatData[indControllerDataRow].size() ) {
                        tabControllers[_Beat_Pulse_display]->verticalSliders[indMainSliderRow][indMainSliderCol]->setAssociatedBox(
                            tabDisplays[_Beat_Pulse_display]->graphsFlashGrid->getBox(indControllerDataRow + 1, indControllerDataCol + 1));
                    }
                }
                else {
                    if( indControllerDataRow < SlidersBeatData.size()
                        && indControllerDataCol < SlidersBeatData[indControllerDataRow].size()
                            && tabDisplays[_Beat_Pulse_display]->graphicalLEDBarRows[indControllerDataRow]) {
                        tabControllers[_Beat_Pulse_display]->verticalSliders[indMainSliderRow][indMainSliderCol]->setAssociatedLEDBar(
                            tabDisplays[_Beat_Pulse_display]->graphicalLEDBarRows[indControllerDataRow]->getLEDBar(indControllerDataCol));
                    }
                }
            }
        }
    }


    /////////////////////////////////////////////////////////
    /// Mixer TAB
    /////////////////////////////////////////////////////////
    // CONNECTION BETWEEN BUTTONS AND LABELS FOR THE INTERACTION FEEDBACK ON LABELS
    for(unsigned int indMainButtonRow = 0 ; indMainButtonRow < ButtonsMixerData.size() ; indMainButtonRow++ ) {
        for(unsigned int indMainButtonCol = 0 ; indMainButtonCol < ButtonsMixerData[indMainButtonRow].size() ; indMainButtonCol++ ) {
            // scenario-based value changes
            if(tabControllers[_Mixer_display]->mainButtons[indMainButtonRow][indMainButtonCol]
                    && indMainButtonRow < ButtonsMixerData.size()
                    && indMainButtonCol < ButtonsMixerData[indMainButtonRow].size() ) {
                tabControllers[_Mixer_display]->mainButtons[indMainButtonRow][indMainButtonCol]->setAssociatedSingleLabel(
                    tabDisplays[_Mixer_display]->graphicalLabelRows[indMainButtonRow]->getSingleLabel(indMainButtonCol));
            }
            // there are no pulse feedback on button values -> nothing to do for the pulsed tab
        }
    }
    /////////////////////////////////////////////////////////
    // CONNECTION BETWEEN SLIDERS AND LED BARS FOR THE INTERACTION FEEDBACK ON SLIDERS
    for(unsigned int indMainSliderRow = 0 ; indMainSliderRow < SlidersMixerData.size() ; indMainSliderRow++ ) {
        for(unsigned int indMainSliderCol = 0 ; indMainSliderCol < SlidersMixerData[indMainSliderRow].size() ; indMainSliderCol++ ) {
            // scenario-based value changes
            if(tabControllers[_Mixer_display]->verticalSliders[indMainSliderRow][indMainSliderCol]) {
                unsigned int indControllerDataRow
                        = tabControllers[_Mixer_display]->verticalSliders[indMainSliderRow][indMainSliderCol]->getIndLEDBarDataRow();
                unsigned int indControllerDataCol
                        = tabControllers[_Mixer_display]->verticalSliders[indMainSliderRow][indMainSliderCol]->getIndLEDBarDataCol();
                if( indControllerDataRow < SlidersMixerData.size()
                    && indControllerDataCol < SlidersMixerData[indControllerDataRow].size()
                        && tabDisplays[_Mixer_display]->graphicalLEDBarRows[indControllerDataRow]) {
                    tabControllers[_Mixer_display]->verticalSliders[indMainSliderRow][indMainSliderCol]->setAssociatedLEDBar(
                        tabDisplays[_Mixer_display]->graphicalLEDBarRows[indControllerDataRow]->getLEDBar(indControllerDataCol));
                }
            }
            // scenario-based pulsed value changes
            if(tabControllers[_Mixer_Pulse_display]->verticalSliders[indMainSliderRow][indMainSliderCol]) {
                unsigned int indControllerDataRow
                        = tabControllers[_Mixer_Pulse_display]->verticalSliders[indMainSliderRow][indMainSliderCol]->getIndLEDBarDataRow();
                unsigned int indControllerDataCol
                        = tabControllers[_Mixer_Pulse_display]->verticalSliders[indMainSliderRow][indMainSliderCol]->getIndLEDBarDataCol();
                if( indControllerDataRow < SlidersMixerData.size()
                    && indControllerDataCol < SlidersMixerData[indControllerDataRow].size()
                        && tabDisplays[_Mixer_Pulse_display]->graphicalLEDBarRows[indControllerDataRow]) {
                    tabControllers[_Mixer_Pulse_display]->verticalSliders[indMainSliderRow][indMainSliderCol]->setAssociatedLEDBar(
                        tabDisplays[_Mixer_Pulse_display]->graphicalLEDBarRows[indControllerDataRow]->getLEDBar(indControllerDataCol));
                }
            }
        }
    }


    /////////////////////////////////////////////////////////
    /// FX TAB
    /////////////////////////////////////////////////////////
    // CONNECTION BETWEEN BUTTONS AND LABELS FOR THE INTERACTION FEEDBACK ON LABELS
    for(unsigned int indMainButtonRow = 0 ; indMainButtonRow < ButtonsFXData.size() ; indMainButtonRow++ ) {
        for(unsigned int indMainButtonCol = 0 ; indMainButtonCol < ButtonsFXData[indMainButtonRow].size() ; indMainButtonCol++ ) {
            // scenario-based value changes
            if(tabControllers[_FX_display]->mainButtons[indMainButtonRow][indMainButtonCol]
                    && indMainButtonRow < ButtonsFXData.size()
                    && indMainButtonCol < ButtonsFXData[indMainButtonRow].size() ) {
                tabControllers[_FX_display]->mainButtons[indMainButtonRow][indMainButtonCol]->setAssociatedSingleLabel(
                    tabDisplays[_FX_display]->graphicalLabelRows[indMainButtonRow]->getSingleLabel(indMainButtonCol));
            }
            // there are no pulse feedback on button values -> nothing to do for the pulsed tab
        }
    }
    /////////////////////////////////////////////////////////
    // CONNECTION BETWEEN SLIDERS AND LED BARS FOR THE INTERACTION FEEDBACK ON SLIDERS
    for(unsigned int indMainSliderRow = 0 ; indMainSliderRow < SlidersFXData.size() ; indMainSliderRow++ ) {
        for(unsigned int indMainSliderCol = 0 ; indMainSliderCol < SlidersFXData[indMainSliderRow].size() ; indMainSliderCol++ ) {
            // scenario-based value changes
            if(tabControllers[_FX_display]->verticalSliders[indMainSliderRow][indMainSliderCol]) {
                unsigned int indControllerDataRow
                        = tabControllers[_FX_display]->verticalSliders[indMainSliderRow][indMainSliderCol]->getIndLEDBarDataRow();
                unsigned int indControllerDataCol
                        = tabControllers[_FX_display]->verticalSliders[indMainSliderRow][indMainSliderCol]->getIndLEDBarDataCol();
                if( indControllerDataRow < SlidersFXData.size()
                    && indControllerDataCol < SlidersFXData[indControllerDataRow].size()
                        && tabDisplays[_FX_display]->graphicalLEDBarRows[indControllerDataRow]) {
                    tabControllers[_FX_display]->verticalSliders[indMainSliderRow][indMainSliderCol]->setAssociatedLEDBar(
                        tabDisplays[_FX_display]->graphicalLEDBarRows[indControllerDataRow]->getLEDBar(indControllerDataCol));
                }
            }
            // scenario-based pulsed value changes
            if(tabControllers[_FX_Pulse_display]->verticalSliders[indMainSliderRow][indMainSliderCol]) {
                unsigned int indControllerDataRow
                        = tabControllers[_FX_Pulse_display]->verticalSliders[indMainSliderRow][indMainSliderCol]->getIndLEDBarDataRow();
                unsigned int indControllerDataCol
                        = tabControllers[_FX_Pulse_display]->verticalSliders[indMainSliderRow][indMainSliderCol]->getIndLEDBarDataCol();
                if( indControllerDataRow < SlidersFXData.size()
                    && indControllerDataCol < SlidersFXData[indControllerDataRow].size()
                        && tabDisplays[_FX_Pulse_display]->graphicalLEDBarRows[indControllerDataRow]) {
                    tabControllers[_FX_Pulse_display]->verticalSliders[indMainSliderRow][indMainSliderCol]->setAssociatedLEDBar(
                        tabDisplays[_FX_Pulse_display]->graphicalLEDBarRows[indControllerDataRow]->getLEDBar(indControllerDataCol));
                }
            }
        }
    }

    /////////////////////////////////////////////////////////
    /// Paths TAB
    /////////////////////////////////////////////////////////
    // CONNECTION BETWEEN BUTTONS AND LABELS FOR THE INTERACTION FEEDBACK ON LABELS
    for(unsigned int indMainButtonRow = 0 ; indMainButtonRow < ButtonsPathsData.size() ; indMainButtonRow++ ) {
        for(unsigned int indMainButtonCol = 0 ; indMainButtonCol < ButtonsPathsData[indMainButtonRow].size() ; indMainButtonCol++ ) {
            // scenario-based value changes
            if(tabControllers[_Paths_display]->mainButtons[indMainButtonRow][indMainButtonCol]
                    && indMainButtonRow < ButtonsPathsData.size()
                    && indMainButtonCol < ButtonsPathsData[indMainButtonRow].size() ) {
                tabControllers[_Paths_display]->mainButtons[indMainButtonRow][indMainButtonCol]->setAssociatedSingleLabel(
                    tabDisplays[_Paths_display]->graphicalLabelRows[indMainButtonRow]->getSingleLabel(indMainButtonCol));
            }
            // there are no pulse feedback on button values -> nothing to do for the pulsed tab
        }
    }
    /////////////////////////////////////////////////////////
    // CONNECTION BETWEEN SLIDERS AND LED BARS FOR THE INTERACTION FEEDBACK ON SLIDERS
    for(unsigned int indMainSliderRow = 0 ; indMainSliderRow < SlidersPathsData.size() ; indMainSliderRow++ ) {
        for(unsigned int indMainSliderCol = 0 ; indMainSliderCol < SlidersPathsData[indMainSliderRow].size() ; indMainSliderCol++ ) {
            // scenario-based value changes
            if(tabControllers[_Paths_display]->verticalSliders[indMainSliderRow][indMainSliderCol]) {
                unsigned int indControllerDataRow
                        = tabControllers[_Paths_display]->verticalSliders[indMainSliderRow][indMainSliderCol]->getIndLEDBarDataRow();
                unsigned int indControllerDataCol
                        = tabControllers[_Paths_display]->verticalSliders[indMainSliderRow][indMainSliderCol]->getIndLEDBarDataCol();
                if( indControllerDataRow < SlidersPathsData.size()
                    && indControllerDataCol < SlidersPathsData[indControllerDataRow].size()
                        && tabDisplays[_Paths_display]->graphicalLEDBarRows[indControllerDataRow]) {
                    tabControllers[_Paths_display]->verticalSliders[indMainSliderRow][indMainSliderCol]->setAssociatedLEDBar(
                        tabDisplays[_Paths_display]->graphicalLEDBarRows[indControllerDataRow]->getLEDBar(indControllerDataCol));
                }
            }
            // scenario-based pulsed value changes
            if(tabControllers[_Paths_Pulse_display]->verticalSliders[indMainSliderRow][indMainSliderCol]) {
                unsigned int indControllerDataRow
                        = tabControllers[_Paths_Pulse_display]->verticalSliders[indMainSliderRow][indMainSliderCol]->getIndLEDBarDataRow();
                unsigned int indControllerDataCol
                        = tabControllers[_Paths_Pulse_display]->verticalSliders[indMainSliderRow][indMainSliderCol]->getIndLEDBarDataCol();
                if( indControllerDataRow < SlidersPathsData.size()
                    && indControllerDataCol < SlidersPathsData[indControllerDataRow].size()
                        && tabDisplays[_Paths_Pulse_display]->graphicalLEDBarRows[indControllerDataRow]) {
                    tabControllers[_Paths_Pulse_display]->verticalSliders[indMainSliderRow][indMainSliderCol]->setAssociatedLEDBar(
                        tabDisplays[_Paths_Pulse_display]->graphicalLEDBarRows[indControllerDataRow]->getLEDBar(indControllerDataCol));
                }
            }
        }
    }

#ifdef TVW
    /////////////////////////////////////////////////////////
    /// TVW TAB
    /////////////////////////////////////////////////////////
    // CONNECTION BETWEEN BUTTONS AND LABELS FOR THE INTERACTION FEEDBACK ON LABELS
    for(unsigned int indMainButtonRow = 0 ; indMainButtonRow < ButtonsTVWData.size() ; indMainButtonRow++ ) {
        for(unsigned int indMainButtonCol = 0 ; indMainButtonCol < ButtonsTVWData[indMainButtonRow].size() ; indMainButtonCol++ ) {
            // scenario-based value changes
            if(tabControllers[_TVW_display]->mainButtons[indMainButtonRow][indMainButtonCol]
                    && indMainButtonRow < ButtonsTVWData.size()
                    && indMainButtonCol < ButtonsTVWData[indMainButtonRow].size() ) {
                tabControllers[_TVW_display]->mainButtons[indMainButtonRow][indMainButtonCol]->setAssociatedSingleLabel(
                    tabDisplays[_TVW_display]->graphicalLabelRows[indMainButtonRow]->getSingleLabel(indMainButtonCol));
            }
            // there are no pulse feedback on button values -> nothing to do for the pulsed tab
        }
    }
    /////////////////////////////////////////////////////////
    // CONNECTION BETWEEN SLIDERS AND LED BARS FOR THE INTERACTION FEEDBACK ON SLIDERS
    for(unsigned int indMainSliderRow = 0 ; indMainSliderRow < SlidersTVWData.size() ; indMainSliderRow++ ) {
        for(unsigned int indMainSliderCol = 0 ; indMainSliderCol < SlidersTVWData[indMainSliderRow].size() ; indMainSliderCol++ ) {
            // scenario-based value changes
            if(tabControllers[_TVW_display]->verticalSliders[indMainSliderRow][indMainSliderCol]) {
                unsigned int indControllerDataRow
                        = tabControllers[_TVW_display]->verticalSliders[indMainSliderRow][indMainSliderCol]->getIndLEDBarDataRow();
                unsigned int indControllerDataCol
                        = tabControllers[_TVW_display]->verticalSliders[indMainSliderRow][indMainSliderCol]->getIndLEDBarDataCol();
                if( indControllerDataRow < SlidersTVWData.size()
                    && indControllerDataCol < SlidersTVWData[indControllerDataRow].size()
                        && tabDisplays[_TVW_display]->graphicalLEDBarRows[indControllerDataRow]) {
                    tabControllers[_TVW_display]->verticalSliders[indMainSliderRow][indMainSliderCol]->setAssociatedLEDBar(
                        tabDisplays[_TVW_display]->graphicalLEDBarRows[indControllerDataRow]->getLEDBar(indControllerDataCol));
                }
            }
            // scenario-based pulsed value changes
            if(tabControllers[_TVW_Pulse_display]->verticalSliders[indMainSliderRow][indMainSliderCol]) {
                unsigned int indControllerDataRow
                        = tabControllers[_TVW_Pulse_display]->verticalSliders[indMainSliderRow][indMainSliderCol]->getIndLEDBarDataRow();
                unsigned int indControllerDataCol
                        = tabControllers[_TVW_Pulse_display]->verticalSliders[indMainSliderRow][indMainSliderCol]->getIndLEDBarDataCol();
                if( indControllerDataRow < SlidersTVWData.size()
                    && indControllerDataCol < SlidersTVWData[indControllerDataRow].size()
                        && tabDisplays[_TVW_Pulse_display]->graphicalLEDBarRows[indControllerDataRow]) {
                    tabControllers[_TVW_Pulse_display]->verticalSliders[indMainSliderRow][indMainSliderCol]->setAssociatedLEDBar(
                        tabDisplays[_TVW_Pulse_display]->graphicalLEDBarRows[indControllerDataRow]->getLEDBar(indControllerDataCol));
                }
            }
        }
    }
#endif

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
