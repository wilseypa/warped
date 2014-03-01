
#include "PartitionInfo.h"
#include "RoundRobinPartitioner.h"
#include "warped.h"

RoundRobinPartitioner::RoundRobinPartitioner() {}

const PartitionInfo*
RoundRobinPartitioner::partition(const vector<SimulationObject*>* objects,
                                 const unsigned int numPartitions) const {

    PartitionInfo* myPartitionInfo = new PartitionInfo(numPartitions);

    vector< vector<SimulationObject*>* > partitions(numPartitions);

    for (unsigned int i = 0; i < numPartitions; i++) {
        partitions[i] = new vector<SimulationObject*>;
    }

    unsigned int n = 0;
    unsigned int skew = 0;
    while (n < objects->size()) {
        partitions[n % numPartitions]->push_back((*objects)[n]);
        n++;
    }

    for (unsigned int i = 0; i < numPartitions; i++) {
        myPartitionInfo->addPartition(partitions[i]);
    }

    return myPartitionInfo;
}
