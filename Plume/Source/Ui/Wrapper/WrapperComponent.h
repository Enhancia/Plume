/*
  ==============================================================================

    WrapperComponent.h
    Created: 26 Jun 2018 5:42:23pm
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../../JuceLibraryCode/JuceHeader.h"
#include "Wrapper/PluginWrapper.h"
#include "Ui/LookAndFeel/PlumeLookAndFeel.h"

//==============================================================================
/*
*/
class WrapperComponent    : public Component,
                            private Button::Listener
{
public:
    //==============================================================================
    WrapperComponent (PluginWrapper& wrap);
    ~WrapperComponent();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    
    //==============================================================================
    void buttonClicked (Button* bttn) override;
    void mouseUp (const MouseEvent& event) override;
    
    //==============================================================================
    void scanPlugin();
    void openEditor();
    void closeEditor();
    bool hasEditor();
    void windowToFront();
    
    //==============================================================================
    void update();
    
private:
    //==============================================================================
    ScopedPointer<ImageButton> scanButton;
    ScopedPointer<ImageButton> openEditorButton;
    ScopedPointer<Label> pluginNameLabel;
    File lastPluginDir;
    
    //==============================================================================
    PluginWrapper& wrapper;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WrapperComponent)
};
