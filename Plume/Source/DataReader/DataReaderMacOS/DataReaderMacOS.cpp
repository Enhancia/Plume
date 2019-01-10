/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/
#if 0
#include "DataReader/DataReaderMacOS/DataReaderMacOS.h"

//==============================================================================
DataReader::DataReader(StatutPipe& stPipe): InterprocessConnection (true, 0x6a6d626e), statutPipe(stPipe)
{
    
    connected = false;
    data = new StringArray (StringArray::fromTokens ("0 0 0 0 0 0 0", " ", String()));
    statutPipe.addChangeListener(this);
	//connectNewPipe(nbPipe);
}

DataReader::~DataReader()
{
}

//==============================================================================
bool DataReader::connectNewPipe(int nbPipe)
{
    return connectToPipe("mynamedpipe" + String(nbPipe), -1);
}

bool DataReader::isConnected()
{
    return connected;
}


//==============================================================================
bool DataReader::readData (String s)
{
    auto strArr = StringArray::fromTokens(s, " ", String());
    
    // Checks for full lines
    if (strArr.size() == DATA_SIZE)
    {
        // Splits the string into 7 separate ones
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

/*
String DataReader::getData()
{
    return data;
}
*/

//==============================================================================
void DataReader::connectionMade()
{
    connected = true;
    String test = "Start";
    sendMessage(MemoryBlock(test.toUTF8(), test.getNumBytesAsUTF8()));
}

void DataReader::connectionLost()
{
    connected = false;
    String test = "Stop";
    sendMessage(MemoryBlock(test.toUTF8(), test.getNumBytesAsUTF8()));
}

void DataReader::messageReceived(const MemoryBlock &message)
{
    if (connected && message.getSize() != 0)
    {
        //data = message.toString();
        if (readData (message.toString()))
        {
            sendChangeMessage();
        }
    }
}

void DataReader::changeListenerCallback (ChangeBroadcaster * source)
{
        int nbPipeToConnect = statutPipe.getPipeToConnect();
        connectNewPipe(nbPipeToConnect);
        statutPipe.disconnect();
}

#endif
