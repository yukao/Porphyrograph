/*! \file pg-udp.cpp
 * 
 * 
 *     Part of this source file is taken from Virtual Choreographer
 *     http://virchor.sourceforge.net/
 * 
 *     File pg-udp.cpp
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
/*
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif
*/
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <stdlib.h>
#include <vector>
#include <string>
#include <cstdio>
#include <cstring>
#include <algorithm>
using std::min;
using std::max;
#include <time.h>
#include <ctype.h>
#include <fcntl.h>
#include <errno.h>
#include <fstream>
#include <math.h>       /* sqrt */

#ifndef _WIN32
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
        #include <sys/resource.h>
	#include <netdb.h>
	#include <unistd.h>
#else // !_WIN32
	//#define socklen_t int
	#include <winsock2.h>
	#include <Ws2tcpip.h>
	//#include <wspiapi.h>
#endif // _WIN32
// \}


#include "lo/lo.h"
#include "OSC_IORec_main.h"
#include "OSC_IORec_UDPClientServer.h"

// OSC_RecordReplay in replay mode
// porphyrograph client for visual messages (mostly pen position, scenes and quitting)
pg_IPClient *Porphyrograph_Replay_client;
// Usine client for sound messages (mostly hand positions from leap motions and scenes)
pg_IPClient *Usine_Replay_client;
// TouchOSC replay control
pg_IPServer *TouchOSC_Replay_server;
// OSC_RecordReplay in record mode
// server receiving messages from porphyrograph (mostly pen position, scenes and quitting) and Leap motion (mostly hand positions)
pg_IPServer *Leap_porphyrograph_Record_server;

// clients for the LeapOSC application in charge of reading Leap data and forwarding them to Usine and OSC_RecordReplay in record mode
pg_IPClient *Leap_Usine_client;
pg_IPClient *Leap_Record_client;

/////////////////////////////////////////////////////
// error reporting
/////////////////////////////////////////////////////

/// Error string
char *ErrorStr = new char[string_length];

void ReportError(char *errorString) {
	fprintf(stderr, "%s\n", errorString);
}

#ifdef _WIN32
static const double epoch = 116444736000000000.;
int gettimeofday(struct timeval* tp, void* tzp) {
	SYSTEMTIME system_time;
	FILETIME file_time;
	ULARGE_INTEGER ularge;

	GetSystemTime(&system_time);
	SystemTimeToFileTime(&system_time, &file_time);
	ularge.LowPart = file_time.dwLowDateTime;
	ularge.HighPart = file_time.dwHighDateTime;

	tp->tv_sec = (long)((ularge.QuadPart - epoch) / 10000000L);
	tp->tv_usec = (long)(system_time.wMilliseconds * 1000);
	/* 0 indicates that the call succeeded. */
	return 0;
}
#endif

double RealTime(void) {
	struct timeval time;
	gettimeofday(&time, 0);
	return (double)time.tv_sec + ((double)time.tv_usec / 1000000.);
}

/////////////////////////////////////////////////////////////////////////////////
// UDP CLIENT

// API for Leap Motion message forwarding
extern "C" void init_LeapMotion_clients_udp(char *Usine_IPaddress, unsigned int Usine_PortNumber, char *OSCRecord_IPaddress, unsigned int OSCRecord_PortNumber);
void init_LeapMotion_clients_udp(char *Usine_IPaddress, unsigned int Usine_PortNumber, char *OSCRecord_IPaddress, unsigned int OSCRecord_PortNumber) {
	Leap_Usine_client = new pg_IPClient(Usine_IPaddress, Usine_PortNumber);
	Leap_Usine_client->InitClient();
	Leap_Record_client = new pg_IPClient(OSCRecord_IPaddress, OSCRecord_PortNumber);
	Leap_Record_client->InitClient();
}

// API for Leap Motion message forwarding
extern "C" void send_message_upd(char *address, char *format, float float_argts[]);
void send_message_upd(char *address, char *format, float float_argts[]) {
	Leap_Usine_client->sendIPmessages(address, format, float_argts, "", 0);
	Leap_Record_client->sendIPmessages(address, format, float_argts, "", 0);
}

pg_IPClient::pg_IPClient(char *IPaddress, unsigned int PortNumber) {
	// remote server address
	Remote_server_IP = std::string(IPaddress);
	Remote_server_port = PortNumber;
	Remote_server_host = NULL;
	printf("Sending UDP messages to \"%s\" at port #%d\n", Remote_server_IP.c_str(), Remote_server_port);

	// remote server socket
	SocketToRemoteServer = SOCKET(-1);
	Output_Message_String
		= new char[max_network_message_length];
	// the OSC string is made 2 times longer because
	// it contains both pattern and argts
	Output_Message_OSC
		= new char[2 * max_network_message_length];
	Output_Message_Pattern
		= new char[max_network_message_length];
	Output_Message_ArgList
		= new char[max_network_message_length];
}
    
pg_IPClient::~pg_IPClient(void) {
#ifdef WIN32
  closesocket(SocketToRemoteServer);
#endif
}

void pg_IPClient::InitClient(void) {
	//////////////////////////////
	// remote server opening
	char Remote_server_port_string[128];
	sprintf_s(Remote_server_port_string, 128, "%d", Remote_server_port);

	// liblo message handling
	// liblo client
	lo_client = lo_address_new(Remote_server_IP.c_str(), Remote_server_port_string);
}

void pg_IPClient::sendOSCMessageFromLine(std::string line) {
	std::string cmd_string;
	std::string OSCaddress;
	std::string OSCpattern;
	float *f_argts = NULL;
	char *s_argts = NULL;
	std::stringstream  sstrem(line);

	// time stamp
	sstrem >> cmd_string;
	// address
	sstrem >> OSCaddress;
	// nb of arguments
	sstrem >> cmd_string;
	int nb_argts = atoi(cmd_string.c_str());
	// pattern
	sstrem >> OSCpattern;
	if (nb_argts == 1) {
		if (OSCpattern.compare("i") == 0) {
			// integer argument
			sstrem >> cmd_string;
			sendIPmessages(OSCaddress.c_str(), OSCpattern.c_str(), NULL, NULL, atoi(cmd_string.c_str()));
		}
		else if (OSCpattern.compare("f") == 0) {
			// float argument
			f_argts = new float[1];
			sstrem >> cmd_string;
			f_argts[0] = float(atof(cmd_string.c_str()));
			sendIPmessages(OSCaddress.c_str(), OSCpattern.c_str(), f_argts, NULL, 0);
			free(f_argts);
		}
		else if (OSCpattern.compare("s") == 0) {
			// integer argument
			sstrem >> cmd_string;
			sendIPmessages(OSCaddress.c_str(), OSCpattern.c_str(), NULL, cmd_string.c_str(), 0);
		}
		else {
			sprintf_s(ErrorStr, string_length, "Error: unexpected 1 character pattern (%s)\n", OSCpattern.c_str()); ReportError(ErrorStr);

		}
	}
	else {
		// float argument
		f_argts = new float[nb_argts];
		std::string expectedPattern = std::string(nb_argts, 'f');
		if (OSCpattern.compare(expectedPattern) != 0) {
			sprintf_s(ErrorStr, string_length, "Error: unexpected %d character pattern (%s)\n", nb_argts, OSCpattern.c_str()); ReportError(ErrorStr);

		}
		for (int indParam = 0; indParam < nb_argts; indParam++) {
			sstrem >> cmd_string;
			f_argts[indParam] = float(atof(cmd_string.c_str()));
		}
		sendIPmessages(OSCaddress.c_str(), OSCpattern.c_str(), f_argts, NULL, 0);
		free(f_argts);
	}
}
void pg_IPClient::sendIPmessages(const char *address, const char *pattern, const float *f_argts, const char *s_argt, const int i_argt) {
	if (SocketToRemoteServer < 0) {
		return;
	}

	// sends OSC message through OSC
	// message that begins by an OSC address /aaaa/aaa 
	// whether or not the pattern is provided with the command
	if (*(pattern)) {
		int pattern_size = int(strlen(pattern));

		// single argument
		if (pattern_size == 1) {
			switch (*(pattern)) {
			case 's':
				if (lo_send(lo_client, address, pattern, s_argt) == -1) {
					sprintf_s(ErrorStr, string_length, "Error: OSC error %d: %s  for message %s\n", lo_address_errno(lo_client),
						lo_address_errstr(lo_client), address); ReportError(ErrorStr);
				}
				break;
			case 'i':
				if (lo_send(lo_client, address, pattern, i_argt) == -1) {
					sprintf_s(ErrorStr, string_length, "Error: OSC error %d: %s  for message %s\n", lo_address_errno(lo_client),
						lo_address_errstr(lo_client), address); ReportError(ErrorStr);
				}
				break;
			case 'f':
				if (lo_send(lo_client, address, pattern, f_argts[0]) == -1) {
					sprintf_s(ErrorStr, string_length, "Error: OSC error %d: %s  for message %s\n", lo_address_errno(lo_client),
						lo_address_errstr(lo_client), address); ReportError(ErrorStr);
				}
				break;
			default:
				sprintf_s(ErrorStr, string_length, "Error: unknown OSC pattern element %c!", *(pattern)); ReportError(ErrorStr);
				break;
			}
		}
		else {
			int length = int(strlen(pattern));
			switch (length) {
			case 2:
				if (lo_send(lo_client, address, pattern,
					f_argts[0], f_argts[1]) == -1) {
					sprintf_s(ErrorStr, string_length, "Error: OSC error %d: %s  for message %s\n", lo_address_errno(lo_client),
						lo_address_errstr(lo_client), address); ReportError(ErrorStr); throw(100);
				}
				break;
			case 3:
				if (lo_send(lo_client, address, pattern,
					f_argts[0], f_argts[1], f_argts[2]) == -1) {
					sprintf_s(ErrorStr, string_length, "Error: OSC error %d: %s  for message %s\n", lo_address_errno(lo_client),
						lo_address_errstr(lo_client), address); ReportError(ErrorStr); throw(100);
				}
				break;
			case 4:
				if (lo_send(lo_client, address, pattern,
					f_argts[0], f_argts[1], f_argts[2], f_argts[3]) == -1) {
					sprintf_s(ErrorStr, string_length, "Error: OSC error %d: %s  for message %s\n", lo_address_errno(lo_client),
						lo_address_errstr(lo_client), address); ReportError(ErrorStr); throw(100);
				}
				break;
			case 5:
				if (lo_send(lo_client, address, pattern,
					f_argts[0], f_argts[1], f_argts[2], f_argts[3],
					f_argts[4]) == -1) {
					sprintf_s(ErrorStr, string_length, "Error: OSC error %d: %s  for message %s\n", lo_address_errno(lo_client),
						lo_address_errstr(lo_client), address); ReportError(ErrorStr); throw(100);
				}
				break;
			case 6:
				if (lo_send(lo_client, address, pattern,
					f_argts[0], f_argts[1], f_argts[2], f_argts[3],
					f_argts[4], f_argts[5]) == -1) {
					sprintf_s(ErrorStr, string_length, "Error: OSC error %d: %s  for message %s\n", lo_address_errno(lo_client),
						lo_address_errstr(lo_client), address); ReportError(ErrorStr); throw(100);
				}
				break;
			case 7:
				if (lo_send(lo_client, address, pattern,
					f_argts[0], f_argts[1], f_argts[2], f_argts[3],
					f_argts[4], f_argts[5], f_argts[6]) == -1) {
					sprintf_s(ErrorStr, string_length, "Error: OSC error %d: %s  for message %s\n", lo_address_errno(lo_client),
						lo_address_errstr(lo_client), address); ReportError(ErrorStr); throw(100);
				}
				break;
			case 8:
				if (lo_send(lo_client, address, pattern,
					f_argts[0], f_argts[1], f_argts[2], f_argts[3],
					f_argts[4], f_argts[5], f_argts[6], f_argts[7]) == -1) {
					sprintf_s(ErrorStr, string_length, "Error: OSC error %d: %s  for message %s\n", lo_address_errno(lo_client),
						lo_address_errstr(lo_client), address); ReportError(ErrorStr); throw(100);
				}
				break;
			case 9:
				if (lo_send(lo_client, address, pattern,
					f_argts[0], f_argts[1], f_argts[2], f_argts[3],
					f_argts[4], f_argts[5], f_argts[6], f_argts[7],
					f_argts[8]) == -1) {
					sprintf_s(ErrorStr, string_length, "Error: OSC error %d: %s  for message %s\n", lo_address_errno(lo_client),
						lo_address_errstr(lo_client), address); ReportError(ErrorStr); throw(100);
				}
				break;
			case 10:
				if (lo_send(lo_client, address, pattern,
					f_argts[0], f_argts[1], f_argts[2], f_argts[3],
					f_argts[4], f_argts[5], f_argts[6], f_argts[7],
					f_argts[8], f_argts[9]) == -1) {
					sprintf_s(ErrorStr, string_length, "Error: OSC error %d: %s  for message %s\n", lo_address_errno(lo_client),
						lo_address_errstr(lo_client), address); ReportError(ErrorStr); throw(100);
				}
				break;
			case 11:
				if (lo_send(lo_client, address, pattern,
					f_argts[0], f_argts[1], f_argts[2], f_argts[3],
					f_argts[4], f_argts[5], f_argts[6], f_argts[7],
					f_argts[8], f_argts[9], f_argts[10]) == -1) {
					sprintf_s(ErrorStr, string_length, "Error: OSC error %d: %s  for message %s\n", lo_address_errno(lo_client),
						lo_address_errstr(lo_client), address); ReportError(ErrorStr); throw(100);
				}
				break;
			case 12:
				if (lo_send(lo_client, address, pattern,
					f_argts[0], f_argts[1], f_argts[2], f_argts[3],
					f_argts[4], f_argts[5], f_argts[6], f_argts[7],
					f_argts[8], f_argts[9], f_argts[10], f_argts[11]) == -1) {
					sprintf_s(ErrorStr, string_length, "Error: OSC error %d: %s  for message %s\n", lo_address_errno(lo_client),
						lo_address_errstr(lo_client), address); ReportError(ErrorStr); throw(100);
				}
				break;
			case 13:
				if (lo_send(lo_client, address, pattern,
					f_argts[0], f_argts[1], f_argts[2], f_argts[3],
					f_argts[4], f_argts[5], f_argts[6], f_argts[7],
					f_argts[8], f_argts[9], f_argts[10], f_argts[11],
					f_argts[12]) == -1) {
					sprintf_s(ErrorStr, string_length, "Error: OSC error %d: %s  for message %s\n", lo_address_errno(lo_client),
						lo_address_errstr(lo_client), address); ReportError(ErrorStr); throw(100);
				}
				break;
			case 14:
				if (lo_send(lo_client, address, pattern,
					f_argts[0], f_argts[1], f_argts[2], f_argts[3],
					f_argts[4], f_argts[5], f_argts[6], f_argts[7],
					f_argts[8], f_argts[9], f_argts[10], f_argts[11],
					f_argts[12], f_argts[13]) == -1) {
					sprintf_s(ErrorStr, string_length, "Error: OSC error %d: %s  for message %s\n", lo_address_errno(lo_client),
						lo_address_errstr(lo_client), address); ReportError(ErrorStr); throw(100);
				}
				break;
			case 15:
				if (lo_send(lo_client, address, pattern,
					f_argts[0], f_argts[1], f_argts[2], f_argts[3],
					f_argts[4], f_argts[5], f_argts[6], f_argts[7],
					f_argts[8], f_argts[9], f_argts[10], f_argts[11],
					f_argts[12], f_argts[13], f_argts[14]) == -1) {
					sprintf_s(ErrorStr, string_length, "Error: OSC error %d: %s  for message %s\n", lo_address_errno(lo_client),
						lo_address_errstr(lo_client), address); ReportError(ErrorStr); throw(100);
				}
				break;
			case 16:
				if (lo_send(lo_client, address, pattern,
					f_argts[0], f_argts[1], f_argts[2], f_argts[3],
					f_argts[4], f_argts[5], f_argts[6], f_argts[7],
					f_argts[8], f_argts[9], f_argts[10], f_argts[11],
					f_argts[12], f_argts[13], f_argts[14], f_argts[15]) == -1) {
					sprintf_s(ErrorStr, string_length, "Error: OSC error %d: %s  for message %s\n", lo_address_errno(lo_client),
						lo_address_errstr(lo_client), address); ReportError(ErrorStr); throw(100);
				}
				break;
			case 17:
				if (lo_send(lo_client, address, pattern,
					f_argts[0], f_argts[1], f_argts[2], f_argts[3],
					f_argts[4], f_argts[5], f_argts[6], f_argts[7],
					f_argts[8], f_argts[9], f_argts[10], f_argts[11],
					f_argts[12], f_argts[13], f_argts[14], f_argts[15],
					f_argts[16]) == -1) {
					sprintf_s(ErrorStr, string_length, "Error: OSC error %d: %s  for message %s\n", lo_address_errno(lo_client),
						lo_address_errstr(lo_client), address); ReportError(ErrorStr); throw(100);
				}
				break;
			case 18:
				if (lo_send(lo_client, address, pattern,
					f_argts[0], f_argts[1], f_argts[2], f_argts[3],
					f_argts[4], f_argts[5], f_argts[6], f_argts[7],
					f_argts[8], f_argts[9], f_argts[10], f_argts[11],
					f_argts[12], f_argts[13], f_argts[14], f_argts[15],
					f_argts[16], f_argts[17]) == -1) {
					sprintf_s(ErrorStr, string_length, "Error: OSC error %d: %s  for message %s\n", lo_address_errno(lo_client),
						lo_address_errstr(lo_client), address); ReportError(ErrorStr); throw(100);
				}
				break;
			case 19:
				if (lo_send(lo_client, address, pattern,
					f_argts[0], f_argts[1], f_argts[2], f_argts[3],
					f_argts[4], f_argts[5], f_argts[6], f_argts[7],
					f_argts[8], f_argts[9], f_argts[10], f_argts[11],
					f_argts[12], f_argts[13], f_argts[14], f_argts[15],
					f_argts[16], f_argts[17], f_argts[18]) == -1) {
					sprintf_s(ErrorStr, string_length, "Error: OSC error %d: %s  for message %s\n", lo_address_errno(lo_client),
						lo_address_errstr(lo_client), address); ReportError(ErrorStr); throw(100);
				}
				// printf("send message address %s pattern %s\n", address, pattern);
				break;
			case 20:
				if (lo_send(lo_client, address, pattern,
					f_argts[0], f_argts[1], f_argts[2], f_argts[3],
					f_argts[4], f_argts[5], f_argts[6], f_argts[7],
					f_argts[8], f_argts[9], f_argts[10], f_argts[11],
					f_argts[12], f_argts[13], f_argts[14], f_argts[15],
					f_argts[16], f_argts[17], f_argts[18], f_argts[19]) == -1) {
					sprintf_s(ErrorStr, string_length, "Error: OSC error %d: %s  for message %s\n", lo_address_errno(lo_client),
						lo_address_errstr(lo_client), address); ReportError(ErrorStr); throw(100);
				}
				break;
			case 21:
				if (lo_send(lo_client, address, pattern,
					f_argts[0], f_argts[1], f_argts[2], f_argts[3],
					f_argts[4], f_argts[5], f_argts[6], f_argts[7],
					f_argts[8], f_argts[9], f_argts[10], f_argts[11],
					f_argts[12], f_argts[13], f_argts[14], f_argts[15],
					f_argts[16], f_argts[17], f_argts[18], f_argts[19],
					f_argts[20]) == -1) {
					sprintf_s(ErrorStr, string_length, "Error: OSC error %d: %s  for message %s\n", lo_address_errno(lo_client),
						lo_address_errstr(lo_client), address); ReportError(ErrorStr); throw(100);
				}
				break;
			default:
				// error: does not process message of more than one argument for the moment
				sprintf_s(ErrorStr, string_length, "Error: OSC output message with more thant one argument can only be from 2 to 20 float arguments %s (size %d)!", address, pattern_size); ReportError(ErrorStr); throw(100);
			}
		}
	}
	// no pattern, just an address
	else {
		if (lo_send(lo_client, address, NULL) == -1) {
			sprintf_s(ErrorStr, string_length, "Error: OSC error %d: %s  for message %s\n", lo_address_errno(lo_client),
				lo_address_errstr(lo_client), address); ReportError(ErrorStr); throw(100);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////
// UDP SERVER

// liblo OSC messages error handling
void liblo_error_handling(int num, const char *msg, const char *path) {
	sprintf_s(ErrorStr, string_length, "Error: liblo server error %d in path %s: %s\n", num, path, msg); ReportError(ErrorStr);
}

pg_IPServer::pg_IPServer(unsigned int PortNumber) {
	// local server port
	Local_server_port = PortNumber;

	// remote client address
	Remote_client_IP = NULL;
	Remote_client_port = 1980;
	Remote_client_host = NULL;

	// local server socket
	SocketToLocalServer = SOCKET(-1);
	Input_Message_Local_Commande_String
		= new char[max_network_message_length];

	// console trace
	IP_message_trace = false;

	// endian reversal
#if CPU_ENDIAN == big
	OSC_endian_reversal = false;
#else
	OSC_endian_reversal = true;
#endif
}

pg_IPServer::~pg_IPServer(void) {
	for (int ind = 0; ind < MAX_OSC_ARGUMENTS; ind++) {
		if (OSC_arguments[ind]) {
			delete[] OSC_arguments[ind];
		}
	}

#ifdef WIN32
	closesocket(SocketToLocalServer);
#endif
}

void pg_IPServer::InitServer(void) {
	///////////////////////////////
	// local server creation
	char Local_server_port_string[128];
	sprintf_s(Local_server_port_string, 128, "%d", Local_server_port);

	/* start a new server on port Local_server_port */
	lo_local_server = lo_server_new(Local_server_port_string, 
		liblo_error_handling);

	/* add method that will match any path and args */
	lo_server_add_method(lo_local_server, NULL, NULL, processLibloReceivedOSC, this);

	printf("Network server: listening on port %u\n", Local_server_port);

	for (int ind = 0; ind < MAX_OSC_ARGUMENTS; ind++) {
		OSC_arguments[ind] = new char[string_length];
		*(OSC_arguments[ind]) = 0;
	}
	OSCTag = new char[string_length];
}

// liblo catch any incoming messages and display them. returning 1 means that the
// message has not been fully handled and the server should try other methods
int processLibloReceivedOSC(const char *path, const char *types, lo_arg ** argv,
	int argc, void *data, void *user_data) {
	pg_IPServer *classInstance = (pg_IPServer *)user_data;
	char argument[string_length];

	// string initialization to 0
	memset(classInstance->Input_Message_Local_Commande_String, 0x0, max_network_message_length);

#ifndef LEAPMOTION
	// recording received command in recording mode of OSC_RecordReplay
	// possible initialization of recording time for the first received message
	if (InitialRecordTime == _NON_INITIALIZED_INITIAL_TIME) {
		InitialRecordTime = RealTime();
	}
	sprintf_s(classInstance->Input_Message_Local_Commande_String, max_network_message_length,
		"%f\t%s\t%d", RealTime() - InitialRecordTime, path, argc);
#else
	sprintf_s(classInstance->Input_Message_Local_Commande_String, max_network_message_length,
		"%f\t%s\t%d", RealTime(), path, argc);
#endif

	if (argc == 1) {
		if (types[0] == 'i') {
			strcat_s(classInstance->Input_Message_Local_Commande_String, max_network_message_length, "\ti\t");
			sprintf_s(argument, string_length, "%d", argv[0]->i);
			strcat_s(classInstance->Input_Message_Local_Commande_String, max_network_message_length, argument);
		}
		else if (types[0] == 'f') {
			strcat_s(classInstance->Input_Message_Local_Commande_String, max_network_message_length, "\ti\t");
			sprintf_s(argument, string_length, "%.5f", argv[0]->f);
			strcat_s(classInstance->Input_Message_Local_Commande_String, max_network_message_length, argument);
		}
		else if (types[0] == 's') {
			strcat_s(classInstance->Input_Message_Local_Commande_String, max_network_message_length, "\ts\t");
			char *argtString = (char *)argv[0];
			if ((*argtString) != '\"') {
				strcat_s(classInstance->Input_Message_Local_Commande_String, max_network_message_length, "\"");
				strcat_s(classInstance->Input_Message_Local_Commande_String, max_network_message_length, argtString);
				strcat_s(classInstance->Input_Message_Local_Commande_String, max_network_message_length, "\"");
			}
			else {
				strcat_s(classInstance->Input_Message_Local_Commande_String, max_network_message_length, argtString);
			}
		}
	}
	else {
		// float argument
		std::string expectedPattern = std::string(argc, 'f');
		if (expectedPattern.compare(std::string(types)) != 0) {
			sprintf_s(ErrorStr, string_length, "Error: unexpected %d character pattern (%s)\n", argc, types); ReportError(ErrorStr);

		}
		strcat_s(classInstance->Input_Message_Local_Commande_String, max_network_message_length, "\t");
		strcat_s(classInstance->Input_Message_Local_Commande_String, max_network_message_length, types);
		for (int indParam = 0; indParam < argc; indParam++) {
			strcat_s(classInstance->Input_Message_Local_Commande_String, max_network_message_length, "\t");
			sprintf_s(argument, string_length, "%.5f", argv[indParam]->f);
				strcat_s(classInstance->Input_Message_Local_Commande_String, max_network_message_length, argument);
		}
	}

	if (classInstance->IP_message_trace) 
	{
		printf("udp server: receive OSC message [%s]\n",
			classInstance->Input_Message_Local_Commande_String);
	}

#ifndef LEAPMOTION
	// recording received command in recording mode of OSC_RecordReplay
	if (currentActivity == recordOSC) {
		// records the command in the recording file
		fprintf(FILE_record, "%s\n", Leap_porphyrograph_Record_server->Input_Message_Local_Commande_String);

		// quitting received in recording mode makes the application loop
		if (strcmp(path, "/quit") == 0) {
			fclose(FILE_record);
			printf("Exiting record (quit command received)\n");
			exit(0);
		}
	}
	// executing TouchOSC command in replay mode of OSC_RecordReplay
	else if (currentActivity == replayOSC) {
		// playing in replay mode 
		if (strcmp(path, "/play") == 0) {
			isReplayOn = true;
			if (InitialReplayTime == _NON_INITIALIZED_INITIAL_TIME) {
				InitialReplayTime = RealTime();
			}
			else if(InitialPauseTime != _NON_INITIALIZED_INITIAL_TIME) {
				InitialReplayTime = InitialReplayTime + (RealTime() - InitialPauseTime);
				InitialPauseTime = _NON_INITIALIZED_INITIAL_TIME;
			}
		}
		// pausing in replay mode 
		else if (strcmp(path, "/pause") == 0) {
			isReplayOn = false;
			InitialPauseTime = RealTime();
		}
		// quitting in replay mode 
		else if (isReplayOn && strcmp(path, "/quit") == 0) {
			printf("Quitting replay (quit command received)\n");
			Porphyrograph_Replay_client->sendIPmessages("/quit", "", NULL, NULL, NULL);
			for (int i = 0; i < 4; i++) {
				std::string address = std::string("/new_scene_") + std::to_string(i);
				Usine_Replay_client->sendIPmessages(address.c_str(), "i", NULL, NULL, 0);
				millisleep(5);
			}
			exit(0);
		}
		// scene for Usine in replay mode 
		else if (isReplayOn && strcmp(path, "/scene_0") == 0
			|| isReplayOn && strcmp(path, "/scene_1") == 0
			|| isReplayOn && strcmp(path, "/scene_2") == 0
			|| isReplayOn && strcmp(path, "/scene_3") == 0) {
			int scene_no = int(path[7] - '0');
			printf("Usine scene %d\n", scene_no);
			for (int i = 0; i < 4; i++) {
				std::string address = std::string("/new_scene_") + std::to_string(i);
				if (i == scene_no) {
					Usine_Replay_client->sendIPmessages(address.c_str(), "i", NULL, NULL, 1);
				}
				else {
					Usine_Replay_client->sendIPmessages(address.c_str(), "i", NULL, NULL, 0);
				}
				millisleep(5);
			}
		}
		// scene for Usine in replay mode 
		else if (isReplayOn && strcmp(path, "/scene_null") == 0) {
			for (int i = 0; i < 4; i++) {
				std::string address = std::string("/new_scene_") + std::to_string(i);
				Usine_Replay_client->sendIPmessages(address.c_str(), "i", NULL, NULL, 0);
				millisleep(5);
			}
		}
		else if (isReplayOn && strcmp(path, "/setup_plus") == 0) {
			InitialPauseTime = ReplayCommand_timeStamp;
			while (ReplayCommand_OSCaddress.compare("/new_scene") != 0) {
				std::getline(FILE_ReplayCommand, ReplayCommandFile_line);
				std::stringstream  sstrem(ReplayCommandFile_line);
				// time stamp
				sstrem >> ReplayCommand_commandString;
				ReplayCommand_timeStamp = atof(ReplayCommand_commandString.c_str());
				// address
				sstrem >> ReplayCommand_OSCaddress;
			}
			InitialReplayTime = InitialReplayTime - (ReplayCommand_timeStamp - InitialPauseTime);
		}
	}
#endif

	return 1;
}


void pg_IPServer::receiveIPMessages(void) {
	while (lo_server_recv_noblock(lo_local_server, 0)) {
	}
}



