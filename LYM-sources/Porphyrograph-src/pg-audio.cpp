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

//////////////////////////////////////////////////////////////////////////////////////////////////////
// GLOBAL VARS
//////////////////////////////////////////////////////////////////////////////////////////////////////

pa_AudioOut pa_sound_data;
ScopedPaHandler* paInit = NULL;
callback_data_s soundfile_data;

// sound track playing
int pg_currentlyPlaying_trackNo = -1;
bool pg_soundTrack_on = true;

// movie soundtrack passes over an onset or a peak before next frame
bool pg_soundTrack_onset = false;
bool pg_soundTrack_peak = false;

// soundtracks
vector<SoundTrack> pg_SoundTracks[PG_MAX_SCENARIOS];
int pg_currentTrackSoundPeakIndex = 0;
int pg_nbTrackSoundPeakIndex[PG_MAX_SCENARIOS] = { 0 };
int pg_currentTrackSoundOnsetIndex = 0;
int pg_nbTrackSoundOnsetIndex[PG_MAX_SCENARIOS] = { 0 };

//////////////////////////////////////////////////////////////////////////////////////////////////////
// PORT AUDIO STREAM MANAGEMENT
//////////////////////////////////////////////////////////////////////////////////////////////////////

pa_AudioOut::pa_AudioOut() : pa_myStream(0), left_phase(0), right_phase(0)
{
    strcpy(pa_message, "No Message");
    is_streaming = false;
}

bool pa_AudioOut::pa_openMyStream(PaDeviceIndex index, int* channelCount, int* sampleRate)
{
    PaStreamParameters outputParameters = PaStreamParameters();
    PaError err = paNoError;

    if (pg_FullScenarioActiveVars[pg_ind_scenario][_soundtrack_PA_weight]) {
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
        printf("Sound file channel count %d sample rate %d\n", *channelCount, *sampleRate);
        outputParameters.sampleFormat = paFloat32; /* 32 bit floating point output */
        outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
        outputParameters.hostApiSpecificStreamInfo = NULL;

        err = Pa_OpenStream(
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
            printf("Failed to open stream to device error %d\n", err);
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
    }
    return true;
}

bool pa_AudioOut::pa_closeMyStream()
{
    PaError err = paNoError;
    if (pg_FullScenarioActiveVars[pg_ind_scenario][_soundtrack_PA_weight]) {
        if (pa_myStream == 0)
            return false;

        err = Pa_CloseStream(pa_myStream);
        is_streaming = false;
    }

    return (err == paNoError);
}


bool pa_AudioOut::pa_startMyStream()
{
    PaError err = paNoError;
    if (pg_FullScenarioActiveVars[pg_ind_scenario][_soundtrack_PA_weight]) {
        if (pa_myStream == 0)
            return false;

        err = Pa_StartStream(pa_myStream);

        is_streaming = true;
    }
    return (err == paNoError);
}

bool pa_AudioOut::pa_stopMyStream()
{
    PaError err = paNoError;
    if (pg_FullScenarioActiveVars[pg_ind_scenario][_soundtrack_PA_weight]) {
        if (pa_myStream == 0)
            return false;

       err = Pa_StopStream(pa_myStream);

        is_streaming = false;
    }
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

    if (pg_FullScenarioActiveVars[pg_ind_scenario][_soundtrack_PA_weight]) {
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
    }
    else {
        return paComplete;
    }

    return paContinue;
}

bool pa_AudioOut::pa_checkAudioStream() {
    if (pg_FullScenarioActiveVars[pg_ind_scenario][_soundtrack_PA_weight]) {
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
                sprintf(pg_errorStr, "Error audio streaming %d", Pa_IsStreamActive(pa_myStream)); pg_ReportError(pg_errorStr); throw 561;
            }
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
    printf("Stream Completed: at %f\n", pg_RealTime() - soundfile_data.sound_file_StartReadingTime);
}

/*
    * This routine is called by portaudio when playback is done.
    */
void pa_AudioOut::paStreamFinished(void* userData)
{
    return ((pa_AudioOut*)userData)->paStreamFinishedMethod();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// SOUNDTRACKS MANAGEMENT
//////////////////////////////////////////////////////////////////////////////////////////////////////
void pg_listAll_soundTracks(void) {
    printf("Listing Soundtracks:\n");
    for (int indScenario = 0; indScenario < pg_NbScenarios; indScenario++) {
        std::cout << "    " << indScenario << ": ";
        for (SoundTrack& soundtrack : pg_SoundTracks[indScenario]) {
            std::cout << soundtrack.soundtrackFileName << " (" << soundtrack.soundtrackShortName << "), ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void PlayTrack(int indTrack, double timeFromStart) {
    if (!pg_SoundTracks[pg_ind_scenario].empty()) {
        bool new_track = (indTrack >= 0 && pg_currentlyPlaying_trackNo != indTrack % pg_SoundTracks[pg_ind_scenario].size());
        //std::cout << "new track ind: " << indTrack << " pg_currentlyPlaying_trackNo:" << pg_currentlyPlaying_trackNo << " nb of tracks:" << 
        // pg_SoundTracks[pg_ind_scenario].size() << std::endl;

        if (new_track) {
            // std::cout << "pg_cwd: " << pg_cwd << std::endl;
            int previouslyPlayingSoundtrackNo = pg_currentlyPlaying_trackNo;
            pg_currentlyPlaying_trackNo = indTrack % pg_SoundTracks[pg_ind_scenario].size();
            std::cout << "Playing soundtrack name: " << pg_SoundTracks[pg_ind_scenario][pg_currentlyPlaying_trackNo].soundtrackFileName << std::endl;
            sprintf(pg_AuxString, "/soundtrack_fileName %s", pg_SoundTracks[pg_ind_scenario][pg_currentlyPlaying_trackNo].soundtrackFileName.c_str());

            // PureData play
            if (pg_FullScenarioActiveVars[pg_ind_scenario][_soundtrack_PD_weight]) {
                pg_send_message_udp((char*)"s", pg_AuxString, (char*)"udp_PD_send");
            }

            // portaudio play
            if (pg_FullScenarioActiveVars[pg_ind_scenario][_soundtrack_PA_weight] && soundtrack_PA_weight > 0 && paInit != NULL) {
                if ((previouslyPlayingSoundtrackNo >= 0 && previouslyPlayingSoundtrackNo < int(pg_SoundTracks[pg_ind_scenario].size()))
                    || pa_sound_data.pa_is_streaming()) {
                    printf("Closing stream (%s)\n", (char*)(pg_SoundTracks[pg_ind_scenario][previouslyPlayingSoundtrackNo].soundtrackFileName).c_str());
                    pa_sound_data.pa_stopMyStream();
                    pa_sound_data.pa_closeMyStream();
                }
                printf("Opening Wav file (%s)\n", (char*)(pg_SoundTracks[pg_ind_scenario][pg_currentlyPlaying_trackNo].soundtrackFileName).c_str());
                /* Open the soundfile */
                soundfile_data.sound_file = sf_open((char*)(pg_SoundTracks[pg_ind_scenario][pg_currentlyPlaying_trackNo].soundtrackFileName).c_str(),
                    SFM_READ, &soundfile_data.sound_file_info);
                //printf("PA soundfile opened\n");
                if (sf_error(soundfile_data.sound_file) != SF_ERR_NO_ERROR) {
                    fprintf(stderr, "%s\n", sf_strerror(soundfile_data.sound_file));
                    sprintf(pg_errorStr, "Wav file not opened (%s)!", (char*)(pg_SoundTracks[pg_ind_scenario][pg_currentlyPlaying_trackNo].soundtrackFileName).c_str()); pg_ReportError(pg_errorStr);
                    return;
                }

                // opening stream
                int channelCount;
                int sampleRate;
                if (pa_sound_data.pa_openMyStream(Pa_GetDefaultOutputDevice(), &channelCount, &sampleRate) && paInit != NULL) {
                    printf("portaudio stream opened %d ch at %d Hz\n", channelCount, sampleRate);
                }
                else {
                    if (paInit != NULL) {
                        fprintf(stderr, "Error number: %d\n", paInit->result());
                        fprintf(stderr, "Error message: %s\n", Pa_GetErrorText(paInit->result()));
                        sprintf(pg_errorStr, "Port audio stream not opened!"); pg_ReportError(pg_errorStr); // throw 100;
                    }
                }
                //printf("PA stream opened\n");

                // starting stream
                if (pa_sound_data.pa_startMyStream() && paInit != NULL) {
                    //printf("portaudio stream started\n");
                }
                else {
                    if (paInit != NULL) {
                        fprintf(stderr, "Error number: %d\n", paInit->result());
                        fprintf(stderr, "Error message: %s\n", Pa_GetErrorText(paInit->result()));
                        sprintf(pg_errorStr, "Port audio stream not started!"); pg_ReportError(pg_errorStr); // throw 100;
                    }
                }
            }

            // JUCE play
            if (pg_FullScenarioActiveVars[pg_ind_scenario][_soundtrack_JUCE_weight]) {
                printf("soundtrack Name %s\n", pg_SoundTracks[pg_ind_scenario][pg_currentlyPlaying_trackNo].soundtrackFileName.c_str());
                sprintf(pg_AuxString, "/JUCE_open_track \"%s\"", pg_SoundTracks[pg_ind_scenario][pg_currentlyPlaying_trackNo].soundtrackFileName.c_str());
                pg_send_message_udp((char*)"s", pg_AuxString, (char*)"udp_script_binary_send");
                pg_send_message_udp((char*)"", (char*)"/JUCE_play_track", (char*)"udp_script_binary_send");
            }
            printf("\n");

            // Peak or Onset indices
            pg_soundTrack_onset = false;
            pg_soundTrack_peak = false;
            pg_currentTrackSoundPeakIndex = 0;
            pg_currentTrackSoundOnsetIndex = 0;

            if (pg_currentlyPlaying_trackNo >= 0
                && int(pg_SoundTracks[pg_ind_scenario].size()) > pg_currentlyPlaying_trackNo) {
                pg_nbTrackSoundPeakIndex[pg_ind_scenario]
                    = pg_SoundTracks[pg_ind_scenario][pg_currentlyPlaying_trackNo].soundtrackPeaks.size();
            }
            else {
                pg_nbTrackSoundPeakIndex[pg_ind_scenario] = 0;
            }
            if (pg_currentlyPlaying_trackNo >= 0
                && int(pg_SoundTracks[pg_ind_scenario].size()) > pg_currentlyPlaying_trackNo) {
                pg_nbTrackSoundOnsetIndex[pg_ind_scenario]
                    = pg_SoundTracks[pg_ind_scenario][pg_currentlyPlaying_trackNo].soundtrackOnsets.size();
            }
            else {
                pg_nbTrackSoundOnsetIndex[pg_ind_scenario] = 0;
            }
            //printf("pg_nbTrackSoundPeakIndex %d pg_nbTrackSoundOnsetIndex %d\n",
            //    pg_nbTrackSoundPeakIndex[pg_ind_scenario], pg_nbTrackSoundOnsetIndex[pg_ind_scenario]);

        }

        // start position seeking
        if (timeFromStart > 0) {
            if (sf_seek(soundfile_data.sound_file, sf_count_t(timeFromStart * soundfile_data.sound_file_info.samplerate), SEEK_SET) == -1) {
                sprintf(pg_errorStr, "Seek error in file (%s)!", (char*)(pg_SoundTracks[pg_ind_scenario][pg_currentlyPlaying_trackNo].soundtrackFileName).c_str()); pg_ReportError(pg_errorStr);
            }
            soundfile_data.sound_file_StartReadingTime
                = pg_RealTime() - pg_SoundTracks[pg_ind_scenario][pg_currentlyPlaying_trackNo].soundtrackOnsetsAndPeasksOffset - timeFromStart;
        }
        else {
            soundfile_data.sound_file_StartReadingTime
                = pg_RealTime() - pg_SoundTracks[pg_ind_scenario][pg_currentlyPlaying_trackNo].soundtrackOnsetsAndPeasksOffset;
        }

        //sprintf(pg_AuxString, "/soundtrack_shortName %s", pg_SoundTracks[pg_ind_scenario][pg_currentlyPlaying_trackNo].trackShortName.c_str());
        //pg_send_message_udp((char *)"s", pg_AuxString, (char *)"udp_TouchOSC_send");
        sprintf(pg_AuxString, "/track_shortName %s", pg_SoundTracks[pg_ind_scenario][pg_currentlyPlaying_trackNo].soundtrackShortName.c_str());
        pg_send_message_udp((char*)"s", pg_AuxString, (char*)"udp_TouchOSC_send");

        //printf("soundtrack #%d %s\n", pg_currentlyPlaying_trackNo, pg_SoundTracks[pg_ind_scenario][pg_currentlyPlaying_trackNo].soundtrackFileName.c_str());
        //pg_BrokenInterpolationVar[_playing_soundtrackNo][pg_ind_scenario] = true;
        pg_currentlyPlaying_trackNo = indTrack;
    }
}

void StopTrack(void) {
    // PD play
    if (pg_FullScenarioActiveVars[pg_ind_scenario][_soundtrack_PD_weight]) {
        sprintf(pg_AuxString, "/soundtrack_fileName void");
        pg_send_message_udp((char*)"s", pg_AuxString, (char*)"udp_PD_send");
    }

    // PA play
    if (pg_FullScenarioActiveVars[pg_ind_scenario][_soundtrack_PA_weight]) {
        if (pa_sound_data.pa_is_streaming()) {
            pa_sound_data.pa_closeMyStream();
        }
    }

    // JUCE play
    if (pg_FullScenarioActiveVars[pg_ind_scenario][_soundtrack_JUCE_weight]) {
        pg_send_message_udp((char*)"", (char*)"/JUCE_stop_track", (char*)"udp_script_binary_send");
    }

    //sprintf(pg_AuxString, "/soundtrack_shortName %s", pg_SoundTracks[pg_ind_scenario][pg_currentlyPlaying_trackNo].trackShortName.c_str());
    //pg_send_message_udp((char *)"s", pg_AuxString, (char *)"udp_TouchOSC_send");
    sprintf(pg_AuxString, "/track_shortName %s", "void");
    pg_send_message_udp((char*)"s", pg_AuxString, (char*)"udp_TouchOSC_send");

    //printf("No soundtrack\n");
    //pg_BrokenInterpolationVar[_playing_soundtrackNo][pg_ind_scenario] = true;
    pg_currentlyPlaying_trackNo = -1;
}

// tests whether the soundtrack is finished reading
void pg_pa_checkAudioStream() {
    if (pg_FullScenarioActiveVars[pg_ind_scenario][_soundtrack_PA_weight]) {
        pa_sound_data.pa_checkAudioStream();
    }
    //printf("soundtrack current time %f\n", pg_RealTime() - soundfile_data.sound_file_StartReadingTime);
}

void soundTrackvolume(float vol) {
    // PD play
    if (pg_FullScenarioActiveVars[pg_ind_scenario][_soundtrack_PD_weight]) {
        sprintf(pg_AuxString, "/soundtrack_weight %.5f", vol);
        pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_PD_send");
        printf("Command: soundtrack: %s\n", pg_AuxString);
    }

    // JUCE play
    if (pg_FullScenarioActiveVars[pg_ind_scenario][_soundtrack_JUCE_weight]) {
        sprintf(pg_AuxString, "/JUCE_soundtrack_weight %.5f", vol);
        pg_send_message_udp((char*)"f", (char*)"/JUCE_soundtrack_weight", (char*)"udp_script_binary_send");
    }

    sprintf(pg_AuxString, "/soundtrack_onOff %d", pg_soundTrack_on);
    pg_send_message_udp((char*)"i", pg_AuxString, (char*)"udp_TouchOSC_send");
    //sprintf(pg_AuxString, "/soundtrack_volume %.5f", vol);
    //pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_TouchOSC_send");
}

void soundTrackonOff() {
    if (pg_FullScenarioActiveVars[pg_ind_scenario][_soundtrack_PD_weight]) {
        sprintf(pg_AuxString, "/soundtrack_onOff %d", pg_soundTrack_on);
        pg_send_message_udp((char*)"i", pg_AuxString, (char*)"udp_PD_send");
    }
    printf("Command: soundtrack: %s\n", pg_AuxString);
    if (pg_FullScenarioActiveVars[pg_ind_scenario][_soundtrack_JUCE_weight]) {
        if (pg_soundTrack_on) {
            pg_send_message_udp((char*)"", (char*)"/JUCE_play_track", (char*)"udp_script_binary_send");
        }
        else {
            pg_send_message_udp((char*)"", (char*)"/JUCE_stop_track", (char*)"udp_script_binary_send");
        }
    }
    sprintf(pg_AuxString, "/soundtrack_onOff %d", pg_soundTrack_on);
    pg_send_message_udp((char*)"i", pg_AuxString, (char*)"udp_TouchOSC_send");
    //sprintf(pg_AuxString, "/soundtrack_volume %.5f", float(int(pg_soundTrack_on)));
    //pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_TouchOSC_send");
}

void pg_pa_closeAudioStream(void) {
    if (pg_FullScenarioActiveVars[pg_ind_scenario][_soundtrack_PA_weight]) {
        pa_sound_data.pa_closeMyStream();
        if (paInit != NULL) {
            delete paInit;
        }
    }
}

void pg_pa_openSoundData(void) {
    if (pg_FullScenarioActiveVars[pg_ind_scenario][_soundtrack_PA_weight]) {
        paInit = new ScopedPaHandler();
        if (paInit != NULL && paInit->result() != paNoError) {
            fprintf(stderr, "Error number: %d\n", paInit->result());
            fprintf(stderr, "Error message: %s\n", Pa_GetErrorText(paInit->result()));
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// AUDIO SCENARIO
////////////////////////////////////////////////////////////////////////////////////////////////////// 
void pg_parseScenario_soundTracks(std::ifstream& scenarioFin, int indScenario) {
    std::stringstream  sstream;
    string line;
    string ID;
    string temp;
    string temp2;

    ////////////////////////////
    ////// SOUNDTRACKS
    std::getline(scenarioFin, line);
    pg_stringstreamStoreLine(&sstream, &line);
    sstream >> ID; // string soundtracks
    if (ID.compare("soundtracks") != 0) {
        sprintf(pg_errorStr, "Error: incorrect configuration file expected string \"soundtracks\" not found! (instead \"%s\")", ID.c_str()); pg_ReportError(pg_errorStr); throw 100;
    }

    while (true) {
        // new line
        std::getline(scenarioFin, line);
        pg_stringstreamStoreLine(&sstream, &line);
        sstream >> ID; // string /soundtracks or track
        if (ID.compare("/soundtracks") == 0) {
            break;
        }
        else if (ID.compare("track") != 0) {
            sprintf(pg_errorStr, "Error: incorrect configuration file expected string \"track\" not found! (instead \"%s\")", ID.c_str()); pg_ReportError(pg_errorStr); throw 100;
        }

        SoundTrack soundtrack;

        sstream >> temp;
        soundtrack.soundtrackFileName = temp;
        if (!pg_isFullPath(soundtrack.soundtrackFileName)) {
            soundtrack.soundtrackFileName = pg_soundtracks_directory + soundtrack.soundtrackFileName;
        }
        sstream >> temp2;
        soundtrack.soundtrackShortName = temp2;
        //std::cout << "Soundtrack: " << pg_SoundTracks[indScenario][indTrack].soundtrackFileName << " " << pg_SoundTracks[indScenario][indTrack].trackShortName << " (#" << indTrack << ")\n";

        // in addition to the track name and short name, 2 additional
        // files can be used to generate beats from sound envelope at 1
        // or sound onsets detected through aubio library

        // possible additional peaked sound envelope at 1.0 or above
        // NULL value or no value means no file
        if (sstream >> temp2) {
            // there is a soundtrack file with peaked sound envelope at 1.0
            if (temp2.compare("") != 0 && temp2.compare("NULL") != 0) {
                soundtrack.soundtrackPeaksFileName = temp2;
                string csv_line;
                vector<float> peak_times;
                std::ifstream peak_file(temp2);
                if (!peak_file) {
                    sprintf(pg_errorStr, "Error: peak file [%s] not found!", temp2.c_str()); pg_ReportError(pg_errorStr); throw 11;
                }
                printf("Read audio soundtrack peaks [%s]\n", temp2.c_str());
                // reads the peaks timecodes and stores them in a float vector
                std::getline(peak_file, csv_line);
                std::getline(peak_file, csv_line);
                std::getline(peak_file, csv_line);
                while (std::getline(peak_file, csv_line)) {
                    std::stringstream  peak_sstream;
                    pg_stringstreamStoreLine(&peak_sstream, &csv_line);
                    float time, sound;
                    peak_sstream >> time;
                    peak_sstream >> sound;
                    if (sound >= 1) {
                        peak_times.push_back(time);
                    }
                }
                peak_file.close();
                soundtrack.soundtrackPeaks = peak_times;;
                //for (int i = 0; i < int(peak_times.size()); ++i) {
                //	std::cout << "Peak: " << peak_times[i] << '\n';
                //}
            }
            else {
                soundtrack.soundtrackPeaksFileName = "";
                soundtrack.soundtrackPeaks = {};
            }
            if (sstream >> temp2 && temp2.compare("") != 0 && temp2.compare("NULL") != 0) {
                soundtrack.soundtrackOnsetsFileName = temp2;
                string csv_line;
                vector<float> onset_times;
                std::ifstream onset_file(temp2);
                if (!onset_file) {
                    sprintf(pg_errorStr, "Error: onset file [%s] not found!", temp2.c_str()); pg_ReportError(pg_errorStr); throw 11;
                }
                // reads the peaks timecodes and stores them in a float vector
                printf("Read audio soundtrack onsets [%s]\n", temp2.c_str());
                while (std::getline(onset_file, csv_line)) {
                    std::stringstream  onset_sstream;
                    pg_stringstreamStoreLine(&onset_sstream, &csv_line);
                    float time;
                    onset_sstream >> time;
                    onset_times.push_back(time);
                }
                onset_file.close();
                soundtrack.soundtrackOnsets = onset_times;;
                //for (int i = 0; i < int(onset_times.size()); ++i) {
                //	std::cout << "Onset: " << onset_times[i] << '\n';
                //}
            }
            else {
                soundtrack.soundtrackOnsetsFileName = "";
                soundtrack.soundtrackOnsets = {};
            }
            float offset = 0.f;
            sstream >> offset;
            soundtrack.soundtrackOnsetsAndPeasksOffset = offset;
        }
        else {
            soundtrack.soundtrackPeaksFileName = "";
            soundtrack.soundtrackPeaks = {};
            soundtrack.soundtrackOnsetsFileName = "";
            soundtrack.soundtrackOnsets = {};
            soundtrack.soundtrackOnsetsAndPeasksOffset = 0.f;
        }
        //std::cout << "track : " << 
        // pg_SoundTracks[indVideo]->soundtrackFileName << "\n";
        pg_SoundTracks[indScenario].push_back(soundtrack);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// AUDIO OSC COMMANDS
//////////////////////////////////////////////////////////////////////////////////////////////////////  
void pg_clamp_color(float in_color[4], float out_color[4]) {
    for (int i = 0; i < 4; i++) {
        out_color[i] = std::max(0.f, std::min(1.f, in_color[i]));
    }
}
void pg_aliasScriptAudio(string address_string, string string_argument_0,
    float float_arguments[PG_MAX_OSC_ARGUMENTS], int nb_arguments, int indVar) {
    // special command not in the scenario file
    switch (indVar) {
    case _reset_sound:
        pg_send_message_udp((char*)"i", (char*)"/reset_sound 1", (char*)"udp_PD_send");
        break;
    case _pulse_spectrum:
        pg_audio_pulse[0] = float_arguments[0] * sound_volume + sound_min;
        pg_audio_pulse[1] = float_arguments[1] * sound_volume + sound_min;
        pg_audio_pulse[2] = float_arguments[2] * sound_volume + sound_min;
        // not used currently  pulse_attack = float_arguments[3] * sound_volume + sound_min;
        sprintf(pg_AuxString, "/pulse_low %.2f", pg_audio_pulse[0]);
        pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_TouchOSC_send");
        sprintf(pg_AuxString, "/pulse_medium %.2f", pg_audio_pulse[1]);
        pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_TouchOSC_send");
        sprintf(pg_AuxString, "/pulse_high %.2f", pg_audio_pulse[2]);
        pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_TouchOSC_send");

        pg_audio_pulse_average_prec = pulse_average;
        pulse_average = (pg_audio_pulse[0] + pg_audio_pulse[1] + pg_audio_pulse[2]) / 3.f;

        sprintf(pg_AuxString, "/pg_audio_pulse %.2f", pulse_average);
        pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_TouchOSC_send");
        // printf("%s\n", pg_AuxString);

        if (pg_FullScenarioActiveVars[pg_ind_scenario][_pen_color]
            && pg_FullScenarioActiveVars[pg_ind_scenario][_pen_grey]
            && !pen_hsv) {
            //printf("pg_audio_pulse %.2f %.2f %.2f avg %.2f\n", pg_audio_pulse[0], pg_audio_pulse[1], pg_audio_pulse[2], pulse_average);
            //printf("pen_color %.2f pen_color_pulse %.2f pen_grey %.2f pen_grey_pulse %.2f pg_pulsed_pen_color  %.2f %.2f %.2f %.2f\n", pen_color, pen_color_pulse, pen_grey, pen_grey_pulse, pg_pulsed_pen_color[0], pg_pulsed_pen_color[1], pg_pulsed_pen_color[2], pg_pulsed_pen_color[3]);
            pg_compute_pulsed_palette_color(pen_color, pen_color_pulse, pen_grey, pen_grey_pulse, pg_pulsed_pen_color, pg_enum_PEN_COLOR);
        }
        else if (pg_FullScenarioActiveVars[pg_ind_scenario][_pen_hue]
            && pg_FullScenarioActiveVars[pg_ind_scenario][_pen_sat]
            && pg_FullScenarioActiveVars[pg_ind_scenario][_pen_value]
            && pen_hsv) {
            pg_compute_pulsed_HSV_color(pen_hue, pen_hue_pulse, pen_sat, pen_sat_pulse, pen_value, pen_value_pulse, pg_pulsed_pen_color, true);
        }

        float clamped_color[4];
        pg_clamp_color(pg_pulsed_pen_color, clamped_color);
        sprintf(pg_AuxString, "/pen_color/color %02x%02x%02xFF", int(clamped_color[0] * 255), int(clamped_color[1] * 255), int(clamped_color[2] * 255));
        pg_send_message_udp((char*)"s", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");

        pg_compute_pulsed_palette_color(repop_colorCA, repop_colorCA_pulse, repop_greyCA, repop_greyCA_pulse, pg_pulsed_repop_colorCA, pg_enum_REPOP_COLOR);
        pg_clamp_color(pg_pulsed_repop_colorCA, clamped_color);
        sprintf(pg_AuxString, "/CA_repopColor/color %02x%02x%02xFF", int(clamped_color[0] * 255), int(clamped_color[1] * 255), int(clamped_color[2] * 255));
        pg_send_message_udp((char*)"s", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
 
        pg_compute_pulsed_palette_color(repop_colorBG, repop_colorBG_pulse, repop_greyBG, repop_greyBG_pulse, pg_pulsed_repop_colorBG, pg_enum_REPOP_COLOR);
        pg_clamp_color(pg_pulsed_repop_colorBG, clamped_color);
        sprintf(pg_AuxString, "/BGcolorRedepopColor/color %02x%02x%02xFF", int(clamped_color[0] * 255), int(clamped_color[1] * 255), int(clamped_color[2] * 255));
        pg_send_message_udp((char*)"s", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");

        if (pg_FullScenarioActiveVars[pg_ind_scenario][_repop_colorPart]
            && pg_FullScenarioActiveVars[pg_ind_scenario][_repop_greyPart]
            && !repop_hsvPart) {
            pg_compute_pulsed_palette_color(repop_colorPart, repop_colorPart_pulse, repop_greyPart, repop_greyPart_pulse, pg_pulsed_repop_colorPart, pg_enum_REPOP_COLOR);
        }
        if (pg_FullScenarioActiveVars[pg_ind_scenario][_repop_huePart]
            && pg_FullScenarioActiveVars[pg_ind_scenario][_repop_satPart]
            && pg_FullScenarioActiveVars[pg_ind_scenario][_repop_valuePart]
            && repop_hsvPart) {
            pg_compute_pulsed_HSV_color(repop_huePart, repop_huePart_pulse, repop_satPart, repop_satPart_pulse, repop_valuePart, repop_valuePart_pulse, pg_pulsed_repop_colorPart, false);
        }
        pg_clamp_color(pg_pulsed_repop_colorPart, clamped_color);
        sprintf(pg_AuxString, "/Part_repopColor/color %02x%02x%02xFF", int(clamped_color[0] * 255), int(clamped_color[1] * 255), int(clamped_color[2] * 255));
        pg_send_message_udp((char*)"s", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
        break;
    case _soundtrack_plus:
        if (!pg_SoundTracks[pg_ind_scenario].empty()) {
            if (pg_currentlyPlaying_trackNo < 0) {
                PlayTrack(0, 0.);
            }
            else {
                PlayTrack((pg_currentlyPlaying_trackNo + 1) % pg_SoundTracks[pg_ind_scenario].size(), 0.);
            }
            pg_BrokenInterpolationVar[_playing_soundtrackNo] = true;
            *((int*)pg_FullScenarioVarPointers[_playing_soundtrackNo]) = pg_currentlyPlaying_trackNo;
        }
        break;
    case _soundtrack_minus:
        if (!pg_SoundTracks[pg_ind_scenario].empty()) {
            if (pg_currentlyPlaying_trackNo < 0) {
                PlayTrack(0, 0.);
            }
            else {
                PlayTrack((pg_currentlyPlaying_trackNo + pg_SoundTracks[pg_ind_scenario].size() - 1) % pg_SoundTracks[pg_ind_scenario].size(), 0.);
            }
            pg_BrokenInterpolationVar[_playing_soundtrackNo] = true;
            *((int*)pg_FullScenarioVarPointers[_playing_soundtrackNo]) = pg_currentlyPlaying_trackNo;
        }
        break;
    case _soundtrack_seek: {
        int soundTrack_no = int(float_arguments[0]);
        double seek_position = 0.;
        if (nb_arguments >= 2) {
            seek_position = double(float_arguments[1]);
        }

        // seek inside the current track or a new track
        if (soundTrack_no >= 0 && soundTrack_no < int(pg_SoundTracks[pg_ind_scenario].size())) {
            if (pg_currentlyPlaying_trackNo != soundTrack_no) {
                printf("soundtrack_seek: pg_currentlyPlaying_trackNo %d != soundTrack_no %d\n", pg_currentlyPlaying_trackNo, soundTrack_no);
                StopTrack();
                pg_BrokenInterpolationVar[_playing_soundtrackNo] = true;
                *((int*)pg_FullScenarioVarPointers[_playing_soundtrackNo]) = soundTrack_no;
                pg_currentlyPlaying_trackNo = soundTrack_no;
            }
            PlayTrack(pg_currentlyPlaying_trackNo, seek_position);
        }
        // stop playing a track
        else if (soundTrack_no < 0) {
            if (playing_soundtrackNo >= 0) {
                printf("soundtrack_seek: playing_soundtrackNo neg %d\n", soundTrack_no);
                StopTrack();
            }
            pg_BrokenInterpolationVar[_playing_soundtrackNo] = true;
            *((int*)pg_FullScenarioVarPointers[_playing_soundtrackNo]) = -1;
            pg_currentlyPlaying_trackNo = -1;
        }
    }
                         break;
    case _soundtrack_onOff:
        pg_soundTrack_on = !pg_soundTrack_on;
        soundTrackonOff();
        break;
    case _soundtrack_volume:
        pg_soundTrack_on = (float_arguments[0] > 0);
        soundTrackvolume(float_arguments[0]);
        break;
        // +++++++++++++++++ JUCE SOUND CONTROL +++++++++++++++++++++++++
    case _JUCE_loop_track:
        if (pg_FullScenarioActiveVars[pg_ind_scenario][_soundtrack_JUCE_weight]) {
            pg_send_message_udp((char*)"", (char*)"/JUCE_loop_track", (char*)"udp_script_binary_send");
        }
        break;
    case _JUCE_exit:
        if (pg_FullScenarioActiveVars[pg_ind_scenario][_soundtrack_JUCE_weight]) {
            pg_send_message_udp((char*)"", (char*)"/JUCE_exit", (char*)"udp_script_binary_send");
        }
        break;
    case _fftLevel8loudestFreqBins:
        if (pg_FullScenarioActiveVars[pg_ind_scenario][_soundtrack_JUCE_weight]) {
            //printf("fft levels: ");
            //for (int indArg = 0; indArg < 8; indArg++) {
            //	printf("%.2f/%.2f ", float_arguments[2 * indArg], float_arguments[2 * indArg + 1]);
            //}
            //printf("\n");
            float totFFTLevel = 0.f;
            for (int indArg = 0; indArg < 8; indArg++) {
                fftFrequencies[indArg] = float_arguments[3 * indArg];
                fftLevels[indArg] = float_arguments[3 * indArg + 1];
                fftPhases[indArg] = float_arguments[3 * indArg + 2];
                totFFTLevel += fftLevels[indArg];
            }
            // normalization of the levels (sum = 0.5 (because cos + 1 used for color))
            totFFTLevel *= 2.f;
            if (totFFTLevel > 0.f) {
                for (int indArg = 0; indArg < 8; indArg++) {
                    fftLevels[indArg] /= totFFTLevel;
                }
            }
        }
        break;
    default:
        sprintf(pg_errorStr, "Audio command not found (%s)!", address_string.c_str()); pg_ReportError(pg_errorStr);
        break;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// AUDIO CALLBACKS
//////////////////////////////////////////////////////////////////////////////////////////////////////  
void audioInput_weight_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
    if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
        audioInput_weight = scenario_or_gui_command_value;
        if (pg_FullScenarioActiveVars[pg_ind_scenario][_soundtrack_PD_weight]) {
            sprintf(pg_AuxString, "/audioInput_weight %.2f", audioInput_weight);
            pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_PD_send");
        }
        if (pg_FullScenarioActiveVars[pg_ind_scenario][_soundtrack_JUCE_weight]) {
            sprintf(pg_AuxString, "/JUCE_audioInput_weight %.2f", audioInput_weight);
            pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_script_binary_send");
        }
    }
}

void soundtrack_PD_weight_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
    if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
        if (pg_FullScenarioActiveVars[pg_ind_scenario][_soundtrack_PD_weight]) {
            soundtrack_PD_weight = scenario_or_gui_command_value;
            sprintf(pg_AuxString, "/soundtrack_PD_weight %.2f", soundtrack_PD_weight);
            pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_PD_send");
        }
    }
}
void soundtrack_JUCE_weight_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
    if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
        if (pg_FullScenarioActiveVars[pg_ind_scenario][_soundtrack_JUCE_weight]) {
            sprintf(pg_AuxString, "/JUCE_soundtrack_weight %.2f", soundtrack_JUCE_weight);
            pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_script_binary_send");
        }
    }
}

void soundtrack_PA_weight_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
    if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
        if (pg_FullScenarioActiveVars[pg_ind_scenario][_soundtrack_PA_weight]) {
            soundtrack_PA_weight = scenario_or_gui_command_value;
            sprintf(pg_AuxString, "/soundtrack_PA_weight %.2f", soundtrack_PA_weight);
            pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_PD_send");
            if (soundtrack_PA_weight > 0 && paInit == NULL) {
                printf("Open portaudio\n");
                pg_pa_openSoundData();
            }
        }
    }
}

void sound_env_min_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
    if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
        if (pg_FullScenarioActiveVars[pg_ind_scenario][_soundtrack_PD_weight]) {
            sprintf(pg_AuxString, "/sound_env_min %.2f", scenario_or_gui_command_value);
            pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_PD_send");
        }
    }
    // printf("reset sound\n");
}

void sound_env_max_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
    if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
        if (pg_FullScenarioActiveVars[pg_ind_scenario][_soundtrack_PD_weight]) {
            sprintf(pg_AuxString, "/sound_env_max %.2f", scenario_or_gui_command_value);
            pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_PD_send");
        }
    }
    // printf("reset sound\n");
}

void playing_soundtrackNo_callBack(pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value) {
    if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
        // play a new track
        if (int(scenario_or_gui_command_value) >= 0
            && int(scenario_or_gui_command_value) < int(pg_SoundTracks[pg_ind_scenario].size())
            && pg_currentlyPlaying_trackNo != int(scenario_or_gui_command_value)) {
            PlayTrack(int(scenario_or_gui_command_value), 0.);
        }
        // stop playing a track
        else if (int(scenario_or_gui_command_value) < 0) {
            //printf("playing_soundtrackNo_callBack: playing_soundtrackNo neg %d\n", int(scenario_or_gui_command_value));
            StopTrack();
        }
    }
}
