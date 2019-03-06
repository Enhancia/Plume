/*
  ==============================================================================

    WrappedPluginEditor.h
    Created: 12 Apr 2018 10:48:52am
    Author:  Alex LEVACHER

  ==============================================================================
*/

#pragma once

#include "../../JuceLibraryCode/JuceHeader.h"
#include "Common/PlumeCommon.h"
#include "Wrapper/WrapperProcessor.h"

//==============================================================================
/**
 *  \class WrapperEditorWindow
 *
 *  \brief A desktop window containing the wrapped plugin's GUI.
 */
class WrapperEditorWindow  : public Component
{
public:
    //==============================================================================
	WrapperEditorWindow (WrapperProcessor&, const Component* componentWindowToAttachTo =nullptr);
    ~WrapperEditorWindow();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    void childBoundsChanged(Component*) override;

    //==============================================================================
    void userTriedToCloseWindow() override;
    /**
     * \brief Called to indicate that this component has just acquired the keyboard focus.
     */
    //void focusGained (FocusChangeType cause) override;
    
    
    //==============================================================================
    WrapperProcessor& wrapperProcessor; /**< \brief Reference to the wrapper processor object linked to this editor */
    
private:
    //==============================================================================
    float getDesktopScaleFactor() const override     { return 1.0f; }

    //==============================================================================
    AudioProcessorEditor* createProcessorEditor (AudioProcessor& processor);

    SafePointer<AudioProcessorEditor> wrappedUi;
    ComponentBoundsConstrainer constrainer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WrapperEditorWindow)
};