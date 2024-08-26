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


#define PG_SIZE_QUAD_ARRAY 4  // quads made of a 4-point triangle strip

enum pg_FBO_Update_samplers {
	pg_enum_CA_FBO_Update_sampler = 0,
	pg_enum_PreviousCA_FBO_Update_sampler,
	pg_enum_Pixels_FBO_Update_sampler,
	pg_enum_Brushes_Update_sampler,
	pg_enum_Camera_frame_Update_sampler,
	pg_enum_Camera_BG_Update_sampler,
	pg_enum_Movie_frame_Update_sampler,
	pg_enum_Noise_Update_sampler,
	pg_enum_RepopDensity_Update_sampler,
	pg_enum_Photo0_Update_sampler,
	pg_enum_Photo1_Update_sampler,
	pg_enum_SecondClipLeft_Update_sampler,
	pg_enum_SecondClipRight_Update_sampler,
	pg_enum_Particle_render_FBO_Update_sampler,
	pg_enum_Trk0_FBO_Update_sampler,
	pg_enum_Trk1_FBO_Update_sampler,
	pg_enum_Trk2_FBO_Update_sampler,
	pg_enum_Trk3_FBO_Update_sampler,
	pg_enum_Camera_BGIni_FBO_Update_sampler,
	pg_enum_pixel_image_acc_Update_sampler,
	pg_enum_CATable_Update_sampler,
};
enum pg_FBO_Update_attachments {
	pg_enum_CA_FBO_Update_attacht = 0,
	pg_enum_Pixels_FBO_Update_attacht,
	pg_enum_Trk0_FBO_Update_attacht,
	pg_enum_Trk1_FBO_Update_attacht,
	pg_enum_Trk2_FBO_Update_attacht,
	pg_enum_Trk3_FBO_Update_attacht,
	pg_enum_FBO_Update_nbAttachts
};
enum pg_FBO_Mixing_samplers {
	pg_enum_CA_FBO_Mixing_sampler = 0,
	pg_enum_ClipArt_render_FBO_Mixing_sampler,
	pg_enum_Particle_render_FBO_Mixing_sampler,
	pg_enum_Render_prec_FBO_Mixing_sampler,
	pg_enum_Screen_Font_FBO_Mixing_sampler,
	pg_enum_Screen_Message_FBO_Mixing_sampler,
	pg_enum_Trk0_FBO_Mixing_sampler,
	pg_enum_Trk1_FBO_Mixing_sampler,
	pg_enum_Trk2_FBO_Mixing_sampler,
	pg_enum_Trk3_FBO_Mixing_sampler,
	pg_enum_Mask_FBO_Mixing_sampler,
};
enum pg_FBO_Master_samplers {
	pg_enum_Render_curr_FBO_Master_sampler = 0,
	pg_enum_CA_FBO_Master_sampler,
	pg_enum_ClipArt_render_FBO_Master_sampler,
	pg_enum_Particle_render_FBO_Master_sampler,
	pg_enum_Trk0_FBO_Master_sampler,
	pg_enum_Trk1_FBO_Master_sampler,
	pg_enum_Trk2_FBO_Master_sampler,
	pg_enum_Trk3_FBO_Master_sampler,
	pg_enum_Mask_FBO_Master_sampler,
};

enum pg_FBO_ParticleAnimation_samplers {
	pg_enum_Part_init_pos_speed_ParticleAnimation_sampler = 0,
	pg_enum_Part_init_col_rad_ParticleAnimation_sampler,
	pg_enum_Part_image_acc_ParticleAnimation_sampler,
	pg_enum_CA_FBO_ParticleAnimation_sampler,
	pg_enum_Part_pos_speed_FBO_ParticleAnimation_sampler,
	pg_enum_Part_col_rad_FBO_ParticleAnimation_sampler,
	pg_enum_Part_Target_pos_col_rad_FBO_ParticleAnimation_sampler,
	pg_enum_Noise_ParticleAnimation_sampler,
	pg_enum_RepopDensity_ParticleAnimation_sampler,
	pg_enum_Camera_frame_ParticleAnimation_sampler,
	pg_enum_Movie_frame_ParticleAnimation_sampler,
	pg_enum_Trk0_FBO_ParticleAnimation_sampler,
	pg_enum_Trk1_FBO_ParticleAnimation_sampler,
	pg_enum_Trk2_FBO_ParticleAnimation_sampler,
	pg_enum_Trk3_FBO_ParticleAnimation_sampler,
};


enum pg_FBO_ParticleAnimation_attachments {
	pg_enum_Part_pos_speed_FBO_ParticleAnimation_attacht = 0,
	pg_enum_Part_col_rad_FBO_ParticleAnimation_attacht,
	pg_enum_Part_Target_pos_col_rad_FBO_ParticleAnimation_attacht,
	pg_enum_FBO_ParticleAnimation_nbAttachts
};
#endif

enum eVertexArrayObject {
	pg_enum_VAOParticle,
	pg_enum_VAOQuad,
	pg_enum_VAOQuadMaster,
	pg_enum_VAOCount
};

enum eVertexBufferObject {
	pg_enum_VBOParticle,
	pg_enum_VBOParticleTexCoords,
	pg_enum_VBOpartRadius,
	pg_enum_VBOParticleColors,
	pg_enum_EAOParticle,
	pg_enum_EABQuad,
	pg_enum_EABQuadMaster,
	pg_enum_VBOCount
};

/////////////////////////////////////////////////////////////////
// Projection and view matrices for the shaders
//////////////////////////////////////////////////////////////////////
extern GLfloat pg_orthoWindowProjMatrix[16];
extern GLfloat pg_doubleProjMatrix[16];
extern GLfloat pg_identityViewMatrix[16];
extern GLfloat pg_identityModelMatrix[16];
extern GLfloat pg_homographyForTexture[9];

// DRAW BUFFER COLOR ATTACHMENTS
extern GLuint enumDrawBuffersEntries[16];

/////////////////////////////////////////////////////////////////
// GEOMETRY
//////////////////////////////////////////////////////////////////////
extern GLuint pg_vaoID[pg_enum_VAOCount];
extern GLuint pg_vboID[pg_enum_VBOCount];
extern unsigned int pg_quadSensor_vao;

// geometrical data
extern float pg_quadDraw_points[];
extern float pg_quadDraw_texCoords[];
extern unsigned int pg_quadDraw_indices[PG_SIZE_QUAD_ARRAY];

// geometrical data
extern float pg_quadMaster_points[];
extern float pg_quadMaster_texCoords[];
extern unsigned int pg_quadMaster_indices[PG_SIZE_QUAD_ARRAY];

// particle curves
extern GLfloat* pg_Particle_control_points;
extern GLfloat* pg_Particle_radius;
extern GLfloat* pg_Particle_colors;

// particle splat or geometry
extern GLfloat* pg_Particle_vertices;
extern GLfloat* pg_Particle_radius;
extern GLfloat* pg_Particle_colors;
extern unsigned int* pg_Particle_indices;

//////////////////////////////////////////////////////////////////////
// SENSORS
//////////////////////////////////////////////////////////////////////

// sensor translations
// current sensor layout
extern float pg_sensorPositions[3 * PG_NB_SENSORS];
// all possible sensor layouts
extern float pg_sensorLayouts[3 * PG_NB_SENSORS * PG_NB_MAX_SENSOR_LAYOUTS];

// sensor on off
// current sensor activation pattern
extern bool pg_sensor_onOff[PG_NB_SENSORS];
extern double pg_sensor_last_activation_time;
// all sensor activation patterns
extern bool pg_sensorActivations[PG_NB_SENSORS * PG_NB_MAX_SENSOR_ACTIVATIONS];

// sample choice
// current sample choice
extern int pg_sample_choice[PG_NB_SENSORS];
// all possible sensor layouts
extern int pg_sensor_sample_setUps[PG_NB_MAX_SENSOR_SAMPLE_SETUPS][PG_NB_SENSORS];
// sample start playing time for muting after 1 cycle
extern double pg_sample_play_start[PG_NB_MAX_SENSOR_SAMPLE_SETUPS * PG_NB_SENSORS];

// +++++++++++++++++++++++ Metawear sensors ++++++++++++++++++++
#define PG_MW_NB_MAX_SENSORS 2
struct metawear_sensor_data {
	float mw_mss_pos[3];		// x y z
	bool  mw_mss_pos_update;
};
extern struct metawear_sensor_data pg_mw_sensors[PG_MW_NB_MAX_SENSORS];

/////////////////////////////////////////////////////////////////
// FBO INITIALIZATION
/////////////////////////////////////////////////////////////////
extern GLuint enumDrawBuffersEntries[16];

/////////////////////////////////////////////
// FBO 
extern GLuint pg_FBO_Update; // update
extern GLuint pg_FBO_ParticleRendering; // particle rendering
extern GLuint pg_FBO_Particle_render_texID; // particle rendering
extern GLuint pg_FBO_ParticleRendering_depthAndStencilBuffer;
extern GLuint pg_FBO_ClipArtRendering; // ClipArt GPU rendering
extern GLuint pg_FBO_ClipArt_render_texID; // ClipArt GPU rendering
extern GLuint FBO_ClipArt_depthAndStencilBuffer;
extern GLuint pg_FBO_ParticleAnimation; // particle animation
extern GLuint pg_FBO_ParticleAnimation_texID[2 * pg_enum_FBO_ParticleAnimation_nbAttachts]; // particle animation
extern GLuint pg_FBO_Mixing_capturedFB_prec; //  drawing memory on odd and even frames for echo
// Augmented Reality: FBO capture of Master to be displayed on a mesh
extern GLuint pg_FBO_Master_capturedFB_prec; // master output memory for mapping on mesh or bypassing mesh rendering

// FBO texture
extern GLuint pg_FBO_Update_texID[2 * pg_enum_FBO_Update_nbAttachts]; // update 
extern GLuint pg_FBO_Mixing_capturedFB_prec_texID[2]; // drawing memory on odd and even frames for echo 
// Augmented Reality: FBO capture of Master to be displayed on a mesh
extern GLuint pg_FBO_Master_capturedFB_prec_texID; // master output memory for mapping on mesh or bypassing mesh rendering


/////////////////////////////////////////////////////////////////
// TEXTURES
/////////////////////////////////////////////////////////////////
//extern GLuint pg_Particle_Pos_Texture_texID;
//extern GLfloat *pg_Particle_Pos_Texture;

// CURVE PARTICLES TEXTURE
extern GLuint pg_curve_particle_2D_texID[PG_MAX_SCENARIOS];
// blurred disk texture
extern std::vector<GLuint> pg_blurredDisk_texture_2D_texID[PG_MAX_SCENARIOS];

//////////////////
// FUNCTIONS
// Perlin Noise
double pg_PerlinNoise(double x, double y, double z);

// opengl error
int pg_printOglError(int no);
// error report
void pg_ReportError(char* errorString);

// current directory
std::string pg_GetCurrentWorkingDir(void);

// time management
double pg_RealTime(void);
#if defined(_WIN32)
int gettimeofday(struct timeval* tp);
#endif

void pg_MemoryUsage(void);

void pg_InterfaceInitializations(void);

