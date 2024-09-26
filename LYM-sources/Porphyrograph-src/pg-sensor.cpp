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
 
//////////////////////////////////////////////////////////////////////////////////////////////////////
// GLOBAL VARS
//////////////////////////////////////////////////////////////////////////////////////////////////////

// +++++++++++++++++++++++ Metawear sensors ++++++++++++++++++++
struct metawear_sensor_data pg_mw_sensors[PG_MW_NB_MAX_SENSORS];

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
int pg_sensor_sample_setUps[PG_NB_MAX_SENSOR_SAMPLE_SETUPS][PG_NB_SENSORS] =
{ { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 },
{ 16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31 },
{ 32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47 } };
// sample start playing time for muting after 1 cycle
double pg_sample_play_start[PG_NB_MAX_SENSOR_SAMPLE_SETUPS * PG_NB_SENSORS] =
{ -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1.,
-1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1.,
-1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1. };
float pg_sample_play_volume[PG_NB_MAX_SENSOR_SAMPLE_SETUPS * PG_NB_SENSORS] =
{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
#else
// all possible sensor layouts
int pg_sensor_sample_setUps[PG_NB_MAX_SENSOR_SAMPLE_SETUPS][PG_NB_SENSORS] =
{ { 0 }, { 1 }, { 2 } };
// sample start playing time for muting after 1 cycle
double pg_sample_play_start[PG_NB_MAX_SENSOR_SAMPLE_SETUPS * PG_NB_SENSORS] =
{ -1.f, -1.f, -1.f };
float pg_sample_play_volume[PG_NB_MAX_SENSOR_SAMPLE_SETUPS * PG_NB_SENSORS] =
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


///////////////////////////////////////////////////////////////////////////////////////////////////////
// SENSOR INITIALIZATION
//////////////////////////////////////////////////////////////////////////////////////////////////////

void pg_SensorInitialization(void) {
#if defined(PG_RENOISE)
	sprintf(pg_AuxString, "/renoise/transport/start"); pg_send_message_udp((char*)"", pg_AuxString, (char*)"udp_RN_send");
#endif

	for (int indSample = 0; indSample < PG_NB_MAX_SENSOR_SAMPLE_SETUPS * PG_NB_SENSORS; indSample++) {
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

// sensor layout on screen
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

// incremental or immediate sensor activations
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

// makes sample choice interpolated between two setups
void pg_sensor_sample_setUp_interpolation(void) {
	float indSetUp = std::max(0.0f, std::min(sensor_sample_setUp, (float)(PG_NB_MAX_SENSOR_SAMPLE_SETUPS - 1)));

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

// initial values for metawear sensors
void pg_MetawearSensorInitialization() {
	for (int ind_sensor = 0; ind_sensor < PG_MW_NB_MAX_SENSORS; ind_sensor++) {
		for (int i = 0; i < 3; i++) {
			pg_mw_sensors[ind_sensor].mw_mss_pos[i] = 0;
		}
		pg_mw_sensors[ind_sensor].mw_mss_pos_update = false;
	}
}
 
///////////////////////////////////////////////////////////////////////////////////////////////////////
// SENSOR OSC COMMANDS
//////////////////////////////////////////////////////////////////////////////////////////////////////

void pg_aliasScript_Sensor(string address_string, string string_argument_0,
	float float_arguments[PG_MAX_OSC_ARGUMENTS], int nb_arguments, int indVar) {
	// special command not in the scenario file
	switch (indVar) {
	case _sensor_layout_plus:
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_sensor_layout]) {
			if (pg_FullScenarioActiveVars[pg_ind_scenario][_sensor_layout]) {
				sensor_layout = (sensor_layout + 1) % PG_NB_MAX_SENSOR_LAYOUTS;
				pg_BrokenInterpolationVar[_sensor_layout] = true;
				pg_assignSensorPositions();
			}
		}
		break;
	case _sensor_sample_setUp_plus:
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_sensor_layout]) {
			if (pg_FullScenarioActiveVars[pg_ind_scenario][_sensor_layout]) {
				sensor_sample_setUp = (sensor_sample_setUp + 1);
				if (sensor_sample_setUp >= PG_NB_MAX_SENSOR_SAMPLE_SETUPS) {
					sensor_sample_setUp -= PG_NB_MAX_SENSOR_SAMPLE_SETUPS;
				}
				pg_BrokenInterpolationVar[_sensor_sample_setUp] = true;
				pg_sensor_sample_setUp_interpolation();
			}
		}
		break;
	case _sensor_activation_plus:
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_sensor_layout]) {
			if (pg_FullScenarioActiveVars[pg_ind_scenario][_sensor_layout]) {
				sensor_activation = (sensor_activation + 1) % PG_NB_MAX_SENSOR_ACTIVATIONS;
				pg_BrokenInterpolationVar[_sensor_activation] = true;
				pg_assignSensorActivations();
			}
		}
		break;
	case _mw_mss_pos:
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_pg_metawear]) {
			if (pg_metawear) {
				int ind_sensor = int(float_arguments[0]);
				if (ind_sensor < PG_MW_NB_MAX_SENSORS) {
					for (int i = 0; i < 3; i++) {
						pg_mw_sensors[ind_sensor].mw_mss_pos[i] = float_arguments[i + 1];
					}
					//printf("update position sensor #%d %.2f %.2f %.2f\n", ind_sensor, 
					//	pg_mw_sensors[ind_sensor].mw_mss_pos[0], pg_mw_sensors[ind_sensor].mw_mss_pos[1], pg_mw_sensors[ind_sensor].mw_mss_pos[2]);
					pg_mw_sensors[ind_sensor].mw_mss_pos_update = true;
				}
				else {
					sprintf(pg_errorStr, "unregistered sensor #%d (max %d)!", ind_sensor, PG_MW_NB_MAX_SENSORS); pg_ReportError(pg_errorStr);
				}
			}
		}
		break;
	default:
		sprintf(pg_errorStr, "Sensor command not found (%s)!", address_string.c_str()); pg_ReportError(pg_errorStr);
		break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// SENSOR CALLBACKS
//////////////////////////////////////////////////////////////////////////////////////////////////////

void sensor_layout_callBack(pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value) {
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_sensor_layout]) {
		// copies the grid layout
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_sensor_layout]) {
			pg_assignSensorPositions();
		}
	}
}
void sensor_activation_callBack(pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value) {
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_sensor_activation]) {
		// copies the grid layout
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_sensor_layout]) {
			pg_assignSensorActivations();
		}
	}
}
void sensor_sample_setUp_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_sensor_sample_setUp]) {
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_sensor_layout]) {
			pg_sensor_sample_setUp_interpolation();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// SENSOR FRAME PIXEL READING AND SAMPLE CHOICE
//////////////////////////////////////////////////////////////////////////////////////////////////////

//// sample choice

void pg_readSensors(void) {
	bool sensorOn[PG_NB_SENSORS] = { false };

	bool sampleOn[PG_NB_MAX_SENSOR_SAMPLE_SETUPS * PG_NB_SENSORS] = { false };
	int sampleToSensorPointer[PG_NB_MAX_SENSOR_SAMPLE_SETUPS * PG_NB_SENSORS] = { 0 };

	GLubyte pixelColor[3 * PG_NB_SENSORS] = { 0 };

	if (!pg_FullScenarioActiveVars[pg_ind_scenario][_sensor_layout]) {
		return;
	}

	// marks all the samples as unread
	for (int indSample = 0; indSample < PG_NB_MAX_SENSOR_SAMPLE_SETUPS * PG_NB_SENSORS; indSample++) {
		sampleOn[indSample] = false;
		sampleToSensorPointer[indSample] = -1;
	}

	glBindFramebuffer(GL_READ_FRAMEBUFFER, pg_FBO_Mixing_capturedFB_prec); // drawing memory on odd and even frames for echo and sensors	
	pg_bindFBOTextures(pg_FBO_Mixing_capturedFB_prec, pg_FBO_Mixing_capturedFB_prec_texID + (pg_FrameNo % 2), 1, false, 0);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	// sensor readback
	//printf("sensor on ");
	//printf("SENSOR position %d %d %d %d %d %d %d %d\n", (int)pg_sensorPositions[0], (int)pg_sensorPositions[1],
	//	(int)pg_sensorPositions[3], (int)pg_sensorPositions[4], (int)pg_sensorPositions[6], (int)pg_sensorPositions[7], (int)pg_sensorPositions[9], (int)pg_sensorPositions[10]);
	for (int indSens = 0; indSens < PG_NB_SENSORS; indSens++) {
		if (pg_sensor_onOff[indSens]) {
			//printf("position %d %d\n", (int)pg_sensorPositions[3 * indSens],
			//	(int)(pg_sensorPositions[3 * indSens + 1]));
			glReadPixels((int)pg_sensorPositions[3 * indSens],
				(int)(pg_sensorPositions[3 * indSens + 1]),
				1, 1,
				GL_RGB, GL_UNSIGNED_BYTE, pixelColor + 3 * indSens);
			pg_sensorLevel[indSens] =
				(pixelColor[3 * indSens] + pixelColor[3 * indSens + 1] + pixelColor[3 * indSens + 2]) / (255.f * 3.f);
			sensorOn[indSens] = (pg_sensorLevel[indSens] > 0.0f);
			if (sensorOn[indSens]) {
				sampleOn[pg_sample_choice[indSens]] = true;
				sampleToSensorPointer[pg_sample_choice[indSens]] = indSens;
				//printf("ON#%d ",indSens);
			}
		}
		else {
			pg_sensorLevel[indSens] = 0.0f;
			sensorOn[indSens] = false;
		}
	}
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	//printf("\n");
	pg_printOglError(70);

	///////////////////////////////////////////////////////////////
	// SENSOR-BASED SOUND IN RENOISE OR PORPHYROGRAPH SOUND
	// message value
	std::string float_string;
	std::string int_string;

	///////////////////////////////////////////////
	// TRIGGERING THE SAMPLE
	for (int indSens = 0; indSens < PG_NB_SENSORS; indSens++) {
		if (sensorOn[indSens] // active sensor
			&& pg_sensorLevel[indSens] > 0 // non black pixel
			&& pg_sample_play_start[pg_sample_choice[indSens]] < 0 // not currently playing
			) {

#if defined(PG_RENOISE)
			// Renoise message format && message posting
			sprintf(pg_AuxString, "/renoise/song/track/%d/prefx_volume %.2f", pg_sample_choice[indSens] + 1, pg_sensorLevel[indSens] * sensor_vol);
			pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_RN_send");

			//printf("RENOISE send %s %f %f\n", pg_AuxString, pg_sensorLevel[indSens], sensor_vol);
#endif

			// starts the clock for stopping the sample play after a certain time
			pg_sample_play_start[pg_sample_choice[indSens]] = pg_CurrentClockTime;
			pg_sample_play_volume[pg_sample_choice[indSens]] = pg_sensorLevel[indSens];
			// printf("lights sensor #%d\n", indSens);
		}

		// the sample has been triggered and has not yet reached 90% of its playing duration
		if (pg_sample_play_start[pg_sample_choice[indSens]] > 0.0
			&& pg_CurrentClockTime - pg_sample_play_start[pg_sample_choice[indSens]] <= 0.9 * sensor_beat_duration) {
			// set the value to the initial value until 0.9 so that there is one visual feedback per loop
			pg_sensorLevel[indSens] = (pg_sample_play_volume[pg_sample_choice[indSens]]
				* float((pg_CurrentClockTime - pg_sample_play_start[pg_sample_choice[indSens]]) / sensor_beat_duration));
		}
	}
	pg_printOglError(68);

	///////////////////////////////////////////////
	// MANAGING THE SAMPLE SEVEL
	for (int indSetup = 0; indSetup < PG_NB_MAX_SENSOR_SAMPLE_SETUPS; indSetup++) {
		for (int indSens = 0; indSens < PG_NB_SENSORS; indSens++) {
			int indSample = pg_sensor_sample_setUps[indSetup][indSens];
			if (pg_sample_play_start[indSample] > 0
				&& pg_CurrentClockTime - pg_sample_play_start[indSample] > sensor_beat_duration) {
#if defined(PG_RENOISE)
				// Renoise message format && message posting
				sprintf(pg_AuxString, "/renoise/song/track/%d/prefx_volume %.2f", indSample + 1, 0.f);
				pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_RN_send");
#endif

				// resets the clock for replaying the sample if sensor triggered again
				pg_sample_play_start[indSample] = -1.f;
			}
		}
	}
	pg_printOglError(69);

	// message trace
	//std::cout << "format: " << format << "\n";
	//std::cout << "msg: " << message << "\n";
}

#if defined(pg_Project_CAaudio)
class pg_greyNote {
public:
	float grey;
	int note;
};
void pg_playChord() {
	// line reading
	GLubyte* rowColor = new GLubyte[3 * 1024];
	pg_greyNote* rowGrey = new pg_greyNote[1024];
	// reads the 2nd horizontal line (from the top)
	glReadPixels(0, 766, 1024, 1,
		GL_RGB, GL_UNSIGNED_BYTE, rowColor);
	GLubyte* ptr = rowColor;
	std::string float_string;
	std::string int_string;
	std::string message;
	// message format
	int nbGrey = 0;
	//printf("Colors\n");
	for (int indPixel = 0; indPixel < 1024; indPixel++) {
		GLubyte r, g, b;
		r = *(ptr++);
		g = *(ptr++);
		b = *(ptr++);
		//if (indPixel < 100) {
		//	printf("(%d,%d,%d) ", int(r), int(g), int(b));
		//}
		float greyVal = (float(r) + float(g) + float(b)) / (255.f * 3.f);
		if (greyVal > 0) {
			rowGrey[nbGrey].note = indPixel;
			rowGrey[nbGrey].grey = greyVal;
			nbGrey++;
		}
	}
	//printf("\n");

	// no non null note
	if (nbGrey <= 0) {
		//printf("No note\n");
		delete rowColor;
		return;
	}

	// less than 10 notes play them all
	if (nbGrey < 10) {
		std::string format = "";
		for (int ind = 0; ind < 20; ind++) {
			format += "f";
		}
		//printf("Less than 10 notes\n");
		//for (int indGreyNote = 0; indGreyNote < nbGrey; indGreyNote++) {
		//	printf("%d %.2f / ", rowGrey[nbGrey].note, rowGrey[nbGrey].grey);
		//}
		//printf("\n");

		message = "/chord ";
		// non null notes
		for (int indGreyNote = 0; indGreyNote < nbGrey; indGreyNote++) {
			// MESSAGE CONSTRUCTION
			// the note is converted into a frequency
			int_string
				= std::to_string(static_cast<long double>(440 * pow(pow(2., 1. / 12.),
					(rowGrey[indGreyNote].note - 512) / 8)));
			message += int_string + " ";
			// the intensity is turned into decibels
			float_string = std::to_string(static_cast<long double>(rowGrey[indGreyNote].grey * 100.f));
			// float_str.resize(4);
			message += float_string;
			if (indGreyNote < 10 - 1) {
				message += float_string + " ";
			}
			else {
				message += float_string;
			}
		}
		// remaining nul notes to fill up to 10 notes in a chord
		for (int indGreyNote = nbGrey; indGreyNote < 10; indGreyNote++) {
			// MESSAGE CONSTRUCTION
			// the note is outside the scale
			int_string = std::to_string(-1);
			message += int_string + " ";
			// the intensity is null
			float_string = std::to_string(static_cast<long double>(0.f));
			// float_str.resize(4);
			if (indGreyNote < 10 - 1) {
				message += float_string + " ";
			}
			else {
				message += float_string;
			}
		}
		// message posting
		pg_send_message_udp((char*)format.c_str(), (char*)message.c_str(), (char*)"udp_PD_send");
	}
	else {
		message = "/chord ";
		// notes are grouped by package of same size into metaNotes with a metaIntensity
		// number minimal of notes per metanote
		int nbNotesMinPerMetaNote = nbGrey / 10; // integer part
		// some metaNotes are built from 1 additional note
		int nbMetaNotesWithNbNMPMPlus1 = (nbGrey * 10) % 10; // first decimal
		// rank of the non null note
		int indGreyNote = 0;
		// rank of the note inside the metaNote group
		int indNotesMinPerMetaNote = 0;
		// average note value
		float metaNote[10] = { 0.f };
		// average intensity values
		float metaGrey[10] = { 0.f };
		// builds the metaNotes from note groups
		for (int indMetaNote = 0; indMetaNote < 10; indMetaNote++) {
			// average note value
			metaNote[indMetaNote] = 0.f;
			// average intensity values
			metaGrey[indMetaNote] = 0.f;

			// METANOTE SIZE
			// number of notes building the metaNote
			int nbNotes
				= (indMetaNote < nbMetaNotesWithNbNMPMPlus1 ? nbNotesMinPerMetaNote + 1 : nbNotesMinPerMetaNote);
			// the last metaNote uses the remaining non null notes
			if (indMetaNote == 10 - 1) {
				nbNotes = nbGrey - indGreyNote;
			}

			// METANOTE computation
			// average note and intensity value
			for (int indLocalGreyNote = 0; indLocalGreyNote < nbNotes && indGreyNote < nbGrey; indLocalGreyNote++) {
				metaGrey[indMetaNote] += rowGrey[indGreyNote].grey;
				metaNote[indMetaNote] += rowGrey[indGreyNote].note * rowGrey[indGreyNote].grey;
				indGreyNote++;
			}

			// AVERAGE VALUES
			// the note is corrected by the sum of weights (intensities) to turn it into a barycenter
			if (metaGrey > 0) {
				metaNote[indMetaNote] /= metaGrey[indMetaNote];
			}
			else {
				metaNote[indMetaNote] = -1;
			}
			if (nbNotes > 0) {
				metaGrey[indMetaNote] /= nbNotes;
			}

			// MESSAGE CONSTRUCTION
			// the note is converted into a frequency
			int_string
				= std::to_string(static_cast<long double>(440 * pow(pow(2., 1. / 12.),
					(int(round(metaNote[indMetaNote])) - 512) / 8)));
			message += int_string + " ";
			// the intensity is turned into decibels
			float_string = std::to_string(static_cast<long double>(metaGrey[indMetaNote] * 100.f));
			// float_str.resize(4);
			if (indMetaNote < 10 - 1) {
				message += float_string + " ";
			}
			else {
				message += float_string;
			}
		}
		std::string format = "";

		format += "f";
	}
	// message posting
	pg_send_message_udp((char*)format.c_str(), (char*)message.c_str(), (char*)"udp_PD_send");
}
delete rowColor;
delete rowGrey;
}
#endif
