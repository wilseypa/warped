
# See copyright notice in file Copyright in the root directory of this archive.

SUBDIRS= . plugins

INCLUDES = -I${PCCTSROOT} -I$(top_srcdir)/src

noinst_PROGRAMS = regressionTest

EXTRA_DIST = Greeter.h \
	configurationParserTest.cfg

regressionTest_SOURCES = \
	ArgumentParserTest.h \
	ArgumentParserTest.cpp \
	ConfigurationParserTest.h \
	ConfigurationParserTest.cpp \
	FileManagerTest.h \
	FileManagerTest.cpp \
	PluginManagerTest.h \
	PluginManagerTest.cpp \
	regressionTest.cpp \
	FieldIteratorTest.cpp \
	FieldIteratorTest.h

all-local: $(top_builddir)/test/configurationParserTest.cfg

$(top_builddir)/test/configurationParserTest.cfg:
	if [ !-e $(top_builddir)/test/configurationParserTest.cfg ]; then
	  cp $(top_srcdir)/test/configurationParserTest.cfg \
		 $(top_builddir)/test/configurationParserTest.cfg
	fi

regressionTest_LDFLAGS = \
	-static \
	-Wl,-E

regressionTest_LDADD = \
	$(top_builddir)/src/libutils.la
