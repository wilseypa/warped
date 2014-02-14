#include "GraphStatistics.h"

#include <fstream>
#include <iostream>
#include <string>
#include <utility>

namespace {
Edge make_edge(int a, int b) {
    return (a <= b) ? std::make_pair(a, b) : std::make_pair(b, a);
}
} // namespace

void GraphStatistics::update_edge_stat(int a, int b, const std::string& stat, int x)  {
    edge_stats[make_edge(a, b)][stat] += x;
}

int GraphStatistics::get_edge_stat(int a, int b, const std::string& stat) {
    return edge_stats[make_edge(a, b)][stat];
}

void GraphStatistics::write_to_file(const std::string& format,
                                    const std::string& filename) {
    if (format == "dot") {
        output_dot_file(filename);
    } else {
        std::cerr << "Unsupported statistics filetype: " << filename << std::endl;
    }
}

void GraphStatistics::output_dot_file(const std::string& filename) {
    std::ofstream file;
    file.open(filename);
    if (!file.is_open()) {
        std::cerr << "Could not open file " << filename << std::endl;
        return;
    }
    file << "graph {\n";
    for (auto edges_it : edge_stats) {
        file << edges_it.first.first << " -- " << edges_it.first.second;
        bool first_stat = true;
        // It would probably be a good idea to add the stat name to the label
        // if there's more than one.
        for (auto stats_it : edges_it.second) {
            if (first_stat) {
                file << " [label=\"";
                first_stat = false;
            } else {
                file << ", ";
            }
            file << stats_it.second;
        }
        if (!first_stat) {
            file << "\"]";
        }
        file << ";\n";

    }
    file << "}";
    file.close();
}