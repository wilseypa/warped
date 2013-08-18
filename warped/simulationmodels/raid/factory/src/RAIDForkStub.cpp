#include "../include/RAIDFork.h"
#include "../include/RAIDForkStub.h"
#include "SimulationObject.h"

SimulationObject*
RAIDForkStub::createSimulationObject(int numberOfArguments,
                                       ostringstream &argumentStream) {
  RAIDFork *newObject;
  
  /* NR OF REQUIRED OBJECTS NOT YET DETERMINED!
     if (numberOfArguments < 6) {
       cerr << "Invalid number of parameters for Process" << endl;
       cerr << "Minimum is 6 but got " << numberOfArguments << endl;
       cerr << "Argument Stream = " << argumentStream.str() << endl;
       delete [] argumentStream.str();
       exit(-1);
     }
  */

  istringstream inputStream(argumentStream.str());
  
  string name;
  int nrOutputs;
  int dummyOutPortNr;
  vector<string> outputNames;
  int dummyInPortNrOfDest;
  int disks;
  int startDisk;

  // Break out the input stream into separate fields
  inputStream >> name >> nrOutputs;

  // Pack the outputNames vector with the output names. 
  int iter = 0;
  string tmp;
  while (iter < nrOutputs) {
    inputStream >> dummyOutPortNr >> tmp >> dummyInPortNrOfDest;
    outputNames.push_back(tmp);
    iter++;
  }
  inputStream >> disks >> startDisk;

  newObject = new RAIDFork(name, nrOutputs, outputNames, disks, startDisk);

  //delete [] argumentStream.str();

  return (SimulationObject *) newObject;
}
