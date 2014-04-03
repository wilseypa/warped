#include "Application.h"

#include "RoundRobinPartitioner.h"
#include "IntVTime.h"
#include <vector>
#include <string>

const PartitionInfo* Application::getPartitionInfo(unsigned int numProcessorsAvailable,
                                                   const std::vector<SimulationObject*>* simulationObjects) {
    RoundRobinPartitioner partitioner;
    return partitioner.partition(simulationObjects, numProcessorsAvailable);
}

int Application::finalize() {
    return 0;
}


const VTime& Application::getPositiveInfinity() {
    return IntVTime::getIntVTimePositiveInfinity();
}

const VTime& Application::getZero() {
    return IntVTime::getIntVTimeZero();
}

const VTime& Application::getTime(std::string& time) {
    IntVTime* vtime = new IntVTime(std::stoi(time));
    return *vtime;
}