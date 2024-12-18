////////////////////////////////////////////////////////////////////
// 
// 
//     Part of this source file is taken from Virtual Choreographer
//     http://virchor.sourceforge.net/
// 
//     File pg-draw.cpp
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

int user_id;
      
#define NB_ATTACHMENTS 3

extern char currentFilename[512];

/////////////////////////////////////////////////////////////////
// config variables
float mouse_x;
float mouse_y;
float mouse_x_prev;
float mouse_y_prev;
float mouse_x_deviation;
float mouse_y_deviation;

// PG_NB_TRACKS tacks + external tablet
float tracks_x[PG_NB_TRACKS + 1];
float tracks_y[PG_NB_TRACKS + 1];
float tracks_x_prev[PG_NB_TRACKS + 1];
float tracks_y_prev[PG_NB_TRACKS + 1];

// PG_NB_TRACKS tacks
float tracks_Color_r[PG_NB_TRACKS];
float tracks_Color_g[PG_NB_TRACKS];
float tracks_Color_b[PG_NB_TRACKS];
float tracks_Color_a[PG_NB_TRACKS];
int tracks_BrushID[PG_NB_TRACKS];

// PG_NB_TRACKS tacks + external tablet
float tracks_RadiusX[PG_NB_TRACKS + 1];
float tracks_RadiusY[PG_NB_TRACKS + 1];

/////////////////////////////////////////////////////////////////
// Projection and view matrices for the shader
GLfloat projMatrix[16];
GLfloat doubleProjMatrix[16];
GLfloat viewMatrix[16];
GLfloat modelMatrix[16];
GLfloat modelMatrixSensor[16];

/////////////////////////////////////////////////////////////////
// textures bitmaps and associated IDs
GLuint pg_screenMessageBitmap_ID = NULL_ID; // nb_attachments=1
GLubyte *pg_screenMessageBitmap = NULL;

GLuint pg_tracks_Pos_Texture_ID[PG_NB_TRACKS] = { NULL_ID , NULL_ID , NULL_ID };
GLfloat *pg_tracks_Pos_Texture[PG_NB_TRACKS] = { NULL , NULL , NULL };
GLuint pg_tracks_Col_Texture_ID[PG_NB_TRACKS] = { NULL_ID , NULL_ID , NULL_ID };
GLfloat *pg_tracks_Col_Texture[PG_NB_TRACKS] = { NULL , NULL , NULL };
GLuint pg_tracks_RadBrushRendmode_Texture_ID[PG_NB_TRACKS] = { NULL_ID , NULL_ID , NULL_ID };
GLfloat *pg_tracks_RadBrushRendmode_Texture[PG_NB_TRACKS] = { NULL , NULL , NULL };

GLuint pg_tracks_Pos_target_Texture_ID[PG_NB_TRACKS] = { NULL_ID , NULL_ID , NULL_ID };
GLfloat *pg_tracks_Pos_target_Texture[PG_NB_TRACKS] = { NULL , NULL , NULL };
GLuint pg_tracks_Col_target_Texture_ID[PG_NB_TRACKS] = { NULL_ID , NULL_ID , NULL_ID };
GLfloat *pg_tracks_Col_target_Texture[PG_NB_TRACKS] = { NULL , NULL , NULL };
GLuint pg_tracks_RadBrushRendmode_target_Texture_ID[PG_NB_TRACKS] = { NULL_ID , NULL_ID , NULL_ID };
GLfloat *pg_tracks_RadBrushRendmode_target_Texture[PG_NB_TRACKS] = { NULL , NULL , NULL };

GLuint pg_CA_data_table_ID = NULL_ID; 
GLubyte *pg_CA_data_table = NULL;

GLuint Font_texture_Rectangle = NULL_ID;
cv::Mat Font_image;

GLuint Pen_texture_2D = NULL_ID;
cv::Mat Pen_image;

GLuint Sensor_texture_rectangle = NULL_ID;
cv::Mat Sensor_image;

GLuint LYMlogo_texture_rectangle = NULL_ID;
cv::Mat LYMlogo_image;

GLuint trackBrush_texture_2D = NULL_ID;
cv::Mat trackBrush_image;

// GLuint trackNoise_texture_Rectangle = NULL_ID;
// cv::Mat trackNoise_image;

GLuint Particle_acceleration_texture_3D = NULL_ID;

const char *TextureEncodingString[EmptyTextureEncoding + 1] = { "jpeg" , "png" , "pnga" , "png_gray" , "pnga_gray" , "rgb", "raw" , "emptyimagetype" };

////////////////////////////////////////
// geometry: quads
int nbFaces = 2; // 6 squares made of 2 triangles
unsigned int quadDraw_vao = 0;
unsigned int quadFinal_vao = 0;
unsigned int quadSensor_vao = 0;

// quad for first and second pass (drawing and compositing)
float quadDraw_points[] = {
  0.0f,         0.0f,  0.0f, //1st triangle
  0.0f,         0.0f,  0.0f,
  0.0f,         0.0f,  0.0f,
  0.0f,         0.0f,  0.0f, //2nd triangle
  0.0f,         0.0f,  0.0f, 
  0.0f,         0.0f,  0.0f
};
float quadDraw_texCoords[] = {
  0.0f,         0.0f, //1st triangle
  0.0f,         0.0f,
  0.0f,         0.0f,
  0.0f,         0.0f, //2nd triangle
  0.0f,         0.0f, 
  0.0f,         0.0f
};

// quad for third pass (display - possibly double screen)
float quadFinal_points[] = {
  0.0f,         0.0f,  0.0f, //1st triangle
  0.0f,         0.0f,  0.0f,
  0.0f,         0.0f,  0.0f,
  0.0f,         0.0f,  0.0f, //2nd triangle
  0.0f,         0.0f,  0.0f, 
  0.0f,         0.0f,  0.0f
};
float quadFinal_texCoords[] = {
  0.0f,         0.0f, //1st triangle
  0.0f,         0.0f,
  0.0f,         0.0f,
  0.0f,         0.0f, //2nd triangle
  0.0f,         0.0f, 
  0.0f,         0.0f
};

// quad for sensors
float quadSensor_points[] = {
  0.0f,         0.0f,  0.0f, //1st triangle
  0.0f,         0.0f,  0.0f,
  0.0f,         0.0f,  0.0f,
  0.0f,         0.0f,  0.0f, //2nd triangle
  0.0f,         0.0f,  0.0f, 
  0.0f,         0.0f,  0.0f
};
float quadSensor_texCoords[] = {
  0.0f,         0.0f, //1st triangle
  0.0f,         0.0f,
  0.0f,         0.0f,
  0.0f,         0.0f, //2nd triangle
  0.0f,         0.0f, 
  0.0f,         0.0f
};

////////////////////////////////////////
// geometry: track lines
unsigned int line_tracks_vao[PG_NB_TRACKS] = {0,0,0};
unsigned int line_tracks_target_vao[PG_NB_TRACKS] = {0,0,0};

float *line_tracks_points[PG_NB_TRACKS] = { NULL , NULL , NULL };
float *line_tracks_target_points[PG_NB_TRACKS] = { NULL , NULL , NULL };

//////////////////////////////////////////////////////////////////////
// SENSORS
//////////////////////////////////////////////////////////////////////

// sensor translations
// current sensor layout
float sensorPositions[ 3 * PG_NB_SENSORS];
// all possible sensor layouts
float sensorLayouts[ 3 * PG_NB_SENSORS * PG_NB_MAX_SENSOR_LAYOUTS];

// sensor on off
// current sensor activation pattern
bool sensor_onOff[ PG_NB_SENSORS];
double sensor_last_activation_time;
// all sensor activation patterns
bool sensorActivations[ PG_NB_SENSORS * PG_NB_MAX_SENSOR_ACTIVATIONS];

// sample choice
// current sample choice
int sample_choice[ PG_NB_SENSORS];
// all possible sensor layouts
int sample_setUps[ PG_NB_MAX_SAMPLE_SETUPS][ PG_NB_SENSORS ] =
  {{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25},
   {26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50},
   {51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75}};
// groups of samples for aliasing with additive samples
int sample_groups[ 18 ][ 4 ] =
  { { 1, 2, 6, 7 },
    { 4, 5, 9, 10 },
    { 16, 17, 21, 22 },
    { 19, 20, 24, 25 },
    { 8, 12, 14, 18 },
    { 3, 11, 15, 23 },
    
    { 26, 27, 31, 32 },
    { 29, 30, 34, 35 },
    { 41, 42, 46, 48 },
    { 44, 45, 49, 50 },
    { 33, 37, 39, 43 },
    { 28, 36, 40, 48 },
    
    { 51, 52, 56, 57 },
    { 54, 55, 59, 60 },
    { 66, 67, 71, 72 },
    { 69, 70, 74, 75 },
    { 58, 62, 64, 68 },
    { 53, 61, 65, 73 } };


// current sensor
int currentSensor = 0;
// sensor follows mouse
bool sensorFollowMouse_onOff = false;

//////////////////////////////////////////////////////////////////////
// TEXT
//////////////////////////////////////////////////////////////////////

GLbyte stb__arial_10_usascii_x[95]={ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,
-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, };
GLubyte stb__arial_10_usascii_y[95]={ 8,1,1,1,1,1,1,1,1,1,1,2,7,5,
7,1,1,1,1,1,1,1,1,1,1,1,3,3,2,3,2,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,9,1,3,1,3,1,3,1,3,1,1,
1,1,1,3,3,3,3,3,3,3,1,3,3,3,3,3,3,1,1,1,4, };
GLubyte stb__arial_10_usascii_w[95]={ 0,2,3,5,5,8,6,2,3,3,4,5,2,3,
2,3,5,4,5,5,5,5,5,5,5,5,2,2,5,5,5,5,9,7,6,7,6,6,6,7,6,2,4,6,
5,7,6,7,6,7,7,6,6,6,6,9,6,6,6,3,3,2,4,7,3,5,5,5,5,5,3,5,5,2,
3,5,2,7,5,5,5,5,4,5,3,5,5,7,5,5,5,3,2,3,5, };
GLubyte stb__arial_10_usascii_h[95]={ 0,7,3,8,8,8,8,3,9,9,4,5,3,2,
1,8,8,7,7,8,7,8,8,7,8,8,5,7,6,4,6,7,9,7,7,8,7,7,7,8,7,7,8,7,
7,7,7,8,7,8,7,8,7,8,7,7,7,7,7,9,8,9,4,1,2,6,8,6,8,6,7,7,7,7,
9,7,7,5,5,6,7,7,5,6,8,6,5,5,5,7,5,9,9,9,2, };
GLubyte stb__arial_10_usascii_s[95]={ 127,80,80,58,76,82,91,84,1,37,72,
95,77,93,101,110,24,104,13,36,109,70,98,60,114,104,73,123,25,121,31,
7,20,115,66,46,97,90,83,9,73,29,53,53,47,39,32,41,22,1,14,
120,1,17,113,103,96,89,82,30,42,34,67,104,97,37,64,49,30,43,66,
70,60,120,16,8,123,113,107,61,54,76,76,19,49,55,101,87,67,1,81,
12,9,5,87, };
GLubyte stb__arial_10_usascii_t[95]={ 1,20,34,1,1,1,1,34,1,1,34,
28,34,34,34,1,11,20,28,11,20,1,1,20,1,1,28,20,28,28,28,
28,1,20,20,11,20,20,20,11,20,20,1,20,20,20,20,1,20,11,20,
1,20,11,11,11,11,11,11,1,11,1,34,34,34,28,1,28,11,28,11,
11,11,11,1,20,11,28,28,28,11,11,28,28,1,28,28,28,28,28,28,
1,1,1,34, };
GLubyte stb__arial_10_usascii_a[95]={ 40,40,51,80,80,127,96,27,
48,48,56,84,40,48,40,40,80,80,80,80,80,80,80,80,
80,80,40,40,84,84,84,80,145,96,96,103,103,96,87,111,
103,40,72,96,80,119,103,111,96,111,103,96,87,103,96,135,
96,96,87,40,40,40,67,80,48,80,80,72,80,80,40,80,
80,32,32,72,32,119,80,80,80,80,48,72,40,80,72,103,
72,72,72,48,37,48,84, };

GLbyte stb__arial_11_usascii_x[95]={ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,
-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, };
GLubyte stb__arial_11_usascii_y[95]={ 8,0,0,0,0,0,0,0,0,0,0,2,7,5,
7,0,0,0,0,0,0,1,0,1,0,0,2,2,2,3,2,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,0,2,0,2,0,2,0,2,0,0,
0,0,0,2,2,2,2,2,2,2,1,2,2,2,2,2,2,0,0,0,3, };
GLubyte stb__arial_11_usascii_w[95]={ 0,2,4,6,6,9,7,2,3,3,4,6,2,3,
2,3,6,3,5,6,6,6,6,6,6,6,2,2,6,6,6,5,10,8,7,7,7,7,6,8,7,2,5,7,
6,8,7,8,7,8,7,7,6,7,7,10,7,7,6,3,3,3,5,7,3,6,6,5,5,6,4,5,5,2,
3,5,2,8,5,6,6,5,4,5,3,5,5,8,5,5,5,4,2,4,6, };
GLubyte stb__arial_11_usascii_h[95]={ 0,8,4,9,10,9,9,4,11,11,4,5,3,1,
1,9,9,8,8,9,8,8,9,7,9,9,6,8,5,3,5,8,11,8,8,9,8,8,8,9,8,8,9,8,
8,8,8,9,8,9,8,9,8,9,8,8,8,8,8,10,9,10,5,1,3,7,9,7,9,7,8,9,8,8,
11,8,8,6,6,7,8,8,6,7,8,7,6,6,6,9,6,11,11,11,3, };
GLubyte stb__arial_11_usascii_s[95]={ 125,88,95,67,41,88,98,100,1,33,90,
76,107,124,125,106,39,9,36,52,29,22,81,60,110,1,125,122,69,110,83,
42,22,13,107,117,1,91,115,69,99,53,63,80,73,64,56,58,45,30,37,
8,24,16,12,1,117,109,102,48,59,37,63,103,103,73,74,67,46,87,78,
24,83,125,18,31,125,100,115,80,89,96,121,48,20,54,94,48,109,52,57,
13,10,5,117, };
GLubyte stb__arial_11_usascii_t[95]={ 10,23,40,1,1,1,1,40,1,1,40,
40,40,40,30,1,13,32,32,13,32,32,1,32,1,13,23,23,40,40,40,
32,1,32,23,1,32,23,23,13,23,23,13,23,23,23,23,1,23,13,23,
13,23,13,23,23,13,13,13,1,13,1,40,44,40,32,1,32,13,32,13,
13,13,1,1,23,13,32,32,32,13,13,32,32,23,32,32,40,32,1,40,
1,1,1,40, };
GLubyte stb__arial_11_usascii_a[95]={ 44,44,56,88,88,140,105,30,
52,52,61,92,44,52,44,44,88,88,88,88,88,88,88,88,
88,88,44,44,92,92,92,88,160,105,105,114,114,105,96,123,
114,44,79,105,88,131,114,123,105,123,114,105,96,114,105,149,
105,105,96,44,44,44,74,88,52,88,88,79,88,88,44,88,
88,35,35,79,35,131,88,88,88,88,52,79,44,88,79,114,
79,79,79,53,41,53,92, };


GLbyte stb__arial_12_usascii_x[95]={ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,1,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,
-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, };
GLubyte stb__arial_12_usascii_y[95]={ 9,1,1,1,0,1,1,1,1,1,1,2,7,5,
7,1,1,1,1,1,1,1,1,1,1,1,3,3,2,3,2,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,10,1,3,1,3,1,3,1,3,1,1,
1,1,1,3,3,3,3,3,3,3,1,3,3,3,3,3,3,1,1,1,4, };
GLubyte stb__arial_12_usascii_w[95]={ 0,3,4,6,6,9,7,2,4,4,4,6,3,4,
3,3,6,4,6,6,6,6,6,6,6,6,3,3,6,6,6,6,11,9,7,8,8,7,7,8,7,2,5,8,
6,9,7,8,7,8,8,7,7,7,8,11,8,8,7,3,3,3,5,8,3,6,6,6,6,6,4,6,6,2,
3,6,2,9,6,6,6,6,4,5,3,6,6,8,6,6,6,4,2,4,6, };
GLubyte stb__arial_12_usascii_h[95]={ 0,8,4,9,11,9,9,4,11,11,4,6,4,2,
2,9,9,8,8,9,8,9,9,8,9,9,6,8,6,4,6,8,11,8,8,9,8,8,8,9,8,8,9,8,
8,8,8,9,8,9,8,9,8,9,8,8,8,8,8,11,9,11,5,2,2,7,9,7,9,7,8,9,8,8,
11,8,8,6,6,7,9,9,6,7,9,7,6,6,6,9,6,11,11,11,3, };
GLubyte stb__arial_12_usascii_s[95]={ 125,116,98,89,1,96,106,95,50,29,86,
55,91,110,123,121,41,1,56,55,70,69,114,31,85,10,62,77,66,79,18,
63,34,46,38,1,22,14,6,76,120,113,100,104,97,87,79,66,71,17,55,
33,47,92,31,19,10,1,115,46,106,21,73,114,110,88,62,101,48,115,110,
26,40,125,25,64,123,8,1,108,82,75,122,95,55,81,41,25,48,59,34,
16,13,8,103, };
GLubyte stb__arial_12_usascii_t[95]={ 10,21,41,1,1,1,1,41,1,1,41,
41,41,44,41,1,13,32,32,11,32,11,1,32,11,13,41,32,41,41,41,
32,1,32,32,13,32,32,32,11,21,21,11,21,21,21,21,1,21,13,21,
13,23,11,23,23,23,23,11,1,11,1,41,41,41,32,11,32,13,32,11,
13,23,1,1,21,11,41,41,32,1,1,32,32,1,32,41,41,41,1,41,
1,1,1,41, };
GLubyte stb__arial_12_usascii_a[95]={ 48,48,61,96,96,153,115,33,
57,57,67,100,48,57,48,48,96,96,96,96,96,96,96,96,
96,96,48,48,100,100,100,96,174,115,115,124,124,115,105,134,
124,48,86,115,96,143,124,134,115,134,124,115,105,124,115,162,
115,115,105,48,48,48,81,96,57,96,96,86,96,96,48,96,
96,38,38,86,38,143,96,96,96,96,57,86,48,96,86,124,
86,86,86,57,45,57,100, };

GLbyte stb__arial_13_usascii_x[95]={ 0,1,0,0,0,0,0,0,0,0,0,0,0,0,
1,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,1,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,
-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0, };
GLubyte stb__arial_13_usascii_y[95]={ 10,1,1,1,0,1,1,1,1,1,1,3,8,6,
8,1,1,1,1,1,1,1,1,1,1,1,3,3,3,4,3,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,11,1,3,1,3,1,3,1,3,1,1,
1,1,1,3,3,3,3,3,3,3,1,3,3,3,3,3,3,1,1,1,4, };
GLubyte stb__arial_13_usascii_w[95]={ 0,2,4,7,6,10,8,2,4,4,5,7,3,4,
2,4,6,4,6,6,6,7,6,6,6,6,2,3,7,7,7,6,12,9,8,8,8,8,7,9,8,2,5,8,
7,9,8,9,8,9,9,8,7,8,8,11,8,8,7,4,4,3,6,8,3,6,6,6,6,6,4,6,6,2,
3,6,2,9,6,7,7,6,5,6,4,6,6,9,6,6,6,4,1,4,7, };
GLubyte stb__arial_13_usascii_h[95]={ 0,9,4,10,12,10,10,4,12,12,5,6,4,2,
2,10,10,9,9,10,9,10,10,9,10,10,7,9,6,4,6,9,12,9,9,10,9,9,9,10,9,9,10,9,
9,9,9,10,9,10,9,10,9,10,9,9,9,9,9,12,10,12,6,2,3,8,10,8,10,8,9,10,9,9,
12,9,9,7,7,8,10,10,7,8,10,8,7,7,7,10,7,12,12,12,3, };
GLubyte stb__arial_13_usascii_s[95]={ 56,125,1,93,1,101,112,123,51,28,105,
97,111,30,27,10,48,19,76,62,90,76,121,50,94,15,47,97,89,115,81,
83,33,66,57,1,41,32,24,84,10,122,110,1,114,104,95,68,83,22,66,
39,55,101,39,27,18,9,1,46,116,20,74,18,6,108,69,1,55,16,121,
32,48,63,24,76,92,30,23,8,85,78,122,115,56,101,40,50,67,61,60,
15,13,8,10, };
GLubyte stb__arial_13_usascii_t[95]={ 12,25,54,1,1,1,1,45,1,1,45,
45,45,54,54,14,14,35,35,14,35,14,1,35,14,14,45,35,45,45,45,
35,1,35,35,14,35,35,35,14,35,25,14,35,25,25,25,1,25,14,25,
14,25,14,25,25,25,25,25,1,14,1,45,54,54,35,14,45,14,45,14,
14,25,25,1,25,25,45,45,45,1,1,35,35,1,35,45,45,45,1,45,
1,1,1,54, };
GLubyte stb__arial_13_usascii_a[95]={ 52,52,66,104,104,166,124,36,
62,62,72,109,52,62,52,52,104,104,104,104,104,104,104,104,
104,104,52,52,109,109,109,104,189,124,124,134,134,124,114,145,
134,52,93,124,104,155,134,145,124,145,134,124,114,134,124,176,
124,124,114,52,52,52,87,104,62,104,104,93,104,104,52,104,
104,41,41,93,41,155,104,104,104,104,62,93,52,104,93,134,
93,93,93,62,48,62,109, };

GLbyte stb__arial_14_usascii_x[95]={ 0,1,0,0,0,0,0,0,0,0,0,0,1,0,
1,0,0,1,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,-1,0,0,0,0,1,0,1,1,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,
-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0, };
GLubyte stb__arial_14_usascii_y[95]={ 11,2,2,1,1,1,1,2,1,1,1,3,9,7,
9,1,1,1,1,1,2,2,1,2,1,1,4,4,3,4,3,1,1,2,2,1,2,2,2,1,2,2,2,2,
2,2,2,1,2,1,2,1,2,2,2,2,2,2,2,2,1,2,1,12,1,4,2,4,2,4,1,4,2,2,
2,2,2,4,4,4,4,4,4,4,2,4,4,4,4,4,4,1,1,1,5, };
GLubyte stb__arial_14_usascii_w[95]={ 0,2,4,7,7,11,9,2,4,4,5,7,2,4,
2,4,7,4,7,7,7,7,7,7,7,7,2,2,7,7,7,7,13,10,8,9,9,8,7,9,8,2,6,9,
7,10,9,10,8,10,9,8,8,9,9,12,9,9,8,4,4,3,6,9,3,7,7,7,7,7,4,7,7,2,
3,7,2,10,7,7,7,7,5,6,4,7,7,9,7,7,6,4,2,4,7, };
GLubyte stb__arial_14_usascii_h[95]={ 0,9,4,11,12,11,11,4,13,13,5,7,4,2,
2,11,11,10,10,11,9,10,11,9,11,11,7,9,7,5,7,10,13,9,9,11,9,9,9,11,9,9,10,9,
9,9,9,11,9,11,9,11,9,10,9,9,9,9,9,12,11,12,6,2,3,8,10,8,10,8,10,10,9,9,
12,9,9,7,7,8,10,10,7,8,10,8,7,7,7,10,7,13,13,13,3, };
GLubyte stb__arial_14_usascii_s[95]={ 59,120,123,70,42,78,108,57,1,19,48,
104,54,123,72,118,1,9,22,9,1,1,17,110,90,45,59,107,62,40,88,
115,24,49,98,98,80,71,63,53,40,123,108,30,22,11,1,59,111,25,88,
36,69,30,49,98,78,59,40,50,123,55,33,75,60,27,84,51,76,12,71,
63,90,60,38,118,9,112,12,35,92,100,20,20,123,43,70,78,96,14,26,
14,11,6,64, };
GLubyte stb__arial_14_usascii_t[95]={ 13,27,48,1,1,1,1,57,1,1,57,
48,57,53,57,1,15,27,27,15,48,27,15,37,1,15,48,37,48,57,48,
15,1,37,37,1,37,37,37,15,37,27,15,38,38,38,38,1,27,15,27,
15,27,27,27,27,27,27,27,1,1,1,57,57,57,48,15,48,15,48,15,
15,37,37,1,37,48,48,57,48,15,15,57,48,15,48,48,48,48,27,57,
1,1,1,57, };
GLubyte stb__arial_14_usascii_a[95]={ 56,56,71,112,112,178,134,38,
67,67,78,117,56,67,56,56,112,112,112,112,112,112,112,112,
112,112,56,56,117,117,117,112,204,134,134,145,145,134,122,156,
145,56,100,134,112,167,145,156,134,156,145,134,122,145,134,189,
134,134,122,56,56,56,94,112,67,112,112,100,112,112,56,112,
112,45,45,100,45,167,112,112,112,112,67,100,56,112,100,145,
100,100,100,67,52,67,117, };

GLbyte stb__arial_15_usascii_x[95]={ 0,1,0,0,0,0,0,0,0,0,0,0,1,0,
1,0,0,1,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,-1,0,0,1,1,1,0,1,1,0,0,
0,0,1,0,1,0,1,0,0,1,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,
-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0, };
GLubyte stb__arial_15_usascii_y[95]={ 12,2,2,2,1,2,2,2,2,2,2,4,10,7,
10,2,2,2,2,2,2,2,2,2,2,2,5,5,4,5,4,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,13,2,4,2,4,2,4,2,4,2,2,
2,2,2,4,4,4,4,4,4,4,2,5,5,5,5,5,5,2,2,2,6, };
GLubyte stb__arial_15_usascii_w[95]={ 0,2,5,8,7,12,9,2,4,4,5,8,2,5,
2,4,7,5,7,7,7,7,7,7,7,7,2,2,8,8,8,7,14,10,9,10,8,8,7,10,8,2,6,9,
7,11,8,10,8,10,9,9,8,8,9,13,9,9,8,4,4,3,6,9,4,7,7,7,7,7,5,7,7,3,
4,7,3,11,7,7,7,7,5,7,4,7,7,10,7,7,7,5,2,5,8, };
GLubyte stb__arial_15_usascii_h[95]={ 0,10,4,11,13,11,11,4,13,13,5,7,4,3,
2,11,11,10,10,11,10,11,11,10,11,11,7,9,7,5,7,10,13,10,10,11,10,10,10,11,10,10,11,10,
10,10,10,11,10,11,10,11,10,11,10,10,10,10,10,13,11,13,6,2,3,9,11,9,11,9,10,11,10,10,
13,10,10,8,8,9,11,11,8,9,11,8,7,7,7,10,7,13,13,13,3, };
GLubyte stb__arial_15_usascii_s[95]={ 127,68,60,79,1,104,117,66,58,29,54,
1,69,72,102,1,25,12,105,67,97,83,59,43,109,33,100,123,21,45,103,
18,34,1,113,14,88,79,71,10,59,1,102,33,25,13,4,68,117,91,103,
41,86,1,72,58,48,38,29,49,117,54,38,92,78,42,6,34,75,58,122,
51,21,82,24,95,113,66,92,50,96,88,78,26,63,84,30,10,112,51,120,
18,15,9,83, };
GLubyte stb__arial_15_usascii_t[95]={ 1,39,61,1,1,1,1,61,1,1,61,
61,61,61,61,15,15,50,39,15,39,15,15,39,15,15,50,39,61,61,50,
50,1,50,39,15,39,39,39,27,39,39,15,39,39,39,39,1,27,15,27,
15,27,27,27,27,27,27,27,1,15,1,61,61,61,50,15,50,15,50,15,
15,27,27,1,27,27,50,50,50,1,1,50,50,1,50,61,61,50,39,50,
1,1,1,61, };
GLubyte stb__arial_15_usascii_a[95]={ 60,60,76,119,119,191,143,41,
72,72,84,125,60,72,60,60,119,119,119,119,119,119,119,119,
119,119,60,60,125,125,125,119,218,143,143,155,155,143,131,167,
155,60,107,143,119,179,155,167,143,167,155,143,131,155,143,203,
143,143,131,60,60,60,101,119,72,119,119,107,119,119,60,119,
119,48,48,107,48,179,119,119,119,119,72,107,60,119,107,155,
107,107,107,72,56,72,125, };

GLbyte stb__arial_16_usascii_x[95]={ 0,1,0,0,0,0,0,0,0,0,0,0,1,0,
1,0,0,1,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,-1,1,0,1,1,1,0,1,1,0,1,
1,1,1,0,1,0,1,0,0,1,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,
-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0, };
GLubyte stb__arial_16_usascii_y[95]={ 12,1,1,1,0,1,1,1,1,1,1,3,10,7,
10,1,1,1,1,1,1,1,1,1,1,1,4,4,3,4,3,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,13,1,4,1,4,1,4,1,4,1,1,
1,1,1,4,4,4,4,4,4,4,1,4,4,4,4,4,4,1,1,1,5, };
GLubyte stb__arial_16_usascii_w[95]={ 0,2,5,8,8,12,10,3,5,5,6,8,2,5,
2,4,8,5,8,8,8,8,8,8,8,8,2,2,8,8,8,8,15,11,8,10,9,8,8,11,9,2,7,9,
7,10,9,11,8,11,10,9,9,9,10,14,10,10,9,4,4,4,7,10,4,8,8,8,7,8,5,8,7,3,
4,8,3,12,7,8,8,7,5,7,4,7,7,11,8,8,7,5,2,5,8, };
GLubyte stb__arial_16_usascii_h[95]={ 0,11,5,12,14,12,12,5,15,15,5,8,5,2,
2,12,12,11,11,12,11,12,12,11,12,12,8,11,8,6,8,11,15,11,11,12,11,11,11,12,11,11,12,11,
11,11,11,12,11,12,11,12,11,12,11,11,11,11,11,14,12,14,7,2,3,9,12,9,12,9,11,12,11,11,
15,11,11,8,8,9,11,11,8,9,12,9,8,8,8,12,8,15,15,15,4, };
GLubyte stb__arial_16_usascii_s[95]={ 125,125,109,111,54,1,14,105,1,43,95,
60,102,82,79,120,85,93,31,102,49,13,94,1,25,45,124,125,69,86,14,
40,27,19,10,34,117,108,99,1,83,125,103,73,65,54,44,82,31,54,11,
75,1,22,102,87,76,65,55,63,120,49,78,68,115,66,111,83,94,101,41,
66,113,121,22,22,40,1,110,92,32,47,118,75,68,58,43,23,51,73,35,
16,13,7,115, };
GLubyte stb__arial_16_usascii_t[95]={ 13,17,67,1,1,17,17,67,1,1,67,
67,67,14,14,1,17,43,55,17,55,30,1,55,17,17,55,29,67,67,67,
55,1,55,55,17,43,43,43,30,43,1,1,43,43,43,43,1,43,17,43,
17,43,30,30,30,30,30,30,1,17,1,67,14,72,55,17,55,17,55,30,
17,30,30,1,43,43,67,55,55,30,30,55,55,1,55,67,67,67,1,67,
1,1,1,67, };
GLubyte stb__arial_16_usascii_a[95]={ 64,64,81,127,127,204,153,44,
76,76,89,134,64,76,64,64,127,127,127,127,127,127,127,127,
127,127,64,64,134,134,134,127,233,153,153,165,165,153,140,178,
165,64,115,153,127,191,165,178,153,178,165,153,140,165,153,216,
153,153,140,64,64,64,108,127,76,127,127,115,127,127,64,127,
127,51,51,115,51,191,127,127,127,127,76,115,64,127,115,165,
115,115,115,77,60,77,134, };

GLbyte stb__arial_17_usascii_x[95]={ 0,1,0,0,0,0,0,0,0,0,0,0,1,0,
1,0,0,1,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,-1,1,0,1,1,1,0,1,1,0,1,
1,1,1,0,1,0,1,0,0,1,0,0,0,0,0,1,0,0,0,-1,0,0,0,0,0,0,0,0,1,1,
-1,1,0,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0, };
GLubyte stb__arial_17_usascii_y[95]={ 13,2,2,1,1,1,1,2,1,1,1,4,11,8,
11,1,2,2,2,2,2,2,2,2,2,2,5,5,3,5,3,1,1,2,2,1,2,2,2,1,2,2,2,2,
2,2,2,1,2,1,2,1,2,2,2,2,2,2,2,2,1,2,1,15,2,4,2,4,2,4,1,4,2,2,
2,2,2,4,4,4,4,4,4,4,2,5,5,5,5,5,5,1,1,1,6, };
GLubyte stb__arial_17_usascii_w[95]={ 0,2,5,9,8,13,10,3,5,5,6,9,2,5,
2,5,8,5,8,8,8,8,8,8,8,8,2,2,9,9,9,8,15,12,9,11,10,9,8,11,9,2,7,10,
7,11,9,12,9,12,10,10,9,9,11,15,11,11,9,3,5,4,7,10,4,8,8,8,8,8,5,8,7,2,
4,7,3,11,7,8,7,8,6,8,5,8,8,11,8,8,8,5,2,5,9, };
GLubyte stb__arial_17_usascii_h[95]={ 0,11,4,13,14,13,13,4,16,16,6,8,5,2,
2,13,12,11,11,12,11,12,12,11,12,12,8,11,9,5,9,12,16,11,11,13,11,11,11,13,11,11,12,11,
11,11,11,13,11,13,11,13,11,12,11,11,11,11,11,15,13,15,7,2,3,10,12,10,12,10,12,13,11,11,
15,11,11,9,9,10,13,13,9,10,12,9,8,8,8,12,8,16,16,16,3, };
GLubyte stb__arial_17_usascii_s[95]={ 127,71,121,80,58,90,1,11,1,22,114,
78,125,30,125,69,62,42,45,19,63,43,89,54,107,1,125,122,1,1,20,
28,28,32,22,21,11,1,113,33,99,124,75,78,70,58,89,67,32,45,13,
58,114,52,102,86,74,1,48,54,121,49,106,36,25,116,116,107,98,98,83,
12,72,125,44,24,109,30,49,89,113,104,42,80,37,11,97,66,57,10,88,
16,13,7,15, };
GLubyte stb__arial_17_usascii_t[95]={ 1,32,69,1,1,1,18,79,1,1,69,
69,54,79,60,18,32,45,57,32,57,32,18,57,18,32,45,45,69,79,69,
32,1,57,57,18,57,57,45,18,45,32,18,45,45,45,45,1,45,18,45,
18,32,32,32,32,32,45,45,1,1,1,69,79,79,57,18,57,18,57,18,
18,57,18,1,45,45,69,69,57,1,1,69,57,32,69,69,69,69,32,69,
1,1,1,79, };
GLubyte stb__arial_17_usascii_a[95]={ 68,68,86,135,135,216,162,46,
81,81,95,142,68,81,68,68,135,135,135,135,135,135,135,135,
135,135,68,68,142,142,142,135,247,162,162,176,176,162,149,189,
176,68,122,162,135,203,176,189,162,189,176,162,149,176,162,230,
162,162,149,68,68,68,114,135,81,135,135,122,135,135,68,135,
135,54,54,122,54,203,135,135,135,135,81,122,68,135,122,176,
122,122,122,81,63,81,142, };

GLbyte stb__arial_18_usascii_x[95]={ 0,1,0,0,0,0,0,0,0,0,0,0,1,0,
1,0,0,1,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,-1,1,0,1,1,1,0,1,1,0,1,
1,1,1,0,1,0,1,0,0,1,0,0,0,0,0,1,0,0,0,-1,0,0,1,0,0,0,0,0,1,1,
-1,1,1,1,1,0,1,0,1,0,0,1,0,0,0,0,0,0,1,0,0, };
GLubyte stb__arial_18_usascii_y[95]={ 14,2,2,2,1,2,2,2,2,2,2,4,12,9,
12,2,2,2,2,2,2,2,2,2,2,2,5,5,4,5,4,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,16,2,5,2,5,2,5,2,5,2,2,
2,2,2,5,5,5,5,5,5,5,2,5,5,5,5,5,5,2,2,2,7, };
GLubyte stb__arial_18_usascii_w[95]={ 0,3,5,9,9,14,11,3,5,5,6,9,3,5,
3,5,9,6,9,9,9,9,9,9,9,9,3,3,9,9,9,9,16,12,9,11,10,9,9,12,10,3,7,10,
8,12,10,12,10,12,11,10,10,10,11,16,11,11,10,4,5,4,8,11,4,9,8,8,8,9,6,8,7,2,
4,7,2,12,7,9,8,8,5,8,5,7,8,12,8,8,8,6,2,5,9, };
GLubyte stb__arial_18_usascii_h[95]={ 0,12,5,13,15,13,13,5,16,16,6,9,5,2,
2,13,13,12,12,13,12,13,13,12,13,13,9,12,9,6,9,12,16,12,12,13,12,12,12,13,12,12,13,12,
12,12,12,13,12,13,12,13,12,13,12,12,12,12,12,16,13,16,7,2,3,10,13,10,13,10,12,13,12,12,
16,12,12,9,9,10,13,13,9,10,13,10,9,9,9,13,9,16,16,16,3, };
GLubyte stb__arial_18_usascii_s[95]={ 127,117,49,117,61,1,16,45,1,38,34,
116,41,86,82,50,99,121,76,118,96,10,28,43,33,56,1,106,5,24,66,
86,44,63,53,38,32,22,12,20,1,113,54,102,93,80,69,86,55,66,35,
88,21,43,1,110,98,86,75,33,62,23,15,70,55,118,1,10,109,29,68,
79,13,32,28,47,66,47,39,19,108,99,60,1,71,110,98,76,107,77,89,
16,13,7,60, };
GLubyte stb__arial_18_usascii_t[95]={ 1,46,83,1,1,18,18,83,1,1,83,
72,83,83,83,18,18,46,59,18,59,32,18,59,32,18,83,59,83,83,72,
59,1,59,59,18,59,59,59,32,59,46,32,46,46,46,46,1,46,18,46,
18,46,32,46,32,32,32,32,1,32,1,83,83,83,59,32,72,18,72,32,
18,46,46,1,46,46,72,72,72,1,1,72,72,1,59,72,72,72,1,72,
1,1,1,83, };
GLubyte stb__arial_18_usascii_a[95]={ 72,72,92,143,143,229,172,49,
86,86,100,151,72,86,72,72,143,143,143,143,143,143,143,143,
143,143,72,72,151,151,151,143,255,172,172,186,186,172,157,201,
// 143,143,72,72,151,151,151,143,262,172,172,186,186,172,157,201,
186,72,129,172,143,215,186,201,172,201,186,172,157,186,172,243,
172,172,157,72,72,72,121,143,86,143,143,129,143,143,72,143,
143,57,57,129,57,215,143,143,143,143,86,129,72,143,129,186,
129,129,129,86,67,86,151, };

/////////////////////////////////////////////////////////////////
// GEOMETRY INITIALIZATION
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
// INTIALIZATION OF THE TWO QUADS USED FOR DISPLAY: DRAWING AND COMPOSITION QUADS
void pg_initGeometry_quads( void ) {
  /////////////////////////////////////////////////////////////////////
  // QUAD FOR DRAWING AND COMPOSITION
  // point positions and texture coordinates
  quadDraw_points[1] = (float)windowHeight;
  quadDraw_points[3] = (float)singleWindowWidth;
  quadDraw_points[10] = (float)windowHeight;
  quadDraw_points[12] = (float)singleWindowWidth;
  quadDraw_points[13] = (float)windowHeight;
  quadDraw_points[15] = (float)singleWindowWidth;

  quadDraw_texCoords[1] = (float)windowHeight;
  quadDraw_texCoords[2] = (float)singleWindowWidth;
  quadDraw_texCoords[7] = (float)windowHeight;
  quadDraw_texCoords[8] = (float)singleWindowWidth;
  quadDraw_texCoords[9] = (float)windowHeight;
  quadDraw_texCoords[10] = (float)singleWindowWidth;

  // vertex buffer objects and vertex array
  unsigned int quadDraw_points_vbo = 0;
  glGenBuffers (1, &quadDraw_points_vbo);
  glBindBuffer (GL_ARRAY_BUFFER, quadDraw_points_vbo);
  glBufferData (GL_ARRAY_BUFFER, 
		3 * 3 * nbFaces * sizeof (float), 
		quadDraw_points, 
		GL_STATIC_DRAW);
  unsigned int quadDraw_texCoord_vbo = 0;
  glGenBuffers (1, &quadDraw_texCoord_vbo);
  glBindBuffer (GL_ARRAY_BUFFER, quadDraw_texCoord_vbo);
  glBufferData (GL_ARRAY_BUFFER,
		2 * 3 * nbFaces * sizeof (float),
		quadDraw_texCoords,
		GL_STATIC_DRAW);

  quadDraw_vao = 0;
  glGenVertexArrays (1, &quadDraw_vao);
  glBindVertexArray (quadDraw_vao);
  // vertex positions are location 0
  glBindBuffer (GL_ARRAY_BUFFER, quadDraw_points_vbo);
  glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
  glEnableVertexAttribArray (0);

  // texture coordinates are location 1
  glBindBuffer (GL_ARRAY_BUFFER, quadDraw_texCoord_vbo);
  glVertexAttribPointer (1, 2, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
  glEnableVertexAttribArray (1); // don't forget this!

  printOglError( 22 );

  /////////////////////////////////////////////////////////////////////
  // QUAD FOR FINAL RENDERING (POSSIBLY DOUBLE)
  // point positions and texture coordinates
  quadFinal_points[1] = (float)windowHeight;
  quadFinal_points[3] = (float)doubleWindowWidth;
  quadFinal_points[10] = (float)windowHeight;
  quadFinal_points[12] = (float)doubleWindowWidth;
  quadFinal_points[13] = (float)windowHeight;
  quadFinal_points[15] = (float)doubleWindowWidth;

  quadFinal_texCoords[1] = (float)windowHeight;
  quadFinal_texCoords[2] = (float)doubleWindowWidth;
  quadFinal_texCoords[7] = (float)windowHeight;
  quadFinal_texCoords[8] = (float)doubleWindowWidth;
  quadFinal_texCoords[9] = (float)windowHeight;
  quadFinal_texCoords[10] = (float)doubleWindowWidth;

  // vertex buffer objects and vertex array
  unsigned int quadFinal_points_vbo = 0;
  glGenBuffers (1, &quadFinal_points_vbo);
  glBindBuffer (GL_ARRAY_BUFFER, quadFinal_points_vbo);
  glBufferData (GL_ARRAY_BUFFER, 
		3 * 3 * nbFaces * sizeof (float), 
		quadFinal_points, 
		GL_STATIC_DRAW);
  unsigned int quadFinal_texCoord_vbo = 0;
  glGenBuffers (1, &quadFinal_texCoord_vbo);
  glBindBuffer (GL_ARRAY_BUFFER, quadFinal_texCoord_vbo);
  glBufferData (GL_ARRAY_BUFFER,
		2 * 3 * nbFaces * sizeof (float),
		quadFinal_texCoords,
		GL_STATIC_DRAW);

  quadFinal_vao = 0;
  glGenVertexArrays (1, &quadFinal_vao);
  glBindVertexArray (quadFinal_vao);
  // vertex positions are location 0
  glBindBuffer (GL_ARRAY_BUFFER, quadFinal_points_vbo);
  glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
  glEnableVertexAttribArray (0);

  // texture coordinates are location 1
  glBindBuffer (GL_ARRAY_BUFFER, quadFinal_texCoord_vbo);
  glVertexAttribPointer (1, 2, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
  glEnableVertexAttribArray (1); // don't forget this!

  printf("Geometry initialized %dx%d & %dx%d\n" , singleWindowWidth , windowHeight ,
	  doubleWindowWidth , windowHeight );

  printOglError( 23 );

  /////////////////////////////////////////////////////////////////////
  // QUADS FOR SENSORS
  // point positions and texture coordinates
  quadSensor_points[0] = (float)-PG_SENSOR_GEOMETRY_WIDTH;
  quadSensor_points[1] = (float)PG_SENSOR_GEOMETRY_WIDTH;
  quadSensor_points[3] = (float)PG_SENSOR_GEOMETRY_WIDTH;
  quadSensor_points[4] = (float)-PG_SENSOR_GEOMETRY_WIDTH;
  quadSensor_points[6] = (float)-PG_SENSOR_GEOMETRY_WIDTH;
  quadSensor_points[7] = (float)-PG_SENSOR_GEOMETRY_WIDTH;
  quadSensor_points[9] = (float)-PG_SENSOR_GEOMETRY_WIDTH;
  quadSensor_points[10] = (float)PG_SENSOR_GEOMETRY_WIDTH;
  quadSensor_points[12] = (float)PG_SENSOR_GEOMETRY_WIDTH;
  quadSensor_points[13] = (float)PG_SENSOR_GEOMETRY_WIDTH;
  quadSensor_points[15] = (float)PG_SENSOR_GEOMETRY_WIDTH;
  quadSensor_points[16] = (float)-PG_SENSOR_GEOMETRY_WIDTH;

  quadSensor_texCoords[1] = (float)PG_SENSOR_TEXTURE_WIDTH;
  quadSensor_texCoords[2] = (float)PG_SENSOR_TEXTURE_WIDTH;
  quadSensor_texCoords[7] = (float)PG_SENSOR_TEXTURE_WIDTH;
  quadSensor_texCoords[8] = (float)PG_SENSOR_TEXTURE_WIDTH;
  quadSensor_texCoords[9] = (float)PG_SENSOR_TEXTURE_WIDTH;
  quadSensor_texCoords[10] = (float)PG_SENSOR_TEXTURE_WIDTH;

  ///////////////////////////////////////////////////////////////////////////////////////
  // vertex buffer objects and vertex array
  ///////////////////////////////////////////////////////////////////////////////////////
  unsigned int quadSensor_points_vbo = 0;
  glGenBuffers (1, &quadSensor_points_vbo);
  glBindBuffer (GL_ARRAY_BUFFER, quadSensor_points_vbo);
  glBufferData (GL_ARRAY_BUFFER, 
		3 * 3 * nbFaces * sizeof (float), 
		quadSensor_points, 
		GL_STATIC_DRAW);
  unsigned int quadSensor_texCoord_vbo = 0;
  glGenBuffers (1, &quadSensor_texCoord_vbo);
  glBindBuffer (GL_ARRAY_BUFFER, quadSensor_texCoord_vbo);
  glBufferData (GL_ARRAY_BUFFER,
		2 * 3 * nbFaces * sizeof (float),
		quadSensor_texCoords,
		GL_STATIC_DRAW);

  quadSensor_vao = 0;
  glGenVertexArrays (1, &quadSensor_vao);
  glBindVertexArray (quadSensor_vao);
  // vertex positions are location 0
  glBindBuffer (GL_ARRAY_BUFFER, quadSensor_points_vbo);
  glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
  glEnableVertexAttribArray (0);

  // texture coordinates are location 1
  glBindBuffer (GL_ARRAY_BUFFER, quadSensor_texCoord_vbo);
  glVertexAttribPointer (1, 2, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
  glEnableVertexAttribArray (1); // don't forget this!

  // printf("Sensor size  %d\n" , PG_SENSOR_GEOMETRY_WIDTH );

  printOglError( 23 );

  ///////////////////////////////////////////////////////////////////////////////////////
  // sensor layouts
  ///////////////////////////////////////////////////////////////////////////////////////
  int indLayout;
  int indSens;
  // square grid
  indLayout = 0;
  for( indSens = 0 ; indSens < PG_NB_SENSORS ; indSens++ ) {
    int heightInt = indSens / 5 - 2;
    int widthInt = indSens % 5 - 2;
    sensorLayouts[ indLayout * 3 * PG_NB_SENSORS + 3 * indSens ] = singleWindowWidth/2.0f + widthInt * 150.0f;
    sensorLayouts[ indLayout * 3 * PG_NB_SENSORS + 3 * indSens + 1 ] = windowHeight/2.0f + heightInt * 150.0f;
    sensorLayouts[ indLayout * 3 * PG_NB_SENSORS + 3 * indSens + 2 ] = 0.1f;
  }

  // circular grid
  indLayout = 1;
  // central sensor
  indSens = 0;
  sensorLayouts[ indLayout * 3 * PG_NB_SENSORS + 3 * indSens ] = singleWindowWidth/2.0f;
  sensorLayouts[ indLayout * 3 * PG_NB_SENSORS + 3 * indSens + 1 ] = windowHeight/2.0f;
  sensorLayouts[ indLayout * 3 * PG_NB_SENSORS + 3 * indSens + 2 ] = 0.1f;
  for( indSens = 1 ; indSens < 9 ; indSens++ ) {
    float radius = windowHeight / 5.0f;
    sensorLayouts[ indLayout * 3 * PG_NB_SENSORS + 3 * indSens ] = singleWindowWidth/2.0f + radius * (float)cos( indSens * 2.0 * M_PI / 8.0f );
    sensorLayouts[ indLayout * 3 * PG_NB_SENSORS + 3 * indSens + 1 ] = windowHeight/2.0f + radius * (float)sin( indSens * 2.0 * M_PI / 8.0f );
    sensorLayouts[ indLayout * 3 * PG_NB_SENSORS + 3 * indSens + 2 ] = 0.1f;
  }
  for( indSens = 9 ; indSens < PG_NB_SENSORS ; indSens++ ) {
    float radius = windowHeight / 3.0f;
    sensorLayouts[ indLayout * 3 * PG_NB_SENSORS + 3 * indSens ] = singleWindowWidth/2.0f + radius * (float)cos( indSens * 2.0 * M_PI / 16.0f );
    sensorLayouts[ indLayout * 3 * PG_NB_SENSORS + 3 * indSens + 1 ] = windowHeight/2.0f + radius * (float)sin( indSens * 2.0 * M_PI / 16.0f );
    sensorLayouts[ indLayout * 3 * PG_NB_SENSORS + 3 * indSens + 2 ] = 0.1f;
  }

  // radial grid
  indLayout = 2;
  // central sensor
  indSens = 0;
  sensorLayouts[ indLayout * 3 * PG_NB_SENSORS + 3 * indSens ] = singleWindowWidth/2.0f;
  sensorLayouts[ indLayout * 3 * PG_NB_SENSORS + 3 * indSens + 1 ] = windowHeight/2.0f;
  sensorLayouts[ indLayout * 3 * PG_NB_SENSORS + 3 * indSens + 2 ] = 0.1f;
  for( int indRay = 0 ; indRay < 6 ; indRay++ ) {
    float angle = indRay * 2.0f * (float)M_PI / 6.0f;
    for( indSens = 1 + indRay * 4 ; indSens < 1 + indRay * 4 + 4 ; indSens++ ) {
      float radius = ((indSens - 1) % 4 + 1) * windowHeight / 10.0f;
      sensorLayouts[ indLayout * 3 * PG_NB_SENSORS + 3 * indSens ] = singleWindowWidth/2.0f + radius * (float)cos( angle );
      sensorLayouts[ indLayout * 3 * PG_NB_SENSORS + 3 * indSens + 1 ] = windowHeight/2.0f + radius * (float)sin( angle );
      sensorLayouts[ indLayout * 3 * PG_NB_SENSORS + 3 * indSens + 2 ] = 0.1f;
    }
  }

  // conflated in the center
  // will be transformed into a random layout each time it is invoked
  indLayout = 3;
  for( indSens = 0 ; indSens < PG_NB_SENSORS ; indSens++ ) {
    sensorLayouts[ indLayout * 3 * PG_NB_SENSORS + 3 * indSens ] = singleWindowWidth/2.0f;
    sensorLayouts[ indLayout * 3 * PG_NB_SENSORS + 3 * indSens + 1 ] = windowHeight/2.0f;
    sensorLayouts[ indLayout * 3 * PG_NB_SENSORS + 3 * indSens + 2 ] = 0.1f;
  }

  ///////////////////////////////////////////////////////////////////////////////////////
  // sensor activations
  ///////////////////////////////////////////////////////////////////////////////////////
  int indActivation;

  // central activation
  indActivation = 0;
  for( indSens = 0 ; indSens < PG_NB_SENSORS ; indSens++ ) {
    sensorActivations[ indActivation * PG_NB_SENSORS + indSens ] = false;
  }
  sensorActivations[ indActivation * PG_NB_SENSORS + 12 ] = true;

  // corners activation
  indActivation = 1;
  for( indSens = 0 ; indSens < PG_NB_SENSORS ; indSens++ ) {
    sensorActivations[ indActivation * PG_NB_SENSORS + indSens ] = false;
  }
  sensorActivations[ indActivation * PG_NB_SENSORS + 0 ] = true;
  sensorActivations[ indActivation * PG_NB_SENSORS + 4 ] = true;
  sensorActivations[ indActivation * PG_NB_SENSORS + 20 ] = true;
  sensorActivations[ indActivation * PG_NB_SENSORS + 24 ] = true;
  sensorActivations[ indActivation * PG_NB_SENSORS + 12 ] = true;

  // central square activation
  indActivation = 2;
  for( indSens = 0 ; indSens < PG_NB_SENSORS ; indSens++ ) {
    sensorActivations[ indActivation * PG_NB_SENSORS + indSens ] = false;
  }
  sensorActivations[ indActivation * PG_NB_SENSORS + 6 ] = true;
  sensorActivations[ indActivation * PG_NB_SENSORS + 7 ] = true;
  sensorActivations[ indActivation * PG_NB_SENSORS + 8 ] = true;
  sensorActivations[ indActivation * PG_NB_SENSORS + 11 ] = true;
  sensorActivations[ indActivation * PG_NB_SENSORS + 12 ] = true;
  sensorActivations[ indActivation * PG_NB_SENSORS + 13 ] = true;
  sensorActivations[ indActivation * PG_NB_SENSORS + 16 ] = true;
  sensorActivations[ indActivation * PG_NB_SENSORS + 17 ] = true;
  sensorActivations[ indActivation * PG_NB_SENSORS + 18 ] = true;

  // every second activation
  indActivation = 3;
  for( indSens = 0 ; indSens < PG_NB_SENSORS ; indSens++ ) {
    if( indSens % 2 == 0 ) {
      sensorActivations[ indActivation * PG_NB_SENSORS + indSens ] = true;
    }
    else {
      sensorActivations[ indActivation * PG_NB_SENSORS + indSens ] = false;
    }
  }

  // full activation
  indActivation = 4;
  for( indSens = 0 ; indSens < PG_NB_SENSORS ; indSens++ ) {
     sensorActivations[ indActivation * PG_NB_SENSORS + indSens ] = true;
  }

  // central activation and activation of an additional sensor each 10 seconds
  indActivation = 5;
  for( indSens = 0 ; indSens < PG_NB_SENSORS ; indSens++ ) {
    sensorActivations[ indActivation * PG_NB_SENSORS + indSens ] = false;
  }
  sensorActivations[ indActivation * PG_NB_SENSORS + 12 ] = true;

  ///////////////////////////////////////////////////////////////////////////////////////
  // sample setup
  ///////////////////////////////////////////////////////////////////////////////////////
  // sample choice
  sample_setUp_interpolation();

  // float sample_choice[ PG_NB_SENSORS];
  // // all possible sensor layouts
  // float sample_setUps[ PG_NB_MAX_SAMPLE_SETUPS][ PG_NB_SENSORS ] =
  //   {{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25},
  //    {26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50},
  //    {51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75}};
}

void sample_setUp_interpolation( void ) {
  float indSetUp = std::max( 0.0f , std::min( sample_setUp , (float)(PG_NB_MAX_SAMPLE_SETUPS - 1) ) );

  float sample_setUp_integerPart = floor( indSetUp );
  float sample_setUp_floatPart = indSetUp - sample_setUp_integerPart;
  int intIndSetup = (int)(sample_setUp_integerPart);

  // copies the setup that corresponds to the integer part
  for( int ind = 0 ; ind < PG_NB_SENSORS ; ind++ ) {
    sample_choice[ ind ] = sample_setUps[ intIndSetup ][ ind ];
  }

  // for the decimal part, copies hybrid sensors of upper level with a ratio
  // proportional to the ratio between floor and current value 
  if( sample_setUp_floatPart > 0.0f ) {
    int nbhybridSensors = (int)round( sample_setUp_floatPart * PG_NB_SENSORS );

    // book keeping of hybridized sensors
    bool hybridized[PG_NB_SENSORS];
    for( int ind = 0 ; ind < PG_NB_SENSORS ; ind++ ) {
      hybridized[ ind ] = false;
    }

    for( int indSensor = 0 ; indSensor < std::min( PG_NB_SENSORS , nbhybridSensors ) ; indSensor++ ) {
      int count = (int)round( ((float)rand()/(float)RAND_MAX) * PG_NB_SENSORS );
      for( int ind = 0 ; ind < PG_NB_SENSORS ; ind++ ) {
	int translatedIndex = (count + PG_NB_SENSORS) % PG_NB_SENSORS;
	if( !hybridized[ translatedIndex ] ) {
	  sample_choice[ translatedIndex ] = sample_setUps[ intIndSetup + 1 ][ translatedIndex ];
	  hybridized[ translatedIndex ] = true;	  
	}
      }
    }
  }

  std::string message = "/sample_setUp";
  std::string format = "";
  for( int indSens = 0 ; indSens < PG_NB_SENSORS - 1 ; indSens++ ) {
    format += "i ";
  }
  format += "i"; 

  // sensor readback
  for( int indSens = 0 ; indSens < PG_NB_SENSORS ; indSens++ ) {
    std::string float_str = std::to_string((long long)sample_choice[indSens]);
    // float_str.resize(4);
    message += " " + float_str;
  }

  pg_send_message_udp( (char *)format.c_str() , (char *)message.c_str() , (char *)"udp_SC_send" );

  // std::cout << "format: " << format << "\n";
  // std::cout << "msg: " << message << "\n";
}

/////////////////////////////////////////////////////////////////
// INTIALIZATION OF THE TACK LINES USED TO DISPLAY A TRACK AS A STRIP
void pg_initGeometry_track_line( int indTrack , bool is_target ) {
  // source track
  if( !is_target && TrackStatus_source[ indTrack ].nbRecordedFrames > 0 ) {
    if( line_tracks_points[indTrack] ) {
      // memory release
      delete [] line_tracks_points[indTrack];
    }
    line_tracks_points[indTrack] = new float[TrackStatus_source[ indTrack ].nbRecordedFrames * 3];
    if(line_tracks_points[indTrack]  == NULL) {		  
      strcpy( ErrorStr , "Track line point allocation error!" ); ReportError( ErrorStr ); throw 425;
    }
    // assigns the index to the coordinates to be able to recover the coordinates from the texture through the point index
    for(int indFrame = 0 ; indFrame < TrackStatus_source[ indTrack ].nbRecordedFrames ; indFrame++ ) {
	line_tracks_points[indTrack][ indFrame * 3 ] = (float)indFrame;
    }

    printOglError( 41 );

    // vertex buffer objects 
    unsigned int line_tracks_points_vbo = 0;
    glGenBuffers (1, &line_tracks_points_vbo);
    glBindBuffer (GL_ARRAY_BUFFER, line_tracks_points_vbo);
    glBufferData (GL_ARRAY_BUFFER, 
		3 * TrackStatus_source[ indTrack ].nbRecordedFrames * sizeof (float), 
		line_tracks_points[indTrack], 
		GL_STATIC_DRAW);

    line_tracks_vao[indTrack] = 0;
    glGenVertexArrays (1, &(line_tracks_vao[indTrack]));
    glBindVertexArray (line_tracks_vao[indTrack]);
    // vertex positions are location 0
    glBindBuffer (GL_ARRAY_BUFFER, line_tracks_points_vbo);
    glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
    glEnableVertexAttribArray (0);

    printf("Source track geometry initialized track %d: size %d\n" , indTrack , TrackStatus_source[ indTrack ].nbRecordedFrames );
    printOglError( 42 );
  }

  // target track
  if( is_target && TrackStatus_target[ indTrack ].nbRecordedFrames > 0 ) {
    if( line_tracks_target_points[indTrack] ) {
      // memory release
      delete [] line_tracks_target_points[indTrack];
    }
    line_tracks_target_points[indTrack] = new float[TrackStatus_target[ indTrack ].nbRecordedFrames * 3];
    if(line_tracks_target_points[indTrack]  == NULL) {		  
      strcpy( ErrorStr , "Target track line point allocation error!" ); ReportError( ErrorStr ); throw 425;
    }
    // assigns the index to the coordinates to be able to recover the coordinates from the texture through the point index
    for(int indFrame = 0 ; indFrame < TrackStatus_target[ indTrack ].nbRecordedFrames ; indFrame++  ) {
	line_tracks_target_points[indTrack][ indFrame * 3 ] = (float)indFrame;
    }

    printOglError( 43 );

    // vertex buffer objects 
    unsigned int line_tracks_target_points_vbo = 0;
    glGenBuffers (1, &line_tracks_target_points_vbo);
    glBindBuffer (GL_ARRAY_BUFFER, line_tracks_target_points_vbo);
    glBufferData (GL_ARRAY_BUFFER, 
		3 * TrackStatus_target[ indTrack ].nbRecordedFrames * sizeof (float), 
		line_tracks_target_points[indTrack], 
		GL_STATIC_DRAW);

    line_tracks_target_vao[indTrack] = 0;
    glGenVertexArrays (1, &(line_tracks_target_vao[indTrack]));
    glBindVertexArray (line_tracks_target_vao[indTrack]);
    // vertex positions are location 0
    glBindBuffer (GL_ARRAY_BUFFER, line_tracks_target_points_vbo);
    glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
    glEnableVertexAttribArray (0);

    printf("Target track geometry initialized track %d: size %d\n" , indTrack , TrackStatus_target[ indTrack ].nbRecordedFrames );

    printOglError( 44 );
  }
}

/////////////////////////////////////////////////////////////////
// TEXTURE INITIALIZATION
/////////////////////////////////////////////////////////////////

// general texture allocation
void *pg_generateTexture( GLuint *textureID , pg_TextureFormat texture_format ,
		       int sizeX , int sizeY ) {
  glGenTextures( 1, textureID );

  // rgb POT raw image allocation (without alpha channel)
  // printf( "Texture %dx%d nb_attachments %d\n" ,  
  // 	  sizeX , sizeY , nb_attachments );
  void *returnvalue = NULL;

  // Return from the function if no file name was passed in
  // Load the image and store the data
  if( texture_format == pg_byte_tex_format ) {
    GLubyte *ptr = new GLubyte[ sizeX * sizeY * 4 ]; 
    // If we can't load the file, quit!
    if(ptr == NULL) {		  
      strcpy( ErrorStr , "Texture allocation error!" ); ReportError( ErrorStr ); throw 425;
    }
    int indTex = 0;
    for( int i = 0 ; i < sizeX * sizeY ; i++ ) {
      ptr[ indTex ] = 0;
      ptr[ indTex + 1 ] = 0;
      ptr[ indTex + 2 ] = 0;
      ptr[ indTex + 3 ] = 0;
      indTex += 4;
    }
    returnvalue = (void *)ptr;
  }
  else if( texture_format == pg_float_tex_format ) {
    float *ptr = new float[ sizeX * sizeY * 4 ]; 
    // If we can't load the file, quit!
    if(ptr == NULL) {		  
      strcpy( ErrorStr , "Texture allocation error!" ); ReportError( ErrorStr ); throw 425;
    }
    int indTex = 0;
    for( int i = 0 ; i < sizeX * sizeY ; i++ ) {
      ptr[ indTex ] = 0.0f;
      ptr[ indTex + 1 ] = 0.0f;
      ptr[ indTex + 2 ] = 0.0f;
      ptr[ indTex + 3 ] = 0.0f;
      indTex += 4;
    }
    returnvalue = (void *)ptr;
  }

  return returnvalue;
}

/////////////////////////////////////////////
// texture loading
bool pg_loadTexture3D( string filePrefixName , string fileSuffixName , 
		    int nbTextures , int bytesperpixel , 
		    bool invert ,
		    GLuint *textureID ,
		    GLint components, GLenum format ,
		    GLenum texturefilter , 
		    int width , int height , int depth ) {
  // data type is assumed to be GL_UNSIGNED_BYTE
  
  char filename[1024];

  long size = width * height * bytesperpixel;
  GLubyte * bitmap = new unsigned char[size * nbTextures];
  long ind = 0;
    
  glEnable(GL_TEXTURE_3D);
  glGenTextures( 1, textureID );

  for( int indTex = 0 ; indTex < nbTextures ; indTex++ ) {
    sprintf( filename , "%s_%03d%s" ,  filePrefixName.c_str() , 
	     indTex + 1 ,  fileSuffixName.c_str() );
    printf(  "Loading %s\n" , filename );

    // texture load through OpenCV
#ifndef OPENCV_3
    cv::Mat img = cv::imread( filename, CV_LOAD_IMAGE_UNCHANGED);   // Read the file
    int colorTransform = (img.channels() == 4) ? CV_BGRA2RGBA : 
      (img.channels() == 3) ? CV_BGR2RGB : CV_GRAY2RGB;
    // int glColorType = (img.channels() == 4) ? GL_RGBA : GL_RGB;
    cv::cvtColor(img, img, colorTransform);
    cv::Mat result;
    if( invert ) 
      cv::flip(img, result , 0); // vertical flip
    else
      result = img;
#else
    cv::Mat img = cv::imread( filename, cv::IMREAD_UNCHANGED );   // Read the file
    int colorTransform = (img.channels() == 4) ? cv::COLOR_BGRA2RGBA : 
      (img.channels() == 3) ? cv::COLOR_BGR2RGB : cv::COLOR_GRAY2RGB;
    // int glColorType = (img.channels() == 4) ? GL_RGBA : GL_RGB;
    cv::cvtColor(img, img, colorTransform);
    cv::Mat result;
    if( invert ) 
      cv::flip(img, result , 0); // vertical flip
    else
      result = img;
#endif

    if(! result.data ) {                              // Check for invalid input
      sprintf( ErrorStr , "Could not open or find the image %s!" , filename ); ReportError( ErrorStr ); throw 425;
      return false;
    }
    if( result.cols != width 
	|| result.rows != height ) {   // Check for invalid input
      sprintf( ErrorStr , "Unexpected image size %s %d/%d %d/%d!" , filename , result.cols , width , result.rows , height); ReportError( ErrorStr ); throw 425;
      return false;
    }

    GLubyte * ptr = result.data;
    for(long int i = 0; i < size ; i++)
      bitmap[ind++] = ptr[i];
  }
  // printf("Final index %ld / %ld\n" , ind , size * nbTextures );
  // glActiveTexture (GL_TEXTURE0 + index);
  glBindTexture(GL_TEXTURE_3D, *textureID );
  glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, (float)texturefilter);
  glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, (float)texturefilter);
  glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S , GL_REPEAT );
  glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT );
  glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT );
  glTexImage3D(GL_TEXTURE_3D,     // Type of texture
	       0,                 // Pyramid level (for mip-mapping) - 0 is the top level
	       components,            // Components: Internal colour format to convert to
	       width,          // Image width
	       height,          // Image heigh
	       depth,          // Image S
	       0,                 // Border width in pixels (can either be 1 or 0)
	       format, // Format: Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
	       GL_UNSIGNED_BYTE,  // Image data type
	       bitmap );        // The actual image data itself
  
   printOglError( 0 );

  // memory release
  delete [] bitmap;

  // glGenerateMipmap(GL_TEXTURE_2D);
  return true;
}

bool pg_loadTexture( string fileName , cv::Mat *image , 
		  GLuint *textureID , bool is_rectangle ,
		  bool invert , 
		  GLint components , GLenum format , 
		  GLenum datatype , GLenum texturefilter,
		  int width , int height ) {

  printf(  "Loading %s\n" , fileName.c_str() );

  glEnable(GL_TEXTURE_2D);
  glGenTextures( 1, textureID );

  // texture load through OpenCV
#ifndef OPENCV_3
  cv::Mat img = cv::imread( fileName, CV_LOAD_IMAGE_UNCHANGED);   // Read the file
  int colorTransform = (img.channels() == 4) ? CV_BGRA2RGBA : 
    (img.channels() == 3) ? CV_BGR2RGB : CV_GRAY2RGB;
  // int glColorType = (img.channels() == 4) ? GL_RGBA : GL_RGB;
  if( img.channels() >= 3 ) {
    cv::cvtColor(img, img, colorTransform);
  }
  if( invert ) 
    cv::flip(img, *image , 0); // vertical flip
  else
    *image = img;
#else
  cv::Mat img = cv::imread( fileName, cv::IMREAD_UNCHANGED);   // Read the file
  int colorTransform = (img.channels() == 4) ? cv::COLOR_BGRA2RGBA : 
    (img.channels() == 3) ? cv::COLOR_BGR2RGB : cv::COLOR_GRAY2RGB;
  // int glColorType = (img.channels() == 4) ? GL_RGBA : GL_RGB;
  if( img.channels() >= 3 ) {
    cv::cvtColor(img, img, colorTransform);
  }
  if( invert ) 
    cv::flip(img, *image , 0); // vertical flip
  else
    *image = img;
#endif

  if(! image->data ) {                              // Check for invalid input
    sprintf( ErrorStr , "Could not open or find the image %s!" , fileName.c_str() ); ReportError( ErrorStr ); throw 425;
    return false;
  }
  if( image->cols != width || image->rows != height ) {                              // Check for invalid input
    sprintf( ErrorStr , "Unexpected image size %s %d/%d %d/%d!" , fileName.c_str() , image->cols , width , image->rows , height); ReportError( ErrorStr ); throw 425;
    return false;
  }

  // glActiveTexture (GL_TEXTURE0 + index);
  if( is_rectangle ) {
    glEnable(GL_TEXTURE_RECTANGLE);
    glBindTexture( GL_TEXTURE_RECTANGLE, *textureID );
    glTexParameterf(GL_TEXTURE_RECTANGLE,GL_TEXTURE_MIN_FILTER, (GLfloat)texturefilter);
    glTexParameterf(GL_TEXTURE_RECTANGLE,GL_TEXTURE_MAG_FILTER, (GLfloat)texturefilter);
    glTexParameterf( GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S , GL_CLAMP );
    glTexParameterf( GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP );
    glTexImage2D(GL_TEXTURE_RECTANGLE,     // Type of texture
		 0,                 // Pyramid level (for mip-mapping) - 0 is the top level
		 components,            // Components: Internal colour format to convert to
		 image->cols,          // Image width
		 image->rows,          // Image heigh
		 0,                 // Border width in pixels (can either be 1 or 0)
		 format, // Format: Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
		 datatype,  // Image data type
		 image->ptr());        // The actual image data itself
    printOglError( 4 );
  }
  else {
    glEnable(GL_TEXTURE_2D);
    glBindTexture( GL_TEXTURE_2D, *textureID );
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, (float)texturefilter);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, (float)texturefilter);
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S , GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexImage2D(GL_TEXTURE_2D,     // Type of texture
		 0,                 // Pyramid level (for mip-mapping) - 0 is the top level
		 components,            // Components: Internal colour format to convert to
		 image->cols,          // Image width
		 image->rows,          // Image height
		 0,                 // Border width in pixels (can either be 1 or 0)
		 format, // Format: Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
		 datatype,  // Image data type
		 image->ptr());        // The actual image data itself
    printOglError( 5 );
  }
  // glGenerateMipmap(GL_TEXTURE_2D);
  return true;
}

void pg_CA_data_table_values( GLuint textureID , GLubyte * data_table , int width , int height ) {
    GLubyte *ptr = data_table;

    //vec4 GOL_params[9]
    //=vec4[9](vec4(0,0,0,0),vec4(3,3,2,3),
	   //  vec4(3,6,2,3),vec4(1,1,1,2),
	   //  vec4(1,2,3,4),vec4(1,2,4,6),
	   //  vec4(2,10,4,6),vec4(2,6,5,6),
	   //  vec4(2,7,5,7));

    ////////////////////////////////////////////
    // GAME OF LIFE FAMILY: 1 neutral + 8 variants
    GLubyte transition_tableGOL[9*2*10] = {
      0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,

      0,0,0,1,0,0,0,0,0,0, 0,0,1,1,0,0,0,0,0,0,
      0,0,0,1,1,1,1,0,0,0, 0,0,1,1,0,0,0,0,0,0,
      0,1,0,0,0,0,0,0,0,0, 0,1,1,0,0,0,0,0,0,0,
      0,1,1,0,0,0,0,0,0,0, 0,0,0,1,1,0,0,0,0,0,

      0,1,1,0,0,0,0,0,0,0, 0,0,0,0,1,1,1,0,0,0,
      0,0,1,1,1,1,1,1,1,1, 0,0,0,0,1,1,1,0,0,0,
      0,0,1,1,1,1,1,0,0,0, 0,0,0,0,0,1,1,0,0,0,
      0,0,1,1,1,1,1,1,0,0, 0,0,0,0,0,1,1,1,0,0,

    };
    for( int indGOL = 0 ; indGOL < 9 ; indGOL++ ) {
      ptr[  0 ] = 2;
      for( int ind = 1 ; ind < std::min( width , 2*10+1 ) ; ind++ ) {
       ptr[  ind * 4 ] = transition_tableGOL[indGOL * 2*10 + ind - 1];
      }
      ptr += 4 * width;
    }

    ////////////////////////////////////////////
    // TOTALISTIC FAMILY: 1 neutral + 8 variants
    // SubType 0: neutral
    ptr[  0 ] = 16;
    for( int ind = 1 ; ind < std::min( width , 16*10+1 ) ; ind++ ) {
     ptr[  ind * 4 ] = 0;
    }
    ptr += 4 * width;

    // SubType 1: CARS
    GLubyte transition_tableCARS[16*10] = {
      0,2,15,6,8,2,4,6,8,0, 0,0,0,0,0,0,0,0,0,0, 4,4,4,4,4,4,4,4,4,0, 0,0,0,0,0,0,0,0,0,0, 
      0,6,6,6,6,6,6,6,6,0, 0,0,0,0,0,0,0,0,0,0, 8,8,8,8,8,8,8,8,8,0, 0,0,0,0,0,0,0,0,0,0, 
      10,10,10,10,10,10,10,10,10,0, 0,0,0,0,0,0,0,0,0,0, 12,12,12,12,12,12,12,12,12,0, 0,0,0,0,0,0,0,0,0,0, 
      14,14,14,14,14,14,14,14,14,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,
    };
    ptr[  0 ] = 16;
    for( int ind = 1 ; ind < std::min( width , 16*10+1 ) ; ind++ ) {
     ptr[  ind * 4 ] = transition_tableCARS[ind-1];
    }
    ptr += 4 * width;

    // SubType 2: EcoLiBra
    GLubyte transition_tableEcoLiBra[16*10] = {
      0,0,7,0,0,0,15,15,0,0, 0,0,0,0,0,0,0,0,0,0, 15,15,15,15,15,2,2,15,15,0, 
      0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 
      0,0,0,0,0,0,0,0,0,0, 12,12,12,12,12,12,12,12,12,0, 0,0,0,0,0,0,0,0,0,0, 
      0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,
      15,0,15,15,15,2,15,15,15,0
    };
    ptr[  0 ] = 16;
    for( int ind = 1 ; ind < std::min( width , 16*10+1 ) ; ind++ ) {
     ptr[  ind * 4 ] = transition_tableEcoLiBra[ind-1];
    }
    ptr += 4 * width;

    // SubType 3: Ladders
    GLubyte transition_tableLadders[16*10] = {
      0,6,5,0,0,2,15,5,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,8,7,15,0,15,0,0, 
      0,0,6,0,0,0,0,0,3,0, 0,0,0,0,0,0,0,0,0,0, 8,0,0,0,0,0,0,0,0,0, 
      8,4,2,5,6,0,0,0,0,0, 4,0,11,0,0,0,0,0,0,0, 0,0,0,0,0,0,15,4,0,0, 
      0,8,0,15,5,0,0,0,0,0, 4,10,0,0,4,5,0,0,4,0, 0,8,8,0,0,12,4,6,0,0, 
      0,0,0,10,2,10,6,6,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,9,0,11,3,0,0, 9,0,0,0,14,0,0,6
    };
    ptr[  0 ] = 16;
    for( int ind = 1 ; ind < std::min( width , 16*10+1 ) ; ind++ ) {
     ptr[  ind * 4 ] = transition_tableLadders[ind-1];
    }
    ptr += 4 * width;

    // SubType 4: Wire World
    GLubyte transition_tableWire[4*10] = {
      0,0,0,0,0,0,0,0,0,0, 2,2,2,2,2,2,2,2,2,0, 3,3,3,3,3,3,3,3,3,0, 3,1,1,3,3,3,3,3,3,3,
    };
    ptr[  0 ] = 4;
    for( int ind = 1 ; ind < std::min( width , 4*10+1 ) ; ind++ ) {
     ptr[  ind * 4 ] = transition_tableWire[ind-1];
    }
    ptr += 4 * width;

    // SubType 5: Busy Brain
    GLubyte transition_tableBusyBrain[3*10] = {
      0,0,1,2,0,2,2,2,2,0, 2,2,2,1,0,2,2,2,2, 0,0,0,0,0,1,2,2,1,2, 
    };
    ptr[  0 ] = 3;
    for( int ind = 1 ; ind < std::min( width , 3*10+1 ) ; ind++ ) {
     ptr[  ind * 4 ] = transition_tableBusyBrain[ind-1];
    }
    ptr += 4 * width;

    // SubType 6: Balloons
    GLubyte transition_tableBalloons[16*10] = {
      0,0,15,0,0,0,5,0,0,0,  0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 
      0,0,0,0,0,0,0,0,0,0, 4,4,8,4,4,4,4,4,4,0,  5,5,5,5,5,7,7,9,11, 
      0,2,2,2,2,2,2,2,2,2,  0,5,5,5,5,5,13,13,9,11, 0,8,8,10,8,8,8,8,8,8,0,  
      2,2,2,2,2,9,13,9,11,0, 10,10,0,10,10,10,10,10,10,0,  4,14,14,14,14,14,14,14,11,0, 2,12,4,12,12,12,12,12,12,0,  
      6,6,6,6,13,13,13,9,11,0, 14,14,14,12,14,14,14,14,14,0,  2,2,2,2,2,2,2,2,2
    };
    ptr[  0 ] = 16;
    for( int ind = 1 ; ind < std::min( width , 16*10+1 ) ; ind++ ) {
     ptr[  ind * 4 ] = transition_tableBalloons[ind-1];
    }
    ptr += 4 * width;

    // SubType 7: Ran Brain
    GLubyte transition_tableRanBrain[16*10] = {
      0,0,5,10,0,0,5,10,0,0, 0,0,5,10,0,0,0,0,15,0, 
      0,0,0,0,0,15,15,0,0,0, 0,0,14,0,0,0,0,0,0,0, 0,0,4,0,0,0,0,0,0,0, 
      2,6,2,6,2,6,2,6,2,0, 2,6,2,6,2,6,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 
      0,0,12,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,12,0,0, 
      0,2,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,14,7
    };
    ptr[  0 ] = 16;
    for( int ind = 1 ; ind < std::min( width , 16*10+1 ) ; ind++ ) {
     ptr[  ind * 4 ] = transition_tableRanBrain[ind-1];
    }
    ptr += 4 * width;

    // SubType 8: Brian's Brain
    GLubyte transition_tableBriansBrain[3*10] = {
      0,0,1,0,0,0,0,0,0,0, 2,2,2,2,2,2,2,2,2,2, 0,0,0,0,0,0,0,0,0,0, 
    };
    ptr[  0 ] = 3;
    for( int ind = 1 ; ind < std::min( width , 3*10+1 ) ; ind++ ) {
     ptr[  ind * 4 ] = transition_tableBriansBrain[ind-1];
    }
    ptr += 4 * width;

    ////////////////////////////////////////////
    // GENERATION FAMILY: 1 neutral + 19 variants
    // SubType 0: neutral
#define nbStatesNeutral 8
    ptr[  0 ] = nbStatesNeutral;
    for( int ind = 1 ; ind < std::min( width , nbStatesNeutral*10+1 ) ; ind++ ) {
     ptr[  ind * 4 ] = 0;
    }
    ptr += 4 * width;

    // SubType 1: TwoStates
#define nbStatesTwoStates 2
    GLubyte TwoStates[nbStatesTwoStates*10] = { 
    0,0,1,0,1,0,0,1,1,0, // TwoStates
    0,0,0,0,0,0,0,0,0,0, // TwoStates
    }; // TwoStates
    ptr[  0 ] = nbStatesTwoStates;
    for( int ind = 1 ; ind < std::min( width , nbStatesTwoStates*10+1 ) ; ind++ ) {
     ptr[  ind * 4 ] = TwoStates[ind-1];
    }
    ptr += 4 * width;

    // SubType 2: Caterpillars
#define nbStatesCaterpillars 4
    GLubyte Caterpillars[nbStatesCaterpillars*10] = { 
    0,0,0,1,0,0,0,1,1,0, // Caterpillars
    2,1,1,2,1,1,1,1,2,2, // Caterpillars
    3,1,1,3,1,1,1,1,3,3, // Caterpillars
    0,1,1,0,1,1,1,1,0,0,
    }; // Caterpillars
    ptr[ 0 ] = nbStatesCaterpillars;
    for( int ind = 1 ; ind < std::min( width , nbStatesCaterpillars*10+1 ) ; ind++ ) {
     ptr[  ind * 4 ] = Caterpillars[ind-1];
    }
    ptr += 4 * width;

    // SubType 3: SoftFreeze
#define nbStatesSoftFreeze 6
    GLubyte SoftFreeze[nbStatesSoftFreeze*10] = { 
    0,0,0,1,0,0,0,0,1,0, // SoftFreeze
    2,1,2,1,1,1,2,2,1,2, // SoftFreeze
    3,1,3,1,1,1,3,3,1,3, // SoftFreeze
    4,1,4,1,1,1,4,4,1,4, // SoftFreeze
    5,1,5,1,1,1,5,5,1,5, // SoftFreeze
    0,1,0,1,1,1,0,0,1,0,
    }; // SoftFreeze
    ptr[  0 ] = nbStatesSoftFreeze;
    for( int ind = 1 ; ind < std::min( width , nbStatesSoftFreeze*10+1 ) ; ind++ ) {
     ptr[  ind * 4 ] = SoftFreeze[ind-1];
    }
    ptr += 4 * width;

    // SubType 4: LOTE
#define nbStatesLOTE 6
    GLubyte LOTE[nbStatesLOTE*10] = { 
    0,0,0,1,0,0,0,0,0,0, // LOTE
    2,2,2,1,1,1,2,2,2,2, // LOTE
    3,3,3,1,1,1,3,3,3,3, // LOTE
    4,4,4,1,1,1,4,4,4,4, // LOTE
    5,5,5,1,1,1,5,5,5,5, // LOTE
    0,0,0,1,1,1,0,0,0,0,
    }; // LOTE
    ptr[  0 ] = nbStatesLOTE;
    for( int ind = 1 ; ind < std::min( width , nbStatesLOTE*10+1 ) ; ind++ ) {
     ptr[  ind * 4 ] = LOTE[ind-1];
    }
    ptr += 4 * width;


    // SubType 5: MeterGuns
#define nbStatesMeterGuns 8
    GLubyte MeterGuns[nbStatesMeterGuns*10] = { 
    0,0,0,1,0,0,0,0,0,0, // MeterGuns
    1,1,1,2,1,1,1,1,1,2, // MeterGuns
    1,1,1,3,1,1,1,1,1,3, // MeterGuns
    1,1,1,4,1,1,1,1,1,4, // MeterGuns
    1,1,1,5,1,1,1,1,1,5, // MeterGuns
    1,1,1,6,1,1,1,1,1,6, // MeterGuns
    1,1,1,7,1,1,1,1,1,7, // MeterGuns
    1,1,1,0,1,1,1,1,1,0,
    }; // MeterGuns
    ptr[  0 ] = nbStatesMeterGuns;
    for( int ind = 1 ; ind < std::min( width , nbStatesMeterGuns*10+1 ) ; ind++ ) {
     ptr[  ind * 4 ] = MeterGuns[ind-1];
    }
    ptr += 4 * width;


    // SubType 6: EbbFlow
#define nbStatesEbbFlow 18
    GLubyte EbbFlow[nbStatesEbbFlow*10] = { 
    0,0,0,1,0,0,1,0,0,0, // EbbFlow
    1,1,1,2,1,2,2,1,1,2, // EbbFlow
    1,1,1,3,1,3,3,1,1,3, // EbbFlow
    1,1,1,4,1,4,4,1,1,4, // EbbFlow
    1,1,1,5,1,5,5,1,1,5, // EbbFlow
    1,1,1,6,1,6,6,1,1,6, // EbbFlow
    1,1,1,7,1,7,7,1,1,7, // EbbFlow
    1,1,1,8,1,8,8,1,1,8, // EbbFlow
    1,1,1,9,1,9,9,1,1,9, // EbbFlow
    1,1,1,10,1,10,10,1,1,10, // EbbFlow
    1,1,1,11,1,11,11,1,1,11, // EbbFlow
    1,1,1,12,1,12,12,1,1,12, // EbbFlow
    1,1,1,13,1,13,13,1,1,13, // EbbFlow
    1,1,1,14,1,14,14,1,1,14, // EbbFlow
    1,1,1,15,1,15,15,1,1,15, // EbbFlow
    1,1,1,16,1,16,16,1,1,16, // EbbFlow
    1,1,1,17,1,17,17,1,1,17, // EbbFlow
    1,1,1,0,1,0,0,1,1,0,
    }; // EbbFlow
    ptr[  0 ] = nbStatesEbbFlow;
    for( int ind = 1 ; ind < std::min( width , nbStatesEbbFlow*10+1 ) ; ind++ ) {
     ptr[  ind * 4 ] = EbbFlow[ind-1];
    }
    ptr += 4 * width;


    // SubType 7: EbbFlow2
#define nbStatesEbbFlow2 18
    GLubyte EbbFlow2[nbStatesEbbFlow2*10] = { 
    0,0,0,1,0,0,0,1,0,0, // EbbFlow2
    1,1,1,2,1,2,1,2,1,2, // EbbFlow2
    1,1,1,3,1,3,1,3,1,3, // EbbFlow2
    1,1,1,4,1,4,1,4,1,4, // EbbFlow2
    1,1,1,5,1,5,1,5,1,5, // EbbFlow2
    1,1,1,6,1,6,1,6,1,6, // EbbFlow2
    1,1,1,7,1,7,1,7,1,7, // EbbFlow2
    1,1,1,8,1,8,1,8,1,8, // EbbFlow2
    1,1,1,9,1,9,1,9,1,9, // EbbFlow2
    1,1,1,10,1,10,1,10,1,10, // EbbFlow2
    1,1,1,11,1,11,1,11,1,11, // EbbFlow2
    1,1,1,12,1,12,1,12,1,12, // EbbFlow2
    1,1,1,13,1,13,1,13,1,13, // EbbFlow2
    1,1,1,14,1,14,1,14,1,14, // EbbFlow2
    1,1,1,15,1,15,1,15,1,15, // EbbFlow2
    1,1,1,16,1,16,1,16,1,16, // EbbFlow2
    1,1,1,17,1,17,1,17,1,17, // EbbFlow2
    1,1,1,0,1,0,1,0,1,0, // EbbFlow2
    }; // EbbFlow2
    ptr[  0 ] = nbStatesEbbFlow2;
    for( int ind = 1 ; ind < std::min( width , nbStatesEbbFlow2*10+1 ) ; ind++ ) {
     ptr[  ind * 4 ] = EbbFlow2[ind-1];
    }
    ptr += 4 * width;


    // SubType 8: SediMental
#define nbStatesSediMental 4
    GLubyte SediMental[nbStatesSediMental*10] = { 
    0,0,1,0,0,1,1,1,1,0, // SediMental
    2,2,2,2,1,1,1,1,1,2, // SediMental
    3,3,3,3,1,1,1,1,1,3, // SediMental
    0,0,0,0,1,1,1,1,1,0,
    }; // SediMental
    ptr[  0 ] = nbStatesSediMental;
    for( int ind = 1 ; ind < std::min( width , nbStatesSediMental*10+1 ) ; ind++ ) {
     ptr[  ind * 4 ] = SediMental[ind-1];
    }
    ptr += 4 * width;


    // SubType 9: Brain6
#define nbStatesBrain6 3
    GLubyte Brain6[nbStatesBrain6*10] = { 
    0,0,1,0,1,0,1,0,0,0, // Brain6
    2,2,2,2,2,2,1,2,2,2, // Brain6
    0,0,0,0,0,0,1,0,0,0, // Brain6
    }; // Brain64
    ptr[  0 ] = nbStatesBrain6;
    for( int ind = 1 ; ind < std::min( width , nbStatesBrain6*10+1 ) ; ind++ ) {
     ptr[  ind * 4 ] = Brain6[ind-1];
    }
    ptr += 4 * width;


    // SubType 10: OrthoGo
#define nbStatesOrthoGo 4
    GLubyte OrthoGo[nbStatesOrthoGo*10] = { 
    0,0,1,0,0,0,0,0,0,0, // OrthoGo
    2,2,2,1,2,2,2,2,2,2, // OrthoGo
    3,3,3,1,3,3,3,3,3,3, // OrthoGo
    }; // OrthoGo
    ptr[  0 ] = nbStatesOrthoGo;
    for( int ind = 1 ; ind < std::min( width , nbStatesOrthoGo*10+1 ) ; ind++ ) {
     ptr[  ind * 4 ] = OrthoGo[ind-1];
    }
    ptr += 4 * width;


    // SubType 11: StarWars
#define nbStatesStarWars 4
    GLubyte StarWars[nbStatesStarWars*10] = { 
    0,0,1,0,0,0,0,0,0,0, // StarWars
    2,2,2,1,1,1,2,2,2,2, // StarWars
    3,3,3,1,1,1,3,3,3,3, // StarWars
    0,0,0,1,1,1,0,0,0,0,
    }; // StarWars
    ptr[  0 ] = nbStatesStarWars;
    for( int ind = 1 ; ind < std::min( width , nbStatesStarWars*10+1 ) ; ind++ ) {
     ptr[  ind * 4 ] = StarWars[ind-1];
    }
    ptr += 4 * width;


    // SubType 12: Transers
#define nbStatesTransers 5
    GLubyte Transers[nbStatesTransers*10] = { 
    0,0,1,0,0,0,1,0,0,0, // Transers
    2,2,2,1,1,1,2,2,2,2, // Transers
    3,3,3,1,1,1,3,3,3,3, // Transers
    4,4,4,1,1,1,4,4,4,4, // Transers
    0,0,0,1,1,1,0,0,0,0,
    }; // Transers
    ptr[  0 ] = nbStatesTransers;
    for( int ind = 1 ; ind < std::min( width , nbStatesTransers*10+1 ) ; ind++ ) {
     ptr[  ind * 4 ] = Transers[ind-1];
    }
    ptr += 4 * width;


    // SubType 13: Snake
#define nbStatesSnake 6
    GLubyte Snake[nbStatesSnake*10] = { 
    0,0,1,0,0,1,0,0,0,0, // Snake
    1,2,2,1,1,2,1,1,2,2, // Snake
    1,3,3,1,1,3,1,1,3,3, // Snake
    1,4,4,1,1,4,1,1,4,4, // Snake
    1,5,5,1,1,5,1,1,5,5, // Snake
    1,0,0,1,1,0,1,1,0,0,
    }; // Snake
    ptr[  0 ] = nbStatesSnake;
    for( int ind = 1 ; ind < std::min( width , nbStatesSnake*10+1 ) ; ind++ ) {
     ptr[  ind * 4 ] = Snake[ind-1];
    }
    ptr += 4 * width;


    // SubType 14: Sticks
#define nbStatesSticks 6
    GLubyte Sticks[nbStatesSticks*10] = { 
    0,0,1,0,0,0,0,0,0,0, // Sticks
    2,2,2,1,1,1,1,2,2,2, // Sticks
    3,3,3,1,1,1,1,3,3,3, // Sticks
    4,4,4,1,1,1,1,4,4,4, // Sticks
    5,5,5,1,1,1,1,5,5,5, // Sticks
    0,0,0,1,1,1,1,0,0,0,
    }; // Sticks
    ptr[  0 ] = nbStatesSticks;
    for( int ind = 1 ; ind < std::min( width , nbStatesSticks*10+1 ) ; ind++ ) {
     ptr[  ind * 4 ] = Sticks[ind-1];
    }
    ptr += 4 * width;


    // SubType 15: Transers2
#define nbStatesTransers2 6
    GLubyte Transers2[nbStatesTransers2*10] = { 
    0,0,1,0,0,0,1,0,0,0, // Transers2
    1,2,2,1,1,1,2,2,2,2, // Transers2
    1,3,3,1,1,1,3,3,3,3, // Transers2
    1,4,4,1,1,1,4,4,4,4, // Transers2
    1,5,5,1,1,1,5,5,5,5, // Transers2
    1,0,0,1,1,1,0,0,0,0, // Transers2
    }; // Transers2
    ptr[  0 ] = nbStatesTransers2;
    for( int ind = 1 ; ind < std::min( width , nbStatesTransers2*10+1 ) ; ind++ ) {
     ptr[  ind * 4 ] = Transers2[ind-1];
    }
    ptr += 4 * width;


    // SubType 16: Worms
#define nbStatesWorms 6
    GLubyte Worms[nbStatesWorms*10] = { 
    0,0,1,0,0,1,0,0,0,0, // Worms
    2,2,2,1,1,2,1,1,2,2, // Worms
    3,3,3,1,1,3,1,1,3,3, // Worms
    4,4,4,1,1,4,1,1,4,4, // Worms
    5,5,5,1,1,5,1,1,5,5, // Worms
    0,0,0,1,1,0,1,1,0,0,
    }; // Worms
    ptr[  0 ] = nbStatesWorms;
    for( int ind = 1 ; ind < std::min( width , nbStatesWorms*10+1 ) ; ind++ ) {
     ptr[  ind * 4 ] = Worms[ind-1];
    }
    ptr += 4 * width;


    // SubType 17: Cooties
#define nbStatesCooties 9
    GLubyte Cooties[nbStatesCooties*10] = { 
    0,0,1,0,0,0,0,0,0,0, // Cooties
    2,2,1,1,2,2,2,2,2,2, // Cooties
    3,3,1,1,3,3,3,3,3,3, // Cooties
    4,4,1,1,4,4,4,4,4,4, // Cooties
    5,5,1,1,5,5,5,5,5,5, // Cooties
    6,6,1,1,6,6,6,6,6,6, // Cooties
    7,7,1,1,7,7,7,7,7,7, // Cooties
    8,8,1,1,8,8,8,8,8,8, // Cooties
    0,0,1,1,0,0,0,0,0,0,
    }; // Cooties
    ptr[  0 ] = nbStatesCooties;
    for( int ind = 1 ; ind < std::min( width , nbStatesCooties*10+1 ) ; ind++ ) {
     ptr[  ind * 4 ] = Cooties[ind-1];
    }
    ptr += 4 * width;



    // SubType 18: Faders
#define nbStatesFaders 25
    GLubyte Faders[nbStatesFaders*10] = { 
    0,0,1,0,0,0,0,0,0,0, // Faders
    2,2,1,2,2,2,2,2,2,2, // Faders
    3,3,1,3,3,3,3,3,3,3, // Faders
    4,4,1,4,4,4,4,4,4,4, // Faders
    5,5,1,5,5,5,5,5,5,5, // Faders
    6,6,1,6,6,6,6,6,6,6, // Faders
    7,7,1,7,7,7,7,7,7,7, // Faders
    8,8,1,8,8,8,8,8,8,8, // Faders
    9,9,1,9,9,9,9,9,9,9, // Faders
    10,10,1,10,10,10,10,10,10,10, // Faders
    11,11,1,11,11,11,11,11,11,11, // Faders
    12,12,1,12,12,12,12,12,12,12, // Faders
    13,13,1,13,13,13,13,13,13,13, // Faders
    14,14,1,14,14,14,14,14,14,14, // Faders
    15,15,1,15,15,15,15,15,15,15, // Faders
    16,16,1,16,16,16,16,16,16,16, // Faders
    17,17,1,17,17,17,17,17,17,17, // Faders
    18,18,1,18,18,18,18,18,18,18, // Faders
    19,19,1,19,19,19,19,19,19,19, // Faders
    20,20,1,20,20,20,20,20,20,20, // Faders
    21,21,1,21,21,21,21,21,21,21, // Faders
    22,22,1,22,22,22,22,22,22,22, // Faders
    23,23,1,23,23,23,23,23,23,23, // Faders
    24,24,1,24,24,24,24,24,24,24, // Faders
    0,0,1,0,0,0,0,0,0,0,
    }; // Faders
    ptr[  0 ] = nbStatesFaders;
    for( int ind = 1 ; ind < std::min( width , nbStatesFaders*10+1 ) ; ind++ ) {
     ptr[  ind * 4 ] = Faders[ind-1];
    }
    ptr += 4 * width;


    // SubType 19: Fireworks
#define nbStatesFireworks 21
    GLubyte Fireworks[nbStatesFireworks*10] = { 
    0,1,0,1,0,0,0,0,0,0, // Fireworks
    2,2,1,2,2,2,2,2,2,2, // Fireworks
    3,3,1,3,3,3,3,3,3,3, // Fireworks
    4,4,1,4,4,4,4,4,4,4, // Fireworks
    5,5,1,5,5,5,5,5,5,5, // Fireworks
    6,6,1,6,6,6,6,6,6,6, // Fireworks
    7,7,1,7,7,7,7,7,7,7, // Fireworks
    8,8,1,8,8,8,8,8,8,8, // Fireworks
    9,9,1,9,9,9,9,9,9,9, // Fireworks
    10,10,1,10,10,10,10,10,10,10, // Fireworks
    11,11,1,11,11,11,11,11,11,11, // Fireworks
    12,12,1,12,12,12,12,12,12,12, // Fireworks
    13,13,1,13,13,13,13,13,13,13, // Fireworks
    14,14,1,14,14,14,14,14,14,14, // Fireworks
    15,15,1,15,15,15,15,15,15,15, // Fireworks
    16,16,1,16,16,16,16,16,16,16, // Fireworks
    17,17,1,17,17,17,17,17,17,17, // Fireworks
    18,18,1,18,18,18,18,18,18,18, // Fireworks
    19,19,1,19,19,19,19,19,19,19, // Fireworks
    20,20,1,20,20,20,20,20,20,20, // Fireworks
    0,0,1,0,0,0,0,0,0,0,
    }; // Fireworks
    ptr[  0 ] = nbStatesFireworks;
    for( int ind = 1 ; ind < std::min( width , nbStatesFireworks*10+1 ) ; ind++ ) {
     ptr[  ind * 4 ] = Fireworks[ind-1];
    }
    ptr += 4 * width;


    ////////////////////////////////////////////
    // GENERAL BINARY FAMILY Moore Neighborhood: 1 neutral + 7 variants
    // Example: Fallski
    // C48,NM,Sb255a,Babb189ab63a
    // 48 states 0-47
    // Moore neihborhood Order N,NE,E,SE,S,SW,W,NW
    // states are encoded: S_N + 2 * S_NE + 4 * S_E + 8 * S_SE ... + 128 * S_NW
    // 00000000 0 neighbor
    // 10000000 N neighbor
    // 01000000 NE neighbor
    // 192 = 00000011 W and NW neighbors
    // Survive b255a survival on no alive neighbors: 
    //                             1 x one    255 x zeros
    // Birth   abb189ab63a birth on a single N or  NE neighbor, or on W and NW neighbors:
    //                             0 1 1   189 x zeros   1   63 x zeros
    // Encoding of Survival and Birth
    // 256 0/1 digits encode 

    // SubType 0: neutral
    ptr[  0 ] = 2;
    for( int ind = 1 ; ind < std::min( width , 256*2+1 ) ; ind++ ) {
     ptr[  ind * 4 ] = 0;
    }
    ptr += 4 * width;

    // Subtype 1: Fallski
    GLubyte transition_tableFallski[256*2] = {
      1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    };
    ptr[  0 ] = 48;
    for( int ind = 1 ; ind < std::min( width , 256*2+1 ) ; ind++ ) {
     ptr[  ind * 4 ] = transition_tableFallski[ind-1];
    }
    ptr += 4 * width;

    // Subtype 2: JustFriends
    GLubyte transition_tableJustFriends[256*2] = {
      0,1,1,1,1,1,1,0,1,1,1,0,1,0,0,0,1,1,1,0,1,0,0,0,1,0,0,0,0,0,0,0,1,1,1,0,1,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,1,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,1,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,1,0,0,0,1,1,0,0,0,0,0,0,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,1,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    };
    ptr[  0 ] = 2;
    for( int ind = 1 ; ind < std::min( width , 256*2+1 ) ; ind++ ) {
     ptr[  ind * 4 ] = transition_tableJustFriends[ind-1];
    }
    ptr += 4 * width;

    // Subtype 3: LogicRule
    GLubyte transition_tableLogicRule[256*2] = {
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,1,0,1,1,0,0,0,0,0,1,0,0,1,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    };
    ptr[  0 ] = 2;
    for( int ind = 1 ; ind < std::min( width , 256*2+1 ) ; ind++ ) {
     ptr[  ind * 4 ] = transition_tableLogicRule[ind-1];
    }
    ptr += 4 * width;

    // Subtype 4: Meteorama
    GLubyte transition_tableMeteorama[256*2] = {
      0,0,0,1,0,0,1,0,0,0,1,1,1,0,0,0,0,0,0,1,1,0,0,0,0,0,1,0,1,1,0,0,1,0,0,0,1,1,0,1,1,0,1,0,0,0,0,0,0,0,1,1,1,1,1,0,1,0,0,1,1,1,1,0,0,1,0,0,0,0,0,1,1,1,0,0,0,1,1,0,0,0,0,0,1,1,0,1,0,1,0,1,1,0,0,0,1,0,1,0,0,1,1,0,0,0,1,1,0,1,0,1,0,0,1,0,0,0,1,1,1,1,1,1,0,1,0,0,0,1,0,1,0,0,0,0,1,1,0,1,1,1,1,1,0,0,1,0,0,0,1,0,1,1,1,0,0,0,0,0,0,1,0,1,1,1,1,0,0,0,0,1,1,1,1,0,0,0,1,0,0,0,1,0,0,0,0,0,1,1,0,1,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,1,1,1,1,0,0,0,1,1,0,1,1,1,0,0,0,1,0,0,0,0,1,0,0,1,0,0,1,1,0,1,0,1,0,1,0,1,0,0,1,0,0,0,0,1,0,0,0,
      0,0,0,0,0,1,0,1,1,1,1,1,0,0,0,1,0,1,0,1,0,1,0,0,0,0,0,0,1,1,1,0,0,1,0,0,0,1,0,1,0,1,1,1,0,0,0,1,1,0,0,0,0,1,1,0,1,0,1,0,0,1,0,0,0,1,0,0,1,0,0,1,0,1,1,0,0,1,1,1,1,1,0,1,1,1,0,1,0,1,0,1,0,0,0,0,0,0,1,0,0,1,0,1,1,0,1,0,0,0,0,0,0,1,0,1,0,0,1,0,0,1,1,1,1,1,0,0,1,0,0,1,1,0,0,1,0,0,0,0,1,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,1,0,1,0,0,0,0,1,0,1,0,1,1,0,0,1,1,1,1,0,0,1,0,0,0,1,1,1,0,0,1,1,0,1,0,0,0,0,0,0,0,1,0,0,1,0,0,1,0,0,0,1,1,1,0,1,1,1,0,1,1,0,1,0,0,0,0,0,0,1,1,0,0,1,0,0,0,0,0,0,0,0,1,1,0,1,1,
    };
    ptr[  0 ] = 24;
    for( int ind = 1 ; ind < std::min( width , 256*2+1 ) ; ind++ ) {
     ptr[  ind * 4 ] = transition_tableMeteorama[ind-1];
    }
    ptr += 4 * width;

    // Subtype 5: Slugfest
    GLubyte transition_tableSlugfest[256*2] = {
      1,1,1,1,0,0,0,1,0,0,1,1,0,0,1,1,1,1,0,1,0,1,1,0,1,0,0,0,0,0,0,1,1,0,0,1,0,0,0,1,0,1,0,0,0,1,1,0,0,0,0,0,1,0,1,0,0,0,0,0,1,0,0,0,1,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,1,0,1,0,1,1,0,0,1,1,1,0,0,1,0,1,1,0,1,0,1,1,0,0,0,0,0,0,1,0,0,1,0,0,1,1,0,1,1,0,1,0,1,1,1,0,0,0,0,1,0,0,1,0,1,0,0,1,1,0,1,0,1,0,0,1,1,0,1,1,0,0,1,1,0,0,0,1,1,0,0,0,1,0,1,1,1,0,0,0,1,0,1,0,0,0,1,1,0,1,1,1,0,0,0,1,1,1,0,0,0,0,0,1,0,0,0,0,0,0,0,1,1,1,0,1,1,0,0,0,0,0,0,0,1,1,0,0,1,0,0,0,1,0,0,0,0,1,1,0,1,0,1,1,0,1,1,1,0,0,0,1,1,0,0,1,1,0,0,1,1,1,0,1,0,
      0,0,0,0,0,0,1,1,0,0,0,0,0,0,1,0,0,1,1,1,0,1,1,0,1,0,0,0,0,0,1,0,0,0,0,0,1,0,1,1,1,0,1,0,0,0,0,0,0,0,1,1,0,0,0,0,1,1,1,0,0,1,0,1,0,0,0,0,0,0,1,1,1,0,1,0,1,0,1,0,1,0,0,0,0,0,0,0,1,0,1,0,1,0,1,1,0,1,1,1,0,0,1,1,0,0,0,0,0,0,1,1,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,1,0,1,0,1,0,0,0,1,1,1,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,1,1,1,1,0,0,1,0,1,1,1,0,1,0,0,1,1,0,0,1,1,1,0,1,1,0,0,0,0,1,1,0,1,1,1,1,0,1,0,1,1,0,0,0,1,0,0,0,0,0,1,1,0,0,0,1,0,0,1,0,0,0,1,1,1,1,0,1,0,1,1,1,1,0,0,0,0,0,0,0,1,1,0,0,1,1,0,
    };
    ptr[  0 ] = 20;
    for( int ind = 1 ; ind < std::min( width , 256*2+1 ) ; ind++ ) {
     ptr[  ind * 4 ] = transition_tableSlugfest[ind-1];
    }
    ptr += 4 * width;

    // Subtype 6: Snowflakes
    GLubyte transition_tableSnowflakes[256*2] = {
      0,1,1,0,1,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,1,0,1,0,0,1,0,0,0,1,0,0,1,0,0,1,1,0,0,0,0,0,1,1,0,0,1,0,0,1,0,0,1,0,1,1,1,1,0,0,0,0,0,0,0,0,1,0,0,1,0,1,1,0,0,0,0,1,0,0,1,0,0,1,1,0,0,0,1,1,0,0,0,1,0,1,1,1,0,1,1,0,1,0,0,0,1,0,1,0,1,0,0,0,1,0,0,1,0,0,0,0,1,1,1,0,0,1,0,1,0,0,1,0,1,0,0,0,0,1,1,0,1,0,0,1,1,0,0,0,0,1,0,1,0,0,1,0,1,0,0,1,1,1,0,0,0,0,1,0,0,1,0,0,0,1,0,1,0,1,0,0,0,1,0,1,1,0,1,1,1,0,1,0,0,0,1,0,0,0,0,1,1,0,0,1,1,0,1,0,0,0,0,1,1,0,1,0,0,1,0,0,0,0,0,0,0,0,1,1,1,1,0,1,0,0,1,0,1,1,0,0,1,1,0,0,0,0,0,1,1,0,0,1,1,0,1,0,0,0,1,0,1,0,1,0,0,0,1,0,1,
    };
    ptr[  0 ] = 2;
    for( int ind = 1 ; ind < std::min( width , 256*2+1 ) ; ind++ ) {
     ptr[  ind * 4 ] = transition_tableSnowflakes[ind-1];
    }
    ptr += 4 * width;

    // Subtype 7: Springski
    GLubyte transition_tableSpringski[256*2] = {
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    };
    ptr[  0 ] = 78;
    for( int ind = 1 ; ind < std::min( width , 256*2+1 ) ; ind++ ) {
     ptr[  ind * 4 ] = transition_tableSpringski[ind-1];
    }
    ptr += 4 * width;

    ////////////////////////////////////////////
    // GENERAL BINARY FAMILY von Neumann Neighborhood: 1 neutral + 3 variants

    // SubType 0: neutral
    ptr[  0 ] = 2;
    for( int ind = 1 ; ind < std::min( width , 16*2+1 ) ; ind++ ) {
     ptr[  ind * 4 ] = 0;
    }
    ptr += 4 * width;

    // Subtype 1: Banks
    GLubyte transition_tableBanks[16*2] = {
      1,1,1,0,1,1,0,1,1,0,1,1,0,1,1,1,
      0,0,0,0,0,0,0,1,0,0,0,1,0,1,1,1,
    };
    ptr[  0 ] = 2;
    for( int ind = 1 ; ind < std::min( width , 16*2+1 ) ; ind++ ) {
     ptr[  ind * 4 ] = transition_tableBanks[ind-1];
    }
    ptr += 4 * width;

    // Subtype 2: FractalBeads
    GLubyte transition_tableFractalBeads[16*2] = {
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,
    };
    ptr[  0 ] = 4;
    for( int ind = 1 ; ind < std::min( width , 16*2+1 ) ; ind++ ) {
     ptr[  ind * 4 ] = transition_tableFractalBeads[ind-1];
    }
    ptr += 4 * width;

    // Subtype 3: Sierpinski
    GLubyte transition_tableSierpinski[16*2] = {
      1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
      0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,
    };
    ptr[  0 ] = 2;
    for( int ind = 1 ; ind < std::min( width , 16*2+1 ) ; ind++ ) {
     ptr[  ind * 4 ] = transition_tableSierpinski[ind-1];
    }
    ptr += 4 * width;


    ////////////////////////////////////////////
    // NEUMANNN BINARY FAMILY : 1 neutral + 18 variants
    // Fredkin2 rule has the following definition: 2,01101001100101101001011001101001
    // The first digit, '2', tells the rule has 2 states (it's a 1 bit rule).
    // The second digit, '0', tells a cell in a configuration ME=0,N=0,E=0,S=0,W=0 will get the state 0.
    // The third digit, '1', tells a cell in a configuration ME=0,N=0,E=0,S=0,W=1 will get the state 1.
    // The fourth digit, '1', tells a cell in a configuration ME=0,N=0,E=0,S=1,W=0 will get the state 1.
    // The fifth digit, '0', tells a cell in a configuration ME=0,N=0,E=0,S=1,W=1 will get the state 0.
    // . . .
    // binary rules are extended to ternary for a uniform rule system

    // SubType 0: neutral
    ptr[  0 ] = 3;
    for( int ind = 1 ; ind < std::min( width , 243+1 ) ; ind++ ) {
     ptr[  ind * 4 ] = 0;
    }
    ptr += 4 * width;

    // Subtype 1: Crystal2
    GLubyte transition_tableCrystal2[243] = {
0,1,0,1,0,0,0,0,0,1,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,1,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,1,1,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
    };
    ptr[  0 ] = 3;
    for( int ind = 1 ; ind < std::min( width , 243+1 ) ; ind++ ) {
     ptr[  ind * 4 ] = transition_tableCrystal2[ind-1];
    }
    ptr += 4 * width;

    // Subtype 2: Fredkin2
    GLubyte transition_tableFredkin2[243] = {
0,1,0,1,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
    };
    ptr[  0 ] = 3;
    for( int ind = 1 ; ind < std::min( width , 243+1 ) ; ind++ ) {
     ptr[  ind * 4 ] = transition_tableFredkin2[ind-1];
    }
    ptr += 4 * width;

    // Subtype 3: Aggregation
    GLubyte transition_tableAggregation[243] = {
0,0,2,0,0,0,2,0,2,0,0,0,0,0,0,0,0,0,2,0,2,0,0,0,2,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,2,0,0,0,2,0,2,0,0,0,0,0,0,0,0,0,2,0,2,0,0,0,2,0,2,0,0,1,0,0,1,1,1,1,0,0,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,1,0,0,1,1,1,1,0,0,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,1,2,0,2,1,2,2,2,0,2,0,2,0,1,0,1,0,2,2,2,2,2,1,2,2,2,0,1,2,0,1,2,0,1,0,1,2,2,0,1,1,2,1,1,0,0,0,1,1,1,1,1,1,2,0,2,1,2,2,2,1,2,1,2,1,1,1,1,1,1,1,2,0,2,1,1,1,2,1,1
    };
    ptr[  0 ] = 3;
    for( int ind = 1 ; ind < std::min( width , 243+1 ) ; ind++ ) {
     ptr[  ind * 4 ] = transition_tableAggregation[ind-1];
    }
    ptr += 4 * width;

    // Subtype 4: Birds
    GLubyte transition_tableBirds[243] = {
0,1,0,1,1,2,0,2,0,1,1,2,1,1,2,2,2,2,0,2,0,2,2,2,0,2,0,1,1,2,1,1,2,2,2,2,1,1,2,1,0,2,2,2,0,2,2,2,2,2,0,2,0,2,0,2,0,2,2,2,0,2,0,2,2,2,2,2,0,2,0,2,0,2,0,2,0,2,0,2,0,1,1,2,1,0,2,2,2,2,1,0,2,0,0,0,2,0,0,2,2,2,2,0,0,2,0,2,1,0,2,0,0,0,2,0,0,0,0,0,0,1,2,0,2,0,2,0,0,0,2,0,0,0,0,2,2,2,2,0,0,2,0,2,2,0,0,0,2,0,0,0,0,2,0,2,0,0,0,2,0,0,0,2,0,2,2,0,0,0,0,2,2,0,2,2,2,0,2,0,0,0,0,0,2,0,0,0,0,2,2,0,2,2,2,0,2,0,2,2,2,2,2,2,2,2,2,0,2,0,2,2,2,0,2,0,0,0,0,0,2,0,0,0,0,0,2,0,2,2,2,0,2,0,0,0,0,0,2,0,0,0,0
    };
    ptr[  0 ] = 3;
    for( int ind = 1 ; ind < std::min( width , 243+1 ) ; ind++ ) {
     ptr[  ind * 4 ] = transition_tableBirds[ind-1];
    }
    ptr += 4 * width;

    // Subtype 5: Colony
    GLubyte transition_tableColony[243] = {
0,1,0,1,0,2,0,2,0,1,0,2,0,1,1,2,1,0,0,2,0,2,1,0,0,0,0,1,0,2,0,1,1,2,1,0,0,1,1,1,0,2,1,2,0,2,1,0,1,2,0,0,0,0,0,2,0,2,1,0,0,0,0,2,1,0,1,2,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,2,1,1,1,2,1,2,1,1,1,1,1,0,1,0,0,2,1,2,1,0,0,2,0,0,1,1,1,1,1,0,1,0,0,1,1,0,1,0,2,0,2,1,1,0,0,0,2,1,0,1,2,2,1,2,1,0,0,2,0,0,1,0,0,0,2,1,0,1,2,2,0,0,0,1,2,0,2,0,0,2,0,2,2,2,0,2,0,2,2,2,2,2,2,2,2,2,0,2,0,2,2,2,0,2,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0,2,0,2,2,2,0,2,0,2,2,2,2,2,2,2,2,2,0,2,0,2,2,2,0,2,0
    };
    ptr[  0 ] = 3;
    for( int ind = 1 ; ind < std::min( width , 243+1 ) ; ind++ ) {
     ptr[  ind * 4 ] = transition_tableColony[ind-1];
    }
    ptr += 4 * width;

    // Subtype 6: Crystal3a
    GLubyte transition_tableCrystal3a[243] = {
0,1,2,1,0,1,2,2,0,1,0,0,0,1,0,1,0,0,2,1,0,2,0,0,0,0,2,1,0,2,0,1,0,0,0,0,0,1,0,1,2,1,0,1,0,1,0,0,0,1,1,0,0,2,2,1,0,0,0,0,0,0,2,2,0,0,0,1,0,0,1,2,0,0,2,0,0,2,2,2,1,1,1,1,1,0,1,1,1,1,1,0,1,0,0,0,1,0,0,1,1,1,1,0,0,1,0,0,1,0,1,0,0,0,1,0,0,0,0,0,0,2,0,0,0,0,1,0,0,0,0,0,0,0,1,1,1,1,1,0,0,1,0,0,1,0,0,0,0,0,0,0,1,1,0,0,0,0,1,0,1,1,2,2,2,2,2,2,2,2,0,2,2,2,2,0,0,2,0,0,2,2,2,2,0,0,0,0,0,2,2,2,2,0,0,2,0,0,2,0,0,0,2,2,0,2,0,2,0,0,0,2,0,0,0,0,2,2,0,2,0,0,2,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0
    };
    ptr[  0 ] = 3;
    for( int ind = 1 ; ind < std::min( width , 243+1 ) ; ind++ ) {
     ptr[  ind * 4 ] = transition_tableCrystal3a[ind-1];
    }
    ptr += 4 * width;

    // Subtype 7: Crystal3b
    GLubyte transition_tableCrystal3b[243] = {
0,1,2,1,0,0,2,0,0,1,0,0,0,1,1,0,0,2,2,0,0,0,1,2,0,2,0,1,0,0,0,2,0,0,2,1,0,2,1,2,2,1,0,2,1,0,1,2,0,2,0,1,2,2,2,0,0,0,2,1,0,1,0,0,0,2,2,1,2,1,0,2,0,2,0,1,1,2,0,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,1,0,0,1,1,1,1,0,0,1,0,0,1,1,1,1,0,0,1,0,0,1,0,0,0,1,0,0,0,1,1,0,0,0,0,1,0,1,1,1,1,1,1,0,0,1,0,0,1,0,0,0,0,1,0,1,1,1,0,0,0,1,1,0,1,1,2,2,2,2,2,2,2,2,2,2,2,2,2,0,0,2,0,0,2,2,2,2,0,0,2,0,0,2,2,2,2,0,0,2,0,0,2,0,0,0,2,2,0,2,2,2,0,0,0,2,0,0,2,0,2,2,2,2,0,0,2,0,0,2,0,0,0,2,2,0,0,0,2,0,0,0,2,0,0,0,0
    };
    ptr[  0 ] = 3;
    for( int ind = 1 ; ind < std::min( width , 243+1 ) ; ind++ ) {
     ptr[  ind * 4 ] = transition_tableCrystal3b[ind-1];
    }
    ptr += 4 * width;

    // Subtype 8: Galaxy
    GLubyte transition_tableGalaxy[243] = {
0,1,0,1,1,2,0,2,0,1,1,2,1,1,2,2,2,0,0,2,0,2,2,0,0,0,0,1,1,2,1,1,2,2,2,0,1,1,2,1,1,0,2,0,0,2,2,0,2,0,0,0,0,0,0,2,0,2,2,0,0,0,0,2,2,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,2,2,2,2,0,0,2,0,0,0,2,0,0,2,2,2,2,0,0,2,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,2,0,2,0,2,2,2,2,0,0,2,0,0,2,0,0,0,0,2,0,2,0,2,0,0,0,2,0,0,0,2,0,2,0,2,2,0,0,0,0,2,2,0,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,0,2,2,0,0,0,0,2,2,0,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
    };
    ptr[  0 ] = 3;
    for( int ind = 1 ; ind < std::min( width , 243+1 ) ; ind++ ) {
     ptr[  ind * 4 ] = transition_tableGalaxy[ind-1];
    }
    ptr += 4 * width;

    // Subtype 9: Greenberg
    GLubyte transition_tableGreenberg[243] = {
0,1,0,1,1,0,0,0,0,1,1,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,1,1,0,0,0,0,1,1,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
    };
    ptr[  0 ] = 3;
    for( int ind = 1 ; ind < std::min( width , 243+1 ) ; ind++ ) {
     ptr[  ind * 4 ] = transition_tableGreenberg[ind-1];
    }
    ptr += 4 * width;

    // Subtype 10: Honeycomb
    GLubyte transition_tableHoneycomb[243] = {
0,1,0,1,0,2,0,2,0,1,0,2,0,0,2,2,2,2,0,2,0,2,2,2,0,2,0,1,0,2,0,0,2,2,2,2,0,0,2,0,0,2,2,2,0,2,2,2,2,2,0,2,0,2,0,2,0,2,2,2,0,2,0,2,2,2,2,2,0,2,0,2,0,2,0,2,0,2,0,2,0,1,1,0,1,1,2,0,2,0,1,1,2,1,0,0,2,0,2,0,2,0,2,0,2,0,2,0,1,1,2,1,0,0,2,0,2,1,0,0,0,0,2,0,2,0,2,0,2,0,2,0,2,0,0,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,0,0,2,0,2,0,0,0,0,0,0,2,0,2,2,2,0,2,0,2,2,2,2,2,2,2,2,2,0,2,0,2,2,2,0,2,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0,2,0,2,2,2,2,2,0,2,2,2,2,0,2,0,2,2,2,0,2,0,2,2,2,2,0,2,2,2,2,0,2,0,2,2,2,0,2,0
    };
    ptr[  0 ] = 3;
    for( int ind = 1 ; ind < std::min( width , 243+1 ) ; ind++ ) {
     ptr[  ind * 4 ] = transition_tableHoneycomb[ind-1];
    }
    ptr += 4 * width;

    // Subtype 11: Knitting
    GLubyte transition_tableKnitting[243] = {
0,1,0,1,1,2,0,2,0,1,1,2,1,1,0,2,0,2,0,2,0,2,0,2,0,2,0,1,1,2,1,1,0,2,0,2,1,1,0,1,1,0,0,0,2,2,0,2,0,0,2,2,2,1,0,2,0,2,0,2,0,2,0,2,0,2,0,0,2,2,2,1,0,2,0,2,2,1,0,1,0,1,0,2,0,1,0,2,0,2,0,1,0,1,0,2,0,2,1,2,0,2,0,2,1,2,1,1,0,1,0,1,0,2,0,2,1,1,0,2,0,0,0,2,0,0,0,2,1,2,0,0,1,0,2,2,0,2,0,2,1,2,1,1,0,2,1,2,0,0,1,0,2,2,1,1,1,0,2,1,2,0,0,2,0,2,2,2,0,2,0,2,2,2,2,2,2,2,2,2,0,2,0,2,2,2,0,2,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0,2,0,2,2,2,0,2,0,2,2,2,2,2,2,2,2,2,0,2,0,2,2,2,0,2,0
    };
    ptr[  0 ] = 3;
    for( int ind = 1 ; ind < std::min( width , 243+1 ) ; ind++ ) {
     ptr[  ind * 4 ] = transition_tableKnitting[ind-1];
    }
    ptr += 4 * width;

    // Subtype 12: Lake
    GLubyte transition_tableLake[243] = {
0,1,0,1,1,2,0,2,0,1,1,2,1,0,0,2,0,2,0,2,0,2,0,2,0,2,0,1,1,2,1,0,0,2,0,2,1,0,0,0,1,0,0,0,2,2,0,2,0,0,2,2,2,0,0,2,0,2,0,2,0,2,0,2,0,2,0,0,2,2,2,0,0,2,0,2,2,0,0,0,0,0,1,2,1,1,0,2,0,2,1,1,0,1,0,0,0,0,0,2,0,2,0,0,0,2,0,0,1,1,0,1,0,0,0,0,0,1,0,0,0,0,2,0,2,2,0,0,0,0,2,2,0,2,0,2,0,2,0,0,0,2,0,0,0,0,0,0,2,2,0,2,0,2,0,0,0,2,0,0,0,0,0,2,0,2,2,2,0,2,0,2,2,2,2,2,2,2,2,2,0,2,0,2,2,2,0,2,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0,2,0,2,2,2,0,2,0,2,2,2,2,2,2,2,2,2,0,2,0,2,2,2,0,2,0
    };
    ptr[  0 ] = 3;
    for( int ind = 1 ; ind < std::min( width , 243+1 ) ; ind++ ) {
     ptr[  ind * 4 ] = transition_tableLake[ind-1];
    }
    ptr += 4 * width;

    // Subtype 13: Plankton
    GLubyte transition_tablePlankton[243] = {
0,1,0,1,1,2,0,2,0,1,1,2,1,1,2,2,2,2,0,2,0,2,2,2,0,2,0,1,1,2,1,1,2,2,2,2,1,1,2,1,0,2,2,2,0,2,2,2,2,2,0,2,0,2,0,2,0,2,2,2,0,2,0,2,2,2,2,2,0,2,0,2,0,2,0,2,0,2,0,2,0,1,0,0,0,0,2,0,2,0,0,0,2,0,1,0,2,0,2,0,2,0,2,0,2,0,2,0,0,0,2,0,1,0,2,0,2,0,1,0,1,0,2,0,2,0,2,0,2,0,2,0,2,0,0,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,0,0,2,0,2,0,0,0,0,0,0,2,0,2,2,0,0,0,0,2,2,0,2,2,2,0,2,0,0,0,0,0,2,0,0,0,0,2,2,0,2,2,2,0,2,0,2,2,2,2,2,2,2,2,2,0,2,0,2,2,2,0,2,0,0,0,0,0,2,0,0,0,0,0,2,0,2,2,2,0,2,0,0,0,0,0,2,0,0,0,0
    };
    ptr[  0 ] = 3;
    for( int ind = 1 ; ind < std::min( width , 243+1 ) ; ind++ ) {
     ptr[  ind * 4 ] = transition_tablePlankton[ind-1];
    }
    ptr += 4 * width;

    // Subtype 14: Pond
    GLubyte transition_tablePond[243] = {
0,1,0,1,1,2,0,2,0,1,1,2,1,1,2,2,2,2,0,2,0,2,2,2,0,2,0,1,1,2,1,1,2,2,2,2,1,1,2,1,0,2,2,2,0,2,2,2,2,2,0,2,0,2,0,2,0,2,2,2,0,2,0,2,2,2,2,2,0,2,0,2,0,2,0,2,0,2,0,2,0,1,1,0,1,0,2,0,2,0,1,0,2,0,1,0,2,0,2,0,2,0,2,0,2,0,2,0,1,0,2,0,1,0,2,0,2,0,1,0,1,1,2,0,2,0,2,0,2,0,2,0,2,0,0,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,0,0,2,0,2,0,0,0,0,0,0,2,0,2,2,0,0,0,0,2,2,0,2,2,2,0,2,0,0,0,0,0,2,0,0,0,0,2,2,0,2,2,2,0,2,0,2,2,2,2,2,2,2,2,2,0,2,0,2,2,2,0,2,0,0,0,0,0,2,0,0,0,0,0,2,0,2,2,2,0,2,0,0,0,0,0,2,0,0,0,0
    };
    ptr[  0 ] = 3;
    for( int ind = 1 ; ind < std::min( width , 243+1 ) ; ind++ ) {
     ptr[  ind * 4 ] = transition_tablePond[ind-1];
    }
    ptr += 4 * width;

    // Subtype 15: Strata
    GLubyte transition_tableStrata[243] = {
0,0,0,0,0,0,2,0,0,1,2,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,2,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
    };
    ptr[  0 ] = 3;
    for( int ind = 1 ; ind < std::min( width , 243+1 ) ; ind++ ) {
     ptr[  ind * 4 ] = transition_tableStrata[ind-1];
    }
    ptr += 4 * width;

    // Subtype 16: Tanks
    GLubyte transition_tableTanks[243] = {
0,1,0,1,1,2,0,2,0,1,1,2,1,1,2,2,2,2,0,2,0,2,2,2,0,2,0,1,1,2,1,1,2,2,2,2,1,1,2,1,0,2,2,2,0,2,2,2,2,2,0,2,0,2,0,2,0,2,2,2,0,2,0,2,2,2,2,2,0,2,0,2,0,2,0,2,0,2,0,2,0,1,1,0,1,0,2,0,2,0,1,0,2,0,1,0,2,0,2,0,2,0,2,0,2,0,2,0,1,0,2,0,1,0,2,0,2,0,1,0,1,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,0,2,0,2,2,2,0,2,0,2,2,2,2,2,0,2,0,0,0,2,0,2,0,0,0,0,0,2,2,2,2,2,0,2,0,0,2,2,0,2,2,2,0,2,0,2,0,0,0,2,0,0,0,0,0,2,0,2,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0
    };
    ptr[  0 ] = 3;
    for( int ind = 1 ; ind < std::min( width , 243+1 ) ; ind++ ) {
     ptr[  ind * 4 ] = transition_tableTanks[ind-1];
    }
    ptr += 4 * width;

    // Subtype 17: Typhoon
    GLubyte transition_tableTyphoon[243] = {
0,1,0,1,1,2,0,2,0,1,1,2,1,1,2,2,2,0,0,2,0,2,2,0,0,0,0,1,1,2,1,1,2,2,2,0,1,1,2,1,1,0,2,0,0,2,2,0,2,0,0,0,0,0,0,2,0,2,2,0,0,0,0,2,2,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,2,2,2,2,0,0,2,0,0,0,2,0,0,2,2,2,2,0,0,2,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,2,0,2,0,2,2,2,2,0,0,2,0,0,2,0,0,0,0,2,0,2,0,2,0,0,0,2,0,0,0,2,0,2,0,2,2,2,0,2,0,2,2,2,2,2,2,2,2,2,0,2,0,2,2,2,0,2,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0,2,0,2,2,2,0,2,0,2,2,2,2,2,2,2,2,2,0,2,0,2,2,2,0,2,0
    };
    ptr[  0 ] = 3;
    for( int ind = 1 ; ind < std::min( width , 243+1 ) ; ind++ ) {
     ptr[  ind * 4 ] = transition_tableTyphoon[ind-1];
    }
    ptr += 4 * width;

    // Subtype 18: Wave
    GLubyte transition_tableWave[243] = {
0,1,0,1,1,2,0,2,0,1,1,2,1,1,0,2,0,2,0,2,0,2,0,2,0,2,0,1,1,2,1,1,0,2,0,2,1,1,0,1,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,1,1,2,1,0,2,2,2,2,1,0,2,0,0,0,2,0,0,2,2,2,2,0,0,2,0,2,1,0,2,0,0,0,2,0,0,0,0,0,0,0,2,0,2,0,2,0,0,0,2,0,0,0,0,2,2,2,2,0,0,2,0,2,2,0,0,0,2,0,0,0,0,2,0,2,0,0,0,2,0,0,0,2,0,2,2,0,0,0,0,2,2,0,2,2,2,0,2,0,0,0,0,0,2,0,0,0,0,2,2,0,2,2,2,0,2,0,2,2,2,2,2,2,2,2,2,0,2,0,2,2,2,0,2,0,0,0,0,0,2,0,0,0,0,0,2,0,2,2,2,0,2,0,0,0,0,0,2,0,0,0,0
    };
    ptr[  0 ] = 3;
    for( int ind = 1 ; ind < std::min( width , 243+1 ) ; ind++ ) {
     ptr[  ind * 4 ] = transition_tableWave[ind-1];
    }
    ptr += 4 * width;






    glEnable(GL_TEXTURE_RECTANGLE);
    glBindTexture( GL_TEXTURE_RECTANGLE, textureID );
    glTexParameterf(GL_TEXTURE_RECTANGLE,GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_RECTANGLE,GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf( GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S , GL_CLAMP );
    glTexParameterf( GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP );
    glTexImage2D(GL_TEXTURE_RECTANGLE,     // Type of texture
		 0,                 // Pyramid level (for mip-mapping) - 0 is the top level
		 GL_RGBA8,            // Components: Internal colour format to convert to
		 width,          // Image width
		 height,          // Image heigh
		 0,                 // Border width in pixels (can either be 1 or 0)
		 GL_RGBA, // Format: Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
		 GL_UNSIGNED_BYTE,  // Image data type
		 data_table );        // The actual image data itself

    printOglError( 4 );
}

bool pg_initTextures( void ) {
   pg_screenMessageBitmap = 
    (GLubyte * )pg_generateTexture( &pg_screenMessageBitmap_ID , pg_byte_tex_format , 
				 PG_EnvironmentNode->message_pixel_length , 1 );
  if( !pg_screenMessageBitmap ) {
    sprintf( ErrorStr , "Error: screen message bitmap not allocated (%s)!" , ScreenMessage ); ReportError( ErrorStr ); throw 336;
  }
  
  for(int indTrack = 0 ; indTrack < PG_NB_TRACKS ; indTrack++ ) {
    pg_tracks_Pos_Texture[indTrack] = 
      (GLfloat * )pg_generateTexture( &(pg_tracks_Pos_Texture_ID[indTrack]) , pg_float_tex_format , 
				 PG_EnvironmentNode->Nb_max_mouse_recording_frames , 1 );
    if( !pg_tracks_Pos_Texture[indTrack] ) {
      sprintf( ErrorStr , "Error: pg_tracks_Pos_Texture not allocated (%s)!" , ScreenMessage ); ReportError( ErrorStr ); throw 336;
    }
    pg_tracks_Col_Texture[indTrack] = 
      (GLfloat * )pg_generateTexture( &(pg_tracks_Col_Texture_ID[indTrack]) , pg_float_tex_format , 
				 PG_EnvironmentNode->Nb_max_mouse_recording_frames , 1 );
    if( !pg_tracks_Col_Texture[indTrack] ) {
      sprintf( ErrorStr , "Error: pg_tracks_Col_Texture not allocated (%s)!" , ScreenMessage ); ReportError( ErrorStr ); throw 336;
    }
    pg_tracks_RadBrushRendmode_Texture[indTrack] = 
      (GLfloat * )pg_generateTexture( &(pg_tracks_RadBrushRendmode_Texture_ID[indTrack]) , pg_float_tex_format , 
				 PG_EnvironmentNode->Nb_max_mouse_recording_frames , 1 );
    if( !pg_tracks_RadBrushRendmode_Texture[indTrack] ) {
      sprintf( ErrorStr , "Error: pg_tracks_RadBrushRendmode_Texture not allocated (%s)!" , ScreenMessage ); ReportError( ErrorStr ); throw 336;
    }

    pg_tracks_Pos_target_Texture[indTrack] = 
      (GLfloat * )pg_generateTexture( &(pg_tracks_Pos_target_Texture_ID[indTrack]) , pg_float_tex_format , 
				 PG_EnvironmentNode->Nb_max_mouse_recording_frames , 1 );
    if( !pg_tracks_Pos_target_Texture[indTrack] ) {
      sprintf( ErrorStr , "Error: pg_tracks_Pos_Texture not allocated (%s)!" , ScreenMessage ); ReportError( ErrorStr ); throw 336;
    }
    pg_tracks_Col_target_Texture[indTrack] = 
      (GLfloat * )pg_generateTexture( &(pg_tracks_Col_target_Texture_ID[indTrack]) , pg_float_tex_format , 
				 PG_EnvironmentNode->Nb_max_mouse_recording_frames , 1 );
    if( !pg_tracks_Col_target_Texture[indTrack] ) {
      sprintf( ErrorStr , "Error: pg_tracks_Col_Texture not allocated (%s)!" , ScreenMessage ); ReportError( ErrorStr ); throw 336;
    }
    pg_tracks_RadBrushRendmode_target_Texture[indTrack] = 
      (GLfloat * )pg_generateTexture( &(pg_tracks_RadBrushRendmode_target_Texture_ID[indTrack]) , pg_float_tex_format , 
				 PG_EnvironmentNode->Nb_max_mouse_recording_frames , 1 );
    if( !pg_tracks_RadBrushRendmode_target_Texture[indTrack] ) {
      sprintf( ErrorStr , "Error: pg_tracks_RadBrushRendmode_Texture not allocated (%s)!" , ScreenMessage ); ReportError( ErrorStr ); throw 336;
    }
  }

#define width_data_table 600
#define height_data_table 200
  pg_CA_data_table = 
    (GLubyte * )pg_generateTexture( &pg_CA_data_table_ID , pg_byte_tex_format , 
				    width_data_table , height_data_table );
  pg_CA_data_table_values( pg_CA_data_table_ID , pg_CA_data_table, 
			   width_data_table , height_data_table  );
  if( !pg_CA_data_table_ID ) {
    sprintf( ErrorStr , "Error: data tables for the CA bitmap not allocated (%s)!" , ScreenMessage ); ReportError( ErrorStr ); throw 336;
  }  

  pg_loadTexture( PG_EnvironmentNode->font_file_name , &Font_image , 
	       &Font_texture_Rectangle , true , false , GL_RGB8 , GL_LUMINANCE , 
	       GL_UNSIGNED_BYTE , GL_LINEAR ,
	       128 , 70 );
  pg_loadTexture( (char *)(project_name+"/textures/pen.png").c_str() , &Pen_image , 
	       &Pen_texture_2D , false , true , GL_RGBA8 , GL_RGBA , 
	       GL_UNSIGNED_BYTE , GL_LINEAR ,
	       4096 , 512 );
  pg_loadTexture( (char *)(project_name+"/textures/sensor.png").c_str() , &Sensor_image , 
	       &Sensor_texture_rectangle , true , false , GL_RGBA8 , GL_RGBA , 
	       GL_UNSIGNED_BYTE , GL_LINEAR ,
	       300 , 100 );
  pg_loadTexture( (char *)(project_name+"/textures/LYMlogo.png").c_str() , &LYMlogo_image , 
	       &LYMlogo_texture_rectangle , true , false , GL_RGBA8 , GL_RGBA , 
	       GL_UNSIGNED_BYTE , GL_LINEAR ,
	       1024 , 768 );
  pg_loadTexture( (char *)(project_name+"/textures/trackBrushes.png").c_str() , &trackBrush_image , 
	       &trackBrush_texture_2D , false , true , GL_RGBA8 , GL_RGBA , 
	       GL_UNSIGNED_BYTE , GL_LINEAR ,
	       128 , 512 );
  pg_loadTexture3D( (char *)(project_name+"/textures/ParticleAcceleration_alK_3D").c_str() , 
		 ".png" , 7 , 4 , true ,
		 &Particle_acceleration_texture_3D , GL_RGBA8 , GL_RGBA ,
		 GL_LINEAR ,
		 2048 , 1024 , 7 );

  return true;
}

/////////////////////////////////////////////////////////////////
// FBO INITIALIZATION
/////////////////////////////////////////////////////////////////
GLuint drawBuffers[16] = {
  GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,
  GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3,
  GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5,
  GL_COLOR_ATTACHMENT6, GL_COLOR_ATTACHMENT7,
  GL_COLOR_ATTACHMENT8, GL_COLOR_ATTACHMENT9,
  GL_COLOR_ATTACHMENT10, GL_COLOR_ATTACHMENT11,
  GL_COLOR_ATTACHMENT12, GL_COLOR_ATTACHMENT13,
  GL_COLOR_ATTACHMENT14, GL_COLOR_ATTACHMENT15
};

/////////////////////////////////////////////
// FBO 
GLuint FBO_localColor_grayCA_tracks[PG_NB_MEMORIZED_PASS] = {0,0,0}; // nb_attachments=5
GLuint FBO_snapshot[2] = {0,0}; //  drawing memory on odd and even frames for echo and sensors
// FBO texture
GLuint FBO_texID_localColor_grayCA_tracks[PG_NB_MEMORIZED_PASS*NB_ATTACHMENTS] = {0,0,0,0,0,0,0,0,0}; // nb_attachments=5 - PG_NB_MEMORIZED_PASS=3
GLuint FBO_texID_snapshot[PG_NB_TRACKS - 1] = {0,0}; //  drawing memory on odd and even frames for echo and sensors

bool pg_initFBOTextures( GLuint *textureID , int nb_attachments ) {
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S , GL_REPEAT );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

  for( int indAtt = 0 ; indAtt < nb_attachments ; indAtt++ ) {
    glBindTexture(GL_TEXTURE_RECTANGLE, 
		  textureID[ indAtt ]);
    glTexParameteri(GL_TEXTURE_RECTANGLE,GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_RECTANGLE,GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexStorage2D(GL_TEXTURE_RECTANGLE,1,GL_RGBA32F, 
		   singleWindowWidth , windowHeight );
    glFramebufferTexture2D( GL_FRAMEBUFFER, 
			    GL_COLOR_ATTACHMENT0 + indAtt ,
			    GL_TEXTURE_RECTANGLE,
			    textureID[ indAtt ] , 
			    0 );
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER)
       ==GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT) {
      sprintf( ErrorStr , "Error: Binding RECT FBO texture No %d ID %d (error %d)!" , indAtt , textureID[ indAtt ] , glCheckFramebufferStatus(GL_FRAMEBUFFER) ); ReportError( ErrorStr ); throw 336;
    }
  }

  return true;
}

bool pg_initFBO( void ) {
  int maxbuffers;
  glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxbuffers);

  if( maxbuffers < NB_ATTACHMENTS ) {
    sprintf( ErrorStr , "Error: Maximal attachment (%d) -> %d required!" , maxbuffers , NB_ATTACHMENTS ); ReportError( ErrorStr ); throw 336;
  }

  glGenFramebuffers( PG_NB_MEMORIZED_PASS, FBO_localColor_grayCA_tracks );
  glGenTextures(PG_NB_MEMORIZED_PASS * NB_ATTACHMENTS, FBO_texID_localColor_grayCA_tracks);
  for( int indFB = 0 ; indFB < PG_NB_MEMORIZED_PASS ; indFB++ ) {
    glBindFramebuffer( GL_FRAMEBUFFER, FBO_localColor_grayCA_tracks[indFB] );
    pg_initFBOTextures(  FBO_texID_localColor_grayCA_tracks + indFB * NB_ATTACHMENTS , NB_ATTACHMENTS );
    glDrawBuffers( NB_ATTACHMENTS , drawBuffers);
  }

  glGenFramebuffers( 2, FBO_snapshot ); //  drawing memory on odd and even frames for echo and sensors
  glGenTextures(2, FBO_texID_snapshot); //  drawing memory on odd and even frames for echo and sensors
  for( int indFB = 0 ; indFB < 2 ; indFB++ ) {
    glBindFramebuffer( GL_FRAMEBUFFER, FBO_snapshot[indFB] );
    pg_initFBOTextures(  FBO_texID_snapshot + indFB , 1 );
    glDrawBuffers( 1 , drawBuffers);
  }

  glBindFramebuffer( GL_FRAMEBUFFER, 0 );
  return true;
}

/////////////////////////////////////////////////////////////////
// MATRIX INITIALIZATION
/////////////////////////////////////////////////////////////////
void pg_initRenderingMatrices( void ) {
  memset( (char *)viewMatrix , 0 , 16 * sizeof( float ) );
  memset( (char *)modelMatrix , 0 , 16 * sizeof( float ) );
  memset( (char *)modelMatrixSensor , 0 , 16 * sizeof( float ) );
  viewMatrix[0] = 1.0f;
  viewMatrix[5] = 1.0f;
  viewMatrix[10] = 1.0f;
  viewMatrix[15] = 1.0f;
  modelMatrix[0] = 1.0f;
  modelMatrix[5] = 1.0f;
  modelMatrix[10] = 1.0f;
  modelMatrix[15] = 1.0f;
  modelMatrixSensor[0] = 1.0f;
  modelMatrixSensor[5] = 1.0f;
  modelMatrixSensor[10] = 1.0f;
  modelMatrixSensor[15] = 1.0f;
  
  // ORTHO
  float l = 0.0f;
  float r = (float)singleWindowWidth;
  float b = 0.0f;
  float t = (float)windowHeight;
  float n = -1.0f;
  float f = 1.0f;
  GLfloat mat[] = { 
    (GLfloat)(2.0/(r-l)), 0.0, 0.0, 0.0, 
    0.0, (GLfloat)(2.0/(t-b)), 0.0, 0.0, 
    0.0, 0.0, (GLfloat)(2.0/(f-n)), 0.0, 
    (GLfloat)(-(r+l)/(r-l)), (GLfloat)(-(t+b)/(t-b)), (GLfloat)(-(f+n)/(f-n)), 1.0 };
  memcpy( (char *)projMatrix , mat , 16 * sizeof( float ) );
  // printf("Orthographic projection l %.2f r %.2f b %.2f t %.2f n %.2f f %.2f\n" , l,r,b,t,n,f);
  r = (float)doubleWindowWidth;
  GLfloat mat2[] = { 
    (GLfloat)(2.0/(r-l)), 0.0, 0.0, 0.0, 
    0.0, (GLfloat)(2.0/(t-b)), 0.0, 0.0, 
    0.0, 0.0, (GLfloat)(2.0/(f-n)), 0.0, 
    (GLfloat)(-(r+l)/(r-l)), (GLfloat)(-(t+b)/(t-b)), (GLfloat)(-(f+n)/(f-n)), 1.0 };
  memcpy( (char *)doubleProjMatrix , mat2 , 16 * sizeof( float ) );
  // printf("Double width orthographic projection l %.2f r %.2f b %.2f t %.2f n %.2f f %.2f\n" , l,r,b,t,n,f);
}

///////////////////////////////////////////////////////
// On-Screen Message Display
///////////////////////////////////////////////////////

void pg_screenMessage_update( void ) {
  // GLbyte *xfont = NULL;
  GLubyte *yfont = NULL;
  GLubyte *wfont = NULL;
  GLubyte *hfont = NULL;
  GLubyte *sfont = NULL;
  GLubyte *tfont = NULL;
  // GLubyte *afont = NULL;

  if( NewScreenMessage ) {
    switch( PG_EnvironmentNode->font_size ) {
    case 10:
      // xfont = stb__arial_10_usascii_x;
      yfont = stb__arial_10_usascii_y;
      wfont = stb__arial_10_usascii_w;
      hfont = stb__arial_10_usascii_h;
      sfont = stb__arial_10_usascii_s;
      tfont = stb__arial_10_usascii_t;
      // afont = stb__arial_10_usascii_a;
      break;
    case 11:
      // xfont = stb__arial_11_usascii_x;
      yfont = stb__arial_11_usascii_y;
      wfont = stb__arial_11_usascii_w;
      hfont = stb__arial_11_usascii_h;
      sfont = stb__arial_11_usascii_s;
      tfont = stb__arial_11_usascii_t;
      // afont = stb__arial_11_usascii_a;
      break;
    case 12:
      // xfont = stb__arial_12_usascii_x;
      yfont = stb__arial_12_usascii_y;
      wfont = stb__arial_12_usascii_w;
      hfont = stb__arial_12_usascii_h;
      sfont = stb__arial_12_usascii_s;
      tfont = stb__arial_12_usascii_t;
      // afont = stb__arial_12_usascii_a;
      break;
    case 13:
      // xfont = stb__arial_13_usascii_x;
      yfont = stb__arial_13_usascii_y;
      wfont = stb__arial_13_usascii_w;
      hfont = stb__arial_13_usascii_h;
      sfont = stb__arial_13_usascii_s;
      tfont = stb__arial_13_usascii_t;
      // afont = stb__arial_13_usascii_a;
      break;
    case 14:
      // xfont = stb__arial_14_usascii_x;
      yfont = stb__arial_14_usascii_y;
      wfont = stb__arial_14_usascii_w;
      hfont = stb__arial_14_usascii_h;
      sfont = stb__arial_14_usascii_s;
      tfont = stb__arial_14_usascii_t;
      // afont = stb__arial_14_usascii_a;
      break;
    case 15:
      // xfont = stb__arial_15_usascii_x;
      yfont = stb__arial_15_usascii_y;
      wfont = stb__arial_15_usascii_w;
      hfont = stb__arial_15_usascii_h;
      sfont = stb__arial_15_usascii_s;
      tfont = stb__arial_15_usascii_t;
      // afont = stb__arial_15_usascii_a;
      break;
    case 16:
      // xfont = stb__arial_16_usascii_x;
      yfont = stb__arial_16_usascii_y;
      wfont = stb__arial_16_usascii_w;
      hfont = stb__arial_16_usascii_h;
      sfont = stb__arial_16_usascii_s;
      tfont = stb__arial_16_usascii_t;
      // afont = stb__arial_16_usascii_a;
      break;
    case 17:
      // xfont = stb__arial_17_usascii_x;
      yfont = stb__arial_17_usascii_y;
      wfont = stb__arial_17_usascii_w;
      hfont = stb__arial_17_usascii_h;
      sfont = stb__arial_17_usascii_s;
      tfont = stb__arial_17_usascii_t;
      // afont = stb__arial_17_usascii_a;
      break;
    default:
    case 18:
      // xfont = stb__arial_18_usascii_x;
      yfont = stb__arial_18_usascii_y;
      wfont = stb__arial_18_usascii_w;
      hfont = stb__arial_18_usascii_h;
      sfont = stb__arial_18_usascii_s;
      tfont = stb__arial_18_usascii_t;
      // afont = stb__arial_18_usascii_a;
      break;
    }
   
    int pixelRank = 0;
    int lengthMax = PG_EnvironmentNode->message_pixel_length;
    memset(pg_screenMessageBitmap, (GLubyte)0, lengthMax * 4 );
    // strcpy( ScreenMessage , "abcdefghijklmnopqrstuvwxyabcdefghijklmnopqrstuvwxyabcdefghijklmnopqrstuvwxyabcdefghijklmnopqrstuvwxyabcdefghijklmnopqrstuvwxyabcdefghijklmnopqrstuv");

    // strcpy( ScreenMessage , "abcdefghijkl");
    // lengthMax = 30;

    for (char *c = ScreenMessage ; *c != '\0' && pixelRank < lengthMax ; c++) 
      {
      char cur_car = *c;
      if( cur_car == '�' || 
	  cur_car == '�' || 
	  cur_car == '�' || 
	  cur_car == '�' ) {
	cur_car = 'e';
      }
      if( cur_car == '�' || 
	  cur_car == '�' ) {
	cur_car = 'a';
      }
      if( cur_car == '�' || 
	  cur_car == '�' ) {
	cur_car = 'i';
      }
      if( cur_car == '�' || 
	  cur_car == '�' || 
	  cur_car == '�' || 
	  cur_car == '�' ) {
	cur_car = 'E';
      }
      if( cur_car == '�' || 
	  cur_car == '�' ) {
	cur_car = 'A';
      }
      if( cur_car == '�' || 
	  cur_car == '�' ) {
	cur_car = 'I';
      }

      // usable ascii starts at blank space
      int cur_car_rank = (int)cur_car - 32;
      cur_car_rank = (cur_car_rank < 0 ? 0 : cur_car_rank );
      cur_car_rank = (cur_car_rank > 94 ? 94 : cur_car_rank );

      // defines offset according to table
      for( int indPixel = 0 ; 
	   indPixel < wfont[ cur_car_rank ] && pixelRank + indPixel < lengthMax ; indPixel++ ) {
	int indPixelColor = (pixelRank + indPixel) * 4;
	pg_screenMessageBitmap[ indPixelColor ] = sfont[ cur_car_rank ]+indPixel;
	pg_screenMessageBitmap[ indPixelColor + 1 ] = tfont[ cur_car_rank ];
	pg_screenMessageBitmap[ indPixelColor + 2 ] = hfont[ cur_car_rank ];
	pg_screenMessageBitmap[ indPixelColor + 3 ] = yfont[ cur_car_rank ];
	// printf( "%d %d - " ,  sfont[ cur_car_rank ] ,  tfont[ cur_car_rank ] );
      }
      pixelRank += wfont[ cur_car_rank ] + 1;
    }

    glBindTexture( GL_TEXTURE_RECTANGLE, pg_screenMessageBitmap_ID );
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER,  
		    GL_NEAREST );
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, 
		    GL_NEAREST );
    glTexParameterf( GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S , GL_CLAMP );
    glTexParameterf( GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP );
    glTexImage2D(GL_TEXTURE_RECTANGLE,     // Type of texture
		 0,                 // Pyramid level (for mip-mapping) - 0 is the top level
		 GL_RGBA8,           // Internal colour format to convert to
		 lengthMax,         // Image width
		 1,                 // Image height
		 0,                 // Border width in pixels (can either be 1 or 0)
		 GL_RGBA,           // Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
		 GL_UNSIGNED_BYTE,  // Image data type
		 pg_screenMessageBitmap);        // The actual image data itself
    NewScreenMessage = false;
    printOglError( 6 );
  }
}


///////////////////////////////////////////////////////
// GLUT draw function (from the viewpoint)

// the glut callback
// requires predrawing (drawing from the user to the root node)

// ------------------------------------------------------------ //
// --------------- DISPLAYS WINDOWS ONE AFTER ANOTHER --------- //
// ------------------------------------------------------------ //
void window_display( void ) {
  // CurrentWindow = PG_EnvironmentNode->PG_Window;
  // glutSetWindow( CurrentWindow->glutID );

  PG_EnvironmentNode->windowDisplayed = true;

  // fprintf(fileLog,"begin window_display %.10f\n" , RealTime() );

  // OpenGL initializations before redisplay
  OpenGLInit();
 
  // fprintf(fileLog,"after OPenGLInit %.10f\n" , RealTime() );

  // proper scene redrawing
  pg_draw_scene( _Render );

  // fprintf(fileLog,"after draw scene %.10f\n" , RealTime() );

  // specific features for interactive environments with
  // messages
  
  // printf( "Window %s\n" , CurrentWindow->id );
  pg_screenMessage_update();

  // flushes OpenGL commands
  glFlush();

  glutSwapBuffers();
  // 

  // fprintf(fileLog,"after glutSwapBuffers %.10f\n" , RealTime() );

  // ------------------------------------------------------------ //
  // --------------- FRAME/SUBFRAME GRABBING -------------------- //

  // ---------------- frame by frame output --------------------- //
  // Svg screen shots
  // printf("Draw Svg\n" );
  if( PG_EnvironmentNode->outputSvg 
	&& FrameNo % PG_EnvironmentNode->stepSvg == 0
	&& FrameNo / PG_EnvironmentNode->stepSvg >= PG_EnvironmentNode->beginSvg && 
	FrameNo / PG_EnvironmentNode->stepSvg <= PG_EnvironmentNode->endSvg ) {
    // printf("Draw Svg %d\n" , FrameNo  );
    pg_draw_scene( _Svg );
  }
    
  // ---------------- frame by frame output --------------------- //
  // Png screen shots
  // printf("Draw Png\n" );
  if( PG_EnvironmentNode->outputPng 
	&& FrameNo % PG_EnvironmentNode->stepPng == 0
	&& FrameNo / PG_EnvironmentNode->stepPng >= PG_EnvironmentNode->beginPng && 
	FrameNo / PG_EnvironmentNode->stepPng <= PG_EnvironmentNode->endPng ) {
    pg_draw_scene( _Png );
  }
    
  // ---------------- frame by frame output --------------------- //
  // Jpg screen shots
  // printf("Draw Jpg\n"  );
  if( PG_EnvironmentNode->outputJpg 
	&& FrameNo % PG_EnvironmentNode->stepJpg == 0
	&& FrameNo / PG_EnvironmentNode->stepJpg >= PG_EnvironmentNode->beginJpg && 
	FrameNo / PG_EnvironmentNode->stepJpg <= PG_EnvironmentNode->endJpg ) {
    pg_draw_scene( _Jpg );
  }
}

//////////////////////////////////////////////////////////////////////
// SAVE IMAGE
//////////////////////////////////////////////////////////////////////

/* Attempts to save PNG to file; returns 0 on success, non-zero on error. */

int writepng(char *filename,
	     int x, int y, int width, int height ) {

  cv::Mat img( height, width, CV_8UC3 );

  // OpenGL's default 4 byte pack alignment would leave extra bytes at the
  //   end of each image row so that each full row contained a number of bytes
  //   divisible by 4.  Ie, an RGB row with 3 pixels and 8-bit componets would
  //   be laid out like "RGBRGBRGBxxx" where the last three "xxx" bytes exist
  //   just to pad the row out to 12 bytes (12 is divisible by 4). To make sure
  //   the rows are packed as tight as possible (no row padding), set the pack
  //   alignment to 1.
  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glReadPixels(x, y, width, height, GL_RGB, GL_UNSIGNED_BYTE, img.data);

  cv::Mat result;
  cv::flip(img, result , 0); // vertical flip
#ifndef OPENCV_3
  cv::cvtColor(result, result, CV_RGB2BGR);

  std::vector<int> params;
  params.push_back(CV_IMWRITE_PNG_COMPRESSION);
#else
  cv::cvtColor(result, result, cv::COLOR_RGB2BGR);

  std::vector<int> params;
  params.push_back(cv::IMWRITE_PNG_COMPRESSION);
#endif
  params.push_back(9);
  params.push_back(0);
  cv::imwrite( filename, result );

  return 0;
}


/* Attempts to save JPG to file; returns 0 on success, non-zero on error. */
int writejpg(char *filename,
	     int x, int y, int width, int height, int compression) {
  cv::Mat img( height, width, CV_8UC3 );

  // OpenGL's default 4 byte pack alignment would leave extra bytes at the
  //   end of each image row so that each full row contained a number of bytes
  //   divisible by 4.  Ie, an RGB row with 3 pixels and 8-bit componets would
  //   be laid out like "RGBRGBRGBxxx" where the last three "xxx" bytes exist
  //   just to pad the row out to 12 bytes (12 is divisible by 4). To make sure
  //   the rows are packed as tight as possible (no row padding), set the pack
  //   alignment to 1. 
  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glReadPixels(x, y, width, height, GL_RGB, GL_UNSIGNED_BYTE, img.data);

  cv::Mat result;
  cv::flip(img, result , 0); // vertical flip
#ifndef OPENCV_3
  cv::cvtColor(result, result, CV_RGB2BGR);

  std::vector<int> params;
  params.push_back(CV_IMWRITE_JPEG_QUALITY);
#else
  cv::cvtColor(result, result, cv::COLOR_RGB2BGR);

  std::vector<int> params;
  params.push_back(cv::IMWRITE_JPEG_QUALITY);
#endif
  params.push_back(70);   // that's percent, so 100 == no compression, 1 == full 
  cv::imwrite( filename, result );

  return 0;
}

//// sample choice
//// current sample choice
//int sample_choice[ PG_NB_SENSORS];
//// all possible sensor layouts
//int sample_setUps[ PG_NB_MAX_SAMPLE_SETUPS][ PG_NB_SENSORS ] =
//  {{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25},
//   {26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50},
//   {51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75}};
//// groups of samples for aliasing with additive samples
//int sample_groups[ 18 ][ 4 ] =
//  { { 1, 2, 6, 7 },
//    { 4, 5, 9, 10 },
//    { 16, 17, 21, 22 },
//    { 19, 20, 24, 25 },
//    { 8, 12, 14, 18 },
//    { 3, 11, 15, 23 },
//    
//    { 26, 27, 31, 32 },
//    { 29, 30, 34, 35 },
//    { 41, 42, 46, 48 },
//    { 44, 45, 49, 50 },
//    { 33, 37, 39, 43 },
//    { 28, 36, 40, 48 },
//    
//    { 51, 52, 56, 57 },
//    { 54, 55, 59, 60 },
//    { 66, 67, 71, 72 },
//    { 69, 70, 74, 75 },
//    { 58, 62, 64, 68 },
//    { 53, 61, 65, 73 } };

void readSensors( void ) {
  float sensorValues[PG_NB_SENSORS + 18];
  bool sensorOn[PG_NB_SENSORS + 18];

  bool sampleOn[PG_NB_MAX_SAMPLE_SETUPS * PG_NB_SENSORS];
  int sampleToSensorPointer[PG_NB_MAX_SAMPLE_SETUPS * PG_NB_SENSORS];

  GLubyte pixelColor[3 * PG_NB_SENSORS];

  // marks all the samples as unread
  for( int indSample = 0 ; indSample < PG_NB_MAX_SAMPLE_SETUPS * PG_NB_SENSORS ; indSample++ ) {
    sampleOn[indSample] = false;
    sampleToSensorPointer[indSample] = -1;
  }
    
  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  // sensor readback
  //printf("sensor on ");
  for( int indSens = 0 ; indSens < PG_NB_SENSORS ; indSens++ ) {
    if( sensor_onOff[ indSens ] ) {
      glReadPixels( (int)sensorPositions[ 3 * indSens ], 
    		    (int)(sensorPositions[ 3 * indSens + 1]), 
    		    1, 1, 
    		    GL_RGB, GL_UNSIGNED_BYTE, pixelColor + 3 * indSens );
      sensorValues[indSens] =
	( pixelColor[ 3 * indSens ] + pixelColor[ 3 * indSens + 1] + pixelColor[ 3 * indSens + 2])/(255.f * 3.f);
      sensorOn[indSens] = ( sensorValues[indSens] > 0.0f );
      if( sensorOn[indSens] ) {
          sampleOn[ sample_choice[ indSens ] ] = true;
	  sampleToSensorPointer[ sample_choice[ indSens ] ] = indSens;
	  //printf("%d ",indSens);
      }
    }
    else {
      sensorValues[indSens] = 0.0f;
      sensorOn[indSens] = false;
    }
  }
  //printf("\n");

  // looks for buffer aliasing possibilities: groups of sounds that could be replaced by a single buffer
  bool groupOn[18];
  float groupValues[18];
  //printf("group on ");
  for(int indgroup = 0 ; indgroup < 18 ; indgroup++ ) {
    if( sampleOn[ sample_groups[ indgroup ][ 0 ] ] && sampleOn[ sample_groups[ indgroup ][ 1 ] ] 
	&& sampleOn[ sample_groups[ indgroup ][ 2 ] ] && sampleOn[ sample_groups[ indgroup ][ 3 ] ] ) {
	  // switches on the group with the average activation value
	  groupOn[indgroup] = true;
	  groupValues[indgroup] = ( sensorValues[ sampleToSensorPointer[ sample_groups[ indgroup ][ 0 ] ] ] 
				    + sensorValues[ sampleToSensorPointer[ sample_groups[ indgroup ][ 1 ] ] ] 
				    + sensorValues[ sampleToSensorPointer[ sample_groups[ indgroup ][ 2 ] ] ] 
				    + sensorValues[ sampleToSensorPointer[ sample_groups[ indgroup ][ 3 ] ] ] );
	  // switches off the associated sensors
          sensorValues[sampleToSensorPointer[ sample_groups[ indgroup ][ 0 ] ]] = 0.0f;
          sensorOn[sampleToSensorPointer[ sample_groups[ indgroup ][ 0 ] ]] = false;
          sensorValues[sampleToSensorPointer[ sample_groups[ indgroup ][ 1 ] ]] = 0.0f;
          sensorOn[sampleToSensorPointer[ sample_groups[ indgroup ][ 1 ] ]] = false;
          sensorValues[sampleToSensorPointer[ sample_groups[ indgroup ][ 2 ] ]] = 0.0f;
          sensorOn[sampleToSensorPointer[ sample_groups[ indgroup ][ 2 ] ]] = false;
          sensorValues[sampleToSensorPointer[ sample_groups[ indgroup ][ 3 ] ]] = 0.0f;
          sensorOn[sampleToSensorPointer[ sample_groups[ indgroup ][ 3 ] ]] = false;
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
  for( int indSens = 0 ; indSens < PG_NB_SENSORS ; indSens++ ) {
    if( sensorOn[indSens] ) {
      float_str = std::to_string(static_cast<long double>(sensorValues[ indSens ]));
      // float_str.resize(4);
      message += " " + float_str;
      totalAmplitude += sensorValues[ indSens ];
    }
    else {
      message += " 0.0";
    }
  }
  for(int indgroup = 0 ; indgroup < 18 ; indgroup++ ) {
    if( groupOn[indgroup] ) {
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
  for( int indSens = 0 ; indSens < PG_NB_SENSORS ; indSens++ ) {
    format += "f ";
  }
  for(int indgroup = 0 ; indgroup < 18 ; indgroup++ ) {
    format += "f ";
  }
  // Total amplitude
  format += "f";

  // message posting
  pg_send_message_udp( (char *)format.c_str() , (char *)message.c_str() , (char *)"udp_SC_send" );

  // message trace
  //std::cout << "format: " << format << "\n";
  //std::cout << "msg: " << message << "\n";
}

//////////////////////////////////////////////////////
// SCENE RENDERING
//////////////////////////////////////////////////////

// generic interactive draw function (from the root)

// scene redisplay (controlled by a drawing mode: file, interactive
// or edition)

void pg_update_scene( void ) {
  ///////////////////////////////////////////////////////////////////////
  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
  // +++ mouse pointer adjustment according to music   +++++++ 
  // +++ notice that there is one frame delay   ++++++++++++++ 
  // +++ for taking these values in consideration   ++++++++++ 
  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
  mouse_x_deviation = xy_spectrum[0] * xy_spectrum_coef;
  mouse_y_deviation = xy_spectrum[1] * xy_spectrum_coef;
  bool is_trackreplay = false;
  switch( currentTrack ) {
    case 0:
      is_trackreplay = track_replay_0;
      break;
  }
  if( !is_trackreplay ) {
    if( activeDrawingStroke > 0 ) {
      tracks_x[ currentTrack ] = mouse_x;
      tracks_y[ currentTrack ] = mouse_y;
      tracks_x_prev[ currentTrack ] = mouse_x_prev;
      tracks_y_prev[ currentTrack ] = mouse_y_prev;
    }
    else {
      tracks_x[ currentTrack ] = -1;
      tracks_y[ currentTrack ] = -1;
      tracks_x_prev[ currentTrack ] = -1;
      tracks_y_prev[ currentTrack ] = -1;
    }
  }	
  // printf("Track %d %.1fx%.1f prev:  %.1fx%.1f\n",currentTrack,tracks_x[ currentTrack ],tracks_y[ currentTrack ],tracks_x_prev[ currentTrack ],tracks_y_prev[ currentTrack ]);
  //  <write_console value="Position: currentTrack activeDrawingStroke ({$config:track_replay[1]}) ({$config:tracks_BrushID[1]}) ({$config:tracks_RadiusX[1]}) ({$config:tracks_RadiusY[1]}) ({$config:tracks_x[1]}) ({$config:tracks_y[1]})"/>

  /////////////////////////////////////////////////////////////////////////
  // DRAWING SHADER UNIFORM VARIABLES
  glUseProgram(shader_Drawing_programme);

  // drawing off and drawing point or line 
  glUniform4f(uniform_Drawing_fs_4fv_W_H_drawingStart_drawingStroke ,
	      (GLfloat)singleWindowWidth ,
	      (GLfloat)windowHeight , 
	      (GLfloat)drawing_start_frame , (GLfloat)activeDrawingStroke );

  // acceleration center and CA subtype
  // in case of interpolation between CA1 and CA2 
 // if( !BrokenInterpolationVar[ _CA1_CA2_weight ] ) {
 //   if( CA1_CA2_weight < 1.0 && CA1_CA2_weight > 0.0 ) {
 //     float randVal = (float)rand() / (float)RAND_MAX;
 //     if( randVal <= CA1_CA2_weight ) {
	//CAType = CA1TypeSubType / 10;
	//CASubType = CA1TypeSubType % 10;
 //     }	
 //     else {
	//CAType = CA2TypeSubType / 10;
	//CASubType = CA2TypeSubType % 10;
 //     }	
 //   }
 //   else if( CA1_CA2_weight >= 1.0 ) {
 //     CAType = CA1TypeSubType / 10;
 //     CASubType = CA1TypeSubType % 10;
 //   }
 //   else if( CA1_CA2_weight <= 0.0 ) {
 //     CAType = CA2TypeSubType / 10;
 //     CASubType = CA2TypeSubType % 10;
 //   }
 //   // printf("CA type/subtype %d-%d\n" , CAType , CASubType );
 // }
  glUniform4f(uniform_Drawing_fs_4fv_CAType_CASubType_partAccCenter , 
	      (float)CAType , (float)CASubType , partAccCenter_0 , partAccCenter_1 );

  // particle acceleration
  glUniform4f(uniform_Drawing_fs_4fv_partMode_partAcc_clearLayer_void ,
	      (GLfloat)particleMode, part_acc_factor + particle_acc_attack,
	      (GLfloat)isClearAllLayers , 0.0f );

  // track decay
  glUniform3f(uniform_Drawing_fs_3fv_trackdecay_CAstep_initCA , 
	      trackdecay_0*trackdecay_sign_0 , (GLfloat)CAstep , initCA );
  // one shot CA launching
  if( initCA ) {
    initCA = 0.0f;
  }

  // CA decay, flash 
  glUniform4f(uniform_Drawing_fs_4fv_flashPart_isBeat_void_clearCA ,
	      (GLfloat)flashPart, (GLfloat)isBeat , 0.0f , (GLfloat)isClearCA );

  // CA type, frame no, flashback and current track
  glUniform4f(uniform_Drawing_fs_4fv_CAdecay_frameno_partTexture_currentTrack ,
	      CAdecay*CAdecay_sign, (GLfloat)FrameNo, particle_texture_ID, (GLfloat)currentTrack);

  // flash back & CA coefs
  glUniform4f(uniform_Drawing_fs_4fv_flashBackCoef_flashCACoefs,
	  flashBack_weight, flashCA_weights[0], flashCA_weights[1], flashCA_weights[2] );
  // printf("flashCA_weights %.2f %.2f %.2f \n",flashCA_weights[0], flashCA_weights[1], flashCA_weights[2] );

  // pen position storage on the two quads 
  glUniform3f(uniform_Drawing_fs_3fv_mouseTracks_replay , 
    (track_replay_0 ? 1.0f : -1.0f), 
    (-1.0f), 
    (-1.0f));
  glUniform4fv(uniform_Drawing_fs_4fv_mouseTracks_x , 1 , tracks_x);
  glUniform4fv(uniform_Drawing_fs_4fv_mouseTracks_y , 1 , tracks_y);
  glUniform4fv(uniform_Drawing_fs_4fv_mouseTracks_x_prev , 1 , tracks_x_prev);
  glUniform4fv(uniform_Drawing_fs_4fv_mouseTracks_y_prev , 1 , tracks_y_prev);
  
  // pen color
  glUniform3fv(uniform_Drawing_fs_3fv_mouseTracks_Color_r , 1 , tracks_Color_r);
  glUniform3fv(uniform_Drawing_fs_3fv_mouseTracks_Color_g , 1 , tracks_Color_g);
  glUniform3fv(uniform_Drawing_fs_3fv_mouseTracks_Color_b , 1 , tracks_Color_b);
  glUniform3fv(uniform_Drawing_fs_3fv_mouseTracks_Color_a , 1 , tracks_Color_a);
  
  // pen brush & size
  glUniform3f(uniform_Drawing_fs_3fv_mouseTracks_BrushID , (GLfloat)tracks_BrushID[0] , (GLfloat)tracks_BrushID[1] , (GLfloat)tracks_BrushID[2] );
  // printf("Current track %d BrushID %.2f %.2f %.2f\n" , currentTrack , tracks_BrushID[0] , tracks_BrushID[1] , tracks_BrushID[2] );
  
  glUniform4fv(uniform_Drawing_fs_4fv_mouseTracks_RadiusX , 1 , tracks_RadiusX);
  glUniform4fv(uniform_Drawing_fs_4fv_mouseTracks_RadiusY , 1 , tracks_RadiusY);

  /////////////////////////////////////////////////////////////////////////
  // COMPOSITION SHADER UNIFORM VARIABLES

  glUseProgram(shader_Composition_programme);

  // CA weight
  glUniform3f(uniform_Composition_fs_3fv_width_height_CAweight , (GLfloat)singleWindowWidth ,
		(GLfloat)windowHeight , CAweight );

  // track weight
  glUniform3f(uniform_Composition_fs_3fv_trackweight , 
	       trackweight_0 , 0.0f , 0.0f );

  // message transparency & echo
  glUniform4f(uniform_Composition_fs_4fv_messageTransparency_echo_echoNeg_invert , 
              messageTransparency , echo , echoNeg ,(invertAllLayers ? 1.0f : -1.0f) );    

  /////////////////////////////////////////////////////////////////////////
  // FINAL SHADER UNIFORM VARIABLES
  glUseProgram(shader_Final_programme);
  glUniform2f(uniform_Final_fs_2fv_transparency_scale, 
    blendTransp, scale );

  // hoover cursor
  glUniform4f(uniform_Final_fs_4fv_xy_frameno_cursorSize , 
	      (GLfloat)CurrentCursorHooverPos_x, (GLfloat)CurrentCursorHooverPos_y, 
		  (GLfloat)FrameNo, (GLfloat)PG_EnvironmentNode->cursorSize);
  glUniform2f(uniform_Final_fs_2fv_tablet1xy , 
	      xy_hoover_tablet1[0] , xy_hoover_tablet1[1] );

  /////////////////////////////////////////////////////////////////////////
  // SENSOR SHADER UNIFORM VARIABLES
  glUseProgram(shader_Sensor_programme);
  //glUniform2f(uniform_Sensor_fs_2fv_frameno_invert, 
  //         (GLfloat)FrameNo, (invertAllLayers ? 1.0f : -1.0f) );
  if( sensorFollowMouse_onOff && (mouse_x > 0 || mouse_y > 0) ) {
    sensorPositions[ 3 * currentSensor ] = mouse_x;
    sensorPositions[ 3 * currentSensor + 1 ] = windowHeight - mouse_y;
  }

  ///////////////////////////////////////////////////////////////////////
  // saves mouse values 
  mouse_x_prev = mouse_x;
  mouse_y_prev = mouse_y;

  ///////////////////////////////////////////////////////////////////////
  // flash reset: restores flash to 0 so that
  // it does not stay on more than one frame 
  for( int indtrack = 0 ; indtrack < PG_NB_TRACKS ; indtrack++ ) {
    flashCA_weights[indtrack] = 0;
  }

  if( flashPart > 0 ) {
    flashPart -= 1;
  }
  flashBack_weight = 0;

  // /////////////////////////
  // clear layer reset
  clearAllLayers = false;
  // clear CA reset
  isClearCA = 0;
  // clear layer reset
  isClearAllLayers = 0;
}

void pg_draw_scene( DrawingMode mode ) {

  // ******************** Svg output ********************
  if( mode == _Svg ) {
    sprintf( currentFilename , "%s%s.%07d.svg" , 
	     PG_EnvironmentNode->Svg_shot_dir_name ,
	     PG_EnvironmentNode->Svg_file_name , 
	     FrameNo / PG_EnvironmentNode->stepSvg );
    fprintf( fileLog , "Snapshot svg step %d (%s)\n" , 
	     FrameNo / PG_EnvironmentNode->stepSvg ,
	     currentFilename );
    writesvg(currentFilename,
	     singleWindowWidth , windowHeight );
  }

  // ******************** Png output ********************
  else if( mode == _Png ) {
    sprintf( currentFilename , "%s%s.%07d.png" , 
	     PG_EnvironmentNode->Png_shot_dir_name ,
	     PG_EnvironmentNode->Png_file_name , 
	     FrameNo / PG_EnvironmentNode->stepPng );
    fprintf( fileLog , "Snapshot png step %d (%s)\n" , 
	     FrameNo / PG_EnvironmentNode->stepPng ,
	     currentFilename );
    glReadBuffer(GL_FRONT);
    writepng(currentFilename, 0,0,
	     singleWindowWidth , windowHeight );
  }

  // ******************** Jpg output ********************
  else if( mode == _Jpg ) {
    sprintf( currentFilename , "%s%s.%07d.jpg" , 
	     PG_EnvironmentNode->Jpg_shot_dir_name ,
	     PG_EnvironmentNode->Jpg_file_name , 
	     FrameNo / PG_EnvironmentNode->stepJpg );
    fprintf( fileLog , "Snapshot jpg step %d (%s)\n" , 
	     FrameNo / PG_EnvironmentNode->stepJpg ,
	     currentFilename );
    printf( "Snapshot jpg step %d (%s)\n" , 
	     FrameNo / PG_EnvironmentNode->stepJpg ,
	     currentFilename );
    glReadBuffer(GL_FRONT);
    writejpg(currentFilename, 0,0,
	     singleWindowWidth , windowHeight ,75);
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
  // ********************  (OpenGL or evi3d) ********************
  else if( mode == _Render ) {

    //////////////////////////////////////////////////
    // SCENE UPDATE
    pg_update_scene();

    printOglError( 51 );

    //////////////////////////////////////////////////
    // PASS #1: PING PONG DRAWING

    // sets viewport to single window
    glViewport (0, 0, singleWindowWidth , windowHeight );

    // ping pong output and input FBO bindings
    // glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO_localColor_grayCA_tracks[(FrameNo % PG_NB_MEMORIZED_PASS)]);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO_localColor_grayCA_tracks[((FrameNo + 1) % PG_NB_MEMORIZED_PASS)]);

    // output buffer cleanup
    glDisable( GL_DEPTH_TEST );
    
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor( 0.0 , 0.0 , 0.0 , 1.0 );

    ////////////////////////////////////////
    // activate shaders and sets uniform variable values    
    glUseProgram (shader_Drawing_programme);
    glBindVertexArray (quadDraw_vao);
    glUniformMatrix4fv(uniform_Drawing_vp_proj, 1, GL_FALSE, projMatrix);
    glUniformMatrix4fv(uniform_Drawing_vp_view, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(uniform_Drawing_vp_model, 1, GL_FALSE, modelMatrix);
    
    // texture unit location
    glUniform1i(uniform_Drawing_texture_fs_decal, 0); 
    glUniform1i(uniform_Drawing_texture_fs_lookupTable1, 1); 
    glUniform1i(uniform_Drawing_texture_fs_lookupTable2, 2); 
    glUniform1i(uniform_Drawing_texture_fs_lookupTable3, 3); 
    glUniform1i(uniform_Drawing_texture_fs_lookupTable4, 4); 
    glUniform1i(uniform_Drawing_texture_fs_lookupTable5, 5); 
    glUniform1i(uniform_Drawing_texture_fs_lookupTable6, 6); 

    glTexParameterf( GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S , GL_CLAMP );
    glTexParameterf( GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP );

    // 3-cycle ping-pong localColor step n (FBO attachment 1)
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_RECTANGLE, FBO_texID_localColor_grayCA_tracks[(FrameNo % PG_NB_MEMORIZED_PASS) * NB_ATTACHMENTS]);

    // 3-cycle ping-pong speed/position of particles step n step n (FBO attachment 2)
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_RECTANGLE, FBO_texID_localColor_grayCA_tracks[(FrameNo % PG_NB_MEMORIZED_PASS) * NB_ATTACHMENTS + 1]);

    // 3-cycle ping-pong CA step n+2 (or n-1) (FBO attachment 1)
    glActiveTexture(GL_TEXTURE0 + 2);
    glBindTexture(GL_TEXTURE_RECTANGLE, FBO_texID_localColor_grayCA_tracks[((FrameNo + 2) % PG_NB_MEMORIZED_PASS) * NB_ATTACHMENTS + 2]);

    // 3-cycle ping-pong CA step n (FBO attachment 3)
    glActiveTexture(GL_TEXTURE0 + 3);
    glBindTexture(GL_TEXTURE_RECTANGLE, FBO_texID_localColor_grayCA_tracks[(FrameNo % PG_NB_MEMORIZED_PASS) * NB_ATTACHMENTS + 2]);

    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S , GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    // pen patterns
    glActiveTexture(GL_TEXTURE0 + 4);
    glBindTexture(GL_TEXTURE_2D, Pen_texture_2D );

    // particle acceleration texture
    glActiveTexture(GL_TEXTURE0 + 5);
    glBindTexture(GL_TEXTURE_3D, Particle_acceleration_texture_3D );

    // data tables for the CA
    glActiveTexture(GL_TEXTURE0 + 6);
    glBindTexture(GL_TEXTURE_RECTANGLE, pg_CA_data_table_ID );

   // draw points from the currently bound VAO with current in-use shader
    glDrawArrays (GL_TRIANGLES, 0, 3 * nbFaces);
     
    printOglError( 52 );

    //////////////////////////////////////////////////
    // PASS #2: COMPOSITION + PING-PONG ECHO

    // binds input to last output
    // glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO_localColor_grayCA_tracks[((FrameNo + 1) % PG_NB_MEMORIZED_PASS)]);
 
    /////////////////////////////////////////////////////////
    // draws the main rectangular surface with 
 
    // outputs inside a buffer that can be used for accumulation
    if( FrameNo > 0 ) {
      glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO_snapshot[(FrameNo % 2)]); //  drawing memory on odd and even frames for echo and sensors
    }

    // output video buffer clean-up
    glClear (GL_COLOR_BUFFER_BIT);
    glClearColor( 0.0 , 0.0 , 0.0 , 1.0 );

    ////////////////////////////////////////
    // activate shaders and sets uniform variable values    
    glUseProgram (shader_Composition_programme);
    glBindVertexArray (quadDraw_vao);

    glUniformMatrix4fv(uniform_Composition_vp_proj, 1, GL_FALSE, projMatrix);
    glUniformMatrix4fv(uniform_Composition_vp_view, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(uniform_Composition_vp_model, 1, GL_FALSE, modelMatrix);

    // texture unit location
    glUniform1i(uniform_Composition_texture_fs_decal, 0); 
    glUniform1i(uniform_Composition_texture_fs_lookupTable1, 1); 
    glUniform1i(uniform_Composition_texture_fs_lookupTable2, 2); 
    glUniform1i(uniform_Composition_texture_fs_lookupTable3, 3); 
    glUniform1i(uniform_Composition_texture_fs_lookupTable4, 4); 

    glUniform1i(uniform_Composition_texture_fs_lookupTable_font, 5); 
    glUniform1i(uniform_Composition_texture_fs_lookupTable_message, 6); 

    glTexParameterf( GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S , GL_CLAMP );
    glTexParameterf( GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP );
    // 3-cycle ping-pong localColor step n + 1 (FBO attachment 1)
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_RECTANGLE, FBO_texID_localColor_grayCA_tracks[((FrameNo + 1) % PG_NB_MEMORIZED_PASS) * NB_ATTACHMENTS]);

    // 3-cycle ping-pong CA step n + 1 (FBO attachment 3)
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_RECTANGLE, FBO_texID_localColor_grayCA_tracks[((FrameNo + 1) % PG_NB_MEMORIZED_PASS) * NB_ATTACHMENTS + 2]);

    // 3-cycle ping-pong track 1 step n + 1 (FBO attachment 4)
    glActiveTexture(GL_TEXTURE0 + 2);
    glBindTexture(GL_TEXTURE_RECTANGLE, FBO_texID_localColor_grayCA_tracks[((FrameNo + 1) % PG_NB_MEMORIZED_PASS) * NB_ATTACHMENTS + 3]);
    
    // 3-cycle ping-pong track 2 step n + 1 (FBO attachment 5)
    glActiveTexture(GL_TEXTURE0 + 3);
    glBindTexture(GL_TEXTURE_RECTANGLE, FBO_texID_localColor_grayCA_tracks[((FrameNo + 1) % PG_NB_MEMORIZED_PASS) * NB_ATTACHMENTS + 4]);

    // preceding snapshot
    glActiveTexture(GL_TEXTURE0 + 4);
    glBindTexture(GL_TEXTURE_RECTANGLE, FBO_texID_snapshot[(std::max( 0 , (FrameNo + 1)) % 2)] ); // drawing memory on odd and even frames for echo and sensors

    // font texture
    glActiveTexture(GL_TEXTURE0 + 5);
    glBindTexture(GL_TEXTURE_RECTANGLE, Font_texture_Rectangle);

    // font texture
    glActiveTexture(GL_TEXTURE0 + 6);
    glBindTexture(GL_TEXTURE_RECTANGLE, pg_screenMessageBitmap_ID);

    // draw points from the currently bound VAO with current in-use shader
    glDrawArrays (GL_TRIANGLES, 0, 3 * nbFaces);

    // /////////////////////////
    // read sensor values and send messages
    glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO_snapshot[(std::max( 0 , FrameNo) % 2)]); // drawing memory on odd and even frames for echo and sensors	
    if( FrameNo % 10 > 0 ) {
      readSensors();
    }
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0 );

    //////////////////////////////////////////////////
    // PASS #3: DISPLAY

    // unbind output FBO 
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    // sets viewport to double window
    glViewport (0, 0, doubleWindowWidth , windowHeight );

    glDrawBuffer(GL_BACK);

    // output video buffer clean-up
    glClear (GL_COLOR_BUFFER_BIT);
    glClearColor( 0.0 , 0.0 , 0.0 , 1.0 );
    
    // printf("rendering\n" );
    
    ////////////////////////////////////////
    // drawing last quad
    // activate shaders and sets uniform variable values    
    glUseProgram (shader_Final_programme);
    glBindVertexArray (quadFinal_vao);
    
    glUniformMatrix4fv(uniform_Final_vp_proj, 1, GL_FALSE, doubleProjMatrix);
    glUniformMatrix4fv(uniform_Final_vp_view, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(uniform_Final_vp_model, 1, GL_FALSE, modelMatrix);

    glTexParameterf( GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S , GL_CLAMP );
    glTexParameterf( GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP );
    // texture unit location
    glUniform1i(uniform_Final_texture_fs_decal, 0); 
    // previous pass output
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_RECTANGLE, FBO_texID_snapshot[(FrameNo % 2)]);
    
    // texture unit location
    glUniform1i(uniform_Final_texture_fs_lookupTable1, 1); 
    // previous pass output
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_RECTANGLE, LYMlogo_texture_rectangle );
    
    // draw points from the currently bound VAO with current in-use shader
    glDrawArrays (GL_TRIANGLES, 0, 3 * nbFaces);
    //}
    
    printOglError( 54 );

    ////////////////////////////////////////
    // drawing sensors
    // activate transparency
    glEnable( GL_BLEND );
    glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
   
    // activate shaders and sets uniform variable values    
    glUseProgram (shader_Sensor_programme);
    glBindVertexArray (quadSensor_vao);
    
    glTexParameterf( GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S , GL_CLAMP );
    glTexParameterf( GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP );
    // texture unit location
    glUniform1i(uniform_Sensor_texture_fs_decal, 0); 
    // previous pass output
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_RECTANGLE, Sensor_texture_rectangle );
    
    glUniformMatrix4fv(uniform_Sensor_vp_view, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(uniform_Sensor_vp_proj, 1, GL_FALSE, doubleProjMatrix);

    // sensor rendering
    for( int indSens = 0 ; indSens < PG_NB_SENSORS ; indSens++ ) {
      if( sensor_onOff[indSens] ) {
	modelMatrixSensor[12] = (sensorPositions[ 3 * indSens ] + 0.5f - singleWindowWidth/2.0f) * scale + singleWindowWidth/2.0f;
	modelMatrixSensor[13] = (sensorPositions[ 3 * indSens + 1] + 0.5f - windowHeight/2.0f) * scale + windowHeight/2.0f;
	modelMatrixSensor[14] = sensorPositions[ 3 * indSens + 2];
	glUniformMatrix4fv(uniform_Sensor_vp_model, 1, GL_FALSE, modelMatrixSensor);

	glUniform4f(uniform_Sensor_fs_4fv_onOff_isCurrentSensor_isFollowMouse_transparency , 
	     (sensor_onOff[indSens] ? 1.0f : -1.0f) , (indSens == currentSensor ? 1.0f : -1.0f) , 
	     (sensorFollowMouse_onOff ? 1.0f : -1.0f) , blendTransp );

	// draw points from the currently bound VAO with current in-use shader
	glDrawArrays (GL_TRIANGLES, 0, 3 * nbFaces);
      }
      else {
        // incremental sensor activation every 10 sec.
	if( sensor_activation == 5 && CurrentClockTime - sensor_last_activation_time > 10 ) {
	   sensor_last_activation_time = CurrentClockTime;
	   sensor_onOff[indSens] = true;
	}
      }
    }
    // duplicates the sensors in case of double window
    if( PG_EnvironmentNode->double_window ) {
      for( int indSens = 0 ; indSens < PG_NB_SENSORS ; indSens++ ) {
	if( sensor_onOff[indSens] ) {
	  modelMatrixSensor[12] = (sensorPositions[ 3 * indSens ] + 0.5f - singleWindowWidth/2.0f) * scale + 3 * singleWindowWidth/2.0f;
	  modelMatrixSensor[13] = (sensorPositions[ 3 * indSens + 1] + 0.5f - windowHeight/2.0f) * scale + windowHeight/2.0f;
	  modelMatrixSensor[14] = sensorPositions[ 3 * indSens + 2];
	  glUniformMatrix4fv(uniform_Sensor_vp_model, 1, GL_FALSE, modelMatrixSensor);

	  glUniform4f(uniform_Sensor_fs_4fv_onOff_isCurrentSensor_isFollowMouse_transparency , 
	       (sensor_onOff[indSens] ? 1.0f : -1.0f) , (indSens == currentSensor ? 1.0f : -1.0f) , 
	       (sensorFollowMouse_onOff ? 1.0f : -1.0f) , blendTransp );

	  // draw points from the currently bound VAO with current in-use shader
	  glDrawArrays (GL_TRIANGLES, 0, 3 * nbFaces);
	}
      }
    }

    printOglError( 595 );
    glDisable( GL_BLEND );

  }


  // // flushes OpenGL commands
  // glFlush();

}

