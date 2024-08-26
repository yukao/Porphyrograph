/*! \file pg-sensor.h
*
*
*     File pg-sensor.h
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

#ifndef PG_SENSOR_H
#define PG_SENSOR_H

//////////////////////////////////////////////////////////////////////////////////////////////////////
// CONSTs
//////////////////////////////////////////////////////////////////////////////////////////////////////

// +++++++++++++++++++++++ Metawear sensors ++++++++++++++++++++
#define PG_MW_NB_MAX_SENSORS 2
struct metawear_sensor_data {
	float mw_mss_pos[3];		// x y z
	bool  mw_mss_pos_update;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////
// EXPORTED VARIABLES
////////////////////////////////////////////////////////////////////////////////////////////////////// 

// sensor on off
// current sensor activation pattern
extern bool pg_sensor_onOff[PG_NB_SENSORS];
extern double pg_sensor_last_activation_time;

// metawear sensors
extern struct metawear_sensor_data pg_mw_sensors[PG_MW_NB_MAX_SENSORS];

// sensor translations
// all possible sensor layouts
extern float pg_sensorLayouts[3 * PG_NB_SENSORS * PG_NB_MAX_SENSOR_LAYOUTS];
// all sensor activation patterns
extern bool pg_sensorActivations[PG_NB_SENSORS * PG_NB_MAX_SENSOR_ACTIVATIONS];
// all possible sensor layouts
extern int pg_Sensor_order[PG_NB_SENSORS];
// sensor translations
// current sensor layout
extern float pg_sensorPositions[3 * PG_NB_SENSORS];
// sensor activation as read from the pixel under their center
extern float pg_sensorLevel[PG_NB_SENSORS];

// current sensor
extern int pg_currentSensor;

//////////////////////////////////////////////////////////////////////////////////////////////////////
// EXPORTED FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////

// makes sample choice interpolated between two setups
void pg_sensor_sample_setUp_interpolation(void);

// SENSOR INITIALIZATION
void pg_SensorInitialization(void);
// sensor layout on screen
void pg_assignSensorPositions(void);
// incremental or immediate sensor activations
void pg_assignSensorActivations(void);
// initial values for metawear sensors
void pg_MetawearSensorInitialization();

// OSC messages for sensors
void pg_aliasScript_Sensor(string address_string, string string_argument_0,
	float float_arguments[PG_MAX_OSC_ARGUMENTS], int nb_arguments, int indVar);
// pixel value under sensor
void pg_readSensors(void);
#endif