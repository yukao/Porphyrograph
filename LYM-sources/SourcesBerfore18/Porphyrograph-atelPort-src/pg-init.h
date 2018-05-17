/*! \file pg-init.h
*
*
*     File pg-init.h
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
/*
based on Francesco Caruso GLSLParametricCurves
https://github.com/fcaruso/GLSLParametricCurve
*/

#ifndef PG_INIT_H
#define PG_INIT_H

#define randomValue ((float)rand()/(float)RAND_MAX)

#define PG_SIZE_QUAD_ARRAY 4  // quads made of a 4-point triangle strip

#if PG_NB_TRACKS == 2
enum pg_FBO_Update_attachments
{
	pg_CA_FBO_attcht = 0,
	pg_Pixel_FBO_attcht,
	pg_ParticlePosSpeed_FBO_attcht,
	pg_ParticleColRadius_FBO_attcht,
	pg_ParticleTargetPosColRadius_FBO_attcht,
	pg_Track0_FBO_attcht,
	pg_Track1_FBO_attcht,
	PG_NB_ATTACHMENTS
};
#endif

#if PG_NB_TRACKS == 3
enum pg_FBO_Update_attachments
{
	pg_CA_FBO_attcht = 0,
	pg_Pixel_FBO_attcht,
	pg_ParticlePosSpeed_FBO_attcht,
	pg_ParticleColRadius_FBO_attcht,
	pg_ParticleTargetPosColRadius_FBO_attcht,
	pg_Track0_FBO_attcht,
	pg_Track1_FBO_attcht,
	pg_Track2_FBO_attcht,
	PG_NB_ATTACHMENTS
};
#endif

#if PG_NB_TRACKS == 4
enum pg_FBO_Update_attachments
{
	pg_CA_FBO_attcht = 0,
	pg_Pixel_FBO_attcht,
	pg_ParticlePosSpeed_FBO_attcht,
	pg_ParticleColRadius_FBO_attcht,
	pg_ParticleTargetPosColRadius_FBO_attcht,
	pg_Track0_FBO_attcht,
	pg_Track1_FBO_attcht,
	pg_Track2_FBO_attcht,
	pg_Track3_FBO_attcht,
	PG_NB_ATTACHMENTS
};
#endif

// #define PG_NB_ATTACHMENTS (3 + PG_NB_TRACKS)

enum eVertexArrayObject
{
	pg_VAOParticle,
	pg_VAOQuad,
	pg_VAOQuadMaster,
	pg_VAOCount
};

enum eVertexBufferObject
{
	pg_VBOParticle,
#if defined BLURRED_SPLAT_PARTICLES || defined LINE_SPLAT_PARTICLES
	pg_VBOParticleTexCoords,
	pg_VBOpartRadius,
	pg_VBOParticleColors,
#endif
	pg_EAOParticle,
	pg_EABQuad,
	pg_EABQuadMaster,
	pg_VBOCount
};



/////////////////////////////////////////////////////////////////
// Projection and view matrices for the shaders
//////////////////////////////////////////////////////////////////////
extern GLfloat projMatrix[16];
extern GLfloat doubleProjMatrix[16];
extern GLfloat viewMatrix[16];
extern GLfloat modelMatrix[16];

/////////////////////////////////////////////////////////////////
// GEOMETRY
//////////////////////////////////////////////////////////////////////
extern GLuint pg_vaoID[pg_VAOCount];
extern GLuint pg_vboID[pg_VBOCount];

// geometrical data
extern float quadDraw_points[];
extern float quadDraw_texCoords[];
extern unsigned int quadDraw_indices[PG_SIZE_QUAD_ARRAY];

// geometrical data
extern float quadMaster_points[];
extern float quadMaster_texCoords[];
extern unsigned int quadMaster_indices[PG_SIZE_QUAD_ARRAY];

// particle curves
#ifdef CURVE_PARTICLES
extern GLfloat *pg_Particle_control_points;
#endif
#if defined BLURRED_SPLAT_PARTICLES || defined LINE_SPLAT_PARTICLES
extern GLfloat *pg_Particle_vertices;
extern GLfloat *pg_Particle_radius;
extern GLfloat *pg_Particle_colors;
#endif
extern unsigned int *pg_Particle_indices;

/////////////////////////////////////////////////////////////////
// FBO INITIALIZATION
/////////////////////////////////////////////////////////////////
extern GLuint drawBuffers[16];

/////////////////////////////////////////////
// FBO 
extern GLuint pg_FBO_Update[2]; // nb_attachments=5
extern GLuint pg_FBO_Mixing_capturedFB_prec[2]; //  drawing memory on odd and even frames for echo
// extern GLuint FBO_CameraFrame; //  video preprocessing outcome
extern GLuint pg_FBO_Particle; // particle rendering

// FBO texture
extern GLuint pg_FBO_Update_texID[2*PG_NB_ATTACHMENTS]; // nb_attachments=5
extern GLuint pg_FBO_Mixing_capturedFB_prec_texID[2]; // drawing memory on odd and even frames for echo 
// extern GLuint FBO_CameraFrame_texID; // video preprocessing outcome 
extern GLuint pg_FBO_Particle_texID; // particle rendering

/////////////////////////////////////////////////////////////////
// TEXTURES
/////////////////////////////////////////////////////////////////
//extern GLuint pg_Particle_Pos_Texture_texID;
//extern GLfloat *pg_Particle_Pos_Texture;

#ifdef CURVE_PARTICLES
// comet texture
extern cv::Mat comet_image;
extern GLuint comet_texture_2D_texID;
#endif
#ifdef BLURRED_SPLAT_PARTICLES
// blurred disk texture
extern cv::Mat blurredDisk_image;
extern GLuint blurredDisk_texture_2D_texID;
#endif


//////////////////
// FUNCTIONS
// opengl error
int printOglError(int no);
// error report
void ReportError(char *errorString);

// time management
double RealTime(void);
#ifdef _WIN32
int gettimeofday(struct timeval* tp, void* tzp);
#endif

void MemoryUsage(void);

void InterfaceInitializations(void);

void pg_initGeometry_quads(void);
bool pg_initFBOTextures(GLuint *textureID, int nb_attachments);
bool pg_initFBO(void);
void pg_initRenderingMatrices(void);
void pg_screenMessage_update(void);
void pg_initParticlePosition_Texture(void);

#endif
