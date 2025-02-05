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

enum pg_FBO_Update_samplers
{
	pg_CA_FBO_Update_sampler = 0,
	pg_PreviousCA_FBO_Update_sampler,
	pg_Pixels_FBO_Update_sampler,
	pg_Brushes_Update_sampler,
	pg_Camera_frame_Update_sampler,
	pg_Camera_BG_Update_sampler,
	pg_Movie_frame_Update_sampler,
	pg_Noise_Update_sampler,
	pg_RepopDensity_Update_sampler,
	pg_Photo0_Update_sampler,
	pg_Photo1_Update_sampler,
	pg_SecondClipLeft_Update_sampler,
	pg_SecondClipRight_Update_sampler,
	pg_Particle_render_FBO_Update_sampler,
	pg_Trk0_FBO_Update_sampler,
	pg_Trk1_FBO_Update_sampler,
	pg_Trk2_FBO_Update_sampler,
	pg_Trk3_FBO_Update_sampler,
	pg_Camera_BGIni_FBO_Update_sampler,
	pg_pixel_image_acc_Update_sampler,
	pg_CATable_Update_sampler,
};
enum pg_FBO_Update_attachments
{
	pg_CA_FBO_Update_attcht = 0,
	pg_Pixels_FBO_Update_attcht,
	pg_Trk0_FBO_Update_attcht,
	pg_Trk1_FBO_Update_attcht,
	pg_Trk2_FBO_Update_attcht,
	pg_Trk3_FBO_Update_attcht,
	PG_FBO_UPDATE_NBATTACHTS
};
enum pg_FBO_Mixing_samplers
{
	pg_CA_FBO_Mixing_sampler = 0,
	pg_ClipArt_render_FBO_Mixing_sampler,
	pg_Particle_render_FBO_Mixing_sampler,
	pg_Render_prec_FBO_Mixing_sampler,
	pg_Screen_Font_FBO_Mixing_sampler,
	pg_Screen_Message_FBO_Mixing_sampler,
	pg_Trk0_FBO_Mixing_sampler,
	pg_Trk1_FBO_Mixing_sampler,
	pg_Trk2_FBO_Mixing_sampler,
	pg_Trk3_FBO_Mixing_sampler,
};
enum pg_FBO_Master_samplers
{
	pg_Render_curr_FBO_Master_sampler = 0,
	pg_CA_FBO_Master_sampler,
	pg_ClipArt_render_FBO_Master_sampler,
	pg_Particle_render_FBO_Master_sampler,
	pg_Trk0_FBO_Master_sampler,
	pg_Trk1_FBO_Master_sampler,
	pg_Trk2_FBO_Master_sampler,
	pg_Trk3_FBO_Master_sampler,
	pg_Mask_FBO_Master_sampler,
};

enum pg_FBO_ParticleAnimation_samplers
{
	pg_Part_init_pos_speed_ParticleAnimation_sampler = 0,
	pg_Part_init_col_rad_ParticleAnimation_sampler,
	pg_Part_image_acc_ParticleAnimation_sampler,
	pg_CA_FBO_ParticleAnimation_sampler,
	pg_Part_pos_speed_FBO_ParticleAnimation_sampler,
	pg_Part_col_rad_FBO_ParticleAnimation_sampler,
	pg_Part_Target_pos_col_rad_FBO_ParticleAnimation_sampler,
	pg_Noise_ParticleAnimation_sampler,
	pg_RepopDensity_ParticleAnimation_sampler,
	pg_Camera_frame_ParticleAnimation_sampler,
	pg_Movie_frame_ParticleAnimation_sampler,
	pg_Trk0_FBO_ParticleAnimation_sampler,
	pg_Trk1_FBO_ParticleAnimation_sampler,
	pg_Trk2_FBO_ParticleAnimation_sampler,
	pg_Trk3_FBO_ParticleAnimation_sampler,
};


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
	pg_VAOParticle,
	pg_VAOQuad,
	pg_VAOQuadMaster,
	pg_VAOCount
};

enum eVertexBufferObject
{
	pg_VBOParticle,
	pg_VBOParticleTexCoords,
	pg_VBOpartRadius,
	pg_VBOParticleColors,
	pg_EAOParticle,
	pg_EABQuad,
	pg_EABQuadMaster,
	pg_VBOCount
};

/////////////////////////////////////////////////////////////////
// Projection and view matrices for the shaders
//////////////////////////////////////////////////////////////////////
extern GLfloat pg_orthoWindowProjMatrix[16];
extern GLfloat doubleProjMatrix[16];
extern GLfloat pg_identityViewMatrix[16];
extern GLfloat pg_identityModelMatrix[16];
extern GLfloat pg_homographyForTexture[9];
extern GLfloat modelMatrixSensor[16];

/////////////////////////////////////////////////////////////////
// GEOMETRY
//////////////////////////////////////////////////////////////////////
extern GLuint pg_vaoID[pg_VAOCount];
extern GLuint pg_vboID[pg_VBOCount];
extern unsigned int quadSensor_vao;

// geometrical data
extern float quadDraw_points[];
extern float quadDraw_texCoords[];
extern unsigned int quadDraw_indices[PG_SIZE_QUAD_ARRAY];

// geometrical data
extern float quadMaster_points[];
extern float quadMaster_texCoords[];
extern unsigned int quadMaster_indices[PG_SIZE_QUAD_ARRAY];

// particle curves
extern GLfloat *pg_Particle_control_points;
extern GLfloat *pg_Particle_radius;
extern GLfloat *pg_Particle_colors;

// particle splat or geometry
extern GLfloat *pg_Particle_vertices;
extern GLfloat *pg_Particle_radius;
extern GLfloat *pg_Particle_colors;
extern unsigned int *pg_Particle_indices;

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
extern double sensor_last_activation_time;
// all sensor activation patterns
extern bool sensorActivations[PG_NB_SENSORS * PG_NB_MAX_SENSOR_ACTIVATIONS];

// sample choice
// current sample choice
extern int sample_choice[PG_NB_SENSORS];
// all possible sensor layouts
extern int sensor_sample_setUps[PG_NB_MAX_SAMPLE_SETUPS][PG_NB_SENSORS];
// sample start playing time for muting after 1 cycle
extern double sample_play_start[PG_NB_MAX_SAMPLE_SETUPS * PG_NB_SENSORS];

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
extern GLuint FBO_ParticleRendering_depthAndStencilBuffer;
extern GLuint pg_FBO_ClipArtRendering; // ClipArt GPU rendering
extern GLuint pg_FBO_ClipArt_render_texID; // ClipArt GPU rendering
extern GLuint FBO_ClipArt_depthAndStencilBuffer;
extern GLuint pg_FBO_ParticleAnimation; // particle animation
extern GLuint pg_FBO_ParticleAnimation_texID[2 * PG_FBO_PARTICLEANIMATION_NBATTACHTS]; // particle animation
extern GLuint pg_FBO_Mixing_capturedFB_prec; //  drawing memory on odd and even frames for echo
// Augmented Reality: FBO capture of Master to be displayed on a mesh
extern GLuint pg_FBO_Master_capturedFB_prec; // master output memory for mapping on mesh or bypassing mesh rendering

// FBO texture
extern GLuint pg_FBO_Update_texID[2 * PG_FBO_UPDATE_NBATTACHTS]; // update 
extern GLuint pg_FBO_Mixing_capturedFB_prec_texID[2]; // drawing memory on odd and even frames for echo 
// Augmented Reality: FBO capture of Master to be displayed on a mesh
extern GLuint pg_FBO_Master_capturedFB_prec_texID; // master output memory for mapping on mesh or bypassing mesh rendering


/////////////////////////////////////////////////////////////////
// TEXTURES
/////////////////////////////////////////////////////////////////
//extern GLuint pg_Particle_Pos_Texture_texID;
//extern GLfloat *pg_Particle_Pos_Texture;

// CURVE PARTICLES TEXTURE
extern GLuint curve_particle_2D_texID[PG_MAX_CONFIGURATIONS];
// blurred disk texture
extern std::vector<GLuint> blurredDisk_texture_2D_texID[PG_MAX_CONFIGURATIONS];

//////////////////
// FUNCTIONS
// Perlin Noise
double PerlinNoise(double x, double y, double z);

// opengl error
int printOglError(int no);
// error report
void ReportError(char *errorString);

// current directory
std::string GetCurrentWorkingDir(void);

// time management
double RealTime(void);
#if defined(_WIN32)
int gettimeofday(struct timeval* tp, void* pt);
#endif

void MemoryUsage(void);

void InterfaceInitializations(void);

void pg_initGeometry_quads(void);
bool pg_initFBOTextureImagesAndRendering(void);
bool pg_bindFBOTextures(GLuint currentFBO, GLuint* currentTexutreID, int nb_attachments, bool with_stencil_andOr_depth, GLuint currentStencilOrDepthBuffer);
bool pg_initFBOTextures(GLuint* textureID, int nb_attachments, bool with_stencil_andOr_depth, GLuint* depthAndStencilBuffer);
void pg_initRenderingMatrices(void);


void pg_initParticlePosition_Texture(void);

void SensorInitialization(void);
void assignSensorPositions(void);
void assignSensorActivations(void);
void sensor_sample_setUp_interpolation(void);

void MetawearSensorInitialization();

void MeshInitialization(void);