
#include <stdlib.h>                     // for NULL, exit
#include <iostream>                     // for operator<<, basic_ostream, etc

#include "Application.h"                // for Application
#include "Configurable.h"               // for Configurable
#include "Event.h"                      // for Event
#include "EventSet.h"                   // for EventSet
#include "EventSetFactory.h"            // for EventSetFactory
#include "SequentialSimulationManager.h"
#include "SimulationConfiguration.h"    // for SimulationConfiguration
#include "SimulationObject.h"           // for SimulationObject
#include "WarpedDebug.h"                // for debugout
#include "warped.h"                     // for ASSERT, werr, wout

class SimulationStream;

using std::cerr;
using std::cout;
using std::endl;
using std::vector;

SequentialSimulationManager::SequentialSimulationManager(Application* initApplication):
    SimulationManagerImplementationBase(),
    simulationTime(initApplication->getZero().clone()),
    sequentialWout(cerr.rdbuf(), ios::out),
    sequentialWerr(cerr.rdbuf(), ios::out),
    myApplication(initApplication),
    totalSimulationTime(0.0),
    trackEventCount(false),
    statisticsFileFormat(""),
    statisticsFilePath(""),
    myEventSet(NULL),
    numberOfProcessedEvents(0) {
    // Initialize wout and werr to suitable sequential simulation streams...
    wout = &sequentialWout;
    werr = &sequentialWerr;
}

SequentialSimulationManager::~SequentialSimulationManager() {
    delete myEventSet;

    // The following code was causing a segmentation fault when the SimulationObject
    // was allocated out of a "non-standard" part of memory.  What it comes down to
    // is we didn't allocate them, so we should not destroy them.

    //   // iteratively call delete on each simulation object pointer
    //   for(typeSimMap::iterator iter = localArrayOfSimObjPtrs->begin();
    //       iter != localArrayOfSimObjPtrs->end();
    //       iter++ ){
    //     SimulationObject *toDelete = (*iter).second;
    //     cout << "Deleting " << *toDelete->getObjectID() << endl;
    //     delete toDelete;
    //   }

    delete localArrayOfSimObjPtrs;
}

const Event*
SequentialSimulationManager::getEvent(SimulationObject* object) {
    ASSERT(myEventSet != NULL);
    const Event* event = myEventSet->getEvent();
    if (event != NULL) {
        numberOfProcessedEvents++;
    }
    return event;
}

const Event*
SequentialSimulationManager::peekEvent(SimulationObject* object) {
    ASSERT(myEventSet != NULL);
    return myEventSet->peekEvent();
}

void
SequentialSimulationManager::handleEvent(const Event* event) {
    ASSERT(event != NULL);
    ASSERT(myEventSet != NULL);
    myEventSet->insert(event);

    if (trackEventCount) {
        graphStatistics.update_edge_stat(
            event->getSender().getSimulationObjectID(),
            event->getReceiver().getSimulationObjectID(),
            "event_count"
        );
    }
}

void
SequentialSimulationManager::initialize() {
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
SequentialSimulationManager::finalize() {
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
         << (numberOfProcessedEvents / totalSimulationTime) << " events/sec).\n"
         << "Initalization - " << initializeWatch.elapsed() << " seconds\n"
         << "Finalization - " << finalizeWatch.elapsed() << " seconds\n";


    if (trackEventCount) {
        graphStatistics.write_to_file(statisticsFileFormat, statisticsFilePath);
    }
}

void
SequentialSimulationManager::simulate(const VTime& simulateUntil) {
    debug::debugout << "Simulating from " << getSimulationTime() << " to " << simulateUntil << endl;

    StopWatch simulateWatch;
    simulateWatch.start();
    const Event* nextEvent = myEventSet->peekEvent();
    while (nextEvent != NULL && (simulationTime == 0 || *simulationTime < simulateUntil)) {
        // This if test guarantees that the simulation does not get ahead of the simulate until
        // time.  This is important for simbus, which may have an event show up unexpectedly
        // from the analog domain. --DNS
        if (nextEvent->getReceiveTime() > simulateUntil)
        { break; }
        setSimulationTime(nextEvent->getReceiveTime());
        SimulationObject* object = getObjectHandle(nextEvent->getReceiver());
        object->setSimulationTime(nextEvent->getReceiveTime());

        // call executeProcess on this object
        object->executeProcess();

        myEventSet->cleanUp();
        nextEvent = myEventSet->peekEvent();
    }
    simulateWatch.stop();
    totalSimulationTime += simulateWatch.elapsed();
}

void
SequentialSimulationManager::configure(SimulationConfiguration& configuration) {

    const EventSetFactory* tempEventSetFactory = EventSetFactory::instance();
    myEventSet = dynamic_cast<EventSet*>(tempEventSetFactory->allocate(configuration,
                                                                       this));

    trackEventCount = configuration.get_bool({"Statistics", "EventCount"}, false);
    statisticsFileFormat = configuration.get_string({"Statistics", "OutputFormat"}, "dot");
    statisticsFilePath = configuration.get_string({"Statistics", "OutputFilePath"}, "statistics.dot");
}

// some tasks this function is responsible for
// a. creating and storing the map of simulation object pointers
// b. assigning unique ids to each simulation object
// c. setting the simulation manager handle in each object
// d. configuring the state by calling allocateState on each object
// e. set the simulation manager pointer in each object
void
SequentialSimulationManager::registerSimulationObjects() {
    std::vector<SimulationObject*>* simulationObjects = myApplication->getSimulationObjects();

    if (simulationObjects == nullptr) {
        shutdown("Application returned null map of simulation objects - exiting");
    }

    // create the map of name -> object
    localArrayOfSimObjPtrs = partitionVectorToHashMap(simulationObjects);
    setNumberOfObjects(simulationObjects->size());

    // copy the simulation object pointers into our local vector
    localArrayOfSimObjIDs = *simulationObjects;

    // assign IDs to each of the objects in the order they were created
    unsigned int count = 0;
    for (auto object : localArrayOfSimObjIDs) {
        // create and store in the map a relation between ids and object names
        OBJECT_ID* id = new OBJECT_ID(count);

        // store this objects id for future reference
        object->setObjectID(id);

        // store a handle to our simulation manager in the object
        object->setSimulationManager(this);

        // lets allocate the initial state here
        object->setInitialState(object->allocateState());

        count++;
    }

    delete simulationObjects;
}

SimulationStream*
SequentialSimulationManager::getIFStream(const string& fileName,
                                         SimulationObject* object) {
    SequentialSimulationStream* simStream =
        new SequentialSimulationStream(fileName, ios::in);
    return simStream;
}

SimulationStream*
SequentialSimulationManager::getOFStream(const string& fileName,
                                         SimulationObject* object,
                                         ios::openmode mode) {
    SequentialSimulationStream* simStream =
        new SequentialSimulationStream(fileName, mode);
    return simStream;
}

SimulationStream*
SequentialSimulationManager::getIOFStream(const string& fileName,
                                          SimulationObject* object) {
    SequentialSimulationStream* simStream =
        new SequentialSimulationStream(fileName, ios::in | ios::app);
    return simStream;
}

const VTime&
SequentialSimulationManager::getCommittedTime() {
    return getSimulationTime();
}

const VTime&
SequentialSimulationManager::getNextEventTime() {
    const Event* nextEvent = myEventSet->peekEvent();
    if (nextEvent != 0) {
        return nextEvent->getReceiveTime();
    } else {
        return getPositiveInfinity();
    }
}

const VTime&
SequentialSimulationManager::getPositiveInfinity() const {
    return myApplication->getPositiveInfinity();
}

const VTime&
SequentialSimulationManager::getZero() const {
    return myApplication->getZero();
}

bool
SequentialSimulationManager::simulationComplete() {
    return myEventSet->peekEvent() == 0;
}

void
SequentialSimulationManager::shutdown(const string& errorMessage) {
    cerr << errorMessage << endl;
    exit(-1);
}
