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
extern unsigned int shader_Sensor_programme;

/////////////////////////////////////////////////////////////////////////
// DRAWING SHADER
// DRAWING SHADER UNIFORM VARIABLES
extern GLint uniform_Drawing_vp_model;
extern GLint uniform_Drawing_vp_view;
extern GLint uniform_Drawing_vp_proj;
extern GLint uniform_Drawing_vp_2fv_width_height;
extern GLint uniform_Drawing_fs_4fv_W_H_drawingStart_drawingStroke;
extern GLint uniform_Drawing_fs_3fv_partAcc_clearAllLayers_clearCA;
extern GLint uniform_Drawing_fs_4fv_flashPart_flashTrack0Coefs_copyTrack12;
extern GLint uniform_Drawing_fs_4fv_trackdecay_clearLayer;
extern GLint uniform_Drawing_fs_3fv_CAdecay_frameno_currentDrawingTrack;
extern GLint uniform_Drawing_fs_4fv_flashBackCoef_flashCACoefs;
extern GLint uniform_Drawing_fs_3fv_mouseTracks_x;
extern GLint uniform_Drawing_fs_4fv_mouseTracks_y_videoH;
//extern GLint uniform_Drawing_fs_3iv_mouseTracks_x_transl;
//extern GLint uniform_Drawing_fs_3iv_mouseTracks_y_transl;
extern GLint uniform_Drawing_fs_4fv_mouseTracks_x_prev_videoW;
extern GLint uniform_Drawing_fs_4fv_mouseTracks_y_prev_videoThreshold;
extern GLint uniform_Drawing_fs_3fv_mouseTracks_Color_r;
extern GLint uniform_Drawing_fs_3fv_mouseTracks_Color_g;
extern GLint uniform_Drawing_fs_3fv_mouseTracks_Color_b;
extern GLint uniform_Drawing_fs_3fv_mouseTracks_Color_a;
extern GLint uniform_Drawing_fs_4fv_mouseTracks_BrushID_Cursor;
extern GLint uniform_Drawing_fs_3fv_mouseTracks_RadiusX;
extern GLint uniform_Drawing_fs_3fv_mouseTracks_RadiusY;

// DRAWING SHADER TEXTURE IDS
extern GLint uniform_Drawing_texture_fs_decal;         // 2-cycle ping-pong BG track step n-1 (FBO)
extern GLint uniform_Drawing_texture_fs_lookupTable1;  // 2-cycle ping-pong speed/position of particles (FBO)
extern GLint uniform_Drawing_texture_fs_lookupTable2;  // ping-pong CA (FBO)
extern GLint uniform_Drawing_texture_fs_lookupTable3;  // ping-pong track 1 (FBO)
extern GLint uniform_Drawing_texture_fs_lookupTable4;  // ping-pong track 2 (FBO)
extern GLint uniform_Drawing_texture_fs_lookupTable5;  // pen patterns
extern GLint uniform_Drawing_texture_fs_lookupTable6;  // particle acceleration texture
#ifdef PG_VIDEO_ACTIVE
extern GLint uniform_Drawing_texture_fs_lookupTable7;  // video frame
extern GLint uniform_Drawing_texture_fs_lookupTable8;  // current background video frame
extern GLint uniform_Drawing_texture_fs_lookupTable9;  // initial background video frame
#endif

/////////////////////////////////////////////////////////////////////////
// COMPOSITION SHADER
// COMPOSITION SHADER UNIFORM VARIABLES
extern GLint uniform_Composition_vp_model;
extern GLint uniform_Composition_vp_view;
extern GLint uniform_Composition_vp_proj;
extern GLint uniform_Composition_fs_3fv_width_height_messageTransparency;
extern GLint uniform_Composition_texture_fs_decal; // ping-pong BG track (FBO)

// COMPOSITION SHADER TEXTURE IDS
extern GLint uniform_Composition_texture_fs_lookupTable1;  // ping-pong CA (FBO)
extern GLint uniform_Composition_texture_fs_lookupTable2;  // ping-pong track 1 (FBO)
extern GLint uniform_Composition_texture_fs_lookupTable3;  // ping-pong track 2 (FBO)
extern GLint uniform_Composition_texture_fs_lookupTable4;  // previous pass output
extern GLint uniform_Composition_texture_fs_lookupTable5;  // captured frame buffer
extern GLint uniform_Composition_texture_fs_lookupTable_font;  // message Font
extern GLint uniform_Composition_texture_fs_lookupTable_message;  // message string

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
extern GLint uniform_Final_texture_fs_lookupTable2;  // ping-pong BG track (FBO)
extern GLint uniform_Final_texture_fs_lookupTable3;  // ping-pong CA (FBO)
#if PG_NB_TRACKS >= 2
extern GLint uniform_Final_texture_fs_lookupTable4;  // ping-pong track 1 (FBO)
#endif
#if PG_NB_TRACKS >= 3
extern GLint uniform_Final_texture_fs_lookupTable5;  // ping-pong track 2 (FBO)
#endif

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