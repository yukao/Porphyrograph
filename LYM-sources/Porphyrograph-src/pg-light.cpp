/*! \file pg-light.cpp
 *
 *
 *     File pg-light.cpp
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

#include "pg-all_include.h"

// old school globals
DMXUSBPROParamsType PRO_Params;
FT_HANDLE device_handle = NULL ;

//unsigned char APIKey[] = { 0x00, 0x00,0x00, 0x00 };
unsigned char APIKey[] = { 0x17, 0xE1, 0x3A, 0x32 };
bool with_trace = false;

unsigned char DMX_message1[513] = { 0 };
unsigned char DMX_message2[513] = { 0 };
int DMX_message1_length = -1;
int DMX_message2_length = -1;

#if SET_PORT_ASSIGNMENT_LABEL == 0
#error PRO MK2 LABELS NOT DEFINED
#endif

/* Function : FTDI_ClosePort
 * Author	: ENTTEC
 * Purpose  : Closes the Open DMX USB PRO Device Handle
 * Parameters: none
 **/
void FTDI_ClosePort()
{
	if (device_handle != NULL)
		FT_Close(device_handle);
}

/* Function : FTDI_ListDevices
 * Author	: ENTTEC
 * Purpose  : Returns the no. of PRO's conneced to the PC
 * Parameters: none
 **/
int FTDI_ListDevices()
{
	FT_STATUS ftStatus;
	DWORD numDevs=0;
	ftStatus = FT_ListDevices((PVOID)&numDevs,NULL,FT_LIST_NUMBER_ONLY);
	if(ftStatus == FT_OK) 
		return numDevs;
	return NO_RESPONSE; 
}

void FTDI_Reload()
{
	WORD wVID = 0x0403;
	WORD wPID = 0x6001;
	FT_STATUS ftStatus;

	printf ("\nReloading devices for use with drivers ");	
	ftStatus = FT_Reload(wVID,wPID);
	// Must wait a while for devices to be re-enumerated
	Sleep(3500);
	if(ftStatus != FT_OK)
	{
		printf("\nReloading Driver FAILED");
	}
	else
		printf("\nReloading Driver D2XX PASSED");
}

/* Function : FTDI_SendData
 * Author	: ENTTEC
 * Purpose  : Send Data (DMX or other packets) to the PRO
 * Parameters: Label, Pointer to Data Structure, Length of Data
 **/
int FTDI_SendData(int label, unsigned char *data, unsigned int length)
{
	unsigned char end_code = DMX_END_CODE;
	FT_STATUS res = 0;
	DWORD bytes_written = 0;
	// Form Packet Header
	unsigned char header[DMX_HEADER_LENGTH];
	header[0] = (unsigned char)DMX_START_CODE;
	header[1] = (unsigned char)label;
	header[2] = (unsigned char)(length & OFFSET);
	header[3] = (unsigned char)(length >> BYTE_LENGTH);
	// Write The Header
	res = FT_Write(	device_handle,(unsigned char *)header,DMX_HEADER_LENGTH,&bytes_written);
	if (bytes_written != DMX_HEADER_LENGTH) return  NO_RESPONSE;
	// Write The Data
	res = FT_Write(	device_handle,(unsigned char *)data,length,&bytes_written);
	if (bytes_written != length) return  NO_RESPONSE;
	// Write End Code
	res = FT_Write(	device_handle,(unsigned char *)&end_code,ONE_BYTE,&bytes_written);
	if (bytes_written != ONE_BYTE) return  NO_RESPONSE;
	if (res == FT_OK)
		return TRUE;
	else
		return FALSE; 
}

/* Function : FTDI_ReceiveData
 * Author	: ENTTEC
 * Purpose  : Receive Data (DMX or other packets) from the PRO
 * Parameters: Label, Pointer to Data Structure, Length of Data
 **/
int FTDI_ReceiveData(int label, unsigned char *data, unsigned int expected_length)
{

	FT_STATUS res = 0;
	DWORD length = 0;
	DWORD bytes_read =0;
	unsigned char byte = 0;
	char buffer[600];
	// Check for Start Code and matching Label
	while (byte != label)
	{
		while (byte != DMX_START_CODE)
		{
			res = FT_Read(device_handle,(unsigned char *)&byte,ONE_BYTE,&bytes_read);
			if(bytes_read== NO_RESPONSE) return  NO_RESPONSE;
		}
		res = FT_Read(device_handle,(unsigned char *)&byte,ONE_BYTE,&bytes_read);
		if (bytes_read== NO_RESPONSE) return  NO_RESPONSE;
	}
	// Read the rest of the Header Byte by Byte -- Get Length
	res = FT_Read(device_handle,(unsigned char *)&byte,ONE_BYTE,&bytes_read);
	if (bytes_read== NO_RESPONSE) return  NO_RESPONSE;
	length = byte;
	res = FT_Read(device_handle,(unsigned char *)&byte,ONE_BYTE,&bytes_read);
	if (res != FT_OK) return  NO_RESPONSE;
	length += ((uint32_t)byte)<<BYTE_LENGTH;	
	// Check Length is not greater than allowed
	if (length > 600)
	{
		printf("error: recieved length exceeds limit !");
		return  NO_RESPONSE;
	}
	// Read the actual Response Data
	res = FT_Read(device_handle,buffer,length,&bytes_read);
	if(bytes_read != length) return  NO_RESPONSE;
	// Check The End Code
	res = FT_Read(device_handle,(unsigned char *)&byte,ONE_BYTE,&bytes_read);
	if(bytes_read== NO_RESPONSE) return  NO_RESPONSE;
	if (byte != DMX_END_CODE) return  NO_RESPONSE;
	// Copy The Data read to the buffer passed
	memcpy(data,buffer,expected_length);
	return TRUE;
}

/* Function : FTDI_PurgeBuffer
 * Author	: ENTTEC
 * Purpose  : Clears the buffer used internally by the PRO
 * Parameters: none
 **/
void FTDI_PurgeBuffer()
{
	FT_Purge (device_handle,FT_PURGE_TX);
	FT_Purge (device_handle,FT_PURGE_RX);
}


/* Function : FTDI_OpenDevice
 * Author	: ENTTEC
 * Purpose  : Opens the PRO; Tests various parameters; outputs info
 * Parameters: device num (returned by the List Device fuc), Fw Version MSB, Fw Version LSB 
 **/
uint16_t FTDI_OpenDevice(int device_num)
{
	int ReadTimeout = 120;
	int WriteTimeout = 100;
	int VersionMSB =0;
	int VersionLSB =0;
	uint8_t temp[4];
	long version;
	uint8_t major_ver,minor_ver,build_ver;
	int size = 0;
	int res = 0;
	int tries =0;
	uint8_t latencyTimer;
	FT_STATUS ftStatus;
	int BreakTime;
	int MABTime;

	// Try at least 3 times 
	do  {
		//printf("\n------ D2XX ------- Opening [Device %d] ------ Try %d",device_num,tries);
		ftStatus = FT_Open(device_num,&device_handle);
		Sleep(500);
		tries ++;
	} while ((ftStatus != FT_OK) && (tries < 3));

	if (ftStatus == FT_OK) 
	{
		// D2XX Driver Version
		ftStatus = FT_GetDriverVersion(device_handle,(LPDWORD)&version);
		if (ftStatus == FT_OK) 
		{
			major_ver = (uint8_t) (version >> 16);
			minor_ver = (uint8_t) (version >> 8);
			build_ver = (uint8_t) (version & 0xFF);
			printf("Lighting: D2XX Driver Version:: %02d.%02d.%02d ",major_ver,minor_ver,build_ver);
		}
		else
			printf("Lighting: Unable to Get D2XX Driver Version") ;

		// Latency Timer
		ftStatus = FT_GetLatencyTimer (device_handle,(PUCHAR)&latencyTimer);
		if (ftStatus == FT_OK) 
			printf("Lighting: Latency Timer:: %d\n",latencyTimer);		
		else
			printf("Lighting: Unable to Get Latency Timer\n") ;
		
		// These are important values that can be altered to suit your needs
		// Timeout in microseconds: Too high or too low value should not be used 
		FT_SetTimeouts(device_handle,ReadTimeout,WriteTimeout); 
		// Buffer size in bytes (multiple of 4096) 
		FT_SetUSBParameters(device_handle,RX_BUFFER_SIZE,TX_BUFFER_SIZE);
		// Good idea to purge the buffer on initialize
		FT_Purge (device_handle,FT_PURGE_RX);

		// Send Get Widget Params to get Device Info
		//printf("Sending GET_WIDGET_PARAMS packet... ");
 		res = FTDI_SendData(GET_WIDGET_PARAMS,(unsigned char *)&size,2);
		if (res == NO_RESPONSE)
		{
			FT_Purge (device_handle,FT_PURGE_TX);
 			res = FTDI_SendData(GET_WIDGET_PARAMS,(unsigned char *)&size,2);
			if (res == NO_RESPONSE)
			{
				FTDI_ClosePort();
				return  NO_RESPONSE;
			}
		}
		//else
		//	printf("DMX/USB PRO MK2 Connected Succesfully\n");
		// Receive Widget Response
		//printf("\nWaiting for GET_WIDGET_PARAMS_REPLY packet... ");
		res=FTDI_ReceiveData(GET_WIDGET_PARAMS_REPLY,(unsigned char *)&PRO_Params,sizeof(DMXUSBPROParamsType));
		if (res == NO_RESPONSE)
		{
			res=FTDI_ReceiveData(GET_WIDGET_PARAMS_REPLY,(unsigned char *)&PRO_Params,sizeof(DMXUSBPROParamsType));
			if (res == NO_RESPONSE)
			{
				FTDI_ClosePort();
				return  NO_RESPONSE;
			}
		}
		//else
		//	printf("\n GET WIDGET REPLY Received ... ");
		// Firmware  Version
		VersionMSB = PRO_Params.FirmwareMSB;
		VersionLSB = PRO_Params.FirmwareLSB;
		// Display All Info avialable
		res = FTDI_SendData(GET_WIDGET_SN,(unsigned char *)&size,2);
		res=FTDI_ReceiveData(GET_WIDGET_SN,(unsigned char *)&temp,4);
		BreakTime = (int) (PRO_Params.BreakTime * 10.67) + 100;
		MABTime = (int) (PRO_Params.MaBTime * 10.67);
		printf("Lighting: USB PRO Connected, FIRMWARE: %d.%d, BREAK TIME: %d ms, MAB TIME: %d ms, SEND REFRESH RATE: %d p/s\n", VersionMSB, VersionLSB, BreakTime, MABTime, PRO_Params.RefreshRate);
		return TRUE;
	}		
	else // Can't open Device 
		return FALSE;
}

/* Function : enable_midi
 * Author	: ENTTEC
 * Purpose  : Enables MIDI on Port2
 * Parameters: none
 **/
void enable_midi()
{
	uint8_t PortSet[] = { 1,2 };
	BOOL res = 0;

	FTDI_PurgeBuffer();
	// Enable Ports to DMX on port1 and MIDI on Port2
	res = FTDI_SendData(SET_PORT_ASSIGNMENT_LABEL, PortSet, 2);
	Sleep(200);
	printf("\nPRO Mk2 ... Ready for MIDI and DMX1 ... ");
}

/* Function : SendMIDI
 * Author	: ENTTEC
 * Purpose  : Send MIDI via the PRO MK2
 * Parameters: PortLabel: the label tells PRO MK2 to send MIDI
 *			 : channel, note & velocity for the MIDI to send
 * Note     : Use the SEND MIDI Label in your API to send MIDI
 **/
void SendMIDI(int PortLabel, unsigned char channel, unsigned char note, unsigned char velocity)
{
	unsigned char MyData[3];
	int counter = 10;
	BOOL res = 0;
	if (device_handle != NULL)
	{
		// Looping to Send MIDI data
		printf("\n Press Enter to Send MIDI data :");
		_getch();
		for (int i = 0; i < counter; i++)
		{
			MyData[0] = channel;
			MyData[1] = note;
			MyData[2] = velocity;

			// send the array here
			res = FTDI_SendData(PortLabel, MyData, 3);
			if (res < 0)
			{
				printf("\nFAILED to send MIDI ... exiting");
				FTDI_ClosePort();
				break;
			}
			printf("\nMIDI Data: ");
			for (int j = 0; j <= 2; j++)
				printf(" %d ", MyData[j]);
			printf("\nIteration: %d", i);
			Sleep(500);
		}
	}
}

/* Function : ReceiveMIDI
 * Author	: ENTTEC
 * Purpose  : Receive MIDI via the USB PRO
 * Parameters: PortLabel: the label tells PRO MK2 to recieve MIDI
 * Note     : Use the Receive MIDI Label in your AxPI to  recieve MIDI
 **/
void ReceiveMIDI(int PortLabel)
{
	unsigned char MyData[3];
	int counter = 20;
	BOOL res = 0;
	if (device_handle != NULL)
	{
		// Looping to receiving MIDI data
		printf("\nPress Enter to receive MIDI data :");
		_getch();
		for (int i = 0; i < counter; i++)
		{
			memset(MyData, 0, 3);
			res = FTDI_ReceiveData(PortLabel, MyData, 3);
			if (res < 0)
			{
				printf("MIDI Receive FAILED ... exiting \n");
				FTDI_ClosePort();
				break;
			}
			printf("\nMIDI Data : ");
			for (int j = 0; j <= 2; j++) {
				printf(" %02X ", MyData[j]);
			}
			printf("Iteration: %d", i);
		}
	}
}

/* Function : init_promk2
 * Author	: ENTTEC
 * Purpose  : Activates the PRO MK2; Sets both Ports for DMX 
 * Parameters: none
 * Notes: use the Key in your API to activate the PRO MK2
 **/
void init_promk2()
{
	unsigned char* MyKey = APIKey;
	uint8_t PortSet[] = {1,1};
	BOOL res = 0;

	FTDI_PurgeBuffer();
	res = FTDI_SendData(SET_API_KEY_LABEL,MyKey,4);
	Sleep(200);
	printf("\nPRO Mk2 ... Activated ... ");

	// Enable Ports to DMX on both 
	res = FTDI_SendData(SET_PORT_ASSIGNMENT_LABEL,PortSet,2);
	Sleep(200);
	printf("\nPRO Mk2 ... Ready for DMX on both ports ... ");
}

/* Function : ReceiveDMX
 * Author	: ENTTEC
 * Purpose  : Recieve DMX via the USB PRO
 * Parameters: PortLabel: the label tells which port to receive DMX from
 * Note     : Use the keys in your API to receive DMX from Port 2
 **/
void ReceiveDMX(int PortLabel)
{
	unsigned char myDmxIn[513];
	BOOL res = 0;
	if (device_handle != NULL)
	{
		// Looping to receiving DMX data
		printf("\nPress Enter to receive DMX data :");
		_getch();
		printf("\nSetting the widget to receive all DMX data ... ");
		unsigned char send_on_change_flag = 0;
		res = FTDI_SendData(RECEIVE_DMX_ON_CHANGE, &send_on_change_flag, 1);
		if (res < 0)
		{
			printf("DMX Receive FAILED\n");
			FTDI_ClosePort();
			return;
		}
		memset(myDmxIn, 0, 513);
		/* Will receive 99 dmx packets from the PRO MK2
		** For real-time scenarios, read in a while loop in a separate thread
		**/
		for (int i = 0; i < 99; i++)
		{
			res = FTDI_ReceiveData(PortLabel, myDmxIn, 513);
			if (res != TRUE)
				printf("\nerror: DMX Receive FAILED ...  \n");
			printf("\nDMX Data from 0 to 512: ");
			for (int j = 0; j <= 512; j++) {
				printf(" %d ", myDmxIn[j]);
			}
			printf("Iteration: %d", i + 1);
			Sleep(10);
		}
	}
}

// ressets messages after sending them to be ready for more light data storage
void  pg_ResetDMX(void) {
	// sets the buffers to 0
	memset(DMX_message1, 0, 513 * sizeof(unsigned char));
	memset(DMX_message2, 0, 513 * sizeof(unsigned char));
	// sets the lengths to -1
	DMX_message1_length = -1;
	DMX_message2_length = -1;
}

// stores the DMX messages of all lights in light groups
bool pg_oneLightGroup_Changed(void) {
	for (int ind_group = 1; ind_group <= pg_nb_light_groups[pg_current_configuration_rank]; ind_group++) {
		if (pg_light_groups[pg_current_configuration_rank][ind_group - 1].get_changed_since_last_DMX_update() 
			|| pg_light_groups[pg_current_configuration_rank][ind_group - 1].get_one_non_null_pulse()) {
			return true;
		}
	}
	return false;
}

// stores the DMX messages of all lights in light groups
void pg_Reset_LightGroup_Changed(void) {
	for (int ind_group = 1; ind_group <= pg_nb_light_groups[pg_current_configuration_rank]; ind_group++) {
		pg_light_groups[pg_current_configuration_rank][ind_group - 1].reset_changed_since_last_DMX_update();
	}
}

// one light group has an active automation
bool pg_oneLightGroup_Loops(void) {
	for (int ind_group = 1; ind_group <= pg_nb_light_groups[pg_current_configuration_rank]; ind_group++) {
		if (pg_light_groups[pg_current_configuration_rank][ind_group - 1].get_group_is_looped()) {
			return true;
		}
	}
	return false;
}

// update light automations such as loops
void pg_light_automation_update(void) {
	for (int ind_group = 1; ind_group <= pg_nb_light_groups[pg_current_configuration_rank]; ind_group++) {
		if (pg_light_groups[pg_current_configuration_rank][ind_group - 1].get_group_is_looped()) {
			pg_light_groups[pg_current_configuration_rank][ind_group - 1].update_group_loop();
		}
	}
}

void pg_Reset_allLightGroups_Changed(void) {
	for (int ind_group = 1; ind_group <= pg_nb_light_groups[pg_current_configuration_rank]; ind_group++) {
		pg_light_groups[pg_current_configuration_rank][ind_group - 1].reset_changed_since_last_DMX_update();
	}
}

// store the DMX messages for all the lights after an update of at least one value
// all the values have to be set so that the unchanged values are not set to 0
void pg_StoreDMXValues_AllLightGroups(void) {
	for (int ind_group = 1; ind_group <= pg_nb_light_groups[pg_current_configuration_rank]; ind_group++) {
		int ind_light = 0;
		for (Light &light : pg_Lights[pg_current_configuration_rank]) {
			if (light.light_group == ind_group) {
				float null3[3] = { 0.f };
				//printf("store DMX group %d Light %d\n", ind_group, ind_light);
				pg_StoreDMX(ind_light,
					(pg_light_groups[pg_current_configuration_rank][ind_group - 1].get_group_onOff(_dimmer) == true ? 
						pg_light_groups[pg_current_configuration_rank][ind_group - 1].get_group_val(_dimmer, pg_Lights[pg_current_configuration_rank][ind_light].index_in_group) : 0.f),
					(pg_light_groups[pg_current_configuration_rank][ind_group - 1].get_group_onOff(_strobe) == true ? 
						pg_light_groups[pg_current_configuration_rank][ind_group - 1].get_group_val(_strobe, pg_Lights[pg_current_configuration_rank][ind_light].index_in_group) : 0.f),
					(pg_light_groups[pg_current_configuration_rank][ind_group - 1].get_group_onOff(_zoom) == true ? 
						pg_light_groups[pg_current_configuration_rank][ind_group - 1].get_group_val(_zoom, pg_Lights[pg_current_configuration_rank][ind_light].index_in_group) : 0.f),
					(pg_light_groups[pg_current_configuration_rank][ind_group - 1].get_group_onOff(_pan) == true ? 
						pg_light_groups[pg_current_configuration_rank][ind_group - 1].get_group_val(_pan, pg_Lights[pg_current_configuration_rank][ind_light].index_in_group) : 0.f), // onOff stops animation but does not reset to 0
					(pg_light_groups[pg_current_configuration_rank][ind_group - 1].get_group_onOff(_tilt) == true ? 
						pg_light_groups[pg_current_configuration_rank][ind_group - 1].get_group_val(_tilt, pg_Lights[pg_current_configuration_rank][ind_light].index_in_group) : 0.f),  // onOff stops animation but does not reset to 0
					(pg_light_groups[pg_current_configuration_rank][ind_group - 1].get_group_onOff(_hue) == true ? 
						pg_light_groups[pg_current_configuration_rank][ind_group - 1].get_group_val(_hue, pg_Lights[pg_current_configuration_rank][ind_light].index_in_group) : 0.f),
					(pg_light_groups[pg_current_configuration_rank][ind_group - 1].get_group_onOff(_red) == true ? 
						pg_light_groups[pg_current_configuration_rank][ind_group - 1].get_group_val(_red, pg_Lights[pg_current_configuration_rank][ind_light].index_in_group) : 0.f),
					(pg_light_groups[pg_current_configuration_rank][ind_group - 1].get_group_onOff(_green) == true ?
						pg_light_groups[pg_current_configuration_rank][ind_group - 1].get_group_val(_green, pg_Lights[pg_current_configuration_rank][ind_light].index_in_group) : 0.f),
					(pg_light_groups[pg_current_configuration_rank][ind_group - 1].get_group_onOff(_blue) == true ? 
						pg_light_groups[pg_current_configuration_rank][ind_group - 1].get_group_val(_blue, pg_Lights[pg_current_configuration_rank][ind_light].index_in_group) : 0.f),
					(pg_light_groups[pg_current_configuration_rank][ind_group - 1].get_group_onOff(_grey) == true ? 
						pg_light_groups[pg_current_configuration_rank][ind_group - 1].get_group_val(_grey, pg_Lights[pg_current_configuration_rank][ind_light].index_in_group) : 0.f));
				//printf("store DMX group %d Light %d h rgb grey %.2f %.2f %.2f %.2f %.2f\n", ind_group, ind_light,
				//	pg_light_groups[pg_current_configuration_rank][ind_group - 1].get_group_val(_hue, pg_Lights[ind_light]->index_in_group),
				//	pg_light_groups[pg_current_configuration_rank][ind_group - 1].get_group_val(_red, pg_Lights[ind_light]->index_in_group),
				//	pg_light_groups[pg_current_configuration_rank][ind_group - 1].get_group_val(_green, pg_Lights[ind_light]->index_in_group),
				//	pg_light_groups[pg_current_configuration_rank][ind_group - 1].get_group_val(_blue, pg_Lights[ind_light]->index_in_group),
				//	pg_light_groups[pg_current_configuration_rank][ind_group - 1].get_group_val(_grey, pg_Lights[ind_light]->index_in_group));
			}
			ind_light++;
		}
	}
}

void store_one_DMXvalue(unsigned char* myDmx, int DMX_light_address, int channel_no, int fine_channel_no, float value) {
	if (fine_channel_no == 0) {
		myDmx[DMX_light_address + channel_no - 1] = unsigned char(value * 255.f);
	}
	else {
		int channel_value = int(max(0.f, min(1.f, value)) * 65535.f);
		int high_value = channel_value / 256;
		int low_value = channel_value - high_value * 256;
		myDmx[DMX_light_address + channel_no - 1] = unsigned char(high_value);
		myDmx[DMX_light_address + fine_channel_no - 1] = unsigned char(low_value);
	}
}

// stores the part of a DMX message that corresponds to a light
void pg_StoreDMX(int light_rank, float dimmer_value, float strobe_value, float zoom_value, float pan_value, float tilt_value,
	float hue_value, float red_value, float green_value, float blue_value, float grey_value)
{
	unsigned char* myDmx = NULL;
	int DMX_data_size = 0;

	if (device_handle != NULL && light_rank < int(pg_Lights[pg_current_configuration_rank].size()))
	{
		// buffer choice according to port
		if (pg_Lights[pg_current_configuration_rank][light_rank].light_port == 1) {
			myDmx = DMX_message1;
		}
		else if (pg_Lights[pg_current_configuration_rank][light_rank].light_port == 2) {
			myDmx = DMX_message2;
		}
		else
			return;

		// light data in DMX message start at address and are channels long
		DMX_data_size = pg_Lights[pg_current_configuration_rank][light_rank].light_channels;
		if (pg_Lights[pg_current_configuration_rank][light_rank].light_address + DMX_data_size - 1 > 512) {
			sprintf(ErrorStr, "Light no %d address %d and channel nb %d are beyond the capacity of DMX message!", light_rank, pg_Lights[pg_current_configuration_rank][light_rank].light_address, pg_Lights[pg_current_configuration_rank][light_rank].light_channels); ReportError(ErrorStr);
		}

		// sets the channels to 0
		memset(myDmx + pg_Lights[pg_current_configuration_rank][light_rank].light_address, 0, DMX_data_size * sizeof(unsigned char));

		// length update
		if (pg_Lights[pg_current_configuration_rank][light_rank].light_port == 1) {
			DMX_message1_length = max(DMX_message1_length, pg_Lights[pg_current_configuration_rank][light_rank].light_address + DMX_data_size);
		}
		else if (pg_Lights[pg_current_configuration_rank][light_rank].light_port == 2) {
			// printf("send on port 2\n");
			DMX_message2_length = max(DMX_message2_length, pg_Lights[pg_current_configuration_rank][light_rank].light_address + DMX_data_size);
		}

		// First byte has to be 0
		myDmx[0] = 0;
		// null channel means no channel for this command
		if (pg_Lights[pg_current_configuration_rank][light_rank].light_red > 0) {
			store_one_DMXvalue(myDmx, pg_Lights[pg_current_configuration_rank][light_rank].light_address, pg_Lights[pg_current_configuration_rank][light_rank].light_red, pg_Lights[pg_current_configuration_rank][light_rank].light_red_fine, red_value);
			//printf("DMX add %d chann %d fine %d r %.3f\n", pg_Lights[pg_current_configuration_rank][light_rank].light_address, pg_Lights[pg_current_configuration_rank][light_rank].light_red, pg_Lights[pg_current_configuration_rank][light_rank].light_red_fine, rgb_color_value[0]);
		}
		if (pg_Lights[pg_current_configuration_rank][light_rank].light_green > 0) {
			store_one_DMXvalue(myDmx, pg_Lights[pg_current_configuration_rank][light_rank].light_address, pg_Lights[pg_current_configuration_rank][light_rank].light_green, pg_Lights[pg_current_configuration_rank][light_rank].light_green_fine, green_value);
			//printf("DMX add %d chann %d fine %d g %.3f\n", pg_Lights[pg_current_configuration_rank][light_rank].light_address, pg_Lights[pg_current_configuration_rank][light_rank].light_green, pg_Lights[pg_current_configuration_rank][light_rank].light_green_fine, rgb_color_value[1]);
		}
		if (pg_Lights[pg_current_configuration_rank][light_rank].light_blue > 0) {
			store_one_DMXvalue(myDmx, pg_Lights[pg_current_configuration_rank][light_rank].light_address, pg_Lights[pg_current_configuration_rank][light_rank].light_blue, pg_Lights[pg_current_configuration_rank][light_rank].light_blue_fine, blue_value);
			//printf("DMX add %d chann %d fine %d b %.3f\n", pg_Lights[pg_current_configuration_rank][light_rank].light_address, pg_Lights[pg_current_configuration_rank][light_rank].light_blue, pg_Lights[pg_current_configuration_rank][light_rank].light_blue_fine, rgb_color_value[2]);
		}
		if (pg_Lights[pg_current_configuration_rank][light_rank].light_dimmer > 0) {
			store_one_DMXvalue(myDmx, pg_Lights[pg_current_configuration_rank][light_rank].light_address, pg_Lights[pg_current_configuration_rank][light_rank].light_dimmer, pg_Lights[pg_current_configuration_rank][light_rank].light_dimmer_fine, dimmer_value);
			//printf("DMX add %d chann %d fine %d dimm %.3f\n", pg_Lights[pg_current_configuration_rank][light_rank].light_address, pg_Lights[pg_current_configuration_rank][light_rank].light_dimmer, pg_Lights[pg_current_configuration_rank][light_rank].light_dimmer_fine, dimmer_value);
		}
		if (pg_Lights[pg_current_configuration_rank][light_rank].light_grey > 0) {
			store_one_DMXvalue(myDmx, pg_Lights[pg_current_configuration_rank][light_rank].light_address, pg_Lights[pg_current_configuration_rank][light_rank].light_grey, pg_Lights[pg_current_configuration_rank][light_rank].light_grey_fine, grey_value);
			//printf("DMX add %d chann %d fine %d grey %.3f\n", pg_Lights[pg_current_configuration_rank][light_rank].light_address, pg_Lights[pg_current_configuration_rank][light_rank].light_grey, pg_Lights[pg_current_configuration_rank][light_rank].light_grey_fine, grey_value);
		}
		if (pg_Lights[pg_current_configuration_rank][light_rank].light_strobe > 0) {
			store_one_DMXvalue(myDmx, pg_Lights[pg_current_configuration_rank][light_rank].light_address, pg_Lights[pg_current_configuration_rank][light_rank].light_strobe, pg_Lights[pg_current_configuration_rank][light_rank].light_strobe_fine, strobe_value);
			//printf("DMX add %d chann %d fine %d strobe %.3f\n", pg_Lights[pg_current_configuration_rank][light_rank].light_address, pg_Lights[pg_current_configuration_rank][light_rank].light_strobe, pg_Lights[pg_current_configuration_rank][light_rank].light_strobe_fine, strobe_value);
		}
		if (pg_Lights[pg_current_configuration_rank][light_rank].light_zoom > 0) {
			store_one_DMXvalue(myDmx, pg_Lights[pg_current_configuration_rank][light_rank].light_address, pg_Lights[pg_current_configuration_rank][light_rank].light_zoom, pg_Lights[pg_current_configuration_rank][light_rank].light_zoom_fine, zoom_value);
		}	
		if (pg_Lights[pg_current_configuration_rank][light_rank].light_pan > 0) {
			store_one_DMXvalue(myDmx, pg_Lights[pg_current_configuration_rank][light_rank].light_address, pg_Lights[pg_current_configuration_rank][light_rank].light_pan, pg_Lights[pg_current_configuration_rank][light_rank].light_pan_fine, pan_value);
		}
		if (pg_Lights[pg_current_configuration_rank][light_rank].light_tilt > 0) {
			store_one_DMXvalue(myDmx, pg_Lights[pg_current_configuration_rank][light_rank].light_address, pg_Lights[pg_current_configuration_rank][light_rank].light_tilt, pg_Lights[pg_current_configuration_rank][light_rank].light_tilt_fine, tilt_value);
		}
		if (pg_Lights[pg_current_configuration_rank][light_rank].light_hue > 0) {
			store_one_DMXvalue(myDmx, pg_Lights[pg_current_configuration_rank][light_rank].light_address, pg_Lights[pg_current_configuration_rank][light_rank].light_hue, pg_Lights[pg_current_configuration_rank][light_rank].light_hue_fine, hue_value);
		}
		if (with_trace) {
			printf("DMX Data from %d to %d: ", pg_Lights[pg_current_configuration_rank][light_rank].light_address, min(pg_Lights[pg_current_configuration_rank][light_rank].light_address + DMX_data_size, 513) - 1);
			for (int j = pg_Lights[pg_current_configuration_rank][light_rank].light_address; j < min(pg_Lights[pg_current_configuration_rank][light_rank].light_address + DMX_data_size, 513); j++)
				printf(" %d ", myDmx[j]);
			printf("\n");
		}
	}
}

// stores the part of a DMX message that corresponds to a light
void pg_StoreDMX(int channel, float channel_value, int light_port, bool has_fine_channel)
{
	unsigned char* myDmx = NULL;
	int DMX_data_size = 0;

	if (device_handle != NULL && channel >= 1 && channel <= 512 && light_port == 1 || light_port == 2)
	{
		// buffer choice according to port
		if (light_port == 1) {
			myDmx = DMX_message1;
		}
		else if (light_port == 2) {
			myDmx = DMX_message2;
		}
		else
			return;

		// light data in DMX message start at address and are channels long
		if (!has_fine_channel) {
			DMX_data_size = 1;
		}
		else {
			DMX_data_size = 2;
		}
		if (channel + DMX_data_size - 1 > 512) {
			sprintf(ErrorStr, "Store channel value: channel nb %d-%d is beyond the capacity of DMX message!", channel, channel + DMX_data_size - 1); ReportError(ErrorStr);
		}

		// sets the channels to 0
		memset(myDmx + channel, 0, DMX_data_size * sizeof(unsigned char));

		// length update
		if (light_port == 1) {
			DMX_message1_length = max(DMX_message1_length, channel + DMX_data_size);
		}
		else if (light_port == 2) {
			// printf("send on port 2\n");
			DMX_message2_length = max(DMX_message2_length, channel + DMX_data_size);
		}

		// First byte has to be 0
		myDmx[0] = 0;
		if (!has_fine_channel) {
			store_one_DMXvalue(myDmx, 1, channel, 0, channel_value);
		}
		else {
			store_one_DMXvalue(myDmx, 1, channel, channel + 1, channel_value);
		}

		if (with_trace) {
			printf("DMX Data from %d to %d: ", channel, min(channel + DMX_data_size, 512));
			for (int j = channel; j < min(channel + DMX_data_size, 512); j++)
				printf(" %d ", int(myDmx[j]));
			printf("\n");
		}
	}
}

// sends the message to the ports, when they are not empty
void pg_SendDMXZeros() {
	printf("Reset DMX\n");
	pg_ResetDMX();
	DMX_message1_length = 513;
	DMX_message2_length = 513;
	pg_SendDMX();
	//printf("Store DMX channel/val/port %d %.2f %d\n", int(float_arguments[0]), float_arguments[1], int(float_arguments[2]));
	//pg_SendDMX();
}

// sends the message to the ports, when they are not empty
void pg_SendDMX()
{
	BOOL res = 0;

	if (device_handle != NULL)
	{
		if (DMX_message1_length > 0) {
			// First byte has to be 0
			DMX_message1[0] = 0;
			res = FTDI_SendData(SEND_DMX_PORT1, DMX_message1, DMX_message1_length);
			if (with_trace) {
				printf("Send DMX on port 1 size %d\n", DMX_message1_length);
				for (int j = 0; j < min(DMX_message1_length, 512); j++)
					printf(" %d ", int(DMX_message1[j]));
				printf("\n");
			}
			if (res < 0)
			{
				printf("FAILED to send DMX on port 1 ... exiting\n");
				FTDI_ClosePort();
			}
		}

		if (DMX_message2_length > 0) {
			// First byte has to be 0
			DMX_message2[0] = 0;
			res = FTDI_SendData(SEND_DMX_PORT2, DMX_message2, DMX_message2_length);
			if (with_trace) {
				printf("Send DMX on port 2 size %d\n", DMX_message2_length);
				for (int j = 0; j < min(DMX_message2_length, 512); j++)
					printf(" %d ", int(DMX_message2[j]));
				printf("\n");
			}
			if (res < 0)
			{
				printf("FAILED to send DMX on port 2 ... exiting\n");
				FTDI_ClosePort();
			}
		}

		pg_ResetDMX();
		pg_Reset_allLightGroups_Changed();
	}
}

// initialization function with everything to do the test
void DMX_light_initialization(void)
{
	uint8_t Num_Devices = 0;
	uint16_t device_connected = 0;
	int i = 0;
	int device_num = 0;
	BOOL res = 0;
	uint8_t hversion;

	Num_Devices = (uint8_t)FTDI_ListDevices();
	printf("Lighting: %d DMX USB PRO Devices\n", Num_Devices);
	// Number of Found Devices
	if (Num_Devices == 0)
	{
		printf("Looking for DMX USB Devices  - 0 Found\n");
	}
	else
	{
		// If you want to open all; use for loop
		// we'll open the first one only
		 for (i=0;i<Num_Devices;i++)
		 {
			if (device_connected)
				break;
			device_num = i;
			device_connected = FTDI_OpenDevice(device_num);		
		 }

		// Clear the buffer
		FTDI_PurgeBuffer();

		// Check if device is Pro Mk2 ?
		res = FTDI_SendData(HARDWARE_VERSION_LABEL,NULL,0);
		res = FTDI_ReceiveData(HARDWARE_VERSION_LABEL,(unsigned char *)&hversion,1);
		printf("hversion %d ", hversion);

		if (hversion >= 2)
		{
			printf("PRO Mk2 found ... Sending Init Messages ...");
			init_promk2();

			// Send and Receive DMX on PORT2
			// SendDMX(SEND_DMX_PORT2);
			// ReceiveDMX(RECEIVE_DMX_PORT2);
		}
		printf("\n");
	}

	pg_ResetDMX();
	pg_SendDMXZeros();
}


////////////////////////////////////////////////////////////////////
// GUI UPDATE
void pg_lightGUI_values_and_pulse_update(int light_param, int interface_light_group, string light_param_string) {
	if (pg_nb_light_groups[pg_current_configuration_rank] > 0) {
		sprintf(AuxString, "/light/%d/%s %.4f", interface_light_group + 1, light_param_string.c_str(),
			pg_light_groups[pg_current_configuration_rank][pg_interface_light_group].get_group_val(light_param, 0));
		//printf("/light/%d/%s %.4f\n", interface_light_group + 1, light_param_string.c_str(),
		//	pg_light_groups[pg_current_configuration_rank][pg_interface_light_group].get_group_val(light_param, 0));
		pg_send_message_udp((char*)"f", AuxString, (char*)"udp_TouchOSC_send");
		sprintf(AuxString, "/light_control/onOff_%s %d %d", light_param_string.c_str(), interface_light_group + 1,
			pg_light_groups[pg_current_configuration_rank][pg_interface_light_group].get_group_onOff(light_param));
		pg_send_message_udp((char*)"ff", AuxString, (char*)"udp_TouchOSC_send");
		sprintf(AuxString, "/light_pulse/%d/%s %.4f", interface_light_group + 1, light_param_string.c_str(),
			pg_light_groups[pg_current_configuration_rank][pg_interface_light_group].get_group_pulse(light_param));
		pg_send_message_udp((char*)"f", AuxString, (char*)"udp_TouchOSC_send");
		sprintf(AuxString, "/light_control/beatRandom_%s %d %d", light_param_string.c_str(), interface_light_group + 1,
			int(pg_light_groups[pg_current_configuration_rank][pg_interface_light_group].get_group_beatRandom(light_param)));
		pg_send_message_udp((char*)"ff", AuxString, (char*)"udp_TouchOSC_send");
		sprintf(AuxString, "/light_control/beatOnOff_%s %d %d", light_param_string.c_str(), interface_light_group + 1,
			int(pg_light_groups[pg_current_configuration_rank][pg_interface_light_group].get_group_beatOnOff(light_param)));
		pg_send_message_udp((char*)"ff", AuxString, (char*)"udp_TouchOSC_send");
	}
}
void pg_lightGUI_loop_update(int light_param, string light_param_string) {
	if (pg_nb_light_groups[pg_current_configuration_rank] > 0) {
		sprintf(AuxString, "/light_control/loop_%s %d", light_param_string.c_str(),
			pg_light_groups[pg_current_configuration_rank][pg_interface_light_group].get_group_loop_is_looped(light_param));
		pg_send_message_udp((char*)"f", AuxString, (char*)"udp_TouchOSC_send");
		sprintf(AuxString, "/light_control/loop_curve_%s %d", light_param_string.c_str(),
			pg_light_groups[pg_current_configuration_rank][pg_interface_light_group].get_group_loop_curve_type(light_param));
		pg_send_message_udp((char*)"f", AuxString, (char*)"udp_TouchOSC_send");
		sprintf(AuxString, "/light_control/loop_parallel_%s %d", light_param_string.c_str(),
			pg_light_groups[pg_current_configuration_rank][pg_interface_light_group].get_group_loop_parallel(light_param));
		pg_send_message_udp((char*)"f", AuxString, (char*)"udp_TouchOSC_send");
		sprintf(AuxString, "/light_control/loop_min_%s %.4f", light_param_string.c_str(),
			pg_light_groups[pg_current_configuration_rank][pg_interface_light_group].get_group_loop_min(light_param));
		pg_send_message_udp((char*)"f", AuxString, (char*)"udp_TouchOSC_send");
		sprintf(AuxString, "/light_control/loop_max_%s %.4f", light_param_string.c_str(),
			pg_light_groups[pg_current_configuration_rank][pg_interface_light_group].get_group_loop_max(light_param));
		pg_send_message_udp((char*)"f", AuxString, (char*)"udp_TouchOSC_send");
		sprintf(AuxString, "/light_control/loop_speed_%s %.4f", light_param_string.c_str(),
			pg_light_groups[pg_current_configuration_rank][pg_interface_light_group].get_group_loop_speed(light_param));
		pg_send_message_udp((char*)"f", AuxString, (char*)"udp_TouchOSC_send");
	}
}
void pg_lightGUI_all_values_and_pulse_update(void) {
	if (pg_nb_light_groups[pg_current_configuration_rank] > 0) {
		sprintf(AuxString, "/light_control/light_group %d", pg_interface_light_group);
		pg_send_message_udp((char*)"f", AuxString, (char*)"udp_TouchOSC_send");
		//printf("group %d id %s\n", pg_interface_light_group, pg_light_groups[pg_current_configuration_rank][pg_interface_light_group].get_group_id().c_str());
		sprintf(AuxString, "/light_control/light_group_label %s", pg_light_groups[pg_current_configuration_rank][pg_interface_light_group].get_group_id().c_str());
		pg_send_message_udp((char*)"s", AuxString, (char*)"udp_TouchOSC_send");
		for (const auto& myPair : pg_light_param_hashMap) {
			int light_param = myPair.first;
			string light_param_string = myPair.second;
			//printf("light value update %d %d (%s)\n", light_param, pg_interface_light_group, light_param_string.c_str());
			pg_lightGUI_values_and_pulse_update(light_param, pg_interface_light_group, light_param_string);

		}
	}
}
void pg_lightGUI_all_loop_update(void) {
	if (pg_nb_light_groups[pg_current_configuration_rank] > 0) {
		for (const auto& myPair : pg_light_loop_param_hashMap) {
			int light_param = myPair.first;
			string light_param_string = myPair.second;
			pg_lightGUI_loop_update(light_param, light_param_string);

		}
	}
}
void pg_lightGUI_initialization(void) {
	if (pg_nb_light_groups[pg_current_configuration_rank] > 0) {
		sprintf(AuxString, "/light_control/nb_light_groups %d", pg_nb_light_groups[pg_current_configuration_rank]);
		pg_send_message_udp((char*)"f", AuxString, (char*)"udp_TouchOSC_send");
		sprintf(AuxString, "/light_control/light_group %d", pg_interface_light_group);
		pg_send_message_udp((char*)"f", AuxString, (char*)"udp_TouchOSC_send");
		pg_lightGUI_all_values_and_pulse_update();
		pg_lightGUI_all_loop_update();
	}
}
void pg_osc_light_command(string address_string, float float_arguments[PG_MAX_OSC_ARGUMENTS], int nb_arguments) {
	// this is a light change command that has to be turned into a DMX command according to the address and the value
// addresses are /light/[0-9]+/(dimmer|color|strobe|grey)
// splits the argument separated by /
	std::vector<std::string> address_tags;
	address_tags = split_string(address_string, '/');
	//printf("light command %s %s\n", address_tags[0].c_str(), address_tags[1].c_str());
		// light group parameters control
	if (address_tags.size() == 3 && address_tags[0].compare("light") == 0
		&& std::all_of(address_tags[1].begin(), address_tags[1].end(), ::isdigit)) {
		int light_group = stoi(address_tags[1]);
		if (light_group > int(pg_Lights[pg_current_configuration_rank].size()) || light_group <= 0) {
			sprintf(ErrorStr, "Unregistered light group %s/%d in light command (max: %d)!", address_tags[1].c_str(), light_group, pg_nb_light_groups[pg_current_configuration_rank]); ReportError(ErrorStr);
		}
		else if (pg_nb_light_groups[pg_current_configuration_rank] > 0) {
			if (pg_inverse_light_param_hashMap.find(address_tags[2]) == pg_inverse_light_param_hashMap.end()) {
				sprintf(ErrorStr, "Unknown light command (%s)!", address_tags[2].c_str()); ReportError(ErrorStr);
			}
			else {
				if (nb_arguments != 1) {
					sprintf(ErrorStr, "light set command (%s) expects %d arguments not %d!", address_string.c_str(), 1, nb_arguments); ReportError(ErrorStr);
				}
				else {
					//printf("sets %s for light group %d %.2f\n", address_tags[2].c_str(), light_group, float_arguments[0]);
					pg_light_groups[pg_current_configuration_rank][light_group - 1].set_group_val(pg_inverse_light_param_hashMap[address_tags[2]], float_arguments[0]);
					sprintf(AuxString, "/%s %.5f", address_string.c_str(), float_arguments[0]); pg_send_message_udp((char*)"f", AuxString, (char*)"udp_TouchOSC_send");
					//printf("TouchOSX % s\n", AuxString);
				}
			}
		}
	}
	// light group parameters control
	else if (address_tags.size() == 3 && address_tags[0].compare("light_pulse") == 0
		&& std::all_of(address_tags[1].begin(), address_tags[1].end(), ::isdigit)) {
		int light_group = stoi(address_tags[1]);
		//printf("light pulse %d %.2f\n", light_group, float_arguments[0]);
		if (light_group > pg_nb_light_groups[pg_current_configuration_rank] || light_group <= 0) {
			sprintf(ErrorStr, "Unregistered light group %s/%d in light command (max: %d)!", address_tags[1].c_str(), light_group, pg_nb_light_groups[pg_current_configuration_rank]); ReportError(ErrorStr);
		}
		else if (pg_nb_light_groups[pg_current_configuration_rank] > 0) {
			if (pg_inverse_light_param_hashMap.find(address_tags[2]) == pg_inverse_light_param_hashMap.end()) {
				sprintf(ErrorStr, "Unknown light command (%s)!", address_tags[2].c_str()); ReportError(ErrorStr);
			}
			else {
				if (nb_arguments != 1) {
					sprintf(ErrorStr, "light pulse command (%s) expects %d arguments not %d!", address_string.c_str(), 1, nb_arguments); ReportError(ErrorStr);
				}
				else {
					switch (pg_inverse_light_param_hashMap[address_tags[2]]) {
					case _dimmer:
						//printf("sets dimmer pulse for light group %d %.2f\n", light_group, float_arguments[0]);
						pg_light_groups[pg_current_configuration_rank][light_group - 1].set_group_val_pulse(_dimmer, float_arguments[0]);
						break;
					case _strobe:
						//printf("sets strobe for light group %d %.2f\n", light_group, float_arguments[0]);
						pg_light_groups[pg_current_configuration_rank][light_group - 1].set_group_val_pulse(_strobe, float_arguments[0]);
						break;
					case _zoom:
						//printf("sets zoom for light group %d %.2f\n", light_group, float_arguments[0]);
						pg_light_groups[pg_current_configuration_rank][light_group - 1].set_group_val_pulse(_zoom, float_arguments[0]);
						break;
					case _pan:
						//printf("sets pan for light group %d %.2f\n", light_group, float_arguments[0]);
						pg_light_groups[pg_current_configuration_rank][light_group - 1].set_group_val_pulse(_pan, float_arguments[0]);
						break;
					case _tilt:
						//printf("sets tilt for light group %d %.2f\n", light_group, float_arguments[0]);
						pg_light_groups[pg_current_configuration_rank][light_group - 1].set_group_val_pulse(_tilt, float_arguments[0]);
						break;
					case _hue:
						//printf("sets hue for light group %d %.2f\n", light_group, float_arguments[0]);
						pg_light_groups[pg_current_configuration_rank][light_group - 1].set_group_val_pulse(_hue, float_arguments[0]);
						break;
					case _red:
						//printf("sets red for light group %d %.2f\n", light_group, float_arguments[0]);
						pg_light_groups[pg_current_configuration_rank][light_group - 1].set_group_val_pulse(_red, float_arguments[0]);
						break;
					case _green:
						//printf("sets green for light group %d %.2f\n", light_group, float_arguments[0]);
						pg_light_groups[pg_current_configuration_rank][light_group - 1].set_group_val_pulse(_green, float_arguments[0]);
						break;
					case _blue:
						//printf("sets blue for light group %d %.2f\n", light_group, float_arguments[0]);
						pg_light_groups[pg_current_configuration_rank][light_group - 1].set_group_val_pulse(_blue, float_arguments[0]);
						break;
					case _grey:
						//printf("sets grey for light group %d %.2f\n", light_group, float_arguments[0]);
						pg_light_groups[pg_current_configuration_rank][light_group - 1].set_group_val_pulse(_grey, float_arguments[0]);
						break;
					case _palette_color:
						//printf("sets palette for light group %d %.2f\n", light_group, float_arguments[0]);
						pg_light_groups[pg_current_configuration_rank][light_group - 1].set_group_val_pulse(_palette_color, float_arguments[0]);
						break;
					}
					sprintf(AuxString, "/%s %.5f", address_string.c_str(), float_arguments[0]); pg_send_message_udp((char*)"f", AuxString, (char*)"udp_TouchOSC_send");
					//printf("TouchOSX % s\n", AuxString);
				}
			}
		}
	}
	// light control command
	else if (address_tags.size() == 2 && address_tags[0].compare("light_control") == 0) {
		//printf("address tag %s %s\n", address_tags[0].c_str(), address_tags[1].c_str());
		// switching off all lights
		if (address_tags[1].compare("zero_light") == 0) {
			pg_SendDMXZeros();
			if (pg_nb_light_groups[pg_current_configuration_rank] > 0) {
				for (const auto& myPair : pg_light_param_hashMap) {
					int light_param = myPair.first;
					string light_param_string = myPair.second;
					sprintf(AuxString, "/light/%s %.4f", light_param_string.c_str(),
						pg_light_groups[pg_current_configuration_rank][pg_interface_light_group].get_group_val(pg_light_command_hashMap_IDs(light_param), 0));
					pg_send_message_udp((char*)"f", AuxString, (char*)"udp_TouchOSC_send");
				}
			}
			for (int indLight = 1; indLight <= PG_NB_LIGHTS; indLight++) {
				if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_light_grey]) {
					BrokenInterpolationVar[_light_grey] = true;
					((float*)pg_FullScenarioVarPointers[_light_grey])[indLight] = 0.;
					BrokenInterpolationVar[_light_color] = true;
					((float*)pg_FullScenarioVarPointers[_light_color])[indLight] = 0.;
					BrokenInterpolationVar[_light_dimmer] = true;
					((float*)pg_FullScenarioVarPointers[_light_dimmer])[indLight] = 0.;
					BrokenInterpolationVar[_light_strobe] = true;
					((float*)pg_FullScenarioVarPointers[_light_strobe])[indLight] = 0.;
				}
			}
		}
		// light group change in the interface, reships the parameter values
		else if (address_tags[1].compare("light_group") == 0) {
			if (nb_arguments != 1) {
				sprintf(ErrorStr, "light light_group command (%s) expects %d arguments not %d!", address_string.c_str(), 1, nb_arguments); ReportError(ErrorStr);
			}
			else if (pg_nb_light_groups[pg_current_configuration_rank] > 0) {
				if (int(float_arguments[0]) < pg_nb_light_groups[pg_current_configuration_rank]) {
					pg_interface_light_group = int(float_arguments[0]);
					//printf("light_group command (%s) %d!", address_string.c_str(), pg_interface_light_group); 
					// interface control values updating according to the current light group focus
					// update for all the parameters
					pg_lightGUI_all_values_and_pulse_update();
					// update for the looped parameters
					pg_lightGUI_all_loop_update();
				}
				//else {
				//	sprintf(AuxString, "/light_control/light_group %d", pg_interface_light_group);
				//	pg_send_message_udp((char*)"f", AuxString, (char*)"udp_TouchOSC_send");
				//}
			}
		}
		// single channel value change single precision
		else if (address_tags[1].compare("channel_value") == 0) {
			if (nb_arguments != 3) {
				sprintf(ErrorStr, "light channel_value command (%s) expects %d arguments not %d!", address_string.c_str(), 3, nb_arguments); ReportError(ErrorStr);
			}
			else {
				//printf("Store DMX channel/val/port %d %.2f %d\n", int(float_arguments[0]), float_arguments[1], int(float_arguments[2]));
				pg_StoreDMX(int(float_arguments[0]), float_arguments[1], int(float_arguments[2]), false);
				pg_SendDMX();
			}
		}
		// single channel value change double precision
		else if (address_tags[1].compare("channel_fine_value") == 0) {
			if (nb_arguments != 3) {
				sprintf(ErrorStr, "light channel_fine_value command (%s) expects %d arguments not %d!", address_string.c_str(), 3, nb_arguments); ReportError(ErrorStr);
			}
			else {
				//printf("Store DMX *fine* channel/val/port %d %.2f %d\n", int(float_arguments[0]), float_arguments[1], int(float_arguments[2]));
				pg_StoreDMX(int(float_arguments[0]), float_arguments[1], int(float_arguments[2]), true);
				pg_SendDMX();
			}
		}
		// random value assignment to a light parameter
		else if (address_tags[1].compare(0, string("random_").size(), "random_") == 0) {
			string command = address_tags[1].substr(string("random_").size());
			int light_group = int(float_arguments[0]);
			if (light_group <= pg_nb_light_groups[pg_current_configuration_rank] && light_group > 0) {
				float val = rand_0_1;
				//printf("sets random value %s for light group %d %.2f\n", command.c_str(), light_group, val);
				pg_light_groups[pg_current_configuration_rank][light_group - 1].set_group_val(pg_inverse_light_param_hashMap[command], val);
				sprintf(AuxString, "/light/%d/%s %.4f", light_group, command.c_str(), val); pg_send_message_udp((char*)"f", AuxString, (char*)"udp_TouchOSC_send");
			}
		}
		// onOff assignment to a light parameter
		else if (address_tags[1].compare(0, string("onOff_").size(), "onOff_") == 0) {
			string command = address_tags[1].substr(string("onOff_").size());
			int light_group = int(float_arguments[0]);
			bool is_on = (float_arguments[1] == 1.f);
			if (light_group <= pg_nb_light_groups[pg_current_configuration_rank] && light_group > 0) {
				//printf("sets onOff value %s for light group %d %d\n", command.c_str(), light_group, int(is_on));
				pg_light_groups[pg_current_configuration_rank][light_group - 1].set_group_onOff(pg_inverse_light_param_hashMap[command], is_on);
				sprintf(AuxString, "/light-control/onOff_%s %.2f %.2f", command.c_str(), float(light_group), float(is_on)); pg_send_message_udp((char*)"ff", AuxString, (char*)"udp_TouchOSC_send");
			}
		}
		// beat based onOff change assignment to a light parameter
		else if (address_tags[1].compare(0, string("beatOnOff_").size(), "beatOnOff_") == 0) {
			string command = address_tags[1].substr(string("beatOnOff_").size());
			int light_group = int(float_arguments[0]);
			bool is_on = (float_arguments[1] == 1.f);
			if (light_group <= pg_nb_light_groups[pg_current_configuration_rank] && light_group > 0) {
				//printf("sets beat on/off %s for light group %d %d\n", command.c_str(), light_group, int(is_on));
				pg_light_groups[pg_current_configuration_rank][light_group - 1].set_group_beatOnOff(pg_inverse_light_param_hashMap[command], is_on);
			}
		}
		// beat based random value assignment to a light parameter
		else if (address_tags[1].compare(0, string("beatRandom_").size(), "beatRandom_") == 0) {
			string command = address_tags[1].substr(string("beatRandom_").size());
			int light_group = int(float_arguments[0]);
			bool is_on = (float_arguments[1] == 1.f);
			if (light_group <= pg_nb_light_groups[pg_current_configuration_rank] && light_group > 0) {
				//printf("sets beat random %s for light group %d %d\n", command.c_str(), light_group, int(is_on));
				pg_light_groups[pg_current_configuration_rank][light_group - 1].set_group_beatRandom(pg_inverse_light_param_hashMap[command], is_on);
			}
		}
		// loop assignment to a light parameter
		else if (address_tags[1].compare(0, string("loop_").size(), "loop_") == 0) {
			if (nb_arguments != 7) {
				sprintf(ErrorStr, "light loop command (%s) expects %d arguments not %d!", address_string.c_str(), 7, nb_arguments); ReportError(ErrorStr);
			}
			else {
				string command = address_tags[1].substr(string("loop_").size());
				//sendOSC('/light_control/loop_tilt', self.values.x, light_group, curve, parallel, min, max, speed, { true })
				bool on_off = (float_arguments[0] == 1.f);
				int light_group = int(float_arguments[1]);
				int curve_type = int(float_arguments[2]);
				int parallel_vs_alternate = int(float_arguments[3]);
				float min_val = float_arguments[4];
				float max_val = float_arguments[5];
				double speed_val = double(float_arguments[6]);
				if (light_group <= pg_nb_light_groups[pg_current_configuration_rank] && light_group > 0) {
					printf("%s loop for light group %d oOff %d curve %d // %d  vals min max speed %.3f %.3f %.3f\n", command.c_str(), light_group, on_off, curve_type, parallel_vs_alternate, min_val, max_val, speed_val);
					pg_light_groups[pg_current_configuration_rank][light_group - 1].set_group_loop(pg_inverse_light_param_hashMap[command], on_off, curve_type, parallel_vs_alternate, min_val, max_val, speed_val);
				}
			}
		}
		else {
			switch (nb_arguments) {
			case 0:
				sprintf(AuxString, "/%s", address_string.c_str());
				pg_send_message_udp((char*)"", AuxString, (char*)"udp_Python_send");
				break;
			case 1:
				sprintf(AuxString, "/%s %.6f", address_string.c_str(), float_arguments[0]);
				pg_send_message_udp((char*)"f", AuxString, (char*)"udp_Python_send");
				break;
			case 2:
				sprintf(AuxString, "/%s %.6f %.6f", address_string.c_str(), float_arguments[0], float_arguments[1]);
				pg_send_message_udp((char*)"ff", AuxString, (char*)"udp_Python_send");
				break;
			case 3:
				sprintf(AuxString, "/%s %.6f %.6f %.6f", address_string.c_str(), float_arguments[0], float_arguments[1], float_arguments[2]);
				pg_send_message_udp((char*)"fff", AuxString, (char*)"udp_Python_send");
				break;
			default:
				sprintf(AuxString, "/%s %.6f %.6f %.6f %.6f", address_string.c_str(), float_arguments[0], float_arguments[1], float_arguments[2], float_arguments[3]);
				pg_send_message_udp((char*)"ffff", AuxString, (char*)"udp_Python_send");
				break;
			}
		}
	}
}
