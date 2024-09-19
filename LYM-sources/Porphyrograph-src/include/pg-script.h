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

//////////////////////////////////////////////////////////////////////////////////////////////////////
// CONSTs
//////////////////////////////////////////////////////////////////////////////////////////////////////

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

// +++++++++++++++++++++++ CA seeding +++++++++++++++++++++++++++++++++
#if defined(pg_Project_CAaudio)
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
};
#endif

///////////////////////////////////////////////////////////////////////////////////
// HASH MAP FOR STRING COMMANDS
enum pg_OSC_addresses_hashMap_IDs
{
	_resend_all = _MaxInterpVarIDs,
	_resend_all_light,
	_setupNo,
	_configurationNo,
	_setupName,
	_setup_plus,
	_interpolation_duration,
	_master_incay,
	_master_decay,
	_setup_plus_5,
	_setup_minus,
	_setup_minus_5,
	_setup_plus_keep_total_dur,
	_snapshot,
	_new_scene,
	_take_snapshot,
	_muteRightScreen_onOff,
	_isClearAllLayers,
	_launch,
	_quit,
		
	_pen_touch,
	_pen_xy,
	_pen2_xy,
	_pen3_xy,
	_pen4_xy,
	_pen5_xy,
	_abs_pen_xy,
	_pen_brush_plus,
	_pen_brush_minus,
	_pen_brush_replay_plus,
	_pen_brush_replay_minus,
	_pen_BW,
	_pressure_onOff,
	_pen_colorPreset_minus,
	_pen_colorPreset_plus,
	_pen_colorPreset,
	_StylusvsRubber,
	_pen_hue_sat_value,

	_NextRecordReplayPath,
	_path_replay_stopAll,
	_path_replay_playAll,
	_clear_path_group,

	_repopBG_BW,
	_repopCA_BW,
	_repopPart_BW,
	_repop_hue_sat_valuePart,

	_flashPixel,
	_flashTrkPart,
	_flashTrkCA,
	_flashCABG,
	_flashCAPart,
	_flashMaster,
	_beat,
	_MIDIVelocity,
	_flashTrkCA_onOff,
	_flashTrkPart_onOff,
	_copyTrack_above,
	_copyTrack_under,
	_MIDINote,

	_CA1Type_plus,
	_CA1Type_minus,
	_CA1SubType_plus,
	_CA1SubType_minus,
	_CAonOff,
	_initCA,
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

	_partStroke_mode_0,
	_partStroke_mode_1,
	_partStroke_mode_2,
	_partColor_mode_0,
	_partColor_mode_1,
	_partColor_mode_2,
	_partExit_mode_0,
	_partExit_mode_1,
	_partExit_mode_2,
	_partExit_mode_plus,
	_partStroke_mode_plus,
	_partColor_mode_plus,
	_pixel_mode_plus,

	_TopLeftKeystone,
	_BottomLeftKeystone,
	_TopRightKeystone,
	_BottomRightKeystone,

	_is_blur_1,
	_is_blur_1_plus,
	_is_blur_1_plus_plus,
	_is_blur_2,

	_reset_sound,
	_pulse_spectrum,
	_soundtrack_plus,
	_soundtrack_minus,
	_soundtrack_seek,
	_soundtrack_onOff,
	_soundtrack_volume,
	_JUCE_loop_track,
	_JUCE_exit,
	_fftLevel8loudestFreqBins,

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
	_ClipArt_x,
	_ClipArt_y,
	_ClipArt_nat_color,
	_ClipArt_white_color,
	_ClipArt_red_color,
	_ClipArt_green_color,
	_ClipArt_blue_color,
	_ClipArt_yellow_color,
	_ClipArt_cyan_color,
	_ClipArt_magenta_color,
	_ClipArt_black_color,
	_ClipArt_translations,

	_currentPhotoTrack_plus,
	_flashPhoto,
	_diaporama_add_dirs,
	_reload_all_diaporamas,
	_diaporama_random,
	_diaporama_slide,
	_diaporama_plus,
	_diaporama_minus,
	_flash_photo_diaporama,

	_clip_plus,
	_clip_minus,
	_clip_forward,
	_clip_backward,
	_clip_scratch,
	_clip_scratch_touch_press,
	_clip_scratch_touch_release,
	_clip_nudge,
	_clip_nudge_touch_press,
	_clip_nudge_touch_release,
	_clip_cue_onOff,
	_clip_cue_jump,
	_clip2_cue_jump,
	_clip_cue_call,
	_clip_fx,
	_clip_fx_std,
	_clip_equalizer,
	_clip2_equalizer,
	_clip_new,
	_clip2_new,
	_clip_sample,
	_clip_sample_range,
	_clip2_sample,
	_clip_fader,
	_clip2_fader,
	_clip_autoplay_left,
	_clip_autoplay_right,
	_clip_play_left,
	_clip_play_right,
	_clip2_autoplay_left,
	_clip2_autoplay_right,
	_clip2_play_left,
	_clip2_play_right,

	_currentVideoTrack_plus,
	_movie_plus,
	_movie_minus,
	_movie_forward,
	_movie_backward,
	_movie_loop_onOff,
		
	_camera_close,
	_camera_open,
	_initialBGCapture,
	_flashCamera,
	_cameraWB_R_plus,
	_cameraWB_R_minus,
	_cameraWB_B_plus,
	_cameraWB_B_minus,
	_cameraExposure_plus,
	_cameraExposure_minus,
	_cameraGain_plus,
	_cameraGain_minus,
	_cameraBrightness_plus,
	_cameraBrightness_minus,
	_cameraSaturation_plus,
	_cameraSaturation_minus,
	_cameraContrast_plus,
	_cameraContrast_minus,
	_cameraGamma_plus,
	_cameraGamma_minus,
	_reset_on_camera,
	_reset_camera_params,
	_cameraCumul_plus,

	_Mesh_onOff,
	_Mesh_mobile_onOff,
	_Caverne_Mesh_Profusion_on,
	_Caverne_Mesh_Profusion_off,
	_Caverne_Mesh_7Solids_on,
	_Caverne_Mesh_7Solids_off,
	_BGcolor_onOff,
	_Mesh_light_x,
	_Mesh_light_y,
	_Mesh_light_z,

	_sensor_layout_plus,
	_sensor_sample_setUp_plus,
	_sensor_activation_plus,
	_mw_mss_pos,

	_testUDP,
	_return_message,
	_QT_connected,
	_PD_connected,
	_connect_PD,
	_processing_video,
	_processing_image,
	_processing_master,
};


enum pg_Keystroke_Input_Type { pg_enum_KEYSTROKE_MINUS = 0, pg_enum_KEYSTROKE_PLUS, pg_enum_KEYSTROKE_VOID };

#define CA_SQUENCR                0
#define CA_TOTALISTIC             1
#define CA_GENERATION             2
#define CA_GAL_BIN_MOORE          3
#define CA_GAL_BIN_NEUMANN        4
#define CA_NEUMANN_BINARY         5

#if defined(pg_Project_CAaudio)
#define CA_TOTALISTIC             0
#define CA_GENERATION             1
#define CA_GAL_BIN_MOORE          2
#define CA_GAL_BIN_NEUMANN        3
#define CA_NEUMANN_BINARY         4
#endif

#define PG_NB_PARTEXIT_MODES 3
#define PG_NB_PARTSTROKE_MODES 4
#define PG_NB_PARTCOLOR_MODES 3

//////////////////////////////////////////////////////////////////////////////////////////////////////
// SCENARIO VARIABLE VALUE MANAGEMENT CLASS
//////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////
// SCENARIO AND CONFIGURATION VARIABLES
// initial values in the scenario (before first scene)
class ScenarioValue {
public:
	double val_num;
	string val_string;
	double* val_array;
	int array_size;
	ScenarioValue() {
		val_num = 0;
		val_string = "";
		val_array = NULL;
		array_size = 0;
	}
	ScenarioValue(double d, string s, double* v_array, int array_size) {
		init_ScenarioValue(d, s, v_array, array_size);
	}
	void init_ScenarioValue(double d, string s, double* v_array, int a_size) {
		val_num = d;
		val_string = s;
		if (a_size > 0) {
			val_array = new double[a_size];
			array_size = a_size;
		}
		if (v_array != NULL) {
			for (int i = 0; i < array_size; i++) {
				val_array[i] = v_array[i];
			}
		}
	}
	void const_value(double d) {
		if (val_array != NULL && array_size > 0) {
			for (int i = 0; i < array_size; i++) {
				val_array[i] = d;
			}
		}
		else {
			val_num = d;
		}
	}
	double get_array_value(int ind) {
		if (val_array != NULL && ind >= 0 && ind < array_size) {
			return val_array[ind];
		}
		printf("get_array_value: Expected array of min size %d, not %d\n", ind + 1, array_size);
		return 0.f;
	}
	void set_array_value(int ind, double val) {
		if (val_array != NULL && ind >= 0 && ind < array_size) {
			val_array[ind] = val;
			return;
		}
		printf("set_array_value: Expected array of min size %d, not %d\n", ind + 1, array_size);
	}
	~ScenarioValue() {
	}
};


//////////////////////////////////////////////////////////////////////////////////////////////////////
// EXPORTED VARIABLES
////////////////////////////////////////////////////////////////////////////////////////////////////// 

#if defined(var_GenerativeNights_planes)
//////////////////////////////////////////////
/// image number of first plane
extern int      firstPlaneFrameNo;

//////////////////////////////////////////////
// CA initialization
extern float initCA;
#endif

#if defined(pg_Project_CAaudio)
extern pg_CAseed_types pg_CAseed_type;
extern pg_CAseed_locations pg_CAseed_location;
extern int pg_CAseed_coordinates[2];
extern int pg_CAseed_size;
extern bool pg_CAseed_trigger;
void pg_CAseed_location_to_coordinates(pg_CAseed_locations location, int coordinates[2]);
#endif

// +++++++++++++++++++++++ FFT levels and frequency storage ++++++++++++++++++++
extern float fftLevels[8];
extern float fftFrequencies[8];
extern float fftPhases[8];

// +++++++++++++++++++++ BLUR +++++++++++++++++++++++++++
#if defined(PG_WITH_BLUR)
extern bool is_blur_1;
extern bool is_blur_2;
extern int nb_blur_frames_1;
extern int nb_blur_frames_2;
#endif

// ++++++++++++++++++++++ SCENARIO +++++++++++++++++++++++++ 
class Scene;
extern Scene* pg_CurrentScene;
extern int pg_CurrentSceneIndex;

// changed to true when a value is changed manually during scenario
extern bool pg_BrokenInterpolationVar[_MaxInterpVarIDs];

// +++++++++++++++++++++++ Beats +++++++++++++++++++++++++++++++++
extern double pg_lastBeatTime;

// pen preset
extern int pg_current_pen_colorPreset;

// +++++++++++++++++++++ COPY LAYERS +++++++++++++++++++++++++++
// copy to layer above (+1) or to layer below (-1)
extern int pg_copyToNextTrack;

// master automatic incay/decay
extern double pg_master_incay_duration;
extern double pg_master_decay_duration;
extern double pg_master_incay_start_time;
extern double pg_master_decay_start_time;
extern float pg_master_incay_start_value;
extern float pg_master_decay_start_value;

// beat no
extern int pg_BeatNo;

// MIDI event creates a local stroke
extern int pg_FourFrameStrokeNb;
extern int pg_FourFrameStroke_x;
extern int pg_FourFrameStroke_y;

// hash table for message strings
extern std::unordered_map<std::string, int> pg_OSC_addresses_hashMap;


//////////////////////////////////////////////////////////////////////////////////////////////////////
// EXPORTED FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////

// keystroke command processing
void pg_process_key(int key);
void pg_process_special_key(int key);

// generic alias script
void pg_aliasScript(string address_string, string string_argument_0,
	float float_arguments[PG_MAX_OSC_ARGUMENTS], int nb_arguments);

#endif
