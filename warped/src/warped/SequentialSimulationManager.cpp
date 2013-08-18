
#include <Debug/Debug.h>
#include "EventSetFactory.h"
#include "ObjectStub.h"
#include "SequentialSimulationManager.h"
#include "Application.h"
#include "warped.h"
#include "PartitionInfo.h"
#include "Event.h"

using std::cerr;
using std::cout;
using std::endl;

SequentialSimulationManager::SequentialSimulationManager( Application *initApplication ):
  SimulationManagerImplementationBase(),
  simulationTime( initApplication->getZero().clone() ), 
  sequentialWout( cerr.rdbuf(), ios::out), 
  sequentialWerr( cerr.rdbuf(), ios::out ),
  myApplication( initApplication ),
  totalSimulationTime( 0.0 ){
  numberOfObjects = initApplication->getNumberOfSimulationObjects(0);

  myEventSet = NULL;
  numberOfProcessedEvents = 0;

  // Initialize wout and werr to suitable sequential simulation streams...
  wout = &sequentialWout;
  werr = &sequentialWerr;
}

SequentialSimulationManager::~SequentialSimulationManager(){
  delete myEventSet;

  // The following code was causing a segmentation fault when the SimulationObject
  // was allocated out of a "non-standard" part of memory.  What it comes down to
  // is we didn't allocate them, so we should not destroy them.

//   // iteratively call delete on each simulation object pointer
//   for( typeSimMap::iterator iter = localArrayOfSimObjPtrs->begin();
//        iter != localArrayOfSimObjPtrs->end();
//        iter++ ){
//     SimulationObject *toDelete = (*iter).second;
//     cout << "Deleting " << *toDelete->getObjectID() << endl;
//     delete toDelete;
//   }
   
  delete localArrayOfSimObjPtrs;
}

const Event *
SequentialSimulationManager::getEvent(SimulationObject *object){
  ASSERT(myEventSet != NULL);
  const Event *event = myEventSet->getEvent();
  if(event != NULL){
    numberOfProcessedEvents++;
  }
  return event;
}

const Event *
SequentialSimulationManager::peekEvent(SimulationObject *object){
  ASSERT(myEventSet != NULL);
  return myEventSet->peekEvent();
}

void
SequentialSimulationManager::handleEvent( const Event *event ){
  ASSERT(event != NULL);
  ASSERT(myEventSet != NULL);
  myEventSet->insert(event);
}

void
SequentialSimulationManager::initialize(){
  cout << "Initializing simulation...";
  cout.flush();
  initializeWatch.start();
  // Register our objects
  registerSimulationObjects();
  initializeObjects();
  initializeWatch.stop();
  cout << "Done.\n";
}

void
SequentialSimulationManager::finalize(){
  cout << "Finalizing...\n";
  cout.flush();
  
  StopWatch finalizeWatch;

  finalizeWatch.start();
  finalizeObjects();
  myApplication->finalize();
  cout << "Done." << endl;
  finalizeWatch.stop();

  cout << "Simulation complete (" << numberOfProcessedEvents << " events in "
       << totalSimulationTime << " secs, "
       << (numberOfProcessedEvents/totalSimulationTime) << " events/sec).\n"
       << "Initalization - " << initializeWatch.elapsed() << " seconds\n"
       << "Finalization - " << finalizeWatch.elapsed() << " seconds\n";
}

void
SequentialSimulationManager::simulate( const VTime& simulateUntil ){
  debug::debugout << "Simulating from " << getSimulationTime() << " to " << simulateUntil << endl;

  StopWatch simulateWatch;
  simulateWatch.start();  
  const Event *nextEvent = myEventSet->peekEvent();
  while( nextEvent != NULL && (simulationTime == 0 || *simulationTime < simulateUntil )){
    // This if test guarantees that the simulation does not get ahead of the simulate until
    // time.  This is important for simbus, which may have an event show up unexpectedly
    // from the analog domain. --DNS
    if (nextEvent->getReceiveTime() > simulateUntil)
      break;
    setSimulationTime( nextEvent->getReceiveTime() );
    SimulationObject *object = getObjectHandle(nextEvent->getReceiver());
    object->setSimulationTime( nextEvent->getReceiveTime() );

    // call executeProcess on this object
    object->executeProcess();
       
    myEventSet->cleanUp();
    nextEvent = myEventSet->peekEvent();
  }
  simulateWatch.stop();
  totalSimulationTime += simulateWatch.elapsed();
}

void
SequentialSimulationManager::configure( SimulationConfiguration &configuration ){

  const EventSetFactory *tempEventSetFactory = EventSetFactory::instance();
  myEventSet = dynamic_cast<EventSet *>(tempEventSetFactory->allocate( configuration,
								       this ));
}

// some tasks this function is responsible for
// a. creating and storing the map of simulation object pointers
// b. assigning unique ids to each simulation object
// c. setting the simulation manager handle in each object
// d. configuring the state by calling allocateState on each object
// e. set the simulation manager pointer in each object
void 
SequentialSimulationManager::registerSimulationObjects (){
  // save the map of simulation object ptrs and object names
  localArrayOfSimObjPtrs = createMapOfObjects();

  if( localArrayOfSimObjPtrs == 0 ){
    shutdown( "Application returned null map of simulation objects - exiting" );
  }

  // allocate memory for our reverse map 
  localArrayOfSimObjIDs.resize(numberOfObjects);

  vector<SimulationObject *>::iterator iter;

  //Obtains all the objects from localArrayOfSimObjPtrs
  vector<SimulationObject *> *objects = getElementVector(localArrayOfSimObjPtrs);

  unsigned int count = 0;

  // now traverse the map and fill in simulation object info
  for( iter = objects->begin();
       iter != objects->end();
       iter++, count++ ){
    // create and store in the map a relation between ids and object names
    OBJECT_ID *id = new OBJECT_ID(count);
    SimulationObject *object = (*iter);
      
    // store this objects id for future reference
    object->setObjectID(id);

    // store a handle to our simulation manager in the object
    object->setSimulationManager(this);

    // lets allocate the initial state here
    object->setInitialState(object->allocateState());

    // save map of ids to names
    localArrayOfSimObjIDs[count] = object;
  }
  delete objects;

};

// this function constructs the map of simulation object names versus
// simulation object pointers by interacting with the application

SimulationManagerImplementationBase::typeSimMap *SequentialSimulationManager::createMapOfObjects(){
  typeSimMap *retval = 0;

  const PartitionInfo *appInfo =  myApplication->getPartitionInfo( 1 );

  if( appInfo->getNumberOfPartitions() != 1 ){
    cerr << "Application returned multiple partitions when the kernel requested 1" << endl;
    abort();
  }

  retval = partitionVectorToHashMap( appInfo->getObjectSet( 0 ) );

  setNumberOfObjects( retval->size() );

  delete appInfo;

  return retval;
}


SimulationStream*
SequentialSimulationManager::getIFStream(const string &fileName,
                                         SimulationObject *object){
  SequentialSimulationStream *simStream =
    new SequentialSimulationStream(fileName, ios::in);
  return simStream;
}

SimulationStream*
SequentialSimulationManager::getOFStream(const string &fileName,
                                         SimulationObject *object,
                                         ios::openmode mode){
  SequentialSimulationStream *simStream =
    new SequentialSimulationStream(fileName, mode);
  return simStream;
}

SimulationStream*
SequentialSimulationManager::getIOFStream(const string &fileName,
                                          SimulationObject *object){
  SequentialSimulationStream *simStream =
    new SequentialSimulationStream(fileName, ios::in|ios::app);
  return simStream;
}

const VTime &
SequentialSimulationManager::getCommittedTime(){
  return getSimulationTime();
}

const VTime &
SequentialSimulationManager::getNextEventTime(){
  const Event *nextEvent = myEventSet->peekEvent();
  if( nextEvent != 0 ){
    return nextEvent->getReceiveTime();
  }
  else{
    return getPositiveInfinity();
  }
}

const VTime &
SequentialSimulationManager::getPositiveInfinity() const {
  return myApplication->getPositiveInfinity(); 
}

const VTime &
SequentialSimulationManager::getZero() const { 
  return myApplication->getZero(); 
}

bool
SequentialSimulationManager::simulationComplete(){
  return myEventSet->peekEvent() == 0;
}

void
SequentialSimulationManager::shutdown( const string &errorMessage ){
  cerr << errorMessage << endl;
  exit( -1 );
}
