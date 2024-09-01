////////////////////////////////////////////////////////////////////
// _
// 
// 
//     File pg-callBack.cpp
// 
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2.1
// of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this program; if not, write to the Free
// Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.
////////////////////////////////////////////////////////////////////

#include "pg-all_include.h"

#include "pg_script_body_Core.cpp"

//////////////////////////////////////////////////////////////////////////////////////////////////////
// GLOBAL VARS
//////////////////////////////////////////////////////////////////////////////////////////////////////

float pg_current_extern_movieNo = -1;

// +++++++++++++++++++++++ CLEAR AND INVERT +++++++++++++++++++++++++++++++++
double pg_lastClearTime = 0.;
int pg_lastClearSceneIndex = -1;

#ifdef _WIN32
// +++++++++++++++++++++++ scripts for external control through python programming +++++
// currently running Python script: NULLL if none
string pg_currently_runnings_script_python = "NULL";
STARTUPINFOA pg_si_script_python = { 0 };
PROCESS_INFORMATION pg_pi_script_python;
// currently running binary script: NULLL if none
string pg_currently_runnings_script_binary = "NULL";
STARTUPINFOA pg_si_script_binary = { 0 };
PROCESS_INFORMATION pg_pi_script_binary;
#endif

// +++++++++++++++++++++ CLEAR +++++++++++++++++++++++++++
float pg_isClearCA;
float pg_isClearLayer;
float pg_isClearAllLayers;
float pg_isClearEcho;

//////////////////////////////////////////////////////////////////////////////////////////////////////
// TYPE CONVERSION UTILS
//////////////////////////////////////////////////////////////////////////////////////////////////////  

// callBack variable conversions
bool double_to_bool(double param) {
	return (param != 0);
}
bool pg_double_to_path(double param) {
	return (param > 0);
}
int pg_double_to_sign(double param) {
	return (param > 0 ? 1 : (param < 0 ? -1 : 0));
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
// INTIAL CALLBACK EXECTUTION
//////////////////////////////////////////////////////////////////////////////////////////////////////  

// INITIALIZES ALL SCENARIO VARIABLES CALLBACKS FOR THE CURRENT SCENARIO, 
// NOT FOR THE FULL SCENARIO
void pg_initializationCallBacks(void) {
	for (int indP = 0; indP < pg_ScenarioVarNb[pg_ind_scenario]; indP++) {
		int indVar = pg_ConfigScenarioVarRank[pg_ind_scenario][indP];
		if (pg_FullScenarioActiveVars[pg_ind_scenario][indVar]
			&& pg_FullScenarioVarCallbacks[indVar]) {
			(*pg_FullScenarioVarCallbacks[indVar])(pg_enum_PG_SCENARIO,
				pg_InitialValuesInterpVar[pg_ind_scenario][indVar]);
		}
	}
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
// SETUP CALLBACKS (SCENARIO BASED COMMANDS)
//////////////////////////////////////////////////////////////////////////////////////////////////////  
void auto_beat_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value) {
	pg_lastBeatTime = pg_CurrentClockTime;
	if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
		// sends a message to PD to stop sending music beats
		sprintf(pg_AuxString, "/auto_beat %d", (int)(*((bool*)pg_FullScenarioVarPointers[_auto_beat])));
		pg_send_message_udp((char*)"i", pg_AuxString, (char*)"udp_PD_send");
		// printf("auto_beat [%s] %.2f %d\n", pg_AuxString, scenario_or_gui_command_value, (int)(*((bool *)pg_FullScenarioVarPointers[_auto_beat])));
	}
}
void auto_pulse_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value) {
	if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
		// sends a message to PD to stop sending music pg_audio_pulse
		sprintf(pg_AuxString, "/auto_pulse %d", (int)(*((bool*)pg_FullScenarioVarPointers[_auto_pulse])));
		pg_send_message_udp((char*)"i", pg_AuxString, (char*)"udp_PD_send");
		// printf("auto_pulse [%s] %.2f %d\n", pg_AuxString, scenario_or_gui_command_value, (int)(*((bool *)pg_FullScenarioVarPointers[_auto_pulse])));
		for (int ind = 0; ind < 3 * 2; ind++) {
			pg_seed_pulsePerlinNoise[ind] = rand_0_1 * 255;
		}
	}
}
void clearAllLayers_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value) {
	if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_KEYSTROKE) {
		//printf("delta %.3f\n", (float)(pg_CurrentClockTime - pg_lastClearTime) );
		if (pg_CurrentClockTime - pg_lastClearTime < 0.5f) {
			// pg_send_message_udp((char *)"s", (char *)"/message clear_all", (char *)"udp_TouchOSC_send");
			pg_isClearAllLayers = 1.f;
			pg_lastClearTime = 0.0f;
			//printf("****** pg_enum_PG_GUI_COMMAND pg_isClearAllLayers %d\n",pg_isClearAllLayers);
		}
		else {
			pg_lastClearTime = pg_CurrentClockTime;
		}
	}
	// clearing is only made once at the beginning of a scene
	else if (param_input_type == pg_enum_PG_SCENARIO && scenario_or_gui_command_value && pg_lastClearSceneIndex != pg_CurrentSceneIndex) {
		// pg_send_message_udp((char *)"s", (char *)"/message clear_all", (char *)"udp_TouchOSC_send");
		pg_isClearAllLayers = 1.f;
		pg_lastClearTime = pg_CurrentClockTime;
		pg_lastClearSceneIndex = pg_CurrentSceneIndex;
		//printf("****** pg_enum_PG_SCENARIO pg_isClearAllLayers %d %d %d\n", pg_isClearAllLayers, pg_FirstFrameInScene, pg_CurrentSceneIndex);
	}
}
void clearLayer_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value) {
	if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_KEYSTROKE) {
		if (pg_CurrentClockTime - pg_lastClearTime < 0.2f) {
			pg_isClearLayer = 1.f;
			pg_lastClearTime = 0.0f;
			// pg_send_message_udp((char *)"s", (char *)"/message clear_all_layers", (char *)"udp_TouchOSC_send");
		}
		else {
			pg_isClearLayer = 1.f;
			pg_lastClearTime = pg_CurrentClockTime;
			// pg_send_message_udp((char *)"s", (char *)"/message clear_layer", (char *)"udp_TouchOSC_send");
		}
	}
	else if (param_input_type == pg_enum_PG_SCENARIO && scenario_or_gui_command_value && pg_lastClearSceneIndex != pg_CurrentSceneIndex) {
		pg_isClearLayer = 1.f;
		pg_lastClearTime = pg_CurrentClockTime;
		pg_lastClearSceneIndex = pg_CurrentSceneIndex;
	}
}
void clearCA_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value) {
	if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_KEYSTROKE) {
		// pg_send_message_udp((char *)"s", (char *)"/message clear_CA", (char *)"udp_TouchOSC_send");
		pg_isClearCA = 1.f;
	}
	else if (param_input_type == pg_enum_PG_SCENARIO && scenario_or_gui_command_value && pg_lastClearSceneIndex != pg_CurrentSceneIndex) {
		pg_isClearCA = 1.f;
		pg_lastClearSceneIndex = pg_CurrentSceneIndex;
	}
}
void clearEcho_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value) {
	if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_KEYSTROKE) {
		// pg_send_message_udp((char *)"s", (char *)"/message clear_CA", (char *)"udp_TouchOSC_send");
		pg_isClearEcho = 1.f;
	}
	else if (param_input_type == pg_enum_PG_SCENARIO && scenario_or_gui_command_value) {
		pg_isClearEcho = 1.f;
	}
}
void script_python_callBack(pg_Parameter_Input_Type param_input_type, string scenario_or_gui_command_value) {
#ifdef _WIN32
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_script_python]) {
		if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
			// stops the current Python script if one is active
			if (pg_currently_runnings_script_python != "NULL") {
				if (pg_pi_script_python.hProcess != NULL) {
					TerminateProcess(pg_pi_script_python.hProcess, 0);
				}

				std::cout << "quit Python script" << pg_currently_runnings_script_python << std::endl;
				pg_currently_runnings_script_python = "NULL";
			}
			// launches a new Python script if one is given by the scenario
			if (scenario_or_gui_command_value != "NULL") {
				// launches the script
				pg_currently_runnings_script_python = scenario_or_gui_command_value;
				string  full_script_name = pg_scripts_directory + pg_currently_runnings_script_python;
				unsigned int serverPort = 7000;
				for (pg_IPClient& client : pg_IP_Clients) {
					if (strcmp("udp_script_python_send", client.id.c_str()) == 0) {
						serverPort = client.Remote_server_port;
						break;
					}
				}
				unsigned int clientPort = 9000;
				for (pg_IPServer& server : pg_IP_Servers) {
					if (strcmp("udp_TouchOSC_receive", server.id.c_str()) == 0) {
						clientPort = server.Local_server_port;
						break;
					}
				}

				sprintf(pg_errorStr, "python.exe %s -p %d -s %d", full_script_name.c_str(), clientPort, serverPort);
				std::cout << "run script " << string(pg_errorStr) << std::endl;
				pg_si_script_python.cb = sizeof(STARTUPINFO);
				pg_si_script_python.dwFlags = STARTF_USESHOWWINDOW;
				pg_si_script_python.wShowWindow = FALSE;
				if (pg_pi_script_python.hProcess != NULL) {
					TerminateProcess(pg_pi_script_python.hProcess, 0);
				}
				if (!CreateProcessA(NULL,   // No module name (use command line)
					pg_errorStr,        // Command line
					NULL,           // Process handle not inheritable
					NULL,           // Thread handle not inheritable
					FALSE,          // Set handle inheritance to FALSE
					0,              // No creation flags
					NULL,           // Use parent's environment block
					NULL,           // Use parent's starting directory 
					&pg_si_script_python,            // Pointer to STARTUPINFO structure
					&pg_pi_script_python)           // Pointer to PROCESS_INFORMATION structure
					)
				{
					printf("CreateProcess failed (%d).\n", GetLastError());
					return;
				}
				//system(pg_errorStr);
			}
		}
	}
#endif
}
void script_binary_callBack(pg_Parameter_Input_Type param_input_type, string scenario_or_gui_command_value) {
#ifdef _WIN32
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_script_binary]) {
		if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
			// stops the current binary script if one is active
			if (pg_currently_runnings_script_binary != "NULL") {
				if (pg_pi_script_binary.hProcess != NULL) {
					TerminateProcess(pg_pi_script_binary.hProcess, 0);
				}

				std::cout << "quit binary script" << pg_currently_runnings_script_binary  << std::endl;
				pg_currently_runnings_script_binary = "NULL";
			}
			// launches a new binary script if one is given by the scenario
			if (scenario_or_gui_command_value != "NULL") {
				// launches the script #1
				pg_currently_runnings_script_binary = scenario_or_gui_command_value;
				string  full_script_name = pg_scripts_directory + pg_currently_runnings_script_binary;
				unsigned int serverPort = 7000;
				for (pg_IPClient& client : pg_IP_Clients) {
					if (strcmp("udp_script_binary_send", client.id.c_str()) == 0) {
						serverPort = client.Remote_server_port;
						break;
					}
				}
				unsigned int clientPort = 9000;
				for (pg_IPServer& server : pg_IP_Servers) {
					if (strcmp("udp_script_binary_receive", server.id.c_str()) == 0) {
						clientPort = server.Local_server_port;
						break;
					}
				}

				sprintf(pg_errorStr, "%s %d %d", full_script_name.c_str(), clientPort, serverPort);
				std::cout << "run script " << string(pg_errorStr) << std::endl;
				pg_si_script_binary.cb = sizeof(STARTUPINFO);
				pg_si_script_binary.dwFlags = STARTF_USESHOWWINDOW;
				pg_si_script_binary.wShowWindow = FALSE;
				if (pg_pi_script_binary.hProcess != NULL) {
					TerminateProcess(pg_pi_script_binary.hProcess, 0);
				}
				if (!CreateProcessA(NULL,   // No module name (use command line)
					pg_errorStr,        // Command line
					NULL,           // Process handle not inheritable
					NULL,           // Thread handle not inheritable
					FALSE,          // Set handle inheritance to FALSE
					0,              // No creation flags
					NULL,           // Use parent's environment block
					NULL,           // Use parent's starting directory 
					&pg_si_script_binary,            // Pointer to STARTUPINFO structure
					&pg_pi_script_binary)           // Pointer to PROCESS_INFORMATION structure
					)
				{
					printf("CreateProcess failed (%d).\n", GetLastError());
					return;
				}
				//system(pg_errorStr);
			}
		}
	}
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// PEN CALLBACKS (SCENARIO BASED COMMANDS)
//////////////////////////////////////////////////////////////////////////////////////////////////////
void pen_color_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
		sprintf(pg_AuxString, "/pen_colorPreset -1.0");
		pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_TouchOSC_send");
	}
}

void pen_hue_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
		float control_color[3];
		pg_HSVtoRGB(pen_hue, pen_sat, pen_value, &control_color[0], &control_color[1], &control_color[2]);
		sprintf(pg_AuxString, "/pen_color/color %02x%02x%02xFF", int(control_color[0] * 255), int(control_color[1] * 255), int(control_color[2] * 255)); pg_send_message_udp((char*)"s", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
		//printf("colorH msg HSV %.2f %.2f %.2f  %s\n", pen_hue, pen_sat, pen_value, pg_AuxString);
	}
}
void pen_sat_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
		float control_color[3];
		pg_HSVtoRGB(pen_hue, pen_sat, pen_value, &control_color[0], &control_color[1], &control_color[2]);
		sprintf(pg_AuxString, "/pen_color/color %02x%02x%02xFF", int(control_color[0] * 255), int(control_color[1] * 255), int(control_color[2] * 255)); pg_send_message_udp((char*)"s", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
		//printf("colorS msg HSV %.2f %.2f %.2f  %s\n", pen_hue, pen_sat, pen_value, pg_AuxString);
	}
}
void pen_value_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
		float control_color[3];
		pg_HSVtoRGB(pen_hue, pen_sat, pen_value, &control_color[0], &control_color[1], &control_color[2]);
		sprintf(pg_AuxString, "/pen_color/color %02x%02x%02xFF", int(control_color[0] * 255), int(control_color[1] * 255), int(control_color[2] * 255)); pg_send_message_udp((char*)"s", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
		//printf("colorV msg HSV %.2f %.2f %.2f  %s\n", pen_hue, pen_sat, pen_value, pg_AuxString);
	}
}
void pen_brush_callBack(pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value) {
	if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_KEYSTROKE || param_input_type == pg_enum_PG_SCENARIO) {
		//if (scenario_or_gui_command_value >= 1) { // large radius for the image brushes*
		//	pen_radiusMultiplier = 50.0f;
		//}
		//else {
		//	pen_radiusMultiplier = 1.0f;
		//}
	}
}
void fingers_callBack(pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value) {
	if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
		// drawing type initialization
		NumberOfInteractionFingers(scenario_or_gui_command_value);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// REPOP CALLBACKS (SCENARIO BASED COMMANDS)
//////////////////////////////////////////////////////////////////////////////////////////////////////  
 
void repop_huePart_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_repop_huePart]
			&& pg_FullScenarioActiveVars[pg_ind_scenario][_repop_satPart]
			&& pg_FullScenarioActiveVars[pg_ind_scenario][_repop_valuePart]
			&& repop_hsvPart) {
			float control_color[3];
			pg_HSVtoRGB(repop_huePart, repop_satPart, repop_valuePart, &control_color[0], &control_color[1], &control_color[2]);
			sprintf(pg_AuxString, "/part_color/color %02x%02x%02xFF", int(control_color[0] * 255), int(control_color[1] * 255), int(control_color[2] * 255)); pg_send_message_udp((char*)"s", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
		}
		//printf("colorH msg HSV %.2f %.2f %.2f  %s\n", repop_huePart, repop_satPart, repop_valuePart, pg_AuxString);
	}
}
void repop_satPart_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_repop_huePart]
			&& pg_FullScenarioActiveVars[pg_ind_scenario][_repop_satPart]
			&& pg_FullScenarioActiveVars[pg_ind_scenario][_repop_valuePart]
			&& repop_hsvPart) {
			float control_color[3];
			pg_HSVtoRGB(repop_huePart, repop_satPart, repop_valuePart, &control_color[0], &control_color[1], &control_color[2]);
			sprintf(pg_AuxString, "/part_color/color %02x%02x%02xFF", int(control_color[0] * 255), int(control_color[1] * 255), int(control_color[2] * 255)); pg_send_message_udp((char*)"s", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
		}
		//printf("colorS msg HSV %.2f %.2f %.2f  %s\n", repop_huePart, repop_satPart, repop_valuePart, pg_AuxString);
	}
}
void repop_valuePart_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
		float control_color[3];
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_repop_huePart]
			&& pg_FullScenarioActiveVars[pg_ind_scenario][_repop_satPart]
			&& pg_FullScenarioActiveVars[pg_ind_scenario][_repop_valuePart]
			&& repop_hsvPart) {
			pg_HSVtoRGB(repop_huePart, repop_satPart, repop_valuePart, &control_color[0], &control_color[1], &control_color[2]);
			sprintf(pg_AuxString, "/part_color/color %02x%02x%02xFF", int(control_color[0] * 255), int(control_color[1] * 255), int(control_color[2] * 255)); pg_send_message_udp((char*)"s", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
			//printf("colorV msg HSV %.2f %.2f %.2f  %s\n", repop_huePart, repop_satPart, repop_valuePart, pg_AuxString);
		}
	}
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
// FLASH CALLBACKS (SCENARIO BASED COMMANDS)
//////////////////////////////////////////////////////////////////////////////////////////////////////

void MIDIwithStroke_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value) {
	if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_MIDIwithStroke]) {
			// sends a message to PD to stop sending music beats
			sprintf(pg_AuxString, "/MIDIwithStroke %d", (int)(*((bool*)pg_FullScenarioVarPointers[_MIDIwithStroke])));
			pg_send_message_udp((char*)"i", pg_AuxString, (char*)"udp_PD_send");
			// printf("MIDIwithStroke[%s] %.2f %d\n", pg_AuxString, scenario_or_gui_command_value, (int)(*((bool *)pg_FullScenarioVarPointers[_MIDIwithBeat])));
		}
	}
}
void MIDIwithBeat_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value) {
	if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_MIDIwithBeat]) {
			// sends a message to PD to stop sending music beats
			sprintf(pg_AuxString, "/MIDIwithBeat %d", (int)(*((bool*)pg_FullScenarioVarPointers[_MIDIwithBeat])));
			pg_send_message_udp((char*)"i", pg_AuxString, (char*)"udp_PD_send");
			// printf("MIDIwithBeat [%s] %.2f %d\n", pg_AuxString, scenario_or_gui_command_value, (int)(*((bool *)pg_FullScenarioVarPointers[_MIDIwithBeat])));
		}
	}
}
void MIDIwithColor_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value) {
	if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_MIDIwithColor]) {
			// sends a message to PD to stop sending music Colors
			sprintf(pg_AuxString, "/MIDIwithColor %d", (int)(*((bool*)pg_FullScenarioVarPointers[_MIDIwithColor])));
			pg_send_message_udp((char*)"i", pg_AuxString, (char*)"udp_PD_send");
			// printf("MIDIwithColor [%s] %.2f %d\n", pg_AuxString, scenario_or_gui_command_value, (int)(*((bool *)pg_FullScenarioVarPointers[_MIDIwithColor])));
		}
	}
}
void MIDIwithBrush_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value) {
	if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_MIDIwithBrush]) {
			// sends a message to PD to stop sending music Brushs
			sprintf(pg_AuxString, "/MIDIwithBrush %d", (int)(*((bool*)pg_FullScenarioVarPointers[_MIDIwithBrush])));
			pg_send_message_udp((char*)"i", pg_AuxString, (char*)"udp_PD_send");
			// printf("MIDIwithBrush [%s] %.2f %d\n", pg_AuxString, scenario_or_gui_command_value, (int)(*((bool *)pg_FullScenarioVarPointers[_MIDIwithBrush])));
		}
	}
}
void MIDIwithCameraFlash_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value) {
	if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_cameraCaptFreq]) {
			// sends a message to PD to stop sending music Flashs
			sprintf(pg_AuxString, "/MIDIwithCameraFlash %d", (int)(*((bool*)pg_FullScenarioVarPointers[_MIDIwithCameraFlash])));
			pg_send_message_udp((char*)"i", pg_AuxString, (char*)"udp_PD_send");
			// printf("MIDIwithFlash [%s] %.2f %d\n", pg_AuxString, scenario_or_gui_command_value, (int)(*((bool *)pg_FullScenarioVarPointers[_MIDIwithFlash])));
		}
	}
}
void MIDIwithPhotoFlash_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value) {
	if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_MIDIwithPhotoFlash]) {
			// sends a message to PD to stop sending music Flashs
			sprintf(pg_AuxString, "/MIDIwithPhotoFlash %d", (int)(*((bool*)pg_FullScenarioVarPointers[_MIDIwithPhotoFlash])));
			pg_send_message_udp((char*)"i", pg_AuxString, (char*)"udp_PD_send");
			// printf("MIDIwithFlash [%s] %.2f %d\n", pg_AuxString, scenario_or_gui_command_value, (int)(*((bool *)pg_FullScenarioVarPointers[_MIDIwithFlash])));
		}
	}
}
void beat_delay_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
		sprintf(pg_AuxString, "/beat_delay %.5f", scenario_or_gui_command_value);
		pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_PD_send");
	}
}
void beat_threshold_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
		sprintf(pg_AuxString, "/beat_threshold %.5f", scenario_or_gui_command_value);
		pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_PD_send");
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// PARTICLE CALLBACKS (SCENARIO BASED COMMANDS)
//////////////////////////////////////////////////////////////////////////////////////////////////////

void part_move_init(void) {
	for (int pathNo = 0; pathNo < PG_NB_PATHS; pathNo++) {
		((bool*)pg_FullScenarioVarPointers[_part_path_follow])[pathNo] = false;
		((bool*)pg_FullScenarioVarPointers[_part_path_repulse])[pathNo] = false;
	}
}
void part_path_follow_callBack(int pathNo, pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value) {
	bool state = false;
	state = part_path_follow[pathNo];
	if (!state) {
		return; // nothing to do for false values (does not follow)
	}
	part_move_init();
	((bool*)pg_FullScenarioVarPointers[_part_path_follow])[pathNo] = state;
}
void part_path_repulse_callBack(int pathNo, pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value) {
	bool state = false;
	state = part_path_repulse[pathNo];
	if (!state) {
		return; // nothing to do for false values (does not reoulse)
	}
	part_move_init();
	((bool*)pg_FullScenarioVarPointers[_part_path_repulse])[pathNo] = state;
}
void partMove_target_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value) {
	if (partMove_target) {
		partMove_rand = false;
	}
	//printf("Particle move target %d\n", int(partMove_target));
	//printf("Particle move rand %d\n", int(partMove_rand));
}
void partMove_rand_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value) {
	if (partMove_rand) {
		partMove_target = false;
	}
	//printf("Particle move target %d\n", int(partMove_target));
	//printf("Particle move rand %d\n", int(partMove_rand));
}
void part_initialization_callBack(pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value) {
	pg_ParticleTargetFrameNo = pg_FrameNo + int(part_timeToTargt);
	//printf("part initialization call back %d %d\n", part_initialization, scenario_or_gui_command_value);
}


