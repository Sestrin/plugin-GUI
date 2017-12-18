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

#ifndef TFPROCESSOR_H_INCLUDED
#define TFPROCESSOR_H_INCLUDED

#ifdef _WIN32
#include <Windows.h>
#endif

#include "c_api.h"
#include <ProcessorHeaders.h>

/**
    This class serves as a template for creating new processors.

    If this were a real processor, this comment section would be used to
    describe the processor's function.

    @see GenericProcessor
*/
class TfProcessor : public GenericProcessor

{
public:
    /** The class constructor, used to initialize any members. */
    TfProcessor();

    /** The class destructor, used to deallocate memory */
    ~TfProcessor();

    /** Indicates if the processor has a custom editor. Defaults to false */
    //bool hasEditor() const { return true; }

    //TODO <Kirill A>: it was commented before, think about it
    /** If the processor has a custom editor, this method must be defined to instantiate it. */
    AudioProcessorEditor* createEditor() override;

    /** Optional method that informs the GUI if the processor is ready to function. If false acquisition cannot start. Defaults to true */
    //bool isReady();

    /** Defines the functionality of the processor.

        The process method is called every time a new data buffer is available.

        Processors can either use this method to add new data, manipulate existing
        data, or send data to an external target (such as a display or other hardware).

        Continuous signals arrive in the "buffer" variable, event data (such as TTLs
        and spikes) is contained in the "events" variable.
    */
    void process (AudioSampleBuffer& buffer, MidiBuffer& events) override;

    /** The method that standard controls on the editor will call.
        It is recommended that any variables used by the "process" function
        are modified only through this method while data acquisition is active. */
    void setParameter (int parameterIndex, float newValue) override;

    /** Saving custom settings to XML. */
    virtual void saveCustomParametersToXml (XmlElement* parentElement) override;

    /** Load custom settings from XML*/
    virtual void loadCustomParametersFromXml() override;

    bool enable();  
    bool disable();  
    int readGraph();
    int initTFVars();
    TF_Buffer* readFile(char* fileName);
    
    
    float target[];        

    /** Optional method called every time the signal chain is refreshed or changed in any way.

        Allows the processor to handle variations in the channel configuration or any other parameter
        passed down the signal chain. The processor can also modify here the settings structure, which contains
        information regarding the input and output channels as well as other signal related parameters. Said
        structure shouldn't be manipulated outside of this method.
    */
    //void updateSettings();

private:

    // private members and methods go here
    //
    // e.g.:
    //
    // float threshold;
    // bool state;

    char file[200];

    TF_Graph* graph;
    TF_Session* session;
    TF_Status* status = TF_NewStatus();
    std::vector<TF_Output> inputs;
    std::vector<TF_Tensor*> input_values;
    std::vector<TF_Output> outputs;
    std::vector<TF_Tensor*> output_values;
    //std::vector<TF_Tensor*> output_values(3, NULL); //hard-coded for now...

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TfProcessor);
};


#endif  // TFPROCESSOR_H_INCLUDED
