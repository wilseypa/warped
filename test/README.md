# WARPED Unit Tests

This directory contains all unit tests for the WARPED library.

For historical reasons, the WARPED unit tests are written with two different frameworks: [CppUnit](http://cppunit.sourceforge.net) and [Catch](http://catch-test.net/). The tests are located in the `cppunit_tests` and `catch_tests` directories, respectively. 

## Usage

To build the tests written with CppUnit, you must install the CppUnit library separately before building WARPED, otherwise only the tests written with Catch will be built.

The Catch library is included with the WARPED source, so installing it separately is not required. All tests written with Catch are built automatically.

The following command builds warped and runs all unit tests if the build was successful:

	autoreconf -i && ./configure && make && make check

