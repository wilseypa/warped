// Copyright (c) Clifton Labs, Inc.
// All rights reserved.

// CLIFTON LABS MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE
// SUITABILITY OF THE SOFTWARE, EITHER EXPRESS OR IMPLIED, INCLUDING BUT
// NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
// PARTICULAR PURPOSE, OR NON-INFRINGEMENT.  CLIFTON LABS SHALL NOT BE
// LIABLE FOR ANY DAMAGES SUFFERED BY LICENSEE AS A RESULT OF USING, RESULT
// OF USING, MODIFYING OR DISTRIBUTING THIS SOFTWARE OR ITS DERIVATIVES.

// By using or copying this Software, Licensee agrees to abide by the
// intellectual property laws, and all other applicable laws of the
// U.S., and the terms of this license.

// Authors: Malolan Chetlur             mal@ececs.uc.edu
//          Jorgen Dahl                 dahlj@ececs.uc.edu
//          Dale E. Martin              dmartin@ececs.uc.edu
//          Radharamanan Radhakrishnan  ramanan@ececs.uc.edu
//          Dhananjai Madhava Rao       dmadhava@ececs.uc.edu
//          Philip A. Wilsey            phil.wilsey@uc.edu

//---------------------------------------------------------------------------
// 
// $Id: PingPongApplication.cpp,v 1.9 2003-02-18 21:31:06 dmartin Exp $
// 
//---------------------------------------------------------------------------

#include "PingPongApplication.h"
#include "PingObject.h"
#include "PingEvent.h"
#include <warped/PartitionInfo.h>
#include <warped/RoundRobinPartitioner.h>
#include <warped/DeserializerManager.h>

#include <tclap/CmdLine.h>
#include <vector>
using std::string;

PingPongApplication::PingPongApplication( unsigned int initNumObjects,
					  unsigned int initNumEventsPerObject,
					  unsigned int initNumBallsAtOnce,
					  bool initRandomDelays ) 
  : numObjects( initNumObjects ),
    numEventsPerObject( initNumEventsPerObject ),
    numBallsAtOnce( initNumBallsAtOnce ),
    randomDelays( initRandomDelays ){}

int 
PingPongApplication::initialize( vector<string> &arguments ){ 
  try {
    TCLAP::CmdLine cmd("Ping Pong Simulation");

    TCLAP::ValueArg<int> numBallsArg("", "numBalls", "number of balls", 
                                     false, numEventsPerObject, "int", cmd);
    TCLAP::ValueArg<int> numPlayersArg("", "numPlayers", "number of simulation objects to pass the balls", 
                                       false, numObjects, "int", cmd);
    TCLAP::ValueArg<int> numAtOnceArg("", "numAtOnce", "number of balls circulating at once", 
                                      false, numBallsAtOnce, "int", cmd);
    TCLAP::SwitchArg randomDelaysArg("", "randomDelays", "should the simulation objects wait random amounts of time", 
                                          cmd, false);

    cmd.parse(arguments);

    numEventsPerObject = numBallsArg.getValue();
    numObjects = numPlayersArg.getValue();
    numBallsAtOnce = numAtOnceArg.getValue();
    randomDelays = randomDelaysArg.getValue();

  } catch (TCLAP::ArgException &e) {
    std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
    exit(-1);
  }
  return 0;
}

int 
PingPongApplication::getNumberOfSimulationObjects(int mgrId) const { 
  return numObjects;
}

vector<SimulationObject *> *
PingPongApplication::getSimulationObjects(){
  vector<SimulationObject *> *retval = new vector<SimulationObject *>;
  retval->reserve( numObjects );

  // The first and last objects are special...
  int mastersLeft = numBallsAtOnce;
  bool master = false;
  for( unsigned int i = 0; i < numObjects - 1; i++ ){
    if( mastersLeft-- > 0 ){
      master = true;
    }
    else{
      master = false;
    }
    retval->push_back( new PingObject( i,
				       PingObject::getName( i + 1 ),
				       numEventsPerObject,
				       master,
				       randomDelays
				       ) );
  }
  if( mastersLeft-- > 0 ){
    master = true;
  }
  else{
    master = false;
  }
  retval->push_back( new PingObject( numObjects - 1,
				     PingObject::getName( 0 ),
				     numEventsPerObject,
				     master,
				     randomDelays ) );
  
  return retval;
}


const PartitionInfo *
PingPongApplication::getPartitionInfo( unsigned int numberOfProcessorsAvailable ){
  const PartitionInfo *retval = 0;

  Partitioner *myPartitioner = new RoundRobinPartitioner();
  // Now we'll create some simulation objects...
  vector<SimulationObject *> *objects = getSimulationObjects();
  retval = myPartitioner->partition( objects, numberOfProcessorsAvailable );
  delete objects;

  return retval;
}

int 
PingPongApplication::finalize(){ 
  return 0; 
}

void 
PingPongApplication::registerDeserializers(){
  DeserializerManager::instance()->registerDeserializer( PingEvent::getPingEventDataType(),
							 &PingEvent::deserialize );

}

