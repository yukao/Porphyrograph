#ifndef _PRO_DRIVER_H
#define _PRO_DRIVER_H


#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <process.h>
#include <assert.h>
#include <tchar.h>
#include <time.h>
#include <conio.h>
#include "Ftd2xx.h"

#ifdef _MSC_VER
    // PORPHYROGRAPH #include "ms_stdint.h"
    #include <stdint.h>
#else
	#include <stdint.h>
#endif

/********************** PLEASE SET THESE FIRST **********************************/

/******************** PRO MK2 LABELS: ASSIGN AS PER YOUR API (request the pdf if you don't have one) *********************/
// THE API Key is LSB First: so if it says 11223344 .. define it as ... 44,33,22,11

//  #define SET_PORT_ASSIGNMENT_LABEL	    0
//  #define SEND_DMX_PORT2					0
//  #define RECEIVE_DMX_PORT2				0
//  #define SEND_MIDI_PORT					0
//  #define RECEIVE_MIDI_PORT				0

#define SET_PORT_ASSIGNMENT_LABEL 209
#define SEND_DMX_PORT2 150
#define RECEIVE_DMX_PORT2 194
#define SEND_MIDI_PORT 228
#define RECEIVE_MIDI_PORT 214

/***********************************************************************************/

// Enttec Pro definitions
#define GET_WIDGET_PARAMS		3
#define GET_WIDGET_PARAMS_REPLY 3
#define SET_WIDGET_PARAMS		4
#define RECEIVE_DMX_PORT1		5
#define SEND_DMX_PORT1			6
#define SEND_DMX_RDM_TX			7
#define RECEIVE_DMX_ON_CHANGE	8
#define RECEIVED_DMX_COS_TYPE	9
#define GET_WIDGET_SN			10
#define SET_API_KEY_LABEL				13
#define HARDWARE_VERSION_LABEL			14


#define ONE_BYTE	1
#define DMX_START_CODE 0x7E 
#define DMX_END_CODE 0xE7 
#define OFFSET 0xFF
#define DMX_HEADER_LENGTH 4
#define BYTE_LENGTH 8
#define HEADER_RDM_LABEL 5
#define NO_RESPONSE 0
#define DMX_PACKET_SIZE 512

#define RX_BUFFER_SIZE 40960
#define TX_BUFFER_SIZE 40960

#pragma pack(1)
typedef struct {
        unsigned char FirmwareLSB;
        unsigned char FirmwareMSB;
        unsigned char BreakTime;
        unsigned char MaBTime;
        unsigned char RefreshRate;
}DMXUSBPROParamsType;

typedef struct {
        unsigned char UserSizeLSB;
        unsigned char UserSizeMSB;
        unsigned char BreakTime;
        unsigned char MaBTime;
        unsigned char RefreshRate;
}DMXUSBPROSetParamsType;
#pragma pack()

struct ReceivedDmxCosStruct
{
	unsigned char start_changed_byte_number;
	unsigned char changed_byte_array[5];
	unsigned char changed_byte_data[40];
};

#define MAX_PROS 20
#define SEND_NOW 0
#define TRUE 1
#define FALSE 0
#define HEAD 0
#define IO_ERROR 9


int FTDI_SendData(int label, unsigned char *data, int length);
int FTDI_ReceiveData(int label, unsigned char *data, unsigned int expected_length);
uint8_t FTDI_SendDataToPro(uint8_t label, unsigned char *data, uint32_t length);
void* Get_Pro_Handle();
uint8_t FTDI_ReceiveDataFromPro(uint8_t label, unsigned char *data, uint32_t expected_length);
uint16_t FTDI_OpenDevice(int device_num, int* VersionMSB, int* VersionLSB,bool full_mode = true);
int FTDI_ListDevices(uint64_t * Locations);
void FTDI_ClosePort();
void FTDI_PurgeBuffer();
void ReceiveMIDI(int PortLabel);
void SendMIDI(int PortLabel, unsigned char channel, unsigned char note, unsigned char velocity);
void ReceiveDMX(int PortLabel);
void SendDMX(int PortLabel);
void pg_StoreDMX_11channels(int portNo, int light_address, float ch1, float ch2, float ch3, float ch4, float ch5, float ch6, float ch7, float ch8, float ch9, float ch10, float ch11);
void pg_SendDMX(void);
void pg_SendDMXZeros(void);
void pg_StoreDMX(int light_rank, float dimmer_value, float strobe_value, float rgb_color_value[3]);
void enable_midi();
void init_promk2();
void FTDI_Reload();
void FTDI_Reload();
void light_init(void);
#endif