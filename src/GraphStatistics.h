#include <iostream>
#include <map>
#include <string>
#include <utility>

typedef std::pair<int, int> Edge;

class GraphStatistics {
public:
    void update_edge_stat(int a, int b, const std::string& stat, int x = 1);
    int get_edge_stat(int a, int b, const std::string& stat); 

    void output_dot_file(const std::string& filename);

    // Although unorderd_map might be a little faster, we'd have to specialze
    // pair<int, int> to be hashable. We can use it as-is with an ordered map.
    std::map<Edge, std::map<std::string, int>> edge_stats;
};