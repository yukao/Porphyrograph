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

#include "pg-interface-atelPort-display.h"
#include "pg-interface-atelPort-window.h"
#include "pg-interface-atelPort-udp.h"

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
   int totalHeight = 60;

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
   sceneName = givensceneName;
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
SingleLabel::SingleLabel( inOutControllerData *givenData,
             QWidget *parent)
    : QLabel("", parent) {
    labelHighlightIn = "";
    valueHighlightIn = 0.f;
    hasValueHighlightIn = false;
    labelText = "";
    midiInControl = 0;
    highlight = false;

    setText(givenData->get_titleDisplay());
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

    setLabelText(givenData->get_titleDisplay());
    setMidiIn(givenData->get_controlMidiIn());
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


/////////////////////////////////////////////////////////////
/// \brief LabelMultipleRowDisplay::LabelMultipleRowDisplay
/// \param givenTitles
/// \param givenNbColsIni
/// \param givenNbColsEnd
/// \param tab_inControllers
/// \param parent
///
LabelMultipleRowDisplay::LabelMultipleRowDisplay(std::vector<std::vector<QString>> givenTitles,
                                             std::vector<std::vector<unsigned int>> givenNbColsIni,
                                             std::vector<std::vector<unsigned int>> givenNbColsEnd,
                                             std::vector<std::vector<inOutControllerData>> tab_inControllers,
                                             QWidget *parent)
    : QGroupBox("", parent) {
    nbRows = tab_inControllers.size();
   textColor = Qt::black;
   values = std::vector<std::vector<int>>(nbRows);
   labelRow = std::vector<std::vector<SingleLabel*>>(nbRows);
   if( givenTitles.size() != nbRows || givenNbColsIni.size() != nbRows || givenNbColsEnd.size() != nbRows  ||
           givenTitles[0].size() != givenNbColsIni[0].size() || givenTitles[0].size() != givenNbColsEnd[0].size() ||
           givenTitles[1].size() != givenNbColsIni[1].size() || givenTitles[1].size() != givenNbColsEnd[1].size() ) {
       printf("Incorrect sizes of graphical buttons\n");
       throw 0;
   }
   for(unsigned int indRow = 0 ; indRow < nbRows; indRow++) {
       values[indRow] = std::vector<int>(tab_inControllers[indRow].size());
       for(unsigned int indCol = 0 ; indCol < tab_inControllers[indRow].size(); indCol++) {
           labelRow[indRow].push_back(
                       new SingleLabel(&tab_inControllers[indRow][indCol]) );
           labelRow[indRow][indCol]->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
       }
   }

   // tries to show title
   setTitle("");
   setObjectName("VOID");

   // setTitle(tr("TEST"));
   // setObjectName(tr("TEST"));
   QVBoxLayout *LabelMultipleRowDisplayLayout = new QVBoxLayout;
   for(unsigned int indRow = 0 ; indRow < nbRows; indRow++) {
       QGroupBox *LabelSingleRowDisplay = new QGroupBox;
       QHBoxLayout *LabelSingleRowDisplayLayout = new QHBoxLayout;
       // row of groups of labels
       for(unsigned int indColGroup = 0 ; indColGroup < givenTitles[indRow].size(); indColGroup++) {
           QGroupBox *LabelSingleRowGroupDisplay = new QGroupBox;
           QHBoxLayout *LabelSingleRowGroupDisplayLayout = new QHBoxLayout;

           // row of labels
           for(unsigned int indCol = givenNbColsIni[indRow][indColGroup] ;
               indCol <= givenNbColsEnd[indRow][indColGroup]; indCol++) {
                if(indCol >= labelRow[indRow].size()) {
                     printf("Incorrect sizes of graphical buttons\n");
                     throw 0;
                }
                LabelSingleRowGroupDisplayLayout->addWidget(labelRow[indRow][indCol], Qt::AlignLeft|Qt::AlignTop);
           }
           LabelSingleRowGroupDisplay->setLayout(LabelSingleRowGroupDisplayLayout);
           LabelSingleRowGroupDisplay->setTitle(givenTitles[indRow][indColGroup]);
           LabelSingleRowGroupDisplay->setObjectName("EMPTY");

           LabelSingleRowDisplayLayout->addWidget(LabelSingleRowGroupDisplay, Qt::AlignLeft|Qt::AlignTop);
       }
       LabelSingleRowDisplay->setLayout(LabelSingleRowDisplayLayout);
       LabelSingleRowDisplay->setTitle("");
       LabelSingleRowDisplay->setObjectName("VOID");
       LabelSingleRowDisplay->setMaximumHeight(48);
       LabelMultipleRowDisplayLayout->addWidget(LabelSingleRowDisplay, Qt::AlignLeft|Qt::AlignTop);
   }
   setLayout(LabelMultipleRowDisplayLayout);
   setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
}

void LabelMultipleRowDisplay::setValue(int indRow, int indCol, int value) {
   values[indRow][indCol] = value;
   QWidget::update();
}

/////////////////////////////////////////////////////////////
/// \brief LabelAlignedMultipleRowDisplay::LabelAlignedMultipleRowDisplay
/// \param givenTitles
/// \param givenNbColsIni
/// \param givenNbColsEnd
/// \param tab_inControllers
/// \param parent
///
LabelAlignedMultipleRowDisplay::LabelAlignedMultipleRowDisplay(
        std::vector<std::vector<QString>> givenTitles,
        std::vector<unsigned int> givenNbColsIni,
        std::vector<unsigned int> givenNbColsEnd,
        std::vector<std::vector<inOutControllerData>> tab_inControllers,
        QWidget *parent)
    : QGroupBox("", parent) {
   nbRows = tab_inControllers.size();
   textColor = Qt::black;
   values = std::vector<std::vector<int>>(nbRows);
   labelRow = std::vector<std::vector<SingleLabel*>>(nbRows);
   if( givenTitles.size() != nbRows ||
           givenTitles[0].size() != givenNbColsIni.size() || givenTitles[0].size() != givenNbColsEnd.size() ||
           givenTitles[1].size() != givenNbColsIni.size() || givenTitles[1].size() != givenNbColsEnd.size() ) {
       printf("Incorrect sizes of graphical buttons\n");
       throw 0;
   }
   for(unsigned int indRow = 0 ; indRow < nbRows; indRow++) {
       values[indRow] = std::vector<int>(tab_inControllers[indRow].size());
       for(unsigned int indCol = 0 ; indCol < tab_inControllers[indRow].size(); indCol++) {
           labelRow[indRow].push_back(
                       new SingleLabel(&tab_inControllers[indRow][indCol]) );
           labelRow[indRow][indCol]->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
       }
   }

   // tries to show title
   setTitle("");
   setObjectName("VOID");

   // setTitle(tr("TEST"));
   // setObjectName(tr("TEST"));
   QHBoxLayout *LabelAlignedMultipleRowDisplayLayout = new QHBoxLayout;
   // row of groups of labels
   for(unsigned int indColGroup = 0 ; indColGroup < givenTitles[0].size(); indColGroup++) {
       // col of labels
       QGroupBox *LabelSingleColGroupDisplay = new QGroupBox;
       QVBoxLayout *LabelSingleColGroupDisplayLayout = new QVBoxLayout;
       for(unsigned int indRow = 0 ; indRow < nbRows; indRow++) {

           QGroupBox *LabelSingleRowGroupDisplay = new QGroupBox;
           QHBoxLayout *LabelSingleRowGroupDisplayLayout = new QHBoxLayout;
           for(unsigned int indCol = givenNbColsIni[indColGroup] ;
                            indCol <= givenNbColsEnd[indColGroup];
                            indCol++) {
                if(indCol >= labelRow[indRow].size()) {
                     printf("Incorrect sizes of graphical buttons\n");
                     throw 0;
                }
                LabelSingleRowGroupDisplayLayout->addWidget(labelRow[indRow][indCol], Qt::AlignLeft|Qt::AlignTop);
            }
            LabelSingleRowGroupDisplay->setLayout(LabelSingleRowGroupDisplayLayout);
            LabelSingleRowGroupDisplay->setTitle(givenTitles[indRow][indColGroup]);
            LabelSingleRowGroupDisplay->setObjectName("EMPTY");

            LabelSingleColGroupDisplayLayout->addWidget(LabelSingleRowGroupDisplay, Qt::AlignLeft|Qt::AlignTop);
       }
       LabelSingleColGroupDisplay->setLayout(LabelSingleColGroupDisplayLayout);
       LabelSingleColGroupDisplay->setTitle("");
       LabelSingleColGroupDisplay->setObjectName("VOID");
       LabelSingleColGroupDisplay->setMaximumHeight(110);

       LabelAlignedMultipleRowDisplayLayout->addWidget(LabelSingleColGroupDisplay, Qt::AlignLeft|Qt::AlignTop);
   }
   setLayout(LabelAlignedMultipleRowDisplayLayout);
   setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed));
}

void LabelAlignedMultipleRowDisplay::setValue(int indRow, int indCol, int value) {
   values[indRow][indCol] = value;
   QWidget::update();
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

void Box::setLabelHighlightIn(QString givenLabel) {
    labelHighlightIn = givenLabel;
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
                         std::vector<std::vector<inOutControllerData>> tab_inControllers,
                         QWidget *parent)
    : QGroupBox("", parent) {
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
           if(indRow >= 1 && indCol >= 1 && tab_inControllers[indRow - 1][indCol - 1].get_labelObject() == "VOID") {
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
               QStringList list = tab_inControllers[indRow - 1][indCol - 1].get_labelHighlightIn().split(QRegExp("\\s+"), QString::SkipEmptyParts);
               if(list.size() >= 1) {
                   boxes[indRow * nbCols + indCol].setLabelHighlightIn(list.at(0));
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
               int curVal
                    = boxes[indRow * nbCols + indCol].getValue();
               QRect *curRect = boxes[indRow * nbCols + indCol].getRect();
               if(boxes[indRow * nbCols + indCol].getInverse()) {
                   painter.fillRect(*curRect,
                                    Qt::cyan);
               }
               else if(curVal > 0) {
                    painter.fillRect(*curRect,
                                     graphs_mainSlidersData[indRow - 1][indCol - 1].bgOnColor);
               }
               else {
                    painter.fillRect(*curRect,
                                     graphs_mainSlidersData[indRow - 1][indCol - 1].bgOffColor);
               }
               if(boxes[indRow * nbCols + indCol].getHighlight()) {
                   QPen pen(Qt::red);
                   pen.setWidth(2);
                   painter.setPen(pen);
                   painter.drawRect(*curRect);
               }

               painter.setPen(textColor);
               painter.setFont(QFont("Arial", graphs_mainSlidersData[indRow - 1][indCol - 1].fontSize, QFont::Bold));
               painter.drawText(*curRect,
                                Qt::AlignCenter,
                                QString::number(curVal));
           }
           else {
               painter.setPen(Qt::white);
               painter.setFont(QFont("Arial", graphs_mainSlidersData[0][0].fontSize - 2, QFont::Bold));
               if(indRow == 0 && indCol >= 1) {
                   painter.drawText(*(boxes[indRow * nbCols + indCol].getRect()),
                           Qt::AlignCenter,
                           graphs_mainSlidersData[0][indCol-1].get_titleDisplay());
               }
               else if(indRow >= 1 && indCol == 0) {
                   painter.drawText(*(boxes[indRow * nbCols + indCol].getRect()),
                           Qt::AlignCenter,
                           graphs_mainSlidersData[indRow - 1][0].get_titleDisplay());
               }
           }
       }
   }
   QWidget::paintEvent(event);
   painter.end();
}

/////////////////////////////////////////////////////////////
LinkingArrow::LinkingArrow(std::vector<QPoint*> points,
                           int width,
                           float rBorder, float gBorder, float bBorder,
                           QObject* parent)
    : QObject(parent) {
    if(points.size() < 2) {
        return;
    }
    myPath.moveTo(*(points[0]));
    for(unsigned int ind = 1; ind < points.size() ; ind++ ){
        myPath.lineTo( *(points[ind]) );
    }
    QPoint endPoint = *(points[points.size() - 1]);
    QVector2D colin = QVector2D(endPoint - *(points[points.size() - 2]));
    colin.normalize();
    QVector2D orthog = QVector2D(colin.y(),-colin.x());
    arrowHead << endPoint << endPoint - colin.toPoint() + 0.5 * orthog.toPoint()
              << endPoint - colin.toPoint() - 0.5 * orthog.toPoint();
    myPath.addPolygon(arrowHead);
    myPath.closeSubpath();

    borderColor.setRgbF(rBorder, gBorder, bBorder);
    borderWidth = width;
}


void LinkingArrow::inverse() {
    borderColor.setRgbF(1.f - borderColor.redF(),
                        1.f - borderColor.greenF(),
                        1.f - borderColor.blueF());
}

QPainterPath *LinkingArrow::getPath() {
    return &myPath;
}

QColor *LinkingArrow::getBorderColor() {
    return &borderColor;
}


/////////////////////////////////////////////////////////////
/// \brief ArrowConnections::ArrowConnections
/// \param givenTitle
/// \param givenNbRows
/// \param givenNbCols
/// \param tab_inControllers
/// \param parent
///
ArrowConnections::ArrowConnections(QString givenTitle,
                                   int givenWidth, int givenHeight,
                                   QWidget *parent)
    : QGroupBox("", parent) {
   for(unsigned int ind = 0 ; ind < 12 ; ind++) {
       values.push_back(0.f);
       std::vector<QPoint*> points;
       points.push_back(new QPoint(0,0));
       points.push_back(new QPoint(0,1));
       points.push_back(new QPoint(5,1));
       points.push_back(new QPoint(5,0));
       arrows.push_back(new LinkingArrow(points,
                                         2,
                                         1.f, 1.f, 1.f));
   }

   // tries to show title
   title = givenTitle;
   setTitle(givenTitle);
   setObjectName("FLASH");

   // dimensions
   totalWidth = givenWidth;
   totalHeight = givenHeight;

   // setTitle(tr("TEST"));
   // setObjectName(tr("TEST"));
   QVBoxLayout *arrowConnectionsDisplayLayout = new QVBoxLayout;
   // Empty slider area
   QSpacerItem *emptyControl = new QSpacerItem(totalWidth , totalHeight);
   arrowConnectionsDisplayLayout->addItem(emptyControl);
   setLayout(arrowConnectionsDisplayLayout);
   resize(totalWidth , totalHeight);
   setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
}

void ArrowConnections::setValue(unsigned int indArrow, int value) {
    if( indArrow < values.size()) {
        values[indArrow] = value;
    }
    QWidget::update();
}
void ArrowConnections::paintEvent(QPaintEvent *event)
{
   QPainter painter;
   QGroupBox::paintEvent(event);
   painter.begin(this);
   QPen pen;
   painter.begin(this);
   pen.setWidth(2);
   painter.setPen(pen);
   for(unsigned int ind = 0 ; ind < arrows.size() ; ind++) {
      pen.setColor(*(arrows[ind]->getBorderColor()));
      painter.drawPath(*(arrows[ind]->getPath()));
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
/// \param givenbarOffColor
/// \param givenbarOnColor
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
               bool upperLabel,
               bool doubleLayerLabels,
               QString givenTitle,
               QString givenLabelHighlightIn,
               float givenAmplitude,
               int givenFontSize,
               QColor *givenbgOffColor, QColor *givenbgOnColor,
               QColor *givenbarOffColor, QColor *givenbarOnColor,
               float givenrBorder, float givengBorder, float givenbBorder,
               float givenrText, float givengText, float givenbText,
               QObject* parent)
                : QObject(parent) {
    title = givenTitle;
    QStringList list = givenLabelHighlightIn.split(QRegExp("\\s+"), QString::SkipEmptyParts);
    labelHighlightIn = "";
    valueHighlightIn = 0.f;
    hasValueHighlightIn = false;
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
    bgOffColor = *givenbgOffColor;
    bgOnColor = *givenbgOnColor;
    barOffColor = *givenbarOffColor;
    barOnColor = *givenbarOnColor;
    barColor = barOffColor;
    bgColor = bgOffColor;
    borderColor.setRgbF(givenrBorder, givengBorder, givenbBorder);
    textColor.setRgbF(givenrText, givengText, givenbText);
    highlight = false;
    inversed = false;
    amplitude = givenAmplitude;
    fontSize = givenFontSize;
    parentLEDBarRow = givenParentLEDBarRow;
}
void LEDBar::setValue(float newValue) {
    float ratio = (newValue / amplitude);
    rectBar.setTop(int(YoffSet + height - ratio * height));
    if(newValue != 0) {
        bgColor = bgOnColor;
    }
    else {
        bgColor = bgOffColor;
    }
//    barColor.setRgbF((1.f - ratio) * barOffColor.redF() + ratio * barOnColor.redF(),
//                     (1.f - ratio) * barOffColor.greenF() + ratio * barOnColor.greenF(),
//                     (1.f - ratio) * barOffColor.blueF() + ratio * barOnColor.blueF());
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
                     std::vector<inOutControllerData> tab_inControllers,
                     bool doubleLayerLabels,
                     float givenrBorder, float givengBorder, float givenbBorder,
                     float givenrrText, float givenrgText, float givenrbText,
                     QWidget *parent)
    : QGroupBox("", parent) {
    title = givenTitle;
    nbLEDBars = tab_inControllers.size();
    int xPos = 0;
    int maxLabelHeight = 0;
    for(unsigned int indLEDBar = 0 ; indLEDBar < nbLEDBars; indLEDBar++) {
        int intervalToNext = tab_inControllers[indLEDBar].intervalle;
        int labelHeight = int(tab_inControllers[indLEDBar].fontSize * 1.3f);
        if(labelHeight > maxLabelHeight)  {
            maxLabelHeight = labelHeight;
        }
        LEDBars.push_back( new LEDBar(  xPos + 10,
                                        20,
                                        tab_inControllers[indLEDBar].get_size(),
                                        givenHeight,
                                        intervalToNext,
                                        labelHeight,
                                        this,
                                        (doubleLayerLabels && (indLEDBar %2 == 0)),
                                        doubleLayerLabels,
                                        tab_inControllers[indLEDBar].get_titleDisplay(),
                                        tab_inControllers[indLEDBar].get_labelHighlightIn(),
                                        tab_inControllers[indLEDBar].get_amplitude(),
                                        tab_inControllers[indLEDBar].fontSize,
                                        &tab_inControllers[indLEDBar].bgOffColor,
                                        &tab_inControllers[indLEDBar].bgOnColor,
                                        &tab_inControllers[indLEDBar].barOffColor,
                                        &tab_inControllers[indLEDBar].barOnColor,
                                        givenrBorder, givengBorder, givenbBorder,
                                        givenrrText, givenrgText, givenrbText) );
//        if(indLEDBar == nbLEDBars - 1) {
//            xPos += tab_inControllers[indLEDBar].get_size();
//        }
//        else {
            xPos += tab_inControllers[indLEDBar].get_size() + intervalToNext;
//        }
    }
    if(doubleLayerLabels) {
        maxLabelHeight *= 2;
    }

    // tries to show title
    setTitle(title);
    setObjectName(tab_inControllers[0].get_labelObject());

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
        printf("Number of titles %d does not correspond to the number of LEDBars %d\n",
               int(newValues.size()),nbLEDBars);
        return;
    }
    for(unsigned int indLEDBar = 0 ; indLEDBar < nbLEDBars; indLEDBar++) {
        LEDBars[indLEDBar]->setValue(newValues[indLEDBar]);
    }
    QWidget::update();
}

void LEDBarRow::setValue(unsigned int index, float value) {
    if(index >= nbLEDBars) {
        printf("Value index %d does not correspond to the number of LEDBars %d\n",
               index, nbLEDBars);
        return;
    }
    LEDBars[index]->setValue(value);
    QWidget::update();
}
void LEDBarRow::setHighlight(unsigned int index, bool onOff) {
    if(index >= nbLEDBars) {
        printf("Value index %d does not correspond to the number of LEDBars %d\n",
               index, nbLEDBars);
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
    for(unsigned int indLEDBar = 0 ; indLEDBar < nbLEDBars; indLEDBar++) {
        if(LEDBars[indLEDBar]->getHighlight()) {
            if(LEDBars[indLEDBar]->getInverse()) {
                painter.fillRect(LEDBars[indLEDBar]->rectBg,
                                 QBrush(Qt::magenta));
                painter.fillRect(LEDBars[indLEDBar]->rectBar,
                                 QBrush(Qt::cyan));
            }
            else {
                painter.fillRect(LEDBars[indLEDBar]->rectBg ,
                                 QBrush(LEDBars[indLEDBar]->bgColor));
                painter.fillRect(LEDBars[indLEDBar]->rectBar,
                                 QBrush(LEDBars[indLEDBar]->barColor));
            }
        }
        else {
            if(LEDBars[indLEDBar]->getInverse()) {
                painter.fillRect(LEDBars[indLEDBar]->rectBg,
                                 QBrush(Qt::cyan));
                painter.fillRect(LEDBars[indLEDBar]->rectBar,
                                 QBrush(Qt::magenta));
            }
            else {
                painter.fillRect(LEDBars[indLEDBar]->rectBg ,
                                 QBrush(LEDBars[indLEDBar]->bgColor));
                painter.fillRect(LEDBars[indLEDBar]->rectBar,
                                 QBrush(LEDBars[indLEDBar]->barColor));
            }
        }


        if(LEDBars[indLEDBar]->getHighlight()) {
            QPen pen(Qt::red);
            pen.setWidth(2);
            painter.setPen(pen);
            painter.drawRect(LEDBars[indLEDBar]->rectBg);
        }
//        painter.setPen(QPen(LEDBars[indLEDBar]->borderColor));
//        painter.drawRect(LEDBars[indLEDBar]->rectBg);

        painter.setPen(QPen(LEDBars[indLEDBar]->textColor));
        painter.setFont(QFont("Arial", LEDBars[indLEDBar]->fontSize, QFont::Bold));
        painter.drawText(LEDBars[indLEDBar]->rectLabel,
                         Qt::AlignLeft | Qt::AlignBottom,
                         LEDBars[indLEDBar]->title);
    }
    painter.end();
}

LEDBar *LEDBarRow::getLEDBar(unsigned int indLEDBar) {
    if( indLEDBar < LEDBars.size()) {
        return LEDBars[indLEDBar];
    }
    std::cout << "Out of range LED Bar " << indLEDBar << " " << LEDBars.size() << " " << title.toStdString().c_str() << std::endl;
    return NULL;
}


/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
// GRAPHS TAB
void GraphicalDisplays::create_graphsTabDisplays(void) {
    // GRAPHS LED BARS
    graphicalLEDBarRows = std::vector<LEDBarRow*>(_MaxGraphsInOutDataRows);
    for(unsigned int ind = 0 ; ind < _MaxGraphsInOutDataRows ; ind++ ) {
        graphicalLEDBarRows[ind] = NULL;
    }
    graphicalLEDBarRows[_PixelInOutData]
            = new LEDBarRow("PIX", 50, graphs_mainSlidersData[_PixelInOutData], true);
    graphicalLEDBarRows[_PulseCAFlashInOutData]
            = new LEDBarRow("PULSE", 50, graphs_mainSlidersData[_PulseCAFlashInOutData], true);

    graphicalLEDBarRows[_PulsePartInOutData]
            = new LEDBarRow("PULSE PART", 50, graphs_mainSlidersData[_PulsePartInOutData], true);

    graphicalLEDBarRows[_CAParamsInOutData]
            = new LEDBarRow("CA PARAMS", 50, graphs_mainSlidersData[_CAParamsInOutData], true);
    graphicalLEDBarRows[_RadiusColorInOutData]
            = new LEDBarRow("RADIUS-COLOR", 50, graphs_mainSlidersData[_RadiusColorInOutData], true);
    graphicalLEDBarRows[_BeatInOutData]
            = new LEDBarRow("BEAT", 50, graphs_mainSlidersData[_BeatInOutData], true);
    graphicalLEDBarRows[_PixRadiusInOutData]
            = new LEDBarRow("PIXEL", 50, graphs_mainSlidersData[_PixRadiusInOutData], true);
    graphicalLEDBarRows[_GraphsSoundBandsInOutData]
            = new LEDBarRow("BANDS", 50, graphs_mainSlidersData[_GraphsSoundBandsInOutData]);
    // 1st & 2nd row graphs buttons
    graphsButtonsLabels
            = new LabelAlignedMultipleRowDisplay({{"PEN", "PEN PALETTE", "PIXEL"},{"REPOP", "PEN BRUSH", "FRZE"}},
    {0,1,7}, {0,6,7}, graphs_mainButtonsData);
    // GRID of inter-layer flashes
    graphsFlashGrid = new BoxGridDisplay("FLASH", 4, 6, graphs_mainSlidersData);
}

/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
// TRACKS TAB
void GraphicalDisplays::create_tracksTabDisplays(void) {
    // TRACKS LED BARS
    graphicalLEDBarRows = std::vector<LEDBarRow*>(_MaxTracksInOutDataRows);
    for(unsigned int ind = 0 ; ind < _MaxTracksInOutDataRows ; ind++ ) {
        graphicalLEDBarRows[ind] = NULL;
    }
    graphicalLEDBarRows[_MixingInOutData]
            = new LEDBarRow("MIXING W", 70, tracks_mainSlidersData[_MixingInOutData]);
    graphicalLEDBarRows[_MediaWeightInOutData]
            = new LEDBarRow("MEDIA WGHT", 70, tracks_mainSlidersData[_MediaWeightInOutData]);
    graphicalLEDBarRows[_TracksSoundBandsInOutData]
            = new LEDBarRow("BANDS", 70, tracks_mainSlidersData[_TracksSoundBandsInOutData]);
    graphicalLEDBarRows[_MasterInOutData]
            = new LEDBarRow("MASTER W", 30, tracks_mainSlidersData[_MasterInOutData]);
    graphicalLEDBarRows[_SoundMinInOutData]
            = new LEDBarRow("XX", 30, tracks_mainSlidersData[_SoundMinInOutData]);
    graphicalLEDBarRows[_RepopInOutData]
            = new LEDBarRow("REPOP", 30, tracks_mainSlidersData[_RepopInOutData]);
    graphicalLEDBarRows[_CamThresholdInOutData]
            = new LEDBarRow("THR", 30, tracks_mainSlidersData[_CamThresholdInOutData]);
    graphicalLEDBarRows[_MediaFreqInOutData]
            = new LEDBarRow("FREQ", 30, tracks_mainSlidersData[_MediaFreqInOutData]);
    graphicalLEDBarRows[_MediaSobelInOutData]
            = new LEDBarRow("SOBEL", 30, tracks_mainSlidersData[_MediaSobelInOutData]);
    graphicalLEDBarRows[_DecayInOutData]
            = new LEDBarRow("DK", 100, tracks_mainSlidersData[_DecayInOutData],true);
    graphicalLEDBarRows[_SoundVolInOutData]
            = new LEDBarRow("BEAT SND-AUT", 100, tracks_mainSlidersData[_SoundVolInOutData],true);
    graphicalLEDBarRows[_EchoInOutData]
            = new LEDBarRow("SND THRS", 100, tracks_mainSlidersData[_EchoInOutData],true);
    graphicalLEDBarRows[_MasterBlendInOutData]
            = new LEDBarRow("MAS", 100, tracks_mainSlidersData[_MasterBlendInOutData],true);
    // 1st & 2nd row tracks buttons
    tracksButtonsLabels =
            new LabelAlignedMultipleRowDisplay({{"LAUNCH", "RESET"},{"DECAY SIGN", "MEDIA+"}},
    {0,5}, {4,7}, tracks_mainButtonsData);
}
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
// PATHS TAB
void GraphicalDisplays::create_pathsTabDisplays(void) {
    // TRACKS LED BARS
    graphicalLEDBarRows = std::vector<LEDBarRow*>(_MaxPathsInOutDataRows);
    for(unsigned int ind = 0 ; ind < _MaxPathsInOutDataRows ; ind++ ) {
        graphicalLEDBarRows[ind] = NULL;
    }
    graphicalLEDBarRows[_ParticleRadiusInOutData]
            = new LEDBarRow("PART RAD/SHAPE", 50, paths_mainSlidersData[_ParticleRadiusInOutData],true);
    graphicalLEDBarRows[_ParticleFluidityInOutData]
            = new LEDBarRow("PART FLUID", 50, paths_mainSlidersData[_ParticleFluidityInOutData], true);
    graphicalLEDBarRows[_ParticleNoiseInOutData]
            = new LEDBarRow("PART NOISE", 50, paths_mainSlidersData[_ParticleNoiseInOutData], true);
    // 1st & 2nd row paths buttons
    pathsButtonsLabels =
            new LabelMultipleRowDisplay({{"PART MOVE", "PART STROKE"},{"PART EXIT", "PART COLOR"},
                                         {"PART INIT"},{"PART PATH REPOP"},{"PART FOLLOW PATH"},{"PART REPULSE PATH"},{"PATH RECORD"},{"PATH REPLAY"},
                                         {"SET-UP", "CA TYPE"},{"CONTROLS"}},
    {{0,4},{0,4},{0},{0},{0},{0},{0},{0},{0,3},{0}},
    {{3,7},{3,7},{7},{7},{7},{7},{7},{7},{2,7},{7}},
                                        paths_mainButtonsData);
}

/////////////////////////////////////////////////////////////
/// \brief GraphicalDisplays::GraphicalDisplays
/// \param givenTitle
/// \param IFimageFileName
/// \param givenType
/// \param parent
///

GraphicalDisplays::GraphicalDisplays(QString givenTitle,
                                     QString IFimageFileName,
                                     DisplayType givenType,
                                     QWidget *parent)
    : QWidget(parent) {
    title = givenTitle;

    // time and scene display
    timeSceneBeatDisplay
            = new TimeSceneBeatDisplay("SCENARIO",
    {Qt::red,QColor("orange"),Qt::green,QColor("violet")},
                                       100,
                                       150,
                                       50,
                                       40);

    // 2 color palettes
    colorPalette = new ColorDisplay({"PEN", "REPO"}, 2, 20);

    /////////////////////////////////////////////////////////
    QGroupBox *allLEDbarsGraphicalDisplay = new QGroupBox;
    QGridLayout *allLEDbarsGraphicalDisplayLayout = new QGridLayout;
    switch(givenType) {
    case _Graphs_display: {
        /////////////////////////////////////////////////////////
        // GRAPHS TAB

        // create displays
        create_graphsTabDisplays();

        // organize them in a grid
        // grouping LED bar rows 2nd row
        QGroupBox *graphsRow1LEDbarsGraphicalDisplay = new QGroupBox;
        QHBoxLayout *graphsRow1LEDbarsGraphicalDisplayLayout = new QHBoxLayout;
        graphsRow1LEDbarsGraphicalDisplayLayout->addWidget(graphicalLEDBarRows[_PixelInOutData], Qt::AlignLeft|Qt::AlignTop);
        graphsRow1LEDbarsGraphicalDisplayLayout->addWidget(graphicalLEDBarRows[_PulseCAFlashInOutData], Qt::AlignLeft|Qt::AlignTop);
        graphsRow1LEDbarsGraphicalDisplayLayout->setSizeConstraint(QLayout::SetFixedSize);
        graphsRow1LEDbarsGraphicalDisplayLayout->setSpacing(3);
        graphsRow1LEDbarsGraphicalDisplay->setLayout(graphsRow1LEDbarsGraphicalDisplayLayout);
        graphsRow1LEDbarsGraphicalDisplay->setTitle("");
        graphsRow1LEDbarsGraphicalDisplay->setObjectName("VOID");

        allLEDbarsGraphicalDisplayLayout->addWidget(graphsFlashGrid, 0, 0, 3, 1, Qt::AlignLeft|Qt::AlignTop);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphsRow1LEDbarsGraphicalDisplay, 0, 1, Qt::AlignLeft|Qt::AlignTop);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLEDBarRows[_GraphsSoundBandsInOutData], 0, 2, Qt::AlignLeft|Qt::AlignTop);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLEDBarRows[_PulsePartInOutData], 1, 1, Qt::AlignLeft|Qt::AlignTop);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLEDBarRows[_CAParamsInOutData], 2, 1, Qt::AlignLeft|Qt::AlignTop);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphsButtonsLabels, 3, 0, 1, 3, Qt::AlignLeft|Qt::AlignTop);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLEDBarRows[_RadiusColorInOutData], 4, 0, Qt::AlignLeft|Qt::AlignTop);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLEDBarRows[_BeatInOutData], 4, 1, Qt::AlignLeft|Qt::AlignTop);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLEDBarRows[_PixRadiusInOutData], 4, 2, Qt::AlignLeft|Qt::AlignTop);
        allLEDbarsGraphicalDisplayLayout->setVerticalSpacing(10);
        allLEDbarsGraphicalDisplayLayout->setHorizontalSpacing(10);
        allLEDbarsGraphicalDisplayLayout->setSizeConstraint(QLayout::SetFixedSize);
        allLEDbarsGraphicalDisplay->setLayout(allLEDbarsGraphicalDisplayLayout);
        allLEDbarsGraphicalDisplay->setTitle("");
        allLEDbarsGraphicalDisplay->setObjectName("VOID");
    }
        break;
    case _Tracks_display: {
        /////////////////////////////////////////////////////////
        // TRACKS TAB

        // create displays
        create_tracksTabDisplays();

        // organize them in a grid
        // grouping LED bar rows 2nd row
        QGroupBox *tracksRow2LEDbarsGraphicalDisplay = new QGroupBox;
        QHBoxLayout *tracksRow2LEDbarsGraphicalDisplayLayout = new QHBoxLayout;
        tracksRow2LEDbarsGraphicalDisplayLayout->addWidget(graphicalLEDBarRows[_CamThresholdInOutData], Qt::AlignLeft|Qt::AlignTop);
        tracksRow2LEDbarsGraphicalDisplayLayout->addWidget(graphicalLEDBarRows[_MediaFreqInOutData], Qt::AlignLeft|Qt::AlignTop);
        tracksRow2LEDbarsGraphicalDisplayLayout->setSizeConstraint(QLayout::SetFixedSize);
        tracksRow2LEDbarsGraphicalDisplayLayout->setSpacing(3);
        tracksRow2LEDbarsGraphicalDisplay->setLayout(tracksRow2LEDbarsGraphicalDisplayLayout);
        tracksRow2LEDbarsGraphicalDisplay->setTitle("");
        tracksRow2LEDbarsGraphicalDisplay->setObjectName("VOID");

        // grouping LED bar rows 3rd row
        QGroupBox *tracksRow3LEDbarsGraphicalDisplay = new QGroupBox;
        QHBoxLayout *tracksRow3LEDbarsGraphicalDisplayLayout = new QHBoxLayout;
        tracksRow3LEDbarsGraphicalDisplayLayout->addWidget(graphicalLEDBarRows[_SoundMinInOutData], Qt::AlignLeft|Qt::AlignTop);
        tracksRow3LEDbarsGraphicalDisplayLayout->addWidget(graphicalLEDBarRows[_MediaSobelInOutData], Qt::AlignLeft|Qt::AlignTop);
        tracksRow3LEDbarsGraphicalDisplayLayout->setSizeConstraint(QLayout::SetFixedSize);
        tracksRow3LEDbarsGraphicalDisplayLayout->setSpacing(3);
        tracksRow3LEDbarsGraphicalDisplay->setLayout(tracksRow3LEDbarsGraphicalDisplayLayout);
        tracksRow3LEDbarsGraphicalDisplay->setTitle("");
        tracksRow3LEDbarsGraphicalDisplay->setObjectName("VOID");

        // grouping LED bar rows 4th row
        QGroupBox *tracksRow4LEDbarsGraphicalDisplay = new QGroupBox;
        QHBoxLayout *tracksRow4LEDbarsGraphicalDisplayLayout = new QHBoxLayout;
        tracksRow4LEDbarsGraphicalDisplayLayout->addWidget(graphicalLEDBarRows[_DecayInOutData], Qt::AlignLeft|Qt::AlignTop);
        tracksRow4LEDbarsGraphicalDisplayLayout->addWidget(graphicalLEDBarRows[_SoundVolInOutData], Qt::AlignLeft|Qt::AlignTop);
        tracksRow4LEDbarsGraphicalDisplayLayout->setSizeConstraint(QLayout::SetFixedSize);
        tracksRow4LEDbarsGraphicalDisplayLayout->setSpacing(3);
        tracksRow4LEDbarsGraphicalDisplay->setLayout(tracksRow4LEDbarsGraphicalDisplayLayout);
        tracksRow4LEDbarsGraphicalDisplay->setTitle("");
        tracksRow4LEDbarsGraphicalDisplay->setObjectName("VOID");

        // grouping LED bar1-2-3 rows 1-2-3 cols
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLEDBarRows[_MixingInOutData], 0, 0, Qt::AlignLeft|Qt::AlignTop);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLEDBarRows[_MediaWeightInOutData], 0, 1, Qt::AlignLeft|Qt::AlignTop);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLEDBarRows[_TracksSoundBandsInOutData], 0, 2, Qt::AlignLeft|Qt::AlignTop);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLEDBarRows[_MasterInOutData], 1, 0, Qt::AlignLeft|Qt::AlignTop);
        allLEDbarsGraphicalDisplayLayout->addWidget(tracksRow2LEDbarsGraphicalDisplay, 1, 1, Qt::AlignLeft|Qt::AlignTop);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLEDBarRows[_RepopInOutData], 2, 0, Qt::AlignLeft|Qt::AlignTop);
        allLEDbarsGraphicalDisplayLayout->addWidget(tracksRow3LEDbarsGraphicalDisplay, 2, 1, Qt::AlignLeft|Qt::AlignTop);
        allLEDbarsGraphicalDisplayLayout->addWidget(tracksButtonsLabels, 3, 0, 1, 3, Qt::AlignLeft|Qt::AlignTop);
        allLEDbarsGraphicalDisplayLayout->addWidget(tracksRow4LEDbarsGraphicalDisplay, 4, 0, Qt::AlignLeft|Qt::AlignTop);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLEDBarRows[_EchoInOutData], 4, 1, Qt::AlignLeft|Qt::AlignTop);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLEDBarRows[_MasterBlendInOutData], 4, 2, Qt::AlignLeft|Qt::AlignTop);
        allLEDbarsGraphicalDisplayLayout->setVerticalSpacing(10);
        allLEDbarsGraphicalDisplayLayout->setHorizontalSpacing(10);
        allLEDbarsGraphicalDisplayLayout->setSizeConstraint(QLayout::SetFixedSize);
        allLEDbarsGraphicalDisplay->setLayout(allLEDbarsGraphicalDisplayLayout);
        allLEDbarsGraphicalDisplay->setTitle("");
        allLEDbarsGraphicalDisplay->setObjectName("VOID");
    }
        break;
    case _Paths_display:
        /////////////////////////////////////////////////////////
        // PATHS TAB

        // create displays
        create_pathsTabDisplays();

        // organize them in a grid
        allLEDbarsGraphicalDisplayLayout->addWidget(pathsButtonsLabels, 0, 0, 1, 3, Qt::AlignLeft|Qt::AlignTop);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLEDBarRows[_ParticleRadiusInOutData], 1, 0, Qt::AlignLeft|Qt::AlignTop);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLEDBarRows[_ParticleFluidityInOutData], 1, 1, Qt::AlignLeft|Qt::AlignTop);
        allLEDbarsGraphicalDisplayLayout->addWidget(graphicalLEDBarRows[_ParticleNoiseInOutData], 1, 2, Qt::AlignLeft|Qt::AlignTop);
        allLEDbarsGraphicalDisplayLayout->setVerticalSpacing(10);
        allLEDbarsGraphicalDisplayLayout->setHorizontalSpacing(10);
        allLEDbarsGraphicalDisplayLayout->setSizeConstraint(QLayout::SetFixedSize);
        allLEDbarsGraphicalDisplay->setLayout(allLEDbarsGraphicalDisplayLayout);
        allLEDbarsGraphicalDisplay->setTitle("");
        allLEDbarsGraphicalDisplay->setObjectName("VOID");
        break;
    default:
        break;
    }
    /////////////////////////////////////////////////////////////////////
    // IMAGE OF THE INTERFACE - NOT USED CURRENTLY
    // display image
    QGroupBox *IFimageGraphicalDisplay = new QGroupBox;
    QHBoxLayout *IFimageGraphicalDisplayLayout = new QHBoxLayout;
    IFimage_graphs = new QPixmap(IFimageFileName);
    IFimageLabel = new QLabel();
    IFimageLabel->setPixmap(*IFimage_graphs);
    IFimageGraphicalDisplayLayout->addWidget(IFimageLabel);
    IFimageGraphicalDisplay->setLayout(IFimageGraphicalDisplayLayout);
    IFimageGraphicalDisplay->setTitle("");
    IFimageGraphicalDisplay->setObjectName("VOID");
    IFimageGraphicalDisplay->resize(360 , 253);
    IFimageGraphicalDisplay->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));

    //    QWidget *IFimageGraphicalDisplay = new QWidget(this);
    //    IFimageGraphicalDisplay->setStyleSheet("background-image: url(textures/graphsIF-med.jpg)");
    //    IFimageGraphicalDisplay->setGeometry(QRect(0,0,360 , 253));
    //    IFimageGraphicalDisplay->resize(360 , 253);
    // IFimageGraphicalDisplay->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));

    // complete graphical interface: time and scene, color and controls
    QVBoxLayout *graphicalDisplayLayout = new QVBoxLayout;
    if(givenType == _Graphs_display || givenType == _Tracks_display) {
        graphicalDisplayLayout->addWidget(timeSceneBeatDisplay, Qt::AlignLeft|Qt::AlignTop);
        graphicalDisplayLayout->addWidget(colorPalette);
    }
    graphicalDisplayLayout->addWidget(allLEDbarsGraphicalDisplay, Qt::AlignLeft|Qt::AlignTop);

    // graphicalDisplayLayout->addWidget(IFimageGraphicalDisplay);
    graphicalDisplayLayout->setSpacing(10);
    graphicalDisplayLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(graphicalDisplayLayout);
}

