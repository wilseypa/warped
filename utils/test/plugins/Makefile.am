
# See copyright notice in file Copyright in the root directory of this archive.

noinst_LTLIBRARIES = FrenchGreeter.la
FrenchGreeter_la_LDFLAGS= \
	-module \
	-no-undefined \
	-avoid-version \
	-L$(top_builddir)/src

FrenchGreeter_la_LIBADD= \
	-lutils

INCLUDES=-I$(top_srcdir)/src

FrenchGreeter_la_SOURCES =\
	FrenchGreeter.cpp \
	FrenchGreeter.h
