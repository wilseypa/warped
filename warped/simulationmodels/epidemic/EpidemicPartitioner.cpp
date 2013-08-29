#include "PartitionInfo.h"
#include "EpidemicPartitioner.h"
#include "warped.h"

EpidemicPartitioner::EpidemicPartitioner() {}

void EpidemicPartitioner::addObjectGroup(
            const vector<SimulationObject *> *group ) {
    groups.push_back(group);
}

const PartitionInfo *EpidemicPartitioner::partition(
                    const vector<SimulationObject *> *objects,
                    const unsigned int numProcessors ) const {

    unsigned int index = 0;
    PartitionInfo *myPartitionInfo = new PartitionInfo( numProcessors );

    vector< vector<SimulationObject *>* > partitions(numProcessors);

    for( index = 0; index < numProcessors; index++ ) {
        partitions[index] = new vector<SimulationObject *>;
    }

    for( index = 0; index < groups.size(); index++ ) {
        partitions[ index%numProcessors ]->insert(
                                        partitions[ index%numProcessors ]->end(),
                                        groups[index]->begin(),
                                        groups[index]->end() );
    }

    for( index = 0; index < numProcessors; index++ ) {
        myPartitionInfo->addPartition( index, partitions[index] );
    }

    return myPartitionInfo;
}

