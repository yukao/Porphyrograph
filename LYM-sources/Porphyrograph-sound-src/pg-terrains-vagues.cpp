////////////////////////////////////////////////////////////////////
// 
// 
// 
//     File pg_terrains_vagues.cpp
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
// AUDIO BUFFERS
////////////////////////////////////////////////////////////////////
// moire buffer for playing: the synthesis from the PG_AUDIO_NB_BUFFERS buffers
float *pg_audio_bufferMix = NULL;
SF_INFO pg_audio_sndInfoMix;
// the after last updated frame: local value (inside the buffer)
// and absolute value (from the beginning of streaming)
long pg_audio_local_upperStreamedFrame = 0;
long pg_audio_absolute_upperStreamedFrame = 0;

// micro track size (drone) in frame (micro_track_size comes frome scenario)
long TV_drone_size = long(0.003 * PG_AUDIO_SAMPLE_RATE);

// Structures pointing to sample audio data and used for playing or for swapping
AudioSoundMachine *SoundMachineRhythms = NULL;
// AudioSoundMachine *SoundMachineDrones = NULL;

// currently playing buffer (is a float to allow for fractional
// variations around the current value)
float CurrentlyPlayingBuffer = 0.f;


////////////////////////////////////////////////////////////////////
// AUDIO SAMPLES
////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////
// sample index management for progessive sample turnover
// according to sample swap frequency and duration
// and according to directory size
int IndCurrentScene = -1;
int scene_offset = 0;
int IndRhythmsSampleDir = 0;
int IndPlayedAudioDronesSampleDir = 0;
int IndPlayedAudioRhythmsSample = 0;
// int IndPlayedAudioDronesSample = 0;

std::string StringRhythmsBaseDir = "./SoundData/soundYN-TVW-data/rhythms";
// std::string DronesSampleDir = "./moire/samples/drones";

static int pg_audio_paStreamCallback( const void *inputBuffer, void *outputBuffer,
			     unsigned long framesPerBuffer,
			     const PaStreamCallbackTimeInfo* timeInfo,
			     PaStreamCallbackFlags statusFlags,
			     void *userData );

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
// SOUND FILE PROCESSING
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
// SAMPLE BUFFER AND FILE INFO INITIALIZATIONS
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
// SAMPLE WEIGHTING AROUND THE CURRENT CURSOR
/////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////

inline
long Clamp(long n, long min, long max)
{
	n = n >= max ? max - 1 : n;
	return n<0 ? 0 : n;
}
inline
float fModulo(float val, float min, float max)
{
	float returnVal = val;
	float quot = floor((returnVal - min) / (max - min));
	returnVal -= quot * (max - min);
	return returnVal;
}

// changes buffer according to buffer variation amplitude
// and changes only if the sample of the new buffer is loaded
// returns true iif the buffer has changed
bool AudioSoundMachine::BufferSelection( float *currentBuffer) {
	/////////////////////////////////////////////////////
	// BUFFER CHOICE AFTER RANDOM VARIATION
	// chooses the buffer to be selected at the track loop
	// random variation from current buffer value
	float bufferVariation = TV_track_shuffle * (float)(((double)rand() / (RAND_MAX))-0.5f);
	// printf("randomW %.2f new currW %.2f\n", randomWeight, current_audio_buffer_weight);
	// NON RANDOM CHOICE: TAKES THE NEXT SAMPLE
	bufferVariation = 1;

	float possibleNewBufferValue = *currentBuffer + bufferVariation;
	// has to stay into the proper interval
	while (possibleNewBufferValue >= PG_AUDIO_NB_BUFFERS) {
		possibleNewBufferValue -= PG_AUDIO_NB_BUFFERS;
	}
	while (possibleNewBufferValue < 0) {
		possibleNewBufferValue += PG_AUDIO_NB_BUFFERS;
	}
	// printf("possible new buffer %.2f %d\n", possibleNewBufferValue,int(possibleNewBufferValue));

	// possible new buffer
	if (int(possibleNewBufferValue) != int(*currentBuffer)) {
		// sample number associated with the selected track
		int possibleSampleIndex = audio_buffer_data[int(possibleNewBufferValue)].pg_SampleIndex;
		int possibleDirIndex = audio_buffer_data[int(possibleNewBufferValue)].pg_DirIndex;
		// buffer sizes
		long bufferSize = SampleBufferData[possibleDirIndex][possibleSampleIndex].sampleBufferFrameSize;
		bool loaded = SampleBufferData[possibleDirIndex][possibleSampleIndex].loaded;

		if (loaded && bufferSize > 0) {
			*currentBuffer = possibleNewBufferValue;
			for (int ind = 0; ind < PG_AUDIO_NB_DRONES; ind++) {
				drone_offsets[ind] = bufferSize/PG_AUDIO_NB_DRONES;
			}
			printf("Play buffer %d %s\n", int(*currentBuffer) , SampleBufferData[possibleDirIndex][possibleSampleIndex].name.c_str());
			return true;
		}
		else {
			// does not change buffer value
			// printf("empty new buffer %d\n", int(*currentBuffer));
			return false;
		}
	}
	else {
		// sample number associated with the current track
		int currentSampleIndex = audio_buffer_data[int(*currentBuffer)].pg_SampleIndex;
		int currentDirIndex = audio_buffer_data[int(*currentBuffer)].pg_DirIndex;
		// buffer sizes
		long bufferSize = SampleBufferData[currentDirIndex][currentSampleIndex].sampleBufferFrameSize;
		bool loaded = SampleBufferData[currentDirIndex][currentSampleIndex].loaded;
		if (!loaded || bufferSize <= 0) {
			// emergency procedure to take the first non empty buffer
			for (int indBuf = 0; indBuf < PG_AUDIO_NB_BUFFERS; indBuf++) {
				// sample number associated with the selected track
				int newSampleIndex = audio_buffer_data[indBuf].pg_SampleIndex;
				int newDirIndex = audio_buffer_data[indBuf].pg_DirIndex;
				// buffer sizes
				long bufferSize = SampleBufferData[newDirIndex][newSampleIndex].sampleBufferFrameSize;
				bool loaded = SampleBufferData[newDirIndex][newSampleIndex].loaded;

				if (loaded && bufferSize > 0) {
					*currentBuffer = float(indBuf);
					for (int ind = 0; ind < PG_AUDIO_NB_DRONES; ind++) {
						drone_offsets[ind] = bufferSize / PG_AUDIO_NB_DRONES;
					}
					// printf("new buffer %d\n", int(*currentBuffer));
					return true;
				}
			}
			// emergency procedure to take the first non empty buffer
			// printf("empty buffer %d\n", int(*currentBuffer));
			return false;
		}
	}
	// otherwise we change buffer value
	// but signal that there is no buffer change (integer value remains the same)
	*currentBuffer = possibleNewBufferValue;
	return false;
}

/*
 * This routine is called by portaudio when playback is done.
 */
static void StreamFinished( void* userData ) {
  printf( "Stream Completed\n" );
}

/////////////////////////////////////////////////////////////
// SAMPLE BUFFER MANAGEMENT
/////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////
// PROCESSES SAMPLE SWAPPING

// checks whether loading of buffer indBuffer is completed
bool AudioSoundMachine::swappableLoadingCheckCompleted(int indBuffer) {
	// ongoing swapping: only starts swapping when both buffers are loaded
	// otherwise the audio_buffer_alphaSwap stays null and no blending between
	// the current buffer and the swapped buffer takes place
	if (SampleBufferData[audio_swap_buffer_data[indBuffer].pg_DirIndex][audio_swap_buffer_data[indBuffer].pg_SampleIndex].loading == false
		&& SampleBufferData[audio_swap_buffer_data[indBuffer].pg_DirIndex][audio_swap_buffer_data[indBuffer].pg_SampleIndex].loaded == true
		&& audio_buffer_data[indBuffer].swappable == true) {
		// loading is finished: the swapping can start
		return true;
	}
	else {
		return false;
	}
}
// checks whether swapping of buffer indBuffer is completed
bool AudioSoundMachine::swapUpdatesAndCheckCompleted( int indBuffer ) {
  // ongoing swapping: only starts swapping when both buffers are loaded
  // otherwise the audio_buffer_alphaSwap stays null and no blending between
  // the current buffer and the swapped buffer takes place
  if (SampleBufferData[audio_buffer_data[indBuffer].pg_DirIndex][audio_buffer_data[indBuffer].pg_SampleIndex].loaded == true
	  && SampleBufferData[audio_swap_buffer_data[indBuffer].pg_DirIndex][audio_swap_buffer_data[indBuffer].pg_SampleIndex].loaded == true
	  && audio_buffer_data[indBuffer].swapping == true) {
	  if (audio_buffer_alphaSwap[indBuffer] < 1.0) {
		  // increases alpha coef until it reaches 1.0f
		  audio_buffer_alphaSwap[indBuffer] 
			= min(1.0f, (float)((RealTime() 
			  - audio_buffer_data[indBuffer].swapStart)/
				  audio_buffer_data[indBuffer].swapDuration));
		  //printf("Buffer %d time %.2f alpha %.2f (start %.2f dur %.2f)\n" , 
		  //  indBuffer , RealTime() , audio_buffer_alphaSwap[indBuffer] ,
		  //  audio_buffer_data[indBuffer].swapStart,
		  //  audio_buffer_data[indBuffer].swapDuration );
		  return false;
	  }
	  else {
		  // swapping is finished: the swapping alpha coeficient has reached 1.O
		  return true;
	  }
  }
  else {
	return false;
  }
}

// when loading of buffer indBuffer is completed, launches swapping
void AudioSoundMachine::buffer_swapBuffer_launch(int indBuffer) {
	audio_buffer_data[indBuffer].swappable = false; // no more candidate to start swapping when buffer is loaded
	bufferSwappingInitialization(indBuffer);

	//printf("Single buffer swap initialization (%s) -> swap buffer %d receives index %d\n" , machineName.c_str(), 
	//			indBuffer , audio_swap_buffer_data[indBuffer].pg_SampleIndex );
	// swapping will start as soon as the sample is loaded
	//printf("    Swap sample buf %d (%s -> %s track %d (dur %.2f))\n", indBuffer,
	//	SampleBufferData[audio_swap_buffer_data[indBuffer].pg_DirIndex][audio_swap_buffer_data[indBuffer].pg_SampleIndex].name.c_str(),
	//	SampleBufferData[audio_buffer_data[indBuffer].pg_DirIndex][audio_buffer_data[indBuffer].pg_SampleIndex].name.c_str(),
	//	audio_swap_buffer_data[indBuffer].pg_SampleIndex,
	//	audio_buffer_data[indBuffer].swapDuration);
	printf("    Swap buffer %d %s trk %d\n", indBuffer,
		SampleBufferData[audio_swap_buffer_data[indBuffer].pg_DirIndex][audio_swap_buffer_data[indBuffer].pg_SampleIndex].name.c_str(),
		audio_swap_buffer_data[indBuffer].pg_SampleIndex);
}

// when swapping of buffer indBuffer is completed, stores the data of the swap buffer in the buffer
void AudioSoundMachine::buffer_swapBuffer_exchange(int indBuffer) {
	audio_buffer_data[indBuffer].swapping = false; // makes it so that there is one frame 
	// that displays the full swapped image

	//char * ptr1 = strrchr( SampleBufferData[IndRhythmsSampleDir][audio_swap_buffer_data[indBuffer].pg_SampleIndex].fname , '/' );
	//char * ptr2 = strrchr( SampleBufferData[IndRhythmsSampleDir][audio_buffer_data[indBuffer].pg_SampleIndex].fname , '/' );
	//printf("Sample swap completion (%s -> %s layer %d (dur %.2f))\n" , 
	//  (ptr1? ptr1 + 1: SampleBufferData[IndRhythmsSampleDir][audio_buffer_data[indBuffer].pg_SampleIndex].fname) ,
	//  (ptr2? ptr2 + 1: SampleBufferData[IndRhythmsSampleDir][audio_swap_buffer_data[indBuffer].pg_SampleIndex].fname), 
	//  indBuffer , 
	//  audio_buffer_data[indBuffer].swapDuration );

	// copies the pointer to the swap sample into the buffer
	audio_buffer_data[indBuffer].pg_SampleIndex
		= audio_swap_buffer_data[indBuffer].pg_SampleIndex;
	audio_buffer_data[indBuffer].pg_DirIndex
		= audio_swap_buffer_data[indBuffer].pg_DirIndex;
	//printf("Sample swap completion (%s) -> buffer %d receives index %d\n" , machineName.c_str(), 
	//  indBuffer , audio_swap_buffer_data[indBuffer].pg_SampleIndex );

	// data cooy
	// sound data copy from swap sample buffer to sample buffer
	audio_buffer_data[indBuffer].pg_audio_trackOffset
		= audio_swap_buffer_data[indBuffer].pg_audio_trackOffset;

	// resets swap weight
	audio_buffer_alphaSwap[indBuffer] = 0.0f;

	// resets the swap sample
	audio_swap_buffer_data[indBuffer].reset();
}

//////////////////////////////////////////////////////////////////
// PROCESSES SAMPLE SWAPPING

// updates the swapping weights from 0 to 1 until the
// swapped image is visible
// if it reaches 1, the image contents are swapped and 
// the swap image is freed
void AudioSoundMachine::update_sample_buffer_swapping (void) {
	for (int indBuffer = 0; indBuffer < PG_AUDIO_NB_BUFFERS; indBuffer++) {
		if (swappableLoadingCheckCompleted(indBuffer)) {
			buffer_swapBuffer_launch(indBuffer);
			// printf("launch swap buffer %d\n", indBuffer);
		}
		if (swapUpdatesAndCheckCompleted(indBuffer)) {
			buffer_swapBuffer_exchange(indBuffer);
			// printf("exchange swap buffer %d\n", indBuffer);
		}
	}
}

//////////////////////////////////////////////////////////////////
// LOOKS FOR AVAILABLE SWAP SAMPLE

// tries to find a sample that is not pointed to by
// a buffer or a swap buffer
// returns available index if not returns negtive
int AudioSoundMachine::available_sample_buffer(void) {
  // starts from a random index
  int indStart = int(((double)rand() / (RAND_MAX)) * PG_AUDIO_NB_SAMPLES);
  for (int indSample = 0; indSample < PG_AUDIO_NB_SAMPLES; indSample++) {
	  int curInd = (indSample + indStart) % PG_AUDIO_NB_SAMPLES;
	  bool found = false;
	  for (int indBuffer = 0; indBuffer < PG_AUDIO_NB_BUFFERS; indBuffer++) {
		  if (audio_buffer_data[indBuffer].pg_SampleIndex == curInd
			  || audio_swap_buffer_data[indBuffer].pg_SampleIndex == curInd ) {
			  found = true;
			  break;
		  }
	  }
	  if( !found ) {
	      return curInd;
	  }
  }
  return -1;
}


//////////////////////////////////////////////////////////////////
// LOADS BUFFER IMAGE FROM FILE

// looks whether the sample is already loaded into SampleBufferData
// if not has to be loaded 
// returns the index into SampleBufferData of the considered audio buffer
int AudioSoundMachine::SampleBufferDataIndex(std::string *fileName) {
  for (int indSample = 0; indSample < PG_AUDIO_NB_SAMPLES; indSample++) {
    if( fileName->compare(SampleBufferData[IndRhythmsSampleDir][indSample].name ) == 0 ) {
		return indSample;
    }
  }
  return -1;
}

// threaded upload of the free swap image from a randomly chosen available layer
//algorithm:
//(1) associate each buffer sample with 2 integers: dir_index and file_index
//(2) load_and_swap_sample:
//   - first check whether the sample dir_index and file_index is already loaded
//   - if not look for a buffer that is not played and replace it with the sample dir_index and file_index
//   - use the same algorithm as for  "sample_alltracks" to change the sample ID of all the tracks to the
//     currently loaded sample

void AudioSoundMachine::bufferSwappingInitialization( int indBuffer ) {
	//printf("Buffer swap initialization (%s) -> swap buffer %d receives index %d\n" , machineName.c_str(), 
	//		indBuffer , audio_swap_buffer_data[indBuffer].pg_SampleIndex );
	audio_buffer_data[indBuffer].swapStart = RealTime();
	audio_buffer_data[indBuffer].swappable = false;
	audio_buffer_data[indBuffer].swapping = true;
	audio_buffer_alphaSwap[indBuffer] = 0.0f;
}

// threaded upload of a free swap sample dans storage in all the buffers
// loads an audio sample file (if a non played sample is available)
// and returns the pointer to the sample
int AudioSoundMachine::pg_threaded_load_sample(std::string *fileName) {
	int indexSample;
	// printf("Looking for sample index %s\n", fileName->c_str());
	// if the sample is not yet loaded, it is loaded by a thread
	if ((indexSample = SampleBufferDataIndex(fileName)) < 0) {
		// printf("Sample index %d\n", indexSample);
		// an available sample is found, threaded upload of the sample
		// a sample is available if no buffer uses it
		if ((indexSample = available_sample_buffer()) >= 0) {
			// deallocates the sample and closes the 
			SampleBufferData[IndRhythmsSampleDir][indexSample].release();
			SampleBufferData[IndRhythmsSampleDir][indexSample].name = *fileName;
			delete fileName;
			int formatSize;
			if (!(formatSize = SampleBufferData[IndRhythmsSampleDir][indexSample].pg_openSample())) {
				printf("File opening failure.\n");
				return -1;
			}

			// and load them
			SampleBufferData[IndRhythmsSampleDir][indexSample].loading = true;
			SampleBufferData[IndRhythmsSampleDir][indexSample].loaded = false;
#ifdef WIN32
			DWORD rc;
			HANDLE  hThread = CreateThread(
				NULL,         // default security attributes
				0,            // use default stack size  
				pg_threadedReadShortSampleBuffer,  // thread function name
				(void *)&(SampleBufferData[IndRhythmsSampleDir][indexSample]),// argument to thread function 
				0,            // use default creation flags 
				&rc);   // returns the thread identifier 
			if (hThread == NULL){
				std::cout << "Error:unable to create thread pg_threadedReadShortSampleBuffer for sample" << std::endl;
				// exit(-1);
				pg_readShortSampleBuffer((void *)&(SampleBufferData[IndRhythmsSampleDir][indexSample]));
			}
			CloseHandle(hThread);
#else
			pthread_t readWavData_thread;
			int rc;
			rc = pthread_create(&pg_threadedReadShortSampleBuffer, NULL,
				pg_threadedReadShortSampleBuffer, (void *)&(SampleBufferData[IndRhythmsSampleDir][indexSample]));
			if (rc){
				std::cout << "Error:unable to create thread pg_threadedReadShortSampleBuffer for sample" << rc << std::endl;
				pthread_exit(NULL);
			}
#endif
			return indexSample;
		}
		else {
			printf("Sample upload not possible (all samples are currently used)\n");
			delete fileName;
			return -1;
		}
	}
	else {
		printf("Sample index %d\n", indexSample);
		// they are already loaded
		SampleBufferData[IndRhythmsSampleDir][indexSample].loading = false;
		SampleBufferData[IndRhythmsSampleDir][indexSample].loaded = true;
		delete fileName;
		return indexSample;
	}
	delete fileName;
	return -1;
}


// threaded upload of a free swap sample ans storage into a single buffer
bool AudioSoundMachine::load_and_swap_audio_buffer(std::string *fileName, double duration, int indBuffer) {
	int indexSample;
	// if the sample is loaded an its index  >= 0 returned
	if ((indexSample = pg_threaded_load_sample(fileName)) >= 0) {
		audio_buffer_data[indBuffer].swappable = true;
		audio_buffer_data[indBuffer].swapDuration = duration;
		audio_swap_buffer_data[indBuffer].pg_SampleIndex = indexSample;
		audio_swap_buffer_data[indBuffer].pg_DirIndex = IndRhythmsSampleDir;
		return true;
	}
	else {
		return false;
	}
}

// threaded upload of a free swap sample ans storage into a single buffer
bool AudioSoundMachine::swap_audio_buffer(int indSample, double duration, int indBuffer) {
	// if the sample is loaded an its index  >= 0 returned
	audio_buffer_data[indBuffer].swappable = true;
	audio_buffer_data[indBuffer].swapDuration = duration;
	audio_swap_buffer_data[indBuffer].pg_SampleIndex = indSample;
	audio_swap_buffer_data[indBuffer].pg_DirIndex = IndRhythmsSampleDir;
	return true;
}

/////////////////////////////////////////////////////////
// INITIAL SAMPLE UPLOADING
/////////////////////////////////////////////////////////
bool AudioSoundMachine::pg_ReadInitalSampleBuffers(int ind_dir) {
	// printf("Image dir %d # %d dur %.2f\n" , dir_index, file_index, duration);
	std::string *fileName;

	bool valret = true;
	if (machineName.compare("Rhythms") == 0) {
	  // IndRhythmsSampleDir = 0;
		for (int indAudioSampleFile = 0; indAudioSampleFile < PG_AUDIO_NB_SAMPLES;
		   indAudioSampleFile++) {
		  int indFile = indAudioSampleFile;
		  IndRhythmsSampleDir = ind_dir;
		  if ((fileName = nextFile(&StringRhythmsBaseDir, &IndRhythmsSampleDir, &indFile))) {
			  // reads the sound file & stores it in the swap buffer data
			  SampleBufferData[IndRhythmsSampleDir][indAudioSampleFile].name = *fileName;
			  int formatSize;
			  if (!(formatSize = SampleBufferData[IndRhythmsSampleDir][indAudioSampleFile].pg_openSample())) {
				  printf("File opening failure.\n");
				  valret = false;
			  }
			  // and load them
			  pg_readShortSampleBuffer((void *)&(SampleBufferData[IndRhythmsSampleDir][indAudioSampleFile]));
			  delete fileName;
		  }
	   }
	}
	//if (machineName.compare("Drones") == 0) {
	//  IndPlayedAudioDronesSampleDir = 0;
	//  for (int indAudioSampleFile = 0; indAudioSampleFile < PG_AUDIO_NB_SAMPLES; 
	//	   indAudioSampleFile++) {
	//	  int indFile = indAudioSampleFile;
	//	  if ((fileName = nextFile(&DronesSampleDir, &IndPlayedAudioDronesSampleDir, &indFile))) {
	//		  // reads the sound file & stores it in the swap buffer data
	//		  strcpy(SampleBufferData[IndRhythmsSampleDir][indAudioSampleFile].fname, fileName->c_str());
	//		  int formatSize;
	//		  if (!(formatSize = SampleBufferData[IndRhythmsSampleDir][indAudioSampleFile].pg_openSample())) {
	//			  printf("File opening failure.\n");
	//			  valret = false;
	//		  }
	//		  // and load them
	//		  pg_readShortSampleBuffer((void *)&(SampleBufferData[IndRhythmsSampleDir][indAudioSampleFile]));
	//		  delete fileName;
	//	  }
	//  }
	//  // global variable for sample reading
	//  IndPlayedAudioDronesSample = PG_AUDIO_NB_SAMPLES;
	//}

	/////////////////////////////////////////////////////
	// first play the first PG_AUDIO_NB_BUFFERS samples
	for (int indBuffer = 0; indBuffer< PG_AUDIO_NB_BUFFERS; indBuffer++) {
		// generates a random number between 0 and current buffer length
		// minus bufferMoireLength
		audio_buffer_data[indBuffer].pg_SampleIndex = indBuffer;
		audio_buffer_data[indBuffer].pg_DirIndex = IndRhythmsSampleDir;
	}

	return valret;
}

