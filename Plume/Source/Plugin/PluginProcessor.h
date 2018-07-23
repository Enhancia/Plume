/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "Wrapper/PluginWrapper.h"
#include "Gesture/Gesture.h"
#include "Gesture/GestureArray.h"
#include "DataReader/DataReader.h"

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
                        public ChangeBroadcaster
{
public:
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

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

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
    void setCurrentProgram (int index) override {};
    const String getProgramName (int index) override { return {}; };
    void changeProgramName (int index, const String& newName) override {};

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
    
    void createPluginXml (XmlElement& wrapperData);
    void createGestureXml (XmlElement& wrapperData);
    
    void loadPluginXml(const XmlElement& pluginData);
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
     * \return Reference to the DataReader object.
     */
    DataReader* getDataReader();
    /**
     * \brief GestureArray getter.
     *
     * \return Reference to the GestureArray object.
     */
    GestureArray& getGestureArray();
    

private:
    //==============================================================================
    ScopedPointer<FileLogger> plumeLogger; /**< \brief Logger object. Allows to write logs for testing purposes. */
    ScopedPointer<PluginWrapper> wrapper; /**< \brief PluginWrapper object. Handles the plugin wrapping. */
    ScopedPointer<DataReader> dataReader; /**< \brief DataReader object. Recieves the data from the ring. */
    ScopedPointer<GestureArray> gestureArray; /**< \brief GestureArray object. Stores all current gesture objects. */
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlumeProcessor)
};
