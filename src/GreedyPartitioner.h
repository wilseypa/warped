#ifndef GREEDYPARTITIONER_H
#define GREEDYPARTITIONER_H

#include <Partitioner.h>

class GreedyPartitioner : public Partitioner {
public:
    GreedyPartitioner();

    const PartitionInfo* partition(const vector<SimulationObject*>* objects,
                                   const unsigned int numPartitions) const;
};

#endif
