/*! \file pg-render.h
 * 
 * 
 *     File pg-render.h
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

#ifndef PG_RENDER_H
#define PG_RENDER_H

//////////////////////////////////////////////////////////////////////////////////////////////////////
// EXPORTED VARIABLES
////////////////////////////////////////////////////////////////////////////////////////////////////// 

#if defined(_CRTDBG_MAP_ALLOC)
// MEMORY LEAK CONTROL
extern _CrtMemState s1;
extern _CrtMemState s2;
extern _CrtMemState s3;
extern _CrtMemState s4;
extern _CrtMemState s5;
extern _CrtMemState s6;
extern _CrtMemState s7;
extern _CrtMemState s8;
extern _CrtMemState s9;
extern _CrtMemState s10;
extern _CrtMemState s11;
extern _CrtMemState s12;
extern _CrtMemState s13;
extern _CrtMemState sDiff;
#endif

extern float pg_repop_ColorBGcolorRed;
extern float pg_repop_ColorBG_g;
extern float pg_repop_ColorBG_b;
extern float pg_repop_ColorCA_r;
extern float pg_repop_ColorCA_g;
extern float pg_repop_ColorCA_b;
extern float pg_repop_ColorPart_r;
extern float pg_repop_ColorPart_g;
extern float pg_repop_ColorPart_b;

/////////////////////////////////////////////////////////////////////
// IMAGE FILES
// image index management for progessive image turnover
// according to image swap frequency and duration
// and according to directory size
extern double pg_CurrentDiaporamaEnd;

/////////////////////////////////////////////////////////////////
// Projection and view matrices for the shaders
extern GLfloat pg_orthoWindowProjMatrix[16];
extern GLfloat pg_doubleProjMatrix[16];
extern GLfloat pg_homographyForTexture[9];
extern GLfloat pg_modelMatrixSensor[16];


extern int clip_image_width[PG_MAX_SCENARIOS];
extern int clip_image_height[PG_MAX_SCENARIOS];
extern int clip_crop_width[PG_MAX_SCENARIOS];
extern int clip_crop_height[PG_MAX_SCENARIOS];
extern int clip_max_length[PG_MAX_SCENARIOS];

// pen brushes
extern int nb_pen_brushes[PG_MAX_SCENARIOS];
// textures with multiple layers
extern int nb_layers_master_mask[PG_MAX_SCENARIOS];

/////////////////////////////////////////////////////////////////
// config variables
extern GLuint pg_CATable_ID;

// ++++++++++++++++++++++ MUSIC CONTROL ++++++++++++++++++++
extern float pulse_average;
// +++++++++++++++++++++++ Beats +++++++++++++++++++++++++++++++++
extern float pg_audio_pulse[3];
// music control inputs received at each frame
extern float pg_audio_pulse_average_prec;

//////////////////////////////////////////////////////////////////////////////////////////////////////
// EXPORTED FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////
// geometry: track lines
void pg_window_display(void);
// DRAWING A SCENE ON VARIOUS MODALITIES (CURVE, IMAGE, FRAMEBUFFER...)
void pg_draw_scene(DrawingMode mode);
#endif
