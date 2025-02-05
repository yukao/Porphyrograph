/*! \file pg-movieCamera.cpp
 *
 *
 *     File pg-movieCamera.cpp
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
 /* based on ENTEC source code */

#include "pg-all_include.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////
// GLOBAL VARS
//////////////////////////////////////////////////////////////////////////////////////////////////////

// movie playing
int pg_currentlyPlaying_movieNo = -1;

// movie soundtrack passes over an onset or a peak before next frame
bool pg_movie_sound_onset = false;
bool pg_movie_sound_peak = false;

VideoCapture  pg_webCam_capture;
vector <VideoCapture> pg_IPCam_capture;
vector<String> pg_IPCam_capture_address;

vector<webCam> pg_webCams;

int pg_current_active_cameraNo = INT_MIN;
bool pg_initializedWebcam = false;
bool pg_cameraCaptureIsOn = false;

VideoCapture  pg_movie_capture;

pg_media_status pg_movie_status = pg_media_status();
bool pg_is_movieLoading = false;
bool pg_is_movieAtEnd = false;

/////////////////////////////////////////////////////////////////
// CAMERA PARAMETERS
float pg_CameraCurrent_exposure = 0.0f;
float pg_CameraCurrent_gain = 0.0f;
float pg_CameraCurrent_brightness = 0.0f;
float pg_CameraCurrent_saturation = 0.0f;
float pg_CameraCurrent_contrast = 0.0f;
float pg_CameraCurrent_FPS = 0.0f;
float pg_CameraCurrent_focus = 0.0f;
float pg_CameraCurrent_gamma = 0.0f;
float pg_CameraCurrent_WB_B = 0.0f;
float pg_CameraCurrent_WB_R = 0.0f;

// +++++++++++++++++++++ DELAYED CAMERA WEIGHT ++++++++++++
int pg_delayedCameraWeight = 0;

// video texture ID, image and camera
GLuint pg_camera_texture_texID = 0;
GLuint pg_movie_texture_texID = 0;
GLuint pg_camera_BG_texture_texID = 0;
GLuint pg_camera_BGIni_texture_texID = 0;

////////////////////////////////////////////////////////////////////
// ClipFrames TEXTURES
////////////////////////////////////////////////////////////////////
// Images used for displaying or for blending
// PhotoBufferDataStructOld describing a buffer of images stored in the GPU
// these images are used to make piled rendering 
// they are doubled by blend images used to smoothly change between images
PhotoData pg_MovieFrame_buffer_data = PhotoData();

////////////////////////////////////////////////////////////////////
// CAMERA FRAME
////////////////////////////////////////////////////////////////////
Mat pg_camera_frame;
int pg_camera_frame_width = -1;
int pg_camera_frame_height = -1;
int pg_camera_x_offset = -1;
int pg_camera_y_offset = -1;

int pg_initialSecondBGCapture = 0;
bool pg_secondInitialBGCapture = false;
bool pg_initialBGCapture = false;

////////////////////////////////////////////////////////////////////
// MOVIE FRAME
////////////////////////////////////////////////////////////////////
int pg_movie_frame_width = 0;
int pg_movie_frame_height = 0;
Mat pg_movie_frame;

// video tracks
vector<VideoTrack> pg_VideoTracks[PG_MAX_SCENARIOS];

////////////////////////////////////////////////////////////////////
// FIRST FRAME IN A MOVIE
Mat pg_FirstMovieFrame; // First frame in a video (opening or looping) 
bool pg_FirstMovieFrameIsAvailable = false; // Available first frame so as to avoid second reading  


//////////////////////////////////////////////////////////////////////////////////////////////////////
// CAMERA FRAME TEXTURE ALLOCATION AND DYNAMIC UPDATE
//////////////////////////////////////////////////////////////////////////////////////////////////////

void pg_initCameraFrameTexture(Mat* camera_frame) {
	if (!pg_FullScenarioActiveVars[pg_ind_scenario][_cameraCaptFreq]) {
		return;
	}

	glEnable(GL_TEXTURE_RECTANGLE);

	// camera image
	if (!pg_camera_texture_texID) {
		glGenTextures(1, &pg_camera_texture_texID);
	}
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_camera_texture_texID);
	glTexImage2D(GL_TEXTURE_RECTANGLE,     // Type of texture
		0,                 // Pyramid level (for mip-mapping) - 0 is the top level
		GL_RGB,            // Internal colour format to convert to
		camera_frame->cols,          // Image width  i.e. 640 for Kinect in standard mode
		camera_frame->rows,          // Image height i.e. 480 for Kinect in standard mode
		0,                 // Border width in pixels (can either be 1 or 0)
		GL_BGR, // Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
		GL_UNSIGNED_BYTE,  // Image data type
		camera_frame->data);        // The actual image data itself

	// current background
	if (!pg_camera_BG_texture_texID) {
		glGenTextures(1, &pg_camera_BG_texture_texID);
	}
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_camera_BG_texture_texID);
	glTexImage2D(GL_TEXTURE_RECTANGLE,     // Type of texture
		0,                 // Pyramid level (for mip-mapping) - 0 is the top level
		GL_RGB,            // Internal colour format to convert to
		camera_frame->cols,          // Image width  i.e. 640 for Kinect in standard mode
		camera_frame->rows,          // Image height i.e. 480 for Kinect in standard mode
		0,                 // Border width in pixels (can either be 1 or 0)
		GL_BGR, // Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
		GL_UNSIGNED_BYTE,  // Image data type
		camera_frame->data);        // The actual image data itself
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// glGenerateMipmap(GL_TEXTURE_2D);

	// initial background
	if (!pg_camera_BGIni_texture_texID) {
		glGenTextures(1, &pg_camera_BGIni_texture_texID);
	}
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_camera_BGIni_texture_texID);
	glTexImage2D(GL_TEXTURE_RECTANGLE,     // Type of texture
		0,                 // Pyramid level (for mip-mapping) - 0 is the top level
		GL_RGB,            // Internal colour format to convert to
		camera_frame->cols,          // Image width  i.e. 640 for Kinect in standard mode
		camera_frame->rows,          // Image height i.e. 480 for Kinect in standard mode
		0,                 // Border width in pixels (can either be 1 or 0)
		GL_BGR, // Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
		GL_UNSIGNED_BYTE,  // Image data type
		camera_frame->data);        // The actual image data itself
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// glGenerateMipmap(GL_TEXTURE_2D);
}

void pg_openCameraCaptureAndLoadFrame(void) {
	if (!pg_FullScenarioActiveVars[pg_ind_scenario][_cameraCaptFreq]) {
		return;
	}

	// video loading openCV
	//Allocates and initializes cvCapture structure
	// for reading a video stream from the camera.
	//Index of camera is -1 since only one camera
	// connected to the computer or it does not
	// matter what camera to use
	printf("camera No %d nbWebcam %d\n", cameraNo, pg_webCams.size());
	if (cameraNo < 0 && -cameraNo - 1 < int(pg_webCams.size())) {
		// cv::CAP_FFMPEG or cv::CAP_IMAGES or cv::CAP_DSHOW.
		printf("Open webcam #%d: %s\n", -cameraNo - 1, pg_webCams[-cameraNo - 1].cameraString.c_str());
		pg_webCam_capture.open(pg_webCams[-cameraNo - 1].cameraID, cv::CAP_DSHOW);
		if (!pg_webCam_capture.isOpened()) {
			sprintf(pg_errorStr, "Error: webcam ID #%d not opened!\n", pg_webCams[-cameraNo - 1].cameraID);
			pg_ReportError(pg_errorStr);
			pg_current_active_cameraNo = INT_MIN;
			pg_cameraCaptureIsOn = false;
		}
		else {
			printf("set webcam #%d frame size %dx%d\n", cameraNo, pg_webCams[-cameraNo - 1].cameraWidth, pg_webCams[-cameraNo - 1].cameraHeight);
			pg_webCam_capture.set(CAP_PROP_FRAME_WIDTH, pg_webCams[-cameraNo - 1].cameraWidth);
			pg_webCam_capture.set(CAP_PROP_FRAME_HEIGHT, pg_webCams[-cameraNo - 1].cameraHeight);

			// initial webcam frame reading
			pg_loadCameraFrame(true, cameraNo);
			pg_current_active_cameraNo = cameraNo;

			if (!pg_initializedWebcam) {
				pg_initWebcamParameters();
				pg_initializedWebcam = true;
			}
			pg_cameraCaptureIsOn = true;
		}
	}
	else if (cameraNo >= 0 && cameraNo < int(pg_IPCam_capture.size())) {
		printf("Open IPcam #%d: %s\n", cameraNo, pg_IPCam_capture_address[cameraNo].c_str());
		pg_IPCam_capture[cameraNo].open(pg_IPCam_capture_address[cameraNo]);
		if (!pg_IPCam_capture[cameraNo].isOpened()) {
			sprintf(pg_errorStr, "Error: IPCam #%d (%s) not opened!\n", cameraNo, pg_IPCam_capture_address[cameraNo].c_str());
			pg_ReportError(pg_errorStr);
			pg_current_active_cameraNo = INT_MIN;
			pg_cameraCaptureIsOn = false;
		}
		else {

			pg_IPCam_capture[cameraNo].set(CAP_PROP_FRAME_WIDTH, 1920);
			pg_IPCam_capture[cameraNo].set(CAP_PROP_FRAME_HEIGHT, 1080);

			// initial IPCam #0 frame reading
			pg_loadCameraFrame(true, cameraNo);
			pg_current_active_cameraNo = cameraNo;
			pg_cameraCaptureIsOn = true;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// CAMERA RELEASE
//////////////////////////////////////////////////////////////////////////////////////////////////////

void pg_releaseCameraCapture(void) {
	if (!pg_FullScenarioActiveVars[pg_ind_scenario][_cameraCaptFreq]) {
		return;
	}

	// video loading openCV
	//Allocates and initializes cvCapture structure
	// for reading a video stream from the camera.
	//Index of camera is -1 since only one camera
	// connected to the computer or it does not
	// matter what camera to use.
	if (pg_cameraCaptureIsOn) {
		if (pg_current_active_cameraNo < 0 && -pg_current_active_cameraNo - 1 < int(pg_webCams.size())) {
			pg_webCam_capture.release();
		}
		else if (pg_current_active_cameraNo >= 0 && pg_current_active_cameraNo < int(pg_IPCam_capture.size())) {
			pg_IPCam_capture[pg_current_active_cameraNo].release();
		}
	}
	pg_current_active_cameraNo = INT_MIN;
	pg_cameraCaptureIsOn = false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// CAMERA PARAMETERS INITIALIZATION
//////////////////////////////////////////////////////////////////////////////////////////////////////

// DISPLAY THE DIFFERENCE BETWEEN THE CURRENT STATE AND THE EXPECTED STATE OF THE CAMERA
void pg_initWebcamParameters(void) {
	if (!pg_FullScenarioActiveVars[pg_ind_scenario][_cameraCaptFreq]) {
		return;
	}

	// to be checked in real time
	// SHARPNESS
	// minimum                 : 0
	// maximum                 : 255
	// default_value           : 128

	// BRIGHTNESS
	// minimum                 : 0
	// maximum                 : 255
	// default_value           : 128

	// CONTRAST
	// minimum                 : 0
	// maximum                 : 255
	// default_value           : 128

	// SATURATION
	// minimum                 : 0
	// maximum                 : 255
	// default_value           : 128

	// GAIN
	// minimum                 : 0
	// maximum                 : 255
	// default_value           : 0

	// WHITE BALANCE
	// minimum                 : 2000
	// maximum                 : 6500
	// default_value           : 4000

	/* cvSetCaptureProperty comment for see3cam */
	//float window_ratio = float(pg_workingWindow_width) / PG_WINDOW_HEIGHT;
	//const float ratio_16_9 = 16.0f / 9.0f;
	//if (window_ratio >= ratio_16_9 || ratio_16_9 - window_ratio < window_ratio - 4.0 / 3.0) {
	//	 //pg_webCam_capture.set(CV_CAP_PROP_FRAME_WIDTH, 1280);
	//	 pg_webCam_capture.set(CV_CAP_PROP_FRAME_WIDTH, 960);
	//	 //pg_webCam_capture.set(CV_CAP_PROP_FRAME_WIDTH, 1920);
	//	 pg_webCam_capture.set(CV_CAP_PROP_FRAME_HEIGHT, 540);
	//	//pg_webCam_capture.set(CV_CAP_PROP_FRAME_HEIGHT, 1080);
	//}
	//else {
	//	pg_webCam_capture.set(CV_CAP_PROP_FRAME_WIDTH, 960);
	//	pg_webCam_capture.set(CV_CAP_PROP_FRAME_HEIGHT, 720);
	//}
	// higher values could be tried for width, fx 1280 x 720 (16:9) 960x720 (4:3)
	//pg_webCam_capture.set(CV_CAP_PROP_FPS, 60);

	pg_CameraCurrent_exposure = (float)pg_webCam_capture.get(CAP_PROP_EXPOSURE);
	pg_CameraCurrent_gain = (float)pg_webCam_capture.get(CAP_PROP_GAIN);
	pg_CameraCurrent_brightness = (float)pg_webCam_capture.get(CAP_PROP_BRIGHTNESS);
	pg_CameraCurrent_saturation = (float)pg_webCam_capture.get(CAP_PROP_SATURATION);
	pg_CameraCurrent_contrast = (float)pg_webCam_capture.get(CAP_PROP_CONTRAST);
	//pg_CameraCurrent_FPS = (float)pg_webCam_capture.get(CAP_PROP_FPS);
	pg_CameraCurrent_focus = (float)pg_webCam_capture.get(CAP_PROP_FOCUS);
	pg_CameraCurrent_gamma = (float)pg_webCam_capture.get(CAP_PROP_GAMMA);
	pg_CameraCurrent_WB_B = (float)pg_webCam_capture.get(CAP_PROP_WHITE_BALANCE_BLUE_U);
	pg_CameraCurrent_WB_R = (float)pg_webCam_capture.get(CAP_PROP_WHITE_BALANCE_RED_V);

	//printf("Current Cam exposure   %.2f\n", pg_CameraCurrent_exposure);
	//printf("Current Cam gain       %.2f\n", pg_CameraCurrent_gain);
	//printf("Current Cam brightness %.2f\n", pg_CameraCurrent_brightness);
	//printf("Current Cam contrast   %.2f\n", pg_CameraCurrent_contrast);
	//printf("Current Cam FPS        %.2f\n", pg_CameraCurrent_FPS);
	//printf("Current Cam focus      %.2f\n", pg_CameraCurrent_focus);
	//printf("Current Cam gamma      %.2f\n", pg_CameraCurrent_gamma);
	//printf("Current Cam WB_B       %.2f\n", pg_CameraCurrent_WB_B);
	//printf("Current Cam WB_R       %.2f\n", pg_CameraCurrent_WB_R);
	//printf("Current Cam saturation %.2f\n", pg_CameraCurrent_saturation);
	//printf("Current Cam WB         %.2f %.2f\n", pg_CameraCurrent_WB_B, pg_CameraCurrent_WB_R);

	if (float(pg_InitialValuesInterpVar[pg_ind_scenario][_cameraExposure].val_num) != pg_CameraCurrent_exposure) {
		printf("cv VideoCapture set exposure new/current  %.2f / %.2f\n", float(pg_InitialValuesInterpVar[pg_ind_scenario][_cameraExposure].val_num), pg_CameraCurrent_exposure);
		//pg_webCam_capture.set(CAP_PROP_EXPOSURE, (*((float *)pg_InitialValuesInterpVar[pg_ind_scenario][_cameraExposure].val_num)));
		pg_CameraCurrent_exposure = float(pg_InitialValuesInterpVar[pg_ind_scenario][_cameraExposure].val_num);
		pg_LastCameraParameterChange_Frame = pg_FrameNo;
	}
	if (float(pg_InitialValuesInterpVar[pg_ind_scenario][_cameraGain].val_num) != pg_CameraCurrent_gain) {
		printf("cv VideoCapture set gain new/current  %.2f / %.2f\n", float(pg_InitialValuesInterpVar[pg_ind_scenario][_cameraGain].val_num), pg_CameraCurrent_gain);
		pg_webCam_capture.set(CAP_PROP_GAIN, float(pg_InitialValuesInterpVar[pg_ind_scenario][_cameraGain].val_num));
		pg_CameraCurrent_gain = float(pg_InitialValuesInterpVar[pg_ind_scenario][_cameraGain].val_num);
		pg_LastCameraParameterChange_Frame = pg_FrameNo;
	}
	if (float(pg_InitialValuesInterpVar[pg_ind_scenario][_cameraBrightness].val_num) != pg_CameraCurrent_brightness) {
		printf("cv VideoCapture set brightness new/current  %.2f / %.2f\n", float(pg_InitialValuesInterpVar[pg_ind_scenario][_cameraBrightness].val_num), pg_CameraCurrent_brightness);
		pg_webCam_capture.set(CAP_PROP_BRIGHTNESS, float(pg_InitialValuesInterpVar[pg_ind_scenario][_cameraBrightness].val_num));
		pg_CameraCurrent_brightness = float(pg_InitialValuesInterpVar[pg_ind_scenario][_cameraBrightness].val_num);
		pg_LastCameraParameterChange_Frame = pg_FrameNo;
	}
	if (float(pg_InitialValuesInterpVar[pg_ind_scenario][_cameraSaturation].val_num) != pg_CameraCurrent_saturation) {
		printf("cv VideoCapture set saturation new/current  %.2f / %.2f\n", float(pg_InitialValuesInterpVar[pg_ind_scenario][_cameraSaturation].val_num), pg_CameraCurrent_saturation);
		pg_webCam_capture.set(CAP_PROP_SATURATION, float(pg_InitialValuesInterpVar[pg_ind_scenario][_cameraSaturation].val_num));
		pg_CameraCurrent_saturation = float(pg_InitialValuesInterpVar[pg_ind_scenario][_cameraSaturation].val_num);
		pg_LastCameraParameterChange_Frame = pg_FrameNo;
	}
	if (float(pg_InitialValuesInterpVar[pg_ind_scenario][_cameraContrast].val_num) != pg_CameraCurrent_contrast) {
		printf("cv VideoCapture set contrast new/current  %.2f / %.2f\n", float(pg_InitialValuesInterpVar[pg_ind_scenario][_cameraContrast].val_num), pg_CameraCurrent_contrast);
		pg_webCam_capture.set(CAP_PROP_CONTRAST, float(pg_InitialValuesInterpVar[pg_ind_scenario][_cameraContrast].val_num));
		pg_CameraCurrent_contrast = float(pg_InitialValuesInterpVar[pg_ind_scenario][_cameraContrast].val_num);
		pg_LastCameraParameterChange_Frame = pg_FrameNo;
	}
	if (float(pg_InitialValuesInterpVar[pg_ind_scenario][_cameraGamma].val_num) != pg_CameraCurrent_gamma) {
		printf("cv VideoCapture set gamma new/current  %.2f / %.2f\n", float(pg_InitialValuesInterpVar[pg_ind_scenario][_cameraGamma].val_num), pg_CameraCurrent_gamma);
		pg_webCam_capture.set(CAP_PROP_GAMMA, float(pg_InitialValuesInterpVar[pg_ind_scenario][_cameraGamma].val_num));
		pg_CameraCurrent_contrast = float(pg_InitialValuesInterpVar[pg_ind_scenario][_cameraGamma].val_num);
		pg_LastCameraParameterChange_Frame = pg_FrameNo;
	}
	if (float(pg_InitialValuesInterpVar[pg_ind_scenario][_cameraWB_R].val_num) != pg_CameraCurrent_WB_R
		&& float(pg_InitialValuesInterpVar[pg_ind_scenario][_cameraWB_R].val_num) >= 0) {
		printf("cv VideoCapture set wbR new/current  %.2f / %.2f\n", float(pg_InitialValuesInterpVar[pg_ind_scenario][_cameraWB_R].val_num), pg_CameraCurrent_WB_R);
		pg_webCam_capture.set(CAP_PROP_WHITE_BALANCE_RED_V, float(pg_InitialValuesInterpVar[pg_ind_scenario][_cameraWB_R].val_num));
		pg_CameraCurrent_WB_R = float(pg_InitialValuesInterpVar[pg_ind_scenario][_cameraWB_R].val_num);
		pg_LastCameraParameterChange_Frame = pg_FrameNo;
	}
	if (float(pg_InitialValuesInterpVar[pg_ind_scenario][_cameraWB_B].val_num) != pg_CameraCurrent_WB_B) {
		printf("cv VideoCapture set wbB new/current  %.2f / %.2f\n", float(pg_InitialValuesInterpVar[pg_ind_scenario][_cameraWB_B].val_num), pg_CameraCurrent_WB_B);
		pg_webCam_capture.set(CAP_PROP_WHITE_BALANCE_BLUE_U, float(pg_InitialValuesInterpVar[pg_ind_scenario][_cameraWB_B].val_num));
		pg_CameraCurrent_WB_B = float(pg_InitialValuesInterpVar[pg_ind_scenario][_cameraWB_B].val_num);
		pg_LastCameraParameterChange_Frame = pg_FrameNo;
	}

	//printf("Initial Cam exposure   %.2f\n", pg_CameraCurrent_exposure);
	//printf("Initial Cam gain       %.2f\n", pg_CameraCurrent_gain);
	//printf("Initial Cam brightness %.2f\n", pg_CameraCurrent_brightness);
	//printf("Initial Cam saturation %.2f\n", pg_CameraCurrent_saturation);
	//printf("Initial Cam contrast   %.2f\n", pg_CameraCurrent_contrast);
	//printf("Initial Cam FPS        %.2f\n", pg_CameraCurrent_FPS);
	//printf("Initial Cam focus      %.2f\n", pg_CameraCurrent_focus);
	//printf("Initial Cam gamma      %.2f\n", pg_CameraCurrent_gamma);
	//printf("Initial Cam WB         %.2f %.2f\n", pg_CameraCurrent_WB_B, pg_CameraCurrent_WB_R);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// MOVIE STREAM STATUS MANAGEMENT
//////////////////////////////////////////////////////////////////////////////////////////////////////

// MOVIE STATUS MANAGEMENT WHILE STREAMING
pg_media_status::pg_media_status() {
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_movieCaptFreq]) {
		nbFramesLeft = 0;
		initialTime = 0.;
		initialMediaNbFrames = 0;

		pg_movie_sound_onset = false;
		pg_movie_sound_peak = false;
		currentMovieSoundPeakIndex = 0;
		if (pg_currentlyPlaying_movieNo >= 0) {
			nbMovieSoundPeakIndex = pg_VideoTracks[pg_ind_scenario][pg_currentlyPlaying_movieNo].videoSoundtrackPeaks.size();
		}
		else {
			nbMovieSoundPeakIndex = 0;
		}
		currentMovieSoundOnsetIndex = 0;
		if (pg_currentlyPlaying_movieNo >= 0) {
			nbMovieSoundOnsetIndex = pg_VideoTracks[pg_ind_scenario][pg_currentlyPlaying_movieNo].videoSoundtrackOnsets.size();
		}
		else {
			nbMovieSoundOnsetIndex = 0;
		}
	}
}
// number frames left until the end from current frame
const int pg_media_status::get_nbFramesLeft() {
	return nbFramesLeft;
}
// number of frames read from the beginning of the movie
const int pg_media_status::get_nbFramesRead() {
	return initialMediaNbFrames - nbFramesLeft;
}
// current time when movie started
const double pg_media_status::get_initialTime() {
	return initialTime;
}
// initial total number of frames in the movie
const int pg_media_status::get_initialNbFrames() {
	return initialMediaNbFrames;
}
// sets the movie at a position from beginning
const void pg_media_status::set_position(int nb_frames_from_beginning) {
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_movieCaptFreq]) {
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
const void pg_media_status::reset_initialTime(float new_captFreq) {
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_movieCaptFreq]) {
		if (movieCaptFreq > 0) {
			int nb_frames_from_beginning = initialMediaNbFrames - nbFramesLeft;
			double initialTimeBeforeChange = initialTime;
			initialTime = pg_CurrentClockTime - (nb_frames_from_beginning / new_captFreq);
			//printf("Initial time before %.2f after %.2f\n", initialTimeBeforeChange, initialTime);
		}
	}
}

const void pg_media_status::updateMoviePeakOrOnset() {
	double timeFromBeginning = pg_CurrentClockTime - initialTime;
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_movieCaptFreq]) {
		if (int(pg_VideoTracks[pg_ind_scenario].size()) > pg_currentlyPlaying_movieNo
			&& pg_VideoTracks[pg_ind_scenario][pg_currentlyPlaying_movieNo].videoSoundtrackPeaksFileName != "") {
			pg_updatePeakOrOnset(timeFromBeginning, &pg_VideoTracks[pg_ind_scenario][pg_currentlyPlaying_movieNo].videoSoundtrackPeaks, &pg_VideoTracks[pg_ind_scenario][pg_currentlyPlaying_movieNo].videoSoundtrackOnsets,
				&pg_movie_sound_peak, &pg_movie_sound_onset, nbMovieSoundPeakIndex, nbMovieSoundOnsetIndex, &currentMovieSoundPeakIndex, &currentMovieSoundOnsetIndex);
		}
	}
}
// reads one frame and updates the remaining number of frames in the movie
const void pg_media_status::read_one_frame() {
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_movieCaptFreq]) {
		nbFramesLeft--;
		updateMoviePeakOrOnset();
	}
}
// (re)starts the movie from begginning with its total number of frames
const void pg_media_status::reset_movie(int nbTotFramesLeft) {
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_movieCaptFreq]) {
		// printf("movie restarted\n");
		nbFramesLeft = nbTotFramesLeft;
		initialMediaNbFrames = nbFramesLeft;
		initialTime = pg_CurrentClockTime;

		currentMovieSoundPeakIndex = 0;
		if (pg_currentlyPlaying_movieNo >= 0) {
			nbMovieSoundPeakIndex = pg_VideoTracks[pg_ind_scenario][pg_currentlyPlaying_movieNo].videoSoundtrackPeaks.size();
		}
		else {
			nbMovieSoundPeakIndex = 0;
		}
		currentMovieSoundOnsetIndex = 0;
		if (pg_currentlyPlaying_movieNo >= 0) {
			nbMovieSoundOnsetIndex = pg_VideoTracks[pg_ind_scenario][pg_currentlyPlaying_movieNo].videoSoundtrackOnsets.size();
		}
		else {
			nbMovieSoundOnsetIndex = 0;
		}
	}
}
pg_media_status::~pg_media_status() {

}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// PEAKS OR ONSETS ASSOCIATED WITH A MOVIE FOR FXs
//////////////////////////////////////////////////////////////////////////////////////////////////////

// checks whether a peak or an onset are passed or closer than one frame
void pg_updatePeakOrOnset(double timeFromBeginning, vector<float>* peaks, vector<float>* onsets,
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

//////////////////////////////////////////////////////////////////////////////////////////////////////
// MOVIE STREAMING
//////////////////////////////////////////////////////////////////////////////////////////////////////

void pg_play_movie_no(void) {
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_movieCaptFreq]) {
		if (playing_movieNo >= 0 && playing_movieNo < int(pg_VideoTracks[pg_ind_scenario].size())
			&& playing_movieNo != pg_currentlyPlaying_movieNo) {
			pg_movie_frame.setTo(Scalar(0, 0, 0));

			// printf("VideoPb Video initialization inside command movie+ (thread) \n");

			pg_BrokenInterpolationVar[_playing_movieNo] = true;
			pg_currentlyPlaying_movieNo = playing_movieNo;
			pg_movie_frame.setTo(Scalar(0, 0, 0));
			sprintf(pg_AuxString, "/movie_shortName %s", pg_VideoTracks[pg_ind_scenario][playing_movieNo].videoShortName.c_str());
			pg_send_message_udp((char*)"s", pg_AuxString, (char*)"udp_TouchOSC_send");

			// texture ID initialization (should not be inside a thread)
			if (pg_movie_texture_texID == NULL_ID) {
				glGenTextures(1, &pg_movie_texture_texID);
			}

			pg_is_movieLoading = true;
			printf("Loading %s\n", pg_VideoTracks[pg_ind_scenario][pg_currentlyPlaying_movieNo].videoFileName.c_str());
			// sprintf(pg_AuxString, "/message %s", movieFileName[pg_currentlyPlaying_movieNo].c_str()); pg_send_message_udp((char *)"s", (char *)pg_AuxString, (char *)"udp_TouchOSC_send");
			pg_initVideoMoviePlayback(&pg_VideoTracks[pg_ind_scenario][pg_currentlyPlaying_movieNo].videoFileName);
		}
	}
}

///////////////////////////////////////
// MOVIE STREAMING JUMPS
void pg_movie_backward(int nb_frames_back) {
	// calculates hypothetical new position
	int posInMovie = int(pg_movie_capture.get(CAP_PROP_POS_FRAMES));
	int new_pos_in_movie = posInMovie - nb_frames_back;
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_movieCaptFreq]) {
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
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_movieCaptFreq]) {
		// advances in the movie
		if (new_pos_in_movie < pg_movie_status.get_initialNbFrames()) {
			printf("forward %d frames %d frames left %.1f initial time\n", int(movieCaptFreq * 10), pg_movie_status.get_nbFramesLeft(), pg_movie_status.get_initialTime());
			pg_movie_capture.set(CAP_PROP_POS_FRAMES, new_pos_in_movie);
			pg_movie_status.set_position(new_pos_in_movie);
		}
		// else stays where it 
	}
}

void* pg_initVideoMoviePlayback(string * fileName) {
	// printf("VideoPb Init movie playback and capture first frame non threaded\n");

	//printf("VideoPb Init movie playback and capture first frame %s\n",
	//	pg_VideoTracks[pg_ind_scenario][pg_currentlyPlaying_movieNo].videoFileName.c_str());

	// film loading openCV
	sprintf(pg_AuxString, "/movie_fileName %s",
		pg_VideoTracks[pg_ind_scenario][pg_currentlyPlaying_movieNo].videoFileName.c_str());
	pg_movie_capture.open(pg_VideoTracks[pg_ind_scenario][pg_currentlyPlaying_movieNo].videoFileName.c_str());

	if (!pg_movie_capture.isOpened()) {
		printf("Movie file [%s] is not loaded!\n", (pg_VideoTracks[pg_ind_scenario][pg_currentlyPlaying_movieNo].videoFileName).c_str());
		return NULL;
	}

	//printf("Opens %s\n", (pg_cwd + pg_videos_directory + movieFileName[pg_currentlyPlaying_movieNo]).c_str());
	if (!pg_movie_capture.isOpened()) {
		printf("Movie file [%s] not loaded!\n", (pg_cwd + pg_videos_directory + pg_VideoTracks[pg_ind_scenario][pg_currentlyPlaying_movieNo].videoFileName).c_str());
		return NULL;
	}
	pg_MovieFrame_buffer_data.PhotoName = pg_cwd + pg_videos_directory + pg_VideoTracks[pg_ind_scenario][pg_currentlyPlaying_movieNo].videoFileName;

	// printf("Name %s Opened\n", pg_MovieFrame_buffer_data.PhotoName);

	//Grabs and returns a frame from movie
	pg_movie_capture >> pg_FirstMovieFrame;
	// texture loading
	if (!pg_FirstMovieFrame.data) {
		printf("Movie frame %s not grabbed!\n", (pg_cwd + pg_videos_directory + pg_VideoTracks[pg_ind_scenario][pg_currentlyPlaying_movieNo].videoFileName).c_str());
		return NULL;
	}
	pg_movie_frame_width = pg_FirstMovieFrame.cols;
	pg_movie_frame_height = pg_FirstMovieFrame.rows;
	pg_initMovieFrameTexture(&pg_FirstMovieFrame);
	pg_FirstMovieFrameIsAvailable = true;

	//printf("Movie frame initialisation %dx%d\n", pg_movie_frame_width, pg_movie_frame_height);
	pg_movie_status.reset_movie(int(pg_movie_capture.get(CAP_PROP_FRAME_COUNT)));
	pg_is_movieLoading = false;

	//printf("Movie nb frames %d\n", pg_movie_status.get_nbFramesLeft());
	return NULL;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
// MOVIE AND CAMERA FRAME CAPTURE UPDATE
//////////////////////////////////////////////////////////////////////////////////////////////////////

void pg_update_clip_camera_and_movie_frame(void) {
	//////////////////////////////////////////////////////////////////////
	// CLIP, CAMERA AND MOVIE FRAME UPDATES BEFORE RENDERING
	// /////////////////////////
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_clipCaptFreq]) {
		// reads next clip frame
		for (int indClipRank = 0; indClipRank < PG_NB_PARALLEL_CLIPS; indClipRank++) {
			for (int clipSide = 0; clipSide < pg_enum_clipLeftRight; clipSide++) {
				//printf("currentPhotoTrack %d clipCaptFreq %.2f clip No %d nb clips %d side %d rank %d\n", currentPhotoTrack, clipCaptFreq, pg_all_clip_status[clipSide].getCurrentlyPlaying_clipNo(indClipRank), pg_nbClips, clipSide, indClipRank);
				if (currentPhotoTrack >= 0 && clipCaptFreq > 0
					&& pg_all_clip_status[clipSide].clip_play[indClipRank]
					&& pg_all_clip_status[clipSide].getCurrentlyPlaying_clipNo(indClipRank) >= 0
					&& pg_all_clip_status[clipSide].getCurrentlyPlaying_clipNo(indClipRank) < pg_nbClips[pg_ind_scenario]) {

					// either the first frame has not been captured and the clip should loop at the beginning
					// or whe are coming close to the end of the clip
					//printf("clip update side %d rank %d\n", clipSide, indClipRank);
					pg_all_clip_status[clipSide].get_nextFrameAndUpdate(indClipRank);
				}
			}
		}
	}

	if (pg_FullScenarioActiveVars[pg_ind_scenario][_movieCaptFreq]) {
		// /////////////////////////
		// reads next movie frame
		if (currentVideoTrack >= 0 && movieCaptFreq > 0
			&& pg_currentlyPlaying_movieNo >= 0
			&& !pg_is_movieLoading
			) {

			// non threaded video update according to fps and time since the beginning of the movie play
			bool new_movie_frame = false;

			// tries to play in sync the whole movie, so counts the time from the 
			// first frame and compares the actual frame no (actualMovieFrameNo)
			// with the frame no expected to play with the correct frame rate (expectedMovieFrameNo)
			int actualMovieFrameNo = (pg_movie_status.get_initialNbFrames() - pg_movie_status.get_nbFramesLeft());
			int expectedMovieFrameNo = int((pg_CurrentClockTime - pg_movie_status.get_initialTime()) * movieCaptFreq);

			pg_is_movieAtEnd = false;

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
				pg_is_movieAtEnd = true;
				//// Short end video loop
				//pg_movie_backward(int(movieCaptFreq * 10));
				//printf("Short end video loop at %d, posInMovie %d\n",
				//	pg_FrameNo, pg_movie_status.get_nbFramesLeft());
			}

			// skips frames if rendering is slower than movie fps
			if ((!pg_is_movieAtEnd) && (pg_FirstMovieFrameIsAvailable && expectedMovieFrameNo - actualMovieFrameNo > 2)) {
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
			if (!pg_is_movieAtEnd && (!pg_FirstMovieFrameIsAvailable || expectedMovieFrameNo - actualMovieFrameNo >= 1)) {
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
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_cameraCaptFreq]) {
		if ((currentVideoTrack >= 0
			&& pg_cameraCaptureIsOn
			&& (// grabs according to camera capture frequence
				(cameraCaptFreq > 0 && pg_FrameNo % int(60.0 / cameraCaptFreq) == 0)
				// grabs at the beginning, otherwise the program is stuck if no camera frame reading takes place
				|| (pg_FrameNo >= camera_rest_delay && pg_FrameNo <= camera_rest_delay + 1000 && pg_FrameNo % 3 == 0))
			// a delay has to be let for camera to digest new parameter values
			&& (pg_FrameNo - pg_LastCameraParameterChange_Frame) > camera_rest_delay
			// the camera weight is positive
			&& cameraWeight > 0)
			|| (reset_camera || pg_initialSecondBGCapture == 1)) {
			// currentVideoTrack <=> video off
			// frame capture
			// printf("frame capture\n");

			// non threaded
			// printf("*** non threaded camera frame capture %d\n", pg_FrameNo);
			if ((pg_current_active_cameraNo < 0 && -pg_current_active_cameraNo - 1 < int(pg_webCams.size()))
				|| (pg_current_active_cameraNo >= 0 && pg_current_active_cameraNo < int(pg_IPCam_capture.size()))) {
				pg_loadCameraFrame(false, cameraNo);
			}
		}
	}
}

//////////////////////////////////////////////
/// NON THREADED LOAD CAMERA FRAME
void pg_loadCameraFrame(bool initial_capture, int IPCam_no) {
	if (!pg_FullScenarioActiveVars[pg_ind_scenario][_cameraCaptFreq]) {
		return;
	}

	//Grabs and returns a frame from camera
	if (IPCam_no < 0) {
		if (pg_webCam_capture.grab()) {
			pg_webCam_capture.retrieve(pg_camera_frame);
		}
	}
	else if (IPCam_no < int(pg_IPCam_capture.size())) {
		pg_IPCam_capture[IPCam_no] >> pg_camera_frame;
	}

	// if it is an initial capture defines image size and allocates frame texture
	if (initial_capture) {
		//Grabs and returns a frame from camera
		if (!pg_camera_frame.data) {
			if (IPCam_no < 0) {
				sprintf(pg_errorStr, "WebCam initial frame not grabbed! \n"); pg_ReportError(pg_errorStr); throw 425;
			}
			else if (IPCam_no < int(pg_IPCam_capture.size())) {
				sprintf(pg_errorStr, "IPCam #%d initial frame not grabbed! \n", IPCam_no); pg_ReportError(pg_errorStr); throw 425;
			}
		}
		if (!pg_camera_texture_texID) {
			pg_initCameraFrameTexture(&pg_camera_frame);
		}

		int camera_x_offset = -1;
		int camera_frame_width = -1;
		int camera_y_offset = -1;
		int camera_frame_height = -1;
		if (pg_camera_frame.cols == 1920 && pg_camera_frame.rows == 1080) {
			// camera frame dimensions and offset
			//printf("HD camera\n");
			camera_x_offset = 0;
			camera_frame_width = pg_camera_frame.cols;
			camera_y_offset = 0;
			camera_frame_height = pg_camera_frame.rows;
		}
		else {
			// tries to preserve ratio
			float window_ratio = float(pg_workingWindow_width) / PG_WINDOW_HEIGHT;
			//printf("screen ratio %d x %d %.2f\n", pg_workingWindow_width, PG_WINDOW_HEIGHT, window_ratio);
			if (pg_camera_frame.rows * window_ratio < pg_camera_frame.cols) {
				//printf("camera size height leading\n");
				camera_frame_width = int(pg_camera_frame.rows * window_ratio);
				camera_frame_height = pg_camera_frame.rows;
				camera_x_offset = (pg_camera_frame.rows - camera_frame_width) / 2;
				camera_y_offset = 0;
			}
			else {
				//printf("camera size width leading\n");
				camera_frame_width = pg_camera_frame.cols;
				camera_frame_height = pg_camera_frame.rows;
				if (pg_camera_frame.cols / window_ratio < pg_camera_frame.rows) {
					 //printf("horizontal offset\n");
					camera_x_offset = (int(pg_camera_frame.rows * window_ratio) - pg_camera_frame.cols) / 2;
					camera_y_offset = 0;
				}
				else {
					 //printf("camera height respecting\n");
					camera_frame_height = pg_camera_frame.rows;
					camera_x_offset = 0;
					camera_y_offset = 0;
				}
			}
			//float ratioSubImage = 1.0f;
			//camera_x_offset = camera_x_offset + int(camera_frame_width * (1.f - ratioSubImage) * 0.5);
			//camera_frame_width = int(camera_frame_width * ratioSubImage);
			//camera_y_offset = camera_y_offset + int(camera_frame_height * (1.f - ratioSubImage) * 0.5);
			//camera_frame_height = int(camera_frame_height * ratioSubImage);
		}

		// first camera frame allocation
		if (pg_camera_x_offset == -1 && pg_camera_frame_width == -1 && pg_camera_y_offset == -1 && pg_camera_frame_height == -1) {
			pg_camera_x_offset = camera_x_offset;
			pg_camera_frame_width = camera_frame_width;
			pg_camera_y_offset = camera_y_offset;
			pg_camera_frame_height = camera_frame_height;
		}
		else {
			if (pg_camera_x_offset != camera_x_offset || pg_camera_y_offset != camera_y_offset) {
				sprintf(pg_errorStr, "Camera frames differ in offset xy %dx%d / %dx%d\n",
					pg_camera_x_offset, pg_camera_y_offset, camera_x_offset, camera_y_offset); pg_ReportError(pg_errorStr);
			}
			if (pg_camera_frame_width != camera_frame_width || pg_camera_frame_height != camera_frame_height) {
				sprintf(pg_errorStr, "Camera frames differ in size: wxh %dx%d / %dx%d\n",
					pg_camera_frame_width, pg_camera_frame_height, camera_frame_width, camera_frame_height); pg_ReportError(pg_errorStr); 
			}
		}

		if (IPCam_no < 0) {
			printf("WebCam frame %dx%d (before ratio %dx%d) offset (%dx%d) ch %d size %d\n",
				pg_camera_frame_width, pg_camera_frame_height,
				pg_camera_frame.cols, pg_camera_frame.rows,
				pg_camera_x_offset, pg_camera_y_offset,
				pg_camera_frame.channels(), int(pg_camera_frame.total() * pg_camera_frame.elemSize()));
		}
		else if (IPCam_no < int(pg_IPCam_capture.size())) {
			printf("IPCam #%d frame %dx%d (before ratio %dx%d) offset (%dx%d) ch %d size %d\n", IPCam_no,
				pg_camera_frame_width, pg_camera_frame_height,
				pg_camera_frame.cols, pg_camera_frame.rows,
				pg_camera_x_offset, pg_camera_y_offset,
				pg_camera_frame.channels(), int(pg_camera_frame.total() * pg_camera_frame.elemSize()));
		}
	}
	// end of initial capture

	if (pg_camera_frame.data) {
		// printf("sends camera frame to GPU\n");
		glEnable(GL_TEXTURE_RECTANGLE);
		glBindTexture(GL_TEXTURE_RECTANGLE, pg_camera_texture_texID);
		glTexImage2D(GL_TEXTURE_RECTANGLE,     // Type of texture
			0,                 // Pyramid level (for mip-mapping) - 0 is the top level
			GL_RGB,            // Internal colour format to convert to
			pg_camera_frame.cols,          // Image width  i.e. 640 for Kinect in standard mode
			pg_camera_frame.rows,          // Image height i.e. 480 for Kinect in standard mode
			0,                 // Border width in pixels (can either be 1 or 0)
			GL_BGR,			   // Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
			GL_UNSIGNED_BYTE,  // Image data type
			pg_camera_frame.data);        // The actual image data itself
		// /////////////////////////
		// reads BG camera frame for image subtraction
		if (reset_camera || pg_initialSecondBGCapture == 1) { // currentVideoTrack <=> video off
			// printf("sends camera BG to GPU\n");
			reset_camera = false;

			// makes a second video capture 
			if (pg_initialSecondBGCapture == 0) {
				pg_initialSecondBGCapture = 1;
			}
			else if (pg_initialSecondBGCapture == 1) {
				pg_initialSecondBGCapture = 2;
			}

			// current background memory
			glEnable(GL_TEXTURE_RECTANGLE);
			glBindTexture(GL_TEXTURE_RECTANGLE, pg_camera_BG_texture_texID);
			glTexImage2D(GL_TEXTURE_RECTANGLE,     // Type of texture
				0,                 // Pyramid level (for mip-mapping) - 0 is the top level
				GL_RGB,            // Internal colour format to convert to
				pg_camera_frame.cols,          // Image width  i.e. 640 for Kinect in standard mode
				pg_camera_frame.rows,          // Image height i.e. 480 for Kinect in standard mode
				0,                 // Border width in pixels (can either be 1 or 0)
				GL_BGR, // Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
				GL_UNSIGNED_BYTE,  // Image data type
				pg_camera_frame.data);        // The actual image data itself
		}

		if (pg_initialBGCapture || pg_secondInitialBGCapture) { // currentVideoTrack <=> video off
			printf("*** non threaded initial BG video capture frame %d\n", pg_FrameNo);
			pg_initialBGCapture = false;

			// makes a second video capture 
			pg_secondInitialBGCapture = !pg_secondInitialBGCapture;

			// current background memory
			// printf("Threaded Transfer of camera BG to GPU (%s)\n", texData->photo_buffer_data->fname);
			glEnable(GL_TEXTURE_RECTANGLE);
			glBindTexture(GL_TEXTURE_RECTANGLE, pg_camera_BGIni_texture_texID);
			glTexImage2D(GL_TEXTURE_RECTANGLE,     // Type of texture
				0,                 // Pyramid level (for mip-mapping) - 0 is the top level
				GL_RGB,            // Internal colour format to convert to
				pg_camera_frame.cols,          // Image width  i.e. 640 for Kinect in standard mode
				pg_camera_frame.rows,          // Image height i.e. 480 for Kinect in standard mode
				0,                 // Border width in pixels (can either be 1 or 0)
				GL_BGR, // Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
				GL_UNSIGNED_BYTE,  // Image data type
				pg_camera_frame.data);        // The actual image data itself
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// CAMERA SCENARIO
//////////////////////////////////////////////////////////////////////////////////////////////////////

void pg_parseScenario_Cameras(std::ifstream& scenarioFin, int indScenario) {
	std::stringstream  sstream;
	string line;
	string ID;
	string temp;
	string temp2;
	string temp3;

	std::getline(scenarioFin, line);
	pg_stringstreamStoreLine(&sstream, &line);
	sstream >> ID;
	if (ID.compare("webCam") != 0) {
		sprintf(pg_errorStr, "Error: incorrect configuration file expected string \"webCam\" not found! (instead \"%s\")", ID.c_str()); pg_ReportError(pg_errorStr); throw 100;
	}

	// VERBATIM
	std::getline(scenarioFin, line);
	// TYPE
	std::getline(scenarioFin, line);
	// ID
	std::getline(scenarioFin, line);

	if (indScenario == 0) {
		while (true) {
			std::getline(scenarioFin, line);
			pg_stringstreamStoreLine(&sstream, &line);
			sstream >> ID; // string "client" or end with "/udp_remote_client"
			if (ID.compare("/webCam") == 0) {
				break;
			}
			else if (ID.compare("camera") != 0) {
				sprintf(pg_errorStr, "Error: incorrect configuration file expected string \"camera\" not found! (instead \"%s\")", ID.c_str()); pg_ReportError(pg_errorStr); throw 100;
			}
			webCam cur_webCam;

			sstream >> cur_webCam.cameraString; // string "cameraString"

			sstream >> temp; // string "cameraID"
			try {
				cur_webCam.cameraID = stoi(temp);
			}
			catch (...) {
				sprintf(pg_errorStr, "Error: webcam configuration incorrect cameraID \"%s\"\n", temp.c_str()); pg_ReportError(pg_errorStr); throw 50;
			}

			sstream >> temp2; // string "cameraWidth"
			try {
				cur_webCam.cameraWidth = stoi(temp2);
			}
			catch (...) {
				sprintf(pg_errorStr, "Error: webcam configuration incorrect cameraWidth \"%s\"\n", temp2.c_str()); pg_ReportError(pg_errorStr); throw 50;
			}

			sstream >> temp3; // string "cameraHeight"
			try {
				cur_webCam.cameraHeight = stoi(temp3);
			}
			catch (...) {
				sprintf(pg_errorStr, "Error: webcam configuration incorrect cameraHeight \"%s\"\n", temp3.c_str()); pg_ReportError(pg_errorStr); throw 50;
			}		//std::cout << temp2 << "\n";
			pg_webCams.push_back(cur_webCam);
		}
	}
	else {
		while (true) {
			std::getline(scenarioFin, line);
			pg_stringstreamStoreLine(&sstream, &line);
			sstream >> ID; // string "client" or end with "/udp_remote_client"
			if (ID.compare("/webCam") == 0) {
				break;
			}
			else if (ID.compare("camera") != 0) {
				sprintf(pg_errorStr, "Error: incorrect configuration file expected string \"camera\" not found! (instead \"%s\")", ID.c_str()); pg_ReportError(pg_errorStr); throw 100;
			}
		}
	}

	// remote_IPCam Number of cameras
	std::getline(scenarioFin, line);
	pg_stringstreamStoreLine(&sstream, &line);
	sstream >> ID;
	if (ID.compare("remote_IPCam") != 0) {
		sprintf(pg_errorStr, "Error: incorrect configuration file expected string \"remote_IPCam\" not found! (instead \"%s\")", ID.c_str()); pg_ReportError(pg_errorStr); throw 100;
	}

	// VERBATIM
	std::getline(scenarioFin, line);
	// TYPE
	std::getline(scenarioFin, line);
	// ID
	std::getline(scenarioFin, line);

	if (indScenario == 0) {
		while (true) {
			std::getline(scenarioFin, line);
			pg_stringstreamStoreLine(&sstream, &line);
			sstream >> ID; // string "client" or end with "/udp_remote_client"
			if (ID.compare("/remote_IPCam") == 0) {
				break;
			}
			else if (ID.compare("IPCam") != 0) {
				sprintf(pg_errorStr, "Error: incorrect configuration file expected string \"IPCam\" not found! (instead \"%s\")", ID.c_str()); pg_ReportError(pg_errorStr); throw 100;
			}
			VideoCapture cur_IPCam;

			std::getline(scenarioFin, line);
			pg_stringstreamStoreLine(&sstream, &line);
			sstream >> temp; // string "IPCam_IP"
			sstream >> temp2; // string "IPCam_Port"
			sstream >> temp3; // string "IPCam_Device"
			//String fulllAddress = "http://" + temp + ":" + temp2;
			// "rtsp://[username]:[pass]@[ip address]/media/video1"
			// fulllAddress = "rtsp://" + temp + "/H264?ch=1&subtype=0";
			// fulllAddress = "rtsp://" + temp + "/H264?ch=1&subtype=0";
			//fulllAddress = "rtsp://" + temp + "/H264";
			//fulllAddress = "rtsp://" + temp + "/mjpeg.cgi?user=yukao.nagemi@gmail.com&password=Fire5432_&channel=0&.mjpg";
			//fulllAddress = "rtsp://yukao.nagemi@gmail.com:Fire5432_@" + temp + ":" + temp2 + "/onvif1";
			//fulllAddress = "rtsp://" + temp + ":" + temp2 + "/onvif1";
			//fulllAddress = "rtsp://" + temp + "/onvif1";
			// rtsp://192.168.1.65:8554/main
			//fulllAddress = "rtsp://192.168.1.65:8554/main";
			pg_IPCam_capture_address.push_back("rtsp://" + temp + ":" + temp2 + "/" + temp3);
			pg_IPCam_capture.push_back(cur_IPCam);
		}
	}
	else {
		while (true) {
			std::getline(scenarioFin, line);
			pg_stringstreamStoreLine(&sstream, &line);
			sstream >> ID; // string "client" or end with "/udp_remote_client"
			if (ID.compare("/remote_IPCam") == 0) {
				break;
			}
			else if (ID.compare("IPCam") != 0) {
				sprintf(pg_errorStr, "Error: incorrect configuration file expected string \"IPCam\" not found! (instead \"%s\")", ID.c_str()); pg_ReportError(pg_errorStr); throw 100;
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// MOVIE SCENARIO
//////////////////////////////////////////////////////////////////////////////////////////////////////
 
void pg_parseScenario_Videos(std::ifstream& scenarioFin, int indScenario) {
	std::stringstream  sstream;
	string line;
	string ID;
	string temp;
	string temp2;

	////////////////////////////
	////// VIDEOS
	// Number of videos
	std::getline(scenarioFin, line);
	pg_stringstreamStoreLine(&sstream, &line);
	sstream >> ID; // string videos
	if (ID.compare("videos") != 0) {
		sprintf(pg_errorStr, "Error: incorrect configuration file expected string \"videos\" not found! (instead \"%s\")", ID.c_str()); pg_ReportError(pg_errorStr); throw 100;
	}

	while (true) {
		// new line
		std::getline(scenarioFin, line);
		pg_stringstreamStoreLine(&sstream, &line);
		sstream >> ID; // string /videos or movie
		if (ID.compare("/videos") == 0) {
			break;
		}
		else if (ID.compare("movie") != 0) {
			sprintf(pg_errorStr, "Error: incorrect configuration file expected string \"movie\" not found! (instead \"%s\")", ID.c_str()); pg_ReportError(pg_errorStr); throw 100;
		}

		VideoTrack video;

		sstream >> temp;
		video.videoFileName = temp;
		if (!pg_isFullPath(video.videoFileName)) {
			video.videoFileName = pg_videos_directory + video.videoFileName;
		}
		sstream >> temp2;
		video.videoShortName = temp2;

		// in addition to the movie name and short name, 2 additional
		// sound files can be provided that are synchronized with the 
		// movie and used to generate beats from sound envelope at 1
		// or sound onsets detected through aubio library

		// possible additional soundtrack file with peaked sound envelope at 1.0 or above
		// NULL value or no value means no file
		if (sstream >> temp2) {
			// there is a soundtrack file with peaked sound envelope at 1.0
			if (temp2.compare("") != 0 && temp2.compare("NULL") != 0) {
				video.videoSoundtrackPeaksFileName = temp2;
				string csv_line;
				vector<float> peak_times;
				std::ifstream peak_file(temp2);
				if (!peak_file) {
					sprintf(pg_errorStr, "Error: peak file [%s] not found!", temp2.c_str()); pg_ReportError(pg_errorStr); throw 11;
				}
				printf("Read video soundtrack peaks [%s]\n", temp2.c_str());
				// reads the peaks timecodes and stores them in a float vector
				std::getline(peak_file, csv_line);
				std::getline(peak_file, csv_line);
				std::getline(peak_file, csv_line);
				while (std::getline(peak_file, csv_line)) {
					std::stringstream  peak_sstream;
					pg_stringstreamStoreLine(&peak_sstream, &csv_line);
					float time, sound;
					peak_sstream >> time;
					peak_sstream >> sound;
					if (sound >= 1) {
						peak_times.push_back(time);
					}
				}
				peak_file.close();
				video.videoSoundtrackPeaks = peak_times;
				//for (int i = 0; i < int(peak_times.size()); ++i) {
				//	std::cout << "Peak: " << peak_times[i] << '\n';
				//}
			}
			else {
				video.videoSoundtrackPeaksFileName = "";
				video.videoSoundtrackPeaks = {};
			}
			if (sstream >> temp2 && temp2.compare("") != 0 && temp2.compare("NULL") != 0) {
				video.videoSoundtrackOnsetsFileName = temp2;
				string csv_line;
				vector<float> onset_times;
				std::ifstream onset_file(temp2);
				if (!onset_file) {
					sprintf(pg_errorStr, "Error: onset file [%s] not found!", temp2.c_str()); pg_ReportError(pg_errorStr); throw 11;
				}
				// reads the peaks timecodes and stores them in a float vector
				printf("Read video soundtrack onsets [%s]\n", temp2.c_str());
				while (std::getline(onset_file, csv_line)) {
					std::stringstream  onset_sstream;
					pg_stringstreamStoreLine(&onset_sstream, &csv_line);
					float time;
					onset_sstream >> time;
					onset_times.push_back(time);
				}
				onset_file.close();
				video.videoSoundtrackOnsets = onset_times;
				//for (int i = 0; i < int(onset_times.size()); ++i) {
				//	std::cout << "Onset: " << onset_times[i] << '\n';
				//}
			}
			else {
				video.videoSoundtrackOnsetsFileName = "";
				video.videoSoundtrackOnsets = {};
			}
		}
		else {
			video.videoSoundtrackPeaksFileName = "";
			video.videoSoundtrackPeaks = {};
			video.videoSoundtrackOnsetsFileName = "";
			video.videoSoundtrackOnsets = {};
		}
		//std::cout << "movie : " << 
		// movieFileName[indVideo] << "\n";
		pg_VideoTracks[indScenario].push_back(video);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// CAMERA OSC COMMANDS
//////////////////////////////////////////////////////////////////////////////////////////////////////

void pg_aliasScript_Camera(string address_string, string string_argument_0,
	float float_arguments[PG_MAX_OSC_ARGUMENTS], int nb_arguments, int indVar) {
	// special command not in the scenario file
	switch (indVar) {
	case _camera_close:
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_cameraCaptFreq]) {
			// release webCam
			if (pg_webCam_capture.isOpened()) {
				pg_webCam_capture.release();
			}
		}
		break;
	case _camera_open:
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_cameraCaptFreq]) {
			// release webCam
			if (!pg_webCam_capture.isOpened() && cameraNo < 0 && -cameraNo - 1 < int(pg_webCams.size())) {
				pg_openCameraCaptureAndLoadFrame();
			}
		}
		break;
		// ====================================== 
		// relaunches CA initialization, in case it does not work                  
		// ====================================== 
	case _initialBGCapture:
#if defined(var_GenerativeNights_planes)
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_GenerativeNights_planes]) {
			pg_initialBGCapture = true;
		}

		// ====================================== 
		// initial background capture                  
		// ====================================== 
#endif
		break;
	case _flashCamera:
		pg_Make_flashCamera();
		break;
		// +++++++++++++++++ CAMERA PARAMETERS +++++++++++++++++++++ 
	case _cameraWB_R_plus:
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_cameraCaptFreq]) {
			*((float*)pg_FullScenarioVarPointers[_cameraWB_R]) += 100.f;
			pg_webCam_capture.set(CAP_PROP_WHITE_BALANCE_RED_V, (*((float*)pg_FullScenarioVarPointers[_cameraWB_R])));
			pg_CameraCurrent_exposure = *((float*)pg_FullScenarioVarPointers[_cameraWB_R]);
		}
		break;
	case _cameraWB_R_minus:
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_cameraCaptFreq]) {
			*((float*)pg_FullScenarioVarPointers[_cameraWB_R]) -= 100.f;
			pg_webCam_capture.set(CAP_PROP_WHITE_BALANCE_RED_V, (*((float*)pg_FullScenarioVarPointers[_cameraWB_R])));
			pg_CameraCurrent_exposure = *((float*)pg_FullScenarioVarPointers[_cameraWB_R]);
		}
		break;
	case _cameraWB_B_plus:
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_cameraCaptFreq]) {
			*((float*)pg_FullScenarioVarPointers[_cameraWB_B]) += 100.f;
			pg_webCam_capture.set(CAP_PROP_WHITE_BALANCE_BLUE_U, (*((float*)pg_FullScenarioVarPointers[_cameraWB_B])));
			pg_CameraCurrent_exposure = *((float*)pg_FullScenarioVarPointers[_cameraWB_B]);
		}
		break;
	case _cameraWB_B_minus:
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_cameraCaptFreq]) {
			*((float*)pg_FullScenarioVarPointers[_cameraWB_B]) -= 100.f;
			pg_webCam_capture.set(CAP_PROP_WHITE_BALANCE_BLUE_U, (*((float*)pg_FullScenarioVarPointers[_cameraWB_B])));
			pg_CameraCurrent_exposure = *((float*)pg_FullScenarioVarPointers[_cameraWB_B]);
		}
		break;
	case _cameraExposure_plus:
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_cameraCaptFreq]) {
			*((float*)pg_FullScenarioVarPointers[_cameraExposure]) += 1.f;
			pg_webCam_capture.set(CAP_PROP_EXPOSURE, (*((float*)pg_FullScenarioVarPointers[_cameraExposure])));
			pg_CameraCurrent_exposure = *((float*)pg_FullScenarioVarPointers[_cameraExposure]);
		}
		break;
	case _cameraExposure_minus:
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_cameraCaptFreq]) {
			*((float*)pg_FullScenarioVarPointers[_cameraExposure]) -= 1.f;
			pg_webCam_capture.set(CAP_PROP_EXPOSURE, (*((float*)pg_FullScenarioVarPointers[_cameraExposure])));
			pg_CameraCurrent_exposure = *((float*)pg_FullScenarioVarPointers[_cameraExposure]);
		}
		break;
	case _cameraGain_plus:
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_cameraCaptFreq]) {
			*((float*)pg_FullScenarioVarPointers[_cameraGain]) += 1.f;
			pg_webCam_capture.set(CAP_PROP_GAIN, (*((float*)pg_FullScenarioVarPointers[_cameraGain])));
			pg_CameraCurrent_exposure = *((float*)pg_FullScenarioVarPointers[_cameraGain]);
		}
		break;
	case _cameraGain_minus:
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_cameraCaptFreq]) {
			*((float*)pg_FullScenarioVarPointers[_cameraGain]) -= 1.f;
			pg_webCam_capture.set(CAP_PROP_GAIN, (*((float*)pg_FullScenarioVarPointers[_cameraGain])));
			pg_CameraCurrent_exposure = *((float*)pg_FullScenarioVarPointers[_cameraGain]);
		}
		break;
	case _cameraBrightness_plus:
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_cameraCaptFreq]) {
			*((float*)pg_FullScenarioVarPointers[_cameraBrightness]) += 10.f;
			pg_webCam_capture.set(CAP_PROP_BRIGHTNESS, (*((float*)pg_FullScenarioVarPointers[_cameraBrightness])));
			pg_CameraCurrent_exposure = *((float*)pg_FullScenarioVarPointers[_cameraBrightness]);
		}
		break;
	case _cameraBrightness_minus:
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_cameraCaptFreq]) {
			*((float*)pg_FullScenarioVarPointers[_cameraBrightness]) -= 10.f;
			pg_webCam_capture.set(CAP_PROP_BRIGHTNESS, (*((float*)pg_FullScenarioVarPointers[_cameraBrightness])));
			pg_CameraCurrent_exposure = *((float*)pg_FullScenarioVarPointers[_cameraBrightness]);
		}
		break;
	case _cameraSaturation_plus:
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_cameraCaptFreq]) {
			*((float*)pg_FullScenarioVarPointers[_cameraSaturation]) += 10.f;
			pg_webCam_capture.set(CAP_PROP_SATURATION, (*((float*)pg_FullScenarioVarPointers[_cameraSaturation])));
			pg_CameraCurrent_exposure = *((float*)pg_FullScenarioVarPointers[_cameraSaturation]);
		}
		break;
	case _cameraSaturation_minus:
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_cameraCaptFreq]) {
			*((float*)pg_FullScenarioVarPointers[_cameraSaturation]) -= 10.f;
			pg_webCam_capture.set(CAP_PROP_SATURATION, (*((float*)pg_FullScenarioVarPointers[_cameraSaturation])));
			pg_CameraCurrent_exposure = *((float*)pg_FullScenarioVarPointers[_cameraSaturation]);
		}
		break;
	case _cameraContrast_plus:
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_cameraCaptFreq]) {
			*((float*)pg_FullScenarioVarPointers[_cameraContrast]) += 10.f;
			pg_webCam_capture.set(CAP_PROP_CONTRAST, (*((float*)pg_FullScenarioVarPointers[_cameraContrast])));
			pg_CameraCurrent_exposure = *((float*)pg_FullScenarioVarPointers[_cameraContrast]);
		}
		break;
	case _cameraContrast_minus:
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_cameraCaptFreq]) {
			*((float*)pg_FullScenarioVarPointers[_cameraContrast]) -= 10.f;
			pg_webCam_capture.set(CAP_PROP_CONTRAST, (*((float*)pg_FullScenarioVarPointers[_cameraContrast])));
			pg_CameraCurrent_exposure = *((float*)pg_FullScenarioVarPointers[_cameraContrast]);
		}
		break;
	case _cameraGamma_plus:
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_cameraCaptFreq]) {
			*((float*)pg_FullScenarioVarPointers[_cameraGamma]) += 10.f;
			pg_webCam_capture.set(CAP_PROP_GAMMA, (*((float*)pg_FullScenarioVarPointers[_cameraGamma])));
			pg_CameraCurrent_exposure = *((float*)pg_FullScenarioVarPointers[_cameraGamma]);
		}
		break;
	case _cameraGamma_minus:
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_cameraCaptFreq]) {
			*((float*)pg_FullScenarioVarPointers[_cameraGamma]) -= 10.f;
			pg_webCam_capture.set(CAP_PROP_GAMMA, (*((float*)pg_FullScenarioVarPointers[_cameraGamma])));
			pg_CameraCurrent_exposure = *((float*)pg_FullScenarioVarPointers[_cameraGamma]);
		}
		break;

		// +++++++++++++++++ CAMERA RESET +++++++++++++++++++++ 
	case _reset_on_camera:
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_cameraCaptFreq]) {
			reset_camera = true;
			// the camera weight is set to 1 in 10 frames
			pg_delayedCameraWeight = 11;
		}
		break;
	case _reset_camera_params:
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_cameraCaptFreq]) {
			pg_initWebcamParameters();
		}
		break;

		// +++++++++++++++++ CAMERA IMAGE CUMUL MODE +++++++++++++++ 
	case _cameraCumul_plus:
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_cameraCaptFreq]) {
			cameraCumul = (cameraCumul + 1) % nb_camera_cumul_modes;
			pg_BrokenInterpolationVar[_cameraCumul] = true;
		}
		break;

	default:
		sprintf(pg_errorStr, "Camera command not found (%s)!", address_string.c_str()); pg_ReportError(pg_errorStr);
		break;
	}
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
// MOVIE OSC COMMANDS
//////////////////////////////////////////////////////////////////////////////////////////////////////

void pg_aliasScript_Movie(string address_string, string string_argument_0,
	float float_arguments[PG_MAX_OSC_ARGUMENTS], int nb_arguments, int indVar) {
	// special command not in the scenario file
	switch (indVar) {
	case _currentVideoTrack_plus:
		currentVideoTrack = (currentVideoTrack + 1) % PG_NB_TRACKS;
		break;
	case _movie_plus:
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_movieCaptFreq]) {
			if (!pg_VideoTracks[pg_ind_scenario].empty()) {
				playing_movieNo = (playing_movieNo + 1) % pg_VideoTracks[pg_ind_scenario].size();
				pg_play_movie_no();
			}
		}
		break;

	case _movie_minus:
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_movieCaptFreq]) {
			if (!pg_VideoTracks[pg_ind_scenario].empty()) {
				playing_movieNo = (playing_movieNo - 1 + pg_VideoTracks[pg_ind_scenario].size()) % pg_VideoTracks[pg_ind_scenario].size();
				pg_play_movie_no();
			}
		}
		break;
	case _movie_forward:
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_movieCaptFreq]) {
			if (pg_currentlyPlaying_movieNo >= 0) {
				pg_movie_forward(int(movieCaptFreq * 10));
			}
		}
		break;
	case _movie_backward:
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_movieCaptFreq]) {
			if (pg_currentlyPlaying_movieNo >= 0) {
				pg_movie_backward(int(movieCaptFreq * 10));
			}
		}
		break;
	case _movie_loop_onOff:
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_movieCaptFreq]) {
			movie_loop = !movie_loop;
			sprintf(pg_AuxString, "/movie_loop_onOff %d", int(movie_loop));
			pg_send_message_udp((char*)"i", pg_AuxString, (char*)"udp_TouchOSC_send");
			printf("Movie loop On/Off %d\n", int(movie_loop));
		}
		break;

	default:
		sprintf(pg_errorStr, "Movie command not found (%s)!", address_string.c_str()); pg_ReportError(pg_errorStr);
		break;
	}
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
// CAMERA CALLBACKS
//////////////////////////////////////////////////////////////////////////////////////////////////////

void cameraGamma_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_cameraCaptFreq]) {
		if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
			// sprintf(pg_AuxString, "/message Gamma%.1f", scenario_or_gui_command_value); pg_send_message_udp((char *)"s", (char *)pg_AuxString, (char *)"udp_TouchOSC_send");
			if (scenario_or_gui_command_value != pg_CameraCurrent_gamma) {
				//printf("Cam gamma %.2f\n", scenario_or_gui_command_value);
				// cvSetCaptureProperty comment for see3cam
				pg_webCam_capture.set(CAP_PROP_GAMMA, scenario_or_gui_command_value);
				pg_CameraCurrent_gamma = scenario_or_gui_command_value;
				pg_LastCameraParameterChange_Frame = pg_FrameNo;
			}
		}
	}
}
void cameraGain_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_cameraCaptFreq]) {
		if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
			if (scenario_or_gui_command_value != pg_CameraCurrent_gain) {
				printf("Cam gain %.2f\n", scenario_or_gui_command_value);
				// cvSetCaptureProperty comment for see3cam 
				pg_webCam_capture.set(CAP_PROP_GAIN, scenario_or_gui_command_value);
				pg_CameraCurrent_gain = scenario_or_gui_command_value;
				pg_LastCameraParameterChange_Frame = pg_FrameNo;
			}
		}
	}
}
void cameraBrightness_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_cameraCaptFreq]) {
		if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
			if (scenario_or_gui_command_value != pg_CameraCurrent_brightness) {
				//printf("Cam brightness %.2f\n", scenario_or_gui_command_value);
				// cvSetCaptureProperty comment for see3cam
				pg_webCam_capture.set(CAP_PROP_BRIGHTNESS, scenario_or_gui_command_value);
				pg_CameraCurrent_brightness = scenario_or_gui_command_value;
				pg_LastCameraParameterChange_Frame = pg_FrameNo;
			}
		}
	}
}
void cameraSaturation_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_cameraCaptFreq]) {
		if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
			if (scenario_or_gui_command_value != pg_CameraCurrent_saturation) {
				//printf("Cam saturation %.2f\n", scenario_or_gui_command_value);
				// cvSetCaptureProperty comment for see3cam 
				pg_webCam_capture.set(CAP_PROP_SATURATION, scenario_or_gui_command_value);
				pg_CameraCurrent_saturation = scenario_or_gui_command_value;
				pg_LastCameraParameterChange_Frame = pg_FrameNo;
			}
		}
	}
}
void cameraContrast_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_cameraCaptFreq]) {
		if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
			if (scenario_or_gui_command_value != pg_CameraCurrent_contrast) {
				//printf("Cam contrast %.2f\n", scenario_or_gui_command_value);
				// cvSetCaptureProperty comment for see3cam
				pg_webCam_capture.set(CAP_PROP_CONTRAST, scenario_or_gui_command_value);
				pg_CameraCurrent_contrast = scenario_or_gui_command_value;
				pg_LastCameraParameterChange_Frame = pg_FrameNo;
			}
		}
	}
}
void cameraWB_B_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_cameraCaptFreq]) {
		if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
			if (scenario_or_gui_command_value != pg_CameraCurrent_WB_B) {
				//printf("Cam WB B %.2f\n", scenario_or_gui_command_value);
				// cvSetCaptureProperty comment for see3cam
				pg_webCam_capture.set(CAP_PROP_WHITE_BALANCE_BLUE_U, scenario_or_gui_command_value);
				pg_CameraCurrent_WB_B = scenario_or_gui_command_value;
				pg_LastCameraParameterChange_Frame = pg_FrameNo;
			}
		}
	}
}
void cameraWB_R_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_cameraCaptFreq]) {
		if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
			if (scenario_or_gui_command_value != pg_CameraCurrent_WB_R) {
				//printf("Cam WB R %.2f\n", scenario_or_gui_command_value);
				// cvSetCaptureProperty comment for see3cam
				if (scenario_or_gui_command_value >= 0) {
					pg_webCam_capture.set(CAP_PROP_WHITE_BALANCE_RED_V, scenario_or_gui_command_value);
					pg_CameraCurrent_WB_R = scenario_or_gui_command_value;
					pg_LastCameraParameterChange_Frame = pg_FrameNo;
				}
			}
		}
	}
}
void cameraNo_callBack(pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value) {
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_cameraCaptFreq]) {
		if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
			//printf("camera current %d new %d param type %d\n", pg_current_active_cameraNo, cameraNo, param_input_type);
			if (cameraNo != pg_current_active_cameraNo) {
				if (pg_FrameNo != -1) {
					pg_releaseCameraCapture();
					pg_openCameraCaptureAndLoadFrame();
				}
			}
		}
	}
}

void cameraExposure_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_cameraCaptFreq]) {
		if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
			// sprintf(pg_AuxString, "/message Exposure_%.1f", scenario_or_gui_command_value); pg_send_message_udp((char *)"s", (char *)pg_AuxString, (char *)"udp_TouchOSC_send");
			if (scenario_or_gui_command_value != pg_CameraCurrent_exposure) {
				//printf("Cam exposure %.2f\n", scenario_or_gui_command_value);
				// cvSetCaptureProperty comment for see3cam
				//pg_webCam_capture.set(CAP_PROP_EXPOSURE, scenario_or_gui_command_value);
				pg_CameraCurrent_exposure = scenario_or_gui_command_value;
				pg_LastCameraParameterChange_Frame = pg_FrameNo;
			}
		}
	}
}
void flashCameraTrkLength_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_KEYSTROKE || param_input_type == pg_enum_PG_SCENARIO) {
		if (scenario_or_gui_command_value > 0.) {
			pg_flashCameraTrk_decay = float(1. / scenario_or_gui_command_value);
		}
		else {
			pg_flashCameraTrk_decay = 0.001f;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// MOVIE CALLBACKS
//////////////////////////////////////////////////////////////////////////////////////////////////////

void playing_movieNo_callBack(pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value) {
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_movieCaptFreq]) {
		if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
			// starts playing another video
			pg_play_movie_no();

			// if the video no is outside the range of available videos: stops the playing video
			if (playing_movieNo != pg_currentlyPlaying_movieNo
				&& (playing_movieNo < 0 || playing_movieNo >= int(pg_VideoTracks[pg_ind_scenario].size()))) {
				printf("video no is outside the range of available videos: stops the playing video \n");

				pg_currentlyPlaying_movieNo = -1;
				movieWeight = 0.f;
				pg_movie_frame.setTo(Scalar(0, 0, 0));
				sprintf(pg_AuxString, "/movie_shortName %s", "---");
				pg_send_message_udp((char*)"s", pg_AuxString, (char*)"udp_TouchOSC_send");
			}
		}
	}
}
void movieCaptFreq_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_movieCaptFreq]) {
		if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
			//printf("Movie No %d\n", playing_movieNo);
			// if the video no is outside the range of available videos: stops the playing video
			if (playing_movieNo >= 0 && playing_movieNo < int(pg_VideoTracks[pg_ind_scenario].size())) {
				pg_movie_status.reset_initialTime(scenario_or_gui_command_value);
			}
		}
	}
}
