// See copyright notice in file Copyright in the root directory of this archive.

#include "SimulationManagerImplementationBase.h"
#include "Application.h"
#include <functional>
#include <algorithm>

using std::unary_function;
using std::for_each;
using std::cerr;
using std::endl;

ostream *wout;
ostream *werr;

SimulationManagerImplementationBase::SimulationManagerImplementationBase()
  : numberOfSimulationManagers(0),
    localArrayOfSimObjPtrs(0),
    localArrayOfSimObjIDs(0){}

SimulationManagerImplementationBase::~SimulationManagerImplementationBase(){
}

class InitObject : public unary_function<SimulationObject *,
		   void> {
public:
  void operator()( SimulationObject *toInit ){
    toInit->initialize();
  }
};

void
SimulationManagerImplementationBase::initializeObjects(){
  //Obtains all the objects from localArrayOfSimObjPtrs
  vector<SimulationObject *> *objects = getElementVector(localArrayOfSimObjPtrs);
  InitObject initObject;
  for_each< vector<SimulationObject *>::iterator, InitObject >( objects->begin(), 
								 objects->end(),
								 initObject );
  delete objects;
}

void
SimulationManagerImplementationBase::setNumberOfObjects(unsigned int numObjects) {
  numberOfObjects = numObjects;
}

void
SimulationManagerImplementationBase::finalizeObjects(){
  //Obtains all the objects from localArrayOfSimObjPtrs
  vector<SimulationObject *> *objects = getElementVector(localArrayOfSimObjPtrs);

  for( unsigned int i = 0; i < objects->size(); i++ ){
    (*objects)[i]->finalize();
  }
  delete objects;
}

void
SimulationManagerImplementationBase::receiveEvent( Event *,
                                                   SimulationObject *,
                                                   SimulationObject * ){
  shutdown("ERROR: receiveEvent() called in  SimulationManagerImplementationBase" );
}

void
SimulationManagerImplementationBase::simulate( const VTime & ){
  shutdown( "ERROR: simulate() called in SimulationManagerImplementationBase" );
}


void
SimulationManagerImplementationBase::registerSimulationObjects(){
  shutdown( "ERROR: registerSimulationObjects() called in SimulationManagerImplementationBase" );
}

void 
SimulationManagerImplementationBase::unregisterSimulationObjects (vector <SimulationObject*>* list){
  // some tasks this function is responsible for
};

// print out the name to simulation object ptr map
void
SimulationManagerImplementationBase::displayObjectMap(ostream& out){

  if(!localArrayOfSimObjPtrs->empty()){
    //Obtains all the keys from localArrayOfSimObjPtrs
    vector<string> *keys = getKeyVector(localArrayOfSimObjPtrs);
    //Obtains all the objects from localArrayOfSimObjPtrs
    vector<SimulationObject *> *objects = getElementVector(localArrayOfSimObjPtrs);

    for( unsigned int i = 0; i < objects->size(); i++ ){
      out << (*keys)[i] << ": " << (*objects)[i]->getObjectID();
    }
    delete objects;
    delete keys;
  }
  else {
    out << "Object Names to Object Pointers Map is empty" << endl;
  }
}

///Converts vector to a std::map(string ObjectName, SimulationObject *)
SimulationManagerImplementationBase::typeSimMap *SimulationManagerImplementationBase::partitionVectorToHashMap(vector<SimulationObject *> *vector){
  typeSimMap *retval = new typeSimMap;
  for( unsigned int i = 0; i < vector->size(); i++ ){
    retval->insert( std::make_pair((*vector)[i]->getName(), (*vector)[i]) );
  }
  return retval;
}
