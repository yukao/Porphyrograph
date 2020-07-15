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

void pg_CopyAndAllocString( char **target , const char *source ) {
  if( !source ) {
    return;
  }
  int str_l = int(strlen(source));
  if( *target && (int(strlen( *target )) < str_l ) ) {
    delete [] *target;
    *target = NULL;
  }
  if( !(*target) ) {
    *target = new char[ str_l + 1 ];
  }
  memcpy( *target , source , str_l * sizeof( char ) );
  (*target)[str_l] = 0;
}

long     pg_ScanIntegerString( int *p_c  ,
			    int withTrailingSpaceChars , 
			    char *charstring , int *ind ) {
  long            i = 0L;
  short           neg = 0;

  if( *p_c == '-') {
    neg = 1;
    *p_c = charstring[*ind];
    (*ind)++;
  }
  else if( *p_c == '+') {
    *p_c = charstring[*ind];
    (*ind)++;
  }
  if (_Num(*p_c)) {
    while ( _Num(*p_c) ) {
      i = i*10L + (long)(*p_c - '0');
      *p_c = charstring[*ind];
      (*ind)++;
    }

    if( withTrailingSpaceChars ) {
      while( _SpaceChar(*p_c) ) {
	*p_c = charstring[*ind];
	(*ind)++;
      }
    }
    if( neg == 1)
      i = -i;
    return i;
  }
  else {
    return 0L;
  }
}

float    pg_ScanFloatString( int *p_c  ,
		   int withTrailingSpaceChars , 
		   char *charstring , int *ind ) {
  float           resul, dec = 0.1F;
  short           neg = 0;

  // arithmetic expression
  if( *p_c == '-' ) {
    neg = 1;
    *p_c = charstring[*ind];
    (*ind)++;
  }
  else if( *p_c == '+' ) {
    *p_c = charstring[*ind];
    (*ind)++;
  }
  resul = (float) pg_ScanIntegerString( p_c , false ,charstring,ind );
  if( *p_c == '.') {
    *p_c = charstring[*ind];
    (*ind)++;

    while ( _Num(*p_c) ) {
      resul += dec * (*p_c - '0');
      dec *= 0.1F;
      *p_c = charstring[*ind];
      (*ind)++;
    }
  }
  // exponent
  if(  *p_c == 'e' ) {
    *p_c = charstring[*ind];
    (*ind)++;
    float exponent = (float)pg_ScanIntegerString( p_c , false ,charstring,ind );
    resul *= powf( 10.0 , exponent );
 }
  if( withTrailingSpaceChars ) {
    while( _SpaceChar(*p_c) ) {
      *p_c = charstring[*ind];
      (*ind)++;
    }
  }
  if( neg == 1)
    resul = -resul;
  return resul;
}

// string splitting into string vector by single char
vector<string> split(string str, char token) {
	vector<string>result;
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
  depth_output_stack = 10;
  current_depth_output_stack = -1;
  maximal_IP_message_delay = 0.1F;
  last_IP_message_time = 0;

  // output message format
  send_format = Plain;

  // labelling of output messages
  first_IP_message_number = 0;
  current_IP_message_number = 0;

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
    for (int ind = 0; ind < depth_output_stack; ind ++) {
      if (output_message_stack[ind]) {
		delete [] output_message_stack[ind];
		output_message_stack[ind] = NULL;
	  }
    }

    delete [] output_message_stack;
    output_message_stack = NULL;
  }

  if (output_pattern_stack) {
    for (int ind = 0; ind < depth_output_stack; ind ++) {
      if (output_pattern_stack[ind]) {
		delete [] output_pattern_stack[ind];
		output_pattern_stack[ind] = NULL;
      }
    }

    delete [] output_pattern_stack;
    output_pattern_stack = NULL;
  }

#ifdef WIN32
  closesocket(SocketToRemoteServer);
#endif
}

void pg_IPClient::InitClient(void) {

	//////////////////////////////
	// remote server opening

	struct                  addrinfo *result;
	struct                  addrinfo hints;

	char Remote_server_port_string[128];
	sprintf(Remote_server_port_string, "%d", Remote_server_port);

	// non liblo message handling
	if (send_format != OSC) {
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
	}
	// liblo message handling
	else {
		// liblo client
		lo_client = lo_address_new(Remote_server_IP.c_str(), Remote_server_port_string);
		printf("Network client '%s': sending OSC to '%s' on port %d\n",
			id.c_str(), Remote_server_IP.c_str(), Remote_server_port);
	}

	// system initializing
	IP_OutputStackInitialization();
}

void pg_IPClient::IP_OutputStackInitialization( void ) {
  // output message stack initializing
  output_message_stack = new Pchar[depth_output_stack];
  for(int ind_stack = 0 ; ind_stack < depth_output_stack ;
      ind_stack++ ) {
    output_message_stack[ ind_stack ] = NULL;
  }
  current_depth_output_stack = -1;
  // output pattern stack initializing
  output_pattern_stack = new Pchar[depth_output_stack];
  for(int ind_stack = 0 ; ind_stack < depth_output_stack ;
      ind_stack++ ) {
    output_pattern_stack[ ind_stack ] = NULL;
  }
}

void pg_IPClient::sendIPmessages(void) {
	if (SocketToRemoteServer < 0) {
		return;
	}

	while (current_depth_output_stack >= 0) {
		// printf( "sendUDPmessage %d\n" , current_depth_output_stack );

		// checks whether last message was received
		// or it is the first message
		// or the elapsed time since the last message is greater than the
		// maximal delay
		double current_time = RealTime();

		bool message_received = true;

		if (message_received
			|| current_IP_message_number == first_IP_message_number
			|| ((current_time - last_IP_message_time) / 1000.0f)
				> maximal_IP_message_delay) {


			// printf( "send message [%s]\n" , output_message_stack[ 0 ]);

			// copies the earliest message
			int     indLocalCommandLine = 0;

			// Plain format
			if (send_format == Plain) {
				strncpy(Output_Message_String, output_message_stack[0],
					max_network_message_length - 1);
			}

			// OSC bundle
			else if (send_format == OSC && *(output_message_stack[0]) != '/') {
				// sends bundle through OSC
				// message that does not begin by an OSC address
				strcpy(Output_Message_OSC, "#bundle");
				for (int ind = int(strlen("#bundle")); ind < 20 - 1; ind++) {
					Output_Message_OSC[ind] = 0;
				}
				strncpy(Output_Message_OSC + 20, output_message_stack[0],
					2 * max_network_message_length - 21);
				Output_Message_OSC[20 - 1] = (char)strlen(Output_Message_OSC + 20);
			}

			// OSC message
			else if (send_format == OSC && *(output_message_stack[0]) == '/') {
				// splits the argument separated by space chars
				std::vector<std::string> message_arguments;
				message_arguments = split(String(output_message_stack[0]), ' ');

				// sends OSC message through OSC
				// message that begins by an OSC address /aaaa/aaa 
					// whether or not the pattern is provided with the command
				if (*(output_pattern_stack[0])) {
					int pattern_size = int(strlen(output_pattern_stack[0]));

					if (pattern_size == message_arguments.size() - 1) {
						// single argument
						if (pattern_size == 1) {
							switch (*(output_pattern_stack[0])) {
							case 's':
								if (lo_send(lo_client, message_arguments[0].c_str(), output_pattern_stack[0], message_arguments[1].c_str()) == -1) {
									sprintf(ErrorStr, "Error: OSC error %d: %s  for message %s\n", lo_address_errno(lo_client),
										lo_address_errstr(lo_client), output_message_stack[0]); ReportError(ErrorStr); 
								}
								break;
							case 'i':
								if (lo_send(lo_client, message_arguments[0].c_str(), output_pattern_stack[0], stoi(message_arguments[1])) == -1) {
									sprintf(ErrorStr, "Error: OSC error %d: %s  for message %s\n", lo_address_errno(lo_client),
										lo_address_errstr(lo_client), output_message_stack[0]); ReportError(ErrorStr); 
								}
								break;
							case 'f':
								if (strcmp(message_arguments[0].c_str(), "/video") == 0) {
									printf("Message integer string %s value %.2f\n", message_arguments[0].c_str(), stof(message_arguments[1]));
								}
								if (lo_send(lo_client, message_arguments[0].c_str(), output_pattern_stack[0], stof(message_arguments[1])) == -1) {
									sprintf(ErrorStr, "Error: OSC error %d: %s  for message %s\n", lo_address_errno(lo_client),
										lo_address_errstr(lo_client), output_message_stack[0]); ReportError(ErrorStr); 
								}
								break;
							default:
								sprintf(ErrorStr, "Error: unknown OSC pattern element %c!", *(output_pattern_stack[0])); ReportError(ErrorStr); 
								break;
							}
						}
						else {
							if (strcmp(output_pattern_stack[0], "ff") == 0) {
								if (lo_send(lo_client, message_arguments[0].c_str(), output_pattern_stack[0],
									stof(message_arguments[1]), stof(message_arguments[2])) == -1) {
									sprintf(ErrorStr, "Error: OSC error %d: %s  for message %s\n", lo_address_errno(lo_client),
										lo_address_errstr(lo_client), output_message_stack[0]); ReportError(ErrorStr); throw(100);
								}
							}
							else if (strcmp(output_pattern_stack[0], "fff") == 0) {
								if (lo_send(lo_client, message_arguments[0].c_str(), output_pattern_stack[0],
									stof(message_arguments[1]), stof(message_arguments[2]), stof(message_arguments[3])) == -1) {
									sprintf(ErrorStr, "Error: OSC error %d: %s  for message %s\n", lo_address_errno(lo_client),
										lo_address_errstr(lo_client), output_message_stack[0]); ReportError(ErrorStr); throw(100);
								}
							}
							else if (strcmp(output_pattern_stack[0], "ffff") == 0) {
								if (lo_send(lo_client, message_arguments[0].c_str(), output_pattern_stack[0],
									stof(message_arguments[1]), stof(message_arguments[2]), stof(message_arguments[3]), stof(message_arguments[4])) == -1) {
									sprintf(ErrorStr, "Error: OSC error %d: %s  for message %s\n", lo_address_errno(lo_client),
										lo_address_errstr(lo_client), output_message_stack[0]); ReportError(ErrorStr); throw(100);
								}
							}
							else if (strcmp(output_pattern_stack[0], "fffff") == 0) {
								if (lo_send(lo_client, message_arguments[0].c_str(), output_pattern_stack[0],
									stof(message_arguments[1]), stof(message_arguments[2]), stof(message_arguments[3]), stof(message_arguments[4]), 
									stof(message_arguments[5])) == -1) {
									sprintf(ErrorStr, "Error: OSC error %d: %s  for message %s\n", lo_address_errno(lo_client),
										lo_address_errstr(lo_client), output_message_stack[0]); ReportError(ErrorStr); throw(100);
								}
							}
							else if (strcmp(output_pattern_stack[0], "ffffff") == 0) {
								if (lo_send(lo_client, message_arguments[0].c_str(), output_pattern_stack[0],
									stof(message_arguments[1]), stof(message_arguments[2]), stof(message_arguments[3]), stof(message_arguments[4]), 
									stof(message_arguments[5]), stof(message_arguments[6])) == -1) {
									sprintf(ErrorStr, "Error: OSC error %d: %s  for message %s\n", lo_address_errno(lo_client),
										lo_address_errstr(lo_client), output_message_stack[0]); ReportError(ErrorStr); throw(100);
								}
							}
							else if (strcmp(output_pattern_stack[0], "fffffff") == 0) {
								if (lo_send(lo_client, message_arguments[0].c_str(), output_pattern_stack[0],
									stof(message_arguments[1]), stof(message_arguments[2]), stof(message_arguments[3]), stof(message_arguments[4]), 
									stof(message_arguments[5]), stof(message_arguments[6]), stof(message_arguments[7])) == -1) {
									sprintf(ErrorStr, "Error: OSC error %d: %s  for message %s\n", lo_address_errno(lo_client),
										lo_address_errstr(lo_client), output_message_stack[0]); ReportError(ErrorStr); throw(100);
								}
							}
							else if (strcmp(output_pattern_stack[0], "ffffffff") == 0) {
								if (lo_send(lo_client, message_arguments[0].c_str(), output_pattern_stack[0],
									stof(message_arguments[1]), stof(message_arguments[2]), stof(message_arguments[3]), stof(message_arguments[4]), 
									stof(message_arguments[5]), stof(message_arguments[6]), stof(message_arguments[7]), stof(message_arguments[8])) == -1) {
									sprintf(ErrorStr, "Error: OSC error %d: %s  for message %s\n", lo_address_errno(lo_client),
										lo_address_errstr(lo_client), output_message_stack[0]); ReportError(ErrorStr); throw(100);
								}
							}
							else if (strcmp(output_pattern_stack[0], "fffffffff") == 0) {
								if (lo_send(lo_client, message_arguments[0].c_str(), output_pattern_stack[0],
									stof(message_arguments[1]), stof(message_arguments[2]), stof(message_arguments[3]), stof(message_arguments[4]), 
									stof(message_arguments[5]), stof(message_arguments[6]), stof(message_arguments[7]), stof(message_arguments[8]), 
									stof(message_arguments[9])) == -1) {
									sprintf(ErrorStr, "Error: OSC error %d: %s  for message %s\n", lo_address_errno(lo_client),
										lo_address_errstr(lo_client), output_message_stack[0]); ReportError(ErrorStr); throw(100);
								}
							}
							else if (strcmp(output_pattern_stack[0], "ffffffffff") == 0) {
								if (lo_send(lo_client, message_arguments[0].c_str(), output_pattern_stack[0],
									stof(message_arguments[1]), stof(message_arguments[2]), stof(message_arguments[3]), stof(message_arguments[4]),
									stof(message_arguments[5]), stof(message_arguments[6]), stof(message_arguments[7]), stof(message_arguments[8]),
									stof(message_arguments[9]), stof(message_arguments[10])) == -1) {
									sprintf(ErrorStr, "Error: OSC error %d: %s  for message %s\n", lo_address_errno(lo_client),
										lo_address_errstr(lo_client), output_message_stack[0]); ReportError(ErrorStr); throw(100);
								}
							}
							else if (strcmp(output_pattern_stack[0], "fffffffffff") == 0) {
								if (lo_send(lo_client, message_arguments[0].c_str(), output_pattern_stack[0],
									stof(message_arguments[1]), stof(message_arguments[2]), stof(message_arguments[3]), stof(message_arguments[4]),
									stof(message_arguments[5]), stof(message_arguments[6]), stof(message_arguments[7]), stof(message_arguments[8]),
									stof(message_arguments[9]), stof(message_arguments[10]), stof(message_arguments[11])) == -1) {
									sprintf(ErrorStr, "Error: OSC error %d: %s  for message %s\n", lo_address_errno(lo_client),
										lo_address_errstr(lo_client), output_message_stack[0]); ReportError(ErrorStr); throw(100);
								}
							}
							else if (strcmp(output_pattern_stack[0], "ffffffffffff") == 0) {
								if (lo_send(lo_client, message_arguments[0].c_str(), output_pattern_stack[0],
									stof(message_arguments[1]), stof(message_arguments[2]), stof(message_arguments[3]), stof(message_arguments[4]),
									stof(message_arguments[5]), stof(message_arguments[6]), stof(message_arguments[7]), stof(message_arguments[8]),
									stof(message_arguments[9]), stof(message_arguments[10]), stof(message_arguments[11]), stof(message_arguments[12])) == -1) {
									sprintf(ErrorStr, "Error: OSC error %d: %s  for message %s\n", lo_address_errno(lo_client),
										lo_address_errstr(lo_client), output_message_stack[0]); ReportError(ErrorStr); throw(100);
								}
							}
							else if (strcmp(output_pattern_stack[0], "fffffffffffff") == 0) {
								if (lo_send(lo_client, message_arguments[0].c_str(), output_pattern_stack[0],
									stof(message_arguments[1]), stof(message_arguments[2]), stof(message_arguments[3]), stof(message_arguments[4]),
									stof(message_arguments[5]), stof(message_arguments[6]), stof(message_arguments[7]), stof(message_arguments[8]),
									stof(message_arguments[9]), stof(message_arguments[10]), stof(message_arguments[11]), stof(message_arguments[12]), 
									stof(message_arguments[13])) == -1) {
									sprintf(ErrorStr, "Error: OSC error %d: %s  for message %s\n", lo_address_errno(lo_client),
										lo_address_errstr(lo_client), output_message_stack[0]); ReportError(ErrorStr); throw(100);
								}
							}
							else if (strcmp(output_pattern_stack[0], "ffffffffffffff") == 0) {
								if (lo_send(lo_client, message_arguments[0].c_str(), output_pattern_stack[0],
									stof(message_arguments[1]), stof(message_arguments[2]), stof(message_arguments[3]), stof(message_arguments[4]),
									stof(message_arguments[5]), stof(message_arguments[6]), stof(message_arguments[7]), stof(message_arguments[8]),
									stof(message_arguments[9]), stof(message_arguments[10]), stof(message_arguments[11]), stof(message_arguments[12]), 
									stof(message_arguments[13]), stof(message_arguments[14])) == -1) {
									sprintf(ErrorStr, "Error: OSC error %d: %s  for message %s\n", lo_address_errno(lo_client),
										lo_address_errstr(lo_client), output_message_stack[0]); ReportError(ErrorStr); throw(100);
								}
							}
							else if (strcmp(output_pattern_stack[0], "fffffffffffffff") == 0) {
								if (lo_send(lo_client, message_arguments[0].c_str(), output_pattern_stack[0],
									stof(message_arguments[1]), stof(message_arguments[2]), stof(message_arguments[3]), stof(message_arguments[4]),
									stof(message_arguments[5]), stof(message_arguments[6]), stof(message_arguments[7]), stof(message_arguments[8]),
									stof(message_arguments[9]), stof(message_arguments[10]), stof(message_arguments[11]), stof(message_arguments[12]),
									stof(message_arguments[13]), stof(message_arguments[14]), stof(message_arguments[15])) == -1) {
									sprintf(ErrorStr, "Error: OSC error %d: %s  for message %s\n", lo_address_errno(lo_client),
										lo_address_errstr(lo_client), output_message_stack[0]); ReportError(ErrorStr); throw(100);
								}
							}
							else if (strcmp(output_pattern_stack[0], "ffffffffffffffff") == 0) {
								if (lo_send(lo_client, message_arguments[0].c_str(), output_pattern_stack[0],
									stof(message_arguments[1]), stof(message_arguments[2]), stof(message_arguments[3]), stof(message_arguments[4]),
									stof(message_arguments[5]), stof(message_arguments[6]), stof(message_arguments[7]), stof(message_arguments[8]),
									stof(message_arguments[9]), stof(message_arguments[10]), stof(message_arguments[11]), stof(message_arguments[12]),
									stof(message_arguments[13]), stof(message_arguments[14]), stof(message_arguments[15]), stof(message_arguments[16])) == -1) {
									sprintf(ErrorStr, "Error: OSC error %d: %s  for message %s\n", lo_address_errno(lo_client),
										lo_address_errstr(lo_client), output_message_stack[0]); ReportError(ErrorStr); throw(100);
								}
							}
							else if (strcmp(output_pattern_stack[0], "fffffffffffffffff") == 0) {
								if (lo_send(lo_client, message_arguments[0].c_str(), output_pattern_stack[0],
									stof(message_arguments[1]), stof(message_arguments[2]), stof(message_arguments[3]), stof(message_arguments[4]),
									stof(message_arguments[5]), stof(message_arguments[6]), stof(message_arguments[7]), stof(message_arguments[8]),
									stof(message_arguments[9]), stof(message_arguments[10]), stof(message_arguments[11]), stof(message_arguments[12]),
									stof(message_arguments[13]), stof(message_arguments[14]), stof(message_arguments[15]), stof(message_arguments[16]),
									stof(message_arguments[17])) == -1) {
									sprintf(ErrorStr, "Error: OSC error %d: %s  for message %s\n", lo_address_errno(lo_client),
										lo_address_errstr(lo_client), output_message_stack[0]); ReportError(ErrorStr); throw(100);
								}
							}
							else if (strcmp(output_pattern_stack[0], "ffffffffffffffffff") == 0) {
								if (lo_send(lo_client, message_arguments[0].c_str(), output_pattern_stack[0],
									stof(message_arguments[1]), stof(message_arguments[2]), stof(message_arguments[3]), stof(message_arguments[4]),
									stof(message_arguments[5]), stof(message_arguments[6]), stof(message_arguments[7]), stof(message_arguments[8]),
									stof(message_arguments[9]), stof(message_arguments[10]), stof(message_arguments[11]), stof(message_arguments[12]),
									stof(message_arguments[13]), stof(message_arguments[14]), stof(message_arguments[15]), stof(message_arguments[16]),
									stof(message_arguments[17]), stof(message_arguments[18])) == -1) {
									sprintf(ErrorStr, "Error: OSC error %d: %s  for message %s\n", lo_address_errno(lo_client),
										lo_address_errstr(lo_client), output_message_stack[0]); ReportError(ErrorStr); throw(100);
								}
							}
							else if (strcmp(output_pattern_stack[0], "fffffffffffffffffff") == 0) {
								if (lo_send(lo_client, message_arguments[0].c_str(), output_pattern_stack[0],
									stof(message_arguments[1]), stof(message_arguments[2]), stof(message_arguments[3]), stof(message_arguments[4]),
									stof(message_arguments[5]), stof(message_arguments[6]), stof(message_arguments[7]), stof(message_arguments[8]),
									stof(message_arguments[9]), stof(message_arguments[10]), stof(message_arguments[11]), stof(message_arguments[12]),
									stof(message_arguments[13]), stof(message_arguments[14]), stof(message_arguments[15]), stof(message_arguments[16]),
									stof(message_arguments[17]), stof(message_arguments[18]), stof(message_arguments[19])) == -1) {
									sprintf(ErrorStr, "Error: OSC error %d: %s  for message %s\n", lo_address_errno(lo_client),
										lo_address_errstr(lo_client), output_message_stack[0]); ReportError(ErrorStr); throw(100);
								}
							}
							else if (strcmp(output_pattern_stack[0], "ffffffffffffffffffff") == 0) {
								if (lo_send(lo_client, message_arguments[0].c_str(), output_pattern_stack[0],
									stof(message_arguments[1]), stof(message_arguments[2]), stof(message_arguments[3]), stof(message_arguments[4]),
									stof(message_arguments[5]), stof(message_arguments[6]), stof(message_arguments[7]), stof(message_arguments[8]),
									stof(message_arguments[9]), stof(message_arguments[10]), stof(message_arguments[11]), stof(message_arguments[12]),
									stof(message_arguments[13]), stof(message_arguments[14]), stof(message_arguments[15]), stof(message_arguments[16]),
									stof(message_arguments[17]), stof(message_arguments[18]), stof(message_arguments[19]), stof(message_arguments[20])) == -1) {
									sprintf(ErrorStr, "Error: OSC error %d: %s  for message %s\n", lo_address_errno(lo_client),
										lo_address_errstr(lo_client), output_message_stack[0]); ReportError(ErrorStr); throw(100);
								}
							}
							else {
								// error: does not process message of more than one argument for the moment
								sprintf(ErrorStr, "Error: OSC output message with more thant one argument can only be from 2 to 20 float arguments %s (size %d)!", output_message_stack[0], pattern_size); ReportError(ErrorStr); throw(100);
							}
						}
					}
					else {
						// error : pattern and message size differ
						sprintf(ErrorStr, "Error: OSC pattern and message size differ %s / %s (pattern size %d message size %d)!", output_pattern_stack[0], output_message_stack[0], pattern_size, message_arguments.size() - 1); ReportError(ErrorStr); throw(100);
					}
				}
				// no pattern, just an address
				else {
					if (lo_send(lo_client, output_message_stack[0], NULL) == -1) {
						sprintf(ErrorStr, "Error: OSC error %d: %s  for message %s\n", lo_address_errno(lo_client),
							lo_address_errstr(lo_client), output_message_stack[0]); ReportError(ErrorStr); throw(100);
					}
				}
			}

			// FIFO processing of messages
			// shifts the stack down 
			for (int ind_mess = 0; ind_mess < current_depth_output_stack;
				ind_mess++) {
				pg_CopyAndAllocString(&(output_message_stack[ind_mess]),
					output_message_stack[ind_mess + 1]);
				pg_CopyAndAllocString(&(output_pattern_stack[ind_mess]),
					output_pattern_stack[ind_mess + 1]);
			}
			current_depth_output_stack--;

			// records the emission time
			last_IP_message_time = RealTime();

			// sends the message
			// formats the command
			// pads the length to a multiple of 4
			int length;
			int localCommandLineLength = 0;
			if (send_format == Plain) {
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
				// MaxOutput_Message_String[ length + (12 - length % 4) - 1] = 0;
			}
			// OSC bundle
			else if (send_format == OSC && *(output_message_stack[0]) != '/') {
				// the OSC bundle message is currently not sent through the OSC library liblo
				sprintf(ErrorStr, "Error: udp client: OSC bundle emission not processed [%s]!", output_message_stack[0]); ReportError(ErrorStr);
				continue;
				/*
				length = int(20 + strlen(Output_Message_OSC + 20));
				int inchar;
				for (inchar = length; inchar < length + (4 - length % 4) - 1;
					inchar++) {
					Output_Message_OSC[inchar] = ' ';
				}
				Output_Message_OSC[length + (4 - length % 4) - 1] = 0;
				Output_Message_OSC[20 - 1] = (char)(strlen(Output_Message_OSC + 20) + 1);
				localCommandLineLength = length + (4 - length % 4);
				*/
				//printf( "message size %d\n" , strlen(Output_Message_OSC + 20 )  + 1 );
			}
			// OSC message
			else if (send_format == OSC && *(output_message_stack[0]) == '/') {
				// the message has been sent through the OSC library liblo
				// localCommandLineLength = indLocalCommandLine;
				continue;
			}

			// sends the command (except OSC messages)
			int rc = 0;
			// Plain format
			if (send_format == Plain) {
				rc = send(SocketToRemoteServer, Output_Message_String,
					localCommandLineLength, 0);
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
			// OSC bundle
			// currently not sent through the liblo library, modify the code in the preceding loop to send bundle
			// OSC messages through liblo
			/*
			else if (send_format == OSC && *(output_message_stack[0]) == '#') {
				rc = 0;
				int len = int(20 + strlen(Output_Message_OSC + 20) + 1);
				rc = send(SocketToRemoteServer, Output_Message_OSC, len, 0);
				//(struct sockaddr *) &remoteServAddr, 
				//sizeof(remoteServAddr));
				if (rc != len) {
					sprintf(ErrorStr, "Error: udp client: OSC data incompletely sent [%s]!", Output_Message_OSC); ReportError(ErrorStr); // close(SocketToRemoteServer); throw 273;
				}
				if (rc > 0 && IP_message_trace) {
					printf("udp client: send OSC bundle %d [%s] (%d bytes)\n",
						(int)*(Output_Message_OSC + 20 - 1),
						Output_Message_OSC + 20,
						(int)(20 + strlen(Output_Message_OSC + 20) + 1));
				}
			}
			*/
			// OSC message
			// the message has been sent through the OSC library liblo
			/*
			else if (send_format == OSC && *Output_Message_OSC == '/') {
				rc = 0;
				rc = send(SocketToRemoteServer, Output_Message_OSC,
							localCommandLineLength, 0);
				//(struct sockaddr *) &remoteServAddr, 
				//sizeof(remoteServAddr));
				if (rc != localCommandLineLength) {
					sprintf(ErrorStr, "Error: udp client: data incompletely sent [%s]!", Output_Message_OSC); ReportError(ErrorStr); // close(SocketToRemoteServer); throw 273;
				}
				if (rc > 0 && IP_message_trace) {
					printf("udp client: send OSC message (%d bytes)\n",
						localCommandLineLength);
					for (int ind = 0; ind < localCommandLineLength; ind++) {
						//printf( "%c(%d) " ,Output_Message_OSC[ind] , (int)Output_Message_OSC[ind] );
						printf("%c", Output_Message_OSC[ind]);
					}
					printf("\n");
				}
			}
			*/
		}
		else {
			if (IP_message_trace) {
				printf("udp client: delay %.5f / %.5f \n",
					((current_time - last_IP_message_time) / 1000.0f),
					maximal_IP_message_delay);
			}
			return;
		}
	}
}

void pg_IPClient::storeIP_output_message(char *commandLine,
	char *pattern) {

	// printf( "commandLine %s %d\n" , commandLine , current_depth_output_stack );
	if (SocketToRemoteServer < 0) {
		return;
	}

	// printf( "commandLine %s %d\n" , commandLine , current_depth_output_stack );
	if (current_depth_output_stack < depth_output_stack - 1) {
		current_depth_output_stack++;
		pg_CopyAndAllocString(&(output_message_stack[current_depth_output_stack]),
			commandLine);
		if (pattern && *pattern) {
			pg_CopyAndAllocString(&(output_pattern_stack[current_depth_output_stack]),
				pattern);
		}
		else {
			pg_CopyAndAllocString(&(output_pattern_stack[current_depth_output_stack]),
				"");
		}
	}
	else {
		sprintf(ErrorStr, "Error: UDP output message stack overflow %d %d (%s)!", current_depth_output_stack, depth_output_stack, commandLine); ReportError(ErrorStr);
	}
}

/////////////////////////////////////////////////////////////////////////////////
// UDP SERVER

// liblo OSC messages error handling
void liblo_error_handling(int num, const char *msg, const char *path) {
	sprintf(ErrorStr, "Error: liblo server error %d in path %s: %s\n", num, path, msg); ReportError(ErrorStr);
}

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

pg_IPServer::~pg_IPServer( void ) {
  id.clear();

  if (input_message_stack)
    {
      for(int ind_stack = 0 ; ind_stack < depth_input_stack ; 
	  ind_stack++ ) 
	{
	  if (input_message_stack[ind_stack])
	    {
	      delete [] input_message_stack[ind_stack];
	      input_message_stack[ind_stack] = NULL;
	    }
	}

      delete [] input_message_stack;
      input_message_stack = NULL;
    }

  if (input_message_length_stack)
    {
      delete [] input_message_length_stack;
      input_message_length_stack = NULL;
    }

  for( int ind = 0 ; ind < MAX_OSC_ARGUMENTS ; ind++ ) {
    if( OSC_arguments[ind] ) {
      delete [] OSC_arguments[ind];
    }
  }

#ifdef WIN32
  closesocket(SocketToLocalServer);
#endif
}

void pg_IPServer::InitServer(void) {
	///////////////////////////////
	// local server creation
	if (receive_format != OSC) {
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

		int rc = bind(SocketToLocalServer, (struct sockaddr *) &localServAddr,
			sizeof(localServAddr));
		if (rc < 0) {
			sprintf(ErrorStr, "Error: cannot bind local port number %d!", Local_server_port); ReportError(ErrorStr);
			return;
		}
	}
	// liblo init server
	else {
		char Local_server_port_string[128];
		sprintf(Local_server_port_string, "%d", Local_server_port);

		/* start a new server on port Local_server_port */
		lo_local_server = lo_server_new(Local_server_port_string, liblo_error_handling);

		/* add method that will match any path and args */
		lo_server_add_method(lo_local_server, NULL, NULL, processLibloReceivedOSC, this);
	}

	printf("Network server: listening on port %u\n", Local_server_port);

	// message stack initialization
	IP_InputStackInitialization();

	for (int ind = 0; ind < MAX_OSC_ARGUMENTS; ind++) {
		OSC_arguments[ind] = new char[StringLength];
		*(OSC_arguments[ind]) = 0;
	}
	OSCTag = new char[StringLength];
}

void pg_IPServer::IP_InputStackInitialization(void) {
	// input message stack initializing
	input_message_stack = new Pchar[depth_input_stack];
	input_message_length_stack = new int[depth_input_stack];
	for (int ind_stack = 0; ind_stack < depth_input_stack;
		ind_stack++) {
		input_message_stack[ind_stack] = NULL;
		input_message_length_stack[ind_stack] = 0;
	}
	current_depth_input_stack = 0;
}

// liblo catch any incoming messages and display them. returning 1 means that the
// message has not been fully handled and the server should try other methods
int processLibloReceivedOSC(const char *path, const char *types, lo_arg ** argv,
	int argc, void *data, void *user_data)
{
	pg_IPServer *classInstance = (pg_IPServer *)user_data;
	char argument[StringLength];

	// string initialization to 0
	memset(Input_Message_Local_Commande_String, 0x0, max_network_message_length);

	strcpy(Input_Message_Local_Commande_String, path);
	for (int indParam = 0; indParam < argc; indParam++) {
		strcat(Input_Message_Local_Commande_String, " ");
		if (types[indParam] == 'i') {
			sprintf(argument, "%d", argv[indParam]->i);
			strcat(Input_Message_Local_Commande_String, argument);
		}
		else if (types[indParam] == 'f') {
			sprintf(argument, "%.5f", argv[indParam]->f);
			// printf("f argument #%d [%s] at %d %d\n", indParam + 1 , argument , indChar , sizeof( float ) ); 
			strcat(Input_Message_Local_Commande_String, argument);
		}
		else if (types[indParam] == 's') {
			char *argtString = (char *)argv[indParam];
			// printf("s argument #%d [%s] at %d\n", indParam + 1 , message + indChar , indChar );
			if( (*argtString) != '\"') {
				strcat(Input_Message_Local_Commande_String, "\"");
				strcat(Input_Message_Local_Commande_String, argtString);
				strcat(Input_Message_Local_Commande_String, "\"");
			}
			else {
				strcat(Input_Message_Local_Commande_String, argtString);
			}
		}
	}

	if (classInstance->IP_message_trace) {
		printf("udp server: receive OSC message [%s]\n",
			Input_Message_Local_Commande_String);
	}
	classInstance->storeIP_input_messages_and_removes_duplicates(Input_Message_Local_Commande_String);

	return 1;
}

int pg_IPServer::receiveOneMessage( char *message ) {
  // message length
  int n = 0;

  // one shot UDP reception
  n = recv(SocketToLocalServer, message, max_network_message_length, 0);
  //printf("message size %d\n", n);
  //for (int i = 0; i < n; i++) {
	 // printf("%c", message[i]);
  //}
  //printf("\n");
  return n;
}

void pg_IPServer::receiveIPMessages(void) {
	// initializes the stack in which message strings are stored
	current_depth_input_stack = 0;

	// non liblo message handling
	if (receive_format != OSC) {
		if (SocketToLocalServer < 0) {
			return;
		}

		// string initialization to 0
		memset(Input_Message_String, 0x0, max_network_message_length);
		memset(Input_Message_Local_Commande_String, 0x0, max_network_message_length);

		// receive message
		int n = receiveOneMessage(Input_Message_String);

		while (n > 0) {

			///////////////////////////////////////////////////////
			// Plain format (string)
			if (receive_format == Plain) {
				if (IP_message_trace) {
					printf("udp server: receive plain message [%s]\n",
						Input_Message_String);
				}
				storeIP_input_messages_and_removes_duplicates(Input_Message_String);
			}

			memset(Input_Message_String, 0x0, max_network_message_length);
			n = receiveOneMessage(Input_Message_String);
		}
	}
	// liblo OSC message handling
	else {
		while (lo_server_recv_noblock(lo_local_server,0)) {
		}
	}

	// forwards the non duplicated messages to the script manager
	// either some predefined commands or scenario variable assignment
	ProcessFilteredInputMessages();
}

void pg_IPServer::storeIP_input_messages_and_removes_duplicates(char *message) {
	if (receive_format != OSC && SocketToLocalServer < 0) {
		return;
	}

	// reads beyond ID (a unique number that identifies the message
	char *messString = message;

	// tag length: the non blank chars at the beginning of the message
	int tagLength = int(strlen(messString));
	char *pch;
	if ((pch = strchr(messString, ' '))) {
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
				&& strncmp(input_message_stack[ind_mess],
					messString, tagLength) == 0) {
				// printf( "duplicate removed [%s] for [%s]\n" ,  input_message_stack[ ind_mess ] , messString );
				pg_CopyAndAllocString(&input_message_stack[ind_mess], messString);
				return;
			}
		}
	}

	// input message stack storing if no duplicate found
	if (current_depth_input_stack < depth_input_stack - 1) {
		pg_CopyAndAllocString(&input_message_stack[current_depth_input_stack], messString);
		input_message_length_stack[current_depth_input_stack] = tagLength;
		current_depth_input_stack++;
	}
	else {
		sprintf(ErrorStr, "Error: UDP input message stack overflow %d %d!", current_depth_input_stack, depth_input_stack); ReportError(ErrorStr);
		// for( int ind_mess = 0 ; ind_mess < depth_input_stack - 1  ; ind_mess++ ) {
		//   printf( "message [%s]\n" ,  input_message_stack[ ind_mess ] );
		// }
	}
}

void pg_IPServer::ProcessFilteredInputMessages(void) {
	char *messString;

	if (receive_format != OSC && SocketToLocalServer < 0) {
		return;
	}

	// processes all the non duplicated messages
	for (int ind_mess = 0; ind_mess < current_depth_input_stack; ind_mess++) {
		messString = input_message_stack[ind_mess];

		if (IP_message_trace)
		{
			printf("process message [%s]\n", messString);
		}

		// general message just as in an ordinary script
		int         indChar = 0;
		int         p_c = ' ';
		int         message_length = int(strlen(messString));

		////////////////////////
		// parsing the associated command
		// first skipping blank characters or comments
		// SpaceCommentsInclude(NULL,&p_c,messString,&indChar);
		while (_SpaceChar(p_c)) {
			p_c = messString[indChar];
			indChar++;
		}

		///////////////////////////////////////
		// OSC message format
		if (p_c == '/') {
			// ignores leading /
			messString += 1;
			indChar += 1;

			// int          tagLength = strlen( messString );
			char         *pch;

			// removes string termination: ; + \n
			if ((pch = strchr(messString, ';'))) {
				*pch = '\0';
			}

			// computes the length of the message tag
			int tagLength = 0;
			if ((pch = strchr(messString, ' '))) {
				tagLength = int(pch - messString);
				p_c = ' ';
			}
			else {
				tagLength = int(strlen(messString));
				p_c = 0;
			}

			strncpy(OSCTag, messString, tagLength);
			OSCTag[tagLength] = 0;
			messString += tagLength;
			indChar += tagLength;
			// printf( "message command [%s] length [%d]\n" , OSCTag , tagLength );

			// printf( "OSC parameters %s\n" , pch );
			int indOSCParam = 0;
			// printf( "p_c %d (%s) mess (%s) pch %c indChar %d\n" , pch , pch ,  messString , p_c ,indChar );
			while (p_c == ' ') {
				messString += 1;
				indChar += 1;
				p_c = *messString;
			}
			// printf( "p_c %d (%s) mess (%s) pch %c indChar %d\n" , pch , pch , messString ,  p_c ,indChar );

			while (indOSCParam < MAX_OSC_ARGUMENTS
				&& p_c
				&& indChar < message_length + 1) {
				// computes the length of the message tag
				tagLength = 0;
				if ((pch = strchr(messString, ' '))) {
					tagLength = int(pch - messString);
					p_c = ' ';
				}
				else {
					tagLength = int(strlen(messString));
					p_c = 0;
				}

				strncpy(OSC_arguments[indOSCParam], messString, tagLength);
				OSC_arguments[indOSCParam][tagLength] = 0;
				messString += tagLength;
				indChar += tagLength;
				// printf( "message command [%s] length [%d]\n" , OSC_arguments[indOSCParam] , tagLength );

				// printf( "p_c %d (%s) mess (%s) pch %c indChar %d\n" , pch , pch  , messString ,  p_c ,indChar );
				while (p_c == ' ') {
					messString += 1;
					indChar += 1;
					p_c = *messString;
				}
				// printf( "p_c %d (%s) mess (%s) pch %c indChar %d\n" , pch , pch , messString ,  p_c ,indChar );

				indOSCParam++;
			}

			if (p_c) {
				sprintf(ErrorStr, "Error: OSC maximal parameter count too low (%d)!", MAX_OSC_ARGUMENTS); ReportError(ErrorStr);
			}

			// printf("Alias command %s size %d :\n" , OSCTag , indOSCParam );
			// for( int ind = 0 ; ind < indOSCParam ; ind++ ) {
			// printf("%s \n" , OSC_arguments[ind] );
			// }

			float args[MAX_OSC_ARGUMENTS];
			for (int ind = 0; ind < MAX_OSC_ARGUMENTS; ind++) {
				args[ind] = (float)atof(OSC_arguments[ind]);
			}
			pg_aliasScript(OSCTag, OSC_arguments[0], args);
		}
		else {
			sprintf(ErrorStr, "Error: incorrect external message syntax %s!", messString); ReportError(ErrorStr); break;
		}
	}
}

