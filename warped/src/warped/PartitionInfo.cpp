// See copyright notice in file Copyright in the root directory of this archive.

#include "PartitionInfo.h"

PartitionInfo::PartitionInfo( unsigned int numPartitions ) :
  myNumberOfPartitions( numPartitions ){}

PartitionInfo::~PartitionInfo(){
  for(int i = 0; i < myObjectSets.size(); i++){
    delete myObjectSets[i];
  }
}

unsigned int 
PartitionInfo::getNumberOfPartitions() const {
  return myNumberOfPartitions;
}

vector<SimulationObject *> *
PartitionInfo::getObjectSet( unsigned int setNumber ) const {
  return myObjectSets[setNumber];
}

void 
PartitionInfo::addPartition( unsigned int partitionNumber,
			     vector<SimulationObject *> *toAdd ){
  myObjectSets.reserve( partitionNumber + 1 );
  myObjectSets.insert( myObjectSets.begin() + partitionNumber,  toAdd );
}

