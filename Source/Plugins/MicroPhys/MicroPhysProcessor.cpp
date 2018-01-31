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

//#define SRV_IP "127.0.0.1" //192.168.0.100 or 101

const int PORT = 8623; //Rx
const int PORTSEND = 2950; //Tx
//const in PACKETS_PER_PROCESS = 10;
const int PACKET_HEADER_LENGTH = 9; //bytes
const int PACKET_SIZE = 1280+PACKET_HEADER_LENGTH; //bytes 1280


MicroPhysProcessor::MicroPhysProcessor()
    : GenericProcessor ("MicroPhys") //, threshold(200.0), state(true)
    , ip_addr("192.168.000.100")

{
    setProcessorType (PROCESSOR_TYPE_SOURCE);

    // Open Ephys Plugin Generator will insert generated code for parameters here. Don't edit this section.
    //[OPENEPHYS_PARAMETERS_SECTION_BEGIN]
    // Parameter 0 code
    auto parameter0 = new Parameter ("", "Sample Rate", 1000, 10000, 2000, 0);
    parameter0->setEditorDesiredBounds (9,8,104,40);
    String parameter0description = "Set sampling rate (Hz)";
    parameter0->setDescription (parameter0description);
    parameters.add (parameter0);

    // Parameter 1 code
    auto parameter1 = new Parameter ("", "IP Address P1", 1, 999, 192, 1); //change so min. value is 0 and that dafault is not 0
    parameter1->setEditorDesiredBounds (8,60,32,40);
    String parameter1description = "Set the IP Address of your MicroPhys";
    parameter1->setDescription (parameter1description);
    parameters.add (parameter1);

    // Parameter 2 code
    auto parameter2 = new Parameter ("", "IP Address P2", 1, 999, 168, 2);
    parameter2->setEditorDesiredBounds (48,60,32,40);
    parameters.add (parameter2);

    // Parameter 3 code
    auto parameter3 = new Parameter ("", "IP Address P3", 0, 999, 0, 3);
    parameter3->setEditorDesiredBounds (88,60,32,40);
    parameters.add (parameter3);

    // Parameter 4 code
    auto parameter4 = new Parameter ("", "IP Address P4", 1, 999, 100, 4);
    parameter4->setEditorDesiredBounds (128,60,32,40);
    parameters.add (parameter4);


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

    struct timeval tv;
    tv.tv_sec = 3;
    tv.tv_usec = 0;
    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv,sizeof(tv)) < 0) {
        perror("timeout");
    }

    timestamp = 0;
    // 
    

}


MicroPhysProcessor::~MicroPhysProcessor()
{
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

    if(parameterIndex == 0)
    {
        currentSampleRate = newValue;
    }
    else
        updateIpAddr(parameterIndex,newValue);
    

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

        
    char packetBuffer[PACKET_SIZE];
    int idx = 0;
    uint16_t tmp;
    float data;

    int readLength = recvfrom(sock, packetBuffer, (size_t)PACKET_SIZE, 0, NULL, 0);

    if (PACKET_SIZE != readLength)
    {
        printf("packet read length incorrect: %d\n",readLength);
        return;
    }    


    //process packet header
    //add later...
    idx = idx + PACKET_HEADER_LENGTH;
    unsigned long packet_id = char2Long(&packetBuffer[1]);

    /*
    for (int i = 1; i < 5; i++)
    {
        packet_id = packet_id + (unsigned long)packetBuffer[i] << 8*(i-1);
    }
    */

    printf("packet_id: %lu",packet_id);

    int nChannels = 32;//buffer.getNumChannels();
    int displayChanNum = getDisplayChanNum();
    
    //int nSamples = (PACKET_SIZE - PACKET_HEADER_LENGTH)/(2*displayChanNum);
    int nSamples = (PACKET_SIZE - PACKET_HEADER_LENGTH)/(2*32);
    //printf("nSamples: %d",nSamples);
    //printf("Neural UDP (NC: %d NS: %d) \n ", buffer.getNumChannels(), buffer.getNumSamples());
    
    //tmp = ((int(packetBuffer[idx+1]) << 8) + int(packetBuffer[idx]));
    //data = ((float)tmp - 32768)*0.195;
    
    //printf("measurement: %f\n ",tmp);
    //buffer.setSample(1, 0, data);
    //printf(" %f \n", buffer.getSample(1, 0)); 

    for (int i = 0; i < nSamples; ++i) //sample loop
    {
        for (int chan = 0; chan < nChannels; ++chan)
        {
            
            tmp = ((int(packetBuffer[idx+1]) << 8) + int(packetBuffer[idx]));
            data = ((float)tmp - 32768)*0.195;            
            
            buffer.setSample(chan, i, data);
            //printf("chan: %d; sample: %d; data: %f \n",chan, i, buffer.getSample(chan, i)); 

            idx = idx + 2;
            //printf("idx: %d\n",idx);
            //float* samplePtr = buffer.getWritePointer(chan,i);
            
            /*
            if (displayChannels[chan])
            {
                tmp = ((int(packetBuffer[idx+1]) << 8) + int(packetBuffer[idx]) - 32768)*0.195;
                printf("measurement: %f\n ",tmp);
                idx = idx + 2;
                //*samplePtr = tmp;
            }
            else
                tmp = 1; // to prevent compiler error...               
                //*samplePtr = 0;
            */

        }
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

    timestamp = timestamp + nSamples;
    setTimestamp(events, timestamp);
    setNumSamples(events, 1);    

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

unsigned long MicroPhysProcessor::char2Long(char* charNum)
{
    //Reverse order of bytes in str
    int j = sizeof(unsigned long) - 1;
    for (int i = 0; i < j; ++i)
    {
        char temp = charNum[i];
        charNum[i] = charNum[j];
        charNum[j] = temp;
        --j;
    }
    unsigned long longNum = (unsigned long)((unsigned char)(charNum[0]) << 24 | //bit shifts 4 chars into a 4 byte long
                                            (unsigned char)(charNum[1]) << 16 |
                                            (unsigned char)(charNum[2]) << 8 |
                                            (unsigned char)(charNum[3]));

    return longNum;
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
}

void MicroPhysProcessor::sendMetaData ()
{
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
    if (inet_aton(ip_addr, &si_other.sin_addr)==0) {
        fprintf(stderr, "inet_aton() failed\n");
        return;
    }
    /*
    // unnecessary.. delete in future 
    if(bind(sockSend, (struct sockaddr *) &si_other, sizeof(si_other)) < 0) {
        perror("Send UDP Bind Failure");
        return;
    }
    //
    */

    int res;
    int64 x1 = 0, x2 = 0;

    //Sample Rate, x1
    x1 = int(round( currentSampleRate / 1000));
    printf("sampling rate: %d\n",int(x1));
    x1 = x1 << 32;

    //Channels to Transmit, x2
    for(int i = 0; i < currentChannelNum; i++)
    {
        x2 = x2 + (1 << i)*int(displayChannels[i]);
        printf("channel: %d\n",int(x2)); //don't transmit the 32nd channel weird stuff will happen!
    }

    int64 dat = x1 + x2;    
    dat = 0b1111111111111111; // testing purposes... needs to be 9 bits
    printf("sending init packet\n");
    int dat_len = sizeof(dat);
    res = sendto(sockSend, &dat, dat_len, 0, (struct sockaddr *) &si_other, slen );
    if (res == -1) {
        perror("sendto");
        return;
    }
    printf("after sending init packet\n");
    close(sockSend);

}

bool MicroPhysProcessor::enable ()
{
    sendMetaData();
    return true;
}

bool MicroPhysProcessor::disable ()
{
    sendStopMessage();
    return true;
}

int MicroPhysProcessor::getDisplayChanNum()
{
    int displayChanNum = 0;

    for(int i=0; i < currentChannelNum; i++)
    {
        displayChanNum = displayChanNum + displayChannels[i];
    }

    return displayChanNum;

}

void MicroPhysProcessor::sendStopMessage()
{
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
    if (inet_aton(ip_addr, &si_other.sin_addr)==0) {
        fprintf(stderr, "inet_aton() failed\n");
        return;
    }    

    int res;
    //int64 dat = 0b00011000;
    uint16_t dat = 0b0000000000000000;
    
    /*
    for(int i = 0; i < 64; i++)
    {
        dat = dat + (1 << i);        
    }
    */
    
    int dat_len = sizeof(dat);
    res = sendto(sockSend, &dat, dat_len, 0, (struct sockaddr *) &si_other, slen );
    if (res == -1) {
        perror("sendto");
        return;
    }
    printf("send stop message\n");
    close(sockSend);
}

void MicroPhysProcessor::updateIpAddr(int index, float num)
{
    int offset = (index-1)*4;
    char str[4];

    sprintf(str,"%03d",int(num));
    for (int i = 0; i < 3; i++)
        ip_addr[offset+i] = str[i];
    //printf("%s\n",ip_addr);
}

