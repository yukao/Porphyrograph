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

std::string project_name = "TVWinstall";

char AuxString[1024];

bool BrokenInterpolationVar[ _MaxInterpVarIDs ];
float InitialValuesInterpVar[ _MaxInterpVarIDs ];
float InitialValuesConfigurationVar[ _MaxConfigurationVarIDs ];

bool tracksSync = false;

#ifdef PG_WITH_CA
// CA on off
bool CA_onOff = true; // CA on off flag
#endif

/////////////////////////////////////////////////////////////////////////
// WORKING VARIABLES
/////////////////////////////////////////////////////////////////////////

#ifdef PG_WITH_CA
//////////////////////////////////////////////
// CA initialization
float initCA = 0.0f;
#endif

// ++++++++++++++++++++++ DRAWING MODE +++++++++++++++++++++
int activeDrawingStroke; // initially not drawing

// ++++++++++++++++++++++ MUSIC CONTROL ++++++++++++++++++++
// music control inputs received at each frame
// addition of color based on palette
float RGBpalette_received[4];

// per track replay interpolation
bool track_interp[PG_NB_TRACKS] = {track_interp_0_ini
#if PG_NB_TRACKS >= 2
				  ,track_interp_1_ini
#endif
#if PG_NB_TRACKS >= 3
				  ,track_interp_2_ini
#endif
				  };
bool track_stepwise_interpolation[PG_NB_TRACKS] = {track_stepwise_interpolation_0_ini
#if PG_NB_TRACKS >= 2
						  ,track_stepwise_interpolation_1_ini
#endif
#if PG_NB_TRACKS >= 3
						  ,track_stepwise_interpolation_2_ini
#endif
						  };
#ifdef PG_TRACK_DISPLAY
// per track display (target, source is in spreadsheet)
bool target_track_display[PG_NB_TRACKS];
#endif

// auxiliary
bool synchr_start_recording_source_track[PG_NB_TRACKS];
bool synchr_start_recording_target_track[PG_NB_TRACKS];
bool synchr_start_replay_track[PG_NB_TRACKS];

// track loading
int currentSVGTrack = 0;

// ++++++++++++++++++++++ FLASHES +++++++++++++++++++++++++
// flash: extension of particle reproduction
int flashPart;
#ifdef PG_WITH_CA
float flashCA_weights[PG_NB_TRACKS];
float flashBack_weight;
#endif
// flash: track to BG copy   -> values passed to the shader
// as a function of on/off values and weights
// only tracks 1 and 2 are concerned since it is a projection on track0
float flashTrack0_weights[PG_NB_TRACKS];

int flashCA_freq_0_saved = 0;
int flashCA_freq_1_saved = 0;

// +++++++++++++++++++++ VIDEO BACKGROUND SUBTRACTION +++++
bool is_currentBG_capture = false;
bool is_initialBG_capture = false;
bool is_firstVideoOn = true;

// +++++++++++++++++++++ VIDEO FLASH +++++++++++++++++++++
bool is_flashVideo = false;
float flashVideoCoef = 0.0f;
float flashVideoDecay = 1.0f;
float flashVideoThreshold = 0.5f;

// +++++++++++++++++++++ CLEAR +++++++++++++++++++++++++++
#ifdef PG_WITH_CA
int isClearCA;
#endif
int isClearLayer;
int isClearAllLayers;

// +++++++++++++++++++++ NEW BEAT +++++++++++++++++++++++++++
int isNewBeat;

// +++++++++++++++++++++ COPY LAYERS +++++++++++++++++++++++++++
int isCopyTrack12;

// ++++++++++++++++++++++ SET-UP +++++++++++++++++++++++++++
int CurrentScene = -1;
int CurrentHashtag = -1;

// scene management
bool FirstFrameInScene = true;
float remainingTimeInScene = 0.0f;

// working variables
int drawing_start_frame;

// SCENARIO LAUNCH TIME
double InitialScenarioTime;
// PERFORMANCE LAUNCH TIME
double InitialPerformanceTime = -1;

// +++++++++++++++++++++++ CLEAR AND INVERT +++++++++++++++++++++++++++++++++
double lastClearTime = 0.0;

// +++++++++++++++++++++++ Beats +++++++++++++++++++++++++++++++++
int BeatNo = 1;
double lastAutoBeatTime = 0.0;
double lastReceivedBeatTime = 0.0;
float pulse[3] = { 0.0f };

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
			*((int *)ConfigurationVarPointers[indVar]) = (int)round(InitialValuesConfigurationVar[indVar]);
		}
		else if (ConfigurationVarTypes[indVar] == _pg_bool) {
			*((bool *)ConfigurationVarPointers[indVar]) = (InitialValuesConfigurationVar[indVar] > 0 ? true : false);
		}
		else if (ConfigurationVarTypes[indVar] == _pg_sign) {
			*((float *)ConfigurationVarPointers[indVar]) = (InitialValuesConfigurationVar[indVar] > 0 ? 1.0f : -1.0f);
		}
		else if (ConfigurationVarTypes[indVar] == _pg_path) {
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
	      (GLfloat)leftWindowWidth , (GLfloat)window_height );
  // printf("VP window size %d %d\n",leftWindowWidth , window_height);

  /////////////////////////////////////////////////////////////////////////
  // SCENARIO-CONTROLLED UNIFORM VARIABLES
  for( int indVar = 0 ; indVar < _MaxInterpVarIDs ; indVar++ ) {
      if (ScenarioVarTypes[indVar] == _pg_float) {
	    *((float *)ScenarioVarPointers[indVar]) = InitialValuesInterpVar[indVar];
      }
      else if (ScenarioVarTypes[indVar] == _pg_int) {
	    *((int *)ScenarioVarPointers[indVar]) = (int)round(InitialValuesInterpVar[indVar]);
      }
      else if (ScenarioVarTypes[indVar] == _pg_bool) {
	    *((bool *)ScenarioVarPointers[indVar]) = (InitialValuesInterpVar[indVar] > 0 ? true : false);
      }
      else if (ScenarioVarTypes[indVar] == _pg_sign) {
	    *((float *)ScenarioVarPointers[indVar]) = (InitialValuesInterpVar[indVar] > 0 ? 1.0f : -1.0f);
      }
      else if (ScenarioVarTypes[indVar] == _pg_path) {
	    *((bool *)ScenarioVarPointers[indVar]) = (InitialValuesInterpVar[indVar] > 0 ? true : false);
      }
	  else {
		  sprintf(ErrorStr, "Incorrect variable type (%d) for scenario variable #%d!", ScenarioVarTypes[indVar] , indVar); ReportError(ErrorStr);
	  }
  }

  // current drawing mode for display
  activeDrawingStroke = activeDrawingStroke_ini; // off drawing
  pg_apply_new_activeDrawingStroke();
  // track replay
  // source and target track recording
  // is recording source -> has to stop recording source
  if( track_replay_0 ) {
      if (tracksSync) {
	      synchr_start_replay_track[0] = true;
      }
      else {
	      pg_track_replay_onOff(0);
      }
  }
#if PG_NB_TRACKS >= 2
  if( track_replay_1 ) {
      if (tracksSync) {
	      synchr_start_replay_track[1] = true;
      }
      else {
	      pg_track_replay_onOff(1);
      }
  }
#endif
#if PG_NB_TRACKS >= 3
  if( track_replay_2 ) {
      if (tracksSync) {
	      synchr_start_replay_track[2] = true;
      }
      else {
	      pg_track_replay_onOff(2);
      }
  }
#endif

  // track recording
  // source track recording
  // is recording source -> has to stop recording source
  if( !source_track_record_0 ) {
      pg_source_track_recording_stop( 0 );
  }
#if PG_NB_TRACKS >= 2
  if( !source_track_record_1 ) {
      pg_source_track_recording_stop( 1 );
  }
#endif
#if PG_NB_TRACKS >= 3
  if( !source_track_record_2 ) {
      pg_source_track_recording_stop( 2 );
  }
#endif

  /////////////////////////////////////////////////////////////////////////
  // NON SCENARIO-CONTROLLED UNIFORM VARIABLES (COULD BECOME CONTROLLED BY FUTURE SCENARIOS)
  // target track display on/off
  for( int indTrack = 0 ; indTrack < PG_NB_TRACKS ; indTrack++ ) {
#ifdef PG_TRACK_DISPLAY
    target_track_display[indTrack] = false;
#endif

    // is interpolating -> has to stop interpolating
    if( track_interp[ indTrack ] ) {
      track_interp[ indTrack ] = false;
      pg_track_interpolate_stop( indTrack );
    }
    // is making smooth interpolation -> has to go to stepwise interpolation
    if( !track_stepwise_interpolation[ indTrack ] ) {
      track_stepwise_interpolation[ indTrack ] = true;
      pg_track_interpolate_step_smooth( 0 , true );
    }
  }

  /////////////////////////////////////////////////////////////////////////
  // SOUND-CONTROLLED UNIFORM VARIABLES: dynamic values received from PD
  // flashes
  for( int indtrack = 0 ; indtrack < PG_NB_TRACKS ; indtrack++ ) {
#ifdef PG_WITH_CA
	  flashCA_weights[indtrack] = 0;
#endif
	  flashTrack0_weights[indtrack] = 0;
  }
  flashPart = 0;
#ifdef PG_WITH_CA
  flashBack_weight = 0;
#endif

  // background subraction
  is_currentBG_capture = false;
  is_initialBG_capture = false;

  // video flash
  is_flashVideo = false;
  flashVideoCoef = 0.0f;

  // framebuffer memory
  //is_capturedFB = false;

  // clear
#ifdef PG_WITH_CA
  isClearCA = 0;
#endif
  isClearLayer = 0;
  isClearAllLayers = 0;

  // new beat reset
  isNewBeat = 0;

  // copy
  isCopyTrack12 = 0;

  // pen music color
  RGBpalette_received[0] = RGBpalette_r_ini;
  RGBpalette_received[1] = RGBpalette_g_ini;
  RGBpalette_received[2] = RGBpalette_b_ini;
  RGBpalette_received[3] = RGBpalette_a_ini;

  /////////////////////////////////////////////////////////////////////////
  // DRAWING-CONTROLLED UNIFORM VARIABLES: dynamic values received from drawing or track display
  // pen position storage on the two quads
  for( int indTrack = 0 ; indTrack < PG_NB_TRACKS ; indTrack++ ) {
    tracks_x[indTrack] = -1.0F;
    tracks_y[indTrack] = -1.0F;
    tracks_x_prev[indTrack] = -1.0F;
    tracks_y_prev[indTrack] = -1.0F;
    // pen color
    tracks_Color_r[indTrack] = 0.0F;
    tracks_Color_g[indTrack] = 0.0F;
    tracks_Color_b[indTrack] = 0.0F;
    tracks_Color_a[indTrack] = 1.0F;
    // pen brush & size
    tracks_BrushID[indTrack] = 0;
    tracks_RadiusX[indTrack] = 0.0F;
    tracks_RadiusY[indTrack] = 0.0F;
  }
  // hoover cursor
  CurrentCursorHooverPos_x = 0;
  CurrentCursorHooverPos_y = 0;
  // stylus type
  CurrentCursorStylusvsRubber = pg_Stylus;

  /////////////////////////////////////////////////////////////////////////
  // TIME CONTROLLED VARIABLES (NOT FORWARDED TO SHADERS)
  // scenario management
  CurrentScene = -1;
  CurrentHashtag = -1;

  /////////////////////////////////////////////////////////////////////////
  // SVG TRACK LOADING
  load_svg_track( (char *)("./ProjectsBefore18/" + project_name + "/textures/SVGs/fall_src_01_01.svg").c_str() ,
		  1 , true );
  load_svg_track( (char *)("./ProjectsBefore18/" + project_name + "/textures/SVGs/fall_src_02_01.svg").c_str() ,
		  2 , true );
  load_svg_track( (char *)("./ProjectsBefore18/" + project_name + "/textures/SVGs/fall_tgt_01_01.svg").c_str() ,
		  1 , false );
  load_svg_track( (char *)("./ProjectsBefore18/" + project_name + "/textures/SVGs/fall_tgt_02_01.svg").c_str() ,
		  2 , false );


  /////////////////////////////////////////////////////////////////////////
  // INTERFACE INITIALIZATION
  pg_send_message_udp( (char *)"i i" , (char *)"/connect" , (char *)"udp_PD_send" );

  // sprintf( AuxString , "/input_volume_can %.5f" , input_volume ); pg_send_message_udp( (char *)"f" , AuxString , (char *)"udp_PD_send" );

  pg_displaySceneVariables();

  pg_send_message_udp( (char *)"s" , (char *)"/message_can INIT_COMPLETED" , (char *)"udp_PD_send" );
}

///////////////////////////////////////////////////////////////////////////////////
// GUI DISPLAY & LOG FILE LOGGING
/////////////////////////////////////////////////////////////
void pg_displaySceneVariables(void) {
	if (FrameNo % 20 == 0) {
		//printf("display scene variables @ Frame %d\n",
		//	FrameNo);
		/////////////////////////////////////////////////////////////////////////
		// NON AUTOMATIC DISPLAY OF VARIABLES
		sprintf(AuxString, "/drawing_can %d", activeDrawingStroke); pg_send_message_udp((char *)"f", AuxString, (char *)"udp_PD_send");

		/////////////////////////////////////////////////////////////////////////
		// SCENARIO-CONTROLLED UNIFORM VARIABLES
		for (int indVar = 0; indVar < _MaxInterpVarIDs; indVar++) {
			if (*(ScenarioVarMessages[indVar])) {
				if (ScenarioVarTypes[indVar] == _pg_float) {
					sprintf(AuxString, "/%s %.4f", ScenarioVarMessages[indVar], *((float *)ScenarioVarPointers[indVar])); pg_send_message_udp((char *)"f", AuxString, (char *)"udp_PD_send");
				}
				else if (ScenarioVarTypes[indVar] == _pg_int) {
					sprintf(AuxString, "/%s %d", ScenarioVarMessages[indVar], *((int *)ScenarioVarPointers[indVar])); pg_send_message_udp((char *)"f", AuxString, (char *)"udp_PD_send");
				}
				else if (ScenarioVarTypes[indVar] == _pg_bool) {
					sprintf(AuxString, "/%s %d", ScenarioVarMessages[indVar], (int)(*((bool *)ScenarioVarPointers[indVar]))); pg_send_message_udp((char *)"f", AuxString, (char *)"udp_PD_send");
				}
				else if (ScenarioVarTypes[indVar] == _pg_sign) {
					sprintf(AuxString, "/%s %.4f", ScenarioVarMessages[indVar], *((float *)ScenarioVarPointers[indVar])); pg_send_message_udp((char *)"f", AuxString, (char *)"udp_PD_send");
				}
				else if (ScenarioVarTypes[indVar] == _pg_path) {
					sprintf(AuxString, "/%s %d", ScenarioVarMessages[indVar], (int)(*((bool *)ScenarioVarPointers[indVar]))); pg_send_message_udp((char *)"f", AuxString, (char *)"udp_PD_send");
				}
			}
		}

		/////////////////////////////////////////////////////////////////////////
		// TIME CONTROLLED VARIABLES (NOT FORWARDED TO SHADERS)
		// time display
		sprintf(AuxString, "/time_can %d:%d", (int)remainingTimeInScene / 60, (int)remainingTimeInScene % 60); pg_send_message_udp((char *)"s", AuxString, (char *)"udp_PD_send");
		float time_since_launch = float(CurrentClockTime - InitialPerformanceTime);
		sprintf(AuxString, "/cumulated_time_can %d:%d", (int)time_since_launch / 60, (int)time_since_launch % 60); pg_send_message_udp((char *)"s", AuxString, (char *)"udp_PD_send");
		// printf("time %s\n", AuxString );
		if (remainingTimeInScene < 10) {
			sprintf(AuxString, "/time_color_can 0"); pg_send_message_udp((char *)"i", AuxString, (char *)"udp_PD_send");
		}
		else if (remainingTimeInScene < 20) {
			sprintf(AuxString, "/time_color_can 1"); pg_send_message_udp((char *)"i", AuxString, (char *)"udp_PD_send");
		}
		else if (remainingTimeInScene < 30) {
			sprintf(AuxString, "/time_color_can 2"); pg_send_message_udp((char *)"i", AuxString, (char *)"udp_PD_send");
		}
		else {
			sprintf(AuxString, "/time_color_can 3"); pg_send_message_udp((char *)"i", AuxString, (char *)"udp_PD_send");
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

//////////////////////////////////////////////////////////////////////////////////////
// LOG FILE
// makes a line of a CSV file to be used as a starting point for a repetition of the
// setup

void pg_logCurrentLineSceneVariables(char *fileName) {
	/////////////////////////////////////////////////////////////////////////
	// SCENARIO-CONTROLLED UNIFORM VARIABLES
	for (int indVar = 0; indVar < _MaxInterpVarIDs; indVar++) {
		if (ScenarioVarTypes[indVar] == _pg_float) {
			fprintf(fileLog, "\t%.6f", *((float *)ScenarioVarPointers[indVar]));
		}
		else if (ScenarioVarTypes[indVar] == _pg_int) {
			fprintf(fileLog, "\t%d", *((int *)ScenarioVarPointers[indVar]));
		}
		else if (ScenarioVarTypes[indVar] == _pg_bool) {
			fprintf(fileLog, "\t%d", int(*((bool *)ScenarioVarPointers[indVar])));
		}
		else if (ScenarioVarTypes[indVar] == _pg_sign) {
			fprintf(fileLog, "\t%d", (*((float *)ScenarioVarPointers[indVar]) > 0 ? +1 : -1));
		}
		else if (ScenarioVarTypes[indVar] == _pg_path) {
			fprintf(fileLog, "\t%d", int(*((bool *)ScenarioVarPointers[indVar])));
		}
	}
	fprintf(fileLog, "\t%s\n", fileName);
}

// makes the first line of a CSV log file
void pg_logFirstLineSceneVariables(void) {
	/////////////////////////////////////////////////////////////////////////
	// SCENARIO-CONTROLLED UNIFORM VARIABLES
	fprintf(fileLog, "ID");
	for (int indVar = 0; indVar < _MaxInterpVarIDs; indVar++) {
		fprintf(fileLog, "\t%s", CmdString[indVar]);
	}
	fprintf(fileLog, "\n");
}


///////////////////////////////////////////////////////////////////////////////////
// SCENARIO BASED COMMANDS
///////////////////////////////////////////////////////////////////////////////////
void videoExposure_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
#ifdef PG_VIDEO_ACTIVE
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO ) {
		// sprintf(AuxString, "/message_can Exposure_%.1f", scenario_or_gui_command_value); pg_send_message_udp((char *)"s", (char *)AuxString, (char *)"udp_PD_send");
		pg_input_camera.set(CV_CAP_PROP_EXPOSURE, scenario_or_gui_command_value);
		pg_input_camera.set(CV_CAP_PROP_FPS, 60);
	}
#endif
}
void is_videoOn_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
#ifdef PG_VIDEO_ACTIVE
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_KEYSTROKE || param_input_type == _PG_SCENARIO) {
		if (is_videoOn && is_firstVideoOn) {
			is_currentBG_capture = true;
			is_initialBG_capture = true;
			is_firstVideoOn = false;
			// printf("is_currentBG_capture\n" );
			// printf("is_initialBG_capture\n" );
		}
	}
#endif
}
void flashVideoLength_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
#ifdef PG_VIDEO_ACTIVE
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_KEYSTROKE || param_input_type == _PG_SCENARIO) {
		if (scenario_or_gui_command_value > 0.) {
			flashVideoDecay = 1.f / scenario_or_gui_command_value;
		}
		else {
			flashVideoDecay = 0.001f;
		}
	}
#endif
}
void partAccCenter_0_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_KEYSTROKE) {
		sprintf(AuxString, "/message_can part_center_H_%.1f", scenario_or_gui_command_value); pg_send_message_udp((char *)"s", (char *)AuxString, (char *)"udp_PD_send");
	}
}
void partAccCenter_1_callBack( pg_Parameter_Input_Type param_input_type , float scenario_or_gui_command_value) {
	if(param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_KEYSTROKE) {
		sprintf( AuxString , "/message_can part_center_V_%.1f" , scenario_or_gui_command_value ); pg_send_message_udp( (char *)"s" , (char *)AuxString , (char *)"udp_PD_send" );
	}
}
void clearAllLayers_callBack( pg_Parameter_Input_Type param_input_type , float scenario_or_gui_command_value) {
    if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_KEYSTROKE) {
	  // printf("delta %.3f\n", (float)(CurrentClockTime - lastClearTime) );
	  if( CurrentClockTime - lastClearTime  < 0.5 ) {
		  pg_send_message_udp((char *)"s", (char *)"/message_can ***CLEAR_ALL_LAYERS***", (char *)"udp_PD_send");
		  isClearAllLayers = 1;
		  lastClearTime = 0.0;
		  // printf("****** isClearAllLayers %d\n",isClearAllLayers);
	  }
	  else {
		  lastClearTime = CurrentClockTime;
	  }
    }
    else if (param_input_type == _PG_SCENARIO && scenario_or_gui_command_value > 0.0 ) {
	if (scenario_or_gui_command_value > 0) {
	    isClearAllLayers = 1;
            lastClearTime = CurrentClockTime;
	  }
	}
}
#ifdef PG_WITH_CA
void clearCA_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_KEYSTROKE) {
		pg_send_message_udp((char *)"s", (char *)"/message_can clear_CA", (char *)"udp_PD_send");
		isClearCA = 1;
	}
	else if (param_input_type == _PG_SCENARIO  && scenario_or_gui_command_value > 0.0) {
		isClearCA = 1;
	}
}
#else
void clearCA_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_KEYSTROKE) {
		pg_send_message_udp((char *)"s", (char *)"/message_can clear_CA", (char *)"udp_PD_send");
	}
	else if (param_input_type == _PG_SCENARIO  && scenario_or_gui_command_value > 0.0) {
	}
}
#endif
void freeze_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_KEYSTROKE) {
		if (freeze) {
			pg_send_message_udp((char *)"s", (char *)"/message_can freeze", (char *)"udp_PD_send");
		}
	}
	else if (param_input_type == _PG_SCENARIO ) {
		if (freeze) {
			pg_send_message_udp((char *)"s", (char *)"/message_can freeze", (char *)"udp_PD_send");
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
	// save previous values of variables that have to know their previous value in their callback
	//float step = 0.0f;
	char mess[50];

	//if (parm_keystroke_type == _PG_KEYSTROKE_MINUS) {
	//	step = StepMinus[indParam];

	//	sprintf(mess, "%.2f %s",
	//		*((float *)ScenarioVarPointers[indParam])+StepMinus[indParam], CmdString[indParam]);
	//	printf("%s\n", mess);
	//	pg_writeMessageOnScreen((char *)mess);
	//}
	//else if (parm_keystroke_type == _PG_KEYSTROKE_PLUS) {
	//	step = StepPlus[indParam];

	//	sprintf(mess, "%.2f %s",
	//		*((float *)ScenarioVarPointers[indParam]) + StepMinus[indParam], CmdString[indParam]);
	//	printf("%s\n", mess);
	//	pg_writeMessageOnScreen((char *)mess);
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
			*((int *)ScenarioVarPointers[indParam]) = (int)round(scenario_or_gui_command_value);
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
	else if (ScenarioVarTypes[indParam] == _pg_path) {
		if (ScenarioVarCallbacks[indParam]) {
			(*ScenarioVarCallbacks[indParam])(param_input_type,scenario_or_gui_command_value);
		}
	}
}

void pg_update_scene_number_for_visuals(bool new_scene ) {
	// all scenes except first and last
	// the first scene is not special in case of relaunch
    printf("scene / hashtag %d %d\n", CurrentScene , CurrentHashtag);

    // IMAGE CHOICE AND SWAP INIT
    // sets the directory for images
    // the first scene starts with 6 preloaded images
    if (CurrentHashtag == 0)
        IndPlayedImageFile = 6;
    else
        IndPlayedImageFile = 0;

    // sets the start index for available image layer to the first layer
    IndInitialSwapImage = 0;

    // TEXT CHOICE AND SWAP INIT
    // sets the index for messages
    // initialization (first message to be displayed)
    if (TextChapter == -1) {
        DisplayText1Front = false;
        DisplayText1 = "";
        DisplayText2 = "";
    }
    TextChapter = CurrentHashtag;
    // last hashtage is played single in the end
    if( TextChapter == PG_NB_HASHTAGS) {
        TextChapter = PG_NB_HASHTAGS - 1;
    }

    if (DisplayText1Front) {
        IndDisplayText2 = DisplayTextFirstInChapter[TextChapter];
        LengthDisplayText2 = (float)pg_displayMessage_upload(2);
        IndDisplayText1 = DisplayTextFirstInChapter[TextChapter] + 1;
        LengthDisplayText1 = (float)pg_displayMessage_upload(1);
        DisplayText1Front = false;
        DisplayText1Alpha = 1.0f;
        DisplayText2Alpha = 0.0f;
    }
    else {
        IndDisplayText1 = DisplayTextFirstInChapter[TextChapter];
        LengthDisplayText1 = (float)pg_displayMessage_upload(1);
        IndDisplayText2 = DisplayTextFirstInChapter[TextChapter] + 1;
        LengthDisplayText2 = (float)pg_displayMessage_upload(2);
        DisplayText1Front = true;
        DisplayText1Alpha = 0.0f;
        DisplayText2Alpha = 1.0f;
    }
    DisplayTextSwapInitialTime = CurrentClockTime;
}

void pg_update_scenario(void) {
	// possible scene advance
	float elapsed_time_from_start = (float)(CurrentClockTime - InitialScenarioTime);
	int hashtagFromElapsedTime;
	hashtagFromElapsedTime = (int(elapsed_time_from_start) / subscene_duration) % (PG_NB_HASHTAGS + 1);
	if (hashtagFromElapsedTime != CurrentHashtag ) {
		CurrentHashtag = hashtagFromElapsedTime;
		pg_update_scene_number_for_visuals(false);
	}
	pg_hashtag_upload();

    // all variables are assigned initial value
    if (FirstFrameInScene) {
        for (int indP = 0; indP < _MaxInterpVarIDs; indP++) {
            pg_update_variable(_PG_SCENARIO, _PG_KEYSTROKE_VOID,
                indP, scene_initial_parameters[0][indP]);
        }
    }
    FirstFrameInScene = false;
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
    // printf( "FPS display %d\n" , DisplayFramePerSecond );
    break;

  /* ------------------------------- snapshot */
  case 's':
    pg_draw_scene( _Jpg );
    break;
  case 'S':
    pg_draw_scene( _Svg );
    break;

  /* ------------------------------- no more automatic snapshots */
  case 'n':
    is_automatic_snapshots = false;
    pg_send_message_udp( (char *)"s" , (char *)"/message_can NO_MORE_SNAPSHOTS" , (char *)"udp_PD_send" );
    break;

  /* ------------------------------- beat */
  case 'b':
    auto_beat = !auto_beat;
    lastAutoBeatTime = CurrentClockTime;
    break;

  /* ------------------------------- current video background capture */
  case 'v':
	  is_currentBG_capture = true;
	  break;

  /* ------------------------------- initial video background capture */
  case 'w':
	  is_initialBG_capture = true;
	  break;

  ///* ------------------------------- frame buffer capture */
  //case 'c':
	 // is_capturedFB = true;
	 // break;

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

void pg_launch_performance(void) {
#ifdef PG_WITH_CA
	isClearCA = 1; // clean CA layer
	initCA = 1.0f;
#endif
	isClearAllLayers = 1; // clears bg layer

    // captures the video background
	is_currentBG_capture = true;
	is_initialBG_capture = true;

	restoreInitialTimesAndDurations();
	// since it is Lola who starts, should not send back a launch command... otherwise there is a ping pong effect
	// sprintf(AuxString, "/launch %f", CurrentClockTime); pg_send_message_udp((char *)"f", AuxString, (char *)"udp_SC_send");
	pg_send_message_udp((char *)"s", (char *)"/message_can START", (char *)"udp_PD_send");
	InitialScenarioTime = CurrentClockTime - scene_initial_times[0];
	InitialPerformanceTime = InitialScenarioTime;

	// reinitialization of the interpolation control variables at the beginning of a new scene
	for (int indVar = 0; indVar < _MaxInterpVarIDs; indVar++) {
		BrokenInterpolationVar[indVar] = false;
	}
}

void pg_keyStrokeScripts(int key) {
  pg_Keystroke_Input_Type CommandType = _PG_KEYSTROKE_VOID;
  //char *CmdChar = NULL;
  //int indChar = -1;
  //bool oneCommandIssued = false;

  //char * ptCh = CmdCharMinus;
  //while ((CmdChar = strchr(ptCh,key)) != NULL) {
	 // CommandType = _PG_KEYSTROKE_MINUS;
	 // indChar = CmdChar - CmdCharMinus;
	 // if (indChar >= 0) {
		//  pg_update_variable(_PG_KEYSTROKE, CommandType,
		//		    indChar, 0.0F );
		//  oneCommandIssued = true;
	 // }
	 // ptCh = CmdChar + 1;
  //}
  //if( oneCommandIssued ) {
  //  return;
  //}
  //ptCh = CmdCharPlus;
  //if ((CmdChar = strchr(ptCh,key)) != NULL) {
	 // CommandType = _PG_KEYSTROKE_PLUS;
	 // indChar = CmdChar - CmdCharPlus;
	 // if (indChar >= 0) {
		//  pg_update_variable(_PG_KEYSTROKE, CommandType,
		//		    indChar, 0.0F );
		//  oneCommandIssued = true;
	 // }
	 // ptCh = CmdChar + 1;
  //}
  //if( oneCommandIssued ) {
  //  return;
  //}

  // printf("indChar %d part_acc_factor %d\n", indChar, _part_acc_factor);

  switch (key) {

    // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // +++++++++++++++++ TEST UDP ++++++++++++++++++++++++++++++
    // +++++++++++++++++   keystroke (T)  ++++++++++++++++++++++
    // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  case'T':
    pg_writeMessageOnScreen( (char * )"******" );
    // pg_send_message_udp( (char *)"f" , (char *)"/testUDP 0" , (char *)"udp_SC" );
    sprintf( AuxString , "/message_can TEST_UDP" ); pg_send_message_udp( (char *)"s" , AuxString , (char *)"udp_PD_send" );
    sprintf( AuxString , "/return_message returnUPD_%f" , CurrentClockTime ); pg_send_message_udp( (char *)"s" , AuxString , (char *)"udp_PD_send" );
    break;

    // ========================================
    // background subtraction modes (Z)
    // ========================================
  case'y':
     camera_BG_subtr = (camera_BG_subtr + 1) % 3;
     printf("BGstbtr %d\n",camera_BG_subtr);
     BrokenInterpolationVar[ _camera_BG_subtr ] = true;
    break;

    // ========================================
    // loop size (number of beats in each loop) (V)
    // ========================================
  case'V':
    flashPart_freq = ( flashPart_freq < PG_LOOP_SIZE ? 2 * flashPart_freq : 1 );
    // printf( "Loop size (PG_LOOP_SIZE)\n" );
    BrokenInterpolationVar[ _flashPart_freq ] = true;
#ifdef PG_WITH_CA
	BrokenInterpolationVar[_flashCA_freq_0] = true;
#if PG_NB_TRACKS >= 2
	BrokenInterpolationVar[_flashCA_freq_1] = true;
#endif
#if PG_NB_TRACKS >= 3
	BrokenInterpolationVar[_flashCA_freq_2] = true;
#endif
#endif
    break;

    // ========================================
    // Track / target interpolation start/stop (()
    // ========================================
  case'(':
    track_interp[ 0 ] = !track_interp[ 0 ];
    if( track_interp[ 0 ] ) {
      pg_track_interpolate_start( 0 );
    } else {
      pg_track_interpolate_stop( 0 );
    }
    break;

    // ========================================
    // Track / target stepwise vs smooth interpolation (Y)
    // ========================================
  case'Y':
    track_stepwise_interpolation[ 0 ]
      = !track_stepwise_interpolation[ 0 ];
    pg_track_interpolate_step_smooth( 0 ,
      track_stepwise_interpolation[ 0 ] );
    break;

  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  // +++++++++++++++++ FLASHES BETWEEN LAYERS & PARTICLES ++++
  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  // flashPart          (f)
  // flashPart duration (Q)
  // ======================================
  case'f':
    flashPart = flashPart_duration;
    break;

#ifdef PG_WITH_CA
  case'h':
	  flashCA_weights[0] = 1.0;
	  break;

  case'H':
	  flashBack_weight = 1.0;
	  break;
#endif

  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  // +++++++++++++++++ DRAWING ON/OFF ++++++++++++++++++++++++
  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  // drawing on off void/currentDrawing mode (w)
  // =============================================
  case'w':
    if( activeDrawingStroke == 0 ) {
      activeDrawingStroke = 1;
    } else {
      activeDrawingStroke = 0;
    }
    pg_apply_new_activeDrawingStroke();
    break;
  }
}

void pg_continuous_flahes( void ) {
    // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // +++++++++++++++++ FLASHES +++++++++++++++++++++++++++++++
    // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // ========================================
    // flash triggering according to beat & frequency, weight, and on/off values
    // ========================================
#ifdef PG_WITH_CA
	if (flashCA_freq_0 > PG_LOOP_SIZE) { // if flashCA_freq_0 > PG_LOOP_SIZE -> update every frame
		flashCA_weights[0] = 1.0;
		// printf( "flashCA (%d)\n" , flashCA_freq_0 );
}
#if PG_NB_TRACKS >= 2
	if (flashCA_freq_1 > PG_LOOP_SIZE) { // if flashCA_freq_1 > PG_LOOP_SIZE -> update every frame
		flashCA_weights[1] = 1.0;
		// printf( "flashCA (%d)\n" , flashCA_freq_1 );
	}
#endif
#if PG_NB_TRACKS >= 3
	if (flashCA_freq_2 > PG_LOOP_SIZE) { // if flashCA_freq_2 > PG_LOOP_SIZE -> update every frame
		flashCA_weights[2] = 1.0;
		// printf( "flashCA (%d)\n" , flashCA_freq_2 );
	}
#endif
#endif

	// flashes are synchronized on beats according to their frequency
    if( flashPart_freq > PG_LOOP_SIZE ) { // if flashPart_freq > PG_LOOP_SIZE -> update every frame
      flashPart = flashPart_duration;
    }

#if PG_NB_TRACKS >= 2
    if( flashTrack0_freq > PG_LOOP_SIZE ) { // if flashTrack0_freq > PG_LOOP_SIZE -> update every frame
        // should only be flashed every second frame because of the way the particles are spreaded
        // otherwise the particles are not emitted
	if( FrameNo %2 == 0 ) {
	  flashTrack0_weights[1] = 1.0;
	}
	//printf( "flashTrack0_weights 1 (%.2f)\n" , flashTrack0_weights[1] );
    }
#endif
#if PG_NB_TRACKS >= 3
    if( flashTrack0_freq_2 > PG_LOOP_SIZE ) { // if flashTrack0_freq_2 > PG_LOOP_SIZE -> update every frame
        // should only be flashed every second frame because of the way the particles are spreaded
        // otherwise the particles are not emitted
	if( FrameNo %2 == 0 ) {
	  flashTrack0_weights[2] = 1.0;
	}
	//printf( "flashTrack0_weights 2 (%.2f)\n" , flashTrack0_weights[2] );
    }
#endif

#ifdef PG_WITH_CA
    if( flashBack_freq > PG_LOOP_SIZE ) { // if flashBack_freq > PG_LOOP_SIZE -> update every frame
      flashBack_weight = 1.0;
      // printf( "flashBack (%d)\n" , flashCA );
    }
#endif
}

///////////////////////////////////////////////////////////////////////////////////
// UDP BASED COMMANDS
///////////////////////////////////////////////////////////////////////////////////

void firstSceneSetup(void) {
    InitialPerformanceTime = InitialScenarioTime;
	FirstFrameInScene = true;
	sprintf(AuxString, "/setup_can %s", scene_IDs[CurrentScene].c_str()); pg_send_message_udp((char *)"s", AuxString, (char *)"udp_PD_send");
	printf("/setup_can %d %s\n", CurrentScene, scene_IDs[CurrentScene].c_str());

	restoreInitialTimesAndDurations();
	InitialScenarioTime = CurrentClockTime - scene_initial_times[CurrentScene];

	// reinitialization of the interpolation control variables at the beginning of a new scene
	for (int indVar = 0; indVar < _MaxInterpVarIDs; indVar++) {
		BrokenInterpolationVar[indVar] = false;
	}

	// first hashtag
	CurrentHashtag = 0;
	pg_update_scene_number_for_visuals(true);

	// stops ongoing flashes if there is one
	flashVideoCoef = 0.0f;
}

void pg_aliasScript(char *command_symbol,
	char* string_argument_0,
	float arguments[MAX_OSC_ARGUMENTS]) {
	char newCommand[512];

	strcpy(newCommand, command_symbol);

	// compares message tag with known command tags

	// ======================================
	// test UDP connection
	// ======================================
	if (strcmp(newCommand, "testUDP") == 0) {
		sprintf(AuxString, "******"); pg_writeMessageOnScreen(AuxString);
		sprintf(AuxString, "/message_can TEST_UDP"); pg_send_message_udp((char *)"s", AuxString, (char *)"udp_PD_send");
		sprintf(AuxString, "/return_message **OK**"); pg_send_message_udp((char *)"s", AuxString, (char *)"udp_PD_send");

	}
	else if (strcmp(newCommand, "return_message") == 0) {
		pg_writeMessageOnScreen(string_argument_0);

		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		// +++++++++++++++++ VIDEO CAPTURE ++++++++++++++++++++++++
		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	}
	else if (strcmp(newCommand, "reset_camera") == 0) {
		is_currentBG_capture = true;
		pg_send_message_udp((char *)"s", (char *)"/message_can CURRENT_BG_CAPTURE", (char *)"udp_PD_send");

	}
	else if (strcmp(newCommand, "initialBGCapture") == 0) {
		is_initialBG_capture = true;
		pg_send_message_udp((char *)"s", (char *)"/message_can INITIAL_BG_CAPTURE", (char *)"udp_PD_send");


		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		// +++++++++++++++++ VIDEO FLASH ++++++++++++++++++++++++
		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	}
	else if (strcmp(newCommand, "flashVideo") == 0) {
		is_flashVideo = true;
		flashVideoCoef = 0.0f;
		// printf("start flash\n");

		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		// +++++++++++++++++ MUSIC CONTROL +++++++++++++++++++++++++
		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	}
	else if (strcmp(newCommand, "RGBspectrum") == 0) {
		RGBpalette_received[0] = arguments[0];
		RGBpalette_received[1] = arguments[1];
		RGBpalette_received[2] = arguments[2];
		RGBpalette_received[3] = 0;
		float value = (RGBpalette_received[0] < RGBpalette_received[1]) ? RGBpalette_received[1] : RGBpalette_received[0];
		value = (value < RGBpalette_received[2]) ? RGBpalette_received[2] : value;
		// printf( "RGBpalette_received: %f %f %f\n" , RGBpalette_received[0] , RGBpalette_received[1] , RGBpalette_received[2] );
	}
	else if (strcmp(newCommand, "pulse_spectrum") == 0) {
		pulse[0] = arguments[0];
		pulse[1] = arguments[1];
		pulse[2] = arguments[2];

		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		// +++++++++++++++++ BEAT: MUSCI TEMPO SYNC ++++++++++++++++
		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		// FlashPart (f)
		// BackFlash (L)
		// ======================================
	}
	else if (strcmp(newCommand, "beat") == 0) {
		int beatNo = (int)round(arguments[0]);
		sprintf(AuxString, "/loop_beat_can %d", beatNo % PG_LOOP_SIZE); pg_send_message_udp((char *)"i", AuxString, (char *)"udp_PD_send");
		// printf("beat no %d\n", beatNo);
		// sets new beat to inform shader
		isNewBeat = 1;
		// counts beat time
		lastReceivedBeatTime = CurrentClockTime;

		// loop start
		if ((beatNo % PG_LOOP_SIZE) == 0) {

			// source track recording is on -> source track start
			if (synchr_start_recording_source_track[0]) {
				pg_source_track_recording_onOff(0);
				// sets to 0 the recording message since recording has started
				synchr_start_recording_source_track[0] = false;
			}

			// target track reading is on -> target read start
			if (synchr_start_replay_track[0]) {
				pg_track_replay_onOff(0);
				synchr_start_replay_track[0] = false;
			}
		}

		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		// video flash
		if (is_flashVideo
			&& ((beatNo % PG_LOOP_SIZE) == flashVideoBeat || flashVideoBeat == 9)) {
			is_flashVideo = false;
			flashVideoCoef = 1.0f;
			flashVideoThreshold = std::min(std::max(0.0f, 1.0f - flashVideoDecay * flashVideoBright),1.0f);
			// printf("decay %.3f bright dur. %d threshold %.2f\n", flashVideoDecay, int(flashVideoBright), flashVideoThreshold);
		}

		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		// +++++++++++++++++ FLASHES +++++++++++++++++++++++++++++++
		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		// ========================================
		// flash triggering according to beat & frequency, weight, and on/off values
		// ========================================
		// flashes are synchronized on beats according to their frequency
		if (flashPart_freq > 0
			&& flashPart_freq <= PG_LOOP_SIZE // if flashPart_freq > PG_LOOP_SIZE -> update every frame
			&& (beatNo % (PG_LOOP_SIZE / flashPart_freq)) == 0) {
			flashPart = flashPart_duration;
		}

#if PG_NB_TRACKS >= 2
		if (flashTrack0_freq > 0
			&& flashTrack0_freq <= PG_LOOP_SIZE // if flashTrack0_freq > PG_LOOP_SIZE -> update every frame
			&& ((int)round(arguments[0]) % (PG_LOOP_SIZE / flashTrack0_freq)) == 0) {
			flashTrack0_weights[1] = 1.0;
			// printf( "flashTrack0 (%d)\n" , flashTrack0 );
		}
#endif
#if PG_NB_TRACKS >= 3
		if (flashTrack0_freq_2 > 0
			&& flashTrack0_freq_2 <= PG_LOOP_SIZE // if flashTrack0_freq_2 > PG_LOOP_SIZE -> update every frame
			&& ((int)round(arguments[0]) % (PG_LOOP_SIZE / flashTrack0_freq_2)) == 0) {
			flashTrack0_weights[2] = 1.0;
			// printf( "flashTrack0 (%d)\n" , flashTrack0 );
		}
#endif

#ifdef PG_WITH_CA
		if (flashCA_freq_0 > 0
			&& flashCA_freq_0 <= PG_LOOP_SIZE // if flashCA_freq_0 > PG_LOOP_SIZE -> update every frame
			&& (beatNo % (PG_LOOP_SIZE / flashCA_freq_0)) == 0) {
			flashCA_weights[0] = 1.0;
			// printf( "flashCA 0 (%.2f)\n" , flashCA_weights[0] );
		}
#if PG_NB_TRACKS >= 2
		if (flashCA_freq_1 > 0
			&& flashCA_freq_1 <= PG_LOOP_SIZE // if flashCA_freq_1 > PG_LOOP_SIZE -> update every frame
			&& ((int)round(arguments[0]) % (PG_LOOP_SIZE / flashCA_freq_1)) == 0) {
			flashCA_weights[1] = 1.0;
			// printf( "flashCA 1 (%.2f)\n" , flashCA_weights[1] );
		}
#endif
#if PG_NB_TRACKS >= 3
		if (flashCA_freq_2 > 0
			&& flashCA_freq_2 <= PG_LOOP_SIZE // if flashCA_freq_0 > PG_LOOP_SIZE -> update every frame
			&& ((int)round(arguments[2]) % (PG_LOOP_SIZE / flashCA_freq_2)) == 0) {
			flashCA_weights[2] = 1.0;
			// printf( "flashCA 2 (%.2f)\n" , flashCA_weights[2] );
		}
#endif
#endif

#ifdef PG_WITH_CA
		if (flashBack_freq > 0
			&& flashBack_freq <= PG_LOOP_SIZE // if flashBack_freq > PG_LOOP_SIZE -> update every frame
			&& (beatNo % (PG_LOOP_SIZE / flashBack_freq)) == 0) {
			flashBack_weight = 1.0;
			// printf( "flashBack (%d)\n" , flashCA );
		}
#endif

		// updates display messages according to text_swap_freq (4)
		// for a swap duration of message_swap_duration
		if (InitialPerformanceTime != -1
			&& CurrentHashtag != -1
			&& TextChapter != -1
			&& (beatNo % (PG_LOOP_SIZE / 4)) == 0) {
			if (DisplayText1Front) {
				IndDisplayText2 = (IndDisplayText2 + 2);
				if (IndDisplayText2 > DisplayTextFirstInChapter[TextChapter + 1]) {
					IndDisplayText2 -= (DisplayTextFirstInChapter[TextChapter + 1] - DisplayTextFirstInChapter[TextChapter]);
				}
				DisplayText1Front = false;
				LengthDisplayText2 = (float)pg_displayMessage_upload(2);
				DisplayText1Alpha = 1.0f;
				DisplayText2Alpha = 0.0f;
			}
			else {
				IndDisplayText1 = (IndDisplayText1 + 2);
				if (IndDisplayText1 > DisplayTextFirstInChapter[TextChapter + 1]) {
					IndDisplayText1 -= (DisplayTextFirstInChapter[TextChapter + 1] - DisplayTextFirstInChapter[TextChapter]);
				}
				DisplayText1Front = true;
				LengthDisplayText1 = (float)pg_displayMessage_upload(1);
				DisplayText1Alpha = 0.0f;
				DisplayText2Alpha = 1.0f;
			}
			DisplayTextSwapInitialTime = CurrentClockTime;
		}

		// updates display images according to image_swap_freq
		// for a swap duration of image_swap_duration
		// printf("image_swap_freq %d beatNo %d %d %d \n",image_swap_freq,beatNo,CurrentHashtag, IndPlayedImageFile);
		if (InitialPerformanceTime != -1
			&& CurrentHashtag != -1
			// && image_swap_freq > 0
			&& (beatNo % (PG_LOOP_SIZE / (image_swap_freq <= PG_LOOP_SIZE ? image_swap_freq : PG_LOOP_SIZE))) == 0) {
			// there should be an initial procedure that scans the dirs by increasing index size
			// nd stores for each of them the number of images
			// printf("image_swapping \n" );
			string * fileName;
			int subdirectoryIndex = CurrentHashtag;
			// last scene is solo for last hashtag
			if( subdirectoryIndex == PG_NB_HASHTAGS) {
                subdirectoryIndex = PG_NB_HASHTAGS - 1;
			}
			if ((fileName = nextFile(&ImageDir, &subdirectoryIndex, &IndPlayedImageFile))) {
				if (pg_load_and_swap_image(fileName, image_swap_duration )) {
				}
				delete fileName;
			}
		}

		// ========================================
		// flash on/off values
		// ========================================
#ifdef PG_WITH_CA
	}
	else if (strcmp(newCommand, "flashCA_onOff") == 0) {
		if (flashCA_freq_0 > 0) {
			flashCA_freq_0_saved = flashCA_freq_0;
			flashCA_freq_0 = 0;
		}
		else {
			flashCA_freq_0 = flashCA_freq_0_saved;
		}

		// ======================================
		// keystroke s: Snapshot
		// ======================================
	}
	else if (strcmp(newCommand, "snapshot") == 0) {
		pg_snapshot((char *)"jpg");
	}

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// +++++++++++++++++ SWAPS TRACK & CA WEIGHTS ++++++++++++++
	// +++++++++++++++++ BTWEEN COMPOSITION & FINAL ++++++++++++
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// drawing on off void/currentDrawing mode (w)
	// =============================================
	else if (strcmp(newCommand, "drawingOnOff") == 0) {
		if (activeDrawingStroke == 0) {
			activeDrawingStroke = 1;
		}
		else {
			activeDrawingStroke = 0;
		}
		pg_apply_new_activeDrawingStroke();

	}
	else if (strcmp(newCommand, "camera_BG_subtr_plus") == 0) {
		camera_BG_subtr = (camera_BG_subtr + 1) % 3;
		// only the two modes (init and current are used, not without BG subtr)
		if (camera_BG_subtr == 0) {
			camera_BG_subtr = (camera_BG_subtr + 1) % 3;
		}
		BrokenInterpolationVar[_camera_BG_subtr] = true;
	}
	else {
		for (int indVar = 0; indVar < _MaxInterpVarIDs; indVar++) {
			if (strcmp(newCommand, CmdString[indVar]) == 0) {
				pg_update_variable(_PG_GUI_COMMAND, _PG_KEYSTROKE_VOID,
					indVar, arguments[0]);
				return;
			}
		}
	}
}

//////////////////////////////////////////////////////////////
// TRACK LIB FUNCTIONS
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
// svg files management
void loadsvg( void ) {
  currentSVGTrack = ((1 + currentSVGTrack)%nb_SVG_tracks);
  sprintf( AuxString , "/message_can SVG_NO_%d" , currentSVGTrack+1 ); pg_send_message_udp( (char *)"s" , AuxString , (char *)"udp_PD_send" );

#if PG_NB_TRACKS >= 2
  // stops ongoing readings
  track_interp[1] = false;
  pg_track_interpolate_stop( 1 );
  // is currently reading -> stops reading
  if( track_replay_1 ) {
    // stops reading
    pg_track_replay_stop( 1 );
  }

  if( currentSVGTrack == 0 ) {
    load_svg_track( (char *)("./ProjectsBefore18/" + project_name + "/textures/SVGs/fall_src_01_01.svg").c_str() ,
		    1 , true );
    load_svg_track( (char *)("./ProjectsBefore18/" + project_name + "/textures/SVGs/fall_src_02_01.svg").c_str() ,
		    2 , true );
    load_svg_track( (char *)("./ProjectsBefore18/" + project_name + "/textures/SVGs/fall_tgt_01_01.svg").c_str() ,
		    1 , false );
    load_svg_track( (char *)("./ProjectsBefore18/" + project_name + "/textures/SVGs/fall_tgt_02_01.svg").c_str() ,
		    2 , false );
  }
#endif
#endif
}

void pg_source_track_recording_onOff( int indTrack ) {
  // switches between recording on/off
  bool is_source_track_record = false;
    switch( indTrack ) {
      case 0:
	source_track_record_0 = !source_track_record_0;
	is_source_track_record = source_track_record_0;
	break;
#if PG_NB_TRACKS >= 2
      case 1:
	source_track_record_1 = !source_track_record_1;
	is_source_track_record = source_track_record_1;
	break;
#endif
#if PG_NB_TRACKS >= 3
      case 2:
	source_track_record_2 = !source_track_record_2;
	is_source_track_record = source_track_record_2;
	break;
#endif
    }

  // starts recording source
  if( is_source_track_record ) {
    bool isTrackReplay = false;
    switch( indTrack ) {
      case 0:
	isTrackReplay = track_replay_0;
	break;
#if PG_NB_TRACKS >= 2
      case 1:
	isTrackReplay = track_replay_1;
	break;
#endif
#if PG_NB_TRACKS >= 3
      case 2:
	isTrackReplay = track_replay_2;
	break;
#endif
    }

    // is reading -> has to stop reading
    if( isTrackReplay ) {
      pg_track_replay_stop( indTrack );
    }

    // launches source recording
    pg_source_track_recording_start( indTrack );

    // switches drawing mode to currentDrawingStroke
    activeDrawingStroke = 1;
    pg_apply_new_activeDrawingStroke();

  } else {
    // stops source recording
    pg_source_track_recording_stop( indTrack );

    // switches drawing mode to void
    activeDrawingStroke = 0;
    pg_apply_new_activeDrawingStroke();
  }
}

void pg_track_replay_onOff(int trackNo) {
	bool isTrackReplay = false;
	bool isSourceTrackRecord = false;
	switch (trackNo) {
	case 0:
		isTrackReplay = track_replay_0;
		isSourceTrackRecord = source_track_record_0;
		break;
#if PG_NB_TRACKS >= 2
	case 1:
		isTrackReplay = track_replay_1;
		isSourceTrackRecord = source_track_record_1;
		break;
#endif
#if PG_NB_TRACKS >= 3
	case 2:
		isTrackReplay = track_replay_2;
		isSourceTrackRecord = source_track_record_2;
		break;
#endif
	}

	// is not currently reading -> starts reading
	if (!isTrackReplay) {
		// printf( "start_read_source_track\n");

		// is recording source -> has to stop recording source
		if (isSourceTrackRecord) {
			pg_source_track_recording_stop(trackNo);
		}

		// start reading
		pg_track_replay_start(trackNo);

		// switches drawing mode to void if the read track is the current one
		if (trackNo == 0) {
			activeDrawingStroke = 0;
			pg_apply_new_activeDrawingStroke();
		}

		// is currently reading -> stops reading
	}
	else {
		// printf( "stop_read_source_track\n");

		// switches drawing mode to void  if the read track is the current one
		if (trackNo == 0) {
			activeDrawingStroke = 0;
			pg_apply_new_activeDrawingStroke();
		}

		// stops reading
		pg_track_replay_stop(trackNo);

	}
}

void pg_source_track_recording_start( int indTrack ) {
	switch (indTrack) {
	case 0:
		source_track_record_0 = true;
		break;
#if PG_NB_TRACKS >= 2
	case 1:
		source_track_record_1 = true;
		break;
#endif
#if PG_NB_TRACKS >= 3
	case 2:
		source_track_record_2 = true;
		break;
#endif
	}
	if (indTrack >= 0
		&& indTrack < PG_NB_TRACKS) {
		TrackStatus_source[indTrack].isActiveRecording = true;
		TrackStatus_source[indTrack].nbRecordedFrames = 0;
		TrackStatus_target[indTrack].isNormalized = false;
		// printf("start recording track %d\n",indTrack);
	}
}

void pg_source_track_recording_stop( int indTrack ) {
    switch( indTrack ) {
      case 0:
	source_track_record_0 = false;
	break;
#if PG_NB_TRACKS >= 2
      case 1:
	source_track_record_1 = false;
	break;
#endif
#if PG_NB_TRACKS >= 3
      case 2:
	source_track_record_2 = false;
	break;
#endif
    }
  if( indTrack >= 0
      && indTrack < PG_NB_TRACKS
      && TrackStatus_source[ indTrack ].isActiveRecording == true ) {
    TrackStatus_source[ indTrack ].isActiveRecording = false;
    // printf("start playing %d\n",indTrack);

    NormalizeTargetTrack( indTrack );
  }
}

void pg_track_interpolate_stop( int indTrack ) {
  if( indTrack >= 0
      && indTrack < PG_NB_TRACKS ) {
    TrackStatus_source[ indTrack ].isActiveRecording = false;
    TrackStatus_target[ indTrack ].isActiveRecording = false;
    TrackStatus_source[ indTrack ].isActiveInterpolation = false;
    // TrackStatus_source[ indTrack ].coefInterpolation = 0;
  }
}
void pg_track_replay_start( int indTrack ) {
    bool isTrackReplay = false;
    switch( indTrack ) {
      case 0:
	isTrackReplay = track_replay_0;
#if PG_NB_TRACKS >= 2
	if( track_replay_1 ) {
	  pg_track_replay_stop(1);
	}
#endif
#if PG_NB_TRACKS >= 3
	if( track_replay_2 ) {
	  pg_track_replay_stop(2);
	}
#endif
	break;
      case 1:
#if PG_NB_TRACKS >= 2
	isTrackReplay = track_replay_1;
#endif
	if( track_replay_0 ) {
	  pg_track_replay_stop(0);
	}
#if PG_NB_TRACKS >= 3
	if( track_replay_2 ) {
	  pg_track_replay_stop(2);
	}
#endif
	break;
      case 2:
#if PG_NB_TRACKS >= 3
	isTrackReplay = track_replay_2;
#endif
	if( track_replay_0 ) {
	  pg_track_replay_stop(0);
	}
#if PG_NB_TRACKS >= 2
	if( track_replay_1 ) {
	  pg_track_replay_stop(1);
	}
#endif
	break;
    }

  if( indTrack >= 0
      && indTrack < PG_NB_TRACKS
      && !isTrackReplay ) {
    switch( indTrack ) {
      case 0:
	track_replay_0 = true;
	break;
#if PG_NB_TRACKS >= 2
      case 1:
	track_replay_1 = true;
	break;
#endif
#if PG_NB_TRACKS >= 3
      case 2:
	track_replay_2 = true;
	break;
#endif
    }
    TrackStatus_source[ indTrack ].indReading = 0;
    TrackStatus_source[ indTrack ].isFirstFrame = true;
    // printf("-> start_read_source_track %d\n", indTrack);
  }
}
void pg_track_replay_stop( int indTrack ) {
    bool isTrackReplay = false;
	switch (indTrack) {
      case 0:
	isTrackReplay = track_replay_0;
	break;
#if PG_NB_TRACKS >= 2
      case 1:
	isTrackReplay = track_replay_1;
	break;
#endif
#if PG_NB_TRACKS >= 3
      case 2:
	isTrackReplay = track_replay_2;
	break;
#endif
     }

    if( indTrack >= 0
      && indTrack < PG_NB_TRACKS
      && isTrackReplay ) {
    switch( indTrack ) {
      case 0:
	track_replay_0 = false;
	break;
#if PG_NB_TRACKS >= 2
      case 1:
	track_replay_1 = false;
	break;
#endif
#if PG_NB_TRACKS >= 3
      case 2:
	track_replay_2 = false;
	break;
#endif
    }
    tracks_x[ indTrack ] = -1.0F;
    tracks_y[ indTrack ] = -1.0F;
    tracks_x_prev[ indTrack ] = -1.0F;
    tracks_y_prev[ indTrack ] = -1.0F;
    tracks_Color_r[ indTrack ] = 0.0F;
    tracks_Color_g[ indTrack ] = 0.0F;
    tracks_Color_b[ indTrack ] = 0.0F;
    tracks_Color_a[ indTrack ] = 1.0F;
    tracks_RadiusX[ indTrack ] = 0.0F;
    tracks_RadiusY[ indTrack ] = 0.0F;
    // printf( "-> start_read_source_track\n"  );
  }
}

void pg_track_interp_size( int indTrack , float nbFrames ) {
  if( indTrack >= 0
      && indTrack < PG_NB_TRACKS ) {
    float nbTotFrameInterpolation =
      nbFrames * TrackStatus_source[ indTrack ].nbRecordedFrames;
    if( nbTotFrameInterpolation > 0 ) {
      TrackStatus_source[ indTrack ].interpolationStep
	= 1.0F /nbTotFrameInterpolation;
    }
    else {
      TrackStatus_source[ indTrack ].interpolationStep
	= 0.0001F;
    }
  }
}

void pg_track_interpolate_start( int indTrack ) {
  if( indTrack >= 0
      && indTrack < PG_NB_TRACKS ) {
    TrackStatus_source[ indTrack ].isActiveRecording = false;
    TrackStatus_target[ indTrack ].isActiveRecording = false;
    TrackStatus_source[ indTrack ].isActiveInterpolation = true;
    //TrackStatus_source[ indTrack ].coefInterpolation = 0;

    NormalizeTargetTrack( indTrack );
  }
}

void pg_track_interpolate_step_smooth( int indTrack , bool isStepwiseInterpolation ) {
  if( indTrack >= 0
      && indTrack < PG_NB_TRACKS) {
    TrackStatus_source[ indTrack ].isStepwiseInterpolation = isStepwiseInterpolation;
  }
}

void pg_track_replay_speed_scale( int indTrack , float speed ) {
  if( indTrack >= 0
      && indTrack < PG_NB_TRACKS ) {
    TrackStatus_source[ indTrack ].readSpeedScale = speed;
  }
}

void pg_track_interpolate_reset( int indTrack ) {
  if( indTrack >= 0
      && indTrack < PG_NB_TRACKS ) {
    TrackStatus_source[ indTrack ].coefInterpolation = 0.0;
    TrackStatus_source[ indTrack ].indReading = 0;
  }
}

//////////////////////////////////////////////////////////////
// MISC LIB FUNCTIONS
//////////////////////////////////////////////////////////////

void pg_writeMessageOnScreen( char *text ) {
  strcpy( ScreenMessage , text );
  double newtime = RealTime();
  ScreenMessageTransparency = 1.0;
  LastScreenMessageDecayTime = newtime;
  ScreenMessageUpdated = true;
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
