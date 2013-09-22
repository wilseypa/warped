#include "DFFApplication.h"
#include "DFF.h"
#include "LogicEvent.h"
#include "FileReaderWriter.h"
#include "NInputNandGate.h"
#include "NotGate.h"
#include "warped/PartitionInfo.h"
#include "warped/RoundRobinPartitioner.h"
#include "warped/DeserializerManager.h"

#include <vector>
#include <iostream>
#include <fstream>
#include "stdlib.h"

using namespace std;

DFFApplication::DFFApplication(string inputFileName, int numObjects)
    : inputFileName(inputFileName),
      numObjects(numObjects) {}

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
