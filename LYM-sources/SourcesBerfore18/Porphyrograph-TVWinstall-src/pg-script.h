/*! \file pg-script.h
 *
 *
 *
 *
 *     File pg-script.h
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
#ifndef PG_SCRIPT_H
#define PG_SCRIPT_H

/////////////////////////////////////////////////////////////////////////
// WORKING CONSTANTS
/////////////////////////////////////////////////////////////////////////

extern int IndCurrentScene;

extern char AuxString[1024];


/////////////////////////////////////////////////////////////////////////
// size

// number of SVG tracks
#define nb_SVG_tracks 2

/////////////////////////////////////////////////////////////////////////
// initial values

// ++++++++++++++++++++++ DRAWING MODE +++++++++++++++++++++
#define activeDrawingStroke_ini 0

// ++++++++++++++++++++++ MUSIC CONTROL ++++++++++++++++++++
#define RGBpalette_r_ini 0.0
#define RGBpalette_g_ini 0.0
#define RGBpalette_b_ini 0.0
#define RGBpalette_a_ini 0.0

// per track replay interpolation
#define track_interp_0_ini false
#define track_interp_1_ini false
#define track_interp_2_ini false
#define track_stepwise_interpolation_0_ini true
#define track_stepwise_interpolation_1_ini true
#define track_stepwise_interpolation_2_ini true

// track loops
#define PG_LOOP_SIZE 8  // standrd 8 beats/loop size

//////////////////////////////////////////////
// scenario variables
extern bool BrokenInterpolationVar[ _MaxInterpVarIDs ];
extern float InitialValuesInterpVar[ _MaxInterpVarIDs ];
extern float InitialValuesConfigurationVar[ _MaxConfigurationVarIDs ];

extern bool tracksSync;


#ifdef PG_WITH_CA
//////////////////////////////////////////////
// CA initialization
extern float initCA;
#endif

/////////////////////////////////////////////////////////////////////////
// WORKING VARIABLES
/////////////////////////////////////////////////////////////////////////

extern std::string project_name;
extern float InitialValuesInterpVar[ _MaxInterpVarIDs ];

// ++++++++++++++++++++++ SCENARIO +++++++++++++++++++++++++
extern int CurrentScene;

// +++++++++++++++++++++ CLEAR +++++++++++++++++++++++++++
#ifdef PG_WITH_CA
extern int isClearCA;
#endif
extern int isClearLayer;
extern int isClearAllLayers;

// +++++++++++++++++++++ NEW BEAT +++++++++++++++++++++++++++
extern int isNewBeat;

// +++++++++++++++++++++ COPY LAYERS +++++++++++++++++++++++++++
extern int isCopyTrack12;

// ++++++++++++++++++++++ SET-UP +++++++++++++++++++++++++++
extern int CurrentScene;
extern int CurrentHashtag;

// scene management
extern bool FirstFrameInScene;
extern float remainingTimeInScene;

// working variables
extern int drawing_start_frame;

// SCENARIO LAUNCH TIME
extern double InitialScenarioTime;
// PERFORMANCE LAUNCH TIME
extern double InitialPerformanceTime;

// ++++++++++++++++++++++ DRAWING MODE +++++++++++++++++++++
extern int activeDrawingStroke;

#ifdef PG_WITH_CA
// ++++++++++++++++++++++ CA +++++++++++++++++++++++++++++++
extern bool CA_onOff; // CA on off flag
#endif

// ++++++++++++++++++++++ MUSIC CONTROL ++++++++++++++++++++
// music control inputs received at each frame
// addition of color based on palette
extern float RGBpalette_received[4];

#ifdef PG_TRACK_DISPLAY
// per traget track display
extern bool target_track_display[PG_NB_TRACKS];
#endif

// per track replay interpolation
extern bool track_interp[PG_NB_TRACKS];
extern bool track_stepwise_interpolation[PG_NB_TRACKS];

// ++++++++++++++++++++++ FLASHES +++++++++++++++++++++++++
#ifdef PG_WITH_CA
extern float flashBack_weight;
// flash: cellular automata refeeding
extern float flashCA_weights[PG_NB_TRACKS];
#endif

// flash: extension of particle reproduction
extern int flashPart;
// flash: track to BG copy
extern float flashTrack0_weights[PG_NB_TRACKS];

// +++++++++++++++++++++ VIDEO BACKGROUND SUBTRACTION +++++
extern bool is_currentBG_capture;
extern bool is_initialBG_capture;

// +++++++++++++++++++++ VIDEO FLASH +++++++++++++++++++++
extern bool is_flashVideo;
extern float flashVideoCoef;
extern float flashVideoDecay;
extern float flashVideoThreshold;

// ++++++++++++++++++++++ CA and TACKS WOKING VARIABLE ++++
// CA and track working variable

// scene management
extern bool FirstFrameInScene;
extern float remainingTimeInScene;

extern bool synchr_start_recording_source_track[PG_NB_TRACKS];
extern bool synchr_start_recording_target_track[PG_NB_TRACKS];
extern bool synchr_start_replay_track[PG_NB_TRACKS];

// track loading
extern int currentSVGTrack;

// working variables
extern int drawing_start_frame;
extern int backtrack_begin_time;

// previously stored on composition
extern int curentNbOSCParams;

// scenario launching time
extern double InitialScenarioTime;
// PERFORMANCE LAUNCH TIME
extern double InitialPerformanceTime;

// +++++++++++++++++++++++ Beats +++++++++++++++++++++++++++++++++
extern int BeatNo;
extern double lastAutoBeatTime;
extern double lastReceivedBeatTime;
extern float pulse[3];

enum pg_Parameter_Input_Type { _PG_KEYSTROKE = 0, _PG_GUI_COMMAND, _PG_SCENARIO, _PG_INPUT_VOID };
enum pg_Keystroke_Input_Type {
	_PG_KEYSTROKE_MINUS = 0, _PG_KEYSTROKE_PLUS, _PG_KEYSTROKE_VOID
};

void pg_initializationScript( void );
void InitializeConfigurationVar(void);
void pg_displaySceneVariables( void );
void pg_send_message_udp( char *pattern , char * message , char *targetHostid );
void pg_logCurrentLineSceneVariables(char *fileName);
void pg_logFirstLineSceneVariables(void);
void pg_update_scene_number_for_visuals( bool new_scene );
void pg_update_scenario(void);
void pg_process_key( int key );
void pg_process_special_key( int key );
void pg_launch_performance(void);
void pg_keyStrokeScripts( int key );
void pg_continuous_flahes( void );
void firstSceneSetup(void);
void pg_aliasScript( char * command_pattern ,
		     char * string_argument_0 ,
		     float arguments[MAX_OSC_ARGUMENTS] );
void loadsvg( void );
void pg_source_track_recording_onOff( int indTrack );
// playing track onoff
void pg_track_replay_onOff( int trckNo );
void track_replay_callBack( int trackNo , pg_Parameter_Input_Type param_input_type , float scenario_or_gui_command_value );
#ifdef PG_TRACK_DISPLAY
void source_track_display_callBack( int trackNo , pg_Parameter_Input_Type param_input_type , float scenario_or_gui_command_value );
#endif
void source_track_record_callBack( int trackNo , pg_Parameter_Input_Type param_input_type , float scenario_or_gui_command_value );
void pg_source_track_recording_start( int indTrack );
void pg_target_track_recording_start( int indTrack );
// recording on off
void pg_source_track_recording_stop( int indTrack );
void pg_target_track_recording_stop( int indTrack );
// interpolation on off
void pg_track_interpolate_stop( int indTrack );
void pg_track_replay_start( int indTrack );
void pg_track_replay_stop( int indTrack );
void pg_track_interp_size( int indTrack , float nbFrames );
void pg_track_interpolate_start( int indTrack );
void pg_track_interpolate_step_smooth( int indTrack , bool isStepwiseInterpolation );
void pg_track_replay_speed_scale( int indTrack , float speed );
// stepwise interpolation on off
void pg_track_interpolate_reset( int indTrack );

void pg_writeMessageOnScreen( char *text );
void pg_snapshot( char * type );
// drawing on/off
void  pg_apply_new_activeDrawingStroke( void );

#endif
