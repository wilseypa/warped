#include "GreedyPartitioner.h"
#include "PartitionInfo.h"              // for PartitionInfo
#include "warped.h"                     // for ASSERT

class SimulationObject;

GreedyPartitioner::GreedyPartitioner() {}

const PartitionInfo*
GreedyPartitioner::partition(const vector<SimulationObject*>* objects,
                             const unsigned int numPartitions) const {

    PartitionInfo* myPartitionInfo = new PartitionInfo(numPartitions);

    unsigned int numLeft = objects->size();
    unsigned int numObjectsPerProcessor = objects->size() / numPartitions;
    unsigned int objectsRemaining = objects->size() % numPartitions;
    unsigned int numObjectsThisProcessor;
    for (unsigned int objectNum = 0; objectNum < numPartitions; objectNum++) {
        if (objectNum < objectsRemaining) {
            numObjectsThisProcessor = numObjectsPerProcessor + 1;
        } else {
            numObjectsThisProcessor = numObjectsPerProcessor;
        }

        vector <SimulationObject*>* partition = new vector <SimulationObject*>;
        for (unsigned int j = 0; j < numObjectsThisProcessor; j++) {
            partition->push_back((*objects)[numLeft - 1]);
            numLeft--;
        }
        myPartitionInfo->addPartition(partition);
    }
    ASSERT(numLeft == 0);

    return myPartitionInfo;
}
