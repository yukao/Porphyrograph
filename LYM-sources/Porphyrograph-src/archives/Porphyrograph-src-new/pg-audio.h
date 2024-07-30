/*! \file pg-update.h
 *
 *
 *     File pg-update.h
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

/*******************************************************************/
extern pa_AudioOut pa_sound_data;
extern ScopedPaHandler* paInit;
extern callback_data_s soundfile_data;
extern PaStream* pa_stream;


void pg_listAllSoundtracks(void);

void PlayTrack(int trackNo, double timeFromStart);
void StopTrack(void);
void updatePeakOrOnset(double timeFromBeginning, vector<float>* peaks, vector<float>* onsets,
    bool* is_new_peak, bool* is_new_onset, int nbPeaks, int nbOnsets, int* curPeakIndex, int* curOnsetIndex);
// MOVIE STREAMING JUMPS
void pg_movie_forward(int nb_frames_forth);
void pg_movie_backward(int nb_frames_back);
// MOVIE AND CAMERA FRAME CAPTURE
void pg_update_clip_camera_and_movie_frame(void);
void soundTrackonOff();
void soundTrackvolume(float vol);
void pg_checkAudioStream();


#endif