/*! \file pg-scenarioDyn.h
 *
 *
 *     File pg-scenarioDyn.h
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

#ifndef PG_SCENARIO_DYN_H
#define PG_SCENARIO_DYN_H

//////////////////////////////////////////////////////////////////////////////////////////////////////
// EXPORTED VARIABLES
////////////////////////////////////////////////////////////////////////////////////////////////////// 

extern double                   pg_current_scene_percent;
extern bool					    pg_last_scene_update;

class Scene;
// interpolation scene between two discontinuous scenes so that there is no visual gap inbetween
extern Scene pg_InterpolationScene;
extern int pg_SceneIndexAfterInterpolation;
extern int pg_SceneIndexBeforeInterpolation;
extern float pg_SceneInterpolationDuration;

// Buttons and labels for ATELIERSENFANTS interface
extern vector<string> pg_FondButtonsPaths;
extern vector<string> pg_FondButtonLabelsPaths;
extern vector<int> pg_FondButtonValues;
// exclusive labels on/off
extern vector<string> pg_StylusvsRubberButtonsPaths;
extern vector<string> pg_StylusvsRubberButtonLabelsPaths;
extern vector<int> pg_StylusvsRubberButtonValues;

// scene management
extern double pg_remainingTimeInScene;

// type of variable, used for update
extern pg_Parameter_Input_Type* pg_variable_param_input_type;

// scenario launching time
extern double pg_InitialScenarioTime;
extern double pg_AbsoluteInitialScenarioTime;

//////////////////////////////////////////////////////////////////////////////////////////////////////
// EXPORTED FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////

// scenario variable type conversion
bool double_to_bool(double param);

// before first scene
void pg_launch_performance(int ind_scene);

// INITIALIZES ALL SCENARIO VARIABLES AND ASSIGNS THEM THE VALUES OF THE FIRST SCENARIO LINE
void pg_initializeScenearioVariables(void);

// scene shift management
void pg_scene(int scene_ind);
void pg_scene_minus(int decay);
void pg_scene_plus(int incay);
void pg_StartNewConfiguration(int config_no);
void pg_StartNewScene(int ind_scene, double delta_time);

// sends message to the interface for a correlation between the pen
// on the graphic tablet and the xy pad
void pg_updateXYKeystonePad(void);

// scenario based variable update
void pg_update_variable(pg_Parameter_Input_Type param_input_type, int indVar,
	ScenarioValue scenario_or_gui_command_value, int array_index);

// SCENARIO MAIN UPDATE: SELECTION OF THE CURRENT SCENE AND VARIABLE UPDATE
void pg_update_scenario(void);

// when launching performance or starting a new scene restores initial data
void pg_restoreInitialTimesAndDurations(void);


#endif