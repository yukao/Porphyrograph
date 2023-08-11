/*! \file pg-shader.h
 * 
 *     Part of this source file is taken from Virtual Choreographer
 *     http://virchor.sourceforge.net/
 * 
 *     File pg-shader.h
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

#ifndef PG_SHADER_H
#define PG_SHADER_H

#ifdef TVW
#include "pg_shader_header_TVW.h"
#endif
#ifdef CRITON
#include "pg_shader_header_Criton.h"
#endif
#if defined(KOMPARTSD)
#include "pg_shader_header_KompartSD.h"
#endif
#if defined(LIGHT)
#include "pg_shader_header_Light.h"
#endif
#if defined(RIVETS)
#include "pg_shader_header_Rivets.h"
#endif
#if defined(CORE)
#include "pg_shader_header_Core.h"
#endif
#if defined(FORET)
#include "pg_shader_header_Foret.h"
#endif
#if defined(VOLUSPA)
#include "pg_shader_header_voluspa.h"
#endif
#if defined(ARAKNIT)
#include "pg_shader_header_araknit.h"
#endif
#if defined(CAAUDIO)
#include "pg_shader_header_CAaudio.h"
#endif

unsigned long pg_getFileLength(ifstream& file);
void pg_printShaderCompileLog(GLuint obj);
void pg_printShaderLinkLog(GLuint obj);
void pg_loadAllShaders( void );
void pg_shaderVariablesInit( void );
// INITIALIZES ALL SCENARIO VARIABLES AND ASSIGNS THEM THE VALUES OF THE FIRST SCENARIO LINE
void pg_initializeScenearioVariables( void );

/////////////////////////////////////////////////////////////////////////
// SHADER PROGRAMMES
enum ShaderFileTypes {
#if defined(var_part_initialization) 
	pg_shader_ParticleAnimation = 0,
	pg_shader_Update,
#else
	pg_shader_Update = 0,
#endif
#if defined(var_part_initialization) 
	pg_shader_ParticleRender,
#endif
	pg_shader_Mixing,
	pg_shader_Master, 
#ifdef var_sensor_layout
	pg_shader_Sensor,
#endif
#if defined(var_activeClipArts)
	pg_ClipArt,
#endif
#if defined(var_activeMeshes)
	pg_shader_Mesh,
#endif
	pg_NbShaderTotal
};
extern unsigned int shader_programme[pg_NbShaderTotal];
extern std::unordered_map<int, std::string> pg_stringShaders;

/////////////////////////////////////////////////////////////////////////
// CAMERA SHADER
// CAMERA SHADER UNIFORM VARIABLES
//extern GLint uniform_Camera_vp_model;
//extern GLint uniform_Camera_vp_view;
//extern GLint uniform_Camera_vp_proj;
//extern GLint uniform_Camera_fs_4fv_W_H;
//extern GLint uniform_Camera_vp_2fv_width_height;
//
//// Camera SHADER TEXTURE IDS
//extern GLint uniform_Camera_texture_fs_decal;         // camera capture
//extern GLint uniform_Camera_texture_fs_lookupTable1;  // current background video frame

#if defined(var_part_initialization) 
/////////////////////////////////////////////////////////////////////////
// PARTICLE ANIMATION SHADER
// PARTICLE ANIMATION SHADER UNIFORM VARIABLES
extern GLint uniform_ParticleAnimation_vp_model;
extern GLint uniform_ParticleAnimation_vp_view;
extern GLint uniform_ParticleAnimation_vp_proj;
extern GLint uniform_ParticleAnimation_vp_2fv_width_height;
extern GLint uniform_ParticleAnimation_fs_4fv_W_H_repopChannel_targetFrameNo;
extern GLint uniform_ParticleAnimation_fs_4fv_flashTrkPartWghts;
extern GLint uniform_ParticleAnimation_path_data;
extern GLint uniform_ParticleAnimation_fs_4fv_repop_Color_frameNo;
extern GLint uniform_ParticleAnimation_fs_4fv_flashCAPartWght_nbPart_clear_nbPartInit;
extern GLint uniform_ParticleAnimation_fs_4fv_Camera_W_H_movieWH;
extern GLint uniform_ParticleAnimation_texture_fs_Trk0;  // ping-pong track 0 (FBO)
#if PG_NB_TRACKS >= 2
extern GLint uniform_ParticleAnimation_texture_fs_Trk1;  // ping-pong track 1 (FBO)
#endif
#if PG_NB_TRACKS >= 3
extern GLint uniform_ParticleAnimation_texture_fs_Trk2;  // ping-pong track 2 (FBO)
#endif
#if PG_NB_TRACKS >= 4
extern GLint uniform_ParticleAnimation_texture_fs_Trk3;  // ping-pong track 3 (FBO)
#endif

// UPDATE SHADER TEXTURE IDS
extern GLint uniform_ParticleAnimation_texture_fs_CA;         // ping-pong CA (FBO)
extern GLint uniform_ParticleAnimation_texture_fs_Part_pos_speed;  // 2-cycle ping-pong position/speed of particles (FBO)
extern GLint uniform_ParticleAnimation_texture_fs_Part_col_rad;  // 2-cycle ping-pong color/radius of particles (FBO)
extern GLint uniform_ParticleAnimation_texture_fs_Part_Target_pos_col_rad;  // 2-cycle ping-pong target position/color/radius of particles (FBO)
extern GLint uniform_ParticleAnimation_texture_fs_Camera_frame;  // camera frame
extern GLint uniform_ParticleAnimation_texture_fs_Movie_frame;  // movie frame
extern GLint uniform_ParticleAnimation_texture_fs_Noise;  // 3D noise
extern GLint uniform_ParticleAnimation_texture_fs_RepopDensity;  // repop density
extern GLint uniform_ParticleAnimation_texture_fs_Part_init_pos_speed;  // particle initialization pairs of textures position/speed
extern GLint uniform_ParticleAnimation_texture_fs_Part_init_col_rad;  // particle initialization pairs of textures color/radius
extern GLint uniform_ParticleAnimation_texture_fs_Part_acc;  // particle acceleration texture
#endif

/////////////////////////////////////////////////////////////////////////
// UPDATE SHADER
// UPDATE SHADER UNIFORM VARIABLES
extern GLint uniform_Update_vp_model;
extern GLint uniform_Update_vp_view;
extern GLint uniform_Update_vp_proj;
#if defined(PG_WITH_PHOTO_HOMOGRAPHY)
extern GLint uniform_Update_homographyForTexture;
#endif
extern GLint uniform_Update_vp_2fv_width_height;
extern GLint uniform_Update_fs_4fv_W_H_time_currentScene;
extern GLint uniform_Update_fs_3fv_clearAllLayers_clearCA_pulsedShift;
#if defined(CAAUDIO) || defined(RIVETS)
extern GLint uniform_Update_fs_4fv_CAseed_type_size_loc;
#endif
#if defined(var_part_initialization) || PG_NB_TRACKS >= 2
extern GLint uniform_Update_fs_4fv_flashTrkBGWghts_flashPartBGWght;
#endif
extern GLint uniform_Update_fs_3fv_frameno_Cursor_flashPartCAWght;
#ifdef var_nb_CATypes
extern GLint uniform_Update_fs_4fv_flashTrkCAWghts;
#endif
extern GLint uniform_Update_path_data;
#ifdef CRITON
extern GLint uniform_Update_fs_4fv_fftLevels03;
extern GLint uniform_Update_fs_4fv_fftFrequencies03;
extern GLint uniform_Update_fs_4fv_fftPhases03;
extern GLint uniform_Update_fs_4fv_fftLevels47;
extern GLint uniform_Update_fs_4fv_fftFrequencies47;
extern GLint uniform_Update_fs_4fv_fftPhases47;
#endif
extern GLint uniform_Update_fs_4fv_movieWH_flashCameraTrkWght_cpTrack;
extern GLint uniform_Update_fs_4fv_repop_ColorBG_flashCABGWght;
#if defined(var_nb_CATypes) && !defined(var_alKemi)
extern GLint uniform_Update_fs_3fv_repop_ColorCA;
#endif
extern GLint uniform_Update_fs_3fv_isClearLayer_flashPixel_flashCameraTrkThres;
#if defined(var_flashPhotoTrkBeat) && defined(var_flashPhotoTrkBright) && defined(var_flashPhotoTrkLength) && defined(var_flashPhotoChangeBeat)
extern GLint uniform_Update_fs_4fv_flashPhotoTrkWght_flashPhotoTrkThres_Photo_offSetsXY;
#endif
#if defined(var_photo_diaporama) || defined(TVW)
extern GLint uniform_Update_fs_4fv_photo01_wh;
#endif
#ifdef var_photo_diaporama
extern GLint uniform_Update_fs_4fv_photo01Wghts_randomValues;
#endif
#if defined(var_clipCaptFreq) && PG_NB_PARALLEL_CLIPS >= 2
extern GLint uniform_Update_fs_2fv_clip01Wghts;
#endif
#if defined(var_cameraCaptFreq)
extern GLint uniform_Update_fs_4fv_Camera_offSetsXY_Camera_W_H;
#endif
extern GLint uniform_Update_fs_4fv_xy_transl_tracks_0_1;
#if defined(var_nb_CATypes) || defined(PG_WITH_BLUR)
extern GLint uniform_Update_fs_4fv_CAType_SubType_blurRadius;
#endif
#if defined(var_clipCaptFreq)
extern GLint uniform_Update_fs_3fv_photo_rgb;
#endif
#if defined(var_CATable)
extern GLint uniform_Update_texture_fs_CATable;
#endif
#if defined(var_GenerativeNights_planes)
extern GLint uniform_Update_fs_2fv_initCA_1stPlaneFrameNo;
#endif
#if defined(var_camera_BG_ini_subtr)
extern GLint uniform_Update_texture_fs_Camera_BGIni;
#endif
extern GLint uniform_Update_texture_fs_pixel_acc;

// UPDATE SHADER TEXTURE IDS
#ifdef var_nb_CATypes
extern GLint uniform_Update_texture_fs_CA;         // ping-pong CA (FBO)
#endif
#ifdef var_alKemi
extern GLint uniform_Update_texture_fs_PreviousCA;         // ping-pong CA (FBO)
#endif
#ifdef PG_NB_PIXEL_MODES
extern GLint uniform_Update_texture_fs_Pixels;  // 2-cycle ping-pong speed/position of pixels (FBO)
#endif
extern GLint uniform_Update_texture_fs_Trk0;  // ping-pong track 0 (FBO)
#if PG_NB_TRACKS >= 2
extern GLint uniform_Update_texture_fs_Trk1;  // ping-pong track 1 (FBO)
#endif
#if PG_NB_TRACKS >= 3
extern GLint uniform_Update_texture_fs_Trk2;  // ping-pong track 2 (FBO)
#endif
#if PG_NB_TRACKS >= 4
extern GLint uniform_Update_texture_fs_Trk3;  // ping-pong track 3 (FBO)
#endif
#if !defined(PG_BEZIER_PATHS) || defined(FORET) || defined(CORE)
extern GLint uniform_Update_texture_fs_Brushes;  // pen patterns
#endif
extern GLint uniform_Update_texture_fs_Camera_frame;  // camera frame
extern GLint uniform_Update_texture_fs_Camera_BG;  // camera BG capture
extern GLint uniform_Update_texture_fs_Movie_frame;  // movie frame
extern GLint uniform_Update_texture_fs_Noise;  // 3D noise
#if defined(var_BG_CA_repop_density)
extern GLint uniform_Update_texture_fs_RepopDensity;  // repop density
#endif
extern GLint uniform_Update_texture_fs_Photo0;  // photo[0]
extern GLint uniform_Update_texture_fs_Photo1;  // photo[1]
extern GLint uniform_Update_texture_fs_Clip0;  // clip[0]
extern GLint uniform_Update_texture_fs_Clip1;  // clip[1]
#if defined(var_part_initialization) 
extern GLint uniform_Update_texture_fs_Part_render;  // FBO capture of particle rendering used for flashing layers with particles
#endif
extern GLint uniform_Update_texture_fs_Part_init_pos_speed;  // particle initialization pairs of textures position/speed
extern GLint uniform_Update_texture_fs_Part_init_col_rad;  // particle initialization pairs of textures color/radius

/////////////////////////////////////////////////////////////////////////
// PARTICLE RENDERING SHADER
// PARTICLE RENDERING SHADER UNIFORM VARIABLES
#if defined(var_part_initialization) 
extern GLint uniform_ParticleSplat_vp_model;
extern GLint uniform_ParticleSplat_vp_view;
extern GLint uniform_ParticleSplat_vp_proj;
extern GLint uniform_ParticleSplat_gs_4fv_part_size_partType_highPitchPulse_windowRatio;
extern GLint uniform_ParticleSplat_vp_3fv_trackReplay_xy_height;

// PARTICLE SHADER TEXTURE IDS
extern GLint uniform_ParticleSplat_texture_vp_Part_pos_speed;				 // Particle position/speed update
extern GLint uniform_ParticleSplat_texture_vp_Part_col_rad;          // Particle color/radius update
#endif
#if defined(CURVE_PARTICLES) 
extern GLint uniform_ParticleCurve_vp_model;
extern GLint uniform_ParticleCurve_vp_view;
extern GLint uniform_ParticleCurve_vp_proj;
extern GLint uniform_ParticleCurve_gs_3fv_partRadius_partType_highPitchPulse;
extern GLint uniform_ParticleCurve_vp_3fv_trackReplay_xy_height;

// PARTICLE SHADER TEXTURE IDS
extern GLint uniform_ParticleCurve_texture_vp_Part_pos_speed;				 // Particle position/speed update
extern GLint uniform_ParticleCurve_texture_vp_Part_col_rad;          // Particle color/radius update
#endif
#ifdef CURVE_PARTICLES
// color texture
extern GLint uniform_ParticleCurve_Comet_texture_fs_decal;  // comet texture
#endif
#if defined(TEXTURED_QUAD_PARTICLES)
// color texture
extern GLint uniform_ParticleSplat_texture_fs_decal;  // blurred disk texture
#endif

/////////////////////////////////////////////////////////////////////////
// MIXING SHADER
// MIXING SHADER UNIFORM VARIABLES
extern GLint uniform_Mixing_vp_model;
extern GLint uniform_Mixing_vp_view;
extern GLint uniform_Mixing_vp_proj;
#if defined(var_flashPhotoTrkBeat) && defined(var_flashPhotoTrkBright) && defined(var_flashPhotoTrkLength) && defined(var_flashPhotoChangeBeat)
extern GLint uniform_Mixing_fs_3fv_height_flashCameraTrkWght_flashPhotoTrkWght;
#else
extern GLint uniform_Mixing_fs_2fv_height_flashCameraTrkWght;
#endif
extern GLint uniform_Mixing_fs_3fv_screenMsgTransp_Text1_2_Alpha;

// MIXING SHADER TEXTURE IDS
#ifdef var_nb_CATypes
extern GLint uniform_Mixing_texture_fs_CA; // ping-pong BG track (FBO)
#endif
#if defined(var_part_initialization)
extern GLint uniform_Mixing_texture_fs_Part_render;  // Particles (FBO)
#endif
extern GLint uniform_Mixing_texture_fs_Trk0;  // ping-pong track 0 (FBO)
#if PG_NB_TRACKS >= 2
extern GLint uniform_Mixing_texture_fs_Trk1;  // ping-pong track 1 (FBO)
#endif
#if PG_NB_TRACKS >= 3
extern GLint uniform_Mixing_texture_fs_Trk2;  // ping-pong track 2 (FBO)
#endif
#if PG_NB_TRACKS >= 4
extern GLint uniform_Mixing_texture_fs_Trk3;  // ping-pong track 3 (FBO)
#endif
#if defined(TVW)
extern GLint uniform_Mixing_texture_fs_Display_Font;  // message Font
extern GLint uniform_Mixing_texture_fs_Display_Message1; // tweets at the bottom of the screen
extern GLint uniform_Mixing_texture_fs_Display_Message2; // tweets at the bottom of the screen
#endif
extern GLint uniform_Mixing_texture_fs_Render_prec;  // preceding snapshot
extern GLint uniform_Mixing_texture_fs_Screen_Font;  // message Font
extern GLint uniform_Mixing_texture_fs_Screen_Message;  // message string
#if defined(TVW)
extern GLint uniform_Update_fs_4fv_weights03;
extern GLint uniform_Update_fs_2fv_weights45;
extern GLint uniform_Update_fs_3fv_alphaSwap02;
extern GLint uniform_Update_fs_3fv_alphaSwap35;
extern GLint uniform_Update_fs_4fv_image_noisesxy;
extern GLint uniform_Update_fs_4fv_mask_noisesxy;

extern GLint uniform_Update_texture_fs_TVWPixels0;  // image buffer #0
extern GLint uniform_Update_texture_fs_TVWPixels1;  // image buffer #1
extern GLint uniform_Update_texture_fs_TVWPixels2;  // image buffer #2
extern GLint uniform_Update_texture_fs_TVWPixels3;  // image buffer #3
extern GLint uniform_Update_texture_fs_TVWPixels4;  // image buffer #4
extern GLint uniform_Update_texture_fs_TVWPixels5;  // image buffer #5
extern GLint uniform_Update_texture_fs_TVWMask02;  // image mask #0
extern GLint uniform_Update_texture_fs_TVWMask35;  // image mask #1
extern GLint uniform_Update_texture_fs_TVWPixelsSwap0;  // buffer swap image #0
extern GLint uniform_Update_texture_fs_TVWPixelsSwap1;  // buffer swap image #1 
extern GLint uniform_Update_texture_fs_TVWPixelsSwap2;  // buffer swap image #2 
extern GLint uniform_Update_texture_fs_TVWPixelsSwap3;  // buffer swap image #3 
extern GLint uniform_Update_texture_fs_TVWPixelsSwap4;  // buffer swap image #4 
extern GLint uniform_Update_texture_fs_TVWPixelsSwap5;  // buffer swap image #5 
#endif

/////////////////////////////////////////////////////////////////////////
// MASTER SHADER
// MASTER SHADER UNIFORM VARIABLES
extern GLint uniform_Master_vp_model;
extern GLint uniform_Master_vp_view;
extern GLint uniform_Master_vp_proj;
extern GLint uniform_Master_fs_4fv_xy_frameno_pulsedShift;
#if defined(TVW) || defined(var_alKemi)
extern GLint uniform_Master_fs_4fv_width_height_rightWindowVMargin_timeFromStart;
#else
extern GLint uniform_Master_fs_4fv_width_height_timeFromStart_muteRightScreen;
#endif
//extern GLint uniform_Master_fs_4fv_pulsedColor_rgb_pen_grey;
//extern GLint uniform_Master_fs_4fv_interpolatedPaletteLow_rgb_currentScene;
//extern GLint uniform_Master_fs_4fv_interpolatedPaletteMedium_rgb_mobile_cursor;
//extern GLint uniform_Master_fs_3fv_interpolatedPaletteHigh_rgb;
extern GLint uniform_Master_fs_2iv_mobile_cursor_currentScene;
#ifdef var_Caverne_BackColor
extern GLint uniform_Master_fs_3fv_Caverne_BackColor_rgb;
#endif
#if defined(var_flashchange_BGcolor_freq)
extern GLint uniform_Master_fs_3fv_BG_color_rgb;
#endif

// MASTER SHADER TEXTURE IDS
extern GLint uniform_Master_texture_fs_Render_curr;         // previous pass output
#ifdef var_nb_CATypes
extern GLint uniform_Master_texture_fs_CA;  // ping-pong CA (FBO)
#endif
extern GLint uniform_Master_texture_fs_Part_render;  // Particles
extern GLint uniform_Master_texture_fs_Trk0;  // ping-pong track 0 (FBO)
#if PG_NB_TRACKS >= 2
extern GLint uniform_Master_texture_fs_Trk1;  // ping-pong track 1 (FBO)
#endif
#if PG_NB_TRACKS >= 3
extern GLint uniform_Master_texture_fs_Trk2;  // ping-pong track 2 (FBO)
#endif
#if PG_NB_TRACKS >= 4
extern GLint uniform_Master_texture_fs_Trk3;  // ping-pong track 3 (FBO)
#endif
#ifdef PG_WITH_MASTER_MASK
extern GLint uniform_Master_texture_fs_Mask;  // mask for master output
#endif

#ifdef var_sensor_layout
  /////////////////////////////////////////////////////////////////////////
// SENSOR SHADER
// SENSOR SHADER UNIFORM VARIABLES
extern GLint uniform_Sensor_vp_model;
extern GLint uniform_Sensor_vp_view;
extern GLint uniform_Sensor_vp_proj;

// SENSOR SHADER TEXTURE IDS
extern GLint uniform_Sensor_texture_fs_decal;         // sensor texture
extern GLint uniform_Sensor_fs_4fv_onOff_isCurrentSensor_masterLevel_transparency;
// extern GLint uniform_Sensor_fs_2fv_frameno_invert;
#endif

#if defined(var_activeMeshes)
/////////////////////////////////////////////////////////////////////////
// MESH SHADER
// MESH SHADER UNIFORM VARIABLES
extern GLint uniform_Mesh_vp_model;
extern GLint uniform_Mesh_vp_view;
extern GLint uniform_Mesh_vp_proj;
extern GLint uniform_Mesh_fs_4fv_isDisplayLookAt_with_mesh_with_blue_currentScene;
extern GLint uniform_Mesh_fs_3fv_light;
#if defined(var_Contact_mesh_expand)
extern GLint uniform_Mesh_vp_2fv_dilate_explode;
#endif
#if defined(var_Contact_mesh_anime)
extern GLint uniform_Mesh_bones_matrices;
#endif
#if defined(var_MmeShanghai_brokenGlass)
extern GLint uniform_Mesh_fs_4fv_color;
extern GLint uniform_Mesh_fs_4fv_color_master_photo_weight_bg;
#endif
// Augmented Reality: FBO capture of Master to be displayed on a mesh
#if defined(var_textureFrontier_wmin) && defined(var_textureFrontier_wmax) and defined(var_textureFrontier_hmin) && defined(var_textureFrontier_hmax) && defined(var_textureFrontier_wmin_width) && defined(var_textureFrontier_wmax_width) and defined(var_textureFrontier_hmin_width) && defined(var_textureFrontier_hmax_width) && defined(var_textureScale_w) && defined(var_textureScale_h) and defined(var_textureTranslate_w) && defined(var_textureTranslate_h) 
extern GLint uniform_Mesh_fs_4fv_textureFrontier;
extern GLint uniform_Mesh_fs_4fv_textureFrontier_width;
extern GLint uniform_Mesh_fs_4fv_textureScaleTransl;
#endif
// Mesh SHADER TEXTURE IDS
extern GLint uniform_Mesh_texture_fs_decal;         // Mesh texture
#if defined(var_Contact_mesh_palette)
extern GLint uniform_Mesh_fs_4fv_color_palette;         // Mesh color
#endif
#if defined(var_MmeShanghai_brokenGlass)
extern GLint uniform_Mesh_texture_fs_BG;         // Mesh texture
#endif
#endif

#endif
