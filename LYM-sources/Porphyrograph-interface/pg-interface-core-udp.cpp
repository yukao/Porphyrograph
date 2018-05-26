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

#include "pg-interface-core-window.h"
#include "pg-interface-core-udp.h"

typedef  char *     Pchar;

// Input/Output network message string
char *Input_Message_String = NULL;
char *Input_Message_Local_Commande_String = NULL;
char *Output_Message_String = NULL;
char *Output_Message_OSC = NULL;
char *Output_Message_Pattern = NULL;
char *Output_Message_ArgList = NULL;

// hash table for UDP in messages lookup
QMultiHash<QString, int> hashSliderUDPIn;
QMultiHash<QString, int> hashLEDBarUDPIn;
QMultiHash<QString, int> hashGridUDPIn;
// hash table for highlight message lookup
QMultiHash<QString, int> hashButtonHighlightIn;
QMultiHash<QString, int> hashSliderHighlightIn;
QMultiHash<QString, int> hashLEDBarHighlightIn;
QMultiHash<QString, int> hashLabelHighlightIn;
QMultiHash<QString, int> hashGridHighlightIn;

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
        qDebug() << "Error: unknown remote host IP '" << QString::fromStdString(Remote_server_IP) << " (" << err << ")'!"; // << std::endl;
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
            qDebug() << "Error: unknown remote host IP '" << QString::fromStdString(Remote_server_IP) << " (" << err << ")'!"; // << std::endl;
        }
        else {
            /* get server IP address (no check if input is IP address or DNS name */
            // deprecated Remote_server_host = gethostbyname(Remote_server_IP.c_str());
            memset(&remoteServAddr, 0, sizeof(remoteServAddr));
            remoteServAddr.sin_family = AF_INET;
            inet_pton(AF_INET, hname, &(remoteServAddr.sin_addr));
            remoteServAddr.sin_port = htons(Remote_server_port);

            qDebug() << "Network client: sending data to " << QString::fromStdString(Remote_server_IP)
                     << " (" << hname << ") on port " << Remote_server_port;

            /* socket creation */
            //memcpy((char *) &remoteServAddr.sin_addr.s_addr,
            //Remote_server_host->h_addr_list[0], Remote_server_host->h_length);

            SocketToRemoteServer = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
            if (SocketToRemoteServer < 0) {
                qDebug() << "Error: cannot open socket to remote server"; // << std::endl;
            }
            err = connect(SocketToRemoteServer, result->ai_addr, result->ai_addrlen);
            if (err == -1) {
                qDebug() << "Error: cannot open socket to remote server"; // << std::endl;
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
        qDebug() << "no connection to remote server"; // << std::endl;
        return;
    }

    while( current_depth_output_stack >= 0 ) {
        // qDebug() << "sendUDPmessage " << current_depth_output_stack;

        // checks whether last message was received
        // or it is the first message
        // or the elapsed time since the last message is greater than the
        bool message_received = true;

        if( message_received
                || current_IP_message_number == first_IP_message_number
                || last_IP_message_time.msecsTo(QTime::currentTime()) > maximal_IP_message_delay ) {


            // qDebug() << "send message [" << output_message_stack[ 0 ] << "]"; // << std::endl;

            // copies the earliest message
            int     indLocalCommandLine = 0;

            if( send_format == Plain ) {
                strncpy( Output_Message_String , output_message_stack[ 0 ] ,
                        max_network_message_length - 1 );
                //                qDebug() << "send message [" << QString(Output_Message_String) << "]"; // << std::endl;
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
                        // qDebug() << "Pattern " << output_pattern_stack[ 0 ];
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
                            // qDebug() << "next_char_pattern " << next_char_pattern;
                            // SpaceCommentsInclude(NULL,&p_c,messageOut,&inChSource);
                            while( _SpaceChar(p_c) ) {
                                p_c = messageOut[ inChSource++ ];
                            }
                            // qDebug() << "p_c " << p_c ;
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

                                // qDebug() << "argt #" nbArg << " float " << f << " char " << p_c << " ind " indArgList;
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

                                // qDebug() << "argt #" nbArg<< " float " << f << " char " << p_c << " ind " indArgList;
                            }
                            // string: anything else
                            else {
                                // qDebug() <<  "ReadStringWithFrontiers p_c " << p_c;
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
                                        qDebug() << "Error: max string size exceeded ReadStringWithFrontiers (max " << StringLength << " chars)!"; // << std::endl;
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
                                        qDebug() << "Error: max string size exceeded ReadStringWithFrontiers (max " << StringLength << ")!"; // << std::endl;
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

                                // qDebug() << "argt #" << nbArg<< " str " << Output_Message_ArgList + indArgList << " char " << p_c << " ind " indArgList;

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
                            qDebug() << "Error: cannot udp client: OSC message incompletely sent [" << Output_Message_OSC << "]!"; // << std::endl;
                        }

                        // trace
                        // 	    qDebug() << "udp client: ";
                        // 	    	  for( int ind = 0 ; ind < indLocalCommandLine ; ind++ ) {
                        // 	    	    qDebug() << Output_Message_OSC[ind] , "(" << (int)localCommandLine[ind] <<")";
                        // 	    	  }
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
                // qDebug() << "message size " << strlen(Output_Message_OSC + 20 )  + 1;
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
                    qDebug() << "Error: udp client: data incompletely sent [" << Output_Message_String << "] (" << rc << "/" << localCommandLineLength << ")!"; // << std::endl;
                }
                if( rc > 0 && IP_message_trace ) {
                    qDebug() << "Network client: send string [" << Output_Message_String << "] (" << (int)strlen(Output_Message_String) << " bytes)";
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
                    qDebug() << "Error: udp client: OSC data incompletely sent [" << Output_Message_OSC << "]"; // << std::endl;
                }
                if( rc > 0 && IP_message_trace ) {
                    qDebug() << "udp client: send OSC bundle " << (int)*(Output_Message_OSC + 20 - 1)
                             << " [" << Output_Message_OSC + 20 << "] (" << (int)(20 + strlen(Output_Message_OSC + 20) + 1) << " bytes)";
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
                    qDebug() << "Error: udp client: data incompletely sent [" << Output_Message_OSC << "]!"; // << std::endl;
                }
                if( rc > 0 && IP_message_trace ) {
                    qDebug() << "udp client: send OSC message (" << localCommandLineLength << " bytes)";
                    for( int ind = 0 ; ind < localCommandLineLength ; ind++ ) {
                        //qDebug() << Output_Message_OSC[ind] << "(" << (int)Output_Message_OSC[ind] << ")";
                        qDebug() << Output_Message_OSC[ind];
                        qDebug() << "udp client: send OSC message (" << localCommandLineLength << " bytes)";
                    }
                }
            }

            // records the emission time
            last_IP_message_time =  QTime::currentTime();
        }
        else {
            if( IP_message_trace ) {
                qDebug() << "udp client: delay " << last_IP_message_time.secsTo(QTime::currentTime())
                         << " / " << maximal_IP_message_delay;
            }
            return;
        }
    }
}

void pg_IPClient::storeIP_output_message( QString *commandLine ,
                                          QString *pattern ) {

    // qDebug() << "commandLine " << commandLine << " " << current_depth_output_stack;
    if( SocketToRemoteServer < 0 ) {
        return;
    }

    // qDebug() << "commandLine " << commandLine << " " << current_depth_output_stack;
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
        qDebug() << "Error: UDP output message stack overflow " << current_depth_output_stack << " " << depth_output_stack << " (" << commandLine << ")!"; // << std::endl;
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
        qDebug() << "Error: udp server cannot open socket to local server!"; // << std::endl;
    }
    else {
        // Read the socket's flags
        unsigned long onoff=1;

        if (ioctlsocket(SocketToLocalServer, FIONBIO, &onoff) != 0){
            qDebug() << "Error: udp server cannot set flag to non-blocking: " << strerror(errno) << "!"; // << std::endl;
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
        qDebug() << "Error: cannot bind local port number " << Local_server_port << "!"; // << std::endl;
        return;
    }
    // qDebug() << "Network server: bound local port number " << Local_server_port;

    qDebug() << "Network server: listening on port "<< Local_server_port;

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
    // qDebug() << "processReceivedOSC " << message << " " << n; // << std::endl;

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
            // qDebug() << "param #" << nbParam + 1 << " [" << message[indChar] << "]";
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
                // qDebug() << "i argument #" << indParam + 1 << " [" << argument << "] at " << indChar;
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
                // qDebug() << "f argument #" << indParam + 1 << " [" << argument << "] at " << indChar << " " << sizeof( float );
                strcat( localCommandLine , argument );
                indChar += 4;
            }
            else if( paramTypesId[ indParam ] == 's' ) {
                // qDebug() << "s argument #" << indParam + 1 << " [" << message + indChar << "] at " << indChar;
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
                qDebug() << "udp server: receive plain message [" <<
                            Input_Message_String + offset_IP_message_received << "]";
            }
            storeIP_input_messages_and_removes_duplicates(Input_Message_String);
        }

        ///////////////////////////////////////////////////////
        // OSC format: bundle
        else if (receive_format == OSC && *Input_Message_String == '#') {
            int length;

            if (n <= 20) {
                qDebug() << "Error: incorrect OSC format of UDP message (incorrect initial length " << n << ")! Check input format in configuration file!"; // << std::endl;
                qDebug() << "udp server: [" << QString::fromStdString(Input_Message_String) << "]";
                for (int ind = 0; ind < n; ind++) {
                    qDebug() << Input_Message_String[ind] << " ("
                             << (int)Input_Message_String[ind] << ")";
                }
                return;
            }

            length = (int) *((unsigned char *)(Input_Message_String + 20 - 1));
            //qDebug() << "\nudp server: message length total %d used %d\n", n , length);
            // //print received message
            //       qDebug() << "udp server: ";
            //       for( int ind = 0 ; ind < n ; ind++ ) {
            // 	          qDebug() << ind << " " <<  Input_Message_String[ind] << " " <<  (int)Input_Message_String[ind];
            //       }


            if (length > n) {
                qDebug() << "Error: incorrect OSC format of UDP message (incorrect message length)! Check input format in configuration file!"; // << std::endl;
                qDebug() << "udp server: [" << Input_Message_String << "]";
                for (int ind = 0; ind < n; ind++) {
                    qDebug() << ind << " " << Input_Message_String[ind] << " " << (int)Input_Message_String[ind];
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
                    qDebug() << "store message [" << Input_Message_Local_Commande_String << "]";
                }
                storeIP_input_messages_and_removes_duplicates(Input_Message_Local_Commande_String);
            }
            else {
                qDebug() << "Error: UDP input message expected in OSC format (" << Input_Message_String + 20 + offset_IP_message_received << ")!"; // << std::endl;
            }
        }

        ///////////////////////////////////////////////////////
        // OSC format: message
        else if (receive_format == OSC && *Input_Message_String == '/') {
            // OSC to string conversion for uniform processing of messages
            processReceivedOSC(Input_Message_Local_Commande_String,
                               Input_Message_String, n);

            if (IP_message_trace) {
                qDebug() << "store message [" << Input_Message_Local_Commande_String << "]";
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

    //    qDebug() << "new message " << messString << " current_depth_input_stack "
    //              <<  current_depth_input_stack; // << std::endl;

    // duplicates removal for OSC message formats
    // checks whether it has a duplicate and replaces it if there is one
    if (*messString == '/' && *(messString + 1) != '_' && OSC_duplicate_removal) {
        for (int ind_mess = 0; ind_mess < current_depth_input_stack; ind_mess++) {
            // same tag
            if (input_message_length_stack[ind_mess] == tagLength
                    && strncmp(input_message_stack[ind_mess],
                               messString, tagLength) == 0) {
                //qDebug() <<  "duplicate removed [" << input_message_stack[ ind_mess ] << "] for [" << messString << "]";
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
        qDebug() << "Error: UDP input message stack overflow " << current_depth_input_stack << " " << depth_input_stack << "!"; // << std::endl;
        // for( int ind_mess = 0 ; ind_mess < depth_input_stack - 1  ; ind_mess++ ) {
        //   qDebug() <<  "message [" <<  input_message_stack[ ind_mess ] << "]";
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
            qDebug() << "process message " << messString; // << std::endl;
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
            // qDebug() << "message command [" << << "] length [" << << "]\n" , OSCTag , tagLength );

            // qDebug() << "OSC parameters %s\n" , pch );
            int indOSCParam = 0;
            // qDebug() << "p_c " << pch<< " (" << pch<< ") mess (" << messString<< ") pch " << p_c<< " indChar;
            while( p_c == ' ' ) {
                messString += 1;
                indChar += 1;
                p_c = *messString;
            }
            // qDebug() << "p_c " << pch<< " (" << pch<< ") mess (" << messString<< ") pch " << p_c<< " indChar;

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
                // qDebug() << "message command [" << OSC_arguments[indOSCParam]<< "] length [" << tagLength<< "]";

                // qDebug() << "p_c " << pch<< " (" << pch<< ") mess (" << messString<< ") pch " << p_c<< " indChar;
                while( p_c == ' ' ) {
                    messString += 1;
                    indChar += 1;
                    p_c = *messString;
                }
                // qDebug() << "p_c " << pch<< " (" << pch<< ") mess (" << messString<< ") pch " << p_c<< " indChar;

                indOSCParam++;
            }

            if( p_c ) {
                qDebug() << "Error: OSC maximal parameter count too low (" << MAX_OSC_ARGUMENTS << ")!"; // << std::endl;
            }

            float args[MAX_OSC_ARGUMENTS];
            for( int ind = 0 ; ind < MAX_OSC_ARGUMENTS ; ind++ ) {
                args[ind] = (float)atof( OSC_arguments[ind] );
            }

            //            qDebug() << "Alias script " << OSCTag << " argt " << OSC_arguments[0] << " / " << OSC_arguments[1];
            pg_window->propagateUDPEventsToControllers(OSCTag , OSC_arguments[0] ,  OSC_arguments[1] , args , indOSCParam);
            //            qDebug() << "Alias script executed";
        }
        else {
            qDebug() << "Error: incorrect external message syntax " << messString << "!"; // << std::endl;
        }
    }
}  

void MainWindow::propagateUDPEventsToControllers(char *OSCTag,
                                                 char *firstArgt, char *secondArgt,
                                                 float args[], int nbargts) {
    QString OSCTagString( OSCTag );
    OSCTagString = "/" + OSCTagString;
    QString firstArgtString( firstArgt );
    QString secondArgtString( secondArgt );

    // << " #2: " << firstArgtString.toStdString(); // << std::endl;
    //    qDebug() << "list size: " << (nbargts + 1) << " #1: ["
    //              << OSCTagString.toStdString() << "]"; // << std::endl;

    //////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////
    // MESSAGES FOR THE COMMON PARTS OF THE INTERAFACE: COLOR, COUNTDOWN, SCENARIO
    if(OSCTagString == "/pulsed_pen_color_can" && (nbargts + 1) >= 4) {
        for(int indDisplay = _Beat_display ; indDisplay <= _Paths_Pulse_display ; indDisplay++) {
            tabDisplays[static_cast<DisplayType>(indDisplay)]
                    ->colorPalette->setMixedColor(
                        args[0], args[1], args[2], 0);
        }
#if defined(TVW) || defined(demo)
        for(int indDisplay = _MinimalIF_display ; indDisplay <= _MinimalIF_Pulse_display ; indDisplay++) {
            tabDisplays[static_cast<DisplayType>(indDisplay)]
                    ->colorPalette->setMixedColor(
                        args[0], args[1], args[2], 0);
        }
#endif
    }
    else if(OSCTagString == "/pen_grey_can") {
        for(int indDisplay = _Beat_display ; indDisplay <= _Paths_Pulse_display ; indDisplay++) {
            tabDisplays[static_cast<DisplayType>(indDisplay)]->colorPalette->setGray(
                        args[0], 0);
        }
#if defined(TVW) || defined(demo)
        for(int indDisplay = _MinimalIF_display ; indDisplay <= _MinimalIF_Pulse_display ; indDisplay++) {
            tabDisplays[static_cast<DisplayType>(indDisplay)]->colorPalette->setGray(
                        args[0], 0);
        }
#endif
    }
    else if(OSCTagString == "/pen_colorPalette_can" && (nbargts + 1) >= 10) {
        for(int indDisplay = _Beat_display ; indDisplay <= _Paths_Pulse_display ; indDisplay++) {
            tabDisplays[static_cast<DisplayType>(indDisplay)]->colorPalette->setPalette(
                        args[0], args[1], args[2],
                    args[3], args[4], args[5],
                    args[6], args[7], args[8], 0);
        }
#if defined(TVW) || defined(demo)
        for(int indDisplay = _MinimalIF_display ; indDisplay <= _MinimalIF_Pulse_display ; indDisplay++) {
            tabDisplays[static_cast<DisplayType>(indDisplay)]->colorPalette->setPalette(
                        args[0], args[1], args[2],
                    args[3], args[4], args[5],
                    args[6], args[7], args[8], 0);
        }
#endif
    }
    else if(OSCTagString == "/pulsed_repop_color_can" && (nbargts + 1) >= 4) {
        for(int indDisplay = _Beat_display ; indDisplay <= _Paths_Pulse_display ; indDisplay++) {
            tabDisplays[static_cast<DisplayType>(indDisplay)]->colorPalette->setMixedColor(
                        args[0], args[1], args[2], 1);
        }
#if defined(TVW) || defined(demo)
        for(int indDisplay = _MinimalIF_display ; indDisplay <= _MinimalIF_Pulse_display ; indDisplay++) {
            tabDisplays[static_cast<DisplayType>(indDisplay)]->colorPalette->setMixedColor(
                        args[0], args[1], args[2], 1);
        }
#endif
    }
    else if(OSCTagString == "/repop_grey_can") {
        for(int indDisplay = _Beat_display ; indDisplay <= _Paths_Pulse_display ; indDisplay++) {
            tabDisplays[static_cast<DisplayType>(indDisplay)]->colorPalette->setGray(
                        args[0], 1);
        }
#if defined(TVW) || defined(demo)
        for(int indDisplay = _MinimalIF_display ; indDisplay <= _MinimalIF_Pulse_display ; indDisplay++) {
            tabDisplays[static_cast<DisplayType>(indDisplay)]->colorPalette->setGray(
                        args[0], 1);
        }
#endif
    }
    else if(OSCTagString == "/repop_colorPalette_can" && (nbargts + 1) >= 10) {
        for(int indDisplay = _Beat_display ; indDisplay <= _Paths_Pulse_display ; indDisplay++) {
            tabDisplays[static_cast<DisplayType>(indDisplay)]->colorPalette->setPalette(
                        args[0], args[1], args[2],
                    args[3], args[4], args[5],
                    args[6], args[7], args[8], 1);
        }
#if defined(TVW) || defined(demo)
        for(int indDisplay = _MinimalIF_display ; indDisplay <= _MinimalIF_Pulse_display ; indDisplay++) {
            tabDisplays[static_cast<DisplayType>(indDisplay)]->colorPalette->setPalette(
                        args[0], args[1], args[2],
                    args[3], args[4], args[5],
                    args[6], args[7], args[8], 1);
        }
#endif
    }
    else if(OSCTagString == "/time_can" && (nbargts + 1) >= 2) {
        for(int indDisplay = _Beat_display ; indDisplay <= _Paths_Pulse_display ; indDisplay++) {
            tabDisplays[static_cast<DisplayType>(indDisplay)]->timeSceneBeatDisplay->setTime(firstArgtString);
        }
#if defined(TVW) || defined(demo)
        for(int indDisplay = _MinimalIF_display ; indDisplay <= _MinimalIF_Pulse_display ; indDisplay++) {
            tabDisplays[static_cast<DisplayType>(indDisplay)]->timeSceneBeatDisplay->setTime(firstArgtString);
        }
#endif
    }
    else if(OSCTagString == "/time_color_can" && (nbargts + 1) >= 2) {
        for(int indDisplay = _Beat_display ; indDisplay <= _Paths_Pulse_display ; indDisplay++) {
            tabDisplays[static_cast<DisplayType>(indDisplay)]->timeSceneBeatDisplay->setColor(int(args[0]));
        }
#if defined(TVW) || defined(demo)
        for(int indDisplay = _MinimalIF_display ; indDisplay <= _MinimalIF_Pulse_display ; indDisplay++) {
            tabDisplays[static_cast<DisplayType>(indDisplay)]->timeSceneBeatDisplay->setColor(int(args[0]));
        }
#endif
    }
    else if(OSCTagString == "/setup_can" && (nbargts + 1) >= 2) {
        for(int indDisplay = _Beat_display ; indDisplay <= _Paths_Pulse_display ; indDisplay++) {
            tabDisplays[static_cast<DisplayType>(indDisplay)]->timeSceneBeatDisplay->setSceneName(firstArgtString);
        }
#if defined(TVW) || defined(demo)
        for(int indDisplay = _MinimalIF_display ; indDisplay <= _MinimalIF_Pulse_display ; indDisplay++) {
            tabDisplays[static_cast<DisplayType>(indDisplay)]->timeSceneBeatDisplay->setSceneName(firstArgtString);
        }
#endif
    }
    else if(OSCTagString == "/message_can" && (nbargts + 1) >= 2) {
        for(int indDisplay = _Beat_display ; indDisplay <= _Paths_Pulse_display ; indDisplay++) {
            tabDisplays[static_cast<DisplayType>(indDisplay)]->timeSceneBeatDisplay->setMessageText(firstArgtString);
        }
#if defined(TVW) || defined(demo)
        for(int indDisplay = _MinimalIF_display ; indDisplay <= _MinimalIF_Pulse_display ; indDisplay++) {
            tabDisplays[static_cast<DisplayType>(indDisplay)]->timeSceneBeatDisplay->setMessageText(firstArgtString);
        }
#endif
    }
    else if(OSCTagString == "/loop_beat_can" && (nbargts + 1) >= 2) {
        for(int indDisplay = _Beat_display ; indDisplay <= _Paths_Pulse_display ; indDisplay++) {
            tabDisplays[static_cast<DisplayType>(indDisplay)]->timeSceneBeatDisplay->setBeatNo(firstArgtString);
        }
#if defined(TVW) || defined(demo)
        for(int indDisplay = _MinimalIF_display ; indDisplay <= _MinimalIF_Pulse_display ; indDisplay++) {
            tabDisplays[static_cast<DisplayType>(indDisplay)]->timeSceneBeatDisplay->setBeatNo(firstArgtString);
        }
#endif
    }
    else if(OSCTagString == "/reset_LEDs") {
        for(int indDisplay = _Beat_display ; indDisplay <= _Paths_Pulse_display ; indDisplay++) {
            // qDebug() << "LEDs reinitialization"; // << std::endl;
            pg_window->reinitializeMidiLighting(static_cast<DisplayType>(indDisplay));
        }
#if defined(TVW) || defined(demo)
        for(int indDisplay = _MinimalIF_display ; indDisplay <= _MinimalIF_Pulse_display ; indDisplay++) {
            pg_window->reinitializeMidiLighting(static_cast<DisplayType>(indDisplay));
        }
#endif
    }
    else if(OSCTagString == "/switchOff_LEDs") {
        for(int indDisplay = _Beat_display ; indDisplay <= _Paths_Pulse_display ; indDisplay++) {
            // qDebug() << "LEDs off"; // << std::endl;
            pg_window->swithOffMidiLighting(static_cast<DisplayType>(indDisplay));
        }
#if defined(TVW) || defined(demo)
        for(int indDisplay = _MinimalIF_display ; indDisplay <= _MinimalIF_Pulse_display ; indDisplay++) {
            pg_window->swithOffMidiLighting(static_cast<DisplayType>(indDisplay));
        }
#endif
    }
    else if(OSCTagString == "/QT_connected") {
        QString message = QString("/QT_connected");
        pg_window->udpInOut->pg_store_message_udp( &message );
    }

    //////////////////////////////////////////////////////////////
    // INTERFACE SELECTION
    else if(OSCTagString == "/minimal_display") {
#if defined(TVW) || defined(demo)
        if(pg_window->getCurrentTabWindow() != _MinimalIF_display ) {
            pg_window->setCurrentTabWindow(_MinimalIF_display);
        }
#endif
    }
    else if(OSCTagString == "/paths_display") {
        if(pg_window->getCurrentTabWindow() != _Paths_display ) {
            pg_window->setCurrentTabWindow(_Paths_display);
        }
    }

    //////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////
    // MESSAGES FOR SLIDERS AND BUTTONS
    //////////////////////////////////////////////////////////////
    //    qDebug() << "Message for sliders and buttons size: " << (nbargts + 1) << " #1: ["
    //              << OSCTagString << "]"; // << std::endl;
    // messages for the sliders
    if(hashSliderUDPIn.contains(OSCTagString)) {
        QList<int> values = hashSliderUDPIn.values(OSCTagString);
        if (values.size() % 3 == 0 && values.size() / 3 >= 1) {
            for(int ind = 0 ; ind < values.size() ; ind += 3 ) {
                int indSliderRow = values.at(ind+0);
                int indCol = values.at(ind+1);
                DisplayType displayType = static_cast<DisplayType>(values.at(ind+2));
                if(tabControllers[displayType]->verticalSliders[indSliderRow][indCol]) {
                    // scenario-based value changes
                    // statusLabel->setText(tr("Received message: \"%1\"").arg(message));
                    // qDebug() << "received UDP message " << OSCTagString.toStdString() << " " << firstArgtString.toStdString(); // << std::endl;
                    tabControllers[displayType]->verticalSliders[indSliderRow][indCol]->setDoubleValueFromUDP(args[0]);
                }
            }
        }
    }
    if(hashSliderHighlightIn.contains(OSCTagString)) {
        QList<int> values = hashSliderHighlightIn.values(OSCTagString);
        if (values.size() % 3 == 0 && values.size() / 3 >= 1) {
            for(int ind = 0 ; ind < values.size() ; ind += 3 ) {
                int indSliderRow = values.at(ind+0);
                int indCol = values.at(ind+1);
                DisplayType displayType = static_cast<DisplayType>(values.at(ind+2));
                if(tabControllers[displayType]->verticalSliders[indSliderRow][indCol]) {
                    // statusLabel->setText(tr("Received message: [%1] [%2]").arg(OSCTagString).arg(firstArgtString));
                    tabControllers[displayType]->verticalSliders[indSliderRow][indCol]->setHighlight(args[0]);
                }
            }
        }
    }
    //////////////////////////////////////////////////////////////
    // messages for the buttons
    if(hashButtonHighlightIn.contains(OSCTagString)) {
        QList<int> values = hashButtonHighlightIn.values(OSCTagString);
        if (values.size() % 3 == 0 && values.size() / 3 >= 1) {
            for(int ind = 0 ; ind < values.size() ; ind += 3 ) {
                int indMainButtonRow = values.at(ind+0);
                int indMainButtonCol = values.at(ind+1);
                DisplayType displayType = static_cast<DisplayType>(values.at(ind+2));
                if(tabControllers[displayType]->mainButtons[indMainButtonRow][indMainButtonCol]) {
                    // statusLabel->setText(tr("Received message: [%1] [%2]").arg(OSCTagString).arg(firstArgtString));
                    // qDebug() << "Alias script set highlight displ " << displayType << " row " << indMainButtonRow << " col " << indMainButtonCol << " val " << args[0];
                    tabControllers[displayType]->mainButtons[indMainButtonRow][indMainButtonCol]->setHighlight(args[0]);
                    // qDebug() << "Alias script highlight set";
                }
            }
        }
    }
    //////////////////////////////////////////////////////////////
    // messages for the LED Bars
    if(hashLEDBarUDPIn.contains(OSCTagString)) {
        QList<int> values = hashLEDBarUDPIn.values(OSCTagString);
        if (values.size() % 3 == 0 && values.size() / 3 >= 1) {
            for(int ind = 0 ; ind < values.size() ; ind += 3 ) {
                int indRow = values.at(ind+0);
                int indCol = values.at(ind+1);
                DisplayType displayType = static_cast<DisplayType>(values.at(ind+2));
                if(tabDisplays[displayType]->graphicalLEDBarRows[indRow]) {
                    // statusLabel->setText(tr("Received message: \"%1\"").arg(message));
                    tabDisplays[displayType]->graphicalLEDBarRows[indRow]
                            ->setValue(indCol,args[0]);
                }
            }
        }
    }
    if(hashLEDBarHighlightIn.contains(OSCTagString)) {
        QList<int> values = hashLEDBarHighlightIn.values(OSCTagString);
        if (values.size() % 3 == 0 && values.size() / 3 >= 1) {
            for(int ind = 0 ; ind < values.size() ; ind += 3 ) {
                int indRow = values.at(ind+0);
                int indCol = values.at(ind+1);
                DisplayType displayType = static_cast<DisplayType>(values.at(ind+2));
                if(tabDisplays[displayType]->graphicalLEDBarRows[indRow]) {
                    LEDBar *curLEDBar = tabDisplays[displayType]->graphicalLEDBarRows[indRow]->getLEDBar(indCol);
                    // statusLabel->setText(tr("Received message: \"%1\"").arg(message));
                    tabDisplays[displayType]->graphicalLEDBarRows[indRow]
                            ->setHighlight(indCol, args[0] == curLEDBar->valueHighlightIn);
                }
            }
        }
    }
    //////////////////////////////////////////////////////////////
    // messages for the labels
    if(hashLabelHighlightIn.contains(OSCTagString)) {
        QList<int> values = hashLabelHighlightIn.values(OSCTagString);
        if (values.size() % 3 == 0 && values.size() / 3 >= 1) {
            for(int ind = 0 ; ind < values.size() ; ind += 3 ) {
                int indMainLabelRow = values.at(ind+0);
                int indMainLabelCol = values.at(ind+1);
                DisplayType displayType = static_cast<DisplayType>(values.at(ind+2));
                if(tabDisplays[displayType]->graphicalLabelRows[indMainLabelRow]) {
                    SingleLabel *curLabel = tabDisplays[displayType]->graphicalLabelRows[indMainLabelRow]->getSingleLabel(indMainLabelCol);
                    // statusLabel->setText(tr("Received message: \"%1\"").arg(message));
                    curLabel->setHighlight(args[0]);
                }
            }
        }
    }
    //////////////////////////////////////////////////////////////
    // messages for the Grid
    if(hashGridUDPIn.contains(OSCTagString)) {
        QList<int> values = hashGridUDPIn.values(OSCTagString);
        if (values.size() == 3) {
            int indGridRow = values.at(0);
            int indCol = values.at(1);
            if(tabDisplays[_Beat_display]->graphsFlashGrid) {
                // statusLabel->setText(tr("Received message: \"%1\"").arg(message));
                tabDisplays[_Beat_display]->graphsFlashGrid
                        ->setValue(indGridRow + 1,indCol + 1,int(args[0]));
            }
        }
    }
    if(hashGridHighlightIn.contains(OSCTagString)) {
        QList<int> values = hashGridHighlightIn.values(OSCTagString);
        if (values.size() == 3) {
            int indGridRow = values.at(0);
            int indCol = values.at(1);
            if(tabDisplays[_Beat_display]->graphsFlashGrid) {
                // statusLabel->setText(tr("Received message: [%1] [%2]").arg(OSCTagString).arg(firstArgtString));
                tabDisplays[_Beat_display]->graphsFlashGrid
                        ->setHighlight(indGridRow + 1,indCol + 1,args[0]);
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////////
    // NON STANDARD MESSAGES EG TEXT FOR LABELS
    for(unsigned int indDisplay = 0; indDisplay < _NumberOfDisplays; indDisplay++) {
        // qDebug() << "Display "  << indDisplay; // << std::endl;
        // processes separately pulse and non-pulse messages
        int displayIndex = 0;
        switch(indDisplay) {
        //////////////////////////////////////////////////////////////
        // GRAPHS DISPLAY
        case _Beat_display:
            displayIndex = _Beat_display;
            //////////////////////////////////////////////////////////////
            // text of the buttons and labels
            if(OSCTagString == "/pixel_mode_can" && (nbargts + 1) >= 2
                    && tabControllers[displayIndex]->mainButtons[_pixelModeButtonsBeatData].size() >= 1
                    && tabControllers[displayIndex]->mainButtons[_pixelModeButtonsBeatData][0] ) {
                int value = int(args[0]);
                if(value == 0) {
                    tabControllers[displayIndex]->mainButtons[_pixelModeButtonsBeatData][0]->setText("X");
                    tabControllers[displayIndex]->mainButtons[_pixelModeButtonsBeatData][0]->getAssociatedSingleLabel()->setText("noPix");
                }
                else if(value == 1) {
                    tabControllers[displayIndex]->mainButtons[_pixelModeButtonsBeatData][0]->setText("pixel");
                    tabControllers[displayIndex]->mainButtons[_pixelModeButtonsBeatData][0]->getAssociatedSingleLabel()->setText("pi");
                }
                else if(value == 2) {
                    tabControllers[displayIndex]->mainButtons[_pixelModeButtonsBeatData][0]->setText("fireWk");
                    tabControllers[displayIndex]->mainButtons[_pixelModeButtonsBeatData][0]->getAssociatedSingleLabel()->setText("FW");
                }
            }
            else if(OSCTagString == "/pen_grey_can" && (nbargts + 1) >= 2
                    && tabControllers[displayIndex]->mainButtons[_penBWButtonsBeatData].size() >= 1
                    && tabControllers[displayIndex]->mainButtons[_penBWButtonsBeatData][0] ) {
                int value = int(args[0]);
                if(value == 0) {
                    tabControllers[displayIndex]->mainButtons[_penBWButtonsBeatData][0]->setText("Black");
                    tabControllers[displayIndex]->mainButtons[_penBWButtonsBeatData][0]->getAssociatedSingleLabel()->setText("B");
                }
                else {
                    tabControllers[displayIndex]->mainButtons[_penBWButtonsBeatData][0]->setText("White");
                    tabControllers[displayIndex]->mainButtons[_penBWButtonsBeatData][0]->getAssociatedSingleLabel()->setText("W");
                }
            }
            else if(OSCTagString == "/repop_grey_can" && (nbargts + 1) >= 2
                    && tabControllers[displayIndex]->mainButtons[_repopBWButtonsBeatData].size() >= 1
                    && tabControllers[displayIndex]->mainButtons[_repopBWButtonsBeatData][0]) {
                int value = int(args[0]);
                if(value == 0) {
                    tabControllers[displayIndex]->mainButtons[_repopBWButtonsBeatData][0]->setText("Black");
                    tabControllers[displayIndex]->mainButtons[_repopBWButtonsBeatData][0]->getAssociatedSingleLabel()->setText("B");
                }
                else {
                    tabControllers[displayIndex]->mainButtons[_repopBWButtonsBeatData][0]->setText("White");
                    tabControllers[displayIndex]->mainButtons[_repopBWButtonsBeatData][0]->getAssociatedSingleLabel()->setText("W");
                }
            }
            else if(OSCTagString.startsWith("/pen_colorPreset_name_can") && (nbargts + 1) >= 2) {
                int value = OSCTagString.right(1).toInt();
                if(value < (int)tabControllers[displayIndex]->mainButtons[_penPaletteButtonsBeatData].size()
                        && tabControllers[displayIndex]->mainButtons[_penPaletteButtonsBeatData][value]) {
                    tabControllers[displayIndex]->mainButtons[_penPaletteButtonsBeatData][value]->setText(firstArgtString);
                    tabControllers[displayIndex]->mainButtons[_penPaletteButtonsBeatData][value]->getAssociatedSingleLabel()->setText(firstArgtString);
                }
            }
            break;

            //////////////////////////////////////////////////////////////
            // Mixer DISPLAY
        case _Mixer_display:
            displayIndex = _Mixer_display;
            //////////////////////////////////////////////////////////////
            // text of the buttons and labels
            if(OSCTagString == "/currentPhotoTrack_can" && (nbargts + 1) >= 2
                    && tabControllers[displayIndex]->mainButtons[_TracksButtonsMixerData].size() >= 1
                    && tabControllers[displayIndex]->mainButtons[_TracksButtonsMixerData][0]) {
                int value = int(args[0]);
                if(value == 0) {
                    tabControllers[displayIndex]->mainButtons[_TracksButtonsMixerData][0]->setText("Photo BG");
                    tabControllers[displayIndex]->mainButtons[_TracksButtonsMixerData][0]->getAssociatedSingleLabel()->setText("Ph0");
                }
                else if(value == 1) {
                    tabControllers[displayIndex]->mainButtons[_TracksButtonsMixerData][0]->setText("Photo T1");
                    tabControllers[displayIndex]->mainButtons[_TracksButtonsMixerData][0]->getAssociatedSingleLabel()->setText("Ph1");
                }
                else if(value == 2) {
                    tabControllers[displayIndex]->mainButtons[_TracksButtonsMixerData][0]->setText("Photo T2");
                    tabControllers[displayIndex]->mainButtons[_TracksButtonsMixerData][0]->getAssociatedSingleLabel()->setText("Ph2");
                }
                else if(value == 3) {
                    tabControllers[displayIndex]->mainButtons[_TracksButtonsMixerData][0]->setText("Photo T3");
                    tabControllers[displayIndex]->mainButtons[_TracksButtonsMixerData][0]->getAssociatedSingleLabel()->setText("Ph3");
                }
            }
            else if(OSCTagString == "/currentVideoTrack_can" && (nbargts + 1) >= 2
                    && tabControllers[displayIndex]->mainButtons[_TracksButtonsMixerData].size() >= 2
                    && tabControllers[displayIndex]->mainButtons[_TracksButtonsMixerData][1]) {
                int value = int(args[0]);
                if(value == 0) {
                    tabControllers[displayIndex]->mainButtons[_TracksButtonsMixerData][1]->setText("Video BG");
                    tabControllers[displayIndex]->mainButtons[_TracksButtonsMixerData][1]->getAssociatedSingleLabel()->setText("Vi0");
                }
                else if(value == 1) {
                    tabControllers[displayIndex]->mainButtons[_TracksButtonsMixerData][1]->setText("Video T1");
                    tabControllers[displayIndex]->mainButtons[_TracksButtonsMixerData][1]->getAssociatedSingleLabel()->setText("Vi1");
                }
                else if(value == 2) {
                    tabControllers[displayIndex]->mainButtons[_TracksButtonsMixerData][1]->setText("Video T2");
                    tabControllers[displayIndex]->mainButtons[_TracksButtonsMixerData][1]->getAssociatedSingleLabel()->setText("Vi2");
                }
                else if(value == 3) {
                    tabControllers[displayIndex]->mainButtons[_TracksButtonsMixerData][1]->setText("Video T3");
                    tabControllers[displayIndex]->mainButtons[_TracksButtonsMixerData][1]->getAssociatedSingleLabel()->setText("Vi3");
                }
            }
            else if(OSCTagString == "/freeze_can" && (nbargts + 1) >= 2
                    && tabControllers[displayIndex]->mainButtons[_freezeBottomButtonsMixerData].size() >= 1
                    && tabControllers[displayIndex]->mainButtons[_freezeBottomButtonsMixerData][0]) {
                int value = int(args[0]);
                if(value == 0) {
                    tabControllers[displayIndex]->mainButtons[_freezeBottomButtonsMixerData][0]->setText("OFF");
                    tabControllers[displayIndex]->mainButtons[_freezeBottomButtonsMixerData][0]->getAssociatedSingleLabel()->setText("Off");
                }
                else {
                    tabControllers[displayIndex]->mainButtons[_freezeBottomButtonsMixerData][0]->setText("ON");
                    tabControllers[displayIndex]->mainButtons[_freezeBottomButtonsMixerData][0]->getAssociatedSingleLabel()->setText("On");
                }
            }
            else if(OSCTagString == "/invertAllLayers_can" && (nbargts + 1) >= 2
                    && tabControllers[displayIndex]->mainButtons[_freezeBottomButtonsMixerData].size() >= 2
                    && tabControllers[displayIndex]->mainButtons[_freezeBottomButtonsMixerData][1]) {
                int value = int(args[0]);
                if(value == 0) {
                    tabControllers[displayIndex]->mainButtons[_freezeBottomButtonsMixerData][1]->setText("OFF");
                    tabControllers[displayIndex]->mainButtons[_freezeBottomButtonsMixerData][1]->getAssociatedSingleLabel()->setText("Off");
                }
                else {
                    tabControllers[displayIndex]->mainButtons[_freezeBottomButtonsMixerData][1]->setText("ON");
                    tabControllers[displayIndex]->mainButtons[_freezeBottomButtonsMixerData][1]->getAssociatedSingleLabel()->setText("On");
                }
            }
            break;

            //////////////////////////////////////////////////////////////
            // FX DISPLAY
        case _FX_display:
            displayIndex = _FX_display;
            //////////////////////////////////////////////////////////////
            // text of the buttons and labels
            if(OSCTagString == "/soundtrack_shortName_can" && (nbargts + 1) >= 2
                    && tabControllers[displayIndex]->mainButtons[_soundSampleButtonsFXData].size() >= 1
                    && tabControllers[displayIndex]->mainButtons[_soundSampleButtonsFXData][0]) {
                tabControllers[displayIndex]->mainButtons[_soundSampleButtonsFXData][0]->setText(firstArgtString);
                tabControllers[displayIndex]->mainButtons[_soundSampleButtonsFXData][0]->getAssociatedSingleLabel()->setText(firstArgtString);
            }
            else if(OSCTagString == "/movie_shortName_can" && (nbargts + 1) >= 2
                    && tabControllers[displayIndex]->mainButtons[_mediaPlusButtonsFXData].size() >= 2
                    && tabControllers[displayIndex]->mainButtons[_mediaPlusButtonsFXData][1]) {
                tabControllers[displayIndex]->mainButtons[_mediaPlusButtonsFXData][1]->setText(firstArgtString);
                tabControllers[displayIndex]->mainButtons[_mediaPlusButtonsFXData][1]->getAssociatedSingleLabel()->setText(firstArgtString);
            }
            else if(OSCTagString == "/soundtrack_onOff_can" && (nbargts + 1) >= 2
                    && tabControllers[displayIndex]->mainButtons[_soundSampleButtonsFXData].size() >= 2
                    && tabControllers[displayIndex]->mainButtons[_soundSampleButtonsFXData][1]) {
                int value = int(args[0]);
                if(value == 0) {
                    tabControllers[displayIndex]->mainButtons[_soundSampleButtonsFXData][1]->setText("OFF");
                    tabControllers[displayIndex]->mainButtons[_soundSampleButtonsFXData][1]->getAssociatedSingleLabel()->setText("X");
                }
                else {
                    tabControllers[displayIndex]->mainButtons[_soundSampleButtonsFXData][1]->setText("ON");
                    tabControllers[displayIndex]->mainButtons[_soundSampleButtonsFXData][1]->getAssociatedSingleLabel()->setText("ON");
                }
            }
            else if(OSCTagString == "/adc_onOff_can" && (nbargts + 1) >= 2
                    && tabControllers[displayIndex]->mainButtons[_micOnOffButtonsFXData].size() >= 1
                    && tabControllers[displayIndex]->mainButtons[_micOnOffButtonsFXData][0]) {
                int value = int(args[0]);
                if(value == 0) {
                    tabControllers[displayIndex]->mainButtons[_micOnOffButtonsFXData][0]->setText("OFF");
                    tabControllers[displayIndex]->mainButtons[_micOnOffButtonsFXData][0]->getAssociatedSingleLabel()->setText("X");
                }
                else {
                    tabControllers[displayIndex]->mainButtons[_micOnOffButtonsFXData][0]->setText("ON");
                    tabControllers[displayIndex]->mainButtons[_micOnOffButtonsFXData][0]->getAssociatedSingleLabel()->setText("ON");
                }
            }
            else if(OSCTagString == "/soundtrack_shortName_can" && (nbargts + 1) >= 2
                    && tabControllers[displayIndex]->mainButtons[_soundSampleButtonsFXData].size() >= 1
                    && tabControllers[displayIndex]->mainButtons[_soundSampleButtonsFXData][0]) {
                tabControllers[displayIndex]->mainButtons[_soundSampleButtonsFXData][0]->setText(firstArgtString);
                tabControllers[displayIndex]->mainButtons[_soundSampleButtonsFXData][0]->getAssociatedSingleLabel()->setText(firstArgtString);
            }
            else if(OSCTagString == "/movie_shortName_can" && (nbargts + 1) >= 2
                    && tabControllers[displayIndex]->mainButtons[_mediaPlusButtonsFXData].size() >= 2
                    && tabControllers[displayIndex]->mainButtons[_mediaPlusButtonsFXData][1]) {
                tabControllers[displayIndex]->mainButtons[_mediaPlusButtonsFXData][1]->setText(firstArgtString);
                tabControllers[displayIndex]->mainButtons[_mediaPlusButtonsFXData][1]->getAssociatedSingleLabel()->setText(firstArgtString);
            }
            else if(OSCTagString == "/soundtrack_onOff_can" && (nbargts + 1) >= 2
                    && tabControllers[displayIndex]->mainButtons[_soundSampleButtonsFXData].size() >= 2
                    && tabControllers[displayIndex]->mainButtons[_soundSampleButtonsFXData][1]) {
                int value = int(args[0]);
                if(value == 0) {
                    tabControllers[displayIndex]->mainButtons[_soundSampleButtonsFXData][1]->setText("OFF");
                    tabControllers[displayIndex]->mainButtons[_soundSampleButtonsFXData][1]->getAssociatedSingleLabel()->setText("X");
                }
                else {
                    tabControllers[displayIndex]->mainButtons[_soundSampleButtonsFXData][1]->setText("ON");
                    tabControllers[displayIndex]->mainButtons[_soundSampleButtonsFXData][1]->getAssociatedSingleLabel()->setText("ON");
                }
            }
            else if(OSCTagString == "/CASubType_can" && (nbargts + 1) >= 2
                    && tabControllers[displayIndex]->mainButtons[_CATypeButtonFXsData].size() >= 1
                    && tabControllers[displayIndex]->mainButtons[_CATypeButtonFXsData][0]) {
                int value = int(args[0]);
                if(value == 0) {
                    tabControllers[displayIndex]->mainButtons[_CATypeButtonFXsData][0]->setText("OFF");
                    tabControllers[displayIndex]->mainButtons[_CATypeButtonFXsData][0]->getAssociatedSingleLabel()->setText("X");
                }
                else {
                    tabControllers[displayIndex]->mainButtons[_CATypeButtonFXsData][0]->setText("ON");
                    tabControllers[displayIndex]->mainButtons[_CATypeButtonFXsData][0]->getAssociatedSingleLabel()->setText("ON");
                }
            }
            else if(OSCTagString == "/CA1Type_can" && (nbargts + 1) >= 2
                    && tabControllers[displayIndex]->mainButtons[_CATypeButtonFXsData].size() >= 5
                    && tabControllers[displayIndex]->mainButtons[_CATypeButtonFXsData][4]) {
                int value = int(args[0]);
                if(value >= 0) {
                    tabControllers[displayIndex]->mainButtons[_CATypeButtonFXsData][4]->setText("+/"+QString('0'+value));
                    tabControllers[displayIndex]->mainButtons[_CATypeButtonFXsData][4]->getAssociatedSingleLabel()->setText("+/"+QString('0'+value));
                }
                else {
                    tabControllers[displayIndex]->mainButtons[_CATypeButtonFXsData][4]->setText("+");
                    tabControllers[displayIndex]->mainButtons[_CATypeButtonFXsData][4]->getAssociatedSingleLabel()->setText("+");
                }
            }
            else if(OSCTagString == "/cameraCumul_can" && (nbargts + 1) >= 2
                    && tabControllers[displayIndex]->mainButtons[_cameraCumulButtonsFXData].size() >= 1
                    && tabControllers[displayIndex]->mainButtons[_cameraCumulButtonsFXData][0]) {
                int value = int(args[0]);
                if(value == 0) {
                    tabControllers[displayIndex]->mainButtons[_cameraCumulButtonsFXData][0]->setText("CaCu:Off");
                    tabControllers[displayIndex]->mainButtons[_cameraCumulButtonsFXData][0]->getAssociatedSingleLabel()->setText("X");
                }
                else if(value == 1) {
                    tabControllers[displayIndex]->mainButtons[_cameraCumulButtonsFXData][0]->setText("CaCu:Add");
                    tabControllers[displayIndex]->mainButtons[_cameraCumulButtonsFXData][0]->getAssociatedSingleLabel()->setText("Add");
                }
                else if(value == 2) {
                    tabControllers[displayIndex]->mainButtons[_cameraCumulButtonsFXData][0]->setText("CaCu:Stmp");
                    tabControllers[displayIndex]->mainButtons[_cameraCumulButtonsFXData][0]->getAssociatedSingleLabel()->setText("Stp");
                }
                else {
                    tabControllers[displayIndex]->mainButtons[_cameraCumulButtonsFXData][0]->setText("CaCu:XOR");
                    tabControllers[displayIndex]->mainButtons[_cameraCumulButtonsFXData][0]->getAssociatedSingleLabel()->setText("XOR");
                }
            }
            else if(OSCTagString == "/sample_setUp_can" && (nbargts + 1) >= 2
                    && tabControllers[displayIndex]->mainButtons[_sensorsButtonsFXData].size() >= 2
                    && tabControllers[displayIndex]->mainButtons[_sensorsButtonsFXData][1]) {
                int value = int(args[0]);
                if(value >= 0) {
                    tabControllers[displayIndex]->mainButtons[_sensorsButtonsFXData][1]->setText("+/"+QString('0'+value));
                    tabControllers[displayIndex]->mainButtons[_sensorsButtonsFXData][1]->getAssociatedSingleLabel()->setText("+/"+QString('0'+value));
                }
                else {
                    tabControllers[displayIndex]->mainButtons[_sensorsButtonsFXData][1]->setText("+");
                    tabControllers[displayIndex]->mainButtons[_sensorsButtonsFXData][1]->getAssociatedSingleLabel()->setText("+");
                }
            }
            break;

            //////////////////////////////////////////////////////////////
            // PATHS DISPLAY
        case _Paths_display:
            displayIndex = _Paths_display;
            break;
#if defined(TVW) || defined(demo)
            //////////////////////////////////////////////////////////////
            // MinimalIF DISPLAY
        case _MinimalIF_display:
            displayIndex = _MinimalIF_display;
            if(OSCTagString == "/pixel_mode_can" && (nbargts + 1) >= 2
                    && tabControllers[displayIndex]->mainButtons[_PixeCAlMinimalIFData].size() >= 1
                    && tabControllers[displayIndex]->mainButtons[_PixeCAlMinimalIFData][0] ) {
                int value = int(args[0]);
                if(value == 0) {
                    tabControllers[displayIndex]->mainButtons[_PixeCAlMinimalIFData][0]->setText("X");
                    tabControllers[displayIndex]->mainButtons[_PixeCAlMinimalIFData][0]->getAssociatedSingleLabel()->setText("noPix");
                }
                else if(value == 1) {
                    tabControllers[displayIndex]->mainButtons[_PixeCAlMinimalIFData][0]->setText("pixel");
                    tabControllers[displayIndex]->mainButtons[_PixeCAlMinimalIFData][0]->getAssociatedSingleLabel()->setText("pi");
                }
                else if(value == 2) {
                    tabControllers[displayIndex]->mainButtons[_PixeCAlMinimalIFData][0]->setText("fireWk");
                    tabControllers[displayIndex]->mainButtons[_PixeCAlMinimalIFData][0]->getAssociatedSingleLabel()->setText("FW");
                }
            }
            else if(OSCTagString == "/pen_grey_can" && (nbargts + 1) >= 2
                    && tabControllers[displayIndex]->mainButtons[_PenGreyButtonTVWsData].size() >= 1
                    && tabControllers[displayIndex]->mainButtons[_PenGreyButtonTVWsData][0] ) {
                int value = int(args[0]);
                if(value == 0) {
                    tabControllers[displayIndex]->mainButtons[_PenGreyButtonTVWsData][0]->setText("Black");
                    tabControllers[displayIndex]->mainButtons[_PenGreyButtonTVWsData][0]->getAssociatedSingleLabel()->setText("B");
                }
                else {
                    tabControllers[displayIndex]->mainButtons[_PenGreyButtonTVWsData][0]->setText("White");
                    tabControllers[displayIndex]->mainButtons[_PenGreyButtonTVWsData][0]->getAssociatedSingleLabel()->setText("W");
                }
            }
            else if(OSCTagString.startsWith("/pen_colorPreset_name_can") && (nbargts + 1) >= 2) {
                int value = OSCTagString.right(1).toInt();
                if(value < (int)tabControllers[displayIndex]->mainButtons[_PenColorMinimalIFData].size()
                        && tabControllers[displayIndex]->mainButtons[_PenColorMinimalIFData][value]) {
                    tabControllers[displayIndex]->mainButtons[_PenColorMinimalIFData][value]->setText(firstArgtString);
                    tabControllers[displayIndex]->mainButtons[_PenColorMinimalIFData][value]->getAssociatedSingleLabel()->setText(firstArgtString);
                }
            }
            else if(OSCTagString == "/CASubType_can" && (nbargts + 1) >= 2
                    && tabControllers[displayIndex]->mainButtons[_PixeCAlMinimalIFData].size() >= 2
                    && tabControllers[displayIndex]->mainButtons[_PixeCAlMinimalIFData][1]) {
                int value = int(args[0]);
                if(value == 0) {
                    tabControllers[displayIndex]->mainButtons[_PixeCAlMinimalIFData][1]->setText("OFF");
                    tabControllers[displayIndex]->mainButtons[_PixeCAlMinimalIFData][1]->getAssociatedSingleLabel()->setText("X");
                }
                else {
                    tabControllers[displayIndex]->mainButtons[_PixeCAlMinimalIFData][1]->setText("ON");
                    tabControllers[displayIndex]->mainButtons[_PixeCAlMinimalIFData][1]->getAssociatedSingleLabel()->setText("ON");
                }
            }
            else if(OSCTagString == "/CA1Type_can" && (nbargts + 1) >= 2
                    && tabControllers[displayIndex]->mainButtons[_PixeCAlMinimalIFData].size() >= 3
                    && tabControllers[displayIndex]->mainButtons[_PixeCAlMinimalIFData][2]) {
                int value = int(args[0]);
                if(value >= 0) {
                    tabControllers[displayIndex]->mainButtons[_PixeCAlMinimalIFData][2]->setText("+/"+QString('0'+value));
                    tabControllers[displayIndex]->mainButtons[_PixeCAlMinimalIFData][2]->getAssociatedSingleLabel()->setText("+/"+QString('0'+value));
                }
                else {
                    tabControllers[displayIndex]->mainButtons[_PixeCAlMinimalIFData][2]->setText("+");
                    tabControllers[displayIndex]->mainButtons[_PixeCAlMinimalIFData][2]->getAssociatedSingleLabel()->setText("+");
                }
            }
            else if(OSCTagString == "/pen_brush_can" && (nbargts + 1) >= 2
                    && tabControllers[displayIndex]->mainButtons[_trackBrushMinimalIFData].size() >= 4
                    && tabControllers[displayIndex]->mainButtons[_trackBrushMinimalIFData][3]) {
                int value = int(args[0]);
                if(value >= 0) {
                    tabControllers[displayIndex]->mainButtons[_trackBrushMinimalIFData][3]->setText("+/"+QString('0'+value));
                    tabControllers[displayIndex]->mainButtons[_trackBrushMinimalIFData][3]->getAssociatedSingleLabel()->setText("+/"+QString('0'+value));
                }
                else {
                    tabControllers[displayIndex]->mainButtons[_trackBrushMinimalIFData][3]->setText("+");
                    tabControllers[displayIndex]->mainButtons[_trackBrushMinimalIFData][3]->getAssociatedSingleLabel()->setText("+");
                }
            }
            break;
#endif
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
        qDebug() << "Error: could not find a usable WinSock DLL!"; // << std::endl;
    }
#endif

    // server initialization
    for( int ind = 0 ; ind < nb_IP_Servers ; ind++ ) {
        IP_Servers[ ind ]->InitServer();
    }
    for( int ind = 0 ; ind < nb_IP_Clients ; ind++ ) {
        // qDebug() << "Client " << ind << " initialization" << ind;
        // qDebug() << "IP_Clients[ ind_IP_Client ]->Remote_server_IP: " << ind << " " << IP_Clients[ind]->Remote_server_IP << "\n";
        // qDebug() << "IP_Clients[ ind_IP_Client ]->Remote_server_port: " << IP_Clients[ind]->Remote_server_port << "\n";
        IP_Clients[ ind ]->InitClient();
        // qDebug() << "Client name " << IP_Clients[ ind ]->id;
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
    // qDebug() << "End of scene initialization";
}

//////////////////////////////////////////////////////////////////////////////
/// \brief pg_store_message_udp
/// \param pattern
/// \param message
/// \param targetHostid
/// Stores a message in the message queue

void UDPInOut::pg_store_message_udp( QString *message , QString *pattern ,
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
        qDebug() << "UDP client unknown " << targetHostid; // << std::endl;
        return;
    }
    // qDebug() << "send_message_udp " << message << " " << pattern << " " << nb_IP_Clients << " " << targetHost;
    targetHost->storeIP_output_message( message ,
                                        pattern );
}

void UDPInOut::pg_store_message_udp( QString *message , QString *pattern ) {
    // does not process the messages of the empty controllers
    if(message->at( 0 ) == 'X') {
        return;
    }
    // qDebug() << "send_message_udp " << message << " " << pattern << " " << nb_IP_Clients << " " << targetHost;
    IP_Clients[ 0 ]->storeIP_output_message( message ,
                                             pattern );
}

void UDPInOut::pg_store_message_udp( QString *message ) {
    // does not process the messages of the empty controllers
    if(message->at( 0 ) == 'X') {
        return;
    }
    //    qDebug() << "send_message_udp " << message; // << std::endl;
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
    // qDebug() << "releaseUDP";
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
