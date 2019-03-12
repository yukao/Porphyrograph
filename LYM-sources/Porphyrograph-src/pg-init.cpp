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
#ifdef PG_SENSORS
GLfloat modelMatrixSensor[16];
#endif

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

#ifdef PG_SENSORS
unsigned int quadSensor_vao = 0;
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
#endif

// particle curves
#ifdef CURVE_PARTICLES
GLfloat *pg_Particle_control_points;
GLfloat *pg_Particle_radius;
GLfloat *pg_Particle_colors;
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
GLuint pg_FBO_Update[2]; // PG_FBO_UPDATE_NBATTACHTS
#if defined BLURRED_SPLAT_PARTICLES || defined LINE_SPLAT_PARTICLES || defined CURVE_PARTICLES
GLuint pg_FBO_ParticleAnimation[2]; // PG_FBO_PARTICLEANIMATION_NBATTACHTS
GLuint pg_FBO_ParticleRendering = 0; // particle rendering
GLuint pg_FBO_ParticleAnimation_texID[2 * PG_FBO_PARTICLEANIMATION_NBATTACHTS]; // particle animation
GLuint pg_FBO_ParticleRendering_texID = 0; // particle rendering + stencil
#endif
GLuint pg_FBO_Mixing_capturedFB_prec[2] = { 0,0 }; //  drawing memory on odd and even frames for echo
// GLuint FBO_CameraFrame = 0; //  video preprocessing outcome

						  // FBO texture
GLuint pg_FBO_Update_texID[2 * PG_FBO_UPDATE_NBATTACHTS]; // Update
GLuint pg_FBO_Mixing_capturedFB_prec_texID[2] = { 0,0 }; // drawing memory on odd and even frames for echo 
// GLuint FBO_CameraFrame_texID = 0; // video preprocessing outcome 

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

GLbyte stb__arial_25_usascii_x[95] = { 0,1,1,0,0,1,0,0,1,1,0,1,1,0,
2,0,0,2,0,0,0,0,0,1,0,0,2,1,1,1,1,0,1,-1,1,1,1,1,1,1,1,2,0,1,
1,1,1,1,1,0,1,1,0,1,0,0,0,0,0,1,0,0,0,-1,0,0,1,0,0,0,0,0,1,1,
-2,1,1,1,1,0,1,0,1,0,0,1,0,0,0,0,0,0,2,0,0, };
GLbyte stb__arial_25_usascii_y[95] = { 20,3,3,3,2,3,3,3,3,3,3,6,17,13,
17,3,3,3,3,3,3,4,3,4,3,3,8,8,6,8,6,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,23,3,8,3,8,3,8,3,8,3,3,
3,3,3,8,8,8,8,8,8,8,4,8,8,8,8,8,8,3,3,3,10, };
// modified from original data by assigning a width of 8 to space char
// first value in this array
GLubyte stb__arial_25_usascii_w[95] = { 8,4,6,13,12,18,15,4,6,6,8,11,4,7,
3,7,12,7,12,12,12,12,12,11,12,12,3,4,11,11,11,12,21,16,13,15,14,13,12,16,14,3,10,14,
11,16,14,16,13,17,15,13,14,14,15,21,15,15,14,5,7,5,10,14,6,12,11,11,11,12,7,11,10,3,
6,11,3,17,10,12,11,11,7,11,7,10,11,16,12,11,11,7,2,7,13, };
GLubyte stb__arial_25_usascii_h[95] = { 0,17,7,18,21,18,18,7,22,22,8,12,7,3,
3,18,18,17,17,18,17,17,18,16,18,18,12,16,12,8,12,17,22,17,17,18,17,17,17,18,17,17,18,17,
17,17,17,18,17,19,17,18,17,18,17,17,17,17,17,22,18,22,10,2,4,13,18,13,18,13,17,17,17,17,
22,17,17,12,12,13,17,17,12,13,17,13,12,12,12,17,12,22,22,22,4, };
GLubyte stb__arial_25_usascii_s[95] = { 255,58,157,123,75,162,181,152,27,1,143,
169,251,185,251,197,55,134,155,137,210,1,205,34,231,218,205,29,193,131,181,
142,47,237,223,80,195,181,168,23,14,252,244,107,95,78,63,106,44,88,28,
1,13,40,232,210,194,178,163,69,15,41,120,193,164,82,150,108,68,69,96,
116,136,147,34,1,248,209,150,95,104,151,161,57,128,46,239,120,137,122,227,
19,16,8,171, };
GLubyte stb__arial_25_usascii_t[95] = { 1,43,74,1,1,1,1,74,1,1,74,
61,61,74,69,1,24,43,43,1,43,61,1,61,1,1,61,61,61,74,61,
43,1,43,43,24,43,43,43,24,61,24,1,43,43,43,43,1,43,1,43,
24,43,24,24,24,24,24,24,1,24,1,74,74,74,61,1,61,24,61,24,
24,24,24,1,43,24,61,61,61,24,24,61,61,24,61,61,61,61,43,61,
1,1,1,74, };
int stb__arial_25_usascii_a[95] = { 99,99,127,199,199,318,239,68,
119,119,139,209,99,119,99,99,199,199,199,199,199,199,199,199,
199,199,99,99,209,209,209,199,363,239,239,259,259,239,219,278,
259,99,179,239,199,298,259,278,239,278,259,239,219,259,239,338,
239,239,219,99,99,99,168,199,119,199,199,179,199,199,99,199,
199,80,80,179,80,298,199,199,199,199,119,179,99,199,179,259,
179,179,179,120,93,120,209, };

GLbyte stb__arial_50_usascii_x[95] = { 0,3,2,0,1,2,1,1,2,2,1,2,3,1,
4,0,1,4,1,1,0,1,1,2,1,1,4,3,2,2,2,1,2,-1,3,2,3,3,3,2,3,4,1,3,
3,3,3,2,3,1,3,2,1,3,0,0,0,0,0,3,0,0,1,-1,1,1,2,1,1,1,0,1,2,2,
-3,2,2,2,2,1,2,1,2,1,0,2,0,0,0,0,0,1,4,1,1, };
GLbyte stb__arial_50_usascii_y[95] = { 40,7,7,7,5,7,7,7,7,7,7,13,35,26,
35,7,7,7,7,7,7,8,7,8,7,7,16,16,13,17,13,7,7,7,7,7,7,7,7,7,7,7,7,7,
7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,46,7,16,7,16,7,16,7,16,7,7,
7,7,7,16,16,16,16,16,16,16,8,16,16,16,16,16,16,7,7,7,20, };
// modified from original data by assigning a width of 16 to space char
// first value in this array
GLubyte stb__arial_50_usascii_w[95] = { 16,6,12,25,22,36,28,6,12,12,15,22,6,13,
5,13,22,13,22,22,23,23,22,21,22,22,5,6,22,22,22,22,42,31,25,29,27,25,23,31,26,5,18,27,
21,31,26,31,25,33,29,26,26,26,30,42,30,30,27,9,13,10,19,27,10,22,22,21,21,23,14,21,20,5,
10,21,5,33,20,23,22,21,14,20,13,20,22,32,23,22,22,13,4,13,24, };
GLubyte stb__arial_50_usascii_h[95] = { 0,33,13,34,40,35,34,13,43,43,15,22,12,5,
5,34,34,33,33,34,33,33,34,32,34,34,24,31,23,14,23,33,43,33,33,34,33,33,33,34,33,33,34,33,
33,33,33,34,33,36,33,34,33,34,33,33,33,33,33,42,34,42,18,3,7,25,34,25,34,25,33,34,33,33,
43,33,33,24,24,25,33,33,24,25,33,25,24,24,24,34,24,43,43,43,8, };
GLubyte stb__arial_50_usascii_s[95] = { 252,57,169,56,135,192,82,162,1,101,123,
80,182,229,246,111,178,225,1,69,183,110,114,207,125,201,246,239,34,139,57,
160,58,78,134,148,50,24,173,37,228,197,164,145,123,91,64,1,31,158,1,
224,201,137,148,105,74,43,15,125,23,114,103,158,214,1,33,24,92,46,210,
1,183,204,47,179,248,115,172,70,225,203,240,229,1,94,149,1,216,229,193,
33,28,14,189, };
GLubyte stb__arial_50_usascii_t[95] = { 1,149,243,45,1,1,45,243,1,1,243,
243,243,209,174,45,45,149,183,80,183,183,80,183,45,45,149,149,243,243,243,
183,1,183,183,45,183,183,149,80,115,149,80,149,149,149,149,45,149,1,149,
45,115,80,115,115,115,115,115,1,80,1,243,38,243,217,45,217,80,217,80,
80,80,80,1,115,80,217,217,217,80,149,217,183,115,217,217,243,217,1,217,
1,1,1,243, };
int stb__arial_50_usascii_a[95] = { 199,199,254,398,398,637,478,137,
238,238,279,418,199,238,199,199,398,398,398,398,398,398,398,398,
398,398,199,199,418,418,418,398,727,478,478,517,517,478,437,557,
517,199,358,478,398,597,517,557,478,557,517,478,437,517,478,676,
478,478,437,199,199,199,336,398,238,398,398,358,398,398,199,398,
398,159,159,358,159,597,398,398,398,398,238,358,199,398,358,517,
358,358,358,239,186,239,418, };


//////////////////////////////////////////////////////////////////////////////////////////////
// INTERFACE INITIALIZATION
//////////////////////////////////////////////////////////////////////////////////////////////

void InterfaceInitializations(void) {
	// PEN COLOR PRESET INTERFACE VARIABLE INITIALIZATION
	for (int ind = 0; ind < nb_pen_colorPresets; ind++) {
		sprintf(AuxString, "/pen_colorPreset_name%d %s",
			ind, pen_colorPresets_names[ind].c_str()); pg_send_message_udp((char *)"s", (char *)AuxString, (char *)"udp_TouchOSC_send");
	}
	// SVG GPU INTERFACE VARIABLE INITIALIZATION
	for (int indImage = 0; indImage < pg_nb_ClipArt; indImage++) {
		sprintf(AuxString, "/ClipArt_%d_onOff %d", indImage, (activeClipArts & (1 << (indImage - 1)))); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_TouchOSC_send");
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
		printf("*** glError at %d -> %d: %s\n", no,
			err, gluErrorString(err));
		retCode = 1;
	}
	return retCode;
}

void ReportError(char *errorString) {
	//if (fileLog) {
	//	fprintf(fileLog, "%s\n", errorString);
	//}

	fprintf(stderr, "%s\n", errorString);
}

//////////////////////////////////////////////////////////////////////////////////////
// FUNCTION FOR TESTING THE CURRENT WORKING DIRECTORY
std::string GetCurrentWorkingDir(void) {
	char buff[FILENAME_MAX];
	GetCurrentDir(buff, FILENAME_MAX);
	std::string current_working_dir(buff);
	std::replace(current_working_dir.begin(), current_working_dir.end(), '\\', '/'); // replace all 'x' to 'y'
	return current_working_dir;
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

double pg_TimeAtApplicationLaunch = -1.;
float RealTime(void) {
	struct timeval time;
	gettimeofday(&time, 0);
	double realtime = (double)time.tv_sec + ((double)time.tv_usec / 1000000.);
	if (pg_TimeAtApplicationLaunch < 0) {
		pg_TimeAtApplicationLaunch = realtime;
	}
	return (float(realtime - pg_TimeAtApplicationLaunch) - InitialRealTime) * time_scale;
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

#ifdef PG_SENSORS
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
	glGenBuffers(1, &quadSensor_points_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, quadSensor_points_vbo);
	glBufferData(GL_ARRAY_BUFFER,
		3 * 3 * 2 * sizeof(float),
		quadSensor_points,
		GL_STATIC_DRAW);
	unsigned int quadSensor_texCoord_vbo = 0;
	glGenBuffers(1, &quadSensor_texCoord_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, quadSensor_texCoord_vbo);
	glBufferData(GL_ARRAY_BUFFER,
		2 * 3 * 2 * sizeof(float),
		quadSensor_texCoords,
		GL_STATIC_DRAW);

	quadSensor_vao = 0;
	glGenVertexArrays(1, &quadSensor_vao);
	glBindVertexArray(quadSensor_vao);
	// vertex positions are location 0
	glBindBuffer(GL_ARRAY_BUFFER, quadSensor_points_vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
	glEnableVertexAttribArray(0);

	// texture coordinates are location 1
	glBindBuffer(GL_ARRAY_BUFFER, quadSensor_texCoord_vbo);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
	glEnableVertexAttribArray(1); // don't forget this!

								  // printf("Sensor size  %d\n" , PG_SENSOR_GEOMETRY_WIDTH );

	printOglError(23);

	///////////////////////////////////////////////////////////////////////////////////////
	// sensor layouts
	///////////////////////////////////////////////////////////////////////////////////////
	int indLayout;
	
#ifndef MALAUSSENA
	int indSens;

	// square grid
	indLayout = 0;
	for (indSens = 0; indSens < PG_NB_SENSORS; indSens++) {
		int heightInt = indSens / 4;
		int widthInt = indSens % 4;
		int heightStep = window_height / (4 + 1);
		int widthStep = leftWindowWidth / (4 + 1);
		sensorLayouts[indLayout * 3 * PG_NB_SENSORS + 3 * indSens] = (float)widthStep + widthInt * (float)widthStep;
		sensorLayouts[indLayout * 3 * PG_NB_SENSORS + 3 * indSens + 1] = (float)heightStep + heightInt * (float)heightStep;
		sensorLayouts[indLayout * 3 * PG_NB_SENSORS + 3 * indSens + 2] = 0.1f;
	}

	// circular grid
	indLayout = 1;
	// central sensor
	indSens = 0;
	sensorLayouts[indLayout * 3 * PG_NB_SENSORS + 3 * indSens] = leftWindowWidth / 2.0f;
	sensorLayouts[indLayout * 3 * PG_NB_SENSORS + 3 * indSens + 1] = window_height / 2.0f;
	sensorLayouts[indLayout * 3 * PG_NB_SENSORS + 3 * indSens + 2] = 0.1f;
	for (indSens = 1; indSens < 5; indSens++) {
		float radius = window_height / 5.0f;
		sensorLayouts[indLayout * 3 * PG_NB_SENSORS + 3 * indSens] = leftWindowWidth / 2.0f + radius * (float)cos(indSens * 2.0 * M_PI / 4.0f);
		sensorLayouts[indLayout * 3 * PG_NB_SENSORS + 3 * indSens + 1] = window_height / 2.0f + radius * (float)sin(indSens * 2.0 * M_PI / 4.0f);
		sensorLayouts[indLayout * 3 * PG_NB_SENSORS + 3 * indSens + 2] = 0.1f;
	}
	for (indSens = 5; indSens < PG_NB_SENSORS; indSens++) {
		float radius = window_height / 3.0f;
		sensorLayouts[indLayout * 3 * PG_NB_SENSORS + 3 * indSens] = leftWindowWidth / 2.0f + radius * (float)cos(indSens * 2.0 * M_PI / 11.0f);
		sensorLayouts[indLayout * 3 * PG_NB_SENSORS + 3 * indSens + 1] = window_height / 2.0f + radius * (float)sin(indSens * 2.0 * M_PI / 11.0f);
		sensorLayouts[indLayout * 3 * PG_NB_SENSORS + 3 * indSens + 2] = 0.1f;
	}

	// radial grid
	indLayout = 2;
	// central sensor
	indSens = 0;
	sensorLayouts[indLayout * 3 * PG_NB_SENSORS + 3 * indSens] = leftWindowWidth / 2.0f;
	sensorLayouts[indLayout * 3 * PG_NB_SENSORS + 3 * indSens + 1] = window_height / 2.0f;
	sensorLayouts[indLayout * 3 * PG_NB_SENSORS + 3 * indSens + 2] = 0.1f;
	for (int indRay = 0; indRay < 5; indRay++) {
		float angle = indRay * 2.0f * (float)M_PI / 5.0f;
		for (indSens = 1 + indRay * 3; indSens < 1 + indRay * 3 + 3; indSens++) {
			float radius = ((indSens - 1) % 3 + 1) * window_height / 10.0f;
			sensorLayouts[indLayout * 3 * PG_NB_SENSORS + 3 * indSens] = leftWindowWidth / 2.0f + radius * (float)cos(angle);
			sensorLayouts[indLayout * 3 * PG_NB_SENSORS + 3 * indSens + 1] = window_height / 2.0f + radius * (float)sin(angle);
			sensorLayouts[indLayout * 3 * PG_NB_SENSORS + 3 * indSens + 2] = 0.1f;
		}
	}

	// conflated in the center
	// will be transformed into a random layout each time it is invoked
	indLayout = 3;
	for (indSens = 0; indSens < PG_NB_SENSORS; indSens++) {
		sensorLayouts[indLayout * 3 * PG_NB_SENSORS + 3 * indSens] = leftWindowWidth / 2.0f;
		sensorLayouts[indLayout * 3 * PG_NB_SENSORS + 3 * indSens + 1] = window_height / 2.0f;
		sensorLayouts[indLayout * 3 * PG_NB_SENSORS + 3 * indSens + 2] = 0.1f;
	}
#else
	// right
	indLayout = 0;
	sensorLayouts[indLayout * 3 * PG_NB_SENSORS] = (float)(7 * leftWindowWidth / 8);
	sensorLayouts[indLayout * 3 * PG_NB_SENSORS + 1] = (float)(window_height / 2);
	sensorLayouts[indLayout * 3 * PG_NB_SENSORS + 2] = 0.1f;

	// center
	indLayout = 1;
	sensorLayouts[indLayout * 3 * PG_NB_SENSORS] = (float)(leftWindowWidth / 2);
	sensorLayouts[indLayout * 3 * PG_NB_SENSORS + 1] = (float)(window_height / 2);
	sensorLayouts[indLayout * 3 * PG_NB_SENSORS + 2] = 0.1f;

	// top
	indLayout = 2;
	sensorLayouts[indLayout * 3 * PG_NB_SENSORS] = (float)(leftWindowWidth / 2);
	sensorLayouts[indLayout * 3 * PG_NB_SENSORS + 1] = (float)(7 * window_height / 8);
	sensorLayouts[indLayout * 3 * PG_NB_SENSORS + 2] = 0.1f;

	// corner
	indLayout = 3;
	sensorLayouts[indLayout * 3 * PG_NB_SENSORS] = (float)(7 * leftWindowWidth / 8);
	sensorLayouts[indLayout * 3 * PG_NB_SENSORS + 1] = (float)(7 * window_height / 8);
	sensorLayouts[indLayout * 3 * PG_NB_SENSORS + 2] = 0.1f;

#endif


	///////////////////////////////////////////////////////////////////////////////////////
	// sensor activations
	///////////////////////////////////////////////////////////////////////////////////////
	int indActivation;

#ifndef MALAUSSENA
	// no activation
	indActivation = 0;
	for (int indSensor = 0; indSensor < PG_NB_SENSORS; indSensor++) {
		sensorActivations[indActivation * PG_NB_SENSORS + indSensor] = false;
	}

	// corners activation
	indActivation = 1;
	for (int indSensor = 0; indSensor < PG_NB_SENSORS; indSensor++) {
		sensorActivations[indActivation * PG_NB_SENSORS + indSensor] = false;
	}
	sensorActivations[indActivation * PG_NB_SENSORS + 0] = true;
	sensorActivations[indActivation * PG_NB_SENSORS + 3] = true;
	sensorActivations[indActivation * PG_NB_SENSORS + 12] = true;
	sensorActivations[indActivation * PG_NB_SENSORS + 15] = true;

	// central square activation
	indActivation = 2;
	for (int indSensor = 0; indSensor < PG_NB_SENSORS; indSensor++) {
		sensorActivations[indActivation * PG_NB_SENSORS + indSensor] = false;
	}
	sensorActivations[indActivation * PG_NB_SENSORS + 5] = true;
	sensorActivations[indActivation * PG_NB_SENSORS + 6] = true;
	sensorActivations[indActivation * PG_NB_SENSORS + 9] = true;
	sensorActivations[indActivation * PG_NB_SENSORS + 10] = true;

	// every second activation
	indActivation = 3;
	for (int indSensor = 0; indSensor < PG_NB_SENSORS; indSensor++) {
		if (indSensor % 2 == 0) {
			sensorActivations[indActivation * PG_NB_SENSORS + indSensor] = true;
		}
		else {
			sensorActivations[indActivation * PG_NB_SENSORS + indSensor] = false;
		}
	}

	// full activation
	indActivation = 4;
	for (int indSensor = 0; indSensor < PG_NB_SENSORS; indSensor++) {
		sensorActivations[indActivation * PG_NB_SENSORS + indSensor] = true;
	}

	// no activation and activation of an additional sensor each 10 seconds
	indActivation = 5;
	for (int indSensor = 0; indSensor < PG_NB_SENSORS; indSensor++) {
		sensorActivations[indActivation * PG_NB_SENSORS + indSensor] = false;
	}
#else
	// full activation
	indActivation = 0;
	for (int indSensor = 0; indSensor < PG_NB_SENSORS; indSensor++) {
		sensorActivations[indActivation * PG_NB_SENSORS + indSensor] = true;
	}

	// no activation 
	indActivation = 1;
	for (int indSensor = 0; indSensor < PG_NB_SENSORS; indSensor++) {
		sensorActivations[indActivation * PG_NB_SENSORS + indSensor] = false;
	}
#endif

	///////////////////////////////////////////////////////////////////////////////////////
	// sample setup
	///////////////////////////////////////////////////////////////////////////////////////
	// sample choice
	sensor_sample_setUp_interpolation();

	// float sample_choice[ PG_NB_SENSORS];
	// // all possible sensor layouts
	//int sensor_sample_setUps[ PG_NB_MAX_SAMPLE_SETUPS][ PG_NB_SENSORS ] =
	//  {{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15},
	//   {16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31},
	//   {32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47}};
#endif


#if defined (BLURRED_SPLAT_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 
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
	// radius
	glBindBuffer(GL_ARRAY_BUFFER, pg_vboID[pg_VBOpartRadius]);
	glBufferData(GL_ARRAY_BUFFER, nb_particles * 1 * sizeof(float), pg_Particle_radius, GL_STATIC_DRAW);
	// color
	glBindBuffer(GL_ARRAY_BUFFER, pg_vboID[pg_VBOParticleColors]);
	glBufferData(GL_ARRAY_BUFFER, nb_particles * 3 * sizeof(float), pg_Particle_colors, GL_STATIC_DRAW);
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
#if defined CURVE_PARTICLES
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
#endif
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	printOglError(32);

	printf("Geometry initialized %dx%d & %dx%d\n", leftWindowWidth, window_height,
		doubleWindowWidth, window_height);
}

/////////////////////////////////////////////////////////////////
// SENSOR INITIALIZATION
/////////////////////////////////////////////////////////////////

void SensorInitialization(void) {
#ifdef PG_SENSORS
#ifdef PG_RENOISE
	sprintf(AuxString, "/renoise/transport/start"); pg_send_message_udp((char *)"", AuxString, (char *)"udp_RN_send");
#endif
#ifdef PG_PORPHYROGRAPH_SOUND
	sprintf(AuxString, "/loop_level 0.0"); pg_send_message_udp((char *)"f", AuxString, (char *)"udp_PGsound_send");
	sprintf(AuxString, "/input_level 0.0"); pg_send_message_udp((char *)"f", AuxString, (char *)"udp_PGsound_send");
#endif
	for (int indSample = 0; indSample < PG_NB_MAX_SAMPLE_SETUPS * PG_NB_SENSORS; indSample++) {
#ifdef PG_RENOISE
		std::string message = "/renoise/song/track/";
		std::string int_string = std::to_string(indSample + 1);
		message += int_string + "/mute";

		// message format
		std::string format = "";

		// message posting
		pg_send_message_udp((char *)format.c_str(), (char *)message.c_str(), (char *)"udp_RN_send");
#endif
#ifdef PG_PORPHYROGRAPH_SOUND
		std::string message = "/track_";
		std::string int_string = std::to_string(indSample + 1);
		message += int_string + "_level 0";

		// message format
		std::string format = "f";

		// message posting
		pg_send_message_udp((char *)format.c_str(), (char *)message.c_str(), (char *)"udp_PGsound_send");
#endif

		// resets the clock for replaying the sample if sensor triggered again
		sample_play_start[indSample] = -1.0f;
	}
#endif
}

#ifdef PG_SENSORS
void assignSensorPositions(void) {
	int indLayout = max(0, min(sensor_layout, PG_NB_MAX_SENSOR_LAYOUTS - 1));
	// non random layout: copy
	if (indLayout < PG_NB_MAX_SENSOR_LAYOUTS - 1) {
		for (int indSensor = 0; indSensor < PG_NB_SENSORS; indSensor++) {
			sensorPositions[3 * indSensor] = sensorLayouts[indLayout * 3 * PG_NB_SENSORS + 3 * indSensor];
			sensorPositions[3 * indSensor + 1] = sensorLayouts[indLayout * 3 * PG_NB_SENSORS + 3 * indSensor + 1];
			sensorPositions[3 * indSensor + 2] = 0.1f;
		}
	}
	// random layout: regenerate
	else {
		for (int indSensor = 0; indSensor < PG_NB_SENSORS; indSensor++) {
			sensorPositions[3 * indSensor] = leftWindowWidth / 2.0f + leftWindowWidth / 2.0f * (2.0f * ((float)rand() / (float)RAND_MAX) - 1.0f);
			sensorPositions[3 * indSensor + 1] = window_height / 2.0f + window_height / 2.0f * (2.0f * ((float)rand() / (float)RAND_MAX) - 1.0f);
			sensorPositions[3 * indSensor + 2] = 0.1f;
		}
	}
}

void assignSensorActivations(void) {
	// copies the single central activation
	int indActivation = std::max(0, std::min(sensor_activation, PG_NB_MAX_SENSOR_ACTIVATIONS - 1));
	for (int indSensor = 0; indSensor < PG_NB_SENSORS; indSensor++) {
		sensor_onOff[indSensor] = sensorActivations[indActivation * PG_NB_SENSORS + indSensor];
	}
	// incremental sensor activation
	if (sensor_activation == 5) {
		sensor_last_activation_time = CurrentClockTime;
	}
}

void sensor_sample_setUp_interpolation(void) {
	float indSetUp = std::max(0.0f, std::min(sensor_sample_setUp, (float)(PG_NB_MAX_SAMPLE_SETUPS - 1)));

	float sensor_sample_setUp_integerPart = floor(indSetUp);
	float sensor_sample_setUp_floatPart = indSetUp - sensor_sample_setUp_integerPart;
	int intIndSetup = (int)(sensor_sample_setUp_integerPart);

	// copies the setup that corresponds to the integer part
	for (int ind = 0; ind < PG_NB_SENSORS; ind++) {
		sample_choice[ind] = sensor_sample_setUps[intIndSetup][ind];
	}

	// for the decimal part, copies hybrid sensors of upper level with a ratio
	// proportional to the ratio between floor and current value 
	if (sensor_sample_setUp_floatPart > 0.0f) {
		int nbhybridSensors = (int)round(sensor_sample_setUp_floatPart * PG_NB_SENSORS);

		// book keeping of hybridized sensors
		bool hybridized[PG_NB_SENSORS];
		for (int ind = 0; ind < PG_NB_SENSORS; ind++) {
			hybridized[ind] = false;
		}

		for (int indSensor = 0; indSensor < min(PG_NB_SENSORS, nbhybridSensors); indSensor++) {
			int count = (int)round(((float)rand() / (float)RAND_MAX) * PG_NB_SENSORS);
			for (int ind = 0; ind < PG_NB_SENSORS; ind++) {
				int translatedIndex = (count + PG_NB_SENSORS) % PG_NB_SENSORS;
				if (!hybridized[translatedIndex]) {
					sample_choice[translatedIndex] = sensor_sample_setUps[intIndSetup + 1][translatedIndex];
					hybridized[translatedIndex] = true;
				}
			}
		}
	}

#ifdef PG_SUPERCOLLIDER
	std::string message = "/sensor_sample_setUp";
	std::string format = "";
	for (int indSensor = 0; indSensor < PG_NB_SENSORS - 1; indSensor++) {
		format += "i ";
	}
	format += "i";

	// sensor readback
	for (int indSensor = 0; indSensor < PG_NB_SENSORS; indSensor++) {
		std::string float_str = std::to_string((long long)sample_choice[indSensor]);
		// float_str.resize(4);
		message += " " + float_str;
	}

	pg_send_message_udp((char *)format.c_str(), (char *)message.c_str(), (char *)"udp_SC_send");
#endif

	// std::cout << "format: " << format << "\n";
	// std::cout << "msg: " << message << "\n";
}
#endif


/////////////////////////////////////////////////////////////////
// FBO TEXTURES INITIALIZATION
/////////////////////////////////////////////////////////////////

bool pg_initFBOTextures(GLuint *textureID, int nb_attachments, bool with_stencil_andOr_depth) {
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
	if (with_stencil_andOr_depth) {
		GLuint depthAndStencilBuffer;
		glGenRenderbuffers(1, &depthAndStencilBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, depthAndStencilBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, leftWindowWidth, window_height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthAndStencilBuffer);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthAndStencilBuffer);
		/*
		glBindTexture(GL_TEXTURE_2D, textureID[nb_attachments]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_UNSIGNED_INT_24_8,
			leftWindowWidth, window_height);
		// printf("FBO size %d %d \n" , leftWindowWidth , window_height );
		glFramebufferTexture2D(GL_FRAMEBUFFER,
			GL_DEPTH24_STENCIL8,
			GL_TEXTURE_2D,
			textureID[nb_attachments],
			0);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER)
			== GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT) {
			sprintf(ErrorStr, "Error: Binding RECT FBO texture No %d ID %d (error %d)!", 1, textureID[1], glCheckFramebufferStatus(GL_FRAMEBUFFER)); ReportError(ErrorStr); throw 336;
		}
		*/
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

	if (maxbuffers < PG_FBO_UPDATE_NBATTACHTS) {
		sprintf(ErrorStr, "Error: Maximal attachment (%d) -> %d required!", maxbuffers, PG_FBO_UPDATE_NBATTACHTS); ReportError(ErrorStr); // throw 336;
	}
	if (maxDrawBuf < PG_FBO_UPDATE_NBATTACHTS) {
		sprintf(ErrorStr, "Error: Maximal draw buffers (%d) -> %d required!", maxbuffers, PG_FBO_UPDATE_NBATTACHTS); ReportError(ErrorStr); // throw 336;
	}

	/*
	// FBO: camera frame after initial processing
	glGenFramebuffers(1, &FBO_CameraFrame);  // drawing memory on odd and even frames for echo 
	if (!FBO_CameraFrame_texID) {
		glGenTextures(1, &FBO_CameraFrame_texID);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, FBO_CameraFrame);
	pg_initFBOTextures(&FBO_CameraFrame_texID, 1, false);
	glDrawBuffers(1, drawBuffers);
	printOglError(343);
	*/

	// FBO: multi-attachment for update 
	glGenFramebuffers(2, pg_FBO_Update);
	// initializations to NULL
	for (int indAttachedFB = 0; indAttachedFB < 2 * PG_FBO_UPDATE_NBATTACHTS; indAttachedFB++) {
		pg_FBO_Update_texID[indAttachedFB] = 0;
	}
	glGenTextures(2 * PG_FBO_UPDATE_NBATTACHTS, pg_FBO_Update_texID);
	for (int indFB = 0; indFB < 2; indFB++) {
		glBindFramebuffer(GL_FRAMEBUFFER, pg_FBO_Update[indFB]);
		pg_initFBOTextures(pg_FBO_Update_texID + indFB * PG_FBO_UPDATE_NBATTACHTS, 
			PG_FBO_UPDATE_NBATTACHTS, false);
		glDrawBuffers(PG_FBO_UPDATE_NBATTACHTS, drawBuffers);
	}
	//printf("FBO UPDATE     %d %d    %d %d %d   %d %d %d \n", pg_FBO_Update[0], pg_FBO_Update[1], pg_FBO_Update_texID[0], pg_FBO_Update_texID[1], pg_FBO_Update_texID[2], pg_FBO_Update_texID[3], pg_FBO_Update_texID[4], pg_FBO_Update_texID[5]);
	printOglError(341);

#if defined (BLURRED_SPLAT_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 
	// FBO: multi-attachment for particle animation 
	glGenFramebuffers(2, pg_FBO_ParticleAnimation);
	// initializations to NULL
	for (int indAttachedFB = 0; indAttachedFB < 2 * PG_FBO_PARTICLEANIMATION_NBATTACHTS; indAttachedFB++) {
		pg_FBO_ParticleAnimation_texID[indAttachedFB] = 0;
	}
	glGenTextures(2 * PG_FBO_PARTICLEANIMATION_NBATTACHTS, pg_FBO_ParticleAnimation_texID);
	for (int indFB = 0; indFB < 2; indFB++) {
		glBindFramebuffer(GL_FRAMEBUFFER, pg_FBO_ParticleAnimation[indFB]);
		pg_initFBOTextures(pg_FBO_ParticleAnimation_texID + indFB * PG_FBO_PARTICLEANIMATION_NBATTACHTS, 
			PG_FBO_PARTICLEANIMATION_NBATTACHTS, false);
		glDrawBuffers(PG_FBO_PARTICLEANIMATION_NBATTACHTS, drawBuffers);
	}
	//printf("FBO ANIMATION       %d %d      %d %d %d   %d %d %d \n", pg_FBO_ParticleAnimation[0], pg_FBO_ParticleAnimation[1], pg_FBO_ParticleAnimation_texID[0], pg_FBO_ParticleAnimation_texID[1], pg_FBO_ParticleAnimation_texID[2], pg_FBO_ParticleAnimation_texID[3], pg_FBO_ParticleAnimation_texID[4], pg_FBO_ParticleAnimation_texID[5]);
	printOglError(341);

	// FBO: particle drawing output 
	glGenFramebuffers(1, &pg_FBO_ParticleRendering);  // drawing memory on odd and even frames for echo 
	glGenTextures(1, &pg_FBO_ParticleRendering_texID);
	glBindFramebuffer(GL_FRAMEBUFFER, pg_FBO_ParticleRendering);
	// pg_initFBOTextures(&pg_FBO_ParticleRendering_texID, 1, false);
	pg_initFBOTextures(&pg_FBO_ParticleRendering_texID, 1, true);
	glDrawBuffers(1, drawBuffers);
	printOglError(344);
#endif

	// FBO: composition output for echo
	glGenFramebuffers(2, pg_FBO_Mixing_capturedFB_prec);  // drawing memory on odd and even frames for echo 
															// initializations to NULL
	for (int indFB = 0; indFB < 2; indFB++) {
		pg_FBO_Mixing_capturedFB_prec_texID[indFB] = 0;
	}
	glGenTextures(2, pg_FBO_Mixing_capturedFB_prec_texID);
	for (int indFB = 0; indFB < 2; indFB++) {
		glBindFramebuffer(GL_FRAMEBUFFER, pg_FBO_Mixing_capturedFB_prec[indFB]);
		pg_initFBOTextures(pg_FBO_Mixing_capturedFB_prec_texID + indFB, 1, false);
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
#ifdef PG_SENSORS
	memset((char *)modelMatrixSensor, 0, 16 * sizeof(float));
#endif

	viewMatrix[0] = 1.0f;
	viewMatrix[5] = 1.0f;
	viewMatrix[10] = 1.0f;
	viewMatrix[15] = 1.0f;
	modelMatrix[0] = 1.0f;
	modelMatrix[5] = 1.0f;
	modelMatrix[10] = 1.0f;
	modelMatrix[15] = 1.0f;
#ifdef PG_SENSORS
	modelMatrixSensor[0] = 1.0f;
	modelMatrixSensor[5] = 1.0f;
	modelMatrixSensor[10] = 1.0f;
	modelMatrixSensor[15] = 1.0f;
#endif

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
		switch (screen_font_size) {
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
				pg_screenMessageBitmap[indPixelColor] = GLubyte(sfont[cur_car_rank] + indPixel);
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

#if defined (TVW)
///////////////////////////////////////////////////////
// Message Uploads

bool pg_ReadAllDisplayMessages(string dir, string basefilename) {
	bool valRet = true;
	DisplayTextList = new string[PG_NB_DISPLAY_MESSAGES];
	DisplayTextFirstInChapter = new int[PG_NB_DISPLAY_MESSAGES];
	for (int ind = 0; ind < PG_NB_DISPLAY_MESSAGES; ind++) {
		DisplayTextList[ind] = "";
		DisplayTextFirstInChapter[ind] = 0;
	}
	ifstream myReadFile;
	myReadFile.open(dir + basefilename);
	int indChapter = 0;
	if (myReadFile.is_open()) {
		while (!myReadFile.eof() && NbDisplayTexts < PG_NB_DISPLAY_MESSAGES) {
			std::getline(myReadFile, DisplayTextList[NbDisplayTexts]); // Saves the line in STRING.
																	   // std::cout << DisplayTextList[NbDisplayTexts]+"\n"; // Prints our STRING.
			while (!myReadFile.eof() && (DisplayTextList[NbDisplayTexts].find("request =>", 0, strlen("request =>")) == 0)) {
				DisplayTextFirstInChapter[indChapter++] = NbDisplayTexts;
				// std::cout << "Message dir #" << indChapter - 1 << " index #" << DisplayTextFirstInChapter[indChapter-1] << "\n"; // Prints our STRING.
				std::getline(myReadFile, DisplayTextList[NbDisplayTexts]);
			}
			if (!(DisplayTextList[NbDisplayTexts].empty())) {
				NbDisplayTexts++;
			}
		}
	}
	else {
		strcpy(ErrorStr, ("File " + dir + basefilename + " not opened!").c_str()); ReportError(ErrorStr); throw 152;
	}
	myReadFile.close();

	if (NbDisplayTexts >= PG_NB_DISPLAY_MESSAGES) {
		strcpy(ErrorStr, ("Only part of the messages read from " + dir + basefilename + "!").c_str()); ReportError(ErrorStr);
	}
	pg_CurrentDiaporamaDir = -1;
	DisplayTextFirstInChapter[indChapter] = NbDisplayTexts;
	NbTextChapters = indChapter;
	std::cout << "Loaded texts #" << NbDisplayTexts << " in chapters #" << NbTextChapters << "\n";

	DisplayText1Front = false;
	DisplayText1 = "";
	DisplayText2 = "";

	return valRet;
}

///////////////////////////////////////////////////////
// large display messages
int pg_displayMessage_update(int indMesg) {
	GLbyte *yfont = stb__arial_25_usascii_y;
	GLubyte *wfont = stb__arial_25_usascii_w;
	GLubyte *hfont = stb__arial_25_usascii_h;
	GLubyte *sfont = stb__arial_25_usascii_s;
	GLubyte *tfont = stb__arial_25_usascii_t;

	int pixelRank = 0;
	GLubyte *offSetText = (indMesg == 1 ? pg_displayMsg1Bitmap : pg_displayMsg2Bitmap);
	memset(offSetText, (GLubyte)0, message_pixel_length * 4);

	if (indMesg == 1) {
		if (IndDisplayText1 == -1) {
			return 0;
		}
		DisplayText1 = DisplayTextList[IndDisplayText1 % NbDisplayTexts];
		// DisplayText1 = "abcdefghijkl";
	}
	else {
		if (IndDisplayText2 == -1) {
			return 0;
		}
		DisplayText2 = DisplayTextList[IndDisplayText2 % NbDisplayTexts];
		// DisplayText1 = "abcdefghijkl";
	}

	for (const char *c = (indMesg == 1 ? DisplayText1.c_str() : DisplayText2.c_str());
		*c != '\0' && pixelRank < message_pixel_length; c++) {
		char cur_car = *c;
		if (cur_car == 'à' ||
			cur_car == 'â') {
			cur_car = 'a';
		}
		else if (cur_car == 'é' ||
			cur_car == 'è' ||
			cur_car == 'ê' ||
			cur_car == 'ë') {
			cur_car = 'e';
		}
		else if (cur_car == 'î' ||
			cur_car == 'ï') {
			cur_car = 'i';
		}
		else if (cur_car == 'ô') {
			cur_car = 'o';
		}
		else if (cur_car == 'û' ||
			cur_car == 'ù') {
			cur_car = 'u';
		}
		else if (cur_car == 'À' ||
			cur_car == 'Â') {
			cur_car = 'A';
		}
		else if (cur_car == 'É' ||
			cur_car == 'È' ||
			cur_car == 'Ê' ||
			cur_car == 'Ë') {
			cur_car = 'E';
		}
		else if (cur_car == 'Î' ||
			cur_car == 'Ï') {
			cur_car = 'I';
		}
		else if (cur_car == 'Ô') {
			cur_car = 'o';
		}
		else if (cur_car == 'Û' ||
			cur_car == 'Ù') {
			cur_car = 'U';
		}

		// usable ascii starts at blank space
		int cur_car_rank = (int)cur_car - 32;
		cur_car_rank = (cur_car_rank < 0 ? 0 : cur_car_rank);
		cur_car_rank = (cur_car_rank > 94 ? 94 : cur_car_rank);

		// defines offset according to table
		// defines the offsets for all the horizontal pixels covering 
		// the texture of the current character
		for (int indPixel = 0;
			indPixel < wfont[cur_car_rank] && (pixelRank + indPixel) < message_pixel_length;
			indPixel++) {
			// the position of the four bytes corresponding to the current horizontal pixel
			// pixelRank is the rank of the first horizontal pixel of the current character
			// indPixel is the rank of the current pixel inside the horizontal texture of the current character
			int indPixelColor = (pixelRank + indPixel) * 4;
			// X encodes the s texture coordinate of the current character (it is offseted by
			// indPixel because we are on the indPixel_th horizontal pixel
			offSetText[indPixelColor] = GLubyte(sfont[cur_car_rank] + indPixel);
			// Y encodes the t texture coordinate of the current character
			offSetText[indPixelColor + 1] = tfont[cur_car_rank];
			// Z and W encode height H and vertical offsets Y
			// the character texture coordinates are bound by Y and Y + H
			offSetText[indPixelColor + 2] = hfont[cur_car_rank];
			offSetText[indPixelColor + 3] = yfont[cur_car_rank];
			// printf( "%d %d - " ,  sfont[ cur_car_rank ] ,  tfont[ cur_car_rank ] );
		}
		// jumps to the next character over the width of the current character + 1 black vertical 1 pixel stripe
		pixelRank += wfont[cur_car_rank] + 1;
	}

	if (indMesg == 1) {
		glBindTexture(GL_TEXTURE_RECTANGLE, pg_Display_Message1_Bitmap_texID);
	}
	else {
		glBindTexture(GL_TEXTURE_RECTANGLE, pg_Display_Message2_Bitmap_texID);
	}
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER,
		GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER,
		GL_NEAREST);
	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_RECTANGLE,     // Type of texture
		0,                 // Pyramid level (for mip-mapping) - 0 is the top level
		GL_RGBA8,           // Internal colour format to convert to
		message_pixel_length,         // Image width
		1,                 // Image height
		0,                 // Border width in pixels (can either be 1 or 0)
		GL_RGBA,           // Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
		GL_UNSIGNED_BYTE,  // Image data type
		offSetText);        // The actual image data itself
	printOglError(6);
	return pixelRank + 1;
}
#endif

/////////////////////////////////////////////////
// PARTICLES INITIALIZATION

void pg_initParticlePosition_Texture( void ) {
#ifdef CURVE_PARTICLES
	// the control point position contain column and row of the control point coordinates
	// inside the texture of initial positions so that the coordinates contained in this
	// texture can be retrieved in the vertex shader
	pg_Particle_control_points = new float[nb_particles * (PG_PARTICLE_CURVE_DEGREE + 1) * 3];
	pg_Particle_indices = new unsigned int[nb_particles * (PG_PARTICLE_CURVE_DEGREE + 1)];

	pg_Particle_radius = new float[nb_particles * 1];
	pg_Particle_colors = new float[nb_particles * 3];

	// the used width is a multiple of (PG_PARTICLE_CURVE_DEGREE + 1) so that each set of control
	// point coordinates is on a single line
	int width_used = leftWindowWidth - leftWindowWidth % (PG_PARTICLE_CURVE_DEGREE + 1);

	for (int indParticle = 0; indParticle < nb_particles; indParticle++) {
		int ind_index = indParticle * (PG_PARTICLE_CURVE_DEGREE + 1);
		int ind_radius = indParticle;
		int ind_color = indParticle * 3;
		int ind_point = ind_index * 3;

		// col 
		pg_Particle_control_points[ind_point + 0] = float((indParticle * (PG_PARTICLE_CURVE_DEGREE + 1) + 0) % width_used);
		// row
		pg_Particle_control_points[ind_point + 1] = float((indParticle * (PG_PARTICLE_CURVE_DEGREE + 1) + 0) / width_used);
		pg_Particle_control_points[ind_point + 2] = 0.f;

		//// u 
		//pg_Particle_texCoords[ind_point + 0] = 0.f;
		//// v
		//pg_Particle_texCoords[ind_point + 1] = 0.f;

		// radius
		pg_Particle_radius[ind_radius + 0] = 1.f;

		// color r g b a
		pg_Particle_colors[ind_color + 0] = 1.f;
		pg_Particle_colors[ind_color + 1] = 1.f;
		pg_Particle_colors[ind_color + 2] = 1.f;

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
			//	pg_Particle_texCoords[ind_point + 0] = 0.f;
			//}
			//else {
			//	pg_Particle_texCoords[ind_point + 0] = 1.f;
			//}
			//// v
			//pg_Particle_texCoords[ind_point + 1] = 0.f;

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


