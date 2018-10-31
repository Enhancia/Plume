/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "Plugin/PluginProcessor.h"
#include "Plugin/PluginEditor.h"

#define TRACE_IN  Logger::writeToLog ("[+FNC] Entering: " + String(__FUNCTION__))
#define TRACE_OUT Logger::writeToLog ("[-FNC]  Leaving: " + String(__FUNCTION__))
//==============================================================================
PlumeProcessor::PlumeProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                       )
#endif
{   
    TRACE_IN;
    
    Time t;
    plumeLogger = FileLogger::createDefaultAppLogger ("Enhancia/Plume/Logs/",
                                                      "plumeLog.txt",
                                                      "Plume Log "+String(t.getYear())+String(t.getMonth())+String(t.getDayOfMonth()));

    Logger::setCurrentLogger (plumeLogger);
    
    dataReader = new DataReader();
    gestureArray = new GestureArray (*dataReader);
    wrapper = new PluginWrapper (*this, *gestureArray);
    dataReader->addChangeListener(gestureArray);
	
}

PlumeProcessor::~PlumeProcessor()
{
    TRACE_IN;
    dataReader->removeChangeListener(gestureArray);
    dataReader = nullptr;
    gestureArray = nullptr;
    wrapper = nullptr;
    
    Logger::setCurrentLogger (nullptr);
    plumeLogger = nullptr;
}

//==============================================================================
void PlumeProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    if (wrapper->isWrapping())
    {
        wrapper->getWrapperProcessor().prepareToPlay (sampleRate, samplesPerBlock);
    }
}

void PlumeProcessor::releaseResources()
{
    if (wrapper->isWrapping())
    {
        wrapper->getWrapperProcessor().releaseResources();
    }
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool PlumeProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void PlumeProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{   
    // if wrapped plugin, processes all MIDI / parameter values
    if (wrapper->isWrapping())
    {
        // Adds the gesture's MIDI messages to the buffer
        gestureArray->process (midiMessages);
        
        // Calls the wrapper processor's processBlock method
        wrapper->getWrapperProcessor().processBlock(buffer, midiMessages);
    }
}

//==============================================================================
PluginWrapper& PlumeProcessor::getWrapper()
{
    return *wrapper;
}

DataReader* PlumeProcessor::getDataReader()
{
    return dataReader;
}

GestureArray& PlumeProcessor::getGestureArray()
{
    return *gestureArray;
}

//==============================================================================
bool PlumeProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* PlumeProcessor::createEditor()
{
    return new PlumeEditor (*this);
}

//==============================================================================
void PlumeProcessor::getStateInformation (MemoryBlock& destData)
{
    TRACE_IN;
    ScopedPointer<XmlElement> wrapperData = new XmlElement ("PLUME");
    
    // Adds plugin and gestures data, and saves them in a binary file
    createPluginXml   (*wrapperData);
    createGestureXml (*wrapperData);
    
    // Creates the binary data
    copyXmlToBinary (*wrapperData, destData);
    
    wrapperData->deleteAllChildElements();
}

void PlumeProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    TRACE_IN;
    ScopedPointer<XmlElement> wrapperData = getXmlFromBinary (data, sizeInBytes);
    
	if (wrapperData == nullptr)
	{
		DBG ("Couldn't load data");
		return;
	}
    
	bool notifyEditor = false;
	
	// Plugin configuration loading
    if (wrapperData->getChildByName ("WRAPPED_PLUGIN") != nullptr)
    {
        loadPluginXml (*(wrapperData->getChildByName ("WRAPPED_PLUGIN")));
        notifyEditor = true;
    }
    
    // Gestures configuration loading
    if (wrapperData->getChildByName ("GESTURES") != nullptr)
    {
        sendActionMessage("blockInterface");
        loadGestureXml (*(wrapperData->getChildByName ("GESTURES")));
        notifyEditor = true;
    }
    
    
    // Sends a change message to the editor so it can update its interface.
    if (notifyEditor) sendActionMessage("updateInterface");

    wrapperData = nullptr;
}

void PlumeProcessor::createPluginXml(XmlElement& wrapperData)
{
    // Creates the child Xml and stores hasWrappedPlugin bool value
    ScopedPointer<XmlElement> pluginData = new XmlElement ("WRAPPED_PLUGIN");
    
    pluginData->setAttribute ("hasWrappedPlugin", wrapper->isWrapping());
    
    if (wrapper->isWrapping())
    {
	    {
	        // Saves the description of current wrapped plugin
	        PluginDescription pd;
	        wrapper->fillInPluginDescription (pd);
	        pluginData->addChildElement (pd.createXml());
	    }
	    
	    {
	        // Saves the state of current wrapped plugin
	        MemoryBlock m;
	        wrapper->getWrapperProcessor().getStateInformation (m);
	        pluginData->createNewChildElement ("STATE")->addTextElement (m.toBase64Encoding());
	    }
    }
    
    wrapperData.addChildElement (new XmlElement (*pluginData));
    
    pluginData = nullptr;
}

void PlumeProcessor::createGestureXml(XmlElement& wrapperData)
{
    ScopedPointer<XmlElement> gesturesData = new XmlElement ("GESTURES");
    
	gestureArray->createGestureXml(*gesturesData);
	wrapperData.addChildElement (new XmlElement (*gesturesData));
	
	gesturesData = nullptr;
}

void PlumeProcessor::loadPluginXml(const XmlElement& pluginData)
{
    // Checks if there was a wrapped plugin in the saved state
    if (pluginData.getBoolAttribute ("hasWrappedPlugin"))
    {
        // Loads the description of the plugin and wraps it
        PluginDescription pd;
            
        if (pd.loadFromXml (*(pluginData.getChildByName ("PLUGIN"))))
        {
            if (File (pd.fileOrIdentifier).exists())
            {
                if (wrapper->isWrapping()) wrapper->rewrapPlugin(pd.fileOrIdentifier);
                else                       wrapper->wrapPlugin(pd.fileOrIdentifier);
            }
            else
            {
                //WIP à faire vendredi recherche plugin """intelligente"""
            }
        }
    }

    // Restores the plugin to its saved state
    if (wrapper->isWrapping())
    {
	    if (auto state = pluginData.getChildByName("STATE"))
		{
			MemoryBlock m;
			m.fromBase64Encoding (state->getAllSubText());

			wrapper->getWrapperProcessor().setStateInformation (m.getData(), (int)m.getSize());
		}
    }
}

void PlumeProcessor::loadGestureXml(const XmlElement& gestureData)
{
    
    int i = 0;
    gestureArray->clearAllGestures();
    
    forEachXmlChildElement (gestureData, gesture)
    {
        gestureArray->addGestureFromXml(*gesture);
        
        if (gesture->getBoolAttribute ("mapped") == true)
        {
            wrapper->addParametersToGestureFromXml (*gesture, i);
        }
        
        i++;
    }
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PlumeProcessor();
}
