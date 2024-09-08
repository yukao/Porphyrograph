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

#include "pg-all_include.h"

#ifndef _WIN32
///  should only be included once
#define GLH_EXT_SINGLE_FILE
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////
// GLOBAL VARS
//////////////////////////////////////////////////////////////////////////////////////////////////////

// displayed window
pg_WindowData *pg_CurrentWindow = NULL;
bool           pg_windowDisplayed = false;

// number of loaded scenarios
int			   pg_NbScenarios = 1;
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

/// Error string
char *pg_errorStr = NULL;

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
int pg_LastDisplayFrame = 0;
double pg_LastDisplayFrameTime = 0;
int pg_FramePerSecond = 0;
bool pg_DisplayFramePerSecond = false;

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

// Perlin noise seed
float pg_seed_pulsePerlinNoise[4 * 2]
= { rand_0_1 * 255, rand_0_1 * 255, rand_0_1 * 255, rand_0_1 * 255,
	rand_0_1 * 255, rand_0_1 * 255, rand_0_1 * 255, rand_0_1 * 255 };

// shutdown status
bool pg_shutdown = false;

int argc; char **argv;

//////////////////////////////////////////////////////////////////////////////////////////////////////
// UTILS
//////////////////////////////////////////////////////////////////////////////////////////////////////

// FUNCTION FOR TESTING THE CURRENT WORKING DIRECTORY
std::string pg_GetCurrentWorkingDir(void) {
	char buff[FILENAME_MAX];
	GetCurrentDir(buff, FILENAME_MAX);
	std::string current_working_dir(buff);
	std::replace(current_working_dir.begin(), current_working_dir.end(), '\\', '/'); // replace all 'x' to 'y'
	return current_working_dir;
}

// CURSOR INITIALIZATION
void pg_CursorInit(void) {
	glutSetCursor(GLUT_CURSOR_NONE);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// QUIT
//////////////////////////////////////////////////////////////////////////////////////////////////////
void pg_quit(void) {
	//  save the svg paths before quitting (could perhaps be generalized)
	pg_draw_scene(pg_enum_render_Svg);

	// script only available for WIN32
#ifdef _WIN32
	// script
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_activeClipArts]) {
		if (pg_pi_script_python.hProcess != NULL) {
			TerminateProcess(pg_pi_script_python.hProcess, 0);
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

	// soundtrack on
	pg_soundTrack_on = false;
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_soundtrack_PD_weight]) {
		pg_send_message_udp((char*)"f", (char*)"/stopsoundtrack 0", (char*)"udp_PD_send");
		sprintf(pg_AuxString, "/soundtrack_onOff %d", pg_soundTrack_on);
		pg_send_message_udp((char*)"i", pg_AuxString, (char*)"udp_PD_send");
	}
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_soundtrack_PA_weight]) {
		printf("Main: soundtrack: %s\n", pg_AuxString);
		pg_pa_closeAudioStream();
		printf("close portaudio\n");
	}
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_soundtrack_JUCE_weight]) {
		// soundtrack off
		pg_send_message_udp((char*)"", (char*)"/JUCE_stop_track", (char*)"udp_script_binary_send");
		pg_send_message_udp((char*)"", (char*)"/JUCE_exit", (char*)"udp_script_binary_send");
	}
	sprintf(pg_AuxString, "/soundtrack_onOff %d", !pg_soundTrack_on);
	pg_send_message_udp((char*)"i", pg_AuxString, (char*)"udp_TouchOSC_send");
	sprintf(pg_AuxString, "/soundtrack_volume %d", 0);
	pg_send_message_udp((char*)"i", pg_AuxString, (char*)"udp_TouchOSC_send");

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

//////////////////////////////////////////////////////////////////////////////////////////////////////
// WINDOW RESHAPE 
//////////////////////////////////////////////////////////////////////////////////////////////////////
void pg_window_reshape(GLsizei width, GLsizei height) {
	glutReshapeWindow(PG_WINDOW_WIDTH, PG_WINDOW_HEIGHT);

	//  printf("reshape main win %d %d %d\n" , PG_WINDOW_WIDTH , PG_WINDOW_HEIGHT , width , height );
	printf("Resize Window %dx%d\n", PG_WINDOW_WIDTH, PG_WINDOW_HEIGHT);

	pg_setWindowDimensions();
	pg_initRenderingMatrices();

#ifndef PG_WACOM_TABLET
	printf("************* Version for mouse pointer **********\n");
#else
	printf("************* Version for tablet **********\n");
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// KEYSTROKE RECEPTION AND PROCESSING
//////////////////////////////////////////////////////////////////////////////////////////////////////
void pg_window_key_browse(unsigned char key, int x, int y) {
	switch (key) {
	case KEY_ESC:
		pg_quit();
		break;
		// default: use standard key processing (non-platform dependent)
	default:
		pg_process_key((int)key);
		break;
	}
}

void pg_window_special_key_browse(int key, int x, int y) {
	// use standard key processing (non-platform dependent)
	pg_process_special_key(key);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
// FBO INITIALIZATION
//////////////////////////////////////////////////////////////////////////////////////////////////////

bool pg_initFBOTextureImagesAndRendering(void) {
	int maxbuffers;
	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxbuffers);
	int maxDrawBuf;
	glGetIntegerv(GL_MAX_DRAW_BUFFERS, &maxDrawBuf);

	if (maxbuffers < pg_enum_FBO_Update_nbAttachts) {
		sprintf(pg_errorStr, "Error: Maximal attachment (%d) -> %d required!", maxbuffers, pg_enum_FBO_Update_nbAttachts); pg_ReportError(pg_errorStr); // throw 336;
	}
	if (maxDrawBuf < pg_enum_FBO_Update_nbAttachts) {
		sprintf(pg_errorStr, "Error: Maximal draw buffers (%d) -> %d required!", maxbuffers, pg_enum_FBO_Update_nbAttachts); pg_ReportError(pg_errorStr); // throw 336;
	}

	/*
	// FBO: camera frame after initial processing
	glGenFramebuffers(1, &FBO_CameraFrame);  // drawing memory on odd and even frames for echo
	if (!FBO_CameraFrame_texID) {
		glGenTextures(1, &FBO_CameraFrame_texID);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, FBO_CameraFrame);
	pg_initFBOTextures(&FBO_CameraFrame_texID, 1, false);
	glDrawBuffers(1, enumDrawBuffersEntries);
	pg_printOglError(343);
	*/

	// FBO: multi-attachment for update 
	// initializations to NULL
		// 2 = FBO ping-pong size for Update shader FBOs
	for (int indAttachedFB = 0; indAttachedFB < 2 * pg_enum_FBO_Update_nbAttachts; indAttachedFB++) {
		pg_FBO_Update_texID[indAttachedFB] = 0;
	}
	glGenTextures(2 * pg_enum_FBO_Update_nbAttachts, pg_FBO_Update_texID);
	printf("FBO Update size %d %d attachments %d (configs %d)\n", pg_workingWindow_width, PG_WINDOW_HEIGHT, pg_enum_FBO_Update_nbAttachts, pg_NbScenarios);
	pg_initFBOTextures(pg_FBO_Update_texID, 2 * pg_enum_FBO_Update_nbAttachts, false, NULL);

	glGenFramebuffers(1, &pg_FBO_Update);
	// ping-pong FBO texture binding, changes each frame

	pg_printOglError(341);

	// FBO: multi-attachment for particle animation 
	// 2 = FBO ping-pong size for ParticleAnimation shader FBOs
	// initializations to NULL
	for (int indAttachedFB = 0; indAttachedFB < 2 * pg_enum_FBO_ParticleAnimation_nbAttachts; indAttachedFB++) {
		pg_FBO_ParticleAnimation_texID[indAttachedFB] = 0;
	}
	glGenTextures(2 * pg_enum_FBO_ParticleAnimation_nbAttachts, pg_FBO_ParticleAnimation_texID);
	printf("FBO Particle animation size %d %d attachments %d\n", pg_workingWindow_width, PG_WINDOW_HEIGHT, pg_enum_FBO_ParticleAnimation_nbAttachts);
	pg_initFBOTextures(pg_FBO_ParticleAnimation_texID, 2 * pg_enum_FBO_ParticleAnimation_nbAttachts, false, NULL);

	glGenFramebuffers(1, &pg_FBO_ParticleAnimation);
	// ping-pong FBO texture binding, changes each frame

	pg_printOglError(341);

	// FBO: ClipArt GPU drawing output 
	glGenTextures(1, &pg_FBO_ClipArt_render_texID);
	glGenRenderbuffers(1, &FBO_ClipArt_depthAndStencilBuffer);
	printf("FBO ClipArt GPU rendering size %d %d\n", pg_workingWindow_width, PG_WINDOW_HEIGHT);
	pg_initFBOTextures(&pg_FBO_ClipArt_render_texID, 1, true, &FBO_ClipArt_depthAndStencilBuffer);

	glGenFramebuffers(1, &pg_FBO_ClipArtRendering);  // drawing memory on odd and even frames for echo 
	// texture binding is constant and made once for all
	pg_bindFBOTextures(pg_FBO_ClipArtRendering, &pg_FBO_ClipArt_render_texID, 1, true, FBO_ClipArt_depthAndStencilBuffer);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	pg_printOglError(344);


	// FBO: particle drawing output 
	glGenTextures(1, &pg_FBO_Particle_render_texID);
	glGenRenderbuffers(1, &pg_FBO_ParticleRendering_depthAndStencilBuffer);
	printf("FBO Particle rendering size %d %d\n", pg_workingWindow_width, PG_WINDOW_HEIGHT);
	pg_initFBOTextures(&pg_FBO_Particle_render_texID, 1, true, &pg_FBO_ParticleRendering_depthAndStencilBuffer);

	glGenFramebuffers(1, &pg_FBO_ParticleRendering);  // drawing memory on odd and even frames for echo 
	// texture binding is constant and made once for all
	pg_bindFBOTextures(pg_FBO_ParticleRendering, &pg_FBO_Particle_render_texID, 1, true, pg_FBO_ParticleRendering_depthAndStencilBuffer);

	pg_printOglError(344);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// FBO: composition output for echo
	// 2 = FBO ping-pong size for echo FBOs
	// initializations to NULL
	for (int indFB = 0; indFB < 2; indFB++) {
		pg_FBO_Mixing_capturedFB_prec_texID[indFB] = 0;
	}
	glGenTextures(2, pg_FBO_Mixing_capturedFB_prec_texID);
	printf("FBO Mixing animation size %d %d attachments %d\n", pg_workingWindow_width, PG_WINDOW_HEIGHT, 2);
	pg_initFBOTextures(pg_FBO_Mixing_capturedFB_prec_texID, 2, false, 0);

	glGenFramebuffers(1, &pg_FBO_Mixing_capturedFB_prec);  // drawing memory on odd and even frames for echo 
	// ping-pong FBO texture binding, changes each frame

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Augmented Reality or bypassing mesh rendering: FBO capture of Master to be displayed on a mesh
	pg_FBO_Master_capturedFB_prec_texID = 0;
	glGenTextures(1, &pg_FBO_Master_capturedFB_prec_texID);
	printf("FBO Master size %d %d attachments %d\n", pg_workingWindow_width, PG_WINDOW_HEIGHT, 1);
	pg_initFBOTextures(&pg_FBO_Master_capturedFB_prec_texID, 1, false, 0);

	glGenFramebuffers(1, &pg_FBO_Master_capturedFB_prec);  // master output memory for mapping on mesh of bypassing mesh rendering
	// texture binding is constant and made once for all
	pg_bindFBOTextures(pg_FBO_Master_capturedFB_prec, &pg_FBO_Master_capturedFB_prec_texID, 1, false, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	pg_printOglError(342);
	return true;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
// VARIABLE INITIALIZATION BEFORE LAUNCHING
//////////////////////////////////////////////////////////////////////////////////////////////////////
void pg_init_scene(void) {
	if (argc <= 1) {
		sprintf(pg_errorStr, "Porphyrograph: not enough arguments"); pg_ReportError(pg_errorStr);
		sprintf(pg_errorStr, "Usage: Porphyrograph [configuration.conf]"); pg_ReportError(pg_errorStr); throw 100;
	}

	// log file
	if (!pg_csv_logFile_name.empty()) {
		printf("Open csv log file %s\n", pg_csv_logFile_name.c_str());
		if ((pg_csv_log_file = fopen(pg_csv_logFile_name.c_str(), "wb")) == NULL) {
			sprintf(pg_errorStr, "File %s not opened!", pg_csv_logFile_name.c_str()); pg_ReportError(pg_errorStr); throw 152;
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

	// reads the text messages in the text file
	pg_ReadAllDisplayMessages(pg_MessageFile[pg_ind_scenario]);

#if defined(var_Novak_flight_on)
	// ------ flight initialization  ------------- //
	Novak_flight_init_control_points();
#endif

	if (pg_FullScenarioActiveVars[pg_ind_scenario][_light_color]) {
		pg_DMX_light_initialization();
	}

	printf("Open portaudio\n");
	pg_pa_openSoundData();

	if (pg_FullScenarioActiveVars[pg_ind_scenario][_sensor_layout]) {
		/////////////////////////////////////////////////////////////////////////
		// SENSORS INITIALIZATION
		// copies the grid layout
		pg_assignSensorPositions();

		// copies the single central activation
		pg_assignSensorActivations();
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
	for (pg_IPServer& server : pg_IP_Servers) {
		server.InitServer();
	}
	// client initialization
	for (pg_IPClient& client : pg_IP_Clients) {
		// printf( "Client %d initialization\n" , ind );
		  //std::cout << "client->Remote_server_IP: " << ind << " " << client->Remote_server_IP << "\n";
		  //std::cout << "client->Remote_server_port: " << client->Remote_server_port << "\n";
		client.InitClient();
		// printf( "Client name %s\n" , pg_IP_Clients[ ind ]->id );
	}
	// printf( "End of scene initialization\n" );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// PEN COORDINATES PROCESSING
//////////////////////////////////////////////////////////////////////////////////////////////////////

void pg_MouseCoordinatesRemapping(int x, int y, int* mappedX, int* mappedY) {
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
	printf("click button %d (%d,%d)\n", button, x, y);
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
	printf("active button (%d,%d)\n", x, y);
	pg_CurrentCursorPos_x[0] = x;
	pg_CurrentCursorPos_y[0] = y;
	pg_CurrentStylusHooverPos_x = x;
	pg_CurrentStylusHooverPos_y = y;
}

void pg_window_passiveMotionFunc_browse(int x, int y) {
	printf("passive button (%d,%d)\n", x, y);
	pg_CurrentCursorPos_x[0] = PG_OUT_OF_SCREEN_CURSOR;
	pg_CurrentCursorPos_y[0] = PG_OUT_OF_SCREEN_CURSOR;
	pg_CurrentStylusHooverPos_x = x;
	pg_CurrentStylusHooverPos_y = y;
	// freeglut (PG): glutGetModifiers() called outside an input callback/int
}
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////
// IDLE FUNCTION WITH FRAME RATE CONTROL
//////////////////////////////////////////////////////////////////////////////////////////////////////

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
				pg_writeMessageOnScreen(string(mess));
				printf("%s\n", pg_screenMessage.c_str());
			}
		}

		// decays window message brightness
		if (pg_messageTransparency > 0.0) {
			pg_messageTransparency -= 0.4f * float(pg_CurrentClockTime - pg_LastScreenMessageDecayTime);
			pg_LastScreenMessageDecayTime = pg_CurrentClockTime;

			// printf( "Message opacity %.2f mess %s\n" , pg_messageTransparency , pg_screenMessage.c_str() );
		}

		// if message brightness becomes null, the message is erased
		if (pg_messageTransparency <= 0.0) {
			pg_screenMessage = "";
			pg_messageTransparency = 0.0;
			pg_LastScreenMessageDecayTime = pg_CurrentClockTime;
		}
		pg_printOglError(300);

		// printf( "Window %s\n" , pg_CurrentWindow->id );
		pg_screenMessage_update();

		// updates diaporama
		if (photo_diaporama >= 0 && photo_diaporama < pg_nbCompressedImageDirs[pg_ind_scenario]) {
			// printf("pg_update_diaporama\n");
			pg_update_diaporama();
		}

		pg_printOglError(467);

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

		// continous flashes
		// in case the flash frequency (flashPixel_freq, flashTrkCA_freq, flashCABG_freq)
		// is equal to (PG_LOOP_SIZE + 1), whatever the value, flashes are emitted at every frame (or on beats if we reach an onset for the video or the movie)
		pg_flash_control(pg_flash_continuous_generation);

		// UDP messages IN/OUT
		for (pg_IPServer& server : pg_IP_Servers) {
			server.receiveIPMessages();
		}
		for (pg_IPClient& client : pg_IP_Clients) {
			client.sendIPmessages();
		}

		// only for DMX
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_light_color]) {
			pg_lightUpdate();
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

void pg_window_idle_browse(void) {
	pg_window_idle_browse(0);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// WINDOW INITIALIZATION
//////////////////////////////////////////////////////////////////////////////////////////////////////

void pg_initGLutWindows(void) {
	glutInitWindowSize(PG_WINDOW_WIDTH, PG_WINDOW_HEIGHT);
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_window_x]) {
		window_x = int(pg_InitialValuesInterpVar[0][_window_x].val_num);
	}
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_window_y]) {
		window_x = int(pg_InitialValuesInterpVar[0][_window_y].val_num);
	}
	glutInitWindowPosition(window_x, window_y);

	pg_CurrentWindow->glutID = glutCreateWindow(" ");

	// window resize
	glutReshapeWindow(PG_WINDOW_WIDTH, PG_WINDOW_HEIGHT);

	printf("GLUT size %dx%d offset %dx%d\n", PG_WINDOW_WIDTH, PG_WINDOW_HEIGHT, window_x, window_y);
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

void pg_OpenGLInit(void) {
	// background color
	glClearColor(pg_OpenGLBGcolor[0], pg_OpenGLBGcolor[1], pg_OpenGLBGcolor[2], pg_OpenGLBGcolor[3]);

	// buffer reset
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// no z-Buffer
	glDisable(GL_DEPTH_TEST);
	// // smooth shading
	// glShadeModel(GL_SMOOTH);
	// // normal vector normalization
	// glEnable( GL_NORMALIZE );

	// disable vsync
	// float r = 

}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// MAIN FUNCTION
//////////////////////////////////////////////////////////////////////////////////////////////////////

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
	pg_NbScenarios = int(argc - 1);
	if (pg_NbScenarios > PG_MAX_SCENARIOS) {
		sprintf(pg_errorStr, "Porphyrograph: should have maximally %d associated configuration and scenario file(s), not %d (<conf.csv> <scenario.csv>)+", PG_MAX_SCENARIOS, (argc - 1) / 2); pg_ReportError(pg_errorStr); throw(6678);
	}
	else {
		printf("Porphyrograph: %d associated scenario file(s) (<scenario.csv>)\n", (argc - 1));
	}

	pg_initPaths();

	pg_Shader_File_Names = new string * [pg_NbScenarios];
	pg_Shader_Stages = new GLenum * *[pg_NbScenarios];
	pg_Shader_nbStages = new int* [pg_NbScenarios];
	pg_shader_programme = new unsigned int* [pg_NbScenarios];
	for (int ind = 1; ind < argc; ind++) {
		pg_shader_programme[(ind - 1)] = new unsigned int[pg_enum_shader_TotalNbTypes];
		for (int ind_shader_type = 0; ind_shader_type < pg_enum_shader_TotalNbTypes; ind_shader_type++) {
			pg_shader_programme[(ind - 1)][ind_shader_type] = 0;
		}
	}

	pg_ScenarioFileNames = new string[pg_NbScenarios];
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

	// OpenGL initialization (clear buffer)
	pg_OpenGLInit();
	// cursor shape selection
	pg_CursorInit();
	pg_printOglError(475);

	// initializations before rendering

	// sensor initialization
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_sensor_layout]) {
		pg_SensorInitialization();
	}

	if (pg_FullScenarioActiveVars[pg_ind_scenario][_pg_metawear]) {
		if (pg_metawear) {
			pg_MetawearSensorInitialization();
		}
	}

	// matrices, geometry, shaders and FBOs
	pg_initRenderingMatrices();
	pg_printOglError(31);

	// buiilds the quads to be rendered
	pg_initGeometry_quads();
	pg_printOglError(32);
	
	// loads the shaders
	pg_loadAllShaders();
	pg_printOglError(33);

	// initializes the FBOS
	pg_initFBOTextureImagesAndRendering();
	pg_printOglError(34);

	// textures loading
	pg_loadAllTextures();
	pg_printOglError(36);

	// meshes loading
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_activeMeshes]) {
		pg_loadAllMeshes();
	}

	// soundtrack listing
	pg_listAll_soundTracks();

	// SVG paths listing
	pg_listAll_Paths();

	/////////////////////////////////////////////////////////////////////////
	// ClipArt GPU INITIALIZATION
	// loads the ClipArts and lists them
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_activeClipArts]) {
		pg_loadAll_ClipArts();
		pg_listAll_ClipArts();
	}


#if defined(PG_RENOISE)
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
	pg_ReadInitalClipFramesTextures();

	// applies all callbacks
	pg_initializationCallBacks();
	pg_printOglError(467);

	// camera frame capture initialization
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_cameraCaptFreq]) {
		pg_openCameraCaptureAndLoadFrame();
	}

	// video intialization: loads the movie of the intial configuration
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_movieCaptFreq]) {
		if (playing_movieNo >= 0 && playing_movieNo < int(pg_VideoTracks[pg_ind_scenario].size())
			&& playing_movieNo != pg_currentlyPlaying_movieNo) {
			pg_movie_frame.setTo(Scalar(0, 0, 0));

			pg_currentlyPlaying_movieNo = playing_movieNo;

			// texture ID initialization (should not be inside a thread)
			if (pg_movie_texture_texID == NULL_ID) {
				glGenTextures(1, &pg_movie_texture_texID);
			}

			pg_is_movieLoading = true;
			printf("Loading movie %s\n",
					pg_VideoTracks[pg_ind_scenario][pg_currentlyPlaying_movieNo].videoFileName.c_str());
			sprintf(pg_AuxString, "/movie_shortName %s", pg_VideoTracks[pg_ind_scenario][pg_currentlyPlaying_movieNo].videoShortName.c_str());
			pg_send_message_udp((char*)"s", pg_AuxString, (char*)"udp_TouchOSC_send");

			pg_initVideoMoviePlayback(&pg_VideoTracks[pg_ind_scenario][pg_currentlyPlaying_movieNo].videoFileName);
		}
	}

	// clip intialization for Left Clip
	if (pg_playing_clipNoLeft >= 0 && pg_playing_clipNoLeft < pg_nbClips[pg_ind_scenario] && pg_playing_clipNoLeft != pg_all_clip_status[pg_enum_clipLeft].getCurrentlyPlaying_clipNo(0)) {
		pg_all_clip_status[pg_enum_clipLeft].setCurrentlyPlaying_clipNo(0, pg_playing_clipNoLeft);
		sprintf(pg_AuxString, "/clip_shortName_0 %03d", pg_playing_clipNoLeft);
		pg_send_message_udp((char*)"s", pg_AuxString, (char*)"udp_TouchOSC_send");
	}
	if (PG_NB_PARALLEL_CLIPS >= 2) {
		if (pg_playing_secondClipNoLeft >= 0 && pg_playing_secondClipNoLeft < pg_nbClips[pg_ind_scenario] && pg_playing_secondClipNoLeft != pg_all_clip_status[pg_enum_clipLeft].getCurrentlyPlaying_clipNo(1)) {
			pg_all_clip_status[pg_enum_clipLeft].setCurrentlyPlaying_clipNo(1, pg_playing_secondClipNoLeft);
			sprintf(pg_AuxString, "/clip2_shortName_0 %03d", pg_playing_secondClipNoLeft);
			pg_send_message_udp((char*)"s", pg_AuxString, (char*)"udp_TouchOSC_send");
		}
	}
	// clip intialization for Right Clip
	if (pg_playing_clipNoRight >= 0 && pg_playing_clipNoRight < pg_nbClips[pg_ind_scenario] && pg_playing_clipNoRight != pg_all_clip_status[pg_enum_clipRight].getCurrentlyPlaying_clipNo(0)) {
		pg_all_clip_status[pg_enum_clipRight].setCurrentlyPlaying_clipNo(0, pg_playing_clipNoRight);
		sprintf(pg_AuxString, "/clip_shortName_1 %03d", pg_playing_clipNoRight);
		pg_send_message_udp((char*)"s", pg_AuxString, (char*)"udp_TouchOSC_send");
	}
	if (PG_NB_PARALLEL_CLIPS >= 2) {
		if (pg_playing_secondClipNoRight >= 0 && pg_playing_secondClipNoRight < pg_nbClips[pg_ind_scenario] && pg_playing_secondClipNoRight != pg_all_clip_status[pg_enum_clipRight].getCurrentlyPlaying_clipNo(1)) {
			pg_all_clip_status[pg_enum_clipRight].setCurrentlyPlaying_clipNo(1, pg_playing_secondClipNoRight);
			sprintf(pg_AuxString, "/clip2_shortName_1 %03d", pg_playing_secondClipNoRight);
			pg_send_message_udp((char*)"s", pg_AuxString, (char*)"udp_TouchOSC_send");
		}
	}

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

