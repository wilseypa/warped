// These unit tests use the Catch unit testing library found at
// http://catch-test.net

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main()
#include "catch.hpp"

#include "SimulationConfiguration.h"

#include <sstream>

TEST_CASE("File is opened", "[configuration]") {
    std::stringstream ss;
    ss << "{}";
    REQUIRE_NOTHROW(SimulationConfiguration c(ss));
}

TEST_CASE("configuration values are read correctly", "[configuration]") {
    std::stringstream ss;
    // Use a raw string literal delimited by `"x(` and `)x"` so that all the
    // quotes in the json don't have to be escaped.
    ss <<   R"x({
                    "string_value": "success",
                    "int_value": 123,
                    "int64_value": 1152921504606846976, // 2^60
                    "bool_true": true,
                    "bool_false": false,
                    "double_value": 0.75, // 0b0.11
                    "nested_value": {
                        "string_value": "success",
                        "nested_value": {
                            "string_value": "success"
                        }
                    }
                })x";

    SimulationConfiguration c(ss);

    SECTION("string return type") {
        REQUIRE(c.as_string({"string_value"}) == "success");
    }

    SECTION("int return type") {
        REQUIRE(c.as_int({"int_value"}) == 123);
    }

    SECTION("int64 return type") {
        REQUIRE(c.as_int64({"int64_value"}) == 1152921504606846976ull);
    }

    SECTION("true return type") {
        REQUIRE(c.as_bool({"bool_true"}) == true);
    }

    SECTION("false return type") {
        REQUIRE(c.as_bool({"bool_false"}) == false);
    }

    SECTION("test single nesting") {
        REQUIRE(c.as_string({"nested_value", "string_value"}) == "success");
    }

    SECTION("test double nesting") {
        REQUIRE(c.as_string({"nested_value", "nested_value", "string_value"}) == "success");
    }

    SECTION("default string return type") {
        REQUIRE(c.get_string({"missing_value"}, "success") == "success");
    }

    SECTION("default int return type") {
        REQUIRE(c.get_int({"missing_value"}, 123) == 123);
    }

    SECTION("default int64 return type") {
        REQUIRE(c.get_int64({"missing_value"}, 1152921504606846976ull) == 1152921504606846976ull);
    }

    SECTION("default true return type") {
        REQUIRE(c.get_bool({"missing_value"}, true) == true);
    }

    SECTION("default false return type") {
        REQUIRE(c.get_bool({"missing_value"}, false) == false);
    }
}


