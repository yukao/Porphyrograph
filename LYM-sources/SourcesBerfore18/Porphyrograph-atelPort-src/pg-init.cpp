/*! \file pg-init.cpp
*
*
*     File pg-init.cpp
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

#include "pg-all_include.h"

float damping = 0.f;


/////////////////////////////////////////////////////////////////
// Projection and view matrices for the shaders
//////////////////////////////////////////////////////////////////////
GLfloat projMatrix[16];
GLfloat doubleProjMatrix[16];
GLfloat viewMatrix[16];
GLfloat modelMatrix[16];

//////////////////////////////////////////////////////////////////////
// GEOMETRY
//////////////////////////////////////////////////////////////////////
// geometry: quads
GLuint pg_vaoID[pg_VAOCount];
GLuint pg_vboID[pg_VBOCount];

// geometrical data
float quadDraw_points[] = {
	0.0f,         0.0f,  0.0f, //1st triangle
	0.0f,         0.0f,  0.0f,
	0.0f,         0.0f,  0.0f,
	0.0f,         0.0f,  0.0f, //2nd triangle
};
float quadDraw_texCoords[] = {
	0.0f,         0.0f, //1st triangle
	0.0f,         0.0f,
	0.0f,         0.0f,
	0.0f,         0.0f, //2nd triangle
};
unsigned int quadDraw_indices[PG_SIZE_QUAD_ARRAY] = { 2, 1, 0, 3 };

// geometrical data
float quadMaster_points[] = {
	0.0f,         0.0f,  0.0f, //1st triangle
	0.0f,         0.0f,  0.0f,
	0.0f,         0.0f,  0.0f,
	0.0f,         0.0f,  0.0f, //2nd triangle
};
float quadMaster_texCoords[] = {
	0.0f,         0.0f, //1st triangle
	0.0f,         0.0f,
	0.0f,         0.0f,
	0.0f,         0.0f, //2nd triangle
};
unsigned int quadMaster_indices[PG_SIZE_QUAD_ARRAY] = { 2, 1, 0, 3 };

// particle curves
#ifdef CURVE_PARTICLES
GLfloat *pg_Particle_control_points;
#endif
#if defined BLURRED_SPLAT_PARTICLES || defined LINE_SPLAT_PARTICLES
GLfloat *pg_Particle_vertices;
GLfloat *pg_Particle_radius;
GLfloat *pg_Particle_colors;
#endif
unsigned int *pg_Particle_indices;

//////////////////////////////////////////////////////////////////////
// TEXTURES
//////////////////////////////////////////////////////////////////////
//GLuint pg_Particle_Pos_Texture_texID = 0;
//GLfloat *pg_Particle_Pos_Texture = NULL;

#ifdef CURVE_PARTICLES
// comet texture
cv::Mat comet_image;
GLuint comet_texture_2D_texID;
#endif
#ifdef BLURRED_SPLAT_PARTICLES
// blurred disk texture
cv::Mat blurredDisk_image;
GLuint blurredDisk_texture_2D_texID;
#endif

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
GLuint pg_FBO_Update[2]; // nb_attachments=6
GLuint pg_FBO_Mixing_capturedFB_prec[2] = { 0,0 }; //  drawing memory on odd and even frames for echo
// GLuint FBO_CameraFrame = 0; //  video preprocessing outcome
GLuint pg_FBO_Particle = 0; // particle rendering

						  // FBO texture
GLuint pg_FBO_Update_texID[2*PG_NB_ATTACHMENTS]; // nb_attachments=9
GLuint pg_FBO_Mixing_capturedFB_prec_texID[2] = { 0,0 }; // drawing memory on odd and even frames for echo 
// GLuint FBO_CameraFrame_texID = 0; // video preprocessing outcome 
GLuint pg_FBO_Particle_texID = 0; // particle rendering

//////////////////////////////////////////////////////////////////////
// TEXT
//////////////////////////////////////////////////////////////////////

GLbyte stb__arial_10_usascii_x[95] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,
-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, };
GLubyte stb__arial_10_usascii_y[95] = { 8,1,1,1,1,1,1,1,1,1,1,2,7,5,
7,1,1,1,1,1,1,1,1,1,1,1,3,3,2,3,2,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,9,1,3,1,3,1,3,1,3,1,1,
1,1,1,3,3,3,3,3,3,3,1,3,3,3,3,3,3,1,1,1,4, };
GLubyte stb__arial_10_usascii_w[95] = { 0,2,3,5,5,8,6,2,3,3,4,5,2,3,
2,3,5,4,5,5,5,5,5,5,5,5,2,2,5,5,5,5,9,7,6,7,6,6,6,7,6,2,4,6,
5,7,6,7,6,7,7,6,6,6,6,9,6,6,6,3,3,2,4,7,3,5,5,5,5,5,3,5,5,2,
3,5,2,7,5,5,5,5,4,5,3,5,5,7,5,5,5,3,2,3,5, };
GLubyte stb__arial_10_usascii_h[95] = { 0,7,3,8,8,8,8,3,9,9,4,5,3,2,
1,8,8,7,7,8,7,8,8,7,8,8,5,7,6,4,6,7,9,7,7,8,7,7,7,8,7,7,8,7,
7,7,7,8,7,8,7,8,7,8,7,7,7,7,7,9,8,9,4,1,2,6,8,6,8,6,7,7,7,7,
9,7,7,5,5,6,7,7,5,6,8,6,5,5,5,7,5,9,9,9,2, };
GLubyte stb__arial_10_usascii_s[95] = { 127,80,80,58,76,82,91,84,1,37,72,
95,77,93,101,110,24,104,13,36,109,70,98,60,114,104,73,123,25,121,31,
7,20,115,66,46,97,90,83,9,73,29,53,53,47,39,32,41,22,1,14,
120,1,17,113,103,96,89,82,30,42,34,67,104,97,37,64,49,30,43,66,
70,60,120,16,8,123,113,107,61,54,76,76,19,49,55,101,87,67,1,81,
12,9,5,87, };
GLubyte stb__arial_10_usascii_t[95] = { 1,20,34,1,1,1,1,34,1,1,34,
28,34,34,34,1,11,20,28,11,20,1,1,20,1,1,28,20,28,28,28,
28,1,20,20,11,20,20,20,11,20,20,1,20,20,20,20,1,20,11,20,
1,20,11,11,11,11,11,11,1,11,1,34,34,34,28,1,28,11,28,11,
11,11,11,1,20,11,28,28,28,11,11,28,28,1,28,28,28,28,28,28,
1,1,1,34, };
GLubyte stb__arial_10_usascii_a[95] = { 40,40,51,80,80,127,96,27,
48,48,56,84,40,48,40,40,80,80,80,80,80,80,80,80,
80,80,40,40,84,84,84,80,145,96,96,103,103,96,87,111,
103,40,72,96,80,119,103,111,96,111,103,96,87,103,96,135,
96,96,87,40,40,40,67,80,48,80,80,72,80,80,40,80,
80,32,32,72,32,119,80,80,80,80,48,72,40,80,72,103,
72,72,72,48,37,48,84, };

GLbyte stb__arial_11_usascii_x[95] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,
-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, };
GLubyte stb__arial_11_usascii_y[95] = { 8,0,0,0,0,0,0,0,0,0,0,2,7,5,
7,0,0,0,0,0,0,1,0,1,0,0,2,2,2,3,2,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,0,2,0,2,0,2,0,2,0,0,
0,0,0,2,2,2,2,2,2,2,1,2,2,2,2,2,2,0,0,0,3, };
GLubyte stb__arial_11_usascii_w[95] = { 0,2,4,6,6,9,7,2,3,3,4,6,2,3,
2,3,6,3,5,6,6,6,6,6,6,6,2,2,6,6,6,5,10,8,7,7,7,7,6,8,7,2,5,7,
6,8,7,8,7,8,7,7,6,7,7,10,7,7,6,3,3,3,5,7,3,6,6,5,5,6,4,5,5,2,
3,5,2,8,5,6,6,5,4,5,3,5,5,8,5,5,5,4,2,4,6, };
GLubyte stb__arial_11_usascii_h[95] = { 0,8,4,9,10,9,9,4,11,11,4,5,3,1,
1,9,9,8,8,9,8,8,9,7,9,9,6,8,5,3,5,8,11,8,8,9,8,8,8,9,8,8,9,8,
8,8,8,9,8,9,8,9,8,9,8,8,8,8,8,10,9,10,5,1,3,7,9,7,9,7,8,9,8,8,
11,8,8,6,6,7,8,8,6,7,8,7,6,6,6,9,6,11,11,11,3, };
GLubyte stb__arial_11_usascii_s[95] = { 125,88,95,67,41,88,98,100,1,33,90,
76,107,124,125,106,39,9,36,52,29,22,81,60,110,1,125,122,69,110,83,
42,22,13,107,117,1,91,115,69,99,53,63,80,73,64,56,58,45,30,37,
8,24,16,12,1,117,109,102,48,59,37,63,103,103,73,74,67,46,87,78,
24,83,125,18,31,125,100,115,80,89,96,121,48,20,54,94,48,109,52,57,
13,10,5,117, };
GLubyte stb__arial_11_usascii_t[95] = { 10,23,40,1,1,1,1,40,1,1,40,
40,40,40,30,1,13,32,32,13,32,32,1,32,1,13,23,23,40,40,40,
32,1,32,23,1,32,23,23,13,23,23,13,23,23,23,23,1,23,13,23,
13,23,13,23,23,13,13,13,1,13,1,40,44,40,32,1,32,13,32,13,
13,13,1,1,23,13,32,32,32,13,13,32,32,23,32,32,40,32,1,40,
1,1,1,40, };
GLubyte stb__arial_11_usascii_a[95] = { 44,44,56,88,88,140,105,30,
52,52,61,92,44,52,44,44,88,88,88,88,88,88,88,88,
88,88,44,44,92,92,92,88,160,105,105,114,114,105,96,123,
114,44,79,105,88,131,114,123,105,123,114,105,96,114,105,149,
105,105,96,44,44,44,74,88,52,88,88,79,88,88,44,88,
88,35,35,79,35,131,88,88,88,88,52,79,44,88,79,114,
79,79,79,53,41,53,92, };


GLbyte stb__arial_12_usascii_x[95] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,1,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,
-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, };
GLubyte stb__arial_12_usascii_y[95] = { 9,1,1,1,0,1,1,1,1,1,1,2,7,5,
7,1,1,1,1,1,1,1,1,1,1,1,3,3,2,3,2,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,10,1,3,1,3,1,3,1,3,1,1,
1,1,1,3,3,3,3,3,3,3,1,3,3,3,3,3,3,1,1,1,4, };
GLubyte stb__arial_12_usascii_w[95] = { 0,3,4,6,6,9,7,2,4,4,4,6,3,4,
3,3,6,4,6,6,6,6,6,6,6,6,3,3,6,6,6,6,11,9,7,8,8,7,7,8,7,2,5,8,
6,9,7,8,7,8,8,7,7,7,8,11,8,8,7,3,3,3,5,8,3,6,6,6,6,6,4,6,6,2,
3,6,2,9,6,6,6,6,4,5,3,6,6,8,6,6,6,4,2,4,6, };
GLubyte stb__arial_12_usascii_h[95] = { 0,8,4,9,11,9,9,4,11,11,4,6,4,2,
2,9,9,8,8,9,8,9,9,8,9,9,6,8,6,4,6,8,11,8,8,9,8,8,8,9,8,8,9,8,
8,8,8,9,8,9,8,9,8,9,8,8,8,8,8,11,9,11,5,2,2,7,9,7,9,7,8,9,8,8,
11,8,8,6,6,7,9,9,6,7,9,7,6,6,6,9,6,11,11,11,3, };
GLubyte stb__arial_12_usascii_s[95] = { 125,116,98,89,1,96,106,95,50,29,86,
55,91,110,123,121,41,1,56,55,70,69,114,31,85,10,62,77,66,79,18,
63,34,46,38,1,22,14,6,76,120,113,100,104,97,87,79,66,71,17,55,
33,47,92,31,19,10,1,115,46,106,21,73,114,110,88,62,101,48,115,110,
26,40,125,25,64,123,8,1,108,82,75,122,95,55,81,41,25,48,59,34,
16,13,8,103, };
GLubyte stb__arial_12_usascii_t[95] = { 10,21,41,1,1,1,1,41,1,1,41,
41,41,44,41,1,13,32,32,11,32,11,1,32,11,13,41,32,41,41,41,
32,1,32,32,13,32,32,32,11,21,21,11,21,21,21,21,1,21,13,21,
13,23,11,23,23,23,23,11,1,11,1,41,41,41,32,11,32,13,32,11,
13,23,1,1,21,11,41,41,32,1,1,32,32,1,32,41,41,41,1,41,
1,1,1,41, };
GLubyte stb__arial_12_usascii_a[95] = { 48,48,61,96,96,153,115,33,
57,57,67,100,48,57,48,48,96,96,96,96,96,96,96,96,
96,96,48,48,100,100,100,96,174,115,115,124,124,115,105,134,
124,48,86,115,96,143,124,134,115,134,124,115,105,124,115,162,
115,115,105,48,48,48,81,96,57,96,96,86,96,96,48,96,
96,38,38,86,38,143,96,96,96,96,57,86,48,96,86,124,
86,86,86,57,45,57,100, };

GLbyte stb__arial_13_usascii_x[95] = { 0,1,0,0,0,0,0,0,0,0,0,0,0,0,
1,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,1,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,
-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0, };
GLubyte stb__arial_13_usascii_y[95] = { 10,1,1,1,0,1,1,1,1,1,1,3,8,6,
8,1,1,1,1,1,1,1,1,1,1,1,3,3,3,4,3,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,11,1,3,1,3,1,3,1,3,1,1,
1,1,1,3,3,3,3,3,3,3,1,3,3,3,3,3,3,1,1,1,4, };
GLubyte stb__arial_13_usascii_w[95] = { 0,2,4,7,6,10,8,2,4,4,5,7,3,4,
2,4,6,4,6,6,6,7,6,6,6,6,2,3,7,7,7,6,12,9,8,8,8,8,7,9,8,2,5,8,
7,9,8,9,8,9,9,8,7,8,8,11,8,8,7,4,4,3,6,8,3,6,6,6,6,6,4,6,6,2,
3,6,2,9,6,7,7,6,5,6,4,6,6,9,6,6,6,4,1,4,7, };
GLubyte stb__arial_13_usascii_h[95] = { 0,9,4,10,12,10,10,4,12,12,5,6,4,2,
2,10,10,9,9,10,9,10,10,9,10,10,7,9,6,4,6,9,12,9,9,10,9,9,9,10,9,9,10,9,
9,9,9,10,9,10,9,10,9,10,9,9,9,9,9,12,10,12,6,2,3,8,10,8,10,8,9,10,9,9,
12,9,9,7,7,8,10,10,7,8,10,8,7,7,7,10,7,12,12,12,3, };
GLubyte stb__arial_13_usascii_s[95] = { 56,125,1,93,1,101,112,123,51,28,105,
97,111,30,27,10,48,19,76,62,90,76,121,50,94,15,47,97,89,115,81,
83,33,66,57,1,41,32,24,84,10,122,110,1,114,104,95,68,83,22,66,
39,55,101,39,27,18,9,1,46,116,20,74,18,6,108,69,1,55,16,121,
32,48,63,24,76,92,30,23,8,85,78,122,115,56,101,40,50,67,61,60,
15,13,8,10, };
GLubyte stb__arial_13_usascii_t[95] = { 12,25,54,1,1,1,1,45,1,1,45,
45,45,54,54,14,14,35,35,14,35,14,1,35,14,14,45,35,45,45,45,
35,1,35,35,14,35,35,35,14,35,25,14,35,25,25,25,1,25,14,25,
14,25,14,25,25,25,25,25,1,14,1,45,54,54,35,14,45,14,45,14,
14,25,25,1,25,25,45,45,45,1,1,35,35,1,35,45,45,45,1,45,
1,1,1,54, };
GLubyte stb__arial_13_usascii_a[95] = { 52,52,66,104,104,166,124,36,
62,62,72,109,52,62,52,52,104,104,104,104,104,104,104,104,
104,104,52,52,109,109,109,104,189,124,124,134,134,124,114,145,
134,52,93,124,104,155,134,145,124,145,134,124,114,134,124,176,
124,124,114,52,52,52,87,104,62,104,104,93,104,104,52,104,
104,41,41,93,41,155,104,104,104,104,62,93,52,104,93,134,
93,93,93,62,48,62,109, };

GLbyte stb__arial_14_usascii_x[95] = { 0,1,0,0,0,0,0,0,0,0,0,0,1,0,
1,0,0,1,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,-1,0,0,0,0,1,0,1,1,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,
-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0, };
GLubyte stb__arial_14_usascii_y[95] = { 11,2,2,1,1,1,1,2,1,1,1,3,9,7,
9,1,1,1,1,1,2,2,1,2,1,1,4,4,3,4,3,1,1,2,2,1,2,2,2,1,2,2,2,2,
2,2,2,1,2,1,2,1,2,2,2,2,2,2,2,2,1,2,1,12,1,4,2,4,2,4,1,4,2,2,
2,2,2,4,4,4,4,4,4,4,2,4,4,4,4,4,4,1,1,1,5, };
GLubyte stb__arial_14_usascii_w[95] = { 0,2,4,7,7,11,9,2,4,4,5,7,2,4,
2,4,7,4,7,7,7,7,7,7,7,7,2,2,7,7,7,7,13,10,8,9,9,8,7,9,8,2,6,9,
7,10,9,10,8,10,9,8,8,9,9,12,9,9,8,4,4,3,6,9,3,7,7,7,7,7,4,7,7,2,
3,7,2,10,7,7,7,7,5,6,4,7,7,9,7,7,6,4,2,4,7, };
GLubyte stb__arial_14_usascii_h[95] = { 0,9,4,11,12,11,11,4,13,13,5,7,4,2,
2,11,11,10,10,11,9,10,11,9,11,11,7,9,7,5,7,10,13,9,9,11,9,9,9,11,9,9,10,9,
9,9,9,11,9,11,9,11,9,10,9,9,9,9,9,12,11,12,6,2,3,8,10,8,10,8,10,10,9,9,
12,9,9,7,7,8,10,10,7,8,10,8,7,7,7,10,7,13,13,13,3, };
GLubyte stb__arial_14_usascii_s[95] = { 59,120,123,70,42,78,108,57,1,19,48,
104,54,123,72,118,1,9,22,9,1,1,17,110,90,45,59,107,62,40,88,
115,24,49,98,98,80,71,63,53,40,123,108,30,22,11,1,59,111,25,88,
36,69,30,49,98,78,59,40,50,123,55,33,75,60,27,84,51,76,12,71,
63,90,60,38,118,9,112,12,35,92,100,20,20,123,43,70,78,96,14,26,
14,11,6,64, };
GLubyte stb__arial_14_usascii_t[95] = { 13,27,48,1,1,1,1,57,1,1,57,
48,57,53,57,1,15,27,27,15,48,27,15,37,1,15,48,37,48,57,48,
15,1,37,37,1,37,37,37,15,37,27,15,38,38,38,38,1,27,15,27,
15,27,27,27,27,27,27,27,1,1,1,57,57,57,48,15,48,15,48,15,
15,37,37,1,37,48,48,57,48,15,15,57,48,15,48,48,48,48,27,57,
1,1,1,57, };
GLubyte stb__arial_14_usascii_a[95] = { 56,56,71,112,112,178,134,38,
67,67,78,117,56,67,56,56,112,112,112,112,112,112,112,112,
112,112,56,56,117,117,117,112,204,134,134,145,145,134,122,156,
145,56,100,134,112,167,145,156,134,156,145,134,122,145,134,189,
134,134,122,56,56,56,94,112,67,112,112,100,112,112,56,112,
112,45,45,100,45,167,112,112,112,112,67,100,56,112,100,145,
100,100,100,67,52,67,117, };

GLbyte stb__arial_15_usascii_x[95] = { 0,1,0,0,0,0,0,0,0,0,0,0,1,0,
1,0,0,1,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,-1,0,0,1,1,1,0,1,1,0,0,
0,0,1,0,1,0,1,0,0,1,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,
-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0, };
GLubyte stb__arial_15_usascii_y[95] = { 12,2,2,2,1,2,2,2,2,2,2,4,10,7,
10,2,2,2,2,2,2,2,2,2,2,2,5,5,4,5,4,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,13,2,4,2,4,2,4,2,4,2,2,
2,2,2,4,4,4,4,4,4,4,2,5,5,5,5,5,5,2,2,2,6, };
GLubyte stb__arial_15_usascii_w[95] = { 0,2,5,8,7,12,9,2,4,4,5,8,2,5,
2,4,7,5,7,7,7,7,7,7,7,7,2,2,8,8,8,7,14,10,9,10,8,8,7,10,8,2,6,9,
7,11,8,10,8,10,9,9,8,8,9,13,9,9,8,4,4,3,6,9,4,7,7,7,7,7,5,7,7,3,
4,7,3,11,7,7,7,7,5,7,4,7,7,10,7,7,7,5,2,5,8, };
GLubyte stb__arial_15_usascii_h[95] = { 0,10,4,11,13,11,11,4,13,13,5,7,4,3,
2,11,11,10,10,11,10,11,11,10,11,11,7,9,7,5,7,10,13,10,10,11,10,10,10,11,10,10,11,10,
10,10,10,11,10,11,10,11,10,11,10,10,10,10,10,13,11,13,6,2,3,9,11,9,11,9,10,11,10,10,
13,10,10,8,8,9,11,11,8,9,11,8,7,7,7,10,7,13,13,13,3, };
GLubyte stb__arial_15_usascii_s[95] = { 127,68,60,79,1,104,117,66,58,29,54,
1,69,72,102,1,25,12,105,67,97,83,59,43,109,33,100,123,21,45,103,
18,34,1,113,14,88,79,71,10,59,1,102,33,25,13,4,68,117,91,103,
41,86,1,72,58,48,38,29,49,117,54,38,92,78,42,6,34,75,58,122,
51,21,82,24,95,113,66,92,50,96,88,78,26,63,84,30,10,112,51,120,
18,15,9,83, };
GLubyte stb__arial_15_usascii_t[95] = { 1,39,61,1,1,1,1,61,1,1,61,
61,61,61,61,15,15,50,39,15,39,15,15,39,15,15,50,39,61,61,50,
50,1,50,39,15,39,39,39,27,39,39,15,39,39,39,39,1,27,15,27,
15,27,27,27,27,27,27,27,1,15,1,61,61,61,50,15,50,15,50,15,
15,27,27,1,27,27,50,50,50,1,1,50,50,1,50,61,61,50,39,50,
1,1,1,61, };
GLubyte stb__arial_15_usascii_a[95] = { 60,60,76,119,119,191,143,41,
72,72,84,125,60,72,60,60,119,119,119,119,119,119,119,119,
119,119,60,60,125,125,125,119,218,143,143,155,155,143,131,167,
155,60,107,143,119,179,155,167,143,167,155,143,131,155,143,203,
143,143,131,60,60,60,101,119,72,119,119,107,119,119,60,119,
119,48,48,107,48,179,119,119,119,119,72,107,60,119,107,155,
107,107,107,72,56,72,125, };

GLbyte stb__arial_16_usascii_x[95] = { 0,1,0,0,0,0,0,0,0,0,0,0,1,0,
1,0,0,1,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,-1,1,0,1,1,1,0,1,1,0,1,
1,1,1,0,1,0,1,0,0,1,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,
-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0, };
GLubyte stb__arial_16_usascii_y[95] = { 12,1,1,1,0,1,1,1,1,1,1,3,10,7,
10,1,1,1,1,1,1,1,1,1,1,1,4,4,3,4,3,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,13,1,4,1,4,1,4,1,4,1,1,
1,1,1,4,4,4,4,4,4,4,1,4,4,4,4,4,4,1,1,1,5, };
GLubyte stb__arial_16_usascii_w[95] = { 0,2,5,8,8,12,10,3,5,5,6,8,2,5,
2,4,8,5,8,8,8,8,8,8,8,8,2,2,8,8,8,8,15,11,8,10,9,8,8,11,9,2,7,9,
7,10,9,11,8,11,10,9,9,9,10,14,10,10,9,4,4,4,7,10,4,8,8,8,7,8,5,8,7,3,
4,8,3,12,7,8,8,7,5,7,4,7,7,11,8,8,7,5,2,5,8, };
GLubyte stb__arial_16_usascii_h[95] = { 0,11,5,12,14,12,12,5,15,15,5,8,5,2,
2,12,12,11,11,12,11,12,12,11,12,12,8,11,8,6,8,11,15,11,11,12,11,11,11,12,11,11,12,11,
11,11,11,12,11,12,11,12,11,12,11,11,11,11,11,14,12,14,7,2,3,9,12,9,12,9,11,12,11,11,
15,11,11,8,8,9,11,11,8,9,12,9,8,8,8,12,8,15,15,15,4, };
GLubyte stb__arial_16_usascii_s[95] = { 125,125,109,111,54,1,14,105,1,43,95,
60,102,82,79,120,85,93,31,102,49,13,94,1,25,45,124,125,69,86,14,
40,27,19,10,34,117,108,99,1,83,125,103,73,65,54,44,82,31,54,11,
75,1,22,102,87,76,65,55,63,120,49,78,68,115,66,111,83,94,101,41,
66,113,121,22,22,40,1,110,92,32,47,118,75,68,58,43,23,51,73,35,
16,13,7,115, };
GLubyte stb__arial_16_usascii_t[95] = { 13,17,67,1,1,17,17,67,1,1,67,
67,67,14,14,1,17,43,55,17,55,30,1,55,17,17,55,29,67,67,67,
55,1,55,55,17,43,43,43,30,43,1,1,43,43,43,43,1,43,17,43,
17,43,30,30,30,30,30,30,1,17,1,67,14,72,55,17,55,17,55,30,
17,30,30,1,43,43,67,55,55,30,30,55,55,1,55,67,67,67,1,67,
1,1,1,67, };
GLubyte stb__arial_16_usascii_a[95] = { 64,64,81,127,127,204,153,44,
76,76,89,134,64,76,64,64,127,127,127,127,127,127,127,127,
127,127,64,64,134,134,134,127,233,153,153,165,165,153,140,178,
165,64,115,153,127,191,165,178,153,178,165,153,140,165,153,216,
153,153,140,64,64,64,108,127,76,127,127,115,127,127,64,127,
127,51,51,115,51,191,127,127,127,127,76,115,64,127,115,165,
115,115,115,77,60,77,134, };

GLbyte stb__arial_17_usascii_x[95] = { 0,1,0,0,0,0,0,0,0,0,0,0,1,0,
1,0,0,1,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,-1,1,0,1,1,1,0,1,1,0,1,
1,1,1,0,1,0,1,0,0,1,0,0,0,0,0,1,0,0,0,-1,0,0,0,0,0,0,0,0,1,1,
-1,1,0,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0, };
GLubyte stb__arial_17_usascii_y[95] = { 13,2,2,1,1,1,1,2,1,1,1,4,11,8,
11,1,2,2,2,2,2,2,2,2,2,2,5,5,3,5,3,1,1,2,2,1,2,2,2,1,2,2,2,2,
2,2,2,1,2,1,2,1,2,2,2,2,2,2,2,2,1,2,1,15,2,4,2,4,2,4,1,4,2,2,
2,2,2,4,4,4,4,4,4,4,2,5,5,5,5,5,5,1,1,1,6, };
GLubyte stb__arial_17_usascii_w[95] = { 0,2,5,9,8,13,10,3,5,5,6,9,2,5,
2,5,8,5,8,8,8,8,8,8,8,8,2,2,9,9,9,8,15,12,9,11,10,9,8,11,9,2,7,10,
7,11,9,12,9,12,10,10,9,9,11,15,11,11,9,3,5,4,7,10,4,8,8,8,8,8,5,8,7,2,
4,7,3,11,7,8,7,8,6,8,5,8,8,11,8,8,8,5,2,5,9, };
GLubyte stb__arial_17_usascii_h[95] = { 0,11,4,13,14,13,13,4,16,16,6,8,5,2,
2,13,12,11,11,12,11,12,12,11,12,12,8,11,9,5,9,12,16,11,11,13,11,11,11,13,11,11,12,11,
11,11,11,13,11,13,11,13,11,12,11,11,11,11,11,15,13,15,7,2,3,10,12,10,12,10,12,13,11,11,
15,11,11,9,9,10,13,13,9,10,12,9,8,8,8,12,8,16,16,16,3, };
GLubyte stb__arial_17_usascii_s[95] = { 127,71,121,80,58,90,1,11,1,22,114,
78,125,30,125,69,62,42,45,19,63,43,89,54,107,1,125,122,1,1,20,
28,28,32,22,21,11,1,113,33,99,124,75,78,70,58,89,67,32,45,13,
58,114,52,102,86,74,1,48,54,121,49,106,36,25,116,116,107,98,98,83,
12,72,125,44,24,109,30,49,89,113,104,42,80,37,11,97,66,57,10,88,
16,13,7,15, };
GLubyte stb__arial_17_usascii_t[95] = { 1,32,69,1,1,1,18,79,1,1,69,
69,54,79,60,18,32,45,57,32,57,32,18,57,18,32,45,45,69,79,69,
32,1,57,57,18,57,57,45,18,45,32,18,45,45,45,45,1,45,18,45,
18,32,32,32,32,32,45,45,1,1,1,69,79,79,57,18,57,18,57,18,
18,57,18,1,45,45,69,69,57,1,1,69,57,32,69,69,69,69,32,69,
1,1,1,79, };
GLubyte stb__arial_17_usascii_a[95] = { 68,68,86,135,135,216,162,46,
81,81,95,142,68,81,68,68,135,135,135,135,135,135,135,135,
135,135,68,68,142,142,142,135,247,162,162,176,176,162,149,189,
176,68,122,162,135,203,176,189,162,189,176,162,149,176,162,230,
162,162,149,68,68,68,114,135,81,135,135,122,135,135,68,135,
135,54,54,122,54,203,135,135,135,135,81,122,68,135,122,176,
122,122,122,81,63,81,142, };

GLbyte stb__arial_18_usascii_x[95] = { 0,1,0,0,0,0,0,0,0,0,0,0,1,0,
1,0,0,1,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,-1,1,0,1,1,1,0,1,1,0,1,
1,1,1,0,1,0,1,0,0,1,0,0,0,0,0,1,0,0,0,-1,0,0,1,0,0,0,0,0,1,1,
-1,1,1,1,1,0,1,0,1,0,0,1,0,0,0,0,0,0,1,0,0, };
GLubyte stb__arial_18_usascii_y[95] = { 14,2,2,2,1,2,2,2,2,2,2,4,12,9,
12,2,2,2,2,2,2,2,2,2,2,2,5,5,4,5,4,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,16,2,5,2,5,2,5,2,5,2,2,
2,2,2,5,5,5,5,5,5,5,2,5,5,5,5,5,5,2,2,2,7, };
GLubyte stb__arial_18_usascii_w[95] = { 0,3,5,9,9,14,11,3,5,5,6,9,3,5,
3,5,9,6,9,9,9,9,9,9,9,9,3,3,9,9,9,9,16,12,9,11,10,9,9,12,10,3,7,10,
8,12,10,12,10,12,11,10,10,10,11,16,11,11,10,4,5,4,8,11,4,9,8,8,8,9,6,8,7,2,
4,7,2,12,7,9,8,8,5,8,5,7,8,12,8,8,8,6,2,5,9, };
GLubyte stb__arial_18_usascii_h[95] = { 0,12,5,13,15,13,13,5,16,16,6,9,5,2,
2,13,13,12,12,13,12,13,13,12,13,13,9,12,9,6,9,12,16,12,12,13,12,12,12,13,12,12,13,12,
12,12,12,13,12,13,12,13,12,13,12,12,12,12,12,16,13,16,7,2,3,10,13,10,13,10,12,13,12,12,
16,12,12,9,9,10,13,13,9,10,13,10,9,9,9,13,9,16,16,16,3, };
GLubyte stb__arial_18_usascii_s[95] = { 127,117,49,117,61,1,16,45,1,38,34,
116,41,86,82,50,99,121,76,118,96,10,28,43,33,56,1,106,5,24,66,
86,44,63,53,38,32,22,12,20,1,113,54,102,93,80,69,86,55,66,35,
88,21,43,1,110,98,86,75,33,62,23,15,70,55,118,1,10,109,29,68,
79,13,32,28,47,66,47,39,19,108,99,60,1,71,110,98,76,107,77,89,
16,13,7,60, };
GLubyte stb__arial_18_usascii_t[95] = { 1,46,83,1,1,18,18,83,1,1,83,
72,83,83,83,18,18,46,59,18,59,32,18,59,32,18,83,59,83,83,72,
59,1,59,59,18,59,59,59,32,59,46,32,46,46,46,46,1,46,18,46,
18,46,32,46,32,32,32,32,1,32,1,83,83,83,59,32,72,18,72,32,
18,46,46,1,46,46,72,72,72,1,1,72,72,1,59,72,72,72,1,72,
1,1,1,83, };
GLubyte stb__arial_18_usascii_a[95] = { 72,72,92,143,143,229,172,49,
86,86,100,151,72,86,72,72,143,143,143,143,143,143,143,143,
143,143,72,72,151,151,151,143,255,172,172,186,186,172,157,201,
// 143,143,72,72,151,151,151,143,262,172,172,186,186,172,157,201,
186,72,129,172,143,215,186,201,172,201,186,172,157,186,172,243,
172,172,157,72,72,72,121,143,86,143,143,129,143,143,72,143,
143,57,57,129,57,215,143,143,143,143,86,129,72,143,129,186,
129,129,129,86,67,86,151, };

//////////////////////////////////////////////////////////////////////////////////////////////
// INTERFACE INITIALIZATION
//////////////////////////////////////////////////////////////////////////////////////////////

void InterfaceInitializations(void) {
	for (int indColorPreset = 0; indColorPreset < nb_pen_colorPresets; indColorPreset++) {
		sprintf(AuxString, "/pen_colorPreset_name_can %d %s", 
			indColorPreset, pen_colorPresets_names[indColorPreset].c_str()); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_QT_send");
	}
}


//////////////////////////////////////////////////////////////////////////////////////////////
// GEOMETRY INITIALIZATION
//////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
// ERRORS
/////////////////////////////////////////////////////////////////
int printOglError(int no) {

	int    retCode = 0;

	GLenum err;
	if (((err = glGetError()) != GL_NO_ERROR)) {
		printf("glError at %d -> %d: %s\n", no,
			err, gluErrorString(err));
		retCode = 1;
	}
	return retCode;
}

void ReportError(char *errorString) {
	if (fileLog) {
		fprintf(fileLog, "%s\n", errorString);
	}

	fprintf(stderr, "%s\n", errorString);
}


/////////////////////////////////////////////////////////////////
// TIME
/////////////////////////////////////////////////////////////////

#ifdef _WIN32
static const double epoch = 116444736000000000.;
int gettimeofday(struct timeval* tp, void* tzp) {
	SYSTEMTIME system_time;
	FILETIME file_time;
	ULARGE_INTEGER ularge;

	GetSystemTime(&system_time);
	SystemTimeToFileTime(&system_time, &file_time);
	ularge.LowPart = file_time.dwLowDateTime;
	ularge.HighPart = file_time.dwHighDateTime;

	tp->tv_sec = (long)((ularge.QuadPart - epoch) / 10000000L);
	tp->tv_usec = (long)(system_time.wMilliseconds * 1000);
	/* 0 indicates that the call succeeded. */
	return 0;
}
#endif

double RealTime(void) {
	struct timeval time;
	gettimeofday(&time, 0);
	double realtime = (double)time.tv_sec + ((double)time.tv_usec / 1000000.);
	return (realtime - InitialRealTime) * time_scale;
}

void MemoryUsage(void) {
#ifndef _WIN32
	// struct rusage {
	//   struct timeval ru_utime; /* user time used */
	//   struct timeval ru_stime; /* system time used */
	//   long   ru_maxrss;        /* maximum resident set size */
	//   long   ru_ixrss;         /* integral shared memory size */
	//   long   ru_idrss;         /* integral unshared data size */
	//   long   ru_isrss;         /* integral unshared stack size */
	//   long   ru_minflt;        /* page reclaims */
	//   long   ru_majflt;        /* page faults */
	//   long   ru_nblend;         /* blends */
	//   long   ru_inblock;       /* block input operations */
	//   long   ru_oublock;       /* block output operations */
	//   long   ru_msgsnd;        /* messages sent */
	//   long   ru_msgrcv;        /* messages received */
	//   long   ru_nsignals;      /* signals received */
	//   long   ru_nvcsw;         /* voluntary context switches */
	//   long   ru_nivcsw;        /* involuntary context switches */
	// };

	struct rusage usage;

	getrusage(RUSAGE_SELF, &usage);

	printf("Memory usage %ld %ld %ld %ld\n", usage.ru_maxrss, usage.ru_ixrss, usage.ru_idrss, usage.ru_isrss);
#endif
}

/////////////////////////////////////////////////////////////////
// GEOMETRY INITIALIZATION
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
// INTIALIZATION OF THE TWO QUADS USED FOR DISPLAY: DRAWING AND COMPOSITION QUADS
void pg_initGeometry_quads(void) {
	for (int ind = 0; ind < pg_VAOCount; ind++) {
		pg_vaoID[ind] = 0;
	}
	for (int ind = 0; ind < pg_VBOCount; ind++) {
		pg_vboID[ind] = 0;
	}
	glGenVertexArrays(pg_VAOCount, pg_vaoID);
	glGenBuffers(pg_VBOCount, pg_vboID);

	/////////////////////////////////////////////////////////////////////
	// QUAD FOR DRAWING AND COMPOSITION
	// point positions and texture coordinates
	quadDraw_points[1] = (float)window_height;
	quadDraw_points[3] = (float)leftWindowWidth;
	quadDraw_points[9] = (float)leftWindowWidth;
	quadDraw_points[10] = (float)window_height;

	quadDraw_texCoords[1] = (float)window_height;
	quadDraw_texCoords[2] = (float)leftWindowWidth;
	quadDraw_texCoords[6] = (float)leftWindowWidth;
	quadDraw_texCoords[7] = (float)window_height;

	quadMaster_points[1] = (float)window_height;
	quadMaster_points[3] = (float)doubleWindowWidth;
	quadMaster_points[9] = (float)doubleWindowWidth;
	quadMaster_points[10] = (float)window_height;

	quadMaster_texCoords[1] = (float)window_height;
	quadMaster_texCoords[2] = (float)doubleWindowWidth;
	quadMaster_texCoords[6] = (float)doubleWindowWidth;
	quadMaster_texCoords[7] = (float)window_height;
	printOglError(21);

	/////////////////////////////////////////////////////////////////////
	// single quad for update

	// vertex buffer objects and vertex array
	unsigned int quadDraw_points_vbo = 0;
	glGenBuffers(1, &quadDraw_points_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, quadDraw_points_vbo);
	glBufferData(GL_ARRAY_BUFFER,
		3 * 4 * sizeof(float),
		quadDraw_points,
		GL_STATIC_DRAW);
	unsigned int quadDraw_texCoord_vbo = 0;
	glGenBuffers(1, &quadDraw_texCoord_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, quadDraw_texCoord_vbo);
	glBufferData(GL_ARRAY_BUFFER,
		2 * 4 * sizeof(float),
		quadDraw_texCoords,
		GL_STATIC_DRAW);

	// vertex array object
	glBindVertexArray(pg_vaoID[pg_VAOQuad]);
	// vertex positions are location 0
	glBindBuffer(GL_ARRAY_BUFFER, quadDraw_points_vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
	glEnableVertexAttribArray(0);

	// texture coordinates are location 1
	glBindBuffer(GL_ARRAY_BUFFER, quadDraw_texCoord_vbo);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
	glEnableVertexAttribArray(1); // don't forget this!

	// vertex indices for indexed rendering 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pg_vboID[pg_EABQuad]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, PG_SIZE_QUAD_ARRAY * sizeof(unsigned int),
		quadDraw_indices, GL_STATIC_DRAW);

	printOglError(22);

	/////////////////////////////////////////////////////////////////////
	// double quad for composition

	// vertex buffer objects and vertex array
	unsigned int quadMaster_points_vbo = 0;
	glGenBuffers(1, &quadMaster_points_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, quadMaster_points_vbo);
	glBufferData(GL_ARRAY_BUFFER,
		3 * 4 * sizeof(float),
		quadMaster_points,
		GL_STATIC_DRAW);
	unsigned int quadMaster_texCoord_vbo = 0;
	glGenBuffers(1, &quadMaster_texCoord_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, quadMaster_texCoord_vbo);
	glBufferData(GL_ARRAY_BUFFER,
		2 * 4 * sizeof(float),
		quadMaster_texCoords,
		GL_STATIC_DRAW);

	// vertex array object
	glBindVertexArray(pg_vaoID[pg_VAOQuadMaster]);
	// vertex positions are location 0
	glBindBuffer(GL_ARRAY_BUFFER, quadMaster_points_vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
	glEnableVertexAttribArray(0);

	// texture coordinates are location 1
	glBindBuffer(GL_ARRAY_BUFFER, quadMaster_texCoord_vbo);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
	glEnableVertexAttribArray(1); // don't forget this!

	// vertex indices for indexed rendering 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pg_vboID[pg_EABQuadMaster]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, PG_SIZE_QUAD_ARRAY * sizeof(unsigned int),
		quadMaster_indices, GL_STATIC_DRAW);
	printOglError(23);

	/////////////////////////////////////////////////////////////////////
	// PARTICLES TO BE TESSELATED
	// initializes the arrays that contains the positions and the indices of the particles
	pg_initParticlePosition_Texture();

#ifdef CURVE_PARTICLES
	// vertex buffer objects and vertex array
	glBindVertexArray(pg_vaoID[pg_VAOParticle]);
	// vertices
	glBindBuffer(GL_ARRAY_BUFFER, pg_vboID[pg_VBOParticle]);
	glBufferData(GL_ARRAY_BUFFER, nb_particles * (PG_PARTICLE_CURVE_DEGREE + 1) * 3 * sizeof(float), pg_Particle_control_points, GL_STATIC_DRAW);
#endif
#if defined BLURRED_SPLAT_PARTICLES || defined LINE_SPLAT_PARTICLES
	// vertex buffer objects and vertex array
	glBindVertexArray(pg_vaoID[pg_VAOParticle]);
	// vertices
	glBindBuffer(GL_ARRAY_BUFFER, pg_vboID[pg_VBOParticle]);
	glBufferData(GL_ARRAY_BUFFER, nb_particles * 3 * sizeof(float), pg_Particle_vertices, GL_STATIC_DRAW);
	// radius
	glBindBuffer(GL_ARRAY_BUFFER, pg_vboID[pg_VBOpartRadius]);
	glBufferData(GL_ARRAY_BUFFER, nb_particles * 1 * sizeof(float), pg_Particle_radius, GL_STATIC_DRAW);
	// color
	glBindBuffer(GL_ARRAY_BUFFER, pg_vboID[pg_VBOParticleColors]);
	glBufferData(GL_ARRAY_BUFFER, nb_particles * 3 * sizeof(float), pg_Particle_colors, GL_STATIC_DRAW);
#endif

	// vertex positions are location 0
	glBindBuffer(GL_ARRAY_BUFFER, pg_vboID[pg_VBOParticle]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
	glEnableVertexAttribArray(0);

#if defined BLURRED_SPLAT_PARTICLES || defined LINE_SPLAT_PARTICLES
	// radius is location 1
	glBindBuffer(GL_ARRAY_BUFFER, pg_vboID[pg_VBOpartRadius]);
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
	glEnableVertexAttribArray(1); // don't forget this!
	// color is location 2
	glBindBuffer(GL_ARRAY_BUFFER, pg_vboID[pg_VBOParticleColors]);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
	glEnableVertexAttribArray(2); // don't forget this!
#endif

#ifdef CURVE_PARTICLES
	// vertex indices for indexed rendering 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pg_vboID[pg_EAOParticle]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, nb_particles * (PG_PARTICLE_CURVE_DEGREE + 1) * sizeof(unsigned int),
		pg_Particle_indices, GL_STATIC_DRAW);
#endif
#if defined BLURRED_SPLAT_PARTICLES || defined LINE_SPLAT_PARTICLES
	// vertex indices for indexed rendering 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pg_vboID[pg_EAOParticle]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, nb_particles * sizeof(unsigned int),
		pg_Particle_indices, GL_STATIC_DRAW);
#endif

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	printOglError(32);

	printf("Geometry initialized %dx%d & %dx%d\n", leftWindowWidth, window_height,
		doubleWindowWidth, window_height);
}


/////////////////////////////////////////////////////////////////
// FBO TEXTURES INITIALIZATION
/////////////////////////////////////////////////////////////////

bool pg_initFBOTextures(GLuint *textureID, int nb_attachments) {
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	for (int indAtt = 0; indAtt < nb_attachments; indAtt++) {
		glBindTexture(GL_TEXTURE_RECTANGLE, textureID[indAtt]);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexStorage2D(GL_TEXTURE_RECTANGLE, 1, GL_RGBA32F,
			leftWindowWidth, window_height);
		// printf("FBO size %d %d \n" , leftWindowWidth , window_height );
		glFramebufferTexture2D(GL_FRAMEBUFFER,
			GL_COLOR_ATTACHMENT0 + indAtt,
			GL_TEXTURE_RECTANGLE,
			textureID[indAtt],
			0);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER)
			== GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT) {
			sprintf(ErrorStr, "Error: Binding RECT FBO texture No %d ID %d (error %d)!", indAtt, textureID[indAtt], glCheckFramebufferStatus(GL_FRAMEBUFFER)); ReportError(ErrorStr); throw 336;
		}
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
// FBO INITIALIZATION
/////////////////////////////////////////////////////////////////
bool pg_initFBO(void) {
	int maxbuffers;
	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxbuffers);
	int maxDrawBuf;
	glGetIntegerv(GL_MAX_DRAW_BUFFERS, &maxDrawBuf);

	if (maxbuffers < PG_NB_ATTACHMENTS) {
		sprintf(ErrorStr, "Error: Maximal attachment (%d) -> %d required!", maxbuffers, PG_NB_ATTACHMENTS); ReportError(ErrorStr); // throw 336;
	}
	if (maxDrawBuf < PG_NB_ATTACHMENTS) {
		sprintf(ErrorStr, "Error: Maximal draw buffers (%d) -> %d required!", maxbuffers, PG_NB_ATTACHMENTS); ReportError(ErrorStr); // throw 336;
	}

	/*
	// FBO: camera frame after initial processing
	glGenFramebuffers(1, &FBO_CameraFrame);  // drawing memory on odd and even frames for echo 
	if (!FBO_CameraFrame_texID) {
		glGenTextures(1, &FBO_CameraFrame_texID);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, FBO_CameraFrame);
	pg_initFBOTextures(&FBO_CameraFrame_texID, 1);
	glDrawBuffers(1, drawBuffers);
	printOglError(343);
	*/

	// FBO: multi-attachment update (CA & pixels, particles, tracks) 
	glGenFramebuffers(2, pg_FBO_Update);
	// initializations to NULL
	for (int indAttachedFB = 0; indAttachedFB < 2 * PG_NB_ATTACHMENTS; indAttachedFB++) {
		pg_FBO_Update_texID[indAttachedFB] = 0;
	}
	glGenTextures(2 * PG_NB_ATTACHMENTS, pg_FBO_Update_texID);
	for (int indFB = 0; indFB < 2; indFB++) {
		glBindFramebuffer(GL_FRAMEBUFFER, pg_FBO_Update[indFB]);
		pg_initFBOTextures(pg_FBO_Update_texID + indFB * PG_NB_ATTACHMENTS, PG_NB_ATTACHMENTS);
		glDrawBuffers(PG_NB_ATTACHMENTS, drawBuffers);
	}
	printOglError(341);

	// FBO: particle drawing output 
	glGenFramebuffers(1, &pg_FBO_Particle);  // drawing memory on odd and even frames for echo 
	if (!pg_FBO_Particle_texID) {
		glGenTextures(1, &pg_FBO_Particle_texID);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, pg_FBO_Particle);
	// pg_initFBOTextures(&pg_FBO_Particle_texID, 1);
	pg_initFBOTextures(&pg_FBO_Particle_texID, 1);
	glDrawBuffers(1, drawBuffers);
	printOglError(344);

	// FBO: composition output for echo
	glGenFramebuffers(2, pg_FBO_Mixing_capturedFB_prec);  // drawing memory on odd and even frames for echo 
															// initializations to NULL
	for (int indFB = 0; indFB < 2; indFB++) {
		pg_FBO_Mixing_capturedFB_prec_texID[indFB] = 0;
	}
	glGenTextures(2, pg_FBO_Mixing_capturedFB_prec_texID);
	for (int indFB = 0; indFB < 2; indFB++) {
		glBindFramebuffer(GL_FRAMEBUFFER, pg_FBO_Mixing_capturedFB_prec[indFB]);
		pg_initFBOTextures(pg_FBO_Mixing_capturedFB_prec_texID + indFB, 1);
		glDrawBuffers(1, drawBuffers);
	}
	printOglError(342);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return true;
}

/////////////////////////////////////////////////////////////////
// MATRIX INITIALIZATION
/////////////////////////////////////////////////////////////////
void pg_initRenderingMatrices(void) {
	memset((char *)viewMatrix, 0, 16 * sizeof(float));
	memset((char *)modelMatrix, 0, 16 * sizeof(float));

	viewMatrix[0] = 1.0f;
	viewMatrix[5] = 1.0f;
	viewMatrix[10] = 1.0f;
	viewMatrix[15] = 1.0f;
	modelMatrix[0] = 1.0f;
	modelMatrix[5] = 1.0f;
	modelMatrix[10] = 1.0f;
	modelMatrix[15] = 1.0f;

	// ORTHO
	float l = 0.0f;
	float r = (float)leftWindowWidth;
	float b = 0.0f;
	float t = (float)window_height;
	float n = -1.0f;
	float f = 1.0f;
	GLfloat mat[] = {
		(GLfloat)(2.0 / (r - l)), 0.0, 0.0, 0.0,
		0.0, (GLfloat)(2.0 / (t - b)), 0.0, 0.0,
		0.0, 0.0, (GLfloat)(2.0 / (f - n)), 0.0,
		(GLfloat)(-(r + l) / (r - l)), (GLfloat)(-(t + b) / (t - b)), (GLfloat)(-(f + n) / (f - n)), 1.0 };
	memcpy((char *)projMatrix, mat, 16 * sizeof(float));
	// printf("Orthographic projection l %.2f r %.2f b %.2f t %.2f n %.2f f %.2f\n" , l,r,b,t,n,f);

	r = (float)doubleWindowWidth;
	GLfloat mat2[] = {
		(GLfloat)(2.0 / (r - l)), 0.0, 0.0, 0.0,
		0.0, (GLfloat)(2.0 / (t - b)), 0.0, 0.0,
		0.0, 0.0, (GLfloat)(2.0 / (f - n)), 0.0,
		(GLfloat)(-(r + l) / (r - l)), (GLfloat)(-(t + b) / (t - b)), (GLfloat)(-(f + n) / (f - n)), 1.0 };
	memcpy((char *)doubleProjMatrix, mat2, 16 * sizeof(float));
	// printf("Double width orthographic projection l %.2f r %.2f b %.2f t %.2f n %.2f f %.2f\n" , l,r,b,t,n,f);
}

///////////////////////////////////////////////////////
// On-Screen Message Display
///////////////////////////////////////////////////////

void pg_screenMessage_update(void) {
	// GLbyte *xfont = NULL;
	GLubyte *yfont = NULL;
	GLubyte *wfont = NULL;
	GLubyte *hfont = NULL;
	GLubyte *sfont = NULL;
	GLubyte *tfont = NULL;
	// GLubyte *afont = NULL;

	if (NewScreenMessage) {
		switch (font_size) {
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
		int lengthMax = message_pixel_length;
		memset(pg_screenMessageBitmap, (GLubyte)0, lengthMax * 4);
		// strcpy( ScreenMessage , "abcdefghijklmnopqrstuvwxyabcdefghijklmnopqrstuvwxyabcdefghijklmnopqrstuvwxyabcdefghijklmnopqrstuvwxyabcdefghijklmnopqrstuvwxyabcdefghijklmnopqrstuv");

		// strcpy( ScreenMessage , "abcdefghijkl");
		// lengthMax = 30;

		for (char *c = ScreenMessage; *c != '\0' && pixelRank < lengthMax; c++)
		{
			char cur_car = *c;
			// if( cur_car == 'é' || 
			// 	  cur_car == 'è' || 
			// 	  cur_car == 'ê' || 
			// 	  cur_car == 'ë' ) {
			// 	cur_car = 'e';
			// }
			// if( cur_car == 'à' || 
			// 	  cur_car == 'â' ) {
			// 	cur_car = 'a';
			// }
			// if( cur_car == 'î' || 
			// 	  cur_car == 'ï' ) {
			// 	cur_car = 'i';
			// }
			// if( cur_car == 'É' || 
			// 	  cur_car == 'È' || 
			// 	  cur_car == 'Ê' || 
			// 	  cur_car == 'Ë' ) {
			// 	cur_car = 'E';
			// }
			// if( cur_car == 'À' || 
			// 	  cur_car == 'Â' ) {
			// 	cur_car = 'A';
			// }
			// if( cur_car == 'Î' || 
			// 	  cur_car == 'Ï' ) {
			// 	cur_car = 'I';
			// }

			// usable ascii starts at blank space
			int cur_car_rank = (int)cur_car - 32;
			cur_car_rank = (cur_car_rank < 0 ? 0 : cur_car_rank);
			cur_car_rank = (cur_car_rank > 94 ? 94 : cur_car_rank);

			// defines offset according to table
			for (int indPixel = 0;
				indPixel < wfont[cur_car_rank] && pixelRank + indPixel < lengthMax; indPixel++) {
				int indPixelColor = (pixelRank + indPixel) * 4;
				pg_screenMessageBitmap[indPixelColor] = sfont[cur_car_rank] + indPixel;
				pg_screenMessageBitmap[indPixelColor + 1] = tfont[cur_car_rank];
				pg_screenMessageBitmap[indPixelColor + 2] = hfont[cur_car_rank];
				pg_screenMessageBitmap[indPixelColor + 3] = yfont[cur_car_rank];
				// printf( "%d %d - " ,  sfont[ cur_car_rank ] ,  tfont[ cur_car_rank ] );
			}
			pixelRank += wfont[cur_car_rank] + 1;
		}

		glBindTexture(GL_TEXTURE_RECTANGLE, pg_screenMessageBitmap_texID);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER,
			GL_NEAREST);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER,
			GL_NEAREST);
		glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP);
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
		printOglError(6);
	}
}

/////////////////////////////////////////////////
// PARTICLES INITIALIZATION

void pg_initParticlePosition_Texture( void ) {
#ifdef CURVE_PARTICLES
	// the control point position contain column and row of the control point coordinates
	// inside the texture of initial positions so that the coordinates contained in this
	// texture can be retrieved in the vertex shader
	pg_Particle_control_points = new float[nb_particles * (PG_PARTICLE_CURVE_DEGREE + 1) * 3];
	pg_Particle_indices = new unsigned int[nb_particles * (PG_PARTICLE_CURVE_DEGREE + 1)];

	// the used width is a multiple of (PG_PARTICLE_CURVE_DEGREE + 1) so that each set of control
	// point coordinates is on a single line
	int width_used = leftWindowWidth - leftWindowWidth % (PG_PARTICLE_CURVE_DEGREE + 1);

	for (int indParticle = 0; indParticle < nb_particles; indParticle++) {
		int ind_index = indParticle * (PG_PARTICLE_CURVE_DEGREE + 1);
		int ind_point = ind_index * 3;

		// col 
		pg_Particle_control_points[ind_point + 0] = float((indParticle * (PG_PARTICLE_CURVE_DEGREE + 1) + 0) % width_used);
		// row
		pg_Particle_control_points[ind_point + 1] = float((indParticle * (PG_PARTICLE_CURVE_DEGREE + 1) + 0) / width_used);
		pg_Particle_control_points[ind_point + 2] = 0.f;

		//// u 
		//pg_ClipArt_texCoords[ind_point + 0] = 0.f;
		//// v
		//pg_ClipArt_texCoords[ind_point + 1] = 0.f;

		pg_Particle_indices[ind_index] = ind_index;

		ind_point += 3;
		ind_index++;
		for (int indControlPoint = 0; indControlPoint < PG_PARTICLE_CURVE_DEGREE; indControlPoint++) {
			// col 
			pg_Particle_control_points[ind_point + 0] = float((indParticle * (PG_PARTICLE_CURVE_DEGREE + 1) + indControlPoint + 1) % width_used);
			// row
			pg_Particle_control_points[ind_point + 1] = float((indParticle * (PG_PARTICLE_CURVE_DEGREE + 1) + indControlPoint + 1) / width_used);
			pg_Particle_control_points[ind_point + 2] = 0.f;

			//// u 
			//if (indControlPoint == 0) {
			//	pg_ClipArt_texCoords[ind_point + 0] = 0.f;
			//}
			//else {
			//	pg_ClipArt_texCoords[ind_point + 0] = 1.f;
			//}
			//// v
			//pg_ClipArt_texCoords[ind_point + 1] = 0.f;

			pg_Particle_indices[ind_index] = ind_index;

			ind_point += 3;
			ind_index++;
		}
	}
#endif
#if defined BLURRED_SPLAT_PARTICLES || defined LINE_SPLAT_PARTICLES
	// the vertices position contain column and row of the vertices coordinates
	// inside the texture of initial positions so that the coordinates contained in this
	// texture can be retrieved in the vertex shader
	pg_Particle_vertices = new float[nb_particles * 3];
	pg_Particle_radius = new float[nb_particles * 1];
	pg_Particle_colors = new float[nb_particles * 3];
	pg_Particle_indices = new unsigned int[nb_particles];

	for (int indParticle = 0; indParticle < nb_particles; indParticle++) {
		int ind_point = indParticle * 3;
		int ind_radius = indParticle;
		int ind_color = indParticle * 3;
		int ind_index = indParticle;

		// col 
		pg_Particle_vertices[ind_point + 0] = float(indParticle % leftWindowWidth);
		// row
		pg_Particle_vertices[ind_point + 1] = float(indParticle / leftWindowWidth);
		pg_Particle_vertices[ind_point + 2] = 0.f;

		// radius
		pg_Particle_radius[ind_radius + 0] = 1.f;

		// color r g b a
		pg_Particle_colors[ind_color + 0] = 1.f;
		pg_Particle_colors[ind_color + 1] = 1.f;
		pg_Particle_colors[ind_color + 2] = 1.f;

		// indices
		pg_Particle_indices[ind_index] = ind_index;
	}
#endif
}
