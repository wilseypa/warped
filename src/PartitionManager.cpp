#include "PartitionManager.h"

#include "SimulationConfiguration.h"
#include "Application.h"
#include "RoundRobinPartitioner.h"
#include "GreedyPartitioner.h"
#include "ProfileGuidedPartitioner.h"

#include <vector>
#include <fstream>

class PartitionInfo;

PartitionManager::PartitionManager(SimulationConfiguration& configuration) {
    partitionType = configuration.get_string({"TimeWarp", "Partitioner", "Type"},
                                             "Default");
    statisticsFile = configuration.get_string({"TimeWarp", "Partitioner", "StatisticsFile"},
                                              "statistics.metis");
}

/// Partitions a vector of simualtion objects based on a configuration value
const PartitionInfo* PartitionManager::getPartitionInfo(Application* application,
                                                        unsigned int numPartitions) {
    const PartitionInfo* retval;
    std::vector<SimulationObject*>* objects = application->getSimulationObjects();

    if (partitionType == "RoundRobin") {
        retval = RoundRobinPartitioner().partition(objects, numPartitions);
    } else if (partitionType == "Greedy") {
        retval = GreedyPartitioner().partition(objects, numPartitions);
    } else if (partitionType == "ProfileGuided") {
        std::ifstream input(statisticsFile);
        try {
            retval = ProfileGuidedPartitioner(input).partition(objects, numPartitions);
        } catch (...) {
            input.close();
            throw;
        }
        input.close();
    } else {
        retval = application->getPartitionInfo(numPartitions, objects);
    }

    delete objects;
    return retval;
}
