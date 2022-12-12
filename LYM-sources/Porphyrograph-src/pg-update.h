/*! \file pg-update.h
 * 
 * 
 *     File pg-update.h
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

#ifndef PG_UPDATE_H
#define PG_UPDATE_H

/////////////////////////////////////////////////////////////////
// textures bitmaps and associated IDs
extern GLuint pg_screenMessageBitmap_texID; // nb_attachments=1
extern GLubyte *pg_screenMessageBitmap;

extern GLuint pg_paths_Pos_Texture_texID[PG_NB_PATHS + 1];
extern GLfloat *pg_paths_Pos_Texture[PG_NB_PATHS + 1];
extern GLuint pg_paths_Col_Texture_texID[PG_NB_PATHS + 1];
extern GLfloat *pg_paths_Col_Texture[PG_NB_PATHS + 1];
extern GLuint pg_paths_RadBrushRendmode_Texture_texID[PG_NB_PATHS + 1];
extern GLfloat *pg_paths_RadBrushRendmode_Texture[PG_NB_PATHS + 1];

extern GLuint Screen_Font_texture_Rectangle_texID;
#if defined (TVW)
extern GLuint Display_Font_texture_Rectangle_texID;
#endif

#ifdef PG_WITH_MASTER_MASK
extern GLuint Master_Mask_texID;
extern GLuint Master_Multilayer_Mask_texID;
#endif

#ifdef PG_WITH_BURST_MASK
extern GLuint Burst_Mask_texID;
#endif

#if !defined (PG_BEZIER_PATHS) || defined(PIERRES) || defined(ENSO) || defined(SONG) || defined(FORET) || defined (SOUNDINITIATIVE) || defined(ALKEMI) || defined(ATELIERSENFANTS) || defined(CORE)
extern GLuint Pen_texture_3D_texID;
#endif
extern GLuint Noise_texture_3D;
#ifdef PG_WITH_REPOP_DENSITY
extern std::vector<GLuint>  pg_RepopDensity_texture_texID;
#endif

#if defined (TEXTURED_QUAD_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 
extern std::vector<GLuint> pg_particle_initial_pos_speed_texID;
extern std::vector<GLuint> pg_particle_initial_color_radius_texID;
extern std::vector<GLuint> pg_particle_acc_texID;
#endif

#ifdef PG_SENSORS
extern GLuint Sensor_texture_rectangle;
#endif

#if defined (TVW)
#define   PG_NB_DISPLAY_MESSAGES 3500

////////////////////////////////////////////////////////////////
// textures bitmaps and associated IDs
extern GLuint pg_Display_Message1_Bitmap_texID;
extern GLubyte *pg_displayMsg1Bitmap;
extern GLuint pg_Display_Message2_Bitmap_texID;
extern GLubyte *pg_displayMsg2Bitmap;

////////////////////////////////////////
// large messages displayed on the bottom of the screen
extern string DisplayText1;
extern float DisplayText1Alpha;
extern float LengthDisplayText1;
extern int IndDisplayText1;

extern string DisplayText2;
extern float DisplayText2Alpha;
extern float LengthDisplayText2;
extern int IndDisplayText2;

extern bool DisplayText1Front;
extern float DisplayTextSwapInitialTime;
#endif
#if defined (TVW) || defined (ETOILES)
extern std::string* DisplayTextList;
extern int* DisplayTextFirstInChapter;
extern int NbDisplayTexts;
extern int pg_Ind_Current_DisplayText;

// message chapters
extern int NbTextChapters;
#endif

#ifdef PG_MESHES
// MVP matrices
extern glm::mat4 VP1perspMatrix;
extern glm::mat4 VP1viewMatrix;
extern glm::mat4 VP1homographyMatrix;
extern glm::mat4 MeshPosModelMatrix;
#endif

////////////////////////////////////////
// geometry: track lines
bool pg_initFBO( void );
void pg_initRenderingMatrices( void );
void pg_initGeometry_quads( void );
bool pg_initTextures( void );

/////////////////////////////////////////////////////////////////
// CAMERA PARAMETERS
extern float CameraCurrent_exposure;
extern float CameraCurrent_gain;
extern float CameraCurrent_brightness;
extern float CameraCurrent_saturation;
extern float CameraCurrent_contrast;
extern float CameraCurrent_FPS;
extern float CameraCurrent_focus;
extern float CameraCurrent_gamma;
extern float CameraCurrent_WB_B;
extern float CameraCurrent_WB_R;

// video texture ID, image and camera
extern GLuint pg_camera_texture_texID;
extern GLuint pg_movie_texture_texID;
extern GLuint pg_camera_BG_texture_texID;
#ifdef GN
extern GLuint pg_camera_BGIni_texture_texID;
#endif
// extern IplImage *pg_camera_frame;
// extern IplImage *pg_movie_frame;
extern int pg_camera_frame_width;
extern int pg_camera_frame_height;
extern int pg_camera_x_offset;
extern int pg_camera_y_offset;
extern int pg_movie_frame_width;
extern int pg_movie_frame_height;
extern Mat pg_movie_frame;

// video tracks
extern vector<string> movieFileName;
extern vector<string> movieShortName;
extern vector<string> movieSoundtrackPeaksFileName;
extern vector<string> movieSoundtrackOnsetsFileName;
extern vector<vector<float>> movieSoundtrackPeaks;
extern vector<vector<float>> movieSoundtrackOnsets;
extern int nb_movies;
// soundtracks
extern vector<string> trackFileName;
extern vector<string> trackShortName;
extern int currentlyPlaying_trackNo;
extern int nb_soundtracks;
// pen palettes presets
extern int nb_pen_colorPresets;
extern vector<string> pen_colorPresets_names;
extern float *pen_colorPreset_values;
// lights presets
extern int pg_nb_lights;
extern int pg_nb_light_groups;
// interface current light group
extern int pg_interface_light_group;
/// Error string
extern char* ErrorStr;
// current time
extern double CurrentClockTime;
// error report
void ReportError(char* errorString);
enum { _rgb_red = 0, _rgb_green, _rgb_blue, _rgb };
void light_channel_string_to_channel_no(string a_light_channel_string, int* light_channel, int* light_channel_fine, int nb_channels);
class Light {
public:
	string light_name;
	int light_group;
	int index_in_group;
	int light_port;
	int light_address;
	int light_channels;
	int light_red;
	int light_red_fine;
	int light_green;
	int light_green_fine;
	int light_blue;
	int light_blue_fine;
	int light_grey;
	int light_grey_fine;
	int light_dimmer;
	int light_dimmer_fine;
	int light_strobe;
	int light_strobe_fine;
	int light_zoom;
	int light_zoom_fine;
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
	void set_light_values(string a_light_name, int a_light_group, int an_index_in_group, int a_light_port, int a_light_address, int a_light_channels,
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
			sprintf(ErrorStr, "Error: incomplete light channels r g b g dimm strobe zoom pan tilt hue (%s) (%s) (%s) (%s) (%s) (%s) (%s) (%s) (%s) (%s)", a_light_red.c_str(), a_light_green.c_str(), a_light_blue.c_str(), a_light_grey.c_str(), a_light_dimmer.c_str(), a_light_strobe.c_str(), a_light_zoom.c_str(), a_light_pan.c_str(), a_light_tilt.c_str(), a_light_hue.c_str()); ReportError(ErrorStr); throw 336;
		}
		light_channel_string_to_channel_no(a_light_red, &light_red, &light_red_fine, light_channels);
		light_channel_string_to_channel_no(a_light_green, &light_green, &light_green_fine, light_channels);
		light_channel_string_to_channel_no(a_light_blue, &light_blue, &light_blue_fine, light_channels);
		light_channel_string_to_channel_no(a_light_grey, &light_grey, &light_grey_fine, light_channels);
		light_channel_string_to_channel_no(a_light_dimmer, &light_dimmer, &light_dimmer_fine, light_channels);
		light_channel_string_to_channel_no(a_light_strobe, &light_strobe, &light_strobe_fine, light_channels);
		light_channel_string_to_channel_no(a_light_zoom, &light_zoom, &light_zoom_fine, light_channels);
		light_channel_string_to_channel_no(a_light_pan, &light_pan, &light_pan_fine, light_channels);
		light_channel_string_to_channel_no(a_light_tilt, &light_tilt, &light_tilt_fine, light_channels);
		light_channel_string_to_channel_no(a_light_hue, &light_hue, &light_hue_fine, light_channels);
	}
};
// Create an unordered_map of three strings (that map to strings)
// corresponds to all the parameters associated with DMX light channels
// key constants are from enum pg_light_command_hashMap_IDs
enum pg_light_command_hashMap_IDs
{
	_dimmer = 0,
	_strobe,
	_zoom,
	_pan,
	_tilt,
	_hue,
	_red,
	_green,
	_blue,
	_grey,
	_palette_color,
	_nb_light_params,
};
extern std::unordered_map<int, std::string> pg_light_param_hashMap;
// same map but from strings to enum values
extern std::unordered_map<std::string, int> pg_inverse_light_param_hashMap;
// submap of the parameters which can be looped
extern std::unordered_map<int, std::string> pg_light_loop_param_hashMap;
extern Light* pg_lights;
#define _loop_speed_factor 3.f
class LightGroup {
private:
	int group_no;
	string group_id;
	float group_val[_nb_light_params];
	bool group_onOff[_nb_light_params];
	float group_val_master[_nb_light_params];
	float group_val_pulse[_nb_light_params];
	bool group_beatOnOff[_nb_light_params];
	bool group_beatRandom[_nb_light_params];
	struct a_loop {
		bool is_looped;
		int curve_type; // 0: sin 1: sawtooth 2: on/off
		int parallel_vs_alternate; // 0: parallel 1: alternate
		float loop_min;
		float loop_max;
		double loop_speed;
	} group_loop[_nb_light_params];
	bool changed_since_last_DMX_update;
	bool group_hasBeatCommand;

public:
	LightGroup() {
		group_no = 0;
		group_id = "";
		for (int light_param = 0; light_param < _nb_light_params; light_param++) {
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
		group_val[_red] = RGBpalette_r_ini;
		group_val[_green] = RGBpalette_g_ini;
		group_val[_blue] = RGBpalette_b_ini;
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
	void set_color(float rgb_color_val[_rgb]) {
		if (!group_loop[_palette_color].is_looped) {
			group_val[_red] = rgb_color_val[_rgb_red];
			group_val[_green] = rgb_color_val[_rgb_green];
			group_val[_blue] = rgb_color_val[_rgb_blue];
			printf("rgb color %.2f %.2f -> rgb %.2f %.2f %.2f\n", group_val[_palette_color], group_val[_grey], group_val[_red], group_val[_green], group_val[_blue]);
			changed_since_last_DMX_update = true;
		}
	}
	void set_group_val(int light_param, float value) {
		if (!group_loop[light_param].is_looped) {
			group_val[light_param] = value;
			if (light_param == _palette_color || light_param == _grey) {
				float rgb_color[_rgb];
				compute_pulsed_palette_color(group_val[_palette_color], group_val_pulse[_palette_color], group_val[_grey], group_val_pulse[_grey], rgb_color, false);
				group_val[_red] = rgb_color[_rgb_red];
				group_val[_green] = rgb_color[_rgb_green];
				group_val[_blue] = rgb_color[_rgb_blue];
				printf("palette color / grey %.2f %.2f -> rgb %.2f %.2f %.2f\n", group_val[_palette_color], group_val[_grey], group_val[_red], group_val[_green], group_val[_blue]);
			}
			changed_since_last_DMX_update = true;
		}
	}
	void set_group_val_pulse(int light_param, float value) {
		if (!group_loop[light_param].is_looped) {
			group_val_pulse[light_param] = value;
			if (light_param == _palette_color || light_param == _grey) {
				float rgb_color[_rgb];
				compute_pulsed_palette_color(group_val[_palette_color], group_val_pulse[_palette_color], group_val[_grey], group_val_pulse[_grey], rgb_color, false);
				group_val[_red] = rgb_color[_rgb_red];
				group_val[_green] = rgb_color[_rgb_green];
				group_val[_blue] = rgb_color[_rgb_blue];
				printf("pulsed palette color / grey %.2f %.2f -> rgb %.2f %.2f %.2f\n", group_val[_palette_color], group_val[_grey], group_val[_red], group_val[_green], group_val[_blue]);
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
		for (int light_param = 0; light_param < _nb_light_params; light_param++) {
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
				sprintf(AuxString, "/light/%d/%s %.4f", group_no, light_param_string.c_str(), group_val[light_param]); pg_send_message_udp((char*)"f", AuxString, (char*)"udp_TouchOSC_send");
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
			sprintf(AuxString, "/light/%d/%s %.4f", group_no, pg_light_param_hashMap[light_param].c_str(), val); pg_send_message_udp((char*)"f", AuxString, (char*)"udp_TouchOSC_send");
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
		for (int light_param = 0; light_param < _nb_light_params; light_param++) {
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
					double absc = CurrentClockTime * group_loop[light_param].loop_speed * 2 * M_PI + phase;
					group_val[light_param] = group_loop[light_param].loop_min
						+ float(sin(absc) * 0.5 + 0.5) * (group_loop[light_param].loop_max - group_loop[light_param].loop_min);
					changed_since_last_DMX_update = true;
					//printf("sin curve %.5f\n", group_val[light_param]);
				}
				// sawtooth curve
				else if (group_loop[light_param].curve_type == 1) {
					double absc = CurrentClockTime * group_loop[light_param].loop_speed;
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
					double absc = CurrentClockTime * group_loop[light_param].loop_speed * 2 * M_PI + phase;
					group_val[light_param] = (sin(absc) > 0 ? group_loop[light_param].loop_max : group_loop[light_param].loop_min);
					changed_since_last_DMX_update = true;
					//printf("stepwise curve %.5f\n", group_val[light_param]);
				}
				sprintf(AuxString, "/light/%d/%s %.4f", group_no, light_param_string.c_str(), group_val[light_param]); pg_send_message_udp((char*)"f", AuxString, (char*)"udp_TouchOSC_send");
			}
		}
	}
	bool get_changed_since_last_DMX_update(void) {
		return changed_since_last_DMX_update;
	}
	void reset_changed_since_last_DMX_update(void) {
		changed_since_last_DMX_update = false;
	}
	bool get_one_non_null_pulse(void) {
		return (group_val_pulse[_dimmer] != 0.f || group_val_pulse[_strobe] != 0.f || group_val_pulse[_zoom] != 0.f || group_val_pulse[_pan] != 0.f || group_val_pulse[_tilt] != 0.f 
			|| group_val_pulse[_hue] != 0.f || group_val_pulse[_grey] != 0.f || group_val_pulse[_palette_color] != 0.f);
	}
	~LightGroup() {
	}
};
extern LightGroup* pg_light_groups;
// Create an unordered_map of three strings (that map to strings)
extern std::unordered_map<std::string, int> pg_inverse_light_param_hashMap;
extern std::unordered_map<int, std::string> pg_light_param_hashMap;
// pen palettes colors
extern int nb_pen_palette_colors;
extern vector<string> pen_palette_colors_names;
extern float **pen_palette_colors_values;
// photo albums
extern string photoAlbumDirName;
extern int nb_photo_albums;
// short video clip albums
extern string clipAlbumDirName;
extern int nb_clip_albums;
extern int clip_image_width;
extern int clip_image_height;
extern int clip_crop_width;
extern int clip_crop_height;
// pen brushes
extern string pen_brushes_fileName;
extern int nb_pen_brushes;
// textures with multiple layers
extern int nb_layers_master_mask;

extern bool is_capture_diaporama;
#ifdef PG_WITH_CAMERA_CAPTURE
extern VideoCapture pg_webCam_capture;
extern VideoCapture* pg_IPCam_capture;
extern String* pg_IPCam_capture_address;
extern int nb_IPCam;
extern int nb_webCam;
class webCam {
public:
	string cameraString;
	int cameraID;
	int cameraWidth;
	int cameraHeight;
	webCam(void) {
		cameraString = "";
		cameraID = -1;
		cameraWidth = 0;
		cameraHeight = 0;
	}
	~webCam(void) {
		cameraString = "";
		cameraID = -1;
		cameraWidth = 0;
		cameraHeight = 0;
	}
};
extern webCam* pg_webCams;
extern int pg_current_active_cameraNo;
extern bool initializedWebcam;
#endif
extern VideoCapture  pg_movie_capture;
class movie_status {
private:
	int nbFramesLeft;
	double initialTime;
	int currentSoundPeakIndex;
	int initialMovieNbFrames;
	int nbSoundPeakIndex;
	int currentSoundOnsetIndex;
	int nbSoundOnsetIndex;

public:
	movie_status();
	// number frames left until the end from current frame
	int get_nbFramesLeft();
	// number of frames read from the beginning of the movie
	int get_nbFramesRead();
	// current time when movie started
	double get_initialTime();
	// initial total number of frames in the movie
	int get_initialNbFrames();
	// checks whether a peak or an onset are passed or closer than one frame
	void updatePeakOrOnset(void);
	// sets the movie at a position from beginning
	void set_position(int nb_frames_from_beginning);
	// reads one frame and updates the remaining number of frames in the movie
	void read_one_frame();
	// (re)starts the movie from begginning with its total number of frames
	void reset_movie(int nbTotFramesLeft);
	~movie_status();
};
extern movie_status pg_movie_status;
extern bool is_movieLooping;
extern bool is_movieLoading;
extern int initialSecondBGCapture;

#ifdef PG_WITH_CLIPS
#define _NbMaxCues 8
enum pg_clip_LR
{
	_clipLeft = 0,
	_clipRight,
	_clipLR,
};

class clip_track {
private:
	int clip_cue[_NbMaxCues];
	int clipNo;
	int initialNbFrames;
	string clip_name;
public:
	clip_track(int indClip, int nbFrames, string name);
	// catches a cue
	int set_cue_onOff(int indCue, int frameNo);
	// resets a cue
	void set_cue_off(int indCue);
	// sets a cue
	void set_cue(int indCue, int indFrame);
	// returns a cue
	int get_cue(int indCue);
	// initial total number of frames in the clip
	int get_initialNbFrames();
	// clip name
	string get_name();
};

// clip track
extern std::vector<clip_track> pg_clip_tracks;

class clip_status {
private:
	pg_clip_LR sideLR;
	int currentlyPlaying_clipNo[PG_NB_PARALLEL_CLIPS];
	int lastFrame[PG_NB_PARALLEL_CLIPS];
	double lastPlayedFrameTime[PG_NB_PARALLEL_CLIPS];
	double cumulated_scratch_offset;
	double currentFPS;
	float last_scratch_angle;
	float last_nudge_angle;
	bool is_scratch_pressed;
	double last_scratch_angle_change;
	double last_nudge_angle_change;

public:
	bool clip_autoplay[PG_NB_PARALLEL_CLIPS];
	bool clip_play[PG_NB_PARALLEL_CLIPS];
	float clip_level;
	float clip_r;
	float clip_g;
	float clip_b;
	int jump_frame_freq;

	clip_status(int clipSide);
	// last frame index
	int get_lastFrame(int indClipRank);
	// last frame exact time
	double get_lastPlayedFrameTime(int indClipRank);
	// last frame exact time
	void set_lastPlayedFrameTime(int indClipRank, double a_time);
	// next frame index and sets it as last frame together 
	// with memory of exact time of current frame (including the delay to the current time)
	int get_nextFrameAndUpdate(int indClipRank);
	// standard speed (at which it was recorded)
	double get_standardSpeed();
	// current speed according to nudging
	double get_currentFPS();
	// current playing clip No
	int getCurrentlyPlaying_clipNo(int indClipRank);
	// play
	bool get_clip_play(int indClipRank);
	// autoplay
	bool get_clip_autoplay(int indClipRank);
	// position of the wheel in last interaction
	float get_last_scratch_angle(double *a_time);
	// position of the wheel in last interaction
	float get_last_nudge_angle(double *a_time);
	// scratch wheel is pressed: do not update clip automatically
	bool get_scratch_pressed();
	// standard speed (at which it was recorded)
	void set_standardSpeed(double a_speed);
	// current speed according to nudging
	void set_currentFPS(double a_FPS);
	// current playing clip No
	void setCurrentlyPlaying_clipNo(int indClipRank, int indClip);
	// position of the wheel in last interaction
	void set_last_scratch_angle(float a_angle, double a_time);
	// position of the wheel in last interaction
	void set_last_nudge_angle(float a_angle, double a_time);
	// scratch wheel is pressed: do not update clip automatically
	void set_scratch_pressed(bool a_value);
	// sets the clip at a position from beginning
	void set_position(int indClipRank, int nb_frames_from_beginning);
	// offsets the movie position by a positive or negative time delta (scratch mode)
	// used several times for a small offset
	void scratch_offset_position(int indClipRank, double elapsedTime);
	// offsets the movie position by a positive or negative time delta (play mode)
	// used once for a big jump
	void play_offset_position(int indClipRank, double elapsedTime);
	// changes the initial time for fast forward/backward
	void reset_clip(int indClipRank);
	~clip_status();
};

// playing clip nos
extern int playing_clipNoLeft;
extern int playing_secondClipNoLeft;
extern int playing_clipNoRight;
extern int playing_secondClipNoRight;
// clip status
extern clip_status pg_clip_status[_clipLR];
// index of top clip 
extern int rankOfTopClip;
#endif

#ifdef GN
extern bool secondInitialBGCapture;
extern bool initialBGCapture;
#endif

/*! \name Group texture_ mode
 */
// \{
/// _Render : rendering,
/// _Jpg : framebuffer output to jpg file
/// _Png : framebuffer output to png file
/// _Svg : framebuffer output to svg file
enum DrawingMode{ _Render=0 , _Svg , _Png , _Jpg , _Video };

extern GLuint drawBuffers[16];

/////////////////////////////////////////////////////////////////
// config variables

// stroke shipping to GPU (Update and PartcleAnimation shaders)
#define PG_PATH_P_X              0
#define PG_PATH_P_Y              1
#ifdef PG_BEZIER_PATHS
#define PG_PATH_BOX              2
#define PG_PATH_COLOR            3
#define PG_PATH_RADIUS_BEGINEND  4
#define PG_MAX_PATH_DATA         5
#else
#define PG_PATH_COLOR            2
#define PG_PATH_RADIUS_BEGINEND  3
#define PG_MAX_PATH_DATA         4
#endif
extern float path_data_Update[(PG_NB_PATHS + 1) * PG_MAX_PATH_DATA * 4];

#define PG_PATH_ANIM_POS              0
#define PG_PATH_ANIM_RAD              1
#define PG_MAX_PATH_ANIM_DATA         2
extern float path_data_ParticleAnimation[(PG_NB_PATHS + 1) * PG_MAX_PATH_ANIM_DATA * 4];

// current mouse location (also used for displaying the cursor)
extern float paths_time[PG_NB_PATHS + 1];
extern float paths_x[PG_NB_PATHS + 1];
extern float paths_y[PG_NB_PATHS + 1];
extern float paths_x_memory[PG_NB_PATHS + 1];
extern float paths_y_memory[PG_NB_PATHS + 1];
extern float paths_x_next[PG_NB_PATHS + 1];
extern float paths_y_next[PG_NB_PATHS + 1];
extern float paths_time_prev_prev[PG_NB_PATHS + 1];
extern float paths_x_prev_prev[PG_NB_PATHS + 1];
extern float paths_y_prev_prev[PG_NB_PATHS + 1];
extern float paths_time_prev[PG_NB_PATHS + 1];
extern float paths_x_prev[PG_NB_PATHS + 1];
extern float paths_y_prev[PG_NB_PATHS + 1];
extern float paths_x_prev_memory[PG_NB_PATHS + 1];
extern float paths_y_prev_memory[PG_NB_PATHS + 1];
extern bool isBegin[PG_NB_PATHS + 1];
extern bool isEnd[PG_NB_PATHS + 1];
extern float paths_Color_r[PG_NB_PATHS + 1];
extern float paths_Color_g[PG_NB_PATHS + 1];
extern float paths_Color_b[PG_NB_PATHS + 1];
extern float paths_Color_a[PG_NB_PATHS + 1];
extern float repop_ColorBG_r;
extern float repop_ColorBG_g;
extern float repop_ColorBG_b;
extern float repop_ColorCA_r;
extern float repop_ColorCA_g;
extern float repop_ColorCA_b;
extern float repop_ColorPart_r;
extern float repop_ColorPart_g;
extern float repop_ColorPart_b;
extern float paths_RadiusX[PG_NB_PATHS + 1];
extern float paths_RadiusY[PG_NB_PATHS + 1];
#ifdef PG_BEZIER_PATHS
extern int path_next_in_hull[PG_NB_PATHS + 1][4];
extern float paths_xL[PG_NB_PATHS + 1];
extern float paths_yL[PG_NB_PATHS + 1];
extern float paths_xR[PG_NB_PATHS + 1];
extern float paths_yR[PG_NB_PATHS + 1];
#endif
extern int paths_BrushID[PG_NB_PATHS + 1];

extern bool mobile_cursor;

#if defined (GN) || defined (CAAUDIO) || defined(RIVETS)
extern GLuint pg_CATable_ID;
extern GLubyte *pg_CATable;
#endif
#if defined (GN) || defined (INTERFERENCE)
extern GLuint pg_LYMlogo_texID;
extern cv::Mat pg_LYMlogo_image;
#endif


#ifdef PG_SENSORS
//////////////////////////////////////////////////////////////////////
// SENSORS
//////////////////////////////////////////////////////////////////////

// sensor translations
// current sensor layout
extern float sensorPositions[3 * PG_NB_SENSORS];
// all possible sensor layouts
extern float sensorLayouts[3 * PG_NB_SENSORS * PG_NB_MAX_SENSOR_LAYOUTS];

// sensor on off
// current sensor activation pattern
extern bool sensor_onOff[PG_NB_SENSORS];
extern double sensor_last_activation_time;
// all sensor activation patterns
extern bool sensorActivations[PG_NB_SENSORS * PG_NB_MAX_SENSOR_ACTIVATIONS];

// sample choice
// current sample choice
extern int sample_choice[PG_NB_SENSORS];
// all possible sensor layouts
extern int sensor_sample_setUps[PG_NB_MAX_SAMPLE_SETUPS][PG_NB_SENSORS];
// sample start playing time for muting after 1 cycle
extern double sample_play_start[PG_NB_MAX_SAMPLE_SETUPS * PG_NB_SENSORS];
#endif

// data structure for threads to use.
// This is passed by void pointer so it can be any data type
// that can be passed using a single void pointer (LPVOID).
class threadData {
public:
  char *fname;
  int w;
  int h;
  cv::Mat *imgThreadData;
};


// FUNCTIONS
void window_display(void);
void one_frame_variables_reset(void);
void pg_update_shader_uniforms(void);
// MOVIE STREAMING JUMPS
void pg_movie_forward(int nb_frames_forth);
void pg_movie_backward(int nb_frames_back);
// MOVIE AND CAMERA FRAME CAPTURE
void pg_update_clip_camera_and_movie_frame(void);
#if defined (TEXTURED_QUAD_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 
// PASS #0: PARTICLE ANIMATION PASS
void pg_ParticleAnimationPass(void);
#endif
// PASS #1: UPDATE (CA, PIXELS, PARTICLES)
void pg_UpdatePass(void);
#if defined (TEXTURED_QUAD_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 
// PASS #2: PARTICLE RENDERING PASS
void pg_SVGandParticleRenderingPass(void);
#endif
// PASS #3: COMPOSITION + PING-PONG ECHO: ECHOED MIX OF LAYERS
void pg_MixingPass(void);
// PASS #4: FINAL DISPLAY: MIX OF ECHOED AND NON-ECHOED LAYERS
void pg_MasterPass(void);
#ifdef PG_SENSORS
// PASS #5: SENSOR PASS
void pg_SensorPass(void);
#endif
#ifdef PG_MESHES
// PASS #6: MESH PASS
void pg_calculate_projection_matrices(void);
void pg_MeshPass(void);
#endif
#if defined(PG_WITH_HOMOGRAPHY) || defined(PG_WITH_PHOTO_HOMOGRAPHY)
void pg_calculate_homography_matrices(std::vector<cv::Point2f>* sourcePoints, std::vector<cv::Point2f>* destinationPoints, GLfloat matValues[], int dim);
#endif
// DRAWING A SCENE ON VARIOUS MODALITIES (CURVE, IMAGE, FRAMEBUFFER...)
void pg_draw_scene(DrawingMode mode, bool threaded);


#endif
