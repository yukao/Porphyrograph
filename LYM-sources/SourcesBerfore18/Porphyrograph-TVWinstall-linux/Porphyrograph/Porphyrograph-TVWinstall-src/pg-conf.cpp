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

int                      NbTotScenes;
float                   *scene_durations;
float                   *scene_originalDurations;
float                   *scene_initial_times;
float                   *scene_originalInitial_times;
float                   *scene_final_times;
float                   *scene_originalFinal_times;
string                  *scene_IDs;
float                  **scene_initial_parameters;
float                  **scene_final_parameters;
pg_Interpolation       **scene_interpolations;

string                  log_file_name            ;
string				    snapshots_dir_path_name;
string                  screen_font_file_name;
string                  display_font_file_name;
string                  hashtag_font_file_name;
string                  font_file_encoding;
TextureEncoding         font_texture_encoding;

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

void parseConfigurationFile( std::ifstream& confFin , std::ifstream&  scenarioFin ) {
  CurrentWindow = new pg_Window();

  ////////////////////////////
  ////// CONFIGURATION VARIABLE INITIALIZATION
  string line;
  string ID;

  /////////////////////////////////////////////////////////////////
  // CONFIGURATION FILE

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

  screen_font_file_name = "./ProjectsBefore18/" + project_name + "/fonts/usascii/arial/stb_font_arial_15_usascii.png";
  display_font_file_name = "./ProjectsBefore18/" + project_name + "/fonts/usascii/arial/stb_font_arial_25_usascii.png";
  hashtag_font_file_name = "./ProjectsBefore18/" + project_name + "/textures/hashtagFont.png";
  font_file_encoding = "png";
  font_texture_encoding = PNG;
  snapshots_dir_path_name = "./snapshots/pic_" + project_name + "_" + date_stringStream.str() + "/";
  log_file_name = snapshots_dir_path_name + "log-LYM-" + project_name + "-" + date_stringStream.str() + ".log";

  int nError = 0;
#ifdef _WIN32
  nError = CreateDirectoryA(snapshots_dir_path_name.c_str(), NULL); // can be used on Windows
#else
  nError = mkdir(snapshots_dir_path_name.c_str(), S_IRWXU); // can be used on non-Windows
#endif
  if (nError != 0) {
	  // handle your error here
  }

  std::getline(confFin, line);
  std::stringstream  ss(line);
  // std::cout << "VALUES: \n";
  for (int indP = 0; indP < _MaxConfigurationVarIDs; indP++) {
    ss >> InitialValuesConfigurationVar[ indP ];
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
  ss.clear();
  ss.str(line);
  // ss = std::stringstream(line);
  ss >> beginVideo_file;
  ss >> endVideo_file;
  ss >> stepVideo_file;
  ss >> Video_file_name;
  outputVideo_file = !Video_file_name.empty();

  // storing the Svg capture values
  std::getline(confFin, line);
  ss.clear();
  ss.str(line);
  // ss = std::stringstream(line);
  ss >> beginSvg;
  ss >> endSvg;
  ss >> stepSvg;
  ss >> Svg_file_name;
  ss >> Svg_shot_dir_name;
  outputSvg = !Svg_file_name.empty();

  // storing the Png capture values
  std::getline(confFin, line);
  ss.clear();
  ss.str(line);
  // ss = std::stringstream(line);
  ss >> beginPng;
  ss >> endPng;
  ss >> stepPng;
  ss >> Png_file_name;
  ss >> Png_shot_dir_name;
  outputPng = !Png_file_name.empty();

  // storing the Jpg capture values
  std::getline(confFin, line);
  ss.clear();
  ss.str(line);
  // ss = std::stringstream(line);
  ss >> ID;
  ss >> beginJpg;
  ss >> endJpg;
  ss >> stepJpg;
  ss >> Jpg_file_name;
  ss >> Jpg_shot_dir_name;
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

  // Number of servers
  std::getline(confFin, line);
  ss.clear();
  ss.str(line);
  // ss = std::stringstream(line);
  ss >> ID;
  ss >> nb_IP_Servers;
  // std::cout << "Nb servers: " << nb_IP_Servers << "\n";

  // VERBATIM
  std::getline(confFin, line);
  // TYPE
  std::getline(confFin, line);
  // ID
  std::getline(confFin, line);

  IP_Servers = new pg_IPServer *[ nb_IP_Servers ];
  for( int ind_IP_Server = 0 ; ind_IP_Server < nb_IP_Servers ; ind_IP_Server++ ) {
    IP_Servers[ ind_IP_Server ] = new pg_IPServer();

    std::getline(confFin, line);
    ss.clear();
    ss.str(line);
    // ss = std::stringstream(line);
    ss >> ID; // string "server"
    ss >> IP_Servers[ ind_IP_Server ]->id;
    ss >> IP_Servers[ ind_IP_Server ]->Local_server_port;
    ss >> ID;
    for( int ind = 0 ; ind < Emptypg_UDPMessageFormat ; ind++ ) {
      if( strcmp( ID.c_str() , pg_UDPMessageFormatString[ind] ) == 0 ) {
		IP_Servers[ ind_IP_Server ]->receive_format = (pg_UDPMessageFormat)ind;
		break;
      }
    }
    if( IP_Servers[ ind_IP_Server ]->receive_format == Emptypg_UDPMessageFormat ) {
      sprintf( ErrorStr , "Error: unknown receive message format [%s]!" , ID.c_str() ); ReportError( ErrorStr ); throw 249;
    }
    ss >> IP_Servers[ ind_IP_Server ]->IP_message_trace;
    ss >> IP_Servers[ ind_IP_Server ]->depth_input_stack;
    ss >> IP_Servers[ ind_IP_Server ]->OSC_duplicate_removal;
    ss >> IP_Servers[ ind_IP_Server ]->OSC_endian_reversal;
  }

  // /udp_local_server
  std::getline(confFin, line);

  // Number of clients
  std::getline(confFin, line);
  ss.clear();
  ss.str(line);
  // ss = std::stringstream(line);
  ss >> ID;
  ss >> nb_IP_Clients;
  // std::cout << "Nb clients: " << nb_IP_Clients << "\n";

  // VERBATIM
  std::getline(confFin, line);
  // TYPE
  std::getline(confFin, line);
  // ID
  std::getline(confFin, line);

  IP_Clients = new pg_IPClient *[ nb_IP_Clients ];
  for( int ind_IP_Client = 0 ; ind_IP_Client < nb_IP_Clients ; ind_IP_Client++ ) {
    IP_Clients[ ind_IP_Client ] = new pg_IPClient();

    std::getline(confFin, line);
    ss.clear();
    ss.str(line);
    // ss = std::stringstream(line);
    ss >> ID; // string "Client"
    ss >> IP_Clients[ ind_IP_Client ]->id;
    ss >> IP_Clients[ ind_IP_Client ]->Remote_server_IP;
    ss >> IP_Clients[ ind_IP_Client ]->Remote_server_port;
    ss >> ID;
    for( int ind = 0 ; ind < Emptypg_UDPMessageFormat ; ind++ ) {
      if( strcmp( ID.c_str() , pg_UDPMessageFormatString[ind] ) == 0 ) {
	IP_Clients[ ind_IP_Client ]->send_format = (pg_UDPMessageFormat)ind;
	break;
      }
    }
    if( IP_Clients[ ind_IP_Client ]->send_format == Emptypg_UDPMessageFormat ) {
      sprintf( ErrorStr , "Error: unknown receive message format [%s]!" , ID.c_str() ); ReportError( ErrorStr ); throw 249;
    }
    ss >> IP_Clients[ ind_IP_Client ]->IP_message_trace;
    ss >> IP_Clients[ ind_IP_Client ]->depth_output_stack;
    ss >> IP_Clients[ ind_IP_Client ]->OSC_endian_reversal;
	//std::cout << "OSC_endian_reversal: " << IP_Clients[ind_IP_Client]->OSC_endian_reversal << "\n";
	//std::cout << "IP_Clients[ ind_IP_Client ]->id: " << IP_Clients[ind_IP_Client]->id << "\n";
	//std::cout << "IP_Clients[ ind_IP_Client ]->Remote_server_IP: " << ind_IP_Client << " " << IP_Clients[ind_IP_Client]->Remote_server_IP << "\n";
	//std::cout << "IP_Clients[ ind_IP_Client ]->Remote_server_port: " << IP_Clients[ind_IP_Client]->Remote_server_port << "\n";

	IP_Clients[ ind_IP_Client ]->nbEmissionFailures = 0;
  }

  // /udp_remote_client
  std::getline(confFin, line);

  // /head
  std::getline(confFin, line);

  /////////////////////////////////////////////////////////////////
  // SCENARIO FILE

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
  ss.clear();
  ss.str(line);
  // ss = std::stringstream(line);
  //std::cout << "line :" << line;
  //std::cout << "\nvalues :\n";
  for (int indP = 0; indP < _MaxInterpVarIDs; indP++) {
    ss >> InitialValuesInterpVar[ indP ];
	//std::cout << InitialValuesInterpVar[indP] << " ";
  }
  //std::cout << "\n";
  // /initial_values
  std::getline(scenarioFin, line);

  ////////////////////////////
  ////// SCENARIO
  NbTotScenes = 0;
  // Number of scenes
  std::getline(scenarioFin, line);
  ss.clear();
  ss.str(line);
  // ss = std::stringstream(line);
  ss >> ID; // string scenario
  ss >> NbTotScenes;

  scene_IDs = new string[NbTotScenes];
  scene_durations = new float[ NbTotScenes ];
  scene_initial_times = new float[ NbTotScenes ];
  scene_final_times = new float[ NbTotScenes ];
  scene_originalDurations = new float[ NbTotScenes ];
  scene_originalInitial_times = new float[ NbTotScenes ];
  scene_originalFinal_times = new float[ NbTotScenes ];
  for(int indScene = 0 ; indScene < NbTotScenes ; indScene++ ) {
    scene_durations[ indScene ] = 0.0;
    scene_initial_times[ indScene ] = 0.0;
    scene_final_times[ indScene ] = 0.0;
    scene_originalDurations[ indScene ] = 0.0;
    scene_originalInitial_times[ indScene ] = 0.0;
    scene_originalFinal_times[ indScene ] = 0.0;
  }

  scene_initial_parameters = new float *[ NbTotScenes ];
  scene_final_parameters = new float *[ NbTotScenes ];
  for(int indScene = 0 ; indScene < NbTotScenes ; indScene++ ) {
    scene_initial_parameters[ indScene ] = NULL;
    scene_final_parameters[ indScene ] = NULL;
  }

  scene_interpolations = new pg_Interpolation *[ NbTotScenes ];
  for(int indScene = 0 ; indScene < NbTotScenes ; indScene++ ) {
    scene_interpolations[ indScene ] = NULL;
  }

  printf("Loading %d scenes with %d variables\n" , NbTotScenes , _MaxInterpVarIDs );
  for(int indScene = 0 ; indScene < NbTotScenes ; indScene++ ) {
	std::getline(scenarioFin, line);
	// std::cout << "scene: " << line << "\n";
    ss.clear();
    ss.str(line);
    // ss = std::stringstream(line);
    ss >> ID; // string scene
    ss >> scene_IDs[ indScene ];
    ss >> scene_durations[ indScene ];
    //std::cout << "duration : " <<
    //  scene_durations[ indScene ] << " sc #" << indScene << "\n";

    if( scene_durations[ indScene ]  <= 0.0 ) {
      sprintf( ErrorStr , "Error: null scene #%d duration [%f]!" , indScene + 1 , scene_durations[ indScene ]  ); ReportError( ErrorStr ); throw 50;
    }
    if( indScene > 0 ) {
      scene_initial_times[ indScene ] = scene_final_times[ indScene - 1 ];
    }
    else {
      scene_initial_times[ indScene ] = 0.0;
    }
    scene_final_times[ indScene ]
      = scene_initial_times[ indScene ] + scene_durations[ indScene ];

    scene_initial_parameters[ indScene ] = new float[ _MaxInterpVarIDs ];
    scene_final_parameters[ indScene ] = new float[ _MaxInterpVarIDs ];
    for(int indP = 0 ; indP < _MaxInterpVarIDs ; indP++ ) {
      scene_initial_parameters[ indScene ][ indP ] = 0.0;
      scene_final_parameters[ indScene ][ indP ] = 0.0;
    }
    scene_interpolations[ indScene ]
      = new pg_Interpolation[ _MaxInterpVarIDs ];
    for(int indP = 0 ; indP < _MaxInterpVarIDs ; indP++ ) {
      scene_interpolations[ indScene ][ indP ].interpolation_mode
	= pg_linear_interpolation;
      scene_interpolations[ indScene ][ indP ].offSet = 0.0;
      scene_interpolations[ indScene ][ indP ].duration = 1.0;
    }

	// verbatim
	std::getline(scenarioFin, line);
	// std::cout << "verbatim: " << line << "\n";
	// var comment
	std::getline(scenarioFin, line);
	// std::cout << "var comment: " << line << "\n";

    // storing the initial values
    std::getline(scenarioFin, line);
    ss.clear();
    ss.str(line);
    // ss = std::stringstream(line);
	for (int indP = 0; indP < _MaxInterpVarIDs; indP++) {
		//std::size_t found = string("-+.0123456789").find(ss.str().at(0));
		//if (found == std::string::npos) {
		//	sprintf(ErrorStr, "Error: unknown numerical value %s (%c)\n", ss.str().c_str(), ss.str().at(0)); ReportError(ErrorStr); throw 50;
		//}
		ss >> scene_initial_parameters[ indScene ][ indP ];
    }

    // storing the final values
    std::getline(scenarioFin, line);
    ss.clear();
    ss.str(line);
    // ss = std::stringstream(line);
    //std::cout << "line :" << line;
    // std::cout << "\nfinal values :\n";
	for (int indP = 0; indP < _MaxInterpVarIDs; indP++) {
		//std::size_t found = string("-+.0123456789").find(ss.str().at(0));
		//if (found == std::string::npos) {
		//	sprintf(ErrorStr, "Error: unknown numerical value %s (%c)\n", ss.str().c_str(), ss.str().at(0)); ReportError(ErrorStr); throw 50;
		//}
		ss >> scene_final_parameters[indScene][indP];
		// std::cout << scene_final_parameters[indScene][indP] << " ";
	}
    // std::cout << "\n";

    std::getline(scenarioFin, line);
	ss.clear();
    ss.str(line);
	// std::cout << "interpolation: " << line << "\n";
    // ss = std::stringstream(line);

    // storing the interpolation mode
    for(int indP = 0 ; indP < _MaxInterpVarIDs ; indP++ ) {
      char valCh = 0;
      float val, val2;

      scene_interpolations[ indScene ][ indP ].offSet = 0.0;
      scene_interpolations[ indScene ][ indP ].duration = 1.0;

      ss >> std::skipws >> valCh;
	  // printf("valch %d\n" , (int)valCh );

      switch( valCh ) {
      // l: value interpolates linearly between initial and final value from 0.0% to 1.0%
      // L: value is initial from 0.0% until offset,
      // interpolates linearly between initial and final value from offset to offset + duration
      // is final value between offset + duration and 1.0%
      case 'l':
      case 'L':
		scene_interpolations[ indScene ][ indP ].interpolation_mode
		  = pg_linear_interpolation;
		if (valCh == 'L') {
			ss >> val;
			ss >> val2;
			if (val < 0.0 || val2 < 0.0) {
				sprintf(ErrorStr, "Error: one of values of L(inear) interpolationn #%d lower than 0.0: %.3f %.3f\n", indP + 1, val, val2); ReportError(ErrorStr); throw 50;
			}
			if (val <= 1.0) {
				scene_interpolations[indScene][indP].offSet = val;
				if (val + val2 <= 1.00001) {
					// deals with approximate values that can summ above 1.0
					if (val + val2 > 1.0) {
						val2 = 1.0f - val;
					}
					scene_interpolations[indScene][indP].duration = val2;
					if (scene_interpolations[indScene][indP].duration <= 0.0) {
						sprintf(ErrorStr, "Error: null L(inear) interpolation #%d duration [%f]!", indP + 1, scene_interpolations[indScene][indP].duration); ReportError(ErrorStr); throw 50;
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
		scene_interpolations[ indScene ][ indP ].interpolation_mode
		  = pg_cosine_interpolation;
		if (valCh == 'C') {
			ss >> val;
			ss >> val2;
			if (val < 0.0 || val2 < 0.0) {
				sprintf(ErrorStr, "Error: one of values of C(osine) interpolation #%d lower than 0.0: %.3f %.3f\n", indP + 1, val, val2); ReportError(ErrorStr); throw 50;
			}
			if (val <= 1.0) {
				scene_interpolations[indScene][indP].offSet = val;
				// deals with approximate values that can summ above 1.0
				if (val + val2 <= 1.00001) {
					if (val + val2 > 1.0) {
						val2 = 1.0f - val;
					}
					scene_interpolations[indScene][indP].duration = val2;
					if (scene_interpolations[indScene][indP].duration <= 0.0) {
						sprintf(ErrorStr, "Error: null C(osine) interpolation #%d duration [%f]!", indP + 1, scene_interpolations[indScene][indP].duration); ReportError(ErrorStr); throw 50;
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
      case 's':
      case 'S':
		scene_interpolations[ indScene ][ indP ].interpolation_mode
		  = pg_stepwise_interpolation;
		// offset is 1.0 because the interpolation algorithm is such that for a 0.0
		// interpolation value and a first frame in scene, the value is set to
		// the initial value
		scene_interpolations[ indScene ][ indP ].offSet = 1.0;
		scene_interpolations[ indScene ][ indP ].duration = 0.0;
		if (valCh == 'S') {
			ss >> val;
			if (val < 0.0) {
				sprintf(ErrorStr, "Error: offset values of S(tepwise) interpolation #%d lower than 0.0: %.3f\n", indP + 1, val); ReportError(ErrorStr); throw 50;
			}
			// the offset is the time at which value change
			if (val <= 1.0) {
				scene_interpolations[indScene][indP].offSet = val;
			}
			else {
				sprintf(ErrorStr, "Error: offset value of S(tepwise) interpolation #%d greater than 1.0: %.3f\n", indP + 1, val); ReportError(ErrorStr); throw 50;
			}
		}
		break;
      case 'k':
      case 'K':
		scene_interpolations[ indScene ][ indP ].interpolation_mode
		  = pg_keep_value;
		break;
      default:
		  sprintf(ErrorStr, "Error: unknown interpolation mode in scene %d parameter %d!", indScene + 1, indP + 1); ReportError(ErrorStr); throw 50;
		  // sprintf(ErrorStr, "Error: unknown interpolation mode [%c] in scene %d parameter %d!", valCh, indScene + 1, indP + 1); ReportError(ErrorStr); throw 50;
		  break;
      }
    }

	// /scene
	std::getline(scenarioFin, line);

  }


  // /scenario
  std::getline(scenarioFin, line);
  ss.clear();
  ss.str(line);
  // ss = std::stringstream(line);
  ss >> ID; // string /scenario
  if (ID.compare("/scenario") != 0) {
	  sprintf(ErrorStr, "Error: last scene not reached /scenario expected [%s] not found!", ID.c_str()); ReportError(ErrorStr); throw 11;
  }

  // saves the original durations
  saveInitialTimesAndDurations();
}

// to be called once after parsing
void saveInitialTimesAndDurations( void ) {
 for(int indScene = 0 ; indScene < NbTotScenes ; indScene++ ) {
	scene_originalDurations[ indScene ] = scene_durations[ indScene ];
	scene_originalInitial_times[ indScene ] = scene_initial_times[ indScene ];
	scene_originalFinal_times[ indScene ] = scene_final_times[ indScene ];
 }
}
// to be called before setup change
void restoreInitialTimesAndDurations( void ) {
 for(int indScene = 0 ; indScene < NbTotScenes ; indScene++ ) {
	scene_durations[ indScene ] = scene_originalDurations[ indScene ];
	scene_initial_times[ indScene ] = scene_originalInitial_times[ indScene ];
	scene_final_times[ indScene ] = scene_originalFinal_times[ indScene ];
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

  InitializeConfigurationVar();

  //std::cout << "\n*** max_network_message_length/max_mouse_recording_frames/minimal_interframe_latency: "
  //  << max_network_message_length <<  << " " << max_mouse_recording_frames << " " << minimal_interframe_latency "\n";
  //std::cout << "\n*** window size: " << window_width << " " << window_height << " " << window_x << " " << window_y << "\n";

}

//////////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS FOR TESTING SUBDIR FILES (MADE FOR SAMPLES AND IMAGES)

bool is_substring_index(std::string str, int ind) {
	char string_number[16];
	sprintf(string_number, "%03d", ind);
	std::string string_number_s(string_number);

	if (str.find(string_number_s) != std::string::npos) {
		// std::cout << "*** FOUND: " + filepath << std::endl;
		return true;
	}
	return false;
}

bool is_substring_index(char * charstr, int ind) {
	std::string str(charstr);

	char string_number[16];
	sprintf(string_number, "%03d", ind);
	std::string string_number_s(string_number);

	if (str.find(string_number_s) != std::string::npos) {
		// std::cout << "*** FOUND: " + filepath << std::endl;
		return true;
	}
	return false;
}

bool is_subdir_index(struct dirent *dirp, std::string *dirpath, int inddir) {
	string filepath = *dirpath + "/" + dirp->d_name;
	struct stat filestat;

	// If the file is a directory and the name contains the integer
	if (stat(filepath.c_str(), &filestat)) return false; // colleccts file status and returns 0 on success
	if (S_ISDIR(filestat.st_mode) && is_substring_index(dirp->d_name, inddir)) { // the file is a directory
		// and contains the integer substring
		return true;
	}
	return false;
}

bool is_subfile_index(struct dirent *dirp, std::string *dirpath, int indfile) {
	string filepath = *dirpath + "/" + dirp->d_name;
	struct stat filestat;

	// If the file is a directory (or is in some way invalid) we'll skip it
	if (stat(filepath.c_str(), &filestat)) return false; // colleccts file status and returns 0 on success
	if (S_ISDIR(filestat.st_mode)) { // the file is a directory
		// and contains the integer substring
		return false;
	}
	ifstream fin;
	fin.open(filepath.c_str());
	if (fin) {
		if (is_substring_index(dirp->d_name, indfile)) {
			fin.close();
			return true;
		}
		fin.close();
	}
	return false;
}

string * is_subdir_subfile_index(std::string *dirpath, int inddir, int indfile) {
	DIR *dp = opendir(dirpath->c_str());
	if (dp == NULL)
	{
		std::cout << "Error(" << errno << ") opening directory " << *dirpath << std::endl;
		return NULL;
	}
	// std::cout << std::endl << "dir to get files of: " + dirpath << std::endl;
	struct dirent *dirp;
	while ((dirp = readdir(dp))) {
		if (is_subdir_index(dirp, dirpath, inddir)) {
			string subdirpath(*dirpath + "/" + dirp->d_name);
			DIR *subdp = opendir(subdirpath.c_str());
			if (subdp == NULL)
			{
				std::cout << "Error(" << errno << ") opening subdirectory " << subdirpath << std::endl;
				return NULL;
			}
			struct dirent *subdirp;
			while ((subdirp = readdir(subdp))) {
				if (is_subfile_index(subdirp, &subdirpath, indfile)) {
					string * subdirfilepath = new string(subdirpath + "/" + subdirp->d_name);
					// std::cout << "found: (" << inddir << "," << indfile << ") in " << *subdirfilepath << std::endl;
					closedir(subdp);
					closedir(dp);
					return subdirfilepath;
				}
			}
			closedir(subdp);
		}
	}
	closedir(dp);
	return NULL;
}

string * nextFile(string *dirpath, int *currentDirIndex, int *currentFileIndex) {
	string * returnedString;
	// next file in the same dir
	if ((returnedString = is_subdir_subfile_index(dirpath, *currentDirIndex, *currentFileIndex))) {
		(*currentFileIndex)++;
		return returnedString;
	}
	else {
		// first file in the same dir
		(*currentFileIndex) = 0;
		if ((returnedString = is_subdir_subfile_index(dirpath, *currentDirIndex, *currentFileIndex))) {
			(*currentFileIndex)++;
			return returnedString;
		}
		else {
			// first file in the next dir
			(*currentDirIndex)++;
			(*currentFileIndex) = 0;
			if ((returnedString = is_subdir_subfile_index(dirpath, *currentDirIndex, *currentFileIndex))) {
				(*currentFileIndex)++;
				return returnedString;
			}
			else {
				// first file in the first dir
				(*currentDirIndex) = 0;
				(*currentFileIndex) = 0;
				if ((returnedString = is_subdir_subfile_index(dirpath, *currentDirIndex, *currentFileIndex))) {
					(*currentFileIndex)++;
					return returnedString;
				}
				else {
					std::cout << "Error(" << errno << ") opening file in " << *dirpath << std::endl;
					return NULL;
				}
			}

		}
	}
	return NULL;
}


