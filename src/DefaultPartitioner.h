#ifndef DEFAULTPARTITIONER_H
#define DEFAULTPARTITIONER_H


#include <Partitioner.h>

class DefaultPartitioner : public Partitioner {
public:
  DefaultPartitioner();

  const PartitionInfo *partition( const vector<SimulationObject *> *objects,
				  const unsigned int numLPs ) const;
};

#endif
