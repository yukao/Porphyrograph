//////////////////////////////////////////////////////////////////////
// SENSORS
//////////////////////////////////////////////////////////////////////

// sensor translations
// current sensor layout
extern float pg_sensorPositions[3 * PG_NB_SENSORS];
// all possible sensor layouts
extern float pg_sensorLayouts[3 * PG_NB_SENSORS * PG_NB_MAX_SENSOR_LAYOUTS];

// sensor on off
// current sensor activation pattern
extern bool pg_sensor_onOff[PG_NB_SENSORS];
extern double pg_sensor_last_activation_time;
// all sensor activation patterns
extern bool pg_sensorActivations[PG_NB_SENSORS * PG_NB_MAX_SENSOR_ACTIVATIONS];

// sample choice
// current sample choice
extern int pg_sample_choice[PG_NB_SENSORS];
// all possible sensor layouts
extern int pg_sensor_sample_setUps[PG_NB_MAX_SAMPLE_SETUPS][PG_NB_SENSORS];
// sample start playing time for muting after 1 cycle
extern double pg_sample_play_start[PG_NB_MAX_SAMPLE_SETUPS * PG_NB_SENSORS];

extern float pg_sensorLevel[PG_NB_SENSORS];
extern float pg_sample_play_volume[PG_NB_MAX_SAMPLE_SETUPS * PG_NB_SENSORS];

// current sensor
extern int pg_currentSensor;

extern int pg_Sensor_order[PG_NB_SENSORS];

void pg_SensorInitialization(void);
void pg_assignSensorPositions(void);
void pg_assignSensorActivations(void);
void pg_sensor_sample_setUp_interpolation(void);

void pg_MetawearSensorInitialization();
