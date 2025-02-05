////////////////////////////////////////////////////////////////////
// _
// 
// 
//     File pg-render.cpp
// 
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2.1
// of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this program; if not, write to the Free
// Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.
////////////////////////////////////////////////////////////////////

#include "pg-all_include.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////
// GLOBAL VARS
//////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(_CRTDBG_MAP_ALLOC)
// MEMORY LEAK CONTROL
_CrtMemState s1{};
_CrtMemState s2{};
_CrtMemState s3{};
_CrtMemState s4{};
_CrtMemState s5{};
_CrtMemState s6{};
_CrtMemState s7{};
_CrtMemState s8{};
_CrtMemState s9{};
_CrtMemState s10{};
_CrtMemState s11{};
_CrtMemState s12{};
_CrtMemState s13{};
_CrtMemState sDiff{};
#endif
 
/////////////////////////////////////////////////////////////////
// Projection and view matrices for the shaders
//////////////////////////////////////////////////////////////////////
GLfloat pg_orthoWindowProjMatrix[16];
GLfloat pg_doubleProjMatrix[16];
GLfloat pg_homographyForTexture[9];
GLfloat pg_modelMatrixSensor[16];

/////////////////////////////////////////////////////////////////
// Repop colors
//////////////////////////////////////////////////////////////////////
float pg_repop_ColorBGcolorRed;
float pg_repop_ColorBG_g;
float pg_repop_ColorBG_b;
float pg_repop_ColorCA_r;
float pg_repop_ColorCA_g;
float pg_repop_ColorCA_b;
float pg_repop_ColorPart_r;
float pg_repop_ColorPart_g;
float pg_repop_ColorPart_b;

// interpolated CA
int CAInterpolatedType;
int CAInterpolatedSubType;
int CAInterpolatedType_prev = -1;
int CAInterpolatedSubType_prev = -1;

// svg gpu translation
float				svg_translate_x = 0.f;
float				svg_translate_y = 0.f;
float				svg_translate_z = 0.f;
// svg gpu scaling
float				svg_scale = 1.f;

// mobile cursor for cursor blinking in Master shader
bool mobile_cursor = true;

int pg_tablet_prec_x = 0;
int pg_tablet_prec_y = 0;

// MVP matrices
glm::mat4 pg_VP1perspMatrix;
glm::mat4 pg_VP1viewMatrix;
glm::mat4 pg_VP1homographyMatrix;
#if defined(PG_SECOND_MESH_CAMERA)
glm::mat4 pg_VP2perspMatrix;
glm::mat4 pg_VP2viewMatrix;
glm::mat4 pg_VP2homographyMatrix;
#endif

// homographyw matrix
float pg_matValues[16] = { 0 };

cv::Mat homography;

float pen_x, pen_y, vec_x, vec_y;

////////////////////////////////////////////////////////////////////
// DIAPORAMA IMAGE FILES
////////////////////////////////////////////////////////////////////
// image index management for progessive image turnover
// according to image blend frequency and duration
// and according to directory size
double pg_CurrentDiaporamaEnd = -1;

GLuint Sensor_texture_rectangle[PG_MAX_SCENARIOS] = { NULL_ID };

// pen palettes presets
vector<ColorPreset> pg_ColorPresets[PG_MAX_SCENARIOS];

// short video clip albums
string pg_ClipDirectory[PG_MAX_SCENARIOS] = { "" };
int pg_NbClipAlbums[PG_MAX_SCENARIOS] = {0};
int clip_image_width[PG_MAX_SCENARIOS] = {0};
int clip_image_height[PG_MAX_SCENARIOS] = {0};
int clip_crop_width[PG_MAX_SCENARIOS] = {0};
int clip_crop_height[PG_MAX_SCENARIOS] = {0};
int clip_max_length[PG_MAX_SCENARIOS] = {0};
// pen brushes
string pen_brushes_fileName[PG_MAX_SCENARIOS] = { "" };
int nb_pen_brushes[PG_MAX_SCENARIOS] = {0};
// textures with multiple layers
int nb_layers_master_mask[PG_MAX_SCENARIOS] = {0};

// indices of the currently played clips
int pg_playing_clipNoLeft = -1;
int pg_playing_secondClipNoLeft = -1;
int pg_playing_clipNoRight = -1;
int pg_playing_secondClipNoRight = -1;

const char *TextureEncodingString[pg_enum_EmptyTextureEncoding + 1] = { "jpeg", "png", "pnga", "png_gray", "pnga_gray", "rgb", "raw", "emptyimagetype" };

// ++++++++++++++++++++++ MUSIC CONTROL ++++++++++++++++++++ 
// music control inputs received at each frame


// factor increasing the acceleration weight of pixels, the brush radius, color...
// based on sound volume attacks
float pulse_average;
float pg_audio_pulse_average_prec = -1.f;
float pg_audio_pulse[3] = { 0.0f };


///////////////////////////////////////////////////////////////////////////////////////////////////////
// RENDERD IMAGE TRANSLATION
//////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////
// CONVERSION OF FLOAT TRANSLATION INTO FRAME BASED INTEGER TRANSLATION
float pg_frame_based_translation(float transl, int translation_rank) {
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_slow_track_translation]) {
		if (slow_track_translation) {
			if (transl == 0) {
				return 0.f;
			}
			else {
				float sign = (transl > 0 ? 1.f : -1.f);
				transl *= sign; // absolute value
				if (transl < 1) {
					if (pg_FrameNo % int(1 / transl) == 0) {
						return 1.f * sign;
					}
					else {
						return 0.f;
					}
				}
				else {
					return float(int(transl)) * sign;
				}
			}
		}
	}
	return 0.f;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// SHADER UNIFORMS UPDATE
//////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////
/////// UPDATES UNIFORMS IN ALL ACTIVE SHADERS
/////// AS WELL AS TABLES FOR SCENARIO VARIABLES BOUND WITH SHADERS
void pg_update_shader_var_data(void) {
#if defined(CORE)
#include "pg_render_body_Core.cpp"
#endif
#if defined(pg_Project_Voluspa)
#include "pg_render_body_voluspa.cpp"
#endif
#if defined(pg_Project_araKnit)
#include "pg_render_body_araknit.cpp"
#endif
	pg_printOglError(37);
}

/////////////////////////////////////////////////////////////////////////
// PARTICLE ANIMATION SHADER UNIFORM VARIABLES
void pg_update_shader_ParticleAnimation_uniforms(void) {
	if (!pg_shader_programme[pg_ind_scenario][pg_enum_shader_ParticleAnimation]) {
		return;
	}

	glUseProgram(pg_shader_programme[pg_ind_scenario][pg_enum_shader_ParticleAnimation]);

	// prints possible particle animation
	if (part_initialization >= 0) {
		printf("particle initialization %d\n", part_initialization);
	}

	// scren size and repop channel
	// at each frame a random repop channel is chosen among the available ones
	int nb_avail_paths = 0;
	for (int pathNo = 0; pathNo < PG_NB_PATHS; pathNo++) {
		if (part_path_repop[pathNo]) {
			nb_avail_paths++;
		}
	}
	int selected_path = 0;
	if (nb_avail_paths > 0) {
		selected_path = int(floor(rand_0_1 * (nb_avail_paths + 1 - 0.00001))) % nb_avail_paths;
		int nb_activ_paths = 0;
		for (int pathNo = 0; pathNo < PG_NB_PATHS; pathNo++) {
			if (part_path_repop[pathNo]) {
				if (nb_activ_paths == selected_path) {
					selected_path = pathNo;
					break;
				}
				nb_activ_paths++;
			}
		}
	}
	glUniform4f(uniform_ParticleAnimation_fs_4fv_W_H_repopChannel_targetFrameNo[pg_ind_scenario],
		(GLfloat)pg_workingWindow_width, (GLfloat)PG_WINDOW_HEIGHT, (GLfloat)selected_path, GLfloat(pg_ParticleTargetFrameNo));

	// pen paths positions
	for (int indPath = 0; indPath < (PG_NB_PATHS + 1); indPath++) {
#if defined(var_Novak_flight_on)
		if (pg_FullScenarioActiveVars[indScenario][_Novak_flight_on]
			&& indPath > 0) {
			pg_paths_currentDynPoint[indPath].pg_paths_x_prev_memory = pg_paths_currentDynPoint[indPath].pg_paths_x_memory;
			pg_paths_currentDynPoint[indPath].pg_paths_x_memory = cur_Novak_flight_2D_points[indPath].x;
			pg_paths_currentDynPoint[indPath].pg_paths_y_prev_memory = pg_paths_currentDynPoint[indPath].pg_paths_y_memory;
			pg_paths_currentDynPoint[indPath].pg_paths_y_memory = cur_Novak_flight_2D_points[indPath].y;
		}
		else {
#endif
			pg_paths_currentDynPoint[indPath].pg_paths_x_memory = (pg_paths_currentDynPoint[indPath].pg_paths_x > 0 ? pg_paths_currentDynPoint[indPath].pg_paths_x : pg_paths_currentDynPoint[indPath].pg_paths_x_memory);
			pg_paths_currentDynPoint[indPath].pg_paths_x_prev_memory = (pg_paths_currentDynPoint[indPath].pg_paths_x_prev > 0 ? pg_paths_currentDynPoint[indPath].pg_paths_x_prev : pg_paths_currentDynPoint[indPath].pg_paths_x_prev_memory);
			pg_paths_currentDynPoint[indPath].pg_paths_y_memory = (pg_paths_currentDynPoint[indPath].pg_paths_y > 0 ? pg_paths_currentDynPoint[indPath].pg_paths_y : pg_paths_currentDynPoint[indPath].pg_paths_y_memory);
			pg_paths_currentDynPoint[indPath].pg_paths_y_prev_memory = (pg_paths_currentDynPoint[indPath].pg_paths_y_prev > 0 ? pg_paths_currentDynPoint[indPath].pg_paths_y_prev : pg_paths_currentDynPoint[indPath].pg_paths_y_prev_memory);
			//if (indPath == 1) {
			//	printf("xy prev xy %.2f %.2f %.2f %.2f\n", pg_paths_currentDynPoint[indPath].pg_paths_x_memory, pg_paths_currentDynPoint[indPath].pg_paths_y_memory, pg_paths_currentDynPoint[indPath].pg_paths_x_prev_memory, pg_paths_currentDynPoint[indPath].pg_paths_y_prev_memory);
			//}
#if defined(var_Novak_flight_on)
		}
#endif
	}

	// position
	for (int indPath = 0; indPath < (PG_NB_PATHS + 1); indPath++) {
		pg_path_data_ParticleAnimation[indPath * PG_MAX_PATH_ANIM_DATA * 4 + PG_PATH_ANIM_POS * 4 + 0] = pg_paths_currentDynPoint[indPath].pg_paths_x_prev_memory;
		pg_path_data_ParticleAnimation[indPath * PG_MAX_PATH_ANIM_DATA * 4 + PG_PATH_ANIM_POS * 4 + 1] = pg_paths_currentDynPoint[indPath].pg_paths_y_prev_memory;
	}
	for (int indPath = 0; indPath < (PG_NB_PATHS + 1); indPath++) {
		pg_path_data_ParticleAnimation[indPath * PG_MAX_PATH_ANIM_DATA * 4 + PG_PATH_ANIM_POS * 4 + 2] = pg_paths_currentDynPoint[indPath].pg_paths_x_memory;
		pg_path_data_ParticleAnimation[indPath * PG_MAX_PATH_ANIM_DATA * 4 + PG_PATH_ANIM_POS * 4 + 3] = pg_paths_currentDynPoint[indPath].pg_paths_y_memory;
	}
	//printf("path data %.2f %.2f %.2f %.2f\n", pg_paths_currentDynPoint[1].pg_paths_x_memory, pg_paths_currentDynPoint[1].pg_paths_y_memory, pg_paths_currentDynPoint[1].pg_paths_x_prev_memory, pg_paths_currentDynPoint[1].pg_paths_y_prev_memory);
	// color, radius, beginning or end of a stroke
	for (int indPath = 0; indPath < (PG_NB_PATHS + 1); indPath++) {
		pg_path_data_ParticleAnimation[indPath * PG_MAX_PATH_ANIM_DATA * 4 + PG_PATH_ANIM_RAD * 4 + 0] = pg_paths_currentDynPoint[indPath].pg_paths_RadiusX;
		pg_path_data_ParticleAnimation[indPath * PG_MAX_PATH_ANIM_DATA * 4 + PG_PATH_ANIM_RAD * 4 + 1] = rand_0_1;
		pg_path_data_ParticleAnimation[indPath * PG_MAX_PATH_ANIM_DATA * 4 + PG_PATH_ANIM_RAD * 4 + 2] = rand_0_1;
	}
	//printf("pg_path_data_ParticleAnimation %.2f %.2f %.2f %.2f\n", pg_path_data_ParticleAnimation[PG_MAX_PATH_ANIM_DATA * 4 + PG_PATH_ANIM_POS * 4 + 0], 
	//	pg_path_data_ParticleAnimation[PG_MAX_PATH_ANIM_DATA * 4 + PG_PATH_ANIM_RAD * 4 + 1],
	//	pg_path_data_ParticleAnimation[PG_MAX_PATH_ANIM_DATA * 4 + PG_PATH_ANIM_RAD * 4 + 2],
	//	pg_path_data_ParticleAnimation[PG_MAX_PATH_ANIM_DATA * 4 + PG_PATH_ANIM_RAD * 4 + 3]);
	glUniform4fv(uniform_ParticleAnimation_path_data[pg_ind_scenario], (PG_NB_PATHS + 1) * PG_MAX_PATH_DATA, pg_path_data_ParticleAnimation);

	// flash Trk -> Part weights
	glUniform4f(uniform_ParticleAnimation_fs_4fv_flashTrkPartWghts[pg_ind_scenario],
		pg_flashTrkPart_weights[0], pg_flashTrkPart_weights[1], pg_flashTrkPart_weights[2], pg_flashTrkPart_weights[3]);
	// printf("pg_flashTrkPart_weights %.2f %.2f %.2f %.2f \n", pg_flashTrkPart_weights[0], pg_flashTrkPart_weights[1], pg_flashTrkPart_weights[2], pg_flashTrkPart_weights[3]);

// movie size, flash camera and copy tracks
// copy to layer above (+1) or to layer below (-1)

// flash CA -> BG & repop color (BG & CA)
	glUniform4f(uniform_ParticleAnimation_fs_4fv_repop_Color_frameNo[pg_ind_scenario],
		pg_repop_ColorPart_r, pg_repop_ColorPart_g, pg_repop_ColorPart_b, GLfloat(pg_FrameNo));
	// clear layer, flash pixel, flash CA -> Part
	glUniform4f(uniform_ParticleAnimation_fs_4fv_flashCAPartWght_nbPart_clear_partSizeUnpulsed[pg_ind_scenario],
		GLfloat(pg_flashCAPart_weight), GLfloat(PG_NB_PARTICLES), GLfloat(pg_isClearAllLayers), *((float*)pg_FullScenarioVarPointers[_part_size]));

	// movie size, flash camera and copy tracks
	// copy to layer above (+1) or to layer below (-1)
	glUniform4f(uniform_ParticleAnimation_fs_4fv_Camera_W_H_movieWH[pg_ind_scenario],
		GLfloat(pg_camera_frame_width), GLfloat(pg_camera_frame_height),
		GLfloat(pg_movie_frame_width), GLfloat(pg_movie_frame_height));
	pg_printOglError(38);
}

/////////////////////////////////////////////////////////////////////////
// UPDATE SHADER UNIFORM VARIABLES
void pg_update_shader_Update_uniforms(void) {
	if (!pg_shader_programme[pg_ind_scenario][pg_enum_shader_Update]) {
		return;
	}

	glUseProgram(pg_shader_programme[pg_ind_scenario][pg_enum_shader_Update]);
	pg_printOglError(43);

	// time is only used in TVW
	glUniform4f(uniform_Update_fs_4fv_W_H_time_currentScene[pg_ind_scenario],
		(GLfloat)pg_workingWindow_width, (GLfloat)PG_WINDOW_HEIGHT, (GLfloat)pg_CurrentClockTime, (GLfloat)pg_CurrentSceneIndex);
	// printf("time %.2f\n", (GLfloat)pg_CurrentClockTime);
	// printf("scene %d\n", pg_CurrentSceneIndex);
	pg_printOglError(45);

	// pixels acceleration
	glUniform3f(uniform_Update_fs_3fv_clearAllLayers_clearCA_pulsedShift[pg_ind_scenario],
		(GLfloat)pg_isClearAllLayers, (GLfloat)pg_isClearCA,
		fabs(pulse_average - pg_audio_pulse_average_prec) * pulsed_part_Vshift);
	//if (pg_isClearAllLayers > 0.f) {
	//	printf("clear all\n");
	//}
	pg_printOglError(44);

#if defined(pg_Project_CAaudio)
	if (pg_CAseed_trigger) {
		glUniform4f(uniform_Update_fs_4fv_CAseed_type_size_loc[pg_ind_scenario],
			(GLfloat)pg_CAseed_type, (GLfloat)pg_CAseed_size,
			(GLfloat)pg_CAseed_coordinates[0], (GLfloat)pg_CAseed_coordinates[1]);
		printf("pg_CAseed_trigger %d %d at %d %d\n", pg_CAseed_type, pg_CAseed_size,
			pg_CAseed_coordinates[0], pg_CAseed_coordinates[1]);
	}
	else {
		glUniform4f(uniform_Update_fs_4fv_CAseed_type_size_loc[pg_ind_scenario],
			(GLfloat)pg_CAseed_type, (GLfloat)pg_CAseed_size,
			-1.f, -1.f);
	}
#endif

	// position
	for (int indPath = 0; indPath < (PG_NB_PATHS + 1); indPath++) {
		pg_path_data_Update[indPath * PG_MAX_PATH_DATA * 4 + PG_PATH_P_X * 4 + 0] = pg_paths_currentDynPoint[indPath].pg_paths_x_prev;
		pg_path_data_Update[indPath * PG_MAX_PATH_DATA * 4 + PG_PATH_P_X * 4 + 1] = pg_paths_currentDynPoint[indPath].pg_paths_xL;
		pg_path_data_Update[indPath * PG_MAX_PATH_DATA * 4 + PG_PATH_P_X * 4 + 2] = pg_paths_currentDynPoint[indPath].pg_paths_xR;
		pg_path_data_Update[indPath * PG_MAX_PATH_DATA * 4 + PG_PATH_P_Y * 4 + 0] = pg_paths_currentDynPoint[indPath].pg_paths_y_prev;
		pg_path_data_Update[indPath * PG_MAX_PATH_DATA * 4 + PG_PATH_P_Y * 4 + 1] = pg_paths_currentDynPoint[indPath].pg_paths_yL;
		pg_path_data_Update[indPath * PG_MAX_PATH_DATA * 4 + PG_PATH_P_Y * 4 + 2] = pg_paths_currentDynPoint[indPath].pg_paths_yR;
	}
	for (int indPath = 0; indPath < fingers; indPath++) {
		pg_path_data_Update[indPath * PG_MAX_PATH_DATA * 4 + PG_PATH_P_X * 4 + 3] = pg_paths_currentDynPoint[indPath].pg_paths_x_forGPU;
		pg_path_data_Update[indPath * PG_MAX_PATH_DATA * 4 + PG_PATH_P_Y * 4 + 3] = pg_paths_currentDynPoint[indPath].pg_paths_y_forGPU;
	}
	for (int indPath = fingers; indPath < (PG_NB_PATHS + 1); indPath++) {
		pg_path_data_Update[indPath * PG_MAX_PATH_DATA * 4 + PG_PATH_P_X * 4 + 3] = pg_paths_currentDynPoint[indPath].pg_paths_x;
		pg_path_data_Update[indPath * PG_MAX_PATH_DATA * 4 + PG_PATH_P_Y * 4 + 3] = pg_paths_currentDynPoint[indPath].pg_paths_y;
	}
	// color, radius, beginning or end of a stroke
	for (int indPath = 0; indPath < (PG_NB_PATHS + 1); indPath++) {
		pg_path_data_Update[indPath * PG_MAX_PATH_DATA * 4 + PG_PATH_COLOR * 4 + 0] = pg_paths_currentDynPoint[indPath].pg_paths_Color_r;
		pg_path_data_Update[indPath * PG_MAX_PATH_DATA * 4 + PG_PATH_COLOR * 4 + 1] = pg_paths_currentDynPoint[indPath].pg_paths_Color_g;
		pg_path_data_Update[indPath * PG_MAX_PATH_DATA * 4 + PG_PATH_COLOR * 4 + 2] = pg_paths_currentDynPoint[indPath].pg_paths_Color_b;
		pg_path_data_Update[indPath * PG_MAX_PATH_DATA * 4 + PG_PATH_COLOR * 4 + 3] = pg_paths_currentDynPoint[indPath].pg_paths_Color_a;
		pg_path_data_Update[indPath * PG_MAX_PATH_DATA * 4 + PG_PATH_RADIUS_BEGINEND * 4 + 0] = pg_paths_currentDynPoint[indPath].pg_paths_RadiusX;
		pg_path_data_Update[indPath * PG_MAX_PATH_DATA * 4 + PG_PATH_RADIUS_BEGINEND * 4 + 1] = (pg_paths_currentDynPoint[indPath].pg_paths_isBegin ? 1.f : (pg_paths_currentDynPoint[indPath].pg_paths_isEnd ? -1.f : 0.f));
		pg_path_data_Update[indPath * PG_MAX_PATH_DATA * 4 + PG_PATH_RADIUS_BEGINEND * 4 + 2] = pg_paths_currentDynPoint[indPath].pg_paths_RadiusY;
		pg_path_data_Update[indPath * PG_MAX_PATH_DATA * 4 + PG_PATH_RADIUS_BEGINEND * 4 + 3] = float(pg_paths_currentDynPoint[indPath].pg_paths_BrushID);
	}
	//printf("Path radius (%d) %.1f %.1f brush ID %.0f xy_GPU %.1f %.1f Color RGBA %.1f %.1f %.1f %.1f \n", 0,
	//	pg_paths_currentDynPoint[0].pg_paths_RadiusX, pg_paths_currentDynPoint[0].pg_paths_RadiusY, float(pg_paths_currentDynPoint[0].pg_paths_BrushID),
	//	pg_paths_currentDynPoint[0].pg_paths_x_forGPU, pg_paths_currentDynPoint[0].pg_paths_y_forGPU,
	//	pg_paths_currentDynPoint[0].pg_paths_Color_r, pg_paths_currentDynPoint[0].pg_paths_Color_g, pg_paths_currentDynPoint[0].pg_paths_Color_b, pg_paths_currentDynPoint[0].pg_paths_Color_a);
	//printf("Bezier in data : %.1fx%.1f %.1fx%.1f %.1fx%.1f %.1fx%.1f \n",
	//	pg_paths_currentDynPoint[0].pg_paths_x_prev, pg_paths_currentDynPoint[0].pg_paths_y_prev,
	//	pg_paths_currentDynPoint[0].pg_paths_xL, pg_paths_currentDynPoint[0].pg_paths_yL,
	//	pg_paths_currentDynPoint[0].pg_paths_xR, pg_paths_currentDynPoint[0].pg_paths_yR,
	//	pg_paths_currentDynPoint[0].pg_paths_x, pg_paths_currentDynPoint[0].pg_paths_y);
	// printf("Radius %.2f %.2f\n", pg_paths_currentDynPoint[0].pg_paths_RadiusX, pg_paths_currentDynPoint[0].pg_paths_RadiusY);
	//printf("pos %.1f %.1f %.1f %.1f %.1f %.1f %.1f %.1f \n", pg_paths_currentDynPoint[1].pg_paths_x_prev, pg_paths_currentDynPoint[1].pg_paths_y_prev,
	//	pg_paths_currentDynPoint[1].pg_paths_xR, pg_paths_currentDynPoint[1].pg_paths_yR, pg_paths_currentDynPoint[1].pg_paths_xL, pg_paths_currentDynPoint[1].pg_paths_yL, pg_paths_currentDynPoint[1].pg_paths_x, pg_paths_currentDynPoint[1].pg_paths_y);
	for (int indPath = 0; indPath < (PG_NB_PATHS + 1); indPath++) {
		pg_build_Bezier_bounding_box(indPath);
	}

	// bounding box containing the bezier curve augmented by its radius
	for (int indPath = 0; indPath < (PG_NB_PATHS + 1); indPath++) {
		pg_path_data_Update[indPath * PG_MAX_PATH_DATA * 4 + PG_PATH_BOX * 4 + 0] = pg_BezierBox[indPath].x;
		pg_path_data_Update[indPath * PG_MAX_PATH_DATA * 4 + PG_PATH_BOX * 4 + 1] = pg_BezierBox[indPath].y;
		pg_path_data_Update[indPath * PG_MAX_PATH_DATA * 4 + PG_PATH_BOX * 4 + 2] = pg_BezierBox[indPath].z;
		pg_path_data_Update[indPath * PG_MAX_PATH_DATA * 4 + PG_PATH_BOX * 4 + 3] = pg_BezierBox[indPath].w;
		//if (indPath == 1) {
		//	printf("bounding box %.1f %.1f %.1f %.1f\n", 
		//		pg_path_data_Update[indPath * PG_MAX_PATH_DATA * 4 + PG_PATH_BOX * 4 + 0],
		//		pg_path_data_Update[indPath * PG_MAX_PATH_DATA * 4 + PG_PATH_BOX * 4 + 1],
		//		pg_path_data_Update[indPath * PG_MAX_PATH_DATA * 4 + PG_PATH_BOX * 4 + 2],
		//		pg_path_data_Update[indPath * PG_MAX_PATH_DATA * 4 + PG_PATH_BOX * 4 + 3]);
		//}
	}

	//printf("pos %.1f %.1f %.1f %.1f bez %.1f %.1f %.1f %.1f beging./end %d %d \n", pg_paths_currentDynPoint[1].pg_paths_Color_r, pg_paths_currentDynPoint[1].pg_paths_Color_g, pg_paths_currentDynPoint[1].pg_paths_Color_b, pg_paths_currentDynPoint[1].pg_paths_Color_a,
	//	pg_BezierBox[1].x, pg_BezierBox[1].y, pg_BezierBox[1].z, pg_BezierBox[1].w, pg_paths_currentDynPoint[1].pg_paths_isBegin, pg_paths_currentDynPoint[1].pg_paths_isEnd);
	glUniform4fv(uniform_Update_path_data[pg_ind_scenario], (PG_NB_PATHS + 1) * PG_MAX_PATH_DATA, pg_path_data_Update);

	if (pg_FullScenarioActiveVars[pg_ind_scenario][_soundtrack_JUCE_weight]) {
		glUniform4f(uniform_Update_fs_4fv_fftLevels03[pg_ind_scenario],
			fftLevels[0], fftLevels[1], fftLevels[2], fftLevels[3]);
		glUniform4f(uniform_Update_fs_4fv_fftFrequencies03[pg_ind_scenario],
			fftFrequencies[0], fftFrequencies[1], fftFrequencies[2], fftFrequencies[3]);
		glUniform4f(uniform_Update_fs_4fv_fftPhases03[pg_ind_scenario],
			fftPhases[0], fftPhases[1], fftPhases[2], fftPhases[3]);
		glUniform4f(uniform_Update_fs_4fv_fftLevels47[pg_ind_scenario],
			fftLevels[4], fftLevels[5], fftLevels[6], fftLevels[7]);
		glUniform4f(uniform_Update_fs_4fv_fftFrequencies47[pg_ind_scenario],
			fftFrequencies[4], fftFrequencies[5], fftFrequencies[6], fftFrequencies[7]);
		glUniform4f(uniform_Update_fs_4fv_fftPhases47[pg_ind_scenario],
			fftPhases[4], fftPhases[5], fftPhases[6], fftPhases[7]);
		//printf("Freq %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f\n", fftFrequencies[0], fftFrequencies[1], fftFrequencies[2], fftFrequencies[3], fftFrequencies[4], fftFrequencies[5], fftFrequencies[6], fftFrequencies[7]);
		//printf("Levels %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f\n", fftLevels[0], fftLevels[1], fftLevels[2], fftLevels[3], fftLevels[4], fftLevels[5], fftLevels[6], fftLevels[7]);
		//printf("Phases %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f\n", fftPhases[0], fftPhases[1], fftPhases[2], fftPhases[3], fftPhases[4], fftPhases[5], fftPhases[6], fftPhases[7]);
		//printf("Freq %.2f\n", fftFrequencies[0]);
		//printf("Levels %.2f\n", fftLevels[0]);
		//printf("Phase %.2f\n", fftPhases[0]);
	}

	// flash BG weights
	glUniform4f(uniform_Update_fs_4fv_flashTrkBGWghts_flashPartBGWght[pg_ind_scenario],
		pg_flashTrkBG_weights[1], pg_flashTrkBG_weights[2], pg_flashTrkBG_weights[3], pg_flashPartBG_weight);

	// flash Trk -> CA weights
	glUniform4f(uniform_Update_fs_4fv_flashTrkCAWghts[pg_ind_scenario],
		pg_flashTrkCA_weights[0], pg_flashTrkCA_weights[1], pg_flashTrkCA_weights[2], pg_flashTrkCA_weights[3]);
	// printf("pg_flashTrkCA_weights %.2f %.2f %.2f %.2f \n", pg_flashTrkCA_weights[0], pg_flashTrkCA_weights[1], pg_flashTrkCA_weights[2], pg_flashTrkCA_weights[3]);

	//printf("Signs %d \n", currentDrawingTrack);

	// CA type, frame no, flashback and current track
	glUniform4f(uniform_Update_fs_4fv_frameno_Cursor_flashPartCAWght_doubleWindow[pg_ind_scenario],
		(GLfloat)pg_FrameNo,
		(GLfloat)pg_CurrentStylus_StylusvsRubber, pg_flashPartCA_weight, (GLfloat)double_window);

	// movie size, flash camera and copy tracks
	// copy to layer above (+1) or to layer below (-1)
	glUniform4f(uniform_Update_fs_4fv_movieWH_flashCameraTrkWght_cpTrack[pg_ind_scenario],
		(GLfloat)pg_movie_frame_width, (GLfloat)pg_movie_frame_height,
		pg_flashCameraTrk_weight, (GLfloat)pg_copyToNextTrack);

	// printf("Movie size %dx%d\n", pg_movie_frame_width, pg_movie_frame_height);
	// printf("Flash camera coef %.1f\n", pg_flashCameraTrk_weight);
	if (pg_copyToNextTrack != 0)
		printf("pg_copyToNextTrack %d\n", pg_copyToNextTrack);

	// photo flash
	photo_offsetX = 0;
	photo_offsetY = 0;
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_photoJitterAmpl]
		&& pg_FullScenarioActiveVars[pg_ind_scenario][_photo_offsetX]
		&& pg_FullScenarioActiveVars[pg_ind_scenario][_photo_offsetY]) {
		float angle = photoJitterAmpl * (pg_audio_pulse[0] - 0.5f) * float(M_PI);
		float radius = photoJitterAmpl * (pg_audio_pulse[2] - 0.5f);
		// offset for POT coordinates
		photo_offsetX = radius * cos(angle) / pg_workingWindow_width;
		photo_offsetY = radius * sin(angle) / PG_WINDOW_HEIGHT;
		glUniform4f(uniform_Update_fs_4fv_flashPhotoTrkWght_flashPhotoTrkThres_Photo_offSetsXY[pg_ind_scenario],
			pg_flashPhotoTrk_weight, pg_flashPhotoTrk_threshold, photo_offsetX, photo_offsetY);
	}

	// flash CA -> BG & repop color (BG & CA)
	glUniform4f(uniform_Update_fs_4fv_repop_ColorBG_flashCABGWght[pg_ind_scenario],
		pg_repop_ColorBGcolorRed, pg_repop_ColorBG_g, pg_repop_ColorBG_b, pg_flashCABG_weight);

#if !defined(var_alKemi)
	glUniform3f(uniform_Update_fs_3fv_repop_ColorCA[pg_ind_scenario],
		pg_repop_ColorCA_r, pg_repop_ColorCA_g, pg_repop_ColorCA_b);
#else
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_alKemi] == false) {
		glUniform3f(uniform_Update_fs_3fv_repop_ColorCA[pg_ind_scenario],
			pg_repop_ColorCA_r, pg_repop_ColorCA_g, pg_repop_ColorCA_b);
	}
#endif

	// clear layer, flash pixel, flash CA -> Part
	glUniform3f(uniform_Update_fs_3fv_isClearLayer_flashPixel_flashCameraTrkThres[pg_ind_scenario],
		(GLfloat)pg_isClearLayer, (GLfloat)pg_flashPixel, pg_flashCameraTrk_threshold);
	// photo rendering
	if (photo_diaporama >= 0) {
		glUniform4f(uniform_Update_fs_4fv_photo01_wh[pg_ind_scenario],
			pg_workingWindow_width_powerOf2_ratio, pg_window_height_powerOf2_ratio,
			pg_workingWindow_width_powerOf2_ratio, pg_window_height_powerOf2_ratio);
		//printf("photo WH %.2fx%.2f %.2fx%.2f\n",
	//	pg_workingWindow_width_powerOf2_ratio, window_height_powerOf2_ratio,
	//	pg_workingWindow_width_powerOf2_ratio, window_height_powerOf2_ratio);

		glUniform4f(uniform_Update_fs_4fv_photo01Wghts_randomValues[pg_ind_scenario],
			pg_Photo_weight[0], pg_Photo_weight[1], rand_0_1, rand_0_1);
		//glUniform4f(uniform_Update_fs_4fv_photo01Wghts_randomValues[pg_ind_scenario],
		//	0.f, 1.f, rand_0_1, rand_0_1);
		//printf("photo weight %.2f %.2f phot index %d %d\n", pg_Photo_weight[0], pg_Photo_weight[1],
			//pg_Photo_swap_buffer_data[0].indSwappedPhoto,
			//pg_Photo_swap_buffer_data[1].indSwappedPhoto);
	}
	else {
		glUniform4f(uniform_Update_fs_4fv_photo01Wghts_randomValues[pg_ind_scenario],
			0.f, 0.f, rand_0_1, rand_0_1);
		//printf("photo weight diaporama -1 0.f 0.f\n");
	}

	// clip rendering
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_clip_mix]
		&& photo_diaporama < 0) {
		// clip weights 
		if (PG_NB_PARALLEL_CLIPS >= 2) {
			if (pg_FullScenarioActiveVars[pg_ind_scenario][_clipCaptFreq]) {
				// 4 clips play
				if ((pg_playing_clipNoLeft >= 0 && pg_playing_clipNoLeft < pg_nbClips[pg_ind_scenario])
					|| (pg_playing_clipNoRight >= 0 && pg_playing_clipNoRight < pg_nbClips[pg_ind_scenario])
					|| (pg_playing_secondClipNoLeft >= 0 && pg_playing_secondClipNoLeft < pg_nbClips[pg_ind_scenario])
					|| (pg_playing_secondClipNoRight >= 0 && pg_playing_secondClipNoRight < pg_nbClips[pg_ind_scenario])) {
					glUniform4f(uniform_Update_fs_4fv_photo01_wh[pg_ind_scenario],
						float(clip_crop_width[pg_ind_scenario]) / clip_image_width[pg_ind_scenario], float(clip_crop_height[pg_ind_scenario]) / clip_image_height[pg_ind_scenario],
						float(clip_crop_width[pg_ind_scenario]) / clip_image_width[pg_ind_scenario], float(clip_crop_height[pg_ind_scenario]) / clip_image_height[pg_ind_scenario]);
					float wl0 = 0.f; // weight of first left clip
					float wr0 = 0.f; // weight of first right clip
					float wl1 = 0.f; // weight of second left clip
					float wr1 = 0.f; // weight of second right clip
					// valid first left clip
					if (pg_playing_clipNoLeft >= 0 && pg_playing_clipNoLeft < pg_nbClips[pg_ind_scenario]) {
						wl0 = 1.f;
					}
					// valid first right clip
					if (pg_playing_clipNoRight >= 0 && pg_playing_clipNoRight < pg_nbClips[pg_ind_scenario]) {
						wr0 = 1.f;
					}
					// valid second left clip
					if (pg_playing_secondClipNoLeft >= 0 && pg_playing_secondClipNoLeft < pg_nbClips[pg_ind_scenario]) {
						wl1 = 1.f;
					}
					// valid second right clip
					if (pg_playing_secondClipNoRight >= 0 && pg_playing_secondClipNoRight < pg_nbClips[pg_ind_scenario]) {
						wr1 = 1.f;
					}
					//printf("wl0 %.2f wl1 %.2f\n", wl0, wl1);
					// mixing fader in the middle
					// first and second clips are mixed fully
					// the right and left levels are used for lateral mixing
					if (clip_mix == 0) {
						glUniform4f(uniform_Update_fs_4fv_photo01Wghts_randomValues[pg_ind_scenario],
							wl0 * pg_all_clip_status[pg_enum_clipLeft].clip_level[0], wr0 * pg_all_clip_status[pg_enum_clipRight].clip_level[0],
							rand_0_1, rand_0_1);
						glUniform2f(uniform_Update_fs_2fv_clip01Wghts[pg_ind_scenario],
							wl1 * pg_all_clip_status[pg_enum_clipLeft].clip_level[1], wr1 * pg_all_clip_status[pg_enum_clipRight].clip_level[1]);
						//printf("neut wl0 %.2f wl1 %.2f\n", wl0, wl1);
					}
					// second clips are mixed fully
					// first clip progressively decrease
					// the right and left levels are used for lateral mixing
					else if (clip_mix < 0) {
						clip_mix = max(clip_mix, -0.5f);
						glUniform4f(uniform_Update_fs_4fv_photo01Wghts_randomValues[pg_ind_scenario],
							wl0 * (1 - 2 * -clip_mix) * pg_all_clip_status[pg_enum_clipLeft].clip_level[0], wr0 * (1 - 2 * -clip_mix) * pg_all_clip_status[pg_enum_clipRight].clip_level[0],
							rand_0_1, rand_0_1);
						glUniform2f(uniform_Update_fs_2fv_clip01Wghts[pg_ind_scenario],
							wl1 * pg_all_clip_status[pg_enum_clipLeft].clip_level[1], wr1 * pg_all_clip_status[pg_enum_clipRight].clip_level[1]);
						//printf("neg %.2f wl0 %.2f wl1 %.2f\n", clip_mix, wl0 * (1 - 2 * -clip_mix), wl1);
					}
					// first clips are mixed fully
					// second lip progressively decrease
					// the right and left levels are used for lateral mixing
					else if (clip_mix > 0) {
						clip_mix = min(clip_mix, 0.5f);
						glUniform4f(uniform_Update_fs_4fv_photo01Wghts_randomValues[pg_ind_scenario],
							wl0 * pg_all_clip_status[pg_enum_clipLeft].clip_level[0], wr0 * pg_all_clip_status[pg_enum_clipRight].clip_level[0],
							rand_0_1, rand_0_1);
						glUniform2f(uniform_Update_fs_2fv_clip01Wghts[pg_ind_scenario],
							wl1 * (1 - 2 * clip_mix) * pg_all_clip_status[pg_enum_clipLeft].clip_level[1], wr1 * (1 - 2 * clip_mix) * pg_all_clip_status[pg_enum_clipRight].clip_level[1]);
						//printf("pos %.2f wl0 %.2f wl1 %.2f\n", clip_mix, wl0, wl1 * (1 - 2 * clip_mix));
					}
				}
			}
		}
		else {
			if (pg_FullScenarioActiveVars[pg_ind_scenario][_clipCaptFreq]) {
				// 2 clips play
				if ((pg_playing_clipNoLeft >= 0 && pg_playing_clipNoLeft < pg_nbClips[pg_ind_scenario])
					|| (pg_playing_clipNoRight >= 0 && pg_playing_clipNoRight < pg_nbClips[pg_ind_scenario])) {
					glUniform4f(uniform_Update_fs_4fv_photo01_wh[pg_ind_scenario],
						float(clip_crop_width[pg_ind_scenario]) / clip_image_width[pg_ind_scenario], float(clip_crop_height[pg_ind_scenario]) / clip_image_height[pg_ind_scenario],
						float(clip_crop_width[pg_ind_scenario]) / clip_image_width[pg_ind_scenario], float(clip_crop_height[pg_ind_scenario]) / clip_image_height[pg_ind_scenario]);
					//printf("photo WH %.2fx%.2f %.2fx%.2f\n",
					//	pg_workingWindow_width_powerOf2_ratio, window_height_powerOf2_ratio,
					//	pg_workingWindow_width_powerOf2_ratio, window_height_powerOf2_ratio);
					if (!(pg_playing_clipNoLeft >= 0 && pg_playing_clipNoLeft < pg_nbClips[pg_ind_scenario])) {
						glUniform4f(uniform_Update_fs_4fv_photo01Wghts_randomValues[pg_ind_scenario],
							0.0, pg_all_clip_status[pg_enum_clipRight].clip_level[0], rand_0_1, rand_0_1);
						//printf("clip weights %.2f %.2f\n", 0.0, pg_all_clip_status[pg_enum_clipRight].clip_level[0]);
					}
					else if (!(pg_playing_clipNoRight >= 0 && pg_playing_clipNoRight < pg_nbClips[pg_ind_scenario])) {
						glUniform4f(uniform_Update_fs_4fv_photo01Wghts_randomValues[pg_ind_scenario],
							pg_all_clip_status[pg_enum_clipLeft].clip_level[1], 0.0, rand_0_1, rand_0_1);
						//printf("clip weights %.2f %.2f\n", pg_all_clip_status[pg_enum_clipLeft].clip_level1[1], 0.0);
					}
				}
			}
		}
	}

	//printf("camera movie weight %.2f %.2f\n",
	//	*((float *)pg_FullScenarioVarPointers[_cameraWeight]), *((float *)pg_FullScenarioVarPointers[_movieWeight]));

	if (pg_FullScenarioActiveVars[pg_ind_scenario][_cameraCaptFreq]) {
		// camera texture offset 
		glUniform4f(uniform_Update_fs_4fv_Camera_offSetsXY_Camera_W_H[pg_ind_scenario],
			(GLfloat)pg_camera_x_offset, (GLfloat)pg_camera_y_offset,
			(GLfloat)pg_camera_frame_width, (GLfloat)pg_camera_frame_height);
		//printf("Update shader camera frame size %dx%d offset %dx%d\n", pg_camera_frame_width, pg_camera_frame_height, pg_camera_x_offset, pg_camera_y_offset);
	}

	if (pg_FullScenarioActiveVars[pg_ind_scenario][_slow_track_translation]) {
		if (slow_track_translation) {
			// track x & y translations
			float translation_x[2] = { 0.f, 0.f };
			float translation_y[2] = { 0.f, 0.f };
			translation_x[0] = pg_frame_based_translation(track_x_transl[0], 0);
			translation_y[0] = pg_frame_based_translation(track_y_transl[0], 1);
			translation_x[1] = pg_frame_based_translation(track_x_transl[1], 2);
			translation_y[1] = pg_frame_based_translation(track_y_transl[1], 3);
			glUniform4f(uniform_Update_fs_4fv_xy_transl_tracks_0_1[pg_ind_scenario],
				translation_x[0], translation_y[0], translation_x[1], translation_y[1]);
		}
		else {
			glUniform4f(uniform_Update_fs_4fv_xy_transl_tracks_0_1[pg_ind_scenario],
				track_x_transl[0], track_y_transl[0], track_x_transl[1], track_y_transl[1]);
		}
	}
	else {
		glUniform4f(uniform_Update_fs_4fv_xy_transl_tracks_0_1[pg_ind_scenario],
			track_x_transl[0], track_y_transl[0], track_x_transl[1], track_y_transl[1]);
	}

	// acceleration center and CA subtype
	// in case of interpolation between CA1 and CA2 
	if (!pg_BrokenInterpolationVar[_CA1_CA2_weight]) {
		if (CA1_CA2_weight < 1.0 && CA1_CA2_weight > 0.0) {
			float randVal = rand_0_1;
			if (randVal <= CA1_CA2_weight) {
				CAInterpolatedType = CA1Type;
				CAInterpolatedSubType = CA1SubType;
			}
			else {
				CAInterpolatedType = CA2Type;
				CAInterpolatedSubType = CA2SubType;
			}
		}
		else if (CA1_CA2_weight >= 1.0) {
			CAInterpolatedType = CA1Type;
			CAInterpolatedSubType = CA1SubType;
		}
		else if (CA1_CA2_weight <= 0.0) {
			CAInterpolatedType = CA2Type;
			CAInterpolatedSubType = CA2SubType;
		}
		//printf("CA1/CA2 mix: CA type/subtype %d-%d\n" , CAInterpolatedType, CAInterpolatedSubType);
	}

#if !defined(PG_WITH_BLUR)
	glUniform4f(uniform_Update_fs_4fv_CAType_SubType_blurRadius[pg_ind_scenario],
		GLfloat(CAInterpolatedType), GLfloat(CAInterpolatedSubType),
		0.f, 0.f);
#else
	glUniform4f(uniform_Update_fs_4fv_CAType_SubType_blurRadius[pg_ind_scenario],
		GLfloat(CAInterpolatedType), GLfloat(CAInterpolatedSubType),
		(is_blur_1 ? float(blurRadius_1) : 0.f), (is_blur_2 ? float(blurRadius_2) : 0.f));
#endif
	pg_printOglError(42);

	// printf("CA type/subtype %d-%d\n" , CAInterpolatedType, CAInterpolatedSubType);
	// printf("blur %.2f %.2f\n", (is_blur_1 ? float(blurRadius_1) : 0.f), (is_blur_2 ? float(blurRadius_2) : 0.f));

	// sets the time of the 1st plane launch 
#if defined(var_GenerativeNights_planes)
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_GenerativeNights_planes]) {
		if (CAInterpolatedType == CA_NEUMANN_BINARY && firstPlaneFrameNo < 0) {
			// printf("First plane frame no %d\n", firstPlaneFrameNo);
			firstPlaneFrameNo = pg_FrameNo - 2300;
		}
		glUniform2f(uniform_Update_fs_2fv_initCA_1stPlaneFrameNo[pg_ind_scenario], initCA, GLfloat(firstPlaneFrameNo));
		// one shot CA launching
		if (initCA > 0.0f) {
			initCA = 0.0f;
		}
	}
#endif

	if (CAInterpolatedType_prev != CAInterpolatedType
		|| CAInterpolatedSubType_prev != CAInterpolatedSubType) {
		sprintf(pg_AuxString, "/CAType %d", CAInterpolatedType);
		pg_send_message_udp((char*)"i", pg_AuxString, (char*)"udp_TouchOSC_send");
		sprintf(pg_AuxString, "/CASubType %d", CAInterpolatedSubType);
		pg_send_message_udp((char*)"i", pg_AuxString, (char*)"udp_TouchOSC_send");
		CAInterpolatedType_prev = CAInterpolatedType;
		CAInterpolatedSubType_prev = CAInterpolatedSubType;
	}
	pg_printOglError(41);
}

/////////////////////////////////////////////////////////////////////////
// PARTICLE RENDERING SHADER UNIFORM VARIABLES
void pg_update_shader_ParticleRender_uniforms(void) {
	if (!pg_shader_programme[pg_ind_scenario][pg_enum_shader_ParticleRender]) {
		return;
	}
	if (particle_geometry == 2) {
		glUseProgram(pg_shader_programme[pg_ind_scenario][pg_enum_shader_ParticleRender]);
		glUniform3f(uniform_ParticleCurve_gs_3fv_partRadius_partType_highPitchPulse[pg_ind_scenario],
			(part_size + pulse_average * part_size_pulse * part_size) / 512.f,
			(GLfloat)particle_type, pg_audio_pulse[2]);
		// printf("part radius GS %.2f\n", (part_size + pulse_average * part_size_pulse * part_size));

		///////////////////////////////////////////////////////////////////////
		bool assigned = false;
		for (int indPath = 1; indPath < PG_NB_PATHS + 1; indPath++) {
			// active reading
			if (pg_is_path_replay[indPath]) {
				glUniform3f(uniform_ParticleCurve_vp_3fv_trackReplay_xy_height[pg_ind_scenario], pg_paths_currentDynPoint[indPath].pg_paths_x, pg_paths_currentDynPoint[indPath].pg_paths_y, (float)PG_WINDOW_HEIGHT);
				assigned = true;
				break;
			}
		}
		if (!assigned) {
			if (pg_paths_currentDynPoint[0].pg_paths_x >= 0 && pg_paths_currentDynPoint[0].pg_paths_y >= 0) {
				glUniform3f(uniform_ParticleCurve_vp_3fv_trackReplay_xy_height[pg_ind_scenario], pg_paths_currentDynPoint[0].pg_paths_x, pg_paths_currentDynPoint[0].pg_paths_y, (float)PG_WINDOW_HEIGHT);
			}
			else {
				glUniform3f(uniform_ParticleCurve_vp_3fv_trackReplay_xy_height[pg_ind_scenario], -1000.f, -1000.f, (float)PG_WINDOW_HEIGHT);
			}
		}
	}
	else if (particle_geometry == 0 || particle_geometry == 1) {
		glUseProgram(pg_shader_programme[pg_ind_scenario][pg_enum_shader_ParticleRender]);
		glUniform4f(uniform_ParticleSplat_gs_4fv_part_size_partType_highPitchPulse_windowRatio[pg_ind_scenario],
			(part_size + pulse_average * part_size_pulse * part_size) / 512.f,
			(GLfloat)particle_type, pg_audio_pulse[2],
			float(pg_workingWindow_width) / float(PG_WINDOW_HEIGHT));
		//printf("part size gs: %.3f\n", (part_size + pulse_average * part_size_pulse * part_size));

		///////////////////////////////////////////////////////////////////////
		bool assigned = false;
		for (int indPath = 1; indPath < PG_NB_PATHS + 1; indPath++) {
			// active reading
			if (pg_is_path_replay[indPath]) {
				glUniform3f(uniform_ParticleSplat_vp_3fv_trackReplay_xy_height[pg_ind_scenario], pg_paths_currentDynPoint[indPath].pg_paths_x, pg_paths_currentDynPoint[indPath].pg_paths_y, (float)PG_WINDOW_HEIGHT);
				assigned = true;
				break;
			}
		}
		if (!assigned) {
			if (pg_paths_currentDynPoint[0].pg_paths_x >= 0 && pg_paths_currentDynPoint[0].pg_paths_y >= 0) {
				glUniform3f(uniform_ParticleSplat_vp_3fv_trackReplay_xy_height[pg_ind_scenario], pg_paths_currentDynPoint[0].pg_paths_x, pg_paths_currentDynPoint[0].pg_paths_y, (float)PG_WINDOW_HEIGHT);
			}
			else {
				glUniform3f(uniform_ParticleSplat_vp_3fv_trackReplay_xy_height[pg_ind_scenario], -1000.f, -1000.f, (float)PG_WINDOW_HEIGHT);
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////
// MIXING SHADER UNIFORM VARIABLES
void pg_update_shader_Mixing_uniforms(void) {
	if (pg_shader_programme[pg_ind_scenario][pg_enum_shader_Mixing]) {
		glUseProgram(pg_shader_programme[pg_ind_scenario][pg_enum_shader_Mixing]);

		// CA weight
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_flashPhotoTrkBeat]
			&& pg_FullScenarioActiveVars[pg_ind_scenario][_flashPhotoTrkBright]
			&& pg_FullScenarioActiveVars[pg_ind_scenario][_flashPhotoTrkLength]
			&& pg_FullScenarioActiveVars[pg_ind_scenario][_flashPhotoChangeBeat]) {
			glUniform3f(uniform_Mixing_fs_3fv_height_flashCameraTrkWght_flashPhotoTrkWght[pg_ind_scenario],
				(GLfloat)PG_WINDOW_HEIGHT, pg_flashCameraTrk_weight, pg_flashPhotoTrk_weight);
		}
		else {
			glUniform3f(uniform_Mixing_fs_3fv_height_flashCameraTrkWght_flashPhotoTrkWght[pg_ind_scenario],
				(GLfloat)PG_WINDOW_HEIGHT, pg_flashCameraTrk_weight, 0.f);
		}

		// TEXT TRANSPARENCY
		glUniform3f(uniform_Mixing_fs_3fv_screenMsgTransp_Text1_2_Alpha[pg_ind_scenario],
			pg_messageTransparency,
			(GLfloat)1.f, (GLfloat)0.f);
	}
	pg_printOglError(39);
}

/////////////////////////////////////////////////////////////////////////
// MASTER SHADER UNIFORM VARIABLES
void pg_update_shader_Master_uniforms(void) {
	if (pg_shader_programme[pg_ind_scenario][pg_enum_shader_Master]) {
		glUseProgram(pg_shader_programme[pg_ind_scenario][pg_enum_shader_Master]);
		// hoover cursor
		mobile_cursor = ((pg_tablet_prec_x != pg_CurrentStylusHooverPos_x) || (pg_tablet_prec_y != pg_CurrentStylusHooverPos_y));
		pg_tablet_prec_x = pg_CurrentStylusHooverPos_x;
		pg_tablet_prec_y = pg_CurrentStylusHooverPos_y;
		//printf("cur %d %d frame %d mobile %d\n", pg_CurrentStylusHooverPos_x, pg_CurrentStylusHooverPos_y, pg_FrameNo, mobile_cursor);
		glUniform4f(uniform_Master_fs_4fv_xy_frameno_pulsedShift[pg_ind_scenario],
			(GLfloat)pg_CurrentStylusHooverPos_x, (GLfloat)pg_CurrentStylusHooverPos_y,
			(GLfloat)pg_FrameNo, (pulse_average - pg_audio_pulse_average_prec) * track_x_transl_pulse[0]);

		// screen size
		glUniform4f(uniform_Master_fs_4fv_width_height_timeFromStart_muteRightScreen[pg_ind_scenario],
			(GLfloat)pg_workingWindow_width, (GLfloat)PG_WINDOW_HEIGHT, GLfloat(pg_CurrentClockTime - pg_InitialScenarioTime),
			(GLfloat)pg_muteRightScreen);

		// printf("mobile cursor %d\n", (mobile_cursor ? 1 : 0));
		glUniform2i(uniform_Master_fs_2iv_mobile_cursor_currentScene[pg_ind_scenario], (mobile_cursor ? 1 : 0), pg_CurrentSceneIndex);
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_BGcolor]
			|| pg_FullScenarioActiveVars[pg_ind_scenario][_flashchange_BGcolor_freq]) {
			// high bandpass color
			glUniform3f(uniform_Master_fs_3fv_BGcolor_rgb[pg_ind_scenario],
				BGcolorRed, BGcolorGreen, BGcolorBlue);
		}
	}
}

/////////////////////////////////////////////////////////////////////////
// MESH SHADER UNIFORM VARIABLES
void pg_update_shader_Mesh_uniforms(void) {
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_activeMeshes]
		&& pg_shader_programme[pg_ind_scenario][pg_enum_shader_Mesh]) {
		/////////////////////////////////////////////////////////////////////////
		// MESH SHADER UNIFORM VARIABLES
		glUseProgram(pg_shader_programme[pg_ind_scenario][pg_enum_shader_Mesh]);
		// the variable of the mesh shader is updated before each rendering mode (lines of facets)
		glUniform3f(uniform_Mesh_fs_3fv_light[pg_ind_scenario], pg_mesh_light_x, pg_mesh_light_y, pg_mesh_light_z);
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_mesh_expand]) {
			glUniform2f(uniform_Mesh_vp_2fv_dilate_explode[pg_ind_scenario], mesh_expand + pulse_average * mesh_expand_pulse, mesh_explode);
		}
		//printf("mesh %.2f %.2f\n", mesh_expand + pulse_average * mesh_expand_pulse, mesh_explode);

		if (pg_FullScenarioActiveVars[pg_ind_scenario][_textureFrontier_wmin]) {
			glUniform4f(uniform_Mesh_fs_4fv_textureFrontier[pg_ind_scenario], textureFrontier_wmin, textureFrontier_wmax, textureFrontier_hmin, textureFrontier_hmax);
			glUniform4f(uniform_Mesh_fs_4fv_textureFrontier_width[pg_ind_scenario], textureFrontier_wmin_width, textureFrontier_wmax_width, textureFrontier_hmin_width, textureFrontier_hmax_width);
			glUniform4f(uniform_Mesh_fs_4fv_textureScaleTransl[pg_ind_scenario], textureScale_w, textureScale_h, textureTranslate_w, textureTranslate_h);
		}
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_mesh_palette]) {
			float pulsed_color[3];
			pg_compute_pulsed_palette_color(mesh_color, pen_color_pulse, mesh_grey, pen_grey_pulse, pulsed_color, pg_enum_PEN_COLOR);
			glUniform4f(uniform_Mesh_fs_4fv_color_palette[pg_ind_scenario], pulsed_color[0], pulsed_color[1], pulsed_color[2], float(mesh_palette));
		}
	}
	pg_printOglError(40);
}

void pg_update_shader_uniforms(void) {
	pg_update_shader_var_data();
	pg_update_shader_ParticleAnimation_uniforms();
	pg_update_shader_Update_uniforms();
	pg_update_shader_Mixing_uniforms();
	pg_update_shader_Master_uniforms();
	pg_update_shader_ParticleRender_uniforms();
	// no update for uniforms in Sensor and ClipArt shaders
	pg_update_shader_Mesh_uniforms();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// SCENE VARIABLES RESET AT EACH FRAME
//////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
// restores variables to 0 or false so that
// they do not stay on more than one frame (flashes, initialization...)
void pg_automatic_var_reset_or_update(void) {
	///////////////////////////////////////////////////////////////////////
	// flash reset: restores flash to 0 so that
	// it does not stay on more than one frame 
	for (int indtrack = 0; indtrack < PG_NB_TRACKS; indtrack++) {
		if (pg_flashTrkCA_weights_duration[indtrack] > 1) {
			pg_flashTrkCA_weights_duration[indtrack]--;
		}
		else {
			pg_flashTrkCA_weights[indtrack] = 0;
			pg_flashTrkCA_weights_duration[indtrack] = 0;
		}
		if (pg_flashTrkBG_weights_duration[indtrack] > 1) {
			pg_flashTrkBG_weights_duration[indtrack]--;
		}
		else {
			pg_flashTrkBG_weights[indtrack] = 0;
			pg_flashTrkBG_weights_duration[indtrack] = 0;
		}
		if (pg_flashTrkPart_weights_duration[indtrack] > 1) {
			pg_flashTrkPart_weights_duration[indtrack]--;
		}
		else {
			pg_flashTrkPart_weights[indtrack] = 0;
			pg_flashTrkPart_weights_duration[indtrack] = 0;
		}
	}

	if (pg_flashPixel > 0) {
		pg_flashPixel -= 1;
	}
	if (pg_flashCABG_weight_duration > 1) {
		pg_flashCABG_weight_duration--;
	}
	else {
		pg_flashCABG_weight_duration = 0;
		pg_flashCABG_weight = 0;
	}
	if (pg_flashCAPart_weight_duration > 1) {
		pg_flashCAPart_weight_duration--;
	}
	else {
		pg_flashCAPart_weight_duration = 0;
		pg_flashCAPart_weight = 0;
	}
	if (pg_flashPartBG_weight_duration > 1) {
		pg_flashPartBG_weight_duration--;
	}
	else {
		pg_flashPartBG_weight_duration = 0;
		pg_flashPartBG_weight = 0;
	}
	if (pg_flashPartCA_weight_duration > 1) {
		pg_flashPartCA_weight_duration--;
	}
	else {
		pg_flashPartCA_weight_duration = 0;
		pg_flashPartCA_weight = 0;
	}

	if (pg_FullScenarioActiveVars[pg_ind_scenario][_master_scale]
		&& pg_FullScenarioActiveVars[pg_ind_scenario][_master_offsetX]
		&& pg_FullScenarioActiveVars[pg_ind_scenario][_photo_gamma]
		&& pg_FullScenarioActiveVars[pg_ind_scenario][_photo_satur]
		&& pg_FullScenarioActiveVars[pg_ind_scenario][_photo_threshold]) {
		if (pg_flashMaster > 0) {
			pg_flashMaster--;
			// end of flash return to dark
			if (pg_CurrentScene && pg_flashMaster == 0) {
				master = 0.f;
				*((float*)pg_FullScenarioVarPointers[_master]) = master;
				master_scale = float(pg_CurrentScene->scene_initial_parameters[_master_scale].val_num);
				*((float*)pg_FullScenarioVarPointers[_master_scale]) = master_scale;
				master_offsetX = float(pg_CurrentScene->scene_initial_parameters[_master_offsetX].val_num);
				*((float*)pg_FullScenarioVarPointers[_master_offsetX]) = master_offsetX;
				master_offsetY = float(pg_CurrentScene->scene_initial_parameters[_master_offsetY].val_num);
				*((float*)pg_FullScenarioVarPointers[_master_offsetY]) = master_offsetY;
				photo_threshold = float(pg_CurrentScene->scene_initial_parameters[_photo_threshold].val_num);
				*((float*)pg_FullScenarioVarPointers[_photo_threshold]) = photo_threshold;
				photo_gamma = float(pg_CurrentScene->scene_initial_parameters[_photo_gamma].val_num);
				*((float*)pg_FullScenarioVarPointers[_photo_gamma]) = photo_gamma;
				photo_satur = float(pg_CurrentScene->scene_initial_parameters[_photo_satur].val_num);
				*((float*)pg_FullScenarioVarPointers[_photo_satur]) = photo_satur;
				invertAllLayers = bool(pg_CurrentScene->scene_initial_parameters[_invertAllLayers].val_num);
				*((bool*)pg_FullScenarioVarPointers[_invertAllLayers]) = invertAllLayers;
			}
		}
	}

	////////////////////////////
	// flash camera reset
	if (pg_flashCameraTrk_weight > 0.0f) {
		if (pg_flashCameraTrk_weight - pg_flashCameraTrk_decay > 0) {
			pg_flashCameraTrk_weight -= pg_flashCameraTrk_decay;
			//printf("flash camera weight %.3f\n", pg_flashCameraTrk_weight);
		}
		else {
			pg_flashCameraTrk_weight = 0.0f;
			//printf("end of flash camera weight %.3f\n", pg_flashCameraTrk_weight);
		}
	}

	if (pg_FullScenarioActiveVars[pg_ind_scenario][_flashPhotoTrkBeat]
		&& pg_FullScenarioActiveVars[pg_ind_scenario][_flashPhotoTrkBright]
		&& pg_FullScenarioActiveVars[pg_ind_scenario][_flashPhotoTrkLength]
		&& pg_FullScenarioActiveVars[pg_ind_scenario][_flashPhotoChangeBeat]) {
		// flash photo reset
		if (pg_flashPhotoTrk_weight > 0.0f) {
			if (pg_flashPhotoTrk_weight > 0) {
				pg_flashPhotoTrk_weight -= pg_flashPhotoTrk_decay;
				if (pg_flashPhotoTrk_weight < 0) {
					pg_flashPhotoTrk_weight = 0.f;
				}
				pg_flashPhotoTrk_nbFrames++;
			}
			else {
				pg_flashPhotoTrk_weight = 0.0f;
				//printf("end of flash photo weight %.3f\n", pg_flashPhotoTrk_weight);
				pg_flashPhotoTrk_nbFrames = 0;
			}
		}
	}

	if (photo_diaporama >= 0 && pg_CurrentDiaporamaEnd > 0) {
		if (pg_CurrentDiaporamaEnd < pg_CurrentClockTime) {
			printf("end of flash_photo_diaporama %d\n", pg_FrameNo);
			pg_CurrentDiaporamaEnd = -1;
			photo_diaporama = -1;
			photoWeight = 0.f;
			sprintf(pg_AuxString, "/diaporama_shortName ---"); pg_send_message_udp((char*)"s", pg_AuxString, (char*)"udp_TouchOSC_send");
		}
	}

	// /////////////////////////
	// clear layer reset
	// does not reset if camera capture is still ongoing
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_cameraCaptFreq]
		&& pg_FullScenarioActiveVars[pg_ind_scenario][_camera_BG_subtr]) {
		if ((reset_camera || (pg_initialSecondBGCapture == 1)) && (pg_initialBGCapture || pg_secondInitialBGCapture)) {
			pg_isClearAllLayers = 0.f;
		}
	}

	// clear 3 frames
	// clear CA reset 
	// clear layer reset
	// clear all layers reset
	// clear echo reset
	(pg_isClearCA > 0.f ? pg_isClearCA -= 0.35f : pg_isClearCA = 0.f);
	(pg_isClearLayer > 0.f ? pg_isClearLayer -= 0.35f : pg_isClearLayer = 0.f);
	(pg_isClearAllLayers > 0.f ? pg_isClearAllLayers -= 0.35f : pg_isClearAllLayers = 0.f);
	(pg_isClearEcho > 0.f ? pg_isClearEcho -= 0.35f : pg_isClearEcho = 0.f);

	// layer copy reset	
	// copy to layer above (+1) or to layer below (-1)
	pg_copyToNextTrack = 0;

	if (pg_FullScenarioActiveVars[pg_ind_scenario][_cameraCaptFreq]) {
		// DELAYED CAMERA WEIGHT COUNTDOWN
		if (pg_delayedCameraWeight > 1) {
			pg_delayedCameraWeight--;
		}
		else if (pg_delayedCameraWeight == 1) {
			*((float*)pg_FullScenarioVarPointers[_cameraWeight]) = 1;
			pg_BrokenInterpolationVar[_cameraWeight] = true;
			cameraWeight = 1;
			pg_delayedCameraWeight = 0;
		}
	}

#if defined(PG_WITH_BLUR)
	// blur reset
	if (nb_blur_frames_1 > 0) {
		nb_blur_frames_1--;
		if (nb_blur_frames_1 <= 0) {
			is_blur_1 = false;
		}
	}
	if (nb_blur_frames_2 > 0) {
		nb_blur_frames_2--;
		if (nb_blur_frames_2 <= 0) {
			is_blur_2 = false;
		}
	}
#endif

#if defined(pg_Project_CAaudio)
	// CA seed
	pg_CAseed_trigger = false;
#endif

	// /////////////////////////
	// particle initialization reset
	part_initialization = -1;

	// automatic master incay/decay
	if (pg_master_incay_duration > 0) {
		if (pg_master_incay_start_time + pg_master_incay_duration >= pg_CurrentClockTime) {
			float alpha = max(0.f, min(1.f, float((pg_CurrentClockTime - pg_master_incay_start_time) / pg_master_incay_duration)));
			master = pg_master_incay_start_value + alpha * (1.f - pg_master_incay_start_value);
		}
		else {
			pg_master_incay_duration = 0.;
			master = 1.f;
		}
		pg_BrokenInterpolationVar[_master] = true;
		*((float*)pg_FullScenarioVarPointers[_master]) = master;
	}
	if (pg_master_decay_duration > 0) {
		if (pg_master_decay_start_time + pg_master_decay_duration >= pg_CurrentClockTime) {
			float alpha = max(0.f, min(1.f, float((pg_CurrentClockTime - pg_master_decay_start_time) / pg_master_decay_duration)));
			master = (1.f - alpha) * pg_master_decay_start_value;
		}
		else {
			pg_master_decay_duration = 0.;
			master = 0.f;
		}
		pg_BrokenInterpolationVar[_master] = true;
		*((float*)pg_FullScenarioVarPointers[_master]) = master;
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
// FRAME RENDERING
//////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// GLUT draw function
void pg_window_display(void) {

#if defined(PG_DEBUG)
	OutputDebugStringW(_T("\nstat 1\n"));
	_CrtMemCheckpoint(&s1);
#endif

	//printf("Maser %.2f\n", master);

	pg_windowDisplayed = true;
	// pg_printOglError(32);

	//////////////////////////////////////////////////
	//////////////////////////////////////////////////
	// SCENE UPDATE AND SHADER UNIFORM PARAMETERS UPDATE
	//////////////////////////////////////////////////
	// scenario update 
	pg_update_scenario();
	// recalculates pulsed colors and reads current paths
	pg_update_pulsed_colors_and_replay_paths(pg_CurrentClockTime);

	// ships uniform variables  pg_printOglError(34);
	pg_printOglError(35);
	pg_update_shader_uniforms();
	pg_printOglError(36);

	// loads movie andor camera frames
	pg_update_clip_camera_and_movie_frame();

#if defined(PG_DEBUG)
	OutputDebugStringW(_T("stat 7\n"));
	_CrtMemCheckpoint(&s7);
	if (_CrtMemDifference(&sDiff, &s9, &s7))
		_CrtMemDumpStatistics(&sDiff);
#endif


	if (pg_FullScenarioActiveVars[pg_ind_scenario][_activeMeshes]) {
		// updates mesh animation
		for (unsigned int indMeshFile = 0; indMeshFile < pg_Meshes[pg_ind_scenario].size(); indMeshFile++) {
			// visibility
			bool visible = false;
#if defined(pg_Project_Etoiles)
			visible = Etoiles_mesh_guided_by_strokes(indMeshFile);
#elif defined(var_Caverne_Mesh_Profusion)
			visible = (indMeshFile < 7 && (activeMeshes & (1 << indMeshFile))) || (pg_Meshes[pg_ind_scenario][indMeshFile].pg_CaverneActveMesh
				&& (pg_CurrentClockTime - pg_Meshes[pg_ind_scenario][indMeshFile].pg_CaverneMeshBirthTime > pg_Meshes[pg_ind_scenario][indMeshFile].pg_CaverneMeshWakeupTime)
				&& (pg_CurrentClockTime < pg_Meshes[pg_ind_scenario][indMeshFile].pg_CaverneMeshDeathTime));
#else
			visible = (activeMeshes & (1 << indMeshFile));

			// visible mesh
			if (visible) {
				if (pg_Mesh_Animations[pg_ind_scenario][indMeshFile].pg_tabBones != NULL) {
					//printf("updage anim & bones mesh %d\n", indMeshFile);
					pg_Mesh_Animations[pg_ind_scenario][indMeshFile].pg_update_BoneAnimation(indMeshFile);
					pg_Mesh_Animations[pg_ind_scenario][indMeshFile].pg_compute_BoneTransformations(indMeshFile);
				}
				if (pg_FullScenarioActiveVars[pg_ind_scenario][_mesh_rotation]) {
					pg_Mesh_Animations[pg_ind_scenario][indMeshFile].pg_update_MeshMotion(indMeshFile);
				}
			}
		}
	}
#endif

	// Looks for next peak or onset for random diaporama (cf GrabIt! music piece)
#if defined(var_Argenteuil_onsetchange_diaporama)
	double timeFromBeginning = pg_RealTime() - soundfile_data.sound_file_StartReadingTime;
	if (pg_currentlyPlaying_trackNo >= 0 
		&& int(pg_SoundTracks[pg_ind_scenario].size()) > pg_currentlyPlaying_trackNo) {
		//printf("size %d %d\n", pg_SoundTracks[pg_currentlyPlaying_trackNo]->soundTrackSoundtrackPeaks.size(), pg_SoundTracks[pg_currentlyPlaying_trackNo]->soundTrackSoundtrackPeaks.size());
		pg_updatePeakOrOnset(timeFromBeginning, &pg_SoundTracks[pg_ind_scenario][pg_currentlyPlaying_trackNo].soundtrackPeaks, 
			&pg_SoundTracks[pg_ind_scenario][pg_currentlyPlaying_trackNo].soundtrackOnsets,
			&pg_soundTrack_peak, &pg_soundTrack_onset, pg_nbTrackSoundPeakIndex[pg_ind_scenario], 
			pg_nbTrackSoundOnsetIndex[pg_ind_scenario], &pg_currentTrackSoundPeakIndex, &pg_currentTrackSoundOnsetIndex);
		if (pg_soundTrack_onset) {
			pg_diaporama_random();
		}
	}
#endif

	// tests whether the soundtrack is finished reading
	pg_pa_checkAudioStream();

	//////////////////////////////////////////////////
	// SCENE DISPLAY AND SHADER UNIFORM PARAMETERS UPDATE
	// OpenGL initializations before redisplay
	pg_OpenGLInit();
	// pg_printOglError(33);

#if defined(PG_DEBUG)
	OutputDebugStringW(_T("stat 4\n"));
	_CrtMemCheckpoint(&s4);
	if (_CrtMemDifference(&sDiff, &s7, &s4))
		_CrtMemDumpStatistics(&sDiff);
#endif

	// proper scene redrawing
	pg_draw_scene( pg_enum_render_Display );

#if defined(PG_DEBUG)
	OutputDebugStringW(_T("stat 2\n"));
	_CrtMemCheckpoint(&s2);
	if (_CrtMemDifference(&sDiff, &s4, &s2))
		_CrtMemDumpStatistics(&sDiff);
#endif

	//////////////////////////////////////////////////
	// ONE-FRAME PARAMETERS RESET OR UPDATE (AUTOMATIC, NON SCENARIO-BASED UPDATES)
	// resets to 0 the variables that are only true for one frame such as flashes, resets...
	pg_automatic_var_reset_or_update();

	// flushes OpenGL commands
	glFlush();

	// displays new frame
	glutSwapBuffers();

#if defined(PG_DEBUG)
	OutputDebugStringW(_T("stat 3\n"));
	_CrtMemCheckpoint(&s3);
	if (_CrtMemDifference(&sDiff, &s2, &s3))
		_CrtMemDumpStatistics(&sDiff);
#endif

	//////////////////////////////////////////////////
	// OPTIONAL SVG SNAPSHOTS
	// Svg screen shots
	// printf("Draw Svg\n" );
	if (take_snapshots && pg_Svg_Capture_param.outputImg) {
		// frame count based output
		if (pg_Svg_Capture_param.stepImgInFrames > 0) {
			if (pg_FrameNo % pg_Svg_Capture_param.stepImgInFrames == 0
				&& pg_FrameNo / pg_Svg_Capture_param.stepImgInFrames >= pg_Svg_Capture_param.beginImg &&
				pg_FrameNo / pg_Svg_Capture_param.stepImgInFrames <= pg_Svg_Capture_param.endImg) {
				pg_draw_scene( pg_enum_render_Svg );
			}
		}
		else if (pg_Svg_Capture_param.stepImgInSeconds > 0) {
			if (pg_Svg_Capture_param.nextImgCapture < 0) {
				pg_Svg_Capture_param.nextImgCapture = pg_CurrentClockTime;
			}
			if (pg_CurrentClockTime >= pg_Svg_Capture_param.nextImgCapture
				&& pg_CurrentClockTime >= pg_Svg_Capture_param.beginImg &&
				pg_CurrentClockTime <= pg_Svg_Capture_param.endImg) {
				pg_Svg_Capture_param.nextImgCapture = max(pg_Svg_Capture_param.nextImgCapture + pg_Svg_Capture_param.stepImgInSeconds, pg_CurrentClockTime);
				pg_draw_scene( pg_enum_render_Svg );
			}
		}
	}

	//////////////////////////////////////////////////
	// OPTIONAL PNG SNAPSHOTS
	// Png screen shots
	// printf("Draw Png\n" );
	if (take_snapshots && pg_Png_Capture_param.outputImg ) {
		// frame count based output
		if (pg_Png_Capture_param.stepImgInFrames > 0) {
			if (pg_FrameNo % pg_Png_Capture_param.stepImgInFrames == 0
				&& pg_FrameNo / pg_Png_Capture_param.stepImgInFrames >= pg_Png_Capture_param.beginImg &&
				pg_FrameNo / pg_Png_Capture_param.stepImgInFrames <= pg_Png_Capture_param.endImg) {
				pg_draw_scene( pg_enum_render_Png );
			}
		}
		else if (pg_Png_Capture_param.stepImgInSeconds > 0) {
			if (pg_Png_Capture_param.nextImgCapture < 0) {
				pg_Png_Capture_param.nextImgCapture = pg_CurrentClockTime;
			}
			if (pg_CurrentClockTime >= pg_Png_Capture_param.nextImgCapture
				&& pg_CurrentClockTime >= pg_Png_Capture_param.beginImg &&
				pg_CurrentClockTime <= pg_Png_Capture_param.endImg) {
				pg_Png_Capture_param.nextImgCapture = max(pg_Png_Capture_param.nextImgCapture + pg_Png_Capture_param.stepImgInSeconds, pg_CurrentClockTime);
				pg_draw_scene( pg_enum_render_Png );
			}
		}
	}

	//////////////////////////////////////////////////
	// OPTIONAL JPG SNAPSHOTS
	// Jpg screen shots
	// printf("Draw Jpg\n"  );
	if (take_snapshots && pg_Jpg_Capture_param.outputImg) {
		// frame count based output
		if (pg_Jpg_Capture_param.stepImgInFrames > 0) {
			if (pg_FrameNo % pg_Jpg_Capture_param.stepImgInFrames == 0
				&& pg_FrameNo / pg_Jpg_Capture_param.stepImgInFrames >= pg_Jpg_Capture_param.beginImg &&
				pg_FrameNo / pg_Jpg_Capture_param.stepImgInFrames <= pg_Jpg_Capture_param.endImg) {
				pg_draw_scene( pg_enum_render_Jpg );
			}
		}
		else if (pg_Jpg_Capture_param.stepImgInSeconds > 0) {
			if (pg_Jpg_Capture_param.nextImgCapture < 0) {
				pg_Jpg_Capture_param.nextImgCapture = pg_CurrentClockTime;
			}
			if (pg_CurrentClockTime >= pg_Jpg_Capture_param.nextImgCapture
				&& pg_CurrentClockTime >= pg_Jpg_Capture_param.beginImg &&
				pg_CurrentClockTime <= pg_Jpg_Capture_param.endImg) {
				pg_Jpg_Capture_param.nextImgCapture = max(pg_Jpg_Capture_param.nextImgCapture + pg_Jpg_Capture_param.stepImgInSeconds, pg_CurrentClockTime);
				pg_draw_scene( pg_enum_render_Jpg );
				//printf("nextJpgCapture %.2f pg_CurrentClockTime %.2f\n", nextJpgCapture, pg_CurrentClockTime);
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// MULTIPASS RENDERING
//////////////////////////////////////////////////////////////////////////////////////////////////////

// initializes the homography matrices for the distortion of the projected image
void pg_calculate_homography_matrices(std::vector<cv::Point2f>* sourcePoints, std::vector<cv::Point2f>* destinationPoints, GLfloat pg_matValues[], int dim) {
	//////////////////////////////////////////////////////////////////////////////////
	// right camera (left display)

	///////////////////////// HOMOGRPAHY
	// This homography transforms the points projected at near=1 into the four
	// points measured experimetally corresponding to the 4 vertices of the
	// quad projected at 1m with the projector axis orthogonal to the projection plane

	homography = cv::findHomography(*sourcePoints, *destinationPoints, 0);
	printf("Homography matrix calculation %d %d\n", homography.size().width, homography.size().height);
	// in GLM Matrix types store their values in column - major order.
	if (dim == 4) {
		pg_matValues[0] = (float)((double*)homography.data)[0];
		pg_matValues[1] = (float)((double*)homography.data)[3];
		pg_matValues[2] = 0.0;
		pg_matValues[3] = (float)((double*)homography.data)[6];

		pg_matValues[4] = (float)((double*)homography.data)[1];
		pg_matValues[5] = (float)((double*)homography.data)[4];
		pg_matValues[6] = 0.0;
		pg_matValues[7] = (float)((double*)homography.data)[7];

		pg_matValues[8] = 0.0;
		pg_matValues[9] = 0.0;
		pg_matValues[10] = 0.0;
		pg_matValues[11] = 0.0;

		pg_matValues[12] = (float)((double*)homography.data)[2];
		pg_matValues[13] = (float)((double*)homography.data)[5];
		pg_matValues[14] = 0.0;
		pg_matValues[15] = (float)((double*)homography.data)[8];
	}
	else if (dim == 3) {
		pg_matValues[0] = (float)((double*)homography.data)[0];
		pg_matValues[1] = (float)((double*)homography.data)[3];
		pg_matValues[2] = (float)((double*)homography.data)[6];

		pg_matValues[3] = (float)((double*)homography.data)[1];
		pg_matValues[4] = (float)((double*)homography.data)[4];
		pg_matValues[5] = (float)((double*)homography.data)[7];

		pg_matValues[6] = (float)((double*)homography.data)[2];
		pg_matValues[7] = (float)((double*)homography.data)[5];
		pg_matValues[8] = (float)((double*)homography.data)[8];
	}
	else {
		sprintf(pg_errorStr, "homography dimension should be 3 or 4, not %d", dim); pg_ReportError(pg_errorStr);
	}
}

// initializes the transformation matrices related to the frustum
// should only be called when the values are changed through pd or maxmsp
void pg_calculate_perspective_matrices(void) {
	//printf("[1] Loc %.1f %.1f %.1f LookAt %.1f %.1f %.1f\n",
	//	VP1LocX, VP1LocY, VP1LocZ, VP1LookAtX, VP1LookAtY, VP1LookAtZ);

	//////////////////////////////////////////////////////////////////////////////////
	// right camera (right display)
	if (double_window) {
		pg_VP1perspMatrix
			= glm::frustum(-VP1WidthTopAt1m / 2.0f, VP1WidthTopAt1m / 2.0f, VP1BottomAt1m, VP1TopAt1m, nearPlane, farPlane);
	}
	else {
		pg_VP1perspMatrix
			= glm::frustum(-VP1WidthTopAt1m / 2.0f, VP1WidthTopAt1m / 2.0f, VP1BottomAt1m, VP1TopAt1m, nearPlane, farPlane);
	}
	//printf("Perspective 1 %.2f %.2f %.2f %.2f near Far %.2f %.2f\n" , -VP1WidthTopAt1m / 2.0f, VP1WidthTopAt1m / 2.0f, VP1BottomAt1m, VP1TopAt1m, nearPlane, farPlane);

	// Camera matrix
	// float eyePosition[3] = {10.1f,4.6f,4.4f};
	// the projection of the bottom of the screen is known
	// we have to deduce the look at from this point && the measure of 
	// the projector calibration
	// the look at has to be recalculated because the targe point is above the axis of projection
	// by an angle alpha
	//printf("VP1BottomAt1m %.2f\n" , VP1BottomAt1m );

	// ARCHIVE float VP1alpha = atan(VP1BottomAt1m);
	// ARCHIVE ::vec4 vectorVPPositionToScreenBottom
	// ARCHIVE 	= glm::vec4(VP1LookAtX - VP1LocX, VP1LookAtY - VP1LocY, VP1LookAtZ - VP1LocZ, 0.0);
	// printf("vectorLookAt init %.2f %.2f %.2f\n" , vectorVPPositionToScreenBottom.x , vectorVPPositionToScreenBottom.y , vectorVPPositionToScreenBottom.z );
	// ARCHIVE ::mat4 rotationMat(1); // Creates a identity matrix
	// rotation axis is k x vectorVPPositionToScreenBottom (or -k if projector is upside down)
	// ARCHIVE ::vec3 rotationAxis = glm::cross(glm::vec3(0, 0, (VP1Reversed ? -1.0f : 1.0f)), glm::vec3(vectorVPPositionToScreenBottom));
	// ARCHIVE  = glm::normalize(rotationAxis);
	// printf("rotationAxis %.2f %.2f %.2f\n", rotationAxis.x, rotationAxis.y, rotationAxis.z);
	// shifts towards the bottom the actual lookat center according to VP1BottomAt1m
	// ARCHIVE rotationMat = glm::rotate(rotationMat, VP1alpha, rotationAxis); // (VP1Reversed?-1.0f:1.0f) *
	// ARCHIVE glm::vec3 vectorLookAt = glm::vec3(rotationMat * vectorVPPositionToScreenBottom);
	// printf("vectorLookAt real %.2f %.2f %.2f\n" , vectorLookAt.x , vectorLookAt.y , vectorLookAt.z );
	// ARCHIVE glm::vec3 lookAtPoint = glm::vec3(VP1LocX, VP1LocY, VP1LocZ) + vectorLookAt;
	//printf("VP1Loc %.2f %.2f %.2f\n", VP1LocX, VP1LocY, VP1LocZ);
	//printf("Look at real %.2f %.2f %.2f\n\n", lookAtPoint.x, lookAtPoint.y, lookAtPoint.z);
	pg_VP1viewMatrix
		= glm::lookAt(
			glm::vec3(VP1LocX, VP1LocY, VP1LocZ), // Camera is at (VP1LocX, VP1LocY, VP1LocZ), in World Space
			glm::vec3(VP1LookAtX, VP1LookAtY, VP1LookAtZ), // && looks at lookAtPoint
			glm::vec3(0, VP1UpY, (VP1Reversed ? -1.0f : 1.0f))  // Head is up (set to 0, VP1UpY, 1 or 0, VP1UpY, -1 if projector is upside down)
		);

#if defined(var_VP2WidthTopAt1m) && defined(var_VP2BottomAt1m) && defined(var_VP2TopAt1m) && defined(var_nearPlane) && defined(var_farPlane)
	//////////////////////////////////////////////////////////////////////////////////
	// left camera (right display)
	pg_VP2perspMatrix
		= glm::frustum(-VP2WidthTopAt1m / 2.0f, VP2WidthTopAt1m / 2.0f, VP2BottomAt1m, VP2TopAt1m, nearPlane, farPlane);

	//printf("Perspective 1 %.2f %.2f %.2f %.2f\n" , -VP2WidthTopAt1m / 2.0f, VP2WidthTopAt1m / 2.0f, VP2BottomAt1m, VP2TopAt1m );

	// Camera matrix
	// float eyePosition[3] = {10.1f,4.6f,4.4f};
	// the projection of the bottom of the screen is known
	// we have to deduce the look at from this point and the measure of 
	// the projector calibration
	// the look at has to be recalculated because the targe point is above the axis of projection
	// by an angle alpha
	//printf("VP2BottomAt1m %.2f\n" , VP2BottomAt1m );

	// ARCHIVE float VP2alpha = atan(VP2BottomAt1m);
	// ARCHIVE vectorVPPositionToScreenBottom
// ARCHIVE = glm::vec4(VP2LookAtX - VP2LocX, VP2LookAtY - VP2LocY, VP2LookAtZ - VP2LocZ, 0.0);
	// printf("vectorLookAt init %.2f %.2f %.2f\n" , vectorVPPositionToScreenBottom.x , vectorVPPositionToScreenBottom.y , vectorVPPositionToScreenBottom.z );
	// rotation axis is k x vectorVPPositionToScreenBottom (or -k if projector is upside down)
	// ARCHIVE rotationAxis = glm::cross(glm::vec3(0, 0, (VP2Reversed ? -1.0f : 1.0f)), glm::vec3(vectorVPPositionToScreenBottom));
	// ARCHIVE  = glm::normalize(rotationAxis);
	// shifts towards the bottom the actual lookat center according to VP2BottomAt1m
	// ARCHIVE rotationMat = glm::rotate(glm::mat4(1.0f), VP2alpha, rotationAxis); // (VP2Reversed?-1.0f:1.0f) *
	// ARCHIVE vectorLookAt = glm::vec3(rotationMat * vectorVPPositionToScreenBottom);
	// printf("vectorLookAt real %.2f %.2f %.2f\n" , vectorLookAt.x , vectorLookAt.y , vectorLookAt.z );
	// ARCHIVE lookAtPoint = glm::vec3(VP2LocX, VP2LocY, VP2LocZ) + vectorLookAt;
	// printf("Look at real %.2f %.2f %.2f\n\n" , lookAtPoint.x , lookAtPoint.y , lookAtPoint.z );
	pg_VP2viewMatrix
		= glm::lookAt(
			glm::vec3(VP2LocX, VP2LocY, VP2LocZ), // Camera is at (VP2LocX, VP2LocY, VP2LocZ), in World Space
			glm::vec3(VP2LookAtX, VP2LookAtY, VP2LookAtZ), // && looks at lookAtPoint
			glm::vec3(0, VP2UpY, (VP2Reversed ? -1.0f : 1.0f))  // Head is up (set to 0, VP2UpY, 1 or 0, VP2UpY, -1 if projector is upside down)
		);
#endif
}

void pg_calculate_orthographic_matrices(void) {
	//printf("[1] Loc %.1f %.1f %.1f LookAt %.1f %.1f %.1f\n",
	//	VP1LocX, VP1LocY, VP1LocZ, VP1LookAtX, VP1LookAtY, VP1LookAtZ);

	if (!pg_FullScenarioActiveVars[pg_ind_scenario][_VP1LocX]
		|| !pg_shader_programme[pg_ind_scenario][_VP1LocY]
		|| !pg_shader_programme[pg_ind_scenario][_VP1LocZ]
		|| !pg_shader_programme[pg_ind_scenario][_VP1LookAtX]
		|| !pg_shader_programme[pg_ind_scenario][_VP1LookAtY]
		|| !pg_shader_programme[pg_ind_scenario][_VP1LookAtZ]
		|| !pg_shader_programme[pg_ind_scenario][_VP1UpY]
		|| !pg_shader_programme[pg_ind_scenario][_VP1Reversed]
		|| !pg_shader_programme[pg_ind_scenario][_VP1WidthTopAt1m]
		|| !pg_shader_programme[pg_ind_scenario][_VP1BottomAt1m]
		|| !pg_shader_programme[pg_ind_scenario][_VP1TopAt1m]
		|| !pg_shader_programme[pg_ind_scenario][_nearPlane]
		|| !pg_shader_programme[pg_ind_scenario][_farPlane]) {
		return;
	}

	//////////////////////////////////////////////////////////////////////////////////
	// right camera (right display)
	if (double_window) {
		pg_VP1perspMatrix
			= glm::ortho(-VP1WidthTopAt1m / 2.0f, VP1WidthTopAt1m / 2.0f, VP1BottomAt1m, VP1TopAt1m, nearPlane, farPlane);
	}
	else {
		pg_VP1perspMatrix
			= glm::ortho(-VP1WidthTopAt1m / 2.0f, VP1WidthTopAt1m / 2.0f, VP1BottomAt1m, VP1TopAt1m, nearPlane, farPlane);
	}
	//printf("Perspective 1 %.2f %.2f %.2f %.2f near Far %.2f %.2f\n" , -VP1WidthTopAt1m / 2.0f, VP1WidthTopAt1m / 2.0f, VP1BottomAt1m, VP1TopAt1m, nearPlane, farPlane);

	// Camera matrix
	//printf("Look at real %.2f %.2f %.2f\n\n", lookAtPoint.x, lookAtPoint.y, lookAtPoint.z);
	glm::mat4 m4(1.0f);
	pg_VP1viewMatrix = m4;


#if defined(var_VP2WidthTopAt1m) && defined(var_VP2BottomAt1m) && defined(var_VP2TopAt1m) && defined(var_nearPlane) && defined(var_farPlane)
	// TODO
#endif
}

/////////////////////////////////////
// PASS #0: PARTICLE ANIMATION PASS

void pg_ParticleAnimationPass(void) {
	if (!pg_shader_programme[pg_ind_scenario][pg_enum_shader_ParticleAnimation]) {
		return;
	}

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, pg_FBO_ParticleAnimation);
	// printf("FBO ANIMATION       %d\n", pg_FBO_ParticleAnimation[((pg_FrameNo + 1) % 2)]);
	pg_bindFBOTextures(pg_FBO_ParticleAnimation, pg_FBO_ParticleAnimation_texID + ((pg_FrameNo + 1) % 2) * pg_enum_FBO_ParticleAnimation_nbAttachts, pg_enum_FBO_ParticleAnimation_nbAttachts, false, 0);

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	////////////////////////////////////////
	// activate shaders and sets uniform variable values    
	glUseProgram(pg_shader_programme[pg_ind_scenario][pg_enum_shader_ParticleAnimation]);

	glBindVertexArray(pg_vaoID[pg_enum_VAOQuad]);

	glUniformMatrix4fv(uniform_ParticleAnimation_vp_proj[pg_ind_scenario], 1, GL_FALSE, pg_orthoWindowProjMatrix);
	glUniformMatrix4fv(uniform_ParticleAnimation_vp_view[pg_ind_scenario], 1, GL_FALSE, pg_identityViewMatrix);
	glUniformMatrix4fv(uniform_ParticleAnimation_vp_model[pg_ind_scenario], 1, GL_FALSE, pg_identityModelMatrix);

	// texture unit location
	glUniform1i(uniform_ParticleAnimation_texture_fs_Part_init_pos_speed[pg_ind_scenario], pg_enum_Part_init_pos_speed_ParticleAnimation_sampler);
	glUniform1i(uniform_ParticleAnimation_texture_fs_Part_init_col_rad[pg_ind_scenario], pg_enum_Part_init_col_rad_ParticleAnimation_sampler);
	glUniform1i(uniform_ParticleAnimation_texture_fs_Part_acc[pg_ind_scenario], pg_enum_Part_image_acc_ParticleAnimation_sampler);
	glUniform1i(uniform_ParticleAnimation_texture_fs_CA[pg_ind_scenario], pg_enum_CA_FBO_ParticleAnimation_sampler);
	glUniform1i(uniform_ParticleAnimation_texture_fs_CA[pg_ind_scenario], pg_enum_CA_FBO_ParticleAnimation_sampler);
	glUniform1i(uniform_ParticleAnimation_texture_fs_Part_pos_speed[pg_ind_scenario], pg_enum_Part_pos_speed_FBO_ParticleAnimation_sampler);
	glUniform1i(uniform_ParticleAnimation_texture_fs_Part_col_rad[pg_ind_scenario], pg_enum_Part_col_rad_FBO_ParticleAnimation_sampler);
	glUniform1i(uniform_ParticleAnimation_texture_fs_Part_Target_pos_col_rad[pg_ind_scenario], pg_enum_Part_Target_pos_col_rad_FBO_ParticleAnimation_sampler);
	glUniform1i(uniform_ParticleAnimation_texture_fs_Noise[pg_ind_scenario], pg_enum_Noise_ParticleAnimation_sampler);
	glUniform1i(uniform_ParticleAnimation_texture_fs_RepopDensity[pg_ind_scenario], pg_enum_RepopDensity_ParticleAnimation_sampler);
	glUniform1i(uniform_ParticleAnimation_texture_fs_Camera_frame[pg_ind_scenario], pg_enum_Camera_frame_ParticleAnimation_sampler);
	glUniform1i(uniform_ParticleAnimation_texture_fs_Movie_frame[pg_ind_scenario], pg_enum_Movie_frame_ParticleAnimation_sampler);
	glUniform1i(uniform_ParticleAnimation_texture_fs_Trk0[pg_ind_scenario], pg_enum_Trk0_FBO_ParticleAnimation_sampler);
	glUniform1i(uniform_ParticleAnimation_texture_fs_Trk1[pg_ind_scenario], pg_enum_Trk1_FBO_ParticleAnimation_sampler);
	glUniform1i(uniform_ParticleAnimation_texture_fs_Trk2[pg_ind_scenario], pg_enum_Trk2_FBO_ParticleAnimation_sampler);
	glUniform1i(uniform_ParticleAnimation_texture_fs_Trk3[pg_ind_scenario], pg_enum_Trk3_FBO_ParticleAnimation_sampler);

	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// position speed are stored in a texture
	glActiveTexture(GL_TEXTURE0 + pg_enum_Part_init_pos_speed_ParticleAnimation_sampler);
	if (part_initialization >= 0
		&& part_initialization < int(pg_particle_initial_pos_speed_texID[pg_ind_scenario].size())) {
		glBindTexture(GL_TEXTURE_RECTANGLE, pg_particle_initial_pos_speed_texID[pg_ind_scenario].at(part_initialization)); // pos - speed 
		printf("particle initialization (position speed) %d\n", part_initialization);
	}
	else {
		glBindTexture(GL_TEXTURE_RECTANGLE, NULL_ID);
	}

	// photo or video particles, position and speed are chosen from a random texture between 
	// 0 & PG_NB_PARTICLE_INITIAL_IMAGES - 1
	//else if (part_initialization >= 0
	//		&& part_initialization < int(pg_particle_initial_pos_speed_texID.size())) {
	//	unsigned int indTex = unsigned int(floor(rand_0_1 * pg_particle_initial_pos_speed_texID.size())) % pg_particle_initial_pos_speed_texID.size();
	//	glBindTexture(GL_TEXTURE_RECTANGLE, pg_particle_initial_pos_speed_texID.at(indTex)); // pos - speed 
	//	printf("particle initialization %d texture %d\n", part_initialization, indTex);
	//}

	glActiveTexture(GL_TEXTURE0 + pg_enum_Part_init_col_rad_ParticleAnimation_sampler);
	// color and radius are taken from photo or video and partic radius
	if (part_initialization >= 0
		&& part_initialization < int(pg_particle_initial_pos_speed_texID[pg_ind_scenario].size())) {
		glBindTexture(GL_TEXTURE_RECTANGLE, pg_particle_initial_color_radius_texID[pg_ind_scenario].at(part_initialization)); // color RGB - rad 
		//printf("particle initialization (color radius) %d\n", part_initialization);
	}
	else {
		glBindTexture(GL_TEXTURE_RECTANGLE, NULL_ID);
	}

	glActiveTexture(GL_TEXTURE0 + pg_enum_Part_image_acc_ParticleAnimation_sampler);
	// acceleration is taken from image 
	if (part_image_acceleration >= 0
		&& part_image_acceleration < int(pg_particle_acc_texID[pg_ind_scenario].size())) {
		glBindTexture(GL_TEXTURE_RECTANGLE, pg_particle_acc_texID[pg_ind_scenario].at(part_image_acceleration)); // color RGB - rad 
		//printf("particle image acceleration (image based acceleration) %d\n", part_image_acceleration);
	}
	else {
		glBindTexture(GL_TEXTURE_RECTANGLE, NULL_ID);
	}

	// 2-cycle ping-pong CA step n step n (FBO attachment 0)
	glActiveTexture(GL_TEXTURE0 + pg_enum_CA_FBO_ParticleAnimation_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[(pg_FrameNo % 2) * pg_enum_FBO_Update_nbAttachts + pg_enum_CA_FBO_Update_attacht]);

	// 2-cycle ping-pong position/speed of particles step n step n (FBO attachment 0)
	glActiveTexture(GL_TEXTURE0 + pg_enum_Part_pos_speed_FBO_ParticleAnimation_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_ParticleAnimation_texID[(pg_FrameNo % 2) * pg_enum_FBO_ParticleAnimation_nbAttachts + pg_enum_Part_pos_speed_FBO_ParticleAnimation_attacht]);

	// 2-cycle ping-pong color/radius of particles step n step n (FBO attachment 1)
	glActiveTexture(GL_TEXTURE0 + pg_enum_Part_col_rad_FBO_ParticleAnimation_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_ParticleAnimation_texID[(pg_FrameNo % 2) * pg_enum_FBO_ParticleAnimation_nbAttachts + pg_enum_Part_col_rad_FBO_ParticleAnimation_attacht]);

	// 2-cycle ping-pong target position/color/radius of particles step n step n (FBO attachment 2)
	glActiveTexture(GL_TEXTURE0 + pg_enum_Part_Target_pos_col_rad_FBO_ParticleAnimation_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_ParticleAnimation_texID[(pg_FrameNo % 2) * pg_enum_FBO_ParticleAnimation_nbAttachts + pg_enum_Part_Target_pos_col_rad_FBO_ParticleAnimation_attacht]);

	// noise texture (noise is also generated procedurally in the update shader)
	glActiveTexture(GL_TEXTURE0 + pg_enum_Noise_ParticleAnimation_sampler);
	glBindTexture(GL_TEXTURE_3D, pg_Noise_texture_3D[pg_ind_scenario]);

	// noise texture (noise is also generated procedurally in the update shader)
	glActiveTexture(GL_TEXTURE0 + pg_enum_RepopDensity_ParticleAnimation_sampler);
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_Part_repop_density]) {
		if (Part_repop_density >= 0
			&& int(Part_repop_density) < int(pg_RepopDensity_texture_texID[pg_ind_scenario].size())) {
			glBindTexture(GL_TEXTURE_RECTANGLE, pg_RepopDensity_texture_texID[pg_ind_scenario].at(Part_repop_density));
			//printf("part repop density %d/%d\n", Part_repop_density, pg_RepopDensity_texture_texID.at(Part_repop_density[pg_ind_scenario]));
		}
		else {
			glBindTexture(GL_TEXTURE_RECTANGLE, NULL_ID);
		}
	}
	else {
		glBindTexture(GL_TEXTURE_RECTANGLE, NULL_ID);
	}

	// camera texture produced at preceding pass
	// glActiveTexture(GL_TEXTURE0 + 7);
	// glBindTexture(GL_TEXTURE_RECTANGLE, FBO_CameraFrame_texID);
	// current camera texture
	glActiveTexture(GL_TEXTURE0 + pg_enum_Camera_frame_ParticleAnimation_sampler);
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_cameraCaptFreq]) {
		glBindTexture(GL_TEXTURE_RECTANGLE, pg_camera_texture_texID);
	}
	else {
		glBindTexture(GL_TEXTURE_RECTANGLE, NULL_ID);
	}

	// movie texture
	glActiveTexture(GL_TEXTURE0 + pg_enum_Movie_frame_ParticleAnimation_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_movie_texture_texID);

	// 2-cycle ping-pong BG track step n (FBO attachment 5)
	glActiveTexture(GL_TEXTURE0 + pg_enum_Trk0_FBO_ParticleAnimation_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[(pg_FrameNo % 2) * pg_enum_FBO_Update_nbAttachts + pg_enum_Trk0_FBO_Update_attacht]);

	// 2-cycle ping-pong track 1 step n (FBO attachment 6)
	glActiveTexture(GL_TEXTURE0 + pg_enum_Trk1_FBO_ParticleAnimation_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[(pg_FrameNo % 2) * pg_enum_FBO_Update_nbAttachts + pg_enum_Trk1_FBO_Update_attacht]);

	// 2-cycle ping-pong track 2 step n (FBO attachment 7)
	glActiveTexture(GL_TEXTURE0 + pg_enum_Trk2_FBO_ParticleAnimation_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[(pg_FrameNo % 2) * pg_enum_FBO_Update_nbAttachts + pg_enum_Trk2_FBO_Update_attacht]);

	// 2-cycle ping-pong track 2 step n (FBO attachment 8)
	glActiveTexture(GL_TEXTURE0 + pg_enum_Trk3_FBO_ParticleAnimation_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[(pg_FrameNo % 2) * pg_enum_FBO_Update_nbAttachts + pg_enum_Trk3_FBO_Update_attacht]);

	// draw points from the currently bound VAO with current in-use shader
	// glDrawArrays (GL_TRIANGLES, 0, 3 * PG_SIZE_QUAD_ARRAY);
	// Index buffer for indexed rendering
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pg_vboID[pg_enum_EABQuad]);
	// Draw the triangles !
	glDrawElements(
		GL_TRIANGLE_STRIP,      // mode
		3 * PG_SIZE_QUAD_ARRAY,    // count
		GL_UNSIGNED_INT,   // type
		(void*)0           // element array buffer offset
	);

	pg_printOglError(46);
}

//////////////////////////////////////////////////
// PASS #1: UPDATE (CA, PIXELS, PARTICLES, DRAWING, PHOTOS & VIDEOS)

void pg_UpdatePass(void) {
	if (!pg_shader_programme[pg_ind_scenario][pg_enum_shader_Update]) {
		return;
	}

	// ping pong output and input FBO bindings
	// next frame
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, pg_FBO_Update);
	//glBindFramebuffer(GL_READ_FRAMEBUFFER, pg_FBO_Update[(pg_FrameNo % 2)]);
	//printf("Output FBO %d\n", pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * pg_enum_FBO_Update_nbAttachts + pg_enum_Pixels_FBO_Update_attacht]);
	//printf("Input FBO %d\n", pg_FBO_Update_texID[(pg_FrameNo % 2) * pg_enum_FBO_Update_nbAttachts + pg_enum_Pixels_FBO_Update_attacht]);	pg_bindFBOTextures(pg_FBO_Update, pg_FBO_Update_texID, pg_enum_FBO_Update_nbAttachts, false, 0);
	pg_bindFBOTextures(pg_FBO_Update, pg_FBO_Update_texID + ((pg_FrameNo + 1) % 2) * pg_enum_FBO_Update_nbAttachts, pg_enum_FBO_Update_nbAttachts, false, 0);

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	////////////////////////////////////////
	// activate shaders and sets uniform variable values    
	glUseProgram(pg_shader_programme[pg_ind_scenario][pg_enum_shader_Update]);
	//if (pg_FrameNo % 300 == 0) {
	//	printf("pg_shader_programme udate %d\n", pg_shader_programme[pg_ind_scenario][pg_enum_shader_Update]);
	//}
	glBindVertexArray(pg_vaoID[pg_enum_VAOQuad]);
	glUniformMatrix4fv(uniform_Update_vp_proj[pg_ind_scenario], 1, GL_FALSE, pg_orthoWindowProjMatrix);
	glUniformMatrix4fv(uniform_Update_vp_view[pg_ind_scenario], 1, GL_FALSE, pg_identityViewMatrix);
	glUniformMatrix4fv(uniform_Update_vp_model[pg_ind_scenario], 1, GL_FALSE, pg_identityModelMatrix);

	if (pg_FullScenarioActiveVars[pg_ind_scenario][_photo_homography]) {
		if (photo_homography) {
			// homography for background texture distortion
	// source points
			std::vector<cv::Point2f> sourcePoints;
			// destination points
			std::vector<cv::Point2f> destinationPoints;

			// the current image coordinates are given by the keystone variables
			sourcePoints.push_back(cv::Point2f(VP1KeystoneXBottomLeft, VP1KeystoneYBottomLeft));
			sourcePoints.push_back(cv::Point2f(VP1KeystoneXBottomRight, VP1KeystoneYBottomRight));
			sourcePoints.push_back(cv::Point2f(VP1KeystoneXTopRight, VP1KeystoneYTopRight));
			sourcePoints.push_back(cv::Point2f(VP1KeystoneXTopLeft, VP1KeystoneYTopLeft));

			// the initial texture coordinates are based on unit for the whole image
			// they are then multiplied by the image ratio (wrt power of two uper dimensions)
			// inside the shader to access only the part of the texture that corresponds to the image
			destinationPoints.push_back(cv::Point2f(0.0f, 1.f));
			destinationPoints.push_back(cv::Point2f(1.f, 1.f));
			destinationPoints.push_back(cv::Point2f(1.f, 0.0f));
			destinationPoints.push_back(cv::Point2f(0.0f, 0.0f));

			// initializes the homography matrices for the distortion of the projected image
			pg_calculate_homography_matrices(&sourcePoints, &destinationPoints, pg_homographyForTexture, 3);

			// and transfer the matrix to the shader
			glUniformMatrix3fv(uniform_Update_homographyForTexture[pg_ind_scenario], 1, GL_FALSE, pg_homographyForTexture);
		}
	}


	////////////////////////////////////////////////////////
	// texture unit location
	glUniform1i(uniform_Update_texture_fs_CA[pg_ind_scenario], pg_enum_CA_FBO_Update_sampler);
	glUniform1i(uniform_Update_texture_fs_PreviousCA[pg_ind_scenario], pg_enum_PreviousCA_FBO_Update_sampler);
	glUniform1i(uniform_Update_texture_fs_Pixels[pg_ind_scenario], pg_enum_Pixels_FBO_Update_sampler);
	glUniform1i(uniform_Update_texture_fs_Brushes[pg_ind_scenario], pg_enum_Brushes_Update_sampler);
	glUniform1i(uniform_Update_texture_fs_Camera_frame[pg_ind_scenario], pg_enum_Camera_frame_Update_sampler);
	glUniform1i(uniform_Update_texture_fs_Camera_BG[pg_ind_scenario], pg_enum_Camera_BG_Update_sampler);
	glUniform1i(uniform_Update_texture_fs_Movie_frame[pg_ind_scenario], pg_enum_Movie_frame_Update_sampler);
	glUniform1i(uniform_Update_texture_fs_Noise[pg_ind_scenario], pg_enum_Noise_Update_sampler);
	glUniform1i(uniform_Update_texture_fs_Pixel_Noise[pg_ind_scenario], pg_enum_Pixel_Noise_Update_sampler);
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_BG_CA_repop_density]) {
		glUniform1i(uniform_Update_texture_fs_RepopDensity[pg_ind_scenario], pg_enum_RepopDensity_Update_sampler);
	}
	glUniform1i(uniform_Update_texture_fs_Photo0[pg_ind_scenario], pg_enum_Photo0_Update_sampler);
	glUniform1i(uniform_Update_texture_fs_Photo1[pg_ind_scenario], pg_enum_Photo1_Update_sampler);
	if (PG_NB_PARALLEL_CLIPS >= 2) {
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_clipCaptFreq]) {
			glUniform1i(uniform_Update_texture_fs_Clip0[pg_ind_scenario], pg_enum_SecondClipLeft_Update_sampler);
			glUniform1i(uniform_Update_texture_fs_Clip1[pg_ind_scenario], pg_enum_SecondClipRight_Update_sampler);
		}
	}
	glUniform1i(uniform_Update_texture_fs_Part_render[pg_ind_scenario], pg_enum_Particle_render_FBO_Update_sampler);

	glUniform1i(uniform_Update_texture_fs_Trk0[pg_ind_scenario], pg_enum_Trk0_FBO_Update_sampler);
	glUniform1i(uniform_Update_texture_fs_Trk1[pg_ind_scenario], pg_enum_Trk1_FBO_Update_sampler);
	glUniform1i(uniform_Update_texture_fs_Trk2[pg_ind_scenario], pg_enum_Trk2_FBO_Update_sampler);
	glUniform1i(uniform_Update_texture_fs_Trk3[pg_ind_scenario], pg_enum_Trk3_FBO_Update_sampler);
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_camera_BG_subtr]) {
		glUniform1i(uniform_Update_texture_fs_Camera_BGIni[pg_ind_scenario], pg_enum_Camera_BGIni_FBO_Update_sampler);
	}
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_pixel_image_acceleration]) {
		glUniform1i(uniform_Update_texture_fs_pixel_acc[pg_ind_scenario], pg_enum_pixel_image_acc_Update_sampler);
	}
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_CATable]) {
		glUniform1i(uniform_Update_texture_fs_CATable[pg_ind_scenario], pg_enum_CATable_Update_sampler);
	}

	////////////////////////////////////////////////////////
	// by default textures are wrapped
	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	////////////////////////////////////////////////////////
	// texture unit binding
	// 2-cycle ping-pong CA step n (FBO attachment 0) -- current Frame
	glActiveTexture(GL_TEXTURE0 + pg_enum_CA_FBO_Update_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[(pg_FrameNo % 2) * pg_enum_FBO_Update_nbAttachts + pg_enum_CA_FBO_Update_attacht]);
	// 2-cycle ping-pong CA step n (FBO attachment 0) -- previous Frame
	glActiveTexture(GL_TEXTURE0 + pg_enum_PreviousCA_FBO_Update_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * pg_enum_FBO_Update_nbAttachts + pg_enum_CA_FBO_Update_attacht]);
	//printf("pg_FBO_Update_texID\n");

	// 2-cycle ping-pong speed/position of pixels step n step n (FBO attachment 1) -- current Frame
	glActiveTexture(GL_TEXTURE0 + pg_enum_Pixels_FBO_Update_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[(pg_FrameNo % 2) * pg_enum_FBO_Update_nbAttachts + pg_enum_Pixels_FBO_Update_attacht]);
	//if (pg_FrameNo < 1000) {
	//	printf("FBO Pixels Update textures config: %d IDs: %d\n", pg_ind_scenario, 
	//		pg_FBO_Update_texID[(pg_FrameNo % 2) * pg_enum_FBO_Update_nbAttachts + pg_enum_Pixels_FBO_Update_attacht]);
	//}

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// pen patterns
	glActiveTexture(GL_TEXTURE0 + pg_enum_Brushes_Update_sampler);
	glBindTexture(GL_TEXTURE_3D, pg_Pen_texture_3D_texID[pg_ind_scenario]);

	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// camera texture produced at preceding pass
	// glActiveTexture(GL_TEXTURE0 + 7);
	// glBindTexture(GL_TEXTURE_RECTANGLE, FBO_CameraFrame_texID);
	// current camera texture
	glActiveTexture(GL_TEXTURE0 + pg_enum_Camera_frame_Update_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_camera_texture_texID);

	// current background texture
	glActiveTexture(GL_TEXTURE0 + pg_enum_Camera_BG_Update_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_camera_BG_texture_texID);

	// movie texture
	glActiveTexture(GL_TEXTURE0 + pg_enum_Movie_frame_Update_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_movie_texture_texID);

	// noise texture (noise is also generated procedurally in the update shader)
	glActiveTexture(GL_TEXTURE0 + pg_enum_Noise_Update_sampler);
	glBindTexture(GL_TEXTURE_3D, pg_Noise_texture_3D[pg_ind_scenario]);

	// pixel noise texture (noise for pixel accceleration)
	glActiveTexture(GL_TEXTURE0 + pg_enum_Pixel_Noise_Update_sampler);
	glBindTexture(GL_TEXTURE_3D, pg_Pixel_Noise_texture_3D[pg_ind_scenario]);

	// repop density texture
	glActiveTexture(GL_TEXTURE0 + pg_enum_RepopDensity_Update_sampler);
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_BG_CA_repop_density]
		&& BG_CA_repop_density >= 0
		&& int(BG_CA_repop_density) < int(pg_RepopDensity_texture_texID[pg_ind_scenario].size())) {
		glBindTexture(GL_TEXTURE_RECTANGLE, pg_RepopDensity_texture_texID[pg_ind_scenario].at(BG_CA_repop_density));
	}
	else {
		glBindTexture(GL_TEXTURE_RECTANGLE, NULL_ID);
	}

	// photo[0] texture
	glActiveTexture(GL_TEXTURE0 + pg_enum_Photo0_Update_sampler);
	if (pg_playing_clipNoLeft >= 0
		&& pg_playing_clipNoLeft < pg_nbClips[pg_ind_scenario]
		&& pg_firstCompressedClipFramesInFolder[pg_ind_scenario][pg_playing_clipNoLeft]
		+ pg_all_clip_status[pg_enum_clipLeft].get_lastFrame(0) < pg_nbCompressedClipFrames[pg_ind_scenario]
		&& pg_firstCompressedClipFramesInFolder[pg_ind_scenario][pg_playing_clipNoLeft]
		+ pg_all_clip_status[pg_enum_clipLeft].get_lastFrame(0) >= 0) {
		glBindTexture(GL_TEXTURE_2D, pg_ClipFrames_buffer_data[pg_ind_scenario][pg_firstCompressedClipFramesInFolder[pg_ind_scenario][pg_playing_clipNoLeft]
			+ pg_all_clip_status[pg_enum_clipLeft].get_lastFrame(0)]->texBuffID);
	}
	else if (photo_diaporama >= 0
		&& photo_diaporama < int(pg_compressedImageData[pg_ind_scenario].size())
		&& pg_compressedImageData[pg_ind_scenario][photo_diaporama].size() > 0
		&& pg_Photo_swap_buffer_data[0].indSwappedPhoto >= 0
		&& pg_Photo_swap_buffer_data[0].indSwappedPhoto < int(pg_compressedImageData[pg_ind_scenario][photo_diaporama].size())) {
		//printf("diapo %d tex buf1 ID %d\n", photo_diaporama, 
		//	pg_compressedImageData[pg_ind_scenario][photo_diaporama][pg_Photo_swap_buffer_data[0].indSwappedPhoto].texBuffID);
		glBindTexture(GL_TEXTURE_2D, pg_compressedImageData[pg_ind_scenario][photo_diaporama][pg_Photo_swap_buffer_data[0].indSwappedPhoto].texBuffID);
	}
	else {
		glBindTexture(GL_TEXTURE_2D, NULL_ID);
	}

	// photo[1] texture
	glActiveTexture(GL_TEXTURE0 + pg_enum_Photo1_Update_sampler);
	if (pg_playing_clipNoRight >= 0
		&& pg_playing_clipNoRight < pg_nbClips[pg_ind_scenario]
		&& pg_firstCompressedClipFramesInFolder[pg_ind_scenario][pg_playing_clipNoRight]
		+ pg_all_clip_status[pg_enum_clipRight].get_lastFrame(0) < pg_nbCompressedClipFrames[pg_ind_scenario]
		&& pg_firstCompressedClipFramesInFolder[pg_ind_scenario][pg_playing_clipNoRight]
		+ pg_all_clip_status[pg_enum_clipRight].get_lastFrame(0) >= 0) {
		glBindTexture(GL_TEXTURE_2D, pg_ClipFrames_buffer_data[pg_ind_scenario][pg_firstCompressedClipFramesInFolder[pg_ind_scenario][pg_playing_clipNoRight]
			+ pg_all_clip_status[pg_enum_clipRight].get_lastFrame(0)]->texBuffID);
	}
	else if (photo_diaporama >= 0
		&& photo_diaporama < int(pg_compressedImageData[pg_ind_scenario].size())
		&& pg_compressedImageData[pg_ind_scenario][photo_diaporama].size() > 0
		&& pg_Photo_swap_buffer_data[1].indSwappedPhoto >= 0
		&& pg_Photo_swap_buffer_data[1].indSwappedPhoto < int(pg_compressedImageData[pg_ind_scenario][photo_diaporama].size())) {
		//printf("diapo %d tex buf2 ID %d\n", photo_diaporama, 
		//	pg_compressedImageData[pg_ind_scenario][photo_diaporama][pg_Photo_swap_buffer_data[1].indSwappedPhoto].texBuffID);
		glBindTexture(GL_TEXTURE_2D, pg_compressedImageData[pg_ind_scenario][photo_diaporama][pg_Photo_swap_buffer_data[1].indSwappedPhoto].texBuffID);
	}
	else {
		//printf("null photo texture\n");
		glBindTexture(GL_TEXTURE_2D, NULL_ID);
	}

	if (PG_NB_PARALLEL_CLIPS >= 2) {
		// clip[0] texture
	// second clips on left and right
		glActiveTexture(GL_TEXTURE0 + pg_enum_SecondClipLeft_Update_sampler);
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_clipCaptFreq]
			&& pg_playing_secondClipNoLeft >= 0
			&& pg_playing_secondClipNoLeft < pg_nbClips[pg_ind_scenario]
			&& pg_firstCompressedClipFramesInFolder[pg_ind_scenario][pg_playing_secondClipNoLeft]
			+ pg_all_clip_status[pg_enum_clipLeft].get_lastFrame(1) < pg_nbCompressedClipFrames[pg_ind_scenario]
			&& pg_firstCompressedClipFramesInFolder[pg_ind_scenario][pg_playing_secondClipNoLeft]
			+ pg_all_clip_status[pg_enum_clipLeft].get_lastFrame(1) >= 0) {
			glBindTexture(GL_TEXTURE_2D, pg_ClipFrames_buffer_data[pg_ind_scenario][pg_firstCompressedClipFramesInFolder[pg_ind_scenario][pg_playing_secondClipNoLeft]
				+ pg_all_clip_status[pg_enum_clipLeft].get_lastFrame(1)]->texBuffID);
		}
		else {
			glBindTexture(GL_TEXTURE_2D, NULL_ID);
		}

		// clip[1] texture
		glActiveTexture(GL_TEXTURE0 + pg_enum_SecondClipRight_Update_sampler);
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_clipCaptFreq]
			&& pg_playing_secondClipNoRight >= 0
			&& pg_playing_secondClipNoRight < pg_nbClips[pg_ind_scenario]
			&& pg_firstCompressedClipFramesInFolder[pg_ind_scenario][pg_playing_secondClipNoRight]
			+ pg_all_clip_status[pg_enum_clipRight].get_lastFrame(1) < pg_nbCompressedClipFrames[pg_ind_scenario]
			&& pg_firstCompressedClipFramesInFolder[pg_ind_scenario][pg_playing_secondClipNoRight]
			+ pg_all_clip_status[pg_enum_clipRight].get_lastFrame(1) >= 0) {
			glBindTexture(GL_TEXTURE_2D, pg_ClipFrames_buffer_data[pg_ind_scenario][pg_firstCompressedClipFramesInFolder[pg_ind_scenario][pg_playing_secondClipNoRight]
				+ pg_all_clip_status[pg_enum_clipRight].get_lastFrame(1)]->texBuffID);
		}
		else {
			//printf("null second photo texture\n");
			glBindTexture(GL_TEXTURE_2D, NULL_ID);
		}
	}

	// FBO capture of particle rendering used for flashing layers with particles
	glActiveTexture(GL_TEXTURE0 + pg_enum_Particle_render_FBO_Update_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Particle_render_texID);

	// 2-cycle ping-pong BG track step n (FBO attachment 5) -- current Frame
	glActiveTexture(GL_TEXTURE0 + pg_enum_Trk0_FBO_Update_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[(pg_FrameNo % 2) * pg_enum_FBO_Update_nbAttachts + pg_enum_Trk0_FBO_Update_attacht]);

	// 2-cycle ping-pong track 1 step n (FBO attachment 6) -- current Frame
	glActiveTexture(GL_TEXTURE0 + pg_enum_Trk1_FBO_Update_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[(pg_FrameNo % 2) * pg_enum_FBO_Update_nbAttachts + pg_enum_Trk1_FBO_Update_attacht]);

	// 2-cycle ping-pong track 2 step n (FBO attachment 7) -- current Frame
	glActiveTexture(GL_TEXTURE0 + pg_enum_Trk2_FBO_Update_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[(pg_FrameNo % 2) * pg_enum_FBO_Update_nbAttachts + pg_enum_Trk2_FBO_Update_attacht]);

	// 2-cycle ping-pong track 2 step n (FBO attachment 8) -- current Frame
	glActiveTexture(GL_TEXTURE0 + pg_enum_Trk3_FBO_Update_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[(pg_FrameNo % 2) * pg_enum_FBO_Update_nbAttachts + pg_enum_Trk3_FBO_Update_attacht]);

	// Initial background texture
	glActiveTexture(GL_TEXTURE0 + pg_enum_Camera_BGIni_FBO_Update_sampler);
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_camera_BG_subtr]) {
		glBindTexture(GL_TEXTURE_RECTANGLE, pg_camera_BGIni_texture_texID);
	}
	else {
		glBindTexture(GL_TEXTURE_RECTANGLE, NULL_ID);
	}

	// pixel acceleration  (FBO attachment 11)
	//printf("pixel image acceleration (image based acceleration) %d size %d\n", pixel_image_acceleration, pg_pixel_acc_texID.size());
	glActiveTexture(GL_TEXTURE0 + pg_enum_pixel_image_acc_Update_sampler);
	// acceleration is taken from photo 
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_pixel_image_acceleration] 
		&& pixel_image_acceleration >= 0
		&& pixel_image_acceleration < int(pg_pixel_acc_texID[pg_ind_scenario].size())) {
		glBindTexture(GL_TEXTURE_RECTANGLE, pg_pixel_acc_texID[pg_ind_scenario].at(pixel_image_acceleration)); // color RGB - rad 
		//printf("pixel image acceleration (image based acceleration) %d\n", pixel_image_acceleration);
	}
	else {
		glBindTexture(GL_TEXTURE_RECTANGLE, NULL_ID);
	}

	// CA Data table (FBO attachment 11)
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_CATable]) {
		glActiveTexture(GL_TEXTURE0 + pg_enum_CATable_Update_sampler);
		glBindTexture(GL_TEXTURE_RECTANGLE, pg_CATable_ID);
	}

	// draw points from the currently bound VAO with current in-use shader
	// glDrawArrays (GL_TRIANGLES, 0, 3 * PG_SIZE_QUAD_ARRAY);
	// Index buffer for indexed rendering
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pg_vboID[pg_enum_EABQuad]);
	// Draw the triangles !
	glDrawElements(
		GL_TRIANGLE_STRIP,      // mode
		3 * PG_SIZE_QUAD_ARRAY,    // count
		GL_UNSIGNED_INT,   // type
		(void*)0           // element array buffer offset
	);

	pg_printOglError(50);
}

/////////////////////////////////////
// PASS #2: TEXT, ClipArt CLIP ART RENDERING PASS
void pg_ClipArtRenderingPass(void) {
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_activeClipArts] 
		&& pg_shader_programme[pg_ind_scenario][pg_enum_shader_ClipArt]
		&& pg_has_NV_path_rendering) {
		if (pg_FrameNo > 0) {
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, pg_FBO_ClipArtRendering);
		}

		glClearColor(0.0, 0.0, 0.0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		////////////////////////////////////////
		// ClipArt TEXT OR CLIP ART RENDERING    
		////////////////////////////////////////
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_moving_messages] && moving_messages >= 0) {
			/* pg_Project_Etoiles TEASER */
			/*
			if (pg_CurrentSceneIndex == 0) {
				pg_Display_ClipArt_Text(&pg_Ind_Current_DisplayText, 0);
			}
			else if (pg_CurrentSceneIndex == 5) {
				pg_Display_ClipArt_Text(&pg_Ind_Current_DisplayText, 1);
			}
			:*/
			pg_Display_ClipArt_Text(&pg_Ind_Current_DisplayText, moving_messages);
		}
		else {
			pg_Display_All_ClipArt(activeClipArts);
		}
		pg_printOglError(47);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/////////////////////////////////////
// PASS #3: PARTICLE RENDERING PASS
void pg_ParticleRenderingPass(void) {
	if (!pg_shader_programme[pg_ind_scenario][pg_enum_shader_ParticleRender]) {
		return;
	}

	// printf("nb paticles %d\^n", PG_NB_PARTICLES);
	// draws the Bezier curve

	if (pg_FrameNo > 0) {
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, pg_FBO_ParticleRendering);
	}

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	////////////////////////////////////////
	// PARTICLE RENDERING    
	////////////////////////////////////////
	if (particle_geometry == 0) {
		glClear(GL_DEPTH_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
		//glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
	}

	//glDisable(GL_BLEND);
	//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	////////////////////////////////////////
	// activate shaders and sets uniform variable values    
	glUseProgram(pg_shader_programme[pg_ind_scenario][pg_enum_shader_ParticleRender]);

	glUniformMatrix4fv(uniform_ParticleSplat_vp_proj[pg_ind_scenario], 1, GL_FALSE, pg_orthoWindowProjMatrix);
	glUniformMatrix4fv(uniform_ParticleSplat_vp_view[pg_ind_scenario], 1, GL_FALSE, pg_identityViewMatrix);
	glUniformMatrix4fv(uniform_ParticleSplat_vp_model[pg_ind_scenario], 1, GL_FALSE, pg_identityModelMatrix);

	// texture unit location
	// position/speed Particle update
	glUniform1i(uniform_ParticleSplat_texture_vp_Part_pos_speed[pg_ind_scenario], 0);
	// color/radius Particle update
	glUniform1i(uniform_ParticleSplat_texture_vp_Part_col_rad[pg_ind_scenario], 1);

	if (particle_geometry == 2) {
		// curve particle texture
		glUniform1i(uniform_ParticleCurve_Curve_texture_fs_decal[pg_ind_scenario], 2);
	}
	else if (particle_geometry == 0) {
		// blurred disk texture
		glUniform1i(uniform_ParticleSplat_texture_fs_decal[pg_ind_scenario], 2);
	}
	pg_printOglError(48);

	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// 2-cycle ping-pong pos/speed
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_ParticleAnimation_texID[(pg_FrameNo % 2) * pg_enum_FBO_ParticleAnimation_nbAttachts + pg_enum_Part_pos_speed_FBO_ParticleAnimation_attacht]);

	// 2-cycle ping-pong color/radius
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_ParticleAnimation_texID[(pg_FrameNo % 2) * pg_enum_FBO_ParticleAnimation_nbAttachts + pg_enum_Part_col_rad_FBO_ParticleAnimation_attacht]);

	if (particle_geometry == 2) {
		// curve particle texture
		glActiveTexture(GL_TEXTURE0 + 2);
		glBindTexture(GL_TEXTURE_2D, pg_curve_particle_2D_texID[pg_ind_scenario]);
	}
	else if (particle_geometry == 0) {
		// blurred disk texture
		glActiveTexture(GL_TEXTURE0 + 2);
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_partSplat_texture]) {
			if (partSplat_texture > 0
				&& partSplat_texture - 1 < int(pg_blurredDisk_texture_2D_texID[pg_ind_scenario].size())) {
				// printf("bind splat texture %d %d\n", partSplat_texture, pg_blurredDisk_texture_2D_texID[pg_ind_scenario].at(partSplat_texture - 1));
				glBindTexture(GL_TEXTURE_2D, pg_blurredDisk_texture_2D_texID[pg_ind_scenario].at(partSplat_texture - 1));
			}
			else {
				//printf("no splat texture %d\n", partSplat_texture);
				glBindTexture(GL_TEXTURE_2D, NULL_ID);
			}
		}
		else {
			if (pg_blurredDisk_texture_2D_texID[pg_ind_scenario].size() > 0) {
				glBindTexture(GL_TEXTURE_2D, pg_blurredDisk_texture_2D_texID[pg_ind_scenario].at(0));
			}
			else {
				glBindTexture(GL_TEXTURE_2D, NULL_ID);
			}
		}
	}
	pg_printOglError(49);

	////////////////////////////////////////
	// binds geometry and displays it    
	// vertex buffer for a patch, made of 4 points: 4 x 3 floats
	glBindVertexArray(pg_vaoID[pg_enum_VAOParticle]);

	if (particle_geometry == 2) {
		// patch vertices for curve particles fed into tesselation shader
		glPatchParameteri(GL_PATCH_VERTICES, (part_curve_degree + 1));  // number of vertices in each patch
	}

	// Index buffer for indexed rendering
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pg_vboID[pg_enum_EAOParticle]);

	if (particle_geometry == 2) {
		// Draw the patches !
		glDrawElements(
			GL_PATCHES,      // mode
			PG_NB_PARTICLES * (part_curve_degree + 1),    // count
			GL_UNSIGNED_INT,   // type
			(void*)0           // element array buffer offset
		);
	}
	else {
		// Draw the patches !
		glDrawElements(
			GL_POINTS,      // mode
			PG_NB_PARTICLES,    // count
			GL_UNSIGNED_INT,   // type
			(void*)0           // element array buffer offset
		);
	}

	// unbinds VBO
	glBindVertexArray(0);
	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// quad texture with transparency
	if (particle_geometry == 0) {
		glDisable(GL_BLEND);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

//////////////////////////////////////////////////
// PASS #4: COMPOSITION + PING-PONG ECHO: ECHOED MIX OF LAYERS

void pg_MixingPass(void) {
	if (!pg_shader_programme[pg_ind_scenario][pg_enum_shader_Mixing]) {
		return;
	}

	// outputs inside a buffer that can be used for accumulation
	if (pg_FrameNo > 0) {
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, pg_FBO_Mixing_capturedFB_prec); //  drawing memory on odd and even frames for echo
		pg_bindFBOTextures(pg_FBO_Mixing_capturedFB_prec, pg_FBO_Mixing_capturedFB_prec_texID + (pg_FrameNo % 2), 1, false, 0);
	}

	// output video buffer clean-up
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	if (pg_isClearEcho == 1) {
		// printf("VideoPb clear echo buffer\n");
		pg_isClearEcho = 0;
		return;
	}

	/////////////////////////////////////////////////////////
	// draws the main rectangular surface with 

	////////////////////////////////////////
	// activate shaders and sets uniform variable values    
	glUseProgram(pg_shader_programme[pg_ind_scenario][pg_enum_shader_Mixing]);
	glBindVertexArray(pg_vaoID[pg_enum_VAOQuad]);

	glUniformMatrix4fv(uniform_Mixing_vp_proj[pg_ind_scenario], 1, GL_FALSE, pg_orthoWindowProjMatrix);
	glUniformMatrix4fv(uniform_Mixing_vp_view[pg_ind_scenario], 1, GL_FALSE, pg_identityViewMatrix);
	glUniformMatrix4fv(uniform_Mixing_vp_model[pg_ind_scenario], 1, GL_FALSE, pg_identityModelMatrix);

	// texture unit location
	glUniform1i(uniform_Mixing_texture_fs_CA[pg_ind_scenario], pg_enum_CA_FBO_Mixing_sampler);
	glUniform1i(uniform_Mixing_texture_fs_ClipArt_render[pg_ind_scenario], pg_enum_ClipArt_render_FBO_Mixing_sampler);
	glUniform1i(uniform_Mixing_texture_fs_Particle_render[pg_ind_scenario], pg_enum_Particle_render_FBO_Mixing_sampler);
	glUniform1i(uniform_Mixing_texture_fs_Render_prec[pg_ind_scenario], pg_enum_Render_prec_FBO_Mixing_sampler);

	glUniform1i(uniform_Mixing_texture_fs_Screen_Font[pg_ind_scenario], pg_enum_Screen_Font_FBO_Mixing_sampler);
	glUniform1i(uniform_Mixing_texture_fs_Screen_Message[pg_ind_scenario], pg_enum_Screen_Message_FBO_Mixing_sampler);

	glUniform1i(uniform_Mixing_texture_fs_Trk0[pg_ind_scenario], pg_enum_Trk0_FBO_Mixing_sampler);
	glUniform1i(uniform_Mixing_texture_fs_Trk1[pg_ind_scenario], pg_enum_Trk1_FBO_Mixing_sampler);
	glUniform1i(uniform_Mixing_texture_fs_Trk2[pg_ind_scenario], pg_enum_Trk2_FBO_Mixing_sampler);
	glUniform1i(uniform_Mixing_texture_fs_Trk3[pg_ind_scenario], pg_enum_Trk3_FBO_Mixing_sampler);

	glUniform1i(uniform_Mixing_texture_fs_Mask[pg_ind_scenario], pg_enum_Mask_FBO_Mixing_sampler);

	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// 2-cycle ping-pong CA step n + 1 (FBO attachment 0) -- next frame (outout from update pass)
	glActiveTexture(GL_TEXTURE0 + pg_enum_CA_FBO_Mixing_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * pg_enum_FBO_Update_nbAttachts + pg_enum_CA_FBO_Update_attacht]);

	// ClipArt GPU step n
	glActiveTexture(GL_TEXTURE0 + pg_enum_ClipArt_render_FBO_Mixing_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_ClipArt_render_texID);
	// Particles step n
	glActiveTexture(GL_TEXTURE0 + pg_enum_Particle_render_FBO_Mixing_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Particle_render_texID);

	// preceding snapshot for echo
	glActiveTexture(GL_TEXTURE0 + pg_enum_Render_prec_FBO_Mixing_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Mixing_capturedFB_prec_texID[((pg_FrameNo + 1) % 2)]);  // drawing memory on odd and even frames for echo 

	// screen font texture
	pg_bindFontTextureID();

	// message offsets texture
	pg_bindMessageTextureID();

	// 2-cycle ping-pong track 0 (FBO attachment 3) -- next frame (outout from update pass)
	glActiveTexture(GL_TEXTURE0 + pg_enum_Trk0_FBO_Mixing_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * pg_enum_FBO_Update_nbAttachts + pg_enum_Trk0_FBO_Update_attacht]);

	// 2-cycle ping-pong track 1 (FBO attachment 4) -- next frame (outout from update pass)
	glActiveTexture(GL_TEXTURE0 + pg_enum_Trk1_FBO_Mixing_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * pg_enum_FBO_Update_nbAttachts + pg_enum_Trk1_FBO_Update_attacht]);

	// 2-cycle ping-pong track 2 step n + 1 (FBO attachment 5) -- next frame (outout from update pass)
	glActiveTexture(GL_TEXTURE0 + pg_enum_Trk2_FBO_Mixing_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * pg_enum_FBO_Update_nbAttachts + pg_enum_Trk2_FBO_Update_attacht]);

	// 2-cycle ping-pong track 3 step n + 1 (FBO attachment 6) -- next frame (outout from update pass)
	glActiveTexture(GL_TEXTURE0 + pg_enum_Trk3_FBO_Mixing_sampler);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * pg_enum_FBO_Update_nbAttachts + pg_enum_Trk3_FBO_Update_attacht]);

	// Master mask texture
	glActiveTexture(GL_TEXTURE0 + pg_enum_Mask_FBO_Mixing_sampler);
	if (nb_layers_master_mask[pg_ind_scenario] > 0) {
		//printf("multilayer mask texture %d (%d layers)\n", pg_Master_Multilayer_Mask_texID[pg_ind_scenario], nb_layers_master_mask[pg_ind_scenario]);
		glBindTexture(GL_TEXTURE_3D, pg_Master_Multilayer_Mask_texID[pg_ind_scenario]);
	}
	else {
		//printf("single layer mask texture %d\n", pg_Master_Mask_texID[pg_ind_scenario]);
		glBindTexture(GL_TEXTURE_RECTANGLE, pg_Master_Mask_texID[pg_ind_scenario]);
	}

	// draw points from the currently bound VAO with current in-use shader
	// glDrawArrays (GL_TRIANGLES, 0, 3 * PG_SIZE_QUAD_ARRAY);
	// Index buffer for indexed rendering
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pg_vboID[pg_enum_EABQuad]);
	// Draw the triangles !
	glDrawElements(
		GL_TRIANGLE_STRIP,      // mode
		3 * PG_SIZE_QUAD_ARRAY,    // count
		GL_UNSIGNED_INT,   // type
		(void*)0           // element array buffer offset
	);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

//////////////////////////////////////////////////
// PASS #5: FINAL DISPLAY: MIX OF ECHOED AND NON-ECHOED LAYERS

void pg_MasterPass(void) {
	if (!pg_shader_programme[pg_ind_scenario][pg_enum_shader_Master]) {
		return;
	}
	// Augmented Reality: FBO capture of Master to be displayed on a mesh
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_activeMeshes]
		&& ((pg_FullScenarioActiveVars[pg_ind_scenario][_augmentedReality] && augmentedReality)
			|| (pg_FullScenarioActiveVars[pg_ind_scenario][_meshRenderBypass] && meshRenderBypass))) {
		if (pg_FrameNo > 0) {
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, pg_FBO_Master_capturedFB_prec); //  master output memory for mapping on mesh or bypassing mesh rendering
		}
	}
	else {
		// unbind output FBO 
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	}

	// sets viewport to double window
	glViewport(0, 0, PG_WINDOW_WIDTH, PG_WINDOW_HEIGHT);

	// output video buffer clean-up
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	////////////////////////////////////////
	// activate shaders and sets uniform variable values    
	if (pg_shader_programme[pg_ind_scenario][pg_enum_shader_Master]) {
		glUseProgram(pg_shader_programme[pg_ind_scenario][pg_enum_shader_Master]);
		glBindVertexArray(pg_vaoID[pg_enum_VAOQuadMaster]);

		glUniformMatrix4fv(uniform_Master_vp_proj[pg_ind_scenario], 1, GL_FALSE, pg_doubleProjMatrix);
		glUniformMatrix4fv(uniform_Master_vp_view[pg_ind_scenario], 1, GL_FALSE, pg_identityViewMatrix);
		glUniformMatrix4fv(uniform_Master_vp_model[pg_ind_scenario], 1, GL_FALSE, pg_identityModelMatrix);

		glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// texture unit locations
		glUniform1i(uniform_Master_texture_fs_Render_curr[pg_ind_scenario], pg_enum_Render_curr_FBO_Master_sampler);
		glUniform1i(uniform_Master_texture_fs_CA[pg_ind_scenario], pg_enum_CA_FBO_Master_sampler);

		glUniform1i(uniform_Master_texture_fs_ClipArt_render[pg_ind_scenario], pg_enum_ClipArt_render_FBO_Master_sampler);
		glUniform1i(uniform_Master_texture_fs_Particle_render[pg_ind_scenario], pg_enum_Particle_render_FBO_Master_sampler);
		glUniform1i(uniform_Master_texture_fs_Trk0[pg_ind_scenario], pg_enum_Trk0_FBO_Master_sampler);
		glUniform1i(uniform_Master_texture_fs_Trk1[pg_ind_scenario], pg_enum_Trk1_FBO_Master_sampler);
		glUniform1i(uniform_Master_texture_fs_Trk2[pg_ind_scenario], pg_enum_Trk2_FBO_Master_sampler);
		glUniform1i(uniform_Master_texture_fs_Trk3[pg_ind_scenario], pg_enum_Trk3_FBO_Master_sampler);

		glUniform1i(uniform_Master_texture_fs_Mask[pg_ind_scenario], pg_enum_Mask_FBO_Master_sampler);

		// Mixing pass output (echoed composition of tracks)
		glActiveTexture(GL_TEXTURE0 + pg_enum_Render_curr_FBO_Master_sampler);
		glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Mixing_capturedFB_prec_texID[(pg_FrameNo % 2)]);

		// 2-cycle ping-pong CA step n (FBO attachment 0) -- next frame (outout from update pass)
		glActiveTexture(GL_TEXTURE0 + pg_enum_CA_FBO_Master_sampler);
		glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * pg_enum_FBO_Update_nbAttachts + pg_enum_CA_FBO_Update_attacht]);

		// ClipArt GPU step n 
		glActiveTexture(GL_TEXTURE0 + pg_enum_ClipArt_render_FBO_Master_sampler);
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_activeClipArts]) {
			glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_ClipArt_render_texID);
		}
		else {
			glBindTexture(GL_TEXTURE_RECTANGLE, NULL_ID);
		}

		// Particles step n 
		glActiveTexture(GL_TEXTURE0 + pg_enum_Particle_render_FBO_Master_sampler);
		glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Particle_render_texID);

		// 2-cycle ping-pong track 0 step n (FBO attachment 3) -- next frame (outout from update pass)
		glActiveTexture(GL_TEXTURE0 + pg_enum_Trk0_FBO_Master_sampler);
		glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * pg_enum_FBO_Update_nbAttachts + pg_enum_Trk0_FBO_Update_attacht]);

		// 2-cycle ping-pong track 1 step n (FBO attachment 4) -- next frame (outout from update pass)
		glActiveTexture(GL_TEXTURE0 + pg_enum_Trk1_FBO_Master_sampler);
		glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * pg_enum_FBO_Update_nbAttachts + pg_enum_Trk1_FBO_Update_attacht]);

		// 2-cycle ping-pong track 2 step n (FBO attachment 5) -- next frame (outout from update pass)
		glActiveTexture(GL_TEXTURE0 + pg_enum_Trk2_FBO_Master_sampler);
		glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * pg_enum_FBO_Update_nbAttachts + pg_enum_Trk2_FBO_Update_attacht]);

		// 2-cycle ping-pong track 3 step n (FBO attachment 6) -- next frame (outout from update pass)
		glActiveTexture(GL_TEXTURE0 + pg_enum_Trk3_FBO_Master_sampler);
		glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * pg_enum_FBO_Update_nbAttachts + pg_enum_Trk3_FBO_Update_attacht]);

		// Master mask texture
		glActiveTexture(GL_TEXTURE0 + pg_enum_Mask_FBO_Master_sampler);
		if (nb_layers_master_mask[pg_ind_scenario] > 0) {
			//printf("multilayer mask texture %d (%d layers)\n", pg_Master_Multilayer_Mask_texID[pg_ind_scenario], nb_layers_master_mask[pg_ind_scenario]);
			glBindTexture(GL_TEXTURE_3D, pg_Master_Multilayer_Mask_texID[pg_ind_scenario]);
		}
		else {
			//printf("single layer mask texture %d\n", pg_Master_Mask_texID[pg_ind_scenario]);
			glBindTexture(GL_TEXTURE_RECTANGLE, pg_Master_Mask_texID[pg_ind_scenario]);
		}

		//if (pg_FrameNo % 1000 <= 1) {
		//	printf("Final check texID 0-5 %d %d %d %d %d %d\n\n",
		//		pg_FBO_Mixing_capturedFB_prec_texID[(pg_FrameNo % 2)],
		//		pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * pg_enum_FBO_Update_nbAttachts],
		//		pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * pg_enum_FBO_Update_nbAttachts + 2],
		//		pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * pg_enum_FBO_Update_nbAttachts + 3],
		//		pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * pg_enum_FBO_Update_nbAttachts + 4],
		//		pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * pg_enum_FBO_Update_nbAttachts + 5]);
		//		pg_FBO_Update_texID[((pg_FrameNo + 1) % 2) * pg_enum_FBO_Update_nbAttachts + 5]);
		//}

		// draw points from the currently bound VAO with current in-use shader
		// glDrawArrays (GL_TRIANGLES, 0, 3 * PG_SIZE_QUAD_ARRAY);
		// Index buffer for indexed rendering
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pg_vboID[pg_enum_EABQuadMaster]);
		// Draw the triangles !
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawElements(
			GL_TRIANGLE_STRIP,      // mode
			3 * PG_SIZE_QUAD_ARRAY,    // count
			GL_UNSIGNED_INT,   // type
			(void*)0           // element array buffer offset
		);
	}
}

//////////////////////////////////////////////////
// PASS #5: SENSOR PASS
void pg_SensorPass(void) {
	if (!pg_FullScenarioActiveVars[pg_ind_scenario][_sensor_layout]
		|| !pg_shader_programme[pg_ind_scenario][pg_enum_shader_Sensor]) {
		return;
	}
	////////////////////////////////////////
	// drawing sensors
	// activate transparency
	//printf("sensor pass\n");
	glEnable(GL_BLEND);
	glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

	// activate shaders and sets uniform variable values    
	glUseProgram(pg_shader_programme[pg_ind_scenario][pg_enum_shader_Sensor]);
	glBindVertexArray(pg_quadSensor_vao);

	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// texture unit location
	glUniform1i(uniform_Sensor_texture_fs_decal[pg_ind_scenario], 0);
	// previous pass output
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_RECTANGLE, Sensor_texture_rectangle[pg_ind_scenario]);

	glUniformMatrix4fv(uniform_Sensor_vp_view[pg_ind_scenario], 1, GL_FALSE, pg_identityViewMatrix);
	glUniformMatrix4fv(uniform_Sensor_vp_proj[pg_ind_scenario], 1, GL_FALSE, pg_doubleProjMatrix);
	pg_printOglError(53);

	for (int indSens = 0; indSens < PG_NB_SENSORS; indSens++) {
		int reindexed_Sensor = pg_Sensor_order[indSens];
		if (pg_sensor_onOff[reindexed_Sensor]) {
			//printf("sensor index %d reindex %d\n", indSens, reindexed_Sensor);
			pg_modelMatrixSensor[12] = (pg_sensorPositions[3 * reindexed_Sensor] + 0.5f - pg_workingWindow_width / 2.0f) + pg_workingWindow_width / 2.0f;
			pg_modelMatrixSensor[13] = (pg_sensorPositions[3 * reindexed_Sensor + 1] + 0.5f - PG_WINDOW_HEIGHT / 2.0f) + PG_WINDOW_HEIGHT / 2.0f;
			pg_modelMatrixSensor[14] = pg_sensorPositions[3 * reindexed_Sensor + 2];
			//printf("left sensor %d %.3f %.3f %.3f\n", indSens, pg_modelMatrixSensor[12], pg_modelMatrixSensor[13], pg_modelMatrixSensor[14]);
			glUniformMatrix4fv(uniform_Sensor_vp_model[pg_ind_scenario], 1, GL_FALSE, pg_modelMatrixSensor);

			// pg_sensorLevel[indSens]
			glUniform4f(uniform_Sensor_fs_4fv_onOff_isCurrentSensor_masterLevel_transparency[pg_ind_scenario],
				(pg_sensor_onOff[reindexed_Sensor] ? 1.0f : -1.0f), (reindexed_Sensor == pg_currentSensor ? 1.0f : -1.0f),
				master, pg_sensorLevel[reindexed_Sensor]);

			// draw points from the currently bound VAO with current in-use shader
			glDrawArrays(GL_TRIANGLES, 0, 3 * 2);
		}
	}
	pg_printOglError(52);

	// duplicates the sensors in case of double window
	if (double_window) {
		int pg_rightWindowVMargin = 0;
		if (PG_WINDOW_WIDTH > 1920) {
			pg_rightWindowVMargin = (PG_WINDOW_WIDTH - 2 * pg_workingWindow_width) / 2;
		}
		for (int indSens = 0; indSens < PG_NB_SENSORS; indSens++) {
			int reindexed_Sensor = pg_Sensor_order[indSens];
			if (pg_sensor_onOff[reindexed_Sensor]) {
				pg_modelMatrixSensor[12] = (pg_sensorPositions[3 * reindexed_Sensor] + 0.5f - pg_workingWindow_width / 2.0f) + pg_workingWindow_width / 2.0f
					+ pg_rightWindowVMargin + pg_workingWindow_width;
				pg_modelMatrixSensor[13] = (pg_sensorPositions[3 * reindexed_Sensor + 1] + 0.5f - PG_WINDOW_HEIGHT / 2.0f) + PG_WINDOW_HEIGHT / 2.0f;
				pg_modelMatrixSensor[14] = pg_sensorPositions[3 * reindexed_Sensor + 2];
				//printf("right sensor %d %.3f %.3f %.3f\n", indSens, pg_modelMatrixSensor[12], pg_modelMatrixSensor[13], pg_modelMatrixSensor[14]);
				glUniformMatrix4fv(uniform_Sensor_vp_model[pg_ind_scenario], 1, GL_FALSE, pg_modelMatrixSensor);

				glUniform4f(uniform_Sensor_fs_4fv_onOff_isCurrentSensor_masterLevel_transparency[pg_ind_scenario],
					(pg_sensor_onOff[reindexed_Sensor] ? 1.0f : -1.0f), (reindexed_Sensor == pg_currentSensor ? 1.0f : -1.0f),
					master, pg_sensorLevel[reindexed_Sensor]);

				// draw points from the currently bound VAO with current in-use shader
				glDrawArrays(GL_TRIANGLES, 0, 3 * 2);
			}
		}
	}

	pg_printOglError(51);
	glDisable(GL_BLEND);
}

//////////////////////////////////////////////////
// PASS #6: MESH PASS
// MESH ANIMATIONS FOR PROJECTS
bool Etoiles_mesh_guided_by_strokes(int indMeshFile) {
	// meshes are guided by strokes
	int path_no = indMeshFile + 1;
	bool visible = false;

	// brings the path coordinates to the normal cube
	if (path_no <= PG_NB_PATHS) {
		visible = (pg_is_path_replay[path_no] && pg_paths_currentDynPoint[path_no].pg_paths_x > 0 && pg_paths_currentDynPoint[path_no].pg_paths_y > 0);
		if (visible) {
			// normal pen coordinates
			pen_x = (pg_paths_currentDynPoint[path_no].pg_paths_x / pg_workingWindow_width) * 2.f - 1.f;
			pen_y = (pg_paths_currentDynPoint[path_no].pg_paths_y / PG_WINDOW_HEIGHT) * 2.f - 1.f;
		}
		else {
			visible = path_record[path_no] && pg_paths_currentDynPoint[0].pg_paths_x > 0 && pg_paths_currentDynPoint[0].pg_paths_y > 0;
			if (visible) {
				// normal pen coordinates
				pen_x = (pg_paths_currentDynPoint[0].pg_paths_x / pg_workingWindow_width) * 2.f - 1.f;
				pen_y = (pg_paths_currentDynPoint[0].pg_paths_y / PG_WINDOW_HEIGHT) * 2.f - 1.f;
			}
		}
	}
	return visible;
}

// PROJECT BASED MESH ANIMATION
void Etoiles_ray_animation(int indMeshFile) {
	// rotates and scales a ray so that it follows a pen
	// vector from ray center to pen
	vec_x = pen_x + pg_Meshes[pg_ind_scenario][indMeshFile].pg_Mesh_Translation_X;
	vec_y = pen_y + pg_Meshes[pg_ind_scenario][indMeshFile].pg_Mesh_Translation_Y;
	// angle from ray center to pen -> ray angle
	if (vec_x != 0) {
		pg_Meshes[pg_ind_scenario][indMeshFile].pg_Mesh_Rotation_angle = atan(vec_y / vec_x);
	}
	else {
		pg_Meshes[pg_ind_scenario][indMeshFile].pg_Mesh_Rotation_angle = 0;
	}
	if (vec_x > 0) {
		pg_Meshes[pg_ind_scenario][indMeshFile].pg_Mesh_Rotation_angle += float(M_PI);
	}
	// ray size so that the tip of the ray coincides with the pen
	float norm_vec = sqrt(vec_x * vec_x + vec_y * vec_y);
	pg_Meshes[pg_ind_scenario][indMeshFile].pg_Mesh_Scale = norm_vec;
}


#if defined(var_Caverne_Mesh_Profusion)
void Caverne_profusion_automatic_rotation(int indMeshFile) {
	// rotation update
	if (indMeshFile < 7) {
		pg_Meshes[pg_ind_scenario][indMeshFile].pg_Mesh_Rotation_angle += 0.03f;
		pg_Meshes[pg_ind_scenario][indMeshFile].pg_Mesh_Rotation_X += 0.01f;
		pg_Meshes[pg_ind_scenario][indMeshFile].pg_Mesh_Rotation_Y += 0.01f;
		pg_Meshes[pg_ind_scenario][indMeshFile].pg_Mesh_Rotation_Z += 0.01f;
		// translation update
		if (mobileMeshes & (1 << indMeshFile)) {
			pg_Meshes[pg_ind_scenario][indMeshFile].pg_Mesh_Translation_X += pg_Meshes[pg_ind_scenario][indMeshFile].pg_Mesh_Motion_X * 2.f;
			pg_Meshes[pg_ind_scenario][indMeshFile].pg_Mesh_Translation_Y += pg_Meshes[pg_ind_scenario][indMeshFile].pg_Mesh_Motion_Y * 2.f;
			pg_Meshes[pg_ind_scenario][indMeshFile].pg_Mesh_Translation_Z += pg_Meshes[pg_ind_scenario][indMeshFile].pg_Mesh_Motion_Z * 2.f;
		}
	}
	else {
		pg_Meshes[pg_ind_scenario][indMeshFile].pg_Mesh_Rotation_angle += 0.06f * rand_0_1;
		pg_Meshes[pg_ind_scenario][indMeshFile].pg_Mesh_Rotation_X += (0.2f * rand_0_1 - 0.1f);
		pg_Meshes[pg_ind_scenario][indMeshFile].pg_Mesh_Rotation_Y += (0.2f * rand_0_1 - 0.1f);
		pg_Meshes[pg_ind_scenario][indMeshFile].pg_Mesh_Rotation_Z += (0.2f * rand_0_1 - 0.1f);
		// translation update
		pg_Meshes[pg_ind_scenario][indMeshFile].pg_Mesh_Translation_X += pg_Meshes[pg_ind_scenario][indMeshFile].pg_Mesh_Motion_X * 4.f * (rand_0_1 - 0.5f);
		pg_Meshes[pg_ind_scenario][indMeshFile].pg_Mesh_Translation_Y += pg_Meshes[pg_ind_scenario][indMeshFile].pg_Mesh_Motion_Y * 4.f * (rand_0_1 - 0.5f);
		pg_Meshes[pg_ind_scenario][indMeshFile].pg_Mesh_Translation_Z += pg_Meshes[pg_ind_scenario][indMeshFile].pg_Mesh_Motion_Z * 4.f * (rand_0_1 - 0.5f);
	}
}
#elif defined(var_MmeShanghai_brokenGlass)
void MmeShanghai_automatic_brokenGlass_animation(int indMeshFile, int indObjectInMesh) {
	float animationTime = float(pg_CurrentClockTime - pg_Meshes[pg_ind_scenario][indMeshFile].pg_MmeShanghaiMeshObjectWakeupTime[indObjectInMesh]);
	pg_Meshes[pg_ind_scenario][indMeshFile].pg_MmeShanghai_Object_Translation_Y[indObjectInMesh] = -0.0065f * animationTime * animationTime;
	pg_Meshes[pg_ind_scenario][indMeshFile].pg_MmeShanghai_Object_Rotation_angle[indObjectInMesh] += pg_Meshes[pg_ind_scenario][indMeshFile].pg_MmeShanghai_Object_Rotation_Ini_angle[indObjectInMesh];
}
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////
// MESH RENDERING
//////////////////////////////////////////////////////////////////////////////////////////////////////

void pg_MeshPass(void) {
	float eyePosition[3] = { 20.f, 0.f, 0.f };
	float lookat[3] = { 0.f, 0.f, 0.f };

	if (!pg_shader_programme[pg_ind_scenario][pg_enum_shader_Mesh]) {
		return;
	}

	// Augmented Reality: FBO capture of Master to be displayed on a mesh

	// Meshes rendering
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_activeMeshes]) {
		// mesh geometry rendering
		if (!pg_FullScenarioActiveVars[pg_ind_scenario][_meshRenderBypass] || !meshRenderBypass) {
			// draws the mesh on top of clip arts or particles if they exit (before mixing/master rendering)
			if (!pg_FullScenarioActiveVars[pg_ind_scenario][_augmentedReality] 	|| !augmentedReality) {
				// draws the mesh alone
				// unbind output FBO 
				glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

				// draws on back buffer
				glDrawBuffer(GL_BACK);

				// no transparency
				glDisable(GL_BLEND);

				// draws the mesh on top clip arts if they exit
				if (pg_FullScenarioActiveVars[pg_ind_scenario][_activeClipArts]) {
					// draws the meshes on top of clipArt
					if (pg_FrameNo > 0) {
						glBindFramebuffer(GL_DRAW_FRAMEBUFFER, pg_FBO_ClipArtRendering);
					}
				}
				// draws the mesh on top particles if they exit
				else {
					// draws the meshes on top of particles
					if (pg_FrameNo > 0) {
						glBindFramebuffer(GL_DRAW_FRAMEBUFFER, pg_FBO_ParticleRendering);
					}
				}
			}
			else {
				// draws the mesh alone after mixing/master rendering
				// unbind output FBO 
				glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

				// draws on back buffer
				glDrawBuffer(GL_BACK);

				// no transparency
				glDisable(GL_BLEND);
			}

			// transparency
			glEnable(GL_BLEND);
			// glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
			glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

			// output buffer cleanup
			glClear(GL_DEPTH_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);
			//glDepthMask(GL_FALSE);
			//glDepthFunc(GL_LESS);

			////////////////////////////////////////
			// drawing meshes

			// activate shaders and sets uniform variable values    
			glUseProgram(pg_shader_programme[pg_ind_scenario][pg_enum_shader_Mesh]);

			// calculates the view and perspective matrices according to the parameters in the scenario file
			// for one camera if single screen, and two otherwise
#if !defined(var_MmeShanghai_brokenGlass)
			pg_calculate_perspective_matrices();
#else
			pg_calculate_orthographic_matrices();
#endif

			if (pg_FullScenarioActiveVars[pg_ind_scenario][_mesh_homography]) {
				if (mesh_homography) {
					glm::highp_mat4 homographyMatrix;

					// Read points
					std::vector<cv::Point2f> sourcePoints;
					//std::vector<cv::Point2f> source2Points;
					std::vector<cv::Point2f> destinationPoints;

					sourcePoints.push_back(cv::Point2f(-VP1KeystoneXBottomLeft,
						-VP1KeystoneYBottomLeft));
					sourcePoints.push_back(cv::Point2f(VP1KeystoneXBottomRight,
						-VP1KeystoneYBottomRight));
					sourcePoints.push_back(cv::Point2f(VP1KeystoneXTopRight,
						VP1KeystoneYTopRight));
					sourcePoints.push_back(cv::Point2f(-VP1KeystoneXTopLeft,
						VP1KeystoneYTopLeft));

					destinationPoints.push_back(cv::Point2f(-1.0f, -1.0f));
					destinationPoints.push_back(cv::Point2f(1.0f, -1.0f));
					destinationPoints.push_back(cv::Point2f(1.0f, 1.0f));
					destinationPoints.push_back(cv::Point2f(-1.0f, 1.0f));

					// initializes the homography matrices for the distortion of the projected image
					pg_calculate_homography_matrices(&sourcePoints, &destinationPoints, pg_matValues, 4);
					homographyMatrix = (glm::make_mat4(pg_matValues));
				}
			}

			// sets viewport to single window
			if (double_window) {
				glViewport(0, 0, pg_workingWindow_width, PG_WINDOW_HEIGHT);
			}
			else {
				glViewport(0, 0, pg_workingWindow_width, PG_WINDOW_HEIGHT);
			}

			// projection and view matrices
			glUniformMatrix4fv(uniform_Mesh_vp_proj[pg_ind_scenario], 1, GL_FALSE,
				// glm::value_ptr(pg_VP1homographyMatrix * pg_VP1perspMatrix));
				glm::value_ptr(pg_VP1perspMatrix));
			glUniformMatrix4fv(uniform_Mesh_vp_view[pg_ind_scenario], 1, GL_FALSE,
				glm::value_ptr(pg_VP1viewMatrix));

			// all the meshes
			for (unsigned int indMeshFile = 0; indMeshFile < pg_Meshes[pg_ind_scenario].size(); indMeshFile++) {
				pg_Meshes[pg_ind_scenario][indMeshFile].pg_drawOneMesh(indMeshFile);
			} // all the meshes
			pg_printOglError(66);
		}
		// direct rendering
		else {
			// bypassing mesh rendering: outputs master output memory 
			glBindFramebuffer(GL_READ_FRAMEBUFFER, pg_FBO_Master_capturedFB_prec); 
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
			glDrawBuffer(GL_BACK);
			glBlitFramebuffer(0, 0, pg_workingWindow_width, PG_WINDOW_HEIGHT, 0, 0, pg_workingWindow_width, PG_WINDOW_HEIGHT,
				GL_COLOR_BUFFER_BIT, GL_NEAREST);
			glBindFramebuffer(GL_READ_FRAMEBUFFER, 0); //  unbind read buffer
			pg_printOglError(55);
		}

#if defined(PG_SECOND_MESH_CAMERA)
		if (!directRenderingwithoutMeshScreen2) {
			// sets viewport to second window
			glViewport(pg_workingWindow_width, 0, pg_workingWindow_width, PG_WINDOW_HEIGHT);

			// duplicates the Meshs in case of double window

			glUniformMatrix4fv(uniform_Mesh_vp_proj[pg_ind_scenario], 1, GL_FALSE,
				// glm::value_ptr(pg_VP2homographyMatrix * pg_VP2perspMatrix));
				glm::value_ptr(pg_VP2perspMatrix));
			glUniformMatrix4fv(uniform_Mesh_vp_view[pg_ind_scenario], 1, GL_FALSE,
				glm::value_ptr(pg_VP2viewMatrix));

			// all mesh files
			for (int indMeshFile = 0; indMeshFile < pg_Meshes[pg_ind_scenario].size(); indMeshFile++) {
				pg_Meshes[pg_ind_scenario][indMeshFile].pg_drawOneMesh2(indMeshFile);
			} // all the meshes

			// sets viewport to full window
			glViewport(0, 0, PG_WINDOW_WIDTH, PG_WINDOW_HEIGHT);

			glDisable(GL_DEPTH_TEST);
			// no transparency
			glDisable(GL_BLEND);
			pg_printOglError(54);
		}
		else {
			//printf("Direct copy of Master shader output right window to back buffer\n");
			// bypassing mesh rendering: outputs master output memory 
			glBindFramebuffer(GL_READ_FRAMEBUFFER, pg_FBO_Master_capturedFB_prec);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
			glDrawBuffer(GL_BACK);
			glBlitFramebuffer(pg_workingWindow_width, 0, 2 * pg_workingWindow_width, PG_WINDOW_HEIGHT, pg_workingWindow_width, 0, 2 * pg_workingWindow_width, PG_WINDOW_HEIGHT,
				GL_COLOR_BUFFER_BIT, GL_NEAREST);
			glBindFramebuffer(GL_READ_FRAMEBUFFER, 0); //  unbind read buffer
			pg_printOglError(56);
		}
#endif
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// DRAWING A SCENE : SCREEN SHOT OR INTERACTIVE RENDERING
//////////////////////////////////////////////////////////////////////////////////////////////////////

void pg_draw_scene(DrawingMode mode) {
	// ******************** Svg output ********************
	if (mode == pg_enum_render_Svg) {
		cv::String imageFileName;
		pg_Svg_Capture_param.indImgSnapshot++;

		imageFileName = format("%s%s-%s-%04d-%02d.svg",
			pg_snapshots_dir_path_name.c_str(),
			pg_Svg_Capture_param.Img_file_name.c_str(),
			pg_date_stringStream.str().c_str(),
			pg_Svg_Capture_param.indImgSnapshot,
			pg_CurrentSceneIndex + 1);
		pg_logCurrentLineSceneVariables(imageFileName);

		pg_writesvg(imageFileName);
	}

	// ******************** Png output ********************
	else if (mode == pg_enum_render_Png) {
		//pDataWritePng.w = pg_workingWindow_width;
		//pDataWritePng.h = PG_WINDOW_HEIGHT;
		cv::String imageFileName;

		pg_Png_Capture_param.indImgSnapshot++;

		imageFileName = format("%s%s-%s-%04d-%02d.png",
			pg_snapshots_dir_path_name.c_str(),
			pg_Png_Capture_param.Img_file_name.c_str(),
			pg_date_stringStream.str().c_str(),
			pg_Png_Capture_param.indImgSnapshot,
			pg_CurrentSceneIndex + 1);
		struct stat buffer;
		int count = 0;
		while (stat(imageFileName.c_str(), &buffer) == 0) {
			imageFileName = format("%s%s-%s-%04d-%04d-%02d.png",
				pg_snapshots_dir_path_name.c_str(),
				pg_Png_Capture_param.Img_file_name.c_str(),
				pg_date_stringStream.str().c_str(),
				pg_Png_Capture_param.indImgSnapshot, 
				count,
				pg_CurrentSceneIndex + 1);
			count++;
		}
		pg_logCurrentLineSceneVariables(imageFileName);

		glReadBuffer(GL_BACK);

		// OpenGL's default 4 byte pack alignment would leave extra bytes at the
		//   end of each image row so that each full row contained a number of bytes
		//   divisible by 4.  Ie, an RGB row with 3 pixels and 8-bit componets would
		//   be laid out like "RGBRGBRGBxxx" where the last three "xxx" bytes exist
		//   just to pad the row out to 12 bytes (12 is divisible by 4). To make sure
		//   the rows are packed as tight as possible (no row padding), set the pack
		//   alignment to 1.
		glPixelStorei(GL_PACK_ALIGNMENT, 1);

		int singleWindowWidth = PG_WINDOW_WIDTH;
		if (double_window == true) {
			singleWindowWidth = PG_WINDOW_WIDTH / 2;
		}
		pngImgMatRGBInitial.create(PG_WINDOW_HEIGHT, singleWindowWidth, CV_8UC3); // GL_RGB
		glReadPixels(0, 0, singleWindowWidth, PG_WINDOW_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, pngImgMatRGBInitial.data);
		pg_printOglError(67);

		pg_writepng(imageFileName);
	}

	// ******************** Jpg output ********************
	else if (mode == pg_enum_render_Jpg) {
		//pDataWriteJpg.w = pg_workingWindow_width;
		//pDataWriteJpg.h = PG_WINDOW_HEIGHT;
		cv::String imageFileName;
	
		pg_Jpg_Capture_param.indImgSnapshot++;

		imageFileName = format("%s%s-%s-%04d-%02d.jpg",
			pg_snapshots_dir_path_name.c_str(),
			pg_Jpg_Capture_param.Img_file_name.c_str(),
			pg_date_stringStream.str().c_str(),
			pg_Jpg_Capture_param.indImgSnapshot,
			pg_CurrentSceneIndex + 1);
		struct stat buffer;
		int count = 0;
		while (stat(imageFileName.c_str(), &buffer) == 0) {
			imageFileName = format("%s%s-%s-%04d-%03d-%02d.jpg",
				pg_snapshots_dir_path_name.c_str(),
				pg_Jpg_Capture_param.Img_file_name.c_str(),
				pg_date_stringStream.str().c_str(),
				pg_Jpg_Capture_param.indImgSnapshot, count,
				pg_CurrentSceneIndex + 1);
			count++;
		}
		pg_logCurrentLineSceneVariables(imageFileName);
		printf("Snapshot jpg step %d (%s)\n",
			pg_Jpg_Capture_param.indImgSnapshot,
			imageFileName.c_str());

		glReadBuffer(GL_BACK);

		// OpenGL's default 4 byte pack alignment would leave extra bytes at the
		//   end of each image row so that each full row contained a number of bytes
		//   divisible by 4.  Ie, an RGB row with 3 pixels and 8-bit componets would
		//   be laid out like "RGBRGBRGBxxx" where the last three "xxx" bytes exist
		//   just to pad the row out to 12 bytes (12 is divisible by 4). To make sure
		//   the rows are packed as tight as possible (no row padding), set the pack
		//   alignment to 1.
		glPixelStorei(GL_PACK_ALIGNMENT, 1);

		int singleWindowWidth = PG_WINDOW_WIDTH;
		if (double_window == true) {
			singleWindowWidth = PG_WINDOW_WIDTH / 2;
		}
		jpgImgMatRGBInitial.create(PG_WINDOW_HEIGHT, singleWindowWidth, CV_8UC3); // GL_RGB
		glReadPixels(0, 0, singleWindowWidth, PG_WINDOW_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE,
			jpgImgMatRGBInitial.data);
		pg_printOglError(67);

		pg_writejpg(imageFileName);
	}

	// ******************** interactive output ********************
	else if (mode == pg_enum_render_Display) {

		// sets viewport to single window
		glViewport(0, 0, pg_workingWindow_width, PG_WINDOW_HEIGHT);

		glDisable(GL_BLEND);

		//////////////////////////////////////
		// particle animation pass #0
		pg_ParticleAnimationPass();
		pg_printOglError(57);

		//////////////////////////////////////
		// update pass #1 
		// image layers update pass (CA, PIXELS, DRAWING, VIDEO AND PHOTO...)
		pg_UpdatePass();
		pg_printOglError(58);

		//////////////////////////////////////
		// ClipArt clip art pass #2
		pg_ClipArtRenderingPass();
		pg_printOglError(59);

		//////////////////////////////////////
		// particle pass #3
		pg_ParticleRenderingPass();
		pg_printOglError(60);

		//////////////////////////////////////
		// mesh pass #3b
		// the meshes are displayed together with the particles except for augmented reality or mesh rendering bypassing
		// where they are displayed last
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_activeMeshes]) {
			if ((!pg_FullScenarioActiveVars[pg_ind_scenario][_augmentedReality] || !augmentedReality)
				&& (!pg_FullScenarioActiveVars[pg_ind_scenario][_meshRenderBypass] || !meshRenderBypass)) {
				if (activeMeshes > 0) {
					pg_MeshPass();
				}
			}
		}

		//////////////////////////////////////
		// layer compositing & echo pass #3
		pg_MixingPass();
		pg_printOglError(61);

		//////////////////////////////////////
		// final combination of echoed and non echoed rendering #4
		pg_MasterPass();
		pg_printOglError(62);

		//////////////////////////////////////
		// additional sensor pass on top of final rendering
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_sensor_layout]) {
			bool oneSensorActiveMin = false;
			for (int indSens = 0; indSens < PG_NB_SENSORS; indSens++) {
				int reindexed_Sensor = pg_Sensor_order[indSens];
				if (pg_sensor_onOff[reindexed_Sensor]) {
					oneSensorActiveMin = true;
					break;
				}
			}
			if (oneSensorActiveMin) {
				pg_SensorPass();
				pg_printOglError(63);

				// ////////////////////////
				// read sensor values on CA (non echoed) and send messages
				if (pg_FrameNo >= 10 + first_frame_number || pg_CurrentSceneIndex > 0) {
					pg_readSensors();
					pg_printOglError(65);
				}
			}
			// incremental sensor activation every 45 sec. = 720/16
			for (int indSens = 0; indSens < PG_NB_SENSORS; indSens++) {
				int reindexed_Sensor = pg_Sensor_order[indSens];
				if (!pg_sensor_onOff[reindexed_Sensor]) {
					//printf("pg_CurrentClockTime - pg_sensor_last_activation_time %f\n", pg_CurrentClockTime - pg_sensor_last_activation_time);
					if (sensor_activation == 5
						&& pg_CurrentClockTime - pg_sensor_last_activation_time > 45) {
						pg_sensor_last_activation_time = pg_CurrentClockTime;
						pg_sensor_onOff[reindexed_Sensor] = true;
						break;
					}
				}
			}
		}


#if defined(pg_Project_CAaudio) && defined(PG_PUREDATA_SOUND) && defined(var_nb_CATypes)
		// drawing memory on odd and even frames for CA	
		glBindFramebuffer(GL_READ_FRAMEBUFFER, pg_FBO_Mixing_capturedFB_prec); // drawing memory on odd and even frames for echo and sensors	
		pg_bindFBOTextures(pg_FBO_Mixing_capturedFB_prec, pg_FBO_Mixing_capturedFB_prec_texID + (pg_FrameNo % 2), 1, false, 0);
		pg_playChord();
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
#endif

		//////////////////////////////////////
		// last mesh pass with final rendering texture
		// for augmented reality
		// Augmented Reality: FBO capture of Master to be displayed on a mesh
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_activeMeshes]) {
			if ((pg_FullScenarioActiveVars[pg_ind_scenario][_augmentedReality] && augmentedReality)
				|| (pg_FullScenarioActiveVars[pg_ind_scenario][_meshRenderBypass] && meshRenderBypass)) {
				if (activeMeshes > 0) {
					pg_MeshPass();
				}
			}
		}
	}
	pg_printOglError(64);
}

