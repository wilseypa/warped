
#include <stdlib.h>                     // for abort, NULL
#include <fstream>                      // for operator<<, basic_ostream, etc
#include <iostream>                     // for cerr
#include <string>

#include "SimulationObjectProxy.h"

class State;

using std::cerr;
using std::string;
using std::endl;

CommunicationManager*
SimulationObjectProxy::myCommunicationManagerHandle = NULL;

SimulationObjectProxy::SimulationObjectProxy(const string& initObjectName,
                                             unsigned int sourceSimMgr,
                                             unsigned int destSimMgr,
                                             CommunicationManager* commMgr) :
    sourceSimulationManager(sourceSimMgr),
    destinationSimulationManager(destSimMgr),
    objectName(initObjectName) {
    DEBUG(
        numberOfPositiveNetworkEvents = 0;
        numberOfNegativeNetworkEvents = 0;
    )
    myCommunicationManagerHandle = commMgr;
}

SimulationObjectProxy::~SimulationObjectProxy() {
    /*
      debug::debugout << "processed (+)(" << numberOfPositiveNetworkEvents
        << ")(-)(" << numberOfNegativeNetworkEvents
        << ") MPI messages" << endl;
    */
}

void
SimulationObjectProxy::initialize() {
    cerr << "ERROR: SimulationObjectProxy::initialize called" << endl;
    abort();
}


void
SimulationObjectProxy::finalize() {
    cerr << "ERROR: SimulationObjectProxy::finalize called" << endl;
    abort();

}


void
SimulationObjectProxy::executeProcess() {
    cerr << "ERROR: SimulationManager(" << sourceSimulationManager
         << ")'s SimulationObjectProxy::executeProcess called" << endl;
    abort();

}

State*
SimulationObjectProxy::allocateState() {
    cerr << "ERROR: SimulationObjectProxy::allocateState called" << endl;
    abort();
    return NULL;
}

void
SimulationObjectProxy::deallocateState(const State*) {
    cerr << "ERROR: SimulationObjectProxy::deallocateState called" << endl;
    abort();
}

void
SimulationObjectProxy::reclaimEvent(const Event*) {
    cerr << "ERROR: SimulationObjectProxy reclaimEvent called!" << endl;
    abort();
}

const string&
SimulationObjectProxy::getEventMessageType() {
    static const string messageType = "EventMessage";
    return messageType;
}

const string&
SimulationObjectProxy::getNegativeEventMessageType() {
    static const string messageType = "NegativeEventMessage";
    return messageType;
}

void
SimulationObjectProxy::reportError(const string&, SEVERITY) {
    cerr << "ERROR: SimulationObjectProxy::reportError called" << endl;
    abort();
}
