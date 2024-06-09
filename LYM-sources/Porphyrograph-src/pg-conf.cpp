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

bool					 pg_last_scene_update = false;

int						 pg_NbConfigurations = 1;

vector<Scene*>			 pg_Scenario[PG_MAX_CONFIGURATIONS];
double                   current_scene_percent = 0.f;
						// table of PG_MAX_CONFIGURATIONS booleans tables indicating whether a var (from the full scenario) 
						// is active for the current configuration and scenario
bool					 pg_ScenarioActiveVars[_MaxInterpVarIDs][PG_MAX_CONFIGURATIONS] = { {false} };

string					 pg_csv_file_name;
string					 snapshots_dir_path_prefix;
string					 snapshots_dir_path_name;
GLuint					 Screen_Font_texture_Rectangle_texID = { NULL_ID };
pg_TextureData           screenFontTexData;
int                      screen_font_size;
string                   font_file_encoding;
TextureEncoding          font_texture_encoding;

pg_Window              *PG_Window;

// PNG capture
pg_Png_Capture pg_Png_Capture_param;

// JPG capture
pg_Jpg_Capture pg_Jpg_Capture_param;

// SVG path capture
pg_Svg_Capture pg_Svg_Capture_param;

// VIDEO capture
pg_Video_Capture pg_Video_Capture_param;

// UDP servers and clients
vector<pg_IPServer*>     IP_Servers;
vector<pg_IPClient*>     IP_Clients;

// nb configuration and scenario files
string *pg_ConfigurationFileNames = NULL;
string *pg_ScenarioFileNames = NULL;

// shader file names
string  ** pg_Shader_File_Names;
GLenum *** pg_Shader_Stages;
int     ** pg_Shader_nbStages;

// ClipArt GPU
// number of files
vector<ClipArt*>pg_ClipArts[PG_MAX_CONFIGURATIONS];

// last activated SvgGpu
int pg_last_activated_ClipArt;
// total number of paths
int pg_nb_tot_SvgGpu_paths[PG_MAX_CONFIGURATIONS];
// base ID of the GPU paths
GLuint ClipArt_path_baseID[PG_MAX_CONFIGURATIONS];
// fill color table
unsigned int** ClipArt_path_fill_color[PG_MAX_CONFIGURATIONS];

#if defined(var_activeMeshes)
// MESHES
vector<MeshData*> pg_Meshes[PG_MAX_CONFIGURATIONS];
// last activated Mesh
int pg_last_activated_Mesh = 0;
#endif

// TEXTURES
// number of Texture files
vector<pg_TextureData *> pg_Textures[PG_MAX_CONFIGURATIONS];
pg_TextureData texDataScreenFont;

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


bool isFullPath(string dir_or_filename) {
	if (dir_or_filename.length() > 0
		&& (dir_or_filename.find(':') != std::string::npos
			|| dir_or_filename[0] == '/')) {
		return true;
	}
	else {
		return false;
	}
}
void completeToFullPath(string &dir_or_filename) {
	if (!isFullPath(dir_or_filename)) {
		if (cwd[cwd.length() - 1] == '/') {
			dir_or_filename = cwd + dir_or_filename;
		}
		else {
			dir_or_filename = cwd + "/" + dir_or_filename;
		}
	}
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
		sstream >> pg_Video_Capture_param.beginVideo_file;
		sstream >> pg_Video_Capture_param.endVideo_file;
		sstream >> temp; // unused
		sstream >> pg_Video_Capture_param.Video_file_name;
		pg_Video_Capture_param.outputVideo_file = !pg_Video_Capture_param.Video_file_name.empty();
	}

	// storing the Svg capture values
	std::getline(confFin, line);
	stringstreamStoreLine(&sstream, &line);
	sstream >> ID;
	if (ID.compare("SVG") != 0) {
		sprintf(ErrorStr, "Error: incorrect configuration file expected string \"SVG\" not found! (instead \"%s\")", ID.c_str()); ReportError(ErrorStr); throw 100;
	}
	if (indConfiguration ==  0) {
		sstream >> pg_Svg_Capture_param.beginSvg;
		sstream >> pg_Svg_Capture_param.endSvg;
		sstream >> temp;
		if (temp.back() == 's') {
			temp.resize(size_t(temp.size() - 1));
			pg_Svg_Capture_param.stepSvgInSeconds = my_stof(temp);
			pg_Svg_Capture_param.nextSvgCapture = -1.;
			pg_Svg_Capture_param.stepSvgInFrames = -1;
		}
		else {
			pg_Svg_Capture_param.stepSvgInFrames = my_stoi(temp);
			pg_Svg_Capture_param.stepSvgInSeconds = -1.;
		}
		sstream >> pg_Svg_Capture_param.Svg_file_name;
		pg_Svg_Capture_param.outputSvg = !pg_Svg_Capture_param.Svg_file_name.empty();
		pg_Svg_Capture_param.indSvgSnapshot = 0;
	}

	// storing the Png capture values
	std::getline(confFin, line);
	stringstreamStoreLine(&sstream, &line);
	sstream >> ID;
	if (ID.compare("PNG") != 0) {
		sprintf(ErrorStr, "Error: incorrect configuration file expected string \"PNG\" not found! (instead \"%s\")", ID.c_str()); ReportError(ErrorStr); throw 100;
	}
	if (indConfiguration ==  0) {
		sstream >> pg_Png_Capture_param.beginPng;
		sstream >> pg_Png_Capture_param.endPng;
		sstream >> temp;
		if (temp.back() == 's') {
			temp.resize(size_t(temp.size() - 1));
			pg_Png_Capture_param.stepPngInSeconds = my_stof(temp);
			pg_Png_Capture_param.nextPngCapture = -1.;
			pg_Png_Capture_param.stepPngInFrames = -1;
		}
		else {
			pg_Png_Capture_param.stepPngInFrames = my_stoi(temp);
			pg_Png_Capture_param.stepPngInSeconds = -1.;
		}
		sstream >> pg_Png_Capture_param.Png_file_name;
		pg_Png_Capture_param.outputPng = !pg_Png_Capture_param.Png_file_name.empty();
		pg_Png_Capture_param.indPngSnapshot = 0;
	}

	// storing the Jpg capture values
	std::getline(confFin, line);
	stringstreamStoreLine(&sstream, &line);
	sstream >> ID;
	if (ID.compare("JPG") != 0) {
		sprintf(ErrorStr, "Error: incorrect configuration file expected string \"JPG\" not found! (instead \"%s\")", ID.c_str()); ReportError(ErrorStr); throw 100;
	}
	if (indConfiguration ==  0) {
		sstream >> pg_Jpg_Capture_param.beginJpg;
		sstream >> pg_Jpg_Capture_param.endJpg;
		sstream >> temp;
		if (temp.back() == 's') {
			temp.resize(size_t(temp.size() - 1));
			pg_Jpg_Capture_param.stepJpgInSeconds = my_stof(temp);
			pg_Jpg_Capture_param.nextJpgCapture = -1.;
			pg_Jpg_Capture_param.stepJpgInFrames = -1;
		}
		else {
			pg_Jpg_Capture_param.stepJpgInFrames = my_stoi(temp);
			pg_Jpg_Capture_param.stepJpgInSeconds = -1.;
		}

		sstream >> pg_Jpg_Capture_param.Jpg_file_name;
		pg_Jpg_Capture_param.outputJpg = !pg_Jpg_Capture_param.Jpg_file_name.empty();
		pg_Jpg_Capture_param.indJpgSnapshot = 0;
	}

	// /rendering_files
	std::getline(confFin, line);

	////////////////////////////////////
	////// IP CLIENTS/SERVERS
	////////////////////////////////////

	if (indConfiguration ==  0) {
		IP_Servers.clear();

		IP_Clients.clear();
	}

	// udp_local_server Number of servers
	std::getline(confFin, line);
	stringstreamStoreLine(&sstream, &line);
	sstream >> ID;
	if (ID.compare("udp_local_server") != 0) {
		sprintf(ErrorStr, "Error: incorrect configuration file expected string \"udp_local_server\" not found! (instead \"%s\")", ID.c_str()); ReportError(ErrorStr); throw 100;
	}

	// VERBATIM
	std::getline(confFin, line);
	// TYPE
	std::getline(confFin, line);
	// ID
	std::getline(confFin, line);

	if (indConfiguration == 0) {
		while(true) {
			std::getline(confFin, line);
			stringstreamStoreLine(&sstream, &line);
			sstream >> ID; // string "server" or end with "/udp_local_server"
			if (ID.compare("/udp_local_server") == 0) {
				break;
			}
			else if (ID.compare("server") != 0) {
				sprintf(ErrorStr, "Error: incorrect configuration file expected string \"server\" not found! (instead \"%s\")", ID.c_str()); ReportError(ErrorStr); throw 100;
			}
			pg_IPServer *server = new pg_IPServer();
			sstream >> server->id;
			sstream >> server->Local_server_port;
			sstream >> ID;
			for (int ind = 0; ind < Emptypg_UDPMessageFormat; ind++) {
				if (strcmp(ID.c_str(), pg_UDPMessageFormatString[ind]) == 0) {
					server->receive_format = (pg_UDPMessageFormat)ind;
					break;
				}
			}
			if (server->receive_format == Emptypg_UDPMessageFormat) {
				sprintf(ErrorStr, "Error: unknown receive message format [%s]!", ID.c_str()); ReportError(ErrorStr); throw 249;
			}
			sstream >> server->IP_message_trace;
			sstream >> server->depth_input_stack;
			sstream >> server->OSC_duplicate_removal;
			// printf("serveur %d duplicate removal %d\n", ind_IP_Server, IP_Servers[ind_IP_Server]->OSC_duplicate_removal);
			sstream >> server->OSC_endian_reversal;
			IP_Servers.push_back(server);
		}
	}
	else {
		while (true) {
			std::getline(confFin, line);
			stringstreamStoreLine(&sstream, &line);
			sstream >> ID; // string "server" or end with "/udp_local_server"
			if (ID.compare("/udp_local_server") == 0) {
				break;
			}
			else if (ID.compare("server") != 0) {
				sprintf(ErrorStr, "Error: incorrect configuration file expected string \"server\" not found! (instead \"%s\")", ID.c_str()); ReportError(ErrorStr); throw 100;
			}
		}
	}

	//// /udp_local_server
	//std::getline(confFin, line);

	// udp_remote_client Number of clients
	std::getline(confFin, line);
	stringstreamStoreLine(&sstream, &line);
	sstream >> ID;
	if (ID.compare("udp_remote_client") != 0) {
		sprintf(ErrorStr, "Error: incorrect configuration file expected string \"udp_remote_client\" not found! (instead \"%s\")", ID.c_str()); ReportError(ErrorStr); throw 100;
	}

	// VERBATIM
	std::getline(confFin, line);
	// TYPE
	std::getline(confFin, line);
	// ID
	std::getline(confFin, line);

	if (indConfiguration == 0) {
		while (true) {
			std::getline(confFin, line);
			stringstreamStoreLine(&sstream, &line);
			sstream >> ID; // string "client" or end with "/udp_remote_client"
			if (ID.compare("/udp_remote_client") == 0) {
				break;
			}
			else if (ID.compare("client") != 0) {
				sprintf(ErrorStr, "Error: incorrect configuration file expected string \"client\" not found! (instead \"%s\")", ID.c_str()); ReportError(ErrorStr); throw 100;
			}
			pg_IPClient* client = new pg_IPClient();
			sstream >> client->id;
			sstream >> client->Remote_server_IP;
			sstream >> client->Remote_server_port;
			sstream >> ID;
			for (int ind = 0; ind < Emptypg_UDPMessageFormat; ind++) {
				if (strcmp(ID.c_str(), pg_UDPMessageFormatString[ind]) == 0) {
					client->send_format = (pg_UDPMessageFormat)ind;
					break;
				}
			}
			if (client->send_format == Emptypg_UDPMessageFormat) {
				sprintf(ErrorStr, "Error: unknown receive message format [%s]!", ID.c_str()); ReportError(ErrorStr); throw 249;
			}
			sstream >> client->IP_message_trace;
			sstream >> client->max_depth_output_stack;
			sstream >> client->OSC_endian_reversal;
			// std::cout << "OSC_trace: " << client->IP_message_trace << "\n";
			//std::cout << "client->id: " << client->id << "\n";
			//std::cout << "client->Remote_server_IP: " << client->Remote_server_IP << "\n";
			//std::cout << "client->Remote_server_port: " << client->Remote_server_port << "\n";
			IP_Clients.push_back(client);
		}
	}
	else {
		while (true) {
			std::getline(confFin, line);
			stringstreamStoreLine(&sstream, &line);
			sstream >> ID; // string "client" or end with "/udp_remote_client"
			if (ID.compare("/udp_remote_client") == 0) {
				break;
			}
			else if (ID.compare("client") != 0) {
				sprintf(ErrorStr, "Error: incorrect configuration file expected string \"client\" not found! (instead \"%s\")", ID.c_str()); ReportError(ErrorStr); throw 100;
			}
		}
	}

	//// /udp_remote_client
	//std::getline(confFin, line);

	////////////////////////////////////
	////// CAMERAS
	////////////////////////////////////

	std::getline(confFin, line);
	stringstreamStoreLine(&sstream, &line);
	sstream >> ID;
	if (ID.compare("webCam") != 0) {
		sprintf(ErrorStr, "Error: incorrect configuration file expected string \"webCam\" not found! (instead \"%s\")", ID.c_str()); ReportError(ErrorStr); throw 100;
	}

	// VERBATIM
	std::getline(confFin, line);
	// TYPE
	std::getline(confFin, line);
	// ID
	std::getline(confFin, line);

	if (indConfiguration == 0) {
		while (true) {
			std::getline(confFin, line);
			stringstreamStoreLine(&sstream, &line);
			sstream >> ID; // string "client" or end with "/udp_remote_client"
			if (ID.compare("/webCam") == 0) {
				break;
			}
			else if (ID.compare("camera") != 0) {
				sprintf(ErrorStr, "Error: incorrect configuration file expected string \"camera\" not found! (instead \"%s\")", ID.c_str()); ReportError(ErrorStr); throw 100;
			}
			webCam *cur_webCam = new webCam();

			sstream >> cur_webCam->cameraString; // string "cameraString"

			sstream >> temp; // string "cameraID"
			try {
				cur_webCam->cameraID = stoi(temp);
			}
			catch (...) {
				sprintf(ErrorStr, "Error: webcam configuration incorrect cameraID \"%s\"\n", temp.c_str()); ReportError(ErrorStr); throw 50;
			}

			sstream >> temp2; // string "cameraWidth"
			try {
				cur_webCam->cameraWidth = stoi(temp2);
			}
			catch (...) {
				sprintf(ErrorStr, "Error: webcam configuration incorrect cameraWidth \"%s\"\n", temp2.c_str()); ReportError(ErrorStr); throw 50;
			}

			sstream >> temp3; // string "cameraHeight"
			try {
				cur_webCam->cameraHeight = stoi(temp3);
			}
			catch (...) {
				sprintf(ErrorStr, "Error: webcam configuration incorrect cameraHeight \"%s\"\n", temp3.c_str()); ReportError(ErrorStr); throw 50;
			}		//std::cout << temp2 << "\n";
			pg_webCams.push_back(cur_webCam);
		}
	}
	else {
		while (true) {
			std::getline(confFin, line);
			stringstreamStoreLine(&sstream, &line);
			sstream >> ID; // string "client" or end with "/udp_remote_client"
			if (ID.compare("/webCam") == 0) {
				break;
			}
			else if (ID.compare("camera") != 0) {
				sprintf(ErrorStr, "Error: incorrect configuration file expected string \"camera\" not found! (instead \"%s\")", ID.c_str()); ReportError(ErrorStr); throw 100;
			}
		}
	}

	// remote_IPCam Number of cameras
	std::getline(confFin, line);
	stringstreamStoreLine(&sstream, &line);
	sstream >> ID;
	if (ID.compare("remote_IPCam") != 0) {
		sprintf(ErrorStr, "Error: incorrect configuration file expected string \"remote_IPCam\" not found! (instead \"%s\")", ID.c_str()); ReportError(ErrorStr); throw 100;
	}

	// VERBATIM
	std::getline(confFin, line);
	// TYPE
	std::getline(confFin, line);
	// ID
	std::getline(confFin, line);

	if (indConfiguration == 0) {
		while (true) {
			std::getline(confFin, line);
			stringstreamStoreLine(&sstream, &line);
			sstream >> ID; // string "client" or end with "/udp_remote_client"
			if (ID.compare("/remote_IPCam") == 0) {
				break;
			}
			else if (ID.compare("IPCam") != 0) {
				sprintf(ErrorStr, "Error: incorrect configuration file expected string \"IPCam\" not found! (instead \"%s\")", ID.c_str()); ReportError(ErrorStr); throw 100;
			}
			VideoCapture cur_IPCam;

			std::getline(confFin, line);
			stringstreamStoreLine(&sstream, &line);
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
			pg_IPCam_capture_address.push_back("rtsp://" + temp + ":" + temp2 + "/" + temp3);
			pg_IPCam_capture.push_back(cur_IPCam);
		}
	}
	else {
		while (true) {
			std::getline(confFin, line);
			stringstreamStoreLine(&sstream, &line);
			sstream >> ID; // string "client" or end with "/udp_remote_client"
			if (ID.compare("/remote_IPCam") == 0) {
				break;
			}
			else if (ID.compare("IPCam") != 0) {
				sprintf(ErrorStr, "Error: incorrect configuration file expected string \"IPCam\" not found! (instead \"%s\")", ID.c_str()); ReportError(ErrorStr); throw 100;
			}
		}
	}

	////////////////////////////////////
	////// SHADERS
	////////////////////////////////////

	// shader file names
	pg_Shader_File_Names[indConfiguration] = NULL;

	// shader_files Number of files<
	std::getline(confFin, line);
	stringstreamStoreLine(&sstream, &line);
	sstream >> ID;
	if (ID.compare("shader_files") != 0) {
		sprintf(ErrorStr, "Error: incorrect configuration file expected string \"shader_files<\" not found! (instead \"%s\")", ID.c_str()); ReportError(ErrorStr); throw 100;
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
		if (pg_ScenarioActiveVars[_part_initialization][indConfiguration]) {
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
		if (pg_ScenarioActiveVars[_sensor_layout][indConfiguration]) {
			if (ind_shader_type == _pg_shader_Sensor
				&& (pg_Shader_nbStages[indConfiguration][ind_shader_type] == 0
					|| pg_Shader_File_Names[indConfiguration][ind_shader_type] == "NULL")) {
				sprintf(ErrorStr, "Error: active shader file for Sensors is missing in header file (name %s/configuration #%d/%s, nb stages %d)",
					pg_Shader_File_Names[indConfiguration][ind_shader_type].c_str(), indConfiguration, pg_ConfigurationFileNames[indConfiguration].c_str(),
					pg_Shader_nbStages[indConfiguration][ind_shader_type]); ReportError(ErrorStr); throw(6778);
				printf("Particle aniation shader, ");
			}
		}
		if (pg_ScenarioActiveVars[_activeClipArts][indConfiguration]) {
			if (ind_shader_type == _pg_shader_ClipArt
				&& (pg_Shader_nbStages[indConfiguration][ind_shader_type] == 0
					|| pg_Shader_File_Names[indConfiguration][ind_shader_type] == "NULL")) {
				sprintf(ErrorStr, "Error: active shader file for Clip Arts is missing in header file (name %s, nb stages%d)", pg_Shader_File_Names[indConfiguration][ind_shader_type].c_str(), pg_Shader_nbStages[indConfiguration][ind_shader_type]); ReportError(ErrorStr); throw(6778);
				printf("Particle aniation shader, ");
			}
		}
#if defined(var_activeMeshes) 
		if (pg_ScenarioActiveVars[_activeMeshes][indConfiguration]) {
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
	// default value
	texDataScreenFont.texture_fileName = "./Data/fonts/usascii/arial/stb_font_arial_15_usascii.png";
	scripts_directory = "./Data/core-data/scripts/";
	videos_directory = "./Data/core-data/videos/";
	soundtracks_directory = "./Data/core-data/soundtracks/";
	cliparts_directory = "./Data/core-data/ClipArts/";
	diaporamas_directory = "./Data/core-data/images/";
	meshes_directory = "./Data/core-data/meshes/";
	messages_directory = "./Data/core-data/messages/";
	SVGpaths_directory = "./Data/core-data/SVGs/";
	textures_directory = "./Data/core-data/textures/";
	while (true) {
		std::getline(confFin, line);
		stringstreamStoreLine(&sstream, &line);
		sstream >> ID; // string "client" or end with "/udp_remote_client"
		if (ID.compare("/directories") == 0) {
			break;
		}
		else if (ID.compare("snapshots") == 0) {
			if (indConfiguration == 0) {
				sstream >> snapshots_dir_path_prefix; // string of the snapshots and log file directory
			}
		}
		else if (ID.compare("screen_font") == 0) {
			if (indConfiguration == 0) {
				sstream >> texDataScreenFont.texture_fileName;
				completeToFullPath(texDataScreenFont.texture_fileName);
			}
		}
		else if (ID.compare("scripts") == 0) {
			if (indConfiguration == 0) {
				sstream >> scripts_directory;
				completeToFullPath(scripts_directory);
			}
		}
		else if (ID.compare("videos") == 0) {
			if (indConfiguration == 0) {
				sstream >> videos_directory;
				completeToFullPath(videos_directory);
			}
		}
		else if (ID.compare("soundtracks") == 0) {
			if (indConfiguration == 0) {
				sstream >> soundtracks_directory;
				completeToFullPath(soundtracks_directory);
			}
		}
		else if (ID.compare("cliparts") == 0) {
			if (indConfiguration == 0) {
				sstream >> cliparts_directory;
				completeToFullPath(cliparts_directory);
			}
		}
		else if (ID.compare("diaporamas") == 0) {
			if (indConfiguration == 0) {
				sstream >> diaporamas_directory;
				completeToFullPath(diaporamas_directory);
			}
		}
		else if (ID.compare("meshes") == 0) {
			if (indConfiguration == 0) {
				sstream >> meshes_directory;
				completeToFullPath(meshes_directory);
			}
		}
		else if (ID.compare("messages") == 0) {
			if (indConfiguration == 0) {
				sstream >> messages_directory;
				completeToFullPath(messages_directory);
			}
		}
		else if (ID.compare("SVGpaths") == 0) {
			if (indConfiguration == 0) {
				sstream >> SVGpaths_directory;
				completeToFullPath(SVGpaths_directory);
			}
		}
		else if (ID.compare("textures") == 0) {
			if (indConfiguration == 0) {
				sstream >> textures_directory;
				completeToFullPath(textures_directory);
			}
		}
		else if (ID.compare("clips") == 0) {
			if (indConfiguration == 0) {
				sstream >> clips_directory;
				completeToFullPath(clips_directory);
			}
		}
		else if (ID.compare("shaders") == 0) {
			if (indConfiguration == 0) {
				sstream >> shaders_directory;
				completeToFullPath(shaders_directory);
			}
		}
		else {
			sprintf(ErrorStr, "Error: incorrect configuration \"%s\" is an unknown directory path type in configuration file (directories)", ID.c_str()); ReportError(ErrorStr); throw 100;
		}
	}

	// /head
	std::getline(confFin, line);

	texDataScreenFont.texture_fileNameSuffix = "";
	texDataScreenFont.texture_Is_Rectangle = true;
	texDataScreenFont.texture_Invert = false;
	texDataScreenFont.texture_Size_X = 128;
	texDataScreenFont.texture_Size_Y = 70;
	screen_font_size = 15;
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
	// Number of videos
	std::getline(scenarioFin, line);
	stringstreamStoreLine(&sstream, &line);
	sstream >> ID; // string videos
	if (ID.compare("videos") != 0) {
		sprintf(ErrorStr, "Error: incorrect configuration file expected string \"videos\" not found! (instead \"%s\")", ID.c_str()); ReportError(ErrorStr); throw 100;
	}

	while (true) {
		// new line
		std::getline(scenarioFin, line);
		stringstreamStoreLine(&sstream, &line);
		sstream >> ID; // string /svg_paths or svg_path
		if (ID.compare("/videos") == 0) {
			break;
		}
		else if (ID.compare("movie") != 0) {
			sprintf(ErrorStr, "Error: incorrect configuration file expected string \"movie\" not found! (instead \"%s\")", ID.c_str()); ReportError(ErrorStr); throw 100;
		}

		VideoTrack* video = new VideoTrack();

		sstream >> temp;
		video->videoFileName = temp;
		if (!isFullPath(video->videoFileName)) {
			video->videoFileName = videos_directory + video->videoFileName;
		}
		sstream >> temp2;
		video->videoShortName = temp2;

		// in addition to the movie name and short name, 2 additional
		// sound files can be provided that are synchronized with the 
		// movie and used to generate beats from sound envelope at 1
		// or sound onsets detected through aubio library

		// possible additional soundtrack file with peaked sound envelope at 1.0 or above
		// NULL value or no value means no file
		if (sstream >> temp2) {
			// there is a soundtrack file with peaked sound envelope at 1.0
			if (temp2.compare("") != 0 && temp2.compare("NULL") != 0) {
				video->videoSoundtrackPeaksFileName = temp2;
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
				video->videoSoundtrackPeaks = peak_times;
				//for (int i = 0; i < int(peak_times.size()); ++i) {
				//	std::cout << "Peak: " << peak_times[i] << '\n';
				//}
			}
			else {
				video->videoSoundtrackPeaksFileName = "";
				video->videoSoundtrackPeaks = {};
			}
			if (sstream >> temp2 && temp2.compare("") != 0 && temp2.compare("NULL") != 0) {
				video->videoSoundtrackOnsetsFileName = temp2;
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
				video->videoSoundtrackOnsets = onset_times;
				//for (int i = 0; i < int(onset_times.size()); ++i) {
				//	std::cout << "Onset: " << onset_times[i] << '\n';
				//}
			}
			else {
				video->videoSoundtrackOnsetsFileName = "";
				video->videoSoundtrackOnsets = {};
			}
		}
		else {
			video->videoSoundtrackPeaksFileName = "";
			video->videoSoundtrackPeaks = {};
			video->videoSoundtrackOnsetsFileName = "";
			video->videoSoundtrackOnsets = {};
		}
		//std::cout << "movie : " << 
		// movieFileName[indVideo] << "\n";
		pg_VideoTracks[indConfiguration].push_back(video);
	}
}

void ParseScenarioClipsAndPhotos(std::ifstream& scenarioFin, int indConfiguration) {
	std::stringstream  sstream;
	string line;
	string ID;
	string temp;

	////////////////////////////
	////// SHORT VIDEO CLIPS ALBUMS
	//pg_NbClipAlbums[indConfiguration] = 0;
	// Number of photo albums
	std::getline(scenarioFin, line);
	stringstreamStoreLine(&sstream, &line);
	sstream >> ID; // string videos
	if (ID.compare("clips") != 0) {
		sprintf(ErrorStr, "Error: incorrect configuration file expected string \"clips\" not found! (instead \"%s\")", ID.c_str()); ReportError(ErrorStr); throw 100;
	}
	//sstream >> pg_NbClipAlbums[indConfiguration];

	//printf("Clip album Number %d\n", pg_NbClipAlbums);

	int nbClipAlbums = 0;
	pg_ClipDirectory[indConfiguration] = "";
	while (true) {
		// new line
		std::getline(scenarioFin, line);
		stringstreamStoreLine(&sstream, &line);
		sstream >> ID; // string /svg_paths or svg_path
		if (ID.compare("/clips") == 0) {
			break;
		}
		else if (ID.compare("album") != 0) {
			sprintf(ErrorStr, "Error: incorrect configuration file expected string \"album\" not found! (instead \"%s\")", ID.c_str()); ReportError(ErrorStr); throw 100;
		}

		if (nbClipAlbums > 1) {
			sprintf(ErrorStr, "Error: incorrect configuration file: Only one clip album with subdirectories is expected!"); ReportError(ErrorStr); throw 100;
		}

		sstream >> pg_ClipDirectory[indConfiguration];
		if (!isFullPath(pg_ClipDirectory[indConfiguration])) {
			pg_ClipDirectory[indConfiguration] = clips_directory + pg_ClipDirectory[indConfiguration];
		}
		std::cout << "Clip album directory: " << pg_ClipDirectory << "\n";
		sstream >> clip_image_width[indConfiguration];
		sstream >> clip_image_height[indConfiguration];
		sstream >> clip_crop_width[indConfiguration];
		sstream >> clip_crop_height[indConfiguration];
		sstream >> clip_max_length[indConfiguration];
		if (clip_image_width[indConfiguration] == 0 || clip_image_height[indConfiguration] == 0 || clip_crop_width[indConfiguration] == 0 || clip_crop_height[indConfiguration] == 0 || clip_max_length[indConfiguration] == 0) {
			sprintf(ErrorStr, "Error: incorrect configuration file: missing dimension data for clip image size %dx%d cropped size %dx%d max length %d\n", clip_image_width[indConfiguration], clip_image_height[indConfiguration], clip_crop_width[indConfiguration], clip_crop_height[indConfiguration], clip_max_length[indConfiguration]); ReportError(ErrorStr); throw 100;
		}

		nbClipAlbums;
	}
	pg_NbClipAlbums[indConfiguration] = nbClipAlbums;

	////////////////////////////
	////// PHOTO ALBUMS
	//pg_NbPhotoAlbums[indConfiguration] = 0;
	// Number of photo albums
	std::getline(scenarioFin, line);
	stringstreamStoreLine(&sstream, &line);
	sstream >> ID; // string videos
	if (ID.compare("photos") != 0) {
		sprintf(ErrorStr, "Error: incorrect configuration file expected string \"photos\" not found! (instead \"%s\")", ID.c_str()); ReportError(ErrorStr); throw 100;
	}

	int nbPhotoAlbums = 0;
	string photoAlbumDir = "";
	while (true) {
		// new line
		std::getline(scenarioFin, line);
		stringstreamStoreLine(&sstream, &line);
		sstream >> ID; // string /svg_paths or svg_path
		if (ID.compare("/photos") == 0) {
			break;
		}
		else if (ID.compare("album") != 0) {
			sprintf(ErrorStr, "Error: incorrect configuration file expected string \"album\" not found! (instead \"%s\")", ID.c_str()); ReportError(ErrorStr); throw 100;
		}

		if (nbPhotoAlbums > 1) {
			sprintf(ErrorStr, "Error: incorrect configuration file: Only one photo album with subdirectories is expected"); ReportError(ErrorStr); throw 100;
		}

		sstream >> photoAlbumDir;
		//std::cout << "photo album : " << photoAlbumDir << "\n";

		nbPhotoAlbums++;
	}

	if (photoAlbumDir != "") {
		if (!isFullPath(photoAlbumDir)) {
			pg_ImageDirectory[indConfiguration] = diaporamas_directory + photoAlbumDir;
		}
		else {
			pg_ImageDirectory[indConfiguration] = photoAlbumDir;
		}
		//std::cout << "Photo album directory: " << pg_ImageDirectory[indConfiguration] << std::endl;
	}
	else {
		pg_ImageDirectory[indConfiguration] = "";
	}

	std::getline(scenarioFin, line);
	stringstreamStoreLine(&sstream, &line);
	sstream >> ID; // string messages
	if (ID.compare("messages") != 0) {
		sprintf(ErrorStr, "Error: incorrect configuration file expected string \"messages\" not found! (instead \"%s\")", ID.c_str()); ReportError(ErrorStr); throw 100;
	}

	std::getline(scenarioFin, line);
	stringstreamStoreLine(&sstream, &line);
	sstream >> ID; // string lines
	if (ID.compare("/messages") != 0) {
		sstream >> pg_MessageFile[indConfiguration];
		if (!isFullPath(pg_MessageFile[indConfiguration])) {
			pg_MessageFile[indConfiguration] = messages_directory + pg_MessageFile[indConfiguration];
		}
		std::cout << "Loading messages from " << pg_MessageFile[indConfiguration] << std::endl;

		// /messages
		std::getline(scenarioFin, line);
		stringstreamStoreLine(&sstream, &line);
		sstream >> ID; // string /messages
		if (ID.compare("/messages") != 0) {
			sprintf(ErrorStr, "Error: incorrect configuration file expected string \"/messages\" not found! (instead \"%s\")", ID.c_str()); ReportError(ErrorStr); throw 100;
		}
	}
	else {
		pg_MessageFile[indConfiguration] = "";
	}

}

void ParseScenarioSoundtracks(std::ifstream& scenarioFin, int indConfiguration) {
	std::stringstream  sstream;
	string line;
	string ID;
	string temp;
	string temp2;

	////////////////////////////
	////// SOUNDTRACKS
	std::getline(scenarioFin, line);
	stringstreamStoreLine(&sstream, &line);
	sstream >> ID; // string soundtracks
	if (ID.compare("soundtracks") != 0) {
		sprintf(ErrorStr, "Error: incorrect configuration file expected string \"soundtracks\" not found! (instead \"%s\")", ID.c_str()); ReportError(ErrorStr); throw 100;
	}

	while (true) {
		// new line
		std::getline(scenarioFin, line);
		stringstreamStoreLine(&sstream, &line);
		sstream >> ID; // string /svg_paths or svg_path
		if (ID.compare("/soundtracks") == 0) {
			break;
		}
		else if (ID.compare("track") != 0) {
			sprintf(ErrorStr, "Error: incorrect configuration file expected string \"track\" not found! (instead \"%s\")", ID.c_str()); ReportError(ErrorStr); throw 100;
		}

		SoundTrack *soundtrack = new SoundTrack();

		sstream >> temp;
		soundtrack->soundtrackFileName = temp;
		if (!isFullPath(soundtrack->soundtrackFileName)) {
			soundtrack->soundtrackFileName = soundtracks_directory + soundtrack->soundtrackFileName;
		}
		sstream >> temp2;
		soundtrack->soundtrackShortName = temp2;
		//std::cout << "Soundtrack: " << pg_SoundTracks[indConfiguration][indTrack]->soundtrackFileName << " " << pg_SoundTracks[indConfiguration][indTrack]->trackShortName << " (#" << indTrack << ")\n";

		// in addition to the track name and short name, 2 additional
		// files can be used to generate beats from sound envelope at 1
		// or sound onsets detected through aubio library

		// possible additional peaked sound envelope at 1.0 or above
		// NULL value or no value means no file
		if (sstream >> temp2) {
			// there is a soundtrack file with peaked sound envelope at 1.0
			if (temp2.compare("") != 0 && temp2.compare("NULL") != 0) {
				soundtrack->soundtrackPeaksFileName = temp2;
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
				soundtrack->soundtrackPeaks = peak_times;;
				//for (int i = 0; i < int(peak_times.size()); ++i) {
				//	std::cout << "Peak: " << peak_times[i] << '\n';
				//}
			}
			else {
				soundtrack->soundtrackPeaksFileName = "";
				soundtrack->soundtrackPeaks = {};
			}
			if (sstream >> temp2 && temp2.compare("") != 0 && temp2.compare("NULL") != 0) {
				soundtrack->soundtrackOnsetsFileName = temp2;
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
				soundtrack->soundtrackOnsets = onset_times;;
				//for (int i = 0; i < int(onset_times.size()); ++i) {
				//	std::cout << "Onset: " << onset_times[i] << '\n';
				//}
			}
			else {
				soundtrack->soundtrackOnsetsFileName = "";
				soundtrack->soundtrackOnsets = {};
			}
			float offset = 0.f;
			sstream >> offset;
			soundtrack->soundtrackOnsetsAndPeasksOffset = offset;
		}
		else {
			soundtrack->soundtrackPeaksFileName = "";
			soundtrack->soundtrackPeaks = {};
			soundtrack->soundtrackOnsetsFileName = "";
			soundtrack->soundtrackOnsets = {};
			soundtrack->soundtrackOnsetsAndPeasksOffset = 0.f;
		}
		//std::cout << "track : " << 
		// pg_SoundTracks[indVideo]->soundtrackFileName << "\n";
		pg_SoundTracks[indConfiguration].push_back(soundtrack);
	}
}

void ParseScenarioSVGPaths(std::ifstream& scenarioFin, int indConfiguration) {
	std::stringstream  sstream;
	string line;
	string ID;
	string temp;
	string temp2;

	////////////////////////////
	////// SVG PATHS

	// Initial markup for SVG pathCurves
	std::getline(scenarioFin, line);
	stringstreamStoreLine(&sstream, &line);
	sstream >> ID; // string svg_paths
	if (ID.compare("svg_paths") != 0) {
		sprintf(ErrorStr, "Error: incorrect configuration file expected string \"svg_paths\" not found! (instead \"%s\")", ID.c_str()); ReportError(ErrorStr); throw 100;
	}

	// initializes the tracks for recording the strokes
	// has to be made before loading the scenario that may load predefined svg paths
	// and has to be made after the configuration file loading that has 
	// the value max_mouse_recording_frames (obsolete with vectors) for the max number of points to memorize

	// mouse pointer tracks recording initialization
	// printf("Path initialization size %d\n", max_mouse_recording_frames (obsolete with vectors));
	for (int ind = 0; ind <= PG_NB_PATHS; ind++) {
		synchr_start_recording_path[ind] = false;
		synchr_start_path_replay_trackNo[ind] = -1;
		recorded_path[ind] = false;
		is_path_replay[ind] = false;
	}

	// each path should have minimally one curve which is used to record/replay live
	for (int pathNo = 1; pathNo <= PG_NB_PATHS; pathNo++) {
		// a path without curves
		if (pg_Path_Status[pathNo].path_PathCurve_Data[indConfiguration].size() == 0) {
			PathCurve_Data curve;
			curve.PathCurve_Data_init();
			pg_Path_Status[pathNo].path_PathCurve_Data[indConfiguration].push_back(curve);
		}
	}

	// parses the full list of paths 
	// check which paths have associated curves and how many pathCurves are associated to each active path
	pg_current_SVG_path_group = 1;
	int nb_path_curves = 0;
	while(true) {
		string fileName = "";
		string local_ID = "";
		string path_ID = "";

		// adds a new curve
		SVG_scenarioPathCurves[indConfiguration].push_back(new SVG_scenarioPathCurve);

		// new line
		std::getline(scenarioFin, line);
		stringstreamStoreLine(&sstream, &line);
		sstream >> ID; // string /svg_paths or svg_path
		if (ID.compare("/svg_paths") == 0) {
			break;
		}
		else if (ID.compare("svg_path") != 0) {
			sprintf(ErrorStr, "Error: incorrect configuration file expected string \"svg_path\" not found! (instead \"%s\")", ID.c_str()); ReportError(ErrorStr); throw 100;
		}

		sstream >> fileName; // file name
		if (!isFullPath(fileName)) {
			fileName = SVGpaths_directory + fileName;
		}
		//printf("Filename %s\n", fileName.c_str());
		sstream >> temp2;
		int pathNo = my_stoi(temp2);
		// adds a new curve to the path, the curve is made of one empty frame 
		// the addition of a new frame is made by filling the back frame and pushing a new one when another one is built
		if (pathNo <= PG_NB_PATHS && pathNo >= 0) {
			PathCurve_Data curve;
			curve.PathCurve_Data_init();
			pg_Path_Status[pathNo].path_PathCurve_Data[indConfiguration].push_back(curve);
		}
		else {
			sprintf(ErrorStr, "Error: incorrect scenario file SVG path %d number (\"%s\"), pathRank should be between 0 and %d",
				pathNo, fileName.c_str(), PG_NB_PATHS); ReportError(ErrorStr); throw 100;
		}
		sstream >> temp2;
		int path_track = my_stoi(temp2);
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
		sstream >> path_ID;
		sstream >> temp2;
		int local_path_group = my_stoi(temp2);
		if (local_path_group <= 0) {
			sprintf(ErrorStr, "Error: incorrect scenario file group %d for SVG path %d number (\"%s\"), path group should be stricly positive",
				local_path_group, pathNo, fileName.c_str()); ReportError(ErrorStr); throw 100;
		}
		else {
			pg_nb_SVG_path_groups[indConfiguration] = max(pg_nb_SVG_path_groups[indConfiguration], local_path_group);
		}
		sstream >> temp2;
		bool with_color_radius_from_scenario = (my_stoi(temp2) != 0);
		sstream >> temp2;
		double secondsforwidth = my_stod(temp2);

		//printf("path no %d group %d\n", indPathCurve, local_path_group);
		//printf("path ID %s radius %.3f\n", path_ID.c_str(), pathRadius);
		//printf("path no %d group %d\n", paths[indConfiguration][indPathCurve].indPathCurve, paths[indConfiguration][indPathCurve].path_group);
		// checks whether there are other curves in the same path
		int rankInPath = 0;
		for (int indAux = 0; indAux < nb_path_curves; indAux++) {
			if (SVG_scenarioPathCurves[indConfiguration][indAux]->path_no == SVG_scenarioPathCurves[indConfiguration][nb_path_curves]->path_no) {
				rankInPath++;
				//sprintf(ErrorStr, "Error: incorrect configuration file paths %d and %d have the same path index %d and same path group %d", 
				//	indAux, indPathCurve, SVG_scenarioPathCurves[indConfiguration][indAux]->indPath, SVG_scenarioPathCurves[indConfiguration][indAux]->path_group); ReportError(ErrorStr); throw 100;
			}
		}
		SVG_scenarioPathCurves[indConfiguration][nb_path_curves]->SVG_scenarioPathCurve_init(pathNo, rankInPath, path_track, pathRadius,
			path_r_color, path_g_color, path_b_color, path_readSpeedScale,
			path_ID, fileName, local_path_group, with_color_radius_from_scenario, secondsforwidth);

		//printf("indPathCurve %d path_track %d pathRadius %.2f path_r_color %.2f path_g_color %.2f path_b_color %.2f path_readSpeedScale %.2f\n",
		//	pathRank, path_track, pathRadius, path_r_color, path_g_color, path_b_color, path_readSpeedScale);
		if (path_track >= 0 && path_track < PG_NB_TRACKS && pathNo >= 1 && pathNo <= PG_NB_PATHS) {
			if (local_path_group == pg_current_SVG_path_group) {
				if (pg_ScenarioActiveVars[_path_replay_trackNo][indConfiguration] 
					&& pg_ScenarioActiveVars[_path_record][indConfiguration]) {
					//printf("Load svg path No %d track %d\n", pathNo, path_track);
					pg_Path_Status[pathNo].load_svg_path((char*)fileName.c_str(),
						pathRadius, path_r_color, path_g_color, path_b_color,
						path_readSpeedScale, path_ID, with_color_radius_from_scenario, secondsforwidth, indConfiguration);
					//printf("time stamps %.2f %.2f %.2f %.2f %.2f\n",
					//	pg_Path_Status[pathNo].path_TmpTimeStamps[0], pg_Path_Status[pathNo].path_TmpTimeStamps[1], 
					//	pg_Path_Status[pathNo].path_TmpTimeStamps[2], pg_Path_Status[pathNo].path_TmpTimeStamps[3], 
					//	pg_Path_Status[pathNo].path_TmpTimeStamps[4]);
				}
			}
		}
		else {
			sprintf(ErrorStr, "Error: incorrect scenario file track %d for SVG path number %d (\"%s\") track number should be between 0 and %d and path number between 1 and %d\n",
				path_track, pathNo, fileName.c_str(), PG_NB_TRACKS, PG_NB_PATHS); ReportError(ErrorStr); throw 100;
		}
		//std::cout << "svg_path #" << pathNo << ": " << SVGpaths_directory + temp << " track #" << path_track << "\n";

		nb_path_curves++;
	}
}

void pg_listAllSVG_paths(void) {
	printf("Listing SVG paths:\n");
	for (int indConfiguration = 0; indConfiguration < pg_NbConfigurations; indConfiguration++) {
		std::cout << "    " << indConfiguration << ": ";
		for (SVG_scenarioPathCurve* curve : SVG_scenarioPathCurves[indConfiguration]) {
			if (pg_ScenarioActiveVars[_path_replay_trackNo][indConfiguration] && pg_ScenarioActiveVars[_path_record][indConfiguration]) {
				std::cout << curve->path_fileName << " (" << curve->path_no << ", "
					<< curve->path_group << "), ";
			}
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

void pg_listAllSoundtracks(void) {
	printf("Listing Soundtracks:\n");
	for (int indConfiguration = 0; indConfiguration < pg_NbConfigurations; indConfiguration++) {
		std::cout << "    " << indConfiguration << ": ";
		for(SoundTrack * soundtrack : pg_SoundTracks[indConfiguration]) {
			std::cout << soundtrack->soundtrackFileName << " (" << soundtrack->soundtrackShortName << "), ";
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
	std::getline(scenarioFin, line);
	stringstreamStoreLine(&sstream, &line);
	sstream >> ID; // string clip_arts
	if (ID.compare("clip_arts") != 0) {
		sprintf(ErrorStr, "Error: incorrect configuration file expected string \"clip_arts\" not found! (instead \"%s\")", ID.c_str()); ReportError(ErrorStr); throw 100;
	}

	pg_nb_tot_SvgGpu_paths[indConfiguration] = 0;
	while (true) {
		// new line
		std::getline(scenarioFin, line);
		stringstreamStoreLine(&sstream, &line);
		sstream >> ID; // string /svg_paths or svg_path
		if (ID.compare("/clip_arts") == 0) {
			break;
		}
		else if (ID.compare("clip_art") != 0) {
			sprintf(ErrorStr, "Error: incorrect configuration file expected string \"clip_art\" not found! (instead \"%s\")", ID.c_str()); ReportError(ErrorStr); throw 100;
		}

		// adds a new clipart
		ClipArt* aClipArt = new ClipArt();

		sstream >> aClipArt->pg_ClipArt_fileNames; // file name
		if (!isFullPath(aClipArt->pg_ClipArt_fileNames)) {
			//printf("CipArts dir [%s] filename [%s]\n", cliparts_directory.c_str(), aClipArt->pg_ClipArt_fileNames.c_str());
			aClipArt->pg_ClipArt_fileNames = cliparts_directory + aClipArt->pg_ClipArt_fileNames;
		}
		sstream >> aClipArt->pg_nb_paths_in_ClipArt; // number of paths in the file
		pg_nb_tot_SvgGpu_paths[indConfiguration] += aClipArt->pg_nb_paths_in_ClipArt;
		//printf("%s, ", aClipArt->pg_ClipArt_fileNames.c_str());
		//printf("ind path file %d name %s nb paths %d, ", indClipArtFile, aClipArt->pg_ClipArt_fileNames.c_str(), aClipArt->pg_nb_paths_in_ClipArt);

		aClipArt->pg_ClipArt_SubPath = new bool[aClipArt->pg_nb_paths_in_ClipArt];
		for (int indPathCurve = 0; indPathCurve < aClipArt->pg_nb_paths_in_ClipArt; indPathCurve++) {
			aClipArt->pg_ClipArt_SubPath[indPathCurve] = true;
		}

		aClipArt->pg_ClipArt_Colors = new pg_ClipArt_Colors_Types[aClipArt->pg_nb_paths_in_ClipArt];
		for (int indPathCurve = 0; indPathCurve < aClipArt->pg_nb_paths_in_ClipArt; indPathCurve++) {
			aClipArt->pg_ClipArt_Colors[indPathCurve] = ClipArt_nat;
		}

		// image initial geometry
		sstream >> aClipArt->pg_ClipArt_Scale;
		sstream >> aClipArt->pg_ClipArt_Translation_X;
		sstream >> aClipArt->pg_ClipArt_Translation_Y;
		//printf("ind clipart %d scale %.2f pos %.2f %.2f\n", indClipArtFile, aClipArt->pg_ClipArt_Scale, aClipArt->pg_ClipArt_Translation_X, aClipArt->pg_ClipArt_Translation_Y);
		sstream >> aClipArt->pg_ClipArt_Rotation;
		sstream >> ID;
		if (ID.compare("nat") == 0) {
			for (int indPathCurve = 0; indPathCurve < aClipArt->pg_nb_paths_in_ClipArt; indPathCurve++) {
				aClipArt->pg_ClipArt_Colors[indPathCurve] = ClipArt_nat;
			}
		}
		else if (ID.compare("white") == 0) {
			for (int indPathCurve = 0; indPathCurve < aClipArt->pg_nb_paths_in_ClipArt; indPathCurve++) {
				aClipArt->pg_ClipArt_Colors[indPathCurve] = ClipArt_white;
			}
		}
		else if (ID.compare("red") == 0) {
			for (int indPathCurve = 0; indPathCurve < aClipArt->pg_nb_paths_in_ClipArt; indPathCurve++) {
				aClipArt->pg_ClipArt_Colors[indPathCurve] = ClipArt_red;
			}
		}
		else if (ID.compare("green") == 0) {
			for (int indPathCurve = 0; indPathCurve < aClipArt->pg_nb_paths_in_ClipArt; indPathCurve++) {
				aClipArt->pg_ClipArt_Colors[indPathCurve] = ClipArt_green;
			}
		}
		else if (ID.compare("blue") == 0) {
			for (int indPathCurve = 0; indPathCurve < aClipArt->pg_nb_paths_in_ClipArt; indPathCurve++) {
				aClipArt->pg_ClipArt_Colors[indPathCurve] = ClipArt_blue;
			}
		}
		else {
			sprintf(ErrorStr, "Error: incorrect configuration file ClipArt GPU color \"%s\" (nat, white, red, blue or green expected)", ID.c_str()); ReportError(ErrorStr); throw 100;
		}

		pg_ClipArts[indConfiguration].push_back(aClipArt);
	}
	printf("\n");
	printf("Nb clip arts %d config %d\n", pg_ClipArts[indConfiguration].size(), indConfiguration);
}

void ParseScenarioMeshes(std::ifstream& scenarioFin, int indConfiguration) {
	std::stringstream  sstream;
	string line;
	string ID;
	string temp;

#if defined(var_activeMeshes)
	if (pg_ScenarioActiveVars[_activeMeshes][indConfiguration]) {
		////////////////////////////
		////// MESHES
		// the meshes are loaded inside the GPU and diplayed depending on their activity
		std::getline(scenarioFin, line);
		stringstreamStoreLine(&sstream, &line);
		sstream >> ID; // string meshes
		if (ID.compare("meshes") != 0) {
			sprintf(ErrorStr, "Error: incorrect configuration file expected string \"meshes\" not found! (instead \"%s\")", ID.c_str()); ReportError(ErrorStr); throw 100;
		}
		while(true) {
			// adds a new mesh
			MeshData* aMesh = new MeshData(indConfiguration);

			// new line
			std::getline(scenarioFin, line);
			stringstreamStoreLine(&sstream, &line);
			sstream >> ID; // string /svg_paths or svg_path
			if (ID.compare("/meshes") == 0) {
				break;
			}
			else if (ID.compare("mesh") != 0) {
				sprintf(ErrorStr, "Error: incorrect configuration file expected string \"mesh\" not found! (instead \"%s\")", ID.c_str()); ReportError(ErrorStr); throw 100;
			}

			aMesh->pg_Mesh_Scale = 1.0f;
			sstream >> aMesh->pg_Mesh_fileNames; // file name
			// full path is not given, look in default local path
			if (!isFullPath(aMesh->pg_Mesh_fileNames)) {
				aMesh->pg_Mesh_fileNames
					= meshes_directory + aMesh->pg_Mesh_fileNames;
			}

			// image initial geometry
			sstream >> aMesh->pg_Mesh_Scale;
			sstream >> aMesh->pg_Mesh_Translation_X;
			sstream >> aMesh->pg_Mesh_Translation_Y;
			sstream >> aMesh->pg_Mesh_Translation_Z;
			sstream >> aMesh->pg_Mesh_Rotation_angle;
			sstream >> aMesh->pg_Mesh_Rotation_X;
			sstream >> aMesh->pg_Mesh_Rotation_Y;
			sstream >> aMesh->pg_Mesh_Rotation_Z;
			if (aMesh->pg_Mesh_Rotation_X == 0 && aMesh->pg_Mesh_Rotation_Y == 0 && aMesh->pg_Mesh_Rotation_Z == 0) {
				sprintf(ErrorStr, "Error: incorrect mesh %s configuration: rotation with Null axix ", aMesh->pg_Mesh_fileNames.c_str()); ReportError(ErrorStr); throw 100;
			}
			sstream >> aMesh->pg_Mesh_Motion_X;
			sstream >> aMesh->pg_Mesh_Motion_Y;
			sstream >> aMesh->pg_Mesh_Motion_Z;
			aMesh->pg_Mesh_Translation_Ini_X = aMesh->pg_Mesh_Translation_X;
			aMesh->pg_Mesh_Translation_Ini_Y = aMesh->pg_Mesh_Translation_Y;
			aMesh->pg_Mesh_Translation_Ini_Z = aMesh->pg_Mesh_Translation_Z;
			aMesh->pg_Mesh_Rotation_Ini_X = aMesh->pg_Mesh_Rotation_X;
			aMesh->pg_Mesh_Rotation_Ini_Y = aMesh->pg_Mesh_Rotation_Y;
			aMesh->pg_Mesh_Rotation_Ini_Z = aMesh->pg_Mesh_Rotation_Z;
			sstream >> ID;
			if (ID.compare("nat") == 0) {
				aMesh->pg_Mesh_Colors[0] = 1.f;
				aMesh->pg_Mesh_Colors[1] = 1.f;
				aMesh->pg_Mesh_Colors[2] = 1.f;
				aMesh->pg_Mesh_Colors[3] = 0.f;
			}
			else if (ID.compare("white") == 0) {
				aMesh->pg_Mesh_Colors[0] = 1.f;
				aMesh->pg_Mesh_Colors[1] = 1.f;
				aMesh->pg_Mesh_Colors[2] = 1.f;
				aMesh->pg_Mesh_Colors[3] = 1.f;
			}
			else if (ID.compare("red") == 0) {
				aMesh->pg_Mesh_Colors[0] = 1.f;
				aMesh->pg_Mesh_Colors[1] = 0.f;
				aMesh->pg_Mesh_Colors[2] = 0.f;
				aMesh->pg_Mesh_Colors[3] = 1.f;
			}
			else if (ID.compare("green") == 0) {
				aMesh->pg_Mesh_Colors[0] = 0.f;
				aMesh->pg_Mesh_Colors[1] = 1.f;
				aMesh->pg_Mesh_Colors[2] = 0.f;
				aMesh->pg_Mesh_Colors[3] = 1.f;
			}
			else if (ID.compare("blue") == 0) {
				aMesh->pg_Mesh_Colors[0] = 0.f;
				aMesh->pg_Mesh_Colors[1] = 0.f;
				aMesh->pg_Mesh_Colors[2] = 1.f;
				aMesh->pg_Mesh_Colors[3] = 1.f;
			}
			else if (ID.compare("cyan") == 0) {
				aMesh->pg_Mesh_Colors[0] = 0.f;
				aMesh->pg_Mesh_Colors[1] = 1.f;
				aMesh->pg_Mesh_Colors[2] = 1.f;
				aMesh->pg_Mesh_Colors[3] = 1.f;
			}
			else if (ID.compare("magenta") == 0) {
				aMesh->pg_Mesh_Colors[0] = 1.f;
				aMesh->pg_Mesh_Colors[1] = 0.f;
				aMesh->pg_Mesh_Colors[2] = 1.f;
				aMesh->pg_Mesh_Colors[3] = 1.f;
			}
			else if (ID.compare("yellow") == 0) {
				aMesh->pg_Mesh_Colors[0] = 1.f;
				aMesh->pg_Mesh_Colors[1] = 1.f;
				aMesh->pg_Mesh_Colors[2] = 0.f;
				aMesh->pg_Mesh_Colors[3] = 1.f;
			}
			else if (ID.compare("black") == 0) {
				aMesh->pg_Mesh_Colors[0] = 0.f;
				aMesh->pg_Mesh_Colors[1] = 0.f;
				aMesh->pg_Mesh_Colors[2] = 0.f;
				aMesh->pg_Mesh_Colors[3] = 1.f;
			}
			else {
				sprintf(ErrorStr, "Error: incorrect configuration file Mesh color \"%s\" (nat, white, cyan, yellow, magenta, red, blue, or greeen expected)", ID.c_str()); ReportError(ErrorStr); throw 100;
			}
			sstream >> aMesh->pg_Mesh_TextureRank;
			//printf("Mesh #%d scale %.2f translation (%.2f,%.2f,%.2f), rotation %.2f\n",
			//	indMeshFile, aMesh->pg_Mesh_Scale, aMesh->pg_Mesh_Translation_X,
			//	aMesh->pg_Mesh_Translation_Y, aMesh->pg_Mesh_Translation_Z,
			//	aMesh->pg_Mesh_Rotation_angle);
			// the rank of the mesh textures applied to this mesh
#if defined(var_MmeShanghai_brokenGlass)
			if (pg_ScenarioActiveVars[_MmeShanghai_brokenGlass][indConfiguration]) {
				sstream >> aMesh->pg_MmeShanghai_NbMeshSubParts;
				aMesh->pg_MmeShanghai_MeshSubPart_FileNames = new string[aMesh->pg_MmeShanghai_NbMeshSubParts];
				aMesh->pg_MmeShanghai_MeshSubParts = new bool* [aMesh->pg_MmeShanghai_NbMeshSubParts];
				for (int indPart = 0; indPart < aMesh->pg_MmeShanghai_NbMeshSubParts; indPart++) {
					sstream >> aMesh->pg_MmeShanghai_MeshSubPart_FileNames[indPart];
					aMesh->pg_MmeShanghai_MeshSubPart_FileNames[indPart]
						= meshes_directory + aMesh->pg_MmeShanghai_MeshSubPart_FileNames[indPart];
					aMesh->pg_MmeShanghai_MeshSubParts[indPart] = NULL;
				}
			}
#endif
			pg_Meshes[indConfiguration].push_back(aMesh);
		}
		// Augmented Reality: FBO capture of Master to be displayed on a mesh
#if defined(var_textureFrontier_wmin)&& defined(var_textureFrontier_wmax) and defined(var_textureFrontier_hmin) && defined(var_textureFrontier_hmax) && defined(var_textureFrontier_wmin_width) && defined(var_textureFrontier_wmax_width) and defined(var_textureFrontier_hmin_width) && defined(var_textureFrontier_hmax_width) && defined(var_textureScale_w) && defined(var_textureScale_h) and defined(var_textureTranslate_w) && defined(var_textureTranslate_h)
		if (pg_ScenarioActiveVars[_textureFrontier_wmin][indConfiguration]) {
			if (pg_Meshes[indConfiguration].empty()) {
				sprintf(ErrorStr, "Error: Augemented reality requires that at least one mesh file is declared in the scenario file"); ReportError(ErrorStr); throw 100;
			}
		}
#endif
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

	// initial markup
	std::getline(scenarioFin, line);
	stringstreamStoreLine(&sstream, &line);
	sstream >> ID; // string textures
	if (ID.compare("textures") != 0) {
		sprintf(ErrorStr, "Error: incorrect configuration file expected string \"textures\" not found! (instead \"%s\")", ID.c_str()); ReportError(ErrorStr); throw 100;
	}

	// Number of textures
	while(true) {
		std::getline(scenarioFin, line);
		stringstreamStoreLine(&sstream, &line);
		sstream >> ID; // string texture
		if (ID.compare("/textures") == 0) {
			break;
		}
		else if (ID.compare("texture") != 0) {
			sprintf(ErrorStr, "Error: incorrect configuration file expected string \"texture\" not found! (instead \"%s\")\n", ID.c_str()); ReportError(ErrorStr); throw 100;
		}

		pg_TextureData *cur_texture = new pg_TextureData();

		sstream >> ID; // file name
		if (!isFullPath(ID)) {
				cur_texture->texture_fileName = textures_directory + ID;
		}
		else {
			cur_texture->texture_fileName = ID;
		}
		sstream >> cur_texture->texture_fileNameSuffix; // file suffix

		// usage
		sstream >> ID;
		//printf("Usage %s: (%s)\n", cur_texture->texture_fileName.c_str(), ID.c_str());
		if (ID.compare("master_mask") == 0) {
			cur_texture->texture_usage = Texture_master_mask;
		}
		else if (ID.compare("mesh") == 0) {
			cur_texture->texture_usage = Texture_mesh;
		}
		else if (ID.compare("sensor") == 0) {
			cur_texture->texture_usage = Texture_sensor;
		}
		else if (ID.compare("logo") == 0) {
			cur_texture->texture_usage = Texture_logo;
		}
		else if (ID.compare("brush") == 0) {
			cur_texture->texture_usage = Texture_brush;
		}
		else if (ID.compare("noise") == 0) {
			cur_texture->texture_usage = Texture_noise;
		}
		else if (ID.compare("curve_particle") == 0) {
			cur_texture->texture_usage = Texture_curve_particle;
		}
		else if (ID.compare("splat_particle") == 0) {
			cur_texture->texture_usage = Texture_splat_particle;
		}
		else if (ID.compare("part_init") == 0) {
			cur_texture->texture_usage = Texture_part_init;
		}
		else if (ID.compare("part_acc") == 0) {
			cur_texture->texture_usage = Texture_part_acc;
		}
		else if (ID.compare("pixel_acc") == 0) {
			cur_texture->texture_usage = Texture_pixel_acc;
		}
		else if (ID.compare("repop_density") == 0) {
			cur_texture->texture_usage = Texture_repop_density;
		}
		else if (ID.compare("multilayer_master_mask") == 0) {
			cur_texture->texture_usage = Texture_multilayer_master_mask;
		}
		else {
			sprintf(ErrorStr, "Error: incorrect configuration file Texture usage \"%s\"\n", ID.c_str()); ReportError(ErrorStr); throw 100;
		}
		// rank of the texture (used in particular for meshes)
		sstream >> cur_texture->texture_Rank;
		// dimension (2 or 3)
		sstream >> cur_texture->texture_Dimension;
		if (cur_texture->texture_Dimension != 2
			&& cur_texture->texture_Dimension != 3) {
			sprintf(ErrorStr, "Error: 2D or 3D texture dimension expected, not %d for texture (%s)\n", cur_texture->texture_Dimension, cur_texture->texture_fileName.c_str()); ReportError(ErrorStr); throw 100;
		}
		// number of piled textures in case of 3D texture or tif format
		sstream >> cur_texture->texture_Nb_Layers;
		if (cur_texture->texture_usage == Texture_brush) {
			nb_pen_brushes[indConfiguration] = cur_texture->texture_Nb_Layers;
		}
		if (cur_texture->texture_usage == Texture_multilayer_master_mask) {
			nb_layers_master_mask[indConfiguration] = cur_texture->texture_Nb_Layers;
		}

		// image initial geometry
		sstream >> cur_texture->texture_Size_X;
		sstream >> cur_texture->texture_Size_Y;
		if (cur_texture->texture_usage == Texture_master_mask &&
			(cur_texture->texture_Size_X < workingWindow_width || cur_texture->texture_Size_Y < window_height)) {
			sprintf(ErrorStr, "Error: master mask texture should be minimlally %dx%d (%dx%d)\n", window_width, window_height,
				cur_texture->texture_Size_X, cur_texture->texture_Size_Y); ReportError(ErrorStr); throw 100;
		}
		if (cur_texture->texture_usage == Texture_multilayer_master_mask &&
			(cur_texture->texture_Size_X < workingWindow_width || cur_texture->texture_Size_Y < window_height)) {
			sprintf(ErrorStr, "Error: multilayer master mask texture should be minimlally %dx%d (%dx%d)\n", window_width, window_height,
				cur_texture->texture_Size_X, cur_texture->texture_Size_Y); ReportError(ErrorStr); throw 100;
		}
		if (cur_texture->texture_usage == Texture_noise &&
			(cur_texture->texture_Size_X < workingWindow_width_powerOf2 || cur_texture->texture_Size_Y < window_height_powerOf2)) {
			sprintf(ErrorStr, "Error: noise texture should be minimlally %dx%d (%dx%d)\n", workingWindow_width_powerOf2, window_height_powerOf2,
				cur_texture->texture_Size_X, cur_texture->texture_Size_Y); ReportError(ErrorStr); throw 100;
		}
		if (cur_texture->texture_usage == Texture_part_init &&
			(cur_texture->texture_Size_X < workingWindow_width || cur_texture->texture_Size_Y < window_height)) {
			sprintf(ErrorStr, "Error: particle initialization texture should be minimlally %dx%d (%dx%d)\n", workingWindow_width, window_height,
				cur_texture->texture_Size_X, cur_texture->texture_Size_Y); ReportError(ErrorStr); throw 100;
		}
		if (cur_texture->texture_usage == Texture_part_acc &&
			(cur_texture->texture_Size_X < workingWindow_width || cur_texture->texture_Size_Y < window_height)) {
			sprintf(ErrorStr, "Error: particle acceleration texture should be minimlally %dx%d (%dx%d)\n", workingWindow_width, window_height,
				cur_texture->texture_Size_X, cur_texture->texture_Size_Y); ReportError(ErrorStr); throw 100;
		}
		if (cur_texture->texture_usage == Texture_pixel_acc &&
			(cur_texture->texture_Size_X < workingWindow_width || cur_texture->texture_Size_Y < window_height)) {
			sprintf(ErrorStr, "Error: pixel acceleration texture should be minimlally %dx%d (%dx%d)\n", workingWindow_width, window_height,
				cur_texture->texture_Size_X, cur_texture->texture_Size_Y); ReportError(ErrorStr); throw 100;
		}
		if (cur_texture->texture_usage == Texture_repop_density &&
			(cur_texture->texture_Size_X < workingWindow_width || cur_texture->texture_Size_Y < window_height)) {
			sprintf(ErrorStr, "Error:  repopulation density texture should be minimlally %dx%d (%dx%d)\n", workingWindow_width, window_height,
				cur_texture->texture_Size_X, cur_texture->texture_Size_Y); ReportError(ErrorStr); throw 100;
		}

		// image color depth
		sstream >> cur_texture->texture_Nb_Bytes_per_Pixel;

		// booleans invert & is rectangle
		sstream >> ID;
		if (ID.compare("true") == 0 || ID.compare("TRUE") == 0) {
			cur_texture->texture_Is_Rectangle = true;
		}
		else if (ID.compare("false") == 0 || ID.compare("FALSE") == 0) {
			cur_texture->texture_Is_Rectangle = false;
		}
		else {
			sprintf(ErrorStr, "Error: incorrect boolean for Texture rectangle \"%s\" (true or false expected)\n", ID.c_str()); ReportError(ErrorStr); throw 100;
		}
		sstream >> ID;
		if (ID.compare("true") == 0 || ID.compare("TRUE") == 0) {
			cur_texture->texture_Invert = true;
		}
		else if (ID.compare("false") == 0 || ID.compare("FALSE") == 0) {
			cur_texture->texture_Invert = false;
		}
		else {
			sprintf(ErrorStr, "Error: incorrect boolean for Texture invert \"%s\" (true or false expected)\n", ID.c_str()); ReportError(ErrorStr); throw 100;
		}

		pg_Textures->push_back(cur_texture);

		//printf("Texture #%d size (%d,%d), rank %d, usage %s\n",
		//	indTextureFile, pg_Texture_Size_X[indTextureFile], pg_Texture_Size_Y[indTextureFile],
		//	pg_Texture_Rank[indTextureFile], pg_Texture_usages[indTextureFile].c_str());
	}
	// /textures
}

void ParseScenarioColorPalettes(std::ifstream& scenarioFin, int indConfiguration) {
	std::stringstream  sstream;
	string line;
	string ID;
	string temp;

	////////////////////////////
	////// PALETTE COLORSS
	// palettes markup
	std::getline(scenarioFin, line);
	stringstreamStoreLine(&sstream, &line);
	sstream >> ID; // string palette_colors
	if (ID.compare("palette_colors") != 0) {
		sprintf(ErrorStr, "Error: incorrect configuration file expected string \"palette_colors\" not found! (instead \"%s\")\n", ID.c_str()); ReportError(ErrorStr); throw 100;
	}

	// palettes parsing
	while (true) {
		std::getline(scenarioFin, line);
		stringstreamStoreLine(&sstream, &line);
		sstream >> ID; // string texture
		if (ID.compare("/palette_colors") == 0) {
			break;
		}
		else if (ID.compare("palette") != 0) {
			sprintf(ErrorStr, "Error: incorrect configuration file expected string \"palette\" not found! (instead \"%s\")\n", ID.c_str()); ReportError(ErrorStr); throw 100;
		}

		array<float, 9> paletteData;
		sstream >> ID; // palette ID
		for (int indColorBandpass = 0; indColorBandpass < 3; indColorBandpass++) {
			for (int indColorChannel = 0; indColorChannel < 3; indColorChannel++) {
				sstream >> paletteData[indColorBandpass * 3 + indColorChannel];
				paletteData[indColorBandpass * 3 + indColorChannel] /= 255.f;
			}
		}
		pg_Palettes[indConfiguration].push_back(new Palette(ID, paletteData));
		// std::cout << "palettes : " << indPalette << " " <<  pen_palettes_names[indPalette] << " " << pen_palettes_values[indPalette] << "\n";
	}
	// std::cout << "line: " << line << "\n";
	// std::cout << "Nb palettes : " << pg_Palettes[indConfiguration].size() <<  "\n";
}

void ParseScenarioColorPresets(std::ifstream& scenarioFin, int indConfiguration) {
	std::stringstream  sstream;
	string line;
	string ID;
	string temp;

	////////////////////////////
	////// COLOR PRESETS
	// Number of presets
	std::getline(scenarioFin, line);
	stringstreamStoreLine(&sstream, &line);
	sstream >> ID; // string color_presets
	if (ID.compare("color_presets") != 0) {
		sprintf(ErrorStr, "Error: incorrect configuration file expected string \"color_presets\" not found! (instead \"%s\")\n", ID.c_str()); ReportError(ErrorStr); throw 100;
	}

	while(true) {
		std::getline(scenarioFin, line);
		stringstreamStoreLine(&sstream, &line);
		sstream >> ID; // string texture
		if (ID.compare("/color_presets") == 0) {
			break;
		}
		else if (ID.compare("preset") != 0) {
			sprintf(ErrorStr, "Error: incorrect configuration file expected string \"preset\" not found! (instead \"%s\")\n", ID.c_str()); ReportError(ErrorStr); throw 100;
		}

		Color presetColor;
		sstream >> ID; // preset ID
		sstream >> presetColor.color;           // hue
		sstream >> presetColor.grey;            // color to white interpolation
		sstream >> presetColor.alpha;           // transparency, translated into to black interpolation for RGB conversion wo transparency
		pg_ColorPresets[indConfiguration].push_back(new ColorPreset(ID, presetColor));
		//std::cout << "palettes : " << indPalette << " " <<  pen_colorPresets_names[indPalette] << " " << pg_colorPreset_values[indPalette] << "\n";
	}
	// std::cout << "line: " << line << "\n";
	// std::cout << "Nb palettes : " << pg_ColorPresets[indConfiguration].size() <<  "\n";
}

void ParseScenarioLights(std::ifstream & scenarioFin, int indConfiguration) {
	std::stringstream  sstream;
	string line;
	string ID;
	string temp;

#if defined(PG_LIGHTS_CONTROL_IN_PG) || defined(PG_LIGHTS_CONTROL_IN_PYTHON)
	////////////////////////////
	////// LIGHTS
	// lights markup
	std::getline(scenarioFin, line);
	stringstreamStoreLine(&sstream, &line);
	sstream >> ID; // string color_presets
	if (ID.compare("lights") != 0) {
		sprintf(ErrorStr, "Error: incorrect configuration file expected string \"lights\" not found! (instead \"%s\")\n", ID.c_str()); ReportError(ErrorStr); throw 100;
	}

	// lights presets
	pg_nb_light_groups[indConfiguration] = 0;
	int nbLights = 0;
	while (true) {
		std::getline(scenarioFin, line);
		stringstreamStoreLine(&sstream, &line);
		sstream >> ID; // string texture
		if (ID.compare("/lights") == 0) {
			break;
		}
		else if (ID.compare("light") != 0) {
			sprintf(ErrorStr, "Error: incorrect configuration file expected string \"light\" not found! (instead \"%s\")\n", ID.c_str()); ReportError(ErrorStr); throw 100;
		}

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
			sprintf(ErrorStr, "Error: incorrect configuration file light group (%d) should be a value >= 1 for light %d\n", a_light_group, nbLights + 1); ReportError(ErrorStr); throw 100;
		}
		pg_nb_light_groups[indConfiguration] = max(pg_nb_light_groups[indConfiguration], a_light_group);

		sstream >> a_light_port;
		if (a_light_port != 1 && a_light_port != 2) {
			sprintf(ErrorStr, "Error: incorrect configuration file light port should be equal to 1 or 2 not %d for light %d\n", a_light_port, nbLights + 1); ReportError(ErrorStr); throw 100;
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
		pg_Lights[indConfiguration].push_back(new Light(a_light_name, a_light_group, 0, a_light_port, a_light_address, a_light_channels,
			a_light_red, a_light_green, a_light_blue, a_light_grey, a_light_dimmer, a_light_strobe, a_light_zoom, a_light_pan, a_light_tilt, a_light_hue));
		//std::cout << "light : " << indLight << " id " <<  a_light_names << " port " 
		//	<< a_light_port << " rank " << a_light_group << " add " << a_light_address << " ch " << a_light_channels << " r "
		//	<< a_light_red << " g " << a_light_green << " b " << a_light_blue 
		//	<< " dimm " << a_light_dimmer <<" strobe " << a_light_strobe << "\n";
	}

	// initializes light groups with no lights (0 light index in each light group)
	vector<int> indexes_in_group(pg_nb_light_groups[indConfiguration],0);
	// assign index inside light group to each light
	for (Light* light : pg_Lights[indConfiguration]) {
		int an_index_in_group = 0;
		int a_light_group = light->light_group;
		if (a_light_group < pg_nb_light_groups[indConfiguration]) {
			an_index_in_group = indexes_in_group[a_light_group];
			indexes_in_group[a_light_group] += 1;
		}
		light->index_in_group = an_index_in_group;
	}


	// classes of light groups initialized to default values
	for (int ind_light_group = 0; ind_light_group < pg_nb_light_groups[indConfiguration]; ind_light_group++) {
		pg_light_groups[indConfiguration].push_back(new LightGroup(ind_light_group + 1));
		for (Light* light : pg_Lights[indConfiguration]) {
			if (light->light_group == ind_light_group + 1) {
				pg_light_groups[indConfiguration][ind_light_group]->set_group_id(light->light_name);
				//printf("group ind %d id %s\n", ind , pg_light_groups[ind].get_group_id().c_str());
				break;
			}
		}
	}

	// Iterate over an unordered_map using range based for loop
	// builds pg_inverse_light_param_hashMap from pg_light_param_hashMap
	// by exchanging keys and values
	for (const auto& myPair : pg_light_param_hashMap) {
		pg_inverse_light_param_hashMap[myPair.second] = myPair.first;
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
	// initializations for scenario variables: value arrray and active vars
	for (int indP = 0; indP < _MaxInterpVarIDs + 1; indP++) {
		pg_ScenarioActiveVars[indP][indConfiguration] = false;
		if (ScenarioVarIndiceRanges[indP][0] == -1) {
			InitialValuesInterpVar[indConfiguration][indP].init_ScenarioValue(0., "", NULL, 0);
			LastGUIShippedValuesInterpVar[indConfiguration][indP].init_ScenarioValue(0., "", NULL, 0);
		}
		else {
			InitialValuesInterpVar[indConfiguration][indP].init_ScenarioValue(0., "", NULL, ScenarioVarIndiceRanges[indP][1]);
			LastGUIShippedValuesInterpVar[indConfiguration][indP].init_ScenarioValue(0., "", NULL, ScenarioVarIndiceRanges[indP][1]);
		}
	}

	// loop over all the variables of the full scenario, but only the active variables for the current configuration are used, 
	// the other ones keep a constant initial value of the full scenario
	for (int indP = 0; indP < _MaxInterpVarIDs + 1; indP++) {
		if (sstream.eof()) {
			break;
			//sprintf(ErrorStr, "Error: scenario (index %d) of configuration %d missing fewer variables in scenario files than expected\n", indP, indConfiguration); ReportError(ErrorStr); throw 50;
		}
		sstream >> temp;
		// first item is "ID"
		if (indP > 0) {
			// the index of the current scenario var into the global list of variables cumulated from all the scenarios
			indVar = pg_varID_to_rank(temp, indConfiguration);
			ConfigScenarioVarRank[indConfiguration][indP - 1] = indVar;
			ConfigScenarioVarIDs[indConfiguration][indP - 1] = temp;
			if (indVar == -1) {
				sprintf(ErrorStr, "Error: scenario variable %s (index %d) of configuration %d not found in the variable list generated by python generator\n", temp.c_str(), indP, indConfiguration); ReportError(ErrorStr); throw 50;
			}
			pg_ScenarioActiveVars[indVar][indConfiguration] = true;
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
		indVar = ConfigScenarioVarRank[indConfiguration][indP];
		if (pg_ScenarioActiveVars[indVar][indConfiguration]) {
			ConfigScenarioVarRank[indConfiguration][indVar];
			if (sstream.eof()) {
				sprintf(ErrorStr, "Error: missing initial value %s %d\n", temp.c_str(), indVar); ReportError(ErrorStr); throw 50;
			}
			sstream >> temp;
			if (ScenarioVarTypes[indVar] != _pg_string) {
				// dimension 1 variable: number
				if (ScenarioVarIndiceRanges[indVar][0] == -1) {
					bool has_only_digits = (temp.find_first_not_of("0123456789-.E") == string::npos);
					if (!has_only_digits) {
						sprintf(ErrorStr, "Error: non numeric variable initial value for var %d (%s)\n", indVar, ScenarioVarStrings[indVar]); ReportError(ErrorStr); throw 50;
					}
					InitialValuesInterpVar[indConfiguration][indVar].val_num = my_stod(temp);
				}
				// array
				else {
					unsigned int size_of_array = ScenarioVarIndiceRanges[indVar][1] - ScenarioVarIndiceRanges[indVar][0];
					vector<string> values = split_string(temp, '/');
					if (values.size() != size_of_array) {
						sprintf(ErrorStr, "Error: numeric array variable expects %d initial value not %d (%s[%d..%d])\n", size_of_array, values.size(), ScenarioVarStrings[indVar], ScenarioVarIndiceRanges[indVar][0], ScenarioVarIndiceRanges[indVar][1]); ReportError(ErrorStr); throw 50;
					}
					int indVect = 0;
					for (int index = ScenarioVarIndiceRanges[indVar][0]; index < ScenarioVarIndiceRanges[indVar][1]; index++) {
						InitialValuesInterpVar[indConfiguration][indVar].val_array[index] = my_stod(values[indVect]);
						indVect++;
					}
				}
			}
			else {
				InitialValuesInterpVar[indConfiguration][indVar].val_string = temp;
			}
			//std::cout << indP << " " << InitialValuesInterpVar[indConfiguration][indP] << " \n";
			LastGUIShippedValuesInterpVar[indConfiguration][indVar].const_value(MAXFLOAT);
		}
	}
	// checks that the number of variables is what is expected
	//sstream >> ID;
	if (!sstream.eof()) {
		sprintf(ErrorStr, "Error: too many initial variable values %s: expected %d\n", sstream.str().c_str(), ScenarioVarNb[indConfiguration]); ReportError(ErrorStr); throw 50;
	}

	//std::cout << "\n";
	// /initial_values
	std::getline(scenarioFin, line);


	////////////////////////////
	////// SCENARIO
	// Number of scenes
	std::getline(scenarioFin, line);
	stringstreamStoreLine(&sstream, &line);
	sstream >> ID; // string scenario

	pg_InterpolationScene.init_scene();
	pg_variable_param_input_type = new pg_Parameter_Input_Type[_MaxInterpVarIDs];
	for (int indP = 0; indP < ScenarioVarNb[indConfiguration]; indP++) {
		indVar = ConfigScenarioVarRank[indConfiguration][indP];
		if (pg_ScenarioActiveVars[indVar][indConfiguration]) {
			pg_variable_param_input_type[indVar] = _PG_SCENARIO;
		}
	}

	printf("Loading scenario %s with %d variables\n", pg_ScenarioFileNames[indConfiguration].c_str(), ScenarioVarNb[indConfiguration]);
	int nbScenesInScenario = 0;
	while(true) {
		std::getline(scenarioFin, line);
		stringstreamStoreLine(&sstream, &line);
		sstream >> ID; // string scene
		if (ID.compare("/scenario") == 0) {
			break;
		}
		else if (ID.compare("scene") != 0) {
			sprintf(ErrorStr, "Error: incorrect configuration file expected string \"scene\" not found! (instead \"%s\")\n", ID.c_str()); ReportError(ErrorStr); throw 100;
		}
		
		Scene* newScene = new Scene();

		newScene->init_scene();

		sstream >> newScene->scene_IDs;
		std::cout << "scene: " << newScene->scene_IDs << "\n";
		sstream >> newScene->scene_duration;
		sstream >> temp;  // change_when_ends or prolong_when_ends
		if (temp.compare("change_when_ends") == 0) {
			newScene->scene_change_when_ends = true;
		}
		else if (temp.compare("prolong_when_ends") == 0) {
			newScene->scene_change_when_ends = false;
		}
		else {
			newScene->scene_change_when_ends = true;
			sprintf(ErrorStr, "Error: one of strings expected as scene ending mode: \"change_when_ends\" or \"prolong_when_ends\" not \"%s\" for scene %d %s\n", temp.c_str(), nbScenesInScenario, newScene->scene_IDs.c_str()); ReportError(ErrorStr); throw 50;
		}
		// second and third comments possibly displayed on the interface to help the user
		if (!sstream.eof()) {
			sstream >> newScene->scene_Msg1;
		}
		else {
			newScene->scene_Msg1 = "";
		}
		if (!sstream.eof()) {
			sstream >> newScene->scene_Msg2;
		}
		else {
			newScene->scene_Msg2 = "";
		}

		//std::cout << "duration : " << 
		//  newScene->scene_duration << " sc #" << nbScenesInScenario << "\n";

		if (newScene->scene_duration <= 0.0) {
			sprintf(ErrorStr, "Error: null scene #%d duration [%f]!", nbScenesInScenario + 1, newScene->scene_duration); ReportError(ErrorStr); throw 50;
		}
		if (nbScenesInScenario > 0) {
			newScene->scene_initial_time = pg_Scenario[indConfiguration][nbScenesInScenario - 1]->scene_final_time;
		}
		else {
			newScene->scene_initial_time = 0.0;
		}
		newScene->scene_final_time
			= newScene->scene_initial_time + newScene->scene_duration;

		// verbatim
		std::getline(scenarioFin, line);
		// var comment
		std::getline(scenarioFin, line);
		// std::cout << "var comment: " << line << "\n";

		// storing the initial values
		std::getline(scenarioFin, line);
		stringstreamStoreLine(&sstream, &line);
		//std::cout << "line :" << line;
		//std::cout << "\ninitial values :\n";
		for (int indP = 0; indP < ScenarioVarNb[indConfiguration]; indP++) {
			indVar = ConfigScenarioVarRank[indConfiguration][indP];
			if (pg_ScenarioActiveVars[indVar][indConfiguration]) {
				if (sstream.eof()) {
					sprintf(ErrorStr, "Error: missing initial value in scene %d var %d (%s)\n",
						nbScenesInScenario + 1, indVar, ScenarioVarMessages[indVar]); ReportError(ErrorStr); throw 50;
				}
				sstream >> temp;
				if (ScenarioVarTypes[indVar] == _pg_string) {
					newScene->scene_initial_parameters[indVar].val_string = temp;
				}
				else {
					// dimension 1 variable: number
					if (ScenarioVarIndiceRanges[indVar][0] == -1) {
						bool has_only_digits = (temp.find_first_not_of("0123456789-.E") == string::npos);
						if (!has_only_digits) {
							sprintf(ErrorStr, "Error: non numeric variable initial value in scene %d var %d (%s) type %d\n",
								nbScenesInScenario + 1, indVar, ScenarioVarStrings[indVar], ScenarioVarTypes[indVar]); ReportError(ErrorStr); throw 50;
						}
						else {
							newScene->scene_initial_parameters[indVar].val_num = my_stod(temp);
						}
					}
					// array
					else {
						unsigned int size_of_array = ScenarioVarIndiceRanges[indVar][1] - ScenarioVarIndiceRanges[indVar][0];
						vector<string> values = split_string(temp, '/');
						if (values.size() != size_of_array) {
							sprintf(ErrorStr, "Error: numeric array variable expects %d scene initial values not %d (%s[%d..%d])\n", size_of_array, values.size(), ScenarioVarStrings[indVar], ScenarioVarIndiceRanges[indVar][0], ScenarioVarIndiceRanges[indVar][1]); ReportError(ErrorStr); throw 50;
						}
						int indVect = 0;
						for (int index = ScenarioVarIndiceRanges[indVar][0]; index < ScenarioVarIndiceRanges[indVar][1]; index++) {
							newScene->scene_initial_parameters[indVar].val_array[index] = my_stod(values[indVect]);
							indVect++;
						}
					}
				}
			}
		}
		// checks that the number of variables is what is expected
		//sstream >> ID;
		if (!sstream.eof()) {
			sprintf(ErrorStr, "Error: too many initial variable values in scene %d (%s)\n", nbScenesInScenario + 1, sstream.str().c_str()); ReportError(ErrorStr); throw 50;
		}

		// storing the final values
		std::getline(scenarioFin, line);
		stringstreamStoreLine(&sstream, &line);
		//std::cout << "line :" << line;
		//std::cout << "\nfinal values :\n";

		for (int indP = 0; indP < ScenarioVarNb[indConfiguration]; indP++) {
			indVar = ConfigScenarioVarRank[indConfiguration][indP];
			if (pg_ScenarioActiveVars[indVar][indConfiguration]) {
				if (sstream.eof()) {
					sprintf(ErrorStr, "Error: missing final value in scene %d var %d (%s)\n", nbScenesInScenario + 1, indVar, temp.c_str()); ReportError(ErrorStr); throw 50;
				}
				sstream >> temp;
				//printf("val %s\n", temp.c_str());
				if (ScenarioVarTypes[indVar] == _pg_string) {
					newScene->scene_final_parameters[indVar].val_string = temp;
				}
				else {
					// dimension 1 variable: number
					if (ScenarioVarIndiceRanges[indVar][0] == -1) {
						bool has_only_digits = (temp.find_first_not_of("0123456789-.E") == string::npos);
						if (!has_only_digits) {
							sprintf(ErrorStr, "Error: non numeric variable initial value in scene %d var %d (%s) type %d\n",
								nbScenesInScenario + 1, indVar, ScenarioVarStrings[indVar], ScenarioVarTypes[indVar]); ReportError(ErrorStr); throw 50;
						}
						else {
							newScene->scene_final_parameters[indVar].val_num = my_stod(temp);
						}
					}
					// array
					else {
						unsigned int size_of_array = ScenarioVarIndiceRanges[indVar][1] - ScenarioVarIndiceRanges[indVar][0];
						vector<string> values = split_string(temp, '/');
						if (values.size() != size_of_array) {
							sprintf(ErrorStr, "Error: numeric array variable expects %d scene initial values not %d (%s[%d..%d])\n", size_of_array, values.size(), ScenarioVarStrings[indVar], ScenarioVarIndiceRanges[indVar][0], ScenarioVarIndiceRanges[indVar][1]); ReportError(ErrorStr); throw 50;
						}
						int indVect = 0;
						for (int index = ScenarioVarIndiceRanges[indVar][0]; index < ScenarioVarIndiceRanges[indVar][1]; index++) {
							newScene->scene_final_parameters[indVar].val_array[index] = my_stod(values[indVect]);
							indVect++;
						}
					}
				}
			}
			// std::cout << scene_final_parameters[nbScenesInScenario][indVar].val_num << " ";
		}
		// checks that the number of variables is what is expected
		//sstream >> ID;
		if (!sstream.eof()) {
			sprintf(ErrorStr, "Error: too many final variable values in scene %d\n", nbScenesInScenario + 1); ReportError(ErrorStr); throw 50;
		}

		std::getline(scenarioFin, line);
		stringstreamStoreLine(&sstream, &line);
		//printf("line interp %s\n", line.c_str());

		// storing the interpolation mode
		for (int indP = 0; indP < ScenarioVarNb[indConfiguration]; indP++) {
			indVar = ConfigScenarioVarRank[indConfiguration][indP];
			if (pg_ScenarioActiveVars[indVar][indConfiguration]) {
				char valCh = 0;
				char valCh2 = 0;
				string vals, val2s;
				float val3;

				newScene->scene_interpolations[indVar].offSet = 0.0;
				newScene->scene_interpolations[indVar].duration = 1.0;
				if (ScenarioVarIndiceRanges[indVar][0] == -1) {
					newScene->scene_interpolations[indVar].midTermValue
						= 0.5 * (newScene->scene_initial_parameters[indVar].val_num
							+ newScene->scene_final_parameters[indVar].val_num);
				}
				else {
					for (int index = ScenarioVarIndiceRanges[indVar][0]; index < ScenarioVarIndiceRanges[indVar][1]; index++) {
						newScene->scene_interpolations[indVar].midTermValueArray[index]
							= 0.5 * (newScene->scene_initial_parameters[indVar].val_array[index]
								+ newScene->scene_final_parameters[indVar].val_array[index]);
					}
				}

				if (sstream.eof()) {
					sprintf(ErrorStr, "Error: missing interpolation value in scene %d (%s) var %d (%s)\n",
						nbScenesInScenario + 1, newScene->scene_IDs.c_str(), indVar + 1, ScenarioVarMessages[indVar]); ReportError(ErrorStr); throw 50;
				}
				sstream >> std::skipws >> temp;
				valCh = temp.at(0);
				//printf("valch %d %c temp %s\n", (int)valCh, valCh, temp.c_str());
				if (temp.length() > 1) {
					valCh2 = temp.at(1);
					if (valCh2 != 'k') {
						sprintf(ErrorStr, "Error: only k modifier is allowed on interpolation mode, not %c for %c interpolation in scene %d var %d\n", 
							valCh2, valCh, nbScenesInScenario + 1, indVar + 1); ReportError(ErrorStr); throw 50;
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
					newScene->scene_interpolations[indVar].interpolation_mode = pg_linear_interpolation;
					if (valCh2 == ' ') {
						newScene->scene_interpolations[indVar].initialization_mode = pg_scenario_initial;
					}
					else if (valCh2 == 'k') {
						newScene->scene_interpolations[indVar].initialization_mode = pg_current_value;
					}
					if (valCh == 'L') {
						sstream >> vals;
						sstream >> val2s;
						double val = stringToDuration(vals, newScene->scene_duration, nbScenesInScenario + 1, indVar + 1);
						double val2 = stringToDuration(val2s, newScene->scene_duration, nbScenesInScenario + 1, indVar + 1);
						if (val < 0.0 || val2 < 0.0) {
							sprintf(ErrorStr, "Error: one of values of L(inear) in scene %d var %d lower than 0.0: %.3f %.3f\n", nbScenesInScenario + 1, indVar + 1, val, val2); ReportError(ErrorStr); throw 50;
						}
						if (val <= 1.0) {
							newScene->scene_interpolations[indVar].offSet = val;
							if (float(val + val2) <= 1.00001f) {
								// deals with approximate values that can summ above 1.0
								if (float(val + val2) > 1.0f) {
									val2 = 1.0f - val;
								}
								newScene->scene_interpolations[indVar].duration = val2;
								if (newScene->scene_interpolations[indVar].duration <= 0.0) {
									sprintf(ErrorStr, "Error: null L(inear) in scene %d var %d duration [%f]!", nbScenesInScenario + 1, indVar + 1, newScene->scene_interpolations[indVar].duration); ReportError(ErrorStr); throw 50;
								}
							}
							else {
								sprintf(ErrorStr, "Error: total duration of L(inear) in scene %d var %d greater than 1.0: %.3f + %.3f\n", nbScenesInScenario + 1, indVar + 1, val, val2); ReportError(ErrorStr); throw 50;
							}
						}
						else {
							sprintf(ErrorStr, "Error: offset value L(inear) in scene %d var %d greater than 1.0: %.3f\n", nbScenesInScenario + 1, indVar + 1, val); ReportError(ErrorStr); throw 50;
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
					newScene->scene_interpolations[indVar].interpolation_mode = pg_cosine_interpolation;
					if (valCh2 == ' ') {
						newScene->scene_interpolations[indVar].initialization_mode = pg_scenario_initial;
					}
					else if (valCh2 == 'k') {
						newScene->scene_interpolations[indVar].initialization_mode = pg_current_value;
					}
					if (valCh == 'C') {
						sstream >> vals;
						sstream >> val2s;
						double val = stringToDuration(vals, newScene->scene_duration, nbScenesInScenario + 1, indVar + 1);
						double val2 = stringToDuration(val2s, newScene->scene_duration, nbScenesInScenario + 1, indVar + 1);
						if (val < 0.0 || val2 < 0.0) {
							sprintf(ErrorStr, "Error: one of values of C(osine) in scene %d var %d lower than 0.0: %.3f %.3f\n", nbScenesInScenario + 1, indVar + 1, val, val2); ReportError(ErrorStr); throw 50;
						}
						if (val <= 1.0) {
							newScene->scene_interpolations[indVar].offSet = val;
							// deals with approximate values that can summ above 1.0
							if (float(val + val2) <= 1.00001f) {
								if (float(val + val2) > 1.0f) {
									val2 = 1.0f - val;
								}
								newScene->scene_interpolations[indVar].duration = val2;
								if (newScene->scene_interpolations[indVar].duration <= 0.0) {
									sprintf(ErrorStr, "Error: null C(osine) in scene %d var %d duration [%f]!", nbScenesInScenario + 1, indVar + 1, newScene->scene_interpolations[indVar].duration); ReportError(ErrorStr); throw 50;
								}
							}
							else {
								sprintf(ErrorStr, "Error: total duration of C(osine) in scene %d var %d greater than 1.0: %.3f + %.3f\n", nbScenesInScenario + 1, indVar + 1, val, val2); ReportError(ErrorStr); throw 50;
							}
						}
						else {
							sprintf(ErrorStr, "Error: offset value C(osine) in scene %d var %d greater than 1.0: %.3f\n", nbScenesInScenario + 1, indVar + 1, val); ReportError(ErrorStr); throw 50;
						}
					}
				}
				// z: value interpolates cosinely between initial and final value from 0.0% to 1.0%
				// Z: value is initial from 0.0% until offset, 
				// Beizer approximated with a cosine and a non linear input 3x^2-2x^3
				// BEZIER INTERPOLATION
				else if (valCh == 'z' || valCh == 'Z') {
					newScene->scene_interpolations[indVar].interpolation_mode = pg_bezier_interpolation;
					if (valCh2 == ' ') {
						newScene->scene_interpolations[indVar].initialization_mode = pg_scenario_initial;
					}
					else if (valCh2 == 'k') {
						newScene->scene_interpolations[indVar].initialization_mode = pg_current_value;
					}
					if (valCh == 'Z') {
						sstream >> vals;
						sstream >> val2s;
						double val = stringToDuration(vals, newScene->scene_duration, nbScenesInScenario + 1, indVar + 1);
						double val2 = stringToDuration(val2s, newScene->scene_duration, nbScenesInScenario + 1, indVar + 1);
						if (val < 0.0 || val2 < 0.0) {
							sprintf(ErrorStr, "Error: one of values of Z(Bezier) in scene %d var %d lower than 0.0: %.3f %.3f\n", nbScenesInScenario + 1, indVar + 1, val, val2); ReportError(ErrorStr); throw 50;
						}
						if (val <= 1.0) {
							newScene->scene_interpolations[indVar].offSet = double(val);
							// deals with approximate values that can summ above 1.0
							if (float(double(val) + double(val2)) <= 1.00001) {
								if (float(double(val) + double(val2)) > 1.0) {
									val2 = 1.0f - val;
								}
								newScene->scene_interpolations[indVar].duration = val2;
								if (newScene->scene_interpolations[indVar].duration <= 0.0) {
									sprintf(ErrorStr, "Error: null Z(Bezier) in scene %d var %d duration [%f]!", nbScenesInScenario + 1, indVar + 1, newScene->scene_interpolations[indVar].duration); ReportError(ErrorStr); throw 50;
								}
							}
							else {
								sprintf(ErrorStr, "Error: total duration of Z(Bezier) in scene %d var %d greater than 1.0: %.3f + %.3f\n", nbScenesInScenario + 1, indVar + 1, val, val2); ReportError(ErrorStr); throw 50;
							}
						}
						else {
							sprintf(ErrorStr, "Error: offset value Z(Bezier) in scene %d var %d greater than 1.0: %.3f\n", nbScenesInScenario + 1, indVar + 1, val); ReportError(ErrorStr); throw 50;
						}
					}
				}
				// e: value interpolates exponentially between initial and final value from 0.0% to 1.0%
				// E: value is initial from 0.0% until offset, 
				// exponential interpolation: alpha ^ exponent
				// EXPONENTIAL INTERPOLATION
				else if (valCh == 'e' || valCh == 'E') {
					newScene->scene_interpolations[indVar].interpolation_mode = pg_exponential_interpolation;
					if (valCh2 == ' ') {
						newScene->scene_interpolations[indVar].initialization_mode = pg_scenario_initial;
					}
					else if (valCh2 == 'k') {
						newScene->scene_interpolations[indVar].initialization_mode = pg_current_value;
					}
					sstream >> val3;
					newScene->scene_interpolations[indVar].exponent = val3;
					if (valCh == 'E') {
						sstream >> vals;
						sstream >> val2s;
						double val = stringToDuration(vals, newScene->scene_duration, nbScenesInScenario + 1, indVar + 1);
						double val2 = stringToDuration(val2s, newScene->scene_duration, nbScenesInScenario + 1, indVar + 1);
						if (val < 0.0 || val2 < 0.0) {
							sprintf(ErrorStr, "Error: one of values of E(exponential) in scene %d var %d lower than 0.0: %.3f %.3f\n", nbScenesInScenario + 1, indVar + 1, val, val2); ReportError(ErrorStr); throw 50;
						}
						if (val <= 1.0) {
							newScene->scene_interpolations[indVar].offSet = val;
							// deals with approximate values that can summ above 1.0
							if (float(double(val) + double(val2)) <= 1.00001) {
								if (float(double(val) + double(val2)) > 1.0) {
									val2 = 1.0f - val;
								}
								newScene->scene_interpolations[indVar].duration = val2;
								if (newScene->scene_interpolations[indVar].duration <= 0.0) {
									sprintf(ErrorStr, "Error: null Z(Bezier) in scene %d var %d duration [%f]!", nbScenesInScenario + 1, indVar + 1, newScene->scene_interpolations[indVar].duration); ReportError(ErrorStr); throw 50;
								}
							}
							else {
								sprintf(ErrorStr, "Error: total duration of Z(Bezier) in scene %d var %d greater than 1.0: %.3f + %.3f\n", nbScenesInScenario + 1, indVar + 1, val, val2); ReportError(ErrorStr); throw 50;
							}
						}
						else {
							sprintf(ErrorStr, "Error: offset value Z(Bezier) in scene %d var %d greater than 1.0: %.3f\n", nbScenesInScenario + 1, indVar + 1, val); ReportError(ErrorStr); throw 50;
						}
					}
				}
				// b: bell curve interpolation between initial, median and final value from (0,0,0)% to (0,1,0)% at mid time to (0,0,1)% at the end
				// BELL INTERPOLATION
				else if (valCh == 'b' || valCh == 'B') {
					newScene->scene_interpolations[indVar].interpolation_mode = pg_bell_interpolation;
					if (valCh2 == ' ') {
						newScene->scene_interpolations[indVar].initialization_mode = pg_scenario_initial;
					}
					else if (valCh2 == 'k') {
						newScene->scene_interpolations[indVar].initialization_mode = pg_current_value;
					}
					sstream >> newScene->scene_interpolations[indVar].midTermValue;
					if (ScenarioVarIndiceRanges[indVar][0] != -1) {
						if (newScene->scene_interpolations[indVar].midTermValueArray != NULL) {
							for (int index = ScenarioVarIndiceRanges[indVar][0]; index < ScenarioVarIndiceRanges[indVar][1]; index++) {
								newScene->scene_interpolations[indVar].midTermValueArray[index] = newScene->scene_interpolations[indVar].midTermValue;
							}
						}
						else {
							sprintf(ErrorStr, "Unexpected null array for bell interpolation %d (%s)!", indVar, ScenarioVarMessages[indVar]); ReportError(ErrorStr); throw 100;
						}
					}
					//printf("Bell interpolation mode in scene %d (%s) parameter %d [%d] [%c] mid term value %.2f!\n",
					//	nbScenesInScenario + 1, newScene->scene_IDs.c_str(), indVar + 1, int(valCh), valCh, 
					//	newScene->scene_interpolations[indVar].midTermValue);
					if (valCh == 'B') {
						sstream >> vals;
						sstream >> val2s;
						double val = stringToDuration(vals, newScene->scene_duration, nbScenesInScenario + 1, indVar + 1);
						double val2 = stringToDuration(val2s, newScene->scene_duration, nbScenesInScenario + 1, indVar + 1);
						if (val < 0.0 || val2 < 0.0) {
							sprintf(ErrorStr, "Error: one of values of B(ell) in scene %d var %d lower than 0.0: %.3f %.3f\n", nbScenesInScenario + 1, indVar + 1, val, val2); ReportError(ErrorStr); throw 50;
						}
						if (val <= 1.0) {
							newScene->scene_interpolations[indVar].offSet = val;
							if (float(val + val2) <= 1.00001f) {
								// deals with approximate values that can summ above 1.0
								if (float(val + val2) > 1.0f) {
									val2 = 1.0f - val;
								}
								newScene->scene_interpolations[indVar].duration = val2;
								if (newScene->scene_interpolations[indVar].duration <= 0.0) {
									sprintf(ErrorStr, "Error: null B(ell) in scene %d var %d duration [%f]!", nbScenesInScenario + 1, indVar + 1, newScene->scene_interpolations[indVar].duration); ReportError(ErrorStr); throw 50;
								}
							}
							else {
								sprintf(ErrorStr, "Error: total duration of B(ell) in scene %d var %d greater than 1.0: %.3f + %.3f\n", nbScenesInScenario + 1, indVar + 1, val, val2); ReportError(ErrorStr); throw 50;
							}
						}
						else {
							sprintf(ErrorStr, "Error: offset value L(inear) in scene %d var %d greater than 1.0: %.3f\n", nbScenesInScenario + 1, indVar + 1, val); ReportError(ErrorStr); throw 50;
						}
						// std::cout << "L " << val << " " << val2 << " ";

					}
				}
				// b: saw tooth linear interpolation between initial, median and final value from (0,0,0)% to (0,1,0)% at mid time to (0,0,1)% at the end
				// SAW TOOTH INTERPOLATION
				else if (valCh == 't') {
					newScene->scene_interpolations[indVar].interpolation_mode = pg_sawtooth_interpolation;
					if (valCh2 == ' ') {
						newScene->scene_interpolations[indVar].initialization_mode = pg_scenario_initial;
					}
					else if (valCh2 == 'k') {
						newScene->scene_interpolations[indVar].initialization_mode = pg_current_value;
					}
					sstream >> newScene->scene_interpolations[indVar].midTermValue;
					if (ScenarioVarIndiceRanges[indVar][0] != -1) {
						for (int index = ScenarioVarIndiceRanges[indVar][0]; index < ScenarioVarIndiceRanges[indVar][1]; index++) {
							newScene->scene_interpolations[indVar].midTermValueArray[index] = newScene->scene_interpolations[indVar].midTermValue;
						}
					}
				}
				// STEPWISE VALUE WITHOUT INTERPOLATION
				else if (valCh == 's' || valCh == 'S') {
					newScene->scene_interpolations[indVar].interpolation_mode = pg_stepwise_interpolation;
					newScene->scene_interpolations[indVar].initialization_mode = pg_scenario_initial;
					newScene->scene_interpolations[indVar].offSet = 0.0;
					newScene->scene_interpolations[indVar].duration = 1.0;
					if (valCh == 'S') {
						sstream >> vals;
						double val = stringToDuration(vals, newScene->scene_duration, nbScenesInScenario + 1, indVar + 1);
						if (val < 0.0) {
							sprintf(ErrorStr, "Error: offset values of S(tepwise) in scene %d (%s) var %d lower than 0.0: %.3f\n", nbScenesInScenario + 1, newScene->scene_IDs.c_str(), indVar + 1, val); ReportError(ErrorStr); throw 50;
						}
						if (val <= 1.0) {
							newScene->scene_interpolations[indVar].offSet = val;
							newScene->scene_interpolations[indVar].duration = 1.0 - double(val);
						}
						else {
							sprintf(ErrorStr, "Error: offset value of S(tepwise) in scene %d (%s) var %d greater than 1.0: %.3f\n", nbScenesInScenario + 1, newScene->scene_IDs.c_str(), indVar + 1, val); ReportError(ErrorStr); throw 50;
						}
					}
					else {
						if (ScenarioVarTypes[indVar] == _pg_string
							&& newScene->scene_initial_parameters[indVar].val_string
							!= newScene->scene_final_parameters[indVar].val_string) {
							sprintf(ErrorStr, "Error: S(tepwise) interpolation should have same initial and final values %s/%s in scene %d (%s) var %d (%s)\n",
								newScene->scene_initial_parameters[indVar].val_string.c_str(), newScene->scene_final_parameters[indVar].val_string.c_str(), 
								nbScenesInScenario + 1, newScene->scene_IDs.c_str(), indVar + 1, ScenarioVarMessages[indVar]); ReportError(ErrorStr); throw 50;
						}
						else if (ScenarioVarTypes[indVar] != _pg_string) {
							if (ScenarioVarIndiceRanges[indVar][0] == -1) {
								if (newScene->scene_initial_parameters[indVar].val_num
									!= newScene->scene_final_parameters[indVar].val_num) {
									sprintf(ErrorStr, "Error: S(tepwise) interpolation should have same initial and final values %.2f/%.2f in scene %d (%s) var %d (%s)\n",
										newScene->scene_initial_parameters[indVar].val_num, newScene->scene_final_parameters[indVar].val_num, nbScenesInScenario + 1,
										newScene->scene_IDs.c_str(), indVar + 1, ScenarioVarMessages[indVar]); ReportError(ErrorStr); throw 50;
								}
							}
							else {
								for (int index = ScenarioVarIndiceRanges[indVar][0]; index < ScenarioVarIndiceRanges[indVar][1]; index++) {
									if (newScene->scene_initial_parameters[indVar].val_array[index]
										!= newScene->scene_final_parameters[indVar].val_array[index]) {
										sprintf(ErrorStr, "Error: S(tepwise) interpolation should have same initial and final values %.2f/%.2f in scene %d (%s) var %d  (%s) index %d\n", 
											newScene->scene_initial_parameters[indVar].val_array[index], newScene->scene_final_parameters[indVar].val_array[index],
											nbScenesInScenario + 1, newScene->scene_IDs.c_str(), indVar + 1, ScenarioVarMessages[indVar], index + 1); ReportError(ErrorStr); throw 50;
									}
								}
							}
						}
					}
				}
				// KEEP CURRENT VALUE WITHOUT INTERPOLATION
				else if (valCh == 'k' || valCh == 'K') {
					newScene->scene_interpolations[indVar].initialization_mode = pg_current_value;
					newScene->scene_interpolations[indVar].interpolation_mode = pg_keep_value;
				}
				else {
					sprintf(ErrorStr, "Error: unknown interpolation mode in scene %d (%s) parameter %d [%d] [%c]!", nbScenesInScenario + 1, newScene->scene_IDs.c_str(), indVar + 1, int(valCh), valCh); ReportError(ErrorStr); throw 50;
				}

				//if (indVar == _trkDecay_1 && nbScenesInScenario == 12) {
				//	printf("Decay Scene %d Interpolation mode %d offset %.2f dur %.2f init %.2f fin %.2f\n", nbScenesInScenario, newScene->scene_interpolations[indVar].interpolation_mode,
				//		newScene->scene_interpolations[indVar].offSet,
				//		newScene->scene_interpolations[indVar].duration,
				//		newScene->scene_initial_parameters[indVar],
				//		newScene->scene_final_parameters[indVar]);
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

		pg_Scenario[indConfiguration].push_back(newScene);
		nbScenesInScenario++;
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
	int ind_scene = 0;
	for (Scene* scene : pg_Scenario[pg_current_configuration_rank]) {
		if (scene->scene_IDs.compare(*sceneID) == 0) {
			return ind_scene;
		}
		ind_scene++;
	}
	return -1;
}


void setWindowDimensions(void) {
	if (double_window && (window_width > 1920)) {
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
	for (Scene *scene : pg_Scenario[indConfiguration]) {
		scene->scene_originalDuration = scene->scene_duration;
		scene->scene_originalInitial_time = scene->scene_initial_time;
		scene->scene_originalFinal_time = scene->scene_final_time;
	}
}
// to be called before setup change
void restoreInitialTimesAndDurations(void) {
	for (Scene* scene : pg_Scenario[pg_current_configuration_rank]) {
		scene->scene_duration = scene->scene_originalDuration;
		scene->scene_initial_time = scene->scene_originalInitial_time;
		scene->scene_final_time = scene->scene_originalFinal_time;
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

