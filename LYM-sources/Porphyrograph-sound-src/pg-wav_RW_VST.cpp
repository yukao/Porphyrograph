////////////////////////////////////////////////////////////////////
// 
// 
// 
//     File pg_wav_RW.cpp
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

#include "pg-all_include.h"

HANDLE  hThread;

////////////////////////////////////////////////////////////////////
// VST DATA
////////////////////////////////////////////////////////////////////
AEffect* myPlugin = NULL;
MVerb<float> em_verb;
mdaDelay delay;

////////////////////////////////////////////////////////////////////
// VST PLUG-INS DATA
////////////////////////////////////////////////////////////////////
// C callbacks
//extern "C" {
//	// Main host callback
//	VstIntPtr VSTCALLBACK hostCallback(AEffect *effect, VstInt32 opcode,
//		VstInt32 index, VstInt32 value, void *ptr, float opt);
//}

extern "C" {
	VstIntPtr VSTCALLBACK hostCallback(AEffect *effect, VstInt32 opcode, VstInt32 index,
		VstInt32 value, void *ptr, float opt) {
		   //VSTEffect *vst = (effect ? (VSTEffect *) effect->user : NULL);
		switch (opcode) {
		case audioMasterVersion:
			return 2400;
		case audioMasterIdle:
			effect->dispatcher(effect, effEditIdle, 0, 0, 0, 0);
			// Handle other opcodes here... there will be lots of them
		case audioMasterGetTime :
			static VstTimeInfo time;
			memset(&time,0,sizeof(time));
			// flags
			//time.flags = kVstTempoValid|kVstBarsValid|kVstCyclePosValid|kVstPpqPosValid|kVstSmpteValid|kVstTimeSigValid;            
			//time.sampleRate = th->samplerate;
			//time.samplePos = th->samplepos;
			//time.ppqPos = th->ppqpos;

			//time.tempo = th->tempo;
			//time.barStartPos = th->barstartpos;
			//time.cycleStartPos = th->cyclestartpos;
			//time.cycleEndPos = th->cycleendpos;

			//time.timeSigNumerator = th->timesignom;
			//time.timeSigDenominator = th->timesigden;

			//// SMPTE data
			//time.smpteOffset = th->smpteoffset;
			//time.smpteFrameRate = th->smpterate;

			//// time.samplesToNextClock = 0;

			//if(value&kVstNanosValid) {
			//	time.nanoSeconds = flext::GetOSTime()*1.e9;
			//	time.flags |= kVstNanosValid;
			//}

			return (long)&time;
			// Handle other opcodes here... there will be lots of them
		default:
			printf("Plugin requested value of opcode %d\n", opcode);
			return NULL;
			break;
		}
	}
}

////////////////////////////////////////////////////////////////////
// REAL TIME
////////////////////////////////////////////////////////////////////
#ifdef _WIN32
static const double epoch = 116444736000000000.;
int gettimeofday(struct timeval* tp, void* tzp) {
	SYSTEMTIME system_time;
	FILETIME file_time;
	ULARGE_INTEGER ularge;

	GetSystemTime(&system_time);
	SystemTimeToFileTime(&system_time, &file_time);
	ularge.LowPart = file_time.dwLowDateTime;
	ularge.HighPart = file_time.dwHighDateTime;

	tp->tv_sec = (long)((ularge.QuadPart - epoch) / 10000000L);
	tp->tv_usec = (long)(system_time.wMilliseconds * 1000);
	/* 0 indicates that the call succeeded. */
	return 0;
}
#endif

double RealTime(void) {
	struct timeval time;
	gettimeofday(&time, 0);
	double realtime = (double)time.tv_sec + ((double)time.tv_usec / 1000000.);
	return realtime;
}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
// SOUND FILE WRITING (WAV)
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
bool pg_audio_create_file(const char * pg_sampleName, SF_INFO *sndInfo, long frames,
	float *buffer) {
	// Open sound file for writing
	SNDFILE *sndFile = sf_open(pg_sampleName, SFM_WRITE, sndInfo);
	if (sndFile == NULL) {
		printf("Error opening sound file '%s': %s\n", pg_sampleName, sf_strerror(sndFile));
		return false;
	}

	// Write frames
	long writtenFrames = (long)sf_writef_float(sndFile, buffer, frames);
	sndInfo->frames = frames;

	// Check correct number of frames saved
	if (writtenFrames != frames) {
		printf("Did not write enough frames for source\n");
		sf_close(sndFile);
		return false;
	}

	// Output Info
	// printf("Write %ld frames from %s, Sample rate: %d, Length: %fs, channels: %d\n",
	// 	(long int)frames, pg_sampleName, (int)sndInfo->samplerate, (float)frames / sndInfo->samplerate, sndInfo->channels);
	printf("Writing %s\n", pg_sampleName );

	// Tidy up
	sf_write_sync(sndFile);
	sf_close(sndFile);

	return true;
} /* pg_audio_create_file */

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
// LOAD float SOUND FILE (WAV 16 OR 32 BITS)
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
bool pg_readSampleData(void * lpParam) {
	AudioSampleDataStruct * pDataArray = (AudioSampleDataStruct *)lpParam;

	long numFrames = 0L;
	if (pDataArray->sampleSndInfo.format == (SF_FORMAT_WAV | SF_FORMAT_PCM_16)) {
		printf("Loading 16 bits wav file [%s]\n", pDataArray->fname);
			// Load data
		numFrames // = pDataArray->sampleBufferFrameSize;
			= (long)sf_readf_float(pDataArray->sampleSndFile,
			pDataArray->sampleBuffer,
			pDataArray->sampleBufferFrameSize);
	}
	else if (pDataArray->sampleSndInfo.format == (SF_FORMAT_WAV | SF_FORMAT_PCM_32)) {
		printf("Loading 32 bits wav file [%s]\n", pDataArray->fname);
		// Load data
		numFrames // = pDataArray->sampleBufferFrameSize;
			= (long)sf_readf_float(pDataArray->sampleSndFile,
			pDataArray->sampleBuffer,
			pDataArray->sampleBufferFrameSize);
	}
	// the format short == PG_AUDIO_SAMPLE_FORMAT (paInt16) is checked during audio initialization
	sf_close(pDataArray->sampleSndFile);
	pDataArray->sampleSndFile = NULL;

	char * ptr = strrchr(pDataArray->fname, '/');
	// printf( "-> RAM sample [%s] size %ld\n" , (ptr? ptr + 1: pDataArray->fname) ,
	// 	numFrames );

	// Check correct number of samples loaded
	if (numFrames != pDataArray->sampleBufferFrameSize) {
		printf("Did not read enough frames for source [%s] (%ld/%ld)\n",
			(ptr ? ptr + 1 : pDataArray->fname), numFrames, pDataArray->sampleBufferFrameSize);
		pDataArray->release();
#ifndef WIN32
		pthread_exit(NULL);
#endif
		return false;
	}

	pDataArray->loaded = true;

	// delete pDataArray;
	return true;
}

/////////////////////////////////////////////
// sample loading
int AudioSampleDataStruct::pg_loadSample(void) {
	int formatSize = 0;
	// Open sound file
	// printf("reading file '%s'\n", pg_sampleName);
	sampleSndFile = sf_open(fname, SFM_READ, &sampleSndInfo);
	if (sampleSndFile == NULL) {
		printf("Error reading source file '%s'\n", fname);
		release();
		return 0;
	}

	// Check format - 16bit PCM
	if (sampleSndInfo.format == (SF_FORMAT_WAV | SF_FORMAT_PCM_16)) {
		formatSize = SF_FORMAT_PCM_16;
	}
	else if (sampleSndInfo.format == (SF_FORMAT_WAV | SF_FORMAT_PCM_32)) {
		formatSize = SF_FORMAT_PCM_32;
	}
	else {
		printf("Input should be 16bit/32bit Wav: %s\n", fname);
		release();
		return 0;
	}

	// Check channels
	if (sampleSndInfo.channels != PG_AUDIO_NB_CHANNELS) {
		printf("Input should be %d channels: %s\n",
			PG_AUDIO_NB_CHANNELS, fname);
		release();
		return 0;
	}

	// Check sample rate
	if (sampleSndInfo.samplerate != PG_AUDIO_SAMPLE_RATE) {
		printf("Input should be %d sample rate: %s\n",
			PG_AUDIO_SAMPLE_RATE, fname);
		release();
		return 0;
	}

	// Allocate memory
	sampleBufferFrameSize = (long)sampleSndInfo.frames;
	if ((formatSize == SF_FORMAT_PCM_16) || (formatSize == SF_FORMAT_PCM_32)) {
		sampleBuffer = new float[(unsigned long)(sampleBufferFrameSize * PG_AUDIO_NB_CHANNELS)];
		if (sampleBuffer == NULL) {
			printf("Could not allocate memory for data\n");
			release();
			return 0;
		}
	}

	pg_readSampleData((void *)this);

	return formatSize;

} /* pg_loadSample */

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
// float BUFFER AND FILE INFO INITIALIZATIONS
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////


bool pg_audio_errorMsg(PaError err) {
	Pa_Terminate();
	printf("An error occured while using the portaudio stream\n");
	printf("Error number: %d\n", err);
	printf("Error message: %s\n", Pa_GetErrorText(err));
	return false;
}


/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
// float GENERATION FROM LONG AUDIO FILE
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
// everything is expressed in secs
void pg_smallSample_generation(int nbSubSamples, // number of subsamples
	// all the lengths and offsets are in frames (times * PG_AUDIO_SAMPLE_RATE)
	long smallSampleDuration, long largeSampleDuration, // the target and the source buffer lengths
	float *smallSampleBuffer, float *largeSampleBuffer, // the target and the source buffers
	long *smallSampleOffset, long *largeSampleOffset, // smallSampleOffset is the position in target small sample
	// offset is the position in source big sample
	long *incay, long *decay,  // fadein from smallSampleOffset to smallSampleOffset + incay
	// fadeout from smallSampleOffset + targetduration - decay to smallSampleOffset + targetduration
	long *sourceduration, long *targetduration, // length of copy from big sample and lenght of copy into small sample
	// if targetduration > sourceduration the sample is repeated as much as needed to fill the
	// target duration
	float selfBlendingRatio) {        // ratio of the sample used for overlap to preserve sound continuity in case of 

	// source sample smaller than the target one (leading possibly to granular synthesis)
	for (int indSmallSubSample = 0; indSmallSubSample < nbSubSamples; indSmallSubSample++) {
		// data copy with possible loop on the copied source sample to fill the target sample
		long cumulatedDuration = 0L;
		long targetOffset = smallSampleOffset[indSmallSubSample];
		long sourceOffset = largeSampleOffset[indSmallSubSample]; 	// off set from beginning

		// should not go beyond the limits of the large sample
		long maxSourceDuration = min(sourceduration[indSmallSubSample], largeSampleDuration - largeSampleOffset[indSmallSubSample]);
		if (maxSourceDuration <= 0) {
			continue;
		}
		// should not go beyond the limits of the small sample
		long maxTargetDuration = min(targetduration[indSmallSubSample], smallSampleDuration - smallSampleOffset[indSmallSubSample]);
		if (maxTargetDuration <= 0) {
			continue;
		}

		// selfBlending cannot be larger than 1.0
		selfBlendingRatio = min(selfBlendingRatio, 1.0f);

		// data copy with possible loop on the copied source sample to fill the target sample
		long indLoop = 0;
		while (cumulatedDuration < maxTargetDuration) {
			// self blending in case of multiple copies
			long selfBlendingRatioFrames = long(selfBlendingRatio * maxSourceDuration);
			// should not copy more than maxTargetDuration in the small sample
			// should not copy more than maxSourceDuration at once
			long copyDuration;
			// from the second loop the blending zone is not copied
			// the copy is made from the beginning of the sample + the size of the bleding zone
			if (indLoop == 0) {
				copyDuration = min(maxTargetDuration - cumulatedDuration, maxSourceDuration);
			}
			else {
				copyDuration = min(maxTargetDuration - cumulatedDuration, maxSourceDuration - selfBlendingRatioFrames);
			}
			if (copyDuration <= 0) {
				break;
			}

			// the full sample is copied
			if (indLoop == 0) {
				memcpy((uint8_t *)(smallSampleBuffer + targetOffset * PG_AUDIO_NB_CHANNELS),
					(void *)(largeSampleBuffer + sourceOffset * PG_AUDIO_NB_CHANNELS),
					copyDuration * PG_AUDIO_NB_CHANNELS * sizeof(float));
			}
			// skips the blended part of the beginning
			else {
				memcpy((uint8_t *)(smallSampleBuffer + targetOffset * PG_AUDIO_NB_CHANNELS),
					(void *)(largeSampleBuffer + (sourceOffset + selfBlendingRatioFrames) * PG_AUDIO_NB_CHANNELS),
					copyDuration * PG_AUDIO_NB_CHANNELS * sizeof(float));
			}

			// blending at the beginning of the copy (over the end of the preceding one)
			if (indLoop > 0 && selfBlendingRatioFrames > 0) {
				long indBegin = targetOffset - selfBlendingRatioFrames;
				for (long indSample = indBegin; indSample < targetOffset; indSample++) {
					double ratio = (double)(indSample - indBegin) / selfBlendingRatioFrames;
					for (int indCh = 0; indCh < 2; indCh++) {
						smallSampleBuffer[indSample * PG_AUDIO_NB_CHANNELS + indCh]
							= float(largeSampleBuffer[(sourceOffset + (indSample - indBegin)) * PG_AUDIO_NB_CHANNELS + indCh] * ratio // the beginning of the newly copied sample
							+ largeSampleBuffer[(sourceOffset + (maxSourceDuration - selfBlendingRatioFrames + (indSample - indBegin))) * PG_AUDIO_NB_CHANNELS + indCh] * (1.0 - ratio)); // the end of the previously copied sample
					}
				}
			}

			// offsets update according to copy
			// sample made slightly shorter so that self bleding can be made in case of multiple copies
			cumulatedDuration += copyDuration;
			targetOffset = smallSampleOffset[indSmallSubSample] + cumulatedDuration;

			// loop counter to manage self-blending in case of multiple copies
			indLoop++;
		}

		// reinitializes the target offset
		targetOffset = smallSampleOffset[indSmallSubSample];

		// fade in management
		long actualincay = min(incay[indSmallSubSample], maxTargetDuration);
		for (long indSample = 0; indSample < actualincay; indSample++) {
			double ratio = (double)indSample / actualincay;
			double weight = sin(ratio * M_PI / 2.0);
			for (int indCh = 0; indCh < 2; indCh++) {
				smallSampleBuffer[(targetOffset + indSample) * PG_AUDIO_NB_CHANNELS + indCh]
					= float(smallSampleBuffer[(targetOffset + indSample) * PG_AUDIO_NB_CHANNELS + indCh] * weight);
			}
		}
		// fade out management
		long actualdecay = min(decay[indSmallSubSample], maxTargetDuration);
		long decayStart = maxTargetDuration - actualdecay;
		for (long indSample = decayStart; indSample < maxTargetDuration; indSample++) {
			double ratio = (double)indSample / (maxTargetDuration - decayStart);
			double weight = sin((1.0 - ratio) * M_PI / 2.0);
			for (int indCh = 0; indCh < 2; indCh++) {
				smallSampleBuffer[(targetOffset + indSample) * PG_AUDIO_NB_CHANNELS + indCh]
					= float(smallSampleBuffer[(targetOffset + indSample) * PG_AUDIO_NB_CHANNELS + indCh] * weight);
			}
		}
	}
}


#define   PG_NB_MAX_SMALLSAMPLE_SUBSAMPLES   16
bool pg_SampleGeneration(string *largeFileName, string *sampleRhythmfileName, string *sampleDronefileName,
	int startRhythmIndex, int startDroneIndex, int nbRhythms, int nbDrones) {
	bool valret = true;

	AudioSampleDataStruct *LargeSampleBufferData = new AudioSampleDataStruct();

	char fnameSmallSample[256];
	SF_INFO smallSample_sndInfo;
	smallSample_sndInfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
	smallSample_sndInfo.channels = PG_AUDIO_NB_CHANNELS;
	smallSample_sndInfo.samplerate = PG_AUDIO_SAMPLE_RATE;

	////////////////////////////////////////////////////////////////
	// LOADS BIG float
	strcpy(LargeSampleBufferData->fname, largeFileName->c_str());
	int formatSize;
	if (!(formatSize = LargeSampleBufferData->pg_loadSample())) {
		printf("File load failure.\n");
		valret = false;
	}
	if (formatSize != SF_FORMAT_PCM_16) {
		printf("File should be first converted into 16-bits format before sample generation.\n");
		return valret;
	}

	double LargeSampleDuration = LargeSampleBufferData->sampleSndInfo.frames / (double)PG_AUDIO_SAMPLE_RATE;

	// allocates the writing buffer
	double smallSampleDuration = 4.0;
	long smallSampleDurationFr = toFrames(smallSampleDuration);
	float *smallSampleBuffer = new float[(unsigned long)(smallSampleDurationFr * PG_AUDIO_NB_CHANNELS)];
	/////////////////////////////////////////////////////
	// 16 rhythmic samples from voice
	for (int indRhythmSample = startRhythmIndex; indRhythmSample < startRhythmIndex + nbRhythms; indRhythmSample++) {
		int nbSubSamples = 4;
		double beat = smallSampleDuration / 16.0;
		double smallSampleOffset[4] = { 0.0, 4.0*beat, 8.0*beat, 12.0*beat };
		double offSetBasis = LargeSampleDuration / 2.0;
		double offSetVariation = LargeSampleDuration / 4.0;
		double largeSampleOffset[16];
		for (int indSubSample = 0; indSubSample < nbSubSamples; indSubSample++) {
			largeSampleOffset[indSubSample] = ((double)rand() / (RAND_MAX))*offSetBasis + ((double)rand() / (RAND_MAX))*offSetVariation;
		}
		double sourceduration[16] = { 4 * beat, 4 * beat, 4 * beat, 4 * beat };
		double targetduration[16] = { 4 * beat, 4 * beat, 4 * beat, 4 * beat };
		double incay[16] = { beat, beat, beat, beat };
		double decay[16] = { beat, beat, beat, beat };
		float selfBlendingRatio = 0.2f;

		long smallSampleOffsetFr[16];
		long largeSampleOffsetFr[16];
		long sourcedurationFr[16];
		long targetdurationFr[16];
		long incayFr[16];
		long decayFr[16];
		for (int indSubSample = 0; indSubSample < nbSubSamples; indSubSample++) {
			smallSampleOffsetFr[indSubSample] = toFrames(smallSampleOffset[indSubSample]);
			largeSampleOffsetFr[indSubSample] = toFrames(largeSampleOffset[indSubSample]);
			sourcedurationFr[indSubSample] = toFrames(sourceduration[indSubSample]);
			targetdurationFr[indSubSample] = toFrames(targetduration[indSubSample]);
			incayFr[indSubSample] = toFrames(incay[indSubSample]);
			decayFr[indSubSample] = toFrames(decay[indSubSample]);
		}
		pg_smallSample_generation(nbSubSamples, // number of subsamples
			// all the lengths and offsets are in frames (times * PG_AUDIO_SAMPLE_RATE * PG_AUDIO_NB_CHANNELS)
			smallSampleDurationFr, LargeSampleBufferData->sampleBufferFrameSize, // the target and the source buffer lengths
			smallSampleBuffer, LargeSampleBufferData->sampleBuffer, // the target and the source buffers
			smallSampleOffsetFr, largeSampleOffsetFr, // smallSampleOffset is the position in target small sample
			// offset is the position in source big sample
			incayFr, decayFr,  // fadein from smallSampleOffset to smallSampleOffset + incay
			// fadeout from smallSampleOffset + targetduration - decay to smallSampleOffset + targetduration
			sourcedurationFr, targetdurationFr, // length of copy from big sample and lenght of copy into small sample
			// if targetduration > sourceduration the sample is repeated as much as needed to fill the
			// target duration
			selfBlendingRatio);        // ratio of the sample used for overlap to preserve sound continuity in case of 


		////////////////////////////////////////////////////////////////
		// SAVES SMALL float
		sprintf(fnameSmallSample, "%s%03d.wav", sampleRhythmfileName->c_str(), indRhythmSample);
		smallSample_sndInfo.frames = smallSampleDurationFr;
		// printf("small sample writing begin %s\n",fnameSmallSample);
		pg_audio_create_file(fnameSmallSample, &smallSample_sndInfo,
			smallSampleDurationFr, smallSampleBuffer);
		// printf("small sample writing end\n");
	}

	/////////////////////////////////////////////////////
	// 16 drone samples from voice
	for (int indDroneSample = startDroneIndex; indDroneSample < startDroneIndex + nbDrones; indDroneSample++) {
		int nbSubSamples = 1;
		double smallSampleOffset[1] = { 0.0 };
		double offSetBasis = LargeSampleDuration / 2.0;
		double offSetVariation = LargeSampleDuration / 4.0;
		double largeSampleOffset[1] = { (((double)rand() / (RAND_MAX))*offSetBasis + ((double)rand() / (RAND_MAX))*offSetVariation) };
		double sourceduration[1] = { 0.03 };
		double targetduration[1] = { smallSampleDuration };
		double incay[1] = { 0.0 };
		double decay[1] = { 0.0 };
		float selfBlendingRatio = 0.2f;

		long smallSampleOffsetFr[1];
		long largeSampleOffsetFr[1];
		long sourcedurationFr[1];
		long targetdurationFr[1];
		long incayFr[1];
		long decayFr[1];
		for (int indSubSample = 0; indSubSample < 1; indSubSample++) {
			smallSampleOffsetFr[indSubSample] = toFrames(smallSampleOffset[indSubSample]);
			largeSampleOffsetFr[indSubSample] = toFrames(largeSampleOffset[indSubSample]);
			sourcedurationFr[indSubSample] = toFrames(sourceduration[indSubSample]);
			targetdurationFr[indSubSample] = toFrames(targetduration[indSubSample]);
			incayFr[indSubSample] = toFrames(incay[indSubSample]);
			decayFr[indSubSample] = toFrames(decay[indSubSample]);
		}
		pg_smallSample_generation(nbSubSamples, // number of subsamples
			// all the lengths and offsets are in frames (times * PG_AUDIO_SAMPLE_RATE * PG_AUDIO_NB_CHANNELS)
			smallSampleDurationFr, LargeSampleBufferData->sampleBufferFrameSize, // the target and the source buffer lengths
			smallSampleBuffer, LargeSampleBufferData->sampleBuffer, // the target and the source buffers
			smallSampleOffsetFr, largeSampleOffsetFr, // smallSampleOffset is the position in target small sample
			// offset is the position in source big sample
			incayFr, decayFr,  // fadein from smallSampleOffset to smallSampleOffset + incay
			// fadeout from smallSampleOffset + targetduration - decay to smallSampleOffset + targetduration
			sourcedurationFr, targetdurationFr, // length of copy from big sample and lenght of copy into small sample
			// if targetduration > sourceduration the sample is repeated as much as needed to fill the
			// target duration
			selfBlendingRatio);        // ratio of the sample used for overlap to preserve sound continuity in case of 


		////////////////////////////////////////////////////////////////
		// SAVES SMALL float
		sprintf(fnameSmallSample, "%s%03d.wav", sampleDronefileName->c_str(), indDroneSample);
		smallSample_sndInfo.frames = smallSampleDurationFr;
		// printf("small sample writing begin\n");
		pg_audio_create_file(fnameSmallSample, &smallSample_sndInfo,
			smallSampleDurationFr, smallSampleBuffer);
		// printf("small sample writing end\n");
	}

	// printf("buffer deletion\n");
	delete smallSampleBuffer;
	delete LargeSampleBufferData;
	return valret;
}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
// PLUG-IN UPLOAD
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
// https://gist.github.com/nikreiman/736032
// http://teragonaudio.com/article/How-to-make-your-own-VST-host.html

AEffect* loadVSTPlugin( char * vstPath ) {
	AEffect *plugin = NULL;

	// loads library with ascii path
	HINSTANCE modulePtr = LoadLibraryA(vstPath);
	if (modulePtr == NULL) {
		printf("Failed trying to load VST from '%s', error %d\n",
			vstPath, GetLastError());
		return NULL;
	}

	vstPluginFuncPtr mainEntryPoint =
		(vstPluginFuncPtr)GetProcAddress(modulePtr, "VSTPluginMain");
	// Instantiate the plugin
	plugin = mainEntryPoint(&hostCallback);
	if (plugin != NULL) {
		printf("Loaded VST plugin [%s]\n", vstPath);
	}
	return plugin;
}

// setting up plugin callback
bool configureVSTPlugin(AEffect *plugin, dispatcherFuncPtr *dispatcher) {
	// Check plugin's magic number
	// If incorrect, then the file either was not loaded properly, is not a real
	// VST plugin, or is otherwise corrupt.
	if (plugin->magic != kEffectMagic) {
		printf("Plugin's magic number is bad\n");
		return false;
	}

	// Create dispatcher handle
	*dispatcher = (dispatcherFuncPtr)(plugin->dispatcher);

	// Set up plugin callback functions
	plugin->getParameter = (getParameterFuncPtr)plugin->getParameter;
	plugin->processReplacing = (processFuncPtr)plugin->processReplacing;
	plugin->setParameter = (setParameterFuncPtr)plugin->setParameter;

	return true;
}

// suspend/resume plugin methods
void resumePlugin(AEffect *plugin, dispatcherFuncPtr dispatcher) {
	dispatcher(plugin, effMainsChanged, 0, 1, NULL, 0.0f);
}

void suspendPlugin(AEffect *plugin, dispatcherFuncPtr dispatcher) {
	dispatcher(plugin, effMainsChanged, 0, 0, NULL, 0.0f);
}

// what a plugin can do
bool canPluginDo(char *canDoString, AEffect *plugin, dispatcherFuncPtr dispatcher) {
	return (dispatcher(plugin, effCanDo, 0, 0, (void*)canDoString, 0.0f) > 0);
}

// initializing a plugin
void initVSTPlugin(AEffect *plugin, dispatcherFuncPtr dispatcher) {
	dispatcher(plugin, effOpen, 0, 0, NULL, 0.0f);

	// Set some default properties
	float sampleRate = 44100.0f;
	dispatcher(plugin, effSetSampleRate, 0, 0, NULL, sampleRate);
	int blocksize = 512;
	dispatcher(plugin, effSetBlockSize, 0, blocksize, NULL, 0.0f);

	resumePlugin(plugin, dispatcher);
}

bool pg_VST_processor(string *inputFileName, string *outputFileName, AEffect* myPlugin) {
	bool valret = true;

	AudioSampleDataStruct *InputSampleBufferData = new AudioSampleDataStruct();

	SF_INFO outputSample_sndInfo;
	outputSample_sndInfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
	outputSample_sndInfo.channels = PG_AUDIO_NB_CHANNELS;
	outputSample_sndInfo.samplerate = PG_AUDIO_SAMPLE_RATE;

	////////////////////////////////////////////////////////////////
	// LOADS BIG float
	strcpy(InputSampleBufferData->fname, inputFileName->c_str());
	int formatSize;
	if (!(formatSize = InputSampleBufferData->pg_loadSample())) {
		printf("File load failure.\n");
		valret = false;
	}
	if (formatSize != SF_FORMAT_PCM_16) {
		printf("File should be first converted into 16-bits format before sample generation.\n");
		return valret;
	}

	/////////////////////////////////////////////////////
	// allocates the writing buffer
	outputSample_sndInfo.frames = InputSampleBufferData->sampleSndInfo.frames;
	float *double_inputSampleBuffer[2] = { NULL, NULL };
	double_inputSampleBuffer[0] = new float[(unsigned long)(outputSample_sndInfo.frames)];
	double_inputSampleBuffer[1] = new float[(unsigned long)(outputSample_sndInfo.frames)];
	float *double_outputSampleBuffer[2] = { NULL, NULL };
	double_outputSampleBuffer[0] = new float[(unsigned long)(outputSample_sndInfo.frames)];
	double_outputSampleBuffer[1] = new float[(unsigned long)(outputSample_sndInfo.frames)];
	float *outputSampleBuffer = new float[(unsigned long)(outputSample_sndInfo.frames * PG_AUDIO_NB_CHANNELS)];

	/////////////////////////////////////////////////////
	// buffer copy / VST process / copy back
	for (long indSample = 0; indSample < outputSample_sndInfo.frames; indSample++) {
		for (int indCh = 0; indCh < PG_AUDIO_NB_CHANNELS; indCh++) {
			double_inputSampleBuffer[indCh][indSample]
				= InputSampleBufferData->sampleBuffer[indSample * PG_AUDIO_NB_CHANNELS + indCh];
		}
	}

	//myPlugin->processReplacing(myPlugin, double_inputSampleBuffer, double_outputSampleBuffer, 
	//							VstInt32(outputSample_sndInfo.frames));
    em_verb.processReplacing(double_inputSampleBuffer, double_outputSampleBuffer, int(outputSample_sndInfo.frames));
    delay.processReplacing(double_inputSampleBuffer, double_outputSampleBuffer, int(outputSample_sndInfo.frames));

	for (long indSample = 0; indSample < outputSample_sndInfo.frames; indSample++) {
		for (int indCh = 0; indCh < PG_AUDIO_NB_CHANNELS; indCh++) {
			outputSampleBuffer[indSample * PG_AUDIO_NB_CHANNELS + indCh]
				= double_outputSampleBuffer[indCh][indSample];
		}
	}

	////////////////////////////////////////////////////////////////
	// SAVES OUTPUT float
	// printf("small sample writing begin %s\n",fnameSmallSample);
	pg_audio_create_file(outputFileName->c_str(), &outputSample_sndInfo,
		long(outputSample_sndInfo.frames), outputSampleBuffer);
	// printf("small sample writing end\n");
	// printf("buffer deletion\n");

	delete double_inputSampleBuffer[0];
	delete double_inputSampleBuffer[1];
	delete double_outputSampleBuffer[0];
	delete double_outputSampleBuffer[1];
	delete outputSampleBuffer;
	delete InputSampleBufferData;
	return valret;
}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
// UTILS
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

/*******************************************************************/
bool pg_audio_deallocation(void) {

	bool valRet = true;

	PaError err = Pa_CloseStream(pg_audio_stream);
	if (err != paNoError) {
		printf("Error when closing audio stream\n");
		pg_audio_errorMsg(err);
		valRet = false;
	}

	Pa_Terminate();
	printf("audio deallocated.\n");
	return valRet;
}


// FUNCTIONS FOR TESTING SUBDIR FILES (MADE FOR SAMPLES AND IMAGES)

bool is_substring_index(std::string str, int ind) {
	char string_number[16];
	sprintf(string_number, "%03d", ind);
	std::string string_number_s(string_number);

	if (str.find(string_number_s) != std::string::npos) {
		// std::cout << "*** FOUND: " + filepath << std::endl;
		return true;
	}
	return false;
}

bool is_substring_index(char * charstr, int ind) {
	std::string str(charstr);

	char string_number[16];
	sprintf(string_number, "%03d", ind);
	std::string string_number_s(string_number);

	if (str.find(string_number_s) != std::string::npos) {
		// std::cout << "*** FOUND: " + filepath << std::endl;
		return true;
	}
	return false;
}

bool is_subdir_index(struct dirent *dirp, std::string *dirpath, int inddir) {
	string filepath = *dirpath + "/" + dirp->d_name;
	struct stat filestat;

	// If the file is a directory and the name contains the integer 
	if (stat(filepath.c_str(), &filestat)) return false; // colleccts file status and returns 0 on success
	if (S_ISDIR(filestat.st_mode) && is_substring_index(dirp->d_name, inddir)) { // the file is a directory 
		// and contains the integer substring
		return true;
	}
	return false;
}

bool is_subfile_index(struct dirent *dirp, std::string *dirpath, int indfile) {
	string filepath = *dirpath + "/" + dirp->d_name;
	struct stat filestat;

	// If the file is a directory (or is in some way invalid) we'll skip it 
	if (stat(filepath.c_str(), &filestat)) return false; // colleccts file status and returns 0 on success
	if (S_ISDIR(filestat.st_mode)) { // the file is a directory 
		// and contains the integer substring
		return false;
	}
	ifstream fin;
	fin.open(filepath.c_str());
	if (fin) {
		if (is_substring_index(dirp->d_name, indfile)) {
			fin.close();
			return true;
		}
		fin.close();
	}
	return false;
}
string * is_subdir_subfile_index(std::string *dirpath, int inddir, int indfile) {
	DIR *dp = opendir(dirpath->c_str());
	if (dp == NULL)
	{
		std::cout << "Error(" << errno << ") opening " << *dirpath << std::endl;
		return NULL;
	}
	// std::cout << std::endl << "dir to get files of: " + dirpath << std::endl;
	struct dirent *dirp;
	while ((dirp = readdir(dp))) {
		if (is_subdir_index(dirp, dirpath, inddir)) {
			string subdirpath(*dirpath + "/" + dirp->d_name);
			DIR *subdp = opendir(subdirpath.c_str());
			if (subdp == NULL)
			{
				std::cout << "Error(" << errno << ") opening " << subdirpath << std::endl;
				return NULL;
			}
			struct dirent *subdirp;
			while ((subdirp = readdir(subdp))) {
				if (is_subfile_index(subdirp, &subdirpath, indfile)) {
					string * subdirfilepath = new string(subdirpath + "/" + subdirp->d_name);
					std::cout << "found: (" << inddir << "," << indfile << ") in " << *subdirfilepath << std::endl;
					closedir(subdp);
					closedir(dp);
					return subdirfilepath;
				}
			}
			closedir(subdp);
		}
	}
	closedir(dp);
	return NULL;
}

string * nextFile(string *dirpath, int *currentDirIndex, int *currentFileIndex) {
	string * returnedString;
	if ((returnedString = is_subdir_subfile_index(dirpath, *currentDirIndex, *currentFileIndex))) {
		(*currentFileIndex)++;
		return returnedString;
	}
	else {
		(*currentDirIndex)++;
		(*currentFileIndex) = 0;
		if ((returnedString = is_subdir_subfile_index(dirpath, *currentDirIndex, *currentFileIndex))) {
			(*currentFileIndex)++;
			return returnedString;
		}
		else {
			(*currentDirIndex) = 0;
			(*currentFileIndex) = 0;
			if ((returnedString = is_subdir_subfile_index(dirpath, *currentDirIndex, *currentFileIndex))) {
				(*currentFileIndex)++;
				return returnedString;
			}
			else {
				std::cout << "Error(" << errno << ") opening " << dirpath << std::endl;
				return NULL;
			}
		}
	}
	return NULL;
}

///////////////////////////////////////////////////////////////////////////////////
// UDP BASED COMMANDS
///////////////////////////////////////////////////////////////////////////////////
void pg_aliasScript( char *newCommand , 
		     char* string_argument_0 ,
		     float arguments[MAX_OSC_ARGUMENTS] ) {
  char AuxString[1024];

   printf("command %s %.2f\n", newCommand , arguments[0]);
  // commands for reverb
  if( strcmp( newCommand , "dampingfreq" ) == 0 ) {
	em_verb.setParameter(MVerb<float>::DAMPINGFREQ, arguments[0]);
	sprintf( AuxString , "/dampingfreq_can %f" , arguments[0] ); pg_send_message_udp( (char *)"f" , (char *)AuxString , (char *)"udp_PD_send" );

  } else if( strcmp( newCommand , "density" ) == 0 ) {
	em_verb.setParameter(MVerb<float>::DENSITY, arguments[0]);
	sprintf( AuxString , "/density_can %f" , arguments[0] ); pg_send_message_udp( (char *)"f" , (char *)AuxString , (char *)"udp_PD_send" );

  } else if( strcmp( newCommand , "bandwidthfreq" ) == 0 ) {
	em_verb.setParameter(MVerb<float>::BANDWIDTHFREQ, arguments[0]);
	sprintf( AuxString , "/bandwidthfreq_can %f" , arguments[0] ); pg_send_message_udp( (char *)"f" , (char *)AuxString , (char *)"udp_PD_send" );

  } else if( strcmp( newCommand , "decay" ) == 0 ) {
	em_verb.setParameter(MVerb<float>::DECAY, arguments[0]);
	sprintf( AuxString , "/decay_can %f" , arguments[0] ); pg_send_message_udp( (char *)"f" , (char *)AuxString , (char *)"udp_PD_send" );

  } else if( strcmp( newCommand , "predelay" ) == 0 ) {
	em_verb.setParameter(MVerb<float>::PREDELAY, arguments[0]);
	sprintf( AuxString , "/predelay_can %f" , arguments[0] ); pg_send_message_udp( (char *)"f" , (char *)AuxString , (char *)"udp_PD_send" );

  } else if( strcmp( newCommand , "size" ) == 0 ) {
	em_verb.setParameter(MVerb<float>::SIZE, arguments[0]);
	sprintf( AuxString , "/size_can %f" , arguments[0] ); pg_send_message_udp( (char *)"f" , (char *)AuxString , (char *)"udp_PD_send" );

  } else if( strcmp( newCommand , "gain" ) == 0 ) {
	em_verb.setParameter(MVerb<float>::GAIN, arguments[0]);
	sprintf( AuxString , "/gain_can %f" , arguments[0] ); pg_send_message_udp( (char *)"f" , (char *)AuxString , (char *)"udp_PD_send" );

  } else if( strcmp( newCommand , "mix" ) == 0 ) {
	em_verb.setParameter(MVerb<float>::MIX, arguments[0]);
	sprintf( AuxString , "/mix_can %f" , arguments[0] ); pg_send_message_udp( (char *)"f" , (char *)AuxString , (char *)"udp_PD_send" );

  } else if( strcmp( newCommand , "earlymix" ) == 0 ) {
	em_verb.setParameter(MVerb<float>::EARLYMIX, arguments[0]);
	sprintf( AuxString , "/earlymix_can %f" , arguments[0] ); pg_send_message_udp( (char *)"f" , (char *)AuxString , (char *)"udp_PD_send" );

	
	// commands for echo
  } else if( strcmp( newCommand , "kLDelay" ) == 0 ) {
	delay.setParameter(kLDelay, arguments[0]);
	sprintf( AuxString , "/kPreDelay_can %f" , arguments[0] ); pg_send_message_udp( (char *)"f" , (char *)AuxString , (char *)"udp_PD_send" );

  } else if( strcmp( newCommand , "kLRDelayRatio" ) == 0 ) {
	delay.setParameter(kLRDelayRatio, arguments[0]);
	sprintf( AuxString , "/kDecay_can %f" , arguments[0] ); pg_send_message_udp( (char *)"f" , (char *)AuxString , (char *)"udp_PD_send" );

   } else if( strcmp( newCommand , "kFeedBk" ) == 0 ) {
	delay.setParameter(kFeedBk, arguments[0]);
	sprintf( AuxString , "/kFeedBk_can %f" , arguments[0] ); pg_send_message_udp( (char *)"f" , (char *)AuxString , (char *)"udp_PD_send" );

  } else if( strcmp( newCommand , "kTone" ) == 0 ) {
	delay.setParameter(kTone, arguments[0]);
	sprintf( AuxString , "/kTone_can %f" , arguments[0] ); pg_send_message_udp( (char *)"f" , (char *)AuxString , (char *)"udp_PD_send" );

  } else if( strcmp( newCommand , "kMix" ) == 0 ) {
	delay.setParameter(kMix, arguments[0]);
	sprintf( AuxString , "/kMix_can %f" , arguments[0] ); pg_send_message_udp( (char *)"f" , (char *)AuxString , (char *)"udp_PD_send" );

  } else if( strcmp( newCommand , "kLevel" ) == 0 ) {
	delay.setParameter(kLevel, arguments[0]);
	sprintf( AuxString , "/kLevel_can %f" , arguments[0] ); pg_send_message_udp( (char *)"f" , (char *)AuxString , (char *)"udp_PD_send" );

  }
}

////////////////////////////////
// scans the original samples and generates all the small ones
void generate_all_samples(void) {
	std::string SampleDir = "./moire/samples-original";
	int IndProcessedSampleFileDir = 0;
	int IndPreviousProcessedSampleFileDir = -1;
	int IndProcessedSampleFile = 0;
	int IndRhythmSample = 0;
	int IndDroneSample = 0;
	std::string *fileName;

	int nbRhythms = 6;
	int nbDromes = 6;

	while ((fileName = nextFile(&SampleDir,
		&IndProcessedSampleFileDir, &IndProcessedSampleFile))) {
		size_t start_pos = fileName->find("/samples-original");
		size_t lastSlashindex = fileName->find_last_of("/");
		string dirRhythm = fileName->substr(0, lastSlashindex);
		dirRhythm = dirRhythm.replace(start_pos, strlen("/samples-original"), "/samples/rhythms");
		string dirDrone = fileName->substr(0, lastSlashindex);
		dirDrone = dirDrone.replace(start_pos, strlen("/samples-original"), "/samples/drones");

		if (IndPreviousProcessedSampleFileDir != IndProcessedSampleFileDir) {
			if (IndProcessedSampleFileDir == 0 && IndPreviousProcessedSampleFileDir > 0) {
				break;
			}
			IndRhythmSample = 0;
			IndDroneSample = 0;

			string nonBSdir = dirRhythm;
#ifdef _WIN32
			std::replace(nonBSdir.begin(), nonBSdir.end(), '/', '\\'); // replace all '/' to '\\'
#endif
			printf("command [%s]\n", ("mkdir " + nonBSdir).c_str());
			if (!system(("mkdir " + nonBSdir).c_str())) {
				// directory exists
			}
			nonBSdir = dirDrone;
#ifdef _WIN32
			std::replace(nonBSdir.begin(), nonBSdir.end(), '/', '\\'); // replace all '/' to '\\'
#endif
			printf("command [%s]\n", ("mkdir " + nonBSdir).c_str());
			if (!system(("mkdir " + nonBSdir).c_str())) {
				// directory exists
			}
		}
		IndPreviousProcessedSampleFileDir = IndProcessedSampleFileDir;

		size_t lastDotindex = fileName->find_last_of(".");
		string convertedFileName = *fileName;
		convertedFileName = convertedFileName.substr(0, lastDotindex) + "_converted.wav";

		string sampleRhythmFileName = dirRhythm + "/sample_";
		string sampleDroneFileName = dirDrone + "/sample_";
		// printf("sample rhythm file [%s] %d\n" , sampleRhythmFileName.c_str() ,IndRhythmSample);
		// printf("sample drone file [%s] %d\n" , sampleDroneFileName.c_str() , IndDroneSample);


		pg_SampleGeneration(&convertedFileName, &sampleRhythmFileName, &sampleDroneFileName,
			IndRhythmSample, IndDroneSample, nbRhythms, nbDromes);

		IndRhythmSample += nbRhythms;
		IndDroneSample += nbDromes;
		delete fileName;
	}
}

void process_file_plugin(void) {
  // VST plugin test
  // loads VST plugin
  // char *vstPath = "C:\\Users\\jacquemi\\Porphyrograph\\vst_plugins\\plugin-test\\HY-Delay_32_64\\HY-Delay_x64.dll";
  // char *vstPath = "C:\\Users\\jacquemi\\Porphyrograph\\vst_plugins\\plugin-test\\HY-Delay_32_64\\HY-Delay.dll";
  // char *vstPath = "C:\\Users\\jacquemi\\Porphyrograph\\vst_plugins\\MVerb_vst\\MVerb\\win\\Release-Win32\\MVerb.dll";
  // char *vstPath = "G:\\home-win\\Porphyrograph\\vst_plugins\\plugin-test\\HY-Delay_32_64\\HY-Delay_x64.dll";
  // char *vstPath = "G:\\home-win\\Porphyrograph\\vst_plugins\\plugin-test\\HY-Delay_32_64\\HY-Delay.dll";
  char *vstPath = "G:\\home-win\\Porphyrograph\\vst_plugins\\MVerb_vst\\MVerb\\win\\Release-Win32\\MVerb.dll";
  enum{ DAMPINGFREQ = 0, DENSITY, BANDWIDTHFREQ, DECAY, PREDELAY, SIZE, GAIN, MIX, EARLYMIX, NUM_PARAMS };
  // DampingFreq = 18000.; BandwidthFreq = 18000.; SampleRate = 44100.; Decay = 0.5;
  // Gain = 1.; Mix = 1.; Size = 1.; EarlyMix = 1.;
  myPlugin = loadVSTPlugin(vstPath);
  dispatcherFuncPtr dispatcher = NULL;
  if (configureVSTPlugin(myPlugin, &dispatcher)) {
	  printf("Configured VST plugin [%s]\n", vstPath);
	  initVSTPlugin(myPlugin, dispatcher);
	  myPlugin->setParameter(myPlugin, VstInt32(MIX), 0.35f);
	  myPlugin->setParameter(myPlugin, VstInt32(PREDELAY), 0.4f);
	  myPlugin->setParameter(myPlugin, VstInt32(DECAY), 0.5f);
  }
  // processes a file with this plugin
  string in = "moire\\life_002.wav";
  string out = "moire\\life_002_vst.wav";
   pg_VST_processor(&in, &out, myPlugin);
}

#ifdef WIN32
DWORD WINAPI processUDP( LPVOID lpParam ) {
#else
void* processUDP(void * lpParam) {
#endif
  int *pData = (int *)lpParam;
  while( true ) {
	   for( int ind = 0 ; ind < nb_IP_Servers ; ind++ ) {
		IP_Servers[ ind ]->receiveIPMessages();
	  }
	  for( int ind = 0 ; ind < nb_IP_Clients ; ind++ ) {
		IP_Clients[ ind ]->sendIPmessages();
	  }
#ifdef WIN32
	  Sleep(100);
#else
	  sleep(100);
#endif
  }
}

  ////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
// MAIN
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
BOOL CtrlHandler( DWORD fdwCtrlType ) 
{ 
  switch( fdwCtrlType ) 
  { 
    // Handle the CTRL-C signal. 
    case CTRL_C_EVENT: 
	  pg_audio_deallocation();
      printf( "Ctrl-C event\n\n" );
	  WaitForSingleObject(hThread, INFINITE);
	  CloseHandle(hThread);

	  //getchar();
      return( TRUE );
 
    // CTRL-CLOSE: confirm that the user wants to exit. 
    case CTRL_CLOSE_EVENT: 
	  pg_audio_deallocation();
      printf( "Ctrl-Close event\n\n" );
	  WaitForSingleObject(hThread, INFINITE);
	  CloseHandle(hThread);

	  //getchar();
      return( TRUE ); 
 
    // Pass other signals to the next handler. 
    case CTRL_BREAK_EVENT: 
	  pg_audio_deallocation();
      printf( "Ctrl-Break event\n\n" );
	  WaitForSingleObject(hThread, INFINITE);
	  CloseHandle(hThread);

	  //getchar();
      return FALSE; 
 
    case CTRL_LOGOFF_EVENT: 
	  pg_audio_deallocation();
      printf( "Ctrl-Logoff event\n\n" );
	  WaitForSingleObject(hThread, INFINITE);
	  CloseHandle(hThread);

	  //getchar();
      return FALSE; 
 
    case CTRL_SHUTDOWN_EVENT: 
	  pg_audio_deallocation();
      printf( "Ctrl-Shutdown event\n\n" );
	  WaitForSingleObject(hThread, INFINITE);
	  CloseHandle(hThread);

	  //getchar();
      return FALSE; 
 
    default: 
	  pg_audio_deallocation();
      printf( "Unknown termination event\n\n" );
	  WaitForSingleObject(hThread, INFINITE);
	  CloseHandle(hThread);

 	  //getchar();
     return FALSE; 
  } 
} 
 
int main(void) {
  string fileName( "./sound/configuration/LYM_sound-head-stereo.csv" );
  printf( "Default configuration file %s\n" , fileName.c_str() );
  printf( "Loading %s\n" , fileName.c_str() );
  std::ifstream fin(fileName.c_str());
  parseConfigurationFile( fin );

  // waits for a char to terminate (the computation is handled by threads)
  if( SetConsoleCtrlHandler( (PHANDLER_ROUTINE) CtrlHandler, TRUE ) ) 
  { 
    printf( "\nThe Control Handler is installed.\n" ); 
    printf( "\n -- Now try pressing Ctrl+C or Ctrl+Break, or" ); 
    printf( "\n    try logging off or closing the console...\n" ); 
    printf( "\n(...waiting in a loop for events...)\n\n" ); 

	udp_init();

	// samples regeneration
	if (false) {
	generate_all_samples();
	}

	// file plugin processing
	if (false) {
	process_file_plugin();
	}

	// reverb initializing
	em_verb.Init();

	// echo initializing
	delay.Init();

	// thread: audio I/O + effects
	Pa_portaudio_init_and_effets();

	// thread: UDP message processing + idle 
	 int pData =0;
#ifdef WIN32
	 DWORD rc;
	 HANDLE  hThread = CreateThread(
			  NULL,                   // default security attributes
			  0,                      // use default stack size  
			  processUDP,		    // thread function name
			  (void *)&pData,		    // argument to thread function 
			  0,                      // use default creation flags 
			  &rc);   // returns the thread identifier 
	 if (hThread == NULL){
		   std::cout << "Error:unable to create thread processUDP" << std::endl;
		   exit(-1);
	 }
	 CloseHandle(hThread);
#else
	 pthread_t processUDP_thread;
	 int rc;
	 rc = pthread_create(&processUDP_thread, NULL, 
							processUDP, (void *)&pData);
	 if (rc){
		   std::cout << "Error:unable to create thread processUDP" << rc << std::endl;
		   pthread_exit(NULL);
	 }
#endif

	// waits for a termination (a char or ^c or x the winow)
    char c;
	do
		c = getchar();
	while (c != 'q');

	pg_audio_deallocation();
	printf( "quit\n" );
	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);
  } 
  else 
  {
    printf( "\nERROR: Could not set control handler"); 
  }
}
