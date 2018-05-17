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

#include "pg-interface-atelPort-comm.h"
#include "pg-interface-atelPort-control.h"
#include "pg-interface-atelPort-window.h"
#include "pg-interface-atelPort-main.h"

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

    QCoreApplication::addLibraryPath(".");
    pg_app = new QApplication(argc, argv);
//    app.setStyle("GTK");

    pg_window = new MainWindow();
    pg_window->installEventFilter(pg_window);

    QString sheetName = "porphyrograph-interface";
    QFile file("../qss/" + sheetName.toLower() + ".qss");
    if( !file.open(QFile::ReadOnly) ) {
        file.setFileName("./qss/" + sheetName.toLower() + ".qss");
        file.open(QFile::ReadOnly);
    }
    QString styleSheet = QString::fromLatin1(file.readAll());
    qApp->setStyleSheet(styleSheet);

    pg_window->reinitializeMidiLighting(_Graphs_display);
    pg_window->reinitializeMidiLighting(_Tracks_display);
    pg_window->reinitializeMidiLighting(_Paths_display);
    pg_window->lastTabWindow = _Tracks_display;
    pg_window->setCurrentTabWindow(_Tracks_display);
    pg_window->show();
    return pg_app->exec();
}


bool MainWindow::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type()==QEvent::KeyPress) {
        QKeyEvent* key = static_cast<QKeyEvent*>(event);
        // std::cout << "receive key press event " << key->key() + ' ' << " " << int('a') <<std::endl;
        switch(key->key() + ' ') {
        case Qt::Key_Enter:
            break;
        case Qt::Key_Return:
            break;
/*
        case int('a'):
            propagateMidiEventsToControllers(_Graphs_display,
                                             11,
                                             7,
                                             1,
                                             64);
            break;
        case int('b'):
            propagateMidiEventsToControllers(_Graphs_display,
                                             11,
                                             7,
                                             2,
                                             64);
            break;
        case int('c'):
            propagateMidiEventsToControllers(_Graphs_display,
                                             11,
                                             7,
                                             1,
                                             0);
            break;
        case int('d'):
            propagateMidiEventsToControllers(_Graphs_display,
                                             11,
                                             7,
                                             2,
                                             0);
            break;
        case int('e'):
            propagateMidiEventsToControllers(_Graphs_display,
                                             11,
                                             9,
                                             1,
                                             0);
            break;
        case int('f'):
            propagateMidiEventsToControllers(_Graphs_display,
                                             11,
                                             13,
                                             2,
                                             64);
            break;
        case int('g'):
            propagateMidiEventsToControllers(_Graphs_display,
                                             11,
                                             13,
                                             2,
                                             0);
            break;
        case int('h'):
            statusBar()->showMessage(tr("Reconnecting"),1000);
            UDPreceiver->reconnect();
            break;
            */

        default:
            return QObject::eventFilter(obj, event);
        }
        return true;
    }
    else {
        return QObject::eventFilter(obj, event);
    }
}
