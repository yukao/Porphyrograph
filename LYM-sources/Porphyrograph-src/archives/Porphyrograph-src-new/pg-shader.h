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

#if defined(CORE)
#include "pg_shader_header_Core.h"
#endif
#if defined(pg_Project_Voluspa)
#include "pg_shader_header_voluspa.h"
#endif
#if defined(pg_Project_araKnit)
#include "pg_shader_header_araknit.h"
#endif

// #include "pg_scripts/pg_shader_header.h"

unsigned long pg_getFileLength(ifstream& file);
void pg_printShaderCompileLog(GLuint obj);
void pg_printShaderLinkLog(GLuint obj);
void pg_loadAllShaders(void);

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
// PARTICLE ANIMATION SHADER
// PARTICLE ANIMATION SHADER UNIFORM VARIABLES
extern GLint uniform_ParticleAnimation_vp_model[PG_MAX_CONFIGURATIONS];
extern GLint uniform_ParticleAnimation_vp_view[PG_MAX_CONFIGURATIONS];
extern GLint uniform_ParticleAnimation_vp_proj[PG_MAX_CONFIGURATIONS];
extern GLint uniform_ParticleAnimation_vp_2fv_width_height[PG_MAX_CONFIGURATIONS];
extern GLint uniform_ParticleAnimation_fs_4fv_W_H_repopChannel_targetFrameNo[PG_MAX_CONFIGURATIONS];
extern GLint uniform_ParticleAnimation_fs_4fv_flashTrkPartWghts[PG_MAX_CONFIGURATIONS];
extern GLint uniform_ParticleAnimation_path_data[PG_MAX_CONFIGURATIONS];
extern GLint uniform_ParticleAnimation_fs_4fv_repop_Color_frameNo[PG_MAX_CONFIGURATIONS];
extern GLint uniform_ParticleAnimation_fs_4fv_flashCAPartWght_nbPart_clear_partSizeUnpulsed[PG_MAX_CONFIGURATIONS];
extern GLint uniform_ParticleAnimation_fs_4fv_Camera_W_H_movieWH[PG_MAX_CONFIGURATIONS];
extern GLint uniform_ParticleAnimation_texture_fs_Trk0[PG_MAX_CONFIGURATIONS];  // ping-pong track 0 (FBO)
extern GLint uniform_ParticleAnimation_texture_fs_Trk1[PG_MAX_CONFIGURATIONS];  // ping-pong track 1 (FBO)
extern GLint uniform_ParticleAnimation_texture_fs_Trk2[PG_MAX_CONFIGURATIONS];  // ping-pong track 2 (FBO)
extern GLint uniform_ParticleAnimation_texture_fs_Trk3[PG_MAX_CONFIGURATIONS];  // ping-pong track 3 (FBO)

// UPDATE SHADER TEXTURE IDS
extern GLint uniform_ParticleAnimation_texture_fs_CA[PG_MAX_CONFIGURATIONS];         // ping-pong CA (FBO)
extern GLint uniform_ParticleAnimation_texture_fs_Part_pos_speed[PG_MAX_CONFIGURATIONS];  // 2-cycle ping-pong position/speed of particles (FBO)
extern GLint uniform_ParticleAnimation_texture_fs_Part_col_rad[PG_MAX_CONFIGURATIONS];  // 2-cycle ping-pong color/radius of particles (FBO)
extern GLint uniform_ParticleAnimation_texture_fs_Part_Target_pos_col_rad[PG_MAX_CONFIGURATIONS];  // 2-cycle ping-pong target position/color/radius of particles (FBO)
extern GLint uniform_ParticleAnimation_texture_fs_Camera_frame[PG_MAX_CONFIGURATIONS];  // camera frame
extern GLint uniform_ParticleAnimation_texture_fs_Movie_frame[PG_MAX_CONFIGURATIONS];  // movie frame
extern GLint uniform_ParticleAnimation_texture_fs_Noise[PG_MAX_CONFIGURATIONS];  // 3D noise
extern GLint uniform_ParticleAnimation_texture_fs_RepopDensity[PG_MAX_CONFIGURATIONS];  // repop density
extern GLint uniform_ParticleAnimation_texture_fs_Part_init_pos_speed[PG_MAX_CONFIGURATIONS];  // particle initialization pairs of textures position/speed
extern GLint uniform_ParticleAnimation_texture_fs_Part_init_col_rad[PG_MAX_CONFIGURATIONS];  // particle initialization pairs of textures color/radius
extern GLint uniform_ParticleAnimation_texture_fs_Part_acc[PG_MAX_CONFIGURATIONS];  // particle acceleration texture

/////////////////////////////////////////////////////////////////////////
// UPDATE SHADER
// UPDATE SHADER UNIFORM VARIABLES
extern GLint uniform_Update_vp_model[PG_MAX_CONFIGURATIONS];
extern GLint uniform_Update_vp_view[PG_MAX_CONFIGURATIONS];
extern GLint uniform_Update_vp_proj[PG_MAX_CONFIGURATIONS];
extern GLint uniform_Update_homographyForTexture[PG_MAX_CONFIGURATIONS];
extern GLint uniform_Update_vp_2fv_width_height[PG_MAX_CONFIGURATIONS];
extern GLint uniform_Update_fs_4fv_W_H_time_currentScene[PG_MAX_CONFIGURATIONS];
extern GLint uniform_Update_fs_3fv_clearAllLayers_clearCA_pulsedShift[PG_MAX_CONFIGURATIONS];
#if defined(pg_Project_CAaudio)
extern GLint uniform_Update_fs_4fv_CAseed_type_size_loc[PG_MAX_CONFIGURATIONS];
#endif
extern GLint uniform_Update_fs_4fv_flashTrkBGWghts_flashPartBGWght[PG_MAX_CONFIGURATIONS];
extern GLint uniform_Update_fs_4fv_frameno_Cursor_flashPartCAWght_doubleWindow[PG_MAX_CONFIGURATIONS];
extern GLint uniform_Update_fs_4fv_flashTrkCAWghts[PG_MAX_CONFIGURATIONS];
extern GLint uniform_Update_path_data[PG_MAX_CONFIGURATIONS];

// pg_Project_Criton PROJECT
extern GLint uniform_Update_fs_4fv_fftLevels03[PG_MAX_CONFIGURATIONS];
extern GLint uniform_Update_fs_4fv_fftFrequencies03[PG_MAX_CONFIGURATIONS];
extern GLint uniform_Update_fs_4fv_fftPhases03[PG_MAX_CONFIGURATIONS];
extern GLint uniform_Update_fs_4fv_fftLevels47[PG_MAX_CONFIGURATIONS];
extern GLint uniform_Update_fs_4fv_fftFrequencies47[PG_MAX_CONFIGURATIONS];
extern GLint uniform_Update_fs_4fv_fftPhases47[PG_MAX_CONFIGURATIONS];

extern GLint uniform_Update_fs_4fv_movieWH_flashCameraTrkWght_cpTrack[PG_MAX_CONFIGURATIONS];
extern GLint uniform_Update_fs_4fv_repop_ColorBG_flashCABGWght[PG_MAX_CONFIGURATIONS];
extern GLint uniform_Update_fs_3fv_repop_ColorCA[PG_MAX_CONFIGURATIONS];
extern GLint uniform_Update_fs_3fv_isClearLayer_flashPixel_flashCameraTrkThres[PG_MAX_CONFIGURATIONS];
extern GLint uniform_Update_fs_4fv_flashPhotoTrkWght_flashPhotoTrkThres_Photo_offSetsXY[PG_MAX_CONFIGURATIONS];
extern GLint uniform_Update_fs_4fv_photo01_wh[PG_MAX_CONFIGURATIONS];
extern GLint uniform_Update_fs_4fv_photo01Wghts_randomValues[PG_MAX_CONFIGURATIONS];
#endif
extern GLint uniform_Update_fs_2fv_clip01Wghts[PG_MAX_CONFIGURATIONS];
extern GLint uniform_Update_fs_4fv_Camera_offSetsXY_Camera_W_H[PG_MAX_CONFIGURATIONS];
extern GLint uniform_Update_fs_4fv_xy_transl_tracks_0_1[PG_MAX_CONFIGURATIONS];
extern GLint uniform_Update_fs_4fv_CAType_SubType_blurRadius[PG_MAX_CONFIGURATIONS];
extern GLint uniform_Update_fs_3fv_photo_rgb[PG_MAX_CONFIGURATIONS];
extern GLint uniform_Update_texture_fs_CATable[PG_MAX_CONFIGURATIONS];
#if defined(var_GenerativeNights_planes)
extern GLint uniform_Update_fs_2fv_initCA_1stPlaneFrameNo[PG_MAX_CONFIGURATIONS];
#endif
extern GLint uniform_Update_texture_fs_Camera_BGIni[PG_MAX_CONFIGURATIONS];
extern GLint uniform_Update_texture_fs_pixel_acc[PG_MAX_CONFIGURATIONS];

// UPDATE SHADER TEXTURE IDS
extern GLint uniform_Update_texture_fs_CA[PG_MAX_CONFIGURATIONS];         // ping-pong CA (FBO)
extern GLint uniform_Update_texture_fs_PreviousCA[PG_MAX_CONFIGURATIONS];         // ping-pong CA (FBO)
extern GLint uniform_Update_texture_fs_Pixels[PG_MAX_CONFIGURATIONS];  // 2-cycle ping-pong speed/position of pixels (FBO)
extern GLint uniform_Update_texture_fs_Trk0[PG_MAX_CONFIGURATIONS];  // ping-pong track 0 (FBO)
extern GLint uniform_Update_texture_fs_Trk1[PG_MAX_CONFIGURATIONS];  // ping-pong track 1 (FBO)
extern GLint uniform_Update_texture_fs_Trk2[PG_MAX_CONFIGURATIONS];  // ping-pong track 2 (FBO)
extern GLint uniform_Update_texture_fs_Trk3[PG_MAX_CONFIGURATIONS];  // ping-pong track 3 (FBO)
extern GLint uniform_Update_texture_fs_Brushes[PG_MAX_CONFIGURATIONS];  // pen patterns
extern GLint uniform_Update_texture_fs_Camera_frame[PG_MAX_CONFIGURATIONS];  // camera frame
extern GLint uniform_Update_texture_fs_Camera_BG[PG_MAX_CONFIGURATIONS];  // camera BG capture
extern GLint uniform_Update_texture_fs_Movie_frame[PG_MAX_CONFIGURATIONS];  // movie frame
extern GLint uniform_Update_texture_fs_Noise[PG_MAX_CONFIGURATIONS];  // 3D noise
extern GLint uniform_Update_texture_fs_RepopDensity[PG_MAX_CONFIGURATIONS];  // repop density
extern GLint uniform_Update_texture_fs_Photo0[PG_MAX_CONFIGURATIONS];  // photo[0]
extern GLint uniform_Update_texture_fs_Photo1[PG_MAX_CONFIGURATIONS];  // photo[1]
extern GLint uniform_Update_texture_fs_Clip0[PG_MAX_CONFIGURATIONS];  // clip[0]
extern GLint uniform_Update_texture_fs_Clip1[PG_MAX_CONFIGURATIONS];  // clip[1]
extern GLint uniform_Update_texture_fs_Part_render[PG_MAX_CONFIGURATIONS];  // FBO capture of particle rendering used for flashing layers with particles
extern GLint uniform_Update_texture_fs_Part_init_pos_speed[PG_MAX_CONFIGURATIONS];  // particle initialization pairs of textures position/speed
extern GLint uniform_Update_texture_fs_Part_init_col_rad[PG_MAX_CONFIGURATIONS];  // particle initialization pairs of textures color/radius

/////////////////////////////////////////////////////////////////////////
// PARTICLE RENDERING SHADER
// PARTICLE RENDERING SHADER UNIFORM VARIABLES
extern GLint uniform_ParticleSplat_vp_model[PG_MAX_CONFIGURATIONS];
extern GLint uniform_ParticleSplat_vp_view[PG_MAX_CONFIGURATIONS];
extern GLint uniform_ParticleSplat_vp_proj[PG_MAX_CONFIGURATIONS];
extern GLint uniform_ParticleSplat_gs_4fv_part_size_partType_highPitchPulse_windowRatio[PG_MAX_CONFIGURATIONS];
extern GLint uniform_ParticleSplat_vp_3fv_trackReplay_xy_height[PG_MAX_CONFIGURATIONS];

// PARTICLE SHADER TEXTURE IDS
extern GLint uniform_ParticleSplat_texture_vp_Part_pos_speed[PG_MAX_CONFIGURATIONS];				 // Particle position/speed update
extern GLint uniform_ParticleSplat_texture_vp_Part_col_rad[PG_MAX_CONFIGURATIONS];          // Particle color/radius update

// CURVE PARTICLES
extern GLint uniform_ParticleCurve_vp_model[PG_MAX_CONFIGURATIONS];
extern GLint uniform_ParticleCurve_vp_view[PG_MAX_CONFIGURATIONS];
extern GLint uniform_ParticleCurve_vp_proj[PG_MAX_CONFIGURATIONS];
extern GLint uniform_ParticleCurve_gs_3fv_partRadius_partType_highPitchPulse[PG_MAX_CONFIGURATIONS];
extern GLint uniform_ParticleCurve_vp_3fv_trackReplay_xy_height[PG_MAX_CONFIGURATIONS];

// PARTICLE SHADER TEXTURE IDS
extern GLint uniform_ParticleCurve_texture_vp_Part_pos_speed[PG_MAX_CONFIGURATIONS];				 // Particle position/speed update
extern GLint uniform_ParticleCurve_texture_vp_Part_col_rad[PG_MAX_CONFIGURATIONS];          // Particle color/radius update

// color texture
extern GLint uniform_ParticleCurve_Curve_texture_fs_decal[PG_MAX_CONFIGURATIONS];  // comet texture

// color texture
extern GLint uniform_ParticleSplat_texture_fs_decal[PG_MAX_CONFIGURATIONS];  // blurred disk texture

/////////////////////////////////////////////////////////////////////////
// MIXING SHADER
// MIXING SHADER UNIFORM VARIABLES
extern GLint uniform_Mixing_vp_model[PG_MAX_CONFIGURATIONS];
extern GLint uniform_Mixing_vp_view[PG_MAX_CONFIGURATIONS];
extern GLint uniform_Mixing_vp_proj[PG_MAX_CONFIGURATIONS];
extern GLint uniform_Mixing_fs_3fv_height_flashCameraTrkWght_flashPhotoTrkWght[PG_MAX_CONFIGURATIONS];
extern GLint uniform_Mixing_fs_3fv_screenMsgTransp_Text1_2_Alpha[PG_MAX_CONFIGURATIONS];

// MIXING SHADER TEXTURE IDS
extern GLint uniform_Mixing_texture_fs_CA[PG_MAX_CONFIGURATIONS]; // ping-pong BG track (FBO)
extern GLint uniform_Mixing_texture_fs_ClipArt_render[PG_MAX_CONFIGURATIONS];  // Particles (FBO)
extern GLint uniform_Mixing_texture_fs_Particle_render[PG_MAX_CONFIGURATIONS];  // Particles (FBO)
extern GLint uniform_Mixing_texture_fs_Trk0[PG_MAX_CONFIGURATIONS];  // ping-pong track 0 (FBO)
extern GLint uniform_Mixing_texture_fs_Trk1[PG_MAX_CONFIGURATIONS];  // ping-pong track 1 (FBO)
extern GLint uniform_Mixing_texture_fs_Trk2[PG_MAX_CONFIGURATIONS];  // ping-pong track 2 (FBO)
extern GLint uniform_Mixing_texture_fs_Trk3[PG_MAX_CONFIGURATIONS];  // ping-pong track 3 (FBO)
extern GLint uniform_Mixing_texture_fs_Render_prec[PG_MAX_CONFIGURATIONS];  // preceding snapshot
extern GLint uniform_Mixing_texture_fs_Screen_Font[PG_MAX_CONFIGURATIONS];  // message Font
extern GLint uniform_Mixing_texture_fs_Screen_Message[PG_MAX_CONFIGURATIONS];  // message string

/////////////////////////////////////////////////////////////////////////
// MASTER SHADER
// MASTER SHADER UNIFORM VARIABLES
extern GLint uniform_Master_vp_model[PG_MAX_CONFIGURATIONS];
extern GLint uniform_Master_vp_view[PG_MAX_CONFIGURATIONS];
extern GLint uniform_Master_vp_proj[PG_MAX_CONFIGURATIONS];
extern GLint uniform_Master_fs_4fv_xy_frameno_pulsedShift[PG_MAX_CONFIGURATIONS];
extern GLint uniform_Master_fs_4fv_width_height_timeFromStart_muteRightScreen[PG_MAX_CONFIGURATIONS];
//extern GLint uniform_Master_fs_4fv_pulsedColor_rgb_pen_grey[PG_MAX_CONFIGURATIONS];
//extern GLint uniform_Master_fs_4fv_interpolatedPaletteLow_rgb_currentScene[PG_MAX_CONFIGURATIONS];
//extern GLint uniform_Master_fs_4fv_interpolatedPaletteMedium_rgb_mobile_cursor[PG_MAX_CONFIGURATIONS];
//extern GLint uniform_Master_fs_3fv_interpolatedPaletteHigh_rgb[PG_MAX_CONFIGURATIONS];
extern GLint uniform_Master_fs_2iv_mobile_cursor_currentScene[PG_MAX_CONFIGURATIONS];
extern GLint uniform_Master_fs_3fv_BGcolor_rgb[PG_MAX_CONFIGURATIONS];

// MASTER SHADER TEXTURE IDS
extern GLint uniform_Master_texture_fs_Render_curr[PG_MAX_CONFIGURATIONS];         // previous pass output
extern GLint uniform_Master_texture_fs_CA[PG_MAX_CONFIGURATIONS];  // ping-pong CA (FBO)
extern GLint uniform_Master_texture_fs_ClipArt_render[PG_MAX_CONFIGURATIONS];  // Particles
extern GLint uniform_Master_texture_fs_Particle_render[PG_MAX_CONFIGURATIONS];  // Particles
extern GLint uniform_Master_texture_fs_Trk0[PG_MAX_CONFIGURATIONS];  // ping-pong track 0 (FBO)
extern GLint uniform_Master_texture_fs_Trk1[PG_MAX_CONFIGURATIONS];  // ping-pong track 1 (FBO)
extern GLint uniform_Master_texture_fs_Trk2[PG_MAX_CONFIGURATIONS];  // ping-pong track 2 (FBO)
extern GLint uniform_Master_texture_fs_Trk3[PG_MAX_CONFIGURATIONS];  // ping-pong track 3 (FBO)
extern GLint uniform_Master_texture_fs_Mask[PG_MAX_CONFIGURATIONS];  // mask for master output

  /////////////////////////////////////////////////////////////////////////
// SENSOR SHADER
// SENSOR SHADER UNIFORM VARIABLES
extern GLint uniform_Sensor_vp_model[PG_MAX_CONFIGURATIONS];
extern GLint uniform_Sensor_vp_view[PG_MAX_CONFIGURATIONS];
extern GLint uniform_Sensor_vp_proj[PG_MAX_CONFIGURATIONS];

// SENSOR SHADER TEXTURE IDS
extern GLint uniform_Sensor_texture_fs_decal[PG_MAX_CONFIGURATIONS];         // sensor texture
extern GLint uniform_Sensor_fs_4fv_onOff_isCurrentSensor_masterLevel_transparency[PG_MAX_CONFIGURATIONS];

/////////////////////////////////////////////////////////////////////////
// MESH SHADER
// MESH SHADER UNIFORM VARIABLES
extern GLint uniform_Mesh_vp_model[PG_MAX_CONFIGURATIONS];
extern GLint uniform_Mesh_vp_view[PG_MAX_CONFIGURATIONS];
extern GLint uniform_Mesh_vp_proj[PG_MAX_CONFIGURATIONS];
extern GLint uniform_Mesh_fs_4fv_isDisplayLookAt_with_mesh_with_blue_currentScene[PG_MAX_CONFIGURATIONS];
extern GLint uniform_Mesh_fs_3fv_light[PG_MAX_CONFIGURATIONS];
#if defined(var_Contact_mesh_expand)
extern GLint uniform_Mesh_vp_2fv_dilate_explode[PG_MAX_CONFIGURATIONS];
#endif
#if defined(var_Contact_mesh_anime)
extern GLint uniform_Mesh_bones_matrices[PG_MAX_CONFIGURATIONS];
#endif
#if defined(var_MmeShanghai_brokenGlass)
extern GLint uniform_Mesh_fs_4fv_color[PG_MAX_CONFIGURATIONS];
extern GLint uniform_Mesh_fs_4fv_color_master_photo_weight_bg[PG_MAX_CONFIGURATIONS];
#endif
// Augmented Reality: FBO capture of Master to be displayed on a mesh
extern GLint uniform_Mesh_fs_4fv_textureFrontier[PG_MAX_CONFIGURATIONS];
extern GLint uniform_Mesh_fs_4fv_textureFrontier_width[PG_MAX_CONFIGURATIONS];
extern GLint uniform_Mesh_fs_4fv_textureScaleTransl[PG_MAX_CONFIGURATIONS];
// Mesh SHADER TEXTURE IDS
extern GLint uniform_Mesh_texture_fs_decal[PG_MAX_CONFIGURATIONS];         // Mesh texture
#if defined(var_Contact_mesh_palette)
extern GLint uniform_Mesh_fs_4fv_color_palette[PG_MAX_CONFIGURATIONS];         // Mesh color
#endif
#if defined(var_MmeShanghai_brokenGlass)
extern GLint uniform_Mesh_texture_fs_BG[PG_MAX_CONFIGURATIONS];         // Mesh texture
#endif