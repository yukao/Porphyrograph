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
void pg_loadAllShaders(void);
void pg_shaderVariablesInit( void );
// INITIALIZES ALL SCENARIO VARIABLES AND ASSIGNS THEM THE VALUES OF THE FIRST SCENARIO LINE
void pg_initializeScenearioVariables( void );

/////////////////////////////////////////////////////////////////////////
// SHADER PROGRAMMES
enum ShaderFileTypes {
	_pg_shader_ParticleAnimation = 0,
	_pg_shader_Update,
	_pg_shader_ParticleRender,
	_pg_shader_Mixing,
	_pg_shader_Master,
	_pg_shader_Sensor,
	_pg_shader_ClipArt,
	_pg_shader_Mesh,
	_pg_shader_TotalNbTypes
};
extern unsigned int ** pg_shader_programme;
extern unsigned int pg_current_configuration_rank;
extern std::unordered_map<int, std::string> pg_stringShaderTypes;

/////////////////////////////////////////////////////////////////////////
// CAMERA SHADER
// CAMERA SHADER UNIFORM VARIABLES
//extern GLint uniform_Camera_vp_model[_NbConfigurations];
//extern GLint uniform_Camera_vp_view[_NbConfigurations];
//extern GLint uniform_Camera_vp_proj[_NbConfigurations];
//extern GLint uniform_Camera_fs_4fv_W_H[_NbConfigurations];
//extern GLint uniform_Camera_vp_2fv_width_height[_NbConfigurations];
//
//// Camera SHADER TEXTURE IDS
//extern GLint uniform_Camera_texture_fs_decal[_NbConfigurations];         // camera capture
//extern GLint uniform_Camera_texture_fs_lookupTable1[_NbConfigurations];  // current background video frame

#if defined(var_part_initialization) 
/////////////////////////////////////////////////////////////////////////
// PARTICLE ANIMATION SHADER
// PARTICLE ANIMATION SHADER UNIFORM VARIABLES
extern GLint uniform_ParticleAnimation_vp_model[_NbConfigurations];
extern GLint uniform_ParticleAnimation_vp_view[_NbConfigurations];
extern GLint uniform_ParticleAnimation_vp_proj[_NbConfigurations];
extern GLint uniform_ParticleAnimation_vp_2fv_width_height[_NbConfigurations];
extern GLint uniform_ParticleAnimation_fs_4fv_W_H_repopChannel_targetFrameNo[_NbConfigurations];
extern GLint uniform_ParticleAnimation_fs_4fv_flashTrkPartWghts[_NbConfigurations];
extern GLint uniform_ParticleAnimation_path_data[_NbConfigurations];
extern GLint uniform_ParticleAnimation_fs_4fv_repop_Color_frameNo[_NbConfigurations];
extern GLint uniform_ParticleAnimation_fs_4fv_flashCAPartWght_nbPart_clear_nbPartInit[_NbConfigurations];
extern GLint uniform_ParticleAnimation_fs_4fv_Camera_W_H_movieWH[_NbConfigurations];
extern GLint uniform_ParticleAnimation_texture_fs_Trk0[_NbConfigurations];  // ping-pong track 0 (FBO)
#if PG_NB_TRACKS >= 2
extern GLint uniform_ParticleAnimation_texture_fs_Trk1[_NbConfigurations];  // ping-pong track 1 (FBO)
#endif
#if PG_NB_TRACKS >= 3
extern GLint uniform_ParticleAnimation_texture_fs_Trk2[_NbConfigurations];  // ping-pong track 2 (FBO)
#endif
#if PG_NB_TRACKS >= 4
extern GLint uniform_ParticleAnimation_texture_fs_Trk3[_NbConfigurations];  // ping-pong track 3 (FBO)
#endif

// UPDATE SHADER TEXTURE IDS
extern GLint uniform_ParticleAnimation_texture_fs_CA[_NbConfigurations];         // ping-pong CA (FBO)
extern GLint uniform_ParticleAnimation_texture_fs_Part_pos_speed[_NbConfigurations];  // 2-cycle ping-pong position/speed of particles (FBO)
extern GLint uniform_ParticleAnimation_texture_fs_Part_col_rad[_NbConfigurations];  // 2-cycle ping-pong color/radius of particles (FBO)
extern GLint uniform_ParticleAnimation_texture_fs_Part_Target_pos_col_rad[_NbConfigurations];  // 2-cycle ping-pong target position/color/radius of particles (FBO)
extern GLint uniform_ParticleAnimation_texture_fs_Camera_frame[_NbConfigurations];  // camera frame
extern GLint uniform_ParticleAnimation_texture_fs_Movie_frame[_NbConfigurations];  // movie frame
extern GLint uniform_ParticleAnimation_texture_fs_Noise[_NbConfigurations];  // 3D noise
extern GLint uniform_ParticleAnimation_texture_fs_RepopDensity[_NbConfigurations];  // repop density
extern GLint uniform_ParticleAnimation_texture_fs_Part_init_pos_speed[_NbConfigurations];  // particle initialization pairs of textures position/speed
extern GLint uniform_ParticleAnimation_texture_fs_Part_init_col_rad[_NbConfigurations];  // particle initialization pairs of textures color/radius
extern GLint uniform_ParticleAnimation_texture_fs_Part_acc[_NbConfigurations];  // particle acceleration texture
#endif

/////////////////////////////////////////////////////////////////////////
// UPDATE SHADER
// UPDATE SHADER UNIFORM VARIABLES
extern GLint uniform_Update_vp_model[_NbConfigurations];
extern GLint uniform_Update_vp_view[_NbConfigurations];
extern GLint uniform_Update_vp_proj[_NbConfigurations];
#if defined(PG_WITH_PHOTO_HOMOGRAPHY)
extern GLint uniform_Update_homographyForTexture[_NbConfigurations];
#endif
extern GLint uniform_Update_vp_2fv_width_height[_NbConfigurations];
extern GLint uniform_Update_fs_4fv_W_H_time_currentScene[_NbConfigurations];
extern GLint uniform_Update_fs_3fv_clearAllLayers_clearCA_pulsedShift[_NbConfigurations];
#if defined(CAAUDIO) || defined(RIVETS)
extern GLint uniform_Update_fs_4fv_CAseed_type_size_loc[_NbConfigurations];
#endif
extern GLint uniform_Update_fs_4fv_flashTrkBGWghts_flashPartBGWght[_NbConfigurations];
extern GLint uniform_Update_fs_3fv_frameno_Cursor_flashPartCAWght[_NbConfigurations];
#if defined(var_nb_CATypes)
extern GLint uniform_Update_fs_4fv_flashTrkCAWghts[_NbConfigurations];
#endif
extern GLint uniform_Update_path_data[_NbConfigurations];
#ifdef CRITON
extern GLint uniform_Update_fs_4fv_fftLevels03[_NbConfigurations];
extern GLint uniform_Update_fs_4fv_fftFrequencies03[_NbConfigurations];
extern GLint uniform_Update_fs_4fv_fftPhases03[_NbConfigurations];
extern GLint uniform_Update_fs_4fv_fftLevels47[_NbConfigurations];
extern GLint uniform_Update_fs_4fv_fftFrequencies47[_NbConfigurations];
extern GLint uniform_Update_fs_4fv_fftPhases47[_NbConfigurations];
#endif
extern GLint uniform_Update_fs_4fv_movieWH_flashCameraTrkWght_cpTrack[_NbConfigurations];
extern GLint uniform_Update_fs_4fv_repop_ColorBG_flashCABGWght[_NbConfigurations];
#if defined(var_nb_CATypes)
extern GLint uniform_Update_fs_3fv_repop_ColorCA[_NbConfigurations];
#endif
extern GLint uniform_Update_fs_3fv_isClearLayer_flashPixel_flashCameraTrkThres[_NbConfigurations];
#if defined(var_flashPhotoTrkBeat) && defined(var_flashPhotoTrkBright) && defined(var_flashPhotoTrkLength) && defined(var_flashPhotoChangeBeat)
extern GLint uniform_Update_fs_4fv_flashPhotoTrkWght_flashPhotoTrkThres_Photo_offSetsXY[_NbConfigurations];
#endif
#if defined(var_photo_diaporama) || defined(TVW)
extern GLint uniform_Update_fs_4fv_photo01_wh[_NbConfigurations];
#endif
#if defined(var_photo_diaporama)
extern GLint uniform_Update_fs_4fv_photo01Wghts_randomValues[_NbConfigurations];
#endif
#if defined(var_clipCaptFreq) && PG_NB_PARALLEL_CLIPS >= 2
extern GLint uniform_Update_fs_2fv_clip01Wghts[_NbConfigurations];
#endif
#if defined(var_cameraCaptFreq)
extern GLint uniform_Update_fs_4fv_Camera_offSetsXY_Camera_W_H[_NbConfigurations];
#endif
extern GLint uniform_Update_fs_4fv_xy_transl_tracks_0_1[_NbConfigurations];
#if defined(var_nb_CATypes) || defined(PG_WITH_BLUR)
extern GLint uniform_Update_fs_4fv_CAType_SubType_blurRadius[_NbConfigurations];
#endif
#if defined(var_clipCaptFreq)
extern GLint uniform_Update_fs_3fv_photo_rgb[_NbConfigurations];
#endif
#if defined(var_CATable)
extern GLint uniform_Update_texture_fs_CATable[_NbConfigurations];
#endif
#if defined(var_GenerativeNights_planes)
extern GLint uniform_Update_fs_2fv_initCA_1stPlaneFrameNo[_NbConfigurations];
#endif
#if defined(var_camera_BG_ini_subtr)
extern GLint uniform_Update_texture_fs_Camera_BGIni[_NbConfigurations];
#endif
extern GLint uniform_Update_texture_fs_pixel_acc[_NbConfigurations];

// UPDATE SHADER TEXTURE IDS
#if defined(var_nb_CATypes)
extern GLint uniform_Update_texture_fs_CA[_NbConfigurations];         // ping-pong CA (FBO)
#endif
extern GLint uniform_Update_texture_fs_PreviousCA[_NbConfigurations];         // ping-pong CA (FBO)
extern GLint uniform_Update_texture_fs_Pixels[_NbConfigurations];  // 2-cycle ping-pong speed/position of pixels (FBO)
extern GLint uniform_Update_texture_fs_Trk0[_NbConfigurations];  // ping-pong track 0 (FBO)
#if PG_NB_TRACKS >= 2
extern GLint uniform_Update_texture_fs_Trk1[_NbConfigurations];  // ping-pong track 1 (FBO)
#endif
#if PG_NB_TRACKS >= 3
extern GLint uniform_Update_texture_fs_Trk2[_NbConfigurations];  // ping-pong track 2 (FBO)
#endif
#if PG_NB_TRACKS >= 4
extern GLint uniform_Update_texture_fs_Trk3[_NbConfigurations];  // ping-pong track 3 (FBO)
#endif
#if !defined(PG_BEZIER_PATHS) || defined(FORET) || defined(CORE)
extern GLint uniform_Update_texture_fs_Brushes[_NbConfigurations];  // pen patterns
#endif
extern GLint uniform_Update_texture_fs_Camera_frame[_NbConfigurations];  // camera frame
extern GLint uniform_Update_texture_fs_Camera_BG[_NbConfigurations];  // camera BG capture
extern GLint uniform_Update_texture_fs_Movie_frame[_NbConfigurations];  // movie frame
extern GLint uniform_Update_texture_fs_Noise[_NbConfigurations];  // 3D noise
#if defined(var_BG_CA_repop_density)
extern GLint uniform_Update_texture_fs_RepopDensity[_NbConfigurations];  // repop density
#endif
extern GLint uniform_Update_texture_fs_Photo0[_NbConfigurations];  // photo[0]
extern GLint uniform_Update_texture_fs_Photo1[_NbConfigurations];  // photo[1]
extern GLint uniform_Update_texture_fs_Clip0[_NbConfigurations];  // clip[0]
extern GLint uniform_Update_texture_fs_Clip1[_NbConfigurations];  // clip[1]
#if defined(var_part_initialization) 
extern GLint uniform_Update_texture_fs_Part_render[_NbConfigurations];  // FBO capture of particle rendering used for flashing layers with particles
#endif
extern GLint uniform_Update_texture_fs_Part_init_pos_speed[_NbConfigurations];  // particle initialization pairs of textures position/speed
extern GLint uniform_Update_texture_fs_Part_init_col_rad[_NbConfigurations];  // particle initialization pairs of textures color/radius

/////////////////////////////////////////////////////////////////////////
// PARTICLE RENDERING SHADER
// PARTICLE RENDERING SHADER UNIFORM VARIABLES
#if defined(var_part_initialization) 
extern GLint uniform_ParticleSplat_vp_model[_NbConfigurations];
extern GLint uniform_ParticleSplat_vp_view[_NbConfigurations];
extern GLint uniform_ParticleSplat_vp_proj[_NbConfigurations];
extern GLint uniform_ParticleSplat_gs_4fv_part_size_partType_highPitchPulse_windowRatio[_NbConfigurations];
extern GLint uniform_ParticleSplat_vp_3fv_trackReplay_xy_height[_NbConfigurations];

// PARTICLE SHADER TEXTURE IDS
extern GLint uniform_ParticleSplat_texture_vp_Part_pos_speed[_NbConfigurations];				 // Particle position/speed update
extern GLint uniform_ParticleSplat_texture_vp_Part_col_rad[_NbConfigurations];          // Particle color/radius update
#endif
#if defined(CURVE_PARTICLES) 
extern GLint uniform_ParticleCurve_vp_model[_NbConfigurations];
extern GLint uniform_ParticleCurve_vp_view[_NbConfigurations];
extern GLint uniform_ParticleCurve_vp_proj[_NbConfigurations];
extern GLint uniform_ParticleCurve_gs_3fv_partRadius_partType_highPitchPulse[_NbConfigurations];
extern GLint uniform_ParticleCurve_vp_3fv_trackReplay_xy_height[_NbConfigurations];

// PARTICLE SHADER TEXTURE IDS
extern GLint uniform_ParticleCurve_texture_vp_Part_pos_speed[_NbConfigurations];				 // Particle position/speed update
extern GLint uniform_ParticleCurve_texture_vp_Part_col_rad[_NbConfigurations];          // Particle color/radius update
#endif
#ifdef CURVE_PARTICLES
// color texture
extern GLint uniform_ParticleCurve_Comet_texture_fs_decal[_NbConfigurations];  // comet texture
#endif
#if defined(TEXTURED_QUAD_PARTICLES)
// color texture
extern GLint uniform_ParticleSplat_texture_fs_decal[_NbConfigurations];  // blurred disk texture
#endif

/////////////////////////////////////////////////////////////////////////
// MIXING SHADER
// MIXING SHADER UNIFORM VARIABLES
extern GLint uniform_Mixing_vp_model[_NbConfigurations];
extern GLint uniform_Mixing_vp_view[_NbConfigurations];
extern GLint uniform_Mixing_vp_proj[_NbConfigurations];
extern GLint uniform_Mixing_fs_3fv_height_flashCameraTrkWght_flashPhotoTrkWght[_NbConfigurations];
extern GLint uniform_Mixing_fs_3fv_screenMsgTransp_Text1_2_Alpha[_NbConfigurations];

// MIXING SHADER TEXTURE IDS
#if defined(var_nb_CATypes)
extern GLint uniform_Mixing_texture_fs_CA[_NbConfigurations]; // ping-pong BG track (FBO)
#endif
extern GLint uniform_Mixing_texture_fs_ClipArt_render[_NbConfigurations];  // Particles (FBO)
extern GLint uniform_Mixing_texture_fs_Particle_render[_NbConfigurations];  // Particles (FBO)
extern GLint uniform_Mixing_texture_fs_Trk0[_NbConfigurations];  // ping-pong track 0 (FBO)
#if PG_NB_TRACKS >= 2
extern GLint uniform_Mixing_texture_fs_Trk1[_NbConfigurations];  // ping-pong track 1 (FBO)
#endif
#if PG_NB_TRACKS >= 3
extern GLint uniform_Mixing_texture_fs_Trk2[_NbConfigurations];  // ping-pong track 2 (FBO)
#endif
#if PG_NB_TRACKS >= 4
extern GLint uniform_Mixing_texture_fs_Trk3[_NbConfigurations];  // ping-pong track 3 (FBO)
#endif
#if defined(TVW)
extern GLint uniform_Mixing_texture_fs_Display_Font[_NbConfigurations];  // message Font
extern GLint uniform_Mixing_texture_fs_Display_Message1[_NbConfigurations]; // tweets at the bottom of the screen
extern GLint uniform_Mixing_texture_fs_Display_Message2[_NbConfigurations]; // tweets at the bottom of the screen
#endif
extern GLint uniform_Mixing_texture_fs_Render_prec[_NbConfigurations];  // preceding snapshot
extern GLint uniform_Mixing_texture_fs_Screen_Font[_NbConfigurations];  // message Font
extern GLint uniform_Mixing_texture_fs_Screen_Message[_NbConfigurations];  // message string
#if defined(TVW)
extern GLint uniform_Update_fs_4fv_weights03[_NbConfigurations];
extern GLint uniform_Update_fs_2fv_weights45[_NbConfigurations];
extern GLint uniform_Update_fs_3fv_alphaSwap02[_NbConfigurations];
extern GLint uniform_Update_fs_3fv_alphaSwap35[_NbConfigurations];
extern GLint uniform_Update_fs_4fv_image_noisesxy[_NbConfigurations];
extern GLint uniform_Update_fs_4fv_mask_noisesxy[_NbConfigurations];

extern GLint uniform_Update_texture_fs_TVWPixels0[_NbConfigurations];  // image buffer #0
extern GLint uniform_Update_texture_fs_TVWPixels1[_NbConfigurations];  // image buffer #1
extern GLint uniform_Update_texture_fs_TVWPixels2[_NbConfigurations];  // image buffer #2
extern GLint uniform_Update_texture_fs_TVWPixels3[_NbConfigurations];  // image buffer #3
extern GLint uniform_Update_texture_fs_TVWPixels4[_NbConfigurations];  // image buffer #4
extern GLint uniform_Update_texture_fs_TVWPixels5[_NbConfigurations];  // image buffer #5
extern GLint uniform_Update_texture_fs_TVWMask02[_NbConfigurations];  // image mask #0
extern GLint uniform_Update_texture_fs_TVWMask35[_NbConfigurations];  // image mask #1
extern GLint uniform_Update_texture_fs_TVWPixelsSwap0[_NbConfigurations];  // buffer swap image #0
extern GLint uniform_Update_texture_fs_TVWPixelsSwap1[_NbConfigurations];  // buffer swap image #1 
extern GLint uniform_Update_texture_fs_TVWPixelsSwap2[_NbConfigurations];  // buffer swap image #2 
extern GLint uniform_Update_texture_fs_TVWPixelsSwap3[_NbConfigurations];  // buffer swap image #3 
extern GLint uniform_Update_texture_fs_TVWPixelsSwap4[_NbConfigurations];  // buffer swap image #4 
extern GLint uniform_Update_texture_fs_TVWPixelsSwap5[_NbConfigurations];  // buffer swap image #5 
#endif

/////////////////////////////////////////////////////////////////////////
// MASTER SHADER
// MASTER SHADER UNIFORM VARIABLES
extern GLint uniform_Master_vp_model[_NbConfigurations];
extern GLint uniform_Master_vp_view[_NbConfigurations];
extern GLint uniform_Master_vp_proj[_NbConfigurations];
extern GLint uniform_Master_fs_4fv_xy_frameno_pulsedShift[_NbConfigurations];
#if defined(TVW)
extern GLint uniform_Master_fs_4fv_width_height_rightWindowVMargin_timeFromStart[_NbConfigurations];
#endif
extern GLint uniform_Master_fs_4fv_width_height_timeFromStart_muteRightScreen[_NbConfigurations];
//extern GLint uniform_Master_fs_4fv_pulsedColor_rgb_pen_grey[_NbConfigurations];
//extern GLint uniform_Master_fs_4fv_interpolatedPaletteLow_rgb_currentScene[_NbConfigurations];
//extern GLint uniform_Master_fs_4fv_interpolatedPaletteMedium_rgb_mobile_cursor[_NbConfigurations];
//extern GLint uniform_Master_fs_3fv_interpolatedPaletteHigh_rgb[_NbConfigurations];
extern GLint uniform_Master_fs_2iv_mobile_cursor_currentScene[_NbConfigurations];
#if defined(var_Caverne_BackColor)
extern GLint uniform_Master_fs_3fv_Caverne_BackColor_rgb[_NbConfigurations];
#endif
#if defined(var_flashchange_BGcolor_freq)
extern GLint uniform_Master_fs_3fv_BG_color_rgb[_NbConfigurations];
#endif

// MASTER SHADER TEXTURE IDS
extern GLint uniform_Master_texture_fs_Render_curr[_NbConfigurations];         // previous pass output
#if defined(var_nb_CATypes)
extern GLint uniform_Master_texture_fs_CA[_NbConfigurations];  // ping-pong CA (FBO)
#endif
extern GLint uniform_Master_texture_fs_ClipArt_render[_NbConfigurations];  // Particles
extern GLint uniform_Master_texture_fs_Particle_render[_NbConfigurations];  // Particles
extern GLint uniform_Master_texture_fs_Trk0[_NbConfigurations];  // ping-pong track 0 (FBO)
#if PG_NB_TRACKS >= 2
extern GLint uniform_Master_texture_fs_Trk1[_NbConfigurations];  // ping-pong track 1 (FBO)
#endif
#if PG_NB_TRACKS >= 3
extern GLint uniform_Master_texture_fs_Trk2[_NbConfigurations];  // ping-pong track 2 (FBO)
#endif
#if PG_NB_TRACKS >= 4
extern GLint uniform_Master_texture_fs_Trk3[_NbConfigurations];  // ping-pong track 3 (FBO)
#endif
#ifdef PG_WITH_MASTER_MASK
extern GLint uniform_Master_texture_fs_Mask[_NbConfigurations];  // mask for master output
#endif

#if defined(var_sensor_layout)
  /////////////////////////////////////////////////////////////////////////
// SENSOR SHADER
// SENSOR SHADER UNIFORM VARIABLES
extern GLint uniform_Sensor_vp_model[_NbConfigurations];
extern GLint uniform_Sensor_vp_view[_NbConfigurations];
extern GLint uniform_Sensor_vp_proj[_NbConfigurations];

// SENSOR SHADER TEXTURE IDS
extern GLint uniform_Sensor_texture_fs_decal[_NbConfigurations];         // sensor texture
extern GLint uniform_Sensor_fs_4fv_onOff_isCurrentSensor_masterLevel_transparency[_NbConfigurations];
// extern GLint uniform_Sensor_fs_2fv_frameno_invert[_NbConfigurations];
#endif

#if defined(var_activeMeshes)
/////////////////////////////////////////////////////////////////////////
// MESH SHADER
// MESH SHADER UNIFORM VARIABLES
extern GLint uniform_Mesh_vp_model[_NbConfigurations];
extern GLint uniform_Mesh_vp_view[_NbConfigurations];
extern GLint uniform_Mesh_vp_proj[_NbConfigurations];
extern GLint uniform_Mesh_fs_4fv_isDisplayLookAt_with_mesh_with_blue_currentScene[_NbConfigurations];
extern GLint uniform_Mesh_fs_3fv_light[_NbConfigurations];
#if defined(var_Contact_mesh_expand)
extern GLint uniform_Mesh_vp_2fv_dilate_explode[_NbConfigurations];
#endif
#if defined(var_Contact_mesh_anime)
extern GLint uniform_Mesh_bones_matrices[_NbConfigurations];
#endif
#if defined(var_MmeShanghai_brokenGlass)
extern GLint uniform_Mesh_fs_4fv_color[_NbConfigurations];
extern GLint uniform_Mesh_fs_4fv_color_master_photo_weight_bg[_NbConfigurations];
#endif
// Augmented Reality: FBO capture of Master to be displayed on a mesh
#if defined(var_textureFrontier_wmin) && defined(var_textureFrontier_wmax) and defined(var_textureFrontier_hmin) && defined(var_textureFrontier_hmax) && defined(var_textureFrontier_wmin_width) && defined(var_textureFrontier_wmax_width) and defined(var_textureFrontier_hmin_width) && defined(var_textureFrontier_hmax_width) && defined(var_textureScale_w) && defined(var_textureScale_h) and defined(var_textureTranslate_w) && defined(var_textureTranslate_h) 
extern GLint uniform_Mesh_fs_4fv_textureFrontier[_NbConfigurations];
extern GLint uniform_Mesh_fs_4fv_textureFrontier_width[_NbConfigurations];
extern GLint uniform_Mesh_fs_4fv_textureScaleTransl[_NbConfigurations];
#endif
// Mesh SHADER TEXTURE IDS
extern GLint uniform_Mesh_texture_fs_decal[_NbConfigurations];         // Mesh texture
#if defined(var_Contact_mesh_palette)
extern GLint uniform_Mesh_fs_4fv_color_palette[_NbConfigurations];         // Mesh color
#endif
#if defined(var_MmeShanghai_brokenGlass)
extern GLint uniform_Mesh_texture_fs_BG[_NbConfigurations];         // Mesh texture
#endif
#endif

#endif
