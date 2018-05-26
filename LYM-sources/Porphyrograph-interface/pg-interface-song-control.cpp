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

#include "pg-interface-song-window.h"
#include "pg-interface-song-udp.h"
#include "pg-interface-song-control.h"

#include <QtWidgets>
#include <QTime>
#include <QtDebug>

#include <vector>
#include <iostream>
#include <cstdlib>

//#define _MIDINoteOff    8
//#define _MIDINoteOn     9
//#define _MIDIController 11

/////////////////////////////////////////////////////////////
/// \brief DoubleSlider::DoubleSlider
/// \param parent
///

DoubleSlider::DoubleSlider(QWidget *parent) :
    QSlider(parent) {
}

// DoubleSlider VALUES CHANGE

void DoubleSlider::setScaledDoubleValue(double givenValue) {
    setValue(static_cast<int>((givenValue/amplitude)*maximum()));
    // emit doubleValueChanged((static_cast<double>(givenValue)/maximum())*amplitude);
}
void DoubleSlider::setInversedScaledIntValue(int givenValue) {
    emit doubleValueChanged((static_cast<double>(givenValue)/maximum())*amplitude);
}

// DoubleSlider ATTRIBUTES ACCESS

void DoubleSlider::setAmplitude(double givenValue) {
    amplitude = givenValue;
}
double DoubleSlider::getAmplitude(void) {
    return amplitude;
}

// SLOTS
void DoubleSlider::inverse(void) {
    setProperty("inversed", true);
    style()->unpolish(this);
    style()->polish(this);
    QWidget::update();
}

void DoubleSlider::normal(void) {
    setProperty("inversed", false);
    style()->unpolish(this);
    style()->polish(this);
    QWidget::update();
}



/////////////////////////////////////////////////////////////
/// \brief SlidersSpinboxGroup::SlidersSpinboxGroup
/// \param indButtonRow
/// \param indCol
/// \param tab_controllers
/// \param parent
///



SlidersSpinboxGroup::SlidersSpinboxGroup(unsigned int indSliderRow,
                                         unsigned int indSliderCol,
                                         DisplayType displayType,
                                         std::vector<std::vector<ControllerData>> &tab_controllers,
                                         bool is_pulse,
                                         AssociatedDisplayType givenAssociatedDisplayType,
                                         QWidget *parent)
    : QGroupBox("", parent) {
    labelHighlightIn = "";
    valueHighlightIn = 0.f;
    hasValueHighlightIn = false;
    labelUDPOut = "";
    midiInChannel = 0;
    midiOutControl = 0;
    // onOffPrevious = false;
    valueFromUDPMessage = false;
    highlight = false;
    indLEDBarDataRow = indSliderRow;
    indLEDBarDataCol = indSliderCol;
    associatedLEDBar = NULL;
    associatedBox = NULL;
    associatedDisplayType = givenAssociatedDisplayType;
    slider = NULL;
    spinBox = NULL;

    if(!is_pulse || (is_pulse && tab_controllers[indSliderRow][indSliderCol].getHasPulseControl())) {
        isActive = true;
        slider = new DoubleSlider();
        slider->setOrientation(Qt::Vertical);
        slider->setFocusPolicy(Qt::StrongFocus);
        slider->setTickPosition(QSlider::TicksBothSides);
        slider->setTickInterval(25);
        slider->setSingleStep(1);
        slider->setAmplitude(127);
        slider->setValue(0);

        spinBox = new QDoubleSpinBox();
        spinBox->setDecimals(3);
        spinBox->setFocusPolicy(Qt::StrongFocus);
        spinBox->setValue(0.);
        // spinBox->setMaximum(0.);
        // spinBox->setFixedWidth(30);

        connect(slider, SIGNAL(sliderMoved(int)), slider, SLOT(setInversedScaledIntValue(int)));
        connect(slider, SIGNAL(doubleValueChanged(double)), spinBox, SLOT(setValue(double)));
        connect(spinBox, SIGNAL(valueChanged(double)), slider, SLOT(setScaledDoubleValue(double)));
        connect(spinBox, SIGNAL(valueChanged(double)), this, SLOT(sendUDPMessage(double)));

        //    SLIDER                            SPINBOX                     THIS
        //    sliderMoved
        //    -> setInversedScaledIntValue
        //       -> doubleValueChanged
        //          ->                          setValue
        //                                      -> valueChanged
        //                                         ->                       sendUDPMessage
        //    setScaledDoubleValue                 <-
        //    -> setValue

        //    THIS                             SLIDER                       SPINBOX
        //    setIntValueFromMIDI
        //    ->                               setInversedScaledIntValue
        //    setDoubleValueFromUDP
        //    ->                                                            setValue

        connect(slider, SIGNAL(sliderPressed(void)), slider, SLOT(inverse(void)));
        connect(slider, SIGNAL(sliderReleased(void)), slider, SLOT(normal(void)));

        QVBoxLayout *slidersLayout = new QVBoxLayout;
        slidersLayout->addWidget(slider);
        slidersLayout->addWidget(spinBox);

        if(tab_controllers.size() <= indSliderRow) {
            qDebug() << "Number of controllers " << int(tab_controllers.size())
                     << " does not correspond to the number of sliders rows " << indSliderRow;
            return;
        }
        if(tab_controllers[indSliderRow].size() <= indSliderCol) {
            qDebug() << "In row " << indSliderRow << " Number of controllers " << int(tab_controllers[indSliderRow].size())
                     << " does not correspond to the number of sliders cols " << indSliderCol
                     << " (" << tab_controllers[indSliderRow][0].get_labelUDPIn() << ")";
            exit(0);
        }

        setTitle(tab_controllers[indSliderRow][indSliderCol].get_titleController());
        setObjectName(tab_controllers[indSliderRow][indSliderCol].get_labelObject());
        setMinimum(0);
        setMaximum(127);
        //    setMaximumWidth(70);
        //    setMaximumWidth(70);
        setAmplitude(tab_controllers[indSliderRow][indSliderCol].get_amplitude());
        setLabelUDPIn(tab_controllers[indSliderRow][indSliderCol].get_labelUDPIn(),is_pulse,
                      indSliderRow, indSliderCol, displayType);
        QStringList list = tab_controllers[indSliderRow][indSliderCol].get_labelHighlightIn().split(QRegExp("\\s+"), QString::SkipEmptyParts);
        if(list.size() >= 1) {
            setLabelHighlightIn(list.at(0),
                                indSliderRow, indSliderCol, displayType);
        }
        if(list.size() >= 2) {
            valueHighlightIn = list.at(1).toFloat();
            hasValueHighlightIn = true;
//            qDebug() << "slider " << indSliderRow << " " << indSliderCol
//                     << " (" << labelHighlightIn << ") (" << valueHighlightIn << ")";
        }
        else {
            valueHighlightIn = 0.f;
            hasValueHighlightIn = false;
        }
        setlabelUDPOut(tab_controllers[indSliderRow][indSliderCol].get_labelUDPOut(), is_pulse);
        setMidiIn(tab_controllers[indSliderRow][indSliderCol].get_channelMidiIn());
        setMidiOut(tab_controllers[indSliderRow][indSliderCol].get_controlMidiOut());

        setLayout(slidersLayout);
    }
    else {
//        QVBoxLayout *boxLayout = new QVBoxLayout;
//        boxLayout->addWidget(new QFrame());
//        setLayout(boxLayout);

        isActive = false;
        setTitle("");
        setObjectName("VOID");
    }

    setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    setMinimumWidth(80);
    setMinimumHeight(100);
}

// SlidersSpinboxGroup VALUES CHANGE

void SlidersSpinboxGroup::setIntValueFromMIDI(int givenValue) {
    if(!isActive) {
        return;
    }
    valueFromUDPMessage = false;
    slider->setInversedScaledIntValue(givenValue);
    //    qDebug() << "set value from MIDI (" << midiInChannel << ") " << (static_cast<double>(givenValue)/slider->maximum())*slider->getAmplitude(); // << std::endl;
    emit(slider->sliderPressed());
    pg_window->lastTimePressedSlider = QTime::currentTime();
    if(pg_window->lastPressedSlider && pg_window->lastPressedSlider != slider) {
        emit(pg_window->lastPressedSlider->sliderReleased());
    }
    pg_window->lastPressedSlider = slider;
}

void SlidersSpinboxGroup::setDoubleValueFromUDP(double givenValue) {
    if(!isActive) {
        return;
    }
    valueFromUDPMessage = true;
    //    qDebug() << "set value from UDP " << givenValue; // << std::endl;
    spinBox->setValue(givenValue);
    slider->setScaledDoubleValue(givenValue);
}

void SlidersSpinboxGroup::setHighlight(float givenValue) {
    if(!isActive) {
        return;
    }
    if(hasValueHighlightIn) {
        highlight = (givenValue == valueHighlightIn);
    }
    else {
        highlight = (givenValue != 0.);
    }
    slider->setProperty("highlight", highlight);
    style()->unpolish(slider);
    style()->polish(slider);
    QWidget::update();
}
bool SlidersSpinboxGroup::getHighlight(void) {
    return highlight;
}
int SlidersSpinboxGroup::getIndLEDBarDataRow(void) {
    return indLEDBarDataRow;
}
int SlidersSpinboxGroup::getIndLEDBarDataCol(void) {
    return indLEDBarDataCol;
}

void SlidersSpinboxGroup::setAssociatedLEDBar(LEDBar *givenLEDBar) {
    if(!isActive) {
        return;
    }
    associatedLEDBar = givenLEDBar;
    if(associatedLEDBar) {
        connect(slider, SIGNAL(sliderPressed(void)), associatedLEDBar, SLOT(inverse(void)));
        connect(slider, SIGNAL(sliderReleased(void)), associatedLEDBar, SLOT(normal(void)));
    }
}
void SlidersSpinboxGroup::setAssociatedBox(Box *givenBox) {
    if(!isActive) {
        return;
    }
    associatedBox = givenBox;
    if(associatedBox) {
        connect(slider, SIGNAL(sliderPressed(void)), associatedBox, SLOT(inverse(void)));
        connect(slider, SIGNAL(sliderReleased(void)), associatedBox, SLOT(normal(void)));
    }
}
AssociatedDisplayType SlidersSpinboxGroup::getAssociatedDisplayType(void) {
    return associatedDisplayType;
}

// SlidersSpinboxGroup MESSAGES EMISSION

void SlidersSpinboxGroup::sendUDPMessage(double givenValue) {
    if(!isActive) {
        return;
    }
    // only sends UDP message if not modified through incoming UDP message
    if(valueFromUDPMessage) {
        valueFromUDPMessage = false;
        return;
    }
    if(labelUDPOut == "/pen_radius") {
        if(givenValue < 1.f) {
            givenValue = 1.f;
        }
    }
    QString message = QString("%1 %2").arg(labelUDPOut).arg(givenValue);
    pg_window->udpInOut->pg_store_message_udp( &message );
    if(labelUDPOut == "/pen_radius_pressure_coef") {
        message = QString("%1 %2").arg("/pen_radius_angleHor_coef").arg(givenValue);
        pg_window->udpInOut->pg_store_message_udp( &message );
        message = QString("%1 %2").arg("/pen_radius_angleVer_coef").arg(givenValue);
        pg_window->udpInOut->pg_store_message_udp( &message );
    }
}

// SlidersSpinboxGroup ATTRIBUTES ACCESS

void SlidersSpinboxGroup::setMinimum(int givenValue) {
    if(!isActive) {
        return;
    }
    slider->setMinimum(givenValue);
    spinBox->setMinimum(givenValue);
}

void SlidersSpinboxGroup::setMaximum(int givenValue) {
    if(!isActive) {
        return;
    }
    slider->setMaximum(givenValue);
}

void SlidersSpinboxGroup::setAmplitude(double givenValue) {
    if(!isActive) {
        return;
    }
    slider->setAmplitude(givenValue);
    spinBox->setMaximum(givenValue);
    spinBox->setSingleStep(givenValue/100.0);
    spinBox->setDecimals(std::max(0,-int(floor(log10(givenValue)))) + 2);
}

void SlidersSpinboxGroup::setLabelUDPIn(QString givenLabel, bool is_pulse,
                                        int indSliderRow, int indSliderCol,
                                        DisplayType displayType) {
    labelUDPIn = givenLabel;
    if(is_pulse && !(givenLabel.isEmpty())) {
        int j = givenLabel.indexOf("_can", 0);
        if(j != -1) {
            labelUDPIn.insert(j,"_pulse");
        }
    }
    if(labelUDPIn != "XXXX") {
        // qDebug() << "label udp in " << labelUDPIn.toStdString(); // << std::endl;
//        if(hashSliderUDPIn.contains(labelUDPIn)) {
//            // qDebug() << "Duplicate Slider label UDPIn " << labelUDPIn; // << std::endl;
//            //    exit(0);
//        }
        hashSliderUDPIn.insert(labelUDPIn, displayType);
        hashSliderUDPIn.insert(labelUDPIn, indSliderCol);
        hashSliderUDPIn.insert(labelUDPIn, indSliderRow);
    }
}
QString SlidersSpinboxGroup::getLabelUDPIn(void) {
    return labelUDPIn;
}

void SlidersSpinboxGroup::setLabelHighlightIn(QString givenLabel,
                                              int indSliderRow, int indSliderCol, DisplayType displayType) {
    labelHighlightIn = givenLabel;
    if(labelHighlightIn != "XXXX") {
//        if(hashSliderHighlightIn.contains(labelHighlightIn)) {
//            qDebug() << "Duplicate Slider label HighlightIn " << labelHighlightIn; // << std::endl;
//            exit(0);
//        }
        hashSliderHighlightIn.insert(labelHighlightIn, displayType);
        hashSliderHighlightIn.insert(labelHighlightIn, indSliderCol);
        hashSliderHighlightIn.insert(labelHighlightIn, indSliderRow);
    }
}
QString SlidersSpinboxGroup::getLabelHighlightIn(void) {
    return labelHighlightIn;
}

void SlidersSpinboxGroup::setlabelUDPOut(QString givenLabel, bool is_pulse) {
    if(!is_pulse) {
        labelUDPOut = givenLabel;
    }
    else {
        if(!(givenLabel.isEmpty())) {
            labelUDPOut = givenLabel + "_pulse";
            // qDebug() << "label udp out " << labelUDPOut; // << std::endl;
        }
    }
}
QString SlidersSpinboxGroup::getlabelUDPOut(void) {
    return labelUDPOut;
}

void SlidersSpinboxGroup::setMidiIn(int givenMidi) {
    midiInChannel = givenMidi;
}
int SlidersSpinboxGroup::getMidiIn(void) {
    return midiInChannel;
}

void SlidersSpinboxGroup::setMidiOut(int givenMidi) {
    midiOutControl = givenMidi;
}
int SlidersSpinboxGroup::getMidiOut(void) {
    return midiOutControl;
}
bool SlidersSpinboxGroup::getIsActive(void) {
    return isActive;
}

/////////////////////////////////////////////////////////////
/// \brief SingleButton::SingleButton
/// \param indButton
/// \param tab_buttonsControllers
/// \param parent
///
SingleButton::SingleButton(unsigned int indButtonRow,
                           unsigned int indButtonCol,
                           DisplayType givenButtonDisplayType,
                           std::vector<std::vector<ControllerData>> &tab_buttonsControllers,
                           QWidget *parent)
    : QPushButton("", parent) {
    if(tab_buttonsControllers.size() <= indButtonRow) {
        qDebug() << "Number of controllers " << int(tab_buttonsControllers.size())
                 << " does not correspond to the number of button rows " << indButtonRow;
        return;
    }
    if(tab_buttonsControllers[indButtonRow].size() <= indButtonCol) {
        qDebug() << "Number of controllers " << int(tab_buttonsControllers[indButtonRow].size())
                 << " does not correspond to the number of button rows " << indButtonCol;
        return;
    }
    init(&tab_buttonsControllers[indButtonRow][indButtonCol],
         indButtonRow, indButtonCol, givenButtonDisplayType);
}

void SingleButton::init(ControllerData *givenData,
                        unsigned int indButtonRow,
                        unsigned int indButtonCol,
                        DisplayType givenButtonDisplayType) {
    labelHighlightIn = "";
    valueHighlightIn = 0.f;
    hasValueHighlightIn = false;
    labelUDPOut = "";
    labelText = "";
    midiInControl = 0;
    midiOutControl = 0;
    highlight = false;
    highlightPrevious = true;
    associatedSingleLabel = NULL;
    buttonDisplayType = givenButtonDisplayType;

    setFocusPolicy(Qt::StrongFocus);
    //    setMinimumWidth(70);
    //    setMaximumWidth(70);

    connect(this, SIGNAL(pressed(void)), this, SLOT(handleEvent(void)));
    connect(this, SIGNAL(pressed(void)), this, SLOT(sendUDPMessage(void)));

    setText(givenData->get_titleController());
    setObjectName(givenData->get_labelObject());
    QStringList list = givenData->get_labelHighlightIn().split(QRegExp("\\s+"), QString::SkipEmptyParts);
    if(list.size() >= 1) {
        setLabelHighlightIn(list.at(0),
                            indButtonRow, indButtonCol, givenButtonDisplayType);
    }
    if(list.size() >= 2) {
        valueHighlightIn = list.at(1).toFloat();
        hasValueHighlightIn = true;
    }
    else {
        valueHighlightIn = 0.f;
        hasValueHighlightIn = false;
    }

    setlabelUDPOut(givenData->get_labelUDPOut());
    setLabelText(givenData->get_titleController());
    setMidiIn(givenData->get_controlMidiIn());
    setMidiOut(givenData->get_controlMidiOut());
    setHighlight(0.f);
    setUDPvalue(givenData->get_amplitude());

    setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    setMinimumWidth(90);
    setMinimumHeight(22);
    setMaximumHeight(25);

    shortcut = NULL;
    if(givenData->get_contolKey() != "") {
        // qDebug() << "shortcut " << givenData->get_contolKey();
        shortcut = new QShortcut(QKeySequence(givenData->get_contolKey()), this);
        connect(shortcut, SIGNAL(activated(void)), this, SLOT(animateClick(void)));
    }
}

// SingleButton VALUES CHANGE

void SingleButton::setHighlight(float givenValue) {
    if(hasValueHighlightIn) {
        highlight = (givenValue == valueHighlightIn);
    }
    else {
        highlight = (givenValue != 0.);
    }
    sendMidiLightingMessage();

    setProperty("highlight", highlight);
    style()->unpolish(this);
    style()->polish(this);
    QWidget::update();
}
void SingleButton::setHighlightOff(void) {
    highlight = false;
    sendMidiLightingMessage();

    setProperty("highlight", highlight);
    style()->unpolish(this);
    style()->polish(this);
    QWidget::update();
}
bool SingleButton::getHighlight(void) {
    return highlight;
}

// SingleButton MESSAGES EMISSION

void SingleButton::sendUDPMessage(void) {
    if(labelUDPOut != "XXXX") {
        QString message = QString("%1 %2").arg(labelUDPOut).arg(float(getUDPvalue()));
        pg_window->udpInOut->pg_store_message_udp( &message );
    }
}

void SingleButton::sendMidiLightingMessage(bool forceEmission) {
    if(!midiOutControl || (!forceEmission && highlight == highlightPrevious)
            ||!pg_window || !pg_window->midiInOut) {
        return;
    }
    pg_window->midiInOut->sendMidiLightingMessage(
                buttonDisplayType,
                midiOutControl ,
                highlight );
    highlightPrevious = highlight;
}

void SingleButton::sendMidiOnOffLightingMessage(bool onOff) {
    if(!midiOutControl ||!pg_window || !pg_window->midiInOut) {
        return;
    }
    pg_window->midiInOut->sendMidiLightingMessage(
                buttonDisplayType,
                midiOutControl ,
                onOff );
    highlightPrevious = onOff;
}

// handling buttons which only act on the interface
void SingleButton::handleEvent(void) {
    // qDebug() << "mindiInControl " << midiInControl;
    // tabulation
    if(midiInControl == 25 && labelText.compare("PULSE") == 0) {
        if(pg_window->getCurrentTabWindow() == _Beat_display
                || pg_window->getCurrentTabWindow() == _Mixer_display
                || pg_window->getCurrentTabWindow() == _FX_display
                || pg_window->getCurrentTabWindow() == _Paths_display
#ifdef TVW
                || pg_window->getCurrentTabWindow() == _TVW_display
#endif
                ) {
            pg_window->setCurrentTabWindow(pg_window->getCurrentTabWindow() + 1);
        }
        else if(pg_window->getCurrentTabWindow() != _Config_display) {
            pg_window->setCurrentTabWindow(pg_window->getCurrentTabWindow() - 1);
        }
    }
    else if(midiInControl == 26 && labelText.compare("TAB") == 0) {
        if(pg_window->getCurrentTabWindow() == _Beat_display
                || pg_window->getCurrentTabWindow() == _Beat_Pulse_display) {
            pg_window->setCurrentTabWindow(_Mixer_display);
        }
        else if(pg_window->getCurrentTabWindow() == _Mixer_display
                || pg_window->getCurrentTabWindow() == _Mixer_Pulse_display) {
            pg_window->setCurrentTabWindow(_FX_display);
        }
        else if(pg_window->getCurrentTabWindow() == _FX_display
                || pg_window->getCurrentTabWindow() == _FX_Pulse_display
                || pg_window->getCurrentTabWindow() == _Paths_display
                || pg_window->getCurrentTabWindow() == _Paths_Pulse_display) {
            pg_window->setCurrentTabWindow(_Beat_display);
        }
    }
    else if(midiInControl == 27 && labelText.compare("PATHS") == 0) {
        if(pg_window->getCurrentTabWindow() != _Paths_display
                && pg_window->getCurrentTabWindow() != _Paths_Pulse_display) {
            pg_window->setCurrentTabWindow(_Paths_display);
        }
        else {
                pg_window->setCurrentTabWindow(pg_window->getLastTabWindow());
        }
    }
    else if(midiInControl == 83 && labelText.compare("CONFIG") == 0) {
        if(pg_window->getCurrentTabWindow() != _Config_display) {
            pg_window->setCurrentTabWindow(_Config_display);
        }
        else {
                pg_window->setCurrentTabWindow(pg_window->getLastTabWindow());
        }
    }
//    else if(midiInControl == 83 && labelText.compare("Reconnect") == 0) {
//        //        pg_window->statusBar()->showMessage(tr("Reconnecting"),1000);
//        //        pg_window->UDPreceiver->reconnect();
//    }
}

// SingleButton ATTRIBUTES ACCESS

void SingleButton::setLabelText(QString givenLabel) {
    labelText = givenLabel;
}
QString SingleButton::getLabelText(void) {
    return labelText;
}
void SingleButton::setLabelHighlightIn(QString givenLabel,
                                       int indSliderRow, int indSliderCol, DisplayType displayType) {
    labelHighlightIn = givenLabel;
    if(labelHighlightIn != "XXXX") {
//        if(hashButtonHighlightIn.contains(labelHighlightIn)) {
//            qDebug() << "Duplicate Button label HighlightIn " << labelHighlightIn; // << std::endl;
//            exit(0);
//        }
        hashButtonHighlightIn.insert(labelHighlightIn, displayType);
        hashButtonHighlightIn.insert(labelHighlightIn, indSliderCol);
        hashButtonHighlightIn.insert(labelHighlightIn, indSliderRow);
    }
}
QString SingleButton::getLabelHighlightIn(void) {
    return labelHighlightIn;
}
void SingleButton::setlabelUDPOut(QString givenLabel) {
    labelUDPOut = givenLabel;
}
QString SingleButton::getlabelUDPOut(void) {
    return labelUDPOut;
}

void SingleButton::setMidiIn(int givenMidi) {
    midiInControl = givenMidi;
}
int SingleButton::getMidiIn(void) {
    return midiInControl;
}
void SingleButton::setMidiOut(int givenMidi) {
    midiOutControl = givenMidi;
}
void SingleButton::setAssociatedSingleLabel( SingleLabel *givenSingleLabel) {
    associatedSingleLabel = givenSingleLabel;
    connect(this, SIGNAL(pressed(void)), associatedSingleLabel, SLOT(inverse(void)));
    connect(this, SIGNAL(released(void)), associatedSingleLabel, SLOT(normal(void)));
}
SingleLabel *SingleButton::getAssociatedSingleLabel(void) {
    return associatedSingleLabel;
}


int SingleButton::getMidiOut(void) {
    return midiOutControl;
}
void SingleButton::setUDPvalue(float givenValue) {
    UDPvalue = givenValue;
}
float SingleButton::getUDPvalue(void) {
    return UDPvalue;
}

/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
////// \brief SliderRowOrGrid
////// \param givenTitle
////// \param tab_inControllers
////// \param sliders
////// \param groupIndex
////// \param parent
///

SliderRowOrGrid::SliderRowOrGrid(QString givenTitle,
                                 std::vector<std::vector<ControllerData>> &tab_inControllers,
                                 bool is_pulse,
                                 std::vector<std::vector<SlidersSpinboxGroup*>> &sliders,
                                 DisplayType displayType,
                                 int groupIndex,
                                 int rowIndexIni,
                                 int rowIndexFin,
                                 AssociatedDisplayType givenAssociatedDisplayType,
                                 QWidget *parent)
    : QGroupBox("", parent) {
    title = givenTitle;
    // case of a grid
    if( rowIndexIni >= 0 && rowIndexFin >= 0 ) {
        QGridLayout *gridLayout = new QGridLayout;
        for(unsigned int indSliderRow = (unsigned int)rowIndexIni ; indSliderRow <= (unsigned int)rowIndexFin ; indSliderRow++ ) {
            for(unsigned int indCol = 0 ; indCol < tab_inControllers[indSliderRow].size() ; indCol++) {
                // displays are boxes, not LEDBars
                sliders[indSliderRow][indCol]
                        = new SlidersSpinboxGroup(indSliderRow, indCol, displayType,
                                                  tab_inControllers, is_pulse,
                                                  givenAssociatedDisplayType );
                gridLayout
                        ->addWidget(sliders[indSliderRow][indCol],
                                    indSliderRow - rowIndexIni, indCol);
            }
        }
        setLayout(gridLayout);
    }
    // case of a single row
    else if( groupIndex >= 0) {
        QHBoxLayout *hBoxLayout = new QHBoxLayout;
        for(unsigned int indCol = 0 ; indCol < tab_inControllers[groupIndex].size() ; indCol++) {
            sliders[groupIndex][indCol]
                    = new SlidersSpinboxGroup(groupIndex, indCol, displayType,
                                              tab_inControllers, is_pulse,
                                              givenAssociatedDisplayType);
            hBoxLayout->addWidget(sliders[groupIndex][indCol]);
        }
        setLayout(hBoxLayout);
    }
    setTitle(givenTitle);
    if(givenTitle.isEmpty()) {
        setObjectName("VOID");
    }
}

/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
////// \brief SliderRowOrGrid
////// \param givenTitle
////// \param tab_inControllers
////// \param sliders
////// \param groupIndex
////// \param parent
///

ButtonRowOrGrid::ButtonRowOrGrid(QString givenTitle,
                                 std::vector<std::vector<ControllerData>> &tab_inControllers,
                                 bool is_pulse,
                                 std::vector<std::vector<SingleButton*>> &buttons,
                                 DisplayType givenButtonDisplayType,
                                 int groupIndex,
                                 int rowIndexIni,
                                 int rowIndexFin,
                                 QWidget *parent)
    : QGroupBox("", parent) {
    title = givenTitle;
    // case of a grid
    if( rowIndexIni >= 0 && rowIndexFin >= 0 ) {
        QGridLayout *gridLayout = new QGridLayout;
        for(unsigned int indSliderRow = (unsigned int)rowIndexIni ; indSliderRow <= (unsigned int)rowIndexFin ; indSliderRow++ ) {
            for(unsigned int indCol = 0 ; indCol < tab_inControllers[indSliderRow].size() ; indCol++) {
                if(!is_pulse || (is_pulse && tab_inControllers[indSliderRow][indCol].getHasPulseControl())) {
                    // displays are boxes, not LEDBars
                    buttons[indSliderRow][indCol]
                            = new SingleButton(indSliderRow, indCol, givenButtonDisplayType,
                                               tab_inControllers);
                    gridLayout
                            ->addWidget(buttons[indSliderRow][indCol],
                                        indSliderRow - rowIndexIni, indCol);
                }
            }
        }
        setLayout(gridLayout);
    }
    // case of a column
    else if( rowIndexIni >= 0 && rowIndexFin < 0 ) {
        // SIDE BUTTON GRID 3 x 1
        QVBoxLayout *columnLayout = new QVBoxLayout;
        for(unsigned int indButtonCol = 0 ; indButtonCol < tab_inControllers[rowIndexIni].size() ; indButtonCol++) {
            if(!is_pulse || (is_pulse && tab_inControllers[rowIndexIni][indButtonCol].getHasPulseControl())) {
                buttons[rowIndexIni][indButtonCol]
                        = new SingleButton(rowIndexIni, indButtonCol, givenButtonDisplayType,
                                           tab_inControllers);
                columnLayout
                        ->addWidget(buttons[rowIndexIni][indButtonCol]);
            }
        }
        setLayout(columnLayout);
    }
    // case of a single row
    else if( groupIndex >= 0) {
        QHBoxLayout *hBoxLayout = new QHBoxLayout;
        for(unsigned int indCol = 0 ; indCol < tab_inControllers[groupIndex].size() ; indCol++) {
            if(!is_pulse || (is_pulse && tab_inControllers[groupIndex][indCol].getHasPulseControl())) {
                buttons[groupIndex][indCol]
                        = new SingleButton(groupIndex, indCol, givenButtonDisplayType,
                                           tab_inControllers);
                hBoxLayout->addWidget(buttons[groupIndex][indCol]);
            }
        }
        setLayout(hBoxLayout);
    }
    setTitle(givenTitle);
    if(givenTitle.isEmpty()) {
        setObjectName("VOID");
    }
}


/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
// GRAPHS TAB
QGridLayout * GraphicalControllers::create_BeatTabControllers(bool is_pulse) {
    /////////////////////////////////////////////////////////
    // GRAPHS SLIDERS
    sliderRows
            = std::vector<SliderRowOrGrid*>(_MaxSlidersBeatData);
    for(unsigned int ind = 0 ; ind < _MaxSlidersBeatData ; ind++ ) {
        sliderRows[ind] = NULL;
    }
    // ALL THE SLIDERS AS DEFINED IN THE DATA
    verticalSliders
            = std::vector<std::vector<SlidersSpinboxGroup*>>(SlidersBeatData.size());
    for(unsigned int indRow = 0 ; indRow < SlidersBeatData.size(); indRow++) {
        verticalSliders[indRow] = std::vector<SlidersSpinboxGroup*>(SlidersBeatData[indRow].size());
        for(unsigned int indCol = 0 ; indCol < SlidersBeatData[indRow].size(); indCol++) {
            verticalSliders[indRow][indCol] = NULL;
        }
    }

    // ALL THE SLIDER ROWS AS DEFINED IN THE DATA
    // FLASH GRID 3x3
    sliderRows[_FlashGridRowCASlidersBeatData]
            = new SliderRowOrGrid(SlidersBeatData_verticalSlidersRowTitles[_FlashGridRowCASlidersBeatData],
                                  SlidersBeatData, is_pulse,
                                  verticalSliders,
                                  (is_pulse?_Beat_Pulse_display:_Beat_display),
                                  -1, _FlashGridRowCASlidersBeatData, _FlashGridRowBGSlidersBeatData, _Box);
    sliderRows[_FlashGridRowPartSlidersBeatData] = sliderRows[_FlashGridRowCASlidersBeatData];
    sliderRows[_FlashGridRowBGSlidersBeatData] = sliderRows[_FlashGridRowCASlidersBeatData];
    // all the other rows
    for(unsigned int ind = _PixelAccCentSlidersBeatData ; ind < _MaxSlidersBeatData ; ind++ ) {
        if(ind != _FlashGridRowCASlidersBeatData
                && ind != _FlashGridRowPartSlidersBeatData
                && ind != _FlashGridRowBGSlidersBeatData) {
            sliderRows[ind]
                    = new SliderRowOrGrid(SlidersBeatData_verticalSlidersRowTitles[ind],
                                          SlidersBeatData, is_pulse,
                                          verticalSliders,
                                          (is_pulse?_Beat_Pulse_display:_Beat_display),
                                          ind, -1, -1);
        }
    }

    /////////////////////////////////////////////////////////
    // GRAPHS BUTTONS
    buttonRows
            = std::vector<ButtonRowOrGrid*>(_MaxButtonsBeatData);
    for(unsigned int ind = 0 ; ind < _MaxButtonsBeatData ; ind++ ) {
        buttonRows[ind] = NULL;
    }
    // ALL THE BUTTONS AS DEFINED IN THE DATA
    mainButtons
            = std::vector<std::vector<SingleButton*>>(ButtonsBeatData.size());
    for(unsigned int indRow = 0 ; indRow < ButtonsBeatData.size(); indRow++) {
        mainButtons[indRow] = std::vector<SingleButton*>(ButtonsBeatData[indRow].size());
        for(unsigned int indCol = 0 ; indCol < ButtonsBeatData[indRow].size(); indCol++) {
            mainButtons[indRow][indCol] = NULL;
        }
    }
    // ALL THE BUTTON ROWS AS DEFINED IN THE DATA
    for(unsigned int ind = 0 ; ind < _MaxButtonsBeatData ; ind++ ) {
        if(ind != _sideButtonsBeatData) {
            buttonRows[ind]
                    = new ButtonRowOrGrid(ButtonsBeatData_ButtonTitles[ind],
                                          ButtonsBeatData, is_pulse,
                                          mainButtons,
                                          (is_pulse?_Beat_Pulse_display:_Beat_display), ind, -1, -1);
            buttonRows[ind]->setMinimumHeight(60);
        }
        // the side buttons
        else {
            buttonRows[ind]
                    = new ButtonRowOrGrid(ButtonsBeatData_ButtonTitles[ind],
                                          ButtonsBeatData, is_pulse,
                                          mainButtons,
                                          (is_pulse?_Beat_Pulse_display:_Beat_display), -1, ind, -1);
        }
    }

    //////////////////////////////////////////////////////////////
    // PUTTING BRICKS TOGETHER

    QGridLayout *allContollersLayout = new QGridLayout;
    allContollersLayout->addWidget(sliderRows[_FlashGridRowCASlidersBeatData], 0, 0, 3, 6);
    allContollersLayout->addWidget(sliderRows[_PixelAccCentSlidersBeatData], 0, 6, 1, 2);
    allContollersLayout->addWidget(buttonRows[_sideButtonsBeatData], 0, 8, 3, 1);
    allContollersLayout->addWidget(sliderRows[_PixelAccCoefSlidersBeatData], 1, 6, 1, 2);
    allContollersLayout->addWidget(sliderRows[_XXXXSlidersBeatData], 2, 6, 1, 2);
    allContollersLayout->addWidget(buttonRows[_penBWButtonsBeatData], 3, 0, 1, 1);
    allContollersLayout->addWidget(buttonRows[_penPaletteButtonsBeatData], 3, 1, 1, 5);
    allContollersLayout->addWidget(buttonRows[_pixelModeButtonsBeatData], 3, 6, 1, 2);
    allContollersLayout->addWidget(buttonRows[_repopBWButtonsBeatData], 4, 0, 1, 1);
    allContollersLayout->addWidget(buttonRows[_penBrushButtonsBeatData], 4, 1, 1, 5);
    allContollersLayout->addWidget(buttonRows[_FlashVideoButtonsBeatData], 4, 6, 1, 2);
    allContollersLayout->addWidget(sliderRows[_RadiusColorSlidersBeatData], 5, 0, 1, 5);
    allContollersLayout->addWidget(sliderRows[_XXXXBottomSlidersBeatData], 5, 5, 1, 3);
    allContollersLayout->addWidget(sliderRows[_MasterBlendSlidersBeatData], 5, 8, 1, 1);
    setLayout(allContollersLayout);
    return allContollersLayout;
}

/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
// Mixer TAB
QGridLayout * GraphicalControllers::create_MixerTabControllers(
        bool is_pulse
        ) {
    /////////////////////////////////////////////////////////
    // Mixer SLIDERS
    sliderRows
            = std::vector<SliderRowOrGrid*>(_MaxSlidersMixerData);
    for(unsigned int ind = 0 ; ind < _MaxSlidersMixerData ; ind++ ) {
        sliderRows[ind] = NULL;
    }
    // ALL THE SLIDERS AS DEFINED IN THE DATA
    verticalSliders
            = std::vector<std::vector<SlidersSpinboxGroup*>>(SlidersMixerData.size());
    for(unsigned int indRow = 0 ; indRow < SlidersMixerData.size(); indRow++) {
        verticalSliders[indRow] = std::vector<SlidersSpinboxGroup*>(SlidersMixerData[indRow].size());
        for(unsigned int indCol = 0 ; indCol < SlidersMixerData[indRow].size(); indCol++) {
            verticalSliders[indRow][indCol] = NULL;
        }
    }
    // ALL THE SLIDER ROWS AS DEFINED IN THE DATA
    // all the other rows
    for(unsigned int ind = 0 ; ind < _MaxSlidersMixerData ; ind++ ) {
            sliderRows[ind]
                    = new SliderRowOrGrid(SlidersMixerData_verticalSlidersRowTitles[ind],
                                          SlidersMixerData, is_pulse,
                                          verticalSliders,
                                          (is_pulse?_Mixer_Pulse_display:_Mixer_display),
                                          ind, -1, -1);
    }

    /////////////////////////////////////////////////////////
    // Mixer BUTTONS
    buttonRows
            = std::vector<ButtonRowOrGrid*>(_MaxButtonsMixerData);
    for(unsigned int ind = 0 ; ind < _MaxButtonsMixerData ; ind++ ) {
        buttonRows[ind] = NULL;
    }
    // ALL THE BUTTONS AS DEFINED IN THE DATA
    mainButtons
            = std::vector<std::vector<SingleButton*>>(ButtonsMixerData.size());
    for(unsigned int indRow = 0 ; indRow < ButtonsMixerData.size(); indRow++) {
        mainButtons[indRow] = std::vector<SingleButton*>(ButtonsMixerData[indRow].size());
        for(unsigned int indCol = 0 ; indCol < ButtonsMixerData[indRow].size(); indCol++) {
            mainButtons[indRow][indCol] = NULL;
        }
    }
    // ALL THE BUTTON ROWS AS DEFINED IN THE DATA
    for(unsigned int ind = 0 ; ind < _MaxButtonsMixerData ; ind++ ) {
        if(ind != _sideButtonsMixerData) {
            buttonRows[ind]
                    = new ButtonRowOrGrid(ButtonsMixerData_ButtonTitles[ind],
                                          ButtonsMixerData, is_pulse,
                                          mainButtons,
                                          (is_pulse?_Mixer_Pulse_display:_Mixer_display), ind, -1, -1);
            buttonRows[ind]->setMinimumHeight(60);
        }
        // the side buttons
        else {
            buttonRows[ind]
                    = new ButtonRowOrGrid(ButtonsMixerData_ButtonTitles[ind],
                                          ButtonsMixerData, is_pulse,
                                          mainButtons,
                                          (is_pulse?_Mixer_Pulse_display:_Mixer_display), -1, ind, -1);
        }
    }

    //////////////////////////////////////////////////////////////
    // PUTTING BRICKS TOGETHER

    QGridLayout *allContollersLayout = new QGridLayout;
    allContollersLayout->addWidget(sliderRows[_MixingSlidersMixerData], 0, 0, 1, 6);
    allContollersLayout->addWidget(sliderRows[_CameraSlidersMixerData], 0, 6, 1, 2);
    allContollersLayout->addWidget(buttonRows[_sideButtonsMixerData], 0, 8, 3, 1);
    allContollersLayout->addWidget(sliderRows[_MasterSlidersMixerData], 1, 0, 1, 6);
    allContollersLayout->addWidget(sliderRows[_MovieMediaFreqSlidersMixerData], 1, 6, 1, 2);
    allContollersLayout->addWidget(sliderRows[_RepopSlidersMixerData], 2, 0, 1, 4);
    allContollersLayout->addWidget(sliderRows[_XXXXSlidersMixerData], 2, 4, 1, 2);
    allContollersLayout->addWidget(sliderRows[_PhotoSlidersMixerData], 2, 6, 1, 2);
    allContollersLayout->addWidget(buttonRows[_TracksButtonsMixerData], 3, 0, 1, 6);
    allContollersLayout->addWidget(buttonRows[_TrackCopyButtonsMixerData], 3, 6, 1, 2);
    allContollersLayout->addWidget(buttonRows[_decaySignButtonMixersData], 4, 0, 1, 6);
    allContollersLayout->addWidget(buttonRows[_freezeBottomButtonsMixerData], 4, 6, 1, 2);
    allContollersLayout->addWidget(sliderRows[_DecaySlidersMixerData], 5, 0, 1, 6);
    allContollersLayout->addWidget(sliderRows[_EchoSlidersMixerData], 5, 6, 1, 2);
    allContollersLayout->addWidget(sliderRows[_MasterBlendSlidersMixerData], 5, 8, 1, 1);
    return allContollersLayout;
}

/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
// FX TAB
QGridLayout * GraphicalControllers::create_FXTabControllers(
        bool is_pulse
        ) {
    /////////////////////////////////////////////////////////
    // FX SLIDERS
    sliderRows
            = std::vector<SliderRowOrGrid*>(_MaxSlidersFXData);
    for(unsigned int ind = 0 ; ind < _MaxSlidersFXData ; ind++ ) {
        sliderRows[ind] = NULL;
    }
    // ALL THE SLIDERS AS DEFINED IN THE DATA
    verticalSliders
            = std::vector<std::vector<SlidersSpinboxGroup*>>(SlidersFXData.size());
    for(unsigned int indRow = 0 ; indRow < SlidersFXData.size(); indRow++) {
        verticalSliders[indRow] = std::vector<SlidersSpinboxGroup*>(SlidersFXData[indRow].size());
        for(unsigned int indCol = 0 ; indCol < SlidersFXData[indRow].size(); indCol++) {
            verticalSliders[indRow][indCol] = NULL;
        }
    }

    // ALL THE SLIDER ROWS AS DEFINED IN THE DATA
    for(unsigned int ind = 0 ; ind < _MaxSlidersFXData ; ind++ ) {
            sliderRows[ind]
                    = new SliderRowOrGrid(SlidersFXData_verticalSlidersRowTitles[ind],
                                          SlidersFXData, is_pulse,
                                          verticalSliders,
                                          (is_pulse?_FX_Pulse_display:_FX_display),
                                          ind, -1, -1);
    }

    /////////////////////////////////////////////////////////
    // FX BUTTONS
    buttonRows
            = std::vector<ButtonRowOrGrid*>(_MaxButtonsFXData);
    for(unsigned int ind = 0 ; ind < _MaxButtonsFXData ; ind++ ) {
        buttonRows[ind] = NULL;
    }
    // ALL THE BUTTONS AS DEFINED IN THE DATA
    mainButtons
            = std::vector<std::vector<SingleButton*>>(ButtonsFXData.size());
    for(unsigned int indRow = 0 ; indRow < ButtonsFXData.size(); indRow++) {
        mainButtons[indRow] = std::vector<SingleButton*>(ButtonsFXData[indRow].size());
        for(unsigned int indCol = 0 ; indCol < ButtonsFXData[indRow].size(); indCol++) {
            mainButtons[indRow][indCol] = NULL;
        }
    }
    // ALL THE BUTTON ROWS AS DEFINED IN THE DATA
    for(unsigned int ind = 0 ; ind < _MaxButtonsFXData ; ind++ ) {
        if(ind != _sideButtonsFXData) {
            buttonRows[ind]
                    = new ButtonRowOrGrid(ButtonsFXData_ButtonTitles[ind],
                                          ButtonsFXData, is_pulse,
                                          mainButtons,
                                          (is_pulse?_FX_Pulse_display:_FX_display), ind, -1, -1);
            buttonRows[ind]->setMinimumHeight(60);
        }
        // the side buttons
        else {
            buttonRows[ind]
                    = new ButtonRowOrGrid(ButtonsFXData_ButtonTitles[ind],
                                          ButtonsFXData, is_pulse,
                                          mainButtons,
                                          (is_pulse?_FX_Pulse_display:_FX_display), -1, ind, -1);
        }
    }

    //////////////////////////////////////////////////////////////
    // PUTTING BRICKS TOGETHER
    QGridLayout *allContollersLayout = new QGridLayout;
    allContollersLayout->addWidget(sliderRows[_CAParamsSlidersFXData], 0, 0, 1, 8);
    allContollersLayout->addWidget(buttonRows[_sideButtonsFXData], 0, 8, 3, 1);
    allContollersLayout->addWidget(sliderRows[_CameraParamsSlidersFXData], 1, 0, 1, 8);
    allContollersLayout->addWidget(sliderRows[_noiseSlidersFXData], 2, 0, 1, 5);
    allContollersLayout->addWidget(sliderRows[_VideoSatFXData], 2, 5, 1, 1);
    allContollersLayout->addWidget(sliderRows[_MediaSobelSlidersFXData], 2, 6, 1, 2);
    allContollersLayout->addWidget(buttonRows[_CATypeButtonFXsData], 3, 0 , 1, 5);
    allContollersLayout->addWidget(buttonRows[_mediaPlusButtonsFXData], 3, 5, 1, 2);
    allContollersLayout->addWidget(buttonRows[_cameraCumulButtonsFXData], 3, 7, 1, 1);
    allContollersLayout->addWidget(buttonRows[_sensorsButtonsFXData], 4, 0, 1, 4);
    allContollersLayout->addWidget(buttonRows[_noiseTypeButtonFXsData], 4, 4, 1, 1);
    allContollersLayout->addWidget(buttonRows[_micOnOffButtonsFXData], 4, 5, 1, 1);
    allContollersLayout->addWidget(buttonRows[_soundSampleButtonsFXData], 4, 6, 1, 2);
    allContollersLayout->addWidget(sliderRows[_BeatSlidersFXData], 5, 0, 1, 3);
    allContollersLayout->addWidget(sliderRows[_SoundSlidersFXData], 5, 3, 1, 5);
    allContollersLayout->addWidget(sliderRows[_SoundVolumeSlidersFXData], 5, 8, 1, 1);
    return allContollersLayout;
}

/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
// PATHS TAB
QGridLayout * GraphicalControllers::create_PathsTabControllers(
        bool is_pulse
        ) {
    /////////////////////////////////////////////////////////
    // PATHS SLIDERS
    sliderRows
            = std::vector<SliderRowOrGrid*>(_MaxSlidersPathsData);
    for(unsigned int ind = 0 ; ind < _MaxSlidersPathsData ; ind++ ) {
        sliderRows[ind] = NULL;
    }
    // ALL THE SLIDERS AS DEFINED IN THE DATA
    verticalSliders
            = std::vector<std::vector<SlidersSpinboxGroup*>>(SlidersPathsData.size());
    for(unsigned int indRow = 0 ; indRow < SlidersPathsData.size(); indRow++) {
        verticalSliders[indRow] = std::vector<SlidersSpinboxGroup*>(SlidersPathsData[indRow].size());
        for(unsigned int indCol = 0 ; indCol < SlidersPathsData[indRow].size(); indCol++) {
            verticalSliders[indRow][indCol] = NULL;
        }
    }
    // ALL THE SLIDER ROWS AS DEFINED IN THE DATA
    for(unsigned int ind = 0 ; ind < _MaxSlidersPathsData ; ind++ ) {
        sliderRows[ind]
                = new SliderRowOrGrid(SlidersPathsData_verticalSlidersRowTitles[ind],
                                      SlidersPathsData, is_pulse,
                                      verticalSliders,
                                      (is_pulse?_Paths_Pulse_display:_Paths_display),
                                      ind, -1, -1);
    }


    /////////////////////////////////////////////////////////
    // PATHS BUTTONS
    buttonRows
            = std::vector<ButtonRowOrGrid*>(_MaxButtonsPathsData);
    for(unsigned int ind = 0 ; ind < _MaxButtonsPathsData ; ind++ ) {
        buttonRows[ind] = NULL;
    }
    // ALL THE BUTTONS AS DEFINED IN THE DATA
    mainButtons
            = std::vector<std::vector<SingleButton*>>(ButtonsPathsData.size());
    for(unsigned int indRow = 0 ; indRow < ButtonsPathsData.size(); indRow++) {
        mainButtons[indRow] = std::vector<SingleButton*>(ButtonsPathsData[indRow].size());
        for(unsigned int indCol = 0 ; indCol < ButtonsPathsData[indRow].size(); indCol++) {
            mainButtons[indRow][indCol] = NULL;
        }
    }
    // ALL THE BUTTON ROWS AS DEFINED IN THE DATA
    for(unsigned int ind = 0 ; ind < _MaxButtonsPathsData ; ind++ ) {
        if(ind != _sideButtonsPathsData) {
            buttonRows[ind]
                    = new ButtonRowOrGrid(ButtonsPathsData_ButtonTitles[ind],
                                          ButtonsPathsData, is_pulse,
                                          mainButtons,
                                          (is_pulse?_Paths_Pulse_display:_Paths_display), ind, -1, -1);
            buttonRows[ind]->setMinimumHeight(51);
        }
        // the side buttons
        else {
            buttonRows[ind]
                    = new ButtonRowOrGrid(ButtonsPathsData_ButtonTitles[ind],
                                          ButtonsPathsData, is_pulse,
                                          mainButtons,
                                          (is_pulse?_Paths_Pulse_display:_Paths_display), -1, ind, -1);
        }
    }


    //////////////////////////////////////////////////////////////
    // PUTTING BRICKS TOGETHER

    QGridLayout *allContollersLayout = new QGridLayout;
    allContollersLayout->addWidget(buttonRows[_partMoveButtonsPathsData], 0, 0, 1, 4);
    allContollersLayout->addWidget(buttonRows[_partStrokeButtonsPathsData], 0, 4, 1, 4);
    allContollersLayout->addWidget(buttonRows[_sideButtonsPathsData], 0, 8, 8, 1);
    allContollersLayout->addWidget(buttonRows[_partExitButtonsPathsData], 1, 0, 1, 4);
    allContollersLayout->addWidget(buttonRows[_partColorButtonsPathsData], 1, 4, 1, 4);
    allContollersLayout->addWidget(buttonRows[_partInitButtonsPathsData], 2, 0, 1, 8);
    allContollersLayout->addWidget(buttonRows[_partRepopPathButtonsPathsData], 3, 0, 1, 8);
    allContollersLayout->addWidget(buttonRows[_partFollowPathButtonsPathsData], 4, 0, 1, 8);
    allContollersLayout->addWidget(buttonRows[_partRepulsePathButtonsPathsData], 5, 0, 1, 8);
    allContollersLayout->addWidget(buttonRows[_pathRecordButtonsPathsData], 6, 0, 1, 8);
    allContollersLayout->addWidget(buttonRows[_pathReplayButtonsPathsData], 7, 0, 1, 8);
    allContollersLayout->addWidget(buttonRows[_setUpButtonsPathsData], 8, 0, 1, 3);
    allContollersLayout->addWidget(buttonRows[_clearButtonsPathsData], 8, 3, 1, 5);
    allContollersLayout->addWidget(sliderRows[_ParticleRadiusSlidersPathsData], 9, 0, 1, 3);
    allContollersLayout->addWidget(sliderRows[_ParticleFluiditySlidersPathsData], 9, 3, 1, 3);
    allContollersLayout->addWidget(sliderRows[_ParticleNoiseSlidersPathsData], 9, 6, 1, 3);
    return allContollersLayout;
}

#ifdef TVW
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
// TVW TAB
QGridLayout * GraphicalControllers::create_TVWTabControllers(
        bool is_pulse
        ) {
    /////////////////////////////////////////////////////////
    // TVW SLIDERS
    sliderRows
            = std::vector<SliderRowOrGrid*>(_MaxSlidersTVWData);
    for(unsigned int ind = 0 ; ind < _MaxSlidersTVWData ; ind++ ) {
        sliderRows[ind] = NULL;
    }
    // ALL THE SLIDERS AS DEFINED IN THE DATA
    verticalSliders
            = std::vector<std::vector<SlidersSpinboxGroup*>>(SlidersTVWData.size());
    for(unsigned int indRow = 0 ; indRow < SlidersTVWData.size(); indRow++) {
        verticalSliders[indRow] = std::vector<SlidersSpinboxGroup*>(SlidersTVWData[indRow].size());
        for(unsigned int indCol = 0 ; indCol < SlidersTVWData[indRow].size(); indCol++) {
            verticalSliders[indRow][indCol] = NULL;
        }
    }

    // ALL THE SLIDER ROWS AS DEFINED IN THE DATA
    for(unsigned int ind = 0 ; ind < _MaxSlidersTVWData ; ind++ ) {
            sliderRows[ind]
                    = new SliderRowOrGrid(SlidersTVWData_verticalSlidersRowTitles[ind],
                                          SlidersTVWData, is_pulse,
                                          verticalSliders,
                                          (is_pulse?_TVW_Pulse_display:_TVW_display),
                                          ind, -1, -1);
    }

    /////////////////////////////////////////////////////////
    // TVW BUTTONS
    buttonRows
            = std::vector<ButtonRowOrGrid*>(_MaxButtonsTVWData);
    for(unsigned int ind = 0 ; ind < _MaxButtonsTVWData ; ind++ ) {
        buttonRows[ind] = NULL;
    }
    // ALL THE BUTTONS AS DEFINED IN THE DATA
    mainButtons
            = std::vector<std::vector<SingleButton*>>(ButtonsTVWData.size());
    for(unsigned int indRow = 0 ; indRow < ButtonsTVWData.size(); indRow++) {
        mainButtons[indRow] = std::vector<SingleButton*>(ButtonsTVWData[indRow].size());
        for(unsigned int indCol = 0 ; indCol < ButtonsTVWData[indRow].size(); indCol++) {
            mainButtons[indRow][indCol] = NULL;
        }
    }
    // ALL THE BUTTON ROWS AS DEFINED IN THE DATA
    for(unsigned int ind = 0 ; ind < _MaxButtonsTVWData ; ind++ ) {
        if(ind != _sideButtonsTVWData) {
            buttonRows[ind]
                    = new ButtonRowOrGrid(ButtonsTVWData_ButtonTitles[ind],
                                          ButtonsTVWData, is_pulse,
                                          mainButtons,
                                          (is_pulse?_TVW_Pulse_display:_TVW_display), ind, -1, -1);
            buttonRows[ind]->setMinimumHeight(60);
        }
        // the side buttons
        else {
            buttonRows[ind]
                    = new ButtonRowOrGrid(ButtonsTVWData_ButtonTitles[ind],
                                          ButtonsTVWData, is_pulse,
                                          mainButtons,
                                          (is_pulse?_TVW_Pulse_display:_TVW_display), -1, ind, -1);
        }
    }

    //////////////////////////////////////////////////////////////
    // PUTTING BRICKS TOGETHER
    QGridLayout *allContollersLayout = new QGridLayout;
    allContollersLayout->addWidget(sliderRows[_XXXX1TVWData], 0, 0, 1, 8);
    allContollersLayout->addWidget(buttonRows[_sideButtonsTVWData], 0, 8, 3, 1);
    allContollersLayout->addWidget(sliderRows[_PixelsTVWData], 1, 0, 1, 8);
    allContollersLayout->addWidget(sliderRows[_ContrastTVWData], 2, 0, 1, 8);
    allContollersLayout->addWidget(buttonRows[_PenColorTVWData], 3, 0, 1, 7);
    allContollersLayout->addWidget(buttonRows[_XXXX3TVWData], 3, 7, 1, 1);
    allContollersLayout->addWidget(buttonRows[_PenGreyButtonTVWsData], 4, 0, 1, 1);
    allContollersLayout->addWidget(buttonRows[_DecayButtonTVWsData], 4, 1, 1, 2);
    allContollersLayout->addWidget(buttonRows[_flashCameraTVWData], 4, 3, 1, 5);
    allContollersLayout->addWidget(sliderRows[_PenSlidersTVWData], 5, 0, 1, 1);
    allContollersLayout->addWidget(sliderRows[_DecaySlidersTVWData], 5, 1, 1, 2);
    allContollersLayout->addWidget(sliderRows[_CameraSlidersTVWData], 5, 3, 1, 2);
    allContollersLayout->addWidget(sliderRows[_SoundSlidersTVWData], 5, 5, 1, 3);
    allContollersLayout->addWidget(sliderRows[_MasterBlendSlidersTVWData], 5, 8, 1, 1);
    return allContollersLayout;
}
#endif

/////////////////////////////////////////////////////////////
/// \brief GraphicalControllers::GraphicalControllers
/// \param givenType
/// \param parent
///

GraphicalControllers::GraphicalControllers(DisplayType givenType,
                                           bool is_pulse,
                                           QWidget *parent)
    : QWidget(parent) {
    /////////////////////////////////////////////////////////
    // TAB CONSTRUCTION
    QGridLayout *allContollersLayout = NULL;
    switch(givenType) {
    case _Beat_display:
        // GRAPHS TAB
        allContollersLayout = create_BeatTabControllers(is_pulse);
        break;
    case _Mixer_display:
        // Mixer TAB
        allContollersLayout = create_MixerTabControllers(is_pulse);
        break;
    case _FX_display:
        // FX TAB
        allContollersLayout = create_FXTabControllers(is_pulse);
        break;
    case _Paths_display:
        // PATHS TAB
        allContollersLayout = create_PathsTabControllers(is_pulse);
        break;
#ifdef TVW
    case _TVW_display:
        // TVW TAB
        allContollersLayout = create_TVWTabControllers(is_pulse);
        break;
#endif
    default:
        break;
    }
    if(allContollersLayout) {
        setLayout(allContollersLayout);
    }
}

