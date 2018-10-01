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
  if( *target && (strlen( *target ) < strlen( source ) ) ) {
    delete [] *target;
    *target = NULL;
  }
  int str_l = int(strlen( source ) + 1);
  if( !(*target) ) {
    *target = new char[ str_l ];
  }
  memcpy( *target , source , str_l * sizeof( char ) );
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

void pg_IPClient::InitClient( void ) {

  //////////////////////////////
  // remote server opening

  struct                  addrinfo *result;
  struct                  addrinfo hints;

  memset(&hints, 0, sizeof hints); // make sure the struct is empty
  hints.ai_family = AF_INET;     // don't care IPv4 or IPv6
  hints.ai_socktype = SOCK_DGRAM; // UDP stream sockets
  char Remote_server_port_string[128];
  sprintf(Remote_server_port_string, "%d", Remote_server_port);
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

	// system initializing
	IP_OutputStackInitialization();
    }
  }
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

			if (send_format == Plain) {
				strncpy(Output_Message_String, output_message_stack[0],
					max_network_message_length - 1);
			}
			else if (send_format == OSC) {
				// sends bundle through OSC
				// message that does not begin by an OSC address
				if (*(output_message_stack[0]) != '/') {
					strcpy(Output_Message_OSC, "#bundle");
					for (int ind = int(strlen("#bundle")); ind < 20 - 1; ind++) {
						Output_Message_OSC[ind] = 0;
					}
					strncpy(Output_Message_OSC + 20, output_message_stack[0],
						2 * max_network_message_length - 21);
					Output_Message_OSC[20 - 1] = (char)strlen(Output_Message_OSC + 20);
				}

				// sends OSC message through OSC
				// message that begins by an OSC address /aaaa/aaa 
				else {
					// whether or not the pattern is provided with the command
					bool hasPattern = false;
					char *patternOut = NULL;
					char next_char_pattern = 0;
					int inChPattern = 0;
					if (*(output_pattern_stack[0])) {
						// printf( "Pattern %s\n" , output_pattern_stack[ 0 ] );
						hasPattern = true;
						patternOut = output_pattern_stack[0];
						next_char_pattern = *patternOut;
						inChPattern = 1;

						// first pattern type
						while (next_char_pattern && next_char_pattern == ' ') {
							next_char_pattern = patternOut[inChPattern++];
						}
					}

					// messageOut: message formatted as a string /xxx argi
					// will be converted into the OSC format
					// /xxxx000,f00argi0000
					char *messageOut = output_message_stack[0];
					// inChSource: the end of the message address
					int inChSource = int(strlen(messageOut));
					char *pch;
					if ((pch = strchr(messageOut, ' '))) {
						inChSource = int(pch - messageOut);
					}

					// copies message address
					strncpy(Output_Message_OSC, messageOut, inChSource);

					// pads with null chars
					indLocalCommandLine = inChSource;
					Output_Message_OSC[indLocalCommandLine++] = 0;
					while (indLocalCommandLine % 4 != 0) {
						Output_Message_OSC[indLocalCommandLine] = 0;
						indLocalCommandLine++;
					}
					Output_Message_OSC[indLocalCommandLine++] = ',';

					// the send_message_udp has a "pattern" attribute
					if (hasPattern) {
						// reads the arguments and builds two strings
						// one with the types and one with the values
						int p_c;
						int nbArg = 0;
						int indArgList = 0;
						inChSource++;
						p_c = messageOut[inChSource++];
						while ((p_c != (int)EOF) && p_c) {
							// printf( "next_char_pattern %c\n" , next_char_pattern );
							// SpaceCommentsInclude(NULL,&p_c,messageOut,&inChSource);
							while (_SpaceChar(p_c)) {
								p_c = messageOut[inChSource++];
							}
							// printf( "p_c %c\n" , p_c );
							// float: f type in pattern or number
							if (next_char_pattern == 'f'
								|| (!next_char_pattern
									&& (isdigit(p_c) || p_c == '-'))) {
								float f = (float)pg_ScanFloatString(&p_c, true,
									messageOut, &inChSource);
								Output_Message_Pattern[nbArg++] = 'f';
								char *number = (char *)(&f);
								if (OSC_endian_reversal) {
									Output_Message_ArgList[indArgList + 0] = number[3];
									Output_Message_ArgList[indArgList + 1] = number[2];
									Output_Message_ArgList[indArgList + 2] = number[1];
									Output_Message_ArgList[indArgList + 3] = number[0];
								}
								else {
									Output_Message_ArgList[indArgList + 0] = number[0];
									Output_Message_ArgList[indArgList + 1] = number[1];
									Output_Message_ArgList[indArgList + 2] = number[2];
									Output_Message_ArgList[indArgList + 3] = number[3];
								}
								indArgList += 4;

								// printf( "argt #%d float %.4f char %c ind %d\n" , nbArg , f , p_c , indArgList);
							}
							// integer: i type in pattern
							else if (next_char_pattern == 'i') {
								int i = (int)pg_ScanIntegerString(&p_c, true,
									messageOut, &inChSource);
								Output_Message_Pattern[nbArg++] = 'i';
								char *number = (char *)(&i);
								if (OSC_endian_reversal) {
									Output_Message_ArgList[indArgList + 0] = number[3];
									Output_Message_ArgList[indArgList + 1] = number[2];
									Output_Message_ArgList[indArgList + 2] = number[1];
									Output_Message_ArgList[indArgList + 3] = number[0];
								}
								else {
									Output_Message_ArgList[indArgList + 0] = number[0];
									Output_Message_ArgList[indArgList + 1] = number[1];
									Output_Message_ArgList[indArgList + 2] = number[2];
									Output_Message_ArgList[indArgList + 3] = number[3];
								}
								indArgList += 4;

								// printf( "argt #%d float %.4f char %c ind %d\n" , nbArg , f , p_c , indArgList);
							}
							// string: anything else
							else {
								// printf( "ReadStringWithFrontiers p_c %c\n" , p_c );
								char temporaryValue[StringLength];
								*temporaryValue = 0;
								if (messageOut[inChSource] == '"') {
									int i = 0;
									p_c = messageOut[inChSource++];
									while (p_c != '"' && i < StringLength - 1 && p_c != 0) {
										temporaryValue[i++] = char(p_c);
										p_c = messageOut[inChSource++];
									}
									if (i == StringLength - 1) {
										sprintf(ErrorStr, "Error: max string size exceeded ReadStringWithFrontiers (max %d chars)!", StringLength); ReportError(ErrorStr); throw(100);
									}
									temporaryValue[i] = '\0';
									p_c = messageOut[inChSource++];
									while (_SpaceChar(p_c)) {
										p_c = messageOut[inChSource++];
									}
								}
								else {
									int i = 0;
									p_c = messageOut[inChSource++];
									while (p_c != ' ' && i < StringLength - 1 && p_c != 0) {
										temporaryValue[i++] = char(p_c);
										p_c = messageOut[inChSource++];
									}
									if (i == StringLength - 1) {
										sprintf(ErrorStr, "Error: max string size exceeded ReadStringWithFrontiers (max %d chars)!", StringLength); ReportError(ErrorStr); throw(100);
									}
									temporaryValue[i] = '\0';
									p_c = messageOut[inChSource++];
									while (_SpaceChar(p_c)) {
										p_c = messageOut[inChSource++];
									}
								}

								strcpy(Output_Message_ArgList + indArgList, temporaryValue);

								Output_Message_Pattern[nbArg++] = 's';
								// int indArgListIni = indArgList;
								indArgList += int(strlen(Output_Message_ArgList + indArgList));
								Output_Message_ArgList[indArgList++] = 0;
								// pads with 0s 
								while (indArgList % 4 != 0) {
									Output_Message_ArgList[indArgList] = 0;
									indArgList++;
								}

								// printf( "argt #%d str %s char %c ind %d\n" , nbArg , Output_Message_ArgList + indArgList , p_c , indArgList);

							}

							// next pattern type
							if (patternOut && next_char_pattern) {
								next_char_pattern = patternOut[inChPattern++];
								while (next_char_pattern && next_char_pattern == ' ') {
									next_char_pattern = patternOut[inChPattern++];
								}
							}
						}

						// copies argts types
						if ((indLocalCommandLine + 4 + nbArg + indArgList)
							< (2 * max_network_message_length - 1)) {
							memcpy(Output_Message_OSC + indLocalCommandLine,
								Output_Message_Pattern, nbArg);
							indLocalCommandLine += nbArg;
							Output_Message_OSC[indLocalCommandLine++] = 0;

							// pads with null chars
							while (indLocalCommandLine % 4 != 0) {
								Output_Message_OSC[indLocalCommandLine] = 0;
								indLocalCommandLine++;
							}

							// copies argts list
							memcpy(Output_Message_OSC + indLocalCommandLine,
								Output_Message_ArgList, indArgList);
							indLocalCommandLine += indArgList;
						}
						else {
							sprintf(ErrorStr, "Error: udp client: OSC message incompletely sent [%s]!", Output_Message_OSC); ReportError(ErrorStr); // close(SocketToRemoteServer);
						}

						// trace
						// 	    printf("udp client: " );
						// 	    	  for( int ind = 0 ; ind < indLocalCommandLine ; ind++ ) {
						// 	    	    printf( "%c(%d) " ,Output_Message_OSC[ind] , (int)localCommandLine[ind] );
						// 	    	  }
						// 	    	  printf( "\n" );
					}
					// the send_message_udp does not have a "pattern" attribute
					else {
						// makes an OSC command with no arguments
						// pads with null chars
						while (indLocalCommandLine % 4 != 0) {
							Output_Message_OSC[indLocalCommandLine] = 0;
							indLocalCommandLine++;
						}
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
			else if (send_format == OSC && *Output_Message_OSC == '#') {
				length = int(20 + strlen(Output_Message_OSC + 20));
				int inchar;
				for (inchar = length; inchar < length + (4 - length % 4) - 1;
					inchar++) {
					Output_Message_OSC[inchar] = ' ';
				}
				Output_Message_OSC[length + (4 - length % 4) - 1] = 0;
				Output_Message_OSC[20 - 1] = (char)(strlen(Output_Message_OSC + 20) + 1);
				localCommandLineLength = length + (4 - length % 4);
				//printf( "message size %d\n" , strlen(Output_Message_OSC + 20 )  + 1 );
			}
			// OSC message
			else if (send_format == OSC && *Output_Message_OSC == '/') {
				localCommandLineLength = indLocalCommandLine;
			}

			// sends the command
			int rc = 0;
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
			else if (send_format == OSC && *Output_Message_OSC == '#') {
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
			// OSC message
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

			// records the emission time
			last_IP_message_time = RealTime();
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

  // acknowledgement of received message 
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

void pg_IPServer::InitServer( void ) {
  ///////////////////////////////
  // local server creation
  struct sockaddr_in localServAddr;

#ifndef _WIN32
  /* socket creation */
  SocketToLocalServer = socket(AF_INET,SOCK_DGRAM,0);

  if(SocketToLocalServer < 0) {
    sprintf( ErrorStr , "Error: cannot open socket to local server!" ); ReportError( ErrorStr );
  }
  else {
    int SocketToLocalServerFlags;
    
    // Read the socket's flags
    SocketToLocalServerFlags = fcntl(SocketToLocalServer, F_GETFL, 0);
    // Sets the socket's flags to non-blocking
    SocketToLocalServerFlags |= O_NONBLOCK;
    int ret = fcntl(SocketToLocalServer, F_SETFL, SocketToLocalServerFlags );
    if(ret < 0) {
      sprintf( ErrorStr , "Error: local server cannot set flag to non-blocking: %s!" , strerror(errno) ); ReportError( ErrorStr );
    }
  }
#else
  /* socket creation */
  SocketToLocalServer = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);

  if(SocketToLocalServer < 0) {
    sprintf( ErrorStr , "Error: udp server cannot open socket to local server!" ); ReportError( ErrorStr );
  }
  else {
    // Read the socket's flags
    unsigned long onoff=1;
    
    if (ioctlsocket(SocketToLocalServer, FIONBIO, &onoff) != 0){
      sprintf( ErrorStr , "Error: udp server cannot set flag to non-blocking: %s!" , strerror(errno)); ReportError( ErrorStr );
    }
  }
#endif
    
  /* bind local server port */
  localServAddr.sin_family = AF_INET;
  localServAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  localServAddr.sin_port = htons(u_short(Local_server_port));

  int rc = bind( SocketToLocalServer, (struct sockaddr *) &localServAddr,
		 sizeof(localServAddr) );
  if(rc < 0) {
    sprintf( ErrorStr , "Error: cannot bind local port number %d!" , Local_server_port ); ReportError( ErrorStr ); 
    return;
  }
  // printf( "Network server: bound local port number %d\n" , Local_server_port );

  printf("Network server: listening on port %u\n", Local_server_port);

  IP_InputStackInitialization();

  for( int ind = 0 ; ind < MAX_OSC_ARGUMENTS ; ind++ ) {
    OSC_arguments[ind] = new char[ StringLength ];
    *(OSC_arguments[ind]) = 0;
  }
  OSCTag = new char[ StringLength ];
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

void pg_IPServer::processReceivedOSC(char *localCommandLine,
	char *message, int n) {
	// printf("processReceivedOSC [%s] [%d]\n", message , n ); 

	// copies address pattern that terminates by a NULL char
	strcpy(localCommandLine, message);
	// first char after address pattern
	int indChar = int(strlen(message));
	// jumps over the null chars after the add pattern
	while (indChar < n && message[indChar] == 0) {
		indChar++;
	}
	if (indChar < n && message[indChar] == ',') {
		// jumps over the comma
		indChar++;

		// counts the arguments and stores their types
		int nbParam = 0;
		char paramTypesId[StringLength];
		char argument[StringLength];
		while (indChar < n && message[indChar] != 0
			&& nbParam < StringLength) {
			// and counts and stores the parameters
			paramTypesId[nbParam] = message[indChar];
			// printf("param #%d [%c]\n", nbParam + 1 , message[indChar] ); 
			nbParam++; indChar++;
		}
		// ignores the closing null char
		indChar++;

		// stores the text form of the parameters in the
		// local command line
		for (int indParam = 0; indParam < nbParam; indParam++) {
			// jumps over the null chars after the address pattern
			while (indChar < n && message[indChar] == 0
				&& indChar % 4 != 0) {
				indChar++;
			}
			strcat(localCommandLine, " ");
			char reversed_endian[4];
			if (paramTypesId[indParam] == 'i') {
				if (OSC_endian_reversal) {
					reversed_endian[3] = message[indChar + 0];
					reversed_endian[2] = message[indChar + 1];
					reversed_endian[1] = message[indChar + 2];
					reversed_endian[0] = message[indChar + 3];
				}
				else {
					reversed_endian[0] = message[indChar + 0];
					reversed_endian[1] = message[indChar + 1];
					reversed_endian[2] = message[indChar + 2];
					reversed_endian[3] = message[indChar + 3];
				}
				int *i = (int *)(reversed_endian);
				sprintf(argument, "%d", *i);
				// printf("i argument #%d [%s] at %d\n", indParam + 1 , argument , indChar ); 
				strcat(localCommandLine, argument);
				indChar += 4;
			}
			else if (paramTypesId[indParam] == 'f') {
				if (OSC_endian_reversal) {
					reversed_endian[3] = message[indChar + 0];
					reversed_endian[2] = message[indChar + 1];
					reversed_endian[1] = message[indChar + 2];
					reversed_endian[0] = message[indChar + 3];
				}
				else {
					reversed_endian[0] = message[indChar + 0];
					reversed_endian[1] = message[indChar + 1];
					reversed_endian[2] = message[indChar + 2];
					reversed_endian[3] = message[indChar + 3];
				}
				float *f = (float *)(reversed_endian);
				sprintf(argument, "%.5f", *f);
				// printf("f argument #%d [%s] at %d %d\n", indParam + 1 , argument , indChar , sizeof( float ) ); 
				strcat(localCommandLine, argument);
				indChar += 4;
			}
			else if (paramTypesId[indParam] == 's') {
				// printf("s argument #%d [%s] at %d\n", indParam + 1 , message + indChar , indChar );
				if (*(message + indChar) != '\"') {
					strcat(localCommandLine, "\"");
					strcat(localCommandLine, message + indChar);
					strcat(localCommandLine, "\"");
				}
				else {
					strcat(localCommandLine, message + indChar);
				}
				indChar += int(strlen(message + indChar) + 1);
			}
		}
	}
	//printf("local commandLine\n", indParam + 1 , argument , indCh
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
	if (SocketToLocalServer < 0) {
		return;
	}

	// string initialization to 0
	memset(Input_Message_String, 0x0, max_network_message_length);
	memset(Input_Message_Local_Commande_String, 0x0, max_network_message_length);

	// receive message
	int n = receiveOneMessage(Input_Message_String);

	// initializes the stack in which message strings are stored
	current_depth_input_stack = 0;

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

		///////////////////////////////////////////////////////
		// OSC format: bundle
		else if (receive_format == OSC && *Input_Message_String == '#') {
			int length;

			if (n <= 20) {
				sprintf(ErrorStr, "Error: incorrect OSC format of UDP message (incorrect initial length %d)! Check input format in configuration file!", n); ReportError(ErrorStr);
				printf("udp server: [%s] \n", Input_Message_String);
				for (int ind = 0; ind < n; ind++) {
					printf("%c(%d) ", Input_Message_String[ind],
						(int)Input_Message_String[ind]);
				}
				printf("\n");
				return;
			}

			// jumps over "#bundle" (8 bytes) + time tag (8 bytes) + length  (4 bytes)
			// and goes one byte backward to get the length of the string (its lowest byte)
			int ind = 20;
			length = (int) *((unsigned char *)(Input_Message_String + ind - 1));
			// printf("\nudp server: message length total %d used %d\n", n , length);
			// //print received message
	  //       printf("udp server: " );
	  //       for( int ind = 0 ; ind < n ; ind++ ) {
	  // 	printf( "%d: [%c] (%d)\n" , ind , Input_Message_String[ind] , (int)Input_Message_String[ind] );
	  //       }

			while (length <= n - ind) {
				//	sprintf(ErrorStr, "Error: incorrect OSC format of UDP message (incorrect message length)! Check input format in configuration file!"); ReportError(ErrorStr);
				//	printf("udp server: [%s] \n", Input_Message_String);
				//	for (int ind = 0; ind < n; ind++) {
				//		printf("%d: [%c] (%d)\n", ind, Input_Message_String[ind], (int)Input_Message_String[ind]);
				//	}
				//	return;
				//}

				// *(Input_Message_String + 20 + length) = 0;
				// standard IP message
				if (*(Input_Message_String + ind) == '/') {
					processReceivedOSC(Input_Message_Local_Commande_String,
									   Input_Message_String + ind, length);

					if (IP_message_trace) {
						printf("store OSC message bundle [%s]\n", Input_Message_Local_Commande_String);
					}
					storeIP_input_messages_and_removes_duplicates(Input_Message_Local_Commande_String);
				}
				else {
					sprintf(ErrorStr, "Error: UDP input message expected in OSC format (%s)!", Input_Message_String + 20); ReportError(ErrorStr);
				}
				ind += length + 4;
				if (ind < n) {
					length = (int) *((unsigned char *)(Input_Message_String + ind - 1));
				}
				else {
					break;
				}
			}
		}

		///////////////////////////////////////////////////////
		// OSC format: message
		else if (receive_format == OSC && *Input_Message_String == '/') {
			// OSC to string conversion for uniform processing of messages
			processReceivedOSC(Input_Message_Local_Commande_String,
				Input_Message_String, n);

			if (IP_message_trace) {
				printf("store OSC message [%s]\n", Input_Message_Local_Commande_String);
			}
			storeIP_input_messages_and_removes_duplicates(Input_Message_Local_Commande_String);
		}


		memset(Input_Message_String, 0x0, max_network_message_length);
		n = receiveOneMessage(Input_Message_String);
	}

	// forwards the non duplicated messages to the script manager
	// either some predefined commands or scenario variable assignment
	ProcessFilteredInputMessages();
}

void pg_IPServer::storeIP_input_messages_and_removes_duplicates(char *message) {
	if (SocketToLocalServer < 0) {
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

void pg_IPServer::ProcessFilteredInputMessages( void ) {
  char *messString;

  if( SocketToLocalServer < 0 ) {
    return;
  }

  // processes all the non duplicated messages
  for( int ind_mess = 0 ; ind_mess < current_depth_input_stack ; ind_mess++ ) {
    messString = input_message_stack[ ind_mess ];

    if( IP_message_trace ) {
      printf( "process message [%s]\n" , messString );
    }
    
    // general message just as in an ordinary script
    int         indChar = 0;
    int         p_c = ' ';
	int         message_length = int(strlen(messString));

    ////////////////////////
    // parsing the associated command
    // first skipping blank characters or comments
    // SpaceCommentsInclude(NULL,&p_c,messString,&indChar);
    while( _SpaceChar(p_c) ) {
      p_c = messString[ indChar ];
      indChar++;
    }

    ///////////////////////////////////////
    // OSC message format
    if( p_c == '/' ) {
      // ignores leading /
      messString += 1;
      indChar += 1;

      // int          tagLength = strlen( messString );
      char         *pch;

      // removes string termination: ; + \n
      if( (pch = strchr( messString ,  ';' ) ) ) {
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

      strncpy( OSCTag , messString , tagLength );
      OSCTag[ tagLength ] = 0;
      messString += tagLength;
      indChar += tagLength;
      // printf( "message command [%s] length [%d]\n" , OSCTag , tagLength );

      // printf( "OSC parameters %s\n" , pch );
      int indOSCParam = 0;
      // printf( "p_c %d (%s) mess (%s) pch %c indChar %d\n" , pch , pch ,  messString , p_c ,indChar );
      while( p_c == ' ' ) {
	messString += 1;
	indChar += 1;
	p_c = *messString;
      }
      // printf( "p_c %d (%s) mess (%s) pch %c indChar %d\n" , pch , pch , messString ,  p_c ,indChar );

      while( indOSCParam < MAX_OSC_ARGUMENTS 
	     && p_c
	     && indChar < message_length + 1 ) {
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

	strncpy( OSC_arguments[indOSCParam] , messString , tagLength );
	OSC_arguments[indOSCParam][ tagLength ] = 0;
	messString += tagLength;
	indChar += tagLength;
	// printf( "message command [%s] length [%d]\n" , OSC_arguments[indOSCParam] , tagLength );

	// printf( "p_c %d (%s) mess (%s) pch %c indChar %d\n" , pch , pch  , messString ,  p_c ,indChar );
	while( p_c == ' ' ) {
	  messString += 1;
	  indChar += 1;
	  p_c = *messString;
	}
	// printf( "p_c %d (%s) mess (%s) pch %c indChar %d\n" , pch , pch , messString ,  p_c ,indChar );

	indOSCParam++;
      }
      
      if( p_c ) {
	sprintf( ErrorStr , "Error: OSC maximal parameter count too low (%d)!" , MAX_OSC_ARGUMENTS); ReportError( ErrorStr ); 
      }

      // printf("Alias command %s size %d :\n" , OSCTag , indOSCParam );
      // for( int ind = 0 ; ind < indOSCParam ; ind++ ) {
	// printf("%s \n" , OSC_arguments[ind] );
      // }

      float args[MAX_OSC_ARGUMENTS];
      for( int ind = 0 ; ind < MAX_OSC_ARGUMENTS ; ind++ ) {
	  args[ind] = (float)atof( OSC_arguments[ind] );
      }
      pg_aliasScript( OSCTag , OSC_arguments[0] , args );
    }
    else {
      sprintf( ErrorStr , "Error: incorrect external message syntax %s!" , messString ); ReportError( ErrorStr ) ; break;
    }
  }    
}  

