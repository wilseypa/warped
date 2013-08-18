
#include "HalfAdderStub.h"
#include "HalfAdder.h"
#include "SimulationObject.h"

SimulationObject*
HalfAdderStub::createSimulationObject(int numberOfArguments,
                                      ostrstream &argumentStream) {

   if(numberOfArguments < 3){
      cerr << "Invalid number of parameters for HalfAdder" << endl;
      cerr << "Expected at least: 3 but got " << numberOfArguments << endl;
      cerr << "Argument Stream = " << argumentStream.str() << endl;
      delete [] argumentStream.str();
      exit(-1);
   }
  
   istrstream inputStream(argumentStream.str());

   string firstArgument;
   int numObjects; // Number of output objects

   string thirdArgument; // a sequence of dest object names and their ports
   int inputPortNumber;
   int outputPortNumber;
   
   int objDelay; // the delay for this object

   // break out the input stream into separate fields
   inputStream >> firstArgument >> numObjects;

   vector<string> *outputObjectNames = new vector<string>;
   vector<int> *destinationPorts = new vector<int>;
   vector<int> *fanOutSize = new vector<int>;
   
   for(int i = 0; i < numObjects; i++){
      inputStream >> outputPortNumber >> thirdArgument >> inputPortNumber;
      (*fanOutSize)[outputPortNumber]++;
      outputObjectNames->push_back(thirdArgument);
      destinationPorts->push_back(inputPortNumber);
   }
   
   inputStream >> objDelay;

   SimulationObject *newObject = new HalfAdder(firstArgument,
                                               numObjects,
                                               outputObjectNames,
                                               destinationPorts,
                                               fanOutSize, objDelay);
   delete [] argumentStream.str();
   
   return (SimulationObject *) newObject;
}


