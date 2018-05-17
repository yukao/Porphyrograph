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

/////////////////////////////////////////////////////////////////////////
// SHADER PROGRAMMES
unsigned int shader_Drawing_programme;
unsigned int shader_Composition_programme;
unsigned int shader_Final_programme;
unsigned int shader_TrackDisplay_programme;

/////////////////////////////////////////////////////////////////////////
// DRAWING SHADER
// DRAWING SHADER UNIFORM VARIABLES
GLint uniform_Drawing_vp_model;
GLint uniform_Drawing_vp_view;
GLint uniform_Drawing_vp_proj;
GLint uniform_Drawing_vp_2fv_width_height;
GLint uniform_Drawing_fs_4fv_W_H_drawingStart_drawingStroke;
GLint uniform_Drawing_fs_4fv_CASubType_partAccCenter_void;
GLint uniform_Drawing_fs_4fv_partMode_partAcc_flashCA_partTexture;
GLint uniform_Drawing_fs_4fv_CAdecay_flashPart_flashTrack0_textureSnapshot;
GLint uniform_Drawing_fs_4fv_trackdecay_CAstep;
GLint uniform_Drawing_fs_4fv_CAType_frameno_flashBack_currentTrack;
GLint uniform_Drawing_fs_2fv_flashBackCoef_clr;
GLint uniform_Drawing_fs_3fv_mouseTracks_replay;
GLint uniform_Drawing_fs_3fv_mouseTracks_x;
GLint uniform_Drawing_fs_3fv_mouseTracks_y;
GLint uniform_Drawing_fs_3iv_mouseTracks_x_transl;
GLint uniform_Drawing_fs_3iv_mouseTracks_y_transl;
GLint uniform_Drawing_fs_3fv_mouseTracks_x_prev;
GLint uniform_Drawing_fs_3fv_mouseTracks_y_prev;
GLint uniform_Drawing_fs_3fv_mouseTracks_Color_r;
GLint uniform_Drawing_fs_3fv_mouseTracks_Color_g;
GLint uniform_Drawing_fs_3fv_mouseTracks_Color_b;
GLint uniform_Drawing_fs_3fv_mouseTracks_Color_a;
GLint uniform_Drawing_fs_3fv_mouseTracks_BrushID;
GLint uniform_Drawing_fs_3fv_mouseTracks_RadiusX;
GLint uniform_Drawing_fs_3fv_mouseTracks_RadiusY;

// DRAWING SHADER TEXTURE IDS
GLint uniform_Drawing_texture_fs_decal;         // 3-cycle ping-pong localColor step n-1 (FBO)
GLint uniform_Drawing_texture_fs_lookupTable1;  // 3-cycle ping-pong speed/position of particles (FBO)
GLint uniform_Drawing_texture_fs_lookupTable2;  // 3-cycle ping-pong localColor step n-2 (FBO)
GLint uniform_Drawing_texture_fs_lookupTable3;  // ping-pong CA (FBO)
GLint uniform_Drawing_texture_fs_lookupTable4;  // ping-pong track 1 (FBO)
GLint uniform_Drawing_texture_fs_lookupTable5;  // ping-pong track 2 (FBO)
GLint uniform_Drawing_texture_fs_lookupTable6;  // pen patterns
GLint uniform_Drawing_texture_fs_lookupTable7;  // particle acceleration texture
GLint uniform_Drawing_texture_fs_lookupTable8;  // particle acceleration FBO

/////////////////////////////////////////////////////////////////////////
// COMPOSITION SHADER
// COMPOSITION SHADER UNIFORM VARIABLES
GLint uniform_Composition_vp_model;
GLint uniform_Composition_vp_view;
GLint uniform_Composition_vp_proj;
GLint uniform_Composition_fs_3fv_width_height_CAweight;
GLint uniform_Composition_fs_3fv_trackweight;
GLint uniform_Composition_fs_3fv_messageTransparency_echo_echoNeg;

// COMPOSITION SHADER TEXTURE IDS
GLint uniform_Composition_texture_fs_decal;         // ping-pong localColor (FBO)
GLint uniform_Composition_texture_fs_lookupTable1;  // ping-pong CA (FBO)
GLint uniform_Composition_texture_fs_lookupTable2;  // ping-pong track 1 (FBO)
GLint uniform_Composition_texture_fs_lookupTable3;  // ping-pong track 2 (FBO)
GLint uniform_Composition_texture_fs_lookupTable4;  // preceding snapshot
GLint uniform_Composition_texture_fs_lookupTable_font;  // message Font
GLint uniform_Composition_texture_fs_lookupTable_message;  // message string

/////////////////////////////////////////////////////////////////////////
// FINAL SHADER
// FINAL SHADER UNIFORM VARIABLES
GLint uniform_Final_vp_model;
GLint uniform_Final_vp_view;
GLint uniform_Final_vp_proj;
GLint uniform_Final_fs_4fv_transparency_invert_leftwidth_rightVMargin;
GLint uniform_Final_fs_4fv_xy_frameno_cursorSize;
GLint uniform_Final_fs_height;

// FINAL SHADER TEXTURE IDS
GLint uniform_Final_texture_fs_decal;         // previous pass output
GLint uniform_Final_texture_fs_lookupTable1;  // previous display tracks output

/////////////////////////////////////////////////////////////////////////
// TRACK DISPLAY SHADER
// TRACK DISPLAY SHADER UNIFORM VARIABLES
GLint uniform_TrackDisplay_vp_4fv_width_FrameNo_noiseAmplitude_globalRadius;
GLint uniform_TrackDisplay_gs_model;
GLint uniform_TrackDisplay_gs_view;
GLint uniform_TrackDisplay_gs_proj;
GLint uniform_TrackDisplay_fs_1fv_transparency;

// TRACK DISPLAY SHADER TEXTURE IDS
GLint uniform_TrackDisplay_texture_vp_lookupTable1; // position
GLint uniform_TrackDisplay_texture_vp_lookupTable2; // color
GLint uniform_TrackDisplay_texture_vp_lookupTable3; // (radius, brush ID and rendering mode)
// GLint uniform_TrackDisplay_texture_vp_lookupTable4; // noise
GLint uniform_TrackDisplay_texture_fs_decal;        // brush

//////////////////////////////////////////////////////////////
// UTILITIES
unsigned long pg_getFileLength(ifstream& file)
{
    if(!file.good()) return 0;
    
    file.seekg(0,ios::end);
    unsigned long len = (long)file.tellg();
    file.seekg(ios::beg);
    
    return len;
}

void pg_printLog(GLuint obj)
{
	int infologLength = 0;
	int maxLength;
 
	if(glIsShader(obj))
		glGetShaderiv(obj,GL_INFO_LOG_LENGTH,&maxLength);
	else
		glGetProgramiv(obj,GL_INFO_LOG_LENGTH,&maxLength);
 
	char *infoLog = new char[maxLength];
 
	if (glIsShader(obj))
		glGetShaderInfoLog(obj, maxLength, &infologLength, infoLog);
	else
		glGetProgramInfoLog(obj, maxLength, &infologLength, infoLog);
 
	if (infologLength > 0)
		printf("%s\n",infoLog);

       delete infoLog;
}


/////////////////////////////////////////////////////////////
// SHADER LOADING
int pg_loadshader(string filename, GLuint shader)
{
  GLchar* shaderSource;
  unsigned long len;

  ifstream file;
  file.open(filename.c_str(), ios::in); // opens as ASCII!
  if(!file) {
    printf( "Error: shader file not found %s!\n" , filename.c_str() ); throw 53;
  }
  
  len = pg_getFileLength(file);
  if (len==0) {
    printf( "Error: empty shader file %s!\n" , filename.c_str() ); throw 53;
  }
  
  shaderSource = new char[len+1];
  if (shaderSource == 0)  {
    printf( "Error: shader string allocation error %s!\n" , filename.c_str() ); throw 53;
  }
  
  printf( "Loading %s\n" , filename.c_str() );

  // len isn't always strlen cause some characters are stripped in ascii read...
  // it is important to 0-terminate the real length later, len is just max possible value... 
  shaderSource[len] = 0; 
  
  unsigned int i=0;
  while (file.good()) {
    shaderSource[i] = file.get();       // get character from file.
    if (!file.eof())
      i++;
  }
  
  shaderSource[i] = 0;  // 0-terminate it at the correct position
  
  file.close();
  
  glShaderSource( shader, 1, (const char **)&shaderSource, NULL );
  
  delete[] shaderSource;
  
  return 0; // No Error
}

void pg_loadAllShaders( void ) {
  ////////////////////////////////////////
  // loading and compiling shaders
  unsigned int pg_vs_Drawing = glCreateShader (GL_VERTEX_SHADER);
  pg_loadshader( (char *)("./ProjectsBefore18/" + project_name+"/LYM_Drawing_"+project_name+"-VP.glsl").c_str() , pg_vs_Drawing);
  glCompileShader (pg_vs_Drawing);
  pg_printLog(pg_vs_Drawing);

  unsigned int pg_vs_Composition = glCreateShader (GL_VERTEX_SHADER);
  pg_loadshader( (char *)("./ProjectsBefore18/" + project_name+"/LYM_Composition_"+project_name+"-VP.glsl").c_str() , pg_vs_Composition);
  glCompileShader (pg_vs_Composition);
  pg_printLog(pg_vs_Composition);

  unsigned int pg_vs_Final = glCreateShader (GL_VERTEX_SHADER);
  pg_loadshader( (char *)("./ProjectsBefore18/" + project_name+"/LYM_Final_"+project_name+"-VP.glsl").c_str() , pg_vs_Final);
  glCompileShader (pg_vs_Final);
  pg_printLog(pg_vs_Final);

  unsigned int pg_vs_TrackDisplay = glCreateShader (GL_VERTEX_SHADER);
  pg_loadshader( (char *)("./ProjectsBefore18/" + project_name+"/LYM_TrackDisplay_"+project_name+"-VP.glsl").c_str() , pg_vs_TrackDisplay);
  glCompileShader (pg_vs_TrackDisplay);
  pg_printLog(pg_vs_TrackDisplay);

  unsigned int pg_gs_TrackDisplay = glCreateShader (GL_GEOMETRY_SHADER);
  pg_loadshader( (char *)("./ProjectsBefore18/" + project_name+"/LYM_TrackDisplay_"+project_name+"-GS.glsl").c_str() , pg_gs_TrackDisplay);
  glCompileShader (pg_gs_TrackDisplay);
  pg_printLog(pg_gs_TrackDisplay);

  unsigned int pg_fs_Drawing = glCreateShader (GL_FRAGMENT_SHADER);
  pg_loadshader( (char *)("./ProjectsBefore18/" + project_name+"/LYM_Drawing_"+project_name+"-FS.glsl").c_str() , pg_fs_Drawing);
  glCompileShader (pg_fs_Drawing);
  pg_printLog(pg_fs_Drawing);

  unsigned int pg_fs_Composition = glCreateShader (GL_FRAGMENT_SHADER);
  pg_loadshader( (char *)("./ProjectsBefore18/" + project_name+"/LYM_Composition_"+project_name+"-FS.glsl").c_str() , pg_fs_Composition);
  glCompileShader (pg_fs_Composition);
  pg_printLog(pg_fs_Composition);

  unsigned int pg_fs_Final = glCreateShader (GL_FRAGMENT_SHADER);
  pg_loadshader( (char *)("./ProjectsBefore18/" + project_name+"/LYM_Final_"+project_name+"-FS.glsl").c_str() , pg_fs_Final);
  glCompileShader (pg_fs_Final);
  pg_printLog(pg_fs_Final);

  unsigned int pg_fs_TrackDisplay = glCreateShader (GL_FRAGMENT_SHADER);
  pg_loadshader( (char *)("./ProjectsBefore18/" + project_name+"/LYM_TrackDisplay_"+project_name+"-FS.glsl").c_str() , pg_fs_TrackDisplay);
  glCompileShader (pg_fs_TrackDisplay);
  pg_printLog(pg_fs_TrackDisplay);

  shader_Drawing_programme = glCreateProgram ();
  glAttachShader (shader_Drawing_programme, pg_fs_Drawing);
  glAttachShader (shader_Drawing_programme, pg_vs_Drawing);
  glLinkProgram (shader_Drawing_programme);

  shader_Composition_programme = glCreateProgram ();
  glAttachShader (shader_Composition_programme, pg_fs_Composition);
  glAttachShader (shader_Composition_programme, pg_vs_Composition);
  glLinkProgram (shader_Composition_programme);

  shader_Final_programme = glCreateProgram ();
  glAttachShader (shader_Final_programme, pg_fs_Final);
  glAttachShader (shader_Final_programme, pg_vs_Final);
  glLinkProgram (shader_Final_programme);

  shader_TrackDisplay_programme = glCreateProgram ();
  glAttachShader (shader_TrackDisplay_programme, pg_fs_TrackDisplay);
  glAttachShader (shader_TrackDisplay_programme, pg_gs_TrackDisplay);
  glAttachShader (shader_TrackDisplay_programme, pg_vs_TrackDisplay);
  glLinkProgram (shader_TrackDisplay_programme);

  ////////////////////////////////////////
  // shader parameeter bindings
  uniform_Drawing_vp_model 
    = glGetUniformLocation(shader_Drawing_programme, "vp_modelMatrix");
  uniform_Drawing_vp_view 
    = glGetUniformLocation(shader_Drawing_programme, "vp_viewMatrix");
  uniform_Drawing_vp_proj 
    = glGetUniformLocation(shader_Drawing_programme, "vp_projMatrix");

  uniform_Drawing_vp_2fv_width_height 
    = glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_vp_2fv_width_height");
  uniform_Drawing_fs_4fv_W_H_drawingStart_drawingStroke 
    = glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_fs_4fv_W_H_drawingStart_drawingStroke");
  uniform_Drawing_fs_4fv_CASubType_partAccCenter_void 
    = glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_fs_4fv_CASubType_partAccCenter_void");
  uniform_Drawing_fs_4fv_partMode_partAcc_flashCA_partTexture 
    = glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_fs_4fv_partMode_partAcc_flashCA_partTexture");
  uniform_Drawing_fs_4fv_CAdecay_flashPart_flashTrack0_textureSnapshot 
    = glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_fs_4fv_CAdecay_flashPart_flashTrack0_textureSnapshot");
  uniform_Drawing_fs_4fv_trackdecay_CAstep 
    = glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_fs_4fv_trackdecay_CAstep");
  uniform_Drawing_fs_4fv_CAType_frameno_flashBack_currentTrack 
    = glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_fs_4fv_CAType_frameno_flashBack_currentTrack");
  uniform_Drawing_fs_2fv_flashBackCoef_clr
	= glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_fs_2fv_flashBackCoef_clr");
  uniform_Drawing_fs_3fv_mouseTracks_replay 
    = glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_fs_3fv_mouseTracks_replay");
  uniform_Drawing_fs_3fv_mouseTracks_x 
    = glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_fs_3fv_mouseTracks_x");
  uniform_Drawing_fs_3fv_mouseTracks_y 
    = glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_fs_3fv_mouseTracks_y");
  uniform_Drawing_fs_3iv_mouseTracks_x_transl 
    = glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_fs_3iv_mouseTracks_x_transl");
  uniform_Drawing_fs_3iv_mouseTracks_y_transl 
    = glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_fs_3iv_mouseTracks_y_transl");
  uniform_Drawing_fs_3fv_mouseTracks_x_prev 
    = glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_fs_3fv_mouseTracks_x_prev");
  uniform_Drawing_fs_3fv_mouseTracks_y_prev 
    = glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_fs_3fv_mouseTracks_y_prev");
  uniform_Drawing_fs_3fv_mouseTracks_Color_r 
    = glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_fs_3fv_mouseTracks_Color_r");
  uniform_Drawing_fs_3fv_mouseTracks_Color_g 
    = glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_fs_3fv_mouseTracks_Color_g");
  uniform_Drawing_fs_3fv_mouseTracks_Color_b 
    = glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_fs_3fv_mouseTracks_Color_b");
  uniform_Drawing_fs_3fv_mouseTracks_Color_a 
    = glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_fs_3fv_mouseTracks_Color_a");
  uniform_Drawing_fs_3fv_mouseTracks_BrushID 
    = glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_fs_3fv_mouseTracks_BrushID");
  uniform_Drawing_fs_3fv_mouseTracks_RadiusX 
    = glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_fs_3fv_mouseTracks_RadiusX");
  uniform_Drawing_fs_3fv_mouseTracks_RadiusY 
    = glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_fs_3fv_mouseTracks_RadiusY");

  // color FBO
  uniform_Drawing_texture_fs_decal 
    = glGetUniformLocation(shader_Drawing_programme, "fs_decal" );         // 3-cycle ping-pong localColor step n-1 (FBO)
  // particle FBO
  uniform_Drawing_texture_fs_lookupTable1 
    = glGetUniformLocation(shader_Drawing_programme, "fs_lookupTable1" );  // 3-cycle ping-pong speed/position of particles (FBO)
  // previous color FBO
  uniform_Drawing_texture_fs_lookupTable2 
    = glGetUniformLocation(shader_Drawing_programme, "fs_lookupTable2" );  // 3-cycle ping-pong localColor step n-2 (FBO)
  // CA FBO
  uniform_Drawing_texture_fs_lookupTable3 
    = glGetUniformLocation(shader_Drawing_programme, "fs_lookupTable3" );  // ping-pong CA (FBO)
  // track FBO
  uniform_Drawing_texture_fs_lookupTable4 
    = glGetUniformLocation(shader_Drawing_programme, "fs_lookupTable4" );  // ping-pong track 1 (FBO)
  uniform_Drawing_texture_fs_lookupTable5 
    = glGetUniformLocation(shader_Drawing_programme, "fs_lookupTable5" );  // ping-pong track 2 (FBO)
  // textures
  uniform_Drawing_texture_fs_lookupTable6 
    = glGetUniformLocation(shader_Drawing_programme, "fs_lookupTable6" );  // pen patterns
  uniform_Drawing_texture_fs_lookupTable7 
    = glGetUniformLocation(shader_Drawing_programme, "fs_lookupTable7" );  // particle acceleration texture
  uniform_Drawing_texture_fs_lookupTable8 
    = glGetUniformLocation(shader_Drawing_programme, "fs_lookupTable8" );  // particle acceleration FBO

  if( (uniform_Drawing_vp_model == -1)
      || (uniform_Drawing_vp_view == -1)
      || (uniform_Drawing_vp_proj == -1)
      || (uniform_Drawing_vp_2fv_width_height == -1)

      || (uniform_Drawing_fs_4fv_W_H_drawingStart_drawingStroke == -1)
      || (uniform_Drawing_fs_4fv_CASubType_partAccCenter_void == -1)
      || (uniform_Drawing_fs_4fv_partMode_partAcc_flashCA_partTexture == -1)
      || (uniform_Drawing_fs_4fv_CAdecay_flashPart_flashTrack0_textureSnapshot == -1)
      || (uniform_Drawing_fs_4fv_trackdecay_CAstep == -1)
      || (uniform_Drawing_fs_4fv_CAType_frameno_flashBack_currentTrack == -1)
      || (uniform_Drawing_fs_2fv_flashBackCoef_clr == -1)
      || (uniform_Drawing_fs_3fv_mouseTracks_replay == -1)
      || (uniform_Drawing_fs_3fv_mouseTracks_x == -1)
      || (uniform_Drawing_fs_3fv_mouseTracks_y == -1)
      || (uniform_Drawing_fs_3iv_mouseTracks_x_transl == -1)
      || (uniform_Drawing_fs_3iv_mouseTracks_y_transl == -1)
      || (uniform_Drawing_fs_3fv_mouseTracks_x_prev == -1)
      || (uniform_Drawing_fs_3fv_mouseTracks_y_prev == -1)
      || (uniform_Drawing_fs_3fv_mouseTracks_Color_r == -1)
      || (uniform_Drawing_fs_3fv_mouseTracks_Color_g == -1)
      || (uniform_Drawing_fs_3fv_mouseTracks_Color_b == -1)
      || (uniform_Drawing_fs_3fv_mouseTracks_Color_a == -1)
      || (uniform_Drawing_fs_3fv_mouseTracks_BrushID == -1)
      || (uniform_Drawing_fs_3fv_mouseTracks_RadiusX == -1)
     || (uniform_Drawing_fs_3fv_mouseTracks_RadiusY == -1)

      || (uniform_Drawing_texture_fs_decal == -1)
      || (uniform_Drawing_texture_fs_lookupTable1 == -1)
      || (uniform_Drawing_texture_fs_lookupTable2 == -1)
      || (uniform_Drawing_texture_fs_lookupTable3 == -1)
      || (uniform_Drawing_texture_fs_lookupTable4 == -1)
      || (uniform_Drawing_texture_fs_lookupTable5 == -1)
      || (uniform_Drawing_texture_fs_lookupTable6 == -1)
      || (uniform_Drawing_texture_fs_lookupTable7 == -1)
      || (uniform_Drawing_texture_fs_lookupTable8 == -1) 
      ) {
	  fprintf(stderr, "Could not bind uniforms Drawing uniform_Drawing_vp_model : %d, uniform_Drawing_vp_view : %d, uniform_Drawing_vp_proj : %d, uniform_Drawing_vp_2fv_width_height : %d, uniform_Drawing_fs_4fv_W_H_drawingStart_drawingStroke : %d, uniform_Drawing_fs_4fv_CASubType_partAccCenter_void : %d, uniform_Drawing_fs_4fv_partMode_partAcc_flashCA_partTexture : %d, uniform_Drawing_fs_4fv_CAdecay_flashPart_flashTrack0_textureSnapshot : %d, uniform_Drawing_fs_4fv_trackdecay_CAstep : %d, uniform_Drawing_fs_4fv_CAType_frameno_flashBack_currentTrack : %d, uniform_Drawing_fs_2fv_flashBackCoef_clr : %d, uniform_Drawing_fs_3fv_mouseTracks_replay : %d, uniform_Drawing_fs_3fv_mouseTracks_x : %d, uniform_Drawing_fs_3fv_mouseTracks_y : %d, uniform_Drawing_fs_3iv_mouseTracks_x_transl : %d, uniform_Drawing_fs_3iv_mouseTracks_y_transl : %d, uniform_Drawing_fs_3fv_mouseTracks_x_prev : %d, uniform_Drawing_fs_3fv_mouseTracks_y_prev : %d, uniform_Drawing_fs_3fv_mouseTracks_Color_r : %d, uniform_Drawing_fs_3fv_mouseTracks_Color_g : %d, uniform_Drawing_fs_3fv_mouseTracks_Color_b : %d, uniform_Drawing_fs_3fv_mouseTracks_Color_a : %d, uniform_Drawing_fs_3fv_mouseTracks_BrushID : %d, uniform_Drawing_fs_3fv_mouseTracks_RadiusX : %d, uniform_Drawing_fs_3fv_mouseTracks_RadiusY : %d, uniform_Drawing_texture_fs_decal : %d, uniform_Drawing_texture_fs_lookupTable1 : %d, uniform_Drawing_texture_fs_lookupTable2 : %d, uniform_Drawing_texture_fs_lookupTable3 : %d, uniform_Drawing_texture_fs_lookupTable4 : %d, uniform_Drawing_texture_fs_lookupTable5 : %d, uniform_Drawing_texture_fs_lookupTable6 : %d, uniform_Drawing_texture_fs_lookupTable7 : %d, uniform_Drawing_texture_fs_lookupTable8 : %d\n", uniform_Drawing_vp_model, uniform_Drawing_vp_view, uniform_Drawing_vp_proj, uniform_Drawing_vp_2fv_width_height, uniform_Drawing_fs_4fv_W_H_drawingStart_drawingStroke, uniform_Drawing_fs_4fv_CASubType_partAccCenter_void, uniform_Drawing_fs_4fv_partMode_partAcc_flashCA_partTexture, uniform_Drawing_fs_4fv_CAdecay_flashPart_flashTrack0_textureSnapshot, uniform_Drawing_fs_4fv_trackdecay_CAstep, uniform_Drawing_fs_4fv_CAType_frameno_flashBack_currentTrack, uniform_Drawing_fs_2fv_flashBackCoef_clr , uniform_Drawing_fs_3fv_mouseTracks_replay, uniform_Drawing_fs_3fv_mouseTracks_x, uniform_Drawing_fs_3fv_mouseTracks_y, uniform_Drawing_fs_3iv_mouseTracks_x_transl, uniform_Drawing_fs_3iv_mouseTracks_y_transl, uniform_Drawing_fs_3fv_mouseTracks_x_prev, uniform_Drawing_fs_3fv_mouseTracks_y_prev, uniform_Drawing_fs_3fv_mouseTracks_Color_r, uniform_Drawing_fs_3fv_mouseTracks_Color_g, uniform_Drawing_fs_3fv_mouseTracks_Color_b, uniform_Drawing_fs_3fv_mouseTracks_Color_a, uniform_Drawing_fs_3fv_mouseTracks_BrushID, uniform_Drawing_fs_3fv_mouseTracks_RadiusX, uniform_Drawing_fs_3fv_mouseTracks_RadiusY, uniform_Drawing_texture_fs_decal, uniform_Drawing_texture_fs_lookupTable1, uniform_Drawing_texture_fs_lookupTable2, uniform_Drawing_texture_fs_lookupTable3, uniform_Drawing_texture_fs_lookupTable4, uniform_Drawing_texture_fs_lookupTable5, uniform_Drawing_texture_fs_lookupTable6, uniform_Drawing_texture_fs_lookupTable7, uniform_Drawing_texture_fs_lookupTable8);
    // return 0;
  }

  ////////////////////////////////////////
  // shader parameeter bindings
  uniform_Composition_vp_model 
    = glGetUniformLocation(shader_Composition_programme, "vp_modelMatrix");
  uniform_Composition_vp_view 
    = glGetUniformLocation(shader_Composition_programme, "vp_viewMatrix");
  uniform_Composition_vp_proj 
    = glGetUniformLocation(shader_Composition_programme, "vp_projMatrix");

  uniform_Composition_fs_3fv_width_height_CAweight 
    = glGetUniformLocation(shader_Composition_programme, "uniform_Composition_fs_3fv_width_height_CAweight");
  uniform_Composition_fs_3fv_trackweight 
    = glGetUniformLocation(shader_Composition_programme, "uniform_Composition_fs_3fv_trackweight");
  uniform_Composition_fs_3fv_messageTransparency_echo_echoNeg 
    = glGetUniformLocation(shader_Composition_programme, "uniform_Composition_fs_3fv_messageTransparency_echo_echoNeg");

  uniform_Composition_texture_fs_decal 
    = glGetUniformLocation(shader_Composition_programme, "fs_decal" ); // ping-pong mouse localColor/gray GOL
  uniform_Composition_texture_fs_lookupTable1 
    = glGetUniformLocation(shader_Composition_programme, "fs_lookupTable1" );  // ping-pong CA (FBO)
  uniform_Composition_texture_fs_lookupTable2 
    = glGetUniformLocation(shader_Composition_programme, "fs_lookupTable2" );  // ping-pong track 1 (FBO)
  uniform_Composition_texture_fs_lookupTable3 
    = glGetUniformLocation(shader_Composition_programme, "fs_lookupTable3" );  // ping-pong track 2 (FBO)
  uniform_Composition_texture_fs_lookupTable4 
    = glGetUniformLocation(shader_Composition_programme, "fs_lookupTable4" );  // 
  uniform_Composition_texture_fs_lookupTable_font 
    = glGetUniformLocation(shader_Composition_programme, "fs_lookupTable_font" );  // message Font
  uniform_Composition_texture_fs_lookupTable_message 
    = glGetUniformLocation(shader_Composition_programme, "fs_lookupTable_message" );  // message string
  if ( (uniform_Composition_vp_proj == -1) 
       || (uniform_Composition_vp_view == -1) 
       || (uniform_Composition_vp_model == -1) 
       || (uniform_Composition_fs_3fv_width_height_CAweight == -1)
       || (uniform_Composition_fs_3fv_trackweight == -1)
       || (uniform_Composition_fs_3fv_messageTransparency_echo_echoNeg == -1)
       || (uniform_Composition_texture_fs_decal == -1)
       || (uniform_Composition_texture_fs_lookupTable1 == -1)
       || (uniform_Composition_texture_fs_lookupTable2 == -1)
       || (uniform_Composition_texture_fs_lookupTable3 == -1)
       || (uniform_Composition_texture_fs_lookupTable4 == -1)
       || (uniform_Composition_texture_fs_lookupTable_font == -1)
       || (uniform_Composition_texture_fs_lookupTable_message == -1)
       ) {
    fprintf(stderr, "Could not bind uniforms Composition uniform_Composition_vp_proj : %d, uniform_Composition_vp_view : %d, uniform_Composition_vp_model : %d, uniform_Composition_fs_3fv_width_height_CAweight : %d, uniform_Composition_fs_3fv_trackweight : %d, uniform_Composition_fs_3fv_messageTransparency_echo_echoNeg : %d, uniform_Composition_texture_fs_decal : %d, uniform_Composition_texture_fs_lookupTable1 : %d, uniform_Composition_texture_fs_lookupTable2 : %d, uniform_Composition_texture_fs_lookupTable3 : %d, uniform_Composition_texture_fs_lookupTable4 : %d, uniform_Composition_texture_fs_lookupTable_font : %d, uniform_Composition_texture_fs_lookupTable_message : %d\n" , uniform_Composition_vp_proj , uniform_Composition_vp_view , uniform_Composition_vp_model , uniform_Composition_fs_3fv_width_height_CAweight , uniform_Composition_fs_3fv_trackweight , uniform_Composition_fs_3fv_messageTransparency_echo_echoNeg , uniform_Composition_texture_fs_decal , uniform_Composition_texture_fs_lookupTable1 , uniform_Composition_texture_fs_lookupTable2 , uniform_Composition_texture_fs_lookupTable3 , uniform_Composition_texture_fs_lookupTable4 , uniform_Composition_texture_fs_lookupTable_font , uniform_Composition_texture_fs_lookupTable_message);
    // return 0;
  }
  
  ////////////////////////////////////////
  // shader parameeter bindings
  uniform_Final_vp_model 
    = glGetUniformLocation(shader_Final_programme, "vp_modelMatrix");
  uniform_Final_vp_view 
    = glGetUniformLocation(shader_Final_programme, "vp_viewMatrix");
  uniform_Final_vp_proj 
    = glGetUniformLocation(shader_Final_programme, "vp_projMatrix");
  uniform_Final_fs_4fv_transparency_invert_leftwidth_rightVMargin
    = glGetUniformLocation(shader_Final_programme, "uniform_Final_fs_4fv_transparency_invert_leftwidth_rightVMargin");
  uniform_Final_fs_4fv_xy_frameno_cursorSize 
    = glGetUniformLocation(shader_Final_programme, "uniform_Final_fs_4fv_xy_frameno_cursorSize");
  uniform_Final_fs_height 
    = glGetUniformLocation(shader_Final_programme, "uniform_Final_fs_height");

  uniform_Final_texture_fs_decal 
    = glGetUniformLocation(shader_Final_programme, "fs_decal" ); // previous pass output
  uniform_Final_texture_fs_lookupTable1 
    = glGetUniformLocation(shader_Final_programme, "fs_lookupTable1" ); // previous pass output

  if ( (uniform_Final_vp_proj == -1) 
       || (uniform_Final_vp_view == -1) 
       || (uniform_Final_vp_model == -1) 
       || (uniform_Final_fs_4fv_transparency_invert_leftwidth_rightVMargin == -1)
       || (uniform_Final_fs_4fv_xy_frameno_cursorSize == -1)
       || (uniform_Final_fs_height == -1)
       || (uniform_Final_texture_fs_decal == -1)
       || (uniform_Final_texture_fs_lookupTable1 == -1)
       ) {
    fprintf(stderr, "Could not bind uniforms Final uniform_Final_vp_proj : %d, uniform_Final_vp_view : %d, uniform_Final_vp_model : %d, uniform_Final_fs_4fv_transparency_invert_leftwidth_rightVMargin : %d , uniform_Final_fs_4fv_xy_frameno_cursorSize : %d, uniform_Final_fs_height : %d , uniform_Final_texture_fs_lookupTable1: %d\n" , uniform_Final_vp_proj , uniform_Final_vp_view , uniform_Final_vp_model , uniform_Final_fs_4fv_transparency_invert_leftwidth_rightVMargin , uniform_Final_fs_4fv_xy_frameno_cursorSize , uniform_Final_fs_height , uniform_Final_texture_fs_lookupTable1 );
    // return 0;
  }

  ////////////////////////////////////////
  // shader parameeter bindings
  uniform_TrackDisplay_gs_model 
    = glGetUniformLocation(shader_TrackDisplay_programme, "gs_modelMatrix");
  uniform_TrackDisplay_gs_view 
    = glGetUniformLocation(shader_TrackDisplay_programme, "gs_viewMatrix");
  uniform_TrackDisplay_gs_proj 
    = glGetUniformLocation(shader_TrackDisplay_programme, "gs_projMatrix");
  uniform_TrackDisplay_fs_1fv_transparency
    = glGetUniformLocation(shader_TrackDisplay_programme, "uniform_TrackDisplay_fs_1fv_transparency");


  uniform_TrackDisplay_texture_fs_decal 
    = glGetUniformLocation(shader_TrackDisplay_programme, "fs_decal" ); // brush
  uniform_TrackDisplay_texture_vp_lookupTable1 
    = glGetUniformLocation(shader_TrackDisplay_programme, "vp_lookupTable1" ); // position
  uniform_TrackDisplay_texture_vp_lookupTable2 
    = glGetUniformLocation(shader_TrackDisplay_programme, "vp_lookupTable2" ); // color
  uniform_TrackDisplay_texture_vp_lookupTable3 
    = glGetUniformLocation(shader_TrackDisplay_programme, "vp_lookupTable3" ); // (radius, brush ID and rendering mode)
  // uniform_TrackDisplay_texture_vp_lookupTable4 
    // = glGetUniformLocation(shader_TrackDisplay_programme, "vp_lookupTable4" ); // noise

  uniform_TrackDisplay_vp_4fv_width_FrameNo_noiseAmplitude_globalRadius 
    = glGetUniformLocation(shader_TrackDisplay_programme, "uniform_TrackDisplay_vp_4fv_width_FrameNo_noiseAmplitude_globalRadius");
  if ( (uniform_TrackDisplay_gs_proj == -1) 
       || (uniform_TrackDisplay_gs_view == -1) 
       || (uniform_TrackDisplay_gs_model == -1) 
       || (uniform_TrackDisplay_texture_fs_decal == -1)
       || (uniform_TrackDisplay_texture_vp_lookupTable1 == -1)
       || (uniform_TrackDisplay_texture_vp_lookupTable2 == -1)
       || (uniform_TrackDisplay_texture_vp_lookupTable3 == -1)
       // || (uniform_TrackDisplay_texture_vp_lookupTable4 == -1)
       || (uniform_TrackDisplay_vp_4fv_width_FrameNo_noiseAmplitude_globalRadius == -1)
       || (uniform_TrackDisplay_fs_1fv_transparency == -1)
       ) {
    fprintf(stderr, "Could not bind uniforms TrackDisplay uniform_TrackDisplay_gs_proj : %d, uniform_TrackDisplay_gs_view : %d, uniform_TrackDisplay_gs_model : %d, uniform_TrackDisplay_texture_fs_decal : %d, uniform_TrackDisplay_texture_vp_lookupTable1 : %d, uniform_TrackDisplay_texture_vp_lookupTable2 : %d, uniform_TrackDisplay_texture_vp_lookupTable3 : %d, uniform_TrackDisplay_vp_4fv_width_FrameNo_noiseAmplitude_globalRadius : %d , uniform_TrackDisplay_fs_1fv_transparency: %d\n" , uniform_TrackDisplay_gs_proj , uniform_TrackDisplay_gs_view , uniform_TrackDisplay_gs_model , uniform_TrackDisplay_texture_fs_decal  , uniform_TrackDisplay_texture_vp_lookupTable1 , uniform_TrackDisplay_texture_vp_lookupTable2 , uniform_TrackDisplay_texture_vp_lookupTable3 , uniform_TrackDisplay_vp_4fv_width_FrameNo_noiseAmplitude_globalRadius , uniform_TrackDisplay_fs_1fv_transparency );
    // return 0;
  }
}

