// These unit tests use the Catch unit testing library found at
// http://catch-test.net

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main()
#include "catch.hpp"

#include "ProfileGuidedPartitioner.h"
#include "SimulationObject.h"
#include "State.h"
#include "PartitionInfo.h"

#include <sstream>
#include <vector>
#include <algorithm>

class MockObject : public SimulationObject {
public:
    MockObject() {}
    ~MockObject() {}

    void initialize() {}
    void finalize() {}
    void executeProcess() {}
    State* allocateState() {return 0;}
    void deallocateState(const State* state) { delete state; }
    void reclaimEvent(const Event* event) { delete event; }
    const string& getName() const { return ""; }
};

TEST_CASE("ProfileGuidedPartitioner partitions fully connected graph", "[partitioning]") {
    std::stringstream input;
    input <<
          "2 1 001\n"
          "%: 0\n"
          "2 1\n"
          "%: 1\n"
          "1 1\n";

    int num_parts = 2;

    std::vector<SimulationObject*> objects {new MockObject(), new MockObject()};

    auto pinfo = ProfileGuidedPartitioner(input).partition(&objects, num_parts);

    SECTION("correct number of partitions") {
        REQUIRE(pinfo->getNumberOfPartitions() == num_parts);
    }

    std::vector<SimulationObject*> partitioned_obs;

    for (int i = 0; i < num_parts; ++i) {
        for (auto o : *pinfo->getPartition(i)) {
            partitioned_obs.push_back(o);
        }
    }

    SECTION("all objects contained") {
        REQUIRE(partitioned_obs.size() == objects.size());
    }

    SECTION("all objects valid") {
        for (auto ob : partitioned_obs) {
            REQUIRE(std::find(objects.begin(), objects.end(), ob) != objects.end());
        }
    }

    SECTION("all objects unique") {
        REQUIRE(partitioned_obs[0] != partitioned_obs[1]);
    }

}

TEST_CASE("ProfileGuidedPartitioner partitions graph with extra nodes", "[partitioning]") {
    std::stringstream input;
    input <<
          "2 1 001\n"
          "%: 0\n"
          "2 1\n"
          "%: 1\n"
          "1 1\n";

    int num_parts = 2;

    std::vector<SimulationObject*> objects {new MockObject(), new MockObject(), new MockObject()};

    auto pinfo = ProfileGuidedPartitioner(input).partition(&objects, num_parts);

    SECTION("correct number of partitions") {
        REQUIRE(pinfo->getNumberOfPartitions() == num_parts);
    }

    std::vector<SimulationObject*> partitioned_obs;

    for (int i = 0; i < num_parts; ++i) {
        for (auto o : *pinfo->getPartition(i)) {
            partitioned_obs.push_back(o);
        }
    }

    SECTION("all objects contained") {
        REQUIRE(partitioned_obs.size() == objects.size());
    }

    SECTION("all objects valid") {
        for (auto ob : partitioned_obs) {
            REQUIRE(std::find(objects.begin(), objects.end(), ob) != objects.end());
        }
    }

    SECTION("all objects unique") {
        REQUIRE(partitioned_obs[0] != partitioned_obs[1]);
        REQUIRE(partitioned_obs[0] != partitioned_obs[2]);
        REQUIRE(partitioned_obs[1] != partitioned_obs[2]);
    }

}