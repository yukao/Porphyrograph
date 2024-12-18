/*
 * $Id: pa_fuzz.c,v 1.1.1.1.4.1 2003/02/11 21:41:32 philburk Exp $
 * pa_fuzz.c
 * Distort input like a fuzz boz.
 *
 * Author: Phil Burk  http://www.softsynth.com
 *
 * This program uses the PortAudio Portable Audio Library.
 * For more information see: http://www.portaudio.com
 * Copyright (c) 1999-2000 Ross Bencina and Phil Burk
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * Any person wishing to distribute modifications to the Software is
 * requested to send the modifications to the original developer so that
 * they can be incorporated into the canonical version.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */
#include "pg-all_include.h"
/*
** Note that many of the older ISA sound cards on PCs do NOT support
** full duplex audio (simultaneous record and playback).
** And some only support full duplex at lower sample rates.
*/

////////////////////////////////////////////////////////////////////
// AUDIO BUFFERS
//////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////
// WORKING BUFFER FOR I/O
unsigned long pg_VSTeffect_BufferSize = FRAMES_PER_BUFFER;
float *pg_VSTeffect_InputBuffer[PG_AUDIO_NB_CHANNELS] = { NULL, NULL };
float *pg_VSTeffect_OutputBuffer[PG_AUDIO_NB_CHANNELS] = { NULL, NULL };

double *pg_FFT_InputBuffer[PG_AUDIO_NB_CHANNELS] = { NULL, NULL };
double *pg_FFT_OutputBuffer[PG_AUDIO_NB_CHANNELS] = { NULL, NULL };

unsigned long pg_InLineAndSamples_BufferSize = FRAMES_PER_BUFFER;
float *pg_InLine_InputBuffer = NULL;
float *pg_Samples_InputBuffer = NULL;
float *pg_RhythmsBackTrack_InputBuffer = NULL;
float *pg_InLine_OutputBuffer = NULL;
float *pg_Samples_OutputBuffer = NULL;

RecordingDataStruct *pg_recordingData = NULL;
int pg_active_recordingbuffer = 0;
int pg_frame_recordingbuffer = 0;

// parameters given by the streaming initialization
PaStreamParameters pg_audio_outputParameters;
PaStream *pg_audio_stream;

// std::cout << "VALUES: \n";
// ss >> AudioInputDevice; // -1 means no input device provided, should be found as default device
// ss >> AudioOutputDevice; // -1 means no output device provided, should be found as default device
// ss >> VerboseDeviceOutput; // -1 means no output device provided, should be found as default device
// std::cout << "\n";

////////////////////////////////////////////////////////////////////
// LAUNCHED PERFORMANCE
////////////////////////////////////////////////////////////////////
bool  pg_Launched_Performance = false;

////////////////////////////////////////////////////////////////////
// INPUT LEVEL
////////////////////////////////////////////////////////////////////
#ifdef CRITON
float input_level = 0.0f;
#else
float input_level = 1.0f;
#endif

////////////////////////////////////////////////////////////////////
// LOOP BUFFER
////////////////////////////////////////////////////////////////////
AudioLoopBuffer *LoopBuffer[PG_NB_LOOP_BUFFERS];
float loop_master_level = 1.f;

////////////////////////////////////////////////////////////////////
/// THREADED OUTPUT WRITING
////////////////////////////////////////////////////////////////////
#ifdef WIN32
DWORD WINAPI pg_threadedWriteRecordingOutput(LPVOID lpParam) {
#else
void* pg_threadedWriteRecordingOutput(void * lpParam) {
#endif
	// printf("Write full buffer %d\n", (pg_active_recordingbuffer + 1) % 2);
	RecordingDataStruct * recDataArray = (RecordingDataStruct *)lpParam;
	sf_command(recDataArray->pg_recordingfile, SFC_GET_NORM_FLOAT, NULL, SF_TRUE);
	sf_writef_float(recDataArray->pg_recordingfile,
		recDataArray->pg_doubleRecordingbuffer[(pg_active_recordingbuffer + 1) % 2],
		PG_RECORDINGBUFFER_SIZE );
	return true;
}

////////////////////////////////////////////////////////////////////
// FFT PERSONAL FUNCTIONS
////////////////////////////////////////////////////////////////////
#ifndef JUCE_APP_VERSION
typedef std::vector<std::vector<char>> PlotMatrixType;
template <typename Iterator>
void dotPlot(PlotMatrixType& plotData, Iterator begin, Iterator end)
{
	const double max = *std::max_element(begin, end);
	const double min = *std::min_element(begin, end);
	const double range = max - min;
	size_t m_height = 16;

	for (std::size_t xPos = 0; xPos < plotData.size(); ++xPos)
	{
		plotData[xPos].resize(m_height, ' ');
		double normalizedValue = (*begin++ - min) / range;
		std::size_t yPos = m_height - static_cast<std::size_t>(
			std::ceil(m_height * normalizedValue));

		// bound the value so it stays within vector dimension
		if (yPos >= m_height)
			yPos = m_height - 1;
		plotData[xPos][yPos] = '*';
	}

	const std::size_t length = plotData.size();

	std::cout << "\n" << "Title" << "\n";
	// output the plot data, flushing only at the end
	for (unsigned int y = 0; y < m_height; ++y)
	{
		for (unsigned int x = 0; x < length; ++x)
		{
			std::cout << plotData[x][y];
		}
		std::cout << "\n";
	}
	std::cout.flush();
}
void spectrumValuesByRows(Aquila::SpectrumType spectrum)
{
	std::size_t halfLength = spectrum.size() / 2;
	if (traceFFT) {
		printf("FFT: ");
		// std::vector<double> absSpectrum(halfLength);
		for (std::size_t i = 0; i < halfLength; ++i)
		{
			// absSpectrum[i] = std::abs(spectrum[i]);
			// amplitude
			printf("%.2f/", abs(spectrum[i]));
			// phase 
			printf("%.2f ", arg(spectrum[i]) / M_PI);
		}
		printf("\n");
		// PlotMatrixType plotData(absSpectrum.size());
		// dotPlot(plotData, absSpectrum.begin(), absSpectrum.end());
	}
}
#endif

////////////////////////////////////////////////////////////////////
// AUDIO STREAM VST PLUGIN I/O PROCESSING
////////////////////////////////////////////////////////////////////

bool pg_VSTeffect_IO_processor(float *VSTinput, float *VSToutput, unsigned long framesPerBuffer,
	MVerb<float> *VST_reverb, mdaDelay *VST_delay , int indProcess ) {
	bool valret = true;
	// printf("Frames per Output buffer %ld\n",framesPerBuffer);

	/////////////////////////////////////////////////////////////////
	// DEALLOCATION / REALLOCATION IN CASE OF TOO SMALL OUTPUT BUFFER
	if (pg_VSTeffect_BufferSize < framesPerBuffer) {
		//////////////////////////////////////
		// memory deallocation
		for (int indCh = 0; indCh < PG_AUDIO_NB_CHANNELS; indCh++) {
			delete pg_VSTeffect_InputBuffer[indCh];
			delete pg_VSTeffect_OutputBuffer[indCh];

			delete pg_FFT_InputBuffer[indCh];
			delete pg_FFT_OutputBuffer[indCh];
		}

		// increases the size until reaching a sufficient one
		while (pg_VSTeffect_BufferSize < framesPerBuffer) {
			pg_VSTeffect_BufferSize += FRAMES_PER_BUFFER;
		}

		//////////////////////////////////////////////////////
		// allocates the input and output buffers (stereo)
		for (int indCh = 0; indCh < PG_AUDIO_NB_CHANNELS; indCh++) {
			pg_VSTeffect_InputBuffer[indCh] = new float[pg_VSTeffect_BufferSize];
			pg_VSTeffect_OutputBuffer[indCh] = new float[pg_VSTeffect_BufferSize];

			pg_FFT_InputBuffer[indCh] = new double[pg_VSTeffect_BufferSize];
			pg_FFT_OutputBuffer[indCh] = new double[pg_VSTeffect_BufferSize];
		}
	}

	/////////////////////////////////////////////////////
	// buffer copy / VST process / copy back

	////////////////////////////////////////////////////
	// BUFFFER COPY
	// the input buffers (from portaudio or from a wav file) are
	// linearized in a single buffer: L sample R sample L sample R sample...
	// they have to be separated into independent buffers because VST
	// plugins always process channels in // buffers
	// without filtering phase
	if (true) {
		for (long indCurFrame = 0; indCurFrame < long(framesPerBuffer); indCurFrame++) {
			for (int indCh = 0; indCh < PG_AUDIO_NB_CHANNELS; indCh++) {
				pg_VSTeffect_InputBuffer[indCh][indCurFrame]
					= VSTinput[indCurFrame * PG_AUDIO_NB_CHANNELS + indCh];
			}
		}
	}
	// FFT analysis and plotting
	// COPIES THE RIGHT AND LEFT SAMPLES IN TWO SEPARATE BUFFERS
	for (long indCurFrame = 0; indCurFrame < long(framesPerBuffer); indCurFrame++) {
		for (int indCh = 0; indCh < PG_AUDIO_NB_CHANNELS; indCh++) {
			pg_FFT_InputBuffer[indCh][indCurFrame]
				= double(VSTinput[indCurFrame * PG_AUDIO_NB_CHANNELS + indCh]);
		}
	}

#ifndef JUCE_APP_VERSION
	// aquila FFT
	std::shared_ptr<Aquila::Fft> fft = Aquila::FftFactory::getFft(PG_FFT_SIZE);
	std::shared_ptr<Aquila::Fft> lastFft = Aquila::FftFactory::getFft(framesPerBuffer % PG_FFT_SIZE);
	Aquila::TextPlot plt("Input signal");
	for (int indCh = 0; indCh < PG_AUDIO_NB_CHANNELS; indCh++) {
		for (int ind = 0; ind < int(floor(PG_FFT_SIZE / framesPerBuffer)); ind++) {
			Aquila::SpectrumType spectrum = fft->fft(pg_FFT_InputBuffer[indCh] + ind * PG_FFT_SIZE);
			plt.setTitle("Spectrum");
			spectrumValuesByRows(spectrum);
		}
		int lastInd = int(floor(PG_FFT_SIZE / framesPerBuffer));
		Aquila::SpectrumType lastSpectrum = lastFft->fft(pg_FFT_InputBuffer[indCh] + lastInd * PG_FFT_SIZE);
		plt.setTitle("Spectrum");
		spectrumValuesByRows(lastSpectrum);
	}

	// FILTERING BASED ON FFT ANALYSIS --- to be reconsidered when it is clear that FFT analysis works
	/*
	else {
		for (long indCurFrame = 0; indCurFrame < long(framesPerBuffer); indCurFrame++) {
			for (int indCh = 0; indCh < PG_AUDIO_NB_CHANNELS; indCh++) {
				pg_FFT_InputBuffer[indCh][indCurFrame]
					= double(VSTinput[indCurFrame * PG_AUDIO_NB_CHANNELS + indCh]);
			}
		}

		////////////////////////////////////////////////////
		// FILTERING
		// aquila library

		// generate a low-pass filter spectrum
		Aquila::SpectrumType filterSpectrum(PG_FFT_SIZE);
		for (std::size_t i = 0; i < PG_FFT_SIZE; ++i)
		{
			if (i < int(0.2 * PG_FFT_SIZE))
			{
				// passband
				filterSpectrum[i] = 1.0;
			}
			else if (i < int(0.3 * PG_FFT_SIZE))
			{
				// passband
				filterSpectrum[i] = 1.0 - 10.0 * (i - 0.2 * PG_FFT_SIZE) / PG_FFT_SIZE;
			}
			else
			{
				// stopband
				filterSpectrum[i] = 0.0;
			}
			filterSpectrum[i] = 1.0;
		}
		// generate a low-pass filter spectrum
		Aquila::SpectrumType lastFilterSpectrum(framesPerBuffer % PG_FFT_SIZE);
		for (std::size_t i = 0; i < framesPerBuffer % PG_FFT_SIZE; ++i)
		{
			if (i < unsigned int(0.2 * (framesPerBuffer % PG_FFT_SIZE)))
			{
				// passband
				lastFilterSpectrum[i] = 1.0;
			}
			else if (i < unsigned int(0.3 * (framesPerBuffer % PG_FFT_SIZE)))
			{
				// passband
				lastFilterSpectrum[i] = 1.0 - 10.0 * (i - 0.2 * (framesPerBuffer % PG_FFT_SIZE)) / (framesPerBuffer % PG_FFT_SIZE);
			}
			else
			{
				// stopband
				lastFilterSpectrum[i] = 0.0;
			}
			lastFilterSpectrum[i] = 1.0;
		}
		std::shared_ptr<Aquila::Fft> fft = Aquila::FftFactory::getFft(PG_FFT_SIZE);
		std::shared_ptr<Aquila::Fft> lastFft = Aquila::FftFactory::getFft(framesPerBuffer % PG_FFT_SIZE);
		// Aquila::SampleType samples[512];
		for (int indCh = 0; indCh < PG_AUDIO_NB_CHANNELS; indCh++) {
			// the following call does the multiplication of two spectra
			// (which is complementary to convolution in time domain)
			for (int ind = 0; ind < int(floor(PG_FFT_SIZE / framesPerBuffer)); ind++) {
				Aquila::SpectrumType spectrum = fft->fft(pg_FFT_InputBuffer[indCh] + ind * PG_FFT_SIZE);
				std::transform(
					std::begin(spectrum),
					std::end(spectrum),
					std::begin(filterSpectrum),
					std::begin(spectrum),
					[](Aquila::ComplexType x, Aquila::ComplexType y) { return x * y; }
				);
				fft->ifft(spectrum, pg_FFT_InputBuffer[indCh] + ind * PG_FFT_SIZE);
			}
			int lastInd = int(floor(PG_FFT_SIZE / framesPerBuffer));
			Aquila::SpectrumType lastSpectrum = lastFft->fft(pg_FFT_InputBuffer[indCh] + lastInd * PG_FFT_SIZE);
			std::transform(
				std::begin(lastSpectrum),
				std::end(lastSpectrum),
				std::begin(lastFilterSpectrum),
				std::begin(lastSpectrum),
				[](Aquila::ComplexType x, Aquila::ComplexType y) { return x * y; }
			);
			fft->ifft(lastSpectrum, pg_FFT_InputBuffer[indCh] + lastInd * PG_FFT_SIZE);
		}
	
		// copy back
		for (long indCurFrame = 0; indCurFrame < long(framesPerBuffer); indCurFrame++) {
			for (int indCh = 0; indCh < PG_AUDIO_NB_CHANNELS; indCh++) {
				pg_VSTeffect_InputBuffer[indCh][indCurFrame]
					= float(pg_FFT_InputBuffer[indCh][indCurFrame]);
			}
		}
	}
	*/
#endif

	/////////////////////////////////////////////////////
	// VST EFFECTS processing of the samples (by directly applying 
	// the plugin processing function processReplacing)

	// (1) REVERB
	if (VST_reverb->getParameter(MVerb<float>::MIX) > 0.f) {
		// printf("reverb %d\n" , indProcess );
		VST_reverb->processReplacing(pg_VSTeffect_InputBuffer, pg_VSTeffect_OutputBuffer, int(framesPerBuffer));
	}
	else {
		for (int indCh = 0; indCh < PG_AUDIO_NB_CHANNELS; indCh++) {
			std::memcpy(pg_VSTeffect_OutputBuffer[indCh],
				pg_VSTeffect_InputBuffer[indCh],
				int(framesPerBuffer) * sizeof(float));
		}
	}

	// (2) DELAY
	if (VST_delay->getParameter(kMix) > 0.f) {
		// printf("delay %d\n", indProcess);
		VST_delay->processReplacing(pg_VSTeffect_OutputBuffer, pg_VSTeffect_InputBuffer, int(framesPerBuffer));
	}
	else {
		for (int indCh = 0; indCh < PG_AUDIO_NB_CHANNELS; indCh++) {
			std::memcpy(pg_VSTeffect_InputBuffer[indCh],
				pg_VSTeffect_OutputBuffer[indCh],
				int(framesPerBuffer) * sizeof(float));
		}
	}

	////////////////////////////////////////////////////
	// BUFFER COPY BACK
	// the processed buffers have to be relinearized into a single output buffer
	for (long indCurFrame = 0; indCurFrame < long(framesPerBuffer); indCurFrame++) {
		for (int indCh = 0; indCh < PG_AUDIO_NB_CHANNELS; indCh++) {
			VSToutput[indCurFrame * PG_AUDIO_NB_CHANNELS + indCh]
				= pg_VSTeffect_InputBuffer[indCh][indCurFrame];
		}
	}

	return valret;
}

////////////////////////////////////////////////////////////////////
// AUDIO STREAM VST PLUGIN I/O PROCESSING
////////////////////////////////////////////////////////////////////

// if it records: plays the existing loop and stores the input inside the loop
// if it plays: plays the existing loop 
bool pg_loop_processor(float *loopInputBuffer, float *loopOutputBuffer, unsigned long framesPerBuffer) {
	bool valret = true;
	// printf("Frames per Output buffer %ld\n",framesPerBuffer);
	// printf("Loop rec/play %d/%d %d/%d %d/%d \n", LoopBuffer[0]->loopRecording, LoopBuffer[0]->loopPlaying, 
	//LoopBuffer[1]->loopRecording, LoopBuffer[1]->loopPlaying, 
	//LoopBuffer[2]->loopRecording, LoopBuffer[2]->loopPlaying);

	////////////////////////////////////////////////////
	// the input buffers (from portaudio or from a wav file) are
	// linearized in a single buffer: L sample R sample L sample R sample...
	// they have to be separated into independent buffers because VST
	// plugins always process channels in // buffers
	for (long indCurFrame = 0; indCurFrame < long(framesPerBuffer); indCurFrame++) {
		// output is set to 0 0
		// and cumulates playing or recording loop buffers
		double currentStreamingRealTime 
		  = StreamingRealTime + FramesToDuration(indCurFrame);
		for (int indLoopTrack = 0; indLoopTrack < PG_NB_LOOP_BUFFERS; indLoopTrack++) {
			// if it records: plays the existing loop and stores the input inside the loop
			// if it plays: plays the existing loop 
			if (LoopBuffer[indLoopTrack]->loopRecording || LoopBuffer[indLoopTrack]->loopPlaying) {
				double loopStreamingRealTime
					= currentStreamingRealTime - LoopBuffer[indLoopTrack]->startRecordingTime;
				long indFrameinLoopBuffer 
					= (DurationToFrames(loopStreamingRealTime)) % LoopBuffer[indLoopTrack]->nbLoopFrames;
				for (int indCh = 0; indCh < PG_AUDIO_NB_CHANNELS; indCh++) {
					long indInLoop = indFrameinLoopBuffer * PG_AUDIO_NB_CHANNELS + indCh;
					long indInInput = indCurFrame * PG_AUDIO_NB_CHANNELS + indCh;
					// plays or records: copies the loop inside the output
					loopOutputBuffer[indInInput] 
						+= LoopBuffer[indLoopTrack]->loopOutLevel * loop_master_level 
						   * LoopBuffer[indLoopTrack]->loopSoundBuffer[indInLoop];
					// record: cumulates the input inside the loop
					if (LoopBuffer[indLoopTrack]->loopRecording) {
						// cumulates the input inside the loop
						LoopBuffer[indLoopTrack]->loopSoundBuffer[indInLoop] 
							+= loopInputBuffer[indInInput];
					}
				}
			}
		}
	}

	return valret;
}

////////////////////////////////////////////////////////////////////
// AUDIO STREAM SAMPLE PLAYBACK ON TOP OF I/O PROCESSING
////////////////////////////////////////////////////////////////////

bool pg_Sequences_and_Backtracks_Playback(float *out_RhythmsBackTrack, float *out_Samples, unsigned long framesPerBuffer) {
	bool valret = true;
	// printf("pg_Sequences_and_Backtracks_Playback\n");

	////////////////////////////////////////////////////////////////////////
	// NON SYNCHRONIZED SAMPLES
	for (int ind_sequences = 0; ind_sequences < NbAudioSequences; ind_sequences++) {
		AudioSequenceStreaming *seq = AudioSequences[ind_sequences];
		// printf("sequence %s started %d playing %d synced %d\n", seq->name.c_str(), seq->seqStarted, seq->seqPlaying, seq->synchronized);
		// plays a sequence that was set as Started, but not yet Playing
		if (seq->seqStarted && !seq->synchronized && !seq->seqPlaying) {
			seq->startStreamingTime = StreamingRealTime;
			seq->seqPlaying = true;
			seq->seqStarted = false;
			// printf("PLAY non synchronized sequence %s %.2f\n", seq->name.c_str(), seq->seqLevel);
		}
	}

	////////////////////////////////////////////////////
	// the processed buffers have to be relinearized into a single output buffer
	for (long indCurFrame = 0; indCurFrame < long(framesPerBuffer); indCurFrame++) {
		double currentStreamingRealTime = StreamingRealTime
										+ FramesToDuration(indCurFrame);

		////////////////////////////////////////////////////////////////////////
		// BEAT PROCESSING
		// next beat if it is time for that
#ifdef PG_TERRAINS_VAGUES
		unsigned long newNbStreamingBeats
			= long(floor((currentStreamingRealTime - InitialTerrainVaguesTime) / BeatDurationRealTime));
#else
		unsigned long newNbStreamingBeats
			= long(floor((currentStreamingRealTime - InitialPerformanceTime) / BeatDurationRealTime));
#endif
		//printf("sequence streaming curr time %f last beat %f duration %f nb Beats %d\n",
		//	currentStreamingRealTime, LastStreamingBeatRealTime, BeatDurationRealTime, newNbStreamingBeats);
		if (newNbStreamingBeats != NbStreamingBeats) {
			NbStreamingBeats = newNbStreamingBeats;
			LastStreamingBeatRealTime = currentStreamingRealTime;
			// printf("streaming beat %d\n", NbStreamingBeats%BeatsPerSequence);
			// sprintf(AuxString, "/beat %d", NbStreamingBeats % BeatsPerSequence); pg_send_message_udp((char *)"i", (char *)AuxString, (char *)"udp_Porphyrograph_send");

#ifdef PG_TERRAINS_VAGUES
			if (SoundMachineRhythms) {
				// updates sample buffers according to TV_swap_freq
			// for a swap duration of sample_swap_duration
				if (InitialTerrainVaguesTime != -1.0
					&& TV_swap_freq > 0
					&& TV_swap_freq <= PG_LOOP_SIZE
					&& (NbStreamingBeats % (PG_LOOP_SIZE / (int(TV_swap_freq) <= PG_LOOP_SIZE ? int(TV_swap_freq) : PG_LOOP_SIZE))) == 0) {
					// there should be an initial procedure that scans the dirs by increasing index size
					// nd stores for each of them the number of images

					// string * fileName;
					// loads each buffer in turn
					SoundMachineRhythms->last_loaded_buffer
						= (SoundMachineRhythms->last_loaded_buffer + 1) % PG_AUDIO_NB_BUFFERS;
					//SoundMachineDrones->last_loaded_buffer
					//	= SoundMachineRhythms->last_loaded_buffer;
					
					// CJ UNCOMMENTED FEB 2018
					std::string *fileName;
					if ((fileName = nextFile(&StringRhythmsBaseDir, &IndRhythmsSampleDir,
						&IndPlayedAudioRhythmsSample))) {
						// printf("next file %s\n", fileName->c_str());
						if (SoundMachineRhythms->load_and_swap_audio_buffer(fileName,
							TV_swap_dur, SoundMachineRhythms->last_loaded_buffer)) {
						}
					}
					

					IndPlayedAudioRhythmsSample = (IndPlayedAudioRhythmsSample + 1) % PG_AUDIO_NB_SAMPLES;
					if (SoundMachineRhythms->swap_audio_buffer(IndPlayedAudioRhythmsSample,
						TV_swap_dur, SoundMachineRhythms->last_loaded_buffer)) {
					}

					//if ((fileName = nextFile(&DronesSampleDir, &IndPlayedAudioDronesSampleDir, 
					//			 &IndPlayedAudioDronesSample))) {
					//	if (SoundMachineDrones->load_and_swap_audio_buffer(fileName, 
					//								 sample_swap_duration, SoundMachineDrones->last_loaded_buffer)) {
					//	}
					//}
					// new track, possible change of sample
					// and variable updates accordingly
					SoundMachineRhythms->BufferSelection(&CurrentlyPlayingBuffer);
				}
			}
#endif

			for (int ind_long_tracks = 0; ind_long_tracks < NbLongTracks; ind_long_tracks++) {
				// plays a long track that is started
				AudioLongTrackStreaming *track = LongTrackStreamings[ind_long_tracks];
				if (track->trackStarted && !track->trackPlaying) {
					track->startStreamingTime = LastStreamingBeatRealTime;
					track->trackPlaying = true;
					track->trackStarted = false;
					//printf("Play track %s %.2f\n", LongTrackBufferData[ind_long_tracks]->name.c_str(),
					//	track->trackLevel);
				}
			}

			// if the beat is a multiple of BeatsPerSequence
			if (NbStreamingBeats % BeatsPerSequence == 0) {
				for (int ind_sequences = 0; ind_sequences < NbAudioSequences; ind_sequences++) {
					AudioSequenceStreaming *seq = AudioSequences[ind_sequences];
					// plays a sequence that was set as Started, but not yet Playing
					if (seq->seqStarted && !seq->seqPlaying && seq->synchronized) {
						seq->startStreamingTime = LastStreamingBeatRealTime;
						seq->seqPlaying = true;
						seq->seqStarted = false;
						//printf("Play synchronized sequence %s %.2f beats %d start %.5f\n", 
						//	seq->name.c_str(), seq->seqLevel, NbStreamingBeats, LastStreamingBeatRealTime);
					}
				}
			}
		}

		////////////////////////////////////////////////////////////////////////
		// SAMPLE SEQUENCE PROCESSING
		// plays a sample inside a sequence 
		// sample pieces do not necessarily happens at an integer beat count
		for (int ind_sequences = 0; ind_sequences < NbAudioSequences; ind_sequences++) {
			AudioSequenceStreaming *seq = AudioSequences[ind_sequences];
			seq->currPiece = -1;

			// looks for the currently playing piece
			if (seq->seqPlaying) {
				// sequence is aligned on beats and duration is counted in beat units or fractions
				if (seq->synchronized) {
					// time since the starting of this sequence
					double seqTimeSinceBeginning = currentStreamingRealTime - seq->startStreamingTime;

					// time since the beginning of the current sequence
					double sequenceCumulDurTime = seq->totalDuration * BeatDurationRealTime;
					long nbSeqRepetitions = long(floor(seqTimeSinceBeginning / sequenceCumulDurTime));
					if (seq->nbLoops > 0 && nbSeqRepetitions >= seq->nbLoops) {
						seq->seqPlaying = false;
						seq->seqStarted = false;
						seq->currPiece = -1;
						seq->seqLevel = 0.f;
						// printf("end of synchronized sequence %s rep %ld\n", seq->name.c_str(), nbSeqRepetitions);
					}
					else {
						double currSequenceTime
							= seqTimeSinceBeginning - nbSeqRepetitions * sequenceCumulDurTime;

						// the time elapsed since the beginning of the sequence play is < to its length
						// looks for the currently playing piece
						seq->currPiece = -1;
						for (int indPiece = 0; indPiece < seq->size; indPiece++) {
							if (seq->cumulDur[indPiece + 1] * BeatDurationRealTime >= currSequenceTime) {
								if (seq->amp[indPiece] <= 0 || seq->seqLevel <= 0) {
									seq->currPiece = -1;
								}
								else {
									seq->currPiece = indPiece;
									seq->currPieceTime
										= currSequenceTime - seq->cumulDur[indPiece] * BeatDurationRealTime;
								}
								break;
							}
						}
					}
					//if( indCurFrame == 0 ){
					//		  printf("sequence %d beat %d playingPiece %d\n",ind_sequences,NbStreamingBeats%BeatsPerSequence,seq->currPiece);
					//}
				}
				// sequence is not synchronized and its length is counted in number of frames
				// of its single track
				else {
					// time since the starting of this sequence
					double seqTimeSinceBeginning = currentStreamingRealTime - seq->startStreamingTime;

					// length of the single track (non synchronized sequences have only one track,
					// possibly repeated several times)
					long nbSeqRepetitions = long(floor(seqTimeSinceBeginning / seq->singleFrameDuration));
					if (seq->nbLoops > 0 && nbSeqRepetitions >= seq->nbLoops) {
						// printf("stop sequence %s rep %ld\n", seq->name.c_str(), nbSeqRepetitions);
						seq->seqPlaying = false;
						seq->currPiece = -1;
						seq->seqLevel = 0.f;
						// printf("end of non synchronized sequence %s rep %ld\n", seq->name.c_str(), nbSeqRepetitions);
					}
					else {
						// the time elapsed since the beginning of the sequence play is < to its length
						// the currently playing piece is the first track since non synchronized sequences
						// typically have only one track
						seq->currPiece = 0;
						seq->currPieceTime
							= seqTimeSinceBeginning - nbSeqRepetitions * seq->singleFrameDuration;
						//if( indCurFrame == 0 ){
						 // printf("sequence #%d time %f\n", ind_sequences , seq->currPieceTime);
						//}
					}
				}
			}
			// not playing
			else {
				seq->currPiece = -1;
			}
		}
		// loop over all the sequences to find the possible playing piece

		////////////////////////////////////////////////////////////////////////
		// SOUND SYNTHESIS OF SHORT SAMPLES INSIDE A SEQUENCE
		// reads the active samples according to the time
		// they are read in loop and their looping time is always syncrhonized on the beats
		// the sample sound is cumulated with possible live sound computed previously
		// Samples from terrain vague
		float outSample[PG_AUDIO_NB_CHANNELS];
		memset((void *)outSample, 0, PG_AUDIO_NB_CHANNELS*sizeof(float));
		// rhythmic sequences and backtrack
		float outRhythmBackTrack[PG_AUDIO_NB_CHANNELS];
		memset((void *)outRhythmBackTrack, 0, PG_AUDIO_NB_CHANNELS * sizeof(float));
		for (int ind_sequences = 0; ind_sequences < NbAudioSequences; ind_sequences++) {
			AudioSequenceStreaming *seq = AudioSequences[ind_sequences];
			// STABLE SHORT SAMPLES
			// playing track: the sound is added to the current output
			if (seq->seqPlaying
				&& seq->currPiece >= 0
				&& seq->currPiece < seq->size) {
#ifdef PG_TERRAINS_VAGUES
				if (!seq->isSequenceTerrainVagues) {
#endif
					// sequence is aligned on beats and duration is counted in beat units or fractions
					if (seq->synchronized) {
						double trackRealTime = seq->currPieceTime;
						long indFrameInTrack = DurationToFrames(trackRealTime);
						// the current frame is inside the track: reads the track
						// and adds the sound
						int indTrack = seq->bufNum[seq->currPiece];

						// size of the current track
						long bufferFrameSize
							= long(ShortTrackBufferData[indTrack]->sampleBufferFrameSize);

						// the number of frames reaches the end of the track
						bool silence = false;
						if (indFrameInTrack >= bufferFrameSize) {
							if (seq->nbLoops < 0 && seq->size == 1
								&& int(seq->totalDuration) % BeatsPerSequence == 0) {
								// printf("Loop %.2f\n", trackRealTime);
								// in case of loop over a single sample reads the beginning of the track to avoid clicks
								indFrameInTrack = indFrameInTrack % bufferFrameSize;
							}
							else {
								// otherwise silence
								silence = true;
							}
						}
						if (!silence) {
							for (int indCh = 0; indCh < PG_AUDIO_NB_CHANNELS; indCh++) {
								outRhythmBackTrack[indCh]
									+= seq->seqLevel * ShortTrackBufferData[indTrack]
									->largeSampleBuffer[indFrameInTrack*PG_AUDIO_NB_CHANNELS + indCh];
							}
						}
						//if (indCurFrame == 0) {
						//	printf("play seq %d level %f\n", indTrack, seq->Level);
						//}
					}
					// sequence is not synchronized and its length is counted in number of frames
					// of its single track
					else {
						// size of the current track
						int indTrack = seq->bufNum[0];
						long bufferFrameSize
							= long(ShortTrackBufferData[indTrack]->sampleBufferFrameSize);
						// frame index from current time inside the track
						long indFrameInTrack = DurationToFrames(seq->currPieceTime);

						// the current frame is inside the track: reads the track
						// and adds the sound
						if (indFrameInTrack < bufferFrameSize) {
							for (int indCh = 0; indCh < PG_AUDIO_NB_CHANNELS; indCh++) {
								outRhythmBackTrack[indCh]
									+= seq->seqLevel * ShortTrackBufferData[indTrack]
									->largeSampleBuffer[indFrameInTrack*PG_AUDIO_NB_CHANNELS + indCh];
							}
						}
						// printf("sequence #%d indFrameInTrack %ld buffer size %ld indFrame %ld currStreamRealTime %.2f\n", ind_sequences, indFrameInTrack, bufferFrameSize,
									//indCurFrame, currentStreamingRealTime);
					}
#ifdef PG_TERRAINS_VAGUES
				}
				// CHANGING SHORT SAMPLES OF TERRAINS VAGUES
				else if (SoundMachineRhythms) {
					// for (int indBuffer = 0; indBuffer < 6; indBuffer++) {
					// int indCurrBuffer = (int(CurrentlyPlayingBuffer) + indBuffer) % PG_AUDIO_NB_SAMPLES;
					int indCurrBuffer = int(CurrentlyPlayingBuffer);

					long bufferSize = SoundMachineRhythms->SampleBufferData[IndRhythmsSampleDir][indCurrBuffer].sampleBufferFrameSize;
					bool loaded = SoundMachineRhythms->SampleBufferData[IndRhythmsSampleDir][indCurrBuffer].loaded;

					if (loaded && bufferSize > 0) {

						// sample number associated with the selected track
						int indexCurrentSample = SoundMachineRhythms->audio_buffer_data[indCurrBuffer].pg_SampleIndex;
						// sample number associated with the selected swap track
						int indexCurrentSwapSample = SoundMachineRhythms->audio_swap_buffer_data[indCurrBuffer].pg_SampleIndex;

						// buffer sizes
						long bufferFrameSize
							= long(SoundMachineRhythms->SampleBufferData[IndRhythmsSampleDir][indexCurrentSample].sampleSndInfo.frames);
						float * sampleBuffer = SoundMachineRhythms->SampleBufferData[IndRhythmsSampleDir][indexCurrentSample].largeSampleBuffer;

						long swapBufferFrameSize = 0;
						float * sampleSwaBuffer = NULL;
						if (indexCurrentSwapSample >= 0 && indexCurrentSwapSample < PG_AUDIO_NB_SAMPLES) {
							swapBufferFrameSize = long(SoundMachineRhythms->SampleBufferData[IndRhythmsSampleDir][indexCurrentSwapSample].sampleSndInfo.frames);
							sampleSwaBuffer = SoundMachineRhythms->SampleBufferData[IndRhythmsSampleDir][indexCurrentSwapSample].largeSampleBuffer;
						}

						// VARYING SAMPLES OF TERRAINS VAGUES (SAMPLE + SWAP)
						if (bufferFrameSize > 0 && seq->seqLevel > 0) {
							// level
							float currPieceLevel = seq->seqLevel * seq->amp[seq->currPiece];

							// track offsets
							long trackOffset = SoundMachineRhythms->audio_buffer_data[indCurrBuffer].pg_audio_trackOffset;
							long swapTrackOffset = SoundMachineRhythms->audio_swap_buffer_data[indCurrBuffer].pg_audio_trackOffset;

							// swapping coefficient
							float alphaSwap = 0.0f;
							if (sampleSwaBuffer && SoundMachineRhythms->SampleBufferData[IndRhythmsSampleDir][indexCurrentSwapSample].loaded) {
								alphaSwap = SoundMachineRhythms->audio_buffer_alphaSwap[indCurrBuffer];
							}
							// alphaSwap = 0.0f;

							double trackRealTime = seq->currPieceTime;
							long indFrameInTrack = DurationToFrames(trackRealTime);
							// adds an offset corresponding to the buffer number (which is not used here because
							// the buffers are corresponding to the current samples
							indFrameInTrack += (bufferFrameSize * (seq->bufNum[seq->currPiece] % PG_AUDIO_NB_DRONES)) / PG_AUDIO_NB_DRONES;

							// the current frame is outside the track: cyclic reading
							if (indFrameInTrack >= bufferFrameSize) {
								indFrameInTrack = indFrameInTrack % bufferFrameSize;
							}

							// the current frame is inside the track: reads the track
							// and adds the sound
							for (int indCh = 0; indCh < PG_AUDIO_NB_CHANNELS; indCh++) {
								float outSeqSample;
								if (alphaSwap > 0) {
									long indFrameInSwapTrack = indFrameInTrack % swapBufferFrameSize;
									outSeqSample
										= (1.f / 6) * ((1.f - alphaSwap) * sampleBuffer[indFrameInTrack*PG_AUDIO_NB_CHANNELS + indCh]
											+ alphaSwap * sampleSwaBuffer[indFrameInSwapTrack*PG_AUDIO_NB_CHANNELS + indCh]);
								}
								else {
									outSeqSample
										= (1.f / 6) * sampleBuffer[indFrameInTrack*PG_AUDIO_NB_CHANNELS + indCh];
								}
								outSample[indCh] += currPieceLevel * outSeqSample;
							}
							// printf("out sample %.2f %.2f\n", outSample[0], outSample[1]);
						}
					}
				}
#endif
			}
		}

		////////////////////////////////////////////////////////////////////////
		// SOUND SYNTHESIS OF LONG SAMPLES (BACKTRACKS)
		// streaming the long samples
		for (int ind_long_tracks = 0; ind_long_tracks < NbLongTracks; ind_long_tracks++) {
			if (LongTrackStreamings[ind_long_tracks]->trackPlaying) {
				double trackTimeSinceBeginning 
					= currentStreamingRealTime - LongTrackStreamings[ind_long_tracks]->startStreamingTime;
				long indFrameInTrack = DurationToFrames(trackTimeSinceBeginning);
				if (indFrameInTrack < LongTrackBufferData[ind_long_tracks]->sampleBufferFrameSize) {
					// !!!!!!!!!!!!! BUG HERE
					for (int indCh = 0; indCh < PG_AUDIO_NB_CHANNELS; indCh++) {
						// ADDITIVE SYNTHESIS (SHORT SAMPLES + BACKTRACK)
						outRhythmBackTrack[indCh]
							+= LongTrackStreamings[ind_long_tracks]->trackLevel
							* LongTrackBufferData[ind_long_tracks]
							->largeSampleBuffer[indFrameInTrack*PG_AUDIO_NB_CHANNELS + indCh];
					}
				}
				// reaches the end of the sample and stops playing
				else {
					LongTrackStreamings[ind_long_tracks]->trackPlaying = false;
					LongTrackStreamings[ind_long_tracks]->trackStarted = false;
				}
			}
		}

		////////////////////////////////////////////////////////////////////////
		// ADDITIVE SYNTHESIS (SHORT SAMPLES + BACKTRACK)
		// the sequence is added to the out buffer that might already contain live output
		for (int indCh = 0; indCh < PG_AUDIO_NB_CHANNELS; indCh++) {
			out_Samples[indCurFrame * PG_AUDIO_NB_CHANNELS + indCh] = outSample[indCh];
			out_RhythmsBackTrack[indCurFrame * PG_AUDIO_NB_CHANNELS + indCh] = outRhythmBackTrack[indCh];
		}
		//if (indCurFrame == 0) {
		//	printf("play sample %f\n", outSample[0]);
		//}
	}
	return valret;
}

////////////////////////////////////////////////////////////////////
// PORTAUDIO STREAMING CALL BACK 
////////////////////////////////////////////////////////////////////

/* This routine will be called by the PortAudio engine when audio is needed.
** It may be called at interrupt level on some machines so don't do anything
** that could mess up the system like calling malloc() or free().
*/
static int pg_audio_paStreamCallback( const void *inputBuffer, void *outputBuffer,
			     unsigned long framesPerBuffer,
			     const PaStreamCallbackTimeInfo* timeInfo,
			     PaStreamCallbackFlags statusFlags,
			     void *userData )
{
    float *floatOutputBuffer = (float*)outputBuffer;
    float *floatInputBuffer = (float*)inputBuffer;
    (void) userData;

	// printf("audio streaming callback ini %.2f\n", StreamingRealTime);

	/////////////////////////////////////////////////////////////////
	// DEALLOCATION / REALLOCATION IN CASE OF TOO SMALL OUTPUT BUFFER
	if (pg_InLineAndSamples_BufferSize < framesPerBuffer) {
		//////////////////////////////////////
		// memory deallocation
		delete pg_InLine_InputBuffer;
		delete pg_Samples_InputBuffer;
		delete pg_RhythmsBackTrack_InputBuffer;
		delete pg_InLine_OutputBuffer;
		delete pg_Samples_OutputBuffer;

		// increases the size until reaching a sufficient one
		while (pg_InLineAndSamples_BufferSize < framesPerBuffer) {
			pg_InLineAndSamples_BufferSize += FRAMES_PER_BUFFER;
		}

		//////////////////////////////////////////////////////
		// allocates the input and output buffers (stereo)
		pg_InLine_InputBuffer = new float[pg_InLineAndSamples_BufferSize * PG_AUDIO_NB_CHANNELS];
		pg_Samples_InputBuffer = new float[pg_InLineAndSamples_BufferSize * PG_AUDIO_NB_CHANNELS];
		pg_RhythmsBackTrack_InputBuffer = new float[pg_InLineAndSamples_BufferSize * PG_AUDIO_NB_CHANNELS];
		pg_InLine_OutputBuffer = new float[pg_InLineAndSamples_BufferSize * PG_AUDIO_NB_CHANNELS];
		pg_Samples_OutputBuffer = new float[pg_InLineAndSamples_BufferSize * PG_AUDIO_NB_CHANNELS];
	}

	// quesion: do we need to resynchronize the application with the time
	// shipped to the sound card, or do we just keep the time from the beginning
	// 1st case: LastPorphyrographRealTime = StreamingRealTime;
	// 2nd case do nothing and rely on initial synchronization

	// live input processing (loop)
    if( inputBuffer == NULL )
    {
		printf("NULL input buffer\n");
		// silent input
		memset((void *)pg_InLine_InputBuffer, 0, (framesPerBuffer * PG_AUDIO_NB_CHANNELS) * sizeof(float));
    }
	else {
		//input_level = 1.0f;
		/*
		for (unsigned long indCurFrame = 0; indCurFrame < framesPerBuffer; indCurFrame++) {
			for (int indCh = 0; indCh < PG_AUDIO_NB_CHANNELS; indCh++) {
				unsigned long ind = indCurFrame * PG_AUDIO_NB_CHANNELS + indCh;
				pg_InLine_InputBuffer[ind] = input_level * floatInputBuffer[ind];
			}
		}
		*/
		// input buffer cleaning
		memset((void *)pg_InLine_InputBuffer, 0, (framesPerBuffer * PG_AUDIO_NB_CHANNELS) * sizeof(float));

		// managing the loops if there are some active
		bool oneNonEmptyLoop = false;
		for (int indLoopTrack = 0; indLoopTrack < PG_NB_LOOP_BUFFERS; indLoopTrack++) {
			if (!LoopBuffer[indLoopTrack]->loopEmpty) {
				oneNonEmptyLoop = true;
				break;
			}
		}
		if(oneNonEmptyLoop) {
			// if it records: plays the existing loop and stores the input inside the loop
			// if it plays: plays the existing loop 
			pg_loop_processor(floatInputBuffer, pg_InLine_InputBuffer, framesPerBuffer);
		}

		//if (input_level > 0.0f) {
		// adds the current input to the possible loop output
		// and transforms left and right mono into L+R stereo
		for (unsigned long indCurFrame = 0; indCurFrame < framesPerBuffer; indCurFrame++) {
			unsigned long ind = indCurFrame * PG_AUDIO_NB_CHANNELS;
			for (int indCh = 0; indCh < PG_AUDIO_NB_CHANNELS; indCh++) {
				pg_InLine_InputBuffer[ind + indCh] += input_level * floatInputBuffer[ind + indCh];
			}
			float leftTmp = pg_InLine_InputBuffer[ind + 0];
			pg_InLine_InputBuffer[ind + 0] += pg_InLine_InputBuffer[ind + 1];
			pg_InLine_InputBuffer[ind + 1] += leftTmp;
		}
		//}
	}

	// sequence playback 
	pg_Sequences_and_Backtracks_Playback(pg_RhythmsBackTrack_InputBuffer, pg_Samples_InputBuffer, framesPerBuffer);

	//printf("effects on samples rev %f del %f and input rev %f del %f\n",
	//	VST_reverb_inLine.getParameter(MVerb<float>::MIX),
	//	VST_delay_inLine.getParameter(kMix),
	//	VST_reverb_samples.getParameter(MVerb<float>::MIX),
	//	VST_delay_samples.getParameter(kMix));
	//printf("loop level %f \n", LoopBuffer[ind]->loopOutLevel);

	// effects on input
	pg_VSTeffect_IO_processor(pg_InLine_InputBuffer, pg_InLine_OutputBuffer, framesPerBuffer,
		&VST_reverb_inLine, &VST_delay_inLine , 0 );

	// effects on sequence playback mixed with line in
#ifdef PG_TERRAINS_VAGUES
	pg_VSTeffect_IO_processor(pg_Samples_InputBuffer, pg_Samples_OutputBuffer, framesPerBuffer,
		&VST_reverb_samples, &VST_delay_samples, 1);
#endif

	////////////////////////////////////////////////////////////////////////
	// ADDITIVE SYNTHESIS (SHORT SAMPLES + BACKTRACK + LIVE INPUT)
	for (unsigned long indCurFrame = 0; indCurFrame < framesPerBuffer; indCurFrame++) {
		for (int indCh = 0; indCh < PG_AUDIO_NB_CHANNELS; indCh++) {
			unsigned long ind = indCurFrame * PG_AUDIO_NB_CHANNELS + indCh;
#ifdef PG_TERRAINS_VAGUES
			floatOutputBuffer[ind] = pg_InLine_OutputBuffer[ind]
				+ pg_Samples_OutputBuffer[ind] + pg_RhythmsBackTrack_InputBuffer[ind];
#else
			floatOutputBuffer[ind] = pg_InLine_OutputBuffer[ind]
				+ pg_Samples_InputBuffer[ind] + pg_RhythmsBackTrack_InputBuffer[ind];
#endif
		}
	}

	////////////////////////////////////////////////////////////////////////
	// WRITING THE NEW FRAMES IN THE FILE SAVING BUFFER
	// look at pg_audio_create_file();
	if (record_output && pg_recordingData) {
		for (unsigned long indCurFrame = 0; indCurFrame < framesPerBuffer; indCurFrame++) {
			for (int indCh = 0; indCh < PG_AUDIO_NB_CHANNELS; indCh++) {
				unsigned long ind = indCurFrame * PG_AUDIO_NB_CHANNELS + indCh;
				pg_recordingData->pg_doubleRecordingbuffer[pg_active_recordingbuffer][pg_frame_recordingbuffer * PG_AUDIO_NB_CHANNELS + indCh]
					= min(1.f,max(-1.f,floatOutputBuffer[ind]));
			}

			// if the end of one recording buffer is reached
			// it is writen on the disk through a thread while the
			// other recording buffer becomes the active one in
			// which data is written
			if (pg_frame_recordingbuffer >= PG_RECORDINGBUFFER_SIZE - 1) {
				// resets index for writing in buffer
				pg_frame_recordingbuffer = 0;
				// swaps buffers
				pg_active_recordingbuffer
					= (pg_active_recordingbuffer + 1) % 2;


#ifdef WIN32
				DWORD rc;
				HANDLE  hThread = CreateThread(
					NULL,         // default security attributes
					0,            // use default stack size
					pg_threadedWriteRecordingOutput,  // thread function name
					(void *)pg_recordingData,// argument to thread function
					0,            // use default creation flags
					&rc);   // returns the thread identifier
				if (hThread == NULL) {
					std::cout << "Error:unable to create thread pg_threadedWriteRecordingOutput for recording" << std::endl;
					sf_writef_float(pg_recordingData->pg_recordingfile,
						pg_recordingData->pg_doubleRecordingbuffer[pg_active_recordingbuffer],
						PG_RECORDINGBUFFER_SIZE);
					// exit(-1);
				}
				CloseHandle(hThread);
#else
				pthread_t readWavData_thread;
				int rc;
				rc = pthread_create(&pg_threadedWriteRecordingOutput, NULL,
					pg_threadedWriteRecordingOutput, (void *)pg_recordingData);
				if (rc) {
					std::cout << "Error:unable to create thread pg_threadedWriteRecordingOutput for recording" << rc << std::endl;
					sf_writef_float(pg_recordingData->pg_recordingfile,
						pg_recordingData->pg_doubleRecordingbuffer[pg_active_recordingbuffer],
						PG_RECORDINGBUFFER_SIZE);
					pthread_exit(NULL);
				}
#endif
			}
			else {
				pg_frame_recordingbuffer++;
			}
		}
	}

	// increases the streaming real time by the time sent out
	StreamingRealTime += FramesToDuration(framesPerBuffer);
	// printf("audio streaming callback end %.2f\n", StreamingRealTime);
	return 0;
}

/*
 * This routine is called by portaudio when playback is done.
 */
static void StreamFinished( void* userData ) {
  printf( "Stream Completed\n" );

  //////////////////////////////////////
  // memory deallocation
  for (int indCh = 0; indCh < PG_AUDIO_NB_CHANNELS; indCh++) {
	  delete pg_VSTeffect_InputBuffer[indCh];
	  delete pg_VSTeffect_OutputBuffer[indCh];

	  delete pg_FFT_InputBuffer[indCh];
	  delete pg_FFT_OutputBuffer[indCh];
  }
  delete pg_InLine_InputBuffer;
  delete pg_Samples_InputBuffer;
  delete pg_RhythmsBackTrack_InputBuffer;
  delete pg_InLine_OutputBuffer;
  delete pg_Samples_OutputBuffer;
}

////////////////////////////////////////////////////////////////////
// PORTAUDIO DEVICES PRINTING
////////////////////////////////////////////////////////////////////
PaDeviceIndex pa_find_device_by_name(const char *name) {
  int i;
  int numDevices;
  const PaDeviceInfo *pdi;
  int len = strlen(name);
  PaDeviceIndex result = paNoDevice;
  numDevices = Pa_GetDeviceCount();
  for(i = 0; i < numDevices; i++) {
    pdi = Pa_GetDeviceInfo(i);
    if(strncmp(name, pdi->name, len) == 0) {
      result = i;
      break;
    }
  }
  return result;
}

void printDevices_seeksASIODevices(int *indASIOinput, int *indASIOoutput,
	int *indDefaultInput, int *indDefaultOutput,
	bool verbose) {
	int numDevices, defaultDisplayed;
	const PaDeviceInfo *deviceInfo;

	numDevices = Pa_GetDeviceCount();
	if (numDevices < 0)
		return;

	printf("Number of devices found = %d\n", numDevices);
	*indASIOinput = -1;
	*indASIOoutput = -1;
	*indDefaultInput = -1;
	*indDefaultOutput = -1;
	for (int i = 0; i < numDevices; i++) {
		deviceInfo = Pa_GetDeviceInfo(i);
		if (verbose) {
			printf("--------------------------------------- device #%d\n", i);
		}
		/* Mark global and API specific default devices */
		defaultDisplayed = 0;
		if (i == Pa_GetDefaultInputDevice()) {
			if (verbose) {
				printf("[ Default Input");
			}
			defaultDisplayed = 1;
			*indDefaultInput = i;
		}
		else if (i == Pa_GetHostApiInfo(deviceInfo->hostApi)->defaultInputDevice) {
			const PaHostApiInfo *hostInfo = Pa_GetHostApiInfo(deviceInfo->hostApi);
			if (verbose) {
				printf("[ Default %s Input", hostInfo->name);
			}
			defaultDisplayed = 1;
			if (strcmp(hostInfo->name, "ASIO") == 0) {
				*indASIOinput = i;
			}
			if (*indDefaultInput == -1) {
				*indDefaultInput = i;
			}
		}

		if (i == Pa_GetDefaultOutputDevice()) {
			if (verbose) {
				printf((defaultDisplayed ? "," : "["));
				printf(" Default Output");
			}
			defaultDisplayed = 1;
			*indDefaultOutput = i;
		}
		else if (i == Pa_GetHostApiInfo(deviceInfo->hostApi)->defaultOutputDevice) {
			const PaHostApiInfo *hostInfo = Pa_GetHostApiInfo(deviceInfo->hostApi);
			if (verbose) {
				printf((defaultDisplayed ? "," : "["));
				printf(" Default %s Output", hostInfo->name);
			}
			defaultDisplayed = 1;
			if (strcmp(hostInfo->name, "ASIO") == 0) {
				*indASIOoutput = i;
			}
			if (*indDefaultOutput == -1) {
				*indDefaultOutput = i;
			}
		}
		if (defaultDisplayed)
			if (verbose) {
			printf(" ]\n");
			}

		/* print device info fields */
		if (verbose) {
			printf("Name                        = %s\n", deviceInfo->name);
			printf("Host API                    = %s\n", Pa_GetHostApiInfo(deviceInfo->hostApi)->name);
			printf("Max inputs = %d", deviceInfo->maxInputChannels);
			printf(", Max outputs = %d\n", deviceInfo->maxOutputChannels);

			printf("Default low input latency   = %8.3f\n", deviceInfo->defaultLowInputLatency);
			printf("Default low output latency  = %8.3f\n", deviceInfo->defaultLowOutputLatency);
			printf("Default high input latency  = %8.3f\n", deviceInfo->defaultHighInputLatency);
			printf("Default high output latency = %8.3f\n", deviceInfo->defaultHighOutputLatency);
		}
	}
}

////////////////////////////////////////////////////////////////////
// PORTAUDIO DEVICES PRINTING
////////////////////////////////////////////////////////////////////
bool Pa_portaudio_init_and_effects(void) {
  PaError err;

  //////////////////////////////////////////////////////
  // allocates the input and output buffers (stereo)
  for (int indCh = 0; indCh < PG_AUDIO_NB_CHANNELS; indCh++) {
	  pg_VSTeffect_InputBuffer[indCh] = new float[pg_VSTeffect_BufferSize];
	  pg_VSTeffect_OutputBuffer[indCh] = new float[pg_VSTeffect_BufferSize];

	  pg_FFT_InputBuffer[indCh] = new double[pg_VSTeffect_BufferSize];
	  pg_FFT_OutputBuffer[indCh] = new double[pg_VSTeffect_BufferSize];
  }
  pg_InLine_InputBuffer = new float[pg_InLineAndSamples_BufferSize * PG_AUDIO_NB_CHANNELS];
  pg_Samples_InputBuffer = new float[pg_InLineAndSamples_BufferSize * PG_AUDIO_NB_CHANNELS];
  pg_RhythmsBackTrack_InputBuffer = new float[pg_InLineAndSamples_BufferSize * PG_AUDIO_NB_CHANNELS];
  pg_InLine_OutputBuffer = new float[pg_InLineAndSamples_BufferSize * PG_AUDIO_NB_CHANNELS];
  pg_Samples_OutputBuffer = new float[pg_InLineAndSamples_BufferSize * PG_AUDIO_NB_CHANNELS];

  //////////////////////////////////////////////////////
  // PORTAUDIO INITIALIZATON
  err = Pa_Initialize();
  if( err != paNoError ) return pg_audio_errorMsg(err);

  printf("Pa_GetHostApiCount %d/default %d  - Pa_GetDeviceCount %d/default %d\n" , 
	  Pa_GetHostApiCount() ,  Pa_GetDefaultHostApi() , Pa_GetDeviceCount() , Pa_GetDefaultOutputDevice());
  int indASIOinput = -1;
  int indASIOoutput = -1;
  int indDefaultInput = -1;
  int indDefaultOutput = -1;
  printDevices_seeksASIODevices(&indASIOinput, &indASIOoutput,
											   &indDefaultInput, &indDefaultOutput ,
											   (VerboseDeviceOutput!=0?true:false));

  //////////////////////////////////
  // INPUT STREAM DATA
  PaStreamParameters pg_audio_intputParameters;

  if( AudioInputDevice == -1 ) {
	if( indASIOinput == -1 && indDefaultInput == -1) {
		printf("Error: No default input device.\n");
		return pg_audio_errorMsg(err);
	}
	else {
	  if( indASIOinput != - 1 )
		pg_audio_intputParameters.device = indASIOinput;
	  else
		pg_audio_intputParameters.device = indDefaultInput;
	}
  }
  else {
		pg_audio_intputParameters.device = AudioInputDevice; // from configuration file
  }
  const PaDeviceInfo *deviceInfo;
  deviceInfo = Pa_GetDeviceInfo(pg_audio_intputParameters.device);
  printf("\nUsing input device %d %s.\n",pg_audio_intputParameters.device,deviceInfo->name);
  if (strcmp(deviceInfo->name, "Focusrite USB 2.0 Audio Driver") != 0) {
	  printf("\n**********\n***  Focusrite sound input not opened. **\n***********\n\n");
  }
  else {
	  printf("Focusrite sound input opened.\n");
  }

  pg_audio_intputParameters.channelCount = PG_AUDIO_NB_CHANNELS;
  pg_audio_intputParameters.sampleFormat =  paFloat32;
  // printf("PG_AUDIO_SAMPLE_FORMAT %d\n",PG_AUDIO_SAMPLE_FORMAT);
  pg_audio_intputParameters.suggestedLatency 
	//  = Pa_GetDeviceInfo(pg_audio_intputParameters.device)->defaultHighOutputLatency;
  = 0.5*(Pa_GetDeviceInfo(pg_audio_intputParameters.device)->defaultLowOutputLatency
	  + Pa_GetDeviceInfo(pg_audio_intputParameters.device)->defaultHighOutputLatency);
  printf("PortAudio chosen latency %.4f (low %.4f - high %.4f)\n\n",
	  (float)pg_audio_intputParameters.suggestedLatency,
	  Pa_GetDeviceInfo(pg_audio_intputParameters.device)->defaultLowOutputLatency,
	  Pa_GetDeviceInfo(pg_audio_intputParameters.device)->defaultHighOutputLatency);
  pg_audio_intputParameters.hostApiSpecificStreamInfo = NULL;


  //////////////////////////////////
  // OUTPUT STREAM DATA
  PaStreamParameters pg_audio_outputParameters;

  pg_audio_outputParameters.device = Pa_GetDefaultOutputDevice();
  if( AudioOutputDevice == -1 ) {
	if( indASIOoutput == -1 && indDefaultOutput == -1 ) {
		printf("Error: No default output device.\n");
		return pg_audio_errorMsg(err);
	}
	else {
	  if( indASIOinput != - 1 )
		pg_audio_outputParameters.device = indASIOoutput;
	  else
		pg_audio_outputParameters.device = indDefaultOutput;
	}
  }
  else {
		pg_audio_outputParameters.device = AudioOutputDevice; // from configuration file
  }
  deviceInfo = Pa_GetDeviceInfo(pg_audio_outputParameters.device);
  printf("Using output device %d %s.\n",pg_audio_outputParameters.device,deviceInfo->name);
  if (strcmp( deviceInfo->name , "Focusrite USB 2.0 Audio Driver") != 0) {
	  printf("\n**********\n***  Focusrite sound output not opened. **\n***********\n\n");
  }
  else {
	  printf("Focusrite sound output opened.\n");
  }

  pg_audio_outputParameters.channelCount = PG_AUDIO_NB_CHANNELS;
  pg_audio_outputParameters.sampleFormat =  paFloat32;
  // printf("PG_AUDIO_SAMPLE_FORMAT %d\n",PG_AUDIO_SAMPLE_FORMAT);
  pg_audio_outputParameters.suggestedLatency 
	= Pa_GetDeviceInfo( pg_audio_outputParameters.device )->defaultLowOutputLatency;
  printf("PortAudio suggested latency %.4f\n\n",(float)pg_audio_outputParameters.suggestedLatency);
  pg_audio_outputParameters.hostApiSpecificStreamInfo = NULL;

  //////////////////////////////////
  // STREAM PROCESSING CALLBACK
  PaError ret = Pa_OpenStream(
				&pg_audio_stream,
				&pg_audio_intputParameters, /* input params */
				&pg_audio_outputParameters, /* output params */
				PG_AUDIO_SAMPLE_RATE,   
				paFramesPerBufferUnspecified, // framesPerBuffer
			    0, // flags
			    &pg_audio_paStreamCallback,
			    NULL //void *userData
			    );

  if(ret != paNoError) {
	printf("Pa_OpenStream failed: (err %i) %s\n", ret, Pa_GetErrorText(ret));
	if(pg_audio_stream)
	  Pa_CloseStream(pg_audio_stream);
	return false;
  }

  //////////////////////////////////
  // STREAM END CALLBACK
  err = Pa_SetStreamFinishedCallback( pg_audio_stream, &StreamFinished );
  if( err != paNoError ) return pg_audio_errorMsg(err);

  //////////////////////////////////
  // STREAM START
  err = Pa_StartStream(pg_audio_stream);
  if( err != paNoError ) return pg_audio_errorMsg(err);

  return true;
}
