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

#define _SpaceChar(c) (c == ' ' || c == '\n' || c == 13 || c == '\t')
#define _Num(c) (c >= '0' && c <= '9')

// string splitting into string vector by single char
vector<string> pg_split_string(string str, char token);

// alternate plain/pg_enum_UDP_OSC message format
enum pg_UDPMessageFormat{ pg_enum_UDP_Plain = 0, pg_enum_UDP_OSC , pg_enum_Empty_UDPMessageFormat };

class pg_UDP_Transprt
{
public:
	int transport_recv(SOCKET serverSocket, void* buffer, size_t size_max);

private:
	char m_buffer[PG_MAX_NETWROK_MESSAGE_LENGTH];
};

// IP client (remote server)
class pg_IPClient {
public:
	// remote server ID
	string id;

	// remote sever address
	string              Remote_server_IP;
	unsigned int        Remote_server_port;
	struct hostent     *Remote_server_host;

	// remote server socket
	SOCKET SocketToRemoteServer;
	struct sockaddr_in remoteServAddr;

	// client stack: stores pending output messages
	// pg_enum_UDP_OSC pattern
	string *output_pattern_stack;
	// the message: string (pg_enum_UDP_Plain format), pg_enum_UDP_OSC encoding (pg_enum_UDP_OSC format)
	string *output_message_stack;

	int     max_depth_output_stack;
	int     current_depth_output_stack;
	float   maximal_IP_message_delay;
	double  last_IP_message_time;

	// emitted message format
	pg_UDPMessageFormat   send_format;

	unsigned int first_IP_message_number;
	unsigned int current_IP_message_number;

	// console trace
	bool   IP_message_trace;

	// byte order reversal
	bool   OSC_endian_reversal;

	pg_IPClient(void);
	~pg_IPClient(void);

	void InitClient(void);

	void IP_OutputStackInitialization(void);

	// send IP messages to IP client (pg_enum_UDP_Plain or pg_enum_UDP_OSC formats)
	size_t makePacket(void* buffer, size_t size);
	void sendIPmessages(void);

	// message processing
	void storeIP_output_message(char *commandLine,
		char *pattern);
};

// IP server (local server)
class pg_IPServer {
public:
	// local server ID
	string       id;

	// pg_enum_UDP_OSC argument parsing
	string OSC_arguments[PG_MAX_OSC_ARGUMENTS];
	string OSC_address;

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

	// server reception stack
	int            *input_argc_stack;
	string         *input_message_stack;
	int            *input_message_length_stack;
	int             depth_input_stack;
	int             current_depth_input_stack;

	// format: plain vs pg_enum_UDP_OSC
	pg_UDPMessageFormat   receive_format;

	unsigned int    last_received_IP_message_number;

	// console trace
	bool            IP_message_trace;

	// byte order reversal
	bool            OSC_endian_reversal;

	// duplicate pg_enum_UDP_OSC message removal
	bool            OSC_duplicate_removal;

	pg_IPServer(void);
	~pg_IPServer(void);

	void InitServer(void);

	void IP_InputStackInitialization(void);

	// message processing 
	// void processReceivedOSC(char *localCommandLine, char *message, int n);

	/// message processing
	void recvPacket(pg_UDP_Transprt* t);
	void receiveIPMessages(void);

	// pg_enum_UDP_OSC message decoding
	int handlePacket(const OSCPP::Server::Packet& packet);

	// stores IP messages before duplicate removal 
	// in case of duplicated messages (with possibly different 
	// numerical values), only the latest message is kept
	void storeIP_input_messages_and_removes_duplicates(char *message, int argc);

	// message interpretation according to alias script (predefined commands
	// or scenario variables assignment)
	void ProcessFilteredInputMessages(void);
};

pg_UDP_Transprt* pg_newUDP_Transport();

#endif
