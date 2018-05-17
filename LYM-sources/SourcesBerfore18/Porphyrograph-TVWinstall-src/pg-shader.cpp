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
#ifdef PG_TRACK_DISPLAY
unsigned int shader_TrackDisplay_programme;
#endif

/////////////////////////////////////////////////////////////////////////
// DRAWING SHADER
// DRAWING SHADER UNIFORM VARIABLES
GLint uniform_Drawing_vp_model;
GLint uniform_Drawing_vp_view;
GLint uniform_Drawing_vp_proj;
GLint uniform_Drawing_vp_2fv_width_height;
GLint uniform_Drawing_fs_4fv_W_H_drawingStart_drawingStroke;
GLint uniform_Drawing_fs_4fv_weights03;
GLint uniform_Drawing_fs_2fv_weights45;
GLint uniform_Drawing_fs_3fv_alphaSwap02;
GLint uniform_Drawing_fs_3fv_alphaSwap35;
GLint uniform_Drawing_fs_4fv_image_noisesxy;
GLint uniform_Drawing_fs_4fv_mask_noisesxy;
GLint uniform_Drawing_fs_4fv_flashPart_flashTrack0Coefs_copyTrack12;
GLint uniform_Drawing_fs_3fv_clearLayer_flashVideoCoef_flashVideoThreshold;
//GLint uniform_Drawing_fs_4fv_mouseTracks_xy_prevxy;
GLint uniform_Drawing_fs_4fv_flashBackCoef_videoWH_videoThreshold;
GLint uniform_Drawing_fs_4fv_mouseTracks_Color_rgba;
GLint uniform_Drawing_fs_4fv_partAcc_clearAllLayers_frameNo_time;
//GLint uniform_Drawing_fs_4fv_mouseTracks_RadiusXY_isNewBeat_Cursor;
GLint uniform_Drawing_fs_4fv_pulse;
#ifdef PG_WITH_CA
//GLint uniform_Drawing_fs_4fv_CAdecay_clearCA_flashCACoefs;
#endif

// DRAWING SHADER TEXTURE IDS
GLint uniform_Drawing_texture_fs_decal;         // 2-cycle ping-pong BG track step n-1 (FBO)
GLint uniform_Drawing_texture_fs_lookupTable1;  // 2-cycle ping-pong speed/position of particles (FBO)
#ifdef PG_WITH_CA
//GLint uniform_Drawing_texture_fs_lookupTable2;  // ping-pong CA (FBO)
#endif
#if PG_NB_TRACKS >= 2
GLint uniform_Drawing_texture_fs_lookupTable3;  // ping-pong track 1 (FBO)
#endif
#if PG_NB_TRACKS >= 3
GLint uniform_Drawing_texture_fs_lookupTable4;  // ping-pong track 2 (FBO)
#endif
//GLint uniform_Drawing_texture_fs_lookupTable5;  // pen patterns
GLint uniform_Drawing_texture_fs_lookupTable6;  // particle acceleration texture
#ifdef PG_VIDEO_ACTIVE
GLint uniform_Drawing_texture_fs_lookupTable7;  // video frame
//GLint uniform_Drawing_texture_fs_lookupTable8;  // current background video frame
//GLint uniform_Drawing_texture_fs_lookupTable9;  // initial background video frame
#endif
GLint uniform_Drawing_texture_fs_lookupTable10;  // image buffer #0
GLint uniform_Drawing_texture_fs_lookupTable11;  // image buffer #1
GLint uniform_Drawing_texture_fs_lookupTable12;  // image buffer #2
GLint uniform_Drawing_texture_fs_lookupTable13;  // image buffer #3
GLint uniform_Drawing_texture_fs_lookupTable14;  // image buffer #4
GLint uniform_Drawing_texture_fs_lookupTable15;  // image buffer #5
GLint uniform_Drawing_texture_fs_lookupTable16;  // image mask #0
GLint uniform_Drawing_texture_fs_lookupTable17;  // image mask #1
//GLint uniform_Drawing_texture_fs_lookupTable18;  // image mask #2
//GLint uniform_Drawing_texture_fs_lookupTable19;  // image mask #3
GLint uniform_Drawing_texture_fs_lookupTable20;  // buffer swap image #0
GLint uniform_Drawing_texture_fs_lookupTable21;  // buffer swap image #1
GLint uniform_Drawing_texture_fs_lookupTable22;  // buffer swap image #2
GLint uniform_Drawing_texture_fs_lookupTable23;  // buffer swap image #3
GLint uniform_Drawing_texture_fs_lookupTable24;  // buffer swap image #4
GLint uniform_Drawing_texture_fs_lookupTable25;  // buffer swap image #5

/////////////////////////////////////////////////////////////////////////
// COMPOSITION SHADER
// COMPOSITION SHADER UNIFORM VARIABLES
GLint uniform_Composition_vp_model;
GLint uniform_Composition_vp_view;
GLint uniform_Composition_vp_proj;
GLint uniform_Composition_fs_4fv_height_screenMsgTransp_Text1_2_Alpha;
GLint uniform_Composition_fs_3fv_flashVideoCoef_flashVideoThreshold_width;

// COMPOSITION SHADER TEXTURE IDS
GLint uniform_Composition_texture_fs_decal;         // ping-pong BG track (FBO)
#ifdef PG_WITH_CA
GLint uniform_Composition_texture_fs_lookupTable1;  // ping-pong CA (FBO)
#endif
#if PG_NB_TRACKS >= 2
GLint uniform_Composition_texture_fs_lookupTable2;  // ping-pong track 1 (FBO)
#endif
#if PG_NB_TRACKS >= 3
GLint uniform_Composition_texture_fs_lookupTable3;  // ping-pong track 2 (FBO)
#endif
GLint uniform_Composition_texture_fs_lookupTable4;  // preceding snapshot
//GLint uniform_Composition_texture_fs_lookupTable5;  // captured frame buffer
GLint uniform_Composition_texture_fs_lookupTable_screen_font;  // message Font
GLint uniform_Composition_texture_fs_lookupTable_screen_msg_offset;  // message string
GLint uniform_Composition_texture_fs_lookupTable_displayMsg_font;  // large display message Font
GLint uniform_Composition_texture_fs_lookupTable_displayMsg1_offset;  // large display message string
GLint uniform_Composition_texture_fs_lookupTable_displayMsg2_offset;  // large display message string
GLint uniform_Composition_texture_fs_lookupTable_hashtag_font;        // hashtag very large font
GLint uniform_Composition_texture_fs_lookupTable_hashtag_offset;       // hashtag very large font offests


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
#ifdef PG_TRACK_DISPLAY
GLint uniform_Final_texture_fs_lookupTable1;  // previous display tracks output
#endif
GLint uniform_Final_texture_fs_lookupTable2;  // ping-pong BG track (FBO)
#ifdef PG_WITH_CA
GLint uniform_Final_texture_fs_lookupTable3;  // ping-pong CA (FBO)
#endif
#if PG_NB_TRACKS >= 2
GLint uniform_Final_texture_fs_lookupTable4;  // ping-pong track 1 (FBO)
#endif
#if PG_NB_TRACKS >= 3
GLint uniform_Final_texture_fs_lookupTable5;  // ping-pong track 2 (FBO)
#endif

#ifdef PG_TRACK_DISPLAY
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
#endif

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
  pg_loadshader( (char *)(project_name+"/shaders/LYM_Drawing_"+project_name+"-VP.glsl").c_str() , pg_vs_Drawing);
  glCompileShader (pg_vs_Drawing);
  pg_printLog(pg_vs_Drawing);

  unsigned int pg_vs_Composition = glCreateShader (GL_VERTEX_SHADER);
  pg_loadshader( (char *)(project_name+"/shaders/LYM_Composition_"+project_name+"-VP.glsl").c_str() , pg_vs_Composition);
  glCompileShader (pg_vs_Composition);
  pg_printLog(pg_vs_Composition);

  unsigned int pg_vs_Final = glCreateShader (GL_VERTEX_SHADER);
  pg_loadshader( (char *)(project_name+"/shaders/LYM_Final_"+project_name+"-VP.glsl").c_str() , pg_vs_Final);
  glCompileShader (pg_vs_Final);
  pg_printLog(pg_vs_Final);

#ifdef PG_TRACK_DISPLAY
  unsigned int pg_vs_TrackDisplay = glCreateShader (GL_VERTEX_SHADER);
  pg_loadshader( (char *)(project_name+"/shaders/LYM_TrackDisplay_"+project_name+"-VP.glsl").c_str() , pg_vs_TrackDisplay);
  glCompileShader (pg_vs_TrackDisplay);
  pg_printLog(pg_vs_TrackDisplay);

  unsigned int pg_gs_TrackDisplay = glCreateShader (GL_GEOMETRY_SHADER);
  pg_loadshader( (char *)(project_name+"/shaders/LYM_TrackDisplay_"+project_name+"-GS.glsl").c_str() , pg_gs_TrackDisplay);
  glCompileShader (pg_gs_TrackDisplay);
  pg_printLog(pg_gs_TrackDisplay);
#endif

  unsigned int pg_fs_Drawing = glCreateShader (GL_FRAGMENT_SHADER);
  pg_loadshader( (char *)(project_name+"/shaders/LYM_Drawing_"+project_name+"-FS_full.glsl").c_str() , pg_fs_Drawing);
  glCompileShader (pg_fs_Drawing);
  pg_printLog(pg_fs_Drawing);

  unsigned int pg_fs_Composition = glCreateShader (GL_FRAGMENT_SHADER);
  pg_loadshader( (char *)(project_name+"/shaders/LYM_Composition_"+project_name+"-FS_full.glsl").c_str() , pg_fs_Composition);
  glCompileShader (pg_fs_Composition);
  pg_printLog(pg_fs_Composition);

  unsigned int pg_fs_Final = glCreateShader (GL_FRAGMENT_SHADER);
  pg_loadshader( (char *)(project_name+"/shaders/LYM_Final_"+project_name+"-FS_full.glsl").c_str() , pg_fs_Final);
  glCompileShader (pg_fs_Final);
  pg_printLog(pg_fs_Final);

#ifdef PG_TRACK_DISPLAY
  unsigned int pg_fs_TrackDisplay = glCreateShader (GL_FRAGMENT_SHADER);
  pg_loadshader( (char *)(project_name+"/shaders/LYM_TrackDisplay_"+project_name+"-FS_full.glsl").c_str() , pg_fs_TrackDisplay);
  glCompileShader (pg_fs_TrackDisplay);
  pg_printLog(pg_fs_TrackDisplay);
#endif

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

#ifdef PG_TRACK_DISPLAY
  shader_TrackDisplay_programme = glCreateProgram ();
  glAttachShader (shader_TrackDisplay_programme, pg_fs_TrackDisplay);
  glAttachShader (shader_TrackDisplay_programme, pg_gs_TrackDisplay);
  glAttachShader (shader_TrackDisplay_programme, pg_vs_TrackDisplay);
  glLinkProgram (shader_TrackDisplay_programme);
#endif

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
#ifdef PG_WITH_CA
//  uniform_Drawing_fs_4fv_CAdecay_clearCA_flashCACoefs
//	  = glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_fs_4fv_CAdecay_clearCA_flashCACoefs");
#endif
  uniform_Drawing_fs_4fv_weights03
	  = glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_fs_4fv_weights03");
  uniform_Drawing_fs_2fv_weights45
	  = glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_fs_2fv_weights45");
  uniform_Drawing_fs_3fv_alphaSwap02
	  = glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_fs_3fv_alphaSwap02");
  uniform_Drawing_fs_3fv_alphaSwap35
	  = glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_fs_3fv_alphaSwap35");
  uniform_Drawing_fs_4fv_image_noisesxy
    = glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_fs_4fv_image_noisesxy");
  uniform_Drawing_fs_4fv_mask_noisesxy
	  = glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_fs_4fv_mask_noisesxy");
  uniform_Drawing_fs_4fv_flashPart_flashTrack0Coefs_copyTrack12
    = glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_fs_4fv_flashPart_flashTrack0Coefs_copyTrack12");
  uniform_Drawing_fs_3fv_clearLayer_flashVideoCoef_flashVideoThreshold
    = glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_fs_3fv_clearLayer_flashVideoCoef_flashVideoThreshold");
//  uniform_Drawing_fs_4fv_mouseTracks_xy_prevxy
//    = glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_fs_4fv_mouseTracks_xy_prevxy");
  uniform_Drawing_fs_4fv_flashBackCoef_videoWH_videoThreshold
    = glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_fs_4fv_flashBackCoef_videoWH_videoThreshold");
  uniform_Drawing_fs_4fv_mouseTracks_Color_rgba
    = glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_fs_4fv_mouseTracks_Color_rgba");
  uniform_Drawing_fs_4fv_partAcc_clearAllLayers_frameNo_time
    = glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_fs_4fv_partAcc_clearAllLayers_frameNo_time");
//  uniform_Drawing_fs_4fv_mouseTracks_RadiusXY_isNewBeat_Cursor
//    = glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_fs_4fv_mouseTracks_RadiusXY_isNewBeat_Cursor");
  uniform_Drawing_fs_4fv_pulse
    = glGetUniformLocation(shader_Drawing_programme, "uniform_Drawing_fs_4fv_pulse");

  // BG track FBO
  uniform_Drawing_texture_fs_decal
    = glGetUniformLocation(shader_Drawing_programme, "fs_decal" );         // 2-cycle ping-pong BG track step n-1 (FBO)
  // particle FBO
  uniform_Drawing_texture_fs_lookupTable1
    = glGetUniformLocation(shader_Drawing_programme, "fs_lookupTable1" );  // 2-cycle ping-pong speed/position of particles (FBO)
#ifdef PG_WITH_CA
// CA FBO
//uniform_Drawing_texture_fs_lookupTable2
//    = glGetUniformLocation(shader_Drawing_programme, "fs_lookupTable2" );  // ping-pong CA (FBO)
// track FBO
#endif
#if PG_NB_TRACKS >= 2
  uniform_Drawing_texture_fs_lookupTable3
    = glGetUniformLocation(shader_Drawing_programme, "fs_lookupTable3" );  // ping-pong track 1 (FBO)
#endif
#if PG_NB_TRACKS >= 3
  uniform_Drawing_texture_fs_lookupTable4
    = glGetUniformLocation(shader_Drawing_programme, "fs_lookupTable4" );  // ping-pong track 2 (FBO)
#endif
  // textures
//  uniform_Drawing_texture_fs_lookupTable5
//    = glGetUniformLocation(shader_Drawing_programme, "fs_lookupTable5" );  // pen patterns
  uniform_Drawing_texture_fs_lookupTable6
    = glGetUniformLocation(shader_Drawing_programme, "fs_lookupTable6" );  // particle acceleration texture
#ifdef PG_VIDEO_ACTIVE
  // video frame
  uniform_Drawing_texture_fs_lookupTable7
	  = glGetUniformLocation(shader_Drawing_programme, "fs_lookupTable7");  // video frame
  // current background frame
  //  uniform_Drawing_texture_fs_lookupTable8
  //	  = glGetUniformLocation(shader_Drawing_programme, "fs_lookupTable8");  // current bakcground video frame
  // initial background frame
  //  uniform_Drawing_texture_fs_lookupTable9
  //	  = glGetUniformLocation(shader_Drawing_programme, "fs_lookupTable9");  // initial bakcground video frame
#endif
  // image buffer 0 -> 5
  uniform_Drawing_texture_fs_lookupTable10
	  = glGetUniformLocation(shader_Drawing_programme, "fs_lookupTable10");  // image buffer #0
  uniform_Drawing_texture_fs_lookupTable11
	  = glGetUniformLocation(shader_Drawing_programme, "fs_lookupTable11");  // image buffer #1
  uniform_Drawing_texture_fs_lookupTable12
	  = glGetUniformLocation(shader_Drawing_programme, "fs_lookupTable12");  // image buffer #2
  uniform_Drawing_texture_fs_lookupTable13
	  = glGetUniformLocation(shader_Drawing_programme, "fs_lookupTable13");  // image buffer #3
  uniform_Drawing_texture_fs_lookupTable14
	  = glGetUniformLocation(shader_Drawing_programme, "fs_lookupTable14");  // image buffer #4
  uniform_Drawing_texture_fs_lookupTable15
	  = glGetUniformLocation(shader_Drawing_programme, "fs_lookupTable15");  // image buffer #5
  uniform_Drawing_texture_fs_lookupTable16
	  = glGetUniformLocation(shader_Drawing_programme, "fs_lookupTable16");  // image mask #0
  uniform_Drawing_texture_fs_lookupTable17
	  = glGetUniformLocation(shader_Drawing_programme, "fs_lookupTable17");  // image mask #1
  //uniform_Drawing_texture_fs_lookupTable18
	 // = glGetUniformLocation(shader_Drawing_programme, "fs_lookupTable18");  // image mask #2
  //uniform_Drawing_texture_fs_lookupTable19
	 // = glGetUniformLocation(shader_Drawing_programme, "fs_lookupTable19");  // image mask #3
  uniform_Drawing_texture_fs_lookupTable20
	  = glGetUniformLocation(shader_Drawing_programme, "fs_lookupTable20");  // buffer swap image #0
  uniform_Drawing_texture_fs_lookupTable21
	  = glGetUniformLocation(shader_Drawing_programme, "fs_lookupTable21");  // buffer swap image #1
  uniform_Drawing_texture_fs_lookupTable22
	  = glGetUniformLocation(shader_Drawing_programme, "fs_lookupTable22");  // buffer swap image #2
  uniform_Drawing_texture_fs_lookupTable23
	  = glGetUniformLocation(shader_Drawing_programme, "fs_lookupTable23");  // buffer swap image #3
  uniform_Drawing_texture_fs_lookupTable24
	  = glGetUniformLocation(shader_Drawing_programme, "fs_lookupTable24");  // buffer swap image #4
  uniform_Drawing_texture_fs_lookupTable25
	  = glGetUniformLocation(shader_Drawing_programme, "fs_lookupTable25");  // buffer swap image #5

  if( (uniform_Drawing_vp_model == -1)
      || (uniform_Drawing_vp_view == -1)
      || (uniform_Drawing_vp_proj == -1)
      || (uniform_Drawing_vp_2fv_width_height == -1)

      || (uniform_Drawing_fs_4fv_W_H_drawingStart_drawingStroke == -1)
#ifdef PG_WITH_CA
//	  || (uniform_Drawing_fs_4fv_CAdecay_clearCA_flashCACoefs == -1)
#endif

	  || (uniform_Drawing_fs_4fv_weights03 == -1)
	  || (uniform_Drawing_fs_2fv_weights45 == -1)
	  || (uniform_Drawing_fs_3fv_alphaSwap02 == -1)
	  || (uniform_Drawing_fs_3fv_alphaSwap35 == -1)
	  || (uniform_Drawing_fs_4fv_image_noisesxy == -1)
	  || (uniform_Drawing_fs_4fv_mask_noisesxy == -1)
	  || (uniform_Drawing_fs_4fv_flashPart_flashTrack0Coefs_copyTrack12 == -1)
      || (uniform_Drawing_fs_3fv_clearLayer_flashVideoCoef_flashVideoThreshold == -1)
//      || (uniform_Drawing_fs_4fv_mouseTracks_xy_prevxy == -1)
      || (uniform_Drawing_fs_4fv_flashBackCoef_videoWH_videoThreshold == -1)
      //|| (uniform_Drawing_fs_3iv_mouseTracks_x_transl == -1)
      //|| (uniform_Drawing_fs_3iv_mouseTracks_y_transl == -1)
      || (uniform_Drawing_fs_4fv_mouseTracks_Color_rgba == -1)
      || (uniform_Drawing_fs_4fv_partAcc_clearAllLayers_frameNo_time == -1)
//      || (uniform_Drawing_fs_4fv_mouseTracks_RadiusXY_isNewBeat_Cursor == -1)
      || (uniform_Drawing_fs_4fv_pulse == -1)

      || (uniform_Drawing_texture_fs_decal == -1)
      || (uniform_Drawing_texture_fs_lookupTable1 == -1)
#ifdef PG_WITH_CA
//      || (uniform_Drawing_texture_fs_lookupTable2 == -1)
#endif
#if PG_NB_TRACKS >= 2
      || (uniform_Drawing_texture_fs_lookupTable3 == -1)
#endif
#if PG_NB_TRACKS >= 3
      || (uniform_Drawing_texture_fs_lookupTable4 == -1)
#endif
//      || (uniform_Drawing_texture_fs_lookupTable5 == -1)
	  || (uniform_Drawing_texture_fs_lookupTable6 == -1)
#ifdef PG_VIDEO_ACTIVE
	  || (uniform_Drawing_texture_fs_lookupTable7 == -1)
    //	  || (uniform_Drawing_texture_fs_lookupTable8 == -1)
    //	  || (uniform_Drawing_texture_fs_lookupTable9 == -1)
#endif
	  || (uniform_Drawing_texture_fs_lookupTable10 == -1)
	  || (uniform_Drawing_texture_fs_lookupTable11 == -1)
	  || (uniform_Drawing_texture_fs_lookupTable12 == -1)
	  || (uniform_Drawing_texture_fs_lookupTable13 == -1)
	  || (uniform_Drawing_texture_fs_lookupTable14 == -1)
	  || (uniform_Drawing_texture_fs_lookupTable15 == -1)
	  || (uniform_Drawing_texture_fs_lookupTable16 == -1)
	  || (uniform_Drawing_texture_fs_lookupTable17 == -1)
	  //|| (uniform_Drawing_texture_fs_lookupTable18 == -1)
	  //|| (uniform_Drawing_texture_fs_lookupTable19 == -1)
	  || (uniform_Drawing_texture_fs_lookupTable20 == -1)
	  || (uniform_Drawing_texture_fs_lookupTable21 == -1)
	  || (uniform_Drawing_texture_fs_lookupTable22 == -1)
	  || (uniform_Drawing_texture_fs_lookupTable23 == -1)
	  || (uniform_Drawing_texture_fs_lookupTable24 == -1)
	  || (uniform_Drawing_texture_fs_lookupTable25 == -1)
	  ) {
	  fprintf(stderr, "Could not bind uniforms Drawing uniform_Drawing_vp_model : %d, uniform_Drawing_vp_view : %d, uniform_Drawing_vp_proj : %d, uniform_Drawing_vp_2fv_width_height : %d, uniform_Drawing_fs_4fv_W_H_drawingStart_drawingStroke : %d, uniform_Drawing_fs_4fv_weights03 : %d, \n uniform_Drawing_fs_2fv_weights45 : %d, uniform_Drawing_fs_3fv_alphaSwap02 : %d, uniform_Drawing_fs_3fv_alphaSwap35 : %d, uniform_Drawing_fs_4fv_image_noisesxy : %d, uniform_Drawing_fs_4fv_mask_noisesxy : %d, uniform_Drawing_fs_4fv_flashPart_flashTrack0Coefs_copyTrack12 : %d, uniform_Drawing_fs_3fv_clearLayer_flashVideoCoef_flashVideoThreshold : %d\n uniform_Drawing_fs_4fv_flashBackCoef_videoWH_videoThreshold : %d, uniform_Drawing_fs_4fv_mouseTracks_Color_rgba : %d, uniform_Drawing_fs_4fv_partAcc_clearAllLayers_frameNo_time : %d, uniform_Drawing_fs_4fv_pulse : %d, uniform_Drawing_texture_fs_decal : %d, uniform_Drawing_texture_fs_lookupTable1 : %d, uniform_Drawing_texture_fs_lookupTable6 : %d\n", uniform_Drawing_vp_model, uniform_Drawing_vp_view, uniform_Drawing_vp_proj, uniform_Drawing_vp_2fv_width_height, uniform_Drawing_fs_4fv_W_H_drawingStart_drawingStroke, uniform_Drawing_fs_4fv_weights03, uniform_Drawing_fs_2fv_weights45, uniform_Drawing_fs_3fv_alphaSwap02, uniform_Drawing_fs_3fv_alphaSwap35, uniform_Drawing_fs_4fv_image_noisesxy, uniform_Drawing_fs_4fv_mask_noisesxy, uniform_Drawing_fs_4fv_flashPart_flashTrack0Coefs_copyTrack12, uniform_Drawing_fs_3fv_clearLayer_flashVideoCoef_flashVideoThreshold, uniform_Drawing_fs_4fv_flashBackCoef_videoWH_videoThreshold, uniform_Drawing_fs_4fv_mouseTracks_Color_rgba, uniform_Drawing_fs_4fv_partAcc_clearAllLayers_frameNo_time, uniform_Drawing_fs_4fv_pulse, uniform_Drawing_texture_fs_decal, uniform_Drawing_texture_fs_lookupTable1, uniform_Drawing_texture_fs_lookupTable6 );
#ifdef PG_WITH_CA
//	  fprintf(stderr, "Could not bind uniforms uniform_Drawing_texture_fs_lookupTable2 : %d\n", uniform_Drawing_texture_fs_lookupTable2);
#endif
#if PG_NB_TRACKS >= 2
	  fprintf(stderr, "Could not bind uniforms uniform_Drawing_texture_fs_lookupTable3 : %d\n", uniform_Drawing_texture_fs_lookupTable3);
#endif
#if PG_NB_TRACKS >= 3
	  fprintf(stderr, "Could not bind uniforms uniform_Drawing_texture_fs_lookupTable4 : %d\n", uniform_Drawing_texture_fs_lookupTable4);
#endif
#ifdef PG_VIDEO_ACTIVE
//	  fprintf(stderr, "Could not bind uniforms uniform_Drawing_texture_fs_lookupTable7 : %d, uniform_Drawing_texture_fs_lookupTable8 : %d, uniform_Drawing_texture_fs_lookupTable9 : %d\n", uniform_Drawing_texture_fs_lookupTable7, uniform_Drawing_texture_fs_lookupTable8, uniform_Drawing_texture_fs_lookupTable9);
	  fprintf(stderr, "Could not bind uniforms uniform_Drawing_texture_fs_lookupTable7 : %d\n", uniform_Drawing_texture_fs_lookupTable7);
#endif
	  fprintf(stderr, "Could not bind uniforms uniform_Drawing_fs_3fv_mouseTracuniform_Drawing_texture_fs_lookupTable10 : %d, uniform_Drawing_texture_fs_lookupTable11 : %d, uniform_Drawing_texture_fs_lookupTable12 : %d, uniform_Drawing_texture_fs_lookupTable13 : %d, uniform_Drawing_texture_fs_lookupTable14 : %d, \n uniform_Drawing_texture_fs_lookupTable15 : %d, uniform_Drawing_texture_fs_lookupTable16 : %d, uniform_Drawing_texture_fs_lookupTable17 : %d, uniform_Drawing_texture_fs_lookupTable20 : %d, uniform_Drawing_texture_fs_lookupTable21 : %d, uniform_Drawing_texture_fs_lookupTable22 : %d, uniform_Drawing_texture_fs_lookupTable23 : %d, \n uniform_Drawing_texture_fs_lookupTable24 : %d, uniform_Drawing_texture_fs_lookupTable25 : %d\n", uniform_Drawing_texture_fs_lookupTable10, uniform_Drawing_texture_fs_lookupTable11, uniform_Drawing_texture_fs_lookupTable12, uniform_Drawing_texture_fs_lookupTable13, uniform_Drawing_texture_fs_lookupTable14, uniform_Drawing_texture_fs_lookupTable15, uniform_Drawing_texture_fs_lookupTable16, uniform_Drawing_texture_fs_lookupTable17, uniform_Drawing_texture_fs_lookupTable20, uniform_Drawing_texture_fs_lookupTable21, uniform_Drawing_texture_fs_lookupTable22, uniform_Drawing_texture_fs_lookupTable23, uniform_Drawing_texture_fs_lookupTable24, uniform_Drawing_texture_fs_lookupTable25);
 }

  ////////////////////////////////////////
  // shader parameeter bindings
  uniform_Composition_vp_model
    = glGetUniformLocation(shader_Composition_programme, "vp_modelMatrix");
  uniform_Composition_vp_view
    = glGetUniformLocation(shader_Composition_programme, "vp_viewMatrix");
  uniform_Composition_vp_proj
    = glGetUniformLocation(shader_Composition_programme, "vp_projMatrix");

  uniform_Composition_fs_4fv_height_screenMsgTransp_Text1_2_Alpha
	  = glGetUniformLocation(shader_Composition_programme, "uniform_Composition_fs_4fv_height_screenMsgTransp_Text1_2_Alpha");
  uniform_Composition_fs_3fv_flashVideoCoef_flashVideoThreshold_width
	  = glGetUniformLocation(shader_Composition_programme, "uniform_Composition_fs_3fv_flashVideoCoef_flashVideoThreshold_width");

  uniform_Composition_texture_fs_decal
    = glGetUniformLocation(shader_Composition_programme, "fs_decal" ); // ping-pong BG track (FBO)
#ifdef PG_WITH_CA
  uniform_Composition_texture_fs_lookupTable1
    = glGetUniformLocation(shader_Composition_programme, "fs_lookupTable1" );  // ping-pong CA (FBO)
#endif
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
  //uniform_Composition_texture_fs_lookupTable5
	 // = glGetUniformLocation(shader_Composition_programme, "fs_lookupTable5");  // captured frame buffer
  uniform_Composition_texture_fs_lookupTable_screen_font
	  = glGetUniformLocation(shader_Composition_programme, "fs_lookupTable_screen_font");  // message Font
  uniform_Composition_texture_fs_lookupTable_screen_msg_offset
	  = glGetUniformLocation(shader_Composition_programme, "fs_lookupTable_screen_msg_offset");  // message string
  uniform_Composition_texture_fs_lookupTable_displayMsg_font
	  = glGetUniformLocation(shader_Composition_programme, "fs_lookupTable_displayMsg_font");  // message Font
  uniform_Composition_texture_fs_lookupTable_displayMsg1_offset
	  = glGetUniformLocation(shader_Composition_programme, "fs_lookupTable_displayMsg1_offset");  // message string
  uniform_Composition_texture_fs_lookupTable_displayMsg2_offset
	  = glGetUniformLocation(shader_Composition_programme, "fs_lookupTable_displayMsg2_offset");  // message string
  uniform_Composition_texture_fs_lookupTable_hashtag_font
	  = glGetUniformLocation(shader_Composition_programme, "fs_lookupTable_hashtag_font");  // message Font
  uniform_Composition_texture_fs_lookupTable_hashtag_offset
	  = glGetUniformLocation(shader_Composition_programme, "fs_lookupTable_hashtag_offset");  // message string
  if ((uniform_Composition_vp_proj == -1)
       || (uniform_Composition_vp_view == -1)
       || (uniform_Composition_vp_model == -1)
       || (uniform_Composition_fs_4fv_height_screenMsgTransp_Text1_2_Alpha == -1)
	   || (uniform_Composition_fs_3fv_flashVideoCoef_flashVideoThreshold_width == -1)
       || (uniform_Composition_texture_fs_decal == -1)
       // || (uniform_Composition_texture_fs_lookupTable1 == -1)
#if PG_NB_TRACKS >= 2
       || (uniform_Composition_texture_fs_lookupTable2 == -1)
#endif
#if PG_NB_TRACKS >= 3
       || (uniform_Composition_texture_fs_lookupTable3 == -1)
#endif
       || (uniform_Composition_texture_fs_lookupTable4 == -1)
       //|| (uniform_Composition_texture_fs_lookupTable5 == -1)
	   || (uniform_Composition_texture_fs_lookupTable_screen_font == -1)
	   || (uniform_Composition_texture_fs_lookupTable_screen_msg_offset == -1)
	  || (uniform_Composition_texture_fs_lookupTable_displayMsg_font == -1)
	  || (uniform_Composition_texture_fs_lookupTable_displayMsg1_offset == -1)
	  || (uniform_Composition_texture_fs_lookupTable_displayMsg2_offset == -1)
	  || (uniform_Composition_texture_fs_lookupTable_hashtag_font == -1)
	  || (uniform_Composition_texture_fs_lookupTable_hashtag_offset == -1)
	  ) {
#if PG_NB_TRACKS >= 2
	  fprintf(stderr, "Could not bind uniforms uniform_Composition_texture_fs_lookupTable2 : %d\n", uniform_Composition_texture_fs_lookupTable2 );
#endif
#if PG_NB_TRACKS >= 3
	  fprintf(stderr, "Could not bind uniforms uniform_Composition_texture_fs_lookupTable3 : %d\n", uniform_Composition_texture_fs_lookupTable3 );
#endif
	  fprintf(stderr, "Could not bind uniforms Composition uniform_Composition_vp_proj : %d, uniform_Composition_vp_view : %d, uniform_Composition_vp_model : %d, uniform_Composition_fs_4fv_height_screenMsgTransp_Text1_2_Alpha : %d, uniform_Composition_fs_3fv_flashVideoCoef_flashVideoThreshold_width : %d, uniform_Composition_texture_fs_decal : %d, uniform_Composition_texture_fs_lookupTable4 : %d, uniform_Composition_texture_fs_lookupTable_screen_font : %d, uniform_Composition_texture_fs_lookupTable_screen_msg_offset : %d, uniform_Composition_texture_fs_lookupTable_displayMsg_font : %d, uniform_Composition_texture_fs_lookupTable_displayMsg1_offset : %d, uniform_Composition_texture_fs_lookupTable_displayMsg2_offset : %d, uniform_Composition_texture_fs_lookupTable_hashtag_font : %d, uniform_Composition_texture_fs_lookupTable_hashtag_offset : %d\n", uniform_Composition_vp_proj, uniform_Composition_vp_view, uniform_Composition_vp_model, uniform_Composition_fs_4fv_height_screenMsgTransp_Text1_2_Alpha, uniform_Composition_fs_3fv_flashVideoCoef_flashVideoThreshold_width, uniform_Composition_texture_fs_decal, uniform_Composition_texture_fs_lookupTable4, uniform_Composition_texture_fs_lookupTable_screen_font, uniform_Composition_texture_fs_lookupTable_screen_msg_offset, uniform_Composition_texture_fs_lookupTable_displayMsg_font, uniform_Composition_texture_fs_lookupTable_displayMsg1_offset, uniform_Composition_texture_fs_lookupTable_displayMsg2_offset, uniform_Composition_texture_fs_lookupTable_hashtag_font, uniform_Composition_texture_fs_lookupTable_hashtag_offset);
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
#ifdef PG_TRACK_DISPLAY
  uniform_Final_texture_fs_lookupTable1
	  = glGetUniformLocation(shader_Final_programme, "fs_lookupTable1"); // previous display tracks output
#endif
  uniform_Final_texture_fs_lookupTable2
	  = glGetUniformLocation(shader_Final_programme, "fs_lookupTable2"); // ping-pong BG track (FBO)
  //uniform_Final_texture_fs_lookupTable3
	 // = glGetUniformLocation(shader_Final_programme, "fs_lookupTable3"); // ping-pong CA (FBO)
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
#ifdef PG_TRACK_DISPLAY
	   || (uniform_Final_texture_fs_lookupTable1 == -1)
#endif
	   || (uniform_Final_texture_fs_lookupTable2 == -1)
#ifdef PG_WITH_CA
	  || (uniform_Final_texture_fs_lookupTable3 == -1)
#endif
#if PG_NB_TRACKS >= 2
	   || (uniform_Final_texture_fs_lookupTable4 == -1)
#endif
#if PG_NB_TRACKS >= 3
	   || (uniform_Final_texture_fs_lookupTable5 == -1)
#endif
	   || (uniform_Final_fs_3fv_xy_frameno == -1)
       || (uniform_Final_fs_2fv_width_height == -1)
       ) {
	  fprintf(stderr, "Could not bind uniforms Final uniform_Final_vp_proj : %d, uniform_Final_vp_view : %d, uniform_Final_vp_model : %d , uniform_Final_texture_fs_decal: %d, uniform_Final_texture_fs_lookupTable2: %d, uniform_Final_fs_3fv_xy_frameno : %d, uniform_Final_fs_2fv_width_height : %d\n", uniform_Final_vp_proj, uniform_Final_vp_view, uniform_Final_vp_model, uniform_Final_texture_fs_decal, uniform_Final_texture_fs_lookupTable2, uniform_Final_fs_3fv_xy_frameno, uniform_Final_fs_2fv_width_height);
#ifdef PG_TRACK_DISPLAY
	  fprintf(stderr, "Could not bind uniforms Final uniform_Final_texture_fs_lookupTable1: %d\n", uniform_Final_texture_fs_lookupTable1);
#endif
#if PG_NB_TRACKS >= 2
	  fprintf(stderr, "Could not bind uniforms Final uniform_Final_texture_fs_lookupTable4: %d\n", uniform_Final_texture_fs_lookupTable4);
#endif
#if PG_NB_TRACKS >= 3
	  fprintf(stderr, "Could not bind uniforms Final uniform_Final_texture_fs_lookupTable5: %d\n", uniform_Final_texture_fs_lookupTable5);
#endif
    // return 0;
  }

#ifdef PG_TRACK_DISPLAY
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
#endif

}
