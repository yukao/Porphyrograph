/*! \file pg-light.h
 *
 *
 *     File pg-light.h
 *
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */
 /* based on ENTEC source code */

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

#if defined(_MSC_VER)
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


int FTDI_SendData(int label, unsigned char *data, unsigned int length);
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
bool pg_oneLightGroup_Changed(void);
void pg_Reset_LightGroup_Changed(void);
bool pg_oneLightGroup_Loops(void);
void pg_light_automation_update(void);
void store_one_DMXvalue(unsigned char* myDmx, int DMX_light_address, int channel_no, int fine_channel_no, float value);
void pg_StoreDMXValues_AllLightGroups(void);
void pg_SendDMX(void);
void pg_SendDMXZeros(void);
void pg_StoreDMX(int light_rank, float dimmer_value, float strobe_value, float zoom_value, float pan_value, float tilt_value, float hue_value, float red_value, float green_value, float blue_value, float grey_value);
void pg_StoreDMX(int channel, float channel_value, int light_port, bool has_fine_channel);
void enable_midi();
void init_promk2();
void FTDI_Reload();
void FTDI_Reload();
void DMX_light_initialization(void);
void pg_lightGUI_values_and_pulse_update(int light_param, string light_param_string);
void pg_lightGUI_loop_update(int light_param, string light_param_string);
void pg_lightGUI_all_values_and_pulse_update(void);
void pg_lightGUI_all_loop_update(void);
void pg_lightGUI_initialization(void);
void pg_osc_light_command(string address_string, float float_arguments[PG_MAX_OSC_ARGUMENTS], int nb_arguments);
#endif