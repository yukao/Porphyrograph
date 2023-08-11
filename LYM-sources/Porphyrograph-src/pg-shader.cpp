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
#ifdef TVW
#include "pg_shader_body_decl_TVW.cpp"
#endif
#ifdef CRITON
#include "pg_shader_body_decl_Criton.cpp"
#endif
#if defined(KOMPARTSD)
#include "pg_shader_body_decl_KompartSD.cpp"
#endif
#if defined(LIGHT)
#include "pg_shader_body_decl_Light.cpp"
#endif
#if defined(RIVETS)
#include "pg_shader_body_decl_Rivets.cpp"
#endif
#if defined(CORE)
#include "pg_shader_body_decl_Core.cpp"
#endif
#if defined(FORET)
#include "pg_shader_body_decl_Foret.cpp"
#endif
#if defined(VOLUSPA)
#include "pg_shader_body_decl_voluspa.cpp"
#endif
#if defined(ARAKNIT)
#include "pg_shader_body_decl_araknit.cpp"
#endif
#if defined(CAAUDIO)
#include "pg_shader_body_decl_CAaudio.cpp"
#endif

/////////////////////////////////////////////////////////////////////////
// SHADER PROGRAMMES
unsigned int shader_programme[pg_NbShaderTotal];
std::unordered_map<int, std::string> pg_stringShaders = {
#if defined(var_part_initialization) 
	{ pg_shader_ParticleAnimation, "pg_shader_ParticleAnimation" },
	{ pg_shader_Update, "pg_shader_Update" },
#else
	{ pg_shader_Update, "pg_shader_Update" },
#endif
#if defined(var_part_initialization) 
	{ pg_shader_ParticleRender, "pg_shader_ParticleRender" },
#endif
	{ pg_shader_Mixing, "pg_shader_Mixing" },
	{ pg_shader_Master, "pg_shader_Master" },
#ifdef var_sensor_layout
	{ pg_shader_Sensor, "pg_shader_Sensor" },
#endif
#if defined(var_activeClipArts)
	{ pg_ClipArt, "pg_ClipArt" },
#endif
#if defined(var_activeMeshes)
	{ pg_shader_Mesh, "pg_shader_Mesh" },
#endif
	{ pg_NbShaderTotal, "pg_NbShaderTotal" },
};


// PARTICLE ANIMATION SHADER
// PARTICLE ANIMATION SHADER UNIFORM VARIABLES
GLint uniform_ParticleAnimation_vp_model;
GLint uniform_ParticleAnimation_vp_view;
GLint uniform_ParticleAnimation_vp_proj;
GLint uniform_ParticleAnimation_vp_2fv_width_height;
GLint uniform_ParticleAnimation_fs_4fv_W_H_repopChannel_targetFrameNo;
GLint uniform_ParticleAnimation_fs_4fv_flashTrkPartWghts;
GLint uniform_ParticleAnimation_path_data;
GLint uniform_ParticleAnimation_fs_4fv_repop_Color_frameNo;
GLint uniform_ParticleAnimation_fs_4fv_flashCAPartWght_nbPart_clear_nbPartInit;
GLint uniform_ParticleAnimation_fs_4fv_Camera_W_H_movieWH;

// UPDATE SHADER TEXTURE IDS
GLint uniform_ParticleAnimation_texture_fs_CA;         // ping-pong CA (FBO)
GLint uniform_ParticleAnimation_texture_fs_Part_pos_speed;  // 2-cycle ping-pong position/speed of particles (FBO)
GLint uniform_ParticleAnimation_texture_fs_Part_col_rad;  // 2-cycle ping-pong color/radius of particles (FBO)
GLint uniform_ParticleAnimation_texture_fs_Part_Target_pos_col_rad;  // 2-cycle ping-pong target position/color/radius of particles (FBO)
GLint uniform_ParticleAnimation_texture_fs_Camera_frame;  // camera frame
GLint uniform_ParticleAnimation_texture_fs_Movie_frame;  // movie frame
GLint uniform_ParticleAnimation_texture_fs_Noise;  // 3D noise
GLint uniform_ParticleAnimation_texture_fs_RepopDensity;  // repop density
GLint uniform_ParticleAnimation_texture_fs_Part_init_pos_speed;  // particle initialization pairs of textures position/speed
GLint uniform_ParticleAnimation_texture_fs_Part_init_col_rad;  // particle initialization pairs of textures color/radius
GLint uniform_ParticleAnimation_texture_fs_Part_acc;  // particle acceleration texture
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
#if defined(PG_WITH_PHOTO_HOMOGRAPHY)
GLint uniform_Update_homographyForTexture;
#endif
GLint uniform_Update_vp_2fv_width_height;
GLint uniform_Update_fs_4fv_W_H_time_currentScene;
GLint uniform_Update_fs_3fv_clearAllLayers_clearCA_pulsedShift;
#if defined(CAAUDIO) || defined(RIVETS)
GLint uniform_Update_fs_4fv_CAseed_type_size_loc;
#endif
#if defined(var_part_initialization) || PG_NB_TRACKS >= 2
GLint uniform_Update_fs_4fv_flashTrkBGWghts_flashPartBGWght;
#endif
GLint uniform_Update_fs_3fv_frameno_Cursor_flashPartCAWght;
#ifdef var_nb_CATypes
GLint uniform_Update_fs_4fv_flashTrkCAWghts;
#endif
GLint uniform_Update_path_data;

#ifdef CRITON
GLint uniform_Update_fs_4fv_fftLevels03;
GLint uniform_Update_fs_4fv_fftFrequencies03;
GLint uniform_Update_fs_4fv_fftPhases03;
GLint uniform_Update_fs_4fv_fftLevels47;
GLint uniform_Update_fs_4fv_fftFrequencies47;
GLint uniform_Update_fs_4fv_fftPhases47;
#endif
GLint uniform_Update_fs_4fv_movieWH_flashCameraTrkWght_cpTrack;
GLint uniform_Update_fs_4fv_repop_ColorBG_flashCABGWght;
#if defined(var_nb_CATypes) && !defined(var_alKemi)
GLint uniform_Update_fs_3fv_repop_ColorCA;
#endif
GLint uniform_Update_fs_3fv_isClearLayer_flashPixel_flashCameraTrkThres;
#if defined(var_flashPhotoTrkBeat) && defined(var_flashPhotoTrkBright) && defined(var_flashPhotoTrkLength) && defined(var_flashPhotoChangeBeat)
GLint uniform_Update_fs_4fv_flashPhotoTrkWght_flashPhotoTrkThres_Photo_offSetsXY;
#endif
#if defined(var_photo_diaporama) || defined(TVW)
GLint uniform_Update_fs_4fv_photo01_wh;
#endif
#ifdef var_photo_diaporama
GLint uniform_Update_fs_4fv_photo01Wghts_randomValues;
#endif
#if defined(var_clipCaptFreq) && PG_NB_PARALLEL_CLIPS >= 2
GLint uniform_Update_fs_2fv_clip01Wghts;
#endif
#if defined(var_cameraCaptFreq)
GLint uniform_Update_fs_4fv_Camera_offSetsXY_Camera_W_H;
#endif
GLint uniform_Update_fs_4fv_xy_transl_tracks_0_1;
#if defined(var_nb_CATypes) || defined(PG_WITH_BLUR)
GLint uniform_Update_fs_4fv_CAType_SubType_blurRadius;
#endif
#if defined(var_clipCaptFreq)
GLint uniform_Update_fs_3fv_photo_rgb;
#endif
#if defined(var_CATable)
GLint uniform_Update_texture_fs_CATable;
#endif
#if defined(var_GenerativeNights_planes)
GLint uniform_Update_fs_2fv_initCA_1stPlaneFrameNo;
#endif
#if defined(var_camera_BG_ini_subtr)
GLint uniform_Update_texture_fs_Camera_BGIni;  // initial camera BG capture
#endif
GLint uniform_Update_texture_fs_pixel_acc;

// UPDATE SHADER TEXTURE IDS
#ifdef var_nb_CATypes
GLint uniform_Update_texture_fs_CA;         // ping-pong CA (FBO)
#endif
#ifdef var_alKemi
GLint uniform_Update_texture_fs_PreviousCA;         // ping-pong CA (FBO)
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
#if !defined(PG_BEZIER_PATHS) || defined(FORET) || defined(CORE)
GLint uniform_Update_texture_fs_Brushes;  // pen patterns
#endif
GLint uniform_Update_texture_fs_Camera_frame;  // camera frame
GLint uniform_Update_texture_fs_Camera_BG;  // current camera BG capture
GLint uniform_Update_texture_fs_Movie_frame;  // movie frame
GLint uniform_Update_texture_fs_Noise;  // 3D noise
#if defined(var_BG_CA_repop_density)
GLint uniform_Update_texture_fs_RepopDensity;  // repop density
#endif
GLint uniform_Update_texture_fs_Photo0;  // photo[0]
GLint uniform_Update_texture_fs_Photo1;  // photo[1]
GLint uniform_Update_texture_fs_Clip0;  // clip[0]
GLint uniform_Update_texture_fs_Clip1;  // clip[1]
#if defined(var_part_initialization) 
GLint uniform_Update_texture_fs_Part_render;  // FBO capture of particle rendering used for flashing layers with particles
#endif
#if defined(TVW)
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
#if defined(var_part_initialization) 
GLint uniform_ParticleSplat_vp_model;
GLint uniform_ParticleSplat_vp_view;
GLint uniform_ParticleSplat_vp_proj;
GLint uniform_ParticleSplat_gs_4fv_part_size_partType_highPitchPulse_windowRatio;
GLint uniform_ParticleSplat_vp_3fv_trackReplay_xy_height;

// PARTICLE SHADER TEXTURE IDS
GLint uniform_ParticleSplat_texture_vp_Part_pos_speed;				 // Particle position/speed update
GLint uniform_ParticleSplat_texture_vp_Part_col_rad;          // Particle color/radius update
#endif
#if defined(CURVE_PARTICLES) 
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
#if defined(TEXTURED_QUAD_PARTICLES)
// color texture
GLint uniform_ParticleSplat_texture_fs_decal;  // blurred disk texture
#endif

/////////////////////////////////////////////////////////////////////////
// MIXING SHADER
// MIXING SHADER UNIFORM VARIABLES
GLint uniform_Mixing_vp_model;
GLint uniform_Mixing_vp_view;
GLint uniform_Mixing_vp_proj;
#if defined(var_flashPhotoTrkBeat) && defined(var_flashPhotoTrkBright) && defined(var_flashPhotoTrkLength) && defined(var_flashPhotoChangeBeat)
GLint uniform_Mixing_fs_3fv_height_flashCameraTrkWght_flashPhotoTrkWght;
#else
GLint uniform_Mixing_fs_2fv_height_flashCameraTrkWght;
#endif
GLint uniform_Mixing_fs_3fv_screenMsgTransp_Text1_2_Alpha;

// MIXING SHADER TEXTURE IDS
#ifdef var_nb_CATypes
GLint uniform_Mixing_texture_fs_CA;         // ping-pong CA (FBO)
#endif
#if defined(var_part_initialization)
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
#if defined(TVW)
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
#if defined(TVW) || defined(var_alKemi)
GLint uniform_Master_fs_4fv_width_height_rightWindowVMargin_timeFromStart;
#else
GLint uniform_Master_fs_4fv_width_height_timeFromStart_muteRightScreen;
#endif
//GLint uniform_Master_fs_4fv_pulsedColor_rgb_pen_grey;
//GLint uniform_Master_fs_4fv_interpolatedPaletteLow_rgb_currentScene;
//GLint uniform_Master_fs_4fv_interpolatedPaletteMedium_rgb_mobile_cursor;
//GLint uniform_Master_fs_3fv_interpolatedPaletteHigh_rgb;
GLint uniform_Master_fs_2iv_mobile_cursor_currentScene;
#ifdef var_Caverne_BackColor
GLint uniform_Master_fs_3fv_Caverne_BackColor_rgb;
#endif
#if defined(var_flashchange_BGcolor_freq)
GLint uniform_Master_fs_3fv_BG_color_rgb;
#endif

// MASTER SHADER TEXTURE IDS
GLint uniform_Master_texture_fs_Render_curr;         // previous pass output
#ifdef var_nb_CATypes
GLint uniform_Master_texture_fs_CA;  // ping-pong CA (FBO)
#endif
GLint uniform_Master_texture_fs_Part_render;  // Particles
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
#ifdef PG_WITH_MASTER_MASK
GLint uniform_Master_texture_fs_Mask;  // mask for master output
#endif

#ifdef var_sensor_layout
/////////////////////////////////////////////////////////////////////////
// SENSOR SHADER
// SENSOR SHADER UNIFORM VARIABLES
GLint uniform_Sensor_vp_model;
GLint uniform_Sensor_vp_view;
GLint uniform_Sensor_vp_proj;

// SENSOR SHADER TEXTURE IDS
GLint uniform_Sensor_texture_fs_decal;         // sensor texture
GLint uniform_Sensor_fs_4fv_onOff_isCurrentSensor_masterLevel_transparency;
// GLint uniform_Sensor_fs_2fv_frameno_invert;
#endif

#if defined(var_activeMeshes)
/////////////////////////////////////////////////////////////////////////
// MESH SHADER
// MESH SHADER UNIFORM VARIABLES
GLint uniform_Mesh_vp_model;
GLint uniform_Mesh_vp_view;
GLint uniform_Mesh_vp_proj;
GLint uniform_Mesh_fs_4fv_isDisplayLookAt_with_mesh_with_blue_currentScene;
GLint uniform_Mesh_fs_3fv_light;
#if defined(var_Contact_mesh_expand)
GLint uniform_Mesh_vp_2fv_dilate_explode;
#endif
#if defined(var_Contact_mesh_anime)
GLint uniform_Mesh_bones_matrices;
#endif
#if defined(var_MmeShanghai_brokenGlass)
GLint uniform_Mesh_fs_4fv_color;
GLint uniform_Mesh_fs_4fv_color_master_photo_weight_bg;
#endif
// Augmented Reality: FBO capture of Master to be displayed on a mesh
#if defined(var_textureFrontier_wmin) && defined(var_textureFrontier_wmax) and defined(var_textureFrontier_hmin) && defined(var_textureFrontier_hmax) && defined(var_textureFrontier_wmin_width) && defined(var_textureFrontier_wmax_width) and defined(var_textureFrontier_hmin_width) && defined(var_textureFrontier_hmax_width) && defined(var_textureScale_w) && defined(var_textureScale_h) and defined(var_textureTranslate_w) && defined(var_textureTranslate_h) 
GLint uniform_Mesh_fs_4fv_textureFrontier;
GLint uniform_Mesh_fs_4fv_textureFrontier_width;
GLint uniform_Mesh_fs_4fv_textureScaleTransl;
#endif
// Mesh SHADER TEXTURE IDS
GLint uniform_Mesh_texture_fs_decal;         // Mesh texture
#if defined(var_Contact_mesh_palette)
GLint uniform_Mesh_fs_4fv_color_palette;         // Mesh color
#endif
#if defined(var_MmeShanghai_brokenGlass)
GLint uniform_Mesh_texture_fs_BG;         // Mesh texture
#endif
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

void bindAndCheckUniform(int *uniformID, string uniformString, ShaderFileTypes shaderType) {
	*uniformID = glGetUniformLocation(shader_programme[shaderType], uniformString.c_str());
	printOglError(332);
	if (*uniformID == -1) {
		std::cerr << "Could not bind uniform " << uniformString << " (ID: " << int(*uniformID) << ") in shader " << pg_stringShaders[shaderType] << ".\n";
	}
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

#ifdef TVW
#include "pg_shader_body_bind_TVW.cpp"
#endif
#ifdef CRITON
#include "pg_shader_body_bind_Criton.cpp"
#endif
#if defined(KOMPARTSD)
#include "pg_shader_body_bind_KompartSD.cpp"
#endif
#if defined(KOMPARTSD)
#include "pg_shader_body_bind_Light.cpp"
#endif
#if defined(RIVETS)
#include "pg_shader_body_bind_Rivets.cpp"
#endif
#if defined(CORE)
#include "pg_shader_body_bind_Core.cpp"
#endif
#if defined(FORET)
#include "pg_shader_body_bind_Foret.cpp"
#endif
#if defined(VOLUSPA)
#include "pg_shader_body_bind_voluspa.cpp"
#endif
#if defined(ARAKNIT)
#include "pg_shader_body_bind_araknit.cpp"
#endif
#if defined(CAAUDIO)
#include "pg_shader_body_bind_CAaudio.cpp"
#endif

#if defined(var_part_initialization) 
	////////////////////////////////////////////////////////////////////////////////
	// PARTICLE ANIMATION SHADER parameters bindings
	////////////////////////////////////////////////////////////////////////////////
	bindAndCheckUniform(&uniform_ParticleAnimation_vp_model, "vp_modelMatrix", pg_shader_ParticleAnimation);
	bindAndCheckUniform(&uniform_ParticleAnimation_vp_view, "vp_viewMatrix", pg_shader_ParticleAnimation);
	bindAndCheckUniform(&uniform_ParticleAnimation_vp_proj, "vp_projMatrix", pg_shader_ParticleAnimation);

	bindAndCheckUniform(&uniform_ParticleAnimation_vp_2fv_width_height, "uniform_ParticleAnimation_vp_2fv_width_height", pg_shader_ParticleAnimation);
	bindAndCheckUniform(&uniform_ParticleAnimation_fs_4fv_W_H_repopChannel_targetFrameNo, "uniform_ParticleAnimation_fs_4fv_W_H_repopChannel_targetFrameNo", pg_shader_ParticleAnimation);
	bindAndCheckUniform(&uniform_ParticleAnimation_fs_4fv_flashTrkPartWghts, "uniform_ParticleAnimation_fs_4fv_flashTrkPartWghts", pg_shader_ParticleAnimation);
	bindAndCheckUniform(&uniform_ParticleAnimation_path_data, "uniform_ParticleAnimation_path_data", pg_shader_ParticleAnimation);
	bindAndCheckUniform(&uniform_ParticleAnimation_fs_4fv_repop_Color_frameNo, "uniform_ParticleAnimation_fs_4fv_repop_Color_frameNo", pg_shader_ParticleAnimation);
	bindAndCheckUniform(&uniform_ParticleAnimation_fs_4fv_flashCAPartWght_nbPart_clear_nbPartInit, "uniform_ParticleAnimation_fs_4fv_flashCAPartWght_nbPart_clear_nbPartInit", pg_shader_ParticleAnimation);
	bindAndCheckUniform(&uniform_ParticleAnimation_fs_4fv_Camera_W_H_movieWH, "uniform_ParticleAnimation_fs_4fv_Camera_W_H_movieWH", pg_shader_ParticleAnimation);

	// BG track FBO
	bindAndCheckUniform(&uniform_ParticleAnimation_texture_fs_CA, "uniform_ParticleAnimation_texture_fs_CA", pg_shader_ParticleAnimation);  // 2-cycle ping-pong position/speed of particles (FBO)
	bindAndCheckUniform(&uniform_ParticleAnimation_texture_fs_Part_pos_speed, "uniform_ParticleAnimation_texture_fs_Part_pos_speed", pg_shader_ParticleAnimation);  // 2-cycle ping-pong position/speed of particles (FBO)
	bindAndCheckUniform(&uniform_ParticleAnimation_texture_fs_Part_col_rad, "uniform_ParticleAnimation_texture_fs_Part_col_rad", pg_shader_ParticleAnimation);  // 2-cycle ping-pong color/radius of particles (FBO)
	bindAndCheckUniform(&uniform_ParticleAnimation_texture_fs_Part_Target_pos_col_rad, "uniform_ParticleAnimation_texture_fs_Part_Target_pos_col_rad", pg_shader_ParticleAnimation);  // 2-cycle ping-pong target position/color/radius of particles (FBO)
	bindAndCheckUniform(&uniform_ParticleAnimation_texture_fs_Trk0, "uniform_ParticleAnimation_texture_fs_Trk0", pg_shader_ParticleAnimation);  // ping-pong track 0 (FBO)
#if PG_NB_TRACKS >= 2
	bindAndCheckUniform(&uniform_ParticleAnimation_texture_fs_Trk1, "uniform_ParticleAnimation_texture_fs_Trk1", pg_shader_ParticleAnimation);  // ping-pong track 1 (FBO)
#endif
#if PG_NB_TRACKS >= 3
	bindAndCheckUniform(&uniform_ParticleAnimation_texture_fs_Trk2, "uniform_ParticleAnimation_texture_fs_Trk2", pg_shader_ParticleAnimation);  // ping-pong track 2 (FBO)
#endif
#if PG_NB_TRACKS >= 4
	bindAndCheckUniform(&uniform_ParticleAnimation_texture_fs_Trk3, "uniform_ParticleAnimation_texture_fs_Trk3", pg_shader_ParticleAnimation);  // ping-pong track 3 (FBO)
#endif

	///////////////////////////////////////////////////////////////////////////
	// textures

	// video frames
	bindAndCheckUniform(&uniform_ParticleAnimation_texture_fs_Camera_frame, "uniform_ParticleAnimation_texture_fs_Camera_frame", pg_shader_ParticleAnimation);  // camera frame
	bindAndCheckUniform(&uniform_ParticleAnimation_texture_fs_Movie_frame, "uniform_ParticleAnimation_texture_fs_Movie_frame", pg_shader_ParticleAnimation);  // movie frame

	bindAndCheckUniform(&uniform_ParticleAnimation_texture_fs_Noise, "uniform_ParticleAnimation_texture_fs_Noise", pg_shader_ParticleAnimation);  // 3D noise

	bindAndCheckUniform(&uniform_ParticleAnimation_texture_fs_RepopDensity, "uniform_ParticleAnimation_texture_fs_RepopDensity", pg_shader_ParticleAnimation);  // repop density
	bindAndCheckUniform(&uniform_ParticleAnimation_texture_fs_Part_init_pos_speed, "uniform_ParticleAnimation_texture_fs_Part_init_pos_speed", pg_shader_ParticleAnimation);  // particle initialization pairs of textures position/speed
	bindAndCheckUniform(&uniform_ParticleAnimation_texture_fs_Part_init_col_rad, "uniform_ParticleAnimation_texture_fs_Part_init_col_rad", pg_shader_ParticleAnimation);  // particle initialization pairs of textures color/radius
	bindAndCheckUniform(&uniform_ParticleAnimation_texture_fs_Part_acc, "uniform_ParticleAnimation_texture_fs_Part_acc", pg_shader_ParticleAnimation);  // particle acceleration texture
#endif

	////////////////////////////////////////////////////////////////////////////////
	// UPDATE SHADER parameters bindings
	////////////////////////////////////////////////////////////////////////////////
	bindAndCheckUniform(&uniform_Update_vp_model, "vp_modelMatrix", pg_shader_Update);
	bindAndCheckUniform(&uniform_Update_vp_view, "vp_viewMatrix", pg_shader_Update);
	bindAndCheckUniform(&uniform_Update_vp_proj, "vp_projMatrix", pg_shader_Update);
#if defined(PG_WITH_PHOTO_HOMOGRAPHY)
	bindAndCheckUniform(&uniform_Update_homographyForTexture, "fs_homographyForTexture", pg_shader_Update);
#endif

	bindAndCheckUniform(&uniform_Update_vp_2fv_width_height, "uniform_Update_vp_2fv_width_height", pg_shader_Update);
	bindAndCheckUniform(&uniform_Update_fs_4fv_W_H_time_currentScene, "uniform_Update_fs_4fv_W_H_time_currentScene", pg_shader_Update);
	bindAndCheckUniform(&uniform_Update_fs_3fv_clearAllLayers_clearCA_pulsedShift, "uniform_Update_fs_3fv_clearAllLayers_clearCA_pulsedShift", pg_shader_Update);
#if defined(CAAUDIO) || defined(RIVETS)
	bindAndCheckUniform(&uniform_Update_fs_4fv_CAseed_type_size_loc, "uniform_Update_fs_4fv_CAseed_type_size_loc", pg_shader_Update);
#endif
#if defined(var_part_initialization) || PG_NB_TRACKS >= 2
	bindAndCheckUniform(&uniform_Update_fs_4fv_flashTrkBGWghts_flashPartBGWght, "uniform_Update_fs_4fv_flashTrkBGWghts_flashPartBGWght", pg_shader_Update);
#endif
	bindAndCheckUniform(&uniform_Update_fs_3fv_frameno_Cursor_flashPartCAWght, "uniform_Update_fs_3fv_frameno_Cursor_flashPartCAWght", pg_shader_Update);
#ifdef var_nb_CATypes
	bindAndCheckUniform(&uniform_Update_fs_4fv_flashTrkCAWghts, "uniform_Update_fs_4fv_flashTrkCAWghts", pg_shader_Update);
#endif
	bindAndCheckUniform(&uniform_Update_path_data, "uniform_Update_path_data", pg_shader_Update);
#ifdef CRITON
	bindAndCheckUniform(&uniform_Update_fs_4fv_fftLevels03, "uniform_Update_fs_4fv_fftLevels03", pg_shader_Update);
	bindAndCheckUniform(&uniform_Update_fs_4fv_fftFrequencies03, "uniform_Update_fs_4fv_fftFrequencies03", pg_shader_Update);
	bindAndCheckUniform(&uniform_Update_fs_4fv_fftPhases03, "uniform_Update_fs_4fv_fftPhases03", pg_shader_Update);
	bindAndCheckUniform(&uniform_Update_fs_4fv_fftLevels47, "uniform_Update_fs_4fv_fftLevels47", pg_shader_Update);
	bindAndCheckUniform(&uniform_Update_fs_4fv_fftFrequencies47, "uniform_Update_fs_4fv_fftFrequencies47", pg_shader_Update);
	bindAndCheckUniform(&uniform_Update_fs_4fv_fftPhases47, "uniform_Update_fs_4fv_fftPhases47", pg_shader_Update);
#endif
	bindAndCheckUniform(&uniform_Update_fs_4fv_movieWH_flashCameraTrkWght_cpTrack, "uniform_Update_fs_4fv_movieWH_flashCameraTrkWght_cpTrack", pg_shader_Update);
	bindAndCheckUniform(&uniform_Update_fs_4fv_repop_ColorBG_flashCABGWght, "uniform_Update_fs_4fv_repop_ColorBG_flashCABGWght", pg_shader_Update);
#if defined(var_nb_CATypes) && !defined(var_alKemi)
	bindAndCheckUniform(&uniform_Update_fs_3fv_repop_ColorCA, "uniform_Update_fs_3fv_repop_ColorCA", pg_shader_Update);
#endif
	bindAndCheckUniform(&uniform_Update_fs_3fv_isClearLayer_flashPixel_flashCameraTrkThres, "uniform_Update_fs_3fv_isClearLayer_flashPixel_flashCameraTrkThres", pg_shader_Update);
#if defined(var_flashPhotoTrkBeat) && defined(var_flashPhotoTrkBright) && defined(var_flashPhotoTrkLength) && defined(var_flashPhotoChangeBeat)
	bindAndCheckUniform(&uniform_Update_fs_4fv_flashPhotoTrkWght_flashPhotoTrkThres_Photo_offSetsXY, "uniform_Update_fs_4fv_flashPhotoTrkWght_flashPhotoTrkThres_Photo_offSetsXY", pg_shader_Update);
#endif
#if defined(var_photo_diaporama) || defined(TVW)
	bindAndCheckUniform(&uniform_Update_fs_4fv_photo01_wh, "uniform_Update_fs_4fv_photo01_wh", pg_shader_Update);
#endif
#ifdef var_photo_diaporama
	bindAndCheckUniform(&uniform_Update_fs_4fv_photo01Wghts_randomValues, "uniform_Update_fs_4fv_photo01Wghts_randomValues", pg_shader_Update);
#endif
#if defined(var_clipCaptFreq) && PG_NB_PARALLEL_CLIPS >= 2
	bindAndCheckUniform(&uniform_Update_fs_2fv_clip01Wghts, "uniform_Update_fs_2fv_clip01Wghts", pg_shader_Update);
#endif
#if defined(var_cameraCaptFreq)
	bindAndCheckUniform(&uniform_Update_fs_4fv_Camera_offSetsXY_Camera_W_H, "uniform_Update_fs_4fv_Camera_offSetsXY_Camera_W_H", pg_shader_Update);
#endif
#if defined(var_clipCaptFreq)
	bindAndCheckUniform(&uniform_Update_fs_3fv_photo_rgb, "uniform_Update_fs_3fv_photo_rgb", pg_shader_Update);
#endif
	bindAndCheckUniform(&uniform_Update_fs_4fv_xy_transl_tracks_0_1, "uniform_Update_fs_4fv_xy_transl_tracks_0_1", pg_shader_Update);
#if defined(var_nb_CATypes) || defined(PG_WITH_BLUR)
	bindAndCheckUniform(&uniform_Update_fs_4fv_CAType_SubType_blurRadius, "uniform_Update_fs_4fv_CAType_SubType_blurRadius", pg_shader_Update);
#endif
#if defined(var_CATable)
	bindAndCheckUniform(&uniform_Update_texture_fs_CATable, "uniform_Update_texture_fs_CATable", pg_shader_Update);
#endif
#if defined(var_GenerativeNights_planes)
	bindAndCheckUniform(&uniform_Update_fs_2fv_initCA_1stPlaneFrameNo, "uniform_Update_fs_2fv_initCA_1stPlaneFrameNo", pg_shader_Update);
#endif
#if defined(var_camera_BG_ini_subtr)
	bindAndCheckUniform(&uniform_Update_texture_fs_Camera_BGIni, "uniform_Update_texture_fs_Camera_BGIni", pg_shader_Update);  // initial background frame
#endif
	bindAndCheckUniform(&uniform_Update_texture_fs_pixel_acc, "uniform_Update_texture_fs_pixel_acc", pg_shader_Update);  // initial background frame

	// BG track FBO
#ifdef var_nb_CATypes
	bindAndCheckUniform(&uniform_Update_texture_fs_CA, "uniform_Update_texture_fs_CA", pg_shader_Update);         // 2-cycle ping-pong CA (FBO)
#endif
#ifdef var_alKemi
	bindAndCheckUniform(&uniform_Update_texture_fs_PreviousCA, "uniform_Update_texture_fs_PreviousCA", pg_shader_Update);         // 2-cycle ping-pong CA (FBO)
#endif
#ifdef PG_NB_PIXEL_MODES
	bindAndCheckUniform(&uniform_Update_texture_fs_Pixels, "uniform_Update_texture_fs_Pixels", pg_shader_Update);  // 2-cycle ping-pong speed/position of pixels (FBO)
#endif
	bindAndCheckUniform(&uniform_Update_texture_fs_Trk0, "uniform_Update_texture_fs_Trk0", pg_shader_Update);  // ping-pong track 0 (FBO)
#if PG_NB_TRACKS >= 2
	bindAndCheckUniform(&uniform_Update_texture_fs_Trk1, "uniform_Update_texture_fs_Trk1", pg_shader_Update);  // ping-pong track 1 (FBO)
#endif
#if PG_NB_TRACKS >= 3
	bindAndCheckUniform(&uniform_Update_texture_fs_Trk2, "uniform_Update_texture_fs_Trk2", pg_shader_Update);  // ping-pong track 2 (FBO)
#endif
#if PG_NB_TRACKS >= 4
	bindAndCheckUniform(&uniform_Update_texture_fs_Trk3, "uniform_Update_texture_fs_Trk3", pg_shader_Update);  // ping-pong track 3 (FBO)
#endif
																									   // textures
#if !defined(PG_BEZIER_PATHS) || defined(FORET) || defined(CORE)
	bindAndCheckUniform(&uniform_Update_texture_fs_Brushes, "uniform_Update_texture_fs_Brushes", pg_shader_Update);  // pen patterns
#endif

	bindAndCheckUniform(&uniform_Update_texture_fs_Camera_frame, "uniform_Update_texture_fs_Camera_frame", pg_shader_Update);  // camera frame
	bindAndCheckUniform(&uniform_Update_texture_fs_Camera_BG, "uniform_Update_texture_fs_Camera_BG", pg_shader_Update);  // current background frame
	bindAndCheckUniform(&uniform_Update_texture_fs_Movie_frame, "uniform_Update_texture_fs_Movie_frame", pg_shader_Update);  // movie frame
	bindAndCheckUniform(&uniform_Update_texture_fs_Noise, "uniform_Update_texture_fs_Noise", pg_shader_Update);  // 3D noise
#if defined(var_BG_CA_repop_density)
	bindAndCheckUniform(&uniform_Update_texture_fs_RepopDensity, "uniform_Update_texture_fs_RepopDensity", pg_shader_Update);  // repop density
#endif

// photo frames
	bindAndCheckUniform(&uniform_Update_texture_fs_Photo0, "uniform_Update_texture_fs_Photo0", pg_shader_Update);  // photo[0] frame
	bindAndCheckUniform(&uniform_Update_texture_fs_Photo1, "uniform_Update_texture_fs_Photo1", pg_shader_Update);  // photo[1] frame
	bindAndCheckUniform(&uniform_Update_texture_fs_Clip0, "uniform_Update_texture_fs_Clip0", pg_shader_Update);  // clip[0]
	bindAndCheckUniform(&uniform_Update_texture_fs_Clip1, "uniform_Update_texture_fs_Clip1", pg_shader_Update);  // clip[1]
#if defined(var_part_initialization) 
	bindAndCheckUniform(&uniform_Update_texture_fs_Part_render, "uniform_Update_texture_fs_Part_render", pg_shader_Update);  // particle rendering capture
#endif
#if defined(TVW)
	bindAndCheckUniform(&uniform_Update_fs_4fv_weights03, "uniform_Update_fs_4fv_weights03", pg_shader_Update);
	bindAndCheckUniform(&uniform_Update_fs_2fv_weights45, "uniform_Update_fs_2fv_weights45", pg_shader_Update);
	bindAndCheckUniform(&uniform_Update_fs_3fv_alphaSwap02, "uniform_Update_fs_3fv_alphaSwap02", pg_shader_Update);
	bindAndCheckUniform(&uniform_Update_fs_3fv_alphaSwap35, "uniform_Update_fs_3fv_alphaSwap35", pg_shader_Update);
	bindAndCheckUniform(&uniform_Update_fs_4fv_image_noisesxy, "uniform_Update_fs_4fv_image_noisesxy", pg_shader_Update);
	bindAndCheckUniform(&uniform_Update_fs_4fv_mask_noisesxy, "uniform_Update_fs_4fv_mask_noisesxy", pg_shader_Update);

	bindAndCheckUniform(&uniform_Update_texture_fs_TVWPixels0, "uniform_Update_texture_fs_TVWPixels0", pg_shader_Update);  // image buffer #0
	bindAndCheckUniform(&uniform_Update_texture_fs_TVWPixels1, "uniform_Update_texture_fs_TVWPixels1", pg_shader_Update);  // image buffer #1
	bindAndCheckUniform(&uniform_Update_texture_fs_TVWPixels2, "uniform_Update_texture_fs_TVWPixels2", pg_shader_Update);  // image buffer #2
	bindAndCheckUniform(&uniform_Update_texture_fs_TVWPixels3, "uniform_Update_texture_fs_TVWPixels3", pg_shader_Update);  // image buffer #3
	bindAndCheckUniform(&uniform_Update_texture_fs_TVWPixels4, "uniform_Update_texture_fs_TVWPixels4", pg_shader_Update);  // image buffer #4
	bindAndCheckUniform(&uniform_Update_texture_fs_TVWPixels5, "uniform_Update_texture_fs_TVWPixels5", pg_shader_Update);  // image buffer #5
	bindAndCheckUniform(&uniform_Update_texture_fs_TVWMask02, "uniform_Update_texture_fs_TVWMask02", pg_shader_Update);  // image mask #0
	bindAndCheckUniform(&uniform_Update_texture_fs_TVWMask35, "uniform_Update_texture_fs_TVWMask35", pg_shader_Update);  // image mask #1
	bindAndCheckUniform(&uniform_Update_texture_fs_TVWPixelsSwap0, "uniform_Update_texture_fs_TVWPixelsSwap0", pg_shader_Update);  // buffer swap image #0
	bindAndCheckUniform(&uniform_Update_texture_fs_TVWPixelsSwap1, "uniform_Update_texture_fs_TVWPixelsSwap1", pg_shader_Update);  // buffer swap image #1
	bindAndCheckUniform(&uniform_Update_texture_fs_TVWPixelsSwap2, "uniform_Update_texture_fs_TVWPixelsSwap2", pg_shader_Update);  // buffer swap image #2
	bindAndCheckUniform(&uniform_Update_texture_fs_TVWPixelsSwap3, "uniform_Update_texture_fs_TVWPixelsSwap3", pg_shader_Update);  // buffer swap image #3
	bindAndCheckUniform(&uniform_Update_texture_fs_TVWPixelsSwap4, "uniform_Update_texture_fs_TVWPixelsSwap4", pg_shader_Update);  // buffer swap image #4
	bindAndCheckUniform(&uniform_Update_texture_fs_TVWPixelsSwap5, "uniform_Update_texture_fs_TVWPixelsSwap5", pg_shader_Update);  // buffer swap image #5
#endif

	

	////////////////////////////////////////////////////////////////////////////////
	// PARTICLE RENDERING SHADER parameters bindings
	////////////////////////////////////////////////////////////////////////////////
#if defined(var_part_initialization) 
	bindAndCheckUniform(&uniform_ParticleSplat_vp_model, "vp_modelMatrix", pg_shader_ParticleRender);
	bindAndCheckUniform(&uniform_ParticleSplat_vp_view, "vp_viewMatrix", pg_shader_ParticleRender);
	bindAndCheckUniform(&uniform_ParticleSplat_vp_proj, "vp_projMatrix", pg_shader_ParticleRender);

	bindAndCheckUniform(&uniform_ParticleSplat_gs_4fv_part_size_partType_highPitchPulse_windowRatio, "uniform_ParticleSplat_gs_4fv_part_size_partType_highPitchPulse_windowRatio", pg_shader_ParticleRender);
	bindAndCheckUniform(&uniform_ParticleSplat_vp_3fv_trackReplay_xy_height, "uniform_ParticleSplat_vp_3fv_trackReplay_xy_height", pg_shader_ParticleRender);

	// particle position/speed FBO
	bindAndCheckUniform(&uniform_ParticleSplat_texture_vp_Part_pos_speed, "uniform_ParticleSplat_texture_vp_Part_pos_speed", pg_shader_ParticleRender);
	// particle color/radius FBO
	bindAndCheckUniform(&uniform_ParticleSplat_texture_vp_Part_col_rad, "uniform_ParticleSplat_texture_vp_Part_col_rad", pg_shader_ParticleRender);
#endif
#if defined(CURVE_PARTICLES) 
	bindAndCheckUniform(&uniform_ParticleCurve_vp_model, "vp_modelMatrix", pg_shader_ParticleRender);
	bindAndCheckUniform(&uniform_ParticleCurve_vp_view, "vp_viewMatrix", pg_shader_ParticleRender);
	bindAndCheckUniform(&uniform_ParticleCurve_vp_proj, "vp_projMatrix", pg_shader_ParticleRender);

	bindAndCheckUniform(&uniform_ParticleCurve_gs_3fv_partRadius_partType_highPitchPulse, "uniform_ParticleCurve_gs_3fv_partRadius_partType_highPitchPulse", pg_shader_ParticleRender);
	bindAndCheckUniform(&uniform_ParticleCurve_vp_3fv_trackReplay_xy_height, "uniform_ParticleCurve_vp_3fv_trackReplay_xy_height", pg_shader_ParticleRender);

	// particle position/speed FBO
	bindAndCheckUniform(&uniform_ParticleCurve_texture_vp_Part_pos_speed, "uniform_ParticleCurve_texture_vp_Part_pos_speed", pg_shader_ParticleRender);
	// particle color/radius FBO
	bindAndCheckUniform(&uniform_ParticleCurve_texture_vp_Part_col_rad, "uniform_ParticleCurve_texture_vp_Part_col_rad", pg_shader_ParticleRender);
#endif

#ifdef CURVE_PARTICLES
	// color texture
	bindAndCheckUniform(&uniform_ParticleCurve_Comet_texture_fs_decal, "fs_comet", pg_shader_ParticleRender);
#endif
#if defined(var_part_initialization) 
	// color texture
	bindAndCheckUniform(&uniform_ParticleSplat_texture_fs_decal, "uniform_ParticleSplat_texture_fs_decal", pg_shader_ParticleRender);
#endif

	////////////////////////////////////////////////////////////////////////////////
	// MIXING SHADER parameters bindings
	////////////////////////////////////////////////////////////////////////////////
	bindAndCheckUniform(&uniform_Mixing_vp_model, "vp_modelMatrix", pg_shader_Mixing);
	bindAndCheckUniform(&uniform_Mixing_vp_view, "vp_viewMatrix", pg_shader_Mixing);
	bindAndCheckUniform(&uniform_Mixing_vp_proj, "vp_projMatrix", pg_shader_Mixing);

#if defined(var_flashPhotoTrkBeat) && defined(var_flashPhotoTrkBright) && defined(var_flashPhotoTrkLength) && defined(var_flashPhotoChangeBeat)
	bindAndCheckUniform(&uniform_Mixing_fs_3fv_height_flashCameraTrkWght_flashPhotoTrkWght, "uniform_Mixing_fs_3fv_height_flashCameraTrkWght_flashPhotoTrkWght", pg_shader_Mixing);
#else
	bindAndCheckUniform(&uniform_Mixing_fs_2fv_height_flashCameraTrkWght, "uniform_Mixing_fs_2fv_height_flashCameraTrkWght", pg_shader_Mixing);
#endif
	bindAndCheckUniform(&uniform_Mixing_fs_3fv_screenMsgTransp_Text1_2_Alpha, "uniform_Mixing_fs_3fv_screenMsgTransp_Text1_2_Alpha", pg_shader_Mixing);

#ifdef var_nb_CATypes
	bindAndCheckUniform(&uniform_Mixing_texture_fs_CA, "uniform_Mixing_texture_fs_CA", pg_shader_Mixing); // ping-pong CA (FBO)
#endif
#if defined(var_part_initialization)
	bindAndCheckUniform(&uniform_Mixing_texture_fs_Part_render, "uniform_Mixing_texture_fs_Part_render", pg_shader_Mixing);  // Particles (FBO)
#endif
	bindAndCheckUniform(&uniform_Mixing_texture_fs_Trk0, "uniform_Mixing_texture_fs_Trk0", pg_shader_Mixing);  // ping-pong track 0 (FBO)
#if PG_NB_TRACKS >= 2
	bindAndCheckUniform(&uniform_Mixing_texture_fs_Trk1, "uniform_Mixing_texture_fs_Trk1", pg_shader_Mixing);  // ping-pong track 1 (FBO)
#endif
#if PG_NB_TRACKS >= 3
	bindAndCheckUniform(&uniform_Mixing_texture_fs_Trk2, "uniform_Mixing_texture_fs_Trk2", pg_shader_Mixing);  // ping-pong track 2 (FBO)
#endif
#if PG_NB_TRACKS >= 4
	bindAndCheckUniform(&uniform_Mixing_texture_fs_Trk3, "uniform_Mixing_texture_fs_Trk3", pg_shader_Mixing);  // ping-pong track 3 (FBO)
#endif
#if defined(TVW)
	bindAndCheckUniform(&uniform_Mixing_texture_fs_Display_Font, "uniform_Mixing_texture_fs_Display_Font", pg_shader_Mixing);  // message Font
	bindAndCheckUniform(&uniform_Mixing_texture_fs_Display_Message1, "uniform_Mixing_texture_fs_Display_Message1", pg_shader_Mixing);  // tweets at the bottom of the screen
	bindAndCheckUniform(&uniform_Mixing_texture_fs_Display_Message2, "uniform_Mixing_texture_fs_Display_Message2", pg_shader_Mixing);  // tweets at the bottom of the screen
#endif

	bindAndCheckUniform(&uniform_Mixing_texture_fs_Render_prec, "uniform_Mixing_texture_fs_Render_prec", pg_shader_Mixing);  // preceding snapshot for echo
	bindAndCheckUniform(&uniform_Mixing_texture_fs_Screen_Font, "uniform_Mixing_texture_fs_Screen_Font", pg_shader_Mixing);  // message Font
	bindAndCheckUniform(&uniform_Mixing_texture_fs_Screen_Message, "uniform_Mixing_texture_fs_Screen_Message", pg_shader_Mixing);  // message string

	////////////////////////////////////////////////////////////////////////////////
	// MASTER SHADER parameters bindings
	////////////////////////////////////////////////////////////////////////////////
	bindAndCheckUniform(&uniform_Master_vp_model, "vp_modelMatrix", pg_shader_Master);
	bindAndCheckUniform(&uniform_Master_vp_view, "vp_viewMatrix", pg_shader_Master);
	bindAndCheckUniform(&uniform_Master_vp_proj, "vp_projMatrix", pg_shader_Master);
	bindAndCheckUniform(&uniform_Master_fs_4fv_xy_frameno_pulsedShift, "uniform_Master_fs_4fv_xy_frameno_pulsedShift", pg_shader_Master);
#if defined(TVW) || defined(var_alKemi)
	bindAndCheckUniform(&uniform_Master_fs_4fv_width_height_rightWindowVMargin_timeFromStart, "uniform_Master_fs_4fv_width_height_rightWindowVMargin_timeFromStart", pg_shader_Master);
#else
	bindAndCheckUniform(&uniform_Master_fs_4fv_width_height_timeFromStart_muteRightScreen, "uniform_Master_fs_4fv_width_height_timeFromStart_muteRightScreen", pg_shader_Master);
#endif
	//bindAndCheckUniform(&uniform_Master_fs_4fv_pulsedColor_rgb_pen_grey, "uniform_Master_fs_4fv_pulsedColor_rgb_pen_grey", pg_shader_Master);
	//bindAndCheckUniform(&uniform_Master_fs_4fv_interpolatedPaletteLow_rgb_currentScene, "uniform_Master_fs_4fv_interpolatedPaletteLow_rgb_currentScene", pg_shader_Master);
	//bindAndCheckUniform(&uniform_Master_fs_4fv_interpolatedPaletteMedium_rgb_mobile_cursor, "uniform_Master_fs_4fv_interpolatedPaletteMedium_rgb_mobile_cursor", pg_shader_Master);
	//bindAndCheckUniform(&uniform_Master_fs_3fv_interpolatedPaletteHigh_rgb, "uniform_Master_fs_3fv_interpolatedPaletteHigh_rgb", pg_shader_Master);
	bindAndCheckUniform(&uniform_Master_fs_2iv_mobile_cursor_currentScene, "uniform_Master_fs_2iv_mobile_cursor_currentScene", pg_shader_Master);
#ifdef var_Caverne_BackColor
	bindAndCheckUniform(&uniform_Master_fs_3fv_Caverne_BackColor_rgb, "uniform_Master_fs_3fv_Caverne_BackColor_rgb", pg_shader_Master);
#endif
#if defined(var_flashchange_BGcolor_freq)
	bindAndCheckUniform(&uniform_Master_fs_3fv_BG_color_rgb, "uniform_Master_fs_3fv_BG_color_rgb", pg_shader_Master);
#endif

	bindAndCheckUniform(&uniform_Master_texture_fs_Render_curr, "uniform_Master_texture_fs_Render_curr", pg_shader_Master); // previous pass output
#ifdef var_nb_CATypes
	bindAndCheckUniform(&uniform_Master_texture_fs_CA, "uniform_Master_texture_fs_CA", pg_shader_Master); // ping-pong CA (FBO)
#endif
	bindAndCheckUniform(&uniform_Master_texture_fs_Part_render, "uniform_Master_texture_fs_Part_render", pg_shader_Master); // Particles
	bindAndCheckUniform(&uniform_Master_texture_fs_Trk0, "uniform_Master_texture_fs_Trk0", pg_shader_Master); // ping-pong track 0 (FBO)
#if PG_NB_TRACKS >= 2
	bindAndCheckUniform(&uniform_Master_texture_fs_Trk1, "uniform_Master_texture_fs_Trk1", pg_shader_Master); // ping-pong track 1 (FBO)
#endif
#if PG_NB_TRACKS >= 3
	bindAndCheckUniform(&uniform_Master_texture_fs_Trk2, "uniform_Master_texture_fs_Trk2", pg_shader_Master); // ping-pong track 2 (FBO)
#endif
#if PG_NB_TRACKS >= 4
	bindAndCheckUniform(&uniform_Master_texture_fs_Trk3, "uniform_Master_texture_fs_Trk3", pg_shader_Master); // ping-pong track 3 (FBO)
#endif
#ifdef PG_WITH_MASTER_MASK
	bindAndCheckUniform(&uniform_Master_texture_fs_Mask, "uniform_Master_texture_fs_Mask", pg_shader_Master);  // mask for master output
#endif

#ifdef var_sensor_layout
	////////////////////////////////////////////////////////////////////////////////
	// SENSOR SHADER parameters bindings
	////////////////////////////////////////////////////////////////////////////////
	bindAndCheckUniform(&uniform_Sensor_vp_model, "vp_modelMatrix", pg_shader_Sensor);
	bindAndCheckUniform(&uniform_Sensor_vp_view, "vp_viewMatrix", pg_shader_Sensor);
	bindAndCheckUniform(&uniform_Sensor_vp_proj, "vp_projMatrix", pg_shader_Sensor);
	//bindAndCheckUniform(&uniform_Sensor_fs_2fv_frameno_invert, "uniform_Sensor_fs_2fv_frameno_invert", pg_shader_Sensor);
	bindAndCheckUniform(&uniform_Sensor_fs_4fv_onOff_isCurrentSensor_masterLevel_transparency, "uniform_Sensor_fs_4fv_onOff_isCurrentSensor_masterLevel_transparency", pg_shader_Sensor);

	bindAndCheckUniform(&uniform_Sensor_texture_fs_decal, "fs_decal", pg_shader_Sensor); // sensor texture
#endif

#if defined(var_activeMeshes)
	////////////////////////////////////////////////////////////////////////////////
	// MESH SHADER parameters bindings
	////////////////////////////////////////////////////////////////////////////////
	bindAndCheckUniform(&uniform_Mesh_vp_model, "vp_modelMatrix", pg_shader_Mesh);
	bindAndCheckUniform(&uniform_Mesh_vp_view, "vp_viewMatrix", pg_shader_Mesh);
	bindAndCheckUniform(&uniform_Mesh_vp_proj, "vp_projMatrix", pg_shader_Mesh);
	bindAndCheckUniform(&uniform_Mesh_fs_4fv_isDisplayLookAt_with_mesh_with_blue_currentScene, "uniform_Mesh_fs_4fv_isDisplayLookAt_with_mesh_with_blue_currentScene", pg_shader_Mesh);
	bindAndCheckUniform(&uniform_Mesh_fs_3fv_light, "uniform_Mesh_fs_3fv_light", pg_shader_Mesh);
#if defined(var_Contact_mesh_expand)
	bindAndCheckUniform(&uniform_Mesh_vp_2fv_dilate_explode, "uniform_Mesh_vp_2fv_dilate_explode", pg_shader_Mesh);
#endif
#if defined(var_Contact_mesh_anime)
	bindAndCheckUniform(&uniform_Mesh_bones_matrices, "uniform_Mesh_bones_matrices", pg_shader_Mesh);
#endif
#if defined(var_MmeShanghai_brokenGlass)
	bindAndCheckUniform(&uniform_Mesh_fs_4fv_color, "uniform_Mesh_fs_4fv_color", pg_shader_Mesh);
	bindAndCheckUniform(&uniform_Mesh_fs_4fv_color_master_photo_weight_bg, "uniform_Mesh_fs_4fv_color_master_photo_weight_bg", pg_shader_Mesh);
#endif
	// Augmented Reality: FBO capture of Master to be displayed on a mesh
#if defined(var_textureFrontier_wmin) && defined(var_textureFrontier_wmax) and defined(var_textureFrontier_hmin) && defined(var_textureFrontier_hmax) && defined(var_textureFrontier_wmin_width) && defined(var_textureFrontier_wmax_width) and defined(var_textureFrontier_hmin_width) && defined(var_textureFrontier_hmax_width) && defined(var_textureScale_w) && defined(var_textureScale_h) and defined(var_textureTranslate_w) && defined(var_textureTranslate_h) 
	bindAndCheckUniform(&uniform_Mesh_fs_4fv_textureFrontier, "uniform_Mesh_fs_4fv_textureFrontier", pg_shader_Mesh);
	bindAndCheckUniform(&uniform_Mesh_fs_4fv_textureFrontier_width, "uniform_Mesh_fs_4fv_textureFrontier_width", pg_shader_Mesh);
	bindAndCheckUniform(&uniform_Mesh_fs_4fv_textureScaleTransl, "uniform_Mesh_fs_4fv_textureScaleTransl", pg_shader_Mesh);
#endif
#if defined(var_Contact_mesh_palette)
	bindAndCheckUniform(&uniform_Mesh_fs_4fv_color_palette, "uniform_Mesh_fs_4fv_color_palette", pg_shader_Mesh); // mesh color
#endif
	bindAndCheckUniform(&uniform_Mesh_texture_fs_decal, "fs_Mesh_texture_fs_decal", pg_shader_Mesh); // previous pass output
#if defined(var_MmeShanghai_brokenGlass)
	bindAndCheckUniform(&uniform_Mesh_texture_fs_BG, "fs_Mesh_texture_fs_BG", pg_shader_Mesh); // previous pass output
#endif
#endif
}
