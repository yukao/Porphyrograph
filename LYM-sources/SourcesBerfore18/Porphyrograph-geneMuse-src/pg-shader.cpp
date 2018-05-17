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
unsigned int shader_Sensor_programme;

/////////////////////////////////////////////////////////////////////////
// DRAWING SHADER
// DRAWING SHADER UNIFORM VARIABLES
GLint uniform_Drawing_vp_model;
GLint uniform_Drawing_vp_view;
GLint uniform_Drawing_vp_proj;
GLint uniform_Drawing_vp_2fv_width_height;
GLint uniform_Drawing_fs_4fv_W_H_drawingStart_drawingStroke;
GLint uniform_Drawing_fs_4fv_CAType_CASubType_partAccCenter;
GLint uniform_Drawing_fs_4fv_partMode_partAcc_clearLayer_void;
GLint uniform_Drawing_fs_4fv_flashPart_isBeat_void_clearCA;
GLint uniform_Drawing_fs_3fv_trackdecay_CAstep_initCA;
GLint uniform_Drawing_fs_4fv_CAdecay_frameno_partTexture_currentTrack;
GLint uniform_Drawing_fs_4fv_flashBackCoef_flashCACoefs;
GLint uniform_Drawing_fs_3fv_mouseTracks_replay;
GLint uniform_Drawing_fs_4fv_mouseTracks_x;
GLint uniform_Drawing_fs_4fv_mouseTracks_y;
GLint uniform_Drawing_fs_4fv_mouseTracks_x_prev;
GLint uniform_Drawing_fs_4fv_mouseTracks_y_prev;
GLint uniform_Drawing_fs_3fv_mouseTracks_Color_r;
GLint uniform_Drawing_fs_3fv_mouseTracks_Color_g;
GLint uniform_Drawing_fs_3fv_mouseTracks_Color_b;
GLint uniform_Drawing_fs_3fv_mouseTracks_Color_a;
GLint uniform_Drawing_fs_3fv_mouseTracks_BrushID;
GLint uniform_Drawing_fs_4fv_mouseTracks_RadiusX;
GLint uniform_Drawing_fs_4fv_mouseTracks_RadiusY;

// DRAWING SHADER TEXTURE IDS
GLint uniform_Drawing_texture_fs_decal;         // 3-cycle ping-pong localColor step n-1 (FBO)
GLint uniform_Drawing_texture_fs_lookupTable1;  // 3-cycle ping-pong speed/position of particles (FBO)
GLint uniform_Drawing_texture_fs_lookupTable2;  // 3-cycle ping-pong localColor step n-2 (FBO)
GLint uniform_Drawing_texture_fs_lookupTable3;  // ping-pong CA (FBO)
GLint uniform_Drawing_texture_fs_lookupTable4;  // pen patterns
GLint uniform_Drawing_texture_fs_lookupTable5;  // particle acceleration texture
GLint uniform_Drawing_texture_fs_lookupTable6;  // data tables for the CA

/////////////////////////////////////////////////////////////////////////
// COMPOSITION SHADER
// COMPOSITION SHADER UNIFORM VARIABLES
GLint uniform_Composition_vp_model;
GLint uniform_Composition_vp_view;
GLint uniform_Composition_vp_proj;
GLint uniform_Composition_fs_3fv_width_height_CAweight;
GLint uniform_Composition_fs_3fv_trackweight;
GLint uniform_Composition_fs_4fv_messageTransparency_echo_echoNeg_invert;

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
GLint uniform_Final_fs_2fv_transparency_scale;
GLint uniform_Final_fs_4fv_xy_frameno_cursorSize;
GLint uniform_Final_fs_2fv_tablet1xy;

// FINAL SHADER TEXTURE IDS
GLint uniform_Final_texture_fs_decal;         // previous pass output
GLint uniform_Final_texture_fs_lookupTable1;         // LYM logo

/////////////////////////////////////////////////////////////////////////
// SENSOR SHADER
// SENSOR SHADER UNIFORM VARIABLES
GLint uniform_Sensor_vp_model;
GLint uniform_Sensor_vp_view;
GLint uniform_Sensor_vp_proj;

// SENSOR SHADER TEXTURE IDS
GLint uniform_Sensor_texture_fs_decal;         // sensor texture
GLint uniform_Sensor_fs_4fv_onOff_isCurrentSensor_isFollowMouse_transparency;
// GLint uniform_Sensor_fs_2fv_frameno_invert;

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
  pg_loadshader( (char *)(project_name+"/LYM_Drawing_"+project_name+"-VP.glsl").c_str() , pg_vs_Drawing);
  glCompileShader (pg_vs_Drawing);
  pg_printLog(pg_vs_Drawing);

  unsigned int pg_vs_Composition = glCreateShader (GL_VERTEX_SHADER);
  pg_loadshader( (char *)(project_name+"/LYM_Composition_"+project_name+"-VP.glsl").c_str() , pg_vs_Composition);
  glCompileShader (pg_vs_Composition);
  pg_printLog(pg_vs_Composition);

  unsigned int pg_vs_Final = glCreateShader (GL_VERTEX_SHADER);
  pg_loadshader( (char *)(project_name+"/LYM_Final_"+project_name+"-VP.glsl").c_str() , pg_vs_Final);
  glCompileShader (pg_vs_Final);
  pg_printLog(pg_vs_Final);

  unsigned int pg_vs_Sensor = glCreateShader (GL_VERTEX_SHADER);
  pg_loadshader( (char *)(project_name+"/LYM_Sensor_"+project_name+"-VP.glsl").c_str() , pg_vs_Sensor);
  glCompileShader (pg_vs_Sensor);
  pg_printLog(pg_vs_Sensor);

  unsigned int pg_fs_Drawing = glCreateShader (GL_FRAGMENT_SHADER);
  pg_loadshader( (char *)(project_name+"/LYM_Drawing_"+project_name+"-FS.glsl").c_str() , pg_fs_Drawing);
  glCompileShader (pg_fs_Drawing);
  pg_printLog(pg_fs_Drawing);

  unsigned int pg_fs_Composition = glCreateShader (GL_FRAGMENT_SHADER);
  pg_loadshader( (char *)(project_name+"/LYM_Composition_"+project_name+"-FS.glsl").c_str() , pg_fs_Composition);
  glCompileShader (pg_fs_Composition);
  pg_printLog(pg_fs_Composition);

  unsigned int pg_fs_Final = glCreateShader (GL_FRAGMENT_SHADER);
  pg_loadshader( (char *)(project_name+"/LYM_Final_"+project_name+"-FS.glsl").c_str() , pg_fs_Final);
  glCompileShader (pg_fs_Final);
  pg_printLog(pg_fs_Final);

  unsigned int pg_fs_Sensor = glCreateShader (GL_FRAGMENT_SHADER);
  pg_loadshader( (char *)(project_name+"/LYM_Sensor_"+project_name+"-FS.glsl").c_str() , pg_fs_Sensor);
  glCompileShader (pg_fs_Sensor);
  pg_printLog(pg_fs_Sensor);

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

  shader_Sensor_programme = glCreateProgram ();
  glAttachShader (shader_Sensor_programme, pg_fs_Sensor);
  glAttachShader (shader_Sensor_programme, pg_vs_Sensor);
  glLinkProgram (shader_Sensor_programme);

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
  uniform_Drawing_fs_4fv_CAType_CASubType_partAccCenter 
    = glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_fs_4fv_CAType_CASubType_partAccCenter");
  uniform_Drawing_fs_4fv_partMode_partAcc_clearLayer_void 
    = glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_fs_4fv_partMode_partAcc_clearLayer_void");
  uniform_Drawing_fs_4fv_flashPart_isBeat_void_clearCA 
    = glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_fs_4fv_flashPart_isBeat_void_clearCA");
  uniform_Drawing_fs_3fv_trackdecay_CAstep_initCA 
    = glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_fs_3fv_trackdecay_CAstep_initCA");
  uniform_Drawing_fs_4fv_CAdecay_frameno_partTexture_currentTrack 
    = glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_fs_4fv_CAdecay_frameno_partTexture_currentTrack");
  uniform_Drawing_fs_4fv_flashBackCoef_flashCACoefs
	= glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_fs_4fv_flashBackCoef_flashCACoefs");
  uniform_Drawing_fs_3fv_mouseTracks_replay 
    = glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_fs_3fv_mouseTracks_replay");
  uniform_Drawing_fs_4fv_mouseTracks_x 
    = glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_fs_4fv_mouseTracks_x");
  uniform_Drawing_fs_4fv_mouseTracks_y 
    = glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_fs_4fv_mouseTracks_y");
  uniform_Drawing_fs_4fv_mouseTracks_x_prev 
    = glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_fs_4fv_mouseTracks_x_prev");
  uniform_Drawing_fs_4fv_mouseTracks_y_prev 
    = glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_fs_4fv_mouseTracks_y_prev");
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
  uniform_Drawing_fs_4fv_mouseTracks_RadiusX 
    = glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_fs_4fv_mouseTracks_RadiusX");
  uniform_Drawing_fs_4fv_mouseTracks_RadiusY 
    = glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_fs_4fv_mouseTracks_RadiusY");

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

  if( (uniform_Drawing_vp_model == -1)
      || (uniform_Drawing_vp_view == -1)
      || (uniform_Drawing_vp_proj == -1)
      || (uniform_Drawing_vp_2fv_width_height == -1)

      || (uniform_Drawing_fs_4fv_W_H_drawingStart_drawingStroke == -1)
      || (uniform_Drawing_fs_4fv_CAType_CASubType_partAccCenter == -1)
      || (uniform_Drawing_fs_4fv_partMode_partAcc_clearLayer_void == -1)
      || (uniform_Drawing_fs_4fv_flashPart_isBeat_void_clearCA == -1)
      || (uniform_Drawing_fs_3fv_trackdecay_CAstep_initCA == -1)
      || (uniform_Drawing_fs_4fv_CAdecay_frameno_partTexture_currentTrack == -1)
      || (uniform_Drawing_fs_4fv_flashBackCoef_flashCACoefs == -1)
      || (uniform_Drawing_fs_3fv_mouseTracks_replay == -1)
      || (uniform_Drawing_fs_4fv_mouseTracks_x == -1)
      || (uniform_Drawing_fs_4fv_mouseTracks_y == -1)
      || (uniform_Drawing_fs_4fv_mouseTracks_x_prev == -1)
      || (uniform_Drawing_fs_4fv_mouseTracks_y_prev == -1)
      || (uniform_Drawing_fs_3fv_mouseTracks_Color_r == -1)
      || (uniform_Drawing_fs_3fv_mouseTracks_Color_g == -1)
      || (uniform_Drawing_fs_3fv_mouseTracks_Color_b == -1)
      || (uniform_Drawing_fs_3fv_mouseTracks_Color_a == -1)
      || (uniform_Drawing_fs_3fv_mouseTracks_BrushID == -1)
      || (uniform_Drawing_fs_4fv_mouseTracks_RadiusX == -1)
     || (uniform_Drawing_fs_4fv_mouseTracks_RadiusY == -1)

      || (uniform_Drawing_texture_fs_decal == -1)
      || (uniform_Drawing_texture_fs_lookupTable1 == -1)
      || (uniform_Drawing_texture_fs_lookupTable2 == -1)
      || (uniform_Drawing_texture_fs_lookupTable3 == -1)
      || (uniform_Drawing_texture_fs_lookupTable4 == -1)
      || (uniform_Drawing_texture_fs_lookupTable5 == -1)
      || (uniform_Drawing_texture_fs_lookupTable6 == -1)
      ) {
	  fprintf(stderr, "Could not bind uniforms Drawing uniform_Drawing_vp_model : %d, uniform_Drawing_vp_view : %d, uniform_Drawing_vp_proj : %d, uniform_Drawing_vp_2fv_width_height : %d, uniform_Drawing_fs_4fv_W_H_drawingStart_drawingStroke : %d, uniform_Drawing_fs_4fv_CAType_CASubType_partAccCenter : %d, uniform_Drawing_fs_4fv_partMode_partAcc_clearLayer_void : %d, uniform_Drawing_fs_4fv_flashPart_isBeat_void_clearCA : %d, uniform_Drawing_fs_3fv_trackdecay_CAstep_initCA : %d, uniform_Drawing_fs_4fv_CAdecay_frameno_partTexture_currentTrack : %d, uniform_Drawing_fs_4fv_flashBackCoef_flashCACoefs : %d, uniform_Drawing_fs_3fv_mouseTracks_replay : %d, uniform_Drawing_fs_4fv_mouseTracks_x : %d, uniform_Drawing_fs_4fv_mouseTracks_y : %d, uniform_Drawing_fs_4fv_mouseTracks_x_prev : %d, uniform_Drawing_fs_4fv_mouseTracks_y_prev : %d, uniform_Drawing_fs_3fv_mouseTracks_Color_r : %d, uniform_Drawing_fs_3fv_mouseTracks_Color_g : %d, uniform_Drawing_fs_3fv_mouseTracks_Color_b : %d, uniform_Drawing_fs_3fv_mouseTracks_Color_a : %d, uniform_Drawing_fs_3fv_mouseTracks_BrushID : %d, uniform_Drawing_fs_4fv_mouseTracks_RadiusX : %d, uniform_Drawing_fs_4fv_mouseTracks_RadiusY : %d, uniform_Drawing_texture_fs_decal : %d, uniform_Drawing_texture_fs_lookupTable1 : %d, uniform_Drawing_texture_fs_lookupTable2 : %d, uniform_Drawing_texture_fs_lookupTable3 : %d, uniform_Drawing_texture_fs_lookupTable4 : %d, uniform_Drawing_texture_fs_lookupTable5 : %d, uniform_Drawing_texture_fs_lookupTable6 : %d\n", uniform_Drawing_vp_model, uniform_Drawing_vp_view, uniform_Drawing_vp_proj, uniform_Drawing_vp_2fv_width_height, uniform_Drawing_fs_4fv_W_H_drawingStart_drawingStroke, uniform_Drawing_fs_4fv_CAType_CASubType_partAccCenter, uniform_Drawing_fs_4fv_partMode_partAcc_clearLayer_void, uniform_Drawing_fs_4fv_flashPart_isBeat_void_clearCA, uniform_Drawing_fs_3fv_trackdecay_CAstep_initCA, uniform_Drawing_fs_4fv_CAdecay_frameno_partTexture_currentTrack, uniform_Drawing_fs_4fv_flashBackCoef_flashCACoefs , uniform_Drawing_fs_3fv_mouseTracks_replay, uniform_Drawing_fs_4fv_mouseTracks_x, uniform_Drawing_fs_4fv_mouseTracks_y, uniform_Drawing_fs_4fv_mouseTracks_x_prev, uniform_Drawing_fs_4fv_mouseTracks_y_prev, uniform_Drawing_fs_3fv_mouseTracks_Color_r, uniform_Drawing_fs_3fv_mouseTracks_Color_g, uniform_Drawing_fs_3fv_mouseTracks_Color_b, uniform_Drawing_fs_3fv_mouseTracks_Color_a, uniform_Drawing_fs_3fv_mouseTracks_BrushID, uniform_Drawing_fs_4fv_mouseTracks_RadiusX, uniform_Drawing_fs_4fv_mouseTracks_RadiusY, uniform_Drawing_texture_fs_decal, uniform_Drawing_texture_fs_lookupTable1, uniform_Drawing_texture_fs_lookupTable2, uniform_Drawing_texture_fs_lookupTable3, uniform_Drawing_texture_fs_lookupTable4, uniform_Drawing_texture_fs_lookupTable5, uniform_Drawing_texture_fs_lookupTable6);
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
  uniform_Composition_fs_4fv_messageTransparency_echo_echoNeg_invert 
    = glGetUniformLocation(shader_Composition_programme, "uniform_Composition_fs_4fv_messageTransparency_echo_echoNeg_invert");

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
       || (uniform_Composition_fs_4fv_messageTransparency_echo_echoNeg_invert == -1)
       || (uniform_Composition_texture_fs_decal == -1)
       || (uniform_Composition_texture_fs_lookupTable1 == -1)
       || (uniform_Composition_texture_fs_lookupTable2 == -1)
       || (uniform_Composition_texture_fs_lookupTable3 == -1)
       || (uniform_Composition_texture_fs_lookupTable4 == -1)
       || (uniform_Composition_texture_fs_lookupTable_font == -1)
       || (uniform_Composition_texture_fs_lookupTable_message == -1)
       ) {
    fprintf(stderr, "Could not bind uniforms Composition uniform_Composition_vp_proj : %d, uniform_Composition_vp_view : %d, uniform_Composition_vp_model : %d, uniform_Composition_fs_3fv_width_height_CAweight : %d, uniform_Composition_fs_3fv_trackweight : %d, uniform_Composition_fs_4fv_messageTransparency_echo_echoNeg_invert : %d, uniform_Composition_texture_fs_decal : %d, uniform_Composition_texture_fs_lookupTable1 : %d, uniform_Composition_texture_fs_lookupTable2 : %d, uniform_Composition_texture_fs_lookupTable3 : %d, uniform_Composition_texture_fs_lookupTable4 : %d, uniform_Composition_texture_fs_lookupTable_font : %d, uniform_Composition_texture_fs_lookupTable_message : %d\n" , uniform_Composition_vp_proj , uniform_Composition_vp_view , uniform_Composition_vp_model , uniform_Composition_fs_3fv_width_height_CAweight , uniform_Composition_fs_3fv_trackweight , uniform_Composition_fs_4fv_messageTransparency_echo_echoNeg_invert , uniform_Composition_texture_fs_decal , uniform_Composition_texture_fs_lookupTable1 , uniform_Composition_texture_fs_lookupTable2 , uniform_Composition_texture_fs_lookupTable3 , uniform_Composition_texture_fs_lookupTable4 , uniform_Composition_texture_fs_lookupTable_font , uniform_Composition_texture_fs_lookupTable_message);
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
  uniform_Final_fs_2fv_transparency_scale
    = glGetUniformLocation(shader_Final_programme, "uniform_Final_fs_2fv_transparency_scale");
  uniform_Final_fs_4fv_xy_frameno_cursorSize 
    = glGetUniformLocation(shader_Final_programme, "uniform_Final_fs_4fv_xy_frameno_cursorSize");
  uniform_Final_fs_2fv_tablet1xy 
    = glGetUniformLocation(shader_Final_programme, "uniform_Final_fs_2fv_tablet1xy");

  uniform_Final_texture_fs_decal 
    = glGetUniformLocation(shader_Final_programme, "fs_decal" ); // previous pass output
  uniform_Final_texture_fs_lookupTable1 
    = glGetUniformLocation(shader_Final_programme, "fs_lookupTable1" ); // LYM logo

  if ( (uniform_Final_vp_proj == -1) 
       || (uniform_Final_vp_view == -1) 
       || (uniform_Final_vp_model == -1) 
       || (uniform_Final_fs_2fv_transparency_scale == -1)
       || (uniform_Final_texture_fs_decal == -1)
       || (uniform_Final_texture_fs_lookupTable1 == -1)
       || (uniform_Final_fs_4fv_xy_frameno_cursorSize == -1)
       || (uniform_Final_fs_2fv_tablet1xy == -1)
       ) {
    fprintf(stderr, "Could not bind uniforms Final uniform_Final_vp_proj : %d, uniform_Final_vp_view : %d, uniform_Final_vp_model : %d, uniform_Final_fs_2fv_transparency_scale : %d , uniform_Final_texture_fs_decal : %d, uniform_Final_texture_fs_lookupTable1 : %d, uniform_Final_fs_4fv_xy_frameno_cursorSize : %d, uniform_Final_fs_2fv_tablet1xy: %d\n" , uniform_Final_vp_proj , uniform_Final_vp_view , uniform_Final_vp_model , uniform_Final_fs_2fv_transparency_scale , uniform_Final_texture_fs_decal , uniform_Final_texture_fs_lookupTable1 , uniform_Final_fs_4fv_xy_frameno_cursorSize, uniform_Final_fs_2fv_tablet1xy );
    // return 0;
  }

  ////////////////////////////////////////
  // shader parameeter bindings
  uniform_Sensor_vp_model 
    = glGetUniformLocation(shader_Sensor_programme, "vp_modelMatrix");
  uniform_Sensor_vp_view 
    = glGetUniformLocation(shader_Sensor_programme, "vp_viewMatrix");
  uniform_Sensor_vp_proj 
    = glGetUniformLocation(shader_Sensor_programme, "vp_projMatrix");
  //uniform_Sensor_fs_2fv_frameno_invert
  //  = glGetUniformLocation(shader_Sensor_programme, "uniform_Sensor_fs_2fv_frameno_invert");
  uniform_Sensor_fs_4fv_onOff_isCurrentSensor_isFollowMouse_transparency
    = glGetUniformLocation(shader_Sensor_programme, "uniform_Sensor_fs_4fv_onOff_isCurrentSensor_isFollowMouse_transparency");

  uniform_Sensor_texture_fs_decal 
    = glGetUniformLocation(shader_Sensor_programme, "fs_decal" ); // previous pass output

  if ( (uniform_Sensor_vp_proj == -1) 
       || (uniform_Sensor_vp_view == -1) 
       || (uniform_Sensor_vp_model == -1) 
//        || (uniform_Sensor_fs_2fv_frameno_invert == -1)
       || (uniform_Sensor_fs_4fv_onOff_isCurrentSensor_isFollowMouse_transparency == -1)
       || (uniform_Sensor_texture_fs_decal == -1)
       ) {
    fprintf(stderr, "Could not bind uniforms Sensor uniform_Sensor_vp_proj : %d, uniform_Sensor_vp_view : %d, uniform_Sensor_vp_model : %d, uniform_Sensor_fs_4fv_onOff_isCurrentSensor_isFollowMouse_transparency : %d , uniform_Sensor_texture_fs_decal : %d\n" , uniform_Sensor_vp_proj , uniform_Sensor_vp_view , uniform_Sensor_vp_model , uniform_Sensor_fs_4fv_onOff_isCurrentSensor_isFollowMouse_transparency , uniform_Sensor_texture_fs_decal );
    // return 0;
  }

}

