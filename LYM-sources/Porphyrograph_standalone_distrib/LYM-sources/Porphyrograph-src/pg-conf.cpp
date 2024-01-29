/*! \file pg-conf.cpp
 *
 * 
 * 
 *     File pg-conf.cpp
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

/*! \name Group constants
 *  \brief Enumerated constants
 */
// \{

const char *pg_UDPMessageFormatString[Emptypg_UDPMessageFormat + 1] = { "Plain" , "OSC" , "Emptypg_UDPMessageFormat" };

/////////////////////////////////////////////////////
// global variable for screenshot file name
/////////////////////////////////////////////////////

int                      pg_NbScenes[_NbConfigurations];
bool					 pg_last_scene_update = false;


Scene                    *pg_Scenario[_NbConfigurations] = {NULL};
double                   current_scene_percent = 0.f;

string					 pg_csv_file_name;
string					 snapshots_dir_path_prefix;
string					 snapshots_dir_path_name;
string					 screen_font_file_name;
int                      screen_font_size;
#if defined(TVW)
string					 display_font_file_name;
int                      display_font_size;
#endif
string                   font_file_encoding;
TextureEncoding          font_texture_encoding;

pg_Window              *PG_Window;

// PNG capture
string                   Png_file_name;
int                      beginPng;
int                      endPng;
int                      stepPngInFrames;
double                   stepPngInSeconds;
double                   nextPngCapture;
bool                     outputPng;
int						 indPngSnapshot;

// JPG capture
string                   Jpg_file_name;
int                      beginJpg;
int                      endJpg;
int                      stepJpgInFrames;
double                   stepJpgInSeconds;
double                   nextJpgCapture;
bool                     outputJpg;
int						 indJpgSnapshot;

// SVG path capture
string                   Svg_file_name;
int                      beginSvg;
int                      endSvg;
int                      stepSvgInFrames;
double                   stepSvgInSeconds;
double                   nextSvgCapture;
bool                     outputSvg;
int						 indSvgSnapshot;

// SVG paths from scenario
int                      pg_nb_SVG_paths[_NbConfigurations] = { 0 };
SVG_path*				 pg_SVG_paths[_NbConfigurations] = { NULL };
int						 pg_current_SVG_path_group = 0;
int						 pg_nb_SVG_path_groups[_NbConfigurations] = { 0 };

// VIDEO capture
string                   Video_file_name;
int                      beginVideo_file;
int                      endVideo_file;
bool                     outputVideo_file;

// UDP servers and clients
pg_IPServer            **IP_Servers;
int                      nb_IP_Servers;
pg_IPClient            **IP_Clients;
int                      nb_IP_Clients;

// nb configuration and scenario files
string *pg_ConfigurationFileNames = NULL;
string *pg_ScenarioFileNames = NULL;

// shader file names
string  ** pg_Shader_File_Names;
GLenum *** pg_Shader_Stages;
int     ** pg_Shader_nbStages;

// ClipArt GPU
// number of files
int pg_nb_ClipArt[_NbConfigurations] = {0};
// number of paths for each file
int *pg_nb_paths_in_ClipArt[_NbConfigurations] = {NULL};
// total number of paths
int pg_nb_tot_SvgGpu_paths[_NbConfigurations] = {0};
// file names
string *pg_ClipArt_fileNames[_NbConfigurations] = {NULL};
// geometrical transformations
float *pg_ClipArt_Scale[_NbConfigurations] = {NULL};
float *pg_ClipArt_Rotation[_NbConfigurations] = {NULL};
float *pg_ClipArt_Translation_X[_NbConfigurations] = {NULL};
float *pg_ClipArt_Translation_Y[_NbConfigurations] = {NULL};
// last activated SvgGpu
int pg_last_activated_ClipArt = 0;
// last activated Mesh
int pg_last_activated_Mesh = 0;
// color
pg_ClipArt_Colors_Types** pg_ClipArt_Colors[_NbConfigurations] = {NULL};
// subpath display
bool** pg_ClipArt_SubPath[_NbConfigurations] = {NULL};

#if defined(var_activeMeshes)
// MESHES
// number of mesh files
int pg_nb_Mesh_files[_NbConfigurations] = { 0 };
int pg_nb_Mesh_objects[_NbConfigurations] = { 0 };
// file names
string *pg_Mesh_fileNames[_NbConfigurations] = {NULL};
// geometrical transformations
float *pg_Mesh_Scale[_NbConfigurations] = {NULL};
float *pg_Mesh_Rotation_angle[_NbConfigurations] = {NULL};
float* pg_Mesh_Rotation_X[_NbConfigurations] = {NULL};
float* pg_Mesh_Rotation_Y[_NbConfigurations] = {NULL};
float* pg_Mesh_Rotation_Z[_NbConfigurations] = {NULL};
float* pg_Mesh_Translation_X[_NbConfigurations] = {NULL};
float* pg_Mesh_Translation_Y[_NbConfigurations] = {NULL};
float* pg_Mesh_Translation_Z[_NbConfigurations] = {NULL};
float* pg_Mesh_Rotation_Ini_X[_NbConfigurations] = {NULL};
float* pg_Mesh_Rotation_Ini_Y[_NbConfigurations] = {NULL};
float* pg_Mesh_Rotation_Ini_Z[_NbConfigurations] = {NULL};
float* pg_Mesh_Translation_Ini_X[_NbConfigurations] = {NULL};
float* pg_Mesh_Translation_Ini_Y[_NbConfigurations] = {NULL};
float* pg_Mesh_Translation_Ini_Z[_NbConfigurations] = {NULL};
float *pg_Mesh_Motion_X[_NbConfigurations] = {NULL};
float *pg_Mesh_Motion_Y[_NbConfigurations] = {NULL};
float *pg_Mesh_Motion_Z[_NbConfigurations] = {NULL};
int *pg_Mesh_TextureRank[_NbConfigurations] = {NULL};
#if defined(var_MmeShanghai_brokenGlass)
bool** pg_MmeShanghaiActveMeshObjects[_NbConfigurations] = {NULL};
double** pg_MmeShanghaiMeshObjectWakeupTime[_NbConfigurations] = {NULL};
bool*** pg_MmeShanghai_MeshSubParts[_NbConfigurations] = {NULL};
string** pg_MmeShanghai_MeshSubPart_FileNames[_NbConfigurations] = {NULL};
int* pg_MmeShanghai_NbMeshSubParts[_NbConfigurations] = {NULL};
float** pg_MmeShanghai_Object_Rotation_angle[_NbConfigurations] = {NULL};
float** pg_MmeShanghai_Object_Rotation_X[_NbConfigurations] = {NULL};
float** pg_MmeShanghai_Object_Rotation_Y[_NbConfigurations] = {NULL};
float** pg_MmeShanghai_Object_Rotation_Z[_NbConfigurations] = {NULL};
float** pg_MmeShanghai_Object_Translation_X[_NbConfigurations] = {NULL};
float** pg_MmeShanghai_Object_Translation_Y[_NbConfigurations] = {NULL};
float** pg_MmeShanghai_Object_Translation_Z[_NbConfigurations] = {NULL};
float** pg_MmeShanghai_Object_Rotation_Ini_angle[_NbConfigurations] = {NULL};
#endif
#if defined(var_Caverne_Mesh_Profusion) && defined(var_Caverne_BackColor)
bool* pg_CaverneActveMesh[_NbConfigurations] = {NULL};
float* pg_CaverneMeshWakeupTime[_NbConfigurations] = {NULL};
float* pg_CaverneMeshBirthTime[_NbConfigurations] = {NULL};
float* pg_CaverneMeshDeathTime[_NbConfigurations] = {NULL};
float Caverne_BackColorRed[_NbConfigurations] = {0.f};
float Caverne_BackColorGreen[_NbConfigurations] = {0.f};
float Caverne_BackColorBlue[_NbConfigurations] = {0.f};
float Caverne_BackColorRed_prec[_NbConfigurations] = {0.f};
float Caverne_BackColorGreen_prec[_NbConfigurations] = {0.f};
float Caverne_BackColorBlue_prec[_NbConfigurations] = {0.f};
bool Caverne_BackColorFlash[_NbConfigurations] = {false};
bool Caverne_BackColorFlash_prec[_NbConfigurations] = {false};
#endif
// color
float **pg_Mesh_Colors[_NbConfigurations] = {NULL};
// textures
GLuint* Mesh_texture_rectangle[_NbConfigurations] = { NULL_ID };
#endif

// TEXTURES
// number of Texture files
int pg_nb_Texture_files[_NbConfigurations] = { 0 };
// file names
string *pg_Texture_fileNames[_NbConfigurations] = {NULL};
string *pg_Texture_fileNamesSuffix[_NbConfigurations] = {NULL};
// usages
pg_Texture_Usages *pg_Texture_usages[_NbConfigurations] = {NULL};
// rank (if used several times for the same usage)
int *pg_Texture_Rank[_NbConfigurations] = {NULL};
// 2D or 3D
int *pg_Texture_Dimension[_NbConfigurations] = {NULL};
// number of piled 2D textures for a 3D texture
int *pg_Texture_Nb_Layers[_NbConfigurations] = {NULL};
// dimensions
int *pg_Texture_Size_X[_NbConfigurations] = {NULL};
int *pg_Texture_Size_Y[_NbConfigurations] = {NULL};
// color depth
int *pg_Texture_Nb_Bytes_per_Pixel[_NbConfigurations] = {NULL};
// rectangle or PoT
bool *pg_Texture_Is_Rectangle[_NbConfigurations] = {NULL};
// texture inversion
bool *pg_Texture_Invert[_NbConfigurations] = {NULL};
// texture ID
GLuint *pg_Texture_texID[_NbConfigurations] = {NULL};

// window(s) size and location
int my_window_x = 0;
int my_window_y = 0;


/////////////////////////////////////////////////////
// Default values for global variables
/////////////////////////////////////////////////////
pg_Window::pg_Window( void ) {
  glutID = -1;
}

pg_Window::~pg_Window(void) {
}

/////////////////////////////////////////////////////
// environment parsing from configuration file
/////////////////////////////////////////////////////

float my_stof(string str) {
	float f = 0.f;
	try {
		f = std::stof(str);
	}
	catch (const std::invalid_argument&) {
		sprintf(ErrorStr, "Error: float argument is invalid (%s)\n", str.c_str()); ReportError(ErrorStr); throw 50;
	}
	catch (const std::out_of_range&) {
		sprintf(ErrorStr, "Error: float argument is out of range (%s)\n", str.c_str()); ReportError(ErrorStr); throw 50;
	}
	catch (const std::exception& e) {
		sprintf(ErrorStr, "Error: float argument is incorrect (%s): %s\n", str.c_str(), e.what()); ReportError(ErrorStr); throw 50;
	}
	return f;
}

int my_stoi(string str) {
	int i = 0;
	try {
		i = std::stoi(str);
	}
	catch (const std::invalid_argument&) {
		sprintf(ErrorStr, "Error: int argument is invalid (%s)\n", str.c_str()); ReportError(ErrorStr); throw 50;
	}
	catch (const std::out_of_range&) {
		sprintf(ErrorStr, "Error: int argument is out of range (%s)\n", str.c_str()); ReportError(ErrorStr); throw 50;
	}
	catch (const std::exception& e) {
		sprintf(ErrorStr, "Error: int argument is incorrect (%s): %s\n", str.c_str(), e.what()); ReportError(ErrorStr); throw 50;
	}
	return i;
}

double my_stod(string str) {
	double d = 0.;
	try {
		d = std::stod(str);
	}
	catch (const std::invalid_argument&) {
		sprintf(ErrorStr, "Error: double argument is invalid (%s)\n", str.c_str()); ReportError(ErrorStr); throw 50;
	}
	catch (const std::out_of_range&) {
		sprintf(ErrorStr, "Error: double argument is out of range (%s)\n", str.c_str()); ReportError(ErrorStr); throw 50;
	}
	catch (const std::exception& e) {
		sprintf(ErrorStr, "Error: double argument is incorrect (%s): %s\n", str.c_str(), e.what()); ReportError(ErrorStr); throw 50;
	}
	return d;
}

void stringstreamStoreLine(std::stringstream *sstream, std::string *line) {
	(*sstream).clear();
	if ((*line).find('\t') == std::string::npos) {
		std::replace((*line).begin(), (*line).end(), ',', ' ');
	}
	//printf("%s\n", (* line).c_str());
	(*sstream).str((*line));
}

double stringToDuration(string percentOrAbsoluteDuration, double full_length, int ind_scene, int ind_var) {
	double returnedPercent = 0.f;
	if (percentOrAbsoluteDuration.back() == 's') {
		percentOrAbsoluteDuration.resize(size_t(percentOrAbsoluteDuration.size() - 1));
		returnedPercent = my_stof(percentOrAbsoluteDuration);
		if (returnedPercent < 0 || returnedPercent > full_length) {
			sprintf(ErrorStr, "Error: in scene %d var %d absolute duration %s negative or greater than full scene duration %.2f\n", ind_scene, ind_var, (percentOrAbsoluteDuration + "s").c_str(), full_length); ReportError(ErrorStr); throw 50;
		}
		if (full_length != 0.f) {
			returnedPercent /= full_length;
		}
	}
	else {
		returnedPercent = my_stof(percentOrAbsoluteDuration);
		if (returnedPercent < 0 || returnedPercent > 1.f) {
			sprintf(ErrorStr, "Error: in scene %d var %d relative duration %s negative or greater than 1.\n", ind_scene, ind_var, percentOrAbsoluteDuration.c_str()); ReportError(ErrorStr); throw 50;
		}
	}
	return min(max(float(returnedPercent), 0.f), 1.f);
}

void light_channel_string_to_channel_no(string a_light_channel_string, int* light_channel, int* light_channel_fine, int nb_channels) {
	if (a_light_channel_string.find('/') != std::string::npos) {
		vector<string> two_light_channels_string = split_string(a_light_channel_string, '/');
		// double light channel
		if (two_light_channels_string.size() == 2 && std::all_of(two_light_channels_string[0].begin(), two_light_channels_string[0].end(), ::isdigit)
			&& std::all_of(two_light_channels_string[1].begin(), two_light_channels_string[1].end(), ::isdigit)) {
			*light_channel = stoi(two_light_channels_string[0]);
			if (*light_channel > nb_channels || *light_channel <= 0) {
				sprintf(ErrorStr, "Unregistered light group %s/%d in light command (max: %d)!", two_light_channels_string[0].c_str(), *light_channel, nb_channels); ReportError(ErrorStr);
			}
			*light_channel_fine = stoi(two_light_channels_string[1]);
			if (*light_channel_fine > nb_channels || *light_channel_fine <= 0) {
				sprintf(ErrorStr, "Unregistered light group %s/%d in light command (max: %d)!", two_light_channels_string[1].c_str(), *light_channel_fine, nb_channels); ReportError(ErrorStr);
			}
		}
	}
	else {
		// single light channel
		if (std::all_of(a_light_channel_string.begin(), a_light_channel_string.end(), ::isdigit)) {
			*light_channel = stoi(a_light_channel_string);
			// 0: no light channel for this command
			// 1-nb_channels: one of the available channels
			if (*light_channel > nb_channels || *light_channel < 0) {
				sprintf(ErrorStr, "Out of range light channel %s/%d in light command (max: %d)!", a_light_channel_string.c_str(), *light_channel, nb_channels); ReportError(ErrorStr);
			}
			*light_channel_fine = 0;
		}
	}
}


/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
// CONFIGURATION FILE
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

void parseConfigurationFile(std::ifstream& confFin, int indConfiguration) {

	string line;
	string ID;
	string temp;
	string temp2;
	string temp3;
	std::stringstream  sstream;

	////////////////////////////////////
	////// CONFIGURATION VARIABLE INITIALIZATION
	////////////////////////////////////

	printf("Loading header %s with %d variables\n", pg_ConfigurationFileNames[indConfiguration].c_str(), _MaxConfigurationVarIDs);

	// RANK
	std::getline(confFin, line);
	// VERBATIM
	std::getline(confFin, line);
	// TYPE
	std::getline(confFin, line);
	// ID
	std::getline(confFin, line);
	// initial_values
	std::getline(confFin, line);
	stringstreamStoreLine(&sstream, &line);
	sstream >> ID;
	if (ID.compare("initial_values") != 0) {
		sprintf(ErrorStr, "Error: incorrect configuration file expected string \"initial_values\" not found! (instead \"%s\")", ID.c_str()); ReportError(ErrorStr); throw 100;
	}

	// reads configuration variables initial values (not used because precalculated in the included script_header file)
	std::getline(confFin, line);
	if (indConfiguration == 0) {
		stringstreamStoreLine(&sstream, &line);
		// some variables should not be precompiled so that there is not a need to recompile to have them changed when loading the header
		for (int indP = 0; indP < _MaxConfigurationVarIDs; indP++) {
			if (indP == _window_x) {
				sstream >> my_window_x;
			}
			else if (indP == _window_y) {
				sstream >> my_window_y;
			}
			else {
				sstream >> temp;
			}
		}
	}

	// /initial_values
	std::getline(confFin, line);

	////////////////////////////////////
	////// RENDERING FILES & DIRECTORIES (VIDEP, PNG, JPG, ClipArt...)
	////////////////////////////////////

	// rendering_files
	std::getline(confFin, line);
	stringstreamStoreLine(&sstream, &line);
	sstream >> ID;
	if (ID.compare("rendering_files") != 0) {
		sprintf(ErrorStr, "Error: incorrect configuration file expected string \"rendering_files\" not found! (instead \"%s\")", ID.c_str()); ReportError(ErrorStr); throw 100;
	}

	// TYPE
	std::getline(confFin, line);
	// ID	begin	end	step	id	shots
	std::getline(confFin, line);

	// storing the Video capture values
	std::getline(confFin, line);
	stringstreamStoreLine(&sstream, &line);
	sstream >> ID;
	if (ID.compare("VIDEO") != 0) {
		sprintf(ErrorStr, "Error: incorrect configuration file expected string \"VIDEO\" not found! (instead \"%s\")", ID.c_str()); ReportError(ErrorStr); throw 100;
	}
	if (indConfiguration ==  0) {
		sstream >> beginVideo_file;
		sstream >> endVideo_file;
		sstream >> temp; // unused
		sstream >> Video_file_name;
		outputVideo_file = !Video_file_name.empty();
	}

	// storing the Svg capture values
	std::getline(confFin, line);
	stringstreamStoreLine(&sstream, &line);
	sstream >> ID;
	if (ID.compare("SVG") != 0) {
		sprintf(ErrorStr, "Error: incorrect configuration file expected string \"SVG\" not found! (instead \"%s\")", ID.c_str()); ReportError(ErrorStr); throw 100;
	}
	if (indConfiguration ==  0) {
		sstream >> beginSvg;
		sstream >> endSvg;
		sstream >> temp;
		if (temp.back() == 's') {
			temp.resize(size_t(temp.size() - 1));
			stepSvgInSeconds = my_stof(temp);
			nextSvgCapture = -1.;
			stepSvgInFrames = -1;
		}
		else {
			stepSvgInFrames = my_stoi(temp);
			stepSvgInSeconds = -1.;
		}
		sstream >> Svg_file_name;
		outputSvg = !Svg_file_name.empty();
		indSvgSnapshot = 0;
	}

	// storing the Png capture values
	std::getline(confFin, line);
	stringstreamStoreLine(&sstream, &line);
	sstream >> ID;
	if (ID.compare("PNG") != 0) {
		sprintf(ErrorStr, "Error: incorrect configuration file expected string \"PNG\" not found! (instead \"%s\")", ID.c_str()); ReportError(ErrorStr); throw 100;
	}
	if (indConfiguration ==  0) {
		sstream >> beginPng;
		sstream >> endPng;
		sstream >> temp;
		if (temp.back() == 's') {
			temp.resize(size_t(temp.size() - 1));
			stepPngInSeconds = my_stof(temp);
			nextPngCapture = -1.;
			stepPngInFrames = -1;
		}
		else {
			stepPngInFrames = my_stoi(temp);
			stepPngInSeconds = -1.;
		}
		sstream >> Png_file_name;
		outputPng = !Png_file_name.empty();
		indPngSnapshot = 0;
	}

	// storing the Jpg capture values
	std::getline(confFin, line);
	stringstreamStoreLine(&sstream, &line);
	sstream >> ID;
	if (ID.compare("JPG") != 0) {
		sprintf(ErrorStr, "Error: incorrect configuration file expected string \"JPG\" not found! (instead \"%s\")", ID.c_str()); ReportError(ErrorStr); throw 100;
	}
	if (indConfiguration ==  0) {
		sstream >> beginJpg;
		sstream >> endJpg;
		sstream >> temp;
		if (temp.back() == 's') {
			temp.resize(size_t(temp.size() - 1));
			stepJpgInSeconds = my_stof(temp);
			nextJpgCapture = -1.;
			stepJpgInFrames = -1;
		}
		else {
			stepJpgInFrames = my_stoi(temp);
			stepJpgInSeconds = -1.;
		}

		sstream >> Jpg_file_name;
		outputJpg = !Jpg_file_name.empty();
		indJpgSnapshot = 0;
	}

	// /rendering_files
	std::getline(confFin, line);

	////////////////////////////////////
	////// IP CLIENTS/SERVERS
	////////////////////////////////////

	int nb_unloaded_IP_Servers = 0;
	int nb_unloaded_IP_Clients = 0;
	if (indConfiguration ==  0) {
		IP_Servers = NULL;
		nb_IP_Servers = 0;

		IP_Clients = NULL;
		nb_IP_Clients = 0;
	}
	else {
		nb_unloaded_IP_Servers = 0;
		nb_unloaded_IP_Clients = 0;
	}

	// udp_local_server Number of servers
	std::getline(confFin, line);
	stringstreamStoreLine(&sstream, &line);
	sstream >> ID;
	if (ID.compare("udp_local_server") != 0) {
		sprintf(ErrorStr, "Error: incorrect configuration file expected string \"udp_local_server\" not found! (instead \"%s\")", ID.c_str()); ReportError(ErrorStr); throw 100;
	}
	if (indConfiguration ==  0) {
		sstream >> nb_IP_Servers;
	}
	else {
		sstream >> nb_unloaded_IP_Servers;
	}
	// std::cout << "Nb servers: " << nb_IP_Servers << "\n";

	// VERBATIM
	std::getline(confFin, line);
	// TYPE
	std::getline(confFin, line);
	// ID
	std::getline(confFin, line);

	if (indConfiguration == 0) {
		IP_Servers = new pg_IPServer * [nb_IP_Servers];
		for (int ind_IP_Server = 0; ind_IP_Server < nb_IP_Servers; ind_IP_Server++) {
			IP_Servers[ind_IP_Server] = new pg_IPServer();

			std::getline(confFin, line);
			stringstreamStoreLine(&sstream, &line);
			sstream >> ID; // string "server"
			sstream >> IP_Servers[ind_IP_Server]->id;
			sstream >> IP_Servers[ind_IP_Server]->Local_server_port;
			sstream >> ID;
			for (int ind = 0; ind < Emptypg_UDPMessageFormat; ind++) {
				if (strcmp(ID.c_str(), pg_UDPMessageFormatString[ind]) == 0) {
					IP_Servers[ind_IP_Server]->receive_format = (pg_UDPMessageFormat)ind;
					break;
				}
			}
			if (IP_Servers[ind_IP_Server]->receive_format == Emptypg_UDPMessageFormat) {
				sprintf(ErrorStr, "Error: unknown receive message format [%s]!", ID.c_str()); ReportError(ErrorStr); throw 249;
			}
			sstream >> IP_Servers[ind_IP_Server]->IP_message_trace;
			sstream >> IP_Servers[ind_IP_Server]->depth_input_stack;
			sstream >> IP_Servers[ind_IP_Server]->OSC_duplicate_removal;
			// printf("serveur %d duplicate removal %d\n", ind_IP_Server, IP_Servers[ind_IP_Server]->OSC_duplicate_removal);
			sstream >> IP_Servers[ind_IP_Server]->OSC_endian_reversal;
		}
	}
	else {
		for (int ind_IP_Server = 0; ind_IP_Server < nb_unloaded_IP_Servers; ind_IP_Server++) {
			std::getline(confFin, line);
		}
	}

	// /udp_local_server
	std::getline(confFin, line);

	// udp_remote_client Number of clients
	std::getline(confFin, line);
	stringstreamStoreLine(&sstream, &line);
	sstream >> ID;
	if (ID.compare("udp_remote_client") != 0) {
		sprintf(ErrorStr, "Error: incorrect configuration file expected string \"udp_remote_client\" not found! (instead \"%s\")", ID.c_str()); ReportError(ErrorStr); throw 100;
	}
	if (indConfiguration == 0) {
		sstream >> nb_IP_Clients;
	}
	else {
		sstream >> nb_unloaded_IP_Clients;
	}
	// std::cout << "Nb clients: " << nb_IP_Clients << "\n";

	// VERBATIM
	std::getline(confFin, line);
	// TYPE
	std::getline(confFin, line);
	// ID
	std::getline(confFin, line);

	if (indConfiguration == 0) {
		IP_Clients = new pg_IPClient * [nb_IP_Clients];
		for (int ind_IP_Client = 0; ind_IP_Client < nb_IP_Clients; ind_IP_Client++) {
			IP_Clients[ind_IP_Client] = new pg_IPClient();

			std::getline(confFin, line);
			stringstreamStoreLine(&sstream, &line);
			sstream >> ID; // string "Client"
			sstream >> IP_Clients[ind_IP_Client]->id;
			sstream >> IP_Clients[ind_IP_Client]->Remote_server_IP;
			sstream >> IP_Clients[ind_IP_Client]->Remote_server_port;
			sstream >> ID;
			for (int ind = 0; ind < Emptypg_UDPMessageFormat; ind++) {
				if (strcmp(ID.c_str(), pg_UDPMessageFormatString[ind]) == 0) {
					IP_Clients[ind_IP_Client]->send_format = (pg_UDPMessageFormat)ind;
					break;
				}
			}
			if (IP_Clients[ind_IP_Client]->send_format == Emptypg_UDPMessageFormat) {
				sprintf(ErrorStr, "Error: unknown receive message format [%s]!", ID.c_str()); ReportError(ErrorStr); throw 249;
			}
			sstream >> IP_Clients[ind_IP_Client]->IP_message_trace;
			sstream >> IP_Clients[ind_IP_Client]->max_depth_output_stack;
			sstream >> IP_Clients[ind_IP_Client]->OSC_endian_reversal;
			// std::cout << "OSC_trace: " << IP_Clients[ind_IP_Client]->IP_message_trace << "\n";
			//std::cout << "IP_Clients[ ind_IP_Client ]->id: " << IP_Clients[ind_IP_Client]->id << "\n";
			//std::cout << "IP_Clients[ ind_IP_Client ]->Remote_server_IP: " << ind_IP_Client << " " << IP_Clients[ind_IP_Client]->Remote_server_IP << "\n";
			//std::cout << "IP_Clients[ ind_IP_Client ]->Remote_server_port: " << IP_Clients[ind_IP_Client]->Remote_server_port << "\n";
		}
	}
	else {
		for (int ind_IP_Client = 0; ind_IP_Client < nb_unloaded_IP_Clients; ind_IP_Client++) {
			std::getline(confFin, line);
		}
	}

	// /udp_remote_client
	std::getline(confFin, line);

	////////////////////////////////////
	////// CAMERAS
	////////////////////////////////////

	// webCam Number of cameras
	int nb_unloaded_webcams = 0;
	int nb_unloaded_IPcams = 0;
	std::getline(confFin, line);
	stringstreamStoreLine(&sstream, &line);
	sstream >> ID;
	if (ID.compare("webCam") != 0) {
		sprintf(ErrorStr, "Error: incorrect configuration file expected string \"webCam\" not found! (instead \"%s\")", ID.c_str()); ReportError(ErrorStr); throw 100;
	}
#if defined(var_cameraCaptFreq)
	if (indConfiguration == 0) {
		sstream >> nb_webCam;
		std::cout << "Nb webCams: " << nb_webCam << "\n";
	}
	else
#endif
	{
		sstream >> nb_unloaded_webcams;
	}


	// VERBATIM
	std::getline(confFin, line);
	// TYPE
	std::getline(confFin, line);
	// ID
	std::getline(confFin, line);

#if defined(var_cameraCaptFreq)
	if (indConfiguration == 0) {
		pg_webCams = new webCam[nb_webCam];
		for (int ind_webCam = 0; ind_webCam < nb_webCam; ind_webCam++) {
			std::getline(confFin, line);
			stringstreamStoreLine(&sstream, &line);
			sstream >> temp; // string "camera"
			sstream >> pg_webCams[ind_webCam].cameraString; // string "cameraString"

			sstream >> temp; // string "cameraID"
			try {
				pg_webCams[ind_webCam].cameraID = stoi(temp);
			}
			catch (...) {
				sprintf(ErrorStr, "Error: webcam configuration incorrect cameraID \"%s\"\n", temp.c_str()); ReportError(ErrorStr); throw 50;
			}

			sstream >> temp2; // string "cameraWidth"
			try {
				pg_webCams[ind_webCam].cameraWidth = stoi(temp2);
			}
			catch (...) {
				sprintf(ErrorStr, "Error: webcam configuration incorrect cameraWidth \"%s\"\n", temp2.c_str()); ReportError(ErrorStr); throw 50;
			}

			sstream >> temp3; // string "cameraHeight"
			try {
				pg_webCams[ind_webCam].cameraHeight = stoi(temp3);
			}
			catch (...) {
				sprintf(ErrorStr, "Error: webcam configuration incorrect cameraHeight \"%s\"\n", temp3.c_str()); ReportError(ErrorStr); throw 50;
			}		//std::cout << temp2 << "\n";
		}
	}
	else {
		for (int ind_webCam = 0; ind_webCam < nb_unloaded_webcams; ind_webCam++) {
			std::getline(confFin, line);
		}
	}
#else
	for (int ind_webCam = 0; ind_webCam < nb_unloaded_webcams; ind_webCam++) {
		std::getline(confFin, line);
	}
#endif
	// /webCam
	std::getline(confFin, line);


	// remote_IPCam Number of cameras
	std::getline(confFin, line);
	stringstreamStoreLine(&sstream, &line);
	sstream >> ID;
	if (ID.compare("remote_IPCam") != 0) {
		sprintf(ErrorStr, "Error: incorrect configuration file expected string \"remote_IPCam\" not found! (instead \"%s\")", ID.c_str()); ReportError(ErrorStr); throw 100;
	}
#if defined(var_cameraCaptFreq)
	if (indConfiguration == 0) {
		sstream >> nb_IPCam;
		// std::cout << "nb_IPCam: " << nb_IPCam << "\n";
	}
	else {
		sstream >> nb_unloaded_IPcams;
	}
#else
	sstream >> nb_unloaded_IPcams;
#endif

	// VERBATIM
	std::getline(confFin, line);
	// TYPE
	std::getline(confFin, line);
	// ID
	std::getline(confFin, line);

#if defined(var_cameraCaptFreq)
	if (indConfiguration == 0) {
		pg_IPCam_capture = new VideoCapture[nb_IPCam];
		pg_IPCam_capture_address = new String[nb_IPCam];
		for (int ind_IPCam = 0; ind_IPCam < nb_IPCam; ind_IPCam++) {
			std::getline(confFin, line);
			stringstreamStoreLine(&sstream, &line);
			sstream >> ID; // string "ID"
			sstream >> temp; // string "IPCam_IP"
			sstream >> temp2; // string "IPCam_Port"
			sstream >> temp3; // string "IPCam_Device"
			//String fulllAddress = "http://" + temp + ":" + temp2;
			// "rtsp://[username]:[pass]@[ip address]/media/video1"
			// fulllAddress = "rtsp://" + temp + "/H264?ch=1&subtype=0";
			// fulllAddress = "rtsp://" + temp + "/H264?ch=1&subtype=0";
			//fulllAddress = "rtsp://" + temp + "/H264";
			//fulllAddress = "rtsp://" + temp + "/mjpeg.cgi?user=yukao.nagemi@gmail.com&password=Fire5432_&channel=0&.mjpg";
			//fulllAddress = "rtsp://yukao.nagemi@gmail.com:Fire5432_@" + temp + ":" + temp2 + "/onvif1";
			//fulllAddress = "rtsp://" + temp + ":" + temp2 + "/onvif1";
			//fulllAddress = "rtsp://" + temp + "/onvif1";
			// rtsp://192.168.1.65:8554/main
			//fulllAddress = "rtsp://192.168.1.65:8554/main";
			pg_IPCam_capture_address[ind_IPCam] = "rtsp://" + temp + ":" + temp2 + "/" + temp3;
		}
	}
	else {
		for (int ind_IPCam = 0; ind_IPCam < nb_unloaded_IPcams; ind_IPCam++) {
			std::getline(confFin, line);
		}
	}
#else
	for (int ind_IPCam = 0; ind_IPCam < nb_unloaded_IPcams; ind_IPCam++) {
		std::getline(confFin, line);
	}
#endif

	// /remote_IPCam
	std::getline(confFin, line);


	////////////////////////////////////
	////// SHADERS
	////////////////////////////////////

	// shader file names
	pg_Shader_File_Names[indConfiguration] = NULL;

	// shader_files Number of files
	std::getline(confFin, line);
	stringstreamStoreLine(&sstream, &line);
	sstream >> ID;
	if (ID.compare("shader_files") != 0) {
		sprintf(ErrorStr, "Error: incorrect configuration file expected string \"shader_files\" not found! (instead \"%s\")", ID.c_str()); ReportError(ErrorStr); throw 100;
	}

	// VERBATIM
	std::getline(confFin, line);
	// TYPE
	std::getline(confFin, line);
	// ID
	std::getline(confFin, line);

	pg_Shader_File_Names[indConfiguration] = new string[_pg_shader_TotalNbTypes];
	pg_Shader_nbStages[indConfiguration] = new int[_pg_shader_TotalNbTypes];
	pg_Shader_Stages[indConfiguration] = new GLenum * [_pg_shader_TotalNbTypes];
	for (int ind_shader_type = 0; ind_shader_type < _pg_shader_TotalNbTypes; ind_shader_type++) {
		pg_Shader_File_Names[indConfiguration][ind_shader_type] = string("");
		pg_Shader_nbStages[indConfiguration][ind_shader_type] = 0;
	}
		//std::unordered_map<int, std::string> pg_stringShaderTypes = {
	//	{ _pg_shader_ParticleAnimation, "ParticleAnimation" },
	//	{ _pg_shader_Update, "Update" },
	//	{ _pg_shader_ParticleRender, "ParticleRender" },
	//	{ _pg_shader_Mixing, "Mixing" },
	//	{ _pg_shader_Master, "Master" },
	//	{ _pg_shader_Sensor, "Sensor" },
	//	{ _pg_shader_ClipArt, "ClipArt" },
	//	{ _pg_shader_Mesh, "Mesh" },
	//	{ _pg_shader_TotalNbTypes, "TotalNbTypes" },
	//};
	for (int ind_shader_type = 0; ind_shader_type < _pg_shader_TotalNbTypes; ind_shader_type++) {
		std::getline(confFin, line);
		stringstreamStoreLine(&sstream, &line);
		sstream >> ID; // shader type
		if (ID != pg_stringShaderTypes[ind_shader_type]) {
			sprintf(ErrorStr, "Error: incorrect shader type expected string \"%s\" not found! (instead \"%s\")", pg_stringShaderTypes[ind_shader_type].c_str(), ID.c_str()); ReportError(ErrorStr); throw 100;
		}
		sstream >> pg_Shader_File_Names[indConfiguration][ind_shader_type];
		sstream >> pg_Shader_nbStages[indConfiguration][ind_shader_type];
		if (pg_Shader_nbStages[indConfiguration][ind_shader_type] > 0 && pg_Shader_File_Names[indConfiguration][ind_shader_type] != "NULL") {
			pg_Shader_Stages[indConfiguration][ind_shader_type] = new GLenum[pg_Shader_nbStages[indConfiguration][ind_shader_type]];
			for (int ind_shader_stage = 0; ind_shader_stage < pg_Shader_nbStages[indConfiguration][ind_shader_type]; ind_shader_stage++) {
				string shader_stage;
				sstream >> shader_stage;
				if (shader_stage.compare("GL_VERTEX_SHADER") == 0) {
					pg_Shader_Stages[indConfiguration][ind_shader_type][ind_shader_stage] = GL_VERTEX_SHADER;
				}
				else if (shader_stage.compare("GL_TESS_CONTROL_SHADER") == 0) {
					pg_Shader_Stages[indConfiguration][ind_shader_type][ind_shader_stage] = GL_TESS_CONTROL_SHADER;
				}
				else if (shader_stage.compare("GL_TESS_EVALUATION_SHADER") == 0) {
					pg_Shader_Stages[indConfiguration][ind_shader_type][ind_shader_stage] = GL_TESS_EVALUATION_SHADER;
				}
				else if (shader_stage.compare("GL_GEOMETRY_SHADER") == 0) {
					pg_Shader_Stages[indConfiguration][ind_shader_type][ind_shader_stage] = GL_GEOMETRY_SHADER;
				}
				else if (shader_stage.compare("GL_FRAGMENT_SHADER") == 0) {
					pg_Shader_Stages[indConfiguration][ind_shader_type][ind_shader_stage] = GL_FRAGMENT_SHADER;
				}
				else {
					sprintf(ErrorStr, "Error: unknown shader type [%s]!", shader_stage.c_str()); ReportError(ErrorStr); throw 430;
				}
			}
		}
#if defined(var_part_initialization) 
		if (ScenarioVarConfigurations[_part_initialization][indConfiguration]) {
			if (ind_shader_type == _pg_shader_ParticleAnimation
				&& (pg_Shader_nbStages[indConfiguration][ind_shader_type] == 0
					|| pg_Shader_File_Names[indConfiguration][ind_shader_type] == "NULL")) {
				sprintf(ErrorStr, "Error: active shader file for Particle Animation is missing in header file (name %s/configuration #%d/%s, nb stages %d)",
					pg_Shader_File_Names[indConfiguration][ind_shader_type].c_str(), indConfiguration, pg_ConfigurationFileNames[indConfiguration].c_str(),
					pg_Shader_nbStages[indConfiguration][ind_shader_type]); ReportError(ErrorStr); throw(6778);
				printf("Particle aniation shader, ");
			}
			if (ind_shader_type == _pg_shader_ParticleRender
				&& (pg_Shader_nbStages[indConfiguration][ind_shader_type] == 0
					|| pg_Shader_File_Names[indConfiguration][ind_shader_type] == "NULL")) {
				sprintf(ErrorStr, "Error: active shader file for Particle Rendering is missing in header file (name %s/configuration #%d/%s, nb stages %d)",
					pg_Shader_File_Names[indConfiguration][ind_shader_type].c_str(), indConfiguration, pg_ConfigurationFileNames[indConfiguration].c_str(),
					pg_Shader_nbStages[indConfiguration][ind_shader_type]); ReportError(ErrorStr); throw(6778);
				printf("Particle aniation shader, ");
			}
		}
#endif
#if defined(var_sensor_layout) 
		if (ScenarioVarConfigurations[_sensor_layout][indConfiguration]) {
			if (ind_shader_type == _pg_shader_Sensor
				&& (pg_Shader_nbStages[indConfiguration][ind_shader_type] == 0
					|| pg_Shader_File_Names[indConfiguration][ind_shader_type] == "NULL")) {
				sprintf(ErrorStr, "Error: active shader file for Sensors is missing in header file (name %s/configuration #%d/%s, nb stages %d)",
					pg_Shader_File_Names[indConfiguration][ind_shader_type].c_str(), indConfiguration, pg_ConfigurationFileNames[indConfiguration].c_str(),
					pg_Shader_nbStages[indConfiguration][ind_shader_type]); ReportError(ErrorStr); throw(6778);
				printf("Particle aniation shader, ");
			}
		}
#endif
#if defined(var_activeClipArts) 
		if (ScenarioVarConfigurations[_activeClipArts][indConfiguration]) {
			if (ind_shader_type == _pg_shader_ClipArt
				&& (pg_Shader_nbStages[indConfiguration][ind_shader_type] == 0
					|| pg_Shader_File_Names[indConfiguration][ind_shader_type] == "NULL")) {
				sprintf(ErrorStr, "Error: active shader file for Clip Arts is missing in header file (name %s, nb stages%d)", pg_Shader_File_Names[indConfiguration][ind_shader_type].c_str(), pg_Shader_nbStages[indConfiguration][ind_shader_type]); ReportError(ErrorStr); throw(6778);
				printf("Particle aniation shader, ");
			}
		}
#endif
#if defined(var_activeMeshes) 
		if (ScenarioVarConfigurations[_activeMeshes][indConfiguration]) {
			if (ind_shader_type == _pg_shader_Mesh
				&& (pg_Shader_nbStages[indConfiguration][ind_shader_type] == 0
					|| pg_Shader_File_Names[indConfiguration][ind_shader_type] == "NULL")) {
				sprintf(ErrorStr, "Error: active shader file for Meshes is missing in header file (name %s/configuration #%d/%s, nb stages %d)",
					pg_Shader_File_Names[indConfiguration][ind_shader_type].c_str(), indConfiguration, pg_ConfigurationFileNames[indConfiguration].c_str(),
					pg_Shader_nbStages[indConfiguration][ind_shader_type]); ReportError(ErrorStr); throw(6778);
				printf("Particle aniation shader, ");
		}
	}
#endif
		if (ind_shader_type == _pg_shader_Update
			&& (pg_Shader_nbStages[indConfiguration][ind_shader_type] == 0
				|| pg_Shader_File_Names[indConfiguration][ind_shader_type] == "NULL")) {
			sprintf(ErrorStr, "Error: active shader file for Update is missing in header file (name %s/configuration #%d/%s, nb stages %d)",
				pg_Shader_File_Names[indConfiguration][ind_shader_type].c_str(), indConfiguration, pg_ConfigurationFileNames[indConfiguration].c_str(),
				pg_Shader_nbStages[indConfiguration][ind_shader_type]); ReportError(ErrorStr); throw(6778);
			printf("Particle aniation shader, ");
		}
		if (ind_shader_type == _pg_shader_Mixing
			&& (pg_Shader_nbStages[indConfiguration][ind_shader_type] == 0
				|| pg_Shader_File_Names[indConfiguration][ind_shader_type] == "NULL")) {
			sprintf(ErrorStr, "Error: active shader file for Mixing is missing in header file (name %s/configuration #%d/%s, nb stages %d)",
				pg_Shader_File_Names[indConfiguration][ind_shader_type].c_str(), indConfiguration, pg_ConfigurationFileNames[indConfiguration].c_str(),
				pg_Shader_nbStages[indConfiguration][ind_shader_type]); ReportError(ErrorStr); throw(6778);
			printf("Particle aniation shader, ");
		}
		if (ind_shader_type == _pg_shader_Master
			&& (pg_Shader_nbStages[indConfiguration][ind_shader_type] == 0
				|| pg_Shader_File_Names[indConfiguration][ind_shader_type] == "NULL")) {
			sprintf(ErrorStr, "Error: active shader file for Master is missing in header file (name %s/configuration #%d/%s, nb stages %d)",
				pg_Shader_File_Names[indConfiguration][ind_shader_type].c_str(), indConfiguration, pg_ConfigurationFileNames[indConfiguration].c_str(),
				pg_Shader_nbStages[indConfiguration][ind_shader_type]); ReportError(ErrorStr); throw(6778);
			printf("Particle aniation shader, ");
		}
	}

	// /shader_files
	std::getline(confFin, line);

	// directories
	std::getline(confFin, line);
	stringstreamStoreLine(&sstream, &line);
	sstream >> ID;
	if (ID.compare("directories") != 0) {
		sprintf(ErrorStr, "Error: incorrect configuration file expected string \"directories\" not found! (instead \"%s\")", ID.c_str()); ReportError(ErrorStr); throw 100;
	}
	int nb_directories;
	sstream >> nb_directories;
	for (int i = 0; i < nb_directories; i++) {
		// reads a configuration directory
		std::getline(confFin, line);
		stringstreamStoreLine(&sstream, &line);
		sstream >> ID; // string describing the storage type, eg snapshots...
		if (ID.compare("snapshots") == 0) {
			sstream >> snapshots_dir_path_prefix; // string of the snapshots and log file directory
		}
		else {
			sprintf(ErrorStr, "Error: incorrect directory storage ID expected string \"snapshots\" not found! (instead \"%s\")", ID.c_str()); ReportError(ErrorStr); throw 100;
		}
	}

	// /directories
	std::getline(confFin, line);

	// /head
	std::getline(confFin, line);
	if (indConfiguration == 0) {
		screen_font_file_name = "Data/fonts/usascii/arial/stb_font_arial_15_usascii.png";
		screen_font_size = 15;
#if defined(TVW)
		display_font_file_name = "Data/fonts/usascii/arial/stb_font_arial_25_usascii.png";
		display_font_size = 25;
#endif
		font_file_encoding = "png";
		font_texture_encoding = PNG;

		snapshots_dir_path_name = snapshots_dir_path_prefix + "/pic_" + project_name + "_" + date_stringStream.str() + "/";
		pg_csv_file_name = snapshots_dir_path_name + "porphyrograph-" + project_name + "-" + date_stringStream.str() + ".csv";

		int nError = 0;
#ifdef _WIN32
		nError = CreateDirectoryA(snapshots_dir_path_name.c_str(), NULL); // can be used on Windows
#else 
		nError = mkdir(snapshots_dir_path_name.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH); // can be used on non-Windows
#endif
		if (nError != 0) {
			// handle your error here
		}
	}

	/////////////////////////////////////////////////////////////////
	// ASSIGNS INITIAL VALUES TO ALL CONFIGURATION FILES
	//InitializeConfigurationVar();

	setWindowDimensions();

	if (indConfiguration == 0) {
		printf("Window %dx%d working window width:%d working window doubling (1/0) %d Margin:%d TopLeft %dx%d\n",
			window_width, window_height, workingWindow_width, double_window,
			rightWindowVMargin, my_window_x, my_window_y);
	}
}

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
// SCENARIO FILE
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

int pg_varID_to_rank(string var_ID, int indConfig) {
	for (int indP = 0; indP < _MaxInterpVarIDs; indP++) {
		if (strcmp(ScenarioVarStrings[indP], var_ID.c_str()) == 0) {
			return indP;
		}
	}
	sprintf(ErrorStr, "Error: scenario variable %s of configuration %d not found in the variable string list generated by python generator\n", var_ID.c_str(), indConfig); ReportError(ErrorStr); throw 50;
	return -1;
}

void ParseScenarioVideos(std::ifstream& scenarioFin, int indConfiguration) {
	std::stringstream  sstream;
	string line;
	string ID;
	string temp;
	string temp2;

	////////////////////////////
	////// VIDEOS
	nb_movies[indConfiguration] = 0;
	// Number of videos
	std::getline(scenarioFin, line);
	stringstreamStoreLine(&sstream, &line);
	sstream >> ID; // string videos
	if (ID.compare("videos") != 0) {
		sprintf(ErrorStr, "Error: incorrect configuration file expected string \"videos\" not found! (instead \"%s\")", ID.c_str()); ReportError(ErrorStr); throw 100;
	}
	sstream >> nb_movies[indConfiguration];

	for (int indVideo = 0; indVideo < nb_movies[indConfiguration]; indVideo++) {
		std::getline(scenarioFin, line);
		// std::cout << "scene: " << line << "\n";
		stringstreamStoreLine(&sstream, &line);
		sstream >> ID; // string movie
		sstream >> temp;
		movieFileName[indConfiguration].push_back(temp);
		sstream >> temp2;
		movieShortName[indConfiguration].push_back(temp2);

		// in addition to the movie name and short name, 2 additional
		// sound files can be provided that are synchronized with the 
		// movie and used to generate beats from sound envelope at 1
		// or sound onsets detected through aubio library

		// possible additional soundtrack file with peaked sound envelope at 1.0 or above
		// NULL value or no value means no file
		if (sstream >> temp2) {
			// there is a soundtrack file with peaked sound envelope at 1.0
			if (temp2.compare("") != 0 && temp2.compare("NULL") != 0) {
				movieSoundtrackPeaksFileName[indConfiguration].push_back(temp2);
				string csv_line;
				vector<float> peak_times;
				std::ifstream peak_file(temp2);
				if (!peak_file) {
					sprintf(ErrorStr, "Error: peak file [%s] not found!", temp2.c_str()); ReportError(ErrorStr); throw 11;
				}
				printf("Read video soundtrack peaks [%s]\n", temp2.c_str());
				// reads the peaks timecodes and stores them in a float vector
				std::getline(peak_file, csv_line);
				std::getline(peak_file, csv_line);
				std::getline(peak_file, csv_line);
				while (std::getline(peak_file, csv_line)) {
					std::stringstream  peak_sstream;
					stringstreamStoreLine(&peak_sstream, &csv_line);
					float time, sound;
					peak_sstream >> time;
					peak_sstream >> sound;
					if (sound >= 1) {
						peak_times.push_back(time);
					}
				}
				peak_file.close();
				movieSoundtrackPeaks[indConfiguration].push_back(peak_times);
				//for (int i = 0; i < int(peak_times.size()); ++i) {
				//	std::cout << "Peak: " << peak_times[i] << '\n';
				//}
			}
			else {
				movieSoundtrackPeaksFileName[indConfiguration].push_back("");
				movieSoundtrackPeaks[indConfiguration].push_back({});
			}
			if (sstream >> temp2 && temp2.compare("") != 0 && temp2.compare("NULL") != 0) {
				movieSoundtrackOnsetsFileName[indConfiguration].push_back(temp2);
				string csv_line;
				vector<float> onset_times;
				std::ifstream onset_file(temp2);
				if (!onset_file) {
					sprintf(ErrorStr, "Error: onset file [%s] not found!", temp2.c_str()); ReportError(ErrorStr); throw 11;
				}
				// reads the peaks timecodes and stores them in a float vector
				printf("Read video soundtrack onsets [%s]\n", temp2.c_str());
				while (std::getline(onset_file, csv_line)) {
					std::stringstream  onset_sstream;
					stringstreamStoreLine(&onset_sstream, &csv_line);
					float time;
					onset_sstream >> time;
					onset_times.push_back(time);
				}
				onset_file.close();
				movieSoundtrackOnsets[indConfiguration].push_back(onset_times);
				//for (int i = 0; i < int(onset_times.size()); ++i) {
				//	std::cout << "Onset: " << onset_times[i] << '\n';
				//}
			}
			else {
				movieSoundtrackOnsetsFileName[indConfiguration].push_back("");
				movieSoundtrackOnsets[indConfiguration].push_back({});
			}
		}
		else {
			movieSoundtrackPeaksFileName[indConfiguration].push_back("");
			movieSoundtrackPeaks[indConfiguration].push_back({});
			movieSoundtrackOnsetsFileName[indConfiguration].push_back("");
			movieSoundtrackOnsets[indConfiguration].push_back({});
		}
		//std::cout << "movie : " << 
		// movieFileName[indVideo] << "\n";
	}
	// /videos
	std::getline(scenarioFin, line);
	stringstreamStoreLine(&sstream, &line);
	sstream >> ID; // string /videos
	if (ID.compare("/videos") != 0) {
		sprintf(ErrorStr, "Error: incorrect configuration file expected string \"/videos\" not found! (instead \"%s\")", ID.c_str()); ReportError(ErrorStr); throw 100;
	}
}

void ParseScenarioClipsAndPhotos(std::ifstream& scenarioFin, int indConfiguration) {
	std::stringstream  sstream;
	string line;
	string ID;
	string temp;

	////////////////////////////
	////// SHORT VIDEO CLIPS ALBUMS
	nb_clip_albums[indConfiguration] = 0;
	// Number of photo albums
	std::getline(scenarioFin, line);
	stringstreamStoreLine(&sstream, &line);
	sstream >> ID; // string videos
	if (ID.compare("clips") != 0) {
		sprintf(ErrorStr, "Error: incorrect configuration file expected string \"clips\" not found! (instead \"%s\")", ID.c_str()); ReportError(ErrorStr); throw 100;
	}
	sstream >> nb_clip_albums[indConfiguration];

	if (nb_clip_albums[indConfiguration] > 1) {
		sprintf(ErrorStr, "Error: incorrect configuration file: Only clip photo album with subdirectories is expected! (instead of \"%d\" dirs)", nb_clip_albums[indConfiguration]); ReportError(ErrorStr); throw 100;
	}
	//printf("Clip album Number %d\n", nb_clip_albums);

	if (nb_clip_albums[indConfiguration] > 0) {
		std::getline(scenarioFin, line);
		stringstreamStoreLine(&sstream, &line);
		sstream >> ID; // string album
		sstream >> clipAlbumDirName[indConfiguration];
		std::cout << "Clip album directory: " << clipAlbumDirName << "\n";
		sstream >> clip_image_width[indConfiguration];
		sstream >> clip_image_height[indConfiguration];
		sstream >> clip_crop_width[indConfiguration];
		sstream >> clip_crop_height[indConfiguration];
		sstream >> clip_max_length[indConfiguration];
		if (clip_image_width[indConfiguration] == 0 || clip_image_height[indConfiguration] == 0 || clip_crop_width[indConfiguration] == 0 || clip_crop_height[indConfiguration] == 0 || clip_max_length[indConfiguration] == 0) {
			sprintf(ErrorStr, "Error: incorrect configuration file: missing dimension data for clip image size %dx%d cropped size %dx%d max length %d\n", clip_image_width[indConfiguration], clip_image_height[indConfiguration], clip_crop_width[indConfiguration], clip_crop_height[indConfiguration], clip_max_length[indConfiguration]); ReportError(ErrorStr); throw 100;
		}
	}
	else {
		clipAlbumDirName[indConfiguration] = "";
	}
	if (nb_clip_albums[indConfiguration] > 0) {
		pg_ClipDirectory[indConfiguration] = clipAlbumDirName[indConfiguration];
	}
	else {
		pg_ClipDirectory[indConfiguration] = "";
	}

	// /clips
	std::getline(scenarioFin, line);
	stringstreamStoreLine(&sstream, &line);
	sstream >> ID; // string /photos
	if (ID.compare("/clips") != 0) {
		sprintf(ErrorStr, "Error: incorrect configuration file expected string \"/clips\" not found! (instead \"%s\")", ID.c_str()); ReportError(ErrorStr); throw 100;
	}

	////////////////////////////
	////// PHOTO ALBUMS
	nb_photo_albums[indConfiguration] = 0;
	// Number of photo albums
	std::getline(scenarioFin, line);
	stringstreamStoreLine(&sstream, &line);
	sstream >> ID; // string videos
	if (ID.compare("photos") != 0) {
		sprintf(ErrorStr, "Error: incorrect configuration file expected string \"photos\" not found! (instead \"%s\")", ID.c_str()); ReportError(ErrorStr); throw 100;
	}
	sstream >> nb_photo_albums[indConfiguration];

	if (nb_photo_albums[indConfiguration] > 1) {
		sprintf(ErrorStr, "Error: incorrect configuration file: Only one photo album with subdirectories is expected! (instead of \"%d\" dirs)", nb_photo_albums[indConfiguration]); ReportError(ErrorStr); throw 100;
	}

	if (nb_photo_albums[indConfiguration] > 0) {
		std::getline(scenarioFin, line);
		// std::cout << "scene: " << line << "\n";
		stringstreamStoreLine(&sstream, &line);
		sstream >> ID; // string album
		sstream >> photoAlbumDirName[indConfiguration];
		//std::cout << "photo album : " << photoAlbumDirName << "\n";
	}
	else {
		photoAlbumDirName[indConfiguration] = "";
	}

	if (nb_photo_albums[indConfiguration] > 0 && photoAlbumDirName[indConfiguration].compare("captures") != 0) {
		if (photoAlbumDirName[indConfiguration].find(':') == std::string::npos) {
			pg_ImageDirectory[indConfiguration] = "Data/" + project_name + "-data/";
			if (nb_photo_albums[indConfiguration] > 0) {
				pg_ImageDirectory[indConfiguration] += photoAlbumDirName[indConfiguration];
			}
			else {
				pg_ImageDirectory[indConfiguration] += "images/";
			}
		}
		else {
			pg_ImageDirectory[indConfiguration] = photoAlbumDirName[indConfiguration];
		}
		//std::cout << "Photo album directory: " << pg_ImageDirectory[indConfiguration] << std::endl;
	}

#if defined(TVW)
	else {
		pg_ImageDirectory[indConfiguration] = "captures";
		std::cout << "Using capture images" << std::endl;
	}
	pg_MaskDirectory[indConfiguration] = "Data/" + project_name + "-data/";
	if (nb_photo_albums > 1) {
		pg_MaskDirectory[indConfiguration] += photoAlbumDirName[1];
	}
	else {
		pg_MaskDirectory[indConfiguration] += "masks/";
	}
	std::cout << "Loading masks from " << pg_MaskDirectory << std::endl;
	pg_MessageDirectory[indConfiguration] = "Data/" + project_name + "-data/";
	if (nb_photo_albums > 2) {
		pg_MessageDirectory[indConfiguration] += photoAlbumDirName[2];
	}
	else {
		pg_MessageDirectory[indConfiguration] += "messages/";
	}
	std::cout << "Loading messages from " << pg_MessageDirectory[indConfiguration] << std::endl;
#endif



	// /photos
	std::getline(scenarioFin, line);
	stringstreamStoreLine(&sstream, &line);
	sstream >> ID; // string /photos
	if (ID.compare("/photos") != 0) {
		sprintf(ErrorStr, "Error: incorrect configuration file expected string \"/photos\" not found! (instead \"%s\")", ID.c_str()); ReportError(ErrorStr); throw 100;
	}

#if defined(var_moving_messages)
	pg_MessageDirectory[indConfiguration] = "Data/" + project_name + "-data/";
	pg_MessageDirectory[indConfiguration] += "messages/";

	std::getline(scenarioFin, line);
	stringstreamStoreLine(&sstream, &line);
	sstream >> ID; // string messages
	if (ID.compare("messages") != 0) {
		sprintf(ErrorStr, "Error: incorrect configuration file expected string \"messages\" not found! (instead \"%s\")", ID.c_str()); ReportError(ErrorStr); throw 100;
	}

	std::getline(scenarioFin, line);
	stringstreamStoreLine(&sstream, &line);
	sstream >> ID; // string lines
	sstream >> pg_MessageFile[indConfiguration];
	std::cout << "Loading messages from " << pg_MessageDirectory[indConfiguration] + pg_MessageFile[indConfiguration] << std::endl;

	// /messages
	std::getline(scenarioFin, line);
	stringstreamStoreLine(&sstream, &line);
	sstream >> ID; // string /messages
	if (ID.compare("/messages") != 0) {
		sprintf(ErrorStr, "Error: incorrect configuration file expected string \"/messages\" not found! (instead \"%s\")", ID.c_str()); ReportError(ErrorStr); throw 100;
	}
#endif
}

void ParseScenarioSoundtracks(std::ifstream& scenarioFin, int indConfiguration) {
	std::stringstream  sstream;
	string line;
	string ID;
	string temp;
	string temp2;

	////////////////////////////
	////// SOUNDTRACKS
	nb_soundtracks[indConfiguration] = 0;
	// Number of soundtracks
	std::getline(scenarioFin, line);
	stringstreamStoreLine(&sstream, &line);
	sstream >> ID; // string soundtracks
	if (ID.compare("soundtracks") != 0) {
		sprintf(ErrorStr, "Error: incorrect configuration file expected string \"soundtracks\" not found! (instead \"%s\")", ID.c_str()); ReportError(ErrorStr); throw 100;
	}
	sstream >> nb_soundtracks[indConfiguration];

	for (int indTrack = 0; indTrack < nb_soundtracks[indConfiguration]; indTrack++) {
		std::getline(scenarioFin, line);
		// std::cout << "scene: " << line << "\n";
		stringstreamStoreLine(&sstream, &line);
		sstream >> ID; // string track
		sstream >> temp;
		trackFileName[indConfiguration].push_back(temp);
		sstream >> temp2;
		trackShortName[indConfiguration].push_back(temp2);
		//std::cout << "Soundtrack: " << trackFileName[indConfiguration][indTrack] << " " << trackShortName[indConfiguration][indTrack] << " (#" << indTrack << ")\n";

		// in addition to the track name and short name, 2 additional
		// files can be used to generate beats from sound envelope at 1
		// or sound onsets detected through aubio library

		// possible additional peaked sound envelope at 1.0 or above
		// NULL value or no value means no file
		if (sstream >> temp2) {
			// there is a soundtrack file with peaked sound envelope at 1.0
			if (temp2.compare("") != 0 && temp2.compare("NULL") != 0) {
				trackSoundtrackPeaksFileName[indConfiguration].push_back(temp2);
				string csv_line;
				vector<float> peak_times;
				std::ifstream peak_file(temp2);
				if (!peak_file) {
					sprintf(ErrorStr, "Error: peak file [%s] not found!", temp2.c_str()); ReportError(ErrorStr); throw 11;
				}
				printf("Read audio soundtrack peaks [%s]\n", temp2.c_str());
				// reads the peaks timecodes and stores them in a float vector
				std::getline(peak_file, csv_line);
				std::getline(peak_file, csv_line);
				std::getline(peak_file, csv_line);
				while (std::getline(peak_file, csv_line)) {
					std::stringstream  peak_sstream;
					stringstreamStoreLine(&peak_sstream, &csv_line);
					float time, sound;
					peak_sstream >> time;
					peak_sstream >> sound;
					if (sound >= 1) {
						peak_times.push_back(time);
					}
				}
				peak_file.close();
				trackSoundtrackPeaks[indConfiguration].push_back(peak_times);
				//for (int i = 0; i < int(peak_times.size()); ++i) {
				//	std::cout << "Peak: " << peak_times[i] << '\n';
				//}
			}
			else {
				trackSoundtrackPeaksFileName[indConfiguration].push_back("");
				trackSoundtrackPeaks[indConfiguration].push_back({});
			}
			if (sstream >> temp2 && temp2.compare("") != 0 && temp2.compare("NULL") != 0) {
				trackSoundtrackOnsetsFileName[indConfiguration].push_back(temp2);
				string csv_line;
				vector<float> onset_times;
				std::ifstream onset_file(temp2);
				if (!onset_file) {
					sprintf(ErrorStr, "Error: onset file [%s] not found!", temp2.c_str()); ReportError(ErrorStr); throw 11;
				}
				// reads the peaks timecodes and stores them in a float vector
				printf("Read audio soundtrack onsets [%s]\n", temp2.c_str());
				while (std::getline(onset_file, csv_line)) {
					std::stringstream  onset_sstream;
					stringstreamStoreLine(&onset_sstream, &csv_line);
					float time;
					onset_sstream >> time;
					onset_times.push_back(time);
				}
				onset_file.close();
				trackSoundtrackOnsets[indConfiguration].push_back(onset_times);
				//for (int i = 0; i < int(onset_times.size()); ++i) {
				//	std::cout << "Onset: " << onset_times[i] << '\n';
				//}
			}
			else {
				trackSoundtrackOnsetsFileName[indConfiguration].push_back("");
				trackSoundtrackOnsets[indConfiguration].push_back({});
			}
			float offset = 0.f;
			sstream >> offset;
			trackSoundtrackOnsetsAndPeasksOffset[indConfiguration].push_back(offset);
		}
		else {
			trackSoundtrackPeaksFileName[indConfiguration].push_back("");
			trackSoundtrackPeaks[indConfiguration].push_back({});
			trackSoundtrackOnsetsFileName[indConfiguration].push_back("");
			trackSoundtrackOnsets[indConfiguration].push_back({});
			trackSoundtrackOnsetsAndPeasksOffset[indConfiguration].push_back(0.f);
		}
		//std::cout << "track : " << 
		// trackFileName[indVideo] << "\n";


	}
	// /soundtracks
	std::getline(scenarioFin, line);
	stringstreamStoreLine(&sstream, &line);
	sstream >> ID; // string /soundtracks
	if (ID.compare("/soundtracks") != 0) {
		sprintf(ErrorStr, "Error: incorrect configuration file expected string \"/soundtracks\" not found! (instead \"%s\")", ID.c_str()); ReportError(ErrorStr); throw 100;
	}
}

void ParseScenarioSVGPaths(std::ifstream& scenarioFin, int indConfiguration) {
	std::stringstream  sstream;
	string line;
	string ID;
	string temp;
	string temp2;

	////////////////////////////
	////// ClipArt PATHS
	// initializes the tracks for recording the strokes
	// has to be made before loading the scenario that may load predefined svg paths
	// and has to be made after the configuration file loading that has 
	// the value max_mouse_recording_frames for the max number of points to memorize
	pg_initStrokes();

	// Number of ClipArt paths
	std::getline(scenarioFin, line);
	stringstreamStoreLine(&sstream, &line);
	sstream >> ID; // string svg_paths
	if (ID.compare("svg_paths") != 0) {
		sprintf(ErrorStr, "Error: incorrect configuration file expected string \"svg_paths\" not found! (instead \"%s\")", ID.c_str()); ReportError(ErrorStr); throw 100;
	}
	pg_nb_SVG_paths[indConfiguration] = 0;
	pg_current_SVG_path_group = 0;
	sstream >> pg_nb_SVG_paths[indConfiguration];
	//printf("nb svg paths %d\n", nb_paths[indConfiguration]);
	pg_SVG_paths[indConfiguration] = new SVG_path[pg_nb_SVG_paths[indConfiguration]]();

	for (int indPath = 0; indPath < pg_nb_SVG_paths[indConfiguration]; indPath++) {
		std::getline(scenarioFin, line);
		stringstreamStoreLine(&sstream, &line);
		sstream >> ID; // string svg_path
		sstream >> temp; // file name
		string fileName = "Data/" + project_name + "-data/SVGs/" + temp;
		sstream >> temp2;
		int pathRank = my_stoi(temp2);
		sstream >> temp2;
		int indTrack = my_stoi(temp2);
		sstream >> temp2;
		float pathRadius = my_stof(temp2);
		sstream >> temp2;
		float path_r_color = my_stof(temp2);
		sstream >> temp2;
		float path_g_color = my_stof(temp2);
		sstream >> temp2;
		float path_b_color = my_stof(temp2);
		sstream >> temp2;
		float path_readSpeedScale = my_stof(temp2);
		string path_ID = "";
		sstream >> path_ID;
		sstream >> temp2;
		int path_group = my_stoi(temp2);
		if (path_group <= 0) {
			sprintf(ErrorStr, "Error: incorrect scenario file group %d for SVG path path %d number (\"%s\"), path group should be stricly positive",
				indTrack, pathRank, fileName.c_str()); ReportError(ErrorStr); throw 100;
		}
		else {
			pg_nb_SVG_path_groups[indConfiguration] = max(pg_nb_SVG_path_groups[indConfiguration], path_group);
		}
		sstream >> temp2;
		bool with_color_radius_from_scenario = (my_stoi(temp2) != 0);
		sstream >> temp2;
		double secondsforwidth = my_stod(temp2);

		//printf("path no %d group %d\n", paths[indConfiguration][indPath].pathRank, paths[indConfiguration][indPath].path_group);
		pg_SVG_paths[indConfiguration][indPath].SVG_path_init(pathRank, indTrack, pathRadius, path_r_color, path_g_color, path_b_color, path_readSpeedScale,
			path_ID, fileName, path_group, with_color_radius_from_scenario, secondsforwidth);
		//printf("path no %d group %d\n", paths[indConfiguration][indPath].indPath, paths[indConfiguration][indPath].path_group);
		// checks whether the paths are not duplicated
		for (int indAux = 0; indAux < indPath; indAux++) {
			if (pg_SVG_paths[indConfiguration][indAux].indPath == pg_SVG_paths[indConfiguration][indPath].indPath
				&& pg_SVG_paths[indConfiguration][indAux].path_group == pg_SVG_paths[indConfiguration][indPath].path_group) {
				sprintf(ErrorStr, "Error: incorrect configuration file paths %d and %d have the same path index %d and same path group %d", 
					indAux, indPath, pg_SVG_paths[indConfiguration][indAux].indPath, pg_SVG_paths[indConfiguration][indAux].path_group); ReportError(ErrorStr); throw 100;
			}
		}

		//printf("indPath %d indTrack %d pathRadius %.2f path_r_color %.2f path_g_color %.2f path_b_color %.2f path_readSpeedScale %.2f\n",
		//	pathRank, indTrack, pathRadius, path_r_color, path_g_color, path_b_color, path_readSpeedScale);
		if (indTrack >= 0 && indTrack < PG_NB_TRACKS && pathRank >= 1 && pathRank <= PG_NB_PATHS) {
			if (path_group - 1 == pg_current_SVG_path_group) {
				//printf("Loading ClipArt path %s track %d\n", (char*)("Data/" + project_name + "-data/ClipArt/" + temp).c_str(), indTrack);
#if defined(var_path_replay_trackNo_1) && defined(var_path_record_1)
				if (ScenarioVarConfigurations[_path_replay_trackNo_1][indConfiguration] && ScenarioVarConfigurations[_path_record_1][indConfiguration]) {
					load_svg_path((char*)fileName.c_str(),
						pathRank, indTrack, pathRadius, path_r_color, path_g_color, path_b_color,
						path_readSpeedScale, path_ID, with_color_radius_from_scenario, secondsforwidth, indConfiguration);
				}
#endif
			}
		}
		else {
			sprintf(ErrorStr, "Error: incorrect scenario file track %d for SVG path %d number (\"%s\")",
				indTrack, pathRank, fileName.c_str()); ReportError(ErrorStr); throw 100;
		}
		//std::cout << "svg_path #" << pathRank << ": " << "Data/" + project_name + "-data/ClipArt/" + temp << " track #" << indTrack << "\n";
	}

	// /svg_paths
	std::getline(scenarioFin, line);
	stringstreamStoreLine(&sstream, &line);
	sstream >> ID; // string /svg_paths
	if (ID.compare("/svg_paths") != 0) {
		sprintf(ErrorStr, "Error: incorrect configuration file expected string \"/svg_paths\" not found! (instead \"%s\")", ID.c_str()); ReportError(ErrorStr); throw 100;
	}
}

void pg_listAllSVG_paths(void) {
	printf("Listing SVG paths:\n");
	for (int indConfiguration = 0; indConfiguration < _NbConfigurations; indConfiguration++) {
		std::cout << "    " << indConfiguration << ": ";
		for (int indPath = 0; indPath < pg_nb_SVG_paths[indConfiguration]; indPath++) {
			if (ScenarioVarConfigurations[_path_replay_trackNo_1][indConfiguration] && ScenarioVarConfigurations[_path_record_1][indConfiguration]) {
				std::cout << pg_SVG_paths[indConfiguration][indPath].path_fileName << " (" << pg_SVG_paths[indConfiguration][indPath].indPath << ", "
					<< pg_SVG_paths[indConfiguration][indPath].path_group << "), ";
			}
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

void pg_listAllSoundtracks(void) {
	printf("Listing Soundtracks:\n");
	for (int indConfiguration = 0; indConfiguration < _NbConfigurations; indConfiguration++) {
		std::cout << "    " << indConfiguration << ": ";
		for (int indTrack = 0; indTrack < nb_soundtracks[indConfiguration]; indTrack++) {
			std::cout << trackFileName[indConfiguration][indTrack] << " (" << trackShortName[indConfiguration][indTrack] << "), ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

void ParseScenarioClipArt(std::ifstream& scenarioFin, int indConfiguration) {
	std::stringstream  sstream;
	string line;
	string ID;
	string temp;

	////////////////////////////
	////// ClipArt GPU PATHS
	// the paths are loaded inside the GPU and diplayed path by path

	// Number of ClipArt
	std::getline(scenarioFin, line);
	stringstreamStoreLine(&sstream, &line);
	sstream >> ID; // string clip_arts
	if (ID.compare("clip_arts") != 0) {
		sprintf(ErrorStr, "Error: incorrect configuration file expected string \"clip_arts\" not found! (instead \"%s\")", ID.c_str()); ReportError(ErrorStr); throw 100;
	}

	sstream >> pg_nb_ClipArt[indConfiguration];
	printf("Nb clip arts %d config %d\n", pg_nb_ClipArt[indConfiguration], indConfiguration);
	printf("Loading clipArts:\n");
	pg_nb_paths_in_ClipArt[indConfiguration] = new int[pg_nb_ClipArt[indConfiguration]];
		pg_ClipArt_fileNames[indConfiguration] = new string[pg_nb_ClipArt[indConfiguration]];

	pg_ClipArt_Scale[indConfiguration] = new float[pg_nb_ClipArt[indConfiguration]];
	for (int indFile = 0; indFile < pg_nb_ClipArt[indConfiguration]; indFile++) {
		pg_ClipArt_Scale[indConfiguration][indFile] = 0.1f;
	}
	pg_ClipArt_Rotation[indConfiguration] = new float[pg_nb_ClipArt[indConfiguration]];
	memset((char*)pg_ClipArt_Rotation[indConfiguration], 0, pg_nb_ClipArt[indConfiguration] * sizeof(float));
	pg_ClipArt_Translation_X[indConfiguration] = new float[pg_nb_ClipArt[indConfiguration]];
	memset((char*)pg_ClipArt_Translation_X[indConfiguration], 0, pg_nb_ClipArt[indConfiguration] * sizeof(float));
	pg_ClipArt_Translation_Y[indConfiguration] = new float[pg_nb_ClipArt[indConfiguration]];
	memset((char*)pg_ClipArt_Translation_Y[indConfiguration], 0, pg_nb_ClipArt[indConfiguration] * sizeof(float));

	pg_nb_tot_SvgGpu_paths[indConfiguration] = 0;
	pg_ClipArt_SubPath[indConfiguration] = new bool *[pg_nb_ClipArt[indConfiguration]];
	pg_ClipArt_Colors[indConfiguration] = new pg_ClipArt_Colors_Types *[pg_nb_ClipArt[indConfiguration]];
	for (int indClipArtFile = 0; indClipArtFile < pg_nb_ClipArt[indConfiguration]; indClipArtFile++) {
		std::getline(scenarioFin, line);
		stringstreamStoreLine(&sstream, &line);
		sstream >> ID; // string svg_clip_art
		sstream >> pg_ClipArt_fileNames[indConfiguration][indClipArtFile]; // file name
		sstream >> pg_nb_paths_in_ClipArt[indConfiguration][indClipArtFile]; // number of paths in the file
		pg_nb_tot_SvgGpu_paths[indConfiguration] += pg_nb_paths_in_ClipArt[indConfiguration][indClipArtFile];
		printf("%s, ", pg_ClipArt_fileNames[indConfiguration][indClipArtFile].c_str());
		//printf("ind path file %d name %s nb paths %d, ", indClipArtFile, pg_ClipArt_fileNames[indConfiguration][indClipArtFile].c_str(), pg_nb_paths_in_ClipArt[indConfiguration][indClipArtFile]);

		pg_ClipArt_SubPath[indConfiguration][indClipArtFile] = new bool[pg_nb_paths_in_ClipArt[indConfiguration][indClipArtFile]];
		for (int indPath = 0; indPath < pg_nb_paths_in_ClipArt[indConfiguration][indClipArtFile]; indPath++) {
			pg_ClipArt_SubPath[indConfiguration][indClipArtFile][indPath] = true;
		}

		pg_ClipArt_Colors[indConfiguration][indClipArtFile] = new pg_ClipArt_Colors_Types[pg_nb_paths_in_ClipArt[indConfiguration][indClipArtFile]];
		for (int indPath = 0; indPath < pg_nb_paths_in_ClipArt[indConfiguration][indClipArtFile]; indPath++) {
			pg_ClipArt_Colors[indConfiguration][indClipArtFile][indPath] = ClipArt_nat;
		}

		// image initial geometry
		sstream >> pg_ClipArt_Scale[indConfiguration][indClipArtFile];
		sstream >> pg_ClipArt_Translation_X[indConfiguration][indClipArtFile];
		sstream >> pg_ClipArt_Translation_Y[indConfiguration][indClipArtFile];
		//printf("ind clipart %d scale %.2f pos %.2f %.2f\n", indClipArtFile, pg_ClipArt_Scale[indConfiguration][indClipArtFile], pg_ClipArt_Translation_X[indConfiguration][indClipArtFile], pg_ClipArt_Translation_Y[indConfiguration][indClipArtFile]);
		sstream >> pg_ClipArt_Rotation[indConfiguration][indClipArtFile];
		sstream >> ID;
		if (ID.compare("nat") == 0) {
			for (int indPath = 0; indPath < pg_nb_paths_in_ClipArt[indConfiguration][indClipArtFile]; indPath++) {
				pg_ClipArt_Colors[indConfiguration][indClipArtFile][indPath] = ClipArt_nat;
			}
		}
		else if (ID.compare("white") == 0) {
			for (int indPath = 0; indPath < pg_nb_paths_in_ClipArt[indConfiguration][indClipArtFile]; indPath++) {
				pg_ClipArt_Colors[indConfiguration][indClipArtFile][indPath] = ClipArt_white;
			}
		}
		else if (ID.compare("red") == 0) {
			for (int indPath = 0; indPath < pg_nb_paths_in_ClipArt[indConfiguration][indClipArtFile]; indPath++) {
				pg_ClipArt_Colors[indConfiguration][indClipArtFile][indPath] = ClipArt_red;
			}
		}
		else if (ID.compare("green") == 0) {
			for (int indPath = 0; indPath < pg_nb_paths_in_ClipArt[indConfiguration][indClipArtFile]; indPath++) {
				pg_ClipArt_Colors[indConfiguration][indClipArtFile][indPath] = ClipArt_green;
			}
		}
		else if (ID.compare("blue") == 0) {
			for (int indPath = 0; indPath < pg_nb_paths_in_ClipArt[indConfiguration][indClipArtFile]; indPath++) {
				pg_ClipArt_Colors[indConfiguration][indClipArtFile][indPath] = ClipArt_blue;
			}
		}
		else {
			sprintf(ErrorStr, "Error: incorrect configuration file ClipArt GPU color \"%s\" (nat, white, red, blue or green expected)", ID.c_str()); ReportError(ErrorStr); throw 100;
		}
	}
	printf("\n");

	// /svg_clip_arts
	std::getline(scenarioFin, line);
	stringstreamStoreLine(&sstream, &line);
	sstream >> ID; // string /clip_arts
	if (ID.compare("/clip_arts") != 0) {
		sprintf(ErrorStr, "Error: incorrect configuration file expected string \"/clip_arts\" not found! (instead \"%s\")", ID.c_str()); ReportError(ErrorStr); throw 100;
	}
}

void ParseScenarioMeshes(std::ifstream& scenarioFin, int indConfiguration) {
	std::stringstream  sstream;
	string line;
	string ID;
	string temp;

#if defined(var_activeMeshes)
	if (ScenarioVarConfigurations[_activeMeshes][indConfiguration]) {
		////////////////////////////
		////// MESHES
		// the meshes are loaded inside the GPU and diplayed depending on their activity

		// Number of meshes
		std::getline(scenarioFin, line);
		stringstreamStoreLine(&sstream, &line);
		sstream >> ID; // string meshes
		if (ID.compare("meshes") != 0) {
			sprintf(ErrorStr, "Error: incorrect configuration file expected string \"meshes\" not found! (instead \"%s\")", ID.c_str()); ReportError(ErrorStr); throw 100;
		}

		// number of obj files in the configuration file
		sstream >> pg_nb_Mesh_files[indConfiguration];

		// Augmented Reality: FBO capture of Master to be displayed on a mesh
#if defined(var_textureFrontier_wmin)&& defined(var_textureFrontier_wmax) and defined(var_textureFrontier_hmin) && defined(var_textureFrontier_hmax) && defined(var_textureFrontier_wmin_width) && defined(var_textureFrontier_wmax_width) and defined(var_textureFrontier_hmin_width) && defined(var_textureFrontier_hmax_width) && defined(var_textureScale_w) && defined(var_textureScale_h) and defined(var_textureTranslate_w) && defined(var_textureTranslate_h)
		if (ScenarioVarConfigurations[_textureFrontier_wmin][indConfiguration]) {
			if (pg_nb_Mesh_files[indConfiguration] <= 0) {
				sprintf(ErrorStr, "Error: Augemented reality requires that at least one mesh file is declared in the scenario file"); ReportError(ErrorStr); throw 100;
			}
		}
#endif

		pg_Mesh_fileNames[indConfiguration] = new string[pg_nb_Mesh_files[indConfiguration]];

		pg_Mesh_Scale[indConfiguration] = new float[pg_nb_Mesh_files[indConfiguration]];
#if defined(var_Caverne_Mesh_Profusion)
		if (ScenarioVarConfigurations[_Caverne_Mesh_Profusion][indConfiguration]) {
			pg_CaverneActveMesh[indConfiguration] = new bool[pg_nb_Mesh_files[indConfiguration]];
			pg_CaverneMeshWakeupTime[indConfiguration] = new float[pg_nb_Mesh_files[indConfiguration]];
			pg_CaverneMeshBirthTime[indConfiguration] = new float[pg_nb_Mesh_files[indConfiguration]];
			pg_CaverneMeshDeathTime[indConfiguration] = new float[pg_nb_Mesh_files[indConfiguration]];
		}
#endif
#if defined(var_MmeShanghai_brokenGlass)
		if (ScenarioVarConfigurations[_MmeShanghai_brokenGlass][indConfiguration]) {
			pg_MmeShanghai_MeshSubParts[indConfiguration] = new bool** [pg_nb_Mesh_files[indConfiguration]];
			pg_MmeShanghai_NbMeshSubParts[indConfiguration] = new int[pg_nb_Mesh_files[indConfiguration]];
			pg_MmeShanghai_MeshSubPart_FileNames[indConfiguration] = new string * [pg_nb_Mesh_files[indConfiguration]];
		}
#endif
		for (int indFile = 0; indFile < pg_nb_Mesh_files[indConfiguration]; indFile++) {
			pg_Mesh_Scale[indConfiguration][indFile] = 1.0f;
#if defined(var_Caverne_Mesh_Profusion)
			if (ScenarioVarConfigurations[_Caverne_Mesh_Profusion][indConfiguration]) {
				pg_CaverneActveMesh[indConfiguration][indFile] = false;
				if (indFile < 7) {
					pg_CaverneMeshWakeupTime[indConfiguration][indFile] = float(rand_0_1 * 10.);
				}
				else {
					pg_CaverneMeshWakeupTime[indConfiguration][indFile] = float(rand_0_1 * 30.);
				}
				pg_CaverneMeshBirthTime[indConfiguration][indFile] = 0.f;
				pg_CaverneMeshDeathTime[indConfiguration][indFile] = 0.f;
			}
#endif
#if defined(var_MmeShanghai_brokenGlass)
			if (ScenarioVarConfigurations[_MmeShanghai_brokenGlass][indConfiguration]) {
				pg_MmeShanghai_MeshSubParts[indConfiguration][indFile] = NULL;
				pg_MmeShanghai_NbMeshSubParts[indConfiguration][indFile] = 0;
				pg_MmeShanghai_MeshSubPart_FileNames[indConfiguration][indFile] = NULL;
			}
#endif
		}

		pg_Mesh_Rotation_angle[indConfiguration] = new float[pg_nb_Mesh_files[indConfiguration]];
		pg_Mesh_Rotation_X[indConfiguration] = new float[pg_nb_Mesh_files[indConfiguration]];
		pg_Mesh_Rotation_Y[indConfiguration] = new float[pg_nb_Mesh_files[indConfiguration]];
		pg_Mesh_Rotation_Z[indConfiguration] = new float[pg_nb_Mesh_files[indConfiguration]];
		pg_Mesh_Rotation_Ini_X[indConfiguration] = new float[pg_nb_Mesh_files[indConfiguration]];
		pg_Mesh_Rotation_Ini_Y[indConfiguration] = new float[pg_nb_Mesh_files[indConfiguration]];
		pg_Mesh_Rotation_Ini_Z[indConfiguration] = new float[pg_nb_Mesh_files[indConfiguration]];
		memset((char*)pg_Mesh_Rotation_angle[indConfiguration], 0, pg_nb_Mesh_files[indConfiguration] * sizeof(float));
		memset((char*)pg_Mesh_Rotation_X[indConfiguration], 0, pg_nb_Mesh_files[indConfiguration] * sizeof(float));
		memset((char*)pg_Mesh_Rotation_Y[indConfiguration], 0, pg_nb_Mesh_files[indConfiguration] * sizeof(float));
		memset((char*)pg_Mesh_Rotation_Z[indConfiguration], 0, pg_nb_Mesh_files[indConfiguration] * sizeof(float));
		memset((char*)pg_Mesh_Rotation_Ini_X[indConfiguration], 0, pg_nb_Mesh_files[indConfiguration] * sizeof(float));
		memset((char*)pg_Mesh_Rotation_Ini_Y[indConfiguration], 0, pg_nb_Mesh_files[indConfiguration] * sizeof(float));
		memset((char*)pg_Mesh_Rotation_Ini_Z[indConfiguration], 0, pg_nb_Mesh_files[indConfiguration] * sizeof(float));
		pg_Mesh_Translation_X[indConfiguration] = new float[pg_nb_Mesh_files[indConfiguration]];
		memset((char*)pg_Mesh_Translation_X[indConfiguration], 0, pg_nb_Mesh_files[indConfiguration] * sizeof(float));
		pg_Mesh_Translation_Y[indConfiguration] = new float[pg_nb_Mesh_files[indConfiguration]];
		memset((char*)pg_Mesh_Translation_Y[indConfiguration], 0, pg_nb_Mesh_files[indConfiguration] * sizeof(float));
		pg_Mesh_Translation_Z[indConfiguration] = new float[pg_nb_Mesh_files[indConfiguration]];
		memset((char*)pg_Mesh_Translation_Z[indConfiguration], 0, pg_nb_Mesh_files[indConfiguration] * sizeof(float));
		pg_Mesh_Translation_Ini_X[indConfiguration] = new float[pg_nb_Mesh_files[indConfiguration]];
		memset((char*)pg_Mesh_Translation_Ini_X[indConfiguration], 0, pg_nb_Mesh_files[indConfiguration] * sizeof(float));
		pg_Mesh_Translation_Ini_Y[indConfiguration] = new float[pg_nb_Mesh_files[indConfiguration]];
		memset((char*)pg_Mesh_Translation_Ini_Y[indConfiguration], 0, pg_nb_Mesh_files[indConfiguration] * sizeof(float));
		pg_Mesh_Translation_Ini_Z[indConfiguration] = new float[pg_nb_Mesh_files[indConfiguration]];
		memset((char*)pg_Mesh_Translation_Ini_Z[indConfiguration], 0, pg_nb_Mesh_files[indConfiguration] * sizeof(float));
		pg_Mesh_Motion_X[indConfiguration] = new float[pg_nb_Mesh_files[indConfiguration]];
		memset((char*)pg_Mesh_Motion_X[indConfiguration], 0, pg_nb_Mesh_files[indConfiguration] * sizeof(float));
		pg_Mesh_Motion_Y[indConfiguration] = new float[pg_nb_Mesh_files[indConfiguration]];
		memset((char*)pg_Mesh_Motion_Y[indConfiguration], 0, pg_nb_Mesh_files[indConfiguration] * sizeof(float));
		pg_Mesh_Motion_Z[indConfiguration] = new float[pg_nb_Mesh_files[indConfiguration]];
		memset((char*)pg_Mesh_Motion_Z[indConfiguration], 0, pg_nb_Mesh_files[indConfiguration] * sizeof(float));
		pg_Mesh_TextureRank[indConfiguration] = new int[pg_nb_Mesh_files[indConfiguration]];
		memset((char*)pg_Mesh_TextureRank[indConfiguration], 0, pg_nb_Mesh_files[indConfiguration] * sizeof(int));
		pg_Mesh_Colors[indConfiguration] = new float* [pg_nb_Mesh_files[indConfiguration]];
		for (int indFile = 0; indFile < pg_nb_Mesh_files[indConfiguration]; indFile++) {
			pg_Mesh_Colors[indConfiguration][indFile] = new float[4];
			pg_Mesh_Colors[indConfiguration][indFile][0] = 1.f;
			pg_Mesh_Colors[indConfiguration][indFile][1] = 1.f;
			pg_Mesh_Colors[indConfiguration][indFile][2] = 1.f;
			pg_Mesh_Colors[indConfiguration][indFile][3] = 1.f;
		}
		Mesh_texture_rectangle[indConfiguration] = new GLuint[pg_nb_Mesh_files[indConfiguration]];
		memset((char*)Mesh_texture_rectangle[indConfiguration], 0, pg_nb_Mesh_files[indConfiguration] * sizeof(GLuint));

		for (int indMeshFile = 0; indMeshFile < pg_nb_Mesh_files[indConfiguration]; indMeshFile++) {
			std::getline(scenarioFin, line);
			stringstreamStoreLine(&sstream, &line);
			sstream >> ID; // string mesh
			sstream >> pg_Mesh_fileNames[indConfiguration][indMeshFile]; // file name
			// full path is not given, look in default local path
			if (pg_Mesh_fileNames[indConfiguration][indMeshFile].find(':') == std::string::npos) {
				pg_Mesh_fileNames[indConfiguration][indMeshFile]
					= "Data/" + project_name + "-data/meshes/"
					+ pg_Mesh_fileNames[indConfiguration][indMeshFile];
			}

			// image initial geometry
			sstream >> pg_Mesh_Scale[indConfiguration][indMeshFile];
			sstream >> pg_Mesh_Translation_X[indConfiguration][indMeshFile];
			sstream >> pg_Mesh_Translation_Y[indConfiguration][indMeshFile];
			sstream >> pg_Mesh_Translation_Z[indConfiguration][indMeshFile];
			sstream >> pg_Mesh_Rotation_angle[indConfiguration][indMeshFile];
			sstream >> pg_Mesh_Rotation_X[indConfiguration][indMeshFile];
			sstream >> pg_Mesh_Rotation_Y[indConfiguration][indMeshFile];
			sstream >> pg_Mesh_Rotation_Z[indConfiguration][indMeshFile];
			if (pg_Mesh_Rotation_X[indConfiguration][indMeshFile] == 0 && pg_Mesh_Rotation_Y[indConfiguration][indMeshFile] == 0 && pg_Mesh_Rotation_Z[indConfiguration][indMeshFile] == 0) {
				sprintf(ErrorStr, "Error: incorrect mesh %s configuration: rotation with Null axix ", pg_Mesh_fileNames[indConfiguration][indMeshFile].c_str()); ReportError(ErrorStr); throw 100;
			}
			sstream >> pg_Mesh_Motion_X[indConfiguration][indMeshFile];
			sstream >> pg_Mesh_Motion_Y[indConfiguration][indMeshFile];
			sstream >> pg_Mesh_Motion_Z[indConfiguration][indMeshFile];
			pg_Mesh_Translation_Ini_X[indConfiguration][indMeshFile] = pg_Mesh_Translation_X[indConfiguration][indMeshFile];
			pg_Mesh_Translation_Ini_Y[indConfiguration][indMeshFile] = pg_Mesh_Translation_Y[indConfiguration][indMeshFile];
			pg_Mesh_Translation_Ini_Z[indConfiguration][indMeshFile] = pg_Mesh_Translation_Z[indConfiguration][indMeshFile];
			pg_Mesh_Rotation_Ini_X[indConfiguration][indMeshFile] = pg_Mesh_Rotation_X[indConfiguration][indMeshFile];
			pg_Mesh_Rotation_Ini_Y[indConfiguration][indMeshFile] = pg_Mesh_Rotation_Y[indConfiguration][indMeshFile];
			pg_Mesh_Rotation_Ini_Z[indConfiguration][indMeshFile] = pg_Mesh_Rotation_Z[indConfiguration][indMeshFile];
			sstream >> ID;
			if (ID.compare("nat") == 0) {
				pg_Mesh_Colors[indConfiguration][indMeshFile][0] = 1.f;
				pg_Mesh_Colors[indConfiguration][indMeshFile][1] = 1.f;
				pg_Mesh_Colors[indConfiguration][indMeshFile][2] = 1.f;
				pg_Mesh_Colors[indConfiguration][indMeshFile][3] = 0.f;
			}
			else if (ID.compare("white") == 0) {
				pg_Mesh_Colors[indConfiguration][indMeshFile][0] = 1.f;
				pg_Mesh_Colors[indConfiguration][indMeshFile][1] = 1.f;
				pg_Mesh_Colors[indConfiguration][indMeshFile][2] = 1.f;
				pg_Mesh_Colors[indConfiguration][indMeshFile][3] = 1.f;
			}
			else if (ID.compare("red") == 0) {
				pg_Mesh_Colors[indConfiguration][indMeshFile][0] = 1.f;
				pg_Mesh_Colors[indConfiguration][indMeshFile][1] = 0.f;
				pg_Mesh_Colors[indConfiguration][indMeshFile][2] = 0.f;
				pg_Mesh_Colors[indConfiguration][indMeshFile][3] = 1.f;
			}
			else if (ID.compare("green") == 0) {
				pg_Mesh_Colors[indConfiguration][indMeshFile][0] = 0.f;
				pg_Mesh_Colors[indConfiguration][indMeshFile][1] = 1.f;
				pg_Mesh_Colors[indConfiguration][indMeshFile][2] = 0.f;
				pg_Mesh_Colors[indConfiguration][indMeshFile][3] = 1.f;
			}
			else if (ID.compare("blue") == 0) {
				pg_Mesh_Colors[indConfiguration][indMeshFile][0] = 0.f;
				pg_Mesh_Colors[indConfiguration][indMeshFile][1] = 0.f;
				pg_Mesh_Colors[indConfiguration][indMeshFile][2] = 1.f;
				pg_Mesh_Colors[indConfiguration][indMeshFile][3] = 1.f;
			}
			else if (ID.compare("cyan") == 0) {
				pg_Mesh_Colors[indConfiguration][indMeshFile][0] = 0.f;
				pg_Mesh_Colors[indConfiguration][indMeshFile][1] = 1.f;
				pg_Mesh_Colors[indConfiguration][indMeshFile][2] = 1.f;
				pg_Mesh_Colors[indConfiguration][indMeshFile][3] = 1.f;
			}
			else if (ID.compare("magenta") == 0) {
				pg_Mesh_Colors[indConfiguration][indMeshFile][0] = 1.f;
				pg_Mesh_Colors[indConfiguration][indMeshFile][1] = 0.f;
				pg_Mesh_Colors[indConfiguration][indMeshFile][2] = 1.f;
				pg_Mesh_Colors[indConfiguration][indMeshFile][3] = 1.f;
			}
			else if (ID.compare("yellow") == 0) {
				pg_Mesh_Colors[indConfiguration][indMeshFile][0] = 1.f;
				pg_Mesh_Colors[indConfiguration][indMeshFile][1] = 1.f;
				pg_Mesh_Colors[indConfiguration][indMeshFile][2] = 0.f;
				pg_Mesh_Colors[indConfiguration][indMeshFile][3] = 1.f;
			}
			else if (ID.compare("black") == 0) {
				pg_Mesh_Colors[indConfiguration][indMeshFile][0] = 0.f;
				pg_Mesh_Colors[indConfiguration][indMeshFile][1] = 0.f;
				pg_Mesh_Colors[indConfiguration][indMeshFile][2] = 0.f;
				pg_Mesh_Colors[indConfiguration][indMeshFile][3] = 1.f;
			}
			else {
				sprintf(ErrorStr, "Error: incorrect configuration file Mesh color \"%s\" (nat, white, cyan, yellow, magenta, red, blue, or greeen expected)", ID.c_str()); ReportError(ErrorStr); throw 100;
			}
			sstream >> pg_Mesh_TextureRank[indConfiguration][indMeshFile];
			//printf("Mesh #%d scale %.2f translation (%.2f,%.2f,%.2f), rotation %.2f\n",
			//	indMeshFile, pg_Mesh_Scale[indMeshFile], pg_Mesh_Translation_X[indMeshFile],
			//	pg_Mesh_Translation_Y[indMeshFile], pg_Mesh_Translation_Z[indMeshFile],
			//	pg_Mesh_Rotation_angle[indMeshFile]);
			// the rank of the mesh textures applied to this mesh
#if defined(var_MmeShanghai_brokenGlass)
			if (ScenarioVarConfigurations[_MmeShanghai_brokenGlass][indConfiguration]) {
				sstream >> pg_MmeShanghai_NbMeshSubParts[indConfiguration][indMeshFile];
				pg_MmeShanghai_MeshSubPart_FileNames[indConfiguration][indMeshFile] = new string[pg_MmeShanghai_NbMeshSubParts[indConfiguration][indMeshFile]];
				pg_MmeShanghai_MeshSubParts[indConfiguration][indMeshFile] = new bool* [pg_MmeShanghai_NbMeshSubParts[indConfiguration][indMeshFile]];
				for (int indPart = 0; indPart < pg_MmeShanghai_NbMeshSubParts[indConfiguration][indMeshFile]; indPart++) {
					sstream >> pg_MmeShanghai_MeshSubPart_FileNames[indConfiguration][indMeshFile][indPart];
					pg_MmeShanghai_MeshSubPart_FileNames[indConfiguration][indMeshFile][indPart]
						= "Data/" + project_name + "-data/meshes/"
						+ pg_MmeShanghai_MeshSubPart_FileNames[indConfiguration][indMeshFile][indPart];
					pg_MmeShanghai_MeshSubParts[indConfiguration][indMeshFile][indPart] = NULL;
				}
			}
#endif
		}

		// /meshes
		std::getline(scenarioFin, line);
		stringstreamStoreLine(&sstream, &line);
		sstream >> ID; // string /meshes
		if (ID.compare("/meshes") != 0) {
			sprintf(ErrorStr, "Error: incorrect configuration file expected string \"/meshes\" not found! (instead \"%s\")", ID.c_str()); ReportError(ErrorStr); throw 100;
		}
	}
#endif
}

void ParseScenarioTextures(std::ifstream& scenarioFin, int indConfiguration) {
	std::stringstream  sstream;
	string line;
	string ID;
	string temp;

	////////////////////////////	
	////// TEXTURES
	// the textures are loaded inside the GPU and diplayed path by path

	// Number of textures
	std::getline(scenarioFin, line);
	stringstreamStoreLine(&sstream, &line);
	sstream >> ID; // string textures
	if (ID.compare("textures") != 0) {
		sprintf(ErrorStr, "Error: incorrect configuration file expected string \"textures\" not found! (instead \"%s\")", ID.c_str()); ReportError(ErrorStr); throw 100;
	}

	// number of obj files in the configuration file
	sstream >> pg_nb_Texture_files[indConfiguration];

	pg_Texture_fileNames[indConfiguration] = new string[pg_nb_Texture_files[indConfiguration]];
	pg_Texture_fileNamesSuffix[indConfiguration] = new string[pg_nb_Texture_files[indConfiguration]];
	pg_Texture_usages[indConfiguration] = new pg_Texture_Usages[pg_nb_Texture_files[indConfiguration]];
	for (int indFile = 0; indFile < pg_nb_Texture_files[indConfiguration]; indFile++) {
		pg_Texture_usages[indConfiguration][indFile] = Texture_brush;
	}

	pg_Texture_Size_X[indConfiguration] = new int[pg_nb_Texture_files[indConfiguration]];
	pg_Texture_Size_Y[indConfiguration] = new int[pg_nb_Texture_files[indConfiguration]];
	pg_Texture_Rank[indConfiguration] = new int[pg_nb_Texture_files[indConfiguration]];
	pg_Texture_Dimension[indConfiguration] = new int[pg_nb_Texture_files[indConfiguration]];
	pg_Texture_Nb_Layers[indConfiguration] = new int[pg_nb_Texture_files[indConfiguration]];
	pg_Texture_Is_Rectangle[indConfiguration] = new bool[pg_nb_Texture_files[indConfiguration]];
	pg_Texture_Invert[indConfiguration] = new bool[pg_nb_Texture_files[indConfiguration]];
	pg_Texture_Nb_Bytes_per_Pixel[indConfiguration] = new int[pg_nb_Texture_files[indConfiguration]];
	pg_Texture_texID[indConfiguration] = new GLuint[pg_nb_Texture_files[indConfiguration]];
	memset((char*)pg_Texture_Size_X[indConfiguration], 0, pg_nb_Texture_files[indConfiguration] * sizeof(int));
	memset((char*)pg_Texture_Size_Y[indConfiguration], 0, pg_nb_Texture_files[indConfiguration] * sizeof(int));
	memset((char*)pg_Texture_Rank[indConfiguration], 0, pg_nb_Texture_files[indConfiguration] * sizeof(int));
	memset((char*)pg_Texture_Dimension[indConfiguration], 0, pg_nb_Texture_files[indConfiguration] * sizeof(int));
	memset((char*)pg_Texture_Is_Rectangle[indConfiguration], 0, pg_nb_Texture_files[indConfiguration] * sizeof(bool));
	memset((char*)pg_Texture_Invert[indConfiguration], 0, pg_nb_Texture_files[indConfiguration] * sizeof(bool));
	memset((char*)pg_Texture_Nb_Bytes_per_Pixel[indConfiguration], 0, pg_nb_Texture_files[indConfiguration] * sizeof(int));
	memset((char*)pg_Texture_texID[indConfiguration], 0, pg_nb_Texture_files[indConfiguration] * sizeof(GLuint));

	for (int indTextureFile = 0; indTextureFile < pg_nb_Texture_files[indConfiguration]; indTextureFile++) {
		std::getline(scenarioFin, line);
		stringstreamStoreLine(&sstream, &line);
		sstream >> ID; // string texture
		sstream >> ID; // file name
		if (ID.find(':') == std::string::npos) {
			pg_Texture_fileNames[indConfiguration][indTextureFile]
				= "Data/" + project_name + "-data/textures/" + ID;
		}
		else {
			pg_Texture_fileNames[indConfiguration][indTextureFile] = ID;
		}
		sstream >> pg_Texture_fileNamesSuffix[indConfiguration][indTextureFile]; // file suffix

		// usage
		sstream >> ID;
		//printf("Usage %s: (%s)\n", pg_Texture_fileNames[indConfiguration][indTextureFile].c_str(), ID.c_str());
		if (ID.compare("master_mask") == 0) {
			pg_Texture_usages[indConfiguration][indTextureFile] = Texture_master_mask;
		}
		else if (ID.compare("mesh") == 0) {
			pg_Texture_usages[indConfiguration][indTextureFile] = Texture_mesh;
		}
		else if (ID.compare("sensor") == 0) {
			pg_Texture_usages[indConfiguration][indTextureFile] = Texture_sensor;
		}
		else if (ID.compare("logo") == 0) {
			pg_Texture_usages[indConfiguration][indTextureFile] = Texture_logo;
		}
		else if (ID.compare("brush") == 0) {
			pg_Texture_usages[indConfiguration][indTextureFile] = Texture_brush;
		}
		else if (ID.compare("noise") == 0) {
			pg_Texture_usages[indConfiguration][indTextureFile] = Texture_noise;
		}
		else if (ID.compare("curve_particle") == 0) {
			pg_Texture_usages[indConfiguration][indTextureFile] = Texture_curve_particle;
		}
		else if (ID.compare("splat_particle") == 0) {
			pg_Texture_usages[indConfiguration][indTextureFile] = Texture_splat_particle;
		}
		else if (ID.compare("part_init") == 0) {
			pg_Texture_usages[indConfiguration][indTextureFile] = Texture_part_init;
		}
		else if (ID.compare("part_acc") == 0) {
			pg_Texture_usages[indConfiguration][indTextureFile] = Texture_part_acc;
		}
		else if (ID.compare("pixel_acc") == 0) {
			pg_Texture_usages[indConfiguration][indTextureFile] = Texture_pixel_acc;
		}
		else if (ID.compare("repop_density") == 0) {
			pg_Texture_usages[indConfiguration][indTextureFile] = Texture_repop_density;
		}
		else if (ID.compare("multilayer_master_mask") == 0) {
			pg_Texture_usages[indConfiguration][indTextureFile] = Texture_multilayer_master_mask;
		}
		else {
			sprintf(ErrorStr, "Error: incorrect configuration file Texture usage \"%s\"\n", ID.c_str()); ReportError(ErrorStr); throw 100;
		}
		// rank of the texture (used in particular for meshes)
		sstream >> pg_Texture_Rank[indConfiguration][indTextureFile];
		// dimension (2 or 3)
		sstream >> pg_Texture_Dimension[indConfiguration][indTextureFile];
		if (pg_Texture_Dimension[indConfiguration][indTextureFile] != 2
			&& pg_Texture_Dimension[indConfiguration][indTextureFile] != 3) {
			sprintf(ErrorStr, "Error: 2D or 3D texture dimension expected, not %d for texture %d (%s)\n", pg_Texture_Dimension[indConfiguration][indTextureFile], indTextureFile, pg_Texture_fileNames[indConfiguration][indTextureFile].c_str()); ReportError(ErrorStr); throw 100;
		}
		// number of piled textures in case of 3D texture or tif format
		sstream >> pg_Texture_Nb_Layers[indConfiguration][indTextureFile];
		if (pg_Texture_usages[indConfiguration][indTextureFile] == Texture_brush) {
			nb_pen_brushes[indConfiguration] = pg_Texture_Nb_Layers[indConfiguration][indTextureFile];
		}
		if (pg_Texture_usages[indConfiguration][indTextureFile] == Texture_multilayer_master_mask) {
			nb_layers_master_mask[indConfiguration] = pg_Texture_Nb_Layers[indConfiguration][indTextureFile];
		}

		// image initial geometry
		sstream >> pg_Texture_Size_X[indConfiguration][indTextureFile];
		sstream >> pg_Texture_Size_Y[indConfiguration][indTextureFile];
		if (pg_Texture_usages[indConfiguration][indTextureFile] == Texture_master_mask &&
			(pg_Texture_Size_X[indConfiguration][indTextureFile] < workingWindow_width || pg_Texture_Size_Y[indConfiguration][indTextureFile] < window_height)) {
			sprintf(ErrorStr, "Error: master mask texture should be minimlally %dx%d (%dx%d)\n", window_width, window_height,
				pg_Texture_Size_X[indConfiguration][indTextureFile], pg_Texture_Size_Y[indConfiguration][indTextureFile]); ReportError(ErrorStr); throw 100;
		}
		if (pg_Texture_usages[indConfiguration][indTextureFile] == Texture_multilayer_master_mask &&
			(pg_Texture_Size_X[indConfiguration][indTextureFile] < workingWindow_width || pg_Texture_Size_Y[indConfiguration][indTextureFile] < window_height)) {
			sprintf(ErrorStr, "Error: multilayer master mask texture should be minimlally %dx%d (%dx%d)\n", window_width, window_height,
				pg_Texture_Size_X[indConfiguration][indTextureFile], pg_Texture_Size_Y[indConfiguration][indTextureFile]); ReportError(ErrorStr); throw 100;
		}
		if (pg_Texture_usages[indConfiguration][indTextureFile] == Texture_noise &&
			(pg_Texture_Size_X[indConfiguration][indTextureFile] < workingWindow_width_powerOf2 || pg_Texture_Size_Y[indConfiguration][indTextureFile] < window_height_powerOf2)) {
			sprintf(ErrorStr, "Error: noise texture should be minimlally %dx%d (%dx%d)\n", workingWindow_width_powerOf2, window_height_powerOf2,
				pg_Texture_Size_X[indConfiguration][indTextureFile], pg_Texture_Size_Y[indConfiguration][indTextureFile]); ReportError(ErrorStr); throw 100;
		}
		if (pg_Texture_usages[indConfiguration][indTextureFile] == Texture_part_init &&
			(pg_Texture_Size_X[indConfiguration][indTextureFile] < workingWindow_width || pg_Texture_Size_Y[indConfiguration][indTextureFile] < window_height)) {
			sprintf(ErrorStr, "Error: particle initialization texture should be minimlally %dx%d (%dx%d)\n", workingWindow_width, window_height,
				pg_Texture_Size_X[indConfiguration][indTextureFile], pg_Texture_Size_Y[indConfiguration][indTextureFile]); ReportError(ErrorStr); throw 100;
		}
		if (pg_Texture_usages[indConfiguration][indTextureFile] == Texture_part_acc &&
			(pg_Texture_Size_X[indConfiguration][indTextureFile] < workingWindow_width || pg_Texture_Size_Y[indConfiguration][indTextureFile] < window_height)) {
			sprintf(ErrorStr, "Error: particle acceleration texture should be minimlally %dx%d (%dx%d)\n", workingWindow_width, window_height,
				pg_Texture_Size_X[indConfiguration][indTextureFile], pg_Texture_Size_Y[indConfiguration][indTextureFile]); ReportError(ErrorStr); throw 100;
		}
		if (pg_Texture_usages[indConfiguration][indTextureFile] == Texture_pixel_acc &&
			(pg_Texture_Size_X[indConfiguration][indTextureFile] < workingWindow_width || pg_Texture_Size_Y[indConfiguration][indTextureFile] < window_height)) {
			sprintf(ErrorStr, "Error: pixel acceleration texture should be minimlally %dx%d (%dx%d)\n", workingWindow_width, window_height,
				pg_Texture_Size_X[indConfiguration][indTextureFile], pg_Texture_Size_Y[indConfiguration][indTextureFile]); ReportError(ErrorStr); throw 100;
		}
		if (pg_Texture_usages[indConfiguration][indTextureFile] == Texture_repop_density &&
			(pg_Texture_Size_X[indConfiguration][indTextureFile] < workingWindow_width || pg_Texture_Size_Y[indConfiguration][indTextureFile] < window_height)) {
			sprintf(ErrorStr, "Error:  repopulation density texture should be minimlally %dx%d (%dx%d)\n", workingWindow_width, window_height,
				pg_Texture_Size_X[indConfiguration][indTextureFile], pg_Texture_Size_Y[indConfiguration][indTextureFile]); ReportError(ErrorStr); throw 100;
		}

		// image color depth
		sstream >> pg_Texture_Nb_Bytes_per_Pixel[indConfiguration][indTextureFile];

		// booleans invert & is rectangle
		sstream >> ID;
		if (ID.compare("true") == 0 || ID.compare("TRUE") == 0) {
			pg_Texture_Is_Rectangle[indConfiguration][indTextureFile] = true;
		}
		else if (ID.compare("false") == 0 || ID.compare("FALSE") == 0) {
			pg_Texture_Is_Rectangle[indConfiguration][indTextureFile] = false;
		}
		else {
			sprintf(ErrorStr, "Error: incorrect boolean for Texture rectangle \"%s\" (true or false expected)\n", ID.c_str()); ReportError(ErrorStr); throw 100;
		}
		sstream >> ID;
		if (ID.compare("true") == 0 || ID.compare("TRUE") == 0) {
			pg_Texture_Invert[indConfiguration][indTextureFile] = true;
		}
		else if (ID.compare("false") == 0 || ID.compare("FALSE") == 0) {
			pg_Texture_Invert[indConfiguration][indTextureFile] = false;
		}
		else {
			sprintf(ErrorStr, "Error: incorrect boolean for Texture invert \"%s\" (true or false expected)\n", ID.c_str()); ReportError(ErrorStr); throw 100;
		}
		//printf("Texture #%d size (%d,%d), rank %d, usage %s\n",
		//	indTextureFile, pg_Texture_Size_X[indTextureFile], pg_Texture_Size_Y[indTextureFile],
		//	pg_Texture_Rank[indTextureFile], pg_Texture_usages[indTextureFile].c_str());
	}

	// /textures
	std::getline(scenarioFin, line);
	stringstreamStoreLine(&sstream, &line);
	sstream >> ID; // string /textures
	if (ID.compare("/textures") != 0) {
		sprintf(ErrorStr, "Error: incorrect configuration file expected string \"/textures\" not found! (instead \"%s\")\n", ID.c_str()); ReportError(ErrorStr); throw 100;
	}
}

void ParseScenarioColorPalettes(std::ifstream& scenarioFin, int indConfiguration) {
	std::stringstream  sstream;
	string line;
	string ID;
	string temp;

	////////////////////////////
	////// PALETTE COLORSS
	nb_pen_palette_colors[indConfiguration] = 0;
	// Number of palettes
	std::getline(scenarioFin, line);
	stringstreamStoreLine(&sstream, &line);
	sstream >> ID; // string palette_colors
	if (ID.compare("palette_colors") != 0) {
		sprintf(ErrorStr, "Error: incorrect configuration file expected string \"palette_colors\" not found! (instead \"%s\")\n", ID.c_str()); ReportError(ErrorStr); throw 100;
	}
	sstream >> nb_pen_palette_colors[indConfiguration];

	// std::cout << "line: " << line << "\n";
	// std::cout << "Nb palettes : " << nb_pen_palette_colors[indConfiguration] <<  "\n";

	pen_palette_colors_values[indConfiguration] = new float* [nb_pen_palette_colors[indConfiguration]];
	for (int indColorPalette = 0; indColorPalette < nb_pen_palette_colors[indConfiguration]; indColorPalette++) {
		pen_palette_colors_values[indConfiguration][indColorPalette] = new float[9];

		std::getline(scenarioFin, line);
		// std::cout << "scene: " << line << "\n";
		stringstreamStoreLine(&sstream, &line);
		sstream >> ID; // string palette
		if (ID.compare("palette") != 0) {
			sprintf(ErrorStr, "Error: incorrect configuration file expected string \"palette\" not found! (instead \"%s\")\n", ID.c_str()); ReportError(ErrorStr); throw 100;
		}
		sstream >> ID; // palette ID
		for (int indColorBandpass = 0; indColorBandpass < 3; indColorBandpass++) {
			for (int indColorChannel = 0; indColorChannel < 3; indColorChannel++) {
				sstream >> pen_palette_colors_values[indConfiguration][indColorPalette][indColorBandpass * 3 + indColorChannel];
				pen_palette_colors_values[indConfiguration][indColorPalette][indColorBandpass * 3 + indColorChannel] /= 255.f;
			}
		}
		pen_palette_colors_names[indConfiguration].push_back(ID.c_str());
		// std::cout << "palettes : " << indPalette << " " <<  pen_palettes_names[indPalette] << " " << pen_palettes_values[indPalette] << "\n";
	}
	// /palettes
	std::getline(scenarioFin, line);
	stringstreamStoreLine(&sstream, &line);
	sstream >> ID; // string /palette_colors
	if (ID.compare("/palette_colors") != 0) {
		sprintf(ErrorStr, "Error: incorrect configuration file expected string \"/palette_colors\" not found! (instead \"%s\")\n", ID.c_str()); ReportError(ErrorStr); throw 100;
	}
}

void ParseScenarioColorPresets(std::ifstream& scenarioFin, int indConfiguration) {
	std::stringstream  sstream;
	string line;
	string ID;
	string temp;

	////////////////////////////
	////// PALETTES
	nb_pen_colorPresets[indConfiguration] = 0;
	// Number of palettes
	std::getline(scenarioFin, line);
	stringstreamStoreLine(&sstream, &line);
	sstream >> ID; // string color_presets
	if (ID.compare("color_presets") != 0) {
		sprintf(ErrorStr, "Error: incorrect configuration file expected string \"color_presets\" not found! (instead \"%s\")\n", ID.c_str()); ReportError(ErrorStr); throw 100;
	}
	sstream >> nb_pen_colorPresets[indConfiguration];

	// std::cout << "line: " << line << "\n";
	// std::cout << "Nb palettes : " << nb_pen_colorPresets[indConfiguration] <<  "\n";

	pg_colorPreset_values[indConfiguration] = new Color[nb_pen_colorPresets[indConfiguration]];
	for (int indPalette = 0; indPalette < nb_pen_colorPresets[indConfiguration]; indPalette++) {
		std::getline(scenarioFin, line);
		// std::cout << "scene: " << line << "\n";
		stringstreamStoreLine(&sstream, &line);
		sstream >> ID; // string preset
		if (ID.compare("preset") != 0) {
			sprintf(ErrorStr, "Error: incorrect configuration file expected string \"preset\" not found! (instead \"%s\")\n", ID.c_str()); ReportError(ErrorStr); throw 100;
		}
		sstream >> ID; // preset ID
		sstream >> pg_colorPreset_values[indConfiguration][indPalette].color;           // hue
		sstream >> pg_colorPreset_values[indConfiguration][indPalette].grey;            // color to white interpolation
		sstream >> pg_colorPreset_values[indConfiguration][indPalette].alpha;           // transparency, translated into to black interpolation for RGB conversion wo transparency
		pen_colorPresets_names[indConfiguration].push_back(ID.c_str());
		//std::cout << "palettes : " << indPalette << " " <<  pen_colorPresets_names[indPalette] << " " << pg_colorPreset_values[indPalette] << "\n";
	}
	// /palettes
	std::getline(scenarioFin, line);
	stringstreamStoreLine(&sstream, &line);
	sstream >> ID; // string /palettes
	if (ID.compare("/color_presets") != 0) {
		sprintf(ErrorStr, "Error: incorrect configuration file expected string \"/color_presets\" not found! (instead \"%s\")\n", ID.c_str()); ReportError(ErrorStr); throw 100;
	}
}

void ParseScenarioLights(std::ifstream & scenarioFin, int indConfiguration) {
	std::stringstream  sstream;
	string line;
	string ID;
	string temp;

#if defined(PG_LIGHTS_CONTROL_IN_PG) || defined(PG_LIGHTS_CONTROL_IN_PYTHON)
	////////////////////////////
	////// LIGHTS
	pg_nb_lights[indConfiguration] = 0;
	// Number of palettes
	std::getline(scenarioFin, line);
	stringstreamStoreLine(&sstream, &line);
	sstream >> ID; // string color_presets
	if (ID.compare("lights") != 0) {
		sprintf(ErrorStr, "Error: incorrect configuration file expected string \"lights\" not found! (instead \"%s\")\n", ID.c_str()); ReportError(ErrorStr); throw 100;
	}
	sstream >> pg_nb_lights[indConfiguration];
	pg_lights[indConfiguration] = new Light[pg_nb_lights[indConfiguration]];
	int* indexes_in_group = new int[pg_nb_lights[indConfiguration]];
	for (int ind = 0; ind < pg_nb_lights[indConfiguration]; ind++) {
		indexes_in_group[ind] = 0;
	}

	// std::cout << "line: " << line << "\n";
	// std::cout << "Nb lights : " << pg_nb_lights <<  "\n";

	// lights presets
	pg_nb_light_groups[indConfiguration] = 0;
	for (int indLight = 0; indLight < pg_nb_lights[indConfiguration]; indLight++) {
		string a_light_name;
		int a_light_group;
		int a_light_port;
		int a_light_address;
		int a_light_channels;
		string a_light_red;
		string a_light_green;
		string a_light_blue;
		string a_light_grey;
		string a_light_dimmer;
		string a_light_strobe;
		string a_light_zoom;
		string a_light_pan;
		string a_light_tilt;
		string a_light_hue;

		std::getline(scenarioFin, line);
		// std::cout << "scene: " << line << "\n";
		stringstreamStoreLine(&sstream, &line);
		sstream >> ID; // "light"
		sstream >> a_light_name; // light ID
		sstream >> a_light_group;
		//#ifdef PG_NB_LIGHTS_GROUPS_IN_PG_VARS
		//		if (a_light_group > PG_NB_LIGHTS_GROUPS_IN_PG_VARS) {
		//			sprintf(ErrorStr, "Error: incorrect configuration file light group of light #%d should be <= %d\n", indLight, PG_NB_LIGHTS_GROUPS_IN_PG_VARS); ReportError(ErrorStr); throw 100;
		//		}
		//#endif
		if (a_light_group < 1) {
			sprintf(ErrorStr, "Error: incorrect configuration file light group (%d) should be a value >= 1 for light %d\n", a_light_group, indLight); ReportError(ErrorStr); throw 100;
		}
		pg_nb_light_groups[indConfiguration] = max(pg_nb_light_groups[indConfiguration], a_light_group);

		sstream >> a_light_port;
		if (a_light_port != 1 && a_light_port != 2) {
			sprintf(ErrorStr, "Error: incorrect configuration file light port should be equal to 1 or 2 not %d for light %d\n", a_light_port, indLight); ReportError(ErrorStr); throw 100;
		}
		sstream >> a_light_address;
		sstream >> a_light_channels;
		sstream >> a_light_red;
		sstream >> a_light_green;
		sstream >> a_light_blue;
		sstream >> a_light_grey;
		sstream >> a_light_dimmer;
		sstream >> a_light_strobe;
		sstream >> a_light_zoom;
		sstream >> a_light_pan;
		sstream >> a_light_tilt;
		sstream >> a_light_hue;
		int an_index_in_group = 0;
		if (a_light_group < pg_nb_lights[indConfiguration]) {
			an_index_in_group = indexes_in_group[a_light_group];
			indexes_in_group[a_light_group] += 1;
		}
		pg_lights[indConfiguration][indLight].set_light_values(a_light_name, a_light_group, an_index_in_group, a_light_port, a_light_address, a_light_channels,
			a_light_red, a_light_green, a_light_blue, a_light_grey, a_light_dimmer, a_light_strobe, a_light_zoom, a_light_pan, a_light_tilt, a_light_hue);
		//std::cout << "light : " << indLight << " id " <<  a_light_names << " port " 
		//	<< a_light_port << " rank " << a_light_group << " add " << a_light_address << " ch " << a_light_channels << " r "
		//	<< a_light_red << " g " << a_light_green << " b " << a_light_blue 
		//	<< " dimm " << a_light_dimmer <<" strobe " << a_light_strobe << "\n";
	}

	//#ifdef PG_NB_LIGHTS_GROUPS_IN_PG_VARS
	//	if (pg_nb_light_groups != PG_NB_LIGHTS_GROUPS_IN_PG_VARS) {
	//		sprintf(ErrorStr, "Error: incorrect configuration file expected %d lights instead of %d\n", PG_NB_LIGHTS_GROUPS_IN_PG_VARS, pg_nb_lights); ReportError(ErrorStr); throw 100;
	//	}
	//#endif
		// classes of light groups initialized to default values
	if (pg_nb_light_groups[indConfiguration] > 0) {
		pg_light_groups[indConfiguration] = new LightGroup[pg_nb_light_groups[indConfiguration]];
		for (int ind = 0; ind < pg_nb_light_groups[indConfiguration]; ind++) {
			pg_light_groups[indConfiguration][ind].set_group_no(ind + 1);
			for (int ind_light = 0; ind_light < pg_nb_lights[indConfiguration]; ind_light++) {
				if (pg_lights[indConfiguration][ind_light].light_group == ind + 1) {
					pg_light_groups[indConfiguration][ind].set_group_id(pg_lights[indConfiguration][ind_light].light_name);
					//printf("group ind %d id %s\n", ind , pg_light_groups[ind].get_group_id().c_str());
					break;
				}
			}
		}
	}
	// Iterate over an unordered_map using range based for loop
	// builds pg_inverse_light_param_hashMap from pg_light_param_hashMap
	// by exchanging keys and values
	for (const auto& myPair : pg_light_param_hashMap) {
		pg_inverse_light_param_hashMap[myPair.second] = myPair.first;
	}

	// /lights
	std::getline(scenarioFin, line);
	stringstreamStoreLine(&sstream, &line);
	sstream >> ID; // string /palettes
	if (ID.compare("/lights") != 0) {
		sprintf(ErrorStr, "Error: incorrect configuration file expected string \"/lights\" not found! (instead \"%s\")\n", ID.c_str()); ReportError(ErrorStr); throw 100;
	}
#endif
}

void parseScenarioFile(std::ifstream& scenarioFin, int indConfiguration) {

	////////////////////////////
	////// CONFIGURATION VARIABLE INITIALIZATION
	string line;
	string ID;
	string temp;
	string temp2;
	string temp3;
	std::stringstream  sstream;
	int indVar;

	// RANK
	std::getline(scenarioFin, line);
	// VERBATIM
	std::getline(scenarioFin, line);
	// TYPE
	std::getline(scenarioFin, line);
	// ID
	std::getline(scenarioFin, line);
	stringstreamStoreLine(&sstream, &line);
	int nb_vars = 0;
	for (int indP = 0; ((indP < _MaxInterpVarIDs + 1) && !sstream.eof()); indP++) {
		sstream >> temp;
		// first item is "ID"
		if (indP > 0) {
			// the index of the current scenario var into the global list of variables cumulated from all the scenarios
			indVar = pg_varID_to_rank(temp, indConfiguration);
			ConfigScenarioVarRank[indP - 1][indConfiguration] = indVar;
			ConfigScenarioVarIDs[indP - 1][indConfiguration] = temp;
			if (indVar == -1) {
				sprintf(ErrorStr, "Error: scenario variable %s (index %d) of configuration %d not found in the variable list generated by python generator\n", temp.c_str(), indP, indConfiguration); ReportError(ErrorStr); throw 50;
			}
			if (!ScenarioVarConfigurations[indVar][indConfiguration]) {
				sprintf(ErrorStr, "Error: scenario variable %s (index %d) considered as not belonging to the scenario of configuration %d by python generator\n", temp.c_str(), indP, indConfiguration); ReportError(ErrorStr); throw 50;
			}
			nb_vars++;
		}
	}

	// stores the number of scenario variabes
	ScenarioVarNb[indConfiguration] = nb_vars;
	//std::cout << "configuration #" << indConfiguration << " nb vars " << ScenarioVarNb[indConfiguration] << "\n";
	//printf("Loading scenario %s with %d variables\n", pg_ScenarioFileNames[indConfiguration].c_str(), ScenarioVarNb[indConfiguration]);

	// CALLBACKS
	std::getline(scenarioFin, line);
	// GUI
	std::getline(scenarioFin, line);
	// SHADER
	std::getline(scenarioFin, line);
	// PULSE
	std::getline(scenarioFin, line);

	// initial_values
	std::getline(scenarioFin, line);
	// storing the initial values
	std::getline(scenarioFin, line);
	stringstreamStoreLine(&sstream, &line);
	for (int indP = 0; indP < ScenarioVarNb[indConfiguration]; indP++) {
		indVar = ConfigScenarioVarRank[indP][indConfiguration];
		if (ScenarioVarConfigurations[indVar][indConfiguration]) {
			ConfigScenarioVarRank[indVar][indConfiguration];
			if (sstream.eof()) {
				sprintf(ErrorStr, "Error: missing initial value %s %d\n", temp.c_str(), indVar); ReportError(ErrorStr); throw 50;
			}
			sstream >> temp;
			if (ScenarioVarTypes[indVar] != _pg_string) {
				bool has_only_digits = (temp.find_first_not_of("0123456789-.E") == string::npos);
				if (!has_only_digits) {
					sprintf(ErrorStr, "Error: non numeric variable initial value for var %d (%s)\n", indVar, temp.c_str()); ReportError(ErrorStr); throw 50;
				}
				InitialValuesInterpVar[indVar][indConfiguration].val_num = my_stod(temp);
			}
			else {
				InitialValuesInterpVar[indVar][indConfiguration].val_string = temp;
			}
			//std::cout << indP << " " << InitialValuesInterpVar[indP][indConfiguration] << " \n";
			LastGUIShippedValuesInterpVar[indVar][indConfiguration] = MAXFLOAT;
		}
	}
	// checks that the number of variables is what is expected
	//sstream >> ID;
	if (!sstream.eof()) {
		sprintf(ErrorStr, "Error: too many initial variable values %s\n", sstream.str().c_str()); ReportError(ErrorStr); throw 50;
	}

	//std::cout << "\n";
	// /initial_values
	std::getline(scenarioFin, line);


	////////////////////////////
	////// SCENARIO
	pg_NbScenes[indConfiguration] = 0;
	// Number of scenes
	std::getline(scenarioFin, line);
	stringstreamStoreLine(&sstream, &line);
	sstream >> ID; // string scenario
	sstream >> pg_NbScenes[indConfiguration];

	pg_Scenario[indConfiguration] = new Scene[pg_NbScenes[indConfiguration]];

	for (int indScene = 0; indScene < pg_NbScenes[indConfiguration]; indScene++) {
		pg_Scenario[indConfiguration][indScene].init_scene();
	}
	pg_InterpolationScene.init_scene();
	pg_variable_updated = new bool[_MaxInterpVarIDs];
	pg_variable_param_input_type = new pg_Parameter_Input_Type[_MaxInterpVarIDs];
	pg_variable_scenario_or_gui_command_value = new ScenarioValue[_MaxInterpVarIDs];
	for (int indP = 0; indP < ScenarioVarNb[indConfiguration]; indP++) {
		indVar = ConfigScenarioVarRank[indP][indConfiguration];
		if (ScenarioVarConfigurations[indVar][indConfiguration]) {
			pg_variable_updated[indVar] = false;
			pg_variable_param_input_type[indVar] = _PG_SCENARIO;
			pg_variable_scenario_or_gui_command_value[indVar] = ScenarioValue();
		}
	}

	printf("Loading scenario %s: %d scenes with %d variables\n", pg_ScenarioFileNames[indConfiguration].c_str(), pg_NbScenes[indConfiguration], ScenarioVarNb[indConfiguration]);
	for (int indScene = 0; indScene < pg_NbScenes[indConfiguration]; indScene++) {
		std::getline(scenarioFin, line);
		// std::cout << "scene: " << line << "\n";
		stringstreamStoreLine(&sstream, &line);
		sstream >> ID; // string scene
		sstream >> pg_Scenario[indConfiguration][indScene].scene_IDs;
		sstream >> pg_Scenario[indConfiguration][indScene].scene_duration;
		sstream >> temp;  // change_when_ends or prolong_when_ends
		if (temp.compare("change_when_ends") == 0) {
			pg_Scenario[indConfiguration][indScene].scene_change_when_ends = true;
		}
		else if (temp.compare("prolong_when_ends") == 0) {
			pg_Scenario[indConfiguration][indScene].scene_change_when_ends = false;
		}
		else {
			pg_Scenario[indConfiguration][indScene].scene_change_when_ends = true;
			sprintf(ErrorStr, "Error: one of strings expected as scene ending mode: \"change_when_ends\" or \"prolong_when_ends\" not \"%s\" for scene %d %s\n", temp.c_str(), indScene, pg_Scenario[indConfiguration][indScene].scene_IDs.c_str()); ReportError(ErrorStr); throw 50;
		}
		// second and third comments possibly displayed on the interface to help the user
		if (!sstream.eof()) {
			sstream >> pg_Scenario[indConfiguration][indScene].scene_Msg1;
		}
		else {
			pg_Scenario[indConfiguration][indScene].scene_Msg1 = "";
		}
		if (!sstream.eof()) {
			sstream >> pg_Scenario[indConfiguration][indScene].scene_Msg2;
		}
		else {
			pg_Scenario[indConfiguration][indScene].scene_Msg2 = "";
		}

		//std::cout << "duration : " << 
		//  pg_Scenario[indConfiguration][indScene].scene_duration << " sc #" << indScene << "\n";

		if (pg_Scenario[indConfiguration][indScene].scene_duration <= 0.0) {
			sprintf(ErrorStr, "Error: null scene #%d duration [%f]!", indScene + 1, pg_Scenario[indConfiguration][indScene].scene_duration); ReportError(ErrorStr); throw 50;
		}
		if (indScene > 0) {
			pg_Scenario[indConfiguration][indScene].scene_initial_time = pg_Scenario[indConfiguration][indScene - 1].scene_final_time;
		}
		else {
			pg_Scenario[indConfiguration][indScene].scene_initial_time = 0.0;
		}
		pg_Scenario[indConfiguration][indScene].scene_final_time
			= pg_Scenario[indConfiguration][indScene].scene_initial_time + pg_Scenario[indConfiguration][indScene].scene_duration;

		// verbatim
		std::getline(scenarioFin, line);
		// std::cout << "verbatim: " << line << "\n";
		stringstreamStoreLine(&sstream, &line);
		int sceneNo;
		sstream >> sceneNo; // string scene
		if (sceneNo != indScene + 1) {
			sprintf(ErrorStr, "Error: scene number not set for scene [%d], run RenumberScenesScenario macro!", indScene + 1); ReportError(ErrorStr); throw 50;
		}
		//printf("scene %d: %s\n", sceneNo, pg_Scenario[indConfiguration][indScene].scene_IDs.c_str());
		// var comment
		std::getline(scenarioFin, line);
		// std::cout << "var comment: " << line << "\n";

		// storing the initial values
		std::getline(scenarioFin, line);
		stringstreamStoreLine(&sstream, &line);
		//std::cout << "line :" << line;
		//std::cout << "\ninitial values :\n";
		for (int indP = 0; indP < ScenarioVarNb[indConfiguration]; indP++) {
			indVar = ConfigScenarioVarRank[indP][indConfiguration];
			if (ScenarioVarConfigurations[indVar][indConfiguration]) {
				if (sstream.eof()) {
					sprintf(ErrorStr, "Error: missing initial value in scene %d var %d (%s)\n",
						indScene + 1, indVar, ScenarioVarMessages[indVar]); ReportError(ErrorStr); throw 50;
				}
				sstream >> temp;
				if (ScenarioVarTypes[indVar] == _pg_string) {
					pg_Scenario[indConfiguration][indScene].scene_initial_parameters[indVar].val_string = temp;
				}
				else {
					bool has_only_digits = (temp.find_first_not_of("0123456789-.E") == string::npos);
					if (!has_only_digits) {
						sprintf(ErrorStr, "Error: non numeric variable initial value in scene %d var %d (%s) type %d (%s)\n",
							indScene + 1, indVar, ScenarioVarMessages[indVar], ScenarioVarTypes[indVar], ScenarioVarMessages[indVar]); ReportError(ErrorStr); throw 50;
					}
					else {
						pg_Scenario[indConfiguration][indScene].scene_initial_parameters[indVar].val_num = my_stod(temp);
					}
				}
			}
		}
		// checks that the number of variables is what is expected
		//sstream >> ID;
		if (!sstream.eof()) {
			sprintf(ErrorStr, "Error: too many initial variable values in scene %d (%s)\n", indScene + 1, sstream.str().c_str()); ReportError(ErrorStr); throw 50;
		}

		// storing the final values
		std::getline(scenarioFin, line);
		stringstreamStoreLine(&sstream, &line);
		//std::cout << "line :" << line;
		//std::cout << "\nfinal values :\n";

		for (int indP = 0; indP < ScenarioVarNb[indConfiguration]; indP++) {
			indVar = ConfigScenarioVarRank[indP][indConfiguration];
			if (ScenarioVarConfigurations[indVar][indConfiguration]) {
				if (sstream.eof()) {
					sprintf(ErrorStr, "Error: missing final value in scene %d var %d (%s)\n", indScene + 1, indVar, temp.c_str()); ReportError(ErrorStr); throw 50;
				}
				sstream >> temp;
				//printf("val %s\n", temp.c_str());
				if (ScenarioVarTypes[indVar] == _pg_string) {
					pg_Scenario[indConfiguration][indScene].scene_final_parameters[indVar].val_string = temp;
				}
				else {
					bool has_only_digits = (temp.find_first_not_of("0123456789-.E") == string::npos);
					if (!has_only_digits) {
						sprintf(ErrorStr, "Error: non numeric variable final value in scene %d var %d type %d (%s)\n", indScene + 1, indVar, ScenarioVarTypes[indVar], temp.c_str()); ReportError(ErrorStr); throw 50;
					}
					else {
						pg_Scenario[indConfiguration][indScene].scene_final_parameters[indVar].val_num = my_stod(temp);
					}
				}
			}
			// std::cout << scene_final_parameters[indScene][indVar].val_num << " ";
		}
		// checks that the number of variables is what is expected
		//sstream >> ID;
		if (!sstream.eof()) {
			sprintf(ErrorStr, "Error: too many final variable values in scene %d\n", indScene + 1); ReportError(ErrorStr); throw 50;
		}

		std::getline(scenarioFin, line);
		stringstreamStoreLine(&sstream, &line);
		//printf("line interp %s\n", line.c_str());

		// storing the interpolation mode
		for (int indP = 0; indP < ScenarioVarNb[indConfiguration]; indP++) {
			indVar = ConfigScenarioVarRank[indP][indConfiguration];
			if (ScenarioVarConfigurations[indVar][indConfiguration]) {
				char valCh = 0;
				char valCh2 = 0;
				string vals, val2s;
				float val3;

				pg_Scenario[indConfiguration][indScene].scene_interpolations[indVar].offSet = 0.0;
				pg_Scenario[indConfiguration][indScene].scene_interpolations[indVar].duration = 1.0;
				pg_Scenario[indConfiguration][indScene].scene_interpolations[indVar].midTermValue
					= 0.5 * (pg_Scenario[indConfiguration][indScene].scene_initial_parameters[indVar].val_num
						+ pg_Scenario[indConfiguration][indScene].scene_final_parameters[indVar].val_num);

				if (sstream.eof()) {
					sprintf(ErrorStr, "Error: missing interpolation value in scene %d (%s) var %d (%s)\n",
						indScene + 1, pg_Scenario[indConfiguration][indScene].scene_IDs.c_str(), indVar + 1, ScenarioVarMessages[indVar]); ReportError(ErrorStr); throw 50;
				}
				sstream >> std::skipws >> temp;
				valCh = temp.at(0);
				//printf("valch %d %c temp %s\n", (int)valCh, valCh, temp.c_str());
				if (temp.length() > 1) {
					valCh2 = temp.at(1);
					if (valCh2 != 'k') {
						sprintf(ErrorStr, "Error: only k modifier is allowed on interpolation mode, not %c in scene %d var %d\n", valCh2, indScene + 1, indVar + 1); ReportError(ErrorStr); throw 50;
					}
				}
				else {
					valCh2 = ' ';
				}

				// l: value interpolates linearly between initial and final value from 0.0% to 1.0%
				// L: value is initial from 0.0% until offset, 
				// interpolates linearly between initial and final value from offset to offset + duration
				// is final value between offset + duration and 1.0%
				if (valCh == 'l' || valCh == 'L') {
					pg_Scenario[indConfiguration][indScene].scene_interpolations[indVar].interpolation_mode = pg_linear_interpolation;
					if (valCh2 == ' ') {
						pg_Scenario[indConfiguration][indScene].scene_interpolations[indVar].initialization_mode = pg_scenario_initial;
					}
					else if (valCh2 == 'k') {
						pg_Scenario[indConfiguration][indScene].scene_interpolations[indVar].initialization_mode = pg_current_value;
					}
					if (valCh == 'L') {
						sstream >> vals;
						sstream >> val2s;
						double val = stringToDuration(vals, pg_Scenario[indConfiguration][indScene].scene_duration, indScene + 1, indVar + 1);
						double val2 = stringToDuration(val2s, pg_Scenario[indConfiguration][indScene].scene_duration, indScene + 1, indVar + 1);
						if (val < 0.0 || val2 < 0.0) {
							sprintf(ErrorStr, "Error: one of values of L(inear) in scene %d var %d lower than 0.0: %.3f %.3f\n", indScene + 1, indVar + 1, val, val2); ReportError(ErrorStr); throw 50;
						}
						if (val <= 1.0) {
							pg_Scenario[indConfiguration][indScene].scene_interpolations[indVar].offSet = val;
							if (float(val + val2) <= 1.00001f) {
								// deals with approximate values that can summ above 1.0
								if (float(val + val2) > 1.0f) {
									val2 = 1.0f - val;
								}
								pg_Scenario[indConfiguration][indScene].scene_interpolations[indVar].duration = val2;
								if (pg_Scenario[indConfiguration][indScene].scene_interpolations[indVar].duration <= 0.0) {
									sprintf(ErrorStr, "Error: null L(inear) in scene %d var %d duration [%f]!", indScene + 1, indVar + 1, pg_Scenario[indConfiguration][indScene].scene_interpolations[indVar].duration); ReportError(ErrorStr); throw 50;
								}
							}
							else {
								sprintf(ErrorStr, "Error: total duration of L(inear) in scene %d var %d greater than 1.0: %.3f + %.3f\n", indScene + 1, indVar + 1, val, val2); ReportError(ErrorStr); throw 50;
							}
						}
						else {
							sprintf(ErrorStr, "Error: offset value L(inear) in scene %d var %d greater than 1.0: %.3f\n", indScene + 1, indVar + 1, val); ReportError(ErrorStr); throw 50;
						}
						// std::cout << "L " << val << " " << val2 << " ";

					}

				}
				// c: value interpolates cosinely between initial and final value from 0.0% to 1.0%
				// C: value is initial from 0.0% until offset, 
				// interpolates cosinely between initial and final value from offset to offset + duration
				// is final value between offset + duration and 1.0%
				// COSINE INTERPOLATION
				else if (valCh == 'c' || valCh == 'C') {
					pg_Scenario[indConfiguration][indScene].scene_interpolations[indVar].interpolation_mode = pg_cosine_interpolation;
					if (valCh2 == ' ') {
						pg_Scenario[indConfiguration][indScene].scene_interpolations[indVar].initialization_mode = pg_scenario_initial;
					}
					else if (valCh2 == 'k') {
						pg_Scenario[indConfiguration][indScene].scene_interpolations[indVar].initialization_mode = pg_current_value;
					}
					if (valCh == 'C') {
						sstream >> vals;
						sstream >> val2s;
						double val = stringToDuration(vals, pg_Scenario[indConfiguration][indScene].scene_duration, indScene + 1, indVar + 1);
						double val2 = stringToDuration(val2s, pg_Scenario[indConfiguration][indScene].scene_duration, indScene + 1, indVar + 1);
						if (val < 0.0 || val2 < 0.0) {
							sprintf(ErrorStr, "Error: one of values of C(osine) in scene %d var %d lower than 0.0: %.3f %.3f\n", indScene + 1, indVar + 1, val, val2); ReportError(ErrorStr); throw 50;
						}
						if (val <= 1.0) {
							pg_Scenario[indConfiguration][indScene].scene_interpolations[indVar].offSet = val;
							// deals with approximate values that can summ above 1.0
							if (float(val + val2) <= 1.00001f) {
								if (float(val + val2) > 1.0f) {
									val2 = 1.0f - val;
								}
								pg_Scenario[indConfiguration][indScene].scene_interpolations[indVar].duration = val2;
								if (pg_Scenario[indConfiguration][indScene].scene_interpolations[indVar].duration <= 0.0) {
									sprintf(ErrorStr, "Error: null C(osine) in scene %d var %d duration [%f]!", indScene + 1, indVar + 1, pg_Scenario[indConfiguration][indScene].scene_interpolations[indVar].duration); ReportError(ErrorStr); throw 50;
								}
							}
							else {
								sprintf(ErrorStr, "Error: total duration of C(osine) in scene %d var %d greater than 1.0: %.3f + %.3f\n", indScene + 1, indVar + 1, val, val2); ReportError(ErrorStr); throw 50;
							}
						}
						else {
							sprintf(ErrorStr, "Error: offset value C(osine) in scene %d var %d greater than 1.0: %.3f\n", indScene + 1, indVar + 1, val); ReportError(ErrorStr); throw 50;
						}
					}
				}
				// z: value interpolates cosinely between initial and final value from 0.0% to 1.0%
				// Z: value is initial from 0.0% until offset, 
				// Beizer approximated with a cosine and a non linear input 3x^2-2x^3
				// BEZIER INTERPOLATION
				else if (valCh == 'z' || valCh == 'Z') {
					pg_Scenario[indConfiguration][indScene].scene_interpolations[indVar].interpolation_mode = pg_bezier_interpolation;
					if (valCh2 == ' ') {
						pg_Scenario[indConfiguration][indScene].scene_interpolations[indVar].initialization_mode = pg_scenario_initial;
					}
					else if (valCh2 == 'k') {
						pg_Scenario[indConfiguration][indScene].scene_interpolations[indVar].initialization_mode = pg_current_value;
					}
					if (valCh == 'Z') {
						sstream >> vals;
						sstream >> val2s;
						double val = stringToDuration(vals, pg_Scenario[indConfiguration][indScene].scene_duration, indScene + 1, indVar + 1);
						double val2 = stringToDuration(val2s, pg_Scenario[indConfiguration][indScene].scene_duration, indScene + 1, indVar + 1);
						if (val < 0.0 || val2 < 0.0) {
							sprintf(ErrorStr, "Error: one of values of Z(Bezier) in scene %d var %d lower than 0.0: %.3f %.3f\n", indScene + 1, indVar + 1, val, val2); ReportError(ErrorStr); throw 50;
						}
						if (val <= 1.0) {
							pg_Scenario[indConfiguration][indScene].scene_interpolations[indVar].offSet = double(val);
							// deals with approximate values that can summ above 1.0
							if (float(double(val) + double(val2)) <= 1.00001) {
								if (float(double(val) + double(val2)) > 1.0) {
									val2 = 1.0f - val;
								}
								pg_Scenario[indConfiguration][indScene].scene_interpolations[indVar].duration = val2;
								if (pg_Scenario[indConfiguration][indScene].scene_interpolations[indVar].duration <= 0.0) {
									sprintf(ErrorStr, "Error: null Z(Bezier) in scene %d var %d duration [%f]!", indScene + 1, indVar + 1, pg_Scenario[indConfiguration][indScene].scene_interpolations[indVar].duration); ReportError(ErrorStr); throw 50;
								}
							}
							else {
								sprintf(ErrorStr, "Error: total duration of Z(Bezier) in scene %d var %d greater than 1.0: %.3f + %.3f\n", indScene + 1, indVar + 1, val, val2); ReportError(ErrorStr); throw 50;
							}
						}
						else {
							sprintf(ErrorStr, "Error: offset value Z(Bezier) in scene %d var %d greater than 1.0: %.3f\n", indScene + 1, indVar + 1, val); ReportError(ErrorStr); throw 50;
						}
					}
				}
				// e: value interpolates exponentially between initial and final value from 0.0% to 1.0%
				// E: value is initial from 0.0% until offset, 
				// exponential interpolation: alpha ^ exponent
				// EXPONENTIAL INTERPOLATION
				else if (valCh == 'e' || valCh == 'E') {
					pg_Scenario[indConfiguration][indScene].scene_interpolations[indVar].interpolation_mode = pg_exponential_interpolation;
					if (valCh2 == ' ') {
						pg_Scenario[indConfiguration][indScene].scene_interpolations[indVar].initialization_mode = pg_scenario_initial;
					}
					else if (valCh2 == 'k') {
						pg_Scenario[indConfiguration][indScene].scene_interpolations[indVar].initialization_mode = pg_current_value;
					}
					sstream >> val3;
					pg_Scenario[indConfiguration][indScene].scene_interpolations[indVar].exponent = val3;
					if (valCh == 'E') {
						sstream >> vals;
						sstream >> val2s;
						double val = stringToDuration(vals, pg_Scenario[indConfiguration][indScene].scene_duration, indScene + 1, indVar + 1);
						double val2 = stringToDuration(val2s, pg_Scenario[indConfiguration][indScene].scene_duration, indScene + 1, indVar + 1);
						if (val < 0.0 || val2 < 0.0) {
							sprintf(ErrorStr, "Error: one of values of E(exponential) in scene %d var %d lower than 0.0: %.3f %.3f\n", indScene + 1, indVar + 1, val, val2); ReportError(ErrorStr); throw 50;
						}
						if (val <= 1.0) {
							pg_Scenario[indConfiguration][indScene].scene_interpolations[indVar].offSet = val;
							// deals with approximate values that can summ above 1.0
							if (float(double(val) + double(val2)) <= 1.00001) {
								if (float(double(val) + double(val2)) > 1.0) {
									val2 = 1.0f - val;
								}
								pg_Scenario[indConfiguration][indScene].scene_interpolations[indVar].duration = val2;
								if (pg_Scenario[indConfiguration][indScene].scene_interpolations[indVar].duration <= 0.0) {
									sprintf(ErrorStr, "Error: null Z(Bezier) in scene %d var %d duration [%f]!", indScene + 1, indVar + 1, pg_Scenario[indConfiguration][indScene].scene_interpolations[indVar].duration); ReportError(ErrorStr); throw 50;
								}
							}
							else {
								sprintf(ErrorStr, "Error: total duration of Z(Bezier) in scene %d var %d greater than 1.0: %.3f + %.3f\n", indScene + 1, indVar + 1, val, val2); ReportError(ErrorStr); throw 50;
							}
						}
						else {
							sprintf(ErrorStr, "Error: offset value Z(Bezier) in scene %d var %d greater than 1.0: %.3f\n", indScene + 1, indVar + 1, val); ReportError(ErrorStr); throw 50;
						}
					}
				}
				// b: bell curve interpolation between initial, median and final value from (0,0,0)% to (0,1,0)% at mid time to (0,0,1)% at the end
				// BELL INTERPOLATION
				else if (valCh == 'b' || valCh == 'B') {
					pg_Scenario[indConfiguration][indScene].scene_interpolations[indVar].interpolation_mode = pg_bell_interpolation;
					if (valCh2 == ' ') {
						pg_Scenario[indConfiguration][indScene].scene_interpolations[indVar].initialization_mode = pg_scenario_initial;
					}
					else if (valCh2 == 'k') {
						pg_Scenario[indConfiguration][indScene].scene_interpolations[indVar].initialization_mode = pg_current_value;
					}
					sstream >> pg_Scenario[indConfiguration][indScene].scene_interpolations[indVar].midTermValue;
					//printf("Bell interpolation mode in scene %d (%s) parameter %d [%d] [%c] mid term value %.2f!\n",
					//	indScene + 1, pg_Scenario[indConfiguration][indScene].scene_IDs.c_str(), indVar + 1, int(valCh), valCh, 
					//	pg_Scenario[indConfiguration][indScene].scene_interpolations[indVar].midTermValue);
					if (valCh == 'B') {
						sstream >> vals;
						sstream >> val2s;
						double val = stringToDuration(vals, pg_Scenario[indConfiguration][indScene].scene_duration, indScene + 1, indVar + 1);
						double val2 = stringToDuration(val2s, pg_Scenario[indConfiguration][indScene].scene_duration, indScene + 1, indVar + 1);
						if (val < 0.0 || val2 < 0.0) {
							sprintf(ErrorStr, "Error: one of values of B(ell) in scene %d var %d lower than 0.0: %.3f %.3f\n", indScene + 1, indVar + 1, val, val2); ReportError(ErrorStr); throw 50;
						}
						if (val <= 1.0) {
							pg_Scenario[indConfiguration][indScene].scene_interpolations[indVar].offSet = val;
							if (float(val + val2) <= 1.00001f) {
								// deals with approximate values that can summ above 1.0
								if (float(val + val2) > 1.0f) {
									val2 = 1.0f - val;
								}
								pg_Scenario[indConfiguration][indScene].scene_interpolations[indVar].duration = val2;
								if (pg_Scenario[indConfiguration][indScene].scene_interpolations[indVar].duration <= 0.0) {
									sprintf(ErrorStr, "Error: null B(ell) in scene %d var %d duration [%f]!", indScene + 1, indVar + 1, pg_Scenario[indConfiguration][indScene].scene_interpolations[indVar].duration); ReportError(ErrorStr); throw 50;
								}
							}
							else {
								sprintf(ErrorStr, "Error: total duration of B(ell) in scene %d var %d greater than 1.0: %.3f + %.3f\n", indScene + 1, indVar + 1, val, val2); ReportError(ErrorStr); throw 50;
							}
						}
						else {
							sprintf(ErrorStr, "Error: offset value L(inear) in scene %d var %d greater than 1.0: %.3f\n", indScene + 1, indVar + 1, val); ReportError(ErrorStr); throw 50;
						}
						// std::cout << "L " << val << " " << val2 << " ";

					}
				}
				// b: saw tooth linear interpolation between initial, median and final value from (0,0,0)% to (0,1,0)% at mid time to (0,0,1)% at the end
				// SAW TOOTH INTERPOLATION
				else if (valCh == 't') {
					pg_Scenario[indConfiguration][indScene].scene_interpolations[indVar].interpolation_mode = pg_sawtooth_interpolation;
					if (valCh2 == ' ') {
						pg_Scenario[indConfiguration][indScene].scene_interpolations[indVar].initialization_mode = pg_scenario_initial;
					}
					else if (valCh2 == 'k') {
						pg_Scenario[indConfiguration][indScene].scene_interpolations[indVar].initialization_mode = pg_current_value;
					}
					sstream >> pg_Scenario[indConfiguration][indScene].scene_interpolations[indVar].midTermValue;
				}
				// STEPWISE VALUE WITHOUT INTERPOLATION
				else if (valCh == 's' || valCh == 'S') {
					pg_Scenario[indConfiguration][indScene].scene_interpolations[indVar].interpolation_mode = pg_stepwise_interpolation;
					pg_Scenario[indConfiguration][indScene].scene_interpolations[indVar].initialization_mode = pg_scenario_initial;
					pg_Scenario[indConfiguration][indScene].scene_interpolations[indVar].offSet = 0.0;
					pg_Scenario[indConfiguration][indScene].scene_interpolations[indVar].duration = 1.0;
					if (valCh == 'S') {
						sstream >> vals;
						double val = stringToDuration(vals, pg_Scenario[indConfiguration][indScene].scene_duration, indScene + 1, indVar + 1);
						if (val < 0.0) {
							sprintf(ErrorStr, "Error: offset values of S(tepwise) in scene %d (%s) var %d lower than 0.0: %.3f\n", indScene + 1, pg_Scenario[indConfiguration][indScene].scene_IDs.c_str(), indVar + 1, val); ReportError(ErrorStr); throw 50;
						}
						if (val <= 1.0) {
							pg_Scenario[indConfiguration][indScene].scene_interpolations[indVar].offSet = val;
							pg_Scenario[indConfiguration][indScene].scene_interpolations[indVar].duration = 1.0 - double(val);
						}
						else {
							sprintf(ErrorStr, "Error: offset value of S(tepwise) in scene %d (%s) var %d greater than 1.0: %.3f\n", indScene + 1, pg_Scenario[indConfiguration][indScene].scene_IDs.c_str(), indVar + 1, val); ReportError(ErrorStr); throw 50;
						}
					}
					else {
						if (ScenarioVarTypes[indVar] == _pg_string
							&& pg_Scenario[indConfiguration][indScene].scene_initial_parameters[indVar].val_string
							!= pg_Scenario[indConfiguration][indScene].scene_final_parameters[indVar].val_string) {
							sprintf(ErrorStr, "Error: S(tepwise) interpolation should have same initial and final values %s/%s in scene %d (%s) var %d\n",
								pg_Scenario[indConfiguration][indScene].scene_initial_parameters[indVar].val_string.c_str(), pg_Scenario[indConfiguration][indScene].scene_final_parameters[indVar].val_string.c_str(), indScene + 1, pg_Scenario[indConfiguration][indScene].scene_IDs.c_str(), indVar + 1); ReportError(ErrorStr); throw 50;
						}
						else if (ScenarioVarTypes[indVar] != _pg_string
							&& pg_Scenario[indConfiguration][indScene].scene_initial_parameters[indVar].val_num
							!= pg_Scenario[indConfiguration][indScene].scene_final_parameters[indVar].val_num) {
							sprintf(ErrorStr, "Error: S(tepwise) interpolation should have same initial and final values %.2f/%.2f in scene %d (%s) var %d\n", pg_Scenario[indConfiguration][indScene].scene_initial_parameters[indVar].val_num, pg_Scenario[indConfiguration][indScene].scene_final_parameters[indVar].val_num, indScene + 1, pg_Scenario[indConfiguration][indScene].scene_IDs.c_str(), indVar + 1); ReportError(ErrorStr); throw 50;
						}
					}
				}
				// KEEP CURRENT VALUE WITHOUT INTERPOLATION
				else if (valCh == 'k' || valCh == 'K') {
					pg_Scenario[indConfiguration][indScene].scene_interpolations[indVar].initialization_mode = pg_current_value;
					pg_Scenario[indConfiguration][indScene].scene_interpolations[indVar].interpolation_mode = pg_keep_value;
				}
				else {
					sprintf(ErrorStr, "Error: unknown interpolation mode in scene %d (%s) parameter %d [%d] [%c]!", indScene + 1, pg_Scenario[indConfiguration][indScene].scene_IDs.c_str(), indVar + 1, int(valCh), valCh); ReportError(ErrorStr); throw 50;
				}

				//if (indVar == _trkDecay_1 && indScene == 12) {
				//	printf("Decay Scene %d Interpolation mode %d offset %.2f dur %.2f init %.2f fin %.2f\n", indScene, pg_Scenario[indConfiguration][indScene].scene_interpolations[indVar].interpolation_mode,
				//		pg_Scenario[indConfiguration][indScene].scene_interpolations[indVar].offSet,
				//		pg_Scenario[indConfiguration][indScene].scene_interpolations[indVar].duration,
				//		pg_Scenario[indConfiguration][indScene].scene_initial_parameters[indVar],
				//		pg_Scenario[indConfiguration][indScene].scene_final_parameters[indVar]);
				//}
			}
		}

		// /scene
		std::getline(scenarioFin, line);
		stringstreamStoreLine(&sstream, &line);
		sstream >> ID; // string /scene
		if (ID.compare("/scene") != 0) {
			sprintf(ErrorStr, "Error: incorrect configuration file expected string \"/scene\" not found! (instead \"%s\")", ID.c_str()); ReportError(ErrorStr); throw 100;
		}
	}
	// /scenario
	std::getline(scenarioFin, line);
	stringstreamStoreLine(&sstream, &line);
	sstream >> ID; // string /scenario
	if (ID.compare("/scenario") != 0) {
		sprintf(ErrorStr, "Error: incorrect configuration file expected string \"/scenario\" not found! (instead \"%s\")", ID.c_str()); ReportError(ErrorStr); throw 100;
	}

	ParseScenarioVideos(scenarioFin, indConfiguration);

	ParseScenarioClipsAndPhotos(scenarioFin, indConfiguration);

	ParseScenarioSoundtracks(scenarioFin, indConfiguration);

	ParseScenarioSVGPaths(scenarioFin, indConfiguration);

	ParseScenarioClipArt(scenarioFin, indConfiguration);

	ParseScenarioMeshes(scenarioFin, indConfiguration);

	ParseScenarioTextures(scenarioFin, indConfiguration);

	ParseScenarioColorPalettes(scenarioFin, indConfiguration);

	ParseScenarioColorPresets(scenarioFin, indConfiguration);

	ParseScenarioLights(scenarioFin, indConfiguration);

	// saves the original durations
	saveInitialTimesAndDurations(indConfiguration);
}	

int FindSceneById(std::string * sceneID) {
	for (int ind_scene = 0; ind_scene < pg_NbScenes[pg_current_configuration_rank]; ind_scene++) {
		if (pg_Scenario[pg_current_configuration_rank][ind_scene].scene_IDs.compare(*sceneID) == 0) {
			return ind_scene;
		}
	}
	return -1;
}


void setWindowDimensions(void) {
	if (double_window && window_width > 1920) {
		if (wide_screen) {
			workingWindow_width = window_width * 3 / 7;
		}
		else {
			workingWindow_width = window_width / 2;
		}
		rightWindowVMargin = (window_width -  2 * workingWindow_width) / 2;
	}
	else {
		workingWindow_width = window_width;
		rightWindowVMargin = 0;
	}
	// looks for the smallest powers of 2 for width and height
	workingWindow_width_powerOf2 = 1;
	while (workingWindow_width_powerOf2 < workingWindow_width)
		workingWindow_width_powerOf2 *= 2;
	window_height_powerOf2 = 1;
	while (window_height_powerOf2 < window_height)
		window_height_powerOf2 *= 2;
	window_width_powerOf2 = 1;
	while (window_width_powerOf2 < window_width)
		window_width_powerOf2 *= 2;
	workingWindow_width_powerOf2_ratio =
		float(workingWindow_width) / float(workingWindow_width_powerOf2);
	window_height_powerOf2_ratio =
		float(window_height) / float(window_height_powerOf2);
}

// to be called once after parsing
void saveInitialTimesAndDurations(int indConfiguration) {
	for (int indScene = 0; indScene < pg_NbScenes[indConfiguration]; indScene++) {
		pg_Scenario[indConfiguration][indScene].scene_originalDuration = pg_Scenario[indConfiguration][indScene].scene_duration;
		pg_Scenario[indConfiguration][indScene].scene_originalInitial_time = pg_Scenario[indConfiguration][indScene].scene_initial_time;
		pg_Scenario[indConfiguration][indScene].scene_originalFinal_time = pg_Scenario[indConfiguration][indScene].scene_final_time;
	}
}
// to be called before setup change
void restoreInitialTimesAndDurations(void) {
	for (int indScene = 0; indScene < pg_NbScenes[pg_current_configuration_rank]; indScene++) {
		pg_Scenario[pg_current_configuration_rank][indScene].scene_duration = pg_Scenario[pg_current_configuration_rank][indScene].scene_originalDuration;
		pg_Scenario[pg_current_configuration_rank][indScene].scene_initial_time = pg_Scenario[pg_current_configuration_rank][indScene].scene_originalInitial_time;
		pg_Scenario[pg_current_configuration_rank][indScene].scene_final_time = pg_Scenario[pg_current_configuration_rank][indScene].scene_originalFinal_time;
	}
}

void pg_LoadConfigurationFile(const char* confFileName, int indConfiguration) {
	//printf("Loading %s\n", confFileName);

	std::ifstream confFin(confFileName);
	if (!confFin) {
		sprintf(ErrorStr, "Error: file [%s] not found!", confFileName); ReportError(ErrorStr); throw 11;
	}
	parseConfigurationFile(confFin, indConfiguration);
	confFin.close();
}
void pg_LoadScenarioFile(const char* scenarioFileName, int indConfiguration) {
	//printf("Loading %s\n", scenarioFileName);

	std::ifstream scenarioFin(scenarioFileName);
	if (!scenarioFin) {
		sprintf(ErrorStr, "Error: file [%s] not found!", scenarioFileName); ReportError(ErrorStr); throw 11;
	}
	parseScenarioFile(scenarioFin, indConfiguration);
	scenarioFin.close();
}

