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

#include "pg_shader_body_decl.cpp"

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
GLint uniform_Drawing_fs_3fv_partAcc_clearAllLayers_clearCA;
GLint uniform_Drawing_fs_4fv_flashPart_flashTrack0Coefs_copyTrack12;
GLint uniform_Drawing_fs_4fv_trackdecay_clearLayer;
GLint uniform_Drawing_fs_3fv_CAdecay_frameno_currentDrawingTrack;
GLint uniform_Drawing_fs_4fv_flashBackCoef_flashCACoefs;
GLint uniform_Drawing_fs_3fv_mouseTracks_x;
GLint uniform_Drawing_fs_4fv_mouseTracks_y_videoH;
GLint uniform_Drawing_fs_4fv_mouseTracks_x_prev_videoW;
GLint uniform_Drawing_fs_4fv_mouseTracks_y_prev_videoThreshold;
GLint uniform_Drawing_fs_3fv_mouseTracks_Color_r;
GLint uniform_Drawing_fs_3fv_mouseTracks_Color_g;
GLint uniform_Drawing_fs_3fv_mouseTracks_Color_b;
GLint uniform_Drawing_fs_3fv_mouseTracks_Color_a;
GLint uniform_Drawing_fs_4fv_mouseTracks_BrushID_Cursor;
GLint uniform_Drawing_fs_3fv_mouseTracks_RadiusX;
GLint uniform_Drawing_fs_3fv_mouseTracks_RadiusY;

// DRAWING SHADER TEXTURE IDS
GLint uniform_Drawing_texture_fs_decal;         // 2-cycle ping-pong BG track step n-1 (FBO)
GLint uniform_Drawing_texture_fs_lookupTable1;  // 2-cycle ping-pong speed/position of particles (FBO)
GLint uniform_Drawing_texture_fs_lookupTable2;  // ping-pong CA (FBO)
#if PG_NB_TRACKS >= 2
GLint uniform_Drawing_texture_fs_lookupTable3;  // ping-pong track 1 (FBO)
#endif
#if PG_NB_TRACKS >= 3
GLint uniform_Drawing_texture_fs_lookupTable4;  // ping-pong track 2 (FBO)
#endif
GLint uniform_Drawing_texture_fs_lookupTable5;  // pen patterns
GLint uniform_Drawing_texture_fs_lookupTable6;  // particle acceleration texture
#ifdef PG_VIDEO_ACTIVE
GLint uniform_Drawing_texture_fs_lookupTable7;  // video frame
GLint uniform_Drawing_texture_fs_lookupTable8;  // current background video frame
GLint uniform_Drawing_texture_fs_lookupTable9;  // initial background video frame
#endif

/////////////////////////////////////////////////////////////////////////
// COMPOSITION SHADER
// COMPOSITION SHADER UNIFORM VARIABLES
GLint uniform_Composition_vp_model;
GLint uniform_Composition_vp_view;
GLint uniform_Composition_vp_proj;
GLint uniform_Composition_fs_3fv_width_height_messageTransparency;

// COMPOSITION SHADER TEXTURE IDS
GLint uniform_Composition_texture_fs_decal;         // ping-pong BG track (FBO)
GLint uniform_Composition_texture_fs_lookupTable1;  // ping-pong CA (FBO)
#if PG_NB_TRACKS >= 2
GLint uniform_Composition_texture_fs_lookupTable2;  // ping-pong track 1 (FBO)
#endif
#if PG_NB_TRACKS >= 3
GLint uniform_Composition_texture_fs_lookupTable3;  // ping-pong track 2 (FBO)
#endif
GLint uniform_Composition_texture_fs_lookupTable4;  // preceding snapshot
GLint uniform_Composition_texture_fs_lookupTable5;  // captured frame buffer
GLint uniform_Composition_texture_fs_lookupTable_font;  // message Font
GLint uniform_Composition_texture_fs_lookupTable_message;  // message string

/////////////////////////////////////////////////////////////////////////
// FINAL SHADER
// FINAL SHADER UNIFORM VARIABLES
GLint uniform_Final_vp_model;
GLint uniform_Final_vp_view;
GLint uniform_Final_vp_proj;
GLint uniform_Final_fs_3fv_xy_frameno;
GLint uniform_Final_fs_2fv_width_height;

// FINAL SHADER TEXTURE IDS
GLint uniform_Final_texture_fs_decal;         // previous pass output
GLint uniform_Final_texture_fs_lookupTable2;  // ping-pong BG track (FBO)
GLint uniform_Final_texture_fs_lookupTable3;  // ping-pong CA (FBO)
#if PG_NB_TRACKS >= 2
GLint uniform_Final_texture_fs_lookupTable4;  // ping-pong track 1 (FBO)
#endif
#if PG_NB_TRACKS >= 3
GLint uniform_Final_texture_fs_lookupTable5;  // ping-pong track 2 (FBO)
#endif

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
  pg_loadshader( (char *)("./shaders/LYM_Drawing_"+project_name+"-VP.glsl").c_str() , pg_vs_Drawing);
  glCompileShader (pg_vs_Drawing);
  pg_printLog(pg_vs_Drawing);

  unsigned int pg_vs_Composition = glCreateShader (GL_VERTEX_SHADER);
  pg_loadshader( (char *)("./shaders/LYM_Composition_"+project_name+"-VP.glsl").c_str() , pg_vs_Composition);
  glCompileShader (pg_vs_Composition);
  pg_printLog(pg_vs_Composition);

  unsigned int pg_vs_Final = glCreateShader (GL_VERTEX_SHADER);
  pg_loadshader( (char *)("./shaders/LYM_Final_"+project_name+"-VP.glsl").c_str() , pg_vs_Final);
  glCompileShader (pg_vs_Final);
  pg_printLog(pg_vs_Final);

  unsigned int pg_vs_Sensor = glCreateShader (GL_VERTEX_SHADER);
  pg_loadshader( (char *)("./shaders/LYM_Sensor_"+project_name+"-VP.glsl").c_str() , pg_vs_Sensor);
  glCompileShader (pg_vs_Sensor);
  pg_printLog(pg_vs_Sensor);


  unsigned int pg_fs_Drawing = glCreateShader (GL_FRAGMENT_SHADER);
  pg_loadshader( (char *)("./shaders/LYM_Drawing_"+project_name+"-FS_full.glsl").c_str() , pg_fs_Drawing);
  glCompileShader (pg_fs_Drawing);
  pg_printLog(pg_fs_Drawing);

  unsigned int pg_fs_Composition = glCreateShader (GL_FRAGMENT_SHADER);
  pg_loadshader( (char *)("./shaders/LYM_Composition_"+project_name+"-FS_full.glsl").c_str() , pg_fs_Composition);
  glCompileShader (pg_fs_Composition);
  pg_printLog(pg_fs_Composition);

  unsigned int pg_fs_Final = glCreateShader (GL_FRAGMENT_SHADER);
  pg_loadshader( (char *)("./shaders/LYM_Final_"+project_name+"-FS_full.glsl").c_str() , pg_fs_Final);
  glCompileShader (pg_fs_Final);
  pg_printLog(pg_fs_Final);

  unsigned int pg_fs_Sensor = glCreateShader (GL_FRAGMENT_SHADER);
  pg_loadshader( (char *)("./shaders/LYM_Sensor_"+project_name+"-FS.glsl").c_str() , pg_fs_Sensor);
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

#include "pg_shader_body_bind.cpp"

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
  uniform_Drawing_fs_3fv_partAcc_clearAllLayers_clearCA 
    = glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_fs_3fv_partAcc_clearAllLayers_clearCA");
  uniform_Drawing_fs_4fv_flashPart_flashTrack0Coefs_copyTrack12 
    = glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_fs_4fv_flashPart_flashTrack0Coefs_copyTrack12");
  uniform_Drawing_fs_4fv_trackdecay_clearLayer 
    = glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_fs_4fv_trackdecay_clearLayer");
  uniform_Drawing_fs_3fv_CAdecay_frameno_currentDrawingTrack 
    = glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_fs_3fv_CAdecay_frameno_currentDrawingTrack");
  uniform_Drawing_fs_4fv_flashBackCoef_flashCACoefs
	= glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_fs_4fv_flashBackCoef_flashCACoefs");
  uniform_Drawing_fs_3fv_mouseTracks_x 
    = glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_fs_3fv_mouseTracks_x");
  uniform_Drawing_fs_4fv_mouseTracks_y_videoH 
    = glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_fs_4fv_mouseTracks_y_videoH");
  uniform_Drawing_fs_4fv_mouseTracks_x_prev_videoW 
    = glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_fs_4fv_mouseTracks_x_prev_videoW");
  uniform_Drawing_fs_4fv_mouseTracks_y_prev_videoThreshold 
    = glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_fs_4fv_mouseTracks_y_prev_videoThreshold");
  uniform_Drawing_fs_3fv_mouseTracks_Color_r 
    = glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_fs_3fv_mouseTracks_Color_r");
  uniform_Drawing_fs_3fv_mouseTracks_Color_g 
    = glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_fs_3fv_mouseTracks_Color_g");
  uniform_Drawing_fs_3fv_mouseTracks_Color_b 
    = glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_fs_3fv_mouseTracks_Color_b");
  uniform_Drawing_fs_3fv_mouseTracks_Color_a 
    = glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_fs_3fv_mouseTracks_Color_a");
  uniform_Drawing_fs_4fv_mouseTracks_BrushID_Cursor 
    = glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_fs_4fv_mouseTracks_BrushID_Cursor");
  uniform_Drawing_fs_3fv_mouseTracks_RadiusX 
    = glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_fs_3fv_mouseTracks_RadiusX");
  uniform_Drawing_fs_3fv_mouseTracks_RadiusY 
    = glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_fs_3fv_mouseTracks_RadiusY");

  // BG track FBO
  uniform_Drawing_texture_fs_decal 
    = glGetUniformLocation(shader_Drawing_programme, "fs_decal" );         // 2-cycle ping-pong BG track step n-1 (FBO)
  // particle FBO
  uniform_Drawing_texture_fs_lookupTable1 
    = glGetUniformLocation(shader_Drawing_programme, "fs_lookupTable1" );  // 2-cycle ping-pong speed/position of particles (FBO)
  // CA FBO
  uniform_Drawing_texture_fs_lookupTable2 
    = glGetUniformLocation(shader_Drawing_programme, "fs_lookupTable2" );  // ping-pong CA (FBO)
  // track FBO
#if PG_NB_TRACKS >= 2
  uniform_Drawing_texture_fs_lookupTable3 
    = glGetUniformLocation(shader_Drawing_programme, "fs_lookupTable3" );  // ping-pong track 1 (FBO)
#endif
#if PG_NB_TRACKS >= 3
  uniform_Drawing_texture_fs_lookupTable4 
    = glGetUniformLocation(shader_Drawing_programme, "fs_lookupTable4" );  // ping-pong track 2 (FBO)
#endif
  // textures
  uniform_Drawing_texture_fs_lookupTable5 
    = glGetUniformLocation(shader_Drawing_programme, "fs_lookupTable5" );  // pen patterns
  uniform_Drawing_texture_fs_lookupTable6 
    = glGetUniformLocation(shader_Drawing_programme, "fs_lookupTable6" );  // particle acceleration texture
#ifdef PG_VIDEO_ACTIVE
  // video frame
  uniform_Drawing_texture_fs_lookupTable7
	  = glGetUniformLocation(shader_Drawing_programme, "fs_lookupTable7");  // video frame
  // current background frame
  uniform_Drawing_texture_fs_lookupTable8
	  = glGetUniformLocation(shader_Drawing_programme, "fs_lookupTable8");  // current bakcground video frame
  // initial background frame
  uniform_Drawing_texture_fs_lookupTable9
	  = glGetUniformLocation(shader_Drawing_programme, "fs_lookupTable9");  // initial bakcground video frame
#endif

  if( (uniform_Drawing_vp_model == -1)
      || (uniform_Drawing_vp_view == -1)
      || (uniform_Drawing_vp_proj == -1)
      || (uniform_Drawing_vp_2fv_width_height == -1)

      || (uniform_Drawing_fs_4fv_W_H_drawingStart_drawingStroke == -1)
      || (uniform_Drawing_fs_3fv_partAcc_clearAllLayers_clearCA == -1)
      || (uniform_Drawing_fs_4fv_flashPart_flashTrack0Coefs_copyTrack12 == -1)
      || (uniform_Drawing_fs_4fv_trackdecay_clearLayer == -1)
      || (uniform_Drawing_fs_3fv_CAdecay_frameno_currentDrawingTrack == -1)
      || (uniform_Drawing_fs_4fv_flashBackCoef_flashCACoefs == -1)
      || (uniform_Drawing_fs_3fv_mouseTracks_x == -1)
      || (uniform_Drawing_fs_4fv_mouseTracks_y_videoH == -1)
      //|| (uniform_Drawing_fs_3iv_mouseTracks_x_transl == -1)
      //|| (uniform_Drawing_fs_3iv_mouseTracks_y_transl == -1)
      || (uniform_Drawing_fs_4fv_mouseTracks_x_prev_videoW == -1)
      || (uniform_Drawing_fs_4fv_mouseTracks_y_prev_videoThreshold == -1)
      || (uniform_Drawing_fs_3fv_mouseTracks_Color_r == -1)
      || (uniform_Drawing_fs_3fv_mouseTracks_Color_g == -1)
      || (uniform_Drawing_fs_3fv_mouseTracks_Color_b == -1)
      || (uniform_Drawing_fs_3fv_mouseTracks_Color_a == -1)
      || (uniform_Drawing_fs_4fv_mouseTracks_BrushID_Cursor == -1)
      || (uniform_Drawing_fs_3fv_mouseTracks_RadiusX == -1)
     || (uniform_Drawing_fs_3fv_mouseTracks_RadiusY == -1)

      || (uniform_Drawing_texture_fs_decal == -1)
      || (uniform_Drawing_texture_fs_lookupTable1 == -1)
      || (uniform_Drawing_texture_fs_lookupTable2 == -1)
#if PG_NB_TRACKS >= 2
      || (uniform_Drawing_texture_fs_lookupTable3 == -1)
#endif
#if PG_NB_TRACKS >= 3
      || (uniform_Drawing_texture_fs_lookupTable4 == -1)
#endif
      || (uniform_Drawing_texture_fs_lookupTable5 == -1)
	  || (uniform_Drawing_texture_fs_lookupTable6 == -1)
#ifdef PG_VIDEO_ACTIVE
	  || (uniform_Drawing_texture_fs_lookupTable7 == -1)
	  || (uniform_Drawing_texture_fs_lookupTable8 == -1)
	  || (uniform_Drawing_texture_fs_lookupTable9 == -1)
#endif
      ) {
	  fprintf(stderr, "Could not bind uniforms Drawing uniform_Drawing_vp_model : %d, uniform_Drawing_vp_view : %d, uniform_Drawing_vp_proj : %d, uniform_Drawing_vp_2fv_width_height : %d, uniform_Drawing_fs_4fv_W_H_drawingStart_drawingStroke : %d, uniform_Drawing_fs_3fv_partAcc_clearAllLayers_clearCA : %d, uniform_Drawing_fs_4fv_flashPart_flashTrack0Coefs_copyTrack12 : %d, uniform_Drawing_fs_4fv_trackdecay_clearLayer : %d, uniform_Drawing_fs_3fv_CAdecay_frameno_currentDrawingTrack : %d, uniform_Drawing_fs_4fv_flashBackCoef_flashCACoefs : %d, uniform_Drawing_fs_3fv_mouseTracks_x : %d, uniform_Drawing_fs_4fv_mouseTracks_y_videoH : %d, uniform_Drawing_fs_4fv_mouseTracks_x_prev_videoW : %d, uniform_Drawing_fs_4fv_mouseTracks_y_prev_videoThreshold : %d, uniform_Drawing_fs_3fv_mouseTracks_Color_r : %d, uniform_Drawing_fs_3fv_mouseTracks_Color_g : %d, uniform_Drawing_fs_3fv_mouseTracks_Color_b : %d, uniform_Drawing_fs_3fv_mouseTracks_Color_a : %d, uniform_Drawing_fs_4fv_mouseTracks_BrushID_Cursor : %d, uniform_Drawing_fs_3fv_mouseTracks_RadiusX : %d, uniform_Drawing_fs_3fv_mouseTracks_RadiusY : %d, uniform_Drawing_texture_fs_decal : %d, uniform_Drawing_texture_fs_lookupTable1 : %d, uniform_Drawing_texture_fs_lookupTable2 : %d, uniform_Drawing_texture_fs_lookupTable5 : %d, uniform_Drawing_texture_fs_lookupTable6 : %d\n", uniform_Drawing_vp_model, uniform_Drawing_vp_view, uniform_Drawing_vp_proj, uniform_Drawing_vp_2fv_width_height, uniform_Drawing_fs_4fv_W_H_drawingStart_drawingStroke, uniform_Drawing_fs_3fv_partAcc_clearAllLayers_clearCA, uniform_Drawing_fs_4fv_flashPart_flashTrack0Coefs_copyTrack12, uniform_Drawing_fs_4fv_trackdecay_clearLayer, uniform_Drawing_fs_3fv_CAdecay_frameno_currentDrawingTrack, uniform_Drawing_fs_4fv_flashBackCoef_flashCACoefs, uniform_Drawing_fs_3fv_mouseTracks_x, uniform_Drawing_fs_4fv_mouseTracks_y_videoH , uniform_Drawing_fs_4fv_mouseTracks_x_prev_videoW, uniform_Drawing_fs_4fv_mouseTracks_y_prev_videoThreshold, uniform_Drawing_fs_3fv_mouseTracks_Color_r, uniform_Drawing_fs_3fv_mouseTracks_Color_g, uniform_Drawing_fs_3fv_mouseTracks_Color_b, uniform_Drawing_fs_3fv_mouseTracks_Color_a, uniform_Drawing_fs_4fv_mouseTracks_BrushID_Cursor, uniform_Drawing_fs_3fv_mouseTracks_RadiusX, uniform_Drawing_fs_3fv_mouseTracks_RadiusY, uniform_Drawing_texture_fs_decal, uniform_Drawing_texture_fs_lookupTable1, uniform_Drawing_texture_fs_lookupTable2, uniform_Drawing_texture_fs_lookupTable5, uniform_Drawing_texture_fs_lookupTable6 );
#if PG_NB_TRACKS >= 2
	  fprintf(stderr, "Could not bind uniforms uniform_Drawing_texture_fs_lookupTable3 : %d\n", uniform_Drawing_texture_fs_lookupTable3);
#endif
#if PG_NB_TRACKS >= 3
	  fprintf(stderr, "Could not bind uniforms uniform_Drawing_texture_fs_lookupTable4 : %d\n", uniform_Drawing_texture_fs_lookupTable4);
#endif
#ifdef PG_VIDEO_ACTIVE
	  fprintf(stderr, "Could not bind uniforms uniform_Drawing_fs_3fv_mouseTracuniform_Drawing_texture_fs_lookupTable7 : %d, uniform_Drawing_texture_fs_lookupTable8 : %d, uniform_Drawing_texture_fs_lookupTable9 : %d\n", uniform_Drawing_texture_fs_lookupTable7, uniform_Drawing_texture_fs_lookupTable8, uniform_Drawing_texture_fs_lookupTable9);
#endif
  }

  ////////////////////////////////////////
  // shader parameeter bindings
  uniform_Composition_vp_model 
    = glGetUniformLocation(shader_Composition_programme, "vp_modelMatrix");
  uniform_Composition_vp_view 
    = glGetUniformLocation(shader_Composition_programme, "vp_viewMatrix");
  uniform_Composition_vp_proj 
    = glGetUniformLocation(shader_Composition_programme, "vp_projMatrix");

  uniform_Composition_fs_3fv_width_height_messageTransparency 
    = glGetUniformLocation(shader_Composition_programme, "uniform_Composition_fs_3fv_width_height_messageTransparency");

  uniform_Composition_texture_fs_decal 
    = glGetUniformLocation(shader_Composition_programme, "fs_decal" ); // ping-pong BG track (FBO)
  uniform_Composition_texture_fs_lookupTable1 
    = glGetUniformLocation(shader_Composition_programme, "fs_lookupTable1" );  // ping-pong CA (FBO)
#if PG_NB_TRACKS >= 2
  uniform_Composition_texture_fs_lookupTable2 
    = glGetUniformLocation(shader_Composition_programme, "fs_lookupTable2" );  // ping-pong track 1 (FBO)
#endif
#if PG_NB_TRACKS >= 3
  uniform_Composition_texture_fs_lookupTable3 
    = glGetUniformLocation(shader_Composition_programme, "fs_lookupTable3" );  // ping-pong track 2 (FBO)
#endif
  uniform_Composition_texture_fs_lookupTable4 
    = glGetUniformLocation(shader_Composition_programme, "fs_lookupTable4" );  // 
  // captured image frame
  uniform_Composition_texture_fs_lookupTable5
	  = glGetUniformLocation(shader_Composition_programme, "fs_lookupTable5");  // captured frame buffer
  uniform_Composition_texture_fs_lookupTable_font
    = glGetUniformLocation(shader_Composition_programme, "fs_lookupTable_font" );  // message Font
  uniform_Composition_texture_fs_lookupTable_message 
    = glGetUniformLocation(shader_Composition_programme, "fs_lookupTable_message" );  // message string
  if ( (uniform_Composition_vp_proj == -1) 
       || (uniform_Composition_vp_view == -1) 
       || (uniform_Composition_vp_model == -1) 
       || (uniform_Composition_fs_3fv_width_height_messageTransparency == -1)
       || (uniform_Composition_texture_fs_decal == -1)
       || (uniform_Composition_texture_fs_lookupTable1 == -1)
#if PG_NB_TRACKS >= 2
       || (uniform_Composition_texture_fs_lookupTable2 == -1)
#endif
#if PG_NB_TRACKS >= 3
       || (uniform_Composition_texture_fs_lookupTable3 == -1)
#endif
       || (uniform_Composition_texture_fs_lookupTable4 == -1)
       || (uniform_Composition_texture_fs_lookupTable5 == -1)
       || (uniform_Composition_texture_fs_lookupTable_font == -1)
       || (uniform_Composition_texture_fs_lookupTable_message == -1)
       ) {
#if PG_NB_TRACKS >= 2
	  fprintf(stderr, "Could not bind uniforms uniform_Composition_texture_fs_lookupTable2 : %d\n", uniform_Composition_texture_fs_lookupTable2 );
#endif
#if PG_NB_TRACKS >= 3
	  fprintf(stderr, "Could not bind uniforms uniform_Composition_texture_fs_lookupTable3 : %d\n", uniform_Composition_texture_fs_lookupTable3 );
#endif
	  fprintf(stderr, "Could not bind uniforms Composition uniform_Composition_vp_proj : %d, uniform_Composition_vp_view : %d, uniform_Composition_vp_model : %d, uniform_Composition_fs_3fv_width_height_messageTransparency : %d, uniform_Composition_texture_fs_decal : %d, uniform_Composition_texture_fs_lookupTable1 : %d, uniform_Composition_texture_fs_lookupTable4 : %d, uniform_Composition_texture_fs_lookupTable5 : %d, uniform_Composition_texture_fs_lookupTable_font : %d, uniform_Composition_texture_fs_lookupTable_message : %d\n", uniform_Composition_vp_proj, uniform_Composition_vp_view, uniform_Composition_vp_model, uniform_Composition_fs_3fv_width_height_messageTransparency, uniform_Composition_texture_fs_decal, uniform_Composition_texture_fs_lookupTable1, uniform_Composition_texture_fs_lookupTable4, uniform_Composition_texture_fs_lookupTable5, uniform_Composition_texture_fs_lookupTable_font, uniform_Composition_texture_fs_lookupTable_message);
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
  uniform_Final_fs_3fv_xy_frameno 
    = glGetUniformLocation(shader_Final_programme, "uniform_Final_fs_3fv_xy_frameno");
  uniform_Final_fs_2fv_width_height
    = glGetUniformLocation(shader_Final_programme, "uniform_Final_fs_2fv_width_height");

  uniform_Final_texture_fs_decal 
    = glGetUniformLocation(shader_Final_programme, "fs_decal" ); // previous pass output
 uniform_Final_texture_fs_lookupTable2
	  = glGetUniformLocation(shader_Final_programme, "fs_lookupTable2"); // ping-pong BG track (FBO)
  uniform_Final_texture_fs_lookupTable3
	  = glGetUniformLocation(shader_Final_programme, "fs_lookupTable3"); // ping-pong CA (FBO)
#if PG_NB_TRACKS >= 2
  uniform_Final_texture_fs_lookupTable4
	  = glGetUniformLocation(shader_Final_programme, "fs_lookupTable4"); // ping-pong track 1 (FBO)
#endif
#if PG_NB_TRACKS >= 3
  uniform_Final_texture_fs_lookupTable5
	  = glGetUniformLocation(shader_Final_programme, "fs_lookupTable5"); // ping-pong track 2 (FBO)
#endif

  if ( (uniform_Final_vp_proj == -1) 
       || (uniform_Final_vp_view == -1) 
       || (uniform_Final_vp_model == -1) 
       || (uniform_Final_texture_fs_decal == -1)
	   || (uniform_Final_texture_fs_lookupTable2 == -1)
	   || (uniform_Final_texture_fs_lookupTable3 == -1)
#if PG_NB_TRACKS >= 2
	   || (uniform_Final_texture_fs_lookupTable4 == -1)
#endif
#if PG_NB_TRACKS >= 3
	   || (uniform_Final_texture_fs_lookupTable5 == -1)
#endif
	   || (uniform_Final_fs_3fv_xy_frameno == -1)
       || (uniform_Final_fs_2fv_width_height == -1)
       ) {
	  fprintf(stderr, "Could not bind uniforms Final uniform_Final_vp_proj : %d, uniform_Final_vp_view : %d, uniform_Final_vp_model : %d , uniform_Final_texture_fs_decal: %d, uniform_Final_texture_fs_lookupTable2: %d, uniform_Final_texture_fs_lookupTable3: %d, uniform_Final_fs_3fv_xy_frameno : %d, uniform_Final_fs_2fv_width_height : %d\n", uniform_Final_vp_proj, uniform_Final_vp_view, uniform_Final_vp_model, uniform_Final_texture_fs_decal, uniform_Final_texture_fs_lookupTable2, uniform_Final_texture_fs_lookupTable3, uniform_Final_fs_3fv_xy_frameno, uniform_Final_fs_2fv_width_height);
#if PG_NB_TRACKS >= 2
	  fprintf(stderr, "Could not bind uniforms Final uniform_Final_texture_fs_lookupTable4: %d\n", uniform_Final_texture_fs_lookupTable4);
#endif
#if PG_NB_TRACKS >= 3
	  fprintf(stderr, "Could not bind uniforms Final uniform_Final_texture_fs_lookupTable5: %d\n", uniform_Final_texture_fs_lookupTable5);
#endif
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
