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

#include <iostream>
#include <cstdlib>
#include <vector>
#include <QTime>

#include "pg-interface-atelPort-window.h"
#include "pg-interface-atelPort-udp.h"

typedef  char *     Pchar;

// Input/Output network message string
char *Input_Message_String = NULL;
char *Input_Message_Local_Commande_String = NULL;
char *Output_Message_String = NULL;
char *Output_Message_OSC = NULL;
char *Output_Message_Pattern = NULL;
char *Output_Message_ArgList = NULL;

// message size
#define max_network_message_length 65535

#ifdef _WIN32
int inet_pton(int af, const char *src, void *dst)
{
  struct sockaddr_storage ss;
  int size = sizeof(ss);
  char src_copy[INET6_ADDRSTRLEN+1];
  wchar_t wsrc_copy[INET6_ADDRSTRLEN+1];

  ZeroMemory(&ss, sizeof(ss));
  /* stupid non-const API */
  strncpy (src_copy, src, INET6_ADDRSTRLEN+1);
  src_copy[INET6_ADDRSTRLEN] = 0;
  wcstombs(src_copy,wsrc_copy,INET6_ADDRSTRLEN);

  if (WSAStringToAddress(wsrc_copy, af, NULL, (struct sockaddr *)&ss, &size) == 0) {
    switch(af) {
      case AF_INET:
    *(struct in_addr *)dst = ((struct sockaddr_in *)&ss)->sin_addr;
    return 1;
      case AF_INET6:
    *(struct in6_addr *)dst = ((struct sockaddr_in6 *)&ss)->sin6_addr;
    return 1;
    }
  }
  return 0;
}
#endif



void pg_CopyAndAllocString( char **target , const char *source ) {
    if( !source ) {
        return;
    }
    if( *target && (strlen( *target ) < strlen( source ) ) ) {
        delete [] *target;
        *target = NULL;
    }
    int str_l = strlen( source ) + 1;
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

///////////////////////////////////////////////////////
/// \brief pg_IPClient::pg_IPClient
///
pg_IPClient::pg_IPClient( void ) {
    id = "";

    // remote server address
    Remote_server_IP = "";
    Remote_server_port = 1979;
    Remote_server_host = NULL;

    // remote server socket
    SocketToRemoteServer = -1;

    // client stack: stores pending output messages
    output_pattern_stack = NULL;
    output_message_stack = NULL;
    depth_output_stack = 10;
    current_depth_output_stack = -1;
    maximal_IP_message_delay = 100;
    last_IP_message_time = QTime(0,0);

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
        std::cout << "Error: unknown remote host IP '" << Remote_server_IP.c_str() << " (" << err << ")'!" << std::endl;
    }
    else {
        char                    hname[1024];
        err = getnameinfo(
                    result->ai_addr,             // Pointer to your struct sockaddr
                    result->ai_addrlen,          // Size of this struct
                    hname,                       // Pointer to hostname string
                    sizeof hname,                // Size of this string
                    NULL,                        // Pointer to service name string
                    0,                           // Size of this string
                    0                            // No flags given
                    );
        if (err != 0) {
            std::cout << "Error: unknown remote host IP '" << Remote_server_IP.c_str() << " (" << err << ")'!" << std::endl;
        }
        else {
            /* get server IP address (no check if input is IP address or DNS name */
            // deprecated Remote_server_host = gethostbyname(Remote_server_IP.c_str());
            memset(&remoteServAddr, 0, sizeof(remoteServAddr));
            remoteServAddr.sin_family = AF_INET;
            inet_pton(AF_INET, hname, &(remoteServAddr.sin_addr));
            remoteServAddr.sin_port = htons(Remote_server_port);

            printf("Network client: sending data to '%s' (%s) on port %d\n",
                   Remote_server_IP.c_str(), hname, Remote_server_port);

            /* socket creation */
            //memcpy((char *) &remoteServAddr.sin_addr.s_addr,
            //Remote_server_host->h_addr_list[0], Remote_server_host->h_length);

            SocketToRemoteServer = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
            if (SocketToRemoteServer < 0) {
                std::cout << "Error: cannot open socket to remote server" << std::endl;
            }
            err = connect(SocketToRemoteServer, result->ai_addr, result->ai_addrlen);
            if (err == -1) {
                std::cout << "Error: cannot open socket to remote server" << std::endl;
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

void pg_IPClient::sendIPmessages( void ) {
    if( SocketToRemoteServer < 0 ) {
        std::cout << "no connection to remote server" << std::endl;
        return;
    }

    while( current_depth_output_stack >= 0 ) {
        // printf( "sendUDPmessage %d\n" , current_depth_output_stack );

        // checks whether last message was received
        // or it is the first message
        // or the elapsed time since the last message is greater than the
        bool message_received = true;

        if( message_received
                || current_IP_message_number == first_IP_message_number
                || last_IP_message_time.msecsTo(QTime::currentTime()) > maximal_IP_message_delay ) {


            // std::cout << "send message [" << output_message_stack[ 0 ] << "]" << std::endl;

            // copies the earliest message
            int     indLocalCommandLine = 0;

            if( send_format == Plain ) {
                strncpy( Output_Message_String , output_message_stack[ 0 ] ,
                        max_network_message_length - 1 );
            }
            else if( send_format == OSC ) {
                // sends bundle through OSC
                // message that does not begin by an OSC address
                if( *(output_message_stack[ 0 ]) != '/' ) {
                    strcpy( Output_Message_OSC ,  "#bundle" );
                    for( int ind = strlen( "#bundle" ) ; ind < 20 - 1 ; ind++ ) {
                        Output_Message_OSC[ ind ] = 0;
                    }
                    strncpy( Output_Message_OSC + 20 , output_message_stack[ 0 ] ,
                            2 * max_network_message_length - 21 );
                    Output_Message_OSC[ 20 - 1 ] = (char)strlen( Output_Message_OSC + 20 );
                }

                // sends OSC message through OSC
                // message that begins by an OSC address /aaaa/aaa
                else {
                    // whether or not the pattern is provided with the command
                    bool hasPattern = false;
                    char *patternOut = NULL;
                    char next_char_pattern = 0;
                    int inChPattern = 0;
                    if( *(output_pattern_stack[ 0 ]) ) {
                        // printf( "Pattern %s\n" , output_pattern_stack[ 0 ] );
                        hasPattern = true;
                        patternOut = output_pattern_stack[ 0 ];
                        next_char_pattern = *patternOut;
                        inChPattern = 1;

                        // first pattern type
                        while( next_char_pattern && next_char_pattern == ' ' ) {
                            next_char_pattern = patternOut[ inChPattern++ ];
                        }
                    }

                    // messageOut: message formatted as a string /xxx argi
                    // will be converted into the OSC format
                    // /xxxx000,f00argi0000
                    char *messageOut = output_message_stack[ 0 ];
                    // inChSource: the end of the message address
                    int inChSource = strlen( messageOut );
                    char *pch;
                    if( (pch = strchr( messageOut ,  ' ' ) ) ) {
                        inChSource = pch -  messageOut;
                    }

                    // copies message address
                    strncpy( Output_Message_OSC ,  messageOut , inChSource );

                    // pads with null chars
                    indLocalCommandLine = inChSource;
                    Output_Message_OSC[ indLocalCommandLine++ ] = 0;
                    while( indLocalCommandLine %4 != 0 ) {
                        Output_Message_OSC[ indLocalCommandLine ] = 0;
                        indLocalCommandLine++;
                    }
                    Output_Message_OSC[ indLocalCommandLine++ ] = ',';

                    // the send_message_udp has a "pattern" attribute
                    if( hasPattern ) {
                        // reads the arguments and builds two strings
                        // one with the types and one with the values
                        int p_c;
                        int nbArg = 0;
                        int indArgList = 0;
                        inChSource++;
                        p_c = messageOut[ inChSource++ ];
                        while( (p_c != (int)EOF) && p_c ) {
                            // printf( "next_char_pattern %c\n" , next_char_pattern );
                            // SpaceCommentsInclude(NULL,&p_c,messageOut,&inChSource);
                            while( _SpaceChar(p_c) ) {
                                p_c = messageOut[ inChSource++ ];
                            }
                            // printf( "p_c %c\n" , p_c );
                            // float: f type in pattern or number
                            if( next_char_pattern == 'f'
                                    || (!next_char_pattern
                                        && ( isdigit( p_c) || p_c == '-' ) ) ) {
                                float f = (float) pg_ScanFloatString( &p_c , true ,
                                                                      messageOut , &inChSource );
                                Output_Message_Pattern[ nbArg++ ] = 'f';
                                char *number = (char *)(&f);
                                if( OSC_endian_reversal ) {
                                    Output_Message_ArgList[indArgList+0] = number[3];
                                    Output_Message_ArgList[indArgList+1] = number[2];
                                    Output_Message_ArgList[indArgList+2] = number[1];
                                    Output_Message_ArgList[indArgList+3] = number[0];
                                }
                                else {
                                    Output_Message_ArgList[indArgList+0] = number[0];
                                    Output_Message_ArgList[indArgList+1] = number[1];
                                    Output_Message_ArgList[indArgList+2] = number[2];
                                    Output_Message_ArgList[indArgList+3] = number[3];
                                }
                                indArgList += 4;

                                // printf( "argt #%d float %.4f char %c ind %d\n" , nbArg , f , p_c , indArgList);
                            }
                            // integer: i type in pattern
                            else if( next_char_pattern == 'i' ) {
                                int i = (int) pg_ScanIntegerString( &p_c , true ,
                                                                    messageOut , &inChSource );
                                Output_Message_Pattern[ nbArg++ ] = 'i';
                                char *number = (char *)(&i);
                                if( OSC_endian_reversal ) {
                                    Output_Message_ArgList[indArgList+0] = number[3];
                                    Output_Message_ArgList[indArgList+1] = number[2];
                                    Output_Message_ArgList[indArgList+2] = number[1];
                                    Output_Message_ArgList[indArgList+3] = number[0];
                                }
                                else {
                                    Output_Message_ArgList[indArgList+0] = number[0];
                                    Output_Message_ArgList[indArgList+1] = number[1];
                                    Output_Message_ArgList[indArgList+2] = number[2];
                                    Output_Message_ArgList[indArgList+3] = number[3];
                                }
                                indArgList += 4;

                                // printf( "argt #%d float %.4f char %c ind %d\n" , nbArg , f , p_c , indArgList);
                            }
                            // string: anything else
                            else {
                                // printf( "ReadStringWithFrontiers p_c %c\n" , p_c );
                                char temporaryValue[ StringLength ];
                                *temporaryValue = 0;
                                if( messageOut[ inChSource ] == '"' ) {
                                    int i = 0;
                                    p_c = messageOut[ inChSource++ ];
                                    while( p_c != '"' && i < StringLength - 1  && p_c != 0 ) {
                                        temporaryValue[i++] = p_c;
                                        p_c = messageOut[ inChSource++ ];
                                    }
                                    if( i == StringLength - 1) {
                                        std::cout << "Error: max string size exceeded ReadStringWithFrontiers (max " << StringLength << " chars)!" << std::endl;
                                    }
                                    temporaryValue[i] = '\0';
                                    p_c = messageOut[ inChSource++ ];
                                    while( _SpaceChar(p_c) ) {
                                        p_c = messageOut[ inChSource++ ];
                                    }
                                }
                                else {
                                    int i = 0;
                                    p_c = messageOut[ inChSource++ ];
                                    while( p_c != ' ' && i < StringLength - 1  && p_c != 0 ) {
                                        temporaryValue[i++] = p_c;
                                        p_c = messageOut[ inChSource++ ];
                                    }
                                    if( i == StringLength - 1) {
                                        std::cout << "Error: max string size exceeded ReadStringWithFrontiers (max " << StringLength << ")!" << std::endl;
                                    }
                                    temporaryValue[i] = '\0';
                                    p_c = messageOut[ inChSource++ ];
                                    while( _SpaceChar(p_c) ) {
                                        p_c = messageOut[ inChSource++ ];
                                    }
                                }

                                strcpy( Output_Message_ArgList + indArgList , temporaryValue );

                                Output_Message_Pattern[ nbArg++ ] = 's';
                                // int indArgListIni = indArgList;
                                indArgList += strlen( Output_Message_ArgList + indArgList );
                                Output_Message_ArgList[ indArgList++ ] = 0;
                                // pads with 0s
                                while( indArgList %4 != 0 ) {
                                    Output_Message_ArgList[ indArgList ] = 0;
                                    indArgList++;
                                }

                                // printf( "argt #%d str %s char %c ind %d\n" , nbArg , Output_Message_ArgList + indArgList , p_c , indArgList);

                            }

                            // next pattern type
                            if( patternOut && next_char_pattern ) {
                                next_char_pattern = patternOut[ inChPattern++ ];
                                while( next_char_pattern && next_char_pattern == ' ' ) {
                                    next_char_pattern = patternOut[ inChPattern++ ];
                                }
                            }
                        }

                        // copies argts types
                        if( (indLocalCommandLine + 4 + nbArg + indArgList )
                                < (2 * max_network_message_length - 1 ) ) {
                            memcpy( Output_Message_OSC + indLocalCommandLine ,
                                    Output_Message_Pattern , nbArg );
                            indLocalCommandLine += nbArg;
                            Output_Message_OSC[ indLocalCommandLine++ ] = 0;

                            // pads with null chars
                            while( indLocalCommandLine %4 != 0 ) {
                                Output_Message_OSC[ indLocalCommandLine ] = 0;
                                indLocalCommandLine++;
                            }

                            // copies argts list
                            memcpy( Output_Message_OSC + indLocalCommandLine ,
                                    Output_Message_ArgList , indArgList );
                            indLocalCommandLine += indArgList;
                        }
                        else {
                            std::cout << "Error: cannot udp client: OSC message incompletely sent [" << Output_Message_OSC << "]!" << std::endl;
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
                        while( indLocalCommandLine %4 != 0 ) {
                            Output_Message_OSC[ indLocalCommandLine ] = 0;
                            indLocalCommandLine++;
                        }
                    }
                }
            }

            // FIFO processing of messages
            // shifts the stack down
            for( int ind_mess = 0 ; ind_mess < current_depth_output_stack ;
                 ind_mess++ ) {
                pg_CopyAndAllocString( &(output_message_stack[ ind_mess ]) ,
                                       output_message_stack[ ind_mess + 1 ] );
                pg_CopyAndAllocString( &(output_pattern_stack[ ind_mess ]) ,
                                       output_pattern_stack[ ind_mess + 1 ] );
            }
            current_depth_output_stack--;

            // formats the command
            // pads the length to a multiple of 4
            int length;
            int localCommandLineLength = 0;
            if( send_format == Plain ) {
                length = strlen( Output_Message_String );
                int inchar;
                for( inchar = length; inchar < length + (12 - length % 4) - 1;
                     inchar++ ) {
                    Output_Message_String[inchar] = ' ';
                }
                // pure data
                Output_Message_String[ length + (12 - length % 4) - 2] = ';';
                Output_Message_String[ length + (12 - length % 4) - 1] = '\x0a';
                Output_Message_String[ length + (12 - length % 4)] = 0;
                localCommandLineLength = length + (12 - length % 4);
                // pure data
                // MaxOutput_Message_String[ length + (12 - length % 4) - 1] = 0;
            }
            // OSC bundle
            else if( send_format == OSC && *Output_Message_OSC == '#' ) {
                length = 20 + strlen(Output_Message_OSC + 20 );
                int inchar;
                for( inchar = length; inchar < length + (4 - length % 4) - 1;
                     inchar++ ) {
                    Output_Message_OSC[inchar] = ' ';
                }
                Output_Message_OSC[ length + (4 - length % 4) - 1] = 0;
                Output_Message_OSC[ 20 - 1 ] = (char)(strlen(Output_Message_OSC + 20 ) + 1);
                localCommandLineLength = length + (4 - length % 4);
                //printf( "message size %d\n" , strlen(Output_Message_OSC + 20 )  + 1 );
            }
            // OSC message
            else if( send_format == OSC && *Output_Message_OSC == '/' ) {
                localCommandLineLength = indLocalCommandLine;
            }

            // sends the command
            int rc = 0;
            if( send_format == Plain ) {
                rc = send(SocketToRemoteServer, Output_Message_String,
                          localCommandLineLength, 0);
                //(struct sockaddr *) &remoteServAddr,
                //sizeof(remoteServAddr));
                if(rc != localCommandLineLength) {
                    std::cout << "Error: udp client: data incompletely sent [" << Output_Message_String << "] (" << rc << "/" << localCommandLineLength << ")!" << std::endl;
                }
                if( rc > 0 && IP_message_trace ) {
                    printf( "Network client: send string [%s] (%d bytes)\n",
                            Output_Message_String,(int)strlen(Output_Message_String));
                }
            }
            // OSC bundle
            else if( send_format == OSC && *Output_Message_OSC == '#' ) {
                rc = 0;
                int len = 20 + strlen(Output_Message_OSC + 20) + 1;
                rc = send(SocketToRemoteServer, Output_Message_OSC, len, 0);
                //(struct sockaddr *) &remoteServAddr,
                //sizeof(remoteServAddr));
                if(rc != len) {
                    std::cout << "Error: udp client: OSC data incompletely sent [" << Output_Message_OSC << "]" << std::endl;
                }
                if( rc > 0 && IP_message_trace ) {
                    printf( "udp client: send OSC bundle %d [%s] (%d bytes)\n",
                            (int)*(Output_Message_OSC + 20 - 1) ,
                            Output_Message_OSC + 20 ,
                            (int)(20 + strlen(Output_Message_OSC + 20) + 1) );
                }
            }
            // OSC message
            else if( send_format == OSC && *Output_Message_OSC == '/' ) {
                rc = 0;
                rc = send(SocketToRemoteServer, Output_Message_OSC,
                          localCommandLineLength, 0);
                //(struct sockaddr *) &remoteServAddr,
                //sizeof(remoteServAddr));
                if(rc != localCommandLineLength) {
                    std::cout << "Error: udp client: data incompletely sent [" << Output_Message_OSC << "]!" << std::endl;
                }
                if( rc > 0 && IP_message_trace ) {
                    printf( "udp client: send OSC message (%d bytes)\n",
                            localCommandLineLength );
                    for( int ind = 0 ; ind < localCommandLineLength ; ind++ ) {
                        //printf( "%c(%d) " ,Output_Message_OSC[ind] , (int)Output_Message_OSC[ind] );
                        printf( "%c" ,Output_Message_OSC[ind] );
                    }
                    printf( "\n" );
                }
            }

            // records the emission time
            last_IP_message_time =  QTime::currentTime();
        }
        else {
            if( IP_message_trace ) {
                printf( "udp client: delay %d / %d \n",
                        last_IP_message_time.secsTo(QTime::currentTime()),
                        maximal_IP_message_delay );
            }
            return;
        }
    }
}

void pg_IPClient::storeIP_output_message( QString *commandLine ,
                                          QString *pattern ) {

    // printf( "commandLine %s %d\n" , commandLine , current_depth_output_stack );
    if( SocketToRemoteServer < 0 ) {
        return;
    }

    // printf( "commandLine %s %d\n" , commandLine , current_depth_output_stack );
    if( current_depth_output_stack < depth_output_stack - 1 ) {
        current_depth_output_stack++;
        pg_CopyAndAllocString( &(output_message_stack[ current_depth_output_stack ]) ,
                               commandLine->toStdString().c_str() );
        if( pattern != NULL && (*pattern)[0] != 0) {
            pg_CopyAndAllocString( &(output_pattern_stack[ current_depth_output_stack ]) ,
                                   pattern->toStdString().c_str() );
        }
        else {
            pg_CopyAndAllocString( &(output_pattern_stack[ current_depth_output_stack ]) ,
                                   "" );
        }
    }
    else {
        std::cout << "Error: UDP output message stack overflow " << current_depth_output_stack << " " << depth_output_stack << " (" << commandLine << ")!" << std::endl;
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
    SocketToLocalServer = -1;

    // server stack: stores received input messages
    input_message_stack = NULL;
    input_message_length_stack = NULL;
    depth_input_stack = 30;
    current_depth_input_stack = 0;

    // received message format
    receive_format = Plain;
    offset_IP_message_received = 0;

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
        std::cout << "Error: udp server cannot open socket to local server!" << std::endl;
    }
    else {
        // Read the socket's flags
        unsigned long onoff=1;

        if (ioctlsocket(SocketToLocalServer, FIONBIO, &onoff) != 0){
            std::cout << "Error: udp server cannot set flag to non-blocking: " << strerror(errno) << "!" << std::endl;
        }
    }
#endif
    
    /* bind local server port */
    localServAddr.sin_family = AF_INET;
    localServAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    localServAddr.sin_port = htons(Local_server_port);

    int rc = bind( SocketToLocalServer, (struct sockaddr *) &localServAddr,
                   sizeof(localServAddr) );
    if(rc < 0) {
        std::cout << "Error: cannot bind local port number " << Local_server_port << "!" << std::endl;
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

void pg_IPServer::processReceivedOSC( char *localCommandLine , 
                                      char *message , int n ) {
    // printf("processReceivedOSC [%s] [%d]\n", message , n );

    // copies address pattern that terminates by a NULL char
    strcpy( localCommandLine , message );
    // first char after address pattern
    int indChar = strlen( message );
    // jumps over the null chars after the add pattern
    while( indChar < n && message[indChar] == 0 ) {
        indChar++;
    }
    if( indChar < n && message[indChar] == ',' ) {
        // jumps over the comma
        indChar++;

        // counts the arguments and stores their types
        int nbParam = 0;
        char paramTypesId[ StringLength ];
        char argument[ StringLength ];
        while( indChar < n && message[indChar] != 0
               && nbParam < StringLength ) {
            // and counts and stores the parameters
            paramTypesId[ nbParam ] = message[indChar];
            // printf("param #%d [%c]\n", nbParam + 1 , message[indChar] );
            nbParam++; indChar++;
        }
        // ignores the closing null char
        indChar++;

        // stores the text form of the parameters in the
        // local command line
        for( int indParam = 0 ; indParam < nbParam ; indParam ++ ) {
            // jumps over the null chars after the address pattern
            while( indChar < n && message[indChar] == 0
                   && indChar % 4 != 0 ) {
                indChar++;
            }
            strcat( localCommandLine , " " );
            char reversed_endian[ 4 ];
            if( paramTypesId[ indParam ] == 'i' ) {
                if( OSC_endian_reversal ) {
                    reversed_endian[3] = message[indChar+0];
                    reversed_endian[2] = message[indChar+1];
                    reversed_endian[1] = message[indChar+2];
                    reversed_endian[0] = message[indChar+3];
                }
                else {
                    reversed_endian[0] = message[indChar+0];
                    reversed_endian[1] = message[indChar+1];
                    reversed_endian[2] = message[indChar+2];
                    reversed_endian[3] = message[indChar+3];
                }
                int *i = (int *)(reversed_endian);
                sprintf( argument , "%d" , *i );
                // printf("i argument #%d [%s] at %d\n", indParam + 1 , argument , indChar );
                strcat( localCommandLine , argument );
                indChar += 4;
            }
            else if( paramTypesId[ indParam ] == 'f' ) {
                if( OSC_endian_reversal ) {
                    reversed_endian[3] = message[indChar+0];
                    reversed_endian[2] = message[indChar+1];
                    reversed_endian[1] = message[indChar+2];
                    reversed_endian[0] = message[indChar+3];
                }
                else {
                    reversed_endian[0] = message[indChar+0];
                    reversed_endian[1] = message[indChar+1];
                    reversed_endian[2] = message[indChar+2];
                    reversed_endian[3] = message[indChar+3];
                }
                float *f = (float *)(reversed_endian);
                sprintf( argument , "%.5f" , *f );
                // printf("f argument #%d [%s] at %d %d\n", indParam + 1 , argument , indChar , sizeof( float ) );
                strcat( localCommandLine , argument );
                indChar += 4;
            }
            else if( paramTypesId[ indParam ] == 's' ) {
                // printf("s argument #%d [%s] at %d\n", indParam + 1 , message + indChar , indChar );
                if( *(message + indChar) != '\"' ) {
                    strcat( localCommandLine , "\"" );
                    strcat( localCommandLine , message + indChar );
                    strcat( localCommandLine , "\"" );
                }
                else {
                    strcat( localCommandLine , message + indChar );
                }
                indChar += strlen( message + indChar ) + 1;
            }
        }
    }
    //printf("local commandLine\n", indParam + 1 , argument , indCh
}

int pg_IPServer::receiveOneMessage( char *message ) {
    // message length
    int n = 0;

    // one shot UDP reception
    n = recv(SocketToLocalServer, message,
             max_network_message_length, 0);

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

    // IP_message_trace = true;
    while (n > 0) {

        ///////////////////////////////////////////////////////
        // Plain format (string)
        if (receive_format == Plain) {
            if (IP_message_trace) {
                printf("udp server: receive plain message [%s]\n",
                       Input_Message_String + offset_IP_message_received);
            }
            storeIP_input_messages_and_removes_duplicates(Input_Message_String);
        }

        ///////////////////////////////////////////////////////
        // OSC format: bundle
        else if (receive_format == OSC && *Input_Message_String == '#') {
            int length;

            if (n <= 20) {
                std::cout << "Error: incorrect OSC format of UDP message (incorrect initial length " << n << ")! Check input format in configuration file!" << std::endl;
                printf("udp server: [%s] \n", Input_Message_String);
                for (int ind = 0; ind < n; ind++) {
                    printf("%c(%d) ", Input_Message_String[ind],
                           (int)Input_Message_String[ind]);
                }
                printf("\n");
                return;
            }

            length = (int) *((unsigned char *)(Input_Message_String + 20 - 1));
            // printf("\nudp server: message length total %d used %d\n", n , length);
            // //print received message
            //       printf("udp server: " );
            //       for( int ind = 0 ; ind < n ; ind++ ) {
            // 	printf( "%d: [%c] (%d)\n" , ind , Input_Message_String[ind] , (int)Input_Message_String[ind] );
            //       }


            if (length > n) {
                std::cout << "Error: incorrect OSC format of UDP message (incorrect message length)! Check input format in configuration file!" << std::endl;
                printf("udp server: [%s] \n", Input_Message_String);
                for (int ind = 0; ind < n; ind++) {
                    printf("%d: [%c] (%d)\n", ind, Input_Message_String[ind], (int)Input_Message_String[ind]);
                }
                return;
            }

            *(Input_Message_String + 20 + length) = 0;
            // standard IP message
            if (*(Input_Message_String + 20 + offset_IP_message_received) == '/') {
                processReceivedOSC(Input_Message_Local_Commande_String,
                                   Input_Message_String + 20 + offset_IP_message_received,
                                   length);

                if (IP_message_trace) {
                    printf("store message [%s]\n", Input_Message_Local_Commande_String);
                }
                storeIP_input_messages_and_removes_duplicates(Input_Message_Local_Commande_String);
            }
            else {
                std::cout << "Error: UDP input message expected in OSC format (" << Input_Message_String + 20 + offset_IP_message_received << ")!" << std::endl;
            }
        }

        ///////////////////////////////////////////////////////
        // OSC format: message
        else if (receive_format == OSC && *Input_Message_String == '/') {
            // OSC to string conversion for uniform processing of messages
            processReceivedOSC(Input_Message_Local_Commande_String,
                               Input_Message_String, n);

            if (IP_message_trace) {
                printf("store message [%s]\n", Input_Message_Local_Commande_String);
            }
            storeIP_input_messages_and_removes_duplicates(Input_Message_Local_Commande_String);
        }


        memset(Input_Message_String, 0x0,
               max_network_message_length);
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
    int tagLength = strlen(messString);
    char *pch;
    if ((pch = strchr(messString, ' '))) {
        tagLength = pch - messString;
    }

    //printf( "new message [%s] current_depth_input_stack %d\n" , messString ,
    //	  current_depth_input_stack );

    // duplicates removal for OSC message formats
    // checks whether it has a duplicate and replaces it if there is one
    if (*messString == '/' && *(messString + 1) != '_' && OSC_duplicate_removal) {
        for (int ind_mess = 0; ind_mess < current_depth_input_stack; ind_mess++) {
            // same tag
            if (input_message_length_stack[ind_mess] == tagLength
                    && strncmp(input_message_stack[ind_mess],
                               messString, tagLength) == 0) {
                //printf( "duplicate removed [%s] for [%s]\n" ,  input_message_stack[ ind_mess ] , messString );
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
        std::cout << "Error: UDP input message stack overflow " << current_depth_input_stack << " " << depth_input_stack << "!" << std::endl;
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
        int         message_length = strlen( messString );

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
            if( (pch = strchr( messString ,  ' ' ) ) ) {
                tagLength = pch -  messString;
                p_c = ' ';
            }
            else {
                tagLength = strlen( messString );
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
                if( (pch = strchr( messString ,  ' ' ) ) ) {
                    tagLength = pch -  messString;
                    p_c = ' ';
                }
                else {
                    tagLength = strlen( messString );
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
                std::cout << "Error: OSC maximal parameter count too low (" << MAX_OSC_ARGUMENTS << ")!" << std::endl;
            }

            float args[MAX_OSC_ARGUMENTS];
            for( int ind = 0 ; ind < MAX_OSC_ARGUMENTS ; ind++ ) {
                args[ind] = (float)atof( OSC_arguments[ind] );
            }

            // qDebug() << "Alias script " << OSCTag << " argt " << OSC_arguments[0] << " / " << OSC_arguments[1];
            pg_window->propagateUDPEventsToControllers(OSCTag , OSC_arguments[0] ,  OSC_arguments[1] , args , indOSCParam);
            // qDebug() << "Alias script executed";
        }
        else {
            std::cout << "Error: incorrect external message syntax " << messString << "!" << std::endl;
        }
    }
}  

void MainWindow::propagateUDPEventsToControllers(char *OSCTag, char *firstArgt, char *secondArgt, float args[], int nbargts) {
    QString OSCTagString( OSCTag );
    OSCTagString = "/" + OSCTagString;
    QString firstArgtString( firstArgt );
    QString secondArgtString( secondArgt );

    // << " #2: " << firstArgtString.toStdString() << std::endl;
    //    std::cout << "list size: " << (nbargts + 1) << " #1: ["
    //              << OSCTagString.toStdString() << "]" << std::endl;

    //////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////
    // MESSAGES FOR THE COMMON PARTS OF THE INTERAFACE: COLOR, COUNTDOWN, SCENARIO
    if(OSCTagString == "/pulsed_pen_color_can" && (nbargts + 1) >= 4) {
        graphsTabDisplays->colorPalette->setMixedColor(
                    args[0], args[1], args[2], 0);
        tracksTabDisplays->colorPalette->setMixedColor(
                    args[0], args[1], args[2], 0);
        pathsTabDisplays->colorPalette->setMixedColor(
                    args[0], args[1], args[2], 0);
    }
    else if(OSCTagString == "/pen_grey_can") {
        graphsTabDisplays->colorPalette->setGray(
                    args[0], 0);
        tracksTabDisplays->colorPalette->setGray(
                    args[0], 0);
        pathsTabDisplays->colorPalette->setGray(
                    args[0], 0);
    }
    else if(OSCTagString == "/pen_colorPalette_can" && (nbargts + 1) >= 10) {
        graphsTabDisplays->colorPalette->setPalette(
                    args[0], args[1], args[2],
                    args[3], args[4], args[5],
                    args[6], args[7], args[8], 0);
        tracksTabDisplays->colorPalette->setPalette(
                    args[0], args[1], args[2],
                    args[3], args[4], args[5],
                    args[6], args[7], args[8], 0);
        pathsTabDisplays->colorPalette->setPalette(
                    args[0], args[1], args[2],
                    args[3], args[4], args[5],
                    args[6], args[7], args[8], 0);
    }
    else if(OSCTagString == "/pulsed_repop_color_can" && (nbargts + 1) >= 4) {
        graphsTabDisplays->colorPalette->setMixedColor(
                    args[0], args[1], args[2], 1);
        tracksTabDisplays->colorPalette->setMixedColor(
                    args[0], args[1], args[2], 1);
        pathsTabDisplays->colorPalette->setMixedColor(
                    args[0], args[1], args[2], 1);
    }
    else if(OSCTagString == "/repop_grey_can") {
        graphsTabDisplays->colorPalette->setGray(
                    args[0], 1);
        tracksTabDisplays->colorPalette->setGray(
                    args[0], 1);
        pathsTabDisplays->colorPalette->setGray(
                    args[0], 1);
    }
    else if(OSCTagString == "/repop_colorPalette_can" && (nbargts + 1) >= 10) {
        graphsTabDisplays->colorPalette->setPalette(
                    args[0], args[1], args[2],
                    args[3], args[4], args[5],
                    args[6], args[7], args[8], 1);
        tracksTabDisplays->colorPalette->setPalette(
                    args[0], args[1], args[2],
                    args[3], args[4], args[5],
                    args[6], args[7], args[8], 1);
        pathsTabDisplays->colorPalette->setPalette(
                    args[0], args[1], args[2],
                    args[3], args[4], args[5],
                    args[6], args[7], args[8], 1);
    }
    else if(OSCTagString == "/time_can" && (nbargts + 1) >= 2) {
        graphsTabDisplays->timeSceneBeatDisplay->setTime(firstArgtString);
        tracksTabDisplays->timeSceneBeatDisplay->setTime(firstArgtString);
        pathsTabDisplays->timeSceneBeatDisplay->setTime(firstArgtString);
    }
    else if(OSCTagString == "/time_color_can" && (nbargts + 1) >= 2) {
        graphsTabDisplays->timeSceneBeatDisplay->setColor(int(args[0]));
        tracksTabDisplays->timeSceneBeatDisplay->setColor(int(args[0]));
        pathsTabDisplays->timeSceneBeatDisplay->setColor(int(args[0]));
    }
    else if(OSCTagString == "/setup_can" && (nbargts + 1) >= 2) {
        graphsTabDisplays->timeSceneBeatDisplay->setSceneName(firstArgtString);
        tracksTabDisplays->timeSceneBeatDisplay->setSceneName(firstArgtString);
        pathsTabDisplays->timeSceneBeatDisplay->setSceneName(firstArgtString);
    }
    else if(OSCTagString == "/message_can" && (nbargts + 1) >= 2) {
        graphsTabDisplays->timeSceneBeatDisplay->setMessageText(firstArgtString);
        tracksTabDisplays->timeSceneBeatDisplay->setMessageText(firstArgtString);
        pathsTabDisplays->timeSceneBeatDisplay->setMessageText(firstArgtString);
    }
    else if(OSCTagString == "/loop_beat_can" && (nbargts + 1) >= 2) {
        graphsTabDisplays->timeSceneBeatDisplay->setBeatNo(firstArgtString);
        tracksTabDisplays->timeSceneBeatDisplay->setBeatNo(firstArgtString);
        pathsTabDisplays->timeSceneBeatDisplay->setBeatNo(firstArgtString);
    }
    else if(OSCTagString == "/reset_LEDs") {
        // std::cout << "LEDs reinitialization" << std::endl;
        pg_window->reinitializeMidiLighting(_Graphs_display);
        pg_window->reinitializeMidiLighting(_Tracks_display);
        pg_window->reinitializeMidiLighting(_Paths_display);
    }
    else if(OSCTagString == "/switchOff_LEDs") {
        // std::cout << "LEDs off" << std::endl;
        pg_window->swithOffMidiLighting(_Graphs_display);
        pg_window->swithOffMidiLighting(_Tracks_display);
        pg_window->swithOffMidiLighting(_Paths_display);
    }
    else if(OSCTagString == "/QT_connected") {
        QString message = QString("/QT_connected");
        pg_window->udpInOut->pg_store_message_udp( &message );
    }

    //////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////
    // MESSAGES FOR SLIDERS AND BUTTONS
    std::vector<std::vector<SingleLabel*>> *curLabelRow = NULL;
    for(unsigned int indDisplay = 0; indDisplay < _NumberOfDisplays; indDisplay++) {
        // std::cout << "Display "  << indDisplay << std::endl;
        switch(indDisplay) {
        //////////////////////////////////////////////////////////////
        // GRAPHS DISPLAY
        case _Graphs_display:
            //////////////////////////////////////////////////////////////
            // messages for the sliders
            for(unsigned int indSliderRow = 0 ; indSliderRow < graphs_mainSlidersData.size() ; indSliderRow++ ) {
                for(unsigned int indCol = 0 ; indCol < graphs_mainSlidersData[indSliderRow].size() ; indCol++ ) {
                    if(graphsTabControllers->verticalSliders_graphs[indSliderRow][indCol]) {
                        // scenario-based value changes
                        if(OSCTagString == graphs_mainSlidersData[indSliderRow][indCol].get_labelUDPIn()) {
                            // statusLabel->setText(tr("Received message: \"%1\"").arg(message));
                            // std::cout << "received UDP message " << OSCTagString.toStdString() << " " << firstArgtString.toStdString() << std::endl;
                            graphsTabControllers->verticalSliders_graphs[indSliderRow][indCol]->setDoubleValueFromUDP(args[0]);
                        }
                        if(OSCTagString == graphsTabControllers->verticalSliders_graphs[indSliderRow][indCol]->getLabelHighlightIn()) {
                            // statusLabel->setText(tr("Received message: [%1] [%2]").arg(OSCTagString).arg(firstArgtString));
                            graphsTabControllers->verticalSliders_graphs[indSliderRow][indCol]->setHighlight(args[0]);
                        }
                    }
                }
            }
            //////////////////////////////////////////////////////////////
            // messages for the buttons
            for(unsigned int indSideButton = 0 ; indSideButton < _NbRows_sideButtons_graphs ; indSideButton++ ) {
                // scenario-based value changes
                if(graphsTabControllers->sideButtons_graphs[indSideButton]
                        && OSCTagString == graphsTabControllers->sideButtons_graphs[indSideButton]->getLabelHighlightIn()) {
                    // statusLabel->setText(tr("Received message: \"%1\"").arg(message));
                    graphsTabControllers->sideButtons_graphs[indSideButton]->setHighlight(args[0]);
                }
            }
            for(unsigned int indMainButtonRow = 0 ; indMainButtonRow < _NbRows_mainButtons_graphs ; indMainButtonRow++ ) {
                for(unsigned int indMainButtonCol = 0 ; indMainButtonCol < graphs_mainButtonsData[indMainButtonRow].size() ; indMainButtonCol++ ) {
                    // scenario-based value changes
                    if(graphsTabControllers->mainButtons_graphs[indMainButtonRow][indMainButtonCol]
                            && OSCTagString == graphsTabControllers->mainButtons_graphs[indMainButtonRow][indMainButtonCol]->getLabelHighlightIn()) {
                        // statusLabel->setText(tr("Received message: [%1] [%2]").arg(OSCTagString).arg(firstArgtString));
                        // qDebug() << "Alias script set highlight";
                        graphsTabControllers->mainButtons_graphs[indMainButtonRow][indMainButtonCol]->setHighlight(args[0]);
                        // qDebug() << "Alias script highlight set";
                    }
                }
            }
            //////////////////////////////////////////////////////////////
            // messages for the LED Bars
            for(unsigned int indRow = _FlashGridRowBGInOutData + 1 ; indRow < graphs_mainSlidersData.size() ; indRow++ ) {
                for(unsigned int indCol = 0 ; indCol < graphs_mainSlidersData[indRow].size() ; indCol++ ) {
                    if(tracksTabDisplays->graphicalLEDBarRows[indRow]) {
                        // scenario-based value changes
                        if(OSCTagString == graphs_mainSlidersData[indRow][indCol].get_labelUDPIn()) {
                            // statusLabel->setText(tr("Received message: \"%1\"").arg(message));
                            graphsTabDisplays->graphicalLEDBarRows[indRow]
                                    ->setValue(indCol,args[0]);
                        }
                        // highlight from porphyrograph
                        LEDBar *curLEDBar = graphsTabDisplays->graphicalLEDBarRows[indRow]->getLEDBar(indCol);
                        if(curLEDBar && OSCTagString == curLEDBar->labelHighlightIn) {
                            // statusLabel->setText(tr("Received message: \"%1\"").arg(message));
                            graphsTabDisplays->graphicalLEDBarRows[indRow]
                                    ->setHighlight(indCol, args[0] == curLEDBar->valueHighlightIn);
                        }
                    }
                }
            }
            //////////////////////////////////////////////////////////////
            // messages for the labels
            curLabelRow = &(graphsTabDisplays->graphsButtonsLabels->labelRow);
            for(unsigned int indMainLabelRow = 0 ; indMainLabelRow < curLabelRow->size() ; indMainLabelRow++ ) {
                for(unsigned int indMainLabelCol = 0 ; indMainLabelCol < (*curLabelRow)[indMainLabelRow].size() ; indMainLabelCol++ ) {
                    // scenario-based value changes
                    if((*curLabelRow)[indMainLabelRow][indMainLabelCol]
                            && OSCTagString == (*curLabelRow)[indMainLabelRow][indMainLabelCol]->getLabelHighlightIn()) {
                        // statusLabel->setText(tr("Received message: [%1] [%2]").arg(OSCTagString).arg(firstArgtString));
                        (*curLabelRow)[indMainLabelRow][indMainLabelCol]->setHighlight(args[0]);
                    }
                }
            }
            //////////////////////////////////////////////////////////////
            // text of the buttons and labels
            if(OSCTagString == "/pixel_mode_can" && (nbargts + 1) >= 2) {
                int value = int(args[0]);
                if(value == 0) {
                    graphsTabControllers->mainButtons_graphs[0][7]->setText("noPix");
                    graphsTabControllers->mainButtons_graphs[0][7]->getAssociatedSingleLabel()->setText("noPix");
                }
                else if(value == 1) {
                    graphsTabControllers->mainButtons_graphs[0][7]->setText("pixel");
                    graphsTabControllers->mainButtons_graphs[0][7]->getAssociatedSingleLabel()->setText("pixel");
                }
                else if(value == 2) {
                    graphsTabControllers->mainButtons_graphs[0][7]->setText("fireWk");
                    graphsTabControllers->mainButtons_graphs[0][7]->getAssociatedSingleLabel()->setText("fireWk");
                }
            }
            if(OSCTagString == "/pen_grey_can" && (nbargts + 1) >= 2) {
                int value = int(args[0]);
                if(value == 0) {
                    graphsTabControllers->mainButtons_graphs[0][0]->setText("Black");
                    graphsTabControllers->mainButtons_graphs[0][0]->getAssociatedSingleLabel()->setText("Blk");
                }
                else {
                    graphsTabControllers->mainButtons_graphs[0][0]->setText("White");
                    graphsTabControllers->mainButtons_graphs[0][0]->getAssociatedSingleLabel()->setText("Wht");
                }
            }
            if(OSCTagString == "/repop_grey_can" && (nbargts + 1) >= 2) {
                int value = int(args[0]);
                if(value == 0) {
                    graphsTabControllers->mainButtons_graphs[1][0]->setText("Black");
                    graphsTabControllers->mainButtons_graphs[1][0]->getAssociatedSingleLabel()->setText("Blk");
                }
                else {
                    graphsTabControllers->mainButtons_graphs[1][0]->setText("White");
                    graphsTabControllers->mainButtons_graphs[1][0]->getAssociatedSingleLabel()->setText("Wht");
                }
            }
            if(OSCTagString == "/freeze_can" && (nbargts + 1) >= 2) {
                int value = int(args[0]);
                if(value == 0) {
                    graphsTabControllers->mainButtons_graphs[1][7]->setText("OFF");
                    graphsTabControllers->mainButtons_graphs[1][7]->getAssociatedSingleLabel()->setText("Off");
                }
                else {
                    graphsTabControllers->mainButtons_graphs[1][7]->setText("ON");
                    graphsTabControllers->mainButtons_graphs[1][7]->getAssociatedSingleLabel()->setText("On");
                }
            }
            if(OSCTagString == "/pen_colorPreset_name_can" && (nbargts + 1) >= 3) {
                int value = int(args[0]);
                if(value < 6) {
                    graphsTabControllers->mainButtons_graphs[0][value + 1]->setText(secondArgtString);
                    graphsTabControllers->mainButtons_graphs[0][value + 1]->getAssociatedSingleLabel()->setText(secondArgtString);
                }
            }
            break;

            //////////////////////////////////////////////////////////////
            // TRACKS DISPLAY
        case _Tracks_display:
            //////////////////////////////////////////////////////////////
            // messages for the sliders
            for(unsigned int indSliderRow = 0 ; indSliderRow < tracks_mainSlidersData.size() ; indSliderRow++ ) {
                for(unsigned int indCol = 0 ; indCol < tracks_mainSlidersData[indSliderRow].size() ; indCol++ ) {
                    if(tracksTabControllers->verticalSliders_tracks[indSliderRow][indCol]) {
                        if(OSCTagString == tracks_mainSlidersData[indSliderRow][indCol].get_labelUDPIn()) {
                            // statusLabel->setText(tr("Received message: \"%1\"").arg(message));
                            tracksTabControllers->verticalSliders_tracks[indSliderRow][indCol]->setDoubleValueFromUDP(args[0]);
                        }
                        if(OSCTagString == tracksTabControllers->verticalSliders_tracks[indSliderRow][indCol]->getLabelHighlightIn()) {
                            // statusLabel->setText(tr("Received message: [%1] [%2]").arg(OSCTagString).arg(firstArgtString));
                            tracksTabControllers->verticalSliders_tracks[indSliderRow][indCol]->setHighlight(args[0]);
                        }
                    }
                }
            }
            //////////////////////////////////////////////////////////////
            // messages for the buttons
            for(unsigned int indSideButton = 0 ; indSideButton < _NbRows_sideButtons_graphs ; indSideButton++ ) {
                if(tracksTabControllers->sideButtons_tracks[indSideButton]
                        && OSCTagString == tracksTabControllers->sideButtons_tracks[indSideButton]->getLabelHighlightIn()) {
                    // statusLabel->setText(tr("Received message: \"%1\"").arg(message));
                    tracksTabControllers->sideButtons_tracks[indSideButton]->setHighlight(args[0]);
                }
            }
            for(unsigned int indMainButtonRow = 0 ; indMainButtonRow < _NbRows_mainButtons_graphs ; indMainButtonRow++ ) {
                for(unsigned int indMainButtonCol = 0 ; indMainButtonCol < tracks_mainButtonsData[indMainButtonRow].size() ; indMainButtonCol++ ) {
                    if(tracksTabControllers->mainButtons_tracks[indMainButtonRow][indMainButtonCol]
                            && OSCTagString == tracksTabControllers->mainButtons_tracks[indMainButtonRow][indMainButtonCol]->getLabelHighlightIn()) {
                        // statusLabel->setText(tr("Received message: [%1] [%2]").arg(OSCTagString).arg(firstArgtString));
                        tracksTabControllers->mainButtons_tracks[indMainButtonRow][indMainButtonCol]->setHighlight(args[0]);
                    }
                }
            }
            //////////////////////////////////////////////////////////////
            // messages for the LED Bars
            for(unsigned int indRow = 0 ; indRow < tracks_mainSlidersData.size() ; indRow++ ) {
                for(unsigned int indCol = 0 ; indCol < tracks_mainSlidersData[indRow].size() ; indCol++ ) {
                    if(tracksTabDisplays->graphicalLEDBarRows[indRow]) {
                        if(OSCTagString == tracks_mainSlidersData[indRow][indCol].get_labelUDPIn()) {
                            // statusLabel->setText(tr("Received message: \"%1\"").arg(message));
                            tracksTabDisplays->graphicalLEDBarRows[indRow]
                                    ->setValue(indCol,args[0]);
                        }
                        // highlight from porphyrograph
                        LEDBar *curLEDBar = tracksTabDisplays->graphicalLEDBarRows[indRow]->getLEDBar(indCol);
                        if(curLEDBar && OSCTagString == curLEDBar->labelHighlightIn) {
                            // statusLabel->setText(tr("Received message: \"%1\"").arg(message));
                            tracksTabDisplays->graphicalLEDBarRows[indRow]
                                    ->setHighlight(indCol, args[0] == curLEDBar->valueHighlightIn);
                        }
                    }
                }
            }
            //////////////////////////////////////////////////////////////
            // messages for the labels
            curLabelRow = &(tracksTabDisplays->tracksButtonsLabels->labelRow);
            for(unsigned int indMainLabelRow = 0 ; indMainLabelRow < curLabelRow->size() ; indMainLabelRow++ ) {
                for(unsigned int indMainLabelCol = 0 ; indMainLabelCol < (*curLabelRow)[indMainLabelRow].size() ; indMainLabelCol++ ) {
                    if((*curLabelRow)[indMainLabelRow][indMainLabelCol]
                            && OSCTagString == (*curLabelRow)[indMainLabelRow][indMainLabelCol]->getLabelHighlightIn()) {
                        // statusLabel->setText(tr("Received message: [%1] [%2]").arg(OSCTagString).arg(firstArgtString));
                        (*curLabelRow)[indMainLabelRow][indMainLabelCol]->setHighlight(args[0]);
                    }
                }
            }
            //////////////////////////////////////////////////////////////
            // text of the buttons and labels
            if(OSCTagString == "/soundtrack_shortName_can" && (nbargts + 1) >= 2) {
                tracksTabControllers->mainButtons_tracks[0][6]->setText(firstArgtString);
                tracksTabControllers->mainButtons_tracks[0][6]->getAssociatedSingleLabel()->setText(firstArgtString);
            }
            else if(OSCTagString == "/movie_shortName_can" && (nbargts + 1) >= 2) {
                tracksTabControllers->mainButtons_tracks[1][7]->setText(firstArgtString);
                tracksTabControllers->mainButtons_tracks[1][7]->getAssociatedSingleLabel()->setText(firstArgtString);
            }
            else if(OSCTagString == "/soundtrack_onOff_can" && (nbargts + 1) >= 2) {
                int value = int(args[0]);
                if(value == 0) {
                    tracksTabControllers->mainButtons_tracks[0][7]->setText("OFF");
                    tracksTabControllers->mainButtons_tracks[0][7]->getAssociatedSingleLabel()->setText("X");
                }
                else {
                    tracksTabControllers->mainButtons_tracks[0][7]->setText("ON");
                    tracksTabControllers->mainButtons_tracks[0][7]->getAssociatedSingleLabel()->setText("ON");
                }
            }
            else if(OSCTagString == "/adc_onOff_can" && (nbargts + 1) >= 2) {
                int value = int(args[0]);
                if(value == 0) {
                    tracksTabControllers->mainButtons_tracks[0][5]->setText("OFF");
                    tracksTabControllers->mainButtons_tracks[0][5]->getAssociatedSingleLabel()->setText("X");
                }
                else {
                    tracksTabControllers->mainButtons_tracks[0][5]->setText("ON");
                    tracksTabControllers->mainButtons_tracks[0][5]->getAssociatedSingleLabel()->setText("ON");
                }
            }
            else if(OSCTagString == "/currentPhotoTrack_can" && (nbargts + 1) >= 2) {
                int value = int(args[0]);
                if(value == 0) {
                    tracksTabControllers->mainButtons_tracks[0][0]->setText("Photo BG");
                    tracksTabControllers->mainButtons_tracks[0][0]->getAssociatedSingleLabel()->setText("Ph0");
                }
                else if(value == 1) {
                    tracksTabControllers->mainButtons_tracks[0][0]->setText("Photo T1");
                    tracksTabControllers->mainButtons_tracks[0][0]->getAssociatedSingleLabel()->setText("Ph1");
                }
                else if(value == 2) {
                    tracksTabControllers->mainButtons_tracks[0][0]->setText("Photo T2");
                    tracksTabControllers->mainButtons_tracks[0][0]->getAssociatedSingleLabel()->setText("Ph2");
                }
            }
            else if(OSCTagString == "/currentVideoTrack_can" && (nbargts + 1) >= 2) {
                int value = int(args[0]);
                if(value == 0) {
                    tracksTabControllers->mainButtons_tracks[0][1]->setText("Video BG");
                    tracksTabControllers->mainButtons_tracks[0][1]->getAssociatedSingleLabel()->setText("Vi0");
                }
                else if(value == 1) {
                    tracksTabControllers->mainButtons_tracks[0][1]->setText("Video T1");
                    tracksTabControllers->mainButtons_tracks[0][1]->getAssociatedSingleLabel()->setText("Vi1");
                }
                else if(value == 2) {
                    tracksTabControllers->mainButtons_tracks[0][1]->setText("Video T2");
                    tracksTabControllers->mainButtons_tracks[0][1]->getAssociatedSingleLabel()->setText("Vi2");
                }
            }
            break;

            //////////////////////////////////////////////////////////////
            // PATHS DISPLAY
        case _Paths_display:
            //////////////////////////////////////////////////////////////
            // messages for the sliders
            // std::cout << "Sliders " << std::endl;
            for(unsigned int indSliderRow = 0 ; indSliderRow < paths_mainSlidersData.size() ; indSliderRow++ ) {
                for(unsigned int indCol = 0 ; indCol < paths_mainSlidersData[indSliderRow].size() ; indCol++ ) {
                    // std::cout << "Slider " << indSliderRow << " " << indCol <<  std::endl;
                    if(pathsTabControllers->verticalSliders_paths[indSliderRow][indCol]) {
                        if(OSCTagString == paths_mainSlidersData[indSliderRow][indCol].get_labelUDPIn()) {
                            // statusLabel->setText(tr("Received message: \"%1\"").arg(message));
                            // std::cout << "set value from UDP (" << indSliderRow << " " << indCol << ") " << args[0] << std::endl;
                            pathsTabControllers->verticalSliders_paths[indSliderRow][indCol]->setDoubleValueFromUDP(args[0]);
                        }
                        if(OSCTagString == pathsTabControllers->verticalSliders_paths[indSliderRow][indCol]->getLabelHighlightIn()) {
                            // statusLabel->setText(tr("Received message: [%1] [%2]").arg(OSCTagString).arg(firstArgtString));
                            pathsTabControllers->verticalSliders_paths[indSliderRow][indCol]->setHighlight(args[0]);
                        }
                    }
                }
            }
            //////////////////////////////////////////////////////////////
            // messages for the buttons
            // std::cout << "Buttons " << std::endl;
            for(unsigned int indMainButtonRow = 0 ; indMainButtonRow < _NbRows_buttons_paths ; indMainButtonRow++ ) {
                for(unsigned int indMainButtonCol = 0 ; indMainButtonCol < paths_mainButtonsData[indMainButtonRow].size() ; indMainButtonCol++ ) {
                    if(pathsTabControllers->mainButtons_paths[indMainButtonRow][indMainButtonCol]
                            && OSCTagString == pathsTabControllers->mainButtons_paths[indMainButtonRow][indMainButtonCol]->getLabelHighlightIn()) {
                        // statusLabel->setText(tr("Received message: \"%1\"").arg(message));
                        pathsTabControllers->mainButtons_paths[indMainButtonRow][indMainButtonCol]->setHighlight(args[0]);
                    }
                }
            }
            //////////////////////////////////////////////////////////////
            // messages for the LED Bars
            // std::cout << "LEDs " << std::endl;
            for(unsigned int indRow = 0 ; indRow < paths_mainSlidersData.size() ; indRow++ ) {
                for(unsigned int indCol = 0 ; indCol < paths_mainSlidersData[indRow].size() ; indCol++ ) {
                    if(pathsTabDisplays->graphicalLEDBarRows[indRow]
                            && OSCTagString == paths_mainSlidersData[indRow][indCol].get_labelUDPIn()) {
                        // statusLabel->setText(tr("Received message: \"%1\"").arg(message));
                        pathsTabDisplays->graphicalLEDBarRows[indRow]
                                ->setValue(indCol,args[0]);
                    }
                    // highlight from porphyrograph
                    if(pathsTabDisplays->graphicalLEDBarRows[indRow]) {
                        LEDBar *curLEDBar = pathsTabDisplays->graphicalLEDBarRows[indRow]->getLEDBar(indCol);
                        if(curLEDBar && OSCTagString == curLEDBar->labelHighlightIn) {
                            // statusLabel->setText(tr("Received message: \"%1\"").arg(message));
                            pathsTabDisplays->graphicalLEDBarRows[indRow]
                                    ->setHighlight(indCol, args[0] == curLEDBar->valueHighlightIn);
                        }
                    }
                }
            }
            //////////////////////////////////////////////////////////////
            // messages for the labels
            // std::cout << "Labels " << std::endl;
            curLabelRow = &(pathsTabDisplays->pathsButtonsLabels->labelRow);
            for(unsigned int indMainLabelRow = 0 ; indMainLabelRow < curLabelRow->size() ; indMainLabelRow++ ) {
                for(unsigned int indMainLabelCol = 0 ; indMainLabelCol < (*curLabelRow)[indMainLabelRow].size() ; indMainLabelCol++ ) {
                    // scenario-based value changes
                    if((*curLabelRow)[indMainLabelRow][indMainLabelCol]
                            && OSCTagString == (*curLabelRow)[indMainLabelRow][indMainLabelCol]->getLabelHighlightIn()) {
                        // statusLabel->setText(tr("Received message: [%1] [%2]").arg(OSCTagString).arg(firstArgtString));
                        (*curLabelRow)[indMainLabelRow][indMainLabelCol]->setHighlight(args[0]);
                    }
                }
            }
            //////////////////////////////////////////////////////////////
            // text of the buttons and labels
            // std::cout << "Text " << std::endl;
            if(OSCTagString == "/CASubType_can" && (nbargts + 1) >= 2) {
                int value = int(args[0]);
                if(value == 0) {
                    pathsTabControllers->mainButtons_paths[8][3]->setText("OFF");
                    pathsTabControllers->mainButtons_paths[8][3]->getAssociatedSingleLabel()->setText("X");
                }
                else {
                    pathsTabControllers->mainButtons_paths[8][3]->setText("ON");
                    pathsTabControllers->mainButtons_paths[8][3]->getAssociatedSingleLabel()->setText("ON");
                }
            }
#ifndef ATELIERS_PORTATIFS
            if(OSCTagString == "/cameraCumul_can" && (nbargts + 1) >= 2) {
                int value = int(args[0]);
                if(value == 0) {
                    pathsTabControllers->mainButtons_paths[9][4]->setText("CaCu:Off");
                    pathsTabControllers->mainButtons_paths[9][4]->getAssociatedSingleLabel()->setText("CaCu:X");
                }
                else if(value == 1) {
                    pathsTabControllers->mainButtons_paths[9][4]->setText("CaCu:Add");
                    pathsTabControllers->mainButtons_paths[9][4]->getAssociatedSingleLabel()->setText("CaCu:Add");
                }
                else if(value == 2) {
                    pathsTabControllers->mainButtons_paths[9][4]->setText("CaCu:Stmp");
                    pathsTabControllers->mainButtons_paths[9][4]->getAssociatedSingleLabel()->setText("CaCu:Stmp");
                }
                else {
                    pathsTabControllers->mainButtons_paths[9][4]->setText("CaCu:XOR");
                    pathsTabControllers->mainButtons_paths[9][4]->getAssociatedSingleLabel()->setText("CaCu:XOR");
                }
            }
#endif
            break;
        }
    }
    // messages for the Grid
    for(unsigned int indGridRow = 0 ; indGridRow < _FlashGridRowBGInOutData + 1 ; indGridRow++ ) {
        for(unsigned int indCol = 0 ; indCol < graphs_mainSlidersData[indGridRow].size() ; indCol++ ) {
            if(OSCTagString == graphs_mainSlidersData[indGridRow][indCol].get_labelUDPIn()) {
                // statusLabel->setText(tr("Received message: \"%1\"").arg(message));
                graphsTabDisplays->graphsFlashGrid
                        ->setValue(indGridRow + 1,indCol + 1,int(args[0]));
            }
            if(OSCTagString == graphsTabDisplays->graphsFlashGrid
                    ->getLabelHighlightIn(indGridRow + 1,indCol + 1)) {
                // statusLabel->setText(tr("Received message: [%1] [%2]").arg(OSCTagString).arg(firstArgtString));
                graphsTabDisplays->graphsFlashGrid
                        ->setHighlight(indGridRow + 1,indCol + 1,args[0]);
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////////
/// \brief UDPInOut::UDPInOut
/// \param parent
///
UDPInOut::UDPInOut(QWidget *parent)
    : QObject(parent)
{
    nb_IP_Servers = 1;
    IP_Servers = new pg_IPServer *[nb_IP_Servers];
    nb_IP_Clients = 1;
    IP_Clients = new pg_IPClient *[nb_IP_Clients];

    IP_Servers[0] = new pg_IPServer();
    IP_Servers[0]->id = "udp_QT_receive";
    IP_Servers[0]->Local_server_port = 1979;
    IP_Servers[0]->receive_format = Plain;
    IP_Servers[0]->IP_message_trace = false;
    IP_Servers[0]->depth_input_stack = 300;
    IP_Servers[0]->OSC_duplicate_removal = true;
    IP_Servers[0]->OSC_endian_reversal = true;

    IP_Clients[0] = new pg_IPClient();
    IP_Clients[0]->id = "udp_QT_send";
    IP_Clients[0]->Remote_server_IP = "127.0.0.1";
    IP_Clients[0]->Remote_server_port = 1978;
    IP_Clients[0]->send_format = Plain;
    IP_Clients[0]->IP_message_trace = false;
    IP_Clients[0]->depth_output_stack = 300;
    IP_Clients[0]->OSC_endian_reversal = true;

#ifdef WIN32
    WSADATA wsaData;
    // Initialize Winsock
    int err = WSAStartup(MAKEWORD(2,2), &wsaData);
    if ( err != 0 ) {
        /* Tell the user that we could not find a usable */
        /* WinSock DLL.                                  */
        std::cout << "Error: could not find a usable WinSock DLL!" << std::endl;
    }
#endif

    // server initialization
    for( int ind = 0 ; ind < nb_IP_Servers ; ind++ ) {
        IP_Servers[ ind ]->InitServer();
    }
    for( int ind = 0 ; ind < nb_IP_Clients ; ind++ ) {
        // printf( "Client %d initialization\n" , ind );
        //std::cout << "IP_Clients[ ind_IP_Client ]->Remote_server_IP: " << ind << " " << IP_Clients[ind]->Remote_server_IP << "\n";
        //std::cout << "IP_Clients[ ind_IP_Client ]->Remote_server_port: " << IP_Clients[ind]->Remote_server_port << "\n";
        IP_Clients[ ind ]->InitClient();
        // printf( "Client name %s\n" , IP_Clients[ ind ]->id );
    }
    Input_Message_String
            = new char[ max_network_message_length ];
    Input_Message_Local_Commande_String
            = new char[ max_network_message_length ];
    Output_Message_String
            = new char[ max_network_message_length ];
    // the OSC string is made 2 times longer because
    // it contains both pattern and argts
    Output_Message_OSC
            = new char[ 2 * max_network_message_length ];
    Output_Message_Pattern
            = new char[ max_network_message_length ];
    Output_Message_ArgList
            = new char[ max_network_message_length ];
    // printf( "End of scene initialization\n" );
}

//////////////////////////////////////////////////////////////////////////////
/// \brief pg_store_message_udp
/// \param pattern
/// \param message
/// \param targetHostid
/// Stores a message in the message queue

void UDPInOut::pg_store_message_udp( QString *pattern , QString *message ,
                          QString *targetHostid ) {
    pg_IPClient* targetHost = NULL;
    // does not process the messages of the empty controllers
    if(message->at( 0 ) == 'X') {
        return;
    }
    for( int ind = 0 ; ind < nb_IP_Clients ; ind++ ) {
        if( targetHostid->toStdString().compare( IP_Clients[ ind ]->id ) == 0 ) {
            targetHost = IP_Clients[ ind ];
            break;
        }
    }
    if( !targetHost ) {
        std::cout << "UDP client unknown " << targetHostid->toStdString() << std::endl;
        return;
    }
    // printf("send_message_udp %s %s %d %d\n", message, pattern, nb_IP_Clients, targetHost);
    targetHost->storeIP_output_message( message ,
                                        pattern );
}

void UDPInOut::pg_store_message_udp( QString *pattern , QString *message ) {
    // does not process the messages of the empty controllers
    if(message->at( 0 ) == 'X') {
        return;
    }
    // printf("send_message_udp %s %s %d %d\n", message, pattern, nb_IP_Clients, targetHost);
    IP_Clients[ 0 ]->storeIP_output_message( message ,
                                             pattern );
}

void UDPInOut::pg_store_message_udp( QString *message ) {
    // does not process the messages of the empty controllers
    if(message->at( 0 ) == 'X') {
        return;
    }
    // printf("send_message_udp %s %s %d %d\n", message, pattern, nb_IP_Clients, targetHost);
    IP_Clients[ 0 ]->storeIP_output_message( message ,
                                             NULL );
}

//////////////////////////////////////////////////////////////////////////////
/// \brief UDPInOut::receiveAndSendIPMessages
///
void UDPInOut::receiveAndSendIPMessages(void) {
    for (int ind = 0; ind < nb_IP_Servers; ind++) {
        if(IP_Servers && IP_Servers[ind]) {
            IP_Servers[ind]->receiveIPMessages();
        }
    }
    for (int ind = 0; ind < nb_IP_Clients; ind++) {
        if(IP_Clients && IP_Clients[ind]) {
            IP_Clients[ind]->sendIPmessages();
        }
    }
}

void UDPInOut::ReleaseUDP(void) {
    // printf("releaseUDP\n");
    if (IP_Servers) {
        for (int ind = 0; ind < nb_IP_Servers; ind++) {
            delete IP_Servers[ind];
            IP_Servers[ind] = NULL;
        }

        delete [] IP_Servers;
        IP_Servers = NULL;
    }

    if (IP_Clients) {
        for (int ind = 0; ind < nb_IP_Clients; ind++) {
            delete IP_Clients[ind];
            IP_Clients[ind] = NULL;
        }

        delete [] IP_Clients;
        IP_Clients = NULL;
    }
}
