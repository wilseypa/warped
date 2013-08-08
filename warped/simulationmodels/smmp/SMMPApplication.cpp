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

#include "SMMPApplication.h"
#include "MemEvents.h"
#include "MemSourceObject.h"
#include "MemRouterObject.h"
#include "SMMPForkObject.h"
#include "SMMPJoinObject.h"
#include "SMMPServerObject.h"
#include "SMMPQueueObject.h"
#include "SMMPPartitioner.h"
#include <warped/PartitionInfo.h>
#include <warped/DeserializerManager.h>

#include <tclap/CmdLine.h>
#include <vector>
#include <iostream>
#include <fstream>
using namespace std;
using std::string;

#define NUMPER 6

SMMPApplication::SMMPApplication()
  : inputFileName( "" ),
    numObjects( 0 ){}

int
SMMPApplication::initialize( vector<string> &arguments ){
  try {
    TCLAP::CmdLine cmd("SMMP Simulation");

    TCLAP::ValueArg<string> inputFileNameArg("", "simulate", "SMMPSim configuration file name",
                                              true, inputFileName, "string", cmd);

    cmd.parse(arguments);

    inputFileName = inputFileNameArg.getValue();
  } catch (TCLAP::ArgException &e) {
      std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
      exit(-1);
  }

  if( inputFileName.empty() ){
    std::cerr << "An SMMPSim configuration file must be specified using --simulate" << std::endl;
    exit(-1);
  }

  return 0;
}

int
SMMPApplication::getNumberOfSimulationObjects(int mgrId) const {
  return numObjects;
}

const PartitionInfo *
SMMPApplication::getPartitionInfo( unsigned int numberOfProcessorsAvailable ){
  SMMPPartitioner *myPartitioner = new SMMPPartitioner();
  int numProcs;
  int cacheSpeed;
  double cacheHitRatio;
  int mainSpeed;
  int numReqPerProc;

  ifstream configFile;
  configFile.open( inputFileName.c_str() );

  if(configFile.fail()){
    cerr << "Could not open file: '" << inputFileName << "'" << endl;
    cerr << "Terminating simulation." << endl;
    abort();
  }

  configFile >> numProcs >> cacheSpeed >> cacheHitRatio >> mainSpeed >> numReqPerProc;
  configFile.close();

  // These names will be used for each object type. A number will be appended to create
  // unique names.
  string sourceName = "Source.";
  string join1Name = "Join1.";
  string queueName = "Queue.";
  string forkName = "Fork.";
  string serverName = "Server.";
  string join2Name = "Join2.";

  // This is the name of the final join object that is used for the main memory.
  stringstream strstm;
  strstm << numProcs * NUMPER;
  string finalJoin = "Join.F";

  // Used to initialize the seeds for the objects.
  double seed = 0;

  vector<SimulationObject*> *procObjs;

  // For each processor in the simulation, initialize 6 objects.
  for(int p = 0; p < numProcs; p++){
    procObjs = new vector<SimulationObject*>;  
    stringstream out;
    out << p;
    string curProc = out.str();

    MemSourceObject *source = new MemSourceObject(sourceName + curProc, numReqPerProc);
    source->setDestination(join1Name + curProc);
    procObjs->push_back(source);

    SMMPJoinObject *join1 = new SMMPJoinObject(join1Name + curProc, queueName + curProc);
    procObjs->push_back(join1);

    SMMPQueueObject *queue = new SMMPQueueObject(queueName + curProc, forkName + curProc);
    procObjs->push_back(queue);

    vector<string> outputs(2,"");
    outputs[0] = serverName + curProc;    
    outputs[1] = finalJoin;
    SMMPForkObject *fork = new SMMPForkObject(forkName + curProc, seed++, cacheHitRatio, outputs);
    procObjs->push_back(fork);

    SMMPServerObject *server = new SMMPServerObject(serverName + curProc, join2Name + curProc, seed++);
    server->setServerDistribution(FIXED, cacheSpeed);
    procObjs->push_back(server);

    SMMPJoinObject *join2 = new SMMPJoinObject(join2Name + curProc, sourceName + curProc);
    procObjs->push_back(join2);

    // Add the group of objects to the partition information.
    myPartitioner->addObjectGroup(procObjs);
  }

  // Then initialize the final 4 objects for the main memory.
  procObjs = new vector<SimulationObject*>;  
  string finalQueue = "Queue.F";
  string finalServer = "Server.F";
  string finalMemRouter = "MemRouter.F";

  SMMPJoinObject *joinF = new SMMPJoinObject(finalJoin, finalQueue);
  procObjs->push_back(joinF);

  SMMPQueueObject *queueF = new SMMPQueueObject(finalQueue, finalServer);
  procObjs->push_back(queueF);

  SMMPServerObject *serverF = new SMMPServerObject(finalServer, finalMemRouter, seed++);
  serverF->setServerDistribution(FIXED, mainSpeed);
  procObjs->push_back(serverF);

  MemRouterObject *memRouterF = new MemRouterObject(finalMemRouter);
  procObjs->push_back(memRouterF);

  // Add the group of objects to the partition information.
  myPartitioner->addObjectGroup(procObjs);

  // Perform the actual partitioning of groups.
  const PartitionInfo *retval = myPartitioner->partition( NULL, numberOfProcessorsAvailable );

  return retval;
}

int
SMMPApplication::finalize(){
  return 0;
}

void
SMMPApplication::registerDeserializers(){
  DeserializerManager::instance()->registerDeserializer( MemRequest::getMemRequestDataType(),
							 &MemRequest::deserialize );
}