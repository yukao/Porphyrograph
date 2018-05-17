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
#include "pg-interface-core-data.h"

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
// IN/OUT CONTROLLER DATA
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
/// BEAT TAB
/////////////////////////////////////////////////////////////////////////
// FADERS
std::vector<std::vector<ControllerData>> SlidersBeatData =
{
  { ControllerData(11,13,1, 0,"XXXX",                        "XXXX",                        "XXXX",                         1.f, "CA",         "XXXX",          "EMPTY","",0.,  15,  6, 12 ),
    ControllerData(11,13,2, 0,"/flashPartCA_freq_can",       "XXXX",                        "/flashPartCA_freq",            9.f, "Pa",         "PART->CA",      "FLASH","",0.,  15, 10, 12 ),
    ControllerData(11,13,3, 0,"/flashTrkCA_freq_0_can",      "/currentDrawingTrack_can 0",  "/flashTrkCA_freq_0",           9.f, "BG",         "BG->CA",        "FLASH","",0.,  15,  6, 12 ),
    ControllerData(11,13,4, 0,"/flashTrkCA_freq_1_can",      "/currentDrawingTrack_can 1",  "/flashTrkCA_freq_1",           9.f, "T1",         "Tr1->CA",       "FLASH","",0.,  15,  6, 12 ),
    ControllerData(11,13,5, 0,"/flashTrkCA_freq_2_can",      "/currentDrawingTrack_can 2",  "/flashTrkCA_freq_2",           9.f, "T2",         "Tr2->CA",       "FLASH","",0.,  15,  6, 12 ),
    ControllerData(11,13,6, 0,"/flashTrkCA_freq_3_can",      "/currentDrawingTrack_can 3",  "/flashTrkCA_freq_3",           9.f, "T3",         "Tr3->CA",       "FLASH","",0.,  15,  6, 12 )},
  { ControllerData(11,12,1, 0,"/flashCAPart_freq_can",       "XXXX",                        "/flashCAPart_freq",            9.f, "Pa",         "CA->PART",      "FLASH","",0.,  15,  6, 12 ),
    ControllerData(11,12,2, 0,"/flashPixel_duration_can",    "XXXX",                        "/flashPixel_duration",      70.99f, "",           "PIXEL dur",     "PIXEL","",0.,  15, 10, 12 ),
    ControllerData(11,12,3, 0,"/flashTrkPart_freq_0_can",    "/currentDrawingTrack_can 0",  "/flashTrkPart_freq_0",         9.f, "",           "BG->PART",      "FLASH","",0.,  15,  6, 12 ),
    ControllerData(11,12,4, 0,"/flashTrkPart_freq_1_can",    "/currentDrawingTrack_can 1",  "/flashTrkPart_freq_1",         9.f, "",           "Tr1->PART",     "FLASH","",0.,  15,  6, 12 ),
    ControllerData(11,12,5, 0,"/flashTrkPart_freq_2_can",    "/currentDrawingTrack_can 2",  "/flashTrkPart_freq_2",         9.f, "",           "Tr2->PART",     "FLASH","",0.,  15,  6, 12 ),
    ControllerData(11,12,6, 0,"/flashTrkPart_freq_3_can",    "/currentDrawingTrack_can 3",  "/flashTrkPart_freq_3",         9.f, "",           "Tr3->PART",     "FLASH","",0.,  15,  6, 12 )},
  { ControllerData(11,10,1, 0,"/flashCABG_freq_can",         "XXXX",                        "/flashCABG_freq",              9.f, "BG",         "CA->BG",        "FLASH","",0.,  15,  6, 12 ),
    ControllerData(11,10,2, 0,"/flashPartBG_freq_can",       "XXXX",                        "/flashPartBG_freq",            9.f, "",           "PART->BG",      "FLASH","",0.,  15, 10, 12 ),
    ControllerData(11,10,3, 0,"/flashPixel_freq_can",        "/currentDrawingTrack_can 0",  "/flashPixel_freq",             9.f, "",           "PIXEL",         "PIXEL","",0.,  15,  6, 12 ),
    ControllerData(11,10,4, 0,"/flashTrkBG_freq_1_can",         "/currentDrawingTrack_can 1",  "/flashTrkBG_freq_1",           9.f, "",           "Tr1->BG",       "FLASH","",0.,  15,  6, 12 ),
    ControllerData(11,10,5, 0,"/flashTrkBG_freq_2_can",      "/currentDrawingTrack_can 2",  "/flashTrkBG_freq_2",           9.f, "",           "Tr2->BG",       "FLASH","",0.,  15,  6, 12 ),
    ControllerData(11,10,6, 0,"/flashTrkBG_freq_3_can",      "/currentDrawingTrack_can 3",  "/flashTrkBG_freq_3",           9.f, "",           "Tr3->BG",       "FLASH","",0.,  15,  6, 12 )},

  { ControllerData(11,13,7, 0,"/pixel_acc_center_0_can",     "XXXX",                        "/pixel_acc_center_0",          3.f, "CtrX",       "Acc CntrX",     "PIXEL","",0.,  10, 18, 10 ),
    ControllerData(11,13,8, 0,"/pixel_acc_center_1_can",     "XXXX",                        "/pixel_acc_center_1",          1.f, "CtrY",       "Acc CntrY",     "PIXEL","",0.,  10,  8, 10 )},
  { ControllerData( 0, 0,0, 0,"/pulse_low",                  "XXXX",                        "XXXX",                         1.f, "L",          "",              "SOUND","",0.,   6,  2,  7 ),
    ControllerData( 0, 0,0, 0,"/pulse_medium",               "XXXX",                        "XXXX",                         1.f, "M",          "",              "SOUND","",0.,   6,  2,  7 ),
    ControllerData( 0, 0,0, 0,"/pulse_high",                 "XXXX",                        "XXXX",                         1.f, "H",          "",              "SOUND","",0.,   6,  7,  7 ),
    ControllerData( 0, 0,0, 0,"/pulse_enveloppe",            "XXXX",                        "XXXX",                         1.f, "E",          "",              "SOUND","",0.,   6,  0,  7 )},

  { ControllerData(11,12,7, 0,"/pixel_acc_factor_can",       "XXXX",                        "/pixel_acc_factor",           .05f, "Acc",        "PIX acc",       "PIXEL","",0.,  10, 18, 10 ),
    ControllerData(11,12,8, 0,"/pixel_radius_can",           "XXXX",                        "/pixel_radius",                1.f, "Rad",        "PIXEL radius",  "PIXEL","",0.,  10,  8, 10 )},
  { ControllerData(11,10,7, 0,"XXXX",                        "XXXX",                        "XXXX",                         1.f,  "X",         "XXXX",          "EMPTY","",0.,  10, 18, 10 ),
    ControllerData(11,10,8, 0,"XXXX",                        "XXXX",                        "XXXX",                         1.f,  "X",         "XXXX",          "EMPTY","",0.,  10,  8, 10 )},
  { ControllerData(11, 7,1, 0,"/pen_radius_can",             "XXXX",                        "/pen_radius",                 20.f, "Rad",        "PEN radius",      "PEN","",1.,  10, 12, 10 ),
    ControllerData(11, 7,2, 0,"/pen_radius_pressure_coef_can","XXXX",                       "/pen_radius_pressure_coef",   30.f, "Pres",       "PEN press",       "PEN","",0.,  10, 18, 10 ),
    ControllerData(11, 7,3, 0,"/pen_radius_replay_can",      "XXXX",                        "/pen_radius_replay",          20.f, "Repl",       "PEN replay rad",  "PEN","",1.,  10, 18, 10 )},
  { ControllerData(11, 7,4, 0,"/pen_grey_can",               "XXXX",                        "/pen_grey",                    1.f, "Grey",       "PEN grey",        "PEN","",0.,  10, 12, 10 ),
    ControllerData(11, 7,5, 0,"/pen_color_can",              "XXXX",                        "/pen_color",                   1.f, "Pen",        "PEN color",       "PEN","",1.,  10, 12, 10 ),
    ControllerData(11, 7,6, 0,"/repop_color_can",            "XXXX",                        "/repop_color",                 1.f, "Repo",       "REPO color",    "REPOP","",1.,  10, 12, 10 ),
    ControllerData(11, 7,7, 0,"XXXX",                        "XXXX",                        "XXXX",                         1.f,  "X",         "XXXX",          "EMPTY","",0.,  10, 18, 10 ),
    ControllerData(11, 7,8, 0,"XXXX",                        "XXXX",                        "XXXX",                         1.f,  "X",         "XXXX",          "EMPTY","",0.,  10,  8, 10 )},
  { ControllerData(11,14,8, 0,"/sound_volume_can",           "XXXX",                        "/sound_volume",                2.f,  "Vol",        "VOLUME",       "SOUND","",0.,  10, 18, 10 )}
};
std::vector<QString> SlidersBeatData_verticalSlidersRowTitles = {"FLASH", "FLASH", "FLASH", "PIXELS ACCELERATION", "BANDS****", "PIXELS ACCELERATION", "XXXX", "RADIUS", "COLOR", ""};
std::vector<QString> SlidersBeatData_LEDBarRowTitles = {"GRID", "GRID", "GRID", "PIX ACC", "BANDS", "PIX ACC", "XXXX", "RADIUS", "COLOR", "VOL"};

// BUTTONS
//// 1st & 2nd row Beat buttons
std::vector<std::vector<ControllerData>> ButtonsBeatData =
{
  { ControllerData(11, 9,1, 1,"XXXX",                        "/pen_grey_can 1",             "/pen_BW",                      1.f, "?",        "B/W",               "PEN","",0.,  40,  6, 12 )},
  { ControllerData(11, 9,2, 4,"XXXX",                        "/pen_colorPreset_can 0",      "/pen_colorPreset",             0.f, "1",          "1",               "PEN","",0.,  40,  6, 12 ),
    ControllerData(11, 9,3, 7,"XXXX",                        "/pen_colorPreset_can 1",      "/pen_colorPreset",             1.f, "2",          "2",               "PEN","",0.,  40,  6, 10 ),
    ControllerData(11, 9,4,10,"XXXX",                        "/pen_colorPreset_can 2",      "/pen_colorPreset",             2.f, "3",          "3",               "PEN","",0.,  40,  6, 10 ),
    ControllerData(11, 9,5,13,"XXXX",                        "/pen_colorPreset_can 3",      "/pen_colorPreset",             3.f, "4",          "4",               "PEN","",0.,  40,  6, 10 ),
    ControllerData(11, 9,6,16,"XXXX",                        "/pen_colorPreset_can 4",      "/pen_colorPreset",             4.f, "5",          "5",               "PEN","",0.,  40,  6, 10 )},
  { ControllerData(11, 9,7,19,"XXXX",                        "/pixel_mode_can 1",           "/pixel_mode_plus",             1.f, "?",          "PIXEL Mode",    "PIXEL","",0.,  40,  6, 10 ),
    ControllerData(11, 9,8,22,"XXXX",                        "XXXX",                        "XXXX",                         1.f,  "X",         "XXXX",          "EMPTY","",0.,  40,  6, 10 )},
  { ControllerData(11, 8,1, 3,"XXXX",                        "/repop_grey_can 1",           "/repop_BW",                    1.f, "?",          "REPOP B/W",     "REPOP","",0.,  40,  6, 12 )},
  { ControllerData(11, 8,2, 6,"XXXX",                        "/pen_brush_can 0",            "/pen_brush",                   0.f, "Ci",         "Circle",          "PEN","",0.,  40,  6, 12 ),
    ControllerData(11, 8,3, 9,"XXXX",                        "/pen_brush_can 1",            "/pen_brush",                   1.f, "Bo",         "Bone",            "PEN","",0.,  40,  6, 10 ),
    ControllerData(11, 8,4,12,"XXXX",                        "/pen_brush_can 2",            "/pen_brush",                   2.f, "Ho",         "Horse",           "PEN","",0.,  40,  6, 10 ),
    ControllerData(11, 8,5,15,"XXXX",                        "/pen_brush_can 3",            "/pen_brush",                   3.f, "Tr",         "Truck",           "PEN","",0.,  40,  6, 10 ),
    ControllerData(11, 8,6,18,"XXXX",                        "/pen_brush_can 4",            "/pen_brush",                   4.f, "A",          "A",               "PEN","",0.,  40,  6, 10 )},
  { ControllerData(11, 8,7,21,"XXXX",                        "XXXX",                        "/flashCamera",                 1.f, "Ca",         "Flash",         "MEDIA","",0.,  40,  6, 10 ),
    ControllerData(11, 8,8,24,"XXXX",                        "XXXX",                        "/beat",                       -1.f, "Bt",         "Beat",            "GUI","",0.,  40,  6, 10 )},
  { ControllerData( 9,25,1,25,"XXXX",                        "XXXX",                        "XXXX",                         0.f, "PULSE",      "PULSE",     "GUI","Ctrl+p",1.,   40,  6, 10 ),
    ControllerData( 9,26,1,26,"XXXX",                        "XXXX",                        "XXXX",                         0.f, "TAB",        "TAB",       "GUI","Ctrl+t",1.,   40,  6, 10 ),
    ControllerData( 9,27,1,27,"XXXX",                        "XXXX",                        "XXXX",                         0.f, "PATHS",      "PATHS",     "GUI","Ctrl+p",1.,   40,  6, 10 )}
};
std::vector<QString> ButtonsBeatData_ButtonTitles = {"PEN B/W", "PEN PALETTE", "PIXEL", "REPOP B/W", "PEN BRUSH", "FLASH CAMERA/BEAT", ""};
std::vector<QString> ButtonsBeatData_LabelTitles = {"PEN", "PEN PALETTE", "PIXEL", "REP", "PEN BRUSH", "FLASH", "SIDE"};

/////////////////////////////////////////////////////////////////////////
/// MIXER TAB
/////////////////////////////////////////////////////////////////////////
// FADERS
std::vector<std::vector<ControllerData>> SlidersMixerData =
{
  { ControllerData(11,13,1, 0,"/CAMixingWeight_can",         "XXXX",                        "/CAMixingWeight",              1.f, "CA",         "CA Mixing",     "TKWGH","",0.,  10, 11, 10 ),
    ControllerData(11,13,2, 0,"/PartMixingWeight_can",       "XXXX",                        "/PartMixingWeight",            1.f, "Pa",         "PART Mixing",   "TKWGH","",0.,  10, 22, 10 ),
    ControllerData(11,13,3, 0,"/trackMixingWeight_0_can",    "/currentDrawingTrack_can 0",  "/trackMixingWeight_0",         1.f, "BG",         "BG Mixing",     "TKWGH","",0.,  10, 11, 10 ),
    ControllerData(11,13,4, 0,"/trackMixingWeight_1_can",    "/currentDrawingTrack_can 1",  "/trackMixingWeight_1",         1.f, "T1",         "Tr1 Mixing",    "TKWGH","",0.,  10, 11, 10 ),
    ControllerData(11,13,5, 0,"/trackMixingWeight_2_can",    "/currentDrawingTrack_can 2",  "/trackMixingWeight_2",         1.f, "T2",         "Tr2 Mixing",    "TKWGH","",0.,  10, 11, 10 ),
    ControllerData(11,13,6, 0,"/trackMixingWeight_3_can",    "/currentDrawingTrack_can 3",  "/trackMixingWeight_3",         1.f, "T3",         "Tr3 Mixing",    "TKWGH","",0.,  10, 11, 10 )},
  { ControllerData(11,12,1, 0,"/CAMasterWeight_can",         "XXXX",                        "/CAMasterWeight",              1.f, "CA",         "CA Master",     "TKWGH","",0.,  10, 11, 10 ),
    ControllerData(11,12,2, 0,"/PartMasterWeight_can",       "XXXX",                        "/PartMasterWeight",            1.f, "Pa",         "PART Master",   "TKWGH","",0.,  10, 22, 10 ),
    ControllerData(11,12,3, 0,"/trackMasterWeight_0_can",    "/currentDrawingTrack_can 0",  "/trackMasterWeight_0",         1.f, "BG",         "BG Master",     "TKWGH","",0.,  10, 11, 10 ),
    ControllerData(11,12,4, 0,"/trackMasterWeight_1_can",    "/currentDrawingTrack_can 1",  "/trackMasterWeight_1",         1.f, "T1",         "Tr1 Master",    "TKWGH","",0.,  10, 11, 10 ),
    ControllerData(11,12,5, 0,"/trackMasterWeight_2_can",    "/currentDrawingTrack_can 2",  "/trackMasterWeight_2",         1.f, "T2",         "Tr2 Master",    "TKWGH","",0.,  10, 11, 10 ),
    ControllerData(11,12,6, 0,"/trackMasterWeight_3_can",    "/currentDrawingTrack_can 3",  "/trackMasterWeight_3",         1.f, "T3",         "Tr3 Master",    "TKWGH","",0.,  10, 11, 10 )},

  { ControllerData(11,13,7, 0,"/cameraWeight_can",           "XXXX",                        "/cameraWeight",                1.f, "Wgh",        "CAM Wght",      "MEDIA","",0.,  10, 18, 10 ),
    ControllerData(11,13,8, 0,"/cameraCaptFreq_can",         "XXXX",                        "/cameraCaptFreq",             20.f, "Frq",        "CAM freq",      "MEDIA","",0.,  10,  7, 10 )},
  { ControllerData(11,12,7, 0,"/movieWeight_can",            "XXXX",                        "/movieWeight",                 1.f, "Wgh",        "MOV Wght",      "MEDIA","",0.,  10, 18, 10 ),
    ControllerData(11,12,8, 0,"/movieCaptFreq_can",          "XXXX",                        "/movieCaptFreq",              20.f, "Frq",        "MOV freq",      "MEDIA","",0.,  10,  7, 10 )},

  { ControllerData( 0, 0,0, 0,"/pulse_low",                  "XXXX",                        "XXXX",                         1.f, "L",          "",              "SOUND","",0.,   6,  2,  7 ),
    ControllerData( 0, 0,0, 0,"/pulse_medium",               "XXXX",                        "XXXX",                         1.f, "M",          "",              "SOUND","",0.,   6,  2,  7 ),
    ControllerData( 0, 0,0, 0,"/pulse_high",                 "XXXX",                        "XXXX",                         1.f, "H",          "",              "SOUND","",0.,   6,  7,  7 ),
    ControllerData( 0, 0,0, 0,"/pulse_enveloppe",            "XXXX",                        "XXXX",                         1.f, "E",          "",              "SOUND","",0.,   6,  0,  7 )},

  { ControllerData(11,10,1, 0,"/repop_CA_can",               "XXXX",                        "/repop_CA",                    .9f,  "CA",        "REPO CA",       "REPOP","",0.,  10, 11, 10 ),
    ControllerData(11,10,2, 0,"/repop_part_can",             "XXXX",                        "/repop_part",                  .9f,  "Pa",        "REPO Part",     "REPOP","", 1.,  10, 11, 10 ),
    ControllerData(11,10,3, 0,"/repop_BG_can",               "XXXX",                        "/repop_BG",                    .9f,  "BG",        "REPO BG",       "REPOP","",0.,  10, 11, 10 ),
    ControllerData(11,10,4, 0,"/repop_path_can",             "XXXX",                        "/repop_path",                  .9f,  "Pth",       "REPO Path",     "REPOP","",0.,  10,  3, 10 )},
  { ControllerData(11,10,5, 0,"/photo_value_can",            "XXXX",                        "/photo_value",                 3.f, "Val",        "PHOT Value",    "MEDIA","", 1.,  10,  3, 10 ),
    ControllerData(11,10,6, 0,"/photo_satur_can",            "XXXX",                        "/photo_satur",                 3.f, "Sat",        "PHOT Satur",    "MEDIA","", 1.,  10,  6, 10 )},
  { ControllerData(11,10,7, 0,"/photoWeight_can",            "XXXX",                        "/photoWeight",                 1.f, "Wgh",        "PHOT Wght",     "MEDIA","",0.,  10, 18, 10 ),
    ControllerData(11,10,8, 0,"XXXX",                        "XXXX",                        "XXXX",                         1.f,  "X",         "XXXX",          "EMPTY","",0.,  10,  7, 10 )},

  { ControllerData(11, 7,1, 0,"/CAdecay_can",                "/CAdecay_sign_can -1",        "/CAdecay",                     .2f,  "CA",        "CA decay",      "TRACK","",10.,  10, 11, 10 ),
    ControllerData(11, 7,2, 0,"/partDecay_can",              "/partDecay_sign_can -1",      "/partDecay",                 0.01f,  "Pa",        "PART decay",    "TRACK","",0.,  10, 22, 10 ),
    ControllerData(11, 7,3, 0,"/trkDecay_0_can",             "/trkDecay_sign_0_can -1",     "/trkDecay_0",                0.01f,  "BG",        "BG decay",      "TRACK","",0.,  10, 11, 10 ),
    ControllerData(11, 7,4, 0,"/trkDecay_1_can",             "/trkDecay_sign_1_can -1",     "/trkDecay_1",                0.01f,  "T1",        "Tr1 decay",     "TRACK","",0.,  10, 11, 10 ),
    ControllerData(11, 7,5, 0,"/trkDecay_2_can",             "/trkDecay_sign_2_can -1",     "/trkDecay_2",                0.01f,  "T2",        "Tr2 decay",     "TRACK","",0.,  10, 11, 10 ),
    ControllerData(11, 7,6, 0,"/trkDecay_3_can",             "/trkDecay_sign_3_can -1",     "/trkDecay_3",                0.01f,  "T3",        "Tr3 decay",     "TRACK","",0.,  10,  6, 10 )},
  { ControllerData(11, 7,7, 0,"/echoNeg_can",                "XXXX",                        "/echoNeg",                     1.f,  "-",         "ECHO Neg",       "ECHO","",0.,  10, 11, 10 ),
    ControllerData(11, 7,8, 0,"/echo_can",                   "XXXX",                        "/echo",                        1.f,  "+",         "ECHO Pos",       "ECHO","",0.,  10,  7, 10 )},
  { ControllerData(11,14,8, 0,"/blendTransp_can",            "XXXX",                        "/blendTransp",                 1.f, "Blk",        "MASTER",        "TKWGH","",0.,  10, 11, 10 )}
};
std::vector<QString> SlidersMixerData_verticalSlidersRowTitles = {"TRACK WEIGHTS","TRACK WEIGHTS","CAMERA","MOVIE","BANDS","REPOP","PHOTO","PHOTO","DECAY","ECHO",""};
std::vector<QString> SlidersMixerData_LEDBarRowTitles = {"MIXING W","MASTER W","CAMERA","MOVIE","BANDS","REPOP","PHOTO","PHOTO","DECAY","ECHO","MAS"};

// BUTTONS
//// 1st & 2nd row Mixer buttons
std::vector<std::vector<ControllerData>> ButtonsMixerData =
{
  { ControllerData(11, 9,1, 1,"XXXX",                        "XXXX",                        "/currentPhotoTrack_plus",      1.f, "Pho",        "Photo Tk +",    "MEDIA","",0.,  40,  6, 10 ),
    ControllerData(11, 9,2, 4,"XXXX",                        "XXXX",                        "/currentVideoTrack_plus",      1.f, "Vid",        "Video Tk +",    "MEDIA","",0.,  40,  6, 10 ),
    ControllerData(11, 9,3, 7,"XXXX",                        "/currentDrawingTrack_can 0",  "/currentDrawingTrack",         0.f, "BG",         "Track BG",      "TRACK","",0.,  40,  6, 10 ),
    ControllerData(11, 9,4,10,"XXXX",                        "/currentDrawingTrack_can 1",  "/currentDrawingTrack",         1.f, "T1",         "Track 1",       "TRACK","",0.,  40,  6, 10 ),
    ControllerData(11, 9,5,13,"XXXX",                        "/currentDrawingTrack_can 2",  "/currentDrawingTrack",         2.f, "T2",         "Track 2",       "TRACK","",0.,  40,  6, 10 ),
    ControllerData(11, 9,6,16,"XXXX",                        "/currentDrawingTrack_can 3",  "/currentDrawingTrack",         3.f, "T3",         "Track 3",       "TRACK","",0.,  40,  6, 10 )},
  { ControllerData(11, 9,7,19,"XXXX",                        "XXXX",                        "/copyTrack_under",             1.f, "Cp-",        "Copy under",      "PEN","",0.,  40,  6, 10 ),
    ControllerData(11, 9,8,22,"XXXX",                        "XXXX",                        "/copyTrack_above",             1.f, "Cp+",        "Copy above",      "PEN","",0.,  40,  6, 10 )},
  { ControllerData(11, 8,1, 3,"XXXX",                        "/CAdecay_sign_can -1",        "/CAdecay_sign",                1.f, "CA",         "CA +/-",        "TRACK","",0.,  40,  6, 10 ),
    ControllerData(11, 8,2, 6,"XXXX",                        "/partDecay_sign_can -1",      "/partDecay_sign",              1.f, "Pa",         "PART +/-",      "TRACK","",0.,  40,  6, 10 ),
    ControllerData(11, 8,3, 9,"XXXX",                        "/trkDecay_sign_0_can -1",     "/trkDecay_sign_0",             1.f, "BG",         "BG +/-",        "TRACK","",0.,  40,  6, 10 ),
    ControllerData(11, 8,4,12,"XXXX",                        "/trkDecay_sign_1_can -1",     "/trkDecay_sign_1",             1.f, "T1",         "TK1 +/-",       "TRACK","",0.,  40,  6, 10 ),
    ControllerData(11, 8,5,15,"XXXX",                        "/trkDecay_sign_2_can -1",     "/trkDecay_sign_2",             1.f, "T2",         "TK2 +/-",       "TRACK","",0.,  40,  6, 10 ),
    ControllerData(11, 8,6,18,"XXXX",                        "/trkDecay_sign_3_can -1",     "/trkDecay_sign_3",             1.f, "T3",         "TK3 +/-",       "TRACK","",0.,  40,  6, 10 )},
  { ControllerData(11, 8,7,21,"XXXX",                        "/freeze_can 1",               "/freeze",                      1.f, "Off",        "OFF",             "GUI","",0.,  40,  6, 10 ),
    ControllerData(11, 8,8,24,"XXXX",                        "/invertAllLayers_can 1",      "/invertAllLayers",             1.f, "INV",        "Invert All",      "GUI","",0.,  40,  6, 10 )},
  { ControllerData( 9,25,1,25,"XXXX",                        "XXXX",                        "XXXX",                         0.f, "PULSE",      "PULSE",     "GUI","Ctrl+h",1.,   40,  6, 10 ),
    ControllerData( 9,26,1,26,"XXXX",                        "XXXX",                        "XXXX",                         0.f, "TAB",        "TAB",       "GUI","Ctrl+t",1.,   40,  6, 10 ),
    ControllerData( 9,27,1,27,"XXXX",                        "XXXX",                        "XXXX",                         0.f, "PATHS",      "PATHS",     "GUI","Ctrl+p",1.,   40,  6, 10 )}
};
std::vector<QString> ButtonsMixerData_ButtonTitles = {"TRACKS", "COPY", "DECAY SIGN", "FREEZE/INVERT", ""};
std::vector<QString> ButtonsMixerData_LabelTitles = {"TRACKS", "COPY", "DECAY SIGN", "FRZE/INV", "SIDE"};

/////////////////////////////////////////////////////////////////////////
/// FX TAB
/////////////////////////////////////////////////////////////////////////
// FADERS
std::vector<std::vector<ControllerData>> SlidersFXData =
{
  { ControllerData(11,13,1, 0,"/CAParams1_can",              "XXXX",                        "/CAParams1",                   1.f, "Par1",       "Param1",          "PEN","",0.,  10, 11, 10 ),
    ControllerData(11,13,2, 0,"/CAParams2_can",              "XXXX",                        "/CAParams2",                   1.f, "Par2",       "Param2",          "PEN","",0.,  10, 22, 10 ),
    ControllerData(11,13,3, 0,"/CAParams3_can",              "XXXX",                        "/CAParams3",                   1.f, "Par3",       "Param3",          "PEN","",0.,  10, 11, 10 ),
    ControllerData(11,13,4, 0,"/CAParams4_can",              "XXXX",                        "/CAParams4",                   1.f, "Par4",       "Param4",          "PEN","",0.,  10, 11, 10 ),
    ControllerData(11,13,5, 0,"/CAParams5_can",              "XXXX",                        "/CAParams5",                   1.f, "Par5",       "Param5",          "PEN","",0.,  10, 11, 10 ),
    ControllerData(11,13,6, 0,"/CAParams6_can",              "XXXX",                        "/CAParams6",                   1.f, "Par6",       "Param6",          "PEN","",0.,  10, 11, 10 ),
    ControllerData(11,13,7, 0,"/CAParams7_can",              "XXXX",                        "/CAParams7",                   1.f, "Par7",       "Param7",          "PEN","",0.,  10, 22, 10 ),
    ControllerData(11,13,8, 0,"/CAParams8_can",              "XXXX",                        "/CAParams8",                   1.f, "Par8",       "Param8",          "PEN","",0.,  10, 11, 10 )},

  { ControllerData(11,12,1, 0,"/cameraThreshold_can",        "XXXX",                        "/cameraThreshold",             1.f, "Thr",        "Thresh",        "MEDIA","",0.,  10, 22, 10 ),
    ControllerData(11,12,2, 0,"/cameraWB_can",               "XXXX",                        "/cameraWB",                  128.f, "WB",         "WB",            "MEDIA","",0.,  10, 11, 10 ),
    ControllerData(11,12,3, 0,"/cameraGamma_can",            "XXXX",                        "/cameraGamma",                 1.f, "Gam",        "Gamma",         "MEDIA","",0.,  10, 11, 10 ),
    ControllerData(11,12,4, 0,"/cameraExposure_can",         "XXXX",                        "/cameraExposure",             10.f, "Exp",        "Expos",         "MEDIA","",0.,  10, 11, 10 ),
    ControllerData(11,12,5, 0,"/cameraGain_can",             "XXXX",                        "/cameraGain",                128.f, "Gain",       "Gain",          "MEDIA","",0.,  10, 11, 10 ),
    ControllerData(11,12,6, 0,"/cameraBrightness_can",       "XXXX",                        "/cameraBrightness",          128.f, "Brg",        "Bright",        "MEDIA","",0.,  10, 11, 10 ),
    ControllerData(11,12,7, 0,"/cameraSaturation_can",       "XXXX",                        "/cameraSaturation",          128.f, "Sat",        "Satur",         "MEDIA","",0.,  10, 22, 10 ),
    ControllerData(11,12,8, 0,"/cameraContrast_can",         "XXXX",                        "/cameraContrast",            128.f, "Con",        "Contr",         "MEDIA","",0.,  10, 11, 10 )},

  { ControllerData( 0, 0,0, 0,"/pulse_low",                  "XXXX",                        "XXXX",                         1.f, "L",          "",              "SOUND","",0.,   6,  2,  7 ),
    ControllerData( 0, 0,0, 0,"/pulse_medium",               "XXXX",                        "XXXX",                         1.f, "M",          "",              "SOUND","",0.,   6,  2,  7 ),
    ControllerData( 0, 0,0, 0,"/pulse_high",                 "XXXX",                        "XXXX",                         1.f, "H",          "",              "SOUND","",0.,   6,  7,  7 ),
    ControllerData( 0, 0,0, 0,"/pulse_enveloppe",            "XXXX",                        "XXXX",                         1.f, "E",          "",              "SOUND","",0.,   6,  0,  7 )},

  { ControllerData(11,10,1, 0,"/noiseScale_can",             "XXXX",                        "/noiseScale",                  1.f,  "Sca",       "Noise Scal",     "PART","",0.,  10, 11, 10 ),
    ControllerData(11,10,2, 0,"/noiseLineScale_can",         "XXXX",                        "/noiseLineScale",              1.f,  "LSc",       "Lin Sc",         "PART","",0.,  10, 11, 10 ),
    ControllerData(11,10,3, 0,"/noiseAngleScale_can",        "XXXX",                        "/noiseAngleScale",             1.f,  "An",        "Angl",           "PART","",0.,  10, 11, 10 ),
    ControllerData(11,10,4, 0,"/noiseCenter_0_can",          "XXXX",                        "/noiseCenter_0",               2.f,  "CtX",       "CentX",          "PART","",0.,  10, 11, 10 ),
    ControllerData(11,10,5, 0,"/noiseCenter_1_can",          "XXXX",                        "/noiseCenter_1",               2.f,  "CtY",       "CzntY",          "PART","",0.,  10,  6, 10 )},
  { ControllerData(11,10,6, 0,"/video_satur_can",            "XXXX",                        "/video_satur",                 1.f,  "Sat",       "VIDEO Sat",     "MEDIA","",0.,  10, 11, 10 )},
  { ControllerData(11,10,7, 0,"/movieSobel_can",             "XXXX",                        "/movieSobel",                  1.f,  "Mov",       "MOV Sobel",     "MEDIA","",0.,  10, 18, 10 ),
    ControllerData(11,10,8, 0,"/cameraSobel_can",            "XXXX",                        "/cameraSobel",                 1.f,  "Ca",        "CAM Sobel",     "MEDIA","",0.,  10,  7, 10 )},

  { ControllerData(11, 7,1, 0,"/beat_threshold_can",         "XXXX",                        "/beat_threshold",              2.f, "Thrs",       "threshold",      "BEAT","",0.,  10, 18, 10 ),
    ControllerData(11, 7,2, 0,"/beat_delay_can",             "XXXX",                        "/beat_delay",               2000.f, "Del",        "delay",          "BEAT","",0.,  10, 18, 10 ),
    ControllerData(11, 7,3, 0,"/auto_beat_duration_can",     "XXXX",                        "/auto_beat_duration",          1.f, "Dur",        "auto dur",       "BEAT","",0.,  10, 18, 10 )},
  { ControllerData(11, 7,4, 0,"/sound_min_can",              "XXXX",                        "/sound_min",                   1.f, "Min",        "Sound Min",     "SOUND","",0.,  10, 12, 10 ),
    ControllerData(11, 7,5, 0,"XXXX",                        "XXXX",                        "XXXX",                         1.f,  "X",         "XXXX",          "EMPTY","",0.,  10, 14, 10 ),
    ControllerData(11, 7,6, 0,"/sound_env_min_can",          "XXXX",                        "/sound_env_min",             100.f, "ThMin",      "Thres Min",     "SOUND","",0.,  10, 18, 10 ),
    ControllerData(11, 7,7, 0,"/sound_env_max_can",          "XXXX",                        "/sound_env_max",             100.f, "ThMax",      "Thres Max",     "SOUND","",0.,  10,  8, 10 ),
    ControllerData(11, 7,8, 0,"/sensor_vol_can",             "XXXX",                        "/sensor_vol",                  2.f, "Sens",       "Sensor Vol",   "SENSOR","",0.,  10, 12, 10 )},

  { ControllerData(11,14,8, 0,"/sound_volume_can",           "XXXX",                        "/sound_volume",                2.f,  "Vol",        "VOLUME",       "SOUND","",0.,  10, 11, 10 )}
};
std::vector<QString> SlidersFXData_verticalSlidersRowTitles = {"CA PARAMS","CAMERA SETUP","BANDS","NOISE","VIDEO","MEDIA SOBEL","BEAT AND SND AUTOM","SOUND VOL",""};
std::vector<QString> SlidersFXData_LEDBarRowTitles = {"CA PARAMS","CAMERA SETUP","BANDS","NOISE","VI","SOBEL","BEAT","SOUND","VOL"};

// BUTTONS
//// 1st & 2nd row FX buttons
std::vector<std::vector<ControllerData>> ButtonsFXData =
{
  { ControllerData(11, 9,1, 1,"XXXX",                        "/CASubType_can 1",            "/CAonOff",                     1.f, "ST",         "CA On/Off",      "PART","",0.,  40,  6, 10 ),
    ControllerData(11, 9,2, 4,"XXXX",                        "/CAType_can 0",               "/CA1Type",                     0.f, "C0",         "CA_0",           "PART","",0.,  40,  6, 10 ),
    ControllerData(11, 9,3, 7,"XXXX",                        "/CAType_can 1",               "/CA1Type",                     1.f, "C1",         "CA_1",           "PART","",0.,  40,  6, 10 ),
    ControllerData(11, 9,4,10,"XXXX",                        "XXXX",                        "/CA1Type_plus",                3.f, "T+",         "CA +",           "PART","",0.,  40,  6, 10 ),
    ControllerData(11, 9,5,13,"XXXX",                        "XXXX",                        "/CA1SubType_plus",             3.f, "S+",         "CA +",           "PART","",0.,  40,  6, 10 )},
  { ControllerData(11, 9,6,16,"XXXX",                        "XXXX",                        "/diaporama_plus",              1.f, "D+",         "DIAPO +",       "MEDIA","",0.,  40,  6, 10 ),
    ControllerData(11, 9,7,19,"XXXX",                        "XXXX",                        "/movie_plus",                  1.f, "M+",         "MOVIE +",       "MEDIA","",0.,  40,  6, 10 )},
  { ControllerData(11, 9,8,22,"XXXX",                        "XXXX",                        "/cameraCumul_plus",            1.f, "Cu",         "CAM cum",       "MEDIA","",0.,  40,  6, 10 )},
  { ControllerData(11, 8,1, 3,"XXXX",                        "/sensor_layout_can 0",        "/sensor_layout_plus",          1.f, "Ly+",        "Layout+",      "SENSOR","",0.,  40,  6, 10 ),
    ControllerData(11, 8,2, 6,"XXXX",                        "/sample_setUp_can 0",         "/sample_setUp_plus",           1.f, "Sm+",        "Sample+",      "SENSOR","",0.,  40,  6, 10 ),
    ControllerData(11, 8,3, 9,"XXXX",                        "/sensor_activation_can 1",    "/sensor_activation_plus",      1.f, "+/-",        "On/Off",       "SENSOR","",0.,  40,  6, 10 ),
    ControllerData(11, 8,4,12,"XXXX",                        "XXXX",                        "XXXX",                         1.f,  "X",         "XXXX",          "EMPTY","",0.,  40,  6, 10 )},
  { ControllerData(11, 8,5,15,"XXXX",                        "/noiseType_can 0",            "/noiseType_plus",              1.f,  "Typ",       "NOISE +",        "PART","",0.,  40,  6, 10 )},
  { ControllerData(11, 8,6,18,"XXXX",                        "/adc_onOff_can 1",            "/adc_onOff",                   1.f, "X",          "Mic On/Off",    "SOUND","",0.,  40,  6, 10 )},
  { ControllerData(11, 8,7,21,"XXXX",                        "/soundtrack_onOff_can 1",     "/soundtrack_plus",             1.f, "+",          "Next",          "SOUND","",0.,  40,  6, 10 ),
    ControllerData(11, 8,8,24,"XXXX",                        "/soundtrack_onOff_can 1",     "/soundtrack_onOff",            1.f, "X",          "On/Off",        "SOUND","",0.,  40,  6, 10 )},
  { ControllerData( 9,25,1,25,"XXXX",                        "XXXX",                        "XXXX",                         0.f, "PULSE",      "PULSE",     "GUI","Ctrl+h",1.,   40,  6, 10 ),
    ControllerData( 9,26,1,26,"XXXX",                        "XXXX",                        "XXXX",                         0.f, "TAB",        "TAB",       "GUI","Ctrl+t",1.,   40,  6, 10 ),
    ControllerData( 9,27,1,27,"XXXX",                        "XXXX",                        "XXXX",                         0.f, "PATHS",      "PATHS",     "GUI","Ctrl+p",1.,   40,  6, 10 )},
};
std::vector<QString> ButtonsFXData_ButtonTitles = {"CA TYPE", "PHOTO/MOVIE +", "CAM cumul",  "SENSOR Lay/Sampl/Act","NOISE", "MIC ON/OFF", "SAMPLES",   ""};
std::vector<QString> ButtonsFXData_LabelTitles = {"CA TYPE", "Pho/Mov+", "Cam", "SENSOR Lay/Sampl/Act","NOISE", "MIC", "SAMPLES",  "SIDE"};

/////////////////////////////////////////////////////////////////////////
/// PATHS TAB
/////////////////////////////////////////////////////////////////////////
// FADERS
std::vector<std::vector<ControllerData>> SlidersPathsData =
{
  { ControllerData(11,48,1, 0,"/part_radius_can",            "XXXX",                        "/part_radius",                 6.f, "All",        "AllRad",         "PART","", 1.,  10, 18, 10 ),
    ControllerData(11,49,1, 0,"/partRepopRadius_can",        "XXXX",                        "/partRepopRadius",             6.f, "Repop",      "RepopRad",       "PART","",0.,  10, 18, 10 ),
    ControllerData(11,50,1, 0,"/particle_type_can",          "XXXX",                        "/particle_type",              3.9f, "Shape",      "Shape",          "PART","",0.,  10, 12, 10 )},
  { ControllerData(11,51,1, 0,"/part_acc_can",               "XXXX",                        "/part_acc",                    .1f, "Acc",        "Acc",            "PART","", 1.,  10, 18, 10 ),
    ControllerData(11,52,1, 0,"/part_damp_factor_can",       "XXXX",                        "/part_damp_factor",           .01f, "Damp",       "Damp",           "PART","",0.,  10, 18, 10 ),
    ControllerData(11,53,1, 0,"/part_damp_targtRad_can",     "XXXX",                        "/part_damp_targtRad",       1000.f, "Tget",       "Damp Tgt",       "PART","",0.,  10, 12, 10 )},
  { ControllerData(11,54,1, 0,"/part_field_weight_can",      "XXXX",                        "/part_field_weight",           1.f, "Weight",     "Field Wght",     "PART","",0.,  10, 18, 10 ),
    ControllerData(11,55,1, 0,"/part_timeToTargt_can",       "XXXX",                        "/part_timeToTargt",          300.f, "SpeedTgt",   "Time to Tgt",    "PART","",0.,  10, 12, 10 ),
    ControllerData(11,56,1, 0,"XXXX",                        "XXXX",                        "XXXX",                         1.f, "XX",         "XXXX",          "EMPTY","",0.,  10, 18, 10 )},
};
std::vector<QString> SlidersPathsData_verticalSlidersRowTitles = {"PART RADIUS/SHAPE", "PART FLUIDITY","PART NOISE"};
std::vector<QString> SlidersPathsData_LEDBarRowTitles = {"PART RAD/SHP", "PART FLUID","PART NOISE"};

// BUTTONS
//// 1st & 2nd row paths buttons
std::vector<std::vector<ControllerData>> ButtonsPathsData =
{
  { ControllerData( 9,56,1,56,"XXXX",                        "/partMove_target_can 1",      "/partMove_target",             1.f, "Tgt",        "TARGET",         "PART","",0.,  40,  6, 10 ),
    ControllerData( 9,57,1,57,"XXXX",                        "/partMove_rand_can 1",        "/partMove_rand",               1.f, "Rand",       "RANDOM",         "PART","",0.,  40,  6, 10 ),
    ControllerData( 9,58,1,58,"XXXX",                        "XXXX",                        "XXXX",                         1.f, "X",          "XXXX",          "EMPTY","",0.,  40,  6, 10 ),
    ControllerData( 9,59,1,59,"XXXX",                        "XXXX",                        "XXXX",                         1.f, "X",          "XXXX",          "EMPTY","",0.,  40,  6, 10 )},
  { ControllerData( 9,60,1,60,"XXXX",                        "/partStroke_mode_can 0",      "/partStroke_mode",             0.f, "X",          "None",           "PART","",0.,  40,  6, 10 ),
    ControllerData( 9,61,1,61,"XXXX",                        "/partStroke_mode_can 1",      "/partStroke_mode",             1.f, "Agr",        "Aggreg",         "PART","",0.,  40,  6, 10 ),
    ControllerData( 9,62,1,62,"XXXX",                        "/partStroke_mode_can 2",      "/partStroke_mode",             2.f, "Bnc",        "Bounce",         "PART","",0.,  40,  6, 10 ),
    ControllerData( 9,63,1,63,"XXXX",                        "/partStroke_mode_can 3",      "/partStroke_mode",             3.f, "Dmp",        "Damp",           "PART","",0.,  40,  6, 10 )},
  { ControllerData( 9,48,1,48,"XXXX",                        "/partExit_mode_can 0",        "/partExit_mode",               0.f, "X",          "None",           "PART","",0.,  40,  6, 10 ),
    ControllerData( 9,49,1,49,"XXXX",                        "/partExit_mode_can 1",        "/partExit_mode",               1.f, "Opp",        "Oppos",          "PART","",0.,  40,  6, 10 ),
    ControllerData( 9,50,1,50,"XXXX",                        "/partExit_mode_can 2",        "/partExit_mode",               2.f, "Bce",        "Bounce",         "PART","",0.,  40,  6, 10 ),
    ControllerData( 9,51,1,51,"XXXX",                        "/partExit_mode_can 3",        "/partExit_mode",               3.f, "X",          "XXXX",          "EMPTY","",0.,  40,  6, 10 )},
  { ControllerData( 9,52,1,52,"XXXX",                        "/partColor_mode_can 0",       "/partColor_mode",              0.f, "White",      "White",          "PART","",0.,  40,  6, 10 ),
    ControllerData( 9,53,1,53,"XXXX",                        "/partColor_mode_can 1",       "/partColor_mode",              1.f, "Max",        "Max",            "PART","",0.,  40,  6, 10 ),
    ControllerData( 9,54,1,54,"XXXX",                        "/partColor_mode_can 2",       "/partColor_mode",              2.f, "Cpy",        "Copy",           "PART","",0.,  40,  6, 10 ),
    ControllerData( 9,55,1,55,"XXXX",                        "/partColor_mode_can 3",       "/partColor_mode",              3.f, "X",          "XXXX",          "EMPTY","",0.,  40,  6, 10 )},
  { ControllerData( 9,40,1,40,"XXXX",                        "/part_initialization_can 0",  "/part_initialization",         0.f, "0",          "0",              "PART","",0.,  40,  6, 10 ),
    ControllerData( 9,41,1,41,"XXXX",                        "/part_initialization_can 1",  "/part_initialization",         1.f, "1",          "1",              "PART","",0.,  40,  6, 10 ),
    ControllerData( 9,42,1,42,"XXXX",                        "/part_initialization_can 2",  "/part_initialization",         2.f, "2",          "2",              "PART","",0.,  40,  6, 10 ),
    ControllerData( 9,43,1,43,"XXXX",                        "/part_initialization_can 3",  "/part_initialization",         3.f, "3",          "3",              "PART","",0.,  40,  6, 10 ),
    ControllerData( 9,44,1,44,"XXXX",                        "/part_initialization_can 4",  "/part_initialization",         4.f, "4",          "4",              "PART","",0.,  40,  6, 10 ),
    ControllerData( 9,45,1,45,"XXXX",                        "/part_initialization_can 5",  "/part_initialization",         5.f, "5",          "5",              "PART","",0.,  40,  6, 10 ),
    ControllerData( 9,46,1,46,"XXXX",                        "/part_initialization_can 6",  "/part_initialization",         6.f, "6",          "6",              "PART","",0.,  40,  6, 10 ),
    ControllerData( 9,47,1,47,"XXXX",                        "/part_initialization_can 7",  "/part_initialization",         7.f, "7",          "7",              "PART","",0.,  40,  6, 10 )},
  { ControllerData( 9,32,1,32,"XXXX",                        "/path_repop_0_can 1",         "/path_repop_0",                1.f, "0",          "0",              "PART","",0.,  40,  6, 10 ),
    ControllerData( 9,33,1,33,"XXXX",                        "/path_repop_1_can 1",         "/path_repop_1",                1.f, "1",          "1",              "PART","",0.,  40,  6, 10 ),
    ControllerData( 9,34,1,34,"XXXX",                        "/path_repop_2_can 1",         "/path_repop_2",                1.f, "2",          "2",              "PART","",0.,  40,  6, 10 ),
    ControllerData( 9,35,1,35,"XXXX",                        "/path_repop_3_can 1",         "/path_repop_3",                1.f, "3",          "3",              "PART","",0.,  40,  6, 10 ),
    ControllerData( 9,36,1,36,"XXXX",                        "/path_repop_4_can 1",         "/path_repop_4",                1.f, "4",          "4",              "PART","",0.,  40,  6, 10 ),
    ControllerData( 9,37,1,37,"XXXX",                        "/path_repop_5_can 1",         "/path_repop_5",                1.f, "5",          "5",              "PART","",0.,  40,  6, 10 ),
    ControllerData( 9,38,1,38,"XXXX",                        "/path_repop_6_can 1",         "/path_repop_6",                1.f, "6",          "6",              "PART","",0.,  40,  6, 10 ),
    ControllerData( 9,39,1,39,"XXXX",                        "/path_repop_7_can 1",         "/path_repop_7",                1.f, "7",          "7",              "PART","",0.,  40,  6, 10 )},
  { ControllerData( 9,24,1,24,"XXXX",                        "/part_path_follow_0_can 1",   "/part_path_follow_0",          1.f, "0",          "0",              "PART","",0.,  40,  6, 10 ),
    ControllerData( 9,25,1,25,"XXXX",                        "/part_path_follow_1_can 1",   "/part_path_follow_1",          1.f, "1",          "1",              "PART","",0.,  40,  6, 10 ),
    ControllerData( 9,26,1,26,"XXXX",                        "/part_path_follow_2_can 1",   "/part_path_follow_2",          1.f, "2",          "2",              "PART","",0.,  40,  6, 10 ),
    ControllerData( 9,27,1,27,"XXXX",                        "/part_path_follow_3_can 1",   "/part_path_follow_3",          1.f, "3",          "3",              "PART","",0.,  40,  6, 10 ),
    ControllerData( 9,28,1,28,"XXXX",                        "/part_path_follow_4_can 1",   "/part_path_follow_4",          1.f, "4",          "4",              "PART","",0.,  40,  6, 10 ),
    ControllerData( 9,29,1,29,"XXXX",                        "/part_path_follow_5_can 1",   "/part_path_follow_5",          1.f, "5",          "5",              "PART","",0.,  40,  6, 10 ),
    ControllerData( 9,30,1,30,"XXXX",                        "/part_path_follow_6_can 1",   "/part_path_follow_6",          1.f, "6",          "6",              "PART","",0.,  40,  6, 10 ),
    ControllerData( 9,31,1,31,"XXXX",                        "/part_path_follow_7_can 1",   "/part_path_follow_7",          1.f, "7",          "7",              "PART","",0.,  40,  6, 10 )},
  { ControllerData( 9,16,1,16,"XXXX",                        "/part_path_repulse_0_can 1",  "/part_path_repulse_0",         1.f, "0",          "0",              "PART","",0.,  40,  6, 10 ),
    ControllerData( 9,17,1,17,"XXXX",                        "/part_path_repulse_1_can 1",  "/part_path_repulse_1",         1.f, "1",          "1",              "PART","",0.,  40,  6, 10 ),
    ControllerData( 9,18,1,18,"XXXX",                        "/part_path_repulse_2_can 1",  "/part_path_repulse_2",         1.f, "2",          "2",              "PART","",0.,  40,  6, 10 ),
    ControllerData( 9,19,1,19,"XXXX",                        "/part_path_repulse_3_can 1",  "/part_path_repulse_3",         1.f, "3",          "3",              "PART","",0.,  40,  6, 10 ),
    ControllerData( 9,20,1,20,"XXXX",                        "/part_path_repulse_4_can 1",  "/part_path_repulse_4",         1.f, "4",          "4",              "PART","",0.,  40,  6, 10 ),
    ControllerData( 9,21,1,21,"XXXX",                        "/part_path_repulse_5_can 1",  "/part_path_repulse_5",         1.f, "5",          "5",              "PART","",0.,  40,  6, 10 ),
    ControllerData( 9,22,1,22,"XXXX",                        "/part_path_repulse_6_can 1",  "/part_path_repulse_6",         1.f, "6",          "6",              "PART","",0.,  40,  6, 10 ),
    ControllerData( 9,23,1,23,"XXXX",                        "/part_path_repulse_7_can 1",  "/part_path_repulse_7",         1.f, "7",          "7",              "PART","",0.,  40,  6, 10 )},
  { ControllerData( 9, 8,1, 8,"XXXX",                        "XXXX",                        "XXXX",                         1.f, "X",          "XXXX",          "EMPTY","",0.,  40,  6, 10 ),
    ControllerData( 9, 9,1, 9,"XXXX",                        "/path_record_1_can 1",        "/path_record_1",               1.f, "1",          "1",              "PATH","",0.,  40,  6, 10 ),
    ControllerData( 9,10,1,10,"XXXX",                        "/path_record_2_can 1",        "/path_record_2",               1.f, "2",          "2",              "PATH","",0.,  40,  6, 10 ),
    ControllerData( 9,11,1,11,"XXXX",                        "/path_record_3_can 1",        "/path_record_3",               1.f, "3",          "3",              "PATH","",0.,  40,  6, 10 ),
    ControllerData( 9,12,1,12,"XXXX",                        "/path_record_4_can 1",        "/path_record_4",               1.f, "4",          "4",              "PATH","",0.,  40,  6, 10 ),
    ControllerData( 9,13,1,13,"XXXX",                        "/path_record_5_can 1",        "/path_record_5",               1.f, "5",          "5",              "PATH","",0.,  40,  6, 10 ),
    ControllerData( 9,14,1,14,"XXXX",                        "/path_record_6_can 1",        "/path_record_6",               1.f, "6",          "6",              "PATH","",0.,  40,  6, 10 ),
    ControllerData( 9,15,1,15,"XXXX",                        "/path_record_7_can 1",        "/path_record_7",               1.f, "7",          "7",              "PATH","",0.,  40,  6, 10 )},
  { ControllerData( 9, 0,1, 0,"XXXX",                        "XXXX",                        "XXXX",                         1.f, "X",          "XXXX",          "EMPTY","",0.,  40,  6, 10 ),
                                                             // does not work because the number can be 0 to PG_NB_TRACKS (only works if replayed on track 1)
    ControllerData( 9, 1,1, 1,"XXXX",                        "/path_replay_trackNo_1_can 1","/path_replay_trackNo_1",       1.f, "1",          "1",              "PATH","",0.,  40,  6, 10 ),
    ControllerData( 9, 2,1, 2,"XXXX",                        "/path_replay_trackNo_2_can 1","/path_replay_trackNo_2",       1.f, "2",          "2",              "PATH","",0.,  40,  6, 10 ),
    ControllerData( 9, 3,1, 3,"XXXX",                        "/path_replay_trackNo_3_can 1","/path_replay_trackNo_3",       1.f, "3",          "3",              "PATH","",0.,  40,  6, 10 ),
    ControllerData( 9, 4,1, 4,"XXXX",                        "/path_replay_trackNo_4_can 1","/path_replay_trackNo_4",       1.f, "4",          "4",              "PATH","",0.,  40,  6, 10 ),
    ControllerData( 9, 5,1, 5,"XXXX",                        "/path_replay_trackNo_5_can 1","/path_replay_trackNo_5",       1.f, "5",          "5",              "PATH","",0.,  40,  6, 10 ),
    ControllerData( 9, 6,1, 6,"XXXX",                        "/path_replay_trackNo_6_can 1","/path_replay_trackNo_6",       1.f, "6",          "6",              "PATH","",0.,  40,  6, 10 ),
    ControllerData( 9, 7,1, 7,"XXXX",                        "/path_replay_trackNo_7_can 1","/path_replay_trackNo_7",       1.f, "7",          "7",              "PATH","",0.,  40,  6, 10 )},
  { ControllerData( 9,64,1,64,"XXXX",                        "XXXX",                        "/setup_plus",                  1.f, "+",          "SetUp +",         "GUI","",0.,  40,  6, 18 ),
    ControllerData( 9,65,1,65,"XXXX",                        "XXXX",                        "/resend_all",                  1.f, "Rsnd",       "Resend all",      "GUI","",0.,  40,  6, 18 ),
    ControllerData( 9,66,1,66,"XXXX",                        "XXXX",                        "/setup_minus",                 1.f, "-",          "SetUp -",         "GUI","",0.,  40,  6, 18 )},
  { ControllerData( 9,67,1,67,"XXXX",                        "XXXX",                        "/clearAllLayers",              1.f, "All",        "Clear All",       "GUI","",0.,  40,  6, 10 ),
    ControllerData( 9,68,1,68,"XXXX",                        "XXXX",                        "/clearCA",                     1.f, "CA",         "Clear CA",        "GUI","",0.,  40,  6, 10 ),
#ifndef GN
    ControllerData( 9,69,1,69,"XXXX",                        "XXXX",                        "XXXX",                         1.f,  "X",         "XXXX",          "EMPTY","",0.,  40,  6, 10 ),
    ControllerData( 9,70,1,70,"XXXX",                        "XXXX",                        "XXXX",                         1.f,  "X",         "XXXX",          "EMPTY","",0.,  40,  6, 10 ),
    ControllerData( 9,71,1,71,"XXXX",                        "XXXX",                        "XXXX",                         1.f,  "X",         "XXXX",          "EMPTY","",0.,  40,  6, 10 )},
#else
    ControllerData( 9,69,1,69,"XXXX",                        "XXXX",                        "/BGSubtr",                     1.f, "BG",         "BG Subtr",        "GUI","",0.,  40,  6, 10 ),
    ControllerData( 9,70,1,70,"XXXX",                        "XXXX",                        "/initCA",                      1.f, "inCA",       "init CA",         "GUI","",0.,  40,  6, 10 ),
    ControllerData( 9,71,1,71,"XXXX",                        "XXXX",                        "/initialBGCapture",            1.f, "inBG",       "iniBG Capt",      "GUI","",0.,  40,  6, 10 )},
#endif
  { ControllerData( 9,82,1,82,"XXXX",                        "/PD_connected 1",             "/connect_PD",                  1.f, "PD",         "Connect PD",      "GUI","",0.,  40,  6, 10 ),
    ControllerData( 9,83,1,83,"XXXX",                        "XXXX",                        "XXXX",                         1.f, "Config",     "CONFIG",          "GUI","",0.,  40,  6, 10 ),
    ControllerData( 9,84,1,84,"XXXX",                        "XXXX",                        "/currentBGCapture",            1.f, "RstIm",      "Reset Img",       "GUI","",0.,  40,  6, 10 ),
    ControllerData( 9,85,1,85,"XXXX",                        "XXXX",                        "/reset_sound",                 1.f, "RsrSnd",     "Reset Snd",       "GUI","",0.,  40,  6, 10 ),
    ControllerData( 9,86,1,86,"XXXX",                        "/snapshot_onOff_can 1",       "/snapshot_onOff",              1.f, "Snap",       "Snap On/Off",     "GUI","",0.,  40,  6, 10 ),
    ControllerData( 9,87,1,87,"XXXX",                        "/mute_screen_can 1",          "/mute_screen",                 1.f, "Mute",       "Mute Screen",     "GUI","",0.,  40,  6, 10 ),
    ControllerData( 9,88,1,88,"XXXX",                        "/auto_beat_can 1",            "/auto_beat",                   1.f, "AutoBt",     "AutoBeat",        "GUI","",0.,  40,  6, 10 ),
    ControllerData( 9,89,1,89,"XXXX",                        "/launch_can 1",               "/launch",                      1.f, "Launch",     "Launch",          "GUI","",0.,  40,  6, 10 )}};
std::vector<QString> ButtonsPathsData_ButtonTitles = {"PART MOVE", "PART STROKE","PART EXIT", "PART COLOR",
                                                      "PART INIT","PART PATH REPOP","PART FOLLOW PATH","PART REPULSE PATH","PATH RECORD","PATH REPLAY",
                                                      "SET-UP", "CLEAR",""};
std::vector<QString> ButtonsPathsData_LabelTitles = {"PART MOVE", "PART STROKE","PART EXIT", "PART COLOR",
                                                      "PART INIT","PART PATH REPOP","PART FOLLOW PATH","PART REPULSE PATH","PATH RECORD","PATH REPLAY",
                                                      "SET-UP", "CLEAR","CONTROLS"};

/////////////////////////////////////////////////////////////////////////
/// PERFORMANCE TAB
/////////////////////////////////////////////////////////////////////////
// FADERS
#if defined(TVW) || defined(demo)
std::vector<std::vector<ControllerData>> SlidersMinimalIFData =
{
#ifdef TVW
  { ControllerData(11,13,1, 0,"/photo_contrast_can",         "XXXX",                        "/photo_contrast",              2.f, "Cont",        "PHOT Contr",   "MEDIA","", 1.,  10,  7, 10 ),
    ControllerData(11,13,2, 0,"/mask_contrast_can",          "XXXX",                        "/mask_contrast",               2.f, "Cont",        "MASK Contr",   "MEDIA","", 1.,  10,  7, 10 )},
  { ControllerData(11,13,3, 0,"XXXX",                        "XXXX",                        "XXXX",                         1.f,  "X",         "XXXX",          "EMPTY","",0.,  10,  6, 10 ),
    ControllerData(11,13,4, 0,"XXXX",                        "XXXX",                        "XXXX",                         1.f,  "X",         "XXXX",          "EMPTY","",0.,  10,  6, 10 ),
    ControllerData(11,13,5, 0,"XXXX",                        "XXXX",                        "XXXX",                         1.f,  "X",         "XXXX",          "EMPTY","",0.,  10,  6, 10 ),
    ControllerData(11,13,6, 0,"XXXX",                        "XXXX",                        "XXXX",                         1.f,  "X",         "XXXX",          "EMPTY","",0.,  10,  6, 10 ),
#endif
#ifdef demo
  { ControllerData(11,13,1, 0,"/echoNeg_can",                "XXXX",                        "/echoNeg",                     1.f,  "-",         "ECHO Neg",       "ECHO","",0.,  10, 11, 10 ),
    ControllerData(11,13,2, 0,"/echo_can",                   "XXXX",                        "/echo",                        1.f,  "+",         "ECHO Pos",       "ECHO","",0.,  10,  7, 10 )},
  { ControllerData(11,13,3, 0,"/CAMixingWeight_can",         "XXXX",                        "/CAMixingWeight",              1.f, "CA",         "CA Mixing",     "TKWGH","",0.,  10, 11, 10 ),
    ControllerData(11,13,4, 0,"/PartMixingWeight_can",       "XXXX",                        "/PartMixingWeight",            1.f, "Pa",         "PART Mixing",   "TKWGH","",0.,  10, 22, 10 ),
    ControllerData(11,13,5, 0,"/trackMixingWeight_0_can",    "/currentDrawingTrack_can 0",  "/trackMixingWeight_0",         1.f, "BG",         "BG Mixing",     "TKWGH","",0.,  10, 11, 10 ),
    ControllerData(11,13,6, 0,"/trackMixingWeight_1_can",    "/currentDrawingTrack_can 1",  "/trackMixingWeight_1",         1.f, "T1",         "Tr1 Mixing",    "TKWGH","",0.,  10, 11, 10 ),
#endif
    ControllerData(11,13,7, 0,"XXXX",                        "XXXX",                        "XXXX",                         1.f,  "X",         "XXXX",          "EMPTY","",0.,  10,  6, 10 ),
    ControllerData(11,13,8, 0,"XXXX",                        "XXXX",                        "XXXX",                         1.f,  "X",         "XXXX",          "EMPTY","",0.,  10,  6, 10 )},
#ifdef TVW
  { ControllerData(11,12,1, 0,"XXXX",                        "XXXX",                        "XXXX",                         1.f,  "X",         "XXXX",          "EMPTY","",0.,  10,  6, 10 ),
    ControllerData(11,12,2, 0,"XXXX",                        "XXXX",                        "XXXX",                         1.f,  "X",         "XXXX",          "EMPTY","",0.,  10,  6, 10 )},
  { ControllerData(11,12,2, 0,"XXXX",                        "XXXX",                        "XXXX",                         1.f,  "X",         "XXXX",          "EMPTY","",0.,  10,  6, 10 ),
    ControllerData(11,12,3, 0,"XXXX",                        "XXXX",                        "XXXX",                         1.f,  "X",         "XXXX",          "EMPTY","",0.,  10,  6, 10 ),
    ControllerData(11,12,4, 0,"XXXX",                        "XXXX",                        "XXXX",                         1.f,  "X",         "XXXX",          "EMPTY","",0.,  10,  6, 10 ),
    ControllerData(11,12,5, 0,"XXXX",                        "XXXX",                        "XXXX",                         1.f,  "X",         "XXXX",          "EMPTY","",0.,  10,  6, 10 ),
#endif
#ifdef demo
  { ControllerData(11,12,1, 0,"/pen_color_can",              "XXXX",                        "/pen_color",                   1.f, "Col",        "PEN color",       "PEN","", 1.,  10, 12, 10 ),
    ControllerData(11,12,2, 0,"/repop_color_can",            "XXXX",                        "/repop_color",                 1.f, "Repo",       "REPO color",    "REPOP","",1.,  10, 12, 10 )},
  { ControllerData(11,12,3, 0,"/CAMasterWeight_can",         "XXXX",                        "/CAMasterWeight",              1.f, "CA",         "CA Master",     "TKWGH","",0.,  10, 11, 10 ),
    ControllerData(11,12,4, 0,"/PartMasterWeight_can",       "XXXX",                        "/PartMasterWeight",            1.f, "Pa",         "PART Master",   "TKWGH","",0.,  10, 22, 10 ),
    ControllerData(11,12,5, 0,"/trackMasterWeight_0_can",    "/currentDrawingTrack_can 0",  "/trackMasterWeight_0",         1.f, "BG",         "BG Master",     "TKWGH","",0.,  10, 11, 10 ),
    ControllerData(11,12,6, 0,"/trackMasterWeight_1_can",    "/currentDrawingTrack_can 1",  "/trackMasterWeight_1",         1.f, "T1",         "Tr1 Master",    "TKWGH","",0.,  10, 11, 10 ),
#endif
    ControllerData(11,12,7, 0,"XXXX",                        "XXXX",                        "XXXX",                         1.f,  "X",         "XXXX",          "EMPTY","",0.,  10,  6, 10 ),
    ControllerData(11,12,8, 0,"XXXX",                        "XXXX",                        "XXXX",                         1.f,  "X",         "XXXX",          "EMPTY","",0.,  10,  6, 10 )},

  { ControllerData( 0, 0,0, 0,"/pulse_low",                  "XXXX",                        "XXXX",                         1.f, "L",          "",              "SOUND","",0.,   6,  2,  7 ),
    ControllerData( 0, 0,0, 0,"/pulse_medium",               "XXXX",                        "XXXX",                         1.f, "M",          "",              "SOUND","",0.,   6,  2,  7 ),
    ControllerData( 0, 0,0, 0,"/pulse_high",                 "XXXX",                        "XXXX",                         1.f, "H",          "",              "SOUND","",0.,   6,  7,  7 ),
    ControllerData( 0, 0,0, 0,"/pulse_enveloppe",            "XXXX",                        "XXXX",                         1.f, "E",          "",              "SOUND","",0.,   6,  0,  7 )},

  { ControllerData(11,10,1, 0,"/pixel_acc_factor_can",       "XXXX",                        "/pixel_acc_factor",           .05f, "Acc",        "PIX acc",       "PIXEL","",0.,  10, 18, 10 ),
#ifdef TVW
    ControllerData(11,10,2, 0,"XXXX",                        "XXXX",                        "XXXX",                         1.f,  "X",         "XXXX",          "EMPTY","",0.,  10,  6, 10 ),
    ControllerData(11,10,3, 0,"XXXX",                        "XXXX",                        "XXXX",                         1.f,  "X",         "XXXX",          "EMPTY","",0.,  10,  6, 10 )},
  { ControllerData(11,10,4, 0,"XXXX",                        "XXXX",                        "XXXX",                         1.f,  "X",         "XXXX",          "EMPTY","",0.,  10,  6, 10 ),
    ControllerData(11,10,5, 0,"XXXX",                        "XXXX",                        "XXXX",                         1.f,  "X",         "XXXX",          "EMPTY","",0.,  10,  6, 10 ),
    ControllerData(11,10,6, 0,"XXXX",                        "XXXX",                        "XXXX",                         1.f,  "X",         "XXXX",          "EMPTY","",0.,  10,  6, 10 ),
#endif
#ifdef demo
    ControllerData(11,10,2, 0,"/noiseScale_can",             "XXXX",                        "/noiseScale",                  1.f,  "Sca",       "Noise Scal",     "PART","",0.,  10, 11, 10 ),
    ControllerData(11,10,3, 0,"/pixel_radius_can",           "XXXX",                        "/pixel_radius",                1.f, "Rad",        "PIXEL radius",  "PIXEL","",0.,  10, 11, 10 )},
  { ControllerData(11,10,4, 0,"/flashTrkCA_freq_1_can",      "/currentDrawingTrack_can 1",  "/flashTrkCA_freq_1",           9.f, "T1>CA",      "Tr1->CA",       "FLASH","",0.,  10, 11, 10 ),
    ControllerData(11,10,5, 0,"/flashTrkBG_freq_1_can",      "/currentDrawingTrack_can 1",  "/flashTrkBG_freq_1",           9.f, "T1>BG",      "Tr1->BG",       "FLASH","",0.,  10, 11, 10 ),
    ControllerData(11,10,6, 0,"/flashCABG_freq_can",         "XXXX",                        "/flashCABG_freq",              9.f, "CA>BG",      "CA->BG",        "FLASH","",0.,  10, 11, 10 ),
#endif
    ControllerData(11,10,7, 0,"XXXX",                        "XXXX",                        "XXXX",                         1.f,  "X",         "XXXX",          "EMPTY","",0.,  10, 11, 10 ),
    ControllerData(11,10,8, 0,"XXXX",                        "XXXX",                        "XXXX",                         1.f,  "X",         "XXXX",          "EMPTY","",0.,  10, 11, 10 )},

  { ControllerData(11, 7,1, 0,"/pen_radius_can",             "XXXX",                        "/pen_radius",                 100.f,  "Ra",        "PEN radius",      "PEN","", 10.,  10, 12, 10 )},
#ifdef TVW
  { ControllerData(11, 7,2, 0,"/trkDecay_0_can",             "/trkDecay_sign_0_can -1",     "/trkDecay_0",                0.01f,  "BG",        "BG decay",      "TRACK","",0.,  10, 11, 10 ),
    ControllerData(11, 7,3, 0,"/trkDecay_1_can",             "/trkDecay_sign_1_can -1",     "/trkDecay_1",                0.01f,  "T1",        "Tr1 decay",     "TRACK","",0.,  10, 11, 10 )},
  { ControllerData(11, 7,4, 0,"/cameraThreshold_can",        "XXXX",                        "/cameraThreshold",             1.f,"Thres",       "Thresh",        "MEDIA","",0.,  10, 11, 10 ),
    ControllerData(11, 7,5, 0,"/photo_value_can",            "XXXX",                        "/photo_value",                 2.f, "Val",        "PHOT Value",    "MEDIA","", 1.,  10,  7, 10 )},
#endif
#ifdef demo
  { ControllerData(11, 7,2, 0,"/CAdecay_can",                "/CAdecay_sign_can -1",        "/CAdecay",                     .2f,  "CA",        "CA decay",      "TRACK","", 1.,  10, 11, 10 ),
    ControllerData(11, 7,3, 0,"/trkDecay_0_can",             "/trkDecay_sign_0_can -1",     "/trkDecay_0",                0.04f,  "BG",        "BG decay",     "TRACK","", 0.,  10, 11, 10 ),
    ControllerData(11, 7,4, 0,"/trkDecay_1_can",             "/trkDecay_sign_1_can -1",     "/trkDecay_1",                0.04f,  "T1",        "Tr1 decay",     "TRACK","", 0.,  10, 11, 10 )},
  { ControllerData(11, 7,5, 0,"/photo_value_can",            "XXXX",                        "/photo_value",                 2.f, "Val",        "PHOT Value",    "MEDIA","", 1.,  10,  7, 10 )},
#endif
  { ControllerData(11, 7,6, 0,"/sound_env_min_can",          "XXXX",                        "/sound_env_min",             100.f, "Min",        "Thres Min",     "SOUND","",0.,  10,  8, 10 ),
    ControllerData(11, 7,7, 0,"/sound_env_max_can",          "XXXX",                        "/sound_env_max",             100.f, "Max",        "Thres Max",     "SOUND","",0.,  10, 18, 10 ),
    ControllerData(11, 7,8, 0,"/sound_volume_can",           "XXXX",                        "/sound_volume",                2.f,  "Vol",        "Volume",       "SOUND","",0.,  10, 11, 10 )},

  { ControllerData(11,14,8, 0,"/blendTransp_can",            "XXXX",                        "/blendTransp",                 1.f, "Blk",        "MASTER",        "TKWGH","",0.,  10, 11, 10 )}
};
#ifdef TVW
std::vector<QString> SlidersMinimalIFData_verticalSlidersRowTitles = {"XXXX","XXXX","PIXELS","XXXX","BANDS","PHOTO/MASK","XXXX","PEN", "DECAY", "IMG", "SOUND","MAS"};
std::vector<QString> SlidersMinimalIFData_LEDBarRowTitles = {"XXXX","XXXX","PIXELS","XXXX","BANDS","PHOTO/MASK","XXXX","PEN", "DECAY", "IMG", "SOUND","MAS"};
#endif
#ifdef demo
std::vector<QString> SlidersMinimalIFData_verticalSlidersRowTitles = {"ECHO","MIXING","PEN","MASTER","BANDS","PIXEL","FLASH","PEN", "DECAY", "IMG", "SOUND","MAS"};
std::vector<QString> SlidersMinimalIFData_LEDBarRowTitles = {"ECHO","MIXING","PEN","MASTER","BANDS","PIXEL","FLASH","PEN", "DECAY", "IMG", "SOUND","MAS"};
#endif

// BUTTONS
//// 1st & 2nd row MinimalIF buttons
std::vector<std::vector<ControllerData>> ButtonsMinimalIFData =
{
#ifdef TVW
  { ControllerData(11, 9,1, 1,"XXXX",                        "/pen_colorPreset_can 0",      "/pen_colorPreset",             0.f, "1",          "1",               "PEN","",0.,  40,  6, 12 ),
    ControllerData(11, 9,2, 4,"XXXX",                        "/pen_colorPreset_can 1",      "/pen_colorPreset",             1.f, "2",          "2",               "PEN","",0.,  40,  6, 10 ),
    ControllerData(11, 9,3, 7,"XXXX",                        "/pen_colorPreset_can 2",      "/pen_colorPreset",             2.f, "3",          "3",               "PEN","",0.,  40,  6, 10 )},
  { ControllerData(11, 9,4,10,"XXXX",                        "/pen_colorPreset_can 3",      "/pen_colorPreset",             3.f, "4",          "4",               "PEN","",0.,  40,  6, 10 ),
    ControllerData(11, 9,5,13,"XXXX",                        "/pen_colorPreset_can 4",      "/pen_colorPreset",             4.f, "5",          "5",               "PEN","",0.,  40,  6, 10 ),
    ControllerData(11, 9,6,16,"XXXX",                        "/pen_colorPreset_can 5",      "/pen_colorPreset",             5.f, "6",          "6",               "PEN","",0.,  40,  6, 10 ),
    ControllerData(11, 9,7,19,"XXXX",                        "/pen_colorPreset_can 6",      "/pen_colorPreset",             6.f, "7",          "7",               "PEN","",0.,  40,  6, 10 )},
  { ControllerData(11, 9,8,22,"XXXX",                        "XXXX",                        "XXXX",                         1.f,  "X",         "XXXX",          "EMPTY","",0.,  40,  6, 10 )},
  { ControllerData(11, 8,1, 3,"XXXX",                        "/pen_grey_can 1",             "/pen_BW",                      1.f,  "?",          "B/W",            "PEN","",0.,  40,  6, 12 )},
  { ControllerData(11, 8,2, 6,"XXXX",                        "/trkDecay_sign_0_can -1",     "/trkDecay_sign_0",             1.f, "BG",         "BG +/-",        "TRACK","",0.,  10, 11, 10 ),
    ControllerData(11, 8,3, 9,"XXXX",                        "/trkDecay_sign_1_can -1",     "/trkDecay_sign_1",             1.f, "T1",         "TK1 +/-",       "TRACK","",0.,  10, 11, 10 )},
  { ControllerData(11, 8,4,12,"XXXX",                        "XXXX",                        "XXXX",                         1.f,  "X",         "XXXX",          "EMPTY","",0.,  40,  6, 10 ),
    ControllerData(11, 8,5,15,"XXXX",                        "XXXX",                        "XXXX",                         1.f,  "X",         "XXXX",          "EMPTY","",0.,  40,  6, 10 ),
    ControllerData(11, 8,6,18,"XXXX",                        "XXXX",                        "XXXX",                         1.f,  "X",         "XXXX",          "EMPTY","",0.,  40,  6, 10 ),
    ControllerData(11, 8,7,21,"XXXX",                        "XXXX",                        "XXXX",                         1.f,  "X",         "XXXX",          "EMPTY","",0.,  40,  6, 10 ),
    ControllerData(11, 8,8,24,"XXXX",                        "XXXX",                        "/flashCamera",                 1.f, "Ca",         "Flash",         "MEDIA","",0.,  40,  6, 10 )},
#endif
#ifdef demo
  { ControllerData(11, 9,1, 1,"XXXX",                        "/pen_colorPreset_can 0",      "/pen_colorPreset",             0.f, "1",          "1",               "PEN","",0.,  40,  6, 12 ),
    ControllerData(11, 9,2, 4,"XXXX",                        "/pen_colorPreset_can 1",      "/pen_colorPreset",             1.f, "2",          "2",               "PEN","",0.,  40,  6, 10 ),
    ControllerData(11, 9,3, 7,"XXXX",                        "/pen_colorPreset_can 2",      "/pen_colorPreset",             2.f, "3",          "3",               "PEN","",0.,  40,  6, 10 )},
  { ControllerData(11, 9,4,10,"XXXX",                        "/pixel_mode_can 1",           "/pixel_mode_plus",             1.f, "?",          "PIXEL Mode",    "PIXEL","",0.,  40,  6, 10 ),
    ControllerData(11, 9,5,13,"XXXX",                        "/CASubType_can 1",            "/CAonOff",                     1.f, "ST",         "CA On/Off",      "PART","",0.,  40,  6, 10 ),
    ControllerData(11, 9,6,16,"XXXX",                        "XXXX",                        "/CA1Type_plus",                1.f, "+",          "CA +",           "PART","",0.,  40,  6, 10 )},
  { ControllerData(11, 9,7,19,"XXXX",                        "/soundtrack_onOff_can 1",     "/soundtrack_plus",             1.f, "+",          "Next",          "SOUND","",0.,  40,  6, 10 ),
    ControllerData(11, 9,8,22,"XXXX",                        "/soundtrack_onOff_can 1",     "/soundtrack_onOff",            1.f, "X",          "On/Off",        "SOUND","",0.,  40,  6, 10 )},
  { ControllerData(11, 8,1, 3,"XXXX",                        "/pen_grey_can 1",             "/pen_BW",                      1.f,  "?",          "B/W",            "PEN","",0.,  40,  6, 12 )},
  { ControllerData(11, 8,2, 6,"XXXX",                        "/CAdecay_sign_can -1",        "/CAdecay_sign",                1.f, "CA",         "CA +/-",        "TRACK","",0.,  40,  6, 10 ),
    ControllerData(11, 8,3, 9,"XXXX",                        "/trkDecay_sign_0_can -1",     "/trkDecay_sign_0",             1.f, "BG",         "BG +/-",        "TRACK","",0.,  10, 11, 10 ),
    ControllerData(11, 8,4,12,"XXXX",                        "/trkDecay_sign_1_can -1",     "/trkDecay_sign_1",             1.f, "T1",         "TK1 +/-",       "TRACK","",0.,  10, 11, 10 )},
  { ControllerData(11, 8,5,15,"XXXX",                        "/currentDrawingTrack_can 0",  "/currentDrawingTrack",         0.f, "BG",         "Track BG",      "TRACK","",0.,  40,  6, 10 ),
    ControllerData(11, 8,6,18,"XXXX",                        "/currentDrawingTrack_can 1",  "/currentDrawingTrack",         1.f, "T1",         "Track 1",       "TRACK","",0.,  40,  6, 10 ),
    ControllerData(11, 8,7,21,"XXXX",                        "/pen_brush_can 0",            "/pen_brush",                   0.f, "Ci",         "Circle",          "PEN","",0.,  40,  6, 12 ),
    ControllerData(11, 8,8,24,"XXXX",                        "XXXX",                        "/pen_brush_plus",              1.f,  "+",         "Brush +",         "PEN","",0.,  40,  6, 10 )},
#endif
  { ControllerData( 9,25,1,25,"XXXX",                        "XXXX",                        "XXXX",                         0.f, "PULSE",      "PULSE",     "GUI","Ctrl+p",1.,   40,  6, 10 ),
    ControllerData( 9,26,1,26,"XXXX",                        "XXXX",                        "XXXX",                         0.f, "TAB",        "TAB",       "GUI","Ctrl+t",1.,   40,  6, 10 ),
    ControllerData( 9,27,1,27,"XXXX",                        "XXXX",                        "XXXX",                         0.f, "PATHS",      "PATHS",     "GUI","Ctrl+p",1.,   40,  6, 10 )}
};
#ifdef TVW
std::vector<QString> ButtonsMinimalIFData_ButtonTitles = {"COLOR", "PIXEL", "XXXX", "PEN", "DECAY", "FLASH", ""};
std::vector<QString> ButtonsMinimalIFData_LabelTitles = {"COLOR", "PIXEL", "XXXX", "PEN", "DECAY", "FLASH", "SIDE"};
#endif
#ifdef demo
std::vector<QString> ButtonsMinimalIFData_ButtonTitles = {"COLOR", "PIXEL/CA", "SAMPLE", "PEN", "DECAY", "TRACK/BRUSH", ""};
std::vector<QString> ButtonsMinimalIFData_LabelTitles = {"COLOR", "PIXEL/CA", "SAMPLE", "PEN", "DECAY", "TRACK/BRUSH", "SIDE"};
#endif
#endif

std::vector<QString> TabbedWindowTitles = {"CONFIG" , "BEAT", "++BEAT", "MIXER", "++MIXER", "FX", "++FX", "PATHS", "++PATHS"
#if defined(TVW) || defined(demo)
                                           , "MINI" , "++MINI"
#endif
                                          };


/////////////////////////////////////////////////////////////
/// \brief ControllerData::ControllerData
/// \param typeIn_par
/// \param controlsIn_par
/// \param channelsIn_par
/// \param labelsUDPIn_par
/// \param labelsHighlight_par
/// \param amplitude_par
/// \param title_par
/// \param labelobject_names_par
/// \param size_par
/// \param intervalle_par
/// \param fontSize_par
///
ControllerData::ControllerData(
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
        float pulseControl_par, // whether or not the parameter can be pulsed
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
    bgOffColor.setRgbF(.7f, .7f, 1.f);
    bgOnColor.setRgbF(1.f, .7f, .7f);
    barNormalColor.setRgbF(.2f, .2f, 1.f);
    barOverflowColor.setRgbF(1.f, .2f, .2f);
    barUnderflowColor.setRgbF(1.f, 1.f, .2f);
    size = size_par;
    intervalle = intervalle_par;
    fontSize = fontSize_par;
    pulseControlFactor = pulseControl_par;
 }
ControllerData::~ControllerData(void) {
}
QString ControllerData::get_labelUDPIn() {
    return labelUDPIn;
}
QString  ControllerData::get_labelUDPOut() {
    return labelUDPOut;
}
QString ControllerData::get_labelHighlightIn() {
    return labelHighlightIn;
}
// MIDI in
int  ControllerData::get_typeMidiIn() {
    return typeMidiIn;
}
int  ControllerData::get_controlMidiIn() {
    return controlMidiIn;
}
int  ControllerData::get_channelMidiIn() {
    return channelMidiIn;
}
int  ControllerData::get_controlMidiOut() {
    return controlMidiOut;
}
float  ControllerData::get_amplitude() {
    return amplitude;
}
int  ControllerData::get_size() {
    return size;
}
// GUI names: displayed name - name used for styling
QString  ControllerData::get_titleDisplay() {
    return titleDisplay;
}
QString  ControllerData::get_titleController() {
    return titleController;
}
QString  ControllerData::get_labelObject() {
    return labelObject;
}
// control Key (for buttons)
QString  ControllerData::get_contolKey() {
    return contolKey;
}
float ControllerData::getPulseControlFactor(void) {
    return pulseControlFactor;
}


