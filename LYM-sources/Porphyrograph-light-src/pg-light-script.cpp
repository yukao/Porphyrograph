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

#include "pg-light-all_include.h"


 /////////////////////////////////////////////////////////////////////////
 // WORK VARIABLES
 /////////////////////////////////////////////////////////////////////////

// echo modulator for clips
float pg_fx_dry_wet = 0.f;

//////////////////////////////////////////////
// SCENARIO AND CONFIGURATION VARIABLES
// changed to true when a value is changed manually during scenario
bool pg_BrokenInterpolationVar[_MaxInterpVarIDs];

//////////////////////////////////////////////
// beat no
int pg_BeatNo = 0;

//////////////////////////////////////////////
// beat no
int pg_MIDINote = 0;

//////////////////////////////////////////////
// MIDI event creates a local stroke
int pg_FourFrameStrokeNb = 5;
int pg_FourFrameStroke_x = PG_OUT_OF_SCREEN_CURSOR;
int pg_FourFrameStroke_y = PG_OUT_OF_SCREEN_CURSOR;

#if defined(var_GenerativeNights_planes)
//////////////////////////////////////////////
/// image number of first plane
int      firstPlaneFrameNo = -1;

//////////////////////////////////////////////
// CA initialization
float initCA = 0.0f;
#endif

// ++++++++++++++++++++++ GLUT MODIFIER ++++++++++++++++++++ 
int pg_key_modifier = 0;

// ++++++++++++++++++++++ MUSIC CONTROL ++++++++++++++++++++ 
// music control inputs received at each frame

// pg_audio_pulse preceding value
float pg_audio_pulse_prec[3] = { -1.f };

// factor increasing the acceleration weight of pixels, the brush radius, color...
// based on sound volume attacks
float pulse_average;
float pg_audio_pulse_average_prec = -1.f;
float pg_audio_pulse[3] = { 0.0f };
float pg_seed_pulsePerlinNoise[4 * 2]
= { rand_0_1 * 255, rand_0_1 * 255, rand_0_1 * 255, rand_0_1 * 255,
	rand_0_1 * 255, rand_0_1 * 255, rand_0_1 * 255, rand_0_1 * 255 };
// not used currently float pulse_attack;

// pen preset
int pg_current_pen_colorPreset = -1;

// +++++++++++++++++++++ VIDEO BACKGROUND SUBTRACTION +++++
bool pg_is_firstVideoOn = true;

// +++++++++++++++++++++ DELAYED CAMERA WEIGHT ++++++++++++
int pg_delayedCameraWeight = 0;

#if defined(PG_WITH_BLUR)
// +++++++++++++++++++++ BLUR +++++++++++++++++++++++++++
bool is_blur_1 = false;
bool is_blur_2 = false;
int nb_blur_frames_1 = 0;
int nb_blur_frames_2 = 0;
#endif

// +++++++++++++++++++++ COPY LAYERS +++++++++++++++++++++++++++
// copy to layer above (+1) or to layer below (-1)
int pg_copyToNextTrack = 0;

// ++++++++++++++++++++++ SET-UP +++++++++++++++++++++++++++ 
int pg_CurrentSceneIndex = -1;
Scene* pg_CurrentScene = NULL;

// master automatic incay/decay
double pg_master_incay_duration = 0.;
double pg_master_incay_start_time = 0.;
float pg_master_incay_start_value = 0.f;
double pg_master_decay_duration = 0.;
double pg_master_decay_start_time = 0.;
float pg_master_decay_start_value = 1.f;

double pg_lastPenTouchTime = 0.;

// +++++++++++++++++++++++ Beats +++++++++++++++++++++++++++++++++
double pg_lastBeatTime = 0.;

// +++++++++++++++++++++++ CA SubType memory for CA on/off command ++++++++++
int pg_CASubTypeMem = 1;

#if defined(pg_Project_CAaudio)
pg_CAseed_types pg_CAseed_type = _pg_CAseed_dot_center;
pg_CAseed_locations pg_CAseed_location = _pg_CAseed_loc_center;
int pg_CAseed_coordinates[2] = { -1, -1 };
int pg_CAseed_size = 1;
bool pg_CAseed_trigger = false;
#endif

// +++++++++++++++++++++++ FFT levels and frequency storage ++++++++++++++++++++
#if defined(pg_Project_Criton)
float fftLevels[8];
float fftFrequencies[8];
float fftPhases[8];
#endif

// +++++++++++++++++++++++ Clip ranges for automatic clip selection for ContAct +++++
#if defined(var_Contact_clip_in_range)
std::vector<int> ContAct_clip_ranges_min;
std::vector<int> ContAct_clip_ranges_max;
#endif

///////////////////////////////////////////////////////////////////////////////////
// HASH MAP FOR STRING COMMANDS
enum pg_OSC_addresses_hashMap_IDs
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
	_flashPhoto,
	_pulse_spectrum,
	_beat,
	_MIDINote,
	_MIDIVelocity,
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
	_pen_touch,
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
	_is_blur_1,
	_is_blur_1_plus,
	_is_blur_1_plus_plus,
	_is_blur_2,
	_cameraCumul_plus,
	_pen_brush_plus,
	_pen_brush_minus,
	_pen_brush_replay_plus,
	_pen_brush_replay_minus,
	_pixel_mode_plus,
	_CA1Type_plus,
	_CA1SubType_plus,
	_CA1Type_minus,
	_CA1SubType_minus,
	_CAonOff,
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
	_diaporama_add_dirs,
	_reload_all_diaporamas,
	_diaporama_random,
	_diaporama_slide,
	_diaporama_plus,
	_diaporama_minus,
	_flash_photo_diaporama,
	_soundtrack_plus,
	_soundtrack_minus,
	_soundtrack_onOff,
	_soundtrack_volume,
	_soundtrack_seek,
	_pen_colorPreset_minus,
	_pen_colorPreset_plus,
	_pen_colorPreset,
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
	_movie_loop_onOff,
	_movie_plus,
	_movie_minus,
	_movie_forward,
	_movie_backward,
	_camera_open,
	_camera_close,
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
	_clip_sample,
	_clip_sample_range,
	_clip2_sample,
	_clip_sample_shift,
	_clip_new,
	_clip2_new,
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
	_resend_all,
	_resend_all_light,
	_configurationNo,
	_setupNo,
	_setupName,
	_setup_plus,
	_setup_plus_5,
	_setup_minus,
	_setup_minus_5,
	_setup_plus_keep_total_dur,
	_interpolation_duration,
	_master_decay,
	_master_incay,
	_JUCE_loop_track,
	_JUCE_exit,
	_fftLevel8,
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
	_Mesh_onOff,
	_Caverne_Mesh_Profusion_on,
	_Caverne_Mesh_Profusion_off,
	_Caverne_Mesh_7Solids_on,
	_Caverne_Mesh_7Solids_off,
	_BGcolor_onOff,
	_Mesh_mobile_onOff,
	_Mesh_light_x,
	_Mesh_light_y,
	_Mesh_light_z,
	_muteRightScreen_onOff,
	_isClearAllLayers,
	_StylusvsRubber,
	_pen_hue_sat_value,
	_repop_hue_sat_valuePart,
	_take_snapshot,
	_path_replay_stopAll,
	_path_replay_playAll,
	_clear_path_group,
	_mw_mss_pos,
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
	_reset_camera_params,
	_processing_video,
	_processing_image,
	_processing_master,
	_BottomLeftKeystone,
	_BottomRightKeystone,
	_TopLeftKeystone,
	_TopRightKeystone,
	_ZeroLight,
	_flashPixel,
	_flashTrkPart,
	_flashTrkCA,
	_flashCABG,
	_flashCAPart,
	_flashMaster,
	_NextRecordReplayPath,
};
// Create an unordered_map of three strings (that map to strings)
std::unordered_map<std::string, int> pg_OSC_addresses_hashMap = {
	{ "testUDP", _testUDP },
	{ "QT_connected", _QT_connected },
	{ "PD_connected", _PD_connected },
	{ "return_message", _return_message },
	{ "launch", _launch },
	{ "reset_sound", _reset_sound },
	{ "connect_PD", _connect_PD },
	{ "quit", _quit },
	{ "flashCamera", _flashCamera },
	{ "flashPhoto", _flashPhoto },
	{ "pulse_spectrum", _pulse_spectrum },
	{ "beat", _beat },
	{ "MIDINote", _MIDINote },
	{ "Velocity1", _MIDIVelocity },
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
	{ "pen_touch", _pen_touch},
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
	{ "is_blur_1", _is_blur_1 },
	{ "is_blur_1_plus", _is_blur_1_plus },
	{ "is_blur_1_plus_plus", _is_blur_1_plus_plus },
	{ "is_blur_2", _is_blur_2 },
	{ "cameraCumul_plus", _cameraCumul_plus },
	{ "pen_brush_plus", _pen_brush_plus },
	{ "pen_brush_minus", _pen_brush_minus },
	{ "pen_brush_replay_minus", _pen_brush_replay_minus },
	{ "pen_brush_replay_plus", _pen_brush_replay_plus },
	{ "pixel_mode_plus", _pixel_mode_plus },
	{ "CA1Type_plus", _CA1Type_plus },
	{ "CA1SubType_plus", _CA1SubType_plus },
	{ "CA1Type_minus", _CA1Type_minus },
	{ "CA1SubType_minus", _CA1SubType_minus },
	{ "CAonOff", _CAonOff },
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
	{ "diaporama_add_dirs", _diaporama_add_dirs },
	{ "reload_all_diaporamas", _reload_all_diaporamas },
	{ "diaporama_random", _diaporama_random },
	{ "diaporama_slide", _diaporama_slide },
	{ "diaporama_plus", _diaporama_plus },
	{ "diaporama_minus", _diaporama_minus },
	{ "flash_photo_diaporama", _flash_photo_diaporama },
	{ "soundtrack_plus", _soundtrack_plus },
	{ "soundtrack_minus", _soundtrack_minus },
	{ "soundtrack_onOff", _soundtrack_onOff },
	{ "soundtrack_volume", _soundtrack_volume },
	{ "soundtrack_seek", _soundtrack_seek },
	{ "pen_colorPreset_minus", _pen_colorPreset_minus },
	{ "pen_colorPreset_plus", _pen_colorPreset_plus },
	{ "pen_colorPreset", _pen_colorPreset },
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
	{ "movie_loop_onOff", _movie_loop_onOff },
	{ "movie_plus", _movie_plus },
	{ "movie_minus", _movie_minus },
	{ "movie_forward", _movie_forward },
	{ "movie_backward", _movie_backward },
	{ "camera_open", _camera_open },
	{ "camera_close", _camera_close },
	{ "clip_plus", _clip_plus },
	{ "clip_minus", _clip_minus },
	{ "clip_forward", _clip_forward },
	{ "clip_backward", _clip_backward },
	{ "clip_scratch", _clip_scratch },
	{ "clip_scratch_touch_press", _clip_scratch_touch_press },
	{ "clip_scratch_touch_release", _clip_scratch_touch_release },
	{ "clip_nudge", _clip_nudge },
	{ "clip_nudge_touch_press", _clip_nudge_touch_press },
	{ "clip_nudge_touch_release", _clip_nudge_touch_release },
	{ "clip_cue_onOff", _clip_cue_onOff },
	{ "clip_cue_jump", _clip_cue_jump },
	{ "clip2_cue_jump", _clip2_cue_jump },
	{ "clip_cue_call", _clip_cue_call },
	{ "clip_fx", _clip_fx },
	{ "clip_fx_std", _clip_fx_std },
	{ "clip_equalizer", _clip_equalizer },
	{ "clip2_equalizer", _clip2_equalizer },
	{ "clip_sample", _clip_sample },
	{ "clip_sample_range", _clip_sample_range },
	{ "clip2_sample", _clip2_sample },
	{ "clip_sample_shift", _clip_sample_shift },
	{ "clip_new", _clip_new },
	{ "clip2_new", _clip2_new },
	{ "clip_fader", _clip_fader },
	{ "clip2_fader", _clip2_fader },
	{ "clip_autoplay_left", _clip_autoplay_left },
	{ "clip_autoplay_right", _clip_autoplay_right },
	{ "clip_play_left", _clip_play_left },
	{ "clip_play_right", _clip_play_right },
	{ "clip2_autoplay_left", _clip_autoplay_left },
	{ "clip2_autoplay_right", _clip_autoplay_right },
	{ "clip2_play_left", _clip_play_left },
	{ "clip2_play_right", _clip_play_right },
	{ "resend_all", _resend_all },
	{ "resend_all_light", _resend_all_light },
	{ "configurationNo", _configurationNo },
	{ "setupNo", _setupNo },
	{ "setupName", _setupName },
	{ "setup_plus", _setup_plus },
	{ "setup_plus_5", _setup_plus_5 },
	{ "setup_minus", _setup_minus },
	{ "setup_minus_5", _setup_minus_5 },
	{ "setup_plus_keep_total_dur", _setup_plus_keep_total_dur },
	{ "interpolation_duration", _interpolation_duration },
	{ "master_decay", _master_decay },
	{ "master_incay", _master_incay },
	{ "JUCE_loop_track", _JUCE_loop_track },
	{ "JUCE_exit", _JUCE_exit },
	{ "fftLevel8", _fftLevel8 },
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
	{ "ClipArt_x", _ClipArt_x },
	{ "ClipArt_y", _ClipArt_y },
	{ "ClipArt_nat_color", _ClipArt_nat_color },
	{ "ClipArt_white_color", _ClipArt_white_color },
	{ "ClipArt_red_color", _ClipArt_red_color },
	{ "ClipArt_green_color", _ClipArt_green_color },
	{ "ClipArt_blue_color", _ClipArt_blue_color },
	{ "ClipArt_yellow_color", _ClipArt_yellow_color },
	{ "ClipArt_cyan_color", _ClipArt_cyan_color },
	{ "ClipArt_magenta_color", _ClipArt_magenta_color },
	{ "ClipArt_black_color", _ClipArt_black_color },
	{ "ClipArt_translations", _ClipArt_translations },
	{ "Mesh_onOff", _Mesh_onOff },
	{ "Caverne_Mesh_Profusion_on", _Caverne_Mesh_Profusion_on },
	{ "Caverne_Mesh_Profusion_off", _Caverne_Mesh_Profusion_off },
	{ "Caverne_Mesh_7Solids_on", _Caverne_Mesh_7Solids_on },
	{ "Caverne_Mesh_7Solids_off", _Caverne_Mesh_7Solids_off },
	{ "BGcolor_onOff", _BGcolor_onOff },
	{ "Mesh_mobile_onOff", _Mesh_mobile_onOff },
	{ "Mesh_light_x", _Mesh_light_x },
	{ "Mesh_light_y", _Mesh_light_y },
	{ "Mesh_light_z", _Mesh_light_z },
	{ "muteRightScreen_onOff", _muteRightScreen_onOff },
	{ "pg_isClearAllLayers", _isClearAllLayers },
	{ "StylusvsRubber", _StylusvsRubber },
	{ "pen_hue_sat_value", _pen_hue_sat_value },
	{ "repop_hue_sat_valuePart", _repop_hue_sat_valuePart },
	{ "take_snapshot", _take_snapshot },
	{ "path_replay_stopAll", _path_replay_stopAll },
	{ "path_replay_playAll", _path_replay_playAll },
	{ "pg_clear_path_group", _clear_path_group },
	{ "mw_mss_pos", _mw_mss_pos },
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
	{ "reset_camera_params", _reset_camera_params },
	{ "processing_video", _processing_video },
	{ "processing_image", _processing_image },
	{ "processing_master", _processing_master },
	{ "TopLeftKeystone", _TopLeftKeystone },
	{ "TopRightKeystone", _TopRightKeystone },
	{ "BottomLeftKeystone", _BottomLeftKeystone },
	{ "BottomRightKeystone", _BottomRightKeystone },
	{ "ZeroLight", _ZeroLight },
	{ "pg_flashPixel", _flashPixel },
	{ "flashTrkPart", _flashTrkPart },
	{ "flashTrkCA", _flashTrkCA },
	{ "flashCABG", _flashCABG },
	{ "flashCAPart", _flashCAPart },
	{ "pg_flashMaster", _flashMaster },
	{ "NextRecordReplayPath", _NextRecordReplayPath },
};

#if defined(pg_Project_CAaudio)
void pg_CAseed_location_to_coordinates(pg_CAseed_locations location, int coordinates[2]) {
	switch (location) {
	case 	_pg_CAseed_loc_center: {
		coordinates[0] = pg_workingWindow_width / 2;
		coordinates[1] = PG_WINDOW_HEIGHT / 2;
		break;
	}
	case 	_pg_CAseed_loc_N: {
		coordinates[0] = pg_workingWindow_width / 2;
		coordinates[1] = 0;
		break;
	}
	case 	_pg_CAseed_loc_E: {
		coordinates[0] = pg_workingWindow_width;
		coordinates[1] = PG_WINDOW_HEIGHT / 2;
		break;
	}
	case 	_pg_CAseed_loc_S: {
		coordinates[0] = pg_workingWindow_width / 2;
		coordinates[1] = PG_WINDOW_HEIGHT;
		break;
	}
	case 	_pg_CAseed_loc_W: {
		coordinates[0] = 0;
		coordinates[1] = PG_WINDOW_HEIGHT / 2;
		break;
	}
	case 	_pg_CAseed_loc_NW: {
		coordinates[0] = 0;
		coordinates[1] = 0;
		break;
	}
	case 	_pg_CAseed_loc_NE: {
		coordinates[0] = pg_workingWindow_width;
		coordinates[1] = 0;
		break;
	}
	case 	_pg_CAseed_loc_SE: {
		coordinates[0] = pg_workingWindow_width;
		coordinates[1] = PG_WINDOW_HEIGHT;
		break;
	}
	case 	_pg_CAseed_loc_SW: {
		coordinates[0] = 0;
		coordinates[1] = PG_WINDOW_HEIGHT;
		break;
	}
	}
}
#endif




///////////////////////////////////////////////////////////////////////////////////
// KEYSTROKE BASED COMMANDS
///////////////////////////////////////////////////////////////////////////////////

void pg_process_key(int key) {
	pg_key_modifier = glutGetModifiers();

	// printf( "key (%d) mod %d\n" , key , mod );

	if (!(pg_key_modifier & GLUT_ACTIVE_ALT)) {
		// non special key that corresponds to a script
		pg_keyStrokeScripts(key);
		return;
	}

	switch (key) {

		/* ------------------------------- beat */
	case 'b':
		auto_beat = !auto_beat;
		pg_lastBeatTime = pg_CurrentClockTime;
		break;

		/* ------------------------------- frame per second */
	case 'f':
		pg_DisplayFramePerSecond = !pg_DisplayFramePerSecond;
		printf("FPS display %d\n", pg_DisplayFramePerSecond);
		break;

		/* ------------------------------- check connection to PD */
	case 'p':
		pg_send_message_udp((char*)"", (char*)"/PD_connected", (char*)"udp_PD_send");
		break;

		/* ------------------------------- snapshot */
	case 's':
		pg_draw_scene(pg_enum_render_Jpg);
		break;
	case 'S':
		pg_draw_scene(pg_enum_render_Svg);
		break;

		/* ------------------------------- current video background capture */
	case 'v':
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_cameraCaptFreq]) {
			reset_camera = true;
		}
		break;

	default:
		printf("Alt key %d is not active.\n", key);
		break;
	}
}

/*!
 * \brief special keys events (interactive mode)
 * \param key			the key pressed
 */
void pg_process_special_key(int key) {
	// switch (key) {    
	// default:
	//   printf ("special key %d is not active.\n", key);
	//   break;
	// }   
}

void pg_keyStrokeScripts(int key) {
	switch (key) {

		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
		// +++++++++++++++++ PEDALS ++++++++++++++++++++++++++++++++ 
		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
		// ====================================== 
	case 'a':
		pg_scene_plus(1);
		break;
		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
		// +++++++++++++++++ SET-UP ++++++++++++++++++++++++++++++++ 
		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
		// ====================================== 
		// advances to next scene without respect for the global timing
	case'S':
		pg_scene_plus(1);
		break;

		// advances to next scene and keeps the global timing
	case's': {
		pg_restoreInitialTimesAndDurations();
		double deltaTime = pg_Scenario[pg_ind_scenario][1 + pg_CurrentSceneIndex].scene_initial_time - (pg_CurrentClockTime - pg_InitialScenarioTime);
		// if the scene has not yet begun
		if (deltaTime > 0) {
			int new_scene = ((1 + pg_CurrentSceneIndex) % pg_Scenario[pg_ind_scenario].size());
			// we place the beginning of the current scene at this time
			pg_Scenario[pg_ind_scenario][new_scene].scene_initial_time -= deltaTime;
			if (new_scene > 0) {
				pg_Scenario[pg_ind_scenario][new_scene - 1].scene_final_time -= deltaTime;
				pg_Scenario[pg_ind_scenario][new_scene - 1].scene_duration -= deltaTime;
			}
			pg_Scenario[pg_ind_scenario][new_scene].scene_duration += deltaTime;
			// unuseful because is already made through pg_Scenario[pg_ind_scenario][new_scene].scene_initial_time -= deltaTime;
			//pg_InitialScenarioTime = pg_CurrentClockTime - pg_Scenario[pg_ind_scenario][new_scene].scene_initial_time;
			pg_StartNewScene(new_scene, 0);
		}
	}
		   break;

		   // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
		   // +++++++++++++++++ TEST UDP ++++++++++++++++++++++++++++++ 
		   // +++++++++++++++++   keystroke (T)  ++++++++++++++++++++++ 
		   // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	case'T':
		// message to supercollider
		pg_send_message_udp((char*)"f", (char*)"/testUDP 0", (char*)"udp_SC");
		// message to touch OSC
		sprintf(pg_AuxString, "/setup_1 TEST_UDP"); pg_send_message_udp((char*)"s", pg_AuxString, (char*)"udp_TouchOSC_send");
		//sprintf(pg_AuxString, "/return_message returnUPD_%f", pg_CurrentClockTime); pg_send_message_udp((char *)"s", pg_AuxString, (char *)"udp_TouchOSC_send");
		break;

		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
		// +++++++++++++++++ LAUNCH PERFORMANCE ++++++++++++++++++++ 
		// +++++++++++++++++   keystroke (t)  ++++++++++++++++++++++ 
		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	case't':
		pg_launch_performance(0);
		break;
		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
		// +++++++++++++++++ LAUNCH SCENE ++++++++++++++++++++++++++ 
		// +++++++++++++++++   keystroke (1--9 and 0)  +++++++++++++ 
		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9': {
		int ind_scene = int(key - '1') % pg_Scenario[pg_ind_scenario].size();
		if (pg_CurrentSceneIndex < 0) {
			pg_launch_performance(ind_scene);
		}
		else {
			pg_StartNewScene(ind_scene, 0);
		}
	}
			break;
	}
}


///////////////////////////////////////////////////////////////////////////////////
// UDP BASED COMMANDS
///////////////////////////////////////////////////////////////////////////////////
void pg_aliasScript(string address_string, string string_argument_0,
	float float_arguments[PG_MAX_OSC_ARGUMENTS], int nb_arguments) {
	float control_color[3] = { 0 };

	// first looks whether the message or its prefix (message minus _?[0-9]+) are either a variable or 
	std::regex array_command("([a-zA-Z0-9_]+[a-zA-Z])_?([0-9]+)(_pulse)?");
	std::smatch pieces_match;
	int indVar = -1;
	int indexChangedValue = -1;
	if (std::regex_match(address_string, pieces_match, array_command)) {
		if (pg_OSC_addresses_hashMap.find(pieces_match[1]) != pg_OSC_addresses_hashMap.end()) {
			if (pieces_match.size() == 3) {
				int provided_index = stoi(pieces_match[2].str());
				if (pg_OSC_addresses_hashMap.find(pieces_match[1].str()) != pg_OSC_addresses_hashMap.end()) {
					indVar = pg_OSC_addresses_hashMap.at(pieces_match[1].str());
					if (pg_FullScenarioVarIndiceRanges[indVar][0] != -1
						&& provided_index >= pg_FullScenarioVarIndiceRanges[indVar][0]
						&& provided_index < pg_FullScenarioVarIndiceRanges[indVar][1]) {
						indexChangedValue = provided_index;
					}
				}
			}
			else if (pieces_match.size() == 4) {
				int provided_index = stoi(pieces_match[2].str());
				if (pg_OSC_addresses_hashMap.find(pieces_match[1].str() + pieces_match[3].str()) != pg_OSC_addresses_hashMap.end()) {
					indVar = pg_OSC_addresses_hashMap.at(pieces_match[1].str() + pieces_match[3].str());
					if (pg_FullScenarioVarIndiceRanges[indVar][0] != -1 && provided_index >= pg_FullScenarioVarIndiceRanges[indVar][0]
						&& provided_index < pg_FullScenarioVarIndiceRanges[indVar][1]) {
						indexChangedValue = provided_index;
					}
				}
			}
			else {
				sprintf(pg_errorStr, "Unexpected format for OSC command %s for multidimensional var!", address_string.c_str()); pg_ReportError(pg_errorStr);
			}
		}
	}
	else if (pg_OSC_addresses_hashMap.find(address_string) != pg_OSC_addresses_hashMap.end()) {
		indVar = pg_OSC_addresses_hashMap[address_string];
	}


	// unregistered command (neither scenario variable nor program defined command)
	if (indVar == -1) {
		// touch OSC sends OSC commands for each tab change, we have decided to suffix their address with _DISPLAY
		string touchOSC_tab_command_ending("_DISPLAY");
		unsigned int touchOSC_tab_command_ending_length = touchOSC_tab_command_ending.length();
		// touch OSC sends OSC commands for MSS and sensor that is forwarded to pg_sensors.py, we have decided to prefix their address with mss_
		string MSS_sensor_command_beginning("mss_");
		string XMM_sensor_command_beginning("xmm_");
		// light OSC address begins with light
		string DMX_command_beginning("light");
		unsigned int DMX_command_beginning_length = DMX_command_beginning.length();
		// TouchOSC tab change
		if (address_string.length() >= touchOSC_tab_command_ending_length
			&& (0 == address_string.compare(address_string.length() - touchOSC_tab_command_ending_length,
				touchOSC_tab_command_ending_length, touchOSC_tab_command_ending))) {
			// OKAY this is a tab change in touchOSC
		}
		// OSC message for lighting
		else if (address_string.length() > DMX_command_beginning_length
			&& (0 == address_string.compare(0, DMX_command_beginning_length, DMX_command_beginning))
			&& pg_nb_light_groups[pg_ind_scenario] > 0) {
			pg_osc_light_command(address_string, float_arguments, nb_arguments);
		}
		// OSC message for sensors
		else if ((address_string.length() >= MSS_sensor_command_beginning.length()
			&& (0 == address_string.compare(0, MSS_sensor_command_beginning.length(), MSS_sensor_command_beginning)))
			|| (address_string.length() >= XMM_sensor_command_beginning.length()
				&& (0 == address_string.compare(0, XMM_sensor_command_beginning.length(), XMM_sensor_command_beginning)))) {
			// OKAY this is a MSS and sensor command that is forwarded to pg_sensors.py
			// printf("MSS and sensor command %s\n", address);
			if (nb_arguments == 1) {
				sprintf(pg_AuxString, "/%s %.5f", address_string.c_str(), float_arguments[0]); pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_MSS_sensor_send");
			}
			else if (nb_arguments == 2) {
				sprintf(pg_AuxString, "/%s %.5f %.5f", address_string.c_str(), float_arguments[0], float_arguments[1]); pg_send_message_udp((char*)"ff", pg_AuxString, (char*)"udp_MSS_sensor_send");
			}
			else {
				sprintf(pg_AuxString, "/%s", address_string.c_str()); pg_send_message_udp((char*)"", pg_AuxString, (char*)"udp_MSS_sensor_send");
			}
		}
		else {
			sprintf(pg_errorStr, "Unknown command (%s)!", address_string.c_str()); pg_ReportError(pg_errorStr);
		}
		return;
	}

	// command from the scenario file
	// change of array variable
	if (indVar < _MaxInterpVarIDs && indexChangedValue >= 0) {
		double* valArray = new double[pg_FullScenarioVarIndiceRanges[indVar][1]];
		// do stuff
		for (int index = pg_FullScenarioVarIndiceRanges[indVar][0]; index < pg_FullScenarioVarIndiceRanges[indVar][1]; index++) {
			valArray[index] = 0.;
		}
		valArray[indexChangedValue] = float_arguments[0];
		//printf("index %d value %.2f\n", indexChangedValue, valArray[indexChangedValue]);
		ScenarioValue interpolated_scene_value(0.f, "", valArray, pg_FullScenarioVarIndiceRanges[indVar][1]);
		pg_update_variable(pg_enum_PG_GUI_COMMAND, indVar, interpolated_scene_value, indexChangedValue);
		delete(valArray);
		return;
	}
	// change of scalar variable
	else if (indVar < _MaxInterpVarIDs) {
		if (pg_FullScenarioActiveVars[pg_ind_scenario][indVar]) {
			ScenarioValue arg_val(float_arguments[0], "", NULL, 0);
			pg_update_variable(pg_enum_PG_GUI_COMMAND, indVar, arg_val, -1);
		}
		else {
			sprintf(pg_errorStr, "OSC command %s for inactive var %d (%s)!", address_string.c_str(), indVar, pg_FullScenarioVarMessages[indVar].c_str()); pg_ReportError(pg_errorStr);
		}
		return;
	}

	// special command not in the scenario file
	switch (indVar) {

		// ====================================== 
		// test UDP connection                    
		// ====================================== 
	case _testUDP: {
		// from TouchOSC, Lola's sound program or Usine
		sprintf(pg_AuxString, "/setup UDP_test_received"); pg_send_message_udp((char*)"s", pg_AuxString, (char*)"udp_TouchOSC_send");
		break;
	}
	case _QT_connected: {
		break;
	}

					  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
					  // +++++++++++++++++ PD CONNECTION CONFIRMATION ++++++++++++ 
					  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	case _PD_connected: {
		// from Yukao's PD
		pg_send_message_udp((char*)"s", (char*)"/PD_connected 1", (char*)"udp_TouchOSC_send");
		break;
	}
	case _return_message: {
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

				// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
				// +++++++++++++++++ SOUND RESET ++++++++++++++++++++++++ 
				// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	case _reset_sound: {
		pg_send_message_udp((char*)"i", (char*)"/reset_sound 1", (char*)"udp_PD_send");
		break;
	}

					 // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
					 // +++++++++++++++++ CONNECTS PD ++++++++++++++++++++++++++ 
					 // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	case _connect_PD: {
		pg_send_message_udp((char*)"i", (char*)"/connect 1", (char*)"udp_PD_send");

		break;
	}

					// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
					// +++++++++++++++++ QUITS +++++++++++++++++++++++++++++++++ 
					// +++++++++++++++++   keystroke (ESC)  ++++++++++++++++++++ 
					// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	case _quit: {
		pg_quit();
		break;
	}

	case _MIDINote:
	{
		int argt0 = int(round(float_arguments[0]));
		if (argt0 >= 0) {
			pg_MIDINote = argt0;
		}
		else {
			pg_MIDINote = 0;
		}
		break;
	}
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// +++++++++++++++++ MUSIC CONTROL +++++++++++++++++++++++++ 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// received from pure data or generated by program if auto_sound is on
	case _pulse_spectrum: {
		pg_audio_pulse[0] = float_arguments[0] * sound_volume + sound_min;
		pg_audio_pulse[1] = float_arguments[1] * sound_volume + sound_min;
		pg_audio_pulse[2] = float_arguments[2] * sound_volume + sound_min;
		// not used currently  pulse_attack = float_arguments[3] * sound_volume + sound_min;
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
		// printf("%s\n", pg_AuxString);

		if (pg_FullScenarioActiveVars[pg_ind_scenario][_pen_color]
			&& pg_FullScenarioActiveVars[pg_ind_scenario][_pen_grey]
			&& !pen_hsv) {
			//printf("pg_audio_pulse %.2f %.2f %.2f avg %.2f\n", pg_audio_pulse[0], pg_audio_pulse[1], pg_audio_pulse[2], pulse_average);
			//printf("pen_color %.2f pen_color_pulse %.2f pen_grey %.2f pen_grey_pulse %.2f pg_pulsed_pen_color  %.2f %.2f %.2f %.2f\n", pen_color, pen_color_pulse, pen_grey, pen_grey_pulse, pg_pulsed_pen_color[0], pg_pulsed_pen_color[1], pg_pulsed_pen_color[2], pg_pulsed_pen_color[3]);
			pg_compute_pulsed_palette_color(pen_color, pen_color_pulse, pen_grey, pen_grey_pulse, pg_pulsed_pen_color, pg_enum_PEN_COLOR);
		}
		else if (pg_FullScenarioActiveVars[pg_ind_scenario][_pen_hue]
			&& pg_FullScenarioActiveVars[pg_ind_scenario][_pen_sat]
			&& pg_FullScenarioActiveVars[pg_ind_scenario][_pen_value]
			&& pen_hsv) {
			pg_compute_pulsed_HSV_color(pen_hue, pen_hue_pulse, pen_sat, pen_sat_pulse, pen_value, pen_value_pulse, pg_pulsed_pen_color, true);
		}

		sprintf(pg_AuxString, "/pen_color/color %02x%02x%02xFF", int(pg_pulsed_pen_color[0] * 255), int(pg_pulsed_pen_color[1] * 255), int(pg_pulsed_pen_color[2] * 255));
		pg_send_message_udp((char*)"s", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
		//printf("%s\n", pg_AuxString);
		pg_compute_pulsed_palette_color(repop_colorCA, repop_colorCA_pulse, repop_greyCA, repop_greyCA_pulse, pg_pulsed_repop_colorCA, pg_enum_REPOP_COLOR);
		sprintf(pg_AuxString, "/CA_repopColor/color %02x%02x%02xFF", int(pg_pulsed_repop_colorCA[0] * 255), int(pg_pulsed_repop_colorCA[1] * 255), int(pg_pulsed_repop_colorCA[2] * 255));
		pg_send_message_udp((char*)"s", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
		pg_compute_pulsed_palette_color(repop_colorBG, repop_colorBG_pulse, repop_greyBG, repop_greyBG_pulse, pg_pulsed_repop_colorBG, pg_enum_REPOP_COLOR);
		sprintf(pg_AuxString, "/BGcolorRedepopColor/color %02x%02x%02xFF", int(pg_pulsed_repop_colorBG[0] * 255), int(pg_pulsed_repop_colorBG[1] * 255), int(pg_pulsed_repop_colorBG[2] * 255));
		pg_send_message_udp((char*)"s", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");

		if (pg_FullScenarioActiveVars[pg_ind_scenario][_repop_colorPart]
			&& pg_FullScenarioActiveVars[pg_ind_scenario][_repop_greyPart]
			&& !repop_hsvPart) {
			pg_compute_pulsed_palette_color(repop_colorPart, repop_colorPart_pulse, repop_greyPart, repop_greyPart_pulse, pg_pulsed_repop_colorPart, pg_enum_REPOP_COLOR);
		}
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_repop_huePart]
			&& pg_FullScenarioActiveVars[pg_ind_scenario][_repop_satPart]
			&& pg_FullScenarioActiveVars[pg_ind_scenario][_repop_valuePart]
			&& repop_hsvPart) {
			pg_compute_pulsed_HSV_color(repop_huePart, repop_huePart_pulse, repop_satPart, repop_satPart_pulse, repop_valuePart, repop_valuePart_pulse, pg_pulsed_repop_colorPart, false);
		}
		sprintf(pg_AuxString, "/Part_repopColor/color %02x%02x%02xFF", int(pg_pulsed_repop_colorPart[0] * 255), int(pg_pulsed_repop_colorPart[1] * 255), int(pg_pulsed_repop_colorPart[2] * 255));
		pg_send_message_udp((char*)"s", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
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
		int argt0 = int(round(float_arguments[0]));
		if (argt0 >= 0) {
			pg_BeatNo = argt0;
		}
		else {
			pg_BeatNo++;
		}
	}
			  break;

	// ====================================== 
	// keystroke s: Snapshot                  
	// ====================================== 
	case _snapshot: {
		if (float_arguments[0] > 0) {
			pg_snapshot((char*)"jpg");
			pg_snapshot((char*)"svg");
		}
		break;
	}

#if defined(var_GenerativeNights_planes)
	// ====================================== 
	// relaunches CA initialization, in case it does not work                  
	// ====================================== 
	case _initialBGCapture: {
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_GenerativeNights_planes]) {
			pg_initialBGCapture = true;
		}

		// ====================================== 
		// initial background capture                  
		// ====================================== 
		break;
	}
		// CA initialization for Generative Nights
	case _initCA: {
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_GenerativeNights_planes]) {
			initCA = 1.2f;
		}
		break;
	}
#endif

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

	case _processing_master: {
		float argt0 = float_arguments[0];
		sprintf(pg_AuxString, "/master %.2f", argt0); pg_send_message_udp((char*)"f", (char*)pg_AuxString, (char*)"udp_Processing_send");
		printf("Processing message %s\n", pg_AuxString);
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
			pg_StartNewScene(int(float_arguments[0]), 0);
		}
		break;
	}

				   // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
				   // +++++++++++++++++ PEN POSITION ++++++++++++++++++++++++++ 
				   // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	case _pen_touch: {
		pg_CurrentCursorPos_x[0] = PG_OUT_OF_SCREEN_CURSOR;
		pg_CurrentCursorPos_y[0] = PG_OUT_OF_SCREEN_CURSOR;
		sprintf(pg_AuxString, "/pen_touch %.2f", float_arguments[0]); pg_send_message_udp((char*)"ff", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
		if (float_arguments[0] == 0) {
			if (pg_CurrentClockTime - pg_lastPenTouchTime < 0.5f) {
				// pg_send_message_udp((char *)"s", (char *)"/message clear_all", (char *)"udp_TouchOSC_send");
				pg_isClearAllLayers = 1.f;
				pg_lastPenTouchTime = 0.0f;
				//printf("****** pg_enum_PG_GUI_COMMAND pg_isClearAllLayers %d\n",pg_isClearAllLayers);
			}
			else {
				pg_lastPenTouchTime = pg_CurrentClockTime;
			}
		}
		break;
	}
	case _pen_xy: {
		pg_CurrentCursorPos_x[0] = int(pg_workingWindow_width * float_arguments[0]);
		pg_CurrentCursorPos_y[0] = int(PG_WINDOW_HEIGHT * float_arguments[1]);
		//sprintf(pg_AuxString, "/pen_xy %.2f %.2f", float_arguments[0], float_arguments[1]); pg_send_message_udp((char*)"ff", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
		break;
	}
	case _pen2_xy: {
		pg_CurrentCursorPos_x[1] = int(pg_workingWindow_width * float_arguments[0]);
		pg_CurrentCursorPos_y[1] = int(PG_WINDOW_HEIGHT * float_arguments[1]);
		sprintf(pg_AuxString, "/pen2_xy %.2f %.2f", float_arguments[0], float_arguments[1]); pg_send_message_udp((char*)"ff", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
		break;
	}
	case _pen3_xy: {
		pg_CurrentCursorPos_x[2] = int(pg_workingWindow_width * float_arguments[0]);
		pg_CurrentCursorPos_y[2] = int(PG_WINDOW_HEIGHT * float_arguments[1]);
		sprintf(pg_AuxString, "/pen3_xy %.2f %.2f", float_arguments[0], float_arguments[1]); pg_send_message_udp((char*)"ff", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
		break;
	}
	case _pen4_xy: {
		pg_CurrentCursorPos_x[3] = int(pg_workingWindow_width * float_arguments[0]);
		pg_CurrentCursorPos_y[3] = int(PG_WINDOW_HEIGHT * float_arguments[1]);
		sprintf(pg_AuxString, "/pen4_xy %.2f %.2f", float_arguments[0], float_arguments[1]); pg_send_message_udp((char*)"ff", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
		break;
	}
	case _pen5_xy: {
		pg_CurrentCursorPos_x[4] = int(pg_workingWindow_width * float_arguments[0]);
		pg_CurrentCursorPos_y[4] = int(PG_WINDOW_HEIGHT * float_arguments[1]);
		sprintf(pg_AuxString, "/pen5_xy %.2f %.2f", float_arguments[0], float_arguments[1]); pg_send_message_udp((char*)"ff", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
		break;
	}
	case _abs_pen_xy: {
		pg_CurrentCursorPos_x[0] = int(float_arguments[0]);
		pg_CurrentCursorPos_y[0] = int(float_arguments[1]);
		// printf("pos %.2f %.2f\n", float_arguments[0], float_arguments[1]);
		break;
	}

					// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
					// +++++++++++++++++ TRACK COPY +++++++++++++++++++++++++++ 
					// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	case _copyTrack_above: {
		pg_copyToNextTrack = +1;
		//sprintf(pg_AuxString, "/message copy_track_%d_to_%d",
		//	currentDrawingTrack, (currentDrawingTrack + 1) % PG_NB_TRACKS);
		//pg_send_message_udp((char *)"s", (char *)pg_AuxString, (char *)"udp_TouchOSC_send");

		break;
	}
	case _copyTrack_under: {
		pg_copyToNextTrack = -1;
		//sprintf(pg_AuxString, "/message copy_track_%d_to_%d", 
		//	currentDrawingTrack , (currentDrawingTrack - 1 + PG_NB_TRACKS) % PG_NB_TRACKS); 
		//pg_send_message_udp((char *)"s", (char *)pg_AuxString, (char *)"udp_TouchOSC_send");


		break;
	}

		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
		// +++++++++++++++++ PART EXIT/STROKE/COLOR MODE +++++++++++++++++++++++++++ 
		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	case _partStroke_mode_0: {
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_partStroke_mode]) {
			*((int*)pg_FullScenarioVarPointers[_partStroke_mode]) = 0;

			for (int ind = 0; ind < 3; ind++) {
				if (ind == *((int*)pg_FullScenarioVarPointers[_partStroke_mode])) {
					sprintf(pg_AuxString, "/partStroke_mode_%d 1", ind); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
				}
				else {
					sprintf(pg_AuxString, "/partStroke_mode_%d 0", ind); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
				}
			}
		}
		break;
	}
	case _partStroke_mode_1: {
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_partStroke_mode]) {
			*((int*)pg_FullScenarioVarPointers[_partStroke_mode]) = 1;

			for (int ind = 0; ind < 3; ind++) {
				if (ind == *((int*)pg_FullScenarioVarPointers[_partStroke_mode])) {
					sprintf(pg_AuxString, "/partStroke_mode_%d 1", ind); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
				}
				else {
					sprintf(pg_AuxString, "/partStroke_mode_%d 0", ind); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
				}
			}
		}
		break;
	}
	case _partStroke_mode_2: {
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_partStroke_mode]) {
			*((int*)pg_FullScenarioVarPointers[_partStroke_mode]) = 2;

			for (int ind = 0; ind < 3; ind++) {
				if (ind == *((int*)pg_FullScenarioVarPointers[_partStroke_mode])) {
					sprintf(pg_AuxString, "/partStroke_mode_%d 1", ind); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
				}
				else {
					sprintf(pg_AuxString, "/partStroke_mode_%d 0", ind); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
				}
			}
		}
		break;
	}
	case _partColor_mode_0: {
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_partColor_mode]) {
			*((int*)pg_FullScenarioVarPointers[_partColor_mode]) = 0;

			for (int ind = 0; ind < 3; ind++) {
				if (ind == *((int*)pg_FullScenarioVarPointers[_partColor_mode])) {
					sprintf(pg_AuxString, "/partColor_mode_%d 1", ind); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
				}
				else {
					sprintf(pg_AuxString, "/partColor_mode_%d 0", ind); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
				}
			}
		}
		break;
	}
	case _partColor_mode_1: {
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_partColor_mode]) {
			*((int*)pg_FullScenarioVarPointers[_partColor_mode]) = 1;

			for (int ind = 0; ind < 3; ind++) {
				if (ind == *((int*)pg_FullScenarioVarPointers[_partColor_mode])) {
					sprintf(pg_AuxString, "/partColor_mode_%d 1", ind); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
				}
				else {
					sprintf(pg_AuxString, "/partColor_mode_%d 0", ind); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
				}
			}
		}
		break;
	}
	case _partColor_mode_2: {
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_partColor_mode]) {
			*((int*)pg_FullScenarioVarPointers[_partColor_mode]) = 2;

			for (int ind = 0; ind < 3; ind++) {
				if (ind == *((int*)pg_FullScenarioVarPointers[_partColor_mode])) {
					sprintf(pg_AuxString, "/partColor_mode_%d 1", ind); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
				}
				else {
					sprintf(pg_AuxString, "/partColor_mode_%d 0", ind); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
				}
			}
		}
		break;
	}
	case _partExit_mode_0: {
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_partExit_mode]) {
			*((int*)pg_FullScenarioVarPointers[_partExit_mode]) = 0;

			for (int ind = 0; ind < 3; ind++) {
				if (ind == *((int*)pg_FullScenarioVarPointers[_partExit_mode])) {
					sprintf(pg_AuxString, "/partExit_mode_%d 1", ind); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
				}
				else {
					sprintf(pg_AuxString, "/partExit_mode_%d 0", ind); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
				}
			}
		}
		break;
	}
	case _partExit_mode_1: {
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_partExit_mode]) {
			*((int*)pg_FullScenarioVarPointers[_partExit_mode]) = 1;

			for (int ind = 0; ind < 3; ind++) {
				if (ind == *((int*)pg_FullScenarioVarPointers[_partExit_mode])) {
					sprintf(pg_AuxString, "/partExit_mode_%d 1", ind); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
				}
				else {
					sprintf(pg_AuxString, "/partExit_mode_%d 0", ind); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
				}
			}
		}
		break;
	}
	case _partExit_mode_2: {
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_partExit_mode]) {
			*((int*)pg_FullScenarioVarPointers[_partExit_mode]) = 2;

			for (int ind = 0; ind < 3; ind++) {
				if (ind == *((int*)pg_FullScenarioVarPointers[_partExit_mode])) {
					sprintf(pg_AuxString, "/partExit_mode_%d 1", ind); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
				}
				else {
					sprintf(pg_AuxString, "/partExit_mode_%d 0", ind); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
				}
			}
		}
		break;
	}

#if defined(PG_WITH_BLUR)
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

		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
		// +++++++++++++++++ CAMERA IMAGE CUMUL MODE +++++++++++++++ 
		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	case _cameraCumul_plus: {
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_cameraCaptFreq]) {
			cameraCumul = (cameraCumul + 1) % nb_camera_cumul_modes;
			pg_BrokenInterpolationVar[_cameraCumul] = true;
		}
		break;
	}

		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
		// +++++++++++++++++ BRUSH ID SHIFT ++++++++++++++++++++++++ 
		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	case _pen_brush_plus:
		if (nb_pen_brushes[pg_ind_scenario] > 0) {
			pen_brush = (pen_brush + 1) % (nb_pen_brushes[pg_ind_scenario] * 3);
			pg_BrokenInterpolationVar[_pen_brush] = true;
			//if (pen_brush >= 1) { // large radius for the image brushes*
			//	pen_radiusMultiplier = 50.0f;
			//}
			//else {
			//	pen_radiusMultiplier = 1.0f;
			//}
		}
		break;
	case _pen_brush_minus:
		if (nb_pen_brushes[pg_ind_scenario] > 0) {
			pen_brush = (pen_brush - 1 + (nb_pen_brushes[pg_ind_scenario] * 3)) % (nb_pen_brushes[pg_ind_scenario] * 3);
			pg_BrokenInterpolationVar[_pen_brush] = true;
			//if (pen_brush >= 1) { // large radius for the image brushes*
			//	pen_radiusMultiplier = 50.0f;
			//}
			//else {
			//	pen_radiusMultiplier = 1.0f;
			//}
		}
		break;

	case _pen_brush_replay_plus:
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_pen_brush_replay]) {
			pen_brush_replay += 1;
			*((int*)pg_FullScenarioVarPointers[_pen_brush_replay]) = pen_brush_replay;
		}
		break;
	case _pen_brush_replay_minus:
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_pen_brush_replay]) {
			pen_brush_replay -= 1;
			*((int*)pg_FullScenarioVarPointers[_pen_brush_replay]) = pen_brush_replay;
		}
		break;

		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
		// +++++++++++++++++ PIXEL MODE +++++++++++++++++++ 
		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	case _pixel_mode_plus: {
		pixel_mode = (pixel_mode + 1) % nb_pixel_modes;
		pg_BrokenInterpolationVar[_pixel_mode] = true;
		break;
	}

		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
		// +++++++++++++++++ CA TYPE AND SUBTYPE +++++++++++++++++++ 
		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	case _CA1Type_plus: {
		CA1Type = (CA1Type + 1) % nb_CATypes;
		pg_BrokenInterpolationVar[_CA1Type] = true;
		// printf("CA1Type %d\n", CA1Type);
		*((int*)pg_FullScenarioVarPointers[_CA1Type]) = CA1Type;
		break;
	}
	case _CA1Type_minus: {
		CA1Type = (CA1Type - 1 + nb_CATypes) % nb_CATypes;
		pg_BrokenInterpolationVar[_CA1Type] = true;
		// printf("CA1Type %d\n", CA1Type);
		*((int*)pg_FullScenarioVarPointers[_CA1Type]) = CA1Type;
		break;
	}
	case _CA1SubType_plus: {
		CA1SubType = (CA1SubType + 1) % nb_CASubTypes;
		pg_BrokenInterpolationVar[_CA1SubType] = true;
		*((int*)pg_FullScenarioVarPointers[_CA1SubType]) = CA1SubType;
		break;
	}
	case _CA1SubType_minus: {
		CA1SubType = (CA1SubType - 1 + nb_CASubTypes) % nb_CASubTypes;
		pg_BrokenInterpolationVar[_CA1SubType] = true;
		*((int*)pg_FullScenarioVarPointers[_CA1SubType]) = CA1SubType;
		break;
	}
	case _CAonOff: {
		if (CA1SubType != 0) {
			pg_CASubTypeMem = CA1SubType;
			CA1SubType = 0;
		}
		else {
			CA1SubType = pg_CASubTypeMem;
		}
		pg_BrokenInterpolationVar[_CA1SubType] = true;
		printf("CA1SubType ON/OFF %d\n", CA1SubType);
		*((int*)pg_FullScenarioVarPointers[_CA1SubType]) = CA1SubType;
		break;
	}

							 // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
							 // +++++++++++++++++ B/W SWITCH ++++++++++++++++++++++++++++ 
							 // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
							 // ====================================== 
	case _pen_BW: {
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_pen_grey]) {
			if (pen_grey > 0) {
				pen_grey = 0.0f;
			}
			else {
				pen_grey = 1.0f;
			}
			pg_BrokenInterpolationVar[_pen_grey] = true;
		}
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_pen_value]) {

			if (pen_value > 0) {
				pen_value = 0.0f;
			}
			else {
				pen_value = 1.0f;
			}
			pg_BrokenInterpolationVar[_pen_value] = true;
		}
		break;
	}
	case _repopBG_BW: {
		if (repop_greyBG > 0) {
			repop_greyBG = 0.0f;
		}
		else {
			repop_greyBG = 1.0f;
		}
		pg_BrokenInterpolationVar[_repop_greyBG] = true;
		// printf("repop color %.1f\n", repop_grey);
		break;
	}
	case _repopCA_BW: {
		if (repop_greyCA > 0) {
			repop_greyCA = 0.0f;
		}
		else {
			repop_greyCA = 1.0f;
		}
		pg_BrokenInterpolationVar[_repop_greyCA] = true;
		// printf("repop color %.1f\n", repop_grey);
		break;
	}
	case _repopPart_BW: {
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_repop_greyPart]) {
			if (repop_greyPart > 0) {
				repop_greyPart = 0.0f;
			}
			else {
				repop_greyPart = 1.0f;
			}
			pg_BrokenInterpolationVar[_repop_greyPart] = true;
		}
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_repop_valuePart]) {
			if (repop_valuePart > 0) {
				repop_valuePart = 0.0f;
			}
			else {
				repop_valuePart = 1.0f;
			}
			pg_BrokenInterpolationVar[_repop_valuePart] = true;
		}
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
		pg_BrokenInterpolationVar[_pen_radius_pressure_coef] = true;
		break;
	}


						  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
						  // +++++++++++++++++ PALETTE NO ++++++++++++++++++++++++++++ 
						  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
						  // ====================================== 
	case _pen_colorPreset_minus: {
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_pen_color]) {
			if (pg_ColorPresets[pg_ind_scenario].size() > 0) {
				pg_current_pen_colorPreset = (pg_current_pen_colorPreset - 1 + pg_ColorPresets[pg_ind_scenario].size()) % pg_ColorPresets[pg_ind_scenario].size();
				// printf( "/new palette %d\n", pg_current_pen_colorPreset);
				pg_BrokenInterpolationVar[_pen_color] = true;
				*((float*)pg_FullScenarioVarPointers[_pen_color]) = pg_ColorPresets[pg_ind_scenario][pg_current_pen_colorPreset].pg_colorPreset_values.color;
				pg_BrokenInterpolationVar[_pen_grey] = true;
				*((float*)pg_FullScenarioVarPointers[_pen_grey]) = pg_ColorPresets[pg_ind_scenario][pg_current_pen_colorPreset].pg_colorPreset_values.grey;
				pg_BrokenInterpolationVar[_pen_color_a] = true;
				*((float*)pg_FullScenarioVarPointers[_pen_color_a]) = pg_ColorPresets[pg_ind_scenario][pg_current_pen_colorPreset].pg_colorPreset_values.alpha;
				// sprintf(pg_AuxString, "/pen_color %.3f", pen_colorPresets_values[pg_current_pen_colorPreset]);
				// pg_send_message_udp((char *)"s", pg_AuxString, (char *)"udp_TouchOSC_send");
				// sprintf(pg_AuxString, "/message palette%s", pg_ColorPresets[pg_ind_scenario][pg_current_pen_colorPreset].pen_colorPresets_names.c_str()); pg_send_message_udp((char *)"s", (char *)pg_AuxString, (char *)"udp_TouchOSC_send");
				sprintf(pg_AuxString, "/pen_colorPreset %d", pg_current_pen_colorPreset); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
			}
		}

		break;
	}
	case _pen_colorPreset_plus: {
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_pen_color]) {
			if (pg_ColorPresets[pg_ind_scenario].size() > 0) {
				pg_current_pen_colorPreset = (pg_current_pen_colorPreset + 1) % pg_ColorPresets[pg_ind_scenario].size();
				pg_BrokenInterpolationVar[_pen_color] = true;
				*((float*)pg_FullScenarioVarPointers[_pen_color]) = pg_ColorPresets[pg_ind_scenario][pg_current_pen_colorPreset].pg_colorPreset_values.color;
				pg_BrokenInterpolationVar[_pen_grey] = true;
				*((float*)pg_FullScenarioVarPointers[_pen_grey]) = pg_ColorPresets[pg_ind_scenario][pg_current_pen_colorPreset].pg_colorPreset_values.grey;
				pg_BrokenInterpolationVar[_pen_color_a] = true;
				*((float*)pg_FullScenarioVarPointers[_pen_color_a]) = pg_ColorPresets[pg_ind_scenario][pg_current_pen_colorPreset].pg_colorPreset_values.alpha;
				// sprintf(pg_AuxString, "/pen_color %.3f", pen_colorPresets_values[pg_current_pen_colorPreset]);
				// pg_send_message_udp((char *)"s", pg_AuxString, (char *)"udp_TouchOSC_send");
				// sprintf(pg_AuxString, "/message palette%s", pg_ColorPresets[pg_ind_scenario][pg_current_pen_colorPreset].pen_colorPresets_names.c_str()); pg_send_message_udp((char *)"s", (char *)pg_AuxString, (char *)"udp_TouchOSC_send");
				sprintf(pg_AuxString, "/pen_colorPreset %d", pg_current_pen_colorPreset); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
			}
		}
		break;
	}
	case _pen_colorPreset: {
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_pen_color]) {
			int presetValue = int(float_arguments[0]);
			if (pg_ColorPresets[pg_ind_scenario].size() > 0) {
				pg_current_pen_colorPreset = presetValue % pg_ColorPresets[pg_ind_scenario].size();
				pg_BrokenInterpolationVar[_pen_color] = true;
				*((float*)pg_FullScenarioVarPointers[_pen_color]) = pg_ColorPresets[pg_ind_scenario][pg_current_pen_colorPreset].pg_colorPreset_values.color;
				pg_BrokenInterpolationVar[_pen_grey] = true;
				*((float*)pg_FullScenarioVarPointers[_pen_grey]) = pg_ColorPresets[pg_ind_scenario][pg_current_pen_colorPreset].pg_colorPreset_values.grey;
				pg_BrokenInterpolationVar[_pen_color_a] = true;
				*((float*)pg_FullScenarioVarPointers[_pen_color_a]) = pg_ColorPresets[pg_ind_scenario][pg_current_pen_colorPreset].pg_colorPreset_values.alpha;
				// sprintf(pg_AuxString, "/pen_color %.3f", pen_colorPresets_values[pg_current_pen_colorPreset]);
				// pg_send_message_udp((char *)"s", pg_AuxString, (char *)"udp_TouchOSC_send");
				// sprintf(pg_AuxString, "/message palette%s", pg_ColorPresets[pg_ind_scenario][pg_current_pen_colorPreset].pen_colorPresets_names.c_str()); pg_send_message_udp((char *)"s", (char *)pg_AuxString, (char *)"udp_TouchOSC_send");
				// printf("************* palette %d\n", pg_current_pen_colorPreset);
				sprintf(pg_AuxString, "/pen_colorPreset %d", pg_current_pen_colorPreset); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
				printf("sent: %s\n", pg_AuxString);
			}
		}
		break;
	}

#if defined(pg_Project_CAaudio)
						 // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
						 // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
						 // +++++++++++++++++ CA SEEDING ++++++++++++++++++++++++++++
						 // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
						 // ====================================== 

	case _CAseed_dot_center: {
		pg_CAseed_type = _pg_CAseed_dot_center;
		sprintf(pg_AuxString, "/CAseed_dot_center %d", 1); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
		sprintf(pg_AuxString, "/CAseed_dot %d", 0); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
		sprintf(pg_AuxString, "/CAseed_h_line %d", 0); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
		sprintf(pg_AuxString, "/CAseed_v_line %d", 0); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
		sprintf(pg_AuxString, "/CAseed_cross %d", 0); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
		sprintf(pg_AuxString, "/CAseed_X %d", 0); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
		sprintf(pg_AuxString, "/CAseed_square %d", 0); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
		break;
	}
	case _CAseed_dot: {
		pg_CAseed_type = _pg_CAseed_dot;
		sprintf(pg_AuxString, "/CAseed_dot_center %d", 0); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
		sprintf(pg_AuxString, "/CAseed_dot %d", 1); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
		sprintf(pg_AuxString, "/CAseed_h_line %d", 0); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
		sprintf(pg_AuxString, "/CAseed_v_line %d", 0); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
		sprintf(pg_AuxString, "/CAseed_cross %d", 0); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
		sprintf(pg_AuxString, "/CAseed_X %d", 0); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
		sprintf(pg_AuxString, "/CAseed_square %d", 0); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
		break;
	}
	case _CAseed_h_line: {
		sprintf(pg_AuxString, "/CAseed_dot_center %d", 0); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
		sprintf(pg_AuxString, "/CAseed_dot %d", 0); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
		sprintf(pg_AuxString, "/CAseed_h_line %d", 1); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
		sprintf(pg_AuxString, "/CAseed_v_line %d", 0); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
		sprintf(pg_AuxString, "/CAseed_cross %d", 0); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
		sprintf(pg_AuxString, "/CAseed_X %d", 0); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
		sprintf(pg_AuxString, "/CAseed_square %d", 0); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
		pg_CAseed_type = _pg_CAseed_h_line;
		break;
	}
	case _CAseed_v_line: {
		sprintf(pg_AuxString, "/CAseed_dot_center %d", 0); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
		sprintf(pg_AuxString, "/CAseed_dot %d", 0); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
		sprintf(pg_AuxString, "/CAseed_h_line %d", 0); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
		sprintf(pg_AuxString, "/CAseed_v_line %d", 1); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
		sprintf(pg_AuxString, "/CAseed_cross %d", 0); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
		sprintf(pg_AuxString, "/CAseed_X %d", 0); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
		sprintf(pg_AuxString, "/CAseed_square %d", 0); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
		pg_CAseed_type = _pg_CAseed_v_line;
		break;
	}
	case _CAseed_cross: {
		sprintf(pg_AuxString, "/CAseed_dot_center %d", 0); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
		sprintf(pg_AuxString, "/CAseed_dot %d", 0); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
		sprintf(pg_AuxString, "/CAseed_h_line %d", 0); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
		sprintf(pg_AuxString, "/CAseed_v_line %d", 0); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
		sprintf(pg_AuxString, "/CAseed_cross %d", 1); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
		sprintf(pg_AuxString, "/CAseed_X %d", 0); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
		sprintf(pg_AuxString, "/CAseed_square %d", 0); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
		pg_CAseed_type = _pg_CAseed_cross;
		break;
	}
	case _CAseed_X: {
		sprintf(pg_AuxString, "/CAseed_dot_center %d", 0); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
		sprintf(pg_AuxString, "/CAseed_dot %d", 0); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
		sprintf(pg_AuxString, "/CAseed_h_line %d", 0); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
		sprintf(pg_AuxString, "/CAseed_v_line %d", 0); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
		sprintf(pg_AuxString, "/CAseed_cross %d", 0); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
		sprintf(pg_AuxString, "/CAseed_X %d", 1); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
		sprintf(pg_AuxString, "/CAseed_square %d", 0); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
		pg_CAseed_type = _pg_CAseed_X;
		break;
	}
	case _CAseed_square: {
		sprintf(pg_AuxString, "/CAseed_dot_center %d", 0); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
		sprintf(pg_AuxString, "/CAseed_dot %d", 0); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
		sprintf(pg_AuxString, "/CAseed_h_line %d", 0); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
		sprintf(pg_AuxString, "/CAseed_v_line %d", 0); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
		sprintf(pg_AuxString, "/CAseed_cross %d", 0); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
		sprintf(pg_AuxString, "/CAseed_X %d", 0); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
		sprintf(pg_AuxString, "/CAseed_square %d", 1); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
		pg_CAseed_type = _pg_CAseed_square;
		break;
	}
	case _CAseed_size: {
		pg_CAseed_size = int(float_arguments[0]);
		sprintf(pg_AuxString, "/CAseed_size %d", pg_CAseed_size); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
		break;
	}
	case _CAseed_loc: {
		pg_CAseed_location = pg_CAseed_locations(int(float_arguments[0]) % _pg_Nb_CAseed_locations);
		pg_CAseed_location_to_coordinates(pg_CAseed_location, pg_CAseed_coordinates);
		sprintf(pg_AuxString, "/CAseed_loc %d", pg_CAseed_location); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
		break;
	}
	case _CAseed_loc_plus: {
		pg_CAseed_location = pg_CAseed_locations((pg_CAseed_location + 1) % _pg_Nb_CAseed_locations);
		pg_CAseed_location_to_coordinates(pg_CAseed_location, pg_CAseed_coordinates);
		sprintf(pg_AuxString, "/CAseed_loc %d", pg_CAseed_location); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
		break;
	}
	case _CAseed_trigger: {
		pg_CAseed_trigger = true;
		break;
	}
#endif

#if defined(pg_Project_Criton)
						// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
						// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
						// +++++++++++++++++ SOUND CONTROL +++++++++++++++++++++++++
						// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
						// ====================================== 
	case _JUCE_loop_track: {
		pg_send_message_udp((char*)"", (char*)"/JUCE_loop_track", (char*)"udp_SoundJUCE_send");
		break;
	}
	case _JUCE_exit: {
		pg_send_message_udp((char*)"", (char*)"/JUCE_exit", (char*)"udp_SoundJUCE_send");
		break;
	}
	case _fftLevel8: {
		//printf("fft levels: ");
		//for (int indArg = 0; indArg < 8; indArg++) {
		//	printf("%.2f/%.2f ", float_arguments[2 * indArg], float_arguments[2 * indArg + 1]);
		//}
		//printf("\n");
		float totFFTLevel = 0.f;
		for (int indArg = 0; indArg < 8; indArg++) {
			fftFrequencies[indArg] = float_arguments[3 * indArg];
			fftLevels[indArg] = float_arguments[3 * indArg + 1];
			fftPhases[indArg] = float_arguments[3 * indArg + 2];
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
						  // +++++++++++++++++ SET-UP ++++++++++++++++++++++++++++++++ 
						  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
						  // ====================================== 
	case _resend_all: {
		pg_resend_all_variables = true;
		break;
	}
	case _resend_all_light: {
		pg_resend_all_light_variables = true;
		break;
	}
	case _setupNo: {
		pg_scene(int(round(float_arguments[0])));
		break;
	}
	case _configurationNo: {
		int config_no = int(round(float_arguments[0]));
		if (config_no >= 0 && config_no != pg_ind_scenario) {
			config_no = config_no % pg_NbConfigurations;
			pg_StartNewConfiguration(config_no);
			pg_logFirstLineSceneVariables();
			pg_ConfigurationFrameNo = 0;
		}
		sprintf(pg_AuxString, "/configurationNo %d", pg_ind_scenario);
		pg_send_message_udp((char*)"i", pg_AuxString, (char*)"udp_TouchOSC_send");

		break;
	}
	case _setupName: {
		std::string sceneID = string_argument_0;
		std::regex newLines_re("\n+");
		std::regex dquotes_re("\"");
		sceneID = std::regex_replace(sceneID, newLines_re, "");
		sceneID = std::regex_replace(sceneID, dquotes_re, "");
		int ind_scene = pg_FindSceneById(&sceneID);
		if (ind_scene >= 0 && ind_scene < int(pg_Scenario[pg_ind_scenario].size())) {
			pg_scene(ind_scene);
		}
		else {
			sprintf(pg_errorStr, "Unknown scene ID %s in /setupName OSC command (%d)!", string_argument_0.c_str(), ind_scene); pg_ReportError(pg_errorStr);

		}
		break;
	}
	case _setup_plus: {
		pg_scene_plus(1);
		break;
	}
	case _interpolation_duration: {
		pg_SceneInterpolationDuration = float_arguments[0];
		sprintf(pg_AuxString, "/interpolation_duration %.2f", pg_SceneInterpolationDuration);
		pg_send_message_udp((char*)"i", pg_AuxString, (char*)"udp_TouchOSC_send");
		break;
	}
	case _master_incay: {
		pg_master_incay_duration = double(float_arguments[0]);
		pg_master_incay_start_time = pg_CurrentClockTime;
		pg_master_incay_start_value = master;
		break;
	}
	case _master_decay: {
		pg_master_decay_duration = double(float_arguments[0]);
		pg_master_decay_start_time = pg_CurrentClockTime;
		pg_master_decay_start_value = master;
		break;
	}
	case _setup_plus_5: {
		pg_scene_plus(5);
		break;
	}
	case _setup_minus: {
		pg_scene_minus(1);
		break;
	}
	case _setup_minus_5: {
		pg_scene_minus(5);
		break;
	}
	case _setup_plus_keep_total_dur: {
		if (pg_CurrentScene) {
			double elapsedTime = (pg_CurrentClockTime - pg_InitialScenarioTime) - pg_CurrentScene->scene_initial_time;
			double deltaTime = pg_Scenario[pg_ind_scenario][1 + pg_CurrentSceneIndex].scene_initial_time - (pg_CurrentClockTime - pg_InitialScenarioTime);
			// only accepted if the current scene has been on for a while
			if (elapsedTime > 15
				// if the scene has not begun since a short while
				&& deltaTime > 0
				// and the current scene is not finished 
				&& deltaTime < 60) {
				// and if the scene is not too far from being finished
				int new_scene = ((1 + pg_CurrentSceneIndex) % pg_Scenario[pg_ind_scenario].size());

				pg_StartNewScene(new_scene, deltaTime);
			}
		}
		break;
	}

	case _take_snapshot:
		pg_draw_scene(pg_enum_render_Jpg);
		sprintf(pg_AuxString, "/snapshot_ID Snap_%04d", pg_Jpg_Capture_param.indJpgSnapshot); pg_send_message_udp((char*)"s", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
		break;
	case _muteRightScreen_onOff:
		pg_muteRightScreen = !pg_muteRightScreen;
		break;
	case _isClearAllLayers:
		pg_isClearAllLayers = 1.f;
		break;
	case _StylusvsRubber:
		if (float_arguments[0] > 0) {
			pg_CurrentStylus_StylusvsRubber = pg_Stylus;
		}
		else {
			pg_CurrentStylus_StylusvsRubber = pg_Rubber;
		}
		// drawing type initialization
		ExclusiveButtonsAndLabelsOnOff(pg_StylusvsRubberButtonsPaths, pg_StylusvsRubberButtonLabelsPaths, pg_StylusvsRubberButtonValues, false, pg_CurrentStylus_StylusvsRubber);
		break;
	case _pen_hue_sat_value:
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_pen_hue]
			&& pg_FullScenarioActiveVars[pg_ind_scenario][_pen_sat]
			&& pg_FullScenarioActiveVars[pg_ind_scenario][_pen_value]
			&& pen_hsv) {
			pen_hue = float_arguments[0];
			pg_BrokenInterpolationVar[_pen_hue] = true;
			*((float*)pg_FullScenarioVarPointers[_pen_hue]) = pen_hue;
			pen_sat = float_arguments[1];
			pg_BrokenInterpolationVar[_pen_sat] = true;
			*((float*)pg_FullScenarioVarPointers[_pen_sat]) = pen_sat;
			pen_value = float_arguments[2];
			pg_BrokenInterpolationVar[_pen_value] = true;
			*((float*)pg_FullScenarioVarPointers[_pen_value]) = pen_value;
			pg_HSVtoRGB(pen_hue, pen_sat, pen_value, &control_color[0], &control_color[1], &control_color[2]);
			sprintf(pg_AuxString, "/pen_color/color %02x%02x%02xFF", int(control_color[0] * 255), int(control_color[1] * 255), int(control_color[2] * 255)); pg_send_message_udp((char*)"s", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
			//printf("color2 msg %s\n", pg_AuxString);
		}
		break;
	case _repop_hue_sat_valuePart:
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_repop_huePart]
			&& pg_FullScenarioActiveVars[pg_ind_scenario][_repop_satPart]
			&& pg_FullScenarioActiveVars[pg_ind_scenario][_repop_valuePart]
			&& repop_hsvPart) {
			repop_huePart = float_arguments[0];
			pg_BrokenInterpolationVar[_repop_huePart] = true;
			*((float*)pg_FullScenarioVarPointers[_repop_huePart]) = repop_huePart;
			repop_satPart = float_arguments[1];
			pg_BrokenInterpolationVar[_repop_satPart] = true;
			*((float*)pg_FullScenarioVarPointers[_repop_satPart]) = repop_satPart;
			repop_valuePart = float_arguments[2];
			pg_BrokenInterpolationVar[_repop_valuePart] = true;
			*((float*)pg_FullScenarioVarPointers[_repop_valuePart]) = repop_valuePart;
			pg_HSVtoRGB(repop_huePart, repop_satPart, repop_valuePart, &control_color[0], &control_color[1], &control_color[2]);
			sprintf(pg_AuxString, "/part_color/color %02x%02x%02xFF", int(control_color[0] * 255), int(control_color[1] * 255), int(control_color[2] * 255)); pg_send_message_udp((char*)"s", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
			//printf("color2 msg %s\n", pg_AuxString);
		}
		break;
	default: {
		sprintf(pg_errorStr, "Command not found (%s)!", address_string.c_str()); pg_ReportError(pg_errorStr);
		break;
	}
	}
}


int pg_FindSceneById(std::string* sceneID) {
	int ind_scene = 0;
	for (Scene& scene : pg_Scenario[pg_ind_scenario]) {
		if (scene.scene_IDs.compare(*sceneID) == 0) {
			return ind_scene;
		}
		ind_scene++;
	}
	return -1;
}


