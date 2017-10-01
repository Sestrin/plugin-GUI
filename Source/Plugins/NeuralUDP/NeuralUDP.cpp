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
#include <netinet/in.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

#include "NeuralUDP.h"

const int PORT = 9000;
const int PACKETS_PER_PROCESS = 60;
const int NUM_NEURAL_CHANNELS = 65;
const int SAMPLES_PER_PACKET = 10;
const char PACKET_HEADER[] = "OneNeural";






NeuralUDP::NeuralUDP()
    : GenericProcessor ("1xNeural UDP") //, threshold(200.0), state(true)
{
    setProcessorType (PROCESSOR_TYPE_SOURCE);

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

}


NeuralUDP::~NeuralUDP()
{
    close(sock);
}



void NeuralUDP::process (AudioSampleBuffer& buffer, MidiBuffer& events)
{
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


