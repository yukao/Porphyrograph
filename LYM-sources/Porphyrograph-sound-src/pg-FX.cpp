////////////////////////////////////////////////////////////////////
// 
// 
// 
//     File pg-FX.cpp
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
// FX ON OFF
////////////////////////////////////////////////////////////////////
//float sampleFXMix = 0.0f;
//float lineInFXMix = 0.0f;

////////////////////////////////////////////////////////////////////
// VST DATA
////////////////////////////////////////////////////////////////////
AEffect* myPlugin = NULL;
MVerb<float> VST_reverb_samples;
mdaDelay VST_delay_samples;
MVerb<float> VST_reverb_inLine;
mdaDelay VST_delay_inLine;

////////////////////////////////////////////////////////////////////
// VST PLUG-INS DATA
////////////////////////////////////////////////////////////////////
// C callbacks
//extern "C" {
//	// Main host callback
//	VstIntPtr VSTCALLBACK hostCallback(AEffect *effect, VstInt32 opcode,
//		VstInt32 index, VstInt32 value, void *ptr, float opt);
//}

extern "C" {
	VstIntPtr VSTCALLBACK hostCallback(AEffect *effect, VstInt32 opcode, VstInt32 index,
		VstInt32 value, void *ptr, float opt) {
		   //VSTEffect *vst = (effect ? (VSTEffect *) effect->user : NULL);
		switch (opcode) {
		case audioMasterVersion:
			return 2400;
		case audioMasterIdle:
			effect->dispatcher(effect, effEditIdle, 0, 0, 0, 0);
			// Handle other opcodes here... there will be lots of them
		case audioMasterGetTime :
			static VstTimeInfo time;
			memset(&time,0,sizeof(time));
			// flags
			//time.flags = kVstTempoValid|kVstBarsValid|kVstCyclePosValid|kVstPpqPosValid|kVstSmpteValid|kVstTimeSigValid;            
			//time.sampleRate = th->samplerate;
			//time.samplePos = th->samplepos;
			//time.ppqPos = th->ppqpos;

			//time.tempo = th->tempo;
			//time.barStartPos = th->barstartpos;
			//time.cycleStartPos = th->cyclestartpos;
			//time.cycleEndPos = th->cycleendpos;

			//time.timeSigNumerator = th->timesignom;
			//time.timeSigDenominator = th->timesigden;

			//// SMPTE data
			//time.smpteOffset = th->smpteoffset;
			//time.smpteFrameRate = th->smpterate;

			//// time.samplesToNextClock = 0;

			//if(value&kVstNanosValid) {
			//	time.nanoSeconds = flext::GetOSTime()*1.e9;
			//	time.flags |= kVstNanosValid;
			//}

			return (long)&time;
			// Handle other opcodes here... there will be lots of them
		default:
			printf("Plugin requested value of opcode %d\n", opcode);
			return NULL;
			break;
		}
	}
}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
// PLUG-IN UPLOAD
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
// https://gist.github.com/nikreiman/736032
// http://teragonaudio.com/article/How-to-make-your-own-VST-host.html

AEffect* loadVSTPlugin( char * vstPath ) {
	AEffect *plugin = NULL;

	// loads library with ascii path
	HINSTANCE modulePtr = LoadLibraryA(vstPath);
	if (modulePtr == NULL) {
		printf("Failed trying to load VST from '%s', error %d\n",
			vstPath, GetLastError());
		return NULL;
	}

	vstPluginFuncPtr mainEntryPoint =
		(vstPluginFuncPtr)GetProcAddress(modulePtr, "VSTPluginMain");
	// Instantiate the plugin
	plugin = mainEntryPoint(&hostCallback);
	if (plugin != NULL) {
		printf("Loaded VST plugin [%s]\n", vstPath);
	}
	return plugin;
}

// setting up plugin callback
bool configureVSTPlugin(AEffect *plugin, dispatcherFuncPtr *dispatcher) {
	// Check plugin's magic number
	// If incorrect, then the file either was not loaded properly, is not a real
	// VST plugin, or is otherwise corrupt.
	if (plugin->magic != kEffectMagic) {
		printf("Plugin's magic number is bad\n");
		return false;
	}

	// Create dispatcher handle
	*dispatcher = (dispatcherFuncPtr)(plugin->dispatcher);

	// Set up plugin callback functions
	plugin->getParameter = (getParameterFuncPtr)plugin->getParameter;
	plugin->processReplacing = (processFuncPtr)plugin->processReplacing;
	plugin->setParameter = (setParameterFuncPtr)plugin->setParameter;

	return true;
}

// suspend/resume plugin methods
void resumePlugin(AEffect *plugin, dispatcherFuncPtr dispatcher) {
	dispatcher(plugin, effMainsChanged, 0, 1, NULL, 0.0f);
}

void suspendPlugin(AEffect *plugin, dispatcherFuncPtr dispatcher) {
	dispatcher(plugin, effMainsChanged, 0, 0, NULL, 0.0f);
}

// what a plugin can do
bool canPluginDo(char *canDoString, AEffect *plugin, dispatcherFuncPtr dispatcher) {
	return (dispatcher(plugin, effCanDo, 0, 0, (void*)canDoString, 0.0f) > 0);
}

// initializing a plugin
void initVSTPlugin(AEffect *plugin, dispatcherFuncPtr dispatcher) {
	dispatcher(plugin, effOpen, 0, 0, NULL, 0.0f);

	// Set some default properties
	float sampleRate = 44100.0f;
	dispatcher(plugin, effSetSampleRate, 0, 0, NULL, sampleRate);
	int blocksize = 512;
	dispatcher(plugin, effSetBlockSize, 0, blocksize, NULL, 0.0f);

	resumePlugin(plugin, dispatcher);
}


void process_file_plugin(void) {
  // VST plugin test
  // loads VST plugin
  char *vstPath = "..\\share\\vst_plugins\\MVerb_vst\\MVerb\\win\\Release-Win32\\MVerb.dll";
  enum{ DAMPINGFREQ = 0, DENSITY, BANDWIDTHFREQ, DECAY, PREDELAY, SIZE, GAIN, MIX, EARLYMIX, NUM_PARAMS };
  // DampingFreq = 18000.; BandwidthFreq = 18000.; SampleRate = 44100.; Decay = 0.5;
  // Gain = 1.; Mix = 1.; Size = 1.; EarlyMix = 1.;
  myPlugin = loadVSTPlugin(vstPath);
  dispatcherFuncPtr dispatcher = NULL;
  if (configureVSTPlugin(myPlugin, &dispatcher)) {
	  printf("Configured VST plugin [%s]\n", vstPath);
	  initVSTPlugin(myPlugin, dispatcher);
	  myPlugin->setParameter(myPlugin, VstInt32(MIX), 0.35f);
	  myPlugin->setParameter(myPlugin, VstInt32(PREDELAY), 0.4f);
	  myPlugin->setParameter(myPlugin, VstInt32(DECAY), 0.5f);
  }
  // processes a file with this plugin
  string in = "moire\\life_002.wav";
  string out = "moire\\life_002_vst.wav";
   pg_VST_Sample_processor(&in, &out, myPlugin);
}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
// I/O EFFECTS
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

bool pg_VST_Sample_processor(string *inputFileName, string *outputFileName, AEffect* myPlugin) {
	bool valret = true;

	AudioSampleDataStruct *InputSampleBufferData = new AudioSampleDataStruct();

	SF_INFO outputSample_sndInfo;
	outputSample_sndInfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
	outputSample_sndInfo.channels = PG_AUDIO_NB_CHANNELS;
	outputSample_sndInfo.samplerate = PG_AUDIO_SAMPLE_RATE;

	////////////////////////////////////////////////////////////////
	// LOADS BIG SAMPLE
	// opens the sample
	InputSampleBufferData->name = *inputFileName;
	int formatSize;
	if (!(formatSize = InputSampleBufferData->pg_openSample())) {
		printf("File load failure.\n");
		valret = false;
	}
	// and loads it
	pg_readShortSampleBuffer((void *)InputSampleBufferData);

	/////////////////////////////////////////////////////
	// allocates the writing buffer
	outputSample_sndInfo.frames = InputSampleBufferData->sampleBufferFrameSize;
	float *pg_localInputEffectBuffer[2] = { NULL, NULL };
	pg_localInputEffectBuffer[0] = new float[(unsigned long)(outputSample_sndInfo.frames)];
	pg_localInputEffectBuffer[1] = new float[(unsigned long)(outputSample_sndInfo.frames)];
	for (unsigned long ind = 0; ind < (unsigned long)(outputSample_sndInfo.frames); ind++) {
		pg_localInputEffectBuffer[0][ind] = 0.f;
		pg_localInputEffectBuffer[1][ind] = 0.f;
	}
	float *pg_localOutputEffectBuffer[2] = { NULL, NULL };
	pg_localOutputEffectBuffer[0] = new float[(unsigned long)(outputSample_sndInfo.frames)];
	pg_localOutputEffectBuffer[1] = new float[(unsigned long)(outputSample_sndInfo.frames)];
	for (unsigned long ind = 0; ind < (unsigned long)(outputSample_sndInfo.frames); ind++) {
		pg_localOutputEffectBuffer[0][ind] = 0.f;
		pg_localOutputEffectBuffer[1][ind] = 0.f;
	}
	float *outputSampleBuffer = new float[(unsigned long)(outputSample_sndInfo.frames * PG_AUDIO_NB_CHANNELS)];
	for (unsigned long ind = 0; ind < (unsigned long)(outputSample_sndInfo.frames * PG_AUDIO_NB_CHANNELS); ind++) {
		outputSampleBuffer[ind] = 0.f;
	}

	/////////////////////////////////////////////////////
	// buffer copy / VST process / copy back
	for (long indTrack = 0; indTrack < outputSample_sndInfo.frames; indTrack++) {
		for (int indCh = 0; indCh < PG_AUDIO_NB_CHANNELS; indCh++) {
			pg_localInputEffectBuffer[indCh][indTrack]
				= InputSampleBufferData->largeSampleBuffer[indTrack * PG_AUDIO_NB_CHANNELS + indCh];
		}
	}

	//myPlugin->processReplacing(myPlugin, pg_localInputEffectBuffer, pg_localOutputEffectBuffer, 
	//							VstInt32(outputSample_sndInfo.frames));
	VST_reverb_samples.processReplacing(pg_localInputEffectBuffer, pg_localOutputEffectBuffer, int(outputSample_sndInfo.frames));
	VST_delay_samples.processReplacing(pg_localInputEffectBuffer, pg_localOutputEffectBuffer, int(outputSample_sndInfo.frames));

	for (long indTrack = 0; indTrack < outputSample_sndInfo.frames; indTrack++) {
		for (int indCh = 0; indCh < PG_AUDIO_NB_CHANNELS; indCh++) {
			outputSampleBuffer[indTrack * PG_AUDIO_NB_CHANNELS + indCh]
				= pg_localOutputEffectBuffer[indCh][indTrack];
		}
	}

	////////////////////////////////////////////////////////////////
	// SAVES OUTPUT SAMPLE
	// printf("small sample writing begin %s\n",fnameMicroSample);
	pg_audio_create_file(outputFileName->c_str(), &outputSample_sndInfo,
		long(outputSample_sndInfo.frames), outputSampleBuffer);
	// printf("small sample writing end\n");
	// printf("buffer deletion\n");

	delete pg_localInputEffectBuffer[0];
	delete pg_localInputEffectBuffer[1];
	delete pg_localOutputEffectBuffer[0];
	delete pg_localOutputEffectBuffer[1];
	delete outputSampleBuffer;
	delete InputSampleBufferData;
	return valret;
}

////////////////////////////////////////////////////////////////////
// EFFECTS INITIAL VALUES
////////////////////////////////////////////////////////////////////
void initial_values_FX( void ) {
	VST_reverb_samples.setParameter(MVerb<float>::DAMPINGFREQ, 0.45f);
	VST_reverb_samples.setParameter(MVerb<float>::DENSITY, 0.5f);
	VST_reverb_samples.setParameter(MVerb<float>::BANDWIDTHFREQ, 0.385f);
	VST_reverb_samples.setParameter(MVerb<float>::DECAY, 0.0f);
	VST_reverb_samples.setParameter(MVerb<float>::PREDELAY, 0.27f);
	VST_reverb_samples.setParameter(MVerb<float>::SIZE, 0.73f);
	VST_reverb_samples.setParameter(MVerb<float>::GAIN, 1.0f);
	VST_reverb_samples.setParameter(MVerb<float>::MIX, 0.0f);
	VST_reverb_samples.setParameter(MVerb<float>::EARLYMIX, 0.0f);
	VST_delay_samples.setParameter(kLDelay, 0.67f);
	VST_delay_samples.setParameter(kLRDelayRatio, 0.54f);
	VST_delay_samples.setParameter(kFeedBk, 0.58f);
	VST_delay_samples.setParameter(kTone, 0.5f);
	VST_delay_samples.setParameter(kMix, 0.0f);
	VST_delay_samples.setParameter(kLevel, 0.5f);

	VST_reverb_inLine.setParameter(MVerb<float>::DAMPINGFREQ, 0.45f);
	VST_reverb_inLine.setParameter(MVerb<float>::DENSITY, 0.5f);
	VST_reverb_inLine.setParameter(MVerb<float>::BANDWIDTHFREQ, 0.385f);
	VST_reverb_inLine.setParameter(MVerb<float>::DECAY, 0.0f);
	VST_reverb_inLine.setParameter(MVerb<float>::PREDELAY, 0.27f);
	VST_reverb_inLine.setParameter(MVerb<float>::SIZE, 0.73f);
	VST_reverb_inLine.setParameter(MVerb<float>::GAIN, 1.0f);
	VST_reverb_inLine.setParameter(MVerb<float>::MIX, 0.0f);
	VST_reverb_inLine.setParameter(MVerb<float>::EARLYMIX, 0.0f);
	VST_delay_inLine.setParameter(kLDelay, 0.67f);
	VST_delay_inLine.setParameter(kLRDelayRatio, 0.54f);
	VST_delay_inLine.setParameter(kFeedBk, 0.58f);
	VST_delay_inLine.setParameter(kTone, 0.5f);
	VST_delay_inLine.setParameter(kMix, 0.0f);
	VST_delay_inLine.setParameter(kLevel, 0.5f);

	//sampleFXMix = 0.0f;
	//lineInFXMix = 0.0f;
}



