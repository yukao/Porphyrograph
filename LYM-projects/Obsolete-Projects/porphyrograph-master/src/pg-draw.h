/*! \file pg-draw.h
 * 
 * 
 *     File pg-draw.h
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

#ifndef PG_DRAW_H
#define PG_DRAW_H

/////////////////////////////////////////////////////////////////
// textures bitmaps and associated IDs
extern GLuint pg_screenMessageBitmap_texID; // nb_attachments=1
extern GLubyte *pg_screenMessageBitmap;

extern GLuint pg_tracks_Pos_Texture_texID[PG_NB_TRACKS];
extern GLfloat *pg_tracks_Pos_Texture[PG_NB_TRACKS];
extern GLuint pg_tracks_Col_Texture_texID[PG_NB_TRACKS];
extern GLfloat *pg_tracks_Col_Texture[PG_NB_TRACKS];
extern GLuint pg_tracks_RadBrushRendmode_Texture_texID[PG_NB_TRACKS];
extern GLfloat *pg_tracks_RadBrushRendmode_Texture[PG_NB_TRACKS];

extern GLuint pg_tracks_Pos_target_Texture_texID[PG_NB_TRACKS];
extern GLfloat *pg_tracks_Pos_target_Texture[PG_NB_TRACKS];
extern GLuint pg_tracks_Col_target_Texture_texID[PG_NB_TRACKS];
extern GLfloat *pg_tracks_Col_target_Texture[PG_NB_TRACKS];
extern GLuint pg_tracks_RadBrushRendmode_target_Texture_texID[PG_NB_TRACKS];
extern GLfloat *pg_tracks_RadBrushRendmode_target_Texture[PG_NB_TRACKS];

extern GLuint pg_tracks_Pos_Col_RadBrushRendmode_target_Texture_ID[PG_NB_TRACKS];
extern GLfloat *pg_tracks_Pos_Col_RadBrushRendmode_target_Texture[PG_NB_TRACKS];

////////////////////////////////////////
// geometry: track lines
extern unsigned int line_tracks_vao[PG_NB_TRACKS];
extern unsigned int line_tracks_target_vao[PG_NB_TRACKS];

bool pg_initFBOTextures( GLuint textureID , int nb_attachments );
bool pg_initFBO( void );
void pg_initRenderingMatrices( void );
void pg_initGeometry_quads( void );
bool pg_initTextures( void );

extern GLuint pg_CA_data_table_texID; 
extern GLubyte *pg_CA_data_table;

enum TextureEncoding{ JPG = 0 , PNG , PNGA , PNG_GRAY , PNGA_GRAY , RGB , RAW , EmptyTextureEncoding };
extern const char *TextureEncodingString[EmptyTextureEncoding + 1];

enum pg_TextureFormat{ pg_byte_tex_format = 0 , pg_float_tex_format , Emptypg_TextureFormat };

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
extern GLfloat modelMatrixSensor[16];

extern GLuint drawBuffers[16];

/////////////////////////////////////////////////////////////////
// config variables
// current mouse location (also used for displaying the cursor)
extern float mouse_x;
extern float mouse_y;
extern float mouse_x_prev;
extern float mouse_y_prev;
extern float mouse_x_deviation;
extern float mouse_y_deviation;

extern float tracks_x[PG_NB_TRACKS];
extern float tracks_y[PG_NB_TRACKS];
extern float tracks_x_prev[PG_NB_TRACKS];
extern float tracks_y_prev[PG_NB_TRACKS];
extern float tracks_Color_r[PG_NB_TRACKS];
extern float tracks_Color_g[PG_NB_TRACKS];
extern float tracks_Color_b[PG_NB_TRACKS];
extern float tracks_Color_a[PG_NB_TRACKS];
extern int tracks_BrushID[PG_NB_TRACKS];
extern float tracks_RadiusX[PG_NB_TRACKS];
extern float tracks_RadiusY[PG_NB_TRACKS];

extern float *line_tracks_points[PG_NB_TRACKS];
extern float *line_tracks_target_points[PG_NB_TRACKS];

//////////////////////////////////////////////////////////////////////
// SENSORS
//////////////////////////////////////////////////////////////////////

// sensor translations
// current sensor layout
extern float sensorPositions[ 3 * PG_NB_SENSORS];
// all possible sensor layouts
extern float sensorLayouts[ 3 * PG_NB_SENSORS * PG_NB_MAX_SENSOR_LAYOUTS];

// sensor on off
// current sensor activation pattern
extern bool sensor_onOff[ PG_NB_SENSORS];
extern double sensor_last_activation_time;
// all sensor activation patterns
extern bool sensorActivations[ PG_NB_SENSORS * PG_NB_MAX_SENSOR_ACTIVATIONS];

// sample choice
// current sample choice
extern int sample_choice[ PG_NB_SENSORS];
// all possible sensor layouts
extern int sample_setUps[ PG_NB_MAX_SAMPLE_SETUPS][ PG_NB_SENSORS ];
// sample start playing time for muting after 1 cycle
extern double sample_play_start[ PG_NB_MAX_SAMPLE_SETUPS * PG_NB_SENSORS ];

// current sensor
extern int currentSensor;
// sensor follows mouse
extern bool sensorFollowMouse_onOff;

#ifdef PG_VIDEO_ACTIVE
// video texture ID, image and camera
extern GLuint pg_video_texture_texID;
extern IplImage *pg_video_frame;
extern CvCapture *pg_input_camera;
#endif

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
void pg_CA_data_table_values( GLuint textureID , GLfloat * data_table , int width , int height );
// opengl error
int printOglError( int no );
// error report
void ReportError( char *errorString );

// time management
double RealTime( void );
#ifdef _WIN32
int gettimeofday(struct timeval* tp, void* tzp);
#endif

void MemoryUsage( void );

// my arithmetic classes so that I can use pointers to basic operators
void *pg_generateTexture( GLuint *textureID , pg_TextureFormat texture_format ,
		       int sizeX , int sizeY );
bool pg_initFBOTextures( GLuint textureID , int nb_attachments );
bool pg_initFBO( void );
void pg_initRenderingMatrices( void );
void pg_initGeometry_quads( void );
bool pg_initTextures( void );
#ifdef PG_VIDEO_ACTIVE
int pg_initVideoCapture(void);
#endif

/// SAVE IMAGE
#ifdef WIN32
DWORD WINAPI writepng(LPVOID lpParam);
#else
void* writepng(void * lpParam);
#endif
#ifdef WIN32
DWORD WINAPI writejpg(LPVOID lpParam);
#else
void* writejpg(void * lpParam);
#endif

// intialization of the tack lines used to display a track as a strip
void pg_initGeometry_track_line( int indTrack , bool is_target );
void sample_setUp_interpolation( void );

#endif