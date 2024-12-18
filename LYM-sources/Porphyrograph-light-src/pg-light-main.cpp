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

#if defined(_WIN32)
// only for VC++
#include<crtdbg.h>
#endif

#include "pg-light-all_include.h"

#ifndef _WIN32
///  should only be included once
#define GLH_EXT_SINGLE_FILE
#endif

///////////////////////////////////////////////////////////////
// global variables
///////////////////////////////////////////////////////////////

// displayed window
pg_WindowData *pg_CurrentWindow = NULL;
bool           pg_windowDisplayed = false;

int			   pg_NbConfigurations = 1;
unsigned int pg_ind_scenario = 0;

// the width of the working window
// half of the display in case of double window (doubling is made in the Master shader in this case)
// full display otherwise
int pg_workingWindow_width = 1920;
int pg_workingWindow_width_powerOf2 = 2048;
float pg_workingWindow_width_powerOf2_ratio = 1.f;
bool pg_muteRightScreen = false;

// the smallest powers of 2 for width and height
int pg_window_width_powerOf2 = 2048;
int pg_window_height_powerOf2 = 2048;
float pg_window_width_powerOf2_ratio = 1.f;
float pg_window_height_powerOf2_ratio = 1.f;

int pg_rightWindowVMargin = 0;

/// screen message drawing
GLfloat pg_messageTransparency;
bool pg_newScreenMessage = false;

/// Error string
char *pg_errorStr = NULL;

// Input network message string
char *pg_input_message_string = NULL;
char *pg_input_message_local_command_string = NULL;
char *pg_output_message_string = NULL;

/// OpenGL background color
float pg_OpenGLBGcolor[4] = {0.0, 0.0, 0.0, 1.0};

/// number of frames displayed
int pg_nbFramesDisplayed = 0;
/// current internal time for animations
double pg_CurrentClockTime = 0.;
double pg_PrecedingClockTime = 0.;
/// initial time (in case of real timescale)
double pg_InitialRealTime = 0.;
/// last frame wall time
double pg_LastScreenMessageDecayTime = 0.;
int pg_LastDisplayFrame = 0;
double pg_LastDisplayFrameTime = 0;
int pg_FramePerSecond = 0;
bool pg_DisplayFramePerSecond = false;

/// current mouse location
int pg_CurrentCursorPos_x[PG_NB_CURSORS_MAX] = { PG_OUT_OF_SCREEN_CURSOR }, pg_CurrentCursorPos_y[PG_NB_CURSORS_MAX] = { PG_OUT_OF_SCREEN_CURSOR };
int pg_Pulsed_CurrentCursorPos_x[PG_NB_CURSORS_MAX] = { PG_OUT_OF_SCREEN_CURSOR }, pg_Pulsed_CurrentCursorPos_y[PG_NB_CURSORS_MAX] = { PG_OUT_OF_SCREEN_CURSOR };
float pg_LastCursorPositionUpdate[PG_NB_CURSORS_MAX] = { -1.f };

int pg_CurrentStylusHooverPos_x, pg_CurrentStylusHooverPos_y;
int pg_CurrentStylus_StylusvsRubber = pg_Stylus;
// current tablet pen pressure and orientation
float pg_CurrentStylusPresuse = 0.0f;
float pg_CurrentStylusAzimut = 0.0f;
float pg_CurrentStylusInclination = 0.0f;

/// log file
FILE    *pg_csv_log_file;

// date string for output files
std::stringstream  pg_date_stringStream;

// frame number 
long      pg_FrameNo;
long      pg_ConfigurationFrameNo;
// frame number when to reach particle target
long      pg_ParticleTargetFrameNo;
// last frame with camera parameter change
long      pg_LastCameraParameterChange_Frame;

// +++++++++++++++++++++++ Aux Var +++++++++++++++++++++++++++++++++
char pg_AuxString[1024];

int argc; char **argv;

/// MAIN FUNCTION
int main(int argcMain, char** argvMain) {
	argc = argcMain;
	argv = argvMain;

	// error message and input buffer memory
	pg_errorStr = new char[1024];

	// current date/time based on current system
	time_t now = time(0);
	tm* ltm = localtime(&now);

	// initialisation of pg_cwd
	pg_cwd = pg_GetCurrentWorkingDir();

	// print various components of tm structure.
	//std::cout << "Year: "<< 1900 + ltm->tm_year << std::endl;
	//std::cout << "Month: "<< 1 + ltm->tm_mon<< std::endl;
	//std::cout << "Day: "<<  ltm->tm_mday << std::endl;
	//std::cout << "Time: "<< 1 + ltm->tm_hour << ":";
	//std::cout << 1 + ltm->tm_min << ":";
	//std::cout << 1 + ltm->tm_sec << std::endl;

	pg_date_stringStream
		<< std::setfill('0') << std::setw(2) << 1 + ltm->tm_mon << "-"
		<< std::setfill('0') << std::setw(2) << ltm->tm_mday << "-"
		<< std::setfill('0') << std::setw(2) << (1900 + ltm->tm_year) << "_"
		<< std::setfill('0') << std::setw(2) << ltm->tm_hour << "-"
		<< std::setfill('0') << std::setw(2) << ltm->tm_min << "-"
		<< ltm->tm_year - 100 << std::flush;

	// new window
	pg_CurrentWindow = new pg_WindowData();

	// print all command line arguments
	std::cout << "name of program: " << argv[0] << '\n';
	for (int argcount = 1; argcount < argc; argcount++) {
		std::cout << argv[argcount] << "\n";
	}

	// cyclic input buffer
	// loading the configuration file
	// the configuration file name must be terminated by ".csv" or ".txt"
	// possibly the first argument with such an extension
	pg_NbConfigurations = int(argc - 1);
	if (pg_NbConfigurations > PG_MAX_SCENARIOS) {
		sprintf(pg_errorStr, "Porphyrograph: should have maximally %d associated configuration and scenario file(s), not %d (<conf.csv> <scenario.csv>)+", PG_MAX_SCENARIOS, (argc - 1) / 2); pg_ReportError(pg_errorStr); throw(6678);
	}
	else {
		printf("Porphyrograph: %d associated scenario file(s) (<scenario.csv>)\n", (argc - 1));
	}
	pg_ScenarioFileNames = new string[pg_NbConfigurations];

	for (int ind = 1; ind < argc; ind++) {
		if (strstr(argv[ind], ".csv")) {
			pg_ScenarioFileNames[(ind - 1)] = string(argv[ind]);
			pg_LoadScenarioFile(argv[ind], (ind - 1));
		}
	}

	///////////////////////////////////////////////
	// GLUT display library

	// scene initialization
	pg_init_scene();

	// Version number
	string applicationName = "Porphyrograph  (" + string(project_name) + ")";
	fprintf(pg_csv_log_file, "%s\n", applicationName.c_str());
	pg_logFirstLineSceneVariables();
	printf("%s\n", applicationName.c_str());

	std::cout << "Date: [" << pg_date_stringStream.str() << "]" << std::endl;

	// printf( "Glut initialization\n" );
	// glut parameters initialization 
	glutInit(&argc, argv);
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_activeMeshes]) {
		glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_STENCIL | GLUT_BORDERLESS | GLUT_DEPTH); // 
	}
	else {
		glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_STENCIL | GLUT_BORDERLESS); // 
	}

	pg_initGLutWindows();
	pg_printOglError(474);

	// cursor shape selection
	pg_CursorInit();
	pg_printOglError(475);

	// initializations before rendering

	// initialization of all the array variables from full scenario with scenario initial values
	pg_FullScenarioArrayVarInit();

	// INITIALIZES ALL SCENARIO VARIABLES AND ASSIGNS THEM THE VALUES OF THE FIRST SCENARIO LINE
	pg_initializeScenearioVariables();

	// GUI DISPLAY & LOG FILE LOGGING
	// GUI initialization
	pg_displaySceneVariables();

	// applies all callbacks
	pg_initializationCallBacks();
	pg_printOglError(467);

	// connects PD to porphyrograph
	pg_send_message_udp((char*)"i", (char*)"/connect 1", (char*)"udp_PD_send");
	pg_printOglError(37);

	/////////////////////////////////////////////////////////////////////////
	// USB INITIALIZATION
	//pg_init_USB();
	//pg_find_USB_pedals();

	// main loop for event processing and display
	glutMainLoop();

	return 0;

	// end of glut display library
	///////////////////////////////////////////////
}


void pg_initGLutWindows(void) {
	printf("GLUT size %dx%d\n", PG_WINDOW_WIDTH, PG_WINDOW_HEIGHT);

	glutInitWindowSize(PG_WINDOW_WIDTH, PG_WINDOW_HEIGHT);
	glutInitWindowPosition(window_x, window_y);

	pg_CurrentWindow->glutID = glutCreateWindow(" ");

	// window resize
	printf("Window size %dx%d\n", PG_WINDOW_WIDTH, PG_WINDOW_HEIGHT);
	glutReshapeWindow(PG_WINDOW_WIDTH, PG_WINDOW_HEIGHT);

	// glutFullScreen();                      // Switch into full screen

	///////////////////////////////////////////////
	// OpenGL extended
#if defined(WIN32_EXTENDED)
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
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
													   //Or better yet, use the GL3 way to get the version number
	int OpenGLVersion[2];
	glGetIntegerv(GL_MAJOR_VERSION, &OpenGLVersion[0]);
	glGetIntegerv(GL_MINOR_VERSION, &OpenGLVersion[1]);
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s (%d-%d)\n", version, OpenGLVersion[0], OpenGLVersion[1]);

	int maxTextureSize;
	int maxGeomVert;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
	glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_VERTICES, &maxGeomVert);
	GLint maxAttach = 0;
	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxAttach);
	GLint maxDrawBuf = 0;
	glGetIntegerv(GL_MAX_DRAW_BUFFERS, &maxDrawBuf);
	//printf ("Max texture size %d geometry shader vertices %d max attach %d max draw buffs %d\n", 
	   // maxTextureSize, maxGeomVert, maxAttach, maxDrawBuf);
	GLint maxFragUnif = 0;
	glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, &maxFragUnif);
	//printf("Max uniform components %d\n",
	   // maxFragUnif);


	// window selection
	// glut win activation
	glutSetWindow(pg_CurrentWindow->glutID);

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// callback procedures for window reshape
	glutReshapeFunc(&pg_window_reshape);
	// keyboard events handling
	glutKeyboardFunc(&pg_window_key_browse);
	// special keys handling
	glutSpecialFunc(&pg_window_special_key_browse);

#if defined(PG_WACOM_TABLET)
	// wacom tablet handling
	glutWacomTabletCursorFunc(&pg_window_PG_WACOM_TABLET_browse);
#else  // mouse clicks handling
	glutMouseFunc(&pg_window_mouseFunc_browse);
	// mouse drags handling
	glutMotionFunc(&pg_window_motionFunc_browse);
	// passive selection
	//if( pg_CurrentWindow->mouse_passive_selection ) {
	glutPassiveMotionFunc(&pg_window_passiveMotionFunc_browse);
#endif

	// window visibility
	glutVisibilityFunc(NULL);
	// idle update
	glutTimerFunc(int(minimal_interframe_latency * 1000),
		&pg_window_idle_browse, pg_nbFramesDisplayed);
	glutDisplayFunc(&pg_window_display);

}

void  pg_init_screen_message( void ) {
}


void pg_CursorInit( void ) {
  glutSetCursor(GLUT_CURSOR_NONE);
}

void pg_init_scene(void) {
	if (argc <= 1) {
		sprintf(pg_errorStr, "Porphyrograph: not enough arguments"); pg_ReportError(pg_errorStr);
		sprintf(pg_errorStr, "Usage: Porphyrograph [configuration.conf]"); pg_ReportError(pg_errorStr); throw 100;
	}

	// log file
	if (!pg_csv_file_name.empty()) {
		printf("Open csv log file %s\n", pg_csv_file_name.c_str());
		if ((pg_csv_log_file = fopen(pg_csv_file_name.c_str(), "wb")) == NULL) {
			sprintf(pg_errorStr, "File %s not opened!", pg_csv_file_name.c_str()); pg_ReportError(pg_errorStr); throw 152;
		}
	}
	else {
		pg_csv_log_file = stdout;
	}

	///////////////////////////////////////////////
	// GLOBAL VARIABLES INITIALIZATION
	// frame number initialization
	pg_FrameNo = first_frame_number - 1;
	pg_ConfigurationFrameNo = pg_FrameNo;
	pg_ParticleTargetFrameNo = pg_FrameNo;

	// last camera change initialization
	pg_LastCameraParameterChange_Frame = pg_FrameNo;

	// intial real time
	pg_InitialRealTime = pg_RealTime() + initial_time;

	// initial scenario time: well before current time to be over
	pg_InitialScenarioTime = pg_InitialRealTime - 1000000.f;
	pg_AbsoluteInitialScenarioTime = pg_InitialRealTime - 1000000.f;

	// current time initialization: internal time
	pg_CurrentClockTime = pg_RealTime();
	pg_PrecedingClockTime = pg_CurrentClockTime;

	//printf("Initial time %.2f (real time %.5f)\n", pg_CurrentClockTime, pg_InitialRealTime);

	// random seed initialization
	srand(clock());

	// ------ screen message initialization  ------------- //
	pg_init_screen_message();

#if defined(var_Novak_flight_on)
	// ------ flight initialization  ------------- //
	Novak_flight_init_control_points();
#endif

	if (pg_FullScenarioActiveVars[pg_ind_scenario][_light_color]) {
		pg_DMX_light_initialization();
	}

	/////////////////////////////////////////////////////////////////////////
	// UDP INITIALIZATION
#if defined(WIN32)
	WSADATA wsaData;
	// Initialize Winsock
	int err = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (err != 0) {
		/* Tell the user that we could not find a usable */
		/* WinSock DLL.                                  */
		sprintf(pg_errorStr, "could not find a usable WinSock DLL!"); pg_ReportError(pg_errorStr); throw 207;
	}
#endif

	// server initialization
	for (pg_IPServer &server : pg_IP_Servers) {
		server.InitServer();
	}
	// client initialization
	for (pg_IPClient &client : pg_IP_Clients) {
		// printf( "Client %d initialization\n" , ind );
		  //std::cout << "client->Remote_server_IP: " << ind << " " << client->Remote_server_IP << "\n";
		  //std::cout << "client->Remote_server_port: " << client->Remote_server_port << "\n";
		client.InitClient();
		// printf( "Client name %s\n" , pg_IP_Clients[ ind ]->id );
	}
	pg_input_message_string = new char[PG_MAX_NETWROK_MESSAGE_LENGTH];
	pg_input_message_local_command_string = new char[PG_MAX_NETWROK_MESSAGE_LENGTH];
	pg_output_message_string = new char[PG_MAX_NETWROK_MESSAGE_LENGTH];
	// printf( "End of scene initialization\n" );
}

bool pg_shutdown = false;

void pg_quit(void) {
	//  save the svg paths before quitting (could perhaps be generalized)
	pg_draw_scene(pg_enum_render_Svg);

	// script only available for WIN32
#ifdef _WIN32
	// script
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_activeClipArts]) {
		if (pg_pi_script_1.hProcess != NULL) {
			TerminateProcess(pg_pi_script_1.hProcess, 0);
		}
	}
#endif

	// lights off the LED
	//pg_send_message_udp((char *)"f", (char *)"/launch 0", (char *)"udp_TouchOSC_send");

#if defined(PG_RENOISE)
	sprintf(pg_AuxString, "/renoise/transport/stop"); pg_send_message_udp((char*)"", pg_AuxString, (char*)"udp_RN_send");
#endif
	// connects PD to porphyrograph
	pg_send_message_udp((char*)"i", (char*)"/disconnect 1", (char*)"udp_PD_send");
	pg_send_message_udp((char*)"f", (char*)"/quit 1", (char*)"udp_TouchOSC_send");

	// lights out the LEDs
	//pg_send_message_udp((char *)"i", (char *)"/switchOff_LEDs 1", (char *)"udp_TouchOSC_send");

	// sends all the remaining messages
	for (pg_IPClient& client : pg_IP_Clients) {
		client.sendIPmessages();
	}

	// stores the shutdown status 
  // in order to avoid recalling itself and infinite recursion
	if (pg_shutdown) {
		return;
	}
	else {
		pg_shutdown = true;
	}

	printf("Last Frame #%d\n", pg_FrameNo);

	if (pg_csv_log_file != stdout) {
		fclose(pg_csv_log_file);
	}

	// release global parameters
	// release the common variables in PG_EnvironmentNode. (Must be relased before releaseSceneObjects()
	// printf("releasewindows\n");
	delete pg_CurrentWindow;
	pg_CurrentWindow = NULL;

	// printf("releaseUDP\n");
	pg_IP_Servers.clear();

	pg_IP_Clients.clear();

	if (pg_errorStr) {
		delete[] pg_errorStr;
		pg_errorStr = NULL;
	}

	exit(1);
}

void pg_window_key_browse(unsigned char key, int x, int y)
{
  switch (key) {
  case KEY_ESC:
    pg_quit();
    break;
  // default: use standard key processing (non-platform dependent)
  default:
    pg_process_key( (int)key );
    break;
  }
}

void pg_window_special_key_browse(int key, int x, int y) {
  // use standard key processing (non-platform dependent)
  pg_process_special_key( key );
}

void pg_MouseCoordinatesRemapping(int x, int y, int *mappedX, int *mappedY) {
	*mappedX = x;
	*mappedY = y;
}

#if defined(PG_WACOM_TABLET)
void pg_window_PG_WACOM_TABLET_browse(int x, int y, float press, float az, float incl, int twist, int cursor) {
	int mappedX = x;
	int mappedY = y;

	//printf("pg_window_PG_WACOM_TABLET_browse xy %d %d press; %.2f cursor: %d\n", x, y, press, cursor);

	pg_MouseCoordinatesRemapping(x, y, &mappedX, &mappedY);

	//printf("remapped xy %d %d\n", mappedX, mappedY);

	// drawing
	if (press > 0.05 && mappedX >= 2 && mappedX < pg_workingWindow_width - 2 && mappedY >= 2 && mappedY < PG_WINDOW_HEIGHT - 2) {
		//printf("pen writing %d %d\n", mappedX, mappedY);
		pg_CurrentCursorPos_x[0] = mappedX;
		pg_CurrentCursorPos_y[0] = mappedY;

		// visual feedback on a drawing interface on the tablet, if there is one
		float x_tab = float(pg_CurrentCursorPos_x[0]) / pg_workingWindow_width;
		float y_tab = float(pg_CurrentCursorPos_y[0]) / PG_WINDOW_HEIGHT;
		sprintf(pg_AuxString, "/pen_xy %.2f %.2f", x_tab, y_tab); pg_send_message_udp((char*)"ff", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");

		if (pg_CurrentStylus_StylusvsRubber == pg_Stylus || pg_CurrentStylus_StylusvsRubber == pg_Rubber) {
			pg_CurrentStylusHooverPos_x = PG_OUT_OF_SCREEN_CURSOR;
			pg_CurrentStylusHooverPos_y = PG_OUT_OF_SCREEN_CURSOR;
		}
		else {
			pg_CurrentStylusHooverPos_x = mappedX; 
			pg_CurrentStylusHooverPos_y = mappedY;
		}
	}	
	// hoovering
	else {
		// standard hoovering
		//printf("pen hoovering %d %d\n", mappedX, mappedY);
		pg_CurrentCursorPos_x[0] = PG_OUT_OF_SCREEN_CURSOR;
		pg_CurrentCursorPos_y[0] = PG_OUT_OF_SCREEN_CURSOR;
		pg_CurrentStylusHooverPos_x = mappedX; //  int(x / 1024. * pg_workingWindow_width);
		pg_CurrentStylusHooverPos_y = mappedY; // int(y / 768. * PG_WINDOW_HEIGHT);
	}

	pg_CurrentStylusPresuse = press;
	pg_CurrentStylusAzimut = az;
	pg_CurrentStylusInclination = incl;
	//printf("press %.2f az %.2f incl %.2f cursor %d\n", press, az, incl, cursor);

	// verify these values received from wacom tablet
	if (cursor == 1 || cursor == 7) {
		pg_CurrentStylus_StylusvsRubber = pg_Stylus;
	}
	else if (cursor == 2 || cursor == 8) {
		pg_CurrentStylus_StylusvsRubber = pg_Rubber;
	}
	//else if (cursor == 7) { // tablette enfants
	//	// use the interface to switch between stylus and rubber
	//}
	//printf("pg_window_PG_WACOM_TABLET_browse xy %d %d cursor: %d\n", x, y, pg_CurrentStylus_StylusvsRubber);


	//printf("Porphyrograph pt [%d,%d] press %.3f az %.3f alt %.3f twist %d\n" , 
		  //x, y, press, az, incl, twist );
}
#else
void pg_window_mouseFunc_browse(int button, int state, int x, int y) {
	 printf( "click button %d (%d,%d)\n" , button , x , y );
	int mappedX = x;
	int mappedY = y;

	pg_MouseCoordinatesRemapping(x, y, &mappedX, &mappedY);

	pg_CurrentCursorPos_x[0] = mappedX;
	pg_CurrentCursorPos_y[0] = mappedY;
	pg_CurrentStylusHooverPos_x = mappedX;
	pg_CurrentStylusHooverPos_y = mappedY;
	if (button == 0 || button == 7) {
		pg_CurrentStylus_StylusvsRubber = pg_Stylus;
	}
	else if (button == 2) {
		pg_CurrentStylus_StylusvsRubber = pg_Rubber;
	}
}

void pg_window_motionFunc_browse(int x, int y) {
	 printf( "active button (%d,%d)\n" , x , y );
	pg_CurrentCursorPos_x[0] = x;
	pg_CurrentCursorPos_y[0] = y;
	pg_CurrentStylusHooverPos_x = x;
	pg_CurrentStylusHooverPos_y = y;
}

void pg_window_passiveMotionFunc_browse(int x, int y) {
	 printf( "passive button (%d,%d)\n" , x , y );
    pg_CurrentCursorPos_x[0] = PG_OUT_OF_SCREEN_CURSOR;
    pg_CurrentCursorPos_y[0] = PG_OUT_OF_SCREEN_CURSOR;
    pg_CurrentStylusHooverPos_x = x;
    pg_CurrentStylusHooverPos_y = y;
  // freeglut (PG): glutGetModifiers() called outside an input callback/int
}
#endif

void pg_window_idle_browse( void ) {
  pg_window_idle_browse( 0 );
}

void pg_window_idle_browse(int step) {
	 //printf("begin pg_window_idle_browse\n");
	// fprintf( pg_csv_log_file, "%.10f begin Time #\n" , pg_RealTime() );
	//printf("cameraWeight %.3f\n", cameraWeight);

	// -------------------- idle function recall ------------------------- //
	glutTimerFunc(int(minimal_interframe_latency * 1000),
		&pg_window_idle_browse, pg_nbFramesDisplayed);

	pg_nbFramesDisplayed = step;

	// -------------------- scene update ------------------------- //
	// update is only made if at least window is displayed
	if (pg_windowDisplayed) {
		pg_windowDisplayed = false;

		// frame count
		pg_FrameNo++;
		pg_ConfigurationFrameNo++;
		if (trace_output_frame_number) {
			//printf(" Frame %d/%d\n", pg_FrameNo, pg_ConfigurationFrameNo);
		}

		// target for particles is reached
		if (pg_FrameNo == pg_ParticleTargetFrameNo) {
			// printf(" End of image target\n");
		}

		// resends variables after a while at the beginning 
		if (pg_FrameNo == 100) {
			pg_resend_all_variables = true;
			pg_resend_all_light_variables = true;
			// sends color presets
			pg_InterfaceInitializations();
		}


		// current clock time
		pg_PrecedingClockTime = pg_CurrentClockTime;
		pg_CurrentClockTime = pg_RealTime();
		if (trace_time) {
			printf(" Time #%.2f\n", pg_CurrentClockTime);
		}

		// GUI DISPLAY & LOG FILE LOGGING
		pg_displaySceneVariables();

		// FPS computation
		if (pg_FrameNo % 100 == 0 && pg_CurrentClockTime != pg_LastDisplayFrameTime) {
			pg_FramePerSecond = (int)((pg_FrameNo - pg_LastDisplayFrame) / (pg_CurrentClockTime - pg_LastDisplayFrameTime));
			pg_LastDisplayFrame = pg_FrameNo;
			pg_LastDisplayFrameTime = pg_CurrentClockTime;
			char mess[50];

			if (pg_DisplayFramePerSecond) {
				sprintf(mess, "%d FPS", pg_FramePerSecond);
			}
		}

		pg_printOglError(300);

		// last image No display
		if (pg_FrameNo > last_frame_number) {
			printf("Reached Final Frame #%d\n", last_frame_number);
			exit(0);
		}

		// beat in case of internal beat
		if (auto_beat && pg_CurrentClockTime > pg_lastBeatTime + double(auto_beat_duration)) {
			float arguments[PG_MAX_OSC_ARGUMENTS];
			pg_BeatNo++;
			pg_lastBeatTime += double(auto_beat_duration);
			if (pg_lastBeatTime > pg_CurrentClockTime + double(auto_beat_duration)
				|| pg_lastBeatTime < pg_CurrentClockTime) {
				pg_lastBeatTime = pg_CurrentClockTime;
			}
			memset(arguments, 0, PG_MAX_OSC_ARGUMENTS * sizeof(float)); // make sure the struct is empty
			arguments[0] = (float)pg_BeatNo;
			pg_aliasScript("beat", "f", arguments, 0);
		}

		// pg_audio_pulse in case of internal pg_audio_pulse
		if (auto_pulse) {
			pg_audio_pulse[0] = float(pg_PerlinNoise(pg_seed_pulsePerlinNoise[0], pg_seed_pulsePerlinNoise[1], pg_FrameNo) * sound_volume + sound_min);
			pg_audio_pulse[1] = float(pg_PerlinNoise(pg_seed_pulsePerlinNoise[2], pg_seed_pulsePerlinNoise[3], pg_FrameNo) * sound_volume + sound_min);
			pg_audio_pulse[2] = float(pg_PerlinNoise(pg_seed_pulsePerlinNoise[4], pg_seed_pulsePerlinNoise[5], pg_FrameNo) * sound_volume + sound_min);
			// not used currently  pulse_attack = pg_PerlinNoise(x, y, pg_FrameNo) * sound_volume + sound_min;
			sprintf(pg_AuxString, "/pulse_low %.2f", pg_audio_pulse[0]);
			pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_TouchOSC_send");
			sprintf(pg_AuxString, "/pulse_medium %.2f", pg_audio_pulse[1]);
			pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_TouchOSC_send");
			sprintf(pg_AuxString, "/pulse_high %.2f", pg_audio_pulse[2]);
			pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_TouchOSC_send");

			pg_audio_pulse_average_prec = pulse_average;
			pulse_average = (pg_audio_pulse[0] + pg_audio_pulse[1] + pg_audio_pulse[2]) / 3.f;

			sprintf(pg_AuxString, "/pg_audio_pulse %.2f", pulse_average);
			pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_TouchOSC_send");
			sprintf(pg_AuxString, "/pen_color/color %02x%02x%02xFF", int(pg_pulsed_pen_color[0] * 255), int(pg_pulsed_pen_color[1] * 255), int(pg_pulsed_pen_color[2] * 255));
			pg_send_message_udp((char*)"s", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
			//printf("%s\n", pg_AuxString);
		}

		// UDP messages IN/OUT
		for (pg_IPServer &server : pg_IP_Servers) {
			server.receiveIPMessages();
		}
		for (pg_IPClient &client : pg_IP_Clients) {
			client.sendIPmessages();
		}

		// only for DMX
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_light_color]) {
			if (pg_nb_light_groups[pg_ind_scenario] > 0) {
				// light automation management
				pg_light_automation_update();
				// DMX message management
				if (pg_oneLightGroup_Changed()) {
					pg_StoreDMXValues_AllLightGroups();
					pg_SendDMX();
					pg_Reset_LightGroup_Changed();
					pg_lightGUI_all_values_and_pulse_update();
				}
			}
		}
	}

	// defines the global variables associated with the current window
	// glutSetWindow( pg_CurrentWindow->glutID );

	// -------------------- window redisplay ------------------------- //
	// fprintf( pg_csv_log_file, "%.10f glutPostRedisplay Time #\n" , pg_RealTime() );
	glutPostRedisplay();

	// fprintf( pg_csv_log_file, "%.10f end Time #\n\n" , pg_RealTime() );
	pg_nbFramesDisplayed++;
	 //printf("end pg_window_idle_browse\n");
}

//////////////////////////////////
// window reshape
//////////////////////////////////
void pg_window_reshape(GLsizei width, GLsizei height) {
	glutReshapeWindow(PG_WINDOW_WIDTH, PG_WINDOW_HEIGHT);

	//  printf("reshape main win %d %d %d\n" , PG_WINDOW_WIDTH , PG_WINDOW_HEIGHT , width , height );
	printf( "Resize Window %dx%d\n" , PG_WINDOW_WIDTH , PG_WINDOW_HEIGHT );

	pg_setWindowDimensions();

#ifndef PG_WACOM_TABLET
	printf("************* Version for mouse pointer **********\n");
#else
	printf("************* Version for tablet **********\n");
#endif
}


//////////////////////////////////////////////////////////////
// MISC LIB FUNCTIONS
//////////////////////////////////////////////////////////////
void pg_snapshot(char* type) {
	if (strcmp(type, "svg") == 0) {
		pg_draw_scene(pg_enum_render_Svg);
	}
	else if (strcmp(type, "png") == 0) {
		pg_draw_scene(pg_enum_render_Png);
	}
	else if (strcmp(type, "jpg") == 0) {
		pg_draw_scene(pg_enum_render_Jpg);
	}
	else {
		sprintf(pg_errorStr, "Incorrect screenshot type (%s): expected svg or png or jpg!", type); pg_ReportError(pg_errorStr);
	}
}
