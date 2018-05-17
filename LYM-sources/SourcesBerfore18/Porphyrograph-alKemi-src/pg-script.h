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

/////////////////////////////////////////////////////////////////////////
// size

// number of SVG tracks 
#define nb_SVG_tracks 2

/////////////////////////////////////////////////////////////////////////
// initial values

// ++++++++++++++++++++++ DRAWING MODE +++++++++++++++++++++ 
#define activeDrawingStroke_ini 0

// ++++++++++++++++++++++ MUSIC CONTROL ++++++++++++++++++++ 
#define xy_spectrum_ini 0.0
#define brush_radius_vol_ini 0
#define color_spectrum_r_ini 0.0
#define color_spectrum_g_ini 0.0
#define color_spectrum_b_ini 0.0
#define color_spectrum_a_ini 0.0
#define particle_acc_attack_ini 0.0

// per track replay interpolation 
#define track_interp_0_ini false
#define track_interp_1_ini false
#define track_interp_2_ini false
#define track_stepwise_interpolation_0_ini true
#define track_stepwise_interpolation_1_ini true
#define track_stepwise_interpolation_2_ini true

// track loops
#define loop_size 8  // standrd 8 beats/loop size

//////////////////////////////////////////////
// scenario variables
extern bool BrokenInterpolationVar[ _MaxInterpVarIDs ];
extern float InitialValuesInterpVar[ _MaxInterpVarIDs ];
extern float InitialValuesConfigurationVar[ _MaxConfigurationVarIDs ];

//////////////////////////////////////////////
// CA initialization
extern float initCA;

/////////////////////////////////////////////////////////////////////////
// WORKING VARIABLES
/////////////////////////////////////////////////////////////////////////

extern std::string project_name;
extern char AuxString[1024];
extern float InitialValuesInterpVar[ _MaxInterpVarIDs ];

// ++++++++++++++++++++++ SCENARIO +++++++++++++++++++++++++ 
extern int cur_scene;

// ++++++++++++++++++++++ DRAWING MODE +++++++++++++++++++++ 
extern int activeDrawingStroke;

extern float isBeat; // boolean for dot-based drawing

// ++++++++++++++++++++++ CA +++++++++++++++++++++++++++++++ 
// various type/subtypes of cellular automata 0=GOL, 1=...  
// extern int CAType;
extern int CAType_saved; // for freeze 
// extern int CASubType;
extern int CASubType_saved; // for freeze 
extern bool CA_onOff; // CA on off flag

// ++++++++++++++++++++++ PARTICLES ++++++++++++++++++++++++ 
// particles on/off  
// extern int particleMode;
extern int particleMode_saved; // for freeze 

// ++++++++++++++++++++++ MUSIC CONTROL ++++++++++++++++++++ 
// music control inputs received at each frame
// pointer motion
extern float xy_spectrum[2];
// brush radius
extern float brush_radius_vol;
// addition of color based on palette
extern float color_spectrum[4];

// factor increasing the acceleration weight of particles
// based on sound volume attacks
extern float particle_acc_attack;

// per traget track display 
extern bool target_track_display[PG_NB_TRACKS];

// per track replay interpolation 
extern bool track_interp[PG_NB_TRACKS];
extern bool track_stepwise_interpolation[PG_NB_TRACKS];

// track loops
// track synchronization with beats
extern int freq_flash_particle;
extern int freq_flash_CA;
extern int freq_flash_track;
extern int freq_flash_back;

// ++++++++++++++++++++++ FLASHES +++++++++++++++++++++++++
// flash: cellular automata refeeding  
extern int flashCA;
//extern float flashCA_coef;
//extern int flashCA_number;
// flash: track 0 refeeding  
extern int flashBack;
//extern float flashBack_coef;
// flash: extension of particle reproduction  
extern int flashPart;
//extern int flashPart_duration;
// flash: track to BG copy  
extern int flashTrack0;
//extern float flashTrack0_coef;
//extern int flashTrack0_number;

// +++++++++++++++++++++ CLEAR +++++++++++++++++++++++++++
extern int isClearCA;
extern int isClearAllLayers;

// ++++++++++++++++++++++ CA and TACKS WOKING VARIABLE ++++
// CA and track working variable
// memory of current track in case of temporary background track control 
extern int currentTrack_memory;

// scene management
extern bool firstFrameInScene;
extern float remainingTimeInScene;

extern bool synchr_start_recording_source_track[PG_NB_TRACKS];
extern bool synchr_start_replay_track[PG_NB_TRACKS];

// track loading 
extern int currentSVGTrack;

// working variables 
extern int drawing_start_frame;
extern int backtrack_begin_time;

extern bool frozen;

// ++++++++++++++++++++++ PARTICLES ACC THROUGH SNAPSHOT ++++++++++++++++++++++++ 
extern bool textureSnapshot;
extern int new_textureSnapshot;
extern int loadsvg_textureSnapshot_frame;

// previously stored on composition
extern int curentNbOSCParams;

// scenario launching time
extern double InitialScenarioTime;

// +++++++++++++++++++++++ Beats +++++++++++++++++++++++++++++++++
#define BEAT_DURATION (1.0/2.0)
extern int BeatNo;
extern double lastBeatTime;

enum pg_Parameter_Input_Type { _PG_KEYSTROKE = 0, _PG_GUI_COMMAND, _PG_SCENARIO, _PG_INPUT_VOID };
enum pg_Keystroke_Input_Type {
	_PG_KEYSTROKE_MINUS = 0, _PG_KEYSTROKE_PLUS, _PG_KEYSTROKE_VOID
};

void pg_initializationScript( void );
void InitializeConfigurationVar(void);
void pg_displaySceneVariables( void );
void pg_send_message_udp( char *pattern , char * message , char *targetHostid );
void pg_logSceneVariables( void );
void pg_update_scenario( void );
void pg_process_key( int key );
void pg_process_special_key( int key );
void pg_keyStrokeScripts( int key );
void pg_continuous_flahes( void );
void pg_aliasScript( char * command_pattern , 
		     char * string_argument_0 ,
		     float arguments[MAX_OSC_ARGUMENTS] );
void loadsvg( void );
void pg_source_track_recording_onOff( int indTrack );
void pg_target_track_recording_onOff( int indTrack );
// playing track onoff
void pg_track_replay_onOff( int trckNo );
void track_replay_callBack( int trackNo , pg_Parameter_Input_Type param_input_type , float scenario_or_gui_command_value );
void source_track_display_callBack( int trackNo , pg_Parameter_Input_Type param_input_type , float scenario_or_gui_command_value );
void source_track_record_callBack( int trackNo , pg_Parameter_Input_Type param_input_type , float scenario_or_gui_command_value );
void target_track_record_callBack( int trackNo , pg_Parameter_Input_Type param_input_type , float scenario_or_gui_command_value );
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
