#ifndef EPIDEMIC_PARTITIONER_H
#define EPIDEMIC_PARTITIONER_H

#include <warped/Partitioner.h>

/* 
   This class is designed specifically to partition the objects
   in the Epidemic simulation.
*/
class EpidemicPartitioner : public Partitioner {

public:

	EpidemicPartitioner();

	const PartitionInfo *partition( const vector<SimulationObject *> *objects, 
										const unsigned int numProcessors ) const;

	void addObjectGroup( const vector<SimulationObject *> *group );

private:
	vector< const vector<SimulationObject *>* > groups;
};

#endif
