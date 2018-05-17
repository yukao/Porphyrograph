////////////////////////////////////////////////////////////////////
// 
// 
//     File pg-main.h.cpp
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

#ifndef PG_MAIN_H
#define PG_MAIN_H

////////////////////////////////////////////////////////////////////
// PORTAUDIO STREAMING
////////////////////////////////////////////////////////////////////
#define PG_AUDIO_SAMPLE_RATE                (44100)
#define PG_AUDIO_NB_CHANNELS                (2)
#define PG_AUDIO_BYTES_PER_SAMPLE           (sizeof(float))
#define PG_AUDIO_SAMPLE_FORMAT              (paFloat32)

#define PG_AUDIO_MOIRE_SIZE                 long(6*PG_AUDIO_SAMPLE_RATE)

#define PG_AUDIO_NB_SAMPLES                 (16)
#define PG_AUDIO_NB_DIRS                    (28)

#define PG_FFT_SIZE                         (64)

inline long DurationToFrames(double time)
{
	return long(time * PG_AUDIO_SAMPLE_RATE);
}
inline double FramesToDuration(double frames)
{
	return double(frames / PG_AUDIO_SAMPLE_RATE);
}

/// Error string
extern char *ErrorStr;

/// Auxiliary string
extern char AuxString[1024];

// date string for output files
extern std::stringstream  date_stringStream;

// boolean for recording the audio output
extern bool record_output;

////////////////////////////////////////////////////////////////////
// UDP PROCESSING THREAD
extern HANDLE  hThread;

////////////////////////////////////////////////////////////////////
// initial time to manage synchronization and  beats
extern double StreamingRealTime;  // the outer streaming output realtime
extern double LastPorphyrographRealTime; // last synchronization between the streamed data and the application
extern double LastStreamingBeatRealTime;  // the time when last beat occurred realtime for the sound car
extern double InitialPerformanceTime;   // the starting time for beats
#ifdef PG_TERRAINS_VAGUES
extern double InitialTerrainVaguesTime;   // the starting time for playing terrains vagues samples
#endif
extern double BeatDurationRealTime;  // the duration of a beat realtime
extern unsigned long NbStreamingBeats; // beats in sound card real time
extern unsigned long NbPorphyrographBeats; // beats in application real time

////////////////////////////////////////////////////////////////////
// terrains vagues samples control
extern float TV_track_shuffle;
extern float TV_drone_mix;
extern float TV_swap_freq;
extern float TV_swap_dur;

/////////////////////////////////////////////////////////////////////
// terrains vagues blast management
extern bool TV_blash_on;
extern float *seqLevels;
extern float track0Level;


////////////////////////////////////////////////////////////////////
// MAIN FUNCTIONS
////////////////////////////////////////////////////////////////////
void pg_audio_deallocation(void);
#ifdef WIN32
DWORD WINAPI processUDP(LPVOID lpParam);
#else
void* processUDP(void * lpParam);
#endif
void pg_aliasScript(char *command_symbol,
					char* string_argument_0 ,
					float arguments[MAX_OSC_ARGUMENTS] );

#endif
