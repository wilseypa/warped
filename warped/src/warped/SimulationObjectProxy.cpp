// See copyright notice in file Copyright in the root directory of this archive.

#include "SimulationObjectProxy.h"
#include "OutputManager.h"
#include "KernelMessage.h"
#include "Event.h"
#include "EventMessage.h"
#include "NegativeEventMessage.h"
#include "SerializedInstance.h"
#include "CommunicationManager.h"
#include "TimeWarpSimulationManager.h"
#include <utils/Debug.h>

using std::cerr;
using std::endl;

CommunicationManager *
SimulationObjectProxy::myCommunicationManagerHandle = NULL;

SimulationObjectProxy::SimulationObjectProxy( const string &initObjectName,
					      unsigned int sourceSimMgr,
					      unsigned int destSimMgr,
					      CommunicationManager *commMgr) :
  sourceSimulationManager(sourceSimMgr),
  destinationSimulationManager(destSimMgr),
  objectName( initObjectName ){
  DEBUG(
	numberOfPositiveNetworkEvents = 0;
	numberOfNegativeNetworkEvents = 0;
	)
    myCommunicationManagerHandle = commMgr;
}

SimulationObjectProxy::~SimulationObjectProxy(){
  utils::debug << "processed (+)(" << numberOfPositiveNetworkEvents 
	<< ")(-)(" << numberOfNegativeNetworkEvents
	<< ") MPI messages" << endl;
}

void
SimulationObjectProxy::initialize(){
  cerr << "ERROR: SimulationObjectProxy::initialize called" << endl;
  abort();
}


void
SimulationObjectProxy::finalize(){
  cerr << "ERROR: SimulationObjectProxy::finalize called" << endl;
  abort();

}


void
SimulationObjectProxy::executeProcess(){
  cerr << "ERROR: SimulationManager(" << sourceSimulationManager
       << ")'s SimulationObjectProxy::executeProcess called" << endl;
  abort();

}

State*
SimulationObjectProxy::allocateState(){
  cerr << "ERROR: SimulationObjectProxy::allocateState called" << endl;
  abort();
  return NULL;
}

void
SimulationObjectProxy::deallocateState( const State * ){
  cerr << "ERROR: SimulationObjectProxy::deallocateState called" << endl;
  abort();
}

void
SimulationObjectProxy::reclaimEvent(const Event * ){
  cerr << "ERROR: SimulationObjectProxy reclaimEvent called!" << endl;
  abort();
}

const string &
SimulationObjectProxy::getEventMessageType(){
  static const string messageType = "EventMessage";
  return messageType;
}

const string &
SimulationObjectProxy::getNegativeEventMessageType(){
  static const string messageType = "NegativeEventMessage";
  return messageType;
}

void
SimulationObjectProxy::reportError( const string &, SEVERITY ){
  cerr << "ERROR: SimulationObjectProxy::reportError called" << endl;
  abort();
}
