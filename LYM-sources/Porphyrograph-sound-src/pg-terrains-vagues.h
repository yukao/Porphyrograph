////////////////////////////////////////////////////////////////////
// 
// 
//     File pg_trarains_vagues.h
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

#ifndef PG_TERRAINS_VAGUES_H
#define PG_TERRAINS_VAGUES_H

////////////////////////////////////////////////////////////////////
// PORTAUDIO STREAMING
////////////////////////////////////////////////////////////////////
#define PG_AUDIO_NB_DRONES                  (6)

#define PG_AUDIO_MAX_TRACK_SIZE             long(3*PG_AUDIO_SAMPLE_RATE)

#ifdef PG_TERRAINS_VAGUES
#define PG_LOOP_SIZE                        (8)  // number of beats for a loop
#endif

///////////////////////////////////////////////////////
// AUDIO DATA STRUCTURE
///////////////////////////////////////////////////////

class AudioBufferDataStruct {
public:
	// sample number for each audio track
	int pg_SampleIndex;
	int pg_DirIndex;
	// buffer swap management
	bool swapping;   // currently swapping between audio_buffer_data & audio_swap_buffer_data
	bool swappable;  // candidate to swap when SampleBufferData[indexSample].loaded == true
	double swapStart;   // date for swapping start
	double swapDuration;   // duration planned for swapping
	// audio tracks offsets
	long pg_audio_trackOffset;
	AudioBufferDataStruct(void) {
		pg_SampleIndex = 0;
		pg_DirIndex = 0;
		swapping = false;
		swappable = false;
		swapStart = 0.0;
		swapDuration = 1.0;
		pg_audio_trackOffset = 0;
	}
	void reset(void) {
		pg_SampleIndex = 0;
		pg_DirIndex = 0;
		swapping = false;
		swappable = false;
		swapStart = 0.0;
		swapDuration = 1.0;
		// pg_audio_trackOffset = 0;
	}
	~AudioBufferDataStruct(void) {
	}
};

////////////////////////////////////////////////////////////////////
// AUDIO BUFFERS
////////////////////////////////////////////////////////////////////
// Structures pointing to sample audio data and used for playing or for swapping
// #define PG_AUDIO_NB_BUFFERS                 (6)
// moire buffer for playing: the synthesis from the PG_AUDIO_NB_BUFFERS buffers
extern float *pg_audio_bufferMix;
extern SF_INFO pg_audio_sndInfoMix;

class AudioSoundMachine {
public:
	string machineName;

	// Structures pointing to sample audio data and used for playing or for swapping
	AudioBufferDataStruct audio_buffer_data[PG_AUDIO_NB_BUFFERS];
	AudioBufferDataStruct audio_swap_buffer_data[PG_AUDIO_NB_BUFFERS];
	// last buffer loaded, to load in turn successive buffers
	int last_loaded_buffer;
	// interpolation weight between audio buffer swap buffer in each layer
	float *audio_buffer_alphaSwap;

	// Samples used for loading sample files and storing the corresponding audio data
	AudioSampleDataStruct SampleBufferData[PG_AUDIO_NB_DIRS][PG_AUDIO_NB_SAMPLES];

	// drone offsets for drone computation associated with sample
	long drone_offsets[PG_AUDIO_NB_DRONES];

	AudioSoundMachine(string name) {
		machineName = name;

		audio_buffer_alphaSwap = new float[PG_AUDIO_NB_BUFFERS];
		for (int ind = 0; ind < PG_AUDIO_NB_BUFFERS; ind++) {
			audio_buffer_alphaSwap[ind] = 0.f;
		}
		last_loaded_buffer = -1;

		for (int indDir = 0; indDir < PG_AUDIO_NB_DIRS; indDir++) {
			for (int ind = 0; ind < PG_AUDIO_NB_SAMPLES; ind++) {
				SampleBufferData[indDir][ind] = AudioSampleDataStruct();
			}
		}

		for (int ind = 0; ind < PG_AUDIO_NB_DRONES; ind++) {
			drone_offsets[ind] = 0;
		}

		for (int indBuffer = 0; indBuffer < PG_AUDIO_NB_BUFFERS; indBuffer++) {
			audio_buffer_data[indBuffer].pg_audio_trackOffset = 0;
		}

		if (sizeof(float) != PG_AUDIO_BYTES_PER_SAMPLE) {
			printf("Float integer size (%d bytes) is not compatible with port audio paFloat32 format (4bytes), code will not be valid on this architecture\n", (int)sizeof(float));
			exit(0);
		}
	}
	~AudioSoundMachine(void) {
		// deallocation
		last_loaded_buffer = -1;
		for (int indDir = 0; indDir < PG_AUDIO_NB_DIRS; indDir++) {
			for (int ind = 0; ind < PG_AUDIO_NB_SAMPLES; ind++) {
				if (!SampleBufferData[indDir][ind].name.empty()) {
					SampleBufferData[indDir][ind].release();
				}
			}
		}
	}
	bool pg_ReadInitalSampleBuffers(int ind_dir);
	bool BufferSelection(float *currentBuffer);

	bool swappableLoadingCheckCompleted(int indBuffer);
	void buffer_swapBuffer_launch(int indBuffer);

	bool swapUpdatesAndCheckCompleted(int indBuffer);
	void buffer_swapBuffer_exchange(int indBuffer);

	void update_sample_buffer_swapping(void);
	void bufferSwappingInitialization(int indBuffer);
	bool load_and_swap_audio_buffer(std::string *fileName, double duration, int indBuffer);
	bool swap_audio_buffer(int ind_buffer, double duration, int indBuffer);
	int SampleBufferDataIndex(std::string *fileName);
	int available_sample_buffer(void);
	int pg_threaded_load_sample(std::string *fileName);
};

// extern AudioSoundMachine *SoundMachineDrones;
extern AudioSoundMachine *SoundMachineRhythms;

// currently playing track
extern float CurrentlyPlayingBuffer;

// micro track size (drone) in frame (track_size comes frome scenario)
extern long TV_drone_size;

////////////////////////////////////////////////////////////////////
// AUDIO SAMPLES
////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////
// sample index management for progessive sample turnover
// according to sample swap frequency and duration
// and according to directory size
extern int IndCurrentScene;
extern int scene_offset;
extern int IndRhythmsSampleDir;
// extern int IndPlayedAudioDronesSampleDir;
extern int IndPlayedAudioRhythmsSample;
// extern int IndPlayedAudioDronesSample;

extern std::string StringRhythmsBaseDir;
// extern std::string DronesSampleDir;

#endif
