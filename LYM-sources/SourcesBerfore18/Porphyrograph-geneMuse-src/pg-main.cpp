/*! \file pg-main.cpp
 * 
 *     Part of this source file is taken from Virtual Choreographer
 *     http://virchor.sourceforge.net/
 *
 *     File pg-main.cpp
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

//#define _CRTDBG_MAP_ALLOC

#include<stdlib.h>

#ifdef _VISUAL_STUDIO
// only for VC++
#include<crtdbg.h>
#endif

#include "pg-all_include.h"

#ifndef _WIN32
///  should only be included once
#define GLH_EXT_SINGLE_FILE
#endif

///////////////////////////////////////////////////////////////
// global variables
///////////////////////////////////////////////////////////////
/// pg_Environment node and values
pg_Environment * PG_EnvironmentNode = NULL;


/*! \name Group current_window_user
 *  \brief current window & user TEMPORARY
 */
// \{
pg_Window     *CurrentWindow = NULL;
// \}

int singleWindowWidth = 1024;
int doubleWindowWidth = 2048;
int windowHeight = 768;

/// screen message drawing
char ScreenMessage[XMLAttrValLength];
int ScreenMessageX = 0;
int ScreenMessageY = 0;
GLfloat messageTransparency;
bool NewScreenMessage = false;
bool ScreenMessageInitialized = false;

/// Error string
char *ErrorStr = NULL;

// Input network message string
char *Input_Message_String = NULL;
char *Input_Message_Local_Commande_String = NULL;
char *Output_Message_String = NULL;
char *Output_Message_OSC = NULL;
char *Output_Message_Pattern = NULL;
char *Output_Message_ArgList = NULL;

/// Input buffer for reporting recent input on error
char *InputBuffer = NULL;
int  InputBufferCurrentBegin = 0;
int  InputBufferCurrentEnd = 0;
// \}

/// unambiguous number for symbol generation
int GenSym = 0;

/// OpenGL background color
float OpenGLBGColor[4] = {0.0, 0.0, 0.0, 1.0};

/*! \name Group pressedButton
 *  \brief currently pressed button
 */
// \{
MouseButtonLocation CurrentButton = PG_BUTTON_EMPTY;
const char *MouseButtonLocationString[PG_BUTTON_EMPTY + 1] = { "left_button" , "middle_button" , "right_button" , "wheel_up" , "wheel_down" , "EmptyMouseButtonLocation" };
// \}    

/// number of frames displayed
int Step = 0;
/// current internal time for animations
double CurrentClockTime = 0.0;
/// initial time (in case of real timescale)
double InitialRealTime = 0.0;
/// last frame wall time
double LastFrameTime = 0;
double LastDecayTime = 0;
int LastFrame = 0;
int FramePerSecond = 0;
bool DisplayFramePerSecond = false;

/// current mouse location
int CurrentMousePos_x = 0, CurrentMousePos_y = 0;
int CurrentCursorHooverPos_x, CurrentCursorHooverPos_y;
// current tablet pen pressure and orientation
float tabletPressureRadius = 0.0f;
float tabletAzimutRadius = 0.0f;
float tabletInclinationRadius = 0.0f;

/// motion while button is down
float LinearMotionStep = 0;

/// log file
FILE    *fileLog;

/// XML files
FILE    *fileXML[ PG_NB_MAX_EMBEDDED_XML_FILES ];
int     lastChar[ PG_NB_MAX_EMBEDDED_XML_FILES ];

/// image number 
int      FrameNo;

/*! \name Group flag_gl_scene_initialization
 *  \brief flag for GL and scene initialization 
 * 
 * flag for GL and scene initialization 
 */
// \{
bool glut_initialized = false;
// \}

int argc; char **argv;

/// MAIN FUNCTION
int main(int argcMain, char **argvMain) {
  argc = argcMain;
  argv = argvMain;

  // error message and input buffer memory
  ErrorStr = new char[1024];

  // cyclic input buffer
  InputBuffer = new char[InputBufferSize];
  InputBufferCurrentBegin = 0;
  InputBufferCurrentEnd = 1;
  InputBuffer[ 0 ] = '\n';
 
  // loading the configuration file
  // the configuration file name must be terminated by ".conf"
  // possibly the first argument with such an extension
  int configurationFileLoaded = false;
  for( int ind = 0 ; ind < argc - 1 ; ind++ ) {
    if( strstr( argv[ind + 1] , ".conf" ) ) {
      char *substr = strstr( argv[ind + 1] , ".conf" );

      if( strcmp( substr , ".conf" ) == 0 ) {
	LoadConfigurationFile( argv[ind + 1] );
	configurationFileLoaded = true;
	break;
      }
    }
  }
  printf("Configuration file loaded: nb tracks %d\n", PG_NB_TRACKS );
  // or the default conf file name
  if( !configurationFileLoaded ) {
    LoadConfigurationFile( "PG.conf" );
  }

  ///////////////////////////////////////////////
  // GLUT display library

  // printf( "Glut initialization\n" );
  // glut parameters initialization 
  glutInit(&argc, argv);

  // scene initialization
  pg_init_scene();

  // glut initialization
  initGlutWindows();

  // glut win activation
  CurrentWindow = PG_EnvironmentNode->PG_Window;
  glutSetWindow( CurrentWindow->glutID );

  // OpenGL initialization
  OpenGLInit();
  // cursor shape selection
  CursorInit();

  // window resize
  window_reshape( CurrentWindow->getWindowWidth() ,
		  CurrentWindow->getWindowHeight() );
  CurrentWindow->windowNonFullScreenWidth = doubleWindowWidth;
  CurrentWindow->windowNonFullScreenHeight = windowHeight;
  
  if( PG_EnvironmentNode->full_screen ) {
    glutFullScreen();
  }
  
  // printf("Initialization completed %d\n", PG_NB_TRACKS);
  
  // initializations before rendering

  // matrices, geometry, shaders and FBOs
  pg_initRenderingMatrices();
  printOglError( 31 );
  // buiilds the quads to be rendered
  pg_initGeometry_quads();
  printOglError( 32 );
  // loads the shaders
  pg_loadAllShaders();
  printOglError( 33 );
  // initializes the FBOS
  pg_initFBO();
  printOglError( 34 );

  // initializes the tracks for recording the strokes
  pg_initStrokes();
  printOglError( 35 );
  // texture initialization
  pg_initTextures();
  printOglError( 36 );

  // initialization of variables that were made in the
  // previous shader definititions and initialization script
  pg_initializationScript();
  pg_trackVariablesInit();

  // main loop for event processing and display
  glutMainLoop();

  return 0;

  // end of glut display library
  ///////////////////////////////////////////////

}

void initGlutWindows( void ) {
//   printf("-----------------------------------------------------------\n");
//   printf("< & > to control the update pace.\n");
//   printf("+ & - to shift between viewpoints.\n");
//   printf("Up & Down (or left ctrl/shift click) to control the height.\n");
//   printf("Left & Right (or left/shift click) for direction and move.\n");
//   printf("p for printing the scene.\n");
//   printf("-----------------------------------------------------------\n");

  glutInitWindowSize( PG_EnvironmentNode->PG_Window->getWindowWidth(),
		      PG_EnvironmentNode->PG_Window->getWindowHeight() );
  glutInitWindowPosition( PG_EnvironmentNode->PG_Window->getWindow_x(),
			  PG_EnvironmentNode->PG_Window->getWindow_y() );
  
  glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_BORDERLESS ); // 
  // glutInitDisplayString("rgb stencil~4 double samples~8");
  
  printf( "Create Window %dx%d %s\n" , PG_EnvironmentNode->PG_Window->getWindowWidth() , PG_EnvironmentNode->PG_Window->getWindowHeight() , PG_EnvironmentNode->PG_Window->id );
  if( PG_EnvironmentNode->game_mode ) {
    char sizeString[128];
    sprintf( sizeString , "%dx%d" , PG_EnvironmentNode->PG_Window->getWindowWidth() ,PG_EnvironmentNode->PG_Window->getWindowHeight() );
    glutGameModeString( sizeString );
    PG_EnvironmentNode->PG_Window->glutID = glutEnterGameMode();
	printf("Game Mode %dx%d at %d hertz\n",
			glutGameModeGet(GLUT_GAME_MODE_WIDTH),
			glutGameModeGet(GLUT_GAME_MODE_HEIGHT),
			glutGameModeGet(GLUT_GAME_MODE_REFRESH_RATE));
    // printf( "Gone into game mode\n" );
	// glutLeaveFullScreen();
  }
  else {
    PG_EnvironmentNode->PG_Window->glutID
      = glutCreateWindow(PG_EnvironmentNode->PG_Window->id);
  }

  if( PG_EnvironmentNode->double_window ) {
    singleWindowWidth = PG_EnvironmentNode->PG_Window->getWindowWidth() / 2;
    doubleWindowWidth = PG_EnvironmentNode->PG_Window->getWindowWidth();
  }
  else {
    singleWindowWidth = PG_EnvironmentNode->PG_Window->getWindowWidth();
    doubleWindowWidth = PG_EnvironmentNode->PG_Window->getWindowWidth();
  }
  windowHeight = PG_EnvironmentNode->PG_Window->getWindowHeight();

  ///////////////////////////////////////////////
  // OpenGL extended
  GLenum err = glewInit();  
  if (GLEW_OK != err) {
    /* Problem: glewInit failed, something is seriously wrong. */
    fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
  }
  fprintf(stdout, "GLEW version %s\n", glewGetString(GLEW_VERSION));

  // get version info
  const GLubyte* renderer = glGetString (GL_RENDERER); // get renderer string
  const GLubyte* version = glGetString (GL_VERSION); // version as a string
  printf ("Renderer: %s\n", renderer);
  printf ("OpenGL version supported %s\n", version);

  int maxTextureSize;
  glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
  printf ("Max texture size %d\n", maxTextureSize);

  // window selection
  // glut win activation
  CurrentWindow = PG_EnvironmentNode->PG_Window;
  glutSetWindow( CurrentWindow->glutID );
  
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClearColor(0.0,0.0,0.0,1.0);
  
  // callback procedures for window reshape
  glutReshapeFunc(&window_reshape);
  // keyboard events handling
  glutKeyboardFunc(&window_key_browse);
  // special keys handling
  glutSpecialFunc(&window_special_key_browse);

#ifdef WACOM_TABLET
  // wacom tablet handling
  glutWacomTabletFunc(&window_wacom_tablet_browse);
#else  // mouse clicks handling
  glutMouseFunc(&window_mouseFunc_browse);
  // mouse drags handling
  glutMotionFunc(&window_motionFunc_browse);
  // passive selection
  //if( PG_EnvironmentNode->PG_Window->mouse_passive_selection ) {
  glutPassiveMotionFunc(&window_passiveMotionFunc_browse);
#endif
  
  // window visibility
  glutVisibilityFunc(NULL);
  // idle update
  glutTimerFunc((int)PG_EnvironmentNode->minimal_interframe_latency,
		&window_idle_browse,Step);
  glutDisplayFunc(&window_display);

}

void  pg_init_screen_message( void ) {
  // screen message initialization
  strcpy( ScreenMessage , "" );
  ScreenMessageX = 0;
  ScreenMessageY = 0;
  messageTransparency = 1.0;
  NewScreenMessage = false;
  ScreenMessageInitialized = false;
  LastDecayTime = CurrentClockTime;
}

void OpenGLInit( void ) {
  // background color
  glClearColor (OpenGLBGColor[0], OpenGLBGColor[1], OpenGLBGColor[2], OpenGLBGColor[3]);

  // buffer reset
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  // no z-Buffer
  glDisable(GL_DEPTH_TEST);
  // // smooth shading
  // glShadeModel(GL_SMOOTH);
  // // normal vector normalization
  // glEnable( GL_NORMALIZE );

  // disable vsync
  // float r = 

}

void CursorInit( void ) {
  // Incidents memorables
  if( PG_EnvironmentNode->cursorShape == NoCursor ) {
    glutSetCursor(GLUT_CURSOR_NONE);
  }
  else if( PG_EnvironmentNode->cursorShape == LeftArrowCursor ) {
    glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
  }
  else if( PG_EnvironmentNode->cursorShape == RightArrowCursor ) {
    glutSetCursor(GLUT_CURSOR_RIGHT_ARROW);
  }
}

void pg_init_scene( void ) {
  if( argc <= 1 ) {
      sprintf( ErrorStr , "PG: not enough arguments" ); ReportError( ErrorStr );
      sprintf( ErrorStr , "Usage: PG [configuration.conf] (3Dscene.xml)+" ); ReportError( ErrorStr ); throw 100;
  }

  // log file
  if( *(PG_EnvironmentNode->log_file_name) ) {
    printf( "Opening log file %s\n" , PG_EnvironmentNode->log_file_name ); 
    if ( ( fileLog = fopen(PG_EnvironmentNode->log_file_name , "wb" )) == NULL ) {
      sprintf( ErrorStr , "File %s not opened!" , PG_EnvironmentNode->log_file_name ); ReportError( ErrorStr ); throw 152;
    }
  }
  else {
    fileLog = stdout;
  }

  ///////////////////////////////////////////////
  // Version number
  fprintf( fileLog , "PG version 1.1\n");
  printf("PG version 1.1\n");

  // ------ input buffer initialization  -------- //
  // input buffer is emptied
  // so that it is not reported in dynamic errors
  InputBufferCurrentBegin = 0;
  InputBufferCurrentEnd = 1;
  InputBuffer[ 0 ] = 0;
  InputBuffer[ 1 ] = 0;

  // ------ global variables initialization  ------------- //
  // frame number initialization
  FrameNo = PG_EnvironmentNode->first_frame_number - 1;

  // intial real time
  InitialRealTime = RealTime() + PG_EnvironmentNode->initial_time;

  // initial scenario time: well before current time to be over
  InitialScenarioTime = InitialRealTime - 1000000;

  // current time initialization: internal time
  CurrentClockTime = RealTime();

  printf( "Initial time %.2f (real time %.5f)\n" , CurrentClockTime , InitialRealTime );

  // random seed initialization
  srand( clock() );

  // ------ screen message initialization  ------------- //
  pg_init_screen_message();


#ifdef WIN32
  WSADATA wsaData;
  // Initialize Winsock
  int err = WSAStartup(MAKEWORD(2,2), &wsaData);
  if ( err != 0 ) {
    /* Tell the user that we could not find a usable */
    /* WinSock DLL.                                  */
    sprintf( ErrorStr , "could not find a usable WinSock DLL!" ); ReportError( ErrorStr ); throw 207;
  }
#endif

  // server initialization
  for( int ind = 0 ; ind < PG_EnvironmentNode->nb_IP_Servers ; ind++ ) {
    PG_EnvironmentNode->IP_Servers[ ind ]->Init();
  }
  for( int ind = 0 ; ind < PG_EnvironmentNode->nb_IP_Clients ; ind++ ) {
    // printf( "Client %d initialization\n" , ind );
      PG_EnvironmentNode->IP_Clients[ ind ]->Init();
      // printf( "Client name %s\n" , PG_EnvironmentNode->IP_Clients[ ind ]->id );
  }
  Input_Message_String 
    = new char[ PG_EnvironmentNode->Max_network_message_length ];
  Input_Message_Local_Commande_String 
    = new char[ PG_EnvironmentNode->Max_network_message_length ];
  Output_Message_String 
    = new char[ PG_EnvironmentNode->Max_network_message_length ];
  // the OSC string is made 2 times longer because
  // it contains both pattern and argts
  Output_Message_OSC
    = new char[ 2 * PG_EnvironmentNode->Max_network_message_length ];
  Output_Message_Pattern 
    = new char[ PG_EnvironmentNode->Max_network_message_length ];
  Output_Message_ArgList
    = new char[ PG_EnvironmentNode->Max_network_message_length ];
  // printf( "End of scene initialization\n" );
}

bool pg_shutdown = false;

void terminates_and_exits( void ) {
  // stores the shutdown status 
  // in order to avoid recalling itself and infinite recursion
  if( pg_shutdown ) {
    return;
  }
  else {
    pg_shutdown = true;
  }

  printf( "Last Frame #%d\n" , FrameNo );

  if( fileLog != stdout ) {
    fclose( fileLog );
  }


  // release global parameters
  // release the common variables in PG_EnvironmentNode. (Must be relased before releaseSceneObjects()
  // printf("releasewindows\n");
  delete PG_EnvironmentNode->PG_Window;
  PG_EnvironmentNode->PG_Window = NULL;

  // printf("releaseUDP\n");
  if (PG_EnvironmentNode->IP_Servers) {
    for (int ind = 0; ind < PG_EnvironmentNode->nb_IP_Servers; ind++) {
      delete PG_EnvironmentNode->IP_Servers[ind];
      PG_EnvironmentNode->IP_Servers[ind] = NULL;
    }
    
    delete [] PG_EnvironmentNode->IP_Servers;
    PG_EnvironmentNode->IP_Servers = NULL;
  }
  
  if (PG_EnvironmentNode->IP_Clients) {
    for (int ind = 0; ind < PG_EnvironmentNode->nb_IP_Clients; ind++) {
      delete PG_EnvironmentNode->IP_Clients[ind];
      PG_EnvironmentNode->IP_Clients[ind] = NULL;
    }
    
    delete [] PG_EnvironmentNode->IP_Clients;
    PG_EnvironmentNode->IP_Clients = NULL;
  }
  
  if (ErrorStr) {
    delete [] ErrorStr;
    ErrorStr = NULL;
  }

  if (InputBuffer) {
    delete [] InputBuffer;
    InputBuffer = NULL;
  }
  
  exit(1);
}

void window_key_browse(unsigned char key, int x, int y)
{
  switch (key) {
  case KEY_ESC:
    terminates_and_exits();
    break;
  // default: use standard key processing (non-platform dependent)
  default:
    pg_process_key( (int)key );
    break;
  }
}

void window_special_key_browse(int key, int x, int y)
{
  // use standard key processing (non-platform dependent)
  pg_process_special_key( key );
}

#ifdef WACOM_TABLET
void window_wacom_tablet_browse(int x, int y, float press, float az, float incl, int twist ) {
    if( press > 0 ) {
	CurrentMousePos_x = x;
	CurrentMousePos_y = y;
        CurrentCursorHooverPos_x = -10000;
        CurrentCursorHooverPos_y = -10000;
    }
    else {
	CurrentMousePos_x = -10000;
	CurrentMousePos_y = -10000;
        CurrentCursorHooverPos_x = x;
        CurrentCursorHooverPos_y = y;
    }
    // printf("pos = %d %d\n",CurrentMousePos_x,CurrentMousePos_y);
  
    tabletPressureRadius = press;
    tabletAzimutRadius = az;
    tabletInclinationRadius = incl;

  //printf("Porphyrograph pt [%d,%d] press %.3f az %.3f alt %.3f twist %d\n" , 
		//x, y, press, az, incl, twist );
}
#else
void window_mouseFunc_browse(int button, int state, int x, int y) {
  // printf( "click button (%d,%d)\n" , x , y );
  CurrentMousePos_x = x;
  CurrentMousePos_y = y;
  CurrentCursorHooverPos_x = x;
  CurrentCursorHooverPos_y = y;
}

void window_motionFunc_browse(int x, int y) {
  // printf( "active button (%d,%d)\n" , x , y );
  CurrentMousePos_x = x;
  CurrentMousePos_y = y;
  CurrentCursorHooverPos_x = x;
  CurrentCursorHooverPos_y = y;
}

void window_passiveMotionFunc_browse(int x, int y) {
  // printf( "passive button (%d,%d)\n" , x , y );
    CurrentMousePos_x = -10000;
    CurrentMousePos_y = -10000;
    CurrentCursorHooverPos_x = x;
    CurrentCursorHooverPos_y = y;
  // freeglut (PG): glutGetModifiers() called outside an input callback/int
}
#endif

void window_idle_browse( void ) {
  window_idle_browse( 0 );
}

void window_idle_browse( int step ) {
  // printf("begin window_idle_browse\n");
  // fprintf( fileLog, "%.10f begin Time #\n" , RealTime() );

  // MemoryUsage();

  // -------------------- idle function recall ------------------------- //
  glutTimerFunc( (int)PG_EnvironmentNode->minimal_interframe_latency ,
	 	 &window_idle_browse , Step );

  Step = step;

  // -------------------- scene update ------------------------- //
  // update is only made if at least window is displayed
  if( PG_EnvironmentNode->windowDisplayed ) {
    PG_EnvironmentNode->windowDisplayed = false;

    // frame count
    FrameNo++;

    // current clock time
    CurrentClockTime = RealTime();
    if( PG_EnvironmentNode->trace_time ) {
      printf( " Time #%.2f\n" , CurrentClockTime );
    }

    // scenario update
    pg_update_scenario();

    // records the current mouse position in the variable
    // and in the tracks is recording is active
    updateMouseEnvironmentVariablesAndTables( CurrentClockTime );

    // sending information to the GUI
    pg_displaySceneVariables();

    // FPS computation
    if( FrameNo % 100 == 0 && CurrentClockTime != LastFrameTime) {
      FramePerSecond = (int)((FrameNo - LastFrame)/(CurrentClockTime-LastFrameTime));
      LastFrame = FrameNo;
      LastFrameTime = CurrentClockTime;
      char mess[50];
      
      if( DisplayFramePerSecond  ) {
	sprintf( mess , "%d FPS" , FramePerSecond );
	pg_writeMessageOnScreen( (char *)mess );
	printf( "%s\n" , ScreenMessage );
      }
    }

    // printf( "transparency %.2f\n" , messageTransparency );

	
   // decays window message brightness
    if( messageTransparency > 0.0 ) {
      messageTransparency -= 0.4f*(float)(CurrentClockTime - LastDecayTime);
      LastDecayTime = CurrentClockTime;

      // printf( "Message opacity %.2f mess %s\n" , messageTransparency , ScreenMessage );
    }

    // if message brightness becomes null, the message is erased
    if( messageTransparency <= 0.0 ) {
      ScreenMessage[ 0 ] = 0;
      messageTransparency = 0.0;
      LastDecayTime = CurrentClockTime;
    }

    if( PG_EnvironmentNode->trace_output_frame_number ) {
      printf( "Frame #%d\n" , FrameNo );
    }
    // fprintf( fileLog , "Frame #%d\n" , FrameNo );
    // last image No display
    if( FrameNo > PG_EnvironmentNode->last_frame_number ) {
      printf( " Last Frame #%d\n" , PG_EnvironmentNode->last_frame_number );
      exit(0);
    }

    // beat in case of internal beat
    if( auto_beat && CurrentClockTime > lastBeatTime + BEAT_DURATION ) {
      float arguments[MAX_OSC_ARGUMENTS];
      memset(arguments, 0, MAX_OSC_ARGUMENTS * sizeof(float) ); // make sure the struct is empty
      arguments[ 0 ] = (float)BeatNo;
      BeatNo++;
      lastBeatTime += BEAT_DURATION;
      if( lastBeatTime > CurrentClockTime + BEAT_DURATION
	   || lastBeatTime < CurrentClockTime ) {
	     lastBeatTime = CurrentClockTime;
      }
      char msg[] = "beat";
      char arg[] = "";
      pg_aliasScript( msg , arg , arguments );
    }

    // fprintf( fileLog, "%.10f popEvents Time #\n" , RealTime() );
    // internal and external event processing

    for( int ind = 0 ; ind < PG_EnvironmentNode->nb_IP_Servers ; ind++ ) {
      PG_EnvironmentNode->IP_Servers[ ind ]->receiveIPMessages();
     }
    for( int ind = 0 ; ind < PG_EnvironmentNode->nb_IP_Clients ; ind++ ) {
      PG_EnvironmentNode->IP_Clients[ ind ]->sendIPmessages();
    }
  }

  // defines the global variables associated with the current window
  // CurrentWindow = PG_EnvironmentNode->PG_Window;
  // glutSetWindow( CurrentWindow->glutID );
  
  // -------------------- window redisplay ------------------------- //
  // fprintf( fileLog, "%.10f glutPostRedisplay Time #\n" , RealTime() );
  glutPostRedisplay();
    
  // fprintf( fileLog, "%.10f end Time #\n\n" , RealTime() );
  Step++;
  // printf("end window_idle_browse\n");
}

//////////////////////////////////
// window reshape
//////////////////////////////////
void window_reshape(GLsizei width, GLsizei height) {
  CurrentWindow = PG_EnvironmentNode->PG_Window;

  CurrentWindow->setWindowHeight( (float)height );
  CurrentWindow->setWindowWidth( (float)width );

  //  printf("reshape main win %d %d %d\n" , CurrentWindow->getWindowWidth() , CurrentWindow->getWindowHeight() , width , height );
  printf( "Resize Window %dx%d %s\n" , PG_EnvironmentNode->PG_Window->getWindowWidth() , PG_EnvironmentNode->PG_Window->getWindowHeight() , PG_EnvironmentNode->PG_Window->id );

  if( PG_EnvironmentNode->double_window ) {
    singleWindowWidth = PG_EnvironmentNode->PG_Window->getWindowWidth() / 2;
    doubleWindowWidth = PG_EnvironmentNode->PG_Window->getWindowWidth();
  }
  else {
    singleWindowWidth = PG_EnvironmentNode->PG_Window->getWindowWidth();
    doubleWindowWidth = PG_EnvironmentNode->PG_Window->getWindowWidth();
  }
  windowHeight = PG_EnvironmentNode->PG_Window->getWindowHeight();

  int glutWinID = glutGetWindow();
  if( glutWinID == PG_EnvironmentNode->PG_Window->glutID ) {
    CurrentWindow = PG_EnvironmentNode->PG_Window;
  }
  else {
    sprintf( ErrorStr , "Current window not found!" ); ReportError( ErrorStr ); throw 100;
  }
}


