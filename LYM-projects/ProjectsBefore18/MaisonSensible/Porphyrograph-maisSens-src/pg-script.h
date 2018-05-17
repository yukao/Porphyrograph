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


/////////////////////////////////////////////////////////////////////////
// initial values

// ++++++++++++++++++++++ DRAWING MODE +++++++++++++++++++++ 
#define activeDrawingStroke_ini 1

// ++++++++++++++++++++++ MUSIC CONTROL ++++++++++++++++++++ 
#define color_spectrum_r_ini 0.0
#define color_spectrum_g_ini 0.0
#define color_spectrum_b_ini 0.0
#define color_spectrum_a_ini 0.0
#define particle_acc_attack_ini 0.0
#define brush_radius_vol_ini 0.0

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
extern float InitialValuesInterpVar[ _MaxInterpVarIDs ];

extern char AuxString[1024];

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
// addition of color based on palette
extern float color_spectrum[4];

// music control of radius
extern float brush_radius_vol;

// factor increasing the acceleration weight of particles
// based on sound volume attacks
extern float particle_acc_attack;

// ++++++++++++++++++++++ CA and TACKS WOKING VARIABLE ++++
// CA and track working variable
// memory of current track in case of temporary background track control 
extern int currentTrack_memory;

// scene management
extern bool firstFrameInScene;
extern float remainingTimeInScene;

// working variables 
extern int drawing_start_frame;
extern int backtrack_begin_time;

extern bool frozen;

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
void pg_aliasScript( char * command_pattern , 
		     char * string_argument_0 ,
		     float arguments[MAX_OSC_ARGUMENTS] );

void pg_writeMessageOnScreen( char *text );
void pg_snapshot( char * type );
// drawing on/off
void  pg_apply_new_activeDrawingStroke( void );

#endif
