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

int tablet_prec_x = 0;
int tablet_prec_y = 0;

// the width of the working window
// half of the display in case of double window (doubling is made in the Master shader in this case)
// full display otherwise
int workingWindow_width = 1024;
int workingWindow_width_powerOf2 = 1024;
float workingWindow_width_powerOf2_ratio = 1.f;
bool muteRightScreen = false;

// the smallest powers of 2 for width and height
int window_width_powerOf2 = 1024;
int window_height_powerOf2 = 1024;
float window_width_powerOf2_ratio = 1.f;
float window_height_powerOf2_ratio = 1.f;

int rightWindowVMargin = 0;

/// screen message drawing
string ScreenMessage;
GLfloat messageTransparency;
bool NewScreenMessage = false;
bool ScreenMessageInitialized = false;

/// Error string
char *ErrorStr = NULL;

// Input network message string
char *Input_Message_String = NULL;
char *Input_Message_Local_Commande_String = NULL;
char *Output_Message_String = NULL;

/// OpenGL background color
float OpenGLBGColor[4] = {0.0, 0.0, 0.0, 1.0};

/// number of frames displayed
int Step = 0;
/// current internal time for animations
double pg_CurrentClockTime = 0.;
double PrecedingClockTime = 0.;
/// initial time (in case of real timescale)
double InitialRealTime = 0.;
/// last image swap
float LastImageSwap = 0.0f;
/// last frame wall time
double LastScreenMessageDecayTime = 0.;
int LastDisplayFrame = 0;
double LastDisplayFrameTime = 0;
int FramePerSecond = 0;
bool DisplayFramePerSecond = false;

/// current mouse location
int CurrentMousePos_x[PG_NB_CURSORS_MAX] = { PG_OUT_OF_SCREEN_CURSOR }, CurrentMousePos_y[PG_NB_CURSORS_MAX] = { PG_OUT_OF_SCREEN_CURSOR };
int Pulsed_CurrentMousePos_x[PG_NB_CURSORS_MAX] = { PG_OUT_OF_SCREEN_CURSOR }, Pulsed_CurrentMousePos_y[PG_NB_CURSORS_MAX] = { PG_OUT_OF_SCREEN_CURSOR };
float LastCursorPositionUpdate[PG_NB_CURSORS_MAX] = { -1.f };

float paths_x_forGPU[PG_NB_CURSORS_MAX] = { PG_OUT_OF_SCREEN_CURSOR }, paths_y_forGPU[PG_NB_CURSORS_MAX] = { PG_OUT_OF_SCREEN_CURSOR };
float paths_x_forGPU_prev[PG_NB_CURSORS_MAX] = { PG_OUT_OF_SCREEN_CURSOR }, paths_y_forGPU_prev[PG_NB_CURSORS_MAX] = { PG_OUT_OF_SCREEN_CURSOR };
float paths_x_forGPU_last_pos_time[PG_NB_CURSORS_MAX] = { -1 };
float paths_x_begin_position[PG_NB_CURSORS_MAX] = { PG_OUT_OF_SCREEN_CURSOR }, paths_y_begin_position[PG_NB_CURSORS_MAX] = { PG_OUT_OF_SCREEN_CURSOR };
int CurrentCursorHooverPos_x, CurrentCursorHooverPos_y;
int CurrentCursorStylusvsRubber = pg_Stylus;
// current tablet pen pressure and orientation
float tabletPressureRadius = 0.0f;
float tabletAzimutRadius = 0.0f;
float tabletInclinationRadius = 0.0f;

/// motion while button is down
float LinearMotionStep = 0;

/// log file
FILE    *pg_csv_log_file;

// date string for output files
std::stringstream  date_stringStream;


// frame number 
int      pg_FrameNo;
int      pg_ConfigurationFrameNo;
// frame number when to reach particle target
int      pg_ParticleTargetFrameNo;
// last frame with camera parameter change
int      pg_LastCameraParameterChange_Frame;

// glut initialized
bool glut_initialized = false;

int argc; char **argv;

/// MAIN FUNCTION
int main(int argcMain, char** argvMain) {
	argc = argcMain;
	argv = argvMain;

	// error message and input buffer memory
	ErrorStr = new char[1024];

	// current date/time based on current system
	time_t now = time(0);
	tm* ltm = localtime(&now);

	// initialisation of cwd
	cwd = GetCurrentWorkingDir();

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

	// new window
	CurrentWindow = new pg_Window();

	// print all command line arguments
	std::cout << "name of program: " << argv[0] << '\n';
	for (int argcount = 1; argcount < argc; argcount++) {
		std::cout << argv[argcount] << "\n";
	}

	// cyclic input buffer
	// loading the configuration file
	// the configuration file name must be terminated by ".csv" or ".txt"
	// possibly the first argument with such an extension
	if (argc % 2 == 1) {
		pg_NbConfigurations = int((argc - 1) / 2);
		if (pg_NbConfigurations > PG_MAX_CONFIGURATIONS) {
			sprintf(ErrorStr, "Porphyrograph: should have maximally %d associated configuration and scenario file(s), not %d (<conf.csv> <scenario.csv>)+", PG_MAX_CONFIGURATIONS, (argc - 1) / 2); ReportError(ErrorStr); throw(6678);
		}
		else {
			printf("Porphyrograph: % d associated configuration and scenario file(s) (<conf.csv> <scenario.csv>) + ", (argc - 1) / 2);
		}
		pg_Shader_File_Names = new string * [pg_NbConfigurations];
		pg_Shader_Stages = new GLenum * *[pg_NbConfigurations];
		pg_Shader_nbStages = new int* [pg_NbConfigurations];
		pg_shader_programme = new unsigned int* [pg_NbConfigurations];
		pg_ConfigurationFileNames = new string[pg_NbConfigurations];
		pg_ScenarioFileNames = new string[pg_NbConfigurations];

		pg_initPaths();

		for (int ind = 1; ind < argc; ind += 2) {
			pg_shader_programme[(ind - 1) / 2] = new unsigned int[_pg_shader_TotalNbTypes];
			for (int ind_shader_type = 0; ind_shader_type < _pg_shader_TotalNbTypes; ind_shader_type++) {
				pg_shader_programme[(ind - 1) / 2][ind_shader_type] = 0;
			}
		}
		for (int ind = 1; ind < argc; ind += 2) {
			if (strstr(argv[ind], ".csv")) {
				pg_ConfigurationFileNames[(ind - 1) / 2] = string(argv[ind]);
				pg_LoadConfigurationFile(argv[ind], (ind - 1) / 2);
			}
		}
		for (int ind = 1; ind < argc; ind += 2) {
			if (strstr(argv[ind + 1], ".csv")) {
				pg_ScenarioFileNames[(ind - 1) / 2] = string(argv[ind + 1]);
				pg_LoadScenarioFile(argv[ind + 1], (ind - 1) / 2);
			}
		}
	}
	else {
		sprintf(ErrorStr, "Porphyrograph: should have an odd number of arguments Porphyrograph.exe (<conf.csv> <scenario.csv>)+"); ReportError(ErrorStr); throw(6678);
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

	std::cout << "Date: [" << date_stringStream.str() << "]" << std::endl;

	// printf( "Glut initialization\n" );
	// glut parameters initialization 
	glutInit(&argc, argv);
#if defined(var_activeMeshes)
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_STENCIL | GLUT_BORDERLESS | GLUT_DEPTH); // 
#else
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_STENCIL | GLUT_BORDERLESS); // 
#endif

	initGlutWindows();
	printOglError(474);

	// OpenGL initialization (clear buffer)
	OpenGLInit();
	// cursor shape selection
	CursorInit();
	printOglError(475);

	// initializations before rendering

	// initialization of all the array variables from full scenario with scenario initial values
	FullScenarioArrayVarInit();

	// sensor initialization
	if (pg_ScenarioActiveVars[pg_current_configuration_rank][_sensor_layout]) {
		SensorInitialization();
	}

#if defined(var_activeMeshes)
	if (pg_ScenarioActiveVars[pg_current_configuration_rank][_activeMeshes]) {
		// mesh initialization
		MeshInitialization();
	}
#endif

#ifdef PG_METAWEAR
	MetawearSensorInitialization();
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
	pg_initFBOTextureImagesAndRendering();
	printOglError(34);

	// textures loading
	pg_loadAllTextures();
	printOglError(36);

	// meshes loading
#if defined(var_activeMeshes)
	pg_loadAllMeshes();
#endif

	// soundtrack listing
	pg_listAllSoundtracks();

	// SVG paths listing
	pg_listAllSVG_paths();

	/////////////////////////////////////////////////////////////////////////
	// ClipArt GPU INITIALIZATION
	// loads the ClipArts and lists them
	if (pg_ScenarioActiveVars[pg_current_configuration_rank][_activeClipArts]) {
		pg_loadAllClipArts();
		pg_listAllClipArts();
	}


	// lights off the LED
	//pg_send_message_udp((char *)"f", (char *)"/launch 0", (char *)"udp_TouchOSC_send");

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
	pg_initializeScenearioVariables();

	// GUI DISPLAY & LOG FILE LOGGING
	// GUI initialization
	pg_displaySceneVariables();

	// LOADS DIAPORAMA
	pg_initDiaporamas();
	pg_loadAllDiaporamas();
#if defined(var_clipCaptFreq)
	pg_ReadInitalClipFramesTextures();
#endif

	// applies all callbacks
	pg_initializationCallBacks();
	printOglError(467);

	// camera frame capture initialization
	if (pg_ScenarioActiveVars[pg_current_configuration_rank][_cameraCaptFreq]) {
		pg_openCameraCaptureAndLoadFrame();
	}

	// video intialization: loads the movie of the intial configuration
	if (pg_ScenarioActiveVars[pg_current_configuration_rank][_movieCaptFreq]) {
		if (playing_movieNo >= 0 && playing_movieNo < int(pg_VideoTracks[pg_current_configuration_rank].size())
			&& playing_movieNo != currentlyPlaying_movieNo) {
			pg_movie_frame.setTo(Scalar(0, 0, 0));

			currentlyPlaying_movieNo = playing_movieNo;

			// texture ID initialization (should not be inside a thread)
			if (pg_movie_texture_texID == NULL_ID) {
				glGenTextures(1, &pg_movie_texture_texID);
			}

			is_movieLoading = true;
			printf("Loading movie %s\n",
					pg_VideoTracks[pg_current_configuration_rank][currentlyPlaying_movieNo].videoFileName.c_str());
			sprintf(AuxString, "/movie_shortName %s", pg_VideoTracks[pg_current_configuration_rank][currentlyPlaying_movieNo].videoShortName.c_str());
			pg_send_message_udp((char*)"s", AuxString, (char*)"udp_TouchOSC_send");

			pg_initVideoMoviePlayback_nonThreaded(&pg_VideoTracks[pg_current_configuration_rank][currentlyPlaying_movieNo].videoFileName);
		}
	}

	// clip intialization for Left Clip
	if (playing_clipNoLeft >= 0 && playing_clipNoLeft < pg_nbClips[pg_current_configuration_rank] && playing_clipNoLeft != pg_clip_status[_clipLeft].getCurrentlyPlaying_clipNo(0)) {
		pg_clip_status[_clipLeft].setCurrentlyPlaying_clipNo(0, playing_clipNoLeft);
		sprintf(AuxString, "/clip_shortName_0 %03d", playing_clipNoLeft);
		pg_send_message_udp((char*)"s", AuxString, (char*)"udp_TouchOSC_send");
	}
#if PG_NB_PARALLEL_CLIPS >= 2
	if (playing_secondClipNoLeft >= 0 && playing_secondClipNoLeft < pg_nbClips[pg_current_configuration_rank] && playing_secondClipNoLeft != pg_clip_status[_clipLeft].getCurrentlyPlaying_clipNo(1)) {
		pg_clip_status[_clipLeft].setCurrentlyPlaying_clipNo(1, playing_secondClipNoLeft);
		sprintf(AuxString, "/clip2_shortName_0 %03d", playing_secondClipNoLeft);
		pg_send_message_udp((char*)"s", AuxString, (char*)"udp_TouchOSC_send");
	}
#endif
		// clip intialization for Right Clip
	if (playing_clipNoRight >= 0 && playing_clipNoRight < pg_nbClips[pg_current_configuration_rank] && playing_clipNoRight != pg_clip_status[_clipRight].getCurrentlyPlaying_clipNo(0)) {
		pg_clip_status[_clipRight].setCurrentlyPlaying_clipNo(0, playing_clipNoRight);
		sprintf(AuxString, "/clip_shortName_1 %03d", playing_clipNoRight);
		pg_send_message_udp((char*)"s", AuxString, (char*)"udp_TouchOSC_send");
	}
#if PG_NB_PARALLEL_CLIPS >= 2
	if (playing_secondClipNoRight >= 0 && playing_secondClipNoRight < pg_nbClips[pg_current_configuration_rank] && playing_secondClipNoRight != pg_clip_status[_clipRight].getCurrentlyPlaying_clipNo(1)) {
		pg_clip_status[_clipRight].setCurrentlyPlaying_clipNo(1, playing_secondClipNoRight);
		sprintf(AuxString, "/clip2_shortName_1 %03d", playing_secondClipNoRight);
		pg_send_message_udp((char*)"s", AuxString, (char*)"udp_TouchOSC_send");
	}
#endif

#ifdef PG_WITH_PUREDATA
	// connects PD to porphyrograph
	pg_send_message_udp((char*)"i", (char*)"/connect 1", (char*)"udp_PD_send");
#endif
	printOglError(37);

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


void initGlutWindows(void) {
	printf("GLUT size %dx%d\n", window_width, window_height);

	glutInitWindowSize(window_width, window_height);
	glutInitWindowPosition(my_window_x, my_window_y);

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
	glutSetWindow(CurrentWindow->glutID);

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
	glutTimerFunc(int(minimal_interframe_latency * 1000),
		&window_idle_browse, Step);
	glutDisplayFunc(&window_display);

}

void  pg_init_screen_message( void ) {
  // screen message initialization
  ScreenMessage = "";
  messageTransparency = 1.0;
  NewScreenMessage = false;
  ScreenMessageInitialized = false;
  LastScreenMessageDecayTime = pg_CurrentClockTime;
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

void pg_init_scene(void) {
	if (argc <= 1) {
		sprintf(ErrorStr, "Porphyrograph: not enough arguments"); ReportError(ErrorStr);
		sprintf(ErrorStr, "Usage: Porphyrograph [configuration.conf]"); ReportError(ErrorStr); throw 100;
	}

	// log file
	if (!pg_csv_file_name.empty()) {
		printf("Open csv log file %s\n", pg_csv_file_name.c_str());
		if ((pg_csv_log_file = fopen(pg_csv_file_name.c_str(), "wb")) == NULL) {
			sprintf(ErrorStr, "File %s not opened!", pg_csv_file_name.c_str()); ReportError(ErrorStr); throw 152;
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
	InitialRealTime = RealTime() + initial_time;

	// initial scenario time: well before current time to be over
	InitialScenarioTime = InitialRealTime - 1000000.f;
	AbsoluteInitialScenarioTime = InitialRealTime - 1000000.f;

	// current time initialization: internal time
	pg_CurrentClockTime = RealTime();
	PrecedingClockTime = pg_CurrentClockTime;

	//printf("Initial time %.2f (real time %.5f)\n", pg_CurrentClockTime, InitialRealTime);

	// random seed initialization
	srand(clock());

	// ------ screen message initialization  ------------- //
	pg_init_screen_message();

	// reads the text messages in the text file
	pg_ReadAllDisplayMessages(pg_MessageFile[pg_current_configuration_rank]);
	
#if defined(var_Novak_flight_on)
	// ------ flight initialization  ------------- //
	Novak_flight_init_control_points();
#endif

#if defined(PG_LIGHTS_DMX_IN_PG)
	DMX_light_initialization();
#endif

#if defined(PG_WITH_PORTAUDIO)
	printf("Open portaudio\n");
	paInit = new ScopedPaHandler();
	if (paInit->result() != paNoError) {
		fprintf(stderr, "Error number: %d\n", paInit->result());
		fprintf(stderr, "Error message: %s\n", Pa_GetErrorText(paInit->result()));
	}
#endif

#ifdef PG_MIDI
	//open_IO_MIDI(string("loopMIDI Port"), string("loopMIDI Port"));
	open_IO_MIDI(string("TouchOSC Bridge"), string("TouchOSC Bridge"));
#endif

	if (pg_ScenarioActiveVars[pg_current_configuration_rank][_sensor_layout]) {
		/////////////////////////////////////////////////////////////////////////
		// SENSORS INITIALIZATION
		// copies the grid layout
		assignSensorPositions();

		// copies the single central activation
		assignSensorActivations();
	}

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
	for (pg_IPServer &server : IP_Servers) {
		server.InitServer();
	}
	// client initialization
	for (pg_IPClient &client : IP_Clients) {
		// printf( "Client %d initialization\n" , ind );
		  //std::cout << "client->Remote_server_IP: " << ind << " " << client->Remote_server_IP << "\n";
		  //std::cout << "client->Remote_server_port: " << client->Remote_server_port << "\n";
		client.InitClient();
		// printf( "Client name %s\n" , IP_Clients[ ind ]->id );
	}
	Input_Message_String = new char[max_network_message_length];
	Input_Message_Local_Commande_String = new char[max_network_message_length];
	Output_Message_String = new char[max_network_message_length];
	// printf( "End of scene initialization\n" );

	//test_hull();
	//exit(0);
}

bool pg_shutdown = false;

void pg_quit( void ) {
	// for Annika performance: save the svg paths before quitting (could perhaps be generalized)
#ifdef KOMPARTSD
	pg_draw_scene(_Svg);
	// sends the position of the cursor to the recorder for later replay
	sprintf(AuxString, "/quit");
	pg_send_message_udp((char *)"", (char *)AuxString, (char*)"udp_Record_send"));
	// sends the new scene to Usine for sample selection
	for (int ind = 0; ind < 4; ind++) {
		sprintf(AuxString, "/new_scene_%d 0", ind);
		pg_send_message_udp((char *)"i", (char *)AuxString, (char*)"udp_Usine_send"));
	}
#endif

	// script
#if defined(var_script_1)
	if (pi_script_1.hProcess != NULL) {
		TerminateProcess(pi_script_1.hProcess, 0);
	}
#endif

	// lights off the LED
	//pg_send_message_udp((char *)"f", (char *)"/launch 0", (char *)"udp_TouchOSC_send");

#ifdef PG_RENOISE
	sprintf(AuxString, "/renoise/transport/stop"); pg_send_message_udp((char *)"", AuxString, (char *)"udp_RN_send");
#endif
#ifdef PG_WITH_PUREDATA
	// connects PD to porphyrograph
	pg_send_message_udp((char*)"i", (char*)"/disconnect 1", (char*)"udp_PD_send");
#endif

#if !defined(LIGHT)
	pg_send_message_udp((char *)"f", (char *)"/quit 1", (char *)"udp_TouchOSC_send");
#endif

	// soundtrack on
	soundTrack_on = false;
#if defined(PG_WITH_PUREDATA)
	pg_send_message_udp((char*)"f", (char*)"/stopsoundtrack 0", (char*)"udp_PD_send");
	sprintf(AuxString, "/soundtrack_onOff %d", soundTrack_on);
	pg_send_message_udp((char*)"i", AuxString, (char*)"udp_PD_send");
	printf("Main: soundtrack: %s\n", AuxString);
#endif
#ifdef PG_WITH_PORTAUDIO
	pa_sound_data.pa_closeMyStream();
	printf("close portaudio\n");
	delete paInit;
#endif
	sprintf(AuxString, "/soundtrack_onOff %d", !soundTrack_on);
	pg_send_message_udp((char*)"i", AuxString, (char*)"udp_TouchOSC_send");
	sprintf(AuxString, "/soundtrack_volume %d", 0);
	pg_send_message_udp((char*)"i", AuxString, (char*)"udp_TouchOSC_send");
#ifdef PG_WITH_JUCE
	// soundtrack off
	pg_send_message_udp((char *)"", (char *)"/JUCE_stop_track", (char *)"udp_SoundJUCE_send");
	pg_send_message_udp((char *)"", (char *)"/JUCE_exit", (char *)"udp_SoundJUCE_send");
#endif

	// lights out the LEDs
	//pg_send_message_udp((char *)"i", (char *)"/switchOff_LEDs 1", (char *)"udp_TouchOSC_send");

#ifdef PG_MIDI
	midi_io.finalise();
#endif

	// sends all the remaining messages
	for (pg_IPClient &client : IP_Clients) {
		client.sendIPmessages();
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

  if( pg_csv_log_file != stdout ) {
    fclose( pg_csv_log_file );
  }

  // release movie
  if (pg_movie_capture.isOpened()) {
	  pg_movie_capture.release();
  }
  // release webCam
  if (pg_webCam_capture.isOpened()) {
	  pg_webCam_capture.release();
  }
  // release IPCam
  for (unsigned int ind_IPCam = 0; ind_IPCam < pg_IPCam_capture.size(); ind_IPCam++) {
	  if (pg_IPCam_capture[ind_IPCam].isOpened()) {
		  pg_IPCam_capture[ind_IPCam].release();
	  }
  }


  //// realse USB
  //pg_release_USB();

  // release global parameters
  // release the common variables in PG_EnvironmentNode. (Must be relased before releaseSceneObjects()
  // printf("releasewindows\n");
  delete CurrentWindow;
  CurrentWindow = NULL;

  // printf("releaseUDP\n");
  IP_Servers.clear();
  
  IP_Clients.clear();
  
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
    pg_quit();
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

void MouseCoordinatesRemapping(int x, int y, int *mappedX, int *mappedY) {
#if defined(TEMPETE)
	//if (!double_window) {
	//printf("pos = %d %d\n", CurrentMousePos_x[0], CurrentMousePos_y[0]);
	// mapping between tablet and target texture position
	// 2 sets of four corners in tablet position are selected (by they x,y coordinates of the pen)
	// the source corners corresponding to the drawing area on the tablet (a rectangle) (X0s,Y0s), (X1s, Y1s)...
	// the target corners corresponding to the drawing area on the screen (a rectangle) (X0t,Y0t), (X1t, Y1t)...
	// the scale is calculated as follows
	// s = ( (X1t-X0t)/(X1s-X0s), (Y3t-Y0t)/(Y3s-Y0s) )
	// and the translation as follows
	// t = (X0t,Y0t) - s * (X0s,Y0s)
	// the final transformation to be applied on the mouse coordinates is
	// CurrentCursorHooverPos_x = sx (x) + tx
	// CurrentCursorHooverPos_y = sy (y) + ty
	//*mappedX = int(x * 1.39f - 451.0f);
	//*mappedY = int(y * 1.336f - 216.5f);
	//*mappedX = int(x * 1.f - 0.0f);
	//*mappedY = int(y * 1.f - 0.0);
	//*mappedX = int(x * 0.7194f + 324.6f);
	//*mappedY = int(y * 0.777f + 146.5f);
	// source 350x0 350x1080      1550x0  1550x1080
	// target 265x15  290x1080       1425x15  1425x1051
	float sx = float(1425 - 290) / (1650 - 250);
	float sy = float(1051 - 15) / (1050 - 0)

	float tx = 265.f - sx * 250;
	float ty = 15.f - sy * 0;
	*mappedX = int(x * sx + tx);
	*mappedY = int(y * sy + ty);
	//* mappedX = int(x);
	//* mappedY = int(y);
	//printf("%d x, %d y, %.2f press, %.2f az, %.2f incl, %.2f twist, %d cursor", x, y, press, az, incl, twist, cursor);
#elif defined(DOUBLE_WIDTH_SCREEN)
	*mappedX = int((x) * 2);
	*mappedY = int(y);
#else
	*mappedX = x;
	*mappedY = y;
#endif
}

#ifdef PG_WACOM_TABLET
void window_PG_WACOM_TABLET_browse(int x, int y, float press, float az, float incl, int twist, int cursor) {
	int mappedX = x;
	int mappedY = y;

	//printf("window_PG_WACOM_TABLET_browse xy %d %d press; %.2f cursor: %d\n", x, y, press, cursor);

	MouseCoordinatesRemapping(x, y, &mappedX, &mappedY);

	//printf("remapped xy %d %d\n", mappedX, mappedY);

	// drawing
	if (press > 0.05 && mappedX >= 2 && mappedX < workingWindow_width - 2 && mappedY >= 2 && mappedY < window_height - 2) {
		//printf("pen writing %d %d\n", mappedX, mappedY);
		CurrentMousePos_x[0] = mappedX;
		CurrentMousePos_y[0] = mappedY;

#if !defined(LIGHT)
		// visual feedback on a drawing interface on the tablet, if there is one
		float x_tab = float(CurrentMousePos_x[0]) / workingWindow_width;
		float y_tab = float(CurrentMousePos_y[0]) / window_height;
		sprintf(AuxString, "/pen_xy %.2f %.2f", x_tab, y_tab); pg_send_message_udp((char*)"ff", (char*)AuxString, (char*)"udp_TouchOSC_send");
#endif

		if (CurrentCursorStylusvsRubber == pg_Stylus || CurrentCursorStylusvsRubber == pg_Rubber) {
			CurrentCursorHooverPos_x = PG_OUT_OF_SCREEN_CURSOR;
			CurrentCursorHooverPos_y = PG_OUT_OF_SCREEN_CURSOR;
		}
		else {
			CurrentCursorHooverPos_x = mappedX; 
			CurrentCursorHooverPos_y = mappedY;
		}
	}	
	// hoovering
	else {
		// standard hoovering
		//printf("pen hoovering %d %d\n", mappedX, mappedY);
		CurrentMousePos_x[0] = PG_OUT_OF_SCREEN_CURSOR;
		CurrentMousePos_y[0] = PG_OUT_OF_SCREEN_CURSOR;
		CurrentCursorHooverPos_x = mappedX; //  int(x / 1024. * workingWindow_width);
		CurrentCursorHooverPos_y = mappedY; // int(y / 768. * window_height);
	}

	tabletPressureRadius = press;
	tabletAzimutRadius = az;
	tabletInclinationRadius = incl;
	//printf("press %.2f az %.2f incl %.2f cursor %d\n", press, az, incl, cursor);

	// verify these values received from wacom tablet
	if (cursor == 1 || cursor == 7) {
		CurrentCursorStylusvsRubber = pg_Stylus;
	}
	else if (cursor == 2 || cursor == 8) {
		CurrentCursorStylusvsRubber = pg_Rubber;
	}
	//else if (cursor == 7) { // tablette enfants
	//	// use the interface to switch between stylus and rubber
	//}
	//printf("window_PG_WACOM_TABLET_browse xy %d %d cursor: %d\n", x, y, CurrentCursorStylusvsRubber);


	//printf("Porphyrograph pt [%d,%d] press %.3f az %.3f alt %.3f twist %d\n" , 
		  //x, y, press, az, incl, twist );
}
#else
void window_mouseFunc_browse(int button, int state, int x, int y) {
	 printf( "click button %d (%d,%d)\n" , button , x , y );
	int mappedX = x;
	int mappedY = y;

	MouseCoordinatesRemapping(x, y, &mappedX, &mappedY);

	CurrentMousePos_x[0] = mappedX;
	CurrentMousePos_y[0] = mappedY;
	CurrentCursorHooverPos_x = mappedX;
	CurrentCursorHooverPos_y = mappedY;
	if (button == 0 || button == 7) {
		CurrentCursorStylusvsRubber = pg_Stylus;
	}
	else if (button == 2) {
		CurrentCursorStylusvsRubber = pg_Rubber;
	}
}

void window_motionFunc_browse(int x, int y) {
	 printf( "active button (%d,%d)\n" , x , y );
	CurrentMousePos_x[0] = x;
	CurrentMousePos_y[0] = y;
	CurrentCursorHooverPos_x = x;
	CurrentCursorHooverPos_y = y;
}

void window_passiveMotionFunc_browse(int x, int y) {
	 printf( "passive button (%d,%d)\n" , x , y );
    CurrentMousePos_x[0] = PG_OUT_OF_SCREEN_CURSOR;
    CurrentMousePos_y[0] = PG_OUT_OF_SCREEN_CURSOR;
    CurrentCursorHooverPos_x = x;
    CurrentCursorHooverPos_y = y;
  // freeglut (PG): glutGetModifiers() called outside an input callback/int
}
#endif

void window_idle_browse( void ) {
  window_idle_browse( 0 );
}

void window_idle_browse(int step) {
	 //printf("begin window_idle_browse\n");
	// fprintf( pg_csv_log_file, "%.10f begin Time #\n" , RealTime() );
	//printf("cameraWeight %.3f\n", cameraWeight);

	// -------------------- idle function recall ------------------------- //
	glutTimerFunc(int(minimal_interframe_latency * 1000),
		&window_idle_browse, Step);

	Step = step;

	// -------------------- scene update ------------------------- //
	// update is only made if at least window is displayed
	if (windowDisplayed) {
		windowDisplayed = false;

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
			resend_all_variables = true;
			resend_all_light_variables = true;
			// sends color presets
			InterfaceInitializations();
		}


		// current clock time
		PrecedingClockTime = pg_CurrentClockTime;
		pg_CurrentClockTime = RealTime();
		if (trace_time) {
			printf(" Time #%.2f\n", pg_CurrentClockTime);
		}

		// GUI DISPLAY & LOG FILE LOGGING
		pg_displaySceneVariables();

		// FPS computation
		if (pg_FrameNo % 100 == 0 && pg_CurrentClockTime != LastDisplayFrameTime) {
			FramePerSecond = (int)((pg_FrameNo - LastDisplayFrame) / (pg_CurrentClockTime - LastDisplayFrameTime));
			LastDisplayFrame = pg_FrameNo;
			LastDisplayFrameTime = pg_CurrentClockTime;
			char mess[50];

			if (DisplayFramePerSecond) {
				sprintf(mess, "%d FPS", FramePerSecond);
				pg_writeMessageOnScreen(string(mess));
				printf("%s\n", ScreenMessage.c_str());
			}
		}

		// decays window message brightness
		if (messageTransparency > 0.0) {
			messageTransparency -= 0.4f * float(pg_CurrentClockTime - LastScreenMessageDecayTime);
			LastScreenMessageDecayTime = pg_CurrentClockTime;

			// printf( "Message opacity %.2f mess %s\n" , messageTransparency , ScreenMessage.c_str() );
		}

		// if message brightness becomes null, the message is erased
		if (messageTransparency <= 0.0) {
			ScreenMessage = "";
			messageTransparency = 0.0;
			LastScreenMessageDecayTime = pg_CurrentClockTime;
		}
		printOglError(300);

		// printf( "Window %s\n" , CurrentWindow->id );
		pg_screenMessage_update();

#ifdef CRITON
		if (PG_MAX_OSC_ARGUMENTS < 3 * 8) {
			std::cout << "Error: unsufficient PG_MAX_OSC_ARGUMENTS value for processing fftLevel8 command!" << std::endl;
			exit(-1);
		}
#endif

#if defined(var_photo_diaporama)
		// updates diaporama
		if (photo_diaporama >= 0 && photo_diaporama < pg_nbCompressedImageDirs[pg_current_configuration_rank]) {
			// printf("pg_update_diaporama\n");
			pg_update_diaporama();
		}
#endif

		printOglError(467);

		// last image No display
		if (pg_FrameNo > last_frame_number) {
			printf("Reached Final Frame #%d\n", last_frame_number);
			exit(0);
		}

		// beat in case of internal beat
		if (auto_beat && pg_CurrentClockTime > lastBeatTime + double(auto_beat_duration)) {
			float arguments[PG_MAX_OSC_ARGUMENTS];
			pg_BeatNo++;
			lastBeatTime += double(auto_beat_duration);
			if (lastBeatTime > pg_CurrentClockTime + double(auto_beat_duration)
				|| lastBeatTime < pg_CurrentClockTime) {
				lastBeatTime = pg_CurrentClockTime;
			}
			memset(arguments, 0, PG_MAX_OSC_ARGUMENTS * sizeof(float)); // make sure the struct is empty
			arguments[0] = (float)pg_BeatNo;
			pg_aliasScript("beat", "f", arguments, 0);
		}

		// pulse in case of internal pulse
		if (auto_pulse) {
			pulse[0] = float(PerlinNoise(seed_pulsePerlinNoise[0], seed_pulsePerlinNoise[1], pg_FrameNo) * sound_volume + sound_min);
			pulse[1] = float(PerlinNoise(seed_pulsePerlinNoise[2], seed_pulsePerlinNoise[3], pg_FrameNo) * sound_volume + sound_min);
			pulse[2] = float(PerlinNoise(seed_pulsePerlinNoise[4], seed_pulsePerlinNoise[5], pg_FrameNo) * sound_volume + sound_min);
			// not used currently  pulse_attack = PerlinNoise(x, y, pg_FrameNo) * sound_volume + sound_min;
#ifdef PG_WITH_SOUND_LEVELS_GUI_FEEDBACK
			sprintf(AuxString, "/pulse_low %.2f", pulse[0]);
			pg_send_message_udp((char*)"f", AuxString, (char*)"udp_TouchOSC_send");
			sprintf(AuxString, "/pulse_medium %.2f", pulse[1]);
			pg_send_message_udp((char*)"f", AuxString, (char*)"udp_TouchOSC_send");
			sprintf(AuxString, "/pulse_high %.2f", pulse[2]);
			pg_send_message_udp((char*)"f", AuxString, (char*)"udp_TouchOSC_send");
#endif

			pulse_average_prec = pulse_average;
			pulse_average = (pulse[0] + pulse[1] + pulse[2]) / 3.f;

#ifdef PG_WITH_SOUND_LEVELS_GUI_FEEDBACK
			sprintf(AuxString, "/pulse %.2f", pulse_average);
			pg_send_message_udp((char*)"f", AuxString, (char*)"udp_TouchOSC_send");
#endif

#if !defined(LIGHT)
			sprintf(AuxString, "/pen_color/color %02x%02x%02xFF", int(pulsed_pen_color[0] * 255), int(pulsed_pen_color[1] * 255), int(pulsed_pen_color[2] * 255));
			pg_send_message_udp((char*)"s", (char*)AuxString, (char*)"udp_TouchOSC_send");
			//printf("%s\n", AuxString);
#endif
		}

		// continous flashes
		// in case the flash frequency (flashPixel_freq, flashTrkCA_freq_0, flashCABG_freq)
		// is equal to (PG_LOOP_SIZE + 1), whatever the value, flashes are emitted at every frame (or on beats if we reach an onset for the video or the movie)
		pg_flash_control(flash_continuous_generation);

		// UDP messages IN/OUT
		for (pg_IPServer &server : IP_Servers) {
			server.receiveIPMessages();
		}
		for (pg_IPClient &client : IP_Clients) {
			client.sendIPmessages();
		}

		// MIDI messages IN
#ifdef PG_MIDI
		event_read = midi_io.read_event(MIDI_IN_event);
		if (event_read) {
			printf("MIDI IN event %d %d %d (time %d)\n", Pm_MessageStatus(MIDI_IN_event.message), Pm_MessageData1(MIDI_IN_event.message), Pm_MessageData2(MIDI_IN_event.message), MIDI_IN_event.timestamp);
		}
#endif

#ifdef PG_LIGHTS_DMX_IN_PG
		if (pg_nb_light_groups[pg_current_configuration_rank] > 0) {
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
#endif
	}

	// defines the global variables associated with the current window
	// glutSetWindow( CurrentWindow->glutID );

	// -------------------- window redisplay ------------------------- //
	// fprintf( pg_csv_log_file, "%.10f glutPostRedisplay Time #\n" , RealTime() );
	glutPostRedisplay();

	// fprintf( pg_csv_log_file, "%.10f end Time #\n\n" , RealTime() );
	Step++;
	 //printf("end window_idle_browse\n");
}

//////////////////////////////////
// window reshape
//////////////////////////////////
void window_reshape(GLsizei width, GLsizei height) {
	//window_height = height;
	//window_width = width;
	// glutPositionWindow(my_window_x + window_width - width, my_window_y);
	glutReshapeWindow(window_width, window_height);

	//  printf("reshape main win %d %d %d\n" , window_width , window_height , width , height );
	printf( "Resize Window %dx%d\n" , window_width , window_height );

	setWindowDimensions();
	pg_initRenderingMatrices();

#ifndef PG_WACOM_TABLET
	printf("************* Version for mouse pointer **********\n");
#else
	printf("************* Version for tablet **********\n");
#endif
}


