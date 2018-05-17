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
#ifndef INTERFACESONGDISPLAY_H
#define INTERFACESONGDISPLAY_H

#include <vector>

#include <QGroupBox>
#include <QRect>
#include <QLabel>
#include <QColor>

#include "pg-interface-song-data.h"

///////////////////////////////////////////////////////////////
/// \brief The ColorDisplay class
///
class ColorDisplay : public QGroupBox
{
    Q_OBJECT

public:
    ColorDisplay(std::vector<QString> givenTitles,
                          int nbOfPalettes,
                          int givenSize,
                           QWidget *parent = 0);
    void setMixedColor(float r, float g, float b, int index);
    void setGray(float g, int index);
    void setPalette(float r1, float g1, float b1,
                    float r2, float g2, float b2,
                    float r3, float g3, float b3, int index);
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
private:
    int nbOfPalettes;
    QString *title;
    QRect *rectLabel;
    QRect *rectMixedColor;
    QRect *rectGray;
    QRect **rectPalette;
    QColor *colorMixedColor;
    QColor *colorGray;
    QColor **colorPalette;
};

///////////////////////////////////////////////////////////////
/// \brief The TimeSceneBeatDisplay class
///
class TimeSceneBeatDisplay : public QGroupBox
{
    Q_OBJECT

public:
    explicit TimeSceneBeatDisplay(QString givenTitle,
                                  std::vector<QColor> givencolorTime,
                                  int givenTimeSize,
                                  int givenSceneNameSize,
                                  int givenBeatSize,
                                  int givenHeight,
                                  QWidget *parent = 0);
    void setTime(QString giventime);
    void setColor(int givencolorIndex);
    void setSceneName(QString givensceneName);
    void setMessageText(QString givenMessageText);
    void setBeatNo(QString givenBeatNo);
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
private:
    QString title;
    QString time;
    QString sceneName;
    QString messageText;
    QString beatNo;
    QRect rectTime;
    QRect rectSceneName;
    QRect rectMessageText;
    QRect rectBeatNo;
    QString windowTitle;
    std::vector<QColor> colorTime;
    int colorIndex;
};

///////////////////////////////////////////////////////////////
/// \brief The SingleLabel class
///
class SingleLabel : public QLabel
{
    Q_OBJECT

public:
    SingleLabel(ControllerData *givenData,
                int indLabelRow, int indLabelCol, DisplayType displayType,
                bool is_pulse,
                QWidget *parent = 0);
    void setHighlight(float givenValue);
    bool getHighlight(void);
    // void setLabelUDPIn(QString givenLabel, bool is_pulse);
    void setLabelHighlightIn(QString givenLabel);
    void setLabelText(QString givenLabel);
    void setMidiIn(int givenLabel);
    QString getLabelUDPIn(void);
    QString getLabelHighlightIn(void);
    QString getLabelText(void);
    QString getlabelUDPOut(void);
    int getMidiIn(void);
    bool getIsActive(void);
    void setIsActive(bool is_active);
    void setLabelHighlightIn(QString givenLabel,
                             int indSliderRow, int indSliderCol, DisplayType displayType);

public slots:
    void inverse(void);
    void normal(void);

private:
    QString labelUDPIn;
    QString labelHighlightIn;
    QString labelText;
    float valueHighlightIn;
    bool hasValueHighlightIn;
    int midiInControl;
    bool highlight;
    bool isActive;
};

///////////////////////////////////////////////////////////////
/// \brief The LEDBarRow class
///
class LabelRow : public QGroupBox
{
    Q_OBJECT

public:
    LabelRow(QString givenTitle,
             int givenHeight,
             std::vector<ControllerData> &tab_inControllers,
             int indLabelRow, DisplayType displayType,
             bool is_pulse,
             QWidget *parent = 0);
    void setValues(std::vector<int> newValues);
    void setValue(unsigned int index, int value);
    void setHighlight(unsigned int index, bool onOff);
    SingleLabel *getSingleLabel(unsigned int indLabel);
    int getSize(void);

private:
    QString title;
    unsigned int nbLabels;
    std::vector<SingleLabel*> Labels;
    std::vector<int> values;
    // void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
};


class LEDBarRow;

class BoxGridDisplay;

///////////////////////////////////////////////////////////////
/// \brief The Box class
///
class Box  : public QObject
{
    Q_OBJECT

public:
    Box(QObject *parent = 0);
    void init(void);
    void setCoords(int t, int l, int b, int r);
    QRect *getRect(void);
    void setValue(int newValue);
    int getValue(void);
    void setHighlight(float givenValue);
    void setLabelHighlightIn(QString givenLabel,
                             int indBoxRow, int indBoxCol, DisplayType displayType);
    void setLabelUDPIn(QString givenLabel, bool is_pulse,
                       int indBoxRow, int indBoxCol, DisplayType displayType);
    void setValueHighlightIn(int givenValue);
    void setHasValueHighlightIn(bool givenValue);
    void setInverse(bool onOff);
    bool getHighlight(void);
    QString getLabelHighlightIn(void);
    float getValueHighlightIn(void);
    float getHasValueHighlightIn(void);
    bool getInverse(void);
    BoxGridDisplay * getParentGrid(void);
    void setParentGrid(BoxGridDisplay *givenParentGrid);
    bool getIsActive(void);
    void setIsActive(bool is_active);

public slots:
    void inverse(void);
    void normal(void);

private:
    bool inversed;
    int value;
    QRect rectBox;
    bool highlight;
    QString labelHighlightIn;
    QString labelUDPIn;
    float valueHighlightIn;
    bool hasValueHighlightIn;
    BoxGridDisplay *parentGrid;
    bool isActive;
};

///////////////////////////////////////////////////////////////
/// \brief The BoxGridDisplay class
///
class BoxGridDisplay : public QGroupBox
{
    Q_OBJECT

public:
    explicit BoxGridDisplay(QString givenTitle,
                            int givenNbRows, int givenNbCols,
                            std::vector<std::vector<ControllerData>> &tab_inControllers,
                            bool is_pulse,
                            QWidget *parent = 0);
    void setValue(int indRow, int indCol, int value);
    void setHighlight(int indRow, int indCol, float value);
    QString getLabelHighlightIn(int indRow, int indCol);
    float getValueHighlightIn(int indRow, int indCol);
    float getHasValueHighlightIn(int indRow, int indCol);
    Box *getBox(int indRow, int indCol);
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
private:
    QString title;
    int nbRows;
    int nbCols;
    int boxSize;
    std::vector<Box> boxes;
    QColor textColor;
};

///////////////////////////////////////////////////////////////
/// \brief The LEDBar class
///
class LEDBar  : public QObject
{
    Q_OBJECT

public:
    QColor bgOnColor;
    QColor bgOffColor;
    QColor bgColor;
    QColor barNormalColor;
    QColor barOverflowColor;
    QColor barUnderflowColor;
    QColor barColor;
    QColor borderColor;
    QColor textColor;
    int fontSize;
    QRect rectBg;
    QRect rectBar;
    QRect rectLabel;
    QString title;
    QString labelHighlightIn;
    QString labelUDPIn;
    float valueHighlightIn;
    bool hasValueHighlightIn;
    LEDBar(int givenXOffset,
           int givenYOffset,
           int givenWidth,
           int givenHeight,
           int givenIntervalToNext,
           int givenLabelHeight,
           LEDBarRow *givenParentLEDBarRow,
           int indLEDBarRow, int indLEDBarCol, DisplayType displayType,
           bool is_pulse,
           bool upperLabel,
           bool doubleLayerLabels,
           QString givenTitle,
           bool is_active,
           QString givenLabelHighlightIn,
           QString givenLabelUDPIn,
           float givenAmplitude,
           int givenFontSize,
           QColor *givenbgOffColor, QColor *givenbgOnColor,
           QColor *givenbarNormalColor, QColor *givenbarOverflowColor, QColor *givenbarUnderflowColor,
           float rBorder = 0.f, float gBorder = 0.f, float bBorder = 0.f,
           float rText = 1.f, float gText = 1.f, float bText = 1.f,
           QObject *parent = 0);
    void setValue(float newValue);
    void setHighlight(bool onOff);
    void setInverse(bool onOff);
    bool getHighlight(void);
    bool getInverse(void);
    LEDBarRow * getParentLEDBarRow(void);
    bool getIsActive(void);
    void setLabelUDPIn(QString givenLabel, bool is_pulse,
                       int indLEDBArRow, int indLEDBArCol, DisplayType displayType);
    void setLabelHighlightIn(QString givenLabel,
                             int indLEDBArRow, int indLEDBArCol, DisplayType displayType);


public slots:
    void inverse(void);
    void normal(void);

private:
    int XoffSet;
    int YoffSet;
    int height;
    float amplitude;
    bool highlight;
    bool inversed;
    LEDBarRow *parentLEDBarRow;
    bool isActive;
};

///////////////////////////////////////////////////////////////
/// \brief The LEDBarRow class
///
class LEDBarRow : public QGroupBox
{
    Q_OBJECT

public:
    LEDBarRow(QString givenTitle,
              int givenHeight,
              std::vector<ControllerData> &tab_inControllers,
              int indLEDBarRow, DisplayType displayType,
              bool is_pulse,
              bool doubleLayerLabels = false,
              float rBorder = 0.f, float gBorder = 0.f, float bBorder = 0.f,
              float rText = 1.f, float gText = 1.f, float bText = 1.f,
              QWidget *parent = 0);
    void setValues(std::vector<float> newValues);
    void setValue(unsigned int index, float value);
    void setHighlight(unsigned int index, bool onOff);
    LEDBar *getLEDBar(unsigned int indLEDBar);

private:
    QString title;
    unsigned int nbLEDBars;
    int xSpacing;
    std::vector<LEDBar*> LEDBars;
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
};

//////////////////////////////////////////////////////////////
///
/// \brief The GraphicalDisplays class
///
class GraphicalDisplays : public QWidget
{
    Q_OBJECT

public:
    explicit GraphicalDisplays(DisplayType givenType,
                               bool is_pulse,
                               QWidget *parent = 0);
    TimeSceneBeatDisplay *timeSceneBeatDisplay;
    ColorDisplay *colorPalette;

    std::vector<LEDBarRow*> graphicalLEDBarRows;
    std::vector<LabelRow*> graphicalLabelRows;

    BoxGridDisplay *graphsFlashGrid;

    void create_BeatTabDisplays(
            bool is_pulse
            );
    void create_MixerTabDisplays(
            bool is_pulse
            );
    void create_FXTabDisplays(
            bool is_pulse
            );
    void create_PathsTabDisplays(
            bool is_pulse
            );
#ifdef TVW
    void create_TVWTabDisplays(
            bool is_pulse
            );
#endif
private:
};

enum AssociatedDisplayType { _LEDBar = 0, _Box, _MaxAssociatedDisplayTypes };


#endif // INTERFACESONGDISPLAY_H
