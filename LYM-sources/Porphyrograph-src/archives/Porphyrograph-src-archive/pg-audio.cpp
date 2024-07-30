/*! \file pg-audio.cpp
 *
 *
 *     File pg-audio.cpp
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
 /* based on portaudio source code */

#include "pg-all_include.h"

/*
 * $Id: paex_sine.c 1752 2011-09-08 03:21:55Z philburk $
 *
 * This program uses the PortAudio Portable Audio Library.
 * For more information see: http://www.portaudio.com/
 * Copyright (c) 1999-2000 Ross Bencina and Phil Burk
 *
 */

pa_AudioOut pa_sound_data;
ScopedPaHandler *paInit;
callback_data_s soundfile_data;

pa_AudioOut::pa_AudioOut() : pa_myStream(0), left_phase(0), right_phase(0)
{
    strcpy(pa_message, "No Message");
    is_streaming = false;
}

bool pa_AudioOut::pa_openMyStream(PaDeviceIndex index, int *channelCount, int *sampleRate)
{
    PaStreamParameters outputParameters;

    outputParameters.device = index;
    if (outputParameters.device == paNoDevice) {
        return false;
    }

    const PaDeviceInfo* pInfo = Pa_GetDeviceInfo(index);
    if (pInfo != 0)
    {
        printf("Output device name: '%s'\n", pInfo->name);
    }

    outputParameters.channelCount = soundfile_data.sound_file_info.channels;
    *channelCount = outputParameters.channelCount;
    *sampleRate = soundfile_data.sound_file_info.samplerate;
    outputParameters.sampleFormat = paFloat32; /* 32 bit floating point output */
    outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;

    PaError err = Pa_OpenStream(
        &pa_myStream,
        NULL, /* no input */
        &outputParameters,
        soundfile_data.sound_file_info.samplerate,
        FRAMES_PER_BUFFER,
        paClipOff,      /* we won't output out of range samples so don't bother clipping them */
        &pa_AudioOut::paCallback,
        &soundfile_data            /* Using 'this' for userData so we can cast to pa_AudioOut* in paCallback method */
    );

    if (err != paNoError)
    {
        printf("Failed to open stream to device\n");
        return false;
    }

    err = Pa_SetStreamFinishedCallback(pa_myStream, &pa_AudioOut::paStreamFinished);

    if (err != paNoError)
    {
        Pa_CloseStream(pa_myStream);
        pa_myStream = 0;

        return false;
    }

    is_streaming = true;
    return true;
}

bool pa_AudioOut::pa_closeMyStream()
{
    if (pa_myStream == 0)
        return false;

    PaError err = Pa_CloseStream(pa_myStream);
    is_streaming = false;

    return (err == paNoError);
}


bool pa_AudioOut::pa_startMyStream()
{
    if (pa_myStream == 0)
        return false;

    PaError err = Pa_StartStream(pa_myStream);

    is_streaming = true;
    return (err == paNoError);
}

bool pa_AudioOut::pa_stopMyStream()
{
    if (pa_myStream == 0)
        return false;

    PaError err = Pa_StopStream(pa_myStream);

    is_streaming = false;
    return (err == paNoError);
}

int pa_AudioOut::paCallback(const void* inputBuffer, void* outputBuffer,
    unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void* userData)
{
    float* out;
    callback_data_s* p_data = (callback_data_s*)userData;
    sf_count_t       num_read;

    out = (float*)outputBuffer;
    p_data = (callback_data_s*)userData;

    /* clear output buffer */
    memset(out, 0, sizeof(float) * framesPerBuffer * p_data->sound_file_info.channels);

    /* read directly into output buffer */
    num_read = sf_read_float(p_data->sound_file, out, framesPerBuffer * p_data->sound_file_info.channels);
    for (int ind = 0; ind < num_read; ind++) {
        out[ind] *= soundtrack_PA_weight;
    }

    /*  If we couldn't read a full frameCount of samples we've reached EOF */
    if (num_read < framesPerBuffer)
    {
        return paComplete;
    }

    return paContinue;
}

bool pa_AudioOut::pa_checkAudioStream() {
    if (is_streaming) {
        // playing
        if (Pa_IsStreamActive(pa_myStream) > 0) {
            return true;
        }
        // not playing
        else if (Pa_IsStreamActive(pa_myStream) == 0) {
            printf("Wav file streaming finished\n");
            is_streaming = false;
            return false;
        }
        // error
        else if (Pa_IsStreamActive(pa_myStream) < 0) {
            printf("Error paBadStreamPtr %d\n", paBadStreamPtr);
            sprintf(ErrorStr, "Error audio streaming %d", Pa_IsStreamActive(pa_myStream)); ReportError(ErrorStr); throw 561;
        }
    }
    return false;
}

bool pa_AudioOut::pa_is_streaming() {
    return is_streaming;
}


void pa_AudioOut::paStreamFinishedMethod()
{
    is_streaming = false;
    printf("Stream Completed: at %f\n", RealTime() - soundfile_data.sound_file_StartReadingTime);
 }

/*
    * This routine is called by portaudio when playback is done.
    */
void pa_AudioOut::paStreamFinished(void* userData)
{
    return ((pa_AudioOut*)userData)->paStreamFinishedMethod();
}
