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
     : AudioProcessor (BusesProperties()
                       //.withInput ("Main Inout", AudioChannelSet::stereo(), false)
                       .withOutput ("Main Output", AudioChannelSet::stereo(), true)
                       )
       , parameters (*this, nullptr, "PARAMETERS", initializeParameters())
{
    PluginHostType pluginHostType;
    DBG ("Plugin host type : " << pluginHostType.type << " | Host Path : " << pluginHostType.getHostPath() << " | Plugin Type : " << pluginHostType.getPluginLoadedAs());

    // Logger
    plumeLogger.reset (FileLogger::createDefaultAppLogger (
                                                      #if JUCE_MAC
                                                        "Enhancia/Plume/",
                                                      #elif JUCE_WINDOWS
                                                        "Enhancia/Plume/Logs/",
                                                      #endif
                                                      "plumeLog.txt",
                                                      "[Plume Log Entry]"
                                                      "\n | Host application : id=" + String (pluginHostType.type)
                                                      + " path=" + pluginHostType.getHostPath()
                                                      + "\n | OS : " + SystemStats::getOperatingSystemName()
                                                      + "\n | Plume v" + JucePlugin_VersionString
                                                      + " (formatId=" + String (pluginHostType.getPluginLoadedAs()) + ")\n"));
    
    Logger::setCurrentLogger (plumeLogger.get());
    
    PLUME::nbInstance = PLUME::nbInstance + 1;
    
    // Parameters
    initializeParameters();
    initializeSettings();
    initializeParamSequences();
    
    // Objects
    dataReader.reset (new DataReader());
    gestureArray.reset (new GestureArray (*dataReader, parameters, getLastArmRef()));
    wrapper.reset (new PluginWrapper (*this, *gestureArray, parameters.state.getChildWithName(PLUME::treeId::general)
		                                                         .getChildWithName(PLUME::treeId::pluginDirs)));
    presetHandler.reset (new PresetHandler (parameters.state.getChildWithName (PLUME::treeId::general)
		                                               .getChildWithName (PLUME::treeId::presetDir)));
    updater.reset (new PlumeUpdater());
    
    dataReader->addChangeListener (gestureArray.get());

    detectPlumeCrashFromPreviousSession();
}

PlumeProcessor::~PlumeProcessor()
{
    PLUME::log::writeToLog ("Removing Plume instance.", PLUME::log::general);

    dataReader->removeChangeListener(gestureArray.get());
    dataReader->connectionLost();
    dataReader = nullptr;
    
    gestureArray = nullptr;
    wrapper = nullptr;
    updater = nullptr;
    presetHandler = nullptr;

    removeLogger();
    
    if(PLUME::nbInstance>0)
    {
        PLUME::nbInstance = PLUME::nbInstance - 1;
    }

  #if JUCE_MAC
    //MessageManager::getInstance()->runDispatchLoopUntil (1000);
  #endif
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

bool PlumeProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (wrapper->isWrapping())
    {
        return wrapper->getWrapperProcessor().isBusesLayoutSupported (layouts);
    }
    
    return (layouts.getMainOutputChannelSet() == AudioChannelSet::mono()
                || layouts.getMainOutputChannelSet() == AudioChannelSet::stereo());
}

void PlumeProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{   
    MidiBuffer plumeBuffer;

    checkAndUpdateRecordingStatus();

    filterInputMidi (midiMessages);
    
    //checkForSignedMidi (midiMessages);
    
    // Adds the gesture's MIDI messages to the buffer, and changes parameters if needed
    int armValue = parameters.getParameter ("track_arm")
                             ->convertFrom0to1 (parameters.getParameter ("track_arm")
                                                          ->getValue());
    lastArm = (armValue == int (PLUME::param::armed));

    // Adds the gesture's MIDI messages to the buffer, and changes parameters if needed
    if (!isDetectingAuthSequence)
        gestureArray->process (midiMessages, plumeBuffer);
    
    // if wrapped plugin, lets the wrapped plugin process all MIDI into sound
    if (wrapper->isWrapping() && !presetIsLocked)
    {
        // Wrapper uses playhead from the DAW
        wrapper->getWrapperProcessor().setPlayHead (getPlayHead());
        
        // Calls the wrapper processor's processBlock method
        if (!(wrapper->getWrapperProcessor().isSuspended()))
        {
            wrapper->getWrapperProcessor().processBlock (buffer, midiMessages);
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
    return dataReader.get();
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

PlumeUpdater& PlumeProcessor::getUpdater()
{
    return *updater;
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

            if (presetHandler->currentPresetRequiresAuth()) startDetectingAuthSequence();
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
    
    for (int i =0; i < numValues; i++)
    {
        layout.add (std::make_unique<AudioParameterFloat> (valuesIds[i],
                                                           valuesIds[i],
                                                           NormalisableRange<float> (0.0f, 1.0f, 0.0001f),
                                                           0.0f));
    }

    for (int gest =0; gest < PLUME::NUM_GEST; gest++)
    {
        for (int i =0; i < numParams; i++)
        {
                String description = "Gest " + String(gest + 1)
                                             + " - Param " + String (i - gesture_param_0 + 1);
                layout.add (std::make_unique<AudioParameterFloat> (String(gest) + paramIds[i],
                                                                   description,
                                                                   NormalisableRange<float> (0.0f, 1.0f, 0.0001f),
                                                                   0.0f));
        }
    }

    layout.add (std::make_unique<AudioParameterInt> ("track_arm",
                                                     "track_arm",
                                                     0, 1, 1));

    return layout;
}

void PlumeProcessor::initializeSettings()
{
    using namespace PLUME::treeId;
    parameters.replaceState (ValueTree (plume));
    parameters.state.addChild (ValueTree (general), 0, nullptr);
    
	auto generalTree = parameters.state.getChildWithName (general);
    generalTree.addChild (ValueTree (winX).setProperty (value, var (-1), nullptr), 0, nullptr);
    generalTree.addChild (ValueTree (winY).setProperty (value, var (-1), nullptr), 1, nullptr);
    generalTree.addChild (ValueTree (winW).setProperty (value, var (PLUME::UI::DEFAULT_WINDOW_WIDTH), nullptr), 2, nullptr);
    generalTree.addChild (ValueTree (winH).setProperty (value, var (PLUME::UI::DEFAULT_WINDOW_HEIGHT), nullptr), 3, nullptr);
    generalTree.addChild (ValueTree (wrapped_winX).setProperty (value, var (-1), nullptr), 4, nullptr);
    generalTree.addChild (ValueTree (wrapped_winY).setProperty (value, var (-1), nullptr), 5, nullptr);
    generalTree.addChild (ValueTree (wrapped_winW).setProperty (value, var (-1), nullptr), 6, nullptr);
    generalTree.addChild (ValueTree (wrapped_winH).setProperty (value, var (-1), nullptr), 7, nullptr);
    generalTree.addChild (ValueTree (wrapped_visible).setProperty (value, var (false), nullptr), 8, nullptr);

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
                                                                + "/Audio/Presets/Enhancia/Plume/User/",
                                                             nullptr), 2, nullptr);
  #endif
    
    generalTree.addChild (ValueTree (pluginDirs), 3, nullptr);
    generalTree.getChild (3).addChild (ValueTree (directory).setProperty (value, "", nullptr),
                                       0, nullptr);
}

void PlumeProcessor::timerCallback (int timerID)
{
    if (timerID == 0) // Unlock send tUnlock
    {
        if (stepInUnlockSequence >= unlockParamSequence.size())
        {
            stopTimer (0);
            stepInUnlockSequence = 0;
        }

        if (getWrapper().isWrapping())
        {
            getWrapper().getWrapperProcessor().getWrappedInstance()
                                              .getParameters()[127]
                                              ->setValue (unlockParamSequence[stepInUnlockSequence++]);
        }
    }
}

void PlumeProcessor::parameterValueChanged (int parameterIndex, float newValue)
{
    if (wrapper->isWrapping() && parameterIndex == 127 && isDetectingAuthSequence)
    {
        if (isNextStepInAuthSequence (newValue))
        {
            stepInAuthSequence++;

            if (stepInAuthSequence >= authParamSequence.size())
            {
                stopAuthDetection (true);
            }
        }
        else
        {
            stopAuthDetection (false);            
        }
    }
}

void PlumeProcessor::parameterGestureChanged (int parameterIndex, bool gestureIsStarting)
{
}

void PlumeProcessor::updateTrackProperties (const AudioProcessor::TrackProperties& properties)
{
	ignoreUnused (properties);
    DBG ("Name : " << properties.name << " | Colour : " << properties.colour.toDisplayString(false));
}

void PlumeProcessor::detectPlumeCrashFromPreviousSession()
{
    if (PLUME::file::deadMansPedal.existsAsFile() &&
        PLUME::file::deadMansPedal.loadFileAsString().isNotEmpty() &&
        File (PLUME::file::deadMansPedal.loadFileAsString()).exists() &&
        Time::getCurrentTime().toMilliseconds()
           - PLUME::file::deadMansPedal.getLastModificationTime().toMilliseconds() > 3000)
    {
        wrapper->getScanner().setLastCrash (PLUME::file::deadMansPedal.loadFileAsString());
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

void PlumeProcessor::initializeParamSequences()
{
    authParamSequence.add ('P');
    authParamSequence.add ('l');
    authParamSequence.add ('u');
    authParamSequence.add ('m');
    authParamSequence.add ('e');
}

void PlumeProcessor::startDetectingAuthSequence()
{    
    isDetectingAuthSequence = true;
    stepInAuthSequence = 0;

    Timer::callAfterDelay (1000, [this]()
    {
        stopAuthDetection (false);        
    });
}

void PlumeProcessor::addListenerForPlumeControlParam (AudioProcessorParameter* plumeControlParam)
{
    if (wrapper->isWrapping())
    {   
        plumeControlParam->addListener (this);
    }
}

void PlumeProcessor::removeListenerForPlumeControlParam (AudioProcessorParameter* plumeControlParam)
{
    if (wrapper->isWrapping())
    {
        plumeControlParam->removeListener (this);
    }   
}

void PlumeProcessor::stopAuthDetection (bool isDetectionSuccessful)
{
    if (isDetectingAuthSequence)
    {
        isDetectingAuthSequence = false;
        stepInAuthSequence = 0;

        if (isDetectionSuccessful)
        {
            startSendingUnlockParamSequence();
        }
    }
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
        (midiMessage.isController() && midiMessage.getControllerNumber() < 120
                                    && gestureArray->isCCInUse (midiMessage.getControllerNumber())))
    {
        return false;
    }

    return true;
}

bool& PlumeProcessor::getLastArmRef()
{
    return lastArm;
}

void PlumeProcessor::startSendingUnlockParamSequence()
{
    stepInUnlockSequence = 0;
    unlockParamSequence.clear();

    for (auto characterToEncode : presetHandler->getCurrentPresetName())
    {
        //const float randomNumber = Random::getSystemRandom().nextFloat();
        //float numberCopy = randomNumber;

        //char* valueChars = reinterpret_cast<char*> (&numberCopy);
        //valueChars[2] = characterToEncode;

        unlockParamSequence.add (int(characterToEncode)/127.0f);
    }

    startTimer (0, 30);
}

bool PlumeProcessor::isNextStepInAuthSequence (float receivedValue)
{
    const char authChar = char (receivedValue*127);
    const bool isnextstep = (authParamSequence[stepInAuthSequence] == authChar);

    return isnextstep;
}
