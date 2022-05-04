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
#if defined (KOMPARTSD)
#include "pg_script_body_KompartSD.cpp"
bool testUDP_KOMPARTSD = false;
#endif
#if defined (REUTLINGEN)
#include "pg_script_body_Reutlingen.cpp"
#endif
#if defined (BICHES)
#include "pg_script_body_Biches.cpp"
#endif
#if defined (ATELIERSENFANTS)
#include "pg_script_body_AteliersEnfants.cpp"
#endif
#if defined (CAVERNEPLATON)
#include "pg_script_body_CavernePlaton.cpp"
#endif
#if defined (PIERRES)
#include "pg_script_body_Pierres.cpp"
#endif
#if defined (TEMPETE)
#include "pg_script_body_Tempete.cpp"
#endif
#if defined (DAWN)
#include "pg_script_body_Dawn.cpp"
#endif
#if defined (RIVETS)
#include "pg_script_body_Rivets.cpp"
#endif
#if defined (ULM)
#include "pg_script_body_Ulm.cpp"
#endif
#ifdef SONG
#include "pg_script_body_Song.cpp"
#endif
#if defined (DEMO) || defined (DEMO_BEZIER)
#include "pg_script_body_demo.cpp"
#endif
#ifdef VOLUSPA
#include "pg_script_body_voluspa.cpp"
#endif
#ifdef ARAKNIT
#include "pg_script_body_araknit.cpp"
#endif
#if defined (ETOILES)
#include "pg_script_body_etoiles.cpp"
#endif
#ifdef INTERFERENCE
#include "pg_script_body_interference.cpp"
#endif
#ifdef CAAUDIO
#include "pg_script_body_CAaudio.cpp"
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
// stepwise interpolation made only once
bool StepwiseInterpolationEffective[_MaxInterpVarIDs];
// initial values in the scenario (before first scene)
double InitialValuesInterpVar[_MaxInterpVarIDs];
// last value shipped to the GUI (PD)
float LastGUIShippedValuesInterpVar[_MaxInterpVarIDs];
// initial values in the configuration file
float InitialValuesConfigurationVar[_MaxConfigurationVarIDs];

//////////////////////////////////////////////
// beat no
int pg_BeatNo = 0;

//////////////////////////////////////////////
// beat no
int pg_MIDINote = 0;

//////////////////////////////////////////////
// Cuurent palette after interpolation
float pen_bandpass_3color_palette[3][3];

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

#ifdef PG_LIGHTS
float pulse_light_prec[3] = { 0.f };
#endif
#if PG_LIGHTS >= 1
float light1_color_prec = 0.f;
float light1_grey_prec = 0.f;
float light1_dimmer_prec = 0.f;
float light1_strobe_prec = 0.f;
// addition of color for light based on palette
float pulsed_light1_color[3];
float pulsed_light1_dimmer;
float pulsed_light1_strobe;
float light1_color_pulse_prec = 0.f;
float light1_grey_pulse_prec = 0.f;
float light1_dimmer_pulse_prec = 0.f;
float light1_strobe_pulse_prec = 0.f;
#endif
#if PG_LIGHTS >= 2
float light2_color_prec = 0.f;
float light2_grey_prec = 0.f;
float light2_dimmer_prec = 0.f;
float light2_strobe_prec = 0.f;
// addition of color for light based on palette
float pulsed_light2_color[3];
float pulsed_light2_dimmer;
float pulsed_light2_strobe;
float light2_color_pulse_prec = 0.f;
float light2_grey_pulse_prec = 0.f;
float light2_dimmer_pulse_prec = 0.f;
float light2_strobe_pulse_prec = 0.f;
#endif
#if PG_LIGHTS >= 3
float light3_color_prec = 0.f;
float light3_grey_prec = 0.f;
float light3_dimmer_prec = 0.f;
float light3_strobe_prec = 0.f;
// addition of color for light based on palette
float pulsed_light3_color[3];
float pulsed_light3_dimmer;
float pulsed_light3_strobe;
float light3_color_pulse_prec = 0.f;
float light3_grey_pulse_prec = 0.f;
float light3_dimmer_pulse_prec = 0.f;
float light3_strobe_pulse_prec = 0.f;
#endif
#if PG_LIGHTS >= 4
float light4_color_prec = 0.f;
float light4_grey_prec = 0.f;
float light4_dimmer_prec = 0.f;
float light4_strobe_prec = 0.f;
// addition of color for light based on palette
float pulsed_light4_color[3];
float pulsed_light4_dimmer;
float pulsed_light4_strobe;
float light4_color_pulse_prec = 0.f;
float light4_grey_pulse_prec = 0.f;
float light4_dimmer_pulse_prec = 0.f;
float light4_strobe_pulse_prec = 0.f;
#endif
#if PG_LIGHTS >= 5
float light5_color_prec = 0.f;
float light5_grey_prec = 0.f;
float light5_dimmer_prec = 0.f;
float light5_strobe_prec = 0.f;
// addition of color for light based on palette
float pulsed_light5_color[3];
float pulsed_light5_dimmer;
float pulsed_light5_strobe;
float light5_color_pulse_prec = 0.f;
float light5_grey_pulse_prec = 0.f;
float light5_dimmer_pulse_prec = 0.f;
float light5_strobe_pulse_prec = 0.f;
#endif
#if PG_LIGHTS >= 6
float light6_color_prec = 0.f;
float light6_grey_prec = 0.f;
float light6_dimmer_prec = 0.f;
float light6_strobe_prec = 0.f;
// addition of color for light based on palette
float pulsed_light6_color[3];
float pulsed_light6_dimmer;
float pulsed_light6_strobe;
float light6_color_pulse_prec = 0.f;
float light6_grey_pulse_prec = 0.f;
float light6_dimmer_pulse_prec = 0.f;
float light6_strobe_pulse_prec = 0.f;
#endif
#if PG_LIGHTS >= 7
float light7_color_prec = 0.f;
float light7_grey_prec = 0.f;
float light7_dimmer_prec = 0.f;
float light7_strobe_prec = 0.f;
// addition of color for light based on palette
float pulsed_light7_color[3];
float pulsed_light7_dimmer;
float pulsed_light7_strobe;
float light7_color_pulse_prec = 0.f;
float light7_grey_pulse_prec = 0.f;
float light7_dimmer_pulse_prec = 0.f;
float light7_strobe_pulse_prec = 0.f;
#endif
#if PG_LIGHTS >= 8
float light8_color_prec = 0.f;
float light8_grey_prec = 0.f;
float light8_dimmer_prec = 0.f;
float light8_strobe_prec = 0.f;
// addition of color for light based on palette
float pulsed_light8_color[3];
float pulsed_light8_dimmer;
float pulsed_light8_strobe;
float light8_color_pulse_prec = 0.f;
float light8_grey_pulse_prec = 0.f;
float light8_dimmer_pulse_prec = 0.f;
float light8_strobe_pulse_prec = 0.f;
#endif

// addition of colorfor pen  based on palette
float pulsed_pen_color[4];
float pulsed_repop_colorPart[3];
float pulsed_repop_colorBG[3];
float pulsed_repop_colorCA[3];

// memory of preceding pulsed color values to avoid unnecessary update
float repop_colorBG_prec = 0.f;
float repop_colorBG_pulse_prec = 0.f;
float repop_greyBG_prec = 0.f;
float repop_greyBG_pulse_prec = 0.f;

float repop_colorCA_prec = 0.f;
float repop_colorCA_pulse_prec = 0.f;
float repop_greyCA_prec = 0.f;
float repop_greyCA_pulse_prec = 0.f;

#ifndef PEN_HSV
float pen_color_prec = 0.f;
float pen_color_pulse_prec = 0.f;
float pen_grey_prec = 0.f;
float pen_grey_pulse_prec = 0.f;
#else
float pen_hue_prec = 0.f;
float pen_hue_pulse_prec = 0.f;
float pen_sat_prec = 0.f;
float pen_sat_pulse_prec = 0.f;
float pen_value_prec = 0.f;
float pen_value_pulse_prec = 0.f;
#endif

#ifndef PART_HSV
float repop_colorPart_prec = 0.f;
float repop_colorPart_pulse_prec = 0.f;
float repop_greyPart_prec = 0.f;
float repop_greyPart_pulse_prec = 0.f;
#else
float repop_huePart_prec = 0.f;
float repop_huePart_pulse_prec = 0.f;
float repop_satPart_prec = 0.f;
float repop_satPart_pulse_prec = 0.f;
float repop_valuePart_prec = 0.f;
float repop_valuePart_pulse_prec = 0.f;
#endif

float pulse_prec[3] = { 0.f };

// factor increasing the acceleration weight of pixels, the brush radius, color...
// based on sound volume attacks
float pulse_average;
float pulse_average_prec = 0.f;
float pulse[3] = { 0.0f };
float seed_pulsePerlinNoise[3 * 2] = { rand_0_1, rand_0_1, rand_0_1, rand_0_1, rand_0_1, rand_0_1};
// not used currently float pulse_attack;

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
float current_extern_movieNo = -1;

// movie soundtrack passes over an onset or a peak before next frame
bool pg_video_sound_onset = false;
bool pg_video_sound_peak = false;

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
#ifdef PG_WITH_PHOTO_FLASH
bool is_flashPhotoTrk = false;
float flashPhotoTrk_weight = 0.0f;
float flashPhotoTrk_decay = 1.0f;
float flashPhotoTrk_threshold = 0.5f;
int flashPhotoTrk_nbFrames = 0;
#endif

// +++++++++++++++++++++ VIDEO BACKGROUND SUBTRACTION +++++
bool is_firstVideoOn = true;

// +++++++++++++++++++++ CLEAR +++++++++++++++++++++++++++
int isClearCA;
int isClearLayer;
int isClearAllLayers;
int isClearEcho;

// +++++++++++++++++++++ SETUP +++++++++++++++++++++++++++
float LastSetupChangeClockTime = 0;

// +++++++++++++++++++++ DELAYED CAMERA WEIGHT ++++++++++++
int delayedCameraWeight = 0;

#ifdef PG_WITH_BLUR
// +++++++++++++++++++++ BLUR +++++++++++++++++++++++++++
bool is_blur_1 = false;
bool is_blur_2 = false;
int nb_blur_frames_1 = 0;
int nb_blur_frames_2 = 0;
#endif

// +++++++++++++++++++++ COPY LAYERS +++++++++++++++++++++++++++
// copy to layer above (+1) or to layer below (-1)
int copyToNextTrack = 0;

// ++++++++++++++++++++++ SET-UP +++++++++++++++++++++++++++ 
int pg_CurrentSceneIndex = -1;
Scene *pg_CurrentScene = NULL;
float pg_setup_interpolation_duration = 0.f;

// scene management
bool pg_FirstFrameInScene = true;
float remainingTimeInScene = 0.0f;

// interpolation scene between two discontinuous scenes so that there is no visual gap inbetween
int pg_SceneIndexAfterInterpolation = -1;
int pg_SceneIndexBeforeInterpolation = -1;
float pg_SceneInterpolationDuration = 0.f;
Scene pg_InterpolationScene;

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

#if defined(CAAUDIO) || defined(RIVETS)
pg_CAseed_types pg_CAseed_type = _pg_CAseed_dot_center;
pg_CAseed_locations pg_CAseed_location = _pg_CAseed_loc_center;
int pg_CAseed_coordinates[2] = { -1, -1 };
int pg_CAseed_size = 1;
bool pg_CAseed_trigger = false;
#endif

// +++++++++++++++++++++++ FFT levels and frequency storage ++++++++++++++++++++
#ifdef CRITON
float fftLevels[8];
float fftFrequencies[8];
float fftPhases[8];
#endif

// +++++++++++++++++++++++ Buttons and labels for ATELIERSENFANTS interface +++++
vector<string> FondButtonsPaths = { "/fond/fond1", "/fond/fond2", "/fond/fond3", "/fond/fond4" };
vector<string> FondButtonLabelsPaths = { "/fond/fond1_label", "/fond/fond2_label", "/fond/fond3_label", "/fond/fond4_label" };
vector<int> FondButtonValues = { 0, 1, 2, 3 };

vector<string> DessinButtonsPaths = { "/page/dessin", "/page/serpent", "/page/avion" };
vector<string> DessinButtonLabelsPaths = { "/page/dessin_label", "/page/serpent_label", "/page/avion_label" };
vector<float> DessinButtonValues = { 0.f, 0.002f, 0.02f };

vector<string> StylusvsRubberButtonsPaths = { "/trait/rubber", "/trait/pen" };
vector<string> StylusvsRubberButtonLabelsPaths = { "/trait/rubber_label", "/trait/pen_label" };
vector<int> StylusvsRubberButtonValues = { pg_Rubber, pg_Stylus };




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
	_Note1,
	_Note2,
	_Note3,
	_flashPhoto,
	_pulse_spectrum,
	_beat,
	_MIDINote,
	_flashTrkCA_onOff,
	_flashTrkPart_onOff,
	_snapshot,
	_initialBGCapture,
	_initCA,
	_currentVideoTrack_plus,
	_currentPhotoTrack_plus,
	_minimal_display,
	_paths_display,
	_new_scene,
	_pen_xy,
	_pen2_xy,
	_pen3_xy,
	_pen4_xy,
	_pen5_xy,
	_abs_pen_xy,
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
#ifdef PG_WITH_BLUR
	_is_blur_1,
	_is_blur_1_plus,
	_is_blur_1_plus_plus,
	_is_blur_2,
#endif
	_cameraCumul_plus,
	_pen_brush_plus,
	_pen_brush_minus,
	_pen_position_dash_plus,
#ifdef PG_NB_PIXEL_MODES
	_pixel_mode_plus,
#endif
#ifdef PG_NB_CA_TYPES
	_CA1Type_plus,
	_CA1SubType_plus,
	_CA1Type_minus,
	_CA1SubType_minus,
	_CAonOff,
#endif
	_sensor_layout_plus,
	_sensor_sample_setUp_plus,
	_sensor_activation_plus,
	_partExit_mode_plus,
	_partStroke_mode_plus,
	_partColor_mode_plus,
	_pen_BW,
	_repopBG_BW,
	_repopCA_BW,
	_repopPart_BW,
	_pressure_onOff,
	_diaporama_plus,
	_diaporama_minus,
	_soundtrack_plus,
	_soundtrack_minus,
	_soundtrack_onOff,
	_soundtrack_volume,
	_movie_onOff,
	_movie_loop_onOff,
	_pen_colorPreset_minus,
	_pen_colorPreset_plus,
	_pen_colorPreset,
#if defined(CAAUDIO) || defined(RIVETS)
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
	_movie_minus,
	_movie_forward,
	_movie_backward,
	_resend_all,
	_setup,
	_setupName,
	_setup_plus,
	_setup_plus_5,
	_setup_minus,
	_setup_plus_keep_total_dur,
	_interpolation_duration,
#ifdef CRITON
	_JUCE_loop_track,
	_JUCE_exit,
	_fftLevel8,
#endif
	_ClipArt_1_onOff,
	_ClipArt_2_onOff,
	_ClipArt_3_onOff,
	_ClipArt_4_onOff,
	_ClipArt_5_onOff,
	_ClipArt_6_onOff,
	_ClipArt_7_onOff,
	_ClipArt_8_onOff,
	_ClipArt_9_onOff,
	_ClipArt_10_onOff,
	_ClipArt_11_onOff,
	_ClipArt_12_onOff,
	_ClipArt_13_onOff,
	_ClipArt_14_onOff,
	_ClipArt_15_onOff,
	_ClipArt_16_onOff,
	_ClipArt_17_onOff,
	_ClipArt_18_onOff,
	_ClipArt_19_onOff,
	_ClipArt_20_onOff,
	_ClipArt_onOff,
	_ClipArt_off,
	_ClipArt_on,
	_ClipArt_SubPath_1_onOff,
	_ClipArt_SubPath_2_onOff,
	_ClipArt_SubPath_3_onOff,
	_ClipArt_SubPath_4_onOff,
	_ClipArt_scale,
	_ClipArt_rotate,
	_ClipArt_xy,
	_ClipArt_nat_color,
	_ClipArt_white_color,
	_ClipArt_red_color,
	_ClipArt_green_color,
	_ClipArt_blue_color,
	_ClipArt_yellow_color,
	_ClipArt_cyan_color,
	_ClipArt_magenta_color,
	_ClipArt_translations,
#ifdef PG_MESHES
	_Mesh_onOff,
#ifdef CAVERNEPLATON
	_Caverne_Mesh_Profusion_on,
	_Caverne_Mesh_Profusion_off,
	_Caverne_Mesh_7Solids_on,
	_Caverne_Mesh_7Solids_off,
	_Caverne_BackColor_onOff,
#endif
	_Mesh_mobile_onOff,
	_Mesh_light_x,
	_Mesh_light_y,
	_Mesh_light_z,
#endif
	_isClearAllLayers,
	_StylusvsRubber,
#ifdef ATELIERSENFANTS
	_pen_hue_sat_value,
	_repop_hue_sat_valuePart,
#endif
	_take_snapshot,
	_path_replay_stopAll,
	_path_replay_playAll,
#ifdef PG_METAWEAR
	_mw_mss_pos,
#endif
#if defined(BICHES) || defined (ATELIERSENFANTS)
	_big_green_pen_track1,
	_big_red_pen_track2,
	_small_black_pen_track2,
#endif
	_cameraWB_R_plus,
	_cameraWB_B_plus,
	_cameraExposure_plus,
	_cameraGain_plus,
	_cameraBrightness_plus,
	_cameraSaturation_plus,
	_cameraContrast_plus,
	_cameraGamma_plus,
	_cameraWB_R_minus,
	_cameraWB_B_minus,
	_cameraExposure_minus,
	_cameraGain_minus,
	_cameraBrightness_minus,
	_cameraSaturation_minus,
	_cameraContrast_minus,
	_cameraGamma_minus,
	_reset_on_camera,
#ifdef PIERRES
	_processing_video,
	_processing_image,
	_processing_master,
#endif
	_BottomLeft,
	_BottomRight,
	_TopLeft,
	_TopRight,
	_ZeroLight,
	_flashPixel,
	_flashTrkPart,
	_flashTrkCA,
	_flashCABG,
	_flashCAPart,               
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
	{ "Note1", _Note1 },
	{ "Note2", _Note2 },
	{ "Note3", _Note3 },
	{ "flashPhoto", _flashPhoto },
	{ "pulse_spectrum", _pulse_spectrum },
	{ "MIDINote", _MIDINote },
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
	{ "new_scene", _new_scene },
	{ "pen_xy", _pen_xy },
	{ "pen2_xy", _pen2_xy },
	{ "pen3_xy", _pen3_xy },
	{ "pen4_xy", _pen4_xy },
	{ "pen5_xy", _pen5_xy },
	{ "abs_pen_xy", _abs_pen_xy },
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
#ifdef PG_WITH_BLUR
	{ "is_blur_1", _is_blur_1 },
	{ "is_blur_1_plus", _is_blur_1_plus },
	{ "is_blur_1_plus_plus", _is_blur_1_plus_plus },
	{ "is_blur_2", _is_blur_2 },
#endif
	{ "cameraCumul_plus", _cameraCumul_plus },
	{ "pen_brush_plus", _pen_brush_plus },
	{ "pen_brush_minus", _pen_brush_minus },
	{ "pen_position_dash_plus", _pen_position_dash_plus },
#ifdef PG_NB_PIXEL_MODES
	{ "pixel_mode_plus", _pixel_mode_plus },
#endif
#ifdef PG_NB_CA_TYPES
	{ "CA1Type_plus", _CA1Type_plus },
	{ "CA1SubType_plus", _CA1SubType_plus },
	{ "CA1Type_minus", _CA1Type_minus },
	{ "CA1SubType_minus", _CA1SubType_minus },
	{ "CAonOff", _CAonOff },
#endif
	{ "sensor_layout_plus", _sensor_layout_plus },
	{ "sensor_sample_setUp_plus", _sensor_sample_setUp_plus },
	{ "sensor_activation_plus", _sensor_activation_plus },
	{ "partExit_mode_plus", _partExit_mode_plus },
	{ "partStroke_mode_plus", _partStroke_mode_plus },
	{ "partColor_mode_plus", _partColor_mode_plus },
	{ "pen_BW", _pen_BW },
	{ "repopBG_BW", _repopBG_BW },
	{ "repopCA_BW", _repopCA_BW },
	{ "repopPart_BW", _repopPart_BW },
	{ "pressure_onOff", _pressure_onOff },
	{ "diaporama_plus", _diaporama_plus },
	{ "diaporama_minus", _diaporama_minus },
	{ "soundtrack_plus", _soundtrack_plus },
	{ "soundtrack_minus", _soundtrack_minus },
	{ "soundtrack_onOff", _soundtrack_onOff },
	{ "soundtrack_volume", _soundtrack_volume },
	{ "movie_onOff", _movie_onOff },
	{ "movie_loop_onOff", _movie_loop_onOff },
	{ "pen_colorPreset_minus", _pen_colorPreset_minus },
	{ "pen_colorPreset_plus", _pen_colorPreset_plus },
	{ "pen_colorPreset", _pen_colorPreset },
#if defined(CAAUDIO) || defined(RIVETS)
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
	{ "movie_minus", _movie_minus },
	{ "movie_forward", _movie_forward },
	{ "movie_backward", _movie_backward },
	{ "resend_all", _resend_all },
	{ "setup_plus", _setup_plus },
	{ "setup_plus_5", _setup_plus_5 },
	{ "setup", _setup },
	{ "setupName", _setupName },
	{ "setup_minus", _setup_minus },
	{ "setup_plus_keep_total_dur", _setup_plus_keep_total_dur },
	{ "interpolation_duration", _interpolation_duration },
#ifdef CRITON
	{ "JUCE_loop_track", _JUCE_loop_track },
	{ "JUCE_exit", _JUCE_exit },
	{ "fftLevel8", _fftLevel8 },
#endif
	{ "ClipArt_1_onOff", _ClipArt_1_onOff },
	{ "ClipArt_2_onOff", _ClipArt_2_onOff },
	{ "ClipArt_3_onOff", _ClipArt_3_onOff },
	{ "ClipArt_4_onOff", _ClipArt_4_onOff },
	{ "ClipArt_5_onOff", _ClipArt_5_onOff },
	{ "ClipArt_6_onOff", _ClipArt_6_onOff },
	{ "ClipArt_7_onOff", _ClipArt_7_onOff },
	{ "ClipArt_8_onOff", _ClipArt_8_onOff },
	{ "ClipArt_9_onOff", _ClipArt_9_onOff },
	{ "ClipArt_10_onOff", _ClipArt_10_onOff },
	{ "ClipArt_11_onOff", _ClipArt_11_onOff },
	{ "ClipArt_12_onOff", _ClipArt_12_onOff },
	{ "ClipArt_13_onOff", _ClipArt_13_onOff },
	{ "ClipArt_14_onOff", _ClipArt_14_onOff },
	{ "ClipArt_15_onOff", _ClipArt_15_onOff },
	{ "ClipArt_16_onOff", _ClipArt_16_onOff },
	{ "ClipArt_17_onOff", _ClipArt_17_onOff },
	{ "ClipArt_18_onOff", _ClipArt_18_onOff },
	{ "ClipArt_19_onOff", _ClipArt_19_onOff },
	{ "ClipArt_20_onOff", _ClipArt_20_onOff },
	{ "ClipArt_onOff", _ClipArt_onOff },
	{ "ClipArt_off", _ClipArt_off },
	{ "ClipArt_on", _ClipArt_on },
	{ "ClipArt_SubPath_1_onOff", _ClipArt_SubPath_1_onOff },
	{ "ClipArt_SubPath_2_onOff", _ClipArt_SubPath_2_onOff },
	{ "ClipArt_SubPath_3_onOff", _ClipArt_SubPath_3_onOff },
	{ "ClipArt_SubPath_4_onOff", _ClipArt_SubPath_4_onOff },
	{ "ClipArt_scale", _ClipArt_scale },
	{ "ClipArt_rotate", _ClipArt_rotate },
	{ "ClipArt_xy", _ClipArt_xy },
	{ "ClipArt_nat_color", _ClipArt_nat_color },
	{ "ClipArt_white_color", _ClipArt_white_color },
	{ "ClipArt_red_color", _ClipArt_red_color },
	{ "ClipArt_green_color", _ClipArt_green_color },
	{ "ClipArt_blue_color", _ClipArt_blue_color },
	{ "ClipArt_yellow_color", _ClipArt_yellow_color },
	{ "ClipArt_cyan_color", _ClipArt_cyan_color },
	{ "ClipArt_magenta_color", _ClipArt_magenta_color },
	{ "ClipArt_translations", _ClipArt_translations },
#ifdef PG_MESHES
	{ "Mesh_onOff", _Mesh_onOff },
#ifdef CAVERNEPLATON
	{ "Caverne_Mesh_Profusion_on", _Caverne_Mesh_Profusion_on },
	{ "Caverne_Mesh_Profusion_off", _Caverne_Mesh_Profusion_off },
	{ "Caverne_Mesh_7Solids_on", _Caverne_Mesh_7Solids_on },
	{ "Caverne_Mesh_7Solids_off", _Caverne_Mesh_7Solids_off },
	{ "Caverne_BackColor_onOff", _Caverne_BackColor_onOff },
#endif
	{ "Mesh_mobile_onOff", _Mesh_mobile_onOff },
	{ "Mesh_light_x", _Mesh_light_x },
	{ "Mesh_light_y", _Mesh_light_y },
	{ "Mesh_light_z", _Mesh_light_z },
#endif
	{ "isClearAllLayers", _isClearAllLayers },
	{ "StylusvsRubber", _StylusvsRubber },
#ifdef ATELIERSENFANTS
	{ "pen_hue_sat_value", _pen_hue_sat_value },
	{ "repop_hue_sat_valuePart", _repop_hue_sat_valuePart },
#endif
	{ "take_snapshot", _take_snapshot },
	{ "path_replay_stopAll", _path_replay_stopAll },
	{ "path_replay_playAll", _path_replay_playAll },
#ifdef PG_METAWEAR
	{ "mw_mss_pos", _mw_mss_pos },
#endif
#if defined(BICHES) || defined (ATELIERSENFANTS)
	{ "big_green_pen_track1", _big_green_pen_track1 },
	{ "big_red_pen_track2",  _big_red_pen_track2 },
	{ "small_black_pen_track2", _small_black_pen_track2 },
#endif
	{ "cameraWB_R_plus", _cameraWB_R_plus },
	{ "cameraWB_B_plus", _cameraWB_B_plus },
	{ "cameraExposure_plus", _cameraExposure_plus },
	{ "cameraGain_plus", _cameraGain_plus },
	{ "cameraBrightness_plus", _cameraBrightness_plus },
	{ "cameraSaturation_plus", _cameraSaturation_plus },
	{ "cameraContrast_plus", _cameraContrast_plus },
	{ "cameraGamma_plus", _cameraGamma_plus },
	{ "cameraWB_R_minus", _cameraWB_R_minus },
	{ "cameraWB_B_minus", _cameraWB_B_minus },
	{ "cameraExposure_minus", _cameraExposure_minus },
	{ "cameraGain_minus", _cameraGain_minus },
	{ "cameraBrightness_minus", _cameraBrightness_minus },
	{ "cameraSaturation_minus", _cameraSaturation_minus },
	{ "cameraContrast_minus", _cameraContrast_minus },
	{ "cameraGamma_minus", _cameraGamma_minus },
	{ "reset_on_camera", _reset_on_camera },
#ifdef PIERRES
	{ "processing_video", _processing_video },
	{ "processing_image", _processing_image },
	{ "processing_master", _processing_master },
#endif
	{ "TopLeft", _TopLeft },
	{ "TopRight", _TopRight },
	{ "BottomLeft", _BottomLeft },
	{ "BottomRight", _BottomRight },
	{ "ZeroLight", _ZeroLight },
	{ "flashPixel", _flashPixel },
	{ "flashTrkPart", _flashTrkPart },
	{ "flashTrkCA", _flashTrkCA },
	{ "flashCABG", _flashCABG },
	{ "flashCAPart", _flashCAPart },
};
// if (strcmp(newCommand, CmdString[indVar]

#if defined(CAAUDIO) || defined(RIVETS)
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
// MANAGES EXCLUSIVE BUTTONS IN INTERFACE
template <typename T>
void ExclusiveButtonsAndLabelsOnOff(vector<string> ButtonPaths, vector<string> ButtonLabelPaths, vector<T> ButtonValues, bool withDefault, T value) {
	if(ButtonPaths.size() != ButtonValues.size() || ButtonLabelPaths.size() != ButtonValues.size()) {
		sprintf(ErrorStr, "Incorrect button values and sizes (%d/%d/%d) differ!", ButtonPaths.size(), ButtonLabelPaths.size(), ButtonValues.size()); ReportError(ErrorStr);  throw 768;
	}
	// all greyed except the active one
	for (int ind = 0; ind < int(ButtonPaths.size()); ind++) {
		if (fabs(value - ButtonValues[ind]) < 0.00001) {
			int selectedButtonIndex = ind;
			for (int indAux = 0; indAux < int(ButtonPaths.size()); indAux++) {
				if (indAux == selectedButtonIndex) {
					sprintf(AuxString, "%s/textColor FF8888FF", ButtonLabelPaths[indAux].c_str()); pg_send_message_udp((char*)"s", (char*)AuxString, (char*)"udp_TouchOSC_send");
					sprintf(AuxString, "%s/color FF0000FF", ButtonPaths[indAux].c_str()); pg_send_message_udp((char*)"s", (char*)AuxString, (char*)"udp_TouchOSC_send");
				}
				else {
					sprintf(AuxString, "%s/textColor AAAAAAFF", ButtonLabelPaths[indAux].c_str()); pg_send_message_udp((char*)"s", (char*)AuxString, (char*)"udp_TouchOSC_send");
					sprintf(AuxString, "%s/color 888888FF", ButtonPaths[indAux].c_str()); pg_send_message_udp((char*)"s", (char*)AuxString, (char*)"udp_TouchOSC_send");
				}
			}
			return;
		}
	}
	// no active one and default allowed => all greyed
	if (withDefault) {
		for (int ind = 0; ind < int(ButtonPaths.size()); ind++) {
			sprintf(AuxString, "%s/textColor AAAAAAFF", ButtonLabelPaths[ind].c_str()); pg_send_message_udp((char*)"s", (char*)AuxString, (char*)"udp_TouchOSC_send");
			sprintf(AuxString, "%s/color 888888FF", ButtonPaths[ind].c_str()); pg_send_message_udp((char*)"s", (char*)AuxString, (char*)"udp_TouchOSC_send");
		}
	}
}


///////////////////////////////////////////////////////////////////////////////////
// ASSIGNS INITIAL VALUES TO ALL CONFIGURATION FILES
void InitializeConfigurationVar(void) {
	for (int indVar = 0; indVar < _MaxConfigurationVarIDs; indVar++) {
		if (ConfigurationVarTypes[indVar] == _pg_float) {
			*((float *)ConfigurationVarPointers[indVar]) = InitialValuesConfigurationVar[indVar];
		}
		else if (ConfigurationVarTypes[indVar] == _pg_int) {
			*((int *)ConfigurationVarPointers[indVar]) = int(InitialValuesConfigurationVar[indVar]);
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
// INITIALIZES ALL SCENARIO VARIABLES AND ASSIGNS THEM THE VALUES OF THE INITIAL VALUES LINE
void pg_initializeScenearioVariables(void) {
	/////////////////////////////////////////////////////////////////////////
	// CONSTANT UNIFORM VARIABLES
	// only assigned at initialization, does not change during the set
	//glUseProgram(shader_programme[pg_shader_Camera]);
	//glUniform2f(uniform_Camera_vp_2fv_width_height,
	//	(GLfloat)leftWindowWidth, (GLfloat)window_height);

#if defined (TEXTURED_QUAD_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 
	// only assigned at initialization, does not change during the set
	glUseProgram(shader_programme[pg_shader_ParticleAnimation]);

	///////////////////////////////////////////////////////////////////
	// VERTEX SHADER VARIABLE
	glUniform2f(uniform_ParticleAnimation_vp_2fv_width_height,
		(GLfloat)leftWindowWidth, (GLfloat)window_height);
#endif

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
			*((float*)ScenarioVarPointers[indVar]) = float(InitialValuesInterpVar[indVar]);
		}
		else if (ScenarioVarTypes[indVar] == _pg_int) {
			*((int*)ScenarioVarPointers[indVar]) = int(InitialValuesInterpVar[indVar]);
		}
		else if (ScenarioVarTypes[indVar] == _pg_bool) {
			*((bool*)ScenarioVarPointers[indVar]) = (InitialValuesInterpVar[indVar] > 0 ? true : false);
		}
		else if (ScenarioVarTypes[indVar] == _pg_sign) {
			*((float*)ScenarioVarPointers[indVar]) = (InitialValuesInterpVar[indVar] > 0 ? 1.0f : -1.0f);
		}
		else if (ScenarioVarTypes[indVar] == _pg_path) {
			*((bool*)ScenarioVarPointers[indVar]) = (InitialValuesInterpVar[indVar] > 0 ? true : false);
		}
		else {
			sprintf(ErrorStr, "Incorrect variable type (%d) for scenario variable #%d!", ScenarioVarTypes[indVar], indVar); ReportError(ErrorStr);
		}
	}

	// track replay 
	// source track recording
	// is recording source -> has to stop recording source 
#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7 || PG_NB_PATHS == 11
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
#if PG_NB_PATHS == 7 || PG_NB_PATHS == 11
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
#if PG_NB_PATHS == 11
	if (path_replay_trackNo_8 >= 0) {
		if (tracksSync) {
			synchr_start_path_replay_trackNo[8] = path_replay_trackNo_8;
		}
		else {
			pg_path_replay_trackNo_onOff(8, path_replay_trackNo_8);
		}
	}
	if (path_replay_trackNo_9 >= 0) {
		if (tracksSync) {
			synchr_start_path_replay_trackNo[9] = path_replay_trackNo_9;
		}
		else {
			pg_path_replay_trackNo_onOff(9, path_replay_trackNo_9);
		}
	}
	if (path_replay_trackNo_10 >= 0) {
		if (tracksSync) {
			synchr_start_path_replay_trackNo[10] = path_replay_trackNo_10;
		}
		else {
			pg_path_replay_trackNo_onOff(10, path_replay_trackNo_10);
		}
	}
	if (path_replay_trackNo_11 >= 0) {
		if (tracksSync) {
			synchr_start_path_replay_trackNo[11] = path_replay_trackNo_11;
		}
		else {
			pg_path_replay_trackNo_onOff(11, path_replay_trackNo_11);
		}
	}
#endif

	// track recording 
	// source track recording
	// is recording source -> has to stop recording source 
#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7 || PG_NB_PATHS == 11
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
#if PG_NB_PATHS == 7 || PG_NB_PATHS == 11
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
#if PG_NB_PATHS == 11
	if (!path_record_8) {
		pg_path_recording_stop(8);
	}
	if (!path_record_9) {
		pg_path_recording_stop(9);
	}
	if (!path_record_10) {
		pg_path_recording_stop(10);
	}
	if (!path_record_11) {
		pg_path_recording_stop(11);
	}
#endif
	for (int indPath = 1; indPath < PG_NB_PATHS; indPath++) {
		sprintf(AuxString, "/path_replay_trackNo_%d 0", indPath);
		pg_send_message_udp((char*)"i", AuxString, (char*)"udp_TouchOSC_send");
		sprintf(AuxString, "/path_record_%d 0", indPath);
		pg_send_message_udp((char*)"i", AuxString, (char*)"udp_TouchOSC_send");
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
#ifdef PG_WITH_CAMERA_CAPTURE
	reset_camera = true;
#endif

#if defined (TEXTURED_QUAD_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 
	// particle initialization reset
	part_initialization = -1;
#endif

	// clear
	isClearCA = 0;
	isClearLayer = 0;
	isClearAllLayers = 0;
	isClearEcho = 0;

	// DELAYED CAMERA WEIGHT
	delayedCameraWeight = 0;

#ifdef PG_WITH_BLUR
	// blur
	is_blur_1 = false;
	is_blur_2 = false;
	nb_blur_frames_1 = 0;
	nb_blur_frames_2 = 0;
#endif

#if defined(CAAUDIO) || defined(RIVETS)
	// CA seed
	pg_CAseed_trigger = false;
	pg_CAseed_type = _pg_CAseed_dot_center;
	// INTERFACE VARIABLE INITIALIZATION
	sprintf(AuxString, "/CAseed_dot_center %d", 1); pg_send_message_udp((char*)"i", (char*)AuxString, (char*)"udp_TouchOSC_send");
	sprintf(AuxString, "/CAseed_dot %d", 0); pg_send_message_udp((char*)"i", (char*)AuxString, (char*)"udp_TouchOSC_send");
	sprintf(AuxString, "/CAseed_h_line %d", 0); pg_send_message_udp((char*)"i", (char*)AuxString, (char*)"udp_TouchOSC_send");
	sprintf(AuxString, "/CAseed_v_line %d", 0); pg_send_message_udp((char*)"i", (char*)AuxString, (char*)"udp_TouchOSC_send");
	sprintf(AuxString, "/CAseed_cross %d", 0); pg_send_message_udp((char*)"i", (char*)AuxString, (char*)"udp_TouchOSC_send");
	sprintf(AuxString, "/CAseed_X %d", 0); pg_send_message_udp((char*)"i", (char*)AuxString, (char*)"udp_TouchOSC_send");
	sprintf(AuxString, "/CAseed_square %d", 0); pg_send_message_udp((char*)"i", (char*)AuxString, (char*)"udp_TouchOSC_send");
	pg_CAseed_location = _pg_CAseed_loc_center;
	sprintf(AuxString, "/CAseed_loc %d", pg_CAseed_location); pg_send_message_udp((char*)"i", (char*)AuxString, (char*)"udp_TouchOSC_send");
	pg_CAseed_coordinates[0] = leftWindowWidth / 2;
	pg_CAseed_coordinates[1] = window_height / 2;
	pg_CAseed_size = 1;
	sprintf(AuxString, "/CAseed_size %d", pg_CAseed_size); pg_send_message_udp((char*)"i", (char*)AuxString, (char*)"udp_TouchOSC_send");
#endif

	// copy to layer above (+1) or to layer below (-1)
	copyToNextTrack = 0;

	// pen music color
	pulsed_pen_color[0] = RGBpalette_r_ini;
	pulsed_pen_color[1] = RGBpalette_g_ini;
	pulsed_pen_color[2] = RGBpalette_b_ini;
	pulsed_pen_color[3] = RGBpalette_a_ini;

	// repop music color
	pulsed_repop_colorBG[0] = RGBpalette_r_ini;
	pulsed_repop_colorBG[1] = RGBpalette_g_ini;
	pulsed_repop_colorBG[2] = RGBpalette_b_ini;
	pulsed_repop_colorCA[0] = RGBpalette_r_ini;
	pulsed_repop_colorCA[1] = RGBpalette_g_ini;
	pulsed_repop_colorCA[2] = RGBpalette_b_ini;
	pulsed_repop_colorPart[0] = RGBpalette_r_ini;
	pulsed_repop_colorPart[1] = RGBpalette_g_ini;
	pulsed_repop_colorPart[2] = RGBpalette_b_ini;

	// light1 music color
#if PG_LIGHTS >= 1
	pulsed_light1_color[0] = RGBpalette_r_ini;
	pulsed_light1_color[1] = RGBpalette_g_ini;
	pulsed_light1_color[2] = RGBpalette_b_ini;
	pulsed_light1_dimmer = 0.f;
	pulsed_light1_strobe = 0.f;
#endif
#if PG_LIGHTS >= 1
	pulsed_light2_color[0] = RGBpalette_r_ini;
	pulsed_light2_color[1] = RGBpalette_g_ini;
	pulsed_light2_color[2] = RGBpalette_b_ini;
	pulsed_light2_dimmer = 0.f;
	pulsed_light2_strobe = 0.f;
#endif
#if PG_LIGHTS >= 1
	pulsed_light3_color[0] = RGBpalette_r_ini;
	pulsed_light3_color[1] = RGBpalette_g_ini;
	pulsed_light3_color[2] = RGBpalette_b_ini;
	pulsed_light3_dimmer = 0.f;
	pulsed_light3_strobe = 0.f;
#endif
	// pulse from sound 
	pulse_average = 0.0f;

	/////////////////////////////////////////////////////////////////////////
	// DRAWING-CONTROLLED UNIFORM VARIABLES: dynamic values received from drawing or track display 
	// pen position storage on the two quads 
	for (int indPath = 0; indPath < PG_NB_PATHS + 1; indPath++) {
		paths_x[indPath] = PG_OUT_OF_SCREEN_CURSOR;
		paths_y[indPath] = PG_OUT_OF_SCREEN_CURSOR;
		paths_x_next[indPath] = PG_OUT_OF_SCREEN_CURSOR;
		paths_y_next[indPath] = PG_OUT_OF_SCREEN_CURSOR;
		paths_x_prev_prev[indPath] = PG_OUT_OF_SCREEN_CURSOR;
		paths_y_prev_prev[indPath] = PG_OUT_OF_SCREEN_CURSOR;
		paths_x_prev[indPath] = PG_OUT_OF_SCREEN_CURSOR;
		paths_y_prev[indPath] = PG_OUT_OF_SCREEN_CURSOR;
		paths_x_memory[indPath] = PG_OUT_OF_SCREEN_CURSOR;
		paths_y_memory[indPath] = PG_OUT_OF_SCREEN_CURSOR;
		paths_x_prev_memory[indPath] = PG_OUT_OF_SCREEN_CURSOR;
		paths_y_prev_memory[indPath] = PG_OUT_OF_SCREEN_CURSOR;
		isBegin[indPath] = false;
		isEnd[indPath] = false;
#ifdef PG_BEZIER_PATHS
		paths_xL[indPath] = PG_OUT_OF_SCREEN_CURSOR;
		paths_yL[indPath] = PG_OUT_OF_SCREEN_CURSOR;
		paths_xR[indPath] = PG_OUT_OF_SCREEN_CURSOR;
		paths_yR[indPath] = PG_OUT_OF_SCREEN_CURSOR;
#endif

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
	repop_ColorBG_r = 0.f;
	repop_ColorBG_g = 0.f;
	repop_ColorBG_b = 0.f;
	repop_ColorCA_r = 0.f;
	repop_ColorCA_g = 0.f;
	repop_ColorCA_b = 0.f;
	repop_ColorPart_r = 0.f;
	repop_ColorPart_g = 0.f;
	repop_ColorPart_b = 0.f;

	// hoover cursor
	CurrentCursorHooverPos_x = 0;
	CurrentCursorHooverPos_y = 0;
	// stylus type
	CurrentCursorStylusvsRubber = pg_Stylus;

	/////////////////////////////////////////////////////////////////////////
	// TIME CONTROLLED VARIABLES (NOT FORWARDED TO SHADERS)
	// scenario management
	pg_CurrentSceneIndex = -1;
	pg_CurrentScene = NULL;
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

	// pg_send_message_udp((char *)"s", (char *)"/message init_completed", (char *)"udp_TouchOSC_send");
	// INTERFACE VARIABLE INITIALIZATION

	// soundtrack on
	soundTrack_on = true;
#ifdef PG_WITH_PUREDATA
	sprintf(AuxString, "/soundtrack_onOff %d", soundTrack_on);
	pg_send_message_udp((char*)"i", AuxString, (char*)"udp_PD_send");
	printf("Init script: soundtrack: %s\n", AuxString);
#endif
	sprintf(AuxString, "/soundtrack_onOff %d", !soundTrack_on);
	pg_send_message_udp((char*)"i", AuxString, (char*)"udp_TouchOSC_send");
	sprintf(AuxString, "/soundtrack_volume %d", int(soundTrack_on));
	pg_send_message_udp((char*)"i", AuxString, (char*)"udp_TouchOSC_send");

	// movie on
	sprintf(AuxString, "/movie_onOff %d", !movie_on);
	pg_send_message_udp((char*)"i", AuxString, (char*)"udp_TouchOSC_send");

#if defined (PIERRES)
	updateXYKeystonePad();
#endif

	// rubber vs pen interface initialization
	if (CurrentCursorStylusvsRubber == pg_Stylus) {
		sprintf(AuxString, "/StylusvsRubber %d", pg_Stylus); pg_send_message_udp((char*)"i", (char*)AuxString, (char*)"udp_TouchOSC_send");
	}
	else {
		sprintf(AuxString, "/StylusvsRubber %d", pg_Rubber); pg_send_message_udp((char*)"i", (char*)AuxString, (char*)"udp_TouchOSC_send");
	}

	// color interface initialization
#ifdef ATELIERSENFANTS
	float control_color[3];
	HSVtoRGB(pen_hue, pen_sat, pen_value, &control_color[0], &control_color[1], &control_color[2]);
	sprintf(AuxString, "/color/couleur/color %02x%02x%02xFF", int(control_color[0] * 255), int(control_color[1] * 255), int(control_color[2] * 255)); pg_send_message_udp((char*)"s", (char*)AuxString, (char*)"udp_TouchOSC_send");
	//printf("color msg HSV %.2f %.2f %.2f  %s\n", pen_hue, pen_sat, pen_value, AuxString);

	HSVtoRGB(repop_huePart, repop_satPart, repop_valuePart, &control_color[0], &control_color[1], &control_color[2]);
	sprintf(AuxString, "/etoiles/part_couleur/color %02x%02x%02xFF", int(control_color[0] * 255), int(control_color[1] * 255), int(control_color[2] * 255)); pg_send_message_udp((char*)"s", (char*)AuxString, (char*)"udp_TouchOSC_send");

	// drawing type initialization
	ExclusiveButtonsAndLabelsOnOff(DessinButtonsPaths, DessinButtonLabelsPaths, DessinButtonValues, true, trkDecay_1);

	// photo diaporama initialization
	ExclusiveButtonsAndLabelsOnOff(FondButtonsPaths, FondButtonLabelsPaths, 
		, true, photo_diaporama);

	// drawing type initialization
	ExclusiveButtonsAndLabelsOnOff(StylusvsRubberButtonsPaths, StylusvsRubberButtonLabelsPaths, StylusvsRubberButtonValues, false, CurrentCursorStylusvsRubber);
#endif
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
				(*ScenarioVarCallbacks[indVar])(_PG_SCENARIO, InitialValuesInterpVar[indVar]);
			}
			else if (ScenarioVarTypes[indVar] == _pg_bool) {
				(*ScenarioVarCallbacks[indVar])(_PG_SCENARIO, (InitialValuesInterpVar[indVar] > 0 ? true : false));
			}
			else if (ScenarioVarTypes[indVar] == _pg_sign) {
				(*ScenarioVarCallbacks[indVar])(_PG_SCENARIO, (InitialValuesInterpVar[indVar] > 0 ? 1.0 : -1.0));
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
	/////////////////////////////////////////////////////////////////////////
	// SCENARIO-CONTROLLED VARIABLES
	for (int indVar = 0; indVar < _MaxInterpVarIDs; indVar++) {
		// the variable has a registered GUI ID in the scenario
		// and its values has changed since it was sent to the GUI last time
		if (*(ScenarioVarMessages[indVar])) {
			if (ScenarioVarTypes[indVar] == _pg_float) {
				float new_value = float(*((float*)ScenarioVarPointers[indVar]));
				switch (ScenarioVarPulse[indVar]) {
				case _pg_pulsed_absolute:
					new_value *= (1.f + pulse_average * float(*((float*)ScenarioVarPointers[indVar + 1])));
					break;
				case _pg_pulsed_uniform:
					new_value += pulse_average * float(*((float*)ScenarioVarPointers[indVar + 1]));
					break;
				case _pg_pulsed_differential:
					new_value *= (pulse_average - pulse_average_prec) * float(*((float*)ScenarioVarPointers[indVar + 1]));
					break;
				}
				if (LastGUIShippedValuesInterpVar[indVar] != new_value || resend_all_variables) {
					sprintf(AuxString, "/%s %.4f", ScenarioVarMessages[indVar], new_value); pg_send_message_udp((char*)"f", AuxString, (char*)"udp_TouchOSC_send");
					LastGUIShippedValuesInterpVar[indVar] = new_value;
					//if (indVar == _repop_grey) {
					//	printf("repop_grey %s %.1f\n", ScenarioVarMessages[indVar], *((float *)ScenarioVarPointers[indVar]));
					//}
				}
			}
			else if (ScenarioVarTypes[indVar] == _pg_int) {
				float new_value = float(*((int*)ScenarioVarPointers[indVar]));
				switch (ScenarioVarPulse[indVar]) {
				case _pg_pulsed_absolute:
					new_value *= (1.f + pulse_average * float(*((float*)ScenarioVarPointers[indVar + 1])));
					break;
				case _pg_pulsed_uniform:
					new_value += pulse_average * float(*((float*)ScenarioVarPointers[indVar + 1]));
					break;
				case _pg_pulsed_differential:
					new_value *= (pulse_average - pulse_average_prec) * float(*((float*)ScenarioVarPointers[indVar + 1]));
					break;
				}
				if (LastGUIShippedValuesInterpVar[indVar] != float(int(new_value)) || resend_all_variables) {
					sprintf(AuxString, "/%s %d", ScenarioVarMessages[indVar], int(new_value)); pg_send_message_udp((char*)"f", AuxString, (char*)"udp_TouchOSC_send");
					// memorizes the shipped value in order to resend it only when changed
					LastGUIShippedValuesInterpVar[indVar] = float(int(new_value));
				}
			}
			else if (ScenarioVarTypes[indVar] == _pg_bool
				&& (LastGUIShippedValuesInterpVar[indVar] != float(*((bool*)ScenarioVarPointers[indVar])) || resend_all_variables)) {
				sprintf(AuxString, "/%s %d", ScenarioVarMessages[indVar], (int)(*((bool*)ScenarioVarPointers[indVar]))); pg_send_message_udp((char*)"f", AuxString, (char*)"udp_TouchOSC_send");
				LastGUIShippedValuesInterpVar[indVar] = float(*((bool*)ScenarioVarPointers[indVar]));
			}
			else if (ScenarioVarTypes[indVar] == _pg_sign
				&& (LastGUIShippedValuesInterpVar[indVar] != float(*((float*)ScenarioVarPointers[indVar])) || resend_all_variables)) {
				sprintf(AuxString, "/%s %.4f", ScenarioVarMessages[indVar], *((float*)ScenarioVarPointers[indVar])); pg_send_message_udp((char*)"f", AuxString, (char*)"udp_TouchOSC_send");
				LastGUIShippedValuesInterpVar[indVar] = float(*((float*)ScenarioVarPointers[indVar]));
			}
			else if (ScenarioVarTypes[indVar] == _pg_path
				&& (LastGUIShippedValuesInterpVar[indVar] != float(*((bool*)ScenarioVarPointers[indVar])) || resend_all_variables)) {
				sprintf(AuxString, "/%s %d", ScenarioVarMessages[indVar], (int)(*((bool*)ScenarioVarPointers[indVar]))); pg_send_message_udp((char*)"f", AuxString, (char*)"udp_TouchOSC_send");
				LastGUIShippedValuesInterpVar[indVar] = float(*((bool*)ScenarioVarPointers[indVar]));
			}
		}
	}
	if (resend_all_variables) {
		if (pg_CurrentSceneIndex >= 0 && pg_CurrentSceneIndex < pg_NbScenes) {
			sprintf(AuxString, "/setup %s", pg_CurrentScene->scene_IDs.c_str()); pg_send_message_udp((char*)"s", AuxString, (char*)"udp_TouchOSC_send");
			sprintf(AuxString, "/setup_1 %s", pg_CurrentScene->scene_Msg1.c_str()); pg_send_message_udp((char*)"s", AuxString, (char*)"udp_TouchOSC_send");
			sprintf(AuxString, "/setup_2 %s", pg_CurrentScene->scene_Msg2.c_str()); pg_send_message_udp((char*)"s", AuxString, (char*)"udp_TouchOSC_send");
		}
		else if (pg_CurrentScene == NULL) {
			sprintf(AuxString, "/setup initial_setup"); pg_send_message_udp((char*)"s", AuxString, (char*)"udp_TouchOSC_send");
			sprintf(AuxString, "/setup_1 ***"); pg_send_message_udp((char*)"s", AuxString, (char*)"udp_TouchOSC_send");
			sprintf(AuxString, "/setup_2 ***"); pg_send_message_udp((char*)"s", AuxString, (char*)"udp_TouchOSC_send");
		}
		else if (pg_CurrentScene == &pg_InterpolationScene) {
			sprintf(AuxString, "/setup interpolation scene"); pg_send_message_udp((char*)"s", AuxString, (char*)"udp_TouchOSC_send");
			sprintf(AuxString, "/setup_1 next_scene_%d", pg_SceneIndexAfterInterpolation); pg_send_message_udp((char*)"s", AuxString, (char*)"udp_TouchOSC_send");
			sprintf(AuxString, "/setup_2 previous_scene_%d", pg_SceneIndexBeforeInterpolation); pg_send_message_udp((char*)"s", AuxString, (char*)"udp_TouchOSC_send");
		}
		sprintf(AuxString, "/pen_colorPreset %d", current_pen_colorPreset); pg_send_message_udp((char*)"i", (char*)AuxString, (char*)"udp_TouchOSC_send");
		InterfaceInitializations();

#ifdef PG_WITH_PUREDATA
		// resends the sound variables to PD (as in the callback)
		sprintf(AuxString, "/sound_env_min %.2f", float(*((float*)ScenarioVarPointers[_sound_env_min])));
		pg_send_message_udp((char*)"f", AuxString, (char*)"udp_PD_send");
		sprintf(AuxString, "/sound_env_max %.2f", float(*((float*)ScenarioVarPointers[_sound_env_max])));
		pg_send_message_udp((char*)"f", AuxString, (char*)"udp_PD_send");
#endif

#if defined (TEXTURED_QUAD_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 
		for (int ind = 0; ind < 3; ind++) {
			if (ind == *((int*)ScenarioVarPointers[_partColor_mode])) {
				sprintf(AuxString, "/partColor_mode_%d 1", ind); pg_send_message_udp((char*)"i", (char*)AuxString, (char*)"udp_TouchOSC_send");
			}
			else {
				sprintf(AuxString, "/partColor_mode_%d 0", ind); pg_send_message_udp((char*)"i", (char*)AuxString, (char*)"udp_TouchOSC_send");
			}
			if (ind == *((int*)ScenarioVarPointers[_partStroke_mode])) {
				sprintf(AuxString, "/partStroke_mode_%d 1", ind); pg_send_message_udp((char*)"i", (char*)AuxString, (char*)"udp_TouchOSC_send");
			}
			else {
				sprintf(AuxString, "/partStroke_mode_%d 0", ind); pg_send_message_udp((char*)"i", (char*)AuxString, (char*)"udp_TouchOSC_send");
			}
			if (ind == *((int*)ScenarioVarPointers[_partExit_mode])) {
				sprintf(AuxString, "/partExit_mode_%d 1", ind); pg_send_message_udp((char*)"i", (char*)AuxString, (char*)"udp_TouchOSC_send");
			}
			else {
				sprintf(AuxString, "/partExit_mode_%d 0", ind); pg_send_message_udp((char*)"i", (char*)AuxString, (char*)"udp_TouchOSC_send");
			}
		}
#endif

		resend_all_variables = false;
	}

	/////////////////////////////////////////////////////////////////////////
	// TIME CONTROLLED VARIABLES (NOT FORWARDED TO SHADERS)
	// time display
	// time exceeded in scene
	sprintf(AuxString, "/volumeDisplay %.2f", pulse_average); pg_send_message_udp((char*)"s", AuxString, (char*)"udp_TouchOSC_send");
	if (remainingTimeInScene >= 0) {
		sprintf(AuxString, "/time %d:%d", (int)remainingTimeInScene / 60, (int)remainingTimeInScene % 60); pg_send_message_udp((char*)"s", AuxString, (char*)"udp_TouchOSC_send");
		if (remainingTimeInScene < 10) {
			sprintf(AuxString, "/time_color_red 1"); pg_send_message_udp((char*)"i", AuxString, (char*)"udp_TouchOSC_send");
			sprintf(AuxString, "/time_color_orange 0"); pg_send_message_udp((char*)"i", AuxString, (char*)"udp_TouchOSC_send");
			sprintf(AuxString, "/time_color_green 0"); pg_send_message_udp((char*)"i", AuxString, (char*)"udp_TouchOSC_send");
		}
		else if (remainingTimeInScene < 20) {
			sprintf(AuxString, "/time_color_red 0"); pg_send_message_udp((char*)"i", AuxString, (char*)"udp_TouchOSC_send");
			sprintf(AuxString, "/time_color_orange 1"); pg_send_message_udp((char*)"i", AuxString, (char*)"udp_TouchOSC_send");
			sprintf(AuxString, "/time_color_green 0"); pg_send_message_udp((char*)"i", AuxString, (char*)"udp_TouchOSC_send");
		}
		else {
			sprintf(AuxString, "/time_color_red 0"); pg_send_message_udp((char*)"i", AuxString, (char*)"udp_TouchOSC_send");
			sprintf(AuxString, "/time_color_orange 0"); pg_send_message_udp((char*)"i", AuxString, (char*)"udp_TouchOSC_send");
			sprintf(AuxString, "/time_color_green 1"); pg_send_message_udp((char*)"i", AuxString, (char*)"udp_TouchOSC_send");
		}
	}
	// time exceeded in scene
	else {
		int remTime = (int)remainingTimeInScene;
		if (remTime > -60) {
			sprintf(AuxString, "/time %d:%d", remTime / 60, remTime % 60); pg_send_message_udp((char*)"s", AuxString, (char*)"udp_TouchOSC_send");
		}
		else {
			sprintf(AuxString, "/time %d:%d", remTime / 60, -(remTime % 60)); pg_send_message_udp((char*)"s", AuxString, (char*)"udp_TouchOSC_send");
		}
		sprintf(AuxString, "/time_color 3"); pg_send_message_udp((char*)"i", AuxString, (char*)"udp_TouchOSC_send");
	}
}

void pg_send_message_udp(char *pattern, char * message, char *targetHostid) {
	pg_IPClient* targetHost = NULL;
	for (int ind = 0; ind < nb_IP_Clients; ind++) {
		if (strcmp(targetHostid, IP_Clients[ind]->id.c_str()) == 0) {
			targetHost = IP_Clients[ind];
			// printf("send_message_udp %s %s %d %d\n", message, pattern, nb_IP_Clients, targetHost);
			targetHost->storeIP_output_message(message, pattern);
		}
	}
	if (!targetHost) {
		// printf( "UDP client unknown %s\n" , targetHostid );
		return;
	}
}

void pg_send_message_udp(char *pattern, char * message, pg_IPClient *targetHost) {
	if (targetHost) {
		// printf("send_message_udp %s %s %d %d\n", message, pattern, nb_IP_Clients, targetHost);
		targetHost->storeIP_output_message(message, pattern);
	}
}

pg_IPClient *pg_UDP_client(char *targetHostid) {
	for (int ind = 0; ind < nb_IP_Clients; ind++) {
		if (strcmp(targetHostid, IP_Clients[ind]->id.c_str()) == 0) {
			return IP_Clients[ind];
		}
	}
	return NULL;
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
			fprintf(pg_csv_file, ",%.6f", *((float *)ScenarioVarPointers[indVar]));
		}
		else if (ScenarioVarTypes[indVar] == _pg_int) {
			fprintf(pg_csv_file, ",%d", *((int *)ScenarioVarPointers[indVar]));
		}
		else if (ScenarioVarTypes[indVar] == _pg_bool) {
			fprintf(pg_csv_file, ",%d", int(*((bool *)ScenarioVarPointers[indVar])));
		}
		else if (ScenarioVarTypes[indVar] == _pg_sign) {
			fprintf(pg_csv_file, ",%d", (*((float *)ScenarioVarPointers[indVar]) > 0 ? +1 : -1));
		}
		else if (ScenarioVarTypes[indVar] == _pg_path) {
			fprintf(pg_csv_file, ",%d", int(*((bool *)ScenarioVarPointers[indVar])));
		}
	}
	fprintf(pg_csv_file, ",%s\n" , fileName);
}

// makes the first line of a CSV log file
void pg_logFirstLineSceneVariables(void) {
	/////////////////////////////////////////////////////////////////////////
	// SCENARIO-CONTROLLED UNIFORM VARIABLES
	fprintf(pg_csv_file, "ID");
	for (int indVar = 0; indVar < _MaxInterpVarIDs; indVar++) {
			fprintf(pg_csv_file, ",%s", CmdString[indVar] );
	}
	fprintf(pg_csv_file, "\n");
}


///////////////////////////////////////////////////////////////////////////////////
// SCENARIO BASED COMMANDS
///////////////////////////////////////////////////////////////////////////////////
#ifdef PG_WITH_PUREDATA
void auto_beat_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	lastBeatTime = CurrentClockTime;
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		// sends a message to PD to stop sending music beats
		sprintf(AuxString, "/auto_beat %d", (int)(*((bool *)ScenarioVarPointers[_auto_beat])));
		pg_send_message_udp((char *)"i", AuxString, (char *)"udp_PD_send");
		// printf("auto_beat [%s] %.2f %d\n", AuxString, scenario_or_gui_command_value, (int)(*((bool *)ScenarioVarPointers[_auto_beat])));
	}
}
void MIDIwithBeat_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		// sends a message to PD to stop sending music beats
		sprintf(AuxString, "/MIDIwithBeat %d", (int)(*((bool *)ScenarioVarPointers[_MIDIwithBeat])));
		pg_send_message_udp((char *)"i", AuxString, (char *)"udp_PD_send");
		// printf("MIDIwithBeat [%s] %.2f %d\n", AuxString, scenario_or_gui_command_value, (int)(*((bool *)ScenarioVarPointers[_MIDIwithBeat])));
	}
}
void MIDIwithColor_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		// sends a message to PD to stop sending music Colors
		sprintf(AuxString, "/MIDIwithColor %d", (int)(*((bool *)ScenarioVarPointers[_MIDIwithColor])));
		pg_send_message_udp((char *)"i", AuxString, (char *)"udp_PD_send");
		// printf("MIDIwithColor [%s] %.2f %d\n", AuxString, scenario_or_gui_command_value, (int)(*((bool *)ScenarioVarPointers[_MIDIwithColor])));
	}
}
void MIDIwithBrush_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		// sends a message to PD to stop sending music Brushs
		sprintf(AuxString, "/MIDIwithBrush %d", (int)(*((bool *)ScenarioVarPointers[_MIDIwithBrush])));
		pg_send_message_udp((char *)"i", AuxString, (char *)"udp_PD_send");
		// printf("MIDIwithBrush [%s] %.2f %d\n", AuxString, scenario_or_gui_command_value, (int)(*((bool *)ScenarioVarPointers[_MIDIwithBrush])));
	}
}
void MIDIwithCameraFlash_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		// sends a message to PD to stop sending music Flashs
		sprintf(AuxString, "/MIDIwithCameraFlash %d", (int)(*((bool *)ScenarioVarPointers[_MIDIwithCameraFlash])));
		pg_send_message_udp((char *)"i", AuxString, (char *)"udp_PD_send");
		// printf("MIDIwithFlash [%s] %.2f %d\n", AuxString, scenario_or_gui_command_value, (int)(*((bool *)ScenarioVarPointers[_MIDIwithFlash])));
	}
}
void MIDIwithPhotoFlash_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		// sends a message to PD to stop sending music Flashs
		sprintf(AuxString, "/MIDIwithPhotoFlash %d", (int)(*((bool *)ScenarioVarPointers[_MIDIwithPhotoFlash])));
		pg_send_message_udp((char *)"i", AuxString, (char *)"udp_PD_send");
		// printf("MIDIwithFlash [%s] %.2f %d\n", AuxString, scenario_or_gui_command_value, (int)(*((bool *)ScenarioVarPointers[_MIDIwithFlash])));
	}
}
void auto_pulse_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		// sends a message to PD to stop sending music pulse
		sprintf(AuxString, "/auto_pulse %d", (int)(*((bool *)ScenarioVarPointers[_auto_pulse])));
		pg_send_message_udp((char *)"i", AuxString, (char *)"udp_PD_send");
		// printf("auto_pulse [%s] %.2f %d\n", AuxString, scenario_or_gui_command_value, (int)(*((bool *)ScenarioVarPointers[_auto_pulse])));
		for (int ind = 0; ind < 3 * 2; ind++) {
			seed_pulsePerlinNoise[ind] = rand_0_1 * 255;
		}
	}
}
void beat_delay_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		sprintf(AuxString, "/beat_delay %.5f", scenario_or_gui_command_value);
		pg_send_message_udp((char *)"f", AuxString, (char *)"udp_PD_send");
	}
}
void beat_threshold_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		sprintf(AuxString, "/beat_threshold %.5f", scenario_or_gui_command_value);
		pg_send_message_udp((char *)"f", AuxString, (char *)"udp_PD_send");
	}
}
#endif
#ifndef PEN_HSV
void pen_color_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		sprintf(AuxString, "/pen_colorPreset -1.0");
		pg_send_message_udp((char*)"f", AuxString, (char*)"udp_TouchOSC_send");
	}
}
#endif
#if defined(PG_WITH_PUREDATA) || defined(PG_WITH_JUCE)
void audioInput_weight_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		audioInput_weight = float(scenario_or_gui_command_value);
#ifdef PG_WITH_PUREDATA
		sprintf(AuxString, "/audioInput_weight %.2f", audioInput_weight);
		pg_send_message_udp((char *)"f", AuxString, (char *)"udp_PD_send");
#endif
#ifdef PG_WITH_JUCE
		sprintf(AuxString, "/JUCE_audioInput_weight %.2f", audioInput_weight);
		pg_send_message_udp((char *)"f", AuxString, (char *)"udp_SoundJUCE_send");
#endif
	}
}
#endif
#if defined(PG_WITH_PUREDATA) || defined(PG_WITH_JUCE)
void soundtrack_weight_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		soundtrack_weight = float(scenario_or_gui_command_value);
#ifdef PG_WITH_PUREDATA
		sprintf(AuxString, "/soundtrack_weight %.2f", soundtrack_weight);
		pg_send_message_udp((char *)"f", AuxString, (char *)"udp_PD_send");
#endif
#ifdef PG_WITH_JUCE
		sprintf(AuxString, "/JUCE_soundtrack_weight %.2f", soundtrack_weight);
		pg_send_message_udp((char *)"f", AuxString, (char *)"udp_SoundJUCE_send");
#endif
	}
}
#endif

#ifdef ATELIERSENFANTS
void pen_hue_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		float control_color[3];
		HSVtoRGB(pen_hue, pen_sat, pen_value, &control_color[0], &control_color[1], &control_color[2]);
		sprintf(AuxString, "/color/couleur/color %02x%02x%02xFF", int(control_color[0] * 255), int(control_color[1] * 255), int(control_color[2] * 255)); pg_send_message_udp((char*)"s", (char*)AuxString, (char*)"udp_TouchOSC_send");
		//printf("colorH msg HSV %.2f %.2f %.2f  %s\n", pen_hue, pen_sat, pen_value, AuxString);
	}
}
void pen_sat_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		float control_color[3];
		HSVtoRGB(pen_hue, pen_sat, pen_value, &control_color[0], &control_color[1], &control_color[2]);
		sprintf(AuxString, "/color/couleur/color %02x%02x%02xFF", int(control_color[0] * 255), int(control_color[1] * 255), int(control_color[2] * 255)); pg_send_message_udp((char*)"s", (char*)AuxString, (char*)"udp_TouchOSC_send");
		//printf("colorS msg HSV %.2f %.2f %.2f  %s\n", pen_hue, pen_sat, pen_value, AuxString);
	}
}
void pen_value_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		float control_color[3];
		HSVtoRGB(pen_hue, pen_sat, pen_value, &control_color[0], &control_color[1], &control_color[2]);
		sprintf(AuxString, "/color/couleur/color %02x%02x%02xFF", int(control_color[0] * 255), int(control_color[1] * 255), int(control_color[2] * 255)); pg_send_message_udp((char*)"s", (char*)AuxString, (char*)"udp_TouchOSC_send");
		//printf("colorV msg HSV %.2f %.2f %.2f  %s\n", pen_hue, pen_sat, pen_value, AuxString);
	}
}
void repop_huePart_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		float control_color[3];
		HSVtoRGB(repop_huePart, repop_satPart, repop_valuePart, &control_color[0], &control_color[1], &control_color[2]);
		sprintf(AuxString, "/etoiles/part_couleur/color %02x%02x%02xFF", int(control_color[0] * 255), int(control_color[1] * 255), int(control_color[2] * 255)); pg_send_message_udp((char*)"s", (char*)AuxString, (char*)"udp_TouchOSC_send");
		//printf("colorH msg HSV %.2f %.2f %.2f  %s\n", repop_huePart, repop_satPart, repop_valuePart, AuxString);
	}
}
void repop_satPart_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		float control_color[3];
		HSVtoRGB(repop_huePart, repop_satPart, repop_valuePart, &control_color[0], &control_color[1], &control_color[2]);
		sprintf(AuxString, "/etoiles/part_couleur/color %02x%02x%02xFF", int(control_color[0] * 255), int(control_color[1] * 255), int(control_color[2] * 255)); pg_send_message_udp((char*)"s", (char*)AuxString, (char*)"udp_TouchOSC_send");
		//printf("colorS msg HSV %.2f %.2f %.2f  %s\n", repop_huePart, repop_satPart, repop_valuePart, AuxString);
	}
}
void repop_valuePart_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		float control_color[3];
		HSVtoRGB(repop_huePart, repop_satPart, repop_valuePart, &control_color[0], &control_color[1], &control_color[2]);
		sprintf(AuxString, "/etoiles/part_couleur/color %02x%02x%02xFF", int(control_color[0] * 255), int(control_color[1] * 255), int(control_color[2] * 255)); pg_send_message_udp((char*)"s", (char*)AuxString, (char*)"udp_TouchOSC_send");
		//printf("colorV msg HSV %.2f %.2f %.2f  %s\n", repop_huePart, repop_satPart, repop_valuePart, AuxString);
	}
}
void trkDecay_1_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		// drawing type initialization
		ExclusiveButtonsAndLabelsOnOff(DessinButtonsPaths, DessinButtonLabelsPaths, DessinButtonValues, true, trkDecay_1);
	}
}
#endif
#ifdef ATELIERSENFANTS
void fingers_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		// drawing type initialization
		NumberOfInteractionFingers(int(scenario_or_gui_command_value));
	}
}
#endif

void cameraExposure_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
#ifdef PG_WITH_CAMERA_CAPTURE
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		// sprintf(AuxString, "/message Exposure_%.1f", scenario_or_gui_command_value); pg_send_message_udp((char *)"s", (char *)AuxString, (char *)"udp_TouchOSC_send");
		if (scenario_or_gui_command_value != CameraCurrent_exposure) {
			//printf("Cam exposure %.2f\n", scenario_or_gui_command_value);
			// cvSetCaptureProperty comment for see3cam
			pg_camera_capture.set(CV_CAP_PROP_EXPOSURE, scenario_or_gui_command_value);
			CameraCurrent_exposure = float(scenario_or_gui_command_value);
			pg_LastCameraParameterChange_Frame = pg_FrameNo;
		}
	}
#endif
}
#if defined(CAVERNEPLATON)
void Caverne_Mesh_Profusion_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		//printf("/Caverne_Mesh_Profusion %.1f\n", scenario_or_gui_command_value);
		if (scenario_or_gui_command_value == 1.) {
			for (int indMesh = 7; indMesh < pg_nb_Mesh_files; indMesh++) {
				Caverne_Mesh_Profusion_On(indMesh);
			}
		}
		else if (scenario_or_gui_command_value == 0.) {
			for (int indMesh = 7; indMesh < pg_nb_Mesh_files; indMesh++) {
				Caverne_Mesh_Profusion_Off(indMesh);
			}
		}
	}
}
#endif
void cameraGamma_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
#ifdef PG_WITH_CAMERA_CAPTURE
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		// sprintf(AuxString, "/message Gamma%.1f", scenario_or_gui_command_value); pg_send_message_udp((char *)"s", (char *)AuxString, (char *)"udp_TouchOSC_send");
		if (scenario_or_gui_command_value != CameraCurrent_gamma) {
			//printf("Cam gamma %.2f\n", scenario_or_gui_command_value);
			// cvSetCaptureProperty comment for see3cam
			pg_camera_capture.set(CV_CAP_PROP_GAMMA, scenario_or_gui_command_value);
			CameraCurrent_gamma = float(scenario_or_gui_command_value);
			pg_LastCameraParameterChange_Frame = pg_FrameNo;
		}
	}
#endif
}
void cameraGain_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
#ifdef PG_WITH_CAMERA_CAPTURE
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		if (scenario_or_gui_command_value != CameraCurrent_gain) {
			printf("Cam gain %.2f\n", scenario_or_gui_command_value);
			// cvSetCaptureProperty comment for see3cam 
			pg_camera_capture.set(CV_CAP_PROP_GAIN, scenario_or_gui_command_value);
			CameraCurrent_gain = float(scenario_or_gui_command_value);
			pg_LastCameraParameterChange_Frame = pg_FrameNo;
		}
	}
#endif
}
void cameraBrightness_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
#ifdef PG_WITH_CAMERA_CAPTURE
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		if (scenario_or_gui_command_value != CameraCurrent_brightness) {
			//printf("Cam brightness %.2f\n", scenario_or_gui_command_value);
			// cvSetCaptureProperty comment for see3cam
			pg_camera_capture.set(CV_CAP_PROP_BRIGHTNESS, scenario_or_gui_command_value);
			CameraCurrent_brightness = float(scenario_or_gui_command_value);
			pg_LastCameraParameterChange_Frame = pg_FrameNo;
		}
	}
#endif
}
void cameraSaturation_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
#ifdef PG_WITH_CAMERA_CAPTURE
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		if (scenario_or_gui_command_value != CameraCurrent_saturation) {
			//printf("Cam saturation %.2f\n", scenario_or_gui_command_value);
			// cvSetCaptureProperty comment for see3cam 
			pg_camera_capture.set(CV_CAP_PROP_SATURATION, scenario_or_gui_command_value);
			CameraCurrent_saturation = float(scenario_or_gui_command_value);
			pg_LastCameraParameterChange_Frame = pg_FrameNo;
		}
	}
#endif
}
void cameraContrast_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
#ifdef PG_WITH_CAMERA_CAPTURE
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		if (scenario_or_gui_command_value != CameraCurrent_contrast) {
			//printf("Cam contrast %.2f\n", scenario_or_gui_command_value);
			// cvSetCaptureProperty comment for see3cam
			pg_camera_capture.set(CV_CAP_PROP_CONTRAST, scenario_or_gui_command_value);
			CameraCurrent_contrast = float(scenario_or_gui_command_value);
			pg_LastCameraParameterChange_Frame = pg_FrameNo;
		}
	}
#endif
}
void cameraWB_B_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
#ifdef PG_WITH_CAMERA_CAPTURE
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		if (scenario_or_gui_command_value != CameraCurrent_WB_B) {
			//printf("Cam WB B %.2f\n", scenario_or_gui_command_value);
			// cvSetCaptureProperty comment for see3cam
			pg_camera_capture.set(CV_CAP_PROP_WHITE_BALANCE_BLUE_U, scenario_or_gui_command_value);
			CameraCurrent_WB_B = float(scenario_or_gui_command_value);
			pg_LastCameraParameterChange_Frame = pg_FrameNo;
		}
	}
#endif
}
void cameraWB_R_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
#ifdef PG_WITH_CAMERA_CAPTURE
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		if (scenario_or_gui_command_value != CameraCurrent_WB_R) {
			//printf("Cam WB R %.2f\n", scenario_or_gui_command_value);
			// cvSetCaptureProperty comment for see3cam
			if (scenario_or_gui_command_value >= 0) {
				pg_camera_capture.set(CV_CAP_PROP_WHITE_BALANCE_RED_V, scenario_or_gui_command_value);
				CameraCurrent_WB_R = float(scenario_or_gui_command_value);
				pg_LastCameraParameterChange_Frame = pg_FrameNo;
			}
		}
	}
#endif
}
#if defined(PG_WITH_JUCE) || defined(PG_WITH_PUREDATA)
void playing_soundtrackNo_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		// play a new track
		if (playing_soundtrackNo >= 0
			&& playing_soundtrackNo < nb_soundtracks) {
//			soundTrack_on = true;
#ifdef PG_WITH_PUREDATA
//			sprintf(AuxString, "/soundtrack_onOff %d", soundTrack_on);
//			pg_send_message_udp((char *)"i", AuxString, (char *)"udp_PD_send");
#endif
//			sprintf(AuxString, "/soundtrack_onOff %d", !soundTrack_on);
//			pg_send_message_udp((char *)"i", AuxString, (char *)"udp_TouchOSC_send");
			currentlyPlaying_trackNo = playing_soundtrackNo;
			PlayTrack(currentlyPlaying_trackNo);
		}
		// stop playing a track
		else if (playing_soundtrackNo < 0) {
			currentlyPlaying_trackNo = playing_soundtrackNo;
			StopTrack();
		}
	}
}
#endif
void playing_movieNo_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		// starts playing another video
		pg_play_movie_no();

		// if the video no is outside the range of available videos: stops the playing video
		if (playing_movieNo != currentlyPlaying_movieNo
			&& playing_movieNo < 0 || playing_movieNo >= nb_movies) {
			// printf("VideoPb Video initialization inside callback (thread) \n");

			currentlyPlaying_movieNo = -1;
			movie_on = false;
			sprintf(AuxString, "/movie_onOff %d", !movie_on);
			pg_send_message_udp((char *)"i", AuxString, (char *)"udp_TouchOSC_send");
			sprintf(AuxString, "/movie_shortName %s", "---");
			pg_send_message_udp((char *)"s", AuxString, (char *)"udp_TouchOSC_send");
		}
	}
}

void pg_play_movie_no(void) {
	if (playing_movieNo >= 0 && playing_movieNo < nb_movies
		&& playing_movieNo != currentlyPlaying_movieNo) {
		// printf("VideoPb Video initialization inside command movie+ (thread) \n");

		BrokenInterpolationVar[_playing_movieNo] = true;
		currentlyPlaying_movieNo = playing_movieNo;
		movie_on = true;
		sprintf(AuxString, "/movie_onOff %d", !movie_on);
		pg_send_message_udp((char*)"i", AuxString, (char*)"udp_TouchOSC_send");
		sprintf(AuxString, "/movie_shortName %s", movieShortName[playing_movieNo].c_str());
		pg_send_message_udp((char*)"s", AuxString, (char*)"udp_TouchOSC_send");

		// texture ID initialization (should not be inside a thread)
		if (pg_movie_texture_texID == NULL_ID) {
			glGenTextures(1, &pg_movie_texture_texID);
		}

		is_movieLoading = true;
		printf("Loading %s\n", movieFileName[currentlyPlaying_movieNo].c_str());
		// sprintf(AuxString, "/message %s", movieFileName[currentlyPlaying_movieNo].c_str()); pg_send_message_udp((char *)"s", (char *)AuxString, (char *)"udp_TouchOSC_send");
#if defined(TEMPETE) || defined(DAWN) || defined(RIVETS)
		pg_initVideoMoviePlayback_nonThreaded(&movieFileName[currentlyPlaying_movieNo]);
#else
#ifdef WIN32
		DWORD rc;
		HANDLE  hThread = CreateThread(
			NULL,                   // default security attributes
			0,                      // use default stack size  
			pg_initVideoMoviePlayback,		    // thread function name
			(void*)(&movieFileName[currentlyPlaying_movieNo]),		    // argument to thread function 
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
			(void*)(&movieFileName[currentlyPlaying_movieNo]));
		if (rc) {
			std::cout << "Error:unable to create thread pg_initVideoMoviePlayback" << rc << std::endl;
			exit(-1);
		}
		pthread_exit(NULL);
#endif
#endif
}
}

#if defined(CAVERNEPLATON)
void extern_movieNo_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		if (extern_movieNo != current_extern_movieNo) {
			current_extern_movieNo = extern_movieNo;
			sprintf(AuxString, "/video %.0f", extern_movieNo);
			pg_send_message_udp((char *)"f", AuxString, (char *)"udp_Processing_send");
		}
	}
}
#endif
void photo_diaporama_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_KEYSTROKE || param_input_type == _PG_SCENARIO) {
		//printf("photo_diaporama_callBack %.2f\n", scenario_or_gui_command_value);
		if (pg_nbCompressedImageDirs > 0 && scenario_or_gui_command_value >= 0 && scenario_or_gui_command_value != pg_CurrentDiaporamaDir) {
			pg_CurrentDiaporamaDir = int(scenario_or_gui_command_value) % pg_nbCompressedImageDirs;
			//printf("pg_CurrentDiaporamaDir %d\n", pg_CurrentDiaporamaDir);
			sprintf(AuxString, "/diaporama_shortName %03d", pg_CurrentDiaporamaDir);
			pg_send_message_udp((char *)"s", AuxString, (char *)"udp_TouchOSC_send");
			pg_launch_diaporama();
		}
		else if (pg_nbCompressedImageDirs > 0 && scenario_or_gui_command_value < 0) {
			pg_CurrentDiaporamaDir = -1;
			//printf("pg_CurrentDiaporamaDir %d\n", pg_CurrentDiaporamaDir);
			sprintf(AuxString, "/diaporama_shortName %03d", pg_CurrentDiaporamaDir);
			pg_send_message_udp((char *)"s", AuxString, (char *)"udp_TouchOSC_send");
			pg_launch_diaporama();
		}
		// do nothing / no images to display
		else {
		}
#ifdef ATELIERSENFANTS
		// photo diaporama initialization
		printf("current photo diaporama %d\n", int(scenario_or_gui_command_value));
		ExclusiveButtonsAndLabelsOnOff(FondButtonsPaths, FondButtonLabelsPaths, FondButtonValues, true, int(scenario_or_gui_command_value));
#endif
	}
}
void flashCameraTrkLength_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_KEYSTROKE || param_input_type == _PG_SCENARIO) {
		if (scenario_or_gui_command_value > 0.) {
			flashCameraTrk_decay = float(1. / scenario_or_gui_command_value);
		}
		else {
			flashCameraTrk_decay = 0.001f;
		}
	}
}
#ifdef PG_WITH_PHOTO_FLASH
void flashPhotoTrkLength_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_KEYSTROKE || param_input_type == _PG_SCENARIO) {
		if (scenario_or_gui_command_value > 0.) {
			flashPhotoTrk_decay = 1.f / float(scenario_or_gui_command_value);
		}
		else {
			flashPhotoTrk_decay = 0.001f;
		}
	}
}
#endif
void pen_brush_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_KEYSTROKE || param_input_type == _PG_SCENARIO) {
		if (scenario_or_gui_command_value >= 1) { // large radius for the image brushes*
			pen_radiusMultiplier = 50.0f;
#if defined(PIERRES) || defined(ATELIERSENFANTS) || defined(SONG)
			pen_radiusMultiplier = 1.0f;
#endif
		}
		else {
			pen_radiusMultiplier = 1.0f;
		}
		// printf("pen_radiusMultiplier %.2f\n", pen_radiusMultiplier);
	}
}
void clearAllLayers_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_KEYSTROKE) {
		// printf("delta %.3f\n", (float)(CurrentClockTime - lastClearTime) );
		if (CurrentClockTime - lastClearTime < 0.5f) {
			// pg_send_message_udp((char *)"s", (char *)"/message clear_all", (char *)"udp_TouchOSC_send");
			isClearAllLayers = 1;
			lastClearTime = 0.0f;
			printf("****** isClearAllLayers %d\n",isClearAllLayers);
		}
		else {
			lastClearTime = CurrentClockTime;
		}
	}
	else if (param_input_type == _PG_SCENARIO && scenario_or_gui_command_value > 0.0) {
		// pg_send_message_udp((char *)"s", (char *)"/message clear_all", (char *)"udp_TouchOSC_send");
		isClearAllLayers = 1;
		lastClearTime = CurrentClockTime;
		// printf("****** isClearAllLayers %d\n", isClearAllLayers);
	}
}
void clearLayer_callBack( pg_Parameter_Input_Type param_input_type , double scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_KEYSTROKE) {
		if( CurrentClockTime - lastClearTime  < 0.2f ) {
			isClearLayer = 1;
			lastClearTime = 0.0f;
			// pg_send_message_udp((char *)"s", (char *)"/message clear_all_layers", (char *)"udp_TouchOSC_send");
		}
		else {
			isClearLayer = 1;
			lastClearTime = CurrentClockTime;
			// pg_send_message_udp((char *)"s", (char *)"/message clear_layer", (char *)"udp_TouchOSC_send");
		}
	}
	else if (param_input_type == _PG_SCENARIO && scenario_or_gui_command_value > 0.0 ) {
		isClearLayer = 1;
		lastClearTime = CurrentClockTime;
	}
}
#ifdef PG_WITH_PUREDATA
void sound_env_min_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		sprintf(AuxString, "/sound_env_min %.2f", scenario_or_gui_command_value);
		pg_send_message_udp((char *)"f", AuxString, (char *)"udp_PD_send");
	}
	// printf("reset sound\n");
}
void sound_env_max_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		sprintf(AuxString, "/sound_env_max %.2f", scenario_or_gui_command_value);
		pg_send_message_udp((char *)"f", AuxString, (char *)"udp_PD_send");
	}
	// printf("reset sound\n");
}
#endif
#ifdef PG_MESHES
void activeMeshes_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_KEYSTROKE || param_input_type == _PG_SCENARIO) {
		// MESH INTERFACE VARIABLE INITIALIZATION
		for (int indImage = 0; indImage < pg_nb_Mesh_files; indImage++) {
			sprintf(AuxString, "/Mesh_%d_onOff %d", indImage + 1, (activeMeshes & (1 << (indImage)))); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
		}
	}
}
void mobileMeshes_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_KEYSTROKE || param_input_type == _PG_SCENARIO) {
		// MESH INTERFACE VARIABLE INITIALIZATION
		for (int indImage = 0; indImage < pg_nb_Mesh_files; indImage++) {
			sprintf(AuxString, "/Mesh_mobile_%d_onOff %d", indImage + 1, (mobileMeshes & (1 << (indImage)))); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
		}
	}
}
#endif
void clearCA_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_KEYSTROKE) {
		// pg_send_message_udp((char *)"s", (char *)"/message clear_CA", (char *)"udp_TouchOSC_send");
		isClearCA = 1;
	}
	else if (param_input_type == _PG_SCENARIO && scenario_or_gui_command_value > 0.0) {
		isClearCA = 1;
	}
}
void clearEcho_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_KEYSTROKE) {
		// pg_send_message_udp((char *)"s", (char *)"/message clear_CA", (char *)"udp_TouchOSC_send");
		isClearEcho = 1;
	}
	else if (param_input_type == _PG_SCENARIO && scenario_or_gui_command_value > 0.0) {
		isClearEcho = 1;
	}
}
void path_replay_trackNo_callBack(int pathNo, pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	int playing_track = -1;
	// for a keystroke or a GUI, the command is the current track whatever its value
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_KEYSTROKE) {
		playing_track = currentDrawingTrack;
	}
	// for a scenario >= 0 -> replay and -1 -> no replay
	else if (param_input_type == _PG_SCENARIO) {
		playing_track = int(scenario_or_gui_command_value);
	}
 	// is not currently reading -> starts reading if it is a valid track number
	if (!is_path_replay[pathNo]) {
		// does not change anything if it is not a valid track
		if (playing_track < 0 || playing_track >= PG_NB_TRACKS) {
			sprintf(AuxString, "/path_replay_trackNo_%d 0", pathNo);
			pg_send_message_udp((char *)"i", AuxString, (char *)"udp_TouchOSC_send");
			//printf("replay unchanged (stop) invalid track\n");
			return;
		}
		// stops recording if recording is on
		bool isTrackRecord = false;
		switch (pathNo) {
#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7 || PG_NB_PATHS == 11
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
#if PG_NB_PATHS == 7 || PG_NB_PATHS == 11
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
#if PG_NB_PATHS == 11
		case 8:
			isTrackRecord = path_record_8;
			break;
		case 9:
			isTrackRecord = path_record_9;
			break;
		case 10:
			isTrackRecord = path_record_10;
			break;
		case 11:
			isTrackRecord = path_record_11;
			break;
#endif
		}
		// is currently recording -> stops recording 
		if (isTrackRecord) {
			// stops recording 
			pg_path_recording_onOff(pathNo);
			//printf("replay stops recording\n");
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
			pg_send_message_udp((char *)"i", AuxString, (char *)"udp_TouchOSC_send");
			//printf("replay on recorded track (%s)\n", AuxString);
		}
		else {
			sprintf(AuxString, "/path_replay_trackNo_%d 0", pathNo);
			pg_send_message_udp((char *)"i", AuxString, (char *)"udp_TouchOSC_send");
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
			pg_send_message_udp((char *)"i", AuxString, (char *)"udp_TouchOSC_send");
			//printf("replay off track was currently read (%s)\n", AuxString);
		}
	}
}

#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7 || PG_NB_PATHS == 11
void path_replay_trackNo_1_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	path_replay_trackNo_callBack(1, param_input_type, scenario_or_gui_command_value);
}
void path_replay_trackNo_2_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	path_replay_trackNo_callBack(2, param_input_type, scenario_or_gui_command_value);
}
void path_replay_trackNo_3_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	path_replay_trackNo_callBack(3, param_input_type, scenario_or_gui_command_value);
}
#endif
#if PG_NB_PATHS == 7 || PG_NB_PATHS == 11
void path_replay_trackNo_4_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	path_replay_trackNo_callBack(4, param_input_type, scenario_or_gui_command_value);
}
void path_replay_trackNo_5_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	path_replay_trackNo_callBack(5, param_input_type, scenario_or_gui_command_value);
}
void path_replay_trackNo_6_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	path_replay_trackNo_callBack(6, param_input_type, scenario_or_gui_command_value);
}
void path_replay_trackNo_7_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	path_replay_trackNo_callBack(7, param_input_type, scenario_or_gui_command_value);
}
#endif
#if PG_NB_PATHS == 11
void path_replay_trackNo_8_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	path_replay_trackNo_callBack(8, param_input_type, scenario_or_gui_command_value);
}
void path_replay_trackNo_9_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	path_replay_trackNo_callBack(9, param_input_type, scenario_or_gui_command_value);
}
void path_replay_trackNo_10_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	path_replay_trackNo_callBack(10, param_input_type, scenario_or_gui_command_value);
}
void path_replay_trackNo_11_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	path_replay_trackNo_callBack(11, param_input_type, scenario_or_gui_command_value);
}
#endif
void path_record_callBack(int pathNo, pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	bool isTrackRecord = false;
	switch (pathNo) {
#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7 || PG_NB_PATHS == 11
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
#if PG_NB_PATHS == 7 || PG_NB_PATHS == 11
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
#if PG_NB_PATHS == 11
	case 8:
		isTrackRecord = path_record_8;
		break;
	case 9:
		isTrackRecord = path_record_9;
		break;
	case 10:
		isTrackRecord = path_record_10;
		break;
	case 11:
		isTrackRecord = path_record_11;
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
	//printf("path record for path %d (recorded %d)\n",
	//	pathNo, recorded_path[pathNo]);
}
#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7 || PG_NB_PATHS == 11
void path_record_1_callBack( pg_Parameter_Input_Type param_input_type , double scenario_or_gui_command_value ) {
  path_record_callBack( 1 , param_input_type , scenario_or_gui_command_value);
}
void path_record_2_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	path_record_callBack(2, param_input_type, scenario_or_gui_command_value);
}
void path_record_3_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	path_record_callBack(3, param_input_type, scenario_or_gui_command_value);
}
#endif
#if PG_NB_PATHS == 7 || PG_NB_PATHS == 11
void path_record_4_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	path_record_callBack(4, param_input_type, scenario_or_gui_command_value);
}
void path_record_5_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	path_record_callBack(5, param_input_type, scenario_or_gui_command_value);
}
void path_record_6_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	path_record_callBack(6, param_input_type, scenario_or_gui_command_value);
}
void path_record_7_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	path_record_callBack(7, param_input_type, scenario_or_gui_command_value);
}
#endif
#if PG_NB_PATHS == 11
void path_record_8_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	path_record_callBack(8, param_input_type, scenario_or_gui_command_value);
}
void path_record_9_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	path_record_callBack(9, param_input_type, scenario_or_gui_command_value);
}
void path_record_10_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	path_record_callBack(10, param_input_type, scenario_or_gui_command_value);
}
void path_record_11_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	path_record_callBack(11, param_input_type, scenario_or_gui_command_value);
}
#endif
#if defined (TEXTURED_QUAD_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 
void part_move_init(void) {
#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7 || PG_NB_PATHS == 11
	*((bool *)ScenarioVarPointers[_part_path_follow_0]) = false;
	*((bool *)ScenarioVarPointers[_part_path_follow_1]) = false;
	*((bool *)ScenarioVarPointers[_part_path_follow_2]) = false;
	*((bool *)ScenarioVarPointers[_part_path_follow_3]) = false;
	*((bool *)ScenarioVarPointers[_part_path_repulse_0]) = false;
	*((bool *)ScenarioVarPointers[_part_path_repulse_1]) = false;
	*((bool *)ScenarioVarPointers[_part_path_repulse_2]) = false;
	*((bool *)ScenarioVarPointers[_part_path_repulse_3]) = false;
#endif
#if PG_NB_PATHS == 7 || PG_NB_PATHS == 11
	*((bool*)ScenarioVarPointers[_part_path_follow_4]) = false;
	*((bool*)ScenarioVarPointers[_part_path_follow_5]) = false;
	*((bool*)ScenarioVarPointers[_part_path_follow_6]) = false;
	*((bool*)ScenarioVarPointers[_part_path_follow_7]) = false;
	*((bool*)ScenarioVarPointers[_part_path_repulse_4]) = false;
	*((bool*)ScenarioVarPointers[_part_path_repulse_5]) = false;
	*((bool*)ScenarioVarPointers[_part_path_repulse_6]) = false;
	*((bool*)ScenarioVarPointers[_part_path_repulse_7]) = false;
#endif
#if PG_NB_PATHS == 11
	*((bool*)ScenarioVarPointers[_part_path_follow_8]) = false;
	*((bool*)ScenarioVarPointers[_part_path_follow_9]) = false;
	*((bool*)ScenarioVarPointers[_part_path_follow_10]) = false;
	*((bool*)ScenarioVarPointers[_part_path_follow_11]) = false;
	*((bool*)ScenarioVarPointers[_part_path_repulse_8]) = false;
	*((bool*)ScenarioVarPointers[_part_path_repulse_9]) = false;
	*((bool*)ScenarioVarPointers[_part_path_repulse_10]) = false;
	*((bool*)ScenarioVarPointers[_part_path_repulse_11]) = false;
#endif
}
void part_path_follow_callBack(int pathNo, pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	bool state = false;
#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7 || PG_NB_PATHS == 11
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
#if PG_NB_PATHS == 7 || PG_NB_PATHS == 11
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
#if PG_NB_PATHS == 11
	switch (pathNo) {
	case 4:
		state = part_path_follow_8;
		break;
	case 5:
		state = part_path_follow_9;
		break;
	case 6:
		state = part_path_follow_10;
		break;
	case 7:
		state = part_path_follow_11;
		break;
	}
#endif
	if (!state)
		return; // nothing to do for null values
	part_move_init();
#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7 || PG_NB_PATHS == 11
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
#if PG_NB_PATHS == 7 || PG_NB_PATHS == 11
	switch (pathNo) {
	case 4:
		*((bool*)ScenarioVarPointers[_part_path_follow_4]) = state;
		break;
	case 5:
		*((bool*)ScenarioVarPointers[_part_path_follow_5]) = state;
		break;
	case 6:
		*((bool*)ScenarioVarPointers[_part_path_follow_6]) = state;
		break;
	case 7:
		*((bool*)ScenarioVarPointers[_part_path_follow_7]) = state;
		break;
	}
#endif
#if PG_NB_PATHS == 11
	switch (pathNo) {
	case 4:
		*((bool*)ScenarioVarPointers[_part_path_follow_8]) = state;
		break;
	case 5:
		*((bool*)ScenarioVarPointers[_part_path_follow_9]) = state;
		break;
	case 6:
		*((bool*)ScenarioVarPointers[_part_path_follow_10]) = state;
		break;
	case 7:
		*((bool*)ScenarioVarPointers[_part_path_follow_11]) = state;
		break;
	}
#endif
}
void part_path_repulse_callBack(int pathNo, pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	bool state = false;
#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7 || PG_NB_PATHS == 11
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
#if PG_NB_PATHS == 7 || PG_NB_PATHS == 11
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
#if PG_NB_PATHS == 11
	switch (pathNo) {
	case 4:
		state = part_path_repulse_8;
		break;
	case 5:
		state = part_path_repulse_9;
		break;
	case 6:
		state = part_path_repulse_10;
		break;
	case 7:
		state = part_path_repulse_11;
		break;
	}
#endif
	if (!state)
		return; // nothing to do for null values
	part_move_init();
#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7 || PG_NB_PATHS == 11
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
#if PG_NB_PATHS == 7 || PG_NB_PATHS == 11
	switch (pathNo) {
	case 4:
		*((bool*)ScenarioVarPointers[_part_path_repulse_4]) = state;
		break;
	case 5:
		*((bool*)ScenarioVarPointers[_part_path_repulse_5]) = state;
		break;
	case 6:
		*((bool*)ScenarioVarPointers[_part_path_repulse_6]) = state;
		break;
	case 7:
		*((bool*)ScenarioVarPointers[_part_path_repulse_7]) = state;
		break;
	}
#endif
#if PG_NB_PATHS == 11
	switch (pathNo) {
	case 4:
		*((bool*)ScenarioVarPointers[_part_path_repulse_8]) = state;
		break;
	case 5:
		*((bool*)ScenarioVarPointers[_part_path_repulse_9]) = state;
		break;
	case 6:
		*((bool*)ScenarioVarPointers[_part_path_repulse_10]) = state;
		break;
	case 7:
		*((bool*)ScenarioVarPointers[_part_path_repulse_11]) = state;
		break;
}
#endif
}
#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7 || PG_NB_PATHS == 11
void part_path_follow_0_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	part_path_follow_callBack(0, param_input_type, scenario_or_gui_command_value);
}
void part_path_follow_1_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	part_path_follow_callBack(1, param_input_type, scenario_or_gui_command_value);
}
void part_path_follow_2_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	part_path_follow_callBack(2, param_input_type, scenario_or_gui_command_value);
}
void part_path_follow_3_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	part_path_follow_callBack(3, param_input_type, scenario_or_gui_command_value);
}
#endif
#if PG_NB_PATHS == 7 || PG_NB_PATHS == 11
void part_path_follow_4_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	part_path_follow_callBack(4, param_input_type, scenario_or_gui_command_value);
}
void part_path_follow_5_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	part_path_follow_callBack(5, param_input_type, scenario_or_gui_command_value);
}
void part_path_follow_6_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	part_path_follow_callBack(6, param_input_type, scenario_or_gui_command_value);
}
void part_path_follow_7_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	part_path_follow_callBack(7, param_input_type, scenario_or_gui_command_value);
}
#endif
#if PG_NB_PATHS == 11
void part_path_follow_8_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	part_path_follow_callBack(8, param_input_type, scenario_or_gui_command_value);
}
void part_path_follow_9_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	part_path_follow_callBack(9, param_input_type, scenario_or_gui_command_value);
}
void part_path_follow_10_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	part_path_follow_callBack(10, param_input_type, scenario_or_gui_command_value);
}
void part_path_follow_11_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	part_path_follow_callBack(11, param_input_type, scenario_or_gui_command_value);
}
#endif
#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7 || PG_NB_PATHS == 11
void part_path_repulse_0_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	part_path_repulse_callBack(0, param_input_type, scenario_or_gui_command_value);
}
void part_path_repulse_1_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	part_path_repulse_callBack(1, param_input_type, scenario_or_gui_command_value);
}
void part_path_repulse_2_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	part_path_repulse_callBack(2, param_input_type, scenario_or_gui_command_value);
}
void part_path_repulse_3_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	part_path_repulse_callBack(3, param_input_type, scenario_or_gui_command_value);
}
#endif
#if PG_NB_PATHS == 7 || PG_NB_PATHS == 11
void part_path_repulse_4_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	part_path_repulse_callBack(4, param_input_type, scenario_or_gui_command_value);
}
void part_path_repulse_5_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	part_path_repulse_callBack(5, param_input_type, scenario_or_gui_command_value);
}
void part_path_repulse_6_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	part_path_repulse_callBack(6, param_input_type, scenario_or_gui_command_value);
}
void part_path_repulse_7_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	part_path_repulse_callBack(7, param_input_type, scenario_or_gui_command_value);
}
#endif
#if PG_NB_PATHS == 11
void part_path_repulse_8_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	part_path_repulse_callBack(8, param_input_type, scenario_or_gui_command_value);
}
void part_path_repulse_9_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	part_path_repulse_callBack(9, param_input_type, scenario_or_gui_command_value);
}
void part_path_repulse_10_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	part_path_repulse_callBack(10, param_input_type, scenario_or_gui_command_value);
}
void part_path_repulse_11_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	part_path_repulse_callBack(11, param_input_type, scenario_or_gui_command_value);
}
#endif
void partMove_target_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	// printf("Particle move target %d\n" , int(partMove_target));
	bool state = partMove_target;
	if (!state)
		return; // nothing to do for null values
	partMove_rand = false;
	partMove_target = state;
}
void partMove_rand_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	bool state = partMove_rand;
	if (!state)
		return; // nothing to do for null values
	partMove_target = false;
	partMove_rand = state;
}
void part_initialization_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	pg_targetFrameNo = pg_FrameNo + int(part_timeToTargt);
	//printf("part initialization call back %d %d\n", part_initialization, int(scenario_or_gui_command_value));
}
#endif
#ifdef PG_SENSORS
void sensor_layout_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	// copies the grid layout
	assignSensorPositions();
}
void sensor_activation_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	// copies the grid layout
	assignSensorActivations();
}
void sensor_sample_setUp_callBack(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value) {
	sensor_sample_setUp_interpolation();
}
#endif

///////////////////////////////////////////////////////////////////////////////////
// SCENARIO BASED COMMANDS
///////////////////////////////////////////////////////////////////////////////////
void pg_update_variable(pg_Parameter_Input_Type param_input_type,
			  pg_Keystroke_Input_Type parm_keystroke_type,
			  int indParam,
			  double scenario_or_gui_command_value ) {
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
			*((float *)ScenarioVarPointers[indParam]) = float(scenario_or_gui_command_value);
		}
		//else if (param_input_type == _PG_KEYSTROKE) {
		//	*((float *)ScenarioVarPointers[indParam]) += step;
		//}
		if (ScenarioVarCallbacks[indParam]) {
			(*ScenarioVarCallbacks[indParam])(param_input_type, float(scenario_or_gui_command_value));
		}
	}
	else if (ScenarioVarTypes[indParam] == _pg_int) {
		if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
#ifdef PG_NB_CA_TYPES
			if (indParam == _CA1Type || indParam == _CA2Type ||
				indParam == _CA1SubType || indParam == _CA2SubType) {
				// for CAType we choose to alternate randomly between both types, according
				// to the proximity of floor or ceiling
				float randVal = rand_0_1;
				float thefloor = float(floor(scenario_or_gui_command_value));
				float decimalPart = float(scenario_or_gui_command_value - thefloor);
				if (randVal > decimalPart) {
					*((int *)ScenarioVarPointers[indParam]) = (int)thefloor;
				}
				if (randVal < decimalPart) {
					*((int *)ScenarioVarPointers[indParam]) = (int)thefloor + 1;
				}
			}
			else {
#endif
				*((int *)ScenarioVarPointers[indParam]) = int(scenario_or_gui_command_value);
#ifdef PG_NB_CA_TYPES
			}
#endif
		}
		//else if (param_input_type == _PG_KEYSTROKE) {
		//	*((int *)ScenarioVarPointers[indParam]) = (*((int *)ScenarioVarPointers[indParam]) + (int)step + (int)MaxValues[indParam] + 1) % ((int)MaxValues[indParam] + 1);
		//}
		if (ScenarioVarCallbacks[indParam]) {
			(*ScenarioVarCallbacks[indParam])(param_input_type, float(scenario_or_gui_command_value));
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
			(*ScenarioVarCallbacks[indParam])(param_input_type, float(scenario_or_gui_command_value));
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

#if defined (PIERRES)
/// <summary>
/// updates the position of the cursor in the Keystone pad for texture keystone distortion
/// </summary>
/// <param name=""></param>
void updateXYKeystonePad(void) {
	// Keystone interface
	sprintf(AuxString, "/TopLeft %.2f %.2f", -VP1KeystoneYTopLeft, VP1KeystoneXTopLeft);
	pg_send_message_udp((char*)"ff", AuxString, (char*)"udp_TouchOSC_send");
	sprintf(AuxString, "/BottomLeft %.2f %.2f", -float(double(VP1KeystoneYBottomLeft) - 1.f), VP1KeystoneXBottomLeft);
	pg_send_message_udp((char*)"ff", AuxString, (char*)"udp_TouchOSC_send");
	sprintf(AuxString, "/TopRight %.2f %.2f", -VP1KeystoneYTopRight, float(double(VP1KeystoneXTopRight) - 1.f));
	pg_send_message_udp((char*)"ff", AuxString, (char*)"udp_TouchOSC_send");
	sprintf(AuxString, "/BottomRight %.2f %.2f", -float(double(VP1KeystoneYBottomRight) - 1.f), float(double(VP1KeystoneXBottomRight) - 1.f));
	pg_send_message_udp((char*)"ff", AuxString, (char*)"udp_TouchOSC_send");
}
#endif


#if defined (TVW)
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
	if ((pg_CurrentSceneIndex > 0 || pg_NbScenes < 13) && (pg_CurrentSceneIndex < pg_NbScenes - 1)) {
		int old_pg_CurrentDiaporamaDir = pg_CurrentDiaporamaDir;
		if (new_scene) {
			pg_CurrentDiaporamaDir = SubScenesDiaporamaDir(pg_CurrentSceneIndex);
		}
		else {
			// possible text and visual scene advance
			float elapsed_time_from_start = CurrentClockTime - InitialScenarioTime;

			pg_CurrentDiaporamaDir
				= (int(elapsed_time_from_start - pg_CurrentScene->scene_initial_time) / 60)
				+ SubScenesDiaporamaDir(pg_CurrentSceneIndex);
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
			// printf("scene / diaporama dir %d %d\n", pg_CurrentSceneIndex, pg_CurrentDiaporamaDir);

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

////////////////////////////////////////////////////////////////////////////////////
// BEGINNING OF A NEW SCENE: INITIALIZATION OF TIMES AND DURATIONS, AND VARIABLE VALUES
void pg_keep_value_copy(Scene* currentScene, int indVar, double* parameter_value) {
	if (ScenarioVarTypes[indVar] == _pg_float) {
		parameter_value[indVar] = double(*((float*)ScenarioVarPointers[indVar]));
	}
	else if (ScenarioVarTypes[indVar] == _pg_int) {
		parameter_value[indVar] = double(*((int*)ScenarioVarPointers[indVar]));
	}
	else if (ScenarioVarTypes[indVar] == _pg_bool) {
		if (*((bool*)ScenarioVarPointers[indVar]) != 0) {
			parameter_value[indVar] = 1;
		}
		else {
			parameter_value[indVar] = 0;
		}
	}
	else if (ScenarioVarTypes[indVar] == _pg_sign) {
		parameter_value[indVar] = double(*((float*)ScenarioVarPointers[indVar]));
	}
	else if (ScenarioVarTypes[indVar] == _pg_path) {
		if (*((bool*)ScenarioVarPointers[indVar]) != 0) {
			parameter_value[indVar] = 1;
		}
		else {
			parameter_value[indVar] = 0;
		}
	}
}

// begins a new scene and updates scene variables
void StartNewScene(int ind_scene) {
	// only one setup change per second to avoid repetition
	if (CurrentClockTime - LastSetupChangeClockTime > 1) {
		LastSetupChangeClockTime = CurrentClockTime;
	}
	else {
		return;
	}

	/////////////////////////////////////////////////
	// No interpolation: starts immediately a new scene
	if (pg_SceneInterpolationDuration <= 0) {
		pg_SceneIndexAfterInterpolation = -1;

		restoreInitialTimesAndDurations();

		InitialScenarioTime = CurrentClockTime - pg_Scenario[ind_scene].scene_initial_time;
		if (ind_scene == 0) {
			// restarts scenarios
			AbsoluteInitialScenarioTime = CurrentClockTime - pg_Scenario[0].scene_initial_time;
		}

		// UPDATES THE SCENE POINTERS AND INDICES
		pg_CurrentSceneIndex = ind_scene;
		pg_CurrentScene = &pg_Scenario[pg_CurrentSceneIndex];
		pg_SceneIndexBeforeInterpolation = -1;
		pg_SceneIndexAfterInterpolation = -1;

#ifdef KOMPARTSD
		pg_IPClient* client;
		if ((client = pg_UDP_client((char*)"udp_Record_send"))) {
			// sends the new scene to recording for later replay
			sprintf(AuxString, "/new_scene %d", ind_scene);
			pg_send_message_udp((char*)"i", (char*)AuxString, client);
		}
		if ((client = pg_UDP_client((char*)"udp_Usine_send"))) {
			// sends the new scene to Usine for sample selection
			for (int ind = 0; ind < 4; ind++) {
				if (ind == ind_scene) {
					sprintf(AuxString, "/new_scene_%d 1", ind);
				}
				else {
					sprintf(AuxString, "/new_scene_%d 0", ind);
				}
				pg_send_message_udp((char*)"i", (char*)AuxString, client);
			}
		}
#endif
#if defined(ETOILES)
		if (pg_CurrentSceneIndex == 0 || pg_CurrentSceneIndex == 5) {
			pg_Ind_Current_DisplayText = 0;
		}
#endif

	}
	/////////////////////////////////////////////////
	// scene interpolation: linear interpolation of current variable values to a new scene
	// variables all keep their current values and interpolate towards the values of the variables in the target scene
	else {
		printf("Start interpolation scene\n");
		// UPDATES THE SCENE POINTERS AND INDICES
		pg_CurrentSceneIndex = -1;
		pg_CurrentScene = &pg_InterpolationScene;
		pg_SceneIndexBeforeInterpolation = pg_CurrentSceneIndex;
		pg_SceneIndexAfterInterpolation = ind_scene;

		//to be completed
		pg_CurrentScene->scene_IDs = "INTERPOLATION";
		pg_CurrentScene->scene_duration = pg_SceneInterpolationDuration;
		pg_CurrentScene->scene_change_when_ends = true;
		pg_CurrentScene->scene_initial_time = CurrentClockTime;
		pg_CurrentScene->scene_final_time = pg_CurrentScene->scene_initial_time + pg_SceneInterpolationDuration;
		pg_CurrentScene->scene_originalDuration = pg_SceneInterpolationDuration;
		pg_CurrentScene->scene_originalInitial_time = CurrentClockTime;
		pg_CurrentScene->scene_originalFinal_time = pg_CurrentScene->scene_initial_time + pg_SceneInterpolationDuration;

		for (int indP = 0; indP < _MaxInterpVarIDs; indP++) {
			// copies the current value in the initial value
			pg_keep_value_copy(pg_CurrentScene, indP, pg_CurrentScene->scene_initial_parameters);
			// if the next interpolation keeps the value, copies the current value in the final value
			// otherwise copies the initial value of the target scene as final value of the interpolaiton scene
			if (pg_Scenario[pg_SceneIndexAfterInterpolation].scene_interpolations[indP].interpolation_mode != pg_keep_value) {
				pg_CurrentScene->scene_final_parameters[indP] = pg_Scenario[pg_SceneIndexAfterInterpolation].scene_initial_parameters[indP]; // next scene initial value
			}
			else {
				pg_keep_value_copy(pg_CurrentScene, indP, pg_CurrentScene->scene_final_parameters);
			}
		}
		for (int indP = 0; indP < _MaxInterpVarIDs; indP++) {
			pg_CurrentScene->scene_interpolations[indP].interpolation_mode = pg_linear_interpolation;
			pg_CurrentScene->scene_interpolations[indP].offSet = 0.0;
			pg_CurrentScene->scene_interpolations[indP].duration = 1.0;
		}
		printf("Master values cur %.2f ini %.2f fin %.2f\n", *((float*)ScenarioVarPointers[_master]), 
			pg_CurrentScene->scene_final_parameters[_master], pg_CurrentScene->scene_final_parameters[_master]);
		// resets interpolation time so that the interpolation does not restart indefinitely
		pg_SceneInterpolationDuration = 0.f;
		sprintf(AuxString, "/interpolation_duration %.2f", pg_SceneInterpolationDuration);
		pg_send_message_udp((char*)"i", AuxString, (char*)"udp_TouchOSC_send");
	}

	pg_FirstFrameInScene = true;
	sprintf(AuxString, "/setup %s", pg_CurrentScene->scene_IDs.c_str()); pg_send_message_udp((char*)"s", AuxString, (char*)"udp_TouchOSC_send");
	sprintf(AuxString, "/setup_1 %s", pg_CurrentScene->scene_Msg1.c_str()); pg_send_message_udp((char*)"s", AuxString, (char*)"udp_TouchOSC_send");
	sprintf(AuxString, "/setup_2 %s", pg_CurrentScene->scene_Msg2.c_str()); pg_send_message_udp((char*)"s", AuxString, (char*)"udp_TouchOSC_send");
	std::cout << "Scene: " << pg_CurrentScene->scene_IDs << std::endl;
	// fprintf(pg_csv_file, "Scene:¨%d %s\n", pg_CurrentSceneIndex, pg_CurrentScene->scene_IDs.c_str());
	// reinitialization of the interpolation control variables at the beginning of a new scene
	for (int indVar = 0; indVar < _MaxInterpVarIDs; indVar++) {
		BrokenInterpolationVar[indVar] = false;
		StepwiseInterpolationEffective[indVar] = false;
		if (pg_CurrentScene->scene_interpolations[indVar].interpolation_mode == pg_keep_value) {
			pg_keep_value_copy(pg_CurrentScene, indVar, pg_CurrentScene->scene_initial_parameters);
		}
	}
	// stops ongoing flashes if there is one
	flashCameraTrk_weight = 0.0f;
#ifdef PG_WITH_PHOTO_FLASH
	flashPhotoTrk_weight = 0.0f;
	flashPhotoTrk_nbFrames = 0;
#endif

#if defined (TVW)
	// updates image and text directories
	pg_update_visual_and_text_chapters(true);
#endif
}

#if defined (TVW)
float starting_time(float elapsed_time_from_start) {
	if (pg_NbScenes == 13) {
		if (elapsed_time_from_start > pg_Scenario[2].scene_initial_time ) {
			return pg_Scenario[2].scene_initial_time;
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

////////////////////////////////////////////////////////////////////////////////////
// UPDATE OF A CURRENT SCENE: UPDATE OF VARIABLE VALUES ACCORDING TO INTERPOLATIONS

void pg_update_scene_variables(Scene* cur_scene, float elapsed_time_from_start) {
	// transformed interpolation according to the interpolation function
	for (int indP = 0; indP < _MaxInterpVarIDs; indP++) {
		if (cur_scene->scene_interpolations[indP].interpolation_mode == pg_keep_value) {
			continue;
		}
		// going from interpolation percentages to interpolation times by multiplying by scene duration
		double absoluteOffset = cur_scene->scene_interpolations[indP].offSet
			* cur_scene->scene_duration;
		double absoluteDuration = cur_scene->scene_interpolations[indP].duration
			* cur_scene->scene_duration;

		double transformedInterpolation = 0.0f;
		double twoStepInterpolation[3] = { 0.0f , 0.0f , 0.0f };

		if (elapsed_time_from_start <= cur_scene->scene_initial_time
			+ absoluteOffset) {
			transformedInterpolation = 0.0;
		}
		else if (elapsed_time_from_start >= cur_scene->scene_initial_time
			+ absoluteOffset + absoluteDuration) {
			transformedInterpolation = 1.0;
		}
		else {
			double coefInterpolation
				= elapsed_time_from_start - (cur_scene->scene_initial_time + absoluteOffset);
			// time normalization for interpolation
			if (absoluteDuration > 0.0) {
				coefInterpolation /= absoluteDuration;
			}
			if (coefInterpolation < 0.0)coefInterpolation = 0.0;
			if (coefInterpolation > 1.0)coefInterpolation = 1.0;

			switch (cur_scene->scene_interpolations[indP].interpolation_mode) {
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
					= pow(fabs(coefInterpolation), cur_scene->scene_interpolations[indP].exponent);
				break;
			case pg_bell_interpolation:
				transformedInterpolation = coefInterpolation;
				if (coefInterpolation < 0.5f) {
					twoStepInterpolation[0] = 0.5F * (cos(2.0f * transformedInterpolation * (double)M_PI) + 1.0F);
					twoStepInterpolation[1] = 1.0f - twoStepInterpolation[0];
					twoStepInterpolation[2] = 0.0f;
				}
				else {
					twoStepInterpolation[0] = 0.0f;
					twoStepInterpolation[1] = 1.0f - 0.5F * (cos(2.0f * transformedInterpolation * (double)M_PI) + 1.0F);
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
				// printf("Stepwise interpolations have no intermediate values\n");
				// no offset keeps initial value during the whole scene
				if (cur_scene->scene_interpolations[indP].offSet == 0) {
					transformedInterpolation = 0.f;
				}
				else {
					if (coefInterpolation <= 0.f) {
						transformedInterpolation = 0.f;
					}
					else {
						transformedInterpolation = 1.f;
					}
				}
				break;
			default:
				printf("Unknown interpolation mode!\n");
				break;
			}
		}

		if ((transformedInterpolation == 0.0 && pg_FirstFrameInScene)
			|| transformedInterpolation > 0.0) {
			// calculation of the interpolated value and assignment to variable
			double interpolated_value = cur_scene->scene_initial_parameters[indP];
			// stepwise interpolation
			if (cur_scene->scene_interpolations[indP].interpolation_mode
				== pg_stepwise_interpolation) {
				//if (indP == _activeClipArts) {
				//	 printf( "Scene %d param %d time from start %.2f transf_interp %.2f value %.2f start interp %.2f end interp %.2f duration %.2f\n" , 
				//		 ind_scene, indP, elapsed_time_from_start , transformedInterpolation , interpolated_value ,
				//		 cur_scene->scene_initial_parameters[indP], cur_scene->scene_final_parameters[indP],
				//		 cur_scene->scene_duration);
				//}
				// delayed stepwise change, made only once
				if (transformedInterpolation > 0.0
					&& StepwiseInterpolationEffective[indP] == false) {
					pg_update_variable(_PG_SCENARIO, _PG_KEYSTROKE_VOID,
						indP, cur_scene->scene_final_parameters[indP]);
					StepwiseInterpolationEffective[indP] = true;
				}
				// initial stepwise value
				else if ((transformedInterpolation == 0.0 && pg_FirstFrameInScene)) {
					pg_update_variable(_PG_SCENARIO, _PG_KEYSTROKE_VOID,
						indP, cur_scene->scene_initial_parameters[indP]);
				}
			}
			// continuous interpolation
			else if (cur_scene->scene_interpolations[indP].interpolation_mode != pg_bell_interpolation
				&& cur_scene->scene_interpolations[indP].interpolation_mode != pg_sawtooth_interpolation) {
				interpolated_value
					= (1.0F - transformedInterpolation)
					* cur_scene->scene_initial_parameters[indP]
					+ transformedInterpolation
					* cur_scene->scene_final_parameters[indP];

				// printf( "Scene %d param %d time from start %.2f interp %.2f transf_interp %.2f value %.2f start interp %.2f duration %.2f\n" , ind_scene, indP, elapsed_time_from_start , coefInterpolation , transformedInterpolation , interpolated_value ,cur_scene->scene_initial_time + scene_interpolations[ ind_scene ][ indP ].offSet , scene_interpolations[ ind_scene ][ indP ].duration );
				pg_update_variable(_PG_SCENARIO, _PG_KEYSTROKE_VOID, indP, interpolated_value);
			}
			// 3-valued bell interpolation between initial, median and end value
			else {
				interpolated_value
					= twoStepInterpolation[0]
					* cur_scene->scene_initial_parameters[indP]
					+ twoStepInterpolation[1]
					* cur_scene->scene_interpolations[indP].midTermValue
					+ twoStepInterpolation[2]
					* cur_scene->scene_final_parameters[indP];

				// printf( "Scene %d param %d time from start %.2f interp %.2f transf_interp %.2f value %.2f start interp %.2f duration %.2f\n" , ind_scene, indP, elapsed_time_from_start , coefInterpolation , transformedInterpolation , interpolated_value ,cur_scene->scene_initial_time + scene_interpolations[ ind_scene ][ indP ].offSet , scene_interpolations[ ind_scene ][ indP ].duration );
				pg_update_variable(_PG_SCENARIO, _PG_KEYSTROKE_VOID, indP, interpolated_value);
			}
		}
	}
	pg_FirstFrameInScene = false;
}

///////////////////////////////////////////////////////////////////////////////////////
// SCENARIO UPDATE: SELECTION OF THE CURRENT SCENE AND VARIABLE UPDATE

void pg_update_scenario(void) {
	// printf("VideoPb Update scenario \n");

#if defined (TVW)
	pg_update_visual_and_text_chapters(false);
#endif
	// the current scene is not the interpolation scene between two scenes
	if (pg_CurrentScene != &pg_InterpolationScene) {
		float elapsed_time_from_start = CurrentClockTime - InitialScenarioTime;

		// scans all scenes to find the current one and applies the corresponding interpolations
		for (int ind_scene = 0; ind_scene < pg_NbScenes; ind_scene++) {
			// printf( "time %.2f beg %.2f end %.2f\n" , elapsed_time_from_start ,  pg_Scenario[ind_scene].scene_initial_time ,  pg_Scenario[ind_scene].scene_final_time );
			// current scene found
			if ((elapsed_time_from_start >= pg_Scenario[ind_scene].scene_initial_time
				&& elapsed_time_from_start < pg_Scenario[ind_scene].scene_final_time)
				|| (pg_CurrentSceneIndex == pg_NbScenes - 1 && ind_scene == pg_NbScenes - 1
					&& elapsed_time_from_start > pg_Scenario[pg_NbScenes - 1].scene_final_time)) {
				// loop at the end of the scenario
				if (pg_CurrentSceneIndex == pg_NbScenes - 1 && ind_scene == pg_NbScenes - 1
					&& elapsed_time_from_start > pg_Scenario[pg_NbScenes - 1].scene_final_time) {
					ind_scene = 0;
				}
				// the current scene is finished 
				if (ind_scene != pg_CurrentSceneIndex) {
					// a new scene is launched only if pg_CurrentScene->scene_change_when_ends
					if (pg_CurrentScene->scene_change_when_ends) {
						StartNewScene(ind_scene);
						elapsed_time_from_start = CurrentClockTime - InitialScenarioTime;
					}
					// otherwise the current scene is prolonged, no interpolation takes place 
					// the parameter values stay as they were 
					else {
						// for time display + colors when reaching the end of the scene
						remainingTimeInScene
							= pg_CurrentScene->scene_initial_time + pg_CurrentScene->scene_duration - elapsed_time_from_start;
						// printf( "time %.2f remainingTimeInScene %.2f\n" , elapsed_time_from_start , remainingTimeInScene);
						return;
					}
				}

				// for time display + colors when reaching the end of the scene
				remainingTimeInScene
					= pg_Scenario[ind_scene].scene_initial_time + pg_Scenario[ind_scene].scene_duration - elapsed_time_from_start;
				// printf( "\ntime %.2f interp %.2f " , elapsed_time_from_start , coefInterpolation );

				pg_update_scene_variables(&pg_Scenario[ind_scene], elapsed_time_from_start);

				// fprintf(pg_csv_file, "frame %ld cursor size %d\n", pg_FrameNo, cursorSize);
				return;
			}
			// current scene found
		}
		// scans all scenes to find the current one and applies the corresponding interpolations
	} 
	// the current scene is the interpolation scene
	else {
		// end of interpolation
		if (CurrentClockTime > pg_CurrentScene->scene_final_time) {
			// starts the scene memorized for after the interpolation
			StartNewScene(pg_SceneIndexAfterInterpolation);
			// for time display + colors when reaching the end of the scene
			float elapsed_time_from_start = CurrentClockTime - InitialScenarioTime;
			remainingTimeInScene
				= pg_CurrentScene->scene_initial_time + pg_CurrentScene->scene_duration - elapsed_time_from_start;
			// printf( "\ntime %.2f interp %.2f " , elapsed_time_from_start , coefInterpolation );

			pg_update_scene_variables(pg_CurrentScene, elapsed_time_from_start);
		}
		// ongoing interpolation
		// elapsed time is counted from scene beginning
		else {
			// for time display + colors when reaching the end of the scene
			remainingTimeInScene
				= pg_CurrentScene->scene_initial_time + pg_CurrentScene->scene_duration - CurrentClockTime;
			// printf( "\ntime %.2f interp %.2f " , elapsed_time_from_start , coefInterpolation );

			pg_update_scene_variables(pg_CurrentScene, CurrentClockTime);
		}
	}
}


///////////////////////////////////////////////////////////////////////////////////
// KEYSTROKE BASED COMMANDS
///////////////////////////////////////////////////////////////////////////////////

void pg_process_key(int key) {
	int mod = glutGetModifiers();

	// printf( "key (%d) mod %d\n" , key , mod );

	if (!(mod & GLUT_ACTIVE_ALT)) {
		// non special key that corresponds to a script
		pg_keyStrokeScripts(key);
		return;
	}

	switch (key) {

		/* ------------------------------- beat */
	case 'b':
		auto_beat = !auto_beat;
		lastBeatTime = CurrentClockTime;
		break;

		/* ------------------------------- frame per second */
	case 'f':
		DisplayFramePerSecond = !DisplayFramePerSecond;
		printf("FPS display %d\n", DisplayFramePerSecond);
		break;

#ifdef PG_WITH_PUREDATA
		/* ------------------------------- check connection to PD */
	case 'p':
		pg_send_message_udp((char*)"", (char*)"/PD_connected", (char*)"udp_PD_send");
		break;
#endif

		/* ------------------------------- check connection to QT */
	case 'q':
		pg_send_message_udp((char *)"", (char *)"/QT_connected", (char *)"udp_TouchOSC_send");
		break;

		/* ------------------------------- snapshot */
	case 's':
		pg_draw_scene(_Jpg, false);
		break;
	case 'S':
		pg_draw_scene(_Svg, false);
		break;

#ifdef PG_WITH_CAMERA_CAPTURE
		/* ------------------------------- current video background capture */
	case 'v':
		reset_camera = true;
		break;
#endif

	default:
		printf("key %d is not active.\n", key);
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

#if defined(PG_WITH_JUCE) || defined(PG_WITH_PUREDATA)
void soundTrackvolume(float vol) {
#ifdef PG_WITH_PUREDATA
	sprintf(AuxString, "/soundtrack_onOff %.5f", vol);
	pg_send_message_udp((char*)"f", AuxString, (char*)"udp_PD_send");
	printf("Command: soundtrack: %s\n", AuxString);
#endif
#ifdef PG_WITH_JUCE
	if (vol > 0) {
		pg_send_message_udp((char*)"", (char*)"/JUCE_play_track", (char*)"udp_SoundJUCE_send");
	}
	else {
		pg_send_message_udp((char*)"", (char*)"/JUCE_stop_track", (char*)"udp_SoundJUCE_send");
	}
#endif
	sprintf(AuxString, "/soundtrack_onOff %d", soundTrack_on);
	pg_send_message_udp((char*)"i", AuxString, (char*)"udp_TouchOSC_send");
	sprintf(AuxString, "/soundtrack_volume %.5f", vol);
	pg_send_message_udp((char*)"f", AuxString, (char*)"udp_TouchOSC_send");
}
void soundTrackonOff() {
#ifdef PG_WITH_PUREDATA
	sprintf(AuxString, "/soundtrack_onOff %d", soundTrack_on);
	pg_send_message_udp((char*)"i", AuxString, (char*)"udp_PD_send");
	printf("Command: soundtrack: %s\n", AuxString);
#endif
#ifdef PG_WITH_JUCE
	if (soundTrack_on) {
		pg_send_message_udp((char*)"", (char*)"/JUCE_play_track", (char*)"udp_SoundJUCE_send");
	}
	else {
		pg_send_message_udp((char*)"", (char*)"/JUCE_stop_track", (char*)"udp_SoundJUCE_send");
	}
#endif
	sprintf(AuxString, "/soundtrack_onOff %d", soundTrack_on);
	pg_send_message_udp((char*)"i", AuxString, (char*)"udp_TouchOSC_send");
	sprintf(AuxString, "/soundtrack_volume %.5f", float(int(soundTrack_on)));
	pg_send_message_udp((char*)"f", AuxString, (char*)"udp_TouchOSC_send");
}
void PlayTrack(int indTrack) {
	if (nb_soundtracks > 0) {
		// std::cout << "cwd: " << cwd << std::endl;
		currentlyPlaying_trackNo = indTrack % nb_soundtracks;
		// std::cout << "name: " << (cwd + "/Data/" + project_name + "-data/soundtracks/" + trackFileName[currentlyPlaying_trackNo]) << std::endl;
#ifdef PG_WITH_PUREDATA
#ifdef CAVERNEPLATON
		sprintf(AuxString, "/soundtrack_fileName %s",
			("C:/sync.com/Sync/YN-works/YN_CavernePlaton_2019/CavernePartagee/" + trackFileName[currentlyPlaying_trackNo]).c_str());
		pg_send_message_udp((char *)"s", AuxString, (char *)"udp_PD_send");
#else
#if defined(BICHES)
		sprintf(AuxString, "/soundtrack_fileName %s",
			("C:/sync.com/Sync/LYM-videos/LYM_LesBiches_2020/" + trackFileName[currentlyPlaying_trackNo]).c_str());
		pg_send_message_udp((char *)"s", AuxString, (char *)"udp_PD_send");
#else
		sprintf(AuxString, "/soundtrack_fileName %s",
			(cwd + "/Data/" + project_name + "-data/soundtracks/" + trackFileName[currentlyPlaying_trackNo]).c_str());
		pg_send_message_udp((char *)"s", AuxString, (char *)"udp_PD_send");
#endif
#endif
#endif
#ifdef PG_WITH_JUCE
		sprintf(AuxString, "/JUCE_open_track \"%s\"",
			(cwd + "/Data/" + project_name + "-data/soundtracks/" + trackFileName[currentlyPlaying_trackNo]).c_str());
		pg_send_message_udp((char *)"s", AuxString, (char *)"udp_SoundJUCE_send");
		pg_send_message_udp((char *)"", (char *)"/JUCE_play_track", (char *)"udp_SoundJUCE_send");
#endif
		//sprintf(AuxString, "/soundtrack_shortName %s", trackShortName[currentlyPlaying_trackNo].c_str());
		//pg_send_message_udp((char *)"s", AuxString, (char *)"udp_TouchOSC_send");
		sprintf(AuxString, "/track_shortName %s", trackShortName[currentlyPlaying_trackNo].c_str());
		pg_send_message_udp((char *)"s", AuxString, (char *)"udp_TouchOSC_send");

		printf("soundtrack #%d %s\n", currentlyPlaying_trackNo, trackFileName[currentlyPlaying_trackNo].c_str());
		BrokenInterpolationVar[_playing_soundtrackNo] = true;
		currentlyPlaying_trackNo = indTrack;
	}
}
void StopTrack(void) {
#ifdef PG_WITH_PUREDATA
#ifdef CAVERNEPLATON
	if (currentlyPlaying_trackNo >= 0 && currentlyPlaying_trackNo < nb_soundtracks) {
		sprintf(AuxString, "/soundtrack_fileName %s",
			("C:/sync.com/Sync/YN-works/YN_CavernePlaton_2019/CavernePartagee/" + trackFileName[currentlyPlaying_trackNo]).c_str());
		pg_send_message_udp((char *)"s", AuxString, (char *)"udp_PD_send");
	}
#else
	sprintf(AuxString, "/soundtrack_fileName %s", (char *)"void");
	pg_send_message_udp((char *)"s", AuxString, (char *)"udp_PD_send");
#endif
#endif
#ifdef PG_WITH_JUCE
	sprintf(AuxString, "/JUCE_open_track \"%s\"", (char *)"void");
	pg_send_message_udp((char *)"s", AuxString, (char *)"udp_SoundJUCE_send");
	pg_send_message_udp((char *)"", (char *)"/JUCE_play_track", (char *)"udp_SoundJUCE_send");
#endif
	//sprintf(AuxString, "/soundtrack_shortName %s", trackShortName[currentlyPlaying_trackNo].c_str());
	//pg_send_message_udp((char *)"s", AuxString, (char *)"udp_TouchOSC_send");
	sprintf(AuxString, "/track_shortName %s", "void");
	pg_send_message_udp((char *)"s", AuxString, (char *)"udp_TouchOSC_send");

	//printf("No soundtrack\n");
	BrokenInterpolationVar[_playing_soundtrackNo] = true;
	currentlyPlaying_trackNo = -1;
}
#endif

void pg_launch_performance(int ind_scene) {
	// sprintf(AuxString, "/message launching"); pg_send_message_udp((char *)"s", AuxString, (char *)"udp_TouchOSC_send");
	
	// cleaning up
	isClearCA = 1; // clean CA layer
	isClearAllLayers = 1; // clears bg layer
	isClearEcho = 1; // clears echo buffer

	pg_LaunchFrameNo = pg_FrameNo;

	// restarts scenarios
	restoreInitialTimesAndDurations();
	InitialScenarioTime = CurrentClockTime - pg_Scenario[0].scene_initial_time;
	AbsoluteInitialScenarioTime = CurrentClockTime - pg_Scenario[0].scene_initial_time;
	sprintf(AuxString, "/setup %s", pg_Scenario[0].scene_IDs.c_str()); pg_send_message_udp((char *)"s", AuxString, (char *)"udp_TouchOSC_send");
	sprintf(AuxString, "/setup_1 %s", pg_Scenario[0].scene_Msg1.c_str()); pg_send_message_udp((char *)"s", AuxString, (char *)"udp_TouchOSC_send");
	sprintf(AuxString, "/setup_2 %s", pg_Scenario[0].scene_Msg2.c_str()); pg_send_message_udp((char *)"s", AuxString, (char *)"udp_TouchOSC_send");
	// reinitialization of the interpolation control variables at the beginning of a new scene
	for (int indVar = 0; indVar < _MaxInterpVarIDs; indVar++) {
		BrokenInterpolationVar[indVar] = false;
		StepwiseInterpolationEffective[indVar] = false;
	}

	// removes snaptshots before launching
	remove_files_in_dir(&snapshots_dir_path_name); // removes snapshots before pg_launch_performance

	// lights up the LED
	pg_send_message_udp((char *)"f", (char *)"/launch 1", (char *)"udp_TouchOSC_send");
#ifdef USINE
	// starts the backtrack
	pg_IPClient * client;
	if ((client = pg_UDP_client((char *)"udp_Usine_send"))) {
		pg_send_message_udp((char *)"f", (char *)"/launch 1", client);
	}
#endif

#ifdef GN
	initCA = 1.2f;
#endif

	StartNewScene(ind_scene);
}

void setup(int scene_ind) {
	if (scene_ind >= 0 && pg_CurrentSceneIndex < 0) {
		pg_launch_performance(scene_ind % pg_NbScenes);
	}
	else {
		StartNewScene(scene_ind % pg_NbScenes);
	}
}

void setup_plus(int incay) {
	if (pg_CurrentSceneIndex < 0) {
		pg_launch_performance(0);
	}
	else {
		int new_scene = ((incay + pg_CurrentSceneIndex) % pg_NbScenes);
		StartNewScene(new_scene);
	}
}

void setup_minus(int decay) {
	int new_scene = ((pg_CurrentSceneIndex - decay + pg_NbScenes) % pg_NbScenes);
	StartNewScene(new_scene);
}

void pg_keyStrokeScripts(int key) {
	switch (key) {

		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
		// +++++++++++++++++ PEDALS ++++++++++++++++++++++++++++++++ 
		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
		// ====================================== 
#ifdef PG_WITH_PHOTO_FLASH
	case 'a':
		//setup_plus(1);
		pg_Make_flashPhoto();
		break;
#endif
	case 'b':
#if PG_NB_PATHS == 11
		if (path_record_11) {
			path_replay_trackNo_callBack(11, _PG_GUI_COMMAND, -1);
			break;
		}
		if (is_path_replay[10]) {
			path_record_callBack(11, _PG_GUI_COMMAND, -1);
			break;
		}
		if (path_record_10) {
			path_replay_trackNo_callBack(10, _PG_GUI_COMMAND, -1);
			break;
		}
		if (is_path_replay[9]) {
			path_record_callBack(10, _PG_GUI_COMMAND, -1);
			break;
		}
		if (path_record_9) {
			path_replay_trackNo_callBack(9, _PG_GUI_COMMAND, -1);
			break;
		}
		if (is_path_replay[8]) {
			path_record_callBack(9, _PG_GUI_COMMAND, -1);
			break;
		}
		if (path_record_8) {
			path_replay_trackNo_callBack(8, _PG_GUI_COMMAND, -1);
			break;
		}
		if (is_path_replay[7]) {
			path_record_callBack(8, _PG_GUI_COMMAND, -1);
			break;
		}
#endif
#if PG_NB_PATHS == 7 || PG_NB_PATHS == 11
		if (path_record_7) {
			path_replay_trackNo_callBack(7, _PG_GUI_COMMAND, -1);
			break;
		}
		if (is_path_replay[6]) {
			path_record_callBack(7, _PG_GUI_COMMAND, -1);
			break;
		}
		if (path_record_6) {
			path_replay_trackNo_callBack(6, _PG_GUI_COMMAND, -1);
			break;
		}
		if (is_path_replay[5]) {
			path_record_callBack(6, _PG_GUI_COMMAND, -1);
			break;
		}
		if (path_record_5) {
			path_replay_trackNo_callBack(5, _PG_GUI_COMMAND, -1);
			break;
		}
		if (is_path_replay[4]) {
			path_record_callBack(5, _PG_GUI_COMMAND, -1);
			break;
		}
		if (path_record_4) {
			path_replay_trackNo_callBack(4, _PG_GUI_COMMAND, -1);
			break;
		}
		if (is_path_replay[3]) {
			path_record_callBack(4, _PG_GUI_COMMAND, -1);
			break;
		}
#endif
#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7 || PG_NB_PATHS == 11
		if (path_record_3) {
			path_replay_trackNo_callBack(3, _PG_GUI_COMMAND, -1);
			break;
		}
		if (is_path_replay[2]) {
			path_record_callBack(3, _PG_GUI_COMMAND, -1);
			break;
		}
		if (path_record_2) {
			path_replay_trackNo_callBack(2, _PG_GUI_COMMAND, -1);
			break;
		}
		if (is_path_replay[1]) {
			path_record_callBack(2, _PG_GUI_COMMAND, -1);
			break;
		}
		if (path_record_1) {
			path_replay_trackNo_callBack(1, _PG_GUI_COMMAND, -1);
			break;
		}
#endif
		else {
			path_record_callBack(1, _PG_GUI_COMMAND, -1);
			break;
		}
		break;

		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
		// +++++++++++++++++ SET-UP ++++++++++++++++++++++++++++++++ 
		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
		// ====================================== 
		// advances to next scene without respect for the global timing
	case'S':
		if (CurrentClockTime - LastSetupChangeClockTime > 1) {
			setup_plus(1);
			LastSetupChangeClockTime = CurrentClockTime;
		}
		break;

		// advances to next scene and keeps the global timing
	case's': {
			restoreInitialTimesAndDurations();
			float deltaTime = pg_Scenario[1 + pg_CurrentSceneIndex].scene_initial_time - (CurrentClockTime - InitialScenarioTime);
			// if the scene has not yet begun
			if (deltaTime > 0) {
				int new_scene = ((1 + pg_CurrentSceneIndex) % pg_NbScenes);
				// we place the beginning of the current scene at this time
				pg_Scenario[new_scene].scene_initial_time -= deltaTime;
				if (new_scene > 0) {
					pg_Scenario[new_scene - 1].scene_final_time -= deltaTime;
					pg_Scenario[new_scene - 1].scene_duration -= deltaTime;
				}
				pg_Scenario[new_scene].scene_duration += deltaTime;
				// unuseful because is already made through pg_Scenario[new_scene].scene_initial_time -= deltaTime;
				//InitialScenarioTime = CurrentClockTime - pg_Scenario[new_scene].scene_initial_time;
				StartNewScene(new_scene);
			}
		}
		break;

		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
		// +++++++++++++++++ TEST UDP ++++++++++++++++++++++++++++++ 
		// +++++++++++++++++   keystroke (T)  ++++++++++++++++++++++ 
		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	case'T':
		pg_writeMessageOnScreen((char*)"******");
		pg_IPClient* client;
		// message to supercollider
		if ((client = pg_UDP_client((char*)"udp_SC"))) {
			pg_send_message_udp((char*)"f", (char*)"/testUDP 0", client);
		}
		// message to touch OSC
		sprintf(AuxString, "/setup TEST_UDP"); pg_send_message_udp((char*)"s", AuxString, (char*)"udp_TouchOSC_send");
		//sprintf(AuxString, "/return_message returnUPD_%f", CurrentClockTime); pg_send_message_udp((char *)"s", AuxString, (char *)"udp_TouchOSC_send");
		break;

		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
		// +++++++++++++++++ LAUNCH TIMER ++++++++++++++++++++++++++ 
		// +++++++++++++++++   keystroke (t)  ++++++++++++++++++++++ 
		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	case't':
		pg_launch_performance(0);
		break;
	}
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
// +++++++++++++++++ FLASHES +++++++++++++++++++++++++++++++ 
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
// ======================================== 
// flash triggering according to beat & frequency, weight, and on/off values 
// ======================================== 
bool flash_beat_generation(int flash_frequency) {
	// flashes are synchronized on beats according to their frequency
	if (flash_frequency > 0
		&& flash_frequency <= PG_LOOP_SIZE 
		&& (pg_BeatNo % (PG_LOOP_SIZE / flash_frequency)) == 0) {
		return true;
	}
	return false;
}
bool flash_continuous_generation(int flash_frequency) {
	// flashes are made every frame if frequency is PG_LOOP_SIZE + 1
	//printf("peak %d\n", int(pg_video_sound_peak));
	if (flash_frequency == (PG_LOOP_SIZE + 1)) {
		return true;
	}
	// flashes are made every video soundtrack peak (sound volume 1) if frequency is PG_LOOP_SIZE + 2
	else if (flash_frequency == (PG_LOOP_SIZE + 2) && pg_video_sound_peak) {
		//printf("peak %.5f left %d\n", CurrentClockTime - pg_movie_status.get_initialTime(), pg_movie_status.get_nbFramesLeft());
		return true;
	}
	// flashes are made every video soundtrack onset detected by aubio library if frequency is PG_LOOP_SIZE + 3
	else if (flash_frequency == (PG_LOOP_SIZE + 3) && pg_video_sound_onset) {
		//printf("onset %.5f left %d\n", CurrentClockTime - pg_movie_status.get_initialTime(), pg_movie_status.get_nbFramesLeft());
		return true;
	}
	return false;
}

void pg_flash_control(bool (*control_function)(int)) {
	if ((*control_function)(flashPixel_freq)) {
		flashPixel = flashPixel_duration;
	}

	if ((*control_function)(flashPixel_freq)) {
		flashPixel = flashPixel_duration;
	}

#ifdef PG_NB_CA_TYPES
	if ((*control_function)(flashParticleInit_freq)) {
		part_initialization = unsigned int(floor(rand_0_1 * pg_particle_initial_pos_speed_texID.size())) % pg_particle_initial_pos_speed_texID.size();
		pg_targetFrameNo = pg_FrameNo + int(part_timeToTargt);
		// printf( "flashTrkCA 0 (%.2f)\n" , flashTrkCA_weights[0] );
	}
#if PG_NB_TRACKS >= 2
	if ((*control_function)(flashTrkCA_freq_1)) {
		flashTrkCA_weights[1] = 1.0;
		// printf( "flashTrkCA 1 (%.2f)\n" , flashTrkCA_weights[1] );
	}
#endif
#if PG_NB_TRACKS >= 3
	if ((*control_function)(flashTrkCA_freq_2)) {
		flashTrkCA_weights[2] = 1.0;
		// printf( "flashTrkCA 2 (%.2f)\n" , flashTrkCA_weights[2] );
	}
#endif
#if PG_NB_TRACKS >= 4
	if ((*control_function)(flashTrkCA_freq_3)) {
		flashTrkCA_weights[3] = 1.0;
		// printf( "flashTrkCA 3 (%.2f)\n" , flashTrkCA_weights[3] );
	}
#endif
#endif

#if defined (TEXTURED_QUAD_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 
	if ((*control_function)(flashTrkPart_freq_0)) {
		flashTrkPart_weights[0] = 1.0;
		// printf( "flashTrkPart 0 (%.2f)\n" , flashTrkPart_weights[0] );
	}
#if PG_NB_TRACKS >= 2
	if ((*control_function)(flashTrkPart_freq_1)) {
		flashTrkPart_weights[1] = 1.0;
		// printf( "flashTrkPart 1 (%.2f)\n" , flashTrkPart_weights[1] );
	}
#endif
#if PG_NB_TRACKS >= 3
	if ((*control_function)(flashTrkPart_freq_2)) {
		flashTrkPart_weights[2] = 1.0;
		// printf( "flashTrkPart 2 (%.2f)\n" , flashTrkPart_weights[2] );
	}
#endif
#if PG_NB_TRACKS >= 4
	if ((*control_function)(flashTrkPart_freq_3)) {
		flashTrkPart_weights[3] = 1.0;
		// printf( "flashTrkPart 3 (%.2f)\n" , flashTrkPart_weights[3] );
	}
#endif
#endif

#if PG_NB_TRACKS >= 2
	if ((*control_function)(flashTrkBG_freq_1)) {
		// should only be flashed every second frame because of the way the pixels are spreaded
		// otherwise the pixels are not emitted
		if (control_function != flash_continuous_generation || pg_FrameNo % 2 == 0) {
			flashTrkBG_weights[1] = 1.0;
			// printf( "flashTrkBG (%d)\n" , flashTrkBG );
		}
	}
#endif
#if PG_NB_TRACKS >= 3
	if ((*control_function)(flashTrkBG_freq_2)) {
		// should only be flashed every second frame because of the way the pixels are spreaded
		// otherwise the pixels are not emitted
		if (control_function != flash_continuous_generation || pg_FrameNo % 2 == 0) {
			flashTrkBG_weights[2] = 1.0;
			// printf( "flashTrkBG (%d)\n" , flashTrkBG );
		}
	}
#endif
#if PG_NB_TRACKS >= 4
	if ((*control_function)(flashTrkBG_freq_3)) {
		// should only be flashed every second frame because of the way the pixels are spreaded
		// otherwise the pixels are not emitted
		if (control_function != flash_continuous_generation || pg_FrameNo % 2 == 0) {
			flashTrkBG_weights[3] = 1.0;
			// printf( "flashTrkBG (%d)\n" , flashTrkBG );
		}
	}
#endif

#ifdef PG_NB_CA_TYPES
	if ((*control_function)(flashCABG_freq)) {
		flashCABG_weight = 1.0;
		// printf( "flashCABG_freq (%d)\n" , flashCABG_freq );
	}
#endif

#if defined (TEXTURED_QUAD_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 
	if ((*control_function)(flashCAPart_freq)) {
		flashCAPart_weight = 1.0;
		// printf( "flashCAPart_freq (%d)\n" , flashCAPart_freq );
	}
	if ((*control_function)(flashPartBG_freq)) {
		flashPartBG_weight = 1.0;
		// printf( "flashPartBG_freq (%d)\n" , flashPartBG_freq );
	}
	if ((*control_function)(flashPartCA_freq)) {
		flashPartCA_weight = 1.0;
		// printf( "flashPartCA_freq (%d)\n" , flashPartCA_freq );
	}
#endif
}

///////////////////////////////////////////////////////////////////////////////////
// beat reception and corresponding events triggering
#ifdef ATELIERSENFANTS
void NumberOfInteractionFingers(int nb_fingers) {
	if (nb_fingers == 1) {
		sprintf(AuxString, "/multixy/5/visible 0"); pg_send_message_udp((char*)"i", (char*)AuxString, (char*)"udp_MultiTouch_send");
		sprintf(AuxString, "/multixy/4/visible 0"); pg_send_message_udp((char*)"i", (char*)AuxString, (char*)"udp_MultiTouch_send");
		sprintf(AuxString, "/multixy/3/visible 0"); pg_send_message_udp((char*)"i", (char*)AuxString, (char*)"udp_MultiTouch_send");
		sprintf(AuxString, "/multixy/2/visible 0"); pg_send_message_udp((char*)"i", (char*)AuxString, (char*)"udp_MultiTouch_send");
		//printf("fingers 2-5 invisible\n");
		CurrentMousePos_x[4] = PG_OUT_OF_SCREEN_CURSOR;
		CurrentMousePos_y[4] = PG_OUT_OF_SCREEN_CURSOR;
		CurrentMousePos_x[3] = PG_OUT_OF_SCREEN_CURSOR;
		CurrentMousePos_y[3] = PG_OUT_OF_SCREEN_CURSOR;
		CurrentMousePos_x[2] = PG_OUT_OF_SCREEN_CURSOR;
		CurrentMousePos_y[2] = PG_OUT_OF_SCREEN_CURSOR;
		CurrentMousePos_x[1] = PG_OUT_OF_SCREEN_CURSOR;
		CurrentMousePos_y[1] = PG_OUT_OF_SCREEN_CURSOR;
	}
	else if(nb_fingers == 5) {
		sprintf(AuxString, "/multixy/5/visible 1"); pg_send_message_udp((char*)"i", (char*)AuxString, (char*)"udp_MultiTouch_send");
		sprintf(AuxString, "/multixy/4/visible 1"); pg_send_message_udp((char*)"i", (char*)AuxString, (char*)"udp_MultiTouch_send");
		sprintf(AuxString, "/multixy/3/visible 1"); pg_send_message_udp((char*)"i", (char*)AuxString, (char*)"udp_MultiTouch_send");
		sprintf(AuxString, "/multixy/2/visible 1"); pg_send_message_udp((char*)"i", (char*)AuxString, (char*)"udp_MultiTouch_send");
		//printf("fingers 2-5 visible\n");
	}
}
#endif



///////////////////////////////////////////////////////////////////////////////////
// beat reception and corresponding events triggering
void ReceiveBeat(void) {
#ifndef ATELIERSENFANTS
	sprintf(AuxString, "/loop_beat %d", pg_BeatNo % PG_LOOP_SIZE); pg_send_message_udp((char *)"i", AuxString, (char *)"udp_TouchOSC_send");
#endif
	// loop start 
	if ((pg_BeatNo % PG_LOOP_SIZE) == 0) {

		for (int ind = 1; ind <= PG_NB_PATHS; ind++) {
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

#ifdef PG_WITH_CAMERA_CAPTURE
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// camera flash
	if (is_flashCameraTrk
		&& ((pg_BeatNo % PG_LOOP_SIZE) == flashCameraTrkBeat || flashCameraTrkBeat == 9)
		) {
		is_flashCameraTrk = false;
		flashCameraTrk_weight = 1.0f;
		flashCameraTrk_threshold = std::min(std::max(0.0f, 1.0f - flashCameraTrk_decay * flashCameraTrkBright), 1.0f);
		//printf("flashCameraTrk_weight %.3f decay %.2f threshold %.3f\n", 
		//	flashCameraTrk_weight , flashCameraTrk_decay, flashPhotoTrk_threshold);
	}
#endif
#ifdef PG_WITH_PHOTO_FLASH
	// photo flash
	// weight is the current brightness of the lighting at each frame  until it reaches the threshold value and is then stopped 
	if (is_flashPhotoTrk
		&& ((pg_BeatNo % PG_LOOP_SIZE) == flashPhotoTrkBeat || flashPhotoTrkBeat == 0)) {
		is_flashPhotoTrk = false;
		pg_Make_flashPhoto();
	}
#endif

	pg_flash_control(flash_beat_generation);

	// photo change flash
	if (flashPhotoChangeBeat > 0 && ((pg_BeatNo % PG_LOOP_SIZE) == flashPhotoChangeBeat)) {
		// goes to the first photo diaporama if it is not already selected and if there is one 
		if (photo_diaporama < 0 && nb_photo_albums > 0) {
			photo_diaporama = 0;
		}
		pg_CurrentDiaporamaDir = (pg_CurrentDiaporamaDir + int(rand_0_1 * pg_nbCompressedImageDirs)) % pg_nbCompressedImageDirs;
		//printf("pg_CurrentDiaporamaDir %d\n", pg_CurrentDiaporamaDir);
		sprintf(AuxString, "/diaporama_shortName %03d", pg_CurrentDiaporamaDir);
		pg_send_message_udp((char*)"s", AuxString, (char*)"udp_TouchOSC_send");
		pg_launch_diaporama();
	}

#if defined (TVW)
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

#ifdef ULM
	if (randomClipArts) {
		int indImage = int(floor(randomValue * (pg_nb_ClipArt - 0.00001))) % pg_nb_ClipArt;
		pg_ClipArt_Scale[indImage] = randomValue * 2 + 0.1f;
		pg_ClipArt_Rotation[indImage] = randomValue * 360;
		pg_ClipArt_Translation_X[indImage] = randomValue * 1024 / pg_ClipArt_Scale[indImage];
		pg_ClipArt_Translation_Y[indImage] = randomValue * 768 / pg_ClipArt_Scale[indImage];
		ClipArt_OnOff(indImage + 1);
	}
#endif

#ifdef CAVERNEPLATON
	if (Caverne_BackColor) {
		Caverne_BackColorRed = rand_0_1;
		Caverne_BackColorGreen = rand_0_1;
		Caverne_BackColorBlue = rand_0_1;
		// color saturation
		float max_col = max(Caverne_BackColorRed, max(Caverne_BackColorGreen, Caverne_BackColorBlue));
		Caverne_BackColorRed += 1.f - max_col;
		Caverne_BackColorGreen += 1.f - max_col;
		Caverne_BackColorBlue += 1.f - max_col;
		// color debrillance
		float brillance = min(Caverne_BackColorRed, min(Caverne_BackColorGreen, Caverne_BackColorBlue));
		if (Caverne_BackColorRed == brillance) {
			Caverne_BackColorRed = 0.f;
		}
		else if (Caverne_BackColorRed != 1.f) {
			Caverne_BackColorRed -= brillance / 2.f;
		}
		if (Caverne_BackColorGreen == brillance) {
			Caverne_BackColorGreen = 0.f;
		}
		else if (Caverne_BackColorGreen != 1.f) {
			Caverne_BackColorGreen -= brillance / 2.f;
		}
		if (Caverne_BackColorBlue == brillance) {
			Caverne_BackColorBlue = 0.f;
		}
		else if (Caverne_BackColorBlue != 1.f) {
			Caverne_BackColorBlue -= brillance / 2.f;
		}

		// background flash
		Caverne_BackColorFlash = true;
		Caverne_BackColorFlash_prec = false;
	}
	else {
		Caverne_BackColorRed = 0.f;
		Caverne_BackColorGreen = 0.f;
		Caverne_BackColorBlue = 0.f;
	}
#endif
}

///////////////////////////////////////////////////////////////////////////////////
// photo flash launching
#ifdef PG_WITH_PHOTO_FLASH
void pg_Make_flashPhoto(void) {
	flashPhotoTrk_weight = 1.0f;
	flashPhotoTrk_nbFrames = 0;
	flashPhotoTrk_threshold = std::min(std::max(0.0f, 1.0f - flashPhotoTrk_decay * flashPhotoTrkBright), 1.0f);
	// invertAllLayers = !invertAllLayers;
	//photo_scale = rand_0_1 * 0.5f + 0.5f;
	//photo_offsetX = rand_0_1 * 1.5f - 0.5f;
	//photo_offsetY = rand_0_1 * 1.5f - 0.5f;
	// pg_CurrentDiaporamaDir = int(floor(9 + rand_0_1 * 14.9));
	pg_CurrentDiaporamaDir = int(floor(rand_0_1 * pg_nbCompressedImageDirs)) % pg_nbCompressedImageDirs;
	pg_launch_diaporama();
	//printf("flashPhotoTrk_weight %.3f decay %.2f threshold %.3f\n",
	//	flashPhotoTrk_weight, flashPhotoTrk_decay, flashPhotoTrk_threshold);
}
#endif

void pg_Make_flashCamera(void) {
	is_flashCameraTrk = true;
	flashCameraTrk_weight = 0.0f;
	// printf("start flash\n");
	//    break;
}

///////////////////////////////////////////////////////////////////////////////////
// UDP BASED COMMANDS
///////////////////////////////////////////////////////////////////////////////////
void pg_aliasScript(char *command_symbol, char* string_argument_0,
	float arguments[MAX_OSC_ARGUMENTS], int argc) {
	std::string newCommand(command_symbol);
#ifdef ATELIERSENFANTS
	float control_color[3];
#endif

	// unregistered command
	if (pg_stringCommands.find(newCommand) == pg_stringCommands.end()) {
		// touch OSC sends OSC commands for each tab change, we have decided to name them _DISPLAY
		string touchOSC_tab_command_ending("_DISPLAY");
		// touch OSC sends OSC commands for MSS and sensor that is forwarded to pg_sensors.py, we have decided to prefix them with mss_
		string MSS_sensor_command_beginning("mss_");
		string XMM_sensor_command_beginning("xmm_");
		if (newCommand.length() >= touchOSC_tab_command_ending.length()
			&& (0 == newCommand.compare(newCommand.length() - touchOSC_tab_command_ending.length(), touchOSC_tab_command_ending.length(), touchOSC_tab_command_ending))) {
			// OKAY this is a tab change in touchOSC
		}
		else if ((newCommand.length() >= MSS_sensor_command_beginning.length()
				&& (0 == newCommand.compare(0, MSS_sensor_command_beginning.length(), MSS_sensor_command_beginning)))
				|| (newCommand.length() >= XMM_sensor_command_beginning.length()
					&& (0 == newCommand.compare(0, XMM_sensor_command_beginning.length(), XMM_sensor_command_beginning))) ){
			// OKAY this is a MSS and sensor command that is forwarded to pg_sensors.py
			// printf("MSS and sensor command %s\n", command_symbol);
			if (argc == 1) {
				sprintf(AuxString, "/%s %.5f", command_symbol, arguments[0]); pg_send_message_udp((char *)"f", AuxString, (char *)"udp_MSS_sensor_send");
			}
			else if (argc == 2) {
				sprintf(AuxString, "/%s %.5f %.5f", command_symbol, arguments[0], arguments[1]); pg_send_message_udp((char *)"ff", AuxString, (char *)"udp_MSS_sensor_send");
			}
			else {
				sprintf(AuxString, "/%s", command_symbol); pg_send_message_udp((char *)"", AuxString, (char *)"udp_MSS_sensor_send");
			}
		}
		else {
			sprintf(ErrorStr, "Unknown command (%s)!", command_symbol); ReportError(ErrorStr);
		}
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
		// from TouchOSC, Lola's sound program or Usine
		sprintf(AuxString, "*** OK ***");
		pg_writeMessageOnScreen(AuxString);
		sprintf(AuxString, "/setup UDP_test_received"); pg_send_message_udp((char *)"s", AuxString, (char *)"udp_TouchOSC_send");
#ifdef KOMPARTSD
		pg_IPClient * client;
		if ((client = pg_UDP_client((char *)"udp_Usine_send"))) {
			// sends a message to Usine for confirming UDP connection between porphyrograph and Usine
			if (testUDP_KOMPARTSD) {
				sprintf(AuxString, "/testUDP 1");
			}
			else {
				sprintf(AuxString, "/testUDP 0");
			}
			testUDP_KOMPARTSD = !testUDP_KOMPARTSD;
			pg_send_message_udp((char *)"i", (char *)AuxString, client);
			// sends the new scene to Usine for sample selection
			for (int ind = 0; ind < 4; ind++) {
				sprintf(AuxString, "/new_scene_%d 0", ind);
				pg_send_message_udp((char *)"i", (char *)AuxString, client);
			}
		}
#endif
		break;
	}
	case _QT_connected: {
		sprintf(AuxString, "*** QT ***"); 
		pg_writeMessageOnScreen(AuxString);
		break;
	}

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// +++++++++++++++++ PD CONNECTION CONFIRMATION ++++++++++++ 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	case _PD_connected: {
		// from Yukao's PD
		pg_send_message_udp((char *)"s", (char *)"/PD_connected 1", (char *)"udp_TouchOSC_send");
		sprintf(AuxString, "*** PD ***"); 
		pg_writeMessageOnScreen(AuxString);
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
		pg_launch_performance(0);
		break;
	}

#ifdef PG_WITH_PUREDATA
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
#endif

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// +++++++++++++++++ QUITS +++++++++++++++++++++++++++++++++ 
	// +++++++++++++++++   keystroke (ESC)  ++++++++++++++++++++ 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	case _quit: {
		quit();
		break;
	}

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// +++++++++++++++++ VIDEO & PHOTO FLASH +++++++++++++++++++ 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	case _flashCamera: {
		pg_Make_flashCamera();
		break;
	}
	case _Note1: 
	case _Note2:
	case _Note3: 
	{
#ifdef PIERRES
		int argt0 = int(round(arguments[0]));
		if (argt0 == 64) {
			ClipArt_OnOff(1); break;
		}
		else if (argt0 == 65) {
			ClipArt_OnOff(2); break;
		}
		else if (argt0 == 67) {
			ClipArt_OnOff(3); break;
		}
		else if (argt0 == 69) {
			ClipArt_OnOff(4); break;
		}
		else if (argt0 == 71) {
			ClipArt_OnOff(5); break;
		}
		else if (argt0 == 72) {
			ClipArt_OnOff(6); break;
		}
#endif
#ifdef SONG
		{
		int argt0 = int(round(arguments[0]));
		if (argt0 >= 0) {
			pg_MIDINote = argt0;
		}
		else {
			pg_MIDINote = 0;
		}
		if (MIDIwithBrush) {
			pen_brush = pg_MIDINote % (nb_pen_brushes * 3);
			BrokenInterpolationVar[_pen_brush] = true;
			printf("new brush %d\n", current_pen_colorPreset);
		}
		if (MIDIwithColor) {
			if (nb_pen_colorPresets > 0) {
				current_pen_colorPreset = pg_MIDINote % nb_pen_colorPresets;
				printf("new palette %d\n", current_pen_colorPreset);
				BrokenInterpolationVar[_pen_color] = true;
				*((float *)ScenarioVarPointers[_pen_color]) = pen_colorPreset_values[current_pen_colorPreset];
				// sprintf(AuxString, "/pen_color %.3f", pen_colorPresets_values[current_pen_colorPreset]);
				// pg_send_message_udp((char *)"s", AuxString, (char *)"udp_TouchOSC_send");
				// sprintf(AuxString, "/message palette%s", pen_colorPresets_names[current_pen_colorPreset].c_str()); pg_send_message_udp((char *)"s", (char *)AuxString, (char *)"udp_TouchOSC_send");
				sprintf(AuxString, "/pen_colorPreset %d", current_pen_colorPreset); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
			}
		}
		if (MIDIwithCameraFlash) {
			printf("Flash camera\n");
			pg_Make_flashCamera();
		}
		if (MIDIwithPhotoFlash) {
			printf("Flash photo\n");
			pg_Make_flashPhoto();
		}
		if (MIDIwithBeat) {
			pg_BeatNo++;
			printf("beat %d\n", pg_BeatNo);
			ReceiveBeat();
		}
	}
#else
		if (pg_CurrentSceneIndex > 0) {
#ifdef PG_WITH_CAMERA_CAPTURE
			is_flashCameraTrk = false;
			flashCameraTrk_weight = flashCameraTrkBright;
			flashCameraTrk_threshold = std::min(std::max(0.0f, 1.0f - flashCameraTrk_decay * flashCameraTrkBright), 1.0f);
			//printf("flashCameraTrk_weight %.3f decay %.2f threshold %.3f\n",
			//	flashCameraTrk_weight, flashCameraTrk_decay, flashPhotoTrk_threshold);
			//printf("start flash\n");
			//    break;
#endif
		}
#endif
	break;
	}
#ifdef PG_WITH_PHOTO_FLASH
	case _flashPhoto: {
		if (flashPhotoTrkBeat == 0) {
			pg_Make_flashPhoto();
		}
		else {
			// will be launched when the beat corresponds
			is_flashPhotoTrk = true;
			flashPhotoTrk_weight = 0.0f;
		}
		// printf("start flash\n");
		break;
	}
#endif

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// +++++++++++++++++ MUSIC CONTROL +++++++++++++++++++++++++ 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// received from pure data or generated by program if auto_sound is on
	case _pulse_spectrum: {
		pulse[0] = arguments[0] * sound_volume + sound_min;
		pulse[1] = arguments[1] * sound_volume + sound_min;
		pulse[2] = arguments[2] * sound_volume + sound_min;
#ifndef ATELIERSENFANTS
		// not used currently  pulse_attack = arguments[3] * sound_volume + sound_min;
		sprintf(AuxString, "/pulse_low %.2f", pulse[0]);
		pg_send_message_udp((char *)"f", AuxString, (char *)"udp_TouchOSC_send");
		sprintf(AuxString, "/pulse_medium %.2f", pulse[1]);
		pg_send_message_udp((char *)"f", AuxString, (char *)"udp_TouchOSC_send");
		sprintf(AuxString, "/pulse_high %.2f", pulse[2]);
		pg_send_message_udp((char *)"f", AuxString, (char *)"udp_TouchOSC_send");
#endif

		pulse_average_prec = pulse_average;
		pulse_average = (pulse[0] + pulse[1] + pulse[2]) / 3.f;
		//printf("pulse %.5f %.5f %.5f avg %.5f\n", pulse[0], pulse[1], pulse[2], pulse_average);

#ifndef ATELIERSENFANTS
		sprintf(AuxString, "/pulse %.2f", pulse_average);
		pg_send_message_udp((char *)"f", AuxString, (char *)"udp_TouchOSC_send");
		// printf("%s\n", AuxString);
#endif

#ifndef PEN_HSV
		compute_pulsed_palette_color(pen_color, pen_color_pulse, pen_grey, pen_grey_pulse, pulsed_pen_color, true);
#else
		compute_pulsed_HSV_color(pen_hue, pen_hue_pulse, pen_sat, pen_sat_pulse, pen_value, pen_value_pulse, pulsed_pen_color, true);
#endif
		sprintf(AuxString, "/pen_color/color %02x%02x%02xFF", int(pulsed_pen_color[0] * 255), int(pulsed_pen_color[1] * 255), int(pulsed_pen_color[2] * 255)); pg_send_message_udp((char*)"s", (char*)AuxString, (char*)"udp_TouchOSC_send");
		compute_pulsed_palette_color(repop_colorCA, repop_colorCA_pulse, repop_greyCA, repop_greyCA_pulse, pulsed_repop_colorCA, false);
		sprintf(AuxString, "/CA_repopColor/color %02x%02x%02xFF", int(pulsed_repop_colorCA[0] * 255), int(pulsed_repop_colorCA[1] * 255), int(pulsed_repop_colorCA[2] * 255)); pg_send_message_udp((char*)"s", (char*)AuxString, (char*)"udp_TouchOSC_send");
		compute_pulsed_palette_color(repop_colorBG, repop_colorBG_pulse, repop_greyBG, repop_greyBG_pulse, pulsed_repop_colorBG, false);
		sprintf(AuxString, "/BG_repopColor/color %02x%02x%02xFF", int(pulsed_repop_colorBG[0] * 255), int(pulsed_repop_colorBG[1] * 255), int(pulsed_repop_colorBG[2] * 255)); pg_send_message_udp((char*)"s", (char*)AuxString, (char*)"udp_TouchOSC_send");
		compute_pulsed_palette_color(repop_colorPart, repop_colorPart_pulse, repop_greyPart, repop_greyPart_pulse, pulsed_repop_colorPart, false);
		sprintf(AuxString, "/Part_repopColor/color %02x%02x%02xFF", int(pulsed_repop_colorPart[0] * 255), int(pulsed_repop_colorPart[1] * 255), int(pulsed_repop_colorPart[2] * 255)); pg_send_message_udp((char*)"s", (char*)AuxString, (char*)"udp_TouchOSC_send");
		break;
	}

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// +++++++++++++++++ FLASHES +++++++++++++++++++++++++++++++ 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// ======================================== 
	// flash triggering according to beat & frequency, weight, and on/off values 
	// ======================================== 
	// flashes are synchronized on beats according to their frequency
	// received from pure data or generated by program if auto_beat is on
	case _beat: {
		int argt0 = int(round(arguments[0]));
		if (argt0 >= 0) {
			pg_BeatNo = argt0;
		}
		else {
			pg_BeatNo++;
		}

		ReceiveBeat();
	}
				break;

	case _MIDINote: {
		int argt0 = int(round(arguments[0]));
		if (argt0 >= 0) {
			pg_MIDINote = argt0;
		}
		else {
			pg_MIDINote = 0;
		}
		if (MIDIwithBrush) {
			pen_brush = pg_MIDINote % (nb_pen_brushes * 3);
			BrokenInterpolationVar[_pen_brush] = true;
			printf("new brush %d\n", current_pen_colorPreset);
		}
		if (MIDIwithColor) {
#ifndef ATELIERSENFANTS
			if (nb_pen_colorPresets > 0) {
				current_pen_colorPreset = pg_MIDINote % nb_pen_colorPresets;
				printf( "new palette %d\n", current_pen_colorPreset);
				BrokenInterpolationVar[_pen_color] = true;
				*((float *)ScenarioVarPointers[_pen_color]) = pen_colorPreset_values[current_pen_colorPreset];
				// sprintf(AuxString, "/pen_color %.3f", pen_colorPresets_values[current_pen_colorPreset]);
				// pg_send_message_udp((char *)"s", AuxString, (char *)"udp_TouchOSC_send");
				// sprintf(AuxString, "/message palette%s", pen_colorPresets_names[current_pen_colorPreset].c_str()); pg_send_message_udp((char *)"s", (char *)AuxString, (char *)"udp_TouchOSC_send");
				sprintf(AuxString, "/pen_colorPreset %d", current_pen_colorPreset); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
			}
#else
			// select hue/sat/value present
			// TODO
#endif
		}
		if (MIDIwithCameraFlash) {
			printf("Flash camera\n");
			pg_Make_flashCamera();
		}
#ifdef PG_WITH_PHOTO_FLASH
		if (MIDIwithPhotoFlash) {
			printf("Flash photo\n");
			pg_Make_flashPhoto();
		}
#endif
		if (MIDIwithBeat) {
			pg_BeatNo++;
			printf("beat %d\n", pg_BeatNo);
			ReceiveBeat();
		}
	}
	break;

	// ======================================== 
	// flash on/off values
	// ======================================== 
#ifdef PG_NB_CA_TYPES
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
#endif

#if defined (TEXTURED_QUAD_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 
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
#endif

	// ====================================== 
	// keystroke s: Snapshot                  
	// ====================================== 
	case _snapshot: {
		if (arguments[0] > 0) {
			pg_snapshot((char*)"jpg");
			pg_snapshot((char*)"svg");
		}

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

#ifdef PG_WITH_CAMERA_CAPTURE
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// +++++++++++++++++ CAMERA PARAMETERS +++++++++++++++++++++ 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	case _cameraWB_R_plus: {
		*((float*)ScenarioVarPointers[_cameraWB_R]) += 100.f;
		pg_camera_capture.set(CV_CAP_PROP_WHITE_BALANCE_RED_V, (*((float*)ScenarioVarPointers[_cameraWB_R])));
		CameraCurrent_exposure = *((float*)ScenarioVarPointers[_cameraWB_R]);
		break;
	}
	case _cameraWB_R_minus: {
		*((float*)ScenarioVarPointers[_cameraWB_R]) -= 100.f;
		pg_camera_capture.set(CV_CAP_PROP_WHITE_BALANCE_RED_V, (*((float*)ScenarioVarPointers[_cameraWB_R])));
		CameraCurrent_exposure = *((float*)ScenarioVarPointers[_cameraWB_R]);
		break;
	}
	case _cameraWB_B_plus: {
		*((float*)ScenarioVarPointers[_cameraWB_B]) += 100.f;
		pg_camera_capture.set(CV_CAP_PROP_WHITE_BALANCE_BLUE_U, (*((float*)ScenarioVarPointers[_cameraWB_B])));
		CameraCurrent_exposure = *((float*)ScenarioVarPointers[_cameraWB_B]);
		break;
	}
	case _cameraWB_B_minus: {
		*((float*)ScenarioVarPointers[_cameraWB_B]) -= 100.f;
		pg_camera_capture.set(CV_CAP_PROP_WHITE_BALANCE_BLUE_U, (*((float*)ScenarioVarPointers[_cameraWB_B])));
		CameraCurrent_exposure = *((float*)ScenarioVarPointers[_cameraWB_B]);
		break;
	}
	case _cameraExposure_plus: {
		*((float*)ScenarioVarPointers[_cameraExposure]) += 1.f;
		pg_camera_capture.set(CV_CAP_PROP_EXPOSURE, (*((float*)ScenarioVarPointers[_cameraExposure])));
		CameraCurrent_exposure = *((float*)ScenarioVarPointers[_cameraExposure]);
		break;
	}
	case _cameraExposure_minus: {
		*((float*)ScenarioVarPointers[_cameraExposure]) -= 1.f;
		pg_camera_capture.set(CV_CAP_PROP_EXPOSURE, (*((float*)ScenarioVarPointers[_cameraExposure])));
		CameraCurrent_exposure = *((float*)ScenarioVarPointers[_cameraExposure]);
		break;
	}
	case _cameraGain_plus: {
		*((float*)ScenarioVarPointers[_cameraGain]) += 1.f;
		pg_camera_capture.set(CV_CAP_PROP_GAIN, (*((float*)ScenarioVarPointers[_cameraGain])));
		CameraCurrent_exposure = *((float*)ScenarioVarPointers[_cameraGain]);
		break;
	}
	case _cameraGain_minus: {
		*((float*)ScenarioVarPointers[_cameraGain]) -= 1.f;
		pg_camera_capture.set(CV_CAP_PROP_GAIN, (*((float*)ScenarioVarPointers[_cameraGain])));
		CameraCurrent_exposure = *((float*)ScenarioVarPointers[_cameraGain]);
		break;
	}
	case _cameraBrightness_plus: {
		*((float*)ScenarioVarPointers[_cameraBrightness]) += 10.f;
		pg_camera_capture.set(CV_CAP_PROP_BRIGHTNESS, (*((float*)ScenarioVarPointers[_cameraBrightness])));
		CameraCurrent_exposure = *((float*)ScenarioVarPointers[_cameraBrightness]);
		break;
	}
	case _cameraBrightness_minus: {
		*((float*)ScenarioVarPointers[_cameraBrightness]) -= 10.f;
		pg_camera_capture.set(CV_CAP_PROP_BRIGHTNESS, (*((float*)ScenarioVarPointers[_cameraBrightness])));
		CameraCurrent_exposure = *((float*)ScenarioVarPointers[_cameraBrightness]);
		break;
	}
	case _cameraSaturation_plus: {
		*((float*)ScenarioVarPointers[_cameraSaturation]) += 10.f;
		pg_camera_capture.set(CV_CAP_PROP_SATURATION, (*((float*)ScenarioVarPointers[_cameraSaturation])));
		CameraCurrent_exposure = *((float*)ScenarioVarPointers[_cameraSaturation]);
		break;
	}
	case _cameraSaturation_minus: {
		*((float*)ScenarioVarPointers[_cameraSaturation]) -= 10.f;
		pg_camera_capture.set(CV_CAP_PROP_SATURATION, (*((float*)ScenarioVarPointers[_cameraSaturation])));
		CameraCurrent_exposure = *((float*)ScenarioVarPointers[_cameraSaturation]);
		break;
	}
	case _cameraContrast_plus: {
		*((float*)ScenarioVarPointers[_cameraContrast]) += 10.f;
		pg_camera_capture.set(CV_CAP_PROP_CONTRAST, (*((float*)ScenarioVarPointers[_cameraContrast])));
		CameraCurrent_exposure = *((float*)ScenarioVarPointers[_cameraContrast]);
		break;
	}
	case _cameraContrast_minus: {
		*((float*)ScenarioVarPointers[_cameraContrast]) -= 10.f;
		pg_camera_capture.set(CV_CAP_PROP_CONTRAST, (*((float*)ScenarioVarPointers[_cameraContrast])));
		CameraCurrent_exposure = *((float*)ScenarioVarPointers[_cameraContrast]);
		break;
	}
	case _cameraGamma_plus: {
		*((float*)ScenarioVarPointers[_cameraGamma]) += 10.f;
		pg_camera_capture.set(CV_CAP_PROP_GAMMA, (*((float*)ScenarioVarPointers[_cameraGamma])));
		CameraCurrent_exposure = *((float*)ScenarioVarPointers[_cameraGamma]);
		break;
	}
	case _cameraGamma_minus: {
		*((float*)ScenarioVarPointers[_cameraGamma]) -= 10.f;
		pg_camera_capture.set(CV_CAP_PROP_GAMMA, (*((float*)ScenarioVarPointers[_cameraGamma])));
		CameraCurrent_exposure = *((float*)ScenarioVarPointers[_cameraGamma]);
		break;
	}

	case _reset_on_camera: {
		reset_camera = true;
		// the camera weight is set to 1 in 10 frames
		delayedCameraWeight = 11;
		break;
	}
#endif

#ifdef PIERRES
	case _processing_video: {
		int argt0 = int(round(arguments[0]));
		sprintf(AuxString, "/video %d", argt0); pg_send_message_udp((char*)"i", (char*)AuxString, (char*)"udp_Processing_send");
		printf("Processing message %s\n", AuxString);
		break;
	}
	case _processing_image: {
		int argt0 = int(round(arguments[0]));
		sprintf(AuxString, "/image %d", argt0); pg_send_message_udp((char*)"i", (char*)AuxString, (char*)"udp_Processing_send");
		printf("Processing message %s\n", AuxString);
		break;
	}
	case _processing_master: {
		float argt0 = arguments[0];
		sprintf(AuxString, "/master %.2f", argt0); pg_send_message_udp((char*)"f", (char*)AuxString, (char*)"udp_Processing_send");
		printf("Processing message %s\n", AuxString);
		break;
	}
#endif
#if defined (PIERRES)
	case _TopLeft: {
		VP1KeystoneYTopLeft = -arguments[0] + 0;
		VP1KeystoneXTopLeft = arguments[1] + 0;
		updateXYKeystonePad();
		break;
	}
	case _BottomLeft: {
		VP1KeystoneYBottomLeft = -arguments[0] + 1.f;
		VP1KeystoneXBottomLeft = arguments[1] + 0;
		updateXYKeystonePad();
		break;
	}
	case _TopRight: {
		VP1KeystoneYTopRight = -arguments[0] + 0;
		VP1KeystoneXTopRight = arguments[1] + 1.f;
		updateXYKeystonePad();
		break;
	}
	case _BottomRight: {
		VP1KeystoneYBottomRight = -arguments[0] + 1.f;
		VP1KeystoneXBottomRight = arguments[1] + 1.f;
		updateXYKeystonePad();
		break;
	}
#endif
#ifdef PG_LIGHTS
	case _ZeroLight: {
		pg_SendDMXZeros();
#if PG_LIGHTS >= 1
		sprintf(AuxString, "/light1_grey %.4f", 0.f); pg_send_message_udp((char*)"f", AuxString, (char*)"udp_TouchOSC_send");
		sprintf(AuxString, "/light1_color %.4f", 0.f); pg_send_message_udp((char*)"f", AuxString, (char*)"udp_TouchOSC_send");
		sprintf(AuxString, "/light1_dimmer %.4f", 0.f); pg_send_message_udp((char*)"f", AuxString, (char*)"udp_TouchOSC_send");
		sprintf(AuxString, "/light1_strobe %.4f", 0.f); pg_send_message_udp((char*)"f", AuxString, (char*)"udp_TouchOSC_send");
		BrokenInterpolationVar[_light1_grey] = true;
		*((float*)ScenarioVarPointers[_light1_grey]) = 0.;
		BrokenInterpolationVar[_light1_color] = true;
		*((float*)ScenarioVarPointers[_light1_color]) = 0.;
		BrokenInterpolationVar[_light1_dimmer] = true;
		*((float*)ScenarioVarPointers[_light1_dimmer]) = 0.;
		BrokenInterpolationVar[_light1_strobe] = true;
		*((float*)ScenarioVarPointers[_light1_strobe]) = 0.;
#endif
#if PG_LIGHTS >= 2
		sprintf(AuxString, "/light2_grey %.4f", 0.f); pg_send_message_udp((char*)"f", AuxString, (char*)"udp_TouchOSC_send");
		sprintf(AuxString, "/light2_color %.4f", 0.f); pg_send_message_udp((char*)"f", AuxString, (char*)"udp_TouchOSC_send");
		sprintf(AuxString, "/light2_dimmer %.4f", 0.f); pg_send_message_udp((char*)"f", AuxString, (char*)"udp_TouchOSC_send");
		sprintf(AuxString, "/light2_strobe %.4f", 0.f); pg_send_message_udp((char*)"f", AuxString, (char*)"udp_TouchOSC_send");
		BrokenInterpolationVar[_light2_grey] = true;
		*((float*)ScenarioVarPointers[_light2_grey]) = 0.;
		BrokenInterpolationVar[_light2_color] = true;
		*((float*)ScenarioVarPointers[_light2_color]) = 0.;
		BrokenInterpolationVar[_light2_dimmer] = true;
		*((float*)ScenarioVarPointers[_light2_dimmer]) = 0.;
		BrokenInterpolationVar[_light2_strobe] = true;
		*((float*)ScenarioVarPointers[_light2_strobe]) = 0.;
#endif
#if PG_LIGHTS >= 3
		sprintf(AuxString, "/light3_grey %.4f", 0.f); pg_send_message_udp((char*)"f", AuxString, (char*)"udp_TouchOSC_send");
		sprintf(AuxString, "/light3_color %.4f", 0.f); pg_send_message_udp((char*)"f", AuxString, (char*)"udp_TouchOSC_send");
		sprintf(AuxString, "/light3_dimmer %.4f", 0.f); pg_send_message_udp((char*)"f", AuxString, (char*)"udp_TouchOSC_send");
		sprintf(AuxString, "/light3_strobe %.4f", 0.f); pg_send_message_udp((char*)"f", AuxString, (char*)"udp_TouchOSC_send");
		BrokenInterpolationVar[_light3_grey] = true;
		*((float*)ScenarioVarPointers[_light3_grey]) = 0.;
		BrokenInterpolationVar[_light3_color] = true;
		*((float*)ScenarioVarPointers[_light3_color]) = 0.;
		BrokenInterpolationVar[_light3_dimmer] = true;
		*((float*)ScenarioVarPointers[_light3_dimmer]) = 0.;
		BrokenInterpolationVar[_light3_strobe] = true;
		*((float*)ScenarioVarPointers[_light3_strobe]) = 0.;
#endif
#if PG_LIGHTS >= 4
		sprintf(AuxString, "/light4_grey %.4f", 0.f); pg_send_message_udp((char*)"f", AuxString, (char*)"udp_TouchOSC_send");
		sprintf(AuxString, "/light4_color %.4f", 0.f); pg_send_message_udp((char*)"f", AuxString, (char*)"udp_TouchOSC_send");
		sprintf(AuxString, "/light4_dimmer %.4f", 0.f); pg_send_message_udp((char*)"f", AuxString, (char*)"udp_TouchOSC_send");
		sprintf(AuxString, "/light4_strobe %.4f", 0.f); pg_send_message_udp((char*)"f", AuxString, (char*)"udp_TouchOSC_send");
		BrokenInterpolationVar[_light4_grey] = true;
		*((float*)ScenarioVarPointers[_light4_grey]) = 0.;
		BrokenInterpolationVar[_light4_color] = true;
		*((float*)ScenarioVarPointers[_light4_color]) = 0.;
		BrokenInterpolationVar[_light4_dimmer] = true;
		*((float*)ScenarioVarPointers[_light4_dimmer]) = 0.;
		BrokenInterpolationVar[_light4_strobe] = true;
		*((float*)ScenarioVarPointers[_light4_strobe]) = 0.;
#endif
#if PG_LIGHTS >= 5
		sprintf(AuxString, "/light5_grey %.4f", 0.f); pg_send_message_udp((char*)"f", AuxString, (char*)"udp_TouchOSC_send");
		sprintf(AuxString, "/light5_color %.4f", 0.f); pg_send_message_udp((char*)"f", AuxString, (char*)"udp_TouchOSC_send");
		sprintf(AuxString, "/light5_dimmer %.4f", 0.f); pg_send_message_udp((char*)"f", AuxString, (char*)"udp_TouchOSC_send");
		sprintf(AuxString, "/light5_strobe %.4f", 0.f); pg_send_message_udp((char*)"f", AuxString, (char*)"udp_TouchOSC_send");
		BrokenInterpolationVar[_light5_grey] = true;
		*((float*)ScenarioVarPointers[_light5_grey]) = 0.;
		BrokenInterpolationVar[_light5_color] = true;
		*((float*)ScenarioVarPointers[_light5_color]) = 0.;
		BrokenInterpolationVar[_light5_dimmer] = true;
		*((float*)ScenarioVarPointers[_light5_dimmer]) = 0.;
		BrokenInterpolationVar[_light5_strobe] = true;
		*((float*)ScenarioVarPointers[_light5_strobe]) = 0.;
#endif
#if PG_LIGHTS >= 6
		sprintf(AuxString, "/light6_grey %.4f", 0.f); pg_send_message_udp((char*)"f", AuxString, (char*)"udp_TouchOSC_send");
		sprintf(AuxString, "/light6_color %.4f", 0.f); pg_send_message_udp((char*)"f", AuxString, (char*)"udp_TouchOSC_send");
		sprintf(AuxString, "/light6_dimmer %.4f", 0.f); pg_send_message_udp((char*)"f", AuxString, (char*)"udp_TouchOSC_send");
		sprintf(AuxString, "/light6_strobe %.4f", 0.f); pg_send_message_udp((char*)"f", AuxString, (char*)"udp_TouchOSC_send");
		BrokenInterpolationVar[_light6_grey] = true;
		*((float*)ScenarioVarPointers[_light6_grey]) = 0.;
		BrokenInterpolationVar[_light6_color] = true;
		*((float*)ScenarioVarPointers[_light6_color]) = 0.;
		BrokenInterpolationVar[_light6_dimmer] = true;
		*((float*)ScenarioVarPointers[_light6_dimmer]) = 0.;
		BrokenInterpolationVar[_light6_strobe] = true;
		*((float*)ScenarioVarPointers[_light6_strobe]) = 0.;
#endif
#if PG_LIGHTS >= 7
		sprintf(AuxString, "/light7_grey %.4f", 0.f); pg_send_message_udp((char*)"f", AuxString, (char*)"udp_TouchOSC_send");
		sprintf(AuxString, "/light7_color %.4f", 0.f); pg_send_message_udp((char*)"f", AuxString, (char*)"udp_TouchOSC_send");
		sprintf(AuxString, "/light7_dimmer %.4f", 0.f); pg_send_message_udp((char*)"f", AuxString, (char*)"udp_TouchOSC_send");
		sprintf(AuxString, "/light7_strobe %.4f", 0.f); pg_send_message_udp((char*)"f", AuxString, (char*)"udp_TouchOSC_send");
		BrokenInterpolationVar[_light7_grey] = true;
		*((float*)ScenarioVarPointers[_light7_grey]) = 0.;
		BrokenInterpolationVar[_light7_color] = true;
		*((float*)ScenarioVarPointers[_light7_color]) = 0.;
		BrokenInterpolationVar[_light7_dimmer] = true;
		*((float*)ScenarioVarPointers[_light7_dimmer]) = 0.;
		BrokenInterpolationVar[_light7_strobe] = true;
		*((float*)ScenarioVarPointers[_light7_strobe]) = 0.;
#endif
#if PG_LIGHTS >= 8
		sprintf(AuxString, "/light8_grey %.4f", 0.f); pg_send_message_udp((char*)"f", AuxString, (char*)"udp_TouchOSC_send");
		sprintf(AuxString, "/light8_color %.4f", 0.f); pg_send_message_udp((char*)"f", AuxString, (char*)"udp_TouchOSC_send");
		sprintf(AuxString, "/light8_dimmer %.4f", 0.f); pg_send_message_udp((char*)"f", AuxString, (char*)"udp_TouchOSC_send");
		sprintf(AuxString, "/light8_strobe %.4f", 0.f); pg_send_message_udp((char*)"f", AuxString, (char*)"udp_TouchOSC_send");
		BrokenInterpolationVar[_light8_grey] = true;
		*((float*)ScenarioVarPointers[_light8_grey]) = 0.;
		BrokenInterpolationVar[_light8_color] = true;
		*((float*)ScenarioVarPointers[_light8_color]) = 0.;
		BrokenInterpolationVar[_light8_dimmer] = true;
		*((float*)ScenarioVarPointers[_light8_dimmer]) = 0.;
		BrokenInterpolationVar[_light8_strobe] = true;
		*((float*)ScenarioVarPointers[_light8_strobe]) = 0.;
#endif
		break;
	}
#endif 

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// +++++++++++++ FLASHES BETWEEN LAYERS, CA & PARTICLES ++++ 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// ====================================== 
	case _flashPixel:
		flashPixel = flashPixel_duration;
		break;

	case _flashTrkPart:
		flashTrkPart_weights[currentDrawingTrack] = 1.0;
		break;

	case _flashTrkCA:
		flashTrkCA_weights[currentDrawingTrack] = 1.0;
		break;

	case _flashCABG:
		flashCABG_weight = 1.0;

	case _flashCAPart:
		flashCAPart_weight = 1.0;
		break;

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// +++++++++++++++++ INTERFACE SELECTION +++++++++++++++++++ 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	case _minimal_display: {
		pg_send_message_udp((char *)"i", (char *)"/minimal_display 1", (char *)"udp_TouchOSC_send");
		break;
	}
	case _paths_display: {
		pg_send_message_udp((char *)"i", (char *)"/paths_display 1", (char *)"udp_TouchOSC_send");
		break;
	}

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// +++++++++++++++++ PEN POSITION ++++++++++++++++++++++++++ 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	case _new_scene: {
		if (pg_CurrentSceneIndex < 0) {
			pg_launch_performance(0);
		}
		else {
			StartNewScene(int(arguments[0]));
		}
		break;
	}

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// +++++++++++++++++ PEN POSITION ++++++++++++++++++++++++++ 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	case _pen_xy: {
		CurrentMousePos_x[0] = int(leftWindowWidth * arguments[0]);
		CurrentMousePos_y[0] = int(window_height * arguments[1]);
		sprintf(AuxString, "/pen_xy %.2f %.2f", arguments[0], arguments[1]); pg_send_message_udp((char*)"ff", (char*)AuxString, (char*)"udp_TouchOSC_send");
		break;
	}
	case _pen2_xy: {
		CurrentMousePos_x[1] = int(leftWindowWidth * arguments[0]);
		CurrentMousePos_y[1] = int(window_height * arguments[1]);
		sprintf(AuxString, "/pen2_xy %.2f %.2f", arguments[0], arguments[1]); pg_send_message_udp((char*)"ff", (char*)AuxString, (char*)"udp_TouchOSC_send");
		break;
	}
	case _pen3_xy: {
		CurrentMousePos_x[2] = int(leftWindowWidth * arguments[0]);
		CurrentMousePos_y[2] = int(window_height * arguments[1]);
		sprintf(AuxString, "/pen3_xy %.2f %.2f", arguments[0], arguments[1]); pg_send_message_udp((char*)"ff", (char*)AuxString, (char*)"udp_TouchOSC_send");
		break;
	}
	case _pen4_xy: {
		CurrentMousePos_x[3] = int(leftWindowWidth * arguments[0]);
		CurrentMousePos_y[3] = int(window_height * arguments[1]);
		sprintf(AuxString, "/pen4_xy %.2f %.2f", arguments[0], arguments[1]); pg_send_message_udp((char*)"ff", (char*)AuxString, (char*)"udp_TouchOSC_send");
		break;
	}
	case _pen5_xy: {
		CurrentMousePos_x[4] = int(leftWindowWidth * arguments[0]);
		CurrentMousePos_y[4] = int(window_height * arguments[1]);
		sprintf(AuxString, "/pen5_xy %.2f %.2f", arguments[0], arguments[1]); pg_send_message_udp((char*)"ff", (char*)AuxString, (char*)"udp_TouchOSC_send");
		break;
	}
	case _abs_pen_xy: {
		CurrentMousePos_x[0] = int(arguments[0]);
		CurrentMousePos_y[0] = int(arguments[1]);
		// printf("pos %.2f %.2f\n", arguments[0], arguments[1]);
		break;
	}

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// +++++++++++++++++ TRACK COPY +++++++++++++++++++++++++++ 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	case _copyTrack_above: {
		copyToNextTrack = +1;
		//sprintf(AuxString, "/message copy_track_%d_to_%d",
		//	currentDrawingTrack, (currentDrawingTrack + 1) % PG_NB_TRACKS);
		//pg_send_message_udp((char *)"s", (char *)AuxString, (char *)"udp_TouchOSC_send");

		break;
	}
	case _copyTrack_under: {
		copyToNextTrack = -1;
		//sprintf(AuxString, "/message copy_track_%d_to_%d", 
		//	currentDrawingTrack , (currentDrawingTrack - 1 + PG_NB_TRACKS) % PG_NB_TRACKS); 
		//pg_send_message_udp((char *)"s", (char *)AuxString, (char *)"udp_TouchOSC_send");


		break;
	}

#if defined (TEXTURED_QUAD_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// +++++++++++++++++ PART EXIT/STROKE/COLOR MODE +++++++++++++++++++++++++++ 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	case _partStroke_mode_0: {
		*((int *)ScenarioVarPointers[_partStroke_mode]) = 0;

		for (int ind = 0; ind < 3; ind++) {
			if (ind == *((int *)ScenarioVarPointers[_partStroke_mode])) {
				sprintf(AuxString, "/partStroke_mode_%d 1", ind); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
			}
			else {
				sprintf(AuxString, "/partStroke_mode_%d 0", ind); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
			}
		}
		break;
	}
	case _partStroke_mode_1: {
		*((int *)ScenarioVarPointers[_partStroke_mode]) = 1;

		for (int ind = 0; ind < 3; ind++) {
			if (ind == *((int *)ScenarioVarPointers[_partStroke_mode])) {
				sprintf(AuxString, "/partStroke_mode_%d 1", ind); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
			}
			else {
				sprintf(AuxString, "/partStroke_mode_%d 0", ind); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
			}
		}
		break;
	}
	case _partStroke_mode_2: {
		*((int *)ScenarioVarPointers[_partStroke_mode]) = 2;

		for (int ind = 0; ind < 3; ind++) {
			if (ind == *((int *)ScenarioVarPointers[_partStroke_mode])) {
				sprintf(AuxString, "/partStroke_mode_%d 1", ind); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
			}
			else {
				sprintf(AuxString, "/partStroke_mode_%d 0", ind); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
			}
		}
		break;
	}
	case _partColor_mode_0: {
		*((int *)ScenarioVarPointers[_partColor_mode]) = 0;

		for (int ind = 0; ind < 3; ind++) {
			if (ind == *((int *)ScenarioVarPointers[_partColor_mode])) {
				sprintf(AuxString, "/partColor_mode_%d 1", ind); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
			}
			else {
				sprintf(AuxString, "/partColor_mode_%d 0", ind); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
			}
		}
		break;
	}
	case _partColor_mode_1: {
		*((int *)ScenarioVarPointers[_partColor_mode]) = 1;

		for (int ind = 0; ind < 3; ind++) {
			if (ind == *((int *)ScenarioVarPointers[_partColor_mode])) {
				sprintf(AuxString, "/partColor_mode_%d 1", ind); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
			}
			else {
				sprintf(AuxString, "/partColor_mode_%d 0", ind); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
			}
		}
		break;
	}
	case _partColor_mode_2: {
		*((int *)ScenarioVarPointers[_partColor_mode]) = 2;

		for (int ind = 0; ind < 3; ind++) {
			if (ind == *((int *)ScenarioVarPointers[_partColor_mode])) {
				sprintf(AuxString, "/partColor_mode_%d 1", ind); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
			}
			else {
				sprintf(AuxString, "/partColor_mode_%d 0", ind); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
			}
		}
		break;
	}
	case _partExit_mode_0: {
		*((int *)ScenarioVarPointers[_partExit_mode]) = 0;

		for (int ind = 0; ind < 3; ind++) {
			if (ind == *((int *)ScenarioVarPointers[_partExit_mode])) {
				sprintf(AuxString, "/partExit_mode_%d 1", ind); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
			}
			else {
				sprintf(AuxString, "/partExit_mode_%d 0", ind); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
			}
		}
		break;
	}
	case _partExit_mode_1: {
		*((int *)ScenarioVarPointers[_partExit_mode]) = 1;

		for (int ind = 0; ind < 3; ind++) {
			if (ind == *((int *)ScenarioVarPointers[_partExit_mode])) {
				sprintf(AuxString, "/partExit_mode_%d 1", ind); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
			}
			else {
				sprintf(AuxString, "/partExit_mode_%d 0", ind); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
			}
		}
		break;
	}
	case _partExit_mode_2: {
		*((int *)ScenarioVarPointers[_partExit_mode]) = 2;

		for (int ind = 0; ind < 3; ind++) {
			if (ind == *((int *)ScenarioVarPointers[_partExit_mode])) {
				sprintf(AuxString, "/partExit_mode_%d 1", ind); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
			}
			else {
				sprintf(AuxString, "/partExit_mode_%d 0", ind); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
			}
		}
		break;
	}
#endif

#ifdef PG_WITH_BLUR
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
#endif


#ifdef PG_WITH_CAMERA_CAPTURE
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// +++++++++++++++++ CAMERA IMAGE CUMUL MODE +++++++++++++++ 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	case _cameraCumul_plus: {
		cameraCumul = (cameraCumul + 1) % PG_NB_CAMERA_CUMUL_MODES;
		BrokenInterpolationVar[_cameraCumul] = true;
		break;
	}
#endif 

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// +++++++++++++++++ BRUSH ID SHIFT ++++++++++++++++++++++++ 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	case _pen_brush_plus:
		if (nb_pen_brushes > 0) {
			pen_brush = (pen_brush + 1) % (nb_pen_brushes * 3);
			BrokenInterpolationVar[_pen_brush] = true;
			if (pen_brush >= 1) { // large radius for the image brushes
				pen_radiusMultiplier = 50.0f;
#if defined(PIERRES) || defined(ATELIERSENFANTS) || defined(SONG)
				pen_radiusMultiplier = 1.0f;
#endif
			}
			else {
				pen_radiusMultiplier = 1.0f;
			}

		}
		break;
	case _pen_brush_minus: 
		if (nb_pen_brushes > 0) {
			pen_brush = (pen_brush - 1 + (nb_pen_brushes * 3)) % (nb_pen_brushes * 3);
			BrokenInterpolationVar[_pen_brush] = true;
			if (pen_brush >= 1) { // large radius for the image brushes
				pen_radiusMultiplier = 50.0f;
#if defined(PIERRES) || defined(ATELIERSENFANTS) || defined(SONG)
				pen_radiusMultiplier = 1.0f;
#endif
			}
			else {
				pen_radiusMultiplier = 1.0f;
			}

		}
		break;

#if defined(PIERRES)
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// +++++++++++++++++ BRUSH DASH INCREASE ++++++++++++++++++++++++ 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	case _pen_position_dash_plus: {
		pen_position_dash = (pen_position_dash + 1) % 5;
		BrokenInterpolationVar[_pen_position_dash] = true;
		break;
	}
#endif

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// +++++++++++++++++ PIXEL MODE +++++++++++++++++++ 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
#ifdef PG_NB_PIXEL_MODES
	case _pixel_mode_plus: {
		pixel_mode = (pixel_mode + 1) % PG_NB_PIXEL_MODES;
		BrokenInterpolationVar[_pixel_mode] = true;
		break;
	}
#endif

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// +++++++++++++++++ CA TYPE AND SUBTYPE +++++++++++++++++++ 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
#ifdef PG_NB_CA_TYPES
	case _CA1Type_plus: {
		CA1Type = (CA1Type + 1) % PG_NB_CA_TYPES;
		BrokenInterpolationVar[_CA1Type] = true;
		// printf("CA1Type %d\n", CA1Type);
		*((int *)ScenarioVarPointers[_CA1Type]) = CA1Type;
		break;
	}
	case _CA1Type_minus: {
		CA1Type = (CA1Type - 1 + PG_NB_CA_TYPES) % PG_NB_CA_TYPES;
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
#endif

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
	case _sensor_sample_setUp_plus: {
		sensor_sample_setUp = (sensor_sample_setUp + 1);
		if (sensor_sample_setUp >= PG_NB_MAX_SAMPLE_SETUPS) {
			sensor_sample_setUp -= PG_NB_MAX_SAMPLE_SETUPS;
		}
		BrokenInterpolationVar[_sensor_sample_setUp] = true;
		sensor_sample_setUp_interpolation();
		break;
	}
	case _sensor_activation_plus: {
		sensor_activation = (sensor_activation + 1) % PG_NB_MAX_SENSOR_ACTIVATIONS;
		BrokenInterpolationVar[_sensor_activation] = true;
		assignSensorActivations();
		break;
	}
#endif

#if defined (TEXTURED_QUAD_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// +++++++++++++++++ PARTICLE MODES ++++++++++++++++++++++++
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
#endif

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// +++++++++++++++++ B/W SWITCH ++++++++++++++++++++++++++++ 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// ====================================== 
	case _pen_BW: {
#ifndef PEN_HSV
			if (pen_grey > 0) {
				pen_grey = 0.0f;
			}								
			else {
				pen_grey = 1.0f;
			}
			BrokenInterpolationVar[_pen_grey] = true;
#else
			if (pen_value > 0) {
				pen_value = 0.0f;
			}
			else {
				pen_value = 1.0f;
			}
			BrokenInterpolationVar[_pen_value] = true;
#endif
		break;
	}
	case _repopBG_BW: {
		if (repop_greyBG > 0) {
			repop_greyBG = 0.0f;
		}
		else {
			repop_greyBG = 1.0f;
		}
		BrokenInterpolationVar[_repop_greyBG] = true;
		// printf("repop color %.1f\n", repop_grey);
		break;
	}
#ifdef PG_NB_CA_TYPES
	case _repopCA_BW: {
		if (repop_greyCA > 0) {
			repop_greyCA = 0.0f;
		}
		else {
			repop_greyCA = 1.0f;
		}
		BrokenInterpolationVar[_repop_greyCA] = true;
		// printf("repop color %.1f\n", repop_grey);
		break;
	}
#endif
	case _repopPart_BW: {
#ifndef PART_HSV
		if (repop_greyPart > 0) {
			repop_greyPart = 0.0f;
		}
		else {
			repop_greyPart = 1.0f;
		}
		BrokenInterpolationVar[_repop_greyPart] = true;
#else
		if (repop_valuePart > 0) {
			repop_valuePart = 0.0f;
		}
		else {
			repop_valuePart = 1.0f;
		}
		BrokenInterpolationVar[_repop_valuePart] = true;
#endif
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
	// +++++++++++++++++ DIAPORAMA +++++++++++++++++++++++++++++ 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// ====================================== 
	case _diaporama_plus: {
		//for (int indPhoto = 0; indPhoto < PG_PHOTO_NB_TEXTURES; indPhoto++) {
		//	// incay and decay are 0 if is_capture_diaporama
		//	float incay = (is_capture_diaporama ? 0 : photo_diaporama_fade);
		//	float decay = (is_capture_diaporama ? 0 : photo_diaporama_fade);
		//	// blended photo is terminated to switch to the next one
		//	if (pg_Photo_swap_buffer_data[indPhoto].blendStart > 0.0f) {
		//		printf("load diaporama #%d\n", indPhoto);
		//		sprintf(AuxString, "/diaporama_shortName %03d", indPhoto);
		//		pg_send_message_udp((char *)"s", AuxString, (char *)"udp_TouchOSC_send");
		//		int nextCompressedImage
		//			= nextFileIndexMemoryLoop(pg_CurrentDiaporamaDir,
		//				&pg_CurrentDiaporamaFile, ascendingDiaporama);
		//		pg_Photo_swap_buffer_data[(indPhoto + 1) % PG_PHOTO_NB_TEXTURES].indSwappedPhoto
		//			= nextCompressedImage;
		//		pg_Photo_swap_buffer_data[indPhoto].blendStart
		//			= CurrentClockTime - (incay + photo_diaporama_plateau + decay);
		//		return;
		//	}
		//}
		// goes to the first photo diaporama if it is not already selected and if there is one 
		if (photo_diaporama < 0 && nb_photo_albums > 0) {
			photo_diaporama = 0;
		}
		pg_CurrentDiaporamaDir = (pg_CurrentDiaporamaDir + 1) % pg_nbCompressedImageDirs;
		//printf("pg_CurrentDiaporamaDir %d\n", pg_CurrentDiaporamaDir);
		sprintf(AuxString, "/diaporama_shortName %03d", pg_CurrentDiaporamaDir);
		pg_send_message_udp((char *)"s", AuxString, (char *)"udp_TouchOSC_send");
		pg_launch_diaporama();
		break;
	}
	case _diaporama_minus: {
		//for (int indPhoto = 0; indPhoto < PG_PHOTO_NB_TEXTURES; indPhoto++) {
		//	// incay and decay are 0 if is_capture_diaporama
		//	float incay = (is_capture_diaporama ? 0 : photo_diaporama_fade);
		//	float decay = (is_capture_diaporama ? 0 : photo_diaporama_fade);
		//	// blended photo is terminated to switch to the next one
		//	if (pg_Photo_swap_buffer_data[indPhoto].blendStart > 0.0f) {
		//		// change order (was decreasing before)
		//		// stops blending
		//		printf("load diaporama #%d\n", indPhoto);
		//		sprintf(AuxString, "/diaporama_shortName %03d", indPhoto);
		//		pg_send_message_udp((char *)"s", AuxString, (char *)"udp_TouchOSC_send");
		//		int nextCompressedImage
		//			= nextFileIndexMemoryLoop(pg_CurrentDiaporamaDir,
		//				&pg_CurrentDiaporamaFile, !ascendingDiaporama);
		//		pg_Photo_swap_buffer_data[(indPhoto + 1) % PG_PHOTO_NB_TEXTURES].indSwappedPhoto
		//			= nextCompressedImage;
		//		pg_Photo_swap_buffer_data[indPhoto].blendStart
		//			= CurrentClockTime - (incay + photo_diaporama_plateau + decay);
		//		return;
		//	}
		//}
		pg_CurrentDiaporamaDir = (pg_CurrentDiaporamaDir - 1 + pg_nbCompressedImageDirs) % pg_nbCompressedImageDirs;
		//printf("pg_CurrentDiaporamaDir %d\n", pg_CurrentDiaporamaDir);
		sprintf(AuxString, "/diaporama_shortName %03d", pg_CurrentDiaporamaDir);
		pg_send_message_udp((char *)"s", AuxString, (char *)"udp_TouchOSC_send");
		pg_launch_diaporama();
		break;
	}

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// +++++++++++++++++ TRACK NO ++++++++++++++++++++++++++++++ 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// ====================================== 
#if defined(PG_WITH_JUCE) || defined(PG_WITH_PUREDATA)
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
	case _soundtrack_minus: {
		if (nb_soundtracks > 0) {
			if (currentlyPlaying_trackNo >= 0) {
				currentlyPlaying_trackNo = (currentlyPlaying_trackNo + nb_soundtracks - 1) % nb_soundtracks;
			}
			else {
				currentlyPlaying_trackNo = 0;
			}
			PlayTrack(currentlyPlaying_trackNo);
			BrokenInterpolationVar[_playing_soundtrackNo] = true;
			*((int*)ScenarioVarPointers[_playing_soundtrackNo]) = currentlyPlaying_trackNo;
		}
		break;
	}
	case _soundtrack_onOff: {
		soundTrack_on = !soundTrack_on;
		soundTrackonOff();
		break;
	}
	case _soundtrack_volume: {
		soundTrack_on = (arguments[0] > 0);
		soundTrackvolume(arguments[0]);
		break;
	}
#endif

	case _movie_onOff: {
		movie_on = !movie_on;
		sprintf(AuxString, "/movie_onOff %d", int(movie_on));
		pg_send_message_udp((char*)"i", AuxString, (char*)"udp_TouchOSC_send");
		printf("Movie On/Off %d\n", int(movie_on));
		break;
	}

	case _movie_loop_onOff: {
		movie_loop = !movie_loop;
		sprintf(AuxString, "/movie_loop_onOff %d", int(movie_loop));
		pg_send_message_udp((char*)"i", AuxString, (char*)"udp_TouchOSC_send");
		printf("Movie loop On/Off %d\n", int(movie_loop));
		break;
	}

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// +++++++++++++++++ PALETTE NO ++++++++++++++++++++++++++++ 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// ====================================== 
#ifndef PEN_HSV
	case _pen_colorPreset_minus: {
		if (nb_pen_colorPresets > 0) {
			current_pen_colorPreset = (current_pen_colorPreset - 1 + nb_pen_colorPresets) % nb_pen_colorPresets;
			// printf( "/new palette %d\n", current_pen_colorPreset);
			BrokenInterpolationVar[_pen_color] = true;
			*((float *)ScenarioVarPointers[_pen_color]) = pen_colorPreset_values[current_pen_colorPreset];
			// sprintf(AuxString, "/pen_color %.3f", pen_colorPresets_values[current_pen_colorPreset]);
			// pg_send_message_udp((char *)"s", AuxString, (char *)"udp_TouchOSC_send");
			// sprintf(AuxString, "/message palette%s", pen_colorPresets_names[current_pen_colorPreset].c_str()); pg_send_message_udp((char *)"s", (char *)AuxString, (char *)"udp_TouchOSC_send");
			sprintf(AuxString, "/pen_colorPreset %d", current_pen_colorPreset); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
		}

		break;
	}
	case _pen_colorPreset_plus: {
		if (nb_pen_colorPresets > 0) {
			current_pen_colorPreset = (current_pen_colorPreset + 1) % nb_pen_colorPresets;
			BrokenInterpolationVar[_pen_color] = true;
			*((float *)ScenarioVarPointers[_pen_color]) = pen_colorPreset_values[current_pen_colorPreset];
			// sprintf(AuxString, "/pen_color %.3f", pen_colorPresets_values[current_pen_colorPreset]);
			// pg_send_message_udp((char *)"s", AuxString, (char *)"udp_TouchOSC_send");
			// sprintf(AuxString, "/message palette%s", pen_colorPresets_names[current_pen_colorPreset].c_str()); pg_send_message_udp((char *)"s", (char *)AuxString, (char *)"udp_TouchOSC_send");
			sprintf(AuxString, "/pen_colorPreset %d", current_pen_colorPreset); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
		}
		break;
	}
	case _pen_colorPreset: {
		int presetValue = int(arguments[0]);
		if (presetValue == -1) { // white
			pen_grey = 1.f;
			BrokenInterpolationVar[_pen_grey] = true;
			*((float*)ScenarioVarPointers[_pen_grey]) = pen_grey;
		}
		else if (nb_pen_colorPresets > 0) {
			pen_grey = 0.f;
			BrokenInterpolationVar[_pen_grey] = true;
			*((float*)ScenarioVarPointers[_pen_grey]) = pen_grey;

			current_pen_colorPreset = presetValue % nb_pen_colorPresets;
			BrokenInterpolationVar[_pen_color] = true;
			*((float *)ScenarioVarPointers[_pen_color]) = pen_colorPreset_values[current_pen_colorPreset];
			// sprintf(AuxString, "/pen_color %.3f", pen_colorPresets_values[current_pen_colorPreset]);
			// pg_send_message_udp((char *)"s", AuxString, (char *)"udp_TouchOSC_send");
			// sprintf(AuxString, "/message palette%s", pen_colorPresets_names[current_pen_colorPreset].c_str()); pg_send_message_udp((char *)"s", (char *)AuxString, (char *)"udp_TouchOSC_send");
			// printf("************* palette %d\n", current_pen_colorPreset);
			sprintf(AuxString, "/pen_colorPreset %d", current_pen_colorPreset); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
			printf("sent: %s\n", AuxString);
		}
		break;
	}
#endif

#if defined(CAAUDIO) || defined(RIVETS)
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
	// +++++++++++++++++ CA SEEDING ++++++++++++++++++++++++++++
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// ====================================== 

	case _CAseed_dot_center: {
		pg_CAseed_type = _pg_CAseed_dot_center;
		sprintf(AuxString, "/CAseed_dot_center %d", 1); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
		sprintf(AuxString, "/CAseed_dot %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
		sprintf(AuxString, "/CAseed_h_line %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
		sprintf(AuxString, "/CAseed_v_line %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
		sprintf(AuxString, "/CAseed_cross %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
		sprintf(AuxString, "/CAseed_X %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
		sprintf(AuxString, "/CAseed_square %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
		break;
	}
	case _CAseed_dot: {
		pg_CAseed_type = _pg_CAseed_dot;
		sprintf(AuxString, "/CAseed_dot_center %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
		sprintf(AuxString, "/CAseed_dot %d", 1); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
		sprintf(AuxString, "/CAseed_h_line %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
		sprintf(AuxString, "/CAseed_v_line %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
		sprintf(AuxString, "/CAseed_cross %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
		sprintf(AuxString, "/CAseed_X %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
		sprintf(AuxString, "/CAseed_square %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
		break;
	}
	case _CAseed_h_line: {
		sprintf(AuxString, "/CAseed_dot_center %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
		sprintf(AuxString, "/CAseed_dot %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
		sprintf(AuxString, "/CAseed_h_line %d", 1); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
		sprintf(AuxString, "/CAseed_v_line %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
		sprintf(AuxString, "/CAseed_cross %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
		sprintf(AuxString, "/CAseed_X %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
		sprintf(AuxString, "/CAseed_square %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
		pg_CAseed_type = _pg_CAseed_h_line;
		break;
	}
	case _CAseed_v_line: {
		sprintf(AuxString, "/CAseed_dot_center %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
		sprintf(AuxString, "/CAseed_dot %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
		sprintf(AuxString, "/CAseed_h_line %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
		sprintf(AuxString, "/CAseed_v_line %d", 1); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
		sprintf(AuxString, "/CAseed_cross %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
		sprintf(AuxString, "/CAseed_X %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
		sprintf(AuxString, "/CAseed_square %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
		pg_CAseed_type = _pg_CAseed_v_line;
		break;
	}
	case _CAseed_cross: {
		sprintf(AuxString, "/CAseed_dot_center %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
		sprintf(AuxString, "/CAseed_dot %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
		sprintf(AuxString, "/CAseed_h_line %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
		sprintf(AuxString, "/CAseed_v_line %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
		sprintf(AuxString, "/CAseed_cross %d", 1); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
		sprintf(AuxString, "/CAseed_X %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
		sprintf(AuxString, "/CAseed_square %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
		pg_CAseed_type = _pg_CAseed_cross;
		break;
	}
	case _CAseed_X: {
		sprintf(AuxString, "/CAseed_dot_center %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
		sprintf(AuxString, "/CAseed_dot %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
		sprintf(AuxString, "/CAseed_h_line %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
		sprintf(AuxString, "/CAseed_v_line %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
		sprintf(AuxString, "/CAseed_cross %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
		sprintf(AuxString, "/CAseed_X %d", 1); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
		sprintf(AuxString, "/CAseed_square %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
		pg_CAseed_type = _pg_CAseed_X;
		break;
	}
	case _CAseed_square: {
		sprintf(AuxString, "/CAseed_dot_center %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
		sprintf(AuxString, "/CAseed_dot %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
		sprintf(AuxString, "/CAseed_h_line %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
		sprintf(AuxString, "/CAseed_v_line %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
		sprintf(AuxString, "/CAseed_cross %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
		sprintf(AuxString, "/CAseed_X %d", 0); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
		sprintf(AuxString, "/CAseed_square %d", 1); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
		pg_CAseed_type = _pg_CAseed_square;
		break;
	}
	case _CAseed_size: {
		pg_CAseed_size = int(arguments[0]);
		sprintf(AuxString, "/CAseed_size %d", pg_CAseed_size); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
		break;
	}
	case _CAseed_loc: {
		pg_CAseed_location = pg_CAseed_locations(int(arguments[0]) % _pg_Nb_CAseed_locations);
		pg_CAseed_location_to_coordinates(pg_CAseed_location, pg_CAseed_coordinates);
		sprintf(AuxString, "/CAseed_loc %d", pg_CAseed_location); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
		break;
	}
	case _CAseed_loc_plus: {
		pg_CAseed_location = pg_CAseed_locations((pg_CAseed_location + 1) % _pg_Nb_CAseed_locations);
		pg_CAseed_location_to_coordinates(pg_CAseed_location, pg_CAseed_coordinates);
		sprintf(AuxString, "/CAseed_loc %d", pg_CAseed_location); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
		break;
	}
	case _CAseed_trigger: {
		pg_CAseed_trigger = true;
		break;
	}
#endif

#ifdef CRITON
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
	// +++++++++++++++++ SOUND CONTROL +++++++++++++++++++++++++
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// ====================================== 
	case _JUCE_loop_track: {
		pg_send_message_udp((char *)"", (char *)"/JUCE_loop_track", (char *)"udp_SoundJUCE_send");
		break;
	}
	case _JUCE_exit: {
		pg_send_message_udp((char *)"", (char *)"/JUCE_exit", (char *)"udp_SoundJUCE_send");
		break;
	}
	case _fftLevel8: {
		//printf("fft levels: ");
		//for (int indArg = 0; indArg < 8; indArg++) {
		//	printf("%.2f/%.2f ", arguments[2 * indArg], arguments[2 * indArg + 1]);
		//}
		//printf("\n");
		float totFFTLevel = 0.f;
		for (int indArg = 0; indArg < 8; indArg++) {
			fftFrequencies[indArg] = arguments[3 * indArg];
			fftLevels[indArg] = arguments[3 * indArg + 1];
			fftPhases[indArg] = arguments[3 * indArg + 2];
			totFFTLevel += fftLevels[indArg];
		}
		// normalization of the levels (sum = 0.5 (because cos + 1 used for color))
		totFFTLevel *= 2.f;
		if (totFFTLevel > 0.f) {
			for (int indArg = 0; indArg < 8; indArg++) {
				fftLevels[indArg] /= totFFTLevel;
			}
		}
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
			pg_play_movie_no();
		}
		break;
	}

	case _movie_minus: {
		if (nb_movies > 0) {
			playing_movieNo = (playing_movieNo - 1 + nb_movies) % nb_movies;
			pg_play_movie_no();
		}
		break;
	}

	// +++++++++++++++++ MOVIE FWD ++++++++++++++++++++++++++++++ 
	case _movie_forward: {
		if (currentlyPlaying_movieNo >= 0) {
			pg_movie_forward(int(movieCaptFreq * 10));
		}
		break;
	}
	case _movie_backward: {
		if (currentlyPlaying_movieNo >= 0) {
			pg_movie_backward(int(movieCaptFreq * 10));
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
	case _setup: {
		setup(int(round(arguments[0])));
		break;
	}
	case _setupName: {
		std::string sceneID = std::string(string_argument_0);
		std::regex newLines_re("\n+");
		std::regex dquotes_re("\"");
		sceneID = std::regex_replace(sceneID, newLines_re, "");
		sceneID = std::regex_replace(sceneID, dquotes_re, "");
		int ind_scene = FindSceneById(&sceneID);
		if (ind_scene >= 0 && ind_scene < pg_NbScenes) {
			setup(ind_scene);
		}
		else {
			sprintf(ErrorStr, "Unknown scene ID %s in /setupName OSC command (%d)!", string_argument_0, ind_scene); ReportError(ErrorStr);

		}
		break;
	}
	case _setup_plus: {
		setup_plus(1);
		break;
	}
	case _interpolation_duration: {
		pg_SceneInterpolationDuration = arguments[0];
		sprintf(AuxString, "/interpolation_duration %.2f", pg_SceneInterpolationDuration);
		pg_send_message_udp((char*)"i", AuxString, (char*)"udp_TouchOSC_send");
		break;
	}
	case _setup_plus_5: {
		setup_plus(5);
		break;
	}
	case _setup_minus: {
		setup_minus(1);
		break;
	}
	case _setup_plus_keep_total_dur: {
		float elapsedTime = (CurrentClockTime - InitialScenarioTime) - pg_CurrentScene->scene_initial_time;
		float deltaTime = pg_Scenario[1 + pg_CurrentSceneIndex].scene_initial_time - (CurrentClockTime - InitialScenarioTime);
		// only accepted if the current scene has been on for a while
		if (elapsedTime > 15
			// and if the scene has not begun since a short while
			&& deltaTime > 0
			// and if the scene is not too far from being finished
			&& deltaTime < 60) {
			int new_scene = ((1 + pg_CurrentSceneIndex) % pg_NbScenes);

			// we place the beginning of the current scene at this time
			pg_Scenario[new_scene].scene_initial_time -= deltaTime;
			pg_Scenario[new_scene].scene_duration += deltaTime;
			if (new_scene > 0) {
				pg_Scenario[new_scene - 1].scene_final_time -= deltaTime;
				pg_Scenario[new_scene - 1].scene_duration -= deltaTime;
			}
			// unuseful because is already made through pg_Scenario[new_scene].scene_initial_time -= deltaTime;
			//InitialScenarioTime = CurrentClockTime - pg_Scenario[new_scene].scene_initial_time;
			StartNewScene(new_scene);
		}
		break;
	}

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// +++++++++++++++++ SVG GPU +++++++++++++++++++++++++++++++ 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// ====================================== 
	case _ClipArt_1_onOff: ClipArt_OnOff(1); break;
	case _ClipArt_2_onOff: ClipArt_OnOff(2); break;
	case _ClipArt_3_onOff: ClipArt_OnOff(3); break;
	case _ClipArt_4_onOff: ClipArt_OnOff(4); break;
	case _ClipArt_5_onOff: ClipArt_OnOff(5); break;
	case _ClipArt_6_onOff: ClipArt_OnOff(6); break;
	case _ClipArt_7_onOff: ClipArt_OnOff(7); break;
	case _ClipArt_8_onOff: ClipArt_OnOff(8); break;
	case _ClipArt_9_onOff: ClipArt_OnOff(9); break;
	case _ClipArt_10_onOff: ClipArt_OnOff(10); break;
	case _ClipArt_11_onOff: ClipArt_OnOff(11); break;
	case _ClipArt_12_onOff: ClipArt_OnOff(12); break;
	case _ClipArt_13_onOff: ClipArt_OnOff(13); break;
	case _ClipArt_14_onOff: ClipArt_OnOff(14); break;
	case _ClipArt_15_onOff: ClipArt_OnOff(15); break;
	case _ClipArt_16_onOff: ClipArt_OnOff(16); break;
	case _ClipArt_17_onOff: ClipArt_OnOff(17); break;
	case _ClipArt_18_onOff: ClipArt_OnOff(18); break;
	case _ClipArt_19_onOff: ClipArt_OnOff(19); break;
	case _ClipArt_20_onOff: ClipArt_OnOff(20); break;

	case _ClipArt_onOff: ClipArt_OnOff(int(arguments[0])); break;
	case _ClipArt_off: ClipArt_Off(int(arguments[0])); break;
	case _ClipArt_on: ClipArt_On(int(arguments[0])); break;

	case _ClipArt_SubPath_1_onOff: ClipArt_SubPathOnOff(1); break;
	case _ClipArt_SubPath_2_onOff: ClipArt_SubPathOnOff(2); break;
	case _ClipArt_SubPath_3_onOff: ClipArt_SubPathOnOff(3); break;
	case _ClipArt_SubPath_4_onOff: ClipArt_SubPathOnOff(4); break;

	case _ClipArt_scale: 
		pg_ClipArt_Scale[pg_last_activated_ClipArt] = arguments[0]; 
		printf("SVG GPU scale %.2f\n", arguments[0]);
		break;
	case _ClipArt_rotate: 
		pg_ClipArt_Rotation[pg_last_activated_ClipArt] = arguments[0]; 
		printf("SVG GPU rotate %.2f\n", arguments[0]);
		break;
	case _ClipArt_xy: 
		pg_ClipArt_Translation_X[pg_last_activated_ClipArt] = arguments[0] * leftWindowWidth;
		pg_ClipArt_Translation_Y[pg_last_activated_ClipArt] = arguments[1] * window_height;
		printf("SVG GPU translate %.2fx%.2f\n", arguments[0] * leftWindowWidth, arguments[1] * window_height);
		break;
	case _ClipArt_nat_color: 
		pg_ClipArt_Colors[pg_last_activated_ClipArt] = ClipArt_nat; 
		break;
	case _ClipArt_white_color: 
		pg_ClipArt_Colors[pg_last_activated_ClipArt] = ClipArt_white; 
		break;
	case _ClipArt_red_color: 
		pg_ClipArt_Colors[pg_last_activated_ClipArt] = ClipArt_red; 
		break;
	case _ClipArt_green_color:
		pg_ClipArt_Colors[pg_last_activated_ClipArt] = ClipArt_green;
		break;
	case _ClipArt_blue_color:
		pg_ClipArt_Colors[pg_last_activated_ClipArt] = ClipArt_blue;
		break;
	case _ClipArt_yellow_color:
		pg_ClipArt_Colors[pg_last_activated_ClipArt] = ClipArt_yellow;
		break;
	case _ClipArt_cyan_color:
		pg_ClipArt_Colors[pg_last_activated_ClipArt] = ClipArt_cyan;
		break;
	case _ClipArt_magenta_color:
		pg_ClipArt_Colors[pg_last_activated_ClipArt] = ClipArt_magenta;
		break;
	case _ClipArt_translations:
		for(int indImage = 0; indImage < pg_nb_ClipArt && (2 * indImage + 1) < argc; indImage++) {
			pg_ClipArt_Translation_X[indImage] = arguments[2 * indImage] * leftWindowWidth;
			pg_ClipArt_Translation_Y[indImage] = arguments[2 * indImage + 1] * window_height;
		}
		// printf("SVG GPU translate %.2fx%.2f\n", arguments[0] * leftWindowWidth, arguments[1] * window_height);
		break;
#ifdef PG_MESHES
	case _Mesh_onOff: Mesh_OnOff(int(arguments[0]) + 1); break;
#ifdef CAVERNEPLATON
	case _Caverne_Mesh_Profusion_on: for (int indMesh = 7; indMesh < pg_nb_Mesh_files; indMesh++) { Caverne_Mesh_Profusion_On(indMesh); }; break;
	case _Caverne_Mesh_Profusion_off: for (int indMesh = 7; indMesh < pg_nb_Mesh_files; indMesh++) { Caverne_Mesh_Profusion_Off(indMesh); }; break;
	case _Caverne_Mesh_7Solids_on: for (int indMesh = 0; indMesh < 7; indMesh++) { Mesh_On(indMesh + 1); Mesh_mobile_Off(indMesh + 1); }; break;
	case _Caverne_Mesh_7Solids_off: for (int indMesh = 0; indMesh < 7; indMesh++) { Mesh_Off(indMesh + 1); Mesh_mobile_Off(indMesh + 1); }; break;
	case _Caverne_BackColor_onOff: Caverne_BackColor = !Caverne_BackColor; break;
#endif
	case _Mesh_mobile_onOff: Mesh_mobile_OnOff(int(arguments[0]) + 1); break;
#ifndef TEMPETE
	case _Mesh_light_x: mesh_light_x = arguments[0]; printf("MESH light x %.2f\n", arguments[0]);  break;
	case _Mesh_light_y: mesh_light_y = arguments[0]; printf("MESH light y %.2f\n", arguments[0]);  break;
	case _Mesh_light_z: mesh_light_z = arguments[0]; printf("MESH light z %.2f\n", arguments[0]);  break;
#endif
#endif
	case _take_snapshot:
		pg_draw_scene(_Jpg, false);
		sprintf(AuxString, "/snapshot_ID Snap_%04d", indJpgSnapshot); pg_send_message_udp((char *)"s", (char *)AuxString, (char *)"udp_TouchOSC_send");
		break;
	case _isClearAllLayers:
		isClearAllLayers = 1;
		break;
	case _StylusvsRubber:
		if (arguments[0] > 0) {
			CurrentCursorStylusvsRubber = pg_Stylus;
		}
		else {
			CurrentCursorStylusvsRubber = pg_Rubber;
		}
#ifdef ATELIERSENFANTS
		// drawing type initialization
		ExclusiveButtonsAndLabelsOnOff(StylusvsRubberButtonsPaths, StylusvsRubberButtonLabelsPaths, StylusvsRubberButtonValues, false, CurrentCursorStylusvsRubber);
#endif
		break;
#ifdef ATELIERSENFANTS
	case _pen_hue_sat_value:
		pen_hue = arguments[0];
		BrokenInterpolationVar[_pen_hue] = true;
		*((float*)ScenarioVarPointers[_pen_hue]) = pen_hue;
		pen_sat = arguments[1];
		BrokenInterpolationVar[_pen_sat] = true;
		*((float*)ScenarioVarPointers[_pen_sat]) = pen_sat;
		pen_value = arguments[2];
		BrokenInterpolationVar[_pen_value] = true;
		*((float*)ScenarioVarPointers[_pen_value]) = pen_value;
		HSVtoRGB(pen_hue, pen_sat, pen_value, &control_color[0], &control_color[1], &control_color[2]);
		sprintf(AuxString, "/color/couleur/color %02x%02x%02xFF", int(control_color[0] * 255), int(control_color[1] * 255), int(control_color[2] * 255)); pg_send_message_udp((char*)"s", (char*)AuxString, (char*)"udp_TouchOSC_send");
		//printf("color2 msg %s\n", AuxString);
		break;
	case _repop_hue_sat_valuePart:
		repop_huePart = arguments[0];
		BrokenInterpolationVar[_repop_huePart] = true;
		*((float*)ScenarioVarPointers[_repop_huePart]) = repop_huePart;
		repop_satPart = arguments[1];
		BrokenInterpolationVar[_repop_satPart] = true;
		*((float*)ScenarioVarPointers[_repop_satPart]) = repop_satPart;
		repop_valuePart = arguments[2];
		BrokenInterpolationVar[_repop_valuePart] = true;
		*((float*)ScenarioVarPointers[_repop_valuePart]) = repop_valuePart;
		HSVtoRGB(repop_huePart, repop_satPart, repop_valuePart, &control_color[0], &control_color[1], &control_color[2]);
		sprintf(AuxString, "/etoiles/part_couleur/color %02x%02x%02xFF", int(control_color[0] * 255), int(control_color[1] * 255), int(control_color[2] * 255)); pg_send_message_udp((char*)"s", (char*)AuxString, (char*)"udp_TouchOSC_send");
		//printf("color2 msg %s\n", AuxString);
		break;
#endif
	case _path_replay_stopAll:
		for (int pathNo = 1; pathNo <= PG_NB_PATHS; pathNo++) {
			if (is_path_replay[pathNo]) {
				pg_path_replay_trackNo_onOff(pathNo, -1);
				sprintf(AuxString, "/path_replay_trackNo_%d 0", pathNo);
				pg_send_message_udp((char*)"i", AuxString, (char*)"udp_TouchOSC_send");
				//printf("replay off track was currently read (%s)\n", AuxString);
			}
		}
		break;
	case _path_replay_playAll:
		for (int pathNo = 1; pathNo <= PG_NB_PATHS; pathNo++) {
			if (!is_path_replay[pathNo] && pg_Path_Status[pathNo].nbRecordedFrames > 0) {
				pg_path_replay_trackNo_onOff(pathNo, 1);
				sprintf(AuxString, "/path_replay_trackNo_%d 1", pathNo);
				pg_send_message_udp((char*)"i", AuxString, (char*)"udp_TouchOSC_send");
				//printf("replay off track was currently read (%s)\n", AuxString);
			}
		}
		break;
#ifdef PG_METAWEAR
	case _mw_mss_pos: {
		int ind_sensor = int(arguments[0]);
		if (ind_sensor < PG_MW_NB_MAX_SENSORS) {
			for (int i = 0; i < 3; i++) {
				pg_mw_sensors[ind_sensor].mw_mss_pos[i] = arguments[i + 1];
			}
			//printf("update position sensor #%d %.2f %.2f %.2f\n", ind_sensor, 
			//	pg_mw_sensors[ind_sensor].mw_mss_pos[0], pg_mw_sensors[ind_sensor].mw_mss_pos[1], pg_mw_sensors[ind_sensor].mw_mss_pos[2]);
			pg_mw_sensors[ind_sensor].mw_mss_pos_update = true;
		}
		else {
			sprintf(ErrorStr, "unregistered sensor #%d (max %d)!", ind_sensor, PG_MW_NB_MAX_SENSORS); ReportError(ErrorStr);
		}
	}
	break;
#endif
#if defined(BICHES)
	case _big_red_pen_track2: {
		pen_radius = 16.f;
		BrokenInterpolationVar[_pen_radius] = true;
		*((float *)ScenarioVarPointers[_pen_radius]) = pen_radius;
		pen_color = 0.97f;
		BrokenInterpolationVar[_pen_color] = true;
		*((float *)ScenarioVarPointers[_pen_color]) = pen_color;
		pen_grey = 0.f;
		BrokenInterpolationVar[_pen_grey] = true;
		*((float *)ScenarioVarPointers[_pen_grey]) = pen_grey;
		currentDrawingTrack = 2;
		BrokenInterpolationVar[_currentDrawingTrack] = true;
		*((int *)ScenarioVarPointers[_currentDrawingTrack]) = currentDrawingTrack;
	}
	break;
	case _big_green_pen_track1: {
		pen_radius = 16.f;
		BrokenInterpolationVar[_pen_radius] = true;
		*((float *)ScenarioVarPointers[_pen_radius]) = pen_radius;
		pen_color = 0.71096f;
		BrokenInterpolationVar[_pen_color] = true;
		*((float *)ScenarioVarPointers[_pen_color]) = pen_color;
		pen_grey = 0.f;
		BrokenInterpolationVar[_pen_grey] = true;
		*((float *)ScenarioVarPointers[_pen_grey]) = pen_grey;
		currentDrawingTrack = 1;
		BrokenInterpolationVar[_currentDrawingTrack] = true;
		*((int *)ScenarioVarPointers[_currentDrawingTrack]) = currentDrawingTrack;
	}
	break;
	case _small_black_pen_track2: {
		pen_radius = 2.f;
		BrokenInterpolationVar[_pen_radius] = true;
		*((float *)ScenarioVarPointers[_pen_radius]) = pen_radius;
		pen_grey = 1.f;
		BrokenInterpolationVar[_pen_grey] = true;
		*((float *)ScenarioVarPointers[_pen_grey]) = pen_grey;
		currentDrawingTrack = 2;
		BrokenInterpolationVar[_currentDrawingTrack] = true;
		*((int *)ScenarioVarPointers[_currentDrawingTrack]) = currentDrawingTrack;
	}
	break;
#endif

	default: {
			sprintf(ErrorStr, "Command not found (%s)!", command_symbol); ReportError(ErrorStr);
			break;
		}
	}
}

void ClipArt_OnOff(int indImage) {
	if (indImage <= pg_nb_ClipArt && activeClipArts != -1) { // activeClipArts == -1 <=> all ClipArts always visible
		bool isImageOn = activeClipArts & (1 << (indImage - 1));
		if (isImageOn) {
			activeClipArts = activeClipArts & ~(1 << (indImage - 1));
		}
		else {
			activeClipArts |= (1 << (indImage - 1));
			pg_last_activated_ClipArt = indImage - 1;
		}
		BrokenInterpolationVar[_activeClipArts] = true;
		*((int*)ScenarioVarPointers[_activeClipArts]) = activeClipArts;
		sprintf(AuxString, "/ClipArt_%d_onOff %d", indImage, (!isImageOn)); pg_send_message_udp((char*)"i", (char*)AuxString, (char*)"udp_TouchOSC_send");
	}
}

void ClipArt_Off(int indImage) {
	if (indImage <= pg_nb_ClipArt && activeClipArts != -1) { // activeClipArts == -1 <=> all ClipArts always visible
		bool isImageOn = activeClipArts & (1 << (indImage - 1));
		if (isImageOn) {
			activeClipArts = activeClipArts & ~(1 << (indImage - 1));
			BrokenInterpolationVar[_activeClipArts] = true;
			*((int*)ScenarioVarPointers[_activeClipArts]) = activeClipArts;
			sprintf(AuxString, "/ClipArt_%d_onOff %d", indImage, (!isImageOn)); pg_send_message_udp((char*)"i", (char*)AuxString, (char*)"udp_TouchOSC_send");
		}
	}
}

void ClipArt_On(int indImage) {
	if (indImage <= pg_nb_ClipArt && activeClipArts != -1) { // activeClipArts == -1 <=> all ClipArts always visible
		bool isImageOn = activeClipArts & (1 << (indImage - 1));
		if (!isImageOn) {
			activeClipArts |= (1 << (indImage - 1));
			pg_last_activated_ClipArt = indImage - 1;
			BrokenInterpolationVar[_activeClipArts] = true;
			*((int*)ScenarioVarPointers[_activeClipArts]) = activeClipArts;
			sprintf(AuxString, "/ClipArt_%d_onOff %d", indImage, (!isImageOn)); pg_send_message_udp((char*)"i", (char*)AuxString, (char*)"udp_TouchOSC_send");
		}
	}
}

#ifdef PG_MESHES
void Mesh_OnOff(int indImage) {
	if (indImage <= pg_nb_Mesh_files) {
		bool isImageOn = activeMeshes & (1 << (indImage - 1));
		if (isImageOn) {
			activeMeshes = activeMeshes & ~(1 << (indImage - 1));
		}
		else {
			activeMeshes |= (1 << (indImage - 1));
			pg_last_activated_Mesh = indImage - 1;
		}
		BrokenInterpolationVar[_activeMeshes] = true;
		*((int*)ScenarioVarPointers[_activeMeshes]) = activeMeshes;
		sprintf(AuxString, "/Mesh_%d_onOff %d", indImage, (!isImageOn)); pg_send_message_udp((char*)"i", (char*)AuxString, (char*)"udp_TouchOSC_send");
	}
}
#if defined(CAVERNEPLATON)
void Mesh_On(int indImage) {
	if (indImage <= pg_nb_Mesh_files) {
		bool isImageOn = activeMeshes & (1 << (indImage - 1));
		if (!isImageOn) {
			activeMeshes |= (1 << (indImage - 1));
			pg_last_activated_Mesh = indImage - 1;
		}
		BrokenInterpolationVar[_activeMeshes] = true;
		*((int*)ScenarioVarPointers[_activeMeshes]) = activeMeshes;
		sprintf(AuxString, "/Mesh_%d_onOff %d", indImage, 1); pg_send_message_udp((char*)"i", (char*)AuxString, (char*)"udp_TouchOSC_send");
	}
}
void Mesh_Off(int indImage) {
	if (indImage <= pg_nb_Mesh_files) {
		bool isImageOn = activeMeshes & (1 << (indImage - 1));
		if (isImageOn) {
			activeMeshes = activeMeshes & ~(1 << (indImage - 1));
		}
		BrokenInterpolationVar[_activeMeshes] = true;
		*((int*)ScenarioVarPointers[_activeMeshes]) = mobileMeshes;
		sprintf(AuxString, "/Mesh_%d_onOff %d", indImage, 0); pg_send_message_udp((char*)"i", (char*)AuxString, (char*)"udp_TouchOSC_send");
	}
}
void Caverne_Mesh_Profusion_On(int indImage) {
	pg_CaverneActveMesh[indImage] = !pg_CaverneActveMesh[indImage];
	pg_CaverneMeshBirthTime[indImage] = CurrentClockTime;
	pg_CaverneMeshDeathTime[indImage] = CurrentClockTime + 100000.f;
}
void Caverne_Mesh_Profusion_Off(int indImage) {
	pg_CaverneMeshDeathTime[indImage] = CurrentClockTime + pg_CaverneMeshWakeupTime[indImage] / 3.f;
}
#endif
void Mesh_mobile_OnOff(int indImage) {
	if (indImage <= pg_nb_Mesh_files) {
		bool isImageOn = mobileMeshes & (1 << (indImage - 1));
		printf("Mesh on %d\n", isImageOn);
		if (isImageOn) {
			mobileMeshes = mobileMeshes & ~(1 << (indImage - 1));
		}
		else {
			mobileMeshes |= (1 << (indImage - 1));
			pg_last_activated_Mesh = indImage - 1;
		}
		BrokenInterpolationVar[_mobileMeshes] = true;
		*((int*)ScenarioVarPointers[_mobileMeshes]) = mobileMeshes;
		sprintf(AuxString, "/Mesh_mobile_%d_onOff %d", indImage, (!isImageOn)); pg_send_message_udp((char*)"i", (char*)AuxString, (char*)"udp_TouchOSC_send");
	}
}
void Mesh_mobile_Off(int indImage) {
	if (indImage <= pg_nb_Mesh_files) {
		bool isImageOn = mobileMeshes & (1 << (indImage - 1));
		if (isImageOn) {
			mobileMeshes = mobileMeshes & ~(1 << (indImage - 1));
			pg_Mesh_Translation_X[indImage - 1] = pg_Mesh_Translation_Ini_X[indImage - 1];
			pg_Mesh_Translation_Y[indImage - 1] = pg_Mesh_Translation_Ini_Y[indImage - 1];
			pg_Mesh_Translation_Z[indImage - 1] = pg_Mesh_Translation_Ini_Z[indImage - 1];
			pg_Mesh_Rotation_X[indImage - 1] = pg_Mesh_Rotation_Ini_X[indImage - 1];
			pg_Mesh_Rotation_Y[indImage - 1] = pg_Mesh_Rotation_Ini_Y[indImage - 1];
			pg_Mesh_Rotation_Z[indImage - 1] = pg_Mesh_Rotation_Ini_Z[indImage - 1];
		}
		BrokenInterpolationVar[_mobileMeshes] = true;
		*((int*)ScenarioVarPointers[_mobileMeshes]) = mobileMeshes;
		sprintf(AuxString, "/Mesh_mobile_%d_onOff %d", indImage, 0); pg_send_message_udp((char*)"i", (char*)AuxString, (char*)"udp_TouchOSC_send");
	}
}
void Mesh_mobile_On(int indImage) {
	if (indImage <= pg_nb_Mesh_files) {
		bool isImageOn = mobileMeshes & (1 << (indImage - 1));
		if (!isImageOn) {
			mobileMeshes |= (1 << (indImage - 1));
			pg_last_activated_Mesh = indImage - 1;
		}
		BrokenInterpolationVar[_mobileMeshes] = true;
		*((int*)ScenarioVarPointers[_mobileMeshes]) = mobileMeshes;
		sprintf(AuxString, "/Mesh_mobile_%d_onOff %d", indImage, 1); pg_send_message_udp((char*)"i", (char*)AuxString, (char*)"udp_TouchOSC_send");
	}
}
#endif

void ClipArt_SubPathOnOff(int indPath) {
	if (indPath - 1 < 4) {
		pg_ClipArt_SubPath[pg_last_activated_ClipArt * 4 + indPath - 1] 
			= !pg_ClipArt_SubPath[pg_last_activated_ClipArt * 4 + indPath - 1];
	}
}

//////////////////////////////////////////////////////////////
// PULSE COLOR FUNCTIONS
//////////////////////////////////////////////////////////////
void compute_pulsed_palette_color(float color, float color_pulse, float grey, float grey_pulse, float pulsed_color[3], bool is_pen_color) {
	float lowPalette[3];
	int indLowPalette[3];
	int indUpperPalette[3];
	float pulsed_color_percentage[3];
	float bandpass_3color_palette[3][3];

	//if (color_pulse > 0) {
	//	for (int indColor = 0; indColor < 3; indColor++) {
	//		float pulsed_channel = (color + pulse[indColor] * color_pulse);
	//		lowPalette[indColor] = floor(pulsed_channel * nb_pen_palette_colors);
	//		while (lowPalette[indColor] < 0) {
	//			lowPalette[indColor] += nb_pen_palette_colors;
	//		}
	//		indLowPalette[indColor] = int(lowPalette[indColor]) % nb_pen_palette_colors;
	//		indUpperPalette[indColor] = (indLowPalette[indColor] + 1) % nb_pen_palette_colors;
	//		percentage[indColor] = pulsed_channel * nb_pen_palette_colors - lowPalette[indColor];
	//	}
	//}
	//else {
	//	for (int indColor = 0; indColor < 3; indColor++) {
	//		lowPalette[indColor] = floor(color * nb_pen_palette_colors);
	//		while (lowPalette[indColor] < 0) {
	//			lowPalette[indColor] += nb_pen_palette_colors;
	//		}
	//		indLowPalette[indColor] = int(lowPalette[indColor]) % nb_pen_palette_colors;
	//		indUpperPalette[indColor] = (indLowPalette[indColor] + 1) % nb_pen_palette_colors;
	//		percentage[indColor] = color * nb_pen_palette_colors - lowPalette[indColor];
	//	}
	//}

	for (int indColorBandPass = 0; indColorBandPass < 3; indColorBandPass++) {
		float pulsed_color;
		if (color_pulse != 0) {
			pulsed_color = color + pulse[indColorBandPass] * color_pulse;
		}
		else {
			pulsed_color = color;
		}
		pulsed_color *= nb_pen_palette_colors;
		pulsed_color_percentage[indColorBandPass] = modf(pulsed_color, &(lowPalette[indColorBandPass]));
		while (lowPalette[indColorBandPass] < 0) {
			lowPalette[indColorBandPass] += nb_pen_palette_colors;
		}
		indLowPalette[indColorBandPass] = int(lowPalette[indColorBandPass]) % nb_pen_palette_colors;
		indUpperPalette[indColorBandPass] = (indLowPalette[indColorBandPass] + 1) % nb_pen_palette_colors;
	}

	// the base palette is obtained by interpolating the two closest palettes according to color
	//printf("pulsed_color_percentage %.2f %.2f %.2f / %.2f\n", pulsed_color_percentage[0], pulsed_color_percentage[1], pulsed_color_percentage[2], color_pulse);
	for (int indColorBandPass = 0; indColorBandPass < 3; indColorBandPass++) {
		for (int indChannel = 0; indChannel < 3; indChannel++) {
			bandpass_3color_palette[indColorBandPass][indChannel]
				= (1.f - pulsed_color_percentage[indColorBandPass]) * pen_palette_colors_values[indLowPalette[indColorBandPass]][indColorBandPass * 3 + indChannel]
				+ pulsed_color_percentage[indColorBandPass] * pen_palette_colors_values[indUpperPalette[indColorBandPass]][indColorBandPass * 3 + indChannel];
			bandpass_3color_palette[indColorBandPass][indChannel] = min(1.f, bandpass_3color_palette[indColorBandPass][indChannel]);
		}
	}

	// stores the bandpass colors that will be shown in the palette interface in the Master shader for color visualization by user
	if (is_pen_color) {
		for (int indColorBandPass = 0; indColorBandPass < 3; indColorBandPass++) {
			for (int indChannel = 0; indChannel < 3; indChannel++) {
				pen_bandpass_3color_palette[indColorBandPass][indChannel] = bandpass_3color_palette[indColorBandPass][indChannel];
			}
		}

	}
	//printf("pen color %.2f palette low/upper/perc %d/%d/%.2f -> colors %.1f %.1f %.1f       %.1f %.1f %.1f        %.1f %.1f %.1f\n", 
	//	pen_color, indLowPenPalette, indUpperPenPalette, percentage, 
	//	bandpass_3color_palette[0][0], bandpass_3color_palette[0][1], bandpass_3color_palette[0][2], 
	//  bandpass_3color_palette[1][0], bandpass_3color_palette[1][1], bandpass_3color_palette[1][2], 
	//  bandpass_3color_palette[2][0], bandpass_3color_palette[2][1], bandpass_3color_palette[2][2]);
	// calculating the color from base luminance + palette colors modulated by the three frequence ranges
	for (int indChannel = 0; indChannel < 3; indChannel++) {
		// adding a base luminance
		pulsed_color[indChannel] = grey + pulse_average * grey_pulse;
		if (pulse_average != 0) {
			for (int indColorBandPass = 0; indColorBandPass < 3; indColorBandPass++) {
				pulsed_color[indChannel]
					+= bandpass_3color_palette[indColorBandPass][indChannel] * (pulse[indColorBandPass] / (3.f * pulse_average));
			}
		}
		else {
			for (int indColorBandPass = 0; indColorBandPass < 3; indColorBandPass++) {
				pulsed_color[indChannel]
					+= bandpass_3color_palette[indColorBandPass][indChannel] / 3.f;
			}
		}
		pulsed_color[indChannel] = min(1.f, pulsed_color[indChannel]);
	}
	float value = (pulsed_color[0] < pulsed_color[1]) ? pulsed_color[1] : pulsed_color[0];
	value = (value < pulsed_color[2]) ? pulsed_color[2] : value;
	// printf( "pulsed_color: %f %f %f\n" , pulsed_color[0] , pulsed_color[1] , pulsed_color[2] );

	// color_pulse negative => saturation of the pulsed color
	if (color_pulse < 0 && value > 0) {
		for (int indChannel = 0; indChannel < 3; indChannel++) {
			pulsed_color[indChannel] /= value;
		}
	}

	// the colors for drawing are inverted in case of inverted rendering, so that the drawing colors are not seen inverted
#if !defined(ULM)
	if (invertAllLayers) {
		pulsed_color[0] = value - pulsed_color[0];
		pulsed_color[1] = value - pulsed_color[1];
		pulsed_color[2] = value - pulsed_color[2];
		float new_value = (pulsed_color[0] < pulsed_color[1]) ? pulsed_color[1] : pulsed_color[0];
		new_value = (new_value < pulsed_color[2]) ? pulsed_color[2] : new_value;
		// the new value should be the same as the intial one, the difference is subtracted from the channels
		pulsed_color[0] += value - new_value;
		pulsed_color[1] += value - new_value;
		pulsed_color[2] += value - new_value;
	}
#endif
}

// from https://www.codespeedy.com/hsv-to-rgb-in-cpp/

void HSVtoRGB(float h, float s, float v, float* r, float* g, float* b) {
	if (h > 1.f || h < 0 || s>1.f || s < 0 || v>1.f || v < 0) {
		std::cout << "The givem HSV values are not in valid range" << std::endl;
		return;
	}
	float H = h * 360.f;
	float C = s * v;
	float X = C * float(1.f - abs(fmod(H / 60.0f, 2) - 1));
	float m = v - C;
	if (H >= 0 && H < 60.f) {
		*r = C, *g = X, *b = 0;
	}
	else if (H >= 60.f && H < 120.f) {
		*r = X, *g = C, *b = 0;
	}
	else if (H >= 120.f && H < 180.f) {
		*r = 0, *g = C, *b = X;
	}
	else if (H >= 180.f && H < 240.f) {
		*r = 0, *g = X, *b = C;
	}
	else if (H >= 240.f && H < 300.f) {
		*r = X, *g = 0, *b = C;
	}
	else {
		*r = C, *g = 0, *b = X;
	}
	*r += m;
	*g += m;
	*b += m;
	//cout << "R : " << R << endl;
	//cout << "G : " << G << endl;
	//cout << "B : " << B << endl;
}

void compute_pulsed_HSV_color(float hue, float hue_pulse, float sat, float sat_pulse, float value, float value_pulse, float pulsed_color[3], bool is_pen_color) {
	float pulsed_hue = hue + pulse_average * hue_pulse;
	float pulsed_sat = sat + pulse_average * sat_pulse;
	float pulsed_value = value + pulse_average * value_pulse;

	// ciruclar hue, keeps the value between 0 and 1
	if (pulsed_hue < 0) {
		float intpart;
		pulsed_hue = modf(pulsed_hue, &intpart) + 1.f;
	}
	else if (pulsed_hue > 1.f) {
		float intpart;
		pulsed_hue = modf(pulsed_hue, &intpart);
	}

	pulsed_sat = max(0.f, min(pulsed_sat, 1.f));
	pulsed_value = max(0.f, min(pulsed_value, 1.f));

	HSVtoRGB(pulsed_hue, pulsed_sat, pulsed_value, &pulsed_color[0], &pulsed_color[1], &pulsed_color[2]);
	//printf("pulsed color RGB %.2f %.2f %.2f \n", pulsed_color[0], pulsed_color[1], pulsed_color[2]);

	// the hues for drawing are inverted in case of inverted rendering, so that the drawing hues are not seen inverted
#if !defined(ULM)
	if (invertAllLayers) {
		pulsed_color[0] = pulsed_value - pulsed_color[0];
		pulsed_color[1] = pulsed_value - pulsed_color[1];
		pulsed_color[2] = pulsed_value - pulsed_color[2];
		float new_value = (pulsed_color[0] < pulsed_color[1]) ? pulsed_color[1] : pulsed_color[0];
		new_value = (new_value < pulsed_color[2]) ? pulsed_color[2] : new_value;
		// the new value should be the same as the intial one, the difference is subtracted from the channels
		pulsed_color[0] += pulsed_value - new_value;
		pulsed_color[1] += pulsed_value - new_value;
		pulsed_color[2] += pulsed_value - new_value;
	}
#endif
}

#ifdef PG_LIGHTS
void pg_StoreAllLights(void) {
#if PG_LIGHTS >= 1
	for (int ind_light = 0; ind_light < nb_lights; ind_light++) {
		if (lights_group[ind_light] == 1) {
			pg_StoreDMX(ind_light, pulsed_light1_dimmer, pulsed_light1_strobe, pulsed_light1_color);
		}
	}
#endif
#if PG_LIGHTS >= 2
	for (int ind_light = 0; ind_light < nb_lights; ind_light++) {
		if (lights_group[ind_light] == 2) {
			pg_StoreDMX(ind_light, pulsed_light2_dimmer, pulsed_light2_strobe, pulsed_light2_color);
		}
	}
#endif
#if PG_LIGHTS >= 3
	for (int ind_light = 0; ind_light < nb_lights; ind_light++) {
		if (lights_group[ind_light] == 3) {
			pg_StoreDMX(ind_light, pulsed_light3_dimmer, pulsed_light3_strobe, pulsed_light3_color);
		}
	}
#endif
#if PG_LIGHTS >= 4
	for (int ind_light = 0; ind_light < nb_lights; ind_light++) {
		if (lights_group[ind_light] == 4) {
			pg_StoreDMX(ind_light, pulsed_light4_dimmer, pulsed_light4_strobe, pulsed_light4_color);
		}
	}
#endif
#if PG_LIGHTS >= 5
	for (int ind_light = 0; ind_light < nb_lights; ind_light++) {
		if (lights_group[ind_light] == 5) {
			pg_StoreDMX(ind_light, pulsed_light5_dimmer, pulsed_light5_strobe, pulsed_light5_color);
		}
	}
#endif
#if PG_LIGHTS >= 6
	for (int ind_light = 0; ind_light < nb_lights; ind_light++) {
		if (lights_group[ind_light] == 6) {
			pg_StoreDMX(ind_light, pulsed_light6_dimmer, pulsed_light6_strobe, pulsed_light6_color);
		}
	}
#endif
#if PG_LIGHTS >= 7
	for (int ind_light = 0; ind_light < nb_lights; ind_light++) {
		if (lights_group[ind_light] == 7) {
			pg_StoreDMX(ind_light, pulsed_light7_dimmer, pulsed_light7_strobe, pulsed_light7_color);
		}
	}
#endif
#if PG_LIGHTS >= 8
	for (int ind_light = 0; ind_light < nb_lights; ind_light++) {
		if (lights_group[ind_light] == 8) {
			pg_StoreDMX(ind_light, pulsed_light8_dimmer, pulsed_light8_strobe, pulsed_light8_color);
		}
	}
#endif
}


void pg_SendDMXUpdate() {
	bool oneLightChanged = false;
#if PG_LIGHTS >= 1
	if (light1_grey != light1_grey_prec || light1_grey_pulse != light1_grey_pulse_prec
		|| light1_color != light1_color_prec || light1_color_pulse != light1_color_pulse_prec
		|| light1_dimmer != light1_dimmer_prec || light1_dimmer_pulse != light1_dimmer_pulse_prec
		|| light1_strobe != light1_strobe_prec || light1_strobe_pulse != light1_strobe_pulse_prec
		|| ((light1_grey_pulse != 0 || light1_color_pulse != 0 || light1_dimmer_pulse != 0 || light1_strobe_pulse != 0)
			&& (pulse_light_prec[0] != pulse[0] || pulse_light_prec[1] != pulse[1] || pulse_light_prec[2] != pulse[2]))
#ifdef CAVERNEPLATON
		// video background and light color are the same and randomly changed
		|| Caverne_BackColorRed != Caverne_BackColorRed_prec
		|| Caverne_BackColorGreen != Caverne_BackColorGreen_prec
		|| Caverne_BackColorBlue != Caverne_BackColorBlue_prec
#endif
		) {
		compute_pulsed_palette_color(light1_color, light1_color_pulse, light1_grey, light1_grey_pulse, pulsed_light1_color, false);
		pulsed_light1_dimmer = light1_dimmer * (1.f + pulse_average * light1_dimmer_pulse);
		pulsed_light1_strobe = light1_strobe * (1.f + pulse_average * light1_strobe_pulse);
		light1_color_prec = light1_color;
		light1_color_pulse_prec = light1_color_pulse;
		light1_grey_prec = light1_grey;
		light1_grey_pulse_prec = light1_grey_pulse;
		light1_dimmer_prec = light1_dimmer;
		light1_dimmer_pulse_prec = light1_dimmer_pulse;
		light1_strobe_prec = light1_strobe;
		light1_strobe_pulse_prec = light1_strobe_pulse;
#ifdef CAVERNEPLATON
		// video background and light color are the same and randomly changed
		if (Caverne_BackColor) {
			pulsed_light1_color[0] = Caverne_BackColorRed;
			pulsed_light1_color[1] = Caverne_BackColorGreen;
			pulsed_light1_color[2] = Caverne_BackColorBlue;
			Caverne_BackColorRed_prec = Caverne_BackColorRed;
			Caverne_BackColorGreen_prec = Caverne_BackColorGreen;
			Caverne_BackColorBlue_prec = Caverne_BackColorBlue;
		}
#endif
		oneLightChanged = true;
	}
#endif
#if PG_LIGHTS >= 2
	if (light2_grey != light2_grey_prec || light2_grey_pulse != light2_grey_pulse_prec
		|| light2_color != light2_color_prec || light2_color_pulse != light2_color_pulse_prec
		|| light2_dimmer != light2_dimmer_prec || light2_dimmer_pulse != light2_dimmer_pulse_prec
		|| light2_strobe != light2_strobe_prec || light2_strobe_pulse != light2_strobe_pulse_prec
		|| ((light2_grey_pulse != 0 || light2_color_pulse != 0 || light2_dimmer_pulse != 0 || light2_strobe_pulse != 0)
			&& (pulse_light_prec[0] != pulse[0] || pulse_light_prec[1] != pulse[1] || pulse_light_prec[2] != pulse[2]))) {
		compute_pulsed_palette_color(light2_color, light2_color_pulse, light2_grey, light2_grey_pulse, pulsed_light2_color, false);
		pulsed_light2_dimmer = light2_dimmer * (1.f + pulse_average * light2_dimmer_pulse);
		pulsed_light2_strobe = light2_strobe * (1.f + pulse_average * light2_strobe_pulse);
		light2_color_prec = light2_color;
		light2_color_pulse_prec = light2_color_pulse;
		light2_grey_prec = light2_grey;
		light2_grey_pulse_prec = light2_grey_pulse;
		light2_dimmer_prec = light2_dimmer;
		light2_dimmer_pulse_prec = light2_dimmer_pulse;
		light2_strobe_prec = light2_strobe;
		light2_strobe_pulse_prec = light2_strobe_pulse;
		oneLightChanged = true;
	}
#endif
#if PG_LIGHTS >= 3
	if (light3_grey != light3_grey_prec || light3_grey_pulse != light3_grey_pulse_prec
		|| light3_color != light3_color_prec || light3_color_pulse != light3_color_pulse_prec
		|| light3_dimmer != light3_dimmer_prec || light3_dimmer_pulse != light3_dimmer_pulse_prec
		|| light3_strobe != light3_strobe_prec || light3_strobe_pulse != light3_strobe_pulse_prec
		|| ((light3_grey_pulse != 0 || light3_color_pulse != 0 || light3_dimmer_pulse != 0 || light3_strobe_pulse != 0)
			&& (pulse_light_prec[0] != pulse[0] || pulse_light_prec[1] != pulse[1] || pulse_light_prec[2] != pulse[2]))) {
		compute_pulsed_palette_color(light3_color, light3_color_pulse, light3_grey, light3_grey_pulse, pulsed_light3_color, false);
		pulsed_light3_dimmer = light3_dimmer * (1.f + pulse_average * light3_dimmer_pulse);
		pulsed_light3_strobe = light3_strobe * (1.f + pulse_average * light3_strobe_pulse);
		light3_color_prec = light3_color;
		light3_color_pulse_prec = light3_color_pulse;
		light3_grey_prec = light3_grey;
		light3_grey_pulse_prec = light3_grey_pulse;
		light3_dimmer_prec = light3_dimmer;
		light3_dimmer_pulse_prec = light3_dimmer_pulse;
		light3_strobe_prec = light3_strobe;
		light3_strobe_pulse_prec = light3_strobe_pulse;
		oneLightChanged = true;
	}
#endif
#if PG_LIGHTS >= 4
	if (light4_grey != light4_grey_prec || light4_grey_pulse != light4_grey_pulse_prec
		|| light4_color != light4_color_prec || light4_color_pulse != light4_color_pulse_prec
		|| light4_dimmer != light4_dimmer_prec || light4_dimmer_pulse != light4_dimmer_pulse_prec
		|| light4_strobe != light4_strobe_prec || light4_strobe_pulse != light4_strobe_pulse_prec
		|| ((light4_grey_pulse != 0 || light4_color_pulse != 0 || light4_dimmer_pulse != 0 || light4_strobe_pulse != 0)
			&& (pulse_light_prec[0] != pulse[0] || pulse_light_prec[1] != pulse[1] || pulse_light_prec[2] != pulse[2]))) {
		compute_pulsed_palette_color(light4_color, light4_color_pulse, light4_grey, light4_grey_pulse, pulsed_light4_color, false);
		pulsed_light4_dimmer = light4_dimmer * (1.f + pulse_average * light4_dimmer_pulse);
		pulsed_light4_strobe = light4_strobe * (1.f + pulse_average * light4_strobe_pulse);
		light4_color_prec = light4_color;
		light4_color_pulse_prec = light4_color_pulse;
		light4_grey_prec = light4_grey;
		light4_grey_pulse_prec = light4_grey_pulse;
		light4_dimmer_prec = light4_dimmer;
		light4_dimmer_pulse_prec = light4_dimmer_pulse;
		light4_strobe_prec = light4_strobe;
		light4_strobe_pulse_prec = light4_strobe_pulse;
		oneLightChanged = true;
	}
#endif
#if PG_LIGHTS >= 5
	if (light5_grey != light5_grey_prec || light5_grey_pulse != light5_grey_pulse_prec
		|| light5_color != light5_color_prec || light5_color_pulse != light5_color_pulse_prec
		|| light5_dimmer != light5_dimmer_prec || light5_dimmer_pulse != light5_dimmer_pulse_prec
		|| light5_strobe != light5_strobe_prec || light5_strobe_pulse != light5_strobe_pulse_prec
		|| ((light5_grey_pulse != 0 || light5_color_pulse != 0 || light5_dimmer_pulse != 0 || light5_strobe_pulse != 0)
			&& (pulse_light_prec[0] != pulse[0] || pulse_light_prec[1] != pulse[1] || pulse_light_prec[2] != pulse[2]))) {
		compute_pulsed_palette_color(light5_color, light5_color_pulse, light5_grey, light5_grey_pulse, pulsed_light5_color, false);
		pulsed_light5_dimmer = light5_dimmer * (1.f + pulse_average * light5_dimmer_pulse);
		pulsed_light5_strobe = light5_strobe * (1.f + pulse_average * light5_strobe_pulse);
		light5_color_prec = light5_color;
		light5_color_pulse_prec = light5_color_pulse;
		light5_grey_prec = light5_grey;
		light5_grey_pulse_prec = light5_grey_pulse;
		light5_dimmer_prec = light5_dimmer;
		light5_dimmer_pulse_prec = light5_dimmer_pulse;
		light5_strobe_prec = light5_strobe;
		light5_strobe_pulse_prec = light5_strobe_pulse;
		oneLightChanged = true;
	}
#endif
#if PG_LIGHTS >= 6
	if (light6_grey != light6_grey_prec || light6_grey_pulse != light6_grey_pulse_prec
		|| light6_color != light6_color_prec || light6_color_pulse != light6_color_pulse_prec
		|| light6_dimmer != light6_dimmer_prec || light6_dimmer_pulse != light6_dimmer_pulse_prec
		|| light6_strobe != light6_strobe_prec || light6_strobe_pulse != light6_strobe_pulse_prec
		|| ((light6_grey_pulse != 0 || light6_color_pulse != 0 || light6_dimmer_pulse != 0 || light6_strobe_pulse != 0)
			&& (pulse_light_prec[0] != pulse[0] || pulse_light_prec[1] != pulse[1] || pulse_light_prec[2] != pulse[2]))) {
		compute_pulsed_palette_color(light6_color, light6_color_pulse, light6_grey, light6_grey_pulse, pulsed_light6_color, false);
		pulsed_light6_dimmer = light6_dimmer * (1.f + pulse_average * light6_dimmer_pulse);
		pulsed_light6_strobe = light6_strobe * (1.f + pulse_average * light6_strobe_pulse);
		light6_color_prec = light6_color;
		light6_color_pulse_prec = light6_color_pulse;
		light6_grey_prec = light6_grey;
		light6_grey_pulse_prec = light6_grey_pulse;
		light6_dimmer_prec = light6_dimmer;
		light6_dimmer_pulse_prec = light6_dimmer_pulse;
		light6_strobe_prec = light6_strobe;
		light6_strobe_pulse_prec = light6_strobe_pulse;
		oneLightChanged = true;
	}
#endif
#if PG_LIGHTS >= 7
	if (light7_grey != light7_grey_prec || light7_grey_pulse != light7_grey_pulse_prec
		|| light7_color != light7_color_prec || light7_color_pulse != light7_color_pulse_prec
		|| light7_dimmer != light7_dimmer_prec || light7_dimmer_pulse != light7_dimmer_pulse_prec
		|| light7_strobe != light7_strobe_prec || light7_strobe_pulse != light7_strobe_pulse_prec
		|| ((light7_grey_pulse != 0 || light7_color_pulse != 0 || light7_dimmer_pulse != 0 || light7_strobe_pulse != 0)
			&& (pulse_light_prec[0] != pulse[0] || pulse_light_prec[1] != pulse[1] || pulse_light_prec[2] != pulse[2]))
		) {
		compute_pulsed_palette_color(light7_color, light7_color_pulse, light7_grey, light7_grey_pulse, pulsed_light7_color, false);
		pulsed_light7_dimmer = light7_dimmer * (1.f + pulse_average * light7_dimmer_pulse);
		pulsed_light7_strobe = light7_strobe * (1.f + pulse_average * light7_strobe_pulse);
		light7_color_prec = light7_color;
		light7_color_pulse_prec = light7_color_pulse;
		light7_grey_prec = light7_grey;
		light7_grey_pulse_prec = light7_grey_pulse;
		light7_dimmer_prec = light7_dimmer;
		light7_dimmer_pulse_prec = light7_dimmer_pulse;
		light7_strobe_prec = light7_strobe;
		light7_strobe_pulse_prec = light7_strobe_pulse;
		oneLightChanged = true;
	}
#endif
#if PG_LIGHTS >= 8
	if (light8_grey != light8_grey_prec || light8_grey_pulse != light8_grey_pulse_prec
		|| light8_color != light8_color_prec || light8_color_pulse != light8_color_pulse_prec
		|| light8_dimmer != light8_dimmer_prec || light8_dimmer_pulse != light8_dimmer_pulse_prec
		|| light8_strobe != light8_strobe_prec || light8_strobe_pulse != light8_strobe_pulse_prec
		|| ((light8_grey_pulse != 0 || light8_color_pulse != 0 || light8_dimmer_pulse != 0 || light8_strobe_pulse != 0)
			&& (pulse_light_prec[0] != pulse[0] || pulse_light_prec[1] != pulse[1] || pulse_light_prec[2] != pulse[2]))
#ifdef CAVERNEPLATON
		// video background and light color are the same and randomly changed
		|| (Caverne_BackColor && Caverne_BackColorFlash != Caverne_BackColorFlash_prec)
#endif
		) {
		compute_pulsed_palette_color(light8_color, light8_color_pulse, light8_grey, light8_grey_pulse, pulsed_light8_color, false);
		pulsed_light8_dimmer = light8_dimmer * (1.f + pulse_average * light8_dimmer_pulse);
		pulsed_light8_strobe = light8_strobe * (1.f + pulse_average * light8_strobe_pulse);
		light8_color_prec = light8_color;
		light8_color_pulse_prec = light8_color_pulse;
		light8_grey_prec = light8_grey;
		light8_grey_pulse_prec = light8_grey_pulse;
		light8_dimmer_prec = light8_dimmer;
		light8_dimmer_pulse_prec = light8_dimmer_pulse;
		light8_strobe_prec = light8_strobe;
		light8_strobe_pulse_prec = light8_strobe_pulse;
#ifdef CAVERNEPLATON
		// video background and light color are the same and randomly changed
		if (Caverne_BackColor && Caverne_BackColorFlash != Caverne_BackColorFlash_prec) {
			if (Caverne_BackColorFlash) {
				pulsed_light8_dimmer = 0.2f * pulsed_light1_dimmer;
				Caverne_BackColorFlash = false;
				Caverne_BackColorFlash_prec = true;
			}
			else {
				Caverne_BackColorFlash = false;
				Caverne_BackColorFlash_prec = false;
				pulsed_light8_dimmer = 0.f;
			}
		}
#endif
		oneLightChanged = true;
	}
#endif

	if (oneLightChanged) {
#ifdef CAVERNEPLATON
		//printf("pulsed_light8_dimmer %.2f           pulsed_light1_color[0] %.2f\n", pulsed_light8_dimmer, pulsed_light1_color[0]);
#endif
		pulse_light_prec[0] = pulse[0];
		pulse_light_prec[1] = pulse[1];
		pulse_light_prec[2] = pulse[2];

		pg_StoreAllLights();
		pg_SendDMX();
	}
}
#endif

void pg_update_pulsed_colors(void) {
	////////////////////////////////////////////////////////////////////////////////////////////////////
	// PEN PULSED COLOR
	// blending the two closest palettes of the pulsed pen color
#ifndef PEN_HSV
	if (pen_color != pen_color_prec || pen_color_pulse != pen_color_pulse_prec
		|| pen_grey != pen_grey_prec || pen_grey_pulse != pen_grey_pulse_prec
		|| ((pen_grey_pulse != 0 || pen_color_pulse != 0)
			&& (pulse_prec[0] != pulse[0] || pulse_prec[1] != pulse[1] || pulse_prec[2] != pulse[2]))) {
		compute_pulsed_palette_color(pen_color, pen_color_pulse, pen_grey, pen_grey_pulse, pulsed_pen_color, true);
		pen_color_prec = pen_color;
		pen_color_pulse_prec = pen_color_pulse;
		pen_grey_prec = pen_grey;
		pen_grey_pulse_prec = pen_grey_pulse;
		pulse_prec[0] = pulse[0];
		pulse_prec[1] = pulse[1];
		pulse_prec[2] = pulse[2];
	}
#else
	if (pen_hue != pen_hue_prec || pen_hue_pulse != pen_hue_pulse_prec
		|| pen_sat != pen_sat_prec || pen_sat_pulse != pen_sat_pulse_prec
		|| pen_value != pen_value_prec || pen_value_pulse != pen_value_pulse_prec
		|| ((pen_sat_pulse != 0 || pen_value_pulse != 0 || pen_hue_pulse != 0)
			&& (pulse_prec[0] != pulse[0] || pulse_prec[1] != pulse[1] || pulse_prec[2] != pulse[2]))) {
		compute_pulsed_HSV_color(pen_hue, pen_hue_pulse, pen_sat, pen_sat_pulse, pen_value, pen_value_pulse, pulsed_pen_color, true);
		pen_hue_prec = pen_hue;
		pen_hue_pulse_prec = pen_hue_pulse;
		pen_sat_prec = pen_sat;
		pen_sat_pulse_prec = pen_sat_pulse;
		pen_value_prec = pen_value;
		pen_value_pulse_prec = pen_value_pulse;
		pulse_prec[0] = pulse[0];
		pulse_prec[1] = pulse[1];
		pulse_prec[2] = pulse[2];
	}
#endif
	pulsed_pen_color[3] = 1.f;

	/***************************** SHOULD BE REACTIVATED WHEN THE INTERFACE CAN DISPLAY COLORS */
	//sprintf(AuxString, "/pen_colorPenPalette %.5f %.5f %.5f %.5f %.5f %.5f %.5f %.5f %.5f",
	//	pen_bandpass_3color_palette[0][0], pen_bandpass_3color_palette[0][1], pen_bandpass_3color_palette[0][2], 
	//  pen_bandpass_3color_palette[1][0], pen_bandpass_3color_palette[1][1], pen_bandpass_3color_palette[1][2], 
	//  pen_bandpass_3color_palette[2][0], pen_bandpass_3color_palette[2][1], pen_bandpass_3color_palette[2][2]);
	//pg_send_message_udp((char *)"fffffffff", AuxString, (char *)"udp_TouchOSC_send");
	//sprintf(AuxString, "/pulsed_pen_color %.5f %.5f %.5f",
	//	pulsed_pen_color[0], pulsed_pen_color[1], pulsed_pen_color[2]);
	//pg_send_message_udp((char *)"fff", AuxString, (char *)"udp_TouchOSC_send");


	////////////////////////////////////////////////////////////////////////////////////////////////////
	// REPOP PULSED COLOR
	// blending the two closest palettes of the pulsed Repop color
	if (repop_colorBG != repop_colorBG_prec || repop_colorBG_pulse != repop_colorBG_pulse_prec
		|| repop_greyBG != repop_greyBG_prec || repop_greyBG_pulse != repop_greyBG_pulse_prec
		|| ((repop_greyBG_pulse != 0 || repop_colorBG_pulse != 0)
			&& (pulse_prec[0] != pulse[0] || pulse_prec[1] != pulse[1] || pulse_prec[2] != pulse[2]))) {
		compute_pulsed_palette_color(repop_colorBG, repop_colorBG_pulse, repop_greyBG, repop_greyBG_pulse, pulsed_repop_colorBG, false);
		repop_colorBG_prec = repop_colorBG;
		repop_colorBG_pulse_prec = repop_colorBG_pulse;
		repop_greyBG_prec = repop_greyBG;
		repop_greyBG_pulse_prec = repop_greyBG_pulse;
		pulse_prec[0] = pulse[0];
		pulse_prec[1] = pulse[1];
		pulse_prec[2] = pulse[2];
	}


	////////////////////////////////////////////////////////////////////////////////////////////////////
	// LIGHT PULSED COLOR
	// blending the two closest palettes of the pulsed Repop color
	//printf("%.2f %.2f\n", light1_dimmer, light1_dimmer_prec);
#ifdef PG_LIGHTS
	if (pg_FrameNo % 10 == 0) {
		pg_SendDMXUpdate();
	}

	//if (l1_prec != l1 ||
	//	l2_prec != l2 ||
	//	l3_prec != l3 ||
	//	l4_prec != l4 ||
	//	l5_prec != l5 ||
	//	l6_prec != l6 ||
	//	l7_prec != l7 ||
	//	l8_prec != l8 ||
	//	l9_prec != l9 ||
	//	l10_prec != l10 ||
	//	l11_prec != l11 ||
	//	l12_prec != l12) {
	//	pg_StoreDMX_11channels(2, 256, l1, l2, l3, l4, l5, l6, l7, l8, l9, l10, l11);
	//	l1_prec = l1;
	//	l2_prec = l2;
	//	l3_prec = l3;
	//	l4_prec = l4;
	//	l5_prec = l5;
	//	l6_prec = l6;
	//	l7_prec = l7;
	//	l8_prec = l8;
	//	l9_prec = l9;
	//	l10_prec = l10;
	//	l11_prec = l11;
	//	l12_prec = l12;
	//}

#endif

#ifdef PG_NB_CA_TYPES
	if (repop_colorCA != repop_colorCA_prec || repop_colorCA_pulse != repop_colorCA_pulse_prec
		|| repop_greyCA != repop_greyCA_prec || repop_greyCA_pulse != repop_greyCA_pulse_prec
		|| ((repop_greyCA_pulse != 0 || repop_colorCA_pulse != 0)
			&& (pulse_prec[0] != pulse[0] || pulse_prec[1] != pulse[1] || pulse_prec[2] != pulse[2]))) {
		compute_pulsed_palette_color(repop_colorCA, repop_colorCA_pulse, repop_greyCA, repop_greyCA_pulse, pulsed_repop_colorCA, false);
		repop_colorCA_prec = repop_colorCA;
		repop_colorCA_pulse_prec = repop_colorCA_pulse;
		repop_greyCA_prec = repop_greyCA;
		repop_greyCA_pulse_prec = repop_greyCA_pulse;
		pulse_prec[0] = pulse[0];
		pulse_prec[1] = pulse[1];
		pulse_prec[2] = pulse[2];
	}
#endif
#ifndef PART_HSV
	if (repop_colorPart != repop_colorPart_prec || repop_colorPart_pulse != repop_colorPart_pulse_prec
		|| repop_greyPart != repop_greyPart_prec || repop_greyPart_pulse != repop_greyPart_pulse_prec
		|| ((repop_greyPart_pulse != 0.f || repop_colorPart_pulse != 0.f)
			&& (pulse_prec[0] != pulse[0] || pulse_prec[1] != pulse[1] || pulse_prec[2] != pulse[2]))) {
		compute_pulsed_palette_color(repop_colorPart, repop_colorPart_pulse, repop_greyPart, repop_greyPart_pulse, pulsed_repop_colorPart, false);
		repop_colorPart_prec = repop_colorPart;
		repop_colorPart_pulse_prec = repop_colorPart_pulse;
		repop_greyPart_prec = repop_greyPart;
		repop_greyPart_pulse_prec = repop_greyPart_pulse;
		pulse_prec[0] = pulse[0];
		pulse_prec[1] = pulse[1];
		pulse_prec[2] = pulse[2];
	}
#else
	if (repop_huePart != repop_huePart_prec || repop_huePart_pulse != repop_huePart_pulse_prec
		|| repop_satPart != repop_satPart_prec || repop_satPart_pulse != repop_satPart_pulse_prec
		|| repop_valuePart != repop_valuePart_prec || repop_valuePart_pulse != repop_valuePart_pulse_prec
		|| ((repop_satPart_pulse != 0.f || repop_valuePart_pulse != 0.f || repop_huePart_pulse != 0.f)
			&& (pulse_prec[0] != pulse[0] || pulse_prec[1] != pulse[1] || pulse_prec[2] != pulse[2]))) {
		compute_pulsed_HSV_color(repop_huePart, repop_huePart_pulse, repop_satPart, repop_satPart_pulse, repop_valuePart, repop_valuePart_pulse, pulsed_repop_colorPart, false);
		repop_huePart_prec = repop_huePart;
		repop_huePart_pulse_prec = repop_huePart_pulse;
		repop_satPart_prec = repop_satPart;
		repop_satPart_pulse_prec = repop_satPart_pulse;
		repop_valuePart_prec = repop_valuePart;
		repop_valuePart_pulse_prec = repop_valuePart_pulse;
		pulse_prec[0] = pulse[0];
		pulse_prec[1] = pulse[1];
		pulse_prec[2] = pulse[2];
	}
#endif

	/***************************** SHOULD BE REACTIVATED WHEN THE INTERFACE CAN DISPLAY COLORS */
	//sprintf(AuxString, "/repop_colorRepopPalette %.5f %.5f %.5f %.5f %.5f %.5f %.5f %.5f %.5f",
	//  pen_bandpass_3color_palette[0][0], pen_bandpass_3color_palette[0][1], pen_bandpass_3color_palette[0][2], 
	//  pen_bandpass_3color_palette[1][0], pen_bandpass_3color_palette[1][1], pen_bandpass_3color_palette[1][2], 
	//  pen_bandpass_3color_palette[2][0], pen_bandpass_3color_palette[2][1], pen_bandpass_3color_palette[2][2]);
	//pg_send_message_udp((char *)"fffffffff", AuxString, (char *)"udp_TouchOSC_send");
	//sprintf(AuxString, "/pulsed_repop_color %.5f %.5f %.5f",
	//	pulsed_repop_color[0], pulsed_repop_color[1], pulsed_repop_color[2]);
	//pg_send_message_udp((char *)"fff", AuxString, (char *)"udp_TouchOSC_send");
}

//////////////////////////////////////////////////////////////
// TRACK LIB FUNCTIONS
//////////////////////////////////////////////////////////////

void pg_path_recording_onOff( int indPath ) {
  // switches between recording on/off 
  bool is_path_record = false;
  switch (indPath) {
#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7 || PG_NB_PATHS == 11
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
#if PG_NB_PATHS == 7 || PG_NB_PATHS == 11
  case 4:
	  is_path_record = !path_record_4;
	  *((bool*)ScenarioVarPointers[_path_record_4]) = is_path_record;
	  break;
  case 5:
	  is_path_record = !path_record_5;
	  *((bool*)ScenarioVarPointers[_path_record_5]) = is_path_record;
	  break;
  case 6:
	  is_path_record = !path_record_6;
	  *((bool*)ScenarioVarPointers[_path_record_6]) = is_path_record;
	  break;
  case 7:
	  is_path_record = !path_record_7;
	  *((bool*)ScenarioVarPointers[_path_record_7]) = is_path_record;
	  break;
#endif
#if PG_NB_PATHS == 11
  case 8:
	  is_path_record = !path_record_8;
	  *((bool*)ScenarioVarPointers[_path_record_8]) = is_path_record;
	  break;
  case 9:
	  is_path_record = !path_record_9;
	  *((bool*)ScenarioVarPointers[_path_record_9]) = is_path_record;
	  break;
  case 10:
	  is_path_record = !path_record_10;
	  *((bool*)ScenarioVarPointers[_path_record_10]) = is_path_record;
	  break;
  case 11:
	  is_path_record = !path_record_11;
	  *((bool*)ScenarioVarPointers[_path_record_11]) = is_path_record;
	  break;
#endif
  }

  // starts recording source 
  if( is_path_record ) {
	// is reading -> has to stop reading 
	// printf("Launches recording indPath %d: is replay %d\n", indPath, is_path_replay[indPath]);
	// printf("Replay indPath %d: %d\n", indPath, is_path_replay[indPath]);
	if( is_path_replay[indPath]) {
		// printf("Stops Replay indPath %d: %d\n", indPath, is_path_replay[indPath]);
		pg_path_replay_trackNo_stop(indPath);
	}

	// launches source recording 
	pg_path_recording_start(indPath);
	sprintf(AuxString, "/path_record_%d 1", indPath);
	pg_send_message_udp((char *)"i", AuxString, (char *)"udp_TouchOSC_send");

	// recording can only be made with one finger
#ifdef ATELIERSENFANTS
	NumberOfInteractionFingers(1);
#endif
  } else {
	// stops source recording 
	pg_path_recording_stop(indPath);
	sprintf(AuxString, "/path_record_%d 0", indPath);
	pg_send_message_udp((char *)"i", AuxString, (char *)"udp_TouchOSC_send");
  }
}

void pg_path_replay_trackNo_onOff(int indPath, int trackNo) {
	bool ispath_record = false;
	switch (indPath) {
#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7 || PG_NB_PATHS == 11
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
#if PG_NB_PATHS == 7 || PG_NB_PATHS == 11
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
#if PG_NB_PATHS == 11
	case 8:
		ispath_record = path_record_8;
		break;
	case 9:
		ispath_record = path_record_9;
		break;
	case 10:
		ispath_record = path_record_10;
		break;
	case 11:
		ispath_record = path_record_11;
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
#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7 || PG_NB_PATHS == 11
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
#if PG_NB_PATHS == 7 || PG_NB_PATHS == 11
	case 4:
		*((int*)ScenarioVarPointers[_path_replay_trackNo_4]) = -1;
		break;
	case 5:
		*((int*)ScenarioVarPointers[_path_replay_trackNo_5]) = -1;
		break;
	case 6:
		*((int*)ScenarioVarPointers[_path_replay_trackNo_6]) = -1;
		break;
	case 7:
		*((int*)ScenarioVarPointers[_path_replay_trackNo_7]) = -1;
		break;
#endif
#if PG_NB_PATHS == 11
	case 8:
		*((int*)ScenarioVarPointers[_path_replay_trackNo_8]) = -1;
		break;
	case 9:
		*((int*)ScenarioVarPointers[_path_replay_trackNo_9]) = -1;
		break;
	case 10:
		*((int*)ScenarioVarPointers[_path_replay_trackNo_10]) = -1;
		break;
	case 11:
		*((int*)ScenarioVarPointers[_path_replay_trackNo_11]) = -1;
		break;
#endif
	}

  }
}

void pg_path_recording_start(int indPath) {
	switch (indPath) {
#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7 || PG_NB_PATHS == 11
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
#if PG_NB_PATHS == 7 || PG_NB_PATHS == 11
	case 4:
		*((bool*)ScenarioVarPointers[_path_record_4]) = true;
		break;
	case 5:
		*((bool*)ScenarioVarPointers[_path_record_5]) = true;
		break;
	case 6:
		*((bool*)ScenarioVarPointers[_path_record_6]) = true;
		break;
	case 7:
		*((bool*)ScenarioVarPointers[_path_record_7]) = true;
		break;
#endif
#if PG_NB_PATHS == 11
	case 8:
		*((bool*)ScenarioVarPointers[_path_record_8]) = true;
		break;
	case 9:
		*((bool*)ScenarioVarPointers[_path_record_9]) = true;
		break;
	case 10:
		*((bool*)ScenarioVarPointers[_path_record_10]) = true;
		break;
	case 11:
		*((bool*)ScenarioVarPointers[_path_record_11]) = true;
		break;
#endif
	}
	if (indPath >= 1 && indPath <= PG_NB_PATHS) {
		pg_Path_Status[indPath].isActiveRecording = true;
		pg_Path_Status[indPath].nbRecordedFrames = 0;
		pg_Path_Status[indPath].readSpeedScale = 1.0f;
		// printf("start recording track %d\n",indPath);
	}
}

void pg_path_recording_stop(int indPath) {
	switch (indPath) {
#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7 || PG_NB_PATHS == 11
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
#if PG_NB_PATHS == 7 || PG_NB_PATHS == 11
	case 4:
		*((bool*)ScenarioVarPointers[_path_record_4]) = false;
		break;
	case 5:
		*((bool*)ScenarioVarPointers[_path_record_5]) = false;
		break;
	case 6:
		*((bool*)ScenarioVarPointers[_path_record_6]) = false;
		break;
	case 7:
		*((bool*)ScenarioVarPointers[_path_record_7]) = false;
		break;
#endif
#if PG_NB_PATHS == 11
	case 8:
		*((bool*)ScenarioVarPointers[_path_record_8]) = false;
		break;
	case 9:
		*((bool*)ScenarioVarPointers[_path_record_9]) = false;
		break;
	case 10:
		*((bool*)ScenarioVarPointers[_path_record_10]) = false;
		break;
	case 11:
		*((bool*)ScenarioVarPointers[_path_record_11]) = false;
		break;
#endif
	}
	if (indPath >= 1 && indPath <= PG_NB_PATHS
		&& pg_Path_Status[indPath].isActiveRecording == true) {
		pg_Path_Status[indPath].isActiveRecording = false;
		// printf("start playing %d\n",indPath);
		sprintf(AuxString, "/path_record_%d 0", indPath);
		pg_send_message_udp((char *)"i", AuxString, (char *)"udp_TouchOSC_send");
	}
}

void pg_path_replay_trackNo_start(int indPath, int trackNo) {
	if (indPath >= 1 && indPath <= PG_NB_PATHS
		&& !is_path_replay[indPath]
		&& pg_Path_Status[indPath].nbRecordedFrames > 0) {
		is_path_replay[indPath] = true;
		//printf("replay path No %d on track %d with %d frames\n", indPath, trackNo, pg_Path_Status[indPath].nbRecordedFrames);
		switch (indPath) {
#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7 || PG_NB_PATHS == 11
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
#if PG_NB_PATHS == 7 || PG_NB_PATHS == 11
		case 4:
			*((int*)ScenarioVarPointers[_path_replay_trackNo_4]) = trackNo;
			break;
		case 5:
			*((int*)ScenarioVarPointers[_path_replay_trackNo_5]) = trackNo;
			break;
		case 6:
			*((int*)ScenarioVarPointers[_path_replay_trackNo_6]) = trackNo;
			break;
		case 7:
			*((int*)ScenarioVarPointers[_path_replay_trackNo_7]) = trackNo;
			break;
#endif
#if PG_NB_PATHS == 11
		case 8:
			*((int*)ScenarioVarPointers[_path_replay_trackNo_8]) = trackNo;
			break;
		case 9:
			*((int*)ScenarioVarPointers[_path_replay_trackNo_9]) = trackNo;
			break;
		case 10:
			*((int*)ScenarioVarPointers[_path_replay_trackNo_10]) = trackNo;
			break;
		case 11:
			*((int*)ScenarioVarPointers[_path_replay_trackNo_11]) = trackNo;
			break;
#endif
		}
		// first time reading: starts from beginning
		// otherwise starts from where it is
		if (pg_Path_Status[indPath].indReading < 0) {
			pg_Path_Status[indPath].indReading = 0;
		}
		pg_Path_Status[indPath].isFirstFrame = true;
		//printf("-> start_read_path %d\n", indPath);
		//printf("ind reading %d\n", pg_Path_Status[indPath].indReading);
	}
}
void pg_path_replay_trackNo_stop(int indPath) {
	if (indPath >= 1 && indPath <= PG_NB_PATHS
		&& is_path_replay[indPath]) {
		is_path_replay[indPath] = false;
		paths_x[indPath] = PG_OUT_OF_SCREEN_CURSOR;
		paths_y[indPath] = PG_OUT_OF_SCREEN_CURSOR;
#ifdef PG_BEZIER_PATHS
		paths_xL[indPath] = PG_OUT_OF_SCREEN_CURSOR;
		paths_yL[indPath] = PG_OUT_OF_SCREEN_CURSOR;
		paths_xR[indPath] = PG_OUT_OF_SCREEN_CURSOR;
		paths_yR[indPath] = PG_OUT_OF_SCREEN_CURSOR;
#endif
		paths_x_prev[indPath] = PG_OUT_OF_SCREEN_CURSOR;
		paths_y_prev[indPath] = PG_OUT_OF_SCREEN_CURSOR;
		isBegin[indPath] = false;
		isEnd[indPath] = false;
		paths_Color_r[indPath] = 0.0F;
		paths_Color_g[indPath] = 0.0F;
		paths_Color_b[indPath] = 0.0F;
		paths_Color_a[indPath] = 1.0F;
		paths_RadiusX[indPath] = 0.0F;
		paths_RadiusY[indPath] = 0.0F;
		// printf( "-> start_read_path\n"  );
		switch (indPath) {
#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7 || PG_NB_PATHS == 11
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
#if PG_NB_PATHS == 7 || PG_NB_PATHS == 11
		case 4:
			*((int*)ScenarioVarPointers[_path_replay_trackNo_4]) = -1;
			break;
		case 5:
			*((int*)ScenarioVarPointers[_path_replay_trackNo_5]) = -1;
			break;
		case 6:
			*((int*)ScenarioVarPointers[_path_replay_trackNo_6]) = -1;
			break;
		case 7:
			*((int*)ScenarioVarPointers[_path_replay_trackNo_7]) = -1;
			break;
#endif
#if PG_NB_PATHS == 11
		case 8:
			*((int*)ScenarioVarPointers[_path_replay_trackNo_8]) = -1;
			break;
		case 9:
			*((int*)ScenarioVarPointers[_path_replay_trackNo_9]) = -1;
			break;
		case 10:
			*((int*)ScenarioVarPointers[_path_replay_trackNo_10]) = -1;
			break;
		case 11:
			*((int*)ScenarioVarPointers[_path_replay_trackNo_11]) = -1;
			break;
#endif
		}
		sprintf(AuxString, "/path_replay_trackNo_%d 0", indPath);
		pg_send_message_udp((char *)"i", AuxString, (char *)"udp_TouchOSC_send");
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
	pg_draw_scene( _Svg, true );
  }
  else if( strcmp( type , "png" ) == 0 ) {
	pg_draw_scene( _Png, true);
  }
  else if( strcmp( type , "jpg" ) == 0 ) {
	pg_draw_scene( _Jpg, true);
  }
  else {
	sprintf( ErrorStr , "Incorrect screenshot type (%s): expected svg or png or jpg!" , type ); ReportError( ErrorStr );
  }
}

