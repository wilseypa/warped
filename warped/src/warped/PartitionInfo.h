#ifndef PARTITION_INFO_H
#define PARTITION_INFO_H


#include <vector>
using std::vector;

class SimulationObject;

/**
   Describes the partitioning information requested by the application.

   Basically, the kernels says "I've got <x> processors available".  The
   application can say in response "<y> is optimal".  It will then provide
   a PartitionInfo that describes how best to partition the application's
   objects.
*/

class PartitionInfo {
public:
  PartitionInfo( unsigned int numPartitions );

  ~PartitionInfo();
  
  /**
     This is what the application feels is the optimal number of
     partitions.  (The kernel might choose to ignore this information.)
  */
  unsigned int getNumberOfPartitions() const;

  /**
     Get set <setNumber> of objects.

     @param setNumber The application will provide a partitioning scheme
     based on the number of processes that the kernel says it wants.  The
     count starts at 0.  (This is C++ after all ;-))
     
     @return A set, or 0 if you've asked for more sets than the Application
     says is optimal.
  */
  vector<SimulationObject *> *getObjectSet( unsigned int setNumber ) const;


  /**
     The constructor of this PartitionInfo can add vectors via this method.
  */
  void addPartition( unsigned int partitionNumber,
		     vector<SimulationObject *> *toAdd );
  

private:
  unsigned int myNumberOfPartitions;
  vector<vector<SimulationObject *> *> myObjectSets;
};

#endif
