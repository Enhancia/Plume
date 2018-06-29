/*
  ==============================================================================

    WrappedPluginEditor.h
    Created: 12 Apr 2018 10:48:52am
    Author:  Alex LEVACHER

  ==============================================================================
*/

#pragma once

#include "../../JuceLibraryCode/JuceHeader.h"
#include "Wrapper/WrapperProcessor.h"

//==============================================================================
/**
 *  \class WrapperEditorWindow
 *
 *  \brief A desktop window containing the wrapped plugin's GUI.
 */
class WrapperEditorWindow  : public DocumentWindow
{
public:
    //==============================================================================
    WrapperEditorWindow (WrapperProcessor&);
    ~WrapperEditorWindow();

    //==============================================================================
    void closeButtonPressed() override ;
    AudioProcessorEditor* createProcessorEditor (AudioProcessor& processor);
    
    //==============================================================================
    WrapperProcessor& wrapperProcessor;
    
private:
    //==============================================================================
    float getDesktopScaleFactor() const override     { return 1.0f; }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WrapperEditorWindow)
};