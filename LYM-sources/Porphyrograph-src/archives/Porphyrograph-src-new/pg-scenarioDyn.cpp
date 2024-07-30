/*! \file pg-stroke.cpp
 *
 *
 *     File pg-stroke.cpp
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
 
/////////////////////////////////////////////////////////////////////////
// INITIALIZES ALL SCENARIO VARIABLES AND ASSIGNS THEM THE VALUES OF THE INITIAL VALUES LINE
void pg_initializeScenearioVariables(void) {
	/////////////////////////////////////////////////////////////////////////
	// ADDITION OF SCENARIO-CONTROLLED UNIFORM VARIABLES TO THE HASH MAP
	for (int indVar = 0; indVar < _MaxInterpVarIDs; indVar++) {
		pg_OSC_addresses_hashMap[pg_FullScenarioVarStrings[indVar]] = indVar;
	}

	/////////////////////////////////////////////////////////////////////////
	// VARIABLE INTITIALIZATION FOR EACH CONFIGURATION
	for (int indConfiguration = 0; indConfiguration < pg_NbConfigurations; indConfiguration++) {
		/////////////////////////////////////////////////////////////////////////
		// CONSTANT UNIFORM VARIABLES
		// only assigned at initialization, does not change during the set
			// only assigned at initialization, does not change during the set
		glUseProgram(pg_shader_programme[indConfiguration][_pg_shader_ParticleAnimation]);

		///////////////////////////////////////////////////////////////////
		// VERTEX SHADER VARIABLE
		glUniform2f(uniform_ParticleAnimation_vp_2fv_width_height[indConfiguration],
			(GLfloat)workingWindow_width, (GLfloat)window_height);

		// only assigned at initialization, does not change during the set
		glUseProgram(pg_shader_programme[indConfiguration][_pg_shader_Update]);

		///////////////////////////////////////////////////////////////////
		// VERTEX SHADER VARIABLE
		glUniform2f(uniform_Update_vp_2fv_width_height[indConfiguration],
			(GLfloat)workingWindow_width, (GLfloat)window_height);

		/////////////////////////////////////////////////////////////////////////
		// INItiAL VALUES OF SCENARIO-CONTROLLED UNIFORM VARIABLES
		for (int indP = 0; indP < ScenarioVarNb[indConfiguration]; indP++) {
			int indVar = ConfigScenarioVarRank[indConfiguration][indP];
			if (pg_FullScenarioActiveVars[indConfiguration][indVar]) {
				if (pg_FullScenarioVarIndiceRanges[indVar][0] == -1) {
					if (pg_FullScenarioVarTypes[indVar] == _pg_float) {
						*((float*)pg_FullScenarioVarPointers[indVar]) = float(InitialValuesInterpVar[indConfiguration][indVar].val_num);
					}
					else if (pg_FullScenarioVarTypes[indVar] == _pg_int) {
						*((int*)pg_FullScenarioVarPointers[indVar]) = int(InitialValuesInterpVar[indConfiguration][indVar].val_num);
					}
					else if (pg_FullScenarioVarTypes[indVar] == _pg_bool) {
						*((bool*)pg_FullScenarioVarPointers[indVar]) = (InitialValuesInterpVar[indConfiguration][indVar].val_num != 0);
					}
					else if (pg_FullScenarioVarTypes[indVar] == _pg_sign) {
						*((int*)pg_FullScenarioVarPointers[indVar]) = (InitialValuesInterpVar[indConfiguration][indVar].val_num > 0 ? 1 : (InitialValuesInterpVar[indConfiguration][indVar].val_num < 0 ? -1 : 0));
					}
					else if (pg_FullScenarioVarTypes[indVar] == _pg_path) {
						*((bool*)pg_FullScenarioVarPointers[indVar]) = (InitialValuesInterpVar[indConfiguration][indVar].val_num > 0);
					}
					else if (pg_FullScenarioVarTypes[indVar] == _pg_string) {
						*((string*)pg_FullScenarioVarPointers[indVar]) = InitialValuesInterpVar[indConfiguration][indVar].val_string;
					}
					else {
						sprintf(ErrorStr, "Incorrect variable type (%d) for scenario variable #%d!", pg_FullScenarioVarTypes[indVar], indVar); ReportError(ErrorStr);
					}
				}
				// array
				else {
					for (int index = pg_FullScenarioVarIndiceRanges[indVar][0]; index < pg_FullScenarioVarIndiceRanges[indVar][1]; index++) {
						if (pg_FullScenarioVarTypes[indVar] == _pg_float) {
							((float*)pg_FullScenarioVarPointers[indVar])[index] = float(InitialValuesInterpVar[indConfiguration][indVar].val_array[index]);
						}
						else if (pg_FullScenarioVarTypes[indVar] == _pg_int) {
							((int*)pg_FullScenarioVarPointers[indVar])[index] = int(InitialValuesInterpVar[indConfiguration][indVar].val_array[index]);
						}
						else if (pg_FullScenarioVarTypes[indVar] == _pg_bool) {
							((bool*)pg_FullScenarioVarPointers[indVar])[index] = (InitialValuesInterpVar[indConfiguration][indVar].val_array[index] != 0);
						}
						else if (pg_FullScenarioVarTypes[indVar] == _pg_sign) {
							((int*)pg_FullScenarioVarPointers[indVar])[index] = (InitialValuesInterpVar[indConfiguration][indVar].val_array[index] > 0 ? 1 : (InitialValuesInterpVar[indConfiguration][indVar].val_array[index] < 0 ? -1 : 0));
						}
						else if (pg_FullScenarioVarTypes[indVar] == _pg_path) {
							((bool*)pg_FullScenarioVarPointers[indVar])[index] = (InitialValuesInterpVar[indConfiguration][indVar].val_array[index] > 0);
						}
						else {
							sprintf(ErrorStr, "Incorrect variable type (%d) for scenario variable #%d!", pg_FullScenarioVarTypes[indVar], indVar); ReportError(ErrorStr);
						}
					}
				}
			}
		}
	}

	/////////////////////////////////////////
	// PATH REPLAY/RECORD FOR THE CURRENT CONFIGURATION
	if (pg_current_configuration_rank >= 0 && pg_current_configuration_rank < unsigned int(pg_NbConfigurations)) {
		// track replay 
		// source track recording
		// is recording source -> has to stop recording source 
		for (int indPath = 1; indPath < PG_NB_PATHS; indPath++) {
			if (path_replay_trackNo[indPath] >= 0) {
				if (tracksSync) {
					synchr_start_path_replay_trackNo[indPath] = path_replay_trackNo[indPath];
				}
				else {
					pg_path_replay_trackNo_onOff(indPath, path_replay_trackNo[indPath]);
				}
			}
		}

		// track recording 
		// source track recording
		// is recording source -> has to stop recording source 
		for (int indPath = 1; indPath < PG_NB_PATHS; indPath++) {
			if (!path_record[indPath]) {
				pg_path_recording_stop(indPath);
			}
		}
	}

	////////////////////////////////
	// INTERFACE INTIALIZATION
	for (int indPath = 1; indPath < PG_NB_PATHS; indPath++) {
		sprintf(AuxString, "/path_replay_trackNo_%d -1", indPath);
		pg_send_message_udp((char*)"i", AuxString, (char*)"udp_TouchOSC_send");
		sprintf(AuxString, "/path_record_%d 0", indPath);
		pg_send_message_udp((char*)"i", AuxString, (char*)"udp_TouchOSC_send");
	}

	/////////////////////////////////////////////////////////////////////////
	// SOUND-CONTROLLED UNIFORM VARIABLES: dynamic values received from PD 
	// flashes
	// FOR THE CURRENT CONFIGURATION
	for (int indtrack = 0; indtrack < PG_NB_TRACKS; indtrack++) {
		flashTrkCA_weights[indtrack] = 0;
		flashTrkPart_weights[indtrack] = 0;
		flashTrkBG_weights[indtrack] = 0;
		flashTrkCA_weights_duration[indtrack] = 0;
		flashTrkPart_weights_duration[indtrack] = 0;
		flashTrkBG_weights_duration[indtrack] = 0;
	}
	flashPixel = 0;
	flashCABG_weight = 0;
	flashCAPart_weight = 0;
	flashPartBG_weight = 0;
	flashPartCA_weight = 0;
	flashCABG_weight_duration = 0;
	flashCAPart_weight_duration = 0;
	flashPartBG_weight_duration = 0;
	flashPartCA_weight_duration = 0;

	// video flash
	is_flashCameraTrk = false;
	flashCameraTrk_weight = 0.0f;

	// master flash
	flashMaster = 0;

	/////////////////////////////////////////
	// SCENARIO VARIABLES INITIALIZATION FOR THE CURRENT CONFIGURATION
	if (pg_current_configuration_rank >= 0 && pg_current_configuration_rank < unsigned int(pg_NbConfigurations)) {
		// background subraction
		if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_cameraCaptFreq]) {
			reset_camera = true;
		}

		// particle initialization reset
		part_initialization = -1;
	}

	// clear 
	isClearCA = 0.f;
	isClearLayer = 0.f;
	isClearAllLayers = 0.f;
	isClearEcho = 0.f;

	// DELAYED CAMERA WEIGHT
	delayedCameraWeight = 0;

#if defined(PG_WITH_BLUR)
	// blur
	is_blur_1 = false;
	is_blur_2 = false;
	nb_blur_frames_1 = 0;
	nb_blur_frames_2 = 0;
#endif

#if defined(pg_Project_CAaudio)
	// CA seed
	pg_CAseed_trigger = false;
	pg_CAseed_type = _pg_CAseed_dot_center;
	// INTERFACE VARIABLE INITIALIZATION
	sprintf(AuxString, "/CAseed_dot_center %d", 1); pg_send_message_udp((char*)"i", (char*)AuxString, (char*)"udp_TouchOSC_send");
	sprintf(AuxString, "/CAseed_dot %d", 0); pg_send_message_udp((char*)"i", (char*)AuxString, (char*)"udp_TouchOSC_send");
	sprintf(AuxString, "/CAseed_h_line %d", 0); pg_send_message_udp((char*)"i", (char*)AuxString, (char*)"udp_TouchOSC_send");
	sprintf(AuxString, "/CAseed_v_line %d", 0); pg_send_message_udp((char*)"i", (char*)AuxString, (char*)"udp_TouchOSC_send");
	sprintf(AuxString, "/CAseed_cross %d", 0); pg_send_message_udp((char*)"i", (char*)AuxString, (char*)"udp_TouchOSC_send");
	sprintf(AuxString, "/CAseed_X %d", 0); pg_send_message_udp((char*)"i", (char*)AuxString, (char*)"udp_TouchOSC_send");
	sprintf(AuxString, "/CAseed_square %d", 0); pg_send_message_udp((char*)"i", (char*)AuxString, (char*)"udp_TouchOSC_send");
	pg_CAseed_location = _pg_CAseed_loc_center;
	sprintf(AuxString, "/CAseed_loc %d", pg_CAseed_location); pg_send_message_udp((char*)"i", (char*)AuxString, (char*)"udp_TouchOSC_send");
	pg_CAseed_coordinates[0] = workingWindow_width / 2;
	pg_CAseed_coordinates[1] = window_height / 2;
	pg_CAseed_size = 1;
	sprintf(AuxString, "/CAseed_size %d", pg_CAseed_size); pg_send_message_udp((char*)"i", (char*)AuxString, (char*)"udp_TouchOSC_send");
#endif

	// copy to layer above (+1) or to layer below (-1)
	copyToNextTrack = 0;

	// pen music color
	pulsed_pen_color[0] = RGBpalette_r_ini;
	pulsed_pen_color[1] = RGBpalette_g_ini;
	pulsed_pen_color[2] = RGBpalette_b_ini;
	pulsed_pen_color[3] = RGBpalette_a_ini;

	// repop music color
	pulsed_repop_colorBG[0] = RGBpalette_r_ini;
	pulsed_repop_colorBG[1] = RGBpalette_g_ini;
	pulsed_repop_colorBG[2] = RGBpalette_b_ini;
	pulsed_repop_colorCA[0] = RGBpalette_r_ini;
	pulsed_repop_colorCA[1] = RGBpalette_g_ini;
	pulsed_repop_colorCA[2] = RGBpalette_b_ini;
	pulsed_repop_colorPart[0] = RGBpalette_r_ini;
	pulsed_repop_colorPart[1] = RGBpalette_g_ini;
	pulsed_repop_colorPart[2] = RGBpalette_b_ini;

	// pulse from sound 
	pulse_average = 0.0f;

	/////////////////////////////////////////////////////////////////////////
	// DRAWING-CONTROLLED UNIFORM VARIABLES: dynamic values received from drawing or track display 
	// pen position storage on the two quads 
	for (int indPath = 0; indPath < PG_NB_PATHS + 1; indPath++) {
		paths_x_next[indPath] = PG_OUT_OF_SCREEN_CURSOR;
		paths_y_next[indPath] = PG_OUT_OF_SCREEN_CURSOR;
		paths_x[indPath] = PG_OUT_OF_SCREEN_CURSOR;
		paths_y[indPath] = PG_OUT_OF_SCREEN_CURSOR;
		paths_x_prev[indPath] = PG_OUT_OF_SCREEN_CURSOR;
		paths_y_prev[indPath] = PG_OUT_OF_SCREEN_CURSOR;
		paths_x_prev_prev[indPath] = PG_OUT_OF_SCREEN_CURSOR;
		paths_y_prev_prev[indPath] = PG_OUT_OF_SCREEN_CURSOR;
		paths_x_memory[indPath] = PG_OUT_OF_SCREEN_CURSOR;
		paths_y_memory[indPath] = PG_OUT_OF_SCREEN_CURSOR;
		paths_x_prev_memory[indPath] = PG_OUT_OF_SCREEN_CURSOR;
		paths_y_prev_memory[indPath] = PG_OUT_OF_SCREEN_CURSOR;
		isBegin[indPath] = false;
		isEnd[indPath] = false;

		paths_xL[indPath] = PG_OUT_OF_SCREEN_CURSOR;
		paths_yL[indPath] = PG_OUT_OF_SCREEN_CURSOR;
		paths_xR[indPath] = PG_OUT_OF_SCREEN_CURSOR;
		paths_yR[indPath] = PG_OUT_OF_SCREEN_CURSOR;
		tang_x[indPath] = 0;
		tang_y[indPath] = 0;
		tang_x_prev[indPath] = 0;
		tang_y_prev[indPath] = 0;

		// pen color
		paths_Color_r[indPath] = 0.0F;
		paths_Color_g[indPath] = 0.0F;
		paths_Color_b[indPath] = 0.0F;
		paths_Color_a[indPath] = 1.0F;
		// pen brush & size
		paths_BrushID[indPath] = 0;
		paths_RadiusX[indPath] = 0.0F;
		paths_RadiusY[indPath] = 0.0F;
	}

	// particule color
	repop_ColorBGcolorRed = 0.f;
	repop_ColorBG_g = 0.f;
	repop_ColorBG_b = 0.f;
	repop_ColorCA_r = 0.f;
	repop_ColorCA_g = 0.f;
	repop_ColorCA_b = 0.f;
	repop_ColorPart_r = 0.f;
	repop_ColorPart_g = 0.f;
	repop_ColorPart_b = 0.f;

	// hoover cursor
	CurrentCursorHooverPos_x = 0;
	CurrentCursorHooverPos_y = 0;
	// stylus type
	CurrentCursorStylusvsRubber = pg_Stylus;

	/////////////////////////////////////////////////////////////////////////
	// TIME CONTROLLED VARIABLES (NOT FORWARDED TO SHADERS)
	// scenario management
	pg_CurrentSceneIndex = -1;
	pg_CurrentScene = NULL;

	// frame number initialization
	pg_FrameNo = first_frame_number - 1;
	pg_ConfigurationFrameNo = pg_FrameNo;
	pg_ParticleTargetFrameNo = pg_FrameNo;

	// intial real time
	InitialRealTime = RealTime() + initial_time;
	// initial scenario time: well before current time to be over
	InitialScenarioTime = InitialRealTime - 1000000.f;
	AbsoluteInitialScenarioTime = InitialRealTime - 1000000.f;
	// TIME CONTROLLED UNIFORM VARIABLES
	// message transparency & echo
	messageTransparency = 0.0;

	// pg_send_message_udp((char *)"s", (char *)"/message init_completed", (char *)"udp_TouchOSC_send");
	// INTERFACE VARIABLE INITIALIZATION

	// soundtrack on
	soundTrack_on = true;
	sprintf(AuxString, "/soundtrack_onOff %d", soundTrack_on);
	pg_send_message_udp((char*)"i", AuxString, (char*)"udp_PD_send");
	//printf("Init script: soundtrack: %s\n", AuxString);

	sprintf(AuxString, "/soundtrack_onOff %d", !soundTrack_on);
	pg_send_message_udp((char*)"i", AuxString, (char*)"udp_TouchOSC_send");
	sprintf(AuxString, "/soundtrack_volume %d", int(soundTrack_on));
	pg_send_message_udp((char*)"i", AuxString, (char*)"udp_TouchOSC_send");

#if defined(pg_Project_Pierres)
	updateXYKeystonePad();
#endif

	// rubber vs pen interface initialization
	if (CurrentCursorStylusvsRubber == pg_Stylus) {
		sprintf(AuxString, "/StylusvsRubber %d", pg_Stylus); pg_send_message_udp((char*)"i", (char*)AuxString, (char*)"udp_TouchOSC_send");
	}
	else {
		sprintf(AuxString, "/StylusvsRubber %d", pg_Rubber); pg_send_message_udp((char*)"i", (char*)AuxString, (char*)"udp_TouchOSC_send");
	}

	/////////////////////////////////////////
	// INTERACE INITIALIZATION FOR THE CURRENT CONFIGURATION
	if (pg_current_configuration_rank >= 0 && pg_current_configuration_rank < unsigned int(pg_NbConfigurations)) {
		// color interface initialization
		if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_pen_hue]
			&& pg_FullScenarioActiveVars[pg_current_configuration_rank][_pen_sat]
			&& pg_FullScenarioActiveVars[pg_current_configuration_rank][_pen_value]
			&& pen_hsv) {
			float control_color[3];
			HSVtoRGB(pen_hue, pen_sat, pen_value, &control_color[0], &control_color[1], &control_color[2]);
			sprintf(AuxString, "/pen_color/color %02x%02x%02xFF", int(control_color[0] * 255), int(control_color[1] * 255), int(control_color[2] * 255)); pg_send_message_udp((char*)"s", (char*)AuxString, (char*)"udp_TouchOSC_send");
			//printf("color msg HSV %.2f %.2f %.2f  %s\n", pen_hue, pen_sat, pen_value, AuxString);
		}
		if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_repop_huePart]
			&& pg_FullScenarioActiveVars[pg_current_configuration_rank][_repop_satPart]
			&& pg_FullScenarioActiveVars[pg_current_configuration_rank][_repop_valuePart]
			&& repop_hsvPart) {
			float control_color[3];
			HSVtoRGB(repop_huePart, repop_satPart, repop_valuePart, &control_color[0], &control_color[1], &control_color[2]);
			sprintf(AuxString, "/part_color/color %02x%02x%02xFF", int(control_color[0] * 255), int(control_color[1] * 255), int(control_color[2] * 255)); pg_send_message_udp((char*)"s", (char*)AuxString, (char*)"udp_TouchOSC_send");
		}

		// drawing type initialization
		ExclusiveButtonsAndLabelsOnOff(DessinButtonsPaths, DessinButtonLabelsPaths, DessinButtonValues, true, trkDecay[1]);

		// photo diaporama initialization
		ExclusiveButtonsAndLabelsOnOff(FondButtonsPaths, FondButtonLabelsPaths, FondButtonValues, true, photo_diaporama);

		// drawing type initialization
		ExclusiveButtonsAndLabelsOnOff(StylusvsRubberButtonsPaths, StylusvsRubberButtonLabelsPaths, StylusvsRubberButtonValues, false, CurrentCursorStylusvsRubber);
	}
}

///////////////////////////////////////////////////////////////////////////////////
// SCENARIO BASED COMMANDS
///////////////////////////////////////////////////////////////////////////////////
void pg_update_variable(pg_Parameter_Input_Type param_input_type, int indVar,
	ScenarioValue scenario_or_gui_command_value, int array_index) {
	// save previous values of variables that have to know their previous value in their 
	if (param_input_type == _PG_SCENARIO) {
		if (BrokenInterpolationVar[indVar]) {
			return;
		}
	}

	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_KEYSTROKE) {
		BrokenInterpolationVar[indVar] = true;
	}

	pg_variable_param_input_type[indVar] = param_input_type;

	// dimension 1 variable
	if (pg_FullScenarioVarIndiceRanges[indVar][0] == -1) {
		if (pg_FullScenarioVarTypes[indVar] == _pg_float) {
			if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
				*((float*)pg_FullScenarioVarPointers[indVar]) = float(scenario_or_gui_command_value.val_num);
			}
			//if (indVar == 45) {
			//	printf("Param %d (%s) interp value %.2f var value %.2f\n", 
			//		indVar, pg_FullScenarioVarStrings[indVar].c_str(), float(scenario_or_gui_command_value.val_num), pen_radius);
			//}
		}
		else if (pg_FullScenarioVarTypes[indVar] == _pg_int) {
			if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
				if ((indVar == _CA1Type || indVar == _CA2Type ||
					indVar == _CA1SubType || indVar == _CA2SubType)) {
					// for CAType we choose to alternate randomly between both types, according
					// to the proximity of floor or ceiling
					float randVal = rand_0_1;
					float thefloor = float(floor(scenario_or_gui_command_value.val_num));
					float decimalPart = float(scenario_or_gui_command_value.val_num - thefloor);
					if (randVal > decimalPart) {
						*((int*)pg_FullScenarioVarPointers[indVar]) = (int)thefloor;
					}
					if (randVal < decimalPart) {
						*((int*)pg_FullScenarioVarPointers[indVar]) = (int)thefloor + 1;
					}
				}
				else
				{
					*((int*)pg_FullScenarioVarPointers[indVar]) = int(round(scenario_or_gui_command_value.val_num));
				}
			}
		}
		else if (pg_FullScenarioVarTypes[indVar] == _pg_bool) {
			if (param_input_type == _PG_SCENARIO) {
				*((bool*)pg_FullScenarioVarPointers[indVar]) = bool(round(scenario_or_gui_command_value.val_num));
			}
			else if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_KEYSTROKE) {
				*((bool*)pg_FullScenarioVarPointers[indVar]) = !(*((bool*)pg_FullScenarioVarPointers[indVar]));
				// printf("Opposite boolean index %d value %d\n" , indVar,*((bool *)pg_FullScenarioVarPointers[indVar]));
			}
		}
		else if (pg_FullScenarioVarTypes[indVar] == _pg_sign) {
			if (param_input_type == _PG_SCENARIO) {
				*((int*)pg_FullScenarioVarPointers[indVar]) = (scenario_or_gui_command_value.val_num > 0 ? 1 : (scenario_or_gui_command_value.val_num > 0 ? false : 0));
			}
			else if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_KEYSTROKE) {
				*((float*)pg_FullScenarioVarPointers[indVar]) = -1.0f * (*((float*)pg_FullScenarioVarPointers[indVar]));
			}
		}
		else if (pg_FullScenarioVarTypes[indVar] == _pg_path) {
			if (param_input_type == _PG_SCENARIO) {
				*((bool*)pg_FullScenarioVarPointers[indVar]) = (scenario_or_gui_command_value.val_num > 0 ? true : false);
			}
			else if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_KEYSTROKE) {
				*((bool*)pg_FullScenarioVarPointers[indVar]) = !(*((bool*)pg_FullScenarioVarPointers[indVar]));
				// printf("Opposite boolean index %d value %d\n" , indVar,*((bool *)pg_FullScenarioVarPointers[indVar]));
			}
		}
		// runs the callBack on the updated variables which have one
		if (pg_FullScenarioActiveVars[pg_current_configuration_rank][indVar] && pg_FullScenarioVarCallbacks[indVar]) {
			//printf("Run callback of var \"%s\"\n", pg_FullScenarioVarMessages[indVar].c_str());
			(*pg_FullScenarioVarCallbacks[indVar])(pg_variable_param_input_type[indVar], scenario_or_gui_command_value);
		}
	}
	// dimension 2 variable: between pg_FullScenarioVarIndiceRanges[indVar][0] and pg_FullScenarioVarIndiceRanges[indVar][1] - 1
	else {
		//printf("2D variable  %d-%d type %d\n", pg_FullScenarioVarIndiceRanges[indVar][0], pg_FullScenarioVarIndiceRanges[indVar][1], pg_FullScenarioVarTypes[indVar]);
		if (pg_FullScenarioVarTypes[indVar] == _pg_float) {
			if (scenario_or_gui_command_value.val_array != NULL) {
				if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
					if (array_index < 0) {
						for (int index = pg_FullScenarioVarIndiceRanges[indVar][0]; index < pg_FullScenarioVarIndiceRanges[indVar][1]; index++) {
							((float*)pg_FullScenarioVarPointers[indVar])[index] = float(scenario_or_gui_command_value.val_array[index]);
						}
					}
					else if (array_index < pg_FullScenarioVarIndiceRanges[indVar][1]) {
						//printf("changed value %.2f index %d\n", scenario_or_gui_command_value.val_array[array_index], array_index);
						((float*)pg_FullScenarioVarPointers[indVar])[array_index] = float(scenario_or_gui_command_value.val_array[array_index]);
					}
				}
			}
			else {
				sprintf(ErrorStr, "Unexpected null array for var %d (%s)!", indVar, pg_FullScenarioVarMessages[indVar].c_str()); ReportError(ErrorStr); throw 100;
			}
		}
		else if (pg_FullScenarioVarTypes[indVar] == _pg_int) {
			if (scenario_or_gui_command_value.val_array != NULL) {
				if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
					if ((indVar == _CA1Type || indVar == _CA2Type ||
						indVar == _CA1SubType || indVar == _CA2SubType)) {
						// for CAType we choose to alternate randomly between both types, according
						// to the proximity of floor or ceiling
						if (array_index < 0) {
							for (int index = pg_FullScenarioVarIndiceRanges[indVar][0]; index < pg_FullScenarioVarIndiceRanges[indVar][1]; index++) {
								float randVal = rand_0_1;
								float thefloor = float(floor(scenario_or_gui_command_value.val_array[index]));
								float decimalPart = float(scenario_or_gui_command_value.val_array[index] - thefloor);
								if (randVal > decimalPart) {
									((int*)pg_FullScenarioVarPointers[indVar])[index] = (int)thefloor;
								}
								if (randVal < decimalPart) {
									((int*)pg_FullScenarioVarPointers[indVar])[index] = (int)thefloor + 1;
								}
							}
						}
						else if (array_index < pg_FullScenarioVarIndiceRanges[indVar][1]) {
							float randVal = rand_0_1;
							float thefloor = float(floor(scenario_or_gui_command_value.val_array[array_index]));
							float decimalPart = float(scenario_or_gui_command_value.val_array[array_index] - thefloor);
							if (randVal > decimalPart) {
								((int*)pg_FullScenarioVarPointers[indVar])[array_index] = (int)thefloor;
							}
							if (randVal < decimalPart) {
								((int*)pg_FullScenarioVarPointers[indVar])[array_index] = (int)thefloor + 1;
							}
						}
					}
					else
					{
						//printf("array index int var %d\n", array_index);
						if (array_index < 0) {
							for (int index = pg_FullScenarioVarIndiceRanges[indVar][0]; index < pg_FullScenarioVarIndiceRanges[indVar][1]; index++) {
								((int*)pg_FullScenarioVarPointers[indVar])[index] = int(scenario_or_gui_command_value.val_array[index]);
							}
						}
						else if (array_index < pg_FullScenarioVarIndiceRanges[indVar][1]) {
							//printf("changed value %d index %d\n", int(scenario_or_gui_command_value.val_array[array_index]), array_index);
							((int*)pg_FullScenarioVarPointers[indVar])[array_index] = int(scenario_or_gui_command_value.val_array[array_index]);
						}
					}
				}
			}
			else {
				sprintf(ErrorStr, "Unexpected null array for var %d (%s)!", indVar, pg_FullScenarioVarMessages[indVar].c_str()); ReportError(ErrorStr); throw 100;
			}
		}
		else if (pg_FullScenarioVarTypes[indVar] == _pg_bool) {
			if (scenario_or_gui_command_value.val_array != NULL) {
				if (array_index < 0) {
					for (int index = pg_FullScenarioVarIndiceRanges[indVar][0]; index < pg_FullScenarioVarIndiceRanges[indVar][1]; index++) {
						if (param_input_type == _PG_SCENARIO) {
							((bool*)pg_FullScenarioVarPointers[indVar])[index] = bool(round(scenario_or_gui_command_value.val_array[index]));
						}
						else if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_KEYSTROKE) {
							((bool*)pg_FullScenarioVarPointers[indVar])[index] = !(((bool*)pg_FullScenarioVarPointers[indVar])[index]);
							//printf("Opposite boolean index %d value %d\n" , indVar,*((bool *)pg_FullScenarioVarPointers[indVar]));
						}
					}
				}
				else if (array_index < pg_FullScenarioVarIndiceRanges[indVar][1]) {
					if (param_input_type == _PG_SCENARIO) {
						((bool*)pg_FullScenarioVarPointers[indVar])[array_index] = bool(round(scenario_or_gui_command_value.val_array[array_index]));
					}
					else if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_KEYSTROKE) {
						((bool*)pg_FullScenarioVarPointers[indVar])[array_index] = !(((bool*)pg_FullScenarioVarPointers[indVar])[array_index]);
						//for (int index = pg_FullScenarioVarIndiceRanges[indVar][0]; index < pg_FullScenarioVarIndiceRanges[indVar][1]; index++) {
						//	printf("Opposite boolean index %d value %d array index %d\n", indVar, ((bool*)pg_FullScenarioVarPointers[indVar])[index], index);
						//}
					}
				}
			}
			else {
				sprintf(ErrorStr, "Unexpected null array for var %d (%s)!", indVar, pg_FullScenarioVarMessages[indVar].c_str()); ReportError(ErrorStr); throw 100;
			}
		}
		// runs the callBack on the updated variables which have one
		if (array_index < 0) {
			for (int index = pg_FullScenarioVarIndiceRanges[indVar][0]; index < pg_FullScenarioVarIndiceRanges[indVar][1]; index++) {
				// runs the callBack on the updated variables which have one
				if (pg_FullScenarioActiveVars[pg_current_configuration_rank][indVar] && pg_FullScenarioArrayVarCallbacks[indVar]) {
					//printf("Run callback of var \"%s\"\n", pg_FullScenarioVarMessages[indVar].c_str());
					(*pg_FullScenarioArrayVarCallbacks[indVar])(pg_variable_param_input_type[indVar], scenario_or_gui_command_value, index);
				}
			}
		}
		else if (array_index < pg_FullScenarioVarIndiceRanges[indVar][1]) {
			//printf("changed value %.2f index %d\n", scenario_or_gui_command_value.val_array[array_index], array_index);
			// runs the callBack on the updated variables which have one
			if (pg_FullScenarioActiveVars[pg_current_configuration_rank][indVar] && pg_FullScenarioArrayVarCallbacks[indVar]) {
				//printf("Run callback of var \"%s\"\n", pg_FullScenarioVarMessages[indVar].c_str());
				(*pg_FullScenarioArrayVarCallbacks[indVar])(pg_variable_param_input_type[indVar], scenario_or_gui_command_value, array_index);
			}
		}
	}
}

/// <summary>
/// updates the position of the cursor in the Keystone pad for texture keystone distortion
/// </summary>
/// <param name=""></param>
void updateXYKeystonePad(void) {
	// Keystone interface
	sprintf(AuxString, "/TopLeft %.2f %.2f", -VP1KeystoneYTopLeft, VP1KeystoneXTopLeft);
	pg_send_message_udp((char*)"ff", AuxString, (char*)"udp_TouchOSC_send");
	sprintf(AuxString, "/BottomLeft %.2f %.2f", -float(double(VP1KeystoneYBottomLeft) - 1.f), VP1KeystoneXBottomLeft);
	pg_send_message_udp((char*)"ff", AuxString, (char*)"udp_TouchOSC_send");
	sprintf(AuxString, "/TopRight %.2f %.2f", -VP1KeystoneYTopRight, float(double(VP1KeystoneXTopRight) - 1.f));
	pg_send_message_udp((char*)"ff", AuxString, (char*)"udp_TouchOSC_send");
	sprintf(AuxString, "/BottomRight %.2f %.2f", -float(double(VP1KeystoneYBottomRight) - 1.f), float(double(VP1KeystoneXBottomRight) - 1.f));
	pg_send_message_udp((char*)"ff", AuxString, (char*)"udp_TouchOSC_send");
}

////////////////////////////////////////////////////////////////////////////////////
// BEGINNING OF A NEW SCENE: INITIALIZATION OF TIMES AND DURATIONS, AND VARIABLE VALUES

// saves the current values of the parameters in case of k (copy) interpolation mode
// or in case of interpolation between the current values and the initial values of the current scene
void pg_keep_value_copy(int indVar, ScenarioValue* parameter_value) {
	if (pg_FullScenarioVarIndiceRanges[indVar][0] == -1) {
		if (pg_FullScenarioVarTypes[indVar] == _pg_float) {
			parameter_value[indVar].val_num = double(*((float*)pg_FullScenarioVarPointers[indVar]));
		}
		else if (pg_FullScenarioVarTypes[indVar] == _pg_int) {
			parameter_value[indVar].val_num = double(*((int*)pg_FullScenarioVarPointers[indVar]));
		}
		else if (pg_FullScenarioVarTypes[indVar] == _pg_bool) {
			if (*((bool*)pg_FullScenarioVarPointers[indVar]) != false) {
				parameter_value[indVar].val_num = 1;
			}
			else {
				parameter_value[indVar].val_num = 0;
			}
		}
		else if (pg_FullScenarioVarTypes[indVar] == _pg_sign) {
			parameter_value[indVar].val_num = double(*((int*)pg_FullScenarioVarPointers[indVar]));
		}
		else if (pg_FullScenarioVarTypes[indVar] == _pg_path) {
			if (*((bool*)pg_FullScenarioVarPointers[indVar]) != false) {
				parameter_value[indVar].val_num = 1;
			}
			else {
				parameter_value[indVar].val_num = 0;
			}
		}
		else if (pg_FullScenarioVarTypes[indVar] == _pg_string) {
			parameter_value[indVar].val_string = *((string*)pg_FullScenarioVarPointers[indVar]);
		}
	}
	// array
	else {
		for (int index = pg_FullScenarioVarIndiceRanges[indVar][0]; index < pg_FullScenarioVarIndiceRanges[indVar][1]; index++) {
			if (pg_FullScenarioVarTypes[indVar] == _pg_float) {
				parameter_value[indVar].val_array[index] = double(((float*)pg_FullScenarioVarPointers[indVar])[index]);
			}
			else if (pg_FullScenarioVarTypes[indVar] == _pg_int) {
				parameter_value[indVar].val_array[index] = double(((int*)pg_FullScenarioVarPointers[indVar])[index]);
			}
			else if (pg_FullScenarioVarTypes[indVar] == _pg_bool) {
				if (((bool*)pg_FullScenarioVarPointers[indVar])[index] != false) {
					parameter_value[indVar].val_array[index] = 1;
				}
				else {
					parameter_value[indVar].val_array[index] = 0;
				}
			}
			else if (pg_FullScenarioVarTypes[indVar] == _pg_sign) {
				parameter_value[indVar].val_array[index] = double(((int*)pg_FullScenarioVarPointers[indVar])[index]);
			}
			else if (pg_FullScenarioVarTypes[indVar] == _pg_path) {
				if (((bool*)pg_FullScenarioVarPointers[indVar])[index] != false) {
					parameter_value[indVar].val_array[index] = 1;
				}
				else {
					parameter_value[indVar].val_array[index] = 0;
				}
			}
		}
	}
}

// begins a new configuration and selects the initial variables in this scene
void StartNewConfiguration(int config_no) {
	if (config_no >= pg_NbConfigurations || config_no < 0) {
		return;
	}
	pg_current_configuration_rank = config_no;

	pg_launch_performance(0);
	StartNewScene(0, 0);
}


// begins a new scene and updates scene variables
void StartNewScene(int ind_scene, double delta_time) {
	pg_last_scene_update = false;

	/////////////////////////////////////////////////
	// No interpolation: starts immediately a new scene
	if (pg_SceneInterpolationDuration <= 0 && ind_scene >= 0 && ind_scene < int(pg_Scenario[pg_current_configuration_rank].size())) {
		pg_SceneIndexAfterInterpolation = -1;

		restoreInitialTimesAndDurations();

		// we place the beginning of the current scene at this time
		pg_Scenario[pg_current_configuration_rank][ind_scene].scene_initial_time -= delta_time;
		pg_Scenario[pg_current_configuration_rank][ind_scene].scene_duration += delta_time;
		if (ind_scene > 0) {
			pg_Scenario[pg_current_configuration_rank][ind_scene - 1].scene_final_time -= delta_time;
			pg_Scenario[pg_current_configuration_rank][ind_scene - 1].scene_duration -= delta_time;
		}
		// unuseful because is already made through pg_Scenario[pg_current_configuration_rank][ind_scene].scene_initial_time -= delta_time;
		//InitialScenarioTime = pg_CurrentClockTime - pg_Scenario[pg_current_configuration_rank][ind_scene].scene_initial_time;

		InitialScenarioTime = pg_CurrentClockTime - pg_Scenario[pg_current_configuration_rank][ind_scene].scene_initial_time;
		if (ind_scene == 0) {
			// restarts scenarios
			AbsoluteInitialScenarioTime = pg_CurrentClockTime - pg_Scenario[pg_current_configuration_rank][0].scene_initial_time;
		}

		// UPDATES THE SCENE POINTERS AND INDICES
		pg_CurrentSceneIndex = ind_scene;
		pg_CurrentScene = &pg_Scenario[pg_current_configuration_rank][pg_CurrentSceneIndex];
		pg_SceneIndexBeforeInterpolation = -1;
		pg_SceneIndexAfterInterpolation = -1;

#if defined(pg_Project_SilentDrawing)
		// sends the new scene to recording for later replay
		sprintf(AuxString, "/new_scene %d", ind_scene);
		pg_send_message_udp((char*)"i", (char*)AuxString, (char*)"udp_Record_send");
#endif

		if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_moving_messages]) {
			if (pg_CurrentSceneIndex == 0 || pg_CurrentSceneIndex == 5) {
				pg_Ind_Current_DisplayText = 0;
			}
		}
	}
	/////////////////////////////////////////////////
	// SCENE INTERPOLATION: linear interpolation of current variable values to a new scene 
	// variables all keep their current values and interpolate towards the values of the variables in the target scene
	else {
		//printf("Start interpolation scene\n");
		// UPDATES THE SCENE POINTERS AND INDICES
		pg_CurrentSceneIndex = -1;
		pg_CurrentScene = &pg_InterpolationScene;
		pg_SceneIndexBeforeInterpolation = pg_CurrentSceneIndex;
		pg_SceneIndexAfterInterpolation = ind_scene;

		//to be completed
		if (pg_CurrentScene) {
			pg_CurrentScene->scene_IDs = "INTERPOLATION";
			pg_CurrentScene->scene_duration = pg_SceneInterpolationDuration;
			pg_CurrentScene->scene_change_when_ends = true;
			pg_CurrentScene->scene_initial_time = pg_CurrentClockTime;
			pg_CurrentScene->scene_final_time = pg_CurrentScene->scene_initial_time + pg_SceneInterpolationDuration;
			pg_CurrentScene->scene_originalDuration = pg_SceneInterpolationDuration;
			pg_CurrentScene->scene_originalInitial_time = pg_CurrentClockTime;
			pg_CurrentScene->scene_originalFinal_time = pg_CurrentScene->scene_initial_time + pg_SceneInterpolationDuration;

			for (int indP = 0; indP < ScenarioVarNb[pg_current_configuration_rank]; indP++) {
				int indVar = ConfigScenarioVarRank[pg_current_configuration_rank][indP];
				if (pg_FullScenarioActiveVars[pg_current_configuration_rank][indVar]) {
					// copies the current value in the initial value
					pg_keep_value_copy(indVar, pg_CurrentScene->scene_initial_parameters);
					// if the next interpolation keeps the value, copies the current value in the final value
					// otherwise copies the initial value of the target scene as final value of the interpolaiton scene
					if (pg_Scenario[pg_current_configuration_rank][pg_SceneIndexAfterInterpolation].scene_interpolations[indVar].interpolation_mode != pg_enum_keep_value) {
						pg_CurrentScene->scene_final_parameters[indVar] = pg_Scenario[pg_current_configuration_rank][pg_SceneIndexAfterInterpolation].scene_initial_parameters[indVar]; // next scene initial value
					}
					else {
						pg_keep_value_copy(indVar, pg_CurrentScene->scene_final_parameters);
					}
				}
			}
			for (int indP = 0; indP < ScenarioVarNb[pg_current_configuration_rank]; indP++) {
				int indVar = ConfigScenarioVarRank[pg_current_configuration_rank][indP];
				if (pg_FullScenarioActiveVars[pg_current_configuration_rank][indVar]) {
					pg_CurrentScene->scene_interpolations[indVar].interpolation_mode = pg_enum_linear_interpolation;
					pg_CurrentScene->scene_interpolations[indVar].offSet = 0.0;
					pg_CurrentScene->scene_interpolations[indVar].duration = 1.0;
				}
			}
			//printf("Master values cur %.2f ini %.2f fin %.2f\n", *((float*)pg_FullScenarioVarPointers[_master]), 
			//	pg_CurrentScene->scene_final_parameters[_master], pg_CurrentScene->scene_final_parameters[_master]);
			// resets interpolation time so that the interpolation does not restart indefinitely
			pg_SceneInterpolationDuration = 0.f;

			sprintf(AuxString, "/interpolation_duration %.2f", pg_SceneInterpolationDuration);
			pg_send_message_udp((char*)"i", AuxString, (char*)"udp_TouchOSC_send");
		}
	}

	if (pg_CurrentScene) {
		pg_FirstFrameInScene = true;
		sprintf(AuxString, "/setupNo %d", pg_CurrentSceneIndex); pg_send_message_udp((char*)"i", AuxString, (char*)"udp_TouchOSC_send");
		sprintf(AuxString, "/setup %s", pg_CurrentScene->scene_IDs.c_str()); pg_send_message_udp((char*)"s", AuxString, (char*)"udp_TouchOSC_send");
		sprintf(AuxString, "/setup_1 %s", pg_CurrentScene->scene_Msg1.c_str()); pg_send_message_udp((char*)"s", AuxString, (char*)"udp_TouchOSC_send");
		sprintf(AuxString, "/setup_2 %s", pg_CurrentScene->scene_Msg2.c_str()); pg_send_message_udp((char*)"s", AuxString, (char*)"udp_TouchOSC_send");
		string next_string = "";
		if (pg_CurrentScene->scene_change_when_ends == true) {
			next_string = "next_autom:";
		}
		else {
			next_string = "next_manual:";
		}
		if (pg_CurrentSceneIndex < int(pg_Scenario[pg_current_configuration_rank].size()) - 1) {
			sprintf(AuxString, "/setup_next %s_%s", next_string.c_str(), pg_Scenario[pg_current_configuration_rank][pg_CurrentSceneIndex + 1].scene_IDs.c_str()); pg_send_message_udp((char*)"s", AuxString, (char*)"udp_TouchOSC_send");
		}
		else {
			sprintf(AuxString, "/setup_next %s_%s", next_string.c_str(), (char*)"END"); pg_send_message_udp((char*)"s", AuxString, (char*)"udp_TouchOSC_send");
		}
	}

	if (pg_CurrentScene) {
		std::cout << "Scene: " << pg_CurrentScene->scene_IDs << std::endl;
		// fprintf(pg_csv_log_file, "Scene:¨%d %s\n", pg_CurrentSceneIndex, pg_CurrentScene->scene_IDs.c_str());
		// reinitialization of the interpolation control variables at the beginning of a new scene
		for (int indP = 0; indP < ScenarioVarNb[pg_current_configuration_rank]; indP++) {
			int indVar = ConfigScenarioVarRank[pg_current_configuration_rank][indP];
			if (pg_FullScenarioActiveVars[pg_current_configuration_rank][indVar]) {
				BrokenInterpolationVar[indVar] = false;
				StepwiseInterpolationEffective[indVar] = false;
				if (pg_CurrentScene->scene_interpolations[indVar].interpolation_mode == pg_enum_keep_value
					|| pg_CurrentScene->scene_interpolations[indVar].initialization_mode == pg_current_value) {
					pg_keep_value_copy(indVar, pg_CurrentScene->scene_initial_parameters);
				}
			}
		}
	}
	// stops ongoing flashes if there is one
	flashCameraTrk_weight = 0.0f;
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_flashPhotoTrkBeat]
		&& pg_FullScenarioActiveVars[pg_current_configuration_rank][_flashPhotoTrkBright]
		&& pg_FullScenarioActiveVars[pg_current_configuration_rank][_flashPhotoTrkLength]
		&& pg_FullScenarioActiveVars[pg_current_configuration_rank][_flashPhotoChangeBeat]) {
		flashPhotoTrk_weight = 0.0f;
		flashPhotoTrk_nbFrames = 0;
	}
}


////////////////////////////////////////////////////////////////////////////////////
// UPDATE OF A CURRENT SCENE: UPDATE OF VARIABLE VALUES ACCORDING TO INTERPOLATIONS

void pg_update_scene_variables(Scene* cur_scene, double elapsed_time_from_start) {
	// transformed interpolation according to the interpolation function
	for (int indP = 0; indP < ScenarioVarNb[pg_current_configuration_rank]; indP++) {
		int indVar = ConfigScenarioVarRank[pg_current_configuration_rank][indP];
		//printf("Scene %d indP %d indVar %d\n", pg_CurrentSceneIndex, indP, indVar);
		//if (indVar == _auto_beat) {
		//	printf("Scene %d belongs to config %d\n",
		//		pg_CurrentSceneIndex, pg_FullScenarioActiveVars[pg_current_configuration_rank][indVar]);
		//}
		if (pg_FullScenarioActiveVars[pg_current_configuration_rank][indVar]) {
			//if (indVar == _cameraNo) {
			//	printf("Scene %d interpolation %d pg_enum_keep_value %d\n",
			//		pg_CurrentSceneIndex, cur_scene->scene_interpolations[indVar].interpolation_mode, pg_enum_keep_value);
			//}
			if (cur_scene->scene_interpolations[indVar].interpolation_mode == pg_enum_keep_value) {
				continue;
			}
			// going from interpolation percentages to interpolation times by multiplying by scene duration
			double absoluteOffset = cur_scene->scene_interpolations[indVar].offSet
				* cur_scene->scene_duration;
			double absoluteDuration = cur_scene->scene_interpolations[indVar].duration
				* cur_scene->scene_duration;

			double transformedInterpolation = 0.0f;
			double coefInterpolation = 0.0f;
			double twoStepInterpolation[3] = { 0.0f , 0.0f , 0.0f };

			if (elapsed_time_from_start <= cur_scene->scene_initial_time + absoluteOffset) {
				transformedInterpolation = 0.0;
				twoStepInterpolation[0] = 1.0;
				twoStepInterpolation[1] = 0.0;
				twoStepInterpolation[2] = 0.0;
			}
			else if (elapsed_time_from_start >= cur_scene->scene_initial_time + absoluteOffset + absoluteDuration) {
				transformedInterpolation = 1.0;
				twoStepInterpolation[0] = 0.0;
				twoStepInterpolation[1] = 0.0;
				twoStepInterpolation[2] = 1.0;
			}
			else {
				coefInterpolation
					= elapsed_time_from_start - (cur_scene->scene_initial_time + absoluteOffset);
				// time normalization for interpolation
				if (absoluteDuration > 0.0) {
					coefInterpolation /= absoluteDuration;
				}
				if (coefInterpolation < 0.0) {
					coefInterpolation = 0.0;
				}
				if (coefInterpolation > 1.0) {
					coefInterpolation = 1.0;
				}

				switch (cur_scene->scene_interpolations[indVar].interpolation_mode) {
				case pg_enum_linear_interpolation:
					transformedInterpolation = coefInterpolation;
					break;
				case pg_enum_cosine_interpolation:
					transformedInterpolation
						= 0.5F * (cos((coefInterpolation - 1.0F) * (float)M_PI) + 1.0F);
					break;
				case pg_enum_bezier_interpolation:
					// approximation of a bezier with cosine (more slow at beginning and end)
					transformedInterpolation
						= 0.5F * (cos((3 * pow(fabs(coefInterpolation), 2) - 2 * pow(fabs(coefInterpolation), 3) - 1.0F) * (float)M_PI) + 1.0F);
					break;
				case pg_enum_exponential_interpolation:
					// approximation of a bezier with cosine (more slow at beginning and end)
					transformedInterpolation
						= pow(fabs(coefInterpolation), cur_scene->scene_interpolations[indVar].exponent);
					break;
				case pg_enum_bell_interpolation:
					transformedInterpolation = coefInterpolation;
					if (coefInterpolation < 0.5f) {
						twoStepInterpolation[0] = 0.5F * (cos(2.0f * transformedInterpolation * (double)M_PI) + 1.0F);
						twoStepInterpolation[1] = 1.0f - twoStepInterpolation[0];
						twoStepInterpolation[2] = 0.0f;
					}
					else {
						twoStepInterpolation[0] = 0.0f;
						twoStepInterpolation[1] = 1.0f - 0.5F * (cos(2.0f * transformedInterpolation * (double)M_PI) + 1.0F);
						twoStepInterpolation[2] = 1.0f - twoStepInterpolation[1];
					}
					//printf("Scene %s param %d time from start %.2f interp %.2f transf_interp %.2f 2-step interp  %.2f %.2f %.2f start interp %.2f duration %.2f\n",
					   //cur_scene->scene_IDs.c_str(), indVar, elapsed_time_from_start, coefInterpolation, transformedInterpolation, twoStepInterpolation[0], twoStepInterpolation[1], twoStepInterpolation[2], absoluteOffset, absoluteDuration);
					break;
				case pg_enum_sawtooth_interpolation:
					transformedInterpolation = coefInterpolation;
					if (coefInterpolation < 0.5f) {
						twoStepInterpolation[0] = 1.0f - 2.0f * transformedInterpolation;
						twoStepInterpolation[1] = 1.0f - twoStepInterpolation[0];
						twoStepInterpolation[2] = 0.0f;
					}
					else {
						twoStepInterpolation[0] = 0.0f;
						twoStepInterpolation[1] = 2.0f - 2.0f * transformedInterpolation;;
						twoStepInterpolation[2] = 1.0f - twoStepInterpolation[1];
					}
					break;
				case pg_enum_stepwise_interpolation:
					// printf("Stepwise interpolations have no intermediate values\n");
					// no offset keeps initial value during the whole scene
					if (cur_scene->scene_interpolations[indVar].offSet == 0) {
						transformedInterpolation = 0.f;
					}
					else {
						if (coefInterpolation <= 0.f) {
							transformedInterpolation = 0.f;
						}
						else {
							transformedInterpolation = 1.f;
						}
					}
					break;
				default:
					printf("Unknown interpolation mode!\n");
					break;
				}
			}

			//if (indVar == _cameraNo) {
			//	printf("Scene %d transformedInterpolation %.2f pg_FirstFrameInScene %d\n",
			//		pg_CurrentSceneIndex, transformedInterpolation, pg_FirstFrameInScene);
			//}

			if ((transformedInterpolation == 0.0 && pg_FirstFrameInScene)
				|| transformedInterpolation > 0.0) {
				// stepwise interpolation
				if (cur_scene->scene_interpolations[indVar].interpolation_mode == pg_enum_stepwise_interpolation) {
					//if (indVar == _cameraNo) {
					//	 printf( "Scene %d param %d time from start %.2f transf_interp %.2f value %.2f start interp %.2f end interp %.2f duration %.2f\n" , 
					//		 pg_CurrentSceneIndex, indVar, elapsed_time_from_start , transformedInterpolation , interpolated_value ,
					//		 cur_scene->scene_initial_parameters[indVar].val_num, cur_scene->scene_final_parameters[indVar].val_num,
					//		 cur_scene->scene_duration);
					//}
					// delayed stepwise change, made only once
					if (transformedInterpolation > 0.0
						&& StepwiseInterpolationEffective[indVar] == false) {
						pg_update_variable(_PG_SCENARIO, indVar, cur_scene->scene_final_parameters[indVar], -1);
						StepwiseInterpolationEffective[indVar] = true;
					}
					// initial stepwise value
					else if ((transformedInterpolation == 0.0 && pg_FirstFrameInScene)) {
						pg_update_variable(_PG_SCENARIO, indVar, cur_scene->scene_initial_parameters[indVar], -1);
						// set to false in case of offSet with later update
						StepwiseInterpolationEffective[indVar] = false;
					}
				}
				// continuous interpolation
				else if (cur_scene->scene_interpolations[indVar].interpolation_mode != pg_enum_bell_interpolation
					&& cur_scene->scene_interpolations[indVar].interpolation_mode != pg_enum_sawtooth_interpolation) {
					if (pg_FullScenarioVarTypes[indVar] != _pg_string) {
						// make sure that variables are interpolated to their final value when they reach the target value
						if (transformedInterpolation >= 1.f) {
							//if (indVar == _part_path_follow_0) {
							//	printf("Final value\n");
							//}				// calculation of the interpolated value and assignment to variable
							if (pg_FullScenarioVarIndiceRanges[indVar][0] == -1) {
								double interpolated_value = cur_scene->scene_final_parameters[indVar].val_num;
								ScenarioValue interpolated_scene_value(interpolated_value, "", NULL, 0);
								pg_update_variable(_PG_SCENARIO, indVar, interpolated_scene_value, -1);

							}
							else {
								double* valArray = new double[pg_FullScenarioVarIndiceRanges[indVar][1]];
								for (int index = pg_FullScenarioVarIndiceRanges[indVar][0]; index < pg_FullScenarioVarIndiceRanges[indVar][1]; index++) {
									valArray[index] = cur_scene->scene_final_parameters[indVar].val_array[index];
								}
								ScenarioValue interpolated_scene_value(0.f, "", valArray, pg_FullScenarioVarIndiceRanges[indVar][1]);
								pg_update_variable(_PG_SCENARIO, indVar, interpolated_scene_value, -1);
								delete(valArray);
							}
						}
						else {
							//if (indVar == _part_path_follow_0) {
							//	printf("Interpolated value\n");
							//}
							if (pg_FullScenarioVarIndiceRanges[indVar][0] == -1) {
								double interpolated_value
									= (1.0F - transformedInterpolation)
									* cur_scene->scene_initial_parameters[indVar].val_num
									+ transformedInterpolation
									* cur_scene->scene_final_parameters[indVar].val_num;
								ScenarioValue interpolated_scene_value(interpolated_value, "", NULL, 0);
								pg_update_variable(_PG_SCENARIO, indVar, interpolated_scene_value, -1);
							}
							else {
								double* valArray = new double[pg_FullScenarioVarIndiceRanges[indVar][1]];
								for (int index = pg_FullScenarioVarIndiceRanges[indVar][0]; index < pg_FullScenarioVarIndiceRanges[indVar][1]; index++) {
									valArray[index]
										= (1.0F - transformedInterpolation)
										* cur_scene->scene_initial_parameters[indVar].val_array[index]
										+ transformedInterpolation
										* cur_scene->scene_final_parameters[indVar].val_array[index];
								}
								ScenarioValue interpolated_scene_value(0.f, "", valArray, pg_FullScenarioVarIndiceRanges[indVar][1]);
								pg_update_variable(_PG_SCENARIO, indVar, interpolated_scene_value, -1);
								delete(valArray);
							}
						}

						//if (indVar == _part_path_follow_0) {
						//	printf("Scene %s param %d time from start %.2f interp %.2f transf_interp %.2f value %.2f\n", 
						//		cur_scene->scene_IDs.c_str(), indVar, elapsed_time_from_start, coefInterpolation, transformedInterpolation, interpolated_value);
						//}

						//if (indVar == _part_path_follow_0) {
						//	printf("val ini fin %.2f %.2f alpha %.2f value %.2f val %d/%d\n",
						//		cur_scene->scene_initial_parameters[indVar].val_num,
						//		cur_scene->scene_final_parameters[indVar].val_num, transformedInterpolation, interpolated_value, part_path_follow_0, *((bool*)pg_FullScenarioVarPointers[_part_path_follow_0]));
						//}
					}
					else {
						if (transformedInterpolation <= 0.5) {
							pg_update_variable(_PG_SCENARIO, indVar, cur_scene->scene_initial_parameters[indVar], -1);
						}
						else {
							pg_update_variable(_PG_SCENARIO, indVar, cur_scene->scene_final_parameters[indVar], -1);
						}
					}
				}
				// 3-valued bell interpolation between initial, median and end value
				else {
					if (pg_FullScenarioVarTypes[indVar] != _pg_string) {
						if (pg_FullScenarioVarIndiceRanges[indVar][0] == -1) {
							double interpolated_value
								= twoStepInterpolation[0]
								* cur_scene->scene_initial_parameters[indVar].val_num
								+ twoStepInterpolation[1]
								* cur_scene->scene_interpolations[indVar].midTermValue
								+ twoStepInterpolation[2]
								* cur_scene->scene_final_parameters[indVar].val_num;
							ScenarioValue interpolated_scene_value(interpolated_value, "", NULL, 0);
							pg_update_variable(_PG_SCENARIO, indVar, interpolated_scene_value, -1);
						}
						else {
							double* valArray = new double[pg_FullScenarioVarIndiceRanges[indVar][1]];
							for (int index = pg_FullScenarioVarIndiceRanges[indVar][0]; index < pg_FullScenarioVarIndiceRanges[indVar][1]; index++) {
								valArray[index]
									= twoStepInterpolation[0]
									* cur_scene->scene_initial_parameters[indVar].val_array[index]
									+ twoStepInterpolation[1]
									* cur_scene->scene_interpolations[indVar].midTermValueArray[index]
									+ twoStepInterpolation[2]
									* cur_scene->scene_final_parameters[indVar].val_array[index];
							}
							ScenarioValue interpolated_scene_value(0.f, "", valArray, pg_FullScenarioVarIndiceRanges[indVar][1]);
							pg_update_variable(_PG_SCENARIO, indVar, interpolated_scene_value, -1);
							delete(valArray);
						}

						//printf("Scene %s param %d time from start %.2f transf_interp %.2f 2-step interp  %.2f %.2f %.2f interpolated value %.2f\n",
						   //cur_scene->scene_IDs.c_str(), indVar, elapsed_time_from_start, transformedInterpolation, twoStepInterpolation[0], twoStepInterpolation[1], twoStepInterpolation[2], interpolated_value);
					}
					else {
						if (twoStepInterpolation[0] > 0) {
							pg_update_variable(_PG_SCENARIO, indVar, cur_scene->scene_initial_parameters[indVar], -1);
						}
						else {
							pg_update_variable(_PG_SCENARIO, indVar, cur_scene->scene_final_parameters[indVar], -1);
						}
					}
				}
			}
		}
	}
	// transformed interpolation according to the interpolation function
	pg_FirstFrameInScene = false;
}

///////////////////////////////////////////////////////////////////////////////////////
// SCENARIO UPDATE: SELECTION OF THE CURRENT SCENE AND VARIABLE UPDATE

void pg_update_scenario(void) {
	// printf("VideoPb Update scenario \n");

	// the current scene is not the interpolation scene between two scenes
	if (pg_CurrentScene) {
		if (pg_CurrentScene != &pg_InterpolationScene) {
			double elapsed_time_from_start = pg_CurrentClockTime - InitialScenarioTime;

			// scans all scenes to find the current one and applies the corresponding interpolations
			for (int ind_scene = 0; ind_scene < int(pg_Scenario[pg_current_configuration_rank].size()); ind_scene++) {
				//printf( "time %.2f beg %.2f end %.2f\n" , elapsed_time_from_start ,  pg_Scenario[pg_current_configuration_rank][ind_scene].scene_initial_time ,  pg_Scenario[pg_current_configuration_rank][ind_scene].scene_final_time );
			   // current scene found
				if ((elapsed_time_from_start >= pg_Scenario[pg_current_configuration_rank][ind_scene].scene_initial_time
					&& elapsed_time_from_start < pg_Scenario[pg_current_configuration_rank][ind_scene].scene_final_time)
					|| (pg_CurrentSceneIndex == pg_Scenario[pg_current_configuration_rank].size() - 1 && ind_scene == pg_Scenario[pg_current_configuration_rank].size() - 1
						&& elapsed_time_from_start > pg_Scenario[pg_current_configuration_rank][pg_Scenario[pg_current_configuration_rank].size() - 1].scene_final_time)) {
					// loop at the end of the scenario
					if (pg_CurrentSceneIndex == pg_Scenario[pg_current_configuration_rank].size() - 1 && ind_scene == pg_Scenario[pg_current_configuration_rank].size() - 1
						&& elapsed_time_from_start > pg_Scenario[pg_current_configuration_rank][pg_Scenario[pg_current_configuration_rank].size() - 1].scene_final_time) {
						ind_scene = 0;
					}
					// the current scene is finished 
					if (ind_scene != pg_CurrentSceneIndex) {
						// a new scene is launched only if pg_CurrentScene->scene_change_when_ends
						if (pg_CurrentScene->scene_change_when_ends == true) {
							StartNewScene(ind_scene, 0);
							elapsed_time_from_start = pg_CurrentClockTime - InitialScenarioTime;
							remainingTimeInScene = pg_CurrentScene->scene_duration;
						}
						// otherwise the current scene is prolonged, no interpolation takes place 
						// the parameter values stay as they were 
						else {
							// for time display + colors when reaching the end of the scene
							remainingTimeInScene = pg_CurrentScene->scene_initial_time + pg_CurrentScene->scene_duration - elapsed_time_from_start;
							if (!pg_last_scene_update) {
								pg_last_scene_update = true;
							}
							current_scene_percent = 1.;
							if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_activeClipArts]) {
								sprintf(AuxString, "/percent %.5f", current_scene_percent);
								//printf("send %.5f\n", 1.);
								pg_send_message_udp((char*)"f", AuxString, (char*)"udp_script_1_send");
								//printf("send %s\n", AuxString);
							}
							return;
						}
					}

					// for time display + colors when reaching the end of the scene
					remainingTimeInScene = pg_Scenario[pg_current_configuration_rank][ind_scene].scene_initial_time + pg_Scenario[pg_current_configuration_rank][ind_scene].scene_duration - elapsed_time_from_start;
					//printf( "time %.2f remainingTimeInScene %.2f\n" , elapsed_time_from_start, remainingTimeInScene);

					pg_update_scene_variables(&pg_Scenario[pg_current_configuration_rank][ind_scene], elapsed_time_from_start);

					if (pg_Scenario[pg_current_configuration_rank][ind_scene].scene_duration != 0) {
						current_scene_percent = 1. - remainingTimeInScene / pg_Scenario[pg_current_configuration_rank][ind_scene].scene_duration;
						current_scene_percent = max(0., min(1., current_scene_percent));
					}
					else {
						current_scene_percent = 0.;
					}
					// feeds the current script #1 if one is active
					if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_activeClipArts]) {
						if (currently_runnings_script_1 != "NULL") {
							sprintf(AuxString, "/percent %.5f", current_scene_percent);
							//printf("send %.5f\n", percent);
							pg_send_message_udp((char*)"f", AuxString, (char*)"udp_script_1_send");
							//printf("send %s\n", AuxString);
						}
					}

					// fprintf(pg_csv_log_file, "frame %ld cursor size %d\n", pg_FrameNo, cursorSize);
					return;
				}
				// current scene found
			}
			// scans all scenes to find the current one and applies the corresponding interpolations
		}
		// the current scene is the interpolation scene
		else {
			// end of interpolation
			if (pg_CurrentClockTime > pg_CurrentScene->scene_final_time) {
				// starts the scene memorized for after the interpolation
				StartNewScene(pg_SceneIndexAfterInterpolation, 0);
				// for time display + colors when reaching the end of the scene
				double elapsed_time_from_start = pg_CurrentClockTime - InitialScenarioTime;
				remainingTimeInScene = pg_CurrentScene->scene_initial_time + pg_CurrentScene->scene_duration - elapsed_time_from_start;
				//printf( "time %.2f remainingTimeInScene %.2f\n" , elapsed_time_from_start , remainingTimeInScene);

				pg_update_scene_variables(pg_CurrentScene, elapsed_time_from_start);
			}
			// ongoing interpolation
			// elapsed time is counted from scene beginning
			else {
				// for time display + colors when reaching the end of the scene
				remainingTimeInScene = pg_CurrentScene->scene_initial_time + pg_CurrentScene->scene_duration - pg_CurrentClockTime;
				//printf( "remainingTimeInScene %.2f pg_CurrentClockTime %.2f\n" , remainingTimeInScene, pg_CurrentClockTime);

				pg_update_scene_variables(pg_CurrentScene, pg_CurrentClockTime);
			}
		}
	}
}


void pg_launch_performance(int ind_scene) {
	// sprintf(AuxString, "/message launching"); pg_send_message_udp((char *)"s", AuxString, (char *)"udp_TouchOSC_send");

	// restarts scenarios
	restoreInitialTimesAndDurations();
	InitialScenarioTime = pg_CurrentClockTime - pg_Scenario[pg_current_configuration_rank][0].scene_initial_time;
	AbsoluteInitialScenarioTime = pg_CurrentClockTime - pg_Scenario[pg_current_configuration_rank][0].scene_initial_time;

	// reinitialization of the interpolation control variables at the beginning of a new scene
	for (int indP = 0; indP < ScenarioVarNb[pg_current_configuration_rank]; indP++) {
		int indVar = ConfigScenarioVarRank[pg_current_configuration_rank][indP];
		if (pg_FullScenarioActiveVars[pg_current_configuration_rank][indVar]) {
			BrokenInterpolationVar[indVar] = false;
			StepwiseInterpolationEffective[indVar] = false;
		}
	}

	// removes snaptshots before launching
	// remove_files_in_dir(&snapshots_dir_path_name); // removes snapshots before pg_launch_performance

#if defined(var_GenerativeNights_planes)
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_GenerativeNights_planes]) {
		initCA = 1.2f;
		printf("initCA %.2f\n", initCA);
	}
#endif

	StartNewScene(ind_scene, 0);
}

void setup(int scene_ind) {
	if (scene_ind >= 0 && pg_CurrentSceneIndex < 0) {
		pg_launch_performance(scene_ind % pg_Scenario[pg_current_configuration_rank].size());
	}
	else {
		StartNewScene(scene_ind % pg_Scenario[pg_current_configuration_rank].size(), 0);
	}
}

void setup_plus(int incay) {
	// only one setup change per second to avoid repetition
	//printf("pg_CurrentSceneIndex %d\n", pg_CurrentSceneIndex);
	if (pg_CurrentClockTime - LastSetupChangeClockTime > 0.3f) {
		LastSetupChangeClockTime = pg_CurrentClockTime;
	}
	else {
		return;
	}

	if (pg_CurrentSceneIndex < 0) {
		//printf("pg_launch_performance\n");
		pg_launch_performance(0);
	}
	else {
		int new_scene = (incay + pg_CurrentSceneIndex) % pg_Scenario[pg_current_configuration_rank].size();
		//printf("new scene %d\n", incay + pg_CurrentSceneIndex);
		StartNewScene(new_scene, 0);
	}
}

void setup_minus(int decay) {
	int new_scene = ((pg_CurrentSceneIndex - decay + pg_Scenario[pg_current_configuration_rank].size()) % pg_Scenario[pg_current_configuration_rank].size());
	StartNewScene(new_scene, 0);
}

