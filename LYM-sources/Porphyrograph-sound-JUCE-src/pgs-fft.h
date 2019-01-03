/*
  ==============================================================================

   This file is part of the JUCE tutorials.
   Copyright (c) 2017 - ROLI Ltd.

   The code included in this file is provided under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license. Permission
   To use, copy, modify, and/or distribute this software for any purpose with or
   without fee is hereby granted provided that the above copyright notice and
   this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES,
   WHETHER EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR
   PURPOSE, ARE DISCLAIMED.

  ==============================================================================
*/

/*******************************************************************************
 The block below describes the properties of this PIP. A PIP is a short snippet
 of code that can be read by the Projucer and used to generate a JUCE project.

 BEGIN_JUCE_PIP_METADATA

 name:             SimpleFFTTutorial
 version:          2.0.0
 vendor:           JUCE
 website:          http://juce.com
 description:      Displays an FFT spectrogram.

 dependencies:     juce_audio_basics, juce_audio_devices, juce_audio_formats,
                   juce_audio_processors, juce_audio_utils, juce_core,
                   juce_data_structures, juce_dsp, juce_events, juce_graphics,
                   juce_gui_basics, juce_gui_extra, juce_osc

 exporters:        xcode_mac, vs2017, linux_make

 type:             Component
 mainClass:        SpectrogramComponent

 useLocalCopy:     1

 END_JUCE_PIP_METADATA

*******************************************************************************/



#pragma once
class Application;
String trackFileName = String("");
bool trackFileOpen = false;

//==============================================================================
class SpectrogramComponent   : public AudioAppComponent,
                               private Timer,
							   private OSCReceiver,
							   private OSCReceiver::Listener<OSCReceiver::RealtimeCallback>
{
public:
    SpectrogramComponent()
        : forwardFFT (fftOrder),
#ifndef PGS_WITHOUT_VISUALS
          spectrogramImage (Image::RGB, 512, 512, true),
#endif
		  OSCReceiver()
    {
        setOpaque (true);
        setAudioChannels (2, 2);  // we want a couple of input channels but no outputs
        startTimerHz (60);
#ifndef PGS_WITHOUT_VISUALS
        setSize (700, 500);
#endif

		formatManager.registerBasicFormats();

		// specify here where to send OSC messages to: host URL and UDP port number
		if (!sender.connect("127.0.0.1", 9001))
			showConnectionErrorMessage("Error: could not connect to UDP port 9001.");

		// specify here on which UDP port number to receive incoming OSC messages
		if (!connect(8001))
			showConnectionErrorMessage("Error: could not connect to UDP port 8001.");

		// tell the component to listen for OSC messages matching this address:
		addListener(this);
	}

    ~SpectrogramComponent()
    {
        shutdownAudio();
		sender.disconnect();
		disconnect();
		removeListener(this);
	}

    //==============================================================================
	void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override
	{
		transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
	}

	void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override
	{
		auto* device = deviceManager.getCurrentAudioDevice();
		auto activeInputChannels = device->getActiveInputChannels();
		auto activeOutputChannels = device->getActiveOutputChannels();
		auto maxInputChannels = activeInputChannels.getHighestBit() + 1;
		auto maxOutputChannels = activeOutputChannels.getHighestBit() + 1;

		// temporary buffer for storing the soundtrack samples before mixing with audio input
		AudioSampleBuffer tmpBuffer(bufferToFill.buffer->getNumChannels(), 
									bufferToFill.buffer->getNumSamples());
		AudioSourceChannelInfo tmpBufferSource(tmpBuffer);
			
		// tests if there is a valid soundtrack to read or audio input
		for (auto outputChannel = 0; outputChannel < maxOutputChannels; ++outputChannel)
		{
			if (readerSource.get() == nullptr
				&& ((!activeOutputChannels[outputChannel]) || maxInputChannels == 0))
			{
				// std::cout << "Null audio source"  << std::endl;

				bufferToFill.buffer->clear(outputChannel, 
											bufferToFill.startSample, bufferToFill.numSamples);
				return;
			}
		}

		// gets the next block from the soundtrack
		transportSource.getNextAudioBlock(tmpBufferSource);

		for (auto outputChannel = 0; outputChannel < maxOutputChannels; ++outputChannel)
		{
			auto actualInputChannel = outputChannel % maxInputChannels; // [1]

		    // mixes the output according to the weights of soundtrack and audio input
			if (outputChannel < bufferToFill.buffer->getNumChannels())
			{
				auto* inBuffer = bufferToFill.buffer->getReadPointer(actualInputChannel,
					bufferToFill.startSample);
				auto* inTmpBuffer = tmpBufferSource.buffer->getReadPointer(actualInputChannel,
					tmpBufferSource.startSample);
				auto* outBuffer = bufferToFill.buffer->getWritePointer(outputChannel,
					bufferToFill.startSample);

				for (auto sample = 0; sample < bufferToFill.numSamples; ++sample)
				{
					outBuffer[sample] = audioInput_weight * inBuffer[sample]
						+ soundtrack_weight * inTmpBuffer[sample];
				}
			}

			// ships the samples of the soundtrack and audioInput to the spectrogram
			if (outputChannel == 0) {
				for (auto i = 0; i < bufferToFill.numSamples; ++i) {
					pushNextSampleIntoFifo(bufferToFill.buffer->getSample(0, i));
				}
			}
		}
	}

	void releaseResources() override
	{
		transportSource.releaseResources();
	}
	//void addListener(OSCReceiver::Listener<MessageLoopCallback>* listenerToAdd) override
	//{
	//}

#ifndef PGS_WITHOUT_VISUALS
    //==============================================================================
    void paint (Graphics& g) override
    {
        g.fillAll (Colours::black);

        g.setOpacity (1.0f);
        g.drawImage (spectrogramImage, getLocalBounds().toFloat());
    }
#endif

	void timerCallback() override
	{
		// spectrogram update
		if (nextFFTBlockReady)
		{
			// std::cout << "Next line of spectrogram" << std::endl;
			fftAnalysisAndDisplay();
			nextFFTBlockReady = false;
#ifdef PGS_WITHOUT_VISUAL
			repaint();
#endif
		}

#ifdef PGS_WITHOUT_VISUAL
		// audio file player interface update (not used currently)
		if (transportSource.isStarted())
		{
			RelativeTime position(transportSource.getCurrentPosition());

			auto minutes = ((int)position.inMinutes()) % 60;
			auto seconds = ((int)position.inSeconds()) % 60;
			auto millis = ((int)position.inMilliseconds()) % 1000;

			auto positionString = String::formatted("%02d:%02d:%03d", minutes, seconds, millis);
		}
		else
		{
			auto positionString = String("Stopped");
		}
#endif
	}

	void oscMessageReceived(const OSCMessage& message) override
	{
		String str = message.getAddressPattern().toString();
		// std::cout << "message size: " << message.size() << " address: " << str  << std::endl;
		// if (message.size() == 1 && message[0].isString())
		// {
		// std::cout << "message: " << str.toStdString() << " ("
		// 	<< message[0].getString().toStdString() << ")" << std::endl;
		// }
		if (str.compare("/JUCE_open_track") == 0) {
			if (message.size() == 1 && message[0].isString())
			{
				trackFileName = message[0].getString();

				if (trackFileName.endsWithChar('"')) {
					trackFileName = trackFileName.substring(0, trackFileName.length() - 1);
				}
				if (trackFileName.startsWithChar('"')) {
					trackFileName = trackFileName.substring(1, trackFileName.length());
				}
			}
			else
			{
				std::cout << "Incorrect " << str.toStdString() << " message size " << message.size() << "!" << std::endl;
				for (int ind = 0; ind < message.size(); ind++) {
					if (message[ind].isString())
					{
						std::cout << "message str: " << ind << " ("
							<< message[ind].getString().toStdString() << ")" << std::endl;
					}
					else if (message[ind].isInt32())
					{
						std::cout << "message int: " << ind << " ("
							<< message[ind].getInt32() << ")" << std::endl;
					}
					else if (message[ind].isFloat32())
					{
						std::cout << "message float: " << ind << " ("
							<< message[ind].getFloat32() << ")" << std::endl;
					}
				}
			}
			// std::cout << "File name: " << trackFileName << std::endl;
			if(!trackFileName.isEmpty())
			{
				auto trackFile = File(trackFileName);
				if (trackFile.existsAsFile())
				{
					auto* reader = formatManager.createReaderFor(trackFile);

					if (reader != nullptr)
					{
						if (state == Started) {
							changeState(Stopping);
						}
						std::unique_ptr<AudioFormatReaderSource> newSource(new AudioFormatReaderSource(reader, true));
						transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
						transportSource.setPosition(0.0);
						readerSource.reset(newSource.release());
						std::cout << "Audio file opened: " << trackFileName.toStdString() << std::endl;
						trackFileOpen = true;
					}
					else {
						std::cout << "Unknown audio file type: " << trackFileName.toStdString() << std::endl;
					}
				}
				else {
					std::cout << "Unknown file: " << trackFileName.toStdString() << std::endl;
				}
			}
			else {
				std::cout << "Incorrect sound_open message!" << std::endl;
			}
		}
		else if (str.compare("/JUCE_play_track") == 0) {
			if (trackFileOpen) {
				looping = true;
				updateLoopState(looping);
				changeState(Starting);
			}
		}
		else if (str.compare("/JUCE_stop_track") == 0) {
			changeState(Stopping);
		}
		else if (str.compare("/JUCE_loop_track") == 0) {
			looping = !looping;
			updateLoopState(looping);
		}
		else if (str.compare("/JUCE_soundtrack_weight") == 0) {
			if (message.size() == 1 && message[0].isFloat32()) {
				soundtrack_weight = message[0].getFloat32();
			}
		}
		else if (str.compare("/JUCE_audioInput_weight") == 0) {
			if (message.size() == 1 && message[0].isFloat32()) {
				audioInput_weight = message[0].getFloat32();
			}
		}
		else if (str.compare("/JUCE_exit") == 0) {
			// delete this;
			// app.systemRequestedQuit();
			// mainWindow = nullptr;
			exit(0);
		}
	}

   void pushNextSampleIntoFifo (float sample) noexcept
    {
	   // std::cout << "Index / Size: " << fifoIndex << " / " << fftSize << std::endl;

	   // if the fifo contains enough data, set a flag to say
        // that the next line should now be rendered..
        if (fifoIndex == fftSize)
        {
            if (! nextFFTBlockReady)
            {
                zeromem (fftData, sizeof (fftData));
                memcpy (fftData, fifo, sizeof (fifo));
                nextFFTBlockReady = true;
            }

            fifoIndex = 0;
        }

        fifo[fifoIndex++] = sample;
    }

   struct greater
   {
	   template<class T>
	   bool operator()(T const &a, T const &b) const { return a > b; }
   }; 
   
   void fftAnalysisAndDisplay()
    {
        // then calculate our FFT data..
        forwardFFT.performFrequencyOnlyForwardTransform (fftData);

		// Store fft data for sorting and sending it through UDP for visualization
		std::vector<std::vector<float>> val_and_id;
		val_and_id.resize(fftSize / 2);
		// frequency 0 (index 0 is not used)
		for (int i = 0; i < fftSize / 2 - 1; i++) {
			val_and_id[i].resize(3); // one to store value, the other for index.
			// Store fft bin frequency level in dimension 1:
			val_and_id[i][0] = fftData[i + 1];
			// Store fft bin frequency value in dim 2:
			// nthbin = n * (sampleFreq/num(DFTpoints))
			// For a 44100 sampling rate, we have a 22050 Hz band. With a 1024 FFT size, we divide this band into 512 bin
			// frequency associated with nth bin
			// nthBin = n * (44100 / 1024) = n * (44100 / 1024) = n * 43.06640625
			val_and_id[i][1] = (i + 1) * (44100.f / 1024.f);
			val_and_id[i][2] = fftData[2 * i + 1];
		}

		// sort bins by decreasing level values
		sort(val_and_id.begin(), val_and_id.end(), greater());
		// display them:
		//std::cout << "Level \t Freq \t Phase \n";
		//for (int i = 0; i < 8; i++) {
		//	std::cout << int(val_and_id[i][0]) << "\t" << int(val_and_id[i][1])  << "\t" << val_and_id[i][2] << "\n";
		//}

		// create and send an OSC message with an address and first 8 frequency/level pairs:
		//if (!sender.send("/fftLevel8", min_maxLevel.getStart(), min_maxLevel.getEnd()))
		//	showConnectionErrorMessage("Error: could not send OSC message.");
		// create and send an OSC message with an address and first 8 frequency/level/phase triplets:
		if (!sender.send("/fftLevel8",
			val_and_id[0][1], val_and_id[0][0], val_and_id[0][2], 
			val_and_id[1][1], val_and_id[1][0], val_and_id[1][2],
			val_and_id[2][1], val_and_id[2][0], val_and_id[2][2],
			val_and_id[3][1], val_and_id[3][0], val_and_id[3][2],
			val_and_id[4][1], val_and_id[4][0], val_and_id[4][2],
			val_and_id[5][1], val_and_id[5][0], val_and_id[5][2],
			val_and_id[6][1], val_and_id[6][0], val_and_id[6][2],
			val_and_id[7][1], val_and_id[7][0], val_and_id[7][2]) ) {
			std::cout << "Error: could not send OSC message!" << std::endl;
		}


#ifdef PGS_WITHOUT_VISUAL
		auto rightHandEdge = spectrogramImage.getWidth() - 1;
		auto imageHeight = spectrogramImage.getHeight();

		// first, shuffle our image leftwards by 1 pixel..
		spectrogramImage.moveImageSection(0, 0, 1, 0, rightHandEdge, imageHeight);

		// find the range of values produced, so we can scale our rendering to
		// show up the detail clearly
		auto min_maxLevel = FloatVectorOperations::findMinAndMax(fftData, fftSize / 2);
		for (auto y = 1; y < imageHeight; ++y)
		{
			auto skewedProportionY = 1.0f - std::exp(std::log(y / (float)imageHeight) * 0.2f);
			auto fftDataIndex = jlimit(0, fftSize / 2, (int)(skewedProportionY * fftSize / 2));
			auto level = jmap(fftData[fftDataIndex], 0.0f, jmax(min_maxLevel.getEnd(), 1e-5f), 0.0f, 1.0f);

			spectrogramImage.setPixelAt(rightHandEdge, y, Colour::fromHSV(level, 1.0f, level, 1.0f));
		}
#endif
	}

	void updateLoopState(bool shouldLoop)
	{
		if (readerSource.get() != nullptr)
			readerSource->setLooping(shouldLoop);
	}
	
	enum
    {
        fftOrder = 10,
        fftSize  = 1 << fftOrder
    };

private:
	//==============================================================================
	void showConnectionErrorMessage(const String& messageText)
	{
		AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon,
			"Connection error",
			messageText,
			"OK");
	}

	//==============================================================================
	enum TransportState
	{
		Starting = 0,
		Started,
		Stopping,
		Stopped,
	};

	void changeState(TransportState newState)
	{
		if (state != newState)
		{
			TransportState oldState = state;
			state = newState;

			switch (state)
			{
			case Starting:
				std::cout << "START" << std::endl;
				transportSource.start();
				changeState(Started);
				break;

			case Started:
				break;

			case Stopping:
				if (oldState == Started) {
					std::cout << "STOP" << std::endl;
					transportSource.stop();
					changeState(Stopped);
				}
				break;

			case Stopped:
				break;
			}
		}
	}

	enum TransportState state = Stopped;
	bool looping = false;
	AudioFormatManager formatManager;
	std::unique_ptr<AudioFormatReaderSource> readerSource;
	AudioTransportSource transportSource;

	float soundtrack_weight = 1.f;
	float audioInput_weight = 0.f;

	dsp::FFT forwardFFT;
#ifdef PGS_WITHOUT_VISUAL
    Image spectrogramImage;
#endif

    float fifo [fftSize];
    float fftData [2 * fftSize];
    int fifoIndex = 0;
    bool nextFFTBlockReady = false;

	OSCSender sender;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SpectrogramComponent)
};
