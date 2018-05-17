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

#include "pg-interface-atelPort-comm.h"
#include "pg-interface-atelPort-window.h"
#include "pg-interface-atelPort-udp.h"
#include "pg-interface-atelPort-display.h"
#include "pg-interface-atelPort-control.h"
#include "pg-interface-atelPort-main.h"

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
                    std::cout << "Receive midi stamp " << stamp << " size " << nBytes << std::endl;
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
                           std::cout << ", Additional data " << i << " " << (int)messageIn[i]  << std::endl;
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
           if(indMidiin == 0 && pg_window->currentTabWindow == 1) {
               targetDisplay = _Graphs_display;
           }
           else if(indMidiin == 0 && pg_window->currentTabWindow == 2) {
               targetDisplay = _Tracks_display;
           }
           else if(indMidiin == 0) {
               if(pg_window->lastTabWindow == _Graphs_display
                       || pg_window->lastTabWindow == _Tracks_display) {
                   targetDisplay = pg_window->lastTabWindow;
               }
               else {
                   targetDisplay = _Graphs_display;
               }
           }
           else if(indMidiin == 1) {
               targetDisplay = _Paths_display;
           }
           else {
               std::cout << "Target display not found!" << std::endl;
           }

           for(unsigned int indVec = 0 ;indVec < vec_dataType.size() ; indVec++) {
               // if(traceConsole)
               //               qDebug() <<  "Midi message for " << targetDisplay << "(" << indMidiin << ") type/index/channel "
               //                         << vec_dataType[indVec] << " " << vec_controlIndex[indVec]
               //                            << " " << vec_indChannel[indVec] << " val " << vec_value[indVec];
               pg_window->propagateMidiEventsToControllers( targetDisplay,
                                                            vec_dataType[indVec],
                                                            vec_controlIndex[indVec],
                                                            vec_indChannel[indVec],
                                                            vec_value[indVec] );
           }
       }
   }
}

void MainWindow::propagateMidiEventsToControllers( DisplayType targetDisplay,
                                                   int dataType,
                                                   int controlIndex,
                                                   int indChannel,
                                                   int value ) {
   /////////////////////////////////////////////////////////////////////////////////////
    switch( targetDisplay ) {
    case _Graphs_display:
        // MAIN SLIDERS
        for(unsigned int indSliderRow = 0 ; indSliderRow < graphs_mainSlidersData.size() ; indSliderRow++ ) {
            for(unsigned int indCol = 0 ; indCol < graphs_mainSlidersData[indSliderRow].size() ; indCol++ ) {
                if( dataType == graphs_mainSlidersData[indSliderRow][indCol].get_typeMidiIn()
                        && controlIndex == graphs_mainSlidersData[indSliderRow][indCol].get_controlMidiIn()
                        && indChannel == graphs_mainSlidersData[indSliderRow][indCol].get_channelMidiIn()
                        && graphsTabControllers->verticalSliders_graphs[indSliderRow][indCol] ) {
                    graphsTabControllers->verticalSliders_graphs[indSliderRow][indCol]->setIntValueFromMIDI(value);
                    break;
                }
            }
        }
        if(value == 127) {
            // SIDE BUTTONS
            for(unsigned int indSingleButton = 0 ; indSingleButton < graphs_sideButtonsData.size() ; indSingleButton++ ) {
                if( dataType == graphs_sideButtonsData[indSingleButton].get_typeMidiIn()
                        && controlIndex == graphs_sideButtonsData[indSingleButton].get_controlMidiIn()
                        && indChannel == graphs_sideButtonsData[indSingleButton].get_channelMidiIn()
                        && graphsTabControllers->sideButtons_graphs[indSingleButton] ) {
                     graphsTabControllers->sideButtons_graphs[indSingleButton]->animateClick();
                     break;
                }
            }
            // MAIN BUTTONS
            for(unsigned int indButtonRow = 0 ; indButtonRow < graphs_mainButtonsData.size() ; indButtonRow++ ) {
                for(unsigned int indCol = 0 ; indCol < graphs_mainButtonsData[indButtonRow].size() ; indCol++ ) {
                    if( dataType == graphs_mainButtonsData[indButtonRow][indCol].get_typeMidiIn()
                            && controlIndex == graphs_mainButtonsData[indButtonRow][indCol].get_controlMidiIn()
                            && indChannel == graphs_mainButtonsData[indButtonRow][indCol].get_channelMidiIn()
                            && graphsTabControllers->mainButtons_graphs[indButtonRow][indCol] ) {
                         graphsTabControllers->mainButtons_graphs[indButtonRow][indCol]->animateClick();
                         break;
                    }
                }
            }
        }
        break;
    case _Tracks_display:
        // MAIN SLIDERS
        for(unsigned int indSliderRow = 0 ; indSliderRow < tracks_mainSlidersData.size() ; indSliderRow++ ) {
            for(unsigned int indCol = 0 ; indCol < tracks_mainSlidersData[indSliderRow].size() ; indCol++ ) {
                if( dataType == tracks_mainSlidersData[indSliderRow][indCol].get_typeMidiIn()
                        && controlIndex == tracks_mainSlidersData[indSliderRow][indCol].get_controlMidiIn()
                        && indChannel == tracks_mainSlidersData[indSliderRow][indCol].get_channelMidiIn()
                        && tracksTabControllers->verticalSliders_tracks[indSliderRow][indCol] ) {
                    tracksTabControllers->verticalSliders_tracks[indSliderRow][indCol]->setIntValueFromMIDI(value);
                    break;
                }
            }
        }
        if(value == 127) {
            // SIDE BUTTONS
            for(unsigned int indSingleButton = 0 ; indSingleButton < tracks_sideButtonsData.size() ; indSingleButton++ ) {
                if( dataType == tracks_sideButtonsData[indSingleButton].get_typeMidiIn()
                        && controlIndex == tracks_sideButtonsData[indSingleButton].get_controlMidiIn()
                        && indChannel == tracks_sideButtonsData[indSingleButton].get_channelMidiIn()
                        && tracksTabControllers->sideButtons_tracks[indSingleButton] ) {
                    tracksTabControllers->sideButtons_tracks[indSingleButton]->animateClick();
                    break;
                }
            }
            // MAIN BUTTONS
            for(unsigned int indButtonRow = 0 ; indButtonRow < tracks_mainButtonsData.size() ; indButtonRow++ ) {
                for(unsigned int indCol = 0 ; indCol < tracks_mainButtonsData[indButtonRow].size() ; indCol++ ) {
                    if( dataType == tracks_mainButtonsData[indButtonRow][indCol].get_typeMidiIn()
                            && controlIndex == tracks_mainButtonsData[indButtonRow][indCol].get_controlMidiIn()
                            && indChannel == tracks_mainButtonsData[indButtonRow][indCol].get_channelMidiIn()
                            && tracksTabControllers->mainButtons_tracks[indButtonRow][indCol] ) {
                         tracksTabControllers->mainButtons_tracks[indButtonRow][indCol]->animateClick();
                         break;
                    }
                }
            }
        }
        break;
    case _Paths_display:
        // MAIN SLIDERS
        for(unsigned int indSliderRow = 0 ; indSliderRow < paths_mainSlidersData.size() ; indSliderRow++ ) {
            for(unsigned int indCol = 0 ; indCol < paths_mainSlidersData[indSliderRow].size() ; indCol++ ) {
                if( dataType == paths_mainSlidersData[indSliderRow][indCol].get_typeMidiIn()
                        && controlIndex == paths_mainSlidersData[indSliderRow][indCol].get_controlMidiIn()
                        && indChannel == paths_mainSlidersData[indSliderRow][indCol].get_channelMidiIn()
                        && pathsTabControllers->verticalSliders_paths[indSliderRow][indCol] ) {
                    pathsTabControllers->verticalSliders_paths[indSliderRow][indCol]->setIntValueFromMIDI(value);
                    break;
                }
            }
        }
        if(value == 127) {
            // MAIN BUTTONS
            for(unsigned int indButtonRow = 0 ; indButtonRow < paths_mainButtonsData.size() ; indButtonRow++ ) {
                for(unsigned int indCol = 0 ; indCol < paths_mainButtonsData[indButtonRow].size() ; indCol++ ) {
                    if( dataType == paths_mainButtonsData[indButtonRow][indCol].get_typeMidiIn()
                            && controlIndex == paths_mainButtonsData[indButtonRow][indCol].get_controlMidiIn()
                            && indChannel == paths_mainButtonsData[indButtonRow][indCol].get_channelMidiIn()
                            && pathsTabControllers->mainButtons_paths[indButtonRow][indCol] ) {
                         pathsTabControllers->mainButtons_paths[indButtonRow][indCol]->animateClick();
                         break;
                    }
                }
            }
        }
        break;
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

    indMidiMixGraphsTracksIn = -1;
    indMidiMixGraphsTracksOut = -1;
    indAPCMiniPathsIn = -1;
    indAPCMiniPathsOut = -1;

    midiOutMessage.push_back( 0 );
    midiOutMessage.push_back( 0 );
    midiOutMessage.push_back( 0 );

  //////////////////////////////////////////////
  // MIDI IN
  // RtMidiIn constructor
  try {
    midiinTest = new RtMidiIn( RtMidi::WINDOWS_MM, std::string( "RtMidi Input Client"), 2048 );
  }
  catch ( RtMidiError &error ) {
    error.printMessage();
    exit( EXIT_FAILURE );
  }
  // Check inputs.
  nPortsIn = midiinTest->getPortCount();
  std::cout << std::endl << "There are " << nPortsIn << " MIDI input sources available." << std::endl;
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
    std::cout << "  Input Port #" << i << ": [" << portName << "]" << std::endl;
    if(portName.compare(0, 8, "MIDI Mix") == 0) {
        indMidiMixGraphsTracksIn = i;
    }
    else if(portName.compare(0, 8, "APC MINI") == 0) {
        indAPCMiniPathsIn = i;
    }
  }
  if(indMidiMixGraphsTracksIn == -1) {
      std::cout << "Error: Input USB Device Midi Mix not found" << std::endl;
      if( !ignoreAllMissingUSBs ) {
       // messageAbortIgnore("Input USB Device Midi Mix not found.");
      }
  }
  if(indAPCMiniPathsIn == -1) {
      std::cout << "Error: Input USB Device APC Mini not found" << std::endl;
      if( !ignoreAllMissingUSBs ) {
      //  messageAbortIgnore("Input USB Device APC Mini not found.");
      }
  }

  // Open available port.
  indIn = 0;
  for ( unsigned int i=0; i<2; i++ ) {
      if(i == 0) {
          if (indMidiMixGraphsTracksIn >= 0) {
            midiReceivers.push_back( midiinTest );
            midiReceivers[i]->openPort( indMidiMixGraphsTracksIn );
            std::cout << "opening Midi Mix In port (tracks and graphs) " << indMidiMixGraphsTracksIn << std::endl;

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
              std::cout << "opening APC Mini In port (paths) " << indAPCMiniPathsIn << std::endl;

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
  std::cout << std::endl << "There are " << nPortsOut << " MIDI output ports available." << std::endl;
  for (unsigned int i=0; i<nPortsOut; i++ ) {
    try {
      portName = midioutTest->getPortName(i);
    }
    catch (RtMidiError &error) {
      error.printMessage();
      midiCleanup();
      exit( EXIT_FAILURE );
    }
    std::cout << "  Output Port #" << i << ": [" << portName << "]" << std::endl;
    if(portName.compare(0, 8, "MIDI Mix") == 0) {
        indMidiMixGraphsTracksOut = i;
    }
    else if(portName.compare(0, 8, "APC MINI") == 0) {
        indAPCMiniPathsOut = i;
    }
  }
  if(indMidiMixGraphsTracksOut == -1) {
      std::cout << "Error: Output USB Device Midi Mix not found" << std::endl;
      if( !ignoreAllMissingUSBs ) {
       // messageAbortIgnore("Output USB Device Midi Mix not found.");
      }
  }
  if(indAPCMiniPathsOut == -1) {
      std::cout << "Error: Output USB Device APC Mini not found" << std::endl;
      if( !ignoreAllMissingUSBs ) {
       // messageAbortIgnore("Output USB Device APC Mini not found.");
      }
  }

  // Open available port.
  indOut = 0;
  for (unsigned int i=0; i<2; i++ ) {
      if(i == 0) {
          if (indMidiMixGraphsTracksOut >= 0) {
            midiSenders.push_back( midioutTest );
            midiSenders[i]->openPort( indMidiMixGraphsTracksOut );
            std::cout << "opening Midi Mix Out port (tracks and graphs) " << indMidiMixGraphsTracksOut << std::endl;
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
            std::cout << "opening APC Mini Out port (paths) " << indAPCMiniPathsOut << std::endl;
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
    // std::cout << "Note On " << midiControl << " display " << targetDisplay << " current " << pg_window->currentTabWindow << " last " << pg_window->lastTabWindow << " val " << int(onOffValue) << std::endl;

    // Note On: status - channel - value
    midiOutMessage[0] = _MIDINoteOn * 16;
    midiOutMessage[1] = midiControl;
    midiOutMessage[2] = (onOffValue?127:0);
    if(targetDisplay == _Graphs_display && (pg_window->currentTabWindow == _Graphs_display
                                            || (pg_window->currentTabWindow == _Paths_display
                                                && pg_window->lastTabWindow == _Graphs_display)) && midiSenders[0]) {
            // std::cout << "Note On " << midiControl << " val " << int(onOffValue) << std::endl;
            midiSenders[0]->sendMessage( &midiOutMessage );
    }
    else if(targetDisplay == _Tracks_display && (pg_window->currentTabWindow == _Tracks_display
                                                 || (pg_window->currentTabWindow == _Paths_display
                                                     && pg_window->lastTabWindow == _Tracks_display)) && midiSenders[0]) {
            midiSenders[0]->sendMessage( &midiOutMessage );
    }
    else if(targetDisplay == _Paths_display && midiSenders[1]) {
            midiSenders[1]->sendMessage( &midiOutMessage );
    }
}

void MainWindow::reinitializeMidiLighting( DisplayType targetDisplay ) {
    /////////////////////////////////////////////////////////////////////////////////////
     switch( targetDisplay ) {
     case _Graphs_display:
         // SIDE BUTTONS
         for(unsigned int indSingleButton = 0 ; indSingleButton < graphs_sideButtonsData.size() ; indSingleButton++ ) {
             graphsTabControllers->sideButtons_graphs[indSingleButton]->sendMidiLightingMessage(true);
         }
         // MAIN BUTTONS
         for(unsigned int indButtonRow = 0 ; indButtonRow < graphs_mainButtonsData.size() ; indButtonRow++ ) {
             for(unsigned int indCol = 0 ; indCol < graphs_mainButtonsData[indButtonRow].size() ; indCol++ ) {
                 graphsTabControllers->mainButtons_graphs[indButtonRow][indCol]->sendMidiLightingMessage(true);
             }
         }
         break;
     case _Tracks_display:
         // SIDE BUTTONS
         for(unsigned int indSingleButton = 0 ; indSingleButton < tracks_sideButtonsData.size() ; indSingleButton++ ) {
            tracksTabControllers->sideButtons_tracks[indSingleButton]->sendMidiLightingMessage(true);
         }
         // MAIN BUTTONS
         for(unsigned int indButtonRow = 0 ; indButtonRow < tracks_mainButtonsData.size() ; indButtonRow++ ) {
             for(unsigned int indCol = 0 ; indCol < tracks_mainButtonsData[indButtonRow].size() ; indCol++ ) {
                 tracksTabControllers->mainButtons_tracks[indButtonRow][indCol]->sendMidiLightingMessage(true);
             }
         }
         break;
     case _Paths_display:
         // MAIN BUTTONS
         for(unsigned int indButtonRow = 0 ; indButtonRow < paths_mainButtonsData.size() ; indButtonRow++ ) {
             for(unsigned int indCol = 0 ; indCol < paths_mainButtonsData[indButtonRow].size() ; indCol++ ) {
                 pathsTabControllers->mainButtons_paths[indButtonRow][indCol]->sendMidiLightingMessage(true);
             }
         }
         break;
     default:
         break;
     }
}

void MainWindow::swithOffMidiLighting( DisplayType targetDisplay ) {
    /////////////////////////////////////////////////////////////////////////////////////
     switch( targetDisplay ) {
     case _Graphs_display:
         // SIDE BUTTONS
         for(unsigned int indSingleButton = 0 ; indSingleButton < graphs_sideButtonsData.size() ; indSingleButton++ ) {
             graphsTabControllers->sideButtons_graphs[indSingleButton]->setHighlightOff();
         }
         // MAIN BUTTONS
         for(unsigned int indButtonRow = 0 ; indButtonRow < graphs_mainButtonsData.size() ; indButtonRow++ ) {
             for(unsigned int indCol = 0 ; indCol < graphs_mainButtonsData[indButtonRow].size() ; indCol++ ) {
                 graphsTabControllers->mainButtons_graphs[indButtonRow][indCol]->setHighlightOff();
             }
         }
         break;
     case _Tracks_display:
         // SIDE BUTTONS
         for(unsigned int indSingleButton = 0 ; indSingleButton < tracks_sideButtonsData.size() ; indSingleButton++ ) {
             tracksTabControllers->sideButtons_tracks[indSingleButton]->setHighlightOff();
         }
         // MAIN BUTTONS
         for(unsigned int indButtonRow = 0 ; indButtonRow < tracks_mainButtonsData.size() ; indButtonRow++ ) {
             for(unsigned int indCol = 0 ; indCol < tracks_mainButtonsData[indButtonRow].size() ; indCol++ ) {
                 tracksTabControllers->mainButtons_tracks[indButtonRow][indCol]->setHighlightOff();
             }
         }
         break;
     case _Paths_display:
         // MAIN BUTTONS
         for(unsigned int indButtonRow = 0 ; indButtonRow < paths_mainButtonsData.size() ; indButtonRow++ ) {
             for(unsigned int indCol = 0 ; indCol < paths_mainButtonsData[indButtonRow].size() ; indCol++ ) {
                 pathsTabControllers->mainButtons_paths[indButtonRow][indCol]->setHighlightOff();
             }
         }
         break;
     default:
         break;
     }
}

