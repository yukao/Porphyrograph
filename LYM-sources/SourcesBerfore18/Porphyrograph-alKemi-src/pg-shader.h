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
extern unsigned int shader_TrackDisplay_programme;

/////////////////////////////////////////////////////////////////////////
// DRAWING SHADER
// DRAWING SHADER UNIFORM VARIABLES
extern GLint uniform_Drawing_vp_model;
extern GLint uniform_Drawing_vp_view;
extern GLint uniform_Drawing_vp_proj;
extern GLint uniform_Drawing_vp_2fv_width_height;
extern GLint uniform_Drawing_fs_4fv_W_H_drawingStart_drawingStroke;
extern GLint uniform_Drawing_fs_4fv_CASubType_partAccCenter_void;
extern GLint uniform_Drawing_fs_4fv_partMode_partAcc_flashCA_partTexture;
extern GLint uniform_Drawing_fs_4fv_CAdecay_flashPart_flashTrack0_textureSnapshot;
extern GLint uniform_Drawing_fs_4fv_trackdecay_CAstep;
extern GLint uniform_Drawing_fs_4fv_CAType_frameno_flashBack_currentTrack;
extern GLint uniform_Drawing_fs_2fv_flashBackCoef_clr;
extern GLint uniform_Drawing_fs_3fv_mouseTracks_replay;
extern GLint uniform_Drawing_fs_3fv_mouseTracks_x;
extern GLint uniform_Drawing_fs_3fv_mouseTracks_y;
extern GLint uniform_Drawing_fs_3iv_mouseTracks_x_transl;
extern GLint uniform_Drawing_fs_3iv_mouseTracks_y_transl;
extern GLint uniform_Drawing_fs_3fv_mouseTracks_x_prev;
extern GLint uniform_Drawing_fs_3fv_mouseTracks_y_prev;
extern GLint uniform_Drawing_fs_3fv_mouseTracks_Color_r;
extern GLint uniform_Drawing_fs_3fv_mouseTracks_Color_g;
extern GLint uniform_Drawing_fs_3fv_mouseTracks_Color_b;
extern GLint uniform_Drawing_fs_3fv_mouseTracks_Color_a;
extern GLint uniform_Drawing_fs_3fv_mouseTracks_BrushID;
extern GLint uniform_Drawing_fs_3fv_mouseTracks_RadiusX;
extern GLint uniform_Drawing_fs_3fv_mouseTracks_RadiusY;

// DRAWING SHADER TEXTURE IDS
extern GLint uniform_Drawing_texture_fs_decal;         // 3-cycle ping-pong localColor step n-1 (FBO)
extern GLint uniform_Drawing_texture_fs_lookupTable1;  // 3-cycle ping-pong speed/position of particles (FBO)
extern GLint uniform_Drawing_texture_fs_lookupTable2;  // 3-cycle ping-pong localColor step n-2 (FBO)
extern GLint uniform_Drawing_texture_fs_lookupTable3;  // ping-pong CA (FBO)
extern GLint uniform_Drawing_texture_fs_lookupTable4;  // ping-pong track 1 (FBO)
extern GLint uniform_Drawing_texture_fs_lookupTable5;  // ping-pong track 2 (FBO)
extern GLint uniform_Drawing_texture_fs_lookupTable6;  // pen patterns
extern GLint uniform_Drawing_texture_fs_lookupTable7;  // particle acceleration texture
extern GLint uniform_Drawing_texture_fs_lookupTable8;  // particle acceleration FBO

/////////////////////////////////////////////////////////////////////////
// COMPOSITION SHADER
// COMPOSITION SHADER UNIFORM VARIABLES
extern GLint uniform_Composition_vp_model;
extern GLint uniform_Composition_vp_view;
extern GLint uniform_Composition_vp_proj;
extern GLint uniform_Composition_fs_3fv_width_height_CAweight;
extern GLint uniform_Composition_fs_3fv_trackweight;
extern GLint uniform_Composition_fs_3fv_messageTransparency_echo_echoNeg;
extern GLint uniform_Composition_texture_fs_decal; // ping-pong mouse localColor/gray GOL

// COMPOSITION SHADER TEXTURE IDS
extern GLint uniform_Composition_texture_fs_lookupTable1;  // ping-pong CA (FBO)
extern GLint uniform_Composition_texture_fs_lookupTable2;  // ping-pong track 1 (FBO)
extern GLint uniform_Composition_texture_fs_lookupTable3;  // ping-pong track 2 (FBO)
extern GLint uniform_Composition_texture_fs_lookupTable4;  // previous pass output
extern GLint uniform_Composition_texture_fs_lookupTable_font;  // message Font
extern GLint uniform_Composition_texture_fs_lookupTable_message;  // message string

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

/////////////////////////////////////////////////////////////////////////
// FINAL SHADER
// FINAL SHADER UNIFORM VARIABLES
extern GLint uniform_Final_vp_model;
extern GLint uniform_Final_vp_view;
extern GLint uniform_Final_vp_proj;
extern GLint uniform_Final_fs_4fv_transparency_invert_leftwidth_rightVMargin;
extern GLint uniform_Final_fs_4fv_xy_frameno_cursorSize;
extern GLint uniform_Final_fs_height;

// FINAL SHADER TEXTURE IDS
extern GLint uniform_Final_texture_fs_decal;         // previous pass output
extern GLint uniform_Final_texture_fs_lookupTable1;  // previous display tracks output

#endif
