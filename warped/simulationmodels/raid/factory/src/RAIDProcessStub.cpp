// Copyright (c) The University of Cincinnati.  
// All rights reserved.

// UC MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE SUITABILITY OF 
// THE SOFTWARE, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
// TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
// PARTICULAR PURPOSE, OR NON-INFRINGEMENT.  UC SHALL NOT BE LIABLE
// FOR ANY DAMAGES SUFFERED BY LICENSEE AS A RESULT OF USING,
// RESULT OF USING, MODIFYING OR DISTRIBUTING THIS SOFTWARE OR ITS
// DERIVATIVES.

// By using or copying this Software, Licensee agrees to abide by the
// intellectual property laws, and all other applicable laws of the
// U.S., and the terms of this license.

// Authors: Malolan Chetlur             mal@ececs.uc.edu
//          Jorgen Dahl                 dahlj@ececs.uc.edu
//          Dale E. Martin              dmartin@ececs.uc.edu
//          Radharamanan Radhakrishnan  ramanan@ececs.uc.edu
//          Dhananjai Madhava Rao       dmadhava@ececs.uc.edu
//          Philip A. Wilsey            phil.wilsey@uc.edu


#include "../include/RAIDProcess.h"
#include "../include/RAIDProcessStub.h"
#include "SimulationObject.h"
#include "../include/RAIDRequest.h"

SimulationObject*
RAIDProcessStub::createSimulationObject(int numberOfArguments,
                                       ostringstream &argumentStream) {
  RAIDProcess *newObject;
  
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
  
  string myName;
  int nrOutputs;
  int dummyOutPortNr;
  string outName;
  int dummyInPortNrOfDest;
  int maxdisks;
  DISK_TYPE disk;
  int maxrequests;
  int firstdisk;

  // Break out the input stream into separate fields
  inputStream >> myName >> nrOutputs >> dummyOutPortNr >>
    outName >> dummyInPortNrOfDest >> maxdisks;

  string diskType;
  inputStream >> diskType;

  inputStream >> maxrequests >> firstdisk;

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

  newObject = new RAIDProcess(myName, outName, maxdisks, disk, 
			      maxrequests, firstdisk, 0);

 // delete [] argumentStream.str();

  return (SimulationObject *) newObject;
}
