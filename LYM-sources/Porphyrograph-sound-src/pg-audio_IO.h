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

#ifndef PG_AUDIO_IO_H
#define PG_AUDIO_IO_H

#define PA_SAMPLE_TYPE             paFloat32
#define FRAMES_PER_BUFFER          (40000)
#define PG_RECORDINGBUFFER_SIZE    (100000)

////////////////////////////////////////////////////////////////////
// AUDIO BUFFERS
////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////
// WORKING BUFFER FOR I/O
extern unsigned long pg_VSTeffect_BufferSize;
extern float *pg_VSTeffect_InputBuffer[PG_AUDIO_NB_CHANNELS];
extern float *pg_VSTeffect_OutputBuffer[PG_AUDIO_NB_CHANNELS];

extern unsigned long pg_InLineAndSamples_BufferSize;
extern float *pg_InLine_InputBuffer;
extern float *pg_Samples_InputBuffer;
extern float *pg_RhythmsBackTrack_InputBuffer;
extern float *pg_InLine_OutputBuffer;
extern float *pg_Samples_OutputBuffer;

// LAUNCHED PERFORMANCE
extern bool  pg_Launched_Performance;


////////////////////////////////////////////////////////////////////
/// THREADED OUTPUT WRITING
////////////////////////////////////////////////////////////////////
class RecordingDataStruct {
public:
	char pg_recordingfile_name[128];
	SNDFILE *pg_recordingfile;
	SF_INFO pg_recordingfileSndInfo;
	float * pg_doubleRecordingbuffer[2];
	RecordingDataStruct(void) {
		pg_doubleRecordingbuffer[0] = new float[PG_RECORDINGBUFFER_SIZE * PG_AUDIO_NB_CHANNELS];
		pg_doubleRecordingbuffer[1] = new float[PG_RECORDINGBUFFER_SIZE * PG_AUDIO_NB_CHANNELS];

		// Open sound file for writing
		time_t rawtime;
		struct tm * timeinfo;
		char time_string[80];
		time(&rawtime);
		timeinfo = localtime(&rawtime);
		strftime(time_string, 80, "%F_%H_%M", timeinfo);
		sprintf(pg_recordingfile_name, "audio_capture_%s.wav", time_string);

		pg_recordingfileSndInfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
		pg_recordingfileSndInfo.channels = PG_AUDIO_NB_CHANNELS;
		pg_recordingfileSndInfo.samplerate = PG_AUDIO_SAMPLE_RATE;

		pg_recordingfile = sf_open(pg_recordingfile_name, SFM_WRITE,&pg_recordingfileSndInfo);
		if (pg_recordingfile == NULL) {
			printf("Error opening sound file '%s': %s\n", pg_recordingfile_name, sf_strerror(pg_recordingfile)); throw 0;
		}
		else {
			printf("Recording in sound file '%s': %s\n", pg_recordingfile_name, sf_strerror(pg_recordingfile)); 
		}
	}
	~RecordingDataStruct(void) {
		printf("Closing recording sound file '%s': %s\n", pg_recordingfile_name, sf_strerror(pg_recordingfile));
		sf_close(pg_recordingfile);
		delete pg_doubleRecordingbuffer[0];
		delete pg_doubleRecordingbuffer[1];
	}
};

extern RecordingDataStruct *pg_recordingData;
extern int pg_active_recordingbuffer;
extern int pg_frame_recordingbuffer;

// parameters given by the streaming initialization
extern PaStreamParameters pg_audio_outputParameters;
extern PaStream *pg_audio_stream;

////////////////////////////////////////////////////////////////////
// LOOP BUFFER
////////////////////////////////////////////////////////////////////
#define PG_NB_LOOP_BUFFERS 3
extern AudioLoopBuffer *LoopBuffer[PG_NB_LOOP_BUFFERS];
extern float loop_master_level;

////////////////////////////////////////////////////////////////////
// INPUT LEVEL
////////////////////////////////////////////////////////////////////
extern float input_level;


#ifdef WIN32
DWORD WINAPI pg_threadedWriteRecordingOutput(LPVOID lpParam);
#else
void* pg_threadedWriteRecordingOutput(void * lpParam);
#endif

bool pg_VSTeffect_IO_processor(float *in, float *out, unsigned long framesPerBuffer,
	MVerb<float> *VST_reverb, mdaDelay *VST_delay , int indProcess );
bool pg_loop_processor(float *in, unsigned long framesPerBuffer);
bool pg_Sequences_and_Backtracks_Playback(float *out, unsigned long framesPerBuffer);
static int pg_audio_paStreamCallback(const void *inputBuffer, void *outputBuffer,
			     unsigned long framesPerBuffer,
			     const PaStreamCallbackTimeInfo* timeInfo,
			     PaStreamCallbackFlags statusFlags,
			     void *userData );
static void StreamFinished( void* userData );
bool Pa_portaudio_init_and_effects(void);
PaDeviceIndex pa_find_device_by_name(const char *name);
void printDevices_seeksASIODevices(int *indASIOinput, int *indASIOoutput,
	int *indDefaultInput, int *indDefaultOutput,
	bool verbose);
bool Pa_portaudio_init_and_effects(void);
 
#endif
