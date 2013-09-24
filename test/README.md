# WARPED Unit Tests

These tests are written with the CPPUnit library, which must be installed on your machine to run these tests. To run the tests, pass the `--enable-cppunit` option to `configure` when building WARPED. 

The following command builds warped and runs all unit tests if the build was successful:

	autoreconf -i && ./configure --prefix=~/lib/warped --enable-cppunit && make && make check

