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

#if defined(_CRTDBG_MAP_ALLOC)
// MEMORY LEAK CONTROL
extern _CrtMemState s1;
extern _CrtMemState s2;
extern _CrtMemState s3;
extern _CrtMemState s4;
extern _CrtMemState s5;
extern _CrtMemState s6;
extern _CrtMemState s7;
extern _CrtMemState s8;
extern _CrtMemState s9;
extern _CrtMemState s10;
extern _CrtMemState s11;
extern _CrtMemState s12;
extern _CrtMemState s13;
extern _CrtMemState sDiff;
#endif

/////////////////////////////////////////////////////////////////
// textures bitmaps and associated IDs
extern GLuint Master_Mask_texID[PG_MAX_CONFIGURATIONS];
extern GLuint Master_Multilayer_Mask_texID[PG_MAX_CONFIGURATIONS];

extern GLuint Pen_texture_3D_texID[PG_MAX_CONFIGURATIONS];

extern GLuint Noise_texture_3D[PG_MAX_CONFIGURATIONS];
extern std::vector<GLuint>  pg_RepopDensity_texture_texID[PG_MAX_CONFIGURATIONS];

extern std::vector<GLuint> pg_particle_initial_pos_speed_texID[PG_MAX_CONFIGURATIONS];
extern std::vector<GLuint> pg_particle_initial_color_radius_texID[PG_MAX_CONFIGURATIONS];
extern std::vector<GLuint> pg_particle_acc_texID[PG_MAX_CONFIGURATIONS];
extern std::vector<GLuint> pg_pixel_acc_texID[PG_MAX_CONFIGURATIONS];

extern GLuint Sensor_texture_rectangle[PG_MAX_CONFIGURATIONS];

extern std::string* DisplayTextList;
extern int* DisplayTextFirstInChapter;
extern int NbDisplayTexts;
extern int DisplayText_maxLen;
extern int pg_Ind_Current_DisplayText;
extern float* DisplayText_rand_translX;
extern float* DisplayText_rand_translY;

// message chapters
extern int NbTextChapters;

// MVP matrices
extern glm::mat4 VP1perspMatrix;
extern glm::mat4 VP1viewMatrix;
extern glm::mat4 VP1homographyMatrix;
extern glm::mat4 MeshPosModelMatrix;

// homography matrix
extern float matValues[16];

////////////////////////////////////////
// geometry: track lines
void pg_initRenderingMatrices( void );
void pg_initGeometry_quads( void );
bool pg_loadAllTextures( void );


// photo albums
extern std::string pg_ImageDirectory[PG_MAX_CONFIGURATIONS];
// short video clip albums
extern string pg_ClipDirectory[PG_MAX_CONFIGURATIONS];
extern int pg_NbClipAlbums[PG_MAX_CONFIGURATIONS];
extern int clip_image_width[PG_MAX_CONFIGURATIONS];
extern int clip_image_height[PG_MAX_CONFIGURATIONS];
extern int clip_crop_width[PG_MAX_CONFIGURATIONS];
extern int clip_crop_height[PG_MAX_CONFIGURATIONS];
extern int clip_max_length[PG_MAX_CONFIGURATIONS];
// pen brushes
extern string pen_brushes_fileName[PG_MAX_CONFIGURATIONS];
extern int nb_pen_brushes[PG_MAX_CONFIGURATIONS];
// textures with multiple layers
extern int nb_layers_master_mask[PG_MAX_CONFIGURATIONS];


extern GLuint enumDrawBuffersEntries[16];

/////////////////////////////////////////////////////////////////
// config variables


extern bool mobile_cursor;

extern GLuint pg_CATable_ID;
extern GLubyte *pg_CATable;

// FUNCTIONS
//void pg_loadFirstVideo(void);
void window_display(void);
void pg_automatic_var_reset_or_update(void);
void pg_update_shader_uniforms(void);
void pg_update_shader_var_data(void);
void pg_update_shader_ParticleAnimation_uniforms(void);
void pg_update_shader_Update_uniforms(void);
void pg_update_shader_Mixing_uniforms(void);
void pg_update_shader_Master_uniforms(void);
void pg_update_shader_ParticleRender_uniforms(void);
// no update for uniforms in Sensor and ClipArt shaders
void pg_update_shader_Mesh_uniforms(void);
// PASS #0: PARTICLE ANIMATION PASS
void pg_ParticleAnimationPass(void);
// PASS #1: UPDATE (CA, PIXELS, PARTICLES)
void pg_UpdatePass(void);
// PASS #2: PARTICLE RENDERING PASS
void pg_ClipArtRenderingPass(void);
// PASS #3: PARTICLE RENDERING PASS
void pg_ParticleRenderingPass(void);
// PASS #4: COMPOSITION + PING-PONG ECHO: ECHOED MIX OF LAYERS
void pg_MixingPass(void);
// PASS #5: FINAL DISPLAY: MIX OF ECHOED AND NON-ECHOED LAYERS
void pg_MasterPass(void);
// PASS #5: SENSOR PASS
void pg_SensorPass(void);

// PASS #6: MESH PASS
void pg_calculate_perspective_matrices(void);
void pg_calculate_orthographic_matrices(void);
void pg_MeshPass(void);
void pg_calculate_homography_matrices(std::vector<cv::Point2f>* sourcePoints, std::vector<cv::Point2f>* destinationPoints, GLfloat matValues[], int dim);
// DRAWING A SCENE ON VARIOUS MODALITIES (CURVE, IMAGE, FRAMEBUFFER...)
void pg_draw_scene(DrawingMode mode);
#endif
