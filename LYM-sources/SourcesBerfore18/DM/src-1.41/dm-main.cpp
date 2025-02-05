/*! \file dm-main.cpp
 * 
 *     Part of this source file is taken from Virtual Choreographer
 *     http://virchor.sourceforge.net/
 *
 *     File dm-main.cpp
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

#include "dm-all_include.h"

#ifndef _WIN32

///  should only be included once
#define GLH_EXT_SINGLE_FILE
//#include <glh/glh_extensions.h>
// #include <glh/glh_glut.h>
#endif

///////////////////////////////////////////////////////////////
// global variables
///////////////////////////////////////////////////////////////
/// the link that stores all the scene object, it is used for decostruction
strAllSceneObject* AllSceneObject = NULL;

// the link that stores all the scripts
strAllScript *AllScript = NULL;

/// the node that carries the environment
dm_SceneNode   *envtNode = NULL;

/// dm_Environment node and values
dm_Environment * DM_EnvironmentNode = NULL;

/// stores all the node that will be deleted finally,
/// for simplication, only the daughter node is used as the link between neighboring node
dm_SceneNode *deletedNodeList = NULL;

/// stores all the scalars defined so that all of these scalars will be deleted finally when the system exits
dm_Scalar *deletedScalarList = NULL;

/// stores all the tables defined so that all of these tables will be deleted finally when the system exits
dm_Table *deletedTableList = NULL;

/// stores all the matries defined so that all of these matrices will be deleted finally when the system exits
dm_Matrix *deletedMatrixList = NULL;

/// stores all the texures defined so that all of these textures will be deleted finally when the system exits;
strAllTexture *AllTexture = NULL;

/*! \name Group current_window_user
 *  \brief current window & user TEMPORARY
 */
// \{
dm_Window     *CurrentWindow = NULL;
int            CurrentIndUserNode = -1;
dm_User       *CurrentUser = NULL;
int            CurrentIndViewPoint = 0;
dm_Viewpoint  *CurrentViewPoint = NULL;
dm_SceneNode  *CurrentViewPointNode = NULL;
dm_SceneNode  *WindowViewPointWithMouseDown = NULL;
float         *CurrentViewPointMatrix = NULL;
dm_Frustum    *CurrentFrustum = NULL;
dm_Ortho      *CurrentOrtho = NULL;
// \}

/*! \name Group Text_messeage_drawing
 *  \brief dm_Text message drawing
 */
// \{
char ScreenMessage[XMLAttrValLength];
int ScreenMessageX = 0;
int ScreenMessageY = 0;
GLfloat ScreenMessageColor = 0.0;
bool NewScreenMessage = false;
bool ScreenMessageInitialized = false;
// \}

/// Error string
char *ErrorStr = NULL;

// Input network message string
char *Input_Message_String = NULL;
char *Input_Message_Local_Commande_String = NULL;
char *Output_Message_String = NULL;
char *Output_Message_OSC = NULL;
char *Output_Message_Pattern = NULL;
char *Output_Message_ArgList = NULL;

/*! \name inputer_buffer_error_report
 *  \brief Input buffer for reporting recent input on error
 */
// \{
char *InputBuffer = NULL;
int  InputBufferCurrentBegin = 0;
int  InputBufferCurrentEnd = 0;
// \}

/// unambiguous number for symbol generation
int GenSym = 0;

/// OpenGL background color
float OpenGLBGColor[4] = {0.0, 0.0, 0.0, 1.0};

/// composition
dm_Composition  *DM_CompositionNode;

/*! \name Group pressedButton
 *  \brief currently pressed button
 */
// \{
MouseButtonLocation CurrentButton = DM_BUTTON_EMPTY;
const char *MouseButtonLocationString[DM_BUTTON_EMPTY + 1] = { "left_button" , "middle_button" , "right_button" , "wheel_up" , "wheel_down" , "EmptyMouseButtonLocation" };
// \}    

/*! \name Group current_height_spherical_coordinates
 *  \brief current height in spherical coordinates
 */
// \{
float CurrentHeight;
int depthTransformation;
// \}

/*! \name Group  rotation_horizontal_axis
 *  \brief rotation around an horizontal axis
 */
// \{
float spin_x = 0, spin_y = 90;
float transl_x = 0.0, transl_y = 0.0, transl_z = 0.0;
// \}

/// elementary moving steps
float ElementaryRotationAngle = (float)(M_PI/300.0);

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

/// motion while button is down
float LinearMotionStep = 0;

/// log file
FILE    *fileLog;

/// XML files
FILE    *fileXML[ DM_NB_MAX_EMBEDDED_XML_FILES ];
int     lastChar[ DM_NB_MAX_EMBEDDED_XML_FILES ];

/*! \name Group script_command_current_output
 *  \brief Script command for current frame output
 */
// \{
bool     WriteCurrentFrame = false;
int      WriteCurrentFrameIndex = -1;
// \}

/// image number 
int      FrameNo;

/// scene nodes that have already received a message through udp
dm_SceneNode **knownTargets;

/// the number of known targets
int     nbKnownTargets = 0;

/*! \name Group flag_gl_scene_initialization
 *  \brief flag for GL and scene initialization 
 * 
 * flag for GL and scene initialization 
 */
// \{
bool scene_initialized = false;
bool glut_initialized = false;
// \}

#ifndef _WIN32
int argc; char **argv;
#endif

/// MAIN FUNCTION
#ifdef _WIN32
int main(int argc, char **argv) {
#else
int main(int argcMain, char **argvMain) {
#endif
//	_CrtDumpMemoryLeaks();
#ifndef _WIN32
  argc = argcMain;
  argv = argvMain;
#endif

  // error message and input buffer memory
  ErrorStr = new char[1024];

  // cyclic input buffer
  InputBuffer = new char[InputBufferSize];
  InputBufferCurrentBegin = 0;
  InputBufferCurrentEnd = 1;
  InputBuffer[ 0 ] = '\n';
 
  // initial the scene object node that stores all the scene structure
  AllSceneObject = new strAllSceneObject;
  AllSceneObject->curObj = NULL;
  AllSceneObject->nextObj = NULL;

  // initial the script link that stores all the scripts
  AllScript = new strAllScript;
  AllScript->curtScript = NULL;
  AllScript->nextScript = NULL;

  // initial the texture link that stores all the textures;
  AllTexture = new strAllTexture;
  AllTexture->curTex = NULL;
  AllTexture->nextTex = NULL;


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
  // or the default conf file name
  if( !configurationFileLoaded ) {
    LoadConfigurationFile( "DM.conf" );
  }

  ///////////////////////////////////////////////
  // GLUT display library

  // printf( "Glut initialization\n" );
  // glut parameters initialization 
  glutInit(&argc, argv);

  // printf( "Scene initialization\n" );
  // flag for scene initialization
  if( !scene_initialized ) {
    // scene initialization
#ifdef _WIN32
    dm_init_scene(argc, argv);
#else
    dm_init_scene();
#endif
    scene_initialized  = true;
  }
  
  // printf( "Glut windows initialization\n" );
  // flag for glut initialization
  if( !glut_initialized ) {
    // glut initialization
    initGlutWindows();
    
    glut_initialized = true;
  }
  
  // printf( "Glut windows selecion\n" );

  // glut win activation
  SetGlutWindow();

  // OpenGL initialization
  OpenGLInit();
  // cursor shape selection
  CursorInit();
  
  window_reshape( CurrentWindow->getWindowWidth() ,
		  CurrentWindow->getWindowHeight() );
  CurrentWindow->windowNonFullScreenWidth 
    = CurrentWindow->getWindowWidth();
  CurrentWindow->windowNonFullScreenHeight 
    = CurrentWindow->getWindowHeight();
  
  if( DM_EnvironmentNode->full_screen ) {
    glutFullScreen();
  }
  
  if( DM_EnvironmentNode->font_file_name ) {
    printf( "Font initialization %s\n" , DM_EnvironmentNode->font_file_name );
    bool newTex = true;
    DM_EnvironmentNode->font_tex_no 
      = DM_CompositionNode->FindTexture( DM_EnvironmentNode->font_file_name ,
					 &DM_EnvironmentNode->font_texture_encoding ,
					 2 ,
					 true ,
					 DM_DIRECT_TEXTURING , 
					 &newTex ,
					 false );
    if( newTex ) {
      DM_CompositionNode->LoadTextureInGPUMemory( DM_EnvironmentNode->font_tex_no ,
						  1 ,
						  1 , 
						  DM_EnvironmentNode->font_texture_encoding ,
						  DM_BITMAP_TEXTURE ,
						  false , 
						  true , 
						  byte_tex_format ,
						  linear_filter ,
						  linear_filter ,
						  GL_TEXTURE_2D );
    }
    DM_EnvironmentNode->message_tex_no 
      = DM_CompositionNode->GenerateTexture( byte_tex_format ,
					     1 ,
					     DM_EnvironmentNode->message_pixel_length ,
					     1 );

    // texture will be freed at shutdown
    // DM_CompositionNode->FreeTextureInRAM( DM_EnvironmentNode->font_texture_encoding , 
    // 					  DM_EnvironmentNode->font_tex_no , 1 );

    // printf( "End of font initialization\n" );
  }
  
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

  // windows must be built in 2 steps: first main windows 
  int nbInitializedWindows = 0;
  nbInitializedWindows++;
  
  glutInitWindowSize( DM_EnvironmentNode->DM_Window->getWindowWidth(),
		      DM_EnvironmentNode->DM_Window->getWindowHeight() );
  glutInitWindowPosition( DM_EnvironmentNode->DM_Window->getWindow_x(),
			  DM_EnvironmentNode->DM_Window->getWindow_y() );
  
  glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_BORDERLESS ); // 
  // glutInitDisplayString("rgb stencil~4 double samples~8");
  
  printf( "Create Window %s\n" , DM_EnvironmentNode->DM_Window->id );
  if( DM_EnvironmentNode->game_mode ) {
    char sizeString[128];
    sprintf( sizeString , "%dx%d" , DM_EnvironmentNode->DM_Window->getWindowWidth() ,DM_EnvironmentNode->DM_Window->getWindowHeight() );
    glutGameModeString( sizeString );
    DM_EnvironmentNode->DM_Window->glutID = glutEnterGameMode();
	printf("Game Mode %dx%d at %d hertz, %d bpp\n",
			glutGameModeGet(GLUT_GAME_MODE_WIDTH),
			glutGameModeGet(GLUT_GAME_MODE_HEIGHT),
			glutGameModeGet(GLUT_GAME_MODE_REFRESH_RATE),
			glutGameModeGet(GLUT_GAME_MODE_PIXEL_DEPTH));
    // printf( "Gone into game mode\n" );
	// glutLeaveFullScreen();
  }
  else {
    DM_EnvironmentNode->DM_Window->glutID
      = glutCreateWindow(DM_EnvironmentNode->DM_Window->id);
  }

  ///////////////////////////////////////////////
  // OpenGL extended
  GLenum err = glewInit();  
  if (GLEW_OK != err) {
    /* Problem: glewInit failed, something is seriously wrong. */
    fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
  }
  fprintf(stdout, "GLEW version %s\n", glewGetString(GLEW_VERSION));

  // window selection
  glutSetWindow( DM_EnvironmentNode->DM_Window->glutID );
  
  glClearColor(0.0,0.0,0.0,1.0);
  glClear(GL_COLOR_BUFFER_BIT);
  
  // callback procedures for window reshape
  glutReshapeFunc(&window_reshape);
  // keyboard events handling
  glutKeyboardFunc(&window_key_browse);
  // special keys handling
  glutSpecialFunc(&window_special_key_browse);
  // mouse clicks handling
  glutMouseFunc(&window_mouseFunc_browse);
  // mouse drags handling
  glutMotionFunc(&window_motionFunc_browse);

  // tablet buttons handling
  glutTabletButtonFunc(&window_tabletButton);
  // tablet motion handling
  glutTabletMotionFunc(&window_tabletMotion);
  
  // passive selection
  //if( DM_EnvironmentNode->DM_Window->mouse_passive_selection ) {
  glutPassiveMotionFunc(&window_passiveMotionFunc_browse);

  // window visibility
  glutVisibilityFunc(NULL);
  // idle update
  glutTimerFunc((int)DM_EnvironmentNode->minimal_interframe_latency,
		&window_idle_browse,Step);
  // display function
  if( DM_EnvironmentNode->DM_Window->userRank == -1 ) {
    glutDisplayFunc(&window_display_blank);
  }
  else {
    glutDisplayFunc(&window_display);
  }

  // stores the viewpoint in which a click is made
  // for automatic navigation with button down
  WindowViewPointWithMouseDown = NULL;

  // glut win activation
  SetGlutWindow();
  // OpenGL initialization
  OpenGLInit();
  // cursor shape selection
  CursorInit();
}

void  dm_init_screen_message( void ) {
  // screen message initialization
  strcpy( ScreenMessage , "" );
  ScreenMessageX = 0;
  ScreenMessageY = 0;
  ScreenMessageColor = 1.0;
  NewScreenMessage = false;
  ScreenMessageInitialized = false;
}

void SetUserAndWindow( void ) {
  CurrentWindow = DM_EnvironmentNode->DM_Window;
  if( CurrentWindow->userRank >= 0 ) {
    CurrentIndUserNode = CurrentWindow->userRank;
    CurrentUser 
      = (DM_CompositionNode->tabUsers[ CurrentIndUserNode ])->GetUser();
    CurrentIndViewPoint = CurrentUser->indCurrentViewpoint;
    if( CurrentUser->viewvolume_type == DM_FRUSTUM ) {
      CurrentFrustum = CurrentUser->frustum;
      CurrentOrtho = NULL;
    }
    else {
      CurrentFrustum = NULL;
      CurrentOrtho = CurrentUser->ortho;
    }
  }
  else {
    CurrentIndUserNode = -1;
    CurrentUser = NULL;
    CurrentIndViewPoint = 0;
    CurrentFrustum = NULL;
    CurrentOrtho = NULL;
  }
  if( DM_CompositionNode ) {
    CurrentViewPointNode = DM_CompositionNode->viewpointsNodes[CurrentIndViewPoint];
    CurrentViewPoint = DM_CompositionNode->viewpoints[CurrentIndViewPoint];
    CurrentViewPointMatrix = DM_CompositionNode->viewpointsMatrices[CurrentIndViewPoint];
  }
  else {
    CurrentViewPointNode = NULL;
    CurrentViewPoint = NULL;
    CurrentViewPointMatrix = NULL;
  }
}


void GetUserAndWindow( void ) {
  int glutWinID = glutGetWindow();
  if( glutWinID == DM_EnvironmentNode->DM_Window->glutID
	&& DM_EnvironmentNode->DM_Window->userRank >= 0 ) {
    SetUserAndWindow();
 }
}


void SetGlutWindow( void ) {
  SetUserAndWindow();
  glutSetWindow( CurrentWindow->glutID );
}

void OpenGLInit( void ) {
  // background color
  if( CurrentUser ) {
    // glClearColor(rand()/(float)RAND_MAX,rand()/(float)RAND_MAX,rand()/(float)RAND_MAX,1);
    glClearColor(CurrentUser->bg_color[0],CurrentUser->bg_color[1],
     		 CurrentUser->bg_color[2],CurrentUser->bg_color[3]);
  }
  else {
    glClearColor (OpenGLBGColor[0], OpenGLBGColor[1], OpenGLBGColor[2], OpenGLBGColor[3]);
  }

  // buffer reset
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  // z-Buffer
  glEnable(GL_DEPTH_TEST);
  // // smooth shading
  // glShadeModel(GL_SMOOTH);
  // // normal vector normalization
  // glEnable( GL_NORMALIZE );

  // disable vsync
  // float r = 

}

void CursorInit( void ) {
  // Incidents memorables
  if( DM_EnvironmentNode->cursor_shape == NoCursor ) {
    glutSetCursor(GLUT_CURSOR_NONE);
  }
  else if( DM_EnvironmentNode->cursor_shape == LeftArrowCursor ) {
    glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
  }
  else if( DM_EnvironmentNode->cursor_shape == RightArrowCursor ) {
    glutSetCursor(GLUT_CURSOR_RIGHT_ARROW);
  }
}

#ifdef _WIN32
void dm_init_scene( int argc, char **argv ) {
#else
void dm_init_scene( void ) {
#endif
  if( argc <= 1 ) {
      sprintf( ErrorStr , "DM: not enough arguments" ); ReportError( ErrorStr );
      sprintf( ErrorStr , "Usage: DM [configuration.conf] (3Dscene.xml)+" ); ReportError( ErrorStr ); throw 100;
  }

  // log file
  if( *(DM_EnvironmentNode->log_file_name) ) {
    if ( ( fileLog = fopen(DM_EnvironmentNode->log_file_name , "wb" )) == NULL ) {
      sprintf( ErrorStr , "File %s not opened!" , DM_EnvironmentNode->log_file_name ); ReportError( ErrorStr ); throw 152;
    }
  }
  else {
    fileLog = stdout;
  }

  ///////////////////////////////////////////////
  // Version number
  fprintf( fileLog , "DM version 1.27\n");
  printf("DM version 1.27\n");

  // ------ composition allocation  ------------- //
  DM_CompositionNode = new dm_Composition();

  // ---------- scene file loading  ------------- //
  //printf( "LoadSceneFiles\n" );
  DM_CompositionNode->LoadSceneFiles( argc , argv );

  // ------ input buffer initialization  -------- //
  // input buffer is emptied
  // so that it is not reported in dynamic errors
  InputBufferCurrentBegin = 0;
  InputBufferCurrentEnd = 1;
  InputBuffer[ 0 ] = 0;
  InputBuffer[ 1 ] = 0;

  // ------------ script gathering  -------------- //
  //printf( "gatherScripts\n" );
  DM_CompositionNode->gatherScripts();

  // ---------- target initialization  ------------ //
  // known UDP targets
  // filled when external messages arrive for internal nodes
  // only contains the nodes that have received a UDP message so far
  nbKnownTargets = 0;
  knownTargets = new PSceneNode[ DM_EnvironmentNode->Nb_max_targets ];

  // ------ global variables initialization  ------------- //
  // frame number initialization
  FrameNo = DM_EnvironmentNode->first_frame_number - 1;

  // intial real time
  InitialRealTime = 0.0;
  InitialRealTime = RealTime() + DM_EnvironmentNode->initial_time;

  // current time initialization: internal time
  CurrentClockTime = DM_EnvironmentNode->initial_time;

  printf( "Initial time %.2f (real time %.5f)\n" , CurrentClockTime , InitialRealTime );

  // random seed initialization
  srand( clock() );

  // ------ screen message initialization  ------------- //
  dm_init_screen_message();


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
  for( int ind = 0 ; ind < DM_EnvironmentNode->nb_IP_Servers ; ind++ ) {
    DM_EnvironmentNode->IP_Servers[ ind ]->Init();
  }
  for( int ind = 0 ; ind < DM_EnvironmentNode->nb_IP_Clients ; ind++ ) {
    // printf( "Client %d initialization\n" , ind );
      DM_EnvironmentNode->IP_Clients[ ind ]->Init();
      // printf( "Client name %s\n" , DM_EnvironmentNode->IP_Clients[ ind ]->id );
  }
  Input_Message_String 
    = new char[ DM_EnvironmentNode->Max_network_message_length ];
  Input_Message_Local_Commande_String 
    = new char[ DM_EnvironmentNode->Max_network_message_length ];
  Output_Message_String 
    = new char[ DM_EnvironmentNode->Max_network_message_length ];
  // the OSC string is made 2 times longer because
  // it contains both pattern and argts
  Output_Message_OSC
    = new char[ 2 * DM_EnvironmentNode->Max_network_message_length ];
  Output_Message_Pattern 
    = new char[ DM_EnvironmentNode->Max_network_message_length ];
  Output_Message_ArgList
    = new char[ DM_EnvironmentNode->Max_network_message_length ];

  // initializes the tracks for recording the strokes
  InitStrokes();

  // launches the initialization command if there is one
  dm_Command *synchronous = NULL;
  if( DM_CompositionNode->initializationCommand ) {
    // printf( "initialization command\n" );
    DM_CompositionNode->initializationCommand
      ->execute( staticReferenceResolution , false , (char *)"" );
    synchronous = DM_CompositionNode->initializationCommand->synchronousCommand;
    while( synchronous ) {
      synchronous->execute( staticReferenceResolution , false , (char *)"" );
      synchronous = synchronous->synchronousCommand;
    }
  }

  // printf( "End of scene initialization\n" );

}

bool dm_shutdown = false;

void terminates_and_exits( void ) {
  // stores the shutdown status 
  // in order to avoid recalling itself and infinite recursion
  if( dm_shutdown ) {
    return;
  }
  else {
    dm_shutdown = true;
  }

#ifdef DM_HAVE_FFMPEG
  // closes video outputs
  if( DM_EnvironmentNode->outputVideo_file ) {
    if( DM_EnvironmentNode->audioVideoOutData ) {
      DM_EnvironmentNode->audioVideoOutData
	->close_audio_video_out();
      DM_EnvironmentNode->audioVideoOutData = NULL;
    }
  }
  // closes external video inputs
  for( int ind = 0 ; ind < DM_CompositionNode->NbTextures ; ind++ ) {
    if( DM_CompositionNode->video_records[ind] 
	&& DM_CompositionNode->video_records[ind]->external ) {
      extern_close_all();
    }
  }
#ifdef DM_EXTERN_LIVE_VIDEO
  grabberShutdownAll();
#endif
#endif

  // launches the termination command if there is one
  dm_Command *synchronous = NULL;
  if( DM_CompositionNode->terminationCommand ) {
    // printf( "termination command\n" );
    DM_CompositionNode->terminationCommand->print(stdout,1,1);
    DM_CompositionNode->terminationCommand
      ->execute( staticReferenceResolution , false , (char *)"" );
    synchronous = DM_CompositionNode->terminationCommand->synchronousCommand;
    while( synchronous ) {
      synchronous->execute( staticReferenceResolution , false , (char *)"" );
      synchronous = synchronous->synchronousCommand;
    }
  }

  // last event processing and udp message output
  DM_CompositionNode->popEvents();
  for( int ind = 0 ; ind < DM_EnvironmentNode->nb_IP_Clients ; ind++ ) {
    DM_EnvironmentNode->IP_Clients[ ind ]->sendIPmessages();
  }

  printf( "Last Frame #%d\n" , FrameNo );

  if( fileLog != stdout ) {
    fclose( fileLog );
  }


  // release global parameters
  // printf( "release CompositionNode\n" );
  if (DM_CompositionNode) {
    delete DM_CompositionNode;
    DM_CompositionNode = NULL;
  }
  
  if (deletedNodeList){
    // printf( "delete node list\n" );
    delete deletedNodeList;
    deletedNodeList = NULL;
  }

  // printf( "release variables\n" );
  releaseScalarList(deletedScalarList);
  releaseTableList(deletedTableList);
  releaseMatrixList(deletedMatrixList);

  // release the common variables in DM_EnvironmentNode. (Must be relased before releaseSceneObjects()
  // printf("releasewindows\n");
  delete DM_EnvironmentNode->DM_Window;
  DM_EnvironmentNode->DM_Window = NULL;

  // printf("releaseUDP\n");
  if (DM_EnvironmentNode->IP_Servers) {
    for (int ind = 0; ind < DM_EnvironmentNode->nb_IP_Servers; ind++) {
      delete DM_EnvironmentNode->IP_Servers[ind];
      DM_EnvironmentNode->IP_Servers[ind] = NULL;
    }
    
    delete [] DM_EnvironmentNode->IP_Servers;
    DM_EnvironmentNode->IP_Servers = NULL;
  }
  
  if (DM_EnvironmentNode->IP_Clients) {
    for (int ind = 0; ind < DM_EnvironmentNode->nb_IP_Clients; ind++) {
      delete DM_EnvironmentNode->IP_Clients[ind];
      DM_EnvironmentNode->IP_Clients[ind] = NULL;
    }
    
    delete [] DM_EnvironmentNode->IP_Clients;
    DM_EnvironmentNode->IP_Clients = NULL;
  }
  
  // printf( "allTexture %d\n" , AllTexture->nextTex->curTex->textureNo );
  // printf("releaseSceneObjects\n");
  releaseSceneObjects(AllSceneObject);
  // printf( "allTexture %d\n" , AllTexture->nextTex->curTex->textureNo );
  // printf("releaseAllScript\n");
  releaseAllScript(AllScript);
  // printf( "allTexture %d\n" , AllTexture->nextTex->curTex->textureNo );
  // printf("releaseAllTextures\n");
  releaseAllTextures(AllTexture);

  // printf("releaseForEnvtNode\n");
  envtNode->releaseForEnvtNode();
  delete envtNode;

  if (knownTargets) {
    delete [] knownTargets;
    knownTargets = NULL;
  }
  
  if (identity_matrix44) {
    delete identity_matrix44;
    identity_matrix44 = NULL;
  }
  
  if (null_matrix44) {
    delete null_matrix44;
    null_matrix44 = NULL;
  }
  
  if (Origin) {
    delete Origin;
    Origin = NULL;
  }
  
  if (NullVector) {
    delete NullVector;
    NullVector = NULL;
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
    dm_process_key( (int)key );
    break;
  }
}

void window_special_key_browse(int key, int x, int y)
{
  // use standard key processing (non-platform dependent)
  dm_process_special_key( key );

//   glutPostRedisplay();

}

void window_mouseFunc_browse(int button, int state, int x, int y) {
  // printf( "click button (%d,%d)\n" , x , y );
  CurrentMousePos_x = x;
  CurrentMousePos_y = y;
}

void window_motionFunc_browse(int x, int y) {
  // printf( "active button (%d,%d)\n" , x , y );
  CurrentMousePos_x = x;
  CurrentMousePos_y = y;
}

void window_passiveMotionFunc_browse(int x, int y) {
  // printf( "passive button (%d,%d)\n" , x , y );
  CurrentMousePos_x = x;
  CurrentMousePos_y = y;
  // freeglut (DM): glutGetModifiers() called outside an input callback/int
}

void window_tabletButton(int button, int state, int x, int y) {
  printf("tablet button %d, state %d x=%d y=%d\n" , button, state, x, y );
}

void window_tabletMotion(int x, int y) {
  printf("tablet motion x=%d y=%d\n" , x , y);
}

void window_idle_browse( void ) {
  window_idle_browse( 0 );
}

void window_idle_browse( int step ) {
  // printf("begin window_idle_browse\n");
  // fprintf( fileLog, "%.10f begin Time #\n" , RealTime() );

  // MemoryUsage();

  // -------------------- idle function recall ------------------------- //
  glutTimerFunc( (int)DM_EnvironmentNode->minimal_interframe_latency ,
	 	 &window_idle_browse , Step );

  Step = step;

  // boolean in order to print the buffer only once (after reception
  // of write_frame command
  WriteCurrentFrame = false;
  WriteCurrentFrameIndex = -1;

  // -------------------- scene update ------------------------- //
  // update is only made if at least window is displayed
  if( DM_EnvironmentNode->windowDisplayed ) {
    DM_EnvironmentNode->windowDisplayed = false;

    // frame count
    FrameNo++;
    DM_EnvironmentNode->current_frame_scalar->SetValue( (float)FrameNo );

    // current clock time
    double newtime = RealTime();

    // records the current mouse position in the variable
    // and in the tracks is recording is active
    updateMouseEnvironmentVariablesAndTables( newtime );

    if( FrameNo == 1 ) {
      // ------------ composition printing  ---------- //FONT
      //printf( "print composition\n" );
      //DM_CompositionNode->print( stdout , true );
      DM_CompositionNode->print( DM_EnvironmentNode , true );
    }

    // FPS computation
    if( FrameNo % 100 == 0 && newtime != LastFrameTime) {
      FramePerSecond = (int)((FrameNo - LastFrame)/(newtime-LastFrameTime));
      LastFrame = FrameNo;
      LastFrameTime = newtime;
      
      if( DisplayFramePerSecond  ) {

#ifdef _WIN32
	_snprintf( ScreenMessage , XMLAttrValLength , "%d FPS" , FramePerSecond );
#else
	snprintf( ScreenMessage , XMLAttrValLength , "%d FPS" , FramePerSecond );
#endif

	ScreenMessageColor = 1.0;
	LastDecayTime = newtime;
	NewScreenMessage = true;
      }
      // printf( "%s\n" , ScreenMessage );
    }

    // decays window message brightness
    if( ScreenMessageColor > 0.0 ) {
      ScreenMessageColor -= 0.4F*((float)newtime - (float)LastDecayTime);
      LastDecayTime = newtime;
    }
    // if message brightness becomes null, the message is erased
    if( ScreenMessageColor <= 0.0 ) {
      ScreenMessage[ 0 ] = 0;
      ScreenMessageColor = 0.0;
      LastDecayTime = newtime;
    }

    if( DM_EnvironmentNode->trace_output_frame_number ) {
      printf( "Frame #%d\n" , FrameNo );
    }
    // fprintf( fileLog , "Frame #%d\n" , FrameNo );
    // last image No display
    if( FrameNo > DM_EnvironmentNode->last_frame_number ) {
      printf( " Last Frame #%d\n" , DM_EnvironmentNode->last_frame_number );
      exit(0);
    }

    // internal time count
    if( !DM_EnvironmentNode->real_time ) {
      if( DM_EnvironmentNode->time_step != 0 ) {
	CurrentClockTime += DM_EnvironmentNode->time_step;
      }
    }
    // real time count
    else {
      CurrentClockTime = RealTime();
    }

    if( DM_EnvironmentNode->trace_time ) {
      printf( " Time #%.2f\n" , CurrentClockTime );
    }

    // fprintf( fileLog, "%.10f popEvents Time #\n" , RealTime() );
    // internal and external event processing
    DM_CompositionNode->popEvents();

    // fprintf( fileLog, "%.10f update Time #\n" , RealTime() );
    // updating of animations
    // printf("begin update\n");
    DM_CompositionNode->update();
    // printf("begin update\n");

    for( int ind = 0 ; ind < DM_EnvironmentNode->nb_IP_Servers ; ind++ ) {
      DM_EnvironmentNode->IP_Servers[ ind ]->receiveIPMessages();
     }
    for( int ind = 0 ; ind < DM_EnvironmentNode->nb_IP_Clients ; ind++ ) {
      DM_EnvironmentNode->IP_Clients[ ind ]->sendIPmessages();
    }
  }

  // printf( "userRank %d (%d)\n" , DM_EnvironmentNode->DM_Window->userRank , ind );
  if( DM_EnvironmentNode->DM_Window->userRank >= 0 ) {
    
    // viewpoint change according to scenarii
    currentViewpoint();
    
    // defines the global variables associated with the current window
    SetGlutWindow();
    
    // -------------------- window redisplay ------------------------- //
    // fprintf( fileLog, "%.10f glutPostRedisplay Time #\n" , RealTime() );
    glutPostRedisplay();
    
  }

  // fprintf( fileLog, "%.10f end Time #\n\n" , RealTime() );
  Step++;
  // printf("end window_idle_browse\n");
}

void window_reshape(GLsizei width, GLsizei height) {
  SetUserAndWindow();
  DM_EnvironmentNode->window_width_scalar->SetValue( (float)width );
  DM_EnvironmentNode->window_height_scalar->SetValue( (float)height );

  CurrentWindow->setWindowHeight( (float)height );
  CurrentWindow->setWindowWidth( (float)width );
  CurrentWindow->manageRatio( CurrentFrustum , CurrentOrtho );
  glViewport((width - CurrentWindow->getWindowWidth())/2,
	     (height - CurrentWindow->getWindowHeight())/2,
	     CurrentWindow->getWindowWidth(),
	     CurrentWindow->getWindowHeight());
  CurrentWindow->setWindowHeight( (float)CurrentWindow->getWindowHeight() );
  CurrentWindow->setWindowWidth( (float)CurrentWindow->getWindowWidth() );
  CurrentWindow->setWindow_x( (float)(width - CurrentWindow->getWindowWidth())/2 );
  CurrentWindow->setWindow_y( (float)(height - CurrentWindow->getWindowHeight())/2 );
  //  printf("reshape main win %d %d %d\n" , CurrentWindow->getWindowWidth() , CurrentWindow->getWindowHeight() , width , height );
  GetUserAndWindow();
}

void currentViewpoint( void ) {
  // the set of shots is followed only if the user
  // has not taken control over the viewpoints
  if( !CurrentUser  || CurrentUser->manualViewpointControl ) {
    return;
  }

  // the first active shot is used to decide the
  // rank of the active viewpoint
  for( int ind = 0; ind < CurrentUser->nbViewpointShots ; ind++ ) {
    if( CurrentClockTime >=  CurrentUser->shotIni[ind]
	&& CurrentClockTime <=  CurrentUser->shotEnd[ind] ) {
      if( CurrentUser->shotIndViewpoint[ind]
	  < DM_CompositionNode->nbRegisteredViewpoints ) {
	CurrentUser->indCurrentViewpoint = CurrentUser->shotIndViewpoint[ind];
	if( CurrentUser->alternate_stereo_viewer
	    && FrameNo % 2 == 0
	    && CurrentUser->shotIndViewpointStereoLeft[ind]
	    < DM_CompositionNode->nbRegisteredViewpoints ) {
	  CurrentUser->indCurrentViewpoint
	    = CurrentUser->shotIndViewpointStereoLeft[ind];
	}
	break;
      }
      else {
	sprintf( ErrorStr , "viewpoint shot number exceeds number of viewpoints in scene!" ); ReportError( ErrorStr ); throw 207;
      }
    }
  }
  //  printf( "current_viewpoint 2 %d\n" , DM_CompositionNode->tabUsers[ 0 ]->indCurrentViewpoint );
}

void releaseSceneObjects(strAllSceneObject* allObjects) {
  if (allObjects) {
    if (allObjects->curObj != NULL) {
      // printf( "delete object ID (%s) \n" , allObjects->curObj->GetId() );
      // printf( "allTexture %d\n" , AllTexture->nextTex->curTex->textureNo );
      // allObjects->curObj->print( stdout , 1, 1);
      // the node in an interpolator node should not be deleted because it is only a carrier node
      delete allObjects->curObj;
      allObjects->curObj = NULL;
      // printf( "allTexture %d\n" , AllTexture->nextTex->curTex->textureNo );
    }
    
    if (allObjects->nextObj) {
      releaseSceneObjects(allObjects->nextObj);
      allObjects->nextObj = NULL;
    }
    
    delete allObjects;
    allObjects = NULL;
  }
}

void releaseAllScript(strAllScript* allScript) {
  if (!allScript) return;
  
  if (allScript->curtScript) {
    // printf( "delete script (%s)\n" , allScript->curtScript->id );
    delete allScript->curtScript;
    allScript->curtScript = NULL;
  }
  
  
  if (allScript->nextScript) {
    releaseAllScript(allScript->nextScript);
    allScript->nextScript = NULL;
  }
  
  delete allScript;
  allScript = NULL;
}

void releaseScalarList(dm_Scalar* listScalar) {
  if (!listScalar) return;
  
  // set the nextvariable or mother variable link to be NULL, so that only chineScalar is used for deletion
  listScalar->nextVariable = NULL;
  listScalar->motherVariable = NULL;
  
  if (listScalar->chineScalar) {
    releaseScalarList(listScalar->chineScalar);
    listScalar->chineScalar = NULL;
  }
  
  if (listScalar) {
    delete (listScalar);
    listScalar = NULL;
  }
}


void releaseTableList(dm_Table* listTable) {
  if (!listTable) return;
  
  // set the nextvariable or mother variable link to be NULL, so that only chineScalar is used for deletion
  listTable->nextVariable = NULL;
  listTable->motherVariable = NULL;
  
  if (listTable->chineTable) {
    releaseTableList(listTable->chineTable);
    listTable->chineTable = NULL;
  }
  
  if (listTable) {
    delete (listTable);
    listTable = NULL;
  }
}


void releaseMatrixList(dm_Matrix* listMatrix) {
  if (!listMatrix) return;
  
  // set the nextvariable or mother variable link to be NULL, so that only chineScalar is used for deletion
  listMatrix->nextVariable = NULL;
  listMatrix->motherVariable = NULL;
  
  if (listMatrix->chineMatrix) {
    releaseMatrixList(listMatrix->chineMatrix);
    listMatrix->chineMatrix = NULL;
  }
  
  if (listMatrix) {
    delete (listMatrix);
    listMatrix = NULL;
  }
}

void releaseAllTextures(strAllTexture* allTexture) {
  if (!allTexture) return;
  
  if (allTexture->curTex) {
    delete allTexture->curTex;
    allTexture->curTex = NULL;
  }
  
  
  if (allTexture->nextTex) {
    releaseAllTextures(allTexture->nextTex);
    allTexture->nextTex = NULL;
  }
  
  delete allTexture;
  allTexture = NULL;
}
