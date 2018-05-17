int   AudioInputDevice    ;
int   AudioOutputDevice   ;
int   VerboseDeviceOutput;
int   BeatsPerSequence;
float   BeatsPerMinute;
VarTypes ConfigurationVarTypes[_MaxConfigurationVarIDs] = {
    _pg_int,
    _pg_int,
	_pg_int,
	_pg_int,
	_pg_float,
};
void * ConfigurationVarPointers[_MaxConfigurationVarIDs] = { 
   (void *)&AudioInputDevice,
   (void *)&AudioOutputDevice,
   (void *)&VerboseDeviceOutput,
   (void *)&BeatsPerSequence,
   (void *)&BeatsPerMinute,
};
