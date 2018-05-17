////////////////////////////////////////////////////////////////////
// 
// 
//     File pg-sample.h
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

#ifndef PG_SAMPLE_H
#define PG_SAMPLE_H

///////////////////////////////////////////////////////
// AUDIO DATA STRUCTURE
///////////////////////////////////////////////////////

// data structure for threads to use.
// This is passed by void pointer so it can be any data type
// that can be passed using a single void pointer (LPVOID).
class AudioSampleDataStruct {
private:
public:
	SF_INFO sampleSndInfo;
	string name;                      // sample file name
	long sampleBufferFrameSize;       // the size of the whole sample in frames
	long lowerLoadedFrame;         // the lower loaded frame in memory for odd and even buffers (for large samples)
	long upperLoadedFrame;         // the higher loaded frame in memory for odd and even buffers (for large samples)
	bool loading;                     // whether or not the sample is currently being loaded by a thread
	bool loaded;                      // whether or not the whole sample has been loaded in memory
									  // tells whether another reading of the file data is possible or not
	SNDFILE *sampleSndFile;
	float * largeSampleBuffer;   
	AudioSampleDataStruct(void) {
		name = "";
		sampleBufferFrameSize = 0;
		lowerLoadedFrame = -1;
		upperLoadedFrame = 0;
		loaded = false;
		sampleSndFile = NULL;
		sampleSndInfo.frames = 0;
		// the full buffer of the sample
		largeSampleBuffer = NULL;
		// a kind of safety zone is preserved at the beginning of the sample
		// this zone contains spare bytes that can be used to blend the end
		// of a sample with its beginning
	}
	void release(void) {
		name = "";
		sampleBufferFrameSize = 0;
		lowerLoadedFrame = -1;
		upperLoadedFrame = 0;
		loaded = false;
		if (sampleSndFile) {
			sf_close(sampleSndFile);
			sampleSndFile = NULL;
		}
		sampleSndInfo.frames = 0;
		if (largeSampleBuffer) {
			delete[](largeSampleBuffer);
			largeSampleBuffer = NULL;
		}
	}
	~AudioSampleDataStruct(void) {
        // std::cout<<"AudioSampleDataStruct destructor"<<std::endl;
		if (sampleSndFile) {
			sf_close(sampleSndFile);
			sampleSndFile = NULL;
		}
		if (largeSampleBuffer) {
			delete[](largeSampleBuffer);
			largeSampleBuffer = NULL;
		}
	}
	int pg_openSample(void);
};

////////////////////////////////////////////////////////////////////
// AUDIO BUFFERS
////////////////////////////////////////////////////////////////////
// Structures pointing to sample audio data and used for playing or for swapping
#define PG_AUDIO_NB_BUFFERS                 (6)
#define PG_NB_SHORT_TRACKS 10
#define PG_LONG_SAMPLE_READ_BUFFER    (262144)

////////////////////////////////////////////////////////////////////
// AUDIO SEQUENCES
////////////////////////////////////////////////////////////////////
class AudioSequenceStreaming {
public:
	string name;
	int *bufNum;                                // small track buffers attached to the sequence

	int size;                                   // number of pieces inside the sequence
	int currPiece;                              // working variable: currently playing piece in the sequence
	int nbSeqRepetitions;                       // working variable: number of times a sequence has looped
	double *seqDur;                             // individual duration of eache piece  (in beats)
	double *cumulDur;                           // cumulated duration before the current piece (in beats)
	double totalDuration;                       // total duration of the sequence (in beats)
							                    // cumulDur[ size ] is the total duration of the sequence
	double currPieceTime;                       // working variable: relative time of the currently playing piece
	float *amp;                                 // amplitudes associated with each piece
												// a null amplitude corresponds to a silence
	int   nbLoops;                              // number of loops, -1 for infinite repetition
	int   synchronized;                         // whether or not a sample play is synchronized on the beats
	double singleFrameDuration;                 // the duration of the single track of a non synchronized sequence

	double startStreamingTime;                  // working variable: time at which the playing of the sequence has started
	float seqLevel;                             // a level for all the pieces in the sequence 
	                                            // (actual level = level * amp[ indPiece ])
	bool seqPlaying;                            // sequence is playing
	bool seqStarted;                            // sequence is started and possibly waiting for next beat 
	                                            // multiple of BeatsPerSequence to start
	bool isSequenceTerrainVagues;               // RHTYHMIC SEQUENCE FOR SAMPLES

	AudioSequenceStreaming(int seq_size) {
		size = seq_size;
		currPiece = -1;
		name = "";
		bufNum = new int[size];
		seqDur = new double[size];
		cumulDur = new double[size + 1];
		totalDuration = 0.;
		amp = new float[size];
		for (int ind = 0; ind < size; ind++) {
			amp[ind] = 0.f;
		}
		nbLoops = -1; // default: infinite repetition
		synchronized = 1; // default: sample synchronized on the beats
		singleFrameDuration = 0.;
		startStreamingTime = 0.0;;
		seqLevel = 0.f;
		seqPlaying = false;
		seqStarted = false;
		nbSeqRepetitions = 0;
		isSequenceTerrainVagues = false;
	}
	~AudioSequenceStreaming(void) {
		size = 0;
		currPiece = 0;
		name = "";
		if (bufNum) {
			delete[]bufNum;
			bufNum = NULL;
		}
		if (seqDur) {
			delete[]seqDur;
			seqDur = NULL;
		}
		if (cumulDur) {
			delete[]cumulDur;
			cumulDur = NULL;
		}
		if (amp) {
			delete[]amp;
			amp = NULL;
		}
		totalDuration = 0.;
		nbLoops = -1; // default: infinite repetition
		synchronized = 1; // default: sample synchronized on the beats
		singleFrameDuration = 0.;
		startStreamingTime = 0.0;;
		seqLevel = 0.f;
		seqPlaying = false;
		seqStarted = false;
		nbSeqRepetitions = 0;
		isSequenceTerrainVagues = false;
	}
};

////////////////////////////////////////////////////////////////////
// LONG SAMPLE STREAMING
////////////////////////////////////////////////////////////////////
class AudioLongTrackStreaming {
public:
	double startStreamingTime;         // working variable: time at which the playing of the sequence has started
	float trackLevel;                       // a level for output 
	bool trackPlaying;                      // sample is playing
	bool trackStarted;                      // sample is started and possibly waiting for next beat 
	// multiple of BeatsPerSequence to start
	AudioLongTrackStreaming(void) {
		startStreamingTime = 0.0;;
		trackLevel = 0.f;
		trackPlaying = false;
		trackStarted = false;
	}
	~AudioLongTrackStreaming(void) {
		startStreamingTime = 0.0;;
		trackLevel = 0.f;
		trackPlaying = false;
		trackStarted = false;
	}
};

////////////////////////////////////////////////////////////////////
// FEEDBACK LOOP
////////////////////////////////////////////////////////////////////
class AudioLoopBuffer {
public:
	double  loopDur;                // loop duration
	long  nbLoopFrames;             // loop nb of frames
	long  nbBufferFrames;             // buffer nb of frames
	float *loopSoundBuffer;         // loop float buffer
	double startRecordingTime;      // working variable: time at which the recording of the loop has started
	float loopOutLevel;             // playing level
	bool loopRecording;             // loop is recording
	bool loopPlaying;               // loop is playing
	bool loopEmpty;                 // loop is empty
	double time_stopped;             // real time last stop pressed
	bool loopFlashing;              // loop flqshing in case of cleaning up

	AudioLoopBuffer(double loop_duration) {
		loopDur = loop_duration;
		nbLoopFrames = long(loopDur * PG_AUDIO_SAMPLE_RATE);
		nbBufferFrames = nbLoopFrames;
	    loopSoundBuffer = new float[(unsigned long)(nbBufferFrames * PG_AUDIO_NB_CHANNELS)];
		memset((void *)loopSoundBuffer, 0, (nbBufferFrames * PG_AUDIO_NB_CHANNELS)*sizeof(float));
		// printf("Loop dur %.4f nbFrames %ld \n",loop_duration,nbLoopFrames);
		startRecordingTime = 0.0;
		loopOutLevel = 0.f;
		loopRecording = false;
		loopPlaying = false;
		loopEmpty = true;
		time_stopped = 0.;
		loopFlashing = false;
	}
	~AudioLoopBuffer(void) {
		loopDur = 0.;
		nbLoopFrames = 0;
		nbBufferFrames = 0;
		if (loopSoundBuffer) {
			delete[]loopSoundBuffer;
			loopSoundBuffer = NULL;
		}
		startRecordingTime = 0.0;;
		loopOutLevel = 0.f;
		loopRecording = false;
		loopPlaying = false;
		loopEmpty = true;
		time_stopped = 0.;
		loopFlashing = false;
	}
};

////////////////////////////////////////////////////////////////////
// AUDIO SAMPLES
////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////
// sample index management for progessive sample turnover
// according to sample swap frequency and duration
// and according to directory size
extern std::string SampleDir;

/// NON THREADED READ OF SHORT SAMPLES
bool pg_readShortSampleBuffer(void * lpParam);

/// THREADED READ OF SHORT SAMPLES
#ifdef WIN32
DWORD WINAPI pg_threadedReadShortSampleBuffer(LPVOID lpParam);
#else
void* pg_threadedReadShortSampleBuffer(void * lpParam);
#endif

bool pg_readLongSampleBuffer(LPVOID lpParam);

/// SAVE float
bool pg_audio_create_file(const char * pg_sampleName, SF_INFO *sndInfo, long frames,
	float *buffer);

bool pg_SampleGeneration(string *initialFileName, string *sampleRhythmfileName, string *sampleDronefileName, 
  int startRhythmIndex, int startDroneIndex, int nbRhythms , int nbDrones );
void pg_smallSample_generation(int nbSubSamples, // number of subsamples
	// all the lengths and offsets are in frames (times * PG_AUDIO_SAMPLE_RATE * PG_AUDIO_NB_CHANNELS)
	long smallSampleDuration, long initialSampleDuration, // the target and the source buffer lengths
	float *smallSampleBuffer, float *initialSampleBuffer, // the target and the source buffers
	long *smallSampleOffset, long *initialSampleOffset, // smallSampleOffset is the position in target small sample
	// offset is the position in source big sample
	long *incay, long *decay,  // fadein from smallSampleOffset to smallSampleOffset + incay
	// fadeout from smallSampleOffset + targetduration - decay to smallSampleOffset + targetduration
	long *sourceduration, long *targetduration, // length of copy from big sample and lenght of copy into small sample
	// if targetduration > sourceduration the sample is repeated as much as needed to fill the
	// target duration
	float selfBlendingRatio);        // ratio of the sample used for overlap to preserve sound continuity in case of 

void generate_all_samples(void);

#endif
