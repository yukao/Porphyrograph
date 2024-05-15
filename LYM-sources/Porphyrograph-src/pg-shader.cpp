/*! \file pg-shader.cpp
 * 
 *     Part of this source file is taken from Virtual Choreographer
 *     http://virchor.sourceforge.net/
 * 
 *     File pg-shader.cpp
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
#ifdef TVW
#include "pg_shader_body_decl_TVW.cpp"
#endif
#ifdef CRITON
#include "pg_shader_body_decl_Criton.cpp"
#endif
#if defined(KOMPARTSD)
#include "pg_shader_body_decl_KompartSD.cpp"
#endif
#if defined(LIGHT)
#include "pg_shader_body_decl_Light.cpp"
#endif
#if defined(CORE)
#include "pg_shader_body_decl_Core.cpp"
#endif
#if defined(VOLUSPA)
#include "pg_shader_body_decl_voluspa.cpp"
#endif
#if defined(ARAKNIT)
#include "pg_shader_body_decl_araknit.cpp"
#endif
#if defined(CAAUDIO)
#include "pg_shader_body_decl_CAaudio.cpp"
#endif

// #include "pg_scripts/pg_shader_body_decl.cpp"

/////////////////////////////////////////////////////////////////////////
// SHADER PROGRAMMES
unsigned int** pg_shader_programme;
unsigned int pg_current_configuration_rank = 0;
std::unordered_map<int, std::string> pg_stringShaderTypes = {
	{ _pg_shader_ParticleAnimation, "ParticleAnimation" },
	{ _pg_shader_Update, "Update" },
	{ _pg_shader_ParticleRender, "ParticleRender" },
	{ _pg_shader_Mixing, "Mixing" },
	{ _pg_shader_Master, "Master" },
	{ _pg_shader_Sensor, "Sensor" },
	{ _pg_shader_ClipArt, "ClipArt" },
	{ _pg_shader_Mesh, "Mesh" },
	{ _pg_shader_TotalNbTypes, "TotalNbTypes" },
};


// PARTICLE ANIMATION SHADER
// PARTICLE ANIMATION SHADER UNIFORM VARIABLES
GLint uniform_ParticleAnimation_vp_model[_NbConfigurations] = {-1};
GLint uniform_ParticleAnimation_vp_view[_NbConfigurations] = {-1};
GLint uniform_ParticleAnimation_vp_proj[_NbConfigurations] = {-1};
GLint uniform_ParticleAnimation_vp_2fv_width_height[_NbConfigurations] = {-1};
GLint uniform_ParticleAnimation_fs_4fv_W_H_repopChannel_targetFrameNo[_NbConfigurations] = {-1};
GLint uniform_ParticleAnimation_fs_4fv_flashTrkPartWghts[_NbConfigurations] = {-1};
GLint uniform_ParticleAnimation_path_data[_NbConfigurations] = {-1};
GLint uniform_ParticleAnimation_fs_4fv_repop_Color_frameNo[_NbConfigurations] = {-1};
GLint uniform_ParticleAnimation_fs_4fv_flashCAPartWght_nbPart_clear_partSizeUnpulsed[_NbConfigurations] = {-1};
GLint uniform_ParticleAnimation_fs_4fv_Camera_W_H_movieWH[_NbConfigurations] = {-1};

// UPDATE SHADER TEXTURE IDS
GLint uniform_ParticleAnimation_texture_fs_CA[_NbConfigurations] = {-1};         // ping-pong CA (FBO)
GLint uniform_ParticleAnimation_texture_fs_Part_pos_speed[_NbConfigurations] = {-1};  // 2-cycle ping-pong position/speed of particles (FBO)
GLint uniform_ParticleAnimation_texture_fs_Part_col_rad[_NbConfigurations] = {-1};  // 2-cycle ping-pong color/radius of particles (FBO)
GLint uniform_ParticleAnimation_texture_fs_Part_Target_pos_col_rad[_NbConfigurations] = {-1};  // 2-cycle ping-pong target position/color/radius of particles (FBO)
GLint uniform_ParticleAnimation_texture_fs_Camera_frame[_NbConfigurations] = {-1};  // camera frame
GLint uniform_ParticleAnimation_texture_fs_Movie_frame[_NbConfigurations] = {-1};  // movie frame
GLint uniform_ParticleAnimation_texture_fs_Noise[_NbConfigurations] = {-1};  // 3D noise
GLint uniform_ParticleAnimation_texture_fs_RepopDensity[_NbConfigurations] = {-1};  // repop density
GLint uniform_ParticleAnimation_texture_fs_Part_init_pos_speed[_NbConfigurations] = {-1};  // particle initialization pairs of textures position/speed
GLint uniform_ParticleAnimation_texture_fs_Part_init_col_rad[_NbConfigurations] = {-1};  // particle initialization pairs of textures color/radius
GLint uniform_ParticleAnimation_texture_fs_Part_acc[_NbConfigurations] = {-1};  // particle acceleration texture
GLint uniform_ParticleAnimation_texture_fs_Trk0[_NbConfigurations] = {-1};  // ping-pong track 0 (FBO)
#if PG_NB_TRACKS >= 2
GLint uniform_ParticleAnimation_texture_fs_Trk1[_NbConfigurations] = {-1};  // ping-pong track 1 (FBO)
#endif
#if PG_NB_TRACKS >= 3
GLint uniform_ParticleAnimation_texture_fs_Trk2[_NbConfigurations] = {-1};  // ping-pong track 2 (FBO)
#endif
#if PG_NB_TRACKS >= 4
GLint uniform_ParticleAnimation_texture_fs_Trk3[_NbConfigurations] = {-1};  // ping-pong track 3 (FBO)
#endif

/////////////////////////////////////////////////////////////////////////
// UPDATE SHADER
// UPDATE SHADER UNIFORM VARIABLES
GLint uniform_Update_vp_model[_NbConfigurations] = {-1};
GLint uniform_Update_vp_view[_NbConfigurations] = {-1};
GLint uniform_Update_vp_proj[_NbConfigurations] = {-1};
#if defined(PG_WITH_PHOTO_HOMOGRAPHY)
GLint uniform_Update_homographyForTexture[_NbConfigurations] = {-1};
#endif
GLint uniform_Update_vp_2fv_width_height[_NbConfigurations] = {-1};
GLint uniform_Update_fs_4fv_W_H_time_currentScene[_NbConfigurations] = {-1};
GLint uniform_Update_fs_3fv_clearAllLayers_clearCA_pulsedShift[_NbConfigurations] = {-1};
#if defined(CAAUDIO)
GLint uniform_Update_fs_4fv_CAseed_type_size_loc[_NbConfigurations] = {-1};
#endif
GLint uniform_Update_fs_4fv_flashTrkBGWghts_flashPartBGWght[_NbConfigurations] = {-1};
GLint uniform_Update_fs_4fv_frameno_Cursor_flashPartCAWght_doubleWindow[_NbConfigurations] = {-1};
#if defined(var_nb_CATypes)
GLint uniform_Update_fs_4fv_flashTrkCAWghts[_NbConfigurations] = {-1};
#endif
GLint uniform_Update_path_data[_NbConfigurations] = {-1};

#ifdef CRITON
GLint uniform_Update_fs_4fv_fftLevels03[_NbConfigurations] = {-1};
GLint uniform_Update_fs_4fv_fftFrequencies03[_NbConfigurations] = {-1};
GLint uniform_Update_fs_4fv_fftPhases03[_NbConfigurations] = {-1};
GLint uniform_Update_fs_4fv_fftLevels47[_NbConfigurations] = {-1};
GLint uniform_Update_fs_4fv_fftFrequencies47[_NbConfigurations] = {-1};
GLint uniform_Update_fs_4fv_fftPhases47[_NbConfigurations] = {-1};
#endif
GLint uniform_Update_fs_4fv_movieWH_flashCameraTrkWght_cpTrack[_NbConfigurations] = {-1};
GLint uniform_Update_fs_4fv_repop_ColorBG_flashCABGWght[_NbConfigurations] = {-1};
#if defined(var_nb_CATypes)
GLint uniform_Update_fs_3fv_repop_ColorCA[_NbConfigurations] = {-1};
#endif
GLint uniform_Update_fs_3fv_isClearLayer_flashPixel_flashCameraTrkThres[_NbConfigurations] = {-1};
#if defined(var_flashPhotoTrkBeat) && defined(var_flashPhotoTrkBright) && defined(var_flashPhotoTrkLength) && defined(var_flashPhotoChangeBeat)
GLint uniform_Update_fs_4fv_flashPhotoTrkWght_flashPhotoTrkThres_Photo_offSetsXY[_NbConfigurations] = {-1};
#endif
#if defined(var_photo_diaporama) || defined(TVW)
GLint uniform_Update_fs_4fv_photo01_wh[_NbConfigurations] = {-1};
#endif
#if defined(var_photo_diaporama)
GLint uniform_Update_fs_4fv_photo01Wghts_randomValues[_NbConfigurations] = {-1};
#endif
#if defined(var_clipCaptFreq) && PG_NB_PARALLEL_CLIPS >= 2
GLint uniform_Update_fs_2fv_clip01Wghts[_NbConfigurations] = {-1};
#endif
#if defined(var_cameraCaptFreq)
GLint uniform_Update_fs_4fv_Camera_offSetsXY_Camera_W_H[_NbConfigurations] = {-1};
#endif
GLint uniform_Update_fs_4fv_xy_transl_tracks_0_1[_NbConfigurations] = {-1};
#if defined(var_nb_CATypes) || defined(PG_WITH_BLUR)
GLint uniform_Update_fs_4fv_CAType_SubType_blurRadius[_NbConfigurations] = {-1};
#endif
#if defined(var_clipCaptFreq)
GLint uniform_Update_fs_3fv_photo_rgb[_NbConfigurations] = {-1};
#endif
#if defined(var_CATable)
GLint uniform_Update_texture_fs_CATable[_NbConfigurations] = {-1};
#endif
#if defined(var_GenerativeNights_planes)
GLint uniform_Update_fs_2fv_initCA_1stPlaneFrameNo[_NbConfigurations] = {-1};
#endif
#if defined(var_camera_BG_subtr)
GLint uniform_Update_texture_fs_Camera_BGIni[_NbConfigurations] = {-1};  // initial camera BG capture
#endif
GLint uniform_Update_texture_fs_pixel_acc[_NbConfigurations] = {-1};

// UPDATE SHADER TEXTURE IDS
#if defined(var_nb_CATypes)
GLint uniform_Update_texture_fs_CA[_NbConfigurations] = {-1};         // ping-pong CA (FBO)
#endif
GLint uniform_Update_texture_fs_PreviousCA[_NbConfigurations] = {-1};         // ping-pong CA (FBO)
GLint uniform_Update_texture_fs_Pixels[_NbConfigurations] = {-1};  // 2-cycle ping-pong speed/position of pixels (FBO)
GLint uniform_Update_texture_fs_Trk0[_NbConfigurations] = {-1};  // ping-pong track 0 (FBO)
#if PG_NB_TRACKS >= 2
GLint uniform_Update_texture_fs_Trk1[_NbConfigurations] = {-1};  // ping-pong track 1 (FBO)
#endif
#if PG_NB_TRACKS >= 3
GLint uniform_Update_texture_fs_Trk2[_NbConfigurations] = {-1};  // ping-pong track 2 (FBO)
#endif
#if PG_NB_TRACKS >= 4
GLint uniform_Update_texture_fs_Trk3[_NbConfigurations] = {-1};  // ping-pong track 3 (FBO)
#endif
#if !defined(PG_BEZIER_PATHS) || defined(CORE)
GLint uniform_Update_texture_fs_Brushes[_NbConfigurations] = {-1};  // pen patterns
#endif
GLint uniform_Update_texture_fs_Camera_frame[_NbConfigurations] = {-1};  // camera frame
GLint uniform_Update_texture_fs_Camera_BG[_NbConfigurations] = {-1};  // current camera BG capture
GLint uniform_Update_texture_fs_Movie_frame[_NbConfigurations] = {-1};  // movie frame
GLint uniform_Update_texture_fs_Noise[_NbConfigurations] = {-1};  // 3D noise
#if defined(var_BG_CA_repop_density)
GLint uniform_Update_texture_fs_RepopDensity[_NbConfigurations] = {-1};  // repop density
#endif
GLint uniform_Update_texture_fs_Photo0[_NbConfigurations] = {-1};  // photo[0]
GLint uniform_Update_texture_fs_Photo1[_NbConfigurations] = {-1};  // photo[1]
GLint uniform_Update_texture_fs_Clip0[_NbConfigurations] = {-1};  // clip[0]
GLint uniform_Update_texture_fs_Clip1[_NbConfigurations] = {-1};  // clip[1]
#if defined(var_part_initialization) 
GLint uniform_Update_texture_fs_Part_render[_NbConfigurations] = {-1};  // FBO capture of particle rendering used for flashing layers with particles
#endif
#if defined(TVW)
GLint uniform_Update_fs_4fv_weights03[_NbConfigurations] = {-1};
GLint uniform_Update_fs_2fv_weights45[_NbConfigurations] = {-1};
GLint uniform_Update_fs_3fv_alphaSwap02[_NbConfigurations] = {-1};
GLint uniform_Update_fs_3fv_alphaSwap35[_NbConfigurations] = {-1};
GLint uniform_Update_fs_4fv_image_noisesxy[_NbConfigurations] = {-1};
GLint uniform_Update_fs_4fv_mask_noisesxy[_NbConfigurations] = {-1};

GLint uniform_Update_texture_fs_TVWPixels0[_NbConfigurations] = {-1};  // image buffer #0
GLint uniform_Update_texture_fs_TVWPixels1[_NbConfigurations] = {-1};  // image buffer #1
GLint uniform_Update_texture_fs_TVWPixels2[_NbConfigurations] = {-1};  // image buffer #2
GLint uniform_Update_texture_fs_TVWPixels3[_NbConfigurations] = {-1};  // image buffer #3
GLint uniform_Update_texture_fs_TVWPixels4[_NbConfigurations] = {-1};  // image buffer #4
GLint uniform_Update_texture_fs_TVWPixels5[_NbConfigurations] = {-1};  // image buffer #5
GLint uniform_Update_texture_fs_TVWMask02[_NbConfigurations] = {-1};  // image mask #0
GLint uniform_Update_texture_fs_TVWMask35[_NbConfigurations] = {-1};  // image mask #1
GLint uniform_Update_texture_fs_TVWPixelsSwap0[_NbConfigurations] = {-1};  // buffer swap image #0
GLint uniform_Update_texture_fs_TVWPixelsSwap1[_NbConfigurations] = {-1};  // buffer swap image #1 
GLint uniform_Update_texture_fs_TVWPixelsSwap2[_NbConfigurations] = {-1};  // buffer swap image #2 
GLint uniform_Update_texture_fs_TVWPixelsSwap3[_NbConfigurations] = {-1};  // buffer swap image #3 
GLint uniform_Update_texture_fs_TVWPixelsSwap4[_NbConfigurations] = {-1};  // buffer swap image #4 
GLint uniform_Update_texture_fs_TVWPixelsSwap5[_NbConfigurations] = {-1};  // buffer swap image #5 
#endif

/////////////////////////////////////////////////////////////////////////
// PARTICLE SHADER
// PARTICLE SHADER UNIFORM VARIABLES
#if defined(var_part_initialization) 
GLint uniform_ParticleSplat_vp_model[_NbConfigurations] = {-1};
GLint uniform_ParticleSplat_vp_view[_NbConfigurations] = {-1};
GLint uniform_ParticleSplat_vp_proj[_NbConfigurations] = {-1};
GLint uniform_ParticleSplat_gs_4fv_part_size_partType_highPitchPulse_windowRatio[_NbConfigurations] = {-1};
GLint uniform_ParticleSplat_vp_3fv_trackReplay_xy_height[_NbConfigurations] = {-1};

// PARTICLE SHADER TEXTURE IDS
GLint uniform_ParticleSplat_texture_vp_Part_pos_speed[_NbConfigurations] = {-1};				 // Particle position/speed update
GLint uniform_ParticleSplat_texture_vp_Part_col_rad[_NbConfigurations] = {-1};          // Particle color/radius update
#endif
#if defined(CURVE_PARTICLES) 
GLint uniform_ParticleCurve_vp_model[_NbConfigurations] = {-1};
GLint uniform_ParticleCurve_vp_view[_NbConfigurations] = {-1};
GLint uniform_ParticleCurve_vp_proj[_NbConfigurations] = {-1};
GLint uniform_ParticleCurve_gs_3fv_partRadius_partType_highPitchPulse[_NbConfigurations] = {-1};
GLint uniform_ParticleCurve_vp_3fv_trackReplay_xy_height[_NbConfigurations] = {-1};

// PARTICLE SHADER TEXTURE IDS
GLint uniform_ParticleCurve_texture_vp_Part_pos_speed[_NbConfigurations] = {-1};				 // Particle position/speed update
GLint uniform_ParticleCurve_texture_vp_Part_col_rad[_NbConfigurations] = {-1};          // Particle color/radius update
#endif
#ifdef CURVE_PARTICLES
// color texture
GLint uniform_ParticleCurve_Comet_texture_fs_decal[_NbConfigurations] = {-1};  // comet texture
#endif
#if defined(TEXTURED_QUAD_PARTICLES)
// color texture
GLint uniform_ParticleSplat_texture_fs_decal[_NbConfigurations] = {-1};  // blurred disk texture
#endif

/////////////////////////////////////////////////////////////////////////
// MIXING SHADER
// MIXING SHADER UNIFORM VARIABLES
GLint uniform_Mixing_vp_model[_NbConfigurations] = {-1};
GLint uniform_Mixing_vp_view[_NbConfigurations] = {-1};
GLint uniform_Mixing_vp_proj[_NbConfigurations] = {-1};
GLint uniform_Mixing_fs_3fv_height_flashCameraTrkWght_flashPhotoTrkWght[_NbConfigurations] = {-1};
GLint uniform_Mixing_fs_3fv_screenMsgTransp_Text1_2_Alpha[_NbConfigurations] = {-1};

// MIXING SHADER TEXTURE IDS
#if defined(var_nb_CATypes)
GLint uniform_Mixing_texture_fs_CA[_NbConfigurations] = {-1};         // ping-pong CA (FBO)
#endif
GLint uniform_Mixing_texture_fs_ClipArt_render[_NbConfigurations] = { -1 };  // ClipArt (FBO)
GLint uniform_Mixing_texture_fs_Particle_render[_NbConfigurations] = { -1 };  // Particles (FBO)
GLint uniform_Mixing_texture_fs_Trk0[_NbConfigurations] = {-1};  // ping-pong track 0 (FBO)
#if PG_NB_TRACKS >= 2
GLint uniform_Mixing_texture_fs_Trk1[_NbConfigurations] = {-1};  // ping-pong track 1 (FBO)
#endif
#if PG_NB_TRACKS >= 3
GLint uniform_Mixing_texture_fs_Trk2[_NbConfigurations] = {-1};  // ping-pong track 2 (FBO)
#endif
#if PG_NB_TRACKS >= 4
GLint uniform_Mixing_texture_fs_Trk3[_NbConfigurations] = {-1};  // ping-pong track 3 (FBO)
#endif
GLint uniform_Mixing_texture_fs_Render_prec[_NbConfigurations] = {-1};  // preceding snapshot
GLint uniform_Mixing_texture_fs_Screen_Font[_NbConfigurations] = {-1};  // message Font
GLint uniform_Mixing_texture_fs_Screen_Message[_NbConfigurations] = {-1};  // message string
#if defined(TVW)
GLint uniform_Mixing_texture_fs_Display_Font[_NbConfigurations] = {-1};  // display Font
GLint uniform_Mixing_texture_fs_Display_Message1[_NbConfigurations] = {-1}; // tweets at the bottom of the screen
GLint uniform_Mixing_texture_fs_Display_Message2[_NbConfigurations] = {-1}; // tweets at the bottom of the screen
#endif

/////////////////////////////////////////////////////////////////////////
// MASTER SHADER
// MASTER SHADER UNIFORM VARIABLES
GLint uniform_Master_vp_model[_NbConfigurations] = {-1};
GLint uniform_Master_vp_view[_NbConfigurations] = {-1};
GLint uniform_Master_vp_proj[_NbConfigurations] = {-1};
GLint uniform_Master_fs_4fv_xy_frameno_pulsedShift[_NbConfigurations] = {-1};
#if defined(TVW)
GLint uniform_Master_fs_4fv_width_height_rightWindowVMargin_timeFromStart[_NbConfigurations] = {-1};
#endif
GLint uniform_Master_fs_4fv_width_height_timeFromStart_muteRightScreen[_NbConfigurations] = {-1};
//GLint uniform_Master_fs_4fv_pulsedColor_rgb_pen_grey[_NbConfigurations] = {-1};
//GLint uniform_Master_fs_4fv_interpolatedPaletteLow_rgb_currentScene[_NbConfigurations] = {-1};
//GLint uniform_Master_fs_4fv_interpolatedPaletteMedium_rgb_mobile_cursor[_NbConfigurations] = {-1};
//GLint uniform_Master_fs_3fv_interpolatedPaletteHigh_rgb[_NbConfigurations] = {-1};
GLint uniform_Master_fs_2iv_mobile_cursor_currentScene[_NbConfigurations] = {-1};
#if defined(var_Caverne_BackColor)
GLint uniform_Master_fs_3fv_Caverne_BackColor_rgb[_NbConfigurations] = {-1};
#endif
#if defined(var_flashchange_BGcolor_freq)
GLint uniform_Master_fs_3fv_BG_color_rgb[_NbConfigurations] = {-1};
#endif

// MASTER SHADER TEXTURE IDS
GLint uniform_Master_texture_fs_Render_curr[_NbConfigurations] = {-1};         // previous pass output
#if defined(var_nb_CATypes)
GLint uniform_Master_texture_fs_CA[_NbConfigurations] = {-1};  // ping-pong CA (FBO)
#endif
GLint uniform_Master_texture_fs_ClipArt_render[_NbConfigurations] = { -1 };  // Particles
GLint uniform_Master_texture_fs_Particle_render[_NbConfigurations] = { -1 };  // Particles
GLint uniform_Master_texture_fs_Trk0[_NbConfigurations] = {-1};  // ping-pong track 0 (FBO)
#if PG_NB_TRACKS >= 2
GLint uniform_Master_texture_fs_Trk1[_NbConfigurations] = {-1};  // ping-pong track 1 (FBO)
#endif
#if PG_NB_TRACKS >= 3
GLint uniform_Master_texture_fs_Trk2[_NbConfigurations] = {-1};  // ping-pong track 2 (FBO)
#endif
#if PG_NB_TRACKS >= 4
GLint uniform_Master_texture_fs_Trk3[_NbConfigurations] = {-1};  // ping-pong track 3 (FBO)
#endif
#ifdef PG_WITH_MASTER_MASK
GLint uniform_Master_texture_fs_Mask[_NbConfigurations] = {-1};  // mask for master output
#endif

#if defined(var_sensor_layout)
/////////////////////////////////////////////////////////////////////////
// SENSOR SHADER
// SENSOR SHADER UNIFORM VARIABLES
GLint uniform_Sensor_vp_model[_NbConfigurations] = {-1};
GLint uniform_Sensor_vp_view[_NbConfigurations] = {-1};
GLint uniform_Sensor_vp_proj[_NbConfigurations] = {-1};

// SENSOR SHADER TEXTURE IDS
GLint uniform_Sensor_texture_fs_decal[_NbConfigurations] = {-1};         // sensor texture
GLint uniform_Sensor_fs_4fv_onOff_isCurrentSensor_masterLevel_transparency[_NbConfigurations] = {-1};
// GLint uniform_Sensor_fs_2fv_frameno_invert[_NbConfigurations] = {-1};
#endif

#if defined(var_activeMeshes)
/////////////////////////////////////////////////////////////////////////
// MESH SHADER
// MESH SHADER UNIFORM VARIABLES
GLint uniform_Mesh_vp_model[_NbConfigurations] = {-1};
GLint uniform_Mesh_vp_view[_NbConfigurations] = {-1};
GLint uniform_Mesh_vp_proj[_NbConfigurations] = {-1};
GLint uniform_Mesh_fs_4fv_isDisplayLookAt_with_mesh_with_blue_currentScene[_NbConfigurations] = {-1};
GLint uniform_Mesh_fs_3fv_light[_NbConfigurations] = {-1};
#if defined(var_Contact_mesh_expand)
GLint uniform_Mesh_vp_2fv_dilate_explode[_NbConfigurations] = {-1};
#endif
#if defined(var_Contact_mesh_anime)
GLint uniform_Mesh_bones_matrices[_NbConfigurations] = {-1};
#endif
#if defined(var_MmeShanghai_brokenGlass)
GLint uniform_Mesh_fs_4fv_color[_NbConfigurations] = {-1};
GLint uniform_Mesh_fs_4fv_color_master_photo_weight_bg[_NbConfigurations] = {-1};
#endif
// Augmented Reality: FBO capture of Master to be displayed on a mesh
#if defined(var_textureFrontier_wmin) && defined(var_textureFrontier_wmax) and defined(var_textureFrontier_hmin) && defined(var_textureFrontier_hmax) && defined(var_textureFrontier_wmin_width) && defined(var_textureFrontier_wmax_width) and defined(var_textureFrontier_hmin_width) && defined(var_textureFrontier_hmax_width) && defined(var_textureScale_w) && defined(var_textureScale_h) and defined(var_textureTranslate_w) && defined(var_textureTranslate_h) 
GLint uniform_Mesh_fs_4fv_textureFrontier[_NbConfigurations] = {-1};
GLint uniform_Mesh_fs_4fv_textureFrontier_width[_NbConfigurations] = {-1};
GLint uniform_Mesh_fs_4fv_textureScaleTransl[_NbConfigurations] = {-1};
#endif
// Mesh SHADER TEXTURE IDS
GLint uniform_Mesh_texture_fs_decal[_NbConfigurations] = {-1};         // Mesh texture
#if defined(var_Contact_mesh_palette)
GLint uniform_Mesh_fs_4fv_color_palette[_NbConfigurations] = {-1};         // Mesh color
#endif
#if defined(var_MmeShanghai_brokenGlass)
GLint uniform_Mesh_texture_fs_BG[_NbConfigurations] = {-1};         // Mesh texture
#endif
#endif


//////////////////////////////////////////////////////////////
// UTILITIES

unsigned long pg_getFileLength(ifstream& file) {
    if(!file.good()) return 0;
    
    file.seekg(0,ios::end);
    unsigned long len = (long)file.tellg();
    file.seekg(ios::beg);
    
    return len;
}

void pg_printShaderCompileLog(GLuint obj) {
	int infologLength = 0;
	int maxLength;

	if (glIsShader(obj)) {
		glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &maxLength);
	}
	else {
		glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &maxLength);
	}

	char *infoLog = new char[maxLength];

	if (glIsShader(obj)) {
		glGetShaderInfoLog(obj, maxLength, &infologLength, infoLog);
	}
	else {
		glGetProgramInfoLog(obj, maxLength, &infologLength, infoLog);
	}

	if (infologLength > 0) {
		printf("Shader compilation failed:\n%s\n", infoLog);  throw 4301;
	}

	delete[] infoLog;
}

void pg_printShaderLinkLog(GLuint obj) {
	int infologLength = 0;
	GLint linked;
	if (glIsShader(obj)) {
		glGetShaderiv(obj, GL_LINK_STATUS, &linked);
	}
	else {
		glGetProgramiv(obj, GL_LINK_STATUS, &linked);
	}

	GLsizei maxLength;
	if (glIsShader(obj)) {
		glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &maxLength);
	}
	else {
		glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &maxLength);
	}

	GLchar* infoLog = new GLchar[maxLength + 1];
	glGetProgramInfoLog(obj, maxLength, &maxLength, infoLog);
	if (infologLength > 0) {
		printf("Shader linking failed:\n%s\n", infoLog); throw 4302;
	}

	delete[] infoLog;
}



/////////////////////////////////////////////////////////////
// SHADER LOADING
int pg_loadshader(string filename, GLuint shader) {
	GLchar* shaderSource;
	unsigned long len;

	ifstream file;
	file.open(filename.c_str(), ios::in); // opens as ASCII!
	if (!file) {
		printf("Error: shader file not found %s!\n", filename.c_str()); throw 53;
	}

	len = pg_getFileLength(file);
	if (len == 0) {
		printf("Error: empty shader file %s!\n", filename.c_str()); throw 53;
	}

	shaderSource = new char[len + 1];
	if (shaderSource == 0) {
		printf("Error: shader string allocation error %s!\n", filename.c_str()); throw 53;
	}

	// len isn't always strlen cause some characters are stripped in ascii read...
	// it is important to 0-terminate the real length later, len is just max possible value... 
	shaderSource[len] = 0;

	unsigned int i = 0;
	while (file.good()) {
		shaderSource[i] = GLchar(file.get());       // get character from file.
		if (!file.eof())
			i++;
	}

	shaderSource[i] = 0;  // 0-terminate it at the correct position

	file.close();

	glShaderSource(shader, 1, (const char**)&shaderSource, NULL);

	delete[] shaderSource;

	return 0; // No Error
}

void pg_allocateBindAndCheckUniform(int configuration_rank, GLint uniformID[_NbConfigurations], string uniformString, ShaderFileTypes shaderType) {
	if (configuration_rank < _NbConfigurations) {
		if (pg_shader_programme[configuration_rank][shaderType]) {
			glUseProgram(pg_shader_programme[configuration_rank][shaderType]);
			uniformID[configuration_rank] = glGetUniformLocation(pg_shader_programme[configuration_rank][shaderType], uniformString.c_str());
		}
		else {
			sprintf(ErrorStr, "Error: inactive shader [%s] configuration %d for binding %s file %s!", pg_stringShaderTypes[shaderType].c_str(), configuration_rank, uniformString.c_str(), pg_Shader_File_Names[configuration_rank][shaderType].c_str()); ReportError(ErrorStr); throw 430;
		}
	}
	else {
		sprintf(ErrorStr, "Error: unknown shader type [%s]!", pg_stringShaderTypes[shaderType].c_str()); ReportError(ErrorStr); throw 430;
	}
	printOglError(332);
	if (uniformID[configuration_rank] == -1) {
		std::cout << uniformString << " (" << pg_stringShaderTypes[shaderType] << ") ";
		//sprintf(ErrorStr, "Error: Could not bind uniform [%s] (ID: %d) in shader type %s configuration #%d shader name %s!", uniformString.c_str(), int(uniformID[configuration_rank]), pg_stringShaderTypes[shaderType].c_str(), configuration_rank, pg_Shader_File_Names[configuration_rank][shaderType].c_str()); ReportError(ErrorStr);
	}
}

void pg_loadAllShaders(void) {
	std::cout << "\nLoading shaders: " << std::endl;
	////////////////////////////////////////
	// loading and compiling shaders
	for (int indConfig = 0; indConfig < _NbConfigurations; indConfig++) {
		std::cout << "   " << indConfig << ": ";
		for (int shader_type = 0; shader_type < _pg_shader_TotalNbTypes; shader_type++) {
			if (pg_Shader_File_Names[indConfig][shader_type] != "NULL") {
				pg_shader_programme[indConfig][shader_type] = glCreateProgram();
				unsigned int *pg_shader = new unsigned int[pg_Shader_nbStages[indConfig][shader_type]];
				string fileShaderName = "";
				std::cout << pg_Shader_File_Names[indConfig][shader_type] + " (";
				for (int shader_stage = 0; shader_stage < pg_Shader_nbStages[indConfig][shader_type]; shader_stage++) {
					//printf("shader conf %d indshader %d name %s stage %d ID %d\n", indConfig, shader_type, fileShaderName.c_str(), pg_Shader_Stages[indConfig][shader_type][shader_stage], pg_shader_programme[indConfig][shader_type]);
					switch (pg_Shader_Stages[indConfig][shader_type][shader_stage]) {
					case GL_VERTEX_SHADER:
						fileShaderName = pg_Shader_File_Names[indConfig][shader_type] + ".vert";
						std::cout << "vert, ";
						break;
					case GL_TESS_CONTROL_SHADER:
						fileShaderName = pg_Shader_File_Names[indConfig][shader_type] + ".tess";
						std::cout << "tess, ";
						break;
					case GL_TESS_EVALUATION_SHADER:
						fileShaderName = pg_Shader_File_Names[indConfig][shader_type] + ".eval";
						std::cout << "eval, ";
						break;
					case GL_GEOMETRY_SHADER:
						fileShaderName = pg_Shader_File_Names[indConfig][shader_type] + ".geom";
						std::cout << "geom, ";
						break;
					case GL_FRAGMENT_SHADER:
						if (shader_type != _pg_shader_Sensor && shader_type != _pg_shader_ClipArt && shader_type != _pg_shader_Mesh) {
							fileShaderName = pg_Shader_File_Names[indConfig][shader_type] + "_full_" + std::to_string(indConfig) + ".frag";
							std::cout << "_full_" + std::to_string(indConfig) + ".frag, ";
						}
						else {
							fileShaderName = pg_Shader_File_Names[indConfig][shader_type] + "_full.frag";
							std::cout << "frag, ";
						}
						break;
					default:
						fileShaderName = "";
						sprintf(ErrorStr, "Error: unknown shader type [%d]!", pg_Shader_Stages[indConfig][shader_type][shader_stage]); ReportError(ErrorStr); throw 430;
						break;
					}

					pg_shader[shader_stage] = glCreateShader(pg_Shader_Stages[indConfig][shader_type][shader_stage]);
					pg_loadshader((char*)("Projects/shaders/" + fileShaderName).c_str(), pg_shader[shader_stage]);
					glCompileShader(pg_shader[shader_stage]);
					pg_printShaderCompileLog(pg_shader[shader_stage]);

					glAttachShader(pg_shader_programme[indConfig][shader_type], pg_shader[shader_stage]);
					printOglError(330);
				}
				std::cout << "), ";

				glLinkProgram(pg_shader_programme[indConfig][shader_type]);
				pg_printShaderLinkLog(pg_shader_programme[indConfig][shader_type]);
				printOglError(331);
				
				// detach afeter inking
				for (int shader_stage = 0; shader_stage < pg_Shader_nbStages[indConfig][shader_type]; shader_stage++) {
					glDetachShader(pg_shader_programme[indConfig][shader_type], pg_shader[shader_stage]);
				}
				// memory release
				delete [] pg_shader;
			}
			else {
				pg_shader_programme[indConfig][shader_type] = 0;
			}
		}
		std::cout << std::endl;
	}

	////////////////////////////////////////
	// binding and allocating tables in shaders

#ifdef TVW
#include "pg_shader_body_bind_TVW.cpp"
#endif
#ifdef CRITON
#include "pg_shader_body_bind_Criton.cpp"
#endif
#if defined(KOMPARTSD)
#include "pg_shader_body_bind_KompartSD.cpp"
#endif
#if defined(KOMPARTSD)
#include "pg_shader_body_bind_Light.cpp"
#endif
#if defined(CORE)
#include "pg_shader_body_bind_Core.cpp"
#endif
#if defined(VOLUSPA)
#include "pg_shader_body_bind_voluspa.cpp"
#endif
#if defined(ARAKNIT)
#include "pg_shader_body_bind_araknit.cpp"
#endif
#if defined(CAAUDIO)
#include "pg_shader_body_bind_CAaudio.cpp"
#endif

// #include "pg_scripts/pg_shader_body_bind.cpp"


	////////////////////////////////////////
	// binding variables in shaders
	std::cout << "Unbound uniforms: " << std::endl;
	for (int indConfig = 0; indConfig < _NbConfigurations; indConfig++) {
		std::cout << "   " << indConfig << ": ";
#if defined(var_part_initialization) 
		if (ScenarioVarConfigurations[_part_initialization][indConfig]) {
			////////////////////////////////////////////////////////////////////////////////
			// PARTICLE ANIMATION SHADER parameters bindings
			////////////////////////////////////////////////////////////////////////////////
			pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleAnimation_vp_model, "vp_modelMatrix", _pg_shader_ParticleAnimation);
			pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleAnimation_vp_view, "vp_viewMatrix", _pg_shader_ParticleAnimation);
			pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleAnimation_vp_proj, "vp_projMatrix", _pg_shader_ParticleAnimation);

			pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleAnimation_vp_2fv_width_height, "uniform_ParticleAnimation_vp_2fv_width_height", _pg_shader_ParticleAnimation);
			pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleAnimation_fs_4fv_W_H_repopChannel_targetFrameNo, "uniform_ParticleAnimation_fs_4fv_W_H_repopChannel_targetFrameNo", _pg_shader_ParticleAnimation);
			pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleAnimation_fs_4fv_flashTrkPartWghts, "uniform_ParticleAnimation_fs_4fv_flashTrkPartWghts", _pg_shader_ParticleAnimation);
			pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleAnimation_path_data, "uniform_ParticleAnimation_path_data", _pg_shader_ParticleAnimation);
			pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleAnimation_fs_4fv_repop_Color_frameNo, "uniform_ParticleAnimation_fs_4fv_repop_Color_frameNo", _pg_shader_ParticleAnimation);
			pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleAnimation_fs_4fv_flashCAPartWght_nbPart_clear_partSizeUnpulsed, "uniform_ParticleAnimation_fs_4fv_flashCAPartWght_nbPart_clear_partSizeUnpulsed", _pg_shader_ParticleAnimation);
			pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleAnimation_fs_4fv_Camera_W_H_movieWH, "uniform_ParticleAnimation_fs_4fv_Camera_W_H_movieWH", _pg_shader_ParticleAnimation);

			// BG track FBO
			pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleAnimation_texture_fs_CA, "uniform_ParticleAnimation_texture_fs_CA", _pg_shader_ParticleAnimation);  // 2-cycle ping-pong position/speed of particles (FBO)
			pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleAnimation_texture_fs_Part_pos_speed, "uniform_ParticleAnimation_texture_fs_Part_pos_speed", _pg_shader_ParticleAnimation);  // 2-cycle ping-pong position/speed of particles (FBO)
			pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleAnimation_texture_fs_Part_col_rad, "uniform_ParticleAnimation_texture_fs_Part_col_rad", _pg_shader_ParticleAnimation);  // 2-cycle ping-pong color/radius of particles (FBO)
			pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleAnimation_texture_fs_Part_Target_pos_col_rad, "uniform_ParticleAnimation_texture_fs_Part_Target_pos_col_rad", _pg_shader_ParticleAnimation);  // 2-cycle ping-pong target position/color/radius of particles (FBO)
			pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleAnimation_texture_fs_Trk0, "uniform_ParticleAnimation_texture_fs_Trk0", _pg_shader_ParticleAnimation);  // ping-pong track 0 (FBO)
#if PG_NB_TRACKS >= 2
			pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleAnimation_texture_fs_Trk1, "uniform_ParticleAnimation_texture_fs_Trk1", _pg_shader_ParticleAnimation);  // ping-pong track 1 (FBO)
#endif
#if PG_NB_TRACKS >= 3
			pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleAnimation_texture_fs_Trk2, "uniform_ParticleAnimation_texture_fs_Trk2", _pg_shader_ParticleAnimation);  // ping-pong track 2 (FBO)
#endif
#if PG_NB_TRACKS >= 4
			pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleAnimation_texture_fs_Trk3, "uniform_ParticleAnimation_texture_fs_Trk3", _pg_shader_ParticleAnimation);  // ping-pong track 3 (FBO)
#endif

			///////////////////////////////////////////////////////////////////////////
			// textures

			// video frames
			pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleAnimation_texture_fs_Camera_frame, "uniform_ParticleAnimation_texture_fs_Camera_frame", _pg_shader_ParticleAnimation);  // camera frame
			pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleAnimation_texture_fs_Movie_frame, "uniform_ParticleAnimation_texture_fs_Movie_frame", _pg_shader_ParticleAnimation);  // movie frame

			pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleAnimation_texture_fs_Noise, "uniform_ParticleAnimation_texture_fs_Noise", _pg_shader_ParticleAnimation);  // 3D noise

			pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleAnimation_texture_fs_RepopDensity, "uniform_ParticleAnimation_texture_fs_RepopDensity", _pg_shader_ParticleAnimation);  // repop density
			pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleAnimation_texture_fs_Part_init_pos_speed, "uniform_ParticleAnimation_texture_fs_Part_init_pos_speed", _pg_shader_ParticleAnimation);  // particle initialization pairs of textures position/speed
			pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleAnimation_texture_fs_Part_init_col_rad, "uniform_ParticleAnimation_texture_fs_Part_init_col_rad", _pg_shader_ParticleAnimation);  // particle initialization pairs of textures color/radius
			pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleAnimation_texture_fs_Part_acc, "uniform_ParticleAnimation_texture_fs_Part_acc", _pg_shader_ParticleAnimation);  // particle acceleration texture
		}
#endif

		////////////////////////////////////////////////////////////////////////////////
		// UPDATE SHADER parameters bindings
		////////////////////////////////////////////////////////////////////////////////
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_vp_model, "vp_modelMatrix", _pg_shader_Update);
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_vp_view, "vp_viewMatrix", _pg_shader_Update);
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_vp_proj, "vp_projMatrix", _pg_shader_Update);
#if defined(PG_WITH_PHOTO_HOMOGRAPHY)
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_homographyForTexture, "fs_homographyForTexture", _pg_shader_Update);
#endif

		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_vp_2fv_width_height, "uniform_Update_vp_2fv_width_height", _pg_shader_Update);
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_fs_4fv_W_H_time_currentScene, "uniform_Update_fs_4fv_W_H_time_currentScene", _pg_shader_Update);
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_fs_3fv_clearAllLayers_clearCA_pulsedShift, "uniform_Update_fs_3fv_clearAllLayers_clearCA_pulsedShift", _pg_shader_Update);
#if defined(CAAUDIO)
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_fs_4fv_CAseed_type_size_loc, "uniform_Update_fs_4fv_CAseed_type_size_loc", _pg_shader_Update);
#endif
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_fs_4fv_flashTrkBGWghts_flashPartBGWght, "uniform_Update_fs_4fv_flashTrkBGWghts_flashPartBGWght", _pg_shader_Update);
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_fs_4fv_frameno_Cursor_flashPartCAWght_doubleWindow, "uniform_Update_fs_4fv_frameno_Cursor_flashPartCAWght_doubleWindow", _pg_shader_Update);
#if defined(var_nb_CATypes)
		if (ScenarioVarConfigurations[_nb_CATypes][indConfig]) {
			pg_allocateBindAndCheckUniform(indConfig, uniform_Update_fs_4fv_flashTrkCAWghts, "uniform_Update_fs_4fv_flashTrkCAWghts", _pg_shader_Update);
		}
#endif
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_path_data, "uniform_Update_path_data", _pg_shader_Update);
#ifdef CRITON
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_fs_4fv_fftLevels03, "uniform_Update_fs_4fv_fftLevels03", _pg_shader_Update);
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_fs_4fv_fftFrequencies03, "uniform_Update_fs_4fv_fftFrequencies03", _pg_shader_Update);
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_fs_4fv_fftPhases03, "uniform_Update_fs_4fv_fftPhases03", _pg_shader_Update);
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_fs_4fv_fftLevels47, "uniform_Update_fs_4fv_fftLevels47", _pg_shader_Update);
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_fs_4fv_fftFrequencies47, "uniform_Update_fs_4fv_fftFrequencies47", _pg_shader_Update);
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_fs_4fv_fftPhases47, "uniform_Update_fs_4fv_fftPhases47", _pg_shader_Update);
#endif
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_fs_4fv_movieWH_flashCameraTrkWght_cpTrack, "uniform_Update_fs_4fv_movieWH_flashCameraTrkWght_cpTrack", _pg_shader_Update);
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_fs_4fv_repop_ColorBG_flashCABGWght, "uniform_Update_fs_4fv_repop_ColorBG_flashCABGWght", _pg_shader_Update);
#if defined(var_nb_CATypes)
		if (ScenarioVarConfigurations[_nb_CATypes][indConfig]) {
			pg_allocateBindAndCheckUniform(indConfig, uniform_Update_fs_3fv_repop_ColorCA, "uniform_Update_fs_3fv_repop_ColorCA", _pg_shader_Update);
		}
#endif
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_fs_3fv_isClearLayer_flashPixel_flashCameraTrkThres, "uniform_Update_fs_3fv_isClearLayer_flashPixel_flashCameraTrkThres", _pg_shader_Update);
#if defined(var_flashPhotoTrkBeat) && defined(var_flashPhotoTrkBright)  && defined(var_flashPhotoTrkLength)  && defined(var_flashPhotoChangeBeat)
		if (ScenarioVarConfigurations[_flashPhotoTrkBeat][indConfig]
			&& ScenarioVarConfigurations[_flashPhotoTrkBright][indConfig]
			&& ScenarioVarConfigurations[_flashPhotoTrkLength][indConfig]
			&& ScenarioVarConfigurations[_flashPhotoChangeBeat][indConfig]) {
			pg_allocateBindAndCheckUniform(indConfig, uniform_Update_fs_4fv_flashPhotoTrkWght_flashPhotoTrkThres_Photo_offSetsXY, "uniform_Update_fs_4fv_flashPhotoTrkWght_flashPhotoTrkThres_Photo_offSetsXY", _pg_shader_Update);
		}
#endif
#if defined(var_photo_diaporama) || defined(TVW)
		if (ScenarioVarConfigurations[_photo_diaporama][indConfig]) {
			pg_allocateBindAndCheckUniform(indConfig, uniform_Update_fs_4fv_photo01_wh, "uniform_Update_fs_4fv_photo01_wh", _pg_shader_Update);
		}
#endif
#if defined(var_photo_diaporama)
		if (ScenarioVarConfigurations[_photo_diaporama][indConfig]) {
			pg_allocateBindAndCheckUniform(indConfig, uniform_Update_fs_4fv_photo01Wghts_randomValues, "uniform_Update_fs_4fv_photo01Wghts_randomValues", _pg_shader_Update);
		}
#endif
#if defined(var_clipCaptFreq) && PG_NB_PARALLEL_CLIPS >= 2
		if (ScenarioVarConfigurations[_clipCaptFreq][indConfig]) {
			pg_allocateBindAndCheckUniform(indConfig, uniform_Update_fs_2fv_clip01Wghts, "uniform_Update_fs_2fv_clip01Wghts", _pg_shader_Update);
		}
#endif
#if defined(var_cameraCaptFreq)
		if (ScenarioVarConfigurations[_cameraCaptFreq][indConfig]) {
			pg_allocateBindAndCheckUniform(indConfig, uniform_Update_fs_4fv_Camera_offSetsXY_Camera_W_H, "uniform_Update_fs_4fv_Camera_offSetsXY_Camera_W_H", _pg_shader_Update);
		}
#endif
#if defined(var_clipCaptFreq)
		if (ScenarioVarConfigurations[_clipCaptFreq][indConfig]) {
			pg_allocateBindAndCheckUniform(indConfig, uniform_Update_fs_3fv_photo_rgb, "uniform_Update_fs_3fv_photo_rgb", _pg_shader_Update);
		}
#endif
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_fs_4fv_xy_transl_tracks_0_1, "uniform_Update_fs_4fv_xy_transl_tracks_0_1", _pg_shader_Update);
#if defined(var_nb_CATypes)	|| defined(PG_WITH_BLUR)
		if (ScenarioVarConfigurations[_nb_CATypes][indConfig]) {
			pg_allocateBindAndCheckUniform(indConfig, uniform_Update_fs_4fv_CAType_SubType_blurRadius, "uniform_Update_fs_4fv_CAType_SubType_blurRadius", _pg_shader_Update);
			//printf("allocate uniform_Update_fs_4fv_CAType_SubType_blurRadius config %d\n", indConfig);
		}
#endif
#if defined(var_CATable)
		if (ScenarioVarConfigurations[_CATable][indConfig]) {
			pg_allocateBindAndCheckUniform(indConfig, uniform_Update_texture_fs_CATable, "uniform_Update_texture_fs_CATable", _pg_shader_Update);
		}
#endif
#if defined(var_GenerativeNights_planes)
		if (ScenarioVarConfigurations[_GenerativeNights_planes][indConfig]) {
			pg_allocateBindAndCheckUniform(indConfig, uniform_Update_fs_2fv_initCA_1stPlaneFrameNo, "uniform_Update_fs_2fv_initCA_1stPlaneFrameNo", _pg_shader_Update);
		}
#endif
#if defined(var_camera_BG_subtr)
		if (ScenarioVarConfigurations[_camera_BG_subtr][indConfig]) {
			pg_allocateBindAndCheckUniform(indConfig, uniform_Update_texture_fs_Camera_BGIni, "uniform_Update_texture_fs_Camera_BGIni", _pg_shader_Update);  // initial background frame
		}
#endif
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_texture_fs_pixel_acc, "uniform_Update_texture_fs_pixel_acc", _pg_shader_Update);  // initial background frame

		// BG track FBO
#if defined(var_nb_CATypes)
		if (ScenarioVarConfigurations[_nb_CATypes][indConfig]) {
			pg_allocateBindAndCheckUniform(indConfig, uniform_Update_texture_fs_CA, "uniform_Update_texture_fs_CA", _pg_shader_Update);         // 2-cycle ping-pong CA (FBO)
		}
#endif
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_texture_fs_PreviousCA, "uniform_Update_texture_fs_PreviousCA", _pg_shader_Update);         // 2-cycle ping-pong CA (FBO)
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_texture_fs_Pixels, "uniform_Update_texture_fs_Pixels", _pg_shader_Update);  // 2-cycle ping-pong speed/position of pixels (FBO)
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_texture_fs_Trk0, "uniform_Update_texture_fs_Trk0", _pg_shader_Update);  // ping-pong track 0 (FBO)
#if PG_NB_TRACKS >= 2
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_texture_fs_Trk1, "uniform_Update_texture_fs_Trk1", _pg_shader_Update);  // ping-pong track 1 (FBO)
#endif
#if PG_NB_TRACKS >= 3
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_texture_fs_Trk2, "uniform_Update_texture_fs_Trk2", _pg_shader_Update);  // ping-pong track 2 (FBO)
#endif
#if PG_NB_TRACKS >= 4
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_texture_fs_Trk3, "uniform_Update_texture_fs_Trk3", _pg_shader_Update);  // ping-pong track 3 (FBO)
#endif
		// textures
#if !defined(PG_BEZIER_PATHS) || defined(CORE)
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_texture_fs_Brushes, "uniform_Update_texture_fs_Brushes", _pg_shader_Update);  // pen patterns
#endif

		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_texture_fs_Camera_frame, "uniform_Update_texture_fs_Camera_frame", _pg_shader_Update);  // camera frame
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_texture_fs_Camera_BG, "uniform_Update_texture_fs_Camera_BG", _pg_shader_Update);  // current background frame
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_texture_fs_Movie_frame, "uniform_Update_texture_fs_Movie_frame", _pg_shader_Update);  // movie frame
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_texture_fs_Noise, "uniform_Update_texture_fs_Noise", _pg_shader_Update);  // 3D noise
#if defined(var_BG_CA_repop_density)
		if (ScenarioVarConfigurations[_BG_CA_repop_density][indConfig]) {
			pg_allocateBindAndCheckUniform(indConfig, uniform_Update_texture_fs_RepopDensity, "uniform_Update_texture_fs_RepopDensity", _pg_shader_Update);  // repop density
		}
#endif

		// photo frames
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_texture_fs_Photo0, "uniform_Update_texture_fs_Photo0", _pg_shader_Update);  // photo[0] frame
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_texture_fs_Photo1, "uniform_Update_texture_fs_Photo1", _pg_shader_Update);  // photo[1] frame
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_texture_fs_Clip0, "uniform_Update_texture_fs_Clip0", _pg_shader_Update);  // clip[0]
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_texture_fs_Clip1, "uniform_Update_texture_fs_Clip1", _pg_shader_Update);  // clip[1]
#if defined(var_part_initialization) 
		if (ScenarioVarConfigurations[_part_initialization][indConfig]) {
			pg_allocateBindAndCheckUniform(indConfig, uniform_Update_texture_fs_Part_render, "uniform_Update_texture_fs_Part_render", _pg_shader_Update);  // particle rendering capture
		}
#endif
#if defined(TVW)
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_fs_4fv_weights03, "uniform_Update_fs_4fv_weights03", _pg_shader_Update);
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_fs_2fv_weights45, "uniform_Update_fs_2fv_weights45", _pg_shader_Update);
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_fs_3fv_alphaSwap02, "uniform_Update_fs_3fv_alphaSwap02", _pg_shader_Update);
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_fs_3fv_alphaSwap35, "uniform_Update_fs_3fv_alphaSwap35", _pg_shader_Update);
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_fs_4fv_image_noisesxy, "uniform_Update_fs_4fv_image_noisesxy", _pg_shader_Update);
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_fs_4fv_mask_noisesxy, "uniform_Update_fs_4fv_mask_noisesxy", _pg_shader_Update);

		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_texture_fs_TVWPixels0, "uniform_Update_texture_fs_TVWPixels0", _pg_shader_Update);  // image buffer #0
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_texture_fs_TVWPixels1, "uniform_Update_texture_fs_TVWPixels1", _pg_shader_Update);  // image buffer #1
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_texture_fs_TVWPixels2, "uniform_Update_texture_fs_TVWPixels2", _pg_shader_Update);  // image buffer #2
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_texture_fs_TVWPixels3, "uniform_Update_texture_fs_TVWPixels3", _pg_shader_Update);  // image buffer #3
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_texture_fs_TVWPixels4, "uniform_Update_texture_fs_TVWPixels4", _pg_shader_Update);  // image buffer #4
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_texture_fs_TVWPixels5, "uniform_Update_texture_fs_TVWPixels5", _pg_shader_Update);  // image buffer #5
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_texture_fs_TVWMask02, "uniform_Update_texture_fs_TVWMask02", _pg_shader_Update);  // image mask #0
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_texture_fs_TVWMask35, "uniform_Update_texture_fs_TVWMask35", _pg_shader_Update);  // image mask #1
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_texture_fs_TVWPixelsSwap0, "uniform_Update_texture_fs_TVWPixelsSwap0", _pg_shader_Update);  // buffer swap image #0
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_texture_fs_TVWPixelsSwap1, "uniform_Update_texture_fs_TVWPixelsSwap1", _pg_shader_Update);  // buffer swap image #1
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_texture_fs_TVWPixelsSwap2, "uniform_Update_texture_fs_TVWPixelsSwap2", _pg_shader_Update);  // buffer swap image #2
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_texture_fs_TVWPixelsSwap3, "uniform_Update_texture_fs_TVWPixelsSwap3", _pg_shader_Update);  // buffer swap image #3
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_texture_fs_TVWPixelsSwap4, "uniform_Update_texture_fs_TVWPixelsSwap4", _pg_shader_Update);  // buffer swap image #4
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_texture_fs_TVWPixelsSwap5, "uniform_Update_texture_fs_TVWPixelsSwap5", _pg_shader_Update);  // buffer swap image #5
#endif



		////////////////////////////////////////////////////////////////////////////////
		// PARTICLE RENDERING SHADER parameters bindings
		////////////////////////////////////////////////////////////////////////////////
#if defined(var_part_initialization) 
		if (ScenarioVarConfigurations[_part_initialization][indConfig]) {
			pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleSplat_vp_model, "vp_modelMatrix", _pg_shader_ParticleRender);
			pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleSplat_vp_view, "vp_viewMatrix", _pg_shader_ParticleRender);
			pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleSplat_vp_proj, "vp_projMatrix", _pg_shader_ParticleRender);

			pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleSplat_gs_4fv_part_size_partType_highPitchPulse_windowRatio, "uniform_ParticleSplat_gs_4fv_part_size_partType_highPitchPulse_windowRatio", _pg_shader_ParticleRender);
			pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleSplat_vp_3fv_trackReplay_xy_height, "uniform_ParticleSplat_vp_3fv_trackReplay_xy_height", _pg_shader_ParticleRender);

			// particle position/speed FBO
			pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleSplat_texture_vp_Part_pos_speed, "uniform_ParticleSplat_texture_vp_Part_pos_speed", _pg_shader_ParticleRender);
			// particle color/radius FBO
			pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleSplat_texture_vp_Part_col_rad, "uniform_ParticleSplat_texture_vp_Part_col_rad", _pg_shader_ParticleRender);
		}
#endif
#if defined(CURVE_PARTICLES) 
		pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleCurve_vp_model, "vp_modelMatrix", _pg_shader_ParticleRender);
		pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleCurve_vp_view, "vp_viewMatrix", _pg_shader_ParticleRender);
		pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleCurve_vp_proj, "vp_projMatrix", _pg_shader_ParticleRender);

		pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleCurve_gs_3fv_partRadius_partType_highPitchPulse, "uniform_ParticleCurve_gs_3fv_partRadius_partType_highPitchPulse", _pg_shader_ParticleRender);
		pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleCurve_vp_3fv_trackReplay_xy_height, "uniform_ParticleCurve_vp_3fv_trackReplay_xy_height", _pg_shader_ParticleRender);

		// particle position/speed FBO
		pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleCurve_texture_vp_Part_pos_speed, "uniform_ParticleCurve_texture_vp_Part_pos_speed", _pg_shader_ParticleRender);
		// particle color/radius FBO
		pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleCurve_texture_vp_Part_col_rad, "uniform_ParticleCurve_texture_vp_Part_col_rad", _pg_shader_ParticleRender);
#endif

#ifdef CURVE_PARTICLES
		// color texture
		pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleCurve_Comet_texture_fs_decal, "fs_comet", _pg_shader_ParticleRender);
#endif
#if defined(var_part_initialization) 
		if (ScenarioVarConfigurations[_part_initialization][indConfig]) {
			// color texture
			pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleSplat_texture_fs_decal, "uniform_ParticleSplat_texture_fs_decal", _pg_shader_ParticleRender);
		}
#endif

		////////////////////////////////////////////////////////////////////////////////
		// MIXING SHADER parameters bindings
		////////////////////////////////////////////////////////////////////////////////
		pg_allocateBindAndCheckUniform(indConfig, uniform_Mixing_vp_model, "vp_modelMatrix", _pg_shader_Mixing);
		pg_allocateBindAndCheckUniform(indConfig, uniform_Mixing_vp_view, "vp_viewMatrix", _pg_shader_Mixing);
		pg_allocateBindAndCheckUniform(indConfig, uniform_Mixing_vp_proj, "vp_projMatrix", _pg_shader_Mixing);

		pg_allocateBindAndCheckUniform(indConfig, uniform_Mixing_fs_3fv_height_flashCameraTrkWght_flashPhotoTrkWght, "uniform_Mixing_fs_3fv_height_flashCameraTrkWght_flashPhotoTrkWght", _pg_shader_Mixing);
		pg_allocateBindAndCheckUniform(indConfig, uniform_Mixing_fs_3fv_screenMsgTransp_Text1_2_Alpha, "uniform_Mixing_fs_3fv_screenMsgTransp_Text1_2_Alpha", _pg_shader_Mixing);

#if defined(var_nb_CATypes)
		if (ScenarioVarConfigurations[_nb_CATypes][indConfig]) {
			pg_allocateBindAndCheckUniform(indConfig, uniform_Mixing_texture_fs_CA, "uniform_Mixing_texture_fs_CA", _pg_shader_Mixing); // ping-pong CA (FBO)
		}
#endif
		pg_allocateBindAndCheckUniform(indConfig, uniform_Mixing_texture_fs_ClipArt_render, "uniform_Mixing_texture_fs_ClipArt_render", _pg_shader_Mixing);  // Particles (FBO)
		pg_allocateBindAndCheckUniform(indConfig, uniform_Mixing_texture_fs_Particle_render, "uniform_Mixing_texture_fs_Particle_render", _pg_shader_Mixing);  // Particles (FBO)
		pg_allocateBindAndCheckUniform(indConfig, uniform_Mixing_texture_fs_Trk0, "uniform_Mixing_texture_fs_Trk0", _pg_shader_Mixing);  // ping-pong track 0 (FBO)
#if PG_NB_TRACKS >= 2
		pg_allocateBindAndCheckUniform(indConfig, uniform_Mixing_texture_fs_Trk1, "uniform_Mixing_texture_fs_Trk1", _pg_shader_Mixing);  // ping-pong track 1 (FBO)
#endif
#if PG_NB_TRACKS >= 3
		pg_allocateBindAndCheckUniform(indConfig, uniform_Mixing_texture_fs_Trk2, "uniform_Mixing_texture_fs_Trk2", _pg_shader_Mixing);  // ping-pong track 2 (FBO)
#endif
#if PG_NB_TRACKS >= 4
		pg_allocateBindAndCheckUniform(indConfig, uniform_Mixing_texture_fs_Trk3, "uniform_Mixing_texture_fs_Trk3", _pg_shader_Mixing);  // ping-pong track 3 (FBO)
#endif
#if defined(TVW)
		pg_allocateBindAndCheckUniform(indConfig, uniform_Mixing_texture_fs_Display_Font, "uniform_Mixing_texture_fs_Display_Font", _pg_shader_Mixing);  // message Font
		pg_allocateBindAndCheckUniform(indConfig, uniform_Mixing_texture_fs_Display_Message1, "uniform_Mixing_texture_fs_Display_Message1", _pg_shader_Mixing);  // tweets at the bottom of the screen
		pg_allocateBindAndCheckUniform(indConfig, uniform_Mixing_texture_fs_Display_Message2, "uniform_Mixing_texture_fs_Display_Message2", _pg_shader_Mixing);  // tweets at the bottom of the screen
#endif

		pg_allocateBindAndCheckUniform(indConfig, uniform_Mixing_texture_fs_Render_prec, "uniform_Mixing_texture_fs_Render_prec", _pg_shader_Mixing);  // preceding snapshot for echo
		pg_allocateBindAndCheckUniform(indConfig, uniform_Mixing_texture_fs_Screen_Font, "uniform_Mixing_texture_fs_Screen_Font", _pg_shader_Mixing);  // message Font
		pg_allocateBindAndCheckUniform(indConfig, uniform_Mixing_texture_fs_Screen_Message, "uniform_Mixing_texture_fs_Screen_Message", _pg_shader_Mixing);  // message string

		////////////////////////////////////////////////////////////////////////////////
		// MASTER SHADER parameters bindings
		////////////////////////////////////////////////////////////////////////////////
		pg_allocateBindAndCheckUniform(indConfig, uniform_Master_vp_model, "vp_modelMatrix", _pg_shader_Master);
		pg_allocateBindAndCheckUniform(indConfig, uniform_Master_vp_view, "vp_viewMatrix", _pg_shader_Master);
		pg_allocateBindAndCheckUniform(indConfig, uniform_Master_vp_proj, "vp_projMatrix", _pg_shader_Master);
		pg_allocateBindAndCheckUniform(indConfig, uniform_Master_fs_4fv_xy_frameno_pulsedShift, "uniform_Master_fs_4fv_xy_frameno_pulsedShift", _pg_shader_Master);
#if defined(TVW)
		pg_allocateBindAndCheckUniform(indConfig, uniform_Master_fs_4fv_width_height_rightWindowVMargin_timeFromStart, "uniform_Master_fs_4fv_width_height_rightWindowVMargin_timeFromStart", _pg_shader_Master);
#endif
		pg_allocateBindAndCheckUniform(indConfig, uniform_Master_fs_4fv_width_height_timeFromStart_muteRightScreen, "uniform_Master_fs_4fv_width_height_timeFromStart_muteRightScreen", _pg_shader_Master);

		//pg_allocateBindAndCheckUniform(indConfig, uniform_Master_fs_4fv_pulsedColor_rgb_pen_grey, "uniform_Master_fs_4fv_pulsedColor_rgb_pen_grey", _pg_shader_Master);
		//pg_allocateBindAndCheckUniform(indConfig, uniform_Master_fs_4fv_interpolatedPaletteLow_rgb_currentScene, "uniform_Master_fs_4fv_interpolatedPaletteLow_rgb_currentScene", _pg_shader_Master);
		//pg_allocateBindAndCheckUniform(indConfig, uniform_Master_fs_4fv_interpolatedPaletteMedium_rgb_mobile_cursor, "uniform_Master_fs_4fv_interpolatedPaletteMedium_rgb_mobile_cursor", _pg_shader_Master);
		//pg_allocateBindAndCheckUniform(indConfig, uniform_Master_fs_3fv_interpolatedPaletteHigh_rgb, "uniform_Master_fs_3fv_interpolatedPaletteHigh_rgb", _pg_shader_Master);
		pg_allocateBindAndCheckUniform(indConfig, uniform_Master_fs_2iv_mobile_cursor_currentScene, "uniform_Master_fs_2iv_mobile_cursor_currentScene", _pg_shader_Master);
#if defined(var_Caverne_BackColor)
		if (ScenarioVarConfigurations[_Caverne_BackColor][indConfig]) {
			pg_allocateBindAndCheckUniform(indConfig, uniform_Master_fs_3fv_Caverne_BackColor_rgb, "uniform_Master_fs_3fv_Caverne_BackColor_rgb", _pg_shader_Master);
		}
#endif
#if defined(var_flashchange_BGcolor_freq)
		if (ScenarioVarConfigurations[_flashchange_BGcolor_freq][indConfig]) {
			pg_allocateBindAndCheckUniform(indConfig, uniform_Master_fs_3fv_BG_color_rgb, "uniform_Master_fs_3fv_BG_color_rgb", _pg_shader_Master);
		}
#endif

		pg_allocateBindAndCheckUniform(indConfig, uniform_Master_texture_fs_Render_curr, "uniform_Master_texture_fs_Render_curr", _pg_shader_Master); // previous pass output
#if defined(var_nb_CATypes)
		if (ScenarioVarConfigurations[_nb_CATypes][indConfig]) {
			pg_allocateBindAndCheckUniform(indConfig, uniform_Master_texture_fs_CA, "uniform_Master_texture_fs_CA", _pg_shader_Master); // ping-pong CA (FBO)
		}
#endif
		pg_allocateBindAndCheckUniform(indConfig, uniform_Master_texture_fs_ClipArt_render, "uniform_Master_texture_fs_ClipArt_render", _pg_shader_Master); // Particles
		pg_allocateBindAndCheckUniform(indConfig, uniform_Master_texture_fs_Particle_render, "uniform_Master_texture_fs_Particle_render", _pg_shader_Master); // Particles
		pg_allocateBindAndCheckUniform(indConfig, uniform_Master_texture_fs_Trk0, "uniform_Master_texture_fs_Trk0", _pg_shader_Master); // ping-pong track 0 (FBO)
#if PG_NB_TRACKS >= 2
		pg_allocateBindAndCheckUniform(indConfig, uniform_Master_texture_fs_Trk1, "uniform_Master_texture_fs_Trk1", _pg_shader_Master); // ping-pong track 1 (FBO)
#endif
#if PG_NB_TRACKS >= 3
		pg_allocateBindAndCheckUniform(indConfig, uniform_Master_texture_fs_Trk2, "uniform_Master_texture_fs_Trk2", _pg_shader_Master); // ping-pong track 2 (FBO)
#endif
#if PG_NB_TRACKS >= 4
		pg_allocateBindAndCheckUniform(indConfig, uniform_Master_texture_fs_Trk3, "uniform_Master_texture_fs_Trk3", _pg_shader_Master); // ping-pong track 3 (FBO)
#endif
#ifdef PG_WITH_MASTER_MASK
		pg_allocateBindAndCheckUniform(indConfig, uniform_Master_texture_fs_Mask, "uniform_Master_texture_fs_Mask", _pg_shader_Master);  // mask for master output
#endif

#if defined(var_sensor_layout)
		if (ScenarioVarConfigurations[_sensor_layout][indConfig]) {
			////////////////////////////////////////////////////////////////////////////////
			// SENSOR SHADER parameters bindings
			////////////////////////////////////////////////////////////////////////////////
			pg_allocateBindAndCheckUniform(indConfig, uniform_Sensor_vp_model, "vp_modelMatrix", _pg_shader_Sensor);
			pg_allocateBindAndCheckUniform(indConfig, uniform_Sensor_vp_view, "vp_viewMatrix", _pg_shader_Sensor);
			pg_allocateBindAndCheckUniform(indConfig, uniform_Sensor_vp_proj, "vp_projMatrix", _pg_shader_Sensor);
			//pg_allocateBindAndCheckUniform(indConfig, uniform_Sensor_fs_2fv_frameno_invert, "uniform_Sensor_fs_2fv_frameno_invert", _pg_shader_Sensor);
			pg_allocateBindAndCheckUniform(indConfig, uniform_Sensor_fs_4fv_onOff_isCurrentSensor_masterLevel_transparency, "uniform_Sensor_fs_4fv_onOff_isCurrentSensor_masterLevel_transparency", _pg_shader_Sensor);

			pg_allocateBindAndCheckUniform(indConfig, uniform_Sensor_texture_fs_decal, "fs_decal", _pg_shader_Sensor); // sensor texture
		}
#endif

#if defined(var_activeMeshes)
		if (ScenarioVarConfigurations[_activeMeshes][indConfig]) {
			////////////////////////////////////////////////////////////////////////////////
			// MESH SHADER parameters bindings
			////////////////////////////////////////////////////////////////////////////////
			pg_allocateBindAndCheckUniform(indConfig, uniform_Mesh_vp_model, "vp_modelMatrix", _pg_shader_Mesh);
			pg_allocateBindAndCheckUniform(indConfig, uniform_Mesh_vp_view, "vp_viewMatrix", _pg_shader_Mesh);
			pg_allocateBindAndCheckUniform(indConfig, uniform_Mesh_vp_proj, "vp_projMatrix", _pg_shader_Mesh);
			pg_allocateBindAndCheckUniform(indConfig, uniform_Mesh_fs_4fv_isDisplayLookAt_with_mesh_with_blue_currentScene, "uniform_Mesh_fs_4fv_isDisplayLookAt_with_mesh_with_blue_currentScene", _pg_shader_Mesh);
			pg_allocateBindAndCheckUniform(indConfig, uniform_Mesh_fs_3fv_light, "uniform_Mesh_fs_3fv_light", _pg_shader_Mesh);
#if defined(var_Contact_mesh_expand)
			if (ScenarioVarConfigurations[_Contact_mesh_expand][indConfig]) {
				pg_allocateBindAndCheckUniform(indConfig, uniform_Mesh_vp_2fv_dilate_explode, "uniform_Mesh_vp_2fv_dilate_explode", _pg_shader_Mesh);
			}
#endif
#if defined(var_Contact_mesh_anime)
			if (ScenarioVarConfigurations[_Contact_mesh_anime][indConfig]) {
				pg_allocateBindAndCheckUniform(indConfig, uniform_Mesh_bones_matrices, "uniform_Mesh_bones_matrices", _pg_shader_Mesh);
			}
#endif
#if defined(var_MmeShanghai_brokenGlass)
			if (ScenarioVarConfigurations[_MmeShanghai_brokenGlass][indConfig]) {
				pg_allocateBindAndCheckUniform(indConfig, uniform_Mesh_fs_4fv_color, "uniform_Mesh_fs_4fv_color", _pg_shader_Mesh);
				pg_allocateBindAndCheckUniform(indConfig, uniform_Mesh_fs_4fv_color_master_photo_weight_bg, "uniform_Mesh_fs_4fv_color_master_photo_weight_bg", _pg_shader_Mesh);
			}
#endif
			// Augmented Reality: FBO capture of Master to be displayed on a mesh
#if defined(var_textureFrontier_wmin) && defined(var_textureFrontier_wmax) && defined(var_textureFrontier_hmin) && defined(var_textureFrontier_hmax) && defined(var_textureFrontier_wmin_width) && defined(var_textureFrontier_wmax_width) and defined(var_textureFrontier_hmin_width) && defined(var_textureFrontier_hmax_width) && defined(var_textureScale_w) && defined(var_textureScale_h) and defined(var_textureTranslate_w) && defined(var_textureTranslate_h)
			if (ScenarioVarConfigurations[_textureFrontier_wmin][indConfig]) {
				pg_allocateBindAndCheckUniform(indConfig, uniform_Mesh_fs_4fv_textureFrontier, "uniform_Mesh_fs_4fv_textureFrontier", _pg_shader_Mesh);
				pg_allocateBindAndCheckUniform(indConfig, uniform_Mesh_fs_4fv_textureFrontier_width, "uniform_Mesh_fs_4fv_textureFrontier_width", _pg_shader_Mesh);
				pg_allocateBindAndCheckUniform(indConfig, uniform_Mesh_fs_4fv_textureScaleTransl, "uniform_Mesh_fs_4fv_textureScaleTransl", _pg_shader_Mesh);
			}
#endif
#if defined(var_Contact_mesh_palette)
			if (ScenarioVarConfigurations[_Contact_mesh_palette][indConfig]) {
				pg_allocateBindAndCheckUniform(indConfig, uniform_Mesh_fs_4fv_color_palette, "uniform_Mesh_fs_4fv_color_palette", _pg_shader_Mesh); // mesh color
			}
#endif
			pg_allocateBindAndCheckUniform(indConfig, uniform_Mesh_texture_fs_decal, "fs_Mesh_texture_fs_decal", _pg_shader_Mesh); // previous pass output
#if defined(var_MmeShanghai_brokenGlass)
			if (ScenarioVarConfigurations[_MmeShanghai_brokenGlass][indConfig]) {
				pg_allocateBindAndCheckUniform(indConfig, uniform_Mesh_texture_fs_BG, "fs_Mesh_texture_fs_BG", _pg_shader_Mesh); // previous pass output
			}
#endif
		}
#endif
		std::cout << std::endl;
	}
	std::cout << std::endl;
}
