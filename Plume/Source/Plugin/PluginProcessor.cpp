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
       , parameters (*this, nullptr /*, PLUME::parametersIdentifier, {}*/)
{
    TRACE_IN;
    
    // Logger
    Time t;
    plumeLogger = FileLogger::createDefaultAppLogger ("Enhancia/Plume/Logs/",
                                                      "plumeLog.txt",
                                                      "Plume Log "+String(t.getYear())+String(t.getMonth())+String(t.getDayOfMonth()));
    
    Logger::setCurrentLogger (plumeLogger);
    
    // Parameters
    initializeParameters();
    parameters.replaceState (ValueTree (PLUME::plumeIdentifier));
    
    // Objects
    dataReader = new DataReader();
    gestureArray = new GestureArray (*dataReader, parameters);
    wrapper = new PluginWrapper (*this, *gestureArray);
    presetHandler = new PresetHandler();
    
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
    MidiBuffer plumeBuffer;
    /* 
    if (auto* pHead = getPlayHead())
    {
		AudioPlayHead::CurrentPositionInfo pos;

		if (pHead->getCurrentPosition(pos))
		{
			DBG ("Playing ? " << String(int(pos.isPlaying)));
			//Logger::writeToLog("Playing ? " + String(int(pos.isPlaying)));
		}
    }
    
    DBG ("\nInput Buses : " << getBusCount (true) << " | Output Buses : " << getBusCount (false));
    DBG ("Main input enabled ? " << int (!(getBusesLayout().getMainInputChannelSet().isDisabled())) << " | Main Output Enabled ? "
                                 << int (!(getBusesLayout().getMainOutputChannelSet().isDisabled())));
    
    
    {
		
        File abf = File (File::getSpecialLocation (File::userDesktopDirectory).getFullPathName() + "/AudioBuff.txt");
        if (!(abf.exists()))
		{
			abf.create();
			DBG ("Attempted to create file : " + abf.getFullPathName());
		}

		for (int i = 0; i < 4; i++)
		{
		    int sample = i * (buffer.getNumSamples() / 4);
			abf.appendText( String (sample) + " : " + String (buffer.getSample (1, sample)) + "\n");
		}
		

        File mbf = File (File::getSpecialLocation (File::userDesktopDirectory).getFullPathName() + "/MidiBuff.txt");
		if (!(mbf.exists()))
		{
			mbf.create();
			DBG ("Attempted to create file : " + mbf.getFullPathName());
		}

		if (!midiMessages.isEmpty())
        {
			mbf.appendText ("\nNew Buffer: size " + String (buffer.getNumSamples()) + "\n");

		    MidiMessage m; int time;
            for (MidiBuffer::Iterator i (midiMessages); i.getNextEvent (m, time);)
            {
                mbf.appendText (m.getDescription() + " | " + String(time) + "\n" );
			    
                //for (const uint8* p = m.getRawData(); p < p + m.getRawDataSize(); p++)
                //{
                //    mbf.appendText (String(*p));
                //}
                //mbf.appendText ("\n\n");
            }
            mbf.appendText ("\n");
        }
        else 
        {
            mbf.appendText ("\nEmpty Buffer\n");
        }
        
    }
    */
    
    // Adds the gesture's MIDI messages to the buffer, and changes parameters if needed
    gestureArray->process (midiMessages, plumeBuffer);
        
    // if wrapped plugin, lets the wrapped plugin process all MIDI into sound
    if (wrapper->isWrapping())
    {
        // Calls the wrapper processor's processBlock method
        if (!(wrapper->getWrapperProcessor().isSuspended()))
        {
            wrapper->getWrapperProcessor().processBlock(buffer, midiMessages);
        }
    }
    
    // returns only the midi from Plume
    midiMessages = plumeBuffer;
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

AudioProcessorValueTreeState& PlumeProcessor::getParameterTree()
{
    return parameters;
}


PresetHandler& PlumeProcessor::getPresetHandler()
{
    return *presetHandler;
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
    suspendProcessing (true);
    
    TRACE_IN;
    ScopedPointer<XmlElement> wrapperData = getXmlFromBinary (data, sizeInBytes);
    
	if (wrapperData == nullptr)
	{
		DBG ("Couldn't load data");
	    PLUME::UI::ANIMATE_UI_FLAG = true;
	    suspendProcessing (false);
		return;
	}
    
	bool notifyEditor = false;
	
	// Plugin configuration loading
    if (wrapperData->getChildByName ("WRAPPED_PLUGIN") != nullptr)
    {
		PLUME::UI::ANIMATE_UI_FLAG = false;
        loadPluginXml (*(wrapperData->getChildByName ("WRAPPED_PLUGIN")));
        notifyEditor = true;
    }
    
    // Gestures configuration loading
    if (wrapperData->getChildByName ("GESTURES") != nullptr)
    {
	    //sendActionMessage ("lockInterface");
	    PLUME::UI::ANIMATE_UI_FLAG = false;
        loadGestureXml (*(wrapperData->getChildByName ("GESTURES")));
        notifyEditor = true;
    }
    
    // Sends a change message to the editor so it can update its interface.
    if (notifyEditor) sendActionMessage ("updateInterface");
    else PLUME::UI::ANIMATE_UI_FLAG = true;

    wrapperData = nullptr;
    suspendProcessing (false);
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
            if (!(pd.fileOrIdentifier.isEmpty()))
            {
                if (wrapper->isWrapping()) wrapper->rewrapPlugin (pd);
                else                       wrapper->wrapPlugin (pd);
            }
			
            /*
            // First searches the direct path
            if (!(pd.fileOrIdentifier.isEmpty()))
            {
                if (wrapper->isWrapping()) requiresSearch = !(wrapper->rewrapPlugin (pd.fileOrIdentifier));
                else                       requiresSearch = !(wrapper->wrapPlugin (pd.fileOrIdentifier));
            }
            // Then the directory where plume is located
            if (requiresSearch && File (pd.fileOrIdentifier).exists())
            {
                File pluginDir (File::getSpecialLocation (File::currentApplicationFile).getParentDirectory());
                String pluginToSearch (pd.name);
                
                if (pd.pluginFormatName.compare ("VST") == 0)
                {
                  #if JUCE_WINDOWS
                    pluginToSearch += ".dll";
                  #elif JUCE_MAC
                    pluginToSearch += ".vst";
                  #endif
                }
                else if (pd.pluginFormatName.compare ("VST3") == 0)
                {
                    pluginToSearch += ".vst3";
                }
                else if (pd.pluginFormatName.compare ("AudioUnit") == 0)
                {
                    pluginToSearch += ".component";
                }
                else if (pd.pluginFormatName.compare ("Aax") == 0)
                {
                    pluginToSearch += ".aax";
                }
                
                Array<File> searchResult = pluginDir.findChildFiles (File::findFiles + File::ignoreHiddenFiles, true, pluginToSearch);
                
                if (!searchResult.isEmpty())
                {
                    if (wrapper->isWrapping()) wrapper->rewrapPlugin (searchResult[0].getFullPathName());
                    else                       wrapper->wrapPlugin (searchResult[0].getFullPathName());
                }
                else 
                {
                    DBG ("Failed to find the plugin in Plume's directory.");
                    if (wrapper->isWrapping()) wrapper->unwrapPlugin();
                    return;
                }
                
            }
			*/
            
        }
    }
    // If there is no plugin in the preset only unwraps
    else
    {
        wrapper->unwrapPlugin();
        return;
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
		if (i < PLUME::NUM_GEST)
		{
			gestureArray->addGestureFromXml(*gesture);

			if (gesture->getBoolAttribute ("mapped") == true)
			{
				gestureArray->getGestureById(i)->clearAllParameters();
				wrapper->addParametersToGestureFromXml(*gesture, i);
			}

			i++;
		}
    }
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PlumeProcessor();
}


//==============================================================================
void PlumeProcessor::initializeParameters()
{
    using namespace PLUME::param;
            
    for (int gest =0; gest < PLUME::NUM_GEST; gest++)
    {
        for (int i =0; i < numParams; i++)
        {
            // boolean parameters
            if (i == on || i == midi_on)
            {
                parameters.createAndAddParameter (std::make_unique<AudioParameterBool> (String(gest) + paramIds[i],
                                                                                        String(gest) + paramIds[i],
                                                                                        false));
            }
            // float parameters
            else
            {
                NormalisableRange<float> range;
                float defVal;
                switch (i)
                {
                    case vibrato_range:
                        range = NormalisableRange<float> (0.0f, 500.0f, 1.0f);
                        defVal = 400.0f;
                        break;
			        case vibrato_thresh:
                        range = NormalisableRange<float> (0.0f, 300.0f, 1.0f);
                        defVal = 40.0f;
                        break;
                    case vibrato_intensity:
                        range = NormalisableRange<float> (0.0f, 1000.0f, 1.0f);
                        defVal = 0.0f;
                        break;
			        case bend_leftLow:
                        range = NormalisableRange<float> (-90.0f, 0.0f, 1.0f);
                        defVal = -50.0f;
                        break;
			        case bend_leftHigh:
                        range = NormalisableRange<float> (-90.0f, 0.0f, 1.0f);
                        defVal = -20.0f;
                        break;
			        case bend_rightLow:
                        range = NormalisableRange<float> (0.0f, 90.0f, 1.0f);
                        defVal = 30.0f;
                        break;
			        case bend_rightHigh:
                        range = NormalisableRange<float> (0.0f, 90.0f, 1.0f);
                        defVal = 60.0f;
                        break;
			        case roll_low:
			        case tilt_low:
                        range = NormalisableRange<float> (-90.0f, 90.0f, 1.0f);
                        defVal = 0.0f;
                        break;
			        case roll_high:
			        case tilt_high:
                        range = NormalisableRange<float> (-90.0f, 90.0f, 1.0f);
                        defVal = 50.0f;
                        break;
					case midi_cc:
						range = NormalisableRange<float>(0.0f, 127.0f, 1.0f);
						defVal = 1.0f;
						break;
			        default:
                        range = NormalisableRange<float> (0.0f, 1.0f, 0.001f);
                        break;
                }
				
                parameters.createAndAddParameter (std::make_unique<AudioParameterFloat> (String(gest) + paramIds[i],
                                                                                         String(gest) + paramIds[i],
                                                                                         range,
                                                                                         defVal));
            }
        }
    }
}

void PlumeProcessor::updateTrackProperties (const AudioProcessor::TrackProperties& properties)
{
    DBG ("Name : " << properties.name << " | Colour : " << properties.colour.toDisplayString(false));
}