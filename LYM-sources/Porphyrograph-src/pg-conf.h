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

extern int                      pg_NbScenes;
extern bool					    pg_last_scene_update;
extern Scene*                   pg_Scenario;

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

// SVG capture
extern string                   Svg_file_name;
extern int                      beginSvg;
extern int                      endSvg;
extern int                      stepSvgInFrames;
extern double                   stepSvgInSeconds;
extern double                   nextSvgCapture;
extern bool                     outputSvg;
extern int						indSvgSnapshot;

// SVG PATHs from scenario
extern int                      nb_svg_paths;
class SVG_path {
public:
	int indPath;
	int indTrack;
	float pathRadius;
	float path_r_color;
	float path_g_color;
	float path_b_color;
	float path_readSpeedScale;
	string path_ID;
	string path_fileName;
	int path_group;
	bool with_color_radius_from_scenario;
	double secondsforwidth;
	SVG_path(void) {
		indPath = 1;
		indTrack = 1;
		pathRadius = 1;
		path_r_color = 1;
		path_g_color = 1;
		path_b_color = 1;
		path_readSpeedScale = 1;
		path_ID = "";
		path_fileName = "";
		path_group = 0;
		with_color_radius_from_scenario = false;
		secondsforwidth = 10.;
	}
	void SVG_path_init(int p_indPath, int p_indTrack, float p_pathRadius, float p_path_r_color, float p_path_g_color,
		float p_path_b_color, float p_path_readSpeedScale, string p_path_ID, string p_path_fileName, int p_path_group,
		bool p_with_color__brush_radius_from_scenario, double p_secondsforwidth) {
		indPath = p_indPath;
		indTrack = p_indTrack;
		pathRadius = p_pathRadius;
		path_r_color = p_path_r_color;
		path_g_color = p_path_g_color;
		path_b_color = p_path_b_color;
		path_readSpeedScale = p_path_readSpeedScale;
		path_ID = p_path_ID;
		path_fileName = p_path_fileName;
		// no path group provided in the scenario file
		if (p_path_group <= 0) {
			path_group = 0;
		}
		// path group number is provided inside the scenario
		else {
			path_group = p_path_group - 1;
		}
		with_color_radius_from_scenario = p_with_color__brush_radius_from_scenario;
		secondsforwidth = p_secondsforwidth;
	}
	~SVG_path() {
		path_ID = "";
	}
};
extern SVG_path *SVGpaths;
extern int current_path_group;

// VIDEO capture
extern string                   Video_file_name;
extern int                      beginVideo_file;
extern int                      endVideo_file;
extern bool                     outputVideo_file;

// UDP serversisClearAllLayersnd clients
extern pg_IPServer            **IP_Servers;
extern int                      nb_IP_Servers;
extern pg_IPClient            **IP_Clients;
extern int                      nb_IP_Clients;

extern string					pg_csv_file_name;
extern string					snapshots_dir_path_name;
extern string					screen_font_file_name;
extern int                      screen_font_size;
#if defined (TVW)
extern string					display_font_file_name;
extern int                      display_font_size;
#endif

// shader file names
extern string * pg_Shader_Files;
extern GLenum ** pg_Shader_Types;
extern int * pg_Shader_nbTypes;
extern int nb_shader_files;

// SVG GPU
// number of files
extern int pg_nb_ClipArt;
// number of paths for each file
extern int *pg_nb_paths_in_ClipArt;
// index of the first path of the current file
extern int *pg_ind_first_SvgGpu_path_in_ClipArt;
// total number of paths
extern int pg_nb_tot_SvgGpu_paths;
// file names
extern string *pg_ClipArt_fileNames;
// geometrical transformations
extern float *pg_ClipArt_Scale;
extern float *pg_ClipArt_Rotation;
extern float *pg_ClipArt_Translation_X;
extern float *pg_ClipArt_Translation_Y;
// lastisClearAllLayersctivated SvgGpu
extern int pg_last_activated_ClipArt;
// lastisClearAllLayersctivated Mesh
extern int pg_last_activated_Mesh;
// color
enum pg_ClipArt_Colors_Types { ClipArt_nat = 0, ClipArt_white, ClipArt_red, ClipArt_green, ClipArt_blue, ClipArt_yellow, ClipArt_cyan, ClipArt_magenta, ClipArt_black};
extern pg_ClipArt_Colors_Types *pg_ClipArt_Colors;
// subpath display
extern bool *pg_ClipArt_SubPath;

#if defined(var_activeMeshes)
// MESHES
// number of meshe files
extern int pg_nb_Mesh_files;
extern int pg_nb_Mesh_objects;
// file names
extern string *pg_Mesh_fileNames;
// geometrical transformations
extern float *pg_Mesh_Scale;
extern float *pg_Mesh_Rotation_angle;
extern float* pg_Mesh_Rotation_X;
extern float* pg_Mesh_Rotation_Y;
extern float* pg_Mesh_Rotation_Z;
extern float* pg_Mesh_Translation_X;
extern float* pg_Mesh_Translation_Y;
extern float* pg_Mesh_Translation_Z;
extern float* pg_Mesh_Rotation_Ini_X;
extern float* pg_Mesh_Rotation_Ini_Y;
extern float* pg_Mesh_Rotation_Ini_Z;
extern float* pg_Mesh_Translation_Ini_X;
extern float* pg_Mesh_Translation_Ini_Y;
extern float* pg_Mesh_Translation_Ini_Z;
extern float *pg_Mesh_Motion_X;
extern float *pg_Mesh_Motion_Y;
extern float *pg_Mesh_Motion_Z;
extern int *pg_Mesh_TextureRank;
#if defined(var_MmeShanghai_brokenGlass)
extern bool** pg_MmeShanghaiActveMeshObjects;
extern double** pg_MmeShanghaiMeshObjectWakeupTime;
extern bool*** pg_MmeShanghai_MeshSubParts;
extern string** pg_MmeShanghai_MeshSubPart_FileNames;
extern int* pg_MmeShanghai_NbMeshSubParts;
extern float** pg_MmeShanghai_Object_Rotation_angle;
extern float** pg_MmeShanghai_Object_Rotation_X;
extern float** pg_MmeShanghai_Object_Rotation_Y;
extern float** pg_MmeShanghai_Object_Rotation_Z;
extern float** pg_MmeShanghai_Object_Translation_X;
extern float** pg_MmeShanghai_Object_Translation_Y;
extern float** pg_MmeShanghai_Object_Translation_Z;
extern float** pg_MmeShanghai_Object_Rotation_Ini_angle;
#endif
#if defined(var_Caverne_Mesh_Profusion)
extern bool* pg_CaverneActveMesh;
extern float* pg_CaverneMeshWakeupTime;
extern float* pg_CaverneMeshBirthTime;
extern float* pg_CaverneMeshDeathTime;
extern bool Caverne_BackColor;
extern float Caverne_BackColorRed;
extern float Caverne_BackColorGreen;
extern float Caverne_BackColorBlue;
extern float Caverne_BackColorRed_prec;
extern float Caverne_BackColorGreen_prec;
extern float Caverne_BackColorBlue_prec;
extern bool Caverne_BackColorFlash;
extern bool Caverne_BackColorFlash_prec;
#endif
// color
extern float **pg_Mesh_Colors;
// textures
extern GLuint *Mesh_texture_rectangle;
#endif

float my_stof(string str);
int my_stoi(string str);
double my_stod(string str);

// TEXTURES
enum pg_Texture_Usages { Texture_master_mask = 0, Texture_mesh, Texture_sensor, Texture_logo, 
	Texture_brush, Texture_noise, Texture_curve_particle, Texture_splat_particle, Texture_part_init,
	Texture_part_acc, Texture_pixel_acc, Texture_repop_density, Texture_multilayer_master_mask};
// number of Texture files
extern int pg_nb_Texture_files;
// file names
extern string *pg_Texture_fileNames;
extern string *pg_Texture_fileNamesSuffix;
// usages
extern pg_Texture_Usages *pg_Texture_usages;
// rank (if used several times for the same usage)
extern int *pg_Texture_Rank;
// 2D or 3D
extern int *pg_Texture_Dimension;
// number of piled 2D textures forisClearAllLayers 3D texture
extern int *pg_Texture_Nb_Layers;
// dimensions
extern int *pg_Texture_Size_X;
extern int *pg_Texture_Size_Y;
// color depth
extern int *pg_Texture_Nb_Bytes_per_Pixel;
// rectangle or PoT
extern bool *pg_Texture_Is_Rectangle;
// texture inversion
extern bool *pg_Texture_Invert;
// texture ID
extern GLuint *pg_Texture_texID;

/// the params of the font file
extern string                    font_file_encoding;
extern TextureEncoding           font_texture_encoding;

// window(s) size and location
extern int my_window_x;
extern int my_window_y;

void light_channel_string_to_channel_no(string a_light_channel_string, int* light_channel, int* light_channel_fine);
void saveInitialTimesAndDurations(void);
void restoreInitialTimesAndDurations( void );
void pg_update_scene_variables(Scene* cur_scene, float elapsed_time_from_start);

void setWindowDimensions(void);

void parseConfigurationFile(std::ifstream& confFin, std::ifstream&  scenarioFin);
void LoadConfigurationFile( const char * confFileName , const char * scenarioFileName );
void stringstreamStoreLine(std::stringstream* sstream, std::string* line);

int FindSceneById(std::string * sceneID);

#endif
