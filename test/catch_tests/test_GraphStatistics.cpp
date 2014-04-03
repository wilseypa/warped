// These unit tests use the Catch unit testing library found at
// http://catch-test.net

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main()
#include "catch.hpp"

#include "GraphStatistics.h"

TEST_CASE("graph statistics edges can be added", "[stats]") {

    GraphStatistics stats;

    stats.update_edge_stat(1, 2, "s", 1);

    SECTION("single stat, single edge") {
        REQUIRE(stats.get_edge_stat(1, 2, "s") == 1);
    }

    SECTION("single stat twice, single edge") {
        stats.update_edge_stat(1, 2, "s", 1);
        
        REQUIRE(stats.get_edge_stat(1, 2, "s") == 2);
    }

    SECTION("two stats, single edge") {
        stats.update_edge_stat(1, 2, "s2", 1);

        REQUIRE(stats.get_edge_stat(1, 2, "s") == 1);
        REQUIRE(stats.get_edge_stat(1, 2, "s2") == 1);
    }

    SECTION("single stat, two edges") {
        stats.update_edge_stat(2, 3, "s", 1);

        REQUIRE(stats.get_edge_stat(1, 2, "s") == 1);
        REQUIRE(stats.get_edge_stat(2, 3, "s") == 1);
    }

    SECTION("undirected edges") {
        stats.update_edge_stat(2, 1, "s", 1);

        REQUIRE(stats.get_edge_stat(1, 2, "s") == 2);
    }
}