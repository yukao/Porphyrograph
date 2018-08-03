/*! \file pg-update.h
 * 
 * 
 *     File pg-update.h
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

#ifndef PG_UPDATE_H
#define PG_UPDATE_H

/////////////////////////////////////////////////////////////////
// textures bitmaps and associated IDs
extern GLuint pg_screenMessageBitmap_texID; // nb_attachments=1
extern GLubyte *pg_screenMessageBitmap;

extern GLuint pg_paths_Pos_Texture_texID[PG_NB_PATHS + 1];
extern GLfloat *pg_paths_Pos_Texture[PG_NB_PATHS + 1];
extern GLuint pg_paths_Col_Texture_texID[PG_NB_PATHS + 1];
extern GLfloat *pg_paths_Col_Texture[PG_NB_PATHS + 1];
extern GLuint pg_paths_RadBrushRendmode_Texture_texID[PG_NB_PATHS + 1];
extern GLfloat *pg_paths_RadBrushRendmode_Texture[PG_NB_PATHS + 1];

extern GLuint Screen_Font_texture_Rectangle_texID;
#if defined (TVW) || defined (CRITON)
extern GLuint Display_Font_texture_Rectangle_texID;
#endif
extern cv::Mat Screen_Font_image;
extern cv::Mat Display_Font_image;

extern GLuint Pen_texture_3D_texID;
extern GLuint Noise_texture_3D;

extern GLuint pg_particle_initial_images_texID[PG_NB_PARTICLE_INITIAL_IMAGES][2];

#ifdef PG_SENSORS
extern GLuint Sensor_texture_rectangle;
extern cv::Mat Sensor_image;
#endif

#if defined (TVW) || defined (CRITON)
#define   PG_NB_DISPLAY_MESSAGES 3500

////////////////////////////////////////////////////////////////
// textures bitmaps and associated IDs
extern GLuint pg_Display_Message1_Bitmap_texID;
extern GLubyte *pg_displayMsg1Bitmap;
extern GLuint pg_Display_Message2_Bitmap_texID;
extern GLubyte *pg_displayMsg2Bitmap;

////////////////////////////////////////
// large messages displayed on the bottom of the screen
extern string DisplayText1;
extern float DisplayText1Alpha;
extern float LengthDisplayText1;
extern int IndDisplayText1;

extern string DisplayText2;
extern float DisplayText2Alpha;
extern float LengthDisplayText2;
extern int IndDisplayText2;

extern bool DisplayText1Front;
extern float DisplayTextSwapInitialTime;

extern std::string* DisplayTextList;
extern int* DisplayTextFirstInChapter;
extern int NbDisplayTexts;

// message chapters
extern int NbTextChapters;
#endif

////////////////////////////////////////
// geometry: track lines
bool pg_initFBO( void );
void pg_initRenderingMatrices( void );
void pg_initGeometry_quads( void );
bool pg_initTextures( void );

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

// video texture ID, image and camera
extern GLuint pg_camera_texture_texID;
extern GLuint pg_movie_texture_texID;
extern GLuint pg_camera_BG_texture_texID;
#ifdef GN
extern GLuint pg_camera_BGIni_texture_texID;
#endif
// extern IplImage *pg_camera_frame;
// extern IplImage *pg_movie_frame;
extern int pg_camera_frame_width;
extern int pg_camera_frame_height;
extern int pg_camera_x_offset;
extern int pg_camera_y_offset;
extern int pg_movie_frame_width;
extern int pg_movie_frame_height;

// video tracks
extern vector<string> movieFileName;
extern vector<string> movieShortName;
extern int nb_movies;
// soundtracks
extern vector<string> trackFileName;
extern vector<string> trackShortName;
extern int currentlyPlaying_trackNo;
extern int nb_soundtracks;
// pen palettes presets
extern int nb_pen_colorPresets;
extern vector<string> pen_colorPresets_names;
extern float *pen_colorPreset_values;
// pen palettes colors
extern int nb_pen_palette_colors;
extern vector<string> pen_palette_colors_names;
extern float **pen_palette_colors_values;
// photo albums
extern vector<string> photoAlbumDirName;
extern int nb_photo_albums;
// pen brushes
extern string pen_brushes_fileName;
extern int nb_pen_brushes;

extern bool is_capture_diaporama;
extern VideoCapture  pg_camera_capture;
extern char pg_camera_capture_name[256];
extern VideoCapture  pg_movie_capture;
extern int pg_movie_nbFrames;
extern bool is_movieLooping;
extern bool is_movieLoading;
extern bool secondCurrentBGCapture;
#ifdef GN
extern bool secondInitialBGCapture;
extern bool initialBGCapture;
#endif

/*! \name Group texture_ mode
 */
// \{
/// _Render : rendering,
/// _Jpg : framebuffer output to jpg file
/// _Png : framebuffer output to png file
/// _Svg : framebuffer output to svg file
enum DrawingMode{ _Render=0 , _Svg , _Png , _Jpg , _Video };

// on/off automatic image output in case of live performance
extern bool is_automatic_snapshots;

// projection and view matrices transmitted to the shader
extern GLfloat projMatrix[16];
extern GLfloat viewMatrix[16];
extern GLfloat modelMatrix[16];

extern GLuint drawBuffers[16];

/////////////////////////////////////////////////////////////////
// config variables
// current mouse location (also used for displaying the cursor)
extern float mouse_x;
extern float mouse_y;
extern float mouse_x_prev;
extern float mouse_y_prev;

extern float paths_x[PG_NB_PATHS + 1];
extern float paths_y[PG_NB_PATHS + 1];
extern float paths_x_prev[PG_NB_PATHS + 1];
extern float paths_y_prev[PG_NB_PATHS + 1];
extern float paths_Color_r[PG_NB_PATHS + 1];
extern float paths_Color_g[PG_NB_PATHS + 1];
extern float paths_Color_b[PG_NB_PATHS + 1];
extern float paths_Color_a[PG_NB_PATHS + 1];
extern float repop_Color_r;
extern float repop_Color_g;
extern float repop_Color_b;
extern int paths_BrushID[PG_NB_PATHS + 1];
extern float paths_RadiusX[PG_NB_PATHS + 1];
extern float paths_RadiusY[PG_NB_PATHS + 1];

#if defined (GN) || defined (MALAUSSENA)
extern GLuint pg_CATable_ID;
extern GLubyte *pg_CATable;
#endif
#ifdef GN
extern GLuint pg_LYMlogo_texID;
extern cv::Mat pg_LYMlogo_image;
#endif


#ifdef PG_SENSORS
//////////////////////////////////////////////////////////////////////
// SENSORS
//////////////////////////////////////////////////////////////////////

// sensor translations
// current sensor layout
extern float sensorPositions[3 * PG_NB_SENSORS];
// all possible sensor layouts
extern float sensorLayouts[3 * PG_NB_SENSORS * PG_NB_MAX_SENSOR_LAYOUTS];

// sensor on off
// current sensor activation pattern
extern bool sensor_onOff[PG_NB_SENSORS];
extern float sensor_last_activation_time;
// all sensor activation patterns
extern bool sensorActivations[PG_NB_SENSORS * PG_NB_MAX_SENSOR_ACTIVATIONS];

// sample choice
// current sample choice
extern int sample_choice[PG_NB_SENSORS];
// all possible sensor layouts
extern int sample_setUps[PG_NB_MAX_SAMPLE_SETUPS][PG_NB_SENSORS];
// sample start playing time for muting after 1 cycle
extern float sample_play_start[PG_NB_MAX_SAMPLE_SETUPS * PG_NB_SENSORS];
#endif

// data structure for threads to use.
// This is passed by void pointer so it can be any data type
// that can be passed using a single void pointer (LPVOID).
class threadData {
public:
  char *fname;
  int w;
  int h;
  cv::Mat *imgThreadData;
};


// FUNCTIONS
void window_display(void);
void one_frame_variables_reset(void);
void pg_update_shader_uniforms(void);
void camera_and_video_frame_updates(void);
// PASS #0: PARTICLE ANIMATION PASS
void pg_ParticleAnimationPass(void);
// PASS #1: UPDATE (CA, PIXELS, PARTICLES)
void pg_UpdatePass(void);
// PASS #2: PARTICLE RENDERING PASS
void pg_ParticleRenderingPass(void);
// PASS #3: COMPOSITION + PING-PONG ECHO: ECHOED MIX OF LAYERS
void pg_MixingPass(void);
// PASS #4: FINAL DISPLAY: MIX OF ECHOED AND NON-ECHOED LAYERS
void pg_MasterPass(void);
#ifdef PG_SENSORS
// PASS #5: SENSOR PASS
void pg_SensorPass(void);
#endif
// DRAWING A SCENE ON VARIOUS MODALITIES (CURVE, IMAGE, FRAMEBUFFER...)
void pg_draw_scene(DrawingMode mode);

#endif
