// See copyright notice in file Copyright in the root directory of this archive.

#include "PartitionInfo.h"
#include "RoundRobinPartitioner.h"
#include "warped.h"

RoundRobinPartitioner::RoundRobinPartitioner(){}

const PartitionInfo *
RoundRobinPartitioner::partition( const vector<SimulationObject *> *objects,
                                  const unsigned int numLPs ) const {

  PartitionInfo *myPartitionInfo = new PartitionInfo( numLPs );
  
  vector< vector<SimulationObject *>* > partitions(numLPs);
  
  for(unsigned int i = 0; i < numLPs; i++){
    partitions[i] = new vector<SimulationObject *>;
  }

  unsigned int n = 0; 
  unsigned int skew = 0;
  while( n < objects->size() ){
//    if((n % numLPs) ==1 && ++skew % 2 == 1)
//      partitions[0]->push_back( (*objects)[n] );
//    else
      partitions[n % numLPs]->push_back( (*objects)[n] );
    n++;
  }

  for(unsigned int i = 0; i < numLPs; i++){
    myPartitionInfo->addPartition( i, partitions[i] );
  }

  return myPartitionInfo;
}
