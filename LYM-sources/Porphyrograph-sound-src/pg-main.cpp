////////////////////////////////////////////////////////////////////
// 
// 
// 
//     File pg-main.cpp
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

/// Error string
char *ErrorStr = NULL;

/// Auxiliary string
char AuxString[1024];

// date string for output files
std::stringstream  date_stringStream;

// booleans that signals the end of the application through user's interaction
bool pg_time_to_quit = false;

// boolean for recording the audio output
bool record_output = false;

////////////////////////////////////////////////////////////////////
// UDP PROCESSING THREAD
HANDLE  hThread;

////////////////////////////////////////////////////////////////////
// initial time to manage synchronization and  beats
double StreamingRealTime = -1.0;
double InitialPerformanceTime = -1.0;   // the starting time for beats
#ifdef PG_TERRAINS_VAGUES
double InitialTerrainVaguesTime = -1.0;   // the starting time for playing terrains vagues samples
#endif
double LastStreamingBeatRealTime = -1.0;
double BeatDurationRealTime = 60 / float(BeatsPerMinute);
unsigned long NbStreamingBeats = 0;
unsigned long NbPorphyrographBeats = 0;

////////////////////////////////////////////////////////////////////
// terrains vagues samples control
float TV_track_shuffle = 6;
float TV_drone_mix = 0.0;
float TV_swap_freq = 2;
float TV_swap_dur = 1;

/////////////////////////////////////////////////////////////////////
// terrains vagues blast management
bool TV_blash_on = false;
float *seqLevels = NULL;
float track0Level;


///////////////////////////////////////////////////////////////////////////////////
// UDP BASED COMMANDS
///////////////////////////////////////////////////////////////////////////////////
void pg_aliasScript(char *newCommand,
	char* string_argument_0,
	float arguments[MAX_OSC_ARGUMENTS]) {
	// printf("command %s %.2f\n", newCommand, arguments[0]);
	// commands for reverb

	string sample_prefix = "sample_";
	string commandAlias = string(newCommand);
	bool isSampleEffectCommand = false;
	if (commandAlias.compare(0, sample_prefix.length(), sample_prefix) == 0) {
		isSampleEffectCommand = true;
		commandAlias = commandAlias.substr(sample_prefix.length());
	}
	// std::cout << "New command: " << commandAlias << " isSample command " << isSampleEffectCommand << std::endl;

	// Expects launch as first command
	//if (commandAlias.compare("launch") != 0 && !pg_Launched_Perfromance) {
	//	return;
	//}

	/////////////////////////////////////////////
	// NETWORK CONNECTION CHECK
	if (commandAlias.compare("testUDP_fromPD") == 0) {
		sprintf(AuxString, "/testUDP"); pg_send_message_udp((char *)"", (char *)AuxString, (char *)"udp_Porphyrograph_send");
	}
	else if (commandAlias.compare("testUDP_OK") == 0) {
		printf("***********************************************\n");
		printf("porphyrograph UDP MESSAGE\n");
		printf("***********************************************\n");
	}

	/////////////////////////////////////////////
	// launching terrains vagues
	else if (commandAlias.compare("launch") == 0) {
		// level and playing initialization
		for (int ind_sequences = 0; ind_sequences < NbAudioSequences; ind_sequences++) {
			sprintf(AuxString, "/%s_onOff_can %d", AudioSequences[ind_sequences]->name.c_str(), int(AudioSequences[ind_sequences]->seqStarted
				|| AudioSequences[ind_sequences]->seqPlaying)); pg_send_message_udp((char *)"f", (char *)AuxString, (char *)"udp_PD_send");
			sprintf(AuxString, "/%s_can %.2f", AudioSequences[ind_sequences]->name.c_str(), AudioSequences[ind_sequences]->seqLevel); pg_send_message_udp((char *)"f", (char *)AuxString, (char *)"udp_PD_send");
		}
		for (int ind_long_tracks = 0; ind_long_tracks < NbLongTracks; ind_long_tracks++) {
			size_t pos = LongTrackBufferData[ind_long_tracks]->name.find_last_of('/');
			string filename = LongTrackBufferData[ind_long_tracks]->name;
			if (pos != std::string::npos) {
				filename = LongTrackBufferData[ind_long_tracks]->name.substr(pos + 1);
			}
			pos = filename.find_last_of('.');
			if (pos != std::string::npos) {
				filename.resize(pos);
			}
			sprintf(AuxString, "/%s_onOff_can %d", filename.c_str(), int(LongTrackStreamings[ind_long_tracks]->trackStarted
				|| LongTrackStreamings[ind_long_tracks]->trackPlaying)); pg_send_message_udp((char *)"f", (char *)AuxString, (char *)"udp_PD_send");
			sprintf(AuxString, "/%s_can %.2f", filename.c_str(), LongTrackStreamings[ind_long_tracks]->trackLevel); pg_send_message_udp((char *)"f", (char *)AuxString, (char *)"udp_PD_send");
		}

		sprintf(AuxString, "/record_output_can %d", int(record_output)); pg_send_message_udp((char *)"f", (char *)AuxString, (char *)"udp_PD_send");

		// if (InitialPerformanceTime == -1.0) {"
		// starts the time reference when the first streaming is made
		// only made at first launch
		if (!pg_Launched_Perfromance) {
			InitialPerformanceTime = RealTime();
			StreamingRealTime = 0.; // the time associated with the samples played in the sound card 
									// (only corresponds to real time at the time when the sample is shipped)
			LastStreamingBeatRealTime = StreamingRealTime; // the sound card time at which occured the last beat
			NbStreamingBeats = 0;
			NbPorphyrographBeats = 0;
		}
		// stops backtracks if already launched
		else {
			for (int ind_long_tracks = 0; ind_long_tracks < NbLongTracks; ind_long_tracks++) {
				size_t pos = LongTrackBufferData[ind_long_tracks]->name.find_last_of('/');
				string filename = LongTrackBufferData[ind_long_tracks]->name;
				if (pos != std::string::npos) {
					filename = LongTrackBufferData[ind_long_tracks]->name.substr(pos + 1);
				}
				pos = filename.find_last_of('.');
				if (pos != std::string::npos) {
					filename.resize(pos);
				}
				if (LongTrackStreamings[ind_long_tracks]->trackStarted
					|| LongTrackStreamings[ind_long_tracks]->trackPlaying) {
					LongTrackStreamings[ind_long_tracks]->trackStarted = false;
					LongTrackStreamings[ind_long_tracks]->trackPlaying = false;
					printf("OFF track %s %.2f\n", filename.c_str(), LongTrackStreamings[ind_long_tracks]->trackLevel);
					sprintf(AuxString, "/%s_onOff_can %d", filename.c_str(), 0); pg_send_message_udp((char *)"f", (char *)AuxString, (char *)"udp_PD_send");
					sprintf(AuxString, "/%s_can %.2f", filename.c_str(), LongTrackStreamings[ind_long_tracks]->trackLevel); pg_send_message_udp((char *)"f", (char *)AuxString, (char *)"udp_PD_send");
				}
			}
		}

#ifdef PG_TERRAINS_VAGUES
		if (!LongTrackStreamings[0]->trackStarted
			&& !LongTrackStreamings[0]->trackPlaying) {
			// launches backtrack
			LongTrackStreamings[0]->trackStarted = true;
			LongTrackStreamings[0]->trackPlaying = false;
			std::string filename = LongTrackBufferData[0]->name;
			printf("ON track %s %.2f\n", filename.c_str(), LongTrackStreamings[0]->trackLevel);
			sprintf(AuxString, "/%s_onOff_can %d", filename.c_str(), 1); pg_send_message_udp((char *)"f", (char *)AuxString, (char *)"udp_PD_send");
			sprintf(AuxString, "/%s_can %.2f", filename.c_str(), LongTrackStreamings[0]->trackLevel); pg_send_message_udp((char *)"f", (char *)AuxString, (char *)"udp_PD_send");
			printf("***********************************************\n");
			printf("LAUNCH\n");
			printf("***********************************************\n");
			sprintf(AuxString, "/launch"); pg_send_message_udp((char *)"s", (char *)AuxString, (char *)"udp_Porphyrograph_send");
		}
#endif

		//}

		// initializes launched variable
		pg_Launched_Perfromance = true;

	}

	/////////////////////////////////////////////
	// terrains vagues sample management
	else if (commandAlias.compare("record_output") == 0) {
		if (record_output) {
			record_output = false;
			//////////////////////////////////////////////////////////////
			// DEALLOCATION OF RECORDING BUFFER IN CASE OF END OF RECORDING
			// finishes recording the last incomplete buffer 
			// and deallocates the recording buffer
			// should normally wait for thred writing of the last
			// full buffer to be finished
			printf("Stop recording\n\n");
			sf_writef_float(pg_recordingData->pg_recordingfile,
				pg_recordingData->pg_doubleRecordingbuffer[pg_active_recordingbuffer],
				pg_frame_recordingbuffer);
			delete pg_recordingData;
			pg_recordingData = NULL;
		}
		else {
			record_output = true;
			//////////////////////////////////////////////////////////////
			// ALLOCATION OF RECORDING BUFFER IN CASE OF RECORDING
			printf("Start recording\n\n");
			pg_recordingData = new RecordingDataStruct;
			pg_active_recordingbuffer = 0;
			pg_frame_recordingbuffer = 0;
		}
		sprintf(AuxString, "/record_output_can %d", int(record_output) ); pg_send_message_udp((char *)"f", (char *)AuxString, (char *)"udp_PD_send");
	}

#ifdef PG_TERRAINS_VAGUES

	/////////////////////////////////////////////
	// terrains vagues sample management
	// scene offest in case of relaunch
	else if (commandAlias.compare("scene_offset") == 0) {
		scene_offset = int(arguments[0]);
	}

	else  if (commandAlias.compare("TV_track_shuffle") == 0) {
		TV_track_shuffle = arguments[0];
		sprintf(AuxString, "/TV_track_shuffle_can %f", arguments[0]); pg_send_message_udp((char *)"f", (char *)AuxString, (char *)"udp_PD_send");
		return;
	}
	else  if (commandAlias.compare("TV_drone_mix") == 0) {
		TV_drone_mix = arguments[0];
		sprintf(AuxString, "/TV_drone_mix_can %f", arguments[0]); pg_send_message_udp((char *)"f", (char *)AuxString, (char *)"udp_PD_send");
		return;
	}
	else  if (commandAlias.compare("TV_drone_size") == 0) {
		TV_drone_size = long(arguments[0] / 1000.f * PG_AUDIO_SAMPLE_RATE);;
		sprintf(AuxString, "/TV_drone_size_can %f", arguments[0]); pg_send_message_udp((char *)"f", (char *)AuxString, (char *)"udp_PD_send");
		return;
	}
	else  if (commandAlias.compare("TV_swap_freq") == 0) {
		TV_swap_freq = arguments[0];
		sprintf(AuxString, "/TV_swap_freq_can %f", arguments[0]); pg_send_message_udp((char *)"f", (char *)AuxString, (char *)"udp_PD_send");
		return;
	}
	else  if (commandAlias.compare("TV_swap_dur") == 0) {
		TV_swap_dur = arguments[0];
		sprintf(AuxString, "/TV_swap_dur_can %f", arguments[0]); pg_send_message_udp((char *)"f", (char *)AuxString, (char *)"udp_PD_send");
		return;
	}
	else  if (commandAlias.compare("TV_blast") == 0) {
		if (!TV_blash_on) {
			// mutes all the sequences and the first track
			for (int ind_sequences = 0; ind_sequences < NbAudioSequences; ind_sequences++) {
				AudioSequenceStreaming *seq = AudioSequences[ind_sequences];
				seqLevels[ind_sequences] = seq->seqLevel;
				seq->seqLevel = 0.f;
			}
			if (NbLongTracks > 0) {
				track0Level = LongTrackStreamings[0]->trackLevel;
				LongTrackStreamings[0]->trackLevel = 0.f;
			}
			// launches the blast
			if (NbLongTracks > 1
				&& !LongTrackStreamings[1]->trackStarted
				&& !LongTrackStreamings[1]->trackPlaying) {
				LongTrackStreamings[1]->trackLevel = 0.5f;
				LongTrackStreamings[1]->trackStarted = true;
				LongTrackStreamings[1]->trackPlaying = false;
			}
			// tells porphyrograph to change scene
			sprintf(AuxString, "/setup_plus"); pg_send_message_udp((char *)"", (char *)AuxString, (char *)"udp_Porphyrograph_send");
			TV_blash_on = true;
		}
		else {
			// unmutes all the sequences and the first track
			for (int ind_sequences = 0; ind_sequences < NbAudioSequences; ind_sequences++) {
				AudioSequenceStreaming *seq = AudioSequences[ind_sequences];
				seq->seqLevel = seqLevels[ind_sequences];
			}
			if( NbLongTracks > 0) {
				LongTrackStreamings[0]->trackLevel = track0Level;
			}
			// the blast is over
			// launches the blast
			if (NbLongTracks > 1
				&& (LongTrackStreamings[1]->trackStarted
				|| LongTrackStreamings[1]->trackPlaying)) {
				LongTrackStreamings[1]->trackLevel = 0.5f;
				LongTrackStreamings[1]->trackStarted = false;
				LongTrackStreamings[1]->trackPlaying = false;
			}
			// tells porphyrograph to change scene
			sprintf(AuxString, "/setup_plus"); pg_send_message_udp((char *)"", (char *)AuxString, (char *)"udp_Porphyrograph_send");
			TV_blash_on = false;
		}
		return;
	}

#endif

	/////////////////////////////////////////////
	// LOOP STOP
	else if (commandAlias.compare("loop_stop") == 0) {
		int ind = int(arguments[0]);
		if (ind < PG_NB_LOOP_BUFFERS) {
			// stops both recording and playing
			if (LoopBuffer[ind]->loopRecording || LoopBuffer[ind]->loopPlaying) {
				LoopBuffer[ind]->loopRecording = false;
				LoopBuffer[ind]->loopPlaying = false;
				printf("Loop %d stopped\n", ind + 1);
				LoopBuffer[ind]->time_stopped = RealTime();
				sprintf(AuxString, "/loop_recording_can %d %d", ind, int(LoopBuffer[ind]->loopRecording)); pg_send_message_udp((char *)"i i", (char *)AuxString, (char *)"udp_PD_send");
				sprintf(AuxString, "/loop_playing_can %d %d", ind, int(LoopBuffer[ind]->loopPlaying)); pg_send_message_udp((char *)"i i", (char *)AuxString, (char *)"udp_PD_send");
			}
			// already stopped or not yet started to record, nothing to do
			else if (RealTime() - LoopBuffer[ind]->time_stopped < 0.2) {
				printf("Loop %d cleaned\n", ind + 1);
				LoopBuffer[ind]->loopEmpty = true;
				LoopBuffer[ind]->loopFlashing = true;
				memset((void *)LoopBuffer[ind]->loopSoundBuffer, 0, (LoopBuffer[ind]->nbLoopFrames * PG_AUDIO_NB_CHANNELS) * sizeof(float));
				// flashes the two buttons
				sprintf(AuxString, "/loop_recording_can %d %d", ind, 1); pg_send_message_udp((char *)"i i", (char *)AuxString, (char *)"udp_PD_send");
				sprintf(AuxString, "/loop_playing_can %d %d", ind, 1); pg_send_message_udp((char *)"i i", (char *)AuxString, (char *)"udp_PD_send");
				LoopBuffer[ind]->time_stopped = RealTime();
			}
			// takes note of stopping time
			else {
				LoopBuffer[ind]->time_stopped = RealTime();
			}
			return;
		}
	}

	/////////////////////////////////////////////
	// LOOP RECORD PLAY
	else if (commandAlias.compare("loop_recPlay") == 0) {
		int ind = int(arguments[0]);
		if (ind < PG_NB_LOOP_BUFFERS) {
			// not playing/not recording
			if (!LoopBuffer[ind]->loopRecording && !LoopBuffer[ind]->loopPlaying) {
				// empty loop -> records
				if (LoopBuffer[ind]->loopEmpty) {
					LoopBuffer[ind]->loopRecording = true;
					LoopBuffer[ind]->loopPlaying = false;
					LoopBuffer[ind]->loopEmpty = false;
					printf("Loop %d recording started\n", ind + 1);
				}
				// otherwise plays
				else {
					LoopBuffer[ind]->loopRecording = false;
					LoopBuffer[ind]->loopPlaying = true;
					printf("Loop %d playing started\n", ind + 1);
				}
			}
			// playing -> recording
			else if (LoopBuffer[ind]->loopPlaying) { // && !LoopBuffer[ind]->loopRecording
				LoopBuffer[ind]->loopRecording = true;
				LoopBuffer[ind]->loopPlaying = false;
				printf("Loop %d recording re-started\n", ind + 1);
			}
			// recording -> playing
			else if (LoopBuffer[ind]->loopRecording) { // && !LoopBuffer[ind]->loopPlaying
				LoopBuffer[ind]->loopRecording = false;
				LoopBuffer[ind]->loopPlaying = true;
				printf("Loop %d playing started\n", ind + 1);
			}
			// that's all folks
			else {
				printf("Recording and playing together should not happen\n");
			}
			sprintf(AuxString, "/loop_recording_can %d %d", ind, int(LoopBuffer[ind]->loopRecording)); pg_send_message_udp((char *)"i i", (char *)AuxString, (char *)"udp_PD_send");
			sprintf(AuxString, "/loop_playing_can %d %d", ind, int(LoopBuffer[ind]->loopPlaying)); pg_send_message_udp((char *)"i i", (char *)AuxString, (char *)"udp_PD_send");
			return;
		}
	}

	/////////////////////////////////////////////
	// LOOP OUTPUT LEVEL
	else if (commandAlias.compare("loop_out_level") == 0) {
		int ind = int(arguments[0]);
		if (ind < PG_NB_LOOP_BUFFERS) {
			LoopBuffer[ind]->loopOutLevel = arguments[1];
			sprintf(AuxString, "/loop_out_level_can %d %f", ind, arguments[1]); pg_send_message_udp((char *)"f", (char *)AuxString, (char *)"udp_PD_send");
			return;
		}
	}

	/////////////////////////////////////////////
	// LOOP OUTPUT MASTER LEVEL
	else if (commandAlias.compare("loop_master_level") == 0) {
		loop_master_level = arguments[0];
	}

	/////////////////////////////////////////////
	// INPUT LEVEL
	else  if (commandAlias.compare("input_level") == 0) {
		input_level = arguments[0];
		sprintf(AuxString, "/input_level_can %f", arguments[0]); pg_send_message_udp((char *)"f", (char *)AuxString, (char *)"udp_PD_send");
		return;
	}

	/////////////////////////////////////////////
	// PIXEL TO SOUND SENSOR LEVEL
	else  if (commandAlias.compare("sensor_vol") == 0) {
		float sensor_vol = arguments[0];
		sprintf(AuxString, "/sensor_vol %.2f", sensor_vol); pg_send_message_udp((char *)"f", (char *)AuxString, (char *)"udp_Porphyrograph_send");
		return;
	}

	/////////////////////////////////////////////
	// BEATS
	else  if (commandAlias.compare("BeatsPerMinute") == 0) {
		BeatsPerMinute = arguments[0];
		BeatDurationRealTime = 60.0f / (float)BeatsPerMinute;
		for (int indLoopTrack = 0; indLoopTrack < PG_NB_LOOP_BUFFERS; indLoopTrack++) {
			LoopBuffer[indLoopTrack]->loopDur = 16. * BeatDurationRealTime;
			LoopBuffer[indLoopTrack]->nbLoopFrames = long(LoopBuffer[indLoopTrack]->loopDur * PG_AUDIO_SAMPLE_RATE);
			if (LoopBuffer[indLoopTrack]->nbLoopFrames > LoopBuffer[indLoopTrack]->nbBufferFrames) {
				if (LoopBuffer[indLoopTrack]->loopSoundBuffer) {
					delete[]LoopBuffer[indLoopTrack]->loopSoundBuffer;
				}
				LoopBuffer[indLoopTrack]->nbBufferFrames = LoopBuffer[indLoopTrack]->nbLoopFrames;
				LoopBuffer[indLoopTrack]->loopSoundBuffer = new float[(unsigned long)(LoopBuffer[indLoopTrack]->nbBufferFrames * PG_AUDIO_NB_CHANNELS)];
				memset((void *)LoopBuffer[indLoopTrack]->loopSoundBuffer, 0, (LoopBuffer[indLoopTrack]->nbBufferFrames * PG_AUDIO_NB_CHANNELS) * sizeof(float));
			}
			else {
				memset((void *)LoopBuffer[indLoopTrack]->loopSoundBuffer, 0, (LoopBuffer[indLoopTrack]->nbLoopFrames * PG_AUDIO_NB_CHANNELS) * sizeof(float));
			}
		}
		return;
	}

	/////////////////////////////////////////////
	// REVERB
	else if (commandAlias.compare("mix") == 0) {
		if (!isSampleEffectCommand) {
			if (arguments[0] < 0.5f) {
				VST_reverb_inLine.setParameter(MVerb<float>::MIX, .0f);
				sprintf(AuxString, "/mix_can %f", .0f); pg_send_message_udp((char *)"f", (char *)AuxString, (char *)"udp_PD_send");
			}
			else {
				VST_reverb_inLine.setParameter(MVerb<float>::MIX, (arguments[0] - .5f) * .2f);
				sprintf(AuxString, "/mix_can %f", (arguments[0] - .5f) * .2f); pg_send_message_udp((char *)"f", (char *)AuxString, (char *)"udp_PD_send");
			}
		}
		else {
			if (arguments[0] < 0.5f) {
				VST_reverb_samples.setParameter(MVerb<float>::MIX, .0f);
				sprintf(AuxString, "/sample_mix_can %f", .0f); pg_send_message_udp((char *)"f", (char *)AuxString, (char *)"udp_PD_send");
			}
			else {
				VST_reverb_samples.setParameter(MVerb<float>::MIX, (arguments[0] - .5f) * .2f);
				sprintf(AuxString, "/sample_mix_can %f", (arguments[0] - .5f) * .2f); pg_send_message_udp((char *)"f", (char *)AuxString, (char *)"udp_PD_send");
			}
		}
	}
	/*
	else if (commandAlias.compare("mix") == 0) {
		if (!isSampleEffectCommand) {
			VST_reverb_inLine.setParameter(MVerb<float>::MIX, arguments[0]);
			sprintf(AuxString, "/mix_can %f", arguments[0]); pg_send_message_udp((char *)"f", (char *)AuxString, (char *)"udp_PD_send");
		}
		else {
			VST_reverb_samples.setParameter(MVerb<float>::MIX, arguments[0]);
			sprintf(AuxString, "/sample_mix_can %f", arguments[0]); pg_send_message_udp((char *)"f", (char *)AuxString, (char *)"udp_PD_send");
		}
	} */
	else  if (commandAlias.compare("dampingfreq") == 0) {
		if (!isSampleEffectCommand) {
			VST_reverb_inLine.setParameter(MVerb<float>::DAMPINGFREQ, arguments[0]);
			sprintf(AuxString, "/dampingfreq_can %f", arguments[0]); pg_send_message_udp((char *)"f", (char *)AuxString, (char *)"udp_PD_send");
		}
		else {
			VST_reverb_samples.setParameter(MVerb<float>::DAMPINGFREQ, arguments[0]);
			sprintf(AuxString, "/sample_dampingfreq_can %f", arguments[0]); pg_send_message_udp((char *)"f", (char *)AuxString, (char *)"udp_PD_send");
		}
	}
	else if (commandAlias.compare("density") == 0) {
		if (!isSampleEffectCommand) {
			VST_reverb_inLine.setParameter(MVerb<float>::DENSITY, arguments[0]);
			sprintf(AuxString, "/density_can %f", arguments[0]); pg_send_message_udp((char *)"f", (char *)AuxString, (char *)"udp_PD_send");
		}
		else {
			VST_reverb_samples.setParameter(MVerb<float>::DENSITY, arguments[0]);
			sprintf(AuxString, "/sample_density_can %f", arguments[0]); pg_send_message_udp((char *)"f", (char *)AuxString, (char *)"udp_PD_send");
		}
	}
	else if (commandAlias.compare("bandwidthfreq") == 0) {
		if (!isSampleEffectCommand) {
			VST_reverb_inLine.setParameter(MVerb<float>::BANDWIDTHFREQ, arguments[0]);
			sprintf(AuxString, "/bandwidthfreq_can %f", arguments[0]); pg_send_message_udp((char *)"f", (char *)AuxString, (char *)"udp_PD_send");
		}
		else {
			VST_reverb_samples.setParameter(MVerb<float>::BANDWIDTHFREQ, arguments[0]);
			sprintf(AuxString, "/sample_bandwidthfreq_can %f", arguments[0]); pg_send_message_udp((char *)"f", (char *)AuxString, (char *)"udp_PD_send");
		}
	}
	else if (commandAlias.compare("decay") == 0) {
		if (!isSampleEffectCommand) {
			VST_reverb_inLine.setParameter(MVerb<float>::DECAY, arguments[0]);
			sprintf(AuxString, "/decay_can %f", arguments[0]); pg_send_message_udp((char *)"f", (char *)AuxString, (char *)"udp_PD_send");
		}
		else {
			VST_reverb_samples.setParameter(MVerb<float>::DECAY, arguments[0]);
			sprintf(AuxString, "/sample_decay_can %f", arguments[0]); pg_send_message_udp((char *)"f", (char *)AuxString, (char *)"udp_PD_send");
		}
	}
	else if (commandAlias.compare("predelay") == 0) {
		if (!isSampleEffectCommand) {
			VST_reverb_inLine.setParameter(MVerb<float>::PREDELAY, arguments[0]);
			sprintf(AuxString, "/predelay_can %f", arguments[0]); pg_send_message_udp((char *)"f", (char *)AuxString, (char *)"udp_PD_send");
		}
		else {
			VST_reverb_samples.setParameter(MVerb<float>::PREDELAY, arguments[0]);
			sprintf(AuxString, "/sample_predelay_can %f", arguments[0]); pg_send_message_udp((char *)"f", (char *)AuxString, (char *)"udp_PD_send");
		}
	}
	else if (commandAlias.compare("size") == 0) {
		if (!isSampleEffectCommand) {
			VST_reverb_inLine.setParameter(MVerb<float>::SIZE, arguments[0]);
			sprintf(AuxString, "/size_can %f", arguments[0]); pg_send_message_udp((char *)"f", (char *)AuxString, (char *)"udp_PD_send");
		}
		else {
			VST_reverb_samples.setParameter(MVerb<float>::SIZE, arguments[0]);
			sprintf(AuxString, "/sample_size_can %f", arguments[0]); pg_send_message_udp((char *)"f", (char *)AuxString, (char *)"udp_PD_send");
		}
	}
	else if (commandAlias.compare("gain") == 0) {
		if (!isSampleEffectCommand) {
			VST_reverb_inLine.setParameter(MVerb<float>::GAIN, arguments[0]);
			sprintf(AuxString, "/gain_can %f", arguments[0]); pg_send_message_udp((char *)"f", (char *)AuxString, (char *)"udp_PD_send");
		}
		else {
			VST_reverb_samples.setParameter(MVerb<float>::GAIN, arguments[0]);
			sprintf(AuxString, "/sample_gain_can %f", arguments[0]); pg_send_message_udp((char *)"f", (char *)AuxString, (char *)"udp_PD_send");
		}
	}
	else if (commandAlias.compare( "earlymix") == 0) {
		if (!isSampleEffectCommand) {
			VST_reverb_inLine.setParameter(MVerb<float>::EARLYMIX, arguments[0]);
			sprintf(AuxString, "/earlymix_can %f", arguments[0]); pg_send_message_udp((char *)"f", (char *)AuxString, (char *)"udp_PD_send");
		}
		else {
			VST_reverb_samples.setParameter(MVerb<float>::EARLYMIX, arguments[0]);
			sprintf(AuxString, "/sample_earlymix_can %f", arguments[0]); pg_send_message_udp((char *)"f", (char *)AuxString, (char *)"udp_PD_send");
		}
	}

	/////////////////////////////////////////////
	// DELAY
	else if (commandAlias.compare( "kMix") == 0) {
		if (!isSampleEffectCommand) {
			if (arguments[0] < 0.5f) {
				VST_delay_inLine.setParameter(kMix, .0f);
				sprintf(AuxString, "/kMix_can %f", .0f); pg_send_message_udp((char *)"f", (char *)AuxString, (char *)"udp_PD_send");
			}
			else {
				VST_delay_inLine.setParameter(kMix, (arguments[0] - .5f) * .2f);
				sprintf(AuxString, "/kMix_can %f", (arguments[0] - .5f) * .2f); pg_send_message_udp((char *)"f", (char *)AuxString, (char *)"udp_PD_send");
			}

		}
		else {
			if (arguments[0] < 0.5f) {
				VST_delay_samples.setParameter(kMix, .0f);
				sprintf(AuxString, "/sample_kMix_can %f", .0f); pg_send_message_udp((char *)"f", (char *)AuxString, (char *)"udp_PD_send");
			}
			else {
				VST_delay_samples.setParameter(kMix, (arguments[0] - .5f) * .2f);
				sprintf(AuxString, "/sample_kMix_can %f", (arguments[0] - .5f) * .2f); pg_send_message_udp((char *)"f", (char *)AuxString, (char *)"udp_PD_send");
			}

		}
	}
	/* else if (commandAlias.compare("kMix") == 0) {
		if (!isSampleEffectCommand) {
			VST_delay_inLine.setParameter(kMix, arguments[0]);
			sprintf(AuxString, "/kMix_can %f", arguments[0]); pg_send_message_udp((char *)"f", (char *)AuxString, (char *)"udp_PD_send");
		}
		else {
			VST_delay_samples.setParameter(kMix, arguments[0]);
			sprintf(AuxString, "/sample_kMix_can %f", arguments[0]); pg_send_message_udp((char *)"f", (char *)AuxString, (char *)"udp_PD_send");
		}
	} */
	else if (commandAlias.compare( "kLDelay") == 0) {
		if (!isSampleEffectCommand) {
			VST_delay_inLine.setParameter(kLDelay, arguments[0]);
			sprintf(AuxString, "/kPreDelay_can %f", arguments[0]); pg_send_message_udp((char *)"f", (char *)AuxString, (char *)"udp_PD_send");
		}
		else {
			VST_delay_samples.setParameter(kLDelay, arguments[0]);
			sprintf(AuxString, "/sample_kPreDelay_can %f", arguments[0]); pg_send_message_udp((char *)"f", (char *)AuxString, (char *)"udp_PD_send");
		}
	}
	else if (commandAlias.compare( "kLRDelayRatio") == 0) {
		if (!isSampleEffectCommand) {
			VST_delay_inLine.setParameter(kLRDelayRatio, arguments[0]);
			sprintf(AuxString, "/kDecay_can %f", arguments[0]); pg_send_message_udp((char *)"f", (char *)AuxString, (char *)"udp_PD_send");
		}
		else {
			VST_delay_samples.setParameter(kLRDelayRatio, arguments[0]);
			sprintf(AuxString, "/sample_kDecay_can %f", arguments[0]); pg_send_message_udp((char *)"f", (char *)AuxString, (char *)"udp_PD_send");
		}
	}
	else if (commandAlias.compare( "kFeedBk") == 0) {
		if (!isSampleEffectCommand) {
			VST_delay_inLine.setParameter(kFeedBk, arguments[0]);
			sprintf(AuxString, "/kFeedBk_can %f", arguments[0]); pg_send_message_udp((char *)"f", (char *)AuxString, (char *)"udp_PD_send");
		}
		else {
			VST_delay_samples.setParameter(kFeedBk, arguments[0]);
			sprintf(AuxString, "/sample_kFeedBk_can %f", arguments[0]); pg_send_message_udp((char *)"f", (char *)AuxString, (char *)"udp_PD_send");
		}
	}
	else if (commandAlias.compare( "kTone") == 0) {
		if (!isSampleEffectCommand) {
			VST_delay_inLine.setParameter(kTone, arguments[0]);
			sprintf(AuxString, "/kTone_can %f", arguments[0]); pg_send_message_udp((char *)"f", (char *)AuxString, (char *)"udp_PD_send");
		}
		else {
			VST_delay_samples.setParameter(kTone, arguments[0]);
			sprintf(AuxString, "/sample_kTone_can %f", arguments[0]); pg_send_message_udp((char *)"f", (char *)AuxString, (char *)"udp_PD_send");
		}
	}

	else if (commandAlias.compare( "kLevel") == 0) {
		if (!isSampleEffectCommand) {
			VST_delay_inLine.setParameter(kLevel, arguments[0]);
			sprintf(AuxString, "/kLevel_can %f", arguments[0]); pg_send_message_udp((char *)"f", (char *)AuxString, (char *)"udp_PD_send");
		}
		else {
			VST_delay_samples.setParameter(kLevel, arguments[0]);
			sprintf(AuxString, "/sample_kLevel_can %f", arguments[0]); pg_send_message_udp((char *)"f", (char *)AuxString, (char *)"udp_PD_send");
		}
	}

	/////////////////////////////////////////////
	// WET/DRY FOR FX ON SAMPLES OR ON LINE IN
	//else if (commandAlias.compare( "sampleFXMix") == 0) {
	//	sampleFXMix = arguments[0];
	//}

	//else if (commandAlias.compare( "lineInFXMix") == 0) {
	//	lineInFXMix = arguments[0];
	//}

	/////////////////////////////////////////////
	// commands for sequences or long tracks
	else {
		for (int ind_sequences = 0; ind_sequences < NbAudioSequences; ind_sequences++) {
			// sequence on off
			if (commandAlias.compare(AudioSequences[ind_sequences]->name + "_onOff") == 0) {
				if (AudioSequences[ind_sequences]->seqStarted
					|| AudioSequences[ind_sequences]->seqPlaying) {
					AudioSequences[ind_sequences]->seqStarted = false;
					AudioSequences[ind_sequences]->seqPlaying = false;
					AudioSequences[ind_sequences]->seqLevel = 0.f;
					AudioSequences[ind_sequences]->nbSeqRepetitions = 0;
					printf("OFF sequence %s %.2f\n", AudioSequences[ind_sequences]->name.c_str(), AudioSequences[ind_sequences]->seqLevel);
					sprintf(AuxString, "/%s_onOff_can %d", AudioSequences[ind_sequences]->name.c_str(), 0); pg_send_message_udp((char *)"f", (char *)AuxString, (char *)"udp_PD_send");
					sprintf(AuxString, "/%s_can %.2f", AudioSequences[ind_sequences]->name.c_str(), AudioSequences[ind_sequences]->seqLevel); pg_send_message_udp((char *)"f", (char *)AuxString, (char *)"udp_PD_send");
				}
				else {
					AudioSequences[ind_sequences]->seqStarted = true;
					AudioSequences[ind_sequences]->seqPlaying = false;
					AudioSequences[ind_sequences]->seqLevel = 0.f;
					AudioSequences[ind_sequences]->nbSeqRepetitions = 0;
					printf("ON sequence %s %.2f\n", AudioSequences[ind_sequences]->name.c_str(), AudioSequences[ind_sequences]->seqLevel);
					sprintf(AuxString, "/%s_onOff_can %d", AudioSequences[ind_sequences]->name.c_str(), 1); pg_send_message_udp((char *)"f", (char *)AuxString, (char *)"udp_PD_send");
					sprintf(AuxString, "/%s_can %.2f", AudioSequences[ind_sequences]->name.c_str(), AudioSequences[ind_sequences]->seqLevel); pg_send_message_udp((char *)"f", (char *)AuxString, (char *)"udp_PD_send");
				}
#ifdef PG_TERRAINS_VAGUES
				if (AudioSequences[ind_sequences]->isSequenceTerrainVagues
					&& InitialTerrainVaguesTime == -1) {
					InitialTerrainVaguesTime = RealTime();
				}
#endif
				return;
			}

			// level of sequences
			if (commandAlias.compare(AudioSequences[ind_sequences]->name + "_level") == 0) {
				AudioSequences[ind_sequences]->seqLevel = arguments[0];
				// printf("sequence %s %.2f\n", AudioSequences[ind_sequences]->name.c_str(), AudioSequences[ind_sequences]->seqLevel);
				sprintf(AuxString, "/%s_can %f", AudioSequences[ind_sequences]->name.c_str(), arguments[0]); pg_send_message_udp((char *)"f", (char *)AuxString, (char *)"udp_PD_send");

				if (!AudioSequences[ind_sequences]->isSequenceTerrainVagues) {
					if (AudioSequences[ind_sequences]->seqLevel > 0.0f
						&& !AudioSequences[ind_sequences]->seqStarted
						&& !AudioSequences[ind_sequences]->seqPlaying) {
						AudioSequences[ind_sequences]->seqStarted = true;
						AudioSequences[ind_sequences]->seqPlaying = false;
						AudioSequences[ind_sequences]->nbSeqRepetitions = 0;
						// printf("start sequence %s %.2f\n", AudioSequences[ind_sequences]->name.c_str(), AudioSequences[ind_sequences]->seqLevel);
					}
					else if (AudioSequences[ind_sequences]->seqLevel <= 0.0f
						&& (AudioSequences[ind_sequences]->seqStarted
							|| AudioSequences[ind_sequences]->seqPlaying)) {
						AudioSequences[ind_sequences]->seqStarted = false;
						AudioSequences[ind_sequences]->seqPlaying = false;
						AudioSequences[ind_sequences]->seqLevel = 0.f;
						AudioSequences[ind_sequences]->nbSeqRepetitions = 0;
						// printf("stop sequence %s %.2f\n", AudioSequences[ind_sequences]->name.c_str(), AudioSequences[ind_sequences]->seqLevel);
					}
				}

				// CJ UNCOMMENTED FEB 2018
#ifdef PG_TERRAINS_VAGUES
				if (AudioSequences[ind_sequences]->isSequenceTerrainVagues
					&& InitialTerrainVaguesTime == -1) {
					InitialTerrainVaguesTime = RealTime();
				}
#endif
				return;
			}
		}

		// level or on_off of long tracks
		for (int ind_long_tracks = 0; ind_long_tracks < NbLongTracks; ind_long_tracks++) {
			size_t pos = LongTrackBufferData[ind_long_tracks]->name.find_last_of('/');
			string filename = LongTrackBufferData[ind_long_tracks]->name;
			if (pos != std::string::npos) {
				filename = LongTrackBufferData[ind_long_tracks]->name.substr(pos + 1);
			}
			pos = filename.find_last_of('.');
			if (pos != std::string::npos) {
				filename.resize(pos);
			}
			// printf("file name long track %s %d\n", filename.c_str(), ind_long_tracks);
			if (commandAlias.compare(filename + "_level") == 0) {
				LongTrackStreamings[ind_long_tracks]->trackLevel = arguments[0];
				sprintf(AuxString, "/%s_can %.2f", filename.c_str(), LongTrackStreamings[ind_long_tracks]->trackLevel); pg_send_message_udp((char *)"f", (char *)AuxString, (char *)"udp_PD_send");
				printf("long track %s %.2f\n", LongTrackBufferData[ind_long_tracks]->name.c_str(),
					LongTrackStreamings[ind_long_tracks]->trackLevel);

				/*
				// starts a track that is not playing if level is > 0
				// disabled for terrains vagues
#ifndef PG_TERRAINS_VAGUES
				if (LongTrackStreamings[ind_long_tracks]->trackLevel > 0.0f
					&& !LongTrackStreamings[ind_long_tracks]->trackStarted
					&& !LongTrackStreamings[ind_long_tracks]->trackPlaying) {
					LongTrackStreamings[ind_long_tracks]->trackStarted = true;
					LongTrackStreamings[ind_long_tracks]->trackPlaying = false;
				}
#endif
                */

				return;
			}

				// printf("file name long track %s %d\n", filename.c_str(), ind_long_tracks);
			if (commandAlias.compare(filename + "_onOff") == 0) {
				// stops a track that is playing 
				if (LongTrackStreamings[ind_long_tracks]->trackStarted
					|| LongTrackStreamings[ind_long_tracks]->trackPlaying) {
					LongTrackStreamings[ind_long_tracks]->trackStarted = false;
					LongTrackStreamings[ind_long_tracks]->trackPlaying = false;
					printf("OFF track %s %.2f\n", filename.c_str(), LongTrackStreamings[ind_long_tracks]->trackLevel);
					sprintf(AuxString, "/%s_onOff_can %d", filename.c_str(), 0); pg_send_message_udp((char *)"f", (char *)AuxString, (char *)"udp_PD_send");
					sprintf(AuxString, "/%s_can %.2f", filename.c_str(), LongTrackStreamings[ind_long_tracks]->trackLevel); pg_send_message_udp((char *)"f", (char *)AuxString, (char *)"udp_PD_send");
				}
				// starts a track that is not playing 
				else {
					LongTrackStreamings[ind_long_tracks]->trackStarted = true;
					LongTrackStreamings[ind_long_tracks]->trackPlaying = false;
					printf("ON track %s %.2f\n", filename.c_str(), LongTrackStreamings[ind_long_tracks]->trackLevel);
					sprintf(AuxString, "/%s_onOff_can %d", filename.c_str(), 1); pg_send_message_udp((char *)"f", (char *)AuxString, (char *)"udp_PD_send");
					sprintf(AuxString, "/%s_can %.2f", filename.c_str(), LongTrackStreamings[ind_long_tracks]->trackLevel); pg_send_message_udp((char *)"f", (char *)AuxString, (char *)"udp_PD_send");
					printf("***********************************************\n");
					printf("LAUNCH\n");
					printf("***********************************************\n");
					sprintf(AuxString, "/launch"); pg_send_message_udp((char *)"s", (char *)AuxString, (char *)"udp_Porphyrograph_send");
				}

				return;
			}
		}
	}
}

void pg_update_scene_number_for_sample_and_visuals(int ind_scene) {
	printf("************ Scene update %d\n", ind_scene);
	IndRhythmsSampleDir = ind_scene;
	//IndPlayedAudioDronesSampleDir = ind_scene;
	if (ind_scene != 0) {
		IndPlayedAudioRhythmsSample = 0;
		//IndPlayedAudioDronesSample = 0;
	}
	else {
		// global variable for sample reading is set to PG_AUDIO_NB_SAMPLES
		// for the first scene because PG_AUDIO_NB_SAMPLES have already been
		// loaded at initialization
		IndPlayedAudioRhythmsSample = PG_AUDIO_NB_SAMPLES;
		//IndPlayedAudioDronesSample = PG_AUDIO_NB_SAMPLES;
	}
}


#ifdef WIN32
DWORD WINAPI processUDP(LPVOID lpParam) {
#else
void* processUDP(void * lpParam) {
#endif
	int *pData = (int *)lpParam;

	while (!pg_time_to_quit) {
		// real time management
		unsigned long newNbPorphyrographBeats 
			= long(floor((RealTime() - InitialPerformanceTime) / BeatDurationRealTime)) % BeatsPerSequence;
		if (newNbPorphyrographBeats != NbPorphyrographBeats) {
			NbPorphyrographBeats = newNbPorphyrographBeats;
			sprintf(AuxString, "/beat %d", NbPorphyrographBeats); pg_send_message_udp((char *)"f", (char *)AuxString, (char *)"udp_PD_send");
			pg_send_message_udp((char *)"f", (char *)AuxString, (char *)"udp_Porphyrograph_send");
			// printf("real time beat %d %.3f\n", NbPorphyrographBeats, BeatDurationRealTime);
			// printf( "/loop_level_can %f\n", LoopBuffer->loopLevel);
		}

#ifdef PG_TERRAINS_VAGUES
		if (SoundMachineRhythms) {
			if (InitialTerrainVaguesTime != -1) {
				float elapsed_time_from_start = (float)(RealTime() - InitialTerrainVaguesTime);

				int indscene = (int(elapsed_time_from_start) / 60) % PG_AUDIO_NB_DIRS;
				if (indscene > IndCurrentScene) {
					pg_update_scene_number_for_sample_and_visuals(indscene + scene_offset);
					IndCurrentScene = indscene;
				}
			}

			// updates sample swapping
			// SoundMachineDrones->update_sample_buffer_swapping();
			SoundMachineRhythms->update_sample_buffer_swapping();
		}
#endif

		// stops flash in case of track cleaning
		for (int ind = 0; ind < PG_NB_LOOP_BUFFERS; ind++) {
			if (LoopBuffer[ind]->loopFlashing && RealTime() - LoopBuffer[ind]->time_stopped > 0.5) {
				LoopBuffer[ind]->loopFlashing = false;
				sprintf(AuxString, "/loop_recording_can %d %d", ind, 0); pg_send_message_udp((char *)"i i", (char *)AuxString, (char *)"udp_PD_send");
				sprintf(AuxString, "/loop_playing_can %d %d", ind, 0); pg_send_message_udp((char *)"i i", (char *)AuxString, (char *)"udp_PD_send");
			}
		}

		// UDP message management
		for (int ind = 0; ind < nb_IP_Servers; ind++) {
			IP_Servers[ind]->receiveIPMessages();
		}
		for (int ind = 0; ind < nb_IP_Clients; ind++) {
			IP_Clients[ind]->sendIPmessages();
		}

#ifdef WIN32
		Sleep(10);
#else
		sleep(10);
#endif
	}
	printf("quit processUDP\n");
	return NULL;
}

////////////////////////////////////////////////////////////////////
// MEMORY DEALLOCATION BEFORE EXITING
////////////////////////////////////////////////////////////////////
void pg_audio_deallocation(void) {
	printf("Audio deallocation\n");
	/* -- Now we stop the stream -- */
	PaError err = Pa_StopStream(pg_audio_stream);
	if (err != paNoError) {
		printf("Error when stopping audio stream\n");
		pg_audio_errorMsg(err);
	}
	err = Pa_CloseStream(pg_audio_stream);
	if (err != paNoError) {
		printf("Error when closing audio stream\n");
		pg_audio_errorMsg(err);
	}
	Pa_Terminate();

#ifdef PG_TERRAINS_VAGUES
	// releases the memory and the associated samples
	if (SoundMachineRhythms) {
		delete SoundMachineRhythms;
		SoundMachineRhythms = NULL;
	}
#endif
	//
	//// deallocates file samples 
	//for (int ind = 0; ind < PG_NB_SHORT_TRACKS; ind++) {
	//	if (ShortTrackBufferData[ind] && !ShortTrackBufferData[ind]->name.empty()) {
	//		ShortTrackBufferData[ind]->release();
	//		 ShortTrackBufferData[ind] = NULL;
	//	}
	//};

	// finishes recording the last incomplete buffer 
	// and deallocates the recording buffer
	// should normally wait for thred writing of the last
	// full buffer to be finished
	if (record_output) {
		record_output = false;
		sprintf(AuxString, "/record_output_can %d", int(record_output)); pg_send_message_udp((char *)"f", (char *)AuxString, (char *)"udp_PD_send");
		sf_writef_float(pg_recordingData->pg_recordingfile,
			pg_recordingData->pg_doubleRecordingbuffer[pg_active_recordingbuffer],
			pg_frame_recordingbuffer);
		delete pg_recordingData;
	}

	printf("Audio deallocated\n");

}

////////////////////////////////////////////////////////////////////
// MAIN
////////////////////////////////////////////////////////////////////
BOOL CtrlHandler(DWORD fdwCtrlType)
{
	switch (fdwCtrlType)
	{
		// Handle the CTRL-C signal. 
	case CTRL_C_EVENT:
		printf("Ctrl-C event\n\n");
		pg_audio_deallocation();
		pg_time_to_quit = true;
		return true;

		// CTRL-CLOSE: confirm that the user wants to exit. 
	case CTRL_CLOSE_EVENT:
		printf("Ctrl-Close event\n\n");
		pg_audio_deallocation();
		pg_time_to_quit = true;
		return true;

		// Pass other signals to the next handler. 
	case CTRL_BREAK_EVENT:
		printf("Ctrl-Break event\n\n");
		pg_audio_deallocation();
		pg_time_to_quit = true;
		return true;

	case CTRL_LOGOFF_EVENT:
		printf("Ctrl-Logoff event\n\n");
		pg_audio_deallocation();
		pg_time_to_quit = true;
		return true;

	case CTRL_SHUTDOWN_EVENT:
		printf("Ctrl-Shutdown event\n\n");
		pg_audio_deallocation();
		pg_time_to_quit = true;
		return true;

	default:
		printf("Unknown termination event\n\n");
		pg_audio_deallocation();
		pg_time_to_quit = true;
		return true;
	}
}
 
/// MAIN FUNCTION
int main(int argc, char * argv[]) {
	string configuration_file_name = "";
	if (argc == 2) {
		configuration_file_name = string(argv[1]);
	}
	else {
		std::cout << "Error: no configuration file name provided: argc " << argc << "!"<< std::endl;
		std::cout << "Porphyrograph_sound <configuration file name>" << std::endl;
		exit(-1);
	}
	// error message and input buffer memory
	ErrorStr = new char[1024];

	// current date/time based on current system
	time_t now = time(0);
	tm *ltm = localtime(&now);

	// print various components of tm structure.
	//std::cout << "Year: "<< 1900 + ltm->tm_year << std::endl;
	//std::cout << "Month: "<< 1 + ltm->tm_mon<< std::endl;
	//std::cout << "Day: "<<  ltm->tm_mday << std::endl;
	//std::cout << "Time: "<< 1 + ltm->tm_hour << ":";
	//std::cout << 1 + ltm->tm_min << ":";
	//std::cout << 1 + ltm->tm_sec << std::endl;

	date_stringStream << std::setfill('0') << std::setw(2) << ltm->tm_hour << "-"
		<< std::setfill('0') << std::setw(2) << ltm->tm_min << "-"
		<< ltm->tm_year - 100 << "-"
		<< std::setfill('0') << std::setw(2) << 1 + ltm->tm_mon << "-"
		<< std::setfill('0') << std::setw(2) << ltm->tm_mday << std::flush;

	std::cout << "Date: [" << date_stringStream.str() << "]" << std::endl;

	// configuration file loading
	LoadConfigurationFile(configuration_file_name);

#ifdef PG_TERRAINS_VAGUES
	if (atLeastOneTerrainsVaguesSequence) {
		// audio initialization
		// SoundMachineDrones = new AudioSoundMachine("Drones");
		SoundMachineRhythms = new AudioSoundMachine("Rhythms");

		// loads the audio sample buffer and assigns memory to the moire buffer
		// SoundMachineDrones->pg_ReadInitalSampleBuffers(0);
		printf("\nLoading initial terrains vagues samples: ");
		for (int indDir = 0; indDir < PG_AUDIO_NB_DIRS; indDir++) {
			printf("%d ", indDir);
			SoundMachineRhythms->pg_ReadInitalSampleBuffers(indDir);
		}
		printf("\n\n");
	}
	else {
		SoundMachineRhythms = NULL;
	}
	IndRhythmsSampleDir = 0;
#endif

	StreamingRealTime = -1.0f;
	LastStreamingBeatRealTime = -1.0f;
	BeatDurationRealTime = 60.0f / (float)BeatsPerMinute;
	printf("beat duration %.3f\n", BeatDurationRealTime);
	NbStreamingBeats = 0;

	//printf("\nThe Control Handler is installed.\n");
	//printf("\n -- Now try pressing Ctrl+C or Ctrl+Break, or");
	//printf("\n    try logging off or closing the console...\n");
	//printf("\n(...waiting in a loop for events...)\n\n");

	udp_init();

	// file samples for later playback are loaded through the configuration file

	// samples regeneration
	if (false) {
		generate_all_samples();
	}

	// file plugin processing
	if (false) {
		process_file_plugin();
	}

	// loop initializing
	// loop duration 16 seconds
	for (int ind = 0; ind < PG_NB_LOOP_BUFFERS; ind++) {
		LoopBuffer[ind] = new AudioLoopBuffer(16. * BeatDurationRealTime);
	}

	// reverb initializing
	VST_reverb_samples.Init();
	VST_reverb_inLine.Init();

	// echo initializing
	VST_delay_samples.Init();
	VST_delay_inLine.Init();

	// effects initial values
	initial_values_FX();

	// thread: audio I/O + effects
	Pa_portaudio_init_and_effects();

	// connects PD to Porphyrograph
	sprintf(AuxString, "/connect 1"); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_PD_send");
	// launches first scene
	sprintf(AuxString, "/interfaceNbFromPorph 0"); pg_send_message_udp((char *)"f", (char *)AuxString, (char *)"udp_PD_send");

	// thread: UDP message processing + idle 
	int pData = 0;
#ifdef WIN32
	DWORD rc;
	HANDLE  hThread = CreateThread(
		NULL,                   // default security attributes
		0,                      // use default stack size  
		processUDP,		    // thread function name
		(void *)&pData,		    // argument to thread function 
		0,                      // use default creation flags 
		&rc);   // returns the thread identifier 
	if (hThread == NULL) {
		std::cout << "Error:unable to create thread processUDP" << std::endl;
		exit(-1);
	}
#else
	pthread_t processUDP_thread;
	int rc;
	rc = pthread_create(&processUDP_thread, NULL,
		processUDP, (void *)&pData);
	if (rc) {
		std::cout << "Error:unable to create thread processUDP" << rc << std::endl;
		pthread_exit(NULL);
	}
#endif

	// waits for a char to terminate (the computation is handled by threads)
	if (SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE))
	{
		printf("\n*Loading completed!*\n\n");
		WaitForSingleObject(hThread, INFINITE);
		printf("Application closing\n");
	}
	else
	{
		printf("\nERROR: Could not set control handler");
	}
	return 0;
}
