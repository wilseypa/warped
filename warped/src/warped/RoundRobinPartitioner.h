#ifndef ROUNDROBINPARTITIONER_H
#define ROUNDROBINPARTITIONER_H

// See copyright notice in file Copyright in the root directory of this archive.

#include <warped/Partitioner.h>

class RoundRobinPartitioner : public Partitioner {
public:
  RoundRobinPartitioner();

  const PartitionInfo *partition( const vector<SimulationObject *> *objects,
                                  const unsigned int numLPs ) const;
};

#endif
