/*! \file pg-udp.h
 * 
 * 
 *     Part of this source file is taken from Virtual Choreographer
 *     http://virchor.sourceforge.net/
 * 
 *     File pg-udp.h
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

#ifndef PG_UDP_H
#define PG_UDP_H

#ifndef _WIN32
typedef int SOCKET;
#endif // !_WIN32


#define MAX_OSC_ARGUMENTS 24

#define _SpaceChar(c) (c == ' ' || c == '\n' || c == 13 || c == '\t')
#define _Num(c) (c >= '0' && c <= '9')

// alternate plain/OSC message format
enum pg_UDPMessageFormat{ Plain = 0, OSC , Emptypg_UDPMessageFormat };

// IP client (remote server)
class pg_IPClient {
public:
	char *Output_Message_String = NULL;
	char *Output_Message_OSC = NULL;
	char *Output_Message_Pattern = NULL;
	char *Output_Message_ArgList = NULL;
	char ErrorStr[1024];

	// remote sever address
	std::string         Remote_server_IP;
	unsigned int        Remote_server_port;
	struct hostent     *Remote_server_host;

	// remote server socket
	SOCKET SocketToRemoteServer;
	struct sockaddr_in remoteServAddr;

	// liblo client
	lo_address lo_client;

	// emitted message format
	pg_UDPMessageFormat   send_format;

	pg_IPClient(char *IPaddress, unsigned int PortNumber);
	~pg_IPClient(void);

	void InitClient(void);

	// send IP messages to IP client (OSC formats)
	void sendIPmessages(const char *address, const char *pattern, const float *f_argts, const char *s_argt, const int i_argt);
	void sendOSCMessageFromLine(std::string line);
};

// IP server (local server)
// liblo message processing 
void liblo_error_handling(int num, const char *m, const char *path);
int processLibloReceivedOSC(const char *path, const char *types, lo_arg ** argv,
	int argc, void *data, void *user_data);

class pg_IPServer {
public:
	char *Input_Message_Local_Commande_String = NULL;

	// OSC argument parsing
	char *OSC_arguments[MAX_OSC_ARGUMENTS];
	char *OSCTag;

	// local server socket
	SOCKET              SocketToLocalServer;
	struct sockaddr_in  localServerAddr;

	// remote server socket
	SOCKET SocketToRemoteClient;
	struct sockaddr_in remoteClientAddr;

	// server address
	unsigned int        Local_server_port;
	char               *Remote_client_IP;
	unsigned int        Remote_client_port;
	struct hostent     *Remote_client_host;

	// liblo server
	lo_server           lo_local_server;

	// console trace
	bool            IP_message_trace;

	// byte order reversal
	bool            OSC_endian_reversal;

	pg_IPServer(unsigned int PortNumber);
	~pg_IPServer(void);

	void InitServer(void);

	/// message processing
	void receiveIPMessages(void);
};

#define   string_length   1024
#define   max_network_message_length 65535

extern char *ErrorStr;

extern pg_IPClient *Porphyrograph_Replay_client;
extern pg_IPClient *Usine_Replay_client;
extern pg_IPServer *Leap_porphyrograph_Record_server;
extern pg_IPServer *TouchOSC_Replay_server;
extern pg_IPClient *Leap_Usine_client;
extern pg_IPClient *Leap_Record_client;
extern FILE *FILE_record;

void millisleep(int milliseconds);
double RealTime(void);
void ReportError(char *errorString);

#endif
