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

#define MAX_OSC_ARGUMENTS 8

#define _Return(c) (c == '\n' || c == 13)
#define _SpaceChar(c) (c == ' ' || _Return(c) || c == '\t')
#define _Num(c) (c >= '0' && c <= '9')


void InitString( char **target );
void StrCpy( char **target , const char *source );

long     ReadIntegerString( int *p_c  ,
		      int p_withSpaceCommentsInclude , 
		      char *charstring , int *ind );
float    ReadFloatString( int *p_c  ,
		   int p_withSpaceCommentsInclude , 
		   char *charstring , int *ind );

/*! \name Group IP_constant
 *  \brief constants for IP send/receive
 */
// \{
enum ExternalMessageFormat{ Plain = 0, OSC , EmptyExternalMessageFormat };
extern const char *ExternalMessageFormatString[EmptyExternalMessageFormat + 1];
// \}

/// IP client (remote server)
class pg_IPClient {
 public:
  /// remote server ID
  string id;

  /*! \name Group remote_serv_addr
   *  \brief remote server address and local client port
   */
  // \{
  string              Remote_server_IP;
  unsigned int        Remote_server_port;
  struct hostent     *Remote_server_host;
  // \}

  // connection to remote server
  bool          connected;

  /*! \name Group remote_serv_sock
   *  \brief remote server socket
   */
  // \{
  int SocketToRemoteServer;
  struct sockaddr_in remoteServAddr;
  // \}

  /*! \name Group client_stack
   *  \brief client stack: stores pending output messages
   */
  // \{
  // OSC pattern
  char  **output_pattern_stack;
  // the message: string (Plain format), OSC encoding (OSC format)
  char  **output_message_stack;

  int     depth_output_stack;
  int     current_depth_output_stack;
  float   maximal_IP_message_delay;
  double  last_IP_message_time;
  // \}

  /*! \name Group output_mess
   *  \brief output message format
   */
  // \{
  ExternalMessageFormat   send_format;
  // \}

  unsigned int first_IP_message_number;
  unsigned int current_IP_message_number;
  // \}

  /// console trace
  bool   IP_message_trace;

  /// byte order reversal
  bool   OSC_endian_reversal;

  pg_IPClient( void );
  ~pg_IPClient( void );

  void InitClient( void );

  void IP_OutputStackInitialization( void );

  /// send IP messages to IP client (Plain or OSC formats)
  void sendIPmessages( void );

  /// message processing
  void storeIP_output_message( char *commandLine ,
				char *pattern );
};

/// IP server (local server)
class pg_IPServer {
 public:
  /// local server ID
  string       id;

  // OSC argument parsing
  char *OSC_arguments[MAX_OSC_ARGUMENTS];
  char *OSCTag;

  /// local server socket
  int                 SocketToLocalServer;
  struct sockaddr_in  localServerAddr;

  /*! \name Group remote_serv_sock
   *  \brief remote server socket
   */
  // \{
  int SocketToRemoteClient;
  struct sockaddr_in remoteClientAddr;
  // \}

  /*! \name Group remote_serv_addr
   *  \brief remote client address and local server port
   */
  // \{
  unsigned int        Local_server_port;
  char               *Remote_client_IP;
  unsigned int        Remote_client_port;
  struct hostent     *Remote_client_host;
  // \}

  /*! \name Group server_stack
   *  \brief server stack: stores received input messages
   */
  // \{
  char          **input_message_stack;
  int            *input_message_length_stack;
  int             depth_input_stack;
  int             current_depth_input_stack;
  // \}

  /*! \name Group received_mess
   *  \brief received message format
   */
  // \{
  ExternalMessageFormat   receive_format;
  char           *receive_prefix;
  int             offset_IP_message_received;
  // \}

  /// labelling of input messages
  bool            receive_ID;

  /*! \name Group ack_received_mess
   *  \brief acknowledgement of received messages
   */
  // \{
  unsigned int    last_received_IP_message_number;
  // \}

  /// console trace
  bool            IP_message_trace;

  /// byte order reversal
  bool            OSC_endian_reversal;

  /// duplicate OSC message removal
  bool            OSC_duplicate_removal;

  pg_IPServer( void );
  ~pg_IPServer( void );

  void InitServer( void );

  void IP_InputStackInitialization( void );

  /*! 
   * \brief receive IP messages
   * \param localCommandLine				to be added
   * \param message								to be added
   * \param n													to be added
   *
   * receive IP messages as IP server
   * filters out acknowledgement messages
   * stores graphic messages before removing duplicates
   */
  void processReceivedOSC( char *localCommandLine , char *message , int n );

  /// message processing
  int receiveOneMessage( char *message );
  void receiveIPMessages( void );

  /*!
   * \brief stores IP messages
   * \param message			to be added
   *
   * stores IP messages before duplicate removal 
   * in case of duplicated messages (with possibly different 
   * numerical values), only the latest message is kept
   */
  void storeIP_input_messages_and_removes_duplicates( 
		       char *message );

  /// pass non duplicated IP messages to the visualization system
  void ProcessFilteredInputMessages( void );
};

#endif
