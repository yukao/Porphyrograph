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

#ifdef GN
#include "pg_script_body_GN.cpp"
#endif
#ifdef TVW
#include "pg_script_body_TVW.cpp"
#endif
#ifdef CRITON
#include "pg_script_body_Criton.cpp"
#endif
#ifdef effe
#include "pg_script_body_effe.cpp"
#endif
#ifdef DEMO
#include "pg_script_body_demo.cpp"
#endif
#ifdef VOLUSPA
#include "pg_script_body_voluspa.cpp"
#endif
#ifdef MALAUSSENA
#include "pg_script_body_Malaussena.cpp"
#endif
#ifdef DASEIN
#include "pg_script_body_dasein.cpp"
#endif
#ifdef BONNOTTE
#include "pg_script_body_bonnotte.cpp"
#endif

std::string project_name = "core";

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
// beat no
int pg_BeatNo = 0;

//////////////////////////////////////////////
// Cuurent palette after interpolation
float pen_base_3color_palette[9];
float repop_base_3color_palette[9];

#ifdef GN
//////////////////////////////////////////////
/// image number of first plane
int      firstPlaneFrameNo = -1;

//////////////////////////////////////////////
// CA initialization
float initCA = 0.0f;
#endif


/////////////////////////////////////////////////////////////////////////
// WORK VARIABLES
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

// current working directory
std::string cwd;

// track loading 
int currentSVGTrack = 0;

// sound track playing
int currentlyPlaying_trackNo = -1;
bool soundTrack_on = true;

// movie playing
int currentlyPlaying_movieNo = -1;
bool movie_on = true;

// pen preset
int current_pen_colorPreset = -1;

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
float flashCameraTrk_threshold = 0.5f;

// +++++++++++++++++++++ VIDEO BACKGROUND SUBTRACTION +++++
bool is_firstVideoOn = true;

// +++++++++++++++++++++ CLEAR +++++++++++++++++++++++++++
int isClearCA;
int isClearLayer;
int isClearAllLayers;

// +++++++++++++++++++++ BLUR +++++++++++++++++++++++++++
bool is_blur_1 = false;
bool is_blur_2 = false;
int nb_blur_frames_1 = 0;
int nb_blur_frames_2 = 0;

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
float InitialScenarioTime;
float AbsoluteInitialScenarioTime;

// +++++++++++++++++++++++ CLEAR AND INVERT +++++++++++++++++++++++++++++++++
float lastClearTime = 0.0f;

// +++++++++++++++++++++++ Beats +++++++++++++++++++++++++++++++++
int BeatNo = 1;
float lastBeatTime = 0.0f;

// +++++++++++++++++++++++ CA SubType memory for CA on/off command ++++++++++
int CASubTypeMem = 1;

#ifdef MALAUSSENA
pg_CAseed_types pg_CAseed_type = _pg_CAseed_dot_center;
pg_CAseed_locations pg_CAseed_location = _pg_CAseed_loc_center;
int pg_CAseed_coordinates[2] = { -1, -1 };
int pg_CAseed_size = 1;
bool pg_CAseed_trigger = false;
#endif


///////////////////////////////////////////////////////////////////////////////////
// HASH MAP FOR STRING COMMANDS
enum pg_stringCommands_IDs
{
	_testUDP = _MaxInterpVarIDs,
	_QT_connected,
	_PD_connected,
	_return_message,
	_launch,
	_reset_sound,
	_connect_PD,
	_quit,
	_flashCamera,
	_pulse_average,
	_pulse_spectrum,
	_beat,
	_flashTrkCA_onOff,
	_flashTrkPart_onOff,
	_snapshot,
	_initialBGCapture,
	_initCA,
	_currentVideoTrack_plus,
	_currentPhotoTrack_plus,
	_minimal_display,
	_paths_display,
	_pen_xy,
	_copyTrack_above,
	_copyTrack_under,
	_partStroke_mode_0,
	_partStroke_mode_1,
	_partStroke_mode_2,
	_partColor_mode_0,
	_partColor_mode_1,
	_partColor_mode_2,
	_partExit_mode_0,
	_partExit_mode_1,
	_partExit_mode_2,
	_is_blur_1,
	_is_blur_1_plus,
	_is_blur_1_plus_plus,
	_is_blur_2,
	_cameraCumul_plus,
	_pen_brush_plus,
	_pixel_mode_plus,
	_CA1Type_plus,
	_CA1SubType_plus,
	_CA1Type_minus,
	_CA1SubType_minus,
	_CAonOff,
	_sensor_layout_plus,
	_sample_setUp_plus,
	_sensor_activation_plus,
	_partExit_mode_plus,
	_partStroke_mode_plus,
	_partColor_mode_plus,
	_pen_BW,
	_repop_BW,
	_pressure_onOff,
	_decay_onOff,
	_diaporama_plus,
	_diaporama_minus,
	_soundtrack_plus,
	_soundtrack_onOff,
	_movie_onOff,
	_pen_colorPreset_minus,
	_pen_colorPreset_plus,
	_pen_colorPreset,
#ifdef MALAUSSENA
	_CAseed_dot_center,
	_CAseed_dot,
	_CAseed_h_line,
	_CAseed_v_line,
	_CAseed_cross,
	_CAseed_X,
	_CAseed_square,
	_CAseed_size,
	_CAseed_loc,
	_CAseed_loc_plus,
	_CAseed_trigger,
#endif
	_movie_plus,
	_resend_all,
	_setup_plus,
	_setup_minus,
	_setup_plus_keep_total_dur,
};
// Create an unordered_map of three strings (that map to strings)
std::unordered_map<std::string, int> pg_stringCommands = {
	{ "testUDP", _testUDP },
	{ "QT_connected", _QT_connected },
	{ "PD_connected", _PD_connected },
	{ "return_message", _return_message },
	{ "launch", _launch },
	{ "reset_sound", _reset_sound },
	{ "connect_PD", _connect_PD },
	{ "quit", _quit },
	{ "flashCamera", _flashCamera },
	{ "pulse_average", _pulse_average },
	{ "pulse_spectrum", _pulse_spectrum },
	{ "beat", _beat },
	{ "flashTrkCA_onOff", _flashTrkCA_onOff },
	{ "flashTrkPart_onOff", _flashTrkPart_onOff },
	{ "snapshot", _snapshot },
	{ "initialBGCapture", _initialBGCapture },
	{ "initCA", _initCA },
	{ "currentVideoTrack_plus", _currentVideoTrack_plus },
	{ "currentPhotoTrack_plus", _currentPhotoTrack_plus },
	{ "minimal_display", _minimal_display },
	{ "paths_display", _paths_display },
	{ "pen_xy", _pen_xy },
	{ "copyTrack_above", _copyTrack_above },
	{ "copyTrack_under", _copyTrack_under },
	{ "partStroke_mode_0", _partStroke_mode_0 },
	{ "partStroke_mode_1", _partStroke_mode_1 },
	{ "partStroke_mode_2", _partStroke_mode_2 },
	{ "partColor_mode_0", _partColor_mode_0 },
	{ "partColor_mode_1", _partColor_mode_1 },
	{ "partColor_mode_2", _partColor_mode_2 },
	{ "partExit_mode_0", _partExit_mode_0 },
	{ "partExit_mode_1", _partExit_mode_1 },
	{ "partExit_mode_2", _partExit_mode_2 },
	{ "is_blur_1", _is_blur_1 },
	{ "is_blur_1_plus", _is_blur_1_plus },
	{ "is_blur_1_plus_plus", _is_blur_1_plus_plus },
	{ "is_blur_2", _is_blur_2 },
	{ "cameraCumul_plus", _cameraCumul_plus },
	{ "pen_brush_plus", _pen_brush_plus },
	{ "pixel_mode_plus", _pixel_mode_plus },
	{ "CA1Type_plus", _CA1Type_plus },
	{ "CA1SubType_plus", _CA1SubType_plus },
	{ "CA1Type_minus", _CA1Type_minus },
	{ "CA1SubType_minus", _CA1SubType_minus },
	{ "CAonOff", _CAonOff },
	{ "sensor_layout_plus", _sensor_layout_plus },
	{ "sample_setUp_plus", _sample_setUp_plus },
	{ "sensor_activation_plus", _sensor_activation_plus },
	{ "partExit_mode_plus", _partExit_mode_plus },
	{ "partStroke_mode_plus", _partStroke_mode_plus },
	{ "partColor_mode_plus", _partColor_mode_plus },
	{ "pen_BW", _pen_BW },
	{ "repop_BW", _repop_BW },
	{ "pressure_onOff", _pressure_onOff },
	{ "decay_onOff", _decay_onOff },
	{ "diaporama_plus", _diaporama_plus },
	{ "diaporama_minus", _diaporama_minus },
	{ "soundtrack_plus", _soundtrack_plus },
	{ "soundtrack_onOff", _soundtrack_onOff },
	{ "movie_onOff", _movie_onOff },
	{ "pen_colorPreset_minus", _pen_colorPreset_minus },
	{ "pen_colorPreset_plus", _pen_colorPreset_plus },
	{ "pen_colorPreset", _pen_colorPreset },
#ifdef MALAUSSENA
	{ "CAseed_dot_center", _CAseed_dot_center },
	{ "CAseed_dot", _CAseed_dot },
	{ "CAseed_h_line", _CAseed_h_line },
	{ "CAseed_v_line", _CAseed_v_line },
	{ "CAseed_cross", _CAseed_cross },
	{ "CAseed_X", _CAseed_X },
	{ "CAseed_square", _CAseed_square },
	{ "CAseed_size", _CAseed_size },
	{ "CAseed_loc", _CAseed_loc },
	{ "CAseed_loc_plus", _CAseed_loc_plus },
	{ "CAseed_trigger", _CAseed_trigger },
#endif
	{ "movie_plus", _movie_plus },
	{ "resend_all", _resend_all },
	{ "setup_plus", _setup_plus },
	{ "setup_minus", _setup_minus },
	{ "setup_plus_keep_total_dur", _setup_plus_keep_total_dur },
};
// if (strcmp(newCommand, CmdString[indVar]

#ifdef MALAUSSENA
void pg_CAseed_location_to_coordinates(pg_CAseed_locations location, int coordinates[2]) {
	switch(location) {
	case 	_pg_CAseed_loc_center: {
		coordinates[0] = leftWindowWidth / 2;
		coordinates[1] = window_height / 2;
		break;
	}
	case 	_pg_CAseed_loc_N: {
		coordinates[0] = leftWindowWidth / 2;
		coordinates[1] = 0;
		break;
	}
	case 	_pg_CAseed_loc_E: {
		coordinates[0] = leftWindowWidth;
		coordinates[1] = window_height / 2;
		break;
	}
	case 	_pg_CAseed_loc_S: {
		coordinates[0] = leftWindowWidth / 2;
		coordinates[1] = window_height;
		break;
	}
	case 	_pg_CAseed_loc_W: {
		coordinates[0] = 0;
		coordinates[1] = window_height / 2;
		break;
	}
	case 	_pg_CAseed_loc_NW: {
		coordinates[0] = 0;
		coordinates[1] = 0;
		break;
	}
	case 	_pg_CAseed_loc_NE: {
		coordinates[0] = leftWindowWidth;
		coordinates[1] = 0;
		break;
	}
	case 	_pg_CAseed_loc_SE: {
		coordinates[0] = leftWindowWidth;
		coordinates[1] = window_height;
		break;
	}
	case 	_pg_CAseed_loc_SW: {
		coordinates[0] = 0;
		coordinates[1] = window_height;
		break;
	}
	}
}
#endif


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
	glUseProgram(shader_programme[pg_shader_ParticleAnimation]);

	///////////////////////////////////////////////////////////////////
	// VERTEX SHADER VARIABLE
	glUniform2f(uniform_ParticleAnimation_vp_2fv_width_height,
		(GLfloat)leftWindowWidth, (GLfloat)window_height);

	// only assigned at initialization, does not change during the set
	glUseProgram(shader_programme[pg_shader_Update]);

	///////////////////////////////////////////////////////////////////
	// VERTEX SHADER VARIABLE
	glUniform2f(uniform_Update_vp_2fv_width_height,
		(GLfloat)leftWindowWidth, (GLfloat)window_height);

	/////////////////////////////////////////////////////////////////////////
	// ADDITION OF SCENARIO-CONTROLLED UNIFORM VARIABLES TO THE HASH MAP
	for (int indVar = 0; indVar < _MaxInterpVarIDs; indVar++) {
		pg_stringCommands[std::string(CmdString[indVar])] = indVar;
	}
			
	/////////////////////////////////////////////////////////////////////////
	// INItiAL VALUES OF SCENARIO-CONTROLLED UNIFORM VARIABLES
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
	for (int indPath = 1; indPath < PG_NB_PATHS; indPath++) {
		sprintf(AuxString, "/path_replay_trackNo_%d 0", indPath);
		pg_send_message_udp((char *)"i", AuxString, (char *)"udp_QT_send");
		sprintf(AuxString, "/path_record_%d 0", indPath);
		pg_send_message_udp((char *)"i", AuxString, (char *)"udp_QT_send");
	}

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
	currentBGCapture = true;

	// particle initialization reset
	part_initialization = -1;

	// clear
	isClearCA = 0;
	isClearLayer = 0;
	isClearAllLayers = 0;

	// blur
	is_blur_1 = false;
	is_blur_2 = false;
	nb_blur_frames_1 = 0;
	nb_blur_frames_2 = 0;

#ifdef MALAUSSENA
	// CA seed
	pg_CAseed_trigger = false;
	pg_CAseed_type = _pg_CAseed_dot_center;
	// INTERFACE VARIABLE INITIALIZATION
	sprintf(AuxString, "/CAseed_dot_center %d", 1); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
	sprintf(AuxString, "/CAseed_dot %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
	sprintf(AuxString, "/CAseed_h_line %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
	sprintf(AuxString, "/CAseed_v_line %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
	sprintf(AuxString, "/CAseed_cross %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
	sprintf(AuxString, "/CAseed_X %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
	sprintf(AuxString, "/CAseed_square %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
	pg_CAseed_location = _pg_CAseed_loc_center;
	sprintf(AuxString, "/CAseed_loc %d", pg_CAseed_location); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
	pg_CAseed_coordinates[0] = leftWindowWidth / 2;
	pg_CAseed_coordinates[1] = window_height / 2;
	pg_CAseed_size = 1;
	sprintf(AuxString, "/CAseed_size %d", pg_CAseed_size); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
#endif

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
	InitialScenarioTime = InitialRealTime - 1000000.f;
	AbsoluteInitialScenarioTime = InitialRealTime - 1000000.f;
	// TIME CONTROLLED UNIFORM VARIABLES
	// message transparency & echo
	messageTransparency = 0.0;

	// pg_send_message_udp((char *)"s", (char *)"/message init_completed", (char *)"udp_QT_send");
	// INTERFACE VARIABLE INITIALIZATION
	sprintf(AuxString, "/soundtrack_onOff %d", !soundTrack_on);
	pg_send_message_udp((char *)"i", AuxString, (char *)"udp_QT_send");
	sprintf(AuxString, "/movie_onOff %d", !movie_on);
	pg_send_message_udp((char *)"i", AuxString, (char *)"udp_QT_send");
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
			if (pg_CurrentScene >= 0 && pg_CurrentScene < pg_NbScenes) {
				sprintf(AuxString, "/setup _%s", Scenario[pg_CurrentScene].scene_IDs.c_str()); pg_send_message_udp((char *)"s", AuxString, (char *)"udp_QT_send");
			}
			else {
				sprintf(AuxString, "/setup _initial_setup"); pg_send_message_udp((char *)"s", AuxString, (char *)"udp_QT_send");
			}
			sprintf(AuxString, "/pen_colorPreset %d", current_pen_colorPreset); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
			InterfaceInitializations();

			// resends the sound variables to PD (as in the callback)
			sprintf(AuxString, "/sound_env_min %.2f", float(*((float *)ScenarioVarPointers[_sound_env_min])));
			pg_send_message_udp((char *)"f", AuxString, (char *)"udp_PD_send");
			sprintf(AuxString, "/sound_env_max %.2f", float(*((float *)ScenarioVarPointers[_sound_env_max])));
			pg_send_message_udp((char *)"f", AuxString, (char *)"udp_PD_send");

			for (int ind = 0; ind < 3; ind++) {
				if (ind == *((int *)ScenarioVarPointers[_partColor_mode])) {
					sprintf(AuxString, "/partColor_mode_%d 1", ind); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
				}
				else {
					sprintf(AuxString, "/partColor_mode_%d 0", ind); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
				}
				if (ind == *((int *)ScenarioVarPointers[_partStroke_mode])) {
					sprintf(AuxString, "/partStroke_mode_%d 1", ind); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
				}
				else {
					sprintf(AuxString, "/partStroke_mode_%d 0", ind); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
				}
				if (ind == *((int *)ScenarioVarPointers[_partExit_mode])) {
					sprintf(AuxString, "/partExit_mode_%d 1", ind); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
				}
				else {
					sprintf(AuxString, "/partExit_mode_%d 0", ind); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
				}
			}

			resend_all_variables = false;
		}
		
		/////////////////////////////////////////////////////////////////////////
		// TIME CONTROLLED VARIABLES (NOT FORWARDED TO SHADERS)
		// time display
		// time exceeded in scene
		if (remainingTimeInScene >= 0) {
				sprintf(AuxString, "/time _%d:%d", (int)remainingTimeInScene / 60, (int)remainingTimeInScene % 60); pg_send_message_udp((char *)"s", AuxString, (char *)"udp_QT_send");
			if (remainingTimeInScene < 10) {
				sprintf(AuxString, "/time_color_red 1"); pg_send_message_udp((char *)"i", AuxString, (char *)"udp_QT_send");
				sprintf(AuxString, "/time_color_orange 0"); pg_send_message_udp((char *)"i", AuxString, (char *)"udp_QT_send");
				sprintf(AuxString, "/time_color_green 0"); pg_send_message_udp((char *)"i", AuxString, (char *)"udp_QT_send");
			}
			else if (remainingTimeInScene < 20) {
				sprintf(AuxString, "/time_color_red 0"); pg_send_message_udp((char *)"i", AuxString, (char *)"udp_QT_send");
				sprintf(AuxString, "/time_color_orange 1"); pg_send_message_udp((char *)"i", AuxString, (char *)"udp_QT_send");
				sprintf(AuxString, "/time_color_green 0"); pg_send_message_udp((char *)"i", AuxString, (char *)"udp_QT_send");
			}
			else {
				sprintf(AuxString, "/time_color_red 0"); pg_send_message_udp((char *)"i", AuxString, (char *)"udp_QT_send");
				sprintf(AuxString, "/time_color_orange 0"); pg_send_message_udp((char *)"i", AuxString, (char *)"udp_QT_send");
				sprintf(AuxString, "/time_color_green 1"); pg_send_message_udp((char *)"i", AuxString, (char *)"udp_QT_send");
			}
		}
		// time exceeded in scene
		else {
			int remTime = (int)remainingTimeInScene;
			if (remTime > -60) {
				sprintf(AuxString, "/time _%d:%d", remTime / 60, remTime % 60); pg_send_message_udp((char *)"s", AuxString, (char *)"udp_QT_send");
			}
			else {
				sprintf(AuxString, "/time _%d:%d", remTime / 60, -(remTime % 60)); pg_send_message_udp((char *)"s", AuxString, (char *)"udp_QT_send");
			}
			sprintf(AuxString, "/time_color 3"); pg_send_message_udp((char *)"i", AuxString, (char *)"udp_QT_send");
		}
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
		if (!hide) {
			glutPositionWindow(window_x, window_y);
			// printf(" WIDE\n");
		}
		else {
			if (double_window) {
				glutPositionWindow(window_x + doubleWindowWidth / 2, window_y);
			}
			else {
				glutPositionWindow(window_x + doubleWindowWidth, window_y);
			}
			// printf(" NARROW\n");
		}
	}
}
void auto_beat_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	lastBeatTime = CurrentClockTime;
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		sprintf(AuxString, "/auto_beat %d", (int)(*((bool *)ScenarioVarPointers[_auto_beat])));
		pg_send_message_udp((char *)"i", AuxString, (char *)"udp_PD_send");
		// printf("auto_beat [%s] %.2f %d\n", AuxString, scenario_or_gui_command_value, (int)(*((bool *)ScenarioVarPointers[_auto_beat])));
	}
}
void beat_delay_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		sprintf(AuxString, "/beat_delay %.5f", scenario_or_gui_command_value);
		pg_send_message_udp((char *)"f", AuxString, (char *)"udp_PD_send");
	}
}
void beat_threshold_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		sprintf(AuxString, "/beat_threshold %.5f", scenario_or_gui_command_value);
		pg_send_message_udp((char *)"f", AuxString, (char *)"udp_PD_send");
	}
}
void pen_color_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		sprintf(AuxString, "/pen_colorPreset -1.0");
		pg_send_message_udp((char *)"f", AuxString, (char *)"udp_QT_send");
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
#ifdef PG_WITH_CAMERA_CAPTURE
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		// sprintf(AuxString, "/message Exposure_%.1f", scenario_or_gui_command_value); pg_send_message_udp((char *)"s", (char *)AuxString, (char *)"udp_QT_send");
		if (scenario_or_gui_command_value != CameraCurrent_exposure) {
			printf("Cam exposure %.2f\n", scenario_or_gui_command_value);
			// cvSetCaptureProperty comment for see3cam
			pg_camera_capture.set(CV_CAP_PROP_EXPOSURE, scenario_or_gui_command_value);
			CameraCurrent_exposure = scenario_or_gui_command_value;
			pg_LastCameraParameterChange_Frame = pg_FrameNo;
		}
	}
#endif
}
void cameraGamma_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
#ifdef PG_WITH_CAMERA_CAPTURE
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		// sprintf(AuxString, "/message Gamma%.1f", scenario_or_gui_command_value); pg_send_message_udp((char *)"s", (char *)AuxString, (char *)"udp_QT_send");
		if (scenario_or_gui_command_value != CameraCurrent_gamma) {
			printf("Cam gamma %.2f\n", scenario_or_gui_command_value);
			// cvSetCaptureProperty comment for see3cam
			pg_camera_capture.set(CV_CAP_PROP_GAMMA, scenario_or_gui_command_value);
			CameraCurrent_gamma = scenario_or_gui_command_value;
			pg_LastCameraParameterChange_Frame = pg_FrameNo;
		}
	}
#endif
}
void cameraGain_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
#ifdef PG_WITH_CAMERA_CAPTURE
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		if (scenario_or_gui_command_value != CameraCurrent_gain) {
			printf("Cam gain %.2f\n", scenario_or_gui_command_value);
			// cvSetCaptureProperty comment for see3cam 
			pg_camera_capture.set(CV_CAP_PROP_GAIN, scenario_or_gui_command_value);
			CameraCurrent_gain = scenario_or_gui_command_value;
			pg_LastCameraParameterChange_Frame = pg_FrameNo;
		}
	}
#endif
}
void cameraBrightness_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
#ifdef PG_WITH_CAMERA_CAPTURE
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		if (scenario_or_gui_command_value != CameraCurrent_brightness) {
			printf("Cam brightness %.2f\n", scenario_or_gui_command_value);
			// cvSetCaptureProperty comment for see3cam
			pg_camera_capture.set(CV_CAP_PROP_BRIGHTNESS, scenario_or_gui_command_value);
			CameraCurrent_brightness = scenario_or_gui_command_value;
			pg_LastCameraParameterChange_Frame = pg_FrameNo;
		}
	}
#endif
}
void cameraSaturation_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
#ifdef PG_WITH_CAMERA_CAPTURE
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		if (scenario_or_gui_command_value != CameraCurrent_saturation) {
			printf("Cam saturation %.2f\n", scenario_or_gui_command_value);
			// cvSetCaptureProperty comment for see3cam 
			pg_camera_capture.set(CV_CAP_PROP_SATURATION, scenario_or_gui_command_value);
			CameraCurrent_saturation = scenario_or_gui_command_value;
			pg_LastCameraParameterChange_Frame = pg_FrameNo;
		}
	}
#endif
}
void cameraContrast_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
#ifdef PG_WITH_CAMERA_CAPTURE
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		if (scenario_or_gui_command_value != CameraCurrent_contrast) {
			printf("Cam contrast %.2f\n", scenario_or_gui_command_value);
			// cvSetCaptureProperty comment for see3cam
			pg_camera_capture.set(CV_CAP_PROP_CONTRAST, scenario_or_gui_command_value);
			CameraCurrent_contrast = scenario_or_gui_command_value;
			pg_LastCameraParameterChange_Frame = pg_FrameNo;
		}
	}
#endif
}
void cameraWB_B_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
#ifdef PG_WITH_CAMERA_CAPTURE
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		if (scenario_or_gui_command_value != CameraCurrent_WB_B) {
			printf("Cam WB B %.2f\n", scenario_or_gui_command_value);
			// cvSetCaptureProperty comment for see3cam
			pg_camera_capture.set(CV_CAP_PROP_WHITE_BALANCE_BLUE_U, scenario_or_gui_command_value);
			CameraCurrent_WB_B = scenario_or_gui_command_value;
			pg_LastCameraParameterChange_Frame = pg_FrameNo;
		}
	}
#endif
}
void cameraWB_R_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
#ifdef PG_WITH_CAMERA_CAPTURE
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		if (scenario_or_gui_command_value != CameraCurrent_WB_R) {
			printf("Cam WB R %.2f\n", scenario_or_gui_command_value);
			// cvSetCaptureProperty comment for see3cam
			if (scenario_or_gui_command_value >= 0) {
				pg_camera_capture.set(CV_CAP_PROP_WHITE_BALANCE_RED_V, scenario_or_gui_command_value);
				CameraCurrent_WB_R = scenario_or_gui_command_value;
				pg_LastCameraParameterChange_Frame = pg_FrameNo;
			}
		}
	}
#endif
}
void playing_soundtrackNo_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		if (playing_soundtrackNo != currentlyPlaying_trackNo
			&& playing_soundtrackNo >= 0 
			&& playing_soundtrackNo < nb_soundtracks) {
			soundTrack_on = true;
			sprintf(AuxString, "/soundtrack_onOff %d", soundTrack_on);
			pg_send_message_udp((char *)"i", AuxString, (char *)"udp_PD_send");
			sprintf(AuxString, "/soundtrack_onOff %d", !soundTrack_on);
			pg_send_message_udp((char *)"i", AuxString, (char *)"udp_QT_send");
			currentlyPlaying_trackNo = playing_soundtrackNo;
			PlayTrack(currentlyPlaying_trackNo);
		}
	}
}
void playing_movieNo_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		if (playing_movieNo != currentlyPlaying_movieNo
			&& playing_movieNo >= 0 && playing_movieNo < nb_movies) {
			currentlyPlaying_movieNo = playing_movieNo;
			movie_on = true;
			sprintf(AuxString, "/movie_onOff %d", !movie_on);
			pg_send_message_udp((char *)"i", AuxString, (char *)"udp_QT_send");
			sprintf(AuxString, "/movie_shortName %s", movieShortName[playing_movieNo].c_str());
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
		if (scenario_or_gui_command_value >= 0 && scenario_or_gui_command_value != pg_CurrentDiaporamaDir) {
			pg_CurrentDiaporamaDir = int(scenario_or_gui_command_value);
			printf("pg_CurrentDiaporamaDir %d\n", pg_CurrentDiaporamaDir);
			pg_launch_diaporama();
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
void clearAllLayers_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_KEYSTROKE) {
		// printf("delta %.3f\n", (float)(CurrentClockTime - lastClearTime) );
		if (CurrentClockTime - lastClearTime < 0.5f) {
			// pg_send_message_udp((char *)"s", (char *)"/message clear_all", (char *)"udp_QT_send");
			isClearAllLayers = 1;
			lastClearTime = 0.0f;
			printf("****** isClearAllLayers %d\n",isClearAllLayers);
		}
		else {
			lastClearTime = CurrentClockTime;
		}
	}
	else if (param_input_type == _PG_SCENARIO && scenario_or_gui_command_value > 0.0) {
		// pg_send_message_udp((char *)"s", (char *)"/message clear_all", (char *)"udp_QT_send");
		isClearAllLayers = 1;
		lastClearTime = CurrentClockTime;
		// printf("****** isClearAllLayers %d\n", isClearAllLayers);
	}
}
void clearLayer_callBack( pg_Parameter_Input_Type param_input_type , float scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_KEYSTROKE) {
		if( CurrentClockTime - lastClearTime  < 0.2f ) {
			isClearLayer = 1;
			lastClearTime = 0.0f;
			// pg_send_message_udp((char *)"s", (char *)"/message clear_all_layers", (char *)"udp_QT_send");
		}
		else {
			isClearLayer = 1;
			lastClearTime = CurrentClockTime;
			// pg_send_message_udp((char *)"s", (char *)"/message clear_layer", (char *)"udp_QT_send");
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
		// pg_send_message_udp((char *)"s", (char *)"/message clear_CA", (char *)"udp_QT_send");
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
			sprintf(AuxString, "/path_replay_trackNo_%d 0", pathNo);
			pg_send_message_udp((char *)"i", AuxString, (char *)"udp_QT_send");
			//printf("replay unchanged (stop) invalid track\n");
			return;
		}

		// only reads a track that has been recorded
		// assumes that no svg track has been loaded for this track
		if (recorded_path[pathNo] == true) {
			if (tracksSync) {
				synchr_start_path_replay_trackNo[pathNo] = playing_track;
			}
			else {
				pg_path_replay_trackNo_onOff(pathNo, playing_track);
			}
			sprintf(AuxString, "/path_replay_trackNo_%d 1", pathNo);
			pg_send_message_udp((char *)"i", AuxString, (char *)"udp_QT_send");
			//printf("replay on recorded track (%s)\n", AuxString);
		}
		else {
			sprintf(AuxString, "/path_replay_trackNo_%d 0", pathNo);
			pg_send_message_udp((char *)"i", AuxString, (char *)"udp_QT_send");
			//printf("replay off non recorded track\n");
		}
	}
	// is currently reading && playing_track < 0 (scenario) or on/off command -> stops reading 
	else {
		if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_KEYSTROKE) {
			playing_track = -1;
		}
		if (playing_track == -1) {
			pg_path_replay_trackNo_onOff(pathNo, playing_track);
			sprintf(AuxString,  "/path_replay_trackNo_%d 0", pathNo);
			pg_send_message_udp((char *)"i", AuxString, (char *)"udp_QT_send");
			//printf("replay off track was currently read (%s)\n", AuxString);
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
	case 1:
		isTrackRecord = path_record_1;
		break;
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
			// stops recording 
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
		// is currently recording -> stops recording 
		else {
			if (isTrackRecord) {
				// stops recording 
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
void part_move_init(void) {
#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7
	*((bool *)ScenarioVarPointers[_part_path_follow_0]) = false;
	*((bool *)ScenarioVarPointers[_part_path_follow_1]) = false;
	*((bool *)ScenarioVarPointers[_part_path_follow_2]) = false;
	*((bool *)ScenarioVarPointers[_part_path_follow_3]) = false;
	*((bool *)ScenarioVarPointers[_part_path_repulse_0]) = false;
	*((bool *)ScenarioVarPointers[_part_path_repulse_1]) = false;
	*((bool *)ScenarioVarPointers[_part_path_repulse_2]) = false;
	*((bool *)ScenarioVarPointers[_part_path_repulse_3]) = false;
#endif
#if PG_NB_PATHS == 7
	*((bool *)ScenarioVarPointers[_part_path_follow_4]) = false;
	*((bool *)ScenarioVarPointers[_part_path_follow_5]) = false;
	*((bool *)ScenarioVarPointers[_part_path_follow_6]) = false;
	*((bool *)ScenarioVarPointers[_part_path_follow_7]) = false;
	*((bool *)ScenarioVarPointers[_part_path_repulse_4]) = false;
	*((bool *)ScenarioVarPointers[_part_path_repulse_5]) = false;
	*((bool *)ScenarioVarPointers[_part_path_repulse_6]) = false;
	*((bool *)ScenarioVarPointers[_part_path_repulse_7]) = false;
#endif
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
		*((bool *)ScenarioVarPointers[_part_path_follow_0]) = state;
		break;
	case 1:
		*((bool *)ScenarioVarPointers[_part_path_follow_1]) = state;
		break;
	case 2:
		*((bool *)ScenarioVarPointers[_part_path_follow_2]) = state;
		break;
	case 3:
		*((bool *)ScenarioVarPointers[_part_path_follow_3]) = state;
		break;
	}
#endif
#if PG_NB_PATHS == 7
	switch (pathNo) {
	case 4:
		*((bool *)ScenarioVarPointers[_part_path_follow_4]) = state;
		break;
	case 5:
		*((bool *)ScenarioVarPointers[_part_path_follow_5]) = state;
		break;
	case 6:
		*((bool *)ScenarioVarPointers[_part_path_follow_6]) = state;
		break;
	case 7:
		*((bool *)ScenarioVarPointers[_part_path_follow_7]) = state;
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
		*((bool *)ScenarioVarPointers[_part_path_repulse_0]) = state;
		break;
	case 1:
		*((bool *)ScenarioVarPointers[_part_path_repulse_1]) = state;
		break;
	case 2:
		*((bool *)ScenarioVarPointers[_part_path_repulse_2]) = state;
		break;
	case 3:
		*((bool *)ScenarioVarPointers[_part_path_repulse_3]) = state;
		break;
	}
#endif
#if PG_NB_PATHS == 7
	switch (pathNo) {
	case 4:
		*((bool *)ScenarioVarPointers[_part_path_repulse_4]) = state;
		break;
	case 5:
		*((bool *)ScenarioVarPointers[_part_path_repulse_5]) = state;
		break;
	case 6:
		*((bool *)ScenarioVarPointers[_part_path_repulse_6]) = state;
		break;
	case 7:
		*((bool *)ScenarioVarPointers[_part_path_repulse_7]) = state;
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
	bool state = partMove_target;
	if (!state)
		return; // nothing to do for null values
	partMove_rand = false;
	partMove_target = state;
}
void partMove_rand_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	bool state = partMove_rand;
	if (!state)
		return; // nothing to do for null values
	partMove_target = false;
	partMove_rand = state;
}
void part_initialization_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	pg_targetFrameNo = pg_FrameNo + int(part_timeToTargt);
	// printf("part initialization call back %d\n", part_initialization);
}
#ifdef PG_SENSORS
void sensor_layout_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	// copies the grid layout
	assignSensorPositions();
}
void sensor_activation_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	// copies the grid layout
	assignSensorActivations();
}
void sample_setUp_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	sample_setUp_interpolation();
}
#endif

///////////////////////////////////////////////////////////////////////////////////
// SCENARIO BASED COMMANDS
///////////////////////////////////////////////////////////////////////////////////
void pg_update_variable(pg_Parameter_Input_Type param_input_type,
			  pg_Keystroke_Input_Type parm_keystroke_type,
			  int indParam,
			  float scenario_or_gui_command_value ) {
	// save previous values of variables that have to know their previous value in their 

	float step = 0.0f;
	if (parm_keystroke_type == _PG_KEYSTROKE_MINUS) {
		step = StepMinus[indParam];
	}
	else {
		step = StepPlus[indParam];
	}
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
		else if (param_input_type == _PG_KEYSTROKE) {
			*((float *)ScenarioVarPointers[indParam]) += step;
		}
		if (ScenarioVarCallbacks[indParam]) {
			(*ScenarioVarCallbacks[indParam])(param_input_type,scenario_or_gui_command_value);
		}
	}
	else if (ScenarioVarTypes[indParam] == _pg_int) {
		if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
			if (indParam == _CA1Type || indParam == _CA2Type ||
				indParam == _CA1SubType || indParam == _CA2SubType) {
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
		else if (param_input_type == _PG_KEYSTROKE) {
			*((int *)ScenarioVarPointers[indParam]) = (*((int *)ScenarioVarPointers[indParam]) + (int)step + (int)MaxValues[indParam] + 1) % ((int)MaxValues[indParam] + 1);
		}
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

#if defined (TVW) || defined (CRITON)
int SubScenesDiaporamaDir(int currentScene) {
	switch (currentScene) {
	case 0:
			return 0;
			break;
		case 1:
			return 0;
			break;
		case 2://letsgo
			return 0;
			break;
		case 3://phoneclear
			return 7;
			break;
		case 4://phonedark
			return 9;
			break;
		case 5://palestine_in
			return 11;
			break;
		case 6://palestine
			return 11;
			break;
		case 7://gbagbo_in
			return 13;
			break;
		case 8://gbagbo
			return 13;
			break;
		case 9://armee
			return 17;
			break;
		case 10://armee_flash
			return 22;
			break;
		case 11://SoE
			return 23;
			break;
		case 12://Master
			return 23;
			break;
		default:
			return 23;
			break;
}}
void pg_update_visual_and_text_chapters(bool new_scene) {
	// all scenes except first and last
	// the first scene is not special in case of relaunch
	if ((pg_CurrentScene > 0 || pg_NbScenes < 13) && (pg_CurrentScene < pg_NbScenes - 1)) {
		int old_pg_CurrentDiaporamaDir = pg_CurrentDiaporamaDir;
		if (new_scene) {
			pg_CurrentDiaporamaDir = SubScenesDiaporamaDir(pg_CurrentScene);
		}
		else {
			// possible text and visual scene advance
			float elapsed_time_from_start = CurrentClockTime - InitialScenarioTime;

			pg_CurrentDiaporamaDir
				= (int(elapsed_time_from_start - Scenario[pg_CurrentScene].scene_initial_time) / 60)
				+ SubScenesDiaporamaDir(pg_CurrentScene);
		}
		if (pg_CurrentDiaporamaDir >= pg_nbCompressedImageDirs) {
			pg_CurrentDiaporamaDir = pg_nbCompressedImageDirs - 1;
		}
		if (old_pg_CurrentDiaporamaDir != pg_CurrentDiaporamaDir) {
			// IMAGE CHOICE AND SWAP INIT
			// sets the directory for images
			// the first scene starts with 6 preloaded images
			if (pg_CurrentDiaporamaDir == 0)
				pg_CurrentDiaporamaFile = 6;
			else
				pg_CurrentDiaporamaFile = 0;
			// printf("scene / diaporama dir %d %d\n", pg_CurrentScene, pg_CurrentDiaporamaDir);

			// TEXT CHOICE AND SWAP INIT
			// sets the index for messages
			// initialization (first message to be displayed)
			if (DisplayText1Front) {
				IndDisplayText2 = DisplayTextFirstInChapter[pg_CurrentDiaporamaDir];
				LengthDisplayText2 = (float)pg_displayMessage_update(2);
				IndDisplayText1 = DisplayTextFirstInChapter[pg_CurrentDiaporamaDir] + 1;
				LengthDisplayText1 = (float)pg_displayMessage_update(1);
				DisplayText1Front = false;
				DisplayText1Alpha = 1.0f;
				DisplayText2Alpha = 0.0f;
			}
			else {
				IndDisplayText1 = DisplayTextFirstInChapter[pg_CurrentDiaporamaDir];
				LengthDisplayText1 = (float)pg_displayMessage_update(1);
				IndDisplayText2 = DisplayTextFirstInChapter[pg_CurrentDiaporamaDir] + 1;
				LengthDisplayText2 = (float)pg_displayMessage_update(2);
				DisplayText1Front = true;
				DisplayText1Alpha = 0.0f;
				DisplayText2Alpha = 1.0f;
			}
			DisplayTextSwapInitialTime = CurrentClockTime;
			//printf("New text DisplayText1Front %d index 1/2 %d/%d dir %d dir size %d\n", int(DisplayText1Front), IndDisplayText1, IndDisplayText2, pg_CurrentDiaporamaDir, DisplayTextFirstInChapter[pg_CurrentDiaporamaDir]);
		}

		// sets the start index for available image layer to the first layer
		if (new_scene) {
			pg_IndInitialSwapPhoto = 0;
		}

		if (pg_CurrentDiaporamaDir >= pg_nbCompressedImageDirs) {
			pg_CurrentDiaporamaDir = pg_nbCompressedImageDirs - 1;
		}
	}
	else {
		// no visuals / no tweets
		IndDisplayText1 = -1;
		IndDisplayText1 = -1;
		DisplayText1Alpha = 0.0f;
		DisplayText2Alpha = 0.0f;
	}
}
#endif

void StartNewScene(int ind_scene) {
	pg_CurrentScene = ind_scene;
	pg_FirstFrameInScene = true;
	sprintf(AuxString, "/setup _%s", Scenario[pg_CurrentScene].scene_IDs.c_str()); pg_send_message_udp((char *)"s", AuxString, (char *)"udp_QT_send");
	// printf( "/setup %s\n" , Scenario[pg_CurrentScene].scene_IDs.c_str() ); 
	// reinitialization of the interpolation control variables at the beginning of a new scene
	for (int indVar = 0; indVar < _MaxInterpVarIDs; indVar++) {
		BrokenInterpolationVar[indVar] = false;
	}
	// stops ongoing flashes if there is one
	flashCameraTrk_weight = 0.0f;

#if defined (TVW) || defined (CRITON)
	// updates image and text directories
	pg_update_visual_and_text_chapters(true);
#endif
}

#if defined (TVW) || defined (CRITON)
float starting_time(float elapsed_time_from_start) {
	if (pg_NbScenes == 13) {
		if (elapsed_time_from_start > Scenario[2].scene_initial_time ) {
			return Scenario[2].scene_initial_time;
		}
		else {
			return 0.f;
		}
	}
	else if (pg_NbScenes == 11) { // relaunch 6mn
		return -360.f;
	}
	else if (pg_NbScenes == 7) { // relaunch 12
		return -720.f;
	}
	else if (pg_NbScenes == 4) { // relaunch 20
		return -1200.f;
	}
	return 0.f;
}
#endif

void pg_update_scenario(void) {
	float elapsed_time_from_start = CurrentClockTime - InitialScenarioTime;

#if defined (TVW) || defined (CRITON)
	pg_update_visual_and_text_chapters(false);
#endif

	// scans all scenes to find the current one and applies the corresponding interpolations
	for (int ind_scene = 0; ind_scene < pg_NbScenes; ind_scene++) {
		// printf( "time %.2f beg %.2f end %.2f\n" , elapsed_time_from_start ,  Scenario[ind_scene].scene_initial_time ,  Scenario[ind_scene].scene_final_time );
		// current scene found
		if (elapsed_time_from_start >= Scenario[ind_scene].scene_initial_time
			&& elapsed_time_from_start < Scenario[ind_scene].scene_final_time) {
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
					case pg_bezier_interpolation:
						// approximation of a bezier with cosine (more slow at beginning and end)
						transformedInterpolation
							= 0.5F * (cos((3 * pow(fabs(coefInterpolation), 2) - 2 * pow(fabs(coefInterpolation), 3) - 1.0F) * (float)M_PI) + 1.0F);
						break;
					case pg_exponential_interpolation:
						// approximation of a bezier with cosine (more slow at beginning and end)
						transformedInterpolation
							= pow(fabs(coefInterpolation), Scenario[ind_scene].scene_interpolations[indP].exponent);
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
					case pg_stepwise_interpolation:
						printf("Stepwise interpolations have no intermediate values\n");
						break;
					default:
						printf("Unknown interpolation mode!\n");
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
	  currentBGCapture = true;
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

void PlayTrack(int indTrack) {
	if (nb_soundtracks > 0) {
		// std::cout << "cwd: " << cwd << std::endl;
		currentlyPlaying_trackNo = indTrack % nb_soundtracks;
		sprintf(AuxString, "/soundtrack_fileName %s",
			(cwd + "/Data/" + project_name + "-data/soundtracks/" + trackFileName[currentlyPlaying_trackNo]).c_str());
		pg_send_message_udp((char *)"s", AuxString, (char *)"udp_PD_send");
		//sprintf(AuxString, "/soundtrack_shortName %s", trackShortName[currentlyPlaying_trackNo].c_str());
		//pg_send_message_udp((char *)"s", AuxString, (char *)"udp_QT_send");
		printf("soundtrack #%d launching %s\n", currentlyPlaying_trackNo,
			trackFileName[currentlyPlaying_trackNo].c_str());
		BrokenInterpolationVar[_playing_soundtrackNo] = true;
		currentlyPlaying_trackNo = indTrack;
	}
}

void pg_launch_performance(void) {
	// sprintf(AuxString, "/message launching"); pg_send_message_udp((char *)"s", AuxString, (char *)"udp_QT_send");
	
	// cleaning up
	isClearCA = 1; // clean CA layer
	isClearAllLayers = 1; // clears bg layer

	pg_LaunchFrameNo = pg_FrameNo;

	// restarts scenario
	restoreInitialTimesAndDurations();
	InitialScenarioTime = CurrentClockTime - Scenario[0].scene_initial_time;
	AbsoluteInitialScenarioTime = CurrentClockTime - Scenario[0].scene_initial_time;
	sprintf(AuxString, "/setup _%s", Scenario[0].scene_IDs.c_str()); pg_send_message_udp((char *)"s", AuxString, (char *)"udp_QT_send");
	// printf( "/setup %s" , Scenario[0].scene_IDs.c_str() ); 
	// reinitialization of the interpolation control variables at the beginning of a new scene
	for (int indVar = 0; indVar < _MaxInterpVarIDs; indVar++) {
		BrokenInterpolationVar[indVar] = false;
	}

	// removes snaptshots before launching
	remove_files_in_dir(&snapshots_dir_path_name); // removes snapshots before pg_launch_performance

	// lights up the LED
	pg_send_message_udp((char *)"f", (char *)"/launch 1", (char *)"udp_QT_send");

#ifdef GN
	initCA = 1.2f;
#endif

	StartNewScene(0);
}

void setup_plus(void) {
	if (pg_CurrentScene < 0) {
		pg_launch_performance();
	}
	else {
		restoreInitialTimesAndDurations();
		int new_scene = ((1 + pg_CurrentScene) % pg_NbScenes);
		InitialScenarioTime = CurrentClockTime - Scenario[new_scene].scene_initial_time;
		StartNewScene(new_scene);
	}
}

void setup_minus(void) {
	restoreInitialTimesAndDurations();
	int new_scene = ((pg_CurrentScene - 1 + pg_NbScenes) % pg_NbScenes);
	InitialScenarioTime = CurrentClockTime - Scenario[new_scene].scene_initial_time;
	StartNewScene(new_scene);
}

void pg_keyStrokeScripts(int key) {
	pg_Keystroke_Input_Type CommandType = _PG_KEYSTROKE_VOID;
	char *CmdChar = NULL;
	int indChar = -1;
	bool oneCommandIssued = false;

	char * ptCh = CmdCharMinus;
	while ((CmdChar = strchr(ptCh, key)) != NULL) {
		CommandType = _PG_KEYSTROKE_MINUS;
		indChar = CmdChar - CmdCharMinus;
		if (indChar >= 0) {
			pg_update_variable(_PG_KEYSTROKE, CommandType,
				indChar, 0.0F);
			oneCommandIssued = true;
		}
		ptCh = CmdChar + 1;
	}
	if (oneCommandIssued) {
		return;
	}
	ptCh = CmdCharPlus;
	if ((CmdChar = strchr(ptCh, key)) != NULL) {
		CommandType = _PG_KEYSTROKE_PLUS;
		indChar = CmdChar - CmdCharPlus;
		// printf("indChar %d\n", indChar);
		if (indChar >= 0) {
			pg_update_variable(_PG_KEYSTROKE, CommandType,
				indChar, 0.0F);
			oneCommandIssued = true;
		}
		ptCh = CmdChar + 1;
	}
	if (oneCommandIssued) {
		return;
	}

	// printf("indChar %d pixel_acc_factor %d\n", indChar, _pixel_acc_factor);

	switch (key) {

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// +++++++++++++++++ TEST UDP ++++++++++++++++++++++++++++++ 
	// +++++++++++++++++   keystroke (T)  ++++++++++++++++++++++ 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	case'T':
		pg_writeMessageOnScreen((char *)"******");
		pg_send_message_udp((char *)"f", (char *)"/testUDP 0", (char *)"udp_SC");
		sprintf(AuxString, "/message TEST_UDP"); pg_send_message_udp((char *)"s", AuxString, (char *)"udp_QT_send");
		sprintf(AuxString, "/return_message returnUPD_%f", CurrentClockTime); pg_send_message_udp((char *)"s", AuxString, (char *)"udp_QT_send");
		break;

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// +++++++++++++++++ LAUNCH TIMER ++++++++++++++++++++++++++ 
	// +++++++++++++++++   keystroke (t)  ++++++++++++++++++++++ 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	case't':
		pg_launch_performance();
		break;

		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
		// +++++++++++++++++ BLUR TRACK 1 ++++++++++++++++++++++++++ 
		// +++++++++++++++++   keystroke (b)  ++++++++++++++++++++++ 
		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	case 'b': {
		is_blur_1 = true;
		break;
	}


	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// +++++++++++++++++ SET-UP ++++++++++++++++++++++++++++++++ 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// ====================================== 
	// advances to next scene without respect for the global timing
	case'S':
		setup_plus();
		break;

	// advances to next scene and keeps the global timing
	case's': {
		restoreInitialTimesAndDurations();
		float elapsedTime = (CurrentClockTime - InitialScenarioTime) - Scenario[pg_CurrentScene].scene_initial_time;
		// only accepted if the current scene has been on for a while
		if (elapsedTime > 10) {
			float deltaTime = Scenario[1 + pg_CurrentScene].scene_initial_time - (CurrentClockTime - InitialScenarioTime);
			// if the scene has not yet begun
			if (deltaTime > 0) {
				int new_scene = ((1 + pg_CurrentScene) % pg_NbScenes);
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
		break;
	}

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// +++++++++++++++++ CA SEEDING ++++++++++++++++++++++++++++ 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// ====================================== 
#ifdef MALAUSSENA
	case'C':
		pg_CAseed_trigger = true;
		break;
#endif

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// +++++++++++++ FLASHES BETWEEN LAYERS, CA & PARTICLES ++++ 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// flashPixel          (f)                 
	// flashTrkPart        (F)                 
	// flashTrkCA          (h)                 
	// flashCABG           (H)                 
	// flashCAPart         (i)                 
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
	std::string newCommand(command_symbol);

	// unregistered command
	if (pg_stringCommands.find(newCommand) == pg_stringCommands.end()) {
		sprintf(ErrorStr, "Unknown command (%s)!", command_symbol); ReportError(ErrorStr);
		return;
	}

	// command from the scenario file
	if (pg_stringCommands[newCommand] < _MaxInterpVarIDs) {
		pg_update_variable(_PG_GUI_COMMAND, _PG_KEYSTROKE_VOID,
			pg_stringCommands[newCommand], arguments[0]);
		return;
	}

	// special command not in the scenario file
	switch (pg_stringCommands[newCommand]) {

	// ====================================== 
	// test UDP connection                    
	// ====================================== 
	case _testUDP: {
		sprintf(AuxString, "*** OK ***"); pg_writeMessageOnScreen(AuxString);
		sprintf(AuxString, "/message TEST_UDP"); pg_send_message_udp((char *)"s", AuxString, (char *)"udp_QT_send");
		// from Yukao's QT
		break;
	}
	case _QT_connected: {
		sprintf(AuxString, "*** QT ***"); pg_writeMessageOnScreen(AuxString);
		break;
	}

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// +++++++++++++++++ PD CONNECTION CONFIRMATION ++++++++++++ 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	case _PD_connected: {
		// from Yukao's PD
		pg_send_message_udp((char *)"s", (char *)"/PD_connected 1", (char *)"udp_QT_send");
		sprintf(AuxString, "*** PD ***"); pg_writeMessageOnScreen(AuxString);
		break;
	}
	case _return_message: {
		pg_writeMessageOnScreen(string_argument_0);
		break;
	}

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// +++++++++++++++++ LAUNCH TIMER ++++++++++++++++++++++++++ 
	// +++++++++++++++++   keystroke (t)  ++++++++++++++++++++++ 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	case _launch: {
		pg_launch_performance();
		break;
	}

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// +++++++++++++++++ SOUND RESET ++++++++++++++++++++++++ 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	case _reset_sound: {
		pg_send_message_udp((char *)"i", (char *)"/reset_sound 1", (char *)"udp_PD_send");
		break;
	}

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// +++++++++++++++++ CONNECTS PD ++++++++++++++++++++++++++ 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	case _connect_PD: {
		pg_send_message_udp((char *)"i", (char *)"/connect 1", (char *)"udp_PD_send");

		break;
	}

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// +++++++++++++++++ QUITS +++++++++++++++++++++++++++++++++ 
	// +++++++++++++++++   keystroke (ESC)  ++++++++++++++++++++ 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	case _quit: {
		quit();
		break;
	}

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// +++++++++++++++++ VIDEO FLASH ++++++++++++++++++++++++ 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	case _flashCamera: {
		is_flashCameraTrk = true;
		flashCameraTrk_weight = 0.0f;
		// printf("start flash\n");
		//    break;
		break;
	}

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// +++++++++++++++++ MUSIC CONTROL +++++++++++++++++++++++++ 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	case _pulse_average: {
		//	pulse_average_prec = pulse_average;
		//	pulse_average = arguments[0] * sound_volume + sound_min;
		//	sprintf(AuxString, "/pulse_enveloppe %.5f", pulse[2]);
		//	pg_send_message_udp((char *)"f", AuxString, (char *)"udp_QT_send");

		//}
		break;
	}

	case _pulse_spectrum: {
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
		break;
	}

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// +++++++++++++++++ FLASHES +++++++++++++++++++++++++++++++ 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// ======================================== 
	// flash triggering according to beat & frequency, weight, and on/off values 
	// ======================================== 
	// flashes are synchronized on beats according to their frequency
	case _beat: {
		int argt0 = (int)round(arguments[0]);
		if (argt0 >= 0) {
			pg_BeatNo = argt0;
		}
		else {
			pg_BeatNo++;
		}

		sprintf(AuxString, "/loop_beat %d", pg_BeatNo % PG_LOOP_SIZE); pg_send_message_udp((char *)"i", AuxString, (char *)"udp_QT_send");

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
			flashCameraTrk_threshold = std::min(std::max(0.0f, 1.0f - flashCameraTrk_decay * flashCameraTrkBright), 1.0f);
			//printf("flashCameraTrk_weight %.3f decay %.2f\n", 
			//	flashCameraTrk_weight , flashCameraTrk_decay );
		}

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

#if defined (TVW) || defined (CRITON)
		// updates display messages according to text_swap_freq (4)
		// for a swap duration of message_swap_duration
		if (AbsoluteInitialScenarioTime != InitialRealTime - 1000000.f
			&& pg_CurrentDiaporamaDir != -1
			&& (pg_BeatNo % PG_LOOP_SIZE / 2 == 0)) {
			//printf("DisplayText1Front %d index 1/2 %d/%d dir %d dir size %d/%d\n", int(DisplayText1Front), IndDisplayText1, IndDisplayText2, pg_CurrentDiaporamaDir, DisplayTextFirstInChapter[pg_CurrentDiaporamaDir], DisplayTextFirstInChapter[pg_CurrentDiaporamaDir + 1]);
			if (DisplayText1Front) {
				IndDisplayText2 = (IndDisplayText2 + 2);
				if (IndDisplayText2 > DisplayTextFirstInChapter[pg_CurrentDiaporamaDir + 1]) {
					IndDisplayText2 -= (DisplayTextFirstInChapter[pg_CurrentDiaporamaDir + 1] - DisplayTextFirstInChapter[pg_CurrentDiaporamaDir]);
				}
				DisplayText1Front = false;
				LengthDisplayText2 = (float)pg_displayMessage_update(2);
				DisplayText1Alpha = 1.0f;
				DisplayText2Alpha = 0.0f;
			}
			else {
				IndDisplayText1 = (IndDisplayText1 + 2);
				if (IndDisplayText1 > DisplayTextFirstInChapter[pg_CurrentDiaporamaDir + 1]) {
					IndDisplayText1 -= (DisplayTextFirstInChapter[pg_CurrentDiaporamaDir + 1] - DisplayTextFirstInChapter[pg_CurrentDiaporamaDir]);
				}
				DisplayText1Front = true;
				LengthDisplayText1 = (float)pg_displayMessage_update(1);
				DisplayText1Alpha = 0.0f;
				DisplayText2Alpha = 1.0f;
			}
			DisplayTextSwapInitialTime = CurrentClockTime;
		}

		// updates display images according to image_swap_freq
		// for a swap duration of image_swap_duration
		// printf("image_swap_freq %d pg_BeatNo %d \n",image_swap_freq,pg_BeatNo);
		if (AbsoluteInitialScenarioTime != InitialRealTime - 1000000.f
			&& pg_CurrentDiaporamaDir != -1
			// && image_swap_freq > 0
			&& ((CurrentClockTime - LastImageSwap) >= photo_diaporama_plateau)) {
			int nextCompressedImage;
			if ((nextCompressedImage
				= nextFileIndexMemoryLoop(pg_CurrentDiaporamaDir,
					&pg_CurrentDiaporamaFile, true)) != NULL_ID) {
				if (pg_swap_image(nextCompressedImage)) {
				}
			}
		}
#endif

		break;
	}

	// ======================================== 
	// flash on/off values
	// ======================================== 
	case _flashTrkCA_onOff: {
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
		break;
	}
	case _flashTrkPart_onOff: {
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

		break;
	}

	// ====================================== 
	// keystroke s: Snapshot                  
	// ====================================== 
	case _snapshot: {
		pg_snapshot((char *)"jpg");

#ifdef GN
		// ====================================== 
		// relaunches CA initialization, in case it does not work                  
		// ====================================== 
		break;
		}
	case _initialBGCapture: {
		initialBGCapture = true;

		// ====================================== 
		// initial background capture                  
		// ====================================== 
		break;
	}
	case _initCA: {
		initCA = 1.2f;
#endif
		break;
	}

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// +++++++++++++++++ TRACK SHIFT +++++++++++++++++++++++++++ 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	case _currentVideoTrack_plus: {
		currentVideoTrack = (currentVideoTrack + 1) % PG_NB_TRACKS;

		break;
	}
	case _currentPhotoTrack_plus: {
		currentPhotoTrack = (currentPhotoTrack + 1) % PG_NB_TRACKS;
		break;
	}

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// +++++++++++++++++ INTERFACE SELECTION +++++++++++++++++++ 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	case _minimal_display: {
		pg_send_message_udp((char *)"i", (char *)"/minimal_display 1", (char *)"udp_QT_send");
		break;
	}
	case _paths_display: {
		pg_send_message_udp((char *)"i", (char *)"/paths_display 1", (char *)"udp_QT_send");
		break;
	}

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// +++++++++++++++++ PEN POSITION ++++++++++++++++++++++++++ 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	case _pen_xy: {
		CurrentMousePos_x = int(leftWindowWidth * arguments[1]);
		CurrentMousePos_y = int(window_height * (1.f - arguments[0]));
		break;
	}

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// +++++++++++++++++ TRACK COPY +++++++++++++++++++++++++++ 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	case _copyTrack_above: {
		copyToNextTrack = +1;
		//sprintf(AuxString, "/message copy_track_%d_to_%d",
		//	currentDrawingTrack, (currentDrawingTrack + 1) % PG_NB_TRACKS);
		//pg_send_message_udp((char *)"s", (char *)AuxString, (char *)"udp_QT_send");

		break;
	}
	case _copyTrack_under: {
		copyToNextTrack = -1;
		//sprintf(AuxString, "/message copy_track_%d_to_%d", 
		//	currentDrawingTrack , (currentDrawingTrack - 1 + PG_NB_TRACKS) % PG_NB_TRACKS); 
		//pg_send_message_udp((char *)"s", (char *)AuxString, (char *)"udp_QT_send");


		break;
	}

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// +++++++++++++++++ PART EXIT/STROKE/COLOR MODE +++++++++++++++++++++++++++ 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	case _partStroke_mode_0: {
		*((int *)ScenarioVarPointers[_partStroke_mode]) = 0;

		for (int ind = 0; ind < 3; ind++) {
			if (ind == *((int *)ScenarioVarPointers[_partStroke_mode])) {
				sprintf(AuxString, "/partStroke_mode_%d 1", ind); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
			}
			else {
				sprintf(AuxString, "/partStroke_mode_%d 0", ind); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
			}
		}
		break;
	}
	case _partStroke_mode_1: {
		*((int *)ScenarioVarPointers[_partStroke_mode]) = 1;

		for (int ind = 0; ind < 3; ind++) {
			if (ind == *((int *)ScenarioVarPointers[_partStroke_mode])) {
				sprintf(AuxString, "/partStroke_mode_%d 1", ind); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
			}
			else {
				sprintf(AuxString, "/partStroke_mode_%d 0", ind); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
			}
		}
		break;
	}
	case _partStroke_mode_2: {
		*((int *)ScenarioVarPointers[_partStroke_mode]) = 2;

		for (int ind = 0; ind < 3; ind++) {
			if (ind == *((int *)ScenarioVarPointers[_partStroke_mode])) {
				sprintf(AuxString, "/partStroke_mode_%d 1", ind); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
			}
			else {
				sprintf(AuxString, "/partStroke_mode_%d 0", ind); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
			}
		}
		break;
	}
	case _partColor_mode_0: {
		*((int *)ScenarioVarPointers[_partColor_mode]) = 0;

		for (int ind = 0; ind < 3; ind++) {
			if (ind == *((int *)ScenarioVarPointers[_partColor_mode])) {
				sprintf(AuxString, "/partColor_mode_%d 1", ind); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
			}
			else {
				sprintf(AuxString, "/partColor_mode_%d 0", ind); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
			}
		}
		break;
	}
	case _partColor_mode_1: {
		*((int *)ScenarioVarPointers[_partColor_mode]) = 1;

		for (int ind = 0; ind < 3; ind++) {
			if (ind == *((int *)ScenarioVarPointers[_partColor_mode])) {
				sprintf(AuxString, "/partColor_mode_%d 1", ind); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
			}
			else {
				sprintf(AuxString, "/partColor_mode_%d 0", ind); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
			}
		}
		break;
	}
	case _partColor_mode_2: {
		*((int *)ScenarioVarPointers[_partColor_mode]) = 2;

		for (int ind = 0; ind < 3; ind++) {
			if (ind == *((int *)ScenarioVarPointers[_partColor_mode])) {
				sprintf(AuxString, "/partColor_mode_%d 1", ind); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
			}
			else {
				sprintf(AuxString, "/partColor_mode_%d 0", ind); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
			}
		}
		break;
	}
	case _partExit_mode_0: {
		*((int *)ScenarioVarPointers[_partExit_mode]) = 0;

		for (int ind = 0; ind < 3; ind++) {
			if (ind == *((int *)ScenarioVarPointers[_partExit_mode])) {
				sprintf(AuxString, "/partExit_mode_%d 1", ind); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
			}
			else {
				sprintf(AuxString, "/partExit_mode_%d 0", ind); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
			}
		}
		break;
	}
	case _partExit_mode_1: {
		*((int *)ScenarioVarPointers[_partExit_mode]) = 1;

		for (int ind = 0; ind < 3; ind++) {
			if (ind == *((int *)ScenarioVarPointers[_partExit_mode])) {
				sprintf(AuxString, "/partExit_mode_%d 1", ind); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
			}
			else {
				sprintf(AuxString, "/partExit_mode_%d 0", ind); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
			}
		}
		break;
	}
	case _partExit_mode_2: {
		*((int *)ScenarioVarPointers[_partExit_mode]) = 2;

		for (int ind = 0; ind < 3; ind++) {
			if (ind == *((int *)ScenarioVarPointers[_partExit_mode])) {
				sprintf(AuxString, "/partExit_mode_%d 1", ind); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
			}
			else {
				sprintf(AuxString, "/partExit_mode_%d 0", ind); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
			}
		}
		break;
	}

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// +++++++++++++++++ BLUR +++++++++++++++++++++++++++ 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	case _is_blur_1: {
		is_blur_1 = true;
		nb_blur_frames_1 = 1;
		break;
	}
	case _is_blur_1_plus: {
		is_blur_1 = true;
		nb_blur_frames_1 = 3;
		break;
	}
	case _is_blur_1_plus_plus: {
		is_blur_1 = true;
		nb_blur_frames_1 = 10;
		break;
	}
	case _is_blur_2: {
		is_blur_2 = true;
		nb_blur_frames_2 = 1;
		break;
	}


	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// +++++++++++++++++ CAMERA IMAGE CUMUL MODE +++++++++++++++ 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	case _cameraCumul_plus: {
		cameraCumul = (cameraCumul + 1) % PG_NB_VIDEO_CUMUL_MODES;
		BrokenInterpolationVar[_cameraCumul] = true;
		break;
	}

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// +++++++++++++++++ BRUSH ID SHIFT ++++++++++++++++++++++++ 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	case _pen_brush_plus: {
		if (nb_pen_brushes > 0) {
			pen_brush = (pen_brush + 1) % (nb_pen_brushes * 3);
			BrokenInterpolationVar[_pen_brush] = true;
			if (pen_brush >= 1) { // large radius for the image brushes
				pen_radiusMultiplier = 50.0f;
			}
			else {
				pen_radiusMultiplier = 1.0f;
			}

		}
		break;
	}

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// +++++++++++++++++ PIXEL MODE +++++++++++++++++++ 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	case _pixel_mode_plus: {
		pixel_mode = (pixel_mode + 1) % PG_NB_PIXEL_MODES;
		BrokenInterpolationVar[_pixel_mode] = true;
		break;
	}

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// +++++++++++++++++ CA TYPE AND SUBTYPE +++++++++++++++++++ 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	case _CA1Type_plus: {
		CA1Type = (CA1Type + 1) % PG_NB_CA_TYPES;
		BrokenInterpolationVar[_CA1Type] = true;
		// printf("CA1Type %d\n", CA1Type);
		*((int *)ScenarioVarPointers[_CA1Type]) = CA1Type;
		break;
	}
	case _CA1SubType_plus: {
		CA1SubType = (CA1SubType + 1) % PG_NB_CA_SUBTYPES;
		BrokenInterpolationVar[_CA1SubType] = true;
		*((int *)ScenarioVarPointers[_CA1SubType]) = CA1SubType;
		break;
	}
	case _CA1Type_minus: {
		CA1Type = (CA1Type - 1 + PG_NB_CA_TYPES) % PG_NB_CA_TYPES;
		BrokenInterpolationVar[_CA1Type] = true;
		// printf("CA1Type %d\n", CA1Type);
		*((int *)ScenarioVarPointers[_CA1Type]) = CA1Type;
		break;
	}
	case _CA1SubType_minus: {
		CA1SubType = (CA1SubType - 1 + PG_NB_CA_SUBTYPES) % PG_NB_CA_SUBTYPES;
		BrokenInterpolationVar[_CA1SubType] = true;
		*((int *)ScenarioVarPointers[_CA1SubType]) = CA1SubType;
		break;
	}
	case _CAonOff: {
		if (CA1SubType != 0) {
			CASubTypeMem = CA1SubType;
			CA1SubType = 0;
		}
		else {
			CA1SubType = CASubTypeMem;
		}
		BrokenInterpolationVar[_CA1SubType] = true;
		printf("CA1SubType ON/OFF %d\n", CA1SubType);
		*((int *)ScenarioVarPointers[_CA1SubType]) = CA1SubType;
		break;
	}

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// +++++++++++++++++ SENSOR LAYOUT AND SAMPLES +++++++++++++ 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
#ifdef PG_SENSORS
	case _sensor_layout_plus: {
		sensor_layout = (sensor_layout + 1) % PG_NB_MAX_SENSOR_LAYOUTS;
		BrokenInterpolationVar[_sensor_layout] = true;
		assignSensorPositions();
		break;
	}
	case _sample_setUp_plus: {
		sample_setUp = (sample_setUp + 1);
		if (sample_setUp >= PG_NB_MAX_SAMPLE_SETUPS) {
			sample_setUp -= PG_NB_MAX_SAMPLE_SETUPS;
		}
		BrokenInterpolationVar[_sample_setUp] = true;
		sample_setUp_interpolation();
		break;
	}
	case _sensor_activation_plus: {
		sensor_activation = (sensor_activation + 1) % PG_NB_MAX_SENSOR_ACTIVATIONS;
		BrokenInterpolationVar[_sensor_activation] = true;
		assignSensorActivations();
		break;
	}
#endif

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// +++++++++++++++++ CAMERA IMAGE CUMUL MODE +++++++++++++++ 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	case _partExit_mode_plus: {
		partExit_mode = (partExit_mode + 1) % PG_NB_PARTEXIT_MODES;
		BrokenInterpolationVar[_partExit_mode] = true;
		break;
	}
	case _partStroke_mode_plus: {
		partStroke_mode = (partStroke_mode + 1) % PG_NB_PARTSTROKE_MODES;
		BrokenInterpolationVar[_partStroke_mode] = true;
		break;
	}
	case _partColor_mode_plus: {
		partColor_mode = (partColor_mode + 1) % PG_NB_PARTCOLOR_MODES;
		BrokenInterpolationVar[_partColor_mode] = true;
		break;
	}

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// +++++++++++++++++ B/W SWITCH ++++++++++++++++++++++++++++ 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// ====================================== 
	case _pen_BW: {
		//	if (pen_grey > 0) {
		//		pen_grey = 0.0f;
		//	}								
		//	else {
		//		pen_grey = 1.0f;
		//	}
		//	BrokenInterpolationVar[_pen_grey] = true;
		//}
		break;
	}
	case _repop_BW: {
		if (repop_grey > 0) {
			repop_grey = 0.0f;
		}
		else {
			repop_grey = 1.0f;
		}
		BrokenInterpolationVar[_repop_grey] = true;
		// printf("repop color %.1f\n", repop_grey);
		break;
	}

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// +++++++++++++++++ PRESSURE SWITCH +++++++++++++++++++++++ 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// ====================================== 
	case _pressure_onOff: {
#define PG_PRESSURE_STANDARD 10.0f
		if (pen_radius_pressure_coef > 0) {
			pen_radius_pressure_coef = 0.0f;
		}
		else {
			pen_radius_pressure_coef = PG_PRESSURE_STANDARD;
		}
		BrokenInterpolationVar[_pen_radius_pressure_coef] = true;
		break;
	}

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// +++++++++++++++++ TRACK DECAY SWITCH ++++++++++++++++++++ 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// ====================================== 
	case _decay_onOff: {
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
		break;
	}

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// +++++++++++++++++ DIAPORAMA +++++++++++++++++++++++++++++ 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// ====================================== 
	case _diaporama_plus: {
		string fileName = "";
		for (int indPhoto = 0; indPhoto < PG_PHOTO_NB_TEXTURES; indPhoto++) {
			// incay and decay are 0 if is_capture_diaporama
			float incay = (is_capture_diaporama ? 0 : photo_diaporama_fade);
			float decay = (is_capture_diaporama ? 0 : photo_diaporama_fade);
			// blended photo is terminated to switch to the next one
			if (pg_Photo_swap_buffer_data[indPhoto].blendStart > 0.0f) {
				printf("load diaporama_minus stops blending layer #%d\n", indPhoto);
				int nextCompressedImage
					= nextFileIndexMemoryLoop(pg_CurrentDiaporamaDir,
						&pg_CurrentDiaporamaFile, ascendingDiaporama);
				pg_Photo_swap_buffer_data[(indPhoto + 1) % PG_PHOTO_NB_TEXTURES].indSwappedPhoto
					= nextCompressedImage;
				pg_Photo_swap_buffer_data[indPhoto].blendStart
					= CurrentClockTime - (incay + photo_diaporama_plateau + decay);
				return;
			}
		}
		break;
	}
	case _diaporama_minus: {
		string fileName = "";
		for (int indPhoto = 0; indPhoto < PG_PHOTO_NB_TEXTURES; indPhoto++) {
			// incay and decay are 0 if is_capture_diaporama
			float incay = (is_capture_diaporama ? 0 : photo_diaporama_fade);
			float decay = (is_capture_diaporama ? 0 : photo_diaporama_fade);
			// blended photo is terminated to switch to the next one
			if (pg_Photo_swap_buffer_data[indPhoto].blendStart > 0.0f) {
				// change order (was decreasing before)
				// stops blending
				printf("load diaporama_minus stops blending layer #%d\n", indPhoto);
				int nextCompressedImage
					= nextFileIndexMemoryLoop(pg_CurrentDiaporamaDir,
						&pg_CurrentDiaporamaFile, !ascendingDiaporama);
				pg_Photo_swap_buffer_data[(indPhoto + 1) % PG_PHOTO_NB_TEXTURES].indSwappedPhoto
					= nextCompressedImage;
				pg_Photo_swap_buffer_data[indPhoto].blendStart
					= CurrentClockTime - (incay + photo_diaporama_plateau + decay);
				return;
			}
		}
		break;
	}

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// +++++++++++++++++ TRACK NO ++++++++++++++++++++++++++++++ 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// ====================================== 
	case _soundtrack_plus: {
		if (nb_soundtracks > 0) {
			if (currentlyPlaying_trackNo >= 0) {
				currentlyPlaying_trackNo = (currentlyPlaying_trackNo + 1) % nb_soundtracks;
			}
			else {
				currentlyPlaying_trackNo = 0;
			}
			PlayTrack(currentlyPlaying_trackNo);
			BrokenInterpolationVar[_playing_soundtrackNo] = true;
			*((int *)ScenarioVarPointers[_playing_soundtrackNo]) = currentlyPlaying_trackNo;
		}
		break;
	}
	case _soundtrack_onOff: {
		soundTrack_on = !soundTrack_on;
		sprintf(AuxString, "/soundtrack_onOff %d", soundTrack_on);
		pg_send_message_udp((char *)"i", AuxString, (char *)"udp_PD_send");
		sprintf(AuxString, "/soundtrack_onOff %d", !soundTrack_on);
		pg_send_message_udp((char *)"i", AuxString, (char *)"udp_QT_send");
		break;
	}

	case _movie_onOff: {
		movie_on = !movie_on;
		sprintf(AuxString, "/movie_onOff %d", !movie_on);
		pg_send_message_udp((char *)"i", AuxString, (char *)"udp_QT_send");
		break;
	}

							// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// +++++++++++++++++ PALETTE NO ++++++++++++++++++++++++++++ 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// ====================================== 
	case _pen_colorPreset_minus: {
		if (nb_pen_colorPresets > 0) {
			current_pen_colorPreset = (current_pen_colorPreset - 1 + nb_pen_colorPresets) % nb_pen_colorPresets;
			// printf( "/new palette %d\n", current_pen_colorPreset);
			BrokenInterpolationVar[_pen_color] = true;
			*((float *)ScenarioVarPointers[_pen_color]) = pen_colorPreset_values[current_pen_colorPreset];
			// sprintf(AuxString, "/pen_color %.3f", pen_colorPresets_values[current_pen_colorPreset]);
			// pg_send_message_udp((char *)"s", AuxString, (char *)"udp_QT_send");
			// sprintf(AuxString, "/message palette_%s", pen_colorPresets_names[current_pen_colorPreset].c_str()); pg_send_message_udp((char *)"s", (char *)AuxString, (char *)"udp_QT_send");
			sprintf(AuxString, "/pen_colorPreset %d", current_pen_colorPreset); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
		}

		break;
	}
	case _pen_colorPreset_plus: {
		if (nb_pen_colorPresets > 0) {
			current_pen_colorPreset = (current_pen_colorPreset + 1) % nb_pen_colorPresets;
			BrokenInterpolationVar[_pen_color] = true;
			*((float *)ScenarioVarPointers[_pen_color]) = pen_colorPreset_values[current_pen_colorPreset];
			// sprintf(AuxString, "/pen_color %.3f", pen_colorPresets_values[current_pen_colorPreset]);
			// pg_send_message_udp((char *)"s", AuxString, (char *)"udp_QT_send");
			// sprintf(AuxString, "/message palette_%s", pen_colorPresets_names[current_pen_colorPreset].c_str()); pg_send_message_udp((char *)"s", (char *)AuxString, (char *)"udp_QT_send");
			sprintf(AuxString, "/pen_colorPreset %d", current_pen_colorPreset); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
		}
		break;
	}
	case _pen_colorPreset: {
		if (nb_pen_colorPresets > 0) {
			current_pen_colorPreset = int(arguments[0]) % nb_pen_colorPresets;
			BrokenInterpolationVar[_pen_color] = true;
			*((float *)ScenarioVarPointers[_pen_color]) = pen_colorPreset_values[current_pen_colorPreset];
			// sprintf(AuxString, "/pen_color %.3f", pen_colorPresets_values[current_pen_colorPreset]);
			// pg_send_message_udp((char *)"s", AuxString, (char *)"udp_QT_send");
			// sprintf(AuxString, "/message palette_%s", pen_colorPresets_names[current_pen_colorPreset].c_str()); pg_send_message_udp((char *)"s", (char *)AuxString, (char *)"udp_QT_send");
			// printf("************* palette %d\n", current_pen_colorPreset);
			sprintf(AuxString, "/pen_colorPreset %d", current_pen_colorPreset); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
			printf("sent: %s\n", AuxString);
		}
		break;
	}
#ifdef MALAUSSENA
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
	// +++++++++++++++++ CA SEEDING ++++++++++++++++++++++++++++
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// ====================================== 

	case _CAseed_dot_center: {
		pg_CAseed_type = _pg_CAseed_dot_center;
		sprintf(AuxString, "/CAseed_dot_center %d", 1); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
		sprintf(AuxString, "/CAseed_dot %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
		sprintf(AuxString, "/CAseed_h_line %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
		sprintf(AuxString, "/CAseed_v_line %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
		sprintf(AuxString, "/CAseed_cross %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
		sprintf(AuxString, "/CAseed_X %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
		sprintf(AuxString, "/CAseed_square %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
		break;
	}
	case _CAseed_dot: {
		pg_CAseed_type = _pg_CAseed_dot;
		sprintf(AuxString, "/CAseed_dot_center %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
		sprintf(AuxString, "/CAseed_dot %d", 1); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
		sprintf(AuxString, "/CAseed_h_line %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
		sprintf(AuxString, "/CAseed_v_line %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
		sprintf(AuxString, "/CAseed_cross %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
		sprintf(AuxString, "/CAseed_X %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
		sprintf(AuxString, "/CAseed_square %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
		break;
	}
	case _CAseed_h_line: {
		sprintf(AuxString, "/CAseed_dot_center %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
		sprintf(AuxString, "/CAseed_dot %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
		sprintf(AuxString, "/CAseed_h_line %d", 1); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
		sprintf(AuxString, "/CAseed_v_line %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
		sprintf(AuxString, "/CAseed_cross %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
		sprintf(AuxString, "/CAseed_X %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
		sprintf(AuxString, "/CAseed_square %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
		pg_CAseed_type = _pg_CAseed_h_line;
		break;
	}
	case _CAseed_v_line: {
		sprintf(AuxString, "/CAseed_dot_center %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
		sprintf(AuxString, "/CAseed_dot %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
		sprintf(AuxString, "/CAseed_h_line %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
		sprintf(AuxString, "/CAseed_v_line %d", 1); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
		sprintf(AuxString, "/CAseed_cross %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
		sprintf(AuxString, "/CAseed_X %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
		sprintf(AuxString, "/CAseed_square %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
		pg_CAseed_type = _pg_CAseed_v_line;
		break;
	}
	case _CAseed_cross: {
		sprintf(AuxString, "/CAseed_dot_center %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
		sprintf(AuxString, "/CAseed_dot %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
		sprintf(AuxString, "/CAseed_h_line %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
		sprintf(AuxString, "/CAseed_v_line %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
		sprintf(AuxString, "/CAseed_cross %d", 1); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
		sprintf(AuxString, "/CAseed_X %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
		sprintf(AuxString, "/CAseed_square %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
		pg_CAseed_type = _pg_CAseed_cross;
		break;
	}
	case _CAseed_X: {
		sprintf(AuxString, "/CAseed_dot_center %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
		sprintf(AuxString, "/CAseed_dot %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
		sprintf(AuxString, "/CAseed_h_line %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
		sprintf(AuxString, "/CAseed_v_line %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
		sprintf(AuxString, "/CAseed_cross %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
		sprintf(AuxString, "/CAseed_X %d", 1); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
		sprintf(AuxString, "/CAseed_square %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
		pg_CAseed_type = _pg_CAseed_X;
		break;
	}
	case _CAseed_square: {
		sprintf(AuxString, "/CAseed_dot_center %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
		sprintf(AuxString, "/CAseed_dot %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
		sprintf(AuxString, "/CAseed_h_line %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
		sprintf(AuxString, "/CAseed_v_line %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
		sprintf(AuxString, "/CAseed_cross %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
		sprintf(AuxString, "/CAseed_X %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
		sprintf(AuxString, "/CAseed_square %d", 1); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
		pg_CAseed_type = _pg_CAseed_square;
		break;
	}
	case _CAseed_size: {
		pg_CAseed_size = int(arguments[0]);
		sprintf(AuxString, "/CAseed_size %d", pg_CAseed_size); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
		break;
	}
	case _CAseed_loc: {
		pg_CAseed_location = pg_CAseed_locations(int(arguments[0]) % _pg_Nb_CAseed_locations);
		pg_CAseed_location_to_coordinates(pg_CAseed_location, pg_CAseed_coordinates);
		sprintf(AuxString, "/CAseed_loc %d", pg_CAseed_location); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
		break;
	}
	case _CAseed_loc_plus: {
		pg_CAseed_location = pg_CAseed_locations((pg_CAseed_location + 1) % _pg_Nb_CAseed_locations);
		pg_CAseed_location_to_coordinates(pg_CAseed_location, pg_CAseed_coordinates);
		sprintf(AuxString, "/CAseed_loc %d", pg_CAseed_location); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
		break;
	}
	case _CAseed_trigger: {
		pg_CAseed_trigger = true;
		break;
	}
#endif

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
	// +++++++++++++++++ MOVIE NO ++++++++++++++++++++++++++++++ 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// ====================================== 
	case _movie_plus: {
		if (nb_movies > 0) {
			playing_movieNo = (playing_movieNo + 1) % nb_movies;
			sprintf(AuxString, "/movie_shortName %s", movieShortName[playing_movieNo].c_str());
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
				// sprintf(AuxString, "/message %s", movieFileName[currentlyPlaying_movieNo].c_str()); pg_send_message_udp((char *)"s", (char *)AuxString, (char *)"udp_QT_send");
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
		break;
	}

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// +++++++++++++++++ SET-UP ++++++++++++++++++++++++++++++++ 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// ====================================== 
	case _resend_all: {
		resend_all_variables = true;
		break;
	}
	case _setup_plus: {
		setup_plus();
		break;
	}
	case _setup_minus: {
		setup_minus();
		break;
	}
	case _setup_plus_keep_total_dur: {
		float elapsedTime = (CurrentClockTime - InitialScenarioTime) - Scenario[pg_CurrentScene].scene_initial_time;
		float deltaTime = Scenario[1 + pg_CurrentScene].scene_initial_time - (CurrentClockTime - InitialScenarioTime);
		// only accepted if the current scene has been on for a while
		if (elapsedTime > 15
			// and if the scene has not begun since a short while
			&& deltaTime > 0
			// and if the scene is not too far from being finished
			&& deltaTime < 60) {
			int new_scene = ((1 + pg_CurrentScene) % pg_NbScenes);

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
		break;
	}
	default: {
		sprintf(ErrorStr, "Command not found (%s)!", command_symbol); ReportError(ErrorStr);
		break;
	}
	}
}

//////////////////////////////////////////////////////////////
// PULSE COLOR FUNCTIONS
//////////////////////////////////////////////////////////////

void update_pulsed_colors(void) {
	////////////////////////////////////////////////////////////////////////////////////////////////////
	// PEN PULSED COLOR
	float lowPenPalette = floor(pen_color * nb_pen_palette_colors);
	int indLowPenPalette
		= int(lowPenPalette) % nb_pen_palette_colors;
	int indUpperPenPalette
		= (indLowPenPalette + 1) % nb_pen_palette_colors;

	// blending the two closest palettes
	float percentage = pen_color * nb_pen_palette_colors - lowPenPalette;
	// the base palette is obtained by interpolating the two closest palettes according to pen_color
	for (int ind = 0; ind < 9; ind++) {
		pen_base_3color_palette[ind] = (1.f - percentage) * pen_palette_colors_values[indLowPenPalette][ind]
			+ percentage * pen_palette_colors_values[indUpperPenPalette][ind];
		pen_base_3color_palette[ind] = min(1.f, pen_base_3color_palette[ind]);
	}
	//printf("pen color %.2f palette low/upper/perc %d/%d/%.2f -> colors %.1f %.1f %.1f       %.1f %.1f %.1f        %.1f %.1f %.1f\n", 
	//	pen_color, indLowPenPalette, indUpperPenPalette, percentage, pen_base_3color_palette[0]*255.f,
	//	pen_base_3color_palette[1]*255.f, pen_base_3color_palette[2]*255.f, pen_base_3color_palette[3]*255.f, pen_base_3color_palette[4]*255.f, pen_base_3color_palette[5]*255.f,
	//	pen_base_3color_palette[6]*255.f, pen_base_3color_palette[7]*255.f, pen_base_3color_palette[8]*255.f);
	// calculating the pulsed color from base luminance + palette colors modulated by the three frequence ranges
	for (int indChannel = 0; indChannel < 3; indChannel++) {
		// adding a base luminance
		pulsed_pen_color[indChannel] = pen_grey;
		for (int indPulse = 0; indPulse < 3; indPulse++) {
			if (pen_color_pulse >= 0) {
				pulsed_pen_color[indChannel]
					+= pen_base_3color_palette[indPulse * 3 + indChannel] * pulse[indPulse] * pen_color_pulse;
			}
			else {
				pulsed_pen_color[indChannel]
					+= pen_base_3color_palette[indPulse * 3 + indChannel] * pulse[indPulse];
			}
		}
		pulsed_pen_color[indChannel] = min(1.f, pulsed_pen_color[indChannel]);
	}
	pulsed_pen_color[3] = 1.f;
	float value = (pulsed_pen_color[0] < pulsed_pen_color[1]) ? pulsed_pen_color[1] : pulsed_pen_color[0];
	value = (value < pulsed_pen_color[2]) ? pulsed_pen_color[2] : value;
    //printf( "pulsed_pen_color: %.2f %.2f %.2f    pulse: %.2f %.2f %.2f\n" , pulsed_pen_color[0] , pulsed_pen_color[1] , pulsed_pen_color[2] , pulse[0], pulse[1], pulse[2]);

	// pen_color_pulse == -1 => saturation of the pulsed color
	if (pen_color_pulse < 0 && value > 0) {
		//printf("color saturation\n");
		for (int indChannel = 0; indChannel < 3; indChannel++) {
			pulsed_pen_color[indChannel] /= value;
		}
	}

	// the colors for drawing are inverted in case of inverted rendering, so that the drawing colors are not seen inverted
	if (invertAllLayers) {
		pulsed_pen_color[0] = value - pulsed_pen_color[0];
		pulsed_pen_color[1] = value - pulsed_pen_color[1];
		pulsed_pen_color[2] = value - pulsed_pen_color[2];
	}
	/***************************** SHOULD BE REACTIVATED WHEN THE INTERFACE CAN DISPLAY COLORS 
	sprintf(AuxString, "/pen_colorPenPalette %.5f %.5f %.5f %.5f %.5f %.5f %.5f %.5f %.5f",
		pen_base_3color_palette[0], pen_base_3color_palette[1], pen_base_3color_palette[2], pen_base_3color_palette[3], pen_base_3color_palette[4], pen_base_3color_palette[5], pen_base_3color_palette[6], pen_base_3color_palette[7], pen_base_3color_palette[8]);
	pg_send_message_udp((char *)"f f f f f f f f f", AuxString, (char *)"udp_QT_send");
	sprintf(AuxString, "/pulsed_pen_color %.5f %.5f %.5f",
		pulsed_pen_color[0], pulsed_pen_color[1], pulsed_pen_color[2]);
	pg_send_message_udp((char *)"f f f", AuxString, (char *)"udp_QT_send");
	*/

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// REPOP PULSED COLOR
	float lowRepopPalette = floor(repop_color * nb_pen_palette_colors);
	int indLowRepopPalette
		= int(lowRepopPalette) % nb_pen_palette_colors;
	int indUpperRepopPalette
		= (indLowRepopPalette + 1) % nb_pen_palette_colors;

	// blending the two closest palettes
	percentage = repop_color * nb_pen_palette_colors - lowRepopPalette;
	// the base palette is obtained by interpolating the two closest palettes according to repop_color
	for (int ind = 0; ind < 9; ind++) {
		repop_base_3color_palette[ind] = (1.f - percentage) * pen_palette_colors_values[indLowRepopPalette][ind]
			+ percentage * pen_palette_colors_values[indUpperRepopPalette][ind];
		repop_base_3color_palette[ind] = min(1.f, repop_base_3color_palette[ind]);
	}

	// calculating the pulsed color from base luminance + palette colors modulated by the three frequence ranges
	for (int indChannel = 0; indChannel < 3; indChannel++) {
		// adding a base luminance
		pulsed_repop_color[indChannel] = repop_grey;
		// we use Low/Medium/high frequency levels to blend the three colors of the palette
		for (int indPulse = 0; indPulse < 3; indPulse++) {
			if (repop_color_pulse >= 0) {
				pulsed_repop_color[indChannel]
					+= repop_base_3color_palette[indPulse * 3 + indChannel] * pulse[indPulse] * repop_color_pulse;
			}
			else {
				pulsed_repop_color[indChannel]
					+= repop_base_3color_palette[indPulse * 3 + indChannel] * pulse[indPulse];
			}
		}
		pulsed_repop_color[indChannel] = min(1.f, pulsed_repop_color[indChannel]);
	}
	value = (pulsed_repop_color[0] < pulsed_repop_color[1]) ? pulsed_repop_color[1] : pulsed_repop_color[0];
	value = (value < pulsed_repop_color[2]) ? pulsed_repop_color[2] : value;
	// printf( "pulsed_repop_color: %f %f %f\n" , pulsed_repop_color[0] , pulsed_repop_color[1] , pulsed_repop_color[2] );

	// repop_color_pulse == -1 => saturation of the pulsed color
	if (repop_color_pulse < 0 && value > 0) {
		for (int indChannel = 0; indChannel < 3; indChannel++) {
			pulsed_repop_color[indChannel] /= value;
		}
	}

	// the colors for drawing are inverted in case of inverted rendering, so that the drawing colors are not seen inverted
	if (invertAllLayers) {
		pulsed_repop_color[0] = value - pulsed_repop_color[0];
		pulsed_repop_color[1] = value - pulsed_repop_color[1];
		pulsed_repop_color[2] = value - pulsed_repop_color[2];
	}
	/***************************** SHOULD BE REACTIVATED WHEN THE INTERFACE CAN DISPLAY COLORS
	sprintf(AuxString, "/repop_colorRepopPalette %.5f %.5f %.5f %.5f %.5f %.5f %.5f %.5f %.5f",
		pen_base_3color_palette[0], pen_base_3color_palette[1], pen_base_3color_palette[2], pen_base_3color_palette[3], pen_base_3color_palette[4], pen_base_3color_palette[5], pen_base_3color_palette[6], pen_base_3color_palette[7], pen_base_3color_palette[8]);
	pg_send_message_udp((char *)"f f f f f f f f f", AuxString, (char *)"udp_QT_send");
	sprintf(AuxString, "/pulsed_repop_color %.5f %.5f %.5f",
		pulsed_repop_color[0], pulsed_repop_color[1], pulsed_repop_color[2]);
	pg_send_message_udp((char *)"f f f", AuxString, (char *)"udp_QT_send");
	*/
}

//////////////////////////////////////////////////////////////
// TRACK LIB FUNCTIONS
//////////////////////////////////////////////////////////////

void pg_path_recording_onOff( int indPath ) {
  // switches between recording on/off 
  bool is_path_record = false;
  switch (indPath) {
#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7
  case 1:
	  is_path_record = !path_record_1;
	  *((bool *)ScenarioVarPointers[_path_record_1]) = is_path_record;
	  break;
  case 2:
	  is_path_record = !path_record_2;
	  *((bool *)ScenarioVarPointers[_path_record_2]) = is_path_record;
	  break;
  case 3:
	  is_path_record = !path_record_3;
	  *((bool *)ScenarioVarPointers[_path_record_3]) = is_path_record;
	  break;
#endif
#if PG_NB_PATHS == 7
  case 4:
	  is_path_record = !path_record_4;
	  *((bool *)ScenarioVarPointers[_path_record_4]) = is_path_record;
	  break;
  case 5:
	  is_path_record = !path_record_5;
	  *((bool *)ScenarioVarPointers[_path_record_5]) = is_path_record;
	  break;
  case 6:
	  is_path_record = !path_record_6;
	  *((bool *)ScenarioVarPointers[_path_record_6]) = is_path_record;
	  break;
  case 7:
	  is_path_record = !path_record_7;
	  *((bool *)ScenarioVarPointers[_path_record_7]) = is_path_record;
	  break;
#endif
  }

  // starts recording source 
  if( is_path_record ) {
	// is reading -> has to stop reading 
	// printf("Replay indPath %d: %d\n", indPath, is_path_replay[indPath]);
	if( is_path_replay[indPath]) {
		// printf("Stops Replay indPath %d: %d\n", indPath, is_path_replay[indPath]);
		pg_path_replay_trackNo_stop(indPath);
	}

	// launches source recording 
	pg_path_recording_start(indPath);
	sprintf(AuxString, "/path_record_%d 1", indPath);
	pg_send_message_udp((char *)"i", AuxString, (char *)"udp_QT_send");
  } else {
	// stops source recording 
	pg_path_recording_stop(indPath);
	sprintf(AuxString, "/path_record_%d 0", indPath);
	pg_send_message_udp((char *)"i", AuxString, (char *)"udp_QT_send");
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
		*((int *)ScenarioVarPointers[_path_replay_trackNo_1]) = -1;
		break;
	case 2:
		*((int *)ScenarioVarPointers[_path_replay_trackNo_2]) = -1;
		break;
	case 3:
		*((int *)ScenarioVarPointers[_path_replay_trackNo_3]) = -1;
		break;
#endif
#if PG_NB_PATHS == 7
	case 4:
		*((int *)ScenarioVarPointers[_path_replay_trackNo_4]) = -1;
		break;
	case 5:
		*((int *)ScenarioVarPointers[_path_replay_trackNo_5]) = -1;
		break;
	case 6:
		*((int *)ScenarioVarPointers[_path_replay_trackNo_6]) = -1;
		break;
	case 7:
		*((int *)ScenarioVarPointers[_path_replay_trackNo_7]) = -1;
		break;
#endif
	}

  }
}

void pg_path_recording_start(int indPath) {
	switch (indPath) {
#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7
	case 1:
		*((bool *)ScenarioVarPointers[_path_record_1]) = true;
		break;
	case 2:
		*((bool *)ScenarioVarPointers[_path_record_2]) = true;
		break;
	case 3:
		*((bool *)ScenarioVarPointers[_path_record_3]) = true;
		break;
#endif
#if PG_NB_PATHS == 7
	case 4:
		*((bool *)ScenarioVarPointers[_path_record_4]) = true;
		break;
	case 5:
		*((bool *)ScenarioVarPointers[_path_record_5]) = true;
		break;
	case 6:
		*((bool *)ScenarioVarPointers[_path_record_6]) = true;
		break;
	case 7:
		*((bool *)ScenarioVarPointers[_path_record_7]) = true;
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
		*((bool *)ScenarioVarPointers[_path_record_1]) = false;
		break;
	case 2:
		*((bool *)ScenarioVarPointers[_path_record_2]) = false;
		break;
	case 3:
		*((bool *)ScenarioVarPointers[_path_record_3]) = false;
		break;
#endif
#if PG_NB_PATHS == 7
	case 4:
		*((bool *)ScenarioVarPointers[_path_record_4]) = false;
		break;
	case 5:
		*((bool *)ScenarioVarPointers[_path_record_5]) = false;
		break;
	case 6:
		*((bool *)ScenarioVarPointers[_path_record_6]) = false;
		break;
	case 7:
		*((bool *)ScenarioVarPointers[_path_record_7]) = false;
		break;
#endif
	}
	if (indPath >= 1
		&& indPath <= PG_NB_PATHS
		&& pg_Path_Status[indPath].isActiveRecording == true) {
		pg_Path_Status[indPath].isActiveRecording = false;
		// printf("start playing %d\n",indPath);
		sprintf(AuxString, "/path_record_%d 0", indPath);
		pg_send_message_udp((char *)"i", AuxString, (char *)"udp_QT_send");
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
			*((int *)ScenarioVarPointers[_path_replay_trackNo_1]) = trackNo;
			break;
		case 2:
			*((int *)ScenarioVarPointers[_path_replay_trackNo_2]) = trackNo;
			break;
		case 3:
			*((int *)ScenarioVarPointers[_path_replay_trackNo_3]) = trackNo;
			break;
#endif
#if PG_NB_PATHS == 7
		case 4:
			*((int *)ScenarioVarPointers[_path_replay_trackNo_4]) = trackNo;
			break;
		case 5:
			*((int *)ScenarioVarPointers[_path_replay_trackNo_5]) = trackNo;
			break;
		case 6:
			*((int *)ScenarioVarPointers[_path_replay_trackNo_6]) = trackNo;
			break;
		case 7:
			*((int *)ScenarioVarPointers[_path_replay_trackNo_7]) = trackNo;
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
	if (indPath >= 1
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
		switch (indPath) {
#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7
		case 1:
			*((int *)ScenarioVarPointers[_path_replay_trackNo_1]) = -1;
			break;
		case 2:
			*((int *)ScenarioVarPointers[_path_replay_trackNo_2]) = -1;
			break;
		case 3:
			*((int *)ScenarioVarPointers[_path_replay_trackNo_3]) = -1;
			break;
#endif
#if PG_NB_PATHS == 7
		case 4:
			*((int *)ScenarioVarPointers[_path_replay_trackNo_4]) = -1;
			break;
		case 5:
			*((int *)ScenarioVarPointers[_path_replay_trackNo_5]) = -1;
			break;
		case 6:
			*((int *)ScenarioVarPointers[_path_replay_trackNo_6]) = -1;
			break;
		case 7:
			*((int *)ScenarioVarPointers[_path_replay_trackNo_7]) = -1;
			break;
#endif
		}
		sprintf(AuxString, "/path_replay_trackNo_%d 0", indPath);
		pg_send_message_udp((char *)"i", AuxString, (char *)"udp_QT_send");
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
  float newtime = RealTime(); 
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

