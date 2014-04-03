#ifndef PARTITION_INFO_H
#define PARTITION_INFO_H


#include <vector>
using std::vector;

class SimulationObject;

/**
    A PartitionInfo class contains a grouping of SimulationObjects. Each
    partition in the PartitionInfo object will be assigned to a different
    thread or process.
*/
class PartitionInfo {
public:
    PartitionInfo(unsigned int numPartitions);

    ~PartitionInfo();

    /**
       The number of partitions in this PartitionInfo object.
    */
    unsigned int getNumberOfPartitions() const;

    /**
       Get a partition of objects that was previously added.

       @param partitionNumber The index of the partition to retrieve. It must
                              be smaller than the numebr of partitions added.
    */
    vector<SimulationObject*>* getPartition(unsigned int partitionNumber) const;


    /**
       Add a partition of SimulationObjects. Every SimulationObject in the
       simulation must be included in exactly one partition.
    */
    void addPartition(vector<SimulationObject*>* partition);

private:
    unsigned int myNumberOfPartitions;
    vector<vector<SimulationObject*> *> myPartitions;
};

#endif
