/*! \file pg-movieCamera.h
 *
 *
 *     File pg-movieCamera.h
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

#ifndef PG_MOVIECAMERA_H
#define PG_MOVIECAMERA_H

//////////////////////////////////////////////////////////////////////////////////////////////////////
// CAMERA AND MOVIE STREAMING MANAGEMENT CLASS
//////////////////////////////////////////////////////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////////////////////////////////////////////////////
// LIVE MEDIA MANAGEMENT CLASS
//////////////////////////////////////////////////////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////////////////////////////////////////////////////
// EXPORTED VARIABLES
////////////////////////////////////////////////////////////////////////////////////////////////////// 

// movie soundtrack passes over an onset or a peak before next frame
extern bool pg_movie_sound_onset;
extern bool pg_movie_sound_peak;

// movie / camera capture
extern VideoCapture  pg_movie_capture;
extern VideoCapture pg_webCam_capture;
extern vector <VideoCapture> pg_IPCam_capture;

// movie playing
extern int pg_currentlyPlaying_movieNo;

// MOVIE FRAME
extern Mat pg_movie_frame;
extern vector<VideoTrack> pg_VideoTracks[PG_MAX_SCENARIOS];
extern bool pg_is_movieLoading;

// video texture ID, image and camera
extern GLuint pg_movie_texture_texID;

// MOVIE/CAMERA FRAME SIZE
extern int pg_movie_frame_width;
extern int pg_movie_frame_height;
extern int pg_camera_frame_width;
extern int pg_camera_frame_height;
extern int pg_camera_x_offset;
extern int pg_camera_y_offset;

// video texture ID, image and camera
extern GLuint pg_camera_texture_texID;
extern GLuint pg_movie_texture_texID;
extern GLuint pg_camera_BG_texture_texID;
extern GLuint pg_camera_BGIni_texture_texID;

// DELAYED CAMERA WEIGHT
extern int pg_delayedCameraWeight;

extern int pg_initialSecondBGCapture;
extern bool pg_secondInitialBGCapture;
extern bool pg_initialBGCapture;

//////////////////////////////////////////////////////////////////////////////////////////////////////
// EXPORTED FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////

// scenario parsing
void pg_parseScenario_Videos(std::ifstream& scenarioFin, int indScenario);
void pg_parseScenario_Cameras(std::ifstream& scenarioFin, int indScenario);

/// NON THREADED LOAD CAMERA FRAME
void pg_openCameraCaptureAndLoadFrame(void);
void pg_initWebcamParameters(void);
void pg_loadCameraFrame(bool initial_capture, int IPCam_no);
/// NON THREADED LOAD MOVIE FRAME
void* pg_initVideoMoviePlayback(string* fileName);

// checks whether a peak or an onset are passed or closer than one frame
void pg_updatePeakOrOnset(double timeFromBeginning, vector<float>* peaks, vector<float>* onsets,
	bool* is_new_peak, bool* is_new_onset, int nbPeaks, int nbOnsets, int* curPeakIndex, int* curOnsetIndex);

// OSC messages
void pg_aliasScript_Movie(string address_string, string string_argument_0,
	float float_arguments[PG_MAX_OSC_ARGUMENTS], int nb_arguments, int indVar);
void pg_aliasScript_Camera(string address_string, string string_argument_0,
	float float_arguments[PG_MAX_OSC_ARGUMENTS], int nb_arguments, int indVar);

// update at each rendering frame of movie/camera frame
void pg_update_clip_camera_and_movie_frame(void);

#endif