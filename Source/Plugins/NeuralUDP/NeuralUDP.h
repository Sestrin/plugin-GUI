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

#ifndef NEURALUDP_H_INCLUDED
#define NEURALUDP_H_INCLUDED


#ifdef _WIN32
#include <Windows.h>
#endif

#include <ProcessorHeaders.h>

/**
    This is a processor that acts as a source from an NEURAL UDP stream

*/
class NeuralUDP : public GenericProcessor
{
public:
    /** The class constructor, used to initialize any members. */
    NeuralUDP();

    /** The class destructor, used to deallocate memory */
    ~NeuralUDP();

    /** Defines the functionality of the processor.

        The process method is called every time a new data buffer is available.

        Processors can either use this method to add new data, manipulate existing
        data, or send data to an external target (such as a display or other hardware).

        Continuous signals arrive in the "buffer" variable, event data (such as TTLs
        and spikes) is contained in the "events" variable, and "nSamples" holds the
        number of continous samples in the current buffer (which may differ from the
        size of the buffer).
    */
    void process (AudioSampleBuffer& buffer, MidiBuffer& events) override;

    int getNumHeadstageOutputs() const override {return 65;} //this is hardcoded... fix later
    

    float getSampleRate()  const override {return 30000.0;} //hardcoded
    float getDefaultSampleRate()  const override {return 30000.0;} //hardcoded
    //float getBitVolts(Channel * chan)  const override {return 1.0;}

    bool isReady() override;
    void setEnabledState(bool newState) override;
    void updateSettings() override;

private:

    /** Network Socket **/
    int sock;

    
    CriticalSection lock;

    int64 timestamp;

    
    // ==================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NeuralUDP);
};



#endif  // NeuralUDP_H_INCLUDED
