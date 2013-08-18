#include "../include/RAIDDisk.h"
#include "../include/RAIDDiskStub.h"
#include "SimulationObject.h"
#include "../include/RAIDRequest.h"

SimulationObject*
RAIDDiskStub::createSimulationObject(int numberOfArguments,
                                       ostringstream &argumentStream) {
  RAIDDisk *newObject;
  
  /* NUMBER OF REQUIRED ARGUMENTS NOT YET DETERMINED
    if (numberOfArguments < 6) {
      cerr << "Invalid number of parameters for Process" << endl;
      cerr << "Minimum is 6 but got " << numberOfArguments << endl;
      cerr << "Argument Stream = " << argumentStream.str() << endl;
      delete [] argumentStream.str();
      exit(-1);
    }
  */

  istringstream inputStream(argumentStream.str());
  
  string myName;
  int nrOutputs;
  DISK_TYPE disk;
  string diskType;

  // Break out the input stream into separate fields
  inputStream >> myName >> nrOutputs;
  inputStream >> diskType;

  // Convert the diskType string to the proper DISK_TYPE
  if (diskType == "FUJITSU") {
    disk = FUJITSU;
  }
  else if (diskType == "FUTUREDISK") {
    disk = FUTUREDISK;
  }
  else if (diskType == "LIGHTNING") {
    disk = LIGHTNING;
  }

  newObject = new RAIDDisk(myName, disk);

  //delete [] argumentStream.str();

  return (SimulationObject *) newObject;
}
