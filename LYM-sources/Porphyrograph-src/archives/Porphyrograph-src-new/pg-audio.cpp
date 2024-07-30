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
ScopedPaHandler* paInit;
callback_data_s soundfile_data;

// sound track playing
int currentlyPlaying_trackNo = -1;
bool soundTrack_on = true;

// movie soundtrack passes over an onset or a peak before next frame
bool pg_track_sound_onset = false;
bool pg_track_sound_peak = false;


pa_AudioOut::pa_AudioOut() : pa_myStream(0), left_phase(0), right_phase(0)
{
    strcpy(pa_message, "No Message");
    is_streaming = false;
}

bool pa_AudioOut::pa_openMyStream(PaDeviceIndex index, int* channelCount, int* sampleRate)
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

void pg_listAllSoundtracks(void) {
    printf("Listing Soundtracks:\n");
    for (int indConfiguration = 0; indConfiguration < pg_NbConfigurations; indConfiguration++) {
        std::cout << "    " << indConfiguration << ": ";
        for (SoundTrack& soundtrack : pg_SoundTracks[indConfiguration]) {
            std::cout << soundtrack.soundtrackFileName << " (" << soundtrack.soundtrackShortName << "), ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void PlayTrack(int indTrack, double timeFromStart) {
    if (!pg_SoundTracks[pg_current_configuration_rank].empty()) {
        bool new_track = (indTrack >= 0 && currentlyPlaying_trackNo != indTrack % pg_SoundTracks[pg_current_configuration_rank].size());
        //std::cout << "new track ind: " << indTrack << " currentlyPlaying_trackNo:" << currentlyPlaying_trackNo << " nb of tracks:" << 
        // pg_SoundTracks[pg_current_configuration_rank].size() << std::endl;

        if (new_track) {
            // std::cout << "cwd: " << cwd << std::endl;
            int previouslyPlayingSoundtrackNo = currentlyPlaying_trackNo;
            currentlyPlaying_trackNo = indTrack % pg_SoundTracks[pg_current_configuration_rank].size();
            std::cout << "Playing soundtrack name: " << pg_SoundTracks[pg_current_configuration_rank][currentlyPlaying_trackNo].soundtrackFileName << std::endl;
            sprintf(AuxString, "/soundtrack_fileName %s", pg_SoundTracks[pg_current_configuration_rank][currentlyPlaying_trackNo].soundtrackFileName.c_str());

            // PureData play
            pg_send_message_udp((char*)"s", AuxString, (char*)"udp_PD_send");

            // portaudio play
            if ((previouslyPlayingSoundtrackNo >= 0 && previouslyPlayingSoundtrackNo < int(pg_SoundTracks[pg_current_configuration_rank].size()))
                || pa_sound_data.pa_is_streaming()) {
                printf("Closing stream (%s)\n", (char*)(pg_SoundTracks[pg_current_configuration_rank][previouslyPlayingSoundtrackNo].soundtrackFileName).c_str());
                pa_sound_data.pa_stopMyStream();
                pa_sound_data.pa_closeMyStream();
            }
            printf("Opening Wav file (%s)\n", (char*)(pg_SoundTracks[pg_current_configuration_rank][currentlyPlaying_trackNo].soundtrackFileName).c_str());
            /* Open the soundfile */
            soundfile_data.sound_file = sf_open((char*)(pg_SoundTracks[pg_current_configuration_rank][currentlyPlaying_trackNo].soundtrackFileName).c_str(),
                SFM_READ, &soundfile_data.sound_file_info);
            if (sf_error(soundfile_data.sound_file) != SF_ERR_NO_ERROR) {
                fprintf(stderr, "%s\n", sf_strerror(soundfile_data.sound_file));
                sprintf(ErrorStr, "Wav file not opened (%s)!", (char*)(pg_SoundTracks[pg_current_configuration_rank][currentlyPlaying_trackNo].soundtrackFileName).c_str()); ReportError(ErrorStr);
                return;
            }

            // opening stream
            int channelCount;
            int sampleRate;
            if (pa_sound_data.pa_openMyStream(Pa_GetDefaultOutputDevice(), &channelCount, &sampleRate)) {
                printf("portaudio stream opened %d ch at %d Hz\n", channelCount, sampleRate);
            }
            else {
                fprintf(stderr, "Error number: %d\n", paInit->result());
                fprintf(stderr, "Error message: %s\n", Pa_GetErrorText(paInit->result()));
                sprintf(ErrorStr, "Port audio stream not opened!"); ReportError(ErrorStr); throw 100;
            }

            // starting stream
            if (pa_sound_data.pa_startMyStream()) {
                printf("portaudio stream started\n");
            }
            else {
                fprintf(stderr, "Error number: %d\n", paInit->result());
                fprintf(stderr, "Error message: %s\n", Pa_GetErrorText(paInit->result()));
                sprintf(ErrorStr, "Port audio stream not started!"); ReportError(ErrorStr); throw 100;
            }

            pg_track_sound_onset = false;
            pg_track_sound_peak = false;
            currentTrackSoundPeakIndex = 0;
            currentTrackSoundOnsetIndex = 0;

            if (currentlyPlaying_trackNo >= 0
                && int(pg_SoundTracks[pg_current_configuration_rank].size()) > currentlyPlaying_trackNo) {
                nbTrackSoundPeakIndex[pg_current_configuration_rank]
                    = pg_SoundTracks[pg_current_configuration_rank][currentlyPlaying_trackNo].soundtrackPeaks.size();
            }
            else {
                nbTrackSoundPeakIndex[pg_current_configuration_rank] = 0;
            }
            if (currentlyPlaying_trackNo >= 0
                && int(pg_SoundTracks[pg_current_configuration_rank].size()) > currentlyPlaying_trackNo) {
                nbTrackSoundOnsetIndex[pg_current_configuration_rank]
                    = pg_SoundTracks[pg_current_configuration_rank][currentlyPlaying_trackNo].soundtrackOnsets.size();
            }
            else {
                nbTrackSoundOnsetIndex[pg_current_configuration_rank] = 0;
            }
            printf("nbTrackSoundPeakIndex %d nbTrackSoundOnsetIndex %d\n",
                nbTrackSoundPeakIndex[pg_current_configuration_rank], nbTrackSoundOnsetIndex[pg_current_configuration_rank]);

        }

        // start position seeking
        if (timeFromStart > 0) {
            if (sf_seek(soundfile_data.sound_file, sf_count_t(timeFromStart * soundfile_data.sound_file_info.samplerate), SEEK_SET) == -1) {
                sprintf(ErrorStr, "Seek error in file (%s)!", (char*)(pg_SoundTracks[pg_current_configuration_rank][currentlyPlaying_trackNo].soundtrackFileName).c_str()); ReportError(ErrorStr);
            }
            soundfile_data.sound_file_StartReadingTime
                = RealTime() - pg_SoundTracks[pg_current_configuration_rank][currentlyPlaying_trackNo].soundtrackOnsetsAndPeasksOffset - timeFromStart;
        }
        else {
            soundfile_data.sound_file_StartReadingTime
                = RealTime() - pg_SoundTracks[pg_current_configuration_rank][currentlyPlaying_trackNo].soundtrackOnsetsAndPeasksOffset;
        }

#if defined(PG_WITH_JUCE)
        sprintf(AuxString, "/JUCE_open_track \"%s\"",
            (cwd + soundtracks_directory + pg_SoundTracks[currentlyPlaying_trackNo]->soundtrackFileName).c_str());
        pg_send_message_udp((char*)"s", AuxString, (char*)"udp_SoundJUCE_send");
        pg_send_message_udp((char*)"", (char*)"/JUCE_play_track", (char*)"udp_SoundJUCE_send");
#endif

        //sprintf(AuxString, "/soundtrack_shortName %s", pg_SoundTracks[pg_current_configuration_rank][currentlyPlaying_trackNo].trackShortName.c_str());
        //pg_send_message_udp((char *)"s", AuxString, (char *)"udp_TouchOSC_send");
        sprintf(AuxString, "/track_shortName %s", pg_SoundTracks[pg_current_configuration_rank][currentlyPlaying_trackNo].soundtrackShortName.c_str());
        pg_send_message_udp((char*)"s", AuxString, (char*)"udp_TouchOSC_send");

        //printf("soundtrack #%d %s\n", currentlyPlaying_trackNo, pg_SoundTracks[pg_current_configuration_rank][currentlyPlaying_trackNo].soundtrackFileName.c_str());
        //BrokenInterpolationVar[_playing_soundtrackNo][pg_current_configuration_rank] = true;
        currentlyPlaying_trackNo = indTrack;
    }
}

void StopTrack(void) {
    sprintf(AuxString, "/soundtrack_fileName %s", (char*)"");
    pg_send_message_udp((char*)"s", AuxString, (char*)"udp_PD_send");

    if (pa_sound_data.pa_is_streaming()) {
        pa_sound_data.pa_closeMyStream();
    }

#if defined(PG_WITH_JUCE)
    sprintf(AuxString, "/JUCE_open_track \"%s\"", (char*)"void");
    pg_send_message_udp((char*)"s", AuxString, (char*)"udp_SoundJUCE_send");
    pg_send_message_udp((char*)"", (char*)"/JUCE_play_track", (char*)"udp_SoundJUCE_send");
#endif

    //sprintf(AuxString, "/soundtrack_shortName %s", pg_SoundTracks[pg_current_configuration_rank][currentlyPlaying_trackNo].trackShortName.c_str());
    //pg_send_message_udp((char *)"s", AuxString, (char *)"udp_TouchOSC_send");
    sprintf(AuxString, "/track_shortName %s", "void");
    pg_send_message_udp((char*)"s", AuxString, (char*)"udp_TouchOSC_send");

    //printf("No soundtrack\n");
    //BrokenInterpolationVar[_playing_soundtrackNo][pg_current_configuration_rank] = true;
    currentlyPlaying_trackNo = -1;
}

// tests whether the soundtrack is finished reading
void pg_checkAudioStream() {
    pa_sound_data.pa_checkAudioStream();
    //printf("soundtrack current time %f\n", RealTime() - soundfile_data.sound_file_StartReadingTime);
}

void soundTrackvolume(float vol) {
    sprintf(AuxString, "/soundtrack_onOff %.5f", vol);
    pg_send_message_udp((char*)"f", AuxString, (char*)"udp_PD_send");
    printf("Command: soundtrack: %s\n", AuxString);

#if defined(PG_WITH_JUCE)
    if (vol > 0) {
        pg_send_message_udp((char*)"", (char*)"/JUCE_play_track", (char*)"udp_SoundJUCE_send");
    }
    else {
        pg_send_message_udp((char*)"", (char*)"/JUCE_stop_track", (char*)"udp_SoundJUCE_send");
    }
#endif

    sprintf(AuxString, "/soundtrack_onOff %d", soundTrack_on);
    pg_send_message_udp((char*)"i", AuxString, (char*)"udp_TouchOSC_send");
    //sprintf(AuxString, "/soundtrack_volume %.5f", vol);
    //pg_send_message_udp((char*)"f", AuxString, (char*)"udp_TouchOSC_send");
}

void soundTrackonOff() {
    sprintf(AuxString, "/soundtrack_onOff %d", soundTrack_on);
    pg_send_message_udp((char*)"i", AuxString, (char*)"udp_PD_send");
    printf("Command: soundtrack: %s\n", AuxString);
#if defined(PG_WITH_JUCE)
    if (soundTrack_on) {
        pg_send_message_udp((char*)"", (char*)"/JUCE_play_track", (char*)"udp_SoundJUCE_send");
    }
    else {
        pg_send_message_udp((char*)"", (char*)"/JUCE_stop_track", (char*)"udp_SoundJUCE_send");
    }
#endif
    sprintf(AuxString, "/soundtrack_onOff %d", soundTrack_on);
    pg_send_message_udp((char*)"i", AuxString, (char*)"udp_TouchOSC_send");
    //sprintf(AuxString, "/soundtrack_volume %.5f", float(int(soundTrack_on)));
    //pg_send_message_udp((char*)"f", AuxString, (char*)"udp_TouchOSC_send");
}


