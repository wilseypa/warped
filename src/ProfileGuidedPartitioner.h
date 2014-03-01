#ifndef PROFILE_GUIDED_PARTITIONER_H
#define PROFILE_GUIDED_PARTITIONER_H

#include "Partitioner.h"

class ProfileGuidedPartitioner : public Partitioner {
public:
    ProfileGuidedPartitioner();

    const PartitionInfo* partition(const vector<SimulationObject*>* objects,
                                   const unsigned int numPartitions) const;
};

#endif
