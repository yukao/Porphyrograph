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

// displayed window
pg_Window     *CurrentWindow = NULL;
bool           windowDisplayed = false;

int leftWindowWidth = 1024;
int rightWindowWidth = 1024;
int rightWindowVMargin = 0;
int doubleWindowWidth = 2048;

// OpenGL contexts
HDC MainOpenGLDeviceContext;
HGLRC MainOpenGLRenderingContext;
// additional contexts for threading transfer to GPU
HGLRC SharedOpenGLRenderingContext[PG_MAX_NUMBER_OF_OPENGL_CONTEXTS];
struct ThreadTextureData ThreadOpenGLTextureData[PG_MAX_NUMBER_OF_OPENGL_CONTEXTS];

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
	// the configuration file name must be terminated by ".csv"
	// possibly the first argument with such an extension
	int configurationFileLoaded = false;
	for (int ind = 0; ind < argc - 1; ind++) {
		if (strstr(argv[ind + 1], ".csv") && strstr(argv[ind + 2], ".csv")) {
			char *substr1 = strstr(argv[ind + 1], ".csv");
			char *substr2 = strstr(argv[ind + 2], ".csv");

			if (strcmp(substr1, ".csv") == 0 && strcmp(substr2, ".csv") == 0) {
				LoadConfigurationFile(argv[ind + 1], argv[ind + 2]);
				configurationFileLoaded = true;
				break;
			}
		}
	}
	printf("Configuration file loaded: nb tracks %d\n", PG_NB_TRACKS);

	// or the default conf file name
	if (!configurationFileLoaded) {
		LoadConfigurationFile((char *)("configuration/LYM_" + project_name + "-head-single.csv").c_str(),
			(char *)("scenarios/LYM_" + project_name + "-scenario.csv").c_str());
	}

	///////////////////////////////////////////////
	// GLUT display library

	// printf( "Glut initialization\n" );
	// glut parameters initialization 
	glutInit(&argc, argv);

	// scene initialization
	pg_init_scene();

	// Version number
	string applicationName = "Porphyrograph  (" + project_name + ")";
	fprintf(fileLog, "%s\n", applicationName.c_str());
	pg_logFirstLineSceneVariables();
	printf("%s\n", applicationName.c_str());

	std::cout << "Date: [" << date_stringStream.str() << "]" << std::endl;

	// glut initialization
	initGlutWindows();

	// glut win activation
	glutSetWindow(CurrentWindow->glutID);

	///////////////////////////////////////////////
	// MEMORIZES THE OPENGL CONTEXT OF THE MAIN THREAD
	//MainOpenGLDeviceContext = wglGetCurrentDC();
	//MainOpenGLRenderingContext = wglGetCurrentContext();
	for (int indContext = 0; indContext < PG_MAX_NUMBER_OF_OPENGL_CONTEXTS; indContext++) {
		//	SharedOpenGLRenderingContext[indContext] = wglCreateContext(MainOpenGLDeviceContext);
		//	BOOL error = wglShareLists(SharedOpenGLRenderingContext[indContext], MainOpenGLRenderingContext);
		//	if (error == FALSE) {
		//		printOglError(234);
		//		printf("Destruction of OpenGL context %d\n", indContext);
		//		//Destroy the GL context and just use 1 GL context
		//		wglDeleteContext(SharedOpenGLRenderingContext[indContext]);
		//		SharedOpenGLRenderingContext[indContext] = NULL;
		//	}
		ThreadOpenGLTextureData[indContext].is_free = true;
		ThreadOpenGLTextureData[indContext].loaderContext = NULL;
	}
	//wglMakeCurrent(MainOpenGLDeviceContext, MainOpenGLRenderingContext);
	printOglError(405);

	// OpenGL initialization
	OpenGLInit();
	// cursor shape selection
	CursorInit();

	// window resize
	window_reshape(window_width, window_height);

	// initializations before rendering

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

	// initialisation of particle positions at launch
	printOglError(37);

	// connects PD to porphyrograph
	pg_send_message_udp((char *)"i", (char *)"/connect 1", (char *)"udp_PD_send");

	// INITIALIZES ALL SCENARIO VARIABLES AND ASSIGNS THEM THE VALUES OF THE FIRST SCENARIO LINE
	pg_initializationScript();

	// GUI DISPLAY & LOG FILE LOGGING
	pg_displaySceneVariables();

	// applies all callbacks
	pg_initializationCallBacks();

	// sends color presets
	InterfaceInitializations();

	// lights out the LEDs
	pg_send_message_udp((char *)"i", (char *)"/reset_LEDs 1", (char *)"udp_QT_send");

#ifdef PG_VIDEO_ACTIVE
	// video intialization
	pg_initVideoCameraCapture();
	if (playing_movieNo >= 0 && playing_movieNo < nb_movies) {
		currentlyPlaying_movieNo = playing_movieNo;

		// texture ID initialization (should not be inside a thread)
		if (pg_movie_texture_texID == NULL_ID) {
			glGenTextures(1, &pg_movie_texture_texID);
		}

		is_movieLoading = true;
		printf("Loading %s\n", ("Data/" + project_name + "-data/videos/" + movieFileName[currentlyPlaying_movieNo]).c_str());
		sprintf(AuxString, "/movie_shortName_can %s", movieShortName[currentlyPlaying_movieNo].c_str());
		pg_send_message_udp((char *)"s", AuxString, (char *)"udp_QT_send");

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
			pg_initVideoMoviePlayback, (void *)movieFileName[currentlyPlaying_movieNo]);
		if (rc) {
			std::cout << "Error:unable to create thread pg_initVideoMoviePlayback" << rc << std::endl;
			exit(-1);
		}
		pthread_exit(NULL);
#endif
	}

	printOglError(37);
#endif

	// main loop for event processing and display
	glutMainLoop();

	return 0;

	// end of glut display library
	///////////////////////////////////////////////
}

#ifdef WIN32
/*
BOOL initSC() {
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0, 0, 0, 0);

	return 0;
}
void resizeSC(int width, int height) {
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

//wilkie / opengl - transparency - demo.cpp
//https ://gist.github.com/wilkie/3644466
BOOL CreateHGLRC(HWND hWnd) {
	HGLRC m_hrc;
	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),
		1,                                // Version Number
		PFD_DRAW_TO_WINDOW |         // Format Must Support Window
		PFD_SUPPORT_OPENGL |         // Format Must Support OpenGL
		PFD_SUPPORT_COMPOSITION |         // Format Must Support Composition
		PFD_DOUBLEBUFFER,                 // Must Support Double Buffering
		PFD_TYPE_RGBA,                    // Request An RGBA Format
		32,                               // Select Our Color Depth
		0, 0, 0, 0, 0, 0,                 // Color Bits Ignored
		8,                                // An Alpha Buffer
		0,                                // Shift Bit Ignored
		0,                                // No Accumulation Buffer
		0, 0, 0, 0,                       // Accumulation Bits Ignored
		24,                               // 16Bit Z-Buffer (Depth Buffer)
		8,                                // Some Stencil Buffer
		0,                                // No Auxiliary Buffer
		PFD_MAIN_PLANE,                   // Main Drawing Layer
		0,                                // Reserved
		0, 0, 0                           // Layer Masks Ignored
	};

	HDC hdc = GetDC(hWnd);
	int PixelFormat = ChoosePixelFormat(hdc, &pfd);
	if (PixelFormat == 0) {
		assert(0);
		return FALSE;
	}

	BOOL bResult = SetPixelFormat(hdc, PixelFormat, &pfd);
	if (bResult == FALSE) {
		assert(0);
		return FALSE;
	}

	m_hrc = wglCreateContext(hdc);
	if (!m_hrc) {
		assert(0);
		return FALSE;
	}

	ReleaseDC(hWnd, hdc);	

	HDC hdcMain = GetDC(hWnd);
	wglMakeCurrent(hdcMain, m_hrc);

	initSC();
	resizeSC(1024, 768);
	ReleaseDC(hWnd, hdcMain);

	return TRUE;
}

// Tutorial: OpenGL 3.1 The First Triangle (C++/Win)
// https://www.khronos.org/opengl/wiki/Tutorial:_OpenGL_3.1_The_First_Triangle_(C%2B%2B/Win)
bool CreateGLContext(void)
{
	// HWND hWnd = GetActiveWindow();
	// HWND hWnd = GetForegroundWindow();
	HWND hWnd = NULL;// glutgetWindowHandle();
	if(!hWnd) {
		fprintf(stderr, "Window Handle not Initialized\n");
		return false;
	}
	// printf("Window Handle Initialized %ld\n", (long)hWnd);

	
	//PIXELFORMATDESCRIPTOR pfd;
	//memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
	//pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	//pfd.nVersion = 1;	
	//pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
	//pfd.iPixelType = PFD_TYPE_RGBA;
	//pfd.cColorBits = 32;
	//pfd.cDepthBits = 32;
	//pfd.iLayerType = PFD_MAIN_PLANE;


	DWM_BLURBEHIND bb = { 0 };
	HRGN hRgn = CreateRectRgn(0, 0, -1, -1);
	bb.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION;
	bb.hRgnBlur = hRgn;
	bb.fEnable = TRUE;
	DwmEnableBlurBehindWindow(hWnd, &bb);

	HDC hdc = GetDC(hWnd);
	// HDC hdc = wglGetCurrentDC();

	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),
		1,                                // Version Number
		PFD_DRAW_TO_WINDOW |         // Format Must Support Window
		PFD_SUPPORT_OPENGL |         // Format Must Support OpenGL
		PFD_SUPPORT_COMPOSITION |         // Format Must Support Composition
		PFD_DOUBLEBUFFER,                 // Must Support Double Buffering
		PFD_TYPE_RGBA,                    // Request An RGBA Format
		32,                               // Select Our Color Depth
		0, 0, 0, 0, 0, 0,                 // Color Bits Ignored
		8,                                // An Alpha Buffer
		0,                                // Shift Bit Ignored
		0,                                // No Accumulation Buffer
		0, 0, 0, 0,                       // Accumulation Bits Ignored
		24,                               // 16Bit Z-Buffer (Depth Buffer)
		8,                                // Some Stencil Buffer
		0,                                // No Auxiliary Buffer
		PFD_MAIN_PLANE,                   // Main Drawing Layer
		0,                                // Reserved
		0, 0, 0                           // Layer Masks Ignored
	};

	int nPixelFormat = ChoosePixelFormat(hdc, &pfd);

	if (nPixelFormat == 0) {
		fprintf(stderr, "Error ChoosePixelFormats\n");
		return false;
	}

	BOOL bResult = SetPixelFormat(hdc, nPixelFormat, &pfd);

	if (!bResult) {
		fprintf(stderr, "Error SetPixelFormat\n");
		return false;
	}

	HGLRC tempContext = wglCreateContext(hdc);
	wglMakeCurrent(hdc, tempContext);

	GLenum err = glewInit();
	if (GLEW_OK != err) {
		//  Problem: glewInit failed, something is seriously wrong. 
		fprintf(stderr, "Error GELW not initialized: %s\n", glewGetErrorString(err));
	}
	fprintf(stdout, "GLEW version %s\n", glewGetString(GLEW_VERSION));


	int attribs[] =
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
		WGL_CONTEXT_MINOR_VERSION_ARB, 1,
		WGL_CONTEXT_FLAGS_ARB, 0,
		0
	};

	HGLRC	 m_hrc;                        // OpenGL Rendering Context
	if (wglewIsSupported("WGL_ARB_create_context") == 1)
	{
		m_hrc = wglCreateContextAttribsARB(hdc, 0, attribs);
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(tempContext);
		wglMakeCurrent(hdc, m_hrc);
	}
	else
	{	//It's not possible to make a GL 3.x context. Use the old style context (GL 2.1 and before)
		m_hrc = tempContext;
	}

	if (!m_hrc) {
		fprintf(stderr, "Error wglCreateContextAttribsARB\n");
		return false;
	}

	return true;
}
*/
#endif

void initGlutWindows( void ) {
  glutInitWindowSize( window_width, window_height );
  glutInitWindowPosition( window_x, window_y );
  
  glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_BORDERLESS ); // 
  
  if( double_window ) {
    if( wide_screen ) {
      leftWindowWidth = window_width * 3 / 7;
    }
    else {
      leftWindowWidth = window_width / 2;
    }
    rightWindowWidth = window_width - leftWindowWidth;
    rightWindowVMargin = (rightWindowWidth - leftWindowWidth) / 2;
    doubleWindowWidth = window_width;
  }
  else {
    leftWindowWidth = window_width;
    rightWindowWidth = 0;
    rightWindowVMargin = 0;
    doubleWindowWidth = window_width;
  }

  printf( "Window %dx%d w:%d+%d Margin:%d\n" , 
	    window_width , 
	    window_height , 
	    leftWindowWidth , 
	    rightWindowWidth , 
	    rightWindowVMargin );

  //WNDCLASSEX wc;
  //memset(&wc, 0, sizeof(wc));
  //wc.cbSize = sizeof(WNDCLASSEX);
  //wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
  //wc.style = CS_HREDRAW | CS_VREDRAW;
  //wc.lpfnWndProc = NULL;
  //wc.cbClsExtra = 0;
  //wc.cbWndExtra = 0;
  //wc.hInstance = NULL; // glutgetWindowInstance();
  //wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
  //wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  //wc.hbrBackground = (HBRUSH)CreateSolidBrush(0x00000000);
  //wc.lpszClassName = TEXT("TransparentGL");

  //if (!RegisterClassEx(&wc)) {
	 // MessageBox(NULL, TEXT("RegisterClassEx - failed"), TEXT("Error"), MB_OK | MB_ICONERROR);
	 // return;
  //}

  CurrentWindow->glutID = glutCreateWindow(" ");

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
    fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
  }
  fprintf(stdout, "GLEW version %s\n", glewGetString(GLEW_VERSION));
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
  printf ("Max texture size %d geometry shader vertices %d\n", maxTextureSize, maxGeomVert);
  
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

void pg_init_scene( void ) {
  if( argc <= 1 ) {
      sprintf( ErrorStr , "Porphyrograph: not enough arguments" ); ReportError( ErrorStr );
      sprintf( ErrorStr , "Usage: Porphyrograph [configuration.conf]" ); ReportError( ErrorStr ); throw 100;
  }

  // log file
  if( !log_file_name.empty() ) {
    printf( "Opening log file %s\n" , log_file_name.c_str() ); 
    if ( ( fileLog = fopen(log_file_name.c_str() , "wb" )) == NULL ) {
      sprintf( ErrorStr , "File %s not opened!" , log_file_name.c_str() ); ReportError( ErrorStr ); throw 152;
    }
  }
  else {
    fileLog = stdout;
  }

  ///////////////////////////////////////////////
  // ------ global variables initialization  ------------- //
  // frame number initialization
  pg_FrameNo = first_frame_number - 1;
  pg_targetFrameNo = pg_FrameNo;

  // last camera change initialization
  pg_LastCameraParameterChange_Frame = pg_FrameNo;

  // intial real time
  InitialRealTime = RealTime() + initial_time;

  // initial scenario time: well before current time to be over
  InitialScenarioTime = InitialRealTime - 1000000;
  AbsoluteInitialScenarioTime = InitialRealTime - 1000000;

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
  for( int ind = 0 ; ind < nb_IP_Servers ; ind++ ) {
    IP_Servers[ ind ]->InitServer();
  }
  // client initialization
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

bool pg_shutdown = false;

void quit( void ) {
#ifdef PG_RENOISE
	sprintf(AuxString, "/renoise/transport/stop"); pg_send_message_udp((char *)"", AuxString, (char *)"udp_RN_send");
#endif
	pg_send_message_udp((char *)"f", (char *)"/quit_can 1", (char *)"udp_QT_send");
	pg_send_message_udp((char *)"f", (char *)"/stopsoundtrack 0", (char *)"udp_PD_send");

	// soundtrack off
	pg_send_message_udp((char *)"i", "/soundtrack_onOff 0", (char *)"udp_PD_send");

	// lights out the LEDs
	pg_send_message_udp((char *)"i", (char *)"/switchOff_LEDs 1", (char *)"udp_QT_send");

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

#ifdef PG_VIDEO_ACTIVE
  // release camera
  pg_movie_capture.release();
  // release movie
  pg_camera_capture.release();
#endif

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
void window_PG_WACOM_TABLET_browse(int x, int y, float press, float az, float incl, int twist , int cursor ) {
    if( press > 0 ) {
	CurrentMousePos_x = x;
	CurrentMousePos_y = y;
	if( CurrentCursorStylusvsRubber == pg_Stylus ) {
	  CurrentCursorHooverPos_x = -10000;
	  CurrentCursorHooverPos_y = -10000;
	}
	else {
	  CurrentCursorHooverPos_x = x;
	  CurrentCursorHooverPos_y = y;
	}
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

    // verify these values received from wacom tablet
#ifndef ATELIERS_PORTATIFS
    if( cursor == 1 ) {
#endif
      CurrentCursorStylusvsRubber = pg_Stylus;
#ifndef ATELIERS_PORTATIFS
    }
    else if( cursor == 2 ) {
      CurrentCursorStylusvsRubber = pg_Rubber;
    }
#endif

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

		if (pg_FrameNo == pg_targetFrameNo) {
			// printf(" End of image target\n");
		}

		// current clock time
		CurrentClockTime = RealTime();
		if (trace_time) {
			printf(" Time #%.2f\n", CurrentClockTime);
		}

		//////////////////////////////////////////////////
		// SCENE UPDATE
		// scenario update
		pg_update_scenario();
		printOglError(466);

		// records the current mouse position in the variable
		// and in the tracks is recording is active
		updateMouseEnvironmentVariablesAndTables(CurrentClockTime);

		// ships uniform variables
		pg_update_scene();
		printOglError(51);

		// printf("render frame %d\n", pg_FrameNo);

		// GUI DISPLAY & LOG FILE LOGGING
		pg_displaySceneVariables();
		sprintf(AuxString, "/mute_screen_can %d", int(mute_screen));
		pg_send_message_udp((char *)"i", AuxString, (char *)"udp_QT_send");

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
			messageTransparency -= 0.4f*(float)(CurrentClockTime - LastDecayTime);
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

		// updates diaporama
		if (photo_diaporama >= 0 && photo_diaporama < nb_photo_albums) {
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


  //  printf("reshape main win %d %d %d\n" , window_width , window_height , width , height );
  printf( "Resize Window %dx%d\n" , window_width , window_height );

  if( double_window ) {
    if( wide_screen ) {
      leftWindowWidth = window_width * 3 / 7;
    }
    else {
      leftWindowWidth = window_width / 2;
    }
    rightWindowWidth = window_width - leftWindowWidth;
    rightWindowVMargin = (rightWindowWidth - leftWindowWidth) / 2;
    doubleWindowWidth = window_width;
  }
  else {
    leftWindowWidth = window_width;
    rightWindowWidth = 0;
    rightWindowVMargin = 0;
    doubleWindowWidth = window_width;
  }
}


