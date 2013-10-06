noinst_PROGRAMS = test_SimulationConfiguration

test_SimulationConfiguration_SOURCES = test_SimulationConfiguration.cpp catch.hpp
test_SimulationConfiguration_CPPFLAGS = -I$(top_srcdir)/thirdparty -I$(top_srcdir)/src
test_SimulationConfiguration_LDFLAGS=-static
test_SimulationConfiguration_LDADD=$(top_builddir)/src/libwarped.la

EXTRA_DIST = test_file.json

TESTS=test_SimulationConfiguration