/*! \file pg-gui.cpp
 *
 *
 *     File pg-gui.cpp
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
 /* based on ENTEC source code */

#include "pg-light-all_include.h"

///////////////////////////////////////////////////////////////////////////////////
// GUI DISPLAY & LOG FILE LOGGING
/////////////////////////////////////////////////////////////
void pg_displaySceneVariables(void) {
	if (pg_resend_all_variables) {
		if (pg_CurrentScene && pg_CurrentSceneIndex >= 0 && pg_CurrentSceneIndex < int(pg_Scenario[pg_ind_scenario].size())) {
			string next_string = "";
			if (pg_CurrentScene->scene_change_when_ends == true) {
				next_string = "next_autom:";
			}
			else {
				next_string = "next_manual:";
			}
			sprintf(pg_AuxString, "/setupNo %d", pg_CurrentSceneIndex); pg_send_message_udp((char*)"i", pg_AuxString, (char*)"udp_TouchOSC_send");
			sprintf(pg_AuxString, "/setup %s", pg_CurrentScene->scene_IDs.c_str()); pg_send_message_udp((char*)"s", pg_AuxString, (char*)"udp_TouchOSC_send");
			sprintf(pg_AuxString, "/setup_1 %s", pg_CurrentScene->scene_Msg1.c_str()); pg_send_message_udp((char*)"s", pg_AuxString, (char*)"udp_TouchOSC_send");
			sprintf(pg_AuxString, "/setup_2 %s", pg_CurrentScene->scene_Msg2.c_str()); pg_send_message_udp((char*)"s", pg_AuxString, (char*)"udp_TouchOSC_send");
			if (pg_CurrentSceneIndex < int(pg_Scenario[pg_ind_scenario].size()) - 1) {
				sprintf(pg_AuxString, "/setup_next %s_%s", next_string.c_str(), pg_Scenario[pg_ind_scenario][pg_CurrentSceneIndex + 1].scene_IDs.c_str()); pg_send_message_udp((char*)"s", pg_AuxString, (char*)"udp_TouchOSC_send");
			}
			else {
				sprintf(pg_AuxString, "/setup_next %s_%s", next_string.c_str(), (char*)"END"); pg_send_message_udp((char*)"s", pg_AuxString, (char*)"udp_TouchOSC_send");
			}
		}
		else if (pg_CurrentScene == NULL) {
			sprintf(pg_AuxString, "/setupNo -1"); pg_send_message_udp((char*)"i", pg_AuxString, (char*)"udp_TouchOSC_send");
			sprintf(pg_AuxString, "/setup initial_setup"); pg_send_message_udp((char*)"s", pg_AuxString, (char*)"udp_TouchOSC_send");
			sprintf(pg_AuxString, "/setup_1 ***"); pg_send_message_udp((char*)"s", pg_AuxString, (char*)"udp_TouchOSC_send");
			sprintf(pg_AuxString, "/setup_2 ***"); pg_send_message_udp((char*)"s", pg_AuxString, (char*)"udp_TouchOSC_send");
			if (pg_Scenario[pg_ind_scenario].size() > 0) {
				sprintf(pg_AuxString, "/setup_next next_manual:_%s", pg_Scenario[pg_ind_scenario][0].scene_IDs.c_str()); pg_send_message_udp((char*)"s", pg_AuxString, (char*)"udp_TouchOSC_send");
			}
		}
		else if (pg_CurrentScene == &pg_InterpolationScene) {
			sprintf(pg_AuxString, "/setupNo -1"); pg_send_message_udp((char*)"i", pg_AuxString, (char*)"udp_TouchOSC_send");
			sprintf(pg_AuxString, "/setup interpolation scene"); pg_send_message_udp((char*)"s", pg_AuxString, (char*)"udp_TouchOSC_send");
			sprintf(pg_AuxString, "/setup_1 next_scene_%s", pg_Scenario[pg_ind_scenario][pg_SceneIndexAfterInterpolation].scene_IDs.c_str()); pg_send_message_udp((char*)"s", pg_AuxString, (char*)"udp_TouchOSC_send");
			sprintf(pg_AuxString, "/setup_2 previous_scene_%s", pg_Scenario[pg_ind_scenario][pg_SceneIndexBeforeInterpolation].scene_IDs.c_str()); pg_send_message_udp((char*)"s", pg_AuxString, (char*)"udp_TouchOSC_send");
		}
		sprintf(pg_AuxString, "/configurationNo %d", pg_ind_scenario); pg_send_message_udp((char*)"i", pg_AuxString, (char*)"udp_TouchOSC_send");
		sprintf(pg_AuxString, "/pen_colorPreset %d", pg_current_pen_colorPreset); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
		pg_InterfaceInitializations();

		// resends the sound variables to PD (as in the callback)
		sprintf(pg_AuxString, "/sound_env_min %.2f", float(*((float*)pg_FullScenarioVarPointers[_sound_env_min])));
		pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_PD_send");
		sprintf(pg_AuxString, "/sound_env_max %.2f", float(*((float*)pg_FullScenarioVarPointers[_sound_env_max])));
		pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_PD_send");

		if (pg_FullScenarioActiveVars[pg_ind_scenario][_partColor_mode]
			|| pg_FullScenarioActiveVars[pg_ind_scenario][_partStroke_mode]
			|| pg_FullScenarioActiveVars[pg_ind_scenario][_partExit_mode]) {
			for (int ind = 0; ind < 3; ind++) {
				if (ind == *((int*)pg_FullScenarioVarPointers[_partColor_mode])) {
					sprintf(pg_AuxString, "/partColor_mode_%d 1", ind); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
				}
				else {
					sprintf(pg_AuxString, "/partColor_mode_%d 0", ind); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
				}
				if (ind == *((int*)pg_FullScenarioVarPointers[_partStroke_mode])) {
					sprintf(pg_AuxString, "/partStroke_mode_%d 1", ind); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
				}
				else {
					sprintf(pg_AuxString, "/partStroke_mode_%d 0", ind); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
				}
				if (ind == *((int*)pg_FullScenarioVarPointers[_partExit_mode])) {
					sprintf(pg_AuxString, "/partExit_mode_%d 1", ind); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
				}
				else {
					sprintf(pg_AuxString, "/partExit_mode_%d 0", ind); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
				}
			}
		}

	}

	// Only for DMX
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_light_color]) {
		if (pg_resend_all_light_variables) {
			pg_lightGUI_initialization();
			pg_resend_all_light_variables = false;
		}
	}

	/////////////////////////////////////////////////////////////////////////
	// TIME CONTROLLED VARIABLES (NOT FORWARDED TO SHADERS)
	// time display
	// time exceeded in scene
	//sprintf(pg_AuxString, "/volumeDisplay %.2f", pulse_average);
	pg_send_message_udp((char*)"s", pg_AuxString, (char*)"udp_TouchOSC_send");

	/*
	if (pg_remainingTimeInScene >= 0) {
		sprintf(pg_AuxString, "/time %d:%d", (int)pg_remainingTimeInScene / 60, (int)pg_remainingTimeInScene % 60); pg_send_message_udp((char*)"s", pg_AuxString, (char*)"udp_TouchOSC_send");
		if (pg_remainingTimeInScene < 20) {
			sprintf(pg_AuxString, "/time_color_red 1"); pg_send_message_udp((char*)"i", pg_AuxString, (char*)"udp_TouchOSC_send");
			sprintf(pg_AuxString, "/time_color_orange 0"); pg_send_message_udp((char*)"i", pg_AuxString, (char*)"udp_TouchOSC_send");
			sprintf(pg_AuxString, "/time_color_green 0"); pg_send_message_udp((char*)"i", pg_AuxString, (char*)"udp_TouchOSC_send");
		}
		else if (pg_remainingTimeInScene < 20) {
			sprintf(pg_AuxString, "/time_color_red 0"); pg_send_message_udp((char*)"i", pg_AuxString, (char*)"udp_TouchOSC_send");
			sprintf(pg_AuxString, "/time_color_orange 1"); pg_send_message_udp((char*)"i", pg_AuxString, (char*)"udp_TouchOSC_send");
			sprintf(pg_AuxString, "/time_color_green 0"); pg_send_message_udp((char*)"i", pg_AuxString, (char*)"udp_TouchOSC_send");
		}
		else {
			sprintf(pg_AuxString, "/time_color_red 0"); pg_send_message_udp((char*)"i", pg_AuxString, (char*)"udp_TouchOSC_send");
			sprintf(pg_AuxString, "/time_color_orange 0"); pg_send_message_udp((char*)"i", pg_AuxString, (char*)"udp_TouchOSC_send");
			sprintf(pg_AuxString, "/time_color_green 1"); pg_send_message_udp((char*)"i", pg_AuxString, (char*)"udp_TouchOSC_send");
		}
	}
	// time exceeded in scene
	else {
		int remTime = (int)pg_remainingTimeInScene;
		if (remTime > -60) {
			sprintf(pg_AuxString, "/time %d:%d", remTime / 60, remTime % 60); pg_send_message_udp((char*)"s", pg_AuxString, (char*)"udp_TouchOSC_send");
		}
		else {
			sprintf(pg_AuxString, "/time %d:%d", remTime / 60, -(remTime % 60)); pg_send_message_udp((char*)"s", pg_AuxString, (char*)"udp_TouchOSC_send");
		}
		sprintf(pg_AuxString, "/time_color 3"); pg_send_message_udp((char*)"i", pg_AuxString, (char*)"udp_TouchOSC_send");
	}
	*/

	/////////////////////////////////////////////////////////////////////////
	// SCENARIO-CONTROLLED VARIABLES
	//int rank = max(pg_FrameNo, 1) % 10;
	//for (int indVar = rank * 45; indVar < min((rank + 1) * 45, int(_MaxInterpVarIDs)); indVar++) {
	for (int indP = 0; indP < pg_ScenarioVarNb[pg_ind_scenario]; indP++) {
		int indVar = pg_ConfigScenarioVarRank[pg_ind_scenario][indP];
		if (pg_FullScenarioActiveVars[pg_ind_scenario][indVar]) {
			// the variable has a registered GUI ID in the scenario
			// and its values has changed since it was sent to the GUI last time
			if (pg_FullScenarioVarMessages[indVar] != "") {
				if (pg_FullScenarioVarIndiceRanges[indVar][0] == -1) {
					if (pg_FullScenarioVarTypes[indVar] == _pg_float) {
						float new_value = float(*((float*)pg_FullScenarioVarPointers[indVar]));
						switch (ScenarioVarPulse[indVar]) {
						case _pg_pulsed_absolute:
							new_value *= (1.f + pulse_average * float(*((float*)pg_FullScenarioVarPointers[indVar + 1])));
							break;
						case _pg_pulsed_uniform:
							new_value += pulse_average * float(*((float*)pg_FullScenarioVarPointers[indVar + 1]));
							break;
						case _pg_pulsed_differential:
							new_value *= (pulse_average - pg_audio_pulse_average_prec) * float(*((float*)pg_FullScenarioVarPointers[indVar + 1]));
							break;
						default:
							break;

						}
						if (pg_LastGUIShippedValuesInterpVar[pg_ind_scenario][indVar].val_num != new_value || pg_resend_all_variables) {
							sprintf(pg_AuxString, "/%s %.4f", pg_FullScenarioVarMessages[indVar].c_str(), new_value); pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_TouchOSC_send");
							pg_LastGUIShippedValuesInterpVar[pg_ind_scenario][indVar].val_num = new_value;
							//if (indVar == _track_y_transl[1]) {
							//	printf("REPOST: track_y_transl[1] %s %.1f\n", pg_FullScenarioVarMessages[indVar].c_str(), new_value);
							//}
						}
					}
					else if (pg_FullScenarioVarTypes[indVar] == _pg_int) {
						float new_value = float(*((int*)pg_FullScenarioVarPointers[indVar]));
						switch (ScenarioVarPulse[indVar]) {
						case _pg_pulsed_absolute:
							new_value *= (1.f + pulse_average * float(*((float*)pg_FullScenarioVarPointers[indVar + 1])));
							break;
						case _pg_pulsed_uniform:
							new_value += pulse_average * float(*((float*)pg_FullScenarioVarPointers[indVar + 1]));
							break;
						case _pg_pulsed_differential:
							new_value *= (pulse_average - pg_audio_pulse_average_prec) * float(*((float*)pg_FullScenarioVarPointers[indVar + 1]));
							break;
						default:
							break;
						}
						if (pg_LastGUIShippedValuesInterpVar[pg_ind_scenario][indVar].val_num != float(int(new_value)) || pg_resend_all_variables) {
							sprintf(pg_AuxString, "/%s %d", pg_FullScenarioVarMessages[indVar].c_str(), int(new_value)); pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_TouchOSC_send");
							// memorizes the shipped value in order to resend it only when changed
							pg_LastGUIShippedValuesInterpVar[pg_ind_scenario][indVar].val_num = float(int(new_value));
						}
					}
					else if (pg_FullScenarioVarTypes[indVar] == _pg_bool
						&& (pg_LastGUIShippedValuesInterpVar[pg_ind_scenario][indVar].val_num != float(*((bool*)pg_FullScenarioVarPointers[indVar])) || pg_resend_all_variables)) {
						sprintf(pg_AuxString, "/%s %d", pg_FullScenarioVarMessages[indVar].c_str(), (int)(*((bool*)pg_FullScenarioVarPointers[indVar]))); pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_TouchOSC_send");
						pg_LastGUIShippedValuesInterpVar[pg_ind_scenario][indVar].val_num = float(*((bool*)pg_FullScenarioVarPointers[indVar]));
					}
					else if (pg_FullScenarioVarTypes[indVar] == _pg_sign
						&& (pg_LastGUIShippedValuesInterpVar[pg_ind_scenario][indVar].val_num != float(*((int*)pg_FullScenarioVarPointers[indVar])) || pg_resend_all_variables)) {
						sprintf(pg_AuxString, "/%s %d", pg_FullScenarioVarMessages[indVar].c_str(), *((int*)pg_FullScenarioVarPointers[indVar])); pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_TouchOSC_send");
						pg_LastGUIShippedValuesInterpVar[pg_ind_scenario][indVar].val_num = float(*((int*)pg_FullScenarioVarPointers[indVar]));
					}
					else if (pg_FullScenarioVarTypes[indVar] == _pg_path
						&& (pg_LastGUIShippedValuesInterpVar[pg_ind_scenario][indVar].val_num != float(*((bool*)pg_FullScenarioVarPointers[indVar])) || pg_resend_all_variables)) {
						sprintf(pg_AuxString, "/%s %d", pg_FullScenarioVarMessages[indVar].c_str(), (int)(*((bool*)pg_FullScenarioVarPointers[indVar]))); pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_TouchOSC_send");
						pg_LastGUIShippedValuesInterpVar[pg_ind_scenario][indVar].val_num = float(*((bool*)pg_FullScenarioVarPointers[indVar]));
					}
				}
				// array
				else {
					for (int index = pg_FullScenarioVarIndiceRanges[indVar][0]; index < pg_FullScenarioVarIndiceRanges[indVar][1]; index++) {
						if (pg_FullScenarioVarTypes[indVar] == _pg_float) {
							float new_value = float(((float*)pg_FullScenarioVarPointers[indVar])[index]);
							switch (ScenarioVarPulse[indVar]) {
							case _pg_pulsed_absolute:
								new_value *= (1.f + pulse_average * float(((float*)pg_FullScenarioVarPointers[indVar + 1])[index]));
								break;
							case _pg_pulsed_uniform:
								new_value += pulse_average * float(((float*)pg_FullScenarioVarPointers[indVar + 1])[index]);
								break;
							case _pg_pulsed_differential:
								new_value *= (pulse_average - pg_audio_pulse_average_prec) * float(((float*)pg_FullScenarioVarPointers[indVar + 1])[index]);
								break;
							default:
								break;

							}
							if (pg_LastGUIShippedValuesInterpVar[pg_ind_scenario][indVar].get_array_value(index) != new_value || pg_resend_all_variables) {
								std::regex array_command("([a-zA-Z0-9_]+[a-zA-Z])(_pulse)");
								std::smatch pieces_match;
								std::string var_mess = pg_FullScenarioVarMessages[indVar];
								if (std::regex_match(var_mess, pieces_match, array_command)) {
									sprintf(pg_AuxString, "/%s_%d%s %.4f", pieces_match[1].str().c_str(), index, pieces_match[2].str().c_str(), new_value); pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_TouchOSC_send");
								}
								else {
									sprintf(pg_AuxString, "/%s_%d %.4f", pg_FullScenarioVarMessages[indVar].c_str(), index, new_value); pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_TouchOSC_send");
								}
								pg_LastGUIShippedValuesInterpVar[pg_ind_scenario][indVar].set_array_value(index, new_value);
								//if (indVar == _track_y_transl[1]) {
								//	printf("REPOST: track_y_transl[1] %s %.1f\n", pg_FullScenarioVarMessages[indVar].c_str(), new_value);
								//}
							}
						}
						else if (pg_FullScenarioVarTypes[indVar] == _pg_int) {
							float new_value = float(((int*)pg_FullScenarioVarPointers[indVar])[index]);
							switch (ScenarioVarPulse[indVar]) {
							case _pg_pulsed_absolute:
								new_value *= (1.f + pulse_average * float(((float*)pg_FullScenarioVarPointers[indVar + 1])[index]));
								break;
							case _pg_pulsed_uniform:
								new_value += pulse_average * float(((float*)pg_FullScenarioVarPointers[indVar + 1])[index]);
								break;
							case _pg_pulsed_differential:
								new_value *= (pulse_average - pg_audio_pulse_average_prec) * float(((float*)pg_FullScenarioVarPointers[indVar + 1])[index]);
								break;
							default:
								break;
							}
							if (pg_LastGUIShippedValuesInterpVar[pg_ind_scenario][indVar].get_array_value(index) != float(int(new_value)) || pg_resend_all_variables) {
								std::regex array_command("([a-zA-Z0-9_]+[a-zA-Z])(_pulse)");
								std::smatch pieces_match;
								std::string var_mess = pg_FullScenarioVarMessages[indVar];
								if (std::regex_match(var_mess, pieces_match, array_command)) {
									sprintf(pg_AuxString, "/%s_%d%s %d", pieces_match[1].str().c_str(), index, pieces_match[2].str().c_str(), int(new_value)); pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_TouchOSC_send");
								}
								else {
									sprintf(pg_AuxString, "/%s_%d %d", pg_FullScenarioVarMessages[indVar].c_str(), index, int(new_value)); pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_TouchOSC_send");
								}
								// memorizes the shipped value in order to resend it only when changed
								pg_LastGUIShippedValuesInterpVar[pg_ind_scenario][indVar].set_array_value(index, float(int(new_value)));
							}
						}
						else if (pg_FullScenarioVarTypes[indVar] == _pg_bool
							&& (pg_LastGUIShippedValuesInterpVar[pg_ind_scenario][indVar].get_array_value(index) != float(((bool*)pg_FullScenarioVarPointers[indVar])[index]) || pg_resend_all_variables)) {
							std::regex array_command("([a-zA-Z0-9_]+[a-zA-Z])(_pulse)");
							std::smatch pieces_match;
							std::string var_mess = pg_FullScenarioVarMessages[indVar];
							if (std::regex_match(var_mess, pieces_match, array_command)) {
								sprintf(pg_AuxString, "/%s_%d%s %d", pieces_match[1].str().c_str(), index, pieces_match[2].str().c_str(), (int)(((bool*)pg_FullScenarioVarPointers[indVar])[index])); pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_TouchOSC_send");
							}
							else {
								sprintf(pg_AuxString, "/%s_%d %d", pg_FullScenarioVarMessages[indVar].c_str(), index, (int)(((bool*)pg_FullScenarioVarPointers[indVar])[index])); pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_TouchOSC_send");
							}
							pg_LastGUIShippedValuesInterpVar[pg_ind_scenario][indVar].set_array_value(index, float(((bool*)pg_FullScenarioVarPointers[indVar])[index]));
						}
						else if (pg_FullScenarioVarTypes[indVar] == _pg_sign
							&& (pg_LastGUIShippedValuesInterpVar[pg_ind_scenario][indVar].get_array_value(index) != float(((int*)pg_FullScenarioVarPointers[indVar])[index]) || pg_resend_all_variables)) {
							std::regex array_command("([a-zA-Z0-9_]+[a-zA-Z])(_pulse)");
							std::smatch pieces_match;
							std::string var_mess = pg_FullScenarioVarMessages[indVar];
							if (std::regex_match(var_mess, pieces_match, array_command)) {
								sprintf(pg_AuxString, "/%s_%d%s %d", pieces_match[1].str().c_str(), index, pieces_match[2].str().c_str(), ((int*)pg_FullScenarioVarPointers[indVar])[index]); pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_TouchOSC_send");
							}
							else {
								sprintf(pg_AuxString, "/%s_%d %d", pg_FullScenarioVarMessages[indVar].c_str(), index, ((int*)pg_FullScenarioVarPointers[indVar])[index]); pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_TouchOSC_send");
							}
							pg_LastGUIShippedValuesInterpVar[pg_ind_scenario][indVar].set_array_value(index, float(((int*)pg_FullScenarioVarPointers[indVar])[index]));
						}
						else if (pg_FullScenarioVarTypes[indVar] == _pg_path
							&& (pg_LastGUIShippedValuesInterpVar[pg_ind_scenario][indVar].get_array_value(index) != float(((bool*)pg_FullScenarioVarPointers[indVar])[index]) || pg_resend_all_variables)) {
							std::regex array_command("([a-zA-Z0-9_]+[a-zA-Z])(_pulse)");
							std::smatch pieces_match;
							std::string var_mess = pg_FullScenarioVarMessages[indVar];
							if (std::regex_match(var_mess, pieces_match, array_command)) {
								sprintf(pg_AuxString, "/%s_%d%s %d", pieces_match[1].str().c_str(), index, pieces_match[2].str().c_str(), (int)(((bool*)pg_FullScenarioVarPointers[indVar])[index])); pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_TouchOSC_send");
							}
							else {
								sprintf(pg_AuxString, "/%s_%d %d", pg_FullScenarioVarMessages[indVar].c_str(), index, (int)(((bool*)pg_FullScenarioVarPointers[indVar])[index])); pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_TouchOSC_send");
							}
							pg_LastGUIShippedValuesInterpVar[pg_ind_scenario][indVar].set_array_value(index, float(((bool*)pg_FullScenarioVarPointers[indVar])[index]));
						}
					}
				}
			}
		}
	}

	pg_resend_all_variables = false;
}

//////////////////////////////////////////////////////////////////////////////////////
// LOG FILE
// makes a line of a CSV file to be used as a starting point for a repetition of the
// preset

void pg_logCurrentLineSceneVariables(string fileName) {
	/////////////////////////////////////////////////////////////////////////
	// SCENARIO-CONTROLLED UNIFORM VARIABLES
	for (int indP = 0; indP < pg_ScenarioVarNb[pg_ind_scenario]; indP++) {
		int indVar = pg_ConfigScenarioVarRank[pg_ind_scenario][indP];
		if (pg_FullScenarioActiveVars[pg_ind_scenario][indVar]) {
			// dimension 1 variable: number
			if (pg_FullScenarioVarIndiceRanges[indVar][0] == -1) {
				if (pg_FullScenarioVarTypes[indVar] == _pg_float) {
					fprintf(pg_csv_log_file, ",%.6f", *((float*)pg_FullScenarioVarPointers[indVar]));
				}
				else if (pg_FullScenarioVarTypes[indVar] == _pg_int) {
					fprintf(pg_csv_log_file, ",%d", *((int*)pg_FullScenarioVarPointers[indVar]));
				}
				else if (pg_FullScenarioVarTypes[indVar] == _pg_bool) {
					fprintf(pg_csv_log_file, ",%d", int(*((bool*)pg_FullScenarioVarPointers[indVar])));
				}
				else if (pg_FullScenarioVarTypes[indVar] == _pg_sign) {
					fprintf(pg_csv_log_file, ",%d", (*((int*)pg_FullScenarioVarPointers[indVar]) > 0 ? +1 : (*((int*)pg_FullScenarioVarPointers[indVar]) < 0) ? -1 : 0));
				}
				else if (pg_FullScenarioVarTypes[indVar] == _pg_path) {
					fprintf(pg_csv_log_file, ",%d", int(*((bool*)pg_FullScenarioVarPointers[indVar])));
				}
				else if (pg_FullScenarioVarTypes[indVar] == _pg_string) {
					fprintf(pg_csv_log_file, ",%s", (*((string*)pg_FullScenarioVarPointers[indVar])).c_str());
				}
			}
			// array
			else {
				for (int index = pg_FullScenarioVarIndiceRanges[indVar][0]; index < pg_FullScenarioVarIndiceRanges[indVar][1]; index++) {
					// column separator
					if (index == pg_FullScenarioVarIndiceRanges[indVar][0]) {
						fprintf(pg_csv_log_file, ",");
					}
					// value
					if (pg_FullScenarioVarTypes[indVar] == _pg_float) {
						fprintf(pg_csv_log_file, "%.6f", ((float*)pg_FullScenarioVarPointers[indVar])[index]);
					}
					else if (pg_FullScenarioVarTypes[indVar] == _pg_int) {
						fprintf(pg_csv_log_file, "%d", ((int*)pg_FullScenarioVarPointers[indVar])[index]);
					}
					else if (pg_FullScenarioVarTypes[indVar] == _pg_bool) {
						fprintf(pg_csv_log_file, "%d", int(((bool*)pg_FullScenarioVarPointers[indVar])[index]));
					}
					else if (pg_FullScenarioVarTypes[indVar] == _pg_sign) {
						fprintf(pg_csv_log_file, "%d", (((int*)pg_FullScenarioVarPointers[indVar])[index] > 0 ? +1 : (((int*)pg_FullScenarioVarPointers[indVar])[index] < 0) ? -1 : 0));
					}
					else if (pg_FullScenarioVarTypes[indVar] == _pg_path) {
						fprintf(pg_csv_log_file, "%d", int(((bool*)pg_FullScenarioVarPointers[indVar])[index]));
					}
					else if (pg_FullScenarioVarTypes[indVar] == _pg_string) {
						fprintf(pg_csv_log_file, "%s", (((string*)pg_FullScenarioVarPointers[indVar])[index]).c_str());
					}
					// value separator
					if (index < pg_FullScenarioVarIndiceRanges[indVar][1] - 1) {
						fprintf(pg_csv_log_file, "/");
					}
				}
			}
		}
	}
	fprintf(pg_csv_log_file, ",%s\n" , fileName.c_str());
}



// makes the first line of a CSV log file
void pg_logFirstLineSceneVariables(void) {
	/////////////////////////////////////////////////////////////////////////
	// SCENARIO-CONTROLLED UNIFORM VARIABLES
	fprintf(pg_csv_log_file, "ID");
	for (int indP = 0; indP < pg_ScenarioVarNb[pg_ind_scenario]; indP++) {
		int indVar = pg_ConfigScenarioVarRank[pg_ind_scenario][indP];
		if (pg_FullScenarioActiveVars[pg_ind_scenario][indVar]) {
			fprintf(pg_csv_log_file, ",%s", pg_FullScenarioVarStrings[indVar].c_str());
		}
	}
	fprintf(pg_csv_log_file, "\n");
}

