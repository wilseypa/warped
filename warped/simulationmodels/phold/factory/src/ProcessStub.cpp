// See copyright notice in file Copyright in the root directory of this archive.

#include "../include/Process.h"
#include "../include/ProcessStub.h"
#include "SimulationObject.h"
#include <iostream>
using namespace std;

SimulationObject*
ProcessStub::createSimulationObject(int numberOfArguments,
                                    ostringstream &argumentStream) {
  Process *newObject;
  
  if (numberOfArguments < 4) {
     cerr << "Invalid number of parameters for Process" << endl;
     cerr << "Minimum is 4 but got " << numberOfArguments << endl;
     cerr << "Argument Stream = " << argumentStream.str() << endl;
     exit(-1);
  }

  istringstream inputStream(argumentStream.str());
  
  string name;
  int nrOutputs;
  vector<string> outputNames;
  int stateSize = 1024;
  int numBalls;
  string distributionString; // distribution_t
  distribution_t dist;
  double seed;
  int procNr;

  // Break out the input stream into separate fields
  inputStream >> name >> nrOutputs;

  // Pack the outputNames vector with the output names. 
  int iter = 0;

  // before the dest object name, the output port is also present. For
  // PHOLD, we can ignore this; for other applications, we need to
  // store the output port number.
  int outputPortNumber;
  
  // right after the dest object name, the input port number is also
  // present.  For PHOLD, we can ignore this; for other applications,
  // we need to store the input port number.
  int inputPortNumber;
  
  string tmp;
  while (iter < nrOutputs) {
     inputStream >> outputPortNumber >> tmp >> inputPortNumber;
     outputNames.push_back(tmp);
     iter++;
  }
  inputStream >> numBalls >> distributionString;

  // Convert the distributionString to the proper distribution.
  if (distributionString == "UNIFORM") {
    dist = UNIFORM;
  } 
  else if (distributionString == "POISSON") {
    dist = POISSON;
  }
  else if (distributionString == "EXPONENTIAL") {
    dist = EXPONENTIAL;
  }
  else if (distributionString == "NORMAL") {
    dist = NORMAL;
  }
  else if (distributionString == "BINOMIAL") {
    dist = BINOMIAL;
  }
  else if (distributionString == "FIXED") {
    dist = FIXED;
  }
  else if (distributionString == "ALTERNATE") {
    dist = ALTERNATE;
  }
  else if (distributionString == "ROUNDROBIN") {
    dist = ROUNDROBIN;
  }
  else if (distributionString == "CONDITIONAL") {
    dist = CONDITIONAL;
  }
  else {
    dist = ALL;
  }
  inputStream >> seed >> procNr;

  newObject = new Process(procNr, name, nrOutputs, outputNames, stateSize,
			  numBalls, dist, 50, seed);

  return (SimulationObject *) newObject;
}
