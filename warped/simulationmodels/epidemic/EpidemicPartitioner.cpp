// Copyright (c) Clifton Labs, Inc.
// All rights reserved.

// CLIFTON LABS MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE
// SUITABILITY OF THE SOFTWARE, EITHER EXPRESS OR IMPLIED, INCLUDING BUT
// NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
// PARTICULAR PURPOSE, OR NON-INFRINGEMENT.  CLIFTON LABS SHALL NOT BE
// LIABLE FOR ANY DAMAGES SUFFERED BY LICENSEE AS A RESULT OF USING, RESULT
// OF USING, MODIFYING OR DISTRIBUTING THIS SOFTWARE OR ITS DERIVATIVES.

// By using or copying this Software, Licensee agrees to abide by the
// intellectual property laws, and all other applicable laws of the
// U.S., and the terms of this license.

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

