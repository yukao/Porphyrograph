////////////////////////////////////////////////////////////////////
// _
// 
// 
//     File pg-update.cpp
// 
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2.1
// of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this program; if not, write to the Free
// Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.
////////////////////////////////////////////////////////////////////

#include "pg-all_include.h"

#if defined(_CRTDBG_MAP_ALLOC)
// MEMORY LEAK CONTROL
_CrtMemState s1{};
_CrtMemState s2{};
_CrtMemState s3{};
_CrtMemState s4{};
_CrtMemState s5{};
_CrtMemState s6{};
_CrtMemState s7{};
_CrtMemState s8{};
_CrtMemState s9{};
_CrtMemState s10{};
_CrtMemState s11{};
_CrtMemState s12{};
_CrtMemState s13{};
_CrtMemState sDiff{};
#endif
      
/////////////////////////////////////////////////////////////////
// config variables
// large array for passing the path data to the fragment shader
float path_data_Update[(PG_NB_PATHS + 1) * PG_MAX_PATH_DATA * 4];
float path_data_ParticleAnimation[(PG_NB_PATHS + 1) * PG_MAX_PATH_ANIM_DATA * 4];

float paths_x_next[PG_NB_PATHS + 1];
float paths_y_next[PG_NB_PATHS + 1];

// path variables
float paths_time[PG_NB_PATHS + 1];
float paths_time_prev[PG_NB_PATHS + 1];
float paths_time_prev_prev[PG_NB_PATHS + 1];
float paths_x[PG_NB_PATHS + 1];
float paths_y[PG_NB_PATHS + 1];
float paths_x_prev[PG_NB_PATHS + 1];
float paths_y_prev[PG_NB_PATHS + 1];
float paths_x_prev_prev[PG_NB_PATHS + 1];
float paths_y_prev_prev[PG_NB_PATHS + 1];
float paths_x_memory[PG_NB_PATHS + 1];
float paths_y_memory[PG_NB_PATHS + 1];
float paths_x_prev_memory[PG_NB_PATHS + 1];
float paths_y_prev_memory[PG_NB_PATHS + 1];
bool isBegin[PG_NB_PATHS + 1];
bool isEnd[PG_NB_PATHS + 1];
float paths_Color_r[PG_NB_PATHS + 1];
float paths_Color_g[PG_NB_PATHS + 1];
float paths_Color_b[PG_NB_PATHS + 1];
float paths_Color_a[PG_NB_PATHS + 1];
float paths_RadiusX[PG_NB_PATHS + 1];
float paths_RadiusY[PG_NB_PATHS + 1];

float paths_xL[PG_NB_PATHS + 1];
float paths_yL[PG_NB_PATHS + 1];
float paths_xR[PG_NB_PATHS + 1];
float paths_yR[PG_NB_PATHS + 1];
float tang_x[PG_NB_PATHS + 1];
float tang_y[PG_NB_PATHS + 1];
float tang_x_prev[PG_NB_PATHS + 1];
float tang_y_prev[PG_NB_PATHS + 1];

int paths_BrushID[PG_NB_PATHS + 1];

float repop_ColorBGcolorRed;
float repop_ColorBG_g;
float repop_ColorBG_b;
float repop_ColorCA_r;
float repop_ColorCA_g;
float repop_ColorCA_b;
float repop_ColorPart_r;
float repop_ColorPart_g;
float repop_ColorPart_b;

// interpolated CA
int CAInterpolatedType;
int CAInterpolatedSubType;
int CAInterpolatedType_prev = -1;
int CAInterpolatedSubType_prev = -1;

// svg gpu translation
float				svg_translate_x = 0.f;
float				svg_translate_y = 0.f;
float				svg_translate_z = 0.f;
// svg gpu scaling
float				svg_scale = 1.f;

// mobile cursor for cursor blinking in Master shader
bool mobile_cursor = true;

// MVP matrices
glm::mat4 VP1perspMatrix;
glm::mat4 VP1viewMatrix;
glm::mat4 MeshPosModelMatrix;
glm::mat4 ObjectPosModelMatrix;
glm::mat4 VP1homographyMatrix;
#if defined(PG_SECOND_MESH_CAMERA)
glm::mat4 VP2perspMatrix;
glm::mat4 VP2viewMatrix;
glm::mat4 VP2homographyMatrix;
#endif

// homography matrix
float matValues[16] = { 0 };

cv::Mat homography;

float pen_x, pen_y, vec_x, vec_y;

//////////////////////////////////////////////////////////////////////
// SENSORS
//////////////////////////////////////////////////////////////////////

// sensor translations
// current sensor layout
float sensorPositions[3 * PG_NB_SENSORS];
// all possible sensor layouts
float sensorLayouts[3 * PG_NB_SENSORS * PG_NB_MAX_SENSOR_LAYOUTS];

// sensor on off
// current sensor activation pattern
bool sensor_onOff[PG_NB_SENSORS];
double sensor_last_activation_time;
// sensor activation as read from the pixel under their center
float sensorLevel[PG_NB_SENSORS];
// all sensor activation patterns
bool sensorActivations[PG_NB_SENSORS * PG_NB_MAX_SENSOR_ACTIVATIONS];

// sample choice
// current sample choice
int sample_choice[PG_NB_SENSORS];
#ifndef pg_Project_CAaudio
// all possible sensor layouts
int sensor_sample_setUps[PG_NB_MAX_SAMPLE_SETUPS][PG_NB_SENSORS] =
{ { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 },
{ 16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31 },
{ 32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47 } };
// sample start playing time for muting after 1 cycle
double sample_play_start[PG_NB_MAX_SAMPLE_SETUPS * PG_NB_SENSORS] =
{ -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1.,
-1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1.,
-1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1. };
float sample_play_volume[PG_NB_MAX_SAMPLE_SETUPS * PG_NB_SENSORS] =
{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
#else
// all possible sensor layouts
int sensor_sample_setUps[PG_NB_MAX_SAMPLE_SETUPS][PG_NB_SENSORS] =
{ { 0 }, { 1 }, { 2 } };
// sample start playing time for muting after 1 cycle
double sample_play_start[PG_NB_MAX_SAMPLE_SETUPS * PG_NB_SENSORS] =
{ -1.f, -1.f, -1.f };
float sample_play_volume[PG_NB_MAX_SAMPLE_SETUPS * PG_NB_SENSORS] =
{ 0.0, 0.0, 0.0 };
#endif

#ifndef pg_Project_CAaudio
// sensor rendering
int Sensor_order[PG_NB_SENSORS] = { 8, 13, 14, 11, 7, 2, 1, 4, 12, 15, 3, 0, 9, 10, 5, 6 };
#else
// sensor rendering
int Sensor_order[PG_NB_SENSORS] = { 0 };
#endif

// current sensor
int currentSensor = 0;

/////////////////////////////////////////////////////////////////
// CAMERA PARAMETERS
float CameraCurrent_exposure = 0.0f;
float CameraCurrent_gain = 0.0f;
float CameraCurrent_brightness = 0.0f;
float CameraCurrent_saturation = 0.0f;
float CameraCurrent_contrast = 0.0f;
float CameraCurrent_FPS = 0.0f;
float CameraCurrent_focus = 0.0f;
float CameraCurrent_gamma = 0.0f;
float CameraCurrent_WB_B = 0.0f;
float CameraCurrent_WB_R = 0.0f;

/////////////////////////////////////////////////////////////////
// VIDEO PARAMETERS
int precedingVideoTrack = -1;

/////////////////////////////////////////////////////////////////
// textures bitmaps and associated IDs
std::vector<GLuint> pg_particle_initial_pos_speed_texID[PG_MAX_CONFIGURATIONS];
std::vector<GLuint> pg_particle_initial_color_radius_texID[PG_MAX_CONFIGURATIONS];
std::vector<GLuint> pg_particle_acc_texID[PG_MAX_CONFIGURATIONS];
std::vector<GLuint> pg_pixel_acc_texID[PG_MAX_CONFIGURATIONS];

GLuint pg_screenMessageBitmap_texID = 0; // nb_attachments=1
GLubyte *pg_screenMessageBitmap = NULL;

GLuint pg_CATable_ID = NULL_ID;
GLubyte *pg_CATable = NULL;


GLuint Pen_texture_3D_texID[PG_MAX_CONFIGURATIONS] = { NULL_ID };
GLuint Noise_texture_3D[PG_MAX_CONFIGURATIONS] = { NULL_ID };
std::vector<GLuint>  pg_RepopDensity_texture_texID[PG_MAX_CONFIGURATIONS];

GLuint Master_Mask_texID[PG_MAX_CONFIGURATIONS] = { NULL_ID };
GLuint Master_Multilayer_Mask_texID[PG_MAX_CONFIGURATIONS] = { NULL_ID };

std::string* DisplayTextList = NULL;
int* DisplayTextFirstInChapter = NULL;
int NbDisplayTexts = 0;
int DisplayText_maxLen = 0;
int pg_Ind_Current_DisplayText = 0;
float * DisplayText_rand_translX = NULL;
float * DisplayText_rand_translY = NULL;


// message chapters
int NbTextChapters = 0;

GLuint Sensor_texture_rectangle[PG_MAX_CONFIGURATIONS] = { NULL_ID };

// video texture ID, image and camera
GLuint pg_camera_texture_texID = 0;
GLuint pg_movie_texture_texID = 0;
GLuint pg_camera_BG_texture_texID = 0;
GLuint pg_camera_BGIni_texture_texID = 0;

// IplImage *pg_camera_frame = NULL;
// IplImage *pg_movie_frame = NULL;
int pg_camera_frame_width = -1;
int pg_camera_frame_height = -1;
int pg_camera_x_offset = -1;
int pg_camera_y_offset = -1;
int pg_movie_frame_width = 0;
int pg_movie_frame_height = 0;

// video tracks
vector<VideoTrack> pg_VideoTracks[PG_MAX_CONFIGURATIONS];

// soundtracks
vector<SoundTrack> pg_SoundTracks[PG_MAX_CONFIGURATIONS];
int currentTrackSoundPeakIndex = 0;
int nbTrackSoundPeakIndex[PG_MAX_CONFIGURATIONS] = {0};
int currentTrackSoundOnsetIndex = 0;
int nbTrackSoundOnsetIndex[PG_MAX_CONFIGURATIONS] = {0};

// pen palettes presets
vector<ColorPreset> pg_ColorPresets[PG_MAX_CONFIGURATIONS];

// lights presets
int pg_nb_light_groups[PG_MAX_CONFIGURATIONS] = {0};
vector<Light> pg_Lights[PG_MAX_CONFIGURATIONS];
vector<LightGroup> pg_light_groups[PG_MAX_CONFIGURATIONS];
// interface current light group
int pg_interface_light_group = 0;

// Create an unordered_map of three strings (that map to strings)
// corresponds to all the parameters associated with DMX light channels
// key constants are from enum pg_light_command_hashMap_IDs
std::unordered_map<int, std::string> pg_light_param_hashMap = {
	{ _dimmer, "dimmer" },
	{ _strobe, "strobe" },
	{ _zoom, "zoom" },
	{ _pan, "pan" },
	{ _tilt, "tilt" },
	{ _hue, "hue" },
	{ _red, "red" },
	{ _green, "green" },
	{ _blue, "blue" },
	{ _grey, "grey" },
	{ _palette_color, "palette_color" }
};
// same map but from strings to enum values
std::unordered_map<std::string, int> pg_inverse_light_param_hashMap = {
};
// submap of the parameters which can be looped
std::unordered_map<int, std::string> pg_light_loop_param_hashMap = {
	{ _zoom, "zoom" },
	{ _pan, "pan" },
	{ _tilt, "tilt" }
};

// pen palettes colors
vector<Palette> pg_Palettes[PG_MAX_CONFIGURATIONS];
// photo albums
std::string pg_ImageDirectory[PG_MAX_CONFIGURATIONS] = { "" };
// short video clip albums
string pg_ClipDirectory[PG_MAX_CONFIGURATIONS] = { "" };
int pg_NbClipAlbums[PG_MAX_CONFIGURATIONS] = {0};
int clip_image_width[PG_MAX_CONFIGURATIONS] = {0};
int clip_image_height[PG_MAX_CONFIGURATIONS] = {0};
int clip_crop_width[PG_MAX_CONFIGURATIONS] = {0};
int clip_crop_height[PG_MAX_CONFIGURATIONS] = {0};
int clip_max_length[PG_MAX_CONFIGURATIONS] = {0};
// pen brushes
string pen_brushes_fileName[PG_MAX_CONFIGURATIONS] = { "" };
int nb_pen_brushes[PG_MAX_CONFIGURATIONS] = {0};
// textures with multiple layers
int nb_layers_master_mask[PG_MAX_CONFIGURATIONS] = {0};
VideoCapture  pg_webCam_capture;
vector <VideoCapture> pg_IPCam_capture;
vector<String> pg_IPCam_capture_address;
vector<webCam> pg_webCams;
int pg_current_active_cameraNo = INT_MIN;
bool pg_initializedWebcam = false;
bool pg_cameraCaptureIsOn = false;
VideoCapture  pg_movie_capture;
Mat pg_movie_frame;

media_status pg_movie_status = media_status();
bool is_movieLoading = false;
bool is_movieAtEnd = false;

int initialSecondBGCapture = 0;

// clip track
std::vector<clip_track> pg_clip_tracks[PG_MAX_CONFIGURATIONS];
// clip status (left and right)
clip_status pg_clip_status[_clipLR] = { clip_status(_clipLeft), clip_status(_clipRight)};
// index of top clip 
int rankOfTopClip = 0;
// indices of the currently played clips
int playing_clipNoLeft = -1;
int playing_secondClipNoLeft = -1;
int playing_clipNoRight = -1;
int playing_secondClipNoRight = -1;

float fx_dry_wet = 0.f;

bool secondInitialBGCapture = false;
bool initialBGCapture = false;

const char *TextureEncodingString[EmptyTextureEncoding + 1] = { "jpeg", "png", "pnga", "png_gray", "pnga_gray", "rgb", "raw", "emptyimagetype" };

/*
threadData pDataWriteJpg;
threadData pDataWriteSvg;
threadData pDataWritePng;
*/

///////////////////////////////////////////////////////
// MOVIE STATUS MANAGEMENT WHILE STREAMING
media_status::media_status() {
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_movieCaptFreq]) {
		nbFramesLeft = 0;
		initialTime = 0.;
		initialMediaNbFrames = 0;

		pg_movie_sound_onset = false;
		pg_movie_sound_peak = false;
		currentMovieSoundPeakIndex = 0;
		if (currentlyPlaying_movieNo >= 0) {
			nbMovieSoundPeakIndex = pg_VideoTracks[pg_current_configuration_rank][currentlyPlaying_movieNo].videoSoundtrackPeaks.size();
		}
		else {
			nbMovieSoundPeakIndex = 0;
		}
		currentMovieSoundOnsetIndex = 0;
		if (currentlyPlaying_movieNo >= 0) {
			nbMovieSoundOnsetIndex = pg_VideoTracks[pg_current_configuration_rank][currentlyPlaying_movieNo].videoSoundtrackOnsets.size();
		}
		else {
			nbMovieSoundOnsetIndex = 0;
		}
	}
}
// number frames left until the end from current frame
const int media_status::get_nbFramesLeft() {
	return nbFramesLeft;
}
// number of frames read from the beginning of the movie
const int media_status::get_nbFramesRead() {
	return initialMediaNbFrames - nbFramesLeft;
}
// current time when movie started
const double media_status::get_initialTime() {
	return initialTime;
}
// initial total number of frames in the movie
const int media_status::get_initialNbFrames() {
	return initialMediaNbFrames;
}
// sets the movie at a position from beginning
const void media_status::set_position(int nb_frames_from_beginning) {
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_movieCaptFreq]) {
		if (nb_frames_from_beginning <= 0) {
			reset_movie(initialMediaNbFrames);
			return;
		}
		if (nb_frames_from_beginning >= initialMediaNbFrames) {
			nb_frames_from_beginning = initialMediaNbFrames - 1;
		}
		nbFramesLeft = initialMediaNbFrames - nb_frames_from_beginning;
		initialTime = pg_CurrentClockTime - (nb_frames_from_beginning / movieCaptFreq);
		//printf("set position frames from start %d frames left %d initialTime %.5f\n", nb_frames_from_beginning, nbFramesLeft, initialTime);

		currentMovieSoundPeakIndex = 0;
		currentMovieSoundOnsetIndex = 0;
		// sets the position for the onsets and peaks as well
		updateMoviePeakOrOnset();
	}
}
// changes the movie initial time if capture frequency changes
const void media_status::reset_initialTime(float new_captFreq) {
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_movieCaptFreq]) {
		if (movieCaptFreq > 0) {
			int nb_frames_from_beginning = initialMediaNbFrames - nbFramesLeft;
			double initialTimeBeforeChange = initialTime;
			initialTime = pg_CurrentClockTime - (nb_frames_from_beginning / new_captFreq);
			//printf("Initial time before %.2f after %.2f\n", initialTimeBeforeChange, initialTime);
		}
	}
}

const void media_status::updateMoviePeakOrOnset() {
	double timeFromBeginning = pg_CurrentClockTime - initialTime;
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_movieCaptFreq]) {
		if (int(pg_VideoTracks[pg_current_configuration_rank].size()) > currentlyPlaying_movieNo
			&& pg_VideoTracks[pg_current_configuration_rank][currentlyPlaying_movieNo].videoSoundtrackPeaksFileName != "") {
			updatePeakOrOnset(timeFromBeginning, &pg_VideoTracks[pg_current_configuration_rank][currentlyPlaying_movieNo].videoSoundtrackPeaks, &pg_VideoTracks[pg_current_configuration_rank][currentlyPlaying_movieNo].videoSoundtrackOnsets,
				&pg_movie_sound_peak, &pg_movie_sound_onset, nbMovieSoundPeakIndex, nbMovieSoundOnsetIndex, &currentMovieSoundPeakIndex, &currentMovieSoundOnsetIndex);
		}
	}
}
// reads one frame and updates the remaining number of frames in the movie
const void media_status::read_one_frame() {
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_movieCaptFreq]) {
		nbFramesLeft--;
		updateMoviePeakOrOnset();
	}
}
// (re)starts the movie from begginning with its total number of frames
const void media_status::reset_movie(int nbTotFramesLeft) {
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_movieCaptFreq]) {
		// printf("movie restarted\n");
		nbFramesLeft = nbTotFramesLeft;
		initialMediaNbFrames = nbFramesLeft;
		initialTime = pg_CurrentClockTime;

		currentMovieSoundPeakIndex = 0;
		if (currentlyPlaying_movieNo >= 0) {
			nbMovieSoundPeakIndex = pg_VideoTracks[pg_current_configuration_rank][currentlyPlaying_movieNo].videoSoundtrackPeaks.size();
		}
		else {
			nbMovieSoundPeakIndex = 0;
		}
		currentMovieSoundOnsetIndex = 0;
		if (currentlyPlaying_movieNo >= 0) {
			nbMovieSoundOnsetIndex = pg_VideoTracks[pg_current_configuration_rank][currentlyPlaying_movieNo].videoSoundtrackOnsets.size();
		}
		else {
			nbMovieSoundOnsetIndex = 0;
		}
	}
}
media_status::~media_status() {

}

// checks whether a peak or an onset are passed or closer than one frame
void updatePeakOrOnset(double timeFromBeginning, vector<float>* peaks, vector<float>* onsets,
	bool* is_new_peak, bool* is_new_onset, int nbPeaks, int nbOnsets, int* curPeakIndex, int* curOnsetIndex) {
	*is_new_peak = false;
	//printf("curPeakIndex %d\n", *curPeakIndex);
	if (*curPeakIndex < nbPeaks - 1) {
		float nextPeak = (*peaks)[*curPeakIndex + 1];
		while (nextPeak < timeFromBeginning || nextPeak - timeFromBeginning < 1 / 70.) {
			*is_new_peak = true;
			//printf("peak next %.5f time from begin %.5f index %d\n", nextPeak, timeFromBeginning, *curPeakIndex);
			(*curPeakIndex)++;
			if (*curPeakIndex < nbPeaks - 1) {
				nextPeak = (*peaks)[*curPeakIndex + 1];
			}
			else {
				break;
			}
		}
	}
	*is_new_onset = false;
	if (*curOnsetIndex < nbOnsets - 1) {
		float nextOnset = (*onsets)[*curOnsetIndex + 1];
		while (nextOnset < timeFromBeginning || nextOnset - timeFromBeginning < 1 / 70.) {
			*is_new_onset = true;
			//printf("onset next %.5f time from begin %.5f\n", nextOnset, timeFromBeginning);
			(*curOnsetIndex)++;
			if (*curOnsetIndex < nbOnsets - 1) {
				nextOnset = (*onsets)[*curOnsetIndex + 1];
			}
			else {
				break;
			}
		}
	}
}

///////////////////////////////////////////////////////
// // CLIP TRACKS
clip_track::clip_track(int ind_clip, int nbFrames, string name) {
	clipNo = ind_clip;
	initialNbFrames = nbFrames;
	for (int ind = 0; ind < _NbMaxCues; ind++) {
		clip_cue[ind] = -1;
	}
	clip_name = name;
}
	// catches a cue
int clip_track::set_cue_onOff(int indCue, int frameNo) {
	if (indCue < _NbMaxCues) {
		printf("onOff indCue %d cue value %d\n", indCue, clip_cue[indCue]);
		if (clip_cue[indCue] < 0 && frameNo < initialNbFrames) {
			clip_cue[indCue] = frameNo;
		}
		else {
			clip_cue[indCue] = -1;
		}
		return clip_cue[indCue];
	}
	return -1;
}
// catches a cue
void clip_track::set_cue_off(int indCue) {
	if (indCue < _NbMaxCues) {
		//printf("Reset indCue %d cue value %d\n", indCue, -1);
		clip_cue[indCue] = -1;
	}
}
// catches a cue
void clip_track::set_cue(int indCue, int indFrame) {
	if (indCue < _NbMaxCues && indFrame < initialNbFrames) {
		clip_cue[indCue] = indFrame;
	}
}
// returns a cue
int clip_track::get_cue(int indCue) {
	if (indCue < _NbMaxCues) {
		return clip_cue[indCue];
	}
	else {
		return -1;
	}
}
// initial total number of frames in the clip
int clip_track::get_initialNbFrames() {
	return initialNbFrames;
}
// clip name
string clip_track::get_name() {
	return clip_name;
}

///////////////////////////////////////////////////////
// CLIP STATUS MANAGEMENT WHILE STREAMING
clip_status::clip_status(int clipSide) {
	for (int ind = 0; ind < PG_NB_PARALLEL_CLIPS; ind++) {
		currentlyPlaying_clipNo[ind] = -1;
		lastFrame[ind] = 0;
		clip_autoplay[ind] = true;
		clip_play[ind] = true;
		lastPlayedFrameTime[ind] = pg_CurrentClockTime;
		clip_level[ind] = 1.0;
		clip_r_channel_level[ind] = 1.0f;
		clip_g_channel_level[ind] = 1.0f;
		clip_b_channel_level[ind] = 1.0f;
	}
	sideLR = pg_clip_LR(clipSide);
	cumulated_scratch_offset = 0.;
	currentFPS = 0;
	last_scratch_angle = 0.f;
	last_scratch_angle_change = 0.;
	last_nudge_angle = 0.f;
	last_nudge_angle_change = 0.;
	is_scratch_pressed = false;
	jump_frame_freq = 0;
}
// last frame index
int clip_status::get_lastFrame(int indClipRank) {
	return lastFrame[indClipRank];
}
// last frame exact time
double clip_status::get_lastPlayedFrameTime(int indClipRank) {
	return lastPlayedFrameTime[indClipRank];
}
void clip_status::set_lastPlayedFrameTime(int indClipRank, double a_time) {
	lastPlayedFrameTime[indClipRank] = a_time;
}

// next frame index and sets it as last frame together 
// with memory of exact time of current frame (including the delay to the current time)
int clip_status::get_nextFrameAndUpdate(int indClipRank) {
	// if minimally one frame has elapsed
	double timeLapseSinceLastFrame = (pg_CurrentClockTime - lastPlayedFrameTime[indClipRank]);
	// clip is not updated if the speed is null or if the scratching plate is pressed 
	if (currentFPS != 0 && !is_scratch_pressed) {
		if (fabs(timeLapseSinceLastFrame) >= fabs(1. / currentFPS)) {
			int lastPlayedFrame = lastFrame[indClipRank];
			int nb_elapsedFrames = int(floor(fabs(timeLapseSinceLastFrame) * currentFPS));
			lastFrame[indClipRank] = (lastFrame[indClipRank] + nb_elapsedFrames);
			int initialNbFrames = pg_clip_tracks[pg_current_configuration_rank][currentlyPlaying_clipNo[indClipRank]].get_initialNbFrames();
			while (lastFrame[indClipRank] >= initialNbFrames) {
				lastFrame[indClipRank] -= initialNbFrames;
			}
			while (lastFrame[indClipRank] < 0) {
				lastFrame[indClipRank] += initialNbFrames;
			}
			if (lastFrame[indClipRank] != lastPlayedFrame) {
				lastPlayedFrameTime[indClipRank] = lastPlayedFrameTime[indClipRank] + nb_elapsedFrames / currentFPS;
				if( fabs(pg_CurrentClockTime - lastPlayedFrameTime[indClipRank]) > 1. / currentFPS) {
					lastPlayedFrameTime[indClipRank] = pg_CurrentClockTime;
				}
			}
			if (jump_frame_freq > 0 && pg_FrameNo % max(1, (10 - jump_frame_freq)) == 0) {
				play_offset_position(indClipRank, 0.5);
			}
			if (initialNbFrames != 0) {
				if (indClipRank == 0) {
					if (sideLR == _clipLeft) {
						sprintf(AuxString, "/clip_percent_left %d", int(lastFrame[indClipRank] / float(initialNbFrames) * 100.f)); pg_send_message_udp((char*)"i", (char*)AuxString, (char*)"udp_TouchOSC_send");
					}
					else {
						sprintf(AuxString, "/clip_percent_right %d", int(lastFrame[indClipRank] / float(initialNbFrames) * 100.f)); pg_send_message_udp((char*)"i", (char*)AuxString, (char*)"udp_TouchOSC_send");
					}
				}
#if PG_NB_PARALLEL_CLIPS >= 2
				else if (indClipRank == 1) {
					if (sideLR == _clipLeft) {
						sprintf(AuxString, "/clip2_percent_left %d", int(lastFrame[indClipRank] / float(initialNbFrames) * 100.f)); pg_send_message_udp((char*)"i", (char*)AuxString, (char*)"udp_TouchOSC_send");
					}
					else {
						sprintf(AuxString, "/clip2_percent_right %d", int(lastFrame[indClipRank] / float(initialNbFrames) * 100.f)); pg_send_message_udp((char*)"i", (char*)AuxString, (char*)"udp_TouchOSC_send");
					}
				}
#endif
			}
		}
	}
	//if (sideLR == _clipLeft) {
	//	sprintf(AuxString, "/clip_frame_left %d", lastFrame[indClipRank]); pg_send_message_udp((char*)"i", (char*)AuxString, (char*)"udp_TouchOSC_send");
	//}
	return lastFrame[indClipRank];
}
// current speed according to nudging
double clip_status::get_currentFPS() {
	return currentFPS;
}
// position of the wheel in last interaction
float clip_status::get_last_scratch_angle(double *a_last_time) {
	last_scratch_angle_change = *a_last_time;
	return last_scratch_angle;
}
// position of the wheel in last interaction
float clip_status::get_last_nudge_angle(double* a_last_time) {
	last_nudge_angle_change = *a_last_time;
	return last_nudge_angle;
}
// scratch wheel is pressed: do not update clip automatically
bool clip_status::get_scratch_pressed() {
	return is_scratch_pressed;
}
// current speed according to nudging
void clip_status::set_currentFPS(double a_FPS) {
	currentFPS = a_FPS;
	if (sideLR == _clipRight) {
		sprintf(AuxString, "/clip_FPS_right %d", int(a_FPS)); pg_send_message_udp((char*)"s", (char*)AuxString, (char*)"udp_TouchOSC_send");
	}
	else {
		sprintf(AuxString, "/clip_FPS_left %d", int(a_FPS)); pg_send_message_udp((char*)"s", (char*)AuxString, (char*)"udp_TouchOSC_send");
	}
}
// current playing clip No
int clip_status::getCurrentlyPlaying_clipNo(int indClipRank) {
	return currentlyPlaying_clipNo[indClipRank];
}
// autoplay
bool clip_status::get_clip_autoplay(int indClipRank) {
	return clip_autoplay[indClipRank];
}
// autoplay
bool clip_status::get_clip_play(int indClipRank) {
	return clip_autoplay[indClipRank];
}
// current playing clip No
void clip_status::setCurrentlyPlaying_clipNo(int indClipRank, int indClip) {
	//printf("setCurrentlyPlaying_clipNo %d %d\n", indClipRank, indClip);
	currentlyPlaying_clipNo[indClipRank] = indClip;
	lastPlayedFrameTime[indClipRank] = pg_CurrentClockTime;
	lastFrame[indClipRank] = 0;
}
// position of the wheel in last interaction
void clip_status::set_last_scratch_angle(float a_angle, double a_time) {
	last_scratch_angle = a_angle;
	last_scratch_angle_change = a_time;
}
// scratch wheel is pressed: do not update clip automatically
void clip_status::set_scratch_pressed(bool a_value) {
	is_scratch_pressed = a_value;
	cumulated_scratch_offset = 0.;
}
// position of the wheel in last interaction
void clip_status::set_last_nudge_angle(float a_angle, double a_time) {
	last_nudge_angle = a_angle;
	last_nudge_angle_change = a_time;
}
// sets the clip at a position from beginning
void clip_status::set_position(int indClipRank, int nb_frames_from_beginning) {
	if (nb_frames_from_beginning <= 0 && lastFrame[indClipRank] != 0) {
		reset_clip(indClipRank);
		//printf("set position %d  initialTime %.5f\n", 0, pg_CurrentClockTime);
		return;
	}
	int initialNbFrames = pg_clip_tracks[pg_current_configuration_rank][currentlyPlaying_clipNo[indClipRank]].get_initialNbFrames();
	lastFrame[indClipRank] = nb_frames_from_beginning % initialNbFrames;
	lastPlayedFrameTime[indClipRank] = pg_CurrentClockTime;
	//printf("set position %d  initialTime %.5f\n", lastFrame[indClipRank], lastPlayedFrameTime[indClipRank]);
}
// offsets the movie position by a positive or negative time delta (play mode)
// used once for a big jump
void clip_status::play_offset_position(int indClipRank, double elapsedTime) {
	int nb_elapsedFrames = int(floor(elapsedTime * currentFPS));
	int initialNbFrames = pg_clip_tracks[pg_current_configuration_rank][currentlyPlaying_clipNo[indClipRank]].get_initialNbFrames();
	lastFrame[indClipRank] = max(min((lastFrame[indClipRank] + nb_elapsedFrames), initialNbFrames - 1), 0);
	lastPlayedFrameTime[indClipRank] = pg_CurrentClockTime;
}
// offsets the movie position by a positive or negative time delta (scratch mode)
// used several times for a small offset
void clip_status::scratch_offset_position(double timeLapse) {
	cumulated_scratch_offset += timeLapse;
	//printf("cumulated_scratch_offset %.3f\n", cumulated_scratch_offset);
	// positive number of frames, move forward
	if (cumulated_scratch_offset >= 0) {
		int nb_elapsedFrames = int(floor(cumulated_scratch_offset * currentFPS));
		//printf("positive lapse %d %.2f\n", nb_elapsedFrames, cumulated_scratch_offset * currentFPS);
		if (nb_elapsedFrames > 0) {
			for (int indClipRank = 0; indClipRank < PG_NB_PARALLEL_CLIPS; indClipRank++) {
				if (currentlyPlaying_clipNo[indClipRank] >= 0) {
					int initialNbFrames = pg_clip_tracks[pg_current_configuration_rank][currentlyPlaying_clipNo[indClipRank]].get_initialNbFrames();
					lastFrame[indClipRank] = (lastFrame[indClipRank] + nb_elapsedFrames) % initialNbFrames;
					lastPlayedFrameTime[indClipRank] = pg_CurrentClockTime;
				}
			}
			cumulated_scratch_offset = cumulated_scratch_offset - nb_elapsedFrames / currentFPS;
		}
	}
	// negative number of frames, move backward
	else {
		int nb_elapsedFrames = int(floor(cumulated_scratch_offset * currentFPS) + 1);
		//printf("negative lapse %d %.2f\n", nb_elapsedFrames, cumulated_scratch_offset * currentFPS);
		if (nb_elapsedFrames < 0) {
			for (int indClipRank = 0; indClipRank < PG_NB_PARALLEL_CLIPS; indClipRank++) {
				if (currentlyPlaying_clipNo[indClipRank] >= 0) {
					int initialNbFrames = pg_clip_tracks[pg_current_configuration_rank][currentlyPlaying_clipNo[indClipRank]].get_initialNbFrames();
					lastFrame[indClipRank] = (lastFrame[indClipRank] + nb_elapsedFrames + initialNbFrames) % initialNbFrames;
					lastPlayedFrameTime[indClipRank] = pg_CurrentClockTime;
				}
			}
			cumulated_scratch_offset = cumulated_scratch_offset - nb_elapsedFrames / currentFPS;
		}
	}
}
// (re)starts the clip from begginning with its total number of frames
void clip_status::reset_clip(int indClipRank) {
	// printf("clip restarted\n");
	if (currentlyPlaying_clipNo[indClipRank] >= 0 && currentlyPlaying_clipNo[indClipRank] < pg_nbClips[pg_current_configuration_rank]) {
		int initialNbFrames = pg_clip_tracks[pg_current_configuration_rank][currentlyPlaying_clipNo[indClipRank]].get_initialNbFrames();
		initialNbFrames = pg_nbCompressedClipFramesPerFolder[pg_current_configuration_rank][currentlyPlaying_clipNo[indClipRank]];
	}
	lastPlayedFrameTime[indClipRank] = pg_CurrentClockTime;
	lastFrame[indClipRank] = 0;
}
clip_status::~clip_status() {

}

///////////////////////////////////////////////////////
// CONVERSION OF FLOAT TRANSLATION INTO FRAME BASED INTEGER TRANSLATION
float frame_based_translation(float transl, int translation_rank) {
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_slow_track_translation]) {
		if (slow_track_translation) {
			if (transl == 0) {
				return 0.f;
			}
			else {
				float sign = (transl > 0 ? 1.f : -1.f);
				transl *= sign; // absolute value
				if (transl < 1) {
					if (pg_FrameNo % int(1 / transl) == 0) {
						return 1.f * sign;
					}
					else {
						return 0.f;
					}
				}
				else {
					return float(int(transl)) * sign;
				}
			}
		}
	}
	return 0.f;
}

///////////////////////////////////////////////////////
// GLUT draw function (from the viewpoint)

// the glut callback
// requires predrawing (drawing from the user to the root node)

// ------------------------------------------------------------ //
// --------------- DISPLAYS WINDOWS ONE AFTER ANOTHER --------- //
// ------------------------------------------------------------ //
void window_display(void) {

#if defined(PG_DEBUG)
	OutputDebugStringW(_T("\nstat 1\n"));
	_CrtMemCheckpoint(&s1);
#endif

	//printf("Maser %.2f\n", master);

	windowDisplayed = true;
	// printOglError(508);

	//////////////////////////////////////////////////
	//////////////////////////////////////////////////
	// SCENE UPDATE AND SHADER UNIFORM PARAMETERS UPDATE
	//////////////////////////////////////////////////
	// scenario update 
	pg_update_scenario();
	// recalculates pulsed colors and reads current paths
	pg_update_pulsed_colors_and_replay_paths(pg_CurrentClockTime);

	// ships uniform variables  printOglError(51);
	printOglError(50);
	pg_update_shader_uniforms();
	printOglError(51);

	// loads movie andor camera frames
	pg_update_clip_camera_and_movie_frame();

#if defined(PG_DEBUG)
	OutputDebugStringW(_T("stat 7\n"));
	_CrtMemCheckpoint(&s7);
	if (_CrtMemDifference(&sDiff, &s9, &s7))
		_CrtMemDumpStatistics(&sDiff);
#endif


	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_activeMeshes]) {
		// updates mesh animation
		for (unsigned int indMeshFile = 0; indMeshFile < pg_Meshes[pg_current_configuration_rank].size(); indMeshFile++) {
			// visibility
			bool visible = false;
#if defined(pg_Project_Etoiles)
			visible = Etoiles_mesh_guided_by_strokes(indMeshFile);
#elif defined(var_Caverne_Mesh_Profusion)
			visible = (indMeshFile < 7 && (activeMeshes & (1 << indMeshFile))) || (pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_CaverneActveMesh
				&& (pg_CurrentClockTime - pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_CaverneMeshBirthTime > pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_CaverneMeshWakeupTime)
				&& (pg_CurrentClockTime < pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_CaverneMeshDeathTime));
#else
			visible = (activeMeshes & (1 << indMeshFile));

			// visible mesh
			if (visible) {
				if (TabBones[indMeshFile] != NULL) {
					//printf("updage anim & bones mesh %d\n", indMeshFile);
					update_anim(indMeshFile);
					update_bones(indMeshFile);
				}
#if defined(var_Contact_mesh_rotation) && defined(var_Contact_mesh_translation_X) && defined(var_Contact_mesh_translation_Y) && defined(var_Contact_mesh_motion)
				if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_Contact_mesh_rotation]) {
					Contact_update_motion(indMeshFile);
				}
#endif
			}
		}
	}
#endif

	// Looks for next peak or onset for random diaporama (cf GrabIt! music piece)
#if defined(var_Argenteuil_onsetchange_diaporama)
	double timeFromBeginning = RealTime() - soundfile_data.sound_file_StartReadingTime;
	if (currentlyPlaying_trackNo >= 0 
		&& int(pg_SoundTracks[pg_current_configuration_rank].size()) > currentlyPlaying_trackNo) {
		//printf("size %d %d\n", pg_SoundTracks[currentlyPlaying_trackNo]->soundTrackSoundtrackPeaks.size(), pg_SoundTracks[currentlyPlaying_trackNo]->soundTrackSoundtrackPeaks.size());
		updatePeakOrOnset(timeFromBeginning, &pg_SoundTracks[pg_current_configuration_rank][currentlyPlaying_trackNo].soundtrackPeaks, 
			&pg_SoundTracks[pg_current_configuration_rank][currentlyPlaying_trackNo].soundtrackOnsets,
			&pg_track_sound_peak, &pg_track_sound_onset, nbTrackSoundPeakIndex[pg_current_configuration_rank], 
			nbTrackSoundOnsetIndex[pg_current_configuration_rank], &currentTrackSoundPeakIndex, &currentTrackSoundOnsetIndex);
		if (pg_track_sound_onset) {
			diaporama_random();
		}
	}
#endif

	// tests whether the soundtrack is finished reading
	if (pa_sound_data.pa_checkAudioStream()) {
		//printf("soundtrack current time %f\n", RealTime() - soundfile_data.sound_file_StartReadingTime);
	}

	//////////////////////////////////////////////////
	//////////////////////////////////////////////////
	// SCENE DISPLAY AND SHADER UNIFORM PARAMETERS UPDATE
	//////////////////////////////////////////////////
	// OpenGL initializations before redisplay
	OpenGLInit();
	// printOglError(509);

#if defined(PG_DEBUG)
	OutputDebugStringW(_T("stat 4\n"));
	_CrtMemCheckpoint(&s4);
	if (_CrtMemDifference(&sDiff, &s7, &s4))
		_CrtMemDumpStatistics(&sDiff);
#endif

	// proper scene redrawing
	pg_draw_scene( _Render );

#if defined(PG_DEBUG)
	OutputDebugStringW(_T("stat 2\n"));
	_CrtMemCheckpoint(&s2);
	if (_CrtMemDifference(&sDiff, &s4, &s2))
		_CrtMemDumpStatistics(&sDiff);
#endif

	//////////////////////////////////////////////////
	//////////////////////////////////////////////////
	// ONE-FRAME PARAMETERS RESET OR UPDATE (AUTOMATIC, NON SCENARIO-BASED UPDATES)
	//////////////////////////////////////////////////
	// resets to 0 the variables that are only true for one frame such as flashes, resets...
	pg_automatic_var_reset_or_update();

	// flushes OpenGL commands
	glFlush();

	// displays new frame
	glutSwapBuffers();

#if defined(PG_DEBUG)
	OutputDebugStringW(_T("stat 3\n"));
	_CrtMemCheckpoint(&s3);
	if (_CrtMemDifference(&sDiff, &s2, &s3))
		_CrtMemDumpStatistics(&sDiff);
#endif

	// ------------------------------------------------------------ //
	// --------------- FRAME/SUBFRAME GRABBING -------------------- //

	// ---------------- frame by frame output --------------------- //
	// Svg screen shots
	// printf("Draw Svg\n" );
	if (take_snapshots && pg_Svg_Capture_param.outputSvg) {
		// frame count based output
		if (pg_Svg_Capture_param.stepSvgInFrames > 0) {
			if (pg_FrameNo % pg_Svg_Capture_param.stepSvgInFrames == 0
				&& pg_FrameNo / pg_Svg_Capture_param.stepSvgInFrames >= pg_Svg_Capture_param.beginSvg &&
				pg_FrameNo / pg_Svg_Capture_param.stepSvgInFrames <= pg_Svg_Capture_param.endSvg) {
				pg_draw_scene( _Svg );
			}
		}
		else if (pg_Svg_Capture_param.stepSvgInSeconds > 0) {
			if (pg_Svg_Capture_param.nextSvgCapture < 0) {
				pg_Svg_Capture_param.nextSvgCapture = pg_CurrentClockTime;
			}
			if (pg_CurrentClockTime >= pg_Svg_Capture_param.nextSvgCapture
				&& pg_CurrentClockTime >= pg_Svg_Capture_param.beginSvg &&
				pg_CurrentClockTime <= pg_Svg_Capture_param.endSvg) {
				pg_Svg_Capture_param.nextSvgCapture = max(pg_Svg_Capture_param.nextSvgCapture + pg_Svg_Capture_param.stepSvgInSeconds, pg_CurrentClockTime);
				pg_draw_scene( _Svg );
			}
		}
	}

	// ---------------- frame by frame output --------------------- //
	// Png screen shots
	// printf("Draw Png\n" );
	if (take_snapshots && pg_Png_Capture_param.outputPng ) {
		// frame count based output
		if (pg_Png_Capture_param.stepPngInFrames > 0) {
			if (pg_FrameNo % pg_Png_Capture_param.stepPngInFrames == 0
				&& pg_FrameNo / pg_Png_Capture_param.stepPngInFrames >= pg_Png_Capture_param.beginPng &&
				pg_FrameNo / pg_Png_Capture_param.stepPngInFrames <= pg_Png_Capture_param.endPng) {
				pg_draw_scene( _Png );
			}
		}
		else if (pg_Png_Capture_param.stepPngInSeconds > 0) {
			if (pg_Png_Capture_param.nextPngCapture < 0) {
				pg_Png_Capture_param.nextPngCapture = pg_CurrentClockTime;
			}
			if (pg_CurrentClockTime >= pg_Png_Capture_param.nextPngCapture
				&& pg_CurrentClockTime >= pg_Png_Capture_param.beginPng &&
				pg_CurrentClockTime <= pg_Png_Capture_param.endPng) {
				pg_Png_Capture_param.nextPngCapture = max(pg_Png_Capture_param.nextPngCapture + pg_Png_Capture_param.stepPngInSeconds, pg_CurrentClockTime);
				pg_draw_scene( _Png );
			}
		}
	}

	// ---------------- frame by frame output --------------------- //
	// Jpg screen shots
	// printf("Draw Jpg\n"  );
	if (take_snapshots && pg_Jpg_Capture_param.outputJpg) {
		// frame count based output
		if (pg_Jpg_Capture_param.stepJpgInFrames > 0) {
			if (pg_FrameNo % pg_Jpg_Capture_param.stepJpgInFrames == 0
				&& pg_FrameNo / pg_Jpg_Capture_param.stepJpgInFrames >= pg_Jpg_Capture_param.beginJpg &&
				pg_FrameNo / pg_Jpg_Capture_param.stepJpgInFrames <= pg_Jpg_Capture_param.endJpg) {
				pg_draw_scene( _Jpg );
			}
		}
		else if (pg_Jpg_Capture_param.stepJpgInSeconds > 0) {
			if (pg_Jpg_Capture_param.nextJpgCapture < 0) {
				pg_Jpg_Capture_param.nextJpgCapture = pg_CurrentClockTime;
			}
			if (pg_CurrentClockTime >= pg_Jpg_Capture_param.nextJpgCapture
				&& pg_CurrentClockTime >= pg_Jpg_Capture_param.beginJpg &&
				pg_CurrentClockTime <= pg_Jpg_Capture_param.endJpg) {
				pg_Jpg_Capture_param.nextJpgCapture = max(pg_Jpg_Capture_param.nextJpgCapture + pg_Jpg_Capture_param.stepJpgInSeconds, pg_CurrentClockTime);
				pg_draw_scene( _Jpg );
				//printf("nextJpgCapture %.2f pg_CurrentClockTime %.2f\n", nextJpgCapture, pg_CurrentClockTime);
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
// SENSOR READING AND SAMPLE CHOICE/
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

//// sample choice
//// current sample choice
//int sample_choice[ PG_NB_SENSORS];
//// all possible sensor layouts
//int sensor_sample_setUps[ PG_NB_MAX_SAMPLE_SETUPS][ PG_NB_SENSORS ] =
//  {{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25},
//   {26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50},
//   {51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75}};
//// groups of samples for aliasing with additive samples

void readSensors(void) {
	bool sensorOn[PG_NB_SENSORS] = { false };

	bool sampleOn[PG_NB_MAX_SAMPLE_SETUPS * PG_NB_SENSORS] = { false };
	int sampleToSensorPointer[PG_NB_MAX_SAMPLE_SETUPS * PG_NB_SENSORS] = { 0 };

	GLubyte pixelColor[3 * PG_NB_SENSORS] = { 0 };

	if (!pg_FullScenarioActiveVars[pg_current_configuration_rank][_sensor_layout]) {
		return;
	}
	
	// marks all the samples as unread
	for (int indSample = 0; indSample < PG_NB_MAX_SAMPLE_SETUPS * PG_NB_SENSORS; indSample++) {
		sampleOn[indSample] = false;
		sampleToSensorPointer[indSample] = -1;
	}

	glBindFramebuffer(GL_READ_FRAMEBUFFER, pg_FBO_Mixing_capturedFB_prec); // drawing memory on odd and even frames for echo and sensors	
	pg_bindFBOTextures(pg_FBO_Mixing_capturedFB_prec, pg_FBO_Mixing_capturedFB_prec_texID + (pg_FrameNo % 2), 1, false, 0);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	// sensor readback
	//printf("sensor on ");
	//printf("SENSOR position %d %d %d %d %d %d %d %d\n", (int)sensorPositions[0], (int)sensorPositions[1],
	//	(int)sensorPositions[3], (int)sensorPositions[4], (int)sensorPositions[6], (int)sensorPositions[7], (int)sensorPositions[9], (int)sensorPositions[10]);
	for (int indSens = 0; indSens < PG_NB_SENSORS; indSens++) {
		if (sensor_onOff[indSens]) {
			//printf("position %d %d\n", (int)sensorPositions[3 * indSens],
			//	(int)(sensorPositions[3 * indSens + 1]));
			glReadPixels((int)sensorPositions[3 * indSens],
				(int)(sensorPositions[3 * indSens + 1]),
				1, 1,
				GL_RGB, GL_UNSIGNED_BYTE, pixelColor + 3 * indSens);
			sensorLevel[indSens] =
				(pixelColor[3 * indSens] + pixelColor[3 * indSens + 1] + pixelColor[3 * indSens + 2]) / (255.f * 3.f);
			sensorOn[indSens] = (sensorLevel[indSens] > 0.0f);
			if (sensorOn[indSens]) {
				sampleOn[sample_choice[indSens]] = true;
				sampleToSensorPointer[sample_choice[indSens]] = indSens;
				//printf("ON#%d ",indSens);
			}
		}
		else {
			sensorLevel[indSens] = 0.0f;
			sensorOn[indSens] = false;
		}
	}
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	//printf("\n");
	printOglError(691);

	///////////////////////////////////////////////////////////////
	// SENSOR-BASED SOUND IN RENOISE OR PORPHYROGRAPH SOUND
	// message value
	std::string float_string;
	std::string int_string;

	///////////////////////////////////////////////
	// TRIGGERING THE SAMPLE
	for (int indSens = 0; indSens < PG_NB_SENSORS; indSens++) {
		if (sensorOn[indSens] // active sensor
			&& sensorLevel[indSens] > 0 // non black pixel
			&& sample_play_start[sample_choice[indSens]] < 0 // not currently playing
			) {

#if defined(PG_RENOISE)
			// Renoise message format && message posting
			sprintf(AuxString, "/renoise/song/track/%d/prefx_volume %.2f", sample_choice[indSens] + 1, sensorLevel[indSens] * sensor_vol);
			pg_send_message_udp((char*)"f", AuxString, (char*)"udp_RN_send");

			//printf("RENOISE send %s %f %f\n", AuxString, sensorLevel[indSens], sensor_vol);
#endif

			// starts the clock for stopping the sample play after a certain time
			sample_play_start[sample_choice[indSens]] = pg_CurrentClockTime;
			sample_play_volume[sample_choice[indSens]] = sensorLevel[indSens];
			// printf("lights sensor #%d\n", indSens);
		}

		// the sample has been triggered and has not yet reached 90% of its playing duration
		if (sample_play_start[sample_choice[indSens]] > 0.0
			&& pg_CurrentClockTime - sample_play_start[sample_choice[indSens]] <= 0.9 * BEAT_DURATION) {
			// set the value to the initial value until 0.9 so that there is one visual feedback per loop
			sensorLevel[indSens] = (sample_play_volume[sample_choice[indSens]]
				* float((pg_CurrentClockTime - sample_play_start[sample_choice[indSens]]) / BEAT_DURATION));
		}
	}
	printOglError(689);

	///////////////////////////////////////////////
	// MANAGING THE SAMPLE SEVEL
	for (int indSetup = 0; indSetup < PG_NB_MAX_SAMPLE_SETUPS; indSetup++) {
		for (int indSens = 0; indSens < PG_NB_SENSORS; indSens++) {
			int indSample = sensor_sample_setUps[indSetup][indSens];
			if (sample_play_start[indSample] > 0
				&& pg_CurrentClockTime - sample_play_start[indSample] > BEAT_DURATION) {
#if defined(PG_RENOISE)
				// Renoise message format && message posting
				sprintf(AuxString, "/renoise/song/track/%d/prefx_volume %.2f", indSample + 1, 0.f);
				pg_send_message_udp((char*)"f", AuxString, (char*)"udp_RN_send");
#endif

				// resets the clock for replaying the sample if sensor triggered again
				sample_play_start[indSample] = -1.f;
			}
		}
	}
	printOglError(690);

	// message trace
	//std::cout << "format: " << format << "\n";
	//std::cout << "msg: " << message << "\n";
}

#if defined(pg_Project_CAaudio)
class GreyNote {
public:
	float grey;
	int note;
};
void playChord() {
	// line reading
	GLubyte *rowColor = new GLubyte[3 * 1024];
	GreyNote *rowGrey = new GreyNote[1024];
	// reads the 2nd horizontal line (from the top)
	glReadPixels(0, 766, 1024, 1,
				 GL_RGB, GL_UNSIGNED_BYTE, rowColor);
	GLubyte *ptr = rowColor;
	std::string float_string;
	std::string int_string;
	std::string message;
	// message format
	int nbGrey = 0;
	//printf("Colors\n");
	for (int indPixel = 0; indPixel < 1024; indPixel++) {
		GLubyte r, g, b;
		r = *(ptr++);
		g = *(ptr++);
		b = *(ptr++);
		//if (indPixel < 100) {
		//	printf("(%d,%d,%d) ", int(r), int(g), int(b));
		//}
		float greyVal = (float(r) + float(g) + float(b)) / (255.f * 3.f);
		if (greyVal > 0) {
			rowGrey[nbGrey].note = indPixel;
			rowGrey[nbGrey].grey = greyVal;
			nbGrey++;
		}
	}
	//printf("\n");

	// no non null note
	if (nbGrey <= 0) {
		//printf("No note\n");
		delete rowColor;
		return;
	}

	// less than 10 notes play them all
	if (nbGrey < 10) {
		std::string format = "";
		for (int ind = 0; ind < 20; ind++) {
			format += "f";
		}
		//printf("Less than 10 notes\n");
		//for (int indGreyNote = 0; indGreyNote < nbGrey; indGreyNote++) {
		//	printf("%d %.2f / ", rowGrey[nbGrey].note, rowGrey[nbGrey].grey);
		//}
		//printf("\n");

		message = "/chord ";
		// non null notes
		for (int indGreyNote = 0; indGreyNote < nbGrey; indGreyNote++) {
			// MESSAGE CONSTRUCTION
			// the note is converted into a frequency
			int_string
				= std::to_string(static_cast<long double>(440 * pow(pow(2., 1. / 12.),
				(rowGrey[indGreyNote].note - 512) / 8)));
			message += int_string + " ";
			// the intensity is turned into decibels
			float_string = std::to_string(static_cast<long double>(rowGrey[indGreyNote].grey * 100.f));
			// float_str.resize(4);
			message += float_string;
			if (indGreyNote < 10 - 1) {
				message += float_string + " ";
			}
			else {
				message += float_string;
			}
		}
		// remaining nul notes to fill up to 10 notes in a chord
		for (int indGreyNote = nbGrey; indGreyNote < 10; indGreyNote++) {
			// MESSAGE CONSTRUCTION
			// the note is outside the scale
			int_string = std::to_string(-1);
			message += int_string + " ";
			// the intensity is null
			float_string = std::to_string(static_cast<long double>(0.f));
			// float_str.resize(4);
			if (indGreyNote < 10 - 1) {
				message += float_string + " ";
			}
			else {
				message += float_string;
			}
		}
		// message posting
		pg_send_message_udp((char *)format.c_str(), (char *)message.c_str(), (char *)"udp_PD_send");
	}
	else {
		message = "/chord ";
		// notes are grouped by package of same size into metaNotes with a metaIntensity
		// number minimal of notes per metanote
		int nbNotesMinPerMetaNote = nbGrey / 10; // integer part
		// some metaNotes are built from 1 additional note
		int nbMetaNotesWithNbNMPMPlus1 = (nbGrey * 10) % 10; // first decimal
		// rank of the non null note
		int indGreyNote = 0;
		// rank of the note inside the metaNote group
		int indNotesMinPerMetaNote = 0;
		// average note value
		float metaNote[10] = { 0.f };
		// average intensity values
		float metaGrey[10] = { 0.f };
		// builds the metaNotes from note groups
		for (int indMetaNote = 0; indMetaNote < 10; indMetaNote++) {
			// average note value
			metaNote[indMetaNote] = 0.f;
			// average intensity values
			metaGrey[indMetaNote] = 0.f;

			// METANOTE SIZE
			// number of notes building the metaNote
			int nbNotes
				= (indMetaNote < nbMetaNotesWithNbNMPMPlus1 ? nbNotesMinPerMetaNote + 1 : nbNotesMinPerMetaNote);
			// the last metaNote uses the remaining non null notes
			if (indMetaNote == 10 - 1) {
				nbNotes = nbGrey - indGreyNote;
			}

			// METANOTE computation
			// average note and intensity value
			for (int indLocalGreyNote = 0; indLocalGreyNote < nbNotes && indGreyNote < nbGrey; indLocalGreyNote++) {
				metaGrey[indMetaNote] += rowGrey[indGreyNote].grey;
				metaNote[indMetaNote] += rowGrey[indGreyNote].note * rowGrey[indGreyNote].grey;
				indGreyNote++;
			}

			// AVERAGE VALUES
			// the note is corrected by the sum of weights (intensities) to turn it into a barycenter
			if (metaGrey > 0) {
				metaNote[indMetaNote] /= metaGrey[indMetaNote];
			}
			else {
				metaNote[indMetaNote] = -1;
			}
			if (nbNotes > 0) {
				metaGrey[indMetaNote] /= nbNotes;
			}

			// MESSAGE CONSTRUCTION
			// the note is converted into a frequency
			int_string
				= std::to_string(static_cast<long double>(440 * pow(pow(2., 1. / 12.),
				(int(round(metaNote[indMetaNote])) - 512) / 8)));
			message += int_string + " ";
			// the intensity is turned into decibels
			float_string = std::to_string(static_cast<long double>(metaGrey[indMetaNote] * 100.f));
			// float_str.resize(4);
			if (indMetaNote < 10 - 1) {
				message += float_string + " ";
			}
			else {
				message += float_string;
			}
		}
		std::string format = "";
		for (int ind = 0; ind < 20; ind++) {
			format += "f";
		}
		// message posting
		pg_send_message_udp((char *)format.c_str(), (char *)message.c_str(), (char *)"udp_PD_send");
	}
	delete rowColor;
	delete rowGrey;
}
#endif
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
// SCENE VARIABLE UPDATES
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
// restores variables to 0 or false so that
// they do not stay on more than one frame (flashes, initialization...)
void pg_automatic_var_reset_or_update(void) {
	///////////////////////////////////////////////////////////////////////
	// flash reset: restores flash to 0 so that
	// it does not stay on more than one frame 
	for (int indtrack = 0; indtrack < PG_NB_TRACKS; indtrack++) {
		if (flashTrkCA_weights_duration[indtrack] > 1) {
			flashTrkCA_weights_duration[indtrack]--;
		}
		else {
			flashTrkCA_weights[indtrack] = 0;
			flashTrkCA_weights_duration[indtrack] = 0;
		}
		if (flashTrkBG_weights_duration[indtrack] > 1) {
			flashTrkBG_weights_duration[indtrack]--;
		}
		else {
			flashTrkBG_weights[indtrack] = 0;
			flashTrkBG_weights_duration[indtrack] = 0;
		}
		if (flashTrkPart_weights_duration[indtrack] > 1) {
			flashTrkPart_weights_duration[indtrack]--;
		}
		else {
			flashTrkPart_weights[indtrack] = 0;
			flashTrkPart_weights_duration[indtrack] = 0;
		}
	}

	if (flashPixel > 0) {
		flashPixel -= 1;
	}
	if (flashCABG_weight_duration > 1) {
		flashCABG_weight_duration--;
	}
	else {
		flashCABG_weight_duration = 0;
		flashCABG_weight = 0;
	}
	if (flashCAPart_weight_duration > 1) {
		flashCAPart_weight_duration--;
	}
	else {
		flashCAPart_weight_duration = 0;
		flashCAPart_weight = 0;
	}
	if (flashPartBG_weight_duration > 1) {
		flashPartBG_weight_duration--;
	}
	else {
		flashPartBG_weight_duration = 0;
		flashPartBG_weight = 0;
	}
	if (flashPartCA_weight_duration > 1) {
		flashPartCA_weight_duration--;
	}
	else {
		flashPartCA_weight_duration = 0;
		flashPartCA_weight = 0;
	}

#if defined(var_master_scale) && defined(var_master_offsetX) && defined(var_photo_gamma) && defined(var_photo_satur) && defined(var_photo_threshold)
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_master_scale]
		&& pg_FullScenarioActiveVars[pg_current_configuration_rank][_master_offsetX]
		&& pg_FullScenarioActiveVars[pg_current_configuration_rank][_photo_gamma]
		&& pg_FullScenarioActiveVars[pg_current_configuration_rank][_photo_satur]
		&& pg_FullScenarioActiveVars[pg_current_configuration_rank][_photo_threshold]) {
		if (flashMaster > 0) {
			flashMaster--;
			// end of flash return to dark
			if (pg_CurrentScene && flashMaster == 0) {
				master = 0.f;
				*((float*)pg_FullScenarioVarPointers[_master]) = master;
				master_scale = float(pg_CurrentScene->scene_initial_parameters[_master_scale].val_num);
				*((float*)pg_FullScenarioVarPointers[_master_scale]) = master_scale;
				master_offsetX = float(pg_CurrentScene->scene_initial_parameters[_master_offsetX].val_num);
				*((float*)pg_FullScenarioVarPointers[_master_offsetX]) = master_offsetX;
				master_offsetY = float(pg_CurrentScene->scene_initial_parameters[_master_offsetY].val_num);
				*((float*)pg_FullScenarioVarPointers[_master_offsetY]) = master_offsetY;
				photo_threshold = float(pg_CurrentScene->scene_initial_parameters[_photo_threshold].val_num);
				*((float*)pg_FullScenarioVarPointers[_photo_threshold]) = photo_threshold;
				photo_gamma = float(pg_CurrentScene->scene_initial_parameters[_photo_gamma].val_num);
				*((float*)pg_FullScenarioVarPointers[_photo_gamma]) = photo_gamma;
				photo_satur = float(pg_CurrentScene->scene_initial_parameters[_photo_satur].val_num);
				*((float*)pg_FullScenarioVarPointers[_photo_satur]) = photo_satur;
				invertAllLayers = bool(pg_CurrentScene->scene_initial_parameters[_invertAllLayers].val_num);
				*((bool*)pg_FullScenarioVarPointers[_invertAllLayers]) = invertAllLayers;
			}
		}
	}
#endif

	////////////////////////////
	// flash camera reset
	if (flashCameraTrk_weight > 0.0f) {
		if (flashCameraTrk_weight - flashCameraTrk_decay > 0) {
			flashCameraTrk_weight -= flashCameraTrk_decay;
			//printf("flash camera weight %.3f\n", flashCameraTrk_weight);
		}
		else {
			flashCameraTrk_weight = 0.0f;
			//printf("end of flash camera weight %.3f\n", flashCameraTrk_weight);
		}
	}
#if defined(var_flashPhotoTrkBeat)  && defined(var_flashPhotoTrkBright) && defined(var_flashPhotoTrkLength) && defined(var_flashPhotoChangeBeat)
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_flashPhotoTrkBeat]
		&& pg_FullScenarioActiveVars[pg_current_configuration_rank][_flashPhotoTrkBright]
		&& pg_FullScenarioActiveVars[pg_current_configuration_rank][_flashPhotoTrkLength]
		&& pg_FullScenarioActiveVars[pg_current_configuration_rank][_flashPhotoChangeBeat]) {
		// flash photo reset
		if (flashPhotoTrk_weight > 0.0f) {
			if (flashPhotoTrk_weight > 0) {
				flashPhotoTrk_weight -= flashPhotoTrk_decay;
				if (flashPhotoTrk_weight < 0) {
					flashPhotoTrk_weight = 0.f;
				}
				flashPhotoTrk_nbFrames++;
			}
			else {
				flashPhotoTrk_weight = 0.0f;
				//printf("end of flash photo weight %.3f\n", flashPhotoTrk_weight);
				flashPhotoTrk_nbFrames = 0;
			}
		}
	}
#endif

	if (photo_diaporama >= 0 && pg_CurrentDiaporamaEnd > 0) {
		if (pg_CurrentDiaporamaEnd < pg_CurrentClockTime) {
			printf("end of flash_photo_diaporama %d\n", pg_FrameNo);
			pg_CurrentDiaporamaEnd = -1;
			photo_diaporama = -1;
			photoWeight = 0.f;
			sprintf(AuxString, "/diaporama_shortName ---"); pg_send_message_udp((char*)"s", AuxString, (char*)"udp_TouchOSC_send");
		}
	}

	// /////////////////////////
	// clear layer reset
	// does not reset if camera capture is still ongoing
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_cameraCaptFreq]
		&& pg_FullScenarioActiveVars[pg_current_configuration_rank][_camera_BG_subtr]) {
		if ((reset_camera || (initialSecondBGCapture == 1)) && (initialBGCapture || secondInitialBGCapture)) {
			isClearAllLayers = 0.f;
		}
	}

	// clear 3 frames
	// clear CA reset 
	// clear layer reset
	// clear all layers reset
	// clear echo reset
	(isClearCA > 0.f ? isClearCA -= 0.35f : isClearCA = 0.f);
	(isClearLayer > 0.f ? isClearLayer -= 0.35f : isClearLayer = 0.f);
	(isClearAllLayers > 0.f ? isClearAllLayers -= 0.35f : isClearAllLayers = 0.f);
	(isClearEcho > 0.f ? isClearEcho -= 0.35f : isClearEcho = 0.f);

	// layer copy reset	
	// copy to layer above (+1) or to layer below (-1)
	copyToNextTrack = 0;

	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_cameraCaptFreq]) {
		// DELAYED CAMERA WEIGHT COUNTDOWN
		if (delayedCameraWeight > 1) {
			delayedCameraWeight--;
		}
		else if (delayedCameraWeight == 1) {
			*((float*)pg_FullScenarioVarPointers[_cameraWeight]) = 1;
			BrokenInterpolationVar[_cameraWeight] = true;
			cameraWeight = 1;
			delayedCameraWeight = 0;
		}
	}

#if defined(PG_WITH_BLUR)
	// blur reset
	if (nb_blur_frames_1 > 0) {
		nb_blur_frames_1--;
		if (nb_blur_frames_1 <= 0) {
			is_blur_1 = false;
		}
	}
	if (nb_blur_frames_2 > 0) {
		nb_blur_frames_2--;
		if (nb_blur_frames_2 <= 0) {
			is_blur_2 = false;
		}
	}
#endif

#if defined(pg_Project_CAaudio)
	// CA seed
	pg_CAseed_trigger = false;
#endif

	// /////////////////////////
	// particle initialization reset
	part_initialization = -1;

	// automatic master incay/decay
	if (pg_master_incay_duration > 0) {
		if (pg_master_incay_start_time + pg_master_incay_duration >= pg_CurrentClockTime) {
			float alpha = max(0.f, min(1.f, float((pg_CurrentClockTime - pg_master_incay_start_time) / pg_master_incay_duration)));
			master = pg_master_incay_start_value + alpha * (1.f - pg_master_incay_start_value);
		}
		else {
			pg_master_incay_duration = 0.;
			master = 1.f;
		}
		BrokenInterpolationVar[_master] = true;
		*((float*)pg_FullScenarioVarPointers[_master]) = master;
	}
	if (pg_master_decay_duration > 0) {
		if (pg_master_decay_start_time + pg_master_decay_duration >= pg_CurrentClockTime) {
			float alpha = max(0.f, min(1.f, float((pg_CurrentClockTime - pg_master_decay_start_time) / pg_master_decay_duration)));
			master = (1.f - alpha) * pg_master_decay_start_value;
		}
		else {
			pg_master_decay_duration = 0.;
			master = 0.f;
		}
		BrokenInterpolationVar[_master] = true;
		*((float*)pg_FullScenarioVarPointers[_master]) = master;
	}
}

///////////////////////////////////////
// MOVIE STREAMING JUMPS
void pg_movie_backward(int nb_frames_back) {
	// calculates hypothetical new position
	int posInMovie = int(pg_movie_capture.get(CAP_PROP_POS_FRAMES));
	int new_pos_in_movie = posInMovie - nb_frames_back;
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_movieCaptFreq]) {
		// rewinds the movie
		if (new_pos_in_movie > 0) {
			pg_movie_capture.set(CAP_PROP_POS_FRAMES, new_pos_in_movie);
			pg_movie_status.set_position(new_pos_in_movie);
			printf("rewinds %d frames %d frames left %.1f initial time\n", int(movieCaptFreq * 10), pg_movie_status.get_nbFramesLeft(), pg_movie_status.get_initialTime());
		}
		else {
			pg_movie_capture.set(CAP_PROP_POS_FRAMES, 0);
			pg_movie_status.reset_movie(int(pg_movie_capture.get(CAP_PROP_FRAME_COUNT)));
			printf("starts from beginning\n");
		}
	}
}

void pg_movie_forward(int nb_frames_forth) {
	// calculates hypothetical new position
	int posInMovie = int(pg_movie_capture.get(CAP_PROP_POS_FRAMES));
	int new_pos_in_movie = posInMovie + nb_frames_forth;
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_movieCaptFreq]) {
		// advances in the movie
		if (new_pos_in_movie < pg_movie_status.get_initialNbFrames()) {
			printf("forward %d frames %d frames left %.1f initial time\n", int(movieCaptFreq * 10), pg_movie_status.get_nbFramesLeft(), pg_movie_status.get_initialTime());
			pg_movie_capture.set(CAP_PROP_POS_FRAMES, new_pos_in_movie);
			pg_movie_status.set_position(new_pos_in_movie);
		}
		// else stays where it 
	}
}

///////////////////////////////////////
// CLIP STREAMING JUMPS

///////////////////////////////////////
// MOVIE AND CAMERA FRAME CAPTURE
void pg_update_clip_camera_and_movie_frame(void) {
	//////////////////////////////////////////////////////////////////////
	// CLIP, CAMERA AND MOVIE FRAME UPDATES BEFORE RENDERING
	// /////////////////////////
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_clipCaptFreq]) {
		// reads next clip frame
		for (int indClipRank = 0; indClipRank < PG_NB_PARALLEL_CLIPS; indClipRank++) {
			for (int clipSide = 0; clipSide < _clipLR; clipSide++) {
				//printf("currentPhotoTrack %d clipCaptFreq %.2f clip No %d nb clips %d side %d rank %d\n", currentPhotoTrack, clipCaptFreq, pg_clip_status[clipSide].getCurrentlyPlaying_clipNo(indClipRank), pg_nbClips, clipSide, indClipRank);
				if (currentPhotoTrack >= 0 && clipCaptFreq > 0
					&& pg_clip_status[clipSide].clip_play[indClipRank]
					&& pg_clip_status[clipSide].getCurrentlyPlaying_clipNo(indClipRank) >= 0
					&& pg_clip_status[clipSide].getCurrentlyPlaying_clipNo(indClipRank) < pg_nbClips[pg_current_configuration_rank]) {

					// either the first frame has not been captured and the clip should loop at the beginning
					// or whe are coming close to the end of the clip
					//printf("clip update side %d rank %d\n", clipSide, indClipRank);
					pg_clip_status[clipSide].get_nextFrameAndUpdate(indClipRank);
				}
			}
		}
	}

	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_movieCaptFreq]) {
		// /////////////////////////
		// reads next movie frame
		if (currentVideoTrack >= 0 && movieCaptFreq > 0
			&& currentlyPlaying_movieNo >= 0
			&& !is_movieLoading
			) {

			// non threaded video update according to fps and time since the beginning of the movie play
			bool new_movie_frame = false;

			// tries to play in sync the whole movie, so counts the time from the 
			// first frame and compares the actual frame no (actualMovieFrameNo)
			// with the frame no expected to play with the correct frame rate (expectedMovieFrameNo)
			int actualMovieFrameNo = (pg_movie_status.get_initialNbFrames() - pg_movie_status.get_nbFramesLeft());
			int expectedMovieFrameNo = int((pg_CurrentClockTime - pg_movie_status.get_initialTime()) * movieCaptFreq);

			is_movieAtEnd = false;

			// either the first frame has not been captured and the movie should loop at the beginning
			// or whe are coming close to the end of the movie
			//printf("%d\n", pg_movie_status.get_nbFramesLeft());
			if (!pg_FirstMovieFrameIsAvailable || (movie_loop && pg_movie_status.get_nbFramesLeft() <= 1)) {
				//printf("Full video loop %d\n", pg_FrameNo);
				// Full video loop
				if (pg_movie_status.get_nbFramesLeft() <= 1) {
					// resets the movie
					pg_movie_capture.set(CAP_PROP_POS_FRAMES, 0);
					pg_movie_status.reset_movie(int(pg_movie_capture.get(CAP_PROP_FRAME_COUNT)));
					printf("Movie restarted\n");
				}
			}

			// if looping is not on and movie is closed to the end, loops on the last 10 frames
			else if (!movie_loop && pg_movie_status.get_nbFramesLeft() <= 1) {
				// does not retrieve more frames and stays on the last one
				is_movieAtEnd = true;
				//// Short end video loop
				//pg_movie_backward(int(movieCaptFreq * 10));
				//printf("Short end video loop at %d, posInMovie %d\n",
				//	pg_FrameNo, pg_movie_status.get_nbFramesLeft());
			}

			// skips frames if rendering is slower than movie fps
			if ((!is_movieAtEnd) && (pg_FirstMovieFrameIsAvailable && expectedMovieFrameNo - actualMovieFrameNo > 2)) {
				while (expectedMovieFrameNo - actualMovieFrameNo > 2) {
					if (pg_movie_capture.grab()) {
						pg_movie_status.read_one_frame();
						actualMovieFrameNo = pg_movie_status.get_nbFramesRead();
						//printf("skip 1 frame\n");
					}
					else {
						break;
					}
				}
			}

			//Grabs and returns a frame from movie if it is time for it
			if (!is_movieAtEnd && (!pg_FirstMovieFrameIsAvailable || expectedMovieFrameNo - actualMovieFrameNo >= 1)) {
				if (pg_movie_capture.grab()) {
					pg_movie_capture.retrieve(pg_movie_frame);
					if (!pg_movie_frame.data) {
						printf("Movie frame not grabbed!\n");
						// has not succeeded to load the movie
						if (!pg_FirstMovieFrameIsAvailable) {
							pg_FirstMovieFrameIsAvailable = false;
						}
						return;
					}
					else {
						pg_movie_status.read_one_frame();
						actualMovieFrameNo = pg_movie_status.get_nbFramesRead();
						if (!pg_FirstMovieFrameIsAvailable) {
							pg_FirstMovieFrameIsAvailable = true;
						}
						new_movie_frame = true;
					}
				}
			}

			//  transfer to GPU
			if (new_movie_frame) {
				pg_initMovieFrameTexture(&pg_movie_frame);
			}
			//elapsedTimeSinceLastCapture = pg_CurrentClockTime - lastFrameCaptureTime;
			// printf("Capture at %.4f last capture %.4f elapsed time %.4f frame duration %.4f\n", pg_CurrentClockTime, lastFrameCaptureTime, elapsedTimeSinceLastCapture, (1.0f / movieCaptFreq));
		}
	}

	// /////////////////////////
	// reads next camera frame
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_cameraCaptFreq]) {
		if ((currentVideoTrack >= 0
			&& pg_cameraCaptureIsOn
			&& (// grabs according to camera capture frequence
				(cameraCaptFreq > 0 && pg_FrameNo % int(60.0 / cameraCaptFreq) == 0)
				// grabs at the beginning, otherwise the program is stuck if no camera frame reading takes place
				|| (pg_FrameNo >= PG_CAMERA_REST_DELAY && pg_FrameNo <= PG_CAMERA_REST_DELAY + 1000 && pg_FrameNo % 3 == 0))
			// a delay has to be let for camera to digest new parameter values
			&& (pg_FrameNo - pg_LastCameraParameterChange_Frame) > PG_CAMERA_REST_DELAY
			// the camera weight is positive
			&& cameraWeight > 0)
			|| (reset_camera || initialSecondBGCapture == 1)) {
			// currentVideoTrack <=> video off
			// frame capture
			// printf("frame capture\n");

			// non threaded
			// printf("*** non threaded camera frame capture %d\n", pg_FrameNo);
			if ((pg_current_active_cameraNo < 0 && -pg_current_active_cameraNo - 1 < int(pg_webCams.size()))
				|| (pg_current_active_cameraNo >= 0 && pg_current_active_cameraNo < int(pg_IPCam_capture.size()))) {
				loadCameraFrame(false, cameraNo);
			}
		}
	}
}

/////////////////////////////////////////////////////////
/////// UPDATES UNIFORMS IN ALL ACTIVE SHADERS
/////// AS WELL AS TABLES FOR SCENARIO VARIABLES BOUND WITH SHADERS
/////////////////////////////////////////////////////////
void pg_update_shader_uniforms(void) {
	pg_update_shader_var_data();
	pg_update_shader_ParticleAnimation_uniforms();
	pg_update_shader_Update_uniforms();
	pg_update_shader_Mixing_uniforms();
	pg_update_shader_Master_uniforms();
	pg_update_shader_ParticleRender_uniforms();
	// no update for uniforms in Sensor and ClipArt shaders
	pg_update_shader_Mesh_uniforms();
}

void pg_update_shader_var_data(void) {
#if defined(CORE)
#include "pg_update_body_Core.cpp"
#endif
#if defined(pg_Project_Voluspa)
#include "pg_update_body_voluspa.cpp"
#endif
#if defined(pg_Project_araKnit)
#include "pg_update_body_araknit.cpp"
#endif

// #include "pg_scripts/pg_update_body.cpp"
	printOglError(510);
}

/////////////////////////////////////////////////////////////////////////
// PARTICLE ANIMATION SHADER UNIFORM VARIABLES
void pg_update_shader_ParticleAnimation_uniforms(void) {
	if (!pg_shader_programme[pg_current_configuration_rank][_pg_shader_ParticleAnimation]) {
		return;
	}

	glUseProgram(pg_shader_programme[pg_current_configuration_rank][_pg_shader_ParticleAnimation]);

	// prints possible particle animation
	if (part_initialization >= 0) {
		printf("particle initialization %d\n", part_initialization);
	}

	// scren size and repop channel
	// at each frame a random repop channel is chosen among the available ones
	int nb_avail_paths = 0;
	for (int pathNo = 0; pathNo < PG_NB_PATHS; pathNo++) {
		if (part_path_repop[pathNo]) {
			nb_avail_paths++;
		}
	}
	int selected_path = 0;
	if (nb_avail_paths > 0) {
		selected_path = int(floor(rand_0_1 * (nb_avail_paths + 1 - 0.00001))) % nb_avail_paths;
		int nb_activ_paths = 0;
		for (int pathNo = 0; pathNo < PG_NB_PATHS; pathNo++) {
			if (part_path_repop[pathNo]) {
				if (nb_activ_paths == selected_path) {
					selected_path = pathNo;
					break;
				}
				nb_activ_paths++;
			}
		}
	}
	glUniform4f(uniform_ParticleAnimation_fs_4fv_W_H_repopChannel_targetFrameNo[pg_current_configuration_rank],
		(GLfloat)workingWindow_width, (GLfloat)window_height, (GLfloat)selected_path, GLfloat(pg_ParticleTargetFrameNo));

	// pen paths positions
	for (int indPath = 0; indPath < (PG_NB_PATHS + 1); indPath++) {
#if defined(var_Novak_flight_on)
		if (pg_FullScenarioActiveVars[indConfiguration][_Novak_flight_on]
			&& indPath > 0) {
			paths_x_prev_memory[indPath] = paths_x_memory[indPath];
			paths_x_memory[indPath] = cur_Novak_flight_2D_points[indPath].x;
			paths_y_prev_memory[indPath] = paths_y_memory[indPath];
			paths_y_memory[indPath] = cur_Novak_flight_2D_points[indPath].y;
		}
		else {
#endif
			paths_x_memory[indPath] = (paths_x[indPath] > 0 ? paths_x[indPath] : paths_x_memory[indPath]);
			paths_x_prev_memory[indPath] = (paths_x_prev[indPath] > 0 ? paths_x_prev[indPath] : paths_x_prev_memory[indPath]);
			paths_y_memory[indPath] = (paths_y[indPath] > 0 ? paths_y[indPath] : paths_y_memory[indPath]);
			paths_y_prev_memory[indPath] = (paths_y_prev[indPath] > 0 ? paths_y_prev[indPath] : paths_y_prev_memory[indPath]);
			//if (indPath == 1) {
			//	printf("xy prev xy %.2f %.2f %.2f %.2f\n", paths_x_memory[indPath], paths_y_memory[indPath], paths_x_prev_memory[indPath], paths_y_prev_memory[indPath]);
			//}
#if defined(var_Novak_flight_on)
		}
#endif
	}

	// position
	for (int indPath = 0; indPath < (PG_NB_PATHS + 1); indPath++) {
		path_data_ParticleAnimation[indPath * PG_MAX_PATH_ANIM_DATA * 4 + PG_PATH_ANIM_POS * 4 + 0] = paths_x_prev_memory[indPath];
		path_data_ParticleAnimation[indPath * PG_MAX_PATH_ANIM_DATA * 4 + PG_PATH_ANIM_POS * 4 + 1] = paths_y_prev_memory[indPath];
	}
	for (int indPath = 0; indPath < (PG_NB_PATHS + 1); indPath++) {
		path_data_ParticleAnimation[indPath * PG_MAX_PATH_ANIM_DATA * 4 + PG_PATH_ANIM_POS * 4 + 2] = paths_x_memory[indPath];
		path_data_ParticleAnimation[indPath * PG_MAX_PATH_ANIM_DATA * 4 + PG_PATH_ANIM_POS * 4 + 3] = paths_y_memory[indPath];
	}
	//printf("path data %.2f %.2f %.2f %.2f\n", paths_x_memory[1], paths_y_memory[1], paths_x_prev_memory[1], paths_y_prev_memory[1]);
	// color, radius, beginning or end of a stroke
	for (int indPath = 0; indPath < (PG_NB_PATHS + 1); indPath++) {
		path_data_ParticleAnimation[indPath * PG_MAX_PATH_ANIM_DATA * 4 + PG_PATH_ANIM_RAD * 4 + 0] = paths_RadiusX[indPath];
		path_data_ParticleAnimation[indPath * PG_MAX_PATH_ANIM_DATA * 4 + PG_PATH_ANIM_RAD * 4 + 1] = rand_0_1;
		path_data_ParticleAnimation[indPath * PG_MAX_PATH_ANIM_DATA * 4 + PG_PATH_ANIM_RAD * 4 + 2] = rand_0_1;
	}
	//printf("path_data_ParticleAnimation %.2f %.2f %.2f %.2f\n", path_data_ParticleAnimation[PG_MAX_PATH_ANIM_DATA * 4 + PG_PATH_ANIM_POS * 4 + 0], 
	//	path_data_ParticleAnimation[PG_MAX_PATH_ANIM_DATA * 4 + PG_PATH_ANIM_RAD * 4 + 1],
	//	path_data_ParticleAnimation[PG_MAX_PATH_ANIM_DATA * 4 + PG_PATH_ANIM_RAD * 4 + 2],
	//	path_data_ParticleAnimation[PG_MAX_PATH_ANIM_DATA * 4 + PG_PATH_ANIM_RAD * 4 + 3]);
	glUniform4fv(uniform_ParticleAnimation_path_data[pg_current_configuration_rank], (PG_NB_PATHS + 1) * PG_MAX_PATH_DATA, path_data_ParticleAnimation);

	// flash Trk -> Part weights
	glUniform4f(uniform_ParticleAnimation_fs_4fv_flashTrkPartWghts[pg_current_configuration_rank],
		flashTrkPart_weights[0], flashTrkPart_weights[1], flashTrkPart_weights[2], flashTrkPart_weights[3]);
	// printf("flashTrkPart_weights %.2f %.2f %.2f %.2f \n", flashTrkPart_weights[0], flashTrkPart_weights[1], flashTrkPart_weights[2], flashTrkPart_weights[3]);

// movie size, flash camera and copy tracks
// copy to layer above (+1) or to layer below (-1)

// flash CA -> BG & repop color (BG & CA)
	glUniform4f(uniform_ParticleAnimation_fs_4fv_repop_Color_frameNo[pg_current_configuration_rank],
		repop_ColorPart_r, repop_ColorPart_g, repop_ColorPart_b, GLfloat(pg_FrameNo));
	// clear layer, flash pixel, flash CA -> Part
	glUniform4f(uniform_ParticleAnimation_fs_4fv_flashCAPartWght_nbPart_clear_partSizeUnpulsed[pg_current_configuration_rank],
		GLfloat(flashCAPart_weight), GLfloat(nb_particles), GLfloat(isClearAllLayers), *((float*)pg_FullScenarioVarPointers[_part_size]));

	// movie size, flash camera and copy tracks
	// copy to layer above (+1) or to layer below (-1)
	glUniform4f(uniform_ParticleAnimation_fs_4fv_Camera_W_H_movieWH[pg_current_configuration_rank],
		GLfloat(pg_camera_frame_width), GLfloat(pg_camera_frame_height),
		GLfloat(pg_movie_frame_width), GLfloat(pg_movie_frame_height));
	printOglError(511);
}

/////////////////////////////////////////////////////////////////////////
// UPDATE SHADER UNIFORM VARIABLES
void pg_update_shader_Update_uniforms(void) {
	if (!pg_shader_programme[pg_current_configuration_rank][_pg_shader_Update]) {
		return;
	}

	glUseProgram(pg_shader_programme[pg_current_configuration_rank][_pg_shader_Update]);
	printOglError(5197);

	// time is only used in TVW
	glUniform4f(uniform_Update_fs_4fv_W_H_time_currentScene[pg_current_configuration_rank],
		(GLfloat)workingWindow_width, (GLfloat)window_height, (GLfloat)pg_CurrentClockTime, (GLfloat)pg_CurrentSceneIndex);
	// printf("time %.2f\n", (GLfloat)pg_CurrentClockTime);
	// printf("scene %d\n", pg_CurrentSceneIndex);
	printOglError(5198);

	// pixels acceleration
	glUniform3f(uniform_Update_fs_3fv_clearAllLayers_clearCA_pulsedShift[pg_current_configuration_rank],
		(GLfloat)isClearAllLayers, (GLfloat)isClearCA,
		fabs(pulse_average - pulse_average_prec) * pulsed_part_Vshift);
	//if (isClearAllLayers > 0.f) {
	//	printf("clear all\n");
	//}
	printOglError(5197);

#if defined(pg_Project_CAaudio)
	if (pg_CAseed_trigger) {
		glUniform4f(uniform_Update_fs_4fv_CAseed_type_size_loc[pg_current_configuration_rank],
			(GLfloat)pg_CAseed_type, (GLfloat)pg_CAseed_size,
			(GLfloat)pg_CAseed_coordinates[0], (GLfloat)pg_CAseed_coordinates[1]);
		printf("pg_CAseed_trigger %d %d at %d %d\n", pg_CAseed_type, pg_CAseed_size,
			pg_CAseed_coordinates[0], pg_CAseed_coordinates[1]);
	}
	else {
		glUniform4f(uniform_Update_fs_4fv_CAseed_type_size_loc[pg_current_configuration_rank],
			(GLfloat)pg_CAseed_type, (GLfloat)pg_CAseed_size,
			-1.f, -1.f);
	}
#endif

	// position
	for (int indPath = 0; indPath < (PG_NB_PATHS + 1); indPath++) {
		path_data_Update[indPath * PG_MAX_PATH_DATA * 4 + PG_PATH_P_X * 4 + 0] = paths_x_prev[indPath];
		path_data_Update[indPath * PG_MAX_PATH_DATA * 4 + PG_PATH_P_X * 4 + 1] = paths_xL[indPath];
		path_data_Update[indPath * PG_MAX_PATH_DATA * 4 + PG_PATH_P_X * 4 + 2] = paths_xR[indPath];
		path_data_Update[indPath * PG_MAX_PATH_DATA * 4 + PG_PATH_P_Y * 4 + 0] = paths_y_prev[indPath];
		path_data_Update[indPath * PG_MAX_PATH_DATA * 4 + PG_PATH_P_Y * 4 + 1] = paths_yL[indPath];
		path_data_Update[indPath * PG_MAX_PATH_DATA * 4 + PG_PATH_P_Y * 4 + 2] = paths_yR[indPath];
	}
	for (int indPath = 0; indPath < fingers; indPath++) {
		path_data_Update[indPath * PG_MAX_PATH_DATA * 4 + PG_PATH_P_X * 4 + 3] = paths_x_forGPU[indPath];
		path_data_Update[indPath * PG_MAX_PATH_DATA * 4 + PG_PATH_P_Y * 4 + 3] = paths_y_forGPU[indPath];
	}
	for (int indPath = fingers; indPath < (PG_NB_PATHS + 1); indPath++) {
		path_data_Update[indPath * PG_MAX_PATH_DATA * 4 + PG_PATH_P_X * 4 + 3] = paths_x[indPath];
		path_data_Update[indPath * PG_MAX_PATH_DATA * 4 + PG_PATH_P_Y * 4 + 3] = paths_y[indPath];
	}
	// color, radius, beginning or end of a stroke
	for (int indPath = 0; indPath < (PG_NB_PATHS + 1); indPath++) {
		path_data_Update[indPath * PG_MAX_PATH_DATA * 4 + PG_PATH_COLOR * 4 + 0] = paths_Color_r[indPath];
		path_data_Update[indPath * PG_MAX_PATH_DATA * 4 + PG_PATH_COLOR * 4 + 1] = paths_Color_g[indPath];
		path_data_Update[indPath * PG_MAX_PATH_DATA * 4 + PG_PATH_COLOR * 4 + 2] = paths_Color_b[indPath];
		path_data_Update[indPath * PG_MAX_PATH_DATA * 4 + PG_PATH_COLOR * 4 + 3] = paths_Color_a[indPath];
		path_data_Update[indPath * PG_MAX_PATH_DATA * 4 + PG_PATH_RADIUS_BEGINEND * 4 + 0] = paths_RadiusX[indPath];
		path_data_Update[indPath * PG_MAX_PATH_DATA * 4 + PG_PATH_RADIUS_BEGINEND * 4 + 1] = (isBegin[indPath] ? 1.f : (isEnd[indPath] ? -1.f : 0.f));
		path_data_Update[indPath * PG_MAX_PATH_DATA * 4 + PG_PATH_RADIUS_BEGINEND * 4 + 2] = paths_RadiusY[indPath];
		path_data_Update[indPath * PG_MAX_PATH_DATA * 4 + PG_PATH_RADIUS_BEGINEND * 4 + 3] = float(paths_BrushID[indPath]);
	}
	//printf("Path radius (%d) %.1f %.1f brush ID %.0f xy_GPU %.1f %.1f Color RGBA %.1f %.1f %.1f %.1f \n", 0,
	//	paths_RadiusX[0], paths_RadiusY[0], float(paths_BrushID[0]),
	//	paths_x_forGPU[0], paths_y_forGPU[0],
	//	paths_Color_r[0], paths_Color_g[0], paths_Color_b[0], paths_Color_a[0]);
	//printf("Bezier in data : %.1fx%.1f %.1fx%.1f %.1fx%.1f %.1fx%.1f \n",
	//	paths_x_prev[0], paths_y_prev[0],
	//	paths_xL[0], paths_yL[0],
	//	paths_xR[0], paths_yR[0],
	//	paths_x[0], paths_y[0]);
	// printf("Radius %.2f %.2f\n", paths_RadiusX[0], paths_RadiusY[0]);
	//printf("pos %.1f %.1f %.1f %.1f %.1f %.1f %.1f %.1f \n", paths_x_prev[1], paths_y_prev[1],
	//	paths_xR[1], paths_yR[1], paths_xL[1], paths_yL[1], paths_x[1], paths_y[1]);
	for (int indPath = 0; indPath < (PG_NB_PATHS + 1); indPath++) {
		build_bounding_box(indPath);
	}

	// bounding box containing the bezier curve augmented by its radius
	for (int indPath = 0; indPath < (PG_NB_PATHS + 1); indPath++) {
		path_data_Update[indPath * PG_MAX_PATH_DATA * 4 + PG_PATH_BOX * 4 + 0] = pg_BezierBox[indPath].x;
		path_data_Update[indPath * PG_MAX_PATH_DATA * 4 + PG_PATH_BOX * 4 + 1] = pg_BezierBox[indPath].y;
		path_data_Update[indPath * PG_MAX_PATH_DATA * 4 + PG_PATH_BOX * 4 + 2] = pg_BezierBox[indPath].z;
		path_data_Update[indPath * PG_MAX_PATH_DATA * 4 + PG_PATH_BOX * 4 + 3] = pg_BezierBox[indPath].w;
		//if (indPath == 1) {
		//	printf("bounding box %.1f %.1f %.1f %.1f\n", 
		//		path_data_Update[indPath * PG_MAX_PATH_DATA * 4 + PG_PATH_BOX * 4 + 0],
		//		path_data_Update[indPath * PG_MAX_PATH_DATA * 4 + PG_PATH_BOX * 4 + 1],
		//		path_data_Update[indPath * PG_MAX_PATH_DATA * 4 + PG_PATH_BOX * 4 + 2],
		//		path_data_Update[indPath * PG_MAX_PATH_DATA * 4 + PG_PATH_BOX * 4 + 3]);
		//}
	}

	//printf("pos %.1f %.1f %.1f %.1f bez %.1f %.1f %.1f %.1f beging./end %d %d \n", paths_Color_r[1], paths_Color_g[1], paths_Color_b[1], paths_Color_a[1],
	//	pg_BezierBox[1].x, pg_BezierBox[1].y, pg_BezierBox[1].z, pg_BezierBox[1].w, isBegin[1], isEnd[1]);
	glUniform4fv(uniform_Update_path_data[pg_current_configuration_rank], (PG_NB_PATHS + 1) * PG_MAX_PATH_DATA, path_data_Update);

#if defined(pg_Project_Criton)
	glUniform4f(uniform_Update_fs_4fv_fftLevels03[pg_current_configuration_rank],
		fftLevels[0], fftLevels[1], fftLevels[2], fftLevels[3]);
	glUniform4f(uniform_Update_fs_4fv_fftFrequencies03[pg_current_configuration_rank],
		fftFrequencies[0], fftFrequencies[1], fftFrequencies[2], fftFrequencies[3]);
	glUniform4f(uniform_Update_fs_4fv_fftPhases03[pg_current_configuration_rank],
		fftPhases[0], fftPhases[1], fftPhases[2], fftPhases[3]);
	glUniform4f(uniform_Update_fs_4fv_fftLevels47[pg_current_configuration_rank],
		fftLevels[4], fftLevels[5], fftLevels[6], fftLevels[7]);
	glUniform4f(uniform_Update_fs_4fv_fftFrequencies47[pg_current_configuration_rank],
		fftFrequencies[4], fftFrequencies[5], fftFrequencies[6], fftFrequencies[7]);
	glUniform4f(uniform_Update_fs_4fv_fftPhases47[pg_current_configuration_rank],
		fftPhases[4], fftPhases[5], fftPhases[6], fftPhases[7]);
	//printf("Freq %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f\n", fftFrequencies[0], fftFrequencies[1], fftFrequencies[2], fftFrequencies[3], fftFrequencies[4], fftFrequencies[5], fftFrequencies[6], fftFrequencies[7]);
	//printf("Levels %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f\n", fftLevels[0], fftLevels[1], fftLevels[2], fftLevels[3], fftLevels[4], fftLevels[5], fftLevels[6], fftLevels[7]);
	//printf("Phases %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f\n", fftPhases[0], fftPhases[1], fftPhases[2], fftPhases[3], fftPhases[4], fftPhases[5], fftPhases[6], fftPhases[7]);
	//printf("Freq %.2f\n", fftFrequencies[0]);
	//printf("Levels %.2f\n", fftLevels[0]);
	//printf("Phase %.2f\n", fftPhases[0]);
#endif

	// flash BG weights
	glUniform4f(uniform_Update_fs_4fv_flashTrkBGWghts_flashPartBGWght[pg_current_configuration_rank],
		flashTrkBG_weights[1], flashTrkBG_weights[2], flashTrkBG_weights[3], flashPartBG_weight);

// flash Trk -> CA weights
	glUniform4f(uniform_Update_fs_4fv_flashTrkCAWghts[pg_current_configuration_rank],
		flashTrkCA_weights[0], flashTrkCA_weights[1], flashTrkCA_weights[2], flashTrkCA_weights[3]);
	// printf("flashTrkCA_weights %.2f %.2f %.2f %.2f \n", flashTrkCA_weights[0], flashTrkCA_weights[1], flashTrkCA_weights[2], flashTrkCA_weights[3]);

	//printf("Signs %d \n", currentDrawingTrack);

	// CA type, frame no, flashback and current track
	glUniform4f(uniform_Update_fs_4fv_frameno_Cursor_flashPartCAWght_doubleWindow[pg_current_configuration_rank],
		(GLfloat)pg_ConfigurationFrameNo,
		(GLfloat)CurrentCursorStylusvsRubber, flashPartCA_weight, (GLfloat)double_window);

	// movie size, flash camera and copy tracks
	// copy to layer above (+1) or to layer below (-1)
	glUniform4f(uniform_Update_fs_4fv_movieWH_flashCameraTrkWght_cpTrack[pg_current_configuration_rank],
		(GLfloat)pg_movie_frame_width, (GLfloat)pg_movie_frame_height,
		flashCameraTrk_weight, (GLfloat)copyToNextTrack);

	// printf("Movie size %dx%d\n", pg_movie_frame_width, pg_movie_frame_height);
	// printf("Flash camera coef %.1f\n", flashCameraTrk_weight);
	if (copyToNextTrack != 0)
		printf("copyToNextTrack %d\n", copyToNextTrack);

	// photo flash
	photo_offsetX = 0;
	photo_offsetY = 0;
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_photoJitterAmpl]
		&& pg_FullScenarioActiveVars[pg_current_configuration_rank][_photo_offsetX]
		&& pg_FullScenarioActiveVars[pg_current_configuration_rank][_photo_offsetY]) {
		float angle = photoJitterAmpl * (pulse[0] - 0.5f) * float(M_PI);
		float radius = photoJitterAmpl * (pulse[2] - 0.5f);
		// offset for POT coordinates
		photo_offsetX = radius * cos(angle) / workingWindow_width;
		photo_offsetY = radius * sin(angle) / window_height;
		glUniform4f(uniform_Update_fs_4fv_flashPhotoTrkWght_flashPhotoTrkThres_Photo_offSetsXY[pg_current_configuration_rank],
			flashPhotoTrk_weight, flashPhotoTrk_threshold, photo_offsetX, photo_offsetY);
	}

	// flash CA -> BG & repop color (BG & CA)
	glUniform4f(uniform_Update_fs_4fv_repop_ColorBG_flashCABGWght[pg_current_configuration_rank],
		repop_ColorBGcolorRed, repop_ColorBG_g, repop_ColorBG_b, flashCABG_weight);

#if !defined(var_alKemi)
		glUniform3f(uniform_Update_fs_3fv_repop_ColorCA[pg_current_configuration_rank],
			repop_ColorCA_r, repop_ColorCA_g, repop_ColorCA_b);
#else
		if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_alKemi] == false) {
			glUniform3f(uniform_Update_fs_3fv_repop_ColorCA[pg_current_configuration_rank],
				repop_ColorCA_r, repop_ColorCA_g, repop_ColorCA_b);
		}
#endif

	// clear layer, flash pixel, flash CA -> Part
	glUniform3f(uniform_Update_fs_3fv_isClearLayer_flashPixel_flashCameraTrkThres[pg_current_configuration_rank],
		(GLfloat)isClearLayer, (GLfloat)flashPixel, flashCameraTrk_threshold);
	// photo rendering
	if (photo_diaporama >= 0) {
		glUniform4f(uniform_Update_fs_4fv_photo01_wh[pg_current_configuration_rank],
			workingWindow_width_powerOf2_ratio, window_height_powerOf2_ratio,
			workingWindow_width_powerOf2_ratio, window_height_powerOf2_ratio);
		//printf("photo WH %.2fx%.2f %.2fx%.2f\n",
	//	workingWindow_width_powerOf2_ratio, window_height_powerOf2_ratio,
	//	workingWindow_width_powerOf2_ratio, window_height_powerOf2_ratio);

		glUniform4f(uniform_Update_fs_4fv_photo01Wghts_randomValues[pg_current_configuration_rank],
			pg_Photo_weight[0], pg_Photo_weight[1], rand_0_1, rand_0_1);
		//glUniform4f(uniform_Update_fs_4fv_photo01Wghts_randomValues[pg_current_configuration_rank],
		//	0.f, 1.f, rand_0_1, rand_0_1);
		//printf("photo weight %.2f %.2f phot index %d %d\n", pg_Photo_weight[0], pg_Photo_weight[1],
			//pg_Photo_swap_buffer_data[0].indSwappedPhoto,
			//pg_Photo_swap_buffer_data[1].indSwappedPhoto);
	}
	else {
		glUniform4f(uniform_Update_fs_4fv_photo01Wghts_randomValues[pg_current_configuration_rank],
			0.f, 0.f, rand_0_1, rand_0_1);
		//printf("photo weight diaporama -1 0.f 0.f\n");
	}

	// clip rendering
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_clip_mix]
		&& photo_diaporama < 0) {
		// clip weights 
#if PG_NB_PARALLEL_CLIPS >= 2
		if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_clipCaptFreq]) {
			// 4 clips play
			if ((playing_clipNoLeft >= 0 && playing_clipNoLeft < pg_nbClips[pg_current_configuration_rank])
				|| (playing_clipNoRight >= 0 && playing_clipNoRight < pg_nbClips[pg_current_configuration_rank])
				|| (playing_secondClipNoLeft >= 0 && playing_secondClipNoLeft < pg_nbClips[pg_current_configuration_rank])
				|| (playing_secondClipNoRight >= 0 && playing_secondClipNoRight < pg_nbClips[pg_current_configuration_rank])) {
				glUniform4f(uniform_Update_fs_4fv_photo01_wh[pg_current_configuration_rank],
					float(clip_crop_width[pg_current_configuration_rank]) / clip_image_width[pg_current_configuration_rank], float(clip_crop_height[pg_current_configuration_rank]) / clip_image_height[pg_current_configuration_rank],
					float(clip_crop_width[pg_current_configuration_rank]) / clip_image_width[pg_current_configuration_rank], float(clip_crop_height[pg_current_configuration_rank]) / clip_image_height[pg_current_configuration_rank]);
				float wl0 = 0.f; // weight of first left clip
				float wr0 = 0.f; // weight of first right clip
				float wl1 = 0.f; // weight of second left clip
				float wr1 = 0.f; // weight of second right clip
				// valid first left clip
				if (playing_clipNoLeft >= 0 && playing_clipNoLeft < pg_nbClips[pg_current_configuration_rank]) {
					wl0 = 1.f;
				}
				// valid first right clip
				if (playing_clipNoRight >= 0 && playing_clipNoRight < pg_nbClips[pg_current_configuration_rank]) {
					wr0 = 1.f;
				}
				// valid second left clip
				if (playing_secondClipNoLeft >= 0 && playing_secondClipNoLeft < pg_nbClips[pg_current_configuration_rank]) {
					wl1 = 1.f;
				}
				// valid second right clip
				if (playing_secondClipNoRight >= 0 && playing_secondClipNoRight < pg_nbClips[pg_current_configuration_rank]) {
					wr1 = 1.f;
				}
				//printf("wl0 %.2f wl1 %.2f\n", wl0, wl1);
				// mixing fader in the middle
				// first and second clips are mixed fully
				// the right and left levels are used for lateral mixing
				if (clip_mix == 0) {
					glUniform4f(uniform_Update_fs_4fv_photo01Wghts_randomValues[pg_current_configuration_rank],
						wl0 * pg_clip_status[_clipLeft].clip_level[0], wr0 * pg_clip_status[_clipRight].clip_level[0], 
						rand_0_1, rand_0_1);
					glUniform2f(uniform_Update_fs_2fv_clip01Wghts[pg_current_configuration_rank],
						wl1 * pg_clip_status[_clipLeft].clip_level[1], wr1 * pg_clip_status[_clipRight].clip_level[1]);
					//printf("neut wl0 %.2f wl1 %.2f\n", wl0, wl1);
				}
				// second clips are mixed fully
				// first clip progressively decrease
				// the right and left levels are used for lateral mixing
				else if (clip_mix < 0) {
					clip_mix = max(clip_mix, -0.5f);
					glUniform4f(uniform_Update_fs_4fv_photo01Wghts_randomValues[pg_current_configuration_rank],
						wl0 * (1 - 2 * -clip_mix) * pg_clip_status[_clipLeft].clip_level[0], wr0 * (1 - 2 * -clip_mix) * pg_clip_status[_clipRight].clip_level[0], 
						rand_0_1, rand_0_1);
					glUniform2f(uniform_Update_fs_2fv_clip01Wghts[pg_current_configuration_rank],
						wl1 * pg_clip_status[_clipLeft].clip_level[1], wr1 * pg_clip_status[_clipRight].clip_level[1]);
					//printf("neg %.2f wl0 %.2f wl1 %.2f\n", clip_mix, wl0 * (1 - 2 * -clip_mix), wl1);
				}
				// first clips are mixed fully
				// second lip progressively decrease
				// the right and left levels are used for lateral mixing
				else if (clip_mix > 0) {
					clip_mix = min(clip_mix, 0.5f);
					glUniform4f(uniform_Update_fs_4fv_photo01Wghts_randomValues[pg_current_configuration_rank],
						wl0 * pg_clip_status[_clipLeft].clip_level[0], wr0 * pg_clip_status[_clipRight].clip_level[0], 
						rand_0_1, rand_0_1);
					glUniform2f(uniform_Update_fs_2fv_clip01Wghts[pg_current_configuration_rank],
						wl1 * (1 - 2 * clip_mix) * pg_clip_status[_clipLeft].clip_level[1], wr1 * (1 - 2 * clip_mix) * pg_clip_status[_clipRight].clip_level[1]);
					//printf("pos %.2f wl0 %.2f wl1 %.2f\n", clip_mix, wl0, wl1 * (1 - 2 * clip_mix));
				}
			}
		}
#else
		if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_clipCaptFreq]) {
			// 2 clips play
			if ((playing_clipNoLeft >= 0 && playing_clipNoLeft < pg_nbClips[pg_current_configuration_rank])
				|| (playing_clipNoRight >= 0 && playing_clipNoRight < pg_nbClips[pg_current_configuration_rank])) {
				glUniform4f(uniform_Update_fs_4fv_photo01_wh[pg_current_configuration_rank],
					float(clip_crop_width[pg_current_configuration_rank]) / clip_image_width[pg_current_configuration_rank], float(clip_crop_height[pg_current_configuration_rank]) / clip_image_height[pg_current_configuration_rank],
					float(clip_crop_width[pg_current_configuration_rank]) / clip_image_width[pg_current_configuration_rank], float(clip_crop_height[pg_current_configuration_rank]) / clip_image_height[pg_current_configuration_rank]);
				//printf("photo WH %.2fx%.2f %.2fx%.2f\n",
				//	workingWindow_width_powerOf2_ratio, window_height_powerOf2_ratio,
				//	workingWindow_width_powerOf2_ratio, window_height_powerOf2_ratio);
				if (!(playing_clipNoLeft >= 0 && playing_clipNoLeft < pg_nbClips[pg_current_configuration_rank])) {
					glUniform4f(uniform_Update_fs_4fv_photo01Wghts_randomValues[pg_current_configuration_rank],
						0.0, pg_clip_status[_clipRight].clip_level[0], rand_0_1, rand_0_1);
					//printf("clip weights %.2f %.2f\n", 0.0, pg_clip_status[_clipRight].clip_level[0]);
				}
				else if (!(playing_clipNoRight >= 0 && playing_clipNoRight < pg_nbClips[pg_current_configuration_rank])) {
					glUniform4f(uniform_Update_fs_4fv_photo01Wghts_randomValues[pg_current_configuration_rank],
						pg_clip_status[_clipLeft].clip_level1[1], 0.0, rand_0_1, rand_0_1);
					//printf("clip weights %.2f %.2f\n", pg_clip_status[_clipLeft].clip_level1[1], 0.0);
				}
			}
		}
#endif
	}

	//printf("camera movie weight %.2f %.2f\n",
	//	*((float *)pg_FullScenarioVarPointers[_cameraWeight]), *((float *)pg_FullScenarioVarPointers[_movieWeight]));

	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_cameraCaptFreq]) {
		// camera texture offset 
		glUniform4f(uniform_Update_fs_4fv_Camera_offSetsXY_Camera_W_H[pg_current_configuration_rank],
			(GLfloat)pg_camera_x_offset, (GLfloat)pg_camera_y_offset,
			(GLfloat)pg_camera_frame_width, (GLfloat)pg_camera_frame_height);
		//printf("Update shader camera frame size %dx%d offset %dx%d\n", pg_camera_frame_width, pg_camera_frame_height, pg_camera_x_offset, pg_camera_y_offset);
	}

	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_slow_track_translation]) {
		if (slow_track_translation) {
			// track x & y translations
			float translation_x[2] = { 0.f, 0.f };
			float translation_y[2] = { 0.f, 0.f };
			translation_x[0] = frame_based_translation(track_x_transl[0], 0);
			translation_y[0] = frame_based_translation(track_y_transl[0], 1);
			translation_x[1] = frame_based_translation(track_x_transl[1], 2);
			translation_y[1] = frame_based_translation(track_y_transl[1], 3);
			glUniform4f(uniform_Update_fs_4fv_xy_transl_tracks_0_1[pg_current_configuration_rank],
				translation_x[0], translation_y[0], translation_x[1], translation_y[1]);
		}
		else {
			glUniform4f(uniform_Update_fs_4fv_xy_transl_tracks_0_1[pg_current_configuration_rank],
				track_x_transl[0], track_y_transl[0], track_x_transl[1], track_y_transl[1]);
		}
	}
	else {
		glUniform4f(uniform_Update_fs_4fv_xy_transl_tracks_0_1[pg_current_configuration_rank],
			track_x_transl[0], track_y_transl[0], track_x_transl[1], track_y_transl[1]);
	}

	// acceleration center and CA subtype
	// in case of interpolation between CA1 and CA2 
	if (!BrokenInterpolationVar[_CA1_CA2_weight]) {
		if (CA1_CA2_weight < 1.0 && CA1_CA2_weight > 0.0) {
			float randVal = rand_0_1;
			if (randVal <= CA1_CA2_weight) {
				CAInterpolatedType = CA1Type;
				CAInterpolatedSubType = CA1SubType;
			}
			else {
				CAInterpolatedType = CA2Type;
				CAInterpolatedSubType = CA2SubType;
			}
		}
		else if (CA1_CA2_weight >= 1.0) {
			CAInterpolatedType = CA1Type;
			CAInterpolatedSubType = CA1SubType;
		}
		else if (CA1_CA2_weight <= 0.0) {
			CAInterpolatedType = CA2Type;
			CAInterpolatedSubType = CA2SubType;
		}
			//printf("CA1/CA2 mix: CA type/subtype %d-%d\n" , CAInterpolatedType, CAInterpolatedSubType);
	}

#if !defined(PG_WITH_BLUR)
	glUniform4f(uniform_Update_fs_4fv_CAType_SubType_blurRadius[pg_current_configuration_rank],
		GLfloat(CAInterpolatedType), GLfloat(CAInterpolatedSubType),
		0.f, 0.f);
#else
	glUniform4f(uniform_Update_fs_4fv_CAType_SubType_blurRadius[pg_current_configuration_rank],
		GLfloat(CAInterpolatedType), GLfloat(CAInterpolatedSubType),
		(is_blur_1 ? float(blurRadius_1) : 0.f), (is_blur_2 ? float(blurRadius_2) : 0.f));
#endif
	printOglError(51905);

	// printf("CA type/subtype %d-%d\n" , CAInterpolatedType, CAInterpolatedSubType);
	// printf("blur %.2f %.2f\n", (is_blur_1 ? float(blurRadius_1) : 0.f), (is_blur_2 ? float(blurRadius_2) : 0.f));

	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_clipCaptFreq]) {
		glUniform3f(uniform_Update_fs_3fv_photo_rgb[pg_current_configuration_rank], pg_clip_status[_clipLeft].clip_r_channel_level[0],
			pg_clip_status[_clipLeft].clip_g_channel_level[0],
			pg_clip_status[_clipLeft].clip_b_channel_level[0]);
		//printf("RGB L %.2f %.2f %.2f\n", pg_clip_status[_clipLeft].clip_r_channel_level, pg_clip_status[_clipLeft].clip_g_channel_level, pg_clip_status[_clipLeft].clip_b_channel_level);
		//printf("RGB R %.2f %.2f %.2f\n", pg_clip_status[_clipRight].clip_r_channel_level, pg_clip_status[_clipRight].clip_g_channel_level, pg_clip_status[_clipRight].clip_b_channel_level);
	}


	// sets the time of the 1st plane launch 
#if defined(var_GenerativeNights_planes)
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_GenerativeNights_planes]) {
		if (CAInterpolatedType == CA_NEUMANN_BINARY && firstPlaneFrameNo < 0) {
			// printf("First plane frame no %d\n", firstPlaneFrameNo);
			firstPlaneFrameNo = pg_FrameNo - 2300;
		}
		glUniform2f(uniform_Update_fs_2fv_initCA_1stPlaneFrameNo[pg_current_configuration_rank], initCA, GLfloat(firstPlaneFrameNo));
		// one shot CA launching
		if (initCA > 0.0f) {
			initCA = 0.0f;
		}
	}
#endif

	if (CAInterpolatedType_prev != CAInterpolatedType
		|| CAInterpolatedSubType_prev != CAInterpolatedSubType) {
		sprintf(AuxString, "/CAType %d", CAInterpolatedType);
		pg_send_message_udp((char*)"i", AuxString, (char*)"udp_TouchOSC_send");
		sprintf(AuxString, "/CASubType %d", CAInterpolatedSubType);
		pg_send_message_udp((char*)"i", AuxString, (char*)"udp_TouchOSC_send");
		CAInterpolatedType_prev = CAInterpolatedType;
		CAInterpolatedSubType_prev = CAInterpolatedSubType;
	}
	printOglError(519);
}

/////////////////////////////////////////////////////////////////////////
// PARTICLE RENDERING SHADER UNIFORM VARIABLES
void pg_update_shader_ParticleRender_uniforms(void) {
	if (!pg_shader_programme[pg_current_configuration_rank][_pg_shader_ParticleRender]) {
		return;
	}
	if (PG_PARTICLE_TYPE == 2) {
		glUseProgram(pg_shader_programme[pg_current_configuration_rank][_pg_shader_ParticleRender]);
		glUniform3f(uniform_ParticleCurve_gs_3fv_partRadius_partType_highPitchPulse[pg_current_configuration_rank],
			(part_size + pulse_average * part_size_pulse * part_size) / 512.f,
			(GLfloat)particle_type, pulse[2]);
		// printf("part radius GS %.2f\n", (part_size + pulse_average * part_size_pulse * part_size));

		///////////////////////////////////////////////////////////////////////
		bool assigned = false;
		for (int indPath = 1; indPath < PG_NB_PATHS + 1; indPath++) {
			// active reading
			if (is_path_replay[indPath]) {
				glUniform3f(uniform_ParticleCurve_vp_3fv_trackReplay_xy_height[pg_current_configuration_rank], paths_x[indPath], paths_y[indPath], (float)window_height);
				assigned = true;
				break;
			}
		}
		if (!assigned) {
			if (paths_x[0] >= 0 && paths_y[0] >= 0) {
				glUniform3f(uniform_ParticleCurve_vp_3fv_trackReplay_xy_height[pg_current_configuration_rank], paths_x[0], paths_y[0], (float)window_height);
			}
			else {
				glUniform3f(uniform_ParticleCurve_vp_3fv_trackReplay_xy_height[pg_current_configuration_rank], -1000.f, -1000.f, (float)window_height);
			}
		}
	}
	else if (PG_PARTICLE_TYPE == 0 || PG_PARTICLE_TYPE == 1) {
		glUseProgram(pg_shader_programme[pg_current_configuration_rank][_pg_shader_ParticleRender]);
		glUniform4f(uniform_ParticleSplat_gs_4fv_part_size_partType_highPitchPulse_windowRatio[pg_current_configuration_rank],
			(part_size + pulse_average * part_size_pulse * part_size) / 512.f,
			(GLfloat)particle_type,
			pulse[2], float(workingWindow_width) / float(window_height));
		//printf("part size gs: %.3f\n", (part_size + pulse_average * part_size_pulse * part_size));

		///////////////////////////////////////////////////////////////////////
		bool assigned = false;
		for (int indPath = 1; indPath < PG_NB_PATHS + 1; indPath++) {
			// active reading
			if (is_path_replay[indPath]) {
				glUniform3f(uniform_ParticleSplat_vp_3fv_trackReplay_xy_height[pg_current_configuration_rank], paths_x[indPath], paths_y[indPath], (float)window_height);
				assigned = true;
				break;
			}
		}
		if (!assigned) {
			if (paths_x[0] >= 0 && paths_y[0] >= 0) {
				glUniform3f(uniform_ParticleSplat_vp_3fv_trackReplay_xy_height[pg_current_configuration_rank], paths_x[0], paths_y[0], (float)window_height);
			}
			else {
				glUniform3f(uniform_ParticleSplat_vp_3fv_trackReplay_xy_height[pg_current_configuration_rank], -1000.f, -1000.f, (float)window_height);
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////
// MIXING SHADER UNIFORM VARIABLES
void pg_update_shader_Mixing_uniforms(void) {
	if (pg_shader_programme[pg_current_configuration_rank][_pg_shader_Mixing]) {
		glUseProgram(pg_shader_programme[pg_current_configuration_rank][_pg_shader_Mixing]);

		// CA weight
#if defined(var_flashPhotoTrkBeat)  && defined(var_flashPhotoTrkBright) && defined(var_flashPhotoTrkLength) && defined(var_flashPhotoChangeBeat)
		if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_flashPhotoTrkBeat]
			&& pg_FullScenarioActiveVars[pg_current_configuration_rank][_flashPhotoTrkBright]
			&& pg_FullScenarioActiveVars[pg_current_configuration_rank][_flashPhotoTrkLength]
			&& pg_FullScenarioActiveVars[pg_current_configuration_rank][_flashPhotoChangeBeat]) {
			glUniform3f(uniform_Mixing_fs_3fv_height_flashCameraTrkWght_flashPhotoTrkWght[pg_current_configuration_rank],
				(GLfloat)window_height, flashCameraTrk_weight, flashPhotoTrk_weight);
		}
		else
#else
		{
			glUniform3f(uniform_Mixing_fs_3fv_height_flashCameraTrkWght_flashPhotoTrkWght[pg_current_configuration_rank],
				(GLfloat)window_height, flashCameraTrk_weight, 0.f);
		}
#endif

		// TEXT TRANSPARENCY
		glUniform3f(uniform_Mixing_fs_3fv_screenMsgTransp_Text1_2_Alpha[pg_current_configuration_rank],
			messageTransparency,
			(GLfloat)1.f, (GLfloat)0.f);	}
		printOglError(512);
}

/////////////////////////////////////////////////////////////////////////
// MASTER SHADER UNIFORM VARIABLES
void pg_update_shader_Master_uniforms(void) {
	if (pg_shader_programme[pg_current_configuration_rank][_pg_shader_Master]) {
		glUseProgram(pg_shader_programme[pg_current_configuration_rank][_pg_shader_Master]);
		// hoover cursor
		mobile_cursor = ((tablet_prec_x != CurrentCursorHooverPos_x) || (tablet_prec_y != CurrentCursorHooverPos_y));
		tablet_prec_x = CurrentCursorHooverPos_x;
		tablet_prec_y = CurrentCursorHooverPos_y;
		//printf("cur %d %d frame %d mobile %d\n", CurrentCursorHooverPos_x, CurrentCursorHooverPos_y, pg_FrameNo, mobile_cursor);
		glUniform4f(uniform_Master_fs_4fv_xy_frameno_pulsedShift[pg_current_configuration_rank],
			(GLfloat)CurrentCursorHooverPos_x, (GLfloat)CurrentCursorHooverPos_y,
			(GLfloat)pg_FrameNo, (pulse_average - pulse_average_prec) * track_x_transl_pulse[0]);

		// screen size
		glUniform4f(uniform_Master_fs_4fv_width_height_timeFromStart_muteRightScreen[pg_current_configuration_rank],
			(GLfloat)workingWindow_width, (GLfloat)window_height, GLfloat(pg_CurrentClockTime - InitialScenarioTime),
			(GLfloat)muteRightScreen);

		// printf("mobile cursor %d\n", (mobile_cursor ? 1 : 0));
		glUniform2i(uniform_Master_fs_2iv_mobile_cursor_currentScene[pg_current_configuration_rank], (mobile_cursor ? 1 : 0), pg_CurrentSceneIndex);
		if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_BGcolor]
			|| pg_FullScenarioActiveVars[pg_current_configuration_rank][_flashchange_BGcolor_freq]) {
			// high bandpass color
			glUniform3f(uniform_Master_fs_3fv_BGcolor_rgb[pg_current_configuration_rank],
				BGcolorRed, BGcolorGreen, BGcolorBlue);
		}
	}
	/*
	if (pg_ConfigurationFrameNo % 1000 == 0) {
		printf("Master #%d %.5f %.5f %.5f %.5f %.5f %.5f %.5f %.5f %d %d\n", pg_current_configuration_rank, (GLfloat)CurrentCursorHooverPos_x, (GLfloat)CurrentCursorHooverPos_y,
			(GLfloat)pg_FrameNo, (pulse_average - pulse_average_prec) * track_x_transl[0]_pulse,
			(GLfloat)workingWindow_width, (GLfloat)window_height, GLfloat(pg_CurrentClockTime - InitialScenarioTime),
			(GLfloat)muteRightScreen,
			(mobile_cursor ? 1 : 0), pg_CurrentSceneIndex);
	}
	*/
}

/////////////////////////////////////////////////////////////////////////
// MESH SHADER UNIFORM VARIABLES
void pg_update_shader_Mesh_uniforms(void) {
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_activeMeshes]
		&& pg_shader_programme[pg_current_configuration_rank][_pg_shader_Mesh]) {
		/////////////////////////////////////////////////////////////////////////
		// MESH SHADER UNIFORM VARIABLES
		glUseProgram(pg_shader_programme[pg_current_configuration_rank][_pg_shader_Mesh]);
		// the variable of the mesh shader is updated before each rendering mode (lines of facets)
			glUniform3f(uniform_Mesh_fs_3fv_light[pg_current_configuration_rank], mesh_light_x, mesh_light_y, mesh_light_z);
#if defined(var_Contact_mesh_expand)
			if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_Contact_mesh_expand]) {
				glUniform2f(uniform_Mesh_vp_2fv_dilate_explode[pg_current_configuration_rank], Contact_mesh_expand + pulse_average * Contact_mesh_expand_pulse, Contact_mesh_explode);
			}
		//printf("mesh %.2f %.2f\n", Contact_mesh_expand + pulse_average * Contact_mesh_expand_pulse, Contact_mesh_explode);
#endif
		if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_textureFrontier_wmin]) {
			glUniform4f(uniform_Mesh_fs_4fv_textureFrontier[pg_current_configuration_rank], textureFrontier_wmin, textureFrontier_wmax, textureFrontier_hmin, textureFrontier_hmax);
			glUniform4f(uniform_Mesh_fs_4fv_textureFrontier_width[pg_current_configuration_rank], textureFrontier_wmin_width, textureFrontier_wmax_width, textureFrontier_hmin_width, textureFrontier_hmax_width);
			glUniform4f(uniform_Mesh_fs_4fv_textureScaleTransl[pg_current_configuration_rank], textureScale_w, textureScale_h, textureTranslate_w, textureTranslate_h);
		}
#if defined(var_Contact_mesh_palette)
		if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_Contact_mesh_palette]) {
			float pulsed_color[3];
			compute_pulsed_palette_color(Contact_mesh_color, pen_color_pulse, Contact_mesh_grey, pen_grey_pulse, pulsed_color, _PG_PEN);
			glUniform4f(uniform_Mesh_fs_4fv_color_palette[pg_current_configuration_rank], pulsed_color[0], pulsed_color[1], pulsed_color[2], float(Contact_mesh_palette));
		}
#endif
	}
	printOglError(517);
}

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
// MULTIPASS RENDERING
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////
// PASS #0: PARTICLE ANIMATION PASS

void pg_ParticleAnimationPass(void) {
	if (!pg_shader_programme[pg_current_configuration_rank][_pg_shader_ParticleAnimation]) {
		return;
	}

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, pg_FBO_ParticleAnimation);
	// printf("FBO ANIMATION       %d\n", pg_FBO_ParticleAnimation[((pg_FrameNo + 1) % 2)]);
	pg_bindFBOTextures(pg_FBO_ParticleAnimation, pg_FBO_ParticleAnimation_texID + ((pg_FrameNo + 1) % 2) * PG_FBO_PARTICLEANIMATION_NBATTACHTS, PG_FBO_PARTICLEANIMATION_NBATTACHTS, false, 0);

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	////////////////////////////////////////
	// activate shaders and sets uniform variable values    
	glUseProgram(pg_shader_programme[pg_current_configuration_rank][_pg_shader_ParticleAnimation]);

	glBindVertexArray(pg_vaoID[pg_VAOQuad]);

	glUniformMatrix4fv(uniform_ParticleAnimation_vp_proj[pg_current_configuration_rank], 1, GL_FALSE, pg_orthoWindowProjMatrix);
	glUniformMatrix4fv(uniform_ParticleAnimation_vp_view[pg_current_configuration_rank], 1, GL_FALSE, pg_identityViewMatrix);
	glUniformMatrix4fv(uniform_ParticleAnimation_vp_model[pg_current_configuration_rank], 1, GL_FALSE, pg_identityModelMatrix);

	// texture unit location
	glUniform1i(uniform_ParticleAnimation_texture_fs_Part_init_pos_speed[pg_current_configuration_rank], pg_Part_init_pos_speed_ParticleAnimation_sampler);
	glUniform1i(uniform_ParticleAnimation_texture_fs_Part_init_col_rad[pg_current_configuration_rank], pg_Part_init_col_rad_ParticleAnimation_sampler);
	glUniform1i(uniform_ParticleAnimation_texture_fs_Part_acc[pg_current_configuration_rank], pg_Part_image_acc_ParticleAnimation_sampler);
	glUniform1i(uniform_ParticleAnimation_texture_fs_CA[pg_current_configuration_rank], pg_CA_FBO_ParticleAnimation_sampler);
	glUniform1i(uniform_ParticleAnimation_texture_fs_CA[pg_current_configuration_rank], pg_CA_FBO_ParticleAnimation_sampler);
	glUniform1i(uniform_ParticleAnimation_texture_fs_Part_pos_speed[pg_current_configuration_rank], pg_Part_pos_speed_FBO_ParticleAnimation_sampler);
	glUniform1i(uniform_ParticleAnimation_texture_fs_Part_col_rad[pg_current_configuration_rank], pg_Part_col_rad_FBO_ParticleAnimation_sampler);
	glUniform1i(uniform_ParticleAnimation_texture_fs_Part_Target_pos_col_rad[pg_current_configuration_rank], pg_Part_Target_pos_col_rad_FBO_ParticleAnimation_sampler);
	glUniform1i(uniform_ParticleAnimation_texture_fs_Noise[pg_current_configuration_rank], pg_Noise_ParticleAnimation_sampler);
	glUniform1i(uniform_ParticleAnimation_texture_fs_RepopDensity[pg_current_configuration_rank], pg_RepopDensity_ParticleAnimation_sampler);
	glUniform1i(uniform_ParticleAnimation_texture_fs_Camera_frame[pg_current_configuration_rank], pg_Camera_frame_ParticleAnimation_sampler);
	glUniform1i(uniform_ParticleAnimation_texture_fs_Movie_frame[pg_current_configuration_rank], pg_Movie_frame_ParticleAnimation_sampler);
	glUniform1i(uniform_ParticleAnimation_texture_fs_Trk0[pg_current_configuration_rank], pg_Trk0_FBO_ParticleAnimation_sampler);
	glUniform1i(uniform_ParticleAnimation_texture_fs_Trk1[pg_current_configuration_rank], pg_Trk1_FBO_ParticleAnimation_sampler);
	glUniform1i(uniform_ParticleAnimation_texture_fs_Trk2[pg_current_configuration_rank], pg_Trk2_FBO_ParticleAnimation_sampler);
	glUniform1i(uniform_ParticleAnimation_texture_fs_Trk3[pg_current_configuration_rank], pg_Trk3_FBO_ParticleAnimation_sampler);

	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// position speed are stored in a texture
	glActiveTexture(GL_TEXTURE0 + pg_Part_init_pos_speed_ParticleAnimation_sampler);
	if (part_initialization >= 0
		&& part_initialization < int(pg_particle_initial_pos_speed_texID[pg_current_configuration_rank].size())) {
		glBindTexture(GL_TEXTURE_RECTANGLE, pg_particle_initial_pos_speed_texID[pg_current_configuration_rank].at(part_initialization)); // pos - speed 
		printf("particle initialization (position speed) %d\n", part_initialization);
	}
	else {
		glBindTexture(GL_TEXTURE_RECTANGLE, NULL_ID);
	}

	// photo or video particles, position and speed are chosen from a random texture between 
	// 0 & PG_NB_PARTICLE_INITIAL_IMAGES - 1
	//else if (part_initialization >= 0
	//		&& part_initialization < int(pg_particle_initial_pos_speed_texID.size())) {
	//	unsigned int indTex = unsigned int(floor(rand_0_1 * pg_particle_initial_pos_speed_texID.size())) % pg_particle_initial_pos_speed_texID.size();
	//	glBindTexture(GL_TEXTURE_RECTANGLE, pg_particle_initial_pos_speed_texID.at(indTex)); // pos - speed 
	//	printf("particle initialization %d texture %d\n", part_initialization, indTex);
	//}

	glActiveTexture(GL_TEXTURE0 + pg_Part_init_col_rad_ParticleAnimation_sampler);
	// color and radius are taken from photo or video and partic radius
	if (part_initialization >= 0
		&& part_initialization < int(pg_particle_initial_pos_speed_texID[pg_current_configuration_rank].size())) {
		glBindTexture(GL_TEXTURE_RECTANGLE, pg_particle_initial_color_radius_texID[pg_current_configuration_rank].at(part_initialization)); // color RGB - rad 
		//printf("particle initialization (color radius) %d\n", part_initialization);
	}
	else {
		glBindTexture(GL_TEXTURE_RECTANGLE, NULL_ID);
	}

	glActiveTexture(GL_TEXTURE0 + pg_Part_image_acc_ParticleAnimation_sampler);
	// acceleration is taken from image 
	if (part_image_acceleration >= 0
		&& part_image_acceleration < int(pg_particle_acc_texID[pg_current_configuration_rank].size())) {
		glBindTexture(GL_TEXTURE_RECTANGLE, pg_particle_acc_texID[pg_current_configuration_rank].at(part_image_acceleration)); // color RGB - rad 
		//printf("particle image acceleration (image based acceleration) %d\n", part_image_acceleration);
	}
	else {
		glBindTexture(GL_TEXTURE_RECTANGLE, NULL_ID);
	}

	// 2-cycle ping-pong CA step n step n (FBO attachment 0)
	glActiveTexture(GL_TEXTURE0 + pg_CA_FBO_ParticleAnimation_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[(pg_FrameNo % 2) * PG_FBO_UPDATE_NBATTACHTS + pg_CA_FBO_Update_attcht]);

	// 2-cycle ping-pong position/speed of particles step n step n (FBO attachment 0)
	glActiveTexture(GL_TEXTURE0 + pg_Part_pos_speed_FBO_ParticleAnimation_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_ParticleAnimation_texID[(pg_FrameNo % 2) * PG_FBO_PARTICLEANIMATION_NBATTACHTS + pg_Part_pos_speed_FBO_ParticleAnimation_attcht]);

	// 2-cycle ping-pong color/radius of particles step n step n (FBO attachment 1)
	glActiveTexture(GL_TEXTURE0 + pg_Part_col_rad_FBO_ParticleAnimation_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_ParticleAnimation_texID[(pg_FrameNo % 2) * PG_FBO_PARTICLEANIMATION_NBATTACHTS + pg_Part_col_rad_FBO_ParticleAnimation_attcht]);

	// 2-cycle ping-pong target position/color/radius of particles step n step n (FBO attachment 2)
	glActiveTexture(GL_TEXTURE0 + pg_Part_Target_pos_col_rad_FBO_ParticleAnimation_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_ParticleAnimation_texID[(pg_FrameNo % 2) * PG_FBO_PARTICLEANIMATION_NBATTACHTS + pg_Part_Target_pos_col_rad_FBO_ParticleAnimation_attcht]);

	// noise texture (noise is also generated procedurally in the update shader)
	glActiveTexture(GL_TEXTURE0 + pg_Noise_ParticleAnimation_sampler);
	glBindTexture(GL_TEXTURE_3D, Noise_texture_3D[pg_current_configuration_rank]);

	// noise texture (noise is also generated procedurally in the update shader)
	glActiveTexture(GL_TEXTURE0 + pg_RepopDensity_ParticleAnimation_sampler);
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_Part_repop_density]) {
		if (Part_repop_density >= 0
			&& int(Part_repop_density) < int(pg_RepopDensity_texture_texID[pg_current_configuration_rank].size())) {
			glBindTexture(GL_TEXTURE_RECTANGLE, pg_RepopDensity_texture_texID[pg_current_configuration_rank].at(Part_repop_density));
			//printf("part repop density %d/%d\n", Part_repop_density, pg_RepopDensity_texture_texID.at(Part_repop_density[pg_current_configuration_rank]));
		}
		else {
			glBindTexture(GL_TEXTURE_RECTANGLE, NULL_ID);
		}
	}
	else {
		glBindTexture(GL_TEXTURE_RECTANGLE, NULL_ID);
	}

	// camera texture produced at preceding pass
	// glActiveTexture(GL_TEXTURE0 + 7);
	// glBindTexture(GL_TEXTURE_RECTANGLE, FBO_CameraFrame_texID);
	// current camera texture
	glActiveTexture(GL_TEXTURE0 + pg_Camera_frame_ParticleAnimation_sampler);
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_cameraCaptFreq]) {
		glBindTexture(GL_TEXTURE_RECTANGLE, pg_camera_texture_texID);
	}
	else {
		glBindTexture(GL_TEXTURE_RECTANGLE, NULL_ID);
	}

	// movie texture
	glActiveTexture(GL_TEXTURE0 + pg_Movie_frame_ParticleAnimation_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_movie_texture_texID);

	// 2-cycle ping-pong BG track step n (FBO attachment 5)
	glActiveTexture(GL_TEXTURE0 + pg_Trk0_FBO_ParticleAnimation_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[(pg_FrameNo % 2) * PG_FBO_UPDATE_NBATTACHTS + pg_Trk0_FBO_Update_attcht]);

	// 2-cycle ping-pong track 1 step n (FBO attachment 6)
	glActiveTexture(GL_TEXTURE0 + pg_Trk1_FBO_ParticleAnimation_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[(pg_FrameNo % 2) * PG_FBO_UPDATE_NBATTACHTS + pg_Trk1_FBO_Update_attcht]);

	// 2-cycle ping-pong track 2 step n (FBO attachment 7)
	glActiveTexture(GL_TEXTURE0 + pg_Trk2_FBO_ParticleAnimation_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[(pg_FrameNo % 2) * PG_FBO_UPDATE_NBATTACHTS + pg_Trk2_FBO_Update_attcht]);

	// 2-cycle ping-pong track 2 step n (FBO attachment 8)
	glActiveTexture(GL_TEXTURE0 + pg_Trk3_FBO_ParticleAnimation_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[(pg_FrameNo % 2) * PG_FBO_UPDATE_NBATTACHTS + pg_Trk3_FBO_Update_attcht]);

	// draw points from the currently bound VAO with current in-use shader
	// glDrawArrays (GL_TRIANGLES, 0, 3 * PG_SIZE_QUAD_ARRAY);
	// Index buffer for indexed rendering
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pg_vboID[pg_EABQuad]);
	// Draw the triangles !
	glDrawElements(
		GL_TRIANGLE_STRIP,      // mode
		3 * PG_SIZE_QUAD_ARRAY,    // count
		GL_UNSIGNED_INT,   // type
		(void*)0           // element array buffer offset
	);

	printOglError(52);
}

//////////////////////////////////////////////////
// PASS #1: UPDATE (CA, PIXELS, PARTICLES, DRAWING, PHOTOS & VIDEOS)

void pg_UpdatePass(void) {
	if (!pg_shader_programme[pg_current_configuration_rank][_pg_shader_Update]) {
		return;
	}

	// ping pong output and input FBO bindings
	// next frame
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, pg_FBO_Update);
	//glBindFramebuffer(GL_READ_FRAMEBUFFER, pg_FBO_Update[(pg_FrameNo % 2)]);
	//printf("Output FBO %d\n", pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * PG_FBO_UPDATE_NBATTACHTS + pg_Pixels_FBO_Update_attcht]);
	//printf("Input FBO %d\n", pg_FBO_Update_texID[(pg_FrameNo % 2) * PG_FBO_UPDATE_NBATTACHTS + pg_Pixels_FBO_Update_attcht]);	pg_bindFBOTextures(pg_FBO_Update, pg_FBO_Update_texID, PG_FBO_UPDATE_NBATTACHTS, false, 0);
	pg_bindFBOTextures(pg_FBO_Update, pg_FBO_Update_texID + ((pg_FrameNo + 1) % 2) * PG_FBO_UPDATE_NBATTACHTS, PG_FBO_UPDATE_NBATTACHTS, false, 0);

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	////////////////////////////////////////
	// activate shaders and sets uniform variable values    
	glUseProgram(pg_shader_programme[pg_current_configuration_rank][_pg_shader_Update]);
	//if (pg_FrameNo % 300 == 0) {
	//	printf("pg_shader_programme udate %d\n", pg_shader_programme[pg_current_configuration_rank][_pg_shader_Update]);
	//}
	glBindVertexArray(pg_vaoID[pg_VAOQuad]);
	glUniformMatrix4fv(uniform_Update_vp_proj[pg_current_configuration_rank], 1, GL_FALSE, pg_orthoWindowProjMatrix);
	glUniformMatrix4fv(uniform_Update_vp_view[pg_current_configuration_rank], 1, GL_FALSE, pg_identityViewMatrix);
	glUniformMatrix4fv(uniform_Update_vp_model[pg_current_configuration_rank], 1, GL_FALSE, pg_identityModelMatrix);

	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_photo_homography]) {
		if (photo_homography) {
			// homography for background texture distortion
	// source points
			std::vector<cv::Point2f> sourcePoints;
			// destination points
			std::vector<cv::Point2f> destinationPoints;

			// the current image coordinates are given by the keystone variables
			sourcePoints.push_back(cv::Point2f(VP1KeystoneXBottomLeft, VP1KeystoneYBottomLeft));
			sourcePoints.push_back(cv::Point2f(VP1KeystoneXBottomRight, VP1KeystoneYBottomRight));
			sourcePoints.push_back(cv::Point2f(VP1KeystoneXTopRight, VP1KeystoneYTopRight));
			sourcePoints.push_back(cv::Point2f(VP1KeystoneXTopLeft, VP1KeystoneYTopLeft));

			// the initial texture coordinates are based on unit for the whole image
			// they are then multiplied by the image ratio (wrt power of two uper dimensions)
			// inside the shader to access only the part of the texture that corresponds to the image
			destinationPoints.push_back(cv::Point2f(0.0f, 1.f));
			destinationPoints.push_back(cv::Point2f(1.f, 1.f));
			destinationPoints.push_back(cv::Point2f(1.f, 0.0f));
			destinationPoints.push_back(cv::Point2f(0.0f, 0.0f));

			// initializes the homography matrices for the distortion of the projected image
			pg_calculate_homography_matrices(&sourcePoints, &destinationPoints, pg_homographyForTexture, 3);

			// and transfer the matrix to the shader
			glUniformMatrix3fv(uniform_Update_homographyForTexture[pg_current_configuration_rank], 1, GL_FALSE, pg_homographyForTexture);
		}
	}


	////////////////////////////////////////////////////////
	// texture unit location
	glUniform1i(uniform_Update_texture_fs_CA[pg_current_configuration_rank], pg_CA_FBO_Update_sampler);
	glUniform1i(uniform_Update_texture_fs_PreviousCA[pg_current_configuration_rank], pg_PreviousCA_FBO_Update_sampler);
	glUniform1i(uniform_Update_texture_fs_Pixels[pg_current_configuration_rank], pg_Pixels_FBO_Update_sampler);
	glUniform1i(uniform_Update_texture_fs_Brushes[pg_current_configuration_rank], pg_Brushes_Update_sampler);
	glUniform1i(uniform_Update_texture_fs_Camera_frame[pg_current_configuration_rank], pg_Camera_frame_Update_sampler);
	glUniform1i(uniform_Update_texture_fs_Camera_BG[pg_current_configuration_rank], pg_Camera_BG_Update_sampler);
	glUniform1i(uniform_Update_texture_fs_Movie_frame[pg_current_configuration_rank], pg_Movie_frame_Update_sampler);
	glUniform1i(uniform_Update_texture_fs_Noise[pg_current_configuration_rank], pg_Noise_Update_sampler);
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_BG_CA_repop_density]) {
		glUniform1i(uniform_Update_texture_fs_RepopDensity[pg_current_configuration_rank], pg_RepopDensity_Update_sampler);
	}

	glUniform1i(uniform_Update_texture_fs_Photo0[pg_current_configuration_rank], pg_Photo0_Update_sampler);
	glUniform1i(uniform_Update_texture_fs_Photo1[pg_current_configuration_rank], pg_Photo1_Update_sampler);
#if PG_NB_PARALLEL_CLIPS >= 2
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_clipCaptFreq]) {
		glUniform1i(uniform_Update_texture_fs_Clip0[pg_current_configuration_rank], pg_SecondClipLeft_Update_sampler);
		glUniform1i(uniform_Update_texture_fs_Clip1[pg_current_configuration_rank], pg_SecondClipRight_Update_sampler);
	}
#endif
	glUniform1i(uniform_Update_texture_fs_Part_render[pg_current_configuration_rank], pg_Particle_render_FBO_Update_sampler);

	glUniform1i(uniform_Update_texture_fs_Trk0[pg_current_configuration_rank], pg_Trk0_FBO_Update_sampler);
	glUniform1i(uniform_Update_texture_fs_Trk1[pg_current_configuration_rank], pg_Trk1_FBO_Update_sampler);
	glUniform1i(uniform_Update_texture_fs_Trk2[pg_current_configuration_rank], pg_Trk2_FBO_Update_sampler);
	glUniform1i(uniform_Update_texture_fs_Trk3[pg_current_configuration_rank], pg_Trk3_FBO_Update_sampler);
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_CATable]) {
		glUniform1i(uniform_Update_texture_fs_CATable[pg_current_configuration_rank], pg_CATable_Update_sampler);
	}
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_camera_BG_subtr]) {
		glUniform1i(uniform_Update_texture_fs_Camera_BGIni[pg_current_configuration_rank], pg_Camera_BGIni_FBO_Update_sampler);
	}
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_pixel_image_acceleration]) {
		glUniform1i(uniform_Update_texture_fs_pixel_acc[pg_current_configuration_rank], pg_pixel_image_acc_Update_sampler);
	}

	////////////////////////////////////////////////////////
	// by default textures are wrapped
	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	////////////////////////////////////////////////////////
	// texture unit binding
	// 2-cycle ping-pong CA step n (FBO attachment 0) -- current Frame
	glActiveTexture(GL_TEXTURE0 + pg_CA_FBO_Update_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[(pg_FrameNo % 2) * PG_FBO_UPDATE_NBATTACHTS + pg_CA_FBO_Update_attcht]);
	// 2-cycle ping-pong CA step n (FBO attachment 0) -- previous Frame
	glActiveTexture(GL_TEXTURE0 + pg_PreviousCA_FBO_Update_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * PG_FBO_UPDATE_NBATTACHTS + pg_CA_FBO_Update_attcht]);
	//printf("pg_FBO_Update_texID\n");

	// 2-cycle ping-pong speed/position of pixels step n step n (FBO attachment 1) -- current Frame
	glActiveTexture(GL_TEXTURE0 + pg_Pixels_FBO_Update_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[(pg_FrameNo % 2) * PG_FBO_UPDATE_NBATTACHTS + pg_Pixels_FBO_Update_attcht]);
	//if (pg_FrameNo < 1000) {
	//	printf("FBO Pixels Update textures config: %d IDs: %d\n", pg_current_configuration_rank, 
	//		pg_FBO_Update_texID[(pg_FrameNo % 2) * PG_FBO_UPDATE_NBATTACHTS + pg_Pixels_FBO_Update_attcht]);
	//}

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// pen patterns
	glActiveTexture(GL_TEXTURE0 + pg_Brushes_Update_sampler);
	glBindTexture(GL_TEXTURE_3D, Pen_texture_3D_texID[pg_current_configuration_rank]);

	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// camera texture produced at preceding pass
	// glActiveTexture(GL_TEXTURE0 + 7);
	// glBindTexture(GL_TEXTURE_RECTANGLE, FBO_CameraFrame_texID);
	// current camera texture
	glActiveTexture(GL_TEXTURE0 + pg_Camera_frame_Update_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_camera_texture_texID);

	// current background texture
	glActiveTexture(GL_TEXTURE0 + pg_Camera_BG_Update_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_camera_BG_texture_texID);

	// movie texture
	glActiveTexture(GL_TEXTURE0 + pg_Movie_frame_Update_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_movie_texture_texID);

	// noise texture (noise is also generated procedurally in the update shader)
	glActiveTexture(GL_TEXTURE0 + pg_Noise_Update_sampler);
	glBindTexture(GL_TEXTURE_3D, Noise_texture_3D[pg_current_configuration_rank]);

	// repop density texture
	glActiveTexture(GL_TEXTURE0 + pg_RepopDensity_Update_sampler);
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_BG_CA_repop_density]
		&& BG_CA_repop_density >= 0
		&& int(BG_CA_repop_density) < int(pg_RepopDensity_texture_texID[pg_current_configuration_rank].size())) {
		glBindTexture(GL_TEXTURE_RECTANGLE, pg_RepopDensity_texture_texID[pg_current_configuration_rank].at(BG_CA_repop_density));
	}
	else {
		glBindTexture(GL_TEXTURE_RECTANGLE, NULL_ID);
	}

	// photo[0] texture
	glActiveTexture(GL_TEXTURE0 + pg_Photo0_Update_sampler);
	if (playing_clipNoLeft >= 0
		&& playing_clipNoLeft < pg_nbClips[pg_current_configuration_rank]
		&& pg_firstCompressedClipFramesInFolder[pg_current_configuration_rank][playing_clipNoLeft]
		+ pg_clip_status[_clipLeft].get_lastFrame(0) < pg_nbCompressedClipFrames[pg_current_configuration_rank]
		&& pg_firstCompressedClipFramesInFolder[pg_current_configuration_rank][playing_clipNoLeft]
		+ pg_clip_status[_clipLeft].get_lastFrame(0) >= 0) {
		glBindTexture(GL_TEXTURE_2D, pg_ClipFrames_buffer_data[pg_current_configuration_rank][pg_firstCompressedClipFramesInFolder[pg_current_configuration_rank][playing_clipNoLeft]
			+ pg_clip_status[_clipLeft].get_lastFrame(0)]->texBuffID);
	}
	else if (photo_diaporama >= 0
		&& photo_diaporama < int(pg_compressedImageData[pg_current_configuration_rank].size())
		&& pg_compressedImageData[pg_current_configuration_rank][photo_diaporama].size() > 0
		&& pg_Photo_swap_buffer_data[0].indSwappedPhoto >= 0
		&& pg_Photo_swap_buffer_data[0].indSwappedPhoto < int(pg_compressedImageData[pg_current_configuration_rank][photo_diaporama].size())) {
		//printf("diapo %d tex buf1 ID %d\n", photo_diaporama, 
		//	pg_compressedImageData[pg_current_configuration_rank][photo_diaporama][pg_Photo_swap_buffer_data[0].indSwappedPhoto].texBuffID);
		glBindTexture(GL_TEXTURE_2D, pg_compressedImageData[pg_current_configuration_rank][photo_diaporama][pg_Photo_swap_buffer_data[0].indSwappedPhoto].texBuffID);
	}
	else {
		glBindTexture(GL_TEXTURE_2D, NULL_ID);
	}

	// photo[1] texture
	glActiveTexture(GL_TEXTURE0 + pg_Photo1_Update_sampler);
	if (playing_clipNoRight >= 0
		&& playing_clipNoRight < pg_nbClips[pg_current_configuration_rank]
		&& pg_firstCompressedClipFramesInFolder[pg_current_configuration_rank][playing_clipNoRight]
		+ pg_clip_status[_clipRight].get_lastFrame(0) < pg_nbCompressedClipFrames[pg_current_configuration_rank]
		&& pg_firstCompressedClipFramesInFolder[pg_current_configuration_rank][playing_clipNoRight]
		+ pg_clip_status[_clipRight].get_lastFrame(0) >= 0) {
		glBindTexture(GL_TEXTURE_2D, pg_ClipFrames_buffer_data[pg_current_configuration_rank][pg_firstCompressedClipFramesInFolder[pg_current_configuration_rank][playing_clipNoRight]
			+ pg_clip_status[_clipRight].get_lastFrame(0)]->texBuffID);
	}
	else if (photo_diaporama >= 0
		&& photo_diaporama < int(pg_compressedImageData[pg_current_configuration_rank].size())
		&& pg_compressedImageData[pg_current_configuration_rank][photo_diaporama].size() > 0
		&& pg_Photo_swap_buffer_data[1].indSwappedPhoto >= 0
		&& pg_Photo_swap_buffer_data[1].indSwappedPhoto < int(pg_compressedImageData[pg_current_configuration_rank][photo_diaporama].size())) {
		//printf("diapo %d tex buf2 ID %d\n", photo_diaporama, 
		//	pg_compressedImageData[pg_current_configuration_rank][photo_diaporama][pg_Photo_swap_buffer_data[1].indSwappedPhoto].texBuffID);
		glBindTexture(GL_TEXTURE_2D, pg_compressedImageData[pg_current_configuration_rank][photo_diaporama][pg_Photo_swap_buffer_data[1].indSwappedPhoto].texBuffID);
	}
	else {
		//printf("null photo texture\n");
		glBindTexture(GL_TEXTURE_2D, NULL_ID);
	}

#if PG_NB_PARALLEL_CLIPS >= 2
	// clip[0] texture
	// second clips on left and right
	glActiveTexture(GL_TEXTURE0 + pg_SecondClipLeft_Update_sampler);
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_clipCaptFreq]
		&& playing_secondClipNoLeft >= 0
		&& playing_secondClipNoLeft < pg_nbClips[pg_current_configuration_rank]
		&& pg_firstCompressedClipFramesInFolder[pg_current_configuration_rank][playing_secondClipNoLeft]
		+ pg_clip_status[_clipLeft].get_lastFrame(1) < pg_nbCompressedClipFrames[pg_current_configuration_rank]
		&& pg_firstCompressedClipFramesInFolder[pg_current_configuration_rank][playing_secondClipNoLeft]
		+ pg_clip_status[_clipLeft].get_lastFrame(1) >= 0) {
		glBindTexture(GL_TEXTURE_2D, pg_ClipFrames_buffer_data[pg_current_configuration_rank][pg_firstCompressedClipFramesInFolder[pg_current_configuration_rank][playing_secondClipNoLeft]
			+ pg_clip_status[_clipLeft].get_lastFrame(1)]->texBuffID);
	}
	else {
		glBindTexture(GL_TEXTURE_2D, NULL_ID);
	}

	// clip[1] texture
	glActiveTexture(GL_TEXTURE0 + pg_SecondClipRight_Update_sampler);
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_clipCaptFreq]
		&& playing_secondClipNoRight >= 0
		&& playing_secondClipNoRight < pg_nbClips[pg_current_configuration_rank]
		&& pg_firstCompressedClipFramesInFolder[pg_current_configuration_rank][playing_secondClipNoRight]
		+ pg_clip_status[_clipRight].get_lastFrame(1) < pg_nbCompressedClipFrames[pg_current_configuration_rank]
		&& pg_firstCompressedClipFramesInFolder[pg_current_configuration_rank][playing_secondClipNoRight]
		+ pg_clip_status[_clipRight].get_lastFrame(1) >= 0) {
		glBindTexture(GL_TEXTURE_2D, pg_ClipFrames_buffer_data[pg_current_configuration_rank][pg_firstCompressedClipFramesInFolder[pg_current_configuration_rank][playing_secondClipNoRight]
			+ pg_clip_status[_clipRight].get_lastFrame(1)]->texBuffID);
	}
	else {
		//printf("null second photo texture\n");
		glBindTexture(GL_TEXTURE_2D, NULL_ID);
	}
#endif

	// FBO capture of particle rendering used for flashing layers with particles
	glActiveTexture(GL_TEXTURE0 + pg_Particle_render_FBO_Update_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Particle_render_texID);

	// 2-cycle ping-pong BG track step n (FBO attachment 5) -- current Frame
	glActiveTexture(GL_TEXTURE0 + pg_Trk0_FBO_Update_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[(pg_FrameNo % 2) * PG_FBO_UPDATE_NBATTACHTS + pg_Trk0_FBO_Update_attcht]);

	// 2-cycle ping-pong track 1 step n (FBO attachment 6) -- current Frame
	glActiveTexture(GL_TEXTURE0 + pg_Trk1_FBO_Update_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[(pg_FrameNo % 2) * PG_FBO_UPDATE_NBATTACHTS + pg_Trk1_FBO_Update_attcht]);

	// 2-cycle ping-pong track 2 step n (FBO attachment 7) -- current Frame
	glActiveTexture(GL_TEXTURE0 + pg_Trk2_FBO_Update_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[(pg_FrameNo % 2) * PG_FBO_UPDATE_NBATTACHTS + pg_Trk2_FBO_Update_attcht]);

	// 2-cycle ping-pong track 2 step n (FBO attachment 8) -- current Frame
	glActiveTexture(GL_TEXTURE0 + pg_Trk3_FBO_Update_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[(pg_FrameNo % 2) * PG_FBO_UPDATE_NBATTACHTS + pg_Trk3_FBO_Update_attcht]);

	// CA Data table (FBO attachment 11)
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_CATable]) {
		glActiveTexture(GL_TEXTURE0 + pg_CATable_Update_sampler);
		glBindTexture(GL_TEXTURE_RECTANGLE, pg_CATable_ID);
	}

	// Initial background texture
	glActiveTexture(GL_TEXTURE0 + pg_Camera_BGIni_FBO_Update_sampler);
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_camera_BG_subtr]) {
		glBindTexture(GL_TEXTURE_RECTANGLE, pg_camera_BGIni_texture_texID);
	}
	else {
		glBindTexture(GL_TEXTURE_RECTANGLE, NULL_ID);
	}

	// pixel acceleration  (FBO attachment 11)
	//printf("pixel image acceleration (image based acceleration) %d size %d\n", pixel_image_acceleration, pg_pixel_acc_texID.size());
	glActiveTexture(GL_TEXTURE0 + pg_pixel_image_acc_Update_sampler);
	// acceleration is taken from photo 
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_pixel_image_acceleration] 
		&& pixel_image_acceleration >= 0
		&& pixel_image_acceleration < int(pg_pixel_acc_texID[pg_current_configuration_rank].size())) {
		glBindTexture(GL_TEXTURE_RECTANGLE, pg_pixel_acc_texID[pg_current_configuration_rank].at(pixel_image_acceleration)); // color RGB - rad 
		//printf("pixel image acceleration (image based acceleration) %d\n", pixel_image_acceleration);
	}
	else {
		glBindTexture(GL_TEXTURE_RECTANGLE, NULL_ID);
	}

	// draw points from the currently bound VAO with current in-use shader
	// glDrawArrays (GL_TRIANGLES, 0, 3 * PG_SIZE_QUAD_ARRAY);
	// Index buffer for indexed rendering
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pg_vboID[pg_EABQuad]);
	// Draw the triangles !
	glDrawElements(
		GL_TRIANGLE_STRIP,      // mode
		3 * PG_SIZE_QUAD_ARRAY,    // count
		GL_UNSIGNED_INT,   // type
		(void*)0           // element array buffer offset
	);

	printOglError(5521);
}

/////////////////////////////////////
// PASS #2: TEXT, ClipArt CLIP ART RENDERING PASS
void pg_ClipArtRenderingPass(void) {
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_activeClipArts] 
		&& pg_shader_programme[pg_current_configuration_rank][_pg_shader_ClipArt]
		&& has_NV_path_rendering) {
		if (pg_FrameNo > 0) {
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, pg_FBO_ClipArtRendering);
		}

		glClearColor(0.0, 0.0, 0.0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		////////////////////////////////////////
		// ClipArt TEXT OR CLIP ART RENDERING    
		////////////////////////////////////////
		if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_moving_messages] && moving_messages >= 0) {
			/* pg_Project_Etoiles TEASER */
			/*
			if (pg_CurrentSceneIndex == 0) {
				pg_Display_ClipArt_Text(&pg_Ind_Current_DisplayText, 0);
			}
			else if (pg_CurrentSceneIndex == 5) {
				pg_Display_ClipArt_Text(&pg_Ind_Current_DisplayText, 1);
			}
			:*/
			pg_Display_ClipArt_Text(&pg_Ind_Current_DisplayText, int(moving_messages));
		}
		else {
			pg_Display_All_ClipArt(activeClipArts);
		}
		printOglError(52526);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// PASS #3: PARTICLE RENDERING PASS
void pg_ParticleRenderingPass(void) {
	if (!pg_shader_programme[pg_current_configuration_rank][_pg_shader_ParticleRender]) {
		return;
	}

	// printf("nb paticles %d\^n", nb_particles);
	// draws the Bezier curve

	if (pg_FrameNo > 0) {
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, pg_FBO_ParticleRendering);
	}

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	////////////////////////////////////////
	// PARTICLE RENDERING    
	////////////////////////////////////////
	if (PG_PARTICLE_TYPE == 0) {
		glClear(GL_DEPTH_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
		//glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
	}

	//glDisable(GL_BLEND);
	//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	////////////////////////////////////////
	// activate shaders and sets uniform variable values    
	glUseProgram(pg_shader_programme[pg_current_configuration_rank][_pg_shader_ParticleRender]);

	glUniformMatrix4fv(uniform_ParticleSplat_vp_proj[pg_current_configuration_rank], 1, GL_FALSE, pg_orthoWindowProjMatrix);
	glUniformMatrix4fv(uniform_ParticleSplat_vp_view[pg_current_configuration_rank], 1, GL_FALSE, pg_identityViewMatrix);
	glUniformMatrix4fv(uniform_ParticleSplat_vp_model[pg_current_configuration_rank], 1, GL_FALSE, pg_identityModelMatrix);

	// texture unit location
	// position/speed Particle update
	glUniform1i(uniform_ParticleSplat_texture_vp_Part_pos_speed[pg_current_configuration_rank], 0);
	// color/radius Particle update
	glUniform1i(uniform_ParticleSplat_texture_vp_Part_col_rad[pg_current_configuration_rank], 1);

	if (PG_PARTICLE_TYPE == 2) {
		// curve particle texture
		glUniform1i(uniform_ParticleCurve_Curve_texture_fs_decal[pg_current_configuration_rank], 2);
	}
	else if (PG_PARTICLE_TYPE == 0) {
		// blurred disk texture
		glUniform1i(uniform_ParticleSplat_texture_fs_decal[pg_current_configuration_rank], 2);
	}
	printOglError(52527);

	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// 2-cycle ping-pong pos/speed
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_ParticleAnimation_texID[(pg_FrameNo % 2) * PG_FBO_PARTICLEANIMATION_NBATTACHTS + pg_Part_pos_speed_FBO_ParticleAnimation_attcht]);

	// 2-cycle ping-pong color/radius
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_ParticleAnimation_texID[(pg_FrameNo % 2) * PG_FBO_PARTICLEANIMATION_NBATTACHTS + pg_Part_col_rad_FBO_ParticleAnimation_attcht]);

	if (PG_PARTICLE_TYPE == 2) {
		// curve particle texture
		glActiveTexture(GL_TEXTURE0 + 2);
		glBindTexture(GL_TEXTURE_2D, curve_particle_2D_texID[pg_current_configuration_rank]);
	}
	else if (PG_PARTICLE_TYPE == 0) {
		// blurred disk texture
		glActiveTexture(GL_TEXTURE0 + 2);
		if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_partSplat_texture]) {
			if (partSplat_texture > 0
				&& partSplat_texture - 1 < int(blurredDisk_texture_2D_texID[pg_current_configuration_rank].size())) {
				// printf("bind splat texture %d %d\n", partSplat_texture, blurredDisk_texture_2D_texID[pg_current_configuration_rank].at(partSplat_texture - 1));
				glBindTexture(GL_TEXTURE_2D, blurredDisk_texture_2D_texID[pg_current_configuration_rank].at(partSplat_texture - 1));
			}
			else {
				//printf("no splat texture %d\n", partSplat_texture);
				glBindTexture(GL_TEXTURE_2D, NULL_ID);
			}
		}
		else {
			if (blurredDisk_texture_2D_texID[pg_current_configuration_rank].size() > 0) {
				glBindTexture(GL_TEXTURE_2D, blurredDisk_texture_2D_texID[pg_current_configuration_rank].at(0));
			}
			else {
				glBindTexture(GL_TEXTURE_2D, NULL_ID);
			}
		}
	}
	printOglError(5259);

	////////////////////////////////////////
	// binds geometry and displays it    
	// vertex buffer for a patch, made of 4 points: 4 x 3 floats
	glBindVertexArray(pg_vaoID[pg_VAOParticle]);

	if (PG_PARTICLE_TYPE == 2) {
		// patch vertices for curve particles fed into tesselation shader
		glPatchParameteri(GL_PATCH_VERTICES, (PG_PARTICLE_CURVE_DEGREE + 1));  // number of vertices in each patch
	}

	// Index buffer for indexed rendering
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pg_vboID[pg_EAOParticle]);

	if (PG_PARTICLE_TYPE == 2) {
		// Draw the patches !
		glDrawElements(
			GL_PATCHES,      // mode
			nb_particles * (PG_PARTICLE_CURVE_DEGREE + 1),    // count
			GL_UNSIGNED_INT,   // type
			(void*)0           // element array buffer offset
		);
	}
	else {
		// Draw the patches !
		glDrawElements(
			GL_POINTS,      // mode
			nb_particles,    // count
			GL_UNSIGNED_INT,   // type
			(void*)0           // element array buffer offset
		);
	}

	// unbinds VBO
	glBindVertexArray(0);
	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// quad texture with transparency
	if (PG_PARTICLE_TYPE == 0) {
		glDisable(GL_BLEND);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

//////////////////////////////////////////////////
// PASS #4: COMPOSITION + PING-PONG ECHO: ECHOED MIX OF LAYERS

void pg_MixingPass(void) {
	if (!pg_shader_programme[pg_current_configuration_rank][_pg_shader_Mixing]) {
		return;
	}

	// outputs inside a buffer that can be used for accumulation
	if (pg_FrameNo > 0) {
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, pg_FBO_Mixing_capturedFB_prec); //  drawing memory on odd and even frames for echo
		pg_bindFBOTextures(pg_FBO_Mixing_capturedFB_prec, pg_FBO_Mixing_capturedFB_prec_texID + (pg_FrameNo % 2), 1, false, 0);
	}

	// output video buffer clean-up
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	if (isClearEcho == 1) {
		// printf("VideoPb clear echo buffer\n");
		isClearEcho = 0;
		return;
	}

	/////////////////////////////////////////////////////////
	// draws the main rectangular surface with 

	////////////////////////////////////////
	// activate shaders and sets uniform variable values    
	glUseProgram(pg_shader_programme[pg_current_configuration_rank][_pg_shader_Mixing]);
	glBindVertexArray(pg_vaoID[pg_VAOQuad]);

	glUniformMatrix4fv(uniform_Mixing_vp_proj[pg_current_configuration_rank], 1, GL_FALSE, pg_orthoWindowProjMatrix);
	glUniformMatrix4fv(uniform_Mixing_vp_view[pg_current_configuration_rank], 1, GL_FALSE, pg_identityViewMatrix);
	glUniformMatrix4fv(uniform_Mixing_vp_model[pg_current_configuration_rank], 1, GL_FALSE, pg_identityModelMatrix);

	// texture unit location
	glUniform1i(uniform_Mixing_texture_fs_CA[pg_current_configuration_rank], pg_CA_FBO_Mixing_sampler);
	glUniform1i(uniform_Mixing_texture_fs_ClipArt_render[pg_current_configuration_rank], pg_ClipArt_render_FBO_Mixing_sampler);
	glUniform1i(uniform_Mixing_texture_fs_Particle_render[pg_current_configuration_rank], pg_Particle_render_FBO_Mixing_sampler);
	glUniform1i(uniform_Mixing_texture_fs_Render_prec[pg_current_configuration_rank], pg_Render_prec_FBO_Mixing_sampler);

	glUniform1i(uniform_Mixing_texture_fs_Screen_Font[pg_current_configuration_rank], pg_Screen_Font_FBO_Mixing_sampler);
	glUniform1i(uniform_Mixing_texture_fs_Screen_Message[pg_current_configuration_rank], pg_Screen_Message_FBO_Mixing_sampler);

	glUniform1i(uniform_Mixing_texture_fs_Trk0[pg_current_configuration_rank], pg_Trk0_FBO_Mixing_sampler);
	glUniform1i(uniform_Mixing_texture_fs_Trk1[pg_current_configuration_rank], pg_Trk1_FBO_Mixing_sampler);
	glUniform1i(uniform_Mixing_texture_fs_Trk2[pg_current_configuration_rank], pg_Trk2_FBO_Mixing_sampler);
	glUniform1i(uniform_Mixing_texture_fs_Trk3[pg_current_configuration_rank], pg_Trk3_FBO_Mixing_sampler);

	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// 2-cycle ping-pong CA step n + 1 (FBO attachment 0) -- next frame (outout from update pass)
	glActiveTexture(GL_TEXTURE0 + pg_CA_FBO_Mixing_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * PG_FBO_UPDATE_NBATTACHTS + pg_CA_FBO_Update_attcht]);

	// ClipArt GPU step n
	glActiveTexture(GL_TEXTURE0 + pg_ClipArt_render_FBO_Mixing_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_ClipArt_render_texID);
	// Particles step n
	glActiveTexture(GL_TEXTURE0 + pg_Particle_render_FBO_Mixing_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Particle_render_texID);

	// preceding snapshot for echo
	glActiveTexture(GL_TEXTURE0 + pg_Render_prec_FBO_Mixing_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Mixing_capturedFB_prec_texID[((pg_FrameNo + 1) % 2)]);  // drawing memory on odd and even frames for echo 

	// screen font texture
	glActiveTexture(GL_TEXTURE0 + pg_Screen_Font_FBO_Mixing_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, Screen_Font_texture_Rectangle_texID);

	// message offsets texture
	glActiveTexture(GL_TEXTURE0 + pg_Screen_Message_FBO_Mixing_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_screenMessageBitmap_texID);

	// 2-cycle ping-pong track 0 (FBO attachment 3) -- next frame (outout from update pass)
	glActiveTexture(GL_TEXTURE0 + pg_Trk0_FBO_Mixing_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * PG_FBO_UPDATE_NBATTACHTS + pg_Trk0_FBO_Update_attcht]);

	// 2-cycle ping-pong track 1 (FBO attachment 4) -- next frame (outout from update pass)
	glActiveTexture(GL_TEXTURE0 + pg_Trk1_FBO_Mixing_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * PG_FBO_UPDATE_NBATTACHTS + pg_Trk1_FBO_Update_attcht]);

	// 2-cycle ping-pong track 2 step n + 1 (FBO attachment 5) -- next frame (outout from update pass)
	glActiveTexture(GL_TEXTURE0 + pg_Trk2_FBO_Mixing_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * PG_FBO_UPDATE_NBATTACHTS + pg_Trk2_FBO_Update_attcht]);

	// 2-cycle ping-pong track 3 step n + 1 (FBO attachment 6) -- next frame (outout from update pass)
	glActiveTexture(GL_TEXTURE0 + pg_Trk3_FBO_Mixing_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * PG_FBO_UPDATE_NBATTACHTS + pg_Trk3_FBO_Update_attcht]);

	//if (pg_FrameNo % 1000 <= 1) {
	//	printf("Comp check texID 0-7 %d %d %d %d %d %d %d %d\n",
	//		pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * PG_FBO_UPDATE_NBATTACHTS],
	//		pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * PG_FBO_UPDATE_NBATTACHTS + 2],
	//		pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * PG_FBO_UPDATE_NBATTACHTS + 3],
	//		pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * PG_FBO_UPDATE_NBATTACHTS + 4],
	//		pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * PG_FBO_UPDATE_NBATTACHTS + 5],
	//		pg_FBO_Mixing_capturedFB_prec_texID[((pg_FrameNo + 1) % 2)],
	//		Screen_Font_texture_Rectangle_texID,
	//		pg_screenMessageBitmap_texID);
	//}

	// draw points from the currently bound VAO with current in-use shader
	// glDrawArrays (GL_TRIANGLES, 0, 3 * PG_SIZE_QUAD_ARRAY);
	// Index buffer for indexed rendering
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pg_vboID[pg_EABQuad]);
	// Draw the triangles !
	glDrawElements(
		GL_TRIANGLE_STRIP,      // mode
		3 * PG_SIZE_QUAD_ARRAY,    // count
		GL_UNSIGNED_INT,   // type
		(void*)0           // element array buffer offset
	);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

//////////////////////////////////////////////////
// PASS #5: FINAL DISPLAY: MIX OF ECHOED AND NON-ECHOED LAYERS

void pg_MasterPass(void) {
	if (!pg_shader_programme[pg_current_configuration_rank][_pg_shader_Master]) {
		return;
	}
	// Augmented Reality: FBO capture of Master to be displayed on a mesh
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_activeMeshes]
		&& ((pg_FullScenarioActiveVars[pg_current_configuration_rank][_augmentedReality] && augmentedReality)
			|| (pg_FullScenarioActiveVars[pg_current_configuration_rank][_meshRenderBypass] && meshRenderBypass))) {
		if (pg_FrameNo > 0) {
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, pg_FBO_Master_capturedFB_prec); //  master output memory for mapping on mesh or bypassing mesh rendering
		}
	}
	else {
		// unbind output FBO 
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	}

	// sets viewport to double window
	glViewport(0, 0, window_width, window_height);

	// output video buffer clean-up
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	////////////////////////////////////////
	// activate shaders and sets uniform variable values    
	if (pg_shader_programme[pg_current_configuration_rank][_pg_shader_Master]) {
		glUseProgram(pg_shader_programme[pg_current_configuration_rank][_pg_shader_Master]);
		glBindVertexArray(pg_vaoID[pg_VAOQuadMaster]);

		glUniformMatrix4fv(uniform_Master_vp_proj[pg_current_configuration_rank], 1, GL_FALSE, doubleProjMatrix);
		glUniformMatrix4fv(uniform_Master_vp_view[pg_current_configuration_rank], 1, GL_FALSE, pg_identityViewMatrix);
		glUniformMatrix4fv(uniform_Master_vp_model[pg_current_configuration_rank], 1, GL_FALSE, pg_identityModelMatrix);

		glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// texture unit locations
		glUniform1i(uniform_Master_texture_fs_Render_curr[pg_current_configuration_rank], pg_Render_curr_FBO_Master_sampler);
		glUniform1i(uniform_Master_texture_fs_CA[pg_current_configuration_rank], pg_CA_FBO_Master_sampler);

		glUniform1i(uniform_Master_texture_fs_ClipArt_render[pg_current_configuration_rank], pg_ClipArt_render_FBO_Master_sampler);
		glUniform1i(uniform_Master_texture_fs_Particle_render[pg_current_configuration_rank], pg_Particle_render_FBO_Master_sampler);
		glUniform1i(uniform_Master_texture_fs_Trk0[pg_current_configuration_rank], pg_Trk0_FBO_Master_sampler);
		glUniform1i(uniform_Master_texture_fs_Trk1[pg_current_configuration_rank], pg_Trk1_FBO_Master_sampler);
		glUniform1i(uniform_Master_texture_fs_Trk2[pg_current_configuration_rank], pg_Trk2_FBO_Master_sampler);
		glUniform1i(uniform_Master_texture_fs_Trk3[pg_current_configuration_rank], pg_Trk3_FBO_Master_sampler);

		glUniform1i(uniform_Master_texture_fs_Mask[pg_current_configuration_rank], pg_Mask_FBO_Master_sampler);

		// Mixing pass output (echoed composition of tracks)
		glActiveTexture(GL_TEXTURE0 + pg_Render_curr_FBO_Master_sampler);
		glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Mixing_capturedFB_prec_texID[(pg_FrameNo % 2)]);

		// 2-cycle ping-pong CA step n (FBO attachment 0) -- next frame (outout from update pass)
		glActiveTexture(GL_TEXTURE0 + pg_CA_FBO_Master_sampler);
		glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * PG_FBO_UPDATE_NBATTACHTS + pg_CA_FBO_Update_attcht]);

		// ClipArt GPU step n 
		glActiveTexture(GL_TEXTURE0 + pg_ClipArt_render_FBO_Master_sampler);
		if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_activeClipArts]) {
			glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_ClipArt_render_texID);
		}
		else {
			glBindTexture(GL_TEXTURE_RECTANGLE, NULL_ID);
		}

		// Particles step n 
		glActiveTexture(GL_TEXTURE0 + pg_Particle_render_FBO_Master_sampler);
		glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Particle_render_texID);

		// 2-cycle ping-pong track 0 step n (FBO attachment 3) -- next frame (outout from update pass)
		glActiveTexture(GL_TEXTURE0 + pg_Trk0_FBO_Master_sampler);
		glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * PG_FBO_UPDATE_NBATTACHTS + pg_Trk0_FBO_Update_attcht]);

		// 2-cycle ping-pong track 1 step n (FBO attachment 4) -- next frame (outout from update pass)
		glActiveTexture(GL_TEXTURE0 + pg_Trk1_FBO_Master_sampler);
		glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * PG_FBO_UPDATE_NBATTACHTS + pg_Trk1_FBO_Update_attcht]);

		// 2-cycle ping-pong track 2 step n (FBO attachment 5) -- next frame (outout from update pass)
		glActiveTexture(GL_TEXTURE0 + pg_Trk2_FBO_Master_sampler);
		glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * PG_FBO_UPDATE_NBATTACHTS + pg_Trk2_FBO_Update_attcht]);

		// 2-cycle ping-pong track 3 step n (FBO attachment 6) -- next frame (outout from update pass)
		glActiveTexture(GL_TEXTURE0 + pg_Trk3_FBO_Master_sampler);
		glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * PG_FBO_UPDATE_NBATTACHTS + pg_Trk3_FBO_Update_attcht]);

		// Master mask texture
		glActiveTexture(GL_TEXTURE0 + pg_Mask_FBO_Master_sampler);
		if (nb_layers_master_mask[pg_current_configuration_rank] > 0) {
			//printf("multilayer mask texture %d (%d layers)\n", Master_Multilayer_Mask_texID[pg_current_configuration_rank], nb_layers_master_mask[pg_current_configuration_rank]);
			glBindTexture(GL_TEXTURE_3D, Master_Multilayer_Mask_texID[pg_current_configuration_rank]);
		}
		else {
			//printf("single layer mask texture %d\n", Master_Mask_texID[pg_current_configuration_rank]);
			glBindTexture(GL_TEXTURE_RECTANGLE, Master_Mask_texID[pg_current_configuration_rank]);
		}

		//if (pg_FrameNo % 1000 <= 1) {
		//	printf("Final check texID 0-5 %d %d %d %d %d %d\n\n",
		//		pg_FBO_Mixing_capturedFB_prec_texID[(pg_FrameNo % 2)],
		//		pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * PG_FBO_UPDATE_NBATTACHTS],
		//		pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * PG_FBO_UPDATE_NBATTACHTS + 2],
		//		pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * PG_FBO_UPDATE_NBATTACHTS + 3],
		//		pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * PG_FBO_UPDATE_NBATTACHTS + 4],
		//		pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * PG_FBO_UPDATE_NBATTACHTS + 5]);
		//		pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * PG_FBO_UPDATE_NBATTACHTS + 5]);
		//}

		// draw points from the currently bound VAO with current in-use shader
		// glDrawArrays (GL_TRIANGLES, 0, 3 * PG_SIZE_QUAD_ARRAY);
		// Index buffer for indexed rendering
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pg_vboID[pg_EABQuadMaster]);
		// Draw the triangles !
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawElements(
			GL_TRIANGLE_STRIP,      // mode
			3 * PG_SIZE_QUAD_ARRAY,    // count
			GL_UNSIGNED_INT,   // type
			(void*)0           // element array buffer offset
		);
	}
}

//////////////////////////////////////////////////
// PASS #5: SENSOR PASS
void pg_SensorPass(void) {
	if (!pg_FullScenarioActiveVars[pg_current_configuration_rank][_sensor_layout]
		|| !pg_shader_programme[pg_current_configuration_rank][_pg_shader_Sensor]) {
		return;
	}
	////////////////////////////////////////
	// drawing sensors
	// activate transparency
	//printf("sensor pass\n");
	glEnable(GL_BLEND);
	glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

	// activate shaders and sets uniform variable values    
	glUseProgram(pg_shader_programme[pg_current_configuration_rank][_pg_shader_Sensor]);
	glBindVertexArray(quadSensor_vao);

	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// texture unit location
	glUniform1i(uniform_Sensor_texture_fs_decal[pg_current_configuration_rank], 0);
	// previous pass output
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_RECTANGLE, Sensor_texture_rectangle[pg_current_configuration_rank]);

	glUniformMatrix4fv(uniform_Sensor_vp_view[pg_current_configuration_rank], 1, GL_FALSE, pg_identityViewMatrix);
	glUniformMatrix4fv(uniform_Sensor_vp_proj[pg_current_configuration_rank], 1, GL_FALSE, doubleProjMatrix);
	printOglError(597);

	for (int indSens = 0; indSens < PG_NB_SENSORS; indSens++) {
		int reindexed_Sensor = Sensor_order[indSens];
		if (sensor_onOff[reindexed_Sensor]) {
			//printf("sensor index %d reindex %d\n", indSens, reindexed_Sensor);
			modelMatrixSensor[12] = (sensorPositions[3 * reindexed_Sensor] + 0.5f - workingWindow_width / 2.0f) + workingWindow_width / 2.0f;
			modelMatrixSensor[13] = (sensorPositions[3 * reindexed_Sensor + 1] + 0.5f - window_height / 2.0f) + window_height / 2.0f;
			modelMatrixSensor[14] = sensorPositions[3 * reindexed_Sensor + 2];
			//printf("left sensor %d %.3f %.3f %.3f\n", indSens, modelMatrixSensor[12], modelMatrixSensor[13], modelMatrixSensor[14]);
			glUniformMatrix4fv(uniform_Sensor_vp_model[pg_current_configuration_rank], 1, GL_FALSE, modelMatrixSensor);

			// sensorLevel[indSens]
			glUniform4f(uniform_Sensor_fs_4fv_onOff_isCurrentSensor_masterLevel_transparency[pg_current_configuration_rank],
				(sensor_onOff[reindexed_Sensor] ? 1.0f : -1.0f), (reindexed_Sensor == currentSensor ? 1.0f : -1.0f),
				master, sensorLevel[reindexed_Sensor]);

			// draw points from the currently bound VAO with current in-use shader
			glDrawArrays(GL_TRIANGLES, 0, 3 * 2);
		}
	}
	printOglError(596);

	// duplicates the sensors in case of double window
	if (double_window) {
		int rightWindowVMargin = 0;
		if (window_width > 1920) {
			rightWindowVMargin = (window_width - 2 * workingWindow_width) / 2;
		}
		for (int indSens = 0; indSens < PG_NB_SENSORS; indSens++) {
			int reindexed_Sensor = Sensor_order[indSens];
			if (sensor_onOff[reindexed_Sensor]) {
				modelMatrixSensor[12] = (sensorPositions[3 * reindexed_Sensor] + 0.5f - workingWindow_width / 2.0f) + workingWindow_width / 2.0f
					+ rightWindowVMargin + workingWindow_width;
				modelMatrixSensor[13] = (sensorPositions[3 * reindexed_Sensor + 1] + 0.5f - window_height / 2.0f) + window_height / 2.0f;
				modelMatrixSensor[14] = sensorPositions[3 * reindexed_Sensor + 2];
				//printf("right sensor %d %.3f %.3f %.3f\n", indSens, modelMatrixSensor[12], modelMatrixSensor[13], modelMatrixSensor[14]);
				glUniformMatrix4fv(uniform_Sensor_vp_model[pg_current_configuration_rank], 1, GL_FALSE, modelMatrixSensor);

				glUniform4f(uniform_Sensor_fs_4fv_onOff_isCurrentSensor_masterLevel_transparency[pg_current_configuration_rank],
					(sensor_onOff[reindexed_Sensor] ? 1.0f : -1.0f), (reindexed_Sensor == currentSensor ? 1.0f : -1.0f),
					master, sensorLevel[reindexed_Sensor]);

				// draw points from the currently bound VAO with current in-use shader
				glDrawArrays(GL_TRIANGLES, 0, 3 * 2);
			}
		}
	}

	printOglError(595);
	glDisable(GL_BLEND);
}

// initializes the homography matrices for the distortion of the projected image
void pg_calculate_homography_matrices(std::vector<cv::Point2f> *sourcePoints, std::vector<cv::Point2f> *destinationPoints, GLfloat matValues[], int dim) {
	//////////////////////////////////////////////////////////////////////////////////
	// right camera (left display)

	///////////////////////// HOMOGRPAHY
	// This homography transforms the points projected at near=1 into the four
	// points measured experimetally corresponding to the 4 vertices of the
	// quad projected at 1m with the projector axis orthogonal to the projection plane

	homography = cv::findHomography(*sourcePoints, *destinationPoints, 0);
	//printf("mat size %d %d\n", homography.size().width, homography.size().height);
	// in GLM Matrix types store their values in column - major order.
	 if (dim == 4) {
		 matValues[0] = (float)((double*)homography.data)[0];
		 matValues[1] = (float)((double*)homography.data)[3];
		 matValues[2] = 0.0;
		 matValues[3] = (float)((double*)homography.data)[6];

		 matValues[4] = (float)((double*)homography.data)[1];
		 matValues[5] = (float)((double*)homography.data)[4];
		 matValues[6] = 0.0;
		 matValues[7] = (float)((double*)homography.data)[7];

		 matValues[8] = 0.0;
		 matValues[9] = 0.0;
		 matValues[10] = 0.0;
		 matValues[11] = 0.0;

		 matValues[12] = (float)((double*)homography.data)[2];
		 matValues[13] = (float)((double*)homography.data)[5];
		 matValues[14] = 0.0;
		 matValues[15] = (float)((double*)homography.data)[8];
	 }
	 else if (dim == 3) {
		 matValues[0] = (float)((double*)homography.data)[0];
		 matValues[1] = (float)((double*)homography.data)[3];
		 matValues[2] = (float)((double*)homography.data)[6];

		 matValues[3] = (float)((double*)homography.data)[1];
		 matValues[4] = (float)((double*)homography.data)[4];
		 matValues[5] = (float)((double*)homography.data)[7];

		 matValues[6] = (float)((double*)homography.data)[2];
		 matValues[7] = (float)((double*)homography.data)[5];
		 matValues[8] = (float)((double*)homography.data)[8];
	 }
	 else {
		 sprintf(ErrorStr, "homography dimension should be 3 or 4, not %d", dim); ReportError(ErrorStr);
	 }
}

// initializes the transformation matrices related to the frustum
// should only be called when the values are changed through pd or maxmsp
void pg_calculate_perspective_matrices(void) {
	//printf("[1] Loc %.1f %.1f %.1f LookAt %.1f %.1f %.1f\n",
	//	VP1LocX, VP1LocY, VP1LocZ, VP1LookAtX, VP1LookAtY, VP1LookAtZ);

	if (!pg_FullScenarioActiveVars[pg_current_configuration_rank][_VP1LocX]
		|| !pg_shader_programme[pg_current_configuration_rank][_VP1LocY]
		|| !pg_shader_programme[pg_current_configuration_rank][_VP1LocZ]
		|| !pg_shader_programme[pg_current_configuration_rank][_VP1LookAtX]
		|| !pg_shader_programme[pg_current_configuration_rank][_VP1LookAtY]
		|| !pg_shader_programme[pg_current_configuration_rank][_VP1LookAtZ]
		|| !pg_shader_programme[pg_current_configuration_rank][_VP1UpY]
		|| !pg_shader_programme[pg_current_configuration_rank][_VP1Reversed]
		|| !pg_shader_programme[pg_current_configuration_rank][_VP1WidthTopAt1m]
		|| !pg_shader_programme[pg_current_configuration_rank][_VP1BottomAt1m]
		|| !pg_shader_programme[pg_current_configuration_rank][_VP1TopAt1m]
		|| !pg_shader_programme[pg_current_configuration_rank][_nearPlane]
		|| !pg_shader_programme[pg_current_configuration_rank][_farPlane]) {
		return;
	}

	//////////////////////////////////////////////////////////////////////////////////
	// right camera (right display)
	if (double_window) {
		VP1perspMatrix
			= glm::frustum(-VP1WidthTopAt1m / 2.0f, VP1WidthTopAt1m / 2.0f, VP1BottomAt1m, VP1TopAt1m, nearPlane, farPlane);
	}
	else {
		VP1perspMatrix
			= glm::frustum(-VP1WidthTopAt1m / 2.0f, VP1WidthTopAt1m / 2.0f, VP1BottomAt1m, VP1TopAt1m, nearPlane, farPlane);
	}
	//printf("Perspective 1 %.2f %.2f %.2f %.2f near Far %.2f %.2f\n" , -VP1WidthTopAt1m / 2.0f, VP1WidthTopAt1m / 2.0f, VP1BottomAt1m, VP1TopAt1m, nearPlane, farPlane);

	// Camera matrix
	// float eyePosition[3] = {10.1f,4.6f,4.4f};
	// the projection of the bottom of the screen is known
	// we have to deduce the look at from this point && the measure of 
	// the projector calibration
	// the look at has to be recalculated because the targe point is above the axis of projection
	// by an angle alpha
	//printf("VP1BottomAt1m %.2f\n" , VP1BottomAt1m );

	float VP1alpha = atan(VP1BottomAt1m);
	glm::vec4 vectorVPPositionToScreenBottom
		= glm::vec4(VP1LookAtX - VP1LocX, VP1LookAtY - VP1LocY, VP1LookAtZ - VP1LocZ, 0.0);
	// printf("vectorLookAt init %.2f %.2f %.2f\n" , vectorVPPositionToScreenBottom.x , vectorVPPositionToScreenBottom.y , vectorVPPositionToScreenBottom.z );
	glm::mat4 rotationMat(1); // Creates a identity matrix
	// rotation axis is k x vectorVPPositionToScreenBottom (or -k if projector is upside down)
	glm::vec3 rotationAxis = glm::cross(glm::vec3(0, 0, (VP1Reversed ? -1.0f : 1.0f)), glm::vec3(vectorVPPositionToScreenBottom));
	rotationAxis = glm::normalize(rotationAxis);
	// printf("rotationAxis %.2f %.2f %.2f\n", rotationAxis.x, rotationAxis.y, rotationAxis.z);
	// shifts towards the bottom the actual lookat center according to VP1BottomAt1m
	rotationMat = glm::rotate(rotationMat, VP1alpha, rotationAxis); // (VP1Reversed?-1.0f:1.0f) *
	glm::vec3 vectorLookAt = glm::vec3(rotationMat * vectorVPPositionToScreenBottom);
	// printf("vectorLookAt real %.2f %.2f %.2f\n" , vectorLookAt.x , vectorLookAt.y , vectorLookAt.z );
	glm::vec3 lookAtPoint = glm::vec3(VP1LocX, VP1LocY, VP1LocZ) + vectorLookAt;
	//printf("VP1Loc %.2f %.2f %.2f\n", VP1LocX, VP1LocY, VP1LocZ);
	//printf("Look at real %.2f %.2f %.2f\n\n", lookAtPoint.x, lookAtPoint.y, lookAtPoint.z);
	VP1viewMatrix
		= glm::lookAt(
			glm::vec3(VP1LocX, VP1LocY, VP1LocZ), // Camera is at (VP1LocX, VP1LocY, VP1LocZ), in World Space
			lookAtPoint, // && looks at lookAtPoint
			glm::vec3(0, VP1UpY, (VP1Reversed ? -1.0f : 1.0f))  // Head is up (set to 0, VP1UpY, 1 or 0, VP1UpY, -1 if projector is upside down)
		);

#if defined(var_VP2WidthTopAt1m) && defined(var_VP2BottomAt1m) && defined(var_VP2TopAt1m) && defined(var_nearPlane) && defined(var_farPlane)
	//////////////////////////////////////////////////////////////////////////////////
	// left camera (right display)
	VP2perspMatrix
		= glm::frustum(-VP2WidthTopAt1m / 2.0f, VP2WidthTopAt1m / 2.0f, VP2BottomAt1m, VP2TopAt1m, nearPlane, farPlane);

	//printf("Perspective 1 %.2f %.2f %.2f %.2f\n" , -VP2WidthTopAt1m / 2.0f, VP2WidthTopAt1m / 2.0f, VP2BottomAt1m, VP2TopAt1m );

	// Camera matrix
	// float eyePosition[3] = {10.1f,4.6f,4.4f};
	// the projection of the bottom of the screen is known
	// we have to deduce the look at from this point and the measure of 
	// the projector calibration
	// the look at has to be recalculated because the targe point is above the axis of projection
	// by an angle alpha
	//printf("VP2BottomAt1m %.2f\n" , VP2BottomAt1m );

	float VP2alpha = atan(VP2BottomAt1m);
	vectorVPPositionToScreenBottom
		= glm::vec4(VP2LookAtX - VP2LocX, VP2LookAtY - VP2LocY, VP2LookAtZ - VP2LocZ, 0.0);
	// printf("vectorLookAt init %.2f %.2f %.2f\n" , vectorVPPositionToScreenBottom.x , vectorVPPositionToScreenBottom.y , vectorVPPositionToScreenBottom.z );
	// rotation axis is k x vectorVPPositionToScreenBottom (or -k if projector is upside down)
	rotationAxis = glm::cross(glm::vec3(0, 0, (VP2Reversed ? -1.0f : 1.0f)), glm::vec3(vectorVPPositionToScreenBottom));
	rotationAxis = glm::normalize(rotationAxis);
	// shifts towards the bottom the actual lookat center according to VP2BottomAt1m
	rotationMat = glm::rotate(glm::mat4(1.0f), VP2alpha, rotationAxis); // (VP2Reversed?-1.0f:1.0f) *
	vectorLookAt = glm::vec3(rotationMat * vectorVPPositionToScreenBottom);
	// printf("vectorLookAt real %.2f %.2f %.2f\n" , vectorLookAt.x , vectorLookAt.y , vectorLookAt.z );
	lookAtPoint = glm::vec3(VP2LocX, VP2LocY, VP2LocZ) + vectorLookAt;
	// printf("Look at real %.2f %.2f %.2f\n\n" , lookAtPoint.x , lookAtPoint.y , lookAtPoint.z );
	VP2viewMatrix
		= glm::lookAt(
			glm::vec3(VP2LocX, VP2LocY, VP2LocZ), // Camera is at (VP2LocX, VP2LocY, VP2LocZ), in World Space
			lookAtPoint, // and looks at lookAtPoint
			glm::vec3(0, VP2UpY, (VP2Reversed ? -1.0f : 1.0f))  // Head is up (set to 0, VP2UpY, 1 or 0, VP2UpY, -1 if projector is upside down)
		);
#endif
}

void pg_calculate_orthographic_matrices(void) {
	//printf("[1] Loc %.1f %.1f %.1f LookAt %.1f %.1f %.1f\n",
	//	VP1LocX, VP1LocY, VP1LocZ, VP1LookAtX, VP1LookAtY, VP1LookAtZ);

	if (!pg_FullScenarioActiveVars[pg_current_configuration_rank][_VP1LocX]
		|| !pg_shader_programme[pg_current_configuration_rank][_VP1LocY]
		|| !pg_shader_programme[pg_current_configuration_rank][_VP1LocZ]
		|| !pg_shader_programme[pg_current_configuration_rank][_VP1LookAtX]
		|| !pg_shader_programme[pg_current_configuration_rank][_VP1LookAtY]
		|| !pg_shader_programme[pg_current_configuration_rank][_VP1LookAtZ]
		|| !pg_shader_programme[pg_current_configuration_rank][_VP1UpY]
		|| !pg_shader_programme[pg_current_configuration_rank][_VP1Reversed]
		|| !pg_shader_programme[pg_current_configuration_rank][_VP1WidthTopAt1m]
		|| !pg_shader_programme[pg_current_configuration_rank][_VP1BottomAt1m]
		|| !pg_shader_programme[pg_current_configuration_rank][_VP1TopAt1m]
		|| !pg_shader_programme[pg_current_configuration_rank][_nearPlane]
		|| !pg_shader_programme[pg_current_configuration_rank][_farPlane]) {
		return;
	}

	//////////////////////////////////////////////////////////////////////////////////
	// right camera (right display)
	if (double_window) {
		VP1perspMatrix
			= glm::ortho(-VP1WidthTopAt1m / 2.0f, VP1WidthTopAt1m / 2.0f, VP1BottomAt1m, VP1TopAt1m, nearPlane, farPlane);
	}
	else {
		VP1perspMatrix
			= glm::ortho(-VP1WidthTopAt1m / 2.0f, VP1WidthTopAt1m / 2.0f, VP1BottomAt1m, VP1TopAt1m, nearPlane, farPlane);
	}
	//printf("Perspective 1 %.2f %.2f %.2f %.2f near Far %.2f %.2f\n" , -VP1WidthTopAt1m / 2.0f, VP1WidthTopAt1m / 2.0f, VP1BottomAt1m, VP1TopAt1m, nearPlane, farPlane);

	// Camera matrix
	//printf("Look at real %.2f %.2f %.2f\n\n", lookAtPoint.x, lookAtPoint.y, lookAtPoint.z);
	glm::mat4 m4(1.0f);
	VP1viewMatrix = m4;


#if defined(var_VP2WidthTopAt1m) && defined(var_VP2BottomAt1m) && defined(var_VP2TopAt1m) && defined(var_nearPlane) && defined(var_farPlane)
	// TODO
#endif
}

//////////////////////////////////////////////////
// PASS #6: MESH PASS
// MESH ANIMATIONS FOR PROJECTS
bool Etoiles_mesh_guided_by_strokes(int indMeshFile) {
	// meshes are guided by strokes
	int path_no = indMeshFile + 1;
	bool visible = false;

	// brings the path coordinates to the normal cube
	if (path_no <= PG_NB_PATHS) {
		visible = (is_path_replay[path_no] && paths_x[path_no] > 0 && paths_y[path_no] > 0);
		if (visible) {
			// normal pen coordinates
			pen_x = (paths_x[path_no] / workingWindow_width) * 2.f - 1.f;
			pen_y = (paths_y[path_no] / window_height) * 2.f - 1.f;
		}
		else {
			visible = path_record[path_no] && paths_x[0] > 0 && paths_y[0] > 0;
			if (visible) {
				// normal pen coordinates
				pen_x = (paths_x[0] / workingWindow_width) * 2.f - 1.f;
				pen_y = (paths_y[0] / window_height) * 2.f - 1.f;
			}
		}
	}
	return visible;
}


void Etoiles_ray_animation(int indMeshFile) {
	// rotates and scales a ray so that it follows a pen
	// vector from ray center to pen
	vec_x = pen_x + pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_Mesh_Translation_X;
	vec_y = pen_y + pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_Mesh_Translation_Y;
	// angle from ray center to pen -> ray angle
	if (vec_x != 0) {
		pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_Mesh_Rotation_angle = atan(vec_y / vec_x);
	}
	else {
		pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_Mesh_Rotation_angle = 0;
	}
	if (vec_x > 0) {
		pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_Mesh_Rotation_angle += float(M_PI);
	}
	// ray size so that the tip of the ray coincides with the pen
	float norm_vec = sqrt(vec_x * vec_x + vec_y * vec_y);
	pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_Mesh_Scale = norm_vec;
}


#if defined(var_Caverne_Mesh_Profusion)
void Caverne_profusion_automatic_rotation(int indMeshFile) {
	// rotation update
	if (indMeshFile < 7) {
		pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_Mesh_Rotation_angle += 0.03f;
		pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_Mesh_Rotation_X += 0.01f;
		pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_Mesh_Rotation_Y += 0.01f;
		pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_Mesh_Rotation_Z += 0.01f;
		// translation update
		if (mobileMeshes & (1 << indMeshFile)) {
			pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_Mesh_Translation_X += pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_Mesh_Motion_X * 2.f;
			pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_Mesh_Translation_Y += pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_Mesh_Motion_Y * 2.f;
			pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_Mesh_Translation_Z += pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_Mesh_Motion_Z * 2.f;
		}
	}
	else {
		pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_Mesh_Rotation_angle += 0.06f * rand_0_1;
		pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_Mesh_Rotation_X += (0.2f * rand_0_1 - 0.1f);
		pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_Mesh_Rotation_Y += (0.2f * rand_0_1 - 0.1f);
		pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_Mesh_Rotation_Z += (0.2f * rand_0_1 - 0.1f);
		// translation update
		pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_Mesh_Translation_X += pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_Mesh_Motion_X * 4.f * (rand_0_1 - 0.5f);
		pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_Mesh_Translation_Y += pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_Mesh_Motion_Y * 4.f * (rand_0_1 - 0.5f);
		pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_Mesh_Translation_Z += pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_Mesh_Motion_Z * 4.f * (rand_0_1 - 0.5f);
	}
}
#elif defined(var_MmeShanghai_brokenGlass)
void MmeShanghai_automatic_brokenGlass_animation(int indMeshFile, int indObjectInMesh) {
	float animationTime = float(pg_CurrentClockTime - pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_MmeShanghaiMeshObjectWakeupTime[indObjectInMesh]);
	pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_MmeShanghai_Object_Translation_Y[indObjectInMesh] = -0.0065f * animationTime * animationTime;
	pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_MmeShanghai_Object_Rotation_angle[indObjectInMesh] += pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_MmeShanghai_Object_Rotation_Ini_angle[indObjectInMesh];
}
#endif

void pg_drawOneMesh(int indMeshFile) {
	// visibility
	bool visible = false;
#if defined(pg_Project_Etoiles)
	visible = Etoiles_mesh_guided_by_strokes(indMeshFile);
#elif defined(var_Caverne_Mesh_Profusion)
	visible = (indMeshFile < 7 && (activeMeshes & (1 << indMeshFile))) || (pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_CaverneActveMesh
		&& (pg_CurrentClockTime - pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_CaverneMeshBirthTime > pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_CaverneMeshWakeupTime)
		&& (pg_CurrentClockTime < pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_CaverneMeshDeathTime));
#else
	visible = (activeMeshes & (1 << indMeshFile));
#endif

	// visible mesh
	if (visible) {
		// mesh animation
#if defined(pg_Project_Etoiles)
		Etoiles_ray_animation();
#elif defined(var_Caverne_Mesh_Profusion)
		Caverne_profusion_automatic_rotation(indMeshFile);
#endif

#if defined(var_Contact_mesh_anime)
		if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_Contact_mesh_anime]) {
			glm::f32* valMats = new glm::f32[16 * pg_nb_bones[indMeshFile]];
			for (int indBone = 0; indBone < pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_nb_bones; indBone++) {
				memcpy(&valMats[16 * indBone],
					glm::value_ptr(pg_Meshes[pg_current_configuration_rank][indMeshFile].TabBones[indBone].pointAnimationMatrix),
					16 * sizeof(glm::f32));
			}
			glUniformMatrix4fv(uniform_Mesh_bones_matrices[pg_current_configuration_rank], 20, GL_FALSE, valMats);
			delete[] valMats;
		}
#endif

		// Model matrix 
		// transformed mesh according to scenario file
		// 1. a varying translation matrix
		MeshPosModelMatrix = glm::translate(glm::mat4(1.0f),
			glm::vec3(pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_Mesh_Translation_X, pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_Mesh_Translation_Y, pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_Mesh_Translation_Z));
		//printf("Mesh translation ind Mesh %d tr %.2f %.2f %.2f \n",indMeshFile, 
			//pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_Mesh_Translation_X, pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_Mesh_Translation_Y, pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_Mesh_Translation_Z);
		// 2. a varying rotation matrix 
		glm::vec3 myRotationAxis(pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_Mesh_Rotation_X,
			pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_Mesh_Rotation_Y, pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_Mesh_Rotation_Z);
#if defined(var_Contact_mesh_rotation)
		if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_Contact_mesh_rotation]) {
			MeshPosModelMatrix = glm::translate(MeshPosModelMatrix, glm::vec3(0.f, -15.f, 0.f));
		}
#endif
		MeshPosModelMatrix = glm::rotate(MeshPosModelMatrix, pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_Mesh_Rotation_angle, myRotationAxis);
#if defined(var_Contact_mesh_rotation)
		if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_Contact_mesh_rotation]) {
			MeshPosModelMatrix = glm::translate(MeshPosModelMatrix, glm::vec3(0.f, 15.f, 0.f));
		}
#endif
		// 3. a varying scaling matrix 
#if defined(var_Caverne_Mesh_Profusion)
		if (indMeshFile < 7) {
			MeshPosModelMatrix = glm::scale(MeshPosModelMatrix, glm::vec3(pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_Mesh_Scale));
		}
		else {
			MeshPosModelMatrix = glm::scale(MeshPosModelMatrix, glm::vec3(pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_Mesh_Scale * min(2.0f, (pg_CurrentClockTime - pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_CaverneMeshBirthTime) / 20.f)));
		}
#else
		MeshPosModelMatrix = glm::scale(MeshPosModelMatrix, glm::vec3(pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_Mesh_Scale));
#endif

		// model matrix transfered to GPU (if it is not object by object made)
		glUniformMatrix4fv(uniform_Mesh_vp_model[pg_current_configuration_rank], 1, GL_FALSE,
			glm::value_ptr(MeshPosModelMatrix));

#if defined(var_MmeShanghai_brokenGlass)
		if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_MmeShanghai_brokenGlass]) {
			if (indMeshFile == 0 /* wall (broken glass on the right panel) */) {
				glUniform4f(uniform_Mesh_fs_4fv_color[pg_current_configuration_rank], 0, 0, 0, 1.0f);
				glUniform4f(uniform_Mesh_fs_4fv_color_master_photo_weight_bg[pg_current_configuration_rank], 0, 0, 1.f, MS_BLACK);
			}
			else if (indMeshFile == 2 /* WHITE_RIGHT */) {
				glUniform4f(uniform_Mesh_fs_4fv_color[pg_current_configuration_rank], MS_WHITE_RIGHT_r, MS_WHITE_RIGHT_g, MS_WHITE_RIGHT_b, 1.0f);
				glUniform4f(uniform_Mesh_fs_4fv_color_master_photo_weight_bg[pg_current_configuration_rank], MS_WHITE_RIGHT_color_weight, MS_WHITE_RIGHT_master_weight, MS_WHITE_RIGHT_photo_weight, MS_BLACK);
			}
			else if (indMeshFile == 1 /* WHITE_LEFT */) {
				glUniform4f(uniform_Mesh_fs_4fv_color[pg_current_configuration_rank], MS_WHITE_LEFT_r, MS_WHITE_LEFT_g, MS_WHITE_LEFT_b, 1.0f);
				glUniform4f(uniform_Mesh_fs_4fv_color_master_photo_weight_bg[pg_current_configuration_rank], MS_WHITE_LEFT_color_weight, MS_WHITE_LEFT_master_weight, MS_WHITE_LEFT_photo_weight, MS_BLACK);
			}
			else if (indMeshFile == 3 /* BLACK */) {
				glUniform4f(uniform_Mesh_fs_4fv_color[pg_current_configuration_rank], MS_WHITE_LEFT_r, MS_WHITE_LEFT_g, MS_WHITE_LEFT_b, 1.0f);
				glUniform4f(uniform_Mesh_fs_4fv_color_master_photo_weight_bg[pg_current_configuration_rank], 0.f, 0.f, 0.f, MS_BLACK);
			}
		}
#endif

		for (int indObjectInMesh = 0; indObjectInMesh < pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_nbObjectsPerMeshFile; indObjectInMesh++) {
			bool visibleObject = true;
#if defined(var_MmeShanghai_brokenGlass)
			if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_MmeShanghai_brokenGlass]) {
				if (indMeshFile == 0) {
					if (MmeShanghai_brokenGlass > 0 && MmeShanghai_brokenGlass <= pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_MmeShanghai_NbMeshSubParts) {
						visibleObject = pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_MmeShanghai_MeshSubParts[MmeShanghai_brokenGlass - 1][indObjectInMesh];
						// new visible object, defines the wake up time for falling
						if (visibleObject && pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_MmeShanghaiMeshObjectWakeupTime[indObjectInMesh] == 0.f
							// animation starts when all the meshes are displayed
							&& MmeShanghai_brokenGlass >= pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_MmeShanghai_NbMeshSubParts) {
							pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_MmeShanghaiMeshObjectWakeupTime[indObjectInMesh] = pg_CurrentClockTime + rand_0_1 * 10.0;
							pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_MmeShanghai_Object_Rotation_Ini_angle[indObjectInMesh] = float(rand_0_1 * M_PI * 0.004);
							pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_MmeShanghai_Object_Rotation_X[indObjectInMesh] = rand_0_1;
							pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_MmeShanghai_Object_Rotation_Y[indObjectInMesh] = rand_0_1;
							pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_MmeShanghai_Object_Rotation_Z[indObjectInMesh] = rand_0_1;
						}
					}
					else if (MmeShanghai_brokenGlass > pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_MmeShanghai_NbMeshSubParts) {
						visibleObject = pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_MmeShanghai_MeshSubParts[pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_MmeShanghai_NbMeshSubParts - 1][indObjectInMesh];
					}
					else {
						visibleObject = false;
					}
					if (visibleObject
						&& pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_MmeShanghaiMeshObjectWakeupTime[indObjectInMesh] < pg_CurrentClockTime
						// animation starts when all the meshes are displayed
						&& MmeShanghai_brokenGlass >= pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_MmeShanghai_NbMeshSubParts) {
						MmeShanghai_automatic_brokenGlass_animation(indMeshFile, indObjectInMesh);
						// Model matrix 
						// transformed mesh according to scenario file
						// 1. a varying translation matrix
						ObjectPosModelMatrix = glm::translate(MeshPosModelMatrix,
							glm::vec3(pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_MmeShanghai_Object_Translation_X[indObjectInMesh],
								pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_MmeShanghai_Object_Translation_Y[indObjectInMesh],
								pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_MmeShanghai_Object_Translation_Z[indObjectInMesh]));
						// 2. a varying rotation matrix (with bakc and forth translation to barycenter so that the object rotates on itself
						ObjectPosModelMatrix = glm::translate(ObjectPosModelMatrix,
							glm::vec3(pg_Meshes[pg_current_configuration_rank][indMeshFile].mesh_barycenter[3 * indObjectInMesh + 0],
								pg_Meshes[pg_current_configuration_rank][indMeshFile].mesh_barycenter[3 * indObjectInMesh + 1],
								pg_Meshes[pg_current_configuration_rank][indMeshFile].mesh_barycenter[3 * indObjectInMesh + 2]));
						glm::vec3 myObjectRotationAxis(pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_MmeShanghai_Object_Rotation_X[indObjectInMesh],
							pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_MmeShanghai_Object_Rotation_Y[indObjectInMesh],
							pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_MmeShanghai_Object_Rotation_Z[indObjectInMesh]);
						ObjectPosModelMatrix = glm::rotate(ObjectPosModelMatrix,
							pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_MmeShanghai_Object_Rotation_angle[indObjectInMesh],
							myObjectRotationAxis);
						ObjectPosModelMatrix = glm::translate(ObjectPosModelMatrix,
							glm::vec3(-pg_Meshes[pg_current_configuration_rank][indMeshFile].mesh_barycenter[3 * indObjectInMesh + 0],
								-pg_Meshes[pg_current_configuration_rank][indMeshFile].mesh_barycenter[3 * indObjectInMesh + 1],
								-pg_Meshes[pg_current_configuration_rank][indMeshFile].mesh_barycenter[3 * indObjectInMesh + 2]));

						// model matrix transfered to GPU (if it is not object by object made)
						glUniformMatrix4fv(uniform_Mesh_vp_model[pg_current_configuration_rank], 1, GL_FALSE,
							glm::value_ptr(ObjectPosModelMatrix));
					}
					else {
						glUniformMatrix4fv(uniform_Mesh_vp_model[pg_current_configuration_rank], 1, GL_FALSE,
							glm::value_ptr(glm::mat4(1.0f)));
					}
				}
			}
#endif
			if (visibleObject 
				&& pg_shader_programme[pg_current_configuration_rank][_pg_shader_Mesh]) {
				//printf("mesh pass mesh %d obj %d\n", indMeshFile, indObjectInMesh);

				// binds VAO
				glBindVertexArray(pg_Meshes[pg_current_configuration_rank][indMeshFile].mesh_vao[indObjectInMesh]);

				// activate shaders and sets uniform variable values    
				glUseProgram(pg_shader_programme[pg_current_configuration_rank][_pg_shader_Mesh]);

				glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				// texture unit location
				glUniform1i(uniform_Mesh_texture_fs_decal[pg_current_configuration_rank], 0);
				glActiveTexture(GL_TEXTURE0 + 0);

				// specific texture
				if (pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_Mesh_TextureRank != -1) {
					glBindTexture(GL_TEXTURE_RECTANGLE, pg_Meshes[pg_current_configuration_rank][indMeshFile].Mesh_texture_rectangle);
				}
				// Augmented Reality: FBO capture of Master to be displayed on a mesh
				else if (augmentedReality) {
					glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Master_capturedFB_prec_texID);
				}
				else {
					glBindTexture(GL_TEXTURE_RECTANGLE, 0);
				}

#if defined(var_MmeShanghai_brokenGlass)
				if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_MmeShanghai_brokenGlass]) {
					// second texture is the BG rendering
					glUniform1i(uniform_Mesh_texture_fs_BG[pg_current_configuration_rank], 1);
					glActiveTexture(GL_TEXTURE0 + 1);
					glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Master_capturedFB_prec_texID);  // master output memory for mapping on mesh
				}
#endif
				// standard filled mesh drawing
				// draw triangles from the currently bound VAO with current in-use shader
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pg_Meshes[pg_current_configuration_rank][indMeshFile].mesh_index_vbo[indObjectInMesh]);

				// updates this variable according whether triangles or lines are shown
				glUniform4f(uniform_Mesh_fs_4fv_isDisplayLookAt_with_mesh_with_blue_currentScene[pg_current_configuration_rank],
					isDisplayLookAt, -1, float(indMeshFile), (GLfloat)pg_CurrentSceneIndex);
				glUniform3f(uniform_Mesh_fs_3fv_light[pg_current_configuration_rank], mesh_light_x, mesh_light_y, mesh_light_z);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				glDrawElements(GL_TRIANGLES, pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_nbFacesPerMeshFile[indObjectInMesh] * 3, GL_UNSIGNED_INT, (GLvoid*)0);
				printOglError(698);

				if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_with_mesh]) {
					// draws the polygon contours
					// updates this variable according whether triangles or lines are shown
					if (with_mesh == 1
#if defined(var_MmeShanghai_brokenGlass)
						&& indMeshFile == 0
#endif
						) {
						glDisable(GL_DEPTH_TEST);
						glLineWidth(1);
						glUniform4f(uniform_Mesh_fs_4fv_isDisplayLookAt_with_mesh_with_blue_currentScene[pg_current_configuration_rank], isDisplayLookAt, 1, pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_Mesh_TextureRank != -1, (GLfloat)pg_CurrentSceneIndex);
						glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
						glDrawElements(GL_TRIANGLES, pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_nbFacesPerMeshFile[indObjectInMesh] * 3, GL_UNSIGNED_INT, (GLvoid*)0);
						glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
						glEnable(GL_DEPTH_TEST);
					}
				}

				// Augmented Reality: FBO capture of Master to be displayed on a mesh
				if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_activeMeshes] 
					&& pg_FullScenarioActiveVars[pg_current_configuration_rank][_meshRenderBypass]) {
					// optional additional drawing of the polygon contours for checking calibration in augmented reality
					if (with_mesh) {
						// no z-Buffer
						glDisable(GL_DEPTH_TEST);
						glLineWidth(3);
						glUniform4f(uniform_Mesh_fs_4fv_isDisplayLookAt_with_mesh_with_blue_currentScene[pg_current_configuration_rank], isDisplayLookAt, 1, with_blue, (GLfloat)pg_CurrentSceneIndex);
						glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
						glDrawElements(GL_TRIANGLES, pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_nbFacesPerMeshFile[indObjectInMesh] * 3, GL_UNSIGNED_INT, (GLvoid*)0);
						glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
						// no z-Buffer
						glEnable(GL_DEPTH_TEST);
					}
				}
				//printf("Display mesh VP1 %d/%d size (nb faces) %d\n\n", indMeshFile + 1, pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_Meshes->pg_nbObjectsPerMeshFile,
				//	pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_nbFacesPerMeshFile[indObjectInMesh]);
			} // visible submesh object
		} // submeshes

		// armature rendering
		if (TabBones[indMeshFile] != NULL && false) {
			glDisable(GL_DEPTH_TEST);
			//printf("render armature mesh #%d\n", indMeshFile);
				// updates this variable according whether triangles or lines are shown
			glUniform4f(uniform_Mesh_fs_4fv_isDisplayLookAt_with_mesh_with_blue_currentScene[pg_current_configuration_rank],
				isDisplayLookAt, 1, float(indMeshFile), (GLfloat)pg_CurrentSceneIndex);
			glUniform3f(uniform_Mesh_fs_3fv_light[pg_current_configuration_rank], 0, 0, 0);
			render_bones(MeshPosModelMatrix, indMeshFile);
		}

	} // visible mesh
}

void pg_drawOneMesh2(int indMeshFile) {
	// visibility
	bool visible = false;
	visible = (activeMeshes & (1 << indMeshFile));

	// visible mesh
	if (visible
		&& pg_shader_programme[pg_current_configuration_rank][_pg_shader_Mesh]) {

		// activate shaders and sets uniform variable values    
		glUseProgram(pg_shader_programme[pg_current_configuration_rank][_pg_shader_Mesh]);

		// Model matrix 
		// transformed mesh according to scenario file
		// 1. a varying translation matrix
		MeshPosModelMatrix = glm::translate(glm::mat4(1.0f),
			glm::vec3(pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_Mesh_Translation_X, pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_Mesh_Translation_Y, pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_Mesh_Translation_Z));
		// 2. a varying rotation matrix 
		glm::vec3 myRotationAxis(pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_Mesh_Rotation_X,
			pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_Mesh_Rotation_Y, pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_Mesh_Rotation_Z);
		MeshPosModelMatrix = glm::rotate(MeshPosModelMatrix, pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_Mesh_Rotation_angle, myRotationAxis);
		// 3. a varying scaling matrix 
		MeshPosModelMatrix = glm::scale(MeshPosModelMatrix, glm::vec3(pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_Mesh_Scale));

		// model matrix transfered to GPU
		glUniformMatrix4fv(uniform_Mesh_vp_model[pg_current_configuration_rank], 1, GL_FALSE,
			glm::value_ptr(MeshPosModelMatrix));

		for (int indObjectInMesh = 0; indObjectInMesh < pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_nbObjectsPerMeshFile; indObjectInMesh++) {
			// binds VAO
			glBindVertexArray(pg_Meshes[pg_current_configuration_rank][indMeshFile].mesh_vao[indObjectInMesh]);

			glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			// texture unit location
			glUniform1i(uniform_Mesh_texture_fs_decal[pg_current_configuration_rank], 0);
			glActiveTexture(GL_TEXTURE0 + 0);
			// specific mesh texture
			if (pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_Mesh_TextureRank != -1) {
				glBindTexture(GL_TEXTURE_RECTANGLE, pg_Meshes[pg_current_configuration_rank][indMeshFile].Mesh_texture_rectangle);
			}
			// Augmented Reality: FBO capture of Master to be displayed on a mesh
			else if (augmentedReality) {
				glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Master_capturedFB_prec_texID);
			}
			else {
				glBindTexture(GL_TEXTURE_RECTANGLE, 0);  // no texture
			}

			// standard filled mesh drawing
			// draw triangles from the currently bound VAO with current in-use shader
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pg_Meshes[pg_current_configuration_rank][indMeshFile].mesh_index_vbo[indObjectInMesh]);

			// updates this variable according whether triangles or lines are shown
			glUniform4f(uniform_Mesh_fs_4fv_isDisplayLookAt_with_mesh_with_blue_currentScene[pg_current_configuration_rank], isDisplayLookAt, 0, with_blue, (GLfloat)pg_CurrentSceneIndex);

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glDrawElements(GL_TRIANGLES, pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_nbFacesPerMeshFile[indObjectInMesh] * 3, GL_UNSIGNED_INT, (GLvoid*)0);

			// Augmented Reality: FBO capture of Master to be displayed on a mesh
					// optional additional drawing of the polygon contours for checking calibration in augmented reality
			if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_textureFrontier_wmin]) {
				if (with_mesh) {
					// no z-Buffer
					glDisable(GL_DEPTH_TEST);
					glLineWidth(3);
					glUniform4f(uniform_Mesh_fs_4fv_isDisplayLookAt_with_mesh_with_blue_currentScene[pg_current_configuration_rank], isDisplayLookAt, 1, with_blue, (GLfloat)pg_CurrentSceneIndex);
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
					glDrawElements(GL_TRIANGLES, pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_nbFacesPerMeshFile[indObjectInMesh] * 3, GL_UNSIGNED_INT, (GLvoid*)0);
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					// no z-Buffer
					glEnable(GL_DEPTH_TEST);
				}
			}
			//printf("Display mesh VP2 %d/%d size (nb faces) %d\n", indMeshFile, indObjectInMesh,
			//	pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_nbFacesPerMeshFile[indObjectInMesh]);


		} // submeshes

	} // visible mesh
}

void pg_MeshPass(void) {
	float eyePosition[3] = { 20.f, 0.f, 0.f };
	float lookat[3] = { 0.f, 0.f, 0.f };

	if (!pg_shader_programme[pg_current_configuration_rank][_pg_shader_Mesh]) {
		return;
	}

	// Augmented Reality: FBO capture of Master to be displayed on a mesh

	// Meshes rendering
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_activeMeshes]) {
		// mesh geometry rendering
		if (!pg_FullScenarioActiveVars[pg_current_configuration_rank][_meshRenderBypass]
			|| !meshRenderBypass) {
			// draws the mesh on top of clip arts or particles if they exit (before mixing/master rendering)
			if (!pg_FullScenarioActiveVars[pg_current_configuration_rank][_augmentedReality]
				|| !augmentedReality) {
				// draws the mesh alone
				// unbind output FBO 
				glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

				// draws on back buffer
				glDrawBuffer(GL_BACK);

				// no transparency
				glDisable(GL_BLEND);

				// draws the mesh on top clip arts if they exit
				if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_activeClipArts]) {
					// draws the meshes on top of clipArt
					if (pg_FrameNo > 0) {
						glBindFramebuffer(GL_DRAW_FRAMEBUFFER, pg_FBO_ClipArtRendering);
					}
				}
				// draws the mesh on top particles if they exit
				else {
					// draws the meshes on top of particles
					if (pg_FrameNo > 0) {
						glBindFramebuffer(GL_DRAW_FRAMEBUFFER, pg_FBO_ParticleRendering);
					}
				}
			}
			else {
				// draws the mesh alone after mixing/master rendering
				// unbind output FBO 
				glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

				// draws on back buffer
				glDrawBuffer(GL_BACK);

				// no transparency
				glDisable(GL_BLEND);
			}

			// transparency
			glEnable(GL_BLEND);
			// glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
			glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

			// output buffer cleanup
			glClear(GL_DEPTH_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);
			//glDepthMask(GL_FALSE);
			//glDepthFunc(GL_LESS);

			////////////////////////////////////////
			// drawing meshes

			// activate shaders and sets uniform variable values    
			glUseProgram(pg_shader_programme[pg_current_configuration_rank][_pg_shader_Mesh]);

			// calculates the view and perspective matrices according to the parameters in the scenario file
			// for one camera if single screen, and two otherwise
#if !defined(var_MmeShanghai_brokenGlass)
			pg_calculate_perspective_matrices();
#else
			pg_calculate_orthographic_matrices();
#endif

			if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_mesh_homography]) {
				if (mesh_homography) {
					glm::highp_mat4 homographyMatrix;

					// Read points
					std::vector<cv::Point2f> sourcePoints;
					//std::vector<cv::Point2f> source2Points;
					std::vector<cv::Point2f> destinationPoints;

					sourcePoints.push_back(cv::Point2f(-VP1KeystoneXBottomLeft,
						-VP1KeystoneYBottomLeft));
					sourcePoints.push_back(cv::Point2f(VP1KeystoneXBottomRight,
						-VP1KeystoneYBottomRight));
					sourcePoints.push_back(cv::Point2f(VP1KeystoneXTopRight,
						VP1KeystoneYTopRight));
					sourcePoints.push_back(cv::Point2f(-VP1KeystoneXTopLeft,
						VP1KeystoneYTopLeft));

					destinationPoints.push_back(cv::Point2f(-1.0f, -1.0f));
					destinationPoints.push_back(cv::Point2f(1.0f, -1.0f));
					destinationPoints.push_back(cv::Point2f(1.0f, 1.0f));
					destinationPoints.push_back(cv::Point2f(-1.0f, 1.0f));

					// initializes the homography matrices for the distortion of the projected image
					pg_calculate_homography_matrices(&sourcePoints, &destinationPoints, matValues, 4);
					homographyMatrix = (glm::make_mat4(matValues));
				}
			}

			// sets viewport to single window
			if (double_window) {
				glViewport(0, 0, workingWindow_width, window_height);
			}
			else {
				glViewport(0, 0, workingWindow_width, window_height);
			}

			// projection and view matrices
			glUniformMatrix4fv(uniform_Mesh_vp_proj[pg_current_configuration_rank], 1, GL_FALSE,
				// glm::value_ptr(VP1homographyMatrix * VP1perspMatrix));
				glm::value_ptr(VP1perspMatrix));
			glUniformMatrix4fv(uniform_Mesh_vp_view[pg_current_configuration_rank], 1, GL_FALSE,
				glm::value_ptr(VP1viewMatrix));

			// all the meshes
			for (unsigned int indMeshFile = 0; indMeshFile < pg_Meshes[pg_current_configuration_rank].size(); indMeshFile++) {
				pg_drawOneMesh(indMeshFile);
			} // all the meshes
			printOglError(697);
		}
		// direct rendering
		else {
			// bypassing mesh rendering: outputs master output memory 
			glBindFramebuffer(GL_READ_FRAMEBUFFER, pg_FBO_Master_capturedFB_prec); 
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
			glDrawBuffer(GL_BACK);
			glBlitFramebuffer(0, 0, workingWindow_width, window_height, 0, 0, workingWindow_width, window_height,
				GL_COLOR_BUFFER_BIT, GL_NEAREST);
			glBindFramebuffer(GL_READ_FRAMEBUFFER, 0); //  unbind read buffer
			printOglError(599);
		}

#if defined(PG_SECOND_MESH_CAMERA)
		if (!directRenderingwithoutMeshScreen2) {
			// sets viewport to second window
			glViewport(workingWindow_width, 0, workingWindow_width, window_height);

			// duplicates the Meshs in case of double window

			glUniformMatrix4fv(uniform_Mesh_vp_proj[pg_current_configuration_rank], 1, GL_FALSE,
				// glm::value_ptr(VP2homographyMatrix * VP2perspMatrix));
				glm::value_ptr(VP2perspMatrix));
			glUniformMatrix4fv(uniform_Mesh_vp_view[pg_current_configuration_rank], 1, GL_FALSE,
				glm::value_ptr(VP2viewMatrix));

			// all mesh files
			for (int indMeshFile = 0; indMeshFile < pg_Meshes[pg_current_configuration_rank].size(); indMeshFile++) {
				pg_drawOneMesh2(indMeshFile);
			} // all the meshes

			// sets viewport to full window
			glViewport(0, 0, window_width, window_height);

			glDisable(GL_DEPTH_TEST);
			// no transparency
			glDisable(GL_BLEND);
			printOglError(598);
		}
		else {
			//printf("Direct copy of Master shader output right window to back buffer\n");
			// bypassing mesh rendering: outputs master output memory 
			glBindFramebuffer(GL_READ_FRAMEBUFFER, pg_FBO_Master_capturedFB_prec);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
			glDrawBuffer(GL_BACK);
			glBlitFramebuffer(workingWindow_width, 0, 2 * workingWindow_width, window_height, workingWindow_width, 0, 2 * workingWindow_width, window_height,
				GL_COLOR_BUFFER_BIT, GL_NEAREST);
			glBindFramebuffer(GL_READ_FRAMEBUFFER, 0); //  unbind read buffer
			printOglError(599);
		}
#endif
	}
}


//////////////////////////////////////////////////////////////////////////////////////////////
// DRAWING A SCENE ON VARIOUS MODALITIES (CURVE, IMAGE, FRAMEBUFFER...)
//////////////////////////////////////////////////////////////////////////////////////////////
void pg_draw_scene(DrawingMode mode) {
	// ******************** Svg output ********************
	if (mode == _Svg) {
		cv::String imageFileName;
		pg_Svg_Capture_param.indSvgSnapshot++;

		imageFileName = format("%s%s-%s-%04d.svg",
			snapshots_dir_path_name.c_str(),
			pg_Svg_Capture_param.Svg_file_name.c_str(),
			date_stringStream.str().c_str(),
			pg_Svg_Capture_param.indSvgSnapshot);
		pg_logCurrentLineSceneVariables(imageFileName);

		writesvg(imageFileName);

/*
		if (!threaded) {
			writesvg((void*)&pDataWriteSvg);
		}
		else {
#if defined(WIN32)
			DWORD rc;
			HANDLE  hThread = CreateThread(
				NULL,                   // default security attributes
				0,                      // use default stack size  
				writesvg,		    // thread function name
				(void*)&pDataWriteSvg,		    // argument to thread function 
				0,                      // use default creation flags 
				&rc);   // returns the thread identifier 
			if (hThread == NULL) {
				std::cout << "Error:unable to create thread writesvg" << std::endl;
				exit(-1);
			}
			CloseHandle(hThread);
#else
			pthread_t drawing_thread;
			int rc;
			rc = pthread_create(&drawing_thread, NULL,
				writesvg, (void*)&pDataWriteSvg);
			if (rc) {
				std::cout << "Error:unable to create thread writesvg" << rc << std::endl;
				exit(-1);
			}
			pthread_exit(NULL);
#endif
		}
*/
	}

	// ******************** Png output ********************
	else if (mode == _Png) {
		//pDataWritePng.w = workingWindow_width;
		//pDataWritePng.h = window_height;
		cv::String imageFileName;

		pg_Png_Capture_param.indPngSnapshot++;

		imageFileName = format("%s%s-%s-%04d.png",
			snapshots_dir_path_name.c_str(),
			pg_Png_Capture_param.Png_file_name.c_str(),
			date_stringStream.str().c_str(),
			pg_Png_Capture_param.indPngSnapshot);
		struct stat buffer;
		int count = 0;
		while (stat(imageFileName.c_str(), &buffer) == 0) {
			imageFileName = format("%s%s-%s-%04d-%03d.png",
				snapshots_dir_path_name.c_str(),
				pg_Png_Capture_param.Png_file_name.c_str(),
				date_stringStream.str().c_str(),
				pg_Png_Capture_param.indPngSnapshot, count);
			count++;
		}
		pg_logCurrentLineSceneVariables(imageFileName);

		glReadBuffer(GL_FRONT);

		// OpenGL's default 4 byte pack alignment would leave extra bytes at the
		//   end of each image row so that each full row contained a number of bytes
		//   divisible by 4.  Ie, an RGB row with 3 pixels and 8-bit componets would
		//   be laid out like "RGBRGBRGBxxx" where the last three "xxx" bytes exist
		//   just to pad the row out to 12 bytes (12 is divisible by 4). To make sure
		//   the rows are packed as tight as possible (no row padding), set the pack
		//   alignment to 1.
		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		pngImgMatRGBInitial.create(window_height, workingWindow_width, CV_8UC3); // GL_RGB
		glReadPixels(0, 0, workingWindow_width, window_height, GL_RGB, GL_UNSIGNED_BYTE, pngImgMatRGBInitial.data);

		writepng(imageFileName);
/*
		if (!threaded) {
			writepng((void*)&pDataWritePng);
		}
		else {
#if defined(WIN32)
			DWORD rc;
			HANDLE  hThread = CreateThread(
				NULL,                   // default security attributes
				0,                      // use default stack size  
				writepng,		    // thread function name
				(void*)&pDataWritePng,		    // argument to thread function 
				0,                      // use default creation flags 
				&rc);   // returns the thread identifier 
			if (hThread == NULL) {
				std::cout << "Error:unable to create thread writepng" << std::endl;
				exit(-1);
			}
			CloseHandle(hThread);
#else
			pthread_t drawing_thread;
			int rc;
			rc = pthread_create(&drawing_thread, NULL,
				writepng, (void*)pData);
			if (rc) {
				std::cout << "Error:unable to create thread writepng" << rc << std::endl;
				exit(-1);
			}
			pthread_exit(NULL);
#endif
		}
*/
	}

	// ******************** Jpg output ********************
	else if (mode == _Jpg) {
		//pDataWriteJpg.w = workingWindow_width;
		//pDataWriteJpg.h = window_height;
		cv::String imageFileName;
	
		pg_Jpg_Capture_param.indJpgSnapshot++;

		imageFileName = format("%s%s-%s-%04d.jpg",
			snapshots_dir_path_name.c_str(),
			pg_Jpg_Capture_param.Jpg_file_name.c_str(),
			date_stringStream.str().c_str(),
			pg_Jpg_Capture_param.indJpgSnapshot);
		struct stat buffer;
		int count = 0;
		while (stat(imageFileName.c_str(), &buffer) == 0) {
			imageFileName = format("%s%s-%s-%04d-%03d.jpg",
				snapshots_dir_path_name.c_str(),
				pg_Jpg_Capture_param.Jpg_file_name.c_str(),
				date_stringStream.str().c_str(),
				pg_Jpg_Capture_param.indJpgSnapshot, count);
			count++;
		}
		pg_logCurrentLineSceneVariables(imageFileName);
		printf("Snapshot jpg step %d (%s)\n",
			pg_Jpg_Capture_param.indJpgSnapshot,
			imageFileName.c_str());

		glReadBuffer(GL_BACK);

		// OpenGL's default 4 byte pack alignment would leave extra bytes at the
		//   end of each image row so that each full row contained a number of bytes
		//   divisible by 4.  Ie, an RGB row with 3 pixels and 8-bit componets would
		//   be laid out like "RGBRGBRGBxxx" where the last three "xxx" bytes exist
		//   just to pad the row out to 12 bytes (12 is divisible by 4). To make sure
		//   the rows are packed as tight as possible (no row padding), set the pack
		//   alignment to 1.
		glPixelStorei(GL_PACK_ALIGNMENT, 1);

		jpgImgMatRGBInitial.create(window_height, workingWindow_width, CV_8UC3); // GL_RGB
		glReadPixels(0, 0, workingWindow_width, window_height, GL_RGB, GL_UNSIGNED_BYTE,
			jpgImgMatRGBInitial.data);
		printOglError(706);

		writejpg(imageFileName);
	}

	// ******************** interactive output ********************
	else if (mode == _Render) {

		// sets viewport to single window
		glViewport(0, 0, workingWindow_width, window_height);

		glDisable(GL_BLEND);

		//////////////////////////////////////
		// particle animation pass #0
		pg_ParticleAnimationPass();
		printOglError(681);

		//////////////////////////////////////
		// update pass #1 
		// image layers update pass (CA, PIXELS, DRAWING, VIDEO AND PHOTO...)
		pg_UpdatePass();
		printOglError(682);

		//////////////////////////////////////
		// ClipArt clip art pass #2
		pg_ClipArtRenderingPass();
		printOglError(683);

		//////////////////////////////////////
		// particle pass #3
		pg_ParticleRenderingPass();
		printOglError(684);

		//////////////////////////////////////
		// mesh pass #2b
		// the meshes are displayed together with the particles except for augmented reality or mesh rendering bypassing
		// where they are displayed last
		if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_activeMeshes]) {
			if (!(pg_FullScenarioActiveVars[pg_current_configuration_rank][_augmentedReality] && augmentedReality)
				|| (pg_FullScenarioActiveVars[pg_current_configuration_rank][_meshRenderBypass] && meshRenderBypass)) {
				if (activeMeshes > 0) {
					pg_MeshPass();
				}
			}
		}

		//////////////////////////////////////
		// layer compositing & echo pass #3
		pg_MixingPass();
		printOglError(684);

		//////////////////////////////////////
		// final combination of echoed and non echoed rendering #4
		pg_MasterPass();
		printOglError(685);

		//////////////////////////////////////
		// additional sensor pass on top of final rendering
		if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_sensor_layout]) {
			bool oneSensorActiveMin = false;
			for (int indSens = 0; indSens < PG_NB_SENSORS; indSens++) {
				int reindexed_Sensor = Sensor_order[indSens];
				if (sensor_onOff[reindexed_Sensor]) {
					oneSensorActiveMin = true;
					break;
				}
			}
			if (oneSensorActiveMin) {
				pg_SensorPass();
				printOglError(686);

				// ////////////////////////
				// read sensor values on CA (non echoed) and send messages
				if (pg_FrameNo >= 10 + first_frame_number || pg_CurrentSceneIndex > 0) {
					readSensors();
					printOglError(687);
				}
			}
			// incremental sensor activation every 45 sec. = 720/16
			for (int indSens = 0; indSens < PG_NB_SENSORS; indSens++) {
				int reindexed_Sensor = Sensor_order[indSens];
				if (!sensor_onOff[reindexed_Sensor]) {
					//printf("pg_CurrentClockTime - sensor_last_activation_time %f\n", pg_CurrentClockTime - sensor_last_activation_time);
					if (sensor_activation == 5
						&& pg_CurrentClockTime - sensor_last_activation_time > 45) {
						sensor_last_activation_time = pg_CurrentClockTime;
						sensor_onOff[reindexed_Sensor] = true;
						break;
					}
				}
			}
		}


#if defined(pg_Project_CAaudio) && defined(PG_PUREDATA_SOUND) && defined(var_nb_CATypes)
		// drawing memory on odd and even frames for CA	
		glBindFramebuffer(GL_READ_FRAMEBUFFER, pg_FBO_Mixing_capturedFB_prec); // drawing memory on odd and even frames for echo and sensors	
		pg_bindFBOTextures(pg_FBO_Mixing_capturedFB_prec, pg_FBO_Mixing_capturedFB_prec_texID + (pg_FrameNo % 2), 1, false, 0);
		playChord();
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
#endif

		//////////////////////////////////////
		// last mesh pass with final rendering texture
		// for augmented reality
		// Augmented Reality: FBO capture of Master to be displayed on a mesh
		if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_activeMeshes]) {
			if ((pg_FullScenarioActiveVars[pg_current_configuration_rank][_augmentedReality] && augmentedReality)
				|| (pg_FullScenarioActiveVars[pg_current_configuration_rank][_meshRenderBypass] && meshRenderBypass)) {
				if (activeMeshes > 0) {
					pg_MeshPass();
				}
			}
		}
	}
	printOglError(686);
}

