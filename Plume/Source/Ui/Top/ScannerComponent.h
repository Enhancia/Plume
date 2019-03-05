/*
  ==============================================================================

    ScannerComponent.h
    Created: 31 Jan 2019 11:42:32am
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../../JuceLibraryCode/JuceHeader.h"
#include "Common/PlumeCommon.h"
#include "Plugin/PluginProcessor.h"

//==============================================================================
/*
*/
class ScannerComponent    : public Component,
                            private Button::Listener,
                            private Timer
{
public:
    //==============================================================================
    ScannerComponent (PlumeProcessor& proc, int buttonWidth =-1);
    ~ScannerComponent();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    
    //==============================================================================
    void buttonClicked (Button* bttn) override;
    void timerCallback() override;

private:
    //==============================================================================
    void scanPlugins();
    void cancelScan();
    void scanFinished();
	bool doNextScan();
    
    //==============================================================================
    ScopedPointer<TextButton> scanButton;
    ScopedPointer<TextButton> cancelButton;
    ScopedPointer<PlumeProgressBar> bar;
    
    PlumeProcessor& processor;
    
    //==============================================================================
    int buttonW;

    //==============================================================================
    bool scanning = false;
    int formatToScan = 0;
    String pluginBeingScanned = "";
    float scanProgress = 0.0f;
    ScopedPointer<PluginDirectoryScanner> dirScanner;
    
    //==============================================================================
    ScopedPointer<ThreadPool> threadPool;
    int numThreads = 1;

	struct ScanJob : public ThreadPoolJob
	{
		ScanJob(ScannerComponent& s) : ThreadPoolJob("pluginscan"), scannerComp(s)
		{
		}

		JobStatus runJob()
		{
			while (scannerComp.doNextScan() && !shouldExit())
			{
			}

			return jobHasFinished;
		}

		ScannerComponent& scannerComp;

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScanJob)
	};
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ScannerComponent)
};
