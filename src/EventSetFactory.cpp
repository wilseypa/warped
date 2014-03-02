
#include <ostream>                      // for operator<<, basic_ostream, etc
#include <string>                       // for allocator, basic_string, etc

#include "Configurable.h"               // for Configurable
#include "EventSet.h"                   // for EventSet
#include "EventSetFactory.h"
#include "SimulationConfiguration.h"    // for SimulationConfiguration
#include "SimulationManager.h"          // for SimulationManager
#include "SingleLinkedList.h"           // for SingleLinkedList
#include "SplayTree.h"                  // for SplayTree
#include "WarpedDebug.h"                // for debugout
#include "warped.h"                     // for ASSERT
using std::string;

EventSetFactory::EventSetFactory() {}

EventSetFactory::~EventSetFactory() {}

Configurable*
EventSetFactory::allocate(SimulationConfiguration& configuration,
                          Configurable* parent) const {
    SimulationManager* mySimulationManager =
        dynamic_cast<SimulationManager*>(parent);
    ASSERT(mySimulationManager != 0);

    EventSet* retval = 0;
    // the following cases are possible:

    // (1) only a event set choice is specified, in which case we
    // proceed assuming that only a centralized event set is desired.

    // (2) following the event set choice, the word "all" is specified
    // meaning that a separate event set is to be maintained for each
    // simulation object.

    std::string eventListType = configuration.get_string({"EventList", "Type"}, "unset");
    if (eventListType == SplayTree::getType()) {
        retval = new SplayTree();
        debug::debugout << "Configured a SplayTree as the event set" << std::endl;
    } else if (eventListType == SingleLinkedList::getType()) {
        retval = new SingleLinkedList();
        debug::debugout << "Configured a SingleLinkedList as the event set" << std::endl;
    } else {
        mySimulationManager->shutdown("Invalid EventList configuration value - \"" + eventListType + "\"");
    }

    return retval;
}

const EventSetFactory*
EventSetFactory::instance() {
    static EventSetFactory* singleton = new EventSetFactory();
    return singleton;
}


