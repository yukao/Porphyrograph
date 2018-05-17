////////////////////////////////////////////////////////////////////
// 
// 
// 
//     File pg-sample.cpp
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
// NON THREADED READING OF SHORT SAMPLES AT ONCE
// loads a whole buffer (when it is not too long) - uses the odd PingPong Buffer
bool pg_readShortSampleBuffer(void * lpParam) {
	AudioSampleDataStruct * pDataArray = (AudioSampleDataStruct *)lpParam;

	long numFrames = 0L;
	// Load data
	numFrames // = pDataArray->sampleBufferFrameSize;
		= (long)sf_readf_float(pDataArray->sampleSndFile,
		pDataArray->largeSampleBuffer,
		pDataArray->sampleBufferFrameSize);

	// the format short == PG_AUDIO_SAMPLE_FORMAT (paInt16) is checked during audio initialization
	sf_close(pDataArray->sampleSndFile);
	pDataArray->sampleSndFile = NULL;

	// Check correct number of samples loaded
	if (numFrames != pDataArray->sampleBufferFrameSize) {
		printf("Did not read enough frames for source [%s] (%ld/%ld)\n",
			pDataArray->name.c_str(), numFrames, pDataArray->sampleBufferFrameSize);
		pDataArray->release();
		return false;
	}

	pDataArray->loading = false;
	pDataArray->loaded = true;

	// delete pDataArray;
	return true;
}

// NON THREADED READING OF SHORT SAMPLES AT ONCE
// loads a whole buffer (when it is not too long) - uses the odd PingPong Buffer
#ifdef WIN32
DWORD WINAPI pg_threadedReadShortSampleBuffer(LPVOID lpParam) {
#else
void* pg_threadedReadShortSampleBuffer(void * lpParam) {
#endif
	AudioSampleDataStruct * pDataArray = (AudioSampleDataStruct *)lpParam;

	long numFrames = 0L;
	// Load data
	numFrames // = pDataArray->sampleBufferFrameSize;
		= (long)sf_readf_float(pDataArray->sampleSndFile,
		pDataArray->largeSampleBuffer,
		pDataArray->sampleBufferFrameSize);

	// the format short == PG_AUDIO_SAMPLE_FORMAT (paInt16) is checked during audio initialization
	sf_close(pDataArray->sampleSndFile);
	pDataArray->sampleSndFile = NULL;

	// Check correct number of samples loaded
	if (numFrames != pDataArray->sampleBufferFrameSize) {
		printf("Did not read enough frames for source [%s] (%ld/%ld)\n",
			pDataArray->name.c_str(), numFrames, pDataArray->sampleBufferFrameSize);
		pDataArray->release();
#ifndef WIN32
		pthread_exit(NULL);
#endif
		return false;
	}

	pDataArray->loading = false;
	pDataArray->loaded = true;

	// delete pDataArray;
	return true;
}


bool pg_readLongSampleBuffer(LPVOID lpParam) {
	AudioSampleDataStruct * pDataArray = (AudioSampleDataStruct *)lpParam;
	// all the data have already been loaded
	if (pDataArray->loaded) {
		return false;
	}

	// the top frame is increased by the size of the data that has just been loaded
	// the lower frame is the former top frame or 0 if it is the first access (the
	// lower frame is initialized to -1)
	if (pDataArray->lowerLoadedFrame < 0) {
		pDataArray->lowerLoadedFrame = 0;
	}

	// loads the frames
	long numFrames = 0L;
	// Load data
	long bufferFrameSize = min(PG_LONG_SAMPLE_READ_BUFFER,
		pDataArray->sampleBufferFrameSize
		- pDataArray->lowerLoadedFrame);
	numFrames // = pDataArray->sampleBufferFrameSize;
		= (long)sf_readf_float(pDataArray->sampleSndFile,
			pDataArray->largeSampleBuffer, bufferFrameSize);

	// the format short == PG_AUDIO_SAMPLE_FORMAT (paInt16) is checked during audio initialization

	// Check correct number of samples loaded
	if (numFrames != bufferFrameSize) {
		printf("Did not read enough frames for source [%s] (%ld/%ld)\n",
			pDataArray->name.c_str(), numFrames, bufferFrameSize);
		// pDataArray->release();
		// return false;
		// replaces by silence
		for (long ind = numFrames; ind < bufferFrameSize; ind++) {
			pDataArray->largeSampleBuffer[ind] = 0.f;
		}
	}

	// updates the upper loaded frame
	pDataArray->upperLoadedFrame
		= pDataArray->lowerLoadedFrame + bufferFrameSize;

	//printf("\nread sample track %ld-%ld buffer %d\n", 
	//		pDataArray->lowerLoadedFrame[indBufferToLoad],
	//		pDataArray->upperLoadedFrame[indBufferToLoad],
	//		indBufferToLoad);

	// if we have  reached the end... change the value of the boolean "loaded"
	if (pDataArray->upperLoadedFrame == pDataArray->sampleBufferFrameSize) {
		pDataArray->loaded = true;
		// sf_close(pDataArray->sampleSndFile);
		// pDataArray->sampleSndFile = NULL;
	}

	// delete pDataArray;
	return true;
}



/////////////////////////////////////////////
// sample loading
int AudioSampleDataStruct::pg_openSample(void) {
	int formatSize = 0;
	// Open sound file
	printf("reading file '%s'\n", name.c_str());
	sampleSndFile = sf_open(name.c_str(), SFM_READ, &sampleSndInfo);
	if (sampleSndFile == NULL) {
		printf("Error reading source file '%s'\n", name.c_str());
		release();
		return 0;
	}

	// Check format - 16bit PCM
	if (sampleSndInfo.format == (SF_FORMAT_WAV | SF_FORMAT_PCM_16)) {
		formatSize = SF_FORMAT_PCM_16;
		//printf("Loading 16 bits wav [%s]\n", name.c_str());
	}
	else if (sampleSndInfo.format == (SF_FORMAT_WAV | SF_FORMAT_PCM_24)) {
		formatSize = SF_FORMAT_PCM_24;
		// printf("Loading 24 bits wav [%s]\n", name.c_str());
	}
	else if (sampleSndInfo.format == (SF_FORMAT_WAV | SF_FORMAT_PCM_32)) {
		formatSize = SF_FORMAT_PCM_32;
		// printf("Loading 32 bits wav [%s]\n", name.c_str());
	}
	else if (sampleSndInfo.format == (SF_FORMAT_WAV | SF_FORMAT_FLOAT)) {
		formatSize = SF_FORMAT_FLOAT;
		// printf("Loading float   wav [%s]\n", name.c_str());
	}
	else {
		printf("Input should be 16bit/24bit/32bit/float \nWav: %s %X\n", name.c_str(),sampleSndInfo.format);
		release();
		return 0;
	}

	// Check channels
	if (sampleSndInfo.channels != PG_AUDIO_NB_CHANNELS) {
		printf("Input should be %d channels: %s\n",
			PG_AUDIO_NB_CHANNELS, name.c_str());
		release();
		return 0;
	}

	// Check sample rate
	if (sampleSndInfo.samplerate != PG_AUDIO_SAMPLE_RATE) {
		printf("Input should be %d sample rate: %s\n",
			PG_AUDIO_SAMPLE_RATE, name.c_str());
		release();
		return 0;
	}

	// Allocate memory
	sampleBufferFrameSize = (long)sampleSndInfo.frames;
	if ((formatSize == SF_FORMAT_PCM_16) || (formatSize == SF_FORMAT_PCM_24) || (formatSize == SF_FORMAT_PCM_32) || (formatSize == SF_FORMAT_FLOAT)) {
		unsigned long buffer_size = (unsigned long)(sampleBufferFrameSize * PG_AUDIO_NB_CHANNELS);
		largeSampleBuffer = new float[buffer_size];
		if (largeSampleBuffer == NULL) {
			printf("Could not allocate memory for data\n");
			release();
			return 0;
		}
		else {
			for (unsigned long ind = 0; ind < buffer_size; ind++) {
				largeSampleBuffer[ind] = 0.f;
			}
		}
	}
	else {
		printf("Input should be 16bit/24bit/32bit/float \nWav: %s %X\n", name.c_str(),sampleSndInfo.format);
		release();
		return 0;
	}

	return formatSize;

} /* pg_openSample */


/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
// SAMPLE GENERATION FROM A SINGLE LONG AUDIO FILE ACCORDING TO OFFSETS & DURATIONS
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
// everything is expressed in secs
void pg_microSample_generation(int nbSubSamples, // number of subsamples
	// all the lengths and offsets are in frames (times * PG_AUDIO_SAMPLE_RATE)
	long microSampleDuration, long initialSampleDuration, // the target and the source buffer lengths
	float *microSampleBuffer, float *initialSampleBuffer, // the target and the source buffers
	long *microSampleOffset, long *initialSampleOffset, // microSampleOffset is the position in target micro sample
	// offset is the position in source big sample
	long *incay, long *decay,  // fadein from microSampleOffset to microSampleOffset + incay
	// fadeout from microSampleOffset + targetduration - decay to microSampleOffset + targetduration
	long *sourceduration, long *targetduration, // length of copy from big sample and lenght of copy into micro sample
	// if targetduration > sourceduration the sample is repeated as much as needed to fill the
	// target duration
	float selfBlendingRatio) {        // ratio of the sample used for overlap to preserve sound continuity in case of 

	// source sample microer than the target one (leading possibly to granular synthesis)
	for (int indMicroSubSample = 0; indMicroSubSample < nbSubSamples; indMicroSubSample++) {
		// data copy with possible loop on the copied source sample to fill the target sample
		long cumulatedDuration = 0L;
		long targetOffset = microSampleOffset[indMicroSubSample];
		long sourceOffset = initialSampleOffset[indMicroSubSample]; 	// off set from beginning

		// should not go beyond the limits of the initial sample
		long maxSourceDuration = min(sourceduration[indMicroSubSample], initialSampleDuration - initialSampleOffset[indMicroSubSample]);
		if (maxSourceDuration <= 0) {
			continue;
		}
		// should not go beyond the limits of the micro sample
		long maxTargetDuration = min(targetduration[indMicroSubSample], microSampleDuration - microSampleOffset[indMicroSubSample]);
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
			// should not copy more than maxTargetDuration in the micro sample
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
				memcpy((uint8_t *)(microSampleBuffer + targetOffset * PG_AUDIO_NB_CHANNELS),
					(void *)(initialSampleBuffer + sourceOffset * PG_AUDIO_NB_CHANNELS),
					copyDuration * PG_AUDIO_NB_CHANNELS * sizeof(float));
			}
			// skips the blended part of the beginning
			else {
				memcpy((uint8_t *)(microSampleBuffer + targetOffset * PG_AUDIO_NB_CHANNELS),
					(void *)(initialSampleBuffer + (sourceOffset + selfBlendingRatioFrames) * PG_AUDIO_NB_CHANNELS),
					copyDuration * PG_AUDIO_NB_CHANNELS * sizeof(float));
			}

			// blending at the beginning of the copy (over the end of the preceding one)
			if (indLoop > 0 && selfBlendingRatioFrames > 0) {
				long indBegin = targetOffset - selfBlendingRatioFrames;
				for (long indTrack = indBegin; indTrack < targetOffset; indTrack++) {
					double ratio = (double)(indTrack - indBegin) / selfBlendingRatioFrames;
					for (int indCh = 0; indCh < 2; indCh++) {
						microSampleBuffer[indTrack * PG_AUDIO_NB_CHANNELS + indCh]
							= float(initialSampleBuffer[(sourceOffset + (indTrack - indBegin)) * PG_AUDIO_NB_CHANNELS + indCh] * ratio // the beginning of the newly copied sample
							+ initialSampleBuffer[(sourceOffset + (maxSourceDuration - selfBlendingRatioFrames + (indTrack - indBegin))) * PG_AUDIO_NB_CHANNELS + indCh] * (1.0 - ratio)); // the end of the previously copied sample
					}
				}
			}

			// offsets update according to copy
			// sample made slightly shorter so that self bleding can be made in case of multiple copies
			cumulatedDuration += copyDuration;
			targetOffset = microSampleOffset[indMicroSubSample] + cumulatedDuration;

			// loop counter to manage self-blending in case of multiple copies
			indLoop++;
		}

		// reinitializes the target offset
		targetOffset = microSampleOffset[indMicroSubSample];

		// fade in management
		long actualincay = min(incay[indMicroSubSample], maxTargetDuration);
		for (long indTrack = 0; indTrack < actualincay; indTrack++) {
			double ratio = (double)indTrack / actualincay;
			double weight = sin(ratio * M_PI / 2.0);
			for (int indCh = 0; indCh < 2; indCh++) {
				microSampleBuffer[(targetOffset + indTrack) * PG_AUDIO_NB_CHANNELS + indCh]
					= float(microSampleBuffer[(targetOffset + indTrack) * PG_AUDIO_NB_CHANNELS + indCh] * weight);
			}
		}
		// fade out management
		long actualdecay = min(decay[indMicroSubSample], maxTargetDuration);
		long decayStart = maxTargetDuration - actualdecay;
		for (long indTrack = decayStart; indTrack < maxTargetDuration; indTrack++) {
			double ratio = (double)indTrack / (maxTargetDuration - decayStart);
			double weight = sin((1.0 - ratio) * M_PI / 2.0);
			for (int indCh = 0; indCh < 2; indCh++) {
				microSampleBuffer[(targetOffset + indTrack) * PG_AUDIO_NB_CHANNELS + indCh]
					= float(microSampleBuffer[(targetOffset + indTrack) * PG_AUDIO_NB_CHANNELS + indCh] * weight);
			}
		}
	}
}

/////////////////////////////////////////////////////////
// REPEATED CALL TO SINGLE SAMPLE GENERATION FROM LONG AUDIO FILE

#define   PG_NB_MAX_SMALLSAMPLE_SUBSAMPLES   16
bool pg_SampleGeneration(string *initialFileName, string *sampleRhythmfileName, string *sampleDronefileName,
	int startRhythmIndex, int startDroneIndex, int nbRhythms, int nbDrones) {
	bool valret = true;

	AudioSampleDataStruct *initialSampleBufferData = new AudioSampleDataStruct();

	char fnameMicroSample[256];
	SF_INFO microSample_sndInfo;
	microSample_sndInfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
	microSample_sndInfo.channels = PG_AUDIO_NB_CHANNELS;
	microSample_sndInfo.samplerate = PG_AUDIO_SAMPLE_RATE;

	////////////////////////////////////////////////////////////////
	// LOADS BIG float
	// opens the samples
	initialSampleBufferData->name = *initialFileName;
	int formatSize;
	if (!(formatSize = initialSampleBufferData->pg_openSample())) {
		printf("File opening failure.\n");
		valret = false;
	}
	// and load them
	pg_readShortSampleBuffer((void *)initialSampleBufferData);

	double initialSampleDuration = initialSampleBufferData->sampleBufferFrameSize / (double)PG_AUDIO_SAMPLE_RATE;

	// allocates the writing buffer
	double microSampleDuration = 4.0;
	long microSampleDurationFr = DurationToFrames(microSampleDuration);
	float *microSampleBuffer = new float[(unsigned long)(microSampleDurationFr * PG_AUDIO_NB_CHANNELS)];
	/////////////////////////////////////////////////////
	// 16 rhythmic samples from voice
	for (int indRhythmSample = startRhythmIndex; indRhythmSample < startRhythmIndex + nbRhythms; indRhythmSample++) {
		int nbSubSamples = 4;
		double beat = microSampleDuration / 16.0;
		double microSampleOffset[4] = { 0.0, 4.0*beat, 8.0*beat, 12.0*beat };
		double offSetBasis = initialSampleDuration / 2.0;
		double offSetVariation = initialSampleDuration / 4.0;
		double initialSampleOffset[16];
		for (int indSubSample = 0; indSubSample < nbSubSamples; indSubSample++) {
			initialSampleOffset[indSubSample] = ((double)rand() / (RAND_MAX))*offSetBasis + ((double)rand() / (RAND_MAX))*offSetVariation;
		}
		double sourceduration[16] = { 4 * beat, 4 * beat, 4 * beat, 4 * beat };
		double targetduration[16] = { 4 * beat, 4 * beat, 4 * beat, 4 * beat };
		double incay[16] = { beat, beat, beat, beat };
		double decay[16] = { beat, beat, beat, beat };
		float selfBlendingRatio = 0.2f;

		long microSampleOffsetFr[16];
		long initialSampleOffsetFr[16];
		long sourcedurationFr[16];
		long targetdurationFr[16];
		long incayFr[16];
		long decayFr[16];
		for (int indSubSample = 0; indSubSample < nbSubSamples; indSubSample++) {
			microSampleOffsetFr[indSubSample] = DurationToFrames(microSampleOffset[indSubSample]);
			initialSampleOffsetFr[indSubSample] = DurationToFrames(initialSampleOffset[indSubSample]);
			sourcedurationFr[indSubSample] = DurationToFrames(sourceduration[indSubSample]);
			targetdurationFr[indSubSample] = DurationToFrames(targetduration[indSubSample]);
			incayFr[indSubSample] = DurationToFrames(incay[indSubSample]);
			decayFr[indSubSample] = DurationToFrames(decay[indSubSample]);
		}
		pg_microSample_generation(nbSubSamples, // number of subsamples
			// all the lengths and offsets are in frames (times * PG_AUDIO_SAMPLE_RATE * PG_AUDIO_NB_CHANNELS)
			microSampleDurationFr, initialSampleBufferData->sampleBufferFrameSize, // the target and the source buffer lengths
			microSampleBuffer, initialSampleBufferData->largeSampleBuffer, // the target and the source buffers
			microSampleOffsetFr, initialSampleOffsetFr, // microSampleOffset is the position in target micro sample
			// offset is the position in source big sample
			incayFr, decayFr,  // fadein from microSampleOffset to microSampleOffset + incay
			// fadeout from microSampleOffset + targetduration - decay to microSampleOffset + targetduration
			sourcedurationFr, targetdurationFr, // length of copy from big sample and lenght of copy into micro sample
			// if targetduration > sourceduration the sample is repeated as much as needed to fill the
			// target duration
			selfBlendingRatio);        // ratio of the sample used for overlap to preserve sound continuity in case of 


		////////////////////////////////////////////////////////////////
		// SAVES SMALL float
		sprintf(fnameMicroSample, "%s%03d.wav", sampleRhythmfileName->c_str(), indRhythmSample);
		microSample_sndInfo.frames = microSampleDurationFr;
		// printf("micro sample writing begin %s\n",fnameMicroSample);
		pg_audio_create_file(fnameMicroSample, &microSample_sndInfo,
			microSampleDurationFr, microSampleBuffer);
		// printf("micro sample writing end\n");
	}

	/////////////////////////////////////////////////////
	// 16 drone samples from voice
	for (int indDroneSample = startDroneIndex; indDroneSample < startDroneIndex + nbDrones; indDroneSample++) {
		int nbSubSamples = 1;
		double microSampleOffset[1] = { 0.0 };
		double offSetBasis = initialSampleDuration / 2.0;
		double offSetVariation = initialSampleDuration / 4.0;
		double initialSampleOffset[1] = { (((double)rand() / (RAND_MAX))*offSetBasis + ((double)rand() / (RAND_MAX))*offSetVariation) };
		double sourceduration[1] = { 0.03 };
		double targetduration[1] = { microSampleDuration };
		double incay[1] = { 0.0 };
		double decay[1] = { 0.0 };
		float selfBlendingRatio = 0.2f;

		long microSampleOffsetFr[1];
		long initialSampleOffsetFr[1];
		long sourcedurationFr[1];
		long targetdurationFr[1];
		long incayFr[1];
		long decayFr[1];
		for (int indSubSample = 0; indSubSample < 1; indSubSample++) {
			microSampleOffsetFr[indSubSample] = DurationToFrames(microSampleOffset[indSubSample]);
			initialSampleOffsetFr[indSubSample] = DurationToFrames(initialSampleOffset[indSubSample]);
			sourcedurationFr[indSubSample] = DurationToFrames(sourceduration[indSubSample]);
			targetdurationFr[indSubSample] = DurationToFrames(targetduration[indSubSample]);
			incayFr[indSubSample] = DurationToFrames(incay[indSubSample]);
			decayFr[indSubSample] = DurationToFrames(decay[indSubSample]);
		}
		pg_microSample_generation(nbSubSamples, // number of subsamples
			// all the lengths and offsets are in frames (times * PG_AUDIO_SAMPLE_RATE * PG_AUDIO_NB_CHANNELS)
			microSampleDurationFr, initialSampleBufferData->sampleBufferFrameSize, // the target and the source buffer lengths
			microSampleBuffer, initialSampleBufferData->largeSampleBuffer, // the target and the source buffers
			microSampleOffsetFr, initialSampleOffsetFr, // microSampleOffset is the position in target micro sample
			// offset is the position in source big sample
			incayFr, decayFr,  // fadein from microSampleOffset to microSampleOffset + incay
			// fadeout from microSampleOffset + targetduration - decay to microSampleOffset + targetduration
			sourcedurationFr, targetdurationFr, // length of copy from big sample and lenght of copy into micro sample
			// if targetduration > sourceduration the sample is repeated as much as needed to fill the
			// target duration
			selfBlendingRatio);        // ratio of the sample used for overlap to preserve sound continuity in case of 


		////////////////////////////////////////////////////////////////
		// SAVES SMALL float
		sprintf(fnameMicroSample, "%s%03d.wav", sampleDronefileName->c_str(), indDroneSample);
		microSample_sndInfo.frames = microSampleDurationFr;
		// printf("micro sample writing begin\n");
		pg_audio_create_file(fnameMicroSample, &microSample_sndInfo,
			microSampleDurationFr, microSampleBuffer);
		// printf("micro sample writing end\n");
	}

	// printf("buffer deletion\n");
	delete microSampleBuffer;
	delete initialSampleBufferData;
	return valret;
}

////////////////////////////////
// scans the original samples and generates all the micro ones
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

	while ((fileName = nextFile(&SampleDir,&IndProcessedSampleFileDir, &IndProcessedSampleFile))) {
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





