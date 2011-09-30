#ifndef DEFAULTPARTITIONER_H
#define DEFAULTPARTITIONER_H

// See copyright notice in file Copyright in the root directory of this archive.

#include <warped/Partitioner.h>

class DefaultPartitioner : public Partitioner {
public:
  DefaultPartitioner();

  const PartitionInfo *partition( const vector<SimulationObject *> *objects,
				  const unsigned int numLPs ) const;
};

#endif
