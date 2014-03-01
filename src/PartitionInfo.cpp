#include "PartitionInfo.h"

PartitionInfo::PartitionInfo(unsigned int numPartitions) :
    myNumberOfPartitions(numPartitions) {}

PartitionInfo::~PartitionInfo() {
    for (int i = 0; i < myPartitions.size(); i++) {
        delete myPartitions[i];
    }
}

unsigned int
PartitionInfo::getNumberOfPartitions() const {
    return myNumberOfPartitions;
}

vector<SimulationObject*>*
PartitionInfo::getPartition(unsigned int partitionNumber) const {
    return myPartitions[partitionNumber];
}

void
PartitionInfo::addPartition(vector<SimulationObject*>* partition) {
    myPartitions.push_back(partition);
}

