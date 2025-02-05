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

int                      nb_scenes;

Scene                    *Scenario;

string					 log_file_name;
string					 snapshots_dir_path_name;
string					 font_file_name;
string                   font_file_encoding;
TextureEncoding          font_texture_encoding;
int                      font_size;

pg_Window              *PG_Window;

// PNG capture
string                    Png_file_name;
string                    Png_shot_dir_name;
int                      beginPng;
int                      endPng;
int                      stepPng;
bool                     outputPng;
// \}

// SVG capture
string                    Svg_file_name;
string                    Svg_shot_dir_name;
int                      beginSvg;
int                      endSvg;
int                      stepSvg;
bool                     outputSvg;

// JPG capture
string                    Jpg_file_name;
string                    Jpg_shot_dir_name;
int                      beginJpg;
int                      endJpg;
int                      stepJpg;
bool                     outputJpg;

// VIDEO capture
string                    Video_file_name;
int                      beginVideo_file;
int                      endVideo_file;
int                      stepVideo_file;
bool                     outputVideo_file;

// UDP servers and clients
pg_IPServer            **IP_Servers;
int                      nb_IP_Servers;
pg_IPClient            **IP_Clients;
int                      nb_IP_Clients;

// shader file names
string * pg_Shader_Files;
GLenum ** pg_Shader_Types;
int * pg_Shader_nbTypes;
int nb_shader_files;

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

void parseConfigurationFile(std::ifstream& confFin, std::ifstream&  scenarioFin) {
	CurrentWindow = new pg_Window();

	////////////////////////////
	////// CONFIGURATION VARIABLE INITIALIZATION
	string line;
	string ID;
	string temp;
	string temp2;
	std::stringstream  sstrem;

	/////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////
	// CONFIGURATION FILE
	/////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////

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

	font_file_name = "./Data/fonts/usascii/arial/stb_font_arial_15_usascii.png";
	font_size = 15;
	font_file_encoding = "png";
	font_texture_encoding = PNG;

	snapshots_dir_path_name = "./snapshots/pic_" + project_name + "_" + date_stringStream.str() + "/";
	log_file_name = snapshots_dir_path_name + "log-LYM-" + project_name + "-" + date_stringStream.str() + ".log";

	int nError = 0;
#ifdef _WIN32
	nError = CreateDirectoryA(snapshots_dir_path_name.c_str(), NULL); // can be used on Windows
#else 
	nError = mkdir(snapshots_dir_path_name.c_str(), nMode); // can be used on non-Windows
#endif
	if (nError != 0) {
		// handle your error here
	}

	std::getline(confFin, line);
	sstrem.str(line);
	// std::cout << "VALUES: \n";
	for (int indP = 0; indP < _MaxConfigurationVarIDs; indP++) {
		sstrem >> InitialValuesConfigurationVar[indP];
		// std::cout << InitialValuesConfigurationVar[indP];
		// std::cout << " ";
	}
	// std::cout << "\n";

	// /initial_values
	std::getline(confFin, line);

	// rendering_files
	std::getline(confFin, line);

	// TYPE
	std::getline(confFin, line);
	// ID ID	begin	end	step	id	shots
	std::getline(confFin, line);

	// storing the Video capture values
	std::getline(confFin, line);
	sstrem.clear();
	sstrem.str(line);
	// sstrem = std::stringstream(line);
	sstrem >> beginVideo_file;
	sstrem >> endVideo_file;
	sstrem >> stepVideo_file;
	sstrem >> Video_file_name;
	outputVideo_file = !Video_file_name.empty();

	// storing the Svg capture values
	std::getline(confFin, line);
	sstrem.clear();
	sstrem.str(line);
	// sstrem = std::stringstream(line);
	sstrem >> beginSvg;
	sstrem >> endSvg;
	sstrem >> stepSvg;
	sstrem >> Svg_file_name;
	sstrem >> Svg_shot_dir_name;
	outputSvg = !Svg_file_name.empty();

	// storing the Png capture values
	std::getline(confFin, line);
	sstrem.clear();
	sstrem.str(line);
	// sstrem = std::stringstream(line);
	sstrem >> beginPng;
	sstrem >> endPng;
	sstrem >> stepPng;
	sstrem >> Png_file_name;
	sstrem >> Png_shot_dir_name;
	outputPng = !Png_file_name.empty();

	// storing the Jpg capture values
	std::getline(confFin, line);
	sstrem.clear();
	sstrem.str(line);
	// sstrem = std::stringstream(line);
	sstrem >> ID;
	sstrem >> beginJpg;
	sstrem >> endJpg;
	sstrem >> stepJpg;
	sstrem >> Jpg_file_name;
	sstrem >> Jpg_shot_dir_name;
	outputJpg = !Jpg_file_name.empty();

	//std::cout << "LINE: " + line + "\n";
	//std::cout << "VALUES: \n";
	//std::cout << beginJpg;
	//std::cout << " ";
	//std::cout << endJpg;
	//std::cout << " ";
	//std::cout << stepJpg;
	//std::cout << " ";
	//std::cout << Jpg_file_name;
	//std::cout << " ";
	//std::cout << Jpg_shot_dir_name;
	//std::cout << " ";
	//std::cout << "\n";

	// /rendering_files
	std::getline(confFin, line);

	////////////////////////////
	////// UDP hosts
	IP_Servers = NULL;
	nb_IP_Servers = 0;

	IP_Clients = NULL;
	nb_IP_Clients = 0;

	// shader file names
	pg_Shader_Files = NULL;
	nb_shader_files = 0;

	// Number of servers
	std::getline(confFin, line);
	sstrem.clear();
	sstrem.str(line);
	// sstrem = std::stringstream(line);
	sstrem >> ID;
	sstrem >> nb_IP_Servers;
	// std::cout << "Nb servers: " << nb_IP_Servers << "\n";

	// VERBATIM
	std::getline(confFin, line);
	// TYPE
	std::getline(confFin, line);
	// ID
	std::getline(confFin, line);

	IP_Servers = new pg_IPServer *[nb_IP_Servers];
	for (int ind_IP_Server = 0; ind_IP_Server < nb_IP_Servers; ind_IP_Server++) {
		IP_Servers[ind_IP_Server] = new pg_IPServer();

		std::getline(confFin, line);
		sstrem.clear();
		sstrem.str(line);
		// sstrem = std::stringstream(line);
		sstrem >> ID; // string "server"
		sstrem >> IP_Servers[ind_IP_Server]->id;
		sstrem >> IP_Servers[ind_IP_Server]->Local_server_port;
		sstrem >> ID;
		for (int ind = 0; ind < Emptypg_UDPMessageFormat; ind++) {
			if (strcmp(ID.c_str(), pg_UDPMessageFormatString[ind]) == 0) {
				IP_Servers[ind_IP_Server]->receive_format = (pg_UDPMessageFormat)ind;
				break;
			}
		}
		if (IP_Servers[ind_IP_Server]->receive_format == Emptypg_UDPMessageFormat) {
			sprintf(ErrorStr, "Error: unknown receive message format [%s]!", ID.c_str()); ReportError(ErrorStr); throw 249;
		}
		sstrem >> IP_Servers[ind_IP_Server]->IP_message_trace;
		sstrem >> IP_Servers[ind_IP_Server]->depth_input_stack;
		sstrem >> IP_Servers[ind_IP_Server]->OSC_duplicate_removal;
		sstrem >> IP_Servers[ind_IP_Server]->OSC_endian_reversal;
	}

	// /udp_local_server
	std::getline(confFin, line);

	// udp_remote_client Number of clients
	std::getline(confFin, line);
	sstrem.clear();
	sstrem.str(line);
	// sstrem = std::stringstream(line);
	sstrem >> ID;
	sstrem >> nb_IP_Clients;
	// std::cout << "Nb clients: " << nb_IP_Clients << "\n";

	// VERBATIM
	std::getline(confFin, line);
	// TYPE
	std::getline(confFin, line);
	// ID
	std::getline(confFin, line);

	IP_Clients = new pg_IPClient *[nb_IP_Clients];
	for (int ind_IP_Client = 0; ind_IP_Client < nb_IP_Clients; ind_IP_Client++) {
		IP_Clients[ind_IP_Client] = new pg_IPClient();

		std::getline(confFin, line);
		sstrem.clear();
		sstrem.str(line);
		// sstrem = std::stringstream(line);
		sstrem >> ID; // string "Client"
		sstrem >> IP_Clients[ind_IP_Client]->id;
		sstrem >> IP_Clients[ind_IP_Client]->Remote_server_IP;
		sstrem >> IP_Clients[ind_IP_Client]->Remote_server_port;
		sstrem >> ID;
		for (int ind = 0; ind < Emptypg_UDPMessageFormat; ind++) {
			if (strcmp(ID.c_str(), pg_UDPMessageFormatString[ind]) == 0) {
				IP_Clients[ind_IP_Client]->send_format = (pg_UDPMessageFormat)ind;
				break;
			}
		}
		if (IP_Clients[ind_IP_Client]->send_format == Emptypg_UDPMessageFormat) {
			sprintf(ErrorStr, "Error: unknown receive message format [%s]!", ID.c_str()); ReportError(ErrorStr); throw 249;
		}
		sstrem >> IP_Clients[ind_IP_Client]->IP_message_trace;
		sstrem >> IP_Clients[ind_IP_Client]->depth_output_stack;
		sstrem >> IP_Clients[ind_IP_Client]->OSC_endian_reversal;
		//std::cout << "OSC_endian_reversal: " << IP_Clients[ind_IP_Client]->OSC_endian_reversal << "\n";
		//std::cout << "IP_Clients[ ind_IP_Client ]->id: " << IP_Clients[ind_IP_Client]->id << "\n";
		//std::cout << "IP_Clients[ ind_IP_Client ]->Remote_server_IP: " << ind_IP_Client << " " << IP_Clients[ind_IP_Client]->Remote_server_IP << "\n";
		//std::cout << "IP_Clients[ ind_IP_Client ]->Remote_server_port: " << IP_Clients[ind_IP_Client]->Remote_server_port << "\n";
	}

	// /udp_remote_client
	std::getline(confFin, line);

	// shader_files Number of files
	std::getline(confFin, line);
	sstrem.clear();
	sstrem.str(line);
	// sstrem = std::stringstream(line);
	sstrem >> ID;
	sstrem >> nb_shader_files;
	// std::cout << "nb_shader_files: " << nb_shader_files << "\n";
	if (nb_shader_files != pg_NbShaderTotal) {
		sprintf(ErrorStr, "Error: number of shader file names does not match expectation [%d/%d]!", nb_shader_files, pg_NbShaderTotal); ReportError(ErrorStr); throw 429;
	}

	// VERBATIM
	std::getline(confFin, line);
	// TYPE
	std::getline(confFin, line);
	// ID
	std::getline(confFin, line);

	pg_Shader_Files = new string[nb_shader_files];
	pg_Shader_nbTypes = new int[nb_shader_files];
	pg_Shader_Types = new GLenum *[nb_shader_files];
	for (int ind_shader_file = 0; ind_shader_file < nb_shader_files; ind_shader_file++) {
		std::getline(confFin, line);
		sstrem.clear();
		sstrem.str(line);
		// sstrem = std::stringstream(line);
		sstrem >> ID; // string "filename"
		sstrem >> pg_Shader_Files[ind_shader_file];
		sstrem >> pg_Shader_nbTypes[ind_shader_file];
		pg_Shader_Types[ind_shader_file] = new GLenum[pg_Shader_nbTypes[ind_shader_file]];
		// std::cout << "pg_Shader_Files[ind_shader_file]: " << pg_Shader_Files[ind_shader_file] << "\n";
		for (int ind_shader_type = 0; ind_shader_type < pg_Shader_nbTypes[ind_shader_file]; ind_shader_type++) {
			string shader_type;
			sstrem >> shader_type;
			if (shader_type.compare("GL_VERTEX_SHADER") == 0) {
				pg_Shader_Types[ind_shader_file][ind_shader_type] = GL_VERTEX_SHADER;
			}
			else if (shader_type.compare("GL_TESS_CONTROL_SHADER") == 0) {
				pg_Shader_Types[ind_shader_file][ind_shader_type] = GL_TESS_CONTROL_SHADER;
			}
			else if (shader_type.compare("GL_TESS_EVALUATION_SHADER") == 0) {
				pg_Shader_Types[ind_shader_file][ind_shader_type] = GL_TESS_EVALUATION_SHADER;
			}
			else if (shader_type.compare("GL_GEOMETRY_SHADER") == 0) {
				pg_Shader_Types[ind_shader_file][ind_shader_type] = GL_GEOMETRY_SHADER;
			}
			else if (shader_type.compare("GL_FRAGMENT_SHADER") == 0) {
				pg_Shader_Types[ind_shader_file][ind_shader_type] = GL_FRAGMENT_SHADER;
			}
			else {
				sprintf(ErrorStr, "Error: unknown shader type [%s]!", shader_type.c_str()); ReportError(ErrorStr); throw 430;
			}
		}
	}

	// /shader_files
	std::getline(confFin, line);

	// /head
	std::getline(confFin, line);

	/////////////////////////////////////////////////////////////////
	// ASSIGNS INITIAL VALUES TO ALL CONFIGURATION FILES
	InitializeConfigurationVar();


	/////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////
	// SCENARIO FILE
	/////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////

	// RANK
	std::getline(scenarioFin, line);
	// VERBATIM
	std::getline(scenarioFin, line);
	// TYPE
	std::getline(scenarioFin, line);
	// ID
	std::getline(scenarioFin, line);
	// FUNCTION
	std::getline(scenarioFin, line);
	// GUI
	std::getline(scenarioFin, line);
	// SHADER
	std::getline(scenarioFin, line);
	// CHAR-
	std::getline(scenarioFin, line);
	// CHAR+
	std::getline(scenarioFin, line);
	// STEP-
	std::getline(scenarioFin, line);
	// STEP+
	std::getline(scenarioFin, line);
	// MIN_OR_EQUAL
	std::getline(scenarioFin, line);
	// MAX_OR_EQUAL
	std::getline(scenarioFin, line);

	// initial_values
	std::getline(scenarioFin, line);
	// storing the initial values
	std::getline(scenarioFin, line);
	sstrem.clear();
	sstrem.str(line);
	// sstrem = std::stringstream(line);
	//std::cout << "line :" << line;
	//std::cout << "\nvalues :\n";
	for (int indP = 0; indP < _MaxInterpVarIDs; indP++) {
		sstrem >> InitialValuesInterpVar[indP];
		//std::cout << InitialValuesInterpVar[indP] << " ";
		LastGUIShippedValuesInterpVar[indP] = MAXFLOAT;
	}
	//std::cout << "\n";
	// /initial_values
	std::getline(scenarioFin, line);


	////////////////////////////
	////// SCENARIO
	nb_scenes = 0;
	// Number of scenes
	std::getline(scenarioFin, line);
	sstrem.clear();
	sstrem.str(line);
	// sstrem = std::stringstream(line);
	sstrem >> ID; // string scenario
	sstrem >> nb_scenes;

	Scenario = new Scene[nb_scenes];

	//scene_IDs = new string[nb_scenes];
	//scene_duration = new float[nb_scenes];
	//scene_initial_time = new float[nb_scenes];
	//scene_final_time = new float[nb_scenes];
	//scene_originalDuration = new float[nb_scenes];
	//scene_originalInitial_time = new float[nb_scenes];
	//scene_originalFinal_time = new float[nb_scenes];
	for (int indScene = 0; indScene < nb_scenes; indScene++) {
		Scenario[indScene].scene_duration = 0.0;
		Scenario[indScene].scene_change_when_ends = true;
		Scenario[indScene].scene_initial_time = 0.0;
		Scenario[indScene].scene_final_time = 0.0;
		Scenario[indScene].scene_originalDuration = 0.0;
		Scenario[indScene].scene_originalInitial_time = 0.0;
		Scenario[indScene].scene_originalFinal_time = 0.0;
	
		Scenario[indScene].scene_initial_parameters = NULL;
		Scenario[indScene].scene_final_parameters = NULL;
		Scenario[indScene].scene_interpolations = NULL;
	}

	printf("Loading %d scenes with %d variables\n", nb_scenes, _MaxInterpVarIDs);
	for (int indScene = 0; indScene < nb_scenes; indScene++) {
		std::getline(scenarioFin, line);
		// std::cout << "scene: " << line << "\n";
		sstrem.clear();
		sstrem.str(line);
		// sstrem = std::stringstream(line);
		sstrem >> ID; // string scene
		sstrem >> Scenario[indScene].scene_IDs;
		sstrem >> Scenario[indScene].scene_duration;
		sstrem >> temp;  // change_when_ends or prolong_when_ends
		if (temp.compare("change_when_ends") == 0) {
			Scenario[indScene].scene_change_when_ends = true;
		}
		else if (temp.compare("prolong_when_ends") == 0) {
			Scenario[indScene].scene_change_when_ends = false;
		}
		else {
			Scenario[indScene].scene_change_when_ends = true;
			sprintf(ErrorStr, "Error: one of strings expected as scene ending mode: \"change_when_ends\" or \"prolong_when_ends\" not \"%s\"\n", temp.c_str()); ReportError(ErrorStr); throw 50;
		}

		//std::cout << "duration : " << 
		//  Scenario[indScene].scene_duration << " sc #" << indScene << "\n";

		if (Scenario[indScene].scene_duration <= 0.0) {
			sprintf(ErrorStr, "Error: null scene #%d duration [%f]!", indScene + 1, Scenario[indScene].scene_duration); ReportError(ErrorStr); throw 50;
		}
		if (indScene > 0) {
			Scenario[indScene].scene_initial_time = Scenario[indScene - 1].scene_final_time;
		}
		else {
			Scenario[indScene].scene_initial_time = 0.0;
		}
		Scenario[indScene].scene_final_time
			= Scenario[indScene].scene_initial_time + Scenario[indScene].scene_duration;

		Scenario[indScene].scene_initial_parameters = new float[_MaxInterpVarIDs];
		Scenario[indScene].scene_final_parameters = new float[_MaxInterpVarIDs];
		for (int indP = 0; indP < _MaxInterpVarIDs; indP++) {
			Scenario[indScene].scene_initial_parameters[indP] = 0.0;
			Scenario[indScene].scene_final_parameters[indP] = 0.0;
		}
		Scenario[indScene].scene_interpolations
			= new pg_Interpolation[_MaxInterpVarIDs];
		for (int indP = 0; indP < _MaxInterpVarIDs; indP++) {
			Scenario[indScene].scene_interpolations[indP].interpolation_mode
				= pg_linear_interpolation;
			Scenario[indScene].scene_interpolations[indP].offSet = 0.0;
			Scenario[indScene].scene_interpolations[indP].duration = 1.0;
		}

		// verbatim
		std::getline(scenarioFin, line);
		// std::cout << "verbatim: " << line << "\n";
		// var comment
		std::getline(scenarioFin, line);
		// std::cout << "var comment: " << line << "\n";

		// storing the initial values
		std::getline(scenarioFin, line);
		sstrem.clear();
		sstrem.str(line);
		// sstrem = std::stringstream(line);
		for (int indP = 0; indP < _MaxInterpVarIDs; indP++) {
			sstrem >> Scenario[indScene].scene_initial_parameters[indP];
		}
		// checks that the number of variables is what is expected
		//sstrem >> ID;
		if (!sstrem.eof()) {
			sprintf(ErrorStr, "Error: too many variables in scene %d\n", indScene + 1); ReportError(ErrorStr); throw 50;
		}

		// storing the final values
		std::getline(scenarioFin, line);
		sstrem.clear();
		sstrem.str(line);
		// sstrem = std::stringstream(line);
		//std::cout << "line :" << line;
		// std::cout << "\nfinal values :\n";
		for (int indP = 0; indP < _MaxInterpVarIDs; indP++) {
			sstrem >> Scenario[indScene].scene_final_parameters[indP];
			// std::cout << scene_final_parameters[indScene][indP] << " ";
		}
		// checks that the number of variables is what is expected
		//sstrem >> ID;
		if (!sstrem.eof()) {
			sprintf(ErrorStr, "Error: too many variables in scene %d\n", indScene + 1); ReportError(ErrorStr); throw 50;
		}

		std::getline(scenarioFin, line);
		sstrem.clear();
		sstrem.str(line);
		// std::cout << "interpolation: " << line << "\n";
		// sstrem = std::stringstream(line);

		// storing the interpolation mode
		for (int indP = 0; indP < _MaxInterpVarIDs; indP++) {
			char valCh = 0;
			float val, val2;

			Scenario[indScene].scene_interpolations[indP].offSet = 0.0;
			Scenario[indScene].scene_interpolations[indP].duration = 1.0;
			Scenario[indScene].scene_interpolations[indP].midTermValue
				= 0.5f * (Scenario[indScene].scene_initial_parameters[indP]
					+ Scenario[indScene].scene_final_parameters[indP]);

			sstrem >> std::skipws >> valCh;
			// printf("valch %d\n" , (int)valCh );

			switch (valCh) {
				// l: value interpolates linearly between initial and final value from 0.0% to 1.0%
				// L: value is initial from 0.0% until offset, 
				// interpolates linearly between initial and final value from offset to offset + duration
				// is final value between offset + duration and 1.0%
			case 'l':
			case 'L':
				Scenario[indScene].scene_interpolations[indP].interpolation_mode
					= pg_linear_interpolation;
				if (valCh == 'L') {
					sstrem >> val;
					sstrem >> val2;
					if (val < 0.0 || val2 < 0.0) {
						sprintf(ErrorStr, "Error: one of values of L(inear) interpolationn #%d lower than 0.0: %.3f %.3f\n", indP + 1, val, val2); ReportError(ErrorStr); throw 50;
					}
					if (val <= 1.0) {
						Scenario[indScene].scene_interpolations[indP].offSet = val;
						if (val + val2 <= 1.00001) {
							// deals with approximate values that can summ above 1.0
							if (val + val2 > 1.0) {
								val2 = 1.0f - val;
							}
							Scenario[indScene].scene_interpolations[indP].duration = val2;
							if (Scenario[indScene].scene_interpolations[indP].duration <= 0.0) {
								sprintf(ErrorStr, "Error: null L(inear) interpolation #%d duration [%f]!", indP + 1, Scenario[indScene].scene_interpolations[indP].duration); ReportError(ErrorStr); throw 50;
							}
						}
						else {
							sprintf(ErrorStr, "Error: total duration of L(inear) interpolation #%d greater than 1.0: %.3f + %.3f\n", indP + 1, val, val2); ReportError(ErrorStr); throw 50;
						}
					}
					else {
						sprintf(ErrorStr, "Error: offset value L(inear) interpolation #%d greater than 1.0: %.3f\n", indP + 1, val); ReportError(ErrorStr); throw 50;
					}
					// std::cout << "L " << val << " " << val2 << " ";

				}
				break;
				// c: value interpolates cosinely between initial and final value from 0.0% to 1.0%
				// C: value is initial from 0.0% until offset, 
				// interpolates cosinely between initial and final value from offset to offset + duration
				// is final value between offset + duration and 1.0%
				// COSINE INTERPOLATION
			case 'c':
			case 'C':
				Scenario[indScene].scene_interpolations[indP].interpolation_mode
					= pg_cosine_interpolation;
				if (valCh == 'C') {
					sstrem >> val;
					sstrem >> val2;
					if (val < 0.0 || val2 < 0.0) {
						sprintf(ErrorStr, "Error: one of values of C(osine) interpolation #%d lower than 0.0: %.3f %.3f\n", indP + 1, val, val2); ReportError(ErrorStr); throw 50;
					}
					if (val <= 1.0) {
						Scenario[indScene].scene_interpolations[indP].offSet = val;
						// deals with approximate values that can summ above 1.0
						if (val + val2 <= 1.00001) {
							if (val + val2 > 1.0) {
								val2 = 1.0f - val;
							}
							Scenario[indScene].scene_interpolations[indP].duration = val2;
							if (Scenario[indScene].scene_interpolations[indP].duration <= 0.0) {
								sprintf(ErrorStr, "Error: null C(osine) interpolation #%d duration [%f]!", indP + 1, Scenario[indScene].scene_interpolations[indP].duration); ReportError(ErrorStr); throw 50;
							}
						}
						else {
							sprintf(ErrorStr, "Error: total duration of C(osine) interpolation #%d greater than 1.0: %.3f + %.3f\n", indP + 1, val, val2); ReportError(ErrorStr); throw 50;
						}
					}
					else {
						sprintf(ErrorStr, "Error: offset value C(osine) interpolation #%d greater than 1.0: %.3f\n", indP + 1, val); ReportError(ErrorStr); throw 50;
					}
				}
				break;
				// b: bell curve interpolation between initial, median and final value from (0,0,0)% to (0,1,0)% at mid time to (0,0,1)% at the end
				// BELL INTERPOLATION
			case 'b':
				Scenario[indScene].scene_interpolations[indP].interpolation_mode
					= pg_bell_interpolation;
				sstrem >> Scenario[indScene].scene_interpolations[indP].midTermValue;
				break;
				// b: saw tooth linear interpolation between initial, median and final value from (0,0,0)% to (0,1,0)% at mid time to (0,0,1)% at the end
				// SAW TOOTH INTERPOLATION
			case 't':
				Scenario[indScene].scene_interpolations[indP].interpolation_mode
					= pg_sawtooth_interpolation;
				sstrem >> Scenario[indScene].scene_interpolations[indP].midTermValue;
				break;
			case 's':
			case 'S':
				Scenario[indScene].scene_interpolations[indP].interpolation_mode
					= pg_stepwise_interpolation;
				Scenario[indScene].scene_interpolations[indP].offSet = 1.0;
				Scenario[indScene].scene_interpolations[indP].duration = 0.0;
				if (valCh == 'S') {
					sstrem >> val;
					if (val < 0.0) {
						sprintf(ErrorStr, "Error: offset values of S(tepwise) interpolation #%d lower than 0.0: %.3f\n", indP + 1, val); ReportError(ErrorStr); throw 50;
					}
					if (val <= 1.0) {
						Scenario[indScene].scene_interpolations[indP].offSet = val;
					}
					else {
						sprintf(ErrorStr, "Error: offset value of S(tepwise) interpolation #%d greater than 1.0: %.3f\n", indP + 1, val); ReportError(ErrorStr); throw 50;
					}
				}
				break;
			case 'k':
			case 'K':
				Scenario[indScene].scene_interpolations[indP].interpolation_mode
					= pg_keep_value;
				break;
			default:
				sprintf(ErrorStr, "Error: unknown  interpolation mode in scene %d parameter %d [%c]!", indScene + 1, indP + 1, valCh); ReportError(ErrorStr); throw 50;
				break;
			}
		}

		// /scene
		std::getline(scenarioFin, line);
		sstrem.clear();
		sstrem.str(line);
		sstrem >> ID; // string /scene
		if (ID.compare("/scene") != 0) {
			sprintf(ErrorStr, "Error: incorrect configuration file expected string \"/scene\" not found! (instead \"%s\")", ID.c_str()); ReportError(ErrorStr); throw 100;
		}

	}
	// /scenario
	std::getline(scenarioFin, line);
	sstrem.clear();
	sstrem.str(line);
	sstrem >> ID; // string /scenario
	if (ID.compare("/scenario") != 0) {
		sprintf(ErrorStr, "Error: incorrect configuration file expected string \"/scenario\" not found! (instead \"%s\")", ID.c_str()); ReportError(ErrorStr); throw 100;
	}

	printf("scenario loaded\n");

	////////////////////////////
	////// VIDEOS
	nb_movies = 0;
	// Number of videos
	std::getline(scenarioFin, line);
	sstrem.clear();
	sstrem.str(line);
	// sstrem = std::stringstream(line);
	sstrem >> ID; // string videos
	sstrem >> nb_movies;

	for (int indVideo = 0; indVideo < nb_movies; indVideo++) {
		std::getline(scenarioFin, line);
		// std::cout << "scene: " << line << "\n";
		sstrem.clear();
		sstrem.str(line);
		// sstrem = std::stringstream(line);
		sstrem >> ID; // string movie
		sstrem >> temp;
		movieFileName.push_back(temp);
		sstrem >> temp2;
		movieShortName.push_back(temp2);
		//std::cout << "movie : " << 
		// movieFileName[indVideo] << "\n";
	}
	// /videos
	std::getline(scenarioFin, line);
	sstrem.clear();
	sstrem.str(line);
	sstrem >> ID; // string /videos
	if (ID.compare("/videos") != 0) {
		sprintf(ErrorStr, "Error: incorrect configuration file expected string \"/videos\" not found! (instead \"%s\")", ID.c_str()); ReportError(ErrorStr); throw 100;
	}

	////////////////////////////
	////// PHOTO ALBUMS
	nb_photo_albums = 0;
	// Number of photo albums
	std::getline(scenarioFin, line);
	sstrem.clear();
	sstrem.str(line);
	// sstrem = std::stringstream(line);
	sstrem >> ID; // string videos
	sstrem >> nb_photo_albums;

	for (int indAlbum = 0; indAlbum < nb_photo_albums; indAlbum++) {
		std::getline(scenarioFin, line);
		// std::cout << "scene: " << line << "\n";
		sstrem.clear();
		sstrem.str(line);
		// sstrem = std::stringstream(line);
		sstrem >> ID; // string movie
		sstrem >> temp;
		photoAlbumDirName.push_back(temp);
		//std::cout << "movie : " << 
		// photoAlbumDirName[indVideo] << "\n";
	}
	// /photos
	std::getline(scenarioFin, line);
	sstrem.clear();
	sstrem.str(line);
	sstrem >> ID; // string /photos
	if (ID.compare("/photos") != 0) {
		sprintf(ErrorStr, "Error: incorrect configuration file expected string \"/photos\" not found! (instead \"%s\")", ID.c_str()); ReportError(ErrorStr); throw 100;
	}

	////////////////////////////
	////// BRUSHES
	nb_pen_brushes = 0;
	// Number of brushes
	std::getline(scenarioFin, line);
	sstrem.clear();
	sstrem.str(line);
	// sstrem = std::stringstream(line);
	sstrem >> ID; // string brushes
	sstrem >> nb_pen_brushes;

	std::getline(scenarioFin, line);
	// std::cout << "scene: " << line << "\n";
	sstrem.clear();
	sstrem.str(line);
	// sstrem = std::stringstream(line);
	sstrem >> ID; // string ID
	sstrem >> pen_brushes_fileName;
	//std::cout << "brush : " << 
	// pen_brushes_fileName << "\n";

	// /brushes
	std::getline(scenarioFin, line);
	sstrem.clear();
	sstrem.str(line);
	sstrem >> ID; // string /brushes
	if (ID.compare("/brushes") != 0) {
		sprintf(ErrorStr, "Error: incorrect configuration file expected string \"/brushes\" not found! (instead \"%s\")", ID.c_str()); ReportError(ErrorStr); throw 100;
	}

	////////////////////////////
	////// SOUNDTRACKS
	nb_soundtracks = 0;
	// Number of soundtracks
	std::getline(scenarioFin, line);
	sstrem.clear();
	sstrem.str(line);
	// sstrem = std::stringstream(line);
	sstrem >> ID; // string soundtracks
	sstrem >> nb_soundtracks;

	for (int indTrack = 0; indTrack < nb_soundtracks; indTrack++) {
		std::getline(scenarioFin, line);
		// std::cout << "scene: " << line << "\n";
		sstrem.clear();
		sstrem.str(line);
		// sstrem = std::stringstream(line);
		sstrem >> ID; // string track
		sstrem >> temp;
		trackFileName.push_back(temp);
		sstrem >> temp2;
		trackShortName.push_back(temp2);
		std::cout << "Soundtrack: " << trackFileName[indTrack] << " " << trackShortName[indTrack] << " (#" << indTrack << ")\n";
	}
	// /soundtracks
	std::getline(scenarioFin, line);
	sstrem.clear();
	sstrem.str(line);
	sstrem >> ID; // string /soundtracks
	if (ID.compare("/soundtracks") != 0) {
		sprintf(ErrorStr, "Error: incorrect configuration file expected string \"/soundtracks\" not found! (instead \"%s\")", ID.c_str()); ReportError(ErrorStr); throw 100;
	}

	////////////////////////////
	////// SVG PATHS
	// initializes the tracks for recording the strokes
	// has to be made before loading the scenario that may load predefined svg paths
	// and has to be made after the configuration file loading that has 
	// the value max_mouse_recording_frames for the max number of points to memorize
	pg_initStrokes();

	// Number of SVG paths
	std::getline(scenarioFin, line);
	sstrem.clear();
	sstrem.str(line);
	// sstrem = std::stringstream(line);
	sstrem >> ID; // string svg_paths
	int nb_svg_paths = 0;
	sstrem >> nb_svg_paths;

	for (int indPrerecPath = 0; indPrerecPath < nb_svg_paths; indPrerecPath++) {
		std::getline(scenarioFin, line);
		// std::cout << "scene: " << line << "\n";
		sstrem.clear();
		sstrem.str(line);
		// sstrem = std::stringstream(line);
		sstrem >> ID; // string track
		sstrem >> temp;
		sstrem >> temp2;
		int indPath = std::stoi(temp2);
		sstrem >> temp2;
		int indTrack = std::stoi(temp2);
		sstrem >> temp2;
		float pathRadius = std::stof(temp2);
		sstrem >> temp2;
		float path_r_color = std::stof(temp2);
		sstrem >> temp2;
		float path_g_color = std::stof(temp2);
		sstrem >> temp2;
		float path_b_color = std::stof(temp2);
		if (indTrack >= 0 && indTrack < PG_NB_TRACKS && indPath >= 1 && indPath <= PG_NB_PATHS) {
			load_svg_path((char *)("Data/" + project_name + "-data/SVGs/" + temp).c_str(),
				indPath, indTrack, pathRadius, path_r_color, path_g_color, path_b_color);
		}
		else {
			sprintf(ErrorStr, "Error: incorrect configuration file track %d or path %d number (\"%s\")", 
				indTrack, indPath, temp2.c_str()); ReportError(ErrorStr); throw 100;
		}
		// std::cout << "svg_path #" << indPath << ": " << "Data/" + project_name + "-data/SVGs/" + temp << " track #" << indTrack << "\n";
	}
	// /soundtracks
	std::getline(scenarioFin, line);
	sstrem.clear();
	sstrem.str(line);
	sstrem >> ID; // string /soundtracks
	if (ID.compare("/svg_paths") != 0) {
		sprintf(ErrorStr, "Error: incorrect configuration file expected string \"/svg_paths\" not found! (instead \"%s\")", ID.c_str()); ReportError(ErrorStr); throw 100;
	}


	////////////////////////////
	////// PALETTE COLORSS
	nb_pen_palette_colors = 0;
	// Number of palettes
	std::getline(scenarioFin, line);
	sstrem.clear();
	sstrem.str(line);
	// sstrem = std::stringstream(line);
	sstrem >> ID; // string palette_colors
	sstrem >> nb_pen_palette_colors;

	// std::cout << "line: " << line << "\n";
	// std::cout << "Nb palettes : " << nb_pen_palette_colors <<  "\n";

	pen_palette_colors_values = new float *[nb_pen_palette_colors];
	for (int indColorPalette = 0; indColorPalette < nb_pen_palette_colors; indColorPalette++) {
		pen_palette_colors_values[indColorPalette] = new float[9];

		std::getline(scenarioFin, line);
		// std::cout << "scene: " << line << "\n";
		sstrem.clear();
		sstrem.str(line);
		// sstrem = std::stringstream(line);
		sstrem >> ID; // palette ID
		for (int indColorChannel = 0; indColorChannel < 9; indColorChannel++) {
			sstrem >> pen_palette_colors_values[indColorPalette][indColorChannel];
			pen_palette_colors_values[indColorPalette][indColorChannel] /= 255.f;
		}
		pen_palette_colors_names.push_back(ID.c_str());
		// std::cout << "palettes : " << indPalette << " " <<  pen_palettes_names[indPalette] << " " << pen_palettes_values[indPalette] << "\n";
	}
	// /palettes
	std::getline(scenarioFin, line);
	sstrem.clear();
	sstrem.str(line);
	sstrem >> ID; // string /palette_colors
	if (ID.compare("/palette_colors") != 0) {
		sprintf(ErrorStr, "Error: incorrect configuration file expected string \"/palettes\" not found! (instead \"%s\")", ID.c_str()); ReportError(ErrorStr); throw 100;
	}

	////////////////////////////
	////// PALETTES
	nb_pen_colorPresets = 0;
	// Number of palettes
	std::getline(scenarioFin, line);
	sstrem.clear();
	sstrem.str(line);
	// sstrem = std::stringstream(line);
	sstrem >> ID; // string color_presets
	sstrem >> nb_pen_colorPresets;

	// std::cout << "line: " << line << "\n";
	// std::cout << "Nb palettes : " << nb_pen_colorPresets <<  "\n";

	pen_colorPreset_values = new float[nb_pen_colorPresets];
	for (int indPalette = 0; indPalette < nb_pen_colorPresets; indPalette++) {
		std::getline(scenarioFin, line);
		// std::cout << "scene: " << line << "\n";
		sstrem.clear();
		sstrem.str(line);
		// sstrem = std::stringstream(line);
		sstrem >> ID; // palette ID
		sstrem >> pen_colorPreset_values[indPalette];
		pen_colorPresets_names.push_back(ID.c_str());
		// std::cout << "palettes : " << indPalette << " " <<  pen_colorPresets_names[indPalette] << " " << pen_colorPresets_values[indPalette] << "\n";
	}
	// /palettes
	std::getline(scenarioFin, line);
	sstrem.clear();
	sstrem.str(line);
	sstrem >> ID; // string /palettes
	if (ID.compare("/color_presets") != 0) {
		sprintf(ErrorStr, "Error: incorrect configuration file expected string \"/palettes\" not found! (instead \"%s\")", ID.c_str()); ReportError(ErrorStr); throw 100;
	}

	// saves the original durations
	saveInitialTimesAndDurations();
}

// to be called once after parsing
void saveInitialTimesAndDurations(void) {
	for (int indScene = 0; indScene < nb_scenes; indScene++) {
		Scenario[indScene].scene_originalDuration = Scenario[indScene].scene_duration;
		Scenario[indScene].scene_originalInitial_time = Scenario[indScene].scene_initial_time;
		Scenario[indScene].scene_originalFinal_time = Scenario[indScene].scene_final_time;
	}
}
// to be called before setup change
void restoreInitialTimesAndDurations(void) {
	for (int indScene = 0; indScene < nb_scenes; indScene++) {
		Scenario[indScene].scene_duration = Scenario[indScene].scene_originalDuration;
		Scenario[indScene].scene_initial_time = Scenario[indScene].scene_originalInitial_time;
		Scenario[indScene].scene_final_time = Scenario[indScene].scene_originalFinal_time;
	}
}

void LoadConfigurationFile( const char * confFileName , const char * scenarioFileName ) {
  printf( "Loading %s & %s\n" , confFileName , scenarioFileName );

  std::ifstream confFin(confFileName);
  if( !confFin ) {
    sprintf( ErrorStr , "Error: file [%s] not found!" , confFileName ); ReportError( ErrorStr ); throw 11;
  }
  std::ifstream scenarioFin(scenarioFileName);
  if( !scenarioFin ) {
    sprintf( ErrorStr , "Error: file [%s] not found!" , scenarioFileName ); ReportError( ErrorStr ); throw 11;
  }

  parseConfigurationFile( confFin , scenarioFin );

  confFin.close();
  scenarioFin.close();

  //std::cout << "\n*** max_network_message_length/max_mouse_recording_frames/minimal_interframe_latency: " 
  //  << max_network_message_length <<  << " " << max_mouse_recording_frames << " " << minimal_interframe_latency "\n";
  //std::cout << "\n*** window size: " << window_width << " " << window_height << " " << window_x << " " << window_y << "\n";

}

