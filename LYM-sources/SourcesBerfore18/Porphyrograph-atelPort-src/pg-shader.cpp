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
unsigned int shader_programme[pg_NbShaderTotal];

/////////////////////////////////////////////////////////////////////////
// CAMERA SHADER
// CAMERA SHADER UNIFORM VARIABLES
//GLint uniform_Camera_vp_model;
//GLint uniform_Camera_vp_view;
//GLint uniform_Camera_vp_proj;
//GLint uniform_Camera_vp_2fv_width_height;
//GLint uniform_Camera_fs_4fv_W_H;
//
//// Camera SHADER TEXTURE IDS
//GLint uniform_Camera_texture_fs_decal;         // 2-cycle ping-pong BG track step n-1 (FBO)
//GLint uniform_Camera_texture_fs_lookupTable1;  // current background video frame

/////////////////////////////////////////////////////////////////////////
// UPDATE SHADER
// UPDATE SHADER UNIFORM VARIABLES
GLint uniform_Update_vp_model;
GLint uniform_Update_vp_view;
GLint uniform_Update_vp_proj;
GLint uniform_Update_vp_2fv_width_height;
GLint uniform_Update_fs_4fv_W_H_repopChannel_targetFrameNo;
GLint uniform_Update_fs_4fv_clearAllLayers_clearCA_pixelRadius_pulsedShift;
GLint uniform_Update_fs_4fv_flashTrkBGWghts_flashPartBGWght;
GLint uniform_Update_fs_4fv_trkDecay_partDecay;
GLint uniform_Update_fs_4fv_CAdecay_frameno_Cursor_flashPartCAWght;
GLint uniform_Update_fs_4fv_flashTrkCAWghts;
GLint uniform_Update_fs_4fv_flashTrkPartWghts;
#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7
GLint uniform_Update_fs_4fv_paths03_x;
GLint uniform_Update_fs_4fv_paths03_y;
GLint uniform_Update_fs_4fv_paths03_x_prev;
GLint uniform_Update_fs_4fv_paths03_y_prev;
GLint uniform_Update_fs_4fv_paths03_r;
GLint uniform_Update_fs_4fv_paths03_g;
GLint uniform_Update_fs_4fv_paths03_b;
GLint uniform_Update_fs_4fv_paths03_a;
GLint uniform_Update_fs_4fv_paths03_BrushID;
GLint uniform_Update_fs_4fv_paths03_RadiusX;
GLint uniform_Update_fs_4fv_paths03_RadiusY;
#endif
#if PG_NB_PATHS == 7
GLint uniform_Update_fs_4fv_paths47_x;
GLint uniform_Update_fs_4fv_paths47_y;
GLint uniform_Update_fs_4fv_paths47_x_prev;
GLint uniform_Update_fs_4fv_paths47_y_prev;
GLint uniform_Update_fs_4fv_paths47_r;
GLint uniform_Update_fs_4fv_paths47_g;
GLint uniform_Update_fs_4fv_paths47_b;
GLint uniform_Update_fs_4fv_paths47_a;
GLint uniform_Update_fs_4fv_paths47_BrushID;
GLint uniform_Update_fs_4fv_paths47_RadiusX;
GLint uniform_Update_fs_4fv_paths47_RadiusY;
#endif
GLint uniform_Update_fs_4fv_pulse;
GLint uniform_Update_fs_4fv_movieWH_flashCameraTrkWght_cpTrack;
GLint uniform_Update_fs_4fv_repop_Color_flashCABGWght;
GLint uniform_Update_fs_4fv_isClearLayer_flashPixel_flashCAPartWght_nbParticles;
GLint uniform_Update_fs_4fv_photo01_wh;
GLint uniform_Update_fs_4fv_photo01Wghts_Camera_W_H;
GLint uniform_Update_fs_4fv_repop_part_path_acc_damp_factor;
GLint uniform_Update_fs_4fv_xy_transl_tracks_0_1;

// UPDATE SHADER TEXTURE IDS
GLint uniform_Update_texture_fs_decal;         // ping-pong CA (FBO)
GLint uniform_Update_texture_fs_lookupTable1;  // 2-cycle ping-pong speed/position of pixels (FBO)
GLint uniform_Update_texture_fs_lookupTable2;  // 2-cycle ping-pong position/speed of particles (FBO)
GLint uniform_Update_texture_fs_lookupTable3;  // 2-cycle ping-pong color/radius of particles (FBO)
GLint uniform_Update_texture_fs_lookupTable4;  // 2-cycle ping-pong target position/color/radius of particles (FBO)
GLint uniform_Update_texture_fs_lookupTable5;  // ping-pong track 0 (FBO)
#if PG_NB_TRACKS >= 2
GLint uniform_Update_texture_fs_lookupTable6;  // ping-pong track 1 (FBO)
#endif
#if PG_NB_TRACKS >= 3
GLint uniform_Update_texture_fs_lookupTable7;  // ping-pong track 2 (FBO)
#endif
#if PG_NB_TRACKS >= 4
GLint uniform_Update_texture_fs_lookupTable8;  // ping-pong track 3 (FBO)
#endif
GLint uniform_Update_texture_fs_lookupTable8;  // pen patterns
#ifdef PG_VIDEO_ACTIVE
GLint uniform_Update_texture_fs_lookupTable9;  // camera frame
GLint uniform_Update_texture_fs_lookupTable10;  // camera BG capture
GLint uniform_Update_texture_fs_lookupTable11;  // movie frame
#endif
GLint uniform_Update_texture_fs_lookupTable12;  // 3D noise
GLint uniform_Update_texture_fs_lookupTable13;  // photo[0]
GLint uniform_Update_texture_fs_lookupTable14;  // photo[1]
GLint uniform_Update_texture_fs_lookupTable15;  // FBO capture of particle rendering used for flashing layers with particles
GLint uniform_Update_texture_fs_lookupTable16;  // particle initialization pairs of textures position/speed
GLint uniform_Update_texture_fs_lookupTable17;  // particle initialization pairs of textures color/radius

/////////////////////////////////////////////////////////////////////////
// PARTICLE SHADER
// PARTICLE SHADER UNIFORM VARIABLES
GLint uniform_Particle_vp_model;
GLint uniform_Particle_vp_view;
GLint uniform_Particle_vp_proj;
GLint uniform_ParticleSplat_gs_3fv_partRadius_partType_highPitchPulse;
GLint uniform_ParticleSplat_vp_2fv_trackReplay_xy;

// PARTICLE SHADER TEXTURE IDS
GLint uniform_Particle_texture_vp_decal;				 // Particle position/speed update
GLint uniform_Particle_texture_vp_lookupTable1;          // Particle color/radius update
#ifdef CURVE_PARTICLES
// color texture
GLint uniform_Particle_Comet_texture_fs_decal;  // comet texture
#endif
#ifdef BLURRED_SPLAT_PARTICLES
// color texture
GLint uniform_Particle_BlurredDisk_texture_fs_decal;  // blurred disk texture
#endif

/////////////////////////////////////////////////////////////////////////
// MIXING SHADER
// MIXING SHADER UNIFORM VARIABLES
GLint uniform_Mixing_vp_model;
GLint uniform_Mixing_vp_view;
GLint uniform_Mixing_vp_proj;
GLint uniform_Mixing_fs_4fv_pulsedShift_height_msgTrspcy_flashCameraTrkWght;

// MIXING SHADER TEXTURE IDS
GLint uniform_Mixing_texture_fs_decal;         // ping-pong CA (FBO)
GLint uniform_Mixing_texture_fs_lookupTable1;  // Particles (FBO)
GLint uniform_Mixing_texture_fs_lookupTable2;  // ping-pong track 0 (FBO)
#if PG_NB_TRACKS >= 2
GLint uniform_Mixing_texture_fs_lookupTable3;  // ping-pong track 1 (FBO)
#endif
#if PG_NB_TRACKS >= 3
GLint uniform_Mixing_texture_fs_lookupTable4;  // ping-pong track 2 (FBO)
#endif
#if PG_NB_TRACKS >= 4
GLint uniform_Mixing_texture_fs_lookupTable5;  // ping-pong track 3 (FBO)
#endif
GLint uniform_Mixing_texture_fs_lookupTable5;  // preceding snapshot
GLint uniform_Mixing_texture_fs_lookupTable_font;  // message Font
GLint uniform_Mixing_texture_fs_lookupTable_message;  // message string

/////////////////////////////////////////////////////////////////////////
// MASTER SHADER
// MASTER SHADER UNIFORM VARIABLES
GLint uniform_Master_vp_model;
GLint uniform_Master_vp_view;
GLint uniform_Master_vp_proj;
GLint uniform_Master_fs_4fv_xy_frameno_pulsedShift;
GLint uniform_Master_fs_3fv_width_height_mute_second_screen;

// MASTER SHADER TEXTURE IDS
GLint uniform_Master_texture_fs_decal;         // previous pass output
GLint uniform_Master_texture_fs_lookupTable1;  // ping-pong CA (FBO)
GLint uniform_Master_texture_fs_lookupTable2;  // Particles
GLint uniform_Master_texture_fs_lookupTable3;  // ping-pong track 0 (FBO)
#if PG_NB_TRACKS >= 2
GLint uniform_Master_texture_fs_lookupTable4;  // ping-pong track 1 (FBO)
#endif
#if PG_NB_TRACKS >= 3
GLint uniform_Master_texture_fs_lookupTable5;  // ping-pong track 2 (FBO)
#endif
#if PG_NB_TRACKS >= 4
GLint uniform_Master_texture_fs_lookupTable6;  // ping-pong track 3 (FBO)
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
			pg_loadshader((char *)("ProjectsBefore18/" + project_name + "/shaders/" + fileShaderName).c_str(), pg_shader);
			glCompileShader(pg_shader);
			pg_printShaderCompileLog(pg_shader);

			glAttachShader(shader_programme[ind_shader_file], pg_shader);
			printOglError(330);
		}

		glLinkProgram(shader_programme[ind_shader_file]);
		pg_printShaderLinkLog(shader_programme[ind_shader_file]);
		printOglError(331);

	}

#include "pg_shader_body_bind.cpp"
	 /*
	////////////////////////////////////////
	// shader parameeter bindings
	uniform_Camera_vp_model
		= glGetUniformLocation(shader_programme[pg_shader_Camera], "vp_modelMatrix");
	uniform_Camera_vp_view
		= glGetUniformLocation(shader_programme[pg_shader_Camera], "vp_viewMatrix");
	uniform_Camera_vp_proj
		= glGetUniformLocation(shader_programme[pg_shader_Camera], "vp_projMatrix");

	uniform_Camera_vp_2fv_width_height
		= glGetUniformLocation(shader_programme[pg_shader_Camera], "uniform_Camera_vp_2fv_width_height");
	uniform_Camera_fs_4fv_W_H
		= glGetUniformLocation(shader_programme[pg_shader_Camera], "uniform_Camera_fs_4fv_W_H");

	// video track FBO
	uniform_Camera_texture_fs_decal
		= glGetUniformLocation(shader_programme[pg_shader_Camera], "fs_decal");
	// video background
	uniform_Camera_texture_fs_lookupTable1
		= glGetUniformLocation(shader_programme[pg_shader_Camera], "fs_lookupTable1");  // current bakcground video frame

	if ((uniform_Camera_vp_model == -1)
		|| (uniform_Camera_vp_view == -1)
		|| (uniform_Camera_vp_proj == -1)
		|| (uniform_Camera_texture_fs_decal == -1)
		|| (uniform_Camera_texture_fs_lookupTable1 == -1)

		|| (uniform_Camera_fs_4fv_W_H == -1)
		|| (uniform_Camera_vp_2fv_width_height == -1)
		) {
		fprintf(stderr, "Could not bind uniforms Camera uniform_Camera_vp_model : %d, uniform_Camera_vp_view : %d, uniform_Camera_vp_proj : %d, uniform_Camera_texture_fs_decal : %d, uniform_Camera_texture_fs_lookupTable1 : %d, uniform_Camera_fs_4fv_W_H : %d, uniform_Camera_vp_2fv_width_height : %d\n", uniform_Camera_vp_model, uniform_Camera_vp_view, uniform_Camera_vp_proj, uniform_Camera_texture_fs_decal, uniform_Camera_texture_fs_lookupTable1, uniform_Camera_fs_4fv_W_H, uniform_Camera_vp_2fv_width_height);
	}
	*/

	////////////////////////////////////////
	// shader parameeter bindings
	uniform_Update_vp_model
		= glGetUniformLocation(shader_programme[pg_shader_Update], "vp_modelMatrix");
	uniform_Update_vp_view
		= glGetUniformLocation(shader_programme[pg_shader_Update], "vp_viewMatrix");
	uniform_Update_vp_proj
		= glGetUniformLocation(shader_programme[pg_shader_Update], "vp_projMatrix");

	uniform_Update_vp_2fv_width_height
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_vp_2fv_width_height");
	uniform_Update_fs_4fv_W_H_repopChannel_targetFrameNo
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_W_H_repopChannel_targetFrameNo");
	uniform_Update_fs_4fv_clearAllLayers_clearCA_pixelRadius_pulsedShift
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_clearAllLayers_clearCA_pixelRadius_pulsedShift");
	uniform_Update_fs_4fv_flashTrkBGWghts_flashPartBGWght
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_flashTrkBGWghts_flashPartBGWght");
	uniform_Update_fs_4fv_trkDecay_partDecay
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_trkDecay_partDecay");
	uniform_Update_fs_4fv_CAdecay_frameno_Cursor_flashPartCAWght
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_CAdecay_frameno_Cursor_flashPartCAWght");
	uniform_Update_fs_4fv_flashTrkCAWghts
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_flashTrkCAWghts");
	uniform_Update_fs_4fv_flashTrkPartWghts
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_flashTrkPartWghts");
#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7
	uniform_Update_fs_4fv_paths03_x
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_paths03_x");
	uniform_Update_fs_4fv_paths03_y
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_paths03_y");
	uniform_Update_fs_4fv_paths03_x_prev
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_paths03_x_prev");
	uniform_Update_fs_4fv_paths03_y_prev
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_paths03_y_prev");
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
	uniform_Update_fs_4fv_paths03_RadiusY
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_paths03_RadiusY");
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
	uniform_Update_fs_4fv_paths47_RadiusY
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_paths47_RadiusY");
#endif
	uniform_Update_fs_4fv_pulse
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_pulse");
	uniform_Update_fs_4fv_movieWH_flashCameraTrkWght_cpTrack
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_movieWH_flashCameraTrkWght_cpTrack");
	uniform_Update_fs_4fv_repop_Color_flashCABGWght
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_repop_Color_flashCABGWght");
	uniform_Update_fs_4fv_isClearLayer_flashPixel_flashCAPartWght_nbParticles
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_isClearLayer_flashPixel_flashCAPartWght_nbParticles");
	uniform_Update_fs_4fv_photo01_wh
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_photo01_wh");
	uniform_Update_fs_4fv_photo01Wghts_Camera_W_H
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_photo01Wghts_Camera_W_H");
	uniform_Update_fs_4fv_repop_part_path_acc_damp_factor
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_repop_part_path_acc_damp_factor");
	uniform_Update_fs_4fv_xy_transl_tracks_0_1
		= glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_xy_transl_tracks_0_1");

	// BG track FBO
	uniform_Update_texture_fs_decal
		= glGetUniformLocation(shader_programme[pg_shader_Update], "fs_decal");         // 2-cycle ping-pong CA (FBO)
	uniform_Update_texture_fs_lookupTable1
		= glGetUniformLocation(shader_programme[pg_shader_Update], "fs_lookupTable1");  // 2-cycle ping-pong speed/position of pixels (FBO)
	uniform_Update_texture_fs_lookupTable2
		= glGetUniformLocation(shader_programme[pg_shader_Update], "fs_lookupTable2");  // 2-cycle ping-pong position/speed of particles (FBO)
	uniform_Update_texture_fs_lookupTable3
		= glGetUniformLocation(shader_programme[pg_shader_Update], "fs_lookupTable3");  // 2-cycle ping-pong color/radius of particles (FBO)
	uniform_Update_texture_fs_lookupTable4
		= glGetUniformLocation(shader_programme[pg_shader_Update], "fs_lookupTable4");  // 2-cycle ping-pong target position/color/radius of particles (FBO)
	uniform_Update_texture_fs_lookupTable5
		= glGetUniformLocation(shader_programme[pg_shader_Update], "fs_lookupTable5");  // ping-pong track 0 (FBO)
#if PG_NB_TRACKS >= 2
	uniform_Update_texture_fs_lookupTable6
		= glGetUniformLocation(shader_programme[pg_shader_Update], "fs_lookupTable6");  // ping-pong track 1 (FBO)
#endif
#if PG_NB_TRACKS >= 3
	uniform_Update_texture_fs_lookupTable7
		= glGetUniformLocation(shader_programme[pg_shader_Update], "fs_lookupTable7");  // ping-pong track 2 (FBO)
#endif
#if PG_NB_TRACKS >= 4
	uniform_Update_texture_fs_lookupTable8
		= glGetUniformLocation(shader_programme[pg_shader_Update], "fs_lookupTable8");  // ping-pong track 3 (FBO)
#endif
  // textures
	uniform_Update_texture_fs_lookupTable8
		= glGetUniformLocation(shader_programme[pg_shader_Update], "fs_lookupTable8");  // pen patterns

#ifdef PG_VIDEO_ACTIVE
  // video frames
	uniform_Update_texture_fs_lookupTable9
		= glGetUniformLocation(shader_programme[pg_shader_Update], "fs_lookupTable9");  // camera frame
	uniform_Update_texture_fs_lookupTable10
		= glGetUniformLocation(shader_programme[pg_shader_Update], "fs_lookupTable10");  // current background frame
	uniform_Update_texture_fs_lookupTable11
		= glGetUniformLocation(shader_programme[pg_shader_Update], "fs_lookupTable11");  // movie frame
#endif
																			
	uniform_Update_texture_fs_lookupTable12
		= glGetUniformLocation(shader_programme[pg_shader_Update], "fs_lookupTable12");  // 3D noise

	// photo frames
	uniform_Update_texture_fs_lookupTable13
		= glGetUniformLocation(shader_programme[pg_shader_Update], "fs_lookupTable13");  // photo[0] frame
	uniform_Update_texture_fs_lookupTable14
		= glGetUniformLocation(shader_programme[pg_shader_Update], "fs_lookupTable14");  // photo[1] frame
	uniform_Update_texture_fs_lookupTable15
		= glGetUniformLocation(shader_programme[pg_shader_Update], "fs_lookupTable15");  // particle rendering capture
	uniform_Update_texture_fs_lookupTable16
		= glGetUniformLocation(shader_programme[pg_shader_Update], "fs_lookupTable16");  // particle initialization pairs of textures position/speed
	uniform_Update_texture_fs_lookupTable17
		= glGetUniformLocation(shader_programme[pg_shader_Update], "fs_lookupTable17");  // particle initialization pairs of textures color/radius

	if ((uniform_Update_vp_model == -1)
		|| (uniform_Update_vp_view == -1)
		|| (uniform_Update_vp_proj == -1)
		|| (uniform_Update_vp_2fv_width_height == -1)

		|| (uniform_Update_fs_4fv_W_H_repopChannel_targetFrameNo == -1)
		|| (uniform_Update_fs_4fv_clearAllLayers_clearCA_pixelRadius_pulsedShift == -1)
		|| (uniform_Update_fs_4fv_flashTrkBGWghts_flashPartBGWght == -1)
		|| (uniform_Update_fs_4fv_trkDecay_partDecay == -1)
		|| (uniform_Update_fs_4fv_CAdecay_frameno_Cursor_flashPartCAWght == -1)
		|| (uniform_Update_fs_4fv_flashTrkCAWghts == -1)
		|| (uniform_Update_fs_4fv_flashTrkPartWghts == -1)
#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7
		|| (uniform_Update_fs_4fv_paths03_x == -1)
		|| (uniform_Update_fs_4fv_paths03_y == -1)
		|| (uniform_Update_fs_4fv_paths03_x_prev == -1)
		|| (uniform_Update_fs_4fv_paths03_y_prev == -1)
		|| (uniform_Update_fs_4fv_paths03_r == -1)
		|| (uniform_Update_fs_4fv_paths03_g == -1)
		|| (uniform_Update_fs_4fv_paths03_b == -1)
		|| (uniform_Update_fs_4fv_paths03_a == -1)
		|| (uniform_Update_fs_4fv_paths03_BrushID == -1)
		|| (uniform_Update_fs_4fv_paths03_RadiusX == -1)
		|| (uniform_Update_fs_4fv_paths03_RadiusY == -1)
#endif
#if PG_NB_PATHS == 7
		|| (uniform_Update_fs_4fv_paths47_x == -1)
		|| (uniform_Update_fs_4fv_paths47_y == -1)
		|| (uniform_Update_fs_4fv_paths47_x_prev == -1)
		|| (uniform_Update_fs_4fv_paths47_y_prev == -1)
		|| (uniform_Update_fs_4fv_paths47_r == -1)
		|| (uniform_Update_fs_4fv_paths47_g == -1)
		|| (uniform_Update_fs_4fv_paths47_b == -1)
		|| (uniform_Update_fs_4fv_paths47_a == -1)
		|| (uniform_Update_fs_4fv_paths47_BrushID == -1)
		|| (uniform_Update_fs_4fv_paths47_RadiusX == -1)
		|| (uniform_Update_fs_4fv_paths47_RadiusY == -1)
#endif
		|| (uniform_Update_fs_4fv_pulse == -1)
		|| (uniform_Update_fs_4fv_movieWH_flashCameraTrkWght_cpTrack == -1)
		|| (uniform_Update_fs_4fv_repop_Color_flashCABGWght == -1)
		|| (uniform_Update_fs_4fv_isClearLayer_flashPixel_flashCAPartWght_nbParticles == -1)
		|| (uniform_Update_fs_4fv_photo01_wh == -1)
		|| (uniform_Update_fs_4fv_photo01Wghts_Camera_W_H == -1)
		|| (uniform_Update_fs_4fv_repop_part_path_acc_damp_factor == -1)
		|| (uniform_Update_texture_fs_decal == -1)
		|| (uniform_Update_fs_4fv_xy_transl_tracks_0_1 == -1)
		|| (uniform_Update_texture_fs_lookupTable1 == -1)
		|| (uniform_Update_texture_fs_lookupTable2 == -1)
		|| (uniform_Update_texture_fs_lookupTable3 == -1)
		|| (uniform_Update_texture_fs_lookupTable4 == -1)
		|| (uniform_Update_texture_fs_lookupTable5 == -1)
#if PG_NB_TRACKS >= 2
		|| (uniform_Update_texture_fs_lookupTable6 == -1)
#endif
#if PG_NB_TRACKS >= 3
		|| (uniform_Update_texture_fs_lookupTable7 == -1)
#endif
#if PG_NB_TRACKS >= 4
		|| (uniform_Update_texture_fs_lookupTable8 == -1)
#endif
		|| (uniform_Update_texture_fs_lookupTable8 == -1)
#ifdef PG_VIDEO_ACTIVE
		|| (uniform_Update_texture_fs_lookupTable9 == -1)
		|| (uniform_Update_texture_fs_lookupTable10 == -1)
		|| (uniform_Update_texture_fs_lookupTable11 == -1)
#endif
		|| (uniform_Update_texture_fs_lookupTable12 == -1)
		|| (uniform_Update_texture_fs_lookupTable13 == -1)
		|| (uniform_Update_texture_fs_lookupTable14 == -1)
		|| (uniform_Update_texture_fs_lookupTable15 == -1)
		|| (uniform_Update_texture_fs_lookupTable16 == -1)
		|| (uniform_Update_texture_fs_lookupTable17 == -1)
		) {
		fprintf(stderr, "Could not bind uniforms Update uniform_Update_vp_model : %d, uniform_Update_vp_view : %d, uniform_Update_vp_proj : %d, uniform_Update_vp_2fv_width_height : %d, uniform_Update_fs_4fv_W_H_repopChannel_targetFrameNo : %d, uniform_Update_fs_4fv_clearAllLayers_clearCA_pixelRadius_pulsedShift : %d, uniform_Update_fs_4fv_flashTrkBGWghts_flashPartBGWght : %d, uniform_Update_fs_4fv_trkDecay_partDecay : %d, uniform_Update_fs_4fv_CAdecay_frameno_Cursor_flashPartCAWght : %d, uniform_Update_fs_4fv_flashTrkCAWghts : %d, uniform_Update_fs_4fv_flashTrkPartWghts : %d, uniform_Update_texture_fs_decal : %d, uniform_Update_texture_fs_lookupTable1 : %d, uniform_Update_texture_fs_lookupTable2 : %d, uniform_Update_texture_fs_lookupTable6 : %d, uniform_Update_texture_fs_lookupTable9 : %d, uniform_Update_fs_4fv_pulse: %d, uniform_Update_fs_4fv_movieWH_flashCameraTrkWght_cpTrack: %d, uniform_Update_fs_4fv_repop_Color_flashCABGWght: %d, uniform_Update_fs_4fv_isClearLayer_flashPixel_flashCAPartWght_nbParticles: %d, uniform_Update_fs_4fv_photo01_wh: %d, uniform_Update_fs_4fv_repop_part_path_acc_damp_factor: %d, uniform_Update_fs_4fv_photo01Wghts_Camera_W_H: %d, uniform_Update_fs_4fv_xy_transl_tracks_0_1 %d\n", uniform_Update_vp_model, uniform_Update_vp_view, uniform_Update_vp_proj, uniform_Update_vp_2fv_width_height, uniform_Update_fs_4fv_W_H_repopChannel_targetFrameNo, uniform_Update_fs_4fv_clearAllLayers_clearCA_pixelRadius_pulsedShift, uniform_Update_fs_4fv_flashTrkBGWghts_flashPartBGWght, uniform_Update_fs_4fv_trkDecay_partDecay, uniform_Update_fs_4fv_CAdecay_frameno_Cursor_flashPartCAWght, uniform_Update_fs_4fv_flashTrkCAWghts, uniform_Update_fs_4fv_flashTrkPartWghts, uniform_Update_texture_fs_decal, uniform_Update_texture_fs_lookupTable1, uniform_Update_texture_fs_lookupTable2, uniform_Update_texture_fs_lookupTable6, uniform_Update_texture_fs_lookupTable9, uniform_Update_fs_4fv_pulse, uniform_Update_fs_4fv_movieWH_flashCameraTrkWght_cpTrack, uniform_Update_fs_4fv_repop_Color_flashCABGWght, uniform_Update_fs_4fv_isClearLayer_flashPixel_flashCAPartWght_nbParticles, uniform_Update_fs_4fv_photo01_wh, uniform_Update_fs_4fv_repop_part_path_acc_damp_factor, uniform_Update_fs_4fv_photo01Wghts_Camera_W_H, uniform_Update_fs_4fv_xy_transl_tracks_0_1);
#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7
		fprintf(stderr, "Could not bind uniforms uniform_Update_fs_4fv_paths03_x : %d, uniform_Update_fs_4fv_paths03_y : %d, uniform_Update_fs_4fv_paths03_x_prev : %d, uniform_Update_fs_4fv_paths03_y_prev : %d, uniform_Update_fs_4fv_paths03_r : %d, uniform_Update_fs_4fv_paths03_g : %d, uniform_Update_fs_4fv_paths03_b : %d, uniform_Update_fs_4fv_paths03_a : %d, uniform_Update_fs_4fv_paths03_BrushID : %d, uniform_Update_fs_4fv_paths03_RadiusX : %d, uniform_Update_fs_4fv_paths03_RadiusY : %d\n", uniform_Update_fs_4fv_paths03_x, uniform_Update_fs_4fv_paths03_y, uniform_Update_fs_4fv_paths03_x_prev, uniform_Update_fs_4fv_paths03_y_prev, uniform_Update_fs_4fv_paths03_r, uniform_Update_fs_4fv_paths03_g, uniform_Update_fs_4fv_paths03_b, uniform_Update_fs_4fv_paths03_a, uniform_Update_fs_4fv_paths03_BrushID, uniform_Update_fs_4fv_paths03_RadiusX, uniform_Update_fs_4fv_paths03_RadiusY);
#endif
#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7
		fprintf(stderr, "Could not bind uniforms uniform_Update_fs_4fv_paths47_x : %d, uniform_Update_fs_4fv_paths47_y : %d, uniform_Update_fs_4fv_paths47_x_prev : %d, uniform_Update_fs_4fv_paths47_y_prev : %d, uniform_Update_fs_4fv_paths47_r : %d, uniform_Update_fs_4fv_paths47_g : %d, uniform_Update_fs_4fv_paths47_b : %d, uniform_Update_fs_4fv_paths47_a : %d, uniform_Update_fs_4fv_paths47_BrushID : %d, uniform_Update_fs_4fv_paths47_RadiusX : %d, uniform_Update_fs_4fv_paths47_RadiusY : %d\n", uniform_Update_fs_4fv_paths47_x, uniform_Update_fs_4fv_paths47_y, uniform_Update_fs_4fv_paths47_x_prev, uniform_Update_fs_4fv_paths47_y_prev, uniform_Update_fs_4fv_paths47_r, uniform_Update_fs_4fv_paths47_g, uniform_Update_fs_4fv_paths47_b, uniform_Update_fs_4fv_paths47_a, uniform_Update_fs_4fv_paths47_BrushID, uniform_Update_fs_4fv_paths47_RadiusX, uniform_Update_fs_4fv_paths47_RadiusY);
#endif
		fprintf(stderr, "Could not bind uniforms uniform_Update_texture_fs_lookupTable3 : %d, uniform_Update_texture_fs_lookupTable4 : %d, uniform_Update_texture_fs_lookupTable5 : %d\n", uniform_Update_texture_fs_lookupTable3, uniform_Update_texture_fs_lookupTable4, uniform_Update_texture_fs_lookupTable5);
#if PG_NB_TRACKS >= 2
		fprintf(stderr, "Could not bind uniforms uniform_Update_texture_fs_lookupTable6 : %d\n", uniform_Update_texture_fs_lookupTable6);
#endif
#if PG_NB_TRACKS >= 3
		fprintf(stderr, "Could not bind uniforms uniform_Update_texture_fs_lookupTable7 : %d\n", uniform_Update_texture_fs_lookupTable7);
#endif
#if PG_NB_TRACKS >= 4
		fprintf(stderr, "Could not bind uniforms uniform_Update_texture_fs_lookupTable8 : %d\n", uniform_Update_texture_fs_lookupTable8);
#endif
#ifdef PG_VIDEO_ACTIVE
		fprintf(stderr, "Could not bind uniforms uniform_Update_texture_fs_lookupTable9 : %d, uniform_Update_texture_fs_lookupTable10 : %d, uniform_Update_texture_fs_lookupTable11 : %d\n",  uniform_Update_texture_fs_lookupTable9, uniform_Update_texture_fs_lookupTable10, uniform_Update_texture_fs_lookupTable11);
#endif
		fprintf(stderr, "Could not bind uniforms uniform_Update_texture_fs_lookupTable8 : %d, uniform_Update_texture_fs_lookupTable12 : %d, uniform_Update_texture_fs_lookupTable13 : %d, uniform_Update_texture_fs_lookupTable14 : %d, uniform_Update_texture_fs_lookupTable15 : %d, uniform_Update_texture_fs_lookupTable16 : %d, uniform_Update_texture_fs_lookupTable17 : %d\n", uniform_Update_texture_fs_lookupTable8, uniform_Update_texture_fs_lookupTable12, uniform_Update_texture_fs_lookupTable13, uniform_Update_texture_fs_lookupTable14, uniform_Update_texture_fs_lookupTable15, uniform_Update_texture_fs_lookupTable16, uniform_Update_texture_fs_lookupTable17);
	}

	////////////////////////////////////////
	// shader parameeter bindings
	uniform_Particle_vp_model
		= glGetUniformLocation(shader_programme[pg_shader_Particle], "vp_modelMatrix");
	uniform_Particle_vp_view
		= glGetUniformLocation(shader_programme[pg_shader_Particle], "vp_viewMatrix");
	uniform_Particle_vp_proj
		= glGetUniformLocation(shader_programme[pg_shader_Particle], "vp_projMatrix");

	uniform_ParticleSplat_gs_3fv_partRadius_partType_highPitchPulse
		= glGetUniformLocation(shader_programme[pg_shader_Particle], "uniform_ParticleSplat_gs_3fv_partRadius_partType_highPitchPulse");
	uniform_ParticleSplat_vp_2fv_trackReplay_xy
		= glGetUniformLocation(shader_programme[pg_shader_Particle], "uniform_ParticleSplat_vp_2fv_trackReplay_xy");

	// particle position/speed FBO
	uniform_Particle_texture_vp_decal
		= glGetUniformLocation(shader_programme[pg_shader_Particle], "vp_decal");
	// particle color/radius FBO
	uniform_Particle_texture_vp_lookupTable1
		= glGetUniformLocation(shader_programme[pg_shader_Particle], "vp_lookupTable1");

#ifdef CURVE_PARTICLES
	// color texture
	uniform_Particle_Comet_texture_fs_decal
		= glGetUniformLocation(shader_programme[pg_shader_Particle], "fs_comet");
#endif
#ifdef BLURRED_SPLAT_PARTICLES
	// color texture
	uniform_Particle_BlurredDisk_texture_fs_decal
		= glGetUniformLocation(shader_programme[pg_shader_Particle], "fs_blurredDisk");
#endif

	if ((uniform_Particle_vp_model == -1)
		|| (uniform_Particle_vp_view == -1)
		|| (uniform_Particle_vp_proj == -1)
		|| (uniform_ParticleSplat_gs_3fv_partRadius_partType_highPitchPulse == -1)
		|| (uniform_ParticleSplat_vp_2fv_trackReplay_xy == -1)
		|| (uniform_Particle_texture_vp_decal == -1)
		|| (uniform_Particle_texture_vp_lookupTable1 == -1)
#ifdef CURVE_PARTICLES
		|| (uniform_Particle_Comet_texture_fs_decal == -1)
#endif
#ifdef BLURRED_SPLAT_PARTICLES
		|| (uniform_Particle_BlurredDisk_texture_fs_decal == -1)
#endif
		) {
#ifdef CURVE_PARTICLES
		fprintf(stderr, "Could not bind uniforms Particle uniform_Particle_vp_model : %d, uniform_Particle_vp_view : %d, uniform_Particle_vp_proj : %d, uniform_ParticleSplat_gs_3fv_partRadius_partType_highPitchPulse : %d, uniform_ParticleSplat_vp_2fv_trackReplay_xy : %d, uniform_Particle_texture_vp_decal : %d, uniform_Particle_texture_vp_lookupTable1 : %d, uniform_Particle_Comet_texture_fs_decal : %d\n", uniform_Particle_vp_model, uniform_Particle_vp_view, uniform_Particle_vp_proj, uniform_ParticleSplat_gs_3fv_partRadius_partType_highPitchPulse, uniform_ParticleSplat_vp_2fv_trackReplay_xy, uniform_Particle_texture_vp_decal, uniform_Particle_texture_vp_lookupTable1, uniform_Particle_Comet_texture_fs_decal);
#endif
#ifdef BLURRED_SPLAT_PARTICLES
		fprintf(stderr, "Could not bind uniforms Particle uniform_Particle_vp_model : %d, uniform_Particle_vp_view : %d, uniform_Particle_vp_proj : %d, uniform_ParticleSplat_gs_3fv_partRadius_partType_highPitchPulse : %d, uniform_ParticleSplat_vp_2fv_trackReplay_xy : %d, uniform_Particle_texture_vp_decal : %d, uniform_Particle_texture_vp_lookupTable1 : %d, uniform_Particle_BlurredDisk_texture_fs_decal : %d\n", uniform_Particle_vp_model, uniform_Particle_vp_view, uniform_Particle_vp_proj, uniform_ParticleSplat_gs_3fv_partRadius_partType_highPitchPulse, uniform_ParticleSplat_vp_2fv_trackReplay_xy, uniform_Particle_texture_vp_decal, uniform_Particle_texture_vp_lookupTable1, uniform_Particle_BlurredDisk_texture_fs_decal);
#endif
	}

	////////////////////////////////////////
	// shader parameeter bindings
	uniform_Mixing_vp_model
		= glGetUniformLocation(shader_programme[pg_shader_Mixing], "vp_modelMatrix");
	uniform_Mixing_vp_view
		= glGetUniformLocation(shader_programme[pg_shader_Mixing], "vp_viewMatrix");
	uniform_Mixing_vp_proj
		= glGetUniformLocation(shader_programme[pg_shader_Mixing], "vp_projMatrix");

	uniform_Mixing_fs_4fv_pulsedShift_height_msgTrspcy_flashCameraTrkWght
		= glGetUniformLocation(shader_programme[pg_shader_Mixing], "uniform_Mixing_fs_4fv_pulsedShift_height_msgTrspcy_flashCameraTrkWght");

	uniform_Mixing_texture_fs_decal
		= glGetUniformLocation(shader_programme[pg_shader_Mixing], "fs_decal"); // ping-pong CA (FBO)
	uniform_Mixing_texture_fs_lookupTable1
		= glGetUniformLocation(shader_programme[pg_shader_Mixing], "fs_lookupTable1");  // Particles (FBO)
	uniform_Mixing_texture_fs_lookupTable2
		= glGetUniformLocation(shader_programme[pg_shader_Mixing], "fs_lookupTable2");  // ping-pong track 0 (FBO)
#if PG_NB_TRACKS >= 2
	uniform_Mixing_texture_fs_lookupTable3
		= glGetUniformLocation(shader_programme[pg_shader_Mixing], "fs_lookupTable3");  // ping-pong track 1 (FBO)
#endif
#if PG_NB_TRACKS >= 3
	uniform_Mixing_texture_fs_lookupTable4
		= glGetUniformLocation(shader_programme[pg_shader_Mixing], "fs_lookupTable4");  // ping-pong track 2 (FBO)
#endif
#if PG_NB_TRACKS >= 4
	uniform_Mixing_texture_fs_lookupTable5
		= glGetUniformLocation(shader_programme[pg_shader_Mixing], "fs_lookupTable5");  // ping-pong track 3 (FBO)
#endif
	uniform_Mixing_texture_fs_lookupTable5
		= glGetUniformLocation(shader_programme[pg_shader_Mixing], "fs_lookupTable5");  // preceding snapshot for echo
	uniform_Mixing_texture_fs_lookupTable_font
		= glGetUniformLocation(shader_programme[pg_shader_Mixing], "fs_lookupTable_font");  // message Font
	uniform_Mixing_texture_fs_lookupTable_message
		= glGetUniformLocation(shader_programme[pg_shader_Mixing], "fs_lookupTable_message");  // message string
	if ((uniform_Mixing_vp_proj == -1)
		|| (uniform_Mixing_vp_view == -1)
		|| (uniform_Mixing_vp_model == -1)
		|| (uniform_Mixing_fs_4fv_pulsedShift_height_msgTrspcy_flashCameraTrkWght == -1)
		|| (uniform_Mixing_texture_fs_decal == -1)
		|| (uniform_Mixing_texture_fs_lookupTable1 == -1)
		|| (uniform_Mixing_texture_fs_lookupTable2 == -1)
#if PG_NB_TRACKS >= 2
		|| (uniform_Mixing_texture_fs_lookupTable3 == -1)
#endif
#if PG_NB_TRACKS >= 3
		|| (uniform_Mixing_texture_fs_lookupTable4 == -1)
#endif
#if PG_NB_TRACKS >= 4
		|| (uniform_Mixing_texture_fs_lookupTable5 == -1)
#endif
		|| (uniform_Mixing_texture_fs_lookupTable5 == -1)
		|| (uniform_Mixing_texture_fs_lookupTable_font == -1)
		|| (uniform_Mixing_texture_fs_lookupTable_message == -1)
		) {
		fprintf(stderr, "Could not bind uniforms uniform_Mixing_texture_fs_lookupTable2 : %d\n", uniform_Mixing_texture_fs_lookupTable2);
#if PG_NB_TRACKS >= 2
		fprintf(stderr, "Could not bind uniforms uniform_Mixing_texture_fs_lookupTable3 : %d\n", uniform_Mixing_texture_fs_lookupTable3);
#endif
#if PG_NB_TRACKS >= 3
		fprintf(stderr, "Could not bind uniforms uniform_Mixing_texture_fs_lookupTable4 : %d\n", uniform_Mixing_texture_fs_lookupTable4);
#endif
#if PG_NB_TRACKS >= 4
		fprintf(stderr, "Could not bind uniforms uniform_Mixing_texture_fs_lookupTable5 : %d\n", uniform_Mixing_texture_fs_lookupTable5);
#endif
		fprintf(stderr, "Could not bind uniforms Mixing uniform_Mixing_vp_proj : %d, uniform_Mixing_vp_view : %d, uniform_Mixing_vp_model : %d, uniform_Mixing_fs_4fv_pulsedShift_height_msgTrspcy_flashCameraTrkWght : %d, uniform_Mixing_texture_fs_decal : %d, uniform_Mixing_texture_fs_lookupTable1 : %d, uniform_Mixing_texture_fs_lookupTable5 : %d, uniform_Mixing_texture_fs_lookupTable_font : %d, uniform_Mixing_texture_fs_lookupTable_message : %d\n", uniform_Mixing_vp_proj, uniform_Mixing_vp_view, uniform_Mixing_vp_model, uniform_Mixing_fs_4fv_pulsedShift_height_msgTrspcy_flashCameraTrkWght, uniform_Mixing_texture_fs_decal, uniform_Mixing_texture_fs_lookupTable1, uniform_Mixing_texture_fs_lookupTable5, uniform_Mixing_texture_fs_lookupTable_font, uniform_Mixing_texture_fs_lookupTable_message);
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
	uniform_Master_fs_3fv_width_height_mute_second_screen
		= glGetUniformLocation(shader_programme[pg_shader_Master], "uniform_Master_fs_3fv_width_height_mute_second_screen");

	uniform_Master_texture_fs_decal
		= glGetUniformLocation(shader_programme[pg_shader_Master], "fs_decal"); // previous pass output
	uniform_Master_texture_fs_lookupTable1
		= glGetUniformLocation(shader_programme[pg_shader_Master], "fs_lookupTable1"); // ping-pong CA (FBO)
	uniform_Master_texture_fs_lookupTable2
		= glGetUniformLocation(shader_programme[pg_shader_Master], "fs_lookupTable2"); // Particles
	uniform_Master_texture_fs_lookupTable3
		= glGetUniformLocation(shader_programme[pg_shader_Master], "fs_lookupTable3"); // ping-pong track 0 (FBO)
#if PG_NB_TRACKS >= 2
	uniform_Master_texture_fs_lookupTable4
		= glGetUniformLocation(shader_programme[pg_shader_Master], "fs_lookupTable4"); // ping-pong track 1 (FBO)
#endif
#if PG_NB_TRACKS >= 3
	uniform_Master_texture_fs_lookupTable5
		= glGetUniformLocation(shader_programme[pg_shader_Master], "fs_lookupTable5"); // ping-pong track 2 (FBO)
#endif
#if PG_NB_TRACKS >= 4
	uniform_Master_texture_fs_lookupTable6
		= glGetUniformLocation(shader_programme[pg_shader_Master], "fs_lookupTable6"); // ping-pong track 3 (FBO)
#endif

	if ((uniform_Master_vp_proj == -1)
		|| (uniform_Master_vp_view == -1)
		|| (uniform_Master_vp_model == -1)
		|| (uniform_Master_texture_fs_decal == -1)
		|| (uniform_Master_texture_fs_lookupTable1 == -1)
		|| (uniform_Master_texture_fs_lookupTable2 == -1)
		|| (uniform_Master_texture_fs_lookupTable3 == -1)
#if PG_NB_TRACKS >= 2
		|| (uniform_Master_texture_fs_lookupTable4 == -1)
#endif
#if PG_NB_TRACKS >= 3
		|| (uniform_Master_texture_fs_lookupTable5 == -1)
#endif
#if PG_NB_TRACKS >= 4
		|| (uniform_Master_texture_fs_lookupTable6 == -1)
#endif
		|| (uniform_Master_fs_4fv_xy_frameno_pulsedShift == -1)
		|| (uniform_Master_fs_3fv_width_height_mute_second_screen == -1)
		) {
		fprintf(stderr, "Could not bind uniforms Master uniform_Master_vp_proj : %d, uniform_Master_vp_view : %d, uniform_Master_vp_model : %d , uniform_Master_texture_fs_decal: %d, uniform_Master_texture_fs_lookupTable1: %d, uniform_Master_texture_fs_lookupTable2: %d, uniform_Master_fs_4fv_xy_frameno_pulsedShift : %d, uniform_Master_fs_3fv_width_height_mute_second_screen : %d\n", uniform_Master_vp_proj, uniform_Master_vp_view, uniform_Master_vp_model, uniform_Master_texture_fs_decal, uniform_Master_texture_fs_lookupTable1, uniform_Master_texture_fs_lookupTable2, uniform_Master_fs_4fv_xy_frameno_pulsedShift, uniform_Master_fs_3fv_width_height_mute_second_screen);
		fprintf(stderr, "Could not bind uniforms Master uniform_Master_texture_fs_lookupTable3: %d\n", uniform_Master_texture_fs_lookupTable3);
#if PG_NB_TRACKS >= 2
		fprintf(stderr, "Could not bind uniforms Master uniform_Master_texture_fs_lookupTable4: %d\n", uniform_Master_texture_fs_lookupTable4);
#endif
#if PG_NB_TRACKS >= 3
		fprintf(stderr, "Could not bind uniforms Master uniform_Master_texture_fs_lookupTable5: %d\n", uniform_Master_texture_fs_lookupTable5);
#endif
#if PG_NB_TRACKS >= 4
		fprintf(stderr, "Could not bind uniforms Master uniform_Master_texture_fs_lookupTable6: %d\n", uniform_Master_texture_fs_lookupTable6);
#endif
		// return 0;
	}
}
