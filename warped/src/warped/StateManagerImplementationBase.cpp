
#include "StateManagerImplementationBase.h"
#include "SimulationObject.h"
#include "State.h"
#include "OptFossilCollManager.h"

using std::cerr;
using std::endl;

StateManagerImplementationBase::StateManagerImplementationBase( TimeWarpSimulationManager *simMgr, 
								unsigned int period) :
  mySimulationManager(simMgr), 
  statePeriod(period),
  myStateQueue( new multiset< SetObject<State> >[ simMgr->getNumberOfSimulationObjects() ] ),
  objectStatePeriod( simMgr->getNumberOfSimulationObjects(), period),
  periodCounter( simMgr->getNumberOfSimulationObjects(), -1 ){
}

StateManagerImplementationBase::~StateManagerImplementationBase(){
  delete [] myStateQueue;
}

void
StateManagerImplementationBase::saveState(const VTime &currentTime,
					  SimulationObject *object){
  cerr << "StateManagerImplementationBase::saveState called" << endl;
  cerr << "Exiting simulation ..." << endl;
  exit(-1);
}

unsigned int
StateManagerImplementationBase::getStatePeriod(){
  return statePeriod;
}

vector<unsigned int> 
StateManagerImplementationBase::getObjectStatePeriod() {
  return objectStatePeriod;
}

const VTime&
StateManagerImplementationBase::restoreState(const VTime &rollbackTime,
					     SimulationObject *object){

  // store this object's id temporarily
  OBJECT_ID *currentObjectID = object->getObjectID();
  unsigned int objId = currentObjectID->getSimulationObjectID();

  multiset< SetObject<State> >::iterator iter_begin =
    myStateQueue[objId].begin();

  // start from the end of the queue
  multiset< SetObject<State> >::iterator iter_end =
    myStateQueue[objId].end();

  if (iter_end != iter_begin){
    iter_end--;

    // restore current state to the last state in the state queue
    // that is less than the rollback time
    while( iter_end != iter_begin && (*iter_end).getMainTime() > rollbackTime){
      // No longer used for optimistic fossil collection.
      /*if(mySimulationManager->getOptFossilColl()){
        mySimulationManager->getFossilCollManager()->deleteState((*iter_end).getElement(), objId);
      }
      else{
        object->deallocateState((*iter_end).getElement());
      }*/
      object->deallocateState((*iter_end).getElement());
      myStateQueue[objId].erase(iter_end--);
    }

    // at this point, the iterator points to the state we want to restore
    object->getState()->copyState((*iter_end).getElement());
    return (*iter_end).getMainTime();
        
  }
  else {
    // There are no states to be restored. If using optimistic fossil collection, rollback to a saved
    // checkpoint. This should never happen when not using optimistic fossil collection, so it will
    // result in a fatal error.
    if(mySimulationManager->getOptFossilColl()){
      if(!mySimulationManager->getRecoveringFromCheckpoint()){
        debug::debugout << mySimulationManager->getSimulationManagerID()
          << " - No states saved for this object. Rollback time: " << rollbackTime << endl;
        debug::debugout << "object" << *currentObjectID << ": Current Simulation Time is "
          << object->getSimulationTime() << endl;

        mySimulationManager->getFossilCollManager()->startRecovery(objId, rollbackTime.getApproximateIntTime());
      }
    }
    else{
      cerr << mySimulationManager->getSimulationManagerID()
        << " - No states saved for this object. Rollback time: " << rollbackTime << endl;
      cerr << "object" << *currentObjectID << ": Current Simulation Time is "
        << object->getSimulationTime() << endl;

      abort();
    }

    return mySimulationManager->getZero();
  }
}

const VTime&
StateManagerImplementationBase::fossilCollect( SimulationObject *object,
                                               const VTime &fossilCollectTime ){
  // store this object's id temporarily
  OBJECT_ID *objID = object->getObjectID();

  if(!myStateQueue[objID->getSimulationObjectID()].empty()){
    if( fossilCollectTime != mySimulationManager->getPositiveInfinity() ){
      // construct my search key
      SetObject<State> searchObject(fossilCollectTime);

      // get a handle to the beginning of this queue
      multiset< SetObject<State> >::iterator iter_begin =
        myStateQueue[objID->getSimulationObjectID()].begin();
   
      // find the last state upto which fossil collection will take place.
      // note: one state older than fossilCollectTime will be kept in the
      // queue since we can rollback to GVT. this is our marker.
      multiset< SetObject<State> >::iterator iter_end =
        myStateQueue[objID->getSimulationObjectID()].lower_bound(searchObject);

      if (iter_end != iter_begin) {
        --iter_end;
      }

      while(iter_begin != iter_end){
        object->deallocateState((*iter_begin).getElement());
        myStateQueue[objID->getSimulationObjectID()].erase(iter_begin++);
      }
    }
    else {
      // walk from the front of the queue and delete everything
      multiset< SetObject<State> >::iterator iter_begin =
        myStateQueue[objID->getSimulationObjectID()].begin();
      multiset< SetObject<State> >::iterator iter_end =
        myStateQueue[objID->getSimulationObjectID()].end();
      while(iter_begin != iter_end){
        object->deallocateState((*iter_begin).getElement());
        
        myStateQueue[objID->getSimulationObjectID()].erase(iter_begin++);
      }
    }
  } // else do nothing

  // Return the lowest timestamped state.
  multiset< SetObject<State> >::iterator iter_begin =
    myStateQueue[objID->getSimulationObjectID()].begin();
  if (!myStateQueue[objID->getSimulationObjectID()].empty()) {
    return (*iter_begin).getMainTime(); 
  }
  else {
    return mySimulationManager->getPositiveInfinity();
  }
}

const VTime&
StateManagerImplementationBase::fossilCollect( SimulationObject *object,
                                               int fossilCollectTime ){
  // store this object's id temporarily
  OBJECT_ID *objID = object->getObjectID();

  if(!myStateQueue[objID->getSimulationObjectID()].empty()){
    if( fossilCollectTime != mySimulationManager->getPositiveInfinity().getApproximateIntTime() ){
      // get a handle to the beginning of this queue
      multiset< SetObject<State> >::iterator iter_begin =
        myStateQueue[objID->getSimulationObjectID()].begin();
   
      // find the last state upto which fossil collection will take place.
      // note: one state older than fossilCollectTime will be kept in the
      // queue since we can rollback to GVT. this is our marker.
      multiset< SetObject<State> >::iterator iter_end =
        myStateQueue[objID->getSimulationObjectID()].end();

      while(iter_begin != iter_end && iter_begin->getMainTime().getApproximateIntTime() < fossilCollectTime){
        object->deallocateState((*iter_begin).getElement());
        myStateQueue[objID->getSimulationObjectID()].erase(iter_begin++);
      }
    }
    else {
      // walk from the front of the queue and delete everything
      multiset< SetObject<State> >::iterator iter_begin =
        myStateQueue[objID->getSimulationObjectID()].begin();
      multiset< SetObject<State> >::iterator iter_end =
        myStateQueue[objID->getSimulationObjectID()].end();
      while(iter_begin != iter_end){
        object->deallocateState((*iter_begin).getElement());
        
        myStateQueue[objID->getSimulationObjectID()].erase(iter_begin++);
      }
    }
  } // else do nothing

  // Return the lowest timestamped state.
  multiset< SetObject<State> >::iterator iter_begin =
    myStateQueue[objID->getSimulationObjectID()].begin();
  if (!myStateQueue[objID->getSimulationObjectID()].empty()) {
    return (*iter_begin).getMainTime(); 
  }
  else {
    return mySimulationManager->getPositiveInfinity();
  }
}
void
StateManagerImplementationBase::printStateQueue(const VTime &currentTime,
                                                SimulationObject *object,
                                                std::ostream &out){
  // store this object's id temporarily
  OBJECT_ID *currentObjectID = object->getObjectID();
   
  if(!myStateQueue[currentObjectID->getSimulationObjectID()].empty()){
      
    // get a handle to the beginning of this queue
    multiset< SetObject<State> >::iterator iter_begin =
      myStateQueue[currentObjectID->getSimulationObjectID()].begin();
      
    // get a handle to the end of this queue
    multiset< SetObject<State> >::iterator iter_end =
      myStateQueue[currentObjectID->getSimulationObjectID()].end();
      
    // iterate through the queue and print each state's time
    out << "object " << currentObjectID->getSimulationObjectID()
        << "@(" << currentTime << "): ";
    while (iter_begin != iter_end){
      out << *iter_begin << " ";
      ++iter_begin;
    }
    out << endl;
  }
}

void
StateManagerImplementationBase::ofcPurge(){
  multiset< SetObject<State> >::iterator it;
  for(int i = 0; i < mySimulationManager->getNumberOfSimulationObjects(); i++){
    it = myStateQueue[i].begin(); 
    while(it != myStateQueue[i].end()){
      delete (*it).getElement();
      myStateQueue[i].erase(it++);
    }
    periodCounter[i] = -1;
  }
}

void
StateManagerImplementationBase::ofcPurge(unsigned int objId){
  multiset< SetObject<State> >::iterator it;

  it = myStateQueue[objId].begin();
  while(it != myStateQueue[objId].end()){
    delete (*it).getElement();
    myStateQueue[objId].erase(it++);
  }
  periodCounter[objId] = -1;
}
