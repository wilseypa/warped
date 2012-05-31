// See copyright notice in file Copyright in the root directory of this archive.

#include <WarpedConfig.h>
#include "DefaultPhysicalCommunicationLayer.h"
#include "CommunicationManagerFactory.h"
#include "DefaultCommunicationManager.h"
#include "MsgAggregatingCommunicationManager.h"
#include "TimeWarpSimulationManager.h"
#include "SimulationConfiguration.h"
#include <utils/ConfigurationScope.h>
#include <utils/Debug.h>

#ifdef USE_TIMEWARP
#include "eclmpl/eclmpl.h"
#include "ThreadedTimeWarpSimulationManager.h"
#endif

CommunicationManagerFactory::CommunicationManagerFactory(){}

CommunicationManagerFactory::~CommunicationManagerFactory(){
}

// Note: configure() performs two configurations steps:
// (a) it first configures the physical communication library
// (b) then it configures the communication manager (passing in the
//     physical communication library as a parameter)
Configurable *
CommunicationManagerFactory::allocate( SimulationConfiguration &configuration,
				       Configurable *parent ) const {
  ASSERT( parent != 0 );
  TimeWarpSimulationManager *mySimulationManager =
    dynamic_cast<TimeWarpSimulationManager *>( parent );
#ifdef USE_TIMEWARP
  ThreadedTimeWarpSimulationManager *myThreadedSimulationManager =
      dynamic_cast<ThreadedTimeWarpSimulationManager *>( parent );
#endif

  PhysicalCommunicationLayer *myPhysicalCommunicationLayer = NULL;
  Configurable *retval = 0;

  // first configure what physical communication layer we are going to use ...
  if( configuration.physicalLayerIs( "MPI" ) ||
      configuration.physicalLayerIs( "UDPSelect" ) ||
      configuration.physicalLayerIs( "TCPSelect" ) ){
    myPhysicalCommunicationLayer = allocatePhysicalCommunicationLayer( configuration.getPhysicalLayerType() );
    utils::debug << "(" << mySimulationManager->getSimulationManagerID() << ") ";
    if( myPhysicalCommunicationLayer == 0 ){
      mySimulationManager->shutdown( "Could not allocate physical layer corresponding to " +
				     configuration.getPhysicalLayerType() +
				     ", perhaps not configured at compile time?" );
    }
    else{
      utils::debug << "configured the " << configuration.getPhysicalLayerType()
		     << " Physical Communication Layer" << endl;
    }
  }
  else{
    myPhysicalCommunicationLayer = new DefaultPhysicalCommunicationLayer();
    utils::debug << "configured the default physical communication layer" << endl;
  }
#if USE_TIMEWARP
	if (configuration.simulationTypeIs("ThreadedTimeWarp")) {
		myPhysicalCommunicationLayer->physicalInit(configuration);
		if (configuration.communicationManagerIs("DEFAULT")) {
			retval = new DefaultCommunicationManager(
					myPhysicalCommunicationLayer, myThreadedSimulationManager);
			utils::debug << "configured the default communication manager"
					<< endl;
		} else if (configuration.communicationManagerIs("MessageAggregating")) {
			retval = new MsgAggregatingCommunicationManager(
					myPhysicalCommunicationLayer, myThreadedSimulationManager);
			utils::debug
					<< "configured a message aggregating communication manager"
					<< endl;
		} else {
			myThreadedSimulationManager->shutdown(
					"Unknown CommunicationManager type \""
							+ configuration.getCommunicationManagerType());
		}

		return retval;
	}
#endif
  myPhysicalCommunicationLayer->physicalInit( configuration );
  if( configuration.communicationManagerIs( "DEFAULT" ) ){
    retval = new DefaultCommunicationManager( myPhysicalCommunicationLayer,
					      mySimulationManager );
    utils::debug << "configured the default communication manager" << endl;
  }
  else if ( configuration.communicationManagerIs( "MessageAggregating" ) ){
    retval = new MsgAggregatingCommunicationManager( myPhysicalCommunicationLayer,
						     mySimulationManager );
    utils::debug << "configured a message aggregating communication manager" << endl;
  }
  else {
    mySimulationManager->shutdown( "Unknown CommunicationManager type \"" +
				   configuration.getCommunicationManagerType() );
  }

  return retval;
}

const CommunicationManagerFactory *
CommunicationManagerFactory::instance(){
  static const CommunicationManagerFactory *singleton = new CommunicationManagerFactory();

  return singleton;
}


PhysicalCommunicationLayer *
CommunicationManagerFactory::
allocatePhysicalCommunicationLayer( const string &physicalLayer ){
  PhysicalCommunicationLayer *retval = 0;

#ifdef USE_TIMEWARP
  if( stringCaseCompare( physicalLayer, "MPI" ) ) {
    retval = new MPIPhysicalCommunicationLayer();
  }
  else if( stringCaseCompare( physicalLayer, "UDPSELECT" ) ) {
    retval = new UDPSelectPhysicalCommunicationLayer();
  }
  else if( stringCaseCompare( physicalLayer, "TCPSELECT" ) ) {
    retval = new TCPSelectPhysicalCommunicationLayer();
  }
#endif
  return retval;
}
