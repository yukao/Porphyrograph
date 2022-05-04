/*
  ==============================================================================

    This file was auto-generated and contains the startup code for a PIP.

  ==============================================================================
*/

#define PGS_WITHOUT_VISUALS

#include "../JuceLibraryCode/JuceHeader.h"
#include "pgs-fft.h"

class Application    : public JUCEApplication 
{
public:
    //==============================================================================
    Application() {}

    const String getApplicationName() override       { return "porphyrograph-sound-JUCE"; }
    const String getApplicationVersion() override    { return "1.0.0"; }

#ifndef PGS_WITHOUT_VISUALS
    void initialise (const String&) override         { mainWindow.reset (new MainWindow ("porphyrograph-sound-JUCE", new SpectrogramComponent(), *this)); }
	void shutdown() override { mainWindow = nullptr; }
#else
	void initialise(const String&) override			 { mainComponent.reset(new MainComponent(*this)); }
	void shutdown() override { mainComponent = nullptr; }
#endif
	//==============================================================================
	void systemRequestedQuit() override
	{
		// This is called when the app is being asked to quit: you can ignore this
		// request and let the app carry on running, or call quit() to allow the app to close.
		quit();
	}

private:
#ifndef PGS_WITHOUT_VISUALS
    class MainWindow    : public DocumentWindow
    {
    public:
        MainWindow (const String& name, Component* c, JUCEApplication& a)
            : DocumentWindow (name, Desktop::getInstance().getDefaultLookAndFeel()
                                                          .findColour (ResizableWindow::backgroundColourId),
                              DocumentWindow::allButtons),
              app (a)
        {
            setUsingNativeTitleBar (true);
            setContentOwned (c, true);

           #if JUCE_ANDROID || JUCE_IOS
            setFullScreen (true);
           #else
            setResizable (true, false);
            setResizeLimits (300, 250, 10000, 10000);
            centreWithSize (getWidth(), getHeight());
           #endif

            setVisible (true);
        }

        void closeButtonPressed() override
        {
            app.systemRequestedQuit();
        }

    private:
        JUCEApplication& app;

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
    };

    std::unique_ptr<MainWindow> mainWindow;
#else
	class MainComponent : public Component
	{
	public:
		Component * c;
		MainComponent(JUCEApplication& a)
			: app(a)
		{
			c = new SpectrogramComponent();
		}

		~MainComponent()
		{
			delete c;
			app.systemRequestedQuit();
		}

	private:
		JUCEApplication & app;

		//==============================================================================
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
	};

	std::unique_ptr<MainComponent> mainComponent;

#endif

};

//==============================================================================
START_JUCE_APPLICATION (Application)
