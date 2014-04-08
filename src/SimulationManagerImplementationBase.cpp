#include <algorithm>                    // for for_each
#include <functional>                   // for unary_function
#include <iostream>                     // for cerr
#include <string>

#include "SimulationManagerImplementationBase.h"
#include "SimulationObject.h"           // for SimulationObject

class Event;
class VTime;

using std::unary_function;
using std::for_each;
using std::cerr;
using std::endl;
using std::vector;

std::ostream* wout;
std::ostream* werr;

SimulationManagerImplementationBase::SimulationManagerImplementationBase()
    : numberOfSimulationManagers(0),
      simObjectsByName(0),
      simObjectsByID(0) {}

SimulationManagerImplementationBase::~SimulationManagerImplementationBase() {
}

class InitObject : public unary_function < SimulationObject*,
        void > {
public:
    void operator()(SimulationObject* toInit) {
        toInit->initialize();
    }
};

void
SimulationManagerImplementationBase::initializeObjects() {
    //Obtains all the objects from simObjectsByName
    vector<SimulationObject*>* objects = getElementVector(simObjectsByName);
    InitObject initObject;
    for_each< vector<SimulationObject*>::iterator, InitObject >(objects->begin(),
                                                                objects->end(),
                                                                initObject);
    delete objects;
}

void
SimulationManagerImplementationBase::setNumberOfObjects(unsigned int numObjects) {
    numberOfObjects = numObjects;
}

void
SimulationManagerImplementationBase::finalizeObjects() {
    //Obtains all the objects from simObjectsByName
    vector<SimulationObject*>* objects = getElementVector(simObjectsByName);

    for (unsigned int i = 0; i < objects->size(); i++) {
        (*objects)[i]->finalize();
    }
    delete objects;
}

///Converts vector to a std::map(string ObjectName, SimulationObject *)
SimulationManagerImplementationBase::typeSimMap*
SimulationManagerImplementationBase::partitionVectorToHashMap(vector<SimulationObject*>* vector) {
    typeSimMap* retval = new typeSimMap;
    for (unsigned int i = 0; i < vector->size(); i++) {
        retval->insert(std::make_pair((*vector)[i]->getName(), (*vector)[i]));
    }
    return retval;
}
