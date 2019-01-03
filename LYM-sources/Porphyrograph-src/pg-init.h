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

#if PG_NB_TRACKS == 1
enum pg_FBO_Update_samplers
{
#ifdef PG_NB_CA_TYPES
	pg_CA_FBO_Update_sampler = 0,
#ifdef PG_NB_PIXEL_MODES
	pg_Pixels_FBO_Update_sampler,
	pg_Brushes_FBO_Update_sampler,
#else
	pg_Brushes_FBO_Update_sampler = 0,
#endif
#else
#ifdef PG_NB_PIXEL_MODES
	pg_Pixels_FBO_Update_sampler = 0,
	pg_Brushes_FBO_Update_sampler,
#else
	pg_Brushes_FBO_Update_sampler = 0,
#endif
#endif
	pg_Camera_frame_FBO_Update_sampler,
	pg_Camera_BG_FBO_Update_sampler,
	pg_Movie_frame_FBO_Update_sampler,
	pg_Noise_FBO_Update_sampler,
#ifdef PG_WITH_PHOTO_DIAPORAMA
	pg_Photo0_FBO_Update_sampler,
	pg_Photo1_FBO_Update_sampler,
#endif
#if defined (BLURRED_SPLAT_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 
	pg_Part_render_FBO_Update_sampler,
#endif
#if defined (TVW)
	pg_TVWPixels0,
	pg_TVWPixels1,
	pg_TVWPixels2,
	pg_TVWPixels3,
	pg_TVWPixels4,
	pg_TVWPixels5,
	pg_TVWMask02,
	pg_TVWMask35,
	pg_TVWPixelsSwap0,
	pg_TVWPixelsSwap1,
	pg_TVWPixelsSwap2,
	pg_TVWPixelsSwap3,
	pg_TVWPixelsSwap4,
	pg_TVWPixelsSwap5,
#endif
	pg_Trk0_FBO_Update_sampler,
#if defined (GN) || defined (MALAUSSENA)
	pg_CATable_Update_sampler,
#endif
#ifdef GN
	pg_Camera_BGIni_FBO_Update_sampler,
#endif
};
enum pg_FBO_Update_attachments
{
#ifdef PG_NB_CA_TYPES
	pg_CA_FBO_Update_attcht = 0,
#ifdef PG_NB_PIXEL_MODES
	pg_Pixels_FBO_Update_attcht,
	pg_Trk0_FBO_Update_attcht,
#else
	pg_Trk0_FBO_Update_attcht = 0,
#endif
#else
#ifdef PG_NB_PIXEL_MODES
	pg_Pixels_FBO_Update_attcht = 0,
	pg_Trk0_FBO_Update_attcht,
#else
	pg_Trk0_FBO_Update_attcht = 0,
#endif
#endif
	PG_FBO_UPDATE_NBATTACHTS
};
enum pg_FBO_Mixing_samplers
{
#ifdef PG_NB_CA_TYPES
	pg_CA_FBO_Mixing_sampler = 0,
#if defined (BLURRED_SPLAT_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 
	pg_Part_render_FBO_Mixing_sampler,
#endif
	pg_Render_prec_FBO_Mixing_sampler,
#else
#if defined (BLURRED_SPLAT_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 
	pg_Part_render_FBO_Mixing_sampler = 0,
	pg_Render_prec_FBO_Mixing_sampler,
#else
	pg_Render_prec_FBO_Mixing_sampler = 0,
#endif
#endif
	pg_Screen_Font_FBO_Mixing_sampler,
	pg_Screen_Message_FBO_Mixing_sampler,
#if defined (TVW)
	pg_Display_Font_FBO_Mixing_sampler,
	pg_Display_Message1_FBO_Mixing_sampler,
	pg_Display_Message2_FBO_Mixing_sampler,
#endif
	pg_Trk0_FBO_Mixing_sampler,
};
enum pg_FBO_Master_samplers
{
	pg_Render_curr_FBO_Master_sampler = 0,
#ifdef PG_NB_CA_TYPES
	pg_CA_FBO_Master_sampler,
#endif
#if defined (BLURRED_SPLAT_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 
	pg_Part_render_FBO_Master_sampler,
#endif
	pg_Trk0_FBO_Master_sampler,
#if defined (GN) || defined (INTERFERENCE)
	pg_LYMlogo_Master_sampler,
#endif
};
#if defined BLURRED_SPLAT_PARTICLES || defined LINE_SPLAT_PARTICLES || defined CURVE_PARTICLES
enum pg_FBO_ParticleAnimation_samplers
{
	pg_Part_init_pos_speed_FBO_ParticleAnimation_sampler = 0,
	pg_Part_init_col_rad_FBO_ParticleAnimation_sampler,
	pg_CA_FBO_ParticleAnimation_sampler,
	pg_Part_pos_speed_FBO_ParticleAnimation_sampler,
	pg_Part_col_rad_FBO_ParticleAnimation_sampler,
	pg_Part_Target_pos_col_rad_FBO_ParticleAnimation_sampler,
	pg_Noise_FBO_ParticleAnimation_sampler,
	pg_Camera_frame_FBO_ParticleAnimation_sampler,
	pg_Movie_frame_FBO_ParticleAnimation_sampler,
	pg_Trk0_FBO_ParticleAnimation_sampler,
};
#endif
#endif

#if PG_NB_TRACKS == 2
enum pg_FBO_Update_samplers
{
#ifdef PG_NB_CA_TYPES
	pg_CA_FBO_Update_sampler = 0,
#ifdef PG_NB_PIXEL_MODES
	pg_Pixels_FBO_Update_sampler,
	pg_Brushes_FBO_Update_sampler,
#else
	pg_Brushes_FBO_Update_sampler = 0,
#endif
#else
#ifdef PG_NB_PIXEL_MODES
	pg_Pixels_FBO_Update_sampler = 0,
	pg_Brushes_FBO_Update_sampler,
#else
	pg_Brushes_FBO_Update_sampler = 0,
#endif
#endif
	pg_Camera_frame_FBO_Update_sampler,
	pg_Camera_BG_FBO_Update_sampler,
	pg_Movie_frame_FBO_Update_sampler,
	pg_Noise_FBO_Update_sampler,
#ifdef PG_WITH_PHOTO_DIAPORAMA
	pg_Photo0_FBO_Update_sampler,
	pg_Photo1_FBO_Update_sampler,
#endif
#if defined (BLURRED_SPLAT_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 
	pg_Part_render_FBO_Update_sampler,
#endif
#if defined (TVW)
	pg_TVWPixels0,
	pg_TVWPixels1,
	pg_TVWPixels2,
	pg_TVWPixels3,
	pg_TVWPixels4,
	pg_TVWPixels5,
	pg_TVWMask02,
	pg_TVWMask35,
	pg_TVWPixelsSwap0,
	pg_TVWPixelsSwap1,
	pg_TVWPixelsSwap2,
	pg_TVWPixelsSwap3,
	pg_TVWPixelsSwap4,
	pg_TVWPixelsSwap5,
#endif
	pg_Trk0_FBO_Update_sampler,
	pg_Trk1_FBO_Update_sampler,
};
enum pg_FBO_Update_attachments
{
#ifdef PG_NB_CA_TYPES
	pg_CA_FBO_Update_attcht = 0,
#ifdef PG_NB_PIXEL_MODES
	pg_Pixels_FBO_Update_attcht,
	pg_Trk0_FBO_Update_attcht,
#else
	pg_Trk0_FBO_Update_attcht = 0,
#endif
#else
#ifdef PG_NB_PIXEL_MODES
	pg_Pixels_FBO_Update_attcht = 0,
	pg_Trk0_FBO_Update_attcht,
#else
	pg_Trk0_FBO_Update_attcht = 0,
#endif
#endif
	pg_Trk1_FBO_Update_attcht,
	PG_FBO_UPDATE_NBATTACHTS
};
enum pg_FBO_Mixing_samplers
{
#ifdef PG_NB_CA_TYPES
	pg_CA_FBO_Mixing_sampler = 0,
#if defined (BLURRED_SPLAT_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 
	pg_Part_render_FBO_Mixing_sampler,
#endif
	pg_Render_prec_FBO_Mixing_sampler,
#else
#if defined (BLURRED_SPLAT_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 
	pg_Part_render_FBO_Mixing_sampler = 0,
	pg_Render_prec_FBO_Mixing_sampler,
#else
	pg_Render_prec_FBO_Mixing_sampler = 0,
#endif
#endif
	pg_Screen_Font_FBO_Mixing_sampler,
	pg_Screen_Message_FBO_Mixing_sampler,
#if defined (TVW)
	pg_Display_Font_FBO_Mixing_sampler,
	pg_Display_Message1_FBO_Mixing_sampler,
	pg_Display_Message2_FBO_Mixing_sampler,
#endif
	pg_Trk0_FBO_Mixing_sampler,
	pg_Trk1_FBO_Mixing_sampler,
};
enum pg_FBO_Master_samplers
{
	pg_Render_curr_FBO_Master_sampler = 0,
#ifdef PG_NB_CA_TYPES
	pg_CA_FBO_Master_sampler,
#endif
#if defined (BLURRED_SPLAT_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 
	pg_Part_render_FBO_Master_sampler,
#endif
	pg_Trk0_FBO_Master_sampler,
	pg_Trk1_FBO_Master_sampler,
#if defined (GN) || defined (INTERFERENCE)
	pg_LYMlogo_Master_sampler,
#endif
};
#if defined BLURRED_SPLAT_PARTICLES || defined LINE_SPLAT_PARTICLES || defined CURVE_PARTICLES
enum pg_FBO_ParticleAnimation_samplers
{
	pg_Part_init_pos_speed_FBO_ParticleAnimation_sampler = 0,
	pg_Part_init_col_rad_FBO_ParticleAnimation_sampler,
	pg_CA_FBO_ParticleAnimation_sampler,
	pg_Part_pos_speed_FBO_ParticleAnimation_sampler,
	pg_Part_col_rad_FBO_ParticleAnimation_sampler,
	pg_Part_Target_pos_col_rad_FBO_ParticleAnimation_sampler,
	pg_Noise_FBO_ParticleAnimation_sampler,
	pg_Camera_frame_FBO_ParticleAnimation_sampler,
	pg_Movie_frame_FBO_ParticleAnimation_sampler,
	pg_Trk0_FBO_ParticleAnimation_sampler,
	pg_Trk1_FBO_ParticleAnimation_sampler,
};
#endif
#endif

#if PG_NB_TRACKS == 3
enum pg_FBO_Update_samplers
{
#ifdef PG_NB_CA_TYPES
	pg_CA_FBO_Update_sampler = 0,
#ifdef PG_NB_PIXEL_MODES
	pg_Pixels_FBO_Update_sampler,
	pg_Brushes_FBO_Update_sampler,
#else
	pg_Brushes_FBO_Update_sampler = 0,
#endif
#else
#ifdef PG_NB_PIXEL_MODES
	pg_Pixels_FBO_Update_sampler = 0,
	pg_Brushes_FBO_Update_sampler,
#else
	pg_Brushes_FBO_Update_sampler = 0,
#endif
#endif
	pg_Camera_frame_FBO_Update_sampler,
	pg_Camera_BG_FBO_Update_sampler,
	pg_Movie_frame_FBO_Update_sampler,
	pg_Noise_FBO_Update_sampler,
#ifdef PG_WITH_PHOTO_DIAPORAMA
	pg_Photo0_FBO_Update_sampler,
	pg_Photo1_FBO_Update_sampler,
#endif
#if defined (BLURRED_SPLAT_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 
	pg_Part_render_FBO_Update_sampler,
#endif
	pg_Trk0_FBO_Update_sampler,
	pg_Trk1_FBO_Update_sampler,
	pg_Trk2_FBO_Update_sampler,
};
enum pg_FBO_Update_attachments
{
#ifdef PG_NB_CA_TYPES
	pg_CA_FBO_Update_attcht = 0,
#ifdef PG_NB_PIXEL_MODES
	pg_Pixels_FBO_Update_attcht,
	pg_Trk0_FBO_Update_attcht,
#else
	pg_Trk0_FBO_Update_attcht = 0,
#endif
#else
#ifdef PG_NB_PIXEL_MODES
	pg_Pixels_FBO_Update_attcht = 0,
	pg_Trk0_FBO_Update_attcht,
#else
	pg_Trk0_FBO_Update_attcht = 0,
#endif
#endif
	pg_Trk1_FBO_Update_attcht,
	pg_Trk2_FBO_Update_attcht,
	PG_FBO_UPDATE_NBATTACHTS
};
enum pg_FBO_Mixing_samplers
{
#ifdef PG_NB_CA_TYPES
	pg_CA_FBO_Mixing_sampler = 0,
#if defined (BLURRED_SPLAT_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 
	pg_Part_render_FBO_Mixing_sampler,
#endif
	pg_Render_prec_FBO_Mixing_sampler,
#else
#if defined (BLURRED_SPLAT_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 
	pg_Part_render_FBO_Mixing_sampler = 0,
	pg_Render_prec_FBO_Mixing_sampler,
#else
	pg_Render_prec_FBO_Mixing_sampler = 0,
#endif
#endif
	pg_Screen_Font_FBO_Mixing_sampler,
	pg_Screen_Message_FBO_Mixing_sampler,
#if defined (TVW)
	pg_Display_Font_FBO_Mixing_sampler,
	pg_Display_Message1_FBO_Mixing_sampler,
	pg_Display_Message2_FBO_Mixing_sampler,
#endif
	pg_Trk0_FBO_Mixing_sampler,
	pg_Trk1_FBO_Mixing_sampler,
	pg_Trk2_FBO_Mixing_sampler,
};
enum pg_FBO_Master_samplers
{
	pg_Render_curr_FBO_Master_sampler = 0,
#ifdef PG_NB_CA_TYPES
	pg_CA_FBO_Master_sampler,
#endif
#if defined (BLURRED_SPLAT_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 
	pg_Part_render_FBO_Master_sampler,
#endif
	pg_Trk0_FBO_Master_sampler,
	pg_Trk1_FBO_Master_sampler,
	pg_Trk2_FBO_Master_sampler,
#if defined (GN) || defined (INTERFERENCE)
	pg_LYMlogo_Master_sampler,
#endif
};
#if defined BLURRED_SPLAT_PARTICLES || defined LINE_SPLAT_PARTICLES || defined CURVE_PARTICLES
enum pg_FBO_ParticleAnimation_samplers
{
	pg_Part_init_pos_speed_FBO_ParticleAnimation_sampler = 0,
	pg_Part_init_col_rad_FBO_ParticleAnimation_sampler,
	pg_CA_FBO_ParticleAnimation_sampler,
	pg_Part_pos_speed_FBO_ParticleAnimation_sampler,
	pg_Part_col_rad_FBO_ParticleAnimation_sampler,
	pg_Part_Target_pos_col_rad_FBO_ParticleAnimation_sampler,
	pg_Noise_FBO_ParticleAnimation_sampler,
	pg_Camera_frame_FBO_ParticleAnimation_sampler,
	pg_Movie_frame_FBO_ParticleAnimation_sampler,
	pg_Trk0_FBO_ParticleAnimation_sampler,
	pg_Trk1_FBO_ParticleAnimation_sampler,
	pg_Trk2_FBO_ParticleAnimation_sampler,
};
#endif
#endif

#if PG_NB_TRACKS == 4
enum pg_FBO_Update_samplers
{
#ifdef PG_NB_CA_TYPES
	pg_CA_FBO_Update_sampler = 0,
#ifdef PG_NB_PIXEL_MODES
	pg_Pixels_FBO_Update_sampler,
	pg_Brushes_FBO_Update_sampler,
#else
	pg_Brushes_FBO_Update_sampler = 0,
#endif
#else
#ifdef PG_NB_PIXEL_MODES
	pg_Pixels_FBO_Update_sampler = 0,
	pg_Brushes_FBO_Update_sampler,
#else
	pg_Brushes_FBO_Update_sampler = 0,
#endif
#endif
	pg_Camera_frame_FBO_Update_sampler,
	pg_Camera_BG_FBO_Update_sampler,
	pg_Movie_frame_FBO_Update_sampler,
	pg_Noise_FBO_Update_sampler,
#ifdef PG_WITH_PHOTO_DIAPORAMA
	pg_Photo0_FBO_Update_sampler,
	pg_Photo1_FBO_Update_sampler,
#endif
#if defined (BLURRED_SPLAT_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 
	pg_Part_render_FBO_Update_sampler,
#endif
	pg_Trk0_FBO_Update_sampler,
	pg_Trk1_FBO_Update_sampler,
	pg_Trk2_FBO_Update_sampler,
	pg_Trk3_FBO_Update_sampler,
};
enum pg_FBO_Update_attachments
{
#ifdef PG_NB_CA_TYPES
	pg_CA_FBO_Update_attcht = 0,
#ifdef PG_NB_PIXEL_MODES
	pg_Pixels_FBO_Update_attcht,
	pg_Trk0_FBO_Update_attcht,
#else
	pg_Trk0_FBO_Update_attcht = 0,
#endif
#else
#ifdef PG_NB_PIXEL_MODES
	pg_Pixels_FBO_Update_attcht = 0,
	pg_Trk0_FBO_Update_attcht,
#else
	pg_Trk0_FBO_Update_attcht = 0,
#endif
#endif
	pg_Trk1_FBO_Update_attcht,
	pg_Trk2_FBO_Update_attcht,
	pg_Trk3_FBO_Update_attcht,
	PG_FBO_UPDATE_NBATTACHTS
};
enum pg_FBO_Mixing_samplers
{
#ifdef PG_NB_CA_TYPES
	pg_CA_FBO_Mixing_sampler = 0,
#if defined (BLURRED_SPLAT_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 
	pg_Part_render_FBO_Mixing_sampler,
#endif
	pg_Render_prec_FBO_Mixing_sampler,
#else
#if defined (BLURRED_SPLAT_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 
	pg_Part_render_FBO_Mixing_sampler = 0,
	pg_Render_prec_FBO_Mixing_sampler,
#else
	pg_Render_prec_FBO_Mixing_sampler = 0,
#endif
#endif
	pg_Screen_Font_FBO_Mixing_sampler,
	pg_Screen_Message_FBO_Mixing_sampler,
#if defined (TVW)
	pg_Display_Font_FBO_Mixing_sampler,
	pg_Display_Message1_FBO_Mixing_sampler,
	pg_Display_Message2_FBO_Mixing_sampler,
#endif
	pg_Trk0_FBO_Mixing_sampler,
	pg_Trk1_FBO_Mixing_sampler,
	pg_Trk2_FBO_Mixing_sampler,
	pg_Trk3_FBO_Mixing_sampler,
};
enum pg_FBO_Master_samplers
{
	pg_Render_curr_FBO_Master_sampler = 0,
#ifdef PG_NB_CA_TYPES
	pg_CA_FBO_Master_sampler,
#endif
#if defined (BLURRED_SPLAT_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 
	pg_Part_render_FBO_Master_sampler,
#endif
	pg_Trk0_FBO_Master_sampler,
	pg_Trk1_FBO_Master_sampler,
	pg_Trk2_FBO_Master_sampler,
	pg_Trk3_FBO_Master_sampler,
};
#if defined BLURRED_SPLAT_PARTICLES || defined LINE_SPLAT_PARTICLES || defined CURVE_PARTICLES
enum pg_FBO_ParticleAnimation_samplers
{
	pg_Part_init_pos_speed_FBO_ParticleAnimation_sampler = 0,
	pg_Part_init_col_rad_FBO_ParticleAnimation_sampler,
	pg_CA_FBO_ParticleAnimation_sampler,
	pg_Part_pos_speed_FBO_ParticleAnimation_sampler,
	pg_Part_col_rad_FBO_ParticleAnimation_sampler,
	pg_Part_Target_pos_col_rad_FBO_ParticleAnimation_sampler,
	pg_Noise_FBO_ParticleAnimation_sampler,
	pg_Camera_frame_FBO_ParticleAnimation_sampler,
	pg_Movie_frame_FBO_ParticleAnimation_sampler,
	pg_Trk0_FBO_ParticleAnimation_sampler,
	pg_Trk1_FBO_ParticleAnimation_sampler,
	pg_Trk2_FBO_ParticleAnimation_sampler,
	pg_Trk3_FBO_ParticleAnimation_sampler,
};
#endif
#endif

#if defined BLURRED_SPLAT_PARTICLES || defined LINE_SPLAT_PARTICLES || defined CURVE_PARTICLES
enum pg_FBO_ParticleAnimation_attachments
{
	pg_Part_pos_speed_FBO_ParticleAnimation_attcht = 0,
	pg_Part_col_rad_FBO_ParticleAnimation_attcht,
	pg_Part_Target_pos_col_rad_FBO_ParticleAnimation_attcht,
	PG_FBO_PARTICLEANIMATION_NBATTACHTS
};
#endif

enum eVertexArrayObject
{
#if defined BLURRED_SPLAT_PARTICLES || defined LINE_SPLAT_PARTICLES || defined CURVE_PARTICLES
	pg_VAOParticle,
#endif
	pg_VAOQuad,
	pg_VAOQuadMaster,
	pg_VAOCount
};

enum eVertexBufferObject
{
#if defined BLURRED_SPLAT_PARTICLES || defined LINE_SPLAT_PARTICLES || defined CURVE_PARTICLES
	pg_VBOParticle,
#endif
#if defined BLURRED_SPLAT_PARTICLES || defined LINE_SPLAT_PARTICLES
	pg_VBOParticleTexCoords,
	pg_VBOpartRadius,
	pg_VBOParticleColors,
#endif
#if defined CURVE_PARTICLES
	pg_VBOParticleTexCoords,
	pg_VBOpartRadius,
	pg_VBOParticleColors,
#endif
#if defined BLURRED_SPLAT_PARTICLES || defined LINE_SPLAT_PARTICLES || defined CURVE_PARTICLES
	pg_EAOParticle,
#endif
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
#ifdef PG_SENSORS
extern GLfloat modelMatrixSensor[16];
#endif

/////////////////////////////////////////////////////////////////
// GEOMETRY
//////////////////////////////////////////////////////////////////////
extern GLuint pg_vaoID[pg_VAOCount];
extern GLuint pg_vboID[pg_VBOCount];
#ifdef PG_SENSORS
extern unsigned int quadSensor_vao;
#endif

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
extern GLfloat *pg_Particle_radius;
extern GLfloat *pg_Particle_colors;
#endif
#if defined BLURRED_SPLAT_PARTICLES || defined LINE_SPLAT_PARTICLES
extern GLfloat *pg_Particle_vertices;
extern GLfloat *pg_Particle_radius;
extern GLfloat *pg_Particle_colors;
#endif
#if defined BLURRED_SPLAT_PARTICLES || defined LINE_SPLAT_PARTICLES || defined CURVE_PARTICLES
extern unsigned int *pg_Particle_indices;
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

/////////////////////////////////////////////////////////////////
// FBO INITIALIZATION
/////////////////////////////////////////////////////////////////
extern GLuint drawBuffers[16];

/////////////////////////////////////////////
// FBO 
extern GLuint pg_FBO_Update[2]; // update
#if defined BLURRED_SPLAT_PARTICLES || defined LINE_SPLAT_PARTICLES || defined CURVE_PARTICLES
extern GLuint pg_FBO_ParticleAnimation[2]; // particle animation
extern GLuint pg_FBO_ParticleRendering; // particle rendering
extern GLuint pg_FBO_ParticleAnimation_texID[2 * PG_FBO_PARTICLEANIMATION_NBATTACHTS]; // particle animation
extern GLuint pg_FBO_ParticleRendering_texID; // particle rendering
#endif
extern GLuint pg_FBO_Mixing_capturedFB_prec[2]; //  drawing memory on odd and even frames for echo
// extern GLuint FBO_CameraFrame; //  video preprocessing outcome

// FBO texture
extern GLuint pg_FBO_Update_texID[2 * PG_FBO_UPDATE_NBATTACHTS]; // update 
extern GLuint pg_FBO_Mixing_capturedFB_prec_texID[2]; // drawing memory on odd and even frames for echo 
// extern GLuint FBO_CameraFrame_texID; // video preprocessing outcome 

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

// current directory
std::string GetCurrentWorkingDir(void);

// time management
float RealTime(void);
#ifdef _WIN32
int gettimeofday(struct timeval* tp, void* tzp);
#endif

void MemoryUsage(void);

void InterfaceInitializations(void);

void pg_initGeometry_quads(void);
bool pg_initFBOTextures(GLuint *textureID, int nb_attachments, bool with_stencil_andOr_depth);
bool pg_initFBO(void);
void pg_initRenderingMatrices(void);

void pg_screenMessage_update(void);
#if defined (TVW)
bool pg_ReadAllDisplayMessages(string dir, string basefilename);
int pg_displayMessage_update(int indMesg);
#endif

void pg_initParticlePosition_Texture(void);
#ifdef PG_SENSORS
void SensorInitialization(void);
void assignSensorPositions(void);
void assignSensorActivations(void);
void sample_setUp_interpolation(void);
#endif

#endif
