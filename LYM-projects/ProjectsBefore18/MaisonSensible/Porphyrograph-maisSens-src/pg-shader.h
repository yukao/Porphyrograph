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
int pg_loadshader(string filename, GLuint shader);

/////////////////////////////////////////////////////////////////////////
// SHADER PROGRAMMES
extern unsigned int shader_Drawing_programme;
extern unsigned int shader_Composition_programme;
extern unsigned int shader_MeshDisplay_programme;
extern unsigned int shader_Final_programme;

/////////////////////////////////////////////////////////////////////////
// DRAWING SHADER
// DRAWING SHADER UNIFORM VARIABLES
extern GLint uniform_Drawing_vp_model;
extern GLint uniform_Drawing_vp_view;
extern GLint uniform_Drawing_vp_proj;
extern GLint uniform_Drawing_vp_2fv_width_height;
extern GLint uniform_Drawing_fs_4fv_W_H_drawingStart_drawingStroke;
extern GLint uniform_Drawing_fs_4fv_CASubType_partAccCenter_radiusParticle;
extern GLint uniform_Drawing_fs_4fv_partMode_partAcc_partTextNoise_partTexture;
extern GLint uniform_Drawing_fs_4fv_CAdecay_repopulatingImage_repopulating_repopulatingCA;
extern GLint uniform_Drawing_fs_4fv_trackdecay_hidingXYcoef;
extern GLint uniform_Drawing_fs_4fv_CAType_frameno_void_void;
extern GLint uniform_Drawing_fs_4fv_void_void_void_clr;
extern GLint uniform_Drawing_fs_4fv_mouseTracks_xy_xyprev;
extern GLint uniform_Drawing_fs_4fv_mouseTracks_Color_rgba;
extern GLint uniform_Drawing_fs_4fv_mouseTracks_BrushIDRadius_replay;

// DRAWING SHADER TEXTURE IDS
extern GLint uniform_Drawing_texture_fs_decal;         // 3-cycle ping-pong localColor step n-1 (FBO)
extern GLint uniform_Drawing_texture_fs_lookupTable1;  // 3-cycle ping-pong speed/position of particles (FBO)
extern GLint uniform_Drawing_texture_fs_lookupTable2;  // ping-pong CA (FBO)
extern GLint uniform_Drawing_texture_fs_lookupTable5;  // pen patterns
extern GLint uniform_Drawing_texture_fs_lookupTable6;  // particle acceleration texture
extern GLint uniform_Drawing_texture_fs_lookupTable7;  // data tables for the CA
extern GLint uniform_Drawing_texture_fs_lookupTable8;  // noise float texture
extern GLint uniform_Drawing_texture_fs_lookupTable9;  // vessels texture

/////////////////////////////////////////////////////////////////////////
// COMPOSITION SHADER
// COMPOSITION SHADER UNIFORM VARIABLES
extern GLint uniform_Composition_vp_model;
extern GLint uniform_Composition_vp_view;
extern GLint uniform_Composition_vp_proj;
extern GLint uniform_Composition_fs_3fv_width_height_CAweight;
extern GLint uniform_Composition_fs_3fv_trackweight;
extern GLint uniform_Composition_fs_4fv_xy_frameno_cursorSize;
extern GLint uniform_Composition_fs_4fv_mesgTransp_echo_echoNeg_void;
extern GLint uniform_Composition_texture_fs_decal; // ping-pong mouse localColor/gray GOL

// COMPOSITION SHADER TEXTURE IDS
extern GLint uniform_Composition_texture_fs_lookupTable1;  // ping-pong CA (FBO)
extern GLint uniform_Composition_texture_fs_lookupTable2;  // ping-pong track 1 (FBO)
extern GLint uniform_Composition_texture_fs_lookupTable3;  // ping-pong track 2 (FBO)
extern GLint uniform_Composition_texture_fs_lookupTable4;  // previous pass output
//extern GLint uniform_Composition_texture_fs_lookupTable_font;  // message Font
//extern GLint uniform_Composition_texture_fs_lookupTable_message;  // message string

/////////////////////////////////////////////////////////////////////////
// MESH DISPLAY SHADER
// MESH DISPLAY SHADER UNIFORM VARIABLES
extern GLint uniform_MeshDisplay_vp_model;
extern GLint uniform_MeshDisplay_vp_view;
extern GLint uniform_MeshDisplay_vp_projection;
extern GLint uniform_MeshDisplay_vp_2i_FBOwh;
extern GLint uniform_MeshDisplay_fs_2i_FBOwh;
extern GLint uniform_MeshDisplay_fs_4fv_FrameNo_breathing_meshFrontier_X_Width;
extern GLint uniform_MeshDisplay_fs_3fv_blendTransp_isFlatRendering_whiteText;
extern GLint uniform_MeshDisplay_fs_4fv_textFrontier_wmin_wmax_wminWidth_wmaxWidth;
extern GLint uniform_MeshDisplay_fs_4fv_textFrontier_hmin_hmax_hminWidth_hmaxWidth;

// MESH DISPLAY SHADER TEXTURE IDS
extern GLint uniform_MeshDisplay_texture_fs_decal;        // particles
extern GLint uniform_MeshDisplay_fs_4i_isLineDrawing_isLeftCam_isDisplayLookAt_withMeshOnBlueBG;   
// particle vs mesh drawing

/////////////////////////////////////////////////////////////////////////
// FINAL SHADER
// FINAL SHADER UNIFORM VARIABLES
extern GLint uniform_Final_vp_model;
extern GLint uniform_Final_vp_view;
extern GLint uniform_Final_vp_proj;
extern GLint uniform_Final_fs_4fv_isDisplayLookAt_projReversed_wh;
extern GLint uniform_Final_fs_2fv_double_window_proj2Reversed;

// FINAL SHADER TEXTURE IDS
extern GLint uniform_Final_texture_fs_decal;         // previous pass output


#endif
