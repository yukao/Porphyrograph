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
      
/////////////////////////////////////////////////////////////////
// config variables
// large array for passing the path data to the fragment shader
float path_data_Update[(PG_NB_PATHS + 1) * PG_MAX_PATH_DATA * 4];
float path_data_ParticleAnimation[(PG_NB_PATHS + 1) * PG_MAX_PATH_ANIM_DATA * 4];

float paths_x_next[PG_NB_PATHS + 1];
float paths_y_next[PG_NB_PATHS + 1];

// path variables
float paths_time[PG_NB_PATHS + 1];
float paths_x[PG_NB_PATHS + 1];
float paths_y[PG_NB_PATHS + 1];
float paths_time_prev_prev[PG_NB_PATHS + 1];
float paths_x_prev_prev[PG_NB_PATHS + 1];
float paths_y_prev_prev[PG_NB_PATHS + 1];
float paths_time_prev[PG_NB_PATHS + 1];
float paths_x_prev[PG_NB_PATHS + 1];
float paths_y_prev[PG_NB_PATHS + 1];
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
#ifdef PG_BEZIER_PATHS
float paths_xL[PG_NB_PATHS + 1];
float paths_yL[PG_NB_PATHS + 1];
float paths_xR[PG_NB_PATHS + 1];
float paths_yR[PG_NB_PATHS + 1];
#endif
int paths_BrushID[PG_NB_PATHS + 1];

float repop_ColorBG_r;
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

#ifdef PG_MESHES
// MVP matrices
glm::mat4 VP1perspMatrix;
glm::mat4 VP1viewMatrix;
glm::mat4 MeshPosModelMatrix;
glm::mat4 VP1homographyMatrix;
#ifdef PG_SECOND_MESH_CAMERA
glm::mat4 VP2perspMatrix;
glm::mat4 VP2viewMatrix;
glm::mat4 VP2homographyMatrix;
#endif
#endif

#ifdef ETOILES_TEASER
float pen_x, pen_y, vec_x, vec_y;
#endif

#ifdef PG_SENSORS
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
#ifndef CAAUDIO
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

#ifndef CAAUDIO
// sensor rendering
int Sensor_order[PG_NB_SENSORS] = { 8, 13, 14, 11, 7, 2, 1, 4, 12, 15, 3, 0, 9, 10, 5, 6 };
#else
// sensor rendering
int Sensor_order[PG_NB_SENSORS] = { 0 };
#endif

#ifdef PG_SUPERCOLLIDER
// groups of samples for aliasing with additive samples
int sample_groups[PG_NB_SENSOR_GROUPS][4] =
{ { 1, 2, 5, 6 },
{ 3, 4, 7, 8 },
{ 9, 10, 13, 14 },
{ 11, 12 , 15, 16 },

{ 17, 18, 21, 22 },
{ 19, 20, 23, 24 },
{ 25, 26, 29, 30 },
{ 27, 28, 31, 32 },

{ 33, 34, 37, 38 },
{ 35, 36, 39, 40 },
{ 41, 42, 45, 46 },
{ 43, 44, 47, 48 } };
#endif

// current sensor
int currentSensor = 0;
// sensor follows mouse
bool sensorFollowMouse_onOff = false;
#endif

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
// textures bitmaps and associated IDs
#if defined (TEXTURED_QUAD_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 
std::vector<GLuint> pg_particle_initial_pos_speed_texID;
std::vector<GLuint> pg_particle_initial_color_radius_texID;
std::vector<GLuint> pg_particle_acc_texID;
#endif

GLuint pg_screenMessageBitmap_texID = 0; // nb_attachments=1
GLubyte *pg_screenMessageBitmap = NULL;

GLuint pg_paths_Pos_Texture_texID[PG_NB_PATHS + 1];
GLfloat *pg_paths_Pos_Texture[PG_NB_PATHS + 1];
GLuint pg_paths_Col_Texture_texID[PG_NB_PATHS + 1];
GLfloat *pg_paths_Col_Texture[PG_NB_PATHS + 1];
GLuint pg_paths_RadBrushRendmode_Texture_texID[PG_NB_PATHS + 1];
GLfloat *pg_paths_RadBrushRendmode_Texture[PG_NB_PATHS + 1];

#if defined (GN) || defined (CAAUDIO) || defined(RIVETS)
GLuint pg_CATable_ID = NULL_ID;
GLubyte *pg_CATable = NULL;
#endif

#if defined (GN) || defined (INTERFERENCE)
GLuint pg_LYMlogo_texID = NULL_ID;
#endif

GLuint Screen_Font_texture_Rectangle_texID = NULL_ID;
#if defined (TVW)
GLuint Display_Font_texture_Rectangle_texID = NULL_ID;
#endif

#if !defined (PG_BEZIER_PATHS) || defined(PIERRES) || defined(ENSO) || defined(SONG) || defined(FORET) || defined (SOUNDINITIATIVE) || defined(ALKEMI)
GLuint Pen_texture_3D_texID = NULL_ID;
#endif
GLuint Noise_texture_3D = NULL_ID;
#ifdef PG_WITH_REPOP_DENSITY
std::vector<GLuint>  pg_RepopDensity_texture_texID;
#endif

#ifdef PG_WITH_MASTER_MASK
GLuint Master_Mask_texID = 0;
GLuint Master_Multilayer_Mask_texID = 0;
#endif

#ifdef PG_WITH_BURST_MASK
GLuint Burst_Mask_texID = 0;
#endif

#if defined (TVW)
/////////////////////////////////////////////////////////////////
// Image distribution
float centralPhoto = 0;
#define PG_MASK_VARIANCE 291

/////////////////////////////////////////////////////////////////
// textures bitmaps and associated IDs
GLuint pg_Display_Message1_Bitmap_texID = NULL_ID;
GLubyte *pg_displayMsg1Bitmap = NULL;
GLuint pg_Display_Message2_Bitmap_texID = NULL_ID;
GLubyte *pg_displayMsg2Bitmap = NULL;

// large messages displayed on the bottom of the screen
string DisplayText1;
float DisplayText1Alpha = 0.0f;
float LengthDisplayText1 = 0.0f;
int IndDisplayText1 = -1;

string DisplayText2;
float DisplayText2Alpha = 0.0f;
float LengthDisplayText2 = 0.0f;
int IndDisplayText2 = -1;

bool DisplayText1Front = true;
float DisplayTextSwapInitialTime = 0;
#endif
#if defined (TVW) || defined (ETOILES)
std::string* DisplayTextList;
int* DisplayTextFirstInChapter;
int NbDisplayTexts = 0;
int pg_Ind_Current_DisplayText = 0;

// message chapters
int NbTextChapters = 0;
#endif

#ifdef PG_SENSORS
GLuint Sensor_texture_rectangle = NULL_ID;
#endif

// video texture ID, image and camera
GLuint pg_camera_texture_texID = 0;
GLuint pg_movie_texture_texID = 0;
GLuint pg_camera_BG_texture_texID = 0;
#ifdef GN
GLuint pg_camera_BGIni_texture_texID = 0;
#endif
// IplImage *pg_camera_frame = NULL;
// IplImage *pg_movie_frame = NULL;
int pg_camera_frame_width = -1;
int pg_camera_frame_height = -1;
int pg_camera_x_offset = -1;
int pg_camera_y_offset = -1;
int pg_movie_frame_width = 0;
int pg_movie_frame_height = 0;

// video tracks
vector<string> movieFileName;
vector<string> movieShortName;
vector<string> movieSoundtrackPeaksFileName;
vector<string> movieSoundtrackOnsetsFileName;
vector<vector<float>> movieSoundtrackPeaks;
vector<vector<float>> movieSoundtrackOnsets;
int nb_movies = 0;
// soundtracks
vector<string> trackFileName;
vector<string> trackShortName;
int nb_soundtracks = 0;
// pen palettes presets
int nb_pen_colorPresets = 0;
vector<string> pen_colorPresets_names;
float* pen_colorPreset_values = NULL;
// lights presets
int pg_nb_lights = 0;
int pg_nb_light_groups = 0;
Light* pg_lights = NULL;
LightGroup* pg_light_groups = NULL;
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
int nb_pen_palette_colors = 0;
vector<string> pen_palette_colors_names;
float **pen_palette_colors_values = NULL;
// photo albums
string photoAlbumDirName;
int nb_photo_albums = 0;
// short video clip albums
string clipAlbumDirName;
int nb_clip_albums = 0;
int clip_image_width = 0;
int clip_image_height = 0;
int clip_crop_width = 0;
int clip_crop_height = 0;
// pen brushes
string pen_brushes_fileName;
int nb_pen_brushes = 0;
// textures with multiple layers
int nb_layers_master_mask = 0;
bool is_capture_diaporama = false;
#ifdef PG_WITH_CAMERA_CAPTURE
VideoCapture  pg_webCam_capture;
VideoCapture* pg_IPCam_capture;
String* pg_IPCam_capture_address;
int nb_IPCam = 0;
int pg_current_active_cameraNo = -2;
bool initializedWebcam = false;
#endif
VideoCapture  pg_movie_capture;

movie_status pg_movie_status = movie_status();
bool is_movieLoading = false;
bool is_movieLooping = false;
bool secondCurrentBGCapture = false;

#ifdef PG_WITH_CLIPS
// clip track
std::vector<clip_track> pg_clip_tracks;
// clip status (left and right)
clip_status pg_clip_status[_clipLR] = { clip_status(_clipLeft), clip_status(_clipRight)};
// index of top clip 
int rankOfTopClip = 0;
#endif

#ifdef GN
bool secondInitialBGCapture = false;
bool initialBGCapture = false;
#endif

const char *TextureEncodingString[EmptyTextureEncoding + 1] = { "jpeg", "png", "pnga", "png_gray", "pnga_gray", "rgb", "raw", "emptyimagetype" };

///////////////////////////////////////////////////////
// MOVIE STATUS MANAGEMENT WHILE STREAMING
movie_status::movie_status() {
	nbFramesLeft = 0;
	initialTime = 0.;
	initialMovieNbFrames = 0;

	currentSoundPeakIndex = 0;
	if (currentlyPlaying_movieNo >= 0) {
		nbSoundPeakIndex = movieSoundtrackPeaks[currentlyPlaying_movieNo].size();
	}
	else {
		nbSoundPeakIndex = 0;
	}
	currentSoundOnsetIndex = 0;
	if (currentlyPlaying_movieNo >= 0) {
		nbSoundOnsetIndex = movieSoundtrackOnsets[currentlyPlaying_movieNo].size();
	}
	else {
		nbSoundOnsetIndex = 0;
	}
}
// number frames left until the end from current frame
int movie_status::get_nbFramesLeft() {
	return nbFramesLeft;
}
// number of frames read from the beginning of the movie
int movie_status::get_nbFramesRead() {
	return initialMovieNbFrames - nbFramesLeft;
}
// current time when movie started
double movie_status::get_initialTime() {
	return initialTime;
}
// initial total number of frames in the movie
int movie_status::get_initialNbFrames() {
	return initialMovieNbFrames;
}
// sets the movie at a position from beginning
void movie_status::set_position(int nb_frames_from_beginning) {
	if (nb_frames_from_beginning <= 0) {
		reset_movie(initialMovieNbFrames);
		return;
	}
	if (nb_frames_from_beginning >= initialMovieNbFrames) {
		nb_frames_from_beginning = initialMovieNbFrames - 1;
	}
	nbFramesLeft = initialMovieNbFrames - nb_frames_from_beginning;
	initialTime = CurrentClockTime - (nb_frames_from_beginning / movieCaptFreq);
	//printf("set position frames from start %d frames left %d initialTime %.5f\n", nb_frames_from_beginning, nbFramesLeft, initialTime);

	currentSoundPeakIndex = 0;
	currentSoundOnsetIndex = 0;
	// sets the position for the onsets and peaks as well
	updatePeakOrOnset();
}

// checks whether a peak or an onset are passed or closer than one frame
void movie_status::updatePeakOrOnset() {
	double timeFromBeginning = CurrentClockTime - initialTime;

	pg_video_sound_peak = false;
	if (currentSoundPeakIndex < nbSoundPeakIndex - 1) {
		float nextPeak = movieSoundtrackPeaks[currentlyPlaying_movieNo][currentSoundPeakIndex + 1];
		while (nextPeak < timeFromBeginning || nextPeak - timeFromBeginning < 1 / movieCaptFreq) {
			pg_video_sound_peak = true;
			//printf("peak next %.5f time from begin %.5f\n", nextPeak, timeFromBeginning);
			currentSoundPeakIndex++;
			if (currentSoundPeakIndex < nbSoundPeakIndex - 1) {
				nextPeak = movieSoundtrackPeaks[currentlyPlaying_movieNo][currentSoundPeakIndex + 1];
			}
			else {
				break;
			}
		}
	}
	pg_video_sound_onset = false;
	if (currentSoundOnsetIndex < nbSoundOnsetIndex - 1) {
		float nextOnset = movieSoundtrackOnsets[currentlyPlaying_movieNo][currentSoundOnsetIndex + 1];
		while (nextOnset < timeFromBeginning || nextOnset - timeFromBeginning < 1 / movieCaptFreq) {
			pg_video_sound_onset = true;
			//printf("onset next %.5f time from begin %.5f\n", nextOnset, timeFromBeginning);
			currentSoundOnsetIndex++;
			if (currentSoundOnsetIndex < nbSoundOnsetIndex - 1) {
				nextOnset = movieSoundtrackOnsets[currentlyPlaying_movieNo][currentSoundOnsetIndex + 1];
			}
			else {
				break;
			}
		}
	}
}
// reads one frame and updates the remaining number of frames in the movie
void movie_status::read_one_frame() {
	nbFramesLeft--;
	updatePeakOrOnset();
}
// (re)starts the movie from begginning with its total number of frames
void movie_status::reset_movie(int nbTotFramesLeft) {
	// printf("movie restarted\n");
	nbFramesLeft = nbTotFramesLeft;
	initialMovieNbFrames = nbFramesLeft;
	initialTime = CurrentClockTime;

	currentSoundPeakIndex = 0;
	if (currentlyPlaying_movieNo >= 0) {
		nbSoundPeakIndex = movieSoundtrackPeaks[currentlyPlaying_movieNo].size();
	}
	else {
		nbSoundPeakIndex = 0;
	}
	currentSoundOnsetIndex = 0;
	if (currentlyPlaying_movieNo >= 0) {
		nbSoundOnsetIndex = movieSoundtrackOnsets[currentlyPlaying_movieNo].size();
	}
	else {
		nbSoundOnsetIndex = 0;
	}
}
movie_status::~movie_status() {

}

#ifdef PG_WITH_CLIPS
///////////////////////////////////////////////////////
// // CLIP TRACKS
clip_track::clip_track(int ind_clip, int nbFrames) {
	clipNo = ind_clip;
	initialNbFrames = nbFrames;
	for (int ind = 0; ind < _NbMaxCues; ind++) {
		clip_cue[ind] = -1;
	}
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

///////////////////////////////////////////////////////
// CLIP STATUS MANAGEMENT WHILE STREAMING
clip_status::clip_status(int clipSide) {
	for (int ind = 0; ind < PG_NB_PARALLEL_CLIPS; ind++) {
		currentlyPlaying_clipNo[ind] = -1;
		lastFrame[ind] = 0;
		clip_autoplay[ind] = true;
		clip_play[ind] = true;
		lastPlayedFrameTime[ind] = CurrentClockTime;
	}
	sideLR = pg_clip_LR(clipSide);
	cumulated_scratch_offset = 0.;
	currentFPS = 0;
	last_scratch_angle = 0.f;
	last_nudge_angle = 0.f;
	is_scratch_pressed = false;
	last_scratch_angle_change = 0.;
	clip_level = 1.0;
	clip_r = 1.0;
	clip_g = 1.0;
	clip_b = 1.0;
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
	double timeLapseSinceLastFrame = (CurrentClockTime - lastPlayedFrameTime[indClipRank]);
	// clip is not updated if the speed is null or if the scratching plate is pressed 
	if (currentFPS != 0 && !is_scratch_pressed) {
		if (fabs(timeLapseSinceLastFrame) >= fabs(1. / currentFPS)) {
			int nb_elapsedFrames = int(floor(fabs(timeLapseSinceLastFrame) * currentFPS)) * (timeLapseSinceLastFrame > 0 ? 1 : -1);
			lastFrame[indClipRank] = (lastFrame[indClipRank] + nb_elapsedFrames);
			int initialNbFrames = pg_clip_tracks[currentlyPlaying_clipNo[indClipRank]].get_initialNbFrames();
			while (lastFrame[indClipRank] >= initialNbFrames) {
				lastFrame[indClipRank] -= initialNbFrames;
			}
			while (lastFrame[indClipRank] < 0) {
				lastFrame[indClipRank] += initialNbFrames;
			}
			if (nb_elapsedFrames != 0) {
				lastPlayedFrameTime[indClipRank] = lastPlayedFrameTime[indClipRank] + nb_elapsedFrames / currentFPS;
				if (CurrentClockTime - lastPlayedFrameTime[indClipRank] > 1. / currentFPS || CurrentClockTime - lastPlayedFrameTime[indClipRank] < -1. / currentFPS) {
					lastPlayedFrameTime[indClipRank] = CurrentClockTime;
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
				else if (indClipRank == 1) {
					if (sideLR == _clipLeft) {
						sprintf(AuxString, "/clip2_percent_left %d", int(lastFrame[indClipRank] / float(initialNbFrames) * 100.f)); pg_send_message_udp((char*)"i", (char*)AuxString, (char*)"udp_TouchOSC_send");
					}
					else {
						sprintf(AuxString, "/clip2_percent_right %d", int(lastFrame[indClipRank] / float(initialNbFrames) * 100.f)); pg_send_message_udp((char*)"i", (char*)AuxString, (char*)"udp_TouchOSC_send");
					}
				}
			}
		}
	}
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
	currentlyPlaying_clipNo[indClipRank] = indClip;
	lastPlayedFrameTime[indClipRank] = CurrentClockTime;
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
		//printf("set position %d  initialTime %.5f\n", 0, CurrentClockTime);
		return;
	}
	int initialNbFrames = pg_clip_tracks[currentlyPlaying_clipNo[indClipRank]].get_initialNbFrames();
	lastFrame[indClipRank] = nb_frames_from_beginning % initialNbFrames;
	lastPlayedFrameTime[indClipRank] = CurrentClockTime;
	//printf("set position %d  initialTime %.5f\n", lastFrame[indClipRank], lastPlayedFrameTime[indClipRank]);
}
// offsets the movie position by a positive or negative time delta (play mode)
// used once for a big jump
void clip_status::play_offset_position(int indClipRank, double elapsedTime) {
	int nb_elapsedFrames = int(floor(elapsedTime * currentFPS));
	int initialNbFrames = pg_clip_tracks[currentlyPlaying_clipNo[indClipRank]].get_initialNbFrames();
	lastFrame[indClipRank] = max(min((lastFrame[indClipRank] + nb_elapsedFrames), initialNbFrames - 1), 0);
	lastPlayedFrameTime[indClipRank] = CurrentClockTime;
}
// offsets the movie position by a positive or negative time delta (scratch mode)
// used several times for a small offset
void clip_status::scratch_offset_position(int indClipRank, double timeLapse) {
	cumulated_scratch_offset += timeLapse;
	//printf("cumulated_scratch_offset %.3f\n", cumulated_scratch_offset);
	// positive number of frames, move forward
	if (cumulated_scratch_offset >= 0) {
		int nb_elapsedFrames = int(floor(cumulated_scratch_offset * currentFPS));
		//printf("positive lapse %d %.2f\n", nb_elapsedFrames, cumulated_scratch_offset * currentFPS);
		if (nb_elapsedFrames > 0) {
			int initialNbFrames = pg_clip_tracks[currentlyPlaying_clipNo[indClipRank]].get_initialNbFrames();
			lastFrame[indClipRank] = (lastFrame[indClipRank] + nb_elapsedFrames) % initialNbFrames;
			lastPlayedFrameTime[indClipRank] = CurrentClockTime;
			cumulated_scratch_offset = cumulated_scratch_offset - nb_elapsedFrames / currentFPS;
		}
	}
	// negative number of frames, move backward
	else {
		int nb_elapsedFrames = int(floor(cumulated_scratch_offset * currentFPS) + 1);
		//printf("negative lapse %d %.2f\n", nb_elapsedFrames, cumulated_scratch_offset * currentFPS);
		if (nb_elapsedFrames < 0) {
			int initialNbFrames = pg_clip_tracks[currentlyPlaying_clipNo[indClipRank]].get_initialNbFrames();
			lastFrame[indClipRank] = (lastFrame[indClipRank] + nb_elapsedFrames + initialNbFrames) % initialNbFrames;
			lastPlayedFrameTime[indClipRank] = CurrentClockTime;
			cumulated_scratch_offset = cumulated_scratch_offset - nb_elapsedFrames / currentFPS;
		}
	}
}
// (re)starts the clip from begginning with its total number of frames
void clip_status::reset_clip(int indClipRank) {
	// printf("clip restarted\n");
	if (currentlyPlaying_clipNo[indClipRank] >= 0 && currentlyPlaying_clipNo[indClipRank] < pg_nbClips) {
		int initialNbFrames = pg_clip_tracks[currentlyPlaying_clipNo[indClipRank]].get_initialNbFrames();
		initialNbFrames = pg_nbCompressedClipFramesPerFolder[currentlyPlaying_clipNo[indClipRank]];
	}
	lastPlayedFrameTime[indClipRank] = CurrentClockTime;
	lastFrame[indClipRank] = 0;
}
clip_status::~clip_status() {

}
#endif

///////////////////////////////////////////////////////
// GLUT draw function (from the viewpoint)

// the glut callback
// requires predrawing (drawing from the user to the root node)

// ------------------------------------------------------------ //
// --------------- DISPLAYS WINDOWS ONE AFTER ANOTHER --------- //
// ------------------------------------------------------------ //
void window_display( void ) {
  windowDisplayed = true;
  // printOglError(508);

  //////////////////////////////////////////////////
  //////////////////////////////////////////////////
  // SCENE UPDATE AND SHADER UNIFORM PARAMETERS UPDATE
  //////////////////////////////////////////////////
  // scenario update
  pg_update_scenario();
  pg_run_callBacks();

  // recalculates pulsed colors and reads current paths
  pg_update_pulsed_colors_and_replay_paths(CurrentClockTime);

  // ships uniform variables  printOglError(51);
  printOglError(50);
  pg_update_shader_uniforms();
  printOglError(51);
  
  // loads movie and/or camera frames
  pg_update_clip_camera_and_movie_frame();


  //////////////////////////////////////////////////
  //////////////////////////////////////////////////
  // SCENE DISPLAY AND SHADER UNIFORM PARAMETERS UPDATE
  //////////////////////////////////////////////////
  // OpenGL initializations before redisplay
  OpenGLInit();
  // printOglError(509);

  // proper scene redrawing
  // printf("VideoPb Draw scene \n\n");
  pg_draw_scene( _Render, false );

  //////////////////////////////////////////////////
  //////////////////////////////////////////////////
  // ONE-FRAME PARAMETERS RESET
  //////////////////////////////////////////////////
  // resets to 0 the variables that are only true for one frame such as flashes, resets...
  one_frame_variables_reset();

  // flushes OpenGL commands
  glFlush();

  glutSwapBuffers();
  // 

  // ------------------------------------------------------------ //
  // --------------- FRAME/SUBFRAME GRABBING -------------------- //

  // ---------------- frame by frame output --------------------- //
  // Svg screen shots
  // printf("Draw Svg\n" );
  if(take_snapshots && outputSvg
	&& pg_FrameNo % stepSvg == 0
	&& pg_FrameNo / stepSvg >= beginSvg && 
	pg_FrameNo / stepSvg <= endSvg ) {
    // printf("Draw Svg %d\n" , pg_FrameNo  );
    pg_draw_scene( _Svg, true);
  }
    
  // ---------------- frame by frame output --------------------- //
  // Png screen shots
  // printf("Draw Png\n" );
  if(take_snapshots && outputPng
	&& pg_FrameNo % stepPng == 0
	&& pg_FrameNo / stepPng >= beginPng && 
	pg_FrameNo / stepPng <= endPng ) {
    pg_draw_scene( _Png, true);
  }
    
  // ---------------- frame by frame output --------------------- //
  // Jpg screen shots
  // printf("Draw Jpg\n"  );
  if(take_snapshots && outputJpg
	&& pg_FrameNo % stepJpg == 0
	&& pg_FrameNo / stepJpg >= beginJpg && 
	pg_FrameNo / stepJpg <= endJpg ) {
    pg_draw_scene( _Jpg, true);
  }
}

#ifdef PG_SENSORS
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
	bool sensorOn[PG_NB_SENSORS];

	bool sampleOn[PG_NB_MAX_SAMPLE_SETUPS * PG_NB_SENSORS];
	int sampleToSensorPointer[PG_NB_MAX_SAMPLE_SETUPS * PG_NB_SENSORS];

	GLubyte pixelColor[3 * PG_NB_SENSORS];
	memset(pixelColor, 0, sizeof(pixelColor));

	// marks all the samples as unread
	for (int indSample = 0; indSample < PG_NB_MAX_SAMPLE_SETUPS * PG_NB_SENSORS; indSample++) {
		sampleOn[indSample] = false;
		sampleToSensorPointer[indSample] = -1;
	}

	glBindFramebuffer(GL_READ_FRAMEBUFFER, pg_FBO_Mixing_capturedFB_prec[(pg_FrameNo % 2)]); // drawing memory on odd and even frames for echo and sensors	
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
	// SENSOR-BASED SOUND IN SUPERCOLLIDER
#ifdef PG_SUPERCOLLIDER
	// looks for buffer aliasing possibilities: groups of sounds that could be replaced by a single buffer
	float sensorLevel[PG_NB_SENSORS + 18];
	bool groupOn[PG_NB_SENSOR_GROUPS];
	float groupValues[PG_NB_SENSOR_GROUPS];
	//printf("group on ");
	for (int indgroup = 0; indgroup < PG_NB_SENSOR_GROUPS; indgroup++) {
		if (sampleOn[sample_groups[indgroup][0]] && sampleOn[sample_groups[indgroup][1]]
			&& sampleOn[sample_groups[indgroup][2]] && sampleOn[sample_groups[indgroup][3]]) {
			// switches on the group with the average activation value
			groupOn[indgroup] = true;
			groupValues[indgroup] = (sensorLevel[sampleToSensorPointer[sample_groups[indgroup][0]]]
				+ sensorLevel[sampleToSensorPointer[sample_groups[indgroup][1]]]
				+ sensorLevel[sampleToSensorPointer[sample_groups[indgroup][2]]]
				+ sensorLevel[sampleToSensorPointer[sample_groups[indgroup][3]]]);
			// switches off the associated sensors
			sensorLevel[sampleToSensorPointer[sample_groups[indgroup][0]]] = 0.0f;
			sensorOn[sampleToSensorPointer[sample_groups[indgroup][0]]] = false;
			sensorLevel[sampleToSensorPointer[sample_groups[indgroup][1]]] = 0.0f;
			sensorOn[sampleToSensorPointer[sample_groups[indgroup][1]]] = false;
			sensorLevel[sampleToSensorPointer[sample_groups[indgroup][2]]] = 0.0f;
			sensorOn[sampleToSensorPointer[sample_groups[indgroup][2]]] = false;
			sensorLevel[sampleToSensorPointer[sample_groups[indgroup][3]]] = 0.0f;
			sensorOn[sampleToSensorPointer[sample_groups[indgroup][3]]] = false;
			//printf("%d (%d,%d,%d,%d) ",indgroup,sampleToSensorPointer[ sample_groups[ indgroup ][ 0 ] ],
			//  sampleToSensorPointer[ sample_groups[ indgroup ][ 1 ] ],
			//  sampleToSensorPointer[ sample_groups[ indgroup ][ 2 ] ],
			//  sampleToSensorPointer[ sample_groups[ indgroup ][ 3 ] ]);
		}
		else {
			groupOn[indgroup] = false;
			groupValues[indgroup] = 0.0f;
		}
	}
	//printf("\n");

	// message value
	std::string float_str;
	std::string message = "/sensors";
	float totalAmplitude = 0.0;
	for (int indSens = 0; indSens < PG_NB_SENSORS; indSens++) {
		if (sensorOn[indSens]) {
			float_str = std::to_string(static_cast<long double>(sensorLevel[indSens]));
			// float_str.resize(4);
			message += " " + float_str;
			totalAmplitude += sensorLevel[indSens];
		}
		else {
			message += " 0.0";
		}
	}
	for (int indgroup = 0; indgroup < PG_NB_SENSOR_GROUPS; indgroup++) {
		if (groupOn[indgroup]) {
			float_str = std::to_string(static_cast<long double>(groupValues[indgroup]));
			// float_str.resize(4);
			message += " " + float_str;
			totalAmplitude += groupValues[indgroup];
		}
		else {
			message += " 0.0";
		}
	}
	float_str = std::to_string(static_cast<long double>(totalAmplitude));
	// float_str.resize(4);
	message += " " + float_str;

	// message format
	std::string format = "";
	for (int indSens = 0; indSens < PG_NB_SENSORS; indSens++) {
		format += "f ";
	}
	for (int indgroup = 0; indgroup < 18; indgroup++) {
		format += "f ";
	}
	// Total amplitude
	format += "f";

	// message posting
	pg_send_message_udp((char *)format.c_str(), (char *)message.c_str(), (char *)"udp_SC_send");
#endif

	///////////////////////////////////////////////////////////////
	// SENSOR-BASED SOUND IN RENOISE OR PORPHYROGRAPH SOUND
#ifndef SUPERCOLLIDER
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

#ifdef PG_RENOISE
			// Renoise message format && message posting
			sprintf(AuxString, "/renoise/song/track/%d/prefx_volume %.2f", sample_choice[indSens] + 1, sensorLevel[indSens] * sensor_vol);
			pg_send_message_udp((char*)"f", AuxString, (char*)"udp_RN_send");

			//printf("RENOISE send %s %f %f\n", AuxString, sensorLevel[indSens], sensor_vol);
#endif
#ifdef PG_PORPHYROGRAPH_SOUND
			// message format
			std::string format = "f";

			std::string message = "/track_";
			int_string = std::to_string(sample_choice[indSens] + 1);
			message += int_string + "_level";
			float_string = std::to_string(static_cast<long double>(sensorLevel[indSens])*sensor_vol);
			// float_str.resize(4);
			message += " " + float_string;
			// message posting
			pg_send_message_udp((char *)format.c_str(), (char *)message.c_str(), (char *)"udp_PGsound_send");
			printf("send %s\n", message.c_str());
			PlaySound(TEXT("C:/sync.com/Sync/LYM-projects/SoundData/soundYN-GN-data/samples-YN/samples_part1/sample_part1-002.wav"), NULL, SND_ASYNC);
#endif

			// starts the clock for stopping the sample play after a certain time
			sample_play_start[sample_choice[indSens]] = CurrentClockTime;
			sample_play_volume[sample_choice[indSens]] = sensorLevel[indSens];
			// printf("lights sensor #%d\n", indSens);
		}

		// the sample has been triggered and has not yet reached 90% of its playing duration
		if (sample_play_start[sample_choice[indSens]] > 0.0
			&& CurrentClockTime - sample_play_start[sample_choice[indSens]] <= 0.9 * BEAT_DURATION) {
			// set the value to the initial value until 0.9 so that there is one visual feedback per loop
			sensorLevel[indSens] = (sample_play_volume[sample_choice[indSens]]
				* float((CurrentClockTime - sample_play_start[sample_choice[indSens]]) / BEAT_DURATION));
		}
	}
	printOglError(689);

	///////////////////////////////////////////////
	// MANAGING THE SAMPLE SEVEL
	for (int indSetup = 0; indSetup < PG_NB_MAX_SAMPLE_SETUPS; indSetup++) {
		for (int indSens = 0; indSens < PG_NB_SENSORS; indSens++) {
			int indSample = sensor_sample_setUps[indSetup][indSens];
			if (sample_play_start[indSample] > 0
				&& CurrentClockTime - sample_play_start[indSample] > BEAT_DURATION) {
#ifdef PG_RENOISE
				// Renoise message format && message posting
				sprintf(AuxString, "/renoise/song/track/%d/prefx_volume %.2f", indSample + 1, 0.f);
				pg_send_message_udp((char*)"f", AuxString, (char*)"udp_RN_send");
#endif

#ifdef PG_PORPHYROGRAPH_SOUND
				// the sample plays until it finishes and only then it can be retriggered
				/*
				std::string message = "/track_";
				int_string = std::to_string(indSample + 1);
				message += int_string + "_level 0";

				// message format
				std::string format = "f";

				// message posting
				pg_send_message_udp((char *)format.c_str(), (char *)message.c_str(), (char *)"udp_PGsound_send");
				*/
#endif

				// resets the clock for replaying the sample if sensor triggered again
				sample_play_start[indSample] = -1.f;
			}
		}
	}
#endif
	printOglError(690);

	// message trace
	//std::cout << "format: " << format << "\n";
	//std::cout << "msg: " << message << "\n";
}
#endif

#ifdef CAAUDIO
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
void one_frame_variables_reset(void) {
	///////////////////////////////////////////////////////////////////////
	// flash reset: restores flash to 0 so that
	// it does not stay on more than one frame 
	for (int indtrack = 0; indtrack < PG_NB_TRACKS; indtrack++) {
		flashTrkCA_weights[indtrack] = 0;
		flashTrkBG_weights[indtrack] = 0;
		flashTrkPart_weights[indtrack] = 0;
	}

	if (flashPixel > 0) {
		flashPixel -= 1;
	}
	flashCABG_weight = 0;
	flashCAPart_weight = 0;
	flashPartBG_weight = 0;
	flashPartCA_weight = 0;

	if (flashMaster > 0) {
		flashMaster--;
		// end of flash return to dark
		if (pg_CurrentScene && flashMaster == 0) {
			master = 0.f;
			*((float*)ScenarioVarPointers[_master]) = master;
			master_scale = float(pg_CurrentScene->scene_initial_parameters[_master_scale]);
			*((float*)ScenarioVarPointers[_master_scale]) = master_scale;
			master_offsetX = float(pg_CurrentScene->scene_initial_parameters[_master_offsetX]);
			*((float*)ScenarioVarPointers[_master_offsetX]) = master_offsetX;
			master_offsetY = float(pg_CurrentScene->scene_initial_parameters[_master_offsetY]);
			*((float*)ScenarioVarPointers[_master_offsetY]) = master_offsetY;
			photo_threshold = float(pg_CurrentScene->scene_initial_parameters[_photo_threshold]);
			*((float*)ScenarioVarPointers[_photo_threshold]) = photo_threshold;
			photo_gamma = float(pg_CurrentScene->scene_initial_parameters[_photo_gamma]);
			*((float*)ScenarioVarPointers[_photo_gamma]) = photo_gamma;
			photo_satur = float(pg_CurrentScene->scene_initial_parameters[_photo_satur]);
			*((float*)ScenarioVarPointers[_photo_satur]) = photo_satur;
			invertAllLayers = bool(pg_CurrentScene->scene_initial_parameters[_invertAllLayers]);
			*((bool*)ScenarioVarPointers[_invertAllLayers]) = invertAllLayers;
		}
	}

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
#ifdef PG_WITH_PHOTO_FLASH
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
#endif

	// /////////////////////////
	// clear layer reset
	// does not reset if camera capture is still ongoing
	if ((
#ifdef PG_WITH_CAMERA_CAPTURE
		!reset_camera && 
#endif
		!secondCurrentBGCapture)
#ifdef GN
		|| (!initialBGCapture && !secondInitialBGCapture)
#endif
		) {
		isClearAllLayers = 0;
	}
	// clear CA reset
	isClearCA = 0;
	// clear layer reset
	isClearLayer = 0;
	// clear all layers reset
	isClearAllLayers = 0;
	// clear CA reset
	isClearEcho = 0;
	// layer copy reset
	// copy to layer above (+1) or to layer below (-1)
	copyToNextTrack = 0;

#ifdef PG_WITH_CAMERA_CAPTURE
	// DELAYED CAMERA WEIGHT COUNTDOWN
	if (delayedCameraWeight > 1) {
		delayedCameraWeight--;
	}
	else if (delayedCameraWeight == 1) {
		*((float*)ScenarioVarPointers[_cameraWeight]) = 1;
		BrokenInterpolationVar[_cameraWeight] = true;
		cameraWeight = 1;
		delayedCameraWeight = 0;
	}
#endif

#ifdef PG_WITH_BLUR
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

#if defined(CAAUDIO) || defined(RIVETS)
	// CA seed
	pg_CAseed_trigger = false;
#endif

#if defined (TEXTURED_QUAD_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 
	// /////////////////////////
	// particle initialization reset
	part_initialization = -1;
#endif
}

///////////////////////////////////////
// MOVIE STREAMING JUMPS

void pg_movie_backward(int nb_frames_back) {
	// calculates hypothetical new position
	int posInMovie = int(pg_movie_capture.get(CV_CAP_PROP_POS_FRAMES));
	int new_pos_in_movie = posInMovie - nb_frames_back;
	// rewinds the movie
	if (new_pos_in_movie > 0) {
		pg_movie_capture.set(CV_CAP_PROP_POS_FRAMES, new_pos_in_movie);
		pg_movie_status.set_position(new_pos_in_movie);
		printf("rewinds %d frames %d frames left %.1f initial time\n", int(movieCaptFreq * 10), pg_movie_status.get_nbFramesLeft(), pg_movie_status.get_initialTime());
	}
	else {
		pg_movie_capture.set(CV_CAP_PROP_POS_FRAMES, 0);
		pg_movie_status.reset_movie(int(pg_movie_capture.get(CV_CAP_PROP_FRAME_COUNT)));
		printf("starts from beginning\n");
	}
}

void pg_movie_forward(int nb_frames_forth) {
	// calculates hypothetical new position
	int posInMovie = int(pg_movie_capture.get(CV_CAP_PROP_POS_FRAMES));
	int new_pos_in_movie = posInMovie + nb_frames_forth;
	// advances in the movie
	if (new_pos_in_movie < pg_movie_status.get_initialNbFrames() ) {
		printf("forward %d frames %d frames left %.1f initial time\n", int(movieCaptFreq * 10), pg_movie_status.get_nbFramesLeft(), pg_movie_status.get_initialTime());
		pg_movie_capture.set(CV_CAP_PROP_POS_FRAMES, new_pos_in_movie);
		pg_movie_status.set_position(new_pos_in_movie);
	}
	// else stays where it is
}

///////////////////////////////////////
// CLIP STREAMING JUMPS

///////////////////////////////////////
// MOVIE AND CAMERA FRAME CAPTURE
void pg_update_clip_camera_and_movie_frame(void) {
	//////////////////////////////////////////////////////////////////////
	// CLIP, CAMERA AND MOVIE FRAME UPDATES BEFORE RENDERING
	// /////////////////////////
#ifdef PG_WITH_CLIPS
	// reads next clip frame
	for (int indClipRank = 0; indClipRank < PG_NB_PARALLEL_CLIPS; indClipRank++) {
		for (int indClip = 0; indClip < _clipLR; indClip++) {
			if (currentPhotoTrack >= 0 && clipCaptFreq > 0
				&& pg_clip_status[indClip].clip_play[indClipRank]
				&& pg_clip_status[indClip].getCurrentlyPlaying_clipNo(indClipRank) >= 0
				&& pg_clip_status[indClip].getCurrentlyPlaying_clipNo(indClipRank) < pg_nbClips) {

				// either the first frame has not been captured and the clip should loop at the beginning
				// or whe are coming close to the end of the clip
				//printf("clip frame %d\n", expectedclipFrameNo);
				pg_clip_status[indClip].get_nextFrameAndUpdate(indClipRank);
			}
		}
	}
#endif

	// /////////////////////////
	// reads next movie frame
	if (currentVideoTrack >= 0 && movieCaptFreq > 0
		&& currentlyPlaying_movieNo >= 0
		&& !is_movieLooping && !is_movieLoading
		) {

		// non threaded video update according to fps and time since the beginning of the movie play
		Mat pg_movie_frame;
		bool new_movie_frame = false;

		// tries to play in sync the whole movie, so counts the time from the 
		// first frame and compares the actual frame no (actualMovieFrameNo)
		// with the frame no expected to play with the correct frame rate (expectedMovieFrameNo)
		int actualMovieFrameNo = (pg_movie_status.get_initialNbFrames() - pg_movie_status.get_nbFramesLeft());
		int expectedMovieFrameNo = int((CurrentClockTime - pg_movie_status.get_initialTime()) * movieCaptFreq);

		// either the first frame has not been captured and the movie should loop at the beginning
		// or whe are coming close to the end of the movie
		if (!pg_FirstMovieFrameIsAvailable || (movie_loop && pg_movie_status.get_nbFramesLeft() < 10)) {
			printf("Full video loop %d\n", pg_FrameNo);
			// Full video loop
			if (pg_movie_status.get_nbFramesLeft() < 10) {
				// resets the movie
				pg_movie_capture.set(CV_CAP_PROP_POS_FRAMES, 0);
				pg_movie_status.reset_movie(int(pg_movie_capture.get(CV_CAP_PROP_FRAME_COUNT)));
				// printf("movie restarted\n");
			}
		}
		// if looping is not on and movie is closed to the end, loops on the last 10 frames
		else if (!movie_loop && pg_movie_status.get_nbFramesLeft() < 10) {
			// Short end video loop
			pg_movie_backward(int(movieCaptFreq * 10));
			printf("Short end video loop at %d, posInMovie %d\n",
				pg_FrameNo, pg_movie_status.get_nbFramesLeft());
		}

		// skips frames if rendering is slower than movie fps
		if (pg_FirstMovieFrameIsAvailable && expectedMovieFrameNo - actualMovieFrameNo > 2) {
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
		if (!pg_FirstMovieFrameIsAvailable || expectedMovieFrameNo - actualMovieFrameNo >= 1) {
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
		//elapsedTimeSinceLastCapture = CurrentClockTime - lastFrameCaptureTime;
		// printf("Capture at %.4f last capture %.4f elapsed time %.4f frame duration %.4f\n", CurrentClockTime, lastFrameCaptureTime, elapsedTimeSinceLastCapture, (1.0f / movieCaptFreq));
	}

	// /////////////////////////
	// reads next camera frame
#ifdef PG_WITH_CAMERA_CAPTURE
	if ((currentVideoTrack >= 0
		&& (// grabs according to camera capture frequence
			(cameraCaptFreq > 0 && pg_FrameNo % int(60.0 / cameraCaptFreq) == 0)
			// grabs at the beginning, otherwise the program is stuck if no camera frame reading takes place
			|| (pg_FrameNo >= PG_CAMERA_REST_DELAY && pg_FrameNo <= PG_CAMERA_REST_DELAY + 1000 && pg_FrameNo % 3 == 0))
		// a delay has to be let for camera to digest new parameter values
		&& (pg_FrameNo - pg_LastCameraParameterChange_Frame) > PG_CAMERA_REST_DELAY
		// the camera weight is positive
		&& cameraWeight > 0)
		|| (reset_camera || secondCurrentBGCapture)) {
		// currentVideoTrack <=> video off
		// frame capture
		// printf("frame capture\n");

		// non threaded
		// printf("*** non threaded camera frame capture %d\n", pg_FrameNo);
		if (cameraNo == -1 || (cameraNo >= 0 && cameraNo < nb_IPCam)) {
			loadCameraFrame(false, cameraNo);
		}
	}
#endif
}


void pg_update_shader_uniforms(void) {
#ifdef GN
#include "pg_update_body_GN.cpp"
#endif
#ifdef TVW
#include "pg_update_body_TVW.cpp"
#endif
#ifdef CRITON
#include "pg_update_body_Criton.cpp"
#endif
#if defined (KOMPARTSD)
#include "pg_update_body_KompartSD.cpp"
#endif
#if defined (KOMPARTSD)
#include "pg_update_body_Light.cpp"
#endif
#if defined (REUTLINGEN)
#include "pg_update_body_Reutlingen.cpp"
#endif
#if defined (BICHES)
#include "pg_update_body_Biches.cpp"
#endif
#if defined (ATELIERSENFANTS)
#include "pg_update_body_AteliersEnfants.cpp"
#endif
#if defined (CAVERNEPLATON)
#include "pg_update_body_CavernePlaton.cpp"
#endif
#if defined (PIERRES)
#include "pg_update_body_Pierres.cpp"
#endif
#if defined (ENSO)
#include "pg_update_body_Enso.cpp"
#endif
#if defined (TEMPETE)
#include "pg_update_body_Tempete.cpp"
#endif
#if defined (DAWN)
#include "pg_update_body_Dawn.cpp"
#endif
#if defined (RIVETS)
#include "pg_update_body_Rivets.cpp"
#endif
#if defined (ULM)
#include "pg_update_body_Ulm.cpp"
#endif
#ifdef SONG
#include "pg_update_body_Song.cpp"
#endif
#ifdef FORET
#include "pg_update_body_Foret.cpp"
#endif
#ifdef SOUNDINITIATIVE
#include "pg_update_body_SoundInitiative.cpp"
#endif
#ifdef ALKEMI
#include "pg_update_body_alKemi.cpp"
#endif
#if defined (DEMO) || defined (DEMO_BEZIER)
#include "pg_update_body_demo.cpp"
#endif
#ifdef VOLUSPA
#include "pg_update_body_voluspa.cpp"
#endif
#ifdef ARAKNIT
#include "pg_update_body_araknit.cpp"
#endif
#if defined (ETOILES)
#include "pg_update_body_etoiles.cpp"
#endif
#ifdef INTERFERENCE
#include "pg_update_body_interference.cpp"
#endif
#ifdef CAAUDIO
#include "pg_update_body_CAaudio.cpp"
#endif
#ifdef DASEIN
#include "pg_update_body_dasein.cpp"
#endif
#ifdef BONNOTTE
#include "pg_update_body_bonnotte.cpp"
#endif
	printOglError(510);

#if defined (TEXTURED_QUAD_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 
	/////////////////////////////////////////////////////////////////////////
	// PARTICLE ANIMATION SHADER UNIFORM VARIABLES
	glUseProgram(shader_programme[pg_shader_ParticleAnimation]);

	// prints possible particle animation
	if (part_initialization >= 0) {
		printf("particle initialization %d\n", part_initialization);
	}

	// scren size and repop channel
	// at each frame a random repop channel is chosen among the available ones
	bool repop_channels[PG_NB_PATHS + 1];
	int nb_repop_channels = 0;
#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7 || PG_NB_PATHS == 11
	nb_repop_channels += int(part_path_repop_0) + int(part_path_repop_1) + int(part_path_repop_2) + int(part_path_repop_3);
	repop_channels[0] = part_path_repop_0;
	repop_channels[1] = part_path_repop_1;
	repop_channels[2] = part_path_repop_2;
	repop_channels[3] = part_path_repop_3;
#endif
#if PG_NB_PATHS == 7 || PG_NB_PATHS == 11
	nb_repop_channels += int(part_path_repop_4) + int(part_path_repop_5) + int(part_path_repop_6) + int(part_path_repop_7);
	repop_channels[4] = part_path_repop_4;
	repop_channels[5] = part_path_repop_5;
	repop_channels[6] = part_path_repop_6;
	repop_channels[7] = part_path_repop_7;
#endif
#if PG_NB_PATHS == 11
	nb_repop_channels += int(part_path_repop_8) + int(part_path_repop_9) + int(part_path_repop_10) + int(part_path_repop_11);
	repop_channels[8] = part_path_repop_8;
	repop_channels[9] = part_path_repop_9;
	repop_channels[10] = part_path_repop_10;
	repop_channels[11] = part_path_repop_11;
#endif
	int selected_channel = int(floor(rand_0_1 * (nb_repop_channels - 0.00001)));
	int nbActChannels = 0;
	for (int indCh = 0; indCh <= PG_NB_PATHS ; indCh++) {
		if (repop_channels[indCh]) {
			if (nbActChannels == selected_channel) {
				selected_channel = indCh;
				break;
			}
			nbActChannels++;
		}
	}
	glUniform4f(uniform_ParticleAnimation_fs_4fv_W_H_repopChannel_targetFrameNo,
		(GLfloat)window_width, (GLfloat)window_height, (GLfloat)selected_channel, GLfloat(pg_targetFrameNo));

	// pen paths positions
#if defined (TVW)
	// special case for army explosion: track 1 is assigned as repulse or follow path but is not replayed
	// the center of the screen is the default position for this track
	if (part_path_repulse_1 || part_path_follow_1) {
		paths_x[1] = float(window_width / 2);
		paths_y[1] = float(window_height / 2);
	}
	// special case for army radar: track 2 is assigned as repop path but is not replayed
	// the center of the top left screen is the default position for this track
	else if (part_path_repop_2 || part_path_repulse_2) {
		paths_x[2] = float(window_width / 4);
		paths_y[2] = float(window_height / 4);
		float randval = float(randomValue * 2 * PI);
		float radius = randomValue;
		paths_x_prev[2] = float(window_width / 4 + 2 * radius * cos(randval));
		paths_y_prev[2] = float(window_height / 4 + 2 * radius * sin(randval));
	}
#endif
	for (int indPath = 0; indPath < (PG_NB_PATHS + 1); indPath++) {
		paths_x_memory[indPath] = (paths_x[indPath] > 0 ? paths_x[indPath] : paths_x_memory[indPath]);
		paths_x_prev_memory[indPath] = (paths_x_prev[indPath] > 0 ? paths_x_prev[indPath] : paths_x_prev_memory[indPath]);
		paths_y_memory[indPath] = (paths_y[indPath] > 0 ? paths_y[indPath] : paths_y_memory[indPath]);
		paths_y_prev_memory[indPath] = (paths_y_prev[indPath] > 0 ? paths_y_prev[indPath] : paths_y_prev_memory[indPath]);
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
	// color, radius, beginning or end of a stroke
	for (int indPath = 0; indPath < (PG_NB_PATHS + 1); indPath++) {
		path_data_ParticleAnimation[indPath * PG_MAX_PATH_ANIM_DATA * 4 + PG_PATH_ANIM_RAD * 4 + 0] = paths_RadiusX[indPath];
		path_data_ParticleAnimation[indPath * PG_MAX_PATH_ANIM_DATA * 4 + PG_PATH_ANIM_RAD * 4 + 1] = rand_0_1;
		path_data_ParticleAnimation[indPath * PG_MAX_PATH_ANIM_DATA * 4 + PG_PATH_ANIM_RAD * 4 + 2] = rand_0_1;
	}
	glUniform4fv(uniform_ParticleAnimation_path_data, (PG_NB_PATHS + 1) * PG_MAX_PATH_DATA, path_data_ParticleAnimation);

#if PG_NB_TRACKS >= 4
	// flash Trk -> Part weights
	glUniform4f(uniform_ParticleAnimation_fs_4fv_flashTrkPartWghts,
		flashTrkPart_weights[0], flashTrkPart_weights[1], flashTrkPart_weights[2], flashTrkPart_weights[3]);
	// printf("flashTrkPart_weights %.2f %.2f %.2f %.2f \n", flashTrkPart_weights[0], flashTrkPart_weights[1], flashTrkPart_weights[2], flashTrkPart_weights[3]);

#else
#if PG_NB_TRACKS >= 3
	// flash Trk -> Part weights
	glUniform4f(uniform_ParticleAnimation_fs_4fv_flashTrkPartWghts,
		flashTrkPart_weights[0], flashTrkPart_weights[1], flashTrkPart_weights[2], 0.f);
	// printf("flashTrkPart_weights %.2f %.2f %.2f \n", flashTrkPart_weights[0], flashTrkPart_weights[1], flashTrkPart_weights[2]);

#else
#if PG_NB_TRACKS >= 2
	// flash Trk -> Part weights
	glUniform4f(uniform_ParticleAnimation_fs_4fv_flashTrkPartWghts,
		flashTrkPart_weights[0], flashTrkPart_weights[1], 0.f, 0.f);
	// printf("flashTrkPart_weights %.2f %.2f \n", flashTrkPart_weights[0],  flashTrkPart_weights[1]);

#else
	// flash Trk -> Part weights
	glUniform4f(uniform_ParticleAnimation_fs_4fv_flashTrkPartWghts,
		flashTrkPart_weights[0], 0.f, 0.f, 0.f);
	// printf("flashTrkPart_weights %.2f %.2f \n", flashTrkPart_weights[0],  flashTrkPart_weights[1]);
#endif
#endif
#endif

	// movie size, flash camera and copy tracks
	// copy to layer above (+1) or to layer below (-1)

	// flash CA -> BG & repop color (BG & CA)
	glUniform4f(uniform_ParticleAnimation_fs_4fv_repop_Color_frameNo,
		repop_ColorPart_r, repop_ColorPart_g, repop_ColorPart_b, GLfloat(pg_FrameNo));
	// clear layer, flash pixel, flash CA -> Part
	glUniform4f(uniform_ParticleAnimation_fs_4fv_flashCAPartWght_nbPart_clear_nbPartInit,
		GLfloat(flashCAPart_weight), GLfloat(nb_particles), GLfloat(isClearAllLayers),
		GLfloat(pg_particle_initial_pos_speed_texID.size()));

	// movie size, flash camera and copy tracks
	// copy to layer above (+1) or to layer below (-1)
	glUniform4f(uniform_ParticleAnimation_fs_4fv_Camera_W_H_movieWH,
		GLfloat(pg_camera_frame_width), GLfloat(pg_camera_frame_height),
		GLfloat(pg_movie_frame_width), GLfloat(pg_movie_frame_height));

#endif
	printOglError(511);

	/////////////////////////////////////////////////////////////////////////
	// UPDATE SHADER UNIFORM VARIABLES
	glUseProgram(shader_programme[pg_shader_Update]);

	// time is only used in TVW
	glUniform4f(uniform_Update_fs_4fv_W_H_time_currentScene,
		(GLfloat)window_width, (GLfloat)window_height, (GLfloat)CurrentClockTime, (GLfloat)pg_CurrentSceneIndex);
	// printf("window_width %.2f\n", (GLfloat)window_width);
	// printf("time %.2f\n", (GLfloat)CurrentClockTime);
	// printf("scene %d\n", pg_CurrentSceneIndex);

	// pixels acceleration
	glUniform3f(uniform_Update_fs_3fv_clearAllLayers_clearCA_pulsedShift,
		(GLfloat)isClearAllLayers, (GLfloat)isClearCA,
#if defined (TEXTURED_QUAD_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 
		fabs(pulse_average - pulse_average_prec) * pulsed_part_Vshift);
#else
		0.f);
#endif

#if defined(CAAUDIO) || defined(RIVETS)
	if (pg_CAseed_trigger) {
		glUniform4f(uniform_Update_fs_4fv_CAseed_type_size_loc,
			(GLfloat)pg_CAseed_type, (GLfloat)pg_CAseed_size,
			(GLfloat)pg_CAseed_coordinates[0], (GLfloat)pg_CAseed_coordinates[1]);
		printf("pg_CAseed_trigger %d %d at %d %d\n", pg_CAseed_type, pg_CAseed_size,
			pg_CAseed_coordinates[0], pg_CAseed_coordinates[1]);
	}
	else {
		glUniform4f(uniform_Update_fs_4fv_CAseed_type_size_loc,
			(GLfloat)pg_CAseed_type, (GLfloat)pg_CAseed_size,
			-1.f, -1.f);
	}
#endif

	// position
	for (int indPath = 0; indPath < (PG_NB_PATHS + 1); indPath++) {
		path_data_Update[indPath * PG_MAX_PATH_DATA * 4 + PG_PATH_P_X * 4 + 0] = paths_x_prev[indPath];
#ifdef PG_BEZIER_PATHS
		path_data_Update[indPath * PG_MAX_PATH_DATA * 4 + PG_PATH_P_X * 4 + 1] = paths_xL[indPath];
		path_data_Update[indPath * PG_MAX_PATH_DATA * 4 + PG_PATH_P_X * 4 + 2] = paths_xR[indPath];
#endif
		path_data_Update[indPath * PG_MAX_PATH_DATA * 4 + PG_PATH_P_Y * 4 + 0] = paths_y_prev[indPath];
#ifdef PG_BEZIER_PATHS
		path_data_Update[indPath * PG_MAX_PATH_DATA * 4 + PG_PATH_P_Y * 4 + 1] = paths_yL[indPath];
		path_data_Update[indPath * PG_MAX_PATH_DATA * 4 + PG_PATH_P_Y * 4 + 2] = paths_yR[indPath];
#endif
	}
	for (int indPath = 0; indPath < PG_NB_CURSORS_MAX; indPath++) {
		path_data_Update[indPath * PG_MAX_PATH_DATA * 4 + PG_PATH_P_X * 4 + 3] = paths_x_forGPU[indPath];
		path_data_Update[indPath * PG_MAX_PATH_DATA * 4 + PG_PATH_P_Y * 4 + 3] = paths_y_forGPU[indPath];
	}
	for (int indPath = PG_NB_CURSORS_MAX; indPath < (PG_NB_PATHS + 1); indPath++) {
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
		//if (indPath == 0) {
		//	printf("Color RGBA %.1f %.1f %.1f %.1f\n", paths_Color_r[0], paths_Color_g[0], paths_Color_b[0], paths_Color_a[0] );
		//}
		path_data_Update[indPath * PG_MAX_PATH_DATA * 4 + PG_PATH_RADIUS_BEGINEND * 4 + 2] = paths_RadiusY[indPath];
		path_data_Update[indPath * PG_MAX_PATH_DATA * 4 + PG_PATH_RADIUS_BEGINEND * 4 + 3] = float(paths_BrushID[indPath]);
		// if (indPath == 0) {
		//  	printf("Brush ID %d\n", paths_BrushID[indPath]);
		// }
	}
	// printf("Radius %.2f %.2f\n", paths_RadiusX[0], paths_RadiusY[0]);
	//printf("pos %.1f %.1f %.1f %.1f %.1f %.1f %.1f %.1f \n", paths_x_prev[1], paths_y_prev[1],
	//	paths_xR[1], paths_yR[1], paths_xL[1], paths_yL[1], paths_x[1], paths_y[1]);
#ifdef PG_BEZIER_PATHS
	for (int indPath = 0; indPath < (PG_NB_PATHS + 1); indPath++) {
		build_bounding_box(indPath);
	}

	// bounding box containing the bezier curve augmented by its radius
	for (int indPath = 0; indPath < (PG_NB_PATHS + 1); indPath++) {
		path_data_Update[indPath * PG_MAX_PATH_DATA * 4 + PG_PATH_BOX * 4 + 0] = pg_BezierBox[indPath].x;
		path_data_Update[indPath * PG_MAX_PATH_DATA * 4 + PG_PATH_BOX * 4 + 1] = pg_BezierBox[indPath].y;
		path_data_Update[indPath * PG_MAX_PATH_DATA * 4 + PG_PATH_BOX * 4 + 2] = pg_BezierBox[indPath].z;
		path_data_Update[indPath * PG_MAX_PATH_DATA * 4 + PG_PATH_BOX * 4 + 3] = pg_BezierBox[indPath].w;
	}
#endif
	//printf("pos %.1f %.1f %.1f %.1f bez %.1f %.1f %.1f %.1f beging./end %d %d \n", paths_Color_r[1], paths_Color_g[1], paths_Color_b[1], paths_Color_a[1],
	//	pg_BezierBox[1].x, pg_BezierBox[1].y, pg_BezierBox[1].z, pg_BezierBox[1].w, isBegin[1], isEnd[1]);
	glUniform4fv(uniform_Update_path_data, (PG_NB_PATHS + 1) * PG_MAX_PATH_DATA, path_data_Update);

#ifdef CRITON
	glUniform4f(uniform_Update_fs_4fv_fftLevels03,
		fftLevels[0], fftLevels[1], fftLevels[2], fftLevels[3]);
	glUniform4f(uniform_Update_fs_4fv_fftFrequencies03,
		fftFrequencies[0], fftFrequencies[1], fftFrequencies[2], fftFrequencies[3]);
	glUniform4f(uniform_Update_fs_4fv_fftPhases03,
		fftPhases[0], fftPhases[1], fftPhases[2], fftPhases[3]);
	glUniform4f(uniform_Update_fs_4fv_fftLevels47,
		fftLevels[4], fftLevels[5], fftLevels[6], fftLevels[7]);
	glUniform4f(uniform_Update_fs_4fv_fftFrequencies47,
		fftFrequencies[4], fftFrequencies[5], fftFrequencies[6], fftFrequencies[7]);
	glUniform4f(uniform_Update_fs_4fv_fftPhases47,
		fftPhases[4], fftPhases[5], fftPhases[6], fftPhases[7]);
	//printf("Freq %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f\n", fftFrequencies[0], fftFrequencies[1], fftFrequencies[2], fftFrequencies[3], fftFrequencies[4], fftFrequencies[5], fftFrequencies[6], fftFrequencies[7]);
	//printf("Levels %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f\n", fftLevels[0], fftLevels[1], fftLevels[2], fftLevels[3], fftLevels[4], fftLevels[5], fftLevels[6], fftLevels[7]);
	//printf("Phases %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f\n", fftPhases[0], fftPhases[1], fftPhases[2], fftPhases[3], fftPhases[4], fftPhases[5], fftPhases[6], fftPhases[7]);
	//printf("Freq %.2f\n", fftFrequencies[0]);
	//printf("Levels %.2f\n", fftLevels[0]);
	//printf("Phase %.2f\n", fftPhases[0]);
#endif

#if PG_NB_TRACKS >= 4
	// flash BG weights
	glUniform4f(uniform_Update_fs_4fv_flashTrkBGWghts_flashPartBGWght,
		flashTrkBG_weights[1], flashTrkBG_weights[2], flashTrkBG_weights[3], flashPartBG_weight);
#ifdef PG_NB_CA_TYPES
	// flash Trk -> CA weights
	glUniform4f(uniform_Update_fs_4fv_flashTrkCAWghts,
		flashTrkCA_weights[0], flashTrkCA_weights[1], flashTrkCA_weights[2], flashTrkCA_weights[3]);
	// printf("flashTrkCA_weights %.2f %.2f %.2f %.2f \n", flashTrkCA_weights[0], flashTrkCA_weights[1], flashTrkCA_weights[2], flashTrkCA_weights[3]);
#endif
#else
#if PG_NB_TRACKS >= 3
	// flash Trk -> BG weights
	glUniform4f(uniform_Update_fs_4fv_flashTrkBGWghts_flashPartBGWght,
		flashTrkBG_weights[1], flashTrkBG_weights[2], 0.f, flashPartBG_weight);
#ifdef PG_NB_CA_TYPES
	// flash Trk -> CA weights
	glUniform4f(uniform_Update_fs_4fv_flashTrkCAWghts,
		flashTrkCA_weights[0], flashTrkCA_weights[1], flashTrkCA_weights[2], 0.f);
	// printf("flashTrkCA_weights %.2f %.2f %.2f \n", flashTrkCA_weights[0], flashTrkCA_weights[1], flashTrkCA_weights[2]);
#endif
#else
#if PG_NB_TRACKS >= 2
	// flash Trk -> BG weights
	glUniform4f(uniform_Update_fs_4fv_flashTrkBGWghts_flashPartBGWght,
		flashTrkBG_weights[1], 0.f, 0.f, flashPartBG_weight);
#ifdef PG_NB_CA_TYPES
	// flash Trk -> CA weights
	glUniform4f(uniform_Update_fs_4fv_flashTrkCAWghts,
		flashTrkCA_weights[0], flashTrkCA_weights[1], 0.f, 0.f);
	// printf("flashTrkCA_weights %.2f %.2f \n", flashTrkCA_weights[0],  flashTrkCA_weights[1]);
#endif
#else
#if PG_NB_TRACKS >= 1
#if defined (TEXTURED_QUAD_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES)
	// flash Trk -> BG weights
	glUniform4f(uniform_Update_fs_4fv_flashTrkBGWghts_flashPartBGWght,
		0.f, 0.f, 0.f, flashPartBG_weight);
#endif
#ifdef PG_NB_CA_TYPES
	// flash Trk -> CA weights
	glUniform4f(uniform_Update_fs_4fv_flashTrkCAWghts,
		flashTrkCA_weights[0], 0.f, 0.f, 0.f);
	// printf("flashTrkCA_weights %.2f %.2f \n", flashTrkCA_weights[0],  flashTrkCA_weights[1]);
#endif
#endif
#endif
#endif
#endif

	//printf("Signs %d \n", currentDrawingTrack);

	// CA type, frame no, flashback and current track
	glUniform3f(uniform_Update_fs_3fv_frameno_Cursor_flashPartCAWght,
		(GLfloat)pg_FrameNo,
		(GLfloat)CurrentCursorStylusvsRubber, flashPartCA_weight);

	// movie size, flash camera and copy tracks
	// copy to layer above (+1) or to layer below (-1)
	glUniform4f(uniform_Update_fs_4fv_movieWH_flashCameraTrkWght_cpTrack,
		(GLfloat)pg_movie_frame_width, (GLfloat)pg_movie_frame_height,
		flashCameraTrk_weight, (GLfloat)copyToNextTrack);
	// printf("Movie size %dx%d\n", pg_movie_frame_width, pg_movie_frame_height);
	// printf("Flash camera coef %.1f\n", flashCameraTrk_weight);
	if (copyToNextTrack != 0)
		printf("copyToNextTrack %d\n", copyToNextTrack);

#ifdef PG_WITH_PHOTO_FLASH
	// photo flash
#if defined (ETOILES_TEASER)
	photo_offsetX = 0;
	photo_offsetY = 0;
	if (pg_CurrentSceneIndex == 0 || pg_CurrentSceneIndex == 1) {
		photo_offsetY = 0.52f - pg_Translate_SVG_Text(pg_Ind_Current_DisplayText) / window_height;
	}
#endif
#if defined (PIERRES)
	float angle = photoJitterAmpl * (pulse[0] - 0.5f) * float(M_PI);
	float radius = photoJitterAmpl * (pulse[2] - 0.5f);
	// offset for POT coordinates
	photo_offsetX = radius * cos(angle) / window_width;
	photo_offsetY = radius * sin(angle) / window_height;
#endif
	glUniform4f(uniform_Update_fs_4fv_flashPhotoTrkWght_flashPhotoTrkThres_Photo_offSetsXY,
		flashPhotoTrk_weight, flashPhotoTrk_threshold, photo_offsetX, photo_offsetY);
#endif

	// flash CA -> BG & repop color (BG & CA)
	glUniform4f(uniform_Update_fs_4fv_repop_ColorBG_flashCABGWght,
		repop_ColorBG_r, repop_ColorBG_g, repop_ColorBG_b, flashCABG_weight);
#if defined(PG_NB_CA_TYPES) && !defined (GN) && !defined (ALKEMI)
	glUniform3f(uniform_Update_fs_3fv_repop_ColorCA,
		repop_ColorCA_r, repop_ColorCA_g, repop_ColorCA_b);
#endif

	// clear layer, flash pixel, flash CA -> Part
	glUniform3f(uniform_Update_fs_3fv_isClearLayer_flashPixel_flashCameraTrkThres,
		(GLfloat)isClearLayer, (GLfloat)flashPixel, flashCameraTrk_threshold);
	// photo size 
#if defined (TVW)
	glUniform4f(uniform_Update_fs_4fv_photo01_wh,
		(GLfloat)pg_Photo_buffer_data[0]->w, (GLfloat)pg_Photo_buffer_data[0]->h,
		(GLfloat)pg_Photo_buffer_data[1]->w, (GLfloat)pg_Photo_buffer_data[1]->h);
#endif

#if defined (PG_WITH_PHOTO_DIAPORAMA)
#ifdef PG_WITH_CLIPS
	// clip weights 
#if defined(PG_NB_PARALLEL_CLIPS) && PG_NB_PARALLEL_CLIPS >= 2
	glUniform4f(uniform_Update_fs_4fv_photo01_wh,
		float(clip_crop_width) / clip_image_width, float(clip_crop_height) / clip_image_height,
		float(clip_crop_width) / clip_image_width, float(clip_crop_height) / clip_image_height);
	float wl0 = 0.f;
	float wr0 = 0.f;
	float wl1 = 0.f;
	float wr1 = 0.f;
	if (playing_clipNoLeft >= 0 && playing_clipNoLeft < pg_nbClips) {
		wl0 = 1.f;
	}
	if (playing_clipNoRight >= 0 && playing_clipNoRight < pg_nbClips) {
		wr0 = 1.f;
	}
	if (playing_secondClipNoLeft >= 0 && playing_secondClipNoLeft < pg_nbClips) {
		wl1 = 1.f;
	}
	if (playing_secondClipNoRight >= 0 && playing_secondClipNoRight < pg_nbClips) {
		wr1 = 1.f;
	}
	if (clip_mix == 0) {
		glUniform4f(uniform_Update_fs_4fv_photo01Wghts_randomValues,
			wl0 * pg_clip_status[_clipLeft].clip_level, wr0 * pg_clip_status[_clipRight].clip_level, rand_0_1, rand_0_1);
		glUniform2f(uniform_Update_fs_2fv_clip01Wghts,
			wl1 * pg_clip_status[_clipLeft].clip_level, wr1 * pg_clip_status[_clipRight].clip_level);
	}
	else if (clip_mix < 0) {
		clip_mix = max(clip_mix, -0.5f);
		glUniform4f(uniform_Update_fs_4fv_photo01Wghts_randomValues,
			wl0 * (1.f + 2 * clip_mix) * pg_clip_status[_clipLeft].clip_level, wr0 * (1.f + 2 * clip_mix) * pg_clip_status[_clipRight].clip_level, rand_0_1, rand_0_1);
		glUniform2f(uniform_Update_fs_2fv_clip01Wghts,
			wl1 * pg_clip_status[_clipLeft].clip_level, wr1 * pg_clip_status[_clipRight].clip_level);
	}
	else if (clip_mix > 0) {
		clip_mix = min(clip_mix, 0.5f);
		glUniform4f(uniform_Update_fs_4fv_photo01Wghts_randomValues,
			wl0 * pg_clip_status[_clipLeft].clip_level, wr0 * pg_clip_status[_clipRight].clip_level, rand_0_1, rand_0_1);
		glUniform2f(uniform_Update_fs_2fv_clip01Wghts,
			wl1 * (1.f - 2 * clip_mix) * pg_clip_status[_clipLeft].clip_level, wr1 * (1.f - 2 * clip_mix) * pg_clip_status[_clipRight].clip_level);
	}
	else
#else
	if ((playing_clipNoLeft >= 0 && playing_clipNoLeft < pg_nbClips)
		|| (playing_clipNoRight >= 0 && playing_clipNoRight < pg_nbClips)) {
		glUniform4f(uniform_Update_fs_4fv_photo01_wh,
			float(clip_crop_width) / clip_image_width, float(clip_crop_height) / clip_image_height,
			float(clip_crop_width) / clip_image_width, float(clip_crop_height) / clip_image_height);
		//printf("photo WH %.2fx%.2f %.2fx%.2f\n",
		//	window_width_powerOf2_ratio, 1.0,
		//	window_width_powerOf2_ratio, 1.0);
		if (!(playing_clipNoLeft >= 0 && playing_clipNoLeft < pg_nbClips)) {
			glUniform4f(uniform_Update_fs_4fv_photo01Wghts_randomValues,
				0.0, pg_clip_status[_clipRight].clip_level, rand_0_1, rand_0_1);
		}
		else if (!(playing_clipNoRight >= 0 && playing_clipNoRight < pg_nbClips)) {
			glUniform4f(uniform_Update_fs_4fv_photo01Wghts_randomValues,
				pg_clip_status[_clipLeft].clip_level, 0.0, rand_0_1, rand_0_1);
		}
	}
	else 
#endif
#endif
	if (pg_CurrentDiaporamaDir >= 0) {
		glUniform4f(uniform_Update_fs_4fv_photo01_wh,
			window_width_powerOf2_ratio, window_height_powerOf2_ratio,
			window_width_powerOf2_ratio, window_height_powerOf2_ratio);
		//printf("photo WH %.2fx%.2f %.2fx%.2f\n",
		//	window_width_powerOf2_ratio, window_height_powerOf2_ratio,
		//	window_width_powerOf2_ratio, window_height_powerOf2_ratio);

		glUniform4f(uniform_Update_fs_4fv_photo01Wghts_randomValues,
			pg_Photo_weight[0], pg_Photo_weight[1], rand_0_1, rand_0_1);
		//glUniform4f(uniform_Update_fs_4fv_photo01Wghts_randomValues,
		//	0.f, 1.f, rand_0_1, rand_0_1);
		//printf("photo weight %.2f %.2f\n",
		//	pg_Photo_weight[0], pg_Photo_weight[1]);
	}
	else {
		glUniform4f(uniform_Update_fs_4fv_photo01Wghts_randomValues,
			0.f, 0.f, rand_0_1, rand_0_1);
		//printf("photo weight 0.f 0.f\n");
	}
#endif

	//printf("camera movie weight %.2f %.2f\n",
	//	*((float *)ScenarioVarPointers[_cameraWeight]), *((float *)ScenarioVarPointers[_movieWeight]));

#ifdef PG_WITH_CAMERA_CAPTURE
	// camera texture offset 
	glUniform4f(uniform_Update_fs_4fv_Camera_offSetsXY_Camera_W_H,
		(GLfloat)pg_camera_x_offset, (GLfloat)pg_camera_y_offset,
		(GLfloat)pg_camera_frame_width, (GLfloat)pg_camera_frame_height);
	//printf("Update shader camera frame size %dx%d offset %dx%d\n", pg_camera_frame_width, pg_camera_frame_height, pg_camera_x_offset, pg_camera_y_offset);
#endif

#if defined (TVW)
	// image buffer layer weights
	centralPhoto += photoJitterAmpl * fabs(rand_0_1 - 0.5f);
	while (centralPhoto < 0) centralPhoto += PG_PHOTO_NB_TEXTURES;
	while (centralPhoto >= PG_PHOTO_NB_TEXTURES) centralPhoto -= PG_PHOTO_NB_TEXTURES;
	// float images_weights_variance = 10.0;
	float dist[PG_PHOTO_NB_TEXTURES];
	for (int ind = 0; ind < PG_PHOTO_NB_TEXTURES; ind++) {
		//// the new value obtained by a gaussian around the central standard value
		dist[ind]
			= std::min(std::min(fabs(ind - centralPhoto),
				fabs(ind + PG_PHOTO_NB_TEXTURES - centralPhoto)),
				fabs(ind - PG_PHOTO_NB_TEXTURES - centralPhoto));
		pg_Photo_weightTVW[ind]
			= std::max(0.f, 0.2f * (exp(-(dist[ind] * dist[ind]) / PG_MASK_VARIANCE) - 0.5f));
		// std value variance 36.f
		pg_Photo_weightTVW[ind] = .35f;
	}
	//printf("weights %.1f %.1f %.1f %.1f %.1f %.1f \n", pg_Photo_weightTVW[0], pg_Photo_weightTVW[1], pg_Photo_weightTVW[2], pg_Photo_weightTVW[3], pg_Photo_weightTVW[4], pg_Photo_weightTVW[5]);


	glUniform4fv(uniform_Update_fs_4fv_weights03, 1, pg_Photo_weightTVW);
	glUniform2fv(uniform_Update_fs_2fv_weights45, 1, pg_Photo_weightTVW + 4);

	// interpolation weight between image buffer swap buffer in each la
	glUniform3fv(uniform_Update_fs_3fv_alphaSwap02, 1, pg_Photo_alphaSwap02);
	glUniform3fv(uniform_Update_fs_3fv_alphaSwap35, 1, pg_Photo_alphaSwap35);
	//printf("swap coefs %.1f %.1f %.1f %.1f %.1f %.1f \n", pg_Photo_alphaSwap02[0], pg_Photo_alphaSwap02[1], pg_Photo_alphaSwap02[2], pg_Photo_alphaSwap35[0], pg_Photo_alphaSwap35[1], pg_Photo_alphaSwap35[2]);
	//for (int ind = 0; ind < 3; ind++) {
	// pg_Photo_alphaSwap02[ind] += 0.001f;
	// pg_Photo_alphaSwap35[ind] += 0.001f;
	//}
	// printf("Jitter %.2f %.2f\n" ,maskJitterAmpl ,maskJitterPhase ); 
	// image buffer layer and masks coordinate offsets
	for (int ind = 0; ind < PG_PHOTO_NB_TEXTURES * 2; ind++) {
		float imageCoordOffest = photoJitterAmpl * (rand_0_1 - 0.5f);
		float maskCoordOffest = maskJitterAmpl * (rand_0_1 - 0.5f);
		if (pg_Photo_position_noises[ind] + imageCoordOffest < 2.f
			&& pg_Photo_position_noises[ind] + imageCoordOffest > -2.f) {
			pg_Photo_position_noises[ind] += imageCoordOffest;
		}
		else {
			pg_Photo_position_noises[ind] -= imageCoordOffest;
		}
		if (pg_Photo_mask_position_noises[ind] + maskCoordOffest < 2.f
			&& pg_Photo_mask_position_noises[ind] + maskCoordOffest > -2.f) {
			pg_Photo_mask_position_noises[ind] += maskCoordOffest;
		}
		else {
			pg_Photo_mask_position_noises[ind] -= maskCoordOffest;
		}
	}
	glUniform4fv(uniform_Update_fs_4fv_image_noisesxy, 3, pg_Photo_position_noises);
	glUniform4fv(uniform_Update_fs_4fv_mask_noisesxy, 3, pg_Photo_mask_position_noises);
#endif

	// track x & y translations
#ifdef PG_SLOW_TRACK_TRANSLATION
	float translation_x[2] = { 0.f, 0.f };
	float translation_y[2] = { 0.f, 0.f };
	if (int(abs(track_x_transl_0)) != 0 && pg_FrameNo % int(abs(track_x_transl_0)) == 0) {
		translation_x[0] = (track_x_transl_0 > 0 ? 1.f : -1.f);
	}
	if (int(abs(track_y_transl_0)) != 0 && pg_FrameNo % int(abs(track_y_transl_0)) == 0) {
		translation_y[0] = (track_y_transl_0 > 0 ? 1.f : -1.f);
	}
	if (int(abs(track_x_transl_1)) != 0 && pg_FrameNo % int(abs(track_x_transl_1)) == 0) {
		translation_x[1] = (track_x_transl_1 > 0 ? 1.f : -1.f);
	}
	if (int(abs(track_y_transl_1)) != 0 && pg_FrameNo % int(abs(track_y_transl_1)) == 0) {
		translation_y[1] = (track_y_transl_1 > 0 ? 1.f : -1.f);
	}
	glUniform4f(uniform_Update_fs_4fv_xy_transl_tracks_0_1,
		translation_x[0], translation_y[0], translation_x[1], translation_y[1]);
#else
	glUniform4f(uniform_Update_fs_4fv_xy_transl_tracks_0_1,
		track_x_transl_0, track_y_transl_0, track_x_transl_1, track_y_transl_1);
#endif

#ifdef PG_NB_CA_TYPES
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
		// printf("CA type/subtype %d-%d\n" , CAInterpolatedType, CAInterpolatedSubType);
	}
#endif

#if defined (PG_NB_CA_TYPES) || defined (PG_WITH_BLUR)
	glUniform4f(uniform_Update_fs_4fv_CAType_SubType_blurRadius,
#ifdef PG_NB_CA_TYPES
		GLfloat(CAInterpolatedType), GLfloat(CAInterpolatedSubType),
#else
		0.f, 0.f,
#endif
#if defined (PG_WITH_BLUR)
		(is_blur_1 ? float(blurRadius_1) : 0.f), (is_blur_2 ? float(blurRadius_2) : 0.f));
#else
		0.f, 0.f);
#endif
	// printf("CA type/subtype %d-%d\n" , CAInterpolatedType, CAInterpolatedSubType);
// printf("blur %.2f %.2f\n", (is_blur_1 ? float(blurRadius_1) : 0.f), (is_blur_2 ? float(blurRadius_2) : 0.f));
#endif

#ifdef PG_WITH_CLIPS
	glUniform3f(uniform_Update_fs_3fv_photo_rgb_0, pg_clip_status[_clipLeft].clip_r, pg_clip_status[_clipLeft].clip_g, pg_clip_status[_clipLeft].clip_b);
	glUniform3f(uniform_Update_fs_3fv_photo_rgb_1, pg_clip_status[_clipRight].clip_r, pg_clip_status[_clipRight].clip_g, pg_clip_status[_clipRight].clip_b);
	//printf("RGB L %.2f %.2f %.2f\n", pg_clip_status[_clipLeft].clip_r, pg_clip_status[_clipLeft].clip_g, pg_clip_status[_clipLeft].clip_b);
	//printf("RGB R %.2f %.2f %.2f\n", pg_clip_status[_clipRight].clip_r, pg_clip_status[_clipRight].clip_g, pg_clip_status[_clipRight].clip_b);
#else
	glUniform3f(uniform_Update_fs_3fv_photo_rgb_0, 1.f, 1.f, 1.f);
	glUniform3f(uniform_Update_fs_3fv_photo_rgb_1, 1.f, 1.f, 1.f);
#endif


#ifdef GN
	// sets the time of the 1st plane launch 
	if (CAInterpolatedType == CA_NEUMANN_BINARY && firstPlaneFrameNo < 0) {
		// printf("First plane frame no %d\n", firstPlaneFrameNo);
		firstPlaneFrameNo = pg_FrameNo - 200;
	}
	glUniform2f(uniform_Update_fs_2fv_initCA_1stPlaneFrameNo,
		initCA, GLfloat(firstPlaneFrameNo));
	// one shot CA launching
	if (initCA > 0.0f) {
		initCA = 0.0f;
	}
#endif
	if (CAInterpolatedType_prev != CAInterpolatedType
		|| CAInterpolatedSubType_prev != CAInterpolatedSubType) {
		sprintf(AuxString, "/CAType %d", CAInterpolatedType);
		pg_send_message_udp((char *)"i", AuxString, (char *)"udp_TouchOSC_send");
		sprintf(AuxString, "/CASubType %d", CAInterpolatedSubType);
		pg_send_message_udp((char *)"i", AuxString, (char *)"udp_TouchOSC_send");
		CAInterpolatedType_prev = CAInterpolatedType;
		CAInterpolatedSubType_prev = CAInterpolatedSubType;
	}
	printOglError(519);

	/////////////////////////////////////////////////////////////////////////
	// PARTICLE RENDERING SHADER UNIFORM VARIABLES

#if defined (TEXTURED_QUAD_PARTICLES) || defined (LINE_SPLAT_PARTICLES) 
	glUseProgram(shader_programme[pg_shader_ParticleRender]);
	glUniform4f(uniform_ParticleSplat_gs_4fv_part_size_partType_highPitchPulse_windowRatio,
		(part_size + pulse_average * part_size_pulse * part_size) / 512.f,
		(GLfloat)particle_type,
		pulse[2], float(window_width)/float(window_height));

	///////////////////////////////////////////////////////////////////////
	bool assigned = false;
	for (int indPath = 1; indPath < PG_NB_PATHS + 1; indPath++) {
		// active reading
		if (is_path_replay[indPath]) {
			glUniform3f(uniform_ParticleSplat_vp_3fv_trackReplay_xy_height, paths_x[indPath], paths_y[indPath], (float)window_height);
			assigned = true;
			break;
		}
	}
	if (!assigned) {
		if (paths_x[0] >= 0 && paths_y[0] >= 0) {
			glUniform3f(uniform_ParticleSplat_vp_3fv_trackReplay_xy_height, paths_x[0], paths_y[0], (float)window_height);
		}
		else {
			glUniform3f(uniform_ParticleSplat_vp_3fv_trackReplay_xy_height, -1000.f, -1000.f, (float)window_height);
		}
	}
#endif
#if defined (CURVE_PARTICLES) 
	glUseProgram(shader_programme[pg_shader_ParticleRender]);
	glUniform3f(uniform_ParticleCurve_gs_3fv_partRadius_partType_highPitchPulse,
		(part_size + pulse_average * part_size_pulse * part_size) / 512.f,
		(GLfloat)particle_type, pulse[2]);
	// printf("part radius GS %.2f\n", (part_size + pulse_average * part_size_pulse * part_size));

	///////////////////////////////////////////////////////////////////////
	bool assigned = false;
	for (int indPath = 1; indPath < PG_NB_PATHS + 1; indPath++) {
		// active reading
		if (is_path_replay[indPath]) {
			glUniform3f(uniform_ParticleCurve_vp_3fv_trackReplay_xy_height, paths_x[indPath], paths_y[indPath], (float)window_height);
			assigned = true;
			break;
		}
	}
	if (!assigned) {
		if (paths_x[0] >= 0 && paths_y[0] >= 0) {
			glUniform3f(uniform_ParticleCurve_vp_3fv_trackReplay_xy_height, paths_x[0], paths_y[0], (float)window_height);
		}
		else {
			glUniform3f(uniform_ParticleCurve_vp_3fv_trackReplay_xy_height, -1000.f, -1000.f, (float)window_height);
		}
	}
#endif
#if defined (TVW)
	// special case for army explosion: track 1 is assigned as repulse or follow path but is not replayed
	// the center of the screen is the default position for this track
	if (part_path_repulse_1 || part_path_follow_1) {
		glUniform2f(uniform_ParticleSplat_vp_3fv_trackReplay_xy_height, float(window_width / 2), float(window_height / 2));
	}
	// special case for army radar: track 2 is assigned as repop path but is not replayed
	// the center of the top left screen is the default position for this track
	else if (part_path_repop_2 || part_path_repulse_2) {
		glUniform2f(uniform_ParticleSplat_vp_3fv_trackReplay_xy_height, float(window_width / 4), float(window_height / 4));
	}
#endif

	//printf("pulsed particle radius %.4f rad %.4f pulse %.4f coef %.4f\n", 
	//	(part_size + pulse_average * part_size_pulse * part_size) / 512.f,
	//	part_size / 512.f, pulse_average, part_size_pulse);

	/////////////////////////////////////////////////////////////////////////
	// MIXING SHADER UNIFORM VARIABLES

	glUseProgram(shader_programme[pg_shader_Mixing]);

	// CA weight
#ifdef PG_WITH_PHOTO_FLASH
	glUniform3f(uniform_Mixing_fs_3fv_height_flashCameraTrkWght_flashPhotoTrkWght,
		(GLfloat)window_height, flashCameraTrk_weight, flashPhotoTrk_weight);
#else
	glUniform2f(uniform_Mixing_fs_2fv_height_flashCameraTrkWght,
		(GLfloat)window_height, flashCameraTrk_weight);
#endif

	// TEXT TRANSPARENCY
#if defined (TVW)
	glUniform3f(uniform_Mixing_fs_3fv_screenMsgTransp_Text1_2_Alpha,
		messageTransparency,
		(GLfloat)DisplayText1Alpha, (GLfloat)DisplayText2Alpha);
#else
	glUniform3f(uniform_Mixing_fs_3fv_screenMsgTransp_Text1_2_Alpha,
		messageTransparency,
		(GLfloat)1.f, (GLfloat)0.f);
#endif

	// printOglError(512);

	/////////////////////////////////////////////////////////////////////////
	// MASTER SHADER UNIFORM VARIABLES
	glUseProgram(shader_programme[pg_shader_Master]);
	// hoover cursor
	//printf("cur %d %d prev %d %d\n", CurrentCursorHooverPos_x, CurrentCursorHooverPos_y, tablet_prec_x, tablet_prec_y);
	mobile_cursor = ((tablet_prec_x != CurrentCursorHooverPos_x) || (tablet_prec_y != CurrentCursorHooverPos_y));
	tablet_prec_x = CurrentCursorHooverPos_x;
	tablet_prec_y = CurrentCursorHooverPos_y;
	glUniform4f(uniform_Master_fs_4fv_xy_frameno_pulsedShift,
		(GLfloat)CurrentCursorHooverPos_x, (GLfloat)CurrentCursorHooverPos_y,
		(GLfloat)pg_FrameNo, (pulse_average - pulse_average_prec) * track_x_transl_0_pulse);

	//printf("curseur %d %d\n", int(paths_x_forGPU[0]), int(paths_y_forGPU[0]));
	///////////////////////// cursor on first finger given up
		glUniform4f(uniform_Master_fs_4fv_xy_frameno_pulsedShift,
			(GLfloat)CurrentCursorHooverPos_x, (GLfloat)CurrentCursorHooverPos_y,
			(GLfloat)pg_FrameNo, (pulse_average - pulse_average_prec) * track_x_transl_0_pulse);


#if defined(GN) || defined(TVW) || defined(ALKEMI)
	int rightWindowVMargin = 0;
	if (double_window && window_width > 1920) {
		rightWindowVMargin = (window_width - 2 * leftWindowWidth) / 2;
	}
	// screen size
	glUniform4f(uniform_Master_fs_4fv_width_height_rightWindowVMargin_timeFromStart,
		(GLfloat)window_width, (GLfloat)window_height, GLfloat(rightWindowVMargin),
		GLfloat(CurrentClockTime - InitialScenarioTime));
#else
	// screen size
	//printf("time from start %.2f\n", GLfloat(CurrentClockTime - InitialScenarioTime));
		glUniform3f(uniform_Master_fs_3fv_width_height_timeFromStart,
		(GLfloat)window_width, (GLfloat)window_height, GLfloat(CurrentClockTime - InitialScenarioTime));
#endif
	// printf("mobile cursor %d\n", (mobile_cursor ? 1 : 0));
	glUniform2i(uniform_Master_fs_2iv_mobile_cursor_currentScene, (mobile_cursor ? 1 : 0), pg_CurrentSceneIndex);
#ifdef CAVERNEPLATON
	// high bandpass color
	glUniform3f(uniform_Master_fs_3fv_Caverne_BackColor_rgb,
		Caverne_BackColorRed, Caverne_BackColorGreen, Caverne_BackColorBlue);
#endif
#ifdef PG_SENSORS
	/////////////////////////////////////////////////////////////////////////
	// SENSOR SHADER UNIFORM VARIABLES
	// glUseProgram(shader_programme[pg_shader_Sensor]);
	// the variable of the sensor shader is updated individually before each sensor rendering
	// no update is made globally for all the sensors
#endif

#ifdef PG_MESHES
	/////////////////////////////////////////////////////////////////////////
	// MESH SHADER UNIFORM VARIABLES
	glUseProgram(shader_programme[pg_shader_Mesh]);
	// the variable of the mesh shader is updated before each rendering mode (lines of facets)
#if !defined(TEMPETE) && !defined(ENSO)
	glUniform3f(uniform_Mesh_fs_3fv_light, mesh_light_x, mesh_light_y, mesh_light_z);
#endif
#ifdef PG_AUGMENTED_REALITY
	glUniform4f(uniform_Mesh_fs_4fv_textureFrontier, textureFrontier_wmin, textureFrontier_wmax, textureFrontier_hmin, textureFrontier_hmax);
	glUniform4f(uniform_Mesh_fs_4fv_textureFrontier_width, textureFrontier_wmin_width, textureFrontier_wmax_width, textureFrontier_hmin_width, textureFrontier_hmax_width);
	glUniform4f(uniform_Mesh_fs_4fv_textureScaleTransl, textureScale_w, textureScale_h, textureTranslate_w, textureTranslate_h);
#endif
#endif

	printOglError(517);

}

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
// MULTIPASS RENDERING
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////
// PASS #0: PARTICLE ANIMATION PASS

#if defined (TEXTURED_QUAD_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 
void pg_ParticleAnimationPass(void) {
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, pg_FBO_ParticleAnimation[((pg_FrameNo + 1) % 2)]);
	// printf("FBO ANIMATION       %d\n", pg_FBO_ParticleAnimation[((pg_FrameNo + 1) % 2)]);

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	////////////////////////////////////////
	// activate shaders and sets uniform variable values    
	glUseProgram(shader_programme[pg_shader_ParticleAnimation]);

	glBindVertexArray(pg_vaoID[pg_VAOQuad]);
	
	glUniformMatrix4fv(uniform_ParticleAnimation_vp_proj, 1, GL_FALSE, pg_orthoWindowProjMatrix);
	glUniformMatrix4fv(uniform_ParticleAnimation_vp_view, 1, GL_FALSE, pg_identityViewMatrix);
	glUniformMatrix4fv(uniform_ParticleAnimation_vp_model, 1, GL_FALSE, pg_identityModelMatrix);

	// texture unit location
	glUniform1i(uniform_ParticleAnimation_texture_fs_Part_init_pos_speed, pg_Part_init_pos_speed_ParticleAnimation_sampler);
	glUniform1i(uniform_ParticleAnimation_texture_fs_Part_init_col_rad, pg_Part_init_col_rad_ParticleAnimation_sampler);
	glUniform1i(uniform_ParticleAnimation_texture_fs_Part_acc, pg_Part_acc_ParticleAnimation_sampler);
#ifdef PG_NB_CA_TYPES
	glUniform1i(uniform_ParticleAnimation_texture_fs_CA, pg_CA_FBO_ParticleAnimation_sampler);
#endif
	glUniform1i(uniform_ParticleAnimation_texture_fs_CA, pg_CA_FBO_ParticleAnimation_sampler);
	glUniform1i(uniform_ParticleAnimation_texture_fs_Part_pos_speed, pg_Part_pos_speed_FBO_ParticleAnimation_sampler);
	glUniform1i(uniform_ParticleAnimation_texture_fs_Part_col_rad, pg_Part_col_rad_FBO_ParticleAnimation_sampler);
	glUniform1i(uniform_ParticleAnimation_texture_fs_Part_Target_pos_col_rad, pg_Part_Target_pos_col_rad_FBO_ParticleAnimation_sampler);
	glUniform1i(uniform_ParticleAnimation_texture_fs_Noise, pg_Noise_ParticleAnimation_sampler);
#ifdef PG_WITH_REPOP_DENSITY
	glUniform1i(uniform_ParticleAnimation_texture_fs_RepopDensity, pg_RepopDensity_ParticleAnimation_sampler);
#endif
#ifdef PG_WITH_CAMERA_CAPTURE
	glUniform1i(uniform_ParticleAnimation_texture_fs_Camera_frame, pg_Camera_frame_ParticleAnimation_sampler);
#endif
	glUniform1i(uniform_ParticleAnimation_texture_fs_Movie_frame, pg_Movie_frame_ParticleAnimation_sampler);
	glUniform1i(uniform_ParticleAnimation_texture_fs_Trk0, pg_Trk0_FBO_ParticleAnimation_sampler);
#if PG_NB_TRACKS >= 2
	glUniform1i(uniform_ParticleAnimation_texture_fs_Trk1, pg_Trk1_FBO_ParticleAnimation_sampler);
#endif
#if PG_NB_TRACKS >= 3
	glUniform1i(uniform_ParticleAnimation_texture_fs_Trk2, pg_Trk2_FBO_ParticleAnimation_sampler);
#endif
#if PG_NB_TRACKS >= 4
	glUniform1i(uniform_ParticleAnimation_texture_fs_Trk3, pg_Trk3_FBO_ParticleAnimation_sampler);
#endif

	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP);

	// position speed are stored in a texture
	glActiveTexture(GL_TEXTURE0 + pg_Part_init_pos_speed_ParticleAnimation_sampler);
	if (part_initialization >= 0
		&& part_initialization < int(pg_particle_initial_pos_speed_texID.size())) {
		glBindTexture(GL_TEXTURE_RECTANGLE, pg_particle_initial_pos_speed_texID.at(part_initialization)); // pos - speed 
		printf("particle initialization (position speed) %d\n", part_initialization);
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
		&& part_initialization < int(pg_particle_initial_pos_speed_texID.size())) {
		glBindTexture(GL_TEXTURE_RECTANGLE, pg_particle_initial_color_radius_texID.at(part_initialization)); // color RGB - rad 
		printf("particle initialization (color radius) %d\n", part_initialization);
	}
	glActiveTexture(GL_TEXTURE0 + pg_Part_acc_ParticleAnimation_sampler);
	// acceleration is taken from photo 
	if (part_image_acceleration >= 0
		&& part_image_acceleration < int(pg_particle_acc_texID.size())) {
		glBindTexture(GL_TEXTURE_RECTANGLE, pg_particle_acc_texID.at(part_image_acceleration)); // color RGB - rad 
		printf("particle image initialization (image based initialization) %d\n", part_initialization);
	}


#ifdef PG_NB_CA_TYPES
	// 2-cycle ping-pong CA step n step n (FBO attachment 0)
	glActiveTexture(GL_TEXTURE0 + pg_CA_FBO_ParticleAnimation_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[(pg_FrameNo % 2) * PG_FBO_UPDATE_NBATTACHTS + pg_CA_FBO_Update_attcht]);
#endif

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
	glBindTexture(GL_TEXTURE_3D, Noise_texture_3D);

#ifdef PG_WITH_REPOP_DENSITY
	// noise texture (noise is also generated procedurally in the update shader)
	glActiveTexture(GL_TEXTURE0 + pg_RepopDensity_ParticleAnimation_sampler);
	if (Part_repop_density >= 0
		&& unsigned int(Part_repop_density) < pg_RepopDensity_texture_texID.size()) {
		glBindTexture(GL_TEXTURE_RECTANGLE, pg_RepopDensity_texture_texID.at(Part_repop_density));
	}
	else {
		glBindTexture(GL_TEXTURE_RECTANGLE, NULL_ID);
	}
#endif

	// camera texture produced at preceding pass
	// glActiveTexture(GL_TEXTURE0 + 7);
	// glBindTexture(GL_TEXTURE_RECTANGLE, FBO_CameraFrame_texID);
	// current camera texture
	glActiveTexture(GL_TEXTURE0 + pg_Camera_frame_ParticleAnimation_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_camera_texture_texID);

	// movie texture
	glActiveTexture(GL_TEXTURE0 + pg_Movie_frame_ParticleAnimation_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_movie_texture_texID);

	// 2-cycle ping-pong BG track step n (FBO attachment 5)
	glActiveTexture(GL_TEXTURE0 + pg_Trk0_FBO_ParticleAnimation_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[(pg_FrameNo % 2) * PG_FBO_UPDATE_NBATTACHTS + pg_Trk0_FBO_Update_attcht]);

#if PG_NB_TRACKS >= 2
	// 2-cycle ping-pong track 1 step n (FBO attachment 6)
	glActiveTexture(GL_TEXTURE0 + pg_Trk1_FBO_ParticleAnimation_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[(pg_FrameNo % 2) * PG_FBO_UPDATE_NBATTACHTS + pg_Trk1_FBO_Update_attcht]);
#endif

#if PG_NB_TRACKS >= 3
	// 2-cycle ping-pong track 2 step n (FBO attachment 7)
	glActiveTexture(GL_TEXTURE0 + pg_Trk2_FBO_ParticleAnimation_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[(pg_FrameNo % 2) * PG_FBO_UPDATE_NBATTACHTS + pg_Trk2_FBO_Update_attcht]);
#endif

#if PG_NB_TRACKS >= 4
	// 2-cycle ping-pong track 2 step n (FBO attachment 8)
	glActiveTexture(GL_TEXTURE0 + pg_Trk3_FBO_ParticleAnimation_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[(pg_FrameNo % 2) * PG_FBO_UPDATE_NBATTACHTS + pg_Trk3_FBO_Update_attcht]);
#endif

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
#endif

//////////////////////////////////////////////////
// PASS #1: UPDATE (CA, PIXELS, PARTICLES, DRAWING, PHOTOS & VIDEOS)

void pg_UpdatePass(void) {
	// ping pong output and input FBO bindings
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, pg_FBO_Update[((pg_FrameNo + 1) % 2)]);
	// printf("FBO UPDATE       %d\n", pg_FBO_Update[((pg_FrameNo + 1) % 2)]);

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	////////////////////////////////////////
	// activate shaders and sets uniform variable values    
	glUseProgram(shader_programme[pg_shader_Update]);
	glBindVertexArray(pg_vaoID[pg_VAOQuad]);
	glUniformMatrix4fv(uniform_Update_vp_proj, 1, GL_FALSE, pg_orthoWindowProjMatrix);
	glUniformMatrix4fv(uniform_Update_vp_view, 1, GL_FALSE, pg_identityViewMatrix);
	glUniformMatrix4fv(uniform_Update_vp_model, 1, GL_FALSE, pg_identityModelMatrix);

#if defined (PIERRES)
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
	glUniformMatrix3fv(uniform_Update_homographyForTexture, 1, GL_FALSE, pg_homographyForTexture);
#endif

	// texture unit location
#ifdef PG_NB_CA_TYPES
	glUniform1i(uniform_Update_texture_fs_CA, pg_CA_FBO_Update_sampler);
#endif
#ifdef PG_NB_PIXEL_MODES
	glUniform1i(uniform_Update_texture_fs_Pixels, pg_Pixels_FBO_Update_sampler);
#endif
#if !defined (PG_BEZIER_PATHS) || defined(PIERRES) || defined(ENSO) || defined(SONG) || defined(FORET) || defined (SOUNDINITIATIVE) || defined(ALKEMI)
	glUniform1i(uniform_Update_texture_fs_Brushes, pg_Brushes_Update_sampler);
#endif
#ifdef PG_WITH_CAMERA_CAPTURE
	glUniform1i(uniform_Update_texture_fs_Camera_frame, pg_Camera_frame_Update_sampler);
	glUniform1i(uniform_Update_texture_fs_Camera_BG, pg_Camera_BG_Update_sampler);
#endif
	glUniform1i(uniform_Update_texture_fs_Movie_frame, pg_Movie_frame_Update_sampler);
	glUniform1i(uniform_Update_texture_fs_Noise, pg_Noise_Update_sampler);
#ifdef PG_WITH_REPOP_DENSITY
	glUniform1i(uniform_Update_texture_fs_RepopDensity, pg_RepopDensity_Update_sampler);
#endif
#ifdef PG_WITH_PHOTO_DIAPORAMA
	glUniform1i(uniform_Update_texture_fs_Photo0, pg_Photo0_Update_sampler);
	glUniform1i(uniform_Update_texture_fs_Photo1, pg_Photo1_Update_sampler);
#endif
#if defined(PG_NB_PARALLEL_CLIPS) && PG_NB_PARALLEL_CLIPS >= 2
	glUniform1i(uniform_Update_texture_fs_Clip0, pg_Clip0_Update_sampler);
	glUniform1i(uniform_Update_texture_fs_Clip1, pg_Clip1_Update_sampler);
#endif
#if defined (TEXTURED_QUAD_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 
	glUniform1i(uniform_Update_texture_fs_Part_render, pg_Part_render_FBO_Update_sampler);
#endif
#if defined (TVW)
	glUniform1i(uniform_Update_texture_fs_TVWPixels0, pg_TVWPixels0);
	glUniform1i(uniform_Update_texture_fs_TVWPixels1, pg_TVWPixels1);
	glUniform1i(uniform_Update_texture_fs_TVWPixels2, pg_TVWPixels2);
	glUniform1i(uniform_Update_texture_fs_TVWPixels3, pg_TVWPixels3);
	glUniform1i(uniform_Update_texture_fs_TVWPixels4, pg_TVWPixels4);
	glUniform1i(uniform_Update_texture_fs_TVWPixels5, pg_TVWPixels5);
	glUniform1i(uniform_Update_texture_fs_TVWMask02, pg_TVWMask02);
	glUniform1i(uniform_Update_texture_fs_TVWMask35, pg_TVWMask35);
	glUniform1i(uniform_Update_texture_fs_TVWPixelsSwap0, pg_TVWPixelsSwap0);
	glUniform1i(uniform_Update_texture_fs_TVWPixelsSwap1, pg_TVWPixelsSwap1);
	glUniform1i(uniform_Update_texture_fs_TVWPixelsSwap2, pg_TVWPixelsSwap2);
	glUniform1i(uniform_Update_texture_fs_TVWPixelsSwap3, pg_TVWPixelsSwap3);
	glUniform1i(uniform_Update_texture_fs_TVWPixelsSwap4, pg_TVWPixelsSwap4);
	glUniform1i(uniform_Update_texture_fs_TVWPixelsSwap5, pg_TVWPixelsSwap5);
#endif
	glUniform1i(uniform_Update_texture_fs_Trk0, pg_Trk0_FBO_Update_sampler);
#if PG_NB_TRACKS >= 2
	glUniform1i(uniform_Update_texture_fs_Trk1, pg_Trk1_FBO_Update_sampler);
#endif
#if PG_NB_TRACKS >= 3
	glUniform1i(uniform_Update_texture_fs_Trk2, pg_Trk2_FBO_Update_sampler);
#endif
#if PG_NB_TRACKS >= 4
	glUniform1i(uniform_Update_texture_fs_Trk3, pg_Trk3_FBO_Update_sampler);
#endif
#if defined (GN) || defined (CAAUDIO) || defined(RIVETS)
	glUniform1i(uniform_Update_texture_fs_CATable, pg_CATable_Update_sampler);
#endif
#ifdef GN
	glUniform1i(uniform_Update_texture_fs_Camera_BGIni, pg_Camera_BGIni_FBO_Update_sampler);
#endif
#ifdef PG_WITH_BURST_MASK
	glUniform1i(uniform_Update_texture_fs_Burst_Mask, pg_Burst_Mask_FBO_Update_sampler);
#endif
	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP);

#ifdef PG_NB_CA_TYPES
	// 2-cycle ping-pong CA step n (FBO attachment 0)
	glActiveTexture(GL_TEXTURE0 + pg_CA_FBO_Update_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[(pg_FrameNo % 2) * PG_FBO_UPDATE_NBATTACHTS + pg_CA_FBO_Update_attcht]);
#endif

#ifdef PG_NB_PIXEL_MODES
	// 2-cycle ping-pong speed/position of pixels step n step n (FBO attachment 1)
	glActiveTexture(GL_TEXTURE0 + pg_Pixels_FBO_Update_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[(pg_FrameNo % 2) * PG_FBO_UPDATE_NBATTACHTS + pg_Pixels_FBO_Update_attcht]);
#endif

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// pen patterns
#if !defined (PG_BEZIER_PATHS) || defined(PIERRES) || defined(ENSO) || defined(SONG) || defined(FORET) || defined (SOUNDINITIATIVE) || defined(ALKEMI)
	glActiveTexture(GL_TEXTURE0 + pg_Brushes_Update_sampler);
	glBindTexture(GL_TEXTURE_3D, Pen_texture_3D_texID);
#endif

	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP);

	// camera texture produced at preceding pass
	// glActiveTexture(GL_TEXTURE0 + 7);
	// glBindTexture(GL_TEXTURE_RECTANGLE, FBO_CameraFrame_texID);
	// current camera texture
#ifdef PG_WITH_CAMERA_CAPTURE
	glActiveTexture(GL_TEXTURE0 + pg_Camera_frame_Update_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_camera_texture_texID);

	// current background texture
	glActiveTexture(GL_TEXTURE0 + pg_Camera_BG_Update_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_camera_BG_texture_texID);
#endif

	// movie texture
	glActiveTexture(GL_TEXTURE0 + pg_Movie_frame_Update_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_movie_texture_texID);

	// noise texture (noise is also generated procedurally in the update shader)
	glActiveTexture(GL_TEXTURE0 + pg_Noise_Update_sampler);
	glBindTexture(GL_TEXTURE_3D, Noise_texture_3D);

#ifdef PG_WITH_REPOP_DENSITY
	// repop density texture
	glActiveTexture(GL_TEXTURE0 + pg_RepopDensity_Update_sampler);
	if (BG_CA_repop_density >= 0
		&& unsigned int(BG_CA_repop_density) < pg_RepopDensity_texture_texID.size()) {
		glBindTexture(GL_TEXTURE_RECTANGLE, pg_RepopDensity_texture_texID.at(BG_CA_repop_density));
	}
	else {
		glBindTexture(GL_TEXTURE_RECTANGLE, NULL_ID);
	}
#endif

#ifdef PG_WITH_PHOTO_DIAPORAMA
	// photo[0] texture
	glActiveTexture(GL_TEXTURE0 + pg_Photo0_Update_sampler);
#ifdef PG_WITH_CLIPS
	if (playing_clipNoLeft >= 0 && playing_clipNoLeft < pg_nbClips
		&& pg_firstCompressedClipFramesInFolder[playing_clipNoLeft] 
			+ pg_clip_status[_clipLeft].get_lastFrame(0) < pg_nbCompressedClipFrames
		&& pg_firstCompressedClipFramesInFolder[playing_clipNoLeft] 
			+ pg_clip_status[_clipLeft].get_lastFrame(0) >= 0) {
			glBindTexture(GL_TEXTURE_2D, pg_ClipFrames_buffer_data[pg_firstCompressedClipFramesInFolder[playing_clipNoLeft] 
				+ pg_clip_status[_clipLeft].get_lastFrame(0)]->texBuffID);
	}
	else 
#endif
	if (pg_Photo_buffer_data && pg_nbCompressedImages >= 2
		&& pg_Photo_swap_buffer_data[0].indSwappedPhoto >= 0
		&& pg_Photo_swap_buffer_data[0].indSwappedPhoto < pg_nbCompressedImages) {
		glBindTexture(GL_TEXTURE_2D, pg_Photo_buffer_data[pg_Photo_swap_buffer_data[0].indSwappedPhoto]->texBuffID);
		//if (pg_FrameNo % 60 == 0) {
			//printf("texture ID indCompressedImage %d\n", pg_Photo_buffer_data[pg_Photo_swap_buffer_data[0].indSwappedPhoto]->texBuffID);
		//}
	}
	else {
		glBindTexture(GL_TEXTURE_2D, NULL_ID);
		//if (pg_FrameNo % 60 == 0) {
		//	printf("texture ID NULL\n");
		//}
	}

	// photo[1] texture
	glActiveTexture(GL_TEXTURE0 + pg_Photo1_Update_sampler);
#ifdef PG_WITH_CLIPS
	if (playing_clipNoRight >= 0 && playing_clipNoRight < pg_nbClips
		&& pg_firstCompressedClipFramesInFolder[playing_clipNoRight]
		+ pg_clip_status[_clipRight].get_lastFrame(0) < pg_nbCompressedClipFrames
		&& pg_firstCompressedClipFramesInFolder[playing_clipNoRight]
		+ pg_clip_status[_clipRight].get_lastFrame(0) >= 0) {
		glBindTexture(GL_TEXTURE_2D, pg_ClipFrames_buffer_data[pg_firstCompressedClipFramesInFolder[playing_clipNoRight]
			+ pg_clip_status[_clipRight].get_lastFrame(0)]->texBuffID);
	}
	else 
#endif
	if (pg_Photo_buffer_data && pg_nbCompressedImages >= 2
		&& pg_Photo_swap_buffer_data[1].indSwappedPhoto >= 0
		&& pg_Photo_swap_buffer_data[1].indSwappedPhoto < pg_nbCompressedImages) {
		glBindTexture(GL_TEXTURE_2D, pg_Photo_buffer_data[pg_Photo_swap_buffer_data[1].indSwappedPhoto]->texBuffID);
		//if (pg_FrameNo % 60 == 0) {
		//	printf("texture ID indCompressedImage %d\n", pg_Photo_buffer_data[pg_Photo_swap_buffer_data[1].indSwappedPhoto]->texBuffID);
		//}
	}
	else {
		glBindTexture(GL_TEXTURE_2D, NULL_ID);
		//if (pg_FrameNo % 60 == 0) {
		//	printf("texture ID NULL\n");
		//}
	}
#endif

	// clip[0] texture
#if defined(PG_NB_PARALLEL_CLIPS) && PG_NB_PARALLEL_CLIPS >= 2
	glActiveTexture(GL_TEXTURE0 + pg_Clip0_Update_sampler);
	if (playing_secondClipNoLeft >= 0 && playing_secondClipNoLeft < pg_nbClips
		&& pg_firstCompressedClipFramesInFolder[playing_secondClipNoLeft]
		+ pg_clip_status[_clipLeft].get_lastFrame(1) < pg_nbCompressedClipFrames
		&& pg_firstCompressedClipFramesInFolder[playing_secondClipNoLeft]
		+ pg_clip_status[_clipLeft].get_lastFrame(1) >= 0) {
		glBindTexture(GL_TEXTURE_2D, pg_ClipFrames_buffer_data[pg_firstCompressedClipFramesInFolder[playing_secondClipNoLeft]
			+ pg_clip_status[_clipLeft].get_lastFrame(1)]->texBuffID);
	}
	else {
		glBindTexture(GL_TEXTURE_2D, NULL_ID);
		//if (pg_FrameNo % 60 == 0) {
		//	printf("texture ID NULL\n");
		//}
	}
#endif
	// clip[1] texture
#if defined(PG_NB_PARALLEL_CLIPS) && PG_NB_PARALLEL_CLIPS >= 2
	glActiveTexture(GL_TEXTURE0 + pg_Clip1_Update_sampler);
	if (playing_secondClipNoRight >= 0 && playing_secondClipNoRight < pg_nbClips
		&& pg_firstCompressedClipFramesInFolder[playing_secondClipNoRight]
		+ pg_clip_status[_clipRight].get_lastFrame(1) < pg_nbCompressedClipFrames
		&& pg_firstCompressedClipFramesInFolder[playing_secondClipNoRight]
		+ pg_clip_status[_clipRight].get_lastFrame(1) >= 0) {
		glBindTexture(GL_TEXTURE_2D, pg_ClipFrames_buffer_data[pg_firstCompressedClipFramesInFolder[playing_secondClipNoRight]
			+ pg_clip_status[_clipRight].get_lastFrame(1)]->texBuffID);
	}
	else {
		glBindTexture(GL_TEXTURE_2D, NULL_ID);
		//if (pg_FrameNo % 60 == 0) {
		//	printf("texture ID NULL\n");
		//}
	}
#endif

#if defined (TEXTURED_QUAD_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 
	// FBO capture of particle rendering used for flashing layers with particles
	glActiveTexture(GL_TEXTURE0 + pg_Part_render_FBO_Update_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_ParticleRendering_texID);
#endif

#if defined (TVW)
	glActiveTexture(GL_TEXTURE0 + pg_TVWPixels0);
	glBindTexture(GL_TEXTURE_2D, 
		pg_Photo_buffer_data[pg_Photo_swap_buffer_data[0].indOldPhoto]->texBuffID);
	// image buffer texture #1
	glActiveTexture(GL_TEXTURE0 + pg_TVWPixels1);
	glBindTexture(GL_TEXTURE_2D,
		pg_Photo_buffer_data[pg_Photo_swap_buffer_data[1].indOldPhoto]->texBuffID);
	// image buffer texture #2
	glActiveTexture(GL_TEXTURE0 + pg_TVWPixels2);
	glBindTexture(GL_TEXTURE_2D,
		pg_Photo_buffer_data[pg_Photo_swap_buffer_data[2].indOldPhoto]->texBuffID);
	// image buffer texture #3
	glActiveTexture(GL_TEXTURE0 + pg_TVWPixels3);
	glBindTexture(GL_TEXTURE_2D,
		pg_Photo_buffer_data[pg_Photo_swap_buffer_data[3].indOldPhoto]->texBuffID);
	// image buffer texture #4
	glActiveTexture(GL_TEXTURE0 + pg_TVWPixels4);
	glBindTexture(GL_TEXTURE_2D,
		pg_Photo_buffer_data[pg_Photo_swap_buffer_data[4].indOldPhoto]->texBuffID);
	// image buffer texture #5
	glActiveTexture(GL_TEXTURE0 + pg_TVWPixels5);
	glBindTexture(GL_TEXTURE_2D,
		pg_Photo_buffer_data[pg_Photo_swap_buffer_data[5].indOldPhoto]->texBuffID);
	// image buffer mask #0
	glActiveTexture(GL_TEXTURE0 + pg_TVWMask02);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_Photo_mask_buffer_data[0].texBuffID);
	// image buffer mask #1
	glActiveTexture(GL_TEXTURE0 + pg_TVWMask35);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_Photo_mask_buffer_data[1].texBuffID);
	// buffer swap image #0
	glActiveTexture(GL_TEXTURE0 + pg_TVWPixelsSwap0);
	glBindTexture(GL_TEXTURE_2D, 
		pg_Photo_buffer_data[pg_Photo_swap_buffer_data[0].indSwappedPhoto]->texBuffID);
	// buffer swap image #1
	glActiveTexture(GL_TEXTURE0 + pg_TVWPixelsSwap1);
	glBindTexture(GL_TEXTURE_2D,
		pg_Photo_buffer_data[pg_Photo_swap_buffer_data[1].indSwappedPhoto]->texBuffID);
	// buffer swap image #2
	glActiveTexture(GL_TEXTURE0 + pg_TVWPixelsSwap2);
	glBindTexture(GL_TEXTURE_2D,
		pg_Photo_buffer_data[pg_Photo_swap_buffer_data[2].indSwappedPhoto]->texBuffID);
	// buffer swap image #3
	glActiveTexture(GL_TEXTURE0 + pg_TVWPixelsSwap3);
	glBindTexture(GL_TEXTURE_2D,
		pg_Photo_buffer_data[pg_Photo_swap_buffer_data[3].indSwappedPhoto]->texBuffID);
	// buffer swap image #4
	glActiveTexture(GL_TEXTURE0 + pg_TVWPixelsSwap4);
	glBindTexture(GL_TEXTURE_2D,
		pg_Photo_buffer_data[pg_Photo_swap_buffer_data[4].indSwappedPhoto]->texBuffID);
	// buffer swap image #5
	glActiveTexture(GL_TEXTURE0 + pg_TVWPixelsSwap5);
	glBindTexture(GL_TEXTURE_2D,
		pg_Photo_buffer_data[pg_Photo_swap_buffer_data[5].indSwappedPhoto]->texBuffID);
#endif

	//printf("texID %02d/%02d %02d/%02d %02d/%02d %02d/%02d %02d/%02d %02d/%02d\n",
		//pg_Photo_buffer_data[pg_Photo_swap_buffer_data[0].indOldPhoto]->texBuffID,
		//pg_Photo_buffer_data[pg_Photo_swap_buffer_data[0].indSwappedPhoto]->texBuffID,
		//pg_Photo_buffer_data[pg_Photo_swap_buffer_data[1].indOldPhoto]->texBuffID,
		//pg_Photo_buffer_data[pg_Photo_swap_buffer_data[1].indSwappedPhoto]->texBuffID,
		//pg_Photo_buffer_data[pg_Photo_swap_buffer_data[2].indOldPhoto]->texBuffID,
		//pg_Photo_buffer_data[pg_Photo_swap_buffer_data[2].indSwappedPhoto]->texBuffID,
		//pg_Photo_buffer_data[pg_Photo_swap_buffer_data[3].indOldPhoto]->texBuffID,
		//pg_Photo_buffer_data[pg_Photo_swap_buffer_data[3].indSwappedPhoto]->texBuffID,
		//pg_Photo_buffer_data[pg_Photo_swap_buffer_data[4].indOldPhoto]->texBuffID,
		//pg_Photo_buffer_data[pg_Photo_swap_buffer_data[4].indSwappedPhoto]->texBuffID,
		//pg_Photo_buffer_data[pg_Photo_swap_buffer_data[5].indOldPhoto]->texBuffID,
		//pg_Photo_buffer_data[pg_Photo_swap_buffer_data[5].indSwappedPhoto]->texBuffID);

	// 2-cycle ping-pong BG track step n (FBO attachment 5)
	glActiveTexture(GL_TEXTURE0 + pg_Trk0_FBO_Update_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[(pg_FrameNo % 2) * PG_FBO_UPDATE_NBATTACHTS + pg_Trk0_FBO_Update_attcht]);

#if PG_NB_TRACKS >= 2
	// 2-cycle ping-pong track 1 step n (FBO attachment 6)
	glActiveTexture(GL_TEXTURE0 + pg_Trk1_FBO_Update_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[(pg_FrameNo % 2) * PG_FBO_UPDATE_NBATTACHTS + pg_Trk1_FBO_Update_attcht]);
#endif

#if PG_NB_TRACKS >= 3
	// 2-cycle ping-pong track 2 step n (FBO attachment 7)
	glActiveTexture(GL_TEXTURE0 + pg_Trk2_FBO_Update_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[(pg_FrameNo % 2) * PG_FBO_UPDATE_NBATTACHTS + pg_Trk2_FBO_Update_attcht]);
#endif

#if PG_NB_TRACKS >= 4
	// 2-cycle ping-pong track 2 step n (FBO attachment 8)
	glActiveTexture(GL_TEXTURE0 + pg_Trk3_FBO_Update_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[(pg_FrameNo % 2) * PG_FBO_UPDATE_NBATTACHTS + pg_Trk3_FBO_Update_attcht]);
#endif

#if defined (GN) || defined (CAAUDIO) || defined(RIVETS)
	// CA Data table (FBO attachment 11)
	glActiveTexture(GL_TEXTURE0 + pg_CATable_Update_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_CATable_ID);
#endif 

#ifdef GN
	// Initial background texture
	glActiveTexture(GL_TEXTURE0 + pg_Camera_BGIni_FBO_Update_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_camera_BGIni_texture_texID);
#endif

#ifdef PG_WITH_BURST_MASK
	// Master mask texture
	glActiveTexture(GL_TEXTURE0 + pg_Burst_Mask_FBO_Update_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, Burst_Mask_texID);
#endif

	// printf("photo IDs %d %d\n", pg_Photo_buffer_data[0].texBuffID, pg_Photo_buffer_data[1].texBuffID);

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

	printOglError(551);
}

/////////////////////////////////////
// PASS #2: PARTICLE, TEXT, SVG CLIP ART RENDERING PASS

#if defined (TEXTURED_QUAD_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 
void pg_SVGandParticleRenderingPass(void) {
	// printf("nb paticles %d\^n", nb_particles);
	// draws the Bezier curve

#ifndef PG_AUGMENTED_REALITY
	if (pg_FrameNo > 0) {
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, pg_FBO_ParticleRendering);
	}

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#endif

	////////////////////////////////////////
	// SVG TEXT OR CLIP ART RENDERING    
	////////////////////////////////////////
#if defined (ETOILES_TEASER)
	if (pg_CurrentSceneIndex == 0) {
		pg_Display_SVG_Text(&pg_Ind_Current_DisplayText, false);
	}
	else if (pg_CurrentSceneIndex == 5) {
		pg_Display_SVG_Text(&pg_Ind_Current_DisplayText, true);
	}
#else
#ifdef PG_WITH_CLIP_ART
	pg_Display_All_SVG_ClipArt(activeClipArts);
	printOglError(5256);
#endif
#endif

	////////////////////////////////////////
	// PARTICLE RENDERING    
	////////////////////////////////////////
#if defined (TEXTURED_QUAD_PARTICLES)
	glClear(GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
	//glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
#endif

	//glDisable(GL_BLEND);
	//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	////////////////////////////////////////
	// activate shaders and sets uniform variable values    
#if defined (TEXTURED_QUAD_PARTICLES) || defined (LINE_SPLAT_PARTICLES) 
	glUseProgram(shader_programme[pg_shader_ParticleRender]);

	glUniformMatrix4fv(uniform_ParticleSplat_vp_proj, 1, GL_FALSE, pg_orthoWindowProjMatrix);
	glUniformMatrix4fv(uniform_ParticleSplat_vp_view, 1, GL_FALSE, pg_identityViewMatrix);
	glUniformMatrix4fv(uniform_ParticleSplat_vp_model, 1, GL_FALSE, pg_identityModelMatrix);

	// texture unit location
	// position/speed Particle update
	glUniform1i(uniform_ParticleSplat_texture_vp_Part_pos_speed, 0);
	// color/radius Particle update
	glUniform1i(uniform_ParticleSplat_texture_vp_Part_col_rad, 1); 

#if defined (TEXTURED_QUAD_PARTICLES)
	// blurred disk texture
	glUniform1i(uniform_ParticleSplat_BlurredDisk_texture_fs_decal, 2);
#endif
#endif
#if defined (CURVE_PARTICLES) 
	glUseProgram(shader_programme[pg_shader_ParticleRender]);

	glUniformMatrix4fv(uniform_ParticleCurve_vp_proj, 1, GL_FALSE, pg_orthoWindowProjMatrix);
	glUniformMatrix4fv(uniform_ParticleCurve_vp_view, 1, GL_FALSE, pg_identityViewMatrix);
	glUniformMatrix4fv(uniform_ParticleCurve_vp_model, 1, GL_FALSE, pg_identityModelMatrix);

	// texture unit location
	// position/speed Particle update
	glUniform1i(uniform_ParticleCurve_texture_vp_Part_pos_speed, 0);
	// color/radius Particle update
	glUniform1i(uniform_ParticleCurve_texture_vp_Part_col_rad, 1); 

	// comet texture
	glUniform1i(uniform_ParticleCurve_Comet_texture_fs_decal, 2);
#endif
	printOglError(5256);

	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP);

	// 2-cycle ping-pong bezier Particle update
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_ParticleAnimation_texID[(pg_FrameNo % 2) * PG_FBO_PARTICLEANIMATION_NBATTACHTS + pg_Part_pos_speed_FBO_ParticleAnimation_attcht]);

	// 2-cycle ping-pong bezier Particle update
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_ParticleAnimation_texID[(pg_FrameNo % 2) * PG_FBO_PARTICLEANIMATION_NBATTACHTS + pg_Part_col_rad_FBO_ParticleAnimation_attcht]);

#ifdef CURVE_PARTICLES
	// comet texture
	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_2D, comet_texture_2D_texID);
#endif
#if defined (TEXTURED_QUAD_PARTICLES)
	// blurred disk texture
	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_2D, blurredDisk_texture_2D_texID);
#endif
	printOglError(5259);

	////////////////////////////////////////
	// binds geometry and displays it    
	// vertex buffer for a patch, made of 4 points: 4 x 3 floats
	glBindVertexArray(pg_vaoID[pg_VAOParticle]);

#ifdef CURVE_PARTICLES
	// patch vertices for curve particles fed into tesselation shader
	glPatchParameteri(GL_PATCH_VERTICES, (PG_PARTICLE_CURVE_DEGREE + 1));  // number of vertices in each patch
#endif

	// Index buffer for indexed rendering
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pg_vboID[pg_EAOParticle]);

#ifdef CURVE_PARTICLES
	// Draw the patches !
	glDrawElements(
		GL_PATCHES,      // mode
		nb_particles * (PG_PARTICLE_CURVE_DEGREE + 1),    // count
		GL_UNSIGNED_INT,   // type
		(void*)0           // element array buffer offset
	);
#endif
#if defined (TEXTURED_QUAD_PARTICLES) || defined (LINE_SPLAT_PARTICLES)
	// Draw the patches !
	glDrawElements(
		GL_POINTS,      // mode
		nb_particles,    // count
		GL_UNSIGNED_INT,   // type
		(void*)0           // element array buffer offset
	);
#endif

	// unbinds VBO
	glBindVertexArray(0);
	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

#if defined (TEXTURED_QUAD_PARTICLES)
	glDisable(GL_BLEND);
#endif

}
#endif

//////////////////////////////////////////////////
// PASS #3: COMPOSITION + PING-PONG ECHO: ECHOED MIX OF LAYERS

void pg_MixingPass(void) {
	// outputs inside a buffer that can be used for accumulation
	if (pg_FrameNo > 0) {
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, pg_FBO_Mixing_capturedFB_prec[(pg_FrameNo % 2)]); //  drawing memory on odd and even frames for echo
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
	glUseProgram(shader_programme[pg_shader_Mixing]);
	glBindVertexArray(pg_vaoID[pg_VAOQuad]);

	glUniformMatrix4fv(uniform_Mixing_vp_proj, 1, GL_FALSE, pg_orthoWindowProjMatrix);
	glUniformMatrix4fv(uniform_Mixing_vp_view, 1, GL_FALSE, pg_identityViewMatrix);
	glUniformMatrix4fv(uniform_Mixing_vp_model, 1, GL_FALSE, pg_identityModelMatrix);

	// texture unit location
#ifdef PG_NB_CA_TYPES
	glUniform1i(uniform_Mixing_texture_fs_CA, pg_CA_FBO_Mixing_sampler);
#endif
#if defined (TEXTURED_QUAD_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 
	glUniform1i(uniform_Mixing_texture_fs_Part_render, pg_Part_render_FBO_Mixing_sampler);
#endif
	glUniform1i(uniform_Mixing_texture_fs_Render_prec, pg_Render_prec_FBO_Mixing_sampler);

	glUniform1i(uniform_Mixing_texture_fs_Screen_Font, pg_Screen_Font_FBO_Mixing_sampler);
	glUniform1i(uniform_Mixing_texture_fs_Screen_Message, pg_Screen_Message_FBO_Mixing_sampler);
#if defined (TVW)
	glUniform1i(uniform_Mixing_texture_fs_Display_Font, pg_Display_Font_FBO_Mixing_sampler);
	glUniform1i(uniform_Mixing_texture_fs_Display_Message1, pg_Display_Message1_FBO_Mixing_sampler);
	glUniform1i(uniform_Mixing_texture_fs_Display_Message2, pg_Display_Message2_FBO_Mixing_sampler);
#endif
	glUniform1i(uniform_Mixing_texture_fs_Trk0, pg_Trk0_FBO_Mixing_sampler);
#if PG_NB_TRACKS >= 2
	glUniform1i(uniform_Mixing_texture_fs_Trk1, pg_Trk1_FBO_Mixing_sampler);
#endif
#if PG_NB_TRACKS >= 3
	glUniform1i(uniform_Mixing_texture_fs_Trk2, pg_Trk2_FBO_Mixing_sampler);
#endif
#if PG_NB_TRACKS >= 4
	glUniform1i(uniform_Mixing_texture_fs_Trk3, pg_Trk3_FBO_Mixing_sampler);
#endif

	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP);

#ifdef PG_NB_CA_TYPES
	// 2-cycle ping-pong CA step n + 1 (FBO attachment 0)
	glActiveTexture(GL_TEXTURE0 + pg_CA_FBO_Mixing_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * PG_FBO_UPDATE_NBATTACHTS + pg_CA_FBO_Update_attcht]);
#endif

#if defined (TEXTURED_QUAD_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 
	// Particles step n
	glActiveTexture(GL_TEXTURE0 + pg_Part_render_FBO_Mixing_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_ParticleRendering_texID);
#endif

	// preceding snapshot for echo
	glActiveTexture(GL_TEXTURE0 + pg_Render_prec_FBO_Mixing_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Mixing_capturedFB_prec_texID[((pg_FrameNo + 1) % 2)]);  // drawing memory on odd and even frames for echo 

	// screen font texture
	glActiveTexture(GL_TEXTURE0 + pg_Screen_Font_FBO_Mixing_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, Screen_Font_texture_Rectangle_texID);

	// message offsets texture
	glActiveTexture(GL_TEXTURE0 + pg_Screen_Message_FBO_Mixing_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_screenMessageBitmap_texID);

#if defined (TVW)
	// display font texture
	glActiveTexture(GL_TEXTURE0 + pg_Display_Font_FBO_Mixing_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, Display_Font_texture_Rectangle_texID);

	// tweets display message offsets texture
	glActiveTexture(GL_TEXTURE0 + pg_Display_Message1_FBO_Mixing_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_Display_Message1_Bitmap_texID);

	// tweets display message offsets texture
	glActiveTexture(GL_TEXTURE0 + pg_Display_Message2_FBO_Mixing_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_Display_Message2_Bitmap_texID);
#endif


	// 2-cycle ping-pong track 0 step n + 1 (FBO attachment 3)
	glActiveTexture(GL_TEXTURE0 + pg_Trk0_FBO_Mixing_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * PG_FBO_UPDATE_NBATTACHTS + pg_Trk0_FBO_Update_attcht]);

#if PG_NB_TRACKS >= 2
	// 2-cycle ping-pong track 1 step n + 1 (FBO attachment 4)
	glActiveTexture(GL_TEXTURE0 + pg_Trk1_FBO_Mixing_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * PG_FBO_UPDATE_NBATTACHTS + pg_Trk1_FBO_Update_attcht]);
#endif

#if PG_NB_TRACKS >= 3
	// 2-cycle ping-pong track 2 step n + 1 (FBO attachment 5)
	glActiveTexture(GL_TEXTURE0 + pg_Trk2_FBO_Mixing_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * PG_FBO_UPDATE_NBATTACHTS + pg_Trk2_FBO_Update_attcht]);
#endif

#if PG_NB_TRACKS >= 4
	// 2-cycle ping-pong track 3 step n + 1 (FBO attachment 6)
	glActiveTexture(GL_TEXTURE0 + pg_Trk3_FBO_Mixing_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * PG_FBO_UPDATE_NBATTACHTS + pg_Trk3_FBO_Update_attcht]);
#endif

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
}

//////////////////////////////////////////////////
// PASS #4: FINAL DISPLAY: MIX OF ECHOED AND NON-ECHOED LAYERS

void pg_MasterPass(void) {
#ifdef PG_AUGMENTED_REALITY
	if (pg_FrameNo > 0) {
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, pg_FBO_Master_capturedFB_prec); //  master output memory for mapping on mesh
	}
#else
	// unbind output FBO 
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
#endif

	// sets viewport to double window
	glViewport(0, 0, doubleWindowWidth, window_height);

#ifndef PG_AUGMENTED_REALITY
	glDrawBuffer(GL_BACK);
#endif

	// output video buffer clean-up
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	////////////////////////////////////////
	// activate shaders and sets uniform variable values    
	glUseProgram(shader_programme[pg_shader_Master]);
	glBindVertexArray(pg_vaoID[pg_VAOQuadMaster]);

	glUniformMatrix4fv(uniform_Master_vp_proj, 1, GL_FALSE, doubleProjMatrix);
	glUniformMatrix4fv(uniform_Master_vp_view, 1, GL_FALSE, pg_identityViewMatrix);
	glUniformMatrix4fv(uniform_Master_vp_model, 1, GL_FALSE, pg_identityModelMatrix);

	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP);
	// texture unit locations
	glUniform1i(uniform_Master_texture_fs_Render_curr, pg_Render_curr_FBO_Master_sampler);
#ifdef PG_NB_CA_TYPES
	glUniform1i(uniform_Master_texture_fs_CA, pg_CA_FBO_Master_sampler);
#endif

#if defined (TEXTURED_QUAD_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 
	glUniform1i(uniform_Master_texture_fs_Part_render, pg_Part_render_FBO_Master_sampler);
#endif
	glUniform1i(uniform_Master_texture_fs_Trk0, pg_Trk0_FBO_Master_sampler);
#if PG_NB_TRACKS >= 2
	glUniform1i(uniform_Master_texture_fs_Trk1, pg_Trk1_FBO_Master_sampler);
#endif
#if PG_NB_TRACKS >= 3
	glUniform1i(uniform_Master_texture_fs_Trk2, pg_Trk2_FBO_Master_sampler);
#endif
#if PG_NB_TRACKS >= 4
	glUniform1i(uniform_Master_texture_fs_Trk3, pg_Trk3_FBO_Master_sampler);
#endif
#ifdef PG_WITH_MASTER_MASK
	glUniform1i(uniform_Master_texture_fs_Mask, pg_Mask_FBO_Master_sampler);
#endif

#if defined (GN) || defined (INTERFERENCE)
	glUniform1i(uniform_Master_texture_fs_LYMlogo, pg_LYMlogo_Master_sampler);
#endif

	// Mixing pass output (echoed composition of tracks)
	glActiveTexture(GL_TEXTURE0 + pg_Render_curr_FBO_Master_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Mixing_capturedFB_prec_texID[(pg_FrameNo % 2)]);

#ifdef PG_NB_CA_TYPES
	// 2-cycle ping-pong CA step n (FBO attachment 0)
	glActiveTexture(GL_TEXTURE0 + pg_CA_FBO_Master_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * PG_FBO_UPDATE_NBATTACHTS + pg_CA_FBO_Update_attcht]);
#endif

#if defined (TEXTURED_QUAD_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 
	// Particles step n 
	glActiveTexture(GL_TEXTURE0 + pg_Part_render_FBO_Master_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_ParticleRendering_texID);
#endif

	// 2-cycle ping-pong track 0 step n (FBO attachment 3)
	glActiveTexture(GL_TEXTURE0 + pg_Trk0_FBO_Master_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * PG_FBO_UPDATE_NBATTACHTS + pg_Trk0_FBO_Update_attcht]);

#if PG_NB_TRACKS >= 2
	// 2-cycle ping-pong track 1 step n (FBO attachment 4)
	glActiveTexture(GL_TEXTURE0 + pg_Trk1_FBO_Master_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * PG_FBO_UPDATE_NBATTACHTS + pg_Trk1_FBO_Update_attcht]);
#endif

#if PG_NB_TRACKS >= 3
	// 2-cycle ping-pong track 2 step n (FBO attachment 5)
	glActiveTexture(GL_TEXTURE0 + pg_Trk2_FBO_Master_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * PG_FBO_UPDATE_NBATTACHTS + pg_Trk2_FBO_Update_attcht]);
#endif

#if PG_NB_TRACKS >= 4
	// 2-cycle ping-pong track 3 step n (FBO attachment 6)
	glActiveTexture(GL_TEXTURE0 + pg_Trk3_FBO_Master_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * PG_FBO_UPDATE_NBATTACHTS + pg_Trk3_FBO_Update_attcht]);
#endif
#ifdef PG_WITH_MASTER_MASK
	// Master mask texture
	glActiveTexture(GL_TEXTURE0 + pg_Mask_FBO_Master_sampler);
	if (nb_layers_master_mask > 0) {
		glBindTexture(GL_TEXTURE_3D, Master_Multilayer_Mask_texID);
	}
	else {
		glBindTexture(GL_TEXTURE_RECTANGLE, Master_Mask_texID);
	}
#endif
#if defined (GN)
	// LYM logo
	glActiveTexture(GL_TEXTURE0 + pg_LYMlogo_Master_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_LYMlogo_texID);
#endif


	//if (pg_FrameNo % 1000 <= 1) {
	//	printf("Final check texID 0-5 %d %d %d %d %d %d\n\n",
	//		pg_FBO_Mixing_capturedFB_prec_texID[(pg_FrameNo % 2)],
	//		pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * PG_FBO_UPDATE_NBATTACHTS],
	//		pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * PG_FBO_UPDATE_NBATTACHTS + 2],
	//		pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * PG_FBO_UPDATE_NBATTACHTS + 3],
	//		pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * PG_FBO_UPDATE_NBATTACHTS + 4],
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

#ifdef PG_SENSORS
//////////////////////////////////////////////////
// PASS #5: SENSOR PASS
void pg_SensorPass(void) {
	////////////////////////////////////////
	// drawing sensors
	// activate transparency
	//printf("sensor pass\n");
	glEnable(GL_BLEND);
	glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

	// activate shaders and sets uniform variable values    
	glUseProgram(shader_programme[pg_shader_Sensor]);
	glBindVertexArray(quadSensor_vao);

	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP);
	// texture unit location
	glUniform1i(uniform_Sensor_texture_fs_decal, 0);
	// previous pass output
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_RECTANGLE, Sensor_texture_rectangle);

	glUniformMatrix4fv(uniform_Sensor_vp_view, 1, GL_FALSE, pg_identityViewMatrix);
	glUniformMatrix4fv(uniform_Sensor_vp_proj, 1, GL_FALSE, doubleProjMatrix);
	printOglError(597);

	for (int indSens = 0; indSens < PG_NB_SENSORS; indSens++) {
		int reindexed_Sensor = Sensor_order[indSens];
		if (sensor_onOff[reindexed_Sensor]) {
			//printf("sensor index %d reindex %d\n", indSens, reindexed_Sensor);
			modelMatrixSensor[12] = (sensorPositions[3 * reindexed_Sensor] + 0.5f - window_width / 2.0f) + window_width / 2.0f;
			modelMatrixSensor[13] = (sensorPositions[3 * reindexed_Sensor + 1] + 0.5f - window_height / 2.0f) + window_height / 2.0f;
			modelMatrixSensor[14] = sensorPositions[3 * reindexed_Sensor + 2];
			glUniformMatrix4fv(uniform_Sensor_vp_model, 1, GL_FALSE, modelMatrixSensor);

			// sensorLevel[indSens]
			glUniform4f(uniform_Sensor_fs_4fv_onOff_isCurrentSensor_isFollowMouse_transparency,
				(sensor_onOff[reindexed_Sensor] ? 1.0f : -1.0f), (reindexed_Sensor == currentSensor ? 1.0f : -1.0f),
				(sensorFollowMouse_onOff ? 1.0f : -1.0f), sensorLevel[reindexed_Sensor]);

			// draw points from the currently bound VAO with current in-use shader
			glDrawArrays(GL_TRIANGLES, 0, 3 * 2);
		}
	}
	printOglError(596);

	// duplicates the sensors in case of double window
	if (double_window) {
		int rightWindowVMargin = 0;
		if (window_width > 1920) {
			rightWindowVMargin = (window_width - 2 * leftWindowWidth) / 2;
		}
		for (int indSens = 0; indSens < PG_NB_SENSORS; indSens++) {
			int reindexed_Sensor = Sensor_order[indSens];
			if (sensor_onOff[reindexed_Sensor]) {
				modelMatrixSensor[12] = (sensorPositions[3 * reindexed_Sensor] + 0.5f - leftWindowWidth / 2.0f) + 3 * leftWindowWidth / 2.0f + rightWindowVMargin;
				modelMatrixSensor[13] = (sensorPositions[3 * reindexed_Sensor + 1] + 0.5f - window_height / 2.0f) + window_height / 2.0f;
				modelMatrixSensor[14] = sensorPositions[3 * reindexed_Sensor + 2];
				glUniformMatrix4fv(uniform_Sensor_vp_model, 1, GL_FALSE, modelMatrixSensor);

				glUniform4f(uniform_Sensor_fs_4fv_onOff_isCurrentSensor_isFollowMouse_transparency,
					(sensor_onOff[reindexed_Sensor] ? 1.0f : -1.0f), (reindexed_Sensor == currentSensor ? 1.0f : -1.0f),
					(sensorFollowMouse_onOff ? 1.0f : -1.0f), sensorLevel[reindexed_Sensor]);

				// draw points from the currently bound VAO with current in-use shader
				glDrawArrays(GL_TRIANGLES, 0, 3 * 2);
			}
		}
	}

	printOglError(595);
	glDisable(GL_BLEND);
}
#endif

#if defined (PIERRES)
// initializes the homography matrices for the distortion of the projected image
void pg_calculate_homography_matrices(std::vector<cv::Point2f> *sourcePoints, std::vector<cv::Point2f> *destinationPoints, GLfloat matValues[], int dim) {
	//////////////////////////////////////////////////////////////////////////////////
	// right camera (left display)

	///////////////////////// HOMOGRPAHY
	// This homography transforms the points projected at near=1 into the four
	// points measured experimetally corresponding to the 4 vertices of the
	// quad projected at 1m with the projector axis orthogonal to the projection plane

	cv::Mat homography = cv::findHomography(*sourcePoints, *destinationPoints, 0);
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
#endif

#ifdef PG_MESHES

// initializes the transformation matrices related to the frustum
// should only be called when the values are changed through pd or maxmsp
void pg_calculate_projection_matrices(void) {
	//printf("[1] Loc %.1f %.1f %.1f LookAt %.1f %.1f %.1f\n",
	//	VP1LocX, VP1LocY, VP1LocZ, VP1LookAtX, VP1LookAtY, VP1LookAtZ);

	//////////////////////////////////////////////////////////////////////////////////
	// right camera (right display)
	if (double_window) {
#ifdef TEMPETE
		VP1perspMatrix
			= glm::frustum(-VP1WidthTopAt1m / 2.0f, VP1WidthTopAt1m / 2.0f, VP1BottomAt1m * 2.0f, VP1TopAt1m * 2.0f, nearPlane, farPlane);
#else
		VP1perspMatrix
			= glm::frustum(-VP1WidthTopAt1m / 2.0f, VP1WidthTopAt1m / 2.0f, VP1BottomAt1m, VP1TopAt1m, nearPlane, farPlane);
#endif
	}
	else {
		VP1perspMatrix
			= glm::frustum(-VP1WidthTopAt1m / 2.0f, VP1WidthTopAt1m / 2.0f, VP1BottomAt1m, VP1TopAt1m, nearPlane, farPlane);
	}
	//printf("Perspective 1 %.2f %.2f %.2f %.2f near Far %.2f %.2f\n" , -VP1WidthTopAt1m / 2.0f, VP1WidthTopAt1m / 2.0f, VP1BottomAt1m, VP1TopAt1m, nearPlane, farPlane);

	// Camera matrix
	// float eyePosition[3] = {10.1f,4.6f,4.4f};
	// the projection of the bottom of the screen is known
	// we have to deduce the look at from this point and the measure of 
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
			lookAtPoint, // and looks at lookAtPoint
			glm::vec3(0, VP1UpY, (VP1Reversed ? -1.0f : 1.0f))  // Head is up (set to 0, VP1UpY, 1 or 0, VP1UpY, -1 if projector is upside down)
		);

#ifdef PG_SECOND_MESH_CAMERA
	//////////////////////////////////////////////////////////////////////////////////
	// left camera (right display)
#ifdef TEMPETE
	VP2perspMatrix
		= glm::frustum(-VP2WidthTopAt1m / 2.0f, VP2WidthTopAt1m / 2.0f, VP2BottomAt1m * 2.0f, VP2TopAt1m * 2.0f, nearPlane, farPlane);
#else
	VP2perspMatrix
		= glm::frustum(-VP2WidthTopAt1m / 2.0f, VP2WidthTopAt1m / 2.0f, VP2BottomAt1m, VP2TopAt1m, nearPlane, farPlane);
#endif
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

//////////////////////////////////////////////////
// PASS #6: MESH PASS

#ifdef ETOILES_TEASER
bool mesh_guided_by_strokes(int indMeshFile) {
	// meshes are guided by strokes
	int path_no = indMeshFile + 1;
	bool visible = false;

	// brings the path coordinates to the normal cube
	if (path_no <= PG_NB_PATHS) {
		visible = (is_path_replay[path_no] && paths_x[path_no] > 0 && paths_y[path_no] > 0);
		if (visible) {
			// normal pen coordinates
			pen_x = (paths_x[path_no] / window_width) * 2.f - 1.f;
			pen_y = (paths_y[path_no] / window_height) * 2.f - 1.f;
		}
		else {
			bool isTrackRecord = false;
			switch (path_no) {
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
			visible = isTrackRecord && paths_x[0] > 0 && paths_y[0] > 0;
			if (visible) {
				// normal pen coordinates
				pen_x = (paths_x[0] / window_width) * 2.f - 1.f;
				pen_y = (paths_y[0] / window_height) * 2.f - 1.f;
			}
		}
	}
	return visible;
}

void ray_animation(int indMeshFile) {
	// rotates and scales a ray so that it follows a pen
	// vector from ray center to pen
	vec_x = pen_x + pg_Mesh_Translation_X[indMeshFile];
	vec_y = pen_y + pg_Mesh_Translation_Y[indMeshFile];
	// angle from ray center to pen -> ray angle
	if (vec_x != 0) {
		pg_Mesh_Rotation_angle[indMeshFile] = atan(vec_y / vec_x);
	}
	else {
		pg_Mesh_Rotation_angle[indMeshFile] = 0;
	}
	if (vec_x > 0) {
		pg_Mesh_Rotation_angle[indMeshFile] += float(M_PI);
	}
	// ray size so that the tip of the ray coincides with the pen
	float norm_vec = sqrt(vec_x * vec_x + vec_y * vec_y);
	pg_Mesh_Scale[indMeshFile] = norm_vec;
}
#endif

#ifdef CAVERNEPLATON
void automatic_rotation(int indMeshFile) {
	// rotation update
	if (indMeshFile < 7) {
		pg_Mesh_Rotation_angle[indMeshFile] += 0.03f;
		pg_Mesh_Rotation_X[indMeshFile] += 0.01f;
		pg_Mesh_Rotation_Y[indMeshFile] += 0.01f;
		pg_Mesh_Rotation_Z[indMeshFile] += 0.01f;
		// translation update
		if (mobileMeshes & (1 << indMeshFile)) {
			pg_Mesh_Translation_X[indMeshFile] += pg_Mesh_Motion_X[indMeshFile] * 2.f;
			pg_Mesh_Translation_Y[indMeshFile] += pg_Mesh_Motion_Y[indMeshFile] * 2.f;
			pg_Mesh_Translation_Z[indMeshFile] += pg_Mesh_Motion_Z[indMeshFile] * 2.f;
		}
	}
	else {
		pg_Mesh_Rotation_angle[indMeshFile] += 0.06f * rand_0_1;
		pg_Mesh_Rotation_X[indMeshFile] += (0.2f * rand_0_1 - 0.1f);
		pg_Mesh_Rotation_Y[indMeshFile] += (0.2f * rand_0_1 - 0.1f);
		pg_Mesh_Rotation_Z[indMeshFile] += (0.2f * rand_0_1 - 0.1f);
		// translation update
		pg_Mesh_Translation_X[indMeshFile] += pg_Mesh_Motion_X[indMeshFile] * 4.f * (rand_0_1 - 0.5f);
		pg_Mesh_Translation_Y[indMeshFile] += pg_Mesh_Motion_Y[indMeshFile] * 4.f * (rand_0_1 - 0.5f);
		pg_Mesh_Translation_Z[indMeshFile] += pg_Mesh_Motion_Z[indMeshFile] * 4.f * (rand_0_1 - 0.5f);
	}
}
#endif

void pg_MeshPass(void) {
	float eyePosition[3] = { 20.f, 0.f, 0.f };
	float lookat[3] = { 0.f, 0.f, 0.f };

#ifndef PG_AUGMENTED_REALITY
	// draws the meshes on top of particles
	if (pg_FrameNo > 0) {
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, pg_FBO_ParticleRendering);
	}
	// transparency
	glEnable(GL_BLEND);
	// glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
#else
	if (!directRenderingwithoutMeshScreen1) {
		// draws the meshes alone
		// unbind output FBO 
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		// draws on back buffer
		glDrawBuffer(GL_BACK);

		// no transparency
		glDisable(GL_BLEND);
#endif

		// output buffer cleanup
		glClear(GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		//glDepthMask(GL_FALSE);
		//glDepthFunc(GL_LESS);

		////////////////////////////////////////
		// drawing meshes

		// activate shaders and sets uniform variable values    
		glUseProgram(shader_programme[pg_shader_Mesh]);

		// calculates the view and perspective matrices according to the parameters in the scenario file
		// for one camera if single screen, and two otherwise
		pg_calculate_projection_matrices();

#ifdef PG_WITH_HOMOGRAPHY
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
		*homographyMatrix = (glm::make_mat4(matValues));
#endif

		// sets viewport to single window
		if (double_window) {
#ifdef TEMPETE
			glViewport(0, window_height / 2, leftWindowWidth, window_height);
#else
			glViewport(0, 0, leftWindowWidth, window_height);
#endif
		}
		else {
			glViewport(0, 0, window_width, window_height);
			//printf("glViewport %d %d %d %d\n", 0, 0, window_width, window_height);
		}

		// projection and view matrices
		glUniformMatrix4fv(uniform_Mesh_vp_proj, 1, GL_FALSE,
			// glm::value_ptr(VP1homographyMatrix * VP1perspMatrix));
			glm::value_ptr(VP1perspMatrix));
		glUniformMatrix4fv(uniform_Mesh_vp_view, 1, GL_FALSE,
			glm::value_ptr(VP1viewMatrix));

		for (int indMeshFile = 0; indMeshFile < pg_nb_Mesh_files; indMeshFile++) {
			// visibility
			bool visible = false;
#ifdef ETOILES_TEASER
			visible = mesh_guided_by_strokes(indMeshFile);
#else
#ifdef CAVERNEPLATON
			visible = (indMeshFile < 7 && (activeMeshes & (1 << indMeshFile))) || (pg_CaverneActveMesh[indMeshFile]
				&& (CurrentClockTime - pg_CaverneMeshBirthTime[indMeshFile] > pg_CaverneMeshWakeupTime[indMeshFile])
				&& (CurrentClockTime < pg_CaverneMeshDeathTime[indMeshFile]));
#else
			visible = (activeMeshes & (1 << indMeshFile));
#endif
#endif
			// visible mesh
			if (visible) {
				// mesh animation
#ifdef CAVERNEPLATON
				automatic_rotation(indMeshFile);
#endif
#ifdef ETOILES_TEASER
				ray_animation();
#endif

				// Model matrix 
				// transformed mesh according to scenario file
				// 1. a varying translation matrix
				MeshPosModelMatrix = glm::translate(glm::mat4(1.0f),
					glm::vec3(pg_Mesh_Translation_X[indMeshFile], pg_Mesh_Translation_Y[indMeshFile], pg_Mesh_Translation_Z[indMeshFile]));
				// 2. a varying rotation matrix 
				glm::vec3 myRotationAxis(pg_Mesh_Rotation_X[indMeshFile],
					pg_Mesh_Rotation_Y[indMeshFile], pg_Mesh_Rotation_Z[indMeshFile]);
				MeshPosModelMatrix = glm::rotate(MeshPosModelMatrix, pg_Mesh_Rotation_angle[indMeshFile], myRotationAxis);
				// 3. a varying scaling matrix 
#if defined (ETOILES_TEASER)
				MeshPosModelMatrix = glm::scale(MeshPosModelMatrix, glm::vec3(pg_Mesh_Scale[indMeshFile]));
#elif defined (CAVERNEPLATON)
				if (indMeshFile < 7) {
					MeshPosModelMatrix = glm::scale(MeshPosModelMatrix, glm::vec3(pg_Mesh_Scale[indMeshFile]));
				}
				else {
					MeshPosModelMatrix = glm::scale(MeshPosModelMatrix, glm::vec3(pg_Mesh_Scale[indMeshFile] * min(2.0f, (CurrentClockTime - pg_CaverneMeshBirthTime[indMeshFile]) / 20.f)));
				}
#else
				MeshPosModelMatrix = glm::scale(MeshPosModelMatrix, glm::vec3(pg_Mesh_Scale[indMeshFile]));
#endif
				// model matrix transfered to GPU
				glUniformMatrix4fv(uniform_Mesh_vp_model, 1, GL_FALSE,
					glm::value_ptr(MeshPosModelMatrix));

				for (int indMeshInFile = 0; indMeshInFile < nbMeshesPerMeshFile[indMeshFile]; indMeshInFile++) {
					// binds VAO
					glBindVertexArray(mesh_vao[indMeshFile][indMeshInFile]);

					// activate shaders and sets uniform variable values    
					glUseProgram(shader_programme[pg_shader_Mesh]);

					glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP);
					glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP);
					// texture unit location
					glUniform1i(uniform_Mesh_texture_fs_decal, 0);
#ifdef PG_MESHES
					glActiveTexture(GL_TEXTURE0 + 0);
					if (pg_Mesh_TextureRank[indMeshFile] != -1) {
						// specific texture
						glBindTexture(GL_TEXTURE_RECTANGLE, Mesh_texture_rectangle[indMeshFile]);
					}
					else {
						// previous pass output
						// mapping echo output (GL_TEXTURE_RECTANGLE, pg_FBO_Mixing_capturedFB_prec_texID[(pg_FrameNo % 2)]);  // drawing memory on odd and even frames for echo 
#ifdef PG_AUGMENTED_REALITY
						glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Master_capturedFB_prec_texID);  // master output memory for mapping on mesh
#endif
					}
#endif
					// standard filled mesh drawing
					// draw triangles from the currently bound VAO with current in-use shader
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_index_vbo[indMeshFile][indMeshInFile]);
					glUseProgram(shader_programme[pg_shader_Mesh]);

					// updates this variable according whether triangles or lines are shown
					glUniform4f(uniform_Mesh_fs_4fv_isDisplayLookAt_with_mesh_with_blue_currentScene, isDisplayLookAt, 0, with_blue, (GLfloat)pg_CurrentSceneIndex);
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					glDrawElements(GL_TRIANGLES, nbFacesPerMesh[indMeshFile][indMeshInFile] * 3, GL_UNSIGNED_INT, (GLvoid*)0);
					printOglError(698);

#if defined(CAVERNEPLATON) 
					// draws the polygon contours
					// updates this variable according whether triangles or lines are shown
					glUniform4f(uniform_Mesh_fs_4fv_isDisplayLookAt_with_mesh_with_blue_currentScene, isDisplayLookAt, 1, with_blue, (GLfloat)pg_CurrentSceneIndex);
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
					glDrawElements(GL_TRIANGLES, nbFacesPerMesh[indMeshFile][indMeshInFile] * 3, GL_UNSIGNED_INT, (GLvoid*)0);
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif
#ifdef PG_AUGMENTED_REALITY
					// optional additional drawing of the polygon contours for checking calibration in augmented reality
					if (with_mesh) {
						// no z-Buffer
						glDisable(GL_DEPTH_TEST);
						glLineWidth(3);
						glUniform4f(uniform_Mesh_fs_4fv_isDisplayLookAt_with_mesh_with_blue_currentScene, isDisplayLookAt, 1, with_blue, (GLfloat)pg_CurrentSceneIndex);
						glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
						glDrawElements(GL_TRIANGLES, nbFacesPerMesh[indMeshFile][indMeshInFile] * 3, GL_UNSIGNED_INT, (GLvoid*)0);
						glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
						// no z-Buffer
						glEnable(GL_DEPTH_TEST);
					}
#endif
					//printf("Display mesh VP1 %d/%d size (nb faces) %d\n\n", indMeshFile + 1, nbMeshesPerMeshFile[indMeshFile],
					//	nbFacesPerMesh[indMeshFile][indMeshInFile]);


				} // submeshes
			} // visible mesh
		} // all the meshes
		printOglError(697);
	}
	else {
		glBindFramebuffer(GL_READ_FRAMEBUFFER, pg_FBO_Master_capturedFB_prec); //  master output memory for mapping on mesh
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glDrawBuffer(GL_BACK);
		glBlitFramebuffer(0, 0, leftWindowWidth, window_height, 0, 0, leftWindowWidth, window_height,
			GL_COLOR_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0); //  unbind read buffer
		printOglError(599);
	}

#ifdef PG_SECOND_MESH_CAMERA
	if (!directRenderingwithoutMeshScreen2) {
		// sets viewport to second window
#ifdef TEMPETE
		glViewport(0, 0, leftWindowWidth, window_height / 2);
#else
		glViewport(leftWindowWidth , 0, leftWindowWidth, window_height);
#endif
		// duplicates the Meshs in case of double window

		glUniformMatrix4fv(uniform_Mesh_vp_proj, 1, GL_FALSE,
			// glm::value_ptr(VP2homographyMatrix * VP2perspMatrix));
			glm::value_ptr(VP2perspMatrix));
		glUniformMatrix4fv(uniform_Mesh_vp_view, 1, GL_FALSE,
			glm::value_ptr(VP2viewMatrix));

		for (int indMeshFile = 0; indMeshFile < pg_nb_Mesh_files; indMeshFile++) {
			// visibility
			bool visible = false;
			visible = (activeMeshes & (1 << indMeshFile));

			// visible mesh
			if (visible) {

				// Model matrix 
				// transformed mesh according to scenario file
				// 1. a varying translation matrix
				MeshPosModelMatrix = glm::translate(glm::mat4(1.0f),
					glm::vec3(pg_Mesh_Translation_X[indMeshFile], pg_Mesh_Translation_Y[indMeshFile], pg_Mesh_Translation_Z[indMeshFile]));
				// 2. a varying rotation matrix 
				glm::vec3 myRotationAxis(pg_Mesh_Rotation_X[indMeshFile],
					pg_Mesh_Rotation_Y[indMeshFile], pg_Mesh_Rotation_Z[indMeshFile]);
				MeshPosModelMatrix = glm::rotate(MeshPosModelMatrix, pg_Mesh_Rotation_angle[indMeshFile], myRotationAxis);
				// 3. a varying scaling matrix 
				MeshPosModelMatrix = glm::scale(MeshPosModelMatrix, glm::vec3(pg_Mesh_Scale[indMeshFile]));

				//printf("tr %.2f %.2f %.2f rot  %.2f %.2f %.2f (rot %.2f) scale  %.2f\n", pg_Mesh_Translation_X[indMeshFile], pg_Mesh_Translation_Y[indMeshFile], pg_Mesh_Translation_Z[indMeshFile], pg_Mesh_Rotation_X[indMeshFile],
				//	pg_Mesh_Rotation_Y[indMeshFile], pg_Mesh_Rotation_Z[indMeshFile], pg_Mesh_Rotation_angle[indMeshFile], pg_Mesh_Scale[indMeshFile]);

				// model matrix transfered to GPU
				glUniformMatrix4fv(uniform_Mesh_vp_model, 1, GL_FALSE,
					glm::value_ptr(MeshPosModelMatrix));

				for (int indMeshInFile = 0; indMeshInFile < nbMeshesPerMeshFile[indMeshFile]; indMeshInFile++) {
					// binds VAO
					glBindVertexArray(mesh_vao[indMeshFile][indMeshInFile]);

					// activate shaders and sets uniform variable values    
					glUseProgram(shader_programme[pg_shader_Mesh]);

					glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP);
					glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP);
					// texture unit location
					glUniform1i(uniform_Mesh_texture_fs_decal, 0);
#ifdef PG_MESHES
					glActiveTexture(GL_TEXTURE0 + 0);
					if (pg_Mesh_TextureRank[indMeshFile] != -1) {
						// specific texture
						glBindTexture(GL_TEXTURE_RECTANGLE, Mesh_texture_rectangle[indMeshFile]);
					}
					else {
						// previous pass output
						// mapping echo output (GL_TEXTURE_RECTANGLE, pg_FBO_Mixing_capturedFB_prec_texID[(pg_FrameNo % 2)]);  // drawing memory on odd and even frames for echo 
#ifdef PG_AUGMENTED_REALITY
						glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Master_capturedFB_prec_texID);  // master output memory for mapping on mesh
#endif
					}
#endif
					// standard filled mesh drawing
					// draw triangles from the currently bound VAO with current in-use shader
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_index_vbo[indMeshFile][indMeshInFile]);
					glUseProgram(shader_programme[pg_shader_Mesh]);

					// updates this variable according whether triangles or lines are shown
					glUniform4f(uniform_Mesh_fs_4fv_isDisplayLookAt_with_mesh_with_blue_currentScene, isDisplayLookAt, 0, with_blue, (GLfloat)pg_CurrentSceneIndex);

					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					glDrawElements(GL_TRIANGLES, nbFacesPerMesh[indMeshFile][indMeshInFile] * 3, GL_UNSIGNED_INT, (GLvoid*)0);

#ifdef PG_AUGMENTED_REALITY
					// optional additional drawing of the polygon contours for checking calibration in augmented reality
					if (with_mesh) {
						// no z-Buffer
						glDisable(GL_DEPTH_TEST);
						glLineWidth(3);
						glUniform4f(uniform_Mesh_fs_4fv_isDisplayLookAt_with_mesh_with_blue_currentScene, isDisplayLookAt, 1, with_blue, (GLfloat)pg_CurrentSceneIndex);
						glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
						glDrawElements(GL_TRIANGLES, nbFacesPerMesh[indMeshFile][indMeshInFile] * 3, GL_UNSIGNED_INT, (GLvoid*)0);
						glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
						// no z-Buffer
						glEnable(GL_DEPTH_TEST);
					}
#endif
					//printf("Display mesh VP2 %d/%d size (nb faces) %d\n", indMeshFile, indMeshInFile,
					//	nbFacesPerMesh[indMeshFile][indMeshInFile]);


				} // submeshes
			} // visible mesh
		} // all the meshes

		// sets viewport to full window
		glViewport(0, 0, window_width, window_height);
#endif

		glDisable(GL_DEPTH_TEST);
		// no transparency
		glDisable(GL_BLEND);
		printOglError(598);
	}
	else {
		//printf("Direct copy of Master shader output right window to back buffer\n");
		glBindFramebuffer(GL_READ_FRAMEBUFFER, pg_FBO_Master_capturedFB_prec); //  master output memory for mapping on mesh
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glDrawBuffer(GL_BACK);
		glBlitFramebuffer(leftWindowWidth, 0, 2 * leftWindowWidth, window_height, leftWindowWidth, 0, 2 * leftWindowWidth, window_height,
			GL_COLOR_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0); //  unbind read buffer
		printOglError(599);
	}
}
#endif


//////////////////////////////////////////////////////////////////////////////////////////////
// DRAWING A SCENE ON VARIOUS MODALITIES (CURVE, IMAGE, FRAMEBUFFER...)
//////////////////////////////////////////////////////////////////////////////////////////////
void pg_draw_scene(DrawingMode mode, bool threaded) {
	// ******************** Svg output ********************
	if (mode == _Svg) {
		threadData* pData = new threadData;
		pData->fname = new char[512];
		pData->w = window_width;
		pData->h = window_height;
		indSvgSnapshot++;

		sprintf(pData->fname, "%s%s-%s-%04d.svg",
			snapshots_dir_path_name.c_str(),
			Svg_file_name.c_str(),
			date_stringStream.str().c_str(),
			indSvgSnapshot);
		pg_logCurrentLineSceneVariables(pData->fname);

		if (!threaded) {
			writesvg((void*)pData);
		}
		else {
#ifdef WIN32
			DWORD rc;
			HANDLE  hThread = CreateThread(
				NULL,                   // default security attributes
				0,                      // use default stack size  
				writesvg,		    // thread function name
				(void*)pData,		    // argument to thread function 
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
				writesvg, (void*)pData);
			if (rc) {
				std::cout << "Error:unable to create thread writesvg" << rc << std::endl;
				exit(-1);
			}
			pthread_exit(NULL);
#endif
		}
	}

	// ******************** Png output ********************
	else if (mode == _Png) {
		threadData* pData = new threadData;
		pData->fname = new char[512];
		pData->w = window_width;
		pData->h = window_height;
		pData->imgThreadData = new cv::Mat(pData->h, pData->w, CV_8UC3);
		indPngSnapshot++;

		sprintf(pData->fname, "%s%s-%s-%04d.png",
			snapshots_dir_path_name.c_str(),
			Png_file_name.c_str(),
			date_stringStream.str().c_str(),
			indPngSnapshot);
		struct stat buffer;
		int count = 0;
		while (stat(pData->fname, &buffer) == 0) {
			sprintf(pData->fname, "%s%s-%s-%04d-%03d.jpg",
				snapshots_dir_path_name.c_str(),
				Png_file_name.c_str(),
				date_stringStream.str().c_str(),
				indPngSnapshot, count);
			count++;
		}
		pg_logCurrentLineSceneVariables(pData->fname);

		glReadBuffer(GL_FRONT);

		// OpenGL's default 4 byte pack alignment would leave extra bytes at the
		//   end of each image row so that each full row contained a number of bytes
		//   divisible by 4.  Ie, an RGB row with 3 pixels and 8-bit componets would
		//   be laid out like "RGBRGBRGBxxx" where the last three "xxx" bytes exist
		//   just to pad the row out to 12 bytes (12 is divisible by 4). To make sure
		//   the rows are packed as tight as possible (no row padding), set the pack
		//   alignment to 1.
		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glReadPixels(0, 0, pData->w, pData->h, GL_RGB, GL_UNSIGNED_BYTE, pData->imgThreadData->data);

		if (!threaded) {
			writepng((void*)pData);
		}
		else {
#ifdef WIN32
			DWORD rc;
			HANDLE  hThread = CreateThread(
				NULL,                   // default security attributes
				0,                      // use default stack size  
				writepng,		    // thread function name
				(void*)pData,		    // argument to thread function 
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
	}

	// ******************** Jpg output ********************
	else if (mode == _Jpg) {
		threadData* pData = new threadData;
		pData->fname = new char[512];
		pData->w = window_width;
		pData->h = window_height;
		pData->imgThreadData = new cv::Mat(pData->h, pData->w, CV_8UC3);
		indJpgSnapshot++;

		sprintf(pData->fname, "%s%s-%s-%04d.jpg",
			snapshots_dir_path_name.c_str(),
			Jpg_file_name.c_str(),
			date_stringStream.str().c_str(),
			indJpgSnapshot);
		struct stat buffer;
		int count = 0;
		while (stat(pData->fname, &buffer) == 0) {
			sprintf(pData->fname, "%s%s-%s-%04d-%03d.jpg",
				snapshots_dir_path_name.c_str(),
				Jpg_file_name.c_str(),
				date_stringStream.str().c_str(),
				indJpgSnapshot, count);
			count++;
		}
		pg_logCurrentLineSceneVariables(pData->fname);
		printf("Snapshot jpg step %d (%s)\n",
			indJpgSnapshot,
			pData->fname);

		glReadBuffer(GL_BACK);

		// OpenGL's default 4 byte pack alignment would leave extra bytes at the
		//   end of each image row so that each full row contained a number of bytes
		//   divisible by 4.  Ie, an RGB row with 3 pixels and 8-bit componets would
		//   be laid out like "RGBRGBRGBxxx" where the last three "xxx" bytes exist
		//   just to pad the row out to 12 bytes (12 is divisible by 4). To make sure
		//   the rows are packed as tight as possible (no row padding), set the pack
		//   alignment to 1.
		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glReadPixels(0, 0, pData->w, pData->h, GL_RGB, GL_UNSIGNED_BYTE, pData->imgThreadData->data);
		printOglError(706);

		if (!threaded) {
			writejpg((void*)pData);
		}
		else {
#ifdef WIN32
			DWORD rc;
			HANDLE  hThread = CreateThread(
				NULL,                   // default security attributes
				0,                      // use default stack size  
				writejpg,		    // thread function name
				(void*)pData,		    // argument to thread function 
				0,                      // use default creation flags 
				&rc);   // returns the thread identifier 
			if (hThread == NULL) {
				std::cout << "Error:unable to create thread writejpg" << std::endl;
				exit(-1);
			}
			CloseHandle(hThread);
#else
			pthread_t drawing_thread;
			int rc;
			rc = pthread_create(&drawing_thread, NULL,
				writejpg, (void*)pData);
			if (rc) {
				std::cout << "Error:unable to create thread writejpg" << rc << std::endl;
				exit(-1);
			}
			pthread_exit(NULL);
#endif
		}
	}

	// ******************** interactive output ********************
	else if (mode == _Render) {

		// sets viewport to single window
		glViewport(0, 0, window_width, window_height);

		glDisable(GL_BLEND);

#if defined(TEXTURED_QUAD_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES)
		//////////////////////////////////////
		// particle animation pass #0
		pg_ParticleAnimationPass();
#endif
		printOglError(681);

		//////////////////////////////////////
		// update pass #1 
		// image layers update pass (CA, PIXELS, DRAWING, VIDEO AND PHOTO...)
		pg_UpdatePass();
		printOglError(682);

		//////////////////////////////////////
		// particle and SVG clip art pass #2a
#if defined(TEXTURED_QUAD_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES)
		pg_SVGandParticleRenderingPass();
		printOglError(683);
#endif

		//////////////////////////////////////
		// mesh pass #2b
		// the meshes are displayed together with the particles except for augmented reality
		// where they are displayed last
#if defined(PG_MESHES) && !defined(PG_AUGMENTED_REALITY)
		if (activeMeshes > 0) {
			pg_MeshPass();
		}
#endif

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
#ifdef PG_SENSORS
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

			// /////////////////////////
			// read sensor values on CA (non echoed) and send messages
			if (pg_FrameNo >= 10 + first_frame_number) {
				readSensors();
				printOglError(687);
			}
		}
		// incremental sensor activation every 45 sec. = 720/16
		for (int indSens = 0; indSens < PG_NB_SENSORS; indSens++) {
			int reindexed_Sensor = Sensor_order[indSens];
			if (!sensor_onOff[reindexed_Sensor]) {
				//printf("CurrentClockTime - sensor_last_activation_time %f\n", CurrentClockTime - sensor_last_activation_time);
				if (sensor_activation == 5
					&& CurrentClockTime - sensor_last_activation_time > 45) {
					sensor_last_activation_time = CurrentClockTime;
					sensor_onOff[reindexed_Sensor] = true;
					break;
				}
			}
		}
#endif

#ifdef CAAUDIO
#ifdef PG_PUREDATA_SOUND
#ifdef PG_NB_CA_TYPES
		// drawing memory on odd and even frames for CA	
		glBindFramebuffer(GL_READ_FRAMEBUFFER, pg_FBO_Mixing_capturedFB_prec[(pg_FrameNo % 2)]); // drawing memory on odd and even frames for echo and sensors	
		playChord();
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
#endif
#endif
#endif

		//////////////////////////////////////
		// last mesh pass with final rendering texture
		// for augmented reality
#if defined(PG_MESHES) && defined(PG_AUGMENTED_REALITY)
		if (activeMeshes > 0) {
			pg_MeshPass();
		}
#endif
	}
	printOglError(686);
}
