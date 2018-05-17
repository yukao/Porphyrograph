// RANK	0	1	2    3
// VERBATIM	audio in device	audio out device	audio device verbose output beat count
enum ConfigurationVarIDs{ 
  _AudioInputDevice = 0,
  _AudioOutputDevice,
  _VerboseDeviceOutput,
  _BeatsPerSequence,
  _BeatsPerMinute,
  _MaxConfigurationVarIDs};
extern int   AudioInputDevice    ;
extern int   AudioOutputDevice   ;
extern int   VerboseDeviceOutput;
extern int   BeatsPerSequence;
extern float   BeatsPerMinute;
enum VarTypes { _pg_bool = 0 , _pg_int , _pg_float , _pg_sign , _pg_track };
extern VarTypes ConfigurationVarTypes[_MaxConfigurationVarIDs];
extern void * ConfigurationVarPointers[_MaxConfigurationVarIDs];
