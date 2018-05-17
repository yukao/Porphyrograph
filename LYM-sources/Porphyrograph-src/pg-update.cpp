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
float mouse_x;
float mouse_y;
float mouse_x_prev = -1;
float mouse_y_prev = -1;
float paths_x[PG_NB_PATHS + 1];
float paths_y[PG_NB_PATHS + 1];
float paths_x_prev[PG_NB_PATHS + 1];
float paths_y_prev[PG_NB_PATHS + 1];
float paths_Color_r[PG_NB_PATHS + 1];
float paths_Color_g[PG_NB_PATHS + 1];
float paths_Color_b[PG_NB_PATHS + 1];
float paths_Color_a[PG_NB_PATHS + 1];
float repop_Color_r;
float repop_Color_g;
float repop_Color_b;
int paths_BrushID[PG_NB_PATHS + 1];
float paths_RadiusX[PG_NB_PATHS + 1];
float paths_RadiusY[PG_NB_PATHS + 1];

// on/off automatic image output in case of live performance
bool is_automatic_snapshots = true;

// interpolated CA
int CAInterpolatedType;
int CAInterpolatedSubType;
int CAInterpolatedType_prev = -1;
int CAInterpolatedSubType_prev = -1;


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
float sensor_last_activation_time;
// sensor activation as read from the pixel under their center
float sensorValues[PG_NB_SENSORS];
// all sensor activation patterns
bool sensorActivations[PG_NB_SENSORS * PG_NB_MAX_SENSOR_ACTIVATIONS];

// sample choice
// current sample choice
int sample_choice[PG_NB_SENSORS];
// all possible sensor layouts
int sample_setUps[PG_NB_MAX_SAMPLE_SETUPS][PG_NB_SENSORS] =
{ { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 },
{ 16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31 },
{ 32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47 } };
// sample start playing time for muting after 1 cycle
float sample_play_start[PG_NB_MAX_SAMPLE_SETUPS * PG_NB_SENSORS] =
{ -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f,
-1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f,
-1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f };
float sample_play_volume[PG_NB_MAX_SAMPLE_SETUPS * PG_NB_SENSORS] =
{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };

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
GLuint pg_particle_initial_images_texID[PG_NB_PARTICLE_INITIAL_IMAGES][2];

GLuint pg_screenMessageBitmap_texID = 0; // nb_attachments=1
GLubyte *pg_screenMessageBitmap = NULL;

GLuint pg_paths_Pos_Texture_texID[PG_NB_PATHS + 1];
GLfloat *pg_paths_Pos_Texture[PG_NB_PATHS + 1];
GLuint pg_paths_Col_Texture_texID[PG_NB_PATHS + 1];
GLfloat *pg_paths_Col_Texture[PG_NB_PATHS + 1];
GLuint pg_paths_RadBrushRendmode_Texture_texID[PG_NB_PATHS + 1];
GLfloat *pg_paths_RadBrushRendmode_Texture[PG_NB_PATHS + 1];

#if defined (GN) || defined (MALAUSSENA)
GLuint pg_CATable_ID = NULL_ID;
GLubyte *pg_CATable = NULL;
#endif

#ifdef GN
GLuint pg_LYMlogo_texID = NULL_ID;
cv::Mat pg_LYMlogo_image;
#endif

GLuint Screen_Font_texture_Rectangle_texID = 0;
cv::Mat Screen_Font_image;
#if defined (TVW) || defined (CRITON)
GLuint Display_Font_texture_Rectangle_texID = 0;
cv::Mat Display_Font_image;
#endif

GLuint Pen_texture_3D_texID = 0;
GLuint Noise_texture_3D = 0;

#if defined (TVW) || defined (CRITON)
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

std::string* DisplayTextList;
int* DisplayTextFirstInChapter;
int NbDisplayTexts = 0;

// message chapters
int NbTextChapters = 0;
#endif

#ifdef PG_SENSORS
GLuint Sensor_texture_rectangle = NULL_ID;
cv::Mat Sensor_image;
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
int pg_camera_frame_width = 0;
int pg_camera_frame_height = 0;
int pg_movie_frame_width = 0;
int pg_movie_frame_height = 0;
int currentlyPlaying_movieNo = -1;
int current_pen_colorPreset = -1;

// video tracks
vector<string> movieFileName;
vector<string> movieShortName;
int nb_movies = 0;
// soundtracks
vector<string> trackFileName;
vector<string> trackShortName;
int currentlyPlaying_trackNo = -1;
int nb_soundtracks = 0;
// pen palettes presets
int nb_pen_colorPresets = 0;
vector<string> pen_colorPresets_names;
float *pen_colorPreset_values = NULL;
// pen palettes colors
int nb_pen_palette_colors = 0;
vector<string> pen_palette_colors_names;
float **pen_palette_colors_values = NULL;
// photo albums
vector<string> photoAlbumDirName;
int nb_photo_albums = 0;
// pen brushes
string pen_brushes_fileName;
int nb_pen_brushes = 0;

bool is_capture_diaporama = false;
VideoCapture  pg_camera_capture;
char pg_camera_capture_name[256];
VideoCapture  pg_movie_capture;
int pg_movie_nbFrames = 0;
bool is_movieLooping = false;
bool is_movieLoading = false;
bool secondCurrentBGCapture = false;
#ifdef GN
bool secondInitialBGCapture = false;
bool initialBGCapture = false;
#endif

const char *TextureEncodingString[EmptyTextureEncoding + 1] = { "jpeg", "png", "pnga", "png_gray", "pnga_gray", "rgb", "raw", "emptyimagetype" };


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
  // SCENE UPDATE AND SAHDER UNIFORM PARAMETERS UPDATE
  //////////////////////////////////////////////////
  // scenario update
  printOglError(466);
  pg_update_scenario();

  // records the current mouse position in the variable
  // and in the tracks is recording is active
  updateMouseEnvironmentVariablesAndTables(CurrentClockTime);

  // ships uniform variables
  pg_update_shader_uniforms();
  printOglError(51);
  
  // loads movie and/or camera frames
  camera_and_video_frame_updates();


  //////////////////////////////////////////////////
  //////////////////////////////////////////////////
  // SCENE DISPLAY AND SAHDER UNIFORM PARAMETERS UPDATE
  //////////////////////////////////////////////////
  // OpenGL initializations before redisplay
  OpenGLInit();
  // printOglError(509);

  // proper scene redrawing
  pg_draw_scene( _Render );

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
  if( outputSvg && is_automatic_snapshots
	&& pg_FrameNo % stepSvg == 0
	&& pg_FrameNo / stepSvg >= beginSvg && 
	pg_FrameNo / stepSvg <= endSvg ) {
    // printf("Draw Svg %d\n" , pg_FrameNo  );
    pg_draw_scene( _Svg );
  }
    
  // ---------------- frame by frame output --------------------- //
  // Png screen shots
  // printf("Draw Png\n" );
  if( outputPng && is_automatic_snapshots
	&& pg_FrameNo % stepPng == 0
	&& pg_FrameNo / stepPng >= beginPng && 
	pg_FrameNo / stepPng <= endPng ) {
    pg_draw_scene( _Png );
  }
    
  // ---------------- frame by frame output --------------------- //
  // Jpg screen shots
  // printf("Draw Jpg\n"  );
  if( outputJpg && is_automatic_snapshots
	&& pg_FrameNo % stepJpg == 0
	&& pg_FrameNo / stepJpg >= beginJpg && 
	pg_FrameNo / stepJpg <= endJpg ) {
    pg_draw_scene( _Jpg );
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
//int sample_setUps[ PG_NB_MAX_SAMPLE_SETUPS][ PG_NB_SENSORS ] =
//  {{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25},
//   {26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50},
//   {51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75}};
//// groups of samples for aliasing with additive samples

void readSensors(void) {
	bool sensorOn[PG_NB_SENSORS + 18];

	bool sampleOn[PG_NB_MAX_SAMPLE_SETUPS * PG_NB_SENSORS];
	int sampleToSensorPointer[PG_NB_MAX_SAMPLE_SETUPS * PG_NB_SENSORS];

	GLubyte pixelColor[3 * PG_NB_SENSORS];

	// marks all the samples as unread
	for (int indSample = 0; indSample < PG_NB_MAX_SAMPLE_SETUPS * PG_NB_SENSORS; indSample++) {
		sampleOn[indSample] = false;
		sampleToSensorPointer[indSample] = -1;
	}

	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	// sensor readback
	//printf("sensor on ");
	for (int indSens = 0; indSens < PG_NB_SENSORS; indSens++) {
		if (sensor_onOff[indSens]) {
			//printf("position %d %d\n", (int)sensorPositions[3 * indSens],
			//	(int)(sensorPositions[3 * indSens + 1]));
			glReadPixels((int)sensorPositions[3 * indSens],
				(int)(sensorPositions[3 * indSens + 1]),
				1, 1,
				GL_RGB, GL_UNSIGNED_BYTE, pixelColor + 3 * indSens);
			sensorValues[indSens] =
				(pixelColor[3 * indSens] + pixelColor[3 * indSens + 1] + pixelColor[3 * indSens + 2]) / (255.f * 3.f);
			sensorOn[indSens] = (sensorValues[indSens] > 0.0f);
			if (sensorOn[indSens]) {
				sampleOn[sample_choice[indSens]] = true;
				sampleToSensorPointer[sample_choice[indSens]] = indSens;
				//printf("ON#%d ",indSens);
			}
		}
		else {
			sensorValues[indSens] = 0.0f;
			sensorOn[indSens] = false;
		}
	}
	//printf("\n");
	printOglError(691);

#ifdef PG_SUPERCOLLIDER
	// looks for buffer aliasing possibilities: groups of sounds that could be replaced by a single buffer
	float sensorValues[PG_NB_SENSORS + 18];
	bool groupOn[PG_NB_SENSOR_GROUPS];
	float groupValues[PG_NB_SENSOR_GROUPS];
	//printf("group on ");
	for (int indgroup = 0; indgroup < PG_NB_SENSOR_GROUPS; indgroup++) {
		if (sampleOn[sample_groups[indgroup][0]] && sampleOn[sample_groups[indgroup][1]]
			&& sampleOn[sample_groups[indgroup][2]] && sampleOn[sample_groups[indgroup][3]]) {
			// switches on the group with the average activation value
			groupOn[indgroup] = true;
			groupValues[indgroup] = (sensorValues[sampleToSensorPointer[sample_groups[indgroup][0]]]
				+ sensorValues[sampleToSensorPointer[sample_groups[indgroup][1]]]
				+ sensorValues[sampleToSensorPointer[sample_groups[indgroup][2]]]
				+ sensorValues[sampleToSensorPointer[sample_groups[indgroup][3]]]);
			// switches off the associated sensors
			sensorValues[sampleToSensorPointer[sample_groups[indgroup][0]]] = 0.0f;
			sensorOn[sampleToSensorPointer[sample_groups[indgroup][0]]] = false;
			sensorValues[sampleToSensorPointer[sample_groups[indgroup][1]]] = 0.0f;
			sensorOn[sampleToSensorPointer[sample_groups[indgroup][1]]] = false;
			sensorValues[sampleToSensorPointer[sample_groups[indgroup][2]]] = 0.0f;
			sensorOn[sampleToSensorPointer[sample_groups[indgroup][2]]] = false;
			sensorValues[sampleToSensorPointer[sample_groups[indgroup][3]]] = 0.0f;
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
			float_str = std::to_string(static_cast<long double>(sensorValues[indSens]));
			// float_str.resize(4);
			message += " " + float_str;
			totalAmplitude += sensorValues[indSens];
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

	// message value
	std::string float_string;
	std::string int_string;

	//printf("ON-OFF %d %d %d %d %d %d %d %d\n", sensor_onOff[0], sensor_onOff[1], sensor_onOff[2], sensor_onOff[3], sensor_onOff[4], sensor_onOff[5], sensor_onOff[6], sensor_onOff[7]);
	//printf("ON %d %d %d %d %d %d %d %d\n", sensorOn[0], sensorOn[1], sensorOn[2], sensorOn[3], sensorOn[4], sensorOn[5], sensorOn[6], sensorOn[7]);
	//printf("PLAY %.1f %.1f %.1f %.1f %.1f %.1f %.1f %.1f\n", sample_play_start[0], sample_play_start[1], sample_play_start[2], sample_play_start[3], sample_play_start[4], sample_play_start[5], sample_play_start[6], sample_play_start[7]);
	//printf("VAL %.1f %.1f %.1f %.1f %.1f %.1f %.1f %.1f\n", sensorValues[0], sensorValues[1], sensorValues[2], sensorValues[3], sensorValues[4], sensorValues[5], sensorValues[6], sensorValues[7]);
	for (int indSens = 0; indSens < PG_NB_SENSORS; indSens++) {
		if (sensorOn[indSens] // active sensor
			&& sensorValues[indSens] > 0 // non black pixel
			&& sample_play_start[sample_choice[indSens]] < 0 // not currently playing
			) {
			// message format
			std::string format = "f";

#ifdef PG_RENOISE
			std::string message = "/renoise/song/track/";
			int_string = std::to_string(sample_choice[indSens] + 1);
			message += int_string + "/unmute_vol";
			float_string = std::to_string(static_cast<long double>(sensorValues[indSens])*sensor_vol);
			// float_str.resize(4);
			message += " " + float_string;
			// printf("send %s\n", message.c_str());
			// message posting
			pg_send_message_udp((char *)format.c_str(), (char *)message.c_str(), (char *)"udp_RN_send");
#endif
#ifdef PG_PORPHYROGRAPH_SOUND
			std::string message = "/track_";
			int_string = std::to_string(sample_choice[indSens] + 1);
			message += int_string + "_level";
			float_string = std::to_string(static_cast<long double>(sensorValues[indSens]));
			// float_str.resize(4);
			message += " " + float_string;
			// message posting
			pg_send_message_udp((char *)format.c_str(), (char *)message.c_str(), (char *)"udp_PGsound_send");
			// printf("send %s\n", message.c_str());
#endif

			// starts the clock for stopping the sample play after a certain time
			sample_play_start[sample_choice[indSens]] = CurrentClockTime;
			sample_play_volume[sample_choice[indSens]] = sensorValues[indSens];
			// printf("lights sensor #%d\n", indSens);
		}
		// the sample has been triggered and has not yet reached 90% of its playing duration
		if (sample_play_start[sample_choice[indSens]] > 0.0
			&& CurrentClockTime - sample_play_start[sample_choice[indSens]] <= 0.9f * BEAT_DURATION) {
			// set the value to the initial value until 0.9 so that there is one visual feedback per loop
			sensorValues[indSens] = (sample_play_volume[sample_choice[indSens]]
				* ((CurrentClockTime - sample_play_start[sample_choice[indSens]]) / BEAT_DURATION));
		}
	}
	printOglError(689);


	for (int indSetup = 0; indSetup < PG_NB_MAX_SAMPLE_SETUPS; indSetup++) {
		for (int indSens = 0; indSens < PG_NB_SENSORS; indSens++) {
			int indSample = sample_setUps[indSetup][indSens];
			if (sample_play_start[indSample] > 0
				&& CurrentClockTime - sample_play_start[indSample] > BEAT_DURATION) {
#ifdef PG_RENOISE
				std::string message = "/renoise/song/track/";
				int_string = std::to_string(indSample + 1);
				message += int_string + "/mute";
				// printf("send %s\n", message.c_str());

				// message format
				std::string format = "";

				// message posting
				pg_send_message_udp((char *)format.c_str(), (char *)message.c_str(), (char *)"udp_RN_send");
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
	printOglError(690);

	// message trace
	//std::cout << "format: " << format << "\n";
	//std::cout << "msg: " << message << "\n";
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


	////////////////////////////
	// flash video reset
	if (flashCameraTrk_weight > 0.0f) {
		if (flashCameraTrk_weight - flashCameraTrk_decay > 0) {
			flashCameraTrk_weight -= flashCameraTrk_decay;
		}
		else {
			flashCameraTrk_weight = 0.0f;
		}
		// printf("coef %.3f\n", flashCameraTrk_weight);
	}

	// /////////////////////////
	// clear layer reset
	// does not reset if camera capture is still ongoing
	if ((!currentBGCapture && !secondCurrentBGCapture)
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
	// layer copy reset
	// copy to layer above (+1) or to layer below (-1)
	copyToNextTrack = 0;

	// blur reset
	is_blur_1 = false;
	is_blur_2 = false;

	// /////////////////////////
	// particle initialization reset
	part_initialization = -1;
}

void camera_and_video_frame_updates(void) {
	//////////////////////////////////////////////////////////////////////
	// CAMERA AND VIDEO FRAME UPDATES BEFORE RENDERING
	// /////////////////////////
	// reads next movie frame
	if (currentVideoTrack >= 0 && movieCaptFreq > 0
		&& currentlyPlaying_movieNo >= 0
		&& !is_movieLooping && !is_movieLoading
		&& pg_FrameNo % int(60.0 / movieCaptFreq) == 0 // currentVideoTrack <=> video off
													   // frame capture
		) {

		if (pg_movie_nbFrames < 10 || !pg_movie_capture.grab()) {
			// loops unless a thread for looping has been launched
			printf("movie loop %d\n", pg_movie_nbFrames);
			is_movieLooping = true;
#ifdef WIN32
			DWORD rc;
			HANDLE  hThread = CreateThread(
				NULL,                   // default security attributes
				0,                      // use default stack size  
				pg_movieLoop,		    // thread function name
				(void *)NULL,		    // argument to thread function 
				0,                      // use default creation flags 
				&rc);   // returns the thread identifier 
			if (hThread == NULL) {
				std::cout << "Error:unable to create thread pg_movieLoop" << std::endl;
				exit(-1);
			}
			CloseHandle(hThread);
#else
			pthread_t drawing_thread;
			int rc;
			rc = pthread_create(&drawing_thread, NULL,
				pg_movieLoop, (void *)NULL);
			if (rc) {
				std::cout << "Error:unable to create thread pg_movieLoop" << rc << std::endl;
				exit(-1);
			}
			pthread_exit(NULL);
#endif
		}
		else {
			// non threaded
			Mat pg_movie_frame;
			pg_movie_capture >> pg_movie_frame;
			pg_movie_nbFrames--;
			if (pg_movie_frame.data) {
				glEnable(GL_TEXTURE_RECTANGLE);
				glBindTexture(GL_TEXTURE_RECTANGLE, pg_movie_texture_texID);
				glTexImage2D(GL_TEXTURE_RECTANGLE,     // Type of texture
					0,                 // Pyramid level (for mip-mapping) - 0 is the top level
					GL_RGB,            // Internal colour format to convert to
					pg_movie_frame_width,          // Image width  i.e. 640 for Kinect in standard mode
					pg_movie_frame_height,          // Image height i.e. 480 for Kinect in standard mode
					0,                 // Border width in pixels (can either be 1 or 0)
					GL_BGR, // Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
					GL_UNSIGNED_BYTE,  // Image data type
					(char *)pg_movie_frame.data);        // The actual image data itself
			}
		}
	}

	// /////////////////////////
	// reads next camera frame
#ifdef PG_WITH_CAMERA_CAPTURE
	if (currentVideoTrack >= 0
		&& (// grabs according to camera capture frequence
		(cameraCaptFreq > 0 && pg_FrameNo % int(60.0 / cameraCaptFreq) == 0)
			// grabs at the beginning, otherwise the program is stuck if no camera frame reading takes place
			|| (pg_FrameNo >= PG_CAMERA_REST_DELAY && pg_FrameNo <= PG_CAMERA_REST_DELAY + 1000 && pg_FrameNo % 3 == 0))
		// a delay has to be let for camera to digest new parameter values
		&& (pg_FrameNo - pg_LastCameraParameterChange_Frame) > PG_CAMERA_REST_DELAY) {
		// currentVideoTrack <=> video off
		// frame capture
		// printf("frame capture\n");

		// non threaded
		// printf("*** non threaded camera frame capture %d\n", pg_FrameNo);
		loadCameraFrame(false);
	}
#endif
}

///////////////////////////////////////////////////////////////////////////////////////
// scene update

void pg_update_shader_uniforms(void) {
//#ifdef TVW
//	/////////////////////////////////////////////////////////////////////////
//	// TRANSFER OF LOADED BUFFER IMAGES TO GPU
//	GPUtransfer_Photo_buffer_dataTVW();
//#endif

#ifdef GN
#include "pg_update_body_GN.cpp"
#endif
#ifdef TVW
#include "pg_update_body_TVW.cpp"
#endif
#ifdef CRITON
#include "pg_update_body_Criton.cpp"
#endif
#ifdef effe
#include "pg_update_body_effe.cpp"
#endif
#ifdef DEMO
#include "pg_update_body_demo.cpp"
#endif
#ifdef MALAUSSENA
#include "pg_update_body_Malaussena.cpp"
#endif
#ifdef DASEIN
#include "pg_update_body_dasein.cpp"
#endif
	printOglError(510);

	/////////////////////////////////////////////////////////////////////////
	// CAMERA SHADER UNIFORM VARIABLES
//	glUseProgram(shader_programme[pg_shader_Camera]);
//	glUniform4f(uniform_Camera_fs_4fv_W_H,
//			(GLfloat)pg_camera_frame_width, (GLfloat)pg_camera_frame_height, 0.f, 0.f);

/////////////////////////////////////////////////////////////////////////
// PARTICLE ANIMATION SHADER UNIFORM VARIABLES
	glUseProgram(shader_programme[pg_shader_ParticleAnimation]);

	// scren size and repop channel
	// at each frame a random repop channel is chosen among the available ones
	bool repop_channels[PG_NB_PATHS + 1];
	int nb_repop_channels = 0;
#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7
	nb_repop_channels += int(path_repop_0) + int(path_repop_1) + int(path_repop_2) + int(path_repop_3);
	repop_channels[0] = path_repop_0;
	repop_channels[1] = path_repop_1;
	repop_channels[2] = path_repop_2;
	repop_channels[3] = path_repop_3;
#endif
#if PG_NB_PATHS == 7
	nb_repop_channels += int(path_repop_4) + int(path_repop_5) + int(path_repop_6) + int(path_repop_7);
	repop_channels[4] = path_repop_4;
	repop_channels[5] = path_repop_5;
	repop_channels[6] = path_repop_6;
	repop_channels[7] = path_repop_7;
#endif
	int selected_channel = int(floor(randomValue * (nb_repop_channels - 0.00001)));
	int nbActChannels = 0;
	for (int indCh = 0; indCh < PG_NB_PATHS + 1; indCh++) {
		if (repop_channels[indCh]) {
			if (nbActChannels == selected_channel) {
				selected_channel = indCh;
				break;
			}
			nbActChannels++;
		}
	}
	glUniform4f(uniform_ParticleAnimation_fs_4fv_W_H_repopChannel_targetFrameNo,
		(GLfloat)leftWindowWidth, (GLfloat)window_height, (GLfloat)selected_channel, GLfloat(pg_targetFrameNo));

#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7
	// pen paths positions
#if defined (TVW) || defined (CRITON)
	// special case for army explosion: track 1 is assigned as repulse or follow path but is not replayed
	// the center of the screen is the default position for this track
	if (part_path_repulse_1 || part_path_follow_1) {
		paths_x[1] = float(leftWindowWidth / 2);
		paths_y[1] = float(window_height / 2);
	}
	// special case for army radar: track 2 is assigned as repop path but is not replayed
	// the center of the top left screen is the default position for this track
	else if (path_repop_2 || part_path_repulse_2) {
		paths_x[2] = float(leftWindowWidth / 4);
		paths_y[2] = float(window_height / 4);
		float randval = float(randomValue * 2 * PI);
		float radius = randomValue;
		paths_x_prev[2] = float(leftWindowWidth / 4 + 2 * radius * cos(randval));
		paths_y_prev[2] = float(window_height / 4 + 2 * radius * sin(randval));
	}
#endif
	glUniform4f(uniform_ParticleAnimation_fs_4fv_paths03_x, paths_x[0], paths_x[1], paths_x[2], paths_x[3]);
	glUniform4f(uniform_ParticleAnimation_fs_4fv_paths03_y, paths_y[0], paths_y[1], paths_y[2], paths_y[3]);
	glUniform4f(uniform_ParticleAnimation_fs_4fv_paths03_x_prev,
		paths_x_prev[0], paths_x_prev[1], paths_x_prev[2], paths_x_prev[3]);
	glUniform4f(uniform_ParticleAnimation_fs_4fv_paths03_y_prev,
		paths_y_prev[0], paths_y_prev[1], paths_y_prev[2], paths_y_prev[3]);

	// pen paths brush & size
	glUniform4f(uniform_ParticleAnimation_fs_4fv_paths03_RadiusX,
		paths_RadiusX[0], paths_RadiusX[1], paths_RadiusX[2], paths_RadiusX[3]);
	// printf("Track radius x %.2f %.2f %.2f %.2f\n" , paths_RadiusX[0], paths_RadiusX[1], paths_RadiusX[2] , paths_RadiusX[3] );
#endif

#if PG_NB_PATHS == 7
	// pen paths positions
	glUniform4f(uniform_ParticleAnimation_fs_4fv_paths47_x, paths_x[4], paths_x[5], paths_x[6], paths_x[7]);
	// printf("path 4-7 X %.f %.f %.f %.f\n", paths_x[4], paths_x[5], paths_x[6], paths_x[7]);
	glUniform4f(uniform_ParticleAnimation_fs_4fv_paths47_y, paths_y[4], paths_y[5], paths_y[6], paths_y[7]);
	glUniform4f(uniform_ParticleAnimation_fs_4fv_paths47_x_prev,
		paths_x_prev[4], paths_x_prev[5], paths_x_prev[6], paths_x_prev[7]);
	glUniform4f(uniform_ParticleAnimation_fs_4fv_paths47_y_prev,
		paths_y_prev[4], paths_y_prev[5], paths_y_prev[6], paths_y_prev[7]);

	// pen paths brush & size
	glUniform4f(uniform_ParticleAnimation_fs_4fv_paths47_RadiusX,
		paths_RadiusX[4], paths_RadiusX[5], paths_RadiusX[6], paths_RadiusX[7]);
	// printf("Track radius x %.2f %.2f %.2f %.2f\n" , paths_RadiusX[4], paths_RadiusX[5], paths_RadiusX[6] , paths_RadiusX[7] );
#endif

	// particle decay
	glUniform1f(uniform_ParticleAnimation_fs_1fv_partDecay, partDecay_sign*partDecay*0.1f);


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
		repop_Color_r, repop_Color_g, repop_Color_b, (GLfloat)pg_FrameNo);
	// clear layer, flash pixel, flash CA -> Part
	glUniform4f(uniform_ParticleAnimation_fs_4fv_flashCAPartWght_nbPart_clear_partRadius,
		(GLfloat)flashCAPart_weight, (GLfloat)nb_particles, (GLfloat)isClearAllLayers,
		(part_radius + pulse_average * part_radius_pulse * part_radius));

	// repop values 
	glUniform4f(uniform_ParticleAnimation_fs_4fv_repop_part_path_acc_damp_factor,
		min(.9f, repop_part + pulse_average * repop_part_pulse),
		min(.9f, repop_path + pulse_average * repop_part_pulse),
		part_acc + pulse_average * part_acc_pulse, part_damp_factor);
	//printf("repop part path %.2f %.2f\n",
	//	min(.9f, repop_part + pulse_average * repop_part_pulse),
	//	min(.9f, repop_path + pulse_average * repop_part_pulse));

	// movie size, flash camera and copy tracks
	// copy to layer above (+1) or to layer below (-1)
	glUniform4f(uniform_ParticleAnimation_fs_4fv_Camera_W_H_movieWH,
		(GLfloat)pg_camera_frame_width, (GLfloat)pg_camera_frame_height,
		(GLfloat)pg_movie_frame_width, (GLfloat)pg_movie_frame_height);

	printOglError(511);

	/////////////////////////////////////////////////////////////////////////
	// UPDATE SHADER UNIFORM VARIABLES
	glUseProgram(shader_programme[pg_shader_Update]);

	// time is only used in TVW
	glUniform4f(uniform_Update_fs_4fv_W_H_time_currentScene,
		(GLfloat)leftWindowWidth, (GLfloat)window_height, (GLfloat)CurrentClockTime, (GLfloat)pg_CurrentScene);
	// printf("time %.2f\n", (GLfloat)CurrentClockTime);

	// pixels acceleration
	glUniform4f(uniform_Update_fs_4fv_clearAllLayers_clearCA_pixelRadius_pulsedShift,
		(GLfloat)isClearAllLayers, (GLfloat)isClearCA,
		pixel_radius + pulse_average * pixel_radius_pulse,
		fabs(pulse_average - pulse_average_prec) * part_Vshift_pulse);

	//if (isClearAllLayers > 0) {
	//	printf("-> clear all layers\n");
	//}
	// printf("pixel_radius_pulse %.2f vol %.2f\n", pixel_radius_pulse, pulse_average * pixel_radius_pulse);
	// printf("is clear layer %.2f\n", (GLfloat)isClearLayer);

#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7
	// pen paths positions
	glUniform4f(uniform_Update_fs_4fv_paths03_x, paths_x[0], paths_x[1], paths_x[2], paths_x[3]);
	glUniform4f(uniform_Update_fs_4fv_paths03_y, paths_y[0], paths_y[1], paths_y[2], paths_y[3]);
	glUniform4f(uniform_Update_fs_4fv_paths03_x_prev,
		paths_x_prev[0], paths_x_prev[1], paths_x_prev[2], paths_x_prev[3]);
	glUniform4f(uniform_Update_fs_4fv_paths03_y_prev,
		paths_y_prev[0], paths_y_prev[1], paths_y_prev[2], paths_y_prev[3]);

	// pen paths color
	glUniform4f(uniform_Update_fs_4fv_paths03_r,
		paths_Color_r[0], paths_Color_r[1], paths_Color_r[2], paths_Color_r[3]);
	glUniform4f(uniform_Update_fs_4fv_paths03_g,
		paths_Color_g[0], paths_Color_g[1], paths_Color_g[2], paths_Color_g[3]);
	glUniform4f(uniform_Update_fs_4fv_paths03_b,
		paths_Color_b[0], paths_Color_b[1], paths_Color_b[2], paths_Color_b[3]);
	glUniform4f(uniform_Update_fs_4fv_paths03_a,
		paths_Color_a[0], paths_Color_a[1], paths_Color_a[2], paths_Color_a[3]);
	//printf("Track#0 color %.2f %.2f %.2f %.2f\n", paths_Color_r[0], paths_Color_g[0], paths_Color_b[0], paths_Color_a[0]);
	//printf("Track#1 color %.2f %.2f %.2f %.2f\n", paths_Color_r[1], paths_Color_g[1], paths_Color_b[1], paths_Color_a[1]);
	//printf("Track#2 color %.2f %.2f %.2f %.2f\n", paths_Color_r[2], paths_Color_g[2], paths_Color_b[2], paths_Color_a[2]);
	//printf("Track#3 color %.2f %.2f %.2f %.2f\n", paths_Color_r[3], paths_Color_g[3], paths_Color_b[3], paths_Color_a[3]);

	// pen paths brush & size
	glUniform4f(uniform_Update_fs_4fv_paths03_BrushID,
		(GLfloat)paths_BrushID[0], (GLfloat)paths_BrushID[1], (GLfloat)paths_BrushID[2], (GLfloat)paths_BrushID[3]);
	// printf("BrushID %.2f %.2f %.2f %.2f\n" , paths_BrushID[0] , paths_BrushID[1] , paths_BrushID[2] , paths_BrushID[3] );
	glUniform4f(uniform_Update_fs_4fv_paths03_RadiusX,
		paths_RadiusX[0], paths_RadiusX[1], paths_RadiusX[2], paths_RadiusX[3]);
	glUniform4f(uniform_Update_fs_4fv_paths03_RadiusY,
		paths_RadiusY[0], paths_RadiusY[1], paths_RadiusY[2], paths_RadiusY[3]);
	// printf("Track radius x %.2f %.2f %.2f %.2f\n" , paths_RadiusX[0], paths_RadiusX[1], paths_RadiusX[2] , paths_RadiusX[3] );
#endif

#if PG_NB_PATHS == 7
	// pen paths positions
	glUniform4f(uniform_Update_fs_4fv_paths47_x, paths_x[4], paths_x[5], paths_x[6], paths_x[7]);
	// printf("path 4-7 X %.f %.f %.f %.f\n", paths_x[4], paths_x[5], paths_x[6], paths_x[7]);
	glUniform4f(uniform_Update_fs_4fv_paths47_y, paths_y[4], paths_y[5], paths_y[6], paths_y[7]);
	glUniform4f(uniform_Update_fs_4fv_paths47_x_prev,
		paths_x_prev[4], paths_x_prev[5], paths_x_prev[6], paths_x_prev[7]);
	glUniform4f(uniform_Update_fs_4fv_paths47_y_prev,
		paths_y_prev[4], paths_y_prev[5], paths_y_prev[6], paths_y_prev[7]);

	// pen paths color
	glUniform4f(uniform_Update_fs_4fv_paths47_r,
		paths_Color_r[4], paths_Color_r[5], paths_Color_r[6], paths_Color_r[7]);
	glUniform4f(uniform_Update_fs_4fv_paths47_g,
		paths_Color_g[4], paths_Color_g[5], paths_Color_g[6], paths_Color_g[7]);
	glUniform4f(uniform_Update_fs_4fv_paths47_b,
		paths_Color_b[4], paths_Color_b[5], paths_Color_b[6], paths_Color_b[7]);
	glUniform4f(uniform_Update_fs_4fv_paths47_a,
		paths_Color_a[4], paths_Color_a[5], paths_Color_a[6], paths_Color_a[7]);

	// pen paths brush & size
	glUniform4f(uniform_Update_fs_4fv_paths47_BrushID,
		(GLfloat)paths_BrushID[4], (GLfloat)paths_BrushID[5], (GLfloat)paths_BrushID[6], (GLfloat)paths_BrushID[7]);
	// printf("BrushID %.2f %.2f %.2f %.2f\n" , paths_BrushID[4] , paths_BrushID[5] , paths_BrushID[6] , paths_BrushID[7] );
	glUniform4f(uniform_Update_fs_4fv_paths47_RadiusX,
		paths_RadiusX[4], paths_RadiusX[5], paths_RadiusX[6], paths_RadiusX[7]);
	glUniform4f(uniform_Update_fs_4fv_paths47_RadiusY,
		paths_RadiusY[4], paths_RadiusY[5], paths_RadiusY[6], paths_RadiusY[7]);
	// printf("Track radius x %.2f %.2f %.2f %.2f\n" , paths_RadiusX[4], paths_RadiusX[5], paths_RadiusX[6] , paths_RadiusX[7] );
#endif

#if PG_NB_TRACKS >= 4
	// flash BG weights
	glUniform4f(uniform_Update_fs_4fv_flashTrkBGWghts_flashPartBGWght,
		flashTrkBG_weights[1], flashTrkBG_weights[2], flashTrkBG_weights[3], flashPartBG_weight);
	// flash Trk -> CA weights
	glUniform4f(uniform_Update_fs_4fv_flashTrkCAWghts,
		flashTrkCA_weights[0], flashTrkCA_weights[1], flashTrkCA_weights[2], flashTrkCA_weights[3]);
	// printf("flashTrkCA_weights %.2f %.2f %.2f %.2f \n", flashTrkCA_weights[0], flashTrkCA_weights[1], flashTrkCA_weights[2], flashTrkCA_weights[3]);

	// track decay
	glUniform4f(uniform_Update_fs_4fv_trkDecay,
		trkDecay_0*trkDecay_sign_0,
		trkDecay_1*trkDecay_sign_1,
		trkDecay_2*trkDecay_sign_2,
		trkDecay_3*trkDecay_sign_3);
#else
#if PG_NB_TRACKS >= 3
	// flash Trk -> BG weights
	glUniform4f(uniform_Update_fs_4fv_flashTrkBGWghts_flashPartBGWght,
		flashTrkBG_weights[1], flashTrkBG_weights[2], 0.f, flashPartBG_weight);
	// flash Trk -> CA weights
	glUniform4f(uniform_Update_fs_4fv_flashTrkCAWghts,
		flashTrkCA_weights[0], flashTrkCA_weights[1], flashTrkCA_weights[2], 0.f);
	// printf("flashTrkCA_weights %.2f %.2f %.2f \n", flashTrkCA_weights[0], flashTrkCA_weights[1], flashTrkCA_weights[2]);

	// track decay
	glUniform4f(uniform_Update_fs_4fv_trkDecay,
		trkDecay_0*trkDecay_sign_0,
		trkDecay_1*trkDecay_sign_1,
		trkDecay_2*trkDecay_sign_2,
		partDecay_sign*partDecay*0.1f);
#else
#if PG_NB_TRACKS >= 2
	// flash Trk -> BG weights
	glUniform4f(uniform_Update_fs_4fv_flashTrkBGWghts_flashPartBGWght,
		flashTrkBG_weights[1], 0.f, 0.f, flashPartBG_weight);
	// flash Trk -> CA weights
	glUniform4f(uniform_Update_fs_4fv_flashTrkCAWghts,
		flashTrkCA_weights[0], flashTrkCA_weights[1], 0.f, 0.f);
	// printf("flashTrkCA_weights %.2f %.2f \n", flashTrkCA_weights[0],  flashTrkCA_weights[1]);

	// track decay
	glUniform4f(uniform_Update_fs_4fv_trkDecay,
		trkDecay_0*trkDecay_sign_0,
		trkDecay_1*trkDecay_sign_1,
		0.f,
		0.f);
#else
#if PG_NB_TRACKS >= 1
	// flash Trk -> BG weights
	glUniform4f(uniform_Update_fs_4fv_flashTrkBGWghts_flashPartBGWght,
		0.f, 0.f, 0.f, flashPartBG_weight);
	// flash Trk -> CA weights
	glUniform4f(uniform_Update_fs_4fv_flashTrkCAWghts,
		flashTrkCA_weights[0], 0.f, 0.f, 0.f);
	// printf("flashTrkCA_weights %.2f %.2f \n", flashTrkCA_weights[0],  flashTrkCA_weights[1]);

	// track decay
	glUniform4f(uniform_Update_fs_4fv_trkDecay,
		trkDecay_0*trkDecay_sign_0,
		0.f,
		0.f,
		0.f);
#endif
#endif
#endif
#endif

	//printf("Signs %d \n", currentDrawingTrack);

	// CA type, frame no, flashback and current track
	glUniform4f(uniform_Update_fs_4fv_CAdecay_frameno_Cursor_flashPartCAWght,
		CAdecay*CAdecay_sign, (GLfloat)pg_FrameNo,
		(GLfloat)CurrentCursorStylusvsRubber, flashPartCA_weight);

	// movie size, flash camera and copy tracks
	// copy to layer above (+1) or to layer below (-1)
	glUniform4f(uniform_Update_fs_4fv_movieWH_flashCameraTrkWght_cpTrack,
		(GLfloat)pg_movie_frame_width, (GLfloat)pg_movie_frame_height,
		flashCameraTrk_weight, (GLfloat)copyToNextTrack);
	// printf("Flash camera coef %.1f\n", flashCameraTrk_weight);
	if (copyToNextTrack != 0)
		printf("copyToNextTrack %d\n", copyToNextTrack);

	// flash CA -> BG & repop color (BG & CA)
	glUniform4f(uniform_Update_fs_4fv_repop_Color_flashCABGWght,
		repop_Color_r, repop_Color_g, repop_Color_b, flashCABG_weight);
	// clear layer, flash pixel, flash CA -> Part
	glUniform3f(uniform_Update_fs_3fv_isClearLayer_flashPixel_flashCameraTrkThres,
		(GLfloat)isClearLayer, (GLfloat)flashPixel, flashCameraTrk_threshold);
	// photo size 
#if defined (TVW) || defined (CRITON)
	glUniform4f(uniform_Update_fs_4fv_photo01_wh,
		(GLfloat)pg_Photo_buffer_dataTVW[0]->w, (GLfloat)pg_Photo_buffer_dataTVW[0]->h,
		(GLfloat)pg_Photo_buffer_dataTVW[1]->w, (GLfloat)pg_Photo_buffer_dataTVW[1]->h);
#else
	glUniform4f(uniform_Update_fs_4fv_photo01_wh,
		(GLfloat)pg_Photo_buffer_data[0].w, (GLfloat)pg_Photo_buffer_data[0].h,
		(GLfloat)pg_Photo_buffer_data[1].w, (GLfloat)pg_Photo_buffer_data[1].h);
#endif

	// photo weights 
	glUniform4f(uniform_Update_fs_4fv_photo01Wghts_Camera_W_H,
		pg_Photo_weight[0] * photoWeight, pg_Photo_weight[1] * photoWeight,
		(GLfloat)pg_camera_frame_width, (GLfloat)pg_camera_frame_height);
	//printf("photo weight %.2f %.2f\n",
	//	pg_Photo_weight[0] * photoWeight, pg_Photo_weight[1] * photoWeight);
	//printf("camera movie weight %.2f %.2f\n",
	//	*((float *)ScenarioVarPointers[_cameraWeight]), *((float *)ScenarioVarPointers[_movieWeight]));

#if defined (TVW) || defined (CRITON)
	// image buffer layer weights
	centralPhoto += photoJitterAmpl * fabs(float((double)rand() / (double)RAND_MAX - 0.5));
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
		float imageCoordOffest = photoJitterAmpl * (float)((double)rand() / (double)RAND_MAX - 0.5);
		float maskCoordOffest = maskJitterAmpl * float((double)rand() / (double)RAND_MAX - 0.5);
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
	//printf("noises %.1f %.1f %.1f %.1f\n", pg_Photo_mask_position_noises[0], pg_Photo_mask_position_noises[1],
	// pg_Photo_mask_position_noises[2], pg_Photo_mask_position_noises[3]);
	//printf("noises %.1f %.1f %.1f %.1f\n", pg_Photo_mask_position_noises[4], pg_Photo_mask_position_noises[5],
	// pg_Photo_mask_position_noises[6], pg_Photo_mask_position_noises[7]);
	//printf("noises %.1f %.1f %.1f %.1f\n\n", pg_Photo_mask_position_noises[8], pg_Photo_mask_position_noises[9],
	// pg_Photo_mask_position_noises[10], pg_Photo_mask_position_noises[11]);
	glUniform4fv(uniform_Update_fs_4fv_image_noisesxy, 3, pg_Photo_position_noises);
	glUniform4fv(uniform_Update_fs_4fv_mask_noisesxy, 3, pg_Photo_mask_position_noises);
#endif

	// music pulse
	glUniform4f(uniform_Update_fs_4fv_pulse, pulse[0], pulse[1], pulse[2],
		pulse_average);

	// track x & y translations
	float translation_x[2] = { 0.f, 0.f };
	float translation_y[2] = { 0.f, 0.f };
	if (track_x_transl_0 && pg_FrameNo % abs(track_x_transl_0) == 0) {
		translation_x[0] = (track_x_transl_0 > 0 ? 1.f : -1.f);
	}
	if (track_y_transl_0 && pg_FrameNo % abs(track_y_transl_0) == 0) {
		translation_y[0] = (track_y_transl_0 > 0 ? 1.f : -1.f);
	}
	if (track_x_transl_1 && pg_FrameNo % abs(track_x_transl_1) == 0) {
		translation_x[1] = (track_x_transl_1 > 0 ? 1.f : -1.f);
	}
	if (track_y_transl_1 && pg_FrameNo % abs(track_y_transl_1) == 0) {
		translation_y[1] = (track_y_transl_1 > 0 ? 1.f : -1.f);
	}
	glUniform4f(uniform_Update_fs_4fv_xy_transl_tracks_0_1,
		translation_x[0],
		translation_y[0],
		translation_x[1],
		translation_y[1]);

	// acceleration center and CA subtype
	// in case of interpolation between CA1 and CA2 
	if (!BrokenInterpolationVar[_CA1_CA2_weight]) {
		if (CA1_CA2_weight < 1.0 && CA1_CA2_weight > 0.0) {
			float randVal = (float)rand() / (float)RAND_MAX;
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
	glUniform4f(uniform_Update_fs_4fv_CAType_SubType_blurRadius,
		GLfloat(CAInterpolatedType),GLfloat(CAInterpolatedSubType),
		(is_blur_1 ? float(blurRadius_1) : 0.f), (is_blur_2 ? float(blurRadius_2) : 0.f));
	// printf("CA type/subtype %d-%d\n" , CAInterpolatedType, CAInterpolatedSubType);
	// printf("blur %.2f %.2f\n", (is_blur_1 ? float(blurRadius_1) : 0.f), (is_blur_2 ? float(blurRadius_2) : 0.f));

#ifdef GN
	// sets the time of the 1st plane launch 
	if (CAInterpolatedType == CA_NEUMANN_BINARY && firstPlaneFrameNo < 0) {
		// printf("frame no %d\n", firstPlaneFrameNo);
		firstPlaneFrameNo = pg_FrameNo - 800;
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
		pg_send_message_udp((char *)"i", AuxString, (char *)"udp_QT_send");
		sprintf(AuxString, "/CASubType %d", CAInterpolatedSubType); 
		pg_send_message_udp((char *)"i", AuxString, (char *)"udp_QT_send");
		CAInterpolatedType_prev = CAInterpolatedType;
		CAInterpolatedSubType_prev = CAInterpolatedSubType;
	}


	printOglError(519);

	/////////////////////////////////////////////////////////////////////////
	// PARTICLE RENDERING SHADER UNIFORM VARIABLES

#if defined (BLURRED_SPLAT_PARTICLES) || defined (LINE_SPLAT_PARTICLES) 
	glUseProgram(shader_programme[pg_shader_ParticleRender]);
	glUniform3f(uniform_ParticleSplat_gs_3fv_partRadius_partType_highPitchPulse,
		(part_radius + pulse_average * part_radius_pulse * part_radius) / 512.f,
		(GLfloat)particle_type,
		pulse[2]);

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
		(part_radius + pulse_average * part_radius_pulse * part_radius) / 512.f,
		(GLfloat)particle_type,
		pulse[2]);

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
#if defined (TVW) || defined (CRITON)
	// special case for army explosion: track 1 is assigned as repulse or follow path but is not replayed
	// the center of the screen is the default position for this track
	if (part_path_repulse_1 || part_path_follow_1) {
		glUniform2f(uniform_ParticleSplat_vp_3fv_trackReplay_xy_height, float(leftWindowWidth/2), float(window_height/2));
	}
	// special case for army radar: track 2 is assigned as repop path but is not replayed
	// the center of the top left screen is the default position for this track
	else if (path_repop_2 || part_path_repulse_2) {
		glUniform2f(uniform_ParticleSplat_vp_3fv_trackReplay_xy_height, float(leftWindowWidth / 4), float(window_height / 4));
	}
#endif

	//printf("pulsed particle radius %.4f rad %.4f pulse %.4f coef %.4f\n", 
	//	(part_radius + pulse_average * part_radius_pulse * part_radius) / 512.f,
	//	part_radius / 512.f, pulse_average, part_radius_pulse);

	/////////////////////////////////////////////////////////////////////////
	// MIXING SHADER UNIFORM VARIABLES

	glUseProgram(shader_programme[pg_shader_Mixing]);

	// CA weight
	glUniform3f(uniform_Mixing_fs_3fv_pulsedShift_height_flashCameraTrkWght,
		(pulse_average - pulse_average_prec) * echo_Hshift_pulse, (GLfloat)window_height,
		 flashCameraTrk_weight);

	// TEXT TRANSPARENCY
#if defined (TVW) || defined (CRITON)
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
	glUniform4f(uniform_Master_fs_4fv_xy_frameno_pulsedShift,
		(GLfloat)CurrentCursorHooverPos_x, (GLfloat)CurrentCursorHooverPos_y,
		(GLfloat)pg_FrameNo, (pulse_average - pulse_average_prec) * tracks_Hshift_pulse);
	// screen size
	glUniform3f(uniform_Master_fs_3fv_width_height_rightWindowVMargin,
		(GLfloat)leftWindowWidth, (GLfloat)window_height, GLfloat(rightWindowVMargin));
	// printf("hoover %d %d\n",CurrentCursorHooverPos_x, CurrentCursorHooverPos_y);

#ifdef PG_SENSORS
	/////////////////////////////////////////////////////////////////////////
	// SENSOR SHADER UNIFORM VARIABLES
	glUseProgram(shader_programme[pg_shader_Sensor]);
	//glUniform2f(uniform_Sensor_fs_2fv_frameno_invert, 
	//         (GLfloat)FrameNo, (invertAllLayers ? 1.0f : -1.0f) );
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

void pg_ParticleAnimationPass(void) {
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, pg_FBO_ParticleAnimation[((pg_FrameNo + 1) % 2)]);

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	////////////////////////////////////////
	// activate shaders and sets uniform variable values    
	glUseProgram(shader_programme[pg_shader_ParticleAnimation]);

	glBindVertexArray(pg_vaoID[pg_VAOQuad]);
	
	glUniformMatrix4fv(uniform_ParticleAnimation_vp_proj, 1, GL_FALSE, projMatrix);
	glUniformMatrix4fv(uniform_ParticleAnimation_vp_view, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(uniform_ParticleAnimation_vp_model, 1, GL_FALSE, modelMatrix);

	// texture unit location
	glUniform1i(uniform_ParticleAnimation_texture_fs_Part_init_pos_speed, pg_Part_init_pos_speed_FBO_ParticleAnimation_sampler);
	glUniform1i(uniform_ParticleAnimation_texture_fs_Part_init_col_rad, pg_Part_init_col_rad_FBO_ParticleAnimation_sampler);
	glUniform1i(uniform_ParticleAnimation_texture_fs_CA, pg_CA_FBO_ParticleAnimation_sampler);
	glUniform1i(uniform_ParticleAnimation_texture_fs_Part_pos_speed, pg_Part_pos_speed_FBO_ParticleAnimation_sampler);
	glUniform1i(uniform_ParticleAnimation_texture_fs_Part_col_rad, pg_Part_col_rad_FBO_ParticleAnimation_sampler);
	glUniform1i(uniform_ParticleAnimation_texture_fs_Part_Target_pos_col_rad, pg_Part_Target_pos_col_rad_FBO_ParticleAnimation_sampler);
	glUniform1i(uniform_ParticleAnimation_texture_fs_Noise, pg_Noise_FBO_ParticleAnimation_sampler);
	glUniform1i(uniform_ParticleAnimation_texture_fs_Camera_frame, pg_Camera_frame_FBO_ParticleAnimation_sampler);
	glUniform1i(uniform_ParticleAnimation_texture_fs_Movie_frame, pg_Movie_frame_FBO_ParticleAnimation_sampler);
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
	glActiveTexture(GL_TEXTURE0 + pg_Part_init_pos_speed_FBO_ParticleAnimation_sampler);
	// TMP
	// glBindTexture(GL_TEXTURE_RECTANGLE, pg_particle_initial_images_texID[0][0]); // pos - speed 
	// TMP
	if (part_initialization >= 0 && part_initialization < PG_NB_PARTICLE_INITIAL_IMAGES) {
		glBindTexture(GL_TEXTURE_RECTANGLE, pg_particle_initial_images_texID[part_initialization][0]); // pos - speed 
		printf("particle initialization %d\n", part_initialization);
	}
	// photo or video particles, position and speed are chosen from a random texture between 
	// 0 & PG_NB_PARTICLE_INITIAL_IMAGES - 1
	else if (part_initialization >= PG_NB_PARTICLE_INITIAL_IMAGES
		&& part_initialization < PG_NB_PARTICLE_INITIAL_IMAGES + 2) {
		int indTex = int(floor(randomValue * PG_NB_PARTICLE_INITIAL_IMAGES)) % PG_NB_PARTICLE_INITIAL_IMAGES;
		glBindTexture(GL_TEXTURE_RECTANGLE, pg_particle_initial_images_texID[indTex][0]); // pos - speed 
		printf("particle initialization %d texture %d\n", part_initialization, indTex);
	}

	glActiveTexture(GL_TEXTURE0 + pg_Part_init_col_rad_FBO_ParticleAnimation_sampler);
	// TMP
	// glBindTexture(GL_TEXTURE_RECTANGLE, pg_particle_initial_images_texID[0][1]); // pos - speed 
	// TMP
	// color and radius are taken from photo or video and partic radius
	if (part_initialization >= 0 && part_initialization < PG_NB_PARTICLE_INITIAL_IMAGES) {
		glBindTexture(GL_TEXTURE_RECTANGLE, pg_particle_initial_images_texID[part_initialization][1]); // color RGB - rad 
	}

	// 2-cycle ping-pong CA step n step n (FBO attachment 2)
	glActiveTexture(GL_TEXTURE0 + pg_CA_FBO_ParticleAnimation_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[(pg_FrameNo % 2) * PG_FBO_UPDATE_NBATTACHTS + pg_CA_FBO_Update_attcht]);

	// 2-cycle ping-pong position/speed of particles step n step n (FBO attachment 2)
	glActiveTexture(GL_TEXTURE0 + pg_Part_pos_speed_FBO_ParticleAnimation_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_ParticleAnimation_texID[(pg_FrameNo % 2) * PG_FBO_PARTICLEANIMATION_NBATTACHTS + pg_Part_pos_speed_FBO_ParticleAnimation_attcht]);

	// 2-cycle ping-pong color/radius of particles step n step n (FBO attachment 3)
	glActiveTexture(GL_TEXTURE0 + pg_Part_col_rad_FBO_ParticleAnimation_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_ParticleAnimation_texID[(pg_FrameNo % 2) * PG_FBO_PARTICLEANIMATION_NBATTACHTS + pg_Part_col_rad_FBO_ParticleAnimation_attcht]);

	// 2-cycle ping-pong target position/color/radius of particles step n step n (FBO attachment 4)
	glActiveTexture(GL_TEXTURE0 + pg_Part_Target_pos_col_rad_FBO_ParticleAnimation_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_ParticleAnimation_texID[(pg_FrameNo % 2) * PG_FBO_PARTICLEANIMATION_NBATTACHTS + pg_Part_Target_pos_col_rad_FBO_ParticleAnimation_attcht]);

	// noise texture (noise is also generated procedurally in the update shader)
	glActiveTexture(GL_TEXTURE0 + pg_Noise_FBO_ParticleAnimation_sampler);
	glBindTexture(GL_TEXTURE_3D, Noise_texture_3D);

	// camera texture produced at preceding pass
	// glActiveTexture(GL_TEXTURE0 + 7);
	// glBindTexture(GL_TEXTURE_RECTANGLE, FBO_CameraFrame_texID);
	// current camera texture
	glActiveTexture(GL_TEXTURE0 + pg_Camera_frame_FBO_ParticleAnimation_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_camera_texture_texID);

	// movie texture
	glActiveTexture(GL_TEXTURE0 + pg_Movie_frame_FBO_ParticleAnimation_sampler);
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

//////////////////////////////////////////////////
// PASS #1: UPDATE (CA, PIXELS, PARTICLES, DRAWING, PHOTOS & VIDEOS)

void pg_UpdatePass(void) {
	// ping pong output and input FBO bindings
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, pg_FBO_Update[((pg_FrameNo + 1) % 2)]);

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	////////////////////////////////////////
	// activate shaders and sets uniform variable values    
	glUseProgram(shader_programme[pg_shader_Update]);
	glBindVertexArray(pg_vaoID[pg_VAOQuad]);
	glUniformMatrix4fv(uniform_Update_vp_proj, 1, GL_FALSE, projMatrix);
	glUniformMatrix4fv(uniform_Update_vp_view, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(uniform_Update_vp_model, 1, GL_FALSE, modelMatrix);

	// texture unit location
	glUniform1i(uniform_Update_texture_fs_CA, pg_CA_FBO_Update_sampler);
	glUniform1i(uniform_Update_texture_fs_Pixels, pg_Pixels_FBO_Update_sampler);
	glUniform1i(uniform_Update_texture_fs_Brushes, pg_Brushes_FBO_Update_sampler);
#ifdef PG_WITH_CAMERA_CAPTURE
	glUniform1i(uniform_Update_texture_fs_Camera_frame, pg_Camera_frame_FBO_Update_sampler);
	glUniform1i(uniform_Update_texture_fs_Camera_BG, pg_Camera_BG_FBO_Update_sampler);
#endif
	glUniform1i(uniform_Update_texture_fs_Movie_frame, pg_Movie_frame_FBO_Update_sampler);
	glUniform1i(uniform_Update_texture_fs_Noise, pg_Noise_FBO_Update_sampler);
#if !defined (TVW) && !defined (CRITON)
	glUniform1i(uniform_Update_texture_fs_Photo0, pg_Photo0_FBO_Update_sampler);
	glUniform1i(uniform_Update_texture_fs_Photo1, pg_Photo1_FBO_Update_sampler);
#endif
	glUniform1i(uniform_Update_texture_fs_Part_render, pg_Part_render_FBO_Update_sampler);
#if defined (TVW) || defined (CRITON)
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
#if defined (GN) || defined (MALAUSSENA)
	glUniform1i(uniform_Update_texture_fs_CATable, pg_CATable_Update_sampler);
#endif
#ifdef GN
	glUniform1i(uniform_Update_texture_fs_Camera_BGIni, pg_Camera_BGIni_FBO_Update_sampler);
#endif
	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP);

	// 2-cycle ping-pong CA step n (FBO attachment 0)
	glActiveTexture(GL_TEXTURE0 + pg_CA_FBO_Update_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[(pg_FrameNo % 2) * PG_FBO_UPDATE_NBATTACHTS + pg_CA_FBO_Update_attcht]);

	// 2-cycle ping-pong speed/position of pixels step n step n (FBO attachment 1)
	glActiveTexture(GL_TEXTURE0 + pg_Pixels_FBO_Update_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[(pg_FrameNo % 2) * PG_FBO_UPDATE_NBATTACHTS + pg_Pixels_FBO_Update_attcht]);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// pen patterns
	glActiveTexture(GL_TEXTURE0 + pg_Brushes_FBO_Update_sampler);
	glBindTexture(GL_TEXTURE_3D, Pen_texture_3D_texID);

	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP);

	// camera texture produced at preceding pass
	// glActiveTexture(GL_TEXTURE0 + 7);
	// glBindTexture(GL_TEXTURE_RECTANGLE, FBO_CameraFrame_texID);
	// current camera texture
#ifdef PG_WITH_CAMERA_CAPTURE
	glActiveTexture(GL_TEXTURE0 + pg_Camera_frame_FBO_Update_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_camera_texture_texID);

	// current background texture
	glActiveTexture(GL_TEXTURE0 + pg_Camera_BG_FBO_Update_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_camera_BG_texture_texID);
#endif

	// movie texture
	glActiveTexture(GL_TEXTURE0 + pg_Movie_frame_FBO_Update_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_movie_texture_texID);

	// noise texture (noise is also generated procedurally in the update shader)
	glActiveTexture(GL_TEXTURE0 + pg_Noise_FBO_Update_sampler);
	glBindTexture(GL_TEXTURE_3D, Noise_texture_3D);

#if !defined (TVW) && !defined (CRITON)
	// photo[0] texture
	glActiveTexture(GL_TEXTURE0 + pg_Photo0_FBO_Update_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_Photo_buffer_data[0].texBuffID);

	// photo[1] texture
	glActiveTexture(GL_TEXTURE0 + pg_Photo1_FBO_Update_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_Photo_buffer_data[1].texBuffID);
#endif

	// FBO capture of particle rendering used for flashing layers with particles
	glActiveTexture(GL_TEXTURE0 + pg_Part_render_FBO_Update_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_ParticleRendering_texID);

#if defined (TVW) || defined (CRITON)
	glActiveTexture(GL_TEXTURE0 + pg_TVWPixels0);
	glBindTexture(GL_TEXTURE_2D, 
		pg_Photo_buffer_dataTVW[pg_Photo_swap_buffer_data[0].indOldPhoto]->texBuffID);
	// image buffer texture #1
	glActiveTexture(GL_TEXTURE0 + pg_TVWPixels1);
	glBindTexture(GL_TEXTURE_2D,
		pg_Photo_buffer_dataTVW[pg_Photo_swap_buffer_data[1].indOldPhoto]->texBuffID);
	// image buffer texture #2
	glActiveTexture(GL_TEXTURE0 + pg_TVWPixels2);
	glBindTexture(GL_TEXTURE_2D,
		pg_Photo_buffer_dataTVW[pg_Photo_swap_buffer_data[2].indOldPhoto]->texBuffID);
	// image buffer texture #3
	glActiveTexture(GL_TEXTURE0 + pg_TVWPixels3);
	glBindTexture(GL_TEXTURE_2D,
		pg_Photo_buffer_dataTVW[pg_Photo_swap_buffer_data[3].indOldPhoto]->texBuffID);
	// image buffer texture #4
	glActiveTexture(GL_TEXTURE0 + pg_TVWPixels4);
	glBindTexture(GL_TEXTURE_2D,
		pg_Photo_buffer_dataTVW[pg_Photo_swap_buffer_data[4].indOldPhoto]->texBuffID);
	// image buffer texture #5
	glActiveTexture(GL_TEXTURE0 + pg_TVWPixels5);
	glBindTexture(GL_TEXTURE_2D,
		pg_Photo_buffer_dataTVW[pg_Photo_swap_buffer_data[5].indOldPhoto]->texBuffID);
	// image buffer mask #0
	glActiveTexture(GL_TEXTURE0 + pg_TVWMask02);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_Photo_mask_buffer_data[0].texBuffID);
	// image buffer mask #1
	glActiveTexture(GL_TEXTURE0 + pg_TVWMask35);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_Photo_mask_buffer_data[1].texBuffID);
	// buffer swap image #0
	glActiveTexture(GL_TEXTURE0 + pg_TVWPixelsSwap0);
	glBindTexture(GL_TEXTURE_2D, 
		pg_Photo_buffer_dataTVW[pg_Photo_swap_buffer_data[0].indSwappedPhoto]->texBuffID);
	// buffer swap image #1
	glActiveTexture(GL_TEXTURE0 + pg_TVWPixelsSwap1);
	glBindTexture(GL_TEXTURE_2D,
		pg_Photo_buffer_dataTVW[pg_Photo_swap_buffer_data[1].indSwappedPhoto]->texBuffID);
	// buffer swap image #2
	glActiveTexture(GL_TEXTURE0 + pg_TVWPixelsSwap2);
	glBindTexture(GL_TEXTURE_2D,
		pg_Photo_buffer_dataTVW[pg_Photo_swap_buffer_data[2].indSwappedPhoto]->texBuffID);
	// buffer swap image #3
	glActiveTexture(GL_TEXTURE0 + pg_TVWPixelsSwap3);
	glBindTexture(GL_TEXTURE_2D,
		pg_Photo_buffer_dataTVW[pg_Photo_swap_buffer_data[3].indSwappedPhoto]->texBuffID);
	// buffer swap image #4
	glActiveTexture(GL_TEXTURE0 + pg_TVWPixelsSwap4);
	glBindTexture(GL_TEXTURE_2D,
		pg_Photo_buffer_dataTVW[pg_Photo_swap_buffer_data[4].indSwappedPhoto]->texBuffID);
	// buffer swap image #5
	glActiveTexture(GL_TEXTURE0 + pg_TVWPixelsSwap5);
	glBindTexture(GL_TEXTURE_2D,
		pg_Photo_buffer_dataTVW[pg_Photo_swap_buffer_data[5].indSwappedPhoto]->texBuffID);
#endif

	//printf("texID %02d/%02d %02d/%02d %02d/%02d %02d/%02d %02d/%02d %02d/%02d\n",
		//pg_Photo_buffer_dataTVW[pg_Photo_swap_buffer_data[0].indOldPhoto]->texBuffID,
		//pg_Photo_buffer_dataTVW[pg_Photo_swap_buffer_data[0].indSwappedPhoto]->texBuffID,
		//pg_Photo_buffer_dataTVW[pg_Photo_swap_buffer_data[1].indOldPhoto]->texBuffID,
		//pg_Photo_buffer_dataTVW[pg_Photo_swap_buffer_data[1].indSwappedPhoto]->texBuffID,
		//pg_Photo_buffer_dataTVW[pg_Photo_swap_buffer_data[2].indOldPhoto]->texBuffID,
		//pg_Photo_buffer_dataTVW[pg_Photo_swap_buffer_data[2].indSwappedPhoto]->texBuffID,
		//pg_Photo_buffer_dataTVW[pg_Photo_swap_buffer_data[3].indOldPhoto]->texBuffID,
		//pg_Photo_buffer_dataTVW[pg_Photo_swap_buffer_data[3].indSwappedPhoto]->texBuffID,
		//pg_Photo_buffer_dataTVW[pg_Photo_swap_buffer_data[4].indOldPhoto]->texBuffID,
		//pg_Photo_buffer_dataTVW[pg_Photo_swap_buffer_data[4].indSwappedPhoto]->texBuffID,
		//pg_Photo_buffer_dataTVW[pg_Photo_swap_buffer_data[5].indOldPhoto]->texBuffID,
		//pg_Photo_buffer_dataTVW[pg_Photo_swap_buffer_data[5].indSwappedPhoto]->texBuffID);

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

#if defined (GN) || defined (MALAUSSENA)
	// CA Data table (FBO attachment 11)
	glActiveTexture(GL_TEXTURE0 + pg_CATable_Update_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_CATable_ID);
#endif 

#ifdef GN
	// Initial background texture
	glActiveTexture(GL_TEXTURE0 + pg_Camera_BGIni_FBO_Update_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_camera_BGIni_texture_texID);
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
// PASS #2: PARTICLE RENDERING PASS

void pg_ParticleRenderingPass( void ) {
	// printf("nb paticles %d\^n", nb_particles);
	// draws the Bezier curve
	if (pg_FrameNo > 0) {
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, pg_FBO_ParticleRendering);
	}
	//glDisable(GL_BLEND);
	//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	////////////////////////////////////////
	// activate shaders and sets uniform variable values    
#if defined (BLURRED_SPLAT_PARTICLES) || defined (LINE_SPLAT_PARTICLES) 
	glUseProgram(shader_programme[pg_shader_ParticleRender]);

	glUniformMatrix4fv(uniform_ParticleSplat_vp_proj, 1, GL_FALSE, projMatrix);
	glUniformMatrix4fv(uniform_ParticleSplat_vp_view, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(uniform_ParticleSplat_vp_model, 1, GL_FALSE, modelMatrix);

	// texture unit location
	// position/speed Particle update
	glUniform1i(uniform_ParticleSplat_texture_vp_Part_pos_speed, 0);
	// color/radius Particle update
	glUniform1i(uniform_ParticleSplat_texture_vp_Part_col_rad, 1); 
#endif
#if defined (CURVE_PARTICLES) 
	glUseProgram(shader_programme[pg_shader_ParticleRender]);

	glUniformMatrix4fv(uniform_ParticleCurve_vp_proj, 1, GL_FALSE, projMatrix);
	glUniformMatrix4fv(uniform_ParticleCurve_vp_view, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(uniform_ParticleCurve_vp_model, 1, GL_FALSE, modelMatrix);

	// texture unit location
	// position/speed Particle update
	glUniform1i(uniform_ParticleCurve_texture_vp_Part_pos_speed, 0);
	// color/radius Particle update
	glUniform1i(uniform_ParticleCurve_texture_vp_Part_col_rad, 1); 

	// comet texture
	glUniform1i(uniform_ParticleCurve_Comet_texture_fs_decal, 2);
#endif

#ifdef BLURRED_SPLAT_PARTICLES
	// blurred disk texture
	glUniform1i(uniform_ParticleSplat_BlurredDisk_texture_fs_decal, 2);
#endif

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
#ifdef BLURRED_SPLAT_PARTICLES
	// blurred disk texture
	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_2D, blurredDisk_texture_2D_texID);
#endif

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
#if defined BLURRED_SPLAT_PARTICLES || defined LINE_SPLAT_PARTICLES
	// Draw the patches !
	glDrawElements(
		GL_POINTS,      // mode
		nb_particles,    // count
		GL_UNSIGNED_INT,   // type
		(void*)0           // element array buffer offset
	);
#endif

	printOglError(5256);

	// unbinds VBO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableVertexAttribArray(0);
}

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

	/////////////////////////////////////////////////////////
	// draws the main rectangular surface with 

	////////////////////////////////////////
	// activate shaders and sets uniform variable values    
	glUseProgram(shader_programme[pg_shader_Mixing]);
	glBindVertexArray(pg_vaoID[pg_VAOQuad]);

	glUniformMatrix4fv(uniform_Mixing_vp_proj, 1, GL_FALSE, projMatrix);
	glUniformMatrix4fv(uniform_Mixing_vp_view, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(uniform_Mixing_vp_model, 1, GL_FALSE, modelMatrix);

	// texture unit location
	glUniform1i(uniform_Mixing_texture_fs_CA, pg_CA_FBO_Mixing_sampler);
	glUniform1i(uniform_Mixing_texture_fs_Part_render, pg_Part_render_FBO_Mixing_sampler);
	glUniform1i(uniform_Mixing_texture_fs_Render_prec, pg_Render_prec_FBO_Mixing_sampler);

	glUniform1i(uniform_Mixing_texture_fs_Screen_Font, pg_Screen_Font_FBO_Mixing_sampler);
	glUniform1i(uniform_Mixing_texture_fs_Screen_Message, pg_Screen_Message_FBO_Mixing_sampler);
#if defined (TVW) || defined (CRITON)
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
	// 2-cycle ping-pong CA step n + 1 (FBO attachment 0)
	glActiveTexture(GL_TEXTURE0 + pg_CA_FBO_Mixing_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * PG_FBO_UPDATE_NBATTACHTS + pg_CA_FBO_Update_attcht]);

	// Particles step n
	glActiveTexture(GL_TEXTURE0 + pg_Part_render_FBO_Mixing_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_ParticleRendering_texID);

	// preceding snapshot for echo
	glActiveTexture(GL_TEXTURE0 + pg_Render_prec_FBO_Mixing_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Mixing_capturedFB_prec_texID[((pg_FrameNo + 1) % 2)]);  // drawing memory on odd and even frames for echo 

	// screen font texture
	glActiveTexture(GL_TEXTURE0 + pg_Screen_Font_FBO_Mixing_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, Screen_Font_texture_Rectangle_texID);

	// message offsets texture
	glActiveTexture(GL_TEXTURE0 + pg_Screen_Message_FBO_Mixing_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_screenMessageBitmap_texID);

#if defined (TVW) || defined (CRITON)
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
	// unbind output FBO 
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	// sets viewport to double window
	glViewport(0, 0, doubleWindowWidth, window_height);

	glDrawBuffer(GL_BACK);

	// output video buffer clean-up
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	////////////////////////////////////////
	// activate shaders and sets uniform variable values    
	glUseProgram(shader_programme[pg_shader_Master]);
	glBindVertexArray(pg_vaoID[pg_VAOQuadMaster]);

	glUniformMatrix4fv(uniform_Master_vp_proj, 1, GL_FALSE, doubleProjMatrix);
	glUniformMatrix4fv(uniform_Master_vp_view, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(uniform_Master_vp_model, 1, GL_FALSE, modelMatrix);

	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP);
	// texture unit locations
	glUniform1i(uniform_Master_texture_fs_Render_curr, pg_Render_curr_FBO_Master_sampler);
	glUniform1i(uniform_Master_texture_fs_CA, pg_CA_FBO_Master_sampler);
	glUniform1i(uniform_Master_texture_fs_Part_render, pg_Part_render_FBO_Master_sampler);
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
#ifdef GN
	glUniform1i(uniform_Master_texture_fs_LYMlogo, pg_LYMlogo_Master_sampler);
#endif

	// Mixing pass output (echoed composition of tracks)
	glActiveTexture(GL_TEXTURE0 + pg_Render_curr_FBO_Master_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Mixing_capturedFB_prec_texID[(pg_FrameNo % 2)]);

	// 2-cycle ping-pong CA step n (FBO attachment 0)
	glActiveTexture(GL_TEXTURE0 + pg_CA_FBO_Master_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * PG_FBO_UPDATE_NBATTACHTS + pg_CA_FBO_Update_attcht]);

	// Particles step n 
	glActiveTexture(GL_TEXTURE0 + pg_Part_render_FBO_Master_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_ParticleRendering_texID);

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
#ifdef GN
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

	glUniformMatrix4fv(uniform_Sensor_vp_view, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(uniform_Sensor_vp_proj, 1, GL_FALSE, doubleProjMatrix);
	printOglError(597);

	// sensor rendering
	int Sensor_order[PG_NB_SENSORS] = { 8, 13, 14, 11, 7, 2, 1, 4, 12, 15, 3, 0, 9, 10, 5, 6 };
	for (int indSens = 0; indSens < PG_NB_SENSORS; indSens++) {
		int reindexed_Sensor = Sensor_order[indSens];
		if (sensor_onOff[reindexed_Sensor]) {
			modelMatrixSensor[12] = (sensorPositions[3 * reindexed_Sensor] + 0.5f - leftWindowWidth / 2.0f) + leftWindowWidth / 2.0f;
			modelMatrixSensor[13] = (sensorPositions[3 * reindexed_Sensor + 1] + 0.5f - window_height / 2.0f) + window_height / 2.0f;
			modelMatrixSensor[14] = sensorPositions[3 * reindexed_Sensor + 2];
			glUniformMatrix4fv(uniform_Sensor_vp_model, 1, GL_FALSE, modelMatrixSensor);

			// sensorValues[indSens]
			glUniform4f(uniform_Sensor_fs_4fv_onOff_isCurrentSensor_isFollowMouse_transparency,
				(sensor_onOff[reindexed_Sensor] ? 1.0f : -1.0f), (reindexed_Sensor == currentSensor ? 1.0f : -1.0f),
				(sensorFollowMouse_onOff ? 1.0f : -1.0f), sensorValues[reindexed_Sensor]);

			// draw points from the currently bound VAO with current in-use shader
			glDrawArrays(GL_TRIANGLES, 0, 3 * 2);
		}
		else {
			// incremental sensor activation every 45 sec. = 720/16
			if (sensor_activation == 5 
				&& CurrentClockTime - sensor_last_activation_time > 45) {
				sensor_last_activation_time = CurrentClockTime;
				sensor_onOff[reindexed_Sensor] = true;
			}
		}
	}
	printOglError(596);

	// duplicates the sensors in case of double window
	if (double_window) {
		for (int indSens = 0; indSens < PG_NB_SENSORS; indSens++) {
			int reindexed_Sensor = Sensor_order[indSens];
			if (sensor_onOff[reindexed_Sensor]) {
				modelMatrixSensor[12] = (sensorPositions[3 * reindexed_Sensor] + 0.5f - leftWindowWidth / 2.0f) + 3 * leftWindowWidth / 2.0f + rightWindowVMargin;
				modelMatrixSensor[13] = (sensorPositions[3 * reindexed_Sensor + 1] + 0.5f - window_height / 2.0f) + window_height / 2.0f;
				modelMatrixSensor[14] = sensorPositions[3 * reindexed_Sensor + 2];
				glUniformMatrix4fv(uniform_Sensor_vp_model, 1, GL_FALSE, modelMatrixSensor);

				glUniform4f(uniform_Sensor_fs_4fv_onOff_isCurrentSensor_isFollowMouse_transparency,
					(sensor_onOff[reindexed_Sensor] ? 1.0f : -1.0f), (reindexed_Sensor == currentSensor ? 1.0f : -1.0f),
					(sensorFollowMouse_onOff ? 1.0f : -1.0f), sensorValues[reindexed_Sensor]);

				// draw points from the currently bound VAO with current in-use shader
				glDrawArrays(GL_TRIANGLES, 0, 3 * 2);
			}
		}
	}

	printOglError(595);
	glDisable(GL_BLEND);
}
#endif


//////////////////////////////////////////////////////////////////////////////////////////////
// DRAWING A SCENE ON VARIOUS MODALITIES (CURVE, IMAGE, FRAMEBUFFER...)
//////////////////////////////////////////////////////////////////////////////////////////////
void pg_draw_scene( DrawingMode mode ) {
  // ******************** Svg output ********************
  if( mode == _Svg ) {
    threadData *pData = new threadData;
    pData->fname = new char[512];
    pData->w = leftWindowWidth;
    pData->h = window_height;

	sprintf(pData->fname, "%s%s-%s-%04d.svg",
		snapshots_dir_path_name.c_str(),
		Svg_file_name.c_str(),
		date_stringStream.str().c_str(),
		pg_FrameNo / stepSvg);
	pg_logCurrentLineSceneVariables(pData->fname);

#ifdef WIN32
   DWORD rc;
   HANDLE  hThread = CreateThread(
            NULL,                   // default security attributes
            0,                      // use default stack size  
            writesvg,		    // thread function name
            (void *)pData,		    // argument to thread function 
            0,                      // use default creation flags 
            &rc);   // returns the thread identifier 
   if (hThread == NULL){
         std::cout << "Error:unable to create thread writesvg" << std::endl;
         exit(-1);
   }
   CloseHandle(hThread);
#else
   pthread_t drawing_thread;
   int rc;
   rc = pthread_create(&drawing_thread, NULL, 
                          writesvg, (void *)pData);
   if (rc){
         std::cout << "Error:unable to create thread writesvg" << rc << std::endl;
         exit(-1);
   }
   pthread_exit(NULL);
#endif
  }

  // ******************** Png output ********************
  else if(take_snapshots && mode == _Png ) {
    threadData *pData = new threadData;
    pData->fname = new char[512];
    pData->w = leftWindowWidth;
    pData->h = window_height;
    pData->imgThreadData = new cv::Mat( pData->h, pData->w, CV_8UC3 );

	sprintf(pData->fname, "%s%s-%s-%04d.png",
		snapshots_dir_path_name.c_str(),
		Png_file_name.c_str(),
		date_stringStream.str().c_str(),
		pg_FrameNo / stepPng);
	struct stat buffer;
	int count = 0;
	while (stat(pData->fname, &buffer) == 0) {
		sprintf(pData->fname, "%s%s-%s-%04d-%03d.jpg",
			snapshots_dir_path_name.c_str(),
			Png_file_name.c_str(),
			date_stringStream.str().c_str(),
			pg_FrameNo / stepPng, count);
		count++;
	}
	pg_logCurrentLineSceneVariables( pData->fname );

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

#ifdef WIN32
   DWORD rc;
   HANDLE  hThread = CreateThread(
            NULL,                   // default security attributes
            0,                      // use default stack size  
            writepng,		    // thread function name
            (void *)pData,		    // argument to thread function 
            0,                      // use default creation flags 
            &rc);   // returns the thread identifier 
   if (hThread == NULL){
         std::cout << "Error:unable to create thread writepng" << std::endl;
         exit(-1);
   }
   CloseHandle(hThread);
#else
   pthread_t drawing_thread;
   int rc;
   rc = pthread_create(&drawing_thread, NULL, 
                          writepng, (void *)pData);
   if (rc){
         std::cout << "Error:unable to create thread writepng" << rc << std::endl;
         exit(-1);
   }
   pthread_exit(NULL);
#endif
  }

  // ******************** Jpg output ********************
  else if(take_snapshots && mode == _Jpg ) {
    threadData *pData = new threadData;
    pData->fname = new char[512];
    pData->w = leftWindowWidth;
    pData->h = window_height;
    pData->imgThreadData = new cv::Mat( pData->h, pData->w, CV_8UC3 );

	sprintf(pData->fname, "%s%s-%s-%04d.jpg",
		snapshots_dir_path_name.c_str(),
		Jpg_file_name.c_str(),
		date_stringStream.str().c_str(),
		pg_FrameNo / stepJpg);
	struct stat buffer;
	int count = 0;
	while (stat(pData->fname, &buffer) == 0) {
		sprintf(pData->fname, "%s%s-%s-%04d-%03d.jpg",
			snapshots_dir_path_name.c_str(),
			Jpg_file_name.c_str(),
			date_stringStream.str().c_str(),
			pg_FrameNo / stepJpg, count);
		count++;
	}
	pg_logCurrentLineSceneVariables(pData->fname);
	printf( "Snapshot jpg step %d (%s)\n" ,
	     pg_FrameNo / stepJpg ,
	     pData->fname );

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

#ifdef WIN32
   DWORD rc;
   HANDLE  hThread = CreateThread(
            NULL,                   // default security attributes
            0,                      // use default stack size  
            writejpg,		    // thread function name
            (void *)pData,		    // argument to thread function 
            0,                      // use default creation flags 
            &rc);   // returns the thread identifier 
   if (hThread == NULL){
         std::cout << "Error:unable to create thread writejpg" << std::endl;
         exit(-1);
   }
   CloseHandle(hThread);
#else
   pthread_t drawing_thread;
   int rc;
   rc = pthread_create(&drawing_thread, NULL, 
                          writejpg, (void *)pData);
   if (rc){
         std::cout << "Error:unable to create thread writejpg" << rc << std::endl;
         exit(-1);
   }
   pthread_exit(NULL);
#endif
  }

  // ******************** interactive output ********************
  else if( mode == _Render ) {

 	// sets viewport to single window
	glViewport(0, 0, leftWindowWidth, window_height);

	glDisable(GL_BLEND);

	// particle pass #0
	pg_ParticleAnimationPass();

	// update pass #1
	pg_UpdatePass();

	/* // particle rendering pass #2
	glEnable(GL_BLEND);
	// glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	*/
	pg_ParticleRenderingPass();
	// glDisable(GL_BLEND);

	// layer compositing & echo pass #3
	pg_MixingPass();

	// final combination of echoed and non echoed rendering #4
	pg_MasterPass();
	printOglError(685);

#ifdef PG_SENSORS
	pg_SensorPass();
#endif
	printOglError(686);

#ifdef PG_SENSORS
	// /////////////////////////
	// read sensor values on CA (non echoed) and send messages
	glBindFramebuffer(GL_READ_FRAMEBUFFER, pg_FBO_Update[((pg_FrameNo) % 2) * PG_FBO_UPDATE_NBATTACHTS + pg_CA_FBO_Update_attcht]); // drawing memory on odd and even frames for echo and sensors	
	if (pg_FrameNo >= 10) {
		readSensors();
		printOglError(687);
	}
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
#endif


  }


  // // flushes OpenGL commands
  // glFlush();

}

