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
