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

#include "pg-light-all_include.h"

// old school globals
DMXUSBPROParamsType PRO_Params;
FT_HANDLE device_handle = NULL;

//unsigned char APIKey[] = { 0x00, 0x00,0x00, 0x00 };
unsigned char APIKey[] = { 0x17, 0xE1, 0x3A, 0x32 };
bool pg_with_light_trace = false;

unsigned char pg_DMX_message1[513] = { 0 };
unsigned char pg_DMX_message2[513] = { 0 };
int pg_DMX_message1_length = -1;
int pg_DMX_message2_length = -1;

float pg_pulse_light_prec[PG_NB_LIGHTS + 1][3] = { { 0.f } };


#if SET_PORT_ASSIGNMENT_LABEL == 0
#error PRO MK2 LABELS NOT DEFINED
#endif

#ifndef _WIN32
#define Sleep sleep
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
	DWORD numDevs = 0;
	ftStatus = FT_ListDevices((PVOID)&numDevs, NULL, FT_LIST_NUMBER_ONLY);
	if (ftStatus == FT_OK)
		return numDevs;
	return NO_RESPONSE;
}

/* Function : FTDI_SendData
 * Author	: ENTTEC
 * Purpose  : Send Data (DMX or other packets) to the PRO
 * Parameters: Label, Pointer to Data Structure, Length of Data
 **/
int FTDI_SendData(int label, unsigned char* data, unsigned int length)
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
	res = FT_Write(device_handle, (unsigned char*)header, DMX_HEADER_LENGTH, &bytes_written);
	if (bytes_written != DMX_HEADER_LENGTH) return  NO_RESPONSE;
	// Write The Data
	res = FT_Write(device_handle, (unsigned char*)data, length, &bytes_written);
	if (bytes_written != length) return  NO_RESPONSE;
	// Write End Code
	res = FT_Write(device_handle, (unsigned char*)&end_code, ONE_BYTE, &bytes_written);
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
int FTDI_ReceiveData(int label, unsigned char* data, unsigned int expected_length)
{

	FT_STATUS res = 0;
	DWORD length = 0;
	DWORD bytes_read = 0;
	unsigned char byte = 0;
	char buffer[600];
	// Check for Start Code and matching Label
	while (byte != label)
	{
		while (byte != DMX_START_CODE)
		{
			res = FT_Read(device_handle, (unsigned char*)&byte, ONE_BYTE, &bytes_read);
			if (bytes_read == NO_RESPONSE) return  NO_RESPONSE;
		}
		res = FT_Read(device_handle, (unsigned char*)&byte, ONE_BYTE, &bytes_read);
		if (bytes_read == NO_RESPONSE) return  NO_RESPONSE;
	}
	// Read the rest of the Header Byte by Byte -- Get Length
	res = FT_Read(device_handle, (unsigned char*)&byte, ONE_BYTE, &bytes_read);
	if (bytes_read == NO_RESPONSE) return  NO_RESPONSE;
	length = byte;
	res = FT_Read(device_handle, (unsigned char*)&byte, ONE_BYTE, &bytes_read);
	if (res != FT_OK) return  NO_RESPONSE;
	length += ((uint32_t)byte) << BYTE_LENGTH;
	// Check Length is not greater than allowed
	if (length > 600)
	{
		printf("error: recieved length exceeds limit !");
		return  NO_RESPONSE;
	}
	// Read the actual Response Data
	res = FT_Read(device_handle, buffer, length, &bytes_read);
	if (bytes_read != length) return  NO_RESPONSE;
	// Check The End Code
	res = FT_Read(device_handle, (unsigned char*)&byte, ONE_BYTE, &bytes_read);
	if (bytes_read == NO_RESPONSE) return  NO_RESPONSE;
	if (byte != DMX_END_CODE) return  NO_RESPONSE;
	// Copy The Data read to the buffer passed
	memcpy(data, buffer, expected_length);
	return TRUE;
}

/* Function : FTDI_PurgeBuffer
 * Author	: ENTTEC
 * Purpose  : Clears the buffer used internally by the PRO
 * Parameters: none
 **/
void FTDI_PurgeBuffer()
{
	FT_Purge(device_handle, FT_PURGE_TX);
	FT_Purge(device_handle, FT_PURGE_RX);
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
	int VersionMSB = 0;
	int VersionLSB = 0;
	uint8_t temp[4];
	long version;
	uint8_t major_ver, minor_ver, build_ver;
	int size = 0;
	int res = 0;
	int tries = 0;
	uint8_t latencyTimer;
	FT_STATUS ftStatus;
	int BreakTime;
	int MABTime;

	// Try at least 3 times 
	do {
		//printf("\n------ D2XX ------- Opening [Device %d] ------ Try %d",device_num,tries);
		ftStatus = FT_Open(device_num, &device_handle);
		Sleep(500);
		tries++;
	} while ((ftStatus != FT_OK) && (tries < 3));

	if (ftStatus == FT_OK)
	{
		// D2XX Driver Version
		ftStatus = FT_GetDriverVersion(device_handle, (LPDWORD)&version);
		if (ftStatus == FT_OK)
		{
			major_ver = (uint8_t)(version >> 16);
			minor_ver = (uint8_t)(version >> 8);
			build_ver = (uint8_t)(version & 0xFF);
			printf("Lighting: D2XX Driver Version:: %02d.%02d.%02d ", major_ver, minor_ver, build_ver);
		}
		else
			printf("Lighting: Unable to Get D2XX Driver Version");

		// Latency Timer
		ftStatus = FT_GetLatencyTimer(device_handle, (PUCHAR)&latencyTimer);
		if (ftStatus == FT_OK)
			printf("Lighting: Latency Timer:: %d\n", latencyTimer);
		else
			printf("Lighting: Unable to Get Latency Timer\n");

		// These are important values that can be altered to suit your needs
		// Timeout in microseconds: Too high or too low value should not be used 
		FT_SetTimeouts(device_handle, ReadTimeout, WriteTimeout);
		// Buffer size in bytes (multiple of 4096) 
		FT_SetUSBParameters(device_handle, RX_BUFFER_SIZE, TX_BUFFER_SIZE);
		// Good idea to purge the buffer on initialize
		FT_Purge(device_handle, FT_PURGE_RX);

		// Send Get Widget Params to get Device Info
		//printf("Sending GET_WIDGET_PARAMS packet... ");
		res = FTDI_SendData(GET_WIDGET_PARAMS, (unsigned char*)&size, 2);
		if (res == NO_RESPONSE)
		{
			FT_Purge(device_handle, FT_PURGE_TX);
			res = FTDI_SendData(GET_WIDGET_PARAMS, (unsigned char*)&size, 2);
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
		res = FTDI_ReceiveData(GET_WIDGET_PARAMS_REPLY, (unsigned char*)&PRO_Params, sizeof(DMXUSBPROParamsType));
		if (res == NO_RESPONSE)
		{
			res = FTDI_ReceiveData(GET_WIDGET_PARAMS_REPLY, (unsigned char*)&PRO_Params, sizeof(DMXUSBPROParamsType));
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
		res = FTDI_SendData(GET_WIDGET_SN, (unsigned char*)&size, 2);
		res = FTDI_ReceiveData(GET_WIDGET_SN, (unsigned char*)&temp, 4);
		BreakTime = (int)(PRO_Params.BreakTime * 10.67) + 100;
		MABTime = (int)(PRO_Params.MaBTime * 10.67);
		printf("Lighting: USB PRO Connected, FIRMWARE: %d.%d, BREAK TIME: %d ms, MAB TIME: %d ms, SEND REFRESH RATE: %d p/s\n", VersionMSB, VersionLSB, BreakTime, MABTime, PRO_Params.RefreshRate);
		return TRUE;
	}
	else // Can't open Device 
		return FALSE;
}

/* Function : pg_init_promk2
 * Author	: ENTTEC
 * Purpose  : Activates the PRO MK2; Sets both Ports for DMX
 * Parameters: none
 * Notes: use the Key in your API to activate the PRO MK2
 **/
void pg_init_promk2()
{
	unsigned char* MyKey = APIKey;
	uint8_t PortSet[] = { 1,1 };
	BOOL res = 0;

	FTDI_PurgeBuffer();
	res = FTDI_SendData(SET_API_KEY_LABEL, MyKey, 4);
	Sleep(200);
	printf("\nPRO Mk2 ... Activated ... ");

	// Enable Ports to DMX on both 
	res = FTDI_SendData(SET_PORT_ASSIGNMENT_LABEL, PortSet, 2);
	Sleep(200);
	printf("\nPRO Mk2 ... Ready for DMX on both ports ... ");
}

// ressets messages after sending them to be ready for more light data storage
void  pg_ResetDMX(void) {
	// sets the buffers to 0
	memset(pg_DMX_message1, 0, 513 * sizeof(unsigned char));
	memset(pg_DMX_message2, 0, 513 * sizeof(unsigned char));
	// sets the lengths to -1
	pg_DMX_message1_length = -1;
	pg_DMX_message2_length = -1;
}

// stores the DMX messages of all lights in light groups
bool pg_oneLightGroup_Changed(void) {
	for (int ind_group = 1; ind_group <= pg_nb_light_groups[pg_ind_scenario]; ind_group++) {
		if (pg_light_groups[pg_ind_scenario][ind_group - 1].get_changed_since_last_DMX_update()
			|| pg_light_groups[pg_ind_scenario][ind_group - 1].get_one_non_null_pulse()) {
			return true;
		}
	}
	return false;
}

// stores the DMX messages of all lights in light groups
void pg_Reset_LightGroup_Changed(void) {
	for (int ind_group = 1; ind_group <= pg_nb_light_groups[pg_ind_scenario]; ind_group++) {
		pg_light_groups[pg_ind_scenario][ind_group - 1].reset_changed_since_last_DMX_update();
	}
}

// one light group has an active automation
bool pg_oneLightGroup_Loops(void) {
	for (int ind_group = 1; ind_group <= pg_nb_light_groups[pg_ind_scenario]; ind_group++) {
		if (pg_light_groups[pg_ind_scenario][ind_group - 1].get_group_is_looped()) {
			return true;
		}
	}
	return false;
}

// update light automations such as loops
void pg_light_automation_update(void) {
	for (int ind_group = 1; ind_group <= pg_nb_light_groups[pg_ind_scenario]; ind_group++) {
		if (pg_light_groups[pg_ind_scenario][ind_group - 1].get_group_is_looped()) {
			pg_light_groups[pg_ind_scenario][ind_group - 1].update_group_loop();
		}
	}
}

void pg_Reset_allLightGroups_Changed(void) {
	for (int ind_group = 1; ind_group <= pg_nb_light_groups[pg_ind_scenario]; ind_group++) {
		pg_light_groups[pg_ind_scenario][ind_group - 1].reset_changed_since_last_DMX_update();
	}
}

// store the DMX messages for all the lights after an update of at least one value
// all the values have to be set so that the unchanged values are not set to 0
void pg_StoreDMXValues_AllLightGroups(void) {
	for (int ind_group = 1; ind_group <= pg_nb_light_groups[pg_ind_scenario]; ind_group++) {
		for(unsigned int ind_light = 1; ind_light <= pg_Lights[pg_ind_scenario].size(); ind_light++) {
			Light& light = pg_Lights[pg_ind_scenario].at(ind_light - 1);
			//printf("group %d light %d index %d dimmer onOff %d index in group %d dimmer Master %.2f val %.2f\n", ind_group, light.light_group, ind_light,
			//	pg_light_groups[pg_ind_scenario][ind_group - 1].get_group_onOff(pg_enum_light_dimmer),
			//	light.index_in_group, pg_light_groups[pg_ind_scenario][ind_group - 1].get_group_master(pg_enum_light_dimmer),
			//	pg_light_groups[pg_ind_scenario][ind_group - 1].get_group_val(pg_enum_light_dimmer, light.index_in_group));
			if (light.light_group == ind_group) {
				pg_StoreDMX(ind_light - 1,
					(pg_light_groups[pg_ind_scenario][ind_group - 1].get_group_onOff(pg_enum_light_dimmer) == true ?
						pg_light_groups[pg_ind_scenario][ind_group - 1].get_group_val(pg_enum_light_dimmer, light.index_in_group) : 0.f),
					(pg_light_groups[pg_ind_scenario][ind_group - 1].get_group_onOff(pg_enum_light_strobe) == true ?
						pg_light_groups[pg_ind_scenario][ind_group - 1].get_group_val(pg_enum_light_strobe, light.index_in_group) : 0.f),
					(pg_light_groups[pg_ind_scenario][ind_group - 1].get_group_onOff(pg_enum_light_zoom) == true ?
						pg_light_groups[pg_ind_scenario][ind_group - 1].get_group_val(pg_enum_light_zoom, light.index_in_group) : 0.f),
					(pg_light_groups[pg_ind_scenario][ind_group - 1].get_group_onOff(pg_enum_light_pan) == true ?
						pg_light_groups[pg_ind_scenario][ind_group - 1].get_group_val(pg_enum_light_pan, light.index_in_group) : 0.f), // onOff stops animation but does not reset to 0
					(pg_light_groups[pg_ind_scenario][ind_group - 1].get_group_onOff(pg_enum_light_tilt) == true ?
						pg_light_groups[pg_ind_scenario][ind_group - 1].get_group_val(pg_enum_light_tilt, light.index_in_group) : 0.f),  // onOff stops animation but does not reset to 0
					(pg_light_groups[pg_ind_scenario][ind_group - 1].get_group_onOff(pg_enum_light_hue) == true ?
						pg_light_groups[pg_ind_scenario][ind_group - 1].get_group_val(pg_enum_light_hue, light.index_in_group) : 0.f),
					(pg_light_groups[pg_ind_scenario][ind_group - 1].get_group_onOff(pg_enum_light_red) == true ?
						pg_light_groups[pg_ind_scenario][ind_group - 1].get_group_val(pg_enum_light_red, light.index_in_group) : 0.f),
					(pg_light_groups[pg_ind_scenario][ind_group - 1].get_group_onOff(pg_enum_light_green) == true ?
						pg_light_groups[pg_ind_scenario][ind_group - 1].get_group_val(pg_enum_light_green, light.index_in_group) : 0.f),
					(pg_light_groups[pg_ind_scenario][ind_group - 1].get_group_onOff(pg_enum_light_blue) == true ?
						pg_light_groups[pg_ind_scenario][ind_group - 1].get_group_val(pg_enum_light_blue, light.index_in_group) : 0.f),
					(pg_light_groups[pg_ind_scenario][ind_group - 1].get_group_onOff(pg_enum_light_grey) == true ?
						pg_light_groups[pg_ind_scenario][ind_group - 1].get_group_val(pg_enum_light_grey, light.index_in_group) : 0.f));
				//printf("store DMX group %d Light %d str rgb %.2f %.2f %.2f %.2f grey %.2f dim %.2f (index in group %d)\n\n", ind_group, ind_light,
				//	pg_light_groups[pg_ind_scenario][ind_group - 1].get_group_val(pg_enum_light_strobe, light.index_in_group),
				//	pg_light_groups[pg_ind_scenario][ind_group - 1].get_group_val(pg_enum_light_red, light.index_in_group),
				//	pg_light_groups[pg_ind_scenario][ind_group - 1].get_group_val(pg_enum_light_green, light.index_in_group),
				//	pg_light_groups[pg_ind_scenario][ind_group - 1].get_group_val(pg_enum_light_blue, light.index_in_group),
				//	pg_light_groups[pg_ind_scenario][ind_group - 1].get_group_val(pg_enum_light_grey, light.index_in_group),
				//	pg_light_groups[pg_ind_scenario][ind_group - 1].get_group_val(pg_enum_light_dimmer, light.index_in_group),
				//	light.index_in_group);
			}
		}
	}
}

void pg_store_one_DMXvalue(unsigned char* myDmx, int DMX_light_address, int channel_no, int fine_channel_no, float value) {
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

	if (device_handle != NULL && light_rank < int(pg_Lights[pg_ind_scenario].size()))
	{
		// buffer choice according to port
		if (pg_Lights[pg_ind_scenario][light_rank].light_port == 1) {
			myDmx = pg_DMX_message1;
		}
		else if (pg_Lights[pg_ind_scenario][light_rank].light_port == 2) {
			myDmx = pg_DMX_message2;
		}
		else
			return;

		// light data in DMX message start at address and are channels long
		DMX_data_size = pg_Lights[pg_ind_scenario][light_rank].light_channels;
		if (pg_Lights[pg_ind_scenario][light_rank].light_address + DMX_data_size - 1 > 512) {
			sprintf(pg_errorStr, "Light no %d address %d and channel nb %d are beyond the capacity of DMX message!", light_rank, pg_Lights[pg_ind_scenario][light_rank].light_address, pg_Lights[pg_ind_scenario][light_rank].light_channels); pg_ReportError(pg_errorStr);
		}

		// sets the channels to 0
		memset(myDmx + pg_Lights[pg_ind_scenario][light_rank].light_address, 0, DMX_data_size * sizeof(unsigned char));

		// length update
		if (pg_Lights[pg_ind_scenario][light_rank].light_port == 1) {
			pg_DMX_message1_length = max(pg_DMX_message1_length, pg_Lights[pg_ind_scenario][light_rank].light_address + DMX_data_size);
		}
		else if (pg_Lights[pg_ind_scenario][light_rank].light_port == 2) {
			// printf("send on port 2\n");
			pg_DMX_message2_length = max(pg_DMX_message2_length, pg_Lights[pg_ind_scenario][light_rank].light_address + DMX_data_size);
		}

		// First byte has to be 0
		myDmx[0] = 0;
		// null channel means no channel for this command
		if (pg_Lights[pg_ind_scenario][light_rank].light_red > 0) {
			pg_store_one_DMXvalue(myDmx, pg_Lights[pg_ind_scenario][light_rank].light_address, pg_Lights[pg_ind_scenario][light_rank].light_red, pg_Lights[pg_ind_scenario][light_rank].light_red_fine, red_value);
			//printf("DMX add %d chann %d fine %d r %.3f\n", pg_Lights[pg_ind_scenario][light_rank].light_address, pg_Lights[pg_ind_scenario][light_rank].light_red, pg_Lights[pg_ind_scenario][light_rank].light_red_fine, rgb_color_value[0]);
		}
		if (pg_Lights[pg_ind_scenario][light_rank].light_green > 0) {
			pg_store_one_DMXvalue(myDmx, pg_Lights[pg_ind_scenario][light_rank].light_address, pg_Lights[pg_ind_scenario][light_rank].light_green, pg_Lights[pg_ind_scenario][light_rank].light_green_fine, green_value);
			//printf("DMX add %d chann %d fine %d g %.3f\n", pg_Lights[pg_ind_scenario][light_rank].light_address, pg_Lights[pg_ind_scenario][light_rank].light_green, pg_Lights[pg_ind_scenario][light_rank].light_green_fine, rgb_color_value[1]);
		}
		if (pg_Lights[pg_ind_scenario][light_rank].light_blue > 0) {
			pg_store_one_DMXvalue(myDmx, pg_Lights[pg_ind_scenario][light_rank].light_address, pg_Lights[pg_ind_scenario][light_rank].light_blue, pg_Lights[pg_ind_scenario][light_rank].light_blue_fine, blue_value);
			//printf("DMX add %d chann %d fine %d b %.3f\n", pg_Lights[pg_ind_scenario][light_rank].light_address, pg_Lights[pg_ind_scenario][light_rank].light_blue, pg_Lights[pg_ind_scenario][light_rank].light_blue_fine, rgb_color_value[2]);
		}
		if (pg_Lights[pg_ind_scenario][light_rank].light_dimmer > 0) {
			pg_store_one_DMXvalue(myDmx, pg_Lights[pg_ind_scenario][light_rank].light_address, pg_Lights[pg_ind_scenario][light_rank].light_dimmer, pg_Lights[pg_ind_scenario][light_rank].light_dimmer_fine, dimmer_value);
			//printf("DMX add %d chann %d fine %d dimm %.3f\n", pg_Lights[pg_ind_scenario][light_rank].light_address, pg_Lights[pg_ind_scenario][light_rank].light_dimmer, pg_Lights[pg_ind_scenario][light_rank].light_dimmer_fine, dimmer_value);
		}
		if (pg_Lights[pg_ind_scenario][light_rank].light_grey > 0) {
			pg_store_one_DMXvalue(myDmx, pg_Lights[pg_ind_scenario][light_rank].light_address, pg_Lights[pg_ind_scenario][light_rank].light_grey, pg_Lights[pg_ind_scenario][light_rank].light_grey_fine, grey_value);
			//printf("DMX add %d chann %d fine %d grey %.3f\n", pg_Lights[pg_ind_scenario][light_rank].light_address, pg_Lights[pg_ind_scenario][light_rank].light_grey, pg_Lights[pg_ind_scenario][light_rank].light_grey_fine, grey_value);
		}
		if (pg_Lights[pg_ind_scenario][light_rank].light_strobe > 0) {
			pg_store_one_DMXvalue(myDmx, pg_Lights[pg_ind_scenario][light_rank].light_address, pg_Lights[pg_ind_scenario][light_rank].light_strobe, pg_Lights[pg_ind_scenario][light_rank].light_strobe_fine, strobe_value);
			//printf("DMX add %d chann %d fine %d strobe %.3f\n", pg_Lights[pg_ind_scenario][light_rank].light_address, pg_Lights[pg_ind_scenario][light_rank].light_strobe, pg_Lights[pg_ind_scenario][light_rank].light_strobe_fine, strobe_value);
		}
		if (pg_Lights[pg_ind_scenario][light_rank].light_zoom > 0) {
			pg_store_one_DMXvalue(myDmx, pg_Lights[pg_ind_scenario][light_rank].light_address, pg_Lights[pg_ind_scenario][light_rank].light_zoom, pg_Lights[pg_ind_scenario][light_rank].light_zoom_fine, zoom_value);
		}
		if (pg_Lights[pg_ind_scenario][light_rank].light_pan > 0) {
			pg_store_one_DMXvalue(myDmx, pg_Lights[pg_ind_scenario][light_rank].light_address, pg_Lights[pg_ind_scenario][light_rank].light_pan, pg_Lights[pg_ind_scenario][light_rank].light_pan_fine, pan_value);
		}
		if (pg_Lights[pg_ind_scenario][light_rank].light_tilt > 0) {
			pg_store_one_DMXvalue(myDmx, pg_Lights[pg_ind_scenario][light_rank].light_address, pg_Lights[pg_ind_scenario][light_rank].light_tilt, pg_Lights[pg_ind_scenario][light_rank].light_tilt_fine, tilt_value);
		}
		if (pg_Lights[pg_ind_scenario][light_rank].light_hue > 0) {
			pg_store_one_DMXvalue(myDmx, pg_Lights[pg_ind_scenario][light_rank].light_address, pg_Lights[pg_ind_scenario][light_rank].light_hue, pg_Lights[pg_ind_scenario][light_rank].light_hue_fine, hue_value);
		}
		if (pg_with_light_trace) {
			printf("DMX Data from %d to %d: ", pg_Lights[pg_ind_scenario][light_rank].light_address, min(pg_Lights[pg_ind_scenario][light_rank].light_address + DMX_data_size, 513) - 1);
			for (int j = pg_Lights[pg_ind_scenario][light_rank].light_address; j < min(pg_Lights[pg_ind_scenario][light_rank].light_address + DMX_data_size, 513); j++)
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
			myDmx = pg_DMX_message1;
		}
		else if (light_port == 2) {
			myDmx = pg_DMX_message2;
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
			sprintf(pg_errorStr, "Store channel value: channel nb %d-%d is beyond the capacity of DMX message!", channel, channel + DMX_data_size - 1); pg_ReportError(pg_errorStr);
		}

		// sets the channels to 0
		memset(myDmx + channel, 0, DMX_data_size * sizeof(unsigned char));

		// length update
		if (light_port == 1) {
			pg_DMX_message1_length = max(pg_DMX_message1_length, channel + DMX_data_size);
		}
		else if (light_port == 2) {
			// printf("send on port 2\n");
			pg_DMX_message2_length = max(pg_DMX_message2_length, channel + DMX_data_size);
		}

		// First byte has to be 0
		myDmx[0] = 0;
		if (!has_fine_channel) {
			pg_store_one_DMXvalue(myDmx, 1, channel, 0, channel_value);
		}
		else {
			pg_store_one_DMXvalue(myDmx, 1, channel, channel + 1, channel_value);
		}

		if (pg_with_light_trace) {
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
	pg_DMX_message1_length = 513;
	pg_DMX_message2_length = 513;
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
		if (pg_DMX_message1_length > 0) {
			// First byte has to be 0
			pg_DMX_message1[0] = 0;
			res = FTDI_SendData(SEND_DMX_PORT1, pg_DMX_message1, pg_DMX_message1_length);
			if (pg_with_light_trace) {
				printf("Send DMX on port 1 size %d\n", pg_DMX_message1_length);
				for (int j = 0; j < min(pg_DMX_message1_length, 512); j++)
					printf(" %d ", int(pg_DMX_message1[j]));
				printf("\n");
			}
			if (res < 0)
			{
				printf("FAILED to send DMX on port 1 ... exiting\n");
				FTDI_ClosePort();
			}
		}

		if (pg_DMX_message2_length > 0) {
			// First byte has to be 0
			pg_DMX_message2[0] = 0;
			res = FTDI_SendData(SEND_DMX_PORT2, pg_DMX_message2, pg_DMX_message2_length);
			if (pg_with_light_trace) {
				printf("Send DMX on port 2 size %d\n", pg_DMX_message2_length);
				for (int j = 0; j < min(pg_DMX_message2_length, 512); j++)
					printf(" %d ", int(pg_DMX_message2[j]));
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
void pg_DMX_light_initialization(void)
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
		for (i = 0; i < Num_Devices; i++)
		{
			if (device_connected)
				break;
			device_num = i;
			device_connected = FTDI_OpenDevice(device_num);
		}

		// Clear the buffer
		FTDI_PurgeBuffer();

		// Check if device is Pro Mk2 ?
		res = FTDI_SendData(HARDWARE_VERSION_LABEL, NULL, 0);
		res = FTDI_ReceiveData(HARDWARE_VERSION_LABEL, (unsigned char*)&hversion, 1);
		printf("hversion %d ", hversion);

		if (hversion >= 2)
		{
			printf("PRO Mk2 found ... Sending Init Messages ...");
			pg_init_promk2();

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
	/*
	if (pg_nb_light_groups[pg_ind_scenario] > 0) {
		sprintf(pg_AuxString, "/light/%d/%s %.4f", interface_light_group + 1, light_param_string.c_str(),
			pg_light_groups[pg_ind_scenario][pg_interface_light_group].get_group_val(light_param, 0));
		//printf("/light/%d/%s %.4f\n", interface_light_group + 1, light_param_string.c_str(),
		//	pg_light_groups[pg_ind_scenario][pg_interface_light_group].get_group_val(light_param, 0));
		pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_TouchOSC_send");
		sprintf(pg_AuxString, "/light_control/onOff_%s %d %d", light_param_string.c_str(), interface_light_group + 1,
			pg_light_groups[pg_ind_scenario][pg_interface_light_group].get_group_onOff(light_param));
		pg_send_message_udp((char*)"ff", pg_AuxString, (char*)"udp_TouchOSC_send");
		sprintf(pg_AuxString, "/light_pulse/%d/%s %.4f", interface_light_group + 1, light_param_string.c_str(),
			pg_light_groups[pg_ind_scenario][pg_interface_light_group].get_group_pulse(light_param));
		pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_TouchOSC_send");
		sprintf(pg_AuxString, "/light_control/beatRandom_%s %d %d", light_param_string.c_str(), interface_light_group + 1,
			int(pg_light_groups[pg_ind_scenario][pg_interface_light_group].get_group_beatRandom(light_param)));
		pg_send_message_udp((char*)"ff", pg_AuxString, (char*)"udp_TouchOSC_send");
		sprintf(pg_AuxString, "/light_control/beatOnOff_%s %d %d", light_param_string.c_str(), interface_light_group + 1,
			int(pg_light_groups[pg_ind_scenario][pg_interface_light_group].get_group_beatOnOff(light_param)));
		pg_send_message_udp((char*)"ff", pg_AuxString, (char*)"udp_TouchOSC_send");
	}
	*/
}
void pg_lightGUI_loop_update(int light_param, string light_param_string) {
	/*
	if (pg_nb_light_groups[pg_ind_scenario] > 0) {
		sprintf(pg_AuxString, "/light_control/loop_%s %d", light_param_string.c_str(),
			pg_light_groups[pg_ind_scenario][pg_interface_light_group].get_group_loop_is_looped(light_param));
		pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_TouchOSC_send");
		sprintf(pg_AuxString, "/light_control/loop_curve_%s %d", light_param_string.c_str(),
			pg_light_groups[pg_ind_scenario][pg_interface_light_group].get_group_loop_curve_type(light_param));
		pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_TouchOSC_send");
		sprintf(pg_AuxString, "/light_control/loop_parallel_%s %d", light_param_string.c_str(),
			pg_light_groups[pg_ind_scenario][pg_interface_light_group].get_group_loop_parallel(light_param));
		pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_TouchOSC_send");
		sprintf(pg_AuxString, "/light_control/loop_min_%s %.4f", light_param_string.c_str(),
			pg_light_groups[pg_ind_scenario][pg_interface_light_group].get_group_loop_min(light_param));
		pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_TouchOSC_send");
		sprintf(pg_AuxString, "/light_control/loop_max_%s %.4f", light_param_string.c_str(),
			pg_light_groups[pg_ind_scenario][pg_interface_light_group].get_group_loop_max(light_param));
		pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_TouchOSC_send");
		sprintf(pg_AuxString, "/light_control/loop_speed_%s %.4f", light_param_string.c_str(),
			pg_light_groups[pg_ind_scenario][pg_interface_light_group].get_group_loop_speed(light_param));
		pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_TouchOSC_send");
	}
	*/
}
void pg_lightGUI_all_values_and_pulse_update(void) {
	/*
	if (pg_nb_light_groups[pg_ind_scenario] > 0) {
		sprintf(pg_AuxString, "/light_control/light_group %d", pg_interface_light_group);
		pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_TouchOSC_send");
		//printf("group %d id %s\n", pg_interface_light_group, pg_light_groups[pg_ind_scenario][pg_interface_light_group].get_group_id().c_str());
		sprintf(pg_AuxString, "/light_control/light_group_label %s", pg_light_groups[pg_ind_scenario][pg_interface_light_group].get_group_id().c_str());
		pg_send_message_udp((char*)"s", pg_AuxString, (char*)"udp_TouchOSC_send");
		for (const auto& myPair : pg_light_param_hashMap) {
			int light_param = myPair.first;
			string light_param_string = myPair.second;
			//printf("light value update %d %d (%s)\n", light_param, pg_interface_light_group, light_param_string.c_str());
			pg_lightGUI_values_and_pulse_update(light_param, pg_interface_light_group, light_param_string);

		}
	}
	*/
}
void pg_lightGUI_all_loop_update(void) {
	if (pg_nb_light_groups[pg_ind_scenario] > 0) {
		for (const auto& myPair : pg_light_loop_param_hashMap) {
			int light_param = myPair.first;
			string light_param_string = myPair.second;
			pg_lightGUI_loop_update(light_param, light_param_string);

		}
	}
}
void pg_lightGUI_initialization(void) {
	//if (pg_nb_light_groups[pg_ind_scenario] > 0) {
	//	sprintf(pg_AuxString, "/light_control/nb_light_groups %d", pg_nb_light_groups[pg_ind_scenario]);
	//	pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_TouchOSC_send");
	//	sprintf(pg_AuxString, "/light_control/light_group %d", pg_interface_light_group);
	//	pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_TouchOSC_send");
	//	pg_lightGUI_all_values_and_pulse_update();
	//	pg_lightGUI_all_loop_update();
	//}
}
void pg_osc_light_command(string address_string, float float_arguments[PG_MAX_OSC_ARGUMENTS], int nb_arguments) {
	// this is a light change command that has to be turned into a DMX command according to the address and the value
// addresses are /light/[0-9]+/(dimmer|color|strobe|grey)
// splits the argument separated by /
	std::vector<std::string> address_tags;
	address_tags = pg_split_string(address_string, '/');
	//printf("light command %s %s\n", address_tags[0].c_str(), address_tags[1].c_str());
		// light group parameters control
	if (address_tags.size() == 3 && address_tags[0].compare("light") == 0
		&& std::all_of(address_tags[1].begin(), address_tags[1].end(), ::isdigit)) {
		int light_group = stoi(address_tags[1]);
		if (light_group > int(pg_Lights[pg_ind_scenario].size()) || light_group <= 0) {
			sprintf(pg_errorStr, "Unregistered light group %s/%d in light command (max: %d)!", address_tags[1].c_str(), light_group, pg_nb_light_groups[pg_ind_scenario]); pg_ReportError(pg_errorStr);
		}
		else if (pg_nb_light_groups[pg_ind_scenario] > 0) {
			if (pg_inverse_light_param_hashMap.find(address_tags[2]) == pg_inverse_light_param_hashMap.end()) {
				sprintf(pg_errorStr, "Unknown light command (%s)!", address_tags[2].c_str()); pg_ReportError(pg_errorStr);
			}
			else {
				if (nb_arguments != 1) {
					sprintf(pg_errorStr, "light set command (%s) expects %d arguments not %d!", address_string.c_str(), 1, nb_arguments); pg_ReportError(pg_errorStr);
				}
				else {
					//printf("sets %s for light group %d %.2f\n", address_tags[2].c_str(), light_group, float_arguments[0]);
					pg_light_groups[pg_ind_scenario][light_group - 1].set_group_val(pg_inverse_light_param_hashMap[address_tags[2]], float_arguments[0]);
					sprintf(pg_AuxString, "/%s %.5f", address_string.c_str(), float_arguments[0]); pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_TouchOSC_send");
					//printf("TouchOSX % s\n", pg_AuxString);
				}
			}
		}
	}
	// light group parameters control
	else if (address_tags.size() == 3 && address_tags[0].compare("light_pulse") == 0
		&& std::all_of(address_tags[1].begin(), address_tags[1].end(), ::isdigit)) {
		int light_group = stoi(address_tags[1]);
		//printf("light pg_audio_pulse %d %.2f\n", light_group, float_arguments[0]);
		if (light_group > pg_nb_light_groups[pg_ind_scenario] || light_group <= 0) {
			sprintf(pg_errorStr, "Unregistered light group %s/%d in light command (max: %d)!", address_tags[1].c_str(), light_group, pg_nb_light_groups[pg_ind_scenario]); pg_ReportError(pg_errorStr);
		}
		else if (pg_nb_light_groups[pg_ind_scenario] > 0) {
			if (pg_inverse_light_param_hashMap.find(address_tags[2]) == pg_inverse_light_param_hashMap.end()) {
				sprintf(pg_errorStr, "Unknown light command (%s)!", address_tags[2].c_str()); pg_ReportError(pg_errorStr);
			}
			else {
				if (nb_arguments != 1) {
					sprintf(pg_errorStr, "light pg_audio_pulse command (%s) expects %d arguments not %d!", address_string.c_str(), 1, nb_arguments); pg_ReportError(pg_errorStr);
				}
				else {
					switch (pg_inverse_light_param_hashMap[address_tags[2]]) {
					case pg_enum_light_dimmer:
						printf("sets dimmer pg_audio_pulse for light group %d %.2f\n", light_group, float_arguments[0]);
						pg_light_groups[pg_ind_scenario][light_group - 1].set_group_val_pulse(pg_enum_light_dimmer, float_arguments[0]);
						break;
					case pg_enum_light_strobe:
						//printf("sets strobe for light group %d %.2f\n", light_group, float_arguments[0]);
						pg_light_groups[pg_ind_scenario][light_group - 1].set_group_val_pulse(pg_enum_light_strobe, float_arguments[0]);
						break;
					case pg_enum_light_zoom:
						//printf("sets zoom for light group %d %.2f\n", light_group, float_arguments[0]);
						pg_light_groups[pg_ind_scenario][light_group - 1].set_group_val_pulse(pg_enum_light_zoom, float_arguments[0]);
						break;
					case pg_enum_light_pan:
						//printf("sets pan for light group %d %.2f\n", light_group, float_arguments[0]);
						pg_light_groups[pg_ind_scenario][light_group - 1].set_group_val_pulse(pg_enum_light_pan, float_arguments[0]);
						break;
					case pg_enum_light_tilt:
						//printf("sets tilt for light group %d %.2f\n", light_group, float_arguments[0]);
						pg_light_groups[pg_ind_scenario][light_group - 1].set_group_val_pulse(pg_enum_light_tilt, float_arguments[0]);
						break;
					case pg_enum_light_hue:
						//printf("sets hue for light group %d %.2f\n", light_group, float_arguments[0]);
						pg_light_groups[pg_ind_scenario][light_group - 1].set_group_val_pulse(pg_enum_light_hue, float_arguments[0]);
						break;
					case pg_enum_light_red:
						//printf("sets red for light group %d %.2f\n", light_group, float_arguments[0]);
						pg_light_groups[pg_ind_scenario][light_group - 1].set_group_val_pulse(pg_enum_light_red, float_arguments[0]);
						break;
					case pg_enum_light_green:
						//printf("sets green for light group %d %.2f\n", light_group, float_arguments[0]);
						pg_light_groups[pg_ind_scenario][light_group - 1].set_group_val_pulse(pg_enum_light_green, float_arguments[0]);
						break;
					case pg_enum_light_blue:
						//printf("sets blue for light group %d %.2f\n", light_group, float_arguments[0]);
						pg_light_groups[pg_ind_scenario][light_group - 1].set_group_val_pulse(pg_enum_light_blue, float_arguments[0]);
						break;
					case pg_enum_light_grey:
						//printf("sets grey for light group %d %.2f\n", light_group, float_arguments[0]);
						pg_light_groups[pg_ind_scenario][light_group - 1].set_group_val_pulse(pg_enum_light_grey, float_arguments[0]);
						break;
					case pg_enum_light_palette_color:
						//printf("sets palette for light group %d %.2f\n", light_group, float_arguments[0]);
						pg_light_groups[pg_ind_scenario][light_group - 1].set_group_val_pulse(pg_enum_light_palette_color, float_arguments[0]);
						break;
					}
					sprintf(pg_AuxString, "/%s %.5f", address_string.c_str(), float_arguments[0]); pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_TouchOSC_send");
					//printf("TouchOSX % s\n", pg_AuxString);
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
			if (pg_nb_light_groups[pg_ind_scenario] > 0) {
				for (const auto& myPair : pg_light_param_hashMap) {
					int light_param = myPair.first;
					string light_param_string = myPair.second;
					sprintf(pg_AuxString, "/light/%s %.4f", light_param_string.c_str(),
						pg_light_groups[pg_ind_scenario][pg_interface_light_group].get_group_val(pg_light_command_hashMap_IDs(light_param), 0));
					pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_TouchOSC_send");
				}
			}
			for (int indLight = 1; indLight <= PG_NB_LIGHTS; indLight++) {
				if (pg_FullScenarioActiveVars[pg_ind_scenario][_light_grey]) {
					pg_BrokenInterpolationVar[_light_grey] = true;
					((float*)pg_FullScenarioVarPointers[_light_grey])[indLight] = 0.;
					pg_BrokenInterpolationVar[_light_color] = true;
					((float*)pg_FullScenarioVarPointers[_light_color])[indLight] = 0.;
					pg_BrokenInterpolationVar[_light_dimmer] = true;
					((float*)pg_FullScenarioVarPointers[_light_dimmer])[indLight] = 0.;
					pg_BrokenInterpolationVar[_light_strobe] = true;
					((float*)pg_FullScenarioVarPointers[_light_strobe])[indLight] = 0.;
				}
			}
		}
		// light group change in the interface, reships the parameter values
		else if (address_tags[1].compare("light_group") == 0) {
			if (nb_arguments != 1) {
				sprintf(pg_errorStr, "light light_group command (%s) expects %d arguments not %d!", address_string.c_str(), 1, nb_arguments); pg_ReportError(pg_errorStr);
			}
			else if (pg_nb_light_groups[pg_ind_scenario] > 0) {
				if (int(float_arguments[0]) < pg_nb_light_groups[pg_ind_scenario]) {
					pg_interface_light_group = int(float_arguments[0]);
					printf("light_group command (%s) %d!", address_string.c_str(), pg_interface_light_group); 
					// interface control values updating according to the current light group focus
					// update for all the parameters
					pg_lightGUI_all_values_and_pulse_update();
					// update for the looped parameters
					pg_lightGUI_all_loop_update();
				}
				//else {
				//	sprintf(pg_AuxString, "/light_control/light_group %d", pg_interface_light_group);
				//	pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_TouchOSC_send");
				//}
			}
		}
		// single channel value change single precision
		else if (address_tags[1].compare("channel_value") == 0) {
			if (nb_arguments != 3) {
				sprintf(pg_errorStr, "light channel_value command (%s) expects %d arguments not %d!", address_string.c_str(), 3, nb_arguments); pg_ReportError(pg_errorStr);
			}
			else {
				printf("Store DMX channel/val/port %d %.2f %d\n", int(float_arguments[0]), float_arguments[1], int(float_arguments[2]));
				pg_StoreDMX(int(float_arguments[0]), float_arguments[1], int(float_arguments[2]), false);
				pg_SendDMX();
			}
		}
		// single channel value change double precision
		else if (address_tags[1].compare("channel_fine_value") == 0) {
			if (nb_arguments != 3) {
				sprintf(pg_errorStr, "light channel_fine_value command (%s) expects %d arguments not %d!", address_string.c_str(), 3, nb_arguments); pg_ReportError(pg_errorStr);
			}
			else {
				printf("Store DMX *fine* channel/val/port %d %.2f %d\n", int(float_arguments[0]), float_arguments[1], int(float_arguments[2]));
				pg_StoreDMX(int(float_arguments[0]), float_arguments[1], int(float_arguments[2]), true);
				pg_SendDMX();
			}
		}
		// random value assignment to a light parameter
		else if (address_tags[1].compare(0, string("random_").size(), "random_") == 0) {
			string command = address_tags[1].substr(string("random_").size());
			int light_group = int(float_arguments[0]);
			if (light_group <= pg_nb_light_groups[pg_ind_scenario] && light_group > 0) {
				float val = rand_0_1;
				//printf("sets random value %s for light group %d %.2f\n", command.c_str(), light_group, val);
				pg_light_groups[pg_ind_scenario][light_group - 1].set_group_val(pg_inverse_light_param_hashMap[command], val);
				sprintf(pg_AuxString, "/light/%d/%s %.4f", light_group, command.c_str(), val); pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_TouchOSC_send");
			}
		}
		// onOff assignment to a light parameter
		else if (address_tags[1].compare(0, string("onOff_").size(), "onOff_") == 0) {
			string command = address_tags[1].substr(string("onOff_").size());
			int light_group = int(float_arguments[0]);
			bool is_on = (float_arguments[1] == 1.f);
			if (light_group <= pg_nb_light_groups[pg_ind_scenario] && light_group > 0) {
				//printf("sets onOff value %s for light group %d %d\n", command.c_str(), light_group, int(is_on));
				pg_light_groups[pg_ind_scenario][light_group - 1].set_group_onOff(pg_inverse_light_param_hashMap[command], is_on);
				sprintf(pg_AuxString, "/light-control/onOff_%s %.2f %.2f", command.c_str(), float(light_group), float(is_on)); pg_send_message_udp((char*)"ff", pg_AuxString, (char*)"udp_TouchOSC_send");
			}
		}
		// beat based onOff change assignment to a light parameter
		else if (address_tags[1].compare(0, string("beatOnOff_").size(), "beatOnOff_") == 0) {
			string command = address_tags[1].substr(string("beatOnOff_").size());
			int light_group = int(float_arguments[0]);
			bool is_on = (float_arguments[1] == 1.f);
			if (light_group <= pg_nb_light_groups[pg_ind_scenario] && light_group > 0) {
				//printf("sets beat on/off %s for light group %d %d\n", command.c_str(), light_group, int(is_on));
				pg_light_groups[pg_ind_scenario][light_group - 1].set_group_beatOnOff(pg_inverse_light_param_hashMap[command], is_on);
			}
		}
		// beat based random value assignment to a light parameter
		else if (address_tags[1].compare(0, string("beatRandom_").size(), "beatRandom_") == 0) {
			string command = address_tags[1].substr(string("beatRandom_").size());
			int light_group = int(float_arguments[0]);
			bool is_on = (float_arguments[1] == 1.f);
			if (light_group <= pg_nb_light_groups[pg_ind_scenario] && light_group > 0) {
				//printf("sets beat random %s for light group %d %d\n", command.c_str(), light_group, int(is_on));
				pg_light_groups[pg_ind_scenario][light_group - 1].set_group_beatRandom(pg_inverse_light_param_hashMap[command], is_on);
			}
		}
		// loop assignment to a light parameter
		else if (address_tags[1].compare(0, string("loop_").size(), "loop_") == 0) {
			if (nb_arguments != 7) {
				sprintf(pg_errorStr, "light loop command (%s) expects %d arguments not %d!", address_string.c_str(), 7, nb_arguments); pg_ReportError(pg_errorStr);
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
				if (light_group <= pg_nb_light_groups[pg_ind_scenario] && light_group > 0) {
					//printf("%s loop for light group %d oOff %d curve %d // %d  vals min max speed %.3f %.3f %.3f\n", command.c_str(), light_group, on_off, curve_type, parallel_vs_alternate, min_val, max_val, speed_val);
					pg_light_groups[pg_ind_scenario][light_group - 1].set_group_loop(pg_inverse_light_param_hashMap[command], on_off, curve_type, parallel_vs_alternate, min_val, max_val, speed_val);
				}
			}
		}
		else {
			switch (nb_arguments) {
			case 0:
				sprintf(pg_AuxString, "/%s", address_string.c_str());
				pg_send_message_udp((char*)"", pg_AuxString, (char*)"udp_Python_send");
				break;
			case 1:
				sprintf(pg_AuxString, "/%s %.6f", address_string.c_str(), float_arguments[0]);
				pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_Python_send");
				break;
			case 2:
				sprintf(pg_AuxString, "/%s %.6f %.6f", address_string.c_str(), float_arguments[0], float_arguments[1]);
				pg_send_message_udp((char*)"ff", pg_AuxString, (char*)"udp_Python_send");
				break;
			case 3:
				sprintf(pg_AuxString, "/%s %.6f %.6f %.6f", address_string.c_str(), float_arguments[0], float_arguments[1], float_arguments[2]);
				pg_send_message_udp((char*)"fff", pg_AuxString, (char*)"udp_Python_send");
				break;
			default:
				sprintf(pg_AuxString, "/%s %.6f %.6f %.6f %.6f", address_string.c_str(), float_arguments[0], float_arguments[1], float_arguments[2], float_arguments[3]);
				pg_send_message_udp((char*)"ffff", pg_AuxString, (char*)"udp_Python_send");
				break;
			}
		}
	}
}

void pg_UpdateLightGroups_from_LightVars() {
	for (int indLight = 1; indLight <= min(int(PG_NB_LIGHTS), int(pg_Lights[pg_ind_scenario].size())); indLight++) {
		if (light_grey[indLight] != pg_light_grey_prec[indLight] || light_grey_pulse[indLight] != pg_light_grey_pulse_prec[indLight]
			|| light_color[indLight] != pg_light_color_prec[indLight] || light_color_pulse[indLight] != pg_light_color_pulse_prec[indLight]
			|| light_dimmer[indLight] != pg_light_dimmer_prec[indLight] || light_dimmer_pulse[indLight] != pg_light_dimmer_pulse_prec[indLight]
			|| light_strobe[indLight] != pg_light_strobe_prec[indLight] || light_strobe_pulse[indLight] != pg_light_strobe_pulse_prec[indLight]
			|| ((light_grey_pulse[indLight] != 0 || light_color_pulse[indLight] != 0 || light_dimmer_pulse[indLight] != 0 || light_strobe_pulse[indLight] != 0)
				&& (pg_pulse_light_prec[indLight][0] != pg_audio_pulse[0] || pg_pulse_light_prec[indLight][1] != pg_audio_pulse[1] || pg_pulse_light_prec[indLight][2] != pg_audio_pulse[2]))
			// video background and light color are the same and randomly changed
			|| BGcolorRed != pg_BGcolorRed_prec
			|| BGcolorGreen != pg_BGcolorGreen_prec
			|| BGcolorBlue != pg_BGcolorBlue_prec
			) {

			float rgb_color[pg_enum_rgb];
			Light light = pg_Lights[pg_ind_scenario][indLight - 1];
			if (light.light_group <= pg_nb_light_groups[pg_ind_scenario]) {
				if (light_color[indLight] >= 0) {
					pg_compute_pulsed_palette_color(light_color[indLight], light_color_pulse[indLight], light_grey[indLight],
						light_grey_pulse[indLight], rgb_color, pg_enum_LIGHT_COLOR);
				}
				else {
					rgb_color[0] = 0.f;
					rgb_color[1] = 0.f;
					rgb_color[2] = 0.f;
				}
				pg_light_groups[pg_ind_scenario][light.light_group - 1].set_color(rgb_color);
				pg_light_groups[pg_ind_scenario][light.light_group - 1].set_group_val(pg_enum_light_dimmer, 
					light_dimmer[indLight] * (1.f + pulse_average * light_dimmer_pulse[indLight]));
				//printf("light 1: dimmer %.2f group %d val %.2f\n", light_dimmer[1], light.light_group, 
				//	light_dimmer[indLight] * (1.f + pulse_average * light_dimmer_pulse[indLight]));
				pg_light_groups[pg_ind_scenario][light.light_group - 1].set_group_val(pg_enum_light_strobe,
					light_strobe[indLight] * (1.f + pulse_average * light_strobe_pulse[indLight]));
				pg_light_color_prec[indLight] = light_color[indLight];
				pg_light_color_pulse_prec[indLight] = light_color_pulse[indLight];
				pg_light_grey_prec[indLight] = light_grey[indLight];
				pg_light_grey_pulse_prec[indLight] = light_grey_pulse[indLight];
				pg_light_dimmer_prec[indLight] = light_dimmer[indLight];
				pg_light_dimmer_pulse_prec[indLight] = light_dimmer_pulse[indLight];
				pg_light_strobe_prec[indLight] = light_strobe[indLight];
				pg_light_strobe_pulse_prec[indLight] = light_strobe_pulse[indLight];
				if (pg_FullScenarioActiveVars[pg_ind_scenario][_BGcolor]) {
					// video background and light color are the same and randomly changed
					if (BGcolor) {
						float lightColor[pg_enum_rgb] = { BGcolorRed, BGcolorGreen, BGcolorBlue };
						pg_light_groups[pg_ind_scenario][light.light_group - 1].set_color(lightColor);
						pg_BGcolorRed_prec = BGcolorRed;
						pg_BGcolorGreen_prec = BGcolorGreen;
						pg_BGcolorBlue_prec = BGcolorBlue;
					}
				}
			}
			pg_light_groups[pg_ind_scenario][light.light_group - 1].set_changed_since_last_DMX_update();
		}
	}

	if (pg_FullScenarioActiveVars[pg_ind_scenario][_BGcolor]) {
		//printf("pg_light_groups[7]->get_group_val(pg_enum_light_dimmer) %.2f pg_light_groups[0].get_color()[0] %.2f\n", pg_light_groups[7]->get_group_val(pg_enum_light_dimmer), pg_light_groups[0].get_color()[0]);
	}
	for (int indLight = 1; indLight <= PG_NB_LIGHTS; indLight++) {
		pg_pulse_light_prec[indLight][0] = pg_audio_pulse[0];
		pg_pulse_light_prec[indLight][1] = pg_audio_pulse[1];
		pg_pulse_light_prec[indLight][2] = pg_audio_pulse[2];
	}
}

void pg_light_channel_string_to_channel_no(string a_light_channel_string, int* light_channel, int* light_channel_fine, int nb_channels) {
	if (a_light_channel_string.find('/') != std::string::npos) {
		vector<string> two_light_channels_string = pg_split_string(a_light_channel_string, '/');
		// double light channel
		if (two_light_channels_string.size() == 2 && std::all_of(two_light_channels_string[0].begin(), two_light_channels_string[0].end(), ::isdigit)
			&& std::all_of(two_light_channels_string[1].begin(), two_light_channels_string[1].end(), ::isdigit)) {
			*light_channel = stoi(two_light_channels_string[0]);
			if (*light_channel > nb_channels || *light_channel <= 0) {
				sprintf(pg_errorStr, "Unregistered light group %s/%d in light command (max: %d)!", two_light_channels_string[0].c_str(), *light_channel, nb_channels); pg_ReportError(pg_errorStr);
			}
			*light_channel_fine = stoi(two_light_channels_string[1]);
			if (*light_channel_fine > nb_channels || *light_channel_fine <= 0) {
				sprintf(pg_errorStr, "Unregistered light group %s/%d in light command (max: %d)!", two_light_channels_string[1].c_str(), *light_channel_fine, nb_channels); pg_ReportError(pg_errorStr);
			}
		}
	}
	else {
		// single light channel
		if (std::all_of(a_light_channel_string.begin(), a_light_channel_string.end(), ::isdigit)) {
			*light_channel = stoi(a_light_channel_string);
			// 0: no light channel for this command
			// 1-nb_channels: one of the available channels
			if (*light_channel > nb_channels || *light_channel < 0) {
				sprintf(pg_errorStr, "Out of range light channel %s/%d in light command (max: %d)!", a_light_channel_string.c_str(), *light_channel, nb_channels); pg_ReportError(pg_errorStr);
			}
			*light_channel_fine = 0;
		}
	}
}


