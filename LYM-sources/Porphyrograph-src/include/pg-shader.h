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

//////////////////////////////////////////////////////////////////////////////////////////////////////
// CONSTs
//////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
// SHADER PROGRAMMES
enum ShaderFileTypes { pg_enum_shader_ParticleAnimation = 0,
	pg_enum_shader_Update,
	pg_enum_shader_ParticleRender,
	pg_enum_shader_Mixing,
	pg_enum_shader_Master,
	pg_enum_shader_Sensor,
	pg_enum_shader_ClipArt,
	pg_enum_shader_Mesh,
	pg_enum_shader_TotalNbTypes
};

//////////////////////////////////////////////////////////////////////////////////////////////////////
// EXPORTED VARIABLES
////////////////////////////////////////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////
// tables connecting scenario, cpp and glsl variables
extern GLint uniform_ParticleAnimation_scenario_var_data[PG_MAX_SCENARIOS];
extern GLint uniform_Mixing_scenario_var_data[PG_MAX_SCENARIOS];
extern GLint uniform_Update_scenario_var_data[PG_MAX_SCENARIOS];
extern GLint uniform_ParticleRender_scenario_var_data[PG_MAX_SCENARIOS];
extern GLint uniform_Master_scenario_var_data[PG_MAX_SCENARIOS];
extern float* ParticleAnimation_scenario_var_data;
extern float* Update_scenario_var_data;
extern float* Mixing_scenario_var_data;
extern float* Master_scenario_var_data;

extern unsigned int ** pg_shader_programme;

/////////////////////////////////////////////////////////////////////////
// PARTICLE ANIMATION SHADER
// PARTICLE ANIMATION SHADER UNIFORM VARIABLES
extern GLint uniform_ParticleAnimation_vp_model[PG_MAX_SCENARIOS];
extern GLint uniform_ParticleAnimation_vp_view[PG_MAX_SCENARIOS];
extern GLint uniform_ParticleAnimation_vp_proj[PG_MAX_SCENARIOS];
extern GLint uniform_ParticleAnimation_vp_2fv_width_height[PG_MAX_SCENARIOS];
extern GLint uniform_ParticleAnimation_fs_4fv_W_H_repopChannel_targetFrameNo[PG_MAX_SCENARIOS];
extern GLint uniform_ParticleAnimation_fs_4fv_flashTrkPartWghts[PG_MAX_SCENARIOS];
extern GLint uniform_ParticleAnimation_path_data[PG_MAX_SCENARIOS];
extern GLint uniform_ParticleAnimation_fs_4fv_repop_Color_frameNo[PG_MAX_SCENARIOS];
extern GLint uniform_ParticleAnimation_fs_4fv_flashCAPartWght_nbPart_clear_partSizeUnpulsed[PG_MAX_SCENARIOS];
extern GLint uniform_ParticleAnimation_fs_4fv_Camera_W_H_movieWH[PG_MAX_SCENARIOS];
extern GLint uniform_ParticleAnimation_texture_fs_Trk0[PG_MAX_SCENARIOS];  // ping-pong track 0 (FBO)
extern GLint uniform_ParticleAnimation_texture_fs_Trk1[PG_MAX_SCENARIOS];  // ping-pong track 1 (FBO)
extern GLint uniform_ParticleAnimation_texture_fs_Trk2[PG_MAX_SCENARIOS];  // ping-pong track 2 (FBO)
extern GLint uniform_ParticleAnimation_texture_fs_Trk3[PG_MAX_SCENARIOS];  // ping-pong track 3 (FBO)

// UPDATE SHADER TEXTURE IDS
extern GLint uniform_ParticleAnimation_texture_fs_CA[PG_MAX_SCENARIOS];         // ping-pong CA (FBO)
extern GLint uniform_ParticleAnimation_texture_fs_Part_pos_speed[PG_MAX_SCENARIOS];  // 2-cycle ping-pong position/speed of particles (FBO)
extern GLint uniform_ParticleAnimation_texture_fs_Part_col_rad[PG_MAX_SCENARIOS];  // 2-cycle ping-pong color/radius of particles (FBO)
extern GLint uniform_ParticleAnimation_texture_fs_Part_Target_pos_col_rad[PG_MAX_SCENARIOS];  // 2-cycle ping-pong target position/color/radius of particles (FBO)
extern GLint uniform_ParticleAnimation_texture_fs_Camera_frame[PG_MAX_SCENARIOS];  // camera frame
extern GLint uniform_ParticleAnimation_texture_fs_Movie_frame[PG_MAX_SCENARIOS];  // movie frame
extern GLint uniform_ParticleAnimation_texture_fs_Noise[PG_MAX_SCENARIOS];  // 3D noise
extern GLint uniform_ParticleAnimation_texture_fs_RepopDensity[PG_MAX_SCENARIOS];  // repop density
extern GLint uniform_ParticleAnimation_texture_fs_Part_init_pos_speed[PG_MAX_SCENARIOS];  // particle initialization pairs of textures position/speed
extern GLint uniform_ParticleAnimation_texture_fs_Part_init_col_rad[PG_MAX_SCENARIOS];  // particle initialization pairs of textures color/radius
extern GLint uniform_ParticleAnimation_texture_fs_Part_acc[PG_MAX_SCENARIOS];  // particle acceleration texture

/////////////////////////////////////////////////////////////////////////
// UPDATE SHADER
// UPDATE SHADER UNIFORM VARIABLES
extern GLint uniform_Update_vp_model[PG_MAX_SCENARIOS];
extern GLint uniform_Update_vp_view[PG_MAX_SCENARIOS];
extern GLint uniform_Update_vp_proj[PG_MAX_SCENARIOS];
extern GLint uniform_Update_homographyForTexture[PG_MAX_SCENARIOS];
extern GLint uniform_Update_vp_2fv_width_height[PG_MAX_SCENARIOS];
extern GLint uniform_Update_fs_4fv_W_H_time_currentScene[PG_MAX_SCENARIOS];
extern GLint uniform_Update_fs_3fv_clearAllLayers_clearCA_pulsedShift[PG_MAX_SCENARIOS];
#if defined(pg_Project_CAaudio)
extern GLint uniform_Update_fs_4fv_CAseed_type_size_loc[PG_MAX_SCENARIOS];
#endif
extern GLint uniform_Update_fs_4fv_flashTrkBGWghts_flashPartBGWght[PG_MAX_SCENARIOS];
extern GLint uniform_Update_fs_4fv_frameno_Cursor_flashPartCAWght_doubleWindow[PG_MAX_SCENARIOS];
extern GLint uniform_Update_fs_4fv_flashTrkCAWghts[PG_MAX_SCENARIOS];
extern GLint uniform_Update_path_data[PG_MAX_SCENARIOS];

// pg_Project_Criton PROJECT
extern GLint uniform_Update_fs_4fv_fftLevels03[PG_MAX_SCENARIOS];
extern GLint uniform_Update_fs_4fv_fftFrequencies03[PG_MAX_SCENARIOS];
extern GLint uniform_Update_fs_4fv_fftPhases03[PG_MAX_SCENARIOS];
extern GLint uniform_Update_fs_4fv_fftLevels47[PG_MAX_SCENARIOS];
extern GLint uniform_Update_fs_4fv_fftFrequencies47[PG_MAX_SCENARIOS];
extern GLint uniform_Update_fs_4fv_fftPhases47[PG_MAX_SCENARIOS];

extern GLint uniform_Update_fs_4fv_movieWH_flashCameraTrkWght_cpTrack[PG_MAX_SCENARIOS];
extern GLint uniform_Update_fs_4fv_repop_ColorBG_flashCABGWght[PG_MAX_SCENARIOS];
extern GLint uniform_Update_fs_3fv_repop_ColorCA[PG_MAX_SCENARIOS];
extern GLint uniform_Update_fs_3fv_isClearLayer_flashPixel_flashCameraTrkThres[PG_MAX_SCENARIOS];
extern GLint uniform_Update_fs_4fv_flashPhotoTrkWght_flashPhotoTrkThres_Photo_offSetsXY[PG_MAX_SCENARIOS];
extern GLint uniform_Update_fs_4fv_photo01_wh[PG_MAX_SCENARIOS];
extern GLint uniform_Update_fs_4fv_photo01Wghts_randomValues[PG_MAX_SCENARIOS];
extern GLint uniform_Update_fs_2fv_clip01Wghts[PG_MAX_SCENARIOS];
extern GLint uniform_Update_fs_4fv_Camera_offSetsXY_Camera_W_H[PG_MAX_SCENARIOS];
extern GLint uniform_Update_fs_4fv_xy_transl_tracks_0_1[PG_MAX_SCENARIOS];
extern GLint uniform_Update_fs_4fv_CAType_SubType_blurRadius[PG_MAX_SCENARIOS];
extern GLint uniform_Update_fs_3fv_photo_rgb[PG_MAX_SCENARIOS];
extern GLint uniform_Update_texture_fs_CATable[PG_MAX_SCENARIOS];
#if defined(var_GenerativeNights_planes)
extern GLint uniform_Update_fs_2fv_initCA_1stPlaneFrameNo[PG_MAX_SCENARIOS];
#endif
extern GLint uniform_Update_texture_fs_Camera_BGIni[PG_MAX_SCENARIOS];
extern GLint uniform_Update_texture_fs_pixel_acc[PG_MAX_SCENARIOS];

// UPDATE SHADER TEXTURE IDS
extern GLint uniform_Update_texture_fs_CA[PG_MAX_SCENARIOS];         // ping-pong CA (FBO)
extern GLint uniform_Update_texture_fs_PreviousCA[PG_MAX_SCENARIOS];         // ping-pong CA (FBO)
extern GLint uniform_Update_texture_fs_Pixels[PG_MAX_SCENARIOS];  // 2-cycle ping-pong speed/position of pixels (FBO)
extern GLint uniform_Update_texture_fs_Trk0[PG_MAX_SCENARIOS];  // ping-pong track 0 (FBO)
extern GLint uniform_Update_texture_fs_Trk1[PG_MAX_SCENARIOS];  // ping-pong track 1 (FBO)
extern GLint uniform_Update_texture_fs_Trk2[PG_MAX_SCENARIOS];  // ping-pong track 2 (FBO)
extern GLint uniform_Update_texture_fs_Trk3[PG_MAX_SCENARIOS];  // ping-pong track 3 (FBO)
extern GLint uniform_Update_texture_fs_Brushes[PG_MAX_SCENARIOS];  // pen patterns
extern GLint uniform_Update_texture_fs_Camera_frame[PG_MAX_SCENARIOS];  // camera frame
extern GLint uniform_Update_texture_fs_Camera_BG[PG_MAX_SCENARIOS];  // camera BG capture
extern GLint uniform_Update_texture_fs_Movie_frame[PG_MAX_SCENARIOS];  // movie frame
extern GLint uniform_Update_texture_fs_Noise[PG_MAX_SCENARIOS];  // 3D noise
extern GLint uniform_Update_texture_fs_RepopDensity[PG_MAX_SCENARIOS];  // repop density
extern GLint uniform_Update_texture_fs_Photo0[PG_MAX_SCENARIOS];  // photo[0]
extern GLint uniform_Update_texture_fs_Photo1[PG_MAX_SCENARIOS];  // photo[1]
extern GLint uniform_Update_texture_fs_Clip0[PG_MAX_SCENARIOS];  // clip[0]
extern GLint uniform_Update_texture_fs_Clip1[PG_MAX_SCENARIOS];  // clip[1]
extern GLint uniform_Update_texture_fs_Part_render[PG_MAX_SCENARIOS];  // FBO capture of particle rendering used for flashing layers with particles
extern GLint uniform_Update_texture_fs_Part_init_pos_speed[PG_MAX_SCENARIOS];  // particle initialization pairs of textures position/speed
extern GLint uniform_Update_texture_fs_Part_init_col_rad[PG_MAX_SCENARIOS];  // particle initialization pairs of textures color/radius

/////////////////////////////////////////////////////////////////////////
// PARTICLE RENDERING SHADER
// PARTICLE RENDERING SHADER UNIFORM VARIABLES
extern GLint uniform_ParticleSplat_vp_model[PG_MAX_SCENARIOS];
extern GLint uniform_ParticleSplat_vp_view[PG_MAX_SCENARIOS];
extern GLint uniform_ParticleSplat_vp_proj[PG_MAX_SCENARIOS];
extern GLint uniform_ParticleSplat_gs_4fv_part_size_partType_highPitchPulse_windowRatio[PG_MAX_SCENARIOS];
extern GLint uniform_ParticleSplat_vp_3fv_trackReplay_xy_height[PG_MAX_SCENARIOS];

// PARTICLE SHADER TEXTURE IDS
extern GLint uniform_ParticleSplat_texture_vp_Part_pos_speed[PG_MAX_SCENARIOS];				 // Particle position/speed update
extern GLint uniform_ParticleSplat_texture_vp_Part_col_rad[PG_MAX_SCENARIOS];          // Particle color/radius update

// CURVE PARTICLES
extern GLint uniform_ParticleCurve_vp_model[PG_MAX_SCENARIOS];
extern GLint uniform_ParticleCurve_vp_view[PG_MAX_SCENARIOS];
extern GLint uniform_ParticleCurve_vp_proj[PG_MAX_SCENARIOS];
extern GLint uniform_ParticleCurve_gs_3fv_partRadius_partType_highPitchPulse[PG_MAX_SCENARIOS];
extern GLint uniform_ParticleCurve_vp_3fv_trackReplay_xy_height[PG_MAX_SCENARIOS];

// PARTICLE SHADER TEXTURE IDS
extern GLint uniform_ParticleCurve_texture_vp_Part_pos_speed[PG_MAX_SCENARIOS];				 // Particle position/speed update
extern GLint uniform_ParticleCurve_texture_vp_Part_col_rad[PG_MAX_SCENARIOS];          // Particle color/radius update

// color texture
extern GLint uniform_ParticleCurve_Curve_texture_fs_decal[PG_MAX_SCENARIOS];  // comet texture

// color texture
extern GLint uniform_ParticleSplat_texture_fs_decal[PG_MAX_SCENARIOS];  // blurred disk texture

/////////////////////////////////////////////////////////////////////////
// MIXING SHADER
// MIXING SHADER UNIFORM VARIABLES
extern GLint uniform_Mixing_vp_model[PG_MAX_SCENARIOS];
extern GLint uniform_Mixing_vp_view[PG_MAX_SCENARIOS];
extern GLint uniform_Mixing_vp_proj[PG_MAX_SCENARIOS];
extern GLint uniform_Mixing_fs_3fv_height_flashCameraTrkWght_flashPhotoTrkWght[PG_MAX_SCENARIOS];
extern GLint uniform_Mixing_fs_3fv_screenMsgTransp_Text1_2_Alpha[PG_MAX_SCENARIOS];

// MIXING SHADER TEXTURE IDS
extern GLint uniform_Mixing_texture_fs_CA[PG_MAX_SCENARIOS]; // ping-pong BG track (FBO)
extern GLint uniform_Mixing_texture_fs_ClipArt_render[PG_MAX_SCENARIOS];  // Particles (FBO)
extern GLint uniform_Mixing_texture_fs_Particle_render[PG_MAX_SCENARIOS];  // Particles (FBO)
extern GLint uniform_Mixing_texture_fs_Trk0[PG_MAX_SCENARIOS];  // ping-pong track 0 (FBO)
extern GLint uniform_Mixing_texture_fs_Trk1[PG_MAX_SCENARIOS];  // ping-pong track 1 (FBO)
extern GLint uniform_Mixing_texture_fs_Trk2[PG_MAX_SCENARIOS];  // ping-pong track 2 (FBO)
extern GLint uniform_Mixing_texture_fs_Trk3[PG_MAX_SCENARIOS];  // ping-pong track 3 (FBO)
extern GLint uniform_Mixing_texture_fs_Render_prec[PG_MAX_SCENARIOS];  // preceding snapshot
extern GLint uniform_Mixing_texture_fs_Screen_Font[PG_MAX_SCENARIOS];  // message Font
extern GLint uniform_Mixing_texture_fs_Screen_Message[PG_MAX_SCENARIOS];  // message string
extern GLint uniform_Mixing_texture_fs_Mask[PG_MAX_SCENARIOS];  // mask for master output

/////////////////////////////////////////////////////////////////////////
// MASTER SHADER
// MASTER SHADER UNIFORM VARIABLES
extern GLint uniform_Master_vp_model[PG_MAX_SCENARIOS];
extern GLint uniform_Master_vp_view[PG_MAX_SCENARIOS];
extern GLint uniform_Master_vp_proj[PG_MAX_SCENARIOS];
extern GLint uniform_Master_fs_4fv_xy_frameno_pulsedShift[PG_MAX_SCENARIOS];
extern GLint uniform_Master_fs_4fv_width_height_timeFromStart_muteRightScreen[PG_MAX_SCENARIOS];
//extern GLint uniform_Master_fs_4fv_pulsedColor_rgb_pen_grey[PG_MAX_SCENARIOS];
//extern GLint uniform_Master_fs_4fv_interpolatedPaletteLow_rgb_currentScene[PG_MAX_SCENARIOS];
//extern GLint uniform_Master_fs_4fv_interpolatedPaletteMedium_rgb_mobile_cursor[PG_MAX_SCENARIOS];
//extern GLint uniform_Master_fs_3fv_interpolatedPaletteHigh_rgb[PG_MAX_SCENARIOS];
extern GLint uniform_Master_fs_2iv_mobile_cursor_currentScene[PG_MAX_SCENARIOS];
extern GLint uniform_Master_fs_3fv_BGcolor_rgb[PG_MAX_SCENARIOS];

// MASTER SHADER TEXTURE IDS
extern GLint uniform_Master_texture_fs_Render_curr[PG_MAX_SCENARIOS];         // previous pass output
extern GLint uniform_Master_texture_fs_CA[PG_MAX_SCENARIOS];  // ping-pong CA (FBO)
extern GLint uniform_Master_texture_fs_ClipArt_render[PG_MAX_SCENARIOS];  // Particles
extern GLint uniform_Master_texture_fs_Particle_render[PG_MAX_SCENARIOS];  // Particles
extern GLint uniform_Master_texture_fs_Trk0[PG_MAX_SCENARIOS];  // ping-pong track 0 (FBO)
extern GLint uniform_Master_texture_fs_Trk1[PG_MAX_SCENARIOS];  // ping-pong track 1 (FBO)
extern GLint uniform_Master_texture_fs_Trk2[PG_MAX_SCENARIOS];  // ping-pong track 2 (FBO)
extern GLint uniform_Master_texture_fs_Trk3[PG_MAX_SCENARIOS];  // ping-pong track 3 (FBO)
extern GLint uniform_Master_texture_fs_Mask[PG_MAX_SCENARIOS];  // mask for master output

  /////////////////////////////////////////////////////////////////////////
// SENSOR SHADER
// SENSOR SHADER UNIFORM VARIABLES
extern GLint uniform_Sensor_vp_model[PG_MAX_SCENARIOS];
extern GLint uniform_Sensor_vp_view[PG_MAX_SCENARIOS];
extern GLint uniform_Sensor_vp_proj[PG_MAX_SCENARIOS];

// SENSOR SHADER TEXTURE IDS
extern GLint uniform_Sensor_texture_fs_decal[PG_MAX_SCENARIOS];         // sensor texture
extern GLint uniform_Sensor_fs_4fv_onOff_isCurrentSensor_masterLevel_transparency[PG_MAX_SCENARIOS];

/////////////////////////////////////////////////////////////////////////
// MESH SHADER
// MESH SHADER UNIFORM VARIABLES
extern GLint uniform_Mesh_vp_model[PG_MAX_SCENARIOS];
extern GLint uniform_Mesh_vp_view[PG_MAX_SCENARIOS];
extern GLint uniform_Mesh_vp_proj[PG_MAX_SCENARIOS];
extern GLint uniform_Mesh_fs_4fv_isDisplayLookAt_with_mesh_with_blue_currentScene[PG_MAX_SCENARIOS];
extern GLint uniform_Mesh_fs_3fv_light[PG_MAX_SCENARIOS];
#if defined(var_Contact_mesh_expand)
extern GLint uniform_Mesh_vp_2fv_dilate_explode[PG_MAX_SCENARIOS];
#endif
#if defined(var_Contact_mesh_anime)
extern GLint uniform_Mesh_bones_matrices[PG_MAX_SCENARIOS];
#endif
#if defined(var_MmeShanghai_brokenGlass)
extern GLint uniform_Mesh_fs_4fv_color[PG_MAX_SCENARIOS];
extern GLint uniform_Mesh_fs_4fv_color_master_photo_weight_bg[PG_MAX_SCENARIOS];
#endif
// Augmented Reality: FBO capture of Master to be displayed on a mesh
extern GLint uniform_Mesh_fs_4fv_textureFrontier[PG_MAX_SCENARIOS];
extern GLint uniform_Mesh_fs_4fv_textureFrontier_width[PG_MAX_SCENARIOS];
extern GLint uniform_Mesh_fs_4fv_textureScaleTransl[PG_MAX_SCENARIOS];
// Mesh SHADER TEXTURE IDS
extern GLint uniform_Mesh_texture_fs_decal[PG_MAX_SCENARIOS];         // Mesh texture
#if defined(var_Contact_mesh_palette)
extern GLint uniform_Mesh_fs_4fv_color_palette[PG_MAX_SCENARIOS];         // Mesh color
#endif
#if defined(var_MmeShanghai_brokenGlass)
extern GLint uniform_Mesh_texture_fs_BG[PG_MAX_SCENARIOS];         // Mesh texture
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////
// EXPORTED FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////

void pg_loadAllShaders(void);
void pg_parseScenarioShaders(std::ifstream& scenarioFin, int indScenario);

#endif
