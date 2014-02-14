#include <iostream>
#include <map>
#include <string>
#include <utility>

typedef std::pair<int, int> Edge;

class GraphStatistics {
public:
    void update_edge_stat(int a, int b, const std::string& stat, int x = 1);
    int get_edge_stat(int a, int b, const std::string& stat);

    /**
    Write the statistics to a file

    @param format The format of the file. Supported formats are "dot"
    @param filename The path to the file that will be created.
    */
    void write_to_file(const std::string& format, const std::string& filename);

    std::map<Edge, std::map<std::string, int>> edge_stats;

private:
    void output_dot_file(const std::string& filename);
};