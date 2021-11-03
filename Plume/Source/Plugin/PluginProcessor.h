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
    enum midiSequenceId
    {
        noSequence =0,
        normal,
        recording,
        alternatingNormal,
        alternatingRecording,
        normalAndRecording
    };

    //==============================================================================
    /**
     * \brief Constructor.
     *
     * Instanciates the objects used by the processor.
     */    
    PlumeProcessor();
    /**
     * \brief Destructor.
     */
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
    void setArm (PLUME::param::armValue newArm);
    bool& getLastArmRef();

    //==============================================================================
    void sendUnlockSignalWhenPossible();

    //==============================================================================
    /**
     * \brief State save method.
     *
     * Creates the data representing the current state of the plugin. It will call
     * both createWrapperXml and createGesturesXml, then save the data in the specified
     * memory block.
     *
     * \param destData The memory block in which the data will be saved.
     */  
    void getStateInformation (MemoryBlock& destData) override;
    /**
     * \brief State load method.
     *
     * Reads the specified data and changes the state of the plugin accordingly.
     *
     * \param destData The memory block in which the data will be saved.
     */
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    /**
     * \brief Xml creator for general plume information.
     *
     * \param wrapperData XmlElement that will get the general Xml as a child element.
     */  
    void createGeneralXml (XmlElement& wrapperData);
    
    /**
     * \brief Xml creator for wrapped plugin related information.
     *
     * Creates the xml data concerning the wrapped plugin, which holds it's name, PluginDescription
     * and it's state so that it may be restored to the very same state later.
     * This Xml element is attached as a child element of the parameter XmlElement wrapperData.
     *
     * \param wrapperData XmlElement that will get the plugin Xml as a child element.
     */  
    void createPluginXml (XmlElement& wrapperData);
    
    /**
     * \brief Xml creator for gesture related information.
     *
     * Creates the xml data concerning the gestures, which holds their name, sensivities, mapped
     * parameters and midi information.
     * This Xml element is attached as a child element to the parameter XmlElement wrapperData.
     *
     * \param wrapperData XmlElement that will get the gesture Xml as a child element.
     */  
    void createGestureXml (XmlElement& wrapperData);
    
    /**
     * \brief Method called by setStateInformation to load the wrapped plugin.
     *
     * Will attempt to restore plume to the state described by pluginData.
     *
     * \param pluginData XmlElement that is used to set Plume's wrapper state.
     */  
    void loadPluginXml(const XmlElement& pluginData);
    
    /**
     * \brief Method called by setStateInformation to load the gestures.
     *
     * Will attempt to restore plume's gestures to the state described by gestureData.
     *
     * \param pluginData XmlElement that is used to set Plume's wrapper state.
     */   
    void loadGestureXml(const XmlElement& gestureData);
    
    //==============================================================================
    // Getters to the main function objects
    
    /**
     * \brief PluginWrapper getter.
     *
     * \return Reference to the PluginWrapper object.
     */
    PluginWrapper& getWrapper();
    /**
     * \brief DataReader getter.
     *
     * \return Pointer to the DataReader object. Handle with Care.
     */
    DataReader* getDataReader();
    /**
     * \brief GestureArray getter.
     *
     * \return Reference to the GestureArray object.
     */
    GestureArray& getGestureArray();
    /**
     * \brief AudioProcessorValueTreeState getter.
     *
     * \return Reference to the AudioProcessorValueTreeState object.
     */
    AudioProcessorValueTreeState& getParameterTree();
    /**
     * \brief PresetHandler getter.
     *
     * \return Reference to the PresetHandler object.
     */
    PresetHandler& getPresetHandler();
    /**
     * \brief PlumeUpdater getter.
     *
     * \return Reference to the PlumeUpdater object.
     */
    PlumeUpdater& getUpdater();
    void startDetectingAuthSequence();
    void addListenerForPlumeControlParam (AudioProcessorParameter* plumeControlParam);
    void removeListenerForPlumeControlParam (AudioProcessorParameter* plumeControlParam);
    
private:
    //==============================================================================
    void checkAndUpdateRecordingStatus();
    void filterInputMidi (MidiBuffer& midiMessages);
    bool messageShouldBeKept (const MidiMessage& midiMessage);
    bool lastArm = false;

    //==============================================================================
    AudioProcessorValueTreeState::ParameterLayout initializeParameters();
    void initializeSettings();
    void removeLogger();

    //==============================================================================
    void initializeParamSequences();
    void stopAuthDetection (bool isDetectionSuccessful);
    void startSendingUnlockParamSequence();
    bool isNextStepInAuthSequence (float recievedValue);
    bool isDetectingAuthSequence = false;
    bool presetIsLocked = false;
    int stepInAuthSequence = 0;
    int stepInUnlockSequence = 0;

    //==============================================================================
    std::unique_ptr<FileLogger> plumeLogger; /**< \brief Logger object. Allows to write logs for testing purposes. */
    std::unique_ptr<PluginWrapper> wrapper; /**< \brief PluginWrapper object. Handles the plugin wrapping. */
    std::unique_ptr<DataReader> dataReader; /**< \brief DataReader object. Recieves the data from the ring. */
    std::unique_ptr<GestureArray> gestureArray; /**< \brief GestureArray object. Stores all current gesture objects. */
    std::unique_ptr<PresetHandler> presetHandler; /**< \brief PresetHandler object. Stores preset directories and lists of all presets. */
    std::unique_ptr<PlumeUpdater> updater; /**< \brief Plume Updater object. Downloads new Plume installer and launches it. */
    
    //==============================================================================
    //ValueTree settings;
    AudioProcessorValueTreeState parameters;

    //==============================================================================
    Array<unsigned char> authParamSequence;
    Array<float> unlockParamSequence;

    bool shouldSendUnlockSequence = false;
    bool lastRecordingStatus = false;
    uint8_t data[1024];

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlumeProcessor)
    //JUCE_HEAVYWEIGHT_LEAK_DETECTOR (PlumeProcessor)
};
