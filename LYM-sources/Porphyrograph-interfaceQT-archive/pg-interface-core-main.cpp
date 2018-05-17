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

#include <QApplication>
#include <QThread>
#include <QFile>
#include <QKeyEvent>
#include <QMainWindow>
#include <QStatusBar>

#include "RtMidi.h"

#include "pg-interface-core-comm.h"
#include "pg-interface-core-control.h"
#include "pg-interface-core-window.h"
#include "pg-interface-core-main.h"

#ifdef _WIN32
#include <windows.h>
#endif

MainWindow *pg_window = NULL;
QApplication *pg_app = NULL;

int main(int argc, char *argv[])
{
    // trick to print on the console thanks to printf
#ifdef _WIN32
    if (AttachConsole(ATTACH_PARENT_PROCESS)) {
        // if (AttachConsole(ATTACH_PARENT_PROCESS) || AllocConsole())
        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);
    }
#endif

    qDebug() << "Open Main window"; // << std::endl;

    // Beat data consistency
    if(SlidersBeatData_verticalSlidersRowTitles.size() != _MaxSlidersBeatData
            || SlidersBeatData_LEDBarRowTitles.size() != _MaxSlidersBeatData
            || SlidersBeatData.size() != _MaxSlidersBeatData ) {
        qDebug() << "Inconsistent data structure for Beat sliders and LEDs "
                  << SlidersBeatData_verticalSlidersRowTitles.size()
                  << " " << SlidersBeatData_LEDBarRowTitles.size() << " "
                  << SlidersBeatData.size() << " " << _MaxSlidersBeatData; // << std::endl;
        exit(0);
    }
    if(ButtonsBeatData_ButtonTitles.size() != _MaxButtonsBeatData
            || ButtonsBeatData_LabelTitles.size() != _MaxButtonsBeatData
            || ButtonsBeatData.size() != _MaxButtonsBeatData ) {
        qDebug() << "Inconsistent data structure for Beat buttons and labels "
                  << ButtonsBeatData_ButtonTitles.size()
                  << " " << ButtonsBeatData_LabelTitles.size() << " "
                  << ButtonsBeatData.size() << " " << _MaxButtonsBeatData; // << std::endl;
        exit(0);
    }
    // Mixer data consistency
    if(SlidersMixerData_verticalSlidersRowTitles.size() != _MaxSlidersMixerData
            || SlidersMixerData_LEDBarRowTitles.size() != _MaxSlidersMixerData
            || SlidersMixerData.size() != _MaxSlidersMixerData ) {
        qDebug() << "Inconsistent data structure for Mixer sliders and LEDs "
                  << SlidersMixerData_verticalSlidersRowTitles.size()
                  << " " << SlidersMixerData_LEDBarRowTitles.size() << " "
                  << SlidersMixerData.size() << " " << _MaxSlidersMixerData; // << std::endl;
        exit(0);
    }
    if(ButtonsMixerData_ButtonTitles.size() != _MaxButtonsMixerData
            || ButtonsMixerData_LabelTitles.size() != _MaxButtonsMixerData
            || ButtonsMixerData.size() != _MaxButtonsMixerData ) {
        qDebug() << "Inconsistent data structure for Mixer buttons and labels "
                  << ButtonsMixerData_ButtonTitles.size()
                  << " " << ButtonsMixerData_LabelTitles.size() << " "
                  << ButtonsMixerData.size() << " " << _MaxButtonsMixerData; // << std::endl;
        exit(0);
    }
    // FX data consistency
    if(SlidersFXData_verticalSlidersRowTitles.size() != _MaxSlidersFXData
            || SlidersFXData_LEDBarRowTitles.size() != _MaxSlidersFXData
            || SlidersFXData.size() != _MaxSlidersFXData ) {
        qDebug() << "Inconsistent data structure for FX sliders and LEDs "
                  << SlidersFXData_verticalSlidersRowTitles.size()
                  << " " << SlidersFXData_LEDBarRowTitles.size() << " "
                  << SlidersFXData.size() << " " << _MaxSlidersFXData; // << std::endl;
        exit(0);
    }
    if(ButtonsFXData_ButtonTitles.size() != _MaxButtonsFXData
            || ButtonsFXData_LabelTitles.size() != _MaxButtonsFXData
            || ButtonsFXData.size() != _MaxButtonsFXData ) {
        qDebug() << "Inconsistent data structure for FX buttons and labels "
                  << ButtonsFXData_ButtonTitles.size()
                  << " " << ButtonsFXData_LabelTitles.size() << " "
                  << ButtonsFXData.size() << " " << _MaxButtonsFXData; // << std::endl;
        exit(0);
    }
    // Paths data consistency
    if(SlidersPathsData_verticalSlidersRowTitles.size() != _MaxSlidersPathsData
            || SlidersPathsData_LEDBarRowTitles.size() != _MaxSlidersPathsData
            || SlidersPathsData.size() != _MaxSlidersPathsData ) {
        qDebug() << "Inconsistent data structure for Paths sliders and LEDs "
                  << SlidersPathsData_verticalSlidersRowTitles.size()
                  << " " << SlidersPathsData_LEDBarRowTitles.size() << " "
                  << SlidersPathsData.size() << " " << _MaxSlidersPathsData; // << std::endl;
        exit(0);
    }
    if(ButtonsPathsData_ButtonTitles.size() != _MaxButtonsPathsData
            || ButtonsPathsData_LabelTitles.size() != _MaxButtonsPathsData
            || ButtonsPathsData.size() != _MaxButtonsPathsData ) {
        qDebug() << "Inconsistent data structure for Paths buttons and labels "
                  << ButtonsPathsData_ButtonTitles.size()
                  << " " << ButtonsPathsData_LabelTitles.size() << " "
                  << ButtonsPathsData.size() << " " << _MaxButtonsPathsData; // << std::endl;
        exit(0);
    }
#if defined(TVW) || defined(demo)
    // MinimalIF data consistency
    if(SlidersMinimalIFData_verticalSlidersRowTitles.size() != _MaxSlidersMinimalIFData
            || SlidersMinimalIFData_LEDBarRowTitles.size() != _MaxSlidersMinimalIFData
            || SlidersMinimalIFData.size() != _MaxSlidersMinimalIFData ) {
        qDebug() << "Inconsistent data structure for MinimalIF sliders and LEDs "
                  << SlidersMinimalIFData_verticalSlidersRowTitles.size()
                  << " " << SlidersMinimalIFData_LEDBarRowTitles.size() << " "
                  << SlidersMinimalIFData.size() << " " << _MaxSlidersMinimalIFData; // << std::endl;
        exit(0);
    }
    if(ButtonsMinimalIFData_ButtonTitles.size() != _MaxButtonsMinimalIFData
            || ButtonsMinimalIFData_LabelTitles.size() != _MaxButtonsMinimalIFData
            || ButtonsMinimalIFData.size() != _MaxButtonsMinimalIFData ) {
        qDebug() << "Inconsistent data structure for MinimalIF buttons and labels "
                  << ButtonsMinimalIFData_ButtonTitles.size()
                  << " " << ButtonsMinimalIFData_LabelTitles.size() << " "
                  << ButtonsMinimalIFData.size() << " " << _MaxButtonsMinimalIFData; // << std::endl;
        exit(0);
    }
#endif
    if(TabbedWindowTitles.size() != _NumberOfDisplays ) {
        qDebug() << "Inconsistent data structure for TabbedWindowTitles "
                  << TabbedWindowTitles.size()
                  << " " << _NumberOfDisplays; // << std::endl;
        exit(0);
    }


    QCoreApplication::addLibraryPath(".");
    pg_app = new QApplication(argc, argv);
//    app.setStyle("GTK");

    pg_window = new MainWindow();
    pg_window->installEventFilter(pg_window);
    pg_window->registerMidiEventsToControllers();

    QString sheetName = "porphyrograph-interface";
    QFile file("../qss/" + sheetName.toLower() + ".qss");
    if( !file.open(QFile::ReadOnly) ) {
        file.setFileName("./qss/" + sheetName.toLower() + ".qss");
        file.open(QFile::ReadOnly);
    }
    QString styleSheet = QString::fromLatin1(file.readAll());
    qApp->setStyleSheet(styleSheet);

    pg_window->setCurrentTabWindow(_Beat_display);
#if defined(TVW) || defined(demo)
    pg_window->setCurrentTabWindow(_MinimalIF_display);
#endif
    pg_window->show();
    return pg_app->exec();
}


bool MainWindow::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type()==QEvent::KeyPress) {
        QKeyEvent* key = static_cast<QKeyEvent*>(event);
        // qDebug() << "receive key press event " << key->key() + ' ' << " " << int('a') <<std::endl;
        switch(key->key() + ' ') {
        case Qt::Key_Enter:
            break;
        case Qt::Key_Return:
            break;
        default:
            return QObject::eventFilter(obj, event);
        }
        return true;
    }
    else {
        return QObject::eventFilter(obj, event);
    }
}
