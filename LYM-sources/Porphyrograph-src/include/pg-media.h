/*! \file pg-media.h
 *
 *
 *     File pg-media.h
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

#ifndef PG_MEDIA_H
#define PG_MEDIA_H

 // movie soundtrack passes over an onset or a peak before next frame
extern bool pg_movie_sound_onset;
extern bool pg_movie_sound_peak;

// movie playing
extern int pg_currentlyPlaying_movieNo;


extern VideoCapture pg_webCam_capture;
extern vector <VideoCapture> pg_IPCam_capture;
extern vector<String> pg_IPCam_capture_address;
class webCam {
public:
	string cameraString;
	int cameraID;
	int cameraWidth;
	int cameraHeight;
	webCam(void) {
		cameraString = "";
		cameraID = -1;
		cameraWidth = 0;
		cameraHeight = 0;
	}
	~webCam(void) {
		cameraString = "";
		cameraID = -1;
		cameraWidth = 0;
		cameraHeight = 0;
	}
};
extern vector<webCam> pg_webCams;
extern int pg_current_active_cameraNo;
extern bool pg_initializedWebcam;
extern bool pg_cameraCaptureIsOn;

extern VideoCapture  pg_movie_capture;
class pg_media_status {
private:
	int nbFramesLeft;
	double initialTime;
	int initialMediaNbFrames;
	int currentMovieSoundPeakIndex;
	int nbMovieSoundPeakIndex;
	int currentMovieSoundOnsetIndex;
	int nbMovieSoundOnsetIndex;

public:
	pg_media_status();
	// number frames left until the end from current frame
	const int get_nbFramesLeft();
	// number of frames read from the beginning of the movie
	const int get_nbFramesRead();
	// current time when movie started
	const double get_initialTime();
	// initial total number of frames in the movie
	const int get_initialNbFrames();
	// checks whether a peak or an onset are passed or closer than one frame
	const void updateMoviePeakOrOnset(void);
	// sets the movie at a position from beginning
	const void set_position(int nb_frames_from_beginning);
	// changes the movie initial time if capture frequency changes
	const void reset_initialTime(float new_captFreq);
	// reads one frame and updates the remaining number of frames in the movie
	const void read_one_frame();
	// (re)starts the movie from begginning with its total number of frames
	const void reset_movie(int nbTotFramesLeft);
	~pg_media_status();
};
extern pg_media_status pg_movie_status;
extern bool pg_is_movieLoading;
extern bool pg_is_movieAtEnd;

#define _NbMaxCues 8
enum pg_clip_LR { pg_enum_clipLeft = 0,
	pg_enum_clipRight,
	pg_enum_clipLeftRight,
};

class pg_clip_track {
private:
	int clip_cue[_NbMaxCues];
	int clipNo;
	int initialNbFrames;
	string clip_name;
public:
	pg_clip_track(int indClip, int nbFrames, string name);
	// catches a cue
	int set_cue_onOff(int indCue, int frameNo);
	// resets a cue
	void set_cue_off(int indCue);
	// sets a cue
	void set_cue(int indCue, int indFrame);
	// returns a cue
	int get_cue(int indCue);
	// initial total number of frames in the clip
	int get_initialNbFrames();
	// clip name
	string get_name();
};

// clip track
extern std::vector<pg_clip_track> pg_clip_tracks[PG_MAX_SCENARIOS];

class pg_clip_status {
private:
	pg_clip_LR sideLR;
	int currentlyPlaying_clipNo[PG_NB_PARALLEL_CLIPS];
	int lastFrame[PG_NB_PARALLEL_CLIPS];
	double lastPlayedFrameTime[PG_NB_PARALLEL_CLIPS];
	double cumulated_scratch_offset;
	double currentFPS;
	float last_scratch_angle;
	float last_nudge_angle;
	bool is_scratch_pressed;
	double last_scratch_angle_change;
	double last_nudge_angle_change;

public:
	bool clip_autoplay[PG_NB_PARALLEL_CLIPS];
	bool clip_play[PG_NB_PARALLEL_CLIPS];
	float clip_level[PG_NB_PARALLEL_CLIPS];
	float clip_r_channel_level[PG_NB_PARALLEL_CLIPS];
	float clip_g_channel_level[PG_NB_PARALLEL_CLIPS];
	float clip_b_channel_level[PG_NB_PARALLEL_CLIPS];
	int jump_frame_freq;

	pg_clip_status(int clipSide);
	// last frame index
	int get_lastFrame(int indClipRank);
	// last frame exact time
	double get_lastPlayedFrameTime(int indClipRank);
	// last frame exact time
	void set_lastPlayedFrameTime(int indClipRank, double a_time);
	// next frame index and sets it as last frame together 
	// with memory of exact time of current frame (including the delay to the current time)
	int get_nextFrameAndUpdate(int indClipRank);
	// standard speed (at which it was recorded)
	double get_standardSpeed();
	// current speed according to nudging
	double get_currentFPS();
	// current playing clip No
	int getCurrentlyPlaying_clipNo(int indClipRank);
	// play
	bool get_clip_play(int indClipRank);
	// autoplay
	bool get_clip_autoplay(int indClipRank);
	// position of the wheel in last interaction
	float get_last_scratch_angle(double* a_time);
	// position of the wheel in last interaction
	float get_last_nudge_angle(double* a_time);
	// scratch wheel is pressed: do not update clip automatically
	bool get_scratch_pressed();
	// standard speed (at which it was recorded)
	void set_standardSpeed(double a_speed);
	// current speed according to nudging
	void set_currentFPS(double a_FPS);
	// current playing clip No
	void setCurrentlyPlaying_clipNo(int indClipRank, int indClip);
	// position of the wheel in last interaction
	void set_last_scratch_angle(float a_angle, double a_time);
	// position of the wheel in last interaction
	void set_last_nudge_angle(float a_angle, double a_time);
	// scratch wheel is pressed: do not update clip automatically
	void set_scratch_pressed(bool a_value);
	// sets the clip at a position from beginning
	void set_position(int indClipRank, int nb_frames_from_beginning);
	// offsets the movie position by a positive or negative time delta (scratch mode)
	// used several times for a small offset
	void scratch_offset_position(double elapsedTime);
	// offsets the movie position by a positive or negative time delta (play mode)
	// used once for a big jump
	void play_offset_position(int indClipRank, double elapsedTime);
	// changes the initial time for fast forward/backward
	void reset_clip(int indClipRank);
	~pg_clip_status();
};

// playing clip nos
extern int pg_playing_clipNoLeft;
extern int pg_playing_secondClipNoLeft;
extern int pg_playing_clipNoRight;
extern int pg_playing_secondClipNoRight;

// clip status
extern pg_clip_status pg_all_clip_status[pg_enum_clipLeftRight];



/////////////////////////////////////////////////////////////////////
// IMAGE FILES
////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
// image index management for progessive image turnover
// according to image swap frequency and duration
// and according to directory size
extern int pg_CurrentDiaporamaFile;


///////////////////////////////////////////////////////////////////
// IMAGE TEXTURES
////////////////////////////////////////////////////////////////////
// Images used for displaying or for swapping
// PhotoBufferDataStructOld describing a buffer of images stored in the GPU
// these images are used to make piled rendering 
// they are doubled by swap images used to smoothly change between images
extern int pg_nbCompressedImages[PG_MAX_SCENARIOS];
extern int pg_nbCompressedImageDirs[PG_MAX_SCENARIOS];
extern vector<int>pg_nbCompressedImagesPerFolder[PG_MAX_SCENARIOS];
// diaporama directories
extern vector<string> pg_compressedImageDirsNames[PG_MAX_SCENARIOS];


class PhotoSwapData {
public:
	int indSwappedPhoto;
	int indOldPhoto;
	bool swapping;
	float swapStart;
	float blendStart;        // start of blending the current photo (<0 if not displayed)
	PhotoSwapData(void) {
		indSwappedPhoto = 0;
		indOldPhoto = 0;
		swapping = false;
		swapStart = 0.0f;
		blendStart = -1.0f; // convention blendStart < 0 == no blending
	}
};
class PhotoData {
public:
	string PhotoName;
	int w;
	int h;
	GLenum photoFormat;
	bool invert;
	bool IDallocated;
	GLuint texBuffID;
	PhotoData(void) {
		w = -1;
		h = -1;
		photoFormat = 0;
		PhotoName = "";
		IDallocated = false;
		invert = false;
		texBuffID = NULL_ID;
	}
	void release(void) {
		w = -1;
		h = -1;
		photoFormat = 0;
		PhotoName = "";
	}
	~PhotoData(void) {
	}
	// calls to pg_loadPhoto should be immediately followed by pg_toGPUPhoto
	// due to global storage variables pg_imgPhotoCompressedFormat[indConfiguration], pg_imgPhotoCompressedBitmap[indConfiguration], pg_imgPhotoBGRInit[indConfiguration], pg_imgPhotoRGB[indConfiguration];
	bool pg_loadPhoto(bool invert, int width, int height, bool verbose, int indConfiguration);
	bool pg_toGPUPhoto(bool is_rectangle,
		GLint components,
		GLenum datatype, GLenum texturefilter, int iindConfiguration);
};

// files inside diaporama directories
extern vector <vector<PhotoData>> pg_compressedImageData[PG_MAX_SCENARIOS];
extern vector<vector<string>> pg_compressedImageFilesNames[PG_MAX_SCENARIOS];

/////////////////////////////////////////////////////////////////
// CAMERA PARAMETERS
extern float pg_CameraCurrent_exposure;
extern float pg_CameraCurrent_gain;
extern float pg_CameraCurrent_brightness;
extern float pg_CameraCurrent_saturation;
extern float pg_CameraCurrent_contrast;
extern float pg_CameraCurrent_FPS;
extern float pg_CameraCurrent_focus;
extern float pg_CameraCurrent_gamma;
extern float pg_CameraCurrent_WB_B;
extern float pg_CameraCurrent_WB_R;

/////////////////////////////////////////////////////////////////
// VIDEO PARAMETERS
// video texture ID, image and camera
extern GLuint pg_camera_texture_texID;
extern GLuint pg_movie_texture_texID;
extern GLuint pg_camera_BG_texture_texID;
extern GLuint pg_camera_BGIni_texture_texID;

////////////////////////////////////////////////////////////////////
// CAMERA FRAME
////////////////////////////////////////////////////////////////////
extern Mat pg_camera_frame;
extern int pg_camera_frame_width;
extern int pg_camera_frame_height;
extern int pg_camera_x_offset;
extern int pg_camera_y_offset;

////////////////////////////////////////////////////////////////////
// MOVIE FRAME
////////////////////////////////////////////////////////////////////
extern int pg_movie_frame_width;
extern int pg_movie_frame_height;
extern Mat pg_movie_frame;

// video tracks
class VideoTrack {
public:
	string videoFileName;
	string videoShortName;
	string videoSoundtrackPeaksFileName;
	string videoSoundtrackOnsetsFileName;
	vector<float> videoSoundtrackPeaks;
	vector<float> videoSoundtrackOnsets;
	VideoTrack(void) {
		videoFileName = "";
		videoShortName = "";
		videoSoundtrackPeaksFileName = "";
		videoSoundtrackOnsetsFileName = "";
	}
	~VideoTrack(void) {
	}
};
extern vector<VideoTrack> pg_VideoTracks[PG_MAX_SCENARIOS];

void pg_clip_new(int indClipRank, int clipSide, int nb_arguments, float* float_arguments);
void pg_updatePeakOrOnset(double timeFromBeginning, vector<float>* peaks, vector<float>* onsets,
	bool* is_new_peak, bool* is_new_onset, int nbPeaks, int nbOnsets, int* curPeakIndex, int* curOnsetIndex);
void pg_movie_forward(int nb_frames_forth);
void pg_movie_backward(int nb_frames_back);

void pg_update_clip_camera_and_movie_frame(void);
void pg_loadCameraFrame(bool initial_capture, int IPCam_no);

void pg_diaporama_random(void);
void pg_initDiaporamas(void);
bool pg_loadAllDiaporamas(void);
void pg_AddDirectoryToDiaporamas(int indConfiguration, fs::directory_entry dir_entry);
void pg_AddFilesToDiaporamas(int indConfiguration, fs::directory_entry dir_entry);
bool pg_reloadAllDiaporamas(int currentConfig, bool with_trace);
bool pg_addNewDiaporamas(int current_config, bool with_trace);
void pg_diaporama_slide(int slideNo);
void pg_launch_diaporama(int slideNo);
bool pg_update_diaporama(void);

// FUNCTIONS FOR TESTING SUBDIR FILES (MADE FOR SAMPLES AND IMAGES)
void pg_nextFileIndexMemoryLoop(bool ascending);

/// NON THREADED LOAD CAMERA FRAME
void pg_initCameraFrameTexture(Mat* video_frame);
void pg_openCameraCaptureAndLoadFrame(void);

void pg_releaseCameraCapture(void);
void pg_initWebcamParameters(void);

void pg_play_movie_no(void);

#if defined(WIN32)
DWORD WINAPI pg_initVideoMoviePlayback(LPVOID lpParam);
#else
void* pg_initVideoMoviePlayback(void* lpParam);
#endif
void* pg_initVideoMoviePlayback_nonThreaded(string* fileName);
void pg_play_clip_no(int indClipRank, int clipSide, int clipNo);

#endif