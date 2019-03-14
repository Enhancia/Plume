/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Common/PlumeCommon.h"

#include "Ui/SideBar/SideBarComponent.h"
#include "Ui/SideBar/WrapperComponent.h"
#include "Ui/SideBar/PresetComponent.h"
#include "Ui/Header/HeaderComponent.h"
#include "Ui/Gesture/GesturePanel.h"
#include "Ui/LookAndFeel/PlumeLookAndFeel.h"
#include "Ui/Top/OptionsPanel.h"
#include "Ui/Top/NewPresetPanel.h"

//#define TRACE_IN  Logger::writeToLog ("[FNC] Entering function: " + __FUNCTION__);
//#define TRACE_OUT Logger::writeToLog ("[FNC] Entering function: " + __FUNCTION__);
//==============================================================================
/**
*/

class PresetComponent;

/**
 *  \class PlumeEditor PluginEditor.h
 *
 *  \brief Plume's editor component. Holds the ui of the plugin.
 *
 *  Holds plumes GUI. This component will create the child components that will handle the 3
 *  interface blocks: wrapper, presets, and gestures.
 */
class PlumeEditor  : public AudioProcessorEditor,
                     public ActionListener,
                     public Button::Listener
{
public:
    /**
     * \brief Constructor.
     *
     * Draws the background, calls the subcomponents and sets their bounds accordingly.
     *
     * \param p Reference to Plume's processor object.
     */
    PlumeEditor (PlumeProcessor& p);
    
    /**
     * \brief Destructor.
     */
    ~PlumeEditor();

    //==============================================================================
    /**
     * \brief JUCE Components' paint method.
     */
    void paint (Graphics& g) override;
    /**
     * \brief JUCE Components' paintOverChildren method to paint the plugin version.
     */
    //void paintOverChildren (Graphics& g) override;
    /**
     * \brief JUCE Components' resized method.
     */
    void resized() override;

    void broughtToFront() override;
    void minimisationStateChanged (bool) override;

    //==============================================================================
    /**
     * \brief Callback to a change message sent by the processor.
     *
     * This method is called by the processor when it needs the interface to be fully updated.
     * It calls the method updateFullInterface. sets the right current wrapped plugin, preset and gestures.
     */
    void actionListenerCallback (const String &message) override;
    
    void buttonClicked (Button* bttn) override;
    
    //==============================================================================
    PlumeProcessor& getProcessor();
    
    //==============================================================================
    /**
     * \brief Method that sets a full interface update.
     *
     * This method is called after a change message is sent by the processor.
     * It sets the right current wrapped plugin, preset and gestures.
     */
    void updateFullInterface();
    
    void setInterfaceUpdates (bool shouldUpdate);
    
private:
    //==============================================================================
    PlumeProcessor& processor; /**< \brief Reference to Plume's processor object */
    //ScopedPointer<WrapperComponent> wrapperComp; /**< \brief Object allowing to choose a plugin to wrap and use with neova */
    //ScopedPointer<PresetComponent> presetComp; /**< \brief Object allowing to save or load presets, in the xml format */
    ScopedPointer<GesturePanel> gesturePanel; /**< \brief Object that handles the different gesture gui objects */
    ScopedPointer<SideBarComponent> sideBar; /**< \brief Hideable SideBar object that displays the preset list, help, and buttons */
    ScopedPointer<HeaderComponent> header; /**< \brief Header object that displays the preset, and the wrapping features */
    ScopedPointer<ShapeButton> sideBarButton; /**< \brief Button that hides or shows the sidebar */
    ScopedPointer<OptionsPanel> optionsPanel;
    ScopedPointer<NewPresetPanel> newPresetPanel;
    
    
    //==============================================================================
    void createSideBarButtonPath(); //TODO mettre dans common avec les autres chemins
    
    //==============================================================================
    PLUME::UI::PlumeLookAndFeel plumeLookAndFeel;
    ScopedPointer<ResizableCornerComponent> resizableCorner;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlumeEditor)
};
