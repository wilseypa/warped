#include "LogicGatesApplication.h"
#include "LogicEvent.h"
#include "NInputAndGate.h"
#include "NInputOrGate.h"
#include <PartitionInfo.h>
#include <RoundRobinPartitioner.h>
#include <DeserializerManager.h>

#include <vector>
#include <iostream>
#include <fstream>

using namespace std;
using std::string;

LogicGatesApplication::LogicGatesApplication()
  :inputFileName(""),
   numObjects(0){}

int
LogicGatesApplication::getNumberOfSimulationObjects(int mgId) const {
  return numObjects;
}

vector<SimulationObject *> *
LogicGatesApplication::getSimulationObjects(){
  string andobj_1 = "and_1";
  string andobj_2 = "and_2";
  string orobj = "or_1";
 

  vector <string> and_1_des,and_2_des;
  string and_1_desname = "or_1";
  string and_2_desname = "or_1";
  and_1_des.push_back(and_1_desname);
  and_2_des.push_back(and_2_desname);
  vector<string> *pt_1,*pt_2;
  pt_1 = &and_1_des;
  pt_2 = &and_2_des;

  vector <int>  numInputPorts; 
  numInputPorts.push_back(2);
  vector <int> *pt_3; 
  pt_3 = &numInputPorts;

  vector <int> numfanouts; 
  numInputPorts.push_back(1);
  vector <int> *pt_4; 
  pt_4 = &numfanouts;

 
  
  vector<SimulationObject *>*retval = new vector<SimulationObject *>;
  NInputAndGate *AND_1 = new NIputAndGate(and_1,2,1,pt_1,pt_3,pt_4,
                         1);
  NInputAndGate *AND_2 = new NIputAndGate(and_2,2,1,pt_2,pt_3,pt_4,
                         1);
  NInputOrGate *OR = new NIputOrGate(or_1,2,1,NULL,NULL,NULL,1);
  retval -> push_back(*AND_1);
  retval -> push_back(*AND_2);
  retval -> push_back(*OR);
  
  return retval;
}

const PartitionInfo *
LogicGatesApplication::*getPartitionInfo(unsigned int numberOfProcessorAvailable){
  const PartitionInfo *retval = 0;
  
  Partitioner *myPartitioner = new RoundRobinPartitioner();
  vector<SimulationObject *> *objects = getSimulationObjects();
  retval = myPartitioner->partition(objects, numberOfProcessorsAvailable);
  delete Objects;

  return retval;

}

int
LogicGatesApplication::finalize(){
  return 0;
}

void 
LogicGatesApplication::registerDeserializers(){  
  DeserializerManager::instance()->registerDeserializer(LogicEvent::getLogicEventDataType(),&LogicEvent::deserialize);
}

ArgumentParser &
LogicGatesApplication::getArgument
