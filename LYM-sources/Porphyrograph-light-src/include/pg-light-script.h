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

// number of ClipArt tracks 
#define nb_ClipArt_tracks 2

/////////////////////////////////////////////////////////////////////////
// initial values

// ++++++++++++++++++++++ MUSIC CONTROL ++++++++++++++++++++ 
#define RGBpalette_r_ini 0.0
#define RGBpalette_g_ini 0.0
#define RGBpalette_b_ini 0.0
#define RGBpalette_a_ini 0.0
#define particle_acc_attack_ini 0.0

// per track replay interpolation 
#define track_interp_ini false
#define track_stepwise_interpolation_ini true

// track loops 
#define PG_LOOP_SIZE 8  // standrd 8 beats/loop size

enum pg_Keystroke_Input_Type { pg_enum_KEYSTROKE_MINUS = 0, pg_enum_KEYSTROKE_PLUS, pg_enum_KEYSTROKE_VOID };

#define CA_SQUENCR                0
#define CA_TOTALISTIC             1
#define CA_GENERATION             2
#define CA_GAL_BIN_MOORE          3
#define CA_GAL_BIN_NEUMANN        4
#define CA_NEUMANN_BINARY         5

#if defined(pg_Project_CAaudio)
#define CA_TOTALISTIC             0
#define CA_GENERATION             1
#define CA_GAL_BIN_MOORE          2
#define CA_GAL_BIN_NEUMANN        3
#define CA_NEUMANN_BINARY         4
#endif

#define PG_NB_PARTEXIT_MODES 3
#define PG_NB_PARTSTROKE_MODES 4
#define PG_NB_PARTCOLOR_MODES 3

/////////////////////////////////////////////////////////////////////////
// WORKING VARIABLES
/////////////////////////////////////////////////////////////////////////

// echo modulator for clips
extern float pg_fx_dry_wet;

//////////////////////////////////////////////
// SCENARIO AND CONFIGURATION VARIABLES
// changed to true when a value is changed manually during scenario
extern bool pg_BrokenInterpolationVar[_MaxInterpVarIDs];
// initial values in the scenario (before first scene)
class ScenarioValue {
public:
	double val_num;
	string val_string;
	double* val_array;
	int array_size;
	ScenarioValue() {
		val_num = 0;
		val_string = "";
		val_array = NULL;
		array_size = 0;
	}
	ScenarioValue(double d, string s, double* v_array, int array_size) {
		init_ScenarioValue(d, s, v_array, array_size);
	}
	void init_ScenarioValue(double d, string s, double* v_array, int a_size) {
		val_num = d;
		val_string = s;
		if (a_size > 0) {
			val_array = new double[a_size];
			array_size = a_size;
		}
		if (v_array != NULL) {
			for (int i = 0; i < array_size; i++) {
				val_array[i] = v_array[i];
			}
		}
	}
	void const_value(double d) {
		if (val_array != NULL && array_size > 0) {
			for (int i = 0; i < array_size; i++) {
				val_array[i] = d;
			}
		}
		else {
			val_num = d;
		}
	}
	double get_array_value(int ind) {
		if (val_array != NULL && ind >= 0 && ind < array_size) {
			return val_array[ind];
		}
		printf("get_array_value: Expected array of min size %d, not %d\n", ind + 1, array_size);
		return 0.f;
	}
	void set_array_value(int ind, double val) {
		if (val_array != NULL && ind >= 0 && ind < array_size) {
			val_array[ind] = val;
			return;
		}
		printf("set_array_value: Expected array of min size %d, not %d\n", ind + 1, array_size);
	}
	~ScenarioValue() {}
};

//////////////////////////////////////////////
// beat no
extern int pg_BeatNo;

//////////////////////////////////////////////
// MIDI event creates a local stroke
extern int pg_FourFrameStrokeNb;
extern int pg_FourFrameStroke_x;
extern int pg_FourFrameStroke_y;

// ++++++++++++++++++++++ GLUT MODIFIER ++++++++++++++++++++ 
extern int pg_key_modifier;

// ++++++++++++++++++++++ MUSIC CONTROL ++++++++++++++++++++ 
// music control inputs received at each frame
// pg_audio_pulse preceding value
extern float pg_audio_pulse_prec[3];

// factor increasing the acceleration weight of particles
// based on sound volume attacks
// brush radius
// particle radius
extern float pulse_average;
extern float pg_audio_pulse_average_prec;
// not used currently extern float pulse_attack;

// +++++++++++++++++++++++ Beats +++++++++++++++++++++++++++++++++
extern float pg_audio_pulse[3];
extern float pg_seed_pulsePerlinNoise[4 * 2];

extern std::unordered_map<std::string, int> pg_OSC_addresses_hashMap;

extern double pg_lastPenTouchTime;

#if defined(var_GenerativeNights_planes)
//////////////////////////////////////////////
/// image number of first plane
extern int      firstPlaneFrameNo;

//////////////////////////////////////////////
// CA initialization
extern float initCA;
#endif

// ++++++++++++++++++++++ SCENARIO +++++++++++++++++++++++++ 
extern int pg_CurrentSceneIndex;
class Scene;
extern Scene* pg_CurrentScene;

// DELAYED CAMERA WEIGHT
extern int pg_delayedCameraWeight;

#if defined(PG_WITH_BLUR)
// +++++++++++++++++++++ BLUR +++++++++++++++++++++++++++
extern bool is_blur_1;
extern bool is_blur_2;
extern int nb_blur_frames_1;
extern int nb_blur_frames_2;
#endif

// +++++++++++++++++++++ COPY LAYERS +++++++++++++++++++++++++++
// copy to layer above (+1) or to layer below (-1)
extern int pg_copyToNextTrack;

// pen preset
extern int pg_current_pen_colorPreset;

// ++++++++++++++++++++++ CA and TACKS WOKING VARIABLE ++++
// CA and track working variable

// +++++++++++++++++++++++ FFT levels and frequency storage ++++++++++++++++++++
#if defined(pg_Project_Criton)
extern float fftLevels[8];
extern float fftFrequencies[8];
extern float fftPhases[8];
#endif

// master automatic incay/decay
extern double pg_master_incay_duration;
extern double pg_master_incay_start_time;
extern float pg_master_incay_start_value;
extern double pg_master_decay_duration;
extern double pg_master_decay_start_time;
extern float pg_master_decay_start_value;

// +++++++++++++++++++++++ Beats +++++++++++++++++++++++++++++++++
extern double pg_lastBeatTime;

// +++++++++++++++++++++++ CA seeding +++++++++++++++++++++++++++++++++
#if defined(pg_Project_CAaudio)
enum pg_CAseed_types
{
	_pg_CAseed_dot_center = 0,
	_pg_CAseed_dot,
	_pg_CAseed_h_line,
	_pg_CAseed_v_line,
	_pg_CAseed_cross,
	_pg_CAseed_X,
	_pg_CAseed_square,
	_pg_Nb_CAseed_types,
};
enum pg_CAseed_locations
{
	_pg_CAseed_loc_center = 0,
	_pg_CAseed_loc_N,
	_pg_CAseed_loc_E,
	_pg_CAseed_loc_S,
	_pg_CAseed_loc_W,
	_pg_CAseed_loc_NW,
	_pg_CAseed_loc_NE,
	_pg_CAseed_loc_SE,
	_pg_CAseed_loc_SW,
	_pg_Nb_CAseed_locations,
}; extern pg_CAseed_types pg_CAseed_type;
extern pg_CAseed_locations pg_CAseed_location;
extern int pg_CAseed_coordinates[2];
extern int pg_CAseed_size;
extern bool pg_CAseed_trigger;
void pg_CAseed_location_to_coordinates(pg_CAseed_locations location, int coordinates[2]);
#endif

// +++++++++++++++++++++++ FFT levels and frequency storage ++++++++++++++++++++
#if defined(pg_Projet_Criton)
extern float fftLevels[8];
extern float fftFrequencies[8];
extern float fftPhases[8];
#endif

// GUI DISPLAY & LOG FILE LOGGING
void pg_send_message_udp( char *pattern , char * message , char *targetHostid );
void pg_send_message_udp(char *pattern, char * message, pg_IPClient *targetHost);
void pg_process_key( int key );
void pg_process_special_key( int key );
void pg_keyStrokeScripts( int key );
void pg_Make_flashPhoto(void);
void pg_aliasScript( string address , string string_argument_0 ,
					 float float_arguments[PG_MAX_OSC_ARGUMENTS], int nb_arguments);

// interpolation on off
void NumberOfInteractionFingers(int nb_fingers);

void pg_snapshot( char * type );



int pg_FindSceneById(std::string* sceneID);

#endif
