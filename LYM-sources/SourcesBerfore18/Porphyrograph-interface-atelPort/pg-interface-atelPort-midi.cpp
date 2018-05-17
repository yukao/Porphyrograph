// midiprobe.cpp
#include <iostream>
#include <cstdlib>
#include <vector>

#include <QApplication>
#include <QThread>

#include "RtMidi.h"

#include "pg-interface-song-midi.h"

// midi in / out interface
RtMidiIn  *midiin = 0;
RtMidiOut *midiout = 0;

// MIDI IN/OUT MESSAGES
std::vector<unsigned char> messageIn;
std::vector<unsigned char> messageOut;

// THREADED MIDI MESSAGE READING
Controller *midiController;

int intiAndTestMidi( void ) {
  printf( "Entering TestMidi\n" );

  //////////////////////////////////////////////
  // MIDI IN
  // RtMidiIn constructor
  try {
    midiin = new RtMidiIn();
  }
  catch ( RtMidiError &error ) {
    error.printMessage();
    exit( EXIT_FAILURE );
  }
  // Check inputs.
  unsigned int nPorts = midiin->getPortCount();
  std::cout << "\nThere are " << nPorts << " MIDI input sources available.\n";
  std::string portName;
  for ( unsigned int i=0; i<nPorts; i++ ) {
    try {
      portName = midiin->getPortName(i);
    }
    catch ( RtMidiError &error ) {
      error.printMessage();
      midiCleanup();
      return 0;
    }
    std::cout << "  Input Port #" << i << ": " << portName << '\n';
  }

  // Open available port.
  printf("opening port %d\n",0);
  midiin->openPort( 0 );
  printf("opening port %d\n",1);
  midiin->openPort( 1 );
  printf("opening port %d\n",2);
  midiin->openPort( 2 );
  // Don't ignore sysex, timing, or active sensing messages.
  midiin->ignoreTypes( false, false, false );


  //////////////////////////////////////////////
  // MIDI OUT
  // RtMidiOut constructor
  try {
    midiout = new RtMidiOut();
  }
  catch ( RtMidiError &error ) {
    error.printMessage();
    exit( EXIT_FAILURE );
  }
  // Check outputs.
  nPorts = midiout->getPortCount();
  std::cout << "\nThere are " << nPorts << " MIDI output ports available.\n";
  for ( unsigned int i=0; i<nPorts; i++ ) {
    try {
      portName = midiout->getPortName(i);
    }
    catch (RtMidiError &error) {
      error.printMessage();
      midiCleanup();
      return 0;
    }
    std::cout << "  Output Port #" << i << ": " << portName << '\n';
  }
  std::cout << '\n';

  // Open available port.
  midiout->openPort( 2 );

  midiController = new Controller();

  return 1;
}

int midiCleanup(void) {
 delete midiin;
 delete midiout;
 return 0;
}
