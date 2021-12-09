/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../../JuceLibraryCode/JuceHeader.h"
#include "../Common/PlumeCommon.h"

#include "../Gesture/GestureArray.h"
#include "../Wrapper/PluginWrapper.h"
#include "../Gesture/Gesture.h"
#include "../DataReader/DataReader.h"
#include "../Presets/PresetHandler.h"
#include "../Updater/PlumeUpdater.h"

//==============================================================================
/**
*/

/**
 *  \class PlumeProcessor PluginProcessor.h
 *
 *  \brief Plume's processor object.
 *
 *  The object that runs plume. Most of it's functions are held by objects instanciated
 *  in this object.
 */
 
class PlumeProcessor  : public AudioProcessor,
                        public ActionBroadcaster,
                        public AudioProcessorParameter::Listener,
                        public MultiTimer
{
public:
    //============================================================================== 
    PlumeProcessor();
    ~PlumeProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override { return JucePlugin_Name; };

    bool acceptsMidi() const override { return true; };
    bool producesMidi() const override { return true; };
    bool isMidiEffect() const override { return false; };
    double getTailLengthSeconds() const override { return 0.0; };

    //==============================================================================
    int getNumPrograms() override { return 1; };
    int getCurrentProgram() override { return 0; };
    void setCurrentProgram (int) override {};
    const String getProgramName (int) override { return {}; };
    void changeProgramName (int, const String&) override {};
    
    //==============================================================================
    void updateTrackProperties (const AudioProcessor::TrackProperties& properties) override;

    //==============================================================================
    void timerCallback (int timerID) override;
    void parameterValueChanged (int parameterIndex, float newValue) override;
    void parameterGestureChanged (int parameterIndex, bool gestureIsStarting) override;

    //==============================================================================
    /**
        \brief Setter for Plume's arm parameter.

        When Plume is not armed, it wont update gestures with incoming ring values. This will lead
        to automations not being ovewritten in a DAW with no parameter automation modes (such as Ableton Live).
    
        \param parameterIndex
     */
    void setArm (PLUME::param::armValue newArm);
    
    /**
        \brief Signals the processor it can start sending the unlock sequence.

        This method is called after a preset sent its authentification sequence. It will start a timer
        that will send a new encoded character every time it ticks. 

        \param newArm The Arm value to set
     */ 
    bool& getLastArmRef();

    //==============================================================================
    /**
        \brief Signals the processor it can start sending the unlock sequence.

        This method is called after a preset sent its authentification sequence. It will start a timer
        that will send a new encoded character every time it ticks. 
     
     */ 
    void sendUnlockSignalWhenPossible();

    //==============================================================================
    /**
        \brief State save method.
        
        Creates the data representing the current state of the plugin. It will call
        both createWrapperXml and createGesturesXml, then save the data in the specified
        memory block.
        
        \param destData The memory block in which the data will be saved.
     */  
    void getStateInformation (MemoryBlock& destData) override;
    /**
        \brief State load method.
        
        Reads the specified data and changes the state of the plugin accordingly.
        
        \param destData The memory block in which the data will be saved.
     */
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    /**
        \brief Xml creator for general plume information.
     
        \param wrapperData XmlElement that will get the general Xml as a child element.
     */  
    void createGeneralXml (XmlElement& wrapperData);
    
    /**
        \brief Xml creator for wrapped plugin related information.
        
        Creates the xml data concerning the wrapped plugin, which holds it's name, PluginDescription
        and it's state so that it may be restored to the very same state later.
        This Xml element is attached as a child element of the parameter XmlElement wrapperData.
        
        \param wrapperData XmlElement that will get the plugin Xml as a child element.
     */  
    void createPluginXml (XmlElement& wrapperData);
    
    /**
        \brief Xml creator for gesture related information.
        
        Creates the xml data concerning the gestures, which holds their name, sensivities, mapped
        parameters and midi information.
        This Xml element is attached as a child element to the parameter XmlElement wrapperData.
        
        \param wrapperData XmlElement that will get the gesture Xml as a child element.
     */  
    void createGestureXml (XmlElement& wrapperData);
    
    /**
        \brief Method called by setStateInformation to load the wrapped plugin.
        
        Will attempt to restore plume to the state described by pluginData.
        
        \param pluginData XmlElement that is used to set Plume's wrapper state.
     */  
    void loadPluginXml(const XmlElement& pluginData);
    
    /**
        \brief Method called by setStateInformation to load the gestures.
        
        Will attempt to restore plume's gestures to the state described by gestureData.
        
        \param pluginData XmlElement that is used to set Plume's wrapper state.
     */   
    void loadGestureXml(const XmlElement& gestureData);
    
    //==============================================================================
    // Getters to the main function objects
    
    /**
        \brief PluginWrapper getter.
        
        \return Reference to the PluginWrapper object.
     */
    PluginWrapper& getWrapper();
    /**
        \brief DataReader getter.
        
        \return Pointer to the DataReader object. Handle with Care.
     */
    DataReader* getDataReader();
    /**
        \brief GestureArray getter.
        
        \return Reference to the GestureArray object.
     */
    GestureArray& getGestureArray();
    /**
        \brief AudioProcessorValueTreeState getter.
        
        \return Reference to the AudioProcessorValueTreeState object.
     */
    AudioProcessorValueTreeState& getParameterTree();
    /**
        \brief PresetHandler getter.
        
        \return Reference to the PresetHandler object.
     */
    PresetHandler& getPresetHandler();
    /**
        \brief PlumeUpdater getter.
        
        \return Reference to the PlumeUpdater object.
     */
    PlumeUpdater& getUpdater();
    /**
        \brief Starts the auth detection process.
        
        This method will setup the auth detection, that will either lead to starting the unlock process or end in a timeout.
     */
    void startDetectingAuthSequence();
    /**
        \brief Registers the specific param as a listener to start the auth and unlock sequences.

        \param plumeControlParam The specific parameter used for the auth and unlock processes. 
     */
    void addListenerForPlumeControlParam (AudioProcessorParameter* plumeControlParam);
    /**
        \brief Removes the specific param as a listener to stop the auth or unlock sequence.        
     
        \param plumeControlParam The specific parameter used for the auth and unlock processes.
     */
    void removeListenerForPlumeControlParam (AudioProcessorParameter* plumeControlParam);
    /**
        \brief Getter for the internal plumeCrashed value.        
     
        \returns True is this Plume file encountered a crash the last time it was used.
     */
    bool hasLastSessionCrashed();
    
private:
    //==============================================================================
    /**
        \brief Checks if last Plume session ended with a crash.        
        
        The method will ask the interface for the according popup prompting action from the user.     
     */
    void detectPlumeCrashFromPreviousSession();
    
    //==============================================================================
    /**
        \brief Removes the specific param as a listener to stop the auth or unlock sequence.        
     
        \param plumeControlParam The specific parameter used for the auth and unlock processes.
     */
    void checkAndUpdateRecordingStatus();
    /**
        \brief Helper method to remove unwanted midi messages from the DAW's buffer.        
     
        This will mostly take care of parasite messages that use the same midi type as one or several
        of the current gestures.

        \param midiMessages Reference to the MidiBuffer object from the DAW.
     */
    void filterInputMidi (MidiBuffer& midiMessages);
    /**
        \brief Helper method to check if a message should be filtered.

        \param midiMesssage The midi message to check.
        \returns true if the message should be kept, false if it should be filtered.
     */
    bool messageShouldBeKept (const MidiMessage& midiMessage);

    //==============================================================================
    /**
        \brief Helper method to create the plugin parameters for Plume.
        
        This should be used in Plume's constructor to create the right AudioParameterValueTreeState object.

        \returns The layout for Plume's parameters.
     */
    AudioProcessorValueTreeState::ParameterLayout initializeParameters();
    /**
        \brief Helper method to set default setting values for Plume.

        This uses the config file from the user if it exists, otherwise it uses the default values and creates the file instead.
     */
    void initializeSettings();
    /**
        \brief Helper method to cleanly delete the logger after ending Plume's session.
     */
    void removeLogger();

    //==============================================================================
    /**
        \brief Helper method to create the sequences for the auth and unlock processes.
     */
    void initializeParamSequences();
    /**
        \brief Stops the process after a auth attempt for current preset.

        All related attributes will be reset after the method is called. Depending of the successn they will be
        set to according values.
     */
    void stopAuthDetection (bool isDetectionSuccessful);
    /**
        \brief Starts the unlock process for current preset.
     */
    void startSendingUnlockParamSequence();
    /**
        \brief Checks if receivedValue is the right value according to the stored Auth sequence.
        
        If receivedValue is the right value, advances auth detection to next step.

        \param receivedValue The next value to check for auth detection.
        \returns true if receivedValue is the next step in the auth sequence.
     */
    bool isNextStepInAuthSequence (float recievedValue);

    bool isDetectingAuthSequence = false; /**< \brief Lets Plume check if Plume is currently in the auth detection process. */
    bool presetIsLocked = false; /**< \brief Tells Plume if the present is currently locked and should therefore not receive MIDI. */
    int stepInAuthSequence = 0; /**< \brief Int that tracks the current step in the auth process. */
    int stepInUnlockSequence = 0; /**< \brief Int that tracks the current step in the unlock process. */
    bool lastArm = false; /**< \brief Boolean that tells if the track Plume is in is currently armed or not. */

    //==============================================================================
    std::unique_ptr<FileLogger> plumeLogger; /**< \brief Logger object. Allows to write logs for testing purposes. */
    std::unique_ptr<PluginWrapper> wrapper; /**< \brief PluginWrapper object. Handles the plugin wrapping. */
    std::unique_ptr<DataReader> dataReader; /**< \brief DataReader object. Recieves the data from the ring. */
    std::unique_ptr<GestureArray> gestureArray; /**< \brief GestureArray object. Stores all current gesture objects. */
    std::unique_ptr<PresetHandler> presetHandler; /**< \brief PresetHandler object. Stores preset directories and lists of all presets. */
    std::unique_ptr<PlumeUpdater> updater; /**< \brief Plume Updater object. Downloads new Plume installer and launches it. */
    
    //==============================================================================
    //ValueTree settings;
    AudioProcessorValueTreeState parameters; /**< \brief Holds all plugin parameters for Plume. */

    //==============================================================================
    bool plumeCrashed = false;

    //==============================================================================
    Array<unsigned char> authParamSequence; /**< \brief Sequence expected to receive for auth. */
    Array<float> unlockParamSequence; /**< \brief Float sequence to send for unlock. */

    bool shouldSendUnlockSequence = false; /**< \brief Tells Plume if an unlock is needed for the specific preset. */
    bool lastRecordingStatus = false; /**< \brief Tracks the recording status in Plume's track. */
    uint8_t data[1024];

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlumeProcessor)
    //JUCE_HEAVYWEIGHT_LEAK_DETECTOR (PlumeProcessor)
};
