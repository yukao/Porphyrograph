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

string log_file_name            ;
string font_file_name           ;
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

  font_file_name = "./fonts/usascii/arial/stb_font_arial_15_usascii.png";
  font_size = 15;
  font_file_encoding =  "png" ;
  font_texture_encoding = PNG;
  log_file_name = "./snapshots/pic/log-LYM-"+project_name+"-"+date_stringStream.str()+".log";

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
  nb_scenes = 0;
  // Number of scenes
  std::getline(scenarioFin, line);
  ss.clear();
  ss.str(line);
  // ss = std::stringstream(line);
  ss >> ID; // string scenario
  ss >> nb_scenes;

  scene_IDs = new string[nb_scenes];
  scene_durations = new float[ nb_scenes ];
  scene_initial_times = new float[ nb_scenes ];
  scene_final_times = new float[ nb_scenes ];
  scene_originalDurations = new float[ nb_scenes ];
  scene_originalInitial_times = new float[ nb_scenes ];
  scene_originalFinal_times = new float[ nb_scenes ];
  for(int indScene = 0 ; indScene < nb_scenes ; indScene++ ) {
    scene_durations[ indScene ] = 0.0;
    scene_initial_times[ indScene ] = 0.0;
    scene_final_times[ indScene ] = 0.0;
    scene_originalDurations[ indScene ] = 0.0;
    scene_originalInitial_times[ indScene ] = 0.0;
    scene_originalFinal_times[ indScene ] = 0.0;
  }

  scene_initial_parameters = new float *[ nb_scenes ];
  scene_final_parameters = new float *[ nb_scenes ];
  for(int indScene = 0 ; indScene < nb_scenes ; indScene++ ) {
    scene_initial_parameters[ indScene ] = NULL;
    scene_final_parameters[ indScene ] = NULL;
  }

  scene_interpolations = new pg_Interpolation *[ nb_scenes ];
  for(int indScene = 0 ; indScene < nb_scenes ; indScene++ ) {
    scene_interpolations[ indScene ] = NULL;
  }

  printf("Loading %d scenes with %d variables\n" , nb_scenes , _MaxInterpVarIDs );
  for(int indScene = 0 ; indScene < nb_scenes ; indScene++ ) {
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
      ss >> scene_final_parameters[ indScene ][ indP ];
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
		scene_interpolations[ indScene ][ indP ].offSet = 1.0;
		scene_interpolations[ indScene ][ indP ].duration = 0.0; 
		if (valCh == 'S') {
			ss >> val;
			if (val < 0.0) {
				sprintf(ErrorStr, "Error: offset values of S(tepwise) interpolation #%d lower than 0.0: %.3f\n", indP + 1, val); ReportError(ErrorStr); throw 50;
			}
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
		sprintf( ErrorStr , "Error: unknown  interpolation mode [%c] in scene %d parameter %d!" , valCh , indScene + 1 , indP + 1 ); ReportError( ErrorStr ); throw 50;
		break;
      }
    }

	// /scene
	std::getline(scenarioFin, line);
    
  }


  // /scenario
  std::getline(scenarioFin, line);

  // saves the original durations
  saveInitialTimesAndDurations();
}

// to be called once after parsing
void saveInitialTimesAndDurations( void ) {
 for(int indScene = 0 ; indScene < nb_scenes ; indScene++ ) {
	scene_originalDurations[ indScene ] = scene_durations[ indScene ];
	scene_originalInitial_times[ indScene ] = scene_initial_times[ indScene ];
	scene_originalFinal_times[ indScene ] = scene_final_times[ indScene ];
 }
}
// to be called before setup change
void restoreInitialTimesAndDurations( void ) {
 for(int indScene = 0 ; indScene < nb_scenes ; indScene++ ) {
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

