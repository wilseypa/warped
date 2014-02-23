#include "PartitionInfo.h"
#include "ProfileGuidedPartitioner.h"
#include "warped.h"
#include "metis.h"

#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

#include <stdexcept>

ProfileGuidedPartitioner::ProfileGuidedPartitioner() {}

const PartitionInfo*
ProfileGuidedPartitioner::partition(const vector<SimulationObject*>* objects,
                                    const unsigned int numLPs) const {

    PartitionInfo* partition_info = new PartitionInfo(numLPs);

    if (numLPs == 1) {
        auto retvec = new vector<SimulationObject*>(*objects);
        partition_info->addPartition(0, retvec);
        return partition_info;
    }

    vector< vector<SimulationObject*>* > partitions(numLPs);

    for (unsigned int i = 0; i < numLPs; i++) {
        partitions[i] = new vector<SimulationObject*>;
    }

    idx_t nvtxs; // number of verticies
    idx_t ncon = 1; // number of constraints
    idx_t nparts = numLPs; // number of partitions
    std::vector<idx_t> xadj; // part of the edge list
    std::vector<idx_t> adjncy; // part of the edge list
    std::vector<idx_t> adjwgt; // edge weights
    idx_t edgecut; // output var for the final communication volume
    std::vector<idx_t> part(objects->size()); // output var for partition list

    std::ifstream infile("statistics.metis");
    if (!infile) {
        throw std::runtime_error("Could not open file.");
    }

    xadj.push_back(0);
    int i = 0;
    for (std::string line; std::getline(infile, line);) {
        if (line[0] == '%') { continue; }
        std::istringstream iss(line);

        if (i == 0) {
            int nedges, format;
            iss >> nvtxs >> nedges >> format;
            if (format != 1) {
                throw std::runtime_error("Graph format must be 001.");
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

    if (nvtxs != objects->size()) {
        std::ostringstream msg;
        msg << "nvtxs (" << nvtxs << ") and objects.size (" << objects->size() << ") differ";
        throw std::runtime_error(msg.str());
    }

    std::cout << "calling METIS with nparts=" << nparts << std::endl;
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
    std::cout << "Done calling METIS" << std::endl;


    unsigned int n = 0;
    while (n < objects->size()) {
        std::cout << n << ' ' << part[n] << '\n';
        partitions[part[n]]->push_back((*objects)[n]);
        n++;
    }

    for (unsigned int i = 0; i < numLPs; i++) {
        partition_info->addPartition(i, partitions[i]);
    }

    return partition_info;
}
