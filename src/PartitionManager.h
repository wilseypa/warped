#ifndef PARTITION_MANAGER_H
#define PARTITION_MANAGER_H

#include <string>

class SimulationConfiguration;
class Application;
class PartitionInfo;

class PartitionManager {
public:
    PartitionManager(SimulationConfiguration& configuration);

    /// Partitions a vector of simualtion objects based on a configuration value
    const PartitionInfo* getPartitionInfo(Application* application, unsigned int numPartitions);

private:
    std::string partitionType;
    std::string statisticsFile;
};

#endif
