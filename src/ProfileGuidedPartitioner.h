#ifndef PROFILE_GUIDED_PARTITIONER_H
#define PROFILE_GUIDED_PARTITIONER_H

#include "Partitioner.h"
#include <string>
#include <iostream>

class ProfileGuidedPartitioner : public Partitioner {
public:
    ProfileGuidedPartitioner(std::istream& input);

    const PartitionInfo* partition(const vector<SimulationObject*>* objects,
                                   const unsigned int numPartitions) const;

private:
    std::istream& input;
};

#endif
