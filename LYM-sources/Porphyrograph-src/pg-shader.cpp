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
#ifdef GN
#include "pg_shader_body_decl_GN.cpp"
#endif
#ifdef TVW
#include "pg_shader_body_decl_TVW.cpp"
#endif
#ifdef CRITON
#include "pg_shader_body_decl_Criton.cpp"
#endif
#if defined (KOMPARTSD)
#include "pg_shader_body_decl_KompartSD.cpp"
#endif
#ifdef effe
#include "pg_shader_body_decl_effe.cpp"
#endif
#ifdef DEMO
#include "pg_shader_body_decl_demo.cpp"
#endif
#ifdef VOLUSPA
#include "pg_shader_body_decl_voluspa.cpp"
#endif
#ifdef INTERFERENCE
#include "pg_shader_body_decl_interference.cpp"
#endif
#ifdef MALAUSSENA
#include "pg_shader_body_decl_Malaussena.cpp"
#endif
#ifdef DASEIN
#include "pg_shader_body_decl_dasein.cpp"
#endif
#ifdef BONNOTTE
#include "pg_shader_body_decl_bonnotte.cpp"
#endif

/////////////////////////////////////////////////////////////////////////
// SHADER PROGRAMMES
unsigned int shader_programme[pg_NbShaderTotal];

// PARTICLE ANIMATION SHADER
// PARTICLE ANIMATION SHADER UNIFORM VARIABLES
GLint uniform_ParticleAnimation_vp_model;
GLint uniform_ParticleAnimation_vp_view;
GLint uniform_ParticleAnimation_vp_proj;
GLint uniform_ParticleAnimation_vp_2fv_width_height;
GLint uniform_ParticleAnimation_fs_4fv_W_H_repopChannel_targetFrameNo;
GLint uniform_ParticleAnimation_fs_1fv_partDecay;
GLint uniform_ParticleAnimation_fs_4fv_flashTrkPartWghts;
#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7
GLint uniform_ParticleAnimation_fs_4fv_paths03_x;
GLint uniform_ParticleAnimation_fs_4fv_paths03_y;
GLint uniform_ParticleAnimation_fs_4fv_paths03_x_prev;
GLint uniform_ParticleAnimation_fs_4fv_paths03_y_prev;
GLint uniform_ParticleAnimation_fs_4fv_paths03_RadiusX;
#endif
#if PG_NB_PATHS == 7
GLint uniform_ParticleAnimation_fs_4fv_paths47_x;
GLint uniform_ParticleAnimation_fs_4fv_paths47_y;
GLint uniform_ParticleAnimation_fs_4fv_paths47_x_prev;
GLint uniform_ParticleAnimation_fs_4fv_paths47_y_prev;
GLint uniform_ParticleAnimation_fs_4fv_paths47_RadiusX;
#endif
GLint uniform_ParticleAnimation_fs_4fv_repop_Color_frameNo;
GLint uniform_ParticleAnimation_fs_4fv_flashCAPartWght_nbPart_clear_partRadius;
GLint uniform_ParticleAnimation_fs_4fv_repop_part_path_acc_damp_factor;
GLint uniform_ParticleAnimation_fs_4fv_Camera_W_H_movieWH;

// UPDATE SHADER TEXTURE IDS
GLint uniform_ParticleAnimation_texture_fs_CA;         // ping-pong CA (FBO)
GLint uniform_ParticleAnimation_texture_fs_Part_pos_speed;  // 2-cycle ping-pong position/speed of particles (FBO)
GLint uniform_ParticleAnimation_texture_fs_Part_col_rad;  // 2-cycle ping-pong color/radius of particles (FBO)
GLint uniform_ParticleAnimation_texture_fs_Part_Target_pos_col_rad;  // 2-cycle ping-pong target position/color/radius of particles (FBO)
GLint uniform_ParticleAnimation_texture_fs_Camera_frame;  // camera frame
GLint uniform_ParticleAnimation_texture_fs_Movie_frame;  // movie frame
GLint uniform_ParticleAnimation_texture_fs_Noise;  // 3D noise
GLint uniform_ParticleAnimation_texture_fs_Part_init_pos_speed;  // particle initialization pairs of textures position/speed
GLint uniform_ParticleAnimation_texture_fs_Part_init_col_rad;  // particle initialization pairs of textures color/radius
GLint uniform_ParticleAnimation_texture_fs_Trk0;  // ping-pong track 0 (FBO)
#if PG_NB_TRACKS >= 2
GLint uniform_ParticleAnimation_texture_fs_Trk1;  // ping-pong track 1 (FBO)
#endif
#if PG_NB_TRACKS >= 3
GLint uniform_ParticleAnimation_texture_fs_Trk2;  // ping-pong track 2 (FBO)
#endif
#if PG_NB_TRACKS >= 4
GLint uniform_ParticleAnimation_texture_fs_Trk3;  // ping-pong track 3 (FBO)
#endif

/////////////////////////////////////////////////////////////////////////
// UPDATE SHADER
// UPDATE SHADER UNIFORM VARIABLES
GLint uniform_Update_vp_model;
GLint uniform_Update_vp_view;
GLint uniform_Update_vp_proj;
GLint uniform_Update_vp_2fv_width_height;
GLint uniform_Update_fs_4fv_W_H_time_currentScene;
GLint uniform_Update_fs_4fv_clearAllLayers_clearCA_pixelRadius_pulsedShift;
#ifdef MALAUSSENA
GLint uniform_Update_fs_4fv_CAseed_type_size_loc;
#endif
#if defined (BLURRED_SPLAT_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) || PG_NB_TRACKS >= 2
GLint uniform_Update_fs_4fv_flashTrkBGWghts_flashPartBGWght;
#endif
GLint uniform_Update_fs_4fv_trkDecay;
GLint uniform_Update_fs_4fv_CAdecay_frameno_Cursor_flashPartCAWght;
#ifdef PG_NB_CA_TYPES
GLint uniform_Update_fs_4fv_flashTrkCAWghts;
#endif
#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7
GLint uniform_Update_fs_4fv_paths03_x;
GLint uniform_Update_fs_4fv_paths03_y;
GLint uniform_Update_fs_4fv_paths03_x_prev;
GLint uniform_Update_fs_4fv_paths03_y_prev;
#ifdef PG_BEZIER_CURVES
// pen Bezier curve tangents
GLint uniform_Update_fs_4fv_paths03_xL;
GLint uniform_Update_fs_4fv_paths03_yL;
GLint uniform_Update_fs_4fv_paths03_xR;
GLint uniform_Update_fs_4fv_paths03_yR;
GLint uniform_Update_fs_4iv_path0_next_in_hull;
#endif
GLint uniform_Update_fs_4fv_paths03_r;
GLint uniform_Update_fs_4fv_paths03_g;
GLint uniform_Update_fs_4fv_paths03_b;
GLint uniform_Update_fs_4fv_paths03_a;
GLint uniform_Update_fs_4fv_paths03_BrushID;
GLint uniform_Update_fs_4fv_paths03_RadiusX;
#ifndef PG_BEZIER_CURVES
GLint uniform_Update_fs_4fv_paths03_RadiusY;
#endif
#endif
#if PG_NB_PATHS == 7
GLint uniform_Update_fs_4fv_paths47_x;
GLint uniform_Update_fs_4fv_paths47_y;
GLint uniform_Update_fs_4fv_paths47_x_prev;
GLint uniform_Update_fs_4fv_paths47_y_prev;
#ifdef PG_BEZIER_CURVES
// pen Bezier curve tangents
GLint uniform_Update_fs_4fv_paths47_xL;
GLint uniform_Update_fs_4fv_paths47_yL;
GLint uniform_Update_fs_4fv_paths47_xR;
GLint uniform_Update_fs_4fv_paths47_yR;
#endif
GLint uniform_Update_fs_4fv_paths47_r;
GLint uniform_Update_fs_4fv_paths47_g;
GLint uniform_Update_fs_4fv_paths47_b;
GLint uniform_Update_fs_4fv_paths47_a;
GLint uniform_Update_fs_4fv_paths47_BrushID;
GLint uniform_Update_fs_4fv_paths47_RadiusX;
#ifndef PG_BEZIER_CURVES
GLint uniform_Update_fs_4fv_paths47_RadiusY;
#endif
#endif
#ifdef CRITON
GLint uniform_Update_fs_4fv_fftLevels03;
GLint uniform_Update_fs_4fv_fftFrequencies03;
GLint uniform_Update_fs_4fv_fftPhases03;
GLint uniform_Update_fs_4fv_fftLevels47;
GLint uniform_Update_fs_4fv_fftFrequencies47;
GLint uniform_Update_fs_4fv_fftPhases47;
#endif
GLint uniform_Update_fs_4fv_pulse;
GLint uniform_Update_fs_4fv_movieWH_flashCameraTrkWght_cpTrack;
GLint uniform_Update_fs_4fv_repop_Color_flashCABGWght;
GLint uniform_Update_fs_3fv_isClearLayer_flashPixel_flashCameraTrkThres;
#if defined (PG_WITH_PHOTO_DIAPORAMA) || defined (TVW)
GLint uniform_Update_fs_4fv_photo01_wh;
#endif
#ifdef PG_WITH_PHOTO_DIAPORAMA
GLint uniform_Update_fs_2fv_photo01Wghts;
#endif
GLint uniform_Update_fs_4fv_Camera_offSetsXY_Camera_W_H;
GLint uniform_Update_fs_4fv_xy_transl_tracks_0_1;
#if defined (PG_NB_CA_TYPES) || defined (PG_WITH_BLUR)
GLint uniform_Update_fs_4fv_CAType_SubType_blurRadius;
#endif
#if defined (GN) || defined (MALAUSSENA)
GLint uniform_Update_texture_fs_CATable;
#endif
#ifdef GN
GLint uniform_Update_fs_2fv_initCA_1stPlaneFrameNo;
GLint uniform_Update_texture_fs_Camera_BGIni;  // initial camera BG capture
#endif

// UPDATE SHADER TEXTURE IDS
#ifdef PG_NB_CA_TYPES
GLint uniform_Update_texture_fs_CA;         // ping-pong CA (FBO)
#endif
#ifdef PG_NB_PIXEL_MODES
GLint uniform_Update_texture_fs_Pixels;  // 2-cycle ping-pong speed/position of pixels (FBO)
#endif
GLint uniform_Update_texture_fs_Trk0;  // ping-pong track 0 (FBO)
#if PG_NB_TRACKS >= 2
GLint uniform_Update_texture_fs_Trk1;  // ping-pong track 1 (FBO)
#endif
#if PG_NB_TRACKS >= 3
GLint uniform_Update_texture_fs_Trk2;  // ping-pong track 2 (FBO)
#endif
#if PG_NB_TRACKS >= 4
GLint uniform_Update_texture_fs_Trk3;  // ping-pong track 3 (FBO)
#endif
GLint uniform_Update_texture_fs_Brushes;  // pen patterns
GLint uniform_Update_texture_fs_Camera_frame;  // camera frame
GLint uniform_Update_texture_fs_Camera_BG;  // current camera BG capture
GLint uniform_Update_texture_fs_Movie_frame;  // movie frame
GLint uniform_Update_texture_fs_Noise;  // 3D noise
#ifdef PG_WITH_PHOTO_DIAPORAMA
GLint uniform_Update_texture_fs_Photo0;  // photo[0]
GLint uniform_Update_texture_fs_Photo1;  // photo[1]
#endif
#if defined (BLURRED_SPLAT_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES)
GLint uniform_Update_texture_fs_Part_render;  // FBO capture of particle rendering used for flashing layers with particles
#endif
#if defined (TVW)
GLint uniform_Update_fs_4fv_weights03;
GLint uniform_Update_fs_2fv_weights45;
GLint uniform_Update_fs_3fv_alphaSwap02;
GLint uniform_Update_fs_3fv_alphaSwap35;
GLint uniform_Update_fs_4fv_image_noisesxy;
GLint uniform_Update_fs_4fv_mask_noisesxy;

GLint uniform_Update_texture_fs_TVWPixels0;  // image buffer #0
GLint uniform_Update_texture_fs_TVWPixels1;  // image buffer #1
GLint uniform_Update_texture_fs_TVWPixels2;  // image buffer #2
GLint uniform_Update_texture_fs_TVWPixels3;  // image buffer #3
GLint uniform_Update_texture_fs_TVWPixels4;  // image buffer #4
GLint uniform_Update_texture_fs_TVWPixels5;  // image buffer #5
GLint uniform_Update_texture_fs_TVWMask02;  // image mask #0
GLint uniform_Update_texture_fs_TVWMask35;  // image mask #1
GLint uniform_Update_texture_fs_TVWPixelsSwap0;  // buffer swap image #0
GLint uniform_Update_texture_fs_TVWPixelsSwap1;  // buffer swap image #1 
GLint uniform_Update_texture_fs_TVWPixelsSwap2;  // buffer swap image #2 
GLint uniform_Update_texture_fs_TVWPixelsSwap3;  // buffer swap image #3 
GLint uniform_Update_texture_fs_TVWPixelsSwap4;  // buffer swap image #4 
GLint uniform_Update_texture_fs_TVWPixelsSwap5;  // buffer swap image #5 
#endif

/////////////////////////////////////////////////////////////////////////
// PARTICLE SHADER
// PARTICLE SHADER UNIFORM VARIABLES
#if defined (BLURRED_SPLAT_PARTICLES) || defined (LINE_SPLAT_PARTICLES) 
GLint uniform_ParticleSplat_vp_model;
GLint uniform_ParticleSplat_vp_view;
GLint uniform_ParticleSplat_vp_proj;
GLint uniform_ParticleSplat_gs_3fv_partRadius_partType_highPitchPulse;
GLint uniform_ParticleSplat_vp_3fv_trackReplay_xy_height;

// PARTICLE SHADER TEXTURE IDS
GLint uniform_ParticleSplat_texture_vp_Part_pos_speed;				 // Particle position/speed update
GLint uniform_ParticleSplat_texture_vp_Part_col_rad;          // Particle color/radius update
#endif
#if defined (CURVE_PARTICLES) 
GLint uniform_ParticleCurve_vp_model;
GLint uniform_ParticleCurve_vp_view;
GLint uniform_ParticleCurve_vp_proj;
GLint uniform_ParticleCurve_gs_3fv_partRadius_partType_highPitchPulse;
GLint uniform_ParticleCurve_vp_3fv_trackReplay_xy_height;

// PARTICLE SHADER TEXTURE IDS
GLint uniform_ParticleCurve_texture_vp_Part_pos_speed;				 // Particle position/speed update
GLint uniform_ParticleCurve_texture_vp_Part_col_rad;          // Particle color/radius update
#endif
#ifdef CURVE_PARTICLES
// color texture
GLint uniform_ParticleCurve_Comet_texture_fs_decal;  // comet texture
#endif
#ifdef BLURRED_SPLAT_PARTICLES_TEXTURED
// color texture
GLint uniform_ParticleSplat_BlurredDisk_texture_fs_decal;  // blurred disk texture
#endif

/////////////////////////////////////////////////////////////////////////
// MIXING SHADER
// MIXING SHADER UNIFORM VARIABLES
GLint uniform_Mixing_vp_model;
GLint uniform_Mixing_vp_view;
GLint uniform_Mixing_vp_proj;
GLint uniform_Mixing_fs_3fv_pulsedShift_height_flashCameraTrkWght;
GLint uniform_Mixing_fs_3fv_screenMsgTransp_Text1_2_Alpha;

// MIXING SHADER TEXTURE IDS
#ifdef PG_NB_CA_TYPES
GLint uniform_Mixing_texture_fs_CA;         // ping-pong CA (FBO)
#endif
#if defined (BLURRED_SPLAT_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES)
GLint uniform_Mixing_texture_fs_Part_render;  // Particles (FBO)
#endif
GLint uniform_Mixing_texture_fs_Trk0;  // ping-pong track 0 (FBO)
#if PG_NB_TRACKS >= 2
GLint uniform_Mixing_texture_fs_Trk1;  // ping-pong track 1 (FBO)
#endif
#if PG_NB_TRACKS >= 3
GLint uniform_Mixing_texture_fs_Trk2;  // ping-pong track 2 (FBO)
#endif
#if PG_NB_TRACKS >= 4
GLint uniform_Mixing_texture_fs_Trk3;  // ping-pong track 3 (FBO)
#endif
GLint uniform_Mixing_texture_fs_Render_prec;  // preceding snapshot
GLint uniform_Mixing_texture_fs_Screen_Font;  // message Font
GLint uniform_Mixing_texture_fs_Screen_Message;  // message string
#if defined (TVW)
GLint uniform_Mixing_texture_fs_Display_Font;  // display Font
GLint uniform_Mixing_texture_fs_Display_Message1; // tweets at the bottom of the screen
GLint uniform_Mixing_texture_fs_Display_Message2; // tweets at the bottom of the screen
#endif

/////////////////////////////////////////////////////////////////////////
// MASTER SHADER
// MASTER SHADER UNIFORM VARIABLES
GLint uniform_Master_vp_model;
GLint uniform_Master_vp_view;
GLint uniform_Master_vp_proj;
GLint uniform_Master_fs_4fv_xy_frameno_pulsedShift;
GLint uniform_Master_fs_4fv_width_height_rightWindowVMargin_timeFromStart;
GLint uniform_Master_fs_4fv_pulsedColor_rgb_pen_grey;
GLint uniform_Master_fs_3fv_interpolatedPaletteLow_rgb;
GLint uniform_Master_fs_3fv_interpolatedPaletteMedium_rgb;
GLint uniform_Master_fs_3fv_interpolatedPaletteHigh_rgb;

// MASTER SHADER TEXTURE IDS
GLint uniform_Master_texture_fs_Render_curr;         // previous pass output
#ifdef PG_NB_CA_TYPES
GLint uniform_Master_texture_fs_CA;  // ping-pong CA (FBO)
#endif
#if defined (BLURRED_SPLAT_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES)
GLint uniform_Master_texture_fs_Part_render;  // Particles
#endif
GLint uniform_Master_texture_fs_Trk0;  // ping-pong track 0 (FBO)
#if PG_NB_TRACKS >= 2
GLint uniform_Master_texture_fs_Trk1;  // ping-pong track 1 (FBO)
#endif
#if PG_NB_TRACKS >= 3
GLint uniform_Master_texture_fs_Trk2;  // ping-pong track 2 (FBO)
#endif
#if PG_NB_TRACKS >= 4
GLint uniform_Master_texture_fs_Trk3;  // ping-pong track 3 (FBO)
#endif
#if defined (GN) || defined (INTERFERENCE)
GLint uniform_Master_texture_fs_LYMlogo;  // LYM logo (texture)
#endif

#ifdef PG_SENSORS
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
#endif


//////////////////////////////////////////////////////////////
// UTILITIES
unsigned long pg_getFileLength(ifstream& file) {
    if(!file.good()) return 0;
    
    file.seekg(0,ios::end);
    unsigned long len = (long)file.tellg();
    file.seekg(ios::beg);
    
    return len;
}

void pg_printShaderLinkLog(GLuint obj) {
	int infologLength = 0;
	GLint linked;
	if (glIsShader(obj))
		glGetShaderiv(obj, GL_LINK_STATUS, &linked);
	else
		glGetProgramiv(obj, GL_LINK_STATUS, &linked);

	GLsizei maxLength;
	if (glIsShader(obj))
		glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &maxLength);
	else
		glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &maxLength);

	GLchar* infoLog = new GLchar[maxLength + 1];
	glGetProgramInfoLog(obj, maxLength, &maxLength, infoLog);
	if (infologLength > 0)
		printf("Shader linking failed:\n%s\n", infoLog);
	// printf("Shader compilation failed: %s\n", log);
	delete[] infoLog;
}

void pg_printShaderCompileLog(GLuint obj) {
	int infologLength = 0;
	int maxLength;

	if (glIsShader(obj))
		glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &maxLength);
	else
		glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &maxLength);

	char *infoLog = new char[maxLength];

	if (glIsShader(obj))
		glGetShaderInfoLog(obj, maxLength, &infologLength, infoLog);
	else
		glGetProgramInfoLog(obj, maxLength, &infologLength, infoLog);

	if (infologLength > 0)
		printf("Shader compilation failed:\n%s\n", infoLog);

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
    shaderSource[i] = GLchar(file.get());       // get character from file.
    if (!file.eof())
      i++;
  }
  
  shaderSource[i] = 0;  // 0-terminate it at the correct position
  
  file.close();
  
  glShaderSource( shader, 1, (const char **)&shaderSource, NULL );
  
  delete[] shaderSource;
  
  return 0; // No Error
}

void pg_loadAllShaders(void) {
	////////////////////////////////////////
	// loading and compiling shaders
	for (int ind_shader_file = 0; ind_shader_file < pg_NbShaderTotal; ind_shader_file++) {
		shader_programme[ind_shader_file] = glCreateProgram();
		for (int ind_shader_type = 0; ind_shader_type < pg_Shader_nbTypes[ind_shader_file]; ind_shader_type++) {
			string fileShaderName = pg_Shader_Files[ind_shader_file];
			switch (pg_Shader_Types[ind_shader_file][ind_shader_type]) {
			case GL_VERTEX_SHADER:
				fileShaderName += ".vert";
				break;
			case GL_TESS_CONTROL_SHADER:
				fileShaderName += ".tess";
				break;
			case GL_TESS_EVALUATION_SHADER:
				fileShaderName += ".eval";
				break;
			case GL_GEOMETRY_SHADER:
				fileShaderName += ".geom";
				break;
			case GL_FRAGMENT_SHADER:
				fileShaderName += "_full.frag";
				break;
			default:
				sprintf(ErrorStr, "Error: unknown shader type [%d]!", pg_Shader_Types[ind_shader_file][ind_shader_type]); ReportError(ErrorStr); throw 430;
				break;
			}

			unsigned int pg_shader = glCreateShader(pg_Shader_Types[ind_shader_file][ind_shader_type]);
			pg_loadshader((char *)("Projects/shaders/" + fileShaderName).c_str(), pg_shader);
			glCompileShader(pg_shader);
			pg_printShaderCompileLog(pg_shader);

			glAttachShader(shader_programme[ind_shader_file], pg_shader);
			printOglError(330);
		}

		glLinkProgram(shader_programme[ind_shader_file]);
		pg_printShaderLinkLog(shader_programme[ind_shader_file]);
		printOglError(331);

	}

#ifdef GN
#include "pg_shader_body_bind_GN.cpp"
#endif
#ifdef TVW
#include "pg_shader_body_bind_TVW.cpp"
#endif
#ifdef CRITON
#include "pg_shader_body_bind_Criton.cpp"
#endif
#if defined (KOMPARTSD)
#include "pg_shader_body_bind_KompartSD.cpp"
#endif
#ifdef effe
#include "pg_shader_body_bind_effe.cpp"
#endif
#ifdef DEMO
#include "pg_shader_body_bind_demo.cpp"
#endif
#ifdef VOLUSPA
#include "pg_shader_body_bind_voluspa.cpp"
#endif
#ifdef INTERFERENCE
#include "pg_shader_body_bind_interference.cpp"
#endif
#ifdef MALAUSSENA
#include "pg_shader_body_bind_Malaussena.cpp"
#endif
#ifdef DASEIN
#include "pg_shader_body_bind_dasein.cpp"
#endif
#ifdef BONNOTTE
#include "pg_shader_body_bind_bonnotte.cpp"
#endif

#if defined (BLURRED_SPLAT_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 
	////////////////////////////////////////
	// Particle Animation shader parameeter bindings
	uniform_ParticleAnimation_vp_model
		= glGetUniformLocation(shader_programme[pg_shader_ParticleAnimation], "vp_modelMatrix");
	uniform_ParticleAnimation_vp_view
		= glGetUniformLocation(shader_programme[pg_shader_ParticleAnimation], "vp_viewMatrix");
	uniform_ParticleAnimation_vp_proj
		= glGetUniformLocation(shader_programme[pg_shader_ParticleAnimation], "vp_projMatrix");

	uniform_ParticleAnimation_vp_2fv_width_height
		= glGetUniformLocation(shader_programme[pg_shader_ParticleAnimation], "uniform_ParticleAnimation_vp_2fv_width_height");
	uniform_ParticleAnimation_fs_4fv_W_H_repopChannel_targetFrameNo
		= glGetUniformLocation(shader_programme[pg_shader_ParticleAnimation], "uniform_ParticleAnimation_fs_4fv_W_H_repopChannel_targetFrameNo");
	uniform_ParticleAnimation_fs_1fv_partDecay
		= glGetUniformLocation(shader_programme[pg_shader_ParticleAnimation], "uniform_ParticleAnimation_fs_1fv_partDecay");
	uniform_ParticleAnimation_fs_4fv_flashTrkPartWghts
		= glGetUniformLocation(shader_programme[pg_shader_ParticleAnimation], "uniform_ParticleAnimation_fs_4fv_flashTrkPartWghts");
#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7
	uniform_ParticleAnimation_fs_4fv_paths03_x
		= glGetUniformLocation(shader_programme[pg_shader_ParticleAnimation], "uniform_ParticleAnimation_fs_4fv_paths03_x");
	uniform_ParticleAnimation_fs_4fv_paths03_y
		= glGetUniformLocation(shader_programme[pg_shader_ParticleAnimation], "uniform_ParticleAnimation_fs_4fv_paths03_y");
	uniform_ParticleAnimation_fs_4fv_paths03_x_prev
		= glGetUniformLocation(shader_programme[pg_shader_ParticleAnimation], "uniform_ParticleAnimation_fs_4fv_paths03_x_prev");
	uniform_ParticleAnimation_fs_4fv_paths03_y_prev
		= glGetUniformLocation(shader_programme[pg_shader_ParticleAnimation], "uniform_ParticleAnimation_fs_4fv_paths03_y_prev");
	uniform_ParticleAnimation_fs_4fv_paths03_RadiusX
		= glGetUniformLocation(shader_programme[pg_shader_ParticleAnimation], "uniform_ParticleAnimation_fs_4fv_paths03_RadiusX");
#endif
#if PG_NB_PATHS == 7
	uniform_ParticleAnimation_fs_4fv_paths47_x
		= glGetUniformLocation(shader_programme[pg_shader_ParticleAnimation], "uniform_ParticleAnimation_fs_4fv_paths47_x");
	uniform_ParticleAnimation_fs_4fv_paths47_y
		= glGetUniformLocation(shader_programme[pg_shader_ParticleAnimation], "uniform_ParticleAnimation_fs_4fv_paths47_y");
	uniform_ParticleAnimation_fs_4fv_paths47_x_prev
		= glGetUniformLocation(shader_programme[pg_shader_ParticleAnimation], "uniform_ParticleAnimation_fs_4fv_paths47_x_prev");
	uniform_ParticleAnimation_fs_4fv_paths47_y_prev
		= glGetUniformLocation(shader_programme[pg_shader_ParticleAnimation], "uniform_ParticleAnimation_fs_4fv_paths47_y_prev");
	uniform_ParticleAnimation_fs_4fv_paths47_RadiusX
		= glGetUniformLocation(shader_programme[pg_shader_ParticleAnimation], "uniform_ParticleAnimation_fs_4fv_paths47_RadiusX");
#endif
	uniform_ParticleAnimation_fs_4fv_repop_Color_frameNo
		= glGetUniformLocation(shader_programme[pg_shader_ParticleAnimation], "uniform_ParticleAnimation_fs_4fv_repop_Color_frameNo");
	uniform_ParticleAnimation_fs_4fv_flashCAPartWght_nbPart_clear_partRadius
		= glGetUniformLocation(shader_programme[pg_shader_ParticleAnimation], "uniform_ParticleAnimation_fs_4fv_flashCAPartWght_nbPart_clear_partRadius");
	uniform_ParticleAnimation_fs_4fv_repop_part_path_acc_damp_factor
		= glGetUniformLocation(shader_programme[pg_shader_ParticleAnimation], "uniform_ParticleAnimation_fs_4fv_repop_part_path_acc_damp_factor");
	uniform_ParticleAnimation_fs_4fv_Camera_W_H_movieWH
		= glGetUniformLocation(shader_programme[pg_shader_ParticleAnimation], "uniform_ParticleAnimation_fs_4fv_Camera_W_H_movieWH");

	// BG track FBO
	uniform_ParticleAnimation_texture_fs_CA
		= glGetUniformLocation(shader_programme[pg_shader_ParticleAnimation], "uniform_ParticleAnimation_texture_fs_CA");  // 2-cycle ping-pong position/speed of particles (FBO)
	uniform_ParticleAnimation_texture_fs_Part_pos_speed
		= glGetUniformLocation(shader_programme[pg_shader_ParticleAnimation], "uniform_ParticleAnimation_texture_fs_Part_pos_speed");  // 2-cycle ping-pong position/speed of particles (FBO)
	uniform_ParticleAnimation_texture_fs_Part_col_rad
		= glGetUniformLocation(shader_programme[pg_shader_ParticleAnimation], "uniform_ParticleAnimation_texture_fs_Part_col_rad");  // 2-cycle ping-pong color/radius of particles (FBO)
	uniform_ParticleAnimation_texture_fs_Part_Target_pos_col_rad
		= glGetUniformLocation(shader_programme[pg_shader_ParticleAnimation], "uniform_ParticleAnimation_texture_fs_Part_Target_pos_col_rad");  // 2-cycle ping-pong target position/color/radius of particles (FBO)
	uniform_ParticleAnimation_texture_fs_Trk0
		= glGetUniformLocation(shader_programme[pg_shader_ParticleAnimation], "uniform_ParticleAnimation_texture_fs_Trk0");  // ping-pong track 0 (FBO)
#if PG_NB_TRACKS >= 2
	uniform_ParticleAnimation_texture_fs_Trk1
		= glGetUniformLocation(shader_programme[pg_shader_ParticleAnimation], "uniform_ParticleAnimation_texture_fs_Trk1");  // ping-pong track 1 (FBO)
#endif
#if PG_NB_TRACKS >= 3
	uniform_ParticleAnimation_texture_fs_Trk2
		= glGetUniformLocation(shader_programme[pg_shader_ParticleAnimation], "uniform_ParticleAnimation_texture_fs_Trk2");  // ping-pong track 2 (FBO)
#endif
#if PG_NB_TRACKS >= 4
	uniform_ParticleAnimation_texture_fs_Trk3
		= glGetUniformLocation(shader_programme[pg_shader_ParticleAnimation], "uniform_ParticleAnimation_texture_fs_Trk3");  // ping-pong track 3 (FBO)
#endif

	///////////////////////////////////////////////////////////////////////////
	// textures

	// video frames
	uniform_ParticleAnimation_texture_fs_Camera_frame
		= glGetUniformLocation(shader_programme[pg_shader_ParticleAnimation], "uniform_ParticleAnimation_texture_fs_Camera_frame");  // camera frame
	uniform_ParticleAnimation_texture_fs_Movie_frame
		= glGetUniformLocation(shader_programme[pg_shader_ParticleAnimation], "uniform_ParticleAnimation_texture_fs_Movie_frame");  // movie frame

	uniform_ParticleAnimation_texture_fs_Noise
		= glGetUniformLocation(shader_programme[pg_shader_ParticleAnimation], "uniform_ParticleAnimation_texture_fs_Noise");  // 3D noise

	uniform_ParticleAnimation_texture_fs_Part_init_pos_speed
		= glGetUniformLocation(shader_programme[pg_shader_ParticleAnimation], "uniform_ParticleAnimation_texture_fs_Part_init_pos_speed");  // particle initialization pairs of textures position/speed
	uniform_ParticleAnimation_texture_fs_Part_init_col_rad
		= glGetUniformLocation(shader_programme[pg_shader_ParticleAnimation], "uniform_ParticleAnimation_texture_fs_Part_init_col_rad");  // particle initialization pairs of textures color/radius

	if ((uniform_ParticleAnimation_vp_model == -1)
		|| (uniform_ParticleAnimation_vp_view == -1)
		|| (uniform_ParticleAnimation_vp_proj == -1)
		|| (uniform_ParticleAnimation_vp_2fv_width_height == -1)

		|| (uniform_ParticleAnimation_fs_4fv_W_H_repopChannel_targetFrameNo == -1)
		|| (uniform_ParticleAnimation_fs_1fv_partDecay == -1)
		|| (uniform_ParticleAnimation_fs_4fv_flashTrkPartWghts == -1)
#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7
		|| (uniform_ParticleAnimation_fs_4fv_paths03_x == -1)
		|| (uniform_ParticleAnimation_fs_4fv_paths03_y == -1)
		|| (uniform_ParticleAnimation_fs_4fv_paths03_x_prev == -1)
		|| (uniform_ParticleAnimation_fs_4fv_paths03_y_prev == -1)
		|| (uniform_ParticleAnimation_fs_4fv_paths03_RadiusX == -1)
#endif
#if PG_NB_PATHS == 7
		|| (uniform_ParticleAnimation_fs_4fv_paths47_x == -1)
		|| (uniform_ParticleAnimation_fs_4fv_paths47_y == -1)
		|| (uniform_ParticleAnimation_fs_4fv_paths47_x_prev == -1)
		|| (uniform_ParticleAnimation_fs_4fv_paths47_y_prev == -1)
		|| (uniform_ParticleAnimation_fs_4fv_paths47_RadiusX == -1)
#endif
		|| (uniform_ParticleAnimation_fs_4fv_repop_Color_frameNo == -1)
		|| (uniform_ParticleAnimation_fs_4fv_flashCAPartWght_nbPart_clear_partRadius == -1)
		|| (uniform_ParticleAnimation_fs_4fv_repop_part_path_acc_damp_factor == -1)
		|| (uniform_ParticleAnimation_fs_4fv_Camera_W_H_movieWH == -1)
		|| (uniform_ParticleAnimation_texture_fs_CA == -1)
		|| (uniform_ParticleAnimation_texture_fs_Part_pos_speed == -1)
		|| (uniform_ParticleAnimation_texture_fs_Part_col_rad == -1)
		|| (uniform_ParticleAnimation_texture_fs_Part_Target_pos_col_rad == -1)
		|| (uniform_ParticleAnimation_texture_fs_Trk0 == -1)
#if PG_NB_TRACKS >= 2
		|| (uniform_ParticleAnimation_texture_fs_Trk1 == -1)
#endif
#if PG_NB_TRACKS >= 3
		|| (uniform_ParticleAnimation_texture_fs_Trk2 == -1)
#endif
#if PG_NB_TRACKS >= 4
		|| (uniform_ParticleAnimation_texture_fs_Trk3 == -1)
#endif
		|| (uniform_ParticleAnimation_texture_fs_Camera_frame == -1)
		|| (uniform_ParticleAnimation_texture_fs_Movie_frame == -1)
		|| (uniform_ParticleAnimation_texture_fs_Noise == -1)
		|| (uniform_ParticleAnimation_texture_fs_Part_init_pos_speed == -1)
		|| (uniform_ParticleAnimation_texture_fs_Part_init_col_rad == -1)
		) {
		fprintf(stderr, "Could not bind uniforms ParticleAnimation uniform_ParticleAnimation_vp_model : %d, uniform_ParticleAnimation_vp_view : %d, uniform_ParticleAnimation_vp_proj : %d, uniform_ParticleAnimation_vp_2fv_width_height : %d, uniform_ParticleAnimation_fs_4fv_W_H_repopChannel_targetFrameNo : %d, uniform_ParticleAnimation_fs_1fv_partDecay : %d, uniform_ParticleAnimation_fs_4fv_flashTrkPartWghts : %d, uniform_ParticleAnimation_texture_fs_Part_pos_speed : %d, uniform_ParticleAnimation_texture_fs_Camera_frame : %d, uniform_ParticleAnimation_fs_4fv_repop_Color_frameNo: %d, uniform_ParticleAnimation_fs_4fv_flashCAPartWght_nbPart_clear_partRadius: %d, uniform_ParticleAnimation_fs_4fv_Camera_W_H_movieWH: %d, uniform_ParticleAnimation_fs_4fv_repop_part_path_acc_damp_factor: %d\n", uniform_ParticleAnimation_vp_model, uniform_ParticleAnimation_vp_view, uniform_ParticleAnimation_vp_proj, uniform_ParticleAnimation_vp_2fv_width_height, uniform_ParticleAnimation_fs_4fv_W_H_repopChannel_targetFrameNo, uniform_ParticleAnimation_fs_1fv_partDecay, uniform_ParticleAnimation_fs_4fv_flashTrkPartWghts, uniform_ParticleAnimation_texture_fs_Part_pos_speed, uniform_ParticleAnimation_texture_fs_Camera_frame, uniform_ParticleAnimation_fs_4fv_repop_Color_frameNo, uniform_ParticleAnimation_fs_4fv_flashCAPartWght_nbPart_clear_partRadius, uniform_ParticleAnimation_fs_4fv_Camera_W_H_movieWH, uniform_ParticleAnimation_fs_4fv_repop_part_path_acc_damp_factor);
#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7
		fprintf(stderr, "Could not bind uniforms uniform_ParticleAnimation_fs_4fv_paths03_x : %d, uniform_ParticleAnimation_fs_4fv_paths03_y : %d, uniform_ParticleAnimation_fs_4fv_paths03_x_prev : %d, uniform_ParticleAnimation_fs_4fv_paths03_y_prev : %d, uniform_ParticleAnimation_fs_4fv_paths03_RadiusX : %d\n", uniform_ParticleAnimation_fs_4fv_paths03_x, uniform_ParticleAnimation_fs_4fv_paths03_y, uniform_ParticleAnimation_fs_4fv_paths03_x_prev, uniform_ParticleAnimation_fs_4fv_paths03_y_prev, uniform_ParticleAnimation_fs_4fv_paths03_RadiusX);
#endif
#if PG_NB_PATHS == 7
		fprintf(stderr, "Could not bind uniforms uniform_ParticleAnimation_fs_4fv_paths47_x : %d, uniform_ParticleAnimation_fs_4fv_paths47_y : %d, uniform_ParticleAnimation_fs_4fv_paths47_x_prev : %d, uniform_ParticleAnimation_fs_4fv_paths47_y_prev : %d, uniform_ParticleAnimation_fs_4fv_paths47_RadiusX : %d\n", uniform_ParticleAnimation_fs_4fv_paths47_x, uniform_ParticleAnimation_fs_4fv_paths47_y, uniform_ParticleAnimation_fs_4fv_paths47_x_prev, uniform_ParticleAnimation_fs_4fv_paths47_y_prev, uniform_ParticleAnimation_fs_4fv_paths47_RadiusX);
#endif
		fprintf(stderr, "Could not bind uniforms uniform_ParticleAnimation_texture_fs_Part_col_rad : %d, uniform_ParticleAnimation_texture_fs_Part_Target_pos_col_rad : %d\n", uniform_ParticleAnimation_texture_fs_Part_col_rad, uniform_ParticleAnimation_texture_fs_Part_Target_pos_col_rad);
		fprintf(stderr, "Could not bind uniforms uniform_ParticleAnimation_texture_fs_CA : %d\n", uniform_ParticleAnimation_texture_fs_CA);
		fprintf(stderr, "Could not bind uniforms uniform_ParticleAnimation_texture_fs_Trk0 : %d\n", uniform_ParticleAnimation_texture_fs_Trk0);
#if PG_NB_TRACKS >= 2
		fprintf(stderr, "Could not bind uniforms uniform_ParticleAnimation_texture_fs_Trk1 : %d\n", uniform_ParticleAnimation_texture_fs_Trk1);
#endif
#if PG_NB_TRACKS >= 3
		fprintf(stderr, "Could not bind uniforms uniform_ParticleAnimation_texture_fs_Trk2 : %d\n", uniform_ParticleAnimation_texture_fs_Trk2);
#endif
#if PG_NB_TRACKS >= 4
		fprintf(stderr, "Could not bind uniforms uniform_ParticleAnimation_texture_fs_Trk3 : %d\n", uniform_ParticleAnimation_texture_fs_Trk3);
#endif
		fprintf(stderr, "Could not bind uniforms uniform_ParticleAnimation_texture_fs_Camera_frame  : %d, uniform_ParticleAnimation_texture_fs_Movie_frame : %d\n", uniform_ParticleAnimation_texture_fs_Camera_frame, uniform_ParticleAnimation_texture_fs_Movie_frame);
		fprintf(stderr, "Could not bind uniforms uniform_ParticleAnimation_texture_fs_Noise : %d, uniform_ParticleAnimation_texture_fs_Part_init_pos_speed : %d, uniform_ParticleAnimation_texture_fs_Part_init_col_rad : %d\n", uniform_ParticleAnimation_texture_fs_Noise, uniform_ParticleAnimation_texture_fs_Part_init_pos_speed, uniform_ParticleAnimation_texture_fs_Part_init_col_rad);
	}
#endif

	////////////////////////////////////////
	// Update shader parameeter bindings
	uniform_Update_vp_model
		= glGetUniformLocation(shader_programme[pg_shader_Update], "vp_modelMatrix");
	uniform_Update_vp_view
		= glGetUniformLocation(shader_programme[pg_shader_Update], "vp_viewMatrix");
	uniform_Update_vp_proj
		= glGetUniformLocation(shader_programme[pg_shader_Update], "vp_projMatrix");

	uniform_Update_vp_2fv_width_height
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_vp_2fv_width_height");
	uniform_Update_fs_4fv_W_H_time_currentScene
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_W_H_time_currentScene");
	uniform_Update_fs_4fv_clearAllLayers_clearCA_pixelRadius_pulsedShift
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_clearAllLayers_clearCA_pixelRadius_pulsedShift");
#ifdef MALAUSSENA
	uniform_Update_fs_4fv_CAseed_type_size_loc
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_CAseed_type_size_loc");
#endif
#if defined (BLURRED_SPLAT_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) || PG_NB_TRACKS >= 2
	uniform_Update_fs_4fv_flashTrkBGWghts_flashPartBGWght
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_flashTrkBGWghts_flashPartBGWght");
#endif
	uniform_Update_fs_4fv_trkDecay
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_trkDecay");
	uniform_Update_fs_4fv_CAdecay_frameno_Cursor_flashPartCAWght
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_CAdecay_frameno_Cursor_flashPartCAWght");
#ifdef PG_NB_CA_TYPES
	uniform_Update_fs_4fv_flashTrkCAWghts
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_flashTrkCAWghts");
#endif
#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7
	uniform_Update_fs_4fv_paths03_x
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_paths03_x");
	uniform_Update_fs_4fv_paths03_y
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_paths03_y");
	uniform_Update_fs_4fv_paths03_x_prev
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_paths03_x_prev");
	uniform_Update_fs_4fv_paths03_y_prev
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_paths03_y_prev");
#ifdef PG_BEZIER_CURVES
	// pen Bezier curve tangents
	uniform_Update_fs_4fv_paths03_xL
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_paths03_xL");
	uniform_Update_fs_4fv_paths03_yL
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_paths03_yL");
	uniform_Update_fs_4fv_paths03_xR
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_paths03_xR");
	uniform_Update_fs_4fv_paths03_yR
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_paths03_yR");
	uniform_Update_fs_4iv_path0_next_in_hull
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4iv_path0_next_in_hull");
#endif
	uniform_Update_fs_4fv_paths03_r
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_paths03_r");
	uniform_Update_fs_4fv_paths03_g
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_paths03_g");
	uniform_Update_fs_4fv_paths03_b
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_paths03_b");
	uniform_Update_fs_4fv_paths03_a
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_paths03_a");
	uniform_Update_fs_4fv_paths03_BrushID
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_paths03_BrushID");
	uniform_Update_fs_4fv_paths03_RadiusX
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_paths03_RadiusX");
#ifndef PG_BEZIER_CURVES
	uniform_Update_fs_4fv_paths03_RadiusY
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_paths03_RadiusY");
#endif
#endif
#if PG_NB_PATHS == 7
	uniform_Update_fs_4fv_paths47_x
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_paths47_x");
	uniform_Update_fs_4fv_paths47_y
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_paths47_y");
	uniform_Update_fs_4fv_paths47_x_prev
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_paths47_x_prev");
	uniform_Update_fs_4fv_paths47_y_prev
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_paths47_y_prev");
#ifdef PG_BEZIER_CURVES
	// pen Bezier curve tangents
	uniform_Update_fs_4fv_paths47_xL
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_paths47_xL");
	uniform_Update_fs_4fv_paths47_yL
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_paths47_yL");
	uniform_Update_fs_4fv_paths47_xR
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_paths47_xR");
	uniform_Update_fs_4fv_paths47_yR
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_paths47_yR");
#endif
	uniform_Update_fs_4fv_paths47_r
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_paths47_r");
	uniform_Update_fs_4fv_paths47_g
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_paths47_g");
	uniform_Update_fs_4fv_paths47_b
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_paths47_b");
	uniform_Update_fs_4fv_paths47_a
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_paths47_a");
	uniform_Update_fs_4fv_paths47_BrushID
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_paths47_BrushID");
	uniform_Update_fs_4fv_paths47_RadiusX
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_paths47_RadiusX");
#ifndef PG_BEZIER_CURVES
	uniform_Update_fs_4fv_paths47_RadiusY
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_paths47_RadiusY");
#endif
#endif
#ifdef CRITON
	uniform_Update_fs_4fv_fftLevels03
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_fftLevels03");
	uniform_Update_fs_4fv_fftFrequencies03
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_fftFrequencies03");
	uniform_Update_fs_4fv_fftPhases03
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_fftPhases03");
	uniform_Update_fs_4fv_fftLevels47
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_fftLevels47");
	uniform_Update_fs_4fv_fftFrequencies47
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_fftFrequencies47");
	uniform_Update_fs_4fv_fftPhases47
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_fftPhases47");
#endif
	uniform_Update_fs_4fv_pulse
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_pulse");
	uniform_Update_fs_4fv_movieWH_flashCameraTrkWght_cpTrack
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_movieWH_flashCameraTrkWght_cpTrack");
	uniform_Update_fs_4fv_repop_Color_flashCABGWght
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_repop_Color_flashCABGWght");
	uniform_Update_fs_3fv_isClearLayer_flashPixel_flashCameraTrkThres
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_3fv_isClearLayer_flashPixel_flashCameraTrkThres");
#if defined (PG_WITH_PHOTO_DIAPORAMA) || defined (TVW)
	uniform_Update_fs_4fv_photo01_wh
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_photo01_wh");
#endif
#ifdef PG_WITH_PHOTO_DIAPORAMA
	uniform_Update_fs_2fv_photo01Wghts
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_2fv_photo01Wghts");
#endif
	uniform_Update_fs_4fv_Camera_offSetsXY_Camera_W_H
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_Camera_offSetsXY_Camera_W_H");
	uniform_Update_fs_4fv_xy_transl_tracks_0_1
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_xy_transl_tracks_0_1");
#if defined (PG_NB_CA_TYPES) || defined (PG_WITH_BLUR)
	uniform_Update_fs_4fv_CAType_SubType_blurRadius
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_CAType_SubType_blurRadius");
#endif
#if defined (GN) || defined (MALAUSSENA)
	uniform_Update_texture_fs_CATable
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_texture_fs_CATable");
#endif
#ifdef GN
	uniform_Update_fs_2fv_initCA_1stPlaneFrameNo
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_2fv_initCA_1stPlaneFrameNo");
	uniform_Update_texture_fs_Camera_BGIni
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_texture_fs_Camera_BGIni");  // initial background frame
#endif

	// BG track FBO
#ifdef PG_NB_CA_TYPES
	uniform_Update_texture_fs_CA
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_texture_fs_CA");         // 2-cycle ping-pong CA (FBO)
#endif
#ifdef PG_NB_PIXEL_MODES
	uniform_Update_texture_fs_Pixels
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_texture_fs_Pixels");  // 2-cycle ping-pong speed/position of pixels (FBO)
#endif
	uniform_Update_texture_fs_Trk0
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_texture_fs_Trk0");  // ping-pong track 0 (FBO)
#if PG_NB_TRACKS >= 2
	uniform_Update_texture_fs_Trk1
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_texture_fs_Trk1");  // ping-pong track 1 (FBO)
#endif
#if PG_NB_TRACKS >= 3
	uniform_Update_texture_fs_Trk2
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_texture_fs_Trk2");  // ping-pong track 2 (FBO)
#endif
#if PG_NB_TRACKS >= 4
	uniform_Update_texture_fs_Trk3
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_texture_fs_Trk3");  // ping-pong track 3 (FBO)
#endif
																									   // textures
	uniform_Update_texture_fs_Brushes
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_texture_fs_Brushes");  // pen patterns
																										// video frames
#ifdef PG_WITH_CAMERA_CAPTURE
	uniform_Update_texture_fs_Camera_frame
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_texture_fs_Camera_frame");  // camera frame
	uniform_Update_texture_fs_Camera_BG
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_texture_fs_Camera_BG");  // current background frame
#endif

	uniform_Update_texture_fs_Movie_frame
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_texture_fs_Movie_frame");  // movie frame

	uniform_Update_texture_fs_Noise
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_texture_fs_Noise");  // 3D noise

																										// photo frames
#ifdef PG_WITH_PHOTO_DIAPORAMA
	uniform_Update_texture_fs_Photo0
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_texture_fs_Photo0");  // photo[0] frame
	uniform_Update_texture_fs_Photo1
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_texture_fs_Photo1");  // photo[1] frame
#endif
#if defined (BLURRED_SPLAT_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES)
	uniform_Update_texture_fs_Part_render
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_texture_fs_Part_render");  // particle rendering capture
#endif
#if defined (TVW)
	uniform_Update_fs_4fv_weights03
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_weights03");
	uniform_Update_fs_2fv_weights45
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_2fv_weights45");
	uniform_Update_fs_3fv_alphaSwap02
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_3fv_alphaSwap02");
	uniform_Update_fs_3fv_alphaSwap35
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_3fv_alphaSwap35");
	uniform_Update_fs_4fv_image_noisesxy
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_image_noisesxy");
	uniform_Update_fs_4fv_mask_noisesxy
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_mask_noisesxy");

	uniform_Update_texture_fs_TVWPixels0
	= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_texture_fs_TVWPixels0");  // image buffer #0
	uniform_Update_texture_fs_TVWPixels1
	= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_texture_fs_TVWPixels1");  // image buffer #1
	uniform_Update_texture_fs_TVWPixels2
	= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_texture_fs_TVWPixels2");  // image buffer #2
	uniform_Update_texture_fs_TVWPixels3
	= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_texture_fs_TVWPixels3");  // image buffer #3
	uniform_Update_texture_fs_TVWPixels4
	= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_texture_fs_TVWPixels4");  // image buffer #4
	uniform_Update_texture_fs_TVWPixels5
	= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_texture_fs_TVWPixels5");  // image buffer #5
	uniform_Update_texture_fs_TVWMask02
	= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_texture_fs_TVWMask02");  // image mask #0
	uniform_Update_texture_fs_TVWMask35
	= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_texture_fs_TVWMask35");  // image mask #1
	uniform_Update_texture_fs_TVWPixelsSwap0
	= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_texture_fs_TVWPixelsSwap0");  // buffer swap image #0
	uniform_Update_texture_fs_TVWPixelsSwap1
	= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_texture_fs_TVWPixelsSwap1");  // buffer swap image #1
	uniform_Update_texture_fs_TVWPixelsSwap2
	= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_texture_fs_TVWPixelsSwap2");  // buffer swap image #2
	uniform_Update_texture_fs_TVWPixelsSwap3
	= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_texture_fs_TVWPixelsSwap3");  // buffer swap image #3
	uniform_Update_texture_fs_TVWPixelsSwap4
	= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_texture_fs_TVWPixelsSwap4");  // buffer swap image #4
	uniform_Update_texture_fs_TVWPixelsSwap5
	= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_texture_fs_TVWPixelsSwap5");  // buffer swap image #5
#endif

	if ((uniform_Update_vp_model == -1)
		|| (uniform_Update_vp_view == -1)
		|| (uniform_Update_vp_proj == -1)
		|| (uniform_Update_vp_2fv_width_height == -1)

		|| (uniform_Update_fs_4fv_W_H_time_currentScene == -1)
		|| (uniform_Update_fs_4fv_clearAllLayers_clearCA_pixelRadius_pulsedShift == -1)
#ifdef MALAUSSENA
		|| (uniform_Update_fs_4fv_CAseed_type_size_loc == -1)
#endif

#if defined (BLURRED_SPLAT_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) || PG_NB_TRACKS >= 2
		|| (uniform_Update_fs_4fv_flashTrkBGWghts_flashPartBGWght == -1)
#endif
		|| (uniform_Update_fs_4fv_trkDecay == -1)
		|| (uniform_Update_fs_4fv_CAdecay_frameno_Cursor_flashPartCAWght == -1)
#ifdef PG_NB_CA_TYPES
		|| (uniform_Update_fs_4fv_flashTrkCAWghts == -1)
#endif
#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7
		|| (uniform_Update_fs_4fv_paths03_x == -1)
		|| (uniform_Update_fs_4fv_paths03_y == -1)
		|| (uniform_Update_fs_4fv_paths03_x_prev == -1)
		|| (uniform_Update_fs_4fv_paths03_y_prev == -1)
#ifdef PG_BEZIER_CURVES
// pen Bezier curve tangents
		|| (uniform_Update_fs_4fv_paths03_xL == -1)
		|| (uniform_Update_fs_4fv_paths03_yL == -1)
		|| (uniform_Update_fs_4fv_paths03_xR == -1)
		|| (uniform_Update_fs_4fv_paths03_yR == -1)
		|| (uniform_Update_fs_4iv_path0_next_in_hull == -1)
#endif
		|| (uniform_Update_fs_4fv_paths03_r == -1)
		|| (uniform_Update_fs_4fv_paths03_g == -1)
		|| (uniform_Update_fs_4fv_paths03_b == -1)
		|| (uniform_Update_fs_4fv_paths03_a == -1)
		|| (uniform_Update_fs_4fv_paths03_BrushID == -1)
		|| (uniform_Update_fs_4fv_paths03_RadiusX == -1)
#ifndef PG_BEZIER_CURVES
		|| (uniform_Update_fs_4fv_paths03_RadiusY == -1)
#endif
#endif
#if PG_NB_PATHS == 7
		|| (uniform_Update_fs_4fv_paths47_x == -1)
		|| (uniform_Update_fs_4fv_paths47_y == -1)
		|| (uniform_Update_fs_4fv_paths47_x_prev == -1)
		|| (uniform_Update_fs_4fv_paths47_y_prev == -1)
#ifdef PG_BEZIER_CURVES
		// pen Bezier curve tangents
		|| (uniform_Update_fs_4fv_paths47_xL == -1)
		|| (uniform_Update_fs_4fv_paths47_yL == -1)
		|| (uniform_Update_fs_4fv_paths47_xR == -1)
		|| (uniform_Update_fs_4fv_paths47_yR == -1)
#endif
		|| (uniform_Update_fs_4fv_paths47_r == -1)
		|| (uniform_Update_fs_4fv_paths47_g == -1)
		|| (uniform_Update_fs_4fv_paths47_b == -1)
		|| (uniform_Update_fs_4fv_paths47_a == -1)
		|| (uniform_Update_fs_4fv_paths47_BrushID == -1)
		|| (uniform_Update_fs_4fv_paths47_RadiusX == -1)
#ifndef PG_BEZIER_CURVES
		|| (uniform_Update_fs_4fv_paths47_RadiusY == -1)
#endif
#endif
#ifdef CRITON
		|| (uniform_Update_fs_4fv_fftLevels03 == -1)
		|| (uniform_Update_fs_4fv_fftFrequencies03 == -1)
		|| (uniform_Update_fs_4fv_fftPhases03 == -1)
		|| (uniform_Update_fs_4fv_fftLevels47 == -1)
		|| (uniform_Update_fs_4fv_fftFrequencies47 == -1)
		|| (uniform_Update_fs_4fv_fftPhases47 == -1)
#endif
		|| (uniform_Update_fs_4fv_pulse == -1)
		|| (uniform_Update_fs_4fv_movieWH_flashCameraTrkWght_cpTrack == -1)
		|| (uniform_Update_fs_4fv_repop_Color_flashCABGWght == -1)
		|| (uniform_Update_fs_3fv_isClearLayer_flashPixel_flashCameraTrkThres == -1)
#if defined (PG_WITH_PHOTO_DIAPORAMA) || defined (TVW)
		|| (uniform_Update_fs_4fv_photo01_wh == -1)
#endif
#ifdef PG_WITH_PHOTO_DIAPORAMA
		|| (uniform_Update_fs_2fv_photo01Wghts == -1)
#endif
		|| (uniform_Update_fs_4fv_Camera_offSetsXY_Camera_W_H == -1)
#ifdef PG_NB_CA_TYPES
		|| (uniform_Update_texture_fs_CA == -1)
#endif
		|| (uniform_Update_fs_4fv_xy_transl_tracks_0_1 == -1)
#if defined (PG_NB_CA_TYPES) || defined (PG_WITH_BLUR)
		|| (uniform_Update_fs_4fv_CAType_SubType_blurRadius == -1)
#endif
#if defined (GN) || defined (MALAUSSENA)
		|| (uniform_Update_texture_fs_CATable == -1)
#endif
#ifdef GN
		|| (uniform_Update_fs_2fv_initCA_1stPlaneFrameNo == -1)
		|| (uniform_Update_texture_fs_Camera_BGIni == -1)
#endif
#ifdef PG_NB_PIXEL_MODES
		|| (uniform_Update_texture_fs_Pixels == -1)
#endif
		|| (uniform_Update_texture_fs_Trk0 == -1)
#if PG_NB_TRACKS >= 2
		|| (uniform_Update_texture_fs_Trk1 == -1)
#endif
#if PG_NB_TRACKS >= 3
		|| (uniform_Update_texture_fs_Trk2 == -1)
#endif
#if PG_NB_TRACKS >= 4
		|| (uniform_Update_texture_fs_Trk3 == -1)
#endif
		|| (uniform_Update_texture_fs_Brushes == -1)
#ifdef PG_WITH_CAMERA_CAPTURE
		|| (uniform_Update_texture_fs_Camera_frame == -1)
		|| (uniform_Update_texture_fs_Camera_BG == -1)
#endif
		|| (uniform_Update_texture_fs_Movie_frame == -1)
		|| (uniform_Update_texture_fs_Noise == -1)
#ifdef PG_WITH_PHOTO_DIAPORAMA
		|| (uniform_Update_texture_fs_Photo0 == -1)
		|| (uniform_Update_texture_fs_Photo1 == -1)
#endif
#if defined (BLURRED_SPLAT_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES)
		|| (uniform_Update_texture_fs_Part_render == -1)
#endif
		) {
		fprintf(stderr, "Could not bind uniforms Update uniform_Update_vp_model : %d, uniform_Update_vp_view : %d, uniform_Update_vp_proj : %d, uniform_Update_vp_2fv_width_height : %d, uniform_Update_fs_4fv_W_H_time_currentScene : %d, uniform_Update_fs_4fv_clearAllLayers_clearCA_pixelRadius_pulsedShift : %d, uniform_Update_fs_4fv_trkDecay : %d, uniform_Update_fs_4fv_CAdecay_frameno_Cursor_flashPartCAWght : %d, uniform_Update_fs_4fv_pulse: %d, uniform_Update_fs_4fv_movieWH_flashCameraTrkWght_cpTrack: %d, uniform_Update_fs_4fv_repop_Color_flashCABGWght: %d, uniform_Update_fs_3fv_isClearLayer_flashPixel_flashCameraTrkThres: %d, uniform_Update_fs_4fv_Camera_offSetsXY_Camera_W_H: %d, uniform_Update_fs_4fv_xy_transl_tracks_0_1 %d\n", uniform_Update_vp_model, uniform_Update_vp_view, uniform_Update_vp_proj, uniform_Update_vp_2fv_width_height, uniform_Update_fs_4fv_W_H_time_currentScene, uniform_Update_fs_4fv_clearAllLayers_clearCA_pixelRadius_pulsedShift, uniform_Update_fs_4fv_trkDecay, uniform_Update_fs_4fv_CAdecay_frameno_Cursor_flashPartCAWght, uniform_Update_fs_4fv_pulse, uniform_Update_fs_4fv_movieWH_flashCameraTrkWght_cpTrack, uniform_Update_fs_4fv_repop_Color_flashCABGWght, uniform_Update_fs_3fv_isClearLayer_flashPixel_flashCameraTrkThres, uniform_Update_fs_4fv_Camera_offSetsXY_Camera_W_H, uniform_Update_fs_4fv_xy_transl_tracks_0_1);
#if defined (PG_WITH_PHOTO_DIAPORAMA) || defined (TVW)
		fprintf(stderr, "Could not bind uniforms uniform_Update_fs_4fv_photo01_wh : %d\n", uniform_Update_fs_4fv_photo01_wh);
#endif
#if defined (PG_NB_CA_TYPES) || defined (PG_WITH_BLUR)
		fprintf(stderr, "Could not bind uniforms uniform_Update_fs_4fv_CAType_SubType_blurRadius : %d\n", uniform_Update_fs_4fv_CAType_SubType_blurRadius);
#endif
#ifdef PG_WITH_PHOTO_DIAPORAMA
		fprintf(stderr, "Could not bind uniforms uniform_Update_fs_2fv_photo01Wghts : %d\n", uniform_Update_fs_2fv_photo01Wghts);
#endif
#if defined (BLURRED_SPLAT_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) || PG_NB_TRACKS >= 2
		fprintf(stderr, "Could not bind uniforms Update uniform_Update_fs_4fv_flashTrkBGWghts_flashPartBGWght : %d\n", uniform_Update_fs_4fv_flashTrkBGWghts_flashPartBGWght);
#endif
#ifdef MALAUSSENA
		fprintf(stderr, "Could not bind uniforms uniform_Update_fs_4fv_CAseed_type_size_loc : %d\n", uniform_Update_fs_4fv_CAseed_type_size_loc);
#endif
#ifdef PG_NB_PIXEL_MODES
		fprintf(stderr, "Could not bind uniforms uniform_Update_texture_fs_Pixels : %d\n", uniform_Update_texture_fs_Pixels);
#endif
#ifdef PG_NB_CA_TYPES
		fprintf(stderr, "Could not bind uniforms uniform_Update_texture_fs_CA : %d, uniform_Update_fs_4fv_flashTrkCAWghts :¨%d\n", uniform_Update_texture_fs_CA, uniform_Update_fs_4fv_flashTrkCAWghts);
#endif
#ifdef PG_WITH_CAMERA_CAPTURE
		fprintf(stderr, "Could not bind uniforms Update uniform_Update_texture_fs_Camera_frame : %d, uniform_Update_texture_fs_Camera_BG : %d\n", uniform_Update_texture_fs_Camera_frame, uniform_Update_texture_fs_Camera_BG);
#endif
#if defined (GN) || defined (MALAUSSENA)
		fprintf(stderr, "Could not bind uniforms uniform_Update_texture_fs_CATable : %d\n",  uniform_Update_texture_fs_CATable );
#endif
#ifdef GN
		fprintf(stderr, "Could not bind uniforms uniform_Update_fs_2fv_initCA_1stPlaneFrameNo : %d, uniform_Update_texture_fs_Camera_BGIni : %d\n", uniform_Update_fs_2fv_initCA_1stPlaneFrameNo, uniform_Update_texture_fs_Camera_BGIni);
#endif
#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7
		fprintf(stderr, "Could not bind uniforms uniform_Update_fs_4fv_paths03_x : %d, uniform_Update_fs_4fv_paths03_y : %d, uniform_Update_fs_4fv_paths03_x_prev : %d, uniform_Update_fs_4fv_paths03_y_prev : %d, uniform_Update_fs_4fv_paths03_r : %d, uniform_Update_fs_4fv_paths03_g : %d, uniform_Update_fs_4fv_paths03_b : %d, uniform_Update_fs_4fv_paths03_a : %d, uniform_Update_fs_4fv_paths03_BrushID : %d, uniform_Update_fs_4fv_paths03_RadiusX : %d\n", uniform_Update_fs_4fv_paths03_x, uniform_Update_fs_4fv_paths03_y, uniform_Update_fs_4fv_paths03_x_prev, uniform_Update_fs_4fv_paths03_y_prev, uniform_Update_fs_4fv_paths03_r, uniform_Update_fs_4fv_paths03_g, uniform_Update_fs_4fv_paths03_b, uniform_Update_fs_4fv_paths03_a, uniform_Update_fs_4fv_paths03_BrushID, uniform_Update_fs_4fv_paths03_RadiusX);
#ifndef PG_BEZIER_CURVES
		fprintf(stderr, "Could not bind uniforms uniform_Update_fs_4fv_paths03_RadiusY : %d\n", uniform_Update_fs_4fv_paths03_RadiusY);
#endif
#ifdef PG_BEZIER_CURVES
		// pen Bezier curve tangents
		fprintf(stderr, "Could not bind uniforms uniform_Update_fs_4fv_paths03_xL : %d, uniform_Update_fs_4fv_paths03_yL : %d, uniform_Update_fs_4fv_paths03_xR : %d, uniform_Update_fs_4fv_paths03_yR : %d, uniform_Update_fs_4iv_path0_next_in_hull : %d\n", uniform_Update_fs_4fv_paths03_xL, uniform_Update_fs_4fv_paths03_yL, uniform_Update_fs_4fv_paths03_xR, uniform_Update_fs_4fv_paths03_yR, uniform_Update_fs_4iv_path0_next_in_hull );
#endif
#endif
#if PG_NB_PATHS == 7
		fprintf(stderr, "Could not bind uniforms uniform_Update_fs_4fv_paths47_x : %d, uniform_Update_fs_4fv_paths47_y : %d, uniform_Update_fs_4fv_paths47_x_prev : %d, uniform_Update_fs_4fv_paths47_y_prev : %d, uniform_Update_fs_4fv_paths47_r : %d, uniform_Update_fs_4fv_paths47_g : %d, uniform_Update_fs_4fv_paths47_b : %d, uniform_Update_fs_4fv_paths47_a : %d, uniform_Update_fs_4fv_paths47_BrushID : %d, uniform_Update_fs_4fv_paths47_RadiusX : %d\n", uniform_Update_fs_4fv_paths47_x, uniform_Update_fs_4fv_paths47_y, uniform_Update_fs_4fv_paths47_x_prev, uniform_Update_fs_4fv_paths47_y_prev, uniform_Update_fs_4fv_paths47_r, uniform_Update_fs_4fv_paths47_g, uniform_Update_fs_4fv_paths47_b, uniform_Update_fs_4fv_paths47_a, uniform_Update_fs_4fv_paths47_BrushID, uniform_Update_fs_4fv_paths47_RadiusX);
#ifndef PG_BEZIER_CURVES
		fprintf(stderr, "Could not bind uniforms uniform_Update_fs_4fv_paths47_RadiusY : %d\n", uniform_Update_fs_4fv_paths47_RadiusY);
#endif
#ifdef PG_BEZIER_CURVES
		// pen Bezier curve tangents
		fprintf(stderr, "Could not bind uniforms uniform_Update_fs_4fv_paths47_xL : %d, uniform_Update_fs_4fv_paths47_yL : %d, uniform_Update_fs_4fv_paths47_xR : %d, uniform_Update_fs_4fv_paths47_yR : %d\n", uniform_Update_fs_4fv_paths47_xL, uniform_Update_fs_4fv_paths47_yL, uniform_Update_fs_4fv_paths47_xR, uniform_Update_fs_4fv_paths47_yR);
#endif
#endif
#ifdef CRITON
		fprintf(stderr, "Could not bind uniforms uniform_Update_fs_4fv_fftLevels03 : %d, uniform_Update_fs_4fv_fftFrequencies03 : %d, uniform_Update_fs_4fv_fftPhases03 : %d, uniform_Update_fs_4fv_fftLevels47 : %d, uniform_Update_fs_4fv_fftFrequencies47 : %d, uniform_Update_fs_4fv_fftPhases47 : %d\n", uniform_Update_fs_4fv_fftLevels03, uniform_Update_fs_4fv_fftFrequencies03, uniform_Update_fs_4fv_fftPhases03, uniform_Update_fs_4fv_fftLevels47, uniform_Update_fs_4fv_fftFrequencies47, uniform_Update_fs_4fv_fftPhases47);
#endif
		fprintf(stderr, "Could not bind uniforms uniform_Update_texture_fs_Trk0 : %d\n", uniform_Update_texture_fs_Trk0);
#if PG_NB_TRACKS >= 2
		fprintf(stderr, "Could not bind uniforms uniform_Update_texture_fs_Trk1 : %d\n", uniform_Update_texture_fs_Trk1);
#endif
#if PG_NB_TRACKS >= 3
		fprintf(stderr, "Could not bind uniforms uniform_Update_texture_fs_Trk2 : %d\n", uniform_Update_texture_fs_Trk2);
#endif
#if PG_NB_TRACKS >= 4
		fprintf(stderr, "Could not bind uniforms uniform_Update_texture_fs_Trk3 : %d\n", uniform_Update_texture_fs_Trk3);
#endif
		fprintf(stderr, "Could not bind uniforms uniform_Update_texture_fs_Movie_frame : %d\n", uniform_Update_texture_fs_Movie_frame);
#ifdef PG_WITH_PHOTO_DIAPORAMA
		fprintf(stderr, "Could not bind uniforms uniform_Update_texture_fs_Photo0 : %d, uniform_Update_texture_fs_Photo1 : %d\n", uniform_Update_texture_fs_Photo0, uniform_Update_texture_fs_Photo1);
#endif
		fprintf(stderr, "Could not bind uniforms uniform_Update_texture_fs_Brushes : %d, uniform_Update_texture_fs_Noise : %d\n", uniform_Update_texture_fs_Brushes, uniform_Update_texture_fs_Noise);
#if defined (BLURRED_SPLAT_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES)
		fprintf(stderr, "Could not bind uniform_Update_texture_fs_Part_render : %d\n", uniform_Update_texture_fs_Part_render);
#endif
	}

	////////////////////////////////////////
	// Particle Rendering shader parameeter bindings
#if defined (BLURRED_SPLAT_PARTICLES) || defined (LINE_SPLAT_PARTICLES) 
	uniform_ParticleSplat_vp_model
		= glGetUniformLocation(shader_programme[pg_shader_ParticleRender], "vp_modelMatrix");
	uniform_ParticleSplat_vp_view
		= glGetUniformLocation(shader_programme[pg_shader_ParticleRender], "vp_viewMatrix");
	uniform_ParticleSplat_vp_proj
		= glGetUniformLocation(shader_programme[pg_shader_ParticleRender], "vp_projMatrix");

	uniform_ParticleSplat_gs_3fv_partRadius_partType_highPitchPulse
		= glGetUniformLocation(shader_programme[pg_shader_ParticleRender], "uniform_ParticleSplat_gs_3fv_partRadius_partType_highPitchPulse");
	uniform_ParticleSplat_vp_3fv_trackReplay_xy_height
		= glGetUniformLocation(shader_programme[pg_shader_ParticleRender], "uniform_ParticleSplat_vp_3fv_trackReplay_xy_height");

	// particle position/speed FBO
	uniform_ParticleSplat_texture_vp_Part_pos_speed
		= glGetUniformLocation(shader_programme[pg_shader_ParticleRender], "uniform_ParticleSplat_texture_vp_Part_pos_speed");
	// particle color/radius FBO
	uniform_ParticleSplat_texture_vp_Part_col_rad
		= glGetUniformLocation(shader_programme[pg_shader_ParticleRender], "uniform_ParticleSplat_texture_vp_Part_col_rad"); 
#endif
#if defined (CURVE_PARTICLES) 
	uniform_ParticleCurve_vp_model
		= glGetUniformLocation(shader_programme[pg_shader_ParticleRender], "vp_modelMatrix");
	uniform_ParticleCurve_vp_view
		= glGetUniformLocation(shader_programme[pg_shader_ParticleRender], "vp_viewMatrix");
	uniform_ParticleCurve_vp_proj
		= glGetUniformLocation(shader_programme[pg_shader_ParticleRender], "vp_projMatrix");

	uniform_ParticleCurve_gs_3fv_partRadius_partType_highPitchPulse
		= glGetUniformLocation(shader_programme[pg_shader_ParticleRender], "uniform_ParticleCurve_gs_3fv_partRadius_partType_highPitchPulse");
	uniform_ParticleCurve_vp_3fv_trackReplay_xy_height
		= glGetUniformLocation(shader_programme[pg_shader_ParticleRender], "uniform_ParticleCurve_vp_3fv_trackReplay_xy_height");

	// particle position/speed FBO
	uniform_ParticleCurve_texture_vp_Part_pos_speed
		= glGetUniformLocation(shader_programme[pg_shader_ParticleRender], "uniform_ParticleCurve_texture_vp_Part_pos_speed");
	// particle color/radius FBO
	uniform_ParticleCurve_texture_vp_Part_col_rad
		= glGetUniformLocation(shader_programme[pg_shader_ParticleRender], "uniform_ParticleCurve_texture_vp_Part_col_rad");
#endif

#ifdef CURVE_PARTICLES
	// color texture
	uniform_ParticleCurve_Comet_texture_fs_decal
		= glGetUniformLocation(shader_programme[pg_shader_ParticleRender], "fs_comet");
#endif
#ifdef BLURRED_SPLAT_PARTICLES_TEXTURED
	// color texture
	uniform_ParticleSplat_BlurredDisk_texture_fs_decal
		= glGetUniformLocation(shader_programme[pg_shader_ParticleRender], "uniform_ParticleSplat_BlurredDisk_texture_fs_decal");
#endif

#if defined (BLURRED_SPLAT_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES)
	if (
#if defined (BLURRED_SPLAT_PARTICLES) || defined (LINE_SPLAT_PARTICLES) 
		(uniform_ParticleSplat_vp_model == -1)
		|| (uniform_ParticleSplat_vp_view == -1)
		|| (uniform_ParticleSplat_vp_proj == -1)
		|| (uniform_ParticleSplat_gs_3fv_partRadius_partType_highPitchPulse == -1)
		|| (uniform_ParticleSplat_vp_3fv_trackReplay_xy_height == -1)
		|| (uniform_ParticleSplat_texture_vp_Part_pos_speed == -1)
		|| (uniform_ParticleSplat_texture_vp_Part_col_rad == -1)
#endif
#if defined (CURVE_PARTICLES) 
		(uniform_ParticleCurve_vp_model == -1)
		|| (uniform_ParticleCurve_vp_view == -1)
		|| (uniform_ParticleCurve_vp_proj == -1)
		|| (uniform_ParticleCurve_gs_3fv_partRadius_partType_highPitchPulse == -1)
		|| (uniform_ParticleCurve_vp_3fv_trackReplay_xy_height == -1)
		|| (uniform_ParticleCurve_texture_vp_Part_pos_speed == -1)
		|| (uniform_ParticleCurve_texture_vp_Part_col_rad == -1)
#endif
#ifdef CURVE_PARTICLES
		|| (uniform_ParticleCurve_Comet_texture_fs_decal == -1)
#endif
#ifdef BLURRED_SPLAT_PARTICLES_TEXTURED
		|| (uniform_ParticleSplat_BlurredDisk_texture_fs_decal == -1)
#endif
		) {
#ifdef LINE_SPLAT_PARTICLES
		fprintf(stderr, "Could not bind uniforms Particle uniform_ParticleSplat_vp_model : %d, uniform_ParticleSplat_vp_view : %d, uniform_ParticleSplat_vp_proj : %d, uniform_ParticleSplat_gs_3fv_partRadius_partType_highPitchPulse : %d, uniform_ParticleSplat_vp_3fv_trackReplay_xy_height : %d, uniform_ParticleSplat_texture_vp_Part_pos_speed : %d, uniform_ParticleSplat_texture_vp_Part_col_rad : %d\n", uniform_ParticleSplat_vp_model, uniform_ParticleSplat_vp_view, uniform_ParticleSplat_vp_proj, uniform_ParticleSplat_gs_3fv_partRadius_partType_highPitchPulse, uniform_ParticleSplat_vp_3fv_trackReplay_xy_height, uniform_ParticleSplat_texture_vp_Part_pos_speed, uniform_ParticleSplat_texture_vp_Part_col_rad); 
#endif
#ifdef CURVE_PARTICLES
		fprintf(stderr, "Could not bind uniforms Particle uniform_ParticleCurve_vp_model : %d, uniform_ParticleCurve_vp_view : %d, uniform_ParticleCurve_vp_proj : %d, uniform_ParticleCurve_gs_3fv_partRadius_partType_highPitchPulse : %d, uniform_ParticleCurve_vp_3fv_trackReplay_xy_height : %d, uniform_ParticleCurve_texture_vp_Part_pos_speed : %d, uniform_ParticleCurve_texture_vp_Part_col_rad : %d, uniform_ParticleCurve_Comet_texture_fs_decal : %d\n", uniform_ParticleCurve_vp_model, uniform_ParticleCurve_vp_view, uniform_ParticleCurve_vp_proj, uniform_ParticleCurve_gs_3fv_partRadius_partType_highPitchPulse, uniform_ParticleCurve_vp_3fv_trackReplay_xy_height, uniform_ParticleCurve_texture_vp_Part_pos_speed, uniform_ParticleCurve_texture_vp_Part_col_rad, uniform_ParticleCurve_Comet_texture_fs_decal);
#endif
#ifdef BLURRED_SPLAT_PARTICLES_TEXTURED
		fprintf(stderr, "Could not bind uniforms Particle uniform_ParticleSplat_vp_model : %d, uniform_ParticleSplat_vp_view : %d, uniform_ParticleSplat_vp_proj : %d, uniform_ParticleSplat_gs_3fv_partRadius_partType_highPitchPulse : %d, uniform_ParticleSplat_vp_3fv_trackReplay_xy_height : %d, uniform_ParticleSplat_texture_vp_Part_pos_speed : %d, uniform_ParticleSplat_texture_vp_Part_col_rad : %d, uniform_ParticleSplat_BlurredDisk_texture_fs_decal : %d\n", uniform_ParticleSplat_vp_model, uniform_ParticleSplat_vp_view, uniform_ParticleSplat_vp_proj, uniform_ParticleSplat_gs_3fv_partRadius_partType_highPitchPulse, uniform_ParticleSplat_vp_3fv_trackReplay_xy_height, uniform_ParticleSplat_texture_vp_Part_pos_speed, uniform_ParticleSplat_texture_vp_Part_col_rad, uniform_ParticleSplat_BlurredDisk_texture_fs_decal);
#endif
	}
#endif

#if defined (TVW)
	if (
		(uniform_Update_fs_4fv_weights03 == -1)
		|| (uniform_Update_fs_2fv_weights45 == -1)
		|| (uniform_Update_fs_3fv_alphaSwap02 == -1)
		|| (uniform_Update_fs_3fv_alphaSwap35 == -1)
		|| (uniform_Update_fs_4fv_image_noisesxy == -1)
		|| (uniform_Update_fs_4fv_mask_noisesxy == -1)
		|| (uniform_Update_texture_fs_TVWPixels0 == -1)
		|| (uniform_Update_texture_fs_TVWPixels1 == -1)
		|| (uniform_Update_texture_fs_TVWPixels2 == -1)
		|| (uniform_Update_texture_fs_TVWPixels3 == -1)
		|| (uniform_Update_texture_fs_TVWPixels4 == -1)
		|| (uniform_Update_texture_fs_TVWPixels5 == -1)
		|| (uniform_Update_texture_fs_TVWMask02 == -1)
		|| (uniform_Update_texture_fs_TVWMask35 == -1)
		|| (uniform_Update_texture_fs_TVWPixelsSwap0 == -1)
		|| (uniform_Update_texture_fs_TVWPixelsSwap1 == -1)
		|| (uniform_Update_texture_fs_TVWPixelsSwap2 == -1)
		|| (uniform_Update_texture_fs_TVWPixelsSwap3 == -1)
		|| (uniform_Update_texture_fs_TVWPixelsSwap4 == -1)
		|| (uniform_Update_texture_fs_TVWPixelsSwap5 == -1)
		) {
		fprintf(stderr, "Could not bind uniforms Drawing uniform_Update_fs_4fv_weights03 : %d, uniform_Update_fs_2fv_weights45 : %d, uniform_Update_fs_3fv_alphaSwap02 : %d, uniform_Update_fs_3fv_alphaSwap35 : %d, uniform_Update_fs_4fv_image_noisesxy : %d, uniform_Update_fs_4fv_mask_noisesxy : %d\n", uniform_Update_fs_4fv_weights03, uniform_Update_fs_2fv_weights45, uniform_Update_fs_3fv_alphaSwap02, uniform_Update_fs_3fv_alphaSwap35, uniform_Update_fs_4fv_image_noisesxy, uniform_Update_fs_4fv_mask_noisesxy);
		fprintf(stderr, "Could not bind uniforms uniform_Update_fs_3fv_mouseTracuniform_Update_texture_fs_TVWPixels0 : %d, uniform_Update_texture_fs_TVWPixels1 : %d, uniform_Update_texture_fs_TVWPixels2 : %d, uniform_Update_texture_fs_TVWPixels3 : %d, uniform_Update_texture_fs_TVWPixels4 : %d, \n uniform_Update_texture_fs_TVWPixels5 : %d, uniform_Update_texture_fs_TVWMask02 : %d, uniform_Update_texture_fs_TVWMask35 : %d, uniform_Update_texture_fs_TVWPixelsSwap0 : %d, uniform_Update_texture_fs_TVWPixelsSwap1 : %d, uniform_Update_texture_fs_TVWPixelsSwap2 : %d, uniform_Update_texture_fs_TVWPixelsSwap3 : %d, \n uniform_Update_texture_fs_TVWPixelsSwap4 : %d, uniform_Update_texture_fs_TVWPixelsSwap5 : %d\n", uniform_Update_texture_fs_TVWPixels0, uniform_Update_texture_fs_TVWPixels1, uniform_Update_texture_fs_TVWPixels2, uniform_Update_texture_fs_TVWPixels3, uniform_Update_texture_fs_TVWPixels4, uniform_Update_texture_fs_TVWPixels5, uniform_Update_texture_fs_TVWMask02, uniform_Update_texture_fs_TVWMask35, uniform_Update_texture_fs_TVWPixelsSwap0, uniform_Update_texture_fs_TVWPixelsSwap1, uniform_Update_texture_fs_TVWPixelsSwap2, uniform_Update_texture_fs_TVWPixelsSwap3, uniform_Update_texture_fs_TVWPixelsSwap4, uniform_Update_texture_fs_TVWPixelsSwap5);
	}
#endif

	////////////////////////////////////////
	// shader parameeter bindings
	uniform_Mixing_vp_model
		= glGetUniformLocation(shader_programme[pg_shader_Mixing], "vp_modelMatrix");
	uniform_Mixing_vp_view
		= glGetUniformLocation(shader_programme[pg_shader_Mixing], "vp_viewMatrix");
	uniform_Mixing_vp_proj
		= glGetUniformLocation(shader_programme[pg_shader_Mixing], "vp_projMatrix");

	uniform_Mixing_fs_3fv_pulsedShift_height_flashCameraTrkWght
		= glGetUniformLocation(shader_programme[pg_shader_Mixing], "uniform_Mixing_fs_3fv_pulsedShift_height_flashCameraTrkWght");
	uniform_Mixing_fs_3fv_screenMsgTransp_Text1_2_Alpha
		= glGetUniformLocation(shader_programme[pg_shader_Mixing], "uniform_Mixing_fs_3fv_screenMsgTransp_Text1_2_Alpha");

#ifdef PG_NB_CA_TYPES
	uniform_Mixing_texture_fs_CA
		= glGetUniformLocation(shader_programme[pg_shader_Mixing], "uniform_Mixing_texture_fs_CA"); // ping-pong CA (FBO)
#endif
#if defined (BLURRED_SPLAT_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES)
	uniform_Mixing_texture_fs_Part_render
		= glGetUniformLocation(shader_programme[pg_shader_Mixing], "uniform_Mixing_texture_fs_Part_render");  // Particles (FBO)
#endif
	uniform_Mixing_texture_fs_Trk0
		= glGetUniformLocation(shader_programme[pg_shader_Mixing], "uniform_Mixing_texture_fs_Trk0");  // ping-pong track 0 (FBO)
#if PG_NB_TRACKS >= 2
	uniform_Mixing_texture_fs_Trk1
		= glGetUniformLocation(shader_programme[pg_shader_Mixing], "uniform_Mixing_texture_fs_Trk1");  // ping-pong track 1 (FBO)
#endif
#if PG_NB_TRACKS >= 3
	uniform_Mixing_texture_fs_Trk2
		= glGetUniformLocation(shader_programme[pg_shader_Mixing], "uniform_Mixing_texture_fs_Trk2");  // ping-pong track 2 (FBO)
#endif
#if PG_NB_TRACKS >= 4
	uniform_Mixing_texture_fs_Trk3
		= glGetUniformLocation(shader_programme[pg_shader_Mixing], "uniform_Mixing_texture_fs_Trk3");  // ping-pong track 3 (FBO)
#endif
#if defined (TVW)
	uniform_Mixing_texture_fs_Display_Font
		= glGetUniformLocation(shader_programme[pg_shader_Mixing], "uniform_Mixing_texture_fs_Display_Font");  // message Font
	uniform_Mixing_texture_fs_Display_Message1
		= glGetUniformLocation(shader_programme[pg_shader_Mixing], "uniform_Mixing_texture_fs_Display_Message1");  // tweets at the bottom of the screen
	uniform_Mixing_texture_fs_Display_Message2
		= glGetUniformLocation(shader_programme[pg_shader_Mixing], "uniform_Mixing_texture_fs_Display_Message2");  // tweets at the bottom of the screen
#endif

	uniform_Mixing_texture_fs_Render_prec
		= glGetUniformLocation(shader_programme[pg_shader_Mixing], "uniform_Mixing_texture_fs_Render_prec");  // preceding snapshot for echo
	uniform_Mixing_texture_fs_Screen_Font
		= glGetUniformLocation(shader_programme[pg_shader_Mixing], "uniform_Mixing_texture_fs_Screen_Font");  // message Font
	uniform_Mixing_texture_fs_Screen_Message
		= glGetUniformLocation(shader_programme[pg_shader_Mixing], "uniform_Mixing_texture_fs_Screen_Message");  // message string
	if ((uniform_Mixing_vp_proj == -1)
		|| (uniform_Mixing_vp_view == -1)
		|| (uniform_Mixing_vp_model == -1)
		|| (uniform_Mixing_fs_3fv_pulsedShift_height_flashCameraTrkWght == -1)
		|| (uniform_Mixing_fs_3fv_screenMsgTransp_Text1_2_Alpha == -1)
#ifdef PG_NB_CA_TYPES
		|| (uniform_Mixing_texture_fs_CA == -1)
#endif
#if defined (BLURRED_SPLAT_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES)
		|| (uniform_Mixing_texture_fs_Part_render == -1)
#endif
		|| (uniform_Mixing_texture_fs_Trk0 == -1)
#if PG_NB_TRACKS >= 2
		|| (uniform_Mixing_texture_fs_Trk1 == -1)
#endif
#if PG_NB_TRACKS >= 3
		|| (uniform_Mixing_texture_fs_Trk2 == -1)
#endif
#if PG_NB_TRACKS >= 4
		|| (uniform_Mixing_texture_fs_Trk3 == -1)
#endif
#if defined (TVW)
		|| (uniform_Mixing_texture_fs_Display_Font == -1)
		|| (uniform_Mixing_texture_fs_Display_Message1 == -1)
		|| (uniform_Mixing_texture_fs_Display_Message2 == -1)
#endif
		|| (uniform_Mixing_texture_fs_Render_prec == -1)
		|| (uniform_Mixing_texture_fs_Screen_Font == -1)
		|| (uniform_Mixing_texture_fs_Screen_Message == -1)
		) {
		fprintf(stderr, "Could not bind uniforms uniform_Mixing_texture_fs_Trk0 : %d\n", uniform_Mixing_texture_fs_Trk0);
#if PG_NB_TRACKS >= 2
		fprintf(stderr, "Could not bind uniforms uniform_Mixing_texture_fs_Trk1 : %d\n", uniform_Mixing_texture_fs_Trk1);
#endif
#if PG_NB_TRACKS >= 3
		fprintf(stderr, "Could not bind uniforms uniform_Mixing_texture_fs_Trk2 : %d\n", uniform_Mixing_texture_fs_Trk2);
#endif
#if PG_NB_TRACKS >= 4
		fprintf(stderr, "Could not bind uniforms uniform_Mixing_texture_fs_Trk3 : %d\n", uniform_Mixing_texture_fs_Trk3);
#endif
#if defined (TVW)
			fprintf(stderr, "Could not bind uniforms uniform_Mixing_texture_fs_Screen_Font : %d , uniform_Mixing_texture_fs_Display_Message1 : %d , uniform_Mixing_texture_fs_Display_Message2 : %d\n", uniform_Mixing_texture_fs_Screen_Font, uniform_Mixing_texture_fs_Display_Message1, uniform_Mixing_texture_fs_Display_Message2);
#endif
		fprintf(stderr, "Could not bind uniforms Mixing uniform_Mixing_vp_proj : %d, uniform_Mixing_vp_view : %d, uniform_Mixing_vp_model : %d, uniform_Mixing_fs_3fv_screenMsgTransp_Text1_2_Alpha : %d, uniform_Mixing_fs_3fv_pulsedShift_height_flashCameraTrkWght : %d, uniform_Mixing_texture_fs_Render_prec : %d, uniform_Mixing_texture_fs_Screen_Font : %d, uniform_Mixing_texture_fs_Screen_Message : %d\n", uniform_Mixing_vp_proj, uniform_Mixing_vp_view, uniform_Mixing_vp_model, uniform_Mixing_fs_3fv_screenMsgTransp_Text1_2_Alpha, uniform_Mixing_fs_3fv_pulsedShift_height_flashCameraTrkWght, uniform_Mixing_texture_fs_Render_prec, uniform_Mixing_texture_fs_Screen_Font, uniform_Mixing_texture_fs_Screen_Message);
#ifdef PG_NB_CA_TYPES
		fprintf(stderr, "Could not bind uniforms uniform_Mixing_texture_fs_CA : %d\n", uniform_Mixing_texture_fs_CA);
#endif
#if defined (BLURRED_SPLAT_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES)
		fprintf(stderr, "Could not bind uniforms uniform_Mixing_texture_fs_Part_render : %d\n", uniform_Mixing_texture_fs_Part_render);
#endif
		// return 0;
	}

	////////////////////////////////////////
	// shader parameeter bindings
	uniform_Master_vp_model
		= glGetUniformLocation(shader_programme[pg_shader_Master], "vp_modelMatrix");
	uniform_Master_vp_view
		= glGetUniformLocation(shader_programme[pg_shader_Master], "vp_viewMatrix");
	uniform_Master_vp_proj
		= glGetUniformLocation(shader_programme[pg_shader_Master], "vp_projMatrix");
	uniform_Master_fs_4fv_xy_frameno_pulsedShift
		= glGetUniformLocation(shader_programme[pg_shader_Master], "uniform_Master_fs_4fv_xy_frameno_pulsedShift");
	uniform_Master_fs_4fv_width_height_rightWindowVMargin_timeFromStart
		= glGetUniformLocation(shader_programme[pg_shader_Master], "uniform_Master_fs_4fv_width_height_rightWindowVMargin_timeFromStart");
	uniform_Master_fs_4fv_pulsedColor_rgb_pen_grey
		= glGetUniformLocation(shader_programme[pg_shader_Master], "uniform_Master_fs_4fv_pulsedColor_rgb_pen_grey");
	uniform_Master_fs_3fv_interpolatedPaletteLow_rgb
		= glGetUniformLocation(shader_programme[pg_shader_Master], "uniform_Master_fs_3fv_interpolatedPaletteLow_rgb");
	uniform_Master_fs_3fv_interpolatedPaletteMedium_rgb
		= glGetUniformLocation(shader_programme[pg_shader_Master], "uniform_Master_fs_3fv_interpolatedPaletteMedium_rgb");
	uniform_Master_fs_3fv_interpolatedPaletteHigh_rgb
		= glGetUniformLocation(shader_programme[pg_shader_Master], "uniform_Master_fs_3fv_interpolatedPaletteHigh_rgb");

	uniform_Master_texture_fs_Render_curr
		= glGetUniformLocation(shader_programme[pg_shader_Master], "uniform_Master_texture_fs_Render_curr"); // previous pass output
#ifdef PG_NB_CA_TYPES
	uniform_Master_texture_fs_CA
		= glGetUniformLocation(shader_programme[pg_shader_Master], "uniform_Master_texture_fs_CA"); // ping-pong CA (FBO)
#endif
#if defined (BLURRED_SPLAT_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES)
	uniform_Master_texture_fs_Part_render
		= glGetUniformLocation(shader_programme[pg_shader_Master], "uniform_Master_texture_fs_Part_render"); // Particles
#endif
	uniform_Master_texture_fs_Trk0
		= glGetUniformLocation(shader_programme[pg_shader_Master], "uniform_Master_texture_fs_Trk0"); // ping-pong track 0 (FBO)
#if defined (GN) || defined (INTERFERENCE)
	uniform_Master_texture_fs_LYMlogo
		= glGetUniformLocation(shader_programme[pg_shader_Master], "uniform_Master_texture_fs_LYMlogo"); // LYM logo (texture)
#endif
#if PG_NB_TRACKS >= 2
	uniform_Master_texture_fs_Trk1
		= glGetUniformLocation(shader_programme[pg_shader_Master], "uniform_Master_texture_fs_Trk1"); // ping-pong track 1 (FBO)
#endif
#if PG_NB_TRACKS >= 3
	uniform_Master_texture_fs_Trk2
		= glGetUniformLocation(shader_programme[pg_shader_Master], "uniform_Master_texture_fs_Trk2"); // ping-pong track 2 (FBO)
#endif
#if PG_NB_TRACKS >= 4
	uniform_Master_texture_fs_Trk3
		= glGetUniformLocation(shader_programme[pg_shader_Master], "uniform_Master_texture_fs_Trk3"); // ping-pong track 3 (FBO)
#endif

	if ((uniform_Master_vp_proj == -1)
		|| (uniform_Master_vp_view == -1)
		|| (uniform_Master_vp_model == -1)
		|| (uniform_Master_texture_fs_Render_curr == -1)
#ifdef PG_NB_CA_TYPES
		|| (uniform_Master_texture_fs_CA == -1)
#endif
#if defined (BLURRED_SPLAT_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES)
		|| (uniform_Master_texture_fs_Part_render == -1)
#endif
		|| (uniform_Master_texture_fs_Trk0 == -1)
#if PG_NB_TRACKS >= 2
		|| (uniform_Master_texture_fs_Trk1 == -1)
#endif
#if PG_NB_TRACKS >= 3
		|| (uniform_Master_texture_fs_Trk2 == -1)
#endif
#if PG_NB_TRACKS >= 4
		|| (uniform_Master_texture_fs_Trk3 == -1)
#endif
#if defined (GN) || defined (INTERFERENCE)
		|| (uniform_Master_texture_fs_LYMlogo == -1)
#endif
		|| (uniform_Master_fs_4fv_xy_frameno_pulsedShift == -1)
		|| (uniform_Master_fs_4fv_width_height_rightWindowVMargin_timeFromStart == -1)
		|| (uniform_Master_fs_4fv_pulsedColor_rgb_pen_grey == -1)
		|| (uniform_Master_fs_3fv_interpolatedPaletteLow_rgb == -1)
		|| (uniform_Master_fs_3fv_interpolatedPaletteMedium_rgb == -1)
		|| (uniform_Master_fs_3fv_interpolatedPaletteHigh_rgb == -1)
	) {
		fprintf(stderr, "Could not bind uniforms Master uniform_Master_vp_proj : %d, uniform_Master_vp_view : %d, uniform_Master_vp_model : %d , uniform_Master_texture_fs_Render_curr: %d, uniform_Master_fs_4fv_xy_frameno_pulsedShift : %d, uniform_Master_fs_4fv_width_height_rightWindowVMargin_timeFromStart : %d\n", uniform_Master_vp_proj, uniform_Master_vp_view, uniform_Master_vp_model, uniform_Master_texture_fs_Render_curr, uniform_Master_fs_4fv_xy_frameno_pulsedShift, uniform_Master_fs_4fv_width_height_rightWindowVMargin_timeFromStart);
#ifdef PG_NB_CA_TYPES
		fprintf(stderr, "Could not bind uniforms uniform_Master_texture_fs_CA: %d\n", uniform_Master_texture_fs_CA);
#endif
#if defined (BLURRED_SPLAT_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES)
		fprintf(stderr, "Could not bind uniforms uniform_Master_texture_fs_Part_render: %d\n", uniform_Master_texture_fs_Part_render);
#endif
		fprintf(stderr, "Could not bind uniforms Master uniform_Master_texture_fs_Trk0: %d, uniform_Master_fs_4fv_pulsedColor_rgb_pen_grey: %d, uniform_Master_fs_3fv_interpolatedPaletteLow_rgb: %d, uniform_Master_fs_3fv_interpolatedPaletteMedium_rgb: %d, uniform_Master_fs_3fv_interpolatedPaletteHigh_rgb: %d\n", uniform_Master_texture_fs_Trk0, uniform_Master_fs_4fv_pulsedColor_rgb_pen_grey, uniform_Master_fs_3fv_interpolatedPaletteLow_rgb, uniform_Master_fs_3fv_interpolatedPaletteMedium_rgb, uniform_Master_fs_3fv_interpolatedPaletteHigh_rgb);
#if PG_NB_TRACKS >= 2
		fprintf(stderr, "Could not bind uniforms Master uniform_Master_texture_fs_Trk1: %d\n", uniform_Master_texture_fs_Trk1);
#endif
#if PG_NB_TRACKS >= 3
		fprintf(stderr, "Could not bind uniforms Master uniform_Master_texture_fs_Trk2: %d\n", uniform_Master_texture_fs_Trk2);
#endif
#if PG_NB_TRACKS >= 4
		fprintf(stderr, "Could not bind uniforms Master uniform_Master_texture_fs_Trk3: %d\n", uniform_Master_texture_fs_Trk3);
#endif
#if defined (GN) || defined (INTERFERENCE)
		fprintf(stderr, "Could not bind uniforms Master uniform_Master_texture_fs_LYMlogo: %d\n", uniform_Master_texture_fs_LYMlogo);
#endif
		// return 0;
	}

#ifdef PG_SENSORS
	////////////////////////////////////////
	// shader parameeter bindings
	uniform_Sensor_vp_model
		= glGetUniformLocation(shader_programme[pg_shader_Sensor], "vp_modelMatrix");
	uniform_Sensor_vp_view
		= glGetUniformLocation(shader_programme[pg_shader_Sensor], "vp_viewMatrix");
	uniform_Sensor_vp_proj
		= glGetUniformLocation(shader_programme[pg_shader_Sensor], "vp_projMatrix");
	//uniform_Sensor_fs_2fv_frameno_invert
	//  = glGetUniformLocation(shader_programme[pg_shader_Sensor], "uniform_Sensor_fs_2fv_frameno_invert");
	uniform_Sensor_fs_4fv_onOff_isCurrentSensor_isFollowMouse_transparency
		= glGetUniformLocation(shader_programme[pg_shader_Sensor], "uniform_Sensor_fs_4fv_onOff_isCurrentSensor_isFollowMouse_transparency");

	uniform_Sensor_texture_fs_decal
		= glGetUniformLocation(shader_programme[pg_shader_Sensor], "fs_decal"); // previous pass output

	if ((uniform_Sensor_vp_proj == -1)
		|| (uniform_Sensor_vp_view == -1)
		|| (uniform_Sensor_vp_model == -1)
		//        || (uniform_Sensor_fs_2fv_frameno_invert == -1)
		|| (uniform_Sensor_fs_4fv_onOff_isCurrentSensor_isFollowMouse_transparency == -1)
		|| (uniform_Sensor_texture_fs_decal == -1)
		) {
		fprintf(stderr, "Could not bind uniforms Sensor uniform_Sensor_vp_proj : %d, uniform_Sensor_vp_view : %d, uniform_Sensor_vp_model : %d, uniform_Sensor_fs_4fv_onOff_isCurrentSensor_isFollowMouse_transparency : %d , uniform_Sensor_texture_fs_decal : %d\n", uniform_Sensor_vp_proj, uniform_Sensor_vp_view, uniform_Sensor_vp_model, uniform_Sensor_fs_4fv_onOff_isCurrentSensor_isFollowMouse_transparency, uniform_Sensor_texture_fs_decal);
		// return 0;
	}
#endif
}
