/*
  ==============================================================================

    HeaderComponent.h
    Created: 12 Dec 2018 4:53:18pm
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
class HeaderComponent    : public PlumeComponent,
                           public Button::Listener
{
public:
    //==============================================================================
    HeaderComponent (PlumeProcessor& proc);
    ~HeaderComponent();

    //==============================================================================
    // PlumeComponent
    const String getInfoString() override;
    void update() override;
    
    //==============================================================================
    // Component
    void paint (Graphics&) override;
    void resized() override;
    
    //==============================================================================
    // Mouse Listener
    void mouseUp (const MouseEvent &event) override;
    void mouseEnter (const MouseEvent &event) override;
    void mouseExit (const MouseEvent &event) override;
    void buttonClicked (Button* bttn) override;
    
    //==============================================================================
    void drawPianoAndFolderPath (Path& pianoPath, Path& folderPath);
    void createPluginMenu (KnownPluginList::SortMethod sort);

private:
	static void pluginMenuCallback (int result, HeaderComponent* header);
	void handlePluginChoice (int chosenId);

    //==============================================================================
    PlumeProcessor& processor;
    Path eyePath;
    Path pianoPath;
    
    PopupMenu pluginListMenu;
    ScopedPointer<ShapeButton> pluginListButton;
    ScopedPointer<Label> pluginNameLabel;
    ScopedPointer<Label> presetNameLabel;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HeaderComponent)
};
