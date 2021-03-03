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
       , parameters (*this, nullptr, "PARAMETERS", initializeParameters())
{
    TRACE_IN;
    
    // Logger
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
    initializeMidiSequences();
    
    // Objects
    dataReader = new DataReader();
    gestureArray = new GestureArray (*dataReader, parameters, getLastArmRef());
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

    checkAndUpdateRecordingStatus();

    filterInputMidi (midiMessages);
    checkForSignedMidi (midiMessages);
    
    // Adds the gesture's MIDI messages to the buffer, and changes parameters if needed
    int armValue = parameters.getParameter ("track_arm")
                             ->convertFrom0to1 (parameters.getParameter ("track_arm")
                                                          ->getValue());
    lastArm = armValue == int (PLUME::param::armed) ||
             (armValue == int (PLUME::param::unknownArm) && isProbablyOnAnArmedTrack());

    // Adds the gesture's MIDI messages to the buffer, and changes parameters if needed
    gestureArray->process (midiMessages, plumeBuffer);
        
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
AudioProcessorValueTreeState::ParameterLayout PlumeProcessor::initializeParameters()
{
    using namespace PLUME::param;
    AudioProcessorValueTreeState::ParameterLayout layout;
            
    for (int gest =0; gest < PLUME::NUM_GEST; gest++)
    {
        for (int i =0; i < numParams; i++)
        {
                String description = "Gest " + String(gest + 1)
                                             + (i < gesture_param_0 ? " - Val " + String (i + 1)
                                                                    : " - Param " + String (i - gesture_param_0 + 1));
                layout.add (std::make_unique<AudioParameterFloat> (String(gest) + paramIds[i],
                                                                   description,
                                                                   NormalisableRange<float> (0.0f, 1.0, 0.0001f),
                                                                   0.0f));
        }
    }

    layout.add (std::make_unique<AudioParameterInt> ("track_arm",
                                                     "track_arm",
                                                     0, 2, 2));

    return layout;
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
    else if (lastSequenceType != noSequence) lastSequenceType = noSequence;

    if (!midiMessages.isEmpty())
    {
        for (const MidiMessageMetadata metadata : midiMessages)
        {
            checkMidiAndUpdateMidiSequence (metadata.getMessage());
        }
    }
}

void PlumeProcessor::checkAndUpdateRecordingStatus()
{
    bool isRecording = false;

    if (auto* playHead = getPlayHead())
    {
        AudioPlayHead::CurrentPositionInfo positionInfo;

        if (playHead->getCurrentPosition (positionInfo))
        {
            // TODO change isPlaying to isRecording
            isRecording = positionInfo.isRecording;

            /*//TO DELETE
            DBG("BPM            : " << String(positionInfo.bpm));
            DBG("Time (s)       : " << String(positionInfo.timeInSeconds));
            DBG("Playing        : " << (positionInfo.isPlaying ? "Y" : "N"));
            DBG("Recording      : " << (positionInfo.isRecording ? "Y" : "N"));
            DBG("Ppq Position   : " << String(positionInfo.ppqPosition));
            DBG("Time (samples) : " << String(positionInfo.timeInSamples));
            DBG("Time sig denom : " << String(positionInfo.timeSigDenominator));
            DBG("Time sig num   : " << String(positionInfo.timeSigNumerator));
            */
        }
    }

    if (lastRecordingStatus != isRecording)
    {
        lastRecordingStatus = isRecording;

        // TODO send recording status change to HUB
        // This will allow the HUB to send the right MIDI sequence
                //test change aftertouch seq sur enregistrement
        if (isRecording)
        {
            memcpy(data, "reco", sizeof("reco"));
            dataReader->sendString(data, 4);
        }
        else 
        {
            memcpy(data, "play", sizeof("play"));
            dataReader->sendString(data, 4);
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
                //  When the DAW is playing, we want plume to activate its midi only if it receives the normal sequence.
                return (lastSequenceType == normal ||
                        lastSequenceType == alternatingNormal ||
                        lastSequenceType == alternatingRecording ||
                        lastSequenceType == normalAndRecording);
            }

            else if (positionInfo.isRecording)
            {
                // If the DAW is recording, Plume's MIDI should activate only if it recieves thd recording sequence
                return (lastSequenceType == recording); 
            }
        }
    }
    // Either playhead is not playing, or Plume failed to get playhead info
    // If the latter is true it is safe to assume assume the host simply cannot playback or is atleast not playing atm
    // In this case, Plume should activate only of it recieves the normal midi sequence
    return (lastSequenceType == normal);
}

void PlumeProcessor::initializeMidiSequences()
{
    for (int value = 0; value < 6; value++)
    {
        if (value < 3) normalMidiSequence.add (new MidiMessage (MidiMessage::channelPressureChange (1, value)));
        else           recordingMidiSequence.add (new MidiMessage (MidiMessage::channelPressureChange (1, value)));
    }
}
void PlumeProcessor::checkMidiAndUpdateMidiSequence (const MidiMessage& midiMessageToCheck)
{
    // Checks if new message should be considered at all
    if (isFromMidiSequence (midiMessageToCheck, normalAndRecording))
    {
        signedMidiBufferCount = int (std::trunc (getSampleRate()/(signedMidiFrequencyHz * getBlockSize()))) + 2;

        if (lastSequenceType == noSequence)
        {
            /*  We just got a fresh sequence, meaning either:
                - Plume was just launched
                - Neova was just connected
                - Neova is NOT connected and DAW just started playing MIDI that was recorded with Neova
            */
            lastSequenceType = isFromMidiSequence (midiMessageToCheck, normal) ? normal : recording;
            lastSignedMidi = {getIdInSequence (midiMessageToCheck, normal),
                              getIdInSequence (midiMessageToCheck, recording)};
        }
        else if (lastSequenceType != normalAndRecording &&
                 lastSequenceType != alternatingNormal &&
                 lastSequenceType != alternatingRecording &&
                 isFromMidiSequence (midiMessageToCheck, lastSequenceType))
        {
            // One sequence, either normal or recording
            if (isNextStepInSequence (midiMessageToCheck, lastSequenceType)) 
            {
                if (lastSequenceType == normal)
                    lastSignedMidi.normalSequenceId = getIdInSequence (midiMessageToCheck, lastSequenceType);

                else if (lastSequenceType == recording)
                    lastSignedMidi.recordingSequenceId = getIdInSequence (midiMessageToCheck, lastSequenceType);
            }
            else 
            {
                /*  TO DELETE (else section is for DBG)
                    Being here means that Plume recieved a message from the right sequence, but not the excepted message...
                    This could mean either:
                        - This message is from Neova, but one or several messages were skipped due to transmission errors
                        - This message is NOT from Neova and should not be considered signed

                    Plume will just discard the message. If the sequence was offset due to a transmission error,
                    the buffer count will naturally drop to 0. Plume will then catch up to the sequence where it should.
                */
                //jassert (false);
            }
        }
        else
        {
            // Two sequences at the same time
            const midiSequenceId newMessageSequenceId = isFromMidiSequence (midiMessageToCheck, normal) ? normal : recording;

            // Still aleternating
            if (((lastSequenceType == alternatingNormal ||
                  lastSequenceType == normal) && newMessageSequenceId == recording) ||
                ((lastSequenceType == alternatingRecording ||
                  lastSequenceType == recording) && newMessageSequenceId == normal))
            {
                // if last midi is strictly in the different sequence, starts or keeps alternating sequences
                if (lastSequenceType == normal || lastSequenceType == recording ||
                    isNextStepInSequence (midiMessageToCheck, lastSequenceType))
                {
                    if (newMessageSequenceId == normal)
                    {
                        lastSequenceType = alternatingNormal;
                        lastSignedMidi.normalSequenceId = getIdInSequence (midiMessageToCheck, newMessageSequenceId);
                    }
                    else if (newMessageSequenceId == recording)
                    {
                        lastSequenceType = alternatingRecording;
                        lastSignedMidi.recordingSequenceId = getIdInSequence (midiMessageToCheck, newMessageSequenceId);
                    }
                }
            }

            // Back to one sequence
            else
            {
                lastSequenceType = newMessageSequenceId;

                if (lastSequenceType == normal)
                    lastSignedMidi.normalSequenceId = getIdInSequence (midiMessageToCheck, lastSequenceType);

                else if (lastSequenceType == recording)
                    lastSignedMidi.recordingSequenceId = getIdInSequence (midiMessageToCheck, lastSequenceType);
            }
        }

        //TO DELETE
        /*DBG ("New MIDI sequence status :\n" <<
             "Sequence      : " << sequenceTypeToString (lastSequenceType) << "\n" <<
             "Buffer Count  : " << int (signedMidiBufferCount) << "\n" <<
             "Last Midi IDs : Normal " << lastSignedMidi.normalSequenceId <<
             " | Recording " << lastSignedMidi.recordingSequenceId <<
             "\n======================================\n\n\n\n");*/
    }
}

const bool PlumeProcessor::isFromMidiSequence (const MidiMessage& midiMessageToCheck, const midiSequenceId sequenceType)
{
    if (midiMessageToCheck.isChannelPressure())
    {
        Array<MidiMessage*> signedMidiSequenceToSearch;

        switch (sequenceType)
        {
            case normal:
                signedMidiSequenceToSearch.addArray (normalMidiSequence);
                break;
            case recording:
                signedMidiSequenceToSearch.addArray (recordingMidiSequence);
                break;
            case alternatingNormal:
            case alternatingRecording:
            case normalAndRecording:
                signedMidiSequenceToSearch.addArray (normalMidiSequence);
                signedMidiSequenceToSearch.addArray (recordingMidiSequence);
                break;
            default:
                break;
        }

        // Checks every message from sequ to see if message is there
        for (auto* message : signedMidiSequenceToSearch)
        {
            if (midiMessageToCheck.getChannelPressureValue() == message->getChannelPressureValue())
            {
                return true;
            }
        }
    }

    return false;
}
const bool PlumeProcessor::isNextStepInSequence (const MidiMessage& midiMessageToCheck, const midiSequenceId sequenceType)
{
    if (midiMessageToCheck.isChannelPressure())
    {
        midiSequenceId sequenceToSearch = sequenceType;
        
        if (sequenceToSearch == alternatingNormal) sequenceToSearch = recording;
        else if (sequenceToSearch == alternatingRecording) sequenceToSearch = normal;
        
        int signedMidiSequenceSize = (sequenceToSearch == normal)
                                        ? normalMidiSequence.size()
                                        : recordingMidiSequence.size();

        const int lastSignedMidiId = (sequenceToSearch == normal)
                                        ? lastSignedMidi.normalSequenceId
                                        : lastSignedMidi.recordingSequenceId;

        if (lastSignedMidiId == 0) return true;

        if (lastSignedMidiId == signedMidiSequenceSize - 1)
        {
            return (getIdInSequence(midiMessageToCheck, sequenceToSearch) == 0);
        }
        else
        {
            return (getIdInSequence(midiMessageToCheck, sequenceToSearch) == lastSignedMidiId + 1);
        }                                                
    }

    return false;
}

int PlumeProcessor::getIdInSequence (const MidiMessage& midiMessageToCheck, const midiSequenceId sequenceType)
{
    if (midiMessageToCheck.isChannelPressure())
    {
        Array<MidiMessage*> signedMidiSequenceToSearch;
        
        switch (sequenceType)
        {
            case noSequence:
                return -1;
            case normal:
                signedMidiSequenceToSearch.addArray (normalMidiSequence);
                break;
            case recording:
                signedMidiSequenceToSearch.addArray (recordingMidiSequence);
                break;
            case alternatingNormal:
            case alternatingRecording:
            case normalAndRecording:
                signedMidiSequenceToSearch.addArray (normalMidiSequence);
                signedMidiSequenceToSearch.addArray (recordingMidiSequence);
                break;
            default:
                break;
        }

        // Checks every message from sequ to see if message is there
        for (int messageId=0; messageId < signedMidiSequenceToSearch.size(); messageId++)
        {
            if (midiMessageToCheck.getChannelPressureValue() == signedMidiSequenceToSearch[messageId]->getChannelPressureValue())
            {
                //DBG ("Message : " << midiMessageToCheck.getDescription() <<
                //     " ID n# " << messageId << " in sequ " << sequenceTypeToString (sequenceType));
                return messageId;
            }
        }
    }

    return -1;
}


String PlumeProcessor::sequenceTypeToString (const midiSequenceId sequenceType) // TO DELETE
{
    switch (sequenceType)
    {
        case normal:
            return "Normal";
        case recording:
            return "Recording";
        case alternatingNormal:
            return "Alternating Normal";
        case alternatingRecording:
            return "Alternating Recording";
        case normalAndRecording:
            return "Normal And Recording";
        default:
            break;
    }

    return "No Sequence";
}

void PlumeProcessor::filterInputMidi (MidiBuffer& midiMessages)
{
    if (!midiMessages.isEmpty())
    {
        MidiBuffer filteredBuffer;
        
        for (const MidiMessageMetadata metadata : midiMessages)
        {
            if (messageShouldBeKept (metadata.getMessage()))
            {
                filteredBuffer.addEvent (metadata.getMessage(), metadata.samplePosition);
            }
        }

        midiMessages.swapWith (filteredBuffer);
    }
}

bool PlumeProcessor::messageShouldBeKept (const MidiMessage& midiMessage)
{
    if (midiMessage.isPitchWheel() ||
        (midiMessage.isController() && midiMessage.getControllerNumber() < 120))
    {
        return false;
    }

    return true;
}

bool& PlumeProcessor::getLastArmRef()
{
    return lastArm;
}