#include <cstdlib>                     // for abort, NULL
#include <iostream>                    // for operator<<, basic_ostream, etc

#include "DefaultPhysicalCommunicationLayer.h"

DefaultPhysicalCommunicationLayer::DefaultPhysicalCommunicationLayer() : PhysicalCommunicationLayer() {}

DefaultPhysicalCommunicationLayer::~DefaultPhysicalCommunicationLayer() {}

void DefaultPhysicalCommunicationLayer::physicalInit() {
    // There is nothing to be done for a
    // DefaultPhysicalCommunicationLayer since all objects are on one
    // simulation manager and so no network communication will take
    // place.
}

unsigned int DefaultPhysicalCommunicationLayer::physicalGetId() const {
    // For a DefaultPhysicalCommunicationLayer, the id is always zero
    // since we are running a uniprocessor simulation.
    return 0;
}

void DefaultPhysicalCommunicationLayer::physicalSend(const SerializedInstance* toSend, unsigned int dest) {
    std::cerr << "DefaultPhysicalCommunicationLayer::physicalSend should not be called at all" << std::endl;
    std::cerr << "Exiting simulation ..." << std::endl;
    abort();
}

SerializedInstance* DefaultPhysicalCommunicationLayer::physicalProbeRecv() {
    std::cerr << "DefaultPhysicalCommunicationLayer::physicalProbeRecv should not be called at all" << std::endl;
    std::cerr << "Exiting simulation ..." << std::endl;
    abort();
    return NULL;
}

void DefaultPhysicalCommunicationLayer::physicalFinalize() {
    // For DefaultPhysicalCommunicationLayer, we didn't initialize MPI
    // or any other communication library; so there is nothing to be
    // done for finalize.
}

unsigned int DefaultPhysicalCommunicationLayer::physicalGetSize() const {
    // For a DefaultPhysicalCommunicationLayer, the size is always 1
    // since we are running a uniprocessor simulation.
    return 1;
}
