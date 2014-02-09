#include "Application.h"
#include <vector>
#include "RoundRobinPartitioner.h"

const PartitionInfo* Application::getPartitionInfo(unsigned int numProcessorsAvailable,
                              const std::vector<SimulationObject*>* simulationObjects) {
    RoundRobinPartitioner partitioner;
    return partitioner.partition(simulationObjects, numProcessorsAvailable);
}