/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/
#include "../../JuceLibraryCode/JuceHeader.h"

#if (JUCE_WINDOWS || defined(__OBJC__))

#include "DataReader.h"


/*
 * GETTERS AND SETTERS
 */

float& DataReader::getBatteryReference ()
{
    return batteryValue;
}

bool DataReader::getHubIsConnected () const
{
    return hubIsConnected;
}

bool DataReader::getRingIsConnected () const
{
    return ringIsConnected;
}

bool DataReader::getRingIsCharging () const
{
    return ringIsCharging;
}

void DataReader::setHubIsConnected (const bool value)
{
    hubIsConnected = value;
}

void  DataReader::setRingIsConnected (const bool value)
{
    ringIsConnected = value;
}

void  DataReader::setRingIsCharging (const bool value)
{
    ringIsCharging = value;
}

/*
 * //GETTERS AND SETTERS
 */

//==============================================================================
DataReader::DataReader(): InterprocessConnection (true, 0x6a6d626e)
{
    setSize (120, 50);
    connected = false;
    
    // Data initialization
    data.reset (new StringArray (StringArray::fromTokens ("0 0 0 0 0 0 0", " ", String())));
    
  // On MacOS we first connect to the daemon StatutPipe to know which dataPipe we can use
  #if JUCE_MAC
    // Run a timer to wait for an amout of time depending on the number of Plume instanciate -> avoid simultaneous connection on statutPipe
    if (PLUME::nbInstance != 1)
    {
        startTimer(1, PLUME::nbInstance*50);
        return;
    }
    // some DAW (ie. Bitwig) host Plume in separated sandBox, so their memory are not shared and nbInstance is not incremented
    // In this case we wait for a random amount of time avoid concurrent connection on statutpipe
    Range<int> rangeWaitingTime(0,1000);
    startTimer(1, juce::Random::getSystemRandom().nextInt(rangeWaitingTime));
  #else
    // Pipe creation
    connectToExistingPipe();
  #endif
}

DataReader::~DataReader()
{
        
    disconnect();

    data = nullptr;
  #if JUCE_MAC
    statutPipe = nullptr;
  #endif
 }

//==============================================================================
void DataReader::paint (Graphics& g)
{
    g.fillAll (Colour (0x42000000));
}

void DataReader::resized()
{
}

//==============================================================================
bool DataReader::readData (String s)
{
	auto strArr = StringArray::fromTokens(s, " ", String());

    // Checks for full lines
    if (strArr.size() == DATA_SIZE)
    {
        // disconnect ring if timer not re-run after 3sec
        startTimer (0, 3000);

        // Set states of ring (is connected and is charging)
        if(!getRingIsConnected())
        {
            setRingIsConnected (true);
            sendActionMessage(PLUME::commands::updateBatteryDisplay);
        }
        else if (getRingIsCharging())
        {
            setRingIsCharging (false);
            sendActionMessage(PLUME::commands::updateBatteryDisplay);
        }


        *data = strArr;
        // Get only battery value
        batteryValue = (*data)[static_cast<int>(PLUME::data::battery)].getFloatValue();
        DBG ("Ring is connected: " << (getRingIsConnected () ? "true" : "false"));
        DBG ("Ring is charging: " << (getRingIsCharging () ? "true" : "false"));
        DBG (batteryValue);
        return true;
    }

    if(strArr.size() == 1)
    {
        // disconnect ring if timer not re-run after 3sec
        startTimer (0, 3000);

        if(!getRingIsConnected())
        {
            setRingIsConnected (true);
            setRingIsCharging (true);
            sendActionMessage(PLUME::commands::updateBatteryDisplay);
        }
        else if (!getRingIsCharging())
        {
            setRingIsCharging (true);
            sendActionMessage(PLUME::commands::updateBatteryDisplay);
        }

        // Get battery value
        *data = strArr;
        batteryValue = (*data)[0].getFloatValue ();

        DBG ("Ring is connected: " << (getRingIsConnected () ? "true" : "false"));
        DBG ("Ring is charging: " << (getRingIsCharging() ? "true" : "false"));
        DBG (batteryValue);

        return true;
    }
    
	return false;
}

const String DataReader::getRawData (int index)
{
    return (*data)[index];
}

bool DataReader::getRawDataAsFloatArray(Array<float>& arrayToFill)
{
    // Checks that the array has the right amont and type of data
    if (arrayToFill.isEmpty() == false) return false;
    
    // Fills the array with new values taken from the "Data" StringArray
    for (int i =0; i<DATA_SIZE; i++)
    {
        arrayToFill.add ((*data)[i].getFloatValue());
    }
    
    return true;
}

//==============================================================================
void  DataReader::sendString(uint8_t* data, int data_size)
{
    bool test = sendMessage(MemoryBlock(data, data_size));
    DBG("Send string return :" + String(int(test)));
}

//==============================================================================
void DataReader::timerCallback (int timerID)
{
    if (timerID == 0)
    {
        if (getRingIsConnected ())
            setRingIsConnected (false);

		stopTimer (0);
    }

  #if JUCE_MAC
    if (timerID == 1)
    {
        stopTimer (1);
        instantiateStatutPipe();
    }
  #endif
}

void DataReader::instantiateStatutPipe()
{
    //only happens on MacOS
  #if JUCE_MAC
    statutPipe = std::make_unique<StatutPipe> ();
    statutPipe->addChangeListener(this);
  #endif
}

void DataReader::changeListenerCallback (ChangeBroadcaster*)
{
  //only happens on MacOS
  #if JUCE_MAC
    int nbPipeToConnect = statutPipe->getPipeToConnect();
    connectToExistingPipe(nbPipeToConnect);
    statutPipe->disconnect();
    statutPipe.reset();
  #endif
}

//==============================================================================
bool DataReader::connectToExistingPipe()
{
	return connectToPipe ("mynamedpipe", -1);
}

bool DataReader::connectToExistingPipe(int
                                        #if JUCE_MAC
                                            nbPipe
                                        #endif
                                       )
{
  //only happens on MacOS
  #if JUCE_MAC
    //get current userID
    uid_t currentUID;
    SCDynamicStoreCopyConsoleUser(NULL, &currentUID, NULL);

    //create namedpipe  with currentUID to enable multi user session
    return connectToPipe("mynamedpipe" + String (currentUID) + String(nbPipe), -1);
  #elif JUCE_WINDOWS
	return false;
  #endif
}

bool DataReader::isConnected()
{
    return connected;
}

//==============================================================================
void DataReader::connectionMade()
{
    connected = true;
    
  #if JUCE_MAC
    String test = "Start";
    sendMessage(MemoryBlock(test.toUTF8(), test.getNumBytesAsUTF8()));
  #endif
}

void DataReader::connectionLost()
{
    connected = false;
    
  #if JUCE_MAC
    String test = "Stop";
    sendMessage(MemoryBlock(test.toUTF8(), test.getNumBytesAsUTF8()));
  #endif
}

void DataReader::messageReceived (const MemoryBlock &message)
{
    if (connected && message.getSize() != 0)
    {
        if (readData (message.toString()))
        {
            sendChangeMessage();
        }
    }
}
#endif //JUCE_WINDOWS || DEFINED(__OBJC__)
