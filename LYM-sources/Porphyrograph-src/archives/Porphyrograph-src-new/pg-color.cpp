/*! \file pg-stroke.cpp
 *
 *
 *     File pg-stroke.cpp
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
 
float lowPalette[3];
int indLowPalette[3];
int indUpperPalette[3];
float pulsed_color_percentage[3];
float bandpass_3color_palette[3][3]; 
float light_color_prec[PG_NB_LIGHTS + 1] = { 0.f };
float light_grey_prec[PG_NB_LIGHTS + 1] = { 0.f };
float light_dimmer_prec[PG_NB_LIGHTS + 1] = { 0.f };
float light_strobe_prec[PG_NB_LIGHTS + 1] = { 0.f };
// addition of color for light based on palette
float light_color_pulse_prec[PG_NB_LIGHTS + 1] = { 0.f };
float light_grey_pulse_prec[PG_NB_LIGHTS + 1] = { 0.f };
float light_dimmer_pulse_prec[PG_NB_LIGHTS + 1] = { 0.f };
float light_strobe_pulse_prec[PG_NB_LIGHTS + 1] = { 0.f };

// addition of colorfor pen  based on palette
float pulsed_pen_color[4];
float pulsed_repop_colorPart[3];
float pulsed_repop_colorBG[3];
float pulsed_repop_colorCA[3];

// memory of preceding pulsed color values to avoid unnecessary update
float repop_colorBG_prec = -1.f;
float repop_colorBG_pulse_prec = -1.f;
float repop_greyBG_prec = -1.f;
float repop_greyBG_pulse_prec = -1.f;

float repop_colorCA_prec = -1.f;
float repop_colorCA_pulse_prec = -1.f;
float repop_greyCA_prec = -1.f;
float repop_greyCA_pulse_prec = -1.f;

float pen_color_prec = -1.f;
float pen_color_pulse_prec = -1.f;
float pen_grey_prec = -1.f;
float pen_grey_pulse_prec = -1.f;
float pen_hue_prec = -1.f;
float pen_hue_pulse_prec = -1.f;
float pen_sat_prec = -1.f;
float pen_sat_pulse_prec = -1.f;
float pen_value_prec = -1.f;
float pen_value_pulse_prec = -1.f;

float repop_colorPart_prec = -1.f;
float repop_colorPart_pulse_prec = -1.f;
float repop_greyPart_prec = -1.f;
float repop_greyPart_pulse_prec = -1.f;
float repop_huePart_prec = -1.f;
float repop_huePart_pulse_prec = -1.f;
float repop_satPart_prec = -1.f;
float repop_satPart_pulse_prec = -1.f;
float repop_valuePart_prec = -1.f;
float repop_valuePart_pulse_prec = -1.f;

//////////////////////////////////////////////////////////////
// PULSE COLOR FUNCTIONS
//////////////////////////////////////////////////////////////
void compute_pulsed_palette_color(float color, float color_pulse, float grey, float grey_pulse, float pulsed_color[3], pg_ColorTarget color_target) {
	if (pg_Palettes[pg_current_configuration_rank].empty()) {
		pulsed_color[3] = { 1 };
		return;
	}

	// computes the corresponding low and high palette and the percentages of contribution of each of them
	for (int indColorBandPass = 0; indColorBandPass < 3; indColorBandPass++) {
		float pulsed_color_palette;
		if (color_pulse != 0) {
			pulsed_color_palette = color + pulse[indColorBandPass] * color_pulse;
		}
		else {
			pulsed_color_palette = color;
		}
		pulsed_color_palette *= pg_Palettes[pg_current_configuration_rank].size();
		pulsed_color_percentage[indColorBandPass] = modf(pulsed_color_palette, &(lowPalette[indColorBandPass]));
		while (lowPalette[indColorBandPass] < 0) {
			lowPalette[indColorBandPass] += pg_Palettes[pg_current_configuration_rank].size();
		}
		indLowPalette[indColorBandPass] = int(lowPalette[indColorBandPass]) % pg_Palettes[pg_current_configuration_rank].size();
		indUpperPalette[indColorBandPass] = (indLowPalette[indColorBandPass] + 1) % pg_Palettes[pg_current_configuration_rank].size();
	}

	// the base palette is obtained by interpolating the two closest palettes according to color
	//printf("pulsed_color_percentage %.2f %.2f %.2f / %.2f\n", pulsed_color_percentage[0], pulsed_color_percentage[1], pulsed_color_percentage[2], color_pulse);
	//printf("ind low palettes %d %d %d\n", indLowPalette[0], indLowPalette[1], indLowPalette[2]);
	//printf("ind high palettes %d %d %d\n", indUpperPalette[0], indUpperPalette[1], indUpperPalette[2]);
	for (int indColorBandPass = 0; indColorBandPass < 3; indColorBandPass++) {
		for (int indChannel = 0; indChannel < 3; indChannel++) {
			bandpass_3color_palette[indColorBandPass][indChannel]
				= (1.f - pulsed_color_percentage[indColorBandPass]) * pg_Palettes[pg_current_configuration_rank][indLowPalette[indColorBandPass]].pen_palette_colors_values[indColorBandPass * 3 + indChannel]
				+ pulsed_color_percentage[indColorBandPass] * pg_Palettes[pg_current_configuration_rank][indUpperPalette[indColorBandPass]].pen_palette_colors_values[indColorBandPass * 3 + indChannel];
			bandpass_3color_palette[indColorBandPass][indChannel] = min(1.f, bandpass_3color_palette[indColorBandPass][indChannel]);
		}
	}

	// stores the bandpass colors that will be shown in the palette interface in the Master shader for color visualization by user
	if (color_target == _PG_PEN) {
		for (int indColorBandPass = 0; indColorBandPass < 3; indColorBandPass++) {
			for (int indChannel = 0; indChannel < 3; indChannel++) {
				pen_bandpass_3color_palette[indColorBandPass][indChannel] = bandpass_3color_palette[indColorBandPass][indChannel];
			}
		}
	}
	//printf("pen color %.2f palette low/upper/perc %d/%d/%.2f -> colors %.1f %.1f %.1f       %.1f %.1f %.1f        %.1f %.1f %.1f\n", 
	//	pen_color, indLowPenPalette, indUpperPenPalette, percentage, 
	//	bandpass_3color_palette[0][0], bandpass_3color_palette[0][1], bandpass_3color_palette[0][2], 
	//  bandpass_3color_palette[1][0], bandpass_3color_palette[1][1], bandpass_3color_palette[1][2], 
	//  bandpass_3color_palette[2][0], bandpass_3color_palette[2][1], bandpass_3color_palette[2][2]);
	// calculating the color from base luminance + palette colors modulated by the three frequence ranges
	for (int indChannel = 0; indChannel < 3; indChannel++) {
		// adding a base luminance
		pulsed_color[indChannel] = grey + pulse_average * grey_pulse;
		// clip art color is not modulated by bandpass pulse contrary to the other colors which are modified to avoid monochromy
		if (pulse_average != 0 && color_target != _PG_CLIP_ART) {
			for (int indColorBandPass = 0; indColorBandPass < 3; indColorBandPass++) {
#if defined(var_alKemi)
				if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_alKemi]) {
					pulsed_color[indChannel]
						+= color_spectrum_coef * bandpass_3color_palette[indColorBandPass][indChannel]
						* (pulse[indColorBandPass] / (3.f * pulse_average));
				}
				else {
					pulsed_color[indChannel]
						+= bandpass_3color_palette[indColorBandPass][indChannel]
						* (pulse[indColorBandPass] / (3.f * pulse_average));
				}
#else
				pulsed_color[indChannel]
					+= bandpass_3color_palette[indColorBandPass][indChannel]
					* (pulse[indColorBandPass] / (3.f * pulse_average));
#endif
			}
		}
		else {
			for (int indColorBandPass = 0; indColorBandPass < 3; indColorBandPass++) {
#if defined(var_alKemi)
				if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_alKemi]) {
					pulsed_color[indChannel]
						+= color_spectrum_coef * bandpass_3color_palette[indColorBandPass][indChannel] / 3.f;
				}
				else {
					pulsed_color[indChannel]
						+= bandpass_3color_palette[indColorBandPass][indChannel] / 3.f;
				}
#else
				pulsed_color[indChannel]
					+= bandpass_3color_palette[indColorBandPass][indChannel] / 3.f;
#endif
			}
		}
		pulsed_color[indChannel] = min(1.f, pulsed_color[indChannel]);
	}
	float value = (pulsed_color[0] < pulsed_color[1]) ? pulsed_color[1] : pulsed_color[0];
	value = (value < pulsed_color[2]) ? pulsed_color[2] : value;
	// printf( "pulsed_color: %f %f %f\n" , pulsed_color[0] , pulsed_color[1] , pulsed_color[2] );

	// color_pulse negative => saturation of the pulsed color
	if (color_pulse < 0 && value > 0) {
		for (int indChannel = 0; indChannel < 3; indChannel++) {
			pulsed_color[indChannel] /= value;
		}
	}

	// the colors for drawing are inverted in case of inverted rendering, so that the drawing colors are not seen inverted
	if (invertAllLayers) {
		pulsed_color[0] = value - pulsed_color[0];
		pulsed_color[1] = value - pulsed_color[1];
		pulsed_color[2] = value - pulsed_color[2];
		float new_value = (pulsed_color[0] < pulsed_color[1]) ? pulsed_color[1] : pulsed_color[0];
		new_value = (new_value < pulsed_color[2]) ? pulsed_color[2] : new_value;
		// the new value should be the same as the intial one, the difference is subtracted from the channels
		pulsed_color[0] += value - new_value;
		pulsed_color[1] += value - new_value;
		pulsed_color[2] += value - new_value;
	}
}

// from https://www.codespeedy.com/hsv-to-rgb-in-cpp/

void HSVtoRGB(float h, float s, float v, float* r, float* g, float* b) {
	if (h > 1.f || h < 0 || s>1.f || s < 0 || v>1.f || v < 0) {
		//std::cout << "The givem HSV values are not in valid range" << std::endl;
		h = max(min(h, 1.f), 0.f);
		s = max(min(s, 1.f), 0.f);
		v = max(min(v, 1.f), 0.f);
	}
	float H = h * 360.f;
	float C = s * v;
	float X = C * float(1.f - abs(fmod(H / 60.0f, 2) - 1));
	float m = v - C;
	if (H >= 0 && H < 60.f) {
		*r = C, * g = X, * b = 0;
	}
	else if (H >= 60.f && H < 120.f) {
		*r = X, * g = C, * b = 0;
	}
	else if (H >= 120.f && H < 180.f) {
		*r = 0, * g = C, * b = X;
	}
	else if (H >= 180.f && H < 240.f) {
		*r = 0, * g = X, * b = C;
	}
	else if (H >= 240.f && H < 300.f) {
		*r = X, * g = 0, * b = C;
	}
	else {
		*r = C, * g = 0, * b = X;
	}
	*r += m;
	*g += m;
	*b += m;
	//cout << "R : " << R << endl;
	//cout << "G : " << G << endl;
	//cout << "B : " << B << endl;
}

void RGBtoHSV(float r, float g, float b, float* h, float* s, float* v) {
	if (r > 1.f || r < 0 || g > 1.f || g < 0 || b > 1.f || b < 0) {
		//std::cout << "The givem RGB values are not in valid range" << std::endl;
		r = max(min(r, 1.f), 0.f);
		g = max(min(g, 1.f), 0.f);
		b = max(min(b, 1.f), 0.f);
	}

	float maxval, minval;

	maxval = max(max(r, g), b);
	minval = min(min(r, g), b);

	float difference = maxval - minval;

	if (difference == 0)
		*h = 0.;
	else if (r == maxval)
		*h = fmod(((60.f * ((g - b) / difference)) + 360.f), 360.0f);
	else if (g == maxval)
		*h = fmod(((60.f * ((b - r) / difference)) + 120.f), 360.0f);
	else if (b == maxval)
		*h = fmod(((60.f * ((r - g) / difference)) + 240.f), 360.0f);

	if (maxval == 0)
		*s = 0.;
	else
		*s = (difference / maxval);

	*v = maxval;

	//cout << "H : " << *h << endl;
	//cout << "S : " << *s << endl;
	//cout << "V : " << *v << endl;
}

void compute_pulsed_HSV_color(float hue, float hue_pulse, float sat, float sat_pulse, float value, float value_pulse, float pulsed_color[3], bool is_pen_color) {
	float pulsed_hue = hue + pulse_average * hue_pulse;
	float pulsed_sat = sat + pulse_average * sat_pulse;
	float pulsed_value = value + pulse_average * value_pulse;

	// ciruclar hue, keeps the value between 0 and 1
	if (pulsed_hue < 0) {
		float intpart;
		pulsed_hue = modf(pulsed_hue, &intpart) + 1.f;
	}
	else if (pulsed_hue > 1.f) {
		float intpart;
		pulsed_hue = modf(pulsed_hue, &intpart);
	}

	pulsed_sat = max(0.f, min(pulsed_sat, 1.f));
	pulsed_value = max(0.f, min(pulsed_value, 1.f));

	HSVtoRGB(pulsed_hue, pulsed_sat, pulsed_value, &pulsed_color[0], &pulsed_color[1], &pulsed_color[2]);
	//printf("pulsed color RGB %.2f %.2f %.2f \n", pulsed_color[0], pulsed_color[1], pulsed_color[2]);

	// the hues for drawing are inverted in case of inverted rendering, so that the drawing hues are not seen inverted
	if (invertAllLayers) {
		pulsed_color[0] = pulsed_value - pulsed_color[0];
		pulsed_color[1] = pulsed_value - pulsed_color[1];
		pulsed_color[2] = pulsed_value - pulsed_color[2];
		float new_value = (pulsed_color[0] < pulsed_color[1]) ? pulsed_color[1] : pulsed_color[0];
		new_value = (new_value < pulsed_color[2]) ? pulsed_color[2] : new_value;
		// the new value should be the same as the intial one, the difference is subtracted from the channels
		pulsed_color[0] += pulsed_value - new_value;
		pulsed_color[1] += pulsed_value - new_value;
		pulsed_color[2] += pulsed_value - new_value;
	}
}

void pg_update_pulsed_colors(void) {
	////////////////////////////////////////////////////////////////////////////////////////////////////
	// PEN PULSED COLOR
	// blending the two closest palettes of the pulsed pen color
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_pen_color]
		&& !pen_hsv) {
		if (pen_color != pen_color_prec || pen_color_pulse != pen_color_pulse_prec
			|| pen_grey != pen_grey_prec || pen_grey_pulse != pen_grey_pulse_prec
			|| ((pen_grey_pulse != 0 || pen_color_pulse != 0)
				&& (pulse_prec[0] != pulse[0] || pulse_prec[1] != pulse[1] || pulse_prec[2] != pulse[2]))) {
			compute_pulsed_palette_color(pen_color, pen_color_pulse, pen_grey, pen_grey_pulse, pulsed_pen_color, _PG_PEN);
			pen_color_prec = pen_color;
			pen_color_pulse_prec = pen_color_pulse;
			pen_grey_prec = pen_grey;
			pen_grey_pulse_prec = pen_grey_pulse;
			pulse_prec[0] = pulse[0];
			pulse_prec[1] = pulse[1];
			pulse_prec[2] = pulse[2];
		}
	}
	else if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_pen_hue]
		&& pg_FullScenarioActiveVars[pg_current_configuration_rank][_pen_sat]
		&& pg_FullScenarioActiveVars[pg_current_configuration_rank][_pen_value]
		&& pen_hsv) {
		if (pen_hue != pen_hue_prec || pen_hue_pulse != pen_hue_pulse_prec
			|| pen_sat != pen_sat_prec || pen_sat_pulse != pen_sat_pulse_prec
			|| pen_value != pen_value_prec || pen_value_pulse != pen_value_pulse_prec
			|| ((pen_sat_pulse != 0 || pen_value_pulse != 0 || pen_hue_pulse != 0)
				&& (pulse_prec[0] != pulse[0] || pulse_prec[1] != pulse[1] || pulse_prec[2] != pulse[2]))) {
			compute_pulsed_HSV_color(pen_hue, pen_hue_pulse, pen_sat, pen_sat_pulse, pen_value, pen_value_pulse, pulsed_pen_color, true);
			pen_hue_prec = pen_hue;
			pen_hue_pulse_prec = pen_hue_pulse;
			pen_sat_prec = pen_sat;
			pen_sat_pulse_prec = pen_sat_pulse;
			pen_value_prec = pen_value;
			pen_value_pulse_prec = pen_value_pulse;
			pulse_prec[0] = pulse[0];
			pulse_prec[1] = pulse[1];
			pulse_prec[2] = pulse[2];
		}
	}

	pulsed_pen_color[3] = 1.f;


	/***************************** SHOULD BE REACTIVATED WHEN THE INTERFACE CAN DISPLAY COLORS */
	//sprintf(AuxString, "/pen_colorPenPalette %.5f %.5f %.5f %.5f %.5f %.5f %.5f %.5f %.5f",
	//	pen_bandpass_3color_palette[0][0], pen_bandpass_3color_palette[0][1], pen_bandpass_3color_palette[0][2], 
	//  pen_bandpass_3color_palette[1][0], pen_bandpass_3color_palette[1][1], pen_bandpass_3color_palette[1][2], 
	//  pen_bandpass_3color_palette[2][0], pen_bandpass_3color_palette[2][1], pen_bandpass_3color_palette[2][2]);
	//pg_send_message_udp((char *)"fffffffff", AuxString, (char *)"udp_TouchOSC_send");
	//sprintf(AuxString, "/pulsed_pen_color %.5f %.5f %.5f",
	//	pulsed_pen_color[0], pulsed_pen_color[1], pulsed_pen_color[2]);
	//pg_send_message_udp((char *)"fff", AuxString, (char *)"udp_TouchOSC_send");


	////////////////////////////////////////////////////////////////////////////////////////////////////
	// REPOP PULSED COLOR
	// blending the two closest palettes of the pulsed Repop color
	if (repop_colorBG != repop_colorBG_prec || repop_colorBG_pulse != repop_colorBG_pulse_prec
		|| repop_greyBG != repop_greyBG_prec || repop_greyBG_pulse != repop_greyBG_pulse_prec
		|| ((repop_greyBG_pulse != 0 || repop_colorBG_pulse != 0)
			&& (pulse_prec[0] != pulse[0] || pulse_prec[1] != pulse[1] || pulse_prec[2] != pulse[2]))) {
		compute_pulsed_palette_color(repop_colorBG, repop_colorBG_pulse, repop_greyBG, repop_greyBG_pulse, pulsed_repop_colorBG, _PG_REPOP);
		repop_colorBG_prec = repop_colorBG;
		repop_colorBG_pulse_prec = repop_colorBG_pulse;
		repop_greyBG_prec = repop_greyBG;
		repop_greyBG_pulse_prec = repop_greyBG_pulse;
		pulse_prec[0] = pulse[0];
		pulse_prec[1] = pulse[1];
		pulse_prec[2] = pulse[2];
	}


	////////////////////////////////////////////////////////////////////////////////////////////////////
	// LIGHT PULSED COLOR
	// blending the two closest palettes of the pulsed Repop color
	if (pg_FrameNo % 10 == 0) {
		pg_UpdateLightGroups_from_LightVars();
	}

	if (repop_colorCA != repop_colorCA_prec || repop_colorCA_pulse != repop_colorCA_pulse_prec
		|| repop_greyCA != repop_greyCA_prec || repop_greyCA_pulse != repop_greyCA_pulse_prec
		|| ((repop_greyCA_pulse != 0 || repop_colorCA_pulse != 0)
			&& (pulse_prec[0] != pulse[0] || pulse_prec[1] != pulse[1] || pulse_prec[2] != pulse[2]))) {
		compute_pulsed_palette_color(repop_colorCA, repop_colorCA_pulse, repop_greyCA, repop_greyCA_pulse, pulsed_repop_colorCA, _PG_REPOP);
		repop_colorCA_prec = repop_colorCA;
		repop_colorCA_pulse_prec = repop_colorCA_pulse;
		repop_greyCA_prec = repop_greyCA;
		repop_greyCA_pulse_prec = repop_greyCA_pulse;
		pulse_prec[0] = pulse[0];
		pulse_prec[1] = pulse[1];
		pulse_prec[2] = pulse[2];
	}

	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_repop_colorPart]
		&& pg_FullScenarioActiveVars[pg_current_configuration_rank][_repop_greyPart]
		&& !repop_hsvPart) {
		if (repop_colorPart != repop_colorPart_prec || repop_colorPart_pulse != repop_colorPart_pulse_prec
			|| repop_greyPart != repop_greyPart_prec || repop_greyPart_pulse != repop_greyPart_pulse_prec
			|| ((repop_greyPart_pulse != 0.f || repop_colorPart_pulse != 0.f)
				&& (pulse_prec[0] != pulse[0] || pulse_prec[1] != pulse[1] || pulse_prec[2] != pulse[2]))) {
			compute_pulsed_palette_color(repop_colorPart, repop_colorPart_pulse, repop_greyPart, repop_greyPart_pulse, pulsed_repop_colorPart, _PG_REPOP);
			repop_colorPart_prec = repop_colorPart;
			repop_colorPart_pulse_prec = repop_colorPart_pulse;
			repop_greyPart_prec = repop_greyPart;
			repop_greyPart_pulse_prec = repop_greyPart_pulse;
			pulse_prec[0] = pulse[0];
			pulse_prec[1] = pulse[1];
			pulse_prec[2] = pulse[2];
		}
	}
	else if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_repop_huePart]
		&& pg_FullScenarioActiveVars[pg_current_configuration_rank][_repop_satPart]
		&& pg_FullScenarioActiveVars[pg_current_configuration_rank][_repop_valuePart]
		&& repop_hsvPart) {
		if (repop_huePart != repop_huePart_prec || repop_huePart_pulse != repop_huePart_pulse_prec
			|| repop_satPart != repop_satPart_prec || repop_satPart_pulse != repop_satPart_pulse_prec
			|| repop_valuePart != repop_valuePart_prec || repop_valuePart_pulse != repop_valuePart_pulse_prec
			|| ((repop_satPart_pulse != 0.f || repop_valuePart_pulse != 0.f || repop_huePart_pulse != 0.f)
				&& (pulse_prec[0] != pulse[0] || pulse_prec[1] != pulse[1] || pulse_prec[2] != pulse[2]))) {
			compute_pulsed_HSV_color(repop_huePart, repop_huePart_pulse, repop_satPart, repop_satPart_pulse, repop_valuePart, repop_valuePart_pulse, pulsed_repop_colorPart, false);
			repop_huePart_prec = repop_huePart;
			repop_huePart_pulse_prec = repop_huePart_pulse;
			repop_satPart_prec = repop_satPart;
			repop_satPart_pulse_prec = repop_satPart_pulse;
			repop_valuePart_prec = repop_valuePart;
			repop_valuePart_pulse_prec = repop_valuePart_pulse;
			pulse_prec[0] = pulse[0];
			pulse_prec[1] = pulse[1];
			pulse_prec[2] = pulse[2];
		}
	}

	/***************************** SHOULD BE REACTIVATED WHEN THE INTERFACE CAN DISPLAY COLORS */
	//sprintf(AuxString, "/repop_colorRepopPalette %.5f %.5f %.5f %.5f %.5f %.5f %.5f %.5f %.5f",
	//  pen_bandpass_3color_palette[0][0], pen_bandpass_3color_palette[0][1], pen_bandpass_3color_palette[0][2], 
	//  pen_bandpass_3color_palette[1][0], pen_bandpass_3color_palette[1][1], pen_bandpass_3color_palette[1][2], 
	//  pen_bandpass_3color_palette[2][0], pen_bandpass_3color_palette[2][1], pen_bandpass_3color_palette[2][2]);
	//pg_send_message_udp((char *)"fffffffff", AuxString, (char *)"udp_TouchOSC_send");
	//sprintf(AuxString, "/pulsed_repop_color %.5f %.5f %.5f",
	//	pulsed_repop_color[0], pulsed_repop_color[1], pulsed_repop_color[2]);
	//pg_send_message_udp((char *)"fff", AuxString, (char *)"udp_TouchOSC_send");
}

