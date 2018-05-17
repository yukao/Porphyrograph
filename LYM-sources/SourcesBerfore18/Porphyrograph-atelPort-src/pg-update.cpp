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

// mute/unmute the right screen output in double screen
bool mute_screen = true;

/////////////////////////////////////////////////////////////////
// CAMERA PARAMETERS
float CameraCurrent_exposure = 0.0f;
float CameraCurrent_gain = 0.0f;
float CameraCurrent_brightness = 0.0f;
float CameraCurrent_saturation = 0.0f;
float CameraCurrent_contrast = 0.0f;
float CameraCurrent_FPS = 0.0f;
float CameraCurrent_focus = 0.0f;
float CameraCurrent_WB_B = 0.0f;
float CameraCurrent_WB_R = 0.0f;

/////////////////////////////////////////////////////////////////
// textures bitmaps and associated IDs
GLuint pg_screenMessageBitmap_texID = 0; // nb_attachments=1
GLubyte *pg_screenMessageBitmap = NULL;

GLuint pg_paths_Pos_Texture_texID[PG_NB_PATHS + 1];
GLfloat *pg_paths_Pos_Texture[PG_NB_PATHS + 1];
GLuint pg_paths_Col_Texture_texID[PG_NB_PATHS + 1];
GLfloat *pg_paths_Col_Texture[PG_NB_PATHS + 1];
GLuint pg_paths_RadBrushRendmode_Texture_texID[PG_NB_PATHS + 1];
GLfloat *pg_paths_RadBrushRendmode_Texture[PG_NB_PATHS + 1];

GLuint Font_texture_Rectangle_texID = 0;
cv::Mat Font_image;

GLuint Pen_texture_3D_texID = 0;
GLuint Noise_texture_3D = 0;

GLuint pg_particle_initial_images_texID[PG_NB_PARTICLE_INITIAL_IMAGES][2];

#ifdef PG_VIDEO_ACTIVE
// video texture ID, image and camera
GLuint pg_camera_texture_texID = 0;
GLuint pg_movie_texture_texID = 0;
GLuint pg_camera_currentBG_texture_texID = 0;
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
bool secondCBGCapture = false;
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

  // OpenGL initializations before redisplay
  OpenGLInit();
  // printOglError(509);

  // proper scene redrawing
  pg_draw_scene( _Render );

  // printf( "Window %s\n" , CurrentWindow->id );
  pg_screenMessage_update();

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
	if (!currentBGCapture && !secondCBGCapture) {
		isClearAllLayers = 0;
	}
	// clear CA reset
	isClearCA = 0;
	// clear layer reset
	isClearLayer = 0;
	// layer copy reset
	// copy to layer above (+1) or to layer below (-1)
	copyToNextTrack = 0;

	// /////////////////////////
	// particle initialization reset
	part_initialization = -1;
}

#ifdef PG_VIDEO_ACTIVE
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
			// threaded
			if (false) {
				if (!pg_MovieFrame_buffer_data.transfering) { // currentVideoTrack <=> video off
															  // frame capture
															  // printf("frame capture\n");
															  // thread for the transfer to GPU
					int indFreeOpenGLContext = GetFreeOpenGLContext();
					// printf("opengl context #%d\n", indFreeOpenGLContext);

					if (indFreeOpenGLContext >= 0) {
						pg_MovieFrame_buffer_data.transfering = true;
						pg_MovieFrame_buffer_data.transferred = false;
						ThreadOpenGLTextureData[indFreeOpenGLContext].fname = pg_MovieFrame_buffer_data.fname;
						ThreadOpenGLTextureData[indFreeOpenGLContext].photo_buffer_data = &pg_MovieFrame_buffer_data;
						ThreadOpenGLTextureData[indFreeOpenGLContext].textureID = pg_movie_texture_texID;
						ThreadOpenGLTextureData[indFreeOpenGLContext].is_rectangle = true;
						ThreadOpenGLTextureData[indFreeOpenGLContext].components = GL_RGB;
						ThreadOpenGLTextureData[indFreeOpenGLContext].datatype = GL_UNSIGNED_BYTE;
						ThreadOpenGLTextureData[indFreeOpenGLContext].texturefilter = GL_LINEAR;
						// printf("Load movie frame text ID %d\n", pg_camera_texture_texID);

						// Assuming that this is the main thread
						ThreadOpenGLTextureData[indFreeOpenGLContext].hdc = wglGetCurrentDC(); // GetDC(hwnd);
						HGLRC mainContext = wglGetCurrentContext();
						if (!ThreadOpenGLTextureData[indFreeOpenGLContext].loaderContext) {
							ThreadOpenGLTextureData[indFreeOpenGLContext].loaderContext
								= wglCreateContext(ThreadOpenGLTextureData[indFreeOpenGLContext].hdc);
						}
						wglShareLists(mainContext, ThreadOpenGLTextureData[indFreeOpenGLContext].loaderContext); // Order matters

#ifdef WIN32
						DWORD rc;
						HANDLE  hThread = CreateThread(
							NULL,                   // default security attributes
							0,                      // use default stack size  
							pg_threaded_loadMovieFrame,		    // thread function name
							(void *)& ThreadOpenGLTextureData[indFreeOpenGLContext],// argument to thread function 
							0,                      // use default creation flags 
							&rc);   // returns the thread identifier 
						if (hThread == NULL) {
							std::cout << "Error:unable to create camera frame thread pg_threaded_loadPhotoData" << std::endl;
						}
						CloseHandle(hThread);
						// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
						// should only be closed when the thread is finished + assigned NULL value
						// make code so that only one reading (or writing thread) is called at a time
						// because of concurrent call to glTexImage2D that might be a source of problem
						// a texture has been loaded in RAM
						// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#else
						pthread_t reading_thread;
						int rc;
						rc = pthread_create(&reading_thread, NULL,
							pg_threaded_loadMovieFrame, (void *)(void *)& ThreadOpenGLTextureData[indFreeOpenGLContext]);
						if (rc) {
							std::cout << "Error:unable to create thread pg_threaded_loadPhotoData" << rc << std::endl;
							pthread_exit(NULL);
						}
#endif
					}
				}
			}
			// non threaded
			else {
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
	}

	// /////////////////////////
	// reads next camera frame
	if (currentVideoTrack >= 0
		&& (// grabs according to camera capture frequence
		(cameraCaptFreq > 0 && pg_FrameNo % int(60.0 / cameraCaptFreq) == 0)
			// grabs at the beginning, otherwise the program is stuck if no camera frame reading takes place
			|| (pg_FrameNo >= PG_CAMERA_REST_DELAY && pg_FrameNo <= PG_CAMERA_REST_DELAY + 1000 && pg_FrameNo % 3 == 0))
		&& !pg_CameraFrame_buffer_data.transfering
		// a delay has to be let for camera to digest new parameter values
		&& (pg_FrameNo - pg_LastCameraParameterChange_Frame) > PG_CAMERA_REST_DELAY) {
		// currentVideoTrack <=> video off
		// frame capture
		// printf("frame capture\n");

		// thread for the transfer to GPU
		// threaded
		if (false) {
			int indFreeOpenGLContext = GetFreeOpenGLContext();
			// printf("opengl context #%d\n", indFreeOpenGLContext);

			if (indFreeOpenGLContext >= 0) {
				pg_CameraFrame_buffer_data.transfering = true;
				pg_CameraFrame_buffer_data.transferred = false;
				ThreadOpenGLTextureData[indFreeOpenGLContext].fname = pg_camera_capture_name;
				ThreadOpenGLTextureData[indFreeOpenGLContext].photo_buffer_data = &pg_CameraFrame_buffer_data;
				ThreadOpenGLTextureData[indFreeOpenGLContext].textureID = pg_camera_texture_texID;
				ThreadOpenGLTextureData[indFreeOpenGLContext].is_rectangle = true;
				ThreadOpenGLTextureData[indFreeOpenGLContext].components = GL_RGB;
				ThreadOpenGLTextureData[indFreeOpenGLContext].datatype = GL_UNSIGNED_BYTE;
				ThreadOpenGLTextureData[indFreeOpenGLContext].texturefilter = GL_LINEAR;
				// printf("Load camera frame text ID %d\n", pg_camera_texture_texID);

				// Assuming that this is the main thread
				ThreadOpenGLTextureData[indFreeOpenGLContext].hdc = wglGetCurrentDC(); // GetDC(hwnd);
				HGLRC mainContext = wglGetCurrentContext();
				if (!ThreadOpenGLTextureData[indFreeOpenGLContext].loaderContext) {
					ThreadOpenGLTextureData[indFreeOpenGLContext].loaderContext
						= wglCreateContext(ThreadOpenGLTextureData[indFreeOpenGLContext].hdc);
				}
				wglShareLists(mainContext, ThreadOpenGLTextureData[indFreeOpenGLContext].loaderContext); // Order matters

#ifdef WIN32
				DWORD rc;
				HANDLE  hThread = CreateThread(
					NULL,                   // default security attributes
					0,                      // use default stack size  
					pg_threaded_loadCameraFrame,		    // thread function name
					(void *)& ThreadOpenGLTextureData[indFreeOpenGLContext],// argument to thread function 
					0,                      // use default creation flags 
					&rc);   // returns the thread identifier 
				if (hThread == NULL) {
					std::cout << "Error:unable to create camera frame thread pg_threaded_loadPhotoData" << std::endl;
				}
				CloseHandle(hThread);
				// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				// should only be closed when the thread is finished + assigned NULL value
				// make code so that only one reading (or writing thread) is called at a time
				// because of concurrent call to glTexImage2D that might be a source of problem
				// a texture has been loaded in RAM
				// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#else
				pthread_t reading_thread;
				int rc;
				rc = pthread_create(&reading_thread, NULL,
					pg_threaded_loadCameraFrame, (void *)(void *)& ThreadOpenGLTextureData[indFreeOpenGLContext]);
				if (rc) {
					std::cout << "Error:unable to create thread pg_threaded_loadPhotoData" << rc << std::endl;
					pthread_exit(NULL);
				}
#endif
			}
		}
		// non threaded
		else {
			// printf("*** non threaded camera frame capture %d\n", pg_FrameNo);
			loadCameraFrame(false);
		}
	}
}
#endif

///////////////////////////////////////////////////////////////////////////////////////
// scene update

void pg_update_scene(void) {

	/////////////////////////////////////////////////////////////////////////
	// TRANSFER OF LOADED BUFFER IMAGES TO GPU
	// GPUtransfer_photo_buffer_data();

#include "pg_update_body.cpp"

	printOglError(510);

	/////////////////////////////////////////////////////////////////////////
	// CAMERA SHADER UNIFORM VARIABLES
//	glUseProgram(shader_programme[pg_shader_Camera]);
//#ifdef PG_VIDEO_ACTIVE
//	glUniform4f(uniform_Camera_fs_4fv_W_H,
//			(GLfloat)pg_camera_frame_width, (GLfloat)pg_camera_frame_height, 0.f, 0.f);
//#else
//	glUniform4f(uniform_Camera_fs_4fv_W_H, 0.f, 0.f, 0.f, 0.f);
//#endif

	/////////////////////////////////////////////////////////////////////////
	// UPDATE SHADER UNIFORM VARIABLES
	glUseProgram(shader_programme[pg_shader_Update]);

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
	for (int indCh = 0; indCh < PG_NB_PATHS; indCh++) {
		if (repop_channels[indCh]) {
			if (nbActChannels == selected_channel) {
				selected_channel = indCh;
				break;
			}
			nbActChannels++;
		}
	}
	glUniform4f(uniform_Update_fs_4fv_W_H_repopChannel_targetFrameNo, 
		(GLfloat)leftWindowWidth, (GLfloat)window_height, (GLfloat)selected_channel, GLfloat(pg_targetFrameNo));

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
	// flash Trk -> Part weights
	glUniform4f(uniform_Update_fs_4fv_flashTrkPartWghts,
		flashTrkPart_weights[0], flashTrkPart_weights[1], flashTrkPart_weights[2], flashTrkPart_weights[3]);
	// printf("flashTrkPart_weights %.2f %.2f %.2f %.2f \n", flashTrkPart_weights[0], flashTrkPart_weights[1], flashTrkPart_weights[2], flashTrkPart_weights[3]);

	// track decay
	glUniform4f(uniform_Update_fs_4fv_trkDecay_partDecay,
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
	// flash Trk -> CA weights
	glUniform4f(uniform_Update_fs_4fv_flashTrkPartWghts,
		flashTrkPart_weights[0], flashTrkPart_weights[1], flashTrkPart_weights[2], 0.f);
	// printf("flashTrkPart_weights %.2f %.2f %.2f \n", flashTrkPart_weights[0], flashTrkPart_weights[1], flashTrkPart_weights[2]);

	// track decay
	glUniform4f(uniform_Update_fs_4fv_trkDecay_partDecay,
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
	// flash Trk -> CA weights
	glUniform4f(uniform_Update_fs_4fv_flashTrkPartWghts,
		flashTrkPart_weights[0], flashTrkPart_weights[1], 0.f, 0.f);
	// printf("flashTrkPart_weights %.2f %.2f \n", flashTrkPart_weights[0],  flashTrkPart_weights[1]);

	// track decay
	glUniform4f(uniform_Update_fs_4fv_trkDecay_partDecay,
		trkDecay_0*trkDecay_sign_0,
		trkDecay_1*trkDecay_sign_1,
		0.f,
		0.f);
#endif
#endif
#endif

	// CA type, frame no, flashback and current track
	glUniform4f(uniform_Update_fs_4fv_CAdecay_frameno_Cursor_flashPartCAWght,
			CAdecay*CAdecay_sign, (GLfloat)pg_FrameNo, 
			(GLfloat)CurrentCursorStylusvsRubber, flashPartCA_weight);
		
	// movie size, flash camera and copy tracks
	// copy to layer above (+1) or to layer below (-1)
#ifdef PG_VIDEO_ACTIVE
	glUniform4f(uniform_Update_fs_4fv_movieWH_flashCameraTrkWght_cpTrack,
			(GLfloat)pg_movie_frame_width, (GLfloat)pg_movie_frame_height, 
			flashCameraTrk_weight, (GLfloat)copyToNextTrack);
	// printf("Flash camera coef %.1f\n", flashCameraTrk_weight);
	if(copyToNextTrack != 0)
		printf("copyToNextTrack %d\n", copyToNextTrack);
#else
	glUniform4f(uniform_Update_fs_4fv_movieWH_flashCameraTrkWght_cpTrack,
		(GLfloat)0.0f, (GLfloat)0.0f, flashCameraTrk_weight, (GLfloat)copyToNextTrack);
#endif
	// flash CA -> BG & repop color (BG & CA)
	glUniform4f(uniform_Update_fs_4fv_repop_Color_flashCABGWght,
		repop_Color_r, repop_Color_g, repop_Color_b, flashCABG_weight);
	// clear layer, flash pixel, flash CA -> Part
	glUniform4f(uniform_Update_fs_4fv_isClearLayer_flashPixel_flashCAPartWght_nbParticles,
		(GLfloat)isClearLayer, (GLfloat)flashPixel, (GLfloat)flashCAPart_weight, (GLfloat)nb_particles);
	// photo size 
	glUniform4f(uniform_Update_fs_4fv_photo01_wh,
		(GLfloat)pg_Photo_buffer_data[0].w, (GLfloat)pg_Photo_buffer_data[0].h, 
		(GLfloat)pg_Photo_buffer_data[1].w, (GLfloat)pg_Photo_buffer_data[1].h);

	// photo weights 
	glUniform4f(uniform_Update_fs_4fv_photo01Wghts_Camera_W_H,
		pg_Photo_weight[0] * photoWeight, pg_Photo_weight[1] * photoWeight,
		(GLfloat)pg_camera_frame_width, (GLfloat)pg_camera_frame_height);
	//printf("photo weight %.2f %.2f\n",
	//	pg_Photo_weight[0] * photoWeight, pg_Photo_weight[1] * photoWeight);
	//printf("camera movie weight %.2f %.2f\n",
	//	*((float *)ScenarioVarPointers[_cameraWeight]), *((float *)ScenarioVarPointers[_movieWeight]));

	// repop values 
	glUniform4f(uniform_Update_fs_4fv_repop_part_path_acc_damp_factor,
		min(.9f , repop_part + pulse_average * repop_part_pulse),
		min(.9f , repop_path + pulse_average * repop_part_pulse),
		part_acc_factor + pulse_average * part_acc_pulse, part_damp_factor);
	//printf("repop part path %.2f %.2f\n",
	//	min(.9f, repop_part + pulse_average * repop_part_pulse),
	//	min(.9f, repop_path + pulse_average * repop_part_pulse));

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

	printOglError(511);

	/////////////////////////////////////////////////////////////////////////
	// PARTICLE SHADER UNIFORM VARIABLES

	glUseProgram(shader_programme[pg_shader_Particle]);
	glUniform3f(uniform_ParticleSplat_gs_3fv_partRadius_partType_highPitchPulse,
				(partRadius + pulse_average * part_radius_pulse * partRadius) / 512.f,
				(GLfloat)particle_type,
				pulse[2]);
#ifdef ATELIERS_PORTATIFS
	///////////////////////////////////////////////////////////////////////
	bool assigned = false;
	for (int indPath = 3; indPath < PG_NB_PATHS + 1; indPath++) {
		// active reading
		if (is_path_replay[indPath]) {
			glUniform2f(uniform_ParticleSplat_vp_2fv_trackReplay_xy, paths_x[indPath], paths_y[indPath]);
			assigned = true;
			break;
		}
	}
	if (!assigned) {
		glUniform2f(uniform_ParticleSplat_vp_2fv_trackReplay_xy, -1000.f, -1000.f);
	}
#else
	///////////////////////////////////////////////////////////////////////
	bool assigned = false;
	for (int indPath = 1; indPath < PG_NB_PATHS + 1; indPath++) {
		// active reading
		if (is_path_replay[indPath]) {
			glUniform2f(uniform_ParticleSplat_vp_2fv_trackReplay_xy, paths_x[indPath], paths_y[indPath]);
			assigned = true;
			break;
		}
	}
	if (!assigned) {
       if(paths_x[0] >= 0 && paths_y[0] >= 0) {
		glUniform2f(uniform_ParticleSplat_vp_2fv_trackReplay_xy, paths_x[0], paths_y[0]);
       }
       else {
		glUniform2f(uniform_ParticleSplat_vp_2fv_trackReplay_xy, -1000.f, -1000.f);
      }
	}
#endif

	//printf("pulsed particle radius %.4f rad %.4f pulse %.4f coef %.4f\n", 
	//	(partRadius + pulse_average * part_radius_pulse * partRadius) / 512.f,
	//	partRadius / 512.f, pulse_average, part_radius_pulse);

	/////////////////////////////////////////////////////////////////////////
	// MIXING SHADER UNIFORM VARIABLES

	glUseProgram(shader_programme[pg_shader_Mixing]);

	// CA weight
	glUniform4f(uniform_Mixing_fs_4fv_pulsedShift_height_msgTrspcy_flashCameraTrkWght, 
		(pulse_average - pulse_average_prec) * echo_Hshift_pulse, (GLfloat)window_height, 
		messageTransparency, flashCameraTrk_weight);

	// printOglError(512);

	/////////////////////////////////////////////////////////////////////////
	// MASTER SHADER UNIFORM VARIABLES
	glUseProgram(shader_programme[pg_shader_Master]);
	// hoover cursor
	glUniform4f(uniform_Master_fs_4fv_xy_frameno_pulsedShift,
		(GLfloat)CurrentCursorHooverPos_x, (GLfloat)CurrentCursorHooverPos_y,
		(GLfloat)pg_FrameNo, (pulse_average - pulse_average_prec) * tracks_Hshift_pulse);
	// screen size
	glUniform3f(uniform_Master_fs_3fv_width_height_mute_screen,
		(GLfloat)leftWindowWidth, (GLfloat)window_height, GLfloat(int(mute_screen)));
	// printf("hoover %d %d\n",CurrentCursorHooverPos_x, CurrentCursorHooverPos_y);


	// resets to 0 the variables that are only true for one frame such as flashes, resets...
	one_frame_variables_reset();

#ifdef PG_VIDEO_ACTIVE
	// loads movie and/or camera frames
	camera_and_video_frame_updates();
#endif

	printOglError(517);

}

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
// MULTIPASS RENDERING
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////
// PASS #1: UPDATE (CA, PIXELS, PARTICLES, DRAWING, PHOTOS & VIDEOS)

void pg_UpdatePass(void) {
	// ping pong output and input FBO bindings
	// glBindFramebuffer(GL_READ_FRAMEBUFFER, pg_FBO_Update[(pg_FrameNo % 2)]);
	if (pg_FrameNo > 0) {
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, pg_FBO_Update[((pg_FrameNo + 1) % 2)]);
	}

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
	glUniform1i(uniform_Update_texture_fs_decal, pg_CA_FBO_attcht);
	glUniform1i(uniform_Update_texture_fs_lookupTable1, pg_Pixel_FBO_attcht);
	glUniform1i(uniform_Update_texture_fs_lookupTable2, pg_ParticlePosSpeed_FBO_attcht);
	glUniform1i(uniform_Update_texture_fs_lookupTable3, pg_ParticleColRadius_FBO_attcht);
	glUniform1i(uniform_Update_texture_fs_lookupTable4, pg_ParticleTargetPosColRadius_FBO_attcht);
	glUniform1i(uniform_Update_texture_fs_lookupTable5, pg_Track0_FBO_attcht);
#if PG_NB_TRACKS >= 2
	glUniform1i(uniform_Update_texture_fs_lookupTable6, pg_Track1_FBO_attcht);
#endif
#if PG_NB_TRACKS >= 3
	glUniform1i(uniform_Update_texture_fs_lookupTable7, pg_Track2_FBO_attcht);
#endif
#if PG_NB_TRACKS >= 4
	glUniform1i(uniform_Update_texture_fs_lookupTable8, 8);
#endif
	glUniform1i(uniform_Update_texture_fs_lookupTable8, 8);
#ifdef PG_VIDEO_ACTIVE
	glUniform1i(uniform_Update_texture_fs_lookupTable9, 9);
	glUniform1i(uniform_Update_texture_fs_lookupTable10, 10);
	glUniform1i(uniform_Update_texture_fs_lookupTable11, 11);
#endif
	glUniform1i(uniform_Update_texture_fs_lookupTable12, 12);
	glUniform1i(uniform_Update_texture_fs_lookupTable13, 13);
	glUniform1i(uniform_Update_texture_fs_lookupTable14, 14);
	glUniform1i(uniform_Update_texture_fs_lookupTable15, 15);
	glUniform1i(uniform_Update_texture_fs_lookupTable16, 16);
	glUniform1i(uniform_Update_texture_fs_lookupTable17, 17);

	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP);

	// 2-cycle ping-pong CA step n (FBO attachment 0)
	glActiveTexture(GL_TEXTURE0 + pg_CA_FBO_attcht);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[(pg_FrameNo % 2) * PG_NB_ATTACHMENTS + pg_CA_FBO_attcht]);

	// 2-cycle ping-pong speed/position of pixels step n step n (FBO attachment 1)
	glActiveTexture(GL_TEXTURE0 + pg_Pixel_FBO_attcht);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[(pg_FrameNo % 2) * PG_NB_ATTACHMENTS + pg_Pixel_FBO_attcht]);

	// 2-cycle ping-pong position/speed of particles step n step n (FBO attachment 2)
	glActiveTexture(GL_TEXTURE0 + pg_ParticlePosSpeed_FBO_attcht);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[(pg_FrameNo % 2) * PG_NB_ATTACHMENTS + pg_ParticlePosSpeed_FBO_attcht]);

	// 2-cycle ping-pong color/radius of particles step n step n (FBO attachment 3)
	glActiveTexture(GL_TEXTURE0 + pg_ParticleColRadius_FBO_attcht);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[(pg_FrameNo % 2) * PG_NB_ATTACHMENTS + pg_ParticleColRadius_FBO_attcht]);

	// 2-cycle ping-pong target position/color/radius of particles step n step n (FBO attachment 4)
	glActiveTexture(GL_TEXTURE0 + pg_ParticleTargetPosColRadius_FBO_attcht);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[(pg_FrameNo % 2) * PG_NB_ATTACHMENTS + pg_ParticleTargetPosColRadius_FBO_attcht]);

	// 2-cycle ping-pong BG track step n (FBO attachment 5)
	glActiveTexture(GL_TEXTURE0 + pg_Track0_FBO_attcht);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[(pg_FrameNo % 2) * PG_NB_ATTACHMENTS + pg_Track0_FBO_attcht]);

#if PG_NB_TRACKS >= 2
	// 2-cycle ping-pong track 1 step n (FBO attachment 6)
	glActiveTexture(GL_TEXTURE0 + pg_Track1_FBO_attcht);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[(pg_FrameNo % 2) * PG_NB_ATTACHMENTS + pg_Track1_FBO_attcht]);
#endif

#if PG_NB_TRACKS >= 3
	// 2-cycle ping-pong track 2 step n (FBO attachment 7)
	glActiveTexture(GL_TEXTURE0 + pg_Track2_FBO_attcht);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[(pg_FrameNo % 2) * PG_NB_ATTACHMENTS + pg_Track2_FBO_attcht]);
#endif

#if PG_NB_TRACKS >= 4
	// 2-cycle ping-pong track 2 step n (FBO attachment 8)
	glActiveTexture(GL_TEXTURE0 + pg_Track3_FBO_attcht);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[(pg_FrameNo % 2) * PG_NB_ATTACHMENTS + pg_Track3_FBO_attcht]);
#endif

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// pen patterns
	glActiveTexture(GL_TEXTURE0 + PG_NB_ATTACHMENTS);
	glBindTexture(GL_TEXTURE_3D, Pen_texture_3D_texID);

#ifdef PG_VIDEO_ACTIVE
	// camera texture produced at preceding pass
	// glActiveTexture(GL_TEXTURE0 + 7);
	// glBindTexture(GL_TEXTURE_RECTANGLE, FBO_CameraFrame_texID);
	// current camera texture
	glActiveTexture(GL_TEXTURE0 + PG_NB_ATTACHMENTS + 1);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_camera_texture_texID);

	// current background texture
	glActiveTexture(GL_TEXTURE0 + PG_NB_ATTACHMENTS + 2);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_camera_currentBG_texture_texID);


	// movie texture
	glActiveTexture(GL_TEXTURE0 + PG_NB_ATTACHMENTS + 3);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_movie_texture_texID);
#endif

	// noise texture (noise is also generated procedurally in the update shader)
	glActiveTexture(GL_TEXTURE0 + PG_NB_ATTACHMENTS + 4);
	glBindTexture(GL_TEXTURE_3D, Noise_texture_3D);

	// photo[0] texture
	glActiveTexture(GL_TEXTURE0 + PG_NB_ATTACHMENTS + 5);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_Photo_buffer_data[0].texBuffID);

	// photo[1] texture
	glActiveTexture(GL_TEXTURE0 + PG_NB_ATTACHMENTS + 6);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_Photo_buffer_data[1].texBuffID);

	// FBO capture of particle rendering used for flashing layers with particles
	glActiveTexture(GL_TEXTURE0 + PG_NB_ATTACHMENTS + 7);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Particle_texID);

	// position speed are stored in a texture
	glActiveTexture(GL_TEXTURE0 + PG_NB_ATTACHMENTS + 8);
	// printf("particle initialization %d\n", part_initialization);
	if (part_initialization >= 0 && part_initialization < PG_NB_PARTICLE_INITIAL_IMAGES) {
		glBindTexture(GL_TEXTURE_RECTANGLE, pg_particle_initial_images_texID[part_initialization][0]);
		printf("particle initialization %d\n", part_initialization);
	}
	// photo or video particles, position and speed are chosen from a random texture between 
	// 0 & PG_NB_PARTICLE_INITIAL_IMAGES - 1
	else if (part_initialization >= PG_NB_PARTICLE_INITIAL_IMAGES 
		&& part_initialization < PG_NB_PARTICLE_INITIAL_IMAGES + 2) {
		int indTex = int(floor(randomValue * PG_NB_PARTICLE_INITIAL_IMAGES)) % PG_NB_PARTICLE_INITIAL_IMAGES;
		glBindTexture(GL_TEXTURE_RECTANGLE, pg_particle_initial_images_texID[indTex][0]);
		printf("particle initialization %d texture %d\n", part_initialization, indTex);
	}
	glActiveTexture(GL_TEXTURE0 + PG_NB_ATTACHMENTS + 9);
	// color and radius are taken from photo or video and partic radius
	if (part_initialization >= 0 && part_initialization < PG_NB_PARTICLE_INITIAL_IMAGES) {
		glBindTexture(GL_TEXTURE_RECTANGLE, pg_particle_initial_images_texID[part_initialization][1]);
	}
	// for part_initialization == PG_NB_PARTICLE_INITIAL_IMAGES || part_initialization == PG_NB_PARTICLE_INITIAL_IMAGES + 1
	// the photo or video pixel values are read directly into the shader 

	//if (pg_FrameNo % 1000 <= 1) {
	//	printf("Update check texID 0-11 %d %d %d %d %d %d %d %d %d %d %d %d\n",
	//		pg_FBO_Update_texID[(pg_FrameNo % 2) * PG_NB_ATTACHMENTS],
	//		pg_FBO_Update_texID[(pg_FrameNo % 2) * PG_NB_ATTACHMENTS + 1],
	//		pg_FBO_Update_texID[(pg_FrameNo % 2) * PG_NB_ATTACHMENTS + 2],
	//		pg_FBO_Update_texID[(pg_FrameNo % 2) * PG_NB_ATTACHMENTS + 3],
	//		pg_FBO_Update_texID[(pg_FrameNo % 2) * PG_NB_ATTACHMENTS + 4],
	//		pg_FBO_Update_texID[(pg_FrameNo % 2) * PG_NB_ATTACHMENTS + 5],
	//		Pen_texture_3D_texID,
	//		FBO_CameraFrame_texID,
	//		pg_movie_texture_texID,
	//		Noise_texture_3D,
	//		pg_Photo_buffer_data[0].texBuffID,
	//		pg_Photo_buffer_data[1].texBuffID);
	//}

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

	printOglError(52);
}

/////////////////////////////////////
// PASS #2: PARTICLE RENDERING PASS

void pg_ParticlePass( void ) {
	// draws the Bezier curve
	if (pg_FrameNo > 0) {
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, pg_FBO_Particle);
	}
	//glDisable(GL_BLEND);
	//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	////////////////////////////////////////
	// activate shaders and sets uniform variable values    
	glUseProgram(shader_programme[pg_shader_Particle]);

	glUniformMatrix4fv(uniform_Particle_vp_proj, 1, GL_FALSE, projMatrix);
	glUniformMatrix4fv(uniform_Particle_vp_view, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(uniform_Particle_vp_model, 1, GL_FALSE, modelMatrix);

	// texture unit location
	// position/speed Particle update
	glUniform1i(uniform_Particle_texture_vp_decal, 0);
	// color/radius Particle update
	glUniform1i(uniform_Particle_texture_vp_lookupTable1, 1);
#ifdef CURVE_PARTICLES
	// comet texture
	glUniform1i(uniform_Particle_Comet_texture_fs_decal, 2);
#endif
#ifdef BLURRED_SPLAT_PARTICLES
	// blurred disk texture
	glUniform1i(uniform_Particle_BlurredDisk_texture_fs_decal, 2);
#endif

	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP);

	// 2-cycle ping-pong bezier Particle update
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[(pg_FrameNo % 2) * PG_NB_ATTACHMENTS + pg_ParticlePosSpeed_FBO_attcht]);

	// 2-cycle ping-pong bezier Particle update
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[(pg_FrameNo % 2) * PG_NB_ATTACHMENTS + pg_ParticleColRadius_FBO_attcht]);

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
	glUniform1i(uniform_Mixing_texture_fs_decal, 0);
	glUniform1i(uniform_Mixing_texture_fs_lookupTable1, 1);
	glUniform1i(uniform_Mixing_texture_fs_lookupTable2, 2);
#if PG_NB_TRACKS >= 2
	glUniform1i(uniform_Mixing_texture_fs_lookupTable3, 3);
#endif
#if PG_NB_TRACKS >= 3
	glUniform1i(uniform_Mixing_texture_fs_lookupTable4, 4);
#endif
#if PG_NB_TRACKS >= 4
	glUniform1i(uniform_Mixing_texture_fs_lookupTable5, 5);
#endif
	glUniform1i(uniform_Mixing_texture_fs_lookupTable5, 5);

	glUniform1i(uniform_Mixing_texture_fs_lookupTable_font, 6);
	glUniform1i(uniform_Mixing_texture_fs_lookupTable_message, 7);

	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP);
	// 2-cycle ping-pong CA step n + 1 (FBO attachment 0)
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * PG_NB_ATTACHMENTS + pg_CA_FBO_attcht]);

	// Particles step n
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Particle_texID);

	// 2-cycle ping-pong track 0 step n + 1 (FBO attachment 3)
	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * PG_NB_ATTACHMENTS + pg_Track0_FBO_attcht]);

#if PG_NB_TRACKS >= 2
	// 2-cycle ping-pong track 1 step n + 1 (FBO attachment 4)
	glActiveTexture(GL_TEXTURE0 + 3);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * PG_NB_ATTACHMENTS + pg_Track1_FBO_attcht]);
#endif

#if PG_NB_TRACKS >= 3
	// 2-cycle ping-pong track 2 step n + 1 (FBO attachment 5)
	glActiveTexture(GL_TEXTURE0 + 4);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * PG_NB_ATTACHMENTS + pg_Track2_FBO_attcht]);
#endif

#if PG_NB_TRACKS >= 4
	// 2-cycle ping-pong track 3 step n + 1 (FBO attachment 6)
	glActiveTexture(GL_TEXTURE0 + 5);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * PG_NB_ATTACHMENTS + pg_Track3_FBO_attcht]);
#endif

	// preceding snapshot for echo
	glActiveTexture(GL_TEXTURE0 + 5);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Mixing_capturedFB_prec_texID[((pg_FrameNo + 1) % 2)]);  // drawing memory on odd and even frames for echo 

	// font texture
	glActiveTexture(GL_TEXTURE0 + 6);
	glBindTexture(GL_TEXTURE_RECTANGLE, Font_texture_Rectangle_texID);

	// font texture
	glActiveTexture(GL_TEXTURE0 + 7);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_screenMessageBitmap_texID);

	//if (pg_FrameNo % 1000 <= 1) {
	//	printf("Comp check texID 0-7 %d %d %d %d %d %d %d %d\n",
	//		pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * PG_NB_ATTACHMENTS],
	//		pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * PG_NB_ATTACHMENTS + 2],
	//		pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * PG_NB_ATTACHMENTS + 3],
	//		pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * PG_NB_ATTACHMENTS + 4],
	//		pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * PG_NB_ATTACHMENTS + 5],
	//		pg_FBO_Mixing_capturedFB_prec_texID[((pg_FrameNo + 1) % 2)],
	//		Font_texture_Rectangle_texID,
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
	glUniform1i(uniform_Master_texture_fs_decal, 0);
	glUniform1i(uniform_Master_texture_fs_lookupTable1, 1);
	glUniform1i(uniform_Master_texture_fs_lookupTable2, 2);
	glUniform1i(uniform_Master_texture_fs_lookupTable3, 3);
#if PG_NB_TRACKS >= 2
	glUniform1i(uniform_Master_texture_fs_lookupTable4, 4);
#endif
#if PG_NB_TRACKS >= 3
	glUniform1i(uniform_Master_texture_fs_lookupTable5, 5);
#endif
#if PG_NB_TRACKS >= 4
	glUniform1i(uniform_Master_texture_fs_lookupTable6, 6);
#endif

	// Mixing pass output (echoed composition of tracks)
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Mixing_capturedFB_prec_texID[(pg_FrameNo % 2)]);

	// 2-cycle ping-pong CA step n (FBO attachment 0)
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * PG_NB_ATTACHMENTS + pg_CA_FBO_attcht]);

	// Particles step n 
	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Particle_texID);

	// 2-cycle ping-pong track 0 step n (FBO attachment 3)
	glActiveTexture(GL_TEXTURE0 + 3);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * PG_NB_ATTACHMENTS + pg_Track0_FBO_attcht]);

#if PG_NB_TRACKS >= 2
	// 2-cycle ping-pong track 1 step n (FBO attachment 4)
	glActiveTexture(GL_TEXTURE0 + 4);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * PG_NB_ATTACHMENTS + pg_Track1_FBO_attcht]);
#endif

#if PG_NB_TRACKS >= 3
	// 2-cycle ping-pong track 2 step n (FBO attachment 5)
	glActiveTexture(GL_TEXTURE0 + 5);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * PG_NB_ATTACHMENTS + pg_Track2_FBO_attcht]);
#endif

#if PG_NB_TRACKS >= 4
	// 2-cycle ping-pong track 3 step n (FBO attachment 6)
	glActiveTexture(GL_TEXTURE0 + 6);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * PG_NB_ATTACHMENTS + pg_Track3_FBO_attcht]);
#endif

	//if (pg_FrameNo % 1000 <= 1) {
	//	printf("Final check texID 0-5 %d %d %d %d %d %d\n\n",
	//		pg_FBO_Mixing_capturedFB_prec_texID[(pg_FrameNo % 2)],
	//		pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * PG_NB_ATTACHMENTS],
	//		pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * PG_NB_ATTACHMENTS + 2],
	//		pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * PG_NB_ATTACHMENTS + 3],
	//		pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * PG_NB_ATTACHMENTS + 4],
	//		pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * PG_NB_ATTACHMENTS + 5]);
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
  else if( mode == _Png ) {
    threadData *pData = new threadData;
    pData->fname = new char[512];
    pData->w = leftWindowWidth;
    pData->h = window_height;
    pData->img = new cv::Mat( pData->h, pData->w, CV_8UC3 );

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
    glReadPixels(0, 0, pData->w, pData->h, GL_RGB, GL_UNSIGNED_BYTE, pData->img->data);

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
  else if( mode == _Jpg ) {
    threadData *pData = new threadData;
    pData->fname = new char[512];
    pData->w = leftWindowWidth;
    pData->h = window_height;
    pData->img = new cv::Mat( pData->h, pData->w, CV_8UC3 );

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
    glReadPixels(0, 0, pData->w, pData->h, GL_RGB, GL_UNSIGNED_BYTE, pData->img->data);

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

  // ******************** Video output ********************
#ifdef PG_HAVE_FFMPEG
  else if( mode == _Video 
	   && PG_EnvironmentNode->audioVideoOutData ) {
    glReadBuffer(GL_BACK);
    PG_EnvironmentNode->audioVideoOutData
      ->write_frame();
  }
#endif

  // ******************** interactive output ********************
  else if( mode == _Render ) {

 	// sets viewport to single window
	glViewport(0, 0, leftWindowWidth, window_height);

	glDisable(GL_BLEND);
	// camera pass #0
	// pg_CameraPass();

	// update pass #1
	pg_UpdatePass();

#ifndef ATELIERS_PORTATIFS
	glEnable(GL_BLEND);
	// glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
#endif

	// particle rendering pass #2
	pg_ParticlePass();

#ifndef ATELIERS_PORTATIFS
	glDisable(GL_BLEND);
#endif

	// layer compositing & echo pass #3
	pg_MixingPass();

	// final combination of echoed and non echoed rendering #4
	pg_MasterPass();
	
    printOglError( 595 );

  }


  // // flushes OpenGL commands
  // glFlush();

}

