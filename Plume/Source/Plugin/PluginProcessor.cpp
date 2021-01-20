/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

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
                                                      "Plume Log | Host application : "
                                                      + File::getSpecialLocation (File::hostApplicationPath)
                                                            .getFullPathName()
                                                      + " | OS :"
                                                      #if JUCE_MAC
                                                        " MAC "
                                                      #elif JUCE_WINDOWS
                                                        " Windows "
                                                      #endif
                                                      + " | Plume v" + JucePlugin_VersionString + " \n");
    
    Logger::setCurrentLogger (plumeLogger);
    
    // Parameters
    initializeParameters();
    initializeSettings();
    initializeMidiSequence();
    
    // Objects
    dataReader = new DataReader();
    gestureArray = new GestureArray (*dataReader, parameters);
    wrapper = new PluginWrapper (*this, *gestureArray, parameters.state.getChildWithName(PLUME::treeId::general)
		                                                         .getChildWithName(PLUME::treeId::pluginDirs));
    presetHandler = new PresetHandler (parameters.state.getChildWithName (PLUME::treeId::general)
		                                               .getChildWithName (PLUME::treeId::presetDir));
    
    dataReader->addChangeListener (gestureArray);

}

PlumeProcessor::~PlumeProcessor()
{
    TRACE_IN;
    dataReader->removeChangeListener(gestureArray);
    dataReader->connectionLost();
    dataReader = nullptr;
    
    gestureArray = nullptr;
    wrapper = nullptr;

    removeLogger();
}

void PlumeProcessor::removeLogger()
{
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

    checkForSignedMidi (midiMessages);
    if (isProbablyOnAnArmedTrack())
    {
        // Adds the gesture's MIDI messages to the buffer, and changes parameters if needed
        gestureArray->process (midiMessages, plumeBuffer);
    }
        
    // if wrapped plugin, lets the wrapped plugin process all MIDI into sound
    if (wrapper->isWrapping())
    {
        // Wrapper uses playhead from the DAW
        wrapper->getWrapperProcessor().setPlayHead (getPlayHead());
        
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
    std::unique_ptr<XmlElement> wrapperData (new XmlElement ("PLUME"));
    
    // Adds plugin and gestures data, and saves them in a binary file
    createGeneralXml (*wrapperData);
    createPluginXml  (*wrapperData);
    createGestureXml (*wrapperData);
    
    // Creates the binary data
    copyXmlToBinary (*wrapperData, destData);
    wrapperData->deleteAllChildElements();
}

void PlumeProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    suspendProcessing (true);
    
    TRACE_IN;
	std::unique_ptr<XmlElement> wrapperData = getXmlFromBinary (data, sizeInBytes);
    
	if (wrapperData == nullptr)
	{
		DBG ("Couldn't load data");
	    PLUME::UI::ANIMATE_UI_FLAG = true;
	    suspendProcessing (false);
		return;
	}
    
	bool notifyEditor = false;
	
	// Plugin configuration loading
    if (wrapperData->getChildByName ("GENERAL") != nullptr)
    {
		PLUME::UI::ANIMATE_UI_FLAG = false;
        parameters.replaceState (ValueTree::fromXml (*wrapperData->getChildByName ("PLUME")));
        notifyEditor = true;
    }
    
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

void PlumeProcessor::createGeneralXml(XmlElement& wrapperData)
{
	wrapperData.addChildElement (new XmlElement (*parameters.state.createXml()));
}

void PlumeProcessor::createPluginXml(XmlElement& wrapperData)
{
    // Creates the child Xml and stores hasWrappedPlugin bool value
    std::unique_ptr<XmlElement> pluginData (new XmlElement ("WRAPPED_PLUGIN"));
    
    pluginData->setAttribute ("hasWrappedPlugin", wrapper->isWrapping());
    
    if (wrapper->isWrapping())
    {
	    {
	        // Saves the description of current wrapped plugin
	        PluginDescription pd;
	        wrapper->fillInPluginDescription (pd);
	        pluginData->addChildElement (new XmlElement (*pd.createXml()));
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
    std::unique_ptr<XmlElement> gesturesData (new XmlElement ("GESTURES"));
    
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
				//gestureArray->getGesture (i)->clearAllParameters();
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
            if (i == on || i == midi_on || i == midi_reverse)
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
                        range = NormalisableRange<float> (0.0f, PLUME::gesture::VIBRATO_RANGE_MAX, 1.0f);
                        defVal = PLUME::gesture::VIBRATO_RANGE_DEFAULT;
                        break;
			        case vibrato_thresh:
                        range = NormalisableRange<float> (0.0f, 500.0f, 1.0f);
                        defVal = PLUME::gesture::VIBRATO_THRESH_DEFAULT;
                        break;
                    case vibrato_intensity:
                        range = NormalisableRange<float> (0.0f, 1000.0f, 1.0f);
                        defVal = 0.0f;
                        break;
			        case bend_leftLow:
                        range = NormalisableRange<float> (PLUME::UI::PITCHBEND_DISPLAY_MIN, PLUME::UI::PITCHBEND_DISPLAY_MAX, 1.0f);
                        defVal = PLUME::gesture::PITCHBEND_DEFAULT_LEFTMIN;
                        break;
			        case bend_leftHigh:
                        range = NormalisableRange<float> (PLUME::UI::PITCHBEND_DISPLAY_MIN, PLUME::UI::PITCHBEND_DISPLAY_MAX, 1.0f);
                        defVal = PLUME::gesture::PITCHBEND_DEFAULT_LEFTMAX;
                        break;
			        case bend_rightLow:
                        range = NormalisableRange<float> (PLUME::UI::PITCHBEND_DISPLAY_MIN, PLUME::UI::PITCHBEND_DISPLAY_MAX, 1.0f);
                        defVal = PLUME::gesture::PITCHBEND_DEFAULT_RIGHTMIN;
                        break;
			        case bend_rightHigh:
                        range = NormalisableRange<float> (PLUME::UI::PITCHBEND_DISPLAY_MIN, PLUME::UI::PITCHBEND_DISPLAY_MAX, 1.0f);
                        defVal = PLUME::gesture::PITCHBEND_DEFAULT_RIGHTMAX;
                        break;
			        case roll_low:
                        range = NormalisableRange<float> (PLUME::UI::ROLL_DISPLAY_MIN, PLUME::UI::ROLL_DISPLAY_MAX, 1.0f);
                        defVal = PLUME::gesture::ROLL_DEFAULT_MIN;
                        break;
			        case roll_high:
                        range = NormalisableRange<float> (PLUME::UI::ROLL_DISPLAY_MIN, PLUME::UI::ROLL_DISPLAY_MAX, 1.0f);
                        defVal = PLUME::gesture::ROLL_DEFAULT_MAX;
                        break;
			        case tilt_low:
                        range = NormalisableRange<float> (PLUME::UI::TILT_DISPLAY_MIN, PLUME::UI::TILT_DISPLAY_MAX, 1.0f);
                        defVal = PLUME::gesture::TILT_DEFAULT_MIN;
                        break;
			        case tilt_high:
                        range = NormalisableRange<float> (PLUME::UI::TILT_DISPLAY_MIN, PLUME::UI::TILT_DISPLAY_MAX, 1.0f);
                        defVal = PLUME::gesture::TILT_DEFAULT_MAX;
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
                /*
                if (i != (int) PLUME::param::value && i != (int) vibrato_intensity)
                {
                    parameters.addParameterListener (String(gest) + paramIds[i], this);
                }*/
            }
        }
    }
}

void PlumeProcessor::initializeSettings()
{
    using namespace PLUME::treeId;
    parameters.replaceState (ValueTree (plume));
    parameters.state.addChild (ValueTree (general), 0, nullptr);
    
	auto generalTree = parameters.state.getChildWithName (general);
    generalTree.addChild (ValueTree (winW).setProperty (value, var (PLUME::UI::DEFAULT_WINDOW_WIDTH), nullptr), 0, nullptr);
    generalTree.addChild (ValueTree (winH).setProperty (value, var (PLUME::UI::DEFAULT_WINDOW_HEIGHT), nullptr), 1, nullptr);

  #if JUCE_WINDOWS
    generalTree.addChild (ValueTree (presetDir).setProperty (value,
                                                             File::getSpecialLocation (File::userDocumentsDirectory)
                                                                .getFullPathName()
                                                                + "\\Enhancia\\Plume\\Presets\\User\\",
                                                             nullptr), 2, nullptr);
  #elif JUCE_MAC
    generalTree.addChild (ValueTree (presetDir).setProperty (value,
                                                             File::getSpecialLocation (File::userApplicationDataDirectory)
                                                                .getFullPathName()
                                                                + "\\Enhancia\\Plume\\Presets\\User\\",
                                                             nullptr), 2, nullptr);
  #endif
    
    generalTree.addChild (ValueTree (pluginDirs), 3, nullptr);
    generalTree.getChild (3).addChild (ValueTree (directory).setProperty (value, "", nullptr),
                                       0, nullptr);
}


void PlumeProcessor::parameterChanged (const String &parameterID, float newValue)
{
    /*
    int paramId = parameterID.upToFirstOccurrenceOf ("_", false, false).getIntValue();
    String gestType = parameterID.fromFirstOccurrenceOf ("_", false, false)
                                 .upToLastOccurrenceOf ("_", false, false);
    String paramType = parameterID.fromLastOccurrenceOf ("_", false, false);

    DBG ("Parameter changed : " << paramId << " " << gestType << " " << paramType);

    if (paramType.compare (low) )
    {

    }
    */
}

void PlumeProcessor::updateTrackProperties (const AudioProcessor::TrackProperties& properties)
{
	ignoreUnused (properties);
    DBG ("Name : " << properties.name << " | Colour : " << properties.colour.toDisplayString(false));
}

void PlumeProcessor::checkForSignedMidi (MidiBuffer& midiMessages)
{
    if (signedMidiBufferCount > 0) signedMidiBufferCount--;
    else if (lastSimultaneousSequenceCount != 0) lastSimultaneousSequenceCount = 0;

    if (!midiMessages.isEmpty())
    {
        for (const MidiMessageMetadata metadata : midiMessages)
        {
            checkMidiAndUpdateMidiSequence (metadata.getMessage());
        }
    }
}

bool PlumeProcessor::isProbablyOnAnArmedTrack()
{
    if (signedMidiBufferCount == 0) return false;
    
    if (auto* playHead = getPlayHead())
    {
        AudioPlayHead::CurrentPositionInfo positionInfo;

        if (playHead->getCurrentPosition (positionInfo))
        {
            if (positionInfo.isPlaying && !positionInfo.isRecording)
            {
                /*  If DAW is playing back some pre recorded midi, we want plume to activate its gestures only if it is armed.
                    When the DAW plays but Plume only recieves one instance of the signed MIDI, it likely
                    indicates that the track is inactive with some previously recorded signed MIDI playing. */
                return (lastSimultaneousSequenceCount != 1);
            }
        }
    }
    
    // Either playhead is not playing, or Plume failed to get playhead info
    // If the latter is true it is safe to assume assume the host simply cannot playback or is atleast not playing atm
    return (lastSimultaneousSequenceCount != 0);
}


void PlumeProcessor::initializeMidiSequence()
{
    // Adds aftertouch messages
    signedMidiSequence.add (new MidiMessage (MidiMessage::channelPressureChange (16, 125)));
    signedMidiSequence.add (new MidiMessage (MidiMessage::channelPressureChange (16, 126)));
    signedMidiSequence.add (new MidiMessage (MidiMessage::channelPressureChange (16, 127)));
}
void PlumeProcessor::checkMidiAndUpdateMidiSequence (const MidiMessage& midiMessageToCheck)
{
    // Checks if new message comes after the current
    if (isFromMidiSequence (midiMessageToCheck))
    {
        signedMidiBufferCount = int (std::trunc (getSampleRate()/(signedMidiFrequencyHz * getBlockSize()))) + 2;
        lastSimultaneousSequenceCount = isNextStepInSequence (midiMessageToCheck) ? 1 : 2;
        lastSignedMidiId = getIdInSequence (midiMessageToCheck);
    }
}

const bool PlumeProcessor::isFromMidiSequence (const MidiMessage& midiMessageToCheck)
{
    if (midiMessageToCheck.isChannelPressure())
    {
        // Checks every message from sequ to see if message is there
        for (auto* message : signedMidiSequence)
        {
            if (midiMessageToCheck.getChannelPressureValue() == message->getChannelPressureValue())
            {
                return true;
            }
        }
    }

    return false;
}
const bool PlumeProcessor::isNextStepInSequence (const MidiMessage& midiMessageToCheck)
{
    if (midiMessageToCheck.isChannelPressure())
    {
        if (lastSignedMidiId == 0) return true;

        if (lastSignedMidiId == signedMidiSequence.size() - 1) return (getIdInSequence(midiMessageToCheck) == 0);
        else                                                   return (getIdInSequence(midiMessageToCheck) == lastSignedMidiId + 1);
    }

    // Checks if message actually is next in the sequence
    return false;
}

int PlumeProcessor::getIdInSequence (const MidiMessage& midiMessageToCheck)
{
    if (midiMessageToCheck.isChannelPressure())
    {
        // Checks every message from sequ to see if message is there
        for (int messageId=0; messageId < signedMidiSequence.size(); messageId++)
        {
            if (midiMessageToCheck.getChannelPressureValue() == signedMidiSequence[messageId]->getChannelPressureValue())
            {
                return messageId;
            }
        }
    }

    return -1;
}