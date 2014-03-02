#include <string.h>                     // for strcasecmp, NULL
#include <ostream>                      // for operator<<, basic_ostream, etc

#include "CommunicationManagerFactory.h"
#include "Configurable.h"               // for Configurable
#include "DefaultCommunicationManager.h"
#include "DefaultPhysicalCommunicationLayer.h"
#include "MsgAggregatingCommunicationManager.h"
#include "SetObject.h"                  // for ostream
#include "SimulationConfiguration.h"    // for SimulationConfiguration
#include "ThreadedTimeWarpSimulationManager.h"
#include "TimeWarpSimulationManager.h"  // for TimeWarpSimulationManager
#include "WarpedDebug.h"                // for debugout
#include "eclmpl/MPIPhysicalCommunicationLayer.h"
#include "eclmpl/PhysicalCommunicationLayer.h"
#include "eclmpl/TCPSelectPhysicalCommunicationLayer.h"
#include "eclmpl/UDPSelectPhysicalCommunicationLayer.h"
#include "warped.h"                     // for ASSERT

CommunicationManagerFactory::CommunicationManagerFactory() {}

CommunicationManagerFactory::~CommunicationManagerFactory() {
}

// Note: configure() performs two configurations steps:
// (a) it first configures the physical communication library
// (b) then it configures the communication manager (passing in the
//     physical communication library as a parameter)
Configurable*
CommunicationManagerFactory::allocate(SimulationConfiguration& configuration,
                                      Configurable* parent) const {
    ASSERT(parent != 0);
    TimeWarpSimulationManager* mySimulationManager =
        dynamic_cast<TimeWarpSimulationManager*>(parent);

    ThreadedTimeWarpSimulationManager* myThreadedSimulationManager =
        dynamic_cast<ThreadedTimeWarpSimulationManager*>(parent);

    PhysicalCommunicationLayer* myPhysicalCommunicationLayer = NULL;
    Configurable* retval = 0;

    // first configure what physical communication layer we are going to use ...
    std::string physicalLayer = configuration.get_string({"TimeWarp", "CommunicationManager", "PhysicalLayer"},
                                                         "Default");
    if (physicalLayer == "MPI" || physicalLayer == "UDPSelect" || physicalLayer == "TCPSelect") {
        myPhysicalCommunicationLayer = allocatePhysicalCommunicationLayer(physicalLayer);
        debug::debugout << "(" << mySimulationManager->getSimulationManagerID() << ") ";
        if (myPhysicalCommunicationLayer == 0) {
            mySimulationManager->shutdown("Could not allocate physical layer corresponding to " + physicalLayer
                                          + ", perhaps not configured at compile time?");
        } else {
            debug::debugout << "configured the " << physicalLayer << " Physical Communication Layer" << endl;
        }
    } else {
        myPhysicalCommunicationLayer = new DefaultPhysicalCommunicationLayer();
        debug::debugout << "configured the default physical communication layer" << endl;
    }

    std::string managerType = configuration.get_string({"TimeWarp", "CommunicationManager", "Type"},
                                                       "Default");
    std::string simulationType = configuration.get_string({"Simulation"}, "Sequential");

    if (simulationType == "ThreadedTimeWarp") {
        myPhysicalCommunicationLayer->physicalInit();
        if (managerType == "Default") {
            retval = new DefaultCommunicationManager(
                myPhysicalCommunicationLayer, myThreadedSimulationManager);
            debug::debugout << "configured the default communication manager"
                            << endl;
        } else if (managerType == "MessageAggregating") {
            retval = new MsgAggregatingCommunicationManager(
                myPhysicalCommunicationLayer, myThreadedSimulationManager);
            debug::debugout
                    << "configured a message aggregating communication manager"
                    << endl;
        } else {
            myThreadedSimulationManager->shutdown("Unknown CommunicationManager type \"" + managerType + "\"");
        }

        return retval;
    }

    myPhysicalCommunicationLayer->physicalInit();
    if (managerType == "Default") {
        retval = new DefaultCommunicationManager(myPhysicalCommunicationLayer,
                                                 mySimulationManager);
        debug::debugout << "configured the default communication manager" << endl;
    } else if (managerType == "MessageAggregating") {
        retval = new MsgAggregatingCommunicationManager(myPhysicalCommunicationLayer,
                                                        mySimulationManager);
        debug::debugout << "configured a message aggregating communication manager" << endl;
    } else {
        mySimulationManager->shutdown("Unknown CommunicationManager type \"" + managerType + "\"");
    }

    return retval;
}

const CommunicationManagerFactory*
CommunicationManagerFactory::instance() {
    static const CommunicationManagerFactory* singleton = new CommunicationManagerFactory();

    return singleton;
}


PhysicalCommunicationLayer*
CommunicationManagerFactory::
allocatePhysicalCommunicationLayer(const string& physicalLayer) {
    PhysicalCommunicationLayer* retval = 0;

    if (strcasecmp(physicalLayer.c_str(), "MPI") == 0) {
        retval = new MPIPhysicalCommunicationLayer();
    } else if (strcasecmp(physicalLayer.c_str(), "UDPSELECT") == 0) {
        retval = new UDPSelectPhysicalCommunicationLayer();
    } else if (strcasecmp(physicalLayer.c_str(), "TCPSELECT") == 0) {
        retval = new TCPSelectPhysicalCommunicationLayer();
    }
    return retval;
}
