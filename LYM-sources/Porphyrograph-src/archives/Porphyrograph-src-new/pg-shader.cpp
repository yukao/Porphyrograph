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
#if defined(CORE)
#include "pg_shader_body_decl_Core.cpp"
#endif
#if defined(pg_Project_Voluspa)
#include "pg_shader_body_decl_voluspa.cpp"
#endif
#if defined(pg_Project_araKnit)
#include "pg_shader_body_decl_araknit.cpp"
#endif

// #include "pg_scripts/pg_shader_body_decl.cpp"

/////////////////////////////////////////////////////////////////////////
// SHADER PROGRAMMES
unsigned int** pg_shader_programme;
unsigned int pg_current_configuration_rank = 0;
std::unordered_map<int, std::string> pg_stringShaderTypes = {
	{ _pg_shader_ParticleAnimation, "ParticleAnimation" },
	{ _pg_shader_Update, "Update" },
	{ _pg_shader_ParticleRender, "ParticleRender" },
	{ _pg_shader_Mixing, "Mixing" },
	{ _pg_shader_Master, "Master" },
	{ _pg_shader_Sensor, "Sensor" },
	{ _pg_shader_ClipArt, "ClipArt" },
	{ _pg_shader_Mesh, "Mesh" },
	{ _pg_shader_TotalNbTypes, "TotalNbTypes" },
};


// PARTICLE ANIMATION SHADER
// PARTICLE ANIMATION SHADER UNIFORM VARIABLES
GLint uniform_ParticleAnimation_vp_model[PG_MAX_CONFIGURATIONS] = {-1};
GLint uniform_ParticleAnimation_vp_view[PG_MAX_CONFIGURATIONS] = {-1};
GLint uniform_ParticleAnimation_vp_proj[PG_MAX_CONFIGURATIONS] = {-1};
GLint uniform_ParticleAnimation_vp_2fv_width_height[PG_MAX_CONFIGURATIONS] = {-1};
GLint uniform_ParticleAnimation_fs_4fv_W_H_repopChannel_targetFrameNo[PG_MAX_CONFIGURATIONS] = {-1};
GLint uniform_ParticleAnimation_fs_4fv_flashTrkPartWghts[PG_MAX_CONFIGURATIONS] = {-1};
GLint uniform_ParticleAnimation_path_data[PG_MAX_CONFIGURATIONS] = {-1};
GLint uniform_ParticleAnimation_fs_4fv_repop_Color_frameNo[PG_MAX_CONFIGURATIONS] = {-1};
GLint uniform_ParticleAnimation_fs_4fv_flashCAPartWght_nbPart_clear_partSizeUnpulsed[PG_MAX_CONFIGURATIONS] = {-1};
GLint uniform_ParticleAnimation_fs_4fv_Camera_W_H_movieWH[PG_MAX_CONFIGURATIONS] = {-1};

// UPDATE SHADER TEXTURE IDS
GLint uniform_ParticleAnimation_texture_fs_CA[PG_MAX_CONFIGURATIONS] = {-1};         // ping-pong CA (FBO)
GLint uniform_ParticleAnimation_texture_fs_Part_pos_speed[PG_MAX_CONFIGURATIONS] = {-1};  // 2-cycle ping-pong position/speed of particles (FBO)
GLint uniform_ParticleAnimation_texture_fs_Part_col_rad[PG_MAX_CONFIGURATIONS] = {-1};  // 2-cycle ping-pong color/radius of particles (FBO)
GLint uniform_ParticleAnimation_texture_fs_Part_Target_pos_col_rad[PG_MAX_CONFIGURATIONS] = {-1};  // 2-cycle ping-pong target position/color/radius of particles (FBO)
GLint uniform_ParticleAnimation_texture_fs_Camera_frame[PG_MAX_CONFIGURATIONS] = {-1};  // camera frame
GLint uniform_ParticleAnimation_texture_fs_Movie_frame[PG_MAX_CONFIGURATIONS] = {-1};  // movie frame
GLint uniform_ParticleAnimation_texture_fs_Noise[PG_MAX_CONFIGURATIONS] = {-1};  // 3D noise
GLint uniform_ParticleAnimation_texture_fs_RepopDensity[PG_MAX_CONFIGURATIONS] = {-1};  // repop density
GLint uniform_ParticleAnimation_texture_fs_Part_init_pos_speed[PG_MAX_CONFIGURATIONS] = {-1};  // particle initialization pairs of textures position/speed
GLint uniform_ParticleAnimation_texture_fs_Part_init_col_rad[PG_MAX_CONFIGURATIONS] = {-1};  // particle initialization pairs of textures color/radius
GLint uniform_ParticleAnimation_texture_fs_Part_acc[PG_MAX_CONFIGURATIONS] = {-1};  // particle acceleration texture
GLint uniform_ParticleAnimation_texture_fs_Trk0[PG_MAX_CONFIGURATIONS] = {-1};  // ping-pong track 0 (FBO)
GLint uniform_ParticleAnimation_texture_fs_Trk1[PG_MAX_CONFIGURATIONS] = {-1};  // ping-pong track 1 (FBO)
GLint uniform_ParticleAnimation_texture_fs_Trk2[PG_MAX_CONFIGURATIONS] = {-1};  // ping-pong track 2 (FBO)
GLint uniform_ParticleAnimation_texture_fs_Trk3[PG_MAX_CONFIGURATIONS] = {-1};  // ping-pong track 3 (FBO)

/////////////////////////////////////////////////////////////////////////
// UPDATE SHADER
// UPDATE SHADER UNIFORM VARIABLES
GLint uniform_Update_vp_model[PG_MAX_CONFIGURATIONS] = {-1};
GLint uniform_Update_vp_view[PG_MAX_CONFIGURATIONS] = {-1};
GLint uniform_Update_vp_proj[PG_MAX_CONFIGURATIONS] = {-1};
GLint uniform_Update_homographyForTexture[PG_MAX_CONFIGURATIONS] = {-1};
GLint uniform_Update_vp_2fv_width_height[PG_MAX_CONFIGURATIONS] = {-1};
GLint uniform_Update_fs_4fv_W_H_time_currentScene[PG_MAX_CONFIGURATIONS] = {-1};
GLint uniform_Update_fs_3fv_clearAllLayers_clearCA_pulsedShift[PG_MAX_CONFIGURATIONS] = {-1};
#if defined(pg_Project_CAaudio)
GLint uniform_Update_fs_4fv_CAseed_type_size_loc[PG_MAX_CONFIGURATIONS] = {-1};
#endif
GLint uniform_Update_fs_4fv_flashTrkBGWghts_flashPartBGWght[PG_MAX_CONFIGURATIONS] = {-1};
GLint uniform_Update_fs_4fv_frameno_Cursor_flashPartCAWght_doubleWindow[PG_MAX_CONFIGURATIONS] = {-1};
GLint uniform_Update_fs_4fv_flashTrkCAWghts[PG_MAX_CONFIGURATIONS] = {-1};
GLint uniform_Update_path_data[PG_MAX_CONFIGURATIONS] = {-1};

// pg_Project_Criton PROJECT
GLint uniform_Update_fs_4fv_fftLevels03[PG_MAX_CONFIGURATIONS] = {-1};
GLint uniform_Update_fs_4fv_fftFrequencies03[PG_MAX_CONFIGURATIONS] = {-1};
GLint uniform_Update_fs_4fv_fftPhases03[PG_MAX_CONFIGURATIONS] = {-1};
GLint uniform_Update_fs_4fv_fftLevels47[PG_MAX_CONFIGURATIONS] = {-1};
GLint uniform_Update_fs_4fv_fftFrequencies47[PG_MAX_CONFIGURATIONS] = {-1};
GLint uniform_Update_fs_4fv_fftPhases47[PG_MAX_CONFIGURATIONS] = {-1};

GLint uniform_Update_fs_4fv_movieWH_flashCameraTrkWght_cpTrack[PG_MAX_CONFIGURATIONS] = {-1};
GLint uniform_Update_fs_4fv_repop_ColorBG_flashCABGWght[PG_MAX_CONFIGURATIONS] = {-1};
GLint uniform_Update_fs_3fv_repop_ColorCA[PG_MAX_CONFIGURATIONS] = {-1};
GLint uniform_Update_fs_3fv_isClearLayer_flashPixel_flashCameraTrkThres[PG_MAX_CONFIGURATIONS] = {-1};
#if defined(var_flashPhotoTrkBeat) && defined(var_flashPhotoTrkBright) && defined(var_flashPhotoTrkLength) && defined(var_flashPhotoChangeBeat)
GLint uniform_Update_fs_4fv_flashPhotoTrkWght_flashPhotoTrkThres_Photo_offSetsXY[PG_MAX_CONFIGURATIONS] = {-1};
#endif
GLint uniform_Update_fs_4fv_photo01_wh[PG_MAX_CONFIGURATIONS] = {-1};
GLint uniform_Update_fs_4fv_photo01Wghts_randomValues[PG_MAX_CONFIGURATIONS] = {-1};
#if defined(var_clipCaptFreq) && PG_NB_PARALLEL_CLIPS >= 2
GLint uniform_Update_fs_2fv_clip01Wghts[PG_MAX_CONFIGURATIONS] = {-1};
#endif
GLint uniform_Update_fs_4fv_Camera_offSetsXY_Camera_W_H[PG_MAX_CONFIGURATIONS] = {-1};
GLint uniform_Update_fs_4fv_xy_transl_tracks_0_1[PG_MAX_CONFIGURATIONS] = {-1};
GLint uniform_Update_fs_4fv_CAType_SubType_blurRadius[PG_MAX_CONFIGURATIONS] = {-1};
GLint uniform_Update_fs_3fv_photo_rgb[PG_MAX_CONFIGURATIONS] = {-1};
GLint uniform_Update_texture_fs_CATable[PG_MAX_CONFIGURATIONS] = {-1};
#if defined(var_GenerativeNights_planes)
GLint uniform_Update_fs_2fv_initCA_1stPlaneFrameNo[PG_MAX_CONFIGURATIONS] = {-1};
#endif
GLint uniform_Update_texture_fs_Camera_BGIni[PG_MAX_CONFIGURATIONS] = {-1};  // initial camera BG capture
GLint uniform_Update_texture_fs_pixel_acc[PG_MAX_CONFIGURATIONS] = {-1};

// UPDATE SHADER TEXTURE IDS
GLint uniform_Update_texture_fs_CA[PG_MAX_CONFIGURATIONS] = {-1};         // ping-pong CA (FBO)
GLint uniform_Update_texture_fs_PreviousCA[PG_MAX_CONFIGURATIONS] = {-1};         // ping-pong CA (FBO)
GLint uniform_Update_texture_fs_Pixels[PG_MAX_CONFIGURATIONS] = {-1};  // 2-cycle ping-pong speed/position of pixels (FBO)
GLint uniform_Update_texture_fs_Trk0[PG_MAX_CONFIGURATIONS] = {-1};  // ping-pong track 0 (FBO)
GLint uniform_Update_texture_fs_Trk1[PG_MAX_CONFIGURATIONS] = {-1};  // ping-pong track 1 (FBO)
GLint uniform_Update_texture_fs_Trk2[PG_MAX_CONFIGURATIONS] = {-1};  // ping-pong track 2 (FBO)
GLint uniform_Update_texture_fs_Trk3[PG_MAX_CONFIGURATIONS] = {-1};  // ping-pong track 3 (FBO)
GLint uniform_Update_texture_fs_Brushes[PG_MAX_CONFIGURATIONS] = {-1};  // pen patterns
GLint uniform_Update_texture_fs_Camera_frame[PG_MAX_CONFIGURATIONS] = {-1};  // camera frame
GLint uniform_Update_texture_fs_Camera_BG[PG_MAX_CONFIGURATIONS] = {-1};  // current camera BG capture
GLint uniform_Update_texture_fs_Movie_frame[PG_MAX_CONFIGURATIONS] = {-1};  // movie frame
GLint uniform_Update_texture_fs_Noise[PG_MAX_CONFIGURATIONS] = {-1};  // 3D noise
GLint uniform_Update_texture_fs_RepopDensity[PG_MAX_CONFIGURATIONS] = {-1};  // repop density
GLint uniform_Update_texture_fs_Photo0[PG_MAX_CONFIGURATIONS] = {-1};  // photo[0]
GLint uniform_Update_texture_fs_Photo1[PG_MAX_CONFIGURATIONS] = {-1};  // photo[1]
GLint uniform_Update_texture_fs_Clip0[PG_MAX_CONFIGURATIONS] = {-1};  // clip[0]
GLint uniform_Update_texture_fs_Clip1[PG_MAX_CONFIGURATIONS] = {-1};  // clip[1]
GLint uniform_Update_texture_fs_Part_render[PG_MAX_CONFIGURATIONS] = {-1};  // FBO capture of particle rendering used for flashing layers with particles

/////////////////////////////////////////////////////////////////////////
// PARTICLE SHADER
// PARTICLE SHADER UNIFORM VARIABLES
GLint uniform_ParticleSplat_vp_model[PG_MAX_CONFIGURATIONS] = {-1};
GLint uniform_ParticleSplat_vp_view[PG_MAX_CONFIGURATIONS] = {-1};
GLint uniform_ParticleSplat_vp_proj[PG_MAX_CONFIGURATIONS] = {-1};
GLint uniform_ParticleSplat_gs_4fv_part_size_partType_highPitchPulse_windowRatio[PG_MAX_CONFIGURATIONS] = {-1};
GLint uniform_ParticleSplat_vp_3fv_trackReplay_xy_height[PG_MAX_CONFIGURATIONS] = {-1};

// PARTICLE SHADER TEXTURE IDS
GLint uniform_ParticleSplat_texture_vp_Part_pos_speed[PG_MAX_CONFIGURATIONS] = {-1};				 // Particle position/speed update
GLint uniform_ParticleSplat_texture_vp_Part_col_rad[PG_MAX_CONFIGURATIONS] = {-1};          // Particle color/radius update

// CURVE PARTICLES
GLint uniform_ParticleCurve_vp_model[PG_MAX_CONFIGURATIONS] = {-1};
GLint uniform_ParticleCurve_vp_view[PG_MAX_CONFIGURATIONS] = {-1};
GLint uniform_ParticleCurve_vp_proj[PG_MAX_CONFIGURATIONS] = {-1};
GLint uniform_ParticleCurve_gs_3fv_partRadius_partType_highPitchPulse[PG_MAX_CONFIGURATIONS] = {-1};
GLint uniform_ParticleCurve_vp_3fv_trackReplay_xy_height[PG_MAX_CONFIGURATIONS] = {-1};

// PARTICLE SHADER TEXTURE IDS
GLint uniform_ParticleCurve_texture_vp_Part_pos_speed[PG_MAX_CONFIGURATIONS] = {-1};				 // Particle position/speed update
GLint uniform_ParticleCurve_texture_vp_Part_col_rad[PG_MAX_CONFIGURATIONS] = {-1};          // Particle color/radius update

// color texture
GLint uniform_ParticleCurve_Curve_texture_fs_decal[PG_MAX_CONFIGURATIONS] = {-1};  // comet texture

// color texture
GLint uniform_ParticleSplat_texture_fs_decal[PG_MAX_CONFIGURATIONS] = {-1};  // blurred disk texture

/////////////////////////////////////////////////////////////////////////
// MIXING SHADER
// MIXING SHADER UNIFORM VARIABLES
GLint uniform_Mixing_vp_model[PG_MAX_CONFIGURATIONS] = {-1};
GLint uniform_Mixing_vp_view[PG_MAX_CONFIGURATIONS] = {-1};
GLint uniform_Mixing_vp_proj[PG_MAX_CONFIGURATIONS] = {-1};
GLint uniform_Mixing_fs_3fv_height_flashCameraTrkWght_flashPhotoTrkWght[PG_MAX_CONFIGURATIONS] = {-1};
GLint uniform_Mixing_fs_3fv_screenMsgTransp_Text1_2_Alpha[PG_MAX_CONFIGURATIONS] = {-1};

// MIXING SHADER TEXTURE IDS
GLint uniform_Mixing_texture_fs_CA[PG_MAX_CONFIGURATIONS] = {-1};         // ping-pong CA (FBO)
GLint uniform_Mixing_texture_fs_ClipArt_render[PG_MAX_CONFIGURATIONS] = { -1 };  // ClipArt (FBO)
GLint uniform_Mixing_texture_fs_Particle_render[PG_MAX_CONFIGURATIONS] = { -1 };  // Particles (FBO)
GLint uniform_Mixing_texture_fs_Trk0[PG_MAX_CONFIGURATIONS] = {-1};  // ping-pong track 0 (FBO)
GLint uniform_Mixing_texture_fs_Trk1[PG_MAX_CONFIGURATIONS] = {-1};  // ping-pong track 1 (FBO)
GLint uniform_Mixing_texture_fs_Trk2[PG_MAX_CONFIGURATIONS] = {-1};  // ping-pong track 2 (FBO)
GLint uniform_Mixing_texture_fs_Trk3[PG_MAX_CONFIGURATIONS] = {-1};  // ping-pong track 3 (FBO)
GLint uniform_Mixing_texture_fs_Render_prec[PG_MAX_CONFIGURATIONS] = {-1};  // preceding snapshot
GLint uniform_Mixing_texture_fs_Screen_Font[PG_MAX_CONFIGURATIONS] = {-1};  // message Font
GLint uniform_Mixing_texture_fs_Screen_Message[PG_MAX_CONFIGURATIONS] = {-1};  // message string

/////////////////////////////////////////////////////////////////////////
// MASTER SHADER
// MASTER SHADER UNIFORM VARIABLES
GLint uniform_Master_vp_model[PG_MAX_CONFIGURATIONS] = {-1};
GLint uniform_Master_vp_view[PG_MAX_CONFIGURATIONS] = {-1};
GLint uniform_Master_vp_proj[PG_MAX_CONFIGURATIONS] = {-1};
GLint uniform_Master_fs_4fv_xy_frameno_pulsedShift[PG_MAX_CONFIGURATIONS] = {-1};
GLint uniform_Master_fs_4fv_width_height_timeFromStart_muteRightScreen[PG_MAX_CONFIGURATIONS] = {-1};
//GLint uniform_Master_fs_4fv_pulsedColor_rgb_pen_grey[PG_MAX_CONFIGURATIONS] = {-1};
//GLint uniform_Master_fs_4fv_interpolatedPaletteLow_rgb_currentScene[PG_MAX_CONFIGURATIONS] = {-1};
//GLint uniform_Master_fs_4fv_interpolatedPaletteMedium_rgb_mobile_cursor[PG_MAX_CONFIGURATIONS] = {-1};
//GLint uniform_Master_fs_3fv_interpolatedPaletteHigh_rgb[PG_MAX_CONFIGURATIONS] = {-1};
GLint uniform_Master_fs_2iv_mobile_cursor_currentScene[PG_MAX_CONFIGURATIONS] = {-1};
GLint uniform_Master_fs_3fv_BGcolor_rgb[PG_MAX_CONFIGURATIONS] = {-1};


// MASTER SHADER TEXTURE IDS
GLint uniform_Master_texture_fs_Render_curr[PG_MAX_CONFIGURATIONS] = {-1};         // previous pass output
GLint uniform_Master_texture_fs_CA[PG_MAX_CONFIGURATIONS] = {-1};  // ping-pong CA (FBO)
GLint uniform_Master_texture_fs_ClipArt_render[PG_MAX_CONFIGURATIONS] = { -1 };  // Particles
GLint uniform_Master_texture_fs_Particle_render[PG_MAX_CONFIGURATIONS] = { -1 };  // Particles
GLint uniform_Master_texture_fs_Trk0[PG_MAX_CONFIGURATIONS] = {-1};  // ping-pong track 0 (FBO)
GLint uniform_Master_texture_fs_Trk1[PG_MAX_CONFIGURATIONS] = {-1};  // ping-pong track 1 (FBO)
GLint uniform_Master_texture_fs_Trk2[PG_MAX_CONFIGURATIONS] = {-1};  // ping-pong track 2 (FBO)
GLint uniform_Master_texture_fs_Trk3[PG_MAX_CONFIGURATIONS] = {-1};  // ping-pong track 3 (FBO)
GLint uniform_Master_texture_fs_Mask[PG_MAX_CONFIGURATIONS] = {-1};  // mask for master output

/////////////////////////////////////////////////////////////////////////
// SENSOR SHADER
// SENSOR SHADER UNIFORM VARIABLES
GLint uniform_Sensor_vp_model[PG_MAX_CONFIGURATIONS] = {-1};
GLint uniform_Sensor_vp_view[PG_MAX_CONFIGURATIONS] = {-1};
GLint uniform_Sensor_vp_proj[PG_MAX_CONFIGURATIONS] = {-1};

// SENSOR SHADER TEXTURE IDS
GLint uniform_Sensor_texture_fs_decal[PG_MAX_CONFIGURATIONS] = {-1};         // sensor texture
GLint uniform_Sensor_fs_4fv_onOff_isCurrentSensor_masterLevel_transparency[PG_MAX_CONFIGURATIONS] = {-1};

/////////////////////////////////////////////////////////////////////////
// MESH SHADER
// MESH SHADER UNIFORM VARIABLES
GLint uniform_Mesh_vp_model[PG_MAX_CONFIGURATIONS] = {-1};
GLint uniform_Mesh_vp_view[PG_MAX_CONFIGURATIONS] = {-1};
GLint uniform_Mesh_vp_proj[PG_MAX_CONFIGURATIONS] = {-1};
GLint uniform_Mesh_fs_4fv_isDisplayLookAt_with_mesh_with_blue_currentScene[PG_MAX_CONFIGURATIONS] = {-1};
GLint uniform_Mesh_fs_3fv_light[PG_MAX_CONFIGURATIONS] = {-1};
#if defined(var_Contact_mesh_expand)
GLint uniform_Mesh_vp_2fv_dilate_explode[PG_MAX_CONFIGURATIONS] = {-1};
#endif
#if defined(var_Contact_mesh_anime)
GLint uniform_Mesh_bones_matrices[PG_MAX_CONFIGURATIONS] = {-1};
#endif
#if defined(var_MmeShanghai_brokenGlass)
GLint uniform_Mesh_fs_4fv_color[PG_MAX_CONFIGURATIONS] = {-1};
GLint uniform_Mesh_fs_4fv_color_master_photo_weight_bg[PG_MAX_CONFIGURATIONS] = {-1};
#endif
// Augmented Reality: FBO capture of Master to be displayed on a mesh
GLint uniform_Mesh_fs_4fv_textureFrontier[PG_MAX_CONFIGURATIONS] = {-1};
GLint uniform_Mesh_fs_4fv_textureFrontier_width[PG_MAX_CONFIGURATIONS] = {-1};
GLint uniform_Mesh_fs_4fv_textureScaleTransl[PG_MAX_CONFIGURATIONS] = {-1};
// Mesh SHADER TEXTURE IDS
GLint uniform_Mesh_texture_fs_decal[PG_MAX_CONFIGURATIONS] = {-1};         // Mesh texture
#if defined(var_Contact_mesh_palette)
GLint uniform_Mesh_fs_4fv_color_palette[PG_MAX_CONFIGURATIONS] = {-1};         // Mesh color
#endif
#if defined(var_MmeShanghai_brokenGlass)
GLint uniform_Mesh_texture_fs_BG[PG_MAX_CONFIGURATIONS] = {-1};         // Mesh texture
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

void pg_printShaderCompileLog(GLuint obj) {
	int infologLength = 0;
	int maxLength;

	if (glIsShader(obj)) {
		glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &maxLength);
	}
	else {
		glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &maxLength);
	}

	char *infoLog = new char[maxLength];

	if (glIsShader(obj)) {
		glGetShaderInfoLog(obj, maxLength, &infologLength, infoLog);
	}
	else {
		glGetProgramInfoLog(obj, maxLength, &infologLength, infoLog);
	}

	if (infologLength > 0) {
		printf("Shader compilation failed:\n%s\n", infoLog);  throw 4301;
	}

	delete[] infoLog;
}

void pg_printShaderLinkLog(GLuint obj) {
	int infologLength = 0;
	GLint linked;
	if (glIsShader(obj)) {
		glGetShaderiv(obj, GL_LINK_STATUS, &linked);
	}
	else {
		glGetProgramiv(obj, GL_LINK_STATUS, &linked);
	}

	GLsizei maxLength;
	if (glIsShader(obj)) {
		glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &maxLength);
	}
	else {
		glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &maxLength);
	}

	GLchar* infoLog = new GLchar[maxLength + 1];
	glGetProgramInfoLog(obj, maxLength, &maxLength, infoLog);
	if (infologLength > 0) {
		printf("Shader linking failed:\n%s\n", infoLog); throw 4302;
	}

	delete[] infoLog;
}



/////////////////////////////////////////////////////////////
// SHADER LOADING
int pg_loadshader(string filename, GLuint shader) {
	GLchar* shaderSource;
	unsigned long len;

	ifstream file;
	file.open(filename.c_str(), ios::in); // opens as ASCII!
	if (!file) {
		printf("Error: shader file not found %s!\n", filename.c_str()); throw 53;
	}

	len = pg_getFileLength(file);
	if (len == 0) {
		printf("Error: empty shader file %s!\n", filename.c_str()); throw 53;
	}

	shaderSource = new char[len + 1];
	if (shaderSource == 0) {
		printf("Error: shader string allocation error %s!\n", filename.c_str()); throw 53;
	}

	// len isn't always strlen cause some characters are stripped in ascii read...
	// it is important to 0-terminate the real length later, len is just max possible value... 
	shaderSource[len] = 0;

	unsigned int i = 0;
	while (file.good()) {
		shaderSource[i] = GLchar(file.get());       // get character from file.
		if (!file.eof())
			i++;
	}

	shaderSource[i] = 0;  // 0-terminate it at the correct position

	file.close();

	glShaderSource(shader, 1, (const char**)&shaderSource, NULL);

	delete[] shaderSource;

	return 0; // No Error
}

void pg_allocateBindAndCheckUniform(int configuration_rank, GLint uniformID[PG_MAX_CONFIGURATIONS], string uniformString, ShaderFileTypes shaderType) {
	if (configuration_rank < pg_NbConfigurations) {
		if (pg_shader_programme[configuration_rank][shaderType]) {
			glUseProgram(pg_shader_programme[configuration_rank][shaderType]);
			uniformID[configuration_rank] = glGetUniformLocation(pg_shader_programme[configuration_rank][shaderType], uniformString.c_str());
		}
		else {
			sprintf(ErrorStr, "Error: inactive shader [%s] configuration %d for binding %s file %s!", 
				pg_stringShaderTypes[shaderType].c_str(), configuration_rank, uniformString.c_str(), 
				pg_Shader_File_Names[configuration_rank][shaderType].c_str()); ReportError(ErrorStr); throw 430;
		}
	}
	else {
		sprintf(ErrorStr, "Error: unknown shader type [%s]!", pg_stringShaderTypes[shaderType].c_str()); ReportError(ErrorStr); throw 430;
	}
	printOglError(332);
	if (uniformID[configuration_rank] == -1) {
		std::cout << uniformString << " (" << pg_stringShaderTypes[shaderType] << ") ";
		//sprintf(ErrorStr, "Error: Could not bind uniform [%s] (ID: %d) in shader type %s configuration #%d shader name %s!", uniformString.c_str(), int(uniformID[configuration_rank]), pg_stringShaderTypes[shaderType].c_str(), configuration_rank, pg_Shader_File_Names[configuration_rank][shaderType].c_str()); ReportError(ErrorStr);
	}
}

void pg_loadAllShaders(void) {
	std::cout << "\nLoading shaders: " << std::endl;
	////////////////////////////////////////
	// loading and compiling shaders
	for (int indConfig = 0; indConfig < pg_NbConfigurations; indConfig++) {
		std::cout << "   " << indConfig << ": ";
		for (int shader_type = 0; shader_type < _pg_shader_TotalNbTypes; shader_type++) {
			if (shader_type == _pg_shader_Sensor && pg_FullScenarioActiveVars[indConfig][_sensor_layout]
				&& pg_Shader_File_Names[indConfig][_pg_shader_Sensor] == "NULL") {
				sprintf(ErrorStr, "Error: Expected [%s] shader in configuration file %s (configuration #%d): currently %s shader file name!",
					pg_stringShaderTypes[_pg_shader_Sensor].c_str(), pg_ConfigurationFileNames[indConfig].c_str(), indConfig,
					pg_Shader_File_Names[indConfig][_pg_shader_Sensor].c_str()); ReportError(ErrorStr); throw 430;
			}
			else if (shader_type == _pg_shader_Mesh && pg_FullScenarioActiveVars[indConfig][_activeMeshes]
				&& pg_Shader_File_Names[indConfig][_pg_shader_Mesh] == "NULL") {
				sprintf(ErrorStr, "Error: Expected [%s] shader in configuration file %s (configuration #%d): currently %s shader file name!",
					pg_stringShaderTypes[_pg_shader_Mesh].c_str(), pg_ConfigurationFileNames[indConfig].c_str(), indConfig,
					pg_Shader_File_Names[indConfig][_pg_shader_Mesh].c_str()); ReportError(ErrorStr); throw 430;
			}
			if (pg_Shader_File_Names[indConfig][shader_type] != "NULL") {
				pg_shader_programme[indConfig][shader_type] = glCreateProgram();
				unsigned int* pg_shader = new unsigned int[pg_Shader_nbStages[indConfig][shader_type]];
				string fileShaderName = "";
				std::cout << pg_Shader_File_Names[indConfig][shader_type] + " (";
				for (int shader_stage = 0; shader_stage < pg_Shader_nbStages[indConfig][shader_type]; shader_stage++) {
					//printf("shader conf %d indshader %d name %s stage %d ID %d\n", indConfig, shader_type, fileShaderName.c_str(), pg_Shader_Stages[indConfig][shader_type][shader_stage], pg_shader_programme[indConfig][shader_type]);
					switch (pg_Shader_Stages[indConfig][shader_type][shader_stage]) {
					case GL_VERTEX_SHADER:
						fileShaderName = pg_Shader_File_Names[indConfig][shader_type] + ".vert";
						std::cout << "vert, ";
						break;
					case GL_TESS_CONTROL_SHADER:
						fileShaderName = pg_Shader_File_Names[indConfig][shader_type] + ".tess";
						std::cout << "tess, ";
						break;
					case GL_TESS_EVALUATION_SHADER:
						fileShaderName = pg_Shader_File_Names[indConfig][shader_type] + ".eval";
						std::cout << "eval, ";
						break;
					case GL_GEOMETRY_SHADER:
						fileShaderName = pg_Shader_File_Names[indConfig][shader_type] + ".geom";
						std::cout << "geom, ";
						break;
					case GL_FRAGMENT_SHADER:
						if (shader_type != _pg_shader_Sensor && shader_type != _pg_shader_ClipArt && shader_type != _pg_shader_Mesh) {
							fileShaderName = pg_Shader_File_Names[indConfig][shader_type] + "_full_" + std::to_string(indConfig) + ".frag";
							std::cout << "_full_" + std::to_string(indConfig) + ".frag, ";
						}
						else {
							fileShaderName = pg_Shader_File_Names[indConfig][shader_type] + "_full.frag";
							std::cout << "frag, ";
						}
						break;
					default:
						fileShaderName = "";
						sprintf(ErrorStr, "Error: unknown shader type [%d]!", pg_Shader_Stages[indConfig][shader_type][shader_stage]); ReportError(ErrorStr); throw 430;
						break;
					}

					pg_shader[shader_stage] = glCreateShader(pg_Shader_Stages[indConfig][shader_type][shader_stage]);
					pg_loadshader((char*)(shaders_directory + fileShaderName).c_str(), pg_shader[shader_stage]);
					glCompileShader(pg_shader[shader_stage]);
					pg_printShaderCompileLog(pg_shader[shader_stage]);

					glAttachShader(pg_shader_programme[indConfig][shader_type], pg_shader[shader_stage]);
					printOglError(330);
				}
				std::cout << "), ";

				glLinkProgram(pg_shader_programme[indConfig][shader_type]);
				pg_printShaderLinkLog(pg_shader_programme[indConfig][shader_type]);
				printOglError(331);

				// detach afeter inking
				for (int shader_stage = 0; shader_stage < pg_Shader_nbStages[indConfig][shader_type]; shader_stage++) {
					glDetachShader(pg_shader_programme[indConfig][shader_type], pg_shader[shader_stage]);
				}
				// memory release
				delete[] pg_shader;
			}
			else {
				pg_shader_programme[indConfig][shader_type] = 0;
			}
		}
		std::cout << std::endl;
	}

	////////////////////////////////////////
	// binding and allocating tables in shaders

#if defined(CORE)
#include "pg_shader_body_bind_Core.cpp"
#endif
#if defined(pg_Project_Voluspa)
#include "pg_shader_body_bind_voluspa.cpp"
#endif
#if defined(pg_Project_araKnit)
#include "pg_shader_body_bind_araknit.cpp"
#endif

// #include "pg_scripts/pg_shader_body_bind.cpp"


	////////////////////////////////////////
	// binding variables in shaders
	std::cout << "Unbound uniforms: " << std::endl;
	for (int indConfig = 0; indConfig < pg_NbConfigurations; indConfig++) {
		std::cout << "   " << indConfig << ": ";
		////////////////////////////////////////////////////////////////////////////////
		// PARTICLE ANIMATION SHADER parameters bindings
		////////////////////////////////////////////////////////////////////////////////
		pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleAnimation_vp_model, "vp_modelMatrix", _pg_shader_ParticleAnimation);
		pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleAnimation_vp_view, "vp_viewMatrix", _pg_shader_ParticleAnimation);
		pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleAnimation_vp_proj, "vp_projMatrix", _pg_shader_ParticleAnimation);

		pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleAnimation_vp_2fv_width_height, "uniform_ParticleAnimation_vp_2fv_width_height", _pg_shader_ParticleAnimation);
		pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleAnimation_fs_4fv_W_H_repopChannel_targetFrameNo, "uniform_ParticleAnimation_fs_4fv_W_H_repopChannel_targetFrameNo", _pg_shader_ParticleAnimation);
		pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleAnimation_fs_4fv_flashTrkPartWghts, "uniform_ParticleAnimation_fs_4fv_flashTrkPartWghts", _pg_shader_ParticleAnimation);
		pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleAnimation_path_data, "uniform_ParticleAnimation_path_data", _pg_shader_ParticleAnimation);
		pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleAnimation_fs_4fv_repop_Color_frameNo, "uniform_ParticleAnimation_fs_4fv_repop_Color_frameNo", _pg_shader_ParticleAnimation);
		pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleAnimation_fs_4fv_flashCAPartWght_nbPart_clear_partSizeUnpulsed, "uniform_ParticleAnimation_fs_4fv_flashCAPartWght_nbPart_clear_partSizeUnpulsed", _pg_shader_ParticleAnimation);
		pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleAnimation_fs_4fv_Camera_W_H_movieWH, "uniform_ParticleAnimation_fs_4fv_Camera_W_H_movieWH", _pg_shader_ParticleAnimation);

		// BG track FBO
		pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleAnimation_texture_fs_CA, "uniform_ParticleAnimation_texture_fs_CA", _pg_shader_ParticleAnimation);  // 2-cycle ping-pong position/speed of particles (FBO)
		pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleAnimation_texture_fs_Part_pos_speed, "uniform_ParticleAnimation_texture_fs_Part_pos_speed", _pg_shader_ParticleAnimation);  // 2-cycle ping-pong position/speed of particles (FBO)
		pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleAnimation_texture_fs_Part_col_rad, "uniform_ParticleAnimation_texture_fs_Part_col_rad", _pg_shader_ParticleAnimation);  // 2-cycle ping-pong color/radius of particles (FBO)
		pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleAnimation_texture_fs_Part_Target_pos_col_rad, "uniform_ParticleAnimation_texture_fs_Part_Target_pos_col_rad", _pg_shader_ParticleAnimation);  // 2-cycle ping-pong target position/color/radius of particles (FBO)
		pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleAnimation_texture_fs_Trk0, "uniform_ParticleAnimation_texture_fs_Trk0", _pg_shader_ParticleAnimation);  // ping-pong track 0 (FBO)
		pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleAnimation_texture_fs_Trk1, "uniform_ParticleAnimation_texture_fs_Trk1", _pg_shader_ParticleAnimation);  // ping-pong track 1 (FBO)
		pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleAnimation_texture_fs_Trk2, "uniform_ParticleAnimation_texture_fs_Trk2", _pg_shader_ParticleAnimation);  // ping-pong track 2 (FBO)
		pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleAnimation_texture_fs_Trk3, "uniform_ParticleAnimation_texture_fs_Trk3", _pg_shader_ParticleAnimation);  // ping-pong track 3 (FBO)

		///////////////////////////////////////////////////////////////////////////
		// textures

		// video frames
		pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleAnimation_texture_fs_Camera_frame, "uniform_ParticleAnimation_texture_fs_Camera_frame", _pg_shader_ParticleAnimation);  // camera frame
		pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleAnimation_texture_fs_Movie_frame, "uniform_ParticleAnimation_texture_fs_Movie_frame", _pg_shader_ParticleAnimation);  // movie frame

		pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleAnimation_texture_fs_Noise, "uniform_ParticleAnimation_texture_fs_Noise", _pg_shader_ParticleAnimation);  // 3D noise

		pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleAnimation_texture_fs_RepopDensity, "uniform_ParticleAnimation_texture_fs_RepopDensity", _pg_shader_ParticleAnimation);  // repop density
		pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleAnimation_texture_fs_Part_init_pos_speed, "uniform_ParticleAnimation_texture_fs_Part_init_pos_speed", _pg_shader_ParticleAnimation);  // particle initialization pairs of textures position/speed
		pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleAnimation_texture_fs_Part_init_col_rad, "uniform_ParticleAnimation_texture_fs_Part_init_col_rad", _pg_shader_ParticleAnimation);  // particle initialization pairs of textures color/radius
		pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleAnimation_texture_fs_Part_acc, "uniform_ParticleAnimation_texture_fs_Part_acc", _pg_shader_ParticleAnimation);  // particle acceleration texture

		////////////////////////////////////////////////////////////////////////////////
		// UPDATE SHADER parameters bindings
		////////////////////////////////////////////////////////////////////////////////
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_vp_model, "vp_modelMatrix", _pg_shader_Update);
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_vp_view, "vp_viewMatrix", _pg_shader_Update);
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_vp_proj, "vp_projMatrix", _pg_shader_Update);
		if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_photo_homography]) {
			if (photo_homography) {
				pg_allocateBindAndCheckUniform(indConfig, uniform_Update_homographyForTexture, "fs_homographyForTexture", _pg_shader_Update);
			}
		}



		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_vp_2fv_width_height, "uniform_Update_vp_2fv_width_height", _pg_shader_Update);
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_fs_4fv_W_H_time_currentScene, "uniform_Update_fs_4fv_W_H_time_currentScene", _pg_shader_Update);
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_fs_3fv_clearAllLayers_clearCA_pulsedShift, "uniform_Update_fs_3fv_clearAllLayers_clearCA_pulsedShift", _pg_shader_Update);
#if defined(pg_Project_CAaudio)
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_fs_4fv_CAseed_type_size_loc, "uniform_Update_fs_4fv_CAseed_type_size_loc", _pg_shader_Update);
#endif
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_fs_4fv_flashTrkBGWghts_flashPartBGWght, "uniform_Update_fs_4fv_flashTrkBGWghts_flashPartBGWght", _pg_shader_Update);
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_fs_4fv_frameno_Cursor_flashPartCAWght_doubleWindow, "uniform_Update_fs_4fv_frameno_Cursor_flashPartCAWght_doubleWindow", _pg_shader_Update);
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_fs_4fv_flashTrkCAWghts, "uniform_Update_fs_4fv_flashTrkCAWghts", _pg_shader_Update);
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_path_data, "uniform_Update_path_data", _pg_shader_Update);

#if defined(pg_Project_Criton)
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_fs_4fv_fftLevels03, "uniform_Update_fs_4fv_fftLevels03", _pg_shader_Update);
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_fs_4fv_fftFrequencies03, "uniform_Update_fs_4fv_fftFrequencies03", _pg_shader_Update);
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_fs_4fv_fftPhases03, "uniform_Update_fs_4fv_fftPhases03", _pg_shader_Update);
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_fs_4fv_fftLevels47, "uniform_Update_fs_4fv_fftLevels47", _pg_shader_Update);
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_fs_4fv_fftFrequencies47, "uniform_Update_fs_4fv_fftFrequencies47", _pg_shader_Update);
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_fs_4fv_fftPhases47, "uniform_Update_fs_4fv_fftPhases47", _pg_shader_Update);
#endif
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_fs_4fv_movieWH_flashCameraTrkWght_cpTrack, "uniform_Update_fs_4fv_movieWH_flashCameraTrkWght_cpTrack", _pg_shader_Update);
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_fs_4fv_repop_ColorBG_flashCABGWght, "uniform_Update_fs_4fv_repop_ColorBG_flashCABGWght", _pg_shader_Update);
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_fs_3fv_repop_ColorCA, "uniform_Update_fs_3fv_repop_ColorCA", _pg_shader_Update);
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_fs_3fv_isClearLayer_flashPixel_flashCameraTrkThres, "uniform_Update_fs_3fv_isClearLayer_flashPixel_flashCameraTrkThres", _pg_shader_Update);
		if (pg_FullScenarioActiveVars[indConfig][_flashPhotoTrkBeat]
			&& pg_FullScenarioActiveVars[indConfig][_flashPhotoTrkBright]
			&& pg_FullScenarioActiveVars[indConfig][_flashPhotoTrkLength]
			&& pg_FullScenarioActiveVars[indConfig][_flashPhotoChangeBeat]) {
			pg_allocateBindAndCheckUniform(indConfig, uniform_Update_fs_4fv_flashPhotoTrkWght_flashPhotoTrkThres_Photo_offSetsXY, "uniform_Update_fs_4fv_flashPhotoTrkWght_flashPhotoTrkThres_Photo_offSetsXY", _pg_shader_Update);
		}
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_fs_4fv_photo01_wh, "uniform_Update_fs_4fv_photo01_wh", _pg_shader_Update);
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_fs_4fv_photo01Wghts_randomValues, "uniform_Update_fs_4fv_photo01Wghts_randomValues", _pg_shader_Update);

#if PG_NB_PARALLEL_CLIPS >= 2
		if (pg_FullScenarioActiveVars[indConfig][_clipCaptFreq]) {
			pg_allocateBindAndCheckUniform(indConfig, uniform_Update_fs_2fv_clip01Wghts, "uniform_Update_fs_2fv_clip01Wghts", _pg_shader_Update);
		}
#endif
		if (pg_FullScenarioActiveVars[indConfig][_cameraCaptFreq]) {
			pg_allocateBindAndCheckUniform(indConfig, uniform_Update_fs_4fv_Camera_offSetsXY_Camera_W_H, "uniform_Update_fs_4fv_Camera_offSetsXY_Camera_W_H", _pg_shader_Update);
		}
		if (pg_FullScenarioActiveVars[indConfig][_clipCaptFreq]) {
			pg_allocateBindAndCheckUniform(indConfig, uniform_Update_fs_3fv_photo_rgb, "uniform_Update_fs_3fv_photo_rgb", _pg_shader_Update);
		}
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_fs_4fv_xy_transl_tracks_0_1, "uniform_Update_fs_4fv_xy_transl_tracks_0_1", _pg_shader_Update);
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_fs_4fv_CAType_SubType_blurRadius, "uniform_Update_fs_4fv_CAType_SubType_blurRadius", _pg_shader_Update);
		//printf("allocate uniform_Update_fs_4fv_CAType_SubType_blurRadius config %d\n", indConfig);
		if (pg_FullScenarioActiveVars[indConfig][_CATable]) {
			pg_allocateBindAndCheckUniform(indConfig, uniform_Update_texture_fs_CATable, "uniform_Update_texture_fs_CATable", _pg_shader_Update);
		}
#if defined(var_GenerativeNights_planes)
		if (pg_FullScenarioActiveVars[indConfig][_GenerativeNights_planes]) {
			pg_allocateBindAndCheckUniform(indConfig, uniform_Update_fs_2fv_initCA_1stPlaneFrameNo, "uniform_Update_fs_2fv_initCA_1stPlaneFrameNo", _pg_shader_Update);
		}
#endif
		if (pg_FullScenarioActiveVars[indConfig][_camera_BG_subtr]) {
			pg_allocateBindAndCheckUniform(indConfig, uniform_Update_texture_fs_Camera_BGIni, "uniform_Update_texture_fs_Camera_BGIni", _pg_shader_Update);  // initial background frame
		}

		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_texture_fs_pixel_acc, "uniform_Update_texture_fs_pixel_acc", _pg_shader_Update);  // initial background frame

		// BG track FBO
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_texture_fs_CA, "uniform_Update_texture_fs_CA", _pg_shader_Update);         // 2-cycle ping-pong CA (FBO)

		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_texture_fs_PreviousCA, "uniform_Update_texture_fs_PreviousCA", _pg_shader_Update);         // 2-cycle ping-pong CA (FBO)
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_texture_fs_Pixels, "uniform_Update_texture_fs_Pixels", _pg_shader_Update);  // 2-cycle ping-pong speed/position of pixels (FBO)
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_texture_fs_Trk0, "uniform_Update_texture_fs_Trk0", _pg_shader_Update);  // ping-pong track 0 (FBO)
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_texture_fs_Trk1, "uniform_Update_texture_fs_Trk1", _pg_shader_Update);  // ping-pong track 1 (FBO)
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_texture_fs_Trk2, "uniform_Update_texture_fs_Trk2", _pg_shader_Update);  // ping-pong track 2 (FBO)
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_texture_fs_Trk3, "uniform_Update_texture_fs_Trk3", _pg_shader_Update);  // ping-pong track 3 (FBO)

		// textures
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_texture_fs_Brushes, "uniform_Update_texture_fs_Brushes", _pg_shader_Update);  // pen patterns
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_texture_fs_Camera_frame, "uniform_Update_texture_fs_Camera_frame", _pg_shader_Update);  // camera frame
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_texture_fs_Camera_BG, "uniform_Update_texture_fs_Camera_BG", _pg_shader_Update);  // current background frame
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_texture_fs_Movie_frame, "uniform_Update_texture_fs_Movie_frame", _pg_shader_Update);  // movie frame
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_texture_fs_Noise, "uniform_Update_texture_fs_Noise", _pg_shader_Update);  // 3D noise
		if (pg_FullScenarioActiveVars[indConfig][_BG_CA_repop_density]) {
			pg_allocateBindAndCheckUniform(indConfig, uniform_Update_texture_fs_RepopDensity, "uniform_Update_texture_fs_RepopDensity", _pg_shader_Update);  // repop density
		}

		// photo frames
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_texture_fs_Photo0, "uniform_Update_texture_fs_Photo0", _pg_shader_Update);  // photo[0] frame
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_texture_fs_Photo1, "uniform_Update_texture_fs_Photo1", _pg_shader_Update);  // photo[1] frame
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_texture_fs_Clip0, "uniform_Update_texture_fs_Clip0", _pg_shader_Update);  // clip[0]
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_texture_fs_Clip1, "uniform_Update_texture_fs_Clip1", _pg_shader_Update);  // clip[1]
		pg_allocateBindAndCheckUniform(indConfig, uniform_Update_texture_fs_Part_render, "uniform_Update_texture_fs_Part_render", _pg_shader_Update);  // particle rendering capture

		////////////////////////////////////////////////////////////////////////////////
		// PARTICLE RENDERING SHADER parameters bindings
		////////////////////////////////////////////////////////////////////////////////
		if (PG_PARTICLE_TYPE == 2) {
			pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleCurve_vp_model, "vp_modelMatrix", _pg_shader_ParticleRender);
			pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleCurve_vp_view, "vp_viewMatrix", _pg_shader_ParticleRender);
			pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleCurve_vp_proj, "vp_projMatrix", _pg_shader_ParticleRender);

			pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleCurve_gs_3fv_partRadius_partType_highPitchPulse, "uniform_ParticleCurve_gs_3fv_partRadius_partType_highPitchPulse", _pg_shader_ParticleRender);
			pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleCurve_vp_3fv_trackReplay_xy_height, "uniform_ParticleCurve_vp_3fv_trackReplay_xy_height", _pg_shader_ParticleRender);

			// particle position/speed FBO
			pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleCurve_texture_vp_Part_pos_speed, "uniform_ParticleCurve_texture_vp_Part_pos_speed", _pg_shader_ParticleRender);
			// particle color/radius FBO
			pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleCurve_texture_vp_Part_col_rad, "uniform_ParticleCurve_texture_vp_Part_col_rad", _pg_shader_ParticleRender);

			// color texture
			pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleCurve_Curve_texture_fs_decal, "fs_comet", _pg_shader_ParticleRender);
		}
		else {
			pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleSplat_vp_model, "vp_modelMatrix", _pg_shader_ParticleRender);
			pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleSplat_vp_view, "vp_viewMatrix", _pg_shader_ParticleRender);
			pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleSplat_vp_proj, "vp_projMatrix", _pg_shader_ParticleRender);

			pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleSplat_gs_4fv_part_size_partType_highPitchPulse_windowRatio, "uniform_ParticleSplat_gs_4fv_part_size_partType_highPitchPulse_windowRatio", _pg_shader_ParticleRender);
			pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleSplat_vp_3fv_trackReplay_xy_height, "uniform_ParticleSplat_vp_3fv_trackReplay_xy_height", _pg_shader_ParticleRender);

			// particle position/speed FBO
			pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleSplat_texture_vp_Part_pos_speed, "uniform_ParticleSplat_texture_vp_Part_pos_speed", _pg_shader_ParticleRender);
			// particle color/radius FBO
			pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleSplat_texture_vp_Part_col_rad, "uniform_ParticleSplat_texture_vp_Part_col_rad", _pg_shader_ParticleRender);
		}

		// color texture
		pg_allocateBindAndCheckUniform(indConfig, uniform_ParticleSplat_texture_fs_decal, "uniform_ParticleSplat_texture_fs_decal", _pg_shader_ParticleRender);

		////////////////////////////////////////////////////////////////////////////////
		// MIXING SHADER parameters bindings
		////////////////////////////////////////////////////////////////////////////////
		pg_allocateBindAndCheckUniform(indConfig, uniform_Mixing_vp_model, "vp_modelMatrix", _pg_shader_Mixing);
		pg_allocateBindAndCheckUniform(indConfig, uniform_Mixing_vp_view, "vp_viewMatrix", _pg_shader_Mixing);
		pg_allocateBindAndCheckUniform(indConfig, uniform_Mixing_vp_proj, "vp_projMatrix", _pg_shader_Mixing);

		pg_allocateBindAndCheckUniform(indConfig, uniform_Mixing_fs_3fv_height_flashCameraTrkWght_flashPhotoTrkWght, "uniform_Mixing_fs_3fv_height_flashCameraTrkWght_flashPhotoTrkWght", _pg_shader_Mixing);
		pg_allocateBindAndCheckUniform(indConfig, uniform_Mixing_fs_3fv_screenMsgTransp_Text1_2_Alpha, "uniform_Mixing_fs_3fv_screenMsgTransp_Text1_2_Alpha", _pg_shader_Mixing);

		pg_allocateBindAndCheckUniform(indConfig, uniform_Mixing_texture_fs_CA, "uniform_Mixing_texture_fs_CA", _pg_shader_Mixing); // ping-pong CA (FBO)
		pg_allocateBindAndCheckUniform(indConfig, uniform_Mixing_texture_fs_ClipArt_render, "uniform_Mixing_texture_fs_ClipArt_render", _pg_shader_Mixing);  // Particles (FBO)
		pg_allocateBindAndCheckUniform(indConfig, uniform_Mixing_texture_fs_Particle_render, "uniform_Mixing_texture_fs_Particle_render", _pg_shader_Mixing);  // Particles (FBO)
		pg_allocateBindAndCheckUniform(indConfig, uniform_Mixing_texture_fs_Trk0, "uniform_Mixing_texture_fs_Trk0", _pg_shader_Mixing);  // ping-pong track 0 (FBO)
		pg_allocateBindAndCheckUniform(indConfig, uniform_Mixing_texture_fs_Trk1, "uniform_Mixing_texture_fs_Trk1", _pg_shader_Mixing);  // ping-pong track 1 (FBO)
		pg_allocateBindAndCheckUniform(indConfig, uniform_Mixing_texture_fs_Trk2, "uniform_Mixing_texture_fs_Trk2", _pg_shader_Mixing);  // ping-pong track 2 (FBO)
		pg_allocateBindAndCheckUniform(indConfig, uniform_Mixing_texture_fs_Trk3, "uniform_Mixing_texture_fs_Trk3", _pg_shader_Mixing);  // ping-pong track 3 (FBO)

		pg_allocateBindAndCheckUniform(indConfig, uniform_Mixing_texture_fs_Render_prec, "uniform_Mixing_texture_fs_Render_prec", _pg_shader_Mixing);  // preceding snapshot for echo
		pg_allocateBindAndCheckUniform(indConfig, uniform_Mixing_texture_fs_Screen_Font, "uniform_Mixing_texture_fs_Screen_Font", _pg_shader_Mixing);  // message Font
		pg_allocateBindAndCheckUniform(indConfig, uniform_Mixing_texture_fs_Screen_Message, "uniform_Mixing_texture_fs_Screen_Message", _pg_shader_Mixing);  // message string

		////////////////////////////////////////////////////////////////////////////////
		// MASTER SHADER parameters bindings
		////////////////////////////////////////////////////////////////////////////////
		pg_allocateBindAndCheckUniform(indConfig, uniform_Master_vp_model, "vp_modelMatrix", _pg_shader_Master);
		pg_allocateBindAndCheckUniform(indConfig, uniform_Master_vp_view, "vp_viewMatrix", _pg_shader_Master);
		pg_allocateBindAndCheckUniform(indConfig, uniform_Master_vp_proj, "vp_projMatrix", _pg_shader_Master);
		pg_allocateBindAndCheckUniform(indConfig, uniform_Master_fs_4fv_xy_frameno_pulsedShift, "uniform_Master_fs_4fv_xy_frameno_pulsedShift", _pg_shader_Master);
		pg_allocateBindAndCheckUniform(indConfig, uniform_Master_fs_4fv_width_height_timeFromStart_muteRightScreen, "uniform_Master_fs_4fv_width_height_timeFromStart_muteRightScreen", _pg_shader_Master);

		//pg_allocateBindAndCheckUniform(indConfig, uniform_Master_fs_4fv_pulsedColor_rgb_pen_grey, "uniform_Master_fs_4fv_pulsedColor_rgb_pen_grey", _pg_shader_Master);
		//pg_allocateBindAndCheckUniform(indConfig, uniform_Master_fs_4fv_interpolatedPaletteLow_rgb_currentScene, "uniform_Master_fs_4fv_interpolatedPaletteLow_rgb_currentScene", _pg_shader_Master);
		//pg_allocateBindAndCheckUniform(indConfig, uniform_Master_fs_4fv_interpolatedPaletteMedium_rgb_mobile_cursor, "uniform_Master_fs_4fv_interpolatedPaletteMedium_rgb_mobile_cursor", _pg_shader_Master);
		//pg_allocateBindAndCheckUniform(indConfig, uniform_Master_fs_3fv_interpolatedPaletteHigh_rgb, "uniform_Master_fs_3fv_interpolatedPaletteHigh_rgb", _pg_shader_Master);
		pg_allocateBindAndCheckUniform(indConfig, uniform_Master_fs_2iv_mobile_cursor_currentScene, "uniform_Master_fs_2iv_mobile_cursor_currentScene", _pg_shader_Master);
		if (pg_FullScenarioActiveVars[indConfig][_BGcolor]
			|| pg_FullScenarioActiveVars[indConfig][_flashchange_BGcolor_freq]) {
			pg_allocateBindAndCheckUniform(indConfig, uniform_Master_fs_3fv_BGcolor_rgb, "uniform_Master_fs_3fv_BGcolor_rgb", _pg_shader_Master);
		}

		pg_allocateBindAndCheckUniform(indConfig, uniform_Master_texture_fs_Render_curr, "uniform_Master_texture_fs_Render_curr", _pg_shader_Master); // previous pass output
		pg_allocateBindAndCheckUniform(indConfig, uniform_Master_texture_fs_CA, "uniform_Master_texture_fs_CA", _pg_shader_Master); // ping-pong CA (FBO)
		pg_allocateBindAndCheckUniform(indConfig, uniform_Master_texture_fs_ClipArt_render, "uniform_Master_texture_fs_ClipArt_render", _pg_shader_Master); // Particles
		pg_allocateBindAndCheckUniform(indConfig, uniform_Master_texture_fs_Particle_render, "uniform_Master_texture_fs_Particle_render", _pg_shader_Master); // Particles
		pg_allocateBindAndCheckUniform(indConfig, uniform_Master_texture_fs_Trk0, "uniform_Master_texture_fs_Trk0", _pg_shader_Master); // ping-pong track 0 (FBO)
		pg_allocateBindAndCheckUniform(indConfig, uniform_Master_texture_fs_Trk1, "uniform_Master_texture_fs_Trk1", _pg_shader_Master); // ping-pong track 1 (FBO)
		pg_allocateBindAndCheckUniform(indConfig, uniform_Master_texture_fs_Trk2, "uniform_Master_texture_fs_Trk2", _pg_shader_Master); // ping-pong track 2 (FBO)
		pg_allocateBindAndCheckUniform(indConfig, uniform_Master_texture_fs_Trk3, "uniform_Master_texture_fs_Trk3", _pg_shader_Master); // ping-pong track 3 (FBO)
		pg_allocateBindAndCheckUniform(indConfig, uniform_Master_texture_fs_Mask, "uniform_Master_texture_fs_Mask", _pg_shader_Master);  // mask for master output

		if (pg_FullScenarioActiveVars[indConfig][_sensor_layout]) {
			////////////////////////////////////////////////////////////////////////////////
			// SENSOR SHADER parameters bindings
			////////////////////////////////////////////////////////////////////////////////
			pg_allocateBindAndCheckUniform(indConfig, uniform_Sensor_vp_model, "vp_modelMatrix", _pg_shader_Sensor);
			pg_allocateBindAndCheckUniform(indConfig, uniform_Sensor_vp_view, "vp_viewMatrix", _pg_shader_Sensor);
			pg_allocateBindAndCheckUniform(indConfig, uniform_Sensor_vp_proj, "vp_projMatrix", _pg_shader_Sensor);
			pg_allocateBindAndCheckUniform(indConfig, uniform_Sensor_fs_4fv_onOff_isCurrentSensor_masterLevel_transparency, "uniform_Sensor_fs_4fv_onOff_isCurrentSensor_masterLevel_transparency", _pg_shader_Sensor);

			pg_allocateBindAndCheckUniform(indConfig, uniform_Sensor_texture_fs_decal, "fs_decal", _pg_shader_Sensor); // sensor texture
		}

		if (pg_FullScenarioActiveVars[indConfig][_activeMeshes]) {
			////////////////////////////////////////////////////////////////////////////////
			// MESH SHADER parameters bindings
			////////////////////////////////////////////////////////////////////////////////
			pg_allocateBindAndCheckUniform(indConfig, uniform_Mesh_vp_model, "vp_modelMatrix", _pg_shader_Mesh);
			pg_allocateBindAndCheckUniform(indConfig, uniform_Mesh_vp_view, "vp_viewMatrix", _pg_shader_Mesh);
			pg_allocateBindAndCheckUniform(indConfig, uniform_Mesh_vp_proj, "vp_projMatrix", _pg_shader_Mesh);
			pg_allocateBindAndCheckUniform(indConfig, uniform_Mesh_fs_4fv_isDisplayLookAt_with_mesh_with_blue_currentScene, "uniform_Mesh_fs_4fv_isDisplayLookAt_with_mesh_with_blue_currentScene", _pg_shader_Mesh);
			pg_allocateBindAndCheckUniform(indConfig, uniform_Mesh_fs_3fv_light, "uniform_Mesh_fs_3fv_light", _pg_shader_Mesh);
#if defined(var_Contact_mesh_expand)
			if (pg_FullScenarioActiveVars[indConfig][_Contact_mesh_expand]) {
				pg_allocateBindAndCheckUniform(indConfig, uniform_Mesh_vp_2fv_dilate_explode, "uniform_Mesh_vp_2fv_dilate_explode", _pg_shader_Mesh);
			}
#endif
#if defined(var_Contact_mesh_anime)
			if (pg_FullScenarioActiveVars[indConfig][_Contact_mesh_anime]) {
				pg_allocateBindAndCheckUniform(indConfig, uniform_Mesh_bones_matrices, "uniform_Mesh_bones_matrices", _pg_shader_Mesh);
			}
#endif
#if defined(var_MmeShanghai_brokenGlass)
			if (pg_FullScenarioActiveVars[indConfig][_MmeShanghai_brokenGlass]) {
				pg_allocateBindAndCheckUniform(indConfig, uniform_Mesh_fs_4fv_color, "uniform_Mesh_fs_4fv_color", _pg_shader_Mesh);
				pg_allocateBindAndCheckUniform(indConfig, uniform_Mesh_fs_4fv_color_master_photo_weight_bg, "uniform_Mesh_fs_4fv_color_master_photo_weight_bg", _pg_shader_Mesh);
			}
#endif
			// Augmented Reality: FBO capture of Master to be displayed on a mesh
			if (pg_FullScenarioActiveVars[indConfig][_textureFrontier_wmin]) {
				pg_allocateBindAndCheckUniform(indConfig, uniform_Mesh_fs_4fv_textureFrontier, "uniform_Mesh_fs_4fv_textureFrontier", _pg_shader_Mesh);
				pg_allocateBindAndCheckUniform(indConfig, uniform_Mesh_fs_4fv_textureFrontier_width, "uniform_Mesh_fs_4fv_textureFrontier_width", _pg_shader_Mesh);
				pg_allocateBindAndCheckUniform(indConfig, uniform_Mesh_fs_4fv_textureScaleTransl, "uniform_Mesh_fs_4fv_textureScaleTransl", _pg_shader_Mesh);
			}
#if defined(var_Contact_mesh_palette)
			if (pg_FullScenarioActiveVars[indConfig][_Contact_mesh_palette]) {
				pg_allocateBindAndCheckUniform(indConfig, uniform_Mesh_fs_4fv_color_palette, "uniform_Mesh_fs_4fv_color_palette", _pg_shader_Mesh); // mesh color
			}
#endif
			pg_allocateBindAndCheckUniform(indConfig, uniform_Mesh_texture_fs_decal, "fs_Mesh_texture_fs_decal", _pg_shader_Mesh); // previous pass output
#if defined(var_MmeShanghai_brokenGlass)
			if (pg_FullScenarioActiveVars[indConfig][_MmeShanghai_brokenGlass]) {
				pg_allocateBindAndCheckUniform(indConfig, uniform_Mesh_texture_fs_BG, "fs_Mesh_texture_fs_BG", _pg_shader_Mesh); // previous pass output
			}
#endif
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}