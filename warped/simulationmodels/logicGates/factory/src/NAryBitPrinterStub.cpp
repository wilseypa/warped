
#include "warped.h"
#include "NAryBitPrinter.h"
#include "NAryBitPrinterStub.h"
#include "SimulationObject.h"

SimulationObject*
NAryBitPrinterStub::createSimulationObject(int numberOfArguments,
                                           ostrstream &argumentStream){
   if(numberOfArguments != 3){
      cerr << "Invalid number of parameters for NAryBitPrinter" << endl;
      cerr << "Expected: 3 but got " << numberOfArguments << endl;
      cerr << "Argument Stream = " << argumentStream.str() << endl;
      delete [] argumentStream.str();
      exit(-1);
  }

  istrstream inputStream(argumentStream.str());
  
  string firstArgument; // name of this object
  int secondArgument;   // Number of output objects
  int outputPortNumber; // output port of this object
  string destObjName;   // name of the destination object
  int inputPortNumber;  // name of input port of destination object
  int numBits;
  string printStatus;     // to print or not to print
  bool status;
  

  // break out the input stream into separate fields
  inputStream >> firstArgument >> secondArgument;

  vector<string> *outputObjectNames = new vector<string>;
  vector<int> *destinationPorts = new vector<int>;
  vector<int> *fanOutSize = new vector<int>(secondArgument, 0);

  for(int i = 0; i < secondArgument; i++){
     inputStream >> outputPortNumber >> destObjName >> inputPortNumber;
     (*fanOutSize)[outputPortNumber]++;
     outputObjectNames->push_back(destObjName);
     destinationPorts->push_back(outputPortNumber);
  }

  inputStream >> numBits >> printStatus;

  if(printStatus == "true"){
     status = true;
  }
  else {
     status = false;
  }
  

  SimulationObject *newObject = new NAryBitPrinter(firstArgument,
                                                   secondArgument,
                                                   outputObjectNames,
                                                   destinationPorts,
                                                   fanOutSize,
                                                   numBits,
                                                   status);

  delete [] argumentStream.str();

  return (SimulationObject *) newObject;
}


