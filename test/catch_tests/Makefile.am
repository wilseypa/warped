noinst_PROGRAMS = test_SimulationConfiguration test_GraphStatistics

CPPFLAGS = -I$(top_srcdir)/thirdparty -I$(top_srcdir)/src
LDFLAGS=-static
LDADD=$(top_builddir)/src/libwarped.la

test_SimulationConfiguration_SOURCES = test_SimulationConfiguration.cpp catch.hpp
test_GraphStatistics_SOURCES = test_GraphStatistics.cpp catch.hpp

TESTS=test_SimulationConfiguration test_GraphStatistics