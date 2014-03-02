#ifndef GREEDYPARTITIONER_H
#define GREEDYPARTITIONER_H

#include <Partitioner.h>                // for Partitioner
#include <vector>                       // for vector

class PartitionInfo;
class SimulationObject;

class GreedyPartitioner : public Partitioner {
public:
    GreedyPartitioner();

    const PartitionInfo* partition(const vector<SimulationObject*>* objects,
                                   const unsigned int numPartitions) const;
};

#endif
