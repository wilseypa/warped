#include "GraphStatistics.h"

#include <algorithm>
#include <set>

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
    std::ofstream file;
    if (format == "graphviz" || format == "metis") {
        file.open(filename);
        if (!file.is_open()) {
            std::cerr << "Could not open file " << filename << std::endl;
            return;
        }
    }

    if (format == "graphviz") {
        output_graphviz_file(file);
    } else if (format == "metis") {
        output_metis_file(file);
    } else {
        std::cerr << "Unsupported statistics filetype: " << filename << std::endl;
        return;
    }
    file.close();
}

void GraphStatistics::output_graphviz_file(std::ofstream& file) {
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
    file << "}\n";
}

void GraphStatistics::output_metis_file(std::ofstream& file) {
    // Metis files require a list of every node, so we have to build a map of
    // edges in both directions.
    std::map<Edge, std::map<std::string, int>> full_graph(edge_stats);

    // Count the vertices and stats.
    std::set<int> vertices;
    std::set<std::string> stats;
    for (auto edges_it : edge_stats) {
        vertices.insert(edges_it.first.first);
        vertices.insert(edges_it.first.second);
        for (auto stats_it : edges_it.second) {
            full_graph[std::make_pair(edges_it.first.second,
                                      edges_it.first.first)][stats_it.first] = stats_it.second;
            stats.insert(stats_it.first);
        }
    }
    int numEdges = edge_stats.size();
    int numVertices = vertices.size();
    int numWeights = stats.size();

    file << "%% The first line contains the following information:\n"
         << "%% <# of vertices> <# of edges> <file format> <# of weights>\n"
         <<  numVertices << ' ' << numEdges << ' ' << "001 " << numWeights << '\n'
         << "%% The remaining lines each describe a vertex and have the following format:\n"
         << "%% <<neighbor>";
    for (auto& stat : stats) {
        file << " <" << stat << '>';
    }
    file << "> ...\n";

    // Metis requires node numbering start at 1 instead of 0, so add one to
    // all indices if node 0 exists
    int offset = vertices.count(0);
    // Keep track of the last vertex, since each vertex is on its own line
    int last_vertex = -1;
    for (auto edges_it : full_graph) {
        // Metis doesn't accept self-edges
        if (edges_it.first.first == edges_it.first.second) {
            continue;
        }
        if (edges_it.first.first != last_vertex) {
            if (last_vertex >= 0) { file << '\n'; }
            last_vertex = edges_it.first.first;
        }
        file << edges_it.first.second + offset << ' ';
        // iterate over stats instead of edges_it.second in case an edge
        // doesn't have every weight
        for (auto stat : stats) {
            // Metis requires weights be non-zero
            int weight = std::max(1, edges_it.second[stat]);
            file << weight << ' ';
        }
    }
    file << '\n';
}