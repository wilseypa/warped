#include <string>
#include <iostream>
#include <fstream>

#include "GraphStatistics.h"

void GraphStatistics::output_dot_file(const std::string& filename) {
    std::ofstream file;
    file.open(filename);
    file << "graph {\n";
    for (auto edges_it : edge_stats) {
        file << edges_it.first.first << " -- " << edges_it.first.second;
        bool first_stat = true;
        // It would probably be a good idea to add the stat to the label if
        // there's more than one.
        for (auto stats_it : edges_it.second) {
            if(first_stat) {
                file << " [label=\"";
                    first_stat = false;
            } else {
                file << ", ";
            }
            file << stats_it.second;
        }
        if(!first_stat) {
            file << "\"]";
        }
        file << ";\n";

    }
    file << "}";
    file.close();
}