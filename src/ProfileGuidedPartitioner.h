#ifndef PROFILE_GUIDED_PARTITIONER_H
#define PROFILE_GUIDED_PARTITIONER_H

#include <iostream>
#include <string>
#include <vector>                       // for vector

#include "Partitioner.h"                // for Partitioner

class PartitionInfo;
class SimulationObject;

class ProfileGuidedPartitioner : public Partitioner {
public:
    ProfileGuidedPartitioner(std::istream& input);

    const PartitionInfo* partition(const vector<SimulationObject*>* objects,
                                   const unsigned int numPartitions) const;

private:
    std::istream& input;
};

#endif
