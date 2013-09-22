#ifndef SMMPPARTITIONER_H
#define SMMPPARTITIONER_H

#include <Partitioner.h>

/* 
   This class is designed specifically to partition the objects
   in the SMMP simulation.
*/
class SMMPPartitioner : public Partitioner {
public:
  SMMPPartitioner();

  const PartitionInfo *partition( const vector<SimulationObject *> *objects,
                                  const unsigned int numLPs ) const;
				  
  void addObjectGroup( const vector<SimulationObject *> *group );
  
private:
  vector< const vector<SimulationObject *>* > groups;
};

#endif
