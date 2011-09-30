
# See copyright notice in file Copyright in the root directory of this archive.

library_includedir=$(includedir)/utils
library_include_HEADERS = \
	utils/AllocatorStack.h \
	utils/ArgumentParser.h \
	$(top_builddir)/src/utils/UtilsConfig.h \
	utils/ConfigurationParserHandle.h \
	utils/ConfigurationScope.h \
	utils/ConfigurationValue.h \
	utils/ConfigurationChoice.h \
	utils/ConfigurationLexer.h \
	utils/Debug.h \
	utils/Iterator.h \
	utils/FileManager.h \
	utils/PosixFileManager.h \
	utils/PluginBase.h  \
	utils/PluginManager.h \
	utils/Set.h \
	utils/StringHashMap.h \
	utils/StringUtilities.h \
	utils/tokens.h \
	utils/FieldIterator.h \
	utils/VarArgs.h \
	utils/VectorIterator.h

lib_LTLIBRARIES = libutils.la
libutils_la_LIBADD = -lltdl

INSTALL_HEADER = $(INSTALL) -p -m 644

INCLUDES = -I${PCCTSROOT}
SRCDIR=$(top_srcdir)/src/utils

EXTRA_DIST = \
	$(SRCDIR)/UtilsConfig.h.in \
	utils/ConfigurationParser.gg \
	utils/utils-config.h.in

BUILT_SOURCES=\
	$(SRCDIR)/ConfigurationLexer.cpp \
	$(SRCDIR)/ConfigurationLexer.h \
	$(SRCDIR)/parser.dlg \
	$(SRCDIR)/tokens.h \
	$(SRCDIR)/ConfigurationParserHandle.cpp \
	$(SRCDIR)/ConfigurationParser.cpp \
	$(SRCDIR)/ConfigurationParserHandle.h \
	$(SRCDIR)/AParser.cpp \
	$(SRCDIR)/ATokenBuffer.cpp \
	$(SRCDIR)/BufFileInput.cpp \
	$(SRCDIR)/DLexerBase.cpp \
	$(SRCDIR)/slist.cpp

libutils_la_SOURCES = \
	$(BUILT_SOURCES) \
	utils/ArgumentParser.cpp \
	utils/parser.dlg \
	utils/ConfigurationChoice.cpp \
	utils/ConfigurationScope.cpp \
	utils/ConfigurationValue.cpp \
	utils/Debug.cpp \
	utils/FileManager.cpp \
	utils/PluginBase.cpp \
	utils/PluginManager.cpp \
	utils/PosixFileManager.cpp \
	utils/StringHashMap.cpp \
	utils/StringUtilities.cpp \
	utils/FieldIterator.cpp

$(SRCDIR)/AParser.cpp: $(SRCDIR)/ConfigurationParser.gg
	cp $(PCCTSROOT)/AParser.cpp $(SRCDIR)/AParser.cpp

$(SRCDIR)/ATokenBuffer.cpp:
	cp $(PCCTSROOT)/ATokenBuffer.cpp $(SRCDIR)/ATokenBuffer.cpp

$(SRCDIR)/BufFileInput.cpp:
	cp $(PCCTSROOT)/BufFileInput.cpp $(SRCDIR)/BufFileInput.cpp

$(SRCDIR)/DLexerBase.cpp:
	cp $(PCCTSROOT)/DLexerBase.cpp $(SRCDIR)/DLexerBase.cpp

$(SRCDIR)/slist.cpp:
	cp $(PCCTSROOT)/slist.cpp $(SRCDIR)/slist.cpp

DO_DLG= $(DLG) -C2 -CC $(SRCDIR)/parser.dlg -cl ConfigurationLexer -o $(SRCDIR)

DO_ANTLR= $(ANTLR) -k 1 -CC -o $(SRCDIR) $(SRCDIR)/ConfigurationParser.gg && $(DO_DLG)

$(SRCDIR)/parser.dlg: $(SRCDIR)/ConfigurationParser.gg
	$(DO_ANTLR)

$(SRCDIR)/ConfigurationLexer.cpp: $(SRCDIR)/parser.dlg
	$(DO_DLG)

$(SRCDIR)/ConfigurationLexer.h: $(SRCDIR)/parser.dlg
	$(DO_DLG)

$(SRCDIR)/tokens.h: $(SRCDIR)/ConfigurationParser.gg
	$(DO_ANTLR)

$(SRCDIR)/ConfigurationParserHandle.cpp: $(SRCDIR)/ConfigurationParser.gg
	$(DO_ANTLR)

$(SRCDIR)/ConfigurationParser.cpp: $(SRCDIR)/ConfigurationParser.gg
	$(DO_ANTLR)

$(SRCDIR)/ConfigurationParserHandle.h: $(SRCDIR)/ConfigurationParser.gg
	$(DO_ANTLR)

$(BUILDDIR)/src/ConfigurationLexer.o: \
	$(SRCDIR)/ConfigurationLexer.h $(SRCDIR)/tokens.h 

$(BUILDDIR)/src/ConfigurationLexer.lo: \
	$(SRCDIR)/ConfigurationLexer.h $(SRCDIR)/tokens.h 

$(BUILDDIR)/src/ConfigurationParser.o: \
	$(SRCDIR)/ConfigurationLexer.h $(SRCDIR)/tokens.h \
	$(SRCDIR)/ConfigurationParserHandle.h 

$(BUILDDIR)/src/ConfigurationParserHandle.o: \
	$(SRCDIR)/tokens.h $(SRCDIR)/ConfigurationParserHandle.h

CLEANFILES=$(BUILT_SOURCES)

