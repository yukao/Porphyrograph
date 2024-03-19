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
unsigned int shader_MeshDisplay_programme;
unsigned int shader_Final_programme;

/////////////////////////////////////////////////////////////////////////
// DRAWING SHADER
// DRAWING SHADER UNIFORM VARIABLES
GLint uniform_Drawing_vp_model;
GLint uniform_Drawing_vp_view;
GLint uniform_Drawing_vp_proj;
GLint uniform_Drawing_vp_2fv_width_height;
GLint uniform_Drawing_fs_4fv_W_H_drawingStart_drawingStroke;
GLint uniform_Drawing_fs_4fv_CASubType_partAccCenter_radiusParticle;
GLint uniform_Drawing_fs_4fv_partMode_partAcc_partTextNoise_partTexture;
GLint uniform_Drawing_fs_4fv_CAdecay_repopulatingImage_repopulating_repopulatingCA;
GLint uniform_Drawing_fs_4fv_trackdecay_hidingXYcoef;
GLint uniform_Drawing_fs_4fv_CAType_frameno_void_void;
GLint uniform_Drawing_fs_4fv_void_void_void_clr;
GLint uniform_Drawing_fs_4fv_mouseTracks_xy_xyprev;
GLint uniform_Drawing_fs_4fv_mouseTracks_Color_rgba;
GLint uniform_Drawing_fs_4fv_mouseTracks_BrushIDRadius_replay;

// DRAWING SHADER TEXTURE IDS
GLint uniform_Drawing_texture_fs_decal;         // 3-cycle ping-pong localColor step n-1 (FBO)
GLint uniform_Drawing_texture_fs_lookupTable1;  // 3-cycle ping-pong speed/position of particles (FBO)
GLint uniform_Drawing_texture_fs_lookupTable2;  // ping-pong CA (FBO)
GLint uniform_Drawing_texture_fs_lookupTable5;  // pen patterns
GLint uniform_Drawing_texture_fs_lookupTable6;  // particle acceleration texture
GLint uniform_Drawing_texture_fs_lookupTable7;  // data tables for the CA
GLint uniform_Drawing_texture_fs_lookupTable8;  // noise float texture
GLint uniform_Drawing_texture_fs_lookupTable9;  // vessels texture

/////////////////////////////////////////////////////////////////////////
// COMPOSITION SHADER
// COMPOSITION SHADER UNIFORM VARIABLES
GLint uniform_Composition_vp_model;
GLint uniform_Composition_vp_view;
GLint uniform_Composition_vp_proj;
GLint uniform_Composition_fs_3fv_width_height_CAweight;
GLint uniform_Composition_fs_3fv_trackweight;
GLint uniform_Composition_fs_4fv_xy_frameno_cursorSize;
GLint uniform_Composition_fs_4fv_mesgTransp_echo_echoNeg_void;

// COMPOSITION SHADER TEXTURE IDS
GLint uniform_Composition_texture_fs_decal;         // ping-pong localColor (FBO)
GLint uniform_Composition_texture_fs_lookupTable1;  // ping-pong CA (FBO)
GLint uniform_Composition_texture_fs_lookupTable2;  // ping-pong track 1 (FBO)
GLint uniform_Composition_texture_fs_lookupTable3;  // ping-pong track 2 (FBO)
GLint uniform_Composition_texture_fs_lookupTable4;  // preceding snapshot
//GLint uniform_Composition_texture_fs_lookupTable_font;  // message Font
//GLint uniform_Composition_texture_fs_lookupTable_message;  // message string

/////////////////////////////////////////////////////////////////////////
// TRACK DISPLAY SHADER
// TRACK DISPLAY SHADER UNIFORM VARIABLES
GLint uniform_MeshDisplay_vp_model;
GLint uniform_MeshDisplay_vp_view;
GLint uniform_MeshDisplay_vp_projection;
GLint uniform_MeshDisplay_vp_2i_FBOwh;
GLint uniform_MeshDisplay_fs_2i_FBOwh;

// TRACK DISPLAY SHADER TEXTURE IDS
GLint uniform_MeshDisplay_texture_fs_decal;        // particles
GLint uniform_MeshDisplay_fs_4fv_FrameNo_breathing_meshFrontier_X_Width; // frame number & breathingg coeficient
GLint uniform_MeshDisplay_fs_3fv_master_isFlatRendering_whiteText; // output opacity
GLint uniform_MeshDisplay_fs_4i_isLineDrawing_isLeftCam_isDisplayLookAt_withMeshOnBlueBG;   // particle vs mesh drawing
GLint uniform_MeshDisplay_fs_4fv_textFrontier_wmin_wmax_wminWidth_wmaxWidth;
GLint uniform_MeshDisplay_fs_4fv_textFrontier_hmin_hmax_hminWidth_hmaxWidth;

/////////////////////////////////////////////////////////////////////////
// FINAL SHADER
// FINAL SHADER UNIFORM VARIABLES
GLint uniform_Final_vp_model;
GLint uniform_Final_vp_view;
GLint uniform_Final_vp_proj;
GLint uniform_Final_fs_4fv_isDisplayLookAt_projReversed_wh;
GLint uniform_Final_fs_2fv_double_window_proj2Reversed;

// FINAL SHADER TEXTURE IDS
GLint uniform_Final_texture_fs_decal;         // previous pass output

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
  pg_loadshader( (char *)("shaders/LYM_Drawing_"+project_name+"-VP.glsl").c_str() , pg_vs_Drawing);
  glCompileShader (pg_vs_Drawing);
  pg_printLog(pg_vs_Drawing);

  unsigned int pg_vs_Composition = glCreateShader (GL_VERTEX_SHADER);
  pg_loadshader( (char *)("shaders/LYM_Composition_"+project_name+"-VP.glsl").c_str() , pg_vs_Composition);
  glCompileShader (pg_vs_Composition);
  pg_printLog(pg_vs_Composition);

  unsigned int pg_vs_MeshDisplay = glCreateShader (GL_VERTEX_SHADER);
  pg_loadshader( (char *)("shaders/LYM_Mesh_"+project_name+"-VP.glsl").c_str() , pg_vs_MeshDisplay);
  glCompileShader (pg_vs_MeshDisplay);
  pg_printLog(pg_vs_MeshDisplay);

  unsigned int pg_vs_Final = glCreateShader (GL_VERTEX_SHADER);
  pg_loadshader( (char *)("shaders/LYM_Final_"+project_name+"-VP.glsl").c_str() , pg_vs_Final);
  glCompileShader (pg_vs_Final);
  pg_printLog(pg_vs_Final);

  unsigned int pg_fs_Drawing = glCreateShader (GL_FRAGMENT_SHADER);
  pg_loadshader( (char *)("shaders/LYM_Drawing_"+project_name+"-FS.glsl").c_str() , pg_fs_Drawing);
  glCompileShader (pg_fs_Drawing);
  pg_printLog(pg_fs_Drawing);

  unsigned int pg_fs_Composition = glCreateShader (GL_FRAGMENT_SHADER);
  pg_loadshader( (char *)("shaders/LYM_Composition_"+project_name+"-FS.glsl").c_str() , pg_fs_Composition);
  glCompileShader (pg_fs_Composition);
  pg_printLog(pg_fs_Composition);

  unsigned int pg_fs_MeshDisplay = glCreateShader (GL_FRAGMENT_SHADER);
  pg_loadshader( (char *)("shaders/LYM_Mesh_"+project_name+"-FS.glsl").c_str() , pg_fs_MeshDisplay);
  glCompileShader (pg_fs_MeshDisplay);
  pg_printLog(pg_fs_MeshDisplay);

  unsigned int pg_fs_Final = glCreateShader (GL_FRAGMENT_SHADER);
  pg_loadshader( (char *)("shaders/LYM_Final_"+project_name+"-FS.glsl").c_str() , pg_fs_Final);
  glCompileShader (pg_fs_Final);
  pg_printLog(pg_fs_Final);

  shader_Drawing_programme = glCreateProgram ();
  glAttachShader (shader_Drawing_programme, pg_fs_Drawing);
  glAttachShader (shader_Drawing_programme, pg_vs_Drawing);
  glLinkProgram (shader_Drawing_programme);

  shader_Composition_programme = glCreateProgram ();
  glAttachShader (shader_Composition_programme, pg_fs_Composition);
  glAttachShader (shader_Composition_programme, pg_vs_Composition);
  glLinkProgram (shader_Composition_programme);

  shader_MeshDisplay_programme = glCreateProgram ();
  glAttachShader (shader_MeshDisplay_programme, pg_fs_MeshDisplay);
  glAttachShader (shader_MeshDisplay_programme, pg_vs_MeshDisplay);
  glLinkProgram (shader_MeshDisplay_programme);

  shader_Final_programme = glCreateProgram ();
  glAttachShader (shader_Final_programme, pg_fs_Final);
  glAttachShader (shader_Final_programme, pg_vs_Final);
  glLinkProgram (shader_Final_programme);

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
  uniform_Drawing_fs_4fv_CASubType_partAccCenter_radiusParticle 
    = glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_fs_4fv_CASubType_partAccCenter_radiusParticle");
  uniform_Drawing_fs_4fv_partMode_partAcc_partTextNoise_partTexture 
    = glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_fs_4fv_partMode_partAcc_partTextNoise_partTexture");
  uniform_Drawing_fs_4fv_CAdecay_repopulatingImage_repopulating_repopulatingCA 
    = glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_fs_4fv_CAdecay_repopulatingImage_repopulating_repopulatingCA");
  uniform_Drawing_fs_4fv_trackdecay_hidingXYcoef 
    = glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_fs_4fv_trackdecay_hidingXYcoef");
  uniform_Drawing_fs_4fv_CAType_frameno_void_void 
    = glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_fs_4fv_CAType_frameno_void_void");
  uniform_Drawing_fs_4fv_void_void_void_clr
	= glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_fs_4fv_void_void_void_clr");
  uniform_Drawing_fs_4fv_mouseTracks_xy_xyprev 
    = glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_fs_4fv_mouseTracks_xy_xyprev");
  uniform_Drawing_fs_4fv_mouseTracks_Color_rgba 
    = glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_fs_4fv_mouseTracks_Color_rgba");
  uniform_Drawing_fs_4fv_mouseTracks_BrushIDRadius_replay 
    = glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_fs_4fv_mouseTracks_BrushIDRadius_replay");

  // color FBO
  uniform_Drawing_texture_fs_decal 
    = glGetUniformLocation(shader_Drawing_programme, "fs_decal" );         // 3-cycle ping-pong localColor step n-1 (FBO)
  // particle FBO
  uniform_Drawing_texture_fs_lookupTable1 
    = glGetUniformLocation(shader_Drawing_programme, "fs_lookupTable1" );  // 3-cycle ping-pong speed/position of particles (FBO)
  // CA FBO
  uniform_Drawing_texture_fs_lookupTable2 
    = glGetUniformLocation(shader_Drawing_programme, "fs_lookupTable2" );  // ping-pong CA (FBO)
  // textures
  uniform_Drawing_texture_fs_lookupTable5 
    = glGetUniformLocation(shader_Drawing_programme, "fs_lookupTable5" );  // pen patterns
  uniform_Drawing_texture_fs_lookupTable6 
    = glGetUniformLocation(shader_Drawing_programme, "fs_lookupTable6" );  // particle acceleration texture
  uniform_Drawing_texture_fs_lookupTable7 
    = glGetUniformLocation(shader_Drawing_programme, "fs_lookupTable7" );  // data tables for the CA
  uniform_Drawing_texture_fs_lookupTable8 
    = glGetUniformLocation(shader_Drawing_programme, "fs_lookupTable8" );  // noise float texture
  uniform_Drawing_texture_fs_lookupTable9 
    = glGetUniformLocation(shader_Drawing_programme, "fs_lookupTable9" );  // vessels texture

  if( (uniform_Drawing_vp_model == -1)
      || (uniform_Drawing_vp_view == -1)
      || (uniform_Drawing_vp_proj == -1)
      || (uniform_Drawing_vp_2fv_width_height == -1)

      || (uniform_Drawing_fs_4fv_W_H_drawingStart_drawingStroke == -1)
      || (uniform_Drawing_fs_4fv_CASubType_partAccCenter_radiusParticle == -1)
      || (uniform_Drawing_fs_4fv_partMode_partAcc_partTextNoise_partTexture == -1)
      || (uniform_Drawing_fs_4fv_CAdecay_repopulatingImage_repopulating_repopulatingCA == -1)
      || (uniform_Drawing_fs_4fv_trackdecay_hidingXYcoef == -1)
      || (uniform_Drawing_fs_4fv_CAType_frameno_void_void == -1)
      || (uniform_Drawing_fs_4fv_void_void_void_clr == -1)
      || (uniform_Drawing_fs_4fv_mouseTracks_xy_xyprev == -1)
      || (uniform_Drawing_fs_4fv_mouseTracks_Color_rgba == -1)
      || (uniform_Drawing_fs_4fv_mouseTracks_BrushIDRadius_replay == -1)

      || (uniform_Drawing_texture_fs_decal == -1)
      || (uniform_Drawing_texture_fs_lookupTable1 == -1)
      || (uniform_Drawing_texture_fs_lookupTable2 == -1)
      || (uniform_Drawing_texture_fs_lookupTable5 == -1)
      || (uniform_Drawing_texture_fs_lookupTable6 == -1)
      || (uniform_Drawing_texture_fs_lookupTable7 == -1) 
      || (uniform_Drawing_texture_fs_lookupTable8 == -1) 
      || (uniform_Drawing_texture_fs_lookupTable9 == -1) 
      ) {
	  fprintf(stderr, "Could not bind uniforms Drawing uniform_Drawing_vp_model : %d, uniform_Drawing_vp_view : %d, uniform_Drawing_vp_proj : %d, uniform_Drawing_vp_2fv_width_height : %d, uniform_Drawing_fs_4fv_W_H_drawingStart_drawingStroke : %d, uniform_Drawing_fs_4fv_CASubType_partAccCenter_radiusParticle : %d, uniform_Drawing_fs_4fv_partMode_partAcc_partTextNoise_partTexture : %d, uniform_Drawing_fs_4fv_CAdecay_repopulatingImage_repopulating_repopulatingCA : %d, uniform_Drawing_fs_4fv_trackdecay_hidingXYcoef : %d, uniform_Drawing_fs_4fv_CAType_frameno_void_void : %d, uniform_Drawing_fs_4fv_void_void_void_clr : %d, uniform_Drawing_fs_4fv_mouseTracks_xy_xyprev : %d, uniform_Drawing_fs_4fv_mouseTracks_BrushIDRadius_replay : %d\n", 
		                                               uniform_Drawing_vp_model, uniform_Drawing_vp_view, uniform_Drawing_vp_proj, uniform_Drawing_vp_2fv_width_height, uniform_Drawing_fs_4fv_W_H_drawingStart_drawingStroke, uniform_Drawing_fs_4fv_CASubType_partAccCenter_radiusParticle, uniform_Drawing_fs_4fv_partMode_partAcc_partTextNoise_partTexture, uniform_Drawing_fs_4fv_CAdecay_repopulatingImage_repopulating_repopulatingCA, uniform_Drawing_fs_4fv_trackdecay_hidingXYcoef, uniform_Drawing_fs_4fv_CAType_frameno_void_void, uniform_Drawing_fs_4fv_void_void_void_clr , uniform_Drawing_fs_4fv_mouseTracks_xy_xyprev, uniform_Drawing_fs_4fv_mouseTracks_BrushIDRadius_replay);
	  fprintf(stderr, "Could not bind uniforms Drawing uniform_Drawing_texture_fs_decal : %d, uniform_Drawing_texture_fs_lookupTable1 : %d, uniform_Drawing_texture_fs_lookupTable2 : %d, uniform_Drawing_texture_fs_lookupTable5 : %d, uniform_Drawing_texture_fs_lookupTable6 : %d, uniform_Drawing_texture_fs_lookupTable7 : %d, uniform_Drawing_texture_fs_lookupTable8 : %d, uniform_Drawing_texture_fs_lookupTable9 : %d\n", 
		                                               uniform_Drawing_texture_fs_decal, uniform_Drawing_texture_fs_lookupTable1, uniform_Drawing_texture_fs_lookupTable2, uniform_Drawing_texture_fs_lookupTable5, uniform_Drawing_texture_fs_lookupTable6, uniform_Drawing_texture_fs_lookupTable7, uniform_Drawing_texture_fs_lookupTable8, uniform_Drawing_texture_fs_lookupTable9);
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
  uniform_Composition_fs_4fv_xy_frameno_cursorSize 
    = glGetUniformLocation(shader_Composition_programme, "uniform_Composition_fs_4fv_xy_frameno_cursorSize");
  uniform_Composition_fs_4fv_mesgTransp_echo_echoNeg_void 
    = glGetUniformLocation(shader_Composition_programme, "uniform_Composition_fs_4fv_mesgTransp_echo_echoNeg_void");

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
  //uniform_Composition_texture_fs_lookupTable_font 
  //  = glGetUniformLocation(shader_Composition_programme, "fs_lookupTable_font" );  // message Font
  //uniform_Composition_texture_fs_lookupTable_message 
  //  = glGetUniformLocation(shader_Composition_programme, "fs_lookupTable_message" );  // message string
  if ( (uniform_Composition_vp_proj == -1) 
       || (uniform_Composition_vp_view == -1) 
       || (uniform_Composition_vp_model == -1) 
       || (uniform_Composition_fs_3fv_width_height_CAweight == -1)
       || (uniform_Composition_fs_3fv_trackweight == -1)
       || (uniform_Composition_fs_4fv_xy_frameno_cursorSize == -1)
       || (uniform_Composition_fs_4fv_mesgTransp_echo_echoNeg_void == -1)
       || (uniform_Composition_texture_fs_decal == -1)
       || (uniform_Composition_texture_fs_lookupTable1 == -1)
       || (uniform_Composition_texture_fs_lookupTable2 == -1)
       || (uniform_Composition_texture_fs_lookupTable3 == -1)
       || (uniform_Composition_texture_fs_lookupTable4 == -1)
       //|| (uniform_Composition_texture_fs_lookupTable_font == -1)
       //|| (uniform_Composition_texture_fs_lookupTable_message == -1)
       ) {
    fprintf(stderr, "Could not bind uniforms Composition uniform_Composition_vp_proj : %d, uniform_Composition_vp_view : %d, uniform_Composition_vp_model : %d, uniform_Composition_fs_3fv_width_height_CAweight : %d, uniform_Composition_fs_3fv_trackweight : %d, uniform_Composition_fs_4fv_xy_frameno_cursorSize : %d, uniform_Composition_fs_4fv_mesgTransp_echo_echoNeg_void : %d, uniform_Composition_texture_fs_decal : %d, uniform_Composition_texture_fs_lookupTable1 : %d, uniform_Composition_texture_fs_lookupTable2 : %d, uniform_Composition_texture_fs_lookupTable3 : %d, uniform_Composition_texture_fs_lookupTable4 : %d\n" , uniform_Composition_vp_proj , uniform_Composition_vp_view , uniform_Composition_vp_model , uniform_Composition_fs_3fv_width_height_CAweight , uniform_Composition_fs_3fv_trackweight , uniform_Composition_fs_4fv_xy_frameno_cursorSize , uniform_Composition_fs_4fv_mesgTransp_echo_echoNeg_void , uniform_Composition_texture_fs_decal , uniform_Composition_texture_fs_lookupTable1 , uniform_Composition_texture_fs_lookupTable2 , uniform_Composition_texture_fs_lookupTable3 , uniform_Composition_texture_fs_lookupTable4 );
    // return 0;
  }
  
  ////////////////////////////////////////
  // shader parameeter bindings
  uniform_MeshDisplay_vp_model 
    = glGetUniformLocation(shader_MeshDisplay_programme, "vp_modelMatrix");
  uniform_MeshDisplay_vp_view 
    = glGetUniformLocation(shader_MeshDisplay_programme, "vp_viewMatrix");
  uniform_MeshDisplay_vp_projection
	  = glGetUniformLocation(shader_MeshDisplay_programme, "vp_projectionMatrix");
  uniform_MeshDisplay_vp_2i_FBOwh
	  = glGetUniformLocation(shader_MeshDisplay_programme, "uniform_MeshDisplay_vp_2i_FBOwh");
  uniform_MeshDisplay_fs_2i_FBOwh
	  = glGetUniformLocation(shader_MeshDisplay_programme, "uniform_MeshDisplay_fs_2i_FBOwh");
  
  uniform_MeshDisplay_texture_fs_decal 
    = glGetUniformLocation(shader_MeshDisplay_programme, "fs_decal" ); // particles
  uniform_MeshDisplay_fs_4fv_FrameNo_breathing_meshFrontier_X_Width 
    = glGetUniformLocation(shader_MeshDisplay_programme, "uniform_MeshDisplay_fs_4fv_FrameNo_breathing_meshFrontier_X_Width");
  uniform_MeshDisplay_fs_3fv_master_isFlatRendering_whiteText 
    = glGetUniformLocation(shader_MeshDisplay_programme, "uniform_MeshDisplay_fs_3fv_master_isFlatRendering_whiteText");
  uniform_MeshDisplay_fs_4i_isLineDrawing_isLeftCam_isDisplayLookAt_withMeshOnBlueBG
    = glGetUniformLocation(shader_MeshDisplay_programme, "uniform_MeshDisplay_fs_4i_isLineDrawing_isLeftCam_isDisplayLookAt_withMeshOnBlueBG");   // particle vs mesh drawing

  uniform_MeshDisplay_fs_4fv_textFrontier_wmin_wmax_wminWidth_wmaxWidth 
    = glGetUniformLocation(shader_MeshDisplay_programme, "uniform_MeshDisplay_fs_4fv_textFrontier_wmin_wmax_wminWidth_wmaxWidth");
  uniform_MeshDisplay_fs_4fv_textFrontier_hmin_hmax_hminWidth_hmaxWidth 
    = glGetUniformLocation(shader_MeshDisplay_programme, "uniform_MeshDisplay_fs_4fv_textFrontier_hmin_hmax_hminWidth_hmaxWidth");

  if ( (uniform_MeshDisplay_vp_projection == -1) 
       || (uniform_MeshDisplay_vp_view == -1) 
       || (uniform_MeshDisplay_vp_model == -1) 
       || (uniform_MeshDisplay_vp_2i_FBOwh == -1)
       || (uniform_MeshDisplay_fs_2i_FBOwh == -1)
       || (uniform_MeshDisplay_texture_fs_decal == -1)
       || (uniform_MeshDisplay_fs_4fv_FrameNo_breathing_meshFrontier_X_Width == -1)
       || (uniform_MeshDisplay_fs_3fv_master_isFlatRendering_whiteText == -1)
       || (uniform_MeshDisplay_fs_4i_isLineDrawing_isLeftCam_isDisplayLookAt_withMeshOnBlueBG == -1)
       || (uniform_MeshDisplay_fs_4fv_textFrontier_wmin_wmax_wminWidth_wmaxWidth == -1)
       || (uniform_MeshDisplay_fs_4fv_textFrontier_hmin_hmax_hminWidth_hmaxWidth == -1)
       ) {
	  fprintf(stderr, "Could not bind uniforms MeshDisplay uniform_MeshDisplay_gs_proj : %d, uniform_MeshDisplay_gs_view : %d, uniform_MeshDisplay_gs_model : %d, uniform_MeshDisplay_vp_2i_FBOwh: %d, uniform_MeshDisplay_fs_2i_FBOwh: %d, uniform_MeshDisplay_texture_fs_decal : %d, uniform_MeshDisplay_fs_4fv_FrameNo_breathing_meshFrontier_X_Width : %d, uniform_MeshDisplay_fs_3fv_master_isFlatRendering_whiteText : %d, uniform_MeshDisplay_fs_4i_isLineDrawing_isLeftCam_isDisplayLookAt_withMeshOnBlueBG: %d , uniform_MeshDisplay_fs_4fv_textFrontier_wmin_wmax_wminWidth_wmaxWidth : %d , uniform_MeshDisplay_fs_4fv_textFrontier_hmin_hmax_hminWidth_hmaxWidth : %d\n", uniform_MeshDisplay_vp_projection, uniform_MeshDisplay_vp_view, uniform_MeshDisplay_vp_model, uniform_MeshDisplay_vp_2i_FBOwh, uniform_MeshDisplay_fs_2i_FBOwh , uniform_MeshDisplay_texture_fs_decal, uniform_MeshDisplay_fs_4fv_FrameNo_breathing_meshFrontier_X_Width, uniform_MeshDisplay_fs_3fv_master_isFlatRendering_whiteText, uniform_MeshDisplay_fs_4i_isLineDrawing_isLeftCam_isDisplayLookAt_withMeshOnBlueBG, uniform_MeshDisplay_fs_4fv_textFrontier_wmin_wmax_wminWidth_wmaxWidth, uniform_MeshDisplay_fs_4fv_textFrontier_hmin_hmax_hminWidth_hmaxWidth);
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

  uniform_Final_fs_4fv_isDisplayLookAt_projReversed_wh 
    = glGetUniformLocation(shader_Final_programme, "uniform_Final_fs_4fv_isDisplayLookAt_projReversed_wh");

  uniform_Final_fs_2fv_double_window_proj2Reversed 
    = glGetUniformLocation(shader_Final_programme, "uniform_Final_fs_2fv_double_window_proj2Reversed");

  uniform_Final_texture_fs_decal 
    = glGetUniformLocation(shader_Final_programme, "fs_decal" ); // previous pass output

  if ( (uniform_Final_vp_proj == -1) 
       || (uniform_Final_vp_view == -1) 
       || (uniform_Final_vp_model == -1) 
       || (uniform_Final_texture_fs_decal == -1)
	   || (uniform_Final_fs_4fv_isDisplayLookAt_projReversed_wh == -1)
	   || (uniform_Final_fs_2fv_double_window_proj2Reversed == -1)
       ) {
	  fprintf(stderr, "Could not bind uniforms Final uniform_Final_vp_proj : %d, uniform_Final_vp_view : %d, uniform_Final_vp_model : %d , uniform_Final_texture_fs_decal: %d, uniform_Final_fs_4fv_isDisplayLookAt_projReversed_wh: %d, uniform_Final_fs_2fv_double_window_proj2Reversed: %d\n", uniform_Final_vp_proj, uniform_Final_vp_view, uniform_Final_vp_model, uniform_Final_texture_fs_decal, uniform_Final_fs_4fv_isDisplayLookAt_projReversed_wh, uniform_Final_fs_2fv_double_window_proj2Reversed);
    // return 0;
  }
}

