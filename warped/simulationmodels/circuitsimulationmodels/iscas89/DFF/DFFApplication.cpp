// Copyright (c) The University of Cincinnati.
// All rights reserved.

// UC MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE SUITABILITY OF
// THE SOFTWARE, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED
// TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
// PARTICULAR PURPOSE, OR NON-INFRINGEMENT.  UC SHALL NOT BE LIABLE
// FOR ANY DAMAGES SUFFERED BY LICENSEE AS A RESULT OF USING,
// RESULT OF USING, MODIFYING OR DISTRIBUTING THIS SOFTWARE OR ITS
// DERIVATIVES.
									  
// By using or copying this Software, Licensee agrees to abide by the
// intellectual property laws, and all other applicable laws of the
// U.S., and the terms of this license.
								
//Authors: Xinyu Guo                   guox2@mail.uc.edu
//         Philip A. Wilsey            phil.wilsey@uc.edu

#include "DFFApplication.h"
#include "DFF.h"
#include "LogicEvent.h"
#include "FileReaderWriter.h"
#include "NInputNandGate.h"
#include "NotGate.h"
#include <warped/PartitionInfo.h>
#include <warped/RoundRobinPartitioner.h>
#include <warped/DeserializerManager.h>

#include <tclap/CmdLine.h>
#include "vector"
#include "iostream"
#include "fstream"
#include "stdlib.h"

using namespace std;

DFFApplication::DFFApplication()
  :inputFileName(""),
    numObjects(0){}

int
DFFApplication::initialize( vector<string> &arguments ){
  try {
    TCLAP::CmdLine cmd("RAID Simulation");

    TCLAP::ValueArg<string> inputFileNameArg("", "simulate", "DFF configuration file name",
                                              true, inputFileName, "string", cmd);

    cmd.parse(arguments);

    inputFileName = inputFileNameArg.getValue();
  } catch (TCLAP::ArgException &e) {
      std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
      exit(-1);
  }

  return 0;
}

int 
DFFApplication::finalize(){
  return 0;
}

int
DFFApplication::getNumberOfSimulationObjects(int mgrId) const {
  return numObjects;
}

const PartitionInfo*
DFFApplication::getPartitionInfo(unsigned int numberOfProcessorsAvailable){
  const PartitionInfo *retval = 0;
  Partitioner *myPartitioner = new RoundRobinPartitioner();
  vector<SimulationObject *> *objects = new vector <SimulationObject *>;
  DFF *dff = new DFF();
  dff->createDFF();
  objects = dff->getDFFGates();
  retval = myPartitioner->partition( objects, numberOfProcessorsAvailable );
  delete dff;
  delete objects;
  
  return retval;

}

void
DFFApplication::registerDeserializers(){
	DeserializerManager::instance()->registerDeserializer(LogicEvent::getLogicEventDataType(),&LogicEvent::deserialize);
}
