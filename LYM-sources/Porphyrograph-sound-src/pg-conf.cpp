/*! \file pg-conf.cpp
 *
 * 
 * 
 *     File pg-conf.cpp
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
#include "pg-all_include.h"

#include "pg_conf_body.cpp"

/*! \name Group constants
 *  \brief Enumerated constants
 */
// \{

std::string project_name = "sound";

const char *pg_UDPMessageFormatString[Emptypg_UDPMessageFormat + 1] = { "Plain", "OSC", "Emptypg_UDPMessageFormat" };

/////////////////////////////////////////////////////
// CONFIGURATION VARIABLES
/////////////////////////////////////////////////////
float InitialValuesConfigurationVar[_MaxConfigurationVarIDs];

/////////////////////////////////////////////////////
// global variable for screenshot file name
/////////////////////////////////////////////////////

// UDP servers and clients
pg_IPServer            **IP_Servers;
int                      nb_IP_Servers;
pg_IPClient            **IP_Clients;
int                      nb_IP_Clients;

////////////////////////////////////////////////////////////////////
// AUDIO SAMPLE DATA
// short samples typically for sequences, loaded at ones
int NbShortTracks = 0;
AudioSampleDataStruct **ShortTrackBufferData;
// long smaples, typically for soundtrack, streamed progressively
int NbLongTracks = 0;
AudioSampleDataStruct **LongTrackBufferData;
AudioLongTrackStreaming **LongTrackStreamings;

int NbAudioSequences = 0;
bool atLeastOneTerrainsVaguesSequence = false;
AudioSequenceStreaming **AudioSequences;


/////////////////////////////////////////////////////
// error reporting
/////////////////////////////////////////////////////

void ReportError(char *errorString) {
	fprintf(stderr, "%s\n", errorString);
}

/////////////////////////////////////////////////////
// environment parsing from configuration file
/////////////////////////////////////////////////////

void parseConfigurationFile(std::ifstream& fin) {
	////////////////////////////
	////// CONFIGURATION VARIABLE INITIALIZATION
	string line;
	string ID;

	// RANK
	std::getline(fin, line);
	// VERBATIM
	std::getline(fin, line);
	// TYPE
	std::getline(fin, line);
	// ID
	std::getline(fin, line);
	// initial_values
	std::getline(fin, line);

	// initial_values
	std::getline(fin, line);
	std::stringstream  ss(line);

	// std::cout << "VALUES: \n";
	for (int indP = 0; indP < _MaxConfigurationVarIDs; indP++) {
		ss >> InitialValuesConfigurationVar[indP];
		// std::cout << InitialValuesConfigurationVar[indP];
		// std::cout << " ";
	}
	// std::cout << "\n";

	// /initial_values
	std::getline(fin, line);

	////////////////////////////
	////// UDP hosts
	IP_Servers = NULL;
	nb_IP_Servers = 0;

	IP_Clients = NULL;
	nb_IP_Clients = 0;

	// udp_local_server Number of servers
	std::getline(fin, line);
	ss.clear();
	ss.str(line);
	// std::cout << line << "\n";
	ss >> ID;
	ss >> nb_IP_Servers;
	// std::cout << "Nb servers: " << nb_IP_Servers << "\n";

	// VERBATIM
	std::getline(fin, line);
	// TYPE
	std::getline(fin, line);
	// ID
	std::getline(fin, line);

	IP_Servers = new pg_IPServer *[nb_IP_Servers];
	for (int ind_IP_Server = 0; ind_IP_Server < nb_IP_Servers; ind_IP_Server++) {
		IP_Servers[ind_IP_Server] = new pg_IPServer();

		std::getline(fin, line);
		ss.clear();
		ss.str(line);
		// ss = std::stringstream(line);
		ss >> ID; // string "server"
		ss >> IP_Servers[ind_IP_Server]->id;
		ss >> IP_Servers[ind_IP_Server]->Local_server_port;
		ss >> ID;
		for (int ind = 0; ind < Emptypg_UDPMessageFormat; ind++) {
			if (strcmp(ID.c_str(), pg_UDPMessageFormatString[ind]) == 0) {
				IP_Servers[ind_IP_Server]->receive_format = (pg_UDPMessageFormat)ind;
				break;
			}
		}
		if (IP_Servers[ind_IP_Server]->receive_format == Emptypg_UDPMessageFormat) {
			sprintf(ErrorStr, "Error: unknown receive message format [%s]!", ID.c_str()); ReportError(ErrorStr); throw 249;
		}
		ss >> IP_Servers[ind_IP_Server]->IP_message_trace;
		ss >> IP_Servers[ind_IP_Server]->depth_input_stack;
		ss >> IP_Servers[ind_IP_Server]->OSC_duplicate_removal;
		ss >> IP_Servers[ind_IP_Server]->OSC_endian_reversal;
	}

	// /udp_local_server
	std::getline(fin, line);
	// udp_remote_client Number of clients
	std::getline(fin, line);
	ss.clear();
	ss.str(line);
	// ss = std::stringstream(line);
	ss >> ID;
	ss >> nb_IP_Clients;
	// std::cout << "Nb clients: " << nb_IP_Clients << "\n";

	// VERBATIM
	std::getline(fin, line);
	// TYPE
	std::getline(fin, line);
	// ID
	std::getline(fin, line);

	IP_Clients = new pg_IPClient *[nb_IP_Clients];
	for (int ind_IP_Client = 0; ind_IP_Client < nb_IP_Clients; ind_IP_Client++) {
		IP_Clients[ind_IP_Client] = new pg_IPClient();

		std::getline(fin, line);
		ss.clear();
		ss.str(line);
		// ss = std::stringstream(line);
		ss >> ID; // string "Client"
		ss >> IP_Clients[ind_IP_Client]->id;
		ss >> IP_Clients[ind_IP_Client]->Remote_server_IP;
		ss >> IP_Clients[ind_IP_Client]->Remote_server_port;
		ss >> ID;
		for (int ind = 0; ind < Emptypg_UDPMessageFormat; ind++) {
			if (strcmp(ID.c_str(), pg_UDPMessageFormatString[ind]) == 0) {
				IP_Clients[ind_IP_Client]->send_format = (pg_UDPMessageFormat)ind;
				break;
			}
		}
		if (IP_Clients[ind_IP_Client]->send_format == Emptypg_UDPMessageFormat) {
			sprintf(ErrorStr, "Error: unknown receive message format [%s]!", ID.c_str()); ReportError(ErrorStr); throw 249;
		}
		ss >> IP_Clients[ind_IP_Client]->IP_message_trace;
		ss >> IP_Clients[ind_IP_Client]->depth_output_stack;
		ss >> IP_Clients[ind_IP_Client]->OSC_endian_reversal;
		//std::cout << "OSC_endian_reversal: " << IP_Clients[ind_IP_Client]->OSC_endian_reversal << "\n";
		//std::cout << "IP_Clients[ ind_IP_Client ]->id: " << IP_Clients[ind_IP_Client]->id << "\n";
		//std::cout << "IP_Clients[ ind_IP_Client ]->Remote_server_IP: " << ind_IP_Client << " " << IP_Clients[ind_IP_Client]->Remote_server_IP << "\n";
		//std::cout << "IP_Clients[ ind_IP_Client ]->Remote_server_port: " << IP_Clients[ind_IP_Client]->Remote_server_port << "\n";
	}

	// /udp_remote_client
	std::getline(fin, line);

	// short_samples Number of short_samples
	std::getline(fin, line);
	ss.clear();
	ss.str(line);
	// ss = std::stringstream(line);
	ss >> ID;
	ss >> NbShortTracks;
	// std::cout << "NbShortTracks: " << NbShortTracks << "\n";

	ShortTrackBufferData = new AudioSampleDataStruct *[NbShortTracks];
	string trackName;

	// VERBATIM
	std::getline(fin, line);

	// reading sample index & file names
	for (int ind_short_samples = 0; ind_short_samples < NbShortTracks; ind_short_samples++) {
		ShortTrackBufferData[ind_short_samples] = new AudioSampleDataStruct();

		std::getline(fin, line);
		ss.clear();
		ss.str(line);
		// ss = std::stringstream(line);
		int indCurFrame;
		ss >> indCurFrame;
		ss >> trackName; // string "Client"
		if (indCurFrame != ind_short_samples) {
			sprintf(ErrorStr, "Error: Wrong index %d (instead of %d) for sample %s.\n", indCurFrame, ind_short_samples, trackName.c_str()); ReportError(ErrorStr); throw 249;
		}

		////////////////////////////////////////////////////////////////
		// LOADS BIG float
		ShortTrackBufferData[ind_short_samples]->name = trackName;
		// opens the samples
		int formatSize;
		if (!(formatSize = ShortTrackBufferData[ind_short_samples]->pg_openSample())) {
			sprintf(ErrorStr, "Error: File load failure %s.\n", trackName.c_str()); ReportError(ErrorStr); throw 249;
		}
		// and loads them
		printf("Loading sample %s", trackName.c_str());
		pg_readShortSampleBuffer((void *)ShortTrackBufferData[ind_short_samples]);
		printf(" dur %.2f\n", double(ShortTrackBufferData[ind_short_samples]->sampleBufferFrameSize)/PG_AUDIO_SAMPLE_RATE);
	}

	// /short_samples
	std::getline(fin, line);

	// long_samples Number of long_samples
	std::getline(fin, line);
	ss.clear();
	ss.str(line);
	// ss = std::stringstream(line);
	ss >> ID;
	ss >> NbLongTracks;
	// std::cout << "NbLongTracks: " << NbLongTracks << "\n";

	LongTrackBufferData = new AudioSampleDataStruct *[NbLongTracks];
	LongTrackStreamings = new AudioLongTrackStreaming *[NbLongTracks];

	// VERBATIM
	std::getline(fin, line);

	// reading sample index & file names
	for (int ind_long_tracks = 0; ind_long_tracks < NbLongTracks; ind_long_tracks++) {
		LongTrackBufferData[ind_long_tracks] = new AudioSampleDataStruct();
		LongTrackStreamings[ind_long_tracks] = new AudioLongTrackStreaming();

		std::getline(fin, line);
		ss.clear();
		ss.str(line);
		// ss = std::stringstream(line);
		int indCurFrame;
		ss >> indCurFrame;
		ss >> trackName; // string "Client"
		if (indCurFrame != ind_long_tracks) {
			sprintf(ErrorStr, "Error: Wrong index %d (instead of %d) for sample %s.\n", indCurFrame, ind_long_tracks, trackName.c_str()); ReportError(ErrorStr); throw 249;
		}

		////////////////////////////////////////////////////////////////
		// LOADS BIG float
		LongTrackBufferData[ind_long_tracks]->name = trackName;
		// opens the samples
		int formatSize;
		if (!(formatSize = LongTrackBufferData[ind_long_tracks]->pg_openSample())) {
			sprintf(ErrorStr, "Error: File load failure %s.\n", trackName.c_str()); ReportError(ErrorStr); throw 249;
		}
		// and loads them
		printf("Loading sample %s", trackName.c_str());
		pg_readShortSampleBuffer((void *)LongTrackBufferData[ind_long_tracks]);
		printf(" dur %.2f\n", double(LongTrackBufferData[ind_long_tracks]->sampleBufferFrameSize) / PG_AUDIO_SAMPLE_RATE);
	}

	// /long_samples
	std::getline(fin, line);

	// sequences Number of sequences
	std::getline(fin, line);
	ss.clear();
	ss.str(line);
	// ss = std::stringstream(line);
	ss >> ID;
	ss >> NbAudioSequences;
	// std::cout << "NbAudioSequences: " << NbAudioSequences << "\n";

	AudioSequences = new AudioSequenceStreaming *[NbAudioSequences];
	// VERBATIM
	std::getline(fin, line);

    // reading sequences: index	ID	buffer #	size	dur	amp loops
	std::cout << "\n";
#ifdef PG_TERRAINS_VAGUES
	atLeastOneTerrainsVaguesSequence = false;
#endif
	for (int ind_sequences = 0; ind_sequences < NbAudioSequences; ind_sequences++) {
		std::getline(fin, line);
		ss.clear();
		ss.str(line);
		// ss = std::stringstream(line);
		int indSequence;
		ss >> indSequence;
		if (indSequence != ind_sequences) {
			sprintf(ErrorStr, "Error: Wrong index %d (instead of %d) for sequence %s.\n", indSequence, ind_sequences, AudioSequences[ind_sequences]->name.c_str()); ReportError(ErrorStr); throw 249;
		}
		int seqSize;
		char c;
		ss >> seqSize;
		AudioSequences[ind_sequences] = new AudioSequenceStreaming(seqSize);
		ss >> AudioSequences[ind_sequences]->name; // sequence ID
		for (int indBeat = 0; indBeat < seqSize; indBeat++) {
			ss >> AudioSequences[ind_sequences]->bufNum[indBeat]; // buffer ID
			if (indBeat < seqSize - 1) {
				ss >> c; // , eater
			}
#ifdef PG_TERRAINS_VAGUES
			if (AudioSequences[ind_sequences]->name.find("terrains_vagues") == std::string::npos) {
				if (AudioSequences[ind_sequences]->bufNum[indBeat] >= NbShortTracks) {
					sprintf(ErrorStr, "Error: Unknown buffer number %d (max number: %d) for sample %s.\n", AudioSequences[ind_sequences]->bufNum[indBeat], NbShortTracks - 1, AudioSequences[ind_sequences]->name.c_str()); ReportError(ErrorStr); throw 249;
				}
				AudioSequences[ind_sequences]->isSequenceTerrainVagues = false;
			}
			else {
				AudioSequences[ind_sequences]->isSequenceTerrainVagues = true;
				atLeastOneTerrainsVaguesSequence = true;
			}
#else
			if (AudioSequences[ind_sequences]->bufNum[indBeat] >= NbShortTracks) {
				sprintf(ErrorStr, "Error: Unknown buffer number %d (max number: %d) for sample %s.\n", AudioSequences[ind_sequences]->bufNum[indBeat], NbShortTracks - 1, AudioSequences[ind_sequences]->name.c_str()); ReportError(ErrorStr); throw 249;
			}
#endif
		}
		std::cout << "sequence: " << std::setfill(' ') << std::setw(2) << indSequence << " ID: " << AudioSequences[ind_sequences]->name << " size:" << seqSize << " bufNum: ";
		for (int indBeat = 0; indBeat < seqSize; indBeat++) {
			std::cout << AudioSequences[ind_sequences]->bufNum[indBeat] << " ";
		}
		std::cout << " dur: ";
		AudioSequences[ind_sequences]->totalDuration = 0.;
		for (int indBeat = 0; indBeat < seqSize; indBeat++) {
			ss >> AudioSequences[ind_sequences]->seqDur[indBeat]; // durations
			if (indBeat < seqSize - 1) {
				ss >> c; // , eater
			}
			std::cout << AudioSequences[ind_sequences]->seqDur[indBeat] << " ";
			// stores the cumulated durations of each piece of a sequence
			AudioSequences[ind_sequences]->cumulDur[indBeat] = AudioSequences[ind_sequences]->totalDuration;
			AudioSequences[ind_sequences]->totalDuration += AudioSequences[ind_sequences]->seqDur[indBeat];
		}
		// the last cumulated duration is the sequence total duration
		AudioSequences[ind_sequences]->cumulDur[seqSize] = AudioSequences[ind_sequences]->totalDuration;
		std::cout << " amp: ";
		for (int indBeat = 0; indBeat < seqSize; indBeat++) {
			ss >> AudioSequences[ind_sequences]->amp[indBeat]; // amps
			if (indBeat < seqSize - 1) {
				ss >> c; // , eater
			}
			std::cout << AudioSequences[ind_sequences]->amp[indBeat] << " ";
		}
		std::cout << " loops: ";
		ss >> AudioSequences[ind_sequences]->nbLoops; // loops
		std::cout << AudioSequences[ind_sequences]->nbLoops << " ";
		std::cout << " sync: ";
		ss >> AudioSequences[ind_sequences]->synchronized; // loops
		std::cout << AudioSequences[ind_sequences]->synchronized << " ";
		std::cout << "\n";
		// non synchronized sequences must be size 1 sequences (1 single sample)
		if (!AudioSequences[ind_sequences]->synchronized && AudioSequences[ind_sequences]->size > 1) {
			sprintf(ErrorStr, "Error: non synchronized sequences must be size 1 sequences (1 single sample) not %d for sequence #%d %s.\n", AudioSequences[ind_sequences]->size , ind_sequences, AudioSequences[ind_sequences]->name.c_str()); ReportError(ErrorStr); throw 249;
		}
		// the length of a non synchronized sequence is the duration of its single track
		if (!AudioSequences[ind_sequences]->synchronized) {
			AudioSequences[ind_sequences]->singleFrameDuration 
				= FramesToDuration(double(ShortTrackBufferData[AudioSequences[ind_sequences]->bufNum[0]]->sampleBufferFrameSize));
		}
	}
    std::cout << "\n";
	// initializes the sequence level memory for the blast in terrains vagues
	seqLevels = new float[NbAudioSequences];

	// /sequences
	std::getline(fin, line);

	// /head
	std::getline(fin, line);
}


void InitializeConfigurationVar(void) {
	for (int indVar = 0; indVar < _MaxConfigurationVarIDs; indVar++) {
		if (ConfigurationVarTypes[indVar] == _pg_float) {
			*((float *)ConfigurationVarPointers[indVar]) = InitialValuesConfigurationVar[indVar];
			printf("float variable %d value %.3f\n", indVar, *((float *)ConfigurationVarPointers[indVar]));
		}
		else if (ConfigurationVarTypes[indVar] == _pg_int) {
			*((int *)ConfigurationVarPointers[indVar]) = (int)round(InitialValuesConfigurationVar[indVar]);
			printf("int variable %d value %d\n", indVar, *((int *)ConfigurationVarPointers[indVar]));
		}
		else if (ConfigurationVarTypes[indVar] == _pg_bool) {
			*((bool *)ConfigurationVarPointers[indVar]) = (InitialValuesConfigurationVar[indVar] > 0 ? true : false);
		}
		else if (ConfigurationVarTypes[indVar] == _pg_sign) {
			*((float *)ConfigurationVarPointers[indVar]) = (InitialValuesConfigurationVar[indVar] > 0 ? 1.0f : -1.0f);
		}
		else if (ConfigurationVarTypes[indVar] == _pg_track) {
			*((bool *)ConfigurationVarPointers[indVar]) = (InitialValuesConfigurationVar[indVar] > 0 ? true : false);
		}
		else {
			sprintf(ErrorStr, "Incorrect variable type (%d) for configuration variable #%d!", ConfigurationVarTypes[indVar], indVar); ReportError(ErrorStr);
		}
	}
}

void LoadConfigurationFile(string confFileName) {
  printf( "Loading %s\n\n" , confFileName.c_str() );

  std::ifstream confFin(confFileName);
  if( !confFin ) {
    sprintf( ErrorStr , "Error: file [%s] not found!" , confFileName.c_str() ); ReportError( ErrorStr ); throw 11;
  }

  parseConfigurationFile( confFin );

  confFin.close();

  InitializeConfigurationVar();

  //std::cout << "\n*** max_network_message_length/max_mouse_recording_frames/minimal_interframe_latency: " 
  //  << max_network_message_length <<  << " " << max_mouse_recording_frames << " " << minimal_interframe_latency "\n";
  //std::cout << "\n*** window size: " << window_width << " " << window_height << " " << window_x << " " << window_y << "\n";

}

