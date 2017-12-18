#include <string.h>
#include <iostream>
#include "c_api.h"

int main( int argc, char ** argv ) 
{
  TF_Graph * graph = TF_NewGraph();
  TF_SessionOptions * options = TF_NewSessionOptions();
  TF_Status * status = TF_NewStatus();
  TF_Session * session = TF_NewSession( graph, options, status );
  char hello[] = "Hello TensorFlow!";
  TF_Tensor * tensor = TF_AllocateTensor( TF_STRING, 0, 0, 8 + TF_StringEncodedSize( strlen( hello ) ) );
  TF_OperationDescription * operationDescription = TF_NewOperation( graph, "Const", "hello" );
  TF_Operation * operation; 
  struct TF_Output * output;

  TF_StringEncode( hello, strlen( hello ), 8 + ( char * ) TF_TensorData( tensor ), TF_StringEncodedSize( strlen( hello ) ), status );
  TF_SetAttrTensor( operationDescription, "value", tensor, status );
  TF_SetAttrType( operationDescription, "dtype", TF_TensorType( tensor ) );
  operation = TF_FinishOperation( operationDescription, status );

  output->oper = operation;
  output->index = 0;

  TF_SessionRun( session, 0,
                 0, 0, 0,  // Inputs
                 output, &tensor, 1,  // Outputs
                 &operation, 1,  // Operations
                 0, status );

  printf( "%i", TF_GetCode( status ) );

  TF_CloseSession( session, status );
  TF_DeleteSession( session, status );
  TF_DeleteStatus( status );
  TF_DeleteSessionOptions( options );  

  return 0;
}