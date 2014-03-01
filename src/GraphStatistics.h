#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <fstream>

typedef std::pair<int, int> Edge;

class GraphStatistics {
public:
    void update_edge_stat(int a, int b, const std::string& stat, int value = 1);
    int get_edge_stat(int a, int b, const std::string& stat);

    /**
    Write the statistics to a file

    @param format The format of the file. Supported formats are: "graphviz", "metis"
    @param filename The path to the file that will be created.
    */
    void write_to_file(const std::string& format, const std::string& filename);

    /** edge_stats is an ordered map, so iterating over it will group edges
        together based on their first vertex
    */
    std::map<Edge, std::map<std::string, int>> edge_stats;

private:
    void output_graphviz_file(std::ofstream& file);
    void output_metis_file(std::ofstream& file);
};