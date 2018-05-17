#ifndef PGINTERFACEALL_INCLUDE_H
#define PGINTERFACEALL_INCLUDE_H
#include <vector>
#include <iostream>
#include <cstdlib>
#include <string>

#include <QObject>
#include <QApplication>
#include <QThread>
#include <QTimer>
#include <QWidget>
#include <QGroupBox>
#include <QHostAddress>
#include <QtWidgets>
#include <QtNetwork>

#ifdef _WIN32
#include <windows.h>
#endif

#include "RtMidi.h"

QT_BEGIN_NAMESPACE
class QDialogButtonBox;
class QLabel;
class QPushButton;
class QTimer;
class QUdpSocket;
class QAction;
class QSpinBox;
class QSlider;
class QCheckBox;
class QComboBox;
class QGroupBox;
class QStackedWidget;
QT_END_NAMESPACE

//class SlidersGroup;
//class Sender;
//class Receiver;

#endif // PGINTERFACEALL_INCLUDE_H
