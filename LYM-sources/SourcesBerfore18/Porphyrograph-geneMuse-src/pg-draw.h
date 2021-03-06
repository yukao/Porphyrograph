/*! \file pg-draw.h
 * 
 *     Part of this source file is taken from Virtual Choreographer
 *     http://virchor.sourceforge.net/
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
extern GLuint pg_screenMessageBitmap_ID; // nb_attachments=1
extern GLubyte *pg_screenMessageBitmap;

extern GLuint pg_tracks_Pos_Texture_ID[PG_NB_TRACKS];
extern GLfloat *pg_tracks_Pos_Texture[PG_NB_TRACKS];
extern GLuint pg_tracks_Col_Texture_ID[PG_NB_TRACKS];
extern GLfloat *pg_tracks_Col_Texture[PG_NB_TRACKS];
extern GLuint pg_tracks_RadBrushRendmode_Texture_ID[PG_NB_TRACKS];
extern GLfloat *pg_tracks_RadBrushRendmode_Texture[PG_NB_TRACKS];

extern GLuint pg_tracks_Pos_target_Texture_ID[PG_NB_TRACKS];
extern GLfloat *pg_tracks_Pos_target_Texture[PG_NB_TRACKS];
extern GLuint pg_tracks_Col_target_Texture_ID[PG_NB_TRACKS];
extern GLfloat *pg_tracks_Col_target_Texture[PG_NB_TRACKS];
extern GLuint pg_tracks_RadBrushRendmode_target_Texture_ID[PG_NB_TRACKS];
extern GLfloat *pg_tracks_RadBrushRendmode_target_Texture[PG_NB_TRACKS];

extern GLuint pg_tracks_Pos_Col_RadBrushRendmode_target_Texture_ID[PG_NB_TRACKS];
extern GLfloat *pg_tracks_Pos_Col_RadBrushRendmode_target_Texture[PG_NB_TRACKS];

extern GLuint pg_CA_data_table_ID; 
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
extern float mouse_x_deviation;
extern float mouse_y_deviation;

// PG_NB_TRACKS tacks + external tablet
extern float tracks_x[PG_NB_TRACKS + 1];
extern float tracks_y[PG_NB_TRACKS + 1];
extern float tracks_x_prev[PG_NB_TRACKS + 1];
extern float tracks_y_prev[PG_NB_TRACKS + 1];

// PG_NB_TRACKS tacks
extern float tracks_Color_r[PG_NB_TRACKS];
extern float tracks_Color_g[PG_NB_TRACKS];
extern float tracks_Color_b[PG_NB_TRACKS];
extern float tracks_Color_a[PG_NB_TRACKS];
extern int tracks_BrushID[PG_NB_TRACKS];

// PG_NB_TRACKS tacks + external tablet
extern float tracks_RadiusX[PG_NB_TRACKS + 1];
extern float tracks_RadiusY[PG_NB_TRACKS + 1];

// PG_NB_TRACKS tacks
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

// current sensor
extern int currentSensor;
// sensor follows mouse
extern bool sensorFollowMouse_onOff;

// FUNCTIONS
void pg_CA_data_table_values( GLuint textureID , GLfloat * data_table , int width , int height );
void *pg_generateTexture( GLuint *textureID , pg_TextureFormat texture_format ,
		       int sizeX , int sizeY );
bool pg_initFBOTextures( GLuint textureID , int nb_attachments );
bool pg_initFBO( void );
void pg_initRenderingMatrices( void );
void pg_initGeometry_quads( void );
bool pg_initTextures( void );

/// SAVE IMAGE
int writepng(char *filename,
	     int x, int y, int width, int height);

int writejpg(char *filename,
	     int x, int y, int width, int height, int compression);

// intialization of the tack lines used to display a track as a strip
void pg_initGeometry_track_line( int indTrack , bool is_target );

// interpolation between different setups
void sample_setUp_interpolation( void );

#endif
