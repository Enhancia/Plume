/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/
#include "../../JuceLibraryCode/JuceHeader.h"

#if (JUCE_WINDOWS || defined(__OBJC__))

#include "DataReader.h"

//==============================================================================
DataReader::DataReader(): InterprocessConnection (true, 0x6a6d626e)
{
    setSize (120, 50);
    connected = false;
    
    // Data initialization
    data.reset (new StringArray (StringArray::fromTokens ("0 0 0 0 0 0 0", " ", String())));
    
    #if JUCE_MAC
        statutPipe = std::make_unique<StatutPipe> ();
        statutPipe->addChangeListener(this);
    #else
        // Pipe creation
        connectToExistingPipe();
    #endif
}

DataReader::~DataReader()
{
    TRACE_IN;
    
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
        // Splits the string into DATA_SIZE separate ones
        *data = strArr;
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
    
    //DBG ("Data: " << data->joinIntoString(", "));
    
    // Fills the array with new values taken from the "Data" StringArray
    for (int i =0; i<DATA_SIZE; i++)
    {
        arrayToFill.add ((*data)[i].getFloatValue());
        //DBG ("Value " << i << " = " << arrayToFill[i]);
    }
    
    return true;
}

//==============================================================================
void  DataReader::sendString(uint8_t* data, int data_size)
{
    TRACE_IN;
    bool test = sendMessage(MemoryBlock(data, data_size));
    DBG("Send string return :" + String(int(test)));
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

#endif //JUCE_WINDOWS || DEFINED(__OBJC__)
