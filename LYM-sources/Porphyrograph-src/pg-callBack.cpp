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

float pg_current_extern_movieNo = -1;

// +++++++++++++++++++++++ CLEAR AND INVERT +++++++++++++++++++++++++++++++++
double pg_lastClearTime = 0.;
int pg_lastClearSceneIndex = -1;

#ifdef _WIN32
// +++++++++++++++++++++++ scripts for external control through python programming +++++
// currently running script: NULLL if none
string pg_currently_runnings_script_1 = "NULL";
STARTUPINFOA pg_si_script_1 = { 0 };
PROCESS_INFORMATION pg_pi_script_1;
#endif

// +++++++++++++++++++++ CLEAR +++++++++++++++++++++++++++
float pg_isClearCA;
float pg_isClearLayer;
float pg_isClearAllLayers;
float pg_isClearEcho;

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


/////////////////////////////////////////////////////////////////////////
// INITIALIZES ALL SCENARIO VARIABLES CALLBACKS
void pg_initializationCallBacks(void) {
	for (int indVar = 0; indVar < _MaxInterpVarIDs; indVar++) {
		if (pg_FullScenarioVarCallbacks[indVar] != NULL) {
			(*pg_FullScenarioVarCallbacks[indVar])(pg_enum_PG_SCENARIO, pg_InitialValuesInterpVar[pg_ind_scenario][indVar]);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////
// SCENARIO BASED COMMANDS
///////////////////////////////////////////////////////////////////////////////////
void auto_beat_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value) {
	pg_lastBeatTime = pg_CurrentClockTime;
	if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
		// sends a message to PD to stop sending music beats
		sprintf(pg_AuxString, "/auto_beat %d", (int)(*((bool*)pg_FullScenarioVarPointers[_auto_beat])));
		pg_send_message_udp((char*)"i", pg_AuxString, (char*)"udp_PD_send");
		// printf("auto_beat [%s] %.2f %d\n", pg_AuxString, scenario_or_gui_command_value, (int)(*((bool *)pg_FullScenarioVarPointers[_auto_beat])));
	}
}
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
#if defined(var_Contact_clip_in_range)
void Contact_clip_in_range_callBack(pg_Parameter_Input_Type param_input_type, string scenario_or_gui_command_value) {
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_clipCaptFreq]) {
		if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
			ContAct_clip_ranges_min.clear();
			ContAct_clip_ranges_max.clear();
			if (scenario_or_gui_command_value != "NULL") {
				std::vector<std::string> ContAct_clip_ranges;
				ContAct_clip_ranges = pg_split_string(scenario_or_gui_command_value, '/');
				for (string& range : ContAct_clip_ranges) {
					std::vector<std::string> clip_min_max;
					clip_min_max = pg_split_string(range, '-');
					ContAct_clip_ranges_min.push_back(stoi(clip_min_max[0]));
					ContAct_clip_ranges_max.push_back(stoi(clip_min_max[1]));
				}
			}
		}
	}
}
#endif
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
void pen_color_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
		sprintf(pg_AuxString, "/pen_colorPreset -1.0");
		pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_TouchOSC_send");
	}
}

void audioInput_weight_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
		audioInput_weight = scenario_or_gui_command_value;
		sprintf(pg_AuxString, "/audioInput_weight %.2f", audioInput_weight);
		pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_PD_send");
#if defined(PG_WITH_JUCE)
		sprintf(pg_AuxString, "/JUCE_audioInput_weight %.2f", audioInput_weight);
		pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_SoundJUCE_send");
#endif
	}
}

void soundtrack_PD_weight_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
		soundtrack_PD_weight = scenario_or_gui_command_value;
		sprintf(pg_AuxString, "/soundtrack_weight %.2f", soundtrack_PD_weight);
		pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_PD_send");

#if defined(PG_WITH_JUCE)
		sprintf(pg_AuxString, "/JUCE_soundtrack_weight %.2f", soundtrack_PD_weight);
		pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_SoundJUCE_send");
#endif
	}
}

void soundtrack_PA_weight_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
		soundtrack_PA_weight = scenario_or_gui_command_value;
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
void fingers_callBack(pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value) {
	if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
		// drawing type initialization
		NumberOfInteractionFingers(scenario_or_gui_command_value);
	}
}
void mesh_anime_speed_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
		for (int indMesh = 0; indMesh < int(pg_Meshes[pg_ind_scenario].size()); indMesh++) {
			pg_mesh_startAnime[pg_ind_scenario][indMesh] = pg_CurrentClockTime;
		}
	}
}
void mesh_motion_speed_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
		for (int indMesh = 0; indMesh < int(pg_Meshes[pg_ind_scenario].size()); indMesh++) {
			pg_mesh_startMotion[pg_ind_scenario][indMesh] = pg_CurrentClockTime;
		}
	}
}

void cameraExposure_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_cameraCaptFreq]) {
		if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
			// sprintf(pg_AuxString, "/message Exposure_%.1f", scenario_or_gui_command_value); pg_send_message_udp((char *)"s", (char *)pg_AuxString, (char *)"udp_TouchOSC_send");
			if (scenario_or_gui_command_value != pg_CameraCurrent_exposure) {
				//printf("Cam exposure %.2f\n", scenario_or_gui_command_value);
				// cvSetCaptureProperty comment for see3cam
				//pg_webCam_capture.set(CAP_PROP_EXPOSURE, scenario_or_gui_command_value);
				pg_CameraCurrent_exposure = scenario_or_gui_command_value;
				pg_LastCameraParameterChange_Frame = pg_FrameNo;
			}
		}
	}
}

#if defined(var_Caverne_Mesh_Profusion)
void Caverne_Mesh_Profusion_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
		//printf("/Caverne_Mesh_Profusion %.1f\n", scenario_or_gui_command_value);
		if (scenario_or_gui_command_value == 1.) {
			for (int indMesh = 7; indMesh < pg_Meshes[pg_ind_scenario].size(); indMesh++) {
				Caverne_Mesh_Profusion_On(indMesh);
			}
		}
		else if (scenario_or_gui_command_value == 0.) {
			for (int indMesh = 7; indMesh < pg_Meshes[pg_ind_scenario].size(); indMesh++) {
				Caverne_Mesh_Profusion_Off(indMesh);
			}
		}
	}
}
#endif
void cameraGamma_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_cameraCaptFreq]) {
		if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
			// sprintf(pg_AuxString, "/message Gamma%.1f", scenario_or_gui_command_value); pg_send_message_udp((char *)"s", (char *)pg_AuxString, (char *)"udp_TouchOSC_send");
			if (scenario_or_gui_command_value != pg_CameraCurrent_gamma) {
				//printf("Cam gamma %.2f\n", scenario_or_gui_command_value);
				// cvSetCaptureProperty comment for see3cam
				pg_webCam_capture.set(CAP_PROP_GAMMA, scenario_or_gui_command_value);
				pg_CameraCurrent_gamma = scenario_or_gui_command_value;
				pg_LastCameraParameterChange_Frame = pg_FrameNo;
			}
		}
	}
}
void cameraGain_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_cameraCaptFreq]) {
		if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
			if (scenario_or_gui_command_value != pg_CameraCurrent_gain) {
				printf("Cam gain %.2f\n", scenario_or_gui_command_value);
				// cvSetCaptureProperty comment for see3cam 
				pg_webCam_capture.set(CAP_PROP_GAIN, scenario_or_gui_command_value);
				pg_CameraCurrent_gain = scenario_or_gui_command_value;
				pg_LastCameraParameterChange_Frame = pg_FrameNo;
			}
		}
	}
}
void cameraBrightness_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_cameraCaptFreq]) {
		if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
			if (scenario_or_gui_command_value != pg_CameraCurrent_brightness) {
				//printf("Cam brightness %.2f\n", scenario_or_gui_command_value);
				// cvSetCaptureProperty comment for see3cam
				pg_webCam_capture.set(CAP_PROP_BRIGHTNESS, scenario_or_gui_command_value);
				pg_CameraCurrent_brightness = scenario_or_gui_command_value;
				pg_LastCameraParameterChange_Frame = pg_FrameNo;
			}
		}
	}
}
void cameraSaturation_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_cameraCaptFreq]) {
		if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
			if (scenario_or_gui_command_value != pg_CameraCurrent_saturation) {
				//printf("Cam saturation %.2f\n", scenario_or_gui_command_value);
				// cvSetCaptureProperty comment for see3cam 
				pg_webCam_capture.set(CAP_PROP_SATURATION, scenario_or_gui_command_value);
				pg_CameraCurrent_saturation = scenario_or_gui_command_value;
				pg_LastCameraParameterChange_Frame = pg_FrameNo;
			}
		}
	}
}
void cameraContrast_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_cameraCaptFreq]) {
		if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
			if (scenario_or_gui_command_value != pg_CameraCurrent_contrast) {
				//printf("Cam contrast %.2f\n", scenario_or_gui_command_value);
				// cvSetCaptureProperty comment for see3cam
				pg_webCam_capture.set(CAP_PROP_CONTRAST, scenario_or_gui_command_value);
				pg_CameraCurrent_contrast = scenario_or_gui_command_value;
				pg_LastCameraParameterChange_Frame = pg_FrameNo;
			}
		}
	}
}
void cameraWB_B_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_cameraCaptFreq]) {
		if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
			if (scenario_or_gui_command_value != pg_CameraCurrent_WB_B) {
				//printf("Cam WB B %.2f\n", scenario_or_gui_command_value);
				// cvSetCaptureProperty comment for see3cam
				pg_webCam_capture.set(CAP_PROP_WHITE_BALANCE_BLUE_U, scenario_or_gui_command_value);
				pg_CameraCurrent_WB_B = scenario_or_gui_command_value;
				pg_LastCameraParameterChange_Frame = pg_FrameNo;
			}
		}
	}
}
void cameraWB_R_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_cameraCaptFreq]) {
		if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
			if (scenario_or_gui_command_value != pg_CameraCurrent_WB_R) {
				//printf("Cam WB R %.2f\n", scenario_or_gui_command_value);
				// cvSetCaptureProperty comment for see3cam
				if (scenario_or_gui_command_value >= 0) {
					pg_webCam_capture.set(CAP_PROP_WHITE_BALANCE_RED_V, scenario_or_gui_command_value);
					pg_CameraCurrent_WB_R = scenario_or_gui_command_value;
					pg_LastCameraParameterChange_Frame = pg_FrameNo;
				}
			}
		}
	}
}
void playing_soundtrackNo_callBack(pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value) {
	if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
		// play a new track
		if (int(scenario_or_gui_command_value) >= 0
			&& int(scenario_or_gui_command_value) < int(pg_SoundTracks[pg_ind_scenario].size())
			&& pg_currentlyPlaying_trackNo != int(scenario_or_gui_command_value)) {
			PlayTrack(int(scenario_or_gui_command_value), 0.);
		}
		// stop playing a track
		else if (int(scenario_or_gui_command_value) < 0) {
			//printf("playing_soundtrackNo_callBack: playing_soundtrackNo neg %d\n", int(scenario_or_gui_command_value));
			StopTrack();
		}
	}
}

void playing_movieNo_callBack(pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value) {
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_movieCaptFreq]) {
		if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
			// starts playing another video
			pg_play_movie_no();

			// if the video no is outside the range of available videos: stops the playing video
			if (playing_movieNo != pg_currentlyPlaying_movieNo
				&& (playing_movieNo < 0 || playing_movieNo >= int(pg_VideoTracks[pg_ind_scenario].size()))) {
				printf("video no is outside the range of available videos: stops the playing video \n");

				pg_currentlyPlaying_movieNo = -1;
				movieWeight = 0.f;
				pg_movie_frame.setTo(Scalar(0, 0, 0));
				sprintf(pg_AuxString, "/movie_shortName %s", "---");
				pg_send_message_udp((char*)"s", pg_AuxString, (char*)"udp_TouchOSC_send");
			}
		}
	}
}
void movieCaptFreq_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_movieCaptFreq]) {
		if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
			//printf("Movie No %d\n", playing_movieNo);
			// if the video no is outside the range of available videos: stops the playing video
			if (playing_movieNo >= 0 && playing_movieNo < int(pg_VideoTracks[pg_ind_scenario].size())) {
				pg_movie_status.reset_initialTime(scenario_or_gui_command_value);
			}
		}
	}
}
void cameraNo_callBack(pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value) {
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_cameraCaptFreq]) {
		if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
			//printf("camera current %d new %d param type %d\n", pg_current_active_cameraNo, cameraNo, param_input_type);
			if (cameraNo != pg_current_active_cameraNo) {
				pg_releaseCameraCapture();
				pg_openCameraCaptureAndLoadFrame();
			}
		}
	}
}

void script_1_callBack(pg_Parameter_Input_Type param_input_type, string scenario_or_gui_command_value) {
#ifdef _WIN32
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_script_1]) {
		if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
			// stops the current script #1 if one is active
			if (pg_currently_runnings_script_1 != "NULL") {
				if (pg_pi_script_1.hProcess != NULL) {
					TerminateProcess(pg_pi_script_1.hProcess, 0);
				}

				std::cout << "quit script" << std::endl;
				pg_currently_runnings_script_1 = "NULL";
			}
			// launches a new current script #1 if one is given by the scenario
			if (scenario_or_gui_command_value != "NULL") {
				// launches the script #1
				pg_currently_runnings_script_1 = scenario_or_gui_command_value;
				string  full_script_name = pg_scripts_directory + pg_currently_runnings_script_1;
				unsigned int serverPort = 7000;
				for (pg_IPClient& client : pg_IP_Clients) {
					if (strcmp("udp_script_1_send", client.id.c_str()) == 0) {
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
				pg_si_script_1.cb = sizeof(STARTUPINFO);
				pg_si_script_1.dwFlags = STARTF_USESHOWWINDOW;
				pg_si_script_1.wShowWindow = FALSE;
				if (pg_pi_script_1.hProcess != NULL) {
					TerminateProcess(pg_pi_script_1.hProcess, 0);
				}
				if (!CreateProcessA(NULL,   // No module name (use command line)
					pg_errorStr,        // Command line
					NULL,           // Process handle not inheritable
					NULL,           // Thread handle not inheritable
					FALSE,          // Set handle inheritance to FALSE
					0,              // No creation flags
					NULL,           // Use parent's environment block
					NULL,           // Use parent's starting directory 
					&pg_si_script_1,            // Pointer to STARTUPINFO structure
					&pg_pi_script_1)           // Pointer to PROCESS_INFORMATION structure
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

void playing_clipNameLeft_callBack(pg_Parameter_Input_Type param_input_type, string scenario_or_gui_command_value) {
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_clipCaptFreq]) {
		if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
			if (scenario_or_gui_command_value != "NULL") {
				for (int ind_clip = 0; ind_clip < pg_nbClips[pg_ind_scenario]; ind_clip++) {
					//printf("clip ID (%s)\n", pg_clip_tracks[pg_ind_scenario][ind_clip].get_name().c_str());
					if (pg_clip_tracks[pg_ind_scenario][ind_clip].get_name() == scenario_or_gui_command_value) {
						// starts playing another clip
						//printf("Start playing clip \n");
						pg_playing_clipNoLeft = ind_clip;
						printf("playing left clip name %s\n", scenario_or_gui_command_value.c_str());
						pg_play_clip_no(0, pg_enum_clipLeft, pg_playing_clipNoLeft);
						return;
					}
				}
				printf("Left cLip not found [%s]\n", scenario_or_gui_command_value.c_str());
			}
			else {
				//printf("Stop playing left cLip [NULL]\n");
			}
			pg_playing_clipNoLeft = -1;
			pg_all_clip_status[pg_enum_clipLeft].setCurrentlyPlaying_clipNo(0, pg_playing_clipNoLeft);
			sprintf(pg_AuxString, "/clip_shortName_0 %s", "---");
			pg_send_message_udp((char*)"s", pg_AuxString, (char*)"udp_TouchOSC_send");
		}
	}
}
void playing_clipNameRight_callBack(pg_Parameter_Input_Type param_input_type, string scenario_or_gui_command_value) {
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_clipCaptFreq]) {
		if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
			if (scenario_or_gui_command_value != "NULL") {
				for (int ind_clip = 0; ind_clip < pg_nbClips[pg_ind_scenario]; ind_clip++) {
					if (pg_clip_tracks[pg_ind_scenario][ind_clip].get_name() == scenario_or_gui_command_value) {
						// starts playing another clip
						//printf("Start playing clip \n");
						pg_playing_clipNoRight = ind_clip;
						printf("playing right clip name %s\n", scenario_or_gui_command_value.c_str());
						pg_play_clip_no(0, pg_enum_clipRight, pg_playing_clipNoRight);
						return;
					}
				}
				printf("Right cLip not found [%s]\n", scenario_or_gui_command_value.c_str());
			}
			else {
				//printf("Stop playing right cLip [NULL]\n");
			}
			pg_playing_clipNoRight = -1;
			pg_all_clip_status[pg_enum_clipRight].setCurrentlyPlaying_clipNo(0, pg_playing_clipNoRight);
			sprintf(pg_AuxString, "/clip_shortName_1 %s", "---");
			pg_send_message_udp((char*)"s", pg_AuxString, (char*)"udp_TouchOSC_send");
		}
	}
}
void playing_secondClipNameLeft_callBack(pg_Parameter_Input_Type param_input_type, string scenario_or_gui_command_value) {
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_clipCaptFreq] && PG_NB_PARALLEL_CLIPS >= 2) {
		if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
			if (scenario_or_gui_command_value != "NULL") {
				for (int ind_clip = 0; ind_clip < pg_nbClips[pg_ind_scenario]; ind_clip++) {
					if (pg_clip_tracks[pg_ind_scenario][ind_clip].get_name() == scenario_or_gui_command_value) {
						// starts playing another clip
						//printf("Start playing clip \n");
						pg_playing_secondClipNoLeft = ind_clip;
						printf("playing second left clip name %s\n", scenario_or_gui_command_value.c_str());
						pg_play_clip_no(1, pg_enum_clipLeft, pg_playing_secondClipNoLeft);
						return;
					}
				}
				printf("Second left cLip not found [%s]\n", scenario_or_gui_command_value.c_str());
			}
			else {
				//printf("Stop playing second left cLip [NULL]\n");
			}
			pg_playing_secondClipNoLeft = -1;
			pg_all_clip_status[pg_enum_clipLeft].setCurrentlyPlaying_clipNo(1, pg_playing_secondClipNoLeft);
			sprintf(pg_AuxString, "/clip2_shortName_0 %s", "---");
			pg_send_message_udp((char*)"s", pg_AuxString, (char*)"udp_TouchOSC_send");
		}
	}
}

void playing_secondClipNameRight_callBack(pg_Parameter_Input_Type param_input_type, string scenario_or_gui_command_value) {
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_clipCaptFreq] && PG_NB_PARALLEL_CLIPS >= 2) {
		if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
			if (scenario_or_gui_command_value != "NULL" && scenario_or_gui_command_value != "") {
				for (int ind_clip = 0; ind_clip < pg_nbClips[pg_ind_scenario]; ind_clip++) {
					if (pg_clip_tracks[pg_ind_scenario][ind_clip].get_name() == scenario_or_gui_command_value) {
						// starts playing another clip
						//printf("Start playing clip \n");
						pg_playing_secondClipNoRight = ind_clip;
						printf("playing second right clip name %s\n", scenario_or_gui_command_value.c_str());
						pg_play_clip_no(1, pg_enum_clipRight, pg_playing_secondClipNoRight);
						return;
					}
				}
				printf("Second left cLip not found [%s]\n", scenario_or_gui_command_value.c_str());
			}
			else {
				//printf("Stop playing second left cLip [NULL]\n");
			}
			pg_playing_secondClipNoRight = -1;
			pg_all_clip_status[pg_enum_clipRight].setCurrentlyPlaying_clipNo(1, pg_playing_secondClipNoRight);
			sprintf(pg_AuxString, "/clip2_shortName_1 %s", "---");
			pg_send_message_udp((char*)"s", pg_AuxString, (char*)"udp_TouchOSC_send");
		}
	}
}

#if defined(var_Caverne_Mesh_Profusion)
void extern_movieNo_callBack(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
		if (extern_movieNo != pg_current_extern_movieNo) {
			pg_current_extern_movieNo = extern_movieNo;
			sprintf(pg_AuxString, "/video %.0f", extern_movieNo);
			pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_Processing_send");
		}
	}
}
#endif

void photo_diaporama_callBack(pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value) {
	if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_KEYSTROKE || param_input_type == pg_enum_PG_SCENARIO) {
		//printf("photo_diaporama_callBack %d\n", scenario_or_gui_command_value);
		if (pg_nbCompressedImageDirs[pg_ind_scenario] > 0 && scenario_or_gui_command_value >= 0) {
			photo_diaporama = int(scenario_or_gui_command_value) % pg_nbCompressedImageDirs[pg_ind_scenario];
			//printf("new photo_diaporama %d\n", photo_diaporama);
			sprintf(pg_AuxString, "/diaporama_shortName %03d", photo_diaporama);
			pg_send_message_udp((char*)"s", pg_AuxString, (char*)"udp_TouchOSC_send");
			pg_launch_diaporama(0);
		}
		else if (pg_nbCompressedImageDirs[pg_ind_scenario] > 0 && scenario_or_gui_command_value < 0) {
			photo_diaporama = -1;
			//printf("photo_diaporama %d\n", photo_diaporama);
			sprintf(pg_AuxString, "/diaporama_shortName %03d", photo_diaporama);
			pg_send_message_udp((char*)"s", pg_AuxString, (char*)"udp_TouchOSC_send");
			pg_launch_diaporama(0);
		}
		// do nothing / no images to display
		else {
		}

		// photo diaporama initialization
		//printf("current photo diaporama %d\n", scenario_or_gui_command_value);
		ExclusiveButtonsAndLabelsOnOff<int>(pg_FondButtonsPaths, pg_FondButtonLabelsPaths, pg_FondButtonValues, true, scenario_or_gui_command_value);
	}
}

void flashCameraTrkLength_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_KEYSTROKE || param_input_type == pg_enum_PG_SCENARIO) {
		if (scenario_or_gui_command_value > 0.) {
			pg_flashCameraTrk_decay = float(1. / scenario_or_gui_command_value);
		}
		else {
			pg_flashCameraTrk_decay = 0.001f;
		}
	}
}
void flashPhotoTrkLength_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_KEYSTROKE || param_input_type == pg_enum_PG_SCENARIO) {
		if (scenario_or_gui_command_value > 0.) {
			pg_flashPhotoTrk_decay = 1.f / scenario_or_gui_command_value;
		}
		else {
			pg_flashPhotoTrk_decay = 0.001f;
		}
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
void currentLightScene_callBack(pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value) {
	if ((param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) && scenario_or_gui_command_value >= 0) {
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_currentLightScene]) {
			sprintf(pg_AuxString, "/light_control/light_scene_start %d", int(currentLightScene) - 1);
			pg_send_message_udp((char*)"i", pg_AuxString, (char*)"udp_Python_send");
		}
	}
}

void sound_env_min_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
		sprintf(pg_AuxString, "/sound_env_min %.2f", scenario_or_gui_command_value);
		pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_PD_send");
	}
	// printf("reset sound\n");
}

void sound_env_max_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
		sprintf(pg_AuxString, "/sound_env_max %.2f", scenario_or_gui_command_value);
		pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_PD_send");
	}
	// printf("reset sound\n");
}

void activeMeshes_callBack(pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value) {
	if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_KEYSTROKE || param_input_type == pg_enum_PG_SCENARIO) {
		// MESH INTERFACE VARIABLE INITIALIZATION
		for (unsigned int indMesh = 0; indMesh < pg_Meshes[pg_ind_scenario].size(); indMesh++) {
			sprintf(pg_AuxString, "/Mesh_%d_onOff %d", indMesh + 1, (activeMeshes & (1 << (indMesh)))); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
		}
	}
}
void mobileMeshes_callBack(pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value) {
	if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_KEYSTROKE || param_input_type == pg_enum_PG_SCENARIO) {
		// MESH INTERFACE VARIABLE INITIALIZATION
		for (unsigned int indMesh = 0; indMesh < pg_Meshes[pg_ind_scenario].size(); indMesh++) {
			sprintf(pg_AuxString, "/Mesh_mobile_%d_onOff %d", indMesh + 1, (mobileMeshes & (1 << (indMesh)))); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
		}
	}
}
#if defined(var_MmeShanghai_brokenGlass)
void MmeShanghai_brokenGlass_callBack(pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value) {
	if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_KEYSTROKE || param_input_type == pg_enum_PG_SCENARIO) {
		// activation of a mesh subgroup for which the edges will be drawn
	}
}
#endif
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
void path_group_callBack(pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value) {
	if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
		if (int(scenario_or_gui_command_value) > pg_nb_SVG_path_groups[pg_ind_scenario]) {
			path_group = pg_current_SVG_path_group;
		}
		else if (int(scenario_or_gui_command_value) >= 0
			&& int(scenario_or_gui_command_value) != pg_current_SVG_path_group) {
			//printf("old current path %d\n", current_path_group);
			bool was_path_replay[PG_NB_PATHS + 1];
			for (SVG_scenarioPathCurve& curve : pg_SVG_scenarioPathCurves[pg_ind_scenario]) {
				//printf("path %d path group %d\n", current_path_group, paths[indPrerecPath].path_group);
				if (curve.path_no <= PG_NB_PATHS
					&& curve.path_group == pg_current_SVG_path_group) {
					was_path_replay[curve.path_no]
						= pg_is_path_replay[curve.path_no];
					if (pg_is_path_replay[curve.path_no]) {
						//printf("Stops Replay path_no %d: %d\n", paths[indPrerecPath].path_no, pg_is_path_replay[paths[indPrerecPath].indPath]);

						(curve.path_no);
					}
					// is recording source -> has to stop recording source 
					if (pg_Path_Status[curve.path_no].path_isActiveRecording) {
						//printf("Stops Recording indPath %d: %d\n", paths[indPrerecPath].path_no, pg_Path_Status[paths[indPrerecPath].path_no].path_isActiveRecording);
						pg_path_recording_stop(curve.path_no);
					}
				}
			}
			pg_clear_path_group();
			pg_current_SVG_path_group = int(scenario_or_gui_command_value);
			//printf("new current path %d\n", current_path_group);
			for (SVG_scenarioPathCurve& curve : pg_SVG_scenarioPathCurves[pg_ind_scenario]) {
				int pathNo = curve.path_no;
				//printf("path %d path group %d\n", current_path_group, paths[indPrerecPath].path_group);
				if (pathNo <= PG_NB_PATHS
					&& curve.path_group == pg_current_SVG_path_group) {
					//printf("load path for replay %s\n", paths[indPrerecPath].path_fileName.c_str());
					pg_Path_Status[pathNo].load_svg_path((char*)curve.path_fileName.c_str(),
						curve.pathRadius,
						curve.path_r_color,
						curve.path_g_color,
						curve.path_b_color,
						curve.path_readSpeedScale,
						curve.path_ID,
						curve.with_color_radius_from_scenario,
						curve.secondsforwidth,
						pg_ind_scenario);
					// start reading if it was already reading
					if (was_path_replay[curve.path_no]) {
						//printf("Starts Replay indPath %d: %d\n", paths[indPrerecPath].path_no, was_path_replay[paths[indPrerecPath].path_no]);
						pg_path_replay_trackNo_start(curve.path_no, curve.path_track);
					}
				}
			}
		}
	}
}

void path_replay_trackNo_callBack(int pathNo, pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value) {
	int replayTrack = -1;
	// for a keystroke or a GUI, the command is the current track whatever its value
	if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_KEYSTROKE) {
		replayTrack = currentDrawingTrack;
	}
	// for a scenario >= 0 -> replay and -1 -> no replay
	else if (param_input_type == pg_enum_PG_SCENARIO) {
		replayTrack = scenario_or_gui_command_value;
	}
	// is not currently reading -> starts reading if it is a valid track number
	//printf("path %d is replay %d replayTrack %d\n", pathNo, pg_is_path_replay[pathNo], replayTrack);
	if (!pg_is_path_replay[pathNo]) {
		// does not change anything if it is not a valid track
		if (replayTrack < 0 || replayTrack >= PG_NB_TRACKS) {
			//sprintf(pg_AuxString, "/path_replay_trackNo_%d -1", pathNo);
			//pg_send_message_udp((char *)"i", pg_AuxString, (char *)"udp_TouchOSC_send");
			//printf("replay unchanged (stop) invalid track\n");
			return;
		}

		// stops recording if recording is on
		bool isTrackRecord = false;
		isTrackRecord = path_record[pathNo];

		// is currently recording -> stops recording 
		if (isTrackRecord) {
			// stops recording 
			pg_path_recording_onOff(pathNo);
			printf("replay stops recording\n");
		}

		// only reads a track that has been recorded
		// assumes that no svg track has been loaded for this track
		//printf("recorded path %d : %d\n", pathNo, pg_recorded_path[pathNo]);
		if (pg_recorded_path[pathNo] == true) {
			if (tracksSync) {
				pg_synchr_start_path_replay_trackNo[pathNo] = replayTrack;
			}
			else {
				//printf("start replay path %d\n", pathNo);
				pg_path_replay_trackNo_onOff(pathNo, replayTrack);
			}
			//sprintf(pg_AuxString, "/path_replay_trackNo_%d 1", pathNo);
			//pg_send_message_udp((char *)"i", pg_AuxString, (char *)"udp_TouchOSC_send");
			//printf("replay on recorded track (%s)\n", pg_AuxString);
		}
		else {
			sprintf(pg_AuxString, "/path_replay_trackNo_%d -1", pathNo);
			pg_send_message_udp((char*)"i", pg_AuxString, (char*)"udp_TouchOSC_send");
			//printf("replay off non recorded track %s\n", pg_AuxString);
			((int*)pg_FullScenarioVarPointers[_path_replay_trackNo])[pathNo] = -1;
		}
	}
	// is currently reading && replayTrack < 0 (scenario) or on/off command -> stops reading 
	else {
		//printf("1 stop replay path %d replay track %d\n", pathNo, replayTrack);
		if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_KEYSTROKE) {
			replayTrack = -1;
		}
		//printf("2 stop replay path %d replay track %d\n", pathNo, replayTrack);
		if (replayTrack == -1) {
			pg_path_replay_trackNo_onOff(pathNo, replayTrack);
			sprintf(pg_AuxString, "/path_replay_trackNo_%d -1", pathNo);
			pg_send_message_udp((char*)"i", pg_AuxString, (char*)"udp_TouchOSC_send");
			//printf("replay off track was currently read (%s)\n", pg_AuxString);
		}
	}
}

void reload_paths_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value) {
	if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
		if (scenario_or_gui_command_value) {
			for (SVG_scenarioPathCurve& curve : pg_SVG_scenarioPathCurves[pg_ind_scenario]) {
				int pathNo = curve.path_no;
				if (pathNo >= 1 && pathNo <= PG_NB_PATHS) {
					pg_Path_Status[pathNo].load_svg_path((char*)curve.path_fileName.c_str(),
						curve.pathRadius,
						curve.path_r_color,
						curve.path_g_color,
						curve.path_b_color,
						curve.path_readSpeedScale,
						curve.path_ID,
						curve.with_color_radius_from_scenario,
						curve.secondsforwidth,
						pg_ind_scenario);
				}
			}
		}
	}
}

void path_record_callBack(int pathNo, pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value) {
	//keystroke on/off command
	//printf("begin callback path %d is track record %d (recorded %d)\n", pathNo, int(pg_Path_Status[pathNo].path_isActiveRecording), int(pg_recorded_path[pathNo]));
	if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_KEYSTROKE) {
		// is not currently recording -> starts recording 
		if (!pg_Path_Status[pathNo].path_isActiveRecording) {
			pg_recorded_path[pathNo] = true;
			if (tracksSync) {
				pg_synchr_start_recording_path[pathNo] = true;
			}
			else {
				pg_path_recording_onOff(pathNo);
			}
		}
		// is currently recording -> stops recording 
		else {
			// stops recording 
			pg_path_recording_onOff(pathNo);
		}
	}
	// scenario absolute command
	if (param_input_type == pg_enum_PG_SCENARIO) {
		// recording on 
		if (scenario_or_gui_command_value) {
			// starts recording 
			if (!pg_Path_Status[pathNo].path_isActiveRecording) {
				pg_recorded_path[pathNo] = true;
				if (tracksSync) {
					pg_synchr_start_recording_path[pathNo] = true;
				}
				else {
					pg_path_recording_onOff(pathNo);
				}
			}
		}
		// recording off 
		else {
			if (pg_Path_Status[pathNo].path_isActiveRecording) {
				// stops recording 
				pg_path_recording_onOff(pathNo);
			}
		}
	}

	//printf("end callback path record for path 1 recording %d active recording %d (recorded %d)\n", path_record[1], int(pg_Path_Status[1].path_isActiveRecording), pg_recorded_path[1]);
	//printf("end callback path record for path 2 recording %d active recording %d (recorded %d)\n", path_record[2], int(pg_Path_Status[2].path_isActiveRecording), pg_recorded_path[2]);
}
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

void sensor_layout_callBack(pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value) {
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_sensor_layout]) {
		// copies the grid layout
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_sensor_layout]) {
			pg_assignSensorPositions();
		}
	}
}
void sensor_activation_callBack(pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value) {
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_sensor_activation]) {
		// copies the grid layout
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_sensor_layout]) {
			pg_assignSensorActivations();
		}
	}
}
void sensor_sample_setUp_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_sensor_sample_setUp]) {
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_sensor_layout]) {
			pg_sensor_sample_setUp_interpolation();
		}
	}
}
