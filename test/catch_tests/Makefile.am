INCLUDES=-I$(top_builddir)/thirdparty

noinst_PROGRAMS = test_SimulationConfiguration

test_SimulationConfiguration_SOURCES = test_SimulationConfiguration.cpp catch.hpp

test_SimulationConfiguration_LDFLAGS=-static
test_SimulationConfiguration_LDADD=$(top_builddir)/src/libwarped.la

TESTS=test_SimulationConfiguration