/*
  ==============================================================================

    WrapperPluginProcessor.h
    Created: 9 Apr 2018 3:23:04pm
    Author:  Alex LEVACHER

  ==============================================================================
*/

#pragma once

#include "../../JuceLibraryCode/JuceHeader.h"
#include "../Common/PlumeCommon.h"

class PluginWrapper;

class WrapperProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    WrapperProcessor(AudioPluginInstance&, PluginWrapper&, AudioProcessorParameter::Listener& proc);
    ~WrapperProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override
    {   return plugin.prepareToPlay (sampleRate, samplesPerBlock); }
    
    void releaseResources() override
    {   return plugin.releaseResources(); }
    
    //==============================================================================
    void processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages) override ;

    //==============================================================================
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    bool applyBusLayouts (const BusesLayout &layouts) override;

    //==============================================================================
    bool canAddBus (bool isInput) const override;
    bool canRemoveBus (bool isInput) const override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override { return plugin.createEditor(); }
    bool hasEditor() const override               { return plugin.hasEditor();    }

    //==============================================================================
    const String getName() const override        { return plugin.getName();              }

    bool acceptsMidi() const override            { return plugin.acceptsMidi();          }
    bool producesMidi() const override           { return plugin.producesMidi();         }
    bool isMidiEffect() const override           { return plugin.isMidiEffect();         }
    double getTailLengthSeconds() const override { return plugin.getTailLengthSeconds(); }

    //==============================================================================
    int getNumPrograms() override                                      { return plugin.getNumPrograms();                   }
    int getCurrentProgram() override                                   { return plugin.getCurrentProgram();                }
    void setCurrentProgram (int index) override                        {        plugin.setCurrentProgram (index);          }
    const String getProgramName (int index) override                   { return plugin.getProgramName (index);             }
    void changeProgramName (int index, const String& newName) override {        plugin.changeProgramName (index, newName); }

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override             { plugin.getStateInformation (destData);          }
    void setStateInformation (const void* data, int sizeInBytes) override { plugin.setStateInformation (data, sizeInBytes); }
    
    //==============================================================================
    void clearEditor();
    
    //==============================================================================
    AudioProcessorParameter& getWrappedParameter (int id);
    
    //==============================================================================
    PluginWrapper& getOwnerWrapper();
    AudioPluginInstance& getWrappedInstance();
    
private:
    //==============================================================================
    void initWrappedParameters();
    
    //==============================================================================
    void writeBusesLayoutToLog();
    void copyWrapperBuffersIntoPlumeBuffer (AudioBuffer<float>& plumeBuffer, AudioBuffer<float>& wrapperBuffer);

    //==============================================================================
    static AudioProcessor::BusesProperties createBusesPropertiesFromPluginInstance (AudioPluginInstance& plugin);

    //==============================================================================
    class WrappedParameter;
    
    //==============================================================================
    AudioPluginInstance& plugin;
    PluginWrapper& owner;
    AudioProcessorParameter::Listener& listener;
    AudioBuffer<float> wrapperBuffer;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WrapperProcessor)
};
