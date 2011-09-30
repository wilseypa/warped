// See copyright notice in file Copyright in the root directory of this archive.

#include "PartitionInfo.h"
#include "DefaultPartitioner.h"
#include "warped.h"

DefaultPartitioner::DefaultPartitioner(){}

const PartitionInfo *
DefaultPartitioner::partition( const vector<SimulationObject *> *objects,
				  const unsigned int numLPs ) const {

  PartitionInfo *myPartitionInfo = new PartitionInfo( numLPs );

  unsigned int numLeft = objects->size();
  unsigned int numObjectsPerProcessor = objects->size() / numLPs;
  unsigned int objectsRemaining = objects->size() % numLPs;
  unsigned int numObjectsThisProcessor;
  for( unsigned int lpNum = 0; lpNum < numLPs; lpNum++ ){
    if(lpNum < objectsRemaining){
      numObjectsThisProcessor = numObjectsPerProcessor + 1;
    }
    else{
      numObjectsThisProcessor = numObjectsPerProcessor;
    }

    vector <SimulationObject *> *partition = new vector <SimulationObject *>;
    for( unsigned int j = 0; j < numObjectsThisProcessor; j++ ){
      partition->push_back( (*objects)[numLeft-1] );
      numLeft--;
    }
    myPartitionInfo->addPartition( lpNum, partition );
  }
  ASSERT( numLeft == 0 );

  return myPartitionInfo;
}
