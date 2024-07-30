////////////////////////////////////////////////////////////////////
// _
// 
// 
//     File pg-update.cpp
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

// callBack variable conversions
bool double_to_bool(double param) {
	return (param != 0);
}
bool double_to_path(double param) {
	return (param > 0);
}
int double_to_sign(double param) {
	return (param > 0 ? 1 : (param < 0 ? -1 : 0));
}


/////////////////////////////////////////////////////////////////////////
// INITIALIZES ALL SCENARIO VARIABLES CALLBACKS
void pg_initializationCallBacks(void) {
	for (int indVar = 0; indVar < _MaxInterpVarIDs; indVar++) {
		if (pg_FullScenarioVarCallbacks[indVar] != NULL) {
			(*pg_FullScenarioVarCallbacks[indVar])(_PG_SCENARIO, InitialValuesInterpVar[pg_current_configuration_rank][indVar]);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////
// SCENARIO BASED COMMANDS
///////////////////////////////////////////////////////////////////////////////////
void auto_beat_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value) {
	lastBeatTime = pg_CurrentClockTime;
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		// sends a message to PD to stop sending music beats
		sprintf(AuxString, "/auto_beat %d", (int)(*((bool*)pg_FullScenarioVarPointers[_auto_beat])));
		pg_send_message_udp((char*)"i", AuxString, (char*)"udp_PD_send");
		// printf("auto_beat [%s] %.2f %d\n", AuxString, scenario_or_gui_command_value, (int)(*((bool *)pg_FullScenarioVarPointers[_auto_beat])));
	}
}
void MIDIwithStroke_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_MIDIwithStroke]) {
			// sends a message to PD to stop sending music beats
			sprintf(AuxString, "/MIDIwithStroke %d", (int)(*((bool*)pg_FullScenarioVarPointers[_MIDIwithStroke])));
			pg_send_message_udp((char*)"i", AuxString, (char*)"udp_PD_send");
			// printf("MIDIwithStroke[%s] %.2f %d\n", AuxString, scenario_or_gui_command_value, (int)(*((bool *)pg_FullScenarioVarPointers[_MIDIwithBeat])));
		}
	}
}
void MIDIwithBeat_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_MIDIwithBeat]) {
			// sends a message to PD to stop sending music beats
			sprintf(AuxString, "/MIDIwithBeat %d", (int)(*((bool*)pg_FullScenarioVarPointers[_MIDIwithBeat])));
			pg_send_message_udp((char*)"i", AuxString, (char*)"udp_PD_send");
			// printf("MIDIwithBeat [%s] %.2f %d\n", AuxString, scenario_or_gui_command_value, (int)(*((bool *)pg_FullScenarioVarPointers[_MIDIwithBeat])));
		}
	}
}
void MIDIwithColor_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_MIDIwithColor]) {
			// sends a message to PD to stop sending music Colors
			sprintf(AuxString, "/MIDIwithColor %d", (int)(*((bool*)pg_FullScenarioVarPointers[_MIDIwithColor])));
			pg_send_message_udp((char*)"i", AuxString, (char*)"udp_PD_send");
			// printf("MIDIwithColor [%s] %.2f %d\n", AuxString, scenario_or_gui_command_value, (int)(*((bool *)pg_FullScenarioVarPointers[_MIDIwithColor])));
		}
	}
}
void MIDIwithBrush_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_MIDIwithBrush]) {
			// sends a message to PD to stop sending music Brushs
			sprintf(AuxString, "/MIDIwithBrush %d", (int)(*((bool*)pg_FullScenarioVarPointers[_MIDIwithBrush])));
			pg_send_message_udp((char*)"i", AuxString, (char*)"udp_PD_send");
			// printf("MIDIwithBrush [%s] %.2f %d\n", AuxString, scenario_or_gui_command_value, (int)(*((bool *)pg_FullScenarioVarPointers[_MIDIwithBrush])));
		}
	}
}
void MIDIwithCameraFlash_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_cameraCaptFreq]) {
			// sends a message to PD to stop sending music Flashs
			sprintf(AuxString, "/MIDIwithCameraFlash %d", (int)(*((bool*)pg_FullScenarioVarPointers[_MIDIwithCameraFlash])));
			pg_send_message_udp((char*)"i", AuxString, (char*)"udp_PD_send");
			// printf("MIDIwithFlash [%s] %.2f %d\n", AuxString, scenario_or_gui_command_value, (int)(*((bool *)pg_FullScenarioVarPointers[_MIDIwithFlash])));
		}
	}
}
#if defined(var_Contact_clip_in_range)
void Contact_clip_in_range_callBack(pg_Parameter_Input_Type param_input_type, string scenario_or_gui_command_value) {
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_clipCaptFreq]) {
		if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
			ContAct_clip_ranges_min.clear();
			ContAct_clip_ranges_max.clear();
			if (scenario_or_gui_command_value != "NULL") {
				std::vector<std::string> ContAct_clip_ranges;
				ContAct_clip_ranges = split_string(scenario_or_gui_command_value, '/');
				for (string& range : ContAct_clip_ranges) {
					std::vector<std::string> clip_min_max;
					clip_min_max = split_string(range, '-');
					ContAct_clip_ranges_min.push_back(stoi(clip_min_max[0]));
					ContAct_clip_ranges_max.push_back(stoi(clip_min_max[1]));
				}
			}
		}
	}
}
#endif
void MIDIwithPhotoFlash_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_MIDIwithPhotoFlash]) {
			// sends a message to PD to stop sending music Flashs
			sprintf(AuxString, "/MIDIwithPhotoFlash %d", (int)(*((bool*)pg_FullScenarioVarPointers[_MIDIwithPhotoFlash])));
			pg_send_message_udp((char*)"i", AuxString, (char*)"udp_PD_send");
			// printf("MIDIwithFlash [%s] %.2f %d\n", AuxString, scenario_or_gui_command_value, (int)(*((bool *)pg_FullScenarioVarPointers[_MIDIwithFlash])));
		}
	}
}
void auto_pulse_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		// sends a message to PD to stop sending music pulse
		sprintf(AuxString, "/auto_pulse %d", (int)(*((bool*)pg_FullScenarioVarPointers[_auto_pulse])));
		pg_send_message_udp((char*)"i", AuxString, (char*)"udp_PD_send");
		// printf("auto_pulse [%s] %.2f %d\n", AuxString, scenario_or_gui_command_value, (int)(*((bool *)pg_FullScenarioVarPointers[_auto_pulse])));
		for (int ind = 0; ind < 3 * 2; ind++) {
			seed_pulsePerlinNoise[ind] = rand_0_1 * 255;
		}
	}
}
void beat_delay_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		sprintf(AuxString, "/beat_delay %.5f", scenario_or_gui_command_value);
		pg_send_message_udp((char*)"f", AuxString, (char*)"udp_PD_send");
	}
}
void beat_threshold_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		sprintf(AuxString, "/beat_threshold %.5f", scenario_or_gui_command_value);
		pg_send_message_udp((char*)"f", AuxString, (char*)"udp_PD_send");
	}
}
void pen_color_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		sprintf(AuxString, "/pen_colorPreset -1.0");
		pg_send_message_udp((char*)"f", AuxString, (char*)"udp_TouchOSC_send");
	}
}

void audioInput_weight_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		audioInput_weight = scenario_or_gui_command_value;
		sprintf(AuxString, "/audioInput_weight %.2f", audioInput_weight);
		pg_send_message_udp((char*)"f", AuxString, (char*)"udp_PD_send");
#if defined(PG_WITH_JUCE)
		sprintf(AuxString, "/JUCE_audioInput_weight %.2f", audioInput_weight);
		pg_send_message_udp((char*)"f", AuxString, (char*)"udp_SoundJUCE_send");
#endif
	}
}

void soundtrack_PD_weight_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		soundtrack_PD_weight = scenario_or_gui_command_value;
		sprintf(AuxString, "/soundtrack_weight %.2f", soundtrack_PD_weight);
		pg_send_message_udp((char*)"f", AuxString, (char*)"udp_PD_send");

#if defined(PG_WITH_JUCE)
		sprintf(AuxString, "/JUCE_soundtrack_weight %.2f", soundtrack_PD_weight);
		pg_send_message_udp((char*)"f", AuxString, (char*)"udp_SoundJUCE_send");
#endif
	}
}

void soundtrack_PA_weight_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		soundtrack_PA_weight = scenario_or_gui_command_value;
	}
}

void pen_hue_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		float control_color[3];
		HSVtoRGB(pen_hue, pen_sat, pen_value, &control_color[0], &control_color[1], &control_color[2]);
		sprintf(AuxString, "/pen_color/color %02x%02x%02xFF", int(control_color[0] * 255), int(control_color[1] * 255), int(control_color[2] * 255)); pg_send_message_udp((char*)"s", (char*)AuxString, (char*)"udp_TouchOSC_send");
		//printf("colorH msg HSV %.2f %.2f %.2f  %s\n", pen_hue, pen_sat, pen_value, AuxString);
	}
}
void pen_sat_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		float control_color[3];
		HSVtoRGB(pen_hue, pen_sat, pen_value, &control_color[0], &control_color[1], &control_color[2]);
		sprintf(AuxString, "/pen_color/color %02x%02x%02xFF", int(control_color[0] * 255), int(control_color[1] * 255), int(control_color[2] * 255)); pg_send_message_udp((char*)"s", (char*)AuxString, (char*)"udp_TouchOSC_send");
		//printf("colorS msg HSV %.2f %.2f %.2f  %s\n", pen_hue, pen_sat, pen_value, AuxString);
	}
}
void pen_value_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		float control_color[3];
		HSVtoRGB(pen_hue, pen_sat, pen_value, &control_color[0], &control_color[1], &control_color[2]);
		sprintf(AuxString, "/pen_color/color %02x%02x%02xFF", int(control_color[0] * 255), int(control_color[1] * 255), int(control_color[2] * 255)); pg_send_message_udp((char*)"s", (char*)AuxString, (char*)"udp_TouchOSC_send");
		//printf("colorV msg HSV %.2f %.2f %.2f  %s\n", pen_hue, pen_sat, pen_value, AuxString);
	}
}
void repop_huePart_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_repop_huePart]
			&& pg_FullScenarioActiveVars[pg_current_configuration_rank][_repop_satPart]
			&& pg_FullScenarioActiveVars[pg_current_configuration_rank][_repop_valuePart]
			&& repop_hsvPart) {
			float control_color[3];
			HSVtoRGB(repop_huePart, repop_satPart, repop_valuePart, &control_color[0], &control_color[1], &control_color[2]);
			sprintf(AuxString, "/part_color/color %02x%02x%02xFF", int(control_color[0] * 255), int(control_color[1] * 255), int(control_color[2] * 255)); pg_send_message_udp((char*)"s", (char*)AuxString, (char*)"udp_TouchOSC_send");
		}
		//printf("colorH msg HSV %.2f %.2f %.2f  %s\n", repop_huePart, repop_satPart, repop_valuePart, AuxString);
	}
}
void repop_satPart_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_repop_huePart]
			&& pg_FullScenarioActiveVars[pg_current_configuration_rank][_repop_satPart]
			&& pg_FullScenarioActiveVars[pg_current_configuration_rank][_repop_valuePart]
			&& repop_hsvPart) {
			float control_color[3];
			HSVtoRGB(repop_huePart, repop_satPart, repop_valuePart, &control_color[0], &control_color[1], &control_color[2]);
			sprintf(AuxString, "/part_color/color %02x%02x%02xFF", int(control_color[0] * 255), int(control_color[1] * 255), int(control_color[2] * 255)); pg_send_message_udp((char*)"s", (char*)AuxString, (char*)"udp_TouchOSC_send");
		}
		//printf("colorS msg HSV %.2f %.2f %.2f  %s\n", repop_huePart, repop_satPart, repop_valuePart, AuxString);
	}
}
void repop_valuePart_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		float control_color[3];
		if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_repop_huePart]
			&& pg_FullScenarioActiveVars[pg_current_configuration_rank][_repop_satPart]
			&& pg_FullScenarioActiveVars[pg_current_configuration_rank][_repop_valuePart]
			&& repop_hsvPart) {
			HSVtoRGB(repop_huePart, repop_satPart, repop_valuePart, &control_color[0], &control_color[1], &control_color[2]);
			sprintf(AuxString, "/part_color/color %02x%02x%02xFF", int(control_color[0] * 255), int(control_color[1] * 255), int(control_color[2] * 255)); pg_send_message_udp((char*)"s", (char*)AuxString, (char*)"udp_TouchOSC_send");
			//printf("colorV msg HSV %.2f %.2f %.2f  %s\n", repop_huePart, repop_satPart, repop_valuePart, AuxString);
		}
	}
}
void fingers_callBack(pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		// drawing type initialization
		NumberOfInteractionFingers(scenario_or_gui_command_value);
	}
}
void mesh_anime_speed_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		for (int indMesh = 0; indMesh < int(pg_Meshes[pg_current_configuration_rank].size()); indMesh++) {
			mesh_startAnime[pg_current_configuration_rank][indMesh] = pg_CurrentClockTime;
		}
	}
}
void mesh_motion_speed_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		for (int indMesh = 0; indMesh < int(pg_Meshes[pg_current_configuration_rank].size()); indMesh++) {
			mesh_startAnime[pg_current_configuration_rank][indMesh] = pg_CurrentClockTime;
		}
	}
}
void cameraExposure_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_cameraCaptFreq]) {
		if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
			// sprintf(AuxString, "/message Exposure_%.1f", scenario_or_gui_command_value); pg_send_message_udp((char *)"s", (char *)AuxString, (char *)"udp_TouchOSC_send");
			if (scenario_or_gui_command_value != CameraCurrent_exposure) {
				//printf("Cam exposure %.2f\n", scenario_or_gui_command_value);
				// cvSetCaptureProperty comment for see3cam
				//pg_webCam_capture.set(CAP_PROP_EXPOSURE, scenario_or_gui_command_value);
				CameraCurrent_exposure = scenario_or_gui_command_value;
				pg_LastCameraParameterChange_Frame = pg_FrameNo;
			}
		}
	}
}

#if defined(var_Caverne_Mesh_Profusion)
void Caverne_Mesh_Profusion_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		//printf("/Caverne_Mesh_Profusion %.1f\n", scenario_or_gui_command_value);
		if (scenario_or_gui_command_value == 1.) {
			for (int indMesh = 7; indMesh < pg_Meshes[pg_current_configuration_rank].size(); indMesh++) {
				Caverne_Mesh_Profusion_On(indMesh);
			}
		}
		else if (scenario_or_gui_command_value == 0.) {
			for (int indMesh = 7; indMesh < pg_Meshes[pg_current_configuration_rank].size(); indMesh++) {
				Caverne_Mesh_Profusion_Off(indMesh);
			}
		}
	}
}
#endif
void cameraGamma_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_cameraCaptFreq]) {
		if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
			// sprintf(AuxString, "/message Gamma%.1f", scenario_or_gui_command_value); pg_send_message_udp((char *)"s", (char *)AuxString, (char *)"udp_TouchOSC_send");
			if (scenario_or_gui_command_value != CameraCurrent_gamma) {
				//printf("Cam gamma %.2f\n", scenario_or_gui_command_value);
				// cvSetCaptureProperty comment for see3cam
				pg_webCam_capture.set(CAP_PROP_GAMMA, scenario_or_gui_command_value);
				CameraCurrent_gamma = scenario_or_gui_command_value;
				pg_LastCameraParameterChange_Frame = pg_FrameNo;
			}
		}
	}
}
void cameraGain_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_cameraCaptFreq]) {
		if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
			if (scenario_or_gui_command_value != CameraCurrent_gain) {
				printf("Cam gain %.2f\n", scenario_or_gui_command_value);
				// cvSetCaptureProperty comment for see3cam 
				pg_webCam_capture.set(CAP_PROP_GAIN, scenario_or_gui_command_value);
				CameraCurrent_gain = scenario_or_gui_command_value;
				pg_LastCameraParameterChange_Frame = pg_FrameNo;
			}
		}
	}
}
void cameraBrightness_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_cameraCaptFreq]) {
		if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
			if (scenario_or_gui_command_value != CameraCurrent_brightness) {
				//printf("Cam brightness %.2f\n", scenario_or_gui_command_value);
				// cvSetCaptureProperty comment for see3cam
				pg_webCam_capture.set(CAP_PROP_BRIGHTNESS, scenario_or_gui_command_value);
				CameraCurrent_brightness = scenario_or_gui_command_value;
				pg_LastCameraParameterChange_Frame = pg_FrameNo;
			}
		}
	}
}
void cameraSaturation_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_cameraCaptFreq]) {
		if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
			if (scenario_or_gui_command_value != CameraCurrent_saturation) {
				//printf("Cam saturation %.2f\n", scenario_or_gui_command_value);
				// cvSetCaptureProperty comment for see3cam 
				pg_webCam_capture.set(CAP_PROP_SATURATION, scenario_or_gui_command_value);
				CameraCurrent_saturation = scenario_or_gui_command_value;
				pg_LastCameraParameterChange_Frame = pg_FrameNo;
			}
		}
	}
}
void cameraContrast_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_cameraCaptFreq]) {
		if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
			if (scenario_or_gui_command_value != CameraCurrent_contrast) {
				//printf("Cam contrast %.2f\n", scenario_or_gui_command_value);
				// cvSetCaptureProperty comment for see3cam
				pg_webCam_capture.set(CAP_PROP_CONTRAST, scenario_or_gui_command_value);
				CameraCurrent_contrast = scenario_or_gui_command_value;
				pg_LastCameraParameterChange_Frame = pg_FrameNo;
			}
		}
	}
}
void cameraWB_B_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_cameraCaptFreq]) {
		if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
			if (scenario_or_gui_command_value != CameraCurrent_WB_B) {
				//printf("Cam WB B %.2f\n", scenario_or_gui_command_value);
				// cvSetCaptureProperty comment for see3cam
				pg_webCam_capture.set(CAP_PROP_WHITE_BALANCE_BLUE_U, scenario_or_gui_command_value);
				CameraCurrent_WB_B = scenario_or_gui_command_value;
				pg_LastCameraParameterChange_Frame = pg_FrameNo;
			}
		}
	}
}
void cameraWB_R_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_cameraCaptFreq]) {
		if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
			if (scenario_or_gui_command_value != CameraCurrent_WB_R) {
				//printf("Cam WB R %.2f\n", scenario_or_gui_command_value);
				// cvSetCaptureProperty comment for see3cam
				if (scenario_or_gui_command_value >= 0) {
					pg_webCam_capture.set(CAP_PROP_WHITE_BALANCE_RED_V, scenario_or_gui_command_value);
					CameraCurrent_WB_R = scenario_or_gui_command_value;
					pg_LastCameraParameterChange_Frame = pg_FrameNo;
				}
			}
		}
	}
}
void playing_soundtrackNo_callBack(pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		// play a new track
		if (int(scenario_or_gui_command_value) >= 0
			&& int(scenario_or_gui_command_value) < int(pg_SoundTracks[pg_current_configuration_rank].size())
			&& currentlyPlaying_trackNo != int(scenario_or_gui_command_value)) {
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
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_movieCaptFreq]) {
		if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
			// starts playing another video
			pg_play_movie_no();

			// if the video no is outside the range of available videos: stops the playing video
			if (playing_movieNo != currentlyPlaying_movieNo
				&& (playing_movieNo < 0 || playing_movieNo >= int(pg_VideoTracks[pg_current_configuration_rank].size()))) {
				printf("video no is outside the range of available videos: stops the playing video \n");

				currentlyPlaying_movieNo = -1;
				movieWeight = 0.f;
				pg_movie_frame.setTo(Scalar(0, 0, 0));
				sprintf(AuxString, "/movie_shortName %s", "---");
				pg_send_message_udp((char*)"s", AuxString, (char*)"udp_TouchOSC_send");
			}
		}
	}
}
void movieCaptFreq_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_movieCaptFreq]) {
		if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
			//printf("Movie No %d\n", playing_movieNo);
			// if the video no is outside the range of available videos: stops the playing video
			if (playing_movieNo >= 0 && playing_movieNo < int(pg_VideoTracks[pg_current_configuration_rank].size())) {
				pg_movie_status.reset_initialTime(scenario_or_gui_command_value);
			}
		}
	}
}
void cameraNo_callBack(pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value) {
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_cameraCaptFreq]) {
		if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
			//printf("camera current %d new %d param type %d\n", pg_current_active_cameraNo, cameraNo, param_input_type);
			if (cameraNo != pg_current_active_cameraNo) {
				pg_releaseCameraCapture();
				pg_openCameraCaptureAndLoadFrame();
			}
		}
	}
}

void script_1_callBack(pg_Parameter_Input_Type param_input_type, string scenario_or_gui_command_value) {
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_script_1]) {
		if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
			// stops the current script #1 if one is active
			if (currently_runnings_script_1 != "NULL") {
				if (pi_script_1.hProcess != NULL) {
					TerminateProcess(pi_script_1.hProcess, 0);
				}

				std::cout << "quit script" << std::endl;
				currently_runnings_script_1 = "NULL";
			}
			// launches a new current script #1 if one is given by the scenario
			if (scenario_or_gui_command_value != "NULL") {
				// launches the script #1
				currently_runnings_script_1 = scenario_or_gui_command_value;
				string  full_script_name = scripts_directory + currently_runnings_script_1;
				unsigned int serverPort = 7000;
				for (pg_IPClient& client : IP_Clients) {
					if (strcmp("udp_script_1_send", client.id.c_str()) == 0) {
						serverPort = client.Remote_server_port;
						break;
					}
				}
				unsigned int clientPort = 9000;
				for (pg_IPServer& server : IP_Servers) {
					if (strcmp("udp_TouchOSC_receive", server.id.c_str()) == 0) {
						clientPort = server.Local_server_port;
						break;
					}
				}

				sprintf(ErrorStr, "python.exe %s -p %d -s %d", full_script_name.c_str(), clientPort, serverPort);
				std::cout << "run script " << string(ErrorStr) << std::endl;
				si_script_1.cb = sizeof(STARTUPINFO);
				si_script_1.dwFlags = STARTF_USESHOWWINDOW;
				si_script_1.wShowWindow = FALSE;
				if (pi_script_1.hProcess != NULL) {
					TerminateProcess(pi_script_1.hProcess, 0);
				}
				if (!CreateProcessA(NULL,   // No module name (use command line)
					ErrorStr,        // Command line
					NULL,           // Process handle not inheritable
					NULL,           // Thread handle not inheritable
					FALSE,          // Set handle inheritance to FALSE
					0,              // No creation flags
					NULL,           // Use parent's environment block
					NULL,           // Use parent's starting directory 
					&si_script_1,            // Pointer to STARTUPINFO structure
					&pi_script_1)           // Pointer to PROCESS_INFORMATION structure
					)
				{
					printf("CreateProcess failed (%d).\n", GetLastError());
					return;
				}
				//system(ErrorStr);
			}
		}
	}
}

void playing_clipNameLeft_callBack(pg_Parameter_Input_Type param_input_type, string scenario_or_gui_command_value) {
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_clipCaptFreq]) {
		if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
			if (scenario_or_gui_command_value != "NULL") {
				for (int ind_clip = 0; ind_clip < pg_nbClips[pg_current_configuration_rank]; ind_clip++) {
					//printf("clip ID (%s)\n", pg_clip_tracks[pg_current_configuration_rank][ind_clip].get_name().c_str());
					if (pg_clip_tracks[pg_current_configuration_rank][ind_clip].get_name() == scenario_or_gui_command_value) {
						// starts playing another clip
						//printf("Start playing clip \n");
						playing_clipNoLeft = ind_clip;
						printf("playing left clip name %s\n", scenario_or_gui_command_value.c_str());
						pg_play_clip_no(0, _clipLeft, playing_clipNoLeft);
						return;
					}
				}
				printf("Left cLip not found [%s]\n", scenario_or_gui_command_value.c_str());
			}
			else {
				//printf("Stop playing left cLip [NULL]\n");
			}
			playing_clipNoLeft = -1;
			pg_clip_status[_clipLeft].setCurrentlyPlaying_clipNo(0, playing_clipNoLeft);
			sprintf(AuxString, "/clip_shortName_0 %s", "---");
			pg_send_message_udp((char*)"s", AuxString, (char*)"udp_TouchOSC_send");
		}
	}
}
void playing_clipNameRight_callBack(pg_Parameter_Input_Type param_input_type, string scenario_or_gui_command_value) {
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_clipCaptFreq]) {
		if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
			if (scenario_or_gui_command_value != "NULL") {
				for (int ind_clip = 0; ind_clip < pg_nbClips[pg_current_configuration_rank]; ind_clip++) {
					if (pg_clip_tracks[pg_current_configuration_rank][ind_clip].get_name() == scenario_or_gui_command_value) {
						// starts playing another clip
						//printf("Start playing clip \n");
						playing_clipNoRight = ind_clip;
						printf("playing right clip name %s\n", scenario_or_gui_command_value.c_str());
						pg_play_clip_no(0, _clipRight, playing_clipNoRight);
						return;
					}
				}
				printf("Right cLip not found [%s]\n", scenario_or_gui_command_value.c_str());
			}
			else {
				//printf("Stop playing right cLip [NULL]\n");
			}
			playing_clipNoRight = -1;
			pg_clip_status[_clipRight].setCurrentlyPlaying_clipNo(0, playing_clipNoRight);
			sprintf(AuxString, "/clip_shortName_1 %s", "---");
			pg_send_message_udp((char*)"s", AuxString, (char*)"udp_TouchOSC_send");
		}
	}
}
#if PG_NB_PARALLEL_CLIPS >= 2
void playing_secondClipNameLeft_callBack(pg_Parameter_Input_Type param_input_type, string scenario_or_gui_command_value) {
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_clipCaptFreq]) {
		if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
			if (scenario_or_gui_command_value != "NULL") {
				for (int ind_clip = 0; ind_clip < pg_nbClips[pg_current_configuration_rank]; ind_clip++) {
					if (pg_clip_tracks[pg_current_configuration_rank][ind_clip].get_name() == scenario_or_gui_command_value) {
						// starts playing another clip
						//printf("Start playing clip \n");
						playing_secondClipNoLeft = ind_clip;
						printf("playing second left clip name %s\n", scenario_or_gui_command_value.c_str());
						pg_play_clip_no(1, _clipLeft, playing_secondClipNoLeft);
						return;
					}
				}
				printf("Second left cLip not found [%s]\n", scenario_or_gui_command_value.c_str());
			}
			else {
				//printf("Stop playing second left cLip [NULL]\n");
			}
			playing_secondClipNoLeft = -1;
			pg_clip_status[_clipLeft].setCurrentlyPlaying_clipNo(1, playing_secondClipNoLeft);
			sprintf(AuxString, "/clip2_shortName_0 %s", "---");
			pg_send_message_udp((char*)"s", AuxString, (char*)"udp_TouchOSC_send");
		}
	}
}

void playing_secondClipNameRight_callBack(pg_Parameter_Input_Type param_input_type, string scenario_or_gui_command_value) {
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_clipCaptFreq]) {
		if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
			if (scenario_or_gui_command_value != "NULL" && scenario_or_gui_command_value != "") {
				for (int ind_clip = 0; ind_clip < pg_nbClips[pg_current_configuration_rank]; ind_clip++) {
					if (pg_clip_tracks[pg_current_configuration_rank][ind_clip].get_name() == scenario_or_gui_command_value) {
						// starts playing another clip
						//printf("Start playing clip \n");
						playing_secondClipNoRight = ind_clip;
						printf("playing second right clip name %s\n", scenario_or_gui_command_value.c_str());
						pg_play_clip_no(1, _clipRight, playing_secondClipNoRight);
						return;
					}
				}
				printf("Second left cLip not found [%s]\n", scenario_or_gui_command_value.c_str());
			}
			else {
				//printf("Stop playing second left cLip [NULL]\n");
			}
			playing_secondClipNoRight = -1;
			pg_clip_status[_clipRight].setCurrentlyPlaying_clipNo(1, playing_secondClipNoRight);
			sprintf(AuxString, "/clip2_shortName_1 %s", "---");
			pg_send_message_udp((char*)"s", AuxString, (char*)"udp_TouchOSC_send");
		}
	}
}
#endif

#if defined(var_Caverne_Mesh_Profusion)
void extern_movieNo_callBack(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		if (extern_movieNo != current_extern_movieNo) {
			current_extern_movieNo = extern_movieNo;
			sprintf(AuxString, "/video %.0f", extern_movieNo);
			pg_send_message_udp((char*)"f", AuxString, (char*)"udp_Processing_send");
		}
	}
}
#endif

void photo_diaporama_callBack(pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_KEYSTROKE || param_input_type == _PG_SCENARIO) {
		//printf("photo_diaporama_callBack %d\n", scenario_or_gui_command_value);
		if (pg_nbCompressedImageDirs[pg_current_configuration_rank] > 0 && scenario_or_gui_command_value >= 0) {
			photo_diaporama = int(scenario_or_gui_command_value) % pg_nbCompressedImageDirs[pg_current_configuration_rank];
			//printf("new photo_diaporama %d\n", photo_diaporama);
			sprintf(AuxString, "/diaporama_shortName %03d", photo_diaporama);
			pg_send_message_udp((char*)"s", AuxString, (char*)"udp_TouchOSC_send");
			pg_launch_diaporama(0);
		}
		else if (pg_nbCompressedImageDirs[pg_current_configuration_rank] > 0 && scenario_or_gui_command_value < 0) {
			photo_diaporama = -1;
			//printf("photo_diaporama %d\n", photo_diaporama);
			sprintf(AuxString, "/diaporama_shortName %03d", photo_diaporama);
			pg_send_message_udp((char*)"s", AuxString, (char*)"udp_TouchOSC_send");
			pg_launch_diaporama(0);
		}
		// do nothing / no images to display
		else {
		}

		// photo diaporama initialization
		//printf("current photo diaporama %d\n", scenario_or_gui_command_value);
		ExclusiveButtonsAndLabelsOnOff(FondButtonsPaths, FondButtonLabelsPaths, FondButtonValues, true, scenario_or_gui_command_value);
	}
}

void flashCameraTrkLength_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_KEYSTROKE || param_input_type == _PG_SCENARIO) {
		if (scenario_or_gui_command_value > 0.) {
			flashCameraTrk_decay = float(1. / scenario_or_gui_command_value);
		}
		else {
			flashCameraTrk_decay = 0.001f;
		}
	}
}
void flashPhotoTrkLength_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_KEYSTROKE || param_input_type == _PG_SCENARIO) {
		if (scenario_or_gui_command_value > 0.) {
			flashPhotoTrk_decay = 1.f / scenario_or_gui_command_value;
		}
		else {
			flashPhotoTrk_decay = 0.001f;
		}
	}
}
void pen_brush_callBack(pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_KEYSTROKE || param_input_type == _PG_SCENARIO) {
		//if (scenario_or_gui_command_value >= 1) { // large radius for the image brushes*
		//	pen_radiusMultiplier = 50.0f;
		//}
		//else {
		//	pen_radiusMultiplier = 1.0f;
		//}
	}
}
void clearAllLayers_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_KEYSTROKE) {
		//printf("delta %.3f\n", (float)(pg_CurrentClockTime - lastClearTime) );
		if (pg_CurrentClockTime - lastClearTime < 0.5f) {
			// pg_send_message_udp((char *)"s", (char *)"/message clear_all", (char *)"udp_TouchOSC_send");
			isClearAllLayers = 1.f;
			lastClearTime = 0.0f;
			//printf("****** _PG_GUI_COMMAND isClearAllLayers %d\n",isClearAllLayers);
		}
		else {
			lastClearTime = pg_CurrentClockTime;
		}
	}
	// clearing is only made once at the beginning of a scene
	else if (param_input_type == _PG_SCENARIO && scenario_or_gui_command_value && lastClearSceneIndex != pg_CurrentSceneIndex) {
		// pg_send_message_udp((char *)"s", (char *)"/message clear_all", (char *)"udp_TouchOSC_send");
		isClearAllLayers = 1.f;
		lastClearTime = pg_CurrentClockTime;
		lastClearSceneIndex = pg_CurrentSceneIndex;
		//printf("****** _PG_SCENARIO isClearAllLayers %d %d %d\n", isClearAllLayers, pg_FirstFrameInScene, pg_CurrentSceneIndex);
	}
}
void clearLayer_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_KEYSTROKE) {
		if (pg_CurrentClockTime - lastClearTime < 0.2f) {
			isClearLayer = 1.f;
			lastClearTime = 0.0f;
			// pg_send_message_udp((char *)"s", (char *)"/message clear_all_layers", (char *)"udp_TouchOSC_send");
		}
		else {
			isClearLayer = 1.f;
			lastClearTime = pg_CurrentClockTime;
			// pg_send_message_udp((char *)"s", (char *)"/message clear_layer", (char *)"udp_TouchOSC_send");
		}
	}
	else if (param_input_type == _PG_SCENARIO && scenario_or_gui_command_value && lastClearSceneIndex != pg_CurrentSceneIndex) {
		isClearLayer = 1.f;
		lastClearTime = pg_CurrentClockTime;
		lastClearSceneIndex = pg_CurrentSceneIndex;
	}
}
#if defined(var_currentLightScene)
void currentLightScene_callBack(pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value) {
	if ((param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) && scenario_or_gui_command_value >= 0) {
		sprintf(AuxString, "/light_control/light_scene_start %d", int(currentLightScene) - 1);
		pg_send_message_udp((char*)"i", AuxString, (char*)"udp_Python_send");
	}
}
#endif

void sound_env_min_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		sprintf(AuxString, "/sound_env_min %.2f", scenario_or_gui_command_value);
		pg_send_message_udp((char*)"f", AuxString, (char*)"udp_PD_send");
	}
	// printf("reset sound\n");
}

void sound_env_max_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		sprintf(AuxString, "/sound_env_max %.2f", scenario_or_gui_command_value);
		pg_send_message_udp((char*)"f", AuxString, (char*)"udp_PD_send");
	}
	// printf("reset sound\n");
}

void activeMeshes_callBack(pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_KEYSTROKE || param_input_type == _PG_SCENARIO) {
		// MESH INTERFACE VARIABLE INITIALIZATION
		for (unsigned int indMesh = 0; indMesh < pg_Meshes[pg_current_configuration_rank].size(); indMesh++) {
			sprintf(AuxString, "/Mesh_%d_onOff %d", indMesh + 1, (activeMeshes & (1 << (indMesh)))); pg_send_message_udp((char*)"i", (char*)AuxString, (char*)"udp_TouchOSC_send");
		}
	}
}
void mobileMeshes_callBack(pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_KEYSTROKE || param_input_type == _PG_SCENARIO) {
		// MESH INTERFACE VARIABLE INITIALIZATION
		for (unsigned int indMesh = 0; indMesh < pg_Meshes[pg_current_configuration_rank].size(); indMesh++) {
			sprintf(AuxString, "/Mesh_mobile_%d_onOff %d", indMesh + 1, (mobileMeshes & (1 << (indMesh)))); pg_send_message_udp((char*)"i", (char*)AuxString, (char*)"udp_TouchOSC_send");
		}
	}
}
#if defined(var_MmeShanghai_brokenGlass)
void MmeShanghai_brokenGlass_callBack(pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_KEYSTROKE || param_input_type == _PG_SCENARIO) {
		// activation of a mesh subgroup for which the edges will be drawn
	}
}
#endif
void clearCA_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_KEYSTROKE) {
		// pg_send_message_udp((char *)"s", (char *)"/message clear_CA", (char *)"udp_TouchOSC_send");
		isClearCA = 1.f;
	}
	else if (param_input_type == _PG_SCENARIO && scenario_or_gui_command_value && lastClearSceneIndex != pg_CurrentSceneIndex) {
		isClearCA = 1.f;
		lastClearSceneIndex = pg_CurrentSceneIndex;
	}
}
void clearEcho_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_KEYSTROKE) {
		// pg_send_message_udp((char *)"s", (char *)"/message clear_CA", (char *)"udp_TouchOSC_send");
		isClearEcho = 1.f;
	}
	else if (param_input_type == _PG_SCENARIO && scenario_or_gui_command_value) {
		isClearEcho = 1.f;
	}
}
void clear_path_group(void) {
	for (int ind = 0; ind <= PG_NB_PATHS; ind++) {
		pg_Path_Status[ind].path_isFirstFrame = false;
		pg_Path_Status[ind].path_isActiveRecording = false;
		pg_Path_Status[ind].path_indReading = -1;
		pg_Path_Status[ind].path_initialTimeReading = 0.0f;
		pg_Path_Status[ind].path_lastPlayedFrameTime = 1.0F;
		pg_Path_Status[ind].path_indPreviousReading = 0;
	}
	for (int ind = 0; ind <= PG_NB_PATHS; ind++) {
		for (int indCurve = 0; indCurve < pg_Path_Status[ind].nbCurves(pg_current_configuration_rank); indCurve++) {
			pg_Path_Status[ind].setCurveValues(pg_current_configuration_rank, 1., 0., 0.);
		}
	}
}
void path_group_callBack(pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		if (int(scenario_or_gui_command_value) > pg_nb_SVG_path_groups[pg_current_configuration_rank]) {
			path_group = pg_current_SVG_path_group;
		}
		else if (int(scenario_or_gui_command_value) >= 0
			&& int(scenario_or_gui_command_value) != pg_current_SVG_path_group) {
			//printf("old current path %d\n", current_path_group);
			bool was_path_replay[PG_NB_PATHS + 1];
			for (SVG_scenarioPathCurve& curve : SVG_scenarioPathCurves[pg_current_configuration_rank]) {
				//printf("path %d path group %d\n", current_path_group, paths[indPrerecPath].path_group);
				if (curve.path_no <= PG_NB_PATHS
					&& curve.path_group == pg_current_SVG_path_group) {
					was_path_replay[curve.path_no]
						= is_path_replay[curve.path_no];
					if (is_path_replay[curve.path_no]) {
						//printf("Stops Replay path_no %d: %d\n", paths[indPrerecPath].path_no, is_path_replay[paths[indPrerecPath].indPath]);

						(curve.path_no);
					}
					// is recording source -> has to stop recording source 
					if (pg_Path_Status[curve.path_no].path_isActiveRecording) {
						//printf("Stops Recording indPath %d: %d\n", paths[indPrerecPath].path_no, pg_Path_Status[paths[indPrerecPath].path_no].path_isActiveRecording);
						pg_path_recording_stop(curve.path_no);
					}
				}
			}
			clear_path_group();
			pg_current_SVG_path_group = int(scenario_or_gui_command_value);
			//printf("new current path %d\n", current_path_group);
			for (SVG_scenarioPathCurve& curve : SVG_scenarioPathCurves[pg_current_configuration_rank]) {
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
						pg_current_configuration_rank);
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
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_KEYSTROKE) {
		replayTrack = currentDrawingTrack;
	}
	// for a scenario >= 0 -> replay and -1 -> no replay
	else if (param_input_type == _PG_SCENARIO) {
		replayTrack = scenario_or_gui_command_value;
	}
	// is not currently reading -> starts reading if it is a valid track number
	//printf("path %d is replay %d replayTrack %d\n", pathNo, is_path_replay[pathNo], replayTrack);
	if (!is_path_replay[pathNo]) {
		// does not change anything if it is not a valid track
		if (replayTrack < 0 || replayTrack >= PG_NB_TRACKS) {
			//sprintf(AuxString, "/path_replay_trackNo_%d -1", pathNo);
			//pg_send_message_udp((char *)"i", AuxString, (char *)"udp_TouchOSC_send");
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
		//printf("recorded path %d : %d\n", pathNo, recorded_path[pathNo]);
		if (recorded_path[pathNo] == true) {
			if (tracksSync) {
				synchr_start_path_replay_trackNo[pathNo] = replayTrack;
			}
			else {
				//printf("start replay path %d\n", pathNo);
				pg_path_replay_trackNo_onOff(pathNo, replayTrack);
			}
			//sprintf(AuxString, "/path_replay_trackNo_%d 1", pathNo);
			//pg_send_message_udp((char *)"i", AuxString, (char *)"udp_TouchOSC_send");
			//printf("replay on recorded track (%s)\n", AuxString);
		}
		else {
			sprintf(AuxString, "/path_replay_trackNo_%d -1", pathNo);
			pg_send_message_udp((char*)"i", AuxString, (char*)"udp_TouchOSC_send");
			//printf("replay off non recorded track %s\n", AuxString);
			((int*)pg_FullScenarioVarPointers[_path_replay_trackNo])[pathNo] = -1;
		}
	}
	// is currently reading && replayTrack < 0 (scenario) or on/off command -> stops reading 
	else {
		//printf("1 stop replay path %d replay track %d\n", pathNo, replayTrack);
		if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_KEYSTROKE) {
			replayTrack = -1;
		}
		//printf("2 stop replay path %d replay track %d\n", pathNo, replayTrack);
		if (replayTrack == -1) {
			pg_path_replay_trackNo_onOff(pathNo, replayTrack);
			sprintf(AuxString, "/path_replay_trackNo_%d -1", pathNo);
			pg_send_message_udp((char*)"i", AuxString, (char*)"udp_TouchOSC_send");
			//printf("replay off track was currently read (%s)\n", AuxString);
		}
	}
}

void reload_paths_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
		if (scenario_or_gui_command_value) {
			for (SVG_scenarioPathCurve& curve : SVG_scenarioPathCurves[pg_current_configuration_rank]) {
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
						pg_current_configuration_rank);
				}
			}
		}
	}
}

void path_record_callBack(int pathNo, pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value) {
	//keystroke on/off command
	//printf("begin callback path %d is track record %d (recorded %d)\n", pathNo, int(pg_Path_Status[pathNo].path_isActiveRecording), int(recorded_path[pathNo]));
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_KEYSTROKE) {
		// is not currently recording -> starts recording 
		if (!pg_Path_Status[pathNo].path_isActiveRecording) {
			recorded_path[pathNo] = true;
			if (tracksSync) {
				synchr_start_recording_path[pathNo] = true;
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
	if (param_input_type == _PG_SCENARIO) {
		// recording on 
		if (scenario_or_gui_command_value) {
			// starts recording 
			if (!pg_Path_Status[pathNo].path_isActiveRecording) {
				recorded_path[pathNo] = true;
				if (tracksSync) {
					synchr_start_recording_path[pathNo] = true;
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

	//printf("end callback path record for path 1 recording %d active recording %d (recorded %d)\n", path_record[1], int(pg_Path_Status[1].path_isActiveRecording), recorded_path[1]);
	//printf("end callback path record for path 2 recording %d active recording %d (recorded %d)\n", path_record[2], int(pg_Path_Status[2].path_isActiveRecording), recorded_path[2]);
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
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_sensor_layout]) {
		// copies the grid layout
		if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_sensor_layout]) {
			assignSensorPositions();
		}
	}
}
void sensor_activation_callBack(pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value) {
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_sensor_activation]) {
		// copies the grid layout
		if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_sensor_layout]) {
			assignSensorActivations();
		}
	}
}
void sensor_sample_setUp_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_sensor_sample_setUp]) {
		if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_sensor_layout]) {
			sensor_sample_setUp_interpolation();
		}
	}
}
