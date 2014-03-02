
#include <set>                          // for multiset
#include <vector>                       // for vector

#include "DefaultObjectID.h"            // for OBJECT_ID
#include "ObjectID.h"                   // for ObjectID
#include "PeriodicStateManager.h"
#include "SetObject.h"                  // for SetObject
#include "SimulationObject.h"           // for SimulationObject
#include "State.h"                      // for State

class TimeWarpSimulationManager;
class VTime;

PeriodicStateManager::PeriodicStateManager(TimeWarpSimulationManager* simMgr,
                                           unsigned int period) :
    StateManagerImplementationBase(simMgr, period) {
}

PeriodicStateManager::~PeriodicStateManager() {
}

const VTime&
PeriodicStateManager::restoreState(const VTime& rollbackTime,
                                   SimulationObject* object) {
    OBJECT_ID* currentObjectID = object->getObjectID();
    unsigned int simObjectID = currentObjectID->getSimulationObjectID();
    //periodCounter[simObjectID] = objectStatePeriod[simObjectID];
    return StateManagerImplementationBase::restoreState(rollbackTime,object);
}

void
PeriodicStateManager::saveState(const VTime& currentTime,
                                SimulationObject* object) {

    // store this object's id temporarily
    OBJECT_ID* currentObjectID = object->getObjectID();
    unsigned int simObjectID = currentObjectID->getSimulationObjectID();

    if (periodCounter[simObjectID] <= 0) {
        // we need to first allocate a state. Copy the current state into
        // this newly created state. Then allocate a state object and
        // fill in the current time and a pointer to the newly copied
        // current state.

        State* newState = object->allocateState();
        // No longer used for optimistic fossil collection.
        /*if(mySimulationManager->getOptFossilColl()){
          newState = mySimulationManager->getFossilCollManager()->newState(object);
        }
        else{
          newState = object->allocateState();
        }*/
        newState->copyState(object->getState());

        SetObject<State> stateObject(currentTime, newState);

        // look up the state queue of the simulation object and insert the
        // newly allocated state object
        myStateQueue[simObjectID].insert(stateObject);
        // reset period counter to state period
        periodCounter[simObjectID] = objectStatePeriod[simObjectID];
    } else {
        // decrement period counter
        periodCounter[simObjectID] = periodCounter[simObjectID] - 1;
    }
}


