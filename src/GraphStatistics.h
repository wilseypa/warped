#include <iostream>
#include <map>
#include <utility>
#include <string>

typedef std::pair<int, int> Edge;

class GraphStatistics {
public:
    inline void update_edge_stat(int a, int b, const std::string& stat, int x = 1)  {
        edge_stats[std::make_pair(a, b)][stat] += x;
    }

    inline int get_edge_stat(int a, int b, const std::string& stat) {
        return edge_stats[std::make_pair(a, b)][stat];
    }

    void output_dot_file(const std::string& filename);

    // Although unorderd_map might be a little faster, we'd have to specialze
    // pair<int, int> to be hashable. We can use it as-is with an ordered map.
    std::map<Edge, std::map<std::string, int>> edge_stats;
};