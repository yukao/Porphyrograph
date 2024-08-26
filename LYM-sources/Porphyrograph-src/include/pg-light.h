/*! \file pg-light.h
 *
 *
 *     File pg-light.h
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

#ifndef PG_LIGHT_H
#define PG_LIGHT_H

#ifdef _WIN32
#include <time.h>
#include "Ftd2xx.h"
#include <stdint.h>
#else
#include <stdint.h>
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////
// CONSTs
//////////////////////////////////////////////////////////////////////////////////////////////////////

/******************** PRO MK2 LABELS: ASSIGN AS PER YOUR API (request the pdf if you don't have one) *********************/
// THE API Key is LSB First: so if it says 11223344 .. define it as ... 44,33,22,11

#define SET_PORT_ASSIGNMENT_LABEL 209
#define SEND_DMX_PORT2 150
#define RECEIVE_DMX_PORT2 194
#define SEND_MIDI_PORT 228
#define RECEIVE_MIDI_PORT 214

/***********************************************************************************/

// Enttec Pro definitions
#define GET_WIDGET_PARAMS		3
#define GET_WIDGET_PARAMS_REPLY 3
#define SET_WIDGET_PARAMS		4
#define RECEIVE_DMX_PORT1		5
#define SEND_DMX_PORT1			6
#define SEND_DMX_RDM_TX			7
#define RECEIVE_DMX_ON_CHANGE	8
#define RECEIVED_DMX_COS_TYPE	9
#define GET_WIDGET_SN			10
#define SET_API_KEY_LABEL				13
#define HARDWARE_VERSION_LABEL			14


#define ONE_BYTE	1
#define DMX_START_CODE 0x7E 
#define DMX_END_CODE 0xE7 
#define OFFSET 0xFF
#define DMX_HEADER_LENGTH 4
#define BYTE_LENGTH 8
#define HEADER_RDM_LABEL 5
#define NO_RESPONSE 0
#define DMX_PACKET_SIZE 512

#define RX_BUFFER_SIZE 40960
#define TX_BUFFER_SIZE 40960

#define MAX_PROS 20
#define SEND_NOW 0
#define TRUE 1
#define FALSE 0
#define HEAD 0
#define IO_ERROR 9

#define _loop_speed_factor 3.f

// Create an unordered_map of three strings (that map to strings)
// corresponds to all the parameters associated with DMX light channels
// key constants are from enum pg_light_command_hashMap_IDs
enum pg_light_command_hashMap_IDs {
	pg_enum_light_dimmer = 0,
	pg_enum_light_strobe,
	pg_enum_light_zoom,
	pg_enum_light_pan,
	pg_enum_light_tilt,
	pg_enum_light_hue,
	pg_enum_light_red,
	pg_enum_light_green,
	pg_enum_light_blue,
	pg_enum_light_grey,
	pg_enum_light_palette_color,
	pg_enum_nb_light_params,
};

////////////////////////////////
// LIGHTS
enum { pg_enum_rgb_red = 0, pg_enum_rgb_green, pg_enum_rgb_blue, pg_enum_rgb };

//////////////////////////////////////////////////////////////////////////////////////////////////////
// USB/DMX MANAGEMENT STRUCT
//////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma pack(1)
typedef struct {
	unsigned char FirmwareLSB;
	unsigned char FirmwareMSB;
	unsigned char BreakTime;
	unsigned char MaBTime;
	unsigned char RefreshRate;
}DMXUSBPROParamsType;

typedef struct {
	unsigned char UserSizeLSB;
	unsigned char UserSizeMSB;
	unsigned char BreakTime;
	unsigned char MaBTime;
	unsigned char RefreshRate;
}DMXUSBPROSetParamsType;
#pragma pack()

struct ReceivedDmxCosStruct
{
	unsigned char start_changed_byte_number;
	unsigned char changed_byte_array[5];
	unsigned char changed_byte_data[40];
};

//////////////////////////////////////////////////////////////////////////////////////////////////////
// LIGHT MANAGEMENT CLASS
//////////////////////////////////////////////////////////////////////////////////////////////////////

class Light {
public:
	string light_name;
	// group of lights with the same values
	int light_group;
	// rank of the light inside its group
	int index_in_group;
	// DMX address
	int light_port;
	int light_address;
	// number of channels
	int light_channels;
	// colors
	int light_red;
	int light_red_fine;
	int light_green;
	int light_green_fine;
	int light_blue;
	int light_blue_fine;
	int light_grey;
	int light_grey_fine;
	// dimmer
	int light_dimmer;
	int light_dimmer_fine;
	// strobe
	int light_strobe;
	int light_strobe_fine;
	// zoom
	int light_zoom;
	int light_zoom_fine;
	// orientation
	int light_pan;
	int light_pan_fine;
	int light_tilt;
	int light_tilt_fine;
	int light_hue;
	int light_hue_fine;
	Light() {
		light_name = "";
		light_group = -1;
		index_in_group = -1;
		light_port = -1;
		light_address = -1;
		light_channels = -1;
		light_red = 0;
		light_red_fine = 0;
		light_green = 0;
		light_green_fine = 0;
		light_blue = 0;
		light_blue_fine = 0;
		light_grey = 0;
		light_grey_fine = 0;
		light_dimmer = 0;
		light_dimmer_fine = 0;
		light_strobe = 0;
		light_strobe_fine = 0;
		light_zoom = 0;
		light_zoom_fine = 0;
		light_pan = 0;
		light_pan_fine = 0;
		light_tilt = 0;
		light_tilt_fine = 0;
		light_hue = 0;
		light_hue_fine = 0;
	}
	Light(string a_light_name, int a_light_group, int an_index_in_group, int a_light_port, int a_light_address, int a_light_channels,
		string a_light_red, string a_light_green, string a_light_blue, string a_light_grey, string a_light_dimmer, string a_light_strobe,
		string a_light_zoom, string a_light_pan, string a_light_tilt, string a_light_hue);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////
// LIGHT GROUP MANAGEMENT CLASS
////////////////////////////////////////////////////////////////////////////////////////////////////// 

class LightGroup {
private:
	int group_no;
	string group_id;
	float group_val[pg_enum_nb_light_params];
	bool group_onOff[pg_enum_nb_light_params];
	float group_val_master[pg_enum_nb_light_params];
	float group_val_pulse[pg_enum_nb_light_params];
	bool group_beatOnOff[pg_enum_nb_light_params];
	bool group_beatRandom[pg_enum_nb_light_params];
	struct a_loop {
		bool is_looped;
		int curve_type; // 0: sin 1: sawtooth 2: on/off
		int parallel_vs_alternate; // 0: parallel 1: alternate
		float loop_min;
		float loop_max;
		double loop_speed;
	} group_loop[pg_enum_nb_light_params];
	bool changed_since_last_DMX_update;
	bool group_hasBeatCommand;

public:
	LightGroup(int indgroup) {
		group_no = indgroup;
		group_id = "";
		for (int light_param = 0; light_param < pg_enum_nb_light_params; light_param++) {
			group_val[light_param] = 0.f;
			group_onOff[light_param] = true;
			group_val_master[light_param] = 1.f;
			group_val_pulse[light_param] = 0.f;
			group_beatOnOff[light_param] = false;
			group_beatRandom[light_param] = false;
			group_loop[light_param].is_looped = false;
			group_loop[light_param].curve_type = 0;
			group_loop[light_param].parallel_vs_alternate = 0;
			group_loop[light_param].loop_min = 0.f;
			group_loop[light_param].loop_max = 1.f;
			group_loop[light_param].loop_speed = 0.2f;
		}
		group_val[pg_enum_light_red] = RGBpalette_r_ini;
		group_val[pg_enum_light_green] = RGBpalette_g_ini;
		group_val[pg_enum_light_blue] = RGBpalette_b_ini;
		changed_since_last_DMX_update = false;
		group_hasBeatCommand = false;
	}
	void set_group_no(int indgroup) {
		group_no = indgroup;
	}
	void set_group_id(string id) {
		group_id = id;
	}
	string get_group_id(void) {
		return group_id;
	}
	int get_group_no(void) {
		return group_no;
	}
	void set_color(float rgb_color_val[pg_enum_rgb]) {
		if (!group_loop[pg_enum_light_palette_color].is_looped) {
			group_val[pg_enum_light_red] = rgb_color_val[pg_enum_rgb_red];
			group_val[pg_enum_light_green] = rgb_color_val[pg_enum_rgb_green];
			group_val[pg_enum_light_blue] = rgb_color_val[pg_enum_rgb_blue];
			//printf("rgb color %.2f %.2f -> rgb %.2f %.2f %.2f\n", group_val[pg_enum_light_palette_color], group_val[pg_enum_light_grey], group_val[pg_enum_light_red], group_val[pg_enum_light_green], group_val[pg_enum_light_blue]);
			changed_since_last_DMX_update = true;
		}
	}
	void set_group_val(int light_param, float value) {
		if (!group_loop[light_param].is_looped) {
			group_val[light_param] = value;
			//printf("Light param %d value %.2f\n", light_param, value);
			if (light_param == pg_enum_light_palette_color || light_param == pg_enum_light_grey) {
				float rgb_color[pg_enum_rgb];
				pg_compute_pulsed_palette_color(group_val[pg_enum_light_palette_color], group_val_pulse[pg_enum_light_palette_color], group_val[pg_enum_light_grey], group_val_pulse[pg_enum_light_grey], rgb_color, pg_enum_LIGHT_COLOR);
				group_val[pg_enum_light_red] = rgb_color[pg_enum_rgb_red];
				group_val[pg_enum_light_green] = rgb_color[pg_enum_rgb_green];
				group_val[pg_enum_light_blue] = rgb_color[pg_enum_rgb_blue];
				//printf("palette color / grey %.2f %.2f -> rgb %.2f %.2f %.2f\n", group_val[pg_enum_light_palette_color], group_val[pg_enum_light_grey], group_val[pg_enum_light_red], group_val[pg_enum_light_green], group_val[pg_enum_light_blue]);
			}
			changed_since_last_DMX_update = true;
		}
	}
	void set_group_val_pulse(int light_param, float value) {
		if (!group_loop[light_param].is_looped) {
			group_val_pulse[light_param] = value;
			if (light_param == pg_enum_light_palette_color || light_param == pg_enum_light_grey) {
				float rgb_color[pg_enum_rgb];
				pg_compute_pulsed_palette_color(group_val[pg_enum_light_palette_color], group_val_pulse[pg_enum_light_palette_color], group_val[pg_enum_light_grey], group_val_pulse[pg_enum_light_grey], rgb_color, pg_enum_LIGHT_COLOR);
				group_val[pg_enum_light_red] = rgb_color[pg_enum_rgb_red];
				group_val[pg_enum_light_green] = rgb_color[pg_enum_rgb_green];
				group_val[pg_enum_light_blue] = rgb_color[pg_enum_rgb_blue];
				//printf("pulsed palette color / grey %.2f %.2f -> rgb %.2f %.2f %.2f\n", group_val[pg_enum_light_palette_color], group_val[pg_enum_light_grey], group_val[pg_enum_light_red], group_val[pg_enum_light_green], group_val[pg_enum_light_blue]);
			}
			changed_since_last_DMX_update = true;
		}
	}
	void set_group_onOff(int light_param, bool value) {
		group_onOff[light_param] = value;
	}
	void set_group_beatOnOff(int light_param, bool value) {
		group_beatOnOff[light_param] = value;
		set_group_hasBeatCommand();
		changed_since_last_DMX_update = true;
	}
	void set_group_beatRandom(int light_param, bool value) {
		group_beatRandom[light_param] = value;
		set_group_hasBeatCommand();
		changed_since_last_DMX_update = true;
	}
	void set_group_hasBeatCommand(void) {
		group_hasBeatCommand = false;
		for (int light_param = 0; light_param < pg_enum_nb_light_params; light_param++) {
			if (group_beatOnOff[light_param] || group_beatRandom[light_param]) {
				group_hasBeatCommand = true;
				return;
			}
		}
	}
	bool get_group_beatOnOff(int light_param) {
		return group_beatOnOff[light_param];
	}
	bool get_group_beatRandom(int light_param) {
		return group_beatRandom[light_param];
	}
	bool get_group_hasBeatCommand(void) {
		return group_hasBeatCommand;
	}
	void update_beatCommand(void);
	void set_group_loop(int light_param, bool onOff, int curve_typ, int par_alt, float l_min, float l_max, double l_speed);
	float get_group_val(int light_param, int ind_light_in_group);
	bool get_group_onOff(int light_param) {
		return group_onOff[light_param];
	}
	float get_group_pulse(int light_param) {
		return group_val_pulse[light_param];
	}
	bool get_group_is_looped(void) {
		for (int light_param = 0; light_param < pg_enum_nb_light_params; light_param++) {
			if (group_loop[light_param].is_looped == true) {
				return true;
			}
		}
		return false;
	}
	bool get_group_loop_is_looped(int light_param) {
		return group_loop[light_param].is_looped;
	}
	int get_group_loop_curve_type(int light_param) {
		return group_loop[light_param].curve_type;
	}
	int get_group_loop_parallel(int light_param) {
		return group_loop[light_param].parallel_vs_alternate;
	}
	float get_group_loop_min(int light_param) {
		return group_loop[light_param].loop_min;
	}
	float get_group_loop_max(int light_param) {
		return group_loop[light_param].loop_max;
	}
	float get_group_master(int light_param) {
		return group_val_master[light_param];
	}
	double get_group_loop_speed(int light_param) {
		return group_loop[light_param].loop_speed * _loop_speed_factor;
	}
	// loop values updating (ignoring value messages from animation in Python)
	void update_group_loop(void);
	bool get_changed_since_last_DMX_update(void) {
		return changed_since_last_DMX_update;
	}
	void reset_changed_since_last_DMX_update(void) {
		changed_since_last_DMX_update = false;
	}
	void set_changed_since_last_DMX_update(void) {
		changed_since_last_DMX_update = true;
	}
	bool get_one_non_null_pulse(void) {
		return (group_val_pulse[pg_enum_light_dimmer] != 0.f || group_val_pulse[pg_enum_light_strobe] != 0.f || group_val_pulse[pg_enum_light_zoom] != 0.f || group_val_pulse[pg_enum_light_pan] != 0.f || group_val_pulse[pg_enum_light_tilt] != 0.f
			|| group_val_pulse[pg_enum_light_hue] != 0.f || group_val_pulse[pg_enum_light_grey] != 0.f || group_val_pulse[pg_enum_light_palette_color] != 0.f);
	}
	~LightGroup() {
	}
};

//////////////////////////////////////////////////////////////////////////////////////////////////////
// EXPORTED VARIABLES
////////////////////////////////////////////////////////////////////////////////////////////////////// 

// lights presets
extern int pg_nb_light_groups[PG_MAX_SCENARIOS];
extern vector<LightGroup> pg_light_groups[PG_MAX_SCENARIOS];

//////////////////////////////////////////////////////////////////////////////////////////////////////
// EXPORTED FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////

void pg_DMX_light_initialization(void);
void pg_lightUpdate(void);
void pg_osc_light_command(string address_string, float float_arguments[PG_MAX_OSC_ARGUMENTS], int nb_arguments);
void pg_UpdateLightGroups_from_LightVars(void);
void pg_parseScenario_Lights(std::ifstream& scenarioFin, int indScenario);
void pg_lightGUI_initialization(void);

#endif