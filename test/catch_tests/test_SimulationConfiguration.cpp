// These unit tests use the Catch unit testing library found at
// http://catch-test.net

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main()
#include "catch.hpp"

#include "SimulationConfiguration.h"

TEST_CASE("File is opened", "[configuration]") {
    REQUIRE_NOTHROW(SimulationConfiguration c("test_file.json"));
}

TEST_CASE("string return type", "[configuration]") {
    SimulationConfiguration c("test_file.json");
    REQUIRE(c.as_string({"string_value"}) == "success");
}

TEST_CASE("int return type", "[configuration]") {
    SimulationConfiguration c("test_file.json");
    REQUIRE(c.as_int({"int_value"}) == 123);
}

TEST_CASE("int64 return type", "[configuration]") {
    SimulationConfiguration c("test_file.json");
    REQUIRE(c.as_int64({"int64_value"}) == 1152921504606846976ull);
}

TEST_CASE("true return type", "[configuration]") {
    SimulationConfiguration c("test_file.json");
    REQUIRE(c.as_bool({"bool_true"}) == true);
}

TEST_CASE("false return type", "[configuration]") {
    SimulationConfiguration c("test_file.json");
    REQUIRE(c.as_bool({"bool_false"}) == false);
}

TEST_CASE("test single nesting", "[configuration]") {
    SimulationConfiguration c("test_file.json");
    REQUIRE(c.as_string({"nested_value", "string_value"}) == "success");
}

TEST_CASE("test double nesting", "[configuration]") {
    SimulationConfiguration c("test_file.json");
    REQUIRE(c.as_string({"nested_value", "nested_value", "string_value"}) == "success");
}

TEST_CASE("default string return type", "[configuration]") {
    SimulationConfiguration c("test_file.json");
    REQUIRE(c.get_string({"missing_value"}, "success") == "success");
}
TEST_CASE("default int return type", "[configuration]") {
    SimulationConfiguration c("test_file.json");
    REQUIRE(c.get_int({"missing_value"}, 123) == 123);
}
TEST_CASE("default int64 return type", "[configuration]") {
    SimulationConfiguration c("test_file.json");
    REQUIRE(c.get_int64({"missing_value"}, 1152921504606846976ull) == 1152921504606846976ull);
}
TEST_CASE("default true return type", "[configuration]") {
    SimulationConfiguration c("test_file.json");
    REQUIRE(c.get_bool({"missing_value"}, true) == true);
}
TEST_CASE("default false return type", "[configuration]") {
    SimulationConfiguration c("test_file.json");
    REQUIRE(c.get_bool({"missing_value"}, false) == false);
}
