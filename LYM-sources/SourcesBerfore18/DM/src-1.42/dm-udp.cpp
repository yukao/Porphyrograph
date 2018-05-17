/*! \file dm-udp.cpp
 * 
 * 
 *     Part of this source file is taken from Virtual Choreographer
 *     http://virchor.sourceforge.net/
 * 
 *     File dm-udp.cpp
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

#include "dm-all_include.h"

dm_MySocketAddresse::dm_MySocketAddresse() {
  memset((char *)&d_sockAddresse,0,sizeof(d_sockAddresse));
  host = NULL;
  host_IP = NULL;
  InitString( &host );
  InitString( &host_IP );
  d_valid = false;
}

dm_MySocketAddresse::~dm_MySocketAddresse(void) {
	if (host)
	{
		delete [] host;
		host = NULL;
	}

	if (host_IP)
	{
		delete [] host_IP;
		host_IP = NULL;
	}
}

void dm_MySocketAddresse::CalculIpDot() {
  // On obtient la string representant l'adresse en notation "a.b.c.d"
  char* s = inet_ntoa( d_sockAddresse.sin_addr );
  if( s == NULL )
    printf( "Erreur: dm_MySocketAddresse::CalculIpDot() ne peut convertir l'adresse.\n" );
  else
    StrCpy( &host_IP, s );
}

void dm_MySocketAddresse::InitFromResolver(char *_host, const int _port) {
  struct hostent 		*hostinfo;

  StrCpy( &host , _host );  
  port=_port;
  d_sockAddresse.sin_family=AF_INET;
  d_sockAddresse.sin_port=htons(port);

  hostinfo=gethostbyname(host);
  if (hostinfo==NULL) {
    fprintf( stdout , "Unknown host %s \n", _host);
    memset((char *)&d_sockAddresse,0,sizeof(d_sockAddresse));
    d_valid = false;
    return;
  }

  d_sockAddresse.sin_addr=*(struct in_addr*)hostinfo->h_addr;
  CalculIpDot();
  d_valid = true;
} 

void dm_MySocketAddresse::test( void ) {
  dm_MySocketAddresse sock;
  char nam[100];

  gethostname(nam, 100);
  printf( "%s\n" , nam );
  sock.InitFromResolver(nam, 3120);

  char* s = inet_ntoa(sock.d_sockAddresse.sin_addr);

  printf( "%s\n" , s );
}

dm_IPClient::dm_IPClient( void ) {
  id = NULL;
  InitString( &id );
  
  // remote server address
  Remote_server_IP = NULL;
  InitString( &Remote_server_IP );
  Remote_server_port = 1979;
  Remote_server_host = NULL;

  // local client port
  Local_client_port = 1980;
  
  // connection to remote server
  connected = false;
  
  // remote server socket
  SocketToRemoteServer = -1;

  // client stack: stores pending output messages
  output_pattern_stack = NULL;
  output_message_stack = NULL;
  depth_output_stack = 10;
  current_depth_output_stack = -1;
  maximal_IP_message_delay = 0.1F;
  last_IP_message_time = 0;

  // output message format
  send_format = Plain;
  send_prefix = NULL;
  InitString( &send_prefix );

  // labelling of output messages
  send_ID = true;
  first_IP_message_number = 0;
  current_IP_message_number = 0;

  // acknowledgement of received message 
  IP_message_acknowledgement = false;

  // console trace
  IP_message_trace = false;

  // byte order reversal
#if CPU_ENDIAN == little
  OSC_endian_reversal = false;
#else
  OSC_endian_reversal = true;
#endif

}
    
dm_IPClient::~dm_IPClient(void) {
  if (id) {
    delete [] id;
    id = NULL;
  }

  if (Remote_server_IP) {
    delete [] Remote_server_IP;
    Remote_server_IP = NULL;
  }

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

  if (send_prefix) {
    delete [] send_prefix;
    send_prefix = NULL;
  }
#ifdef WIN32
  closesocket(SocketToRemoteServer);
#endif
}

void dm_IPClient::Init( void ) {

  //////////////////////////////
  // remote server opening

  /* get server IP address (no check if input is IP address or DNS name */
  Remote_server_host = gethostbyname(Remote_server_IP);
  if(Remote_server_host==NULL) {
    sprintf( ErrorStr , "Error: unknown remote host IP '%s'!" , Remote_server_IP ); ReportError( ErrorStr ); 
  }
  else {
    printf("Network client: sending data to '%s' (IP : %s) on port %d\n", 
	   Remote_server_host->h_name,
	   inet_ntoa(*(struct in_addr *)Remote_server_host->h_addr_list[0]),
	   Remote_server_port);
    
    /* socket creation */
    remoteServAddr.sin_family = Remote_server_host->h_addrtype;
    memcpy((char *) &remoteServAddr.sin_addr.s_addr, 
	   Remote_server_host->h_addr_list[0], Remote_server_host->h_length);
    remoteServAddr.sin_port = htons(Remote_server_port);
    
    SocketToRemoteServer = socket(AF_INET,SOCK_DGRAM,0);
    if(SocketToRemoteServer < 0) {
      sprintf( ErrorStr , "Error: cannot open socket to remote server!" ); ReportError( ErrorStr ); 
    }
  }

  // system initializing
  if( Remote_server_host && SocketToRemoteServer >= 0 ) {
    IP_OutputStackInitialization();
  }
}

void dm_IPClient::IP_OutputStackInitialization( void ) {
  // output message stack initializing
  output_message_stack = new Pchar[depth_output_stack];
  for(int ind_stack = 0 ; ind_stack < depth_output_stack ;
      ind_stack++ ) {
    output_message_stack[ ind_stack ] = NULL;
    InitString( &(output_message_stack[ ind_stack ]) );
  }
  current_depth_output_stack = -1;
  // output pattern stack initializing
  output_pattern_stack = new Pchar[depth_output_stack];
  for(int ind_stack = 0 ; ind_stack < depth_output_stack ;
      ind_stack++ ) {
    output_pattern_stack[ ind_stack ] = NULL;
    InitString( &(output_pattern_stack[ ind_stack ]) );
  }
}
  
void dm_IPClient::SetId( char *newId ) {
  StrCpy( &id , newId );
}

void dm_IPClient::setParameters( dm_XMLTag *XMLtag , bool atRunTime ,
				  FloatBinOp oper , IntBinOp operI ) {
  
  // remote server ID
  SetId( XMLtag->getAttributeStringValue( "id" ) );

  // remote server address
  if( XMLtag->hasAttribute( "server_ip" ) ) {
    StrCpy( &Remote_server_IP ,
	     XMLtag->getAttributeStringValue( "server_ip" ) );
  }
  else if( XMLtag->hasAttribute( "remote_ip" ) ) {
    StrCpy( &Remote_server_IP ,
	     XMLtag->getAttributeStringValue( "remote_ip" ) );
  }

  if( XMLtag->hasAttribute( "server_port" ) ) {
    Remote_server_port 
      = (unsigned int)XMLtag->getAttributeIntValue( "server_port" , 
						    atRunTime );
  }
  else if( XMLtag->hasAttribute( "remote_port" ) ) {
    Remote_server_port 
      = (unsigned int)XMLtag->getAttributeIntValue( "remote_port" , 
						    atRunTime );
  }

  if( XMLtag->hasAttribute( "client_port" ) ) {
    Local_client_port 
      = (unsigned int)XMLtag->getAttributeIntValue( "client_port" , 
						    atRunTime );
  }

  // client stack: stores pending output messages
  if( XMLtag->hasAttribute( "output_message_stack_depth" ) ) {
    depth_output_stack 
      = (int)XMLtag->getAttributeIntValue( "output_message_stack_depth" , 
					   atRunTime );
    if( depth_output_stack < 2 ) {
      depth_output_stack = 2;
    }
  }

  if( XMLtag->hasAttribute( "maximal_IP_message_delay" ) ) {
    maximal_IP_message_delay 
      = (float)XMLtag->getAttributeDoubleValue( "maximal_IP_message_delay" , 
						atRunTime );
  }
  else if( XMLtag->hasAttribute( "maximal_UDP_message_delay" ) ) {
    maximal_IP_message_delay 
      = (float)XMLtag->getAttributeDoubleValue( "maximal_UDP_message_delay" , 
						atRunTime );
  }

  // output message format
  if( XMLtag->hasAttribute( "send_format" ) ) {
    char *format = XMLtag->getAttributeStringValue( "send_format" );
    for( int ind = 0 ; ind < EmptyExternalMessageFormat ; ind++ ) {
      if( strcmp( format , ExternalMessageFormatString[ind] ) == 0 ) {
	send_format = (ExternalMessageFormat)ind;
	break;
      }
    }
    if( send_format == EmptyExternalMessageFormat ) {
      sprintf( ErrorStr , "Error: unknown send message format [%s]!" , format ); ReportError( ErrorStr ); throw 249;
    }
  }

  if( XMLtag->hasAttribute( "send_prefix" ) ) {
    StrCpy( &send_prefix , XMLtag->getAttributeStringValue( "send_prefix" ) );
  }

  // labelling of output messages
  if( XMLtag->hasAttribute( "send_ID" ) ) {
    XMLtag->BooleanFieldValue ( "send_ID" , &send_ID );
  }

  if( XMLtag->hasAttribute( "first_IP_message_number" ) ) {
    first_IP_message_number 
      = (unsigned int)XMLtag->getAttributeIntValue( "first_IP_message_number" , 
						    atRunTime );
    current_IP_message_number = first_IP_message_number;
  }
  else if( XMLtag->hasAttribute( "first_UDP_message_number" ) ) {
    first_IP_message_number 
      = (unsigned int)XMLtag->getAttributeIntValue( "first_UDP_message_number" , 
						    atRunTime );
    current_IP_message_number = first_IP_message_number;
  }

  // acknowledgement of sent message
  if( XMLtag->hasAttribute( "IP_message_acknowledgement" ) ) {
    XMLtag->BooleanFieldValue (  "IP_message_acknowledgement" ,
				 &IP_message_acknowledgement );
  }
  else if( XMLtag->hasAttribute( "UDP_message_acknowledgement" ) ) {
    XMLtag->BooleanFieldValue (  "UDP_message_acknowledgement" ,
				 &IP_message_acknowledgement );
  }

  // console trace
  if( XMLtag->hasAttribute( "IP_message_trace" ) ) {
    XMLtag->BooleanFieldValue ( "IP_message_trace" ,
				&IP_message_trace );
  }
  else if( XMLtag->hasAttribute( "UDP_message_trace" ) ) {
    XMLtag->BooleanFieldValue ( "UDP_message_trace" ,
				&IP_message_trace );
  }

  // console trace
  if( XMLtag->hasAttribute( "OSC_endian_reversal" ) ) {
    XMLtag->BooleanFieldValue ( "OSC_endian_reversal" ,
				&OSC_endian_reversal );
  }
}

bool dm_IPClient::getParameters( char *attribute , 
				    DataType * dataType ,
				    double * valDouble ,
				    int * valInt ,
				    bool * valBool ,
				    char ** valString ,
			      int * tableOrMatrixSize ,
			      dm_ValScalar ** valScalars ) {
  if( strcmp( attribute , "id" ) == 0 ) {
    *dataType = DataTypeString;
    *valString = id;
    return true;
  }
  else if( strcmp( attribute , "remote_ip" ) == 0 ) {
    *dataType = DataTypeString;
    *valString = Remote_server_IP;
    return true;
  }
  else if( strcmp( attribute , "remote_port" ) == 0 ) {
    *dataType = DataTypeInt;
    *valInt = Remote_server_port;
    return true;
  }
  else if( strcmp( attribute , "client_port" ) == 0 ) {
    *dataType = DataTypeInt;
    *valInt = Local_client_port;
    return true;
  }
  else if( strcmp( attribute , "output_message_stack_depth" ) == 0 ) {
    *dataType = DataTypeInt;
    *valInt = depth_output_stack;
    return true;
  }
  else if( strcmp( attribute , "maximal_IP_message_delay" ) == 0 
	   || strcmp( attribute , "maximal_UDP_message_delay" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)maximal_IP_message_delay;
    return true;
  }
  else if( strcmp( attribute , "send_format" ) == 0 ) {
    *dataType = DataTypeString;
    *valString = (char *)ExternalMessageFormatString[send_format];
    return true;
  }
  else if( strcmp( attribute , "send_prefix" ) == 0 ) {
    *dataType = DataTypeString;
    *valString = send_prefix;
    return true;
  }
  else if( strcmp( attribute , "send_ID" ) == 0 ) {
    *dataType = DataTypeBool;
    *valBool = send_ID;
    return true;
  }
  else if( strcmp( attribute , "first_IP_message_number" ) == 0 
	   || strcmp( attribute , "first_UDP_message_number" ) == 0  ) {
    *dataType = DataTypeInt;
    *valInt = current_IP_message_number;
    return true;
  }
  else if( strcmp( attribute , "IP_message_acknowledgement" ) == 0
	   || strcmp( attribute , "UDP_message_acknowledgement" ) == 0 ) {
    *dataType = DataTypeBool;
    *valBool = IP_message_acknowledgement;
    return true;
  }
  else if( strcmp( attribute , "IP_message_trace" ) == 0 
	   || strcmp( attribute , "UDP_message_trace" ) == 0  ) {
    *dataType = DataTypeBool;
    *valBool = IP_message_trace;
    return true;
  }
  else if( strcmp( attribute , "OSC_endian_reversal" ) == 0 ) {
    *dataType = DataTypeBool;
    *valBool = OSC_endian_reversal;
    return true;
  }
  return false;
}

void dm_IPClient::print( FILE *file , int isLong , int depth ) {
  indent( file , depth );
  fprintf( file , "<ip_client id=\"%s\" server_ip=\"%s\" server_port=\"%d\" client_port=\"%d\" IP_message_acknowledgement=\"%s\" first_IP_message_number=\"%d\" maximal_IP_message_delay=\"%.5f\" IP_message_trace=\"%s\" OSC_endian_reversal=\"%s\" output_message_stack_depth=\"%d\" send_format=\"%s\" send_prefix=\"%s\" send_ID=\"%s\" />\n" , id , Remote_server_IP , Remote_server_port , Local_client_port , BooleanString[BooleanToInt(IP_message_acknowledgement)] , first_IP_message_number , maximal_IP_message_delay , BooleanString[BooleanToInt(IP_message_trace)] , BooleanString[BooleanToInt(OSC_endian_reversal)] , depth_output_stack , ExternalMessageFormatString[send_format] , send_prefix , BooleanString[BooleanToInt(send_ID)] );  
}

void dm_IPClient::sendIPmessages( void ) {
  if( SocketToRemoteServer < 0 ) {
    return;
  }

  while( current_depth_output_stack >= 0 ) {
    // printf( "sendUDPmessage %d\n" , current_depth_output_stack );

    // checks whether last message was received
    // or it is the first message
    // or the elapsed time since the last message is greater than the
    // maximal delay
    double current_time =  RealTime();

    bool message_received = true;

    if( IP_message_acknowledgement ) {
      message_received = false;
      for( int ind = 0 ; ind < DM_EnvironmentNode->nb_IP_Servers ; ind++ ) {
	if( DM_EnvironmentNode->IP_Servers[ ind ]->last_received_IP_message_number 
	    == (current_IP_message_number - 1)) {
	  message_received = true;
	  break;
	}
      }
    }

    if( message_received
	|| current_IP_message_number == first_IP_message_number
	|| (float)((double)(current_time - last_IP_message_time)/1000.0)
	> (float)maximal_IP_message_delay ) {


      // printf( "send message [%s]\n" , output_message_stack[ 0 ]);
    
      // copies the earliest message
      int     indLocalCommandLine = 0;

      if( send_format == Plain ) {
	if( send_ID ) {
	  sprintf(Output_Message_String , "%s%u " ,
		   send_prefix ,
		   current_IP_message_number++ );
	  strncat( Output_Message_String , output_message_stack[ 0 ] , 
		   DM_EnvironmentNode->Max_network_message_length 
		   - strlen( Output_Message_String ) - 1 );
	}
	else {
	  if( *send_prefix ) {
	    sprintf(Output_Message_String , "%s " ,
		     send_prefix );
	    strncat( Output_Message_String , output_message_stack[ 0 ] , 
		   DM_EnvironmentNode->Max_network_message_length 
		   - strlen( Output_Message_String ) - 1 );
	  }
	  else {
	    strncpy( Output_Message_String , output_message_stack[ 0 ] , 
		     DM_EnvironmentNode->Max_network_message_length - 1 );
	  }
	}
      }
      else if( send_format == OSC ) {
	// sends bundle through OSC
	// message that does not begin by an OSC address
	if( *(output_message_stack[ 0 ]) != '/' ) {
	  strcpy( Output_Message_OSC ,  "#bundle" );
	  for( int ind = strlen( "#bundle" ) ; ind < 20 - 1 ; ind++ ) {
	    Output_Message_OSC[ ind ] = 0;
	  }
	  if( send_ID ) {
	    sprintf( Output_Message_OSC + 20 , "%s%u " , 
		     send_prefix ,
		     current_IP_message_number++ );
	    strncat( Output_Message_OSC + 20 , output_message_stack[ 0 ] , 
		     2 * DM_EnvironmentNode->Max_network_message_length 
		     - strlen( Output_Message_OSC + 20 ) - 21 );
	  }
	  else {
	    if( *send_prefix ) {
	      sprintf( Output_Message_OSC + 20 , "%s " ,
		       send_prefix );
	    strncat( Output_Message_OSC + 20 , output_message_stack[ 0 ] , 
		     2 * DM_EnvironmentNode->Max_network_message_length 
		     - strlen( Output_Message_OSC + 20 ) - 21 );
	    }
	    else {
	      strncpy( Output_Message_OSC + 20 , output_message_stack[ 0 ] , 
		       2 * DM_EnvironmentNode->Max_network_message_length - 21 );
	    }
	  }
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
	    p_c = messageOut[ inChSource ];
	    inChSource++;
	    while( (p_c != (int)EOF) && p_c ) {
	      // printf( "next_char_pattern %c\n" , next_char_pattern );
	      SpaceCommentsInclude(NULL,&p_c,messageOut,&inChSource);
	      // printf( "p_c %c\n" , p_c );
	      // float: f type in pattern or number
	      if( next_char_pattern == 'f'
		  || (!next_char_pattern
		      && ( isdigit( p_c) || p_c == '-' ) ) ) {
		float f = (float) ReadFloat( NULL , &p_c , true ,
					     messageOut , &inChSource , false );
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
		int i = (int) ReadInteger( NULL , &p_c , true ,
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
		char *temporaryValue = NULL;
		if( messageOut[ inChSource ] == '"' ) {
		  ReadStringWithFrontiers( NULL , &p_c , 
					   &temporaryValue ,
					   '"' , '"' , true , 
					   messageOut , &inChSource );
		}
		else {
		  ReadStringWithTermination( NULL , &p_c , 
					     &temporaryValue ,
					     ' ' , true , 
					     messageOut , &inChSource );
		}

		strcpy( Output_Message_ArgList + indArgList , temporaryValue );
		if (temporaryValue) {
		  delete [] temporaryValue;
		  temporaryValue = NULL;
		}
		
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
		 < (2 * DM_EnvironmentNode->Max_network_message_length - 1 ) ) {
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
	      sprintf( ErrorStr , "Error: udp client: OSC message incompletely sent [%s]!" ,Output_Message_OSC ); ReportError( ErrorStr ); // close(SocketToRemoteServer);
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
	StrCpy( &(output_message_stack[ ind_mess ]) , 
		output_message_stack[ ind_mess + 1 ] );
	StrCpy( &(output_pattern_stack[ ind_mess ]) , 
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
	rc = sendto(SocketToRemoteServer, Output_Message_String, 
		    localCommandLineLength, 0, 
		    (struct sockaddr *) &remoteServAddr, 
		    sizeof(remoteServAddr));
	if(rc != localCommandLineLength) {
	  sprintf( ErrorStr , "Error: udp client: data incompletely sent [%s] (%d/%d)!" , Output_Message_String , rc , localCommandLineLength ); ReportError( ErrorStr ); // close(SocketToRemoteServer); throw 273;
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
	rc = sendto(SocketToRemoteServer, Output_Message_OSC, len , 0, 
		    (struct sockaddr *) &remoteServAddr, 
		    sizeof(remoteServAddr));
	if(rc != len) {
	  sprintf( ErrorStr , "Error: udp client: data incompletely sent [%s]!" ,Output_Message_OSC ); ReportError( ErrorStr ); // close(SocketToRemoteServer); throw 273;
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
	rc = sendto(SocketToRemoteServer,Output_Message_OSC, 
		    localCommandLineLength, 0, 
		    (struct sockaddr *) &remoteServAddr, 
		    sizeof(remoteServAddr));
	if(rc != localCommandLineLength) {
	  sprintf( ErrorStr , "Error: udp client: data incompletely sent [%s]!" ,Output_Message_OSC ); ReportError( ErrorStr ); // close(SocketToRemoteServer); throw 273;
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
      last_IP_message_time =  RealTime();
    }
    else {
      if( IP_message_trace ) {
	printf( "udp client: delay %.5f / %.5f \n",
		(float)((double)(current_time - last_IP_message_time)/1000.0),
		(float)maximal_IP_message_delay );
      }
      return;
    }
  }
}

void dm_IPClient::storeIP_output_message( char *commandLine ,
					    char *pattern ) {

  // printf( "commandLine %s %d\n" , commandLine , current_depth_output_stack );
  if( SocketToRemoteServer < 0 ) {
    return;
  }

  // printf( "commandLine %s %d\n" , commandLine , current_depth_output_stack );
  if( current_depth_output_stack < depth_output_stack - 1 ) {
    current_depth_output_stack++;
    StrCpy( &(output_message_stack[ current_depth_output_stack ]) , 
	     commandLine );
    if( pattern && *pattern ) {
      StrCpy( &(output_pattern_stack[ current_depth_output_stack ]) , 
	      pattern );
    }
    else {
      StrCpy( &(output_pattern_stack[ current_depth_output_stack ]) , 
	      "" );
    }
  }
  else {
    sprintf( ErrorStr , "Error: UDP output message stack overflow %d %d (%s)!" , current_depth_output_stack , depth_output_stack , commandLine ); ReportError( ErrorStr ); 
  } 
}

void dm_IPClient::operator=(dm_IPClient& c) {
  StrCpy( &id , c.id );
  StrCpy( &Remote_server_IP , c.Remote_server_IP );
  Remote_server_port = c.Remote_server_port;
  Local_client_port = c.Local_client_port;
  Remote_server_host = c.Remote_server_host;

  first_IP_message_number = c.first_IP_message_number;

  depth_output_stack = c.depth_output_stack;
  IP_message_trace = c.IP_message_trace;
  OSC_endian_reversal = c.OSC_endian_reversal;
  send_format = c.send_format;
  StrCpy( &send_prefix , c.send_prefix );
  send_ID = c.send_ID;
  IP_message_acknowledgement = c.IP_message_acknowledgement;
}

dm_IPServer::dm_IPServer( void ) {
  // local server ID
  id = NULL;
  InitString( &id );
  
  // local server port
  Local_server_port = 1979;

  // remote client address
  Remote_client_IP = NULL;
  InitString( &Remote_client_IP );
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
  receive_prefix = NULL;
  InitString( &receive_prefix );
  offset_IP_message_received = 0;

  // labelling of received messages
  receive_ID = true;

  // acknowledgement of received message 
  IP_message_acknowledgement = false;
  acknowledge_prefix = NULL;
  InitString( &acknowledge_prefix );
  last_received_IP_message_number = INT_MAX;

  // carbon copy to a remote client
  carbon_copyID = NULL;
  InitString( &carbon_copyID );
  carbon_copyIPClient = NULL;

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

dm_IPServer::~dm_IPServer( void ) {
  if (id)
    {
      delete [] id;
      id = NULL;
    }

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

  if (receive_prefix)
    {
      delete [] receive_prefix;
      receive_prefix = NULL;
    }

  if (acknowledge_prefix)
    {
      delete [] acknowledge_prefix;
      acknowledge_prefix = NULL;
    }

  if (carbon_copyID)
    {
      delete [] carbon_copyID;
      carbon_copyID = NULL;
    }

#ifdef WIN32
  closesocket(SocketToLocalServer);
#endif
}

void dm_IPServer::Init( void ) {
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
  localServAddr.sin_port = htons(Local_server_port);
  
  int rc = bind( SocketToLocalServer, (struct sockaddr *) &localServAddr,
		 sizeof(localServAddr) );
  if(rc < 0) {
    sprintf( ErrorStr , "Error: cannot bind locat port number %d!" , Local_server_port ); ReportError( ErrorStr ); 
    return;
  }
  printf( "Network server: bound local port number %d!\n" , Local_server_port );

  printf("Network server: waiting for data on port %u\n", Local_server_port);

  IP_InputStackInitialization();
}

void dm_IPServer::IP_InputStackInitialization( void ) {
  // input message stack initializing
  input_message_stack = new Pchar[depth_input_stack];
  input_message_length_stack = new int[depth_input_stack];
  for(int ind_stack = 0 ; ind_stack < depth_input_stack ; 
      ind_stack++ ) {
    input_message_stack[ ind_stack ] = NULL;
    InitString( &(input_message_stack[ ind_stack ]) );
    input_message_length_stack[ ind_stack ] = 0;
  }
  current_depth_input_stack = 0;
}

void dm_IPServer::SetId( char *newId ) {
  StrCpy( &id , newId );
}

void dm_IPServer::setParameters( dm_XMLTag *XMLtag , bool atRunTime ,
				  FloatBinOp oper , IntBinOp operI ) {
  
  // local server ID
  SetId( XMLtag->getAttributeStringValue( "id" ) );

  // local server address
  if( XMLtag->hasAttribute( "local_port" ) ) {
    Local_server_port 
      = (unsigned int)XMLtag->getAttributeIntValue( "local_port" , 
						    atRunTime );
  }
  else if( XMLtag->hasAttribute( "server_port" ) ) {
    Local_server_port 
      = (unsigned int)XMLtag->getAttributeIntValue( "server_port" , 
						    atRunTime );
  }

  // remote client address
  if( XMLtag->hasAttribute( "client_ip" ) ) {
    StrCpy( &Remote_client_IP ,
	    XMLtag->getAttributeStringValue( "client_ip" ) );
  }
  if( XMLtag->hasAttribute( "client_port" ) ) {
    Remote_client_port 
      = (unsigned int)XMLtag->getAttributeIntValue( "client_port" , 
						    atRunTime );
  }

  // server stack: stores received input messages
  if( XMLtag->hasAttribute( "input_message_stack_depth" ) ) {
    depth_input_stack 
      = (int)XMLtag->getAttributeIntValue( "input_message_stack_depth" , 
					   atRunTime );
    if( depth_input_stack < 2 ) {
      depth_input_stack = 2;
    }
  }

  // received message format
  if( XMLtag->hasAttribute( "receive_format" ) ) {
    char *format = XMLtag->getAttributeStringValue( "receive_format" );
    for( int ind = 0 ; ind < EmptyExternalMessageFormat ; ind++ ) {
      if( strcmp( format , ExternalMessageFormatString[ind] ) == 0 ) {
	receive_format = (ExternalMessageFormat)ind;
	break;
      }
    }
    if( receive_format == EmptyExternalMessageFormat ) {
      sprintf( ErrorStr , "Error: unknown receive message format [%s]!" , format ); ReportError( ErrorStr ); throw 249;
    }
  }

  if( XMLtag->hasAttribute( "receive_prefix" ) ) {
    StrCpy( &receive_prefix , XMLtag->getAttributeStringValue( "receive_prefix" ) );
  }

  // labelling of received messages
  if( XMLtag->hasAttribute( "receive_ID" ) ) {
    XMLtag->BooleanFieldValue ( "receive_ID" , &receive_ID );
  }

  if( XMLtag->hasAttribute( "offset_IP_message_received" ) ) {
    offset_IP_message_received 
      = (int)XMLtag->getAttributeIntValue( "offset_IP_message_received" , 
					   atRunTime );
  }
  else if( XMLtag->hasAttribute( "offset_UDP_message_received" ) ) {
    offset_IP_message_received 
      = (int)XMLtag->getAttributeIntValue( "offset_UDP_message_received" , 
					   atRunTime );
  }

  if( XMLtag->hasAttribute( "carbon_copy" ) ) {
    StrCpy( &carbon_copyID , XMLtag->getAttributeStringValue( "carbon_copy" ) );
  }

  // acknowledgement of received message format
  if( XMLtag->hasAttribute( "IP_message_acknowledgement" ) ) {
    XMLtag->BooleanFieldValue ( "IP_message_acknowledgement" ,
				&IP_message_acknowledgement );
  }
  else if( XMLtag->hasAttribute( "UDP_message_acknowledgement" ) ) {
    XMLtag->BooleanFieldValue ( "UDP_message_acknowledgement" ,
				&IP_message_acknowledgement );
  }

  if( XMLtag->hasAttribute(  "acknowledge_prefix" ) ) {
    StrCpy( &acknowledge_prefix , 
	    XMLtag->getAttributeStringValue( "acknowledge_prefix" ) );
  }

  // console trace
  if( XMLtag->hasAttribute( "IP_message_trace" ) ) {
    XMLtag->BooleanFieldValue ( "IP_message_trace" ,
				&IP_message_trace );
  }
  else if( XMLtag->hasAttribute( "UDP_message_trace" ) ) {
    XMLtag->BooleanFieldValue ( "UDP_message_trace" ,
				&IP_message_trace );
  }

  // byte order reversal
  if( XMLtag->hasAttribute( "OSC_endian_reversal" ) ) {
    XMLtag->BooleanFieldValue ( "OSC_endian_reversal" ,
				&OSC_endian_reversal );
  }

  // duplicate OSC message removal
  if( XMLtag->hasAttribute( "OSC_duplicate_removal" ) ) {
    XMLtag->BooleanFieldValue ( "OSC_duplicate_removal" ,
				&OSC_duplicate_removal );
  }
}

bool dm_IPServer::getParameters( char *attribute , 
				    DataType * dataType ,
				    double * valDouble ,
				    int * valInt ,
				    bool * valBool ,
				    char ** valString ) {
  if( strcmp( attribute , "id" ) == 0 ) {
    *dataType = DataTypeString;
    *valString = id;
    return true;
  }
  else if( strcmp( attribute , "local_port" ) == 0 ) {
    *dataType = DataTypeInt;
    *valInt = Local_server_port;
    return true;
  }
  else if( strcmp( attribute , "server_port" ) == 0 ) {
    *dataType = DataTypeInt;
    *valInt = Local_server_port;
    return true;
  }
  else if( strcmp( attribute , "client_ip" ) == 0 ) {
    *dataType = DataTypeString;
    *valString = Remote_client_IP;
    return true;
  }
  else if( strcmp( attribute , "client_port" ) == 0 ) {
    *dataType = DataTypeInt;
    *valInt = Remote_client_port;
    return true;
  }
  else if( strcmp( attribute , "input_message_stack_depth" ) == 0 ) {
    *dataType = DataTypeInt;
    *valInt = depth_input_stack;
    return true;
  }
  else if( strcmp( attribute , "receive_format" ) == 0 ) {
    *dataType = DataTypeString;
    *valString = (char *)ExternalMessageFormatString[receive_format];
    return true;
  }
  else if( strcmp( attribute , "receive_prefix" ) == 0 ) {
    *dataType = DataTypeString;
    *valString = receive_prefix;
    return true;
  }
  else if( strcmp( attribute , "receive_ID" ) == 0 ) {
    *dataType = DataTypeBool;
    *valBool = receive_ID;
    return true;
  }
  else if( strcmp( attribute , "offset_IP_message_received" ) == 0 
	   || strcmp( attribute , "offset_UDP_message_received" ) == 0 ) {
    *dataType = DataTypeInt;
    *valInt = offset_IP_message_received;
    return true;
  }
  else if( strcmp( attribute , "carbon_copy" ) == 0 ) {
    *dataType = DataTypeString;
    *valString = carbon_copyID;
    return true;
  }
  else if( strcmp( attribute , "IP_message_acknowledgement" ) == 0 
	   || strcmp( attribute , "UDP_message_acknowledgement" ) == 0 ) {
    *dataType = DataTypeBool;
    *valBool = IP_message_acknowledgement;
    return true;
  }
  else if( strcmp( attribute , "acknowledge_prefix" ) == 0 ) {
    *dataType = DataTypeString;
    *valString = acknowledge_prefix;
    return true;
  }
  else if( strcmp( attribute , "IP_message_trace" ) == 0
	   || strcmp( attribute , "UDP_message_trace" ) == 0 ) {
    *dataType = DataTypeBool;
    *valBool = IP_message_trace;
    return true;
  }
  else if( strcmp( attribute , "OSC_endian_reversal" ) == 0 ) {
    *dataType = DataTypeBool;
    *valBool = OSC_endian_reversal;
    return true;
  }
  else if( strcmp( attribute , "OSC_duplicate_removal" ) == 0 ) {
    *dataType = DataTypeBool;
    *valBool = OSC_duplicate_removal;
    return true;
  }
  return false;
}

void dm_IPServer::print( FILE *file , int isLong , int depth ) {
  indent( file , depth );
  fprintf( file , "<ip_server id=\"%s\" server_port=\"%d\" client_ip=\"%s\" client_port=\"%d\" IP_message_acknowledgement=\"%s\" IP_message_trace=\"%s\" OSC_endian_reversal=\"%s\" OSC_duplicate_removal=\"%s\" input_message_stack_depth=\"%d\" receive_format=\"%s\" receive_prefix=\"%s\" acknowledge_prefix=\"%s\" offset_IP_message_received=\"%d\" carbon_copy=\"%s\" receive_ID=\"%s\" />\n" , id , Local_server_port , Remote_client_IP , Remote_client_port , BooleanString[BooleanToInt(IP_message_acknowledgement)] , BooleanString[BooleanToInt(IP_message_trace)] , BooleanString[BooleanToInt(OSC_endian_reversal)] , BooleanString[BooleanToInt(OSC_duplicate_removal)] , depth_input_stack , ExternalMessageFormatString[receive_format] , receive_prefix , acknowledge_prefix , offset_IP_message_received , carbon_copyID , BooleanString[BooleanToInt(receive_ID)] );
}

void dm_IPServer::processReceivedOSC( char *localCommandLine , 
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
    char paramTypesId[ XMLAttrValLength ];
    char argument[ NumberMaximalLength ];
    while( indChar < n && message[indChar] != 0 
	   && nbParam < XMLAttrValLength ) {
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

int dm_IPServer::receiveOneMessage( char *message ) {
  // message length
  int n = 0;

  // one shot UDP reception
  n = recv(SocketToLocalServer, message, 
	   DM_EnvironmentNode->Max_network_message_length, 0);

  if( carbon_copyIPClient ) {
    int rc;
    rc = sendto(carbon_copyIPClient->SocketToRemoteServer, message, 
		n, 0, 
		(struct sockaddr *) &(carbon_copyIPClient->remoteServAddr), 
		sizeof(carbon_copyIPClient->remoteServAddr));
    if(rc != n) {
      sprintf( ErrorStr , "Error: udp client: data incompletely sent [%s]!" , message ); ReportError( ErrorStr ); // close(SocketToRemoteServer); throw 273;
    }
  }

  return n;
}

void dm_IPServer::receiveIPMessages( void ) {
  if( SocketToLocalServer < 0 ) {
    return;
  }
    
  //   struct sockaddr_in clientAdddress;
  //struct sockaddr_in servAddr;
    
  /* initXMLTag buffer */
  memset(Input_Message_String,0x0,
	 DM_EnvironmentNode->Max_network_message_length);
  memset(Input_Message_Local_Commande_String,0x0,
	 DM_EnvironmentNode->Max_network_message_length);
  
  /* receive message */
  // #ifdef IRIX
  //   int clientLength = sizeof(clientAdddress);
  // #else
  //   socklen_t clientLength = sizeof(clientAdddress);
  // #endif
  //   int n = recvfrom(SocketToLocalServer, message, XMLAttrValLength, 0, 
  // 		   (struct sockaddr *) &clientAdddress, &clientLength);
  int n = receiveOneMessage( Input_Message_String );

//   if( n>= 0 ) {
//     printf( "Message size %d\n" , n );
//     for( int ind = 0 ; ind < n ; ind++ ) {
//       printf( "%c(%d) " , Input_Message_String[ind] , (int)Input_Message_String[ind] );
//     }
//     printf( "\n" );
//   }
    
    
  // initializes the stack in which message strings are stored
  current_depth_input_stack = 0;

  while( n > 0 ){
      
    ///////////////////////////////////////////////////////
    // Plain format (string)

    if( receive_format == Plain ) {
      if( IP_message_acknowledgement
	  && strncmp( Input_Message_String  + offset_IP_message_received ,
		      acknowledge_prefix , 
		      strlen( acknowledge_prefix ) ) == 0 ) {
	if( receive_ID) {
	  sscanf( Input_Message_String  + offset_IP_message_received 
		  + strlen( acknowledge_prefix ) , 
		  "%d" , &last_received_IP_message_number );
	}
	else {
	  last_received_IP_message_number = 0;
	}
	if( IP_message_trace ) {
	  printf("udp server: acknowledge message #%d [%s]\n", 
		 last_received_IP_message_number , Input_Message_String
		 + offset_IP_message_received );   
	}
      }
      else if( strncmp( Input_Message_String + offset_IP_message_received ,
			receive_prefix , 
			strlen( receive_prefix ) ) == 0 ) {
	if( IP_message_trace ) {
	  printf("udp server: receive plain message [%s]\n", 
		 Input_Message_String + offset_IP_message_received );   
	}
	storeIP_input_messages_and_removes_duplicates( 
			     Input_Message_String + strlen( receive_prefix ) );
      }
    }

    ///////////////////////////////////////////////////////
    // OSC format: bundle

    else if( receive_format == OSC && *Input_Message_String == '#' ) {
      int length;

      if( n <= 20 ) {
	sprintf( ErrorStr , "Error: incorrect OSC format of UDP message (incorrect initial length %d)! Check input format in configuration file!", n ); ReportError( ErrorStr ); 
	printf("udp server: [%s] \n", Input_Message_String);
	for( int ind = 0 ; ind < n ; ind++ ) {
	  printf( "%c(%d) " , Input_Message_String[ind] , 
		  (int)Input_Message_String[ind] );
	}
	printf( "\n" );
	return;
      }

      length =  (int) *((unsigned char *)(Input_Message_String + 20 - 1));
      // printf("\nudp server: message length total %d used %d\n", n , length);
      // //print received message
//       printf("udp server: " );
//       for( int ind = 0 ; ind < n ; ind++ ) {
// 	printf( "%d: [%c] (%d)\n" , ind , Input_Message_String[ind] , (int)Input_Message_String[ind] );
//       }
      

      if( length > n ) {
	sprintf( ErrorStr , "Error: incorrect OSC format of UDP message (incorrect message length)! Check input format in configuration file!" ); ReportError( ErrorStr ); 
	printf("udp server: [%s] \n", Input_Message_String);
	for( int ind = 0 ; ind < n ; ind++ ) {
	  printf( "%d: [%c] (%d)\n" , ind , Input_Message_String[ind] , (int)Input_Message_String[ind] );
	}
	return;
      }

      *(Input_Message_String + 20 + length) = 0;
      // printf("udp server: trailing message: %s\n", 
      // 	     Input_Message_String  + 20 + offset_IP_message_received );
      // message acknowledgement
      if( IP_message_acknowledgement
	  && *acknowledge_prefix 
	  && strncmp( Input_Message_String  + 20 + offset_IP_message_received ,
		      acknowledge_prefix , 
		      strlen( acknowledge_prefix ) ) == 0 ) {
	if( receive_ID) {
	  sscanf( Input_Message_String + 20 + offset_IP_message_received 
		  + strlen( acknowledge_prefix )  ,
		  "%d" , &last_received_IP_message_number );
	}
	else {
	  last_received_IP_message_number = 0;
	}
	if( IP_message_trace ) {
	  printf("udp server: acknowledge message #%d [%s]\n", 
		 last_received_IP_message_number , Input_Message_String + 20 ); 
	} 
      } 
      // receive prefix
      else if( *receive_prefix 
	       && strncmp( Input_Message_String + 20 + offset_IP_message_received ,
			receive_prefix , 
			strlen( receive_prefix ) ) == 0 ) {
	if( IP_message_trace ) {
	  printf("udp server: receive bundle message [%s]\n", 
		 Input_Message_String  + 20 + offset_IP_message_received ); 
	  printf("store message [%s]\n", 
		 Input_Message_String  + 20 + offset_IP_message_received 
		 + strlen( receive_prefix )); 
	}  
	storeIP_input_messages_and_removes_duplicates( 
		      Input_Message_String  + 20 + offset_IP_message_received 
			       + strlen( receive_prefix ) );
	// 	printf("store message [%s]\n", 
	// 	       Input_Message_String  + 20 + offset_IP_message_received ); 
      }
      // standard IP message
      else {
	if( *(Input_Message_String  + 20 + offset_IP_message_received) == '/' ) {
	  processReceivedOSC( Input_Message_Local_Commande_String , 
		      Input_Message_String  + 20 + offset_IP_message_received ,
			      length );
	  
	  if( IP_message_trace ) {
	    printf( "store message [%s]\n", Input_Message_Local_Commande_String ); 
	  }  
	  storeIP_input_messages_and_removes_duplicates( Input_Message_Local_Commande_String );
	}
	else {
	  sprintf( ErrorStr , "Error: UDP input message expected in OSC format (%s)!" , Input_Message_String  + 20 + offset_IP_message_received ); ReportError( ErrorStr ); 
	}
      }
    }

    ///////////////////////////////////////////////////////
    // OSC format: message

    else if( receive_format == OSC && *Input_Message_String == '/' ) {
      // acknowledgement and prefix removal are not
      // available in this format that contains no prefix
      // if( IP_message_trace ) {
      // print received message
      // printf("processReceivedOSC: " );
// 	printf("udp server: " );
// 	for( int ind = 0 ; ind < n ; ind++ ) {
// 	  printf( "%c(%d) " , Input_Message_String[ind] , (int)Input_Message_String[ind] );
// 	}
// 	printf( "\n" );
      // }  

      // OSC to string conversion for uniform processing of messages
      processReceivedOSC( Input_Message_Local_Commande_String , Input_Message_String , n );
	
      if( IP_message_trace ) {
	printf( "store message [%s]\n", Input_Message_Local_Commande_String ); 
      }  
      storeIP_input_messages_and_removes_duplicates( Input_Message_Local_Commande_String );
    }


    memset(Input_Message_String,0x0,
	   DM_EnvironmentNode->Max_network_message_length);
    n = receiveOneMessage( Input_Message_String );
    //     n = recvfrom(SocketToLocalServer, message, XMLAttrValLength, 0, 
    // 		 (struct sockaddr *) &clientAdddress, &clientLength);  
  }

  // forwards the non duplicated messages to the graphic system
  ProcessFilteredInputMessages();
}

void dm_IPServer::storeIP_input_messages_and_removes_duplicates( 
		      char *message ) {
  if( SocketToLocalServer < 0 ) {
    return;
  }

  // reads beyond ID (a unique number that identifies the message
  char *messString = message;
  if( receive_ID && isdigit( *message ) ) {
    messString = strchr( message , ' ' );
    while( *messString ==  ' ' ) {
      messString++;
    }
    if( !(*messString) ) {
      return;
    }
  }
  else {
    messString = message;
  }

  // tag length: the non blank chars at the beginning of the message
  int tagLength = strlen( messString );
  char *pch;
  if( (pch = strchr( messString ,  ' ' ) ) ) {
    tagLength = pch -  messString;
  }
    
  //printf( "new message [%s] current_depth_input_stack %d\n" , messString ,
  //	  current_depth_input_stack );

  // duplicates can only be removed for OSC message formats
  // checks whether it has a duplicate and replaces it if there is one
  if( *messString == '/' && *(messString + 1) != '_' && OSC_duplicate_removal ) {
    for( int ind_mess = 0 ; ind_mess < current_depth_input_stack ; ind_mess++ ) {
      // same tag
      if( input_message_length_stack[ ind_mess ] == tagLength 
	  && strncmp( input_message_stack[ ind_mess ] ,
		      messString , tagLength ) == 0 ) {
	//printf( "duplicate removed [%s] for [%s]\n" ,  input_message_stack[ ind_mess ] , messString );
	StrCpy( &input_message_stack[ ind_mess ] , messString );
	return;
      }
    }
  }
    
  // input message stack storing if no duplicate found
  if( current_depth_input_stack < depth_input_stack - 1 ) {
    StrCpy( &input_message_stack[ current_depth_input_stack ] , messString );
    input_message_length_stack[ current_depth_input_stack ] = tagLength;
    current_depth_input_stack++;
  }
  else {
    sprintf( ErrorStr , "Error: UDP input message stack overflow %d %d!" , current_depth_input_stack , depth_input_stack ); ReportError( ErrorStr ); 
    // for( int ind_mess = 0 ; ind_mess < depth_input_stack - 1  ; ind_mess++ ) {
    //   printf( "message [%s]\n" ,  input_message_stack[ ind_mess ] );
    // }
  }
}

void dm_IPServer::ProcessFilteredInputMessages( void ) {
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
    // message is 
    // <send_message value="stop scale"/> <target type="single_node" value="#moving sphere 1 " />
//     else 

    int         indChar = 0;
    int         p_c = ' ';

    ////////////////////////
    // parsing the associated command
    SpaceCommentsInclude(NULL,&p_c,messString,&indChar);

    ///////////////////////////////////////
    // OLD SYTLE: XML element is transfered thru the network
    if( p_c == '<' ) {
      dm_XMLTag        *XMLtag = new dm_XMLTag();
      dm_ElemAction     parsedAction;

      // allocates the target nodes
      parsedAction.clean();
      //printf( "dm_Action (%s)\n" , messString );

      // new tag XML 
      // 	ParseTagXML( NULL , &p_c , tag , attributes , values ,
      // 		     &nbAttributes , &tagType , true , messString , &indChar );
      if( !parsedAction.ParseElemAction( NULL , &p_c , 
					  XMLtag ,
					  messString , &indChar ,
					  EmptyEventType , (char *)"" , NULL ) ) {
	sprintf( ErrorStr , "Error: incorrect external message syntax!" ); ReportError( ErrorStr ) ; break;
      }
      else {
	//parsedAction.print( stdout , 1 , 1 );
	parsedAction.execute( dynamicReferenceResolution , 
			      false , messString , NULL );	  
      }
      parsedAction.deleteAllocatedComponents();
      delete XMLtag;
	  XMLtag = NULL;
    }
    // OLD SYTLE: XML element is transfered thru the network
    ///////////////////////////////////////
    ///////////////////////////////////////
    // NEW SYTLE: OSC message format
    else if( p_c == '/' ) {
      // ignores leading /
      messString += 1;

      int          tagLength = strlen( messString );
      char         *pch;

      // removes string termination: ; + \n
      if( (pch = strchr( messString ,  ';' ) ) ) {
	*pch = '\0';
      }

      // computes the length of the message tag
      if( (pch = strchr( messString ,  ' ' ) ) ) {
	tagLength = pch -  messString;
      }
      else {
	tagLength = strlen( messString );
      }

      // compares message tag with known command tags
      bool           commandExecuted = false;
      dm_Command    *commandsAliases = DM_CompositionNode->aliasCommand;
      while( commandsAliases && !commandExecuted ) {
	// printf( "message ID [%s] command [%s] length [%d]\n" , messString , commandsAliases->CommandAliasOSCTag , tagLength );
	if( strlen( commandsAliases->CommandAliasOSCTag ) 
	                            == (unsigned int)tagLength
	    && strncmp( messString , 
			commandsAliases->CommandAliasOSCTag , 
			tagLength ) == 0 ) {
	  if( pch ) {
	    commandsAliases->StoresOSCParameters( pch );

	    // stores parameters on the composition node
	    DM_CompositionNode->curentNbOSCParams 
	      = commandsAliases->commandNbOSCParams;
	    DM_CompositionNode->curentOSCParams 
	      = commandsAliases->commandOSCParams;

	    // there is no need to make a copy and the command
	    // parameters can be directly used because no recursion
	    // is possible for the alias commands contrary to 
	    // internal commands
	  }

	  // printf( "execute\n" );
	  // commandsAliases->print(stdout,true,1);
	  dm_Command *synchronous = NULL;
	  // printf( "initialization command\n" );
	  commandsAliases->execute( staticReferenceResolution , false ,
				    messString );
	  synchronous = commandsAliases->synchronousCommand;
	  while( synchronous ) {
	    synchronous->execute( staticReferenceResolution , false ,
				  messString );
	    synchronous = synchronous->synchronousCommand;
	  }
	  commandExecuted = true;
	}
	commandsAliases = commandsAliases->nextCommand;
      }
    }
    else {
	sprintf( ErrorStr , "Error: incorrect external message syntax!" ); ReportError( ErrorStr ) ; break;
    }
  }      
}

void dm_IPServer::operator=(dm_IPServer& s) {
  StrCpy( &id , s.id );
  Local_server_port = s.Local_server_port;
  Remote_client_port = s.Remote_client_port;
  StrCpy( &Remote_client_IP , s.Remote_client_IP);
  Remote_client_host = s.Remote_client_host;

  depth_input_stack = s.depth_input_stack;
  IP_message_acknowledgement = s.IP_message_acknowledgement;
  IP_message_trace = s.IP_message_trace;
  OSC_endian_reversal = s.OSC_endian_reversal;
  OSC_duplicate_removal = s.OSC_duplicate_removal;
  receive_format = s.receive_format;
  offset_IP_message_received = s.offset_IP_message_received;
  StrCpy( &receive_prefix , s.receive_prefix );
  receive_ID = s.receive_ID;
  StrCpy( &acknowledge_prefix , s.acknowledge_prefix );
  StrCpy( &carbon_copyID , s.carbon_copyID );
  carbon_copyIPClient = s.carbon_copyIPClient;
}

