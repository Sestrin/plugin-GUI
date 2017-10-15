/*
    ------------------------------------------------------------------

    This file is part of the Open Ephys GUI
    Copyright (C) 2016 Open Ephys

    ------------------------------------------------------------------

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/


#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

#include "MicroPhysProcessor.h"
#include "MicroPhysProcessorEditor.h"

#define SRV_IP "127.0.0.1" //Need to update this!

const int PORT = 9000; //Rx
const int PORTSEND = 9001; //Tx


MicroPhysProcessor::MicroPhysProcessor()
    : GenericProcessor ("MicroPhys") //, threshold(200.0), state(true)

{
    setProcessorType (PROCESSOR_TYPE_SOURCE);

    // Open Ephys Plugin Generator will insert generated code for parameters here. Don't edit this section.
    //[OPENEPHYS_PARAMETERS_SECTION_BEGIN]
    // Parameter 0 code
    auto parameter0 = new Parameter ("Sample Rate", "Sample Rate", 1000, 10000, currentSampleRate, 0);
    parameter0->setEditorDesiredBounds (0,0,0,0);
    String parameter0description = "Set sampling rate (Hz)";
    parameter0->setDescription (parameter0description);
    parameters.add (parameter0);


    //[OPENEPHYS_PARAMETERS_SECTION_END]

    //Without a custom editor, generic parameter controls can be added
    //parameters.add(Parameter("thresh", 0.0, 500.0, 200.0, 0));

    //Display channel state all on by default    
    for(int i=0; i < currentChannelNum; i++)
    {
        displayChannels[i] = 1;
    }
    

    // UDP Receive setup
    struct sockaddr_in myaddr;


    // Port should accept traffic from any address
    memset((char *)&myaddr, 0, sizeof(myaddr));
    myaddr.sin_family = AF_INET;
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    myaddr.sin_port = htons(PORT);

    sock = socket(AF_INET, SOCK_DGRAM, 0);

    if(sock == -1){
        perror("Neural UDP could not create a socket");
        return;
    }

    if(bind(sock, (struct sockaddr *) &myaddr, sizeof(myaddr)) < 0) {
        perror("Neural UDP Socket Bind Failure");
        return;
    }

    timestamp = 0;
    // 

    // UDP Send setup
    struct sockaddr_in si_other;
    int slen = sizeof(si_other);
    sockSend = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockSend==-1) {
        perror("socket");
        return;
    }

    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(PORTSEND);
    if (inet_aton(SRV_IP, &si_other.sin_addr)==0) {
        fprintf(stderr, "inet_aton() failed\n");
        return;
    }
    // unnecessary.. delete in future 
    if(bind(sockSend, (struct sockaddr *) &si_other, sizeof(si_other)) < 0) {
        perror("Send UDP Bind Failure");
        return;
    }
    //

}


MicroPhysProcessor::~MicroPhysProcessor()
{
        close(sock);
        close(sockSend);
}


/**
  If the processor uses a custom editor, this method must be present.
*/
AudioProcessorEditor* MicroPhysProcessor::createEditor()
{
    editor = new MicroPhysProcessorEditor (this, true);

    //std::cout << "Creating editor." << std::endl;

    return editor;
}


void MicroPhysProcessor::setParameter (int parameterIndex, float newValue)
{
    GenericProcessor::setParameter (parameterIndex, newValue);
    editor->updateParameterButtons (parameterIndex);

    if(parameterIndex == 0)
    {
        currentSampleRate = newValue;
    }
    

    //Parameter& p =  parameters.getReference(parameterIndex);
    //p.setValue(newValue, 0);

    //threshold = newValue;

    //std::cout << float(p[0]) << std::endl;
}


void MicroPhysProcessor::process (AudioSampleBuffer& buffer, MidiBuffer& events)
{
    /**
      Generic structure for processing buffer data
    */
    sendMetaData();
    int nChannels = buffer.getNumChannels();
    for (int chan = 0; chan < nChannels; ++chan)
    {
        int nSamples = getNumSamples (chan);
        /* =============================================================================
          Do something here.

          To obtain a read-only pointer to the n sample of a channel:
          float* samplePtr = buffer.getReadPointer(chan,n);

          To obtain a read-write pointer to the n sample of a channel:
          float* samplePtr = buffer.getWritePointer(chan,n);

          All the samples in a channel are consecutive, so this example is valid:
          float* samplePtr = buffer.getWritePointer(chan,0);
          for (i=0; i < nSamples; i++)
          {
         *(samplePtr+i) = (*samplePtr+i)+offset;
         }

         See also documentation and examples for buffer.copyFrom and buffer.addFrom to operate on entire channels at once.

         To add a TTL event generated on the n-th sample:
         addEvents(events, TTL, n);
         =============================================================================== */
    }

    /** Simple example that creates an event when the first channel goes under a negative threshold

      for (int i = 0; i < getNumSamples(channels[0]->sourceNodeId); i++)
      {
      if ((*buffer.getReadPointer(0, i) < -threshold) && !state)
      {

    // generate midi event
    addEvent(events, TTL, i);

    state = true;

    } else if ((*buffer.getReadPointer(0, i) > -threshold + bufferZone)  && state)
    {
    state = false;
    }

    }
    */
}


void MicroPhysProcessor::saveCustomParametersToXml (XmlElement* parentElement)
{
    XmlElement* mainNode = parentElement->createNewChildElement ("MicroPhysProcessor");
    mainNode->setAttribute ("numParameters", getNumParameters());

    // Open Ephys Plugin Generator will insert generated code to save parameters here. Don't edit this section.
    //[OPENEPHYS_PARAMETERS_SAVE_SECTION_BEGIN]
    for (int i = 0; i < getNumParameters(); ++i)
    {
        XmlElement* parameterNode = mainNode->createNewChildElement ("Parameter");

        auto parameter = getParameterObject(i);
        parameterNode->setAttribute ("name", parameter->getName());
        parameterNode->setAttribute ("type", parameter->getParameterTypeString());

        auto parameterValue = getParameterVar (i, currentChannel);

        if (parameter->isBoolean())
            parameterNode->setAttribute ("value", (int)parameterValue);
        else if (parameter->isContinuous() || parameter->isDiscrete() || parameter->isNumerical())
            parameterNode->setAttribute ("value", (double)parameterValue);
    }
    //[OPENEPHYS_PARAMETERS_SAVE_SECTION_END]
}


void MicroPhysProcessor::loadCustomParametersFromXml()
{
    if (parametersAsXml == nullptr) // prevent double-loading
        return;

    // use parametersAsXml to restore state

    // Open Ephys Plugin Generator will insert generated code to load parameters here. Don't edit this section.
    //[OPENEPHYS_PARAMETERS_LOAD_SECTION_BEGIN]
    forEachXmlChildElement (*parametersAsXml, mainNode)
    {
        if (mainNode->hasTagName ("MicroPhysProcessor"))
        {
            int parameterIdx = -1;

            forEachXmlChildElement (*mainNode, parameterNode)
            {
                if (parameterNode->hasTagName ("Parameter"))
                {
                    ++parameterIdx;

                    String parameterType = parameterNode->getStringAttribute ("type");
                    if (parameterType == "Boolean")
                        setParameter (parameterIdx, parameterNode->getBoolAttribute ("value"));
                    else if (parameterType == "Continuous" || parameterType == "Numerical")
                        setParameter (parameterIdx, parameterNode->getDoubleAttribute ("value"));
                    else if (parameterType == "Discrete")
                        setParameter (parameterIdx, parameterNode->getIntAttribute ("value"));
                }
            }
        }
    }
    //[OPENEPHYS_PARAMETERS_LOAD_SECTION_END]
}

float MicroPhysProcessor::getSampleRate() const
{
    return currentSampleRate;
}

float MicroPhysProcessor::getDefaultSampleRate() const
{
    return currentSampleRate;
}

int MicroPhysProcessor::getNumHeadstageOutputs() const
{
    return currentChannelNum;
}

void MicroPhysProcessor::setDisplayChannelState (int channel, bool newState)
{
    displayChannels[channel] = newState;    
    printf("channel: %d, state: %d\n",channel,newState);
}

void MicroPhysProcessor::sendMetaData ()
{
    int res;
    int64 x1 = 0, x2 = 0;

    x1 = int(round( currentSampleRate / 1000));
    printf("sampling rate: %d\n",int(x1));
    x1 = x1 << 32;

    for(int i = 0; i < currentChannelNum; i++)
    {
        x2 = x2 + (1 << i)*int(displayChannels[i]);
        printf("channel: %d\n",int(x2)); //don't transmit the 32nd channel weird stuff will happen!
    }

    int64 dat = x1 + x2;
    int dat_len = sizeof(dat);
    res = sendto(sockSend, &dat, dat_len, 0, NULL, 0 );
    if (res == -1) {
        perror("sendto");
        return;
    }

}