/*! \file pg-audio.h
 *
 *
 *     File pg-audio.h
 *
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#ifndef PG_AUDIO_H
#define PG_AUDIO_H

#include <stdio.h>
#include <math.h>
#include "sndfile.h"
#include "portaudio.h"

#define NUM_SECONDS   (5)
#define SAMPLE_RATE   (44100)
#define FRAMES_PER_BUFFER  (64)

#ifndef M_PI
#define M_PI  (3.14159265)
#endif

class pa_AudioOut
{
public:
    pa_AudioOut();
    bool pa_openMyStream(PaDeviceIndex index, int* channelCount, int* sampleRate);
    bool pa_closeMyStream();
    bool pa_startMyStream();
    bool pa_stopMyStream();
    bool pa_checkAudioStream();
    bool pa_is_streaming();

private:
    PaStream* pa_myStream;
    int left_phase;
    int right_phase;
    char pa_message[512];
    bool is_streaming;

    ///* The instance callback, where we have access to every method/variable in object of class pa_AudioOut */
    //int paCallbackMethod(const void* inputBuffer, void* outputBuffer,
    //    unsigned long framesPerBuffer,
    //    const PaStreamCallbackTimeInfo* timeInfo,
    //    PaStreamCallbackFlags statusFlags);

    /* This routine will be called by the PortAudio engine when audio is needed.
    ** It may called at interrupt level on some machines so don't do anything
    ** that could mess up the system like calling malloc() or free().
    */
    static int paCallback(const void* inputBuffer, void* outputBuffer,
        unsigned long framesPerBuffer,
        const PaStreamCallbackTimeInfo* timeInfo,
        PaStreamCallbackFlags statusFlags,
        void* userData);


    void paStreamFinishedMethod();

    /*
     * This routine is called by portaudio when playback is done.
     */
    static void paStreamFinished(void* userData);
};

class ScopedPaHandler
{
public:
    ScopedPaHandler()
        : _result(Pa_Initialize())
    {
    }
    ~ScopedPaHandler()
    {
        if (_result == paNoError)
        {
            Pa_Terminate();
        }
    }

    PaError result() const { return _result; }

private:
    PaError _result;
};

typedef struct
{
    SNDFILE* sound_file;
    SF_INFO               sound_file_info;
    double                sound_file_StartReadingTime;
} callback_data_s;

// sound track playing
extern int pg_currentlyPlaying_trackNo;
extern bool pg_soundTrack_on;

// movie soundtrack passes over an onset or a peak before next frame
extern bool pg_track_sound_onset;
extern bool pg_track_sound_peak;

// soundtracks
class SoundTrack {
public:
    string soundtrackFileName;
    string soundtrackShortName;
    string soundtrackPeaksFileName;
    string soundtrackOnsetsFileName;
    float soundtrackOnsetsAndPeasksOffset;
    vector<float> soundtrackPeaks;
    vector<float> soundtrackOnsets;
    SoundTrack(void) {
        soundtrackFileName = "";
        soundtrackShortName = "";
        soundtrackPeaksFileName = "";
        soundtrackOnsetsFileName = "";
        soundtrackOnsetsAndPeasksOffset = 0.f;
    }
    ~SoundTrack(void) {
    }
};
extern vector<SoundTrack> pg_SoundTracks[PG_MAX_SCENARIOS];
extern int pg_currentTrackSoundPeakIndex;
extern int pg_nbTrackSoundPeakIndex[PG_MAX_SCENARIOS];
extern int pg_currentTrackSoundOnsetIndex;
extern int pg_nbTrackSoundOnsetIndex[PG_MAX_SCENARIOS];

/*******************************************************************/
void pg_listAllSoundtracks(void);

void PlayTrack(int trackNo, double timeFromStart);
void StopTrack(void);

void soundTrackonOff();
void soundTrackvolume(float vol);
void pg_checkAudioStream();
void pg_pa_closeStream(void);
void pg_pa_openSoundData(void);

#endif