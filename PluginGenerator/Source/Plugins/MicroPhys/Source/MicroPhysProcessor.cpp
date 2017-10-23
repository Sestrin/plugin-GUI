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
    // Parameter 0 code
    auto parameter0 = new Parameter ("Sample Rate", "Sample Rate", 1000, 10000, 2000, 0);
    parameter0->setEditorDesiredBounds (9,38,104,24);
    String parameter0description = "Set sampling rate (Hz)";
    parameter0->setDescription (parameter0description);
    parameters.add (parameter0);

    // Parameter 1 code
    auto parameter1 = new Parameter ("IP Address P1", "IP Address P1", 0, 999, 192, 1);
    parameter1->setEditorDesiredBounds (8,88,32,24);
    String parameter1description = "Set the IP Address of your MicroPhys";
    parameter1->setDescription (parameter1description);
    parameters.add (parameter1);

    // Parameter 2 code
    auto parameter2 = new Parameter ("IP Address P2", "IP Address P2", 0, 999, 168, 2);
    parameter2->setEditorDesiredBounds (48,88,32,24);
    parameters.add (parameter2);

    // Parameter 3 code
    auto parameter3 = new Parameter ("IP Address P3", "IP Address P3", 0, 999, 0, 3);
    parameter3->setEditorDesiredBounds (88,88,32,24);
    parameters.add (parameter3);

    // Parameter 4 code
    auto parameter4 = new Parameter ("IP Address P4", "IP Address P4", 0, 999, 100, 4);
    parameter4->setEditorDesiredBounds (128,88,32,24);
    parameters.add (parameter4);


    //[OPENEPHYS_PARAMETERS_SECTION_END]

    //Without a custom editor, generic parameter controls can be added
    //parameters.add(Parameter("thresh", 0.0, 500.0, 200.0, 0));
}


MicroPhysProcessor::~MicroPhysProcessor()
{
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

float MicroPhysProcessor::getSampleRate()
{
    return currentSampleRate;
}

float MicroPhysProcessor::getDefaultSampleRate()
{
    return currentSampleRate;
}
