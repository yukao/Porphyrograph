extern unsigned int imgPhotoCompressedFormat[PG_MAX_CONFIGURATIONS];
extern unsigned char* imgPhotoCompressedBitmap[PG_MAX_CONFIGURATIONS];
extern cv::Mat imgPhotoBGRInit[PG_MAX_CONFIGURATIONS];
extern cv::Mat imgPhotoRGB[PG_MAX_CONFIGURATIONS];

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
	// due to global storage variables imgPhotoCompressedFormat[indConfiguration], imgPhotoCompressedBitmap[indConfiguration], imgPhotoBGRInit[indConfiguration], imgPhotoRGB[indConfiguration];
	bool pg_loadPhoto(bool invert, int width, int height, bool verbose, int indConfiguration);
	bool pg_toGPUPhoto(bool is_rectangle,
		GLint components,
		GLenum datatype, GLenum texturefilter, int iindConfiguration);
};

class ClipFramesData {
public:
	GLuint texBuffID;
	GLenum clipImgFormat;
	ClipFramesData(void) {
		clipImgFormat = 0;
		texBuffID = NULL_ID;
	}
	~ClipFramesData(void) {
		clipImgFormat = 0;
		texBuffID = NULL_ID;
	}
	// calls to pg_loadClipFrames should be immediately followed by pg_toGPUClipFrames
	// due to global storage variables imgPhotoCompressedFormat, imgPhotoCompressedBitmap, imgPhotoBGRInit, imgPhotoRGB;
	bool pg_loadClipFrames(string* fileName, int width, int height, int indConfiguration);
	bool pg_toGPUClipFrames(int w, int h, GLenum texturefilter, int indConfiguration);
};

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
class media_status {
private:
	int nbFramesLeft;
	double initialTime;
	int initialMediaNbFrames;
	int currentMovieSoundPeakIndex;
	int nbMovieSoundPeakIndex;
	int currentMovieSoundOnsetIndex;
	int nbMovieSoundOnsetIndex;

public:
	media_status();
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
	~media_status();
};
extern media_status pg_movie_status;
extern bool is_movieLoading;
extern bool is_movieAtEnd;

extern int initialSecondBGCapture;

#define _NbMaxCues 8
enum pg_clip_LR
{
	_clipLeft = 0,
	_clipRight,
	_clipLR,
};

class clip_track {
private:
	int clip_cue[_NbMaxCues];
	int clipNo;
	int initialNbFrames;
	string clip_name;
public:
	clip_track(int indClip, int nbFrames, string name);
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
extern std::vector<clip_track> pg_clip_tracks[PG_MAX_CONFIGURATIONS];

class clip_status {
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

	clip_status(int clipSide);
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
	~clip_status();
};

// playing clip nos
extern int playing_clipNoLeft;
extern int playing_secondClipNoLeft;
extern int playing_clipNoRight;
extern int playing_secondClipNoRight;
extern float fx_dry_wet;
// clip status
extern clip_status pg_clip_status[_clipLR];
// index of top clip 
extern int rankOfTopClip;

extern bool secondInitialBGCapture;
extern bool initialBGCapture;


/////////////////////////////////////////////////////////////////////
// IMAGE FILES
////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
// image index management for progessive image turnover
// according to image swap frequency and duration
// and according to directory size
extern double pg_CurrentDiaporamaEnd;
extern int pg_CurrentDiaporamaFile;


///////////////////////////////////////////////////////////////////
// IMAGE TEXTURES
////////////////////////////////////////////////////////////////////
// Images used for displaying or for swapping
// PhotoBufferDataStructOld describing a buffer of images stored in the GPU
// these images are used to make piled rendering 
// they are doubled by swap images used to smoothly change between images
extern int pg_nbCompressedImages[PG_MAX_CONFIGURATIONS];
extern int pg_nbCompressedImageDirs[PG_MAX_CONFIGURATIONS];
extern vector<int>pg_nbCompressedImagesPerFolder[PG_MAX_CONFIGURATIONS];
// diaporama directories
extern vector<string> pg_compressedImageDirsNames[PG_MAX_CONFIGURATIONS];
// files inside diaporama directories
extern vector <vector<PhotoData>> pg_compressedImageData[PG_MAX_CONFIGURATIONS];
extern vector<vector<string>> pg_compressedImageFilesNames[PG_MAX_CONFIGURATIONS];

// the index from which an image available for swapping is looked for
extern int pg_IndInitialSwapPhoto;

////////////////////////////////////////////////////////////////////
// SHORT CLIP IMAGE FILES
////////////////////////////////////////////////////////////////////
// clips are made of sequences of images loaded in GPU
extern ClipFramesData** pg_ClipFrames_buffer_data[PG_MAX_CONFIGURATIONS];
extern int pg_nbClips[PG_MAX_CONFIGURATIONS];
extern int* pg_nbCompressedClipFramesPerFolder[PG_MAX_CONFIGURATIONS];
extern int* pg_firstCompressedClipFramesInFolder[PG_MAX_CONFIGURATIONS];
extern int pg_nbCompressedClipFrames[PG_MAX_CONFIGURATIONS];



/////////////////////////////////////////////////////////////////
// CAMERA PARAMETERS
extern float CameraCurrent_exposure;
extern float CameraCurrent_gain;
extern float CameraCurrent_brightness;
extern float CameraCurrent_saturation;
extern float CameraCurrent_contrast;
extern float CameraCurrent_FPS;
extern float CameraCurrent_focus;
extern float CameraCurrent_gamma;
extern float CameraCurrent_WB_B;
extern float CameraCurrent_WB_R;

/////////////////////////////////////////////////////////////////
// VIDEO PARAMETERS
extern int precedingVideoTrack;

// video texture ID, image and camera
extern GLuint pg_camera_texture_texID;
extern GLuint pg_movie_texture_texID;
extern GLuint pg_camera_BG_texture_texID;
extern GLuint pg_camera_BGIni_texture_texID;

// extern IplImage *pg_camera_frame;
// extern IplImage *pg_movie_frame;
extern int pg_camera_frame_width;
extern int pg_camera_frame_height;
extern int pg_camera_x_offset;
extern int pg_camera_y_offset;
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
extern vector<VideoTrack> pg_VideoTracks[PG_MAX_CONFIGURATIONS];
// soundtracks
class SoundTrack {
public:
	string soundtrackFileName;
	string soundtrackShortName;
	string soundtrackPeaksFileName;
	string soundtrackOnsetsFileName;
	float soundtrackOnsetsAndPeasksOffset;
	vector<float> soundtrackPeaks;
	vector<float> soundtrackOnsets;
	SoundTrack(void) {
		soundtrackFileName = "";
		soundtrackShortName = "";
		soundtrackPeaksFileName = "";
		soundtrackOnsetsFileName = "";
		soundtrackOnsetsAndPeasksOffset = 0.f;
	}
	~SoundTrack(void) {
	}
};
extern vector<SoundTrack> pg_SoundTracks[PG_MAX_CONFIGURATIONS];
extern int currentlyPlaying_trackNo;
extern int currentTrackSoundPeakIndex;
extern int nbTrackSoundPeakIndex[PG_MAX_CONFIGURATIONS];
extern int currentTrackSoundOnsetIndex;
extern int nbTrackSoundOnsetIndex[PG_MAX_CONFIGURATIONS];

void clip_new(int indClipRank, int clipSide, int nb_arguments, float* float_arguments);