
# See copyright notice in file Copyright in the root directory of this archive.

CPPFLAGS=$(GLOBALCPPFLAGS) @CPPFLAGS@
CXXFLAGS=$(GLOBALCXXFLAGS) @CXXFLAGS@
LDFLAGS=$(GLOBALLDFLAGS) @LDFLAGS@
# ACLOCAL_AMFLAGS = -I m4

SUBDIRS= src simulationmodels

EXTRA_DIST = Makefile.am.common \
	src/warped/Makefile.am.stub \
	src/warped/eclmpl/Makefile.am.stub \
	src/warped/WarpedConfig.h.in \
	simulationmodels/pingPong/Makefile.am.stub \
	simulationmodels/warped/Makefile.am.stub
