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

extern GLuint Font_texture_Rectangle_texID;
extern cv::Mat Font_image;

extern GLuint Pen_texture_3D_texID;
extern GLuint Noise_texture_3D;

extern GLuint pg_particle_initial_images_texID[PG_NB_PARTICLE_INITIAL_IMAGES][2];

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
extern float CameraCurrent_WB_B;
extern float CameraCurrent_WB_R;

#ifdef PG_VIDEO_ACTIVE
// video texture ID, image and camera
extern GLuint pg_camera_texture_texID;
extern GLuint pg_movie_texture_texID;
extern GLuint pg_camera_currentBG_texture_texID;
// extern IplImage *pg_camera_frame;
// extern IplImage *pg_movie_frame;
extern int pg_camera_frame_width;
extern int pg_camera_frame_height;
extern int pg_movie_frame_width;
extern int pg_movie_frame_height;
extern int currentlyPlaying_movieNo;
extern int currentlyPlaying_trackNo;
extern int current_pen_colorPreset;

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
extern bool secondCBGCapture;
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

// mute/unmute the right screen output in double screen
extern bool mute_second_screen;

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

// data structure for threads to use.
// This is passed by void pointer so it can be any data type
// that can be passed using a single void pointer (LPVOID).
class threadData {
public:
  char *fname;
  int w;
  int h;
  cv::Mat *img;
};


// FUNCTIONS
void window_display(void);
void one_frame_variables_reset(void);
void pg_update_scene(void);
#ifdef PG_VIDEO_ACTIVE
void camera_and_video_frame_updates(void);
#endif
// PASS #1: UPDATE (CA, PIXELS, PARTICLES)
void pg_UpdatePass(void);
// PASS #2: PARTICLE RENDERING PASS
void pg_ParticlePass(void);
// PASS #3: COMPOSITION + PING-PONG ECHO: ECHOED MIX OF LAYERS
void pg_MixingPass(void);
// PASS #4: FINAL DISPLAY: MIX OF ECHOED AND NON-ECHOED LAYERS
void pg_MasterPass(void);
// DRAWING A SCENE ON VARIOUS MODALITIES (CURVE, IMAGE, FRAMEBUFFER...)
void pg_draw_scene(DrawingMode mode);

#endif
