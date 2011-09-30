#ifndef PARTITIONER_H
#define PARTITIONER_H

// See copyright notice in file Copyright in the root directory of this archive.

#include <vector>
using std::vector;
class SimulationObject;
class PartitionInfo;

class Partitioner {
public:
  /**
     Partition the objects.  Caller is responsible for deleting both the
     arguments and the returned PartitionInfo.
  */
  virtual const PartitionInfo *partition( const vector<SimulationObject *> *objects,
					  const unsigned int numLPs ) const = 0;

  virtual ~Partitioner() = 0;

protected:
  Partitioner(){}
};

#endif
