/*! \file pg-conf.h
 * 
 * 
* 
 *     File pg-conf.h
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

#ifndef PG_CONF_H
#define PG_CONF_H

enum InterpolationType { pg_linear_interpolation = 0, pg_cosine_interpolation, pg_bell_interpolation, pg_bezier_interpolation, pg_sawtooth_interpolation, pg_stepwise_interpolation, pg_keep_value, pg_exponential_interpolation, EmptyInterpolationType };
enum InitializationType { pg_scenario_initial = 0, pg_current_value, EmptyInitializationType };

struct pg_Interpolation
{
	InterpolationType interpolation_mode;
	InitializationType initialization_mode;
	double offSet;
	double exponent;
	double duration;
	double midTermValue;
};

class pg_Window {
  public:
  /// glut ID for the window
  int                     glutID;

  pg_Window ();
  ~pg_Window();
};

class Scene {
public:
	double                  scene_duration;
	bool                    scene_change_when_ends;
	double                  scene_originalDuration;
	double                  scene_initial_time;
	double                  scene_originalInitial_time;
	double                  scene_final_time;
	double                  scene_originalFinal_time;
	string                  scene_IDs;
	string                  scene_Msg1;
	string                  scene_Msg2;
	ScenarioValue*          scene_initial_parameters;
	ScenarioValue*          scene_final_parameters;
	pg_Interpolation*       scene_interpolations;

	void init_scene(void) {
		scene_duration = 0.0;
		scene_change_when_ends = true;
		scene_initial_time = 0.0;
		scene_final_time = 0.0;
		scene_originalDuration = 0.0;
		scene_originalInitial_time = 0.0;
		scene_originalFinal_time = 0.0;

		scene_initial_parameters = new ScenarioValue[_MaxInterpVarIDs];
		scene_final_parameters = new ScenarioValue[_MaxInterpVarIDs];
		for (int indP = 0; indP < _MaxInterpVarIDs; indP++) {
			scene_initial_parameters[indP] = ScenarioValue();
			scene_final_parameters[indP] = ScenarioValue();
		}
		scene_interpolations = new pg_Interpolation[_MaxInterpVarIDs];
		for (int indP = 0; indP < _MaxInterpVarIDs; indP++) {
			scene_interpolations[indP].interpolation_mode = pg_linear_interpolation;
			scene_interpolations[indP].initialization_mode = pg_scenario_initial;
			scene_interpolations[indP].offSet = 0.0;
			scene_interpolations[indP].duration = 1.0;
		}
	}
};

extern int                      pg_NbScenes[_NbConfigurations];
extern bool					    pg_last_scene_update;
extern Scene*                   pg_Scenario[_NbConfigurations];
extern double                   current_scene_percent;

// PNG capture
extern string                   Png_file_name;
extern int                      beginPng;
extern int                      endPng;
extern int                      stepPngInFrames;
extern double                   stepPngInSeconds;
extern double                   nextPngCapture;
extern bool                     outputPng;
extern int					    indPngSnapshot;

// JPG capture
extern string                   Jpg_file_name;
extern int                      beginJpg;
extern int                      endJpg;
extern int                      stepJpgInFrames;
extern double                   stepJpgInSeconds;
extern double                   nextJpgCapture;
extern bool                     outputJpg;
extern int						indJpgSnapshot;

// ClipArt capture
extern string                   Svg_file_name;
extern int                      beginSvg;
extern int                      endSvg;
extern int                      stepSvgInFrames;
extern double                   stepSvgInSeconds;
extern double                   nextSvgCapture;
extern bool                     outputSvg;
extern int						indSvgSnapshot;

// VIDEO capture
extern string                   Video_file_name;
extern int                      beginVideo_file;
extern int                      endVideo_file;
extern bool                     outputVideo_file;

// UDP serversisClearAllLayersnd clients
extern vector<pg_IPServer*>		IP_Servers;
extern int                      nb_IP_Servers;
extern vector<pg_IPClient*>		IP_Clients;
extern int                      nb_IP_Clients;

extern string					pg_csv_file_name;
extern string					snapshots_dir_path_name;
extern string					screen_font_file_name;
extern GLuint					Screen_Font_texture_Rectangle_texID;
extern pg_TextureData           screenFontTexData;
extern int                      screen_font_size;

// nb configuration and scenario files
extern string *pg_ConfigurationFileNames;
extern string *pg_ScenarioFileNames;

// shader file names
extern string ** pg_Shader_File_Names;
extern GLenum *** pg_Shader_Stages;
extern int ** pg_Shader_nbStages;

// ClipArt GPU
// number of files
extern int pg_nb_ClipArt[_NbConfigurations];
// number of paths for each file
extern int *pg_nb_paths_in_ClipArt[_NbConfigurations];
// total number of paths
extern int pg_nb_tot_SvgGpu_paths[_NbConfigurations];
// file names
extern string *pg_ClipArt_fileNames[_NbConfigurations];
// geometrical transformations
extern float *pg_ClipArt_Scale[_NbConfigurations];
extern float *pg_ClipArt_Rotation[_NbConfigurations];
extern float *pg_ClipArt_Translation_X[_NbConfigurations];
extern float *pg_ClipArt_Translation_Y[_NbConfigurations];
// lastisClearAllLayersctivated SvgGpu
extern int pg_last_activated_ClipArt;
// lastisClearAllLayersctivated Mesh
extern int pg_last_activated_Mesh;
// color
enum pg_ClipArt_Colors_Types { ClipArt_nat = 0, ClipArt_white, ClipArt_red, ClipArt_green, ClipArt_blue, ClipArt_yellow, ClipArt_cyan, ClipArt_magenta, ClipArt_black};
extern pg_ClipArt_Colors_Types** pg_ClipArt_Colors[_NbConfigurations];
// subpath display
extern bool** pg_ClipArt_SubPath[_NbConfigurations];

#if defined(var_activeMeshes)
// MESHES
// number of meshe files
extern int pg_nb_Mesh_files[_NbConfigurations];
extern int pg_nb_Mesh_objects[_NbConfigurations];
// file names
extern string *pg_Mesh_fileNames[_NbConfigurations];
// geometrical transformations
extern float *pg_Mesh_Scale[_NbConfigurations];
extern float *pg_Mesh_Rotation_angle[_NbConfigurations];
extern float* pg_Mesh_Rotation_X[_NbConfigurations];
extern float* pg_Mesh_Rotation_Y[_NbConfigurations];
extern float* pg_Mesh_Rotation_Z[_NbConfigurations];
extern float* pg_Mesh_Translation_X[_NbConfigurations];
extern float* pg_Mesh_Translation_Y[_NbConfigurations];
extern float* pg_Mesh_Translation_Z[_NbConfigurations];
extern float* pg_Mesh_Rotation_Ini_X[_NbConfigurations];
extern float* pg_Mesh_Rotation_Ini_Y[_NbConfigurations];
extern float* pg_Mesh_Rotation_Ini_Z[_NbConfigurations];
extern float* pg_Mesh_Translation_Ini_X[_NbConfigurations];
extern float* pg_Mesh_Translation_Ini_Y[_NbConfigurations];
extern float* pg_Mesh_Translation_Ini_Z[_NbConfigurations];
extern float *pg_Mesh_Motion_X[_NbConfigurations];
extern float *pg_Mesh_Motion_Y[_NbConfigurations];
extern float *pg_Mesh_Motion_Z[_NbConfigurations];
extern int *pg_Mesh_TextureRank[_NbConfigurations];
#if defined(var_MmeShanghai_brokenGlass)
extern bool** pg_MmeShanghaiActveMeshObjects[_NbConfigurations];
extern double** pg_MmeShanghaiMeshObjectWakeupTime[_NbConfigurations];
extern bool*** pg_MmeShanghai_MeshSubParts[_NbConfigurations];
extern string** pg_MmeShanghai_MeshSubPart_FileNames[_NbConfigurations];
extern int* pg_MmeShanghai_NbMeshSubParts[_NbConfigurations];
extern float** pg_MmeShanghai_Object_Rotation_angle[_NbConfigurations];
extern float** pg_MmeShanghai_Object_Rotation_X[_NbConfigurations];
extern float** pg_MmeShanghai_Object_Rotation_Y[_NbConfigurations];
extern float** pg_MmeShanghai_Object_Rotation_Z[_NbConfigurations];
extern float** pg_MmeShanghai_Object_Translation_X[_NbConfigurations];
extern float** pg_MmeShanghai_Object_Translation_Y[_NbConfigurations];
extern float** pg_MmeShanghai_Object_Translation_Z[_NbConfigurations];
extern float** pg_MmeShanghai_Object_Rotation_Ini_angle[_NbConfigurations];
#endif
#if defined(var_Caverne_Mesh_Profusion)
extern bool* pg_CaverneActveMesh[_NbConfigurations];
extern float* pg_CaverneMeshWakeupTime[_NbConfigurations];
extern float* pg_CaverneMeshBirthTime[_NbConfigurations];
extern float* pg_CaverneMeshDeathTime[_NbConfigurations];
extern bool Caverne_BackColor[_NbConfigurations];
extern float Caverne_BackColorRed[_NbConfigurations];
extern float Caverne_BackColorGreen[_NbConfigurations];
extern float Caverne_BackColorBlue[_NbConfigurations];
extern float Caverne_BackColorRed_prec[_NbConfigurations];
extern float Caverne_BackColorGreen_prec[_NbConfigurations];
extern float Caverne_BackColorBlue_prec[_NbConfigurations];
extern bool Caverne_BackColorFlash[_NbConfigurations];
extern bool Caverne_BackColorFlash_prec[_NbConfigurations];
#endif
// color
extern float **pg_Mesh_Colors[_NbConfigurations];
// textures
extern GLuint *Mesh_texture_rectangle[_NbConfigurations];
#endif

float my_stof(string str);
int my_stoi(string str);
double my_stod(string str);

// TEXTURES
// number of Texture files
extern int pg_nb_Texture_files[_NbConfigurations];
// file names
extern vector<pg_TextureData *> pg_Textures[_NbConfigurations];

/// the params of the font file
extern string                    font_file_encoding;
extern TextureEncoding           font_texture_encoding;

// window(s) size and location
extern int my_window_x;
extern int my_window_y;

void light_channel_string_to_channel_no(string a_light_channel_string, int* light_channel, int* light_channel_fine);
void saveInitialTimesAndDurations(int indConfiguration);
void restoreInitialTimesAndDurations( void );
void pg_update_scene_variables(Scene* cur_scene, float elapsed_time_from_start);

void setWindowDimensions(void);

void parseConfigurationFile(std::ifstream& confFin, int fileRank);
int pg_varID_to_rank(string var_ID, int indConfig);
void parseScenarioFile(std::ifstream& scenarioFin, int fileRank);
void pg_LoadConfigurationFile(const char* confFileName, int fileRank);
void pg_LoadScenarioFile(const char* scenarioFileName, int fileRank);
void pg_listAllSVG_paths(void);
void pg_listAllSoundtracks(void);
void ParseScenarioClipArt(std::ifstream & scenarioFin, int indConfiguration);
void ParseScenarioSVGPaths(std::ifstream& scenarioFin, int indConfiguration);
void ParseScenarioSoundtracks(std::ifstream& scenarioFin, int indConfiguration);
void ParseScenarioClipsAndPhotos(std::ifstream& scenarioFin, int indConfiguration);
void ParseScenarioVideos(std::ifstream& scenarioFin, int indConfiguration);

void stringstreamStoreLine(std::stringstream* sstream, std::string* line);
int FindSceneById(std::string * sceneID);

#endif
