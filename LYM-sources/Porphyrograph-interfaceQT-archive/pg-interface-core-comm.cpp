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
#include <iostream>
#include <cstdlib>
#include <vector>

#include "RtMidi.h"

#include "pg-interface-core-comm.h"
#include "pg-interface-core-comm.h"
#include "pg-interface-core-window.h"
#include "pg-interface-core-udp.h"
#include "pg-interface-core-display.h"
#include "pg-interface-core-control.h"
#include "pg-interface-core-main.h"

#include <QApplication>
#include <QThread>
#include <QtWidgets>
#include <QtNetwork>
#include <QTime>
#include <QSlider>
#include <QPushButton>

/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
// GLOBAL VARIABLES
bool ignoreAllMissingUSBs = false;

// hash table for MIDI message lookup
QMultiHash<QString, int> hashButtonBeatMIDIIn;
QMultiHash<QString, int> hashSliderBeatMIDIIn;
QMultiHash<QString, int> hashButtonBeatPulseMIDIIn;
QMultiHash<QString, int> hashSliderBeatPulseMIDIIn;
QMultiHash<QString, int> hashButtonMixerMIDIIn;
QMultiHash<QString, int> hashSliderMixerMIDIIn;
QMultiHash<QString, int> hashButtonMixerPulseMIDIIn;
QMultiHash<QString, int> hashSliderMixerPulseMIDIIn;
QMultiHash<QString, int> hashButtonFXMIDIIn;
QMultiHash<QString, int> hashSliderFXMIDIIn;
QMultiHash<QString, int> hashButtonFXPulseMIDIIn;
QMultiHash<QString, int> hashSliderFXPulseMIDIIn;
QMultiHash<QString, int> hashButtonPathsMIDIIn;
QMultiHash<QString, int> hashSliderPathsMIDIIn;
QMultiHash<QString, int> hashButtonPathsPulseMIDIIn;
QMultiHash<QString, int> hashSliderPathsPulseMIDIIn;
#if defined(TVW) || defined(demo)
QMultiHash<QString, int> hashButtonMinimalIFMIDIIn;
QMultiHash<QString, int> hashSliderMinimalIFMIDIIn;
QMultiHash<QString, int> hashButtonMinimalIFPulseMIDIIn;
QMultiHash<QString, int> hashSlideMinimalIFPulseMIDIIn;
#endif


////////////////////////////////////////////////////////////
// MIDI COMMUNICATION
////////////////////////////////////////////////////////////

void MidiInOut::receiveMidiIn(void) {
   // MIDI IN MESSAGES
   std::vector<unsigned char> messageIn;
   double stamp;
   int nBytes;
   bool traceConsole = false;
   for(unsigned int indMidiin = 0 ; indMidiin < pg_window->midiInOut->midiReceivers.size(); indMidiin++) {
       if(pg_window->midiInOut->midiReceivers[indMidiin]) {
           std::vector<int> vec_dataType;
           std::vector<int> vec_indChannel;
           std::vector<int> vec_controlIndex;
           std::vector<int> vec_value;
           while(true) {
               if( pg_window->lastPressedSlider
                   && pg_window->lastTimePressedSlider.secsTo(QTime::currentTime()) >= 1 ) {
                   emit(pg_window->lastPressedSlider->sliderReleased());
                   pg_window->lastPressedSlider = NULL;
               }

               stamp = pg_window->midiInOut->midiReceivers[indMidiin]->getMessage( &messageIn );
               nBytes = messageIn.size();
               if ( nBytes == 0 ) {
                   break;
               }
               if(traceConsole)
                    qDebug() << "Receive midi stamp " << stamp << " size " << nBytes; // << std::endl;
               int dataType = 0;
               int indChannel = 0;
               int controlIndex = 0;
               int value = 0;
               for (int i=0; i<nBytes; i++ ){
                   switch (i) {
                   case 0:
                       dataType = (int)messageIn[i]/16;
                       indChannel = (int)messageIn[i] - dataType * 16 + 1;
                       break;
                   case 1:
                       controlIndex = (int)messageIn[i];
                       break;
                   case 2:
                       value = (int)messageIn[i];
                       break;
                   default:
                       if(traceConsole)
                           qDebug() << ", Additional data " << i << " " << (int)messageIn[i]; // << std::endl;
                       break;
                   }
               }
               bool found = false;
               for(unsigned int indVec = 0 ;indVec < vec_dataType.size() ; indVec++) {
                   if(vec_dataType[indVec] == dataType
                           && vec_indChannel[indVec] == indChannel
                           && vec_controlIndex[indVec] == controlIndex) {
                       vec_value[indVec] = value;
                       found = true;
                   }
               }
               if(!found) {
                   vec_dataType.push_back( dataType );
                   vec_indChannel.push_back( indChannel );
                   vec_controlIndex.push_back( controlIndex );
                   vec_value.push_back( value );
               }
           }

           DisplayType targetDisplay = _Config_display;
           // Beat or Mixer or FX w or wo pulse
           if(indMidiin == 0 && pg_window->getCurrentTabWindow() >= _Beat_display
                   && pg_window->getCurrentTabWindow() <= _FX_Pulse_display) {
               targetDisplay = DisplayType(pg_window->getCurrentTabWindow());
           }
#if defined(TVW) || defined(demo)
           else if(indMidiin == 0 && pg_window->getCurrentTabWindow() >= _MinimalIF_display
                   && pg_window->getCurrentTabWindow() <= _MinimalIF_Pulse_display) {
               targetDisplay = DisplayType(pg_window->getCurrentTabWindow());
           }
#endif
           // non Paths control while Paths active (mainly TAB, PULSE, PATHS)
           // should not be made for a proper window management (or similar buttons should be added to Paths)
           else if(indMidiin == 0 && pg_window->getCurrentTabWindow() >= _Paths_display
                   && pg_window->getCurrentTabWindow() <= _Paths_Pulse_display) {
               targetDisplay = _Beat_display;
           }
           // Paths controller
           else if(indMidiin == 1) {
               if(pg_window->getCurrentTabWindow() >= _Paths_display
                       && pg_window->getCurrentTabWindow() <= _Paths_Pulse_display) {
                   targetDisplay = DisplayType(pg_window->getCurrentTabWindow());
               }
               else {
                   targetDisplay = _Paths_display;
               }
           }
//           else {
//               qDebug() << "Target display not found midiIn " << indMidiin << " display " << pg_window->getCurrentTabWindow(); // << std::endl;
//           }

           for(unsigned int indVec = 0 ;indVec < vec_dataType.size() ; indVec++) {
               // if(traceConsole)
//                   qDebug() <<  "Midi message for " << targetDisplay << "(" << indMidiin << ") type/index/channel "
//                             << vec_dataType[indVec] << " " << vec_controlIndex[indVec]
//                                << " " << vec_indChannel[indVec] << " val " << vec_value[indVec];
               pg_window->propagateMidiEventsToControllers( targetDisplay,
                                                            vec_dataType[indVec],
                                                            vec_controlIndex[indVec],
                                                            vec_indChannel[indVec],
                                                            vec_value[indVec] );
           }
       }
   }
}

// registering the MIDI events to the controllers for quick access through hash tables
void MainWindow::registerMidiEventsToControllers( void ) {
    /////////////////////////////////////////////////////////////////////////////////////
    // GRAPHS
    // MAIN SLIDERS
    for(unsigned int indSliderRow = 0 ; indSliderRow < SlidersBeatData.size() ; indSliderRow++ ) {
        for(unsigned int indCol = 0 ; indCol < SlidersBeatData[indSliderRow].size() ; indCol++ ) {
            int dataType = SlidersBeatData[indSliderRow][indCol].get_typeMidiIn();
            int controlIndex = SlidersBeatData[indSliderRow][indCol].get_controlMidiIn();
            int indChannel = SlidersBeatData[indSliderRow][indCol].get_channelMidiIn();
            if(dataType == 0 && controlIndex == 0 && indChannel == 0) {
                continue;
            }
            QString key = QString( "%1 %2 %3").arg(dataType).arg(controlIndex).arg(indChannel);
            if( tabControllers[_Beat_display]->verticalSliders[indSliderRow][indCol] ) {
                if(hashSliderBeatMIDIIn.contains(key)) {
                    qDebug() << "Duplicate Beat slider midi In " << key
                             << " row " << indSliderRow << " col " << indCol; // << std::endl;
                    exit(0);
                }
                hashSliderBeatMIDIIn.insert(key,indCol);
                hashSliderBeatMIDIIn.insert(key,indSliderRow);
            }
            if( tabControllers[_Beat_Pulse_display]->verticalSliders[indSliderRow][indCol] ) {
                if(hashSliderBeatPulseMIDIIn.contains(key)) {
                    qDebug() << "Duplicate Beat Pulse slider midi In " << key
                             << " row " << indSliderRow << " col " << indCol; // << std::endl;
                    exit(0);
                }
                hashSliderBeatPulseMIDIIn.insert(key,indCol);
                hashSliderBeatPulseMIDIIn.insert(key,indSliderRow);
            }
        }
    }
    // MAIN BUTTONS
    for(unsigned int indButtonRow = 0 ; indButtonRow < ButtonsBeatData.size() ; indButtonRow++ ) {
        for(unsigned int indCol = 0 ; indCol < ButtonsBeatData[indButtonRow].size() ; indCol++ ) {
            int dataType = ButtonsBeatData[indButtonRow][indCol].get_typeMidiIn();
            int controlIndex = ButtonsBeatData[indButtonRow][indCol].get_controlMidiIn();
            int indChannel = ButtonsBeatData[indButtonRow][indCol].get_channelMidiIn();
            if(dataType == 0 && controlIndex == 0 && indChannel == 0) {
                continue;
            }
            QString key = QString( "%1 %2 %3").arg(dataType).arg(controlIndex).arg(indChannel);
            if( tabControllers[_Beat_display]->mainButtons[indButtonRow][indCol] ) {
                if(hashButtonBeatMIDIIn.contains(key)) {
                    qDebug() << "Duplicate Beat button midi In " << key
                             << " row " << indButtonRow << " col " << indCol; // << std::endl;
                    exit(0);
                }
                hashButtonBeatMIDIIn.insert(key,indCol);
                hashButtonBeatMIDIIn.insert(key,indButtonRow);
            }
            if( tabControllers[_Beat_Pulse_display]->mainButtons[indButtonRow][indCol] ) {
                if(hashButtonBeatPulseMIDIIn.contains(key)) {
                    qDebug() << "Duplicate Beat Pulse button midi In " << key
                             << " row " << indButtonRow << " col " << indCol; // << std::endl;
                    exit(0);
                }
                hashButtonBeatPulseMIDIIn.insert(key,indCol);
                hashButtonBeatPulseMIDIIn.insert(key,indButtonRow);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////
    // Mixer
    // MAIN SLIDERS
    for(unsigned int indSliderRow = 0 ; indSliderRow < SlidersMixerData.size() ; indSliderRow++ ) {
        for(unsigned int indCol = 0 ; indCol < SlidersMixerData[indSliderRow].size() ; indCol++ ) {
            int dataType = SlidersMixerData[indSliderRow][indCol].get_typeMidiIn();
            int controlIndex = SlidersMixerData[indSliderRow][indCol].get_controlMidiIn();
            int indChannel = SlidersMixerData[indSliderRow][indCol].get_channelMidiIn();
            if(dataType == 0 && controlIndex == 0 && indChannel == 0) {
                continue;
            }
            QString key = QString( "%1 %2 %3").arg(dataType).arg(controlIndex).arg(indChannel);
            if( tabControllers[_Mixer_display]->verticalSliders[indSliderRow][indCol] ) {
                if(hashSliderMixerMIDIIn.contains(key)) {
                    qDebug() << "Duplicate Mixer slider midi In " << key
                             << " row " << indSliderRow << " col " << indCol; // << std::endl;
                    exit(0);
                }
                hashSliderMixerMIDIIn.insert(key,indCol);
                hashSliderMixerMIDIIn.insert(key,indSliderRow);
            }
            if( tabControllers[_Mixer_Pulse_display]->verticalSliders[indSliderRow][indCol] ) {
                if(hashSliderMixerPulseMIDIIn.contains(key)) {
                    qDebug() << "Duplicate Mixer Pulse slider midi In " << key
                             << " row " << indSliderRow << " col " << indCol; // << std::endl;
                    exit(0);
                }
                hashSliderMixerPulseMIDIIn.insert(key,indCol);
                hashSliderMixerPulseMIDIIn.insert(key,indSliderRow);
            }
        }
    }
    // MAIN BUTTONS
    for(unsigned int indButtonRow = 0 ; indButtonRow < ButtonsMixerData.size() ; indButtonRow++ ) {
        for(unsigned int indCol = 0 ; indCol < ButtonsMixerData[indButtonRow].size() ; indCol++ ) {
            int dataType = ButtonsMixerData[indButtonRow][indCol].get_typeMidiIn();
            int controlIndex = ButtonsMixerData[indButtonRow][indCol].get_controlMidiIn();
            int indChannel = ButtonsMixerData[indButtonRow][indCol].get_channelMidiIn();
            if(dataType == 0 && controlIndex == 0 && indChannel == 0) {
                continue;
            }
            QString key = QString( "%1 %2 %3").arg(dataType).arg(controlIndex).arg(indChannel);
            if( tabControllers[_Mixer_display]->mainButtons[indButtonRow][indCol] ) {
                if(hashButtonMixerMIDIIn.contains(key)) {
                    qDebug() << "Duplicate Mixer button midi In " << key
                             << " row " << indButtonRow << " col " << indCol; // << std::endl;
                    exit(0);
                }
                hashButtonMixerMIDIIn.insert(key,indCol);
                hashButtonMixerMIDIIn.insert(key,indButtonRow);
            }
            if( tabControllers[_Mixer_Pulse_display]->mainButtons[indButtonRow][indCol] ) {
                if(hashButtonMixerPulseMIDIIn.contains(key)) {
                    qDebug() << "Duplicate Mixer Pulse button midi In " << key
                             << " row " << indButtonRow << " col " << indCol; // << std::endl;
                    exit(0);
                }
                hashButtonMixerPulseMIDIIn.insert(key,indCol);
                hashButtonMixerPulseMIDIIn.insert(key,indButtonRow);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////
    // FX
    // MAIN SLIDERS
    for(unsigned int indSliderRow = 0 ; indSliderRow < SlidersFXData.size() ; indSliderRow++ ) {
        for(unsigned int indCol = 0 ; indCol < SlidersFXData[indSliderRow].size() ; indCol++ ) {
            int dataType = SlidersFXData[indSliderRow][indCol].get_typeMidiIn();
            int controlIndex = SlidersFXData[indSliderRow][indCol].get_controlMidiIn();
            int indChannel = SlidersFXData[indSliderRow][indCol].get_channelMidiIn();
            if(dataType == 0 && controlIndex == 0 && indChannel == 0) {
                continue;
            }
            QString key = QString( "%1 %2 %3").arg(dataType).arg(controlIndex).arg(indChannel);
            if( tabControllers[_FX_display]->verticalSliders[indSliderRow][indCol] ) {
                if(hashSliderFXMIDIIn.contains(key)) {
                    qDebug() << "Duplicate FX slider midi In " << key
                             << " row " << indSliderRow << " col " << indCol; // << std::endl;
                    exit(0);
                }
                hashSliderFXMIDIIn.insert(key,indCol);
                hashSliderFXMIDIIn.insert(key,indSliderRow);
            }
            if( tabControllers[_FX_Pulse_display]->verticalSliders[indSliderRow][indCol] ) {
                if(hashSliderFXPulseMIDIIn.contains(key)) {
                    qDebug() << "Duplicate FX Pulse slider midi In " << key
                             << " row " << indSliderRow << " col " << indCol; // << std::endl;
                    exit(0);
                }
                hashSliderFXPulseMIDIIn.insert(key,indCol);
                hashSliderFXPulseMIDIIn.insert(key,indSliderRow);
            }
        }
    }
    // MAIN BUTTONS
    for(unsigned int indButtonRow = 0 ; indButtonRow < ButtonsFXData.size() ; indButtonRow++ ) {
        for(unsigned int indCol = 0 ; indCol < ButtonsFXData[indButtonRow].size() ; indCol++ ) {
            int dataType = ButtonsFXData[indButtonRow][indCol].get_typeMidiIn();
            int controlIndex = ButtonsFXData[indButtonRow][indCol].get_controlMidiIn();
            int indChannel = ButtonsFXData[indButtonRow][indCol].get_channelMidiIn();
            if(dataType == 0 && controlIndex == 0 && indChannel == 0) {
                continue;
            }
            QString key = QString( "%1 %2 %3").arg(dataType).arg(controlIndex).arg(indChannel);
            if( tabControllers[_FX_display]->mainButtons[indButtonRow][indCol] ) {
                if(hashButtonFXMIDIIn.contains(key)) {
                    qDebug() << "Duplicate FX button midi In " << key
                             << " row " << indButtonRow << " col " << indCol; // << std::endl;
                    exit(0);
                }
                hashButtonFXMIDIIn.insert(key,indCol);
                hashButtonFXMIDIIn.insert(key,indButtonRow);
            }
            if( tabControllers[_FX_Pulse_display]->mainButtons[indButtonRow][indCol] ) {
                if(hashButtonFXPulseMIDIIn.contains(key)) {
                    qDebug() << "Duplicate FX Pulse button midi In " << key
                             << " row " << indButtonRow << " col " << indCol; // << std::endl;
                    exit(0);
                }
                hashButtonFXPulseMIDIIn.insert(key,indCol);
                hashButtonFXPulseMIDIIn.insert(key,indButtonRow);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////
    // PATHS
    // MAIN SLIDERS
    for(unsigned int indSliderRow = 0 ; indSliderRow < SlidersPathsData.size() ; indSliderRow++ ) {
        for(unsigned int indCol = 0 ; indCol < SlidersPathsData[indSliderRow].size() ; indCol++ ) {
            int dataType = SlidersPathsData[indSliderRow][indCol].get_typeMidiIn();
            int controlIndex = SlidersPathsData[indSliderRow][indCol].get_controlMidiIn();
            int indChannel = SlidersPathsData[indSliderRow][indCol].get_channelMidiIn();
            if(dataType == 0 && controlIndex == 0 && indChannel == 0) {
                continue;
            }
            QString key = QString( "%1 %2 %3").arg(dataType).arg(controlIndex).arg(indChannel);
            if( tabControllers[_Paths_display]->verticalSliders[indSliderRow][indCol] ) {
                if(hashSliderPathsMIDIIn.contains(key)) {
                    qDebug() << "Duplicate Paths slider midi In " << key
                             << " row " << indSliderRow << " col " << indCol; // << std::endl;
                    exit(0);
                }
                hashSliderPathsMIDIIn.insert(key,indCol);
                hashSliderPathsMIDIIn.insert(key,indSliderRow);
            }
            if( tabControllers[_Paths_Pulse_display]->verticalSliders[indSliderRow][indCol] ) {
                if(hashSliderPathsPulseMIDIIn.contains(key)) {
                    qDebug() << "Duplicate Paths Pulse slider midi In " << key
                             << " row " << indSliderRow << " col " << indCol; // << std::endl;
                    exit(0);
                }
                hashSliderPathsPulseMIDIIn.insert(key,indCol);
                hashSliderPathsPulseMIDIIn.insert(key,indSliderRow);
            }
        }
    }
    // MAIN BUTTONS
    for(unsigned int indButtonRow = 0 ; indButtonRow < ButtonsPathsData.size() ; indButtonRow++ ) {
        for(unsigned int indCol = 0 ; indCol < ButtonsPathsData[indButtonRow].size() ; indCol++ ) {
            int dataType = ButtonsPathsData[indButtonRow][indCol].get_typeMidiIn();
            int controlIndex = ButtonsPathsData[indButtonRow][indCol].get_controlMidiIn();
            int indChannel = ButtonsPathsData[indButtonRow][indCol].get_channelMidiIn();
            if(dataType == 0 && controlIndex == 0 && indChannel == 0) {
                continue;
            }
            QString key = QString( "%1 %2 %3").arg(dataType).arg(controlIndex).arg(indChannel);
            if( tabControllers[_Paths_display]->mainButtons[indButtonRow][indCol] ) {
                if(hashButtonPathsMIDIIn.contains(key)) {
                    qDebug() << "Duplicate Paths button midi In " << key
                             << " row " << indButtonRow << " col " << indCol; // << std::endl;
                    exit(0);
                }
                hashButtonPathsMIDIIn.insert(key,indCol);
                hashButtonPathsMIDIIn.insert(key,indButtonRow);
            }
            if( tabControllers[_Paths_Pulse_display]->mainButtons[indButtonRow][indCol] ) {
                if(hashButtonPathsPulseMIDIIn.contains(key)) {
                    qDebug() << "Duplicate Paths Pulse button midi In " << key
                             << " row " << indButtonRow << " col " << indCol; // << std::endl;
                    exit(0);
                }
                hashButtonPathsPulseMIDIIn.insert(key,indCol);
                hashButtonPathsPulseMIDIIn.insert(key,indButtonRow);
            }
        }
    }

#if defined(TVW) || defined(demo)
    /////////////////////////////////////////////////////////////////////////////////////
    // MinimalIF
    // MAIN SLIDERS
    for(unsigned int indSliderRow = 0 ; indSliderRow < SlidersMinimalIFData.size() ; indSliderRow++ ) {
        for(unsigned int indCol = 0 ; indCol < SlidersMinimalIFData[indSliderRow].size() ; indCol++ ) {
            int dataType = SlidersMinimalIFData[indSliderRow][indCol].get_typeMidiIn();
            int controlIndex = SlidersMinimalIFData[indSliderRow][indCol].get_controlMidiIn();
            int indChannel = SlidersMinimalIFData[indSliderRow][indCol].get_channelMidiIn();
            if(dataType == 0 && controlIndex == 0 && indChannel == 0) {
                continue;
            }
            QString key = QString( "%1 %2 %3").arg(dataType).arg(controlIndex).arg(indChannel);
            if( tabControllers[_MinimalIF_display]->verticalSliders[indSliderRow][indCol] ) {
                if(hashSliderMinimalIFMIDIIn.contains(key)) {
                    qDebug() << "Duplicate MinimalIF slider midi In " << key
                             << " row " << indSliderRow << " col " << indCol; // << std::endl;
                    exit(0);
                }
                hashSliderMinimalIFMIDIIn.insert(key,indCol);
                hashSliderMinimalIFMIDIIn.insert(key,indSliderRow);
            }
            if( tabControllers[_MinimalIF_Pulse_display]->verticalSliders[indSliderRow][indCol] ) {
                if(hashSlideMinimalIFPulseMIDIIn.contains(key)) {
                    qDebug() << "Duplicate MinimalIF Pulse slider midi In " << key
                             << " row " << indSliderRow << " col " << indCol; // << std::endl;
                    exit(0);
                }
                hashSlideMinimalIFPulseMIDIIn.insert(key,indCol);
                hashSlideMinimalIFPulseMIDIIn.insert(key,indSliderRow);
            }
        }
    }
    // MAIN BUTTONS
    for(unsigned int indButtonRow = 0 ; indButtonRow < ButtonsMinimalIFData.size() ; indButtonRow++ ) {
        for(unsigned int indCol = 0 ; indCol < ButtonsMinimalIFData[indButtonRow].size() ; indCol++ ) {
            int dataType = ButtonsMinimalIFData[indButtonRow][indCol].get_typeMidiIn();
            int controlIndex = ButtonsMinimalIFData[indButtonRow][indCol].get_controlMidiIn();
            int indChannel = ButtonsMinimalIFData[indButtonRow][indCol].get_channelMidiIn();
            if(dataType == 0 && controlIndex == 0 && indChannel == 0) {
                continue;
            }
            QString key = QString( "%1 %2 %3").arg(dataType).arg(controlIndex).arg(indChannel);
            if( tabControllers[_MinimalIF_display]->mainButtons[indButtonRow][indCol] ) {
                if(hashButtonMinimalIFMIDIIn.contains(key)) {
                    qDebug() << "Duplicate MinimalIF button midi In " << key
                             << " row " << indButtonRow << " col " << indCol; // << std::endl;
                    exit(0);
                }
                hashButtonMinimalIFMIDIIn.insert(key,indCol);
                hashButtonMinimalIFMIDIIn.insert(key,indButtonRow);
            }
            if( tabControllers[_MinimalIF_Pulse_display]->mainButtons[indButtonRow][indCol] ) {
                if(hashButtonMinimalIFPulseMIDIIn.contains(key)) {
                    qDebug() << "Duplicate MinimalIF Pulse button midi In " << key
                             << " row " << indButtonRow << " col " << indCol; // << std::endl;
                    exit(0);
                }
                hashButtonMinimalIFPulseMIDIIn.insert(key,indCol);
                hashButtonMinimalIFPulseMIDIIn.insert(key,indButtonRow);
            }
        }
    }
#endif
}

// shipping MIDI events to controllers
void MainWindow::propagateMidiEventsToControllers( DisplayType targetDisplay,
                                                   int dataType,
                                                   int controlIndex,
                                                   int indChannel,
                                                   int value ) {
    QString key = QString( "%1 %2 %3").arg(dataType).arg(controlIndex).arg(indChannel);
    switch( targetDisplay ) {
    case _Beat_display:
        if(hashSliderBeatMIDIIn.contains(key)) {
            QList<int> values = hashSliderBeatMIDIIn.values(key);
            if (values.size() % 2 == 0 && values.size() / 2 >= 1) {
                for(int ind = 0 ; ind < values.size() ; ind += 2 ) {
                    int indSliderRow = values.at(ind+0);
                    int indCol = values.at(ind+1);
                    if(tabControllers[targetDisplay]->verticalSliders[indSliderRow][indCol]) {
                        tabControllers[targetDisplay]->verticalSliders[indSliderRow][indCol]->setIntValueFromMIDI(value);
                    }
                }
            }
        }
        if(value == 127 && hashButtonBeatMIDIIn.contains(key)) {
            QList<int> values = hashButtonBeatMIDIIn.values(key);
            if (values.size() % 2 == 0 && values.size() / 2 >= 1) {
                for(int ind = 0 ; ind < values.size() ; ind += 2 ) {
                    int indButtonRow = values.at(ind+0);
                    int indCol = values.at(ind+1);
                    if(tabControllers[targetDisplay]->mainButtons[indButtonRow][indCol]) {
                        tabControllers[targetDisplay]->mainButtons[indButtonRow][indCol]->animateClick();
                    }
                }
            }
        }
        break;
    case _Beat_Pulse_display:
        if(hashSliderBeatPulseMIDIIn.contains(key)) {
            QList<int> values = hashSliderBeatPulseMIDIIn.values(key);
            if (values.size() % 2 == 0 && values.size() / 2 >= 1) {
                for(int ind = 0 ; ind < values.size() ; ind += 2 ) {
                    int indSliderRow = values.at(ind+0);
                    int indCol = values.at(ind+1);
                    if(tabControllers[targetDisplay]->verticalSliders[indSliderRow][indCol]) {
                        tabControllers[targetDisplay]->verticalSliders[indSliderRow][indCol]->setIntValueFromMIDI(value);
                    }
                }
            }
        }
        if(value == 127 && hashButtonBeatPulseMIDIIn.contains(key)) {
            QList<int> values = hashButtonBeatPulseMIDIIn.values(key);
            if (values.size() % 2 == 0 && values.size() / 2 >= 1) {
                for(int ind = 0 ; ind < values.size() ; ind += 2 ) {
                    int indButtonRow = values.at(ind+0);
                    int indCol = values.at(ind+1);
                    if(tabControllers[targetDisplay]->mainButtons[indButtonRow][indCol]) {
                        tabControllers[targetDisplay]->mainButtons[indButtonRow][indCol]->animateClick();
                    }
                }
            }
        }
        break;
    case _Mixer_display:
        if(hashSliderMixerMIDIIn.contains(key)) {
            QList<int> values = hashSliderMixerMIDIIn.values(key);
            if (values.size() % 2 == 0 && values.size() / 2 >= 1) {
                for(int ind = 0 ; ind < values.size() ; ind += 2 ) {
                    int indSliderRow = values.at(ind+0);
                    int indCol = values.at(ind+1);
                    if(tabControllers[targetDisplay]->verticalSliders[indSliderRow][indCol]) {
                        tabControllers[targetDisplay]->verticalSliders[indSliderRow][indCol]->setIntValueFromMIDI(value);
                    }
                }
            }
        }
        if(value == 127 && hashButtonMixerMIDIIn.contains(key)) {
            QList<int> values = hashButtonMixerMIDIIn.values(key);
            if (values.size() % 2 == 0 && values.size() / 2 >= 1) {
                for(int ind = 0 ; ind < values.size() ; ind += 2 ) {
                    int indButtonRow = values.at(ind+0);
                    int indCol = values.at(ind+1);
                    if(tabControllers[targetDisplay]->mainButtons[indButtonRow][indCol]) {
                        tabControllers[targetDisplay]->mainButtons[indButtonRow][indCol]->animateClick();
                    }
                }
            }
        }
        break;
    case _Mixer_Pulse_display:
        if(hashSliderMixerPulseMIDIIn.contains(key)) {
            QList<int> values = hashSliderMixerPulseMIDIIn.values(key);
            if (values.size() % 2 == 0 && values.size() / 2 >= 1) {
                for(int ind = 0 ; ind < values.size() ; ind += 2 ) {
                    int indSliderRow = values.at(ind+0);
                    int indCol = values.at(ind+1);
                    if(tabControllers[targetDisplay]->verticalSliders[indSliderRow][indCol]) {
                        tabControllers[targetDisplay]->verticalSliders[indSliderRow][indCol]->setIntValueFromMIDI(value);
                    }
                }
            }
        }
        if(value == 127 && hashButtonMixerPulseMIDIIn.contains(key)) {
            QList<int> values = hashButtonMixerPulseMIDIIn.values(key);
            if (values.size() % 2 == 0 && values.size() / 2 >= 1) {
                for(int ind = 0 ; ind < values.size() ; ind += 2 ) {
                    int indButtonRow = values.at(ind+0);
                    int indCol = values.at(ind+1);
                    if(tabControllers[targetDisplay]->mainButtons[indButtonRow][indCol]) {
                        tabControllers[targetDisplay]->mainButtons[indButtonRow][indCol]->animateClick();
                    }
                }
            }
        }
        break;
    case _FX_display:
        if(hashSliderFXMIDIIn.contains(key)) {
            QList<int> values = hashSliderFXMIDIIn.values(key);
            if (values.size() % 2 == 0 && values.size() / 2 >= 1) {
                for(int ind = 0 ; ind < values.size() ; ind += 2 ) {
                    int indSliderRow = values.at(ind+0);
                    int indCol = values.at(ind+1);
                    if(tabControllers[targetDisplay]->verticalSliders[indSliderRow][indCol]) {
                        tabControllers[targetDisplay]->verticalSliders[indSliderRow][indCol]->setIntValueFromMIDI(value);
                    }
                }
            }
        }
        if(value == 127 && hashButtonFXMIDIIn.contains(key)) {
            QList<int> values = hashButtonFXMIDIIn.values(key);
            if (values.size() % 2 == 0 && values.size() / 2 >= 1) {
                for(int ind = 0 ; ind < values.size() ; ind += 2 ) {
                    int indButtonRow = values.at(ind+0);
                    int indCol = values.at(ind+1);
                    if(tabControllers[targetDisplay]->mainButtons[indButtonRow][indCol]) {
                        tabControllers[targetDisplay]->mainButtons[indButtonRow][indCol]->animateClick();
                    }
                }
            }
        }
        break;
    case _FX_Pulse_display:
        if(hashSliderFXPulseMIDIIn.contains(key)) {
            QList<int> values = hashSliderFXPulseMIDIIn.values(key);
            if (values.size() % 2 == 0 && values.size() / 2 >= 1) {
                for(int ind = 0 ; ind < values.size() ; ind += 2 ) {
                    int indSliderRow = values.at(ind+0);
                    int indCol = values.at(ind+1);
                    if(tabControllers[targetDisplay]->verticalSliders[indSliderRow][indCol]) {
                        tabControllers[targetDisplay]->verticalSliders[indSliderRow][indCol]->setIntValueFromMIDI(value);
                    }
                }
            }
        }
        if(value == 127 && hashButtonFXPulseMIDIIn.contains(key)) {
            QList<int> values = hashButtonFXPulseMIDIIn.values(key);
            if (values.size() % 2 == 0 && values.size() / 2 >= 1) {
                for(int ind = 0 ; ind < values.size() ; ind += 2 ) {
                    int indButtonRow = values.at(ind+0);
                    int indCol = values.at(ind+1);
                    if(tabControllers[targetDisplay]->mainButtons[indButtonRow][indCol]) {
                        tabControllers[targetDisplay]->mainButtons[indButtonRow][indCol]->animateClick();
                    }
                }
            }
        }
        break;
    case _Paths_display:
        if(hashSliderPathsMIDIIn.contains(key)) {
            QList<int> values = hashSliderPathsMIDIIn.values(key);
            if (values.size() % 2 == 0 && values.size() / 2 >= 1) {
                for(int ind = 0 ; ind < values.size() ; ind += 2 ) {
                    int indSliderRow = values.at(ind+0);
                    int indCol = values.at(ind+1);
                    if(tabControllers[targetDisplay]->verticalSliders[indSliderRow][indCol]) {
                        tabControllers[targetDisplay]->verticalSliders[indSliderRow][indCol]->setIntValueFromMIDI(value);
                    }
                }
            }
        }
        if(value == 127 && hashButtonPathsMIDIIn.contains(key)) {
            QList<int> values = hashButtonPathsMIDIIn.values(key);
            if (values.size() % 2 == 0 && values.size() / 2 >= 1) {
                for(int ind = 0 ; ind < values.size() ; ind += 2 ) {
                    int indButtonRow = values.at(ind+0);
                    int indCol = values.at(ind+1);
                    if(tabControllers[targetDisplay]->mainButtons[indButtonRow][indCol]) {
                        tabControllers[targetDisplay]->mainButtons[indButtonRow][indCol]->animateClick();
                    }
                }
            }
        }
        break;
    case _Paths_Pulse_display:
        if(hashSliderPathsPulseMIDIIn.contains(key)) {
            QList<int> values = hashSliderPathsPulseMIDIIn.values(key);
            if (values.size() % 2 == 0 && values.size() / 2 >= 1) {
                for(int ind = 0 ; ind < values.size() ; ind += 2 ) {
                    int indSliderRow = values.at(ind+0);
                    int indCol = values.at(ind+1);
                    if(tabControllers[targetDisplay]->verticalSliders[indSliderRow][indCol]) {
                        tabControllers[targetDisplay]->verticalSliders[indSliderRow][indCol]->setIntValueFromMIDI(value);
                    }
                }
            }
        }
        if(value == 127 && hashButtonPathsPulseMIDIIn.contains(key)) {
            QList<int> values = hashButtonPathsPulseMIDIIn.values(key);
            if (values.size() % 2 == 0 && values.size() / 2 >= 1) {
                for(int ind = 0 ; ind < values.size() ; ind += 2 ) {
                    int indButtonRow = values.at(ind+0);
                    int indCol = values.at(ind+1);
                    if(tabControllers[targetDisplay]->mainButtons[indButtonRow][indCol]) {
                        tabControllers[targetDisplay]->mainButtons[indButtonRow][indCol]->animateClick();
                    }
                }
            }
        }
        break;
#if defined(TVW) || defined(demo)
    case _MinimalIF_display:
        if(hashSliderMinimalIFMIDIIn.contains(key)) {
            QList<int> values = hashSliderMinimalIFMIDIIn.values(key);
            if (values.size() % 2 == 0 && values.size() / 2 >= 1) {
                for(int ind = 0 ; ind < values.size() ; ind += 2 ) {
                    int indSliderRow = values.at(ind+0);
                    int indCol = values.at(ind+1);
                    if(tabControllers[targetDisplay]->verticalSliders[indSliderRow][indCol]) {
                        tabControllers[targetDisplay]->verticalSliders[indSliderRow][indCol]->setIntValueFromMIDI(value);
                    }
                }
            }
        }
        if(value == 127 && hashButtonMinimalIFMIDIIn.contains(key)) {
            QList<int> values = hashButtonMinimalIFMIDIIn.values(key);
            if (values.size() % 2 == 0 && values.size() / 2 >= 1) {
                for(int ind = 0 ; ind < values.size() ; ind += 2 ) {
                    int indButtonRow = values.at(ind+0);
                    int indCol = values.at(ind+1);
                    if(tabControllers[targetDisplay]->mainButtons[indButtonRow][indCol]) {
                        tabControllers[targetDisplay]->mainButtons[indButtonRow][indCol]->animateClick();
                    }
                }
            }
        }
        break;
    case _MinimalIF_Pulse_display:
        if(hashSlideMinimalIFPulseMIDIIn.contains(key)) {
            QList<int> values = hashSlideMinimalIFPulseMIDIIn.values(key);
            if (values.size() % 2 == 0 && values.size() / 2 >= 1) {
                for(int ind = 0 ; ind < values.size() ; ind += 2 ) {
                    int indSliderRow = values.at(ind+0);
                    int indCol = values.at(ind+1);
                    if(tabControllers[targetDisplay]->verticalSliders[indSliderRow][indCol]) {
                        tabControllers[targetDisplay]->verticalSliders[indSliderRow][indCol]->setIntValueFromMIDI(value);
                    }
                }
            }
        }
        if(value == 127 && hashButtonMinimalIFPulseMIDIIn.contains(key)) {
            QList<int> values = hashButtonMinimalIFPulseMIDIIn.values(key);
            if (values.size() % 2 == 0 && values.size() / 2 >= 1) {
                for(int ind = 0 ; ind < values.size() ; ind += 2 ) {
                    int indButtonRow = values.at(ind+0);
                    int indCol = values.at(ind+1);
                    if(tabControllers[targetDisplay]->mainButtons[indButtonRow][indCol]) {
                        tabControllers[targetDisplay]->mainButtons[indButtonRow][indCol]->animateClick();
                    }
                }
            }
        }
        break;
#endif
    default:
        break;
    }
}

void messageAbortIgnore(QString message) {
    QMessageBox msgBox;
    msgBox.setText(message);
    msgBox.setInformativeText("Do you want to Abort?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::NoToAll);
    msgBox.setDefaultButton(QMessageBox::Yes);
    int ret = msgBox.exec();
    switch (ret) {
    case QMessageBox::Yes:
        exit(EXIT_FAILURE);
        break;
    case QMessageBox::NoToAll:
        // ignore next missing USBs
        ignoreAllMissingUSBs = true;
        break;
    case QMessageBox::No:
        // continue
        break;
    }
}

MidiInOut::MidiInOut(QWidget *parent)
    : QObject(parent)
{
    nPortsIn = 0;
    nPortsOut = 0;

    midiinTest = NULL;
    midioutTest = NULL;

    indMidiMixBeatMixerFXIn = -1;
    indMidiMixBeatMixerFXOut = -1;
    indAPCMiniPathsIn = -1;
    indAPCMiniPathsOut = -1;

    midiOutMessage.push_back( 0 );
    midiOutMessage.push_back( 0 );
    midiOutMessage.push_back( 0 );

  //////////////////////////////////////////////
  // MIDI IN
  // RtMidiIn constructor
  try {
    midiinTest = new RtMidiIn( RtMidi::WINDOWS_MM, std::string( "RtMidi Input Client"), int(2048) );
  }
  catch ( RtMidiError &error ) {
    error.printMessage();
    exit( EXIT_FAILURE );
  }
  // Check inputs.
  nPortsIn = midiinTest->getPortCount();
  qDebug() << "There are " << nPortsIn << " MIDI input sources available."; // << std::endl;
  std::string portName;
  for ( unsigned int i=0; i<nPortsIn; i++ ) {
    try {
      portName = midiinTest->getPortName(i);
    }
    catch ( RtMidiError &error ) {
      error.printMessage();
      midiCleanup();
      exit( EXIT_FAILURE );
    }
    qDebug() << "  Input Port #" << i << ": [" << QString::fromStdString(portName) << "]"; // << std::endl;
    if(portName.compare(0, 8, "MIDI Mix") == 0) {
        indMidiMixBeatMixerFXIn = i;
    }
    else if(portName.compare(0, 8, "APC MINI") == 0) {
        indAPCMiniPathsIn = i;
    }
  }
  if(indMidiMixBeatMixerFXIn == -1) {
      qDebug() << "Error: Input USB Device Midi Mix not found"; // << std::endl;
      if( !ignoreAllMissingUSBs ) {
       // messageAbortIgnore("Input USB Device Midi Mix not found.");
      }
  }
  if(indAPCMiniPathsIn == -1) {
      qDebug() << "Error: Input USB Device APC Mini not found"; // << std::endl;
      if( !ignoreAllMissingUSBs ) {
      //  messageAbortIgnore("Input USB Device APC Mini not found.");
      }
  }

  // Open available port.
  indIn = 0;
  for ( unsigned int i=0; i<2; i++ ) {
      if(i == 0) {
          if (indMidiMixBeatMixerFXIn >= 0) {
            midiReceivers.push_back( midiinTest );
            midiReceivers[i]->openPort( indMidiMixBeatMixerFXIn );
            qDebug() << "opening Midi Mix In port (Mixer and graphs) " << indMidiMixBeatMixerFXIn; // << std::endl;

            // Don't ignore sysex, timing, or active sensing messages.
            midiReceivers[i]->ignoreTypes( false, false, false );
            indIn++;
          }
          else {
              delete midiinTest;
              midiReceivers.push_back( NULL );
          }
      }
      else if(i == 1) {
          if (indAPCMiniPathsIn >= 0) {
              midiReceivers.push_back(new RtMidiIn( RtMidi::WINDOWS_MM, std::string( "RtMidi Input Client"), 2048 ));
              midiReceivers[i]->openPort( indAPCMiniPathsIn );
              qDebug() << "opening APC Mini In port (paths) " << indAPCMiniPathsIn; // << std::endl;

              // Don't ignore sysex, timing, or active sensing messages.
              midiReceivers[i]->ignoreTypes( false, false, false );
              indIn++;
          }
          else {
              midiReceivers.push_back( NULL );
          }
      }
  }

  //////////////////////////////////////////////
  // MIDI OUT
  // RtMidiOut constructor
  try {
    midioutTest = new RtMidiOut();
  }
  catch ( RtMidiError &error ) {
    error.printMessage();
    exit( EXIT_FAILURE );
  }
  // Check outputs.
  nPortsOut = midioutTest->getPortCount();
  qDebug() << "There are " << nPortsOut << " MIDI output ports available."; // << std::endl;
  for (unsigned int i=0; i<nPortsOut; i++ ) {
    try {
      portName = midioutTest->getPortName(i);
    }
    catch (RtMidiError &error) {
      error.printMessage();
      midiCleanup();
      exit( EXIT_FAILURE );
    }
    qDebug() << "  Output Port #" << i << ": [" << QString::fromStdString(portName) << "]"; // << std::endl;
    if(portName.compare(0, 8, "MIDI Mix") == 0) {
        indMidiMixBeatMixerFXOut = i;
    }
    else if(portName.compare(0, 8, "APC MINI") == 0) {
        indAPCMiniPathsOut = i;
    }
  }
  if(indMidiMixBeatMixerFXOut == -1) {
      qDebug() << "Error: Output USB Device Midi Mix not found"; // << std::endl;
      if( !ignoreAllMissingUSBs ) {
       // messageAbortIgnore("Output USB Device Midi Mix not found.");
      }
  }
  if(indAPCMiniPathsOut == -1) {
      qDebug() << "Error: Output USB Device APC Mini not found"; // << std::endl;
      if( !ignoreAllMissingUSBs ) {
       // messageAbortIgnore("Output USB Device APC Mini not found.");
      }
  }

  // Open available port.
  indOut = 0;
  for (unsigned int i=0; i<2; i++ ) {
      if(i == 0) {
          if (indMidiMixBeatMixerFXOut >= 0) {
            midiSenders.push_back( midioutTest );
            midiSenders[i]->openPort( indMidiMixBeatMixerFXOut );
            qDebug() << "opening Midi Mix Out port (Mixer and graphs) " << indMidiMixBeatMixerFXOut; // << std::endl;
            indOut++;
          }
          else {
              delete midioutTest;
              midiSenders.push_back( NULL );
          }
      }
      else if(i == 1) {
          if (indAPCMiniPathsOut >= 0) {
            midiSenders.push_back(new RtMidiOut());
            midiSenders[i]->openPort( indAPCMiniPathsOut );
            qDebug() << "opening APC Mini Out port (paths) " << indAPCMiniPathsOut; // << std::endl;
            indOut++;
          }
          else {
              midiSenders.push_back( NULL );
          }
      }
  }
}

int MidiInOut::midiCleanup(void) {
    for (unsigned int i=0; i<midiReceivers.size(); i++ ) {
        if(midiReceivers[i])
            delete midiReceivers[i];
    }
    for (unsigned int i=0; i<midiSenders.size(); i++ ) {
        if(midiSenders[i])
            delete midiSenders[i];
    }
    return 0;
}

void MidiInOut::sendMidiLightingMessage(DisplayType targetDisplay,
                                        int midiControl ,
                                        bool onOffValue) {
    // qDebug() << "Note On " << midiControl << " display " << targetDisplay << " current " << pg_window->getCurrentTabWindow() << " last " << pg_window->lastTabWindow << " val " << int(onOffValue); // << std::endl;

    // Note On: status - channel - value
    midiOutMessage[0] = _MIDINoteOn * 16;
    midiOutMessage[1] = midiControl;
    midiOutMessage[2] = (onOffValue?127:0);
    // qDebug() << "Note On " << midiControl << " val " << int(onOffValue); // << std::endl;
    if((targetDisplay == _Paths_display||targetDisplay == _Paths_Pulse_display) && midiSenders[1]) {
        midiSenders[1]->sendMessage( &midiOutMessage );
    }
    else if(pg_window->getCurrentTabWindow() == targetDisplay) {
        midiSenders[0]->sendMessage( &midiOutMessage );
    }
}

void MainWindow::reinitializeMidiLighting( DisplayType targetDisplay ) {
    /////////////////////////////////////////////////////////////////////////////////////
     switch( targetDisplay ) {
     case _Beat_display:
     case _Beat_Pulse_display:
         // MAIN BUTTONS
         for(unsigned int indButtonRow = 0 ; indButtonRow < ButtonsBeatData.size() ; indButtonRow++ ) {
             for(unsigned int indCol = 0 ; indCol < ButtonsBeatData[indButtonRow].size() ; indCol++ ) {
                 if(tabControllers[targetDisplay]->mainButtons[indButtonRow][indCol]) {
                    tabControllers[targetDisplay]->mainButtons[indButtonRow][indCol]->sendMidiLightingMessage(true);
                 }
             }
         }
         break;
     case _Mixer_display:
     case _Mixer_Pulse_display:
         // MAIN BUTTONS
         for(unsigned int indButtonRow = 0 ; indButtonRow < ButtonsMixerData.size() ; indButtonRow++ ) {
             for(unsigned int indCol = 0 ; indCol < ButtonsMixerData[indButtonRow].size() ; indCol++ ) {
                 if(tabControllers[targetDisplay]->mainButtons[indButtonRow][indCol]) {
                    tabControllers[targetDisplay]->mainButtons[indButtonRow][indCol]->sendMidiLightingMessage(true);
                 }
             }
         }
         break;
     case _FX_display:
     case _FX_Pulse_display:
         // MAIN BUTTONS
         for(unsigned int indButtonRow = 0 ; indButtonRow < ButtonsFXData.size() ; indButtonRow++ ) {
             for(unsigned int indCol = 0 ; indCol < ButtonsFXData[indButtonRow].size() ; indCol++ ) {
                 if(tabControllers[targetDisplay]->mainButtons[indButtonRow][indCol]) {
                    tabControllers[targetDisplay]->mainButtons[indButtonRow][indCol]->sendMidiLightingMessage(true);
                 }
             }
         }
         break;
     case _Paths_display:
     case _Paths_Pulse_display:
         // MAIN BUTTONS
         for(unsigned int indButtonRow = 0 ; indButtonRow < ButtonsPathsData.size() ; indButtonRow++ ) {
             for(unsigned int indCol = 0 ; indCol < ButtonsPathsData[indButtonRow].size() ; indCol++ ) {
                 if(tabControllers[targetDisplay]->mainButtons[indButtonRow][indCol]) {
                     tabControllers[targetDisplay]->mainButtons[indButtonRow][indCol]->sendMidiLightingMessage(true);
                 }
             }
         }
         break;
#if defined(TVW) || defined(demo)
     case _MinimalIF_display:
     case _MinimalIF_Pulse_display:
         // MAIN BUTTONS
         for(unsigned int indButtonRow = 0 ; indButtonRow < ButtonsMinimalIFData.size() ; indButtonRow++ ) {
             for(unsigned int indCol = 0 ; indCol < ButtonsMinimalIFData[indButtonRow].size() ; indCol++ ) {
                 if(tabControllers[targetDisplay]->mainButtons[indButtonRow][indCol]) {
                    tabControllers[targetDisplay]->mainButtons[indButtonRow][indCol]->sendMidiLightingMessage(true);
                 }
             }
         }
         break;
#endif
     default:
         break;
     }
}

void MainWindow::swithOffMidiLighting( DisplayType targetDisplay ) {
    /////////////////////////////////////////////////////////////////////////////////////
     switch( targetDisplay ) {
     case _Beat_display:
     case _Beat_Pulse_display:
         // MAIN BUTTONS
         for(unsigned int indButtonRow = 0 ; indButtonRow < ButtonsBeatData.size() ; indButtonRow++ ) {
             for(unsigned int indCol = 0 ; indCol < ButtonsBeatData[indButtonRow].size() ; indCol++ ) {
                 if(tabControllers[targetDisplay]->mainButtons[indButtonRow][indCol]) {
                     tabControllers[targetDisplay]->mainButtons[indButtonRow][indCol]->setHighlightOff();
                 }
             }
         }
         break;
     case _Mixer_display:
     case _Mixer_Pulse_display:
         // MAIN BUTTONS
         for(unsigned int indButtonRow = 0 ; indButtonRow < ButtonsMixerData.size() ; indButtonRow++ ) {
             for(unsigned int indCol = 0 ; indCol < ButtonsMixerData[indButtonRow].size() ; indCol++ ) {
                 if(tabControllers[targetDisplay]->mainButtons[indButtonRow][indCol]) {
                     tabControllers[targetDisplay]->mainButtons[indButtonRow][indCol]->setHighlightOff();
                 }
             }
         }
         break;
     case _FX_display:
     case _FX_Pulse_display:
         // MAIN BUTTONS
         for(unsigned int indButtonRow = 0 ; indButtonRow < ButtonsFXData.size() ; indButtonRow++ ) {
             for(unsigned int indCol = 0 ; indCol < ButtonsFXData[indButtonRow].size() ; indCol++ ) {
                 if(tabControllers[targetDisplay]->mainButtons[indButtonRow][indCol]) {
                     tabControllers[targetDisplay]->mainButtons[indButtonRow][indCol]->setHighlightOff();
                 }
             }
         }
         break;
     case _Paths_display:
     case _Paths_Pulse_display:
         // MAIN BUTTONS
         for(unsigned int indButtonRow = 0 ; indButtonRow < ButtonsPathsData.size() ; indButtonRow++ ) {
             for(unsigned int indCol = 0 ; indCol < ButtonsPathsData[indButtonRow].size() ; indCol++ ) {
                 if(tabControllers[targetDisplay]->mainButtons[indButtonRow][indCol]) {
                    tabControllers[targetDisplay]->mainButtons[indButtonRow][indCol]->setHighlightOff();
                 }
             }
         }
         break;
#if defined(TVW) || defined(demo)
     case _MinimalIF_display:
     case _MinimalIF_Pulse_display:
         // MAIN BUTTONS
         for(unsigned int indButtonRow = 0 ; indButtonRow < ButtonsMinimalIFData.size() ; indButtonRow++ ) {
             for(unsigned int indCol = 0 ; indCol < ButtonsMinimalIFData[indButtonRow].size() ; indCol++ ) {
                 if(tabControllers[targetDisplay]->mainButtons[indButtonRow][indCol]) {
                     tabControllers[targetDisplay]->mainButtons[indButtonRow][indCol]->setHighlightOff();
                 }
             }
         }
         break;
#endif
     default:
         break;
     }
}

