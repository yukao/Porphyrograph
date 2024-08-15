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

/********************** PLEASE SET THESE FIRST **********************************/

/******************** PRO MK2 LABELS: ASSIGN AS PER YOUR API (request the pdf if you don't have one) *********************/
// THE API Key is LSB First: so if it says 11223344 .. define it as ... 44,33,22,11

//  #define SET_PORT_ASSIGNMENT_LABEL	    0
//  #define SEND_DMX_PORT2					0
//  #define RECEIVE_DMX_PORT2				0
//  #define SEND_MIDI_PORT					0
//  #define RECEIVE_MIDI_PORT				0

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

#define MAX_PROS 20
#define SEND_NOW 0
#define TRUE 1
#define FALSE 0
#define HEAD 0
#define IO_ERROR 9

////////////////////////////////
// LIGHTS
// lights presets
extern int pg_nb_light_groups[PG_MAX_SCENARIOS];
// interface current light group
extern int pg_interface_light_group;

enum { pg_enum_rgb_red = 0, pg_enum_rgb_green, pg_enum_rgb_blue, pg_enum_rgb };
void pg_light_channel_string_to_channel_no(string a_light_channel_string, int* light_channel, int* light_channel_fine, int nb_channels);
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
		string a_light_zoom, string a_light_pan, string a_light_tilt, string a_light_hue) {
		light_name = a_light_name;
		light_group = a_light_group;
		index_in_group = an_index_in_group;
		light_port = a_light_port;
		light_address = a_light_address;
		light_channels = a_light_channels;
		if (a_light_red == "" || a_light_green == "" || a_light_blue == "" || a_light_grey == "" || a_light_dimmer == ""
			|| a_light_strobe == "" || a_light_zoom == "" || a_light_pan == "" || a_light_tilt == "" || a_light_hue == "") {
			sprintf(pg_errorStr, "Error: incomplete light channels r g b g dimm strobe zoom pan tilt hue (%s) (%s) (%s) (%s) (%s) (%s) (%s) (%s) (%s) (%s)", a_light_red.c_str(), a_light_green.c_str(), a_light_blue.c_str(), a_light_grey.c_str(), a_light_dimmer.c_str(), a_light_strobe.c_str(), a_light_zoom.c_str(), a_light_pan.c_str(), a_light_tilt.c_str(), a_light_hue.c_str()); pg_ReportError(pg_errorStr); throw 336;
		}
		pg_light_channel_string_to_channel_no(a_light_red, &light_red, &light_red_fine, light_channels);
		pg_light_channel_string_to_channel_no(a_light_green, &light_green, &light_green_fine, light_channels);
		pg_light_channel_string_to_channel_no(a_light_blue, &light_blue, &light_blue_fine, light_channels);
		pg_light_channel_string_to_channel_no(a_light_grey, &light_grey, &light_grey_fine, light_channels);
		pg_light_channel_string_to_channel_no(a_light_dimmer, &light_dimmer, &light_dimmer_fine, light_channels);
		pg_light_channel_string_to_channel_no(a_light_strobe, &light_strobe, &light_strobe_fine, light_channels);
		pg_light_channel_string_to_channel_no(a_light_zoom, &light_zoom, &light_zoom_fine, light_channels);
		pg_light_channel_string_to_channel_no(a_light_pan, &light_pan, &light_pan_fine, light_channels);
		pg_light_channel_string_to_channel_no(a_light_tilt, &light_tilt, &light_tilt_fine, light_channels);
		pg_light_channel_string_to_channel_no(a_light_hue, &light_hue, &light_hue_fine, light_channels);
	}
};
// Create an unordered_map of three strings (that map to strings)
// corresponds to all the parameters associated with DMX light channels
// key constants are from enum pg_light_command_hashMap_IDs
enum pg_light_command_hashMap_IDs {	pg_enum_light_dimmer = 0,
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
extern std::unordered_map<int, std::string> pg_light_param_hashMap;
// same map but from strings to enum values
extern std::unordered_map<std::string, int> pg_inverse_light_param_hashMap;
// submap of the parameters which can be looped
extern std::unordered_map<int, std::string> pg_light_loop_param_hashMap;
// lights
extern vector<Light> pg_Lights[PG_MAX_SCENARIOS];
#define _loop_speed_factor 3.f
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
	void update_beatCommand(void) {
		for (const auto& myPair : pg_light_param_hashMap) {
			int light_param = myPair.first;
			string light_param_string = myPair.second;
			if (group_beatOnOff[light_param]) {
				group_val_master[light_param] = 1.f - group_val_master[light_param];
				changed_since_last_DMX_update = true;
			}
			if (group_beatRandom[light_param]) {
				group_val[light_param] = rand_0_1;
				changed_since_last_DMX_update = true;
				sprintf(pg_AuxString, "/light/%d/%s %.4f", group_no + 1, light_param_string.c_str(), group_val[light_param]); pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_TouchOSC_send");
			}
		}
	}
	void set_group_loop(int light_param, bool onOff, int curve_typ, int par_alt, float l_min, float l_max, double l_speed) {
		group_loop[light_param].is_looped = onOff;
		group_loop[light_param].curve_type = curve_typ;
		group_loop[light_param].parallel_vs_alternate = par_alt;
		group_loop[light_param].loop_min = l_min;
		group_loop[light_param].loop_max = l_max;
		group_loop[light_param].loop_speed = l_speed / _loop_speed_factor;
		//printf("group loop light param %d looped %d curve %d // %d min/max/speed %.2f %.2f %.2f\n",
		//	light_param,
		//	group_loop[light_param].is_looped,
		//	group_loop[light_param].curve_type,
		//	group_loop[light_param].parallel_vs_alternate,
		//	group_loop[light_param].loop_min,
		//	group_loop[light_param].loop_max,
		//	group_loop[light_param].loop_speed);
	}
	float get_group_val(int light_param, int ind_light_in_group) {
		float group_value = group_val[light_param];
		if (group_loop[light_param].is_looped == true && group_onOff[light_param] == true && group_loop[light_param].parallel_vs_alternate == 0
			&& ind_light_in_group % 2 == 1) {
			group_value = 1.f - group_value;
		}
		if (group_val_pulse[light_param] != 0.f) {
			float val = group_value * (1.f + pulse_average * group_val_pulse[light_param]) * group_val_master[light_param];
			sprintf(pg_AuxString, "/light/%d/%s %.4f", group_no + 1, pg_light_param_hashMap[light_param].c_str(), val); pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_TouchOSC_send");
			return val;
		}
		else {
			return group_value * group_val_master[light_param];
		}
	}
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
	void update_group_loop(void) {
		for (const auto& myPair : pg_light_param_hashMap) {
			int light_param = myPair.first;
			string light_param_string = myPair.second;
			if (group_loop[light_param].is_looped == true && group_onOff[light_param] == true) {
				//printf("update loop param %d curve_type %d min %.2f max %.2f speed %.2f\n", light_param, group_loop[light_param].curve_type, group_loop[light_param].loop_min, group_loop[light_param].loop_max, group_loop[light_param].loop_speed);
				// sin curve
				if (group_loop[light_param].curve_type == 0) {
					double phase = 0.f;
					double absc = pg_CurrentClockTime * group_loop[light_param].loop_speed * 2 * M_PI + phase;
					group_val[light_param] = group_loop[light_param].loop_min
						+ float(sin(absc) * 0.5 + 0.5) * (group_loop[light_param].loop_max - group_loop[light_param].loop_min);
					changed_since_last_DMX_update = true;
					//printf("sin curve %.5f\n", group_val[light_param]);
				}
				// sawtooth curve
				else if (group_loop[light_param].curve_type == 1) {
					double absc = pg_CurrentClockTime * group_loop[light_param].loop_speed;
					if (group_loop[light_param].parallel_vs_alternate == 1) {
						group_val[light_param] = group_loop[light_param].loop_min
							+ float(absc - floor(absc)) * (group_loop[light_param].loop_max - group_loop[light_param].loop_min);
					}
					else {
						group_val[light_param] = group_loop[light_param].loop_min
							+ float(1. - (absc - floor(absc))) * (group_loop[light_param].loop_max - group_loop[light_param].loop_min);
					}
					changed_since_last_DMX_update = true;
					//printf("sawtooth curve %.5f\n", group_val[light_param]);
				}
				// on/off stepwise curve
				else if (group_loop[light_param].curve_type == 2) {
					double phase = 0.f;
					double absc = pg_CurrentClockTime * group_loop[light_param].loop_speed * 2 * M_PI + phase;
					group_val[light_param] = (sin(absc) > 0 ? group_loop[light_param].loop_max : group_loop[light_param].loop_min);
					changed_since_last_DMX_update = true;
					//printf("stepwise curve %.5f\n", group_val[light_param]);
				}
				sprintf(pg_AuxString, "/light/%d/%s %.4f", group_no + 1, light_param_string.c_str(), group_val[light_param]); pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_TouchOSC_send");
			}
		}
	}
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
extern vector<LightGroup> pg_light_groups[PG_MAX_SCENARIOS];
// Create an unordered_map of three strings (that map to strings)
extern std::unordered_map<std::string, int> pg_inverse_light_param_hashMap;
extern std::unordered_map<int, std::string> pg_light_param_hashMap;




int FTDI_SendData(int label, unsigned char* data, unsigned int length);
int FTDI_ReceiveData(int label, unsigned char* data, unsigned int expected_length);
uint16_t FTDI_OpenDevice(int device_num);
int FTDI_ListDevices(void);
void FTDI_ClosePort();
void FTDI_PurgeBuffer();
void pg_DMX_light_initialization(void);
void pg_init_promk2();
bool pg_oneLightGroup_Changed(void);
void pg_Reset_LightGroup_Changed(void);
void pg_Reset_allLightGroups_Changed(void);
bool pg_oneLightGroup_Loops(void);
bool pg_oneLightGroup_Loops(void);
void pg_light_automation_update(void);
void pg_store_one_DMXvalue(unsigned char* myDmx, int DMX_light_address, int channel_no, int fine_channel_no, float value);
void pg_StoreDMXValues_AllLightGroups(void);
void pg_SendDMX(void);
void pg_SendDMXZeros(void);
void pg_StoreDMX(int light_rank, float dimmer_value, float strobe_value, float zoom_value, float pan_value, float tilt_value, float hue_value, float red_value, float green_value, float blue_value, float grey_value);
void pg_StoreDMX(int channel, float channel_value, int light_port, bool has_fine_channel);
void pg_lightGUI_values_and_pulse_update(int light_param, int interface_light_group, string light_param_string);
void pg_lightGUI_loop_update(int light_param, string light_param_string);
void pg_lightGUI_all_values_and_pulse_update(void);
void pg_lightGUI_all_loop_update(void);
void pg_lightGUI_initialization(void);
void pg_osc_light_command(string address_string, float float_arguments[PG_MAX_OSC_ARGUMENTS], int nb_arguments);
void pg_light_channel_string_to_channel_no(string a_light_channel_string, int* light_channel, int* light_channel_fine, int nb_channels);
void pg_UpdateLightGroups_from_LightVars(void);

#endif