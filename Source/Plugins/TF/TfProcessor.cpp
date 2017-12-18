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
#include <assert.h>
#include <string.h>
#include "c_api.h" 

#include "TfProcessor.h"
#include "TfProcessorEditor.h"

//If the processor uses a custom editor, it needs its header to instantiate it
//#include "ExampleEditor.h"


TfProcessor::TfProcessor()
    : GenericProcessor ("TF") //, threshold(200.0), state(true)
    , file("zeke.pb")
    

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

    /*
    printf("gets to before TF_SessionRun\n");
    printf("inputs.size(): %d\n",inputs.size());
    printf("input_values.size(): %d\n",input_values.size());
    printf("outputs.size(): %d\n",outputs.size());
    printf("output_values.size(): %d\n",output_values.size());
    */

    TF_SessionRun(session, NULL,
                &inputs[0], &input_values[0], inputs.size(),
                &outputs[0], &output_values[0], outputs.size(),
                NULL, 0, NULL, status);
       
    
    assert(TF_GetCode(status) == TF_OK);  
    printf("gets past TF_SessionRun\n"); 
    
     
    float* out_vals = static_cast<float*>(TF_TensorData(output_values[0]));
    for (int i = 0; i < 3; ++i)
    {
        std::cout << "Output values info: " << *out_vals++ << "\n";
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
    int res2 = initTFVars();

    if (res + res2)
        return false;
    else
        return true;    
}

bool TfProcessor::disable()
{            
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
    status = TF_NewStatus();                                                     
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
    TF_DeleteBuffer(graph_def);                                                                 
    return 0;
}

void free_buffer(void* data, size_t length) {                                             
        free(data);                                                                       
}   

TF_Buffer* TfProcessor::readFile(char* fileName)
{
    //char fileName[] = "tensorflow_inception_graph.pb";
    printf("%s",fileName);
    FILE *f = fopen("zeke.pb", "rb");
    if (f == NULL)
        printf("file pointer is null\n");

    printf("gets past fopen\n");
    fseek(f, 0, SEEK_END);
    printf("gets past fseek\n");
    long fsize = ftell(f);      
    printf("gets past ftell\n");                                                            
    fseek(f, 0, SEEK_SET);  //same as rewind(f);                                            
    printf("gets past fseek2\n");
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

static void Deallocator(void* data, size_t length, void* arg) {
        free(data);
        // *reinterpret_cast<bool*>(arg) = true;
}

int TfProcessor::initTFVars()
{

  // Create variables to store the size of the input and output variables
  const int num_bytes_in = 32 * 10 * sizeof(float);
  const int num_bytes_out = 1 * sizeof(float);

  // Set input dimensions - this should match the dimensionality of the input in
  // the loaded graph
  int64_t in_dims[] = {1, 10, 32};
  int64_t out_dims[] = {1, 1};

  // ######################
  // Set up graph inputs
  // ######################

  // Create a variable containing your values
  float values[10][32];
  for (int i=0; i < 10; i++)
  {
    for (int j=0; j < 32; j++)
    {
      values[i][j] = 0;
    }
  }

  // Create vectors to store graph input operations and input tensors
  //std::vector<TF_Output> inputs;
  //std::vector<TF_Tensor*> input_values;
  /*std::vector<std::string> input_names;
  input_names.push_back("x_input");
  input_names.push_back("y_input");
  input_names.push_back("z_input");*/

  // Pass the graph and a string name of your input operation
  // (make sure the operation name is correct)
  //TF_Operation* input_op;
  //TF_Output input_opout;

  TF_Operation* input_op = TF_GraphOperationByName(graph, "x_input");
  TF_Output input_opout = {input_op, 0};
  inputs.push_back(input_opout);

  TF_Operation* input_op1 = TF_GraphOperationByName(graph, "y_input");
  TF_Output input_opout1 = {input_op1, 0};
  inputs.push_back(input_opout1);

  TF_Operation* input_op2 = TF_GraphOperationByName(graph, "z_input");
  TF_Output input_opout2 = {input_op2, 0};
  inputs.push_back(input_opout2);

  // Create the input tensor using the dimension (in_dims) and size (num_bytes_in)
  // variables created earlier
  TF_Tensor* input = TF_NewTensor(TF_FLOAT, in_dims, 3, values, num_bytes_in, &Deallocator, 0);
  input_values.push_back(input);

  TF_Tensor* input1 = TF_NewTensor(TF_FLOAT, in_dims, 3, values, num_bytes_in, &Deallocator, 0);
  input_values.push_back(input1);

  TF_Tensor* input2 = TF_NewTensor(TF_FLOAT, in_dims, 3, values, num_bytes_in, &Deallocator, 0);
  input_values.push_back(input2);

  // Optionally, you can check that your input_op and input tensors are correct
  // by using some of the functions provided by the C API.
  std::cout << "Input op info: " << TF_OperationName(input_op1) << "\n";
  std::cout << "Input data info: " << TF_NumDims(input1) << "\n";

  // ######################
  // Set up graph outputs (similar to setting up graph inputs)
  // ######################

  // Create vector to store graph output operations
  //std::vector<TF_Output> outputs;
  TF_Operation* output_op = TF_GraphOperationByName(graph, "par_out_0");
  TF_Output output_opout = {output_op, 0};
  outputs.push_back(output_opout);

  TF_Operation* output_op1 = TF_GraphOperationByName(graph, "par_out_1");
  TF_Output output_opout1 = {output_op1, 0};
  outputs.push_back(output_opout1);

  TF_Operation* output_op2 = TF_GraphOperationByName(graph, "par_out_2");
  TF_Output output_opout2 = {output_op2, 0};
  outputs.push_back(output_opout2);

  // Create TF_Tensor* vector
  //std::vector<TF_Tensor*> output_values(outputs.size(), NULL);

  // Similar to creating the input tensor, however here we don't yet have the
  // output values, so we use TF_AllocateTensor()
  TF_Tensor* output_value = TF_AllocateTensor(TF_FLOAT, out_dims, 2, num_bytes_out);  
  output_values.push_back(output_value);

  // As with inputs, check the values for the output operation and output tensor
  std::cout << "Output: " << TF_OperationName(output_op1) << "\n";
  std::cout << "Output info: " << TF_NumDims(output_value) << "\n";

  // ######################
  // Run graph
  // ######################
  fprintf(stdout, "Running session...\n");
  
  TF_SessionOptions* sess_opts = TF_NewSessionOptions();  
  session = TF_NewSession(graph, sess_opts, status);  
  assert(TF_GetCode(status) == TF_OK); 
  
  printf("gets to the end of TfProcessor\n");
  return 0; 

}