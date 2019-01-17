/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "DataReader/DataReader.h"

//==============================================================================
DataReader::DataReader(): InterprocessConnection (true, 0x6a6d626e)
{
    connected = false;
    
    // Data initialization
    data = new StringArray (StringArray::fromTokens ("0 0 0 0 0 0 0", " ", String()));
    
    // Pipe creation
	connectToExistingPipe();
}

DataReader::~DataReader()
{
    data = nullptr;
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

//==============================================================================
bool DataReader::connectToExistingPipe()
{
	return connectToPipe ("mynamedpipe", -1);
}

bool DataReader::isConnected()
{
    return connected;
}

//==============================================================================
void DataReader::connectionMade()
{
    connected = true;
}

void DataReader::connectionLost()
{
    connected = false;
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