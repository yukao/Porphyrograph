/*! \file pg-scenarioParse.h
 * 
 * 
* 
 *     File pg-scenarioParse.h
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

#ifndef PG_SCENARIOPARSE_H
#define PG_SCENARIOPARSE_H

//////////////////////////////////////////////////////////////////////////////////////////////////////
// CONSTs
//////////////////////////////////////////////////////////////////////////////////////////////////////

enum InterpolationType { pg_enum_linear_interpolation = 0, pg_enum_cosine_interpolation, pg_enum_bell_interpolation, pg_enum_bezier_interpolation, pg_enum_sawtooth_interpolation, pg_enum_stepwise_interpolation, pg_enum_keep_value, pg_enum_exponential_interpolation, EmptyInterpolationType };
enum InitializationType { pg_enum_scenario_initial = 0, pg_current_value, EmptyInitializationType };
enum CaptureType { pg_enum_Jpg = 0, pg_enum_Png, pg_enum_Svg, EmptyCaptureType };

//////////////////////////////////////////////////////////////////////////////////////////////////////
// VARIALBE INTERPOLATION STRUCT, WINDOW DATA AND SCENE MANAGEMENT CLASSES
////////////////////////////////////////////////////////////////////////////////////////////////////// 

struct pg_Interpolation {
	InterpolationType interpolation_mode;
	InitializationType initialization_mode;
	double offSet;
	double exponent;
	double duration;
	double midTermValue;
	double *midTermValueArray = NULL;
};

class pg_WindowData {
  public:
  /// glut ID for the window
  int                     glutID;

  pg_WindowData ();
  ~pg_WindowData();
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

//////////////////////////////////////////////////////////////////////////////////////////////////////
// SCREEN AND PATH CAPTURE MANAGEMENT CLASS
////////////////////////////////////////////////////////////////////////////////////////////////////// 

// IMG capture
class pg_Img_Capture {
public:
	CaptureType              imageFormat;
	string                   Img_file_name;
	int                      beginImg;
	int                      endImg;
	int                      stepImgInFrames;
	double                   stepImgInSeconds;
	double                   nextImgCapture;
	bool                     outputImg;
	int					    indImgSnapshot;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////
// EXPORTED VARIABLES
////////////////////////////////////////////////////////////////////////////////////////////////////// 

// ++++++++++++++++++++++ PATHS, IDs FROM CONFIGURATION FILE ++++++++++++++++++++ 
extern string pg_scripts_directory;
extern string pg_videos_directory;
extern string pg_soundtracks_directory;
extern string pg_cliparts_directory;
extern string pg_diaporamas_directory;
extern string pg_meshes_directory;
extern string pg_messages_directory;
extern string pg_SVGpaths_directory;
extern string pg_textures_directory;
extern string pg_clips_directory;
extern string pg_shaders_directory;

// captures of snapshots
extern pg_Img_Capture pg_Png_Capture_param;
extern pg_Img_Capture pg_Jpg_Capture_param;
extern pg_Img_Capture pg_Svg_Capture_param;

// csv log file name and path
extern string pg_csv_logFile_name;
extern string pg_snapshots_dir_path_name;

// scenes and interpolated variables
extern vector<Scene> pg_Scenario[PG_MAX_SCENARIOS];

// subset of variables of the full scenario found in the current scenario
extern bool	pg_FullScenarioActiveVars[PG_MAX_SCENARIOS][_MaxInterpVarIDs];
// number of vars in the scenario file
extern int pg_ScenarioVarNb[PG_MAX_SCENARIOS];
// rank of the scenario variable in the list of variables generated by python
extern int pg_ConfigScenarioVarRank[PG_MAX_SCENARIOS][_MaxInterpVarIDs];

// current working directory
extern std::string pg_cwd;

// scenario file names
extern string* pg_ScenarioFileNames;
// shader file names
extern string** pg_Shader_File_Names;
extern GLenum*** pg_Shader_Stages;
extern int** pg_Shader_nbStages;

// initial values in the scenario (before first scene)
extern ScenarioValue pg_InitialValuesInterpVar[PG_MAX_SCENARIOS][_MaxInterpVarIDs];
// last value shipped to the GUI (PD)
extern ScenarioValue pg_LastGUIShippedValuesInterpVar[PG_MAX_SCENARIOS][_MaxInterpVarIDs];

//////////////////////////////////////////////////////////////////////////////////////////////////////
// EXPORTED FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////

// string to integer/float conversion with exceptions in case of failure
int pg_stoi(string str);
double pg_stod(string str);
float pg_stof(string str);

// loads scenario file
void pg_LoadScenarioFile(const char* scenarioFileName, int fileRank);

// stores a string stream in which commas are replaced by space chars
void pg_stringstreamStoreLine(std::stringstream* sstream, std::string* line);
// checks whether a path begins at the root (X: on Windows, / on linux)
bool pg_isFullPath(string dir_or_filename);

// defines glut window size
void pg_setWindowDimensions(void);


#endif
