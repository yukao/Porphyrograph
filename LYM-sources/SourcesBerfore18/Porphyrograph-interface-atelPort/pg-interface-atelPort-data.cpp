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

#include <vector>
#include "pg-interface-atelPort-data.h"

/////////////////////////////////////////////////////////////
/// \brief outControllerData::outControllerData
/// \param typeIn_par
/// \param controlsIn_par
/// \param channelsIn_par
/// \param controlsOut_par
/// \param labelsUDPIn_par
/// \param labelsHighlight_par
/// \param labelsUDPOut_par
/// \param amplitude_par
/// \param title_par
/// \param labelobject_names_par
/// \param contolKey_par
///
outControllerData::outControllerData(
        int typeIn_par,
        int controlsIn_par,
        int channelsIn_par,
        int controlsOut_par,
        QString labelsUDPIn_par,
        QString labelsHighlight_par,
        QString labelsUDPOut_par,
        float amplitude_par,
        QString title_par,
        QString labelobject_names_par,
        QString contolKey_par
        ) {
    typeMidiIn = typeIn_par;
    controlMidiIn = controlsIn_par;
    channelMidiIn = channelsIn_par;
    controlMidiOut = controlsOut_par;
    labelUDPIn = labelsUDPIn_par;
    labelHighlightIn = labelsHighlight_par;
    labelUDPOut = labelsUDPOut_par;
    amplitude = amplitude_par;
    title = title_par;
    labelobject_names = labelobject_names_par;
    contolKey = contolKey_par;
 }
outControllerData::~outControllerData(void) {
}

QString outControllerData::get_labelUDPIn() {
    return labelUDPIn;
}
QString outControllerData::get_labelHighlightIn() {
    return labelHighlightIn;
}
// MIDI in/out
int  outControllerData::get_typeMidiIn() {
    return typeMidiIn;
}
int  outControllerData::get_controlMidiIn() {
    return controlMidiIn;
}
int  outControllerData::get_channelMidiIn() {
    return channelMidiIn;
}
float  outControllerData::get_amplitude() {
    return amplitude;
}
// GUI names: displayed name - name used for styling
QString  outControllerData::get_title() {
    return title;
}
QString  outControllerData::get_labelobject_names() {
    return labelobject_names;
}

/////////////////////////////////////////////////////////////////////////
// IN/OUT CONTROLLER DATA

std::vector<std::vector<inOutControllerData>> graphs_mainSlidersData =
{
    { inOutControllerData(11,13,1, 0,"XXXX",                        "XXXX",                        "XXXX",                         1.f, "CA",         "XXXX",          "EMPTY","",  .7f, 1.f, .7f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 15,  6, 12 ),
      inOutControllerData(11,13,2, 0,"/flashPartCA_freq_can",       "XXXX",                        "/flashPartCA_freq",            9.f, "Pa",         "PART->CA",      "FLASH","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 15, 10, 12 ),
      inOutControllerData(11,13,3, 0,"/flashTrkCA_freq_0_can",      "/currentDrawingTrack_can 0",  "/flashTrkCA_freq_0",           9.f, "BG",         "BG->CA",        "FLASH","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 15,  6, 12 ),
      inOutControllerData(11,13,4, 0,"/flashTrkCA_freq_1_can",      "/currentDrawingTrack_can 1",  "/flashTrkCA_freq_1",           9.f, "T1",         "Tr1->CA",       "FLASH","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 15,  6, 12 ),
      inOutControllerData(11,13,5, 0,"/flashTrkCA_freq_2_can",      "/currentDrawingTrack_can 2",  "/flashTrkCA_freq_2",           9.f, "T2",         "Tr2->CA",       "FLASH","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 15,  6, 12 )},
    { inOutControllerData(11,12,1, 0,"/flashCAPart_freq_can",       "XXXX",                        "/flashCAPart_freq",            9.f, "Pa",         "CA->PART",      "FLASH","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 15,  6, 12 ),
      inOutControllerData(11,12,2, 0,"/flashPixel_duration_can",    "XXXX",                        "/flashPixel_duration",      70.99f, "",           "PIXEL dur",     "PIXEL","",  .7f, 1.f, .7f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 15, 10, 12 ),
      inOutControllerData(11,12,3, 0,"/flashTrkPart_freq_0_can",    "/currentDrawingTrack_can 0",  "/flashTrkPart_freq_0",         9.f, "",           "BG->PART",      "FLASH","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 15,  6, 12 ),
      inOutControllerData(11,12,4, 0,"/flashTrkPart_freq_1_can",    "/currentDrawingTrack_can 1",  "/flashTrkPart_freq_1",         9.f, "",           "Tr1->PART",     "FLASH","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 15,  6, 12 ),
      inOutControllerData(11,12,5, 0,"/flashTrkPart_freq_2_can",    "/currentDrawingTrack_can 2",  "/flashTrkPart_freq_2",         9.f, "",           "Tr2->PART",     "FLASH","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 15,  6, 12 )},
    { inOutControllerData(11,10,1, 0,"/flashCABG_freq_can",         "XXXX",                        "/flashCABG_freq",              9.f, "BG",         "CA->BG",        "FLASH","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 15,  6, 12 ),
      inOutControllerData(11,10,2, 0,"/flashPartBG_freq_can",       "XXXX",                        "/flashPartBG_freq",            9.f, "",           "PART->BG",      "FLASH","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 15, 10, 12 ),
      inOutControllerData(11,10,3, 0,"/flashPixel_freq_can",        "/currentDrawingTrack_can 0",  "/flashPixel_freq",             9.f, "",           "PIXEL",         "PIXEL","",  .7f, 1.f, .7f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 15,  6, 12 ),
      inOutControllerData(11,10,4, 0,"/flashTrkBG_freq_1_can",      "/currentDrawingTrack_can 1",  "/flashTrkBG_freq_1",           9.f, "",           "Tr1->BG",       "FLASH","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 15,  6, 12 ),
      inOutControllerData(11,10,5, 0,"/flashTrkBG_freq_2_can",      "/currentDrawingTrack_can 2",  "/flashTrkBG_freq_2",           9.f, "",           "Tr2->BG",       "FLASH","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 15,  6, 12 )},

    { inOutControllerData(11,13,6, 0,"/pixel_acc_factor_can",       "XXXX",                        "/pixel_acc_factor",           .05f, "Acc",        "PIX acc",       "PIXEL","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 10, 8, 10 )},
    { inOutControllerData(11,13,7, 0,"/CAdecay_pulse_can",          "XXXX",                        "/CAdecay_pulse",               3.f, "CADk",       "CA dec",        "PULSE","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 10, 12, 10 ),
      inOutControllerData(11,13,8, 0,"/flashWeight_pulse_can",      "XXXX",                        "/flashWeight_pulse",           1.f, "FlW",        "FLASH Wght",    "PULSE","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 10, 10, 10 )},
    { inOutControllerData(11, 0,0, 0,"/pulse_low",                  "XXXX",                        "XXXX",                         1.f, "L",          "",              "SOUND","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f ,  6,  2,  7 ),
      inOutControllerData(11, 0,0, 0,"/pulse_medium",               "XXXX",                        "XXXX",                         1.f, "M",          "",              "SOUND","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f ,  6,  2,  7 ),
      inOutControllerData(11, 0,0, 0,"/pulse_high",                 "XXXX",                        "XXXX",                         1.f, "H",          "",              "SOUND","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f ,  6,  7,  7 ),
      inOutControllerData(11, 0,0, 0,"/pulse_enveloppe",            "XXXX",                        "XXXX",                         1.f, "E",          "",              "SOUND","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f ,  6,  0,  7 )},


#ifdef GENERATIVE_NIGHTS
    { inOutControllerData(11,12,6, 0,"/CAParams4_can",              "XXXX",                        "/CAParams4",                   1.f, "Par4",       "Param4",          "PEN","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 10, 18, 10 ),
      inOutControllerData(11,12,7, 0,"/CAParams5_can",              "XXXX",                        "/CAParams5",                   1.f, "Par5",       "Param5",          "PEN","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 10, 18, 10 ),
      inOutControllerData(11,12,8, 0,"/CAParams6_can",              "XXXX",                        "/CAParams6",                   1.f, "Par6",       "Param6",          "PEN","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 10, 18, 10 )},
#else
    { inOutControllerData(11,12,6, 0,"/repop_hue_pulse_can",        "XXXX",                        "/repop_hue_pulse",             .3f, "Hue",        "PART hue",      "PULSE","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 10, 18, 10 ),
      inOutControllerData(11,12,7, 0,"/part_acc_pulse_can",         "XXXX",                        "/part_acc_pulse",              1.f, "Acc",        "PART acc",      "PULSE","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 10, 18, 10 ),
      inOutControllerData(11,12,8, 0,"/part_radius_pulse_can",      "XXXX",                        "/part_radius_pulse",          20.f, "Rad",        "PART rad",      "PULSE","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 10, 18, 10 )},
#endif
    { inOutControllerData(11,10,6, 0,"/CAParams1_can",              "XXXX",                        "/CAParams1",                   1.f, "Par1",       "Param1",          "PEN","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 10, 18, 10 ),
      inOutControllerData(11,10,7, 0,"/CAParams2_can",              "XXXX",                        "/CAParams2",                   1.f, "Par2",       "Param2",          "PEN","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 10, 18, 10 ),
      inOutControllerData(11,10,8, 0,"/CAParams3_can",              "XXXX",                        "/CAParams3",                   1.f, "Par3",       "Param3",          "PEN","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 10, 18, 10 )},
    { inOutControllerData(11, 7,1, 0,"/pen_radius_can",             "XXXX",                        "/pen_radius",                 20.f, "Rad",        "PEN radius",      "PEN","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 10, 12, 10 ),
      inOutControllerData(11, 7,2, 0,"/pen_radius_pressure_coef_can","XXXX",                       "/pen_radius_pressure_coef",   30.f, "Pres",       "PEN press",       "PEN","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 10, 18, 10 ),
      inOutControllerData(11, 7,3, 0,"/pen_grey_can",               "XXXX",                        "/pen_grey",                    1.f, "Grey",       "PEN grey",        "PEN","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 10, 12, 10 ),
      inOutControllerData(11, 7,4, 0,"/pen_color_can",              "XXXX",                        "/pen_color",                   1.f, "Pen",        "PEN color",       "PEN","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 10, 12, 10 ),
      inOutControllerData(11, 7,5, 0,"/repop_color_can",            "XXXX",                        "/repop_color",                 1.f, "Rep",        "REPO color",    "REPOP","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 10, 12, 10 )},
    { inOutControllerData(11, 7,6, 0,"/XXXXXX",                     "XXXX",                        "/XXXX",                        2.f, "XX",         "XX",            "EMPTY","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 10, 18, 10 ),
      inOutControllerData(11, 7,7, 0,"/beat_delay_can",             "XXXX",                        "/beat_delay",               2000.f, "Del",        "delay",          "BEAT","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 10, 18, 10 ),
      inOutControllerData(11, 7,8, 0,"/auto_beat_duration_can",     "XXXX",                        "/auto_beat_duration",          1.f, "Dur",        "auto dur",       "BEAT","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 10, 18, 10 )},
    { inOutControllerData(11,14,8, 0,"/pixel_radius_can",           "XXXX",                        "/pixel_radius",                1.f, "Rad",        "PIXEL radius",  "PIXEL","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 10, 18, 10 )}
};

std::vector<std::vector<inOutControllerData>> tracks_mainSlidersData =
{
    { inOutControllerData(11,13,1, 0,"/CAMixingWeight_can",         "XXXX",                        "/CAMixingWeight",              1.f, "CA",         "CA Mixing",     "TKWGH","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 10, 11, 10 ),
      inOutControllerData(11,13,2, 0,"/PartMixingWeight_can",       "XXXX",                        "/PartMixingWeight",            1.f, "Pa",         "PART Mixing",   "TKWGH","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 10, 22, 10 ),
      inOutControllerData(11,13,3, 0,"/trackMixingWeight_0_can",    "/currentDrawingTrack_can 0",  "/trackMixingWeight_0",         1.f, "BG",         "BG Mixing",     "TKWGH","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 10, 11, 10 ),
      inOutControllerData(11,13,4, 0,"/trackMixingWeight_1_can",    "/currentDrawingTrack_can 1",  "/trackMixingWeight_1",         1.f, "T1",         "Tr1 Mixing",    "TKWGH","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 10, 11, 10 ),
      inOutControllerData(11,13,5, 0,"/trackMixingWeight_2_can",    "/currentDrawingTrack_can 2",  "/trackMixingWeight_2",         1.f, "T2",         "Tr2 Mixing",    "TKWGH","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 10, 11, 10 )},
    { inOutControllerData(11,13,6, 0,"/photoWeight_can",            "XXXX",                        "/photoWeight",                 1.f, "Pho",        "PHOT Wght",     "MEDIA","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 10, 32, 10 ),
      inOutControllerData(11,13,7, 0,"/movieWeight_can",            "XXXX",                        "/movieWeight",                 1.f, "Mv",        "MOV Wght",      "MEDIA","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 10, 11, 10 ),
      inOutControllerData(11,13,8, 0,"/cameraWeight_can",           "XXXX",                        "/cameraWeight",                1.f, "Ca",         "CAM Wght",      "MEDIA","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 10,  7, 10 )},
    { inOutControllerData(11, 0,0, 0,"/pulse_low",                  "XXXX",                        "XXXX",                         1.f, "L",          "",              "SOUND","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f ,  6,  2,  7 ),
      inOutControllerData(11, 0,0, 0,"/pulse_medium",               "XXXX",                        "XXXX",                         1.f, "M",          "",              "SOUND","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f ,  6,  2,  7 ),
      inOutControllerData(11, 0,0, 0,"/pulse_high",                 "XXXX",                        "XXXX",                         1.f, "H",          "",              "SOUND","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f ,  6,  7,  7 ),
      inOutControllerData(11, 0,0, 0,"/pulse_enveloppe",            "XXXX",                        "XXXX",                         1.f, "E",          "",              "SOUND","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f ,  6,  0,  7 )},

    { inOutControllerData(11,12,1, 0,"/CAMasterWeight_can",         "XXXX",                        "/CAMasterWeight",              1.f, "CA",         "CA Master",     "TKWGH","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 10, 11, 10 ),
      inOutControllerData(11,12,2, 0,"/PartMasterWeight_can",       "XXXX",                        "/PartMasterWeight",            1.f, "Pa",         "PART Master",   "TKWGH","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 10, 22, 10 ),
      inOutControllerData(11,12,3, 0,"/trackMasterWeight_0_can",    "/currentDrawingTrack_can 0",  "/trackMasterWeight_0",         1.f, "BG",         "BG Master",     "TKWGH","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 10, 11, 10 ),
      inOutControllerData(11,12,4, 0,"/trackMasterWeight_1_can",    "/currentDrawingTrack_can 1",  "/trackMasterWeight_1",         1.f, "T1",         "Tr1 Master",    "TKWGH","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 10, 11, 10 ),
      inOutControllerData(11,12,5, 0,"/trackMasterWeight_2_can",    "/currentDrawingTrack_can 2",  "/trackMasterWeight_2",         1.f, "T2",         "Tr2 Master",    "TKWGH","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 10, 11, 10 )},
    { inOutControllerData(11,12,6, 0,"/cameraThreshold_can",        "XXXX",                        "/cameraThreshold",             1.f, "Ca",         "CAM thrshld",   "MEDIA","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 10,  8, 10 )},
    { inOutControllerData(11,12,7, 0,"/movieCaptFreq_can",          "XXXX",                        "/movieCaptFreq",              20.f, "Mv",        "MOV freq",      "MEDIA","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 10, 11, 10 ),
      inOutControllerData(11,12,8, 0,"/cameraCaptFreq_can",         "XXXX",                        "/cameraCaptFreq",             20.f, "Ca",         "CAM freq",      "MEDIA","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 10,  7, 10 )},

    { inOutControllerData(11,10,1, 0,"/repop_CA_can",               "XXXX",                        "/repop_CA",                    .9f,  "CA",        "REPO CA",       "REPOP","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 10, 11, 10 ),
      inOutControllerData(11,10,2, 0,"/repop_part_can",             "XXXX",                        "/repop_part",                  .9f,  "Pa",        "REPO Part",     "REPOP","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 10, 18, 10 ),
      inOutControllerData(11,10,3, 0,"/repop_BG_can",               "XXXX",                        "/repop_BG",                    .9f,  "BG",        "REPO BG",       "REPOP","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 10, 18, 10 ),
      inOutControllerData(11,10,4, 0,"/repop_path_can",             "XXXX",                        "/repop_path",                  .9f,  "Pth",       "REPO Path",     "REPOP","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 10, 11, 10 )},
//      inOutControllerData(11,10,5, 0,"XXXX",                        "XXXX",                        "XXXX",                         1.f,  "XX",        "XXXX",        "EMPTY","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 10, 11, 10 )},
    { inOutControllerData(11,10,6, 0,"/XXXX",              "XXXX",                        "/XXXX",                   1.f,  "XX",       "XX",   "EMPTY","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 10, 8, 10 )},
    { inOutControllerData(11,10,7, 0,"/movieSobel_can",             "XXXX",                        "/movieSobel",                  1.f,  "Mv",       "MOV Sobel",     "MEDIA","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 10, 11, 10 ),
      inOutControllerData(11,10,8, 0,"/cameraSobel_can",            "XXXX",                        "/cameraSobel",                 1.f,  "Ca",        "CAM Sobel",     "MEDIA","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 10,  7, 10 )},

    { inOutControllerData(11, 7,1, 0,"/trkDecay_1_can",             "/trkDecay_sign_1_can -1",     "/trkDecay_1",                0.01f,  "T1",        "Tr1 decay",     "TRACK","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 10, 8, 10 )},
    { inOutControllerData(11, 7,2, 0,"/beat_threshold_can",         "XXXX",                        "/beat_threshold",              2.f, "Thrs",       "threshold",      "BEAT","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 10, 18, 10 ),
      inOutControllerData(11, 7,3, 0,"/autom_env_min_can",          "XXXX",                        "XXXX",                       100.f, "Min",      "Autom Min",    "SOUND","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 10, 12, 10 ),
      inOutControllerData(11, 7,4, 0,"/autom_env_max_can",          "XXXX",                        "XXXX",                       100.f, "Max",      "Autom Max",    "SOUND","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 10, 14, 10 )},
    { inOutControllerData(11, 7,5, 0,"/sound_env_min_can",          "XXXX",                        "/sound_env_min",             100.f, "ThMin",      "Thres Min",     "SOUND","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 10, 8, 10 ),
      inOutControllerData(11, 7,6, 0,"/sound_env_max_can",          "XXXX",                        "/sound_env_max",             100.f, "ThMax",      "Thres Max",     "SOUND","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 10, 18, 10 ),
      inOutControllerData(11, 7,7, 0,"/sound_volume_can",           "XXXX",                        "/sound_volume",                2.f,  "Vol",        "Volume",       "SOUND","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 10,  8, 10 ),
      inOutControllerData(11, 7,8, 0,"/sound_min_can",              "XXXX",                        "/sound_min",                   2.f,  "VMin",       "Min Vol",    "SOUND","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 10,  12, 10 )},
    { inOutControllerData(11,14,8, 0,"/blendTransp_can",            "XXXX",                        "/blendTransp",                 1.f, "Blk",        "MASTER",        "TKWGH","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 10, 11, 10 )}
};

std::vector<std::vector<inOutControllerData>> paths_mainSlidersData =
{
    { inOutControllerData(11,48,1, 0,"/partRadius_can",             "XXXX",                        "/partRadius",                  3.f, "All",        "AllRad",         "PART","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 10, 18, 10 ),
      inOutControllerData(11,49,1, 0,"/partRepopRadius_can",        "XXXX",                        "/partRepopRadius",             3.f, "Repop",      "RepopRad",       "PART","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 10, 18, 10 ),
      inOutControllerData(11,50,1, 0,"/part_type_can",              "XXXX",                        "/part_type",                  3.9f, "Shape",      "Shape",          "PART","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 10, 18, 10 )},
    { inOutControllerData(11,51,1, 0,"/part_acc_factor_can",        "XXXX",                        "/part_acc_factor",             .1f, "Acc",        "Acc",            "PART","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 10, 18, 10 ),
      inOutControllerData(11,52,1, 0,"/part_damp_factor_can",       "XXXX",                        "/part_damp_factor",           .01f, "Damp",       "Damp",           "PART","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 10, 18, 10 ),
      inOutControllerData(11,53,1, 0,"/part_damp_targtRad_can",     "XXXX",                        "/part_damp_targtRad",       1000.f, "Tget",       "Damp Tgt",       "PART","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 10, 18, 10 )},
    { inOutControllerData(11,54,1, 0,"/noiseScale_can",             "XXXX",                        "/noiseScale",                  1.f, "Scale",      "Noise Scal",     "PART","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 10, 18, 10 ),
      inOutControllerData(11,55,1, 0,"/part_field_weight_can",      "XXXX",                        "/part_field_weight",           1.f, "Weight",     "Field Wght",     "PART","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 10, 18, 10 ),
      inOutControllerData(11,56,1, 0,"/part_timeToTargt_can",       "XXXX",                        "/part_timeToTargt",          300.f, "SpeedTgt",   "Time to Tgt",    "PART","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 10, 18, 10 )},
};

//// 1st & 2nd row graphs buttons
std::vector<std::vector<inOutControllerData>> graphs_mainButtonsData =
{
    { inOutControllerData(11, 9,1, 1,"XXXX",                        "/pen_grey_can 1",             "/pen_BW",                      1.f, "B/W",        "B/W",             "PEN","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 12 ),
      inOutControllerData(11, 9,2, 4,"XXXX",                        "/pen_colorPreset_can 0",      "/pen_colorPreset",             0.f, "1",          "1",               "PEN","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 12 ),
      inOutControllerData(11, 9,3, 7,"XXXX",                        "/pen_colorPreset_can 1",      "/pen_colorPreset",             1.f, "2",          "2",               "PEN","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData(11, 9,4,10,"XXXX",                        "/pen_colorPreset_can 2",      "/pen_colorPreset",             2.f, "3",          "3",               "PEN","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData(11, 9,5,13,"XXXX",                        "/pen_colorPreset_can 3",      "/pen_colorPreset",             3.f, "4",          "4",               "PEN","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData(11, 9,6,16,"XXXX",                        "/pen_colorPreset_can 4",      "/pen_colorPreset",             4.f, "5",          "5",               "PEN","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData(11, 9,7,19,"XXXX",                        "/pen_colorPreset_can 5",      "/pen_colorPreset",             5.f, "6",          "6",               "PEN","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData(11, 9,8,22,"XXXX",                        "/pixel_mode_can 1",           "/pixel_mode_plus",             1.f, "Mode",       "PIXEL Mode",    "PIXEL","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 )},
    { inOutControllerData(11, 8,1, 3,"XXXX",                        "/repop_grey_can 1",           "/repop_BW",                    1.f, "B/W",        "REPOP B/W",     "REPOP","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 12 ),
      inOutControllerData(11, 8,2, 6,"XXXX",                        "/pen_brush_can 0",            "/pen_brush",                   0.f, "Cir",        "Circle",          "PEN","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 12 ),
      inOutControllerData(11, 8,3, 9,"XXXX",                        "/pen_brush_can 1",            "/pen_brush",                   1.f, "Bon",        "Bone",            "PEN","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData(11, 8,4,12,"XXXX",                        "/pen_brush_can 2",            "/pen_brush",                   2.f, "Hrs",        "Horse",           "PEN","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData(11, 8,5,15,"XXXX",                        "/pen_brush_can 3",            "/pen_brush",                   3.f, "Trk",        "Truck",           "PEN","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData(11, 8,6,18,"XXXX",                        "/pen_brush_can 4",            "/pen_brush",                   4.f, "A",          "A",               "PEN","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData(11, 8,7,21,"XXXX",                        "/pen_brush_can 5",            "/pen_brush",                   5.f, "Hrt",        "Heart",           "PEN","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData(11, 8,8,24,"XXXX",                        "/freeze_can 1",               "/freeze",                      1.f, "Off",        "OFF",             "GUI","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 )}
};

//// 1st & 2nd row tracks buttons
std::vector<std::vector<inOutControllerData>> tracks_mainButtonsData =
{
    { inOutControllerData(11, 9,1, 1,"XXXX",                        "/launch_can 1",               "/launch",                      1.f, "Launch",     "Launch",          "GUI","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData(11, 9,2, 4,"XXXX",                        "XXXX",                        "XXXX",                         1.f, "XX",         "XXXX",          "EMPTY","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData(11, 9,3, 7,"XXXX",                        "/mute_screen_can 1",          "/mute_screen",                 1.f, "Mute",       "Mute Screen",     "GUI","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData(11, 9,4,10,"XXXX",                        "XXXX",                        "XXXX",                         1.f, "XX",         "XXXX",          "EMPTY","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData(11, 9,5,13,"XXXX",                        "XXXX",                        "XXXX",                         1.f, "XX",         "XXXX",          "EMPTY","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData(11, 9,6,16,"XXXX",                        "XXXX",                        "/clearAllLayers",              1.f, "CLR",        "Clear All",       "GUI","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData(11, 9,7,19,"XXXX",                        "XXXX",                        "XXXX",                         1.f, "XX",         "XXXX",          "EMPTY","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData(11, 9,8,22,"XXXX",                        "XXXX",                        "/currentBGCapture",            1.f, "RstIm",      "Reset Img",       "GUI","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 )},
    { inOutControllerData(11, 8,1, 3,"XXXX",                        "/CAdecay_sign_can -1",        "/CAdecay_sign",                1.f, "CA",         "CA +/-",        "TRACK","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 12 ),
      inOutControllerData(11, 8,2, 6,"XXXX",                        "/partDecay_sign_can -1",      "/partDecay_sign",              1.f, "Pa",         "PART +/-",      "TRACK","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 12 ),
      inOutControllerData(11, 8,3, 9,"XXXX",                        "/trkDecay_sign_0_can -1",     "/trkDecay_sign_0",             1.f, "BG",         "BG +/-",        "TRACK","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData(11, 8,4,12,"XXXX",                        "/trkDecay_sign_1_can -1",     "/trkDecay_sign_1",             1.f, "T1",         "TK1 +/-",       "TRACK","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData(11, 8,5,15,"XXXX",                        "/trkDecay_sign_2_can -1",     "/trkDecay_sign_2",             1.f, "T2",         "TK2 +/-",       "TRACK","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData(11, 8,6,18,"XXXX",                        "XXXX",                        "XXXX",                         1.f, "XX",         "XXXX",          "EMPTY","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData(11, 8,7,21,"XXXX",                        "XXXX",                        "/diaporama_plus",              1.f, "Diap+",      "DIAPO +",       "MEDIA","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData(11, 8,8,24,"XXXX",                        "XXXX",                        "/movie_plus",                  1.f, "Mov+",       "MOVIE +",       "MEDIA","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 )}
};

//// 1st & 2nd row paths buttons
std::vector<std::vector<inOutControllerData>> paths_mainButtonsData =
{
    { inOutControllerData( 9,56,1,56,"XXXX",                        "/partMove_target_can 1",      "/partMove_target",             1.f, "Tgt",        "TARGET",         "PART","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData( 9,57,1,57,"XXXX",                        "/partMove_rand_can 1",        "/partMove_rand",               1.f, "Rand",       "RANDOM",         "PART","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData( 9,58,1,58,"XXXX",                        "XXXX",                        "XXXX",                         1.f, "X",          "XXXX",          "EMPTY","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData( 9,59,1,59,"XXXX",                        "XXXX",                        "XXXX",                         1.f, "X",          "XXXX",          "EMPTY","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData( 9,60,1,60,"XXXX",                        "/partStroke_mode_can 0",      "/partStroke_mode",             0.f, "X",          "None",           "PART","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData( 9,61,1,61,"XXXX",                        "/partStroke_mode_can 1",      "/partStroke_mode",             1.f, "Agr",        "Aggreg",         "PART","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData( 9,62,1,62,"XXXX",                        "/partStroke_mode_can 2",      "/partStroke_mode",             2.f, "Bnc",        "Bounce",         "PART","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData( 9,63,1,63,"XXXX",                        "/partStroke_mode_can 3",      "/partStroke_mode",             3.f, "Dmp",        "Damp",           "PART","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 )},
    { inOutControllerData( 9,48,1,48,"XXXX",                        "/partExit_mode_can 0",        "/partExit_mode",               0.f, "X",          "None",           "PART","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData( 9,49,1,49,"XXXX",                        "/partExit_mode_can 1",        "/partExit_mode",               1.f, "Opp",        "Oppos",          "PART","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData( 9,50,1,50,"XXXX",                        "/partExit_mode_can 2",        "/partExit_mode",               2.f, "Bce",        "Bounce",         "PART","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData( 9,51,1,51,"XXXX",                        "/partExit_mode_can 3",        "/partExit_mode",               3.f, "X",          "XXXX",          "EMPTY","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData( 9,52,1,52,"XXXX",                        "/partColor_mode_can 0",       "/partColor_mode",              0.f, "White",      "White",          "PART","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData( 9,53,1,53,"XXXX",                        "/partColor_mode_can 1",       "/partColor_mode",              1.f, "Max",        "Max",            "PART","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData( 9,54,1,54,"XXXX",                        "/partColor_mode_can 2",       "/partColor_mode",              2.f, "Cpy",        "Copy",           "PART","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData( 9,55,1,55,"XXXX",                        "/partColor_mode_can 3",       "/partColor_mode",              3.f, "X",          "XXXX",          "EMPTY","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 )},
    { inOutControllerData( 9,40,1,40,"XXXX",                        "/part_initialization_can 0",  "/part_initialization",         0.f, "0",          "0",              "PART","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData( 9,41,1,41,"XXXX",                        "/part_initialization_can 1",  "/part_initialization",         1.f, "1",          "1",              "PART","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData( 9,42,1,42,"XXXX",                        "/part_initialization_can 2",  "/part_initialization",         2.f, "2",          "2",              "PART","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData( 9,43,1,43,"XXXX",                        "/part_initialization_can 3",  "/part_initialization",         3.f, "3",          "3",              "PART","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData( 9,44,1,44,"XXXX",                        "/part_initialization_can 4",  "/part_initialization",         4.f, "4",          "4",              "PART","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData( 9,45,1,45,"XXXX",                        "/part_initialization_can 5",  "/part_initialization",         5.f, "5",          "5",              "PART","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData( 9,46,1,46,"XXXX",                        "/part_initialization_can 6",  "/part_initialization",         6.f, "6",          "6",              "PART","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData( 9,47,1,47,"XXXX",                        "/part_initialization_can 7",  "/part_initialization",         7.f, "7",          "7",              "PART","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 )},
    { inOutControllerData( 9,32,1,32,"XXXX",                        "/path_repop_0_can 1",         "/path_repop_0",                1.f, "0",          "0",              "PART","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData( 9,33,1,33,"XXXX",                        "/path_repop_1_can 1",         "/path_repop_1",                1.f, "1",          "1",              "PART","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData( 9,34,1,34,"XXXX",                        "/path_repop_2_can 1",         "/path_repop_2",                1.f, "2",          "2",              "PART","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData( 9,35,1,35,"XXXX",                        "/path_repop_3_can 1",         "/path_repop_3",                1.f, "3",          "3",              "PART","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData( 9,36,1,36,"XXXX",                        "/path_repop_4_can 1",         "/path_repop_4",                1.f, "4",          "4",              "PART","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData( 9,37,1,37,"XXXX",                        "/path_repop_5_can 1",         "/path_repop_5",                1.f, "5",          "5",              "PART","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData( 9,38,1,38,"XXXX",                        "/path_repop_6_can 1",         "/path_repop_6",                1.f, "6",          "6",              "PART","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData( 9,39,1,39,"XXXX",                        "/path_repop_7_can 1",         "/path_repop_7",                1.f, "7",          "7",              "PART","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 )},
    { inOutControllerData( 9,24,1,24,"XXXX",                        "/part_path_follow_0_can 1",   "/part_path_follow_0",          1.f, "0",          "0",              "PART","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData( 9,25,1,25,"XXXX",                        "/part_path_follow_1_can 1",   "/part_path_follow_1",          1.f, "1",          "1",              "PART","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData( 9,26,1,26,"XXXX",                        "/part_path_follow_2_can 1",   "/part_path_follow_2",          1.f, "2",          "2",              "PART","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData( 9,27,1,27,"XXXX",                        "/part_path_follow_3_can 1",   "/part_path_follow_3",          1.f, "3",          "3",              "PART","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData( 9,28,1,28,"XXXX",                        "/part_path_follow_4_can 1",   "/part_path_follow_4",          1.f, "4",          "4",              "PART","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData( 9,29,1,29,"XXXX",                        "/part_path_follow_5_can 1",   "/part_path_follow_5",          1.f, "5",          "5",              "PART","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData( 9,30,1,30,"XXXX",                        "/part_path_follow_6_can 1",   "/part_path_follow_6",          1.f, "6",          "6",              "PART","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData( 9,31,1,31,"XXXX",                        "/part_path_follow_7_can 1",   "/part_path_follow_7",          1.f, "7",          "7",              "PART","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 )},
    { inOutControllerData( 9,16,1,16,"XXXX",                        "/part_path_repulse_0_can 1",  "/part_path_repulse_0",         1.f, "0",          "0",              "PART","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData( 9,17,1,17,"XXXX",                        "/part_path_repulse_1_can 1",  "/part_path_repulse_1",         1.f, "1",          "1",              "PART","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData( 9,18,1,18,"XXXX",                        "/part_path_repulse_2_can 1",  "/part_path_repulse_2",         1.f, "2",          "2",              "PART","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData( 9,19,1,19,"XXXX",                        "/part_path_repulse_3_can 1",  "/part_path_repulse_3",         1.f, "3",          "3",              "PART","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData( 9,20,1,20,"XXXX",                        "/part_path_repulse_4_can 1",  "/part_path_repulse_4",         1.f, "4",          "4",              "PART","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData( 9,21,1,21,"XXXX",                        "/part_path_repulse_5_can 1",  "/part_path_repulse_5",         1.f, "5",          "5",              "PART","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData( 9,22,1,22,"XXXX",                        "/part_path_repulse_6_can 1",  "/part_path_repulse_6",         1.f, "6",          "6",              "PART","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData( 9,23,1,23,"XXXX",                        "/part_path_repulse_7_can 1",  "/part_path_repulse_7",         1.f, "7",          "7",              "PART","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 )},
    { inOutControllerData( 9, 8,1, 8,"XXXX",                        "XXXX",                        "XXXX",                         1.f, "X",          "XXXX",          "EMPTY","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData( 9, 9,1, 9,"XXXX",                        "/path_record_1_can 1",        "/path_record_1",               1.f, "1",          "1",              "PATH","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData( 9,10,1,10,"XXXX",                        "/path_record_2_can 1",        "/path_record_2",               1.f, "2",          "2",              "PATH","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData( 9,11,1,11,"XXXX",                        "/path_record_3_can 1",        "/path_record_3",               1.f, "3",          "3",              "PATH","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData( 9,12,1,12,"XXXX",                        "/path_record_4_can 1",        "/path_record_4",               1.f, "4",          "4",              "PATH","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData( 9,13,1,13,"XXXX",                        "/path_record_5_can 1",        "/path_record_5",               1.f, "5",          "5",              "PATH","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData( 9,14,1,14,"XXXX",                        "/path_record_6_can 1",        "/path_record_6",               1.f, "6",          "6",              "PATH","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData( 9,15,1,15,"XXXX",                        "/path_record_7_can 1",        "/path_record_7",               1.f, "7",          "7",              "PATH","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 )},
    { inOutControllerData( 9, 0,1, 0,"XXXX",                        "XXXX",                        "XXXX",                         1.f, "X",          "XXXX",          "EMPTY","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData( 9, 1,1, 1,"XXXX",                        "/path_replay_trackNo_1_can 1","/path_replay_trackNo_1",       1.f, "1",          "1",              "PATH","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData( 9, 2,1, 2,"XXXX",                        "/path_replay_trackNo_2_can 1","/path_replay_trackNo_2",       1.f, "2",          "2",              "PATH","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData( 9, 3,1, 3,"XXXX",                        "/path_replay_trackNo_3_can 1","/path_replay_trackNo_3",       1.f, "3",          "3",              "PATH","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData( 9, 4,1, 4,"XXXX",                        "/path_replay_trackNo_4_can 1","/path_replay_trackNo_4",       1.f, "4",          "4",              "PATH","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData( 9, 5,1, 5,"XXXX",                        "/path_replay_trackNo_5_can 1","/path_replay_trackNo_5",       1.f, "5",          "5",              "PATH","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData( 9, 6,1, 6,"XXXX",                        "/path_replay_trackNo_6_can 1","/path_replay_trackNo_6",       1.f, "6",          "6",              "PATH","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData( 9, 7,1, 7,"XXXX",                        "/path_replay_trackNo_7_can 1","/path_replay_trackNo_7",       1.f, "7",          "7",              "PATH","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 )},
    { inOutControllerData( 9,64,1,64,"XXXX",                        "XXXX",                        "/setup_plus",                  1.f, "+",          "SetUp +",         "GUI","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 18 ),
      inOutControllerData( 9,65,1,65,"XXXX",                        "XXXX",                        "/resend_all",                  1.f, "Rsnd",       "Resend all",      "GUI","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 18 ),
      inOutControllerData( 9,66,1,66,"XXXX",                        "XXXX",                        "/setup_minus",                 1.f, "-",          "SetUp -",        "GUI","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 18 ),
      inOutControllerData( 9,67,1,67,"XXXX",                        "/CASubType_can 1",            "/CAonOff",                     1.f, "ON/OFF",     "CA On/Off",      "PART","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData( 9,68,1,68,"XXXX",                        "/CAType_can 0",               "/CAType",                      0.f, "Cy0",        "Cyclic_0",       "PART","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData( 9,69,1,69,"XXXX",                        "/CAType_can 1",               "/CAType",                      1.f, "Cy1",        "Cyclic_1",       "PART","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData( 9,70,1,70,"XXXX",                        "/CAType_can 2",               "/CAType",                      2.f, "GoL",        "GOL",            "PART","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData( 9,71,1,71,"XXXX",                        "/CAType_can 3",               "/CAType",                      3.f, "PrCl",       "ProtoCell",      "PART","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 )},
    { inOutControllerData( 9,82,1,82,"XXXX",                        "/PD_connected 1",             "/connect_PD",                  1.f, "PD",         "Connect PD",      "GUI","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData( 9,83,1,83,"XXXX",                        "XXXX",                        "/beat",                       -1.f, "Beat",       "Beat",            "GUI","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData( 9,84,1,84,"XXXX",                        "XXXX",                        "/currentBGCapture",            1.f, "RstIm",      "Reset Img",       "GUI","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData( 9,85,1,85,"XXXX",                        "XXXX",                        "/reset_sound",                 1.f, "RsrSnd",     "Reset Snd",       "GUI","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData( 9,86,1,86,"XXXX",                        "/launch_can 1",               "/launch",                      1.f, "Launch",     "Launch",          "GUI","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData( 9,87,1,87,"XXXX",                        "/mute_screen_can 1",          "/mute_screen",                 1.f, "Mute",       "Mute Screen",     "GUI","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData( 9,88,1,88,"XXXX",                        "XXXX",                        "/clearAllLayers",              1.f, "CLR",        "Clear All",       "GUI","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData( 9,89,1,89,"XXXX",                        "/invertAllLayers_can 1",      "/invertAllLayers",             1.f, "INV",        "Invert All",      "GUI","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 )}};

std::vector<inOutControllerData> graphs_sideButtonsData =
    { inOutControllerData( 9,25,1,25,"XXXX",                        "XXXX",                        "/hide",                        0.f, "HIDE",       "HIDE",      "GUI","Ctrl+h",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData( 9,26,1,26,"XXXX",                        "XXXX",                        "XXXX",                         0.f, "TAB",        "TAB",       "GUI","Ctrl+t",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData( 9,27,1,27,"XXXX",                        "XXXX",                        "/reset_sound",                 1.f, "RsrSnd",     "Reset Snd",       "GUI","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 )};

std::vector<inOutControllerData> tracks_sideButtonsData =
    { inOutControllerData( 9,25,1,25,"XXXX",                        "XXXX",                        "/hide",                        0.f, "HIDE",       "HIDE",      "GUI","Ctrl+h",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData( 9,26,1,26,"XXXX",                        "XXXX",                        "XXXX",                         0.f, "TAB",        "TAB",       "GUI","Ctrl+t",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 ),
      inOutControllerData( 9,27,1,27,"XXXX",                        "XXXX",                        "/reset_sound",                 1.f, "RsrSnd",     "Reset Snd",       "GUI","",  .7f, .7f, 1.f, 1.f, .7f, .7f, .2f, .2f, 1.f, 1.f, .2f, .2f , 40,  6, 10 )};

/////////////////////////////////////////////////////////////
/// \brief inOutControllerData::inOutControllerData
/// \param typeIn_par
/// \param controlsIn_par
/// \param channelsIn_par
/// \param labelsUDPIn_par
/// \param labelsHighlight_par
/// \param amplitude_par
/// \param title_par
/// \param labelobject_names_par
/// \param rBgOn_par
/// \param gBgOn_par
/// \param bBgOn_par
/// \param rBgOff_par
/// \param gBgOff_par
/// \param bBgOff_par
/// \param rBarOn_par
/// \param gBarOn_par
/// \param bBarOn_par
/// \param rBarOff_par
/// \param gBarOff_par
/// \param bBarOff_par
/// \param size_par
/// \param intervalle_par
/// \param fontSize_par
///
inOutControllerData::inOutControllerData(
        int typeIn_par,
        int controlsIn_par,
        int channelsIn_par,
        int controlsOut_par, // added to In
        QString labelUDPIn_par,
        QString labelsHighlight_par,
        QString labelsUDPOut_par, // added to In
        float amplitude_par,
        QString titleDisplay_par, // added to In
        QString titleController_par,
        QString labelObject_par,
        QString contolKey_par, // added to In
        float rBgOff_par, float gBgOff_par, float bBgOff_par,
        float rBgOn_par, float gBgOn_par, float bBgOn_par,
        float rBarOff_par, float gBarOff_par, float bBarOff_par,
        float rBarOn_par, float gBarOn_par, float bBarOn_par,
        int size_par,
        int intervalle_par,
        int fontSize_par
        ) {
    typeMidiIn = typeIn_par;
    controlMidiIn = controlsIn_par;
    channelMidiIn = channelsIn_par;
    controlMidiOut = controlsOut_par;
    labelUDPIn = labelUDPIn_par;
    labelUDPOut = labelsUDPOut_par;
    labelHighlightIn = labelsHighlight_par;
    amplitude = amplitude_par;
    titleDisplay = titleDisplay_par;
    titleController = titleController_par;
    labelObject = labelObject_par;
    contolKey = contolKey_par;
    bgOffColor.setRgbF(rBgOff_par, gBgOff_par, bBgOff_par);
    bgOnColor.setRgbF(rBgOn_par, gBgOn_par, bBgOn_par);
    barOnColor.setRgbF(rBarOn_par, gBarOn_par, bBarOn_par);
    barOffColor.setRgbF(rBarOff_par, gBarOff_par, bBarOff_par);
    size = size_par;
    intervalle = intervalle_par;
    fontSize = fontSize_par;
 }
inOutControllerData::~inOutControllerData(void) {
}
QString inOutControllerData::get_labelUDPIn() {
    return labelUDPIn;
}
QString  inOutControllerData::get_labelUDPOut() {
    return labelUDPOut;
}
QString inOutControllerData::get_labelHighlightIn() {
    return labelHighlightIn;
}
// MIDI in
int  inOutControllerData::get_typeMidiIn() {
    return typeMidiIn;
}
int  inOutControllerData::get_controlMidiIn() {
    return controlMidiIn;
}
int  inOutControllerData::get_channelMidiIn() {
    return channelMidiIn;
}
int  inOutControllerData::get_controlMidiOut() {
    return controlMidiOut;
}
float  inOutControllerData::get_amplitude() {
    return amplitude;
}
int  inOutControllerData::get_size() {
    return size;
}
// GUI names: displayed name - name used for styling
QString  inOutControllerData::get_titleDisplay() {
    return titleDisplay;
}
QString  inOutControllerData::get_titleController() {
    return titleController;
}
QString  inOutControllerData::get_labelObject() {
    return labelObject;
}
// control Key (for buttons)
QString  inOutControllerData::get_contolKey() {
    return contolKey;
}


