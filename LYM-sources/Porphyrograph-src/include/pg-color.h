/*! \file pg-color.h
 *
 *
 *     File pg-color.h
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

#ifndef PG_COLOR_H
#define PG_COLOR_H

//////////////////////////////////////////////////////////////////////////////////////////////////////
// CONSTs
//////////////////////////////////////////////////////////////////////////////////////////////////////

enum pg_ColorTarget { pg_enum_PEN_COLOR = 0, pg_enum_REPOP_COLOR, pg_enum_CLIP_ART_COLOR, pg_enum_LIGHT_COLOR };
 
//////////////////////////////////////////////////////////////////////////////////////////////////////
// COLOR AND COLOR PRESETS MANAGEMENT CLASS
//////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////
// COLOR PRESETS
// pen palettes presets
class Color {
public:
	float color;
	float grey;
	float alpha;
	Color() {
		color = 0.f;
		grey = 0.f;
		alpha = 1.f;
	}
	Color(float c, float g, float a) {
		color = c;
		grey = g;
		alpha = a;
	}
	~Color() {
	}
	Color operator*(float x) const
	{
		return Color(color * x, grey * x, alpha * x);
	}
	Color operator+(const Color c) const
	{
		return Color(color + c.color, grey + c.grey, alpha + c.alpha);
	}
	void print(void) {
		printf("Color %.2f, Color %.2f, Color %.2f\n", color, grey, alpha);
	}
};
class ColorPreset {
public:
	string pen_colorPresets_names;
	Color pg_colorPreset_values;
	ColorPreset(string &id, Color &color) {
		pen_colorPresets_names = id;
		pg_colorPreset_values = color;
	}
	~ColorPreset(void) {
	}
};

////////////////////////////////////////////
// COLOR PALETTES
// pen palettes colors
class Palette {
public:
	string pen_palette_colors_names;
	array<float, 9> pen_palette_colors_values;
	Palette(string &id, array<float, 9> &colors) {
		pen_palette_colors_names = id;
		pen_palette_colors_values = colors;
	}
	~Palette(void) {
	}
};

//////////////////////////////////////////////////////////////////////////////////////////////////////
// EXPORTED VARIABLES
////////////////////////////////////////////////////////////////////////////////////////////////////// 

// predefined colors
extern vector<ColorPreset> pg_ColorPresets[PG_MAX_SCENARIOS];
// palettes made of 3 colors corresponding to low/medium/high pitch sound
extern vector<Palette> pg_Palettes[PG_MAX_SCENARIOS];

// ++++++++++++++++++++++ MUSIC CONTROL ++++++++++++++++++++ 
// music control inputs received at each frame
// addition of color based on palette for pen
extern float pg_pulsed_pen_color[4];

// color based on palette for BG, CA and particles
extern float pg_pulsed_repop_colorBG[3];
extern float pg_pulsed_repop_colorCA[3];
extern float pg_pulsed_repop_colorPart[3];

//////////////////////////////////////////////////////////////////////////////////////////////////////
// EXPORTED FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////
// Cuurent palette after interpolation based on sound
void pg_compute_pulsed_palette_color(float color, float color_pulse, float grey, float grey_pulse, float pulsed_color[3], pg_ColorTarget color_target);
void pg_compute_pulsed_HSV_color(float hue, float hue_pulse, float sat, float sat_pulse, float value, float value_pulse, float pulsed_color[3], bool is_pen_color);
void pg_HSVtoRGB(float h, float s, float v, float* r, float* g, float* b);
void pg_RGBtoHSV(float r, float g, float b, float* h, float* s, float* v);
void pg_update_pulsed_colors(void);

#endif