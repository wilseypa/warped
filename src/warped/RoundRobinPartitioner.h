#ifndef ROUNDROBINPARTITIONER_H
#define ROUNDROBINPARTITIONER_H


#include <warped/Partitioner.h>

class RoundRobinPartitioner : public Partitioner {
public:
  RoundRobinPartitioner();

  const PartitionInfo *partition( const vector<SimulationObject *> *objects,
                                  const unsigned int numLPs ) const;
};

#endif
