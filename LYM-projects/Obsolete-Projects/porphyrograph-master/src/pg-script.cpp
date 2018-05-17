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

std::string project_name = "core";

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

// ++++++++++++++++++++++ FREEZING AND MEMORY VARIABLES ++++++++++++++++++++++++ 
bool frozen = false;

// CA saved in case of freezing
int CAType_saved = 0; // for freeze 
int CASubType_saved = 0; // for freeze 
bool CA_onOff = true; // CA on off flag

// particles mode saved in case of freezing
int particleMode_saved = 0; // for freeze 

// ++++++++++++++++++++++ CA set-up memory ++++++++++++++++++++++++ 
#define PG_NB_MEMORIZED_CA 2
int   CAType_memory[PG_NB_MEMORIZED_CA];
int   CASubType_memory[PG_NB_MEMORIZED_CA];
float CAParams1_memory[PG_NB_MEMORIZED_CA];
float CAParams2_memory[PG_NB_MEMORIZED_CA];
float CAParams3_memory[PG_NB_MEMORIZED_CA];
float CAParams4_memory[PG_NB_MEMORIZED_CA];
float CAParams5_memory[PG_NB_MEMORIZED_CA];
float CAParams6_memory[PG_NB_MEMORIZED_CA];
float CAParams7_memory[PG_NB_MEMORIZED_CA];
float CAParams8_memory[PG_NB_MEMORIZED_CA];


// ++++++++++++++++++++++ MUSIC CONTROL ++++++++++++++++++++ 
// music control inputs received at each frame
// pointer motion
float xy_spectrum[2];
// brush radius
float brush_radius_vol;
// addition of color based on palette
float RGBpalette_received[4];
// factor increasing the acceleration weight of particles
// based on sound volume attacks
float particle_acc_attack;

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
// auxiliary 
bool synchr_start_recording_source_track[PG_NB_TRACKS];
bool synchr_start_recording_target_track[PG_NB_TRACKS];
bool synchr_start_replay_track[PG_NB_TRACKS];

// track loading 
int currentSVGTrack = 0;

// ++++++++++++++++++++++ FLASHES +++++++++++++++++++++++++
// flash: cellular automata refeeding -> values passed to the shader
// as a function of on/off values and weights
float flashCA_weights[PG_NB_TRACKS];
// flash: track 0 refeeding  
float flashBack_weight;
// flash: extension of particle reproduction  
int flashPart;
// flash: track to BG copy   -> values passed to the shader
// as a function of on/off values and weights
// only tracks 1 and 2 are concerned since it is a projection on track0
float flashTrack0_weights[PG_NB_TRACKS];

int flashCA_freq_0_saved = 0;
int flashCA_freq_1_saved = 0;
int flashCA_freq_2_saved = 0;

int flashTrack0_freq_1_saved = 0;
int flashTrack0_freq_2_saved = 0;

// +++++++++++++++++++++ VIDEO BACKGROUND SUBTRACTION +++++
bool is_currentBG_capture = false;
bool is_initialBG_capture = false;
bool is_firstVideoOn = true;

// +++++++++++++++++++++ VIDEO FRAME CAPTURE +++++
bool is_capturedFB = false;

// +++++++++++++++++++++ CLEAR +++++++++++++++++++++++++++
int isClearCA;
int isClearLayer;
int isClearAllLayers;

// +++++++++++++++++++++ COPY LAYERS +++++++++++++++++++++++++++
int isCopyTrack12;

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
			*((int *)ConfigurationVarPointers[indVar]) = (int)round(InitialValuesConfigurationVar[indVar]);
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
	      (GLfloat)leftWindowWidth , (GLfloat)window_height );
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

  // target track recording
  // is recording target -> has to stop recording target 
  if( !target_track_record_0 ) {
      pg_target_track_recording_stop( 0 );
  }
#if PG_NB_TRACKS >= 2
  if( !target_track_record_1 ) {
      pg_target_track_recording_stop( 1 );
  }
#endif
#if PG_NB_TRACKS >= 3
  if( !target_track_record_2 ) {
      pg_target_track_recording_stop( 2 );
  }
#endif

  /////////////////////////////////////////////////////////////////////////
  // NON SCENARIO-CONTROLLED UNIFORM VARIABLES (COULD BECOME CONTROLLED BY FUTURE SCENARIOS)
  // target track display on/off 
  for( int indTrack = 0 ; indTrack < PG_NB_TRACKS ; indTrack++ ) {

	// is interpolating -> has to stop interpolating 
    if( track_interp[ indTrack ] ) {
      track_interp[ indTrack ] = false;
      pg_track_interpolate_stop( indTrack );
    }
    // is making smooth interpolation -> has to go to stepwise interpolation 
    if( !track_stepwise_interpolation[ indTrack ] ) {
      track_stepwise_interpolation[ indTrack ] = true;
      pg_track_interpolate_step_smooth( currentDrawingTrack , true );
    }
  }

  /////////////////////////////////////////////////////////////////////////
  // SOUND-CONTROLLED UNIFORM VARIABLES: dynamic values received from PD 
  // flashes
  for( int indtrack = 0 ; indtrack < PG_NB_TRACKS ; indtrack++ ) {
    flashCA_weights[indtrack] = 0;
    flashTrack0_weights[indtrack] = 0;
  }
  flashPart = 0;
  flashBack_weight = 0;

  // background subraction
  is_currentBG_capture = false;
  is_initialBG_capture = false;

  // framebuffer memory
  is_capturedFB = false;

  // clear
  isClearCA = 0;
  isClearLayer = 0;
  isClearAllLayers = 0;

  // copy
  isCopyTrack12 = 0;

  // pen music color
  RGBpalette_received[0] = RGBpalette_r_ini;
  RGBpalette_received[1] = RGBpalette_g_ini;
  RGBpalette_received[2] = RGBpalette_b_ini;
  RGBpalette_received[3] = RGBpalette_a_ini;
  // xy jitter
  xy_spectrum[0] = xy_spectrum_ini;
  xy_spectrum[1] = xy_spectrum_ini;
  // radius 
  brush_radius_vol = brush_radius_vol_ini;
  // particle acceleration based on sound attack
  particle_acc_attack = particle_acc_attack_ini;

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
  // SVG TRACK LOADING
  load_svg_track( (char *)("./textures/SVGs/fall_src_01_01.svg") , 
		  1 , true );
  load_svg_track( (char *)("./textures/SVGs/fall_src_02_01.svg") , 
		  2 , true );
  load_svg_track( (char *)("./textures/SVGs/fall_tgt_01_01.svg") , 
		  1 , false );
  load_svg_track( (char *)("./textures/SVGs/fall_tgt_02_01.svg") , 
		  2 , false );

  /////////////////////////////////////////////////////////////////////////
  // SENSORS INITIALIZATION
  // copies the grid layout
  int indLayout = sensor_layout;
  for( int indSens = 0 ; indSens < PG_NB_SENSORS ; indSens++ ) {
    sensorPositions[ 3 * indSens ] = sensorLayouts[ indLayout * 3 * PG_NB_SENSORS + 3 * indSens ];
    sensorPositions[ 3 * indSens + 1 ] = sensorLayouts[ indLayout * 3 * PG_NB_SENSORS + 3 * indSens + 1 ];
    sensorPositions[ 3 * indSens + 2 ] = 0.1f;
  }

  // copies the single central activation
  int indActivation = sensor_activation;
  for( int indSens = 0 ; indSens < PG_NB_SENSORS ; indSens++ ) {
    sensor_onOff[ indSens ] = sensorActivations[ indActivation * PG_NB_SENSORS + indSens ];
  }


  /////////////////////////////////////////////////////////////////////////
  // INTERFACE INITIALIZATION
  pg_send_message_udp( (char *)"i i" , (char *)"/connect" , (char *)"udp_PD_send" );

  sprintf( AuxString , "/input_volume_can %.5f" , input_volume ); pg_send_message_udp( (char *)"f" , AuxString , (char *)"udp_PD_send" );
  sprintf( AuxString , "/videoThreshold_can %.5f" , videoThreshold ); pg_send_message_udp( (char *)"f" , AuxString , (char *)"udp_PD_send" );

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
// SCENARIO BASED COMMANDS
///////////////////////////////////////////////////////////////////////////////////
void currentDrawingStroke_callBack(pg_Parameter_Input_Type param_input_type , float scenario_or_gui_command_value) {
	if (activeDrawingStroke > 0) {
		activeDrawingStroke = currentDrawingStroke + 1;
		pg_apply_new_activeDrawingStroke();
	}
}
void partAccCenter_0_callBack( pg_Parameter_Input_Type param_input_type , float scenario_or_gui_command_value) {
	if(param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_KEYSTROKE) {
		sprintf( AuxString , "/message_can part_center_H_%.1f" , scenario_or_gui_command_value ); pg_send_message_udp( (char *)"s" , (char *)AuxString , (char *)"udp_PD_send" );
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
void clearLayer_callBack( pg_Parameter_Input_Type param_input_type , float scenario_or_gui_command_value) {
    if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_KEYSTROKE) {
		if( CurrentClockTime - lastClearTime  < 0.2 ) {
			isClearLayer = 1;
			lastClearTime = 0.0;
			pg_send_message_udp((char *)"s", (char *)"/message_can clear_all_layers", (char *)"udp_PD_send");
		}
		else {
			isClearLayer = 1;
			lastClearTime = CurrentClockTime;
			pg_send_message_udp((char *)"s", (char *)"/message_can clear_layer", (char *)"udp_PD_send");
		}
    }
    else if (param_input_type == _PG_SCENARIO && scenario_or_gui_command_value > 0.0 ) {
	    isClearLayer = 1;
        lastClearTime = CurrentClockTime;
	}
}
void clearCA_callBack( pg_Parameter_Input_Type param_input_type , float scenario_or_gui_command_value) {
    if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_KEYSTROKE) {
	  pg_send_message_udp((char *)"s", (char *)"/message_can clear_CA", (char *)"udp_PD_send");
	      isClearCA = 1;
	  }
	  else if (param_input_type == _PG_SCENARIO  && scenario_or_gui_command_value > 0.0 ) {
		  isClearCA = 1;
	}
}
void copyTrack12_callBack( pg_Parameter_Input_Type param_input_type , float scenario_or_gui_command_value) {
    pg_send_message_udp((char *)"s", (char *)"/message_can copy_track_1_2", (char *)"udp_PD_send");
    if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_KEYSTROKE) {
	isCopyTrack12 = 1;
  }
    else if (param_input_type == _PG_SCENARIO) {
	if (scenario_or_gui_command_value > 0) {
	    isCopyTrack12 = 1;
  }
  }
}
void sensor_layout_callBack( pg_Parameter_Input_Type param_input_type , float scenario_or_gui_command_value) {
  // copies the grid layout
  int indLayout = std::max( 0 , std::min( sensor_layout , PG_NB_MAX_SENSOR_LAYOUTS - 1 ) );
  // non random layout: copy
  if( indLayout < PG_NB_MAX_SENSOR_LAYOUTS - 1 ) {
    for( int indSens = 0 ; indSens < PG_NB_SENSORS ; indSens++ ) {
      sensorPositions[ 3 * indSens ] = sensorLayouts[ indLayout * 3 * PG_NB_SENSORS + 3 * indSens ];
      sensorPositions[ 3 * indSens + 1 ] = sensorLayouts[ indLayout * 3 * PG_NB_SENSORS + 3 * indSens + 1 ];
      sensorPositions[ 3 * indSens + 2 ] = 0.1f;
    }
  }
  // random layout: regenerate
  else {
    for( int indSens = 0 ; indSens < PG_NB_SENSORS ; indSens++ ) {
      sensorPositions[ 3 * indSens ] = leftWindowWidth/2.0f + leftWindowWidth/2.0f * (2.0f * ((float)rand()/(float)RAND_MAX) - 1.0f);
      sensorPositions[ 3 * indSens + 1 ] = window_height/2.0f + window_height/2.0f * (2.0f * ((float)rand()/(float)RAND_MAX) - 1.0f);
      sensorPositions[ 3 * indSens + 2 ] = 0.1f;
    }
  }
}
void sensor_activation_callBack( pg_Parameter_Input_Type param_input_type , float scenario_or_gui_command_value) {
  // copies the grid layout
  // copies the single central activation
  int indActivation = std::max( 0 , std::min( sensor_activation , PG_NB_MAX_SENSOR_ACTIVATIONS - 1 ) );
  for( int indSens = 0 ; indSens < PG_NB_SENSORS ; indSens++ ) {
    sensor_onOff[ indSens ] = sensorActivations[ indActivation * PG_NB_SENSORS + indSens ];
  }
  // incremental sensor activation
  if( sensor_activation == 5 ) {
     sensor_last_activation_time = CurrentClockTime;
  }
}
void sample_setUp_callBack( pg_Parameter_Input_Type param_input_type , float scenario_or_gui_command_value) {
  sample_setUp_interpolation();
}
void track_replay_callBack( int trackNo , pg_Parameter_Input_Type param_input_type , float scenario_or_gui_command_value ) {
    bool isTrackReplay = false;
    switch( trackNo ) {
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
    if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_KEYSTROKE) {
	// is not currently reading -> starts reading 
	if (!isTrackReplay) {
	    if (tracksSync) {
		    synchr_start_replay_track[trackNo] = true;
	    }
	    else {
		    pg_track_replay_onOff(trackNo);
	    }
	}
	// is currently reading -> stops reading 
	else {
		    pg_track_replay_onOff(trackNo);
	}
    }
    else if (param_input_type == _PG_SCENARIO) {
	if (scenario_or_gui_command_value > 0) {
	    if (!isTrackReplay) {
		    // printf("is not currently reading -> starts reading \n");
		    if (tracksSync) {
			    // printf("synchronized starts reading \n");
			    synchr_start_replay_track[trackNo] = true;
		    }
		    else {
			    pg_track_replay_onOff(trackNo);
		    }
	    }
	}
	else {
	    if (isTrackReplay) {
		    // printf("is currently reading -> stops reading \n");
		    pg_track_replay_onOff(trackNo);
	    }
	}
    }
}
void track_replay_0_callBack( pg_Parameter_Input_Type param_input_type , float scenario_or_gui_command_value ) {
  track_replay_callBack( 0 , param_input_type , scenario_or_gui_command_value);
}
void track_replay_1_callBack( pg_Parameter_Input_Type param_input_type , float scenario_or_gui_command_value ) {
#if PG_NB_TRACKS >= 2
  track_replay_callBack( 1 , param_input_type , scenario_or_gui_command_value);
#endif
}
void track_replay_2_callBack( pg_Parameter_Input_Type param_input_type , float scenario_or_gui_command_value ) {
#if PG_NB_TRACKS >= 3
  track_replay_callBack( 2 , param_input_type , scenario_or_gui_command_value);
#endif
}
void source_track_record_callBack( int trackNo , pg_Parameter_Input_Type param_input_type , float scenario_or_gui_command_value ) {
    bool isTrackRecord = false;
    switch( trackNo ) {
      case 0:
    isTrackRecord = source_track_record_0;
	break;
#if PG_NB_TRACKS >= 2
      case 1:
	isTrackRecord = source_track_record_1;
	break;
#endif
#if PG_NB_TRACKS >= 3
      case 2:
	isTrackRecord = source_track_record_2;
	break;
#endif
    }
    if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_KEYSTROKE) {
	// is not currently recording -> starts recording 
	if (!isTrackRecord) {
	    if (tracksSync) {
		    synchr_start_recording_source_track[trackNo] = true;
	    }
	    else {
		    pg_source_track_recording_onOff(trackNo);
	    }
	}
	// is currently recording -> stops recording 
	else {
		    pg_source_track_recording_onOff(trackNo);
	}
    }
    else if (param_input_type == _PG_SCENARIO) {
	if (scenario_or_gui_command_value > 0) {
	    // is not currently recording -> starts recording 
	    if (!isTrackRecord) {
		    if (tracksSync) {
			    synchr_start_recording_source_track[trackNo] = true;
		    }
		    else {
			    pg_source_track_recording_onOff(trackNo);
		    }
	    }
	}
	else {
	    if (isTrackRecord) {
		    // is currently recording -> stops recording 
		    pg_source_track_recording_onOff(trackNo);
	    }
	}
    }
}
void source_track_record_0_callBack( pg_Parameter_Input_Type param_input_type , float scenario_or_gui_command_value ) {
  source_track_record_callBack( 0 , param_input_type , scenario_or_gui_command_value);
}
void source_track_record_1_callBack( pg_Parameter_Input_Type param_input_type , float scenario_or_gui_command_value ) {
#if PG_NB_TRACKS >= 2
  source_track_record_callBack( 1 , param_input_type , scenario_or_gui_command_value);
#endif
}
void source_track_record_2_callBack( pg_Parameter_Input_Type param_input_type , float scenario_or_gui_command_value ) {
#if PG_NB_TRACKS >= 3
  source_track_record_callBack( 2 , param_input_type , scenario_or_gui_command_value);
#endif
}
void target_track_record_callBack( int trackNo , pg_Parameter_Input_Type param_input_type , float scenario_or_gui_command_value ) {
    bool isTrackRecord = false;
    switch( trackNo ) {
      case 0:
	isTrackRecord = target_track_record_0;
	break;
#if PG_NB_TRACKS >= 2
      case 1:
	isTrackRecord = target_track_record_1;
	break;
#endif
#if PG_NB_TRACKS >= 3
      case 2:
	isTrackRecord = target_track_record_2;
	break;
#endif
    }
    if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_KEYSTROKE) {
	// is not currently recording -> starts recording 
	if (!isTrackRecord) {
	    if (tracksSync) {
		    synchr_start_recording_target_track[trackNo] = true;
	    }
	    else {
		    pg_target_track_recording_onOff(trackNo);
	    }
	}
	// is currently recording -> stops recording 
	else {
		    pg_target_track_recording_onOff(trackNo);
	}
    }
    else if (param_input_type == _PG_SCENARIO) {
	if (scenario_or_gui_command_value > 0) {
	    // is not currently recording -> starts recording 
	    if (!isTrackRecord) {
		    if (tracksSync) {
			    synchr_start_recording_target_track[trackNo] = true;
		    }
		    else {
			    pg_target_track_recording_onOff(trackNo);
		    }
	    }
	}
	else {
	    if (isTrackRecord) {
		    // is currently recording -> stops recording 
		    pg_target_track_recording_onOff(trackNo);
	    }
	}
    }
}
void target_track_record_0_callBack( pg_Parameter_Input_Type param_input_type , float scenario_or_gui_command_value ) {
  target_track_record_callBack( 0 , param_input_type , scenario_or_gui_command_value);
}
void target_track_record_1_callBack( pg_Parameter_Input_Type param_input_type , float scenario_or_gui_command_value ) {
#if PG_NB_TRACKS >= 2
  target_track_record_callBack( 1 , param_input_type , scenario_or_gui_command_value);
#endif
}
void target_track_record_2_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
#if PG_NB_TRACKS >= 3
	target_track_record_callBack(2, param_input_type, scenario_or_gui_command_value);
#endif
}
void auto_beat_callBack( pg_Parameter_Input_Type param_input_type , float scenario_or_gui_command_value ) {
  lastBeatTime = CurrentClockTime;
}
void is_videoOn_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_KEYSTROKE || param_input_type == _PG_SCENARIO) {
		if (is_videoOn && is_firstVideoOn) {
			is_currentBG_capture = true;
			is_initialBG_capture = true;
			is_firstVideoOn = false;
			// printf("is_currentBG_capture\n" );
			// printf("is_initialBG_capture\n" );
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
	float step = 0.0f;
	if (parm_keystroke_type == _PG_KEYSTROKE_MINUS) {
		step = StepMinus[indParam];
	}
	else {
		step = StepPlus[indParam];
	}
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
		else if (param_input_type == _PG_KEYSTROKE) {
			*((float *)ScenarioVarPointers[indParam]) += step;
		}
		if (ScenarioVarCallbacks[indParam]) {
			(*ScenarioVarCallbacks[indParam])(param_input_type,scenario_or_gui_command_value);
		}
	}
	else if (ScenarioVarTypes[indParam] == _pg_int) {
		if (param_input_type == _PG_GUI_COMMAND || param_input_type == _PG_SCENARIO) {
			*((int *)ScenarioVarPointers[indParam]) = (int)round(scenario_or_gui_command_value);
		}
		else if (param_input_type == _PG_KEYSTROKE) {
			*((int *)ScenarioVarPointers[indParam]) = (*((int *)ScenarioVarPointers[indParam]) + (int)step + (int)MaxValues[indParam] + 1) % ((int)MaxValues[indParam] + 1);
		}
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
		if( scene_interpolations[ ind_scene ][ indP ].interpolation_mode == pg_keep_value ) {
		  continue;
		}

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

  /* ------------------------------- no more automatic snapshots */
  case 'n':
    is_automatic_snapshots = false;
    pg_send_message_udp( (char *)"s" , (char *)"/message_can NO_MORE_SNAPSHOTS" , (char *)"udp_PD_send" );
    break;

  /* ------------------------------- beat */
  case 'b':
    auto_beat = !auto_beat;
    lastBeatTime = CurrentClockTime;
    break;
    
  /* ------------------------------- current video background capture */
  case 'v':
	  is_currentBG_capture = true;
	  break;

  /* ------------------------------- initial video background capture */
  case 'w':
	  is_initialBG_capture = true;
	  break;

  /* ------------------------------- frame buffer capture */
  case 'c':
	  is_capturedFB = true;
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
  char *CmdChar = NULL;
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
  }

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
    
    // ======================================== 
    // background subtraction modes (Z) 
    // ======================================== 
  case'y':
     BGSubtr = (BGSubtr + 1) % 3;
     printf("BGstbtr %d\n",BGSubtr);
     BrokenInterpolationVar[ _BGSubtr ] = true;
    break;

    // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
    // +++++++++++++++++ LAUNCH TIMER ++++++++++++++++++++++++++ 
    // +++++++++++++++++   keystroke (t)  ++++++++++++++++++++++ 
    // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
  case't':
    isClearCA = 1; // clean CA layer
    isClearAllLayers = 1; // clears bg layer
    restoreInitialTimesAndDurations();
    sprintf( AuxString , "/PDbt1_on 1.0" ); pg_send_message_udp( (char *)"f" , AuxString , (char *)"udp_SC_send" );
    sprintf( AuxString , "/launch %f" , CurrentClockTime ); pg_send_message_udp( (char *)"f" , AuxString , (char *)"udp_SC_send" );
#ifdef PG_RENOISE
    sprintf( AuxString , "/renoise/transport/start" ); pg_send_message_udp( (char *)"" , AuxString , (char *)"udp_RN_send" );
    for( int indSample = 0 ; indSample < PG_NB_MAX_SAMPLE_SETUPS * PG_NB_SENSORS ; indSample++ ) {
	  std::string message = "/renoise/song/track/";
	  std::string float_string = std::to_string(static_cast<long double>(indSample + 1));
	  message += float_string + "/mute";

	  // message format
	  std::string format = "";

	  // message posting
	  pg_send_message_udp( (char *)format.c_str() , (char *)message.c_str() , (char *)"udp_RN_send" );

	  // resets the clock for replaying the sample if sensor triggered again
	  sample_play_start[ indSample ] = -1.0;
    }
#endif
    pg_send_message_udp( (char *)"s" , (char *)"/message_can START" , (char *)"udp_PD_send" );
    InitialScenarioTime 
      = CurrentClockTime - scene_initial_times[ 0 ];
    sprintf( AuxString , "/setup_can %s" , scene_IDs[ 0 ].c_str() ); pg_send_message_udp( (char *)"s" , AuxString , (char *)"udp_PD_send" );
    // printf( "/setup_can %s" , scene_IDs[ 0 ].c_str() ); 
    // reinitialization of the interpolation control variables at the beginning of a new scene
    for( int indVar = 0 ; indVar < _MaxInterpVarIDs ; indVar++ ) {
      BrokenInterpolationVar[ indVar ] = false;
    }
    initCA = 1.0f;
    break;

	    // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
    // +++++++++++++++++ SENSORS +++++++++++++++++++++++++ 
    // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
  case'k':
    currentSensor = (currentSensor + 1) % PG_NB_SENSORS;
    break;

  case'K':
    currentSensor = (currentSensor - 1 + PG_NB_SENSORS) % PG_NB_SENSORS;
    break;

  case'l':
    sensorFollowMouse_onOff = !sensorFollowMouse_onOff;
    break;
 
  case'L':
    sensor_onOff[ currentSensor ] = !sensor_onOff[ currentSensor ];
    break;
 

    // ======================================== 
    // loop size (number of beats in each loop) (V) 
    // ======================================== 
  case'V':
    flashBack_freq = ( flashBack_freq < loop_size ? 2 * flashBack_freq : 1 );
    flashPart_freq = ( flashPart_freq < loop_size ? 2 * flashPart_freq : 1 );
    // printf( "Loop size (loop_size)\n" );
    BrokenInterpolationVar[ _flashCA_freq_0 ] = true;
#if PG_NB_TRACKS >= 2
    BrokenInterpolationVar[ _flashCA_freq_1 ] = true;
#endif
#if PG_NB_TRACKS >= 3
    BrokenInterpolationVar[ _flashCA_freq_2 ] = true;
#endif
    BrokenInterpolationVar[ _flashBack_freq ] = true;
    BrokenInterpolationVar[ _flashPart_freq ] = true;
    break;

    // ======================================== 
    // Track / target interpolation start/stop (() 
    // ======================================== 
  case'(':
    track_interp[ currentDrawingTrack ] = !track_interp[ currentDrawingTrack ];
    if( track_interp[ currentDrawingTrack ] ) {
      pg_track_interpolate_start( currentDrawingTrack );
    } else {
      pg_track_interpolate_stop( currentDrawingTrack );
    }
    break;
 
    // ======================================== 
    // Track / target stepwise vs smooth interpolation (Y) 
    // ======================================== 
  case'Y':
    track_stepwise_interpolation[ currentDrawingTrack ] 
      = !track_stepwise_interpolation[ currentDrawingTrack ];
    pg_track_interpolate_step_smooth( currentDrawingTrack , 
      track_stepwise_interpolation[ currentDrawingTrack ] );
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
  // +++++++++++++++++ FLASHES BETWEEN LAYERS & PARTICLES ++++ 
  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
  // flashPart          (f)                 
  // flashCA            (h)                 
  // flashBack          (H)                 
  // flashPart duration (Q)                 
  // ====================================== 
  case'f':
    flashPart = flashPart_duration;
    break;
    
  case'h':
      flashCA_weights[currentDrawingTrack] = 1.0;
    break;
    
  case'H':
    flashBack_weight = 1.0;
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

void pg_continuous_flahes( void ) {
    // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
    // +++++++++++++++++ FLASHES +++++++++++++++++++++++++++++++ 
    // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
    // ======================================== 
    // flash triggering according to beat & frequency, weight, and on/off values 
    // ======================================== 
    // flashes are synchronized on beats according to their frequency
    if( flashPart_freq > loop_size ) { // if flashPart_freq > loop_size -> update every frame
      flashPart = flashPart_duration;
    }

    if( flashCA_freq_0 > loop_size ) { // if flashCA_freq_0 > loop_size -> update every frame
	flashCA_weights[0] = 1.0;
      // printf( "flashCA (%d)\n" , flashCA_freq_0 );
    }
#if PG_NB_TRACKS >= 2
    if( flashCA_freq_1 > loop_size ) { // if flashCA_freq_1 > loop_size -> update every frame
	flashCA_weights[1] = 1.0;
      // printf( "flashCA (%d)\n" , flashCA_freq_1 );
    }
#endif
#if PG_NB_TRACKS >= 3
    if( flashCA_freq_2 > loop_size ) { // if flashCA_freq_2 > loop_size -> update every frame
	flashCA_weights[2] = 1.0;
      // printf( "flashCA (%d)\n" , flashCA_freq_2 );
    }
#endif

#if PG_NB_TRACKS >= 2
    if( flashTrack0_freq_1 > loop_size ) { // if flashTrack0_freq_1 > loop_size -> update every frame
        // should only be flashed every second frame because of the way the particles are spreaded
        // otherwise the particles are not emitted
	if( FrameNo %2 == 0 ) {
	  flashTrack0_weights[1] = 1.0;
	}
	//printf( "flashTrack0_weights 1 (%.2f)\n" , flashTrack0_weights[1] );
    }
#endif
#if PG_NB_TRACKS >= 3
    if( flashTrack0_freq_2 > loop_size ) { // if flashTrack0_freq_2 > loop_size -> update every frame
        // should only be flashed every second frame because of the way the particles are spreaded
        // otherwise the particles are not emitted
	if( FrameNo %2 == 0 ) {
	  flashTrack0_weights[2] = 1.0;
	}
	//printf( "flashTrack0_weights 2 (%.2f)\n" , flashTrack0_weights[2] );
    }
#endif

    if( flashBack_freq > loop_size ) { // if flashBack_freq > loop_size -> update every frame
      flashBack_weight = 1.0;
      // printf( "flashBack (%d)\n" , flashCA );
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

  // renames the command according to the variables found in the scenario
  if( strcmp( command_symbol , "trackdecay_sign" ) == 0 ) {
    sprintf( newCommand , "trackdecay_sign_%d" , currentDrawingTrack );
  } else if( strcmp( command_symbol , "trackdecay" ) == 0 ) {
    sprintf( newCommand , "trackdecay_%d" , currentDrawingTrack );
  } else if (strcmp(command_symbol, "trackCompositionWeight") == 0) {
	  sprintf(newCommand, "trackCompositionWeight_%d", currentDrawingTrack);
	  // printf("trackCompositionWeight_%d\n", currentDrawingTrack);
  } else if (strcmp(command_symbol, "trackFinalWeight") == 0) {
	  sprintf(newCommand, "trackFinalWeight_%d", currentDrawingTrack);
	  // printf("trackFinalWeight_%d\n", currentDrawingTrack);
  } else if (strcmp(command_symbol, "track_replay") == 0) {
    sprintf( newCommand , "track_replay_%d" , currentDrawingTrack );
  } else if( strcmp( command_symbol , "source_track_record" ) == 0 ) {
    sprintf( newCommand , "source_track_record_%d" , currentDrawingTrack );
  } else if( strcmp( command_symbol , "target_track_record" ) == 0 ) {
    sprintf( newCommand , "target_track_record_%d" , currentDrawingTrack );
     
  } else if( strcmp( command_symbol , "flashTrack0_coef" ) == 0 ) {
    sprintf( newCommand , "flashTrack0_coef_%d" , currentDrawingTrack );
  } else if( strcmp( command_symbol , "flashCA_coef" ) == 0 ) {
    sprintf( newCommand , "flashCA_coef_%d" , currentDrawingTrack );
  } else if( strcmp( command_symbol , "flashTrack0_freq" ) == 0 ) {
    sprintf( newCommand , "flashTrack0_freq_%d" , currentDrawingTrack );
  } else if( strcmp( command_symbol , "flashCA_freq" ) == 0 ) {
    sprintf( newCommand , "flashCA_freq_%d" , currentDrawingTrack );
  }

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
    isClearCA = 1; // clean CA layer
    isClearAllLayers = 1; // clears bg layer
    restoreInitialTimesAndDurations();
    sprintf( AuxString , "/launch %f" , CurrentClockTime ); pg_send_message_udp( (char *)"f" , AuxString , (char *)"udp_PD_send" );
#ifdef PG_RENOISE
    sprintf( AuxString , "/renoise/transport/start" ); pg_send_message_udp( (char *)"" , AuxString , (char *)"udp_RN_send" );
    for( int indSample = 0 ; indSample < PG_NB_MAX_SAMPLE_SETUPS * PG_NB_SENSORS ; indSample++ ) {
	  std::string message = "/renoise/song/track/";
	  std::string float_string = std::to_string(static_cast<long double>(indSample + 1));
	  message += float_string + "/mute";

	  // message format
	  std::string format = "";

	  // message posting
	  pg_send_message_udp( (char *)format.c_str() , (char *)message.c_str() , (char *)"udp_RN_send" );

	  // resets the clock for replaying the sample if sensor triggered again
	  sample_play_start[ indSample ] = -1.0;
    }
#endif
    pg_send_message_udp( (char *)"s" , (char *)"/message_can START" , (char *)"udp_PD_send" );
    InitialScenarioTime 
      = CurrentClockTime - scene_initial_times[ 0 ];
	sprintf(AuxString, "/setup_can %s", scene_IDs[0].c_str()); pg_send_message_udp((char *)"s", AuxString, (char *)"udp_PD_send");
    // reinitialization of the interpolation control variables at the beginning of a new scene
    for( int indVar = 0 ; indVar < _MaxInterpVarIDs ; indVar++ ) {
      BrokenInterpolationVar[ indVar ] = false;
    }

    // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
    // +++++++++++++++++ SENSORS +++++++++++++++++++++++++ 
    // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
  } else if( strcmp( newCommand , "currentSensor_plus" ) == 0 ) {
      currentSensor = (currentSensor + 1) % PG_NB_SENSORS;
 
  } else if( strcmp( newCommand , "currentSensor_minus" ) == 0 ) {
      currentSensor = (currentSensor - 1 + PG_NB_SENSORS) % PG_NB_SENSORS;
 
  } else if( strcmp( newCommand , "sensor_listen_on_off" ) == 0 ) {
      sensor_onOff[ currentSensor ] = !sensor_onOff[ currentSensor ];
 
  } else if( strcmp( newCommand , "sensorFollowMouse_on_off" ) == 0 ) {
    sensorFollowMouse_onOff = !sensorFollowMouse_onOff;

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// +++++++++++++++++ VIDEO CALIBRATION +++++++++++++++++++++++++ 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
  // has to be explicitely received because it is a header variable
  } else if( strcmp( newCommand , "videoThreshold" ) == 0 ) {
     videoThreshold = arguments[0]; 
     sprintf( AuxString , "/videoThreshold_can %.5f" , videoThreshold ); pg_send_message_udp( (char *)"s" , AuxString , (char *)"udp_PD_send" );
    
  }
  else if (strcmp(newCommand, "currentBGCapture") == 0) {
	  is_currentBG_capture = true;
	  pg_send_message_udp((char *)"s", (char *)"/message_can CURRENT_BG_CAPTURE", (char *)"udp_PD_send");

  }
  else if (strcmp(newCommand, "initialBGCapture") == 0) {
	  is_initialBG_capture = true;
	  pg_send_message_udp((char *)"s", (char *)"/message_can INITIAL_BG_CAPTURE", (char *)"udp_PD_send");

  }
  else if (strcmp(newCommand, "videoFBCapture") == 0) {
	  is_capturedFB = true;

	  
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
    // +++++++++++++++++ MUSIC CONTROL +++++++++++++++++++++++++ 
    // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
  } else if( strcmp( newCommand , "xy_spectrum" ) == 0 ) {
    xy_spectrum[0] = arguments[0]; 
    xy_spectrum[1] = arguments[1]; 
    
  } else if( strcmp( newCommand , "RGBspectrum" ) == 0 ) {
    RGBpalette_received[0] = arguments[0]; 
    RGBpalette_received[1] = arguments[1]; 
    RGBpalette_received[2] = arguments[2]; 
    RGBpalette_received[3] = 0; 
    float value = (RGBpalette_received[0]<RGBpalette_received[1])?RGBpalette_received[1]:RGBpalette_received[0];
    value = (value<RGBpalette_received[2])?RGBpalette_received[2]:value;
    // printf( "RGBpalette_received: %f %f %f\n" , RGBpalette_received[0] , RGBpalette_received[1] , RGBpalette_received[2] );
    if( invertAllLayers ) {
      RGBpalette_received[0] = value - RGBpalette_received[0]; 
      RGBpalette_received[1] = value - RGBpalette_received[1]; 
      RGBpalette_received[2] = value - RGBpalette_received[2]; 
    }
    
  } else if( strcmp( newCommand , "brush_radius_vol" ) == 0 ) {
    brush_radius_vol = arguments[0];

  } else if( strcmp( newCommand , "particle_acc_attack" ) == 0 ) {
    particle_acc_attack = arguments[0] * part_acc_attack_coef; //  * part_acc_attack_coef is made in PD

    // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
    // +++++++++++++++++ BEAT: MUSCI TEMPO SYNC ++++++++++++++++ 
    // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
    // FlashPart (f)                              
    // BackFlash (L)                          
    // ====================================== 
  } else if( strcmp( newCommand , "beat" ) == 0 ) {
    int beatNo = (int)round(arguments[0]);
    sprintf( AuxString , "/loop_beat_can %d" , beatNo % loop_size ); pg_send_message_udp( (char *)"i" , AuxString , (char *)"udp_PD_send" );
      
    // loop start 
    if( ( beatNo % loop_size) == 0 ) {

      // source track recording is on -> source track start 
      if( synchr_start_recording_source_track[ currentDrawingTrack ] ) {
	pg_source_track_recording_onOff( currentDrawingTrack );
	// sets to 0 the recording message since recording has started 
	synchr_start_recording_source_track[ currentDrawingTrack ] = false;
      }

      // target track recording is on -> target track start 
      if( synchr_start_recording_target_track[ currentDrawingTrack ] ) {
	pg_target_track_recording_onOff( currentDrawingTrack );
	// sets to 0 the recording message since recording has started 
	synchr_start_recording_target_track[ currentDrawingTrack ] = false;
      }
      
      // target track reading is on -> target read start 
      if( synchr_start_replay_track[ currentDrawingTrack ] ) {
	pg_track_replay_onOff( currentDrawingTrack );
	synchr_start_replay_track[ currentDrawingTrack ] = false;
      }
    }

    // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
    // +++++++++++++++++ FLASHES +++++++++++++++++++++++++++++++ 
    // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
    // ======================================== 
    // flash triggering according to beat & frequency, weight, and on/off values 
    // ======================================== 
    // flashes are synchronized on beats according to their frequency
    if( flashPart_freq > 0 
      && flashPart_freq <= loop_size // if flashPart_freq > loop_size -> update every frame
      && ( beatNo % (loop_size / flashPart_freq) ) == 0 ) {
      flashPart = flashPart_duration;
    }

#if PG_NB_TRACKS >= 2
    if( flashTrack0_freq_1 > 0 
      && flashTrack0_freq_1 <= loop_size // if flashTrack0_freq_1 > loop_size -> update every frame
      && ( (int)round(arguments[0]) % (loop_size / flashTrack0_freq_1) ) == 0 ) {
	flashTrack0_weights[1] = 1.0;
      // printf( "flashTrack0 (%d)\n" , flashTrack0 );
    }
#endif
#if PG_NB_TRACKS >= 3
    if( flashTrack0_freq_2 > 0 
      && flashTrack0_freq_2 <= loop_size // if flashTrack0_freq_2 > loop_size -> update every frame
      && ( (int)round(arguments[0]) % (loop_size / flashTrack0_freq_2) ) == 0 ) {
	flashTrack0_weights[2] = 1.0;
      // printf( "flashTrack0 (%d)\n" , flashTrack0 );
    }
#endif

    if( flashCA_freq_0 > 0 
      && flashCA_freq_0 <= loop_size // if flashCA_freq_0 > loop_size -> update every frame
      && ( beatNo % (loop_size / flashCA_freq_0) ) == 0 ) {
	flashCA_weights[0] = 1.0;
        // printf( "flashCA 0 (%.2f)\n" , flashCA_weights[0] );
    }
#if PG_NB_TRACKS >= 2
    if( flashCA_freq_1 > 0 
      && flashCA_freq_1 <= loop_size // if flashCA_freq_1 > loop_size -> update every frame
      && ( (int)round(arguments[0]) % (loop_size / flashCA_freq_1) ) == 0 ) {
	flashCA_weights[1] = 1.0;
        // printf( "flashCA 1 (%.2f)\n" , flashCA_weights[1] );
    }
#endif
#if PG_NB_TRACKS >= 3
    if( flashCA_freq_2 > 0 
      && flashCA_freq_2 <= loop_size // if flashCA_freq_0 > loop_size -> update every frame
      && ( (int)round(arguments[2]) % (loop_size / flashCA_freq_2) ) == 0 ) {
	flashCA_weights[2] = 1.0;
        // printf( "flashCA 2 (%.2f)\n" , flashCA_weights[2] );
    }
#endif

    if( flashBack_freq > 0 
      && flashBack_freq <= loop_size // if flashBack_freq > loop_size -> update every frame
      && ( beatNo % (loop_size / flashBack_freq ) ) == 0 ) {
      flashBack_weight = 1.0;
      // printf( "flashBack (%d)\n" , flashCA );
    }

    // ======================================== 
    // flash on/off values
    // ======================================== 
  } else if( strcmp( newCommand , "flashCA_onOff" ) == 0 ) {
	  switch (currentDrawingTrack) {
	  case 0:
		  if (flashCA_freq_0 > 0) {
			  flashCA_freq_0_saved = flashCA_freq_0;
			  flashCA_freq_0 = 0;
		  }
		  else {
			  flashCA_freq_0 = flashCA_freq_0_saved;
		  }
		  break;
#if PG_NB_TRACKS >= 2
	  case 1:
		  if (flashCA_freq_1 > 0) {
			  flashCA_freq_1_saved = flashCA_freq_1;
			  flashCA_freq_1 = 0;
		  }
		  else {
			  flashCA_freq_1 = flashCA_freq_1_saved;
		  }
		  break;
#endif
#if PG_NB_TRACKS >= 3
	  case 2:
		  if (flashCA_freq_2 > 0) {
			  flashCA_freq_2_saved = flashCA_freq_2;
			  flashCA_freq_2 = 0;
		  }
		  else {
			  flashCA_freq_2 = flashCA_freq_2_saved;
		  }
		  break;
#endif
	  }
//  } else if( strcmp( newCommand , "flashTrack0_onOff" ) == 0 ) {
//    switch( currentDrawingTrack ) {
//#if PG_NB_TRACKS >= 2
//	case 1:
//	      if (flashCA_freq_1 > 0) {
//		      flashTrack0_freq_1_saved = flashTrack0_freq_1;
//		      flashTrack0_freq_1 = 0;
//	      }
//	      else {
//		      flashTrack0_freq_1 = flashTrack0_freq_1_saved;
//	      }
//	      break;
//#endif
//#if PG_NB_TRACKS >= 3
//	case 2:
//		if (flashCA_freq_2 > 0) {
//			flashTrack0_freq_2_saved = flashTrack0_freq_2;
//			flashTrack0_freq_2 = 0;
//		}
//		else {
//			flashTrack0_freq_2 = flashTrack0_freq_2_saved;
//		}
//		break;
//#endif
//    }
    
    // ======================================== 
    // Source / target track interpolation start/stop (() 
    // ======================================== 
  //} else if( strcmp( newCommand , "track_interp_on_off" ) == 0 ) {
  //  track_interp[ currentDrawingTrack ] = !track_interp[ currentDrawingTrack ];
  //  if( track_interp[ currentDrawingTrack ] ) {
  //    pg_track_interpolate_start( currentDrawingTrack );
  //  } else {
  //    pg_track_interpolate_stop( currentDrawingTrack );
  //  }
    
    // ======================================== 
    // Source / target track interpolation reset () 
    // ======================================== 
  //} else if( strcmp( newCommand , "pg_track_interpolate_reset" ) == 0 ) {
  //  pg_track_interpolate_reset( currentDrawingTrack );

    // ======================================== 
    // Source / target track interpolation speed: number 
    // of drawings to make the whole transformation () 
    // ======================================== 
  //} else if( strcmp( newCommand , "track_interp_size" ) == 0 ) {
  //    sprintf( AuxString , "/message_can INTERP_SIZE_(%d):%.1f" , currentDrawingTrack , arguments[0] ); pg_send_message_udp( (char *)"s" , AuxString , (char *)"udp_PD_send" );
  //    pg_track_interp_size( currentDrawingTrack , arguments[0] );

    // ======================================== 
    // Track replay speed scale to speed-up track replay (>= 1) () 
    // ======================================== 
  //} else if( strcmp( newCommand , "track_replay_speed_scale" ) == 0 ) {
  //    sprintf( AuxString , "/message_can SPEED_SCALE_(%d):%.1f" , currentDrawingTrack , arguments[0] ); pg_send_message_udp( (char *)"s" , AuxString , (char *)"udp_PD_send" );
  //    pg_track_replay_speed_scale( currentDrawingTrack , arguments[0] );

    // ============================================================ 
    // Source / target track stepwise vs smooth interpolation (y)   
    // ============================================================ 
 // } else if( strcmp( newCommand , "track_interp_step_smooth" ) == 0 ) {
 //   track_stepwise_interpolation[ currentDrawingTrack ] 
 //     = !track_stepwise_interpolation[ currentDrawingTrack ];
 //   pg_track_interpolate_step_smooth( currentDrawingTrack , 
	//track_stepwise_interpolation[ currentDrawingTrack ] );

    // ====================================== 
    // keystroke s: Snapshot                  
    // ====================================== 
  } else if( strcmp( newCommand , "snapshot" ) == 0 ) {
    pg_snapshot( (char *)"jpg" );
    pg_logSceneVariables();

  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
  // +++++++++++++++++ FRAMEBUFFER CAPTURE DISPLAY ON/OFF ++++
  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
  // drawing on off void/currentDrawing mode (w)   
  // ============================================= 
  } else if (strcmp(newCommand, "FBCaptureOnOff") == 0) {
	  if (FBCaptureWeight > 0.0f) {
		  FBCaptureWeight = 0.0f;
	  }
	  else {
		  FBCaptureWeight = 1.0f;
	  }

  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
  // +++++++++++++++++ DRAWING ON/OFF ++++++++++++++++++++++++ 
  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
  // drawing on off void/currentDrawing mode (w)   
  // ============================================= 
  } else if( strcmp( newCommand , "trackSwapCompositionFinalWeights" ) == 0 ) {
    float tmpw;
    switch( currentDrawingTrack ) {
      case 0:
	tmpw = trackCompositionWeight_0;
	trackCompositionWeight_0 = trackFinalWeight_0;
	trackFinalWeight_0 = tmpw;
        BrokenInterpolationVar[ _trackCompositionWeight_0 ] = true;
        BrokenInterpolationVar[ _trackFinalWeight_0 ] = true;
	break;
#if PG_NB_TRACKS >= 2
      case 1:
	tmpw = trackCompositionWeight_1;
	trackCompositionWeight_1 = trackFinalWeight_1;
	trackFinalWeight_1 = tmpw;
        BrokenInterpolationVar[ _trackCompositionWeight_1 ] = true;
        BrokenInterpolationVar[ _trackFinalWeight_1 ] = true;
	break;
#endif
#if PG_NB_TRACKS >= 3
      case 2:
	tmpw = trackCompositionWeight_2;
	trackCompositionWeight_2 = trackFinalWeight_2;
	trackFinalWeight_2 = tmpw;
        BrokenInterpolationVar[ _trackCompositionWeight_2 ] = true;
        BrokenInterpolationVar[ _trackFinalWeight_2 ] = true;
	break;
#endif
    }
    
  } else if( strcmp( newCommand , "CASwapCompositionFinalWeights" ) == 0 ) {
    float tmpw = CACompositionWeight;
    CACompositionWeight = CAFinalWeight;
    CAFinalWeight = tmpw;
    BrokenInterpolationVar[ _CACompositionWeight ] = true;
    BrokenInterpolationVar[ _CAFinalWeight ] = true;
    
  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
  // +++++++++++++++++ SWAPS TRACK & CA WEIGHTS ++++++++++++++ 
  // +++++++++++++++++ BTWEEN COMPOSITION & FINAL ++++++++++++ 
  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
  // drawing on off void/currentDrawing mode (w)   
  // ============================================= 
  } else if( strcmp( newCommand , "drawingOnOff" ) == 0 ) {
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
  //} else if( strcmp( newCommand , "frozen_on_off" ) == 0 ) {
  //  if( frozen == 0 ) {
  //    frozen = 1;
  //    particleMode_saved = particleMode;
  //    CASubType_saved = CASubType;
  //    CAType_saved = CAType;
  //    particleMode = 0;
  //    CASubType = 0;
  //    CAType = 0;
  //    CA_onOff = false;
  //  } else {
  //    frozen = 0;
  //    particleMode = particleMode_saved;
  //    CASubType =  CASubType_saved;
  //    CAType = CAType_saved;
  //    CA_onOff = true;
  //  }

    // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
    // +++++++++++++++++ CA MEMORY +++++++++++++++++++++++++++++ 
    // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
    // stores or restaures CA modes
    // ====================================== 
  } else if( strcmp( newCommand , "CAmem_1" ) == 0 ) {
    CAType_memory[0] = CAType;
    CASubType_memory[0] = CASubType;
    CAParams1_memory[0] = CAParams1;
    CAParams2_memory[0] = CAParams2;
    CAParams3_memory[0] = CAParams3;
    CAParams4_memory[0] = CAParams4;
    CAParams5_memory[0] = CAParams5;
    CAParams6_memory[0] = CAParams6;
    CAParams7_memory[0] = CAParams7;
    CAParams8_memory[0] = CAParams8;

  } else if( strcmp( newCommand , "CAmem_2" ) == 0 ) {
    CAType_memory[1] = CAType;
    CASubType_memory[1] = CASubType;
    CAParams1_memory[1] = CAParams1;
    CAParams2_memory[1] = CAParams2;
    CAParams3_memory[1] = CAParams3;
    CAParams4_memory[1] = CAParams4;
    CAParams5_memory[1] = CAParams5;
    CAParams6_memory[1] = CAParams6;
    CAParams7_memory[1] = CAParams7;
    CAParams8_memory[1] = CAParams8;

  } else if( strcmp( newCommand , "CArecall_1" ) == 0 ) {
    CAType = CAType_memory[0];
    CASubType = CASubType_memory[0];
    CAParams1 = CAParams1_memory[0];
    CAParams2 = CAParams2_memory[0];
    CAParams3 = CAParams3_memory[0];
    CAParams4 = CAParams4_memory[0];
    CAParams5 = CAParams5_memory[0];
    CAParams6 = CAParams6_memory[0];
    CAParams7 = CAParams7_memory[0];
    CAParams8 = CAParams8_memory[0];

  } else if( strcmp( newCommand , "CArecall_2" ) == 0 ) {
    CAType = CAType_memory[1];
    CASubType = CASubType_memory[1];
    CAParams1 = CAParams1_memory[1];
    CAParams2 = CAParams2_memory[1];
    CAParams3 = CAParams3_memory[1];
    CAParams4 = CAParams4_memory[1];
    CAParams5 = CAParams5_memory[1];
    CAParams6 = CAParams6_memory[1];
    CAParams7 = CAParams7_memory[1];
    CAParams8 = CAParams8_memory[1];

  } else if( strcmp( newCommand , "currentDrawingTrack_plus" ) == 0 ) {
      currentDrawingTrack = (currentDrawingTrack + 1) % PG_NB_TRACKS;

  } else if( strcmp( newCommand , "currentVideoTrack_plus" ) == 0 ) {
     currentVideoTrack = (currentVideoTrack + 1) % PG_NB_TRACKS;

  } else if( strcmp( newCommand , "BGSubtr_plus" ) == 0 ) {
     BGSubtr = (BGSubtr + 1) % 3;
     BrokenInterpolationVar[ _BGSubtr ] = true;

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
    if( currentDrawingTrack == 1 ) {
      // switches drawing mode to void 
      activeDrawingStroke = 0;
      pg_apply_new_activeDrawingStroke();
    }
    // stops reading 
    pg_track_replay_stop( 1 );
  }

  if( currentSVGTrack == 0 ) {
    load_svg_track( (char *)("./textures/SVGs/fall_src_01_01.svg") , 
		    1 , true );
    load_svg_track( (char *)("./textures/SVGs/fall_src_02_01.svg") , 
		    2 , true );
    load_svg_track( (char *)("./textures/SVGs/fall_tgt_01_01.svg") , 
		    1 , false );
    load_svg_track( (char *)("./textures/SVGs/fall_tgt_02_01.svg") , 
		    2 , false );
  }
#endif
#if PG_NB_TRACKS >= 3
  // stops ongoing readings 
  track_interp[2] = false;
  pg_track_interpolate_stop( 2 );
  if( track_replay_2 ) {
    if( currentDrawingTrack == 2 ) {
      // switches drawing mode to void 
      activeDrawingStroke = 0;
      pg_apply_new_activeDrawingStroke();
    }
    // stops reading 
    pg_track_replay_stop( 2 );
  }
  if( currentSVGTrack == 1 ) {
    load_svg_track( (char *)("./textures/SVGs/fall_src_01_02.svg") , 
		    1 , true );
    load_svg_track( (char *)("./textures/SVGs/fall_src_02_02.svg") , 
		    2 , true );
    load_svg_track( (char *)("./textures/SVGs/fall_tgt_01_02.svg") , 
		    1 , false );
    load_svg_track( (char *)("./textures/SVGs/fall_tgt_02_02.svg") , 
		    2 , false );
  }
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
    bool isTargetTrackRecord = false;
    switch( indTrack ) {
      case 0:
	isTrackReplay = track_replay_0;
	isTargetTrackRecord = target_track_record_0;
	break;
#if PG_NB_TRACKS >= 2
      case 1:
	isTrackReplay = track_replay_1;
	isTargetTrackRecord = target_track_record_1;
	break;
#endif
#if PG_NB_TRACKS >= 3
      case 2:
	isTrackReplay = track_replay_2;
	isTargetTrackRecord = target_track_record_2;
	break;
#endif
    }

    // is reading -> has to stop reading 
    if( isTrackReplay ) {
      pg_track_replay_stop( indTrack );
    }
    
    // is recording target -> has to stop recording target 
    if( isTargetTrackRecord ) {
      pg_target_track_recording_stop( indTrack );
    }

    // launches source recording 
    pg_source_track_recording_start( indTrack );

    // switches drawing mode to currentDrawingStroke 
    activeDrawingStroke = currentDrawingStroke + 1;
    pg_apply_new_activeDrawingStroke();
    
  } else {
    // stops source recording 
    pg_source_track_recording_stop( indTrack );

    // switches drawing mode to void 
    activeDrawingStroke = 0;
    pg_apply_new_activeDrawingStroke();
  }
}

void pg_target_track_recording_onOff( int indTrack ) {
  // switches between recording on/off 
    bool isTargetTrackRecord = false;
    switch( indTrack ) {
      case 0:
	target_track_record_0 = !target_track_record_0;
        isTargetTrackRecord = target_track_record_0;
	break;
#if PG_NB_TRACKS >= 2
      case 1:
	target_track_record_1 = !target_track_record_1;
	isTargetTrackRecord = target_track_record_1;
	break;
#endif
#if PG_NB_TRACKS >= 3
      case 2:
	target_track_record_2 = !target_track_record_2;
	isTargetTrackRecord = target_track_record_2;
	break;
#endif
    }

      
  // starts recording target 
  if( isTargetTrackRecord ) {

    bool isTrackReplay = false;
    bool isSourceTrackRecord = false;
    switch( indTrack ) {
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

    // is reading -> has to stop reading 
    if( isTrackReplay ) {
      pg_track_replay_stop( indTrack );
    }

    // is recording source -> has to stop recording source 
    if( isSourceTrackRecord ) {
      pg_source_track_recording_stop( indTrack );
    }

    // launches target recording 
    pg_target_track_recording_start( indTrack );

    // switches drawing mode to currentDrawingStroke 
    activeDrawingStroke = currentDrawingStroke + 1;
    pg_apply_new_activeDrawingStroke();
  
  } else {
    // stops target recording 
    pg_target_track_recording_stop( indTrack );
  }
}

void pg_track_replay_onOff( int trackNo ) {
    bool isTrackReplay = false;
    bool isSourceTrackRecord = false;
    bool isTargetTrackRecord = false;
    switch( trackNo ) {
      case 0:
	isTrackReplay = track_replay_0;
	isSourceTrackRecord = source_track_record_0;
	isTargetTrackRecord = target_track_record_0;
	break;
#if PG_NB_TRACKS >= 2
      case 1:
	isTrackReplay = track_replay_1;
	isSourceTrackRecord = source_track_record_1;
	isTargetTrackRecord = target_track_record_1;
	break;
#endif
#if PG_NB_TRACKS >= 3
      case 2:
	isTrackReplay = track_replay_2;
	isSourceTrackRecord = source_track_record_2;
	isTargetTrackRecord = target_track_record_2;
	break;
#endif
    }

  // is not currently reading -> starts reading 
  if( !isTrackReplay ) {
    // printf( "start_read_source_track\n");

    // is recording source -> has to stop recording source 
    if( isSourceTrackRecord ) {
      pg_source_track_recording_stop( trackNo );
    }

    // is recording target -> has to stop recording target 
    if( isTargetTrackRecord ) {
      pg_target_track_recording_stop( trackNo );
    }

    // start reading 
    pg_track_replay_start( trackNo );

    // switches drawing mode to void if the read track is the current one
    if( trackNo == currentDrawingTrack ) {
      activeDrawingStroke = 0;
      pg_apply_new_activeDrawingStroke();
    }
  
    // is currently reading -> stops reading 
  } else {
    // printf( "stop_read_source_track\n");

    // switches drawing mode to void  if the read track is the current one
    if( trackNo == currentDrawingTrack ) {
      activeDrawingStroke = 0;
      pg_apply_new_activeDrawingStroke();
    }
    
    // stops reading 
    pg_track_replay_stop( trackNo );

  }
}

void pg_source_track_recording_start( int indTrack ) {
    switch( indTrack ) {
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
  if( indTrack >= 0 
      && indTrack < PG_NB_TRACKS ) {
    TrackStatus_source[ indTrack ].isActiveRecording = true;
    TrackStatus_source[ indTrack ].nbRecordedFrames = 0;
    TrackStatus_target[ indTrack ].isNormalized = false;
    // printf("start recording track %d\n",indTrack);
  }
}
void pg_target_track_recording_start( int indTrack ) {
    switch( indTrack ) {
      case 0:
	target_track_record_0 = true;
	break;
#if PG_NB_TRACKS >= 2
      case 1:
	target_track_record_1 = true;
	break;
#endif
#if PG_NB_TRACKS >= 3
      case 2:
	target_track_record_2 = true;
	break;
#endif
    }
  if( indTrack >= 0 
      && indTrack < PG_NB_TRACKS ) {
    TrackStatus_target[ indTrack ].isActiveRecording = true;
    TrackStatus_target[ indTrack ].nbRecordedFrames = 0;
    TrackStatus_target[ indTrack ].isNormalized = false;
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
void pg_target_track_recording_stop( int indTrack ) {
    switch( indTrack ) {
      case 0:
	target_track_record_0 = false;
	break;
#if PG_NB_TRACKS >= 2
      case 1:
	target_track_record_1 = false;
	break;
#endif
#if PG_NB_TRACKS >= 3
      case 2:
	target_track_record_2 = false;
	break;
#endif
    }
  if( indTrack >= 0 
      && indTrack < PG_NB_TRACKS 
      && TrackStatus_target[ indTrack ].isActiveRecording == true ) {
    TrackStatus_target[ indTrack ].isActiveRecording = false;
    
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
