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
#include <sys/time.h>
#include <netinet/in.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

const int PORT = 9000;
const int PACKETS_PER_PROCESS = 60;
const int NUM_NEURAL_CHANNELS = 65;
const int SAMPLES_PER_PACKET = 10;
const char PACKET_HEADER[] = "OneNeural";

#include "MicroPhysProcessor.h"
#include "MicroPhysProcessorEditor.h"

//If the processor uses a custom editor, it needs its header to instantiate it
//#include "ExampleEditor.h"


MicroPhysProcessor::MicroPhysProcessor()
    : GenericProcessor ("MicroPhys") //, threshold(200.0), state(true)

{
    setProcessorType (PROCESSOR_TYPE_SOURCE);

    // Open Ephys Plugin Generator will insert generated code for parameters here. Don't edit this section.
    //[OPENEPHYS_PARAMETERS_SECTION_BEGIN]
    //[OPENEPHYS_PARAMETERS_SECTION_END]

    //Without a custom editor, generic parameter controls can be added
    //parameters.add(Parameter("thresh", 0.0, 500.0, 200.0, 0));
    // setup port
    struct sockaddr_in myaddr;

    // Port should accept traffic from any address
    memset((char *)&myaddr, 0, sizeof(myaddr));
    myaddr.sin_family = AF_INET;
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    myaddr.sin_port = htons(PORT);

    sock = socket(AF_INET, SOCK_DGRAM, 0);

    struct timeval tv;
    tv.tv_sec = 3;
    tv.tv_usec = 0;
    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0)
    {
        perror("Error");
        return;
    }

    if(sock == -1){
        perror("Neural UDP could not create a socket");
        return;
    }

    if(bind(sock, (struct sockaddr *) &myaddr, sizeof(myaddr)) < 0) {
        perror("Neural UDP Socket Bind Failure");
        return;
    }

    timestamp = 0;
    
}


MicroPhysProcessor::~MicroPhysProcessor()
{
    //close socket
    close(sock);
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

    //Parameter& p =  parameters.getReference(parameterIndex);
    //p.setValue(newValue, 0);

    //threshold = newValue;

    //std::cout << float(p[0]) << std::endl;
}


void MicroPhysProcessor::process (AudioSampleBuffer& buffer, MidiBuffer& events)
{   

    

    // packet process
    static int PACKET_HEADER_LENGTH = strlen(PACKET_HEADER);
    static int PACKET_SIZE = 2*NUM_NEURAL_CHANNELS*SAMPLES_PER_PACKET + PACKET_HEADER_LENGTH;
    char packetBuffer[PACKET_SIZE];
    static int readLength = 0, bufferIdx = 0;
    static int16_t * sampleInt16Ptr = NULL;
    static uint16_t * sampleUint16Ptr = NULL;

    printf("Neural UDP (NC: %d NS: %d) \n ", buffer.getNumChannels(), buffer.getNumSamples());

        for(int packetNum = 0; packetNum < PACKETS_PER_PROCESS; packetNum++)
    {
        readLength = recvfrom(sock, packetBuffer, (size_t)PACKET_SIZE, 0, NULL, 0);
        if(readLength != PACKET_SIZE){
            printf("Readlength: %d\n",readLength);
            printf("PacketSize: %d\n",PACKET_SIZE);
            perror("Incorrect UDP packet size read");
            return;
        }
        if(strncmp(PACKET_HEADER, packetBuffer, PACKET_HEADER_LENGTH) != 0){
            perror("Incorrect UDP packet header");
            return;   
        }

        printf("Processing a packet!\n");



        for(int packetChannel = 0; packetChannel < NUM_NEURAL_CHANNELS; packetChannel++)
        {
            for(int packetSampleNum = 0; packetSampleNum < SAMPLES_PER_PACKET; packetSampleNum ++)
            {
                if(packetChannel == 0){
                    sampleUint16Ptr = (uint16_t*) &packetBuffer[2*(packetChannel*SAMPLES_PER_PACKET+packetSampleNum) + PACKET_HEADER_LENGTH];
                    buffer.setSample(packetChannel, packetNum*SAMPLES_PER_PACKET+packetSampleNum, (float) *sampleUint16Ptr);
                    
                    printf(" %f ", buffer.getSample(packetChannel, packetNum*SAMPLES_PER_PACKET+packetSampleNum));

                }
                else{
                    sampleInt16Ptr = (int16_t*) &packetBuffer[2*(packetChannel*SAMPLES_PER_PACKET+packetSampleNum) + PACKET_HEADER_LENGTH];
                    buffer.setSample(packetChannel, packetNum*SAMPLES_PER_PACKET+packetSampleNum, (float) *sampleInt16Ptr);
                }

            }
        }
    }
    
    timestamp += PACKETS_PER_PROCESS*SAMPLES_PER_PACKET;
    setTimestamp(events, timestamp);
    setNumSamples(events, SAMPLES_PER_PACKET*PACKETS_PER_PROCESS); 

    
       
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
