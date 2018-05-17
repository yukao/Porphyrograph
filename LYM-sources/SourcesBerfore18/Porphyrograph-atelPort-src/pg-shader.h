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

#include "pg_shader_header.h"

unsigned long pg_getFileLength(ifstream& file);
void pg_printShaderCompileLog(GLuint obj);
void pg_printShaderLinkLog(GLuint obj);
void pg_loadAllShaders( void );
void pg_shaderVariablesInit( void );
// INITIALIZES ALL SCENARIO VARIABLES AND ASSIGNS THEM THE VALUES OF THE FIRST SCENARIO LINE
void pg_initializationScript( void );

/////////////////////////////////////////////////////////////////////////
// SHADER PROGRAMMES
enum ShaderFileTypes {
	pg_shader_Update = 0, 
	pg_shader_Particle, pg_shader_Mixing,
	pg_shader_Master, pg_NbShaderTotal
};
extern unsigned int shader_programme[pg_NbShaderTotal];

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

/////////////////////////////////////////////////////////////////////////
// UPDATE SHADER
// UPDATE SHADER UNIFORM VARIABLES
extern GLint uniform_Update_vp_model;
extern GLint uniform_Update_vp_view;
extern GLint uniform_Update_vp_proj;
extern GLint uniform_Update_vp_2fv_width_height;
extern GLint uniform_Update_fs_4fv_W_H_repopChannel_targetFrameNo;
extern GLint uniform_Update_fs_4fv_clearAllLayers_clearCA_pixelRadius_pulsedShift;
extern GLint uniform_Update_fs_4fv_flashTrkBGWghts_flashPartBGWght;
extern GLint uniform_Update_fs_4fv_trkDecay_partDecay;
extern GLint uniform_Update_fs_4fv_CAdecay_frameno_Cursor_flashPartCAWght;
extern GLint uniform_Update_fs_4fv_flashTrkCAWghts;
extern GLint uniform_Update_fs_4fv_flashTrkPartWghts;
#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7
extern GLint uniform_Update_fs_4fv_paths03_x;
extern GLint uniform_Update_fs_4fv_paths03_y;
extern GLint uniform_Update_fs_4fv_paths03_x_prev;
extern GLint uniform_Update_fs_4fv_paths03_y_prev;
extern GLint uniform_Update_fs_4fv_paths03_r;
extern GLint uniform_Update_fs_4fv_paths03_g;
extern GLint uniform_Update_fs_4fv_paths03_b;
extern GLint uniform_Update_fs_4fv_paths03_a;
extern GLint uniform_Update_fs_4fv_paths03_BrushID;
extern GLint uniform_Update_fs_4fv_paths03_RadiusX;
extern GLint uniform_Update_fs_4fv_paths03_RadiusY;
#endif
#if PG_NB_PATHS == 7
extern GLint uniform_Update_fs_4fv_paths47_x;
extern GLint uniform_Update_fs_4fv_paths47_y;
extern GLint uniform_Update_fs_4fv_paths47_x_prev;
extern GLint uniform_Update_fs_4fv_paths47_y_prev;
extern GLint uniform_Update_fs_4fv_paths47_r;
extern GLint uniform_Update_fs_4fv_paths47_g;
extern GLint uniform_Update_fs_4fv_paths47_b;
extern GLint uniform_Update_fs_4fv_paths47_a;
extern GLint uniform_Update_fs_4fv_paths47_BrushID;
extern GLint uniform_Update_fs_4fv_paths47_RadiusX;
extern GLint uniform_Update_fs_4fv_paths47_RadiusY;
#endif
extern GLint uniform_Update_fs_4fv_pulse;
extern GLint uniform_Update_fs_4fv_movieWH_flashCameraTrkWght_cpTrack;
extern GLint uniform_Update_fs_4fv_repop_Color_flashCABGWght;
extern GLint uniform_Update_fs_4fv_isClearLayer_flashPixel_flashCAPartWght_nbParticles;
extern GLint uniform_Update_fs_4fv_photo01_wh;
extern GLint uniform_Update_fs_4fv_photo01Wghts_Camera_W_H;
extern GLint uniform_Update_fs_4fv_repop_part_path_acc_damp_factor;
extern GLint uniform_Update_fs_4fv_xy_transl_tracks_0_1;

// UPDATE SHADER TEXTURE IDS
extern GLint uniform_Update_texture_fs_decal;         // ping-pong CA (FBO)
extern GLint uniform_Update_texture_fs_lookupTable1;  // 2-cycle ping-pong speed/position of pixels (FBO)
extern GLint uniform_Update_texture_fs_lookupTable2;  // 2-cycle ping-pong position/speed of particles (FBO)
extern GLint uniform_Update_texture_fs_lookupTable3;  // 2-cycle ping-pong color/radius of particles (FBO)
extern GLint uniform_Update_texture_fs_lookupTable4;  // 2-cycle ping-pong target position/color/radius of particles (FBO)
extern GLint uniform_Update_texture_fs_lookupTable5;  // ping-pong track 0 (FBO)
#if PG_NB_TRACKS >= 2
extern GLint uniform_Update_texture_fs_lookupTable6;  // ping-pong track 1 (FBO)
#endif
#if PG_NB_TRACKS >= 3
extern GLint uniform_Update_texture_fs_lookupTable7;  // ping-pong track 2 (FBO)
#endif
#if PG_NB_TRACKS >= 4
extern GLint uniform_Update_texture_fs_lookupTable8;  // ping-pong track 3 (FBO)
#endif
extern GLint uniform_Update_texture_fs_lookupTable8;  // pen patterns
#ifdef PG_VIDEO_ACTIVE
extern GLint uniform_Update_texture_fs_lookupTable9;  // camera frame
extern GLint uniform_Update_texture_fs_lookupTable10;  // camera BG capture
extern GLint uniform_Update_texture_fs_lookupTable11;  // movie frame
#endif
extern GLint uniform_Update_texture_fs_lookupTable12;  // 3D noise
extern GLint uniform_Update_texture_fs_lookupTable13;  // photo[0]
extern GLint uniform_Update_texture_fs_lookupTable14;  // photo[1]
extern GLint uniform_Update_texture_fs_lookupTable15;  // FBO capture of particle rendering used for flashing layers with particles
extern GLint uniform_Update_texture_fs_lookupTable16;  // particle initialization pairs of textures position/speed
extern GLint uniform_Update_texture_fs_lookupTable17;  // particle initialization pairs of textures color/radius

/////////////////////////////////////////////////////////////////////////
// PARTICLE SHADER
// PARTICLE SHADER UNIFORM VARIABLES
extern GLint uniform_Particle_vp_model;
extern GLint uniform_Particle_vp_view;
extern GLint uniform_Particle_vp_proj;
extern GLint uniform_ParticleSplat_gs_3fv_partRadius_partType_highPitchPulse;
extern GLint uniform_ParticleSplat_vp_2fv_trackReplay_xy;

// PARTICLE SHADER TEXTURE IDS
extern GLint uniform_Particle_texture_vp_decal;				 // Particle position/speed update
extern GLint uniform_Particle_texture_vp_lookupTable1;          // Particle color/radius update
#ifdef CURVE_PARTICLES
// color texture
extern GLint uniform_Particle_Comet_texture_fs_decal;  // comet texture
#endif
#ifdef BLURRED_SPLAT_PARTICLES
// color texture
extern GLint uniform_Particle_BlurredDisk_texture_fs_decal;  // blurred disk texture
#endif

/////////////////////////////////////////////////////////////////////////
// MIXING SHADER
// MIXING SHADER UNIFORM VARIABLES
extern GLint uniform_Mixing_vp_model;
extern GLint uniform_Mixing_vp_view;
extern GLint uniform_Mixing_vp_proj;
extern GLint uniform_Mixing_fs_4fv_pulsedShift_height_msgTrspcy_flashCameraTrkWght;
extern GLint uniform_Mixing_texture_fs_decal; // ping-pong BG track (FBO)

// MIXING SHADER TEXTURE IDS
extern GLint uniform_Mixing_texture_fs_decal;         // ping-pong CA (FBO)
extern GLint uniform_Mixing_texture_fs_lookupTable1;  // Particles (FBO)
extern GLint uniform_Mixing_texture_fs_lookupTable2;  // ping-pong track 0 (FBO)
#if PG_NB_TRACKS >= 2
extern GLint uniform_Mixing_texture_fs_lookupTable3;  // ping-pong track 1 (FBO)
#endif
#if PG_NB_TRACKS >= 3
extern GLint uniform_Mixing_texture_fs_lookupTable4;  // ping-pong track 2 (FBO)
#endif
#if PG_NB_TRACKS >= 4
extern GLint uniform_Mixing_texture_fs_lookupTable5;  // ping-pong track 3 (FBO)
#endif
extern GLint uniform_Mixing_texture_fs_lookupTable5;  // preceding snapshot
extern GLint uniform_Mixing_texture_fs_lookupTable_font;  // message Font
extern GLint uniform_Mixing_texture_fs_lookupTable_message;  // message string

/////////////////////////////////////////////////////////////////////////
// MASTER SHADER
// MASTER SHADER UNIFORM VARIABLES
extern GLint uniform_Master_vp_model;
extern GLint uniform_Master_vp_view;
extern GLint uniform_Master_vp_proj;
extern GLint uniform_Master_fs_4fv_xy_frameno_pulsedShift;
extern GLint uniform_Master_fs_3fv_width_height_mute_screen;

// MASTER SHADER TEXTURE IDS
extern GLint uniform_Master_texture_fs_decal;         // previous pass output
extern GLint uniform_Master_texture_fs_lookupTable1;  // ping-pong CA (FBO)
extern GLint uniform_Master_texture_fs_lookupTable2;  // Particles
extern GLint uniform_Master_texture_fs_lookupTable3;  // ping-pong track 0 (FBO)
#if PG_NB_TRACKS >= 2
extern GLint uniform_Master_texture_fs_lookupTable4;  // ping-pong track 1 (FBO)
#endif
#if PG_NB_TRACKS >= 3
extern GLint uniform_Master_texture_fs_lookupTable5;  // ping-pong track 2 (FBO)
#endif
#if PG_NB_TRACKS >= 4
extern GLint uniform_Master_texture_fs_lookupTable6;  // ping-pong track 3 (FBO)
#endif

#endif
