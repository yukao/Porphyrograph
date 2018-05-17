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
void pg_printLog(GLuint obj);
void pg_loadAllShaders( void );
void pg_shaderVariablesInit( void );
void pg_initializationScript( void );

/////////////////////////////////////////////////////////////////////////
// SHADER PROGRAMMES
extern unsigned int shader_Drawing_programme;
extern unsigned int shader_Composition_programme;
extern unsigned int shader_Final_programme;
#ifdef PG_TRACK_DISPLAY
extern unsigned int shader_TrackDisplay_programme;
#endif

/////////////////////////////////////////////////////////////////////////
// DRAWING SHADER
// DRAWING SHADER UNIFORM VARIABLES
extern GLint uniform_Drawing_vp_model;
extern GLint uniform_Drawing_vp_view;
extern GLint uniform_Drawing_vp_proj;
extern GLint uniform_Drawing_vp_2fv_width_height;
extern GLint uniform_Drawing_fs_4fv_W_H_drawingStart_drawingStroke;
extern GLint uniform_Drawing_fs_4fv_weights03;
extern GLint uniform_Drawing_fs_2fv_weights45;
extern GLint uniform_Drawing_fs_3fv_alphaSwap02;
extern GLint uniform_Drawing_fs_3fv_alphaSwap35;
extern GLint uniform_Drawing_fs_4fv_image_noisesxy;
extern GLint uniform_Drawing_fs_4fv_mask_noisesxy;
extern GLint uniform_Drawing_fs_4fv_flashPart_flashTrack0Coefs_copyTrack12;
extern GLint uniform_Drawing_fs_3fv_clearLayer_flashVideoCoef_flashVideoThreshold;
//extern GLint uniform_Drawing_fs_4fv_mouseTracks_xy_prevxy;
extern GLint uniform_Drawing_fs_4fv_flashBackCoef_videoWH_videoThreshold;
//extern GLint uniform_Drawing_fs_3iv_mouseTracks_x_transl;
//extern GLint uniform_Drawing_fs_3iv_mouseTracks_y_transl;
extern GLint uniform_Drawing_fs_4fv_mouseTracks_Color_rgba;
extern GLint uniform_Drawing_fs_4fv_partAcc_clearAllLayers_frameNo_time;
//extern GLint uniform_Drawing_fs_4fv_mouseTracks_RadiusXY_isNewBeat_Cursor;
extern GLint uniform_Drawing_fs_4fv_pulse;
#ifdef PG_WITH_CA
//extern GLint uniform_Drawing_fs_4fv_CAdecay_clearCA_flashCACoefs;
#endif

// DRAWING SHADER TEXTURE IDS
extern GLint uniform_Drawing_texture_fs_decal;         // 2-cycle ping-pong BG track step n-1 (FBO)
extern GLint uniform_Drawing_texture_fs_lookupTable1;  // 2-cycle ping-pong speed/position of particles (FBO)
#ifdef PG_WITH_CA
//extern GLint uniform_Drawing_texture_fs_lookupTable2;  // ping-pong CA (FBO)
#endif
extern GLint uniform_Drawing_texture_fs_lookupTable3;  // ping-pong track 1 (FBO)
extern GLint uniform_Drawing_texture_fs_lookupTable4;  // ping-pong track 2 (FBO)
//extern GLint uniform_Drawing_texture_fs_lookupTable5;  // pen patterns
extern GLint uniform_Drawing_texture_fs_lookupTable6;  // particle acceleration texture
#ifdef PG_VIDEO_ACTIVE
extern GLint uniform_Drawing_texture_fs_lookupTable7;  // video frame
//extern GLint uniform_Drawing_texture_fs_lookupTable8;  // current background video frame
//extern GLint uniform_Drawing_texture_fs_lookupTable9;  // initial background video frame
#endif
extern GLint uniform_Drawing_texture_fs_lookupTable10;  // image buffer #0
extern GLint uniform_Drawing_texture_fs_lookupTable11;  // image buffer #1
extern GLint uniform_Drawing_texture_fs_lookupTable12;  // image buffer #2
extern GLint uniform_Drawing_texture_fs_lookupTable13;  // image buffer #3
extern GLint uniform_Drawing_texture_fs_lookupTable14;  // image buffer #4
extern GLint uniform_Drawing_texture_fs_lookupTable15;  // image buffer #5
extern GLint uniform_Drawing_texture_fs_lookupTable16;  // image mask #0
extern GLint uniform_Drawing_texture_fs_lookupTable17;  // image mask #1
//extern GLint uniform_Drawing_texture_fs_lookupTable18;  // image mask #2
//extern GLint uniform_Drawing_texture_fs_lookupTable19;  // image mask #3
extern GLint uniform_Drawing_texture_fs_lookupTable20;  // buffer swap image #0
extern GLint uniform_Drawing_texture_fs_lookupTable21;  // buffer swap image #1
extern GLint uniform_Drawing_texture_fs_lookupTable22;  // buffer swap image #2
extern GLint uniform_Drawing_texture_fs_lookupTable23;  // buffer swap image #3
extern GLint uniform_Drawing_texture_fs_lookupTable24;  // buffer swap image #4
extern GLint uniform_Drawing_texture_fs_lookupTable25;  // buffer swap image #5

/////////////////////////////////////////////////////////////////////////
// COMPOSITION SHADER
// COMPOSITION SHADER UNIFORM VARIABLES
extern GLint uniform_Composition_vp_model;
extern GLint uniform_Composition_vp_view;
extern GLint uniform_Composition_vp_proj;
extern GLint uniform_Composition_fs_4fv_height_screenMsgTransp_Text1_2_Alpha;
extern GLint uniform_Composition_fs_3fv_flashVideoCoef_flashVideoThreshold_width;
extern GLint uniform_Composition_texture_fs_decal; // ping-pong BG track (FBO)

// COMPOSITION SHADER TEXTURE IDS
#ifdef PG_WITH_CA
extern GLint uniform_Composition_texture_fs_lookupTable1;  // ping-pong CA (FBO)
#endif
extern GLint uniform_Composition_texture_fs_lookupTable2;  // ping-pong track 1 (FBO)
extern GLint uniform_Composition_texture_fs_lookupTable3;  // ping-pong track 2 (FBO)
extern GLint uniform_Composition_texture_fs_lookupTable4;  // previous pass output
// extern GLint uniform_Composition_texture_fs_lookupTable5;  // captured frame buffer
extern GLint uniform_Composition_texture_fs_lookupTable_screen_font;  // small system message Font
extern GLint uniform_Composition_texture_fs_lookupTable_screen_msg_offset;  // small system message string
extern GLint uniform_Composition_texture_fs_lookupTable_displayMsg_font;  // large display message Font
extern GLint uniform_Composition_texture_fs_lookupTable_displayMsg1_offset;  // large display message string
extern GLint uniform_Composition_texture_fs_lookupTable_displayMsg2_offset;  // large display message string
extern GLint uniform_Composition_texture_fs_lookupTable_hashtag_font;        // hashtag very large font
extern GLint uniform_Composition_texture_fs_lookupTable_hashtag_offset;       // hashtag very large font offests

#ifdef PG_TRACK_DISPLAY
/////////////////////////////////////////////////////////////////////////
// TRACK DISPLAY SHADER
// TRACK DISPLAY SHADER UNIFORM VARIABLES
extern GLint uniform_TrackDisplay_vp_4fv_width_FrameNo_noiseAmplitude_globalRadius;
extern GLint uniform_TrackDisplay_gs_model;
extern GLint uniform_TrackDisplay_gs_view;
extern GLint uniform_TrackDisplay_gs_proj;
extern GLint uniform_TrackDisplay_fs_1fv_transparency;

// TRACK DISPLAY SHADER TEXTURE IDS
extern GLint uniform_TrackDisplay_texture_vp_lookupTable1; // position
extern GLint uniform_TrackDisplay_texture_vp_lookupTable2; // color
extern GLint uniform_TrackDisplay_texture_vp_lookupTable3; // (radius, brush ID and TrackDisplaying mode)
// extern GLint uniform_TrackDisplay_texture_vp_lookupTable4; // noise
extern GLint uniform_TrackDisplay_texture_fs_decal;        // brush
#endif

/////////////////////////////////////////////////////////////////////////
// FINAL SHADER
// FINAL SHADER UNIFORM VARIABLES
extern GLint uniform_Final_vp_model;
extern GLint uniform_Final_vp_view;
extern GLint uniform_Final_vp_proj;
extern GLint uniform_Final_fs_3fv_xy_frameno;
extern GLint uniform_Final_fs_2fv_width_height;

// FINAL SHADER TEXTURE IDS
extern GLint uniform_Final_texture_fs_decal;         // previous pass output
#ifdef PG_TRACK_DISPLAY
extern GLint uniform_Final_texture_fs_lookupTable1;  // previous display tracks output
#endif
extern GLint uniform_Final_texture_fs_lookupTable2;  // ping-pong BG track (FBO)
#ifdef PG_WITH_CA
extern GLint uniform_Final_texture_fs_lookupTable3;  // ping-pong CA (FBO)
#endif
#if PG_NB_TRACKS >= 2
extern GLint uniform_Final_texture_fs_lookupTable4;  // ping-pong track 1 (FBO)
#endif
#if PG_NB_TRACKS >= 3
extern GLint uniform_Final_texture_fs_lookupTable5;  // ping-pong track 2 (FBO)
#endif

#endif
