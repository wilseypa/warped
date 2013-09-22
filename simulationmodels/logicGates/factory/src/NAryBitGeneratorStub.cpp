
#include "NAryBitGeneratorStub.h"
#include "NAryBitGenerator.h"
#include "SimulationObject.h"

SimulationObject*
NAryBitGeneratorStub::createSimulationObject(int numberOfArguments,
                                            ostrstream &argumentStream){

  if(numberOfArguments < 3){
      cerr << "Invalid number of parameters for NAryBitGenerator" << endl;
      cerr << "Expected at least: 3 but got " << numberOfArguments << endl;
      cerr << "Argument Stream = " << argumentStream.str() << endl;
      delete [] argumentStream.str();
      exit(-1);
   }

  cout << "string is " << argumentStream.str() << endl;
  
  istrstream inputStream(argumentStream.str());

  string firstArgument;
  int numObjects; // Number of output objects

  string thirdArgument; // a sequence of dest object names and their ports
  int inputPortNumber;
  int outputPortNumber;
   
  int numIterations; // the number of iterations for this object
  int numBits;       // the number of bits to generate
  int objDelay;      // the delay for this object
  string printStatus;
  bool status;

  // break out the input stream into separate fields
  inputStream >> firstArgument >> numObjects;

   vector<string> *outputObjectNames = new vector<string>;
   vector<int> *destinationPorts = new vector<int>;
   vector<int> *fanOutSize = new vector<int>(numObjects, 0);
   
   for(int i = 0; i < numObjects; i++){
      inputStream >> outputPortNumber >> thirdArgument >> inputPortNumber;
      (*fanOutSize)[outputPortNumber-1]++;
      outputObjectNames->push_back(thirdArgument);
      destinationPorts->push_back(inputPortNumber);
   }
   
   inputStream >> numIterations >> numBits >> objDelay >> printStatus;

   if(printStatus == "true"){
      status = true;
   }
   else {
      status = false;
   }
   
   SimulationObject *newObject = new NAryBitGenerator(firstArgument,
                                                      numObjects,
                                                      outputObjectNames,
                                                      destinationPorts,
                                                      fanOutSize,
                                                      numIterations,
                                                      numBits, objDelay,
                                                      status);
   delete [] argumentStream.str();
   
   return (SimulationObject *) newObject;
}


