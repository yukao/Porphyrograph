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

enum InterpolationType { pg_enum_linear_interpolation = 0, pg_enum_cosine_interpolation, pg_enum_bell_interpolation, pg_enum_bezier_interpolation, pg_enum_sawtooth_interpolation, pg_enum_stepwise_interpolation, pg_enum_keep_value, pg_enum_exponential_interpolation, EmptyInterpolationType };
enum InitializationType { pg_enum_scenario_initial = 0, pg_current_value, EmptyInitializationType };

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
			scene_interpolations[indP].interpolation_mode = pg_enum_linear_interpolation;
			scene_interpolations[indP].initialization_mode = pg_enum_scenario_initial;
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

// nb configuration and scenario files
extern string *pg_ConfigurationFileNames;
extern string *pg_ScenarioFileNames;

// shader file names
extern string ** pg_Shader_File_Names;
extern GLenum *** pg_Shader_Stages;
extern int ** pg_Shader_nbStages;

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

// window(s) size and location
extern int my_window_x;
extern int my_window_y;

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
void ParseScenarioClipArt(std::ifstream & scenarioFin, int indConfiguration);
void ParseScenarioSVGPaths(std::ifstream& scenarioFin, int indConfiguration);
void ParseScenarioSoundtracks(std::ifstream& scenarioFin, int indConfiguration);
void ParseScenarioClipsAndPhotos(std::ifstream& scenarioFin, int indConfiguration);
void ParseScenarioVideos(std::ifstream& scenarioFin, int indConfiguration);

void stringstreamStoreLine(std::stringstream* sstream, std::string* line);
int FindSceneById(std::string * sceneID);

#endif
