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
 
// +++++++++++++++++++++++ Metawear sensors ++++++++++++++++++++
struct metawear_sensor_data pg_mw_sensors[PG_MW_NB_MAX_SENSORS];
GLfloat pg_modelMatrixSensor[16];

//////////////////////////////////////////////////////////////////////
// SENSORS
//////////////////////////////////////////////////////////////////////

// sensor translations
// current sensor layout
float pg_sensorPositions[3 * PG_NB_SENSORS];
// all possible sensor layouts
float pg_sensorLayouts[3 * PG_NB_SENSORS * PG_NB_MAX_SENSOR_LAYOUTS];

// sensor on off
// current sensor activation pattern
bool pg_sensor_onOff[PG_NB_SENSORS];
double pg_sensor_last_activation_time;
// sensor activation as read from the pixel under their center
float pg_sensorLevel[PG_NB_SENSORS];
// all sensor activation patterns
bool pg_sensorActivations[PG_NB_SENSORS * PG_NB_MAX_SENSOR_ACTIVATIONS];

// sample choice
// current sample choice
int pg_sample_choice[PG_NB_SENSORS];
#ifndef pg_Project_CAaudio
// all possible sensor layouts
int pg_sensor_sample_setUps[PG_NB_MAX_SAMPLE_SETUPS][PG_NB_SENSORS] =
{ { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 },
{ 16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31 },
{ 32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47 } };
// sample start playing time for muting after 1 cycle
double pg_sample_play_start[PG_NB_MAX_SAMPLE_SETUPS * PG_NB_SENSORS] =
{ -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1.,
-1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1.,
-1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1. };
float pg_sample_play_volume[PG_NB_MAX_SAMPLE_SETUPS * PG_NB_SENSORS] =
{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
#else
// all possible sensor layouts
int pg_sensor_sample_setUps[PG_NB_MAX_SAMPLE_SETUPS][PG_NB_SENSORS] =
{ { 0 }, { 1 }, { 2 } };
// sample start playing time for muting after 1 cycle
double pg_sample_play_start[PG_NB_MAX_SAMPLE_SETUPS * PG_NB_SENSORS] =
{ -1.f, -1.f, -1.f };
float pg_sample_play_volume[PG_NB_MAX_SAMPLE_SETUPS * PG_NB_SENSORS] =
{ 0.0, 0.0, 0.0 };
#endif

#ifndef pg_Project_CAaudio
// sensor rendering
int pg_Sensor_order[PG_NB_SENSORS] = { 8, 13, 14, 11, 7, 2, 1, 4, 12, 15, 3, 0, 9, 10, 5, 6 };
#else
// sensor rendering
int pg_Sensor_order[PG_NB_SENSORS] = { 0 };
#endif

// current sensor
int pg_currentSensor = 0;


/////////////////////////////////////////////////////////////////
// SENSOR INITIALIZATION
/////////////////////////////////////////////////////////////////

void pg_SensorInitialization(void) {
#if defined(PG_RENOISE)
	sprintf(pg_AuxString, "/renoise/transport/start"); pg_send_message_udp((char*)"", pg_AuxString, (char*)"udp_RN_send");
#endif

	for (int indSample = 0; indSample < PG_NB_MAX_SAMPLE_SETUPS * PG_NB_SENSORS; indSample++) {
#if defined(PG_RENOISE)
		// Renoise message format && message posting
		sprintf(pg_AuxString, "/renoise/song/track/%d/prefx_volume %.2f", indSample + 1, 0.f);
		pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_RN_send");
		// Renoise message format && message posting
		sprintf(pg_AuxString, "/renoise/song/track/%d/prostx_volume %.2f", indSample + 1, 3.f);
		pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_RN_send");
#endif
		// resets the clock for replaying the sample if sensor triggered again
		pg_sample_play_start[indSample] = -1.0f;
	}
}

void pg_assignSensorPositions(void) {
	int indLayout = max(0, min(sensor_layout, PG_NB_MAX_SENSOR_LAYOUTS - 1));
	// non random layout: copy
	if (indLayout < PG_NB_MAX_SENSOR_LAYOUTS - 1) {
		for (int indSensor = 0; indSensor < PG_NB_SENSORS; indSensor++) {
			pg_sensorPositions[3 * indSensor] = pg_sensorLayouts[indLayout * 3 * PG_NB_SENSORS + 3 * indSensor];
			pg_sensorPositions[3 * indSensor + 1] = pg_sensorLayouts[indLayout * 3 * PG_NB_SENSORS + 3 * indSensor + 1];
			pg_sensorPositions[3 * indSensor + 2] = 0.1f;
		}
	}
	// random layout: regenerate
	else {
		for (int indSensor = 0; indSensor < PG_NB_SENSORS; indSensor++) {
			pg_sensorPositions[3 * indSensor] = pg_workingWindow_width / 2.0f + pg_workingWindow_width / 2.0f * (2.0f * rand_0_1 - 1.0f);
			pg_sensorPositions[3 * indSensor + 1] = PG_WINDOW_HEIGHT / 2.0f + PG_WINDOW_HEIGHT / 2.0f * (2.0f * rand_0_1 - 1.0f);
			pg_sensorPositions[3 * indSensor + 2] = 0.1f;
		}
	}
}

void pg_assignSensorActivations(void) {
	// copies the single central activation
	int indActivation = std::max(0, std::min(sensor_activation, PG_NB_MAX_SENSOR_ACTIVATIONS - 1));
	for (int indSensor = 0; indSensor < PG_NB_SENSORS; indSensor++) {
		pg_sensor_onOff[indSensor] = pg_sensorActivations[indActivation * PG_NB_SENSORS + indSensor];
	}
	// incremental sensor activation
	if (sensor_activation == 5) {
		pg_sensor_last_activation_time = pg_CurrentClockTime;
		//printf("initial activ %f %d\n", pg_CurrentClockTime, sensor_activation);
	}
}

void pg_sensor_sample_setUp_interpolation(void) {
	float indSetUp = std::max(0.0f, std::min(sensor_sample_setUp, (float)(PG_NB_MAX_SAMPLE_SETUPS - 1)));

	float sensor_sample_setUp_integerPart = floor(indSetUp);
	float sensor_sample_setUp_floatPart = indSetUp - sensor_sample_setUp_integerPart;
	int intIndSetup = (int)(sensor_sample_setUp_integerPart);

	// copies the preset that corresponds to the integer part
	for (int ind = 0; ind < PG_NB_SENSORS; ind++) {
		pg_sample_choice[ind] = pg_sensor_sample_setUps[intIndSetup][ind];
	}

	// for the decimal part, copies hybrid sensors of upper level with a ratio
	// proportional to the ratio between floor and current value 
	if (sensor_sample_setUp_floatPart > 0.0f) {
		int nbhybridSensors = (int)round(sensor_sample_setUp_floatPart * PG_NB_SENSORS);

		// book keeping of hybridized sensors
		bool hybridized[PG_NB_SENSORS];
		for (int ind = 0; ind < PG_NB_SENSORS; ind++) {
			hybridized[ind] = false;
		}

		for (int indSensor = 0; indSensor < min(PG_NB_SENSORS, nbhybridSensors); indSensor++) {
			int count = (int)round(rand_0_1 * PG_NB_SENSORS);
			for (int ind = 0; ind < PG_NB_SENSORS; ind++) {
				int translatedIndex = (count + PG_NB_SENSORS) % PG_NB_SENSORS;
				if (!hybridized[translatedIndex]) {
					pg_sample_choice[translatedIndex] = pg_sensor_sample_setUps[intIndSetup + 1][translatedIndex];
					hybridized[translatedIndex] = true;
				}
			}
		}
	}
}

void pg_MetawearSensorInitialization() {
	for (int ind_sensor = 0; ind_sensor < PG_MW_NB_MAX_SENSORS; ind_sensor++) {
		for (int i = 0; i < 3; i++) {
			pg_mw_sensors[ind_sensor].mw_mss_pos[i] = 0;
		}
		pg_mw_sensors[ind_sensor].mw_mss_pos_update = false;
	}
}

