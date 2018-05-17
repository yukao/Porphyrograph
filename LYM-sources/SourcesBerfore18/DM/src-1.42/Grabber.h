#ifndef _GRABBER_H_
#define _GRABBER_H_

// This is a fascade around the incredibly poorly designed
// DirectShow API for Video Capture.

extern char *grabberLastDevice; 	// The name of the last device started

//X
char **grabberGetDevices( int *count=0 );
	// list of video capture devices.

//X
int grabberStartDevice( char *deviceName=NULL, int maxWidth=320, int maxHeight=240 );
	// Starts the specified device, the first one if none specified

int grabberDeviceNumFromName( char *deviceName );//ajout Didier
	//returns the index of grabberConnection

void grabberShutdownDevice( char *deviceName=NULL );
	// very important, 

//X
void grabberShutdownAll();
	// Clears all

//X
char* grabberLockNewest( char *deviceName=NULL, int *frameNumber=NULL );
	// Locks and gives the newest frame available.
	// deviceName = NULL  => last device  started
	// bottom to top ordering, RGB
	// If you specify an (optional) frame number, it will
	// return NULL if there is no frame available newer than frameNumber 
	// in which case you need not unlock it.

//X
char *grabberLockNewest( int deviceNum, int *frameNumber=NULL ); //ajout

//X
void grabberUnlock( char *deviceName=NULL );
	// Unlocks the buffer.  call this as soon as the frame is processedint

int grabberGetAvgFrameTimeInMils( char *deviceName=(char*)0 );
	// Get the avg speed at which frames are arriving from this device

//X
void grabberGetBitmapDesc( char *deviceName, int &w, int &h, int &d );
	// deviceName=NULL   for last device
	// w = width in pixels, h = height in pixels, d = depth in bytes
	// bottom to top ordering, RGB

int grabberShowFilterPropertyPageModalDialog( char *deviceName=(char*)0 );
	// Shows a modal dialog box for the device's filter properties
	// Returns 1 if the vidcap system has changed its size
	// in which case, if you do any high-level buffering of the dimensions,
	// you should update it.

//X
int grabberShowPinPropertyPageModalDialog( char *deviceName=(char*)0 );
	// Shows a modal dialog box for the device's output pin properties
	// Returns 1 if the vidcap system has changed its size
	// in which case, if you do any high-level buffering of the dimensions,
	// you should update it.

HRESULT CaptureVideo( int maxWidth, int maxHeight );


#endif