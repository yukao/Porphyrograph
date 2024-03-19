/*! \file pg-script.cpp
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

#include "pg_script_body.cpp"

std::string project_name = "atelPort";

char AuxString[1024];

//////////////////////////////////////////////
// SCENARIO AND CONFIGURATION VARIABLES
// changed to true when a value is changed manually during scenario
bool BrokenInterpolationVar[_MaxInterpVarIDs];
// initial values in the scenario (before first scene)
float InitialValuesInterpVar[_MaxInterpVarIDs];
// last value shipped to the GUI (PD)
float LastGUIShippedValuesInterpVar[_MaxInterpVarIDs];
// initial values in the configuration file
float InitialValuesConfigurationVar[_MaxConfigurationVarIDs];

//////////////////////////////////////////////
// CA initialization
float initCA = 0.0f;

//////////////////////////////////////////////
// beat no
int pg_BeatNo = 0;

/////////////////////////////////////////////////////////////////////////
// WORKING VARIABLES
/////////////////////////////////////////////////////////////////////////

// ++++++++++++++++++++++ MUSIC CONTROL ++++++++++++++++++++ 
// music control inputs received at each frame
// addition of colorfor pen  based on palette
float pulsed_pen_color[4];
// addition of color for repop based on palette
float pulsed_repop_color[3];
// factor increasing the acceleration weight of pixels, the brush radius, color...
// based on sound volume attacks
float pulse_average;
float pulse_average_prec = 0.f;
float pulse[3] = { 0.0f };
float pulse_attack;

// auxiliary 
bool synchr_start_recording_path[PG_NB_PATHS + 1];
int synchr_start_path_replay_trackNo[PG_NB_PATHS + 1];
bool recorded_path[PG_NB_PATHS + 1];
bool is_path_replay[PG_NB_PATHS + 1];


// track loading 
int currentSVGTrack = 0;

// ++++++++++++++++++++++ FLASHES +++++++++++++++++++++++++
// flash: Trk->CA -> values passed to the shader
// as a function of on/off values and weights
float flashTrkCA_weights[PG_NB_TRACKS];
// flash: Trk->Part -> values passed to the shader
// as a function of on/off values and weights
float flashTrkPart_weights[PG_NB_TRACKS];
// flash: CA->BG
float flashCABG_weight;
// flash: CA->Part
float flashCAPart_weight;
// flash: Part->BG
float flashPartBG_weight;
// flash: Part->CA
float flashPartCA_weight;
// flash: extension of pixel reproduction  
int flashPixel;
// flash: Trk->BG   -> values passed to the shader
// as a function of on/off values and weights
// only tracks 1 and 2 are concerned since it is a projection on track0
float flashTrkBG_weights[PG_NB_TRACKS];

int flashTrkCA_freq_0_saved = 0;
#if PG_NB_TRACKS >= 2
int flashTrkCA_freq_1_saved = 0;
#endif
#if PG_NB_TRACKS >= 3
int flashTrkCA_freq_2_saved = 0;
#endif
#if PG_NB_TRACKS >= 4
int flashTrkCA_freq_3_saved = 0;
#endif

int flashTrkPart_freq_0_saved = 0;
#if PG_NB_TRACKS >= 2
int flashTrkPart_freq_1_saved = 0;
#endif
#if PG_NB_TRACKS >= 3
int flashTrkPart_freq_2_saved = 0;
#endif
#if PG_NB_TRACKS >= 4
int flashTrkPart_freq_3_saved = 0;
#endif

int flashTrkBG_freq_1_saved = 0;
int flashTrkBG_freq_2_saved = 0;

// +++++++++++++++++++++ VIDEO FLASH +++++++++++++++++++++
bool is_flashCameraTrk = false;
float flashCameraTrk_weight = 0.0f;
float flashCameraTrk_decay = 1.0f;

// +++++++++++++++++++++ VIDEO BACKGROUND SUBTRACTION +++++
bool is_firstVideoOn = true;

// +++++++++++++++++++++ CLEAR +++++++++++++++++++++++++++
int isClearCA;
int isClearLayer;
int isClearAllLayers;

// +++++++++++++++++++++ COPY LAYERS +++++++++++++++++++++++++++
// copy to layer above (+1) or to layer below (-1)
int copyToNextTrack = 0;

// ++++++++++++++++++++++ SET-UP +++++++++++++++++++++++++++ 
int pg_CurrentScene = -1;

// scene management
bool pg_FirstFrameInScene = true;
float remainingTimeInScene = 0.0f;

// resend all values (fx after interface crash)
bool resend_all_variables = true;

// working variables 
int drawing_start_frame;

// SCENARIO LAUNCH TIME
double InitialScenarioTime;
double AbsoluteInitialScenarioTime;

// +++++++++++++++++++++++ CLEAR AND INVERT +++++++++++++++++++++++++++++++++
double lastClearTime = 0.0;

// +++++++++++++++++++++++ Beats +++++++++++++++++++++++++++++++++
int BeatNo = 1;
double lastBeatTime = 0.0;

// +++++++++++++++++++++++ CA SubType memory for CA on/off command ++++++++++
int CASubTypeMem = 1;

///////////////////////////////////////////////////////////////////////////////////
// ASSIGNS INITIAL VALUES TO ALL CONFIGURATION FILES
void InitializeConfigurationVar(void) {
	for (int indVar = 0; indVar < _MaxConfigurationVarIDs; indVar++) {
		if (ConfigurationVarTypes[indVar] == _pg_float) {
			*((float *)ConfigurationVarPointers[indVar]) = InitialValuesConfigurationVar[indVar];
		}
		else if (ConfigurationVarTypes[indVar] == _pg_int) {
			*((int *)ConfigurationVarPointers[indVar]) = (int)round(InitialValuesConfigurationVar[indVar]);
		}
		else if (ConfigurationVarTypes[indVar] == _pg_bool) {
			*((bool *)ConfigurationVarPointers[indVar]) = (InitialValuesConfigurationVar[indVar] > 0 ? true : false);
		}
		else if (ConfigurationVarTypes[indVar] == _pg_sign) {
			*((float *)ConfigurationVarPointers[indVar]) = (InitialValuesConfigurationVar[indVar] > 0 ? 1.0f : -1.0f);
		}
		else if (ConfigurationVarTypes[indVar] == _pg_path) {
			*((bool *)ConfigurationVarPointers[indVar]) = (InitialValuesConfigurationVar[indVar] > 0 ? true : false);
		}
		else {
			sprintf(ErrorStr, "Incorrect variable type (%d) for configuration variable #%d!", ConfigurationVarTypes[indVar], indVar); ReportError(ErrorStr);
		}
	}
}

/////////////////////////////////////////////////////////////////////////
// INITIALIZES ALL SCENARIO VARIABLES AND ASSIGNS THEM THE VALUES OF THE FIRST SCENARIO LINE
void pg_initializationScript(void) {
	/////////////////////////////////////////////////////////////////////////
	// CONSTANT UNIFORM VARIABLES
	// only assigned at initialization, does not change during the set
	//glUseProgram(shader_programme[pg_shader_Camera]);
	//glUniform2f(uniform_Camera_vp_2fv_width_height,
	//	(GLfloat)leftWindowWidth, (GLfloat)window_height);

	// only assigned at initialization, does not change during the set
	glUseProgram(shader_programme[pg_shader_Update]);
	glUniform2f(uniform_Update_vp_2fv_width_height,
		(GLfloat)leftWindowWidth, (GLfloat)window_height);

	/////////////////////////////////////////////////////////////////////////
	// SCENARIO-CONTROLLED UNIFORM VARIABLES
	for (int indVar = 0; indVar < _MaxInterpVarIDs; indVar++) {
		if (ScenarioVarTypes[indVar] == _pg_float) {
			*((float *)ScenarioVarPointers[indVar]) = InitialValuesInterpVar[indVar];
		}
		else if (ScenarioVarTypes[indVar] == _pg_int) {
			*((int *)ScenarioVarPointers[indVar]) = (int)round(InitialValuesInterpVar[indVar]);
		}
		else if (ScenarioVarTypes[indVar] == _pg_bool) {
			*((bool *)ScenarioVarPointers[indVar]) = (InitialValuesInterpVar[indVar] > 0 ? true : false);
		}
		else if (ScenarioVarTypes[indVar] == _pg_sign) {
			*((float *)ScenarioVarPointers[indVar]) = (InitialValuesInterpVar[indVar] > 0 ? 1.0f : -1.0f);
		}
		else if (ScenarioVarTypes[indVar] == _pg_path) {
			*((bool *)ScenarioVarPointers[indVar]) = (InitialValuesInterpVar[indVar] > 0 ? true : false);
		}
		else {
			sprintf(ErrorStr, "Incorrect variable type (%d) for scenario variable #%d!", ScenarioVarTypes[indVar], indVar); ReportError(ErrorStr);
		}
	}

	// track replay 
	// source track recording
	// is recording source -> has to stop recording source 
#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7
	if (path_replay_trackNo_1 >= 0) {
		if (tracksSync) {
			synchr_start_path_replay_trackNo[1] = path_replay_trackNo_1;
		}
		else {
			pg_path_replay_trackNo_onOff(1, path_replay_trackNo_1);
		}
	}
	if (path_replay_trackNo_2 >= 0) {
		if (tracksSync) {
			synchr_start_path_replay_trackNo[2] = path_replay_trackNo_2;
		}
		else {
			pg_path_replay_trackNo_onOff(2, path_replay_trackNo_2);
		}
	}
	if (path_replay_trackNo_3 >= 0) {
		if (tracksSync) {
			synchr_start_path_replay_trackNo[3] = path_replay_trackNo_3;
		}
		else {
			pg_path_replay_trackNo_onOff(3, path_replay_trackNo_3);
		}
	}
#endif
#if PG_NB_PATHS == 7
	if (path_replay_trackNo_4 >= 0) {
		if (tracksSync) {
			synchr_start_path_replay_trackNo[4] = path_replay_trackNo_4;
		}
		else {
			pg_path_replay_trackNo_onOff(4, path_replay_trackNo_4);
		}
	}
	if (path_replay_trackNo_5 >= 0) {
		if (tracksSync) {
			synchr_start_path_replay_trackNo[5] = path_replay_trackNo_5;
		}
		else {
			pg_path_replay_trackNo_onOff(5, path_replay_trackNo_5);
		}
	}
	if (path_replay_trackNo_6 >= 0) {
		if (tracksSync) {
			synchr_start_path_replay_trackNo[6] = path_replay_trackNo_6;
		}
		else {
			pg_path_replay_trackNo_onOff(6, path_replay_trackNo_6);
		}
	}
	if (path_replay_trackNo_7 >= 0) {
		if (tracksSync) {
			synchr_start_path_replay_trackNo[7] = path_replay_trackNo_7;
		}
		else {
			pg_path_replay_trackNo_onOff(7, path_replay_trackNo_7);
		}
	}
#endif

	// track recording 
	// source track recording
	// is recording source -> has to stop recording source 
#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7
	if (!path_record_1) {
		pg_path_recording_stop(1);
	}
	if (!path_record_2) {
		pg_path_recording_stop(2);
	}
	if (!path_record_3) {
		pg_path_recording_stop(3);
	}
#endif
#if PG_NB_PATHS == 7
	if (!path_record_4) {
		pg_path_recording_stop(4);
	}
	if (!path_record_5) {
		pg_path_recording_stop(5);
	}
	if (!path_record_6) {
		pg_path_recording_stop(6);
	}
	if (!path_record_7) {
		pg_path_recording_stop(7);
	}
#endif


	/////////////////////////////////////////////////////////////////////////
	// SOUND-CONTROLLED UNIFORM VARIABLES: dynamic values received from PD 
	// flashes
	for (int indtrack = 0; indtrack < PG_NB_TRACKS; indtrack++) {
		flashTrkCA_weights[indtrack] = 0;
		flashTrkPart_weights[indtrack] = 0;
		flashTrkBG_weights[indtrack] = 0;
	}
	flashPixel = 0;
	flashCABG_weight = 0;
	flashCAPart_weight = 0;
	flashPartBG_weight = 0;
	flashPartCA_weight = 0;

	// video flash
	is_flashCameraTrk = false;
	flashCameraTrk_weight = 0.0f;

	// background subraction
	reset_camera = true;

	// particle initialization reset
	part_initialization = -1;

	// clear
	isClearCA = 0;
	isClearLayer = 0;
	isClearAllLayers = 0;

	// copy to layer above (+1) or to layer below (-1)
	copyToNextTrack = 0;

	// pen music color
	pulsed_pen_color[0] = RGBpalette_r_ini;
	pulsed_pen_color[1] = RGBpalette_g_ini;
	pulsed_pen_color[2] = RGBpalette_b_ini;
	pulsed_pen_color[3] = RGBpalette_a_ini;
	// repop music color
	pulsed_repop_color[0] = RGBpalette_r_ini;
	pulsed_repop_color[1] = RGBpalette_g_ini;
	pulsed_repop_color[2] = RGBpalette_b_ini;
	// pulse from sound 
	pulse_average = 0.0f;

	/////////////////////////////////////////////////////////////////////////
	// DRAWING-CONTROLLED UNIFORM VARIABLES: dynamic values received from drawing or track display 
	// pen position storage on the two quads 
	for (int indPath = 0; indPath < PG_NB_PATHS + 1; indPath++) {
		paths_x[indPath] = -1.0F;
		paths_y[indPath] = -1.0F;
		paths_x_prev[indPath] = -1.0F;
		paths_y_prev[indPath] = -1.0F;
		// pen color
		paths_Color_r[indPath] = 0.0F;
		paths_Color_g[indPath] = 0.0F;
		paths_Color_b[indPath] = 0.0F;
		paths_Color_a[indPath] = 1.0F;
		// pen brush & size
		paths_BrushID[indPath] = 0;
		paths_RadiusX[indPath] = 0.0F;
		paths_RadiusY[indPath] = 0.0F;
	}

	// particule color
	repop_Color_r = 0.0f;
	repop_Color_g = 0.0f;
	repop_Color_b = 0.0f;

	// hoover cursor
	CurrentCursorHooverPos_x = 0;
	CurrentCursorHooverPos_y = 0;
	// stylus type
	CurrentCursorStylusvsRubber = pg_Stylus;

	/////////////////////////////////////////////////////////////////////////
	// TIME CONTROLLED VARIABLES (NOT FORWARDED TO SHADERS)
	// scenario management
	pg_CurrentScene = -1;
	// frame number initialization
	pg_FrameNo = first_frame_number - 1;
	// intial real time
	InitialRealTime = RealTime() + initial_time;
	// initial scenario time: well before current time to be over
	InitialScenarioTime = InitialRealTime - 1000000;
	AbsoluteInitialScenarioTime = InitialRealTime - 1000000;
	// TIME CONTROLLED UNIFORM VARIABLES
	// message transparency & echo
	messageTransparency = 0.0;

	// pg_send_message_udp((char *)"s", (char *)"/message_can init_completed", (char *)"udp_QT_send");
}

/////////////////////////////////////////////////////////////////////////
// INITIALIZES ALL SCENARIO VARIABLES CALLBACKS
void pg_initializationCallBacks(void) {
	for (int indVar = 0; indVar < _MaxInterpVarIDs; indVar++) {
		if (ScenarioVarCallbacks[indVar]) {
			if (ScenarioVarTypes[indVar] == _pg_float) {
				(*ScenarioVarCallbacks[indVar])(_PG_SCENARIO, InitialValuesInterpVar[indVar]);
			}
			else if (ScenarioVarTypes[indVar] == _pg_int) {
				(*ScenarioVarCallbacks[indVar])(_PG_SCENARIO, float((int)round(InitialValuesInterpVar[indVar])));
			}
			else if (ScenarioVarTypes[indVar] == _pg_bool) {
				(*ScenarioVarCallbacks[indVar])(_PG_SCENARIO, (InitialValuesInterpVar[indVar] > 0 ? true : false));
			}
			else if (ScenarioVarTypes[indVar] == _pg_sign) {
				(*ScenarioVarCallbacks[indVar])(_PG_SCENARIO, (InitialValuesInterpVar[indVar] > 0 ? 1.0f : -1.0f));
			}
			else if (ScenarioVarTypes[indVar] == _pg_path) {
				(*ScenarioVarCallbacks[indVar])(_PG_SCENARIO, (InitialValuesInterpVar[indVar] > 0 ? true : false));
			}
			else {
				sprintf(ErrorStr, "Incorrect variable type (%d) for scenario variable #%d!", ScenarioVarTypes[indVar], indVar); ReportError(ErrorStr);
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////
// GUI DISPLAY & LOG FILE LOGGING
/////////////////////////////////////////////////////////////
void pg_displaySceneVariables(void) {
	if (true ) { //pg_FrameNo % 3 == 0 || pg_FirstFrameInScene) {
		/////////////////////////////////////////////////////////////////////////
		// SCENARIO-CONTROLLED VARIABLES
		for (int indVar = 0; indVar < _MaxInterpVarIDs; indVar++) {
			// the variable has a registered GUI ID in the scenario
			// and its values has changed since it was sent to the GUI last time
			if (*(ScenarioVarMessages[indVar])) {
				if (ScenarioVarTypes[indVar] == _pg_float
					&& (LastGUIShippedValuesInterpVar[indVar] != float(*((float *)ScenarioVarPointers[indVar]))
						|| resend_all_variables)) {
					sprintf(AuxString, "/%s %.4f", ScenarioVarMessages[indVar], *((float *)ScenarioVarPointers[indVar])); pg_send_message_udp((char *)"f", AuxString, (char *)"udp_QT_send");
					LastGUIShippedValuesInterpVar[indVar] = float(*((float *)ScenarioVarPointers[indVar]));
					//if (indVar == _repop_grey) {
					//	printf("repop_grey %s %.1f\n", ScenarioVarMessages[indVar], *((float *)ScenarioVarPointers[indVar]));
					//}
				}
				else if (ScenarioVarTypes[indVar] == _pg_int
					&& (LastGUIShippedValuesInterpVar[indVar] != float(*((int *)ScenarioVarPointers[indVar]))
						|| resend_all_variables)) {
					sprintf(AuxString, "/%s %d", ScenarioVarMessages[indVar], *((int *)ScenarioVarPointers[indVar])); pg_send_message_udp((char *)"f", AuxString, (char *)"udp_QT_send");
					// memorizes the shipped value in order to resend it only when changed
					LastGUIShippedValuesInterpVar[indVar] = float(*((int *)ScenarioVarPointers[indVar]));
				}
				else if (ScenarioVarTypes[indVar] == _pg_bool
					&& (LastGUIShippedValuesInterpVar[indVar] != float(*((bool *)ScenarioVarPointers[indVar]))
						|| resend_all_variables)) {
					sprintf(AuxString, "/%s %d", ScenarioVarMessages[indVar], (int)(*((bool *)ScenarioVarPointers[indVar]))); pg_send_message_udp((char *)"f", AuxString, (char *)"udp_QT_send");
					LastGUIShippedValuesInterpVar[indVar] = float(*((bool *)ScenarioVarPointers[indVar]));
				}
				else if (ScenarioVarTypes[indVar] == _pg_sign
					&& (LastGUIShippedValuesInterpVar[indVar] != float(*((float *)ScenarioVarPointers[indVar]))
						|| resend_all_variables)) {
					sprintf(AuxString, "/%s %.4f", ScenarioVarMessages[indVar], *((float *)ScenarioVarPointers[indVar])); pg_send_message_udp((char *)"f", AuxString, (char *)"udp_QT_send");
					LastGUIShippedValuesInterpVar[indVar] = float(*((float *)ScenarioVarPointers[indVar]));
				}
				else if (ScenarioVarTypes[indVar] == _pg_path
					&& (LastGUIShippedValuesInterpVar[indVar] != float(*((bool *)ScenarioVarPointers[indVar]))
						|| resend_all_variables)) {
					sprintf(AuxString, "/%s %d", ScenarioVarMessages[indVar], (int)(*((bool *)ScenarioVarPointers[indVar]))); pg_send_message_udp((char *)"f", AuxString, (char *)"udp_QT_send");
					LastGUIShippedValuesInterpVar[indVar] = float(*((bool *)ScenarioVarPointers[indVar]));
				}
			}
		}
		if (resend_all_variables) {
			if (pg_CurrentScene >= 0 && pg_CurrentScene < nb_scenes) {
				sprintf(AuxString, "/setup_can %s", Scenario[pg_CurrentScene].scene_IDs.c_str()); pg_send_message_udp((char *)"s", AuxString, (char *)"udp_QT_send");
			}
			else {
				sprintf(AuxString, "/setup_can initial_setup"); pg_send_message_udp((char *)"s", AuxString, (char *)"udp_QT_send");
			}
			sprintf(AuxString, "/pen_colorPreset_can %d", current_pen_colorPreset); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
			InterfaceInitializations();
			resend_all_variables = false;
		}
		
		/////////////////////////////////////////////////////////////////////////
		// TIME CONTROLLED VARIABLES (NOT FORWARDED TO SHADERS)
		// time display
		// time exceeded in scene
		if (remainingTimeInScene >= 0) {
			sprintf(AuxString, "/time_can %d:%d", (int)remainingTimeInScene / 60, (int)remainingTimeInScene % 60); pg_send_message_udp((char *)"s", AuxString, (char *)"udp_QT_send");
			if (remainingTimeInScene < 10) {
				sprintf(AuxString, "/time_color_can 0"); pg_send_message_udp((char *)"i", AuxString, (char *)"udp_QT_send");
			}
			else if (remainingTimeInScene < 20) {
				sprintf(AuxString, "/time_color_can 1"); pg_send_message_udp((char *)"i", AuxString, (char *)"udp_QT_send");
			}
			else {
				sprintf(AuxString, "/time_color_can 2"); pg_send_message_udp((char *)"i", AuxString, (char *)"udp_QT_send");
			}
		}
		// time exceeded in scene
		else {
			int remTime = (int)remainingTimeInScene;
			if (remTime > -60) {
				sprintf(AuxString, "/time_can %d:%d", remTime / 60, remTime % 60); pg_send_message_udp((char *)"s", AuxString, (char *)"udp_QT_send");
			}
			else {
				sprintf(AuxString, "/time_can %d:%d", remTime / 60, -(remTime % 60)); pg_send_message_udp((char *)"s", AuxString, (char *)"udp_QT_send");
			}
			sprintf(AuxString, "/time_color_can 3"); pg_send_message_udp((char *)"i", AuxString, (char *)"udp_QT_send");
		}
#ifdef ATELIERS_PORTATIFS
		float time_since_launching = (float)(CurrentClockTime - AbsoluteInitialScenarioTime);
		if (time_since_launching >= 0) {
			if (time_since_launching <= 10.016) { // simul: 3.83) {
				pg_send_message_udp((char *)"s", (char *)"/message_can 2_Sustain", (char *)"udp_QT_send");
			}
			else if (time_since_launching <= 43.908) { // simul: 38.49) {
				pg_send_message_udp((char *)"s", (char *)"/message_can 3_Glissando", (char *)"udp_QT_send");
			}
			else if (time_since_launching <= 50.498) { // simul: 43.93) {
				pg_send_message_udp((char *)"s", (char *)"/message_can 4_PEN___STAMP", (char *)"udp_QT_send");
			}
			else if (time_since_launching <= 74.597) { // simul: 66.23) {
				pg_send_message_udp((char *)"s", (char *)"/message_can 5_Glissando", (char *)"udp_QT_send");
			}
			else if (time_since_launching <= 81.148) { // simul: 72.32) {
				pg_send_message_udp((char *)"s", (char *)"/message_can 6_PEN___BUILDINGS", (char *)"udp_QT_send");
			}
			else if (time_since_launching <= 109.693) { // simul: 97.5) {
				pg_send_message_udp((char *)"s", (char *)"/message_can 7_DRAW___BUSES", (char *)"udp_QT_send");
			}
			else if (time_since_launching <= 183.9) { // simul: 164.28) {
				pg_send_message_udp((char *)"s", (char *)"/message_can 8_PEN___SHIP", (char *)"udp_QT_send");
			}
			else if (time_since_launching <= 241.066) { // simul: 216.57) {
				pg_send_message_udp((char *)"s", (char *)"/message_can 9_PEN___AAAAAAAAA", (char *)"udp_QT_send");
			}
			else if (time_since_launching <= 313.051) { // simul: 282.24) {
				pg_send_message_udp((char *)"s", (char *)"/message_can 10_PEN___TRUCK", (char *)"udp_QT_send");
			}
			else if (time_since_launching <= 363.549) { // simul: 327.51) {
				pg_send_message_udp((char *)"s", (char *)"/message_can 11_DRAW___HEAD", (char *)"udp_QT_send");
			}
			else if (time_since_launching <= 380.317) { // simul: 343.05) {
				pg_send_message_udp((char *)"s", (char *)"/message_can 12_Glissando/Doppler", (char *)"udp_QT_send");
			}
			else if (time_since_launching <= 402.506) { // simul: 363.37) {
				pg_send_message_udp((char *)"s", (char *)"/message_can 13_DRAW___ELBOW", (char *)"udp_QT_send");
			}
			else if (time_since_launching <= 414.984) { // simul: 374.57) {
				pg_send_message_udp((char *)"s", (char *)"/message_can 14_DRAW___CYLINDER", (char *)"udp_QT_send");
			}
			else if (time_since_launching <= 452.926) { // simul: 409.33) {
				pg_send_message_udp((char *)"s", (char *)"/message_can 15_DRAW___HAND", (char *)"udp_QT_send");
			}
			else if (time_since_launching <= 477.024) { // simul: 430.84) {
				pg_send_message_udp((char *)"s", (char *)"/message_can 16_DRAW___BIG_DIVER", (char *)"udp_QT_send");
			}
			else if (time_since_launching <= 533.06) { // simul: 481.97) {
				pg_send_message_udp((char *)"s", (char *)"/message_can 17_DRAW___BONE", (char *)"udp_QT_send");
			}
			else if (time_since_launching <= 593.385) { // simul: 534.44) {
				pg_send_message_udp((char *)"s", (char *)"/message_can 18_DRAW___SMALL_DIVERS", (char *)"udp_QT_send");
			}
			else if (time_since_launching <= 720.781) { // simul: 650.58) {
				pg_send_message_udp((char *)"s", (char *)"/message_can 19_PEN___CIRCLES", (char *)"udp_QT_send");
			}
			else if (time_since_launching <= 757.047) { // simul: 683.28) {
				pg_send_message_udp((char *)"s", (char *)"/message_can 20_Silence_HORSE", (char *)"udp_QT_send");
			}
			else if (time_since_launching <= 779.274) { // simul: 706.32) {
				pg_send_message_udp((char *)"s", (char *)"/message_can END", (char *)"udp_QT_send");
			}
			else {
				pg_send_message_udp((char *)"s", (char *)"/message_can END", (char *)"udp_QT_send");
			}
		}
#endif
	}
}

void pg_send_message_udp( char *pattern , char * message , char *targetHostid ) {
	pg_IPClient* targetHost = NULL;
	for( int ind = 0 ; ind < nb_IP_Clients ; ind++ ) {
		if( strcmp( targetHostid , IP_Clients[ ind ]->id.c_str() ) == 0 ) {
			targetHost = IP_Clients[ ind ];
			break;
		}
	}
	if( !targetHost ) {
		printf( "UDP client unknown %s\n" , targetHostid );
		return;
	}
	// printf("send_message_udp %s %s %d %d\n", message, pattern, nb_IP_Clients, targetHost);
	targetHost->storeIP_output_message( message , pattern );
}

//////////////////////////////////////////////////////////////////////////////////////
// LOG FILE
// makes a line of a CSV file to be used as a starting point for a repetition of the
// setup

void pg_logCurrentLineSceneVariables(char *fileName) {
	/////////////////////////////////////////////////////////////////////////
	// SCENARIO-CONTROLLED UNIFORM VARIABLES
	for (int indVar = 0; indVar < _MaxInterpVarIDs; indVar++) {
		if (ScenarioVarTypes[indVar] == _pg_float) {
			fprintf(fileLog, "\t%.6f", *((float *)ScenarioVarPointers[indVar]));
		}
		else if (ScenarioVarTypes[indVar] == _pg_int) {
			fprintf(fileLog, "\t%d", *((int *)ScenarioVarPointers[indVar]));
		}
		else if (ScenarioVarTypes[indVar] == _pg_bool) {
			fprintf(fileLog, "\t%d", int(*((bool *)ScenarioVarPointers[indVar])));
		}
		else if (ScenarioVarTypes[indVar] == _pg_sign) {
			fprintf(fileLog, "\t%d", (*((float *)ScenarioVarPointers[indVar]) > 0 ? +1 : -1));
		}
		else if (ScenarioVarTypes[indVar] == _pg_path) {
			fprintf(fileLog, "\t%d", int(*((bool *)ScenarioVarPointers[indVar])));
		}
	}
	fprintf(fileLog, "\t%s\n" , fileName);
}

// makes the first line of a CSV log file
void pg_logFirstLineSceneVariables(void) {
	/////////////////////////////////////////////////////////////////////////
	// SCENARIO-CONTROLLED UNIFORM VARIABLES
	fprintf(fileLog, "ID");
	for (int indVar = 0; indVar < _MaxInterpVarIDs; indVar++) {
			fprintf(fileLog, "\t%s", CmdString[indVar] );
	}
	fprintf(fileLog, "\n");
}


///////////////////////////////////////////////////////////////////////////////////
// SCENARIO BASED COMMANDS
///////////////////////////////////////////////////////////////////////////////////
void hide_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		// sprintf(AuxString, "/message_can Exposure_%.1f", scenario_or_gui_command_value); pg_send_message_udp((char *)"s", (char *)AuxString, (char *)"udp_QT_send");
		glutPositionWindow(window_x, window_y);
		// printf(" WIDE\n");
	}
}
void beat_delay_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		sprintf(AuxString, "/beat_delay %.5f", scenario_or_gui_command_value);
		pg_send_message_udp((char *)"f", AuxString, (char *)"udp_PD_send");
	}
}
void pen_color_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		sprintf(AuxString, "/pen_colorPreset_can -1.0");
		pg_send_message_udp((char *)"f", AuxString, (char *)"udp_QT_send");
	}
}
void beat_threshold_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		sprintf(AuxString, "/beat_threshold %.5f", scenario_or_gui_command_value);
		pg_send_message_udp((char *)"f", AuxString, (char *)"udp_PD_send");
	}
}
void soundtrack_onOff_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND) {
		soundtrack_onOff = !soundtrack_onOff;
		sprintf(AuxString, "/soundtrack_onOff %d", int(soundtrack_onOff));
		pg_send_message_udp((char *)"i", AuxString, (char *)"udp_PD_send");
	}
}
void adc_onOff_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND) {
		adc_onOff = !adc_onOff;
		sprintf(AuxString, "/adc_onOff %d", int(adc_onOff));
		pg_send_message_udp((char *)"i", AuxString, (char *)"udp_PD_send");
	}
}
void cameraExposure_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		// sprintf(AuxString, "/message_can Exposure_%.1f", scenario_or_gui_command_value); pg_send_message_udp((char *)"s", (char *)AuxString, (char *)"udp_QT_send");
		if (scenario_or_gui_command_value != CameraCurrent_exposure) {
			printf("Cam exposure %.2f\n", scenario_or_gui_command_value);
			// cvSetCaptureProperty comment for see3cam
			pg_camera_capture.set(CV_CAP_PROP_EXPOSURE, scenario_or_gui_command_value);
			CameraCurrent_exposure = scenario_or_gui_command_value;
			pg_LastCameraParameterChange_Frame = pg_FrameNo;
		}
	}
}
void cameraGain_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		// sprintf(AuxString, "/message_can Exposure_%.1f", scenario_or_gui_command_value); pg_send_message_udp((char *)"s", (char *)AuxString, (char *)"udp_QT_send");
		if (scenario_or_gui_command_value != CameraCurrent_gain) {
			printf("Cam gain %.2f\n", scenario_or_gui_command_value);
			// cvSetCaptureProperty comment for see3cam 
			pg_camera_capture.set(CV_CAP_PROP_GAIN, scenario_or_gui_command_value);
			CameraCurrent_gain = scenario_or_gui_command_value;
			pg_LastCameraParameterChange_Frame = pg_FrameNo;
		}
	}
}
void cameraBrightness_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		// sprintf(AuxString, "/message_can Exposure_%.1f", scenario_or_gui_command_value); pg_send_message_udp((char *)"s", (char *)AuxString, (char *)"udp_QT_send");
		if (scenario_or_gui_command_value != CameraCurrent_brightness) {
			printf("Cam brightness %.2f\n", scenario_or_gui_command_value);
			// cvSetCaptureProperty comment for see3cam
			pg_camera_capture.set(CV_CAP_PROP_BRIGHTNESS, scenario_or_gui_command_value);
			CameraCurrent_brightness = scenario_or_gui_command_value;
			pg_LastCameraParameterChange_Frame = pg_FrameNo;
		}
	}
}
void cameraSaturation_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		// sprintf(AuxString, "/message_can Exposure_%.1f", scenario_or_gui_command_value); pg_send_message_udp((char *)"s", (char *)AuxString, (char *)"udp_QT_send");
		if (scenario_or_gui_command_value != CameraCurrent_saturation) {
			printf("Cam saturation %.2f\n", scenario_or_gui_command_value);
			// cvSetCaptureProperty comment for see3cam 
			pg_camera_capture.set(CV_CAP_PROP_SATURATION, scenario_or_gui_command_value);
			CameraCurrent_saturation = scenario_or_gui_command_value;
			pg_LastCameraParameterChange_Frame = pg_FrameNo;
		}
	}
}
void cameraContrast_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		// sprintf(AuxString, "/message_can Exposure_%.1f", scenario_or_gui_command_value); pg_send_message_udp((char *)"s", (char *)AuxString, (char *)"udp_QT_send");
		if (scenario_or_gui_command_value != CameraCurrent_contrast) {
			printf("Cam contrast %.2f\n", scenario_or_gui_command_value);
			// cvSetCaptureProperty comment for see3cam
			pg_camera_capture.set(CV_CAP_PROP_CONTRAST, scenario_or_gui_command_value);
			CameraCurrent_contrast = scenario_or_gui_command_value;
			pg_LastCameraParameterChange_Frame = pg_FrameNo;
		}
	}
}
void cameraWB_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		// sprintf(AuxString, "/message_can Exposure_%.1f", scenario_or_gui_command_value); pg_send_message_udp((char *)"s", (char *)AuxString, (char *)"udp_QT_send");
		if (scenario_or_gui_command_value != CameraCurrent_WB_B) {
			printf("Cam WB %.2f\n", scenario_or_gui_command_value);
			// cvSetCaptureProperty comment for see3cam
			// pg_camera_capture.set(CV_CAP_PROP_WHITE_BALANCE_BLUE_U, scenario_or_gui_command_value);
			pg_camera_capture.set(CV_CAP_PROP_WHITE_BALANCE_RED_V, scenario_or_gui_command_value);
			CameraCurrent_WB_B = scenario_or_gui_command_value;
			// CameraCurrent_WB_R = scenario_or_gui_command_value;
			pg_LastCameraParameterChange_Frame = pg_FrameNo;
		}
	}
}
void playing_movieNo_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		if (playing_movieNo != currentlyPlaying_movieNo
			&& playing_movieNo >= 0 && playing_movieNo < nb_movies) {
			currentlyPlaying_movieNo = playing_movieNo;
			sprintf(AuxString, "/movie_shortName_can %s", movieShortName[playing_movieNo].c_str());
			pg_send_message_udp((char *)"s", AuxString, (char *)"udp_QT_send");

			// texture ID initialization (should not be inside a thread)
			if (pg_movie_texture_texID == NULL_ID) {
				glGenTextures(1, &pg_movie_texture_texID);
			}

			is_movieLoading = true;
			printf("Loading %s\n", movieFileName[currentlyPlaying_movieNo].c_str());
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
	}
}
void photo_diaporama_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_KEYSTROKE || param_input_type == _PG_SCENARIO) {
		if (scenario_or_gui_command_value >= 0 && scenario_or_gui_command_value != CurrentDiaporamaDir) {
			pg_launch_diaporama();
			CurrentDiaporamaDir = int(scenario_or_gui_command_value);
		}
	}
}
void flashCameraTrkLength_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_KEYSTROKE || param_input_type == _PG_SCENARIO) {
		if (scenario_or_gui_command_value > 0.) {
			flashCameraTrk_decay = 1.f / scenario_or_gui_command_value;
		}
		else {
			flashCameraTrk_decay = 0.001f;
		}
	}
}
void pen_brush_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_KEYSTROKE || param_input_type == _PG_SCENARIO) {
		if (scenario_or_gui_command_value >= 1) { // large radius for the image brushes
			pen_radiusMultiplier = 50.0f;
		}
		else {
			pen_radiusMultiplier = 1.0f;
		}
		// printf("pen_radiusMultiplier %.2f\n", pen_radiusMultiplier);
	}
}
//void pixel_acc_shiftX_callBack( pg_Parameter_Input_Type param_input_type , float scenario_or_gui_command_value) {
//	if(param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_KEYSTROKE) {
//		sprintf( AuxString , "/message_can part_center_HV_%.1f_%.1f" , pixel_acc_shiftX , pixel_acc_shiftY ); pg_send_message_udp( (char *)"s" , (char *)AuxString , (char *)"udp_QT_send" );
//	}
//}
//void pixel_acc_shiftY_callBack( pg_Parameter_Input_Type param_input_type , float scenario_or_gui_command_value) {
//	if(param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_KEYSTROKE) {
//		sprintf( AuxString , "/message_can part_center_HV_%.1f_%.1f" , pixel_acc_shiftX, pixel_acc_shiftY); pg_send_message_udp( (char *)"s" , (char *)AuxString , (char *)"udp_QT_send" );
//	}
//}
void clearAllLayers_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_KEYSTROKE) {
		// printf("delta %.3f\n", (float)(CurrentClockTime - lastClearTime) );
		if (CurrentClockTime - lastClearTime < 0.5) {
			// pg_send_message_udp((char *)"s", (char *)"/message_can clear_all", (char *)"udp_QT_send");
			isClearAllLayers = 1;
			lastClearTime = 0.0;
			// printf("****** isClearAllLayers %d\n",isClearAllLayers);
		}
		else {
			lastClearTime = CurrentClockTime;
		}
	}
	else if (param_input_type == _PG_SCENARIO && scenario_or_gui_command_value > 0.0) {
		// pg_send_message_udp((char *)"s", (char *)"/message_can clear_all", (char *)"udp_QT_send");
		isClearAllLayers = 1;
		lastClearTime = CurrentClockTime;
		// printf("****** isClearAllLayers %d\n", isClearAllLayers);
	}
}
void clearLayer_callBack( pg_Parameter_Input_Type param_input_type , float scenario_or_gui_command_value) {
    if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_KEYSTROKE) {
		if( CurrentClockTime - lastClearTime  < 0.2 ) {
			isClearLayer = 1;
			lastClearTime = 0.0;
			// pg_send_message_udp((char *)"s", (char *)"/message_can clear_all_layers", (char *)"udp_QT_send");
		}
		else {
			isClearLayer = 1;
			lastClearTime = CurrentClockTime;
			// pg_send_message_udp((char *)"s", (char *)"/message_can clear_layer", (char *)"udp_QT_send");
		}
    }
    else if (param_input_type == _PG_SCENARIO && scenario_or_gui_command_value > 0.0 ) {
	    isClearLayer = 1;
        lastClearTime = CurrentClockTime;
	}
}
void sound_env_min_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		sprintf(AuxString, "/sound_env_min %.2f", scenario_or_gui_command_value);
		pg_send_message_udp((char *)"f", AuxString, (char *)"udp_PD_send");
	}
	// printf("reset sound\n");
}
void sound_env_max_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		sprintf(AuxString, "/sound_env_max %.2f", scenario_or_gui_command_value);
		pg_send_message_udp((char *)"f", AuxString, (char *)"udp_PD_send");
	}
	// printf("reset sound\n");
}
void clearCA_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_KEYSTROKE) {
		// pg_send_message_udp((char *)"s", (char *)"/message_can clear_CA", (char *)"udp_QT_send");
		isClearCA = 1;
	}
	else if (param_input_type == _PG_SCENARIO  && scenario_or_gui_command_value > 0.0) {
		isClearCA = 1;
	}
}
void path_replay_trackNo_callBack(int pathNo, pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	int playing_track = -1;
	// for a keystroke or a GUI, the command is the current track whatever its value
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_KEYSTROKE) {
		playing_track = currentDrawingTrack;
	}
	// for a scenario >= 0 -> replay and -1 -> no replay
	else if (param_input_type == _PG_SCENARIO) {
		playing_track = int(scenario_or_gui_command_value);
	}
	 //printf("path replay for path %d track No %d (curently %d) (replay track %d) (1 %d, 2 %d)\n", 
	 //       pathNo, int(scenario_or_gui_command_value), is_path_replay[pathNo], playing_track , path_replay_trackNo_1, path_replay_trackNo_2);
	// is not currently reading -> starts reading if it is a valid track number
	if (!is_path_replay[pathNo]) {
		// does not change anything if it is not a valid track
		if (playing_track < 0 || playing_track >= PG_NB_TRACKS) {
			return;
		}

		// only reads a track that has been recorded
		// assumes that no svg track has been loaded for this track
		if (recorded_path[pathNo] == true
#ifdef ATELIERS_PORTATIFS
			|| pathNo == 1  // track for the signal
#endif
			) {
			if (tracksSync) {
				synchr_start_path_replay_trackNo[pathNo] = playing_track;
			}
			else {
				pg_path_replay_trackNo_onOff(pathNo, playing_track);
			}
     	}
    }
	// is currently reading && playing_track < 0 (scenario) or on/off command -> stops reading 
	else {
		if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_KEYSTROKE) {
			playing_track = -1;
		}
		if (playing_track == -1) {
			pg_path_replay_trackNo_onOff(pathNo, playing_track);
		}
	}
	// otherwise do nothing
}

#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7
void path_replay_trackNo_1_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	path_replay_trackNo_callBack(1, param_input_type, scenario_or_gui_command_value);
}
void path_replay_trackNo_2_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	path_replay_trackNo_callBack(2, param_input_type, scenario_or_gui_command_value);
}
void path_replay_trackNo_3_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	path_replay_trackNo_callBack(3, param_input_type, scenario_or_gui_command_value);
}
#endif
#if PG_NB_PATHS == 7
void path_replay_trackNo_4_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	path_replay_trackNo_callBack(4, param_input_type, scenario_or_gui_command_value);
}
void path_replay_trackNo_5_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	path_replay_trackNo_callBack(5, param_input_type, scenario_or_gui_command_value);
}
void path_replay_trackNo_6_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	path_replay_trackNo_callBack(6, param_input_type, scenario_or_gui_command_value);
}
void path_replay_trackNo_7_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	path_replay_trackNo_callBack(7, param_input_type, scenario_or_gui_command_value);
}
#endif
void path_record_callBack(int pathNo, pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	bool isTrackRecord = false;
	switch (pathNo) {
#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7
#ifndef ATELIERS_PORTATIFS
	case 1:
		isTrackRecord = path_record_1;
		break;
#endif
	case 2:
		isTrackRecord = path_record_2;
		break;
	case 3:
		isTrackRecord = path_record_3;
		break;
#endif
#if PG_NB_PATHS == 7
	case 4:
		isTrackRecord = path_record_4;
		break;
	case 5:
		isTrackRecord = path_record_5;
		break;
	case 6:
		isTrackRecord = path_record_6;
		break;
	case 7:
		isTrackRecord = path_record_7;
		break;
#endif
	}
	//keystroke on/off command
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_KEYSTROKE) {
		// is not currently recording -> starts recording 
		if (!isTrackRecord) {
			recorded_path[pathNo] = true;
			if (tracksSync) {
				synchr_start_recording_path[pathNo] = true;
			}
			else {
				pg_path_recording_onOff(pathNo);
			}
		}
		// is currently recording -> stops recording 
		else {
			pg_path_recording_onOff(pathNo);
		}
	}
	// scenario absolute command
	if (param_input_type == _PG_SCENARIO) {
		if (scenario_or_gui_command_value > 0) {
			// starts recording 
			if (!isTrackRecord) {
				recorded_path[pathNo] = true;
				if (tracksSync) {
					synchr_start_recording_path[pathNo] = true;
				}
				else {
					pg_path_recording_onOff(pathNo);
				}
			}
		}
		else {
			// stops recording 
			if (isTrackRecord) {
				pg_path_recording_onOff(pathNo);
			}
		}
	}
}
#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7
void path_record_1_callBack( pg_Parameter_Input_Type param_input_type , float scenario_or_gui_command_value ) {
  path_record_callBack( 1 , param_input_type , scenario_or_gui_command_value);
}
void path_record_2_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	path_record_callBack(2, param_input_type, scenario_or_gui_command_value);
}
void path_record_3_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	path_record_callBack(3, param_input_type, scenario_or_gui_command_value);
}
#endif
#if PG_NB_PATHS == 7
void path_record_4_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	path_record_callBack(4, param_input_type, scenario_or_gui_command_value);
}
void path_record_5_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	path_record_callBack(5, param_input_type, scenario_or_gui_command_value);
}
void path_record_6_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	path_record_callBack(6, param_input_type, scenario_or_gui_command_value);
}
void path_record_7_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	path_record_callBack(7, param_input_type, scenario_or_gui_command_value);
}
#endif
void auto_beat_callBack( pg_Parameter_Input_Type param_input_type , float scenario_or_gui_command_value ) {
  lastBeatTime = CurrentClockTime;
}
void part_move_init(void) {
#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7
	part_path_follow_0 = false;
	part_path_follow_1 = false;
	part_path_follow_2 = false;
	part_path_follow_3 = false;
	part_path_repulse_0 = false;
	part_path_repulse_1 = false;
	part_path_repulse_2 = false;
	part_path_repulse_3 = false;
#endif
#if PG_NB_PATHS == 7
	part_path_follow_4 = false;
	part_path_follow_5 = false;
	part_path_follow_6 = false;
	part_path_follow_7 = false;
	part_path_repulse_4 = false;
	part_path_repulse_5 = false;
	part_path_repulse_6 = false;
	part_path_repulse_7 = false;
#endif
	partMove_target = false;
	partMove_rand = false;
}
void part_path_follow_callBack(int pathNo, pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	bool state = false;
#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7
	switch (pathNo) {
	case 0:
		state = part_path_follow_0;
		break;
	case 1:
		state = part_path_follow_1;
		break;
	case 2:
		state = part_path_follow_2;
		break;
	case 3:
		state = part_path_follow_3;
		break;
	}
#endif
#if PG_NB_PATHS == 7
	switch (pathNo) {
	case 4:
		state = part_path_follow_4;
		break;
	case 5:
		state = part_path_follow_5;
		break;
	case 6:
		state = part_path_follow_6;
		break;
	case 7:
		state = part_path_follow_7;
		break;
	}
#endif
	if (!state)
		return; // nothing to do for null values
	part_move_init();
#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7
	switch (pathNo) {
	case 0:
		part_path_follow_0 = state;
		break;
	case 1:
		part_path_follow_1 = state;
		break;
	case 2:
		part_path_follow_2 = state;
		break;
	case 3:
		part_path_follow_3 = state;
		break;
	}
#endif
#if PG_NB_PATHS == 7
	switch (pathNo) {
	case 4:
		part_path_follow_4 = state;
		break;
	case 5:
		part_path_follow_5 = state;
		break;
	case 6:
		part_path_follow_6 = state;
		break;
	case 7:
		part_path_follow_7 = state;
		break;
	}
#endif
}
void part_path_repulse_callBack(int pathNo, pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	bool state = false;
#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7
	switch (pathNo) {
	case 0:
		state = part_path_repulse_0;
		break;
	case 1:
		state = part_path_repulse_1;
		break;
	case 2:
		state = part_path_repulse_2;
		break;
	case 3:
		state = part_path_repulse_3;
		break;
	}
#endif
#if PG_NB_PATHS == 7
	switch (pathNo) {
	case 4:
		state = part_path_repulse_4;
		break;
	case 5:
		state = part_path_repulse_5;
		break;
	case 6:
		state = part_path_repulse_6;
		break;
	case 7:
		state = part_path_repulse_7;
		break;
	}
#endif
	if (!state)
		return; // nothing to do for null values
	part_move_init();
#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7
	switch (pathNo) {
	case 0:
		part_path_repulse_0 = state;
		break;
	case 1:
		part_path_repulse_1 = state;
		break;
	case 2:
		part_path_repulse_2 = state;
		break;
	case 3:
		part_path_repulse_3 = state;
		break;
	}
#endif
#if PG_NB_PATHS == 7
	switch (pathNo) {
	case 4:
		part_path_repulse_4 = state;
		break;
	case 5:
		part_path_repulse_5 = state;
		break;
	case 6:
		part_path_repulse_6 = state;
		break;
	case 7:
		part_path_repulse_7 = state;
		break;
	}
#endif
}
#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7
void part_path_follow_0_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	part_path_follow_callBack(0, param_input_type, scenario_or_gui_command_value);
}
void part_path_follow_1_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	part_path_follow_callBack(1, param_input_type, scenario_or_gui_command_value);
}
void part_path_follow_2_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	part_path_follow_callBack(2, param_input_type, scenario_or_gui_command_value);
}
void part_path_follow_3_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	part_path_follow_callBack(3, param_input_type, scenario_or_gui_command_value);
}
#endif
#if PG_NB_PATHS == 7
void part_path_follow_4_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	part_path_follow_callBack(4, param_input_type, scenario_or_gui_command_value);
}
void part_path_follow_5_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	part_path_follow_callBack(5, param_input_type, scenario_or_gui_command_value);
}
void part_path_follow_6_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	part_path_follow_callBack(6, param_input_type, scenario_or_gui_command_value);
}
void part_path_follow_7_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	part_path_follow_callBack(7, param_input_type, scenario_or_gui_command_value);
}
#endif
#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7
void part_path_repulse_0_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	part_path_repulse_callBack(0, param_input_type, scenario_or_gui_command_value);
}
void part_path_repulse_1_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	part_path_repulse_callBack(1, param_input_type, scenario_or_gui_command_value);
}
void part_path_repulse_2_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	part_path_repulse_callBack(2, param_input_type, scenario_or_gui_command_value);
}
void part_path_repulse_3_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	part_path_repulse_callBack(3, param_input_type, scenario_or_gui_command_value);
}
#endif
#if PG_NB_PATHS == 7
void part_path_repulse_4_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	part_path_repulse_callBack(4, param_input_type, scenario_or_gui_command_value);
}
void part_path_repulse_5_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	part_path_repulse_callBack(5, param_input_type, scenario_or_gui_command_value);
}
void part_path_repulse_6_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	part_path_repulse_callBack(6, param_input_type, scenario_or_gui_command_value);
}
void part_path_repulse_7_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	part_path_repulse_callBack(7, param_input_type, scenario_or_gui_command_value);
}
#endif
void partMove_target_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	// printf("Particle move target %d\n" , int(partMove_target));
}
void partMove_rand_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	bool state = partMove_rand;
	if (!state)
		return; // nothing to do for null values
	part_move_init();
	partMove_rand = state;
}
void part_initialization_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	pg_targetFrameNo = pg_FrameNo + int(part_timeToTargt);
}


///////////////////////////////////////////////////////////////////////////////////
// SCENARIO BASED COMMANDS
///////////////////////////////////////////////////////////////////////////////////
void pg_update_variable(pg_Parameter_Input_Type param_input_type,
		      pg_Keystroke_Input_Type parm_keystroke_type,
		      int indParam,
		      float scenario_or_gui_command_value ) {
	// save previous values of variables that have to know their previous value in their 

	//float step = 0.0f;
	//if (parm_keystroke_type == _PG_KEYSTROKE_MINUS) {
	//	step = StepMinus[indParam];
	//}
	//else {
	//	step = StepPlus[indParam];
	//}
	if (param_input_type == _PG_SCENARIO) {
		if (BrokenInterpolationVar[indParam]) {
			return;
		}
	}
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_KEYSTROKE) {
		BrokenInterpolationVar[indParam] = true;
	}
	
	if (ScenarioVarTypes[indParam] == _pg_float) {
		if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
			*((float *)ScenarioVarPointers[indParam]) = scenario_or_gui_command_value;
		}
		//else if (param_input_type == _PG_KEYSTROKE) {
		//	*((float *)ScenarioVarPointers[indParam]) += step;
		//}
		if (ScenarioVarCallbacks[indParam]) {
			(*ScenarioVarCallbacks[indParam])(param_input_type,scenario_or_gui_command_value);
		}
	}
	else if (ScenarioVarTypes[indParam] == _pg_int) {
		if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
			if (indParam == _CAType) {
				// for CAType we choose to alternate randomly between both types, according
				// to the proximity of floor or ceiling
				float randVal = (float)rand() / (float)RAND_MAX;
				float thefloor = floor(scenario_or_gui_command_value);
				float decimalPart = scenario_or_gui_command_value - thefloor;
				if (randVal > decimalPart) {
					*((int *)ScenarioVarPointers[indParam]) = (int)thefloor;
				}
				if (randVal < decimalPart) {
					*((int *)ScenarioVarPointers[indParam]) = (int)thefloor + 1;
				}
			}
			else {
				*((int *)ScenarioVarPointers[indParam]) = (int)round(scenario_or_gui_command_value);
			}
		}
		//else if (param_input_type == _PG_KEYSTROKE) {
		//	*((int *)ScenarioVarPointers[indParam]) = (*((int *)ScenarioVarPointers[indParam]) + (int)step + (int)MaxValues[indParam] + 1) % ((int)MaxValues[indParam] + 1);
		//}
		if (ScenarioVarCallbacks[indParam]) {
			(*ScenarioVarCallbacks[indParam])(param_input_type,scenario_or_gui_command_value);
		}
	}
	else if (ScenarioVarTypes[indParam] == _pg_bool) {
		if (param_input_type == _PG_SCENARIO) {
			*((bool *)ScenarioVarPointers[indParam]) = (scenario_or_gui_command_value > 0 ? true : false);
		}
		else if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_KEYSTROKE) {
			*((bool *)ScenarioVarPointers[indParam]) = !(*((bool *)ScenarioVarPointers[indParam]));
			// printf("Opposite boolean index %d value %d\n" , indParam,*((bool *)ScenarioVarPointers[indParam]));
		}
		if (ScenarioVarCallbacks[indParam]) {
			(*ScenarioVarCallbacks[indParam])(param_input_type,scenario_or_gui_command_value);
		}
	}
	else if (ScenarioVarTypes[indParam] == _pg_sign) {
		if (param_input_type == _PG_SCENARIO) {
			*((float *)ScenarioVarPointers[indParam]) = (scenario_or_gui_command_value > 0 ? 1.0f : -1.0f);
		}
		else if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_KEYSTROKE) {
			*((float *)ScenarioVarPointers[indParam]) = -1.0f * (*((float *)ScenarioVarPointers[indParam]));
		}
		if (ScenarioVarCallbacks[indParam]) {
			(*ScenarioVarCallbacks[indParam])(param_input_type,scenario_or_gui_command_value);
		}
	}
	else if (ScenarioVarTypes[indParam] == _pg_path) {
		if (ScenarioVarCallbacks[indParam]) {
			(*ScenarioVarCallbacks[indParam])(param_input_type,scenario_or_gui_command_value);
		}
	}
}

void StartNewScene(int ind_scene) {
	pg_CurrentScene = ind_scene;
	pg_FirstFrameInScene = true;
	sprintf(AuxString, "/setup_can %s", Scenario[pg_CurrentScene].scene_IDs.c_str()); pg_send_message_udp((char *)"s", AuxString, (char *)"udp_QT_send");
	// printf( "/setup_can %s\n" , Scenario[pg_CurrentScene].scene_IDs.c_str() ); 
	// reinitialization of the interpolation control variables at the beginning of a new scene
	for (int indVar = 0; indVar < _MaxInterpVarIDs; indVar++) {
		BrokenInterpolationVar[indVar] = false;
	}
}

void pg_update_scenario(void) {
	float elapsed_time_from_start = (float)(CurrentClockTime - InitialScenarioTime);
	// scans all scenes to find the current one and applies the corresponding interpolations
	for (int ind_scene = 0; ind_scene < nb_scenes; ind_scene++) {
		// printf( "time %.2f beg %.2f end %.2f\n" , elapsed_time_from_start ,  Scenario[ind_scene].scene_initial_time ,  Scenario[ind_scene].scene_final_time );
		// current scene found
		if (elapsed_time_from_start >= Scenario[ind_scene].scene_initial_time
			&& elapsed_time_from_start <= Scenario[ind_scene].scene_final_time) {
			// the current scene is finished 
			if (ind_scene != pg_CurrentScene) {
				// a new scene is launched only if Scenario[pg_CurrentScene].scene_change_when_ends
				if (Scenario[pg_CurrentScene].scene_change_when_ends) {
					StartNewScene(ind_scene);
				}
				// otherwise the current scene is prolonged, no interpolation takes place 
				// the parameter values stay as they were 
				else {
					// for time display + colors when reaching the end of the scene
					remainingTimeInScene
						= Scenario[pg_CurrentScene].scene_initial_time + Scenario[pg_CurrentScene].scene_duration - elapsed_time_from_start;
					// printf( "time %.2f remainingTimeInScene %.2f\n" , elapsed_time_from_start , remainingTimeInScene);
					return;
				}
			}

			// for time display + colors when reaching the end of the scene
			remainingTimeInScene
				= Scenario[ind_scene].scene_initial_time + Scenario[ind_scene].scene_duration - elapsed_time_from_start;
			// printf( "\ntime %.2f interp %.2f " , elapsed_time_from_start , coefInterpolation );

			// transformed interpolation according to the interpolation function
			for (int indP = 0; indP < _MaxInterpVarIDs; indP++) {
				if (Scenario[ind_scene].scene_interpolations[indP].interpolation_mode == pg_keep_value) {
					continue;
				}
				// going from interpolation percentages to interpolation times by multiplying by scene duration
				float absoluteOffset = Scenario[ind_scene].scene_interpolations[indP].offSet
					* Scenario[ind_scene].scene_duration;
				float absoluteDuration = Scenario[ind_scene].scene_interpolations[indP].duration
					* Scenario[ind_scene].scene_duration;

				float transformedInterpolation = 0.0f;
				float twoStepInterpolation[3] = { 0.0f , 0.0f , 0.0f };

				if (elapsed_time_from_start <= Scenario[ind_scene].scene_initial_time
					+ absoluteOffset) {
					transformedInterpolation = 0.0;
				}
				else if (elapsed_time_from_start >= Scenario[ind_scene].scene_initial_time
					+ absoluteOffset + absoluteDuration) {
					transformedInterpolation = 1.0;
				}
				else {
					float coefInterpolation
						= elapsed_time_from_start - (Scenario[ind_scene].scene_initial_time + absoluteOffset);
					// time normalization for interpolation
					if (absoluteDuration > 0.0) {
						coefInterpolation /= absoluteDuration;
					}
					if (coefInterpolation < 0.0)coefInterpolation = 0.0;
					if (coefInterpolation > 1.0)coefInterpolation = 1.0;

					switch (Scenario[ind_scene].scene_interpolations[indP].interpolation_mode) {
					case pg_linear_interpolation:
						transformedInterpolation = coefInterpolation;
						break;
					case pg_cosine_interpolation:
						transformedInterpolation
							= 0.5F * (cos((coefInterpolation - 1.0F) * (float)M_PI) + 1.0F);
						break;
					case pg_bell_interpolation:
						transformedInterpolation = coefInterpolation;
						if (coefInterpolation < 0.5f) {
							twoStepInterpolation[0] = 0.5F * (cos(2.0f * transformedInterpolation * (float)M_PI) + 1.0F);
							twoStepInterpolation[1] = 1.0f - twoStepInterpolation[0];
							twoStepInterpolation[2] = 0.0f;
						}
						else {
							twoStepInterpolation[0] = 0.0f;
							twoStepInterpolation[1] = 1.0f - 0.5F * (cos(2.0f * transformedInterpolation  * (float)M_PI) + 1.0F);
							twoStepInterpolation[2] = 1.0f - twoStepInterpolation[1];
						}
						break;
					case pg_sawtooth_interpolation:
						transformedInterpolation = coefInterpolation;
						if (coefInterpolation < 0.5f) {
							twoStepInterpolation[0] = 1.0f - 2.0f * transformedInterpolation;
							twoStepInterpolation[1] = 1.0f - twoStepInterpolation[0];
							twoStepInterpolation[2] = 0.0f;
						}
						else {
							twoStepInterpolation[0] = 0.0f;
							twoStepInterpolation[1] = 2.0f - 2.0f * transformedInterpolation;;
							twoStepInterpolation[2] = 1.0f - twoStepInterpolation[1];
						}
						break;
					default:
						printf("Stepwise interpolations have no intermediate values\n");
						break;
					}
				}

				if ((transformedInterpolation == 0.0 && pg_FirstFrameInScene)
					|| transformedInterpolation > 0.0) {
					// calculation of the interpolated value and assignment to variable
					float interpolated_value = Scenario[ind_scene].scene_initial_parameters[indP];
					if (Scenario[ind_scene].scene_interpolations[indP].interpolation_mode != pg_bell_interpolation
						&& Scenario[ind_scene].scene_interpolations[indP].interpolation_mode != pg_sawtooth_interpolation) {
						interpolated_value
							= (1.0F - transformedInterpolation)
							* Scenario[ind_scene].scene_initial_parameters[indP]
							+ transformedInterpolation
							* Scenario[ind_scene].scene_final_parameters[indP];

						// printf( "Scene %d param %d time from start %.2f interp %.2f transf_interp %.2f value %.2f start interp %.2f duration %.2f\n" , ind_scene, indP, elapsed_time_from_start , coefInterpolation , transformedInterpolation , interpolated_value ,Scenario[ind_scene].scene_initial_time + scene_interpolations[ ind_scene ][ indP ].offSet , scene_interpolations[ ind_scene ][ indP ].duration );
					}
					// 3-valued bell interpolation between initial, median and end value
					else {
						interpolated_value
							= twoStepInterpolation[0]
							* Scenario[ind_scene].scene_initial_parameters[indP]
							+ twoStepInterpolation[1]
							* Scenario[ind_scene].scene_interpolations[indP].midTermValue
							+ twoStepInterpolation[2]
							* Scenario[ind_scene].scene_final_parameters[indP];

						// printf( "Scene %d param %d time from start %.2f interp %.2f transf_interp %.2f value %.2f start interp %.2f duration %.2f\n" , ind_scene, indP, elapsed_time_from_start , coefInterpolation , transformedInterpolation , interpolated_value ,Scenario[ind_scene].scene_initial_time + scene_interpolations[ ind_scene ][ indP ].offSet , scene_interpolations[ ind_scene ][ indP ].duration );
					}
					pg_update_variable(_PG_SCENARIO, _PG_KEYSTROKE_VOID,
						indP, interpolated_value);
				}
			}
			pg_FirstFrameInScene = false;
			return;
		}
		// current scene found
	}
	// scans all scenes to find the current one and applies the corresponding interpolations
}


///////////////////////////////////////////////////////////////////////////////////
// KEYSTROKE BASED COMMANDS
///////////////////////////////////////////////////////////////////////////////////

void pg_process_key( int key ) {
  int mod = glutGetModifiers();

  // printf( "key (%d) mod %d\n" , key , mod );

  if(!(mod & GLUT_ACTIVE_ALT) ) {
    // non special key that corresponds to a script
    pg_keyStrokeScripts( key );
    return;
  }

  switch (key) {
  
	  /* ------------------------------- check connection to QT */
  case 'q':
	  pg_send_message_udp((char *)"", (char *)"/QT_connected", (char *)"udp_QT_send");
	  break;

	  /* ------------------------------- check connection to PD */
  case 'p':
	  pg_send_message_udp((char *)"", (char *)"/PD_connected", (char *)"udp_PD_send");
	  break;

	  /* ------------------------------- frame per second */
  case 'f':
	  DisplayFramePerSecond = !DisplayFramePerSecond;
	  printf("FPS display %d\n", DisplayFramePerSecond);
	  break;

	  /* ------------------------------- snapshot */
  case 's':
    pg_draw_scene( _Jpg );
    break;
  case 'S':
    pg_draw_scene( _Svg );
    break;

  /* ------------------------------- beat */
  case 'b':
    auto_beat = !auto_beat;
    lastBeatTime = CurrentClockTime;
    break;
    
  /* ------------------------------- current video background capture */
  case 'v':
	  reset_camera = true;
	  break;

 default:
    printf ("key %d is not active.\n", key);
    break;
  }     
}

/*!
 * \brief special keys events (interactive mode)
 * \param key			the key pressed
 */
void pg_process_special_key( int key ) {
  // switch (key) {    
  // default:
  //   printf ("special key %d is not active.\n", key);
  //   break;
  // }   
}

void launch(void) {
	// sprintf(AuxString, "/message_can launching"); pg_send_message_udp((char *)"s", AuxString, (char *)"udp_QT_send");
	
	// cleaning up
	isClearCA = 1; // clean CA layer
	isClearAllLayers = 1; // clears bg layer
	initCA = 1.0f;
	mute_second_screen = false; // double screen
	sprintf(AuxString, "/mute_second_screen_can %d", int(mute_second_screen));
	pg_send_message_udp((char *)"i", AuxString, (char *)"udp_QT_send");

	pg_LaunchFrameNo = pg_FrameNo;

	// restarts scenario
	restoreInitialTimesAndDurations();
	InitialScenarioTime = CurrentClockTime - Scenario[0].scene_initial_time;
	AbsoluteInitialScenarioTime = CurrentClockTime - Scenario[0].scene_initial_time;
	sprintf(AuxString, "/setup_can %s", Scenario[0].scene_IDs.c_str()); pg_send_message_udp((char *)"s", AuxString, (char *)"udp_QT_send");
	// printf( "/setup_can %s" , Scenario[0].scene_IDs.c_str() ); 
	// reinitialization of the interpolation control variables at the beginning of a new scene
	for (int indVar = 0; indVar < _MaxInterpVarIDs; indVar++) {
		BrokenInterpolationVar[indVar] = false;
	}

	// removes snaptshots before launching
	remove_files_in_dir(&snapshots_dir_path_name); // removes snapshots before launch

	// lights up the LED
	pg_send_message_udp((char *)"f", (char *)"/launch_can 1", (char *)"udp_QT_send");

	// launches first soundtrack
	if (nb_soundtracks > 0) {
		currentlyPlaying_trackNo = 0;
		sprintf(AuxString, "/soundtrack_fileName %s",
			("Data/" + project_name + "-data/soundtracks/" + trackFileName[currentlyPlaying_trackNo]).c_str());
		pg_send_message_udp((char *)"s", AuxString, (char *)"udp_PD_send");
		sprintf(AuxString, "/soundtrack_shortName_can %s", trackShortName[currentlyPlaying_trackNo].c_str());
		pg_send_message_udp((char *)"s", AuxString, (char *)"udp_QT_send");
		BrokenInterpolationVar[_soundtrack_onOff] = true;
		soundtrack_onOff = true;
	}

	StartNewScene(0);
}

void setup_plus(void) {
	if (pg_CurrentScene < 0) {
		launch();
	}
	else {
		restoreInitialTimesAndDurations();
		int new_scene = ((1 + pg_CurrentScene) % nb_scenes);
		InitialScenarioTime = CurrentClockTime - Scenario[new_scene].scene_initial_time;
		StartNewScene(new_scene);
	}
}

void setup_minus(void) {
	restoreInitialTimesAndDurations();
	int new_scene = ((pg_CurrentScene - 1 + nb_scenes) % nb_scenes);
	InitialScenarioTime = CurrentClockTime - Scenario[new_scene].scene_initial_time;
	StartNewScene(new_scene);
}

void pg_keyStrokeScripts(int key) {
  pg_Keystroke_Input_Type CommandType = _PG_KEYSTROKE_VOID;
  //char *CmdChar = NULL;
  //int indChar = -1;
  //bool oneCommandIssued = false;

  //char * ptCh = CmdCharMinus;
  //while ((CmdChar = strchr(ptCh,key)) != NULL) {
	 // CommandType = _PG_KEYSTROKE_MINUS;
	 // indChar = CmdChar - CmdCharMinus;
	 // if (indChar >= 0) {
		//  pg_update_variable(_PG_KEYSTROKE, CommandType,
		//		    indChar, 0.0F );
		//  oneCommandIssued = true;
	 // }
	 // ptCh = CmdChar + 1;
  //}
  //if( oneCommandIssued ) {
  //  return;
  //}
  //ptCh = CmdCharPlus;
  //if ((CmdChar = strchr(ptCh,key)) != NULL) {
	 // CommandType = _PG_KEYSTROKE_PLUS;
	 // indChar = CmdChar - CmdCharPlus;
	 // // printf("indChar %d\n", indChar);
	 // if (indChar >= 0) {
		//  pg_update_variable(_PG_KEYSTROKE, CommandType,
		//		    indChar, 0.0F );
		//  oneCommandIssued = true;
	 // }
	 // ptCh = CmdChar + 1;
  //}
  //if( oneCommandIssued ) {
  //  return;
  //}

  // printf("indChar %d pixel_acc %d\n", indChar, _pixel_acc);

  switch (key) {
  
    // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
    // +++++++++++++++++ TEST UDP ++++++++++++++++++++++++++++++ 
    // +++++++++++++++++   keystroke (T)  ++++++++++++++++++++++ 
    // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
  case'T':
    pg_writeMessageOnScreen( (char * )"******" );
    pg_send_message_udp( (char *)"f" , (char *)"/testUDP 0" , (char *)"udp_SC" );
    sprintf( AuxString , "/message_can TEST_UDP" ); pg_send_message_udp( (char *)"s" , AuxString , (char *)"udp_QT_send" );
    sprintf( AuxString , "/return_message returnUPD_%f" , CurrentClockTime ); pg_send_message_udp( (char *)"s" , AuxString , (char *)"udp_QT_send" );
    break;
    
    // ======================================== 
    // background subtraction modes (Z) 
    // ======================================== 
  case'y':
     camera_BG_subtr = !camera_BG_subtr;
     BrokenInterpolationVar[ _camera_BG_subtr ] = true;
    break;

    // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
    // +++++++++++++++++ LAUNCH TIMER ++++++++++++++++++++++++++ 
    // +++++++++++++++++   keystroke (t)  ++++++++++++++++++++++ 
    // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
  case't':
	  launch();
	  break;

    // ======================================== 
    // loop size (number of beats in each loop) (V) 
    // ======================================== 
  case'V':
	  flashCABG_freq = (flashCABG_freq < PG_LOOP_SIZE ? 2 * flashCABG_freq : 1);
	  flashCAPart_freq = (flashCAPart_freq < PG_LOOP_SIZE ? 2 * flashCAPart_freq : 1);
	  flashPartBG_freq = (flashPartBG_freq < PG_LOOP_SIZE ? 2 * flashPartBG_freq : 1);
	  flashPartCA_freq = (flashPartCA_freq < PG_LOOP_SIZE ? 2 * flashPartCA_freq : 1);

	  flashPixel_freq = ( flashPixel_freq < PG_LOOP_SIZE ? 2 * flashPixel_freq : 1 );
	  // printf( "Loop size (PG_LOOP_SIZE)\n" );
	  BrokenInterpolationVar[_flashTrkCA_freq_0] = true;
#if PG_NB_TRACKS >= 2
	  BrokenInterpolationVar[_flashTrkCA_freq_1] = true;
#endif
#if PG_NB_TRACKS >= 3
	  BrokenInterpolationVar[_flashTrkCA_freq_2] = true;
#endif
#if PG_NB_TRACKS >= 4
	  BrokenInterpolationVar[_flashTrkCA_freq_3] = true;
#endif
	  // printf( "Loop size (PG_LOOP_SIZE)\n" );
	  BrokenInterpolationVar[_flashTrkPart_freq_0] = true;
#if PG_NB_TRACKS >= 2
	  BrokenInterpolationVar[_flashTrkPart_freq_1] = true;
#endif
#if PG_NB_TRACKS >= 3
	  BrokenInterpolationVar[_flashTrkPart_freq_2] = true;
#endif
#if PG_NB_TRACKS >= 4
	  BrokenInterpolationVar[_flashTrkPart_freq_3] = true;
#endif
	  BrokenInterpolationVar[_flashCABG_freq] = true;
	  BrokenInterpolationVar[_flashCAPart_freq] = true;
	  BrokenInterpolationVar[_flashPartBG_freq] = true;
	  BrokenInterpolationVar[_flashPartCA_freq] = true;
	  BrokenInterpolationVar[ _flashPixel_freq ] = true;
	break;

  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
  // +++++++++++++ FLASHES BETWEEN LAYERS, CA & PARTICLES ++++ 
  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
  // flashPixel          (f)                 
  // flashTrkPart            (F)                 
  // flashTrkCA            (h)                 
  // flashCABG          (H)                 
  // flashCAPart          (i)                 
  // flashPixel duration (Q)                 
  // ====================================== 
  case'f':
    flashPixel = flashPixel_duration;
    break;
    
  case'F':
	  flashTrkPart_weights[currentDrawingTrack] = 1.0;
	  break;

  case'h':
	  flashTrkCA_weights[currentDrawingTrack] = 1.0;
	  break;

  case'H':
	  flashCABG_weight = 1.0;

  case'i':
	  flashCAPart_weight = 1.0;
	  break;
 
	  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	  /* ------------------------------- dump particle animation FBO values */
	  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
  case 'w':
  {
	  int nbLines = nb_particles / leftWindowWidth + 1;
	  float *storageBuffer = new float[leftWindowWidth * nbLines * 4];

	  printOglError(97);
	  glBindFramebuffer(GL_READ_FRAMEBUFFER, pg_FBO_Update[((pg_FrameNo + 1) % 2)]);
	  glReadBuffer(GL_COLOR_ATTACHMENT0 + pg_ParticlePosSpeed_FBO_attcht);

	  // OpenGL's default 4 byte pack alignment would leave extra bytes at the
	  //   end of each image row so that each full row contained a number of bytes
	  //   divisible by 4.  Ie, an RGB row with 3 pixels and 8-bit componets would
	  //   be laid out like "RGBRGBRGBxxx" where the last three "xxx" bytes exist
	  //   just to pad the row out to 12 bytes (12 is divisible by 4). To make sure
	  //   the rows are packed as tight as possible (no row padding), set the pack
	  //   alignment to 1.
	  // glPixelStorei(GL_PACK_ALIGNMENT, 1);
	  glReadPixels(0, 0, leftWindowWidth, nbLines, GL_RGBA, GL_FLOAT, (float *)storageBuffer);
	  glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

	  // prints first line
	  for (int indCol = 0; indCol < leftWindowWidth; indCol++) {
		  fprintf(fileLog, "\t%d\t-\t-\t-", indCol);
	  }
	  fprintf(fileLog, "\n");
	  // prints data line
	  for (int indRow = 0; indRow < nbLines; indRow++) {
		  for (int indCol = 0; indCol < leftWindowWidth; indCol++) {
			  int indBuff = indRow * leftWindowWidth * 4 + indCol * 4;
			  if (indCol % 2 == 0) {
				  fprintf(fileLog, "\tHpx:%.2f\tHpy:%.2f\tHvx:%.2f\tHvy:%.2f",
					  storageBuffer[indBuff + 0], storageBuffer[indBuff + 1],
					  storageBuffer[indBuff + 2], storageBuffer[indBuff + 3]);
			  }
			  else {
				  fprintf(fileLog, "\tTpx:%.2f\tTpy:%.2f\tTvx:%.2f\tTvy:%.2f",
					  storageBuffer[indBuff + 0], storageBuffer[indBuff + 1],
					  storageBuffer[indBuff + 2], storageBuffer[indBuff + 3]);
			  }
		  }
		  fprintf(fileLog, "\n");
	  }
	  delete[] storageBuffer;
	  printf("FBO rendering completed\n");
	  printOglError(101);
  }
  break;

  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
  // +++++++++++++++++ SET-UP ++++++++++++++++++++++++++++++++ 
  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
  // ====================================== 
  // advances to next scene without respect for the global timing
  case'S':
	  setup_plus();
	  break;

  // advances to next scene and keeps the global timing
  case's':
  {
	  restoreInitialTimesAndDurations();
	  float elapsedTime = (float)(CurrentClockTime - InitialScenarioTime) - Scenario[pg_CurrentScene].scene_initial_time;
	  // only accepted if the current scene has been on for a while
	  if (elapsedTime > 10) {
		  float deltaTime = Scenario[1 + pg_CurrentScene].scene_initial_time - (float)(CurrentClockTime - InitialScenarioTime);
		  // if the scene has not yet begun
		  if (deltaTime > 0) {
			  int new_scene = ((1 + pg_CurrentScene) % nb_scenes);
			  // we place the beginning of the current scene at this time
			  Scenario[new_scene].scene_initial_time -= deltaTime;
			  if (new_scene > 0) {
				  Scenario[new_scene - 1].scene_final_time -= deltaTime;
				  Scenario[new_scene - 1].scene_duration -= deltaTime;
			  }
			  Scenario[new_scene].scene_duration += deltaTime;
			  // unuseful because is already made through Scenario[new_scene].scene_initial_time -= deltaTime;
			  //InitialScenarioTime = CurrentClockTime - Scenario[new_scene].scene_initial_time;
			  StartNewScene(new_scene);
		  }
	  }
  }
  break;
  }
}

void pg_continuous_flahes(void) {
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// +++++++++++++++++ FLASHES +++++++++++++++++++++++++++++++ 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// ======================================== 
	// flash triggering according to beat & frequency, weight, and on/off values 
	// ======================================== 
	// flashes are synchronized on beats according to their frequency
	if (flashPixel_freq > PG_LOOP_SIZE) { // if flashPixel_freq > PG_LOOP_SIZE -> update every frame
		flashPixel = flashPixel_duration;
	}

	if (flashTrkCA_freq_0 > PG_LOOP_SIZE) { // if flashTrkCA_freq_0 > PG_LOOP_SIZE -> update every frame
		flashTrkCA_weights[0] = 1.0;
		// printf( "flashTrkCA (%d)\n" , flashTrkCA_freq_0 );
	}
#if PG_NB_TRACKS >= 2
	if (flashTrkCA_freq_1 > PG_LOOP_SIZE) { // if flashTrkCA_freq_1 > PG_LOOP_SIZE -> update every frame
		flashTrkCA_weights[1] = 1.0;
		// printf( "flashTrkCA (%d)\n" , flashTrkCA_freq_1 );
	}
#endif
#if PG_NB_TRACKS >= 3
	if (flashTrkCA_freq_2 > PG_LOOP_SIZE) { // if flashTrkCA_freq_2 > PG_LOOP_SIZE -> update every frame
		flashTrkCA_weights[2] = 1.0;
		// printf( "flashTrkCA (%d)\n" , flashTrkCA_freq_2 );
	}
#endif
#if PG_NB_TRACKS >= 4
	if (flashTrkCA_freq_3 > PG_LOOP_SIZE) { // if flashTrkCA_freq_2 > PG_LOOP_SIZE -> update every frame
		flashTrkCA_weights[3] = 1.0;
		// printf( "flashTrkCA (%d)\n" , flashTrkCA_freq_2 );
	}
#endif

	if (flashTrkPart_freq_0 > PG_LOOP_SIZE) { // if flashTrkPart_freq_0 > PG_LOOP_SIZE -> update every frame
		flashTrkPart_weights[0] = 1.0;
		// printf( "flashTrkPart (%d)\n" , flashTrkPart_freq_0 );
	}
#if PG_NB_TRACKS >= 2
	if (flashTrkPart_freq_1 > PG_LOOP_SIZE) { // if flashTrkPart_freq_1 > PG_LOOP_SIZE -> update every frame
		flashTrkPart_weights[1] = 1.0;
		// printf( "flashTrkPart (%d)\n" , flashTrkPart_freq_1 );
	}
#endif
#if PG_NB_TRACKS >= 3
	if (flashTrkPart_freq_2 > PG_LOOP_SIZE) { // if flashTrkPart_freq_2 > PG_LOOP_SIZE -> update every frame
		flashTrkPart_weights[2] = 1.0;
		// printf( "flashTrkPart (%d)\n" , flashTrkPart_freq_2 );
	}
#endif
#if PG_NB_TRACKS >= 4
	if (flashTrkPart_freq_3 > PG_LOOP_SIZE) { // if flashTrkPart_freq_2 > PG_LOOP_SIZE -> update every frame
		flashTrkPart_weights[3] = 1.0;
		// printf( "flashTrkPart (%d)\n" , flashTrkPart_freq_2 );
	}
#endif

#if PG_NB_TRACKS >= 2
	if (flashTrkBG_freq_1 > PG_LOOP_SIZE) { // if flashTrkBG_freq_1 > PG_LOOP_SIZE -> update every frame
											// should only be flashed every second frame because of the way the pixels are spreaded
											// otherwise the pixels are not emitted
		if (pg_FrameNo % 2 == 0) {
			flashTrkBG_weights[1] = 1.0;
		}
		//printf( "flashTrkBG_weights 1 (%.2f)\n" , flashTrkBG_weights[1] );
	}
#endif
#if PG_NB_TRACKS >= 3
	if (flashTrkBG_freq_2 > PG_LOOP_SIZE) { // if flashTrkBG_freq_2 > PG_LOOP_SIZE -> update every frame
											// should only be flashed every second frame because of the way the pixels are spreaded
											// otherwise the pixels are not emitted
		if (pg_FrameNo % 2 == 0) {
			flashTrkBG_weights[2] = 1.0;
		}
		//printf( "flashTrkBG_weights 2 (%.2f)\n" , flashTrkBG_weights[2] );
	}
#endif
#if PG_NB_TRACKS >= 4
	if (flashTrkBG_freq_3 > PG_LOOP_SIZE) { // if flashTrkBG_freq_2 > PG_LOOP_SIZE -> update every frame
											// should only be flashed every second frame because of the way the pixels are spreaded
											// otherwise the pixels are not emitted
		if (pg_FrameNo % 2 == 0) {
			flashTrkBG_weights[3] = 1.0;
		}
		//printf( "flashTrkBG_weights 3 (%.2f)\n" , flashTrkBG_weights[3] );
	}
#endif

	if (flashCABG_freq > PG_LOOP_SIZE) { // if flashCABG_freq > PG_LOOP_SIZE -> update every frame
		flashCABG_weight = 1.0;
		// printf( "flashCABG_freq (%d)\n" , flashCABG_freq );
	}
	if (flashCAPart_freq > PG_LOOP_SIZE) { // if flashCAPart_freq > PG_LOOP_SIZE -> update every frame
		flashCAPart_weight = 1.0;
		// printf( "flashCAPart_freq (%d)\n" , flashCAPart_freq );
	}
	if (flashPartBG_freq > PG_LOOP_SIZE) { // if flashPartBG_freq > PG_LOOP_SIZE -> update every frame
		flashPartBG_weight = 1.0;
		// printf( "flashPartBG_freq (%d)\n" , flashPartBG_freq );
	}
	if (flashPartCA_freq > PG_LOOP_SIZE) { // if flashPartCA_freq > PG_LOOP_SIZE -> update every frame
		flashPartCA_weight = 1.0;
		// printf( "flashPartCA_freq (%d)\n" , flashPartCA_freq );
	}
}

///////////////////////////////////////////////////////////////////////////////////
// UDP BASED COMMANDS
///////////////////////////////////////////////////////////////////////////////////
void pg_aliasScript(char *command_symbol,
	char* string_argument_0,
	float arguments[MAX_OSC_ARGUMENTS]) {
	char newCommand[512];

	strcpy(newCommand, command_symbol);

	// renames the command according to the variables found in the scenario
	// compares message tag with known command tags
	//if (strncmp(newCommand, "pulse", 5) != 0) {
	//	printf("new command %s\n", newCommand);
	//}

	// ====================================== 
	// test UDP connection                    
	// ====================================== 
	// from Lola's porphyrograph_sound
	if (strcmp(newCommand, "testUDP") == 0) {
		sprintf(AuxString, "*** OK ***"); pg_writeMessageOnScreen(AuxString);
		sprintf(AuxString, "/message_can TEST_UDP"); pg_send_message_udp((char *)"s", AuxString, (char *)"udp_QT_send");
	}
	// from Yukao's QT
	else if (strcmp(newCommand, "QT_connected") == 0) {
		sprintf(AuxString, "*** QT ***"); pg_writeMessageOnScreen(AuxString);
	}
	// from Yukao's PD
	else if (strcmp(newCommand, "PD_connected") == 0) {
		sprintf(AuxString, "*** PD ***"); pg_writeMessageOnScreen(AuxString);
	}
	else if (strcmp(newCommand, "return_message") == 0) {
		pg_writeMessageOnScreen(string_argument_0);

		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
		// +++++++++++++++++ LAUNCH TIMER ++++++++++++++++++++++++++ 
		// +++++++++++++++++   keystroke (t)  ++++++++++++++++++++++ 
		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	}
	else if (strcmp(newCommand, "launch") == 0) {
		launch();


		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
		// +++++++++++++++++ PD CONNECTION CONFIRMATION ++++++++++++ 
		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	}
	else if (strcmp(newCommand, "reset_sound") == 0) {
		pg_send_message_udp((char *)"i", (char *)"/reset_sound 1", (char *)"udp_PD_send");

		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
		// +++++++++++++++++ PD CONNECTION CONFIRMATION ++++++++++++ 
		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	}
	else if (strcmp(newCommand, "PD_connected") == 0) {
		pg_send_message_udp((char *)"s", (char *)"/PD_connected 1", (char *)"udp_QT_send");

		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
		// +++++++++++++++++ CONNECTS PD ++++++++++++++++++++++++++ 
		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	}
	else if (strcmp(newCommand, "connect_PD") == 0) {
		pg_send_message_udp((char *)"i", (char *)"/connect 1", (char *)"udp_PD_send");

		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
		// +++++++++++++++++ QUITS +++++++++++++++++++++++++++++++++ 
		// +++++++++++++++++   keystroke (ESC)  ++++++++++++++++++++ 
		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	}
	else if (strcmp(newCommand, "quit") == 0) {
		quit();

		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
		// +++++++++++++++++ PARTICLE INITIALIZATION +++++++++++++++
		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	}
	else if (strcmp(newCommand, "reset_camera") == 0) {
		reset_camera = true;
		// pg_send_message_udp((char *)"s", (char *)"/message_can camera_background_capture", (char *)"udp_QT_send");

		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
		// +++++++++++++++++ MUTE/UNMUTE LEFT SCREEN +++++++++++++++ 
		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	}
	else if (strcmp(newCommand, "mute_second_screen") == 0) {
		mute_second_screen = !mute_second_screen;
		sprintf(AuxString, "/mute_second_screen_can %d", int(mute_second_screen));
		pg_send_message_udp((char *)"i", AuxString, (char *)"udp_QT_send");

		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
		// +++++++++++++++++ VIDEO FLASH ++++++++++++++++++++++++ 
		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	}
	else if (strcmp(newCommand, "flashVideo") == 0) {
		is_flashCameraTrk = true;
		flashCameraTrk_weight = 0.0f;
		// printf("start flash\n");

		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
		// +++++++++++++++++ MUSIC CONTROL +++++++++++++++++++++++++ 
		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	}
	//else if (strcmp(newCommand, "pulse_average") == 0) {
	//	pulse_average_prec = pulse_average;
	//	pulse_average = arguments[0] * sound_volume + sound_min;
	//	sprintf(AuxString, "/pulse_enveloppe %.5f", pulse[2]);
	//	pg_send_message_udp((char *)"f", AuxString, (char *)"udp_QT_send");

	//}
	else if (strcmp(newCommand, "pulse_spectrum") == 0) {
		pulse[0] = arguments[0] * sound_volume + sound_min;
		pulse[1] = arguments[1] * sound_volume + sound_min;
		pulse[2] = arguments[2] * sound_volume + sound_min;
		pulse_attack = arguments[3] * sound_volume + sound_min;
		sprintf(AuxString, "/pulse_low %.5f", pulse[0]);
		pg_send_message_udp((char *)"f", AuxString, (char *)"udp_QT_send");
		sprintf(AuxString, "/pulse_medium %.5f", pulse[1]);
		pg_send_message_udp((char *)"f", AuxString, (char *)"udp_QT_send");
		sprintf(AuxString, "/pulse_high %.5f", pulse[2]);
		pg_send_message_udp((char *)"f", AuxString, (char *)"udp_QT_send");

		pulse_average_prec = pulse_average;
		pulse_average = (pulse[0] + pulse[1] + pulse[2]) / 3.f;
		sprintf(AuxString, "/pulse_enveloppe %.5f", pulse_attack);
		pg_send_message_udp((char *)"f", AuxString, (char *)"udp_QT_send");

		// PEN PULSED COLOR
		float lowPalette = floor(pen_color * nb_pen_palette_colors);
		int indLowPalette
			= int(lowPalette) % nb_pen_palette_colors;
		int indUpperPalette
			= (indLowPalette + 1) % nb_pen_palette_colors;
		float percentage = pen_color * nb_pen_palette_colors - lowPalette;
		float base_color[9];
		for (int ind = 0; ind < 9; ind++) {
			base_color[ind] = (1.f - percentage) * pen_palette_colors_values[indLowPalette][ind]
				+ percentage * pen_palette_colors_values[indUpperPalette][ind];
			base_color[ind] = std::min(1.f, base_color[ind]);
		}
		for (int indChannel = 0; indChannel < 3; indChannel++) {
			pulsed_pen_color[indChannel] = pen_grey;
			for (int indPulse = 0; indPulse < 3; indPulse++) {
				pulsed_pen_color[indChannel]
					+= base_color[indPulse * 3 + indChannel] * pulse[indPulse];
			}
			pulsed_pen_color[indChannel] = std::min(1.f, pulsed_pen_color[indChannel]);
		}
		pulsed_pen_color[3] = 1.f;
		float value = (pulsed_pen_color[0] < pulsed_pen_color[1]) ? pulsed_pen_color[1] : pulsed_pen_color[0];
		value = (value < pulsed_pen_color[2]) ? pulsed_pen_color[2] : value;
		// printf( "pulsed_pen_color: %f %f %f\n" , pulsed_pen_color[0] , pulsed_pen_color[1] , pulsed_pen_color[2] );
#if !defined (CAVERNEPLATON) && !defined(ULM)
		if (invertAllLayers) {
			pulsed_pen_color[0] = value - pulsed_pen_color[0];
			pulsed_pen_color[1] = value - pulsed_pen_color[1];
			pulsed_pen_color[2] = value - pulsed_pen_color[2];
		}
#endif
		sprintf(AuxString, "/pen_colorPalette_can %.5f %.5f %.5f %.5f %.5f %.5f %.5f %.5f %.5f",
			base_color[0], base_color[1], base_color[2], base_color[3], base_color[4], base_color[5], base_color[6], base_color[7], base_color[8]);
		pg_send_message_udp((char *)"f f f f f f f f f", AuxString, (char *)"udp_QT_send");
		sprintf(AuxString, "/pulsed_pen_color_can %.5f %.5f %.5f",
			pulsed_pen_color[0], pulsed_pen_color[1], pulsed_pen_color[2]);
		pg_send_message_udp((char *)"f f f", AuxString, (char *)"udp_QT_send");

		// REPOP PULSED COLOR
		float repop_color_palette_pulsed = repop_color + pulse_average * repop_hue_pulse;
		lowPalette = floor(repop_color_palette_pulsed * nb_pen_palette_colors);
		indLowPalette
			= int(lowPalette) % nb_pen_palette_colors;
		indUpperPalette
			= (indLowPalette + 1) % nb_pen_palette_colors;
		percentage = repop_color_palette_pulsed * nb_pen_palette_colors - lowPalette;
		for (int ind = 0; ind < 9; ind++) {
			base_color[ind] = (1.f - percentage) * pen_palette_colors_values[indLowPalette][ind]
				+ percentage * pen_palette_colors_values[indUpperPalette][ind];
			base_color[ind] = std::min(1.f, base_color[ind]);
		}
		for (int indChannel = 0; indChannel < 3; indChannel++) {
			pulsed_repop_color[indChannel] = repop_grey;
			for (int indPulse = 0; indPulse < 3; indPulse++) {
				pulsed_repop_color[indChannel]
					+= base_color[indPulse * 3 + indChannel] * pulse[indPulse];
			}
			pulsed_repop_color[indChannel] = std::min(1.f, pulsed_repop_color[indChannel]);
		}
		value = (pulsed_repop_color[0] < pulsed_repop_color[1]) ? pulsed_repop_color[1] : pulsed_repop_color[0];
		value = (value < pulsed_repop_color[2]) ? pulsed_repop_color[2] : value;
		// printf( "pulsed_repop_color: %f %f %f\n" , pulsed_repop_color[0] , pulsed_repop_color[1] , pulsed_repop_color[2] );
#if !defined (CAVERNEPLATON) && !defined(ULM)
		if (invertAllLayers) {
			pulsed_repop_color[0] = value - pulsed_repop_color[0];
			pulsed_repop_color[1] = value - pulsed_repop_color[1];
			pulsed_repop_color[2] = value - pulsed_repop_color[2];
		}
#endif
		sprintf(AuxString, "/repop_colorPalette_can %.5f %.5f %.5f %.5f %.5f %.5f %.5f %.5f %.5f",
			base_color[0], base_color[1], base_color[2], base_color[3], base_color[4], base_color[5], base_color[6], base_color[7], base_color[8]);
		pg_send_message_udp((char *)"f f f f f f f f f", AuxString, (char *)"udp_QT_send");
		sprintf(AuxString, "/pulsed_repop_color_can %.5f %.5f %.5f",
			pulsed_repop_color[0], pulsed_repop_color[1], pulsed_repop_color[2]);
		pg_send_message_udp((char *)"f f f", AuxString, (char *)"udp_QT_send");

	}
	else if (strcmp(newCommand, "autom_env_min_max") == 0) {
		sprintf(AuxString, "/autom_env_min_can %.1f", arguments[0]);
		pg_send_message_udp((char *)"f", AuxString, (char *)"udp_QT_send");
		sprintf(AuxString, "/autom_env_max_can %.1f", arguments[1]);
		pg_send_message_udp((char *)"f", AuxString, (char *)"udp_QT_send");
		//}

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
		// +++++++++++++++++ BEAT: MUSCI TEMPO SYNC ++++++++++++++++ 
		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
		// flashPixel (f)                              
		// BackFlash (L)                          
		// ====================================== 
	}
	else if (strcmp(newCommand, "beat") == 0) {
		int argt0 = (int)round(arguments[0]);
		if (argt0 >= 0) {
			pg_BeatNo = argt0;
		}
		else {
			pg_BeatNo++;
		}

		sprintf(AuxString, "/loop_beat_can %d", pg_BeatNo % PG_LOOP_SIZE); pg_send_message_udp((char *)"i", AuxString, (char *)"udp_QT_send");

		// loop start 
		if ((pg_BeatNo % PG_LOOP_SIZE) == 0) {

			for (int ind = 1; ind < PG_NB_PATHS + 1; ind++) {
				// source track recording is on -> source track start 
				if (synchr_start_recording_path[ind]) {
					pg_path_recording_onOff(ind);
					// sets to 0 the recording message since recording has started 
					synchr_start_recording_path[ind] = false;
				}


				// track reading is on -> track read start 
				if (synchr_start_path_replay_trackNo[ind] >= 0) {
					pg_path_replay_trackNo_onOff(ind, synchr_start_path_replay_trackNo[ind]);
					synchr_start_path_replay_trackNo[ind] = -1;
				}
			}
		}

		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
		// video flash
		if (is_flashCameraTrk
			&& ((pg_BeatNo % PG_LOOP_SIZE) == flashCameraTrkBeat || flashCameraTrkBeat == 9)) {
			is_flashCameraTrk = false;
			flashCameraTrk_weight = 1.0f;
			//printf("flashCameraTrk_weight %.3f decay %.2f\n", 
			//	flashCameraTrk_weight , flashCameraTrk_decay );
		}

		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
		// +++++++++++++++++ FLASHES +++++++++++++++++++++++++++++++ 
		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
		// ======================================== 
		// flash triggering according to beat & frequency, weight, and on/off values 
		// ======================================== 
		// flashes are synchronized on beats according to their frequency
		if (flashPixel_freq > 0
			&& flashPixel_freq <= PG_LOOP_SIZE // if flashPixel_freq > PG_LOOP_SIZE -> update every frame
			&& (pg_BeatNo % (PG_LOOP_SIZE / flashPixel_freq)) == 0) {
			flashPixel = flashPixel_duration;
		}

#if PG_NB_TRACKS >= 2
		if (flashTrkBG_freq_1 > 0
			&& flashTrkBG_freq_1 <= PG_LOOP_SIZE // if flashTrkBG_freq_1 > PG_LOOP_SIZE -> update every frame
			&& ((int)round(arguments[0]) % (PG_LOOP_SIZE / flashTrkBG_freq_1)) == 0) {
			flashTrkBG_weights[1] = 1.0;
			// printf( "flashTrkBG (%d)\n" , flashTrkBG );
		}
#endif
#if PG_NB_TRACKS >= 3
		if (flashTrkBG_freq_2 > 0
			&& flashTrkBG_freq_2 <= PG_LOOP_SIZE // if flashTrkBG_freq_2 > PG_LOOP_SIZE -> update every frame
			&& ((int)round(arguments[0]) % (PG_LOOP_SIZE / flashTrkBG_freq_2)) == 0) {
			flashTrkBG_weights[2] = 1.0;
			// printf( "flashTrkBG (%d)\n" , flashTrkBG );
		}
#endif
#if PG_NB_TRACKS >= 4
		if (flashTrkBG_freq_3 > 0
			&& flashTrkBG_freq_3 <= PG_LOOP_SIZE // if flashTrkBG_freq_3 > PG_LOOP_SIZE -> update every frame
			&& ((int)round(arguments[0]) % (PG_LOOP_SIZE / flashTrkBG_freq_3)) == 0) {
			flashTrkBG_weights[3] = 1.0;
			// printf( "flashTrkBG (%d)\n" , flashTrkBG );
		}
#endif

		if (flashTrkCA_freq_0 > 0
			&& flashTrkCA_freq_0 <= PG_LOOP_SIZE // if flashTrkCA_freq_0 > PG_LOOP_SIZE -> update every frame
			&& (pg_BeatNo % (PG_LOOP_SIZE / flashTrkCA_freq_0)) == 0) {
			flashTrkCA_weights[0] = 1.0;
			// printf( "flashTrkCA 0 (%.2f)\n" , flashTrkCA_weights[0] );
		}
#if PG_NB_TRACKS >= 2
		if (flashTrkCA_freq_1 > 0
			&& flashTrkCA_freq_1 <= PG_LOOP_SIZE // if flashTrkCA_freq_1 > PG_LOOP_SIZE -> update every frame
			&& ((int)round(arguments[0]) % (PG_LOOP_SIZE / flashTrkCA_freq_1)) == 0) {
			flashTrkCA_weights[1] = 1.0;
			// printf( "flashTrkCA 1 (%.2f)\n" , flashTrkCA_weights[1] );
		}
#endif
#if PG_NB_TRACKS >= 3
		if (flashTrkCA_freq_2 > 0
			&& flashTrkCA_freq_2 <= PG_LOOP_SIZE // if flashTrkCA_freq_2 > PG_LOOP_SIZE -> update every frame
			&& ((int)round(arguments[2]) % (PG_LOOP_SIZE / flashTrkCA_freq_2)) == 0) {
			flashTrkCA_weights[2] = 1.0;
			// printf( "flashTrkCA 2 (%.2f)\n" , flashTrkCA_weights[2] );
		}
#endif
#if PG_NB_TRACKS >= 4
		if (flashTrkCA_freq_3 > 0
			&& flashTrkCA_freq_3 <= PG_LOOP_SIZE // if flashTrkCA_freq_3 > PG_LOOP_SIZE -> update every frame
			&& ((int)round(arguments[3]) % (PG_LOOP_SIZE / flashTrkCA_freq_3)) == 0) {
			flashTrkCA_weights[3] = 1.0;
			// printf( "flashTrkCA 3 (%.2f)\n" , flashTrkCA_weights[3] );
		}
#endif

		if (flashTrkPart_freq_0 > 0
			&& flashTrkPart_freq_0 <= PG_LOOP_SIZE // if flashTrkPart_freq_0 > PG_LOOP_SIZE -> update every frame
			&& (pg_BeatNo % (PG_LOOP_SIZE / flashTrkPart_freq_0)) == 0) {
			flashTrkPart_weights[0] = 1.0;
			// printf( "flashTrkPart 0 (%.2f)\n" , flashTrkPart_weights[0] );
		}
#if PG_NB_TRACKS >= 2
		if (flashTrkPart_freq_1 > 0
			&& flashTrkPart_freq_1 <= PG_LOOP_SIZE // if flashTrkPart_freq_1 > PG_LOOP_SIZE -> update every frame
			&& ((int)round(arguments[0]) % (PG_LOOP_SIZE / flashTrkPart_freq_1)) == 0) {
			flashTrkPart_weights[1] = 1.0;
			// printf( "flashTrkPart 1 (%.2f)\n" , flashTrkPart_weights[1] );
		}
#endif
#if PG_NB_TRACKS >= 3
		if (flashTrkPart_freq_2 > 0
			&& flashTrkPart_freq_2 <= PG_LOOP_SIZE // if flashTrkPart_freq_2 > PG_LOOP_SIZE -> update every frame
			&& ((int)round(arguments[2]) % (PG_LOOP_SIZE / flashTrkPart_freq_2)) == 0) {
			flashTrkPart_weights[2] = 1.0;
			// printf( "flashTrkPart 2 (%.2f)\n" , flashTrkPart_weights[2] );
		}
#endif
#if PG_NB_TRACKS >= 4
		if (flashTrkPart_freq_3 > 0
			&& flashTrkPart_freq_3 <= PG_LOOP_SIZE // if flashTrkPart_freq_3 > PG_LOOP_SIZE -> update every frame
			&& ((int)round(arguments[3]) % (PG_LOOP_SIZE / flashTrkPart_freq_3)) == 0) {
			flashTrkPart_weights[3] = 1.0;
			// printf( "flashTrkPart 3 (%.2f)\n" , flashTrkPart_weights[3] );
		}
#endif

		if (flashCABG_freq > 0
			&& flashCABG_freq <= PG_LOOP_SIZE // if flashCABG_freq > PG_LOOP_SIZE -> update every frame
			&& (pg_BeatNo % (PG_LOOP_SIZE / flashCABG_freq)) == 0) {
			flashCABG_weight = 1.0;
			// printf( "flashCABG_freq (%d)\n" , flashCABG_freq );
		}
		if (flashCAPart_freq > 0
			&& flashCAPart_freq <= PG_LOOP_SIZE // if flashCAPart_freq > PG_LOOP_SIZE -> update every frame
			&& (pg_BeatNo % (PG_LOOP_SIZE / flashCAPart_freq)) == 0) {
			flashCAPart_weight = 1.0;
			// printf( "flashCAPart_freq (%d)\n" , flashCAPart_freq );
		}
		if (flashPartBG_freq > 0
			&& flashPartBG_freq <= PG_LOOP_SIZE // if flashPartBG_freq > PG_LOOP_SIZE -> update every frame
			&& (pg_BeatNo % (PG_LOOP_SIZE / flashPartBG_freq)) == 0) {
			flashPartBG_weight = 1.0;
			// printf( "flashPartBG_freq (%d)\n" , flashPartBG_freq );
		}
		if (flashPartCA_freq > 0
			&& flashPartCA_freq <= PG_LOOP_SIZE // if flashPartCA_freq > PG_LOOP_SIZE -> update every frame
			&& (pg_BeatNo % (PG_LOOP_SIZE / flashPartCA_freq)) == 0) {
			flashPartCA_weight = 1.0;
			// printf( "flashPartCA_freq (%d)\n" , flashPartCA_freq );
		}


		// ======================================== 
		// flash on/off values
		// ======================================== 
	}
	else if (strcmp(newCommand, "flashTrkCA_onOff") == 0) {
		switch (currentDrawingTrack) {
		case 0:
			if (flashTrkCA_freq_0 > 0) {
				flashTrkCA_freq_0_saved = flashTrkCA_freq_0;
				flashTrkCA_freq_0 = 0;
			}
			else {
				flashTrkCA_freq_0 = flashTrkCA_freq_0_saved;
			}
			break;
#if PG_NB_TRACKS >= 2
		case 1:
			if (flashTrkCA_freq_1 > 0) {
				flashTrkCA_freq_1_saved = flashTrkCA_freq_1;
				flashTrkCA_freq_1 = 0;
			}
			else {
				flashTrkCA_freq_1 = flashTrkCA_freq_1_saved;
			}
			break;
#endif
#if PG_NB_TRACKS >= 3
		case 2:
			if (flashTrkCA_freq_2 > 0) {
				flashTrkCA_freq_2_saved = flashTrkCA_freq_2;
				flashTrkCA_freq_2 = 0;
			}
			else {
				flashTrkCA_freq_2 = flashTrkCA_freq_2_saved;
			}
			break;
#endif
#if PG_NB_TRACKS >= 4
		case 3:
			if (flashTrkCA_freq_3 > 0) {
				flashTrkCA_freq_3_saved = flashTrkCA_freq_3;
				flashTrkCA_freq_3 = 0;
			}
			else {
				flashTrkCA_freq_3 = flashTrkCA_freq_3_saved;
			}
			break;
#endif
		}
	}

	else if (strcmp(newCommand, "flashTrkPart_onOff") == 0) {
		switch (currentDrawingTrack) {
		case 0:
			if (flashTrkPart_freq_0 > 0) {
				flashTrkPart_freq_0_saved = flashTrkPart_freq_0;
				flashTrkPart_freq_0 = 0;
			}
			else {
				flashTrkPart_freq_0 = flashTrkPart_freq_0_saved;
			}
			break;
#if PG_NB_TRACKS >= 2
		case 1:
			if (flashTrkPart_freq_1 > 0) {
				flashTrkPart_freq_1_saved = flashTrkPart_freq_1;
				flashTrkPart_freq_1 = 0;
			}
			else {
				flashTrkPart_freq_1 = flashTrkPart_freq_1_saved;
			}
			break;
#endif
#if PG_NB_TRACKS >= 3
		case 2:
			if (flashTrkPart_freq_2 > 0) {
				flashTrkPart_freq_2_saved = flashTrkPart_freq_2;
				flashTrkPart_freq_2 = 0;
			}
			else {
				flashTrkPart_freq_2 = flashTrkPart_freq_2_saved;
			}
			break;
#endif
#if PG_NB_TRACKS >= 4
		case 3:
			if (flashTrkPart_freq_3 > 0) {
				flashTrkPart_freq_3_saved = flashTrkPart_freq_3;
				flashTrkPart_freq_3 = 0;
			}
			else {
				flashTrkPart_freq_3 = flashTrkPart_freq_3_saved;
			}
			break;
#endif
		}

		// ====================================== 
			// keystroke s: Snapshot                  
			// ====================================== 
	}
	else if (strcmp(newCommand, "snapshot") == 0) {
		pg_snapshot((char *)"jpg");

		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
		// +++++++++++++++++ TRACK SHIFT +++++++++++++++++++++++++++ 
		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	}
	else if (strcmp(newCommand, "currentDrawingTrack_plus") == 0) {
		currentDrawingTrack = (currentDrawingTrack + 1) % PG_NB_TRACKS;

	}
	else if (strcmp(newCommand, "currentDrawingTrack_minus") == 0) {
		currentDrawingTrack = (currentDrawingTrack - 1 + PG_NB_TRACKS) % PG_NB_TRACKS;
	}
	else if (strcmp(newCommand, "currentVideoTrack_plus") == 0) {
		currentVideoTrack = (currentVideoTrack + 1) % PG_NB_TRACKS;

	}
	else if (strcmp(newCommand, "currentPhotoTrack_plus") == 0) {
		currentPhotoTrack = (currentPhotoTrack + 1) % PG_NB_TRACKS;

		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
		// +++++++++++++++++ TRACK COPY +++++++++++++++++++++++++++ 
		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	}
	else if (strcmp(newCommand, "copyTrack_above") == 0) {
		copyToNextTrack = +1;
		//sprintf(AuxString, "/message_can copy_track_%d_to_%d",
		//	currentDrawingTrack, (currentDrawingTrack + 1) % PG_NB_TRACKS);
		//pg_send_message_udp((char *)"s", (char *)AuxString, (char *)"udp_QT_send");
	}
	else if (strcmp(newCommand, "copyTrack_under") == 0) {
		copyToNextTrack = -1;
		//sprintf(AuxString, "/message_can copy_track_%d_to_%d", 
		//	currentDrawingTrack , (currentDrawingTrack - 1 + PG_NB_TRACKS) % PG_NB_TRACKS); 
		//pg_send_message_udp((char *)"s", (char *)AuxString, (char *)"udp_QT_send");


		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
		// +++++++++++++++++ CAMERA IMAGE CUMUL MODE +++++++++++++++ 
		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	}
	else if (strcmp(newCommand, "cameraCumul_plus") == 0) {
		cameraCumul = (cameraCumul + 1) % PG_NB_VIDEO_CUMUL_MODES;
		BrokenInterpolationVar[_cameraCumul] = true;

		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
		// +++++++++++++++++ PIXEL MODE +++++++++++++++++++ 
		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	}
	else if (strcmp(newCommand, "pixel_mode_plus") == 0) {
		pixel_mode = (pixel_mode + 1) % PG_NB_PIXEL_MODES;
		BrokenInterpolationVar[_pixel_mode] = true;

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// +++++++++++++++++ CA TYPE AND SUBTYPE +++++++++++++++++++ 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	}
	else if (strcmp(newCommand, "CAType_plus") == 0) {
		CAType = (CAType + 1) % PG_NB_CA_TYPES;
		BrokenInterpolationVar[_CAType] = true;
	}
	else if (strcmp(newCommand, "CASubType_plus") == 0) {
		CASubType = (CASubType + 1) % PG_NB_CA_SUBTYPES;
		BrokenInterpolationVar[_CASubType] = true;
	}
	else if (strcmp(newCommand, "CAonOff") == 0) {
		if (CASubType != 0) {
			CASubTypeMem = CASubType;
			CASubType = 0;
		}
		else {
			CASubType = CASubTypeMem;
		}
		BrokenInterpolationVar[_CAType] = true;

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// +++++++++++++++++ CAMERA IMAGE CUMUL MODE +++++++++++++++ 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	}
	else if (strcmp(newCommand, "partExit_mode_plus") == 0) {
		partExit_mode = (partExit_mode + 1) % PG_NB_PARTEXIT_MODES;
		BrokenInterpolationVar[_partExit_mode] = true;
	}
	else if (strcmp(newCommand, "partStroke_mode_plus") == 0) {
		partStroke_mode = (partStroke_mode + 1) % PG_NB_PARTSTROKE_MODES;
		BrokenInterpolationVar[_partStroke_mode] = true;
	}
	else if (strcmp(newCommand, "partColor_mode_plus") == 0) {
		partColor_mode = (partColor_mode + 1) % PG_NB_PARTCOLOR_MODES;
		BrokenInterpolationVar[_partColor_mode] = true;
	}

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// +++++++++++++++++ B/W SWITCH ++++++++++++++++++++++++++++ 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// ====================================== 
	else if (strcmp(newCommand, "pen_BW") == 0) {
		if (pen_grey > 0) {
			pen_grey = 0.0f;
		}
		else {
			pen_grey = 1.0f;
		}
		BrokenInterpolationVar[_pen_grey] = true;
	}
	else if (strcmp(newCommand, "repop_BW") == 0) {
		if (repop_grey > 0) {
			repop_grey = 0.0f;
		}
		else {
			repop_grey = 1.0f;
		}
		BrokenInterpolationVar[_repop_grey] = true;
		// printf("repop color %.1f\n", repop_grey);
	}

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// +++++++++++++++++ PRESSURE SWITCH +++++++++++++++++++++++ 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// ====================================== 
	else if (strcmp(newCommand, "pressure_onOff") == 0) {
#define PG_PRESSURE_STANDARD 10.0f
		if (pen_radius_pressure_coef > 0) {
			pen_radius_pressure_coef = 0.0f;
		}
		else {
			pen_radius_pressure_coef = PG_PRESSURE_STANDARD;
		}
		BrokenInterpolationVar[_pen_radius_pressure_coef] = true;
	}

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// +++++++++++++++++ TRACK DECAY SWITCH ++++++++++++++++++++ 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// ====================================== 
	else if (strcmp(newCommand, "decay_onOff") == 0) {
#define PG_DECAY_STANDARD 0.0003f
		switch (currentDrawingTrack) {
		case 0:
			if (trkDecay_0 == 0) {
				trkDecay_0 = PG_DECAY_STANDARD;
			}
			else {
				trkDecay_0 = 0.0f;
			}
			BrokenInterpolationVar[_trkDecay_0] = true;
			break;
#if PG_NB_TRACKS >= 2
		case 1:
			if (trkDecay_1 == 0) {
				trkDecay_1 = PG_DECAY_STANDARD;
			}
			else {
				trkDecay_1 = 0.0f;
			}
			BrokenInterpolationVar[_trkDecay_1] = true;
			break;
#endif
#if PG_NB_TRACKS >= 3
		case 2:
			if (trkDecay_2 == 0) {
				trkDecay_2 = PG_DECAY_STANDARD;
			}
			else {
				trkDecay_2 = 0.0f;
			}
			BrokenInterpolationVar[_trkDecay_2] = true;
			break;
#endif
#if PG_NB_TRACKS >= 4
		case 3:
			if (trkDecay_3 == 0) {
				trkDecay_3 = PG_DECAY_STANDARD;
			}
			else {
				trkDecay_3 = 0.0f;
			}
			BrokenInterpolationVar[_trkDecay_3] = true;
			break;
#endif
		}
	}

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// +++++++++++++++++ DIAPORAMA +++++++++++++++++++++++++++++ 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// ====================================== 
	else if (strcmp(newCommand, "diaporama_plus") == 0) {
		string fileName = "";
		for (int indPhoto = 0; indPhoto < PG_PHOTO_NB_TEXTURES; indPhoto++) {
			// incay and decay are 0 if is_capture_diaporama
			float incay = (is_capture_diaporama ? 0 : photo_diaporama_fade);
			float decay = (is_capture_diaporama ? 0 : photo_diaporama_fade);
			// blended photo is terminated to switch to the next one
			if (pg_Photo_buffer_data[indPhoto].blendStart > 0.0f) {
				// change order (was decreasing before)
				if (ascendingDiaporama == false) {
					ascendingDiaporama = true;
					IndPlayedDiaporamaFile += 3;
					if ((nextFile(&IndPlayedDiaporamaFile, &fileName, ascendingDiaporama))) {
						// printf("load diaporama initial files name %s\n", fileName->c_str());
						if (pg_load_photo_layer(&fileName, (indPhoto + 1) % PG_PHOTO_NB_TEXTURES, false)) {
						}
					}
				}
				// stops blending
				printf("load diaporama_plus stops blending layer #%d\n", indPhoto);
				pg_Photo_buffer_data[indPhoto].blendStart
					= CurrentClockTime - (incay + photo_diaporama_plateau + decay);
				return;
			}
		}
	}

	else if (strcmp(newCommand, "diaporama_minus") == 0) {
		string fileName = "";
		for (int indPhoto = 0; indPhoto < PG_PHOTO_NB_TEXTURES; indPhoto++) {
			// incay and decay are 0 if is_capture_diaporama
			float incay = (is_capture_diaporama ? 0 : photo_diaporama_fade);
			float decay = (is_capture_diaporama ? 0 : photo_diaporama_fade);
			// blended photo is terminated to switch to the next one
			if (pg_Photo_buffer_data[indPhoto].blendStart > 0.0f) {
				// change order (was decreasing before)
				if (ascendingDiaporama == true) {
					ascendingDiaporama = false;
					IndPlayedDiaporamaFile -= 3;
					if ((nextFile(&IndPlayedDiaporamaFile, &fileName, ascendingDiaporama))) {
						// printf("load diaporama initial files name %s\n", fileName->c_str());
						if (pg_load_photo_layer(&fileName, (indPhoto + 1) % PG_PHOTO_NB_TEXTURES, false)) {
						}
					}
				}
				// stops blending
				printf("load diaporama_minus stops blending layer #%d\n", indPhoto);
				pg_Photo_buffer_data[indPhoto].blendStart
					= CurrentClockTime - (incay + photo_diaporama_plateau + decay);
				return;
			}
		}
	}

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// +++++++++++++++++ TRACK NO ++++++++++++++++++++++++++++++ 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// ====================================== 
	else if (strcmp(newCommand, "soundtrack_plus") == 0) {
		if (nb_soundtracks > 0) {
			currentlyPlaying_trackNo = (currentlyPlaying_trackNo + 1) % nb_soundtracks;
			sprintf(AuxString, "/soundtrack_fileName %s",
				("Data/" + project_name + "-data/soundtracks/" + trackFileName[currentlyPlaying_trackNo]).c_str());
			pg_send_message_udp((char *)"s", AuxString, (char *)"udp_PD_send");
			sprintf(AuxString, "/soundtrack_shortName_can %s", trackShortName[currentlyPlaying_trackNo].c_str());
			pg_send_message_udp((char *)"s", AuxString, (char *)"udp_QT_send");
			BrokenInterpolationVar[_soundtrack_onOff] = true;
			soundtrack_onOff = true;
		}
	}

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// +++++++++++++++++ PALETTE NO ++++++++++++++++++++++++++++ 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// ====================================== 
	else if (strcmp(newCommand, "pen_colorPreset_minus") == 0) {
		if (nb_pen_colorPresets > 0) {
			current_pen_colorPreset = (current_pen_colorPreset - 1 + nb_pen_colorPresets) % nb_pen_colorPresets;
			// printf( "/new palette %d\n", current_pen_colorPreset);
			BrokenInterpolationVar[_pen_color] = true;
			pen_color = pen_colorPreset_values[current_pen_colorPreset];
			// sprintf(AuxString, "/pen_color_can %.3f", pen_colorPresets_values[current_pen_colorPreset]);
			// pg_send_message_udp((char *)"s", AuxString, (char *)"udp_QT_send");
			// sprintf(AuxString, "/message_can palette_%s", pen_colorPresets_names[current_pen_colorPreset].c_str()); pg_send_message_udp((char *)"s", (char *)AuxString, (char *)"udp_QT_send");
			sprintf(AuxString, "/pen_colorPreset_can %d", current_pen_colorPreset); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
		}
	}

	else if (strcmp(newCommand, "pen_colorPreset_plus") == 0) {
		if (nb_pen_colorPresets > 0) {
			current_pen_colorPreset = (current_pen_colorPreset + 1) % nb_pen_colorPresets;
			BrokenInterpolationVar[_pen_color] = true;
			pen_color = pen_colorPreset_values[current_pen_colorPreset];
			// sprintf(AuxString, "/pen_color_can %.3f", pen_colorPresets_values[current_pen_colorPreset]);
			// pg_send_message_udp((char *)"s", AuxString, (char *)"udp_QT_send");
			// sprintf(AuxString, "/message_can palette_%s", pen_colorPresets_names[current_pen_colorPreset].c_str()); pg_send_message_udp((char *)"s", (char *)AuxString, (char *)"udp_QT_send");
			sprintf(AuxString, "/pen_colorPreset_can %d", current_pen_colorPreset); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
		}
	}

	else if (strcmp(newCommand, "pen_colorPreset") == 0) {
		if (nb_pen_colorPresets > 0) {
			current_pen_colorPreset = int(arguments[0]) % nb_pen_colorPresets;
			BrokenInterpolationVar[_pen_color] = true;
			pen_color = pen_colorPreset_values[current_pen_colorPreset];
			// sprintf(AuxString, "/pen_color_can %.3f", pen_colorPresets_values[current_pen_colorPreset]);
			// pg_send_message_udp((char *)"s", AuxString, (char *)"udp_QT_send");
			// sprintf(AuxString, "/message_can palette_%s", pen_colorPresets_names[current_pen_colorPreset].c_str()); pg_send_message_udp((char *)"s", (char *)AuxString, (char *)"udp_QT_send");
			// printf("************* palette %d\n", current_pen_colorPreset);
			sprintf(AuxString, "/pen_colorPreset_can %d", current_pen_colorPreset); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
		}
	}

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++  arguments[0]
	// +++++++++++++++++ MOVIE NO ++++++++++++++++++++++++++++++ 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// ====================================== 
	else if (strcmp(newCommand, "movie_plus") == 0) {
		if (nb_movies > 0) {
			playing_movieNo = (playing_movieNo + 1) % nb_movies;
			sprintf(AuxString, "/movie_shortName_can %s", movieShortName[playing_movieNo].c_str());
			pg_send_message_udp((char *)"s", AuxString, (char *)"udp_QT_send");
			BrokenInterpolationVar[_playing_movieNo] = true;

			if (playing_movieNo >= 0 && playing_movieNo < nb_movies
				&& playing_movieNo != currentlyPlaying_movieNo) {
				currentlyPlaying_movieNo = playing_movieNo;

				// texture ID initialization (should not be inside a thread)
				if (pg_movie_texture_texID == NULL_ID) {
					glGenTextures(1, &pg_movie_texture_texID);
				}

				is_movieLoading = true;
				printf("Loading %s\n", movieFileName[currentlyPlaying_movieNo].c_str());
				// sprintf(AuxString, "/message_can %s", movieFileName[currentlyPlaying_movieNo].c_str()); pg_send_message_udp((char *)"s", (char *)AuxString, (char *)"udp_QT_send");
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
		}
	}

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// +++++++++++++++++ SET-UP ++++++++++++++++++++++++++++++++ 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// ====================================== 
	else if (strcmp(newCommand, "resend_all") == 0) {
		resend_all_variables = true;
	}
	else if (strcmp(newCommand, "setup_plus") == 0) {
		setup_plus();
	}
	else if (strcmp(newCommand, "setup_minus") == 0) {
		setup_minus();
	}
	else if (strcmp(newCommand, "setup_plus_keep_total_dur") == 0) {
		float elapsedTime = (float)(CurrentClockTime - InitialScenarioTime) - Scenario[pg_CurrentScene].scene_initial_time;
		float deltaTime = Scenario[1 + pg_CurrentScene].scene_initial_time - (float)(CurrentClockTime - InitialScenarioTime);
		// only accepted if the current scene has been on for a while
		if (elapsedTime > 15
			// and if the scene has not begun since a short while
			&& deltaTime > 0
			// and if the scene is not too far from being finished
			&& deltaTime < 60) {
			int new_scene = ((1 + pg_CurrentScene) % nb_scenes);

			restoreInitialTimesAndDurations();

			// we place the beginning of the current scene at this time
			Scenario[new_scene].scene_initial_time -= deltaTime;
			Scenario[new_scene].scene_duration += deltaTime;
			if (new_scene > 0) {
				Scenario[new_scene - 1].scene_final_time -= deltaTime;
				Scenario[new_scene - 1].scene_duration -= deltaTime;
			}
			// unuseful because is already made through Scenario[new_scene].scene_initial_time -= deltaTime;
			//InitialScenarioTime = CurrentClockTime - Scenario[new_scene].scene_initial_time;
			StartNewScene(new_scene);
		}
	}
	else {
		for (int indVar = 0; indVar < _MaxInterpVarIDs; indVar++) {
			if (strcmp(newCommand, CmdString[indVar]) == 0) {
				pg_update_variable(_PG_GUI_COMMAND, _PG_KEYSTROKE_VOID,
					indVar, arguments[0]);
				return;
			}
		}
	}
}

//////////////////////////////////////////////////////////////
// TRACK LIB FUNCTIONS
//////////////////////////////////////////////////////////////

void pg_path_recording_onOff( int indPath ) {
  // switches between recording on/off 
  bool is_path_record = false;
  switch (indPath) {
#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7
#ifndef ATELIERS_PORTATIFS
  case 1:
	  path_record_1 = !path_record_1;
	  is_path_record = path_record_1;
	  break;
#endif
  case 2:
	  path_record_2 = !path_record_2;
	  is_path_record = path_record_2;
	  break;
  case 3:
	  path_record_3 = !path_record_3;
	  is_path_record = path_record_3;
	  break;
#endif
#if PG_NB_PATHS == 7
  case 4:
	  path_record_4 = !path_record_4;
	  is_path_record = path_record_4;
	  break;
  case 5:
	  path_record_5 = !path_record_5;
	  is_path_record = path_record_5;
	  break;
  case 6:
	  path_record_6 = !path_record_6;
	  is_path_record = path_record_6;
	  break;
  case 7:
	  path_record_7 = !path_record_7;
	  is_path_record = path_record_7;
	  break;
#endif
  }

  // starts recording source 
  if( is_path_record ) {
    // is reading -> has to stop reading 
    if( is_path_replay[indPath] ) {
      pg_path_replay_trackNo_stop(indPath);
    }

    // launches source recording 
    pg_path_recording_start(indPath);
  } else {
    // stops source recording 
    pg_path_recording_stop(indPath);
  }
}

void pg_path_replay_trackNo_onOff(int indPath, int trackNo) {
    bool ispath_record = false;
	switch (indPath) {
#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7
	case 1:
		ispath_record = path_record_1;
		break;
	case 2:
		ispath_record = path_record_2;
		break;
	case 3:
		ispath_record = path_record_3;
		break;
#endif
#if PG_NB_PATHS == 7
	case 4:
		ispath_record = path_record_4;
		break;
	case 5:
		ispath_record = path_record_5;
		break;
	case 6:
		ispath_record = path_record_6;
		break;
	case 7:
		ispath_record = path_record_7;
		break;
#endif
	}

  // is not currently reading and positive or null track -> starts reading 
  if(!is_path_replay[indPath] && trackNo >= 0) {
	  // printf( "start_read_path #%d on track %d\n", indPath, trackNo);

    // is recording source -> has to stop recording source 
    if( ispath_record ) {
      pg_path_recording_stop(indPath);
    }

    // start reading 
    pg_path_replay_trackNo_start(indPath, trackNo);
  } 
  // is currently reading && negative track -> stops reading 
  else if (is_path_replay[indPath] && trackNo < 0) {
	  // printf("stop_read_path #%d (track No negative)\n", indPath);

    // stops reading 
    pg_path_replay_trackNo_stop(indPath);

	switch (indPath) {
#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7
	case 1:
		path_replay_trackNo_1 = -1;
		break;
	case 2:
		path_replay_trackNo_2 = -1;
		break;
	case 3:
		path_replay_trackNo_3 = -1;
		break;
#endif
#if PG_NB_PATHS == 7
	case 4:
		path_replay_trackNo_4 = -1;
		break;
	case 5:
		path_replay_trackNo_5 = -1;
		break;
	case 6:
		path_replay_trackNo_6 = -1;
		break;
	case 7:
		path_replay_trackNo_7 = -1;
		break;
#endif
	}

  }
}

void pg_path_recording_start(int indPath) {
	switch (indPath) {
#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7
	case 1:
		path_record_1 = true;
		break;
	case 2:
		path_record_2 = true;
		break;
	case 3:
		path_record_3 = true;
		break;
#endif
#if PG_NB_PATHS == 7
	case 4:
		path_record_4 = true;
		break;
	case 5:
		path_record_5 = true;
		break;
	case 6:
		path_record_6 = true;
		break;
	case 7:
		path_record_7 = true;
		break;
#endif
	}
	if (indPath >= 1
		&& indPath <= PG_NB_PATHS) {
		pg_Path_Status[indPath].isActiveRecording = true;
		pg_Path_Status[indPath].nbRecordedFrames = 0;
		// printf("start recording track %d\n",indPath);
	}
}

void pg_path_recording_stop(int indPath) {
	switch (indPath) {
#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7
	case 1:
		path_record_1 = false;
		break;
	case 2:
		path_record_2 = false;
		break;
	case 3:
		path_record_3 = false;
		break;
#endif
#if PG_NB_PATHS == 7
	case 4:
		path_record_4 = false;
		break;
	case 5:
		path_record_5 = false;
		break;
	case 6:
		path_record_6 = false;
		break;
	case 7:
		path_record_7 = false;
		break;
#endif
	}
	if (indPath >= 1
		&& indPath <= PG_NB_PATHS
		&& pg_Path_Status[indPath].isActiveRecording == true) {
		pg_Path_Status[indPath].isActiveRecording = false;
		// printf("start playing %d\n",indPath);
	}
}

void pg_path_replay_trackNo_start(int indPath, int trackNo) {
	if (indPath >= 1
		&& indPath <= PG_NB_PATHS
		&& !is_path_replay[indPath]) {
		is_path_replay[indPath] = true;
		switch (indPath) {
#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7
		case 1:
			path_replay_trackNo_1 = trackNo;
			break;
		case 2:
			path_replay_trackNo_2 = trackNo;
			break;
		case 3:
			path_replay_trackNo_3 = trackNo;
			break;
#endif
#if PG_NB_PATHS == 7
		case 4:
			path_replay_trackNo_4 = trackNo;
			break;
		case 5:
			path_replay_trackNo_5 = trackNo;
			break;
		case 6:
			path_replay_trackNo_6 = trackNo;
			break;
		case 7:
			path_replay_trackNo_7 = trackNo;
			break;
#endif
		}
		// first time reading: starts from beginning
		// otherwise starts from where it is
		if (pg_Path_Status[indPath].indReading < 0) {
			pg_Path_Status[indPath].indReading = 0;
		}
		pg_Path_Status[indPath].isFirstFrame = true;
		// printf("-> start_read_path %d\n", indPath);
	}
}
void pg_path_replay_trackNo_stop(int indPath) {
#ifdef ATELIERS_PORTATIFS
	if (indPath >= 2
#else
	if (indPath >= 1
#endif
		&& indPath <= PG_NB_PATHS
		&& is_path_replay[indPath]) {
		is_path_replay[indPath] = false;
		paths_x[indPath] = -1.0F;
		paths_y[indPath] = -1.0F;
		paths_x_prev[indPath] = -1.0F;
		paths_y_prev[indPath] = -1.0F;
		paths_Color_r[indPath] = 0.0F;
		paths_Color_g[indPath] = 0.0F;
		paths_Color_b[indPath] = 0.0F;
		paths_Color_a[indPath] = 1.0F;
		paths_RadiusX[indPath] = 0.0F;
		paths_RadiusY[indPath] = 0.0F;
		// printf( "-> start_read_path\n"  );
	}
}


void pg_path_replay_trackNo_speed_scale( int indPath, float speed ) {
  if(indPath >= 0
      && indPath < PG_NB_PATHS) {
    pg_Path_Status[indPath].readSpeedScale = speed;
  }
}


//////////////////////////////////////////////////////////////
// MISC LIB FUNCTIONS
//////////////////////////////////////////////////////////////

void pg_writeMessageOnScreen( char *text ) {
  strcpy( ScreenMessage , text ); 
  double newtime = RealTime(); 
  messageTransparency = 1.0; 
  LastDecayTime = newtime; 
  NewScreenMessage = true;
}

void pg_snapshot( char * type ) {
  if( strcmp( type , "svg" ) == 0 ) {
    pg_draw_scene( _Svg );
  }
  else if( strcmp( type , "png" ) == 0 ) {
    pg_draw_scene( _Png );
  }
  else if( strcmp( type , "jpg" ) == 0 ) {
    pg_draw_scene( _Jpg );
  }
  else {
    sprintf( ErrorStr , "Incorrect screenshot type (%s): expected svg or png or jpg!" , type ); ReportError( ErrorStr );
  }
}

