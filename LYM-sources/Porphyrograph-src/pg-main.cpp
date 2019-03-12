/*! \file pg-main.cpp
 * 
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

#ifdef _WIN32
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

// displayed window
pg_Window     *CurrentWindow = NULL;
bool           windowDisplayed = false;

int leftWindowWidth = 1024;
int rightWindowWidth = 1024;
int rightWindowVMargin = 0;
int doubleWindowWidth = 2048;

/// screen message drawing
char ScreenMessage[StringLength];
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

/// OpenGL background color
float OpenGLBGColor[4] = {0.0, 0.0, 0.0, 1.0};

/// number of frames displayed
int Step = 0;
/// current internal time for animations
float CurrentClockTime = 0.0f;
/// initial time (in case of real timescale)
float InitialRealTime = 0.0f;
/// last image swap
float LastImageSwap = 0.0f;
/// last frame wall time
float LastFrameTime = 0.f;
float LastDecayTime = 0.f;
int LastFrame = 0;
int FramePerSecond = 0;
bool DisplayFramePerSecond = false;

/// current mouse location
int CurrentMousePos_x = PG_OUT_OF_SCREEN_CURSOR, CurrentMousePos_y = PG_OUT_OF_SCREEN_CURSOR;
float paths_x_0_forGPU = PG_OUT_OF_SCREEN_CURSOR, paths_y_0_forGPU = PG_OUT_OF_SCREEN_CURSOR;
float paths_x_0_begin_position = PG_OUT_OF_SCREEN_CURSOR, paths_y_0_begin_position = PG_OUT_OF_SCREEN_CURSOR;
int CurrentCursorHooverPos_x, CurrentCursorHooverPos_y;
int CurrentCursorStylusvsRubber = pg_Stylus;
// current tablet pen pressure and orientation
float tabletPressureRadius = 0.0f;
float tabletAzimutRadius = 0.0f;
float tabletInclinationRadius = 0.0f;

/// motion while button is down
float LinearMotionStep = 0;

/// log file
FILE    *fileLog;

// date string for output files
std::stringstream  date_stringStream;


// frame number 
int      pg_FrameNo;
int      pg_LaunchFrameNo = INT_MAX;
// frame number when to reach particle target
int      pg_targetFrameNo;
// last frame with camera parameter change
int      pg_LastCameraParameterChange_Frame;

// glut initialized
bool glut_initialized = false;

int argc; char **argv;

/// MAIN FUNCTION
int main(int argcMain, char **argvMain) {
	argc = argcMain;
	argv = argvMain;

	// error message and input buffer memory
	ErrorStr = new char[1024];

	// current date/time based on current system
	time_t now = time(0);
	tm *ltm = localtime(&now);

	// print various components of tm structure.
	//std::cout << "Year: "<< 1900 + ltm->tm_year << std::endl;
	//std::cout << "Month: "<< 1 + ltm->tm_mon<< std::endl;
	//std::cout << "Day: "<<  ltm->tm_mday << std::endl;
	//std::cout << "Time: "<< 1 + ltm->tm_hour << ":";
	//std::cout << 1 + ltm->tm_min << ":";
	//std::cout << 1 + ltm->tm_sec << std::endl;

	date_stringStream
		<< std::setfill('0') << std::setw(2) << 1 + ltm->tm_mon << "-"
		<< std::setfill('0') << std::setw(2) << ltm->tm_mday << "-"
		<< std::setfill('0') << std::setw(2) << (1900 + ltm->tm_year) << "_"
		<< std::setfill('0') << std::setw(2) << ltm->tm_hour << "-"
		<< std::setfill('0') << std::setw(2) << ltm->tm_min << "-"
		<< ltm->tm_year - 100 << std::flush;

	// cyclic input buffer
	// loading the configuration file
	// the configuration file name must be terminated by ".csv" or ".txt"
	// possibly the first argument with such an extension
	int configurationFileLoaded = false;
	for (int ind = 0; ind < argc - 1; ind++) {
		if ((strstr(argv[ind + 1], ".csv") || strstr(argv[ind + 1], ".txt"))
			&& (strstr(argv[ind + 2], ".csv") || strstr(argv[ind + 2], ".txt"))) {
			LoadConfigurationFile(argv[ind + 1], argv[ind + 2]);
			configurationFileLoaded = true;
			printf("Configuration file loaded: nb tracks %d\n", PG_NB_TRACKS);
			break;
		}
	}

	// or the default conf file name
	if (!configurationFileLoaded) {
		LoadConfigurationFile((char *)("configuration/LYM_" + project_name + "-head-single.txt").c_str(),
			(char *)("scenarios/LYM_" + project_name + "-scenario.txt").c_str());
	}

	///////////////////////////////////////////////
	// GLUT display library

	// scene initialization
	pg_init_scene();

	// Version number
	string applicationName = "Porphyrograph  (" + project_name + ")";
	fprintf(fileLog, "%s\n", applicationName.c_str());
	pg_logFirstLineSceneVariables();
	printf("%s\n", applicationName.c_str());

	std::cout << "Date: [" << date_stringStream.str() << "]" << std::endl;

	// printf( "Glut initialization\n" );
	// glut parameters initialization 
	glutInit(&argc, argv);
	initGlutWindows();
	printOglError(474);

	// OpenGL initialization (clear buffer)
	OpenGLInit();
	// cursor shape selection
	CursorInit();
	printOglError(475);


	// initializations before rendering

	// sensor initialization
#ifdef PG_SENSORS
	SensorInitialization();
#endif

	// matrices, geometry, shaders and FBOs
	pg_initRenderingMatrices();
	printOglError(31);

	// buiilds the quads to be rendered
	pg_initGeometry_quads();
	printOglError(32);
	
	// loads the shaders
	pg_loadAllShaders();
	printOglError(33);

	// initializes the FBOS
	pg_initFBO();
	printOglError(34);

	// texture initialization
	pg_initTextures();
	printOglError(36);

	// initialisation of cwd
	cwd = GetCurrentWorkingDir();

#ifdef PG_WITH_PUREDATA
	// connects PD to porphyrograph
	pg_send_message_udp((char *)"i", (char *)"/connect 1", (char *)"udp_PD_send");
#endif

	// lights off the LED
	pg_send_message_udp((char *)"f", (char *)"/launch 0", (char *)"udp_TouchOSC_send");
#ifdef VOLUSPA
	// tells Usine that it is not launched
	pg_send_message_udp((char *)"f", (char *)"/launch 0", (char *)"udp_Usine_send");
#endif


#ifdef PG_RENOISE
	//pg_send_message_udp((char *)"i", (char *)"/track/1/solo 1", (char *)"udp_RN_send");
	//pg_send_message_udp((char *)"i", (char *)"/track/2/solo 1", (char *)"udp_RN_send");
	//pg_send_message_udp((char *)"i", (char *)"/track/3/mute 1", (char *)"udp_RN_send");
	//pg_send_message_udp((char *)"i", (char *)"/track/4/mute 1", (char *)"udp_RN_send");
	//pg_send_message_udp((char *)"i", (char *)"/track/4/volume 0", (char *)"udp_RN_send");
	//pg_send_message_udp((char *)"i", (char *)"/master/mute 1", (char *)"udp_RN_send");
	pg_send_message_udp((char *)"", (char *)"/scene/1/launch", (char *)"udp_RN_send");
#endif


	// INITIALIZES ALL SCENARIO VARIABLES AND ASSIGNS THEM THE VALUES OF THE FIRST SCENARIO LINE
	pg_initializationScript();

	// GUI DISPLAY & LOG FILE LOGGING
	pg_displaySceneVariables();

	// LOADS DIAPORAMA
#if defined (TVW)
	// loads the texture buffer of the initial images and masks
	// pg_ReadInitalImageTexturesTVW(0, 2130, 28, -1);
	// pg_ReadInitalImageTexturesTVW(0, 200, 28, -1);
	pg_ReadInitalImageTexturesTVW(0, -1, -1, 15);
#else
	pg_ReadInitalImageTextures(0, -1, -1, 15);
#endif

	// applies all callbacks
	pg_initializationCallBacks();
	printOglError(467);

	// camera frame capture initialization
#ifdef PG_WITH_CAMERA_CAPTURE
	pg_initVideoCameraCapture();
#endif

	// video intialization
	if (playing_movieNo >= 0 && playing_movieNo < nb_movies) {
		currentlyPlaying_movieNo = playing_movieNo;

		// texture ID initialization (should not be inside a thread)
		if (pg_movie_texture_texID == NULL_ID) {
			glGenTextures(1, &pg_movie_texture_texID);
		}

		is_movieLoading = true;
		printf("Loading %s\n", ("Data/" + project_name + "-data/videos/" + movieFileName[currentlyPlaying_movieNo]).c_str());
		sprintf(AuxString, "/movie_shortName %s", movieShortName[currentlyPlaying_movieNo].c_str());
		pg_send_message_udp((char *)"s", AuxString, (char *)"udp_TouchOSC_send");

#ifdef WIN32
		DWORD rc;
		HANDLE  hThread = CreateThread(
			NULL,                   // default security attributes
			0,                      // use default stack size  
			pg_initVideoMoviePlayback,		    // thread function name
			(void *)(&movieFileName[currentlyPlaying_movieNo]),		    // argument to thread function 
			0,                      // use default creation flags 
			&rc);   // returns the thread identifier 
		if (hThread == NULL) {
			std::cout << "Error:unable to create thread pg_initVideoMoviePlayback" << std::endl;
			exit(-1);
		}
		CloseHandle(hThread);
#else
		pthread_t drawing_thread;
		int rc;
		rc = pthread_create(&drawing_thread, NULL,
				    pg_initVideoMoviePlayback,
				    (void *)(&movieFileName[currentlyPlaying_movieNo]));
		if (rc) {
			std::cout << "Error:unable to create thread pg_initVideoMoviePlayback" << rc << std::endl;
			exit(-1);
		}
		pthread_exit(NULL);
#endif
	}

	/////////////////////////////////////////////////////////////////////////
	// SVG GPU INITIALIZATION
	const char	*SVG_GPU_programName = "nvpr_SVG_GPU";
	initializeNVPathRender(SVG_GPU_programName);
	SVG_path_baseID = glGenPathsNV(pg_nb_tot_SvgGpu_paths);
	SVG_path_fill_color = new GLint[pg_nb_tot_SvgGpu_paths];
	for (int indClipArtFile = 0; indClipArtFile < pg_nb_ClipArt; indClipArtFile++) {
		LoadSVGPathsToGPU("Data/" + project_name + "-data/SVG_GPUs/" + pg_ClipArt_fileNames[indClipArtFile], pg_ind_first_SvgGpu_path_in_ClipArt[indClipArtFile], pg_nb_paths_in_ClipArt[indClipArtFile]);
		// std::cout << "svg_path #" << indPath << ": " << "Data/" + project_name + "-data/SVGs/" + temp << " track #" << indTrack << "\n";
	}

	printOglError(37);

	// main loop for event processing and display
	glutMainLoop();

	return 0;

	// end of glut display library
	///////////////////////////////////////////////
}


void initGlutWindows( void ) {
	printf("GLUT size %dx%d\n", window_width, window_height);

  glutInitWindowSize( window_width, window_height );
  glutInitWindowPosition( window_x, window_y );
  
  glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_STENCIL | GLUT_BORDERLESS ); // 
  
  CurrentWindow->glutID = glutCreateWindow(" ");

  // window resize
  printf("Window size %dx%d\n", window_width, window_height);
  glutReshapeWindow(window_width, window_height);
  // glutFullScreen();                      // Switch into full screen

  ///////////////////////////////////////////////
  // OpenGL extended
#ifdef WIN32_EXTENDED
  if (!CreateGLContext()) {
	  printf("OpenGL transparency context not initialized\n");
	  exit(0);
  }
#else
  GLenum err = glewInit();  
  if (GLEW_OK != err) {
    /* Problem: glewInit failed, something is seriously wrong. */
    fprintf(stderr, "Error: OpenGL Extension Wrangler (GLEW) failed to initialize %s\n", glewGetErrorString(err));
  }
  fprintf(stdout, "GLEW version %s\n", glewGetString(GLEW_VERSION));
  bool hasDSA = glewIsSupported("GL_EXT_direct_state_access");
  if (!hasDSA) {
	  fprintf(stderr, "Error: OpenGL implementation doesn't support GL_EXT_direct_state_access\n");
  }
#endif

  // get version info
  const GLubyte* renderer = glGetString (GL_RENDERER); // get renderer string
  const GLubyte* version = glGetString (GL_VERSION); // version as a string
													 //Or better yet, use the GL3 way to get the version number
  int OpenGLVersion[2];
  glGetIntegerv(GL_MAJOR_VERSION, &OpenGLVersion[0]);
  glGetIntegerv(GL_MINOR_VERSION, &OpenGLVersion[1]);
  printf ("Renderer: %s\n", renderer);
  printf ("OpenGL version supported %s (%d-%d)\n", version, OpenGLVersion[0], OpenGLVersion[1] );

  int maxTextureSize;
  int maxGeomVert;
  glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
  glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_VERTICES, &maxGeomVert);
  GLint maxAttach = 0;
  glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxAttach);
  GLint maxDrawBuf = 0;
  glGetIntegerv(GL_MAX_DRAW_BUFFERS, &maxDrawBuf);
  printf ("Max texture size %d geometry shader vertices %d max attach %d max draw buffs %d\n", 
	  maxTextureSize, maxGeomVert, maxAttach, maxDrawBuf);
  GLint maxFragUnif = 0;
  glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, &maxFragUnif);
  printf("Max uniform components %d\n",
	  maxFragUnif);
  
  
  // window selection
  // glut win activation
  glutSetWindow(CurrentWindow->glutID);

  glClearColor(0.0, 0.0, 0.0, 0.0);
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  // callback procedures for window reshape
  glutReshapeFunc(&window_reshape);
  // keyboard events handling
  glutKeyboardFunc(&window_key_browse);
  // special keys handling
  glutSpecialFunc(&window_special_key_browse);

#ifdef PG_WACOM_TABLET
  // wacom tablet handling
  glutWacomTabletCursorFunc(&window_PG_WACOM_TABLET_browse);
#else  // mouse clicks handling
  glutMouseFunc(&window_mouseFunc_browse);
  // mouse drags handling
  glutMotionFunc(&window_motionFunc_browse);
  // passive selection
  //if( CurrentWindow->mouse_passive_selection ) {
  glutPassiveMotionFunc(&window_passiveMotionFunc_browse);
#endif
  
  // window visibility
  glutVisibilityFunc(NULL);
  // idle update
  glutTimerFunc((int)minimal_interframe_latency,
		&window_idle_browse,Step);
  glutDisplayFunc(&window_display);

}

void  pg_init_screen_message( void ) {
  // screen message initialization
  strcpy( ScreenMessage , "" );
  messageTransparency = 1.0;
  NewScreenMessage = false;
  ScreenMessageInitialized = false;
  LastDecayTime = CurrentClockTime;
}

#if defined (TVW)
void  pg_init_display_message(void) {
	// screen message initialization
	pg_CurrentDiaporamaDir = -1;
	DisplayText1Alpha = 0.0f;
	DisplayText2Alpha = 0.0f;
}
#endif

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
  glutSetCursor(GLUT_CURSOR_NONE);
}

void pg_init_scene(void) {
	if (argc <= 1) {
		sprintf(ErrorStr, "Porphyrograph: not enough arguments"); ReportError(ErrorStr);
		sprintf(ErrorStr, "Usage: Porphyrograph [configuration.conf]"); ReportError(ErrorStr); throw 100;
	}

	// log file
	if (!log_file_name.empty()) {
		printf("Opening log file %s\n", log_file_name.c_str());
		if ((fileLog = fopen(log_file_name.c_str(), "wb")) == NULL) {
			sprintf(ErrorStr, "File %s not opened!", log_file_name.c_str()); ReportError(ErrorStr); throw 152;
		}
	}
	else {
		fileLog = stdout;
	}

	///////////////////////////////////////////////
	// GLOBAL VARIABLES INITIALIZATION
	// frame number initialization
	pg_FrameNo = first_frame_number - 1;
	pg_targetFrameNo = pg_FrameNo;

	// last camera change initialization
	pg_LastCameraParameterChange_Frame = pg_FrameNo;

	// intial real time
	InitialRealTime = RealTime() + initial_time;

	// initial scenario time: well before current time to be over
	InitialScenarioTime = InitialRealTime - 1000000.f;
	AbsoluteInitialScenarioTime = InitialRealTime - 1000000.f;

	// current time initialization: internal time
	CurrentClockTime = RealTime();

	printf("Initial time %.2f (real time %.5f)\n", CurrentClockTime, InitialRealTime);

	// random seed initialization
	srand(clock());

	// ------ screen message initialization  ------------- //
	pg_init_screen_message();

#if defined (TVW)
	// reads the text messages in the text file
	pg_ReadAllDisplayMessages(pg_MessageDirectory, "message_list.txt");

	// ------ display message initialization  ------------- //
	pg_init_display_message();
#endif


#ifdef PG_SENSORS
	/////////////////////////////////////////////////////////////////////////
	// SENSORS INITIALIZATION
	// copies the grid layout
	assignSensorPositions();

	// copies the single central activation
	assignSensorActivations();
#endif

	/////////////////////////////////////////////////////////////////////////
	// UDP INITIALIZATION
#ifdef WIN32
	WSADATA wsaData;
	// Initialize Winsock
	int err = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (err != 0) {
		/* Tell the user that we could not find a usable */
		/* WinSock DLL.                                  */
		sprintf(ErrorStr, "could not find a usable WinSock DLL!"); ReportError(ErrorStr); throw 207;
	}
#endif

	// server initialization
	for (int ind = 0; ind < nb_IP_Servers; ind++) {
		IP_Servers[ind]->InitServer();
	}
	// client initialization
	for (int ind = 0; ind < nb_IP_Clients; ind++) {
		// printf( "Client %d initialization\n" , ind );
		  //std::cout << "IP_Clients[ ind_IP_Client ]->Remote_server_IP: " << ind << " " << IP_Clients[ind]->Remote_server_IP << "\n";
		  //std::cout << "IP_Clients[ ind_IP_Client ]->Remote_server_port: " << IP_Clients[ind]->Remote_server_port << "\n";
		IP_Clients[ind]->InitClient();
		// printf( "Client name %s\n" , IP_Clients[ ind ]->id );
	}
	Input_Message_String
		= new char[max_network_message_length];
	Input_Message_Local_Commande_String
		= new char[max_network_message_length];
	Output_Message_String
		= new char[max_network_message_length];
	// the OSC string is made 2 times longer because
	// it contains both pattern and argts
	Output_Message_OSC
		= new char[2 * max_network_message_length];
	Output_Message_Pattern
		= new char[max_network_message_length];
	Output_Message_ArgList
		= new char[max_network_message_length];
	// printf( "End of scene initialization\n" );
}

bool pg_shutdown = false;

void quit( void ) {
	// for Annika performance: save the svg paths before quitting (could perhaps be generalized)
#ifdef KOMPARTSD
	pg_draw_scene(_Svg, false);
#endif
	
	// lights off the LED
	pg_send_message_udp((char *)"f", (char *)"/launch 0", (char *)"udp_TouchOSC_send");
#ifdef VOLUSPA
	// starts the backtrack
	pg_send_message_udp((char *)"f", (char *)"/launch 0", (char *)"udp_Usine_send");
#endif
#ifdef PG_RENOISE
	sprintf(AuxString, "/renoise/transport/stop"); pg_send_message_udp((char *)"", AuxString, (char *)"udp_RN_send");
#endif
	pg_send_message_udp((char *)"f", (char *)"/quit 1", (char *)"udp_TouchOSC_send");

#ifdef PG_WITH_PUREDATA
	pg_send_message_udp((char *)"f", (char *)"/stopsoundtrack 0", (char *)"udp_PD_send");

	// soundtrack off
	pg_send_message_udp((char *)"i", "/soundtrack_onOff 0", (char *)"udp_PD_send");
#endif
#ifdef PG_WITH_JUCE
	// soundtrack off
	pg_send_message_udp((char *)"", (char *)"/JUCE_stop_track", (char *)"udp_SoundJUCE_send");

#endif

	// lights out the LEDs
	pg_send_message_udp((char *)"i", (char *)"/switchOff_LEDs 1", (char *)"udp_TouchOSC_send");

	// sends all the remaining messages
	for (int ind = 0; ind < nb_IP_Clients; ind++) {
		IP_Clients[ind]->sendIPmessages();
	}

	// stores the shutdown status 
  // in order to avoid recalling itself and infinite recursion
  if( pg_shutdown ) {
    return;
  }
  else {
    pg_shutdown = true;
  }

  printf( "Last Frame #%d\n" , pg_FrameNo );

  if( fileLog != stdout ) {
    fclose( fileLog );
  }

  // release camera
  pg_movie_capture.release();
  // release movie
  pg_camera_capture.release();

  // release global parameters
  // release the common variables in PG_EnvironmentNode. (Must be relased before releaseSceneObjects()
  // printf("releasewindows\n");
  delete CurrentWindow;
  CurrentWindow = NULL;

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
  
  if (ErrorStr) {
    delete [] ErrorStr;
    ErrorStr = NULL;
  }

  exit(1);
}

void window_key_browse(unsigned char key, int x, int y)
{
  switch (key) {
  case KEY_ESC:
    quit();
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

#ifdef PG_WACOM_TABLET
void window_PG_WACOM_TABLET_browse(int x, int y, float press, float az, float incl, int twist, int cursor) {
	// drawing
	if (press > 0) {
		//printf("pen writing %d %d\n",x,y);
		CurrentMousePos_x = x; //  int(x / 1024. * leftWindowWidth);
		CurrentMousePos_y = y; // int(y / 768. * window_height);
		if (CurrentCursorStylusvsRubber == pg_Stylus) {
			CurrentCursorHooverPos_x = PG_OUT_OF_SCREEN_CURSOR;
			CurrentCursorHooverPos_y = PG_OUT_OF_SCREEN_CURSOR;
		}
		else {
			CurrentCursorHooverPos_x = x; // int(x / 1024. * leftWindowWidth);
			CurrentCursorHooverPos_y = y; // int(y / 768. * window_height);
		}
	}	
	// hoovering
	else {
		//printf("pen hoovering %d %d\n", x, y);
		CurrentMousePos_x = PG_OUT_OF_SCREEN_CURSOR;
		CurrentMousePos_y = PG_OUT_OF_SCREEN_CURSOR;
		CurrentCursorHooverPos_x = x; // int(x / 1024. * leftWindowWidth);
		CurrentCursorHooverPos_y = y; // int(y / 768. * window_height);
	}
	// printf("pos = %d %d\n", CurrentMousePos_x, CurrentMousePos_y);

	tabletPressureRadius = press;
	tabletAzimutRadius = az;
	tabletInclinationRadius = incl;

	// verify these values received from wacom tablet
	if (cursor == 1) {
		CurrentCursorStylusvsRubber = pg_Stylus;
	}
	else if (cursor == 2) {
		CurrentCursorStylusvsRubber = pg_Rubber;
	}

	//printf("Porphyrograph pt [%d,%d] press %.3f az %.3f alt %.3f twist %d\n" , 
		  //x, y, press, az, incl, twist );
}
#else
void window_mouseFunc_browse(int button, int state, int x, int y) {
  // printf( "click button %d (%d,%d)\n" , button , x , y );
  CurrentMousePos_x = x;
  CurrentMousePos_y = y;
  CurrentCursorHooverPos_x = x;
  CurrentCursorHooverPos_y = y;
  if( button == 0 ) {
      CurrentCursorStylusvsRubber = pg_Stylus;
   }
   else if( button == 2 ) {
      CurrentCursorStylusvsRubber = pg_Rubber;
   }
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
    CurrentMousePos_x = PG_OUT_OF_SCREEN_CURSOR;
    CurrentMousePos_y = PG_OUT_OF_SCREEN_CURSOR;
    CurrentCursorHooverPos_x = x;
    CurrentCursorHooverPos_y = y;
  // freeglut (PG): glutGetModifiers() called outside an input callback/int
}
#endif

void window_idle_browse( void ) {
  window_idle_browse( 0 );
}

void window_idle_browse(int step) {
	// printf("begin window_idle_browse\n");
	// fprintf( fileLog, "%.10f begin Time #\n" , RealTime() );

	// MemoryUsage();

	// -------------------- idle function recall ------------------------- //
	glutTimerFunc((int)minimal_interframe_latency,
		&window_idle_browse, Step);

	Step = step;

	// -------------------- scene update ------------------------- //
	// update is only made if at least window is displayed
	if (windowDisplayed) {
		windowDisplayed = false;

		// frame count
		pg_FrameNo++;
		// printf(" Frame %d\n", pg_FrameNo);

		if (pg_FrameNo == pg_targetFrameNo) {
			// printf(" End of image target\n");
		}

		// current clock time
		CurrentClockTime = RealTime();
		if (trace_time) {
			printf(" Time #%.2f\n", CurrentClockTime);
		}

		// printf("render frame %d\n", pg_FrameNo);

		// GUI DISPLAY & LOG FILE LOGGING
		pg_displaySceneVariables();

		// FPS computation
		if (pg_FrameNo % 100 == 0 && CurrentClockTime != LastFrameTime) {
			FramePerSecond = (int)((pg_FrameNo - LastFrame) / (CurrentClockTime - LastFrameTime));
			LastFrame = pg_FrameNo;
			LastFrameTime = CurrentClockTime;
			char mess[50];

			if (DisplayFramePerSecond) {
				sprintf(mess, "%d FPS", FramePerSecond);
				pg_writeMessageOnScreen((char *)mess);
				printf("%s\n", ScreenMessage);
			}
		}

		// printf( "CAType %d subtype %d\n" , CAType , CASubType );


		// decays window message brightness
		if (messageTransparency > 0.0) {
			messageTransparency -= 0.4f*(CurrentClockTime - LastDecayTime);
			LastDecayTime = CurrentClockTime;

			// printf( "Message opacity %.2f mess %s\n" , messageTransparency , ScreenMessage );
		}

		// if message brightness becomes null, the message is erased
		if (messageTransparency <= 0.0) {
			ScreenMessage[0] = 0;
			messageTransparency = 0.0;
			LastDecayTime = CurrentClockTime;
		}
		printOglError( 300 );

		// printf( "Window %s\n" , CurrentWindow->id );
		pg_screenMessage_update();

#if defined (TVW)
		// updates message alpha
		// only for the terrain vagues scenes
		if (pg_CurrentScene > 0 && pg_CurrentScene < pg_NbScenes - 1) {
			if (DisplayText1Front) {
				// increase text1 alpha up to 1 and text2 alpha down to 0 (cross fading text2 -> text1) according to text_swap_duration (1)
				DisplayText1Alpha = std::min(1.0f, (CurrentClockTime - DisplayTextSwapInitialTime));
				DisplayText2Alpha = 1.0f - DisplayText1Alpha;
			}
			else {
				// increase text2 alpha up to 1 and text1 alpha down to 0 (cross fading text1 -> text2)
				DisplayText2Alpha = std::min(1.0f, (CurrentClockTime - DisplayTextSwapInitialTime));
				DisplayText1Alpha = 1.0f - DisplayText2Alpha;
			}
			// printf("display text alpha %.2f %.2f front1 (true/false) %d\n", DisplayText1Alpha, DisplayText2Alpha, int(DisplayText1Front));

			// updates image swapping
			update_image_buffer_swapping();
		}
#endif

#ifdef CRITON
		if (MAX_OSC_ARGUMENTS < 3 * 8) {
			std::cout << "Error: unsufficient MAX_OSC_ARGUMENTS value for processing fftLevel8 command!" << std::endl;
			exit(-1);
		}
#endif

		// updates diaporama
		if (photo_diaporama >= 0 && photo_diaporama < nb_photo_albums) {
			// printf("pg_update_diaporama\n");
			pg_update_diaporama();
		}

		printOglError(467);

		if (trace_output_frame_number) {
			printf("Frame #%d\n", pg_FrameNo);
		}
		// fprintf( fileLog , "Frame #%d\n" , pg_FrameNo );
		// last image No display
		if (pg_FrameNo > last_frame_number) {
			printf("Reached Final Frame #%d\n", last_frame_number);
			exit(0);
		}

		// beat in case of internal beat
		if (auto_beat && CurrentClockTime > lastBeatTime + auto_beat_duration) {
			float arguments[MAX_OSC_ARGUMENTS];
			memset(arguments, 0, MAX_OSC_ARGUMENTS * sizeof(float)); // make sure the struct is empty
			arguments[0] = (float)BeatNo;
			BeatNo++;
			lastBeatTime += auto_beat_duration;
			if (lastBeatTime > CurrentClockTime + auto_beat_duration
				|| lastBeatTime < CurrentClockTime) {
				lastBeatTime = CurrentClockTime;
			}
			char msg[] = "beat";
			char arg[] = "";
			pg_aliasScript(msg, arg, arguments);
		}

		// continous flashes
		// in case the flash frequency (flashPixel_freq, flashTrkCA_freq_0, flashCABG_freq)
		// is greater than PG_LOOP_SIZE, whatever the value, flashes are emitted at every frame
		pg_continuous_flahes();

		// fprintf( fileLog, "%.10f popEvents Time #\n" , RealTime() );
		// internal and external event processing

		// sends the color IDS during the first frames
		if (pg_FrameNo == first_frame_number) {
			// sends color presets
			InterfaceInitializations();
		}

		for (int ind = 0; ind < nb_IP_Servers; ind++) {
			IP_Servers[ind]->receiveIPMessages();
		}
		for (int ind = 0; ind < nb_IP_Clients; ind++) {
			IP_Clients[ind]->sendIPmessages();
		}
	}

	// defines the global variables associated with the current window
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
	window_height = height;
	window_width = width;
	// glutPositionWindow(window_x + doubleWindowWidth - width, window_y);
	glutReshapeWindow(window_width, window_height);

	//  printf("reshape main win %d %d %d\n" , window_width , window_height , width , height );
	printf( "Resize Window %dx%d\n" , window_width , window_height );

	setWindowDimensions();
	pg_initRenderingMatrices();
}


