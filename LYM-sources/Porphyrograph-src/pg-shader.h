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

#ifdef GN
#include "pg_shader_header_GN.h"
#endif
#ifdef TVW
#include "pg_shader_header_TVW.h"
#endif
#ifdef CRITON
#include "pg_shader_header_Criton.h"
#endif
#if defined (KOMPARTSD)
#include "pg_shader_header_KompartSD.h"
#endif
#ifdef effe
#include "pg_shader_header_effe.h"
#endif
#ifdef DEMO
#include "pg_shader_header_demo.h"
#endif
#ifdef VOLUSPA
#include "pg_shader_header_voluspa.h"
#endif
#ifdef INTERFERENCE
#include "pg_shader_header_interference.h"
#endif
#ifdef MALAUSSENA
#include "pg_shader_header_Malaussena.h"
#endif
#ifdef DASEIN
#include "pg_shader_header_dasein.h"
#endif
#ifdef BONNOTTE
#include "pg_shader_header_bonnotte.h"
#endif

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
#if defined (BLURRED_SPLAT_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 
	pg_shader_ParticleAnimation = 0,
	pg_shader_Update,
#else
	pg_shader_Update = 0,
#endif
#if defined (BLURRED_SPLAT_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 
	pg_shader_ParticleRender,
#endif
	pg_shader_Mixing,
	pg_shader_Master, 
#ifdef PG_SENSORS
	pg_shader_Sensor,
#endif
	pg_SvgGpu,
	pg_NbShaderTotal
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

#if defined (BLURRED_SPLAT_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 
/////////////////////////////////////////////////////////////////////////
// PARTICLE ANIMATION SHADER
// PARTICLE ANIMATION SHADER UNIFORM VARIABLES
extern GLint uniform_ParticleAnimation_vp_model;
extern GLint uniform_ParticleAnimation_vp_view;
extern GLint uniform_ParticleAnimation_vp_proj;
extern GLint uniform_ParticleAnimation_vp_2fv_width_height;
extern GLint uniform_ParticleAnimation_fs_4fv_W_H_repopChannel_targetFrameNo;
extern GLint uniform_ParticleAnimation_fs_1fv_partDecay;
extern GLint uniform_ParticleAnimation_fs_4fv_flashTrkPartWghts;
#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7
extern GLint uniform_ParticleAnimation_fs_4fv_paths03_x;
extern GLint uniform_ParticleAnimation_fs_4fv_paths03_y;
extern GLint uniform_ParticleAnimation_fs_4fv_paths03_x_prev;
extern GLint uniform_ParticleAnimation_fs_4fv_paths03_y_prev;
extern GLint uniform_ParticleAnimation_fs_4fv_paths03_RadiusX;
#endif
#if PG_NB_PATHS == 7
extern GLint uniform_ParticleAnimation_fs_4fv_paths47_x;
extern GLint uniform_ParticleAnimation_fs_4fv_paths47_y;
extern GLint uniform_ParticleAnimation_fs_4fv_paths47_x_prev;
extern GLint uniform_ParticleAnimation_fs_4fv_paths47_y_prev;
extern GLint uniform_ParticleAnimation_fs_4fv_paths47_RadiusX;
#endif
extern GLint uniform_ParticleAnimation_fs_4fv_repop_Color_frameNo;
extern GLint uniform_ParticleAnimation_fs_4fv_flashCAPartWght_nbPart_clear_partRadius;
extern GLint uniform_ParticleAnimation_fs_4fv_repop_part_path_acc_damp_factor;
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
extern GLint uniform_ParticleAnimation_texture_fs_Part_init_pos_speed;  // particle initialization pairs of textures position/speed
extern GLint uniform_ParticleAnimation_texture_fs_Part_init_col_rad;  // particle initialization pairs of textures color/radius
#endif

/////////////////////////////////////////////////////////////////////////
// UPDATE SHADER
// UPDATE SHADER UNIFORM VARIABLES
extern GLint uniform_Update_vp_model;
extern GLint uniform_Update_vp_view;
extern GLint uniform_Update_vp_proj;
extern GLint uniform_Update_vp_2fv_width_height;
extern GLint uniform_Update_fs_4fv_W_H_time_currentScene;
extern GLint uniform_Update_fs_4fv_clearAllLayers_clearCA_pixelRadius_pulsedShift;
#ifdef MALAUSSENA
extern GLint uniform_Update_fs_4fv_CAseed_type_size_loc;
#endif
#if defined (BLURRED_SPLAT_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) || PG_NB_TRACKS >= 2
extern GLint uniform_Update_fs_4fv_flashTrkBGWghts_flashPartBGWght;
#endif
extern GLint uniform_Update_fs_4fv_trkDecay;
extern GLint uniform_Update_fs_4fv_CAdecay_frameno_Cursor_flashPartCAWght;
#ifdef PG_NB_CA_TYPES
extern GLint uniform_Update_fs_4fv_flashTrkCAWghts;
#endif
#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7
extern GLint uniform_Update_fs_4fv_paths03_x;
extern GLint uniform_Update_fs_4fv_paths03_y;
extern GLint uniform_Update_fs_4fv_paths03_x_prev;
extern GLint uniform_Update_fs_4fv_paths03_y_prev;
#ifdef PG_BEZIER_CURVES
// pen Bezier curve tangents
extern GLint uniform_Update_fs_4fv_paths03_xL;
extern GLint uniform_Update_fs_4fv_paths03_yL;
extern GLint uniform_Update_fs_4fv_paths03_xR;
extern GLint uniform_Update_fs_4fv_paths03_yR;
extern GLint uniform_Update_fs_4iv_path03_beginOrEnd;
#endif
extern GLint uniform_Update_fs_4fv_paths03_r;
extern GLint uniform_Update_fs_4fv_paths03_g;
extern GLint uniform_Update_fs_4fv_paths03_b;
extern GLint uniform_Update_fs_4fv_paths03_a;
extern GLint uniform_Update_fs_4fv_paths03_BrushID;
extern GLint uniform_Update_fs_4fv_paths03_RadiusX;
#ifndef PG_BEZIER_CURVES
extern GLint uniform_Update_fs_4fv_paths03_RadiusY;
#endif
#endif
#if PG_NB_PATHS == 7
extern GLint uniform_Update_fs_4fv_paths47_x;
extern GLint uniform_Update_fs_4fv_paths47_y;
extern GLint uniform_Update_fs_4fv_paths47_x_prev;
extern GLint uniform_Update_fs_4fv_paths47_y_prev;
#ifdef PG_BEZIER_CURVES
// pen Bezier curve tangents
extern GLint uniform_Update_fs_4fv_paths47_xL;
extern GLint uniform_Update_fs_4fv_paths47_yL;
extern GLint uniform_Update_fs_4fv_paths47_xR;
extern GLint uniform_Update_fs_4fv_paths47_yR;
extern GLint uniform_Update_fs_4iv_path47_beginOrEnd;
#endif
extern GLint uniform_Update_fs_4fv_paths47_r;
extern GLint uniform_Update_fs_4fv_paths47_g;
extern GLint uniform_Update_fs_4fv_paths47_b;
extern GLint uniform_Update_fs_4fv_paths47_a;
extern GLint uniform_Update_fs_4fv_paths47_BrushID;
extern GLint uniform_Update_fs_4fv_paths47_RadiusX;
#ifndef PG_BEZIER_CURVES
extern GLint uniform_Update_fs_4fv_paths47_RadiusY;
#endif
#endif
#ifdef CRITON
extern GLint uniform_Update_fs_4fv_fftLevels03;
extern GLint uniform_Update_fs_4fv_fftFrequencies03;
extern GLint uniform_Update_fs_4fv_fftPhases03;
extern GLint uniform_Update_fs_4fv_fftLevels47;
extern GLint uniform_Update_fs_4fv_fftFrequencies47;
extern GLint uniform_Update_fs_4fv_fftPhases47;
#endif
extern GLint uniform_Update_fs_4fv_pulse;
extern GLint uniform_Update_fs_4fv_movieWH_flashCameraTrkWght_cpTrack;
extern GLint uniform_Update_fs_4fv_repop_Color_flashCABGWght;
extern GLint uniform_Update_fs_3fv_isClearLayer_flashPixel_flashCameraTrkThres;
#if defined (PG_WITH_PHOTO_DIAPORAMA) || defined (TVW)
extern GLint uniform_Update_fs_4fv_photo01_wh;
#endif
#ifdef PG_WITH_PHOTO_DIAPORAMA
extern GLint uniform_Update_fs_2fv_photo01Wghts;
#endif
extern GLint uniform_Update_fs_4fv_Camera_offSetsXY_Camera_W_H;
extern GLint uniform_Update_fs_4fv_xy_transl_tracks_0_1;
#if defined (PG_NB_CA_TYPES) || defined (PG_WITH_BLUR)
extern GLint uniform_Update_fs_4fv_CAType_SubType_blurRadius;
#endif
#if defined (GN) || defined (MALAUSSENA)
extern GLint uniform_Update_texture_fs_CATable;
#endif
#ifdef GN
extern GLint uniform_Update_fs_2fv_initCA_1stPlaneFrameNo;
extern GLint uniform_Update_texture_fs_Camera_BGIni;
#endif

// UPDATE SHADER TEXTURE IDS
#ifdef PG_NB_CA_TYPES
extern GLint uniform_Update_texture_fs_CA;         // ping-pong CA (FBO)
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
extern GLint uniform_Update_texture_fs_Brushes;  // pen patterns
extern GLint uniform_Update_texture_fs_Camera_frame;  // camera frame
extern GLint uniform_Update_texture_fs_Camera_BG;  // camera BG capture
extern GLint uniform_Update_texture_fs_Movie_frame;  // movie frame
extern GLint uniform_Update_texture_fs_Noise;  // 3D noise
#ifdef PG_WITH_PHOTO_DIAPORAMA
extern GLint uniform_Update_texture_fs_Photo0;  // photo[0]
extern GLint uniform_Update_texture_fs_Photo1;  // photo[1]
#endif
#if defined (BLURRED_SPLAT_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES)
extern GLint uniform_Update_texture_fs_Part_render;  // FBO capture of particle rendering used for flashing layers with particles
#endif
extern GLint uniform_Update_texture_fs_Part_init_pos_speed;  // particle initialization pairs of textures position/speed
extern GLint uniform_Update_texture_fs_Part_init_col_rad;  // particle initialization pairs of textures color/radius

/////////////////////////////////////////////////////////////////////////
// PARTICLE RENDERING SHADER
// PARTICLE RENDERING SHADER UNIFORM VARIABLES
#if defined (BLURRED_SPLAT_PARTICLES) || defined (LINE_SPLAT_PARTICLES) 
extern GLint uniform_ParticleSplat_vp_model;
extern GLint uniform_ParticleSplat_vp_view;
extern GLint uniform_ParticleSplat_vp_proj;
extern GLint uniform_ParticleSplat_gs_3fv_partRadius_partType_highPitchPulse;
extern GLint uniform_ParticleSplat_vp_3fv_trackReplay_xy_height;

// PARTICLE SHADER TEXTURE IDS
extern GLint uniform_ParticleSplat_texture_vp_Part_pos_speed;				 // Particle position/speed update
extern GLint uniform_ParticleSplat_texture_vp_Part_col_rad;          // Particle color/radius update
#endif
#if defined (CURVE_PARTICLES) 
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
#ifdef BLURRED_SPLAT_PARTICLES_TEXTURED
// color texture
extern GLint uniform_ParticleSplat_BlurredDisk_texture_fs_decal;  // blurred disk texture
#endif

/////////////////////////////////////////////////////////////////////////
// MIXING SHADER
// MIXING SHADER UNIFORM VARIABLES
extern GLint uniform_Mixing_vp_model;
extern GLint uniform_Mixing_vp_view;
extern GLint uniform_Mixing_vp_proj;
extern GLint uniform_Mixing_fs_3fv_pulsedShift_height_flashCameraTrkWght;
extern GLint uniform_Mixing_fs_3fv_screenMsgTransp_Text1_2_Alpha;

// MIXING SHADER TEXTURE IDS
#ifdef PG_NB_CA_TYPES
extern GLint uniform_Mixing_texture_fs_CA; // ping-pong BG track (FBO)
#endif
#if defined (BLURRED_SPLAT_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES)
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
#if defined (TVW)
extern GLint uniform_Mixing_texture_fs_Display_Font;  // message Font
extern GLint uniform_Mixing_texture_fs_Display_Message1; // tweets at the bottom of the screen
extern GLint uniform_Mixing_texture_fs_Display_Message2; // tweets at the bottom of the screen
#endif
extern GLint uniform_Mixing_texture_fs_Render_prec;  // preceding snapshot
extern GLint uniform_Mixing_texture_fs_Screen_Font;  // message Font
extern GLint uniform_Mixing_texture_fs_Screen_Message;  // message string
#if defined (TVW)
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
extern GLint uniform_Master_fs_4fv_width_height_rightWindowVMargin_timeFromStart;
extern GLint uniform_Master_fs_4fv_pulsedColor_rgb_pen_grey;
extern GLint uniform_Master_fs_3fv_interpolatedPaletteLow_rgb;
extern GLint uniform_Master_fs_3fv_interpolatedPaletteMedium_rgb;
extern GLint uniform_Master_fs_3fv_interpolatedPaletteHigh_rgb;

// MASTER SHADER TEXTURE IDS
extern GLint uniform_Master_texture_fs_Render_curr;         // previous pass output
#ifdef PG_NB_CA_TYPES
extern GLint uniform_Master_texture_fs_CA;  // ping-pong CA (FBO)
#endif
#if defined (BLURRED_SPLAT_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES)
extern GLint uniform_Master_texture_fs_Part_render;  // Particles
#endif
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
#if defined (GN) || defined (INTERFERENCE)
extern GLint uniform_Master_texture_fs_LYMlogo;  // LYM logo (texture)
#endif

#ifdef PG_SENSORS
  /////////////////////////////////////////////////////////////////////////
// SENSOR SHADER
// SENSOR SHADER UNIFORM VARIABLES
extern GLint uniform_Sensor_vp_model;
extern GLint uniform_Sensor_vp_view;
extern GLint uniform_Sensor_vp_proj;

// SENSOR SHADER TEXTURE IDS
extern GLint uniform_Sensor_texture_fs_decal;         // sensor texture
extern GLint uniform_Sensor_fs_4fv_onOff_isCurrentSensor_isFollowMouse_transparency;
// extern GLint uniform_Sensor_fs_2fv_frameno_invert;
#endif

#endif
