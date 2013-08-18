#include "NInputAndGateStub.h"
#include "NInputAndGate.h"
#include "SimulationObject.h"

SimulationObject*
NInputAndGateStub::createSimulationObject(int numberOfArguments,
                                            ostrstream &argumentStream) {

   if(numberOfArguments < 3){
      cerr << "Invalid number of parameters for NInputAndGate" << endl;
      cerr << "Expected at least: 3 but got " << numberOfArguments << endl;
      cerr << "Argument Stream = " << argumentStream.str() << endl;
      delete [] argumentStream.str();
      exit(-1);
   }

   istrstream inputStream(argumentStream.str());
  
   string objName;
   int numberOfOutputs; // Number of output objects
   string destObjName;
   int inputPortNumber;
   int outputPortNumber;
   int numberOfInputs;
   int objDelay;

    // break out the input stream into separate fields
   inputStream >> objName >> numberOfOutputs;
   
   vector<string> *outputObjectNames = new vector<string>;
   vector<int> *destinationPorts = new vector<int>;
   vector<int> *fanOutSize = new vector<int>(numberOfOutputs, 0);

   for(int i = 0; i < numberOfOutputs; i++){
      inputStream >> outputPortNumber >> destObjName >> inputPortNumber;
      (*fanOutSize)[outputPortNumber - 1]++;
      outputObjectNames->push_back(destObjName);
      destinationPorts->push_back(inputPortNumber);
   }
   
   inputStream >> numberOfInputs >> objDelay;

   SimulationObject *newObject = new NInputAndGate(objName,
                                                   numberOfInputs,
                                                   numberOfOutputs,
                                                   outputObjectNames,
                                                   destinationPorts,
                                                   fanOutSize,
                                                   objDelay);
   delete [] argumentStream.str();

   return (SimulationObject *) newObject;
}


