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

#include "pg-all_include.h"

typedef  char *     Pchar;
std::array<char, max_network_message_length> UDP_buffer;
std::unique_ptr<UDP_Transport> udp_transport(newUDP_Transport());

// string splitting into string vector by single char
vector<std::string> split_string(string str, char token) {
	vector<std::string>result;
	while (str.size()) {
		int index = str.find(token);
		if (index != string::npos) {
			result.push_back(str.substr(0, index));
			str = str.substr(index + 1);
			if (str.size() == 0)result.push_back(str);
		}
		else {
			result.push_back(str);
			str = "";
		}
	}
	return result;
}


/////////////////////////////////////////////////////////////////////////////////
// UDP CLIENT

pg_IPClient::pg_IPClient( void ) {
  id = "";
  
  // remote server address
  Remote_server_IP = "";
  Remote_server_port = 1979;
  Remote_server_host = NULL;

  // remote server socket
  SocketToRemoteServer = SOCKET(-1);

  // client stack: stores pending output messages
  output_pattern_stack = NULL;
  output_message_stack = NULL;
  max_depth_output_stack = 10;
  current_depth_output_stack = 0;
  maximal_IP_message_delay = 100.f; // ms between two bursts of UDP messages emmissions
  last_IP_message_time = 0;

  // output message format
  send_format = Plain;

  // console trace
  IP_message_trace = false;

  // byte order reversal
#if CPU_ENDIAN == little
  OSC_endian_reversal = false;
#else
  OSC_endian_reversal = true;
#endif

}
    
pg_IPClient::~pg_IPClient(void) {
	id.clear();

	if (output_message_stack) {
		delete[] output_message_stack;
	}
	output_message_stack = NULL;


	if (output_pattern_stack) {
		delete[] output_pattern_stack;
	}
	output_pattern_stack = NULL;

#ifdef WIN32
	//if (send_format != OSC) {
		closesocket(SocketToRemoteServer);
	//}
	//else {
	//	lo_address_free(lo_client);
	//}
#endif
}

void pg_IPClient::InitClient(void) {

	//////////////////////////////
	// remote server opening

	struct                  addrinfo* result;
	struct                  addrinfo hints;

	char Remote_server_port_string[128];
	sprintf(Remote_server_port_string, "%d", Remote_server_port);

	memset(&hints, 0, sizeof hints); // make sure the struct is empty
	hints.ai_family = AF_INET;     // don't care IPv4 or IPv6
	hints.ai_socktype = SOCK_DGRAM; // UDP stream sockets
	int err = getaddrinfo(Remote_server_IP.c_str(), Remote_server_port_string, &hints, &result);
	if (err != 0) {
		sprintf(ErrorStr, "Error: unknown remote host IP '%s (%d)'!", Remote_server_IP.c_str(), err); ReportError(ErrorStr);
	}
	else {
		char                    hname[1024];
		err = getnameinfo(
			result->ai_addr,             // Pointer to your struct sockaddr
			socklen_t(result->ai_addrlen),          // Size of this struct
			hname,                       // Pointer to hostname string
			sizeof hname,                // Size of this string
			NULL,                        // Pointer to service name string
			0,                           // Size of this string
			0                            // No flags given
		);
		if (err != 0) {
			sprintf(ErrorStr, "Error: unknown remote host IP '%s (%d)'!", Remote_server_IP.c_str(), err); ReportError(ErrorStr);
		}
		else {
			/* get server IP address (no check if input is IP address or DNS name */
			// deprecated Remote_server_host = gethostbyname(Remote_server_IP.c_str());
			memset(&remoteServAddr, 0, sizeof(remoteServAddr));
			remoteServAddr.sin_family = AF_INET;
			inet_pton(AF_INET, hname, &(remoteServAddr.sin_addr));
			remoteServAddr.sin_port = htons(u_short(Remote_server_port));

			printf("Network client '%s': sending data to '%s' (%s) on port %d\n",
				id.c_str(), Remote_server_IP.c_str(), hname, Remote_server_port);

			/* socket creation */
			//memcpy((char *) &remoteServAddr.sin_addr.s_addr, 
			//Remote_server_host->h_addr_list[0], Remote_server_host->h_length);

			SocketToRemoteServer = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
			if (SocketToRemoteServer < 0) {
				sprintf(ErrorStr, "Error: cannot open socket to remote server!"); ReportError(ErrorStr);
			}
			err = connect(SocketToRemoteServer, result->ai_addr, int(result->ai_addrlen));
			if (err == -1) {
				sprintf(ErrorStr, "Error: cannot open socket to remote server!"); ReportError(ErrorStr);
			}
			// listen(SocketToRemoteServer, 200);
		}
	}

	// system initializing
	IP_OutputStackInitialization();
}

void pg_IPClient::IP_OutputStackInitialization( void ) {
  // output message stack initializing
  output_message_stack = new string[max_depth_output_stack];
  for(int ind_stack = 0 ; ind_stack < max_depth_output_stack ; ind_stack++ ) {
    output_message_stack[ ind_stack ] = "";
  }
  // output pattern stack initializing
  output_pattern_stack = new string[max_depth_output_stack];
  for(int ind_stack = 0 ; ind_stack < max_depth_output_stack ; ind_stack++ ) {
    output_pattern_stack[ ind_stack ] = "";
  }
  current_depth_output_stack = 0;
}

size_t pg_IPClient::makePacket(void* buffer, size_t size)
{
	// Construct a packet
	OSCPP::Client::Packet OSC_packet(buffer, size);

	// OSC message bundle
	if (output_message_stack[current_depth_output_stack - 1][0] == '#') {
		// splits the argument separated by space chars
		std::vector<std::string> message_arguments;
		message_arguments = split_string(output_message_stack[current_depth_output_stack - 1], ' ');

		// sends OSC message through OSC bundle
		// message that begins by an OSC address #/aaaa/aaa 
		// whether or not the pattern is provided with the command
		unsigned int pattern_size = output_pattern_stack[current_depth_output_stack - 1].length();

		//lo_bundle osc_bundle = lo_bundle_new(LO_TT_IMMEDIATE);
		//lo_message message_osc = lo_message_new();

		if (pattern_size == message_arguments.size() - 1) {
			// single argument
			if (pattern_size == 1) {
				switch (output_pattern_stack[current_depth_output_stack - 1][0]) {
				case 's':
					OSC_packet
						// Open a bundle with a timetag
						.openBundle(1234ULL)
						// Add a message address
						.openMessage(message_arguments[0].c_str(), 1)
						// Write the arguments
						.string(message_arguments[1].c_str())
						.closeMessage()
						.closeBundle();
					break;
				case 'i':
					OSC_packet
						// Open a bundle with a timetag
						.openBundle(1234ULL)
						// Add a message address
						.openMessage(message_arguments[0].c_str(), 1)
						// Write the arguments
						.int32(stoi(message_arguments[1]))
						.closeMessage()
						.closeBundle();
					break;
				case 'f':
					OSC_packet
						// Open a bundle with a timetag
						.openBundle(1234ULL)
						// Add a message address
						.openMessage(message_arguments[0].c_str(), 1)
						// Write the arguments
						.float32(stof(message_arguments[1]))
						.closeMessage()
						.closeBundle();
					break;
				default:
					sprintf(ErrorStr, "Error: unknown OSC pattern element %c for output message %s!", output_pattern_stack[current_depth_output_stack - 1][0], output_message_stack[current_depth_output_stack - 1].c_str()); ReportError(ErrorStr);
					break;
				}
			}
			//lo_bundle_free(osc_bundle);
			//lo_message_free(message_osc);
		}
	}
	// OSC message bundle

	// OSC message not bundle
	else if (output_message_stack[current_depth_output_stack - 1][0] == '/') {
		// splits the argument separated by space chars
		std::vector<std::string> message_arguments;
		message_arguments = split_string(output_message_stack[current_depth_output_stack - 1], ' ');

		// message that begins by an OSC address /aaaa/aaa 
		// msg with more than one argument
		if (output_pattern_stack[current_depth_output_stack - 1] != "") {
			unsigned int pattern_size = output_pattern_stack[current_depth_output_stack - 1].length();

			// pattern and argument size match
			if (pattern_size == message_arguments.size() - 1) {
				// single argument
				if (pattern_size == 1) {
					switch (output_pattern_stack[current_depth_output_stack - 1][0]) {
					case 's':
						OSC_packet
							// Add a message address
							.openMessage(message_arguments[0].c_str(), 1)
							// Write the arguments
							.string(message_arguments[1].c_str())
							.closeMessage();
						break;
					case 'i':
						OSC_packet
							// Add a message address
							.openMessage(message_arguments[0].c_str(), 1)
							// Write the arguments
							.int32(stoi(message_arguments[1]))
							.closeMessage();
						break;
					case 'f':
						OSC_packet
							// Add a message address
							.openMessage(message_arguments[0].c_str(), 1)
							// Write the arguments
							.float32(stof(message_arguments[1]))
							.closeMessage();
						break;
					default:
						sprintf(ErrorStr, "Error: unknown OSC pattern element %c for output message %s!", output_pattern_stack[current_depth_output_stack - 1][0], output_message_stack[current_depth_output_stack - 1].c_str()); ReportError(ErrorStr);
						break;
					}
				}
				else {
					if (output_pattern_stack[current_depth_output_stack - 1] == "ff") {
						OSC_packet
							.openMessage(message_arguments[0].c_str(), 2)
							.float32(stof(message_arguments[1]))
							.float32(stof(message_arguments[2]))
							.closeMessage();
					}
					else if (output_pattern_stack[current_depth_output_stack - 1] == "sff") {
						OSC_packet
							.openMessage(message_arguments[0].c_str(), 3)
							.string(message_arguments[1].c_str())
							.float32(stof(message_arguments[2]))
							.float32(stof(message_arguments[3]))
							.closeMessage();
					}
					else if (output_pattern_stack[current_depth_output_stack - 1] == "fff") {
						OSC_packet
							.openMessage(message_arguments[0].c_str(), 3)
							.float32(stof(message_arguments[1]))
							.float32(stof(message_arguments[2]))
							.float32(stof(message_arguments[3]))
							.closeMessage();
					}
					else if (output_pattern_stack[current_depth_output_stack - 1] == "ffff") {
						OSC_packet
							.openMessage(message_arguments[0].c_str(), 4)
							.float32(stof(message_arguments[1]))
							.float32(stof(message_arguments[2]))
							.float32(stof(message_arguments[3]))
							.float32(stof(message_arguments[4]))
							.closeMessage();
					}
					else if (output_pattern_stack[current_depth_output_stack - 1] == "fffff") {
						OSC_packet
							.openMessage(message_arguments[0].c_str(), 5)
							.float32(stof(message_arguments[1]))
							.float32(stof(message_arguments[2]))
							.float32(stof(message_arguments[3]))
							.float32(stof(message_arguments[4]))
							.float32(stof(message_arguments[5]))
							.closeMessage();
					}
					else if (output_pattern_stack[current_depth_output_stack - 1] == "ffffff") {
						OSC_packet
							.openMessage(message_arguments[0].c_str(), 6)
							.float32(stof(message_arguments[1]))
							.float32(stof(message_arguments[2]))
							.float32(stof(message_arguments[3]))
							.float32(stof(message_arguments[4]))
							.float32(stof(message_arguments[5]))
							.float32(stof(message_arguments[6]))
							.closeMessage();
					}
					else if (output_pattern_stack[current_depth_output_stack - 1] == "fffffff") {
						OSC_packet
							.openMessage(message_arguments[0].c_str(), 7)
							.float32(stof(message_arguments[1]))
							.float32(stof(message_arguments[2]))
							.float32(stof(message_arguments[3]))
							.float32(stof(message_arguments[4]))
							.float32(stof(message_arguments[5]))
							.float32(stof(message_arguments[6]))
							.float32(stof(message_arguments[7]))
							.closeMessage();
					}
					else if (output_pattern_stack[current_depth_output_stack - 1] == "ffffffff") {
						OSC_packet
							.openMessage(message_arguments[0].c_str(), 8)
							.float32(stof(message_arguments[1]))
							.float32(stof(message_arguments[2]))
							.float32(stof(message_arguments[3]))
							.float32(stof(message_arguments[4]))
							.float32(stof(message_arguments[5]))
							.float32(stof(message_arguments[6]))
							.float32(stof(message_arguments[7]))
							.float32(stof(message_arguments[8]))
							.closeMessage();
					}
					else if (output_pattern_stack[current_depth_output_stack - 1] == "fffffffff") {
						OSC_packet
							.openMessage(message_arguments[0].c_str(), 9)
							.float32(stof(message_arguments[1]))
							.float32(stof(message_arguments[2]))
							.float32(stof(message_arguments[3]))
							.float32(stof(message_arguments[4]))
							.float32(stof(message_arguments[5]))
							.float32(stof(message_arguments[6]))
							.float32(stof(message_arguments[7]))
							.float32(stof(message_arguments[8]))
							.float32(stof(message_arguments[9]))
							.closeMessage();
					}
					else if (output_pattern_stack[current_depth_output_stack - 1] == "ffffffffff") {
						OSC_packet
							.openMessage(message_arguments[0].c_str(), 10)
							.float32(stof(message_arguments[1]))
							.float32(stof(message_arguments[2]))
							.float32(stof(message_arguments[3]))
							.float32(stof(message_arguments[4]))
							.float32(stof(message_arguments[5]))
							.float32(stof(message_arguments[6]))
							.float32(stof(message_arguments[7]))
							.float32(stof(message_arguments[8]))
							.float32(stof(message_arguments[9]))
							.float32(stof(message_arguments[10]))
							.closeMessage();
					}
					else if (output_pattern_stack[current_depth_output_stack - 1] == "fffffffffff") {
						OSC_packet
							.openMessage(message_arguments[0].c_str(), 11)
							.float32(stof(message_arguments[1]))
							.float32(stof(message_arguments[2]))
							.float32(stof(message_arguments[3]))
							.float32(stof(message_arguments[4]))
							.float32(stof(message_arguments[5]))
							.float32(stof(message_arguments[6]))
							.float32(stof(message_arguments[7]))
							.float32(stof(message_arguments[8]))
							.float32(stof(message_arguments[9]))
							.float32(stof(message_arguments[10]))
							.float32(stof(message_arguments[11]))
							.closeMessage();
					}
					else if (output_pattern_stack[current_depth_output_stack - 1] == "ffffffffffff") {
						OSC_packet
							.openMessage(message_arguments[0].c_str(), 12)
							.float32(stof(message_arguments[1]))
							.float32(stof(message_arguments[2]))
							.float32(stof(message_arguments[3]))
							.float32(stof(message_arguments[4]))
							.float32(stof(message_arguments[5]))
							.float32(stof(message_arguments[6]))
							.float32(stof(message_arguments[7]))
							.float32(stof(message_arguments[8]))
							.float32(stof(message_arguments[9]))
							.float32(stof(message_arguments[10]))
							.float32(stof(message_arguments[11]))
							.float32(stof(message_arguments[12]))
							.closeMessage();
					}
					else if (output_pattern_stack[current_depth_output_stack - 1] == "fffffffffffff") {
						OSC_packet
							.openMessage(message_arguments[0].c_str(), 13)
							.float32(stof(message_arguments[1]))
							.float32(stof(message_arguments[2]))
							.float32(stof(message_arguments[3]))
							.float32(stof(message_arguments[4]))
							.float32(stof(message_arguments[5]))
							.float32(stof(message_arguments[6]))
							.float32(stof(message_arguments[7]))
							.float32(stof(message_arguments[8]))
							.float32(stof(message_arguments[9]))
							.float32(stof(message_arguments[10]))
							.float32(stof(message_arguments[11]))
							.float32(stof(message_arguments[12]))
							.float32(stof(message_arguments[13]))
							.closeMessage();
					}
					else if (output_pattern_stack[current_depth_output_stack - 1] == "ffffffffffffff") {
						OSC_packet
							.openMessage(message_arguments[0].c_str(), 14)
							.float32(stof(message_arguments[1]))
							.float32(stof(message_arguments[2]))
							.float32(stof(message_arguments[3]))
							.float32(stof(message_arguments[4]))
							.float32(stof(message_arguments[5]))
							.float32(stof(message_arguments[6]))
							.float32(stof(message_arguments[7]))
							.float32(stof(message_arguments[8]))
							.float32(stof(message_arguments[9]))
							.float32(stof(message_arguments[10]))
							.float32(stof(message_arguments[11]))
							.float32(stof(message_arguments[12]))
							.float32(stof(message_arguments[13]))
							.float32(stof(message_arguments[14]))
							.closeMessage();
					}
					else if (output_pattern_stack[current_depth_output_stack - 1] == "fffffffffffffff") {
						OSC_packet
							.openMessage(message_arguments[0].c_str(), 15)
							.float32(stof(message_arguments[1]))
							.float32(stof(message_arguments[2]))
							.float32(stof(message_arguments[3]))
							.float32(stof(message_arguments[4]))
							.float32(stof(message_arguments[5]))
							.float32(stof(message_arguments[6]))
							.float32(stof(message_arguments[7]))
							.float32(stof(message_arguments[8]))
							.float32(stof(message_arguments[9]))
							.float32(stof(message_arguments[10]))
							.float32(stof(message_arguments[11]))
							.float32(stof(message_arguments[12]))
							.float32(stof(message_arguments[13]))
							.float32(stof(message_arguments[14]))
							.float32(stof(message_arguments[15]))
							.closeMessage();
					}
					else if (output_pattern_stack[current_depth_output_stack - 1] == "ffffffffffffffff") {
						OSC_packet
							.openMessage(message_arguments[0].c_str(), 16)
							.float32(stof(message_arguments[1]))
							.float32(stof(message_arguments[2]))
							.float32(stof(message_arguments[3]))
							.float32(stof(message_arguments[4]))
							.float32(stof(message_arguments[5]))
							.float32(stof(message_arguments[6]))
							.float32(stof(message_arguments[7]))
							.float32(stof(message_arguments[8]))
							.float32(stof(message_arguments[9]))
							.float32(stof(message_arguments[10]))
							.float32(stof(message_arguments[11]))
							.float32(stof(message_arguments[12]))
							.float32(stof(message_arguments[13]))
							.float32(stof(message_arguments[14]))
							.float32(stof(message_arguments[15]))
							.float32(stof(message_arguments[16]))
							.closeMessage();
					}
					else if (output_pattern_stack[current_depth_output_stack - 1] == "fffffffffffffffff") {
						OSC_packet
							.openMessage(message_arguments[0].c_str(), 17)
							.float32(stof(message_arguments[1]))
							.float32(stof(message_arguments[2]))
							.float32(stof(message_arguments[3]))
							.float32(stof(message_arguments[4]))
							.float32(stof(message_arguments[5]))
							.float32(stof(message_arguments[6]))
							.float32(stof(message_arguments[7]))
							.float32(stof(message_arguments[8]))
							.float32(stof(message_arguments[9]))
							.float32(stof(message_arguments[10]))
							.float32(stof(message_arguments[11]))
							.float32(stof(message_arguments[12]))
							.float32(stof(message_arguments[13]))
							.float32(stof(message_arguments[14]))
							.float32(stof(message_arguments[15]))
							.float32(stof(message_arguments[16]))
							.float32(stof(message_arguments[17]))
							.closeMessage();
					}
					else if (output_pattern_stack[current_depth_output_stack - 1] == "ffffffffffffffffff") {
						OSC_packet
							.openMessage(message_arguments[0].c_str(), 18)
							.float32(stof(message_arguments[1]))
							.float32(stof(message_arguments[2]))
							.float32(stof(message_arguments[3]))
							.float32(stof(message_arguments[4]))
							.float32(stof(message_arguments[5]))
							.float32(stof(message_arguments[6]))
							.float32(stof(message_arguments[7]))
							.float32(stof(message_arguments[8]))
							.float32(stof(message_arguments[9]))
							.float32(stof(message_arguments[10]))
							.float32(stof(message_arguments[11]))
							.float32(stof(message_arguments[12]))
							.float32(stof(message_arguments[13]))
							.float32(stof(message_arguments[14]))
							.float32(stof(message_arguments[15]))
							.float32(stof(message_arguments[16]))
							.float32(stof(message_arguments[17]))
							.float32(stof(message_arguments[18]))
							.closeMessage();
					}
					else if (output_pattern_stack[current_depth_output_stack - 1] == "fffffffffffffffffff") {
						OSC_packet
							.openMessage(message_arguments[0].c_str(), 19)
							.float32(stof(message_arguments[1]))
							.float32(stof(message_arguments[2]))
							.float32(stof(message_arguments[3]))
							.float32(stof(message_arguments[4]))
							.float32(stof(message_arguments[5]))
							.float32(stof(message_arguments[6]))
							.float32(stof(message_arguments[7]))
							.float32(stof(message_arguments[8]))
							.float32(stof(message_arguments[9]))
							.float32(stof(message_arguments[10]))
							.float32(stof(message_arguments[11]))
							.float32(stof(message_arguments[12]))
							.float32(stof(message_arguments[13]))
							.float32(stof(message_arguments[14]))
							.float32(stof(message_arguments[15]))
							.float32(stof(message_arguments[16]))
							.float32(stof(message_arguments[17]))
							.float32(stof(message_arguments[18]))
							.float32(stof(message_arguments[19]))
							.closeMessage();
					}
					else if (output_pattern_stack[current_depth_output_stack - 1] == "ffffffffffffffffffff") {
						OSC_packet
							.openMessage(message_arguments[0].c_str(), 20)
							.float32(stof(message_arguments[1]))
							.float32(stof(message_arguments[2]))
							.float32(stof(message_arguments[3]))
							.float32(stof(message_arguments[4]))
							.float32(stof(message_arguments[5]))
							.float32(stof(message_arguments[6]))
							.float32(stof(message_arguments[7]))
							.float32(stof(message_arguments[8]))
							.float32(stof(message_arguments[9]))
							.float32(stof(message_arguments[10]))
							.float32(stof(message_arguments[11]))
							.float32(stof(message_arguments[12]))
							.float32(stof(message_arguments[13]))
							.float32(stof(message_arguments[14]))
							.float32(stof(message_arguments[15]))
							.float32(stof(message_arguments[16]))
							.float32(stof(message_arguments[17]))
							.float32(stof(message_arguments[18]))
							.float32(stof(message_arguments[19]))
							.float32(stof(message_arguments[20]))
							.closeMessage();
					}
					else {
						// error: does not process message of more than one argument for the moment
						sprintf(ErrorStr, "Error: OSC output message with more than one argument can only be from 2 to 20 float arguments %s (size %d)!", output_message_stack[current_depth_output_stack - 1].c_str(), pattern_size); ReportError(ErrorStr);
					}
				}
			}
			// pattern and argument size match
			// error : pattern and message size differ
			else {
				sprintf(ErrorStr, "Error: OSC pattern and message size differ %s / %s (pattern size %d message size %lu)!", output_pattern_stack[current_depth_output_stack - 1].c_str(), output_message_stack[current_depth_output_stack - 1].c_str(), pattern_size, message_arguments.size() - 1); ReportError(ErrorStr);
			}
			// error : pattern and message size differ
		}
		// msg with more than one argument
		// msg with 0 argument
		else {
			OSC_packet
				// Add a message address
				.openMessage(message_arguments[0].c_str(), 0)
				.closeMessage();
		}
		// msg with 0 argument
	}
	// OSC message not bundle

	return OSC_packet.size(); }

void pg_IPClient::sendIPmessages(void) {
	if (SocketToRemoteServer < 0) {
		return;
	}

	int nb_sent_messages = 0;
	// messages are sent by bursts to avoir overflowing the client with more messages than it can process at a time
	while (current_depth_output_stack > 0 && ++nb_sent_messages < 50) {
		// printf( "sendUDPmessage %d\n" , current_depth_output_stack );

		// if the elapsed time since the last message is lower than the maximal delay
		double current_time = RealTime();
		if (((current_time - last_IP_message_time) / 1000.) < maximal_IP_message_delay) {
			if (IP_message_trace) {
				printf("send message [%s, %s]\n", output_message_stack[current_depth_output_stack - 1].c_str(), output_pattern_stack[current_depth_output_stack - 1].c_str());
			}

			// records the emission time
			last_IP_message_time = RealTime();

			// sends the message
			int length;
			int localCommandLineLength = 0;
			// emitted packet size
			int rc = 0;

			// Plain format
			// formats the command
			// pads the length to a multiple of 4
			if (send_format == Plain) {
				strncpy(Output_Message_String, output_message_stack[current_depth_output_stack - 1].c_str(), max_network_message_length - 1);

				length = int(strlen(Output_Message_String));
				int inchar;
				for (inchar = length; inchar < length + (12 - length % 4) - 1;
					inchar++) {
					Output_Message_String[inchar] = ' ';
				}
				// pure data
				Output_Message_String[length + (12 - length % 4) - 2] = ';';
				Output_Message_String[length + (12 - length % 4) - 1] = '\x0a';
				Output_Message_String[length + (12 - length % 4)] = 0;
				localCommandLineLength = length + (12 - length % 4);
				// pure data

				rc = send(SocketToRemoteServer, Output_Message_String, localCommandLineLength, 0);
				//(struct sockaddr *) &remoteServAddr, 
				//sizeof(remoteServAddr));
				if (rc != localCommandLineLength) {
					sprintf(ErrorStr, "Error: udp client: data incompletely sent [%s] (%d/%d)!", Output_Message_String, rc, localCommandLineLength); ReportError(ErrorStr); // close(SocketToRemoteServer); throw 273;
				}
				if (rc > 0 && IP_message_trace) {
					printf("Network client: send string [%s] (%d bytes)\n",
						Output_Message_String, (int)strlen(Output_Message_String));
				}
			}
			// OSC format
			else if(send_format == OSC) {
				const size_t packetSize = makePacket(Output_Message_String, max_network_message_length);

				rc = send(SocketToRemoteServer, Output_Message_String, (int)packetSize, 0);
				//(struct sockaddr *) &remoteServAddr, 
				//sizeof(remoteServAddr));
				if (rc != (int)packetSize) {
					sprintf(ErrorStr, "Error: udp client: data incompletely sent [%s] (%d/%d)!", Output_Message_String, rc, (int)packetSize); ReportError(ErrorStr); // close(SocketToRemoteServer); throw 273;
				}
				if (rc > 0 && IP_message_trace) {
					printf("Network client: send string [%s] (%d bytes)\n",Output_Message_String, (int)packetSize);
				}
			}

			// stack depiling
			//printf("msg emmission stack size %d (%s,%s)\n", current_depth_output_stack, output_message_stack[current_depth_output_stack - 1].c_str(), output_pattern_stack[current_depth_output_stack - 1].c_str());
			current_depth_output_stack--;
		}
		// if the elapsed time since the last message is lower than the maximal delay
		// Higher than the maximal delay
		else {
			if (IP_message_trace) {
				printf("udp client: delay %.5f / %.5f \n",
					((current_time - last_IP_message_time) / 1000.),
					maximal_IP_message_delay);
			}
			return;
		}
		// Higher than the maximal delay
	}
	// messages are sent by bursts to avoir overflowing the client with more messages than it can process at a time
}

void pg_IPClient::storeIP_output_message(char *commandLine, char *pattern) {

	// printf( "commandLine %s %d\n" , commandLine , current_depth_output_stack );
	if (SocketToRemoteServer < 0) {
		return;
	}

	// printf( "commandLine %s %d\n" , commandLine , current_depth_output_stack );
	if (current_depth_output_stack < max_depth_output_stack - 1) {
		output_message_stack[current_depth_output_stack] = string(commandLine);
		if (pattern && *pattern) {
			output_pattern_stack[current_depth_output_stack] = string(pattern);
		}
		else {
			output_pattern_stack[current_depth_output_stack] = "";
		}
		current_depth_output_stack++;
		//printf("msg storage stack size %d (%s,%s)\n", current_depth_output_stack, commandLine, pattern);
	}
	else {
		sprintf(ErrorStr, "Error: UDP output message stack overflow %d %d (%s) to IP %s:%d!", 
			current_depth_output_stack, max_depth_output_stack, commandLine, Remote_server_IP.c_str(), Remote_server_port); ReportError(ErrorStr);
		current_depth_output_stack = 0;
	}
}

/////////////////////////////////////////////////////////////////////////////////
// UDP SERVER

pg_IPServer::pg_IPServer( void ) {
  // local server ID
  id = "";
  
  // local server port
  Local_server_port = 1979;

  // remote client address
  Remote_client_IP = NULL;
  Remote_client_port = 1980;
  Remote_client_host = NULL;

  // local server socket
  SocketToLocalServer = SOCKET(-1);

  // server stack: stores received input messages
  input_message_stack = NULL;
  input_argc_stack = NULL;
  input_message_length_stack = NULL;
  depth_input_stack = 30;
  current_depth_input_stack = 0;
 
  // received message format
  receive_format = Plain;

  // acknowledgement of received message x
  last_received_IP_message_number = INT_MAX;

  // console trace
  IP_message_trace = false;

  // endian reversal
#if CPU_ENDIAN == big
  OSC_endian_reversal = false;
#else
  OSC_endian_reversal = true;
#endif

  // duplicate OSC message removal
  OSC_duplicate_removal = true;
}

pg_IPServer::~pg_IPServer(void) {
	id.clear();

	if (input_message_stack) {
		delete[] input_message_stack;
		input_message_stack = NULL;
	}
	if (input_argc_stack) {
		delete[] input_argc_stack;
		input_argc_stack = NULL;
	}

	if (input_message_length_stack) {
		delete[] input_message_length_stack;
		input_message_length_stack = NULL;
	}

#ifdef WIN32
	closesocket(SocketToLocalServer);
#endif
}

void pg_IPServer::InitServer(void) {
	///////////////////////////////
	// local server creation
	struct sockaddr_in localServAddr;

#ifndef _WIN32
	/* socket creation */
	SocketToLocalServer = socket(AF_INET, SOCK_DGRAM, 0);

	if (SocketToLocalServer < 0) {
		sprintf(ErrorStr, "Error: cannot open socket to local server!"); ReportError(ErrorStr);
	}
	else {
		int SocketToLocalServerFlags;

		// Read the socket's flags
		SocketToLocalServerFlags = fcntl(SocketToLocalServer, F_GETFL, 0);
		// Sets the socket's flags to non-blocking
		SocketToLocalServerFlags |= O_NONBLOCK;
		int ret = fcntl(SocketToLocalServer, F_SETFL, SocketToLocalServerFlags);
		if (ret < 0) {
			sprintf(ErrorStr, "Error: local server cannot set flag to non-blocking: %s!", strerror(errno)); ReportError(ErrorStr);
		}
	}
#else
	/* socket creation */
	SocketToLocalServer = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (SocketToLocalServer < 0) {
		sprintf(ErrorStr, "Error: udp server cannot open socket to local server!"); ReportError(ErrorStr);
	}
	else {
		// Read the socket's flags
		unsigned long onoff = 1;

		if (ioctlsocket(SocketToLocalServer, FIONBIO, &onoff) != 0) {
			sprintf(ErrorStr, "Error: udp server cannot set flag to non-blocking: %s!", strerror(errno)); ReportError(ErrorStr);
		}
	}
#endif

	/* bind local server port */
	localServAddr.sin_family = AF_INET;
	localServAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	localServAddr.sin_port = htons(u_short(Local_server_port));

	int rc = bind(SocketToLocalServer, (struct sockaddr*)&localServAddr,
		sizeof(localServAddr));
	if (rc < 0) {
		sprintf(ErrorStr, "Error: cannot bind local port number %d!", Local_server_port); ReportError(ErrorStr);
		return;
	}

	printf("Network server: listening on port %u\n", Local_server_port);

	// message stack initialization
	IP_InputStackInitialization();

	for (int ind = 0; ind < MAX_OSC_ARGUMENTS; ind++) {
		OSC_arguments[ind] = "";
	}
}

void pg_IPServer::IP_InputStackInitialization(void) {
	// input message stack initializing
	input_argc_stack = new int[depth_input_stack];
	input_message_stack = new string[depth_input_stack];
	input_message_length_stack = new int[depth_input_stack];
	for (int ind_stack = 0; ind_stack < depth_input_stack;
		ind_stack++) {
		input_argc_stack[ind_stack] = -1;
		input_message_stack[ind_stack] = "";
		input_message_length_stack[ind_stack] = 0;
	}
	current_depth_input_stack = 0;
}


UDP_Transport* newUDP_Transport()
{
	return new UDP_Transport;
}

int UDP_Transport::transport_recv(SOCKET serverSocket, void* buffer, size_t size_max) {
	int n = recv(serverSocket, m_buffer, size_max, 0);
	if (n > 0) {
		std::memcpy(buffer, m_buffer, n);
		return n;
	}
	else {
		return 0;
	}
}

int pg_IPServer::handlePacket(const OSCPP::Server::Packet& packet) {
	char argument[StringLength];
	if (packet.isBundle()) {
		// Convert to bundle
		OSCPP::Server::Bundle bundle(packet);

		// Print the time
		std::cout << "#bundle " << bundle.time() << std::endl;

		// Get packet stream
		OSCPP::Server::PacketStream packets(bundle.packets());

		// Iterate over all the packets and call handlePacket recursively.
		// Cuidado: Might lead to stack overflow!
		while (!packets.atEnd()) {
			return handlePacket(packets.next());
		}
	}
	else {
		// Convert to message
		OSCPP::Server::Message msg(packet);
		// Get argument stream
		OSCPP::Server::ArgStream args(msg.args());
		// copies the address
		strcpy(Input_Message_Local_Commande_String, msg.address());

		int argc = args.size();
		//printf("OSC message with %d arguments\n", argc);
		int indParam = 0;
		while (!args.atEnd()) {
			indParam++;
			strcat(Input_Message_Local_Commande_String, " ");
			char tag = args.tag();
			if (tag == 'i') {
				sprintf(argument, "%d", args.int32());
				//printf("i argument #%d [%s]\n", indParam, argument);
				strcat(Input_Message_Local_Commande_String, argument);
			}
			else if (tag == 'f') {
					// printf("f argument #%d [%s] at %d %d\n", indParam + 1 , argument , indChar , sizeof( float ) ); 
				sprintf(argument, "%.5f", args.float32());
				//printf("f argument #%d [%s]\n", indParam, argument);
				strcat(Input_Message_Local_Commande_String, argument);
			}
			else if (tag == 's') {
				const char* argtString = args.string();
				// printf("s argument #%d [%s] at %d\n", indParam + 1 , message + indChar , indChar );
				if ((*argtString) != '\"') {
					strcat(Input_Message_Local_Commande_String, "\"");
					strcat(Input_Message_Local_Commande_String, argtString);
					strcat(Input_Message_Local_Commande_String, "\"");
				}
				else {
					strcat(Input_Message_Local_Commande_String, argtString);
				}
				//printf("s argument #%d [%s]\n", indParam, argtString);
			}
			else {
				sprintf(ErrorStr, "Error: unknown OSC pattern element %c in received message!", tag); ReportError(ErrorStr);
			}
		}
		return argc;
	}
	return 0;
}

void pg_IPServer::receiveIPMessages(void) {
	// initializes the stack in which message strings are stored
	current_depth_input_stack = 0;

	// Plain message handling
	if (receive_format != OSC) {
		if (SocketToLocalServer < 0) {
			return;
		}

		// string initialization to 0
		memset(Input_Message_String, 0x0, max_network_message_length);
		memset(Input_Message_Local_Commande_String, 0x0, max_network_message_length);

		// receive message
		int n = recv(SocketToLocalServer, Input_Message_String, max_network_message_length, 0);

		while (n > 0) {

			///////////////////////////////////////////////////////
			// Plain format (string)
			if (receive_format == Plain) {
				if (IP_message_trace) {
					printf("udp server: receive plain message [%s]\n",
						Input_Message_String);
				}
				// argc should be updated later for string message (non OSC)
				storeIP_input_messages_and_removes_duplicates(Input_Message_String, 1);
			}

			memset(Input_Message_String, 0x0, max_network_message_length);
			n = recv(SocketToLocalServer, Input_Message_String, max_network_message_length, 0);
		}
	}
	// OSC message handling
	else {
		if (SocketToLocalServer < 0) {
			return;
		}

		// string initialization to 0
		memset(Input_Message_String, 0x0, max_network_message_length);
		memset(Input_Message_Local_Commande_String, 0x0, max_network_message_length);

		// receive message
		int size = udp_transport->transport_recv(SocketToLocalServer, UDP_buffer.data(), UDP_buffer.size());

		while (size > 0) {
			///////////////////////////////////////////////////////
			// OSC format (string)
			if (receive_format == OSC) {
				if (IP_message_trace) {
					printf("udp server: receive OSC message [%s]\n", UDP_buffer.data());
				}
				int argc = handlePacket(OSCPP::Server::Packet(UDP_buffer.data(), size));
				storeIP_input_messages_and_removes_duplicates(Input_Message_Local_Commande_String, argc);
			}

			memset(Input_Message_String, 0x0, max_network_message_length);
			size = udp_transport->transport_recv(SocketToLocalServer, UDP_buffer.data(), UDP_buffer.size());
		}

	}

	// forwards the non duplicated messages to the script manager
	// either some predefined commands or scenario variable assignment
	ProcessFilteredInputMessages();
}

void pg_IPServer::storeIP_input_messages_and_removes_duplicates(char *message, int argc) {
	if (/* receive_format != OSC && */ SocketToLocalServer < 0) {
		return;
	}

	// reads beyond ID (a unique number that identifies the message
	char *messString = message;

	// tag length: the non blank chars at the beginning of the message
	int tagLength = int(strlen(messString));
	char *pch = strchr(messString, ' ');
	if (pch != NULL) {
		tagLength = int(pch - messString);
	}

	//printf( "new message [%s] current_depth_input_stack %d\n" , messString ,
	//	  current_depth_input_stack );

	// duplicates removal for OSC message formats
	// checks whether it has a duplicate and replaces it if there is one
	if (*messString == '/' && *(messString + 1) != '_' && OSC_duplicate_removal) {
		for (int ind_mess = 0; ind_mess < current_depth_input_stack; ind_mess++) {
			// same tag
			// the current message is kept instead of the previous one with the same tag
			if (input_message_length_stack[ind_mess] == tagLength
				&& strncmp(input_message_stack[ind_mess].c_str(), messString, tagLength) == 0) {
				// printf( "duplicate removed [%s] for [%s]\n" ,  input_message_stack[ ind_mess ].c_str() , messString );
				input_argc_stack[ind_mess] = argc;
				input_message_stack[ind_mess] = string(messString);
				input_argc_stack[ind_mess] = argc;
				return;
			}
		}
	}

	// input message stack storing if no duplicate found
	if (current_depth_input_stack < depth_input_stack - 1) {
		input_message_stack[current_depth_input_stack] = string(messString);
		input_argc_stack[current_depth_input_stack] = argc;
		input_message_length_stack[current_depth_input_stack] = tagLength;
		current_depth_input_stack++;
	}
	else {
		sprintf(ErrorStr, "Error: UDP input message stack overflow %d %d!", current_depth_input_stack, depth_input_stack); ReportError(ErrorStr);
		// for( int ind_mess = 0 ; ind_mess < depth_input_stack - 1  ; ind_mess++ ) {
		//   printf( "message [%s]\n" ,  input_message_stack[ ind_mess ].c_str() );
		// }
	}
}

void pg_IPServer::ProcessFilteredInputMessages(void) {
	string messString;

	if (/* receive_format != OSC && */ SocketToLocalServer < 0) {
		return;
	}

	// processes all the non duplicated messages
	for (int ind_mess = 0; ind_mess < current_depth_input_stack; ind_mess++) {
		messString = input_message_stack[ind_mess];
		int nb_arguments = input_argc_stack[ind_mess];

		if (IP_message_trace)
		{
			printf("process message [%s]\n", messString.c_str());
		}

		///////////////////////////////////////
		// OSC message format
		// skips first chars before first /
		if (messString.find('/', 0) != string::npos){

			// ignores leading /
			messString.erase(0,1);

			// removes string termination: ; + \n
			if (messString.find(';', 0) != string::npos) {
				messString = messString.substr(0, messString.find(';', 0));
			}

			// computes the length of the message tag
			int tagLength = 0;
			int indOSCParam = 0;
			float OSC_float_arguments[MAX_OSC_ARGUMENTS] = { 0.f };
			for (int ind = 0; ind < MAX_OSC_ARGUMENTS; ind++) {
				OSC_arguments[ind] = "";
			}
			if (messString.find(' ', 0) != string::npos) {
				tagLength = messString.find(' ', 0);
				OSC_address = messString.substr(0, tagLength);
				messString = messString.substr(tagLength + 1, string::npos);;

				// printf( "OSC parameters %s\n" );
				while (indOSCParam < MAX_OSC_ARGUMENTS
					&& messString != "") {
					// computes the length of the message tag
					if (messString.find(' ', 0) != string::npos) {
						tagLength = messString.find(' ', 0);
						OSC_arguments[indOSCParam] = messString.substr(0, tagLength);
						messString = messString.substr(tagLength + 1, string::npos);
						indOSCParam++;
					}
					else {
						OSC_arguments[indOSCParam] = messString;
						messString = "";
						indOSCParam++;
					}
				}
			}
			else {
				OSC_address = messString;
				indOSCParam = 0;
				messString = "";
			}


			if (messString != "") {
				sprintf(ErrorStr, "Error: OSC maximal parameter count too low (%d vs %d)!", indOSCParam, MAX_OSC_ARGUMENTS); ReportError(ErrorStr);
			}

			//printf("Alias command %s size %d :\n" , OSC_address.c_str(), indOSCParam);

			for (int ind = 0; ind < indOSCParam; ind++) {
				try {
					OSC_float_arguments[ind] = std::stof(OSC_arguments[ind]);
				}
				catch (...) {
					OSC_float_arguments[ind] = 0.f;
				}
				//printf("arg %d %.5f, ", ind, OSC_float_arguments[ind]);
			}
			//printf("\n");
			pg_aliasScript(OSC_address, OSC_arguments[0], OSC_float_arguments, nb_arguments);
		}
		else {
			sprintf(ErrorStr, "Error: incorrect external message syntax %s!", messString.c_str()); ReportError(ErrorStr); break;
		}
	}
}

