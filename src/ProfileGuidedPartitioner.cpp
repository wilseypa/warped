#include "PartitionInfo.h"
#include "ProfileGuidedPartitioner.h"
#include "warped.h"
#include "metis.h"

#include <vector>
#include <set>
#include <string>
#include <sstream>
#include <iostream>
#include <stdexcept>

ProfileGuidedPartitioner::ProfileGuidedPartitioner(std::istream& input)
    : input(input) {}

const PartitionInfo*
ProfileGuidedPartitioner::partition(const vector<SimulationObject*>* objects,
                                    const unsigned int numPartitions) const {

    PartitionInfo* partition_info = new PartitionInfo(numPartitions);

    if (numPartitions == 1) {
        auto retvec = new vector<SimulationObject*>(*objects);
        partition_info->addPartition(retvec);
        return partition_info;
    }

    vector< vector<SimulationObject*>* > partitions(numPartitions);

    for (unsigned int i = 0; i < numPartitions; i++) {
        partitions[i] = new vector<SimulationObject*>;
    }

    // A map of METIS node number -> SimulationObject index
    // This is necessary since there may be objects that didn't have any
    // statistics, and so won't be partitioned by Metis. Metis files start
    // counting at 1, so indexes[0] is unused.
    std::vector<int> indexes(objects->size() + 1);

    // METIS parameters
    // idx_t is a METIS typedef
    idx_t nvtxs; // number of verticies
    idx_t ncon = 1; // number of constraints
    idx_t nparts = numPartitions; // number of partitions
    std::vector<idx_t> xadj; // part of the edge list
    std::vector<idx_t> adjncy; // part of the edge list
    std::vector<idx_t> adjwgt; // edge weights
    idx_t edgecut; // output var for the final communication volume
    std::vector<idx_t> part(objects->size()); // output var for partition list

    if (!input) {
        throw std::runtime_error("Could not read file.");
    }

    xadj.push_back(0);
    int i = 0;
    for (std::string line; std::getline(input, line);) {
        if (line[0] == '%') {
            // Skip comment lines unless they are a map comment
            if (line[1] != ':') { continue; }
            int x = std::stoi(line.substr(2));

            indexes[i] = x;
            continue;
        }

        std::istringstream iss(line);
        if (i == 0) {
            int nedges, format;
            iss >> nvtxs >> nedges >> format;
            if (format != 1) {
                throw std::runtime_error("Graph format must be 001.");
            }
            if (nvtxs > objects->size()) {
                throw std::runtime_error("Invalid statistics file for this simulation.");
            }
        } else {
            int vertex, weight;
            while (iss >> vertex >> weight) {
                // The file format counts from 1, but the API counts from 0. Cool.
                adjncy.push_back(vertex - 1);
                adjwgt.push_back(weight);
            }
            xadj.push_back(adjncy.size());
        }

        i++;
    }

    METIS_PartGraphKway(&nvtxs,     // nvtxs
                        &ncon,      // ncon
                        &xadj[0],   // xadj
                        &adjncy[0], // adjncy
                        NULL,       // vwgt
                        NULL,       // vsize
                        &adjwgt[0], // adjwgt
                        &nparts,    // nparts
                        NULL,       // tpwgts
                        NULL,       // ubvec
                        NULL   ,    // options
                        &edgecut,   // edgecut
                        &part[0]    // part
                       );

    // Create a set of all object indices so that we can find any that weren't
    // partitioned by Metis
    std::set<int> remaining_objects;
    for (int i = 0; i < objects->size(); i++) {
        remaining_objects.insert(i);
    }

    // Add the metis output to partitons
    for (int i = 1; i < nvtxs + 1; i++) {
        remaining_objects.erase(indexes[i]);
        partitions[part[i]]->push_back((*objects)[indexes[i]]);
    }

    // Add any objects that metis didn't partition
    for (auto i : remaining_objects) {
        partitions[i % numPartitions]->push_back((*objects)[i]);
    }

    // Fill the PartitionInfo object
    for (int i = 0; i < numPartitions; i++) {
        partition_info->addPartition(partitions[i]);
    }

    return partition_info;
}
