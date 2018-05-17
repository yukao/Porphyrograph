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

#include "pg-interface-atelPort-window.h"
#include "pg-interface-atelPort-udp.h"
#include "pg-interface-atelPort-control.h"

#include <QtWidgets>
#include <QTime>
#include <QtDebug>

#include <vector>

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
                                         std::vector<std::vector<inOutControllerData>> tab_controllers,
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
        printf("Number of controllers %d does not correspond to the number of sliders rows %d\n",
               int(tab_controllers.size()),indSliderRow);
        return;
    }
    if(tab_controllers[indSliderRow].size() <= indSliderCol) {
        printf("In row %d Number of controllers %d does not correspond to the number of sliders cols %d (%s)\n",
               indSliderRow, int(tab_controllers[indSliderRow].size()),indSliderCol,
               tab_controllers[indSliderRow][0].get_labelUDPIn().toStdString().c_str());
        exit(0);
    }

    setTitle(tab_controllers[indSliderRow][indSliderCol].get_titleController());
    setObjectName(tab_controllers[indSliderRow][indSliderCol].get_labelObject());
    setMinimum(0);
    setMaximum(127);
//    setMaximumWidth(70);
//    setMaximumWidth(70);
    setAmplitude(tab_controllers[indSliderRow][indSliderCol].get_amplitude());
    setLabelUDPIn(tab_controllers[indSliderRow][indSliderCol].get_labelUDPIn());
    QStringList list = tab_controllers[indSliderRow][indSliderCol].get_labelHighlightIn().split(QRegExp("\\s+"), QString::SkipEmptyParts);
    if(list.size() >= 1) {
        labelHighlightIn = list.at(0);
    }
    if(list.size() >= 2) {
        valueHighlightIn = list.at(1).toFloat();
        hasValueHighlightIn = true;
        // printf("slider %d %d (%s) (%.2f)\n",indSliderRow,indSliderCol,labelHighlightIn.toStdString().c_str(),valueHighlightIn);
    }
    else {
        valueHighlightIn = 0.f;
        hasValueHighlightIn = false;
    }
    setlabelUDPOut(tab_controllers[indSliderRow][indSliderCol].get_labelUDPOut());
    setMidiIn(tab_controllers[indSliderRow][indSliderCol].get_channelMidiIn());
    setMidiOut(tab_controllers[indSliderRow][indSliderCol].get_controlMidiOut());
    setLayout(slidersLayout);

    setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    setMinimumWidth(80);
    setMinimumHeight(100);
}

// SlidersSpinboxGroup VALUES CHANGE

void SlidersSpinboxGroup::setIntValueFromMIDI(int givenValue) {
    valueFromUDPMessage = false;
    slider->setInversedScaledIntValue(givenValue);
    //    std::cout << "set value from MIDI (" << midiInChannel << ") " << (static_cast<double>(givenValue)/slider->maximum())*slider->getAmplitude() << std::endl;
    emit(slider->sliderPressed());
    pg_window->lastTimePressedSlider = QTime::currentTime();
    if(pg_window->lastPressedSlider && pg_window->lastPressedSlider != slider) {
        emit(pg_window->lastPressedSlider->sliderReleased());
    }
    pg_window->lastPressedSlider = slider;
}

void SlidersSpinboxGroup::setDoubleValueFromUDP(double givenValue) {
    valueFromUDPMessage = true;
    //    std::cout << "set value from UDP " << givenValue << std::endl;
    spinBox->setValue(givenValue);
    slider->setScaledDoubleValue(givenValue);
}

void SlidersSpinboxGroup::setHighlight(float givenValue) {
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
    associatedLEDBar = givenLEDBar;
    if(associatedLEDBar) {
        connect(slider, SIGNAL(sliderPressed(void)), associatedLEDBar, SLOT(inverse(void)));
        connect(slider, SIGNAL(sliderReleased(void)), associatedLEDBar, SLOT(normal(void)));
    }
}
void SlidersSpinboxGroup::setAssociatedBox(Box *givenBox) {
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
    slider->setMinimum(givenValue);
    spinBox->setMinimum(givenValue);
}

void SlidersSpinboxGroup::setMaximum(int givenValue) {
    slider->setMaximum(givenValue);
}

void SlidersSpinboxGroup::setAmplitude(double givenValue) {
    slider->setAmplitude(givenValue);
    spinBox->setMaximum(givenValue);
    spinBox->setSingleStep(givenValue/100.0);
    spinBox->setDecimals(std::max(0,-int(floor(log10(givenValue)))) + 2);
}

void SlidersSpinboxGroup::setLabelUDPIn(QString givenLabel) {
    labelUDPIn = givenLabel;
}
QString SlidersSpinboxGroup::getLabelUDPIn(void) {
    return labelUDPIn;
}

void SlidersSpinboxGroup::setLabelHighlightIn(QString givenLabel) {
    labelHighlightIn = givenLabel;
}
QString SlidersSpinboxGroup::getLabelHighlightIn(void) {
    return labelHighlightIn;
}

void SlidersSpinboxGroup::setlabelUDPOut(QString givenLabel) {
    labelUDPOut = givenLabel;
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

/////////////////////////////////////////////////////////////
/// \brief SingleButton::SingleButton
/// \param indButton
/// \param tab_buttonsControllers
/// \param parent
///
SingleButton::SingleButton(unsigned int indButton,
                           std::vector<inOutControllerData> tab_buttonsControllers,
                           DisplayType givenButtonDisplayType,
                           QWidget *parent)
    : QPushButton("", parent) {
    if(tab_buttonsControllers.size() <= indButton) {
        printf("Number of controllers %d does not correspond to the number of button cols %d\n",
               int(tab_buttonsControllers.size()),indButton);
        return;
    }
    init(&tab_buttonsControllers[indButton],
         givenButtonDisplayType);
}

SingleButton::SingleButton(unsigned int indButtonRow,
                           unsigned int indButtonCol,
                           std::vector<std::vector<inOutControllerData>> tab_buttonsControllers,
                           DisplayType givenButtonDisplayType,
                           QWidget *parent)
    : QPushButton("", parent) {
    if(tab_buttonsControllers.size() <= indButtonRow) {
        printf("Number of controllers %d does not correspond to the number of button rows %d\n",
               int(tab_buttonsControllers.size()),indButtonRow);
        return;
    }
    if(tab_buttonsControllers[indButtonRow].size() <= indButtonCol) {
        printf("Number of controllers %d does not correspond to the number of button cols %d\n",
               int(tab_buttonsControllers[indButtonRow].size()),indButtonCol);
        return;
    }
    init(&tab_buttonsControllers[indButtonRow][indButtonCol],
         givenButtonDisplayType);
}

void SingleButton::init(inOutControllerData *givenData,
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
        labelHighlightIn = list.at(0);
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
    // printf("mindiInControl %d\n",midiInControl);
    // tabulation
    if(midiInControl == 26 && labelText.compare("TAB") == 0) {
        if(pg_window->currentTabWindow == _Graphs_display) {
            pg_window->setCurrentTabWindow(_Tracks_display);
        }
        else if(pg_window->currentTabWindow == _Tracks_display) {
            pg_window->setCurrentTabWindow(_Paths_display);
        }
        else {
            pg_window->setCurrentTabWindow(_Graphs_display);
        }
    }
}

// SingleButton ATTRIBUTES ACCESS

void SingleButton::setLabelText(QString givenLabel) {
    labelText = givenLabel;
}
QString SingleButton::getLabelText(void) {
    return labelText;
}
void SingleButton::setLabelHighlightIn(QString givenLabel) {
    labelHighlightIn = givenLabel;
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
// GRAPHS TAB
void GraphicalControllers::create_graphsTabControllers(void) {
    //////////////////////////////////////////////////////////////
    // SLIDERS

    QGroupBox *flashSliders_graphs;
    QGroupBox *pixelSliders_graphs;
    QGroupBox *pulseCAFlashSliders_graphs;
    QGroupBox *pulseSliders_graphs;
    QGroupBox *CAParam_graphs;
    QGroupBox *pixelpulseSliders_graphs;
    QGroupBox *radiusColorSliders_graphs;
    QGroupBox *beatSliders_graphs;

    // Empty slider area
    // QSpacerItem *emptyControl = new QSpacerItem(70,120);

    /////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////
    // GRAPHS TAB
    // FLASH GRID 3 x 5
    flashSliders_graphs = new QGroupBox;
    QGridLayout *flashSlidersLayout_graphs = new QGridLayout;
    for(unsigned int indSliderRow = _FlashGridRowCAInOutData ; indSliderRow <= _FlashGridRowBGInOutData ; indSliderRow++ ) {
        for(unsigned int indCol = 0 ; indCol < graphs_mainSlidersData[indSliderRow].size() ; indCol++) {
            // displays are boxes, not LEDBars
            verticalSliders_graphs[indSliderRow][indCol]
                    = new SlidersSpinboxGroup(indSliderRow, indCol,
                                              graphs_mainSlidersData, _Box );
            flashSlidersLayout_graphs
                    ->addWidget(verticalSliders_graphs[indSliderRow][indCol],
                                indSliderRow, indCol);
        }
    }
    flashSliders_graphs->setLayout(flashSlidersLayout_graphs);
    flashSliders_graphs->setTitle("FLASH");

    // PIXEL GRID 1 x 3
    pixelSliders_graphs = new QGroupBox;
    QHBoxLayout *pixelSlidersLayout_graphs = new QHBoxLayout;
    for(unsigned int indCol = 0 ; indCol < graphs_mainSlidersData[_PixelInOutData].size() ; indCol++) {
        verticalSliders_graphs[_PixelInOutData][indCol]
                = new SlidersSpinboxGroup(_PixelInOutData, indCol, graphs_mainSlidersData);
        pixelSlidersLayout_graphs
                ->addWidget(verticalSliders_graphs[_PixelInOutData][indCol]);
    }
    pixelSliders_graphs->setLayout(pixelSlidersLayout_graphs);
    pixelSliders_graphs->setTitle("PIXELS");
    // PULSE GRID 1 x 2
    pulseCAFlashSliders_graphs = new QGroupBox;
    QHBoxLayout *pulseCAFlashSlidersLayout_graphs = new QHBoxLayout;
    for(unsigned int indCol = 0 ; indCol < graphs_mainSlidersData[_PulseCAFlashInOutData].size() ; indCol++) {
        verticalSliders_graphs[_PulseCAFlashInOutData][indCol]
                = new SlidersSpinboxGroup(_PulseCAFlashInOutData, indCol, graphs_mainSlidersData);
        pulseCAFlashSlidersLayout_graphs
                ->addWidget(verticalSliders_graphs[_PulseCAFlashInOutData][indCol]);
    }
    pulseCAFlashSliders_graphs->setLayout(pulseCAFlashSlidersLayout_graphs);
    pulseCAFlashSliders_graphs->setTitle("PULSE FLASH CA");

    // PIXEL + PULSE GRID 1 x 3
    pixelPulseSliders_graphs = new QGroupBox;
    QHBoxLayout *pixelPulseLayout_graphs = new QHBoxLayout;
    pixelPulseLayout_graphs->addWidget(pixelSliders_graphs);
    pixelPulseLayout_graphs->addWidget(pulseCAFlashSliders_graphs);
    pixelPulseSliders_graphs->setLayout(pixelPulseLayout_graphs);
    pixelPulseSliders_graphs->setTitle("");
    pixelPulseSliders_graphs->setObjectName("VOID");

    // PULSE GRID 1 x 3
    pulseSliders_graphs = new QGroupBox;
    QGridLayout *pulseSlidersLayout_graphs = new QGridLayout;
    for(unsigned int indCol = 0 ; indCol < graphs_mainSlidersData[_PulsePartInOutData].size() ; indCol++) {
        verticalSliders_graphs[_PulsePartInOutData][indCol]
                = new SlidersSpinboxGroup(_PulsePartInOutData, indCol, graphs_mainSlidersData);
        pulseSlidersLayout_graphs
                ->addWidget(verticalSliders_graphs[_PulsePartInOutData][indCol],
                            _PulsePartInOutData, indCol);
    }
    pulseSliders_graphs->setLayout(pulseSlidersLayout_graphs);
    pulseSliders_graphs->setTitle("PULSE PART");

    // CA PARAM GRID 1 x 3
    CAParam_graphs = new QGroupBox;
    QGridLayout *CAParamLayout_graphs = new QGridLayout;
    for(unsigned int indCol = 0 ; indCol < graphs_mainSlidersData[_CAParamsInOutData].size() ; indCol++) {
        verticalSliders_graphs[_CAParamsInOutData][indCol]
                = new SlidersSpinboxGroup(_CAParamsInOutData, indCol, graphs_mainSlidersData);
        CAParamLayout_graphs
                ->addWidget(verticalSliders_graphs[_CAParamsInOutData][indCol],
                            _CAParamsInOutData, indCol);
    }
    CAParam_graphs->setLayout(CAParamLayout_graphs);
    CAParam_graphs->setTitle("CA PARAMS");

    // PIXEL + PULSE GRID 3 x 3
    pixelpulseSliders_graphs = new QGroupBox;
    QVBoxLayout *pixelpulseSlidersLayout_graphs = new QVBoxLayout;
    pixelpulseSlidersLayout_graphs->addWidget(pixelPulseSliders_graphs);
    pixelpulseSlidersLayout_graphs->addWidget(pulseSliders_graphs);
    pixelpulseSlidersLayout_graphs->addWidget(CAParam_graphs);
    pixelpulseSliders_graphs->setLayout(pixelpulseSlidersLayout_graphs);
    pixelpulseSliders_graphs->setTitle("");
    pixelpulseSliders_graphs->setObjectName("VOID");

    // FLASH + PIXEL + PULSE GRID 3 x 8
    flashpixelpulseSliders_graphs = new QGroupBox;
    QHBoxLayout *flashpixelpulseSlidersLayout_graphs = new QHBoxLayout;
    flashpixelpulseSlidersLayout_graphs->addWidget(flashSliders_graphs);
    flashpixelpulseSlidersLayout_graphs->addWidget(pixelpulseSliders_graphs);
    flashpixelpulseSliders_graphs->setLayout(flashpixelpulseSlidersLayout_graphs);
    flashpixelpulseSliders_graphs->setTitle("");
    flashpixelpulseSliders_graphs->setObjectName("VOID");
    flashpixelpulseSliders_graphs->setMinimumHeight(400);

    // RADIUS-COLOR GRID 1 x 5
    radiusColorSliders_graphs = new QGroupBox;
    QHBoxLayout *radiusColorSlidersLayout_graphs = new QHBoxLayout;
    for(unsigned int indCol = 0 ; indCol < graphs_mainSlidersData[_RadiusColorInOutData].size() ; indCol++) {
        verticalSliders_graphs[_RadiusColorInOutData][indCol]
                = new SlidersSpinboxGroup(_RadiusColorInOutData, indCol, graphs_mainSlidersData);
        radiusColorSlidersLayout_graphs
                ->addWidget(verticalSliders_graphs[_RadiusColorInOutData][indCol]);
    }
    radiusColorSliders_graphs->setLayout(radiusColorSlidersLayout_graphs);
    radiusColorSliders_graphs->setTitle("RADIUS-COLOR");

    // BEAT GRID 1 x 3
    beatSliders_graphs = new QGroupBox;
    QHBoxLayout *beatSlidersLayout_graphs = new QHBoxLayout;
    for(unsigned int indCol = 0 ; indCol < graphs_mainSlidersData[_BeatInOutData].size() ; indCol++) {
        verticalSliders_graphs[_BeatInOutData][indCol]
                = new SlidersSpinboxGroup(_BeatInOutData, indCol, graphs_mainSlidersData);
        beatSlidersLayout_graphs
                ->addWidget(verticalSliders_graphs[_BeatInOutData][indCol]);
    }
    beatSliders_graphs->setLayout(beatSlidersLayout_graphs);
    beatSliders_graphs->setTitle("BEAT");

    // RADIUS-COLOR + BEAT GRID 1 x 8
    radiusbeatSliders_graphs = new QGroupBox;
    QHBoxLayout *radiusbeatSlidersLayout_graphs = new QHBoxLayout;
    radiusbeatSlidersLayout_graphs->addWidget(radiusColorSliders_graphs);
    radiusbeatSlidersLayout_graphs->addWidget(beatSliders_graphs);
    radiusbeatSliders_graphs->setLayout(radiusbeatSlidersLayout_graphs);
    radiusbeatSliders_graphs->setTitle("");
    radiusbeatSliders_graphs->setObjectName("VOID");

    // PIXEL RADIUS SLIDER 1 x 1
    pixelRadiusSliders_graphs = new QGroupBox;
    QHBoxLayout *pixelRaidusSlidersLayout_graphs = new QHBoxLayout;
    verticalSliders_graphs[_PixRadiusInOutData][0]
            = new SlidersSpinboxGroup(_PixRadiusInOutData, 0, graphs_mainSlidersData);
    pixelRaidusSlidersLayout_graphs
            ->addWidget(verticalSliders_graphs[_PixRadiusInOutData][0]);
    pixelRadiusSliders_graphs->setLayout(pixelRaidusSlidersLayout_graphs);
    pixelRadiusSliders_graphs->setTitle("");
    pixelRadiusSliders_graphs->setObjectName("VOID");

    //////////////////////////////////////////////////////////////
    // BUTTONS

    QGroupBox *penBWButton_graphs;
    QGroupBox *penColorButtons_graphs;
    QGroupBox *pixelModeButton_graphs;
    QGroupBox *repopBWButton_graphs;
    QGroupBox *penBrushButtons_graphs;
    QGroupBox *freezeButton_graphs;

    // SIDE BUTTON GRID 3 x 1
    sideButton_graphs = new QGroupBox;
    QVBoxLayout *sideButtonLayout_graphs = new QVBoxLayout;
    for(unsigned int indSliderRow = 0 ; indSliderRow < 3 ; indSliderRow++) {
        sideButtons_graphs[indSliderRow]
                    = new SingleButton(indSliderRow, graphs_sideButtonsData, _Graphs_display);
        sideButtonLayout_graphs
                    ->addWidget(sideButtons_graphs[indSliderRow]);
    }
    sideButton_graphs->setLayout(sideButtonLayout_graphs);
    sideButton_graphs->setTitle("");
    sideButton_graphs->setObjectName("VOID");

    // REPOP B/W BUTTON 1 x 1
    penBWButton_graphs = new QGroupBox;
    QHBoxLayout *penBWButtonLayout_graphs = new QHBoxLayout;
    mainButtons_graphs[0][0]
            = new SingleButton(0, 0, graphs_mainButtonsData, _Graphs_display);
    penBWButtonLayout_graphs
            ->addWidget(mainButtons_graphs[0][0]);
    penBWButton_graphs->setLayout(penBWButtonLayout_graphs);
    penBWButton_graphs->setTitle("PEN B/W");
    penBWButton_graphs->setMinimumHeight(60);

    // PEN COLOR ROW BUTTON 1 x 6
    penColorButtons_graphs = new QGroupBox;
    QHBoxLayout *penColorButtonsLayout_graphs = new QHBoxLayout;
    for(unsigned int indButtonCol = 1 ; indButtonCol < 7 ; indButtonCol++) {
            mainButtons_graphs[0][indButtonCol]
                    = new SingleButton(0, indButtonCol, graphs_mainButtonsData, _Graphs_display);
            penColorButtonsLayout_graphs
                    ->addWidget(mainButtons_graphs[0][indButtonCol]);
    }
    penColorButtons_graphs->setLayout(penColorButtonsLayout_graphs);
    penColorButtons_graphs->setTitle("PEN PALETTE");
    penColorButtons_graphs->setMinimumHeight(60);

    // PIXEL MODE BUTTON 1 x 1
    pixelModeButton_graphs = new QGroupBox;
    QHBoxLayout *pixelModeButtonLayout_graphs = new QHBoxLayout;
    mainButtons_graphs[0][7]
            = new SingleButton(0, 7, graphs_mainButtonsData, _Graphs_display);
    pixelModeButtonLayout_graphs
            ->addWidget(mainButtons_graphs[0][7]);
    pixelModeButton_graphs->setLayout(pixelModeButtonLayout_graphs);
    pixelModeButton_graphs->setTitle("PIXEL Mode");
    pixelModeButton_graphs->setMinimumHeight(60);

    // PEN COLOR + PIXEL MODE 1 x 8
    penColorpixelModeButtons_graphs = new QGroupBox;
    QHBoxLayout *penColorpixelModeButtonsLayout_graphs = new QHBoxLayout;
    penColorpixelModeButtonsLayout_graphs->addWidget(penBWButton_graphs);
    penColorpixelModeButtonsLayout_graphs->addWidget(penColorButtons_graphs);
    penColorpixelModeButtonsLayout_graphs->addWidget(pixelModeButton_graphs);
    penColorpixelModeButtons_graphs->setLayout(penColorpixelModeButtonsLayout_graphs);
    penColorpixelModeButtons_graphs->setTitle("");
    penColorpixelModeButtons_graphs->setObjectName("VOID");


    // REPOP B/W BUTTON 1 x 1
    repopBWButton_graphs = new QGroupBox;
    QHBoxLayout *repopBWButtonLayout_graphs = new QHBoxLayout;
    mainButtons_graphs[1][0]
            = new SingleButton(1, 0, graphs_mainButtonsData, _Graphs_display);
    repopBWButtonLayout_graphs
            ->addWidget(mainButtons_graphs[1][0]);
    repopBWButton_graphs->setLayout(repopBWButtonLayout_graphs);
    repopBWButton_graphs->setTitle("REPOP B/W");
    repopBWButton_graphs->setMinimumHeight(60);

    // PEN BRUSH ROW BUTTON 1 x 6
    penBrushButtons_graphs = new QGroupBox;
    QHBoxLayout *penBrushButtonsLayout_graphs = new QHBoxLayout;
    for(unsigned int indButtonCol = 1 ; indButtonCol < 7 ; indButtonCol++) {
            mainButtons_graphs[1][indButtonCol]
                    = new SingleButton(1, indButtonCol, graphs_mainButtonsData, _Graphs_display);
            penBrushButtonsLayout_graphs
                    ->addWidget(mainButtons_graphs[1][indButtonCol]);
    }
    penBrushButtons_graphs->setLayout(penBrushButtonsLayout_graphs);
    penBrushButtons_graphs->setTitle("PEN BRUSH");
    penBrushButtons_graphs->setMinimumHeight(60);

    // FREEZE BUTTON 1 x 1
    freezeButton_graphs = new QGroupBox;
    QHBoxLayout *freezeButtonLayout_graphs = new QHBoxLayout;
    mainButtons_graphs[1][7]
            = new SingleButton(1, 7, graphs_mainButtonsData, _Graphs_display);
    freezeButtonLayout_graphs
            ->addWidget(mainButtons_graphs[1][7]);
    freezeButton_graphs->setLayout(freezeButtonLayout_graphs);
    freezeButton_graphs->setTitle("FREEZE");
    freezeButton_graphs->setMinimumHeight(60);

    // REPOP BW + PEN BRUSH 1 x 8
    repopBWpenbrushModeButtons_graphs = new QGroupBox;
    QHBoxLayout *repopBWpenbrushModeButtonsLayout_graphs = new QHBoxLayout;
    repopBWpenbrushModeButtonsLayout_graphs->addWidget(repopBWButton_graphs);
    repopBWpenbrushModeButtonsLayout_graphs->addWidget(penBrushButtons_graphs);
    repopBWpenbrushModeButtonsLayout_graphs->addWidget(freezeButton_graphs);
    repopBWpenbrushModeButtons_graphs->setLayout(repopBWpenbrushModeButtonsLayout_graphs);
    repopBWpenbrushModeButtons_graphs->setTitle("");
    repopBWpenbrushModeButtons_graphs->setObjectName("VOID");
}

/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
// TRACKS TAB
void GraphicalControllers::create_tracksTabControllers(void) {
    //////////////////////////////////////////////////////////////
    // SLIDERS

    QGroupBox *weightSliders_tracks;
    QGroupBox *repopSliders_tracks;
    QGroupBox *weightrepopSliders_tracks;
    QGroupBox *mediaSliders_tracks;
    QGroupBox *soundminSlider_tracks;
    QGroupBox *sobelSliders_tracks;
    QGroupBox *soundsobelSliders_tracks;
    QGroupBox *mediasoundsobelSliders_tracks;
    QGroupBox *decaySliders_tracks;
    QGroupBox *soundSliders_tracks;
    QGroupBox *echoSliders_tracks;

    // Empty slider area
    // QSpacerItem *emptyControl = new QSpacerItem(70,120);

    // WEIGHT GRID 2 x 5
    weightSliders_tracks = new QGroupBox;
    QGridLayout *weightSlidersLayout_tracks = new QGridLayout;
    std::vector<unsigned int> numbers = {_MixingInOutData, _MasterInOutData};
    for(unsigned int ind = 0; ind < numbers.size() ; ind++) {
        unsigned int indSliderRow = numbers[ind];
        for(unsigned int indCol = 0 ; indCol < tracks_mainSlidersData[indSliderRow].size() ; indCol++) {
            verticalSliders_tracks[indSliderRow][indCol]
                    = new SlidersSpinboxGroup(indSliderRow, indCol, tracks_mainSlidersData);
                    weightSlidersLayout_tracks
                    ->addWidget(verticalSliders_tracks[indSliderRow][indCol],
                                indSliderRow, indCol);
        }
    }
    weightSliders_tracks->setLayout(weightSlidersLayout_tracks);
    weightSliders_tracks->setTitle("TRACK WEIGHTS");

    // REPOP GRID 1 x 5
    repopSliders_tracks = new QGroupBox;
    QHBoxLayout *repopSlidersLayout_tracks = new QHBoxLayout;
    for(unsigned int indCol = 0 ; indCol < tracks_mainSlidersData[_RepopInOutData].size() ; indCol++) {
        verticalSliders_tracks[_RepopInOutData][indCol]
                = new SlidersSpinboxGroup(_RepopInOutData, indCol, tracks_mainSlidersData);
        repopSlidersLayout_tracks
                ->addWidget(verticalSliders_tracks[_RepopInOutData][indCol]);
    }
    repopSliders_tracks->setLayout(repopSlidersLayout_tracks);
    repopSliders_tracks->setTitle("REPOP");

    // WEIGHT + REPOP GRID 3 x 5
    weightrepopSliders_tracks = new QGroupBox;
    QVBoxLayout *weightrepopSlidersLayout_tracks = new QVBoxLayout;
    weightrepopSlidersLayout_tracks->addWidget(weightSliders_tracks);
    weightrepopSlidersLayout_tracks->addWidget(repopSliders_tracks);
    weightrepopSliders_tracks->setLayout(weightrepopSlidersLayout_tracks);
    weightrepopSliders_tracks->setTitle("");
    weightrepopSliders_tracks->setObjectName("VOID");

    // MEDIA GRID 2 x 3
    mediaSliders_tracks = new QGroupBox;
    QGridLayout *mediaSlidersLayout_tracks = new QGridLayout;
    numbers = {_MediaWeightInOutData, _CamThresholdInOutData, _MediaFreqInOutData};
    for(unsigned int ind = 0; ind < numbers.size() ; ind++) {
        unsigned int indSliderRow = numbers[ind];
        for(unsigned int indCol = 0 ; indCol < tracks_mainSlidersData[indSliderRow].size() ; indCol++) {
            verticalSliders_tracks[indSliderRow][indCol]
                    = new SlidersSpinboxGroup(indSliderRow, indCol, tracks_mainSlidersData);
            if( indSliderRow == _MediaWeightInOutData) {
                mediaSlidersLayout_tracks
                        ->addWidget(verticalSliders_tracks[indSliderRow][indCol],
                                    0, indCol);
            }
            else if( indSliderRow == _CamThresholdInOutData) {
                mediaSlidersLayout_tracks
                        ->addWidget(verticalSliders_tracks[indSliderRow][indCol],
                                    1, indCol);
            }
            else if( indSliderRow == _MediaFreqInOutData) {
                mediaSlidersLayout_tracks
                        ->addWidget(verticalSliders_tracks[indSliderRow][indCol],
                                    1, indCol + 1);
            }
        }
    }
    mediaSliders_tracks->setLayout(mediaSlidersLayout_tracks);
    mediaSliders_tracks->setTitle("MEDIA   WEIGHTS  FREQUENCY");

    // SOUND GRID 1 x 1
    soundminSlider_tracks = new QGroupBox;
    QGridLayout *soundminSliderLayout_tracks = new QGridLayout;
    for(unsigned int indCol = 0 ; indCol < tracks_mainSlidersData[_SoundMinInOutData].size() ; indCol++) {
        verticalSliders_tracks[_SoundMinInOutData][indCol]
                = new SlidersSpinboxGroup(_SoundMinInOutData, indCol, tracks_mainSlidersData);
        soundminSliderLayout_tracks
                ->addWidget(verticalSliders_tracks[_SoundMinInOutData][indCol],
                            _SoundMinInOutData, indCol);
    }
    soundminSlider_tracks->setLayout(soundminSliderLayout_tracks);
    soundminSlider_tracks->setTitle("SOUND");

    // SOBEL GRID 1 x 2
    sobelSliders_tracks = new QGroupBox;
    QGridLayout *sobelSlidersLayout_tracks = new QGridLayout;
    for(unsigned int indCol = 0 ; indCol < tracks_mainSlidersData[_MediaSobelInOutData].size() ; indCol++) {
        verticalSliders_tracks[_MediaSobelInOutData][indCol]
                = new SlidersSpinboxGroup(_MediaSobelInOutData, indCol, tracks_mainSlidersData);
        sobelSlidersLayout_tracks
                ->addWidget(verticalSliders_tracks[_MediaSobelInOutData][indCol],
                            _MediaSobelInOutData, indCol);
    }
    sobelSliders_tracks->setLayout(sobelSlidersLayout_tracks);
    sobelSliders_tracks->setTitle("MEDIA SOBEL");

    // SOUND + SOBEL GRID 1 x 3
    soundsobelSliders_tracks = new QGroupBox;
    QHBoxLayout *soundsobelSlidersLayout_tracks = new QHBoxLayout;
    soundsobelSlidersLayout_tracks->addWidget(soundminSlider_tracks);
    soundsobelSlidersLayout_tracks->addWidget(sobelSliders_tracks);
    soundsobelSliders_tracks->setLayout(soundsobelSlidersLayout_tracks);
    soundsobelSliders_tracks->setTitle("");
    soundsobelSliders_tracks->setObjectName("VOID");

    // MEDIA + SOBEL GRID 3 x 3
    mediasoundsobelSliders_tracks = new QGroupBox;
    QVBoxLayout *mediasoundsobelSlidersLayout_tracks = new QVBoxLayout;
    mediasoundsobelSlidersLayout_tracks->addWidget(mediaSliders_tracks);
    mediasoundsobelSlidersLayout_tracks->addWidget(soundsobelSliders_tracks);
    mediasoundsobelSliders_tracks->setLayout(mediasoundsobelSlidersLayout_tracks);
    mediasoundsobelSliders_tracks->setTitle("");
    mediasoundsobelSliders_tracks->setObjectName("VOID");

    // WEIGHT + REPOP + MEDIA + SOBEL GRID 3 x 8
    weightrepopmediasobelSliders_tracks = new QGroupBox;
    QHBoxLayout *weightrepopmediasobelSlidersLayout_graphs = new QHBoxLayout;
    weightrepopmediasobelSlidersLayout_graphs->addWidget(weightrepopSliders_tracks);
    weightrepopmediasobelSlidersLayout_graphs->addWidget(mediasoundsobelSliders_tracks);
    weightrepopmediasobelSliders_tracks->setLayout(weightrepopmediasobelSlidersLayout_graphs);
    weightrepopmediasobelSliders_tracks->setTitle("");
    weightrepopmediasobelSliders_tracks->setObjectName("VOID");
    weightrepopmediasobelSliders_tracks->setMinimumHeight(400);

    //////////////////////////////////////////////////////////////
    // BUTTONS

    QGroupBox *tracksButtons_tracks;
    QGroupBox *miconoffButton_tracks;
    QGroupBox *soundsampleButton_tracks;
    QGroupBox *decaysignButtons_tracks;
    QGroupBox *emptyButton_tracks;
    QGroupBox *mediaPlusButtons_tracks;

   // TRACKS BUTTON 1 x 5
    tracksButtons_tracks = new QGroupBox;
    QHBoxLayout *tracksButtonsLayout_tracks = new QHBoxLayout;
    for(int indButtonCol = 0 ; indButtonCol < 5 ; indButtonCol++) {
            mainButtons_tracks[0][indButtonCol]
                    = new SingleButton(0, indButtonCol, tracks_mainButtonsData, _Tracks_display);
            tracksButtonsLayout_tracks
                    ->addWidget(mainButtons_tracks[0][indButtonCol]);
    }
    tracksButtons_tracks->setLayout(tracksButtonsLayout_tracks);
    tracksButtons_tracks->setTitle("LAUNCH");
    tracksButtons_tracks->setMinimumHeight(60);

    // SOUND BUTTONS 1 x 3
    soundsampleButton_tracks = new QGroupBox;
    QHBoxLayout *soundsampleButtonLayout_tracks = new QHBoxLayout;
    for(int indButtonCol = 5 ; indButtonCol < 8 ; indButtonCol++) {
        mainButtons_tracks[0][indButtonCol]
                = new SingleButton(0, indButtonCol, tracks_mainButtonsData, _Tracks_display);
        soundsampleButtonLayout_tracks
                ->addWidget(mainButtons_tracks[0][indButtonCol]);
    }
    soundsampleButton_tracks->setLayout(soundsampleButtonLayout_tracks);
    soundsampleButton_tracks->setTitle("RESET");
    soundsampleButton_tracks->setMinimumHeight(60);

    // TRACKS + MEDIA BUTTONS 1 x 8
    tracksmiconoffsoundsampleButtons_tracks = new QGroupBox;
    QHBoxLayout *tracksmiconoffsoundsampleButtonsLayout_tracks = new QHBoxLayout;
    tracksmiconoffsoundsampleButtonsLayout_tracks->addWidget(tracksButtons_tracks);
    tracksmiconoffsoundsampleButtonsLayout_tracks->addWidget(soundsampleButton_tracks);
    tracksmiconoffsoundsampleButtons_tracks->setLayout(tracksmiconoffsoundsampleButtonsLayout_tracks);
    tracksmiconoffsoundsampleButtons_tracks->setTitle("");
    tracksmiconoffsoundsampleButtons_tracks->setObjectName("VOID");

    // DECAY SIGN BUTTON 1 x 5
    decaysignButtons_tracks = new QGroupBox;
    QHBoxLayout *decaysignButtonsLayout_tracks = new QHBoxLayout;
    for(int indButtonCol = 0 ; indButtonCol < 5 ; indButtonCol++) {
            mainButtons_tracks[1][indButtonCol]
                    = new SingleButton(1, indButtonCol, tracks_mainButtonsData, _Tracks_display);
            decaysignButtonsLayout_tracks
                    ->addWidget(mainButtons_tracks[1][indButtonCol]);
    }
    decaysignButtons_tracks->setLayout(decaysignButtonsLayout_tracks);
    decaysignButtons_tracks->setTitle("DECAY SIGN");
    decaysignButtons_tracks->setMinimumHeight(60);

    // MEDIA PLUS BUTTON 1 x 3
    mediaPlusButtons_tracks = new QGroupBox;
    QHBoxLayout *mediaPlusButtonsLayout_tracks = new QHBoxLayout;
    for(int indButtonCol = 5 ; indButtonCol < 8 ; indButtonCol++) {
            mainButtons_tracks[1][indButtonCol]
                    = new SingleButton(1, indButtonCol, tracks_mainButtonsData, _Tracks_display);
            mediaPlusButtonsLayout_tracks
                    ->addWidget(mainButtons_tracks[1][indButtonCol]);
    }
    mediaPlusButtons_tracks->setLayout(mediaPlusButtonsLayout_tracks);
    mediaPlusButtons_tracks->setTitle("MOVIE - PHOTO TRACK +");
    mediaPlusButtons_tracks->setMinimumHeight(60);

    // DECAY SIGN + MEDIA PLUS 1 x 8
    decaysignemptyButtonmediaplusButtons_tracks = new QGroupBox;
    QHBoxLayout *decaysignemptyButtonmediaplusButtonsLayout_tracks = new QHBoxLayout;
    decaysignemptyButtonmediaplusButtonsLayout_tracks->addWidget(decaysignButtons_tracks);
    decaysignemptyButtonmediaplusButtonsLayout_tracks->addWidget(mediaPlusButtons_tracks);
    decaysignemptyButtonmediaplusButtons_tracks->setLayout(decaysignemptyButtonmediaplusButtonsLayout_tracks);
    decaysignemptyButtonmediaplusButtons_tracks->setTitle("");
    decaysignemptyButtonmediaplusButtons_tracks->setObjectName("VOID");

    // DECAY GRID 1 x 5
    decaySliders_tracks = new QGroupBox;
    QHBoxLayout *decaySlidersLayout_tracks = new QHBoxLayout;
    for(unsigned int indCol = 0 ; indCol < tracks_mainSlidersData[_DecayInOutData].size() ; indCol++) {
        verticalSliders_tracks[_DecayInOutData][indCol]
                = new SlidersSpinboxGroup(_DecayInOutData, indCol, tracks_mainSlidersData);
        decaySlidersLayout_tracks
                ->addWidget(verticalSliders_tracks[_DecayInOutData][indCol]);
    }
    decaySliders_tracks->setLayout(decaySlidersLayout_tracks);
    decaySliders_tracks->setTitle("DECAY");

    // SOUND GRID 1 x 1
    soundSliders_tracks = new QGroupBox;
    QHBoxLayout *soundSlidersLayout_tracks = new QHBoxLayout;
    for(unsigned int indCol = 0 ; indCol < tracks_mainSlidersData[_SoundVolInOutData].size() ; indCol++) {
        verticalSliders_tracks[_SoundVolInOutData][indCol]
                = new SlidersSpinboxGroup(_SoundVolInOutData, indCol, tracks_mainSlidersData);
        soundSlidersLayout_tracks
                ->addWidget(verticalSliders_tracks[_SoundVolInOutData][indCol]);
    }
    soundSliders_tracks->setLayout(soundSlidersLayout_tracks);
    soundSliders_tracks->setTitle("BEAT");

    // ECHO GRID 1 x 3
    echoSliders_tracks = new QGroupBox;
    QHBoxLayout *echoSlidersLayout_tracks = new QHBoxLayout;
    for(unsigned int indCol = 0 ; indCol < tracks_mainSlidersData[_EchoInOutData].size() ; indCol++) {
        verticalSliders_tracks[_EchoInOutData][indCol]
                = new SlidersSpinboxGroup(_EchoInOutData, indCol, tracks_mainSlidersData);
        echoSlidersLayout_tracks
                ->addWidget(verticalSliders_tracks[_EchoInOutData][indCol]);
    }
    echoSliders_tracks->setLayout(echoSlidersLayout_tracks);
    echoSliders_tracks->setTitle("SOUND THRESHOLD");

    // DECAY + ECHO SOUND GRID 1 x 8
    decayechosoundSliders_tracks = new QGroupBox;
    QHBoxLayout *decayechosoundSlidersLayout_tracks = new QHBoxLayout;
    decayechosoundSlidersLayout_tracks->addWidget(decaySliders_tracks);
    decayechosoundSlidersLayout_tracks->addWidget(soundSliders_tracks);
    decayechosoundSlidersLayout_tracks->addWidget(echoSliders_tracks);
    decayechosoundSliders_tracks->setLayout(decayechosoundSlidersLayout_tracks);
    decayechosoundSliders_tracks->setTitle("");
    decayechosoundSliders_tracks->setObjectName("VOID");

    // SIDE BUTTON GRID 3 x 1
    sideButton_tracks = new QGroupBox;
    QVBoxLayout *sideButtonLayout_tracks = new QVBoxLayout;
    for(int indButtonCol = 0 ; indButtonCol < 3 ; indButtonCol++) {
            sideButtons_tracks[indButtonCol]
                    = new SingleButton(indButtonCol, tracks_sideButtonsData, _Tracks_display);
            sideButtonLayout_tracks
                    ->addWidget(sideButtons_tracks[indButtonCol]);
    }
    sideButton_tracks->setLayout(sideButtonLayout_tracks);
    sideButton_tracks->setTitle("");
    sideButton_tracks->setObjectName("VOID");

    // MASTER BUTTON 1 x 1
    masterSliders_tracks = new QGroupBox;
    QHBoxLayout *masterSlidersLayout_tracks = new QHBoxLayout;
    verticalSliders_tracks[_MasterBlendInOutData][0]
            = new SlidersSpinboxGroup(_MasterBlendInOutData, 0, tracks_mainSlidersData);
    masterSlidersLayout_tracks
            ->addWidget(verticalSliders_tracks[_MasterBlendInOutData][0]);
    masterSliders_tracks->setLayout(masterSlidersLayout_tracks);
    masterSliders_tracks->setTitle("");
    masterSliders_tracks->setObjectName("VOID");
}

/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
// PATHS TAB
void GraphicalControllers::create_pathsTabControllers(void) {
    QGroupBox *partMoveButtons_paths;
    QGroupBox *partStrokeButtons_paths;
    QGroupBox *partExitButtons_paths;
    QGroupBox *partColorButtons_paths;
    QGroupBox *setUpButtons_paths;
    QGroupBox *CATypeButtons_paths;
    QGroupBox *partRadiusSliders_paths;
    QGroupBox *partAccSliders_paths;
    QGroupBox *partNoiseSliders_paths;

    // PATHS LINE 1
    // partMove BUTTON 1 x 4
    partMoveButtons_paths = new QGroupBox;
    QHBoxLayout *partMoveButtonsLayout_paths = new QHBoxLayout;
    for(int indButtonCol = 0 ; indButtonCol < 4 ; indButtonCol++) {
            mainButtons_paths[0][indButtonCol]
                    = new SingleButton(0, indButtonCol, paths_mainButtonsData, _Paths_display);
            partMoveButtonsLayout_paths
                    ->addWidget(mainButtons_paths[0][indButtonCol]);
    }
    partMoveButtons_paths->setLayout(partMoveButtonsLayout_paths);
    partMoveButtons_paths->setTitle("PART MOVE");
    partMoveButtons_paths->setMinimumHeight(51);

    // partStroke BUTTON 1 x 4
    partStrokeButtons_paths = new QGroupBox;
    QHBoxLayout *partStrokeButtonsLayout_paths = new QHBoxLayout;
    for(int indButtonCol = 4 ; indButtonCol < 8 ; indButtonCol++) {
            mainButtons_paths[0][indButtonCol]
                    = new SingleButton(0, indButtonCol, paths_mainButtonsData, _Paths_display);
            partStrokeButtonsLayout_paths
                    ->addWidget(mainButtons_paths[0][indButtonCol]);
    }
    partStrokeButtons_paths->setLayout(partStrokeButtonsLayout_paths);
    partStrokeButtons_paths->setTitle("PART STROKE");
    partStrokeButtons_paths->setMinimumHeight(51);

    // Paths Line 1 1 x 8
    partLine1Buttons_paths = new QGroupBox;
    QHBoxLayout *partLine1ButtonsLayout_paths = new QHBoxLayout;
    partLine1ButtonsLayout_paths->addWidget(partMoveButtons_paths);
    partLine1ButtonsLayout_paths->addWidget(partStrokeButtons_paths);
    partLine1Buttons_paths->setLayout(partLine1ButtonsLayout_paths);
    partLine1Buttons_paths->setTitle("");
    partLine1Buttons_paths->setObjectName("VOID");


    // PATHS LINE 2
    // partExit BUTTON 1 x 4
    partExitButtons_paths = new QGroupBox;
    QHBoxLayout *partExitButtonsLayout_paths = new QHBoxLayout;
    for(int indButtonCol = 0 ; indButtonCol < 4 ; indButtonCol++) {
            mainButtons_paths[1][indButtonCol]
                    = new SingleButton(1, indButtonCol, paths_mainButtonsData, _Paths_display);
            partExitButtonsLayout_paths
                    ->addWidget(mainButtons_paths[1][indButtonCol]);
    }
    partExitButtons_paths->setLayout(partExitButtonsLayout_paths);
    partExitButtons_paths->setTitle("PART EXIT");
    partExitButtons_paths->setMinimumHeight(51);

    // partColorBis BUTTON 1 x 4
    partColorButtons_paths = new QGroupBox;
    QHBoxLayout *partColorButtonsLayout_paths = new QHBoxLayout;
    for(int indButtonCol = 4 ; indButtonCol < 8 ; indButtonCol++) {
            mainButtons_paths[1][indButtonCol]
                    = new SingleButton(1, indButtonCol, paths_mainButtonsData, _Paths_display);
            partColorButtonsLayout_paths
                    ->addWidget(mainButtons_paths[1][indButtonCol]);
    }
    partColorButtons_paths->setLayout(partColorButtonsLayout_paths);
    partColorButtons_paths->setTitle("PART COLOR");
    partColorButtons_paths->setMinimumHeight(51);

    // Paths Line 2 1 x 8
    partLine2Buttons_paths = new QGroupBox;
    QHBoxLayout *partLine2ButtonsLayout_paths = new QHBoxLayout;
    partLine2ButtonsLayout_paths->addWidget(partExitButtons_paths);
    partLine2ButtonsLayout_paths->addWidget(partColorButtons_paths);
    partLine2Buttons_paths->setLayout(partLine2ButtonsLayout_paths);
    partLine2Buttons_paths->setTitle("");
    partLine2Buttons_paths->setObjectName("VOID");


    // PATHS LINE 3
    // partInit BUTTON 1 x 8
    partInitButtons_paths = new QGroupBox;
    QHBoxLayout *partInitButtonsLayout_paths = new QHBoxLayout;
    for(int indButtonCol = 0 ; indButtonCol < 8 ; indButtonCol++) {
            mainButtons_paths[2][indButtonCol]
                    = new SingleButton(2, indButtonCol, paths_mainButtonsData, _Paths_display);
            partInitButtonsLayout_paths
                    ->addWidget(mainButtons_paths[2][indButtonCol]);
    }
    partInitButtons_paths->setLayout(partInitButtonsLayout_paths);
    partInitButtons_paths->setTitle("PART INIT");
    partInitButtons_paths->setMinimumHeight(51);

//    // Paths Line 3 1 x 8
//    partLine3Buttons_paths = new QGroupBox;
//    QHBoxLayout *partLine3ButtonsLayout_paths = new QHBoxLayout;
//    partLine3ButtonsLayout_paths->addWidget(partInitButtons_paths);
//    partLine3Buttons_paths->setLayout(partLine3ButtonsLayout_paths);
//    partLine3Buttons_paths->setTitle("");
//    partLine3Buttons_paths->setObjectName("VOID");


    // PATHS LINE 4
    // pathtRepop BUTTON 1 x 8
    pathtRepopButtons_paths = new QGroupBox;
    QHBoxLayout *pathtRepopButtonsLayout_paths = new QHBoxLayout;
    for(int indButtonCol = 0 ; indButtonCol < 8 ; indButtonCol++) {
            mainButtons_paths[3][indButtonCol]
                    = new SingleButton(3, indButtonCol, paths_mainButtonsData, _Paths_display);
            pathtRepopButtonsLayout_paths
                    ->addWidget(mainButtons_paths[3][indButtonCol]);
    }
    pathtRepopButtons_paths->setLayout(pathtRepopButtonsLayout_paths);
    pathtRepopButtons_paths->setTitle("PART PATH REPOP");
    pathtRepopButtons_paths->setMinimumHeight(51);

    // PATHS LINE 5
    // pathFollow BUTTON 1 x 8
    pathFollowButtons_paths = new QGroupBox;
    QHBoxLayout *pathFollowButtonsLayout_paths = new QHBoxLayout;
    for(int indButtonCol = 0 ; indButtonCol < 8 ; indButtonCol++) {
            mainButtons_paths[4][indButtonCol]
                    = new SingleButton(4, indButtonCol, paths_mainButtonsData, _Paths_display);
            pathFollowButtonsLayout_paths
                    ->addWidget(mainButtons_paths[4][indButtonCol]);
    }
    pathFollowButtons_paths->setLayout(pathFollowButtonsLayout_paths);
    pathFollowButtons_paths->setTitle("PART FOLLOW PATH");
    pathFollowButtons_paths->setMinimumHeight(51);

    // PATHS LINE 6
    // pathRepulse BUTTON 1 x 8
    pathRepulseButtons_paths = new QGroupBox;
    QHBoxLayout *pathRepulseButtonsLayout_paths = new QHBoxLayout;
    for(int indButtonCol = 0 ; indButtonCol < 8 ; indButtonCol++) {
            mainButtons_paths[5][indButtonCol]
                    = new SingleButton(5, indButtonCol, paths_mainButtonsData, _Paths_display);
            pathRepulseButtonsLayout_paths
                    ->addWidget(mainButtons_paths[5][indButtonCol]);
    }
    pathRepulseButtons_paths->setLayout(pathRepulseButtonsLayout_paths);
    pathRepulseButtons_paths->setTitle("PART REPULSE PATH");
    pathRepulseButtons_paths->setMinimumHeight(51);

    // PATHS LINE 7
    // pathRecord BUTTON 1 x 8
    pathRecordButtons_paths = new QGroupBox;
    QHBoxLayout *pathRecordButtonsLayout_paths = new QHBoxLayout;
    for(int indButtonCol = 0 ; indButtonCol < 8 ; indButtonCol++) {
            mainButtons_paths[6][indButtonCol]
                    = new SingleButton(6, indButtonCol, paths_mainButtonsData, _Paths_display);
            pathRecordButtonsLayout_paths
                    ->addWidget(mainButtons_paths[6][indButtonCol]);
    }
    pathRecordButtons_paths->setLayout(pathRecordButtonsLayout_paths);
    pathRecordButtons_paths->setTitle("PATH RECORD");
    pathRecordButtons_paths->setMinimumHeight(51);

    // PATHS LINE 8
    // pathReplay BUTTON 1 x 8
    pathReplayButtons_paths = new QGroupBox;
    QHBoxLayout *pathReplayButtonsLayout_paths = new QHBoxLayout;
    for(int indButtonCol = 0 ; indButtonCol < 8 ; indButtonCol++) {
            mainButtons_paths[7][indButtonCol]
                    = new SingleButton(7, indButtonCol, paths_mainButtonsData, _Paths_display);
            pathReplayButtonsLayout_paths
                    ->addWidget(mainButtons_paths[7][indButtonCol]);
    }
    pathReplayButtons_paths->setLayout(pathReplayButtonsLayout_paths);
    pathReplayButtons_paths->setTitle("PATH REPLAY");
    pathReplayButtons_paths->setMinimumHeight(51);

    // ROUND BUTTONS: SETUP/CA LINE 9
    // setUp BUTTON 1 x 3
    setUpButtons_paths = new QGroupBox;
    QHBoxLayout *setUpButtonsLayout_paths = new QHBoxLayout;
    for(int indButtonCol = 0 ; indButtonCol < 3 ; indButtonCol++) {
            mainButtons_paths[8][indButtonCol]
                    = new SingleButton(8, indButtonCol, paths_mainButtonsData, _Paths_display);
            setUpButtonsLayout_paths
                    ->addWidget(mainButtons_paths[8][indButtonCol]);
    }
    setUpButtons_paths->setLayout(setUpButtonsLayout_paths);
    setUpButtons_paths->setTitle("SET-UP");
    setUpButtons_paths->setMinimumHeight(51);

    // CAType BUTTON 1 x 5
    CATypeButtons_paths = new QGroupBox;
    QHBoxLayout *CATypeButtonsLayout_paths = new QHBoxLayout;
    for(int indButtonCol = 3 ; indButtonCol < 8 ; indButtonCol++) {
            mainButtons_paths[8][indButtonCol]
                    = new SingleButton(8, indButtonCol, paths_mainButtonsData, _Paths_display);
            CATypeButtonsLayout_paths
                    ->addWidget(mainButtons_paths[8][indButtonCol]);
    }
    CATypeButtons_paths->setLayout(CATypeButtonsLayout_paths);
    CATypeButtons_paths->setTitle("CA TYPE");
    CATypeButtons_paths->setMinimumHeight(51);

    // Paths Line 9 1 x 8
    partLine9Buttons_paths = new QGroupBox;
    QHBoxLayout *partLine9ButtonsLayout_paths = new QHBoxLayout;
    partLine9ButtonsLayout_paths->addWidget(setUpButtons_paths);
    partLine9ButtonsLayout_paths->addWidget(CATypeButtons_paths);
    partLine9Buttons_paths->setLayout(partLine9ButtonsLayout_paths);
    partLine9Buttons_paths->setTitle("");
    partLine9Buttons_paths->setObjectName("VOID");


    // SIDE BUTTON GRID 3 x 1
    sideButton_paths = new QGroupBox;
    QVBoxLayout *sideButtonLayout_paths = new QVBoxLayout;
    for(int indButtonCol = 0 ; indButtonCol < 8 ; indButtonCol++) {
        mainButtons_paths[9][indButtonCol]
                    = new SingleButton(9, indButtonCol, paths_mainButtonsData, _Paths_display);
            sideButtonLayout_paths
                    ->addWidget(mainButtons_paths[9][indButtonCol]);
    }
    sideButton_paths->setLayout(sideButtonLayout_paths);
    sideButton_paths->setTitle("");
    sideButton_paths->setObjectName("VOID");



    // SLIDERS LINE 10
    // partRadius GRID 1 x 3
    partRadiusSliders_paths = new QGroupBox;
    QGridLayout *partRadiusLayout_paths = new QGridLayout;
    for(unsigned int indCol = 0 ; indCol < paths_mainSlidersData[_ParticleRadiusInOutData].size() ; indCol++) {
        verticalSliders_paths[_ParticleRadiusInOutData][indCol]
                = new SlidersSpinboxGroup(_ParticleRadiusInOutData, indCol, paths_mainSlidersData);
        partRadiusLayout_paths
                ->addWidget(verticalSliders_paths[_ParticleRadiusInOutData][indCol],
                            _ParticleRadiusInOutData, indCol);
    }

    partRadiusSliders_paths->setLayout(partRadiusLayout_paths);
    partRadiusSliders_paths->setTitle("PART RADIUS/SHAPE");

    // partAcc GRID 1 x 3
    partAccSliders_paths = new QGroupBox;
    QGridLayout *partAccLayout_paths = new QGridLayout;
    for(unsigned int indCol = 0 ; indCol < paths_mainSlidersData[_ParticleFluidityInOutData].size() ; indCol++) {
        verticalSliders_paths[_ParticleFluidityInOutData][indCol]
                = new SlidersSpinboxGroup(_ParticleFluidityInOutData, indCol, paths_mainSlidersData);
        partAccLayout_paths
                ->addWidget(verticalSliders_paths[_ParticleFluidityInOutData][indCol],
                            _ParticleFluidityInOutData, indCol);
    }
    partAccSliders_paths->setLayout(partAccLayout_paths);
    partAccSliders_paths->setTitle("PART FLUIDITY");

    // partNoise GRID 1 x 3
    partNoiseSliders_paths = new QGroupBox;
    QGridLayout *partNoiseLayout_paths = new QGridLayout;
    for(unsigned int indCol = 0 ; indCol < paths_mainSlidersData[_ParticleNoiseInOutData].size() ; indCol++) {
        verticalSliders_paths[_ParticleNoiseInOutData][indCol]
                = new SlidersSpinboxGroup(_ParticleNoiseInOutData, indCol, paths_mainSlidersData);
        partNoiseLayout_paths
                ->addWidget(verticalSliders_paths[_ParticleNoiseInOutData][indCol],
                            _ParticleNoiseInOutData, indCol);
    }
    partNoiseSliders_paths->setLayout(partNoiseLayout_paths);
    partNoiseSliders_paths->setTitle("PART NOISE");

    // Paths Line 10 1 x 9
    partLine10Sliders_paths = new QGroupBox;
    QHBoxLayout *partLine10SlidersLayout_graphs = new QHBoxLayout;
    partLine10SlidersLayout_graphs->addWidget(partRadiusSliders_paths);
    partLine10SlidersLayout_graphs->addWidget(partAccSliders_paths);
    partLine10SlidersLayout_graphs->addWidget(partNoiseSliders_paths);
    partLine10Sliders_paths->setLayout(partLine10SlidersLayout_graphs);
    partLine10Sliders_paths->setTitle("");
    partLine10Sliders_paths->setObjectName("VOID");
}

/////////////////////////////////////////////////////////////
/// \brief GraphicalControllers::GraphicalControllers
/// \param givenTitle
/// \param IFimageFileName
/// \param givenType
/// \param parent
///

GraphicalControllers::GraphicalControllers(QString givenTitle,
                                           DisplayType givenType,
                                           QWidget *parent)
    : QWidget(parent) {
    title = givenTitle;

    /////////////////////////////////////////////////////////////////////
    // INITIALIZATION OF THE CONTROLLERS
    // 1 Sliders and buttons initialization
    switch(givenType) {
    case _Graphs_display:
        // GRAPHS CONTROLLERS
        for(int indRow = 0; indRow < _MaxGraphsInOutDataRows ; indRow++ ) {
            verticalSliders_graphs[indRow]
                    = new SlidersSpinboxGroup*[graphs_mainSlidersData[indRow].size()];
            for(unsigned int indCol = 0; indCol < graphs_mainSlidersData[indRow].size() ; indCol++ ) {
                verticalSliders_graphs[indRow][indCol] = NULL;
            }
        }
        for(int indRow = 0; indRow < _NbRows_mainButtons_graphs ; indRow++ ) {
            mainButtons_graphs[indRow]
                    = new SingleButton*[graphs_mainButtonsData[indRow].size()];
            for(unsigned int indCol = 0; indCol < graphs_mainButtonsData[indRow].size() ; indCol++ ) {
                mainButtons_graphs[indRow][indCol] = NULL;
            }
        }
        for(int indRow = 0; indRow < _NbRows_sideButtons_graphs ; indRow++ ) {
            sideButtons_graphs[indRow] = NULL;
        }
        break;
    case _Tracks_display:
        // TRACKS CONTROLLERS
        for(int indRow = 0; indRow < _MaxTracksInOutDataRows ; indRow++ ) {
            verticalSliders_tracks[indRow]
                    = new SlidersSpinboxGroup*[tracks_mainSlidersData[indRow].size()];
            for(unsigned int indCol = 0; indCol < tracks_mainSlidersData[indRow].size() ; indCol++ ) {
                verticalSliders_tracks[indRow][indCol] = NULL;
            }
        }
        for(int indRow = 0; indRow < _NbRows_mainButtons_graphs ; indRow++ ) {
            mainButtons_tracks[indRow]
                    = new SingleButton*[tracks_mainButtonsData[indRow].size()];
            for(unsigned int indCol = 0; indCol < tracks_mainButtonsData[indRow].size() ; indCol++ ) {
                mainButtons_tracks[indRow][indCol] = NULL;
            }
        }
        for(int indRow = 0; indRow < _NbRows_sideButtons_graphs ; indRow++ ) {
            sideButtons_tracks[indRow] = NULL;
        }
        break;
    case _Paths_display:
        // TRACKS CONTROLLERS
        for(int indRow = 0; indRow < _MaxPathsInOutDataRows ; indRow++ ) {
            verticalSliders_paths[indRow]
                    = new SlidersSpinboxGroup*[paths_mainSlidersData[indRow].size()];
            for(unsigned int indCol = 0; indCol < paths_mainSlidersData[indRow].size() ; indCol++ ) {
                verticalSliders_paths[indRow][indCol] = NULL;
            }
        }
        for(int indRow = 0; indRow < _NbRows_buttons_paths ; indRow++ ) {
            mainButtons_paths[indRow]
                    = new SingleButton*[paths_mainButtonsData[indRow].size()];
            for(unsigned int indCol = 0; indCol < paths_mainButtonsData[indRow].size() ; indCol++ ) {
                mainButtons_paths[indRow][indCol] = NULL;
            }
        }
        break;
    default:
        break;
    }

    /////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////
    // TAB CONSTRUCTION
    QGridLayout *allContollersLayout = new QGridLayout;
    switch(givenType) {
    case _Graphs_display:
        /////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////
        // GRAPHS TAB
        create_graphsTabControllers();
        allContollersLayout->addWidget(flashpixelpulseSliders_graphs, 0, 0);
        allContollersLayout->addWidget(sideButton_graphs, 0, 1);
        allContollersLayout->addWidget(penColorpixelModeButtons_graphs, 1, 0);
        allContollersLayout->addWidget(repopBWpenbrushModeButtons_graphs, 2, 0);
        allContollersLayout->addWidget(radiusbeatSliders_graphs, 3, 0);
        allContollersLayout->addWidget(pixelRadiusSliders_graphs, 3, 1);
        setLayout(allContollersLayout);
        break;
    case _Tracks_display:
        /////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////
        // TRACKS TAB
        create_tracksTabControllers();
        allContollersLayout->addWidget(weightrepopmediasobelSliders_tracks, 0, 0);
        allContollersLayout->addWidget(sideButton_tracks, 0, 1);
        allContollersLayout->addWidget(tracksmiconoffsoundsampleButtons_tracks, 1, 0);
        allContollersLayout->addWidget(decaysignemptyButtonmediaplusButtons_tracks, 2, 0);
        allContollersLayout->addWidget(decayechosoundSliders_tracks, 3, 0);
        allContollersLayout->addWidget(masterSliders_tracks, 3, 1);
        setLayout(allContollersLayout);
        break;
    case _Paths_display:
        /////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////
        // PATHS TAB
        create_pathsTabControllers();
        allContollersLayout->addWidget(partLine1Buttons_paths, 0, 0);
        allContollersLayout->addWidget(sideButton_paths, 0, 1, 8, 1);
        allContollersLayout->addWidget(partLine2Buttons_paths, 1, 0);
        allContollersLayout->addWidget(partInitButtons_paths, 2, 0);
        allContollersLayout->addWidget(pathtRepopButtons_paths, 3, 0);
        allContollersLayout->addWidget(pathFollowButtons_paths, 4, 0);
        allContollersLayout->addWidget(pathRepulseButtons_paths, 5, 0);
        allContollersLayout->addWidget(pathRecordButtons_paths, 6, 0);
        allContollersLayout->addWidget(pathReplayButtons_paths, 7, 0);
        allContollersLayout->addWidget(partLine9Buttons_paths, 8, 0);
        allContollersLayout->addWidget(partLine10Sliders_paths, 9, 0);
        setLayout(allContollersLayout);
        break;
    default:
        break;
    }
}

