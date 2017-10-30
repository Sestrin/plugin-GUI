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
#include "c_api.h" 

#include "TfProcessor.h"
#include "TfProcessorEditor.h"

//If the processor uses a custom editor, it needs its header to instantiate it
//#include "ExampleEditor.h"


TfProcessor::TfProcessor()
    : GenericProcessor ("TF") //, threshold(200.0), state(true)
    , file("tensorflow_inception_graph.pb")
    

{
    setProcessorType (PROCESSOR_TYPE_FILTER);
    printf("Hello from TensorFlow C library version %s\n", TF_Version());
    readGraph();

    // Open Ephys Plugin Generator will insert generated code for parameters here. Don't edit this section.
    //[OPENEPHYS_PARAMETERS_SECTION_BEGIN]
    //[OPENEPHYS_PARAMETERS_SECTION_END]

    //Without a custom editor, generic parameter controls can be added
    //parameters.add(Parameter("thresh", 0.0, 500.0, 200.0, 0));
}


TfProcessor::~TfProcessor()
{
    TF_DeleteGraph(graph);
}


/**
  If the processor uses a custom editor, this method must be present.
*/
AudioProcessorEditor* TfProcessor::createEditor()
{
    editor = new TfProcessorEditor (this, true);

    //std::cout << "Creating editor." << std::endl;

    return editor;
}


void TfProcessor::setParameter (int parameterIndex, float newValue)
{
    GenericProcessor::setParameter (parameterIndex, newValue);
    editor->updateParameterButtons (parameterIndex);

    //Parameter& p =  parameters.getReference(parameterIndex);
    //p.setValue(newValue, 0);

    //threshold = newValue;

    //std::cout << float(p[0]) << std::endl;
}


void TfProcessor::process (AudioSampleBuffer& buffer, MidiBuffer& events)
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

    float data[] = {1.2, 1.3, 1.4};
    inference(data,graph);

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


void TfProcessor::saveCustomParametersToXml (XmlElement* parentElement)
{
    XmlElement* mainNode = parentElement->createNewChildElement ("TfProcessor");
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


void TfProcessor::loadCustomParametersFromXml()
{
    if (parametersAsXml == nullptr) // prevent double-loading
        return;

    // use parametersAsXml to restore state

    // Open Ephys Plugin Generator will insert generated code to load parameters here. Don't edit this section.
    //[OPENEPHYS_PARAMETERS_LOAD_SECTION_BEGIN]
    forEachXmlChildElement (*parametersAsXml, mainNode)
    {
        if (mainNode->hasTagName ("TfProcessor"))
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

bool TfProcessor::enable()
{
    printf("entered here\n");
    int res = readGraph();
    if (res)
        return false;
    else
        return true;    
}

int TfProcessor::readGraph()
{       
    TF_Buffer* graph_def = readFile(file);   
    printf("entered here1\n");                   
    //TF_Graph* graph = TF_NewGraph();
    graph = TF_NewGraph();
    printf("entered here2\n");        

      // Import graph_def into graph                                                          
    TF_Status* status = TF_NewStatus();                                                     
    TF_ImportGraphDefOptions* opts = TF_NewImportGraphDefOptions();                         
    printf("entered here3\n"); 
    TF_GraphImportGraphDef(graph, graph_def, opts, status); //seg fault
    printf("entered here4\n"); 
    TF_DeleteImportGraphDefOptions(opts);   

    if (TF_GetCode(status) != TF_OK) {
      fprintf(stderr, "ERROR: Unable to import graph %s", TF_Message(status));        
      return 1;
  }       
    fprintf(stdout, "Successfully imported graph");                                         
    TF_DeleteStatus(status);
    TF_DeleteBuffer(graph_def);                                                                 
    return 0;
}

void free_buffer(void* data, size_t length) {                                             
        free(data);                                                                       
}   

TF_Buffer* TfProcessor::readFile(char* fileName)
{
    //char fileName[] = "tensorflow_inception_graph.pb";
    FILE *f = fopen(fileName, "rb");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);                                                                  
    fseek(f, 0, SEEK_SET);  //same as rewind(f);                                            

    void* data = malloc(fsize);                                                             
    int readLength = fread(data, fsize, 1, f);
    printf("readLength: %d\n",readLength);
    fclose(f);

    TF_Buffer* buf = TF_NewBuffer();                                                        
    buf->data = data;
    buf->length = fsize;                                                                    
    buf->data_deallocator = free_buffer; 
    return buf;
}



void TfProcessor::inference(float* data,TF_Graph* tf_graph)
{

    target[0] = 5.1; //dummy code for now
}