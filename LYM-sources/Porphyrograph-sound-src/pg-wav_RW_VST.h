////////////////////////////////////////////////////////////////////
// 
// 
//     File pg_wav_RW.cpp
// 
//
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2.1
// of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this program; if not, write to the Free
// Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.
////////////////////////////////////////////////////////////////////

#ifndef PG_WAV_RW_H
#define PG_WAV_RW_H

/// initial time (in case of real timescale)
extern double InitialRealTime;

////////////////////////////////////////////////////////////////////
// VST DATA
////////////////////////////////////////////////////////////////////
extern AEffect* myPlugin;
extern MVerb<float> em_verb;
extern mdaDelay delay;

////////////////////////////////////////////////////////////////////
// PORTAUDIO STREAMING
////////////////////////////////////////////////////////////////////
#define PG_AUDIO_SAMPLE_RATE                (44100)
#define PG_AUDIO_NB_CHANNELS                (2)
#define PG_AUDIO_BYTES_PER_SAMPLE           (2)
#define PG_AUDIO_SAMPLE_FORMAT              (paInt16)

#define PG_AUDIO_NB_SAMPLES                 (15)

#define PG_AUDIO_MOIRE_SIZE                 (100000)

inline long toFrames(double time)
{
	return long(time * PG_AUDIO_SAMPLE_RATE);
}

///////////////////////////////////////////////////////
// AUDIO DATA STRUCTURE
///////////////////////////////////////////////////////

// data structure for threads to use.
// This is passed by void pointer so it can be any data type
// that can be passed using a single void pointer (LPVOID).
class AudioSampleDataStruct {
public:
	char fname[256];
	long sampleBufferFrameSize;
	bool loaded;
	SNDFILE *sampleSndFile;
	SF_INFO sampleSndInfo;
	float * sampleBuffer;
	AudioSampleDataStruct(void) {
		*fname = 0;
		sampleBufferFrameSize = 0;
		loaded = false;
		sampleSndFile = NULL;
		sampleSndInfo.frames = 0;
		// the full buffer of the sample
		sampleBuffer = NULL;
		// a kind of safety zone is preserved at the beginning of the sample
		// this zone contains spare bytes that can be used to blend the end
		// of a sample with its beginning
	}
	void release(void) {
		*fname = 0;
		sampleBufferFrameSize = 0;
		loaded = false;
		if (sampleSndFile) {
			sf_close(sampleSndFile);
			sampleSndFile = NULL;
		}
		sampleSndInfo.frames = 0;
		if (sampleBuffer) {
			delete[](sampleBuffer);
			sampleBuffer = NULL;
		}
	}
	~AudioSampleDataStruct(void) {
        // std::cout<<"AudioSampleDataStruct destructor"<<std::endl;
		if (sampleSndFile) {
			sf_close(sampleSndFile);
			sampleSndFile = NULL;
		}
		if (sampleBuffer) {
			delete[](sampleBuffer);
			sampleBuffer = NULL;
		}
	}
	int pg_loadSample(void);
};

class AudioBufferDataStruct {
public:
	// sample number for each audio track
	int pg_SampleIndex;
	// buffer swap management
	bool swapping;
	double swapStart;
	double swapDuration;
	// audio tracks offsets
	long pg_audio_macroTrackOffset;
	long pg_audio_microTrackOffset;
	AudioBufferDataStruct(void) {
		pg_SampleIndex = 0;
		swapping = false;
		swapStart = 0.0;
		swapDuration = 1.0;
		pg_audio_macroTrackOffset = 0;
		pg_audio_microTrackOffset = 0;
	}
	void reset(void) {
		pg_SampleIndex = 0;
		swapping = false;
		swapStart = 0.0;
		swapDuration = 1.0;
		pg_audio_macroTrackOffset = 0;
		pg_audio_microTrackOffset = 0;
	}
	~AudioBufferDataStruct(void) {
	}
};

////////////////////////////////////////////////////////////////////
// VST PLUG-INS DATA
////////////////////////////////////////////////////////////////////

// Plugin's entry point
typedef AEffect *(*vstPluginFuncPtr)(audioMasterCallback host);
// Plugin's dispatcher function
typedef VstIntPtr(*dispatcherFuncPtr)(AEffect *effect, VstInt32 opCode,
	VstInt32 index, VstInt32 value, void *ptr, float opt);
// Plugin's getParameter() method
typedef float(*getParameterFuncPtr)(AEffect *effect, VstInt32 index);
// Plugin's setParameter() method
typedef void(*setParameterFuncPtr)(AEffect *effect, VstInt32 index, float value);
// Plugin's processEvents() method
typedef VstInt32(*processEventsFuncPtr)(VstEvents *events);
// Plugin's process() method
typedef void(*processFuncPtr)(AEffect *effect, float **inputs,
	float **outputs, VstInt32 sampleFrames);

AEffect* loadVSTPlugin(char *vstPath);
bool configureVSTPlugin(AEffect *plugin, dispatcherFuncPtr *dispatcher);
bool canPluginDo(char *canDoString, AEffect *plugin, dispatcherFuncPtr dispatcher);
void resumePlugin(AEffect *plugin, dispatcherFuncPtr dispatcher);
void suspendPlugin(AEffect *plugin, dispatcherFuncPtr dispatcher);
void initVSTPlugin(AEffect *plugin, dispatcherFuncPtr dispatcher);

////////////////////////////////////////////////////////////////////
// AUDIO BUFFERS
////////////////////////////////////////////////////////////////////
// Structures pointing to sample audio data and used for playing or for swapping
#define PG_AUDIO_NB_BUFFERS                 (6)

////////////////////////////////////////////////////////////////////
// AUDIO SAMPLES
////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////
// sample index management for progessive sample turnover
// according to sample swap frequency and duration
// and according to directory size
extern std::string SampleDir;

///  READ SAVE float
bool pg_readSampleData(void * lpParam);

/// SAVE float
bool pg_audio_create_file(const char * pg_sampleName, SF_INFO *sndInfo, long frames,
	float *buffer);

// float BUFFER MANAGEMENT
int load_sample(std::string *fileName);

bool pg_audio_deallocation(void);
bool pg_audio_errorMsg(PaError err);
bool pg_SampleGeneration(string *largeFileName, string *sampleRhythmfileName, string *sampleDronefileName, 
  int startRhythmIndex, int startDroneIndex, int nbRhythms , int nbDrones );
void pg_smallSample_generation(int nbSubSamples, // number of subsamples
	// all the lengths and offsets are in frames (times * PG_AUDIO_SAMPLE_RATE * PG_AUDIO_NB_CHANNELS)
	long smallSampleDuration, long largeSampleDuration, // the target and the source buffer lengths
	float *smallSampleBuffer, float *largeSampleBuffer, // the target and the source buffers
	long *smallSampleOffset, long *largeSampleOffset, // smallSampleOffset is the position in target small sample
	// offset is the position in source big sample
	long *incay, long *decay,  // fadein from smallSampleOffset to smallSampleOffset + incay
	// fadeout from smallSampleOffset + targetduration - decay to smallSampleOffset + targetduration
	long *sourceduration, long *targetduration, // length of copy from big sample and lenght of copy into small sample
	// if targetduration > sourceduration the sample is repeated as much as needed to fill the
	// target duration
	float selfBlendingRatio);        // ratio of the sample used for overlap to preserve sound continuity in case of 
bool pg_VST_processor(string *inputFileName, string *outputFileName, AEffect* myPlugin);
void pg_aliasScript( char *command_symbol , 
		     char* string_argument_0 ,
		     float arguments[MAX_OSC_ARGUMENTS] );
#ifdef _WIN32
int gettimeofday(struct timeval* tp, void* tzp);
#endif
double RealTime(void);

#endif
