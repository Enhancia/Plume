/*
 ==============================================================================
 
 This file was auto-generated!
 
 ==============================================================================
 */

#include "DataReader/DataReaderMacOS/StatutPipe.h"

//==============================================================================
StatutPipe::StatutPipe(): InterprocessConnection (true, 0x6a6d626e)
{
    connected = false;
    nbPipeToConnect = 0;
    connectNewPipe();
}

StatutPipe::~StatutPipe()
{}


//==============================================================================
bool StatutPipe::connectNewPipe()
{
    return connectToPipe("StatutPipe", -1);
}

bool StatutPipe::isConnected()
{
    return connected;
}
int StatutPipe::getPipeToConnect()
{
    return nbPipeToConnect;
}

//==============================================================================
void StatutPipe::connectionMade()
{
    connected = true;
    String test = "Start";
    sendMessage(MemoryBlock(test.toUTF8(), test.getNumBytesAsUTF8()));
}

void StatutPipe::connectionLost()
{
    connected = false;
}

void StatutPipe::messageReceived(const MemoryBlock &message)
{
    if (connected && message.getSize() != 0)
    {
        nbPipeToConnect = message.toString().getIntValue();
        sendChangeMessage();
        disconnect();
    }
}
