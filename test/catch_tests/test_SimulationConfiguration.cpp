// These unit tests use the Catch unit testing library found at 
// http://catch-test.net

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main()
#include "catch.hpp"

#include "SimulationConfiguration.h"


TEST_CASE("File is opened", "[configuration]") {
    REQUIRE_NOTHROW(SimulationConfiguration c("test_file.json"));
}

TEST_CASE("Return types", "[configuration]") {
    SimulationConfiguration c("test_file.json");
    REQUIRE( c.as_string({"string_value"}) == "success" );
    REQUIRE( c.as_int({"int_value"}) == 123 );
    REQUIRE( c.as_int64({"int64_value"}) == 1152921504606846976ull );
    REQUIRE( c.as_bool({"bool_true"}) == true );
    REQUIRE( c.as_bool({"bool_false"}) == false );
    REQUIRE( c.as_int({"int_value"}) == 123 );
}

