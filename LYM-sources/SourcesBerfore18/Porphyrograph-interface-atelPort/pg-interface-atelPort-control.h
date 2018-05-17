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

#ifndef CONTROL_H
#define CONTROL_H

#include "pg-interface-atelPort-data.h"
#include "pg-interface-atelPort-display.h"

#include <QGroupBox>
#include <QSlider>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QShortcut>

QT_BEGIN_NAMESPACE
class QLabel;
class QSpinBox;
class QSlider;
class QPushButton;
class QSortcut;
QT_END_NAMESPACE

///////////////////////////////////////////////////////////////
/// \brief The DoubleSlider class
///
class DoubleSlider : public QSlider
{
    Q_OBJECT

    public:
        explicit DoubleSlider(QWidget *parent = 0);
        double getAmplitude(void);
        void setAmplitude(double givenValue);

    public slots:
        void setInversedScaledIntValue(int givenValue);
        void setScaledDoubleValue(double givenValue);
        void inverse(void);
        void normal(void);


    signals:
        void doubleValueChanged(double newValue);
        void intValueChanged(int newValue);

    private:
        double amplitude;
};

///////////////////////////////////////////////////////////////
/// \brief The SlidersSpinboxGroup class
///
class SlidersSpinboxGroup : public QGroupBox
{
    Q_OBJECT

public:
    SlidersSpinboxGroup(unsigned int indSliderRow,
                        unsigned int indSliderCol,
                        std::vector<std::vector<inOutControllerData>> tab_controllers,
                        AssociatedDisplayType givenAssociatedDisplayType = _LEDBar,
                        QWidget *parent = 0);
    void setAmplitude(double givenValue);
    void setLabelUDPIn(QString givenLabel);
    void setLabelHighlightIn(QString givenLabel);
    void setlabelUDPOut(QString givenLabel);
    void setMidiIn(int givenLabel);
    void setMidiOut(int givenLabel);
    void setHighlight(float givenValue);
    bool getHighlight(void);
    int getIndLEDBarDataRow(void);
    int getIndLEDBarDataCol(void);
    void setAssociatedLEDBar(LEDBar *givenLEDBar);
    void setAssociatedBox(Box *givenBox);
    AssociatedDisplayType getAssociatedDisplayType(void);
    QString getLabelUDPIn(void);
    QString getLabelHighlightIn(void);
    QString getlabelUDPOut(void);
    int getMidiIn(void);
    int getMidiOut(void);
    void setMinimum(int givenValue);
    void setMaximum(int givenValue);

public slots:
    void setIntValueFromMIDI(int givenValue);
    void setDoubleValueFromUDP(double givenValue);
    void sendUDPMessage(double givenValue);

private:
    QString labelUDPIn;
    QString labelHighlightIn;
    float valueHighlightIn;
    bool hasValueHighlightIn;
    QString labelUDPOut;
    int midiInChannel;
    int midiOutControl;
    // bool onOffPrevious;
    DoubleSlider *slider;
    QDoubleSpinBox *spinBox;
    bool valueFromUDPMessage;
    bool highlight;
    int indLEDBarDataRow;
    int indLEDBarDataCol;
    LEDBar *associatedLEDBar;
    Box *associatedBox;
    AssociatedDisplayType associatedDisplayType;
};

///////////////////////////////////////////////////////////////
/// \brief The SingleButton class
///
class SingleButton : public QPushButton
{
    Q_OBJECT

public:
    SingleButton(unsigned int indButton,
                 std::vector<inOutControllerData> tab_buttonsControllers,
                 DisplayType givenButtonDisplayType,
                 QWidget *parent = 0);
    SingleButton(unsigned int indButtonRow,
                 unsigned int indButtonCol,
                 std::vector<std::vector<inOutControllerData>> tab_buttonsControllers,
                 DisplayType givenButtonDisplayType,
                 QWidget *parent = 0);
    void init(inOutControllerData *givenData,
              DisplayType givenButtonDisplayType);
    void setHighlight(float givenValue);
    void setHighlightOff(void);
    bool getHighlight(void);
    void setUDPvalue(float givenValue);
    float getUDPvalue(void);
    void setLabelUDPIn(QString givenLabel);
    void setLabelHighlightIn(QString givenLabel);
    void setLabelText(QString givenLabel);
    void setlabelUDPOut(QString givenLabel);
    void setMidiIn(int givenLabel);
    void setMidiOut(int givenLabel);
    void setAssociatedSingleLabel( SingleLabel *givenSingleLabel);
    SingleLabel *getAssociatedSingleLabel(void);
    QString getLabelUDPIn(void);
    QString getLabelHighlightIn(void);
    QString getLabelText(void);
    QString getlabelUDPOut(void);
    int getMidiIn(void);
    int getMidiOut(void);
    void sendMidiLightingMessage(bool forceEmission = false);
    void sendMidiOnOffLightingMessage(bool onOff);

public slots:
    void sendUDPMessage(void);
    void handleEvent(void);

private:
    QString labelUDPIn;
    QString labelHighlightIn;
    QString labelText;
    float valueHighlightIn;
    bool hasValueHighlightIn;
    QString labelUDPOut;
    int midiInControl;
    int midiOutControl;
//    QPushButton *button;
    QShortcut *shortcut;
    bool highlight;
    bool highlightPrevious;
    float UDPvalue;
    SingleLabel *associatedSingleLabel;
    DisplayType buttonDisplayType;
};

#define _NbRows_mainButtons_graphs 2
#define _NbRows_buttons_paths 10
#define _NbRows_sideButtons_graphs 3

//////////////////////////////////////////////////////////////
///
/// \brief The GraphicalControllers class
///
class GraphicalControllers : public QWidget
{
    Q_OBJECT

public:
    explicit GraphicalControllers(QString givenTitle,
                                  DisplayType givenType,
                                  QWidget *parent = 0);
    SlidersSpinboxGroup **verticalSliders_graphs[_MaxGraphsInOutDataRows];
    SingleButton **mainButtons_graphs[_NbRows_mainButtons_graphs];
    SingleButton *sideButtons_graphs[_NbRows_sideButtons_graphs];

    SlidersSpinboxGroup **verticalSliders_tracks[_MaxTracksInOutDataRows];
    SingleButton **mainButtons_tracks[_NbRows_mainButtons_graphs];
    SingleButton *sideButtons_tracks[_NbRows_sideButtons_graphs];

    SlidersSpinboxGroup **verticalSliders_paths[_MaxPathsInOutDataRows];
    SingleButton **mainButtons_paths[_NbRows_buttons_paths];

    QGroupBox *flashpixelpulseSliders_graphs;
    QGroupBox *sideButton_graphs;
    QGroupBox *penColorpixelModeButtons_graphs;
    QGroupBox *repopBWpenbrushModeButtons_graphs;
    QGroupBox *radiusbeatSliders_graphs;
    QGroupBox *pixelRadiusSliders_graphs;
    QGroupBox *pixelPulseSliders_graphs;

    QGroupBox *weightrepopmediasobelSliders_tracks;
    QGroupBox *sideButton_tracks;
    QGroupBox *tracksmiconoffsoundsampleButtons_tracks;
    QGroupBox *decaysignemptyButtonmediaplusButtons_tracks;
    QGroupBox *decayechosoundSliders_tracks;
    QGroupBox *masterSliders_tracks;

    QGroupBox *partLine1Buttons_paths;
    QGroupBox *sideButton_paths;
    QGroupBox *partLine2Buttons_paths;
    QGroupBox *partInitButtons_paths;
    QGroupBox *pathtRepopButtons_paths;
    QGroupBox *pathFollowButtons_paths;
    QGroupBox *pathRepulseButtons_paths;
    QGroupBox *pathRecordButtons_paths;
    QGroupBox *pathReplayButtons_paths;
    QGroupBox *partLine9Buttons_paths;
    QGroupBox *partLine10Sliders_paths;

    void create_graphsTabControllers(void);
    void create_tracksTabControllers(void);
    void create_pathsTabControllers(void);

private:
    QString title;
};

#endif
