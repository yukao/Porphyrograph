#ifndef PGINTERFACESONGMIDI_H
#define PGINTERFACESONGMIDI_H

#include <vector>

#include <QApplication>
#include <QThread>
#include <QTimer>

#include "RtMidi.h"

// midi in / out interface
extern RtMidiIn *midiin;
extern RtMidiOut *midiout;

// MIDI IN/OUT MESSAGES
extern std::vector<unsigned char> messageIn;
extern std::vector<unsigned char> messageOut;

class Worker : public QObject
{
    Q_OBJECT

public slots:
    void receiveMidiIn(void) {
        QString result;
        double stamp;
        int nBytes;
        while(true) {
            printf("Receive midi\n");
            stamp = midiin->getMessage( &messageIn );
            printf("stamp %f\n",stamp);
            nBytes = messageIn.size();
            printf("size %d\n",nBytes);
            if ( nBytes == 0 ) {
                break;
            }
            for (int i=0; i<nBytes; i++ )
              std::cout << "Byte " << i << " = " << (int)messageIn[i] << ", ";
            if ( nBytes > 0 )
              std::cout << "stamp = " << stamp << std::endl;
        }
    }
};

class Controller : public QObject
{
    Q_OBJECT
    QThread workerThread;
public:
    Controller() {
        Worker *worker = new Worker;
        worker->moveToThread(&workerThread);
        connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
        workerThread.start();

        QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, worker, &Worker::receiveMidiIn);
        // connect(timer, SIGNAL(timeout()), worker, SLOT(receiveMidiIn()));
        timer->start(1000);
    }

    ~Controller() {
        workerThread.quit();
        workerThread.wait();
    }
};

// THREADED MIDI MESSAGE READING
extern Controller *midiController;

int intiAndTestMidi( void );
int midiCleanup(void);



#endif // PGINTERFACESONGMIDI_H
