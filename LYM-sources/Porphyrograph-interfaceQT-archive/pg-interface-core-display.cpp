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
#include <iostream>
#include <cstdlib>

#include "pg-interface-core-display.h"
#include "pg-interface-core-window.h"
#include "pg-interface-core-udp.h"

#include <QtWidgets>
#include <QBrush>

/////////////////////////////////////////////////////////////
/// \brief ColorDisplay::ColorDisplay
/// \param givenTitles
/// \param givennbOfPalettes
/// \param givenSize
/// \param parent
///
ColorDisplay::ColorDisplay(std::vector<QString> givenTitles,
                           int givennbOfPalettes,
                           int givenSize,
                          QWidget *parent)
    : QGroupBox("", parent) {
    int xpos = 0;
    int ypos = 20;
    nbOfPalettes = givennbOfPalettes;

    title = new QString[nbOfPalettes];
    rectLabel = new QRect[nbOfPalettes];
    rectMixedColor = new QRect[nbOfPalettes];
    rectGray = new QRect[nbOfPalettes];
    rectPalette = new QRect*[nbOfPalettes];
    for(int i = 0 ; i < nbOfPalettes ; i++ ) {
        rectPalette[i] = new QRect[3];
    }
    colorMixedColor = new QColor[nbOfPalettes];
    colorGray = new QColor[nbOfPalettes];
    colorPalette = new QColor*[nbOfPalettes];
    for(int i = 0 ; i < nbOfPalettes ; i++ ) {
        colorPalette[i] = new QColor[3];
    }

    for(int ind = 0 ; ind < nbOfPalettes; ind++) {
       title[ind] = givenTitles[ind];

       float labelSize = (ind == 0? 1.5f:2.f);
       rectLabel[ind].setCoords(xpos, ypos, xpos + givenSize * labelSize, ypos + givenSize);
       xpos += givenSize * labelSize + 3 ;
       rectMixedColor[ind].setCoords(xpos, ypos, xpos + givenSize, ypos + givenSize);
       colorMixedColor[ind].setHsl(180,255,255);
       xpos += givenSize;
       rectGray[ind].setCoords(xpos, ypos, xpos + givenSize, ypos + givenSize);
       colorGray[ind].setRgb(127, 127, 127);
       xpos += givenSize;
       for(int indRect = 0 ; indRect < 3 ; indRect++) {
           rectPalette[ind][indRect].setCoords(xpos, ypos, xpos + givenSize, ypos + givenSize);
           colorPalette[ind][indRect].setHsl(indRect * 60,255,255);
           xpos += givenSize;
       }
       if( ind != nbOfPalettes - 1) {
            xpos += givenSize * 1;
       }
    }

   // tries to show title
//   setTitle("COLOR");
//   setObjectName("COLOR");
   setTitle("");
   setObjectName("VOID");

   // dimensions
   int totalWidth = xpos;
   int totalHeight = 40;

   QVBoxLayout *ColorDisplayLayout = new QVBoxLayout;
   // Empty slider area
   QSpacerItem *emptyControl = new QSpacerItem(totalWidth , totalHeight);
   ColorDisplayLayout->addItem(emptyControl);
   setLayout(ColorDisplayLayout);
   resize(totalWidth , totalHeight);
   setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
}

void ColorDisplay::setMixedColor(float r, float g, float b, int index) {
   colorMixedColor[index].setRgbF(r, g, b);
   QWidget::update();
}

void ColorDisplay::setGray(float g, int index) {
   colorGray[index].setRgbF(g, g, g);
   QWidget::update();
}

void ColorDisplay::setPalette(float r1, float g1, float b1,
               float r2, float g2, float b2,
               float r3, float g3, float b3, int index) {
   colorPalette[index][0].setRgbF(r1, g1, b1);
   colorPalette[index][1].setRgbF(r2, g2, b2);
   colorPalette[index][2].setRgbF(r3, g3, b3);
   QWidget::update();
}
void ColorDisplay::paintEvent(QPaintEvent *event)
{
   QPainter painter;
   QGroupBox::paintEvent(event);
   painter.begin(this);
   for(int ind = 0 ; ind < nbOfPalettes; ind++) {
       painter.setPen(QPen(Qt::white));
       painter.setFont(QFont("Arial", 10, QFont::Bold));
       painter.drawText(rectLabel[ind],
                        Qt::AlignRight,
                        title[ind]);
       painter.fillRect(rectMixedColor[ind], QBrush(colorMixedColor[ind]));
       painter.fillRect(rectGray[ind], QBrush(colorGray[ind]));
       for(int indRect = 0 ; indRect < 3 ; indRect++) {
           painter.fillRect(rectPalette[ind][indRect], QBrush(colorPalette[ind][indRect]));
       }
   }
   QWidget::paintEvent(event);
   painter.end();
}

/////////////////////////////////////////////////////////////
/// \brief TimeSceneBeatDisplay::TimeSceneBeatDisplay
/// \param givenTitle
/// \param givencolorTime
/// \param givenTimeSize
/// \param givenSceneNameSize
/// \param givenHeight
/// \param parent
///
TimeSceneBeatDisplay::TimeSceneBeatDisplay(QString givenTitle,
                                           std::vector<QColor> givencolorTime,
                                           int givenTimeSize,
                                           int givenSceneNameSize,
                                           int givenBeatSize,
                                           int givenHeight,
                                           QWidget *parent)
    : QGroupBox("", parent) {
   title = givenTitle;
   colorTime = givencolorTime;
   colorIndex = 0;

   // spatial positioning
   int xpos = 0;
   int ypos = 0;
   rectTime.setCoords(xpos, ypos, xpos + givenTimeSize, ypos + givenHeight);
   xpos += givenTimeSize;
   rectSceneName.setCoords(xpos, ypos, xpos + givenSceneNameSize, ypos + int(.66 * givenHeight));
   rectMessageText.setCoords(xpos, ypos + int(.66 * givenHeight), xpos + givenSceneNameSize, ypos + givenHeight);
   xpos += givenSceneNameSize;
   rectBeatNo.setCoords(xpos, ypos, xpos + givenBeatSize, ypos + givenHeight);
   xpos += givenBeatSize;

   // tries to show title
   //setTitle(title);
   setTitle("");
   setObjectName("VOID");

   // dimensions
   int totalWidth = xpos;
   int totalHeight = givenHeight;

   QVBoxLayout *TimeSceneBeatDisplayLayout = new QVBoxLayout;
   // Empty slider area
   QSpacerItem *emptyControl = new QSpacerItem(totalWidth , totalHeight);
   TimeSceneBeatDisplayLayout->addItem(emptyControl);
   setLayout(TimeSceneBeatDisplayLayout);
   resize(totalWidth , totalHeight);
   setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
}

void TimeSceneBeatDisplay::setTime(QString giventime) {
   time = giventime;
   QWidget::update();
}

void TimeSceneBeatDisplay::setColor(int givencolorIndex) {
   colorIndex = givencolorIndex;
   QWidget::update();
}

void TimeSceneBeatDisplay::setSceneName(QString givensceneName) {
   sceneName = "[" + title + "]: " + givensceneName;
   QWidget::update();
}

void TimeSceneBeatDisplay::setMessageText(QString givenMessageText) {
   messageText = givenMessageText;
   QWidget::update();
}

void TimeSceneBeatDisplay::setBeatNo(QString givenBeatNo) {
    beatNo = givenBeatNo;
    QWidget::update();
}

void TimeSceneBeatDisplay::paintEvent(QPaintEvent *event)
{
   QPainter painter;
   QGroupBox::paintEvent(event);
   painter.begin(this);
   if(colorIndex < 4) {
       painter.fillRect(rectTime, QBrush(colorTime[colorIndex]));
   }
   else {
       painter.fillRect(rectTime, QBrush(colorTime[0]));
   }
   painter.fillRect(rectSceneName, QBrush(Qt::white));
   painter.fillRect(rectMessageText, QBrush(Qt::white));
   painter.setPen(Qt::black);
   painter.setFont(QFont("Arial", 24, QFont::Bold));
   painter.drawText(rectTime,
                    Qt::AlignCenter,
                    time);
   painter.setFont(QFont("Arial", 10, QFont::Bold));
   painter.drawText(rectSceneName.translated(5,0),
                    Qt::AlignLeft,
                    sceneName);
   painter.setFont(QFont("Arial", 10, QFont::Normal));
   painter.drawText(rectMessageText.translated(5,-2),
                    Qt::AlignLeft,
                    messageText);
   painter.fillRect(rectBeatNo, QBrush(Qt::white));
   painter.setFont(QFont("Arial", 24, QFont::Bold));
   painter.drawText(rectBeatNo,
                    Qt::AlignCenter,
                    beatNo);

   QWidget::paintEvent(event);
   painter.end();
}

/////////////////////////////////////////////////////////////
/// \brief SingleLabel::SingleLabel
/// \param indButton
/// \param tab_labelsControllers
/// \param parent
///
SingleLabel::SingleLabel( ControllerData *givenData,
                          int indLabelRow, int indLabelCol, DisplayType displayType,
                          bool is_pulse,
                          QWidget *parent)
    : QLabel("", parent) {
    labelHighlightIn = "";
    valueHighlightIn = 0.f;
    hasValueHighlightIn = false;
    labelText = "";
    midiInControl = 0;
    highlight = false;

    if((!is_pulse && givenData->get_labelObject() != QString("EMPTY"))|| (is_pulse && givenData->getPulseControlFactor() != 0.)) {
        isActive = true;

        setText(givenData->get_titleDisplay());
        setObjectName(givenData->get_labelObject());
        QStringList list = givenData->get_labelHighlightIn().split(QRegExp("\\s+"), QString::SkipEmptyParts);
        if(list.size() >= 1) {
            setLabelHighlightIn(list.at(0),
                                indLabelRow, indLabelCol, displayType);
        }
        if(list.size() >= 2) {
            valueHighlightIn = list.at(1).toFloat();
            hasValueHighlightIn = true;
        }
        else {
            valueHighlightIn = 0.f;
            hasValueHighlightIn = false;
        }

        setLabelText(givenData->get_titleDisplay());
        setMidiIn(givenData->get_controlMidiIn());
    }
    else {
        isActive = false;

        valueHighlightIn = 0.f;
        hasValueHighlightIn = false;
        setLabelText("");
        setText("");
        setObjectName("VOID");
    }
    setHighlight(0.f);

    setAlignment(Qt::AlignCenter);

    setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    setMinimumHeight(18);
    setMinimumWidth(20);
}

// SingleLabel VALUES CHANGE

void SingleLabel::setHighlight(float givenValue) {
    if(hasValueHighlightIn) {
        highlight = (givenValue == valueHighlightIn);
    }
    else {
        highlight = (givenValue != 0.f);
    }
    setProperty("highlight", highlight);
    style()->unpolish(this);
    style()->polish(this);
    QWidget::update();
}
bool SingleLabel::getHighlight(void) {
    return highlight;
}

// SingleLabel ATTRIBUTES ACCESS

void SingleLabel::setLabelText(QString givenLabel) {
    labelText = givenLabel;
}
QString SingleLabel::getLabelText(void) {
    return labelText;
}
void SingleLabel::setLabelHighlightIn(QString givenLabel) {
    labelHighlightIn = givenLabel;
}
QString SingleLabel::getLabelHighlightIn(void) {
    return labelHighlightIn;
}

void SingleLabel::setMidiIn(int givenMidi) {
    midiInControl = givenMidi;
}
int SingleLabel::getMidiIn(void) {
    return midiInControl;
}

// SLOTS
void SingleLabel::inverse(void) {
    setProperty("inversed", true);
    style()->unpolish(this);
    style()->polish(this);
    QWidget::update();
}

void SingleLabel::normal(void) {
    setProperty("inversed", false);
    style()->unpolish(this);
    style()->polish(this);
    QWidget::update();
}
bool SingleLabel::getIsActive(void) {
    return isActive;
}
void SingleLabel::setIsActive(bool is_active) {
    isActive = is_active;
}
void SingleLabel::setLabelHighlightIn(QString givenLabel,
                                      int indSliderRow, int indSliderCol, DisplayType displayType) {
    labelHighlightIn = givenLabel;
    if(labelHighlightIn != "XXXX") {
//        if(hashLabelHighlightIn.contains(labelHighlightIn)) {
//            qDebug() << "Duplicate Label label HighlightIn " << labelHighlightIn; // << std::endl;
//            exit(0);
//        }
        hashLabelHighlightIn.insert(labelHighlightIn, displayType);
        hashLabelHighlightIn.insert(labelHighlightIn, indSliderCol);
        hashLabelHighlightIn.insert(labelHighlightIn, indSliderRow);
    }
}

/////////////////////////////////////////////////////////////
/// \brief LabelRow::LabelRow
/// \param givenTitle
/// \param givenHeight
/// \param tab_inControllers
/// \param parent
///

LabelRow::LabelRow(QString givenTitle,
                   int givenHeight,
                   std::vector<ControllerData> &tab_inControllers,
                   int indLabelRow, DisplayType displayType,
                   bool is_pulse,
                   QWidget *parent)
    : QGroupBox("", parent) {
    title = givenTitle;
    nbLabels = tab_inControllers.size();
    for(unsigned int indLabelCol = 0 ; indLabelCol < nbLabels; indLabelCol++) {
        Labels.push_back( new SingleLabel( &tab_inControllers[indLabelCol],
                                           indLabelRow, indLabelCol, displayType,
                                           is_pulse) );
        Labels[indLabelCol]->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    }
    values = std::vector<int>(nbLabels);

    QHBoxLayout *LabelSingleRowGroupDisplayLayout = new QHBoxLayout;
    // subgroup of labels
    for(unsigned int indCol = 0 ; indCol < nbLabels; indCol++) {
        LabelSingleRowGroupDisplayLayout->addWidget(Labels[indCol], Qt::AlignLeft|Qt::AlignTop);
    }
    setLayout(LabelSingleRowGroupDisplayLayout);
    setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));

    // tries to show title
    setTitle(title);
    // gives the first label type to the row
    setObjectName(tab_inControllers[0].get_labelObject());
    // looks for the first non empty Label to give his type to the row
    if(objectName() == QString("EMPTY")) {
        for(unsigned int indLabelCol = 1 ; indLabelCol < nbLabels; indLabelCol++) {
            setObjectName(tab_inControllers[indLabelCol].get_labelObject());
            if(objectName() != QString("EMPTY")) {
                break;
            }
        }
    }
    setMaximumHeight(givenHeight);
}


void LabelRow::setValues(std::vector<int> newValues) {
    if(newValues.size() != nbLabels) {
        qDebug() << "Number of titles " << int(newValues.size())
                 << " does not correspond to the number of Labels " << nbLabels;
        return;
    }
    for(unsigned int indLabelCol = 0 ; indLabelCol < nbLabels; indLabelCol++) {
        values[indLabelCol] = newValues[indLabelCol];
    }
    QWidget::update();
}

void LabelRow::setValue(unsigned int index, int value) {
    if(index >= nbLabels) {
        qDebug() << "Value index " << index
                 << " does not correspond to the number of Labels " << nbLabels;
        return;
    }
    values[index] = value;
    QWidget::update();
}
void LabelRow::setHighlight(unsigned int index, bool onOff) {
    if(index >= nbLabels) {
        qDebug() << "Value index " << index
                 << " does not correspond to the number of Labels " << nbLabels;
        return;
    }
    Labels[index]->setHighlight(onOff);
    QWidget::update();
}

SingleLabel *LabelRow::getSingleLabel(unsigned int indLabelCol) {
    if( indLabelCol < Labels.size()) {
        return Labels[indLabelCol];
    }
    qDebug() << "Out of range Label " << indLabelCol << " " << Labels.size() << " " << title; // << std::endl;
    return NULL;
}

int LabelRow::getSize(void) {
    return nbLabels;
}

class BoxGridDisplay;

/////////////////////////////////////////////////////////////
/// \brief Box::Box

Box::Box(QObject *parent)
    : QObject(parent) {
    init();
}
void Box::init(void) {
    value = 0;
    rectBox.setCoords(0,0,0,0);
    highlight = false;
    inversed = false;
    labelHighlightIn = "";
    valueHighlightIn = 0.f;
    hasValueHighlightIn = false;
    parentGrid = NULL;
    isActive = false;
}

void Box::setCoords(int t, int l, int b, int r) {
    rectBox.setCoords( t, l, b, r );
}

void Box::setHighlight(float givenValue) {
    if(hasValueHighlightIn) {
        highlight = (givenValue == valueHighlightIn);
    }
    else {
        highlight = (givenValue != 0.);
    }
}
bool Box::getHighlight(void) {
    return highlight;
}

BoxGridDisplay * Box::getParentGrid(void) {
    return parentGrid;
}
void Box::setParentGrid(BoxGridDisplay *givenParentGrid) {
    parentGrid = givenParentGrid;
}

// SLOTS
void Box::inverse(void) {
    inversed = true;
    emit parentGrid->repaint();
}

void Box::normal(void) {
    inversed = false;
    emit parentGrid->repaint();
}

bool Box::getInverse(void) {
    return inversed;
}

QString Box::getLabelHighlightIn(void) {
    return labelHighlightIn;
}

float Box::getValueHighlightIn(void) {
    return valueHighlightIn;
}

void Box::setLabelHighlightIn(QString givenLabel,
                              int indBoxRow, int indBoxCol, DisplayType displayType) {
    labelHighlightIn = givenLabel;
    if(labelHighlightIn != "XXXX") {
//        if(hashGridHighlightIn.contains(labelHighlightIn)) {
//            qDebug() << "Duplicate Grid label HighlightIn " << labelHighlightIn; // << std::endl;
//            exit(0);
//        }
        hashGridHighlightIn.insert(labelHighlightIn, displayType);
        hashGridHighlightIn.insert(labelHighlightIn, indBoxCol);
        hashGridHighlightIn.insert(labelHighlightIn, indBoxRow);
    }
}
void Box::setLabelUDPIn(QString givenLabel, bool is_pulse,
                        int indBoxRow, int indBoxCol, DisplayType displayType) {
    labelUDPIn = givenLabel;
    if(is_pulse && !(givenLabel.isEmpty())) {
        int j = givenLabel.indexOf("_can", 0);
        if(j != -1) {
            labelUDPIn.insert(j,"_pulse");
        }
    }
    if(labelUDPIn != "XXXX") {
        if(hashGridUDPIn.contains(labelUDPIn)) {
            qDebug() << "Duplicate Grid label UDPIn " << labelUDPIn
                     << " row " << indBoxRow << " col " << indBoxCol; // << std::endl;
            exit(0);
        }
//        qDebug() << "Insert Grid label UDPIn " << labelUDPIn
//                 << " row " << indBoxRow << " col " << indBoxCol; // << std::endl;
        hashGridUDPIn.insert(labelUDPIn, displayType);
        hashGridUDPIn.insert(labelUDPIn, indBoxCol - 1);
        hashGridUDPIn.insert(labelUDPIn, indBoxRow - 1);
    }
}

void Box::setValueHighlightIn(int givenValue) {
    valueHighlightIn = givenValue;
}

void Box::setHasValueHighlightIn(bool givenValue) {
    hasValueHighlightIn = givenValue;
}

QRect *Box::getRect(void) {
    return &rectBox;
}

void Box::setValue(int newValue) {
    value = newValue;
}

int Box::getValue(void) {
    return value;
}
bool Box::getIsActive(void) {
    return isActive;
}
void Box::setIsActive(bool is_active) {
    isActive = is_active;
}


/////////////////////////////////////////////////////////////
/// \brief BoxGridDisplay::BoxGridDisplay
/// \param givenTitle
/// \param givenNbRows
/// \param givenNbCols
/// \param tab_inControllers
/// \param parent
///
BoxGridDisplay::BoxGridDisplay(QString givenTitle,
                               int givenNbRows, int givenNbCols,
                               std::vector<std::vector<ControllerData>> &tab_inControllers,
                               bool is_pulse,
                               QWidget *parent)
    : QGroupBox("", parent) {
//    qDebug() << "new BoxGridDisplay";
   title = givenTitle;
   nbRows = givenNbRows;
   nbCols = givenNbCols;
   boxSize = tab_inControllers[0][0].get_size();
   textColor = Qt::black;
   int xpos = 5;
   int ypos = 20;
   boxes = std::vector<Box>(nbRows * nbCols);
   int horizSize = 0;
   int verticSize = 0;
   for(int indRow = 0 ; indRow < nbRows; indRow++) {
       int horizSizeRow = 0;
       for(int indCol = 0 ; indCol < nbCols; indCol++) {
           boxes[indRow * nbCols + indCol].init();
           boxes[indRow * nbCols + indCol].setParentGrid(this);
           int intervalleToNext = tab_inControllers[(indRow==nbRows-1?nbRows-2:indRow)][(indCol==nbCols-1?nbCols-2:indCol)].intervalle;
           if(indRow >= 1 && indCol >= 1
                   && tab_inControllers[indRow - 1][indCol - 1].get_labelObject() == "VOID") {
               boxes[indRow * nbCols + indCol].setCoords(0,0,0,0);
           }
           else {
               boxes[indRow * nbCols + indCol]
                       .setCoords(xpos + horizSizeRow,
                                  ypos + verticSize,
                                  xpos + horizSizeRow + boxSize,
                                  ypos + verticSize + boxSize);
           }
           if(indRow >= 1 && indCol >= 1) {
               if(!is_pulse || (is_pulse && tab_inControllers[indRow - 1][indCol - 1].getPulseControlFactor() != 0.)) {
                   QStringList list = tab_inControllers[indRow - 1][indCol - 1].get_labelHighlightIn().split(QRegExp("\\s+"), QString::SkipEmptyParts);
                   boxes[indRow * nbCols + indCol].setIsActive(true);
                   if(list.size() >= 1) {
                       boxes[indRow * nbCols + indCol].setLabelHighlightIn(list.at(0),
                                                                           indRow, indCol, _Beat_display);
                   }
                   if(list.size() >= 2) {
                       boxes[indRow * nbCols + indCol].setValueHighlightIn(list.at(1).toFloat());
                       boxes[indRow * nbCols + indCol].setHasValueHighlightIn(true);
                   }
                   else {
                       boxes[indRow * nbCols + indCol].setValueHighlightIn(0.f);
                       boxes[indRow * nbCols + indCol].setHasValueHighlightIn(false);
                   }
               }
               boxes[indRow * nbCols + indCol].setLabelUDPIn(
                       tab_inControllers[indRow - 1][indCol - 1].get_labelUDPIn(),
                       is_pulse,
                       indRow, indCol, _Beat_display);
           }
           else {
               boxes[indRow * nbCols + indCol].setLabelHighlightIn("XXXX",
                                                                   indRow, indCol, _Beat_display);
               boxes[indRow * nbCols + indCol].setLabelUDPIn("XXXX",
                                                             is_pulse,
                                                             indRow, indCol, _Beat_display);
           }
           if( indCol == nbCols - 1) {
               horizSizeRow += boxSize - 5;
               if( indRow == nbRows - 1) {
                    verticSize += boxSize;
               }
               else {
                    verticSize += boxSize + intervalleToNext;
               }
           }
           else {
               horizSizeRow += boxSize + intervalleToNext;
           }
       }
       if(horizSize < horizSizeRow) {
           horizSize = horizSizeRow;
       }
   }

   // tries to show title
   setTitle(title);
   setObjectName("FLASH");

   // dimensions
   int totalWidth = horizSize;
   int totalHeight = verticSize;

   // setTitle(tr("TEST"));
   // setObjectName(tr("TEST"));
   QVBoxLayout *LabelGridDisplayLayout = new QVBoxLayout;
   // Empty slider area
   QSpacerItem *emptyControl = new QSpacerItem(totalWidth , totalHeight);
   LabelGridDisplayLayout->addItem(emptyControl);
   setLayout(LabelGridDisplayLayout);
   resize(totalWidth , totalHeight);
   setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
//   qDebug() << "end of new BoxGridDisplay";
}

void BoxGridDisplay::setValue(int indRow, int indCol, int value) {
    if(indRow >= 1 && indCol >= 1) {
        boxes[indRow * nbCols + indCol].setValue(value);
    }
    QWidget::update();
}
void BoxGridDisplay::setHighlight(int indRow, int indCol, float value) {
    if(indRow >= 1 && indCol >= 1) {
        boxes[indRow * nbCols + indCol].setHighlight(value);
    }
    QWidget::update();
}
QString BoxGridDisplay::getLabelHighlightIn(int indRow, int indCol) {
    if(indRow >= 1 && indCol >= 1) {
        return boxes[indRow * nbCols + indCol].getLabelHighlightIn();
    }
    return "";
}

float BoxGridDisplay::getValueHighlightIn(int indRow, int indCol) {
    if(indRow >= 1 && indCol >= 1) {
        return boxes[indRow * nbCols + indCol].getValueHighlightIn();
    }
    return 0.f;
}
Box *BoxGridDisplay::getBox(int indRow, int indCol) {
    if(indRow >= 1 && indCol >= 1) {
        return &(boxes[indRow * nbCols + indCol]);
    }
    return NULL;
}
void BoxGridDisplay::paintEvent(QPaintEvent *event)
{
   QPainter painter;
   QGroupBox::paintEvent(event);
   painter.begin(this);
   for(int indRow = 0 ; indRow < nbRows ; indRow++) {
       for(int indCol = 0 ; indCol < nbCols ; indCol++) {
           if(indRow >= 1 && indCol >= 1) {
               QRect *curRect = boxes[indRow * nbCols + indCol].getRect();
               if(boxes[indRow * nbCols + indCol].getIsActive()) {
                   int curVal
                           = boxes[indRow * nbCols + indCol].getValue();
                   if(boxes[indRow * nbCols + indCol].getInverse()) {
                       painter.fillRect(*curRect,
                                        Qt::cyan);
                   }
                   else if(curVal > 0) {
                       painter.fillRect(*curRect,
                                        SlidersBeatData[indRow - 1][indCol - 1].bgOnColor);
                   }
                   else {
                       painter.fillRect(*curRect,
                                        SlidersBeatData[indRow - 1][indCol - 1].bgOffColor);
                   }
                   if(boxes[indRow * nbCols + indCol].getHighlight()) {
                       QPen pen(Qt::red);
                       pen.setWidth(2);
                       painter.setPen(pen);
                       painter.drawRect(*curRect);
                   }

                   painter.setPen(textColor);
                   painter.setFont(QFont("Arial", SlidersBeatData[indRow - 1][indCol - 1].fontSize, QFont::Bold));
                   painter.drawText(*curRect,
                                    Qt::AlignCenter,
                                    QString::number(curVal));
               }
               else {
                   painter.fillRect(*curRect,
                                    Qt::black);
               }
           }
           else {
               painter.setPen(Qt::white);
               painter.setFont(QFont("Arial", SlidersBeatData[0][0].fontSize - 2, QFont::Bold));
               if(indRow == 0 && indCol >= 1) {
                   painter.drawText(*(boxes[indRow * nbCols + indCol].getRect()),
                           Qt::AlignCenter,
                           SlidersBeatData[0][indCol-1].get_titleDisplay());
               }
               else if(indRow >= 1 && indCol == 0) {
                   painter.drawText(*(boxes[indRow * nbCols + indCol].getRect()),
                           Qt::AlignCenter,
                           SlidersBeatData[indRow - 1][0].get_titleDisplay());
               }
           }
       }
   }
   QWidget::paintEvent(event);
   painter.end();
}

/////////////////////////////////////////////////////////////
/// \brief LEDBar::LEDBar
/// \param givenXOffset
/// \param givenYOffset
/// \param givenWidth
/// \param givenHeight
/// \param givenIntervalToNext
/// \param givenLabelHeight
/// \param upperLabel
/// \param doubleLayerLabels
/// \param givenTitle
/// \param givenLabelHighlightIn
/// \param givenAmplitude
/// \param givenFontSize
/// \param givenbgOffColor
/// \param givenbgOnColor
/// \param givenbarNormalColor
/// \param givenbarOverflowColor
/// \param givenrBorder
/// \param givengBorder
/// \param givenbBorder
/// \param givenrText
/// \param givengText
/// \param givenbText
///

LEDBar::LEDBar(int givenXOffset,
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
               float givenrBorder, float givengBorder, float givenbBorder,
               float givenrText, float givengText, float givenbText,
               QObject* parent)
                : QObject(parent) {
    title = givenTitle;
    QStringList list = givenLabelHighlightIn.split(QRegExp("\\s+"), QString::SkipEmptyParts);
    labelHighlightIn = "";
    labelUDPIn = "";
    valueHighlightIn = 0.f;
    hasValueHighlightIn = false;

    if(list.size() >= 1) {
        setLabelHighlightIn(list.at(0),
                            indLEDBarRow, indLEDBarCol, displayType);
    }
    if(list.size() >= 2) {
        valueHighlightIn = list.at(1).toFloat();
        hasValueHighlightIn = true;
    }
    else {
        valueHighlightIn = 0.f;
        hasValueHighlightIn = false;
    }
    setLabelUDPIn(givenLabelUDPIn, is_pulse,
                  indLEDBarRow, indLEDBarCol, displayType);
    XoffSet = givenXOffset;
    YoffSet = givenYOffset + givenLabelHeight;
    height = givenHeight;
    int labelWidth = givenWidth + givenIntervalToNext;
    if(doubleLayerLabels) {
        labelWidth *= 2;
        YoffSet += givenLabelHeight;
    }
    int labelYOffset = YoffSet - givenLabelHeight;
    if(upperLabel) {
        labelYOffset -= givenLabelHeight;
    }
    rectLabel.setCoords(XoffSet, labelYOffset,
                        XoffSet + labelWidth - 2 , labelYOffset + givenLabelHeight);
    rectBg.setCoords(XoffSet, YoffSet,
                     XoffSet + givenWidth, YoffSet + height);
    rectBar.setCoords(XoffSet + 2, YoffSet + givenHeight,
                      XoffSet + givenWidth - 1, YoffSet + height);
    if(is_active) {
        isActive = true;
        bgOffColor = *givenbgOffColor;
        bgOnColor = *givenbgOnColor;
        barNormalColor = *givenbarNormalColor;
        barOverflowColor = *givenbarOverflowColor;
        barUnderflowColor = *givenbarUnderflowColor;
        barColor = barNormalColor;
        bgColor = bgOffColor;
        borderColor.setRgbF(givenrBorder, givengBorder, givenbBorder);
        textColor.setRgbF(givenrText, givengText, givenbText);
    }
    else {
        isActive = false;
        bgOffColor = Qt::black;
        bgOnColor = Qt::black;
        barNormalColor = Qt::black;
        barOverflowColor = Qt::black;
        barUnderflowColor = Qt::black;
        barColor = Qt::black;
        bgColor = Qt::black;
        borderColor.setRgbF(0,0,0);
        textColor.setRgbF(0,0,0);
    }

    highlight = false;
    inversed = false;
    amplitude = givenAmplitude;
    fontSize = givenFontSize;
    parentLEDBarRow = givenParentLEDBarRow;
}
void LEDBar::setValue(float newValue) {
    float ratio = (newValue / amplitude);
    int top = int(YoffSet + height - ratio * height);
    if(ratio < 0.f) {
        barColor = barUnderflowColor;
        if(-ratio <= 1.f) {
            top = int(YoffSet + height + ratio * height);
            rectBar.setTop(top);
        }
        else {
            top = int(YoffSet + height + ratio * height);
            rectBar.setTop(YoffSet);
        }
    }
    else if(ratio <= 1.f) {
        rectBar.setTop(top);
        barColor = barNormalColor;
    }
    else {
        rectBar.setTop(YoffSet);
        barColor = barOverflowColor;
    }
    if(newValue != 0) {
        bgColor = bgOnColor;
    }
    else {
        bgColor = bgOffColor;
    }
//    barColor.setRgbF((1.f - ratio) * barNormalColor.redF() + ratio * barOverflowColor.redF(),
//                     (1.f - ratio) * barNormalColor.greenF() + ratio * barOverflowColor.greenF(),
//                     (1.f - ratio) * barNormalColor.blueF() + ratio * barOverflowColor.blueF());
}
void LEDBar::setHighlight(bool onOff) {
        highlight = onOff;
}
bool LEDBar::getHighlight(void) {
    return highlight;
}

LEDBarRow *LEDBar::getParentLEDBarRow(void) {
    return parentLEDBarRow;
}

bool LEDBar::getIsActive(void) {
    return isActive;
}

// SLOTS
void LEDBar::inverse(void) {
    inversed = true;
    emit parentLEDBarRow->repaint();
}

void LEDBar::normal(void) {
    inversed = false;
    emit parentLEDBarRow->repaint();
}

bool LEDBar::getInverse(void) {
    return inversed;
}
void LEDBar::setLabelHighlightIn(QString givenLabel,
                                 int indLEDBArRow, int indLEDBArCol, DisplayType displayType) {
    labelHighlightIn = givenLabel;
    if(labelHighlightIn != "XXXX") {
//        if(hashLEDBarHighlightIn.contains(labelHighlightIn)) {
//            qDebug() << "Duplicate LEDBar label HighlightIn " << labelHighlightIn; // << std::endl;
//            exit(0);
//        }
        hashLEDBarHighlightIn.insert(labelHighlightIn, displayType);
        hashLEDBarHighlightIn.insert(labelHighlightIn, indLEDBArCol);
        hashLEDBarHighlightIn.insert(labelHighlightIn, indLEDBArRow);
    }
}
void LEDBar::setLabelUDPIn(QString givenLabel, bool is_pulse,
                           int indLEDBArRow, int indLEDBArCol,
                           DisplayType displayType) {
    labelUDPIn = givenLabel;
    if(is_pulse && !(givenLabel.isEmpty())) {
        int j = givenLabel.indexOf("_can", 0);
        if(j != -1) {
            labelUDPIn.insert(j,"_pulse");
        }
    }
    if(labelUDPIn != "XXXX") {
        if(hashLEDBarHighlightIn.contains(labelUDPIn)) {
            qDebug() << "Duplicate LEDBar label UDPIn " << labelUDPIn; // << std::endl;
            exit(0);
        }
        hashLEDBarUDPIn.insert(labelUDPIn, displayType);
        hashLEDBarUDPIn.insert(labelUDPIn, indLEDBArCol);
        hashLEDBarUDPIn.insert(labelUDPIn, indLEDBArRow);
    }
}


/////////////////////////////////////////////////////////////
/// \brief LEDBarRow::LEDBarRow
/// \param givenTitle
/// \param givenHeight
/// \param tab_inControllers
/// \param doubleLayerLabels
/// \param givenrBorder
/// \param givengBorder
/// \param givenbBorder
/// \param givenrrText
/// \param givenrgText
/// \param givenrbText
/// \param givenHighlight
/// \param parent
///

LEDBarRow::LEDBarRow(QString givenTitle,
                     int givenHeight,
                     std::vector<ControllerData> &tab_inControllers,
                     int indLEDBarRow, DisplayType displayType,
                     bool is_pulse,
                     bool doubleLayerLabels,
                     float givenrBorder, float givengBorder, float givenbBorder,
                     float givenrrText, float givenrgText, float givenrbText,
                     QWidget *parent)
    : QGroupBox("", parent) {
    title = givenTitle;
    nbLEDBars = tab_inControllers.size();
    int xPos = 0;
    int maxLabelHeight = 0;
    for(unsigned int indLEDBarCol = 0 ; indLEDBarCol < nbLEDBars; indLEDBarCol++) {
        int intervalToNext = tab_inControllers[indLEDBarCol].intervalle;
        int labelHeight = int(tab_inControllers[indLEDBarCol].fontSize * 1.3f);
        if(labelHeight > maxLabelHeight)  {
            maxLabelHeight = labelHeight;
        }
        float amplitude = 0.f;
        if(!is_pulse) {
            amplitude = tab_inControllers[indLEDBarCol].get_amplitude();
        }
        else {
            amplitude = tab_inControllers[indLEDBarCol].get_amplitude()
                        * tab_inControllers[indLEDBarCol].getPulseControlFactor();
        }
        LEDBars.push_back( new LEDBar(  xPos + 10,
                                        20,
                                        tab_inControllers[indLEDBarCol].get_size(),
                                        givenHeight,
                                        intervalToNext,
                                        labelHeight,
                                        this,
                                        indLEDBarRow, indLEDBarCol, displayType,
                                        is_pulse,
                                        (doubleLayerLabels && (indLEDBarCol %2 == 0)),
                                        doubleLayerLabels,
                                        tab_inControllers[indLEDBarCol].get_titleDisplay(),
                                        ((!is_pulse && tab_inControllers[indLEDBarCol].get_labelObject() != QString("EMPTY"))
                                         || (is_pulse && tab_inControllers[indLEDBarCol].getPulseControlFactor() != 0.)),
                                        tab_inControllers[indLEDBarCol].get_labelHighlightIn(),
                                        tab_inControllers[indLEDBarCol].get_labelUDPIn(),
                                        amplitude,
                                        tab_inControllers[indLEDBarCol].fontSize,
                                        &tab_inControllers[indLEDBarCol].bgOffColor,
                                        &tab_inControllers[indLEDBarCol].bgOnColor,
                                        &tab_inControllers[indLEDBarCol].barNormalColor,
                                        &tab_inControllers[indLEDBarCol].barOverflowColor,
                                        &tab_inControllers[indLEDBarCol].barUnderflowColor,
                                        givenrBorder, givengBorder, givenbBorder,
                                        givenrrText, givenrgText, givenrbText) );
//        if(indLEDBarCol == nbLEDBars - 1) {
//            xPos += tab_inControllers[indLEDBarCol].get_size();
//        }
//        else {
            xPos += tab_inControllers[indLEDBarCol].get_size() + intervalToNext;
//        }
    }
    if(doubleLayerLabels) {
        maxLabelHeight *= 2;
    }

    // tries to show title
    setTitle(title);
    // gives the first label type to the row
    setObjectName(tab_inControllers[0].get_labelObject());
    // looks for the first non empty Label to give his type to the row
    if(objectName() == QString("EMPTY")) {
        for(unsigned int indLEDBarCol = 0 ; indLEDBarCol < nbLEDBars; indLEDBarCol++) {
            setObjectName(tab_inControllers[indLEDBarCol].get_labelObject());
            if(objectName() != QString("EMPTY")) {
                break;
            }
        }
    }

    // dimensions
    int totalWidth = xPos;
    int totalHeight = givenHeight + maxLabelHeight;

    // setTitle(tr("TEST"));
    // setObjectName(tr("TEST"));
    QVBoxLayout *LEDBarRowLayout = new QVBoxLayout;
    // Empty slider area
    QSpacerItem *emptyControl = new QSpacerItem(totalWidth , totalHeight);
    LEDBarRowLayout->addItem(emptyControl);
    setLayout(LEDBarRowLayout);
    resize(totalWidth , totalHeight);
    setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
}


void LEDBarRow::setValues(std::vector<float> newValues) {
    if(newValues.size() != nbLEDBars) {
        qDebug() << "Number of titles " << int(newValues.size())
                 << " does not correspond to the number of LEDBars " << nbLEDBars;
        return;
    }
    for(unsigned int indLEDBarCol = 0 ; indLEDBarCol < nbLEDBars; indLEDBarCol++) {
        LEDBars[indLEDBarCol]->setValue(newValues[indLEDBarCol]);
    }
    QWidget::update();
}

void LEDBarRow::setValue(unsigned int index, float value) {
    if(index >= nbLEDBars) {
        qDebug() << "Value index " << index
                 << " does not correspond to the number of LEDBars " << nbLEDBars;
        return;
    }
    LEDBars[index]->setValue(value);
    QWidget::update();
}
void LEDBarRow::setHighlight(unsigned int index, bool onOff) {
    if(index >= nbLEDBars) {
        qDebug() << "Value index " << index
                 << " does not correspond to the number of LEDBars " << nbLEDBars;
        return;
    }
    LEDBars[index]->setHighlight(onOff);
    QWidget::update();
}
//http://doc.qt.io/qt-5/qtwidgets-widgets-wiggly-example.html
void LEDBarRow::paintEvent(QPaintEvent *event)
{
    // QGroupBox::paintEvent(event);
    QPainter painter;
    QGroupBox::paintEvent(event);
    QPen pen;
    QBrush brush;
    painter.begin(this);
    for(unsigned int indLEDBarCol = 0 ; indLEDBarCol < nbLEDBars; indLEDBarCol++) {
        if(LEDBars[indLEDBarCol]->getHighlight()) {
            if(LEDBars[indLEDBarCol]->getInverse()) {
                painter.fillRect(LEDBars[indLEDBarCol]->rectBg,
                                 QBrush(Qt::magenta));
                painter.fillRect(LEDBars[indLEDBarCol]->rectBar,
                                 QBrush(Qt::cyan));
            }
            else {
                painter.fillRect(LEDBars[indLEDBarCol]->rectBg ,
                                 QBrush(LEDBars[indLEDBarCol]->bgColor));
                painter.fillRect(LEDBars[indLEDBarCol]->rectBar,
                                 QBrush(LEDBars[indLEDBarCol]->barColor));
            }
        }
        else {
            if(LEDBars[indLEDBarCol]->getInverse()) {
                painter.fillRect(LEDBars[indLEDBarCol]->rectBg,
                                 QBrush(Qt::cyan));
                painter.fillRect(LEDBars[indLEDBarCol]->rectBar,
                                 QBrush(Qt::magenta));
            }
            else {
                painter.fillRect(LEDBars[indLEDBarCol]->rectBg ,
                                 QBrush(LEDBars[indLEDBarCol]->bgColor));
                painter.fillRect(LEDBars[indLEDBarCol]->rectBar,
                                 QBrush(LEDBars[indLEDBarCol]->barColor));
            }
        }


        if(LEDBars[indLEDBarCol]->getHighlight()) {
            QPen pen(Qt::red);
            pen.setWidth(2);
            painter.setPen(pen);
            painter.drawRect(LEDBars[indLEDBarCol]->rectBg);
        }
//        painter.setPen(QPen(LEDBars[indLEDBarCol]->borderColor));
//        painter.drawRect(LEDBars[indLEDBarCol]->rectBg);

        painter.setPen(QPen(LEDBars[indLEDBarCol]->textColor));
        painter.setFont(QFont("Arial", LEDBars[indLEDBarCol]->fontSize, QFont::Bold));
        painter.drawText(LEDBars[indLEDBarCol]->rectLabel,
                         Qt::AlignLeft | Qt::AlignBottom,
                         LEDBars[indLEDBarCol]->title);
    }
    painter.end();
}

LEDBar *LEDBarRow::getLEDBar(unsigned int indLEDBarCol) {
    if( indLEDBarCol < LEDBars.size()) {
        return LEDBars[indLEDBarCol];
    }
    qDebug() << "Out of range LED Bar " << indLEDBarCol << " " << LEDBars.size() << " " << title; // << std::endl;
    return NULL;
}


/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
// BEAT TAB
void GraphicalDisplays::create_BeatTabDisplays(
        bool is_pulse
        ) {
    // GRAPHS LED BARS
    graphicalLEDBarRows = std::vector<LEDBarRow*>(_MaxSlidersBeatData);
    for(unsigned int ind = 0 ; ind < _MaxSlidersBeatData ; ind++ ) {
        graphicalLEDBarRows[ind] = NULL;
    }
    // ALL THE LED BAR ROWS AS DEFINED IN THE DATA
    // the first three are not created because they correspond to the grid
    for(unsigned int indRow = _PixelAccCentSlidersBeatData ; indRow < _MaxSlidersBeatData; indRow++) {
        graphicalLEDBarRows[indRow]
                = new LEDBarRow(SlidersBeatData_LEDBarRowTitles[indRow],
                                50, SlidersBeatData[indRow],
                                indRow,
                                (is_pulse?_Beat_Pulse_display:_Beat_display),
                                is_pulse, true);
    }

    // GRAPHS LABELS
    graphicalLabelRows = std::vector<LabelRow*>(_MaxButtonsBeatData);
    for(unsigned int ind = 0 ; ind < _MaxButtonsBeatData ; ind++ ) {
        graphicalLabelRows[ind] = NULL;
    }
    // ALL THE LABEL ROWS AS DEFINED IN THE DATA
    for(unsigned int indRow = 0 ; indRow < _MaxButtonsBeatData; indRow++) {
        graphicalLabelRows[indRow] = new LabelRow(ButtonsBeatData_LabelTitles[indRow],
                                                  48, ButtonsBeatData[indRow],
                                                  indRow,
                                                  (is_pulse?_Beat_Pulse_display:_Beat_display),
                                                  is_pulse);
    }

    // GRID of inter-layer flashes
    graphsFlashGrid = new BoxGridDisplay("FLASH", 4, 7, SlidersBeatData, is_pulse);

}

/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
// MIXER TAB
void GraphicalDisplays::create_MixerTabDisplays(
        bool is_pulse
        ) {
    // Mixer LED BARS
    graphicalLEDBarRows = std::vector<LEDBarRow*>(_MaxSlidersMixerData);
    for(unsigned int ind = 0 ; ind < _MaxSlidersMixerData ; ind++ ) {
        graphicalLEDBarRows[ind] = NULL;
    }
    // ALL THE LED BAR ROWS AS DEFINED IN THE DATA
    for(unsigned int indRow = 0 ; indRow < _MaxSlidersMixerData; indRow++) {
        graphicalLEDBarRows[indRow]
                = new LEDBarRow(SlidersMixerData_LEDBarRowTitles[indRow],
                                50, SlidersMixerData[indRow],
                                indRow,
                                (is_pulse?_Mixer_Pulse_display:_Mixer_display),
                                is_pulse, true);
    }

    // Mixer LABELS
    graphicalLabelRows = std::vector<LabelRow*>(_MaxButtonsMixerData);
    for(unsigned int ind = 0 ; ind < _MaxButtonsMixerData ; ind++ ) {
        graphicalLabelRows[ind] = NULL;
    }
    // ALL THE LABEL ROWS AS DEFINED IN THE DATA
    for(unsigned int indRow = 0 ; indRow < _MaxButtonsMixerData; indRow++) {
        graphicalLabelRows[indRow] = new LabelRow(ButtonsMixerData_LabelTitles[indRow],
                                                  48, ButtonsMixerData[indRow],
                                                  indRow,
                                                  (is_pulse?_Mixer_Pulse_display:_Mixer_display),
                                                  is_pulse);
    }

}
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
// FX TAB
void GraphicalDisplays::create_FXTabDisplays(
        bool is_pulse
        ) {
    // FX LED BARS
    graphicalLEDBarRows = std::vector<LEDBarRow*>(_MaxSlidersFXData);
    for(unsigned int ind = 0 ; ind < _MaxSlidersFXData ; ind++ ) {
        graphicalLEDBarRows[ind] = NULL;
    }
    // ALL THE LED BAR ROWS AS DEFINED IN THE DATA
    for(unsigned int indRow = 0 ; indRow < _MaxSlidersFXData; indRow++) {
        graphicalLEDBarRows[indRow]
                = new LEDBarRow(SlidersFXData_LEDBarRowTitles[indRow],
                                50, SlidersFXData[indRow],
                                indRow,
                                (is_pulse?_FX_Pulse_display:_FX_display),
                                is_pulse, true);
    }

    // FX LABELS
    graphicalLabelRows = std::vector<LabelRow*>(_MaxButtonsFXData);
    for(unsigned int ind = 0 ; ind < _MaxButtonsFXData ; ind++ ) {
        graphicalLabelRows[ind] = NULL;
    }
    // ALL THE LABEL ROWS AS DEFINED IN THE DATA
    for(unsigned int indRow = 0 ; indRow < _MaxButtonsFXData; indRow++) {
        graphicalLabelRows[indRow] = new LabelRow(ButtonsFXData_LabelTitles[indRow],
                                                  48, ButtonsFXData[indRow],
                                                  indRow,
                                                  (is_pulse?_FX_Pulse_display:_FX_display),
                                                  is_pulse);
    }

}
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
// PATHS TAB
void GraphicalDisplays::create_PathsTabDisplays(
        bool is_pulse
        ) {
    // PATHS LED BARS
    graphicalLEDBarRows = std::vector<LEDBarRow*>(_MaxSlidersPathsData);
    for(unsigned int ind = 0 ; ind < _MaxSlidersPathsData ; ind++ ) {
        graphicalLEDBarRows[ind] = NULL;
    }
    // ALL THE LED BAR ROWS AS DEFINED IN THE DATA
    for(unsigned int indRow = 0 ; indRow < _MaxSlidersPathsData; indRow++) {
        graphicalLEDBarRows[indRow]
                = new LEDBarRow(SlidersPathsData_LEDBarRowTitles[indRow],
                                50, SlidersPathsData[indRow],
                                indRow,
                                (is_pulse?_Paths_Pulse_display:_Paths_display),
                                is_pulse,true);
    }

    // PATHS LABELS
    graphicalLabelRows = std::vector<LabelRow*>(_MaxButtonsPathsData);
    for(unsigned int ind = 0 ; ind < _MaxButtonsPathsData ; ind++ ) {
        graphicalLabelRows[ind] = NULL;
    }
    // ALL THE LABEL ROWS AS DEFINED IN THE DATA
    for(unsigned int indRow = 0 ; indRow < _MaxButtonsPathsData; indRow++) {
        graphicalLabelRows[indRow] = new LabelRow(ButtonsPathsData_LabelTitles[indRow],
                                                  48, ButtonsPathsData[indRow],
                                                  indRow,
                                                  (is_pulse?_Paths_Pulse_display:_Paths_display),
                                                  is_pulse);
        graphicalLabelRows[indRow]->setMaximumHeight(46);
    }
}
#if defined(TVW) || defined(demo)
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
// MinimalIF TAB
void GraphicalDisplays::create_MinimalIFTabDisplays(
        bool is_pulse
        ) {
    // MinimalIF LED BARS
    graphicalLEDBarRows = std::vector<LEDBarRow*>(_MaxSlidersMinimalIFData);
    for(unsigned int ind = 0 ; ind < _MaxSlidersMinimalIFData ; ind++ ) {
        graphicalLEDBarRows[ind] = NULL;
    }
    // ALL THE LED BAR ROWS AS DEFINED IN THE DATA
    for(unsigned int indRow = 0 ; indRow < _MaxSlidersMinimalIFData; indRow++) {
        graphicalLEDBarRows[indRow]
                = new LEDBarRow(SlidersMinimalIFData_LEDBarRowTitles[indRow],
                                50, SlidersMinimalIFData[indRow],
                                indRow,
                                (is_pulse?_MinimalIF_Pulse_display:_MinimalIF_display),
                                is_pulse, true);
    }

    // MinimalIF LABELS
    graphicalLabelRows = std::vector<LabelRow*>(_MaxButtonsMinimalIFData);
    for(unsigned int ind = 0 ; ind < _MaxButtonsMinimalIFData ; ind++ ) {
        graphicalLabelRows[ind] = NULL;
    }
    // ALL THE LABEL ROWS AS DEFINED IN THE DATA
    for(unsigned int indRow = 0 ; indRow < _MaxButtonsMinimalIFData; indRow++) {
        graphicalLabelRows[indRow] = new LabelRow(ButtonsMinimalIFData_LabelTitles[indRow],
                                                  48, ButtonsMinimalIFData[indRow],
                                                  indRow,
                                                  (is_pulse?_MinimalIF_Pulse_display:_MinimalIF_display),
                                                  is_pulse);
    }

}
#endif

/////////////////////////////////////////////////////////////
/// \brief GraphicalDisplays::GraphicalDisplays
/// \param givenType
/// \param parent
///

GraphicalDisplays::GraphicalDisplays(DisplayType givenType,
                                     bool is_pulse,
                                     QWidget *parent)
    : QWidget(parent) {
    // time and scene display

    timeSceneBeatDisplay
            = new TimeSceneBeatDisplay(TabbedWindowTitles[givenType+(is_pulse?1:0)],
                 {Qt::red,QColor("orange"),Qt::green,QColor("violet")},
                 100, 150, 50, 40);

    // 2 color palettes
    colorPalette = new ColorDisplay({"PEN", "REPO"}, 2, 20);

    /////////////////////////////////////////////////////////
    QGroupBox *allLEDbarsGraphicalDisplay = new QGroupBox;
    QGridLayout *allLEDbarsGraphicalDisplayLayout = new QGridLayout;
    switch(givenType) {
    case _Beat_display: {
        /////////////////////////////////////////////////////////
        // Beat TAB

        // create displays
        create_BeatTabDisplays(is_pulse);

        // organize them in a grid
        allLEDbarsGraphicalDisplayLayout->addWidget(graphsFlashGrid, 0, 0, 1, 6);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLEDBarRows[_PixelAccCentSlidersBeatData], 0, 6, 1, 2);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLEDBarRows[_BeatSoundBandsSlidersBeatData], 1, 0, 1, 3);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLEDBarRows[_PixelAccCoefSlidersBeatData], 1, 6, 1, 2);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLEDBarRows[_XXXXSlidersBeatData], 2, 6, 1, 2);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLabelRows[_penBWButtonsBeatData], 3, 0, 1, 1);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLabelRows[_penPaletteButtonsBeatData], 3, 1, 1, 5);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLabelRows[_pixelModeButtonsBeatData], 3, 6, 1, 2);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLabelRows[_repopBWButtonsBeatData], 4, 0, 1, 1);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLabelRows[_penBrushButtonsBeatData], 4, 1, 1, 5);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLabelRows[_FlashVideoButtonsBeatData], 4, 6, 1, 2);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLEDBarRows[_RadiusSlidersBeatData], 5, 0, 1, 3);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLEDBarRows[_ColorSlidersBeatData], 5, 3, 1, 5);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLEDBarRows[_MasterBlendSlidersBeatData], 5, 8, 1, 1);
        allLEDbarsGraphicalDisplayLayout->setVerticalSpacing(10);
        allLEDbarsGraphicalDisplayLayout->setHorizontalSpacing(10);
        allLEDbarsGraphicalDisplayLayout->setSizeConstraint(QLayout::SetFixedSize);
        allLEDbarsGraphicalDisplay->setLayout(allLEDbarsGraphicalDisplayLayout);
        allLEDbarsGraphicalDisplay->setTitle("");
        allLEDbarsGraphicalDisplay->setObjectName("VOID");
    }
        break;
    case _Mixer_display: {
        /////////////////////////////////////////////////////////
        // Mixer TAB

        // create displays
        create_MixerTabDisplays(is_pulse);

        // organize them in a grid
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLEDBarRows[_MixingSlidersMixerData], 0, 0, 1, 6);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLEDBarRows[_CameraSlidersMixerData], 0, 6, 1, 2);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLEDBarRows[_MixerSoundBandsSlidersMixerData], 0, 8, 1, 1);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLEDBarRows[_MasterSlidersMixerData], 1, 0, 1, 6);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLEDBarRows[_MovieMediaFreqSlidersMixerData], 1, 6, 1, 2);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLEDBarRows[_RepopSlidersMixerData], 2, 0, 1, 4);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLEDBarRows[_XXXXSlidersMixerData], 2, 4, 1, 2);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLEDBarRows[_PhotoSlidersMixerData], 2, 6, 1, 2);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLabelRows[_TracksButtonsMixerData], 3, 0, 1, 6);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLabelRows[_TrackCopyButtonsMixerData], 3, 6, 1, 2);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLabelRows[_decaySignButtonMixersData], 4, 0 , 1, 6);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLabelRows[_freezeBottomButtonsMixerData], 4, 6, 1, 2);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLEDBarRows[_DecaySlidersMixerData], 5, 0 , 1, 6);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLEDBarRows[_EchoSlidersMixerData], 5, 6, 1, 2);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLEDBarRows[_MasterBlendSlidersMixerData], 5, 8, 1, 1);
        allLEDbarsGraphicalDisplayLayout->setVerticalSpacing(10);
        allLEDbarsGraphicalDisplayLayout->setHorizontalSpacing(10);
        allLEDbarsGraphicalDisplayLayout->setSizeConstraint(QLayout::SetFixedSize);
        allLEDbarsGraphicalDisplay->setLayout(allLEDbarsGraphicalDisplayLayout);
        allLEDbarsGraphicalDisplay->setTitle("");
        allLEDbarsGraphicalDisplay->setObjectName("VOID");
    }
        break;
    case _FX_display: {
        /////////////////////////////////////////////////////////
        // FX TAB

        // create displays
        create_FXTabDisplays(is_pulse);

        // first item row/col nb rows/nb cols
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLEDBarRows[_CAParamsSlidersFXData], 0, 0, 1, 8);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLEDBarRows[_FXSoundBandsSlidersFXData], 0, 5, 1, 1);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLEDBarRows[_CameraParamsSlidersFXData], 1, 0, 1, 8);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLEDBarRows[_noiseSlidersFXData], 2, 0, 1, 5);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLEDBarRows[_VideoSatFXData], 2, 4, 1, 1);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLEDBarRows[_MediaSobelSlidersFXData], 2, 5, 1, 2);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLabelRows[_CATypeButtonFXsData], 3, 0 , 1, 5);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLabelRows[_mediaPlusButtonsFXData], 3, 4, 1, 2);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLabelRows[_cameraCumulButtonsFXData], 3, 6, 1, 1);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLabelRows[_sensorsButtonsFXData], 4, 0, 1, 4);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLabelRows[_noiseTypeButtonFXsData], 4, 4 , 1, 1);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLabelRows[_micOnOffButtonsFXData], 4, 5, 1, 1);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLabelRows[_soundSampleButtonsFXData], 4, 5, 1, 2);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLEDBarRows[_BeatSlidersFXData], 5, 0, 1, 3);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLEDBarRows[_SoundSlidersFXData], 5, 3, 1, 5);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLEDBarRows[_SoundVolumeSlidersFXData], 5, 6, 1, 1);
        allLEDbarsGraphicalDisplayLayout->setVerticalSpacing(10);
        allLEDbarsGraphicalDisplayLayout->setHorizontalSpacing(10);
        allLEDbarsGraphicalDisplayLayout->setSizeConstraint(QLayout::SetFixedSize);
        allLEDbarsGraphicalDisplay->setLayout(allLEDbarsGraphicalDisplayLayout);
        allLEDbarsGraphicalDisplay->setTitle("");
        allLEDbarsGraphicalDisplay->setObjectName("VOID");
    }
        break;
    case _Paths_display: {
        /////////////////////////////////////////////////////////
        // Paths TAB

        // create displays
        create_PathsTabDisplays(is_pulse);

        // organize them in a grid
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLabelRows[_partMoveButtonsPathsData], 0, 0, 1, 4);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLabelRows[_partStrokeButtonsPathsData], 0, 4, 1, 4);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLabelRows[_partExitButtonsPathsData], 1, 0, 1, 4);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLabelRows[_partColorButtonsPathsData], 1, 4, 1, 4);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLabelRows[_partInitButtonsPathsData], 2, 0, 1, 8);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLabelRows[_partRepopPathButtonsPathsData], 3, 0, 1, 8);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLabelRows[_partFollowPathButtonsPathsData], 4, 0, 1, 8);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLabelRows[_partRepulsePathButtonsPathsData], 5, 0, 1, 8);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLabelRows[_pathRecordButtonsPathsData], 6, 0, 1, 8);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLabelRows[_pathReplayButtonsPathsData], 7, 0, 1, 8);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLabelRows[_setUpButtonsPathsData], 8, 0, 1, 3);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLabelRows[_clearButtonsPathsData], 8, 3, 1, 5);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLEDBarRows[_ParticleRadiusSlidersPathsData], 9, 0, 1, 3);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLEDBarRows[_ParticleFluiditySlidersPathsData], 9, 3, 1, 3);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLEDBarRows[_ParticleNoiseSlidersPathsData], 9, 6, 1, 3);
        allLEDbarsGraphicalDisplayLayout->setVerticalSpacing(10);
        allLEDbarsGraphicalDisplayLayout->setHorizontalSpacing(10);
        allLEDbarsGraphicalDisplayLayout->setSizeConstraint(QLayout::SetFixedSize);

        allLEDbarsGraphicalDisplay->setLayout(allLEDbarsGraphicalDisplayLayout);
        allLEDbarsGraphicalDisplay->setTitle("");
        allLEDbarsGraphicalDisplay->setObjectName("VOID");
    }
        break;
#if defined(TVW) || defined(demo)
    case _MinimalIF_display: {
        /////////////////////////////////////////////////////////
        // MinimalIF TAB

        // create displays
        create_MinimalIFTabDisplays(is_pulse);

        // first item row/col nb rows/nb cols
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLEDBarRows[_EchoMinimalIFData], 0, 0, 1, 2);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLEDBarRows[_MixingMinimalIFData], 0, 2, 1, 5);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLEDBarRows[_FXSoundBandsSlidersMinimalIFData], 0, 6, 1, 3);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLEDBarRows[_PenRepopMinimalIFData], 1, 0, 1, 2);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLEDBarRows[_MasterMinimalIFData], 1, 2, 1, 6);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLEDBarRows[_PixelsMinimalIFData], 2, 0, 1, 3);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLEDBarRows[_FlashMinimalIFData], 2, 3, 1, 5);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLabelRows[_PenColorMinimalIFData], 3, 0, 1, 3);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLabelRows[_PixeCAlMinimalIFData], 3, 2, 1, 3);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLabelRows[_SampleMinimalIFData], 3, 5, 1, 1);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLabelRows[_PenGreyButtonTVWsData], 4, 0, 1, 1);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLabelRows[_DecayButtonTVWsData], 4, 1, 1, 3);
#if defined(TVW)
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLabelRows[_flashCameraMinimalIFData], 4, 4, 1, 4);
#endif
#if defined(demo)
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLabelRows[_trackBrushMinimalIFData], 4, 4, 1, 4);
#endif
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLEDBarRows[_PenSlidersMinimalIFData], 5, 0, 1, 1);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLEDBarRows[_DecaySlidersMinimalIFData], 5, 1, 1, 3);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLEDBarRows[_CameraSlidersMinimalIFData], 5, 4, 1, 1);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLEDBarRows[_SoundSlidersMinimalIFData], 5, 5, 1, 3);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLEDBarRows[_MasterBlendSlidersMinimalIFData], 5, 8, 1, 1);
        allLEDbarsGraphicalDisplayLayout->setVerticalSpacing(10);
        allLEDbarsGraphicalDisplayLayout->setHorizontalSpacing(10);
        allLEDbarsGraphicalDisplayLayout->setSizeConstraint(QLayout::SetFixedSize);
        allLEDbarsGraphicalDisplay->setLayout(allLEDbarsGraphicalDisplayLayout);
        allLEDbarsGraphicalDisplay->setTitle("");
        allLEDbarsGraphicalDisplay->setObjectName("VOID");
    }
        break;
#endif
    default:
        break;
    }

    // complete graphical interface: time and scene, color and controls
    QVBoxLayout *graphicalDisplayLayout = new QVBoxLayout;
    if(givenType == _Beat_display || givenType == _Mixer_display || givenType == _FX_display || givenType == _Paths_display
#if defined(TVW) || defined(demo)
             || givenType == _MinimalIF_display
#endif
            ) {
        graphicalDisplayLayout->addWidget(timeSceneBeatDisplay, Qt::AlignLeft|Qt::AlignTop);
        graphicalDisplayLayout->addWidget(colorPalette, Qt::AlignLeft|Qt::AlignTop);
    }
    graphicalDisplayLayout->addWidget(allLEDbarsGraphicalDisplay, Qt::AlignLeft|Qt::AlignTop);
    graphicalDisplayLayout->setMargin(0);

    // graphicalDisplayLayout->addWidget(IFimageGraphicalDisplay);
    graphicalDisplayLayout->setSpacing(10);
    graphicalDisplayLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(graphicalDisplayLayout);
}

