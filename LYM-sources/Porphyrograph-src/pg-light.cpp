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
int FTDI_SendData(int label, unsigned char *data, int length)
{
	unsigned char end_code = DMX_END_CODE;
	FT_STATUS res = 0;
	DWORD bytes_to_write = length;
	DWORD bytes_written = 0;
	HANDLE event = NULL;
	int size=0;
	// Form Packet Header
	unsigned char header[DMX_HEADER_LENGTH];
	header[0] = DMX_START_CODE;
	header[1] = label;
	header[2] = length & OFFSET;
	header[3] = length >> BYTE_LENGTH;
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
	DWORD bytes_to_read = 1;
	DWORD bytes_read =0;
	unsigned char byte = 0;
	HANDLE event = NULL;
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
	int recvd =0;
	unsigned char byte = 0;
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
			major_ver = (int) version >> 16;
			minor_ver = (int) version >> 8;
			build_ver = (int) version & 0xFF;
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

/* Function : SendDMX
 * Author	: ENTTEC
 * Purpose  : Send DMX via the USB PRO  
 * Parameters: PortLabel: the label tells which port to send DMX on 
 * Note     : Use the keys in your API to send DMX to Port 2  	
 **/
void SendDMX(int PortLabel)
{
	unsigned char myDmx[513];
	int counter = 1;
	BOOL res =0; 
	if (device_handle != NULL)
	{
		// Looping to Send DMX data
		printf("\n Press Enter to Send DMX data :");
		_getch();
		for (int i = 0; i < counter ; i++)
		{
			// sets the channels to increasing value: 1=1, 2=2 etc ....
			memset(myDmx,i,sizeof(myDmx));

			// First byte has to be 0
			myDmx[0] = 0;
			for (int i = 1; i <= 512; i++) {
				myDmx[i] = 0;
			}

			// send the array here
			res = FTDI_SendData(PortLabel, myDmx, 513);
			if (res < 0)
			{
				printf("\nFAILED to send DMX ... exiting");
				FTDI_ClosePort();
				break;
			}
			if (with_trace) {
				printf("\nDMX Data from 0 to 8: ");
				for (int j = 0; j <= 8; j++)
					printf(" %d ", myDmx[j]);
				printf("\nIteration: %d", i);
			}
				
		}
	}
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

// light function discovery on 11 channels
void pg_StoreDMX_11channels(int portNo, int light_address, float ch1, float ch2, float ch3, float ch4, float ch5, float ch6, float ch7, float ch8, float ch9, float ch10, float ch11)
{
	unsigned char* myDmx = NULL;
	BOOL res = 0;

	if (device_handle != NULL && light_address + 11 <= 513)
	{
		// buffer choice according to port and length update
		if (portNo == 1) {
			myDmx = DMX_message1;
			DMX_message1_length = max(DMX_message1_length, light_address + 11);
		}
		else if (portNo == 2) {
			myDmx = DMX_message2;
			DMX_message2_length = max(DMX_message2_length, light_address + 11);
		}
		else
			return;

		// First byte has to be 0
		myDmx[0] = 0;

		myDmx[light_address + 0] = unsigned char(ch1 * 255.f);
		myDmx[light_address + 1] = unsigned char(ch2 * 255.f);
		myDmx[light_address + 2] = unsigned char(ch3 * 255.f);
		myDmx[light_address + 3] = unsigned char(ch4 * 255.f);
		myDmx[light_address + 4] = unsigned char(ch5 * 255.f);
		myDmx[light_address + 5] = unsigned char(ch6 * 255.f);
		myDmx[light_address + 6] = unsigned char(ch7 * 255.f);
		myDmx[light_address + 7] = unsigned char(ch8 * 255.f);
		myDmx[light_address + 8] = unsigned char(ch9 * 255.f);
		myDmx[light_address + 9] = unsigned char(ch10 * 255.f);
		myDmx[light_address + 10] = unsigned char(ch11 * 255.f);
		if (with_trace) {
			printf("DMX Data from %d to %d: ", light_address, light_address + 10);
			for (int j = light_address; j < light_address + 11; j++)
				printf(" %d ", myDmx[j]);
			printf("\n");
		}
	}
}

// stores the part of a DMX message that corresponds to a light
void pg_StoreDMX(int light_rank, float dimmer_value, float strobe_value, float rgb_color_value[3])
{
	unsigned char* myDmx = NULL;
	int DMX_data_size = 0;

	if (device_handle != NULL && light_rank < nb_lights)
	{
		// buffer choice according to port
		if (lights_port[light_rank] == 1) {
			myDmx = DMX_message1;
		}
		else if (lights_port[light_rank] == 2) {
			myDmx = DMX_message2;
		}
		else
			return;

		// light data in DMX message start at address and are channels long
		DMX_data_size = lights_channels[light_rank];
		if (lights_address[light_rank] + DMX_data_size > 513) {
			sprintf(ErrorStr, "Light no %d address %d and channel nb %d are beyond the capacity of DMX message!", light_rank, lights_address[light_rank], lights_channels[light_rank]); ReportError(ErrorStr);
		}

		// sets the channels to 0
		memset(myDmx + lights_address[light_rank], 0, DMX_data_size * sizeof(unsigned char));

		// length update
		if (lights_port[light_rank] == 1) {
			DMX_message1_length = max(DMX_message1_length, lights_address[light_rank] + DMX_data_size);
		}
		else if (lights_port[light_rank] == 2) {
			// printf("send on port 2\n");
			DMX_message2_length = max(DMX_message2_length, lights_address[light_rank] + DMX_data_size);
		}

		// First byte has to be 0
		myDmx[0] = 0;
		if(lights_red[light_rank] > 0)
			myDmx[lights_address[light_rank] + lights_red[light_rank] - 1] = unsigned char(rgb_color_value[0] * 255.f);
		if (lights_green[light_rank] > 0)
			myDmx[lights_address[light_rank] + lights_green[light_rank] - 1] = unsigned char(rgb_color_value[1] * 255.f);
		if (lights_blue[light_rank] > 0)
			myDmx[lights_address[light_rank] + lights_blue[light_rank] - 1] = unsigned char(rgb_color_value[2] * 255.f);
		if (lights_dimmer[light_rank] > 0)
			myDmx[lights_address[light_rank] + lights_dimmer[light_rank] - 1] = unsigned char(dimmer_value * 255.f);
		if (lights_grey[light_rank] > 0)
			myDmx[lights_address[light_rank] + lights_grey[light_rank] - 1] = unsigned char(dimmer_value * 255.f);
		if (lights_strobe[light_rank] > 0)
			myDmx[lights_address[light_rank] + lights_strobe[light_rank] - 1] = unsigned char(strobe_value * 255.f);
		if (with_trace) {
			printf("DMX Data from %d to %d: ", lights_address[light_rank], min(DMX_data_size, 513) - 1);
			for (int j = lights_address[light_rank]; j < min(DMX_data_size, 513) ; j++)
				printf(" %d ", myDmx[j]);
			printf("\n");
		}
	}
}

// sends the message to the ports, when they are not empty
void pg_SendDMXZeros() {
	pg_ResetDMX();
	DMX_message1_length = 513;
	DMX_message2_length = 513;
	pg_SendDMX();
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
			if (with_trace)
				printf("Send DMX on port 1 size %d\n", DMX_message1_length);
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
			if (with_trace)
				printf("Send DMX on port 2 size %d\n", DMX_message2_length);
			if (res < 0)
			{
				printf("FAILED to send DMX on port 2 ... exiting\n");
				FTDI_ClosePort();
			}
		}

		pg_ResetDMX();
	}
}

// initialization function with everything to do the test
void light_init(void)
{
	uint8_t Num_Devices =0;
	uint16_t device_connected =0;
	int i=0;
	int device_num=0;
	BOOL res = 0;
	uint8_t hversion;

	//printf("\nEnttec Pro - C - Windows - Sample Test\n");
	//printf("\nLooking for USB PRO's connected to PC ... ");
	
	// If you face problems identifying the PRO: Use this code to reload device drivers: takes a few secs
	//FTDI_Reload();

	// Just to make sure the Device is correct
	//printf("\n Press Enter to Intialize Device :");
	//_getch();
	Num_Devices = FTDI_ListDevices(); 
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

		 //printf("\n Send DMX on PORT 1");
		 //SendDMX(SEND_DMX_PORT1);
		//ReceiveDMX(RECEIVE_DMX_PORT1);

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

			/*
				// Send and Receive DMX on PORT2
			SendDMX(SEND_DMX_PORT2);
			ReceiveDMX(RECEIVE_DMX_PORT2);

			// Send and Recieve MIDI 
			enable_midi();
			SendMIDI(SEND_MIDI_PORT,10,20,0x12);
			ReceiveMIDI(RECEIVE_MIDI_PORT);
			*/
		}
		printf("\n");
	}

	pg_ResetDMX();
	pg_SendDMXZeros();
}