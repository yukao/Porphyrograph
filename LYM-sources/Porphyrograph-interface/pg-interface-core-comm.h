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
#ifndef PGINTERFACESONGMIDI_H
#define PGINTERFACESONGMIDI_H

#include <vector>

#include <QApplication>
#include <QThread>
#include <QTimer>
#include <QWidget>
#include <QGroupBox>
#include <QUdpSocket>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QLabel>
#include <QSpinBox>

#include "RtMidi.h"
#include "pg-interface-core-display.h"

#define _MIDINoteOff    8
#define _MIDINoteOn     9
#define _MIDIController 11

////////////////////////////////////////////////////////////
// MIDI COMMUNICATION
////////////////////////////////////////////////////////////

class MidiInOut : public QObject
{
    Q_OBJECT

public:
    // midi in / out interface
    RtMidiIn *midiinTest;
    RtMidiOut *midioutTest;
    std::vector<RtMidiIn*> midiReceivers;
    std::vector<RtMidiOut*> midiSenders;

    unsigned int nPortsIn;
    unsigned int nPortsOut;
    int indIn;
    int indOut;

    int indMidiMixBeatMixerFXIn;
    int indMidiMixBeatMixerFXOut;
    int indAPCMiniPathsIn;
    int indAPCMiniPathsOut;

    MidiInOut(QWidget *parent = 0);

    int midiCleanup( void );

public slots:
    void receiveMidiIn(void);

public:
    void sendMidiLightingMessage(DisplayType targetDisplay,
                         int midiControl ,
                         bool onOffValue);

private:
    std::vector<unsigned char> midiOutMessage;
 };

// hash table for MIDI message lookup
extern QMultiHash<QString, int> hashButtonBeatMIDIIn;
extern QMultiHash<QString, int> hashSliderBeatMIDIIn;
extern QMultiHash<QString, int> hashButtonBeatPulseMIDIIn;
extern QMultiHash<QString, int> hashSliderBeatPulseMIDIIn;
extern QMultiHash<QString, int> hashButtonMixerMIDIIn;
extern QMultiHash<QString, int> hashSliderMixerMIDIIn;
extern QMultiHash<QString, int> hashButtonMixerPulseMIDIIn;
extern QMultiHash<QString, int> hashSliderMixerPulseMIDIIn;
extern QMultiHash<QString, int> hashButtonFXMIDIIn;
extern QMultiHash<QString, int> hashSliderFXMIDIIn;
extern QMultiHash<QString, int> hashButtonFXPulseMIDIIn;
extern QMultiHash<QString, int> hashSliderFXPulseMIDIIn;
extern QMultiHash<QString, int> hashButtonPathsMIDIIn;
extern QMultiHash<QString, int> hashSliderPathsMIDIIn;
extern QMultiHash<QString, int> hashButtonPathsPulseMIDIIn;
extern QMultiHash<QString, int> hashSliderPathsPulseMIDIIn;
#if defined(TVW) || defined(demo)
extern QMultiHash<QString, int> hashButtonMinimalIFMIDIIn;
extern QMultiHash<QString, int> hashSliderMinimalIFMIDIIn;
extern QMultiHash<QString, int> hashButtonMinimalIFPulseMIDIIn;
extern QMultiHash<QString, int> hashSlideMinimalIFPulseMIDIIn;
#endif
void messageAbortIgnore(QString message);

#endif // PGINTERFACESONGMIDI_H