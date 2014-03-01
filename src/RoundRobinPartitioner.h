#ifndef ROUNDROBINPARTITIONER_H
#define ROUNDROBINPARTITIONER_H


#include <Partitioner.h>

class RoundRobinPartitioner : public Partitioner {
public:
    RoundRobinPartitioner();

    const PartitionInfo* partition(const vector<SimulationObject*>* objects,
                                   const unsigned int numPartitions) const;
};

#endif
