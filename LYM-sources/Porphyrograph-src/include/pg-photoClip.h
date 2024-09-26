/*! \file pg-photoClip.h
 *
 *
 *     File pg-photoClip.h
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

#ifndef PG_PHOTOCLIP_H
#define PG_PHOTOCLIP_H

#define _NbMaxCues 8

//////////////////////////////////////////////////////////////////////////////////////////////////////
// CONSTs
//////////////////////////////////////////////////////////////////////////////////////////////////////

enum pg_clip_LR { pg_enum_clipLeft = 0,
	pg_enum_clipRight,
	pg_enum_clipLeftRight,
};

//////////////////////////////////////////////////////////////////////////////////////////////////////
// TRACK PLAYED CLIP MANAGEMENT CLASS
//////////////////////////////////////////////////////////////////////////////////////////////////////


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

//////////////////////////////////////////////////////////////////////////////////////////////////////
// LOADED CLIP DYNNAMIC STATUS MANAGEMENT CLASS
//////////////////////////////////////////////////////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////////////////////////////////////////////////////
// DIAPORAMA PHOTO MANAGEMENT CLASS
//////////////////////////////////////////////////////////////////////////////////////////////////////

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
	// due to global storage variables pg_imgPhotoCompressedFormat[indScenario], pg_imgPhotoCompressedBitmap[indScenario], pg_imgPhotoBGRInit[indScenario], pg_imgPhotoRGB[indScenario];
	bool pg_loadPhoto(bool invert, int width, int height, bool verbose, int indScenario);
	bool pg_toGPUPhoto(bool is_rectangle,
		GLint components,
		GLenum datatype, GLenum texturefilter, int iindConfiguration);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////
// EXPORTED VARIABLES
////////////////////////////////////////////////////////////////////////////////////////////////////// 

// clip status
extern pg_clip_status pg_all_clip_status[pg_enum_clipLeftRight];
// clip track
extern std::vector<pg_clip_track> pg_clip_tracks[PG_MAX_SCENARIOS];

// IMAGE TEXTURES
extern int pg_nbCompressedImageDirs[PG_MAX_SCENARIOS];

// playing clip nos
extern int pg_playing_clipNoLeft;
extern int pg_playing_secondClipNoLeft;
extern int pg_playing_clipNoRight;
extern int pg_playing_secondClipNoRight;

// +++++++++++++++++++++++ Clip ranges for automatic clip selection +++++
extern std::vector<int> pg_clip_ranges_min;
extern std::vector<int> pg_clip_ranges_max;

// displaying diaporama no
extern int pg_CurrentDiaporamaFile;
#define PG_PHOTO_NB_TEXTURES                 (2)
extern float pg_Photo_weight[PG_PHOTO_NB_TEXTURES];
extern PhotoSwapData pg_Photo_swap_buffer_data[PG_PHOTO_NB_TEXTURES];

// photo albums
extern std::string pg_ImageDirectory[PG_MAX_SCENARIOS];

// short video clip albums
extern string pg_ClipDirectory[PG_MAX_SCENARIOS];

// files inside diaporama directories
extern vector <vector<PhotoData>> pg_compressedImageData[PG_MAX_SCENARIOS];

//////////////////////////////////////////////////////////////////////////////////////////////////////
// EXPORTED FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////

// parsing clips and diaporamas
void pg_parseScenario_ClipsAndPhotos(std::ifstream& scenarioFin, int indScenario);

// OSC Commands
void pg_aliasScript_Photo(string address_string, string string_argument_0,
	float float_arguments[PG_MAX_OSC_ARGUMENTS], int nb_arguments, int indVar);
void pg_aliasScript_Clip(string address_string, string string_argument_0,
	float float_arguments[PG_MAX_OSC_ARGUMENTS], int nb_arguments, int indVar);

void pg_play_clip_no(int indClipRank, int clipSide, int clipNo);

// diaporama management
void pg_initDiaporamas(void);
bool pg_loadAllDiaporamas(void);
void pg_launch_diaporama(int slideNo);
bool pg_update_diaporama(void);


#endif