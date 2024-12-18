/*! \file pg-script.h
 *
 *
 *
 * 
 *     File pg-script.h
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
#ifndef PG_SCRIPT_H
#define PG_SCRIPT_H

/////////////////////////////////////////////////////////////////////////
// WORKING CONSTANTS
/////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
// size

// number of ClipArt tracks 
#define nb_ClipArt_tracks 2

/////////////////////////////////////////////////////////////////////////
// initial values

// ++++++++++++++++++++++ MUSIC CONTROL ++++++++++++++++++++ 
#define RGBpalette_r_ini 0.0
#define RGBpalette_g_ini 0.0
#define RGBpalette_b_ini 0.0
#define RGBpalette_a_ini 0.0
#define particle_acc_attack_ini 0.0

// per track replay interpolation 
#define track_interp_ini false
#define track_stepwise_interpolation_ini true

// track loops 
#define PG_LOOP_SIZE 8  // standrd 8 beats/loop size

enum pg_Parameter_Input_Type { _PG_KEYSTROKE = 0, _PG_GUI_COMMAND, _PG_SCENARIO, _PG_INPUT_VOID };
enum pg_Keystroke_Input_Type {
	_PG_KEYSTROKE_MINUS = 0, _PG_KEYSTROKE_PLUS, _PG_KEYSTROKE_VOID
};



//////////////////////////////////////////////
// SCENARIO AND CONFIGURATION VARIABLES
// changed to true when a value is changed manually during scenario
extern bool BrokenInterpolationVar[_MaxInterpVarIDs];
// stepwise interpolation made only once
extern bool StepwiseInterpolationEffective[_MaxInterpVarIDs];
// initial values in the scenario (before first scene)
class ScenarioValue {
public:
	double val_num;
	string val_string;
	ScenarioValue() { val_num = 0; val_string = ""; }
	ScenarioValue(double d, string s) { val_num = d; val_string = s; }
	~ScenarioValue() {}
};
// initial values in the scenario (before first scene)
extern ScenarioValue InitialValuesInterpVar[_MaxInterpVarIDs][_NbConfigurations];
// rank of the scenario variable in the list of variables generated by python
extern int ConfigScenarioVarRank[_MaxInterpVarIDs][_NbConfigurations];
// var IDs in scenarios in the order in which they appear in the scenario
extern string ConfigScenarioVarIDs[_MaxInterpVarIDs][_NbConfigurations];
// number of vars in the scenario file
extern int ScenarioVarNb[_NbConfigurations];
// last value shipped to the GUI (PD)
extern float LastGUIShippedValuesInterpVar[_MaxInterpVarIDs][_NbConfigurations];
// initial values in the configuration file
//extern float InitialValuesConfigurationVar[_MaxConfigurationVarIDs];

//////////////////////////////////////////////
// MIDI event creates a local stroke
extern int FourFrameStrokeNb;
extern int FourFrameStroke_x;
extern int FourFrameStroke_y;

//////////////////////////////////////////////
// Cuurent palette after interpolation
extern float pen_bandpass_3color_palette[3][3];

#if defined(CORE)
#define CA_SQUENCR                0
#define CA_TOTALISTIC             1
#define CA_GENERATION             2
#define CA_GAL_BIN_MOORE          3
#define CA_GAL_BIN_NEUMANN        4
#define CA_NEUMANN_BINARY         5
#endif

#if (defined (RIVETS) || defined (CAAUDIO)) && !defined(SOUNDINITATIVE)
#define CA_TOTALISTIC             0
#define CA_GENERATION             1
#define CA_GAL_BIN_MOORE          2
#define CA_GAL_BIN_NEUMANN        3
#define CA_NEUMANN_BINARY         4
#endif

#if defined(var_partExit_mode) 
#define PG_NB_PARTEXIT_MODES 3
#endif
#if defined(var_partStroke_mode) 
#define PG_NB_PARTSTROKE_MODES 4
#endif
#if defined(var_partColor_mode) 
#define PG_NB_PARTCOLOR_MODES 3
#endif

#if defined(var_GenerativeNights_planes)
//////////////////////////////////////////////
/// image number of first plane
extern int      firstPlaneFrameNo;

//////////////////////////////////////////////
// CA initialization
extern float initCA;
#endif

// +++++++++++++++++++++++ scripts for external control through python programming +++++
#if defined(var_script_1)
// currently running script: NULLL if none
extern string currently_runnings_script_1;
extern STARTUPINFOA si_script_1;
extern PROCESS_INFORMATION pi_script_1;
#endif


/////////////////////////////////////////////////////////////////////////
// WORKING VARIABLES
/////////////////////////////////////////////////////////////////////////

// ++++++++++++++++++++++ GLUT MODIFIER ++++++++++++++++++++ 
extern int pg_key_modifier;

extern std::string project_name;

// ++++++++++++++++++++++ SCENARIO +++++++++++++++++++++++++ 
extern int pg_CurrentSceneIndex;
extern float pg_setup_interpolation_duration;
class Scene;
extern Scene* pg_CurrentScene;

// +++++++++++++++++++++ CLEAR +++++++++++++++++++++++++++
extern float isClearCA;
extern float isClearLayer;
extern float isClearAllLayers;
extern float isClearEcho;

// DELAYED CAMERA WEIGHT
extern int delayedCameraWeight;

#ifdef PG_WITH_BLUR
// +++++++++++++++++++++ BLUR +++++++++++++++++++++++++++
extern bool is_blur_1;
extern bool is_blur_2;
extern int nb_blur_frames_1;
extern int nb_blur_frames_2;
#endif

// +++++++++++++++++++++ COPY LAYERS +++++++++++++++++++++++++++
// copy to layer above (+1) or to layer below (-1)
extern int copyToNextTrack;

// ++++++++++++++++++++++ MUSIC CONTROL ++++++++++++++++++++ 
// music control inputs received at each frame
// addition of color based on palette for pen
extern float pulsed_pen_color[4];
// addition of color based on palette for particles
extern float pulsed_repop_colorBG[3];
extern float pulsed_repop_colorCA[3];
extern float pulsed_repop_colorPart[3];

// factor increasing the acceleration weight of particles
// based on sound volume attacks
// brush radius
// particle radius
extern float pulse_average;
extern float pulse_average_prec;
// not used currently extern float pulse_attack;

// sound track playing
extern int currentlyPlaying_trackNo;
extern bool soundTrack_on;

// movie soundtrack passes over an onset or a peak before next frame
extern bool pg_track_sound_onset;
extern bool pg_track_sound_peak;

// movie playing
extern int currentlyPlaying_movieNo;
extern float current_extern_movieNo;

// movie soundtrack passes over an onset or a peak before next frame
extern bool pg_movie_sound_onset;
extern bool pg_movie_sound_peak;

// pen preset
extern int current_pen_colorPreset;

// +++++++++++++++++++++++ Beats +++++++++++++++++++++++++++++++++
extern float pulse[3];
extern float seed_pulsePerlinNoise[4 * 2];

// ++++++++++++++++++++++ FLASHES +++++++++++++++++++++++++
// flash: Trk->CA  
extern float flashTrkCA_weights[PG_NB_TRACKS];
extern int flashTrkCA_weights_duration[PG_NB_TRACKS];
// flash: Trk->Part
extern float flashTrkPart_weights[PG_NB_TRACKS];
extern int flashTrkPart_weights_duration[PG_NB_TRACKS];
// flash: CA->BG
extern float flashCABG_weight;
extern int flashCABG_weight_duration;
// flash: CA->Part
extern float flashCAPart_weight;
extern int flashCAPart_weight_duration;
// flash: Part->BG
extern float flashPartBG_weight;
extern int flashPartBG_weight_duration;
// flash: Part->CA
extern float flashPartCA_weight;
extern int flashPartCA_weight_duration;
// flash: extension of pixel reproduction  
extern int flashPixel;
// one-frame master flashing  
extern int flashMaster;
// flash: Trk->BG  
extern float flashTrkBG_weights[PG_NB_TRACKS];
extern int flashTrkBG_weights_duration[PG_NB_TRACKS];

// +++++++++++++++++++++ VIDEO FLASH +++++++++++++++++++++
extern bool is_flashCameraTrk;
extern float flashCameraTrk_weight;
extern float flashCameraTrk_decay;
extern float flashCameraTrk_threshold;

// +++++++++++++++++++++ PHOTO FLASH +++++++++++++++++++++
#if defined(var_flashPhotoTrkBeat) && defined(var_flashPhotoTrkBright) && defined(var_flashPhotoTrkLength) && defined(var_flashPhotoChangeBeat)
extern bool is_flashPhotoTrk;
extern float flashPhotoTrk_weight;
extern float flashPhotoTrk_decay;
extern float flashPhotoTrk_threshold;
extern int flashPhotoTrk_nbFrames;
#endif

// ++++++++++++++++++++++ CA and TACKS WOKING VARIABLE ++++
// CA and track working variable

// +++++++++++++++++++++++ FFT levels and frequency storage ++++++++++++++++++++
#ifdef CRITON
extern float fftLevels[8];
extern float fftFrequencies[8];
extern float fftPhases[8];
#endif

// scene management
extern bool pg_FirstFrameInScene;
extern double remainingTimeInScene;

// interpolation scene between two discontinuous scenes so that there is no visual gap inbetween
extern int pg_SceneIndexAfterInterpolation;
extern int pg_SceneIndexBeforeInterpolation;
extern float pg_SceneInterpolationDuration;
class Scene;
extern Scene pg_InterpolationScene;
extern bool* pg_variable_updated;
extern pg_Parameter_Input_Type* pg_variable_param_input_type;
extern ScenarioValue* pg_variable_scenario_or_gui_command_value;
bool double_to_bool(double param);
bool double_to_path(double param);
int double_to_sign(double param);

// master automatic incay/decay
extern double pg_master_incay_duration;
extern double pg_master_incay_start_time;
extern float pg_master_incay_start_value;
extern double pg_master_decay_duration;
extern double pg_master_decay_start_time;
extern float pg_master_decay_start_value;


// resend all values (fx after interface crash)
extern bool resend_all_variables;
// resend all light values (fx after interface crash)
extern bool resend_all_light_variables;

extern bool synchr_start_recording_path[PG_NB_PATHS + 1];
extern int synchr_start_path_replay_trackNo[PG_NB_PATHS + 1];
extern bool recorded_path[PG_NB_PATHS + 1];
extern bool is_path_replay[PG_NB_PATHS + 1];

// track loading 
extern int currentClipArtTrack;

// working variables 
extern int drawing_start_frame;

extern bool frozen;

// previously stored on composition
extern int curentNbOSCParams;

// scenario launching time
extern double InitialScenarioTime;
extern double AbsoluteInitialScenarioTime;

// +++++++++++++++++++++++ Beats +++++++++++++++++++++++++++++++++
extern double lastBeatTime;

//////////////////////////////////////////////
// beat no
extern int pg_BeatNo;

// +++++++++++++++++++++++ CA seeding +++++++++++++++++++++++++++++++++
#if defined(CAAUDIO) || defined(RIVETS)
enum pg_CAseed_types
{
	_pg_CAseed_dot_center = 0,
	_pg_CAseed_dot,
	_pg_CAseed_h_line,
	_pg_CAseed_v_line,
	_pg_CAseed_cross,
	_pg_CAseed_X,
	_pg_CAseed_square,
	_pg_Nb_CAseed_types,
};
enum pg_CAseed_locations
{
	_pg_CAseed_loc_center = 0,
	_pg_CAseed_loc_N,
	_pg_CAseed_loc_E,
	_pg_CAseed_loc_S,
	_pg_CAseed_loc_W,
	_pg_CAseed_loc_NW,
	_pg_CAseed_loc_NE,
	_pg_CAseed_loc_SE,
	_pg_CAseed_loc_SW,
	_pg_Nb_CAseed_locations,
}; extern pg_CAseed_types pg_CAseed_type;
extern pg_CAseed_locations pg_CAseed_location;
extern int pg_CAseed_coordinates[2];
extern int pg_CAseed_size;
extern bool pg_CAseed_trigger;
void pg_CAseed_location_to_coordinates(pg_CAseed_locations location, int coordinates[2]);
#endif

// +++++++++++++++++++++++ FFT levels and frequency storage ++++++++++++++++++++
#ifdef CRITON
extern float fftLevels[8];
extern float fftFrequencies[8];
extern float fftPhases[8];
#endif


// +++++++++++++++++++++++ Aux Var +++++++++++++++++++++++++++++++++
extern char AuxString[1024];

// current working directory
extern std::string cwd;

// MANAGES EXCLUSIVE BUTTONS IN INTERFACE
template <typename T>
void ExclusiveButtonsAndLabelsOnOff(vector<string> ButtonPaths, vector<string> ButtonLabelPaths, vector<T> ButtonValues, bool withDefault, T value);
// READ CHAR FOR USB PEDALS
int getch_noblock(void);
// INITIALIZES ALL SCENARIO VARIABLES AND ASSIGNS THEM THE VALUES OF THE FIRST SCENARIO LINE
void pg_initializeScenearioVariables( void );
// INITIALIZES ALL SCENARIO VARIABLES CALLBACKS
void pg_initializationCallBacks(void);
// ASSIGNS INITIAL VALUES TO ALL CONFIGURATION FILES
void InitializeConfigurationVar(void);
// GUI DISPLAY & LOG FILE LOGGING
void pg_displaySceneVariables( void );
void pg_send_message_udp( char *pattern , char * message , char *targetHostid );
void pg_send_message_udp(char *pattern, char * message, pg_IPClient *targetHost);
pg_IPClient *pg_UDP_client(char *targetHostid);
void pg_logCurrentLineSceneVariables(string fileName);
void pg_logFirstLineSceneVariables(void);
void PlayTrack(int trackNo, double timeFromStart);
void StopTrack(void);
#if defined(TVW)
float starting_time(float elapsed_time_from_start);
#endif
void pg_launch_performance(int ind_scene);
void pg_update_scenario(void);
void updateXYKeystonePad(void);
void pg_process_key( int key );
void pg_process_special_key( int key );
void setup(int scene_ind);
void setup_minus(int decay);
void StartNewConfiguration(int config_no);
void StartNewScene(int ind_scene, double delta_time);
void pg_keyStrokeScripts( int key );
bool flash_beat_generation(int flash_frequency);
bool flash_continuous_generation(int flash_frequency);
void pg_flash_control(bool (*control_function)(int));
void pg_beat_controlled_flashes(void);
void pg_non_beat_controlled_flashes(void);
#if defined(var_flashPhotoTrkBright)
void pg_Make_flashPhoto(void);
#endif
void pg_aliasScript( string address , string string_argument_0 ,
					 float float_arguments[MAX_OSC_ARGUMENTS], int nb_arguments);
void ClipArt_OnOff(int indImage);
void ClipArt_Off(int indImage);
void ClipArt_On(int indImage);
#if defined(var_activeMeshes)
void Mesh_OnOff(int indImage);
#endif
void Caverne_Mesh_Profusion_On(int indImage);
void Caverne_Mesh_Profusion_Off(int indImage);
#if defined(var_mobileMeshes)
void Mesh_mobile_OnOff(int indImage);
void Mesh_mobile_Off(int indImage);
void Mesh_mobile_On(int indImage);
#endif
void Mesh_Off(int indImage);
void Mesh_On(int indImage);
void ClipArt_SubPathOnOff(int indPath);
void pg_update_pulsed_colors(void);
void compute_pulsed_palette_color(float color, float color_pulse, float grey, float grey_pulse, float pulsed_color[3], bool is_pen_color);
void HSVtoRGB(float h, float s, float v, float* r, float* g, float* b);
void RGBtoHSV(float r, float g, float b, float* h, float* s, float* v);
void compute_pulsed_HSV_color(float hue, float hue_pulse, float sat, float sat_pulse, float value, float value_pulse, float pulsed_color[3], bool is_pen_color);
#if defined(var_path_record_1)
void pg_path_recording_onOff(int indPath);
#endif
// playing track onoff
#if defined(var_path_replay_trackNo_1)
void pg_path_replay_trackNo_onOff( int indPath, int trackNo);
void path_replay_trackNo_callBack( int indPath, pg_Parameter_Input_Type param_input_type , int scenario_or_gui_command_value );
void pg_path_replay_trackNo_start(int indPath, int trackNo);
void pg_path_replay_trackNo_stop(int indPath);
#endif
#if defined(var_path_record_1)
void path_record_callBack( int indPath, pg_Parameter_Input_Type param_input_type , int scenario_or_gui_command_value );
void pg_path_recording_start( int indPath );
// recording on off
void pg_path_recording_stop( int indPath );
#endif
// interpolation on off
#if defined(var_fingers)
void NumberOfInteractionFingers(int nb_fingers);
#endif

void pg_writeMessageOnScreen( string text );
void pg_snapshot( char * type );
void pg_run_callBacks(void);

void pg_play_movie_no(void);
void setup_plus(int incay);

#if defined(var_clipCaptFreq)
void pg_play_clip_no(int indClipRank, int clipSide, int clipNo);
#endif

#endif
