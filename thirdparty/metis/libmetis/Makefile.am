LIBMETIS_H_FILES = \
    defs.h \
    gklib_defs.h \
    gklib_rename.h \
    macros.h \
    metislib.h \
    proto.h \
    rename.h \
    stdheaders.h \
    struct.h 

LIBMETIS_C_FILES = \
    auxapi.c \
    balance.c \
    bucketsort.c \
    checkgraph.c \
    coarsen.c \
    compress.c \
    contig.c \
    debug.c \
    fm.c \
    fortran.c \
    frename.c \
    gklib.c \
    graph.c \
    initpart.c \
    kmetis.c \
    kwayfm.c \
    kwayrefine.c \
    mcutil.c \
    mesh.c \
    meshpart.c \
    minconn.c \
    mincover.c \
    mmd.c \
    ometis.c \
    options.c \
    parmetis.c \
    pmetis.c \
    refine.c \
    separator.c \
    sfm.c \
    srefine.c \
    stat.c \
    timing.c \
    util.c \
    wspace.c 

noinst_LTLIBRARIES = libmetis.la

libmetis_la_CPPFLAGS = -I$(top_builddir)/thirdparty/metis/include -I$(top_builddir)/thirdparty/metis/GKlib
libmetis_la_LIBADD = $(top_builddir)/thirdparty/metis/GKlib/libgk.la

libmetis_la_SOURCES = \
    $(LIBMETIS_H_FILES) \
    $(LIBMETIS_C_FILES)