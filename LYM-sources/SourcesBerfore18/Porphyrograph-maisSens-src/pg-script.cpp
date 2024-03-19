/*! \file pg-script.cpp
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

#include "pg_script_body.cpp"

std::string project_name = "maisSens";

char AuxString[1024];

bool BrokenInterpolationVar[ _MaxInterpVarIDs ];
float InitialValuesInterpVar[ _MaxInterpVarIDs ];
float InitialValuesConfigurationVar[ _MaxConfigurationVarIDs ];

//////////////////////////////////////////////
// CA initialization
float initCA = 0.0f;

/////////////////////////////////////////////////////////////////////////
// WORKING VARIABLES
/////////////////////////////////////////////////////////////////////////

// ++++++++++++++++++++++ DRAWING MODE +++++++++++++++++++++ 
int activeDrawingStroke; // initially not drawing

float isBeat = 0.0f; // boolean for dot-based drawing

// ++++++++++++++++++++++ FREEZING AND MEMORY VARIABLES ++++++++++++++++++++++++ 
bool frozen = false;

// CA saved in case of freezing
int CAType_saved = 0; // for freeze 
int CASubType_saved = 0; // for freeze 
bool CA_onOff = true; // CA on off flag

// particles mode saved in case of freezing
int particleMode_saved = 0; // for freeze 

// ++++++++++++++++++++++ MUSIC CONTROL ++++++++++++++++++++ 
// music control inputs received at each frame
// addition of color based on palette
float color_spectrum[4];
// factor increasing the acceleration weight of particles
// based on sound volume attacks
float particle_acc_attack;
// radius factor
float brush_radius_vol;

// ++++++++++++++++++++++ SET-UP +++++++++++++++++++++++++++ 
int cur_scene;

// scene management
bool firstFrameInScene = true;
float remainingTimeInScene = 0.0f;

// working variables 
int drawing_start_frame;

// SCENARIO LAUNCH TIME
double InitialScenarioTime;

// +++++++++++++++++++++++ CLEAR AND INVERT +++++++++++++++++++++++++++++++++
double lastClearTime = 0.0;

// +++++++++++++++++++++++ Beats +++++++++++++++++++++++++++++++++
int BeatNo = 1;
double lastBeatTime = 0.0;

///////////////////////////////////////////////////////////////////////////////////
// INITIALIZATION COMMAND
// SHADER VARIABLE INITIAL VALUES
/////////////////////////////////////////////////////////////

void InitializeConfigurationVar(void) {
	for (int indVar = 0; indVar < _MaxConfigurationVarIDs; indVar++) {
		if (ConfigurationVarTypes[indVar] == _pg_float) {
			*((float *)ConfigurationVarPointers[indVar]) = InitialValuesConfigurationVar[indVar];
		}
		else if (ConfigurationVarTypes[indVar] == _pg_int) {
			*((int *)ConfigurationVarPointers[indVar]) = (int)myround(InitialValuesConfigurationVar[indVar]);
		}
		else if (ConfigurationVarTypes[indVar] == _pg_bool) {
			*((bool *)ConfigurationVarPointers[indVar]) = (InitialValuesConfigurationVar[indVar] > 0 ? true : false);
		}
		else if (ConfigurationVarTypes[indVar] == _pg_sign) {
			*((float *)ConfigurationVarPointers[indVar]) = (InitialValuesConfigurationVar[indVar] > 0 ? 1.0f : -1.0f);
		}
		else if (ConfigurationVarTypes[indVar] == _pg_track) {
			*((bool *)ConfigurationVarPointers[indVar]) = (InitialValuesConfigurationVar[indVar] > 0 ? true : false);
		}
		else {
			sprintf(ErrorStr, "Incorrect variable type (%d) for configuration variable #%d!", ConfigurationVarTypes[indVar], indVar); ReportError(ErrorStr);
		}
	}
}

void pg_initializationScript( void ) {
  /////////////////////////////////////////////////////////////////////////
  // CONSTANT UNIFORM VARIABLES
  glUseProgram(shader_Drawing_programme);
  // only assigned at initialization, does not change during the set
  glUniform2f(uniform_Drawing_vp_2fv_width_height , 
	      (GLfloat)PG_FBO_WIDTH , (GLfloat)PG_FBO_HEIGHT );
  /////////////////////////////////////////////////////////////////////////
  // SCENARIO-CONTROLLED UNIFORM VARIABLES
  for( int indVar = 0 ; indVar < _MaxInterpVarIDs ; indVar++ ) {
      if (ScenarioVarTypes[indVar] == _pg_float) {
	    *((float *)ScenarioVarPointers[indVar]) = InitialValuesInterpVar[indVar];
      }
      else if (ScenarioVarTypes[indVar] == _pg_int) {
	    *((int *)ScenarioVarPointers[indVar]) = (int)myround(InitialValuesInterpVar[indVar]);
      }
      else if (ScenarioVarTypes[indVar] == _pg_bool) {
	    *((bool *)ScenarioVarPointers[indVar]) = (InitialValuesInterpVar[indVar] > 0 ? true : false);
      }
      else if (ScenarioVarTypes[indVar] == _pg_sign) {
	    *((float *)ScenarioVarPointers[indVar]) = (InitialValuesInterpVar[indVar] > 0 ? 1.0f : -1.0f);
      }
      else if (ScenarioVarTypes[indVar] == _pg_track) {
	    *((bool *)ScenarioVarPointers[indVar]) = (InitialValuesInterpVar[indVar] > 0 ? true : false);
      }
	  else {
		  sprintf(ErrorStr, "Incorrect variable type (%d) for scenario variable #%d!", ScenarioVarTypes[indVar] , indVar); ReportError(ErrorStr);
	  }
  }

  // current drawing mode for display 
  activeDrawingStroke = activeDrawingStroke_ini; // off drawing
  pg_apply_new_activeDrawingStroke();

  /////////////////////////////////////////////////////////////////////////
  // SOUND-CONTROLLED UNIFORM VARIABLES: dynamic values received from PD 
  // pen music color
  color_spectrum[0] = color_spectrum_r_ini;
  color_spectrum[1] = color_spectrum_g_ini;
  color_spectrum[2] = color_spectrum_b_ini;
  color_spectrum[3] = color_spectrum_a_ini;
  // particle acceleration based on sound attack
  particle_acc_attack = particle_acc_attack_ini;
  // radius based on music
  brush_radius_vol = brush_radius_vol_ini;

  /////////////////////////////////////////////////////////////////////////
  // DRAWING-CONTROLLED UNIFORM VARIABLES: dynamic values received from drawing or track display 
  // pen position storage on the two quads 
  tracks_x[0] = -1.0F;
  tracks_x[1] = -1.0F;
  tracks_x[2] = -1.0F;
  tracks_y[0] = -1.0F;
  tracks_y[1] = -1.0F;
  tracks_y[2] = -1.0F;
  tracks_x_prev[0] = -1.0F;
  tracks_x_prev[1] = -1.0F;
  tracks_x_prev[2] = -1.0F;
  tracks_y_prev[0] = -1.0F;
  tracks_y_prev[1] = -1.0F;
  tracks_y_prev[2] = -1.0F;
  // pen color
  tracks_Color_r[0] = 0.0F;
  tracks_Color_r[1] = 0.0F;
  tracks_Color_r[2] = 0.0F;
  tracks_Color_g[0] = 0.0F;
  tracks_Color_g[1] = 0.0F;
  tracks_Color_g[2] = 0.0F;
  tracks_Color_b[0] = 0.0F;
  tracks_Color_b[1] = 0.0F;
  tracks_Color_b[2] = 0.0F;
  tracks_Color_a[0] = 1.0F;
  tracks_Color_a[1] = 1.0F;
  tracks_Color_a[2] = 1.0F;
  // pen brush & size
  tracks_BrushID[0] = 0;
  tracks_BrushID[1] = 0;
  tracks_BrushID[2] = 0;
  tracks_RadiusX[0] = 0.0F;
  tracks_RadiusX[1] = 0.0F;
  tracks_RadiusX[2] = 0.0F;
  tracks_RadiusY[0] = 0.0F;
  tracks_RadiusY[1] = 0.0F;
  tracks_RadiusY[2] = 0.0F;
  // hoover cursor
  CurrentCursorHooverPos_x = 0;
  CurrentCursorHooverPos_y = 0;

  /////////////////////////////////////////////////////////////////////////
  // TIME CONTROLLED VARIABLES (NOT FORWARDED TO SHADERS)
  // scenario management
  cur_scene = -1;
  // frame number initialization
  FrameNo = first_frame_number - 1;
  // intial real time
  InitialRealTime = RealTime() + initial_time;
  // initial scenario time: well before current time to be over
  InitialScenarioTime = InitialRealTime - 1000000;
  // TIME CONTROLLED UNIFORM VARIABLES
  // message transparency & echo
  messageTransparency = 0.0;

  /////////////////////////////////////////////////////////////////////////
  // INTERFACE INITIALIZATION
  pg_send_message_udp( (char *)"" , (char *)"/connect" , (char *)"udp_PD_send" );

  // MICROPHONE CALIBRATION
  sprintf( AuxString , "/input_volume %.5f" , input_volume ); pg_send_message_udp( (char *)"f" , AuxString , (char *)"udp_PD_send" );

  pg_displaySceneVariables();

  pg_send_message_udp( (char *)"s" , (char *)"/message_can INIT_COMPLETED" , (char *)"udp_PD_send" );
}

///////////////////////////////////////////////////////////////////////////////////
// GUI DISPLAY & LOG FILE LOGGING
/////////////////////////////////////////////////////////////
void pg_displaySceneVariables( void ) {
  if( FrameNo % 20 == 0 || firstFrameInScene ) {
    /////////////////////////////////////////////////////////////////////////
    // NON AUTOMATIC DISPLAY OF VARIABLES
    sprintf( AuxString , "/drawing_can %d" , activeDrawingStroke ); pg_send_message_udp( (char *)"f" , AuxString , (char *)"udp_PD_send" );
    if( clearAllLayers ) {
       pg_send_message_udp( (char *)"s" , (char *)"/message_can ***CLEAR***" , (char *)"udp_PD_send" );
    }

    /////////////////////////////////////////////////////////////////////////
    // SCENARIO-CONTROLLED UNIFORM VARIABLES
    for( int indVar = 0 ; indVar < _MaxInterpVarIDs ; indVar++ ) {
      if( *(ScenarioVarMessages[indVar]) ) {
	if (ScenarioVarTypes[indVar] == _pg_float) {
	      sprintf( AuxString , "/%s %.4f" , ScenarioVarMessages[indVar] , *((float *)ScenarioVarPointers[indVar]) ); pg_send_message_udp( (char *)"f" , AuxString , (char *)"udp_PD_send" );
	}
	else if (ScenarioVarTypes[indVar] == _pg_int) {
	      sprintf( AuxString , "/%s %d" , ScenarioVarMessages[indVar] , *((int *)ScenarioVarPointers[indVar]) ); pg_send_message_udp( (char *)"f" , AuxString , (char *)"udp_PD_send" );
	}
	else if (ScenarioVarTypes[indVar] == _pg_bool) {
	      sprintf( AuxString , "/%s %d" , ScenarioVarMessages[indVar] , (int)(*((bool *)ScenarioVarPointers[indVar])) ); pg_send_message_udp( (char *)"f" , AuxString , (char *)"udp_PD_send" );
	}
	else if (ScenarioVarTypes[indVar] == _pg_sign) {
	      sprintf( AuxString , "/%s %.4f" , ScenarioVarMessages[indVar] , *((float *)ScenarioVarPointers[indVar]) ); pg_send_message_udp( (char *)"f" , AuxString , (char *)"udp_PD_send" );
	}
	else if (ScenarioVarTypes[indVar] == _pg_track) {
	      sprintf( AuxString , "/%s %d" , ScenarioVarMessages[indVar] , (int)(*((bool *)ScenarioVarPointers[indVar])) ); pg_send_message_udp( (char *)"f" , AuxString , (char *)"udp_PD_send" );
	}
      }
    }

    /////////////////////////////////////////////////////////////////////////
    // TIME CONTROLLED VARIABLES (NOT FORWARDED TO SHADERS)
    // time display
    sprintf( AuxString , "/time_can %d:%d" , (int)remainingTimeInScene/60 , (int)remainingTimeInScene % 60 ); pg_send_message_udp( (char *)"s" , AuxString , (char *)"udp_PD_send" );
    if( remainingTimeInScene < 10 ) {
      sprintf( AuxString , "/time_color_can 0" ); pg_send_message_udp( (char *)"i" , AuxString , (char *)"udp_PD_send" );
    }
    else if( remainingTimeInScene < 20 ) {
      sprintf( AuxString , "/time_color_can 1" ); pg_send_message_udp( (char *)"i" , AuxString , (char *)"udp_PD_send" );
    }
    else if( remainingTimeInScene < 30 ) {
      sprintf( AuxString , "/time_color_can 2" ); pg_send_message_udp( (char *)"i" , AuxString , (char *)"udp_PD_send" );
    }
    else {
      sprintf( AuxString , "/time_color_can 3" ); pg_send_message_udp( (char *)"i" , AuxString , (char *)"udp_PD_send" );
    }
  }
}

void pg_send_message_udp( char *pattern , char * message , char *targetHostid ) {
	// sprintf( AuxString , "/particles_can %f" , particleMode ); pg_send_message_udp( (char *)"f" , AuxString , (char *)"udp_PD_send" );
	pg_IPClient* targetHost = NULL;
	for( int ind = 0 ; ind < nb_IP_Clients ; ind++ ) {
		if( strcmp( targetHostid , IP_Clients[ ind ]->id.c_str() ) == 0 ) {
			targetHost = IP_Clients[ ind ];
			break;
		}
	}
	if( !targetHost ) {
		printf( "UDP client unknown %s\n" , targetHostid );
		return;
	}
	// printf("send_message_udp %s %s %d %d\n", message, pattern, nb_IP_Clients, targetHost);
	targetHost->storeIP_output_message( message , pattern );
}

void pg_logSceneVariables( void ) {
  /////////////////////////////////////////////////////////////////////////
  // SCENARIO-CONTROLLED UNIFORM VARIABLES
  for( int indVar = 0 ; indVar < _MaxInterpVarIDs ; indVar++ ) {
      if (ScenarioVarTypes[indVar] == _pg_float) {
	      fprintf( fileLog , "/%s %.6f\n" , CmdString[indVar] , *((float *)ScenarioVarPointers[indVar]) );
      }
      else if (ScenarioVarTypes[indVar] == _pg_int) {
	      fprintf( fileLog , "/%s %d\n" , CmdString[indVar] , *((int *)ScenarioVarPointers[indVar]) );
      }
      else if (ScenarioVarTypes[indVar] == _pg_bool) {
	      fprintf( fileLog , "/%s %d\n" , CmdString[indVar] , (int)(*((bool *)ScenarioVarPointers[indVar])) );
      }
      else if (ScenarioVarTypes[indVar] == _pg_sign) {
	      fprintf( fileLog , "/%s %.6f\n" , CmdString[indVar] , *((float *)ScenarioVarPointers[indVar]) );
      }
      else if (ScenarioVarTypes[indVar] == _pg_track) {
	      fprintf( fileLog , "/%s %d\n" , CmdString[indVar] , (int)(*((bool *)ScenarioVarPointers[indVar])) );
      }
  }
}


///////////////////////////////////////////////////////////////////////////////////
// SCENARIO CALLBACKS
///////////////////////////////////////////////////////////////////////////////////
void currentDrawingStroke_callBack(pg_Parameter_Input_Type param_input_type , float scenario_or_gui_command_value) {
	if (activeDrawingStroke > 0) {
		activeDrawingStroke = currentDrawingStroke + 1;
		pg_apply_new_activeDrawingStroke();
	}
}
void particle_texture_ID_callBack( pg_Parameter_Input_Type param_input_type , float scenario_or_gui_command_value) {
}
void impactX_callBack( pg_Parameter_Input_Type param_input_type , float scenario_or_gui_command_value) {
	newImpact = true;
}
void impactY_callBack( pg_Parameter_Input_Type param_input_type , float scenario_or_gui_command_value) {
		  pg_send_message_udp((char *)"s", (char *)"/message_can ***CLEAR***", (char *)"udp_PD_send");
}
void projReversed_callBack( pg_Parameter_Input_Type param_input_type , float scenario_or_gui_command_value) {
	// sprintf(AuxString, "/projReversed_can %d", int(projReversed)); pg_send_message_udp((char *)"i", AuxString, (char *)"udp_PD_send");
  //printf("%s\n",AuxString);
}
void proj2Reversed_callBack( pg_Parameter_Input_Type param_input_type , float scenario_or_gui_command_value) {
	//sprintf(AuxString, "/proj2Reversed_can %d", int(proj2Reversed)); pg_send_message_udp((char *)"i", AuxString, (char *)"udp_PD_send");
  //printf("%s\n",AuxString);
}
void clearAllLayers_callBack( pg_Parameter_Input_Type param_input_type , float scenario_or_gui_command_value) {
        if( scenario_or_gui_command_value ) {
	  //printf("delta %.3f\n", (float)(CurrentClockTime - lastClearTime) );
	  if( CurrentClockTime - lastClearTime  < 0.5 && param_input_type == _PG_KEYSTROKE ) {
		  pg_send_message_udp((char *)"s", (char *)"/message_can ***CLEAR***", (char *)"udp_PD_send");
		  pg_initializationScript();
		  lastClearTime = 0.0;
	  }
	  else {
		  pg_send_message_udp((char *)"s", (char *)"/message_can clear", (char *)"udp_PD_send");
		  lastClearTime = CurrentClockTime;
	  }
	}
}

///////////////////////////////////////////////////////////////////////////////////
// SCENARIO BASED COMMANDS
///////////////////////////////////////////////////////////////////////////////////
void pg_update_variable(pg_Parameter_Input_Type param_input_type,
		      pg_Keystroke_Input_Type parm_keystroke_type,
		      int indParam,
		      float scenario_or_gui_command_value ) {
	//float step = 0.0f;
	//if (parm_keystroke_type == _PG_KEYSTROKE_MINUS) {
	//	step = StepMinus[indParam];
	//}
	//else {
	//	step = StepPlus[indParam];
	//}
	if (param_input_type == _PG_SCENARIO) {
		if (BrokenInterpolationVar[indParam]) {
			return;
		}
	}
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_KEYSTROKE) {
		BrokenInterpolationVar[indParam] = true;
	}
	
	if (ScenarioVarTypes[indParam] == _pg_float) {
		if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
			*((float *)ScenarioVarPointers[indParam]) = scenario_or_gui_command_value;
		}
		//else if (param_input_type == _PG_KEYSTROKE) {
		//	*((float *)ScenarioVarPointers[indParam]) += step;
		//}
		if (ScenarioVarCallbacks[indParam]) {
			(*ScenarioVarCallbacks[indParam])(param_input_type,scenario_or_gui_command_value);
		}
	}
	else if (ScenarioVarTypes[indParam] == _pg_int) {
		if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
			*((int *)ScenarioVarPointers[indParam]) = (int)myround(scenario_or_gui_command_value);
		}
		//else if (param_input_type == _PG_KEYSTROKE) {
		//	*((int *)ScenarioVarPointers[indParam]) = (*((int *)ScenarioVarPointers[indParam]) + (int)step + (int)MaxValues[indParam] + 1) % ((int)MaxValues[indParam] + 1);
		//}
		if (ScenarioVarCallbacks[indParam]) {
			(*ScenarioVarCallbacks[indParam])(param_input_type,scenario_or_gui_command_value);
		}
	}
	else if (ScenarioVarTypes[indParam] == _pg_bool) {
		if (param_input_type == _PG_SCENARIO) {
			*((bool *)ScenarioVarPointers[indParam]) = (scenario_or_gui_command_value > 0 ? true : false);
		}
		else if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_KEYSTROKE) {
			*((bool *)ScenarioVarPointers[indParam]) = !(*((bool *)ScenarioVarPointers[indParam]));
			// printf("Opposite boolean index %d value %d\n" , indParam,*((bool *)ScenarioVarPointers[indParam]));
		}
		if (ScenarioVarCallbacks[indParam]) {
			(*ScenarioVarCallbacks[indParam])(param_input_type,scenario_or_gui_command_value);
		}
	}
	else if (ScenarioVarTypes[indParam] == _pg_sign) {
		if (param_input_type == _PG_SCENARIO) {
			*((float *)ScenarioVarPointers[indParam]) = (scenario_or_gui_command_value > 0 ? 1.0f : -1.0f);
		}
		else if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_KEYSTROKE) {
			*((float *)ScenarioVarPointers[indParam]) = -1.0f * (*((float *)ScenarioVarPointers[indParam]));
		}
		if (ScenarioVarCallbacks[indParam]) {
			(*ScenarioVarCallbacks[indParam])(param_input_type,scenario_or_gui_command_value);
		}
	}
	else if (ScenarioVarTypes[indParam] == _pg_track) {
		if (ScenarioVarCallbacks[indParam]) {
			(*ScenarioVarCallbacks[indParam])(param_input_type,scenario_or_gui_command_value);
		}
	}
}

void pg_update_scenario( void ) {
  float elapsed_time_from_start = (float)(CurrentClockTime - InitialScenarioTime);
  firstFrameInScene = false;
  // scans all scenes to find the current one and applies the corresponding interpolations
  for(int ind_scene = 0 ; ind_scene < nb_scenes ; ind_scene++ ) {
    // printf( "time %.2f beg %.2f end %.2f\n" , elapsed_time_from_start ,  scene_initial_times[ ind_scene ] ,  scene_final_times[ ind_scene ] );
    // current scene found
    if( elapsed_time_from_start >= scene_initial_times[ ind_scene ]
	&& elapsed_time_from_start <= scene_final_times[ ind_scene ] ) {
      if( ind_scene != cur_scene ) {
	cur_scene = ind_scene;
	firstFrameInScene = true;
	sprintf(AuxString, "/setup_can %s", scene_IDs[cur_scene].c_str()); pg_send_message_udp((char *)"s", AuxString, (char *)"udp_PD_send");
	// printf( "/setup_can %s\n" , scene_IDs[ cur_scene ].c_str() ); 
	// reinitialization of the interpolation control variables at the beginning of a new scene
	for( int indVar = 0 ; indVar < _MaxInterpVarIDs ; indVar++ ) {
	  BrokenInterpolationVar[ indVar ] = false;
	}
      }

      // for time display + colors when reaching the end of the scene
      remainingTimeInScene 
	= scene_initial_times[ ind_scene ] + scene_durations[ ind_scene ] - elapsed_time_from_start;
      // printf( "\ntime %.2f interp %.2f " , elapsed_time_from_start , coefInterpolation );

      // transformed interpolation according to the interpolation function
      for(int indP = 0 ; indP < _MaxInterpVarIDs ; indP++ ) {
	// going from interpolation percentages to interpolation times by multiplying by scene duration
	float absoluteOffset = scene_interpolations[ ind_scene ][ indP ].offSet 
	                       * scene_durations[ ind_scene ];
	float absoluteDuration = scene_interpolations[ ind_scene ][ indP ].duration 
	                         * scene_durations[ ind_scene ];

	float transformedInterpolation = 0.0;
	if( elapsed_time_from_start <=  scene_initial_times[ ind_scene ] 
	                                 + absoluteOffset ) {
	  transformedInterpolation = 0.0;
	}
	else if( elapsed_time_from_start >=  scene_initial_times[ ind_scene ] 
	                                      + absoluteOffset + absoluteDuration ) {
	  transformedInterpolation = 1.0;
	}
	else {
	  float coefInterpolation 
	    = elapsed_time_from_start - (scene_initial_times[ ind_scene ] + absoluteOffset);
	  // time normalization for interpolation
	  if( absoluteDuration > 0.0 ) {
	    coefInterpolation /= absoluteDuration;
	  }
	  if( coefInterpolation <  0.0 )coefInterpolation = 0.0;
	  if( coefInterpolation >  1.0 )coefInterpolation = 1.0;

	  switch( scene_interpolations[ ind_scene ][ indP ].interpolation_mode ) {
	  case pg_linear_interpolation:
	    transformedInterpolation = coefInterpolation;
	    break;
	  case pg_cosine_interpolation:
	    transformedInterpolation 
	      = 0.5F * (cos( (coefInterpolation - 1.0F) * (float)M_PI ) + 1.0F );
	    break;
	  default:
	    printf("Stepwise interpolations have no intermediate values\n");
	    break;
	  }
	}

	if( (transformedInterpolation == 0.0 && firstFrameInScene)
	    || transformedInterpolation > 0.0 ) {
	  // calculation of the interpolated value and assignment to variable
	  float interpolated_value 
	    = (1.0F - transformedInterpolation) 
	    * scene_initial_parameters[ ind_scene ][ indP ]
	    + transformedInterpolation 
	    * scene_final_parameters[ ind_scene ][ indP ];

	  // printf( "Scene %d param %d time from start %.2f interp %.2f transf_interp %.2f value %.2f start interp %.2f duration %.2f\n" , ind_scene, indP, elapsed_time_from_start , coefInterpolation , transformedInterpolation , interpolated_value ,scene_initial_times[ ind_scene ] + scene_interpolations[ ind_scene ][ indP ].offSet , scene_interpolations[ ind_scene ][ indP ].duration );

	  pg_update_variable(_PG_SCENARIO, _PG_KEYSTROKE_VOID,
			      indP, interpolated_value );
	}
      }
      return;      
    }
    // current scene found
  }
  // scans all scenes to find the current one and applies the corresponding interpolations
}


///////////////////////////////////////////////////////////////////////////////////
// KEYSTROKE BASED COMMANDS
///////////////////////////////////////////////////////////////////////////////////

void pg_process_key( int key ) {
  int mod = glutGetModifiers();

  // printf( "key (%d) mod %d\n" , key , mod );

  if(!(mod & GLUT_ACTIVE_ALT) ) {
    // non special key that corresponds to a script
    pg_keyStrokeScripts( key );
    return;
  }

  switch (key) {
  
    /* ------------------------------- frame per second */
  case 'f':
    DisplayFramePerSecond = !DisplayFramePerSecond;
    printf( "FPS display %d\n" , DisplayFramePerSecond );
    break;

    /* ------------------------------- snapshot */
  case 's':
    pg_draw_scene( _Jpg );
    break;
  case 'S':
    pg_draw_scene( _Svg );
    break;

    /* ------------------------------- beat */
  case 'c':
    pg_send_message_udp( (char *)"f" , (char *)"/buffer 0" , (char *)"udp_SC_send" );
    break;

  case 'a':
    with_mesh = !with_mesh;
    isDisplayLookAt = !isDisplayLookAt;
    break;

  case 'b':
    with_blue = !with_blue;
    // printf("with mesh on blue %d\n" , with_blue );
    break;

  default:
    printf ("key %d is not active.\n", key);
    break;
  }     
}

/*!
 * \brief special keys events (interactive mode)
 * \param key			the key pressed
 */
void pg_process_special_key( int key ) {
  // switch (key) {    
  // default:
  //   printf ("special key %d is not active.\n", key);
  //   break;
  // }   
}

void pg_keyStrokeScripts(int key) {
  pg_Keystroke_Input_Type CommandType = _PG_KEYSTROKE_VOID;
 /* char *CmdChar = NULL;
  int indChar = -1;
  bool oneCommandIssued = false;

  char * ptCh = CmdCharMinus;
  while ((CmdChar = strchr(ptCh,key)) != NULL) {
	  CommandType = _PG_KEYSTROKE_MINUS;
	  indChar = CmdChar - CmdCharMinus;
	  if (indChar >= 0) {
		  pg_update_variable(_PG_KEYSTROKE, CommandType,
				    indChar, 0.0F );
		  oneCommandIssued = true;
	  }
	  ptCh = CmdChar + 1;
  }
  if( oneCommandIssued ) {
    return;
  }
  ptCh = CmdCharPlus;
  if ((CmdChar = strchr(ptCh,key)) != NULL) {
	  CommandType = _PG_KEYSTROKE_PLUS;
	  indChar = CmdChar - CmdCharPlus;
	  if (indChar >= 0) {
		  pg_update_variable(_PG_KEYSTROKE, CommandType,
				    indChar, 0.0F );
		  oneCommandIssued = true;
	  }
	  ptCh = CmdChar + 1;
  }
  if( oneCommandIssued ) {
    return;
  }*/

  // printf("indChar %d part_acc_factor %d\n", indChar, _part_acc_factor);

  switch (key) {
  
    // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
    // +++++++++++++++++ TEST UDP ++++++++++++++++++++++++++++++ 
    // +++++++++++++++++   keystroke (T)  ++++++++++++++++++++++ 
    // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
  case'T':
    pg_writeMessageOnScreen( (char * )"******" );
    pg_send_message_udp( (char *)"f" , (char *)"/testUDP 0" , (char *)"udp_SC" );
    sprintf( AuxString , "/message_can TEST_UDP" ); pg_send_message_udp( (char *)"s" , AuxString , (char *)"udp_PD_send" );
    sprintf( AuxString , "/return_message returnUPD_%f" , CurrentClockTime ); pg_send_message_udp( (char *)"s" , AuxString , (char *)"udp_PD_send" );
    break;
    
    // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
    // +++++++++++++++++ LAUNCH TIMER ++++++++++++++++++++++++++ 
    // +++++++++++++++++   keystroke (t)  ++++++++++++++++++++++ 
    // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
  case't':
    restoreInitialTimesAndDurations();
    sprintf( AuxString , "/launch %f" , CurrentClockTime ); pg_send_message_udp( (char *)"f" , AuxString , (char *)"udp_SC_send" );
    pg_send_message_udp( (char *)"s" , (char *)"/message_can START" , (char *)"udp_PD_send" );
    InitialScenarioTime 
      = CurrentClockTime - scene_initial_times[ 0 ];
    sprintf( AuxString , "/setup_can %s" , scene_IDs[ 0 ].c_str() ); pg_send_message_udp( (char *)"s" , AuxString , (char *)"udp_PD_send" );
    // printf( "/setup_can %s" , scene_IDs[ 0 ].c_str() ); 
    // reinitialization of the interpolation control variables at the beginning of a new scene
    for( int indVar = 0 ; indVar < _MaxInterpVarIDs ; indVar++ ) {
      BrokenInterpolationVar[ indVar ] = false;
    }
    break;

    // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
    // +++++++++++++++++ FREEZE ON/OFF +++++++++++++++++++++++ 
    // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
    // Stops or restaures CA and particle modes (F)  
    // ====================================== 
  case'F':
    if( frozen == 0 ) {
      frozen = 1;
      particleMode_saved = particleMode;
      CASubType_saved = CASubType;
      CAType_saved = CAType;
      CASubType = 0;
      CAType = 0;
      CA_onOff = false;
      particleMode = 0;
    } else {
      frozen = 0;
      particleMode = particleMode_saved;
      CASubType =  CASubType_saved;
      CAType = CAType_saved;
      CA_onOff = true;
    }
    break;

  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
  // +++++++++++++++++ DRAWING ON/OFF ++++++++++++++++++++++++ 
  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
  // drawing on off void/currentDrawing mode (w)   
  // ============================================= 
  case'w':
    if( activeDrawingStroke == 0 ) {
      activeDrawingStroke = currentDrawingStroke + 1;
    } else {
      activeDrawingStroke = 0;
    }
    pg_apply_new_activeDrawingStroke();
    break;

  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
  // +++++++++++++++++ SET-UP ++++++++++++++++++++++++++++++++ 
  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
  // ====================================== 
  // advances to next scene without respect for the global timing
  case'S':
  {
	  restoreInitialTimesAndDurations();
	  int new_scene = ((1 + cur_scene) % nb_scenes);
	  InitialScenarioTime
		  = CurrentClockTime - scene_initial_times[new_scene];
	  sprintf(AuxString, "/setup_can %s", scene_IDs[new_scene].c_str()); pg_send_message_udp((char *)"s", AuxString, (char *)"udp_PD_send");
  }
  break;

  // advances to next scene and keeps the global timing
  case's':
  {
	  restoreInitialTimesAndDurations();
	  float elapsedTime = (float)(CurrentClockTime - InitialScenarioTime) - scene_initial_times[cur_scene];
	  // only accepted if the current scene has been on for a while
	  if (elapsedTime > 10) {
		  float deltaTime = scene_initial_times[1 + cur_scene] - (float)(CurrentClockTime - InitialScenarioTime);
		  // if the scene has not yet begun
		  if (deltaTime > 0) {
			  int new_scene = ((1 + cur_scene) % nb_scenes);
			  // we place the beginning of the current scene at this time
			  scene_initial_times[new_scene] -= deltaTime;
			  if (new_scene > 0) {
				  scene_final_times[new_scene - 1] -= deltaTime;
				  scene_durations[new_scene - 1] -= deltaTime;
			  }
			  scene_durations[new_scene] += deltaTime;
			  // unuseful because is already made through scene_initial_times[ new_scene ] -= deltaTime;
			  //InitialScenarioTime 
			  //  = CurrentClockTime - scene_initial_times[ new_scene ];
			  sprintf(AuxString, "/setup_can %s", scene_IDs[new_scene].c_str()); pg_send_message_udp((char *)"s", AuxString, (char *)"udp_PD_send");
		  }
	  }
  }
  break;
  }
}

///////////////////////////////////////////////////////////////////////////////////
// UDP BASED COMMANDS
///////////////////////////////////////////////////////////////////////////////////
void pg_aliasScript( char *command_symbol , 
		     char* string_argument_0 ,
		     float arguments[MAX_OSC_ARGUMENTS] ) {
  char newCommand[512];

  strcpy( newCommand , command_symbol );

  // compares message tag with known command tags


  // ====================================== 
  // test UDP connection                    
  // ====================================== 
  if( strcmp( newCommand , "testUDP" ) == 0 ) {
    sprintf( AuxString , "******" ); pg_writeMessageOnScreen( AuxString );
    sprintf( AuxString , "/message_can TEST_UDP" ); pg_send_message_udp( (char *)"s" , AuxString , (char *)"udp_PD_send" );
    sprintf( AuxString , "/return_message **OK**" ); pg_send_message_udp( (char *)"s" , AuxString , (char *)"udp_PD_send" );
    
  } else if( strcmp( newCommand , "return_message" ) == 0 ) {
    pg_writeMessageOnScreen( string_argument_0 );

    // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
    // +++++++++++++++++ LAUNCH TIMER ++++++++++++++++++++++++++ 
    // +++++++++++++++++   keystroke (t)  ++++++++++++++++++++++ 
    // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
  } else if( strcmp( newCommand , "launch" ) == 0 ) {
    restoreInitialTimesAndDurations();
    sprintf( AuxString , "/launch %f" , CurrentClockTime ); pg_send_message_udp( (char *)"f" , AuxString , (char *)"udp_PD_send" );
    pg_send_message_udp( (char *)"s" , (char *)"/message_can START" , (char *)"udp_PD_send" );
    InitialScenarioTime 
      = CurrentClockTime - scene_initial_times[ 0 ];
	sprintf(AuxString, "/setup_can %s", scene_IDs[0].c_str()); pg_send_message_udp((char *)"s", AuxString, (char *)"udp_PD_send");
    // reinitialization of the interpolation control variables at the beginning of a new scene
    for( int indVar = 0 ; indVar < _MaxInterpVarIDs ; indVar++ ) {
      BrokenInterpolationVar[ indVar ] = false;
    }

    // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
    // +++++++++++++++++ MUSIC CONTROL +++++++++++++++++++++++++ 
    // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
  } else if( strcmp( newCommand , "color_spectrum" ) == 0 ) {
    color_spectrum[0] = arguments[0]; 
    color_spectrum[1] = arguments[1]; 
    color_spectrum[2] = arguments[2]; 
    color_spectrum[3] = 0; 
    float value = (color_spectrum[0]<color_spectrum[1])?color_spectrum[1]:color_spectrum[0];
    value = (value<color_spectrum[2])?color_spectrum[2]:value;
    // printf( "color_spectrum: %f %f %f\n" , color_spectrum[0] , color_spectrum[1] , color_spectrum[2] );
    if( invertAllLayers ) {
      color_spectrum[0] = value - color_spectrum[0]; 
      color_spectrum[1] = value - color_spectrum[1]; 
      color_spectrum[2] = value - color_spectrum[2]; 
    }
    
    //color_spectrum[0] = 1.0; 
    //color_spectrum[1] = 1.0; 
    //color_spectrum[2] = 1.0; 
  } else if( strcmp( newCommand , "brush_radius_vol" ) == 0 ) {
    brush_radius_vol = arguments[0]; 
    
  } else if( strcmp( newCommand , "particle_acc_attack" ) == 0 ) {
    particle_acc_attack = 0.0f;// arguments[0] * part_acc_attack_coef; //  * part_acc_attack_coef is made in PD

    // ====================================== 
    // keystroke s: Snapshot                  
    // ====================================== 
  } else if( strcmp( newCommand , "snapshot" ) == 0 ) {
    pg_snapshot( (char *)"jpg" );
    pg_logSceneVariables();

    // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
    // +++++++++++++++++ DRAWING ON/OFF ++++++++++++++++++++++++ 
    // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
    // drawing on off void/currentDrawing mode (w)   
    // ============================================= 
  } else if( strcmp( newCommand , "drawing_on_off" ) == 0 ) {
    if( activeDrawingStroke == 0 ) {
      activeDrawingStroke = currentDrawingStroke + 1;
    } else {
      activeDrawingStroke = 0;
    }
    pg_apply_new_activeDrawingStroke();

    // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
    // +++++++++++++++++ FREEZE ON/OFF +++++++++++++++++++++++ 
    // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
    // Stops or restaures CA and particle modes (F)  
    // ====================================== 
  } else if( strcmp( newCommand , "frozen_on_off" ) == 0 ) {
    if( frozen == 0 ) {
      frozen = 1;
      particleMode_saved = particleMode;
      CASubType_saved = CASubType;
      CAType_saved = CAType;
      particleMode = 0;
      CASubType = 0;
      CAType = 0;
      CA_onOff = false;
    } else {
      frozen = 0;
      particleMode = particleMode_saved;
      CASubType =  CASubType_saved;
      CAType = CAType_saved;
      CA_onOff = true;
    }

  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
    // +++++++++++++++++ SET-UP ++++++++++++++++++++++++++++++++ 
    // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
    // ====================================== 
  } else if( strcmp( newCommand , "setup_plus" ) == 0 ) {
    restoreInitialTimesAndDurations();
    int new_scene = ((1 + cur_scene)%nb_scenes);
    InitialScenarioTime 
      = CurrentClockTime - scene_initial_times[ new_scene ];
	sprintf(AuxString, "/setup_can %s", scene_IDs[new_scene].c_str()); pg_send_message_udp((char *)"s", AuxString, (char *)"udp_PD_send");

  } else if( strcmp( newCommand , "setup_plus_and_keep_total_duration" ) == 0 ) {
    float elapsedTime = (float)(CurrentClockTime - InitialScenarioTime) - scene_initial_times[ cur_scene ];
    float deltaTime = scene_initial_times[ 1 + cur_scene ] - (float)(CurrentClockTime - InitialScenarioTime);
        // only accepted if the current scene has been on for a while
    if( elapsedTime > 15
	// and if the scene has not begun since a short while
	&& deltaTime > 0 
	// and if the scene is not too far from being finished
	&& deltaTime < 60 ) {
      int new_scene = ((1 + cur_scene) % nb_scenes);
    
      restoreInitialTimesAndDurations();

      // we place the beginning of the current scene at this time
      scene_initial_times[ new_scene ] -= deltaTime;
      scene_durations[ new_scene ] += deltaTime;
      if( new_scene > 0 ) {
	scene_final_times[ new_scene - 1 ] -= deltaTime;
	scene_durations[ new_scene - 1 ] -= deltaTime;
      }
      // unuseful because is already made through scene_initial_times[ new_scene ] -= deltaTime;
      //InitialScenarioTime 
      //  = CurrentClockTime - scene_initial_times[ new_scene ];
	  sprintf(AuxString, "/setup_can %s", scene_IDs[new_scene].c_str()); pg_send_message_udp((char *)"s", AuxString, (char *)"udp_PD_send");
    }
  }
  // otherwise scans all the existing variables generated from scenario
  else {
    for( int indVar = 0 ; indVar < _MaxInterpVarIDs ; indVar ++ ) {
       if( strcmp( newCommand , CmdString[indVar] ) == 0 ) {
	  pg_update_variable(_PG_GUI_COMMAND, _PG_KEYSTROKE_VOID,
			    indVar, arguments[0] );
	  return;
       }
    }
  }
}

//////////////////////////////////////////////////////////////
// MISC LIB FUNCTIONS
//////////////////////////////////////////////////////////////

void pg_writeMessageOnScreen( char *text ) {
  strcpy( ScreenMessage , text ); 
  double newtime = RealTime(); 
  messageTransparency = 1.0; 
  LastDecayTime = newtime; 
  NewScreenMessage = true;
}

void pg_snapshot( char * type ) {
  if( strcmp( type , "svg" ) == 0 ) {
    pg_draw_scene( _Svg );
  }
  else if( strcmp( type , "png" ) == 0 ) {
    pg_draw_scene( _Png );
  }
  else if( strcmp( type , "jpg" ) == 0 ) {
    pg_draw_scene( _Jpg );
  }
  else {
    sprintf( ErrorStr , "Incorrect screenshot type (%s): expected svg or png or jpg!" , type ); ReportError( ErrorStr );
  }
}

void  pg_apply_new_activeDrawingStroke( void ) {
  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
  // +++ COMMON ACTIONS IN CASE OF DRAWING MODE CHANGE +++++++ 
  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
  drawing_start_frame = FrameNo; 
  mouse_x_prev = -1;
  mouse_y_prev = -1;
}
