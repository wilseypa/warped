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
#include "SMMPPartitioner.h"
#include "warped.h"

SMMPPartitioner::SMMPPartitioner(){}

void 
SMMPPartitioner::addObjectGroup( const vector<SimulationObject *> *group ){
  groups.push_back(group);
}

const PartitionInfo *
SMMPPartitioner::partition( const vector<SimulationObject *> *objects,
                            const unsigned int numLPs ) const {
  PartitionInfo *myPartitionInfo = new PartitionInfo( numLPs );
  
  vector< vector<SimulationObject *>* > partitions(numLPs);
  
  for(unsigned int i = 0; i < numLPs; i++){
    partitions[i] = new vector<SimulationObject *>;
  }
  
  unsigned int n = 0;
  while( n < groups.size() ){
    partitions[n % numLPs]->insert( partitions[n % numLPs]->end(),
                                   groups[n]->begin(),
                                   groups[n]->end() );
    n++;
  }

  for(unsigned int i = 0; i < numLPs; i++){
    myPartitionInfo->addPartition( i, partitions[i] );
  }

  return myPartitionInfo;
}
