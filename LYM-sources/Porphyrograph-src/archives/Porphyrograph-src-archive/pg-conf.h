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
	double *midTermValueArray = NULL;
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
			if (pg_FullScenarioVarIndiceRanges[indP][0] == -1) {
				scene_initial_parameters[indP].init_ScenarioValue(0., "", NULL, 0);
				scene_final_parameters[indP].init_ScenarioValue(0., "", NULL, 0);
			}
			else {
				scene_initial_parameters[indP].init_ScenarioValue(0., "", NULL, pg_FullScenarioVarIndiceRanges[indP][1]);
				scene_final_parameters[indP].init_ScenarioValue(0., "", NULL, pg_FullScenarioVarIndiceRanges[indP][1]);
			}
		}
		scene_interpolations = new pg_Interpolation[_MaxInterpVarIDs];
		for (int indP = 0; indP < _MaxInterpVarIDs; indP++) {
			scene_interpolations[indP].interpolation_mode = pg_linear_interpolation;
			scene_interpolations[indP].initialization_mode = pg_scenario_initial;
			scene_interpolations[indP].offSet = 0.0;
			scene_interpolations[indP].duration = 1.0;
			if (pg_FullScenarioVarIndiceRanges[indP][0] != -1) {
				scene_interpolations[indP].midTermValueArray = new double[pg_FullScenarioVarIndiceRanges[indP][1]];
			}
		}
	}
};

extern bool					    pg_last_scene_update;
extern int						pg_NbConfigurations;
extern vector<Scene>           pg_Scenario[PG_MAX_CONFIGURATIONS];
extern double                   current_scene_percent;
extern bool						pg_FullScenarioActiveVars[PG_MAX_CONFIGURATIONS][_MaxInterpVarIDs];

// PNG capture
class pg_Png_Capture {
public:
	string                   Png_file_name;
	int                      beginPng;
	int                      endPng;
	int                      stepPngInFrames;
	double                   stepPngInSeconds;
	double                   nextPngCapture;
	bool                     outputPng;
	int					    indPngSnapshot;
};
extern pg_Png_Capture pg_Png_Capture_param;

// JPG capture
class pg_Jpg_Capture {
public:
	string                   Jpg_file_name;
	int                      beginJpg;
	int                      endJpg;
	int                      stepJpgInFrames;
	double                   stepJpgInSeconds;
	double                   nextJpgCapture;
	bool                     outputJpg;
	int						indJpgSnapshot;
};
extern pg_Jpg_Capture pg_Jpg_Capture_param;

// Svg capture
class pg_Svg_Capture {
public:
	string                   Svg_file_name;
	int                      beginSvg;
	int                      endSvg;
	int                      stepSvgInFrames;
	double                   stepSvgInSeconds;
	double                   nextSvgCapture;
	bool                     outputSvg;
	int						indSvgSnapshot;
};
extern pg_Svg_Capture pg_Svg_Capture_param;

// VIDEO capture
class pg_Video_Capture {
public:
	string                   Video_file_name;
	int                      beginVideo_file;
	int                      endVideo_file;
	bool                     outputVideo_file;
};
extern pg_Video_Capture pg_Video_Capture_param;

// UDP serversisClearAllLayersnd clients
extern vector<pg_IPServer>		IP_Servers;
extern vector<pg_IPClient>		IP_Clients;

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
// color
enum pg_ClipArt_Colors_Types { ClipArt_nat = 0, ClipArt_white, ClipArt_red, ClipArt_green, ClipArt_blue, ClipArt_yellow, ClipArt_cyan, ClipArt_magenta, ClipArt_black };
class ClipArt {
public:
	// number of paths for each file
	int pg_nb_paths_in_ClipArt;
	// file names
	string pg_ClipArt_fileNames;
	// geometrical transformations
	float pg_ClipArt_Scale;
	float pg_ClipArt_Rotation;
	float pg_ClipArt_Translation_X;
	float pg_ClipArt_Translation_Y;
	// color
	pg_ClipArt_Colors_Types* pg_ClipArt_Colors;
	// subpath display
	bool* pg_ClipArt_SubPath;
	// base ID of the GPU files
	GLuint ClipArt_file_baseID;
	ClipArt(void) {
		// number of paths for each file
		pg_nb_paths_in_ClipArt = 0;
		// file names
		pg_ClipArt_fileNames = "";
		// geometrical transformations
		pg_ClipArt_Scale = 1.f;
		pg_ClipArt_Rotation = 0.f;
		pg_ClipArt_Translation_X = 0.f;
		pg_ClipArt_Translation_Y = 0.f;
		// color
		pg_ClipArt_Colors = NULL;
		// subpath display
		pg_ClipArt_SubPath = NULL;
		// base ID of the GPU files
		ClipArt_file_baseID = NULL_ID;
	}
	~ClipArt(void) {
	}
};
extern vector<ClipArt>pg_ClipArts[PG_MAX_CONFIGURATIONS];
// total number of paths
extern int pg_nb_tot_SvgGpu_paths[PG_MAX_CONFIGURATIONS];
// lastisClearAllLayersctivated SvgGpu
extern int pg_last_activated_ClipArt;
// base ID of the GPU paths
extern GLuint ClipArt_path_baseID[PG_MAX_CONFIGURATIONS];
// fill color table
extern unsigned int** ClipArt_path_fill_color[PG_MAX_CONFIGURATIONS];

class MeshData {
public:
	string pg_Mesh_fileNames;
	// geometrical transformations
	float pg_Mesh_Scale;
	float pg_Mesh_Rotation_angle;
	float pg_Mesh_Rotation_X;
	float pg_Mesh_Rotation_Y;
	float pg_Mesh_Rotation_Z;
	float pg_Mesh_Translation_X;
	float pg_Mesh_Translation_Y;
	float pg_Mesh_Translation_Z;
	float pg_Mesh_Rotation_Ini_X;
	float pg_Mesh_Rotation_Ini_Y;
	float pg_Mesh_Rotation_Ini_Z;
	float pg_Mesh_Translation_Ini_X;
	float pg_Mesh_Translation_Ini_Y;
	float pg_Mesh_Translation_Ini_Z;
	float pg_Mesh_Motion_X;
	float pg_Mesh_Motion_Y;
	float pg_Mesh_Motion_Z;
	int pg_Mesh_TextureRank;
	int pg_nbObjectsPerMeshFile;
	int* pg_nbFacesPerMeshFile;
	// var_MmeShanghai_brokenGlass #0
#if defined(var_MmeShanghai_brokenGlass)
	bool* pg_MmeShanghaiActveMeshObjects;
	double* pg_MmeShanghaiMeshObjectWakeupTime;
	bool** pg_MmeShanghai_MeshSubParts;
	string* pg_MmeShanghai_MeshSubPart_FileNames;
	int pg_MmeShanghai_NbMeshSubParts;
	float* pg_MmeShanghai_Object_Rotation_angle;
	float* pg_MmeShanghai_Object_Rotation_X;
	float* pg_MmeShanghai_Object_Rotation_Y;
	float* pg_MmeShanghai_Object_Rotation_Z;
	float* pg_MmeShanghai_Object_Translation_X;
	float* pg_MmeShanghai_Object_Translation_Y;
	float* pg_MmeShanghai_Object_Translation_Z;
	float* pg_MmeShanghai_Object_Rotation_Ini_angle;
#endif
#if defined(var_Caverne_Mesh_Profusion)
	bool pg_CaverneActveMesh;
	float pg_CaverneMeshWakeupTime;
	float pg_CaverneMeshBirthTime;
	float pg_CaverneMeshDeathTime;
#endif
	// color
	array<float, 4> pg_Mesh_Colors;
	// textures
	GLuint Mesh_texture_rectangle;
	vector<unsigned int> mesh_vao;
	vector<array<float, 3>> mesh_barycenter;
	vector<unsigned int> mesh_index_vbo;
	MeshData(int indConfiguration) {
		pg_Mesh_fileNames = "";
		// geometrical transformations
		pg_Mesh_Scale = 1.f;
		pg_Mesh_Rotation_angle = 0.f;
		pg_Mesh_Rotation_X = 0.f;
		pg_Mesh_Rotation_Y = 0.f;
		pg_Mesh_Rotation_Z = 0.f;
		pg_Mesh_Translation_X = 0.f;
		pg_Mesh_Translation_Y = 0.f;
		pg_Mesh_Translation_Z = 0.f;
		pg_Mesh_Rotation_Ini_X = 0.f;
		pg_Mesh_Rotation_Ini_Y = 0.f;
		pg_Mesh_Rotation_Ini_Z = 0.f;
		pg_Mesh_Translation_Ini_X = 0.f;
		pg_Mesh_Translation_Ini_Y = 0.f;
		pg_Mesh_Translation_Ini_Z = 0.f;
		pg_Mesh_Motion_X = 0.f;
		pg_Mesh_Motion_Y = 0.f;
		pg_Mesh_Motion_Z = 0.f;
		pg_Mesh_TextureRank = 0;
		pg_Mesh_Colors[0] = 1.f;
		pg_Mesh_Colors[1] = 1.f;
		pg_Mesh_Colors[2] = 1.f;
		pg_Mesh_Colors[3] = 1.f;
		pg_nbObjectsPerMeshFile = 0;
		pg_nbFacesPerMeshFile = NULL;
		Mesh_texture_rectangle = NULL_ID;
		mesh_vao = {};
		mesh_barycenter = {};
		mesh_index_vbo = {};
#if defined(var_Caverne_Mesh_Profusion)
		if (pg_FullScenarioActiveVars[indConfiguration][_Caverne_Mesh_Profusion]) {
			pg_CaverneActveMesh = false;
			if (indFile < 7) {
				pg_CaverneMeshWakeupTime = float(rand_0_1 * 10.);
			}
			else {
				pg_CaverneMeshWakeupTime = float(rand_0_1 * 30.);
			}
			pg_CaverneMeshBirthTime = 0.f;
			pg_CaverneMeshDeathTime = 0.f;
		}
#endif
#if defined(var_MmeShanghai_brokenGlass)
		if (pg_FullScenarioActiveVars[indConfiguration][_MmeShanghai_brokenGlass]) {
			pg_MmeShanghai_MeshSubParts = NULL;
			pg_MmeShanghai_NbMeshSubParts = 0;
			pg_MmeShanghai_MeshSubPart_FileNames = NULL;
		}
		pg_MmeShanghaiActveMeshObjects = NULL;
		pg_MmeShanghaiMeshObjectWakeupTime = NULL;
		pg_MmeShanghai_MeshSubParts = NULL;
		pg_MmeShanghai_MeshSubPart_FileNames = NULL;
		pg_MmeShanghai_NbMeshSubParts = NULL;
		pg_MmeShanghai_Object_Rotation_angle = NULL;
		pg_MmeShanghai_Object_Rotation_X = NULL;
		pg_MmeShanghai_Object_Rotation_Y = NULL;
		pg_MmeShanghai_Object_Rotation_Z = NULL;
		pg_MmeShanghai_Object_Translation_X = NULL;
		pg_MmeShanghai_Object_Translation_Y = NULL;
		pg_MmeShanghai_Object_Translation_Z = NULL;
		pg_MmeShanghai_Object_Rotation_Ini_angle = NULL;
#endif
#if defined(var_Caverne_Mesh_Profusion)
		pg_CaverneActveMesh = false;
		pg_CaverneMeshWakeupTime = 0.f;
		pg_CaverneMeshBirthTime = 0.f;
		pg_CaverneMeshDeathTime = 0.f;
#endif
	}
	~MeshData(void) {
	}
};

// MESHES
extern vector<MeshData> pg_Meshes[PG_MAX_CONFIGURATIONS];
extern int pg_last_activated_Mesh;

// BACKGROUND COLOR
extern float BGcolorRed_prec;
extern float BGcolorGreen_prec;
extern float BGcolorBlue_prec;
extern bool BGcolorFlash;
extern bool BGcolorFlash_prec;

float my_stof(string str);
int my_stoi(string str);
double my_stod(string str);

// TEXTURES
// file names
extern vector<pg_TextureData> pg_Textures[PG_MAX_CONFIGURATIONS];

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

bool isFullPath(string dir_or_filename);
void completeToFullPath(string& dir_or_filename);
	
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
