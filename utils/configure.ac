dnl Process this file with autoconf to produce a configure script.
AC_INIT([utils],[20110930])
AC_CONFIG_SRCDIR([src/utils/ConfigurationChoice.cpp])
AM_INIT_AUTOMAKE(1.7.0)
AC_PREREQ(2.57)

AC_CANONICAL_HOST
AC_LANG(C++)

dnl Checks for programs.
AC_PROG_LIBTOOL
AC_PROG_CPP
AC_PROG_CXX
AC_PROG_INSTALL
CL_PROG_PCCTS

dnl Checks for header files.
AC_HEADER_STDC

dnl Find the tr1/unordered_map header.
AC_CHECK_HEADERS([tr1/unordered_map], [break])

dnl look for the libtool headers
AC_CHECK_HEADER( [ltdl.h], ,
	AC_MSG_ERROR([Couldn't find ltdl.h which is required and 
		part of the libtool distribution.  You probably
		need to define an include path via a -I in CPPFLAGS]))

dnl look for the libtool library
AC_CHECK_LIB( [ltdl], [lt_dlopen],,
	AC_MSG_ERROR([Couldn't find libltdl.so or libltdl.a in
		link path.  You probably need to define a libary
		path via a -L in LDFLAGS]))

AC_CHECK_LIB( [ldl], [dlopen],, )

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_MODE_T
AC_STRUCT_TM

AC_CHECK_SIZEOF(long int)
AC_CHECK_SIZEOF(long long int)

dnl CPPFLAGS="$CPPFLAGS -DSIZEOF_LONG=$ac_cv_sizeof_long"
dnl CPPFLAGS="$CPPFLAGS -DSIZEOF_LONG=$ac_cv_sizeof_long_long"

dnl Checks for library functions.
AC_FUNC_MEMCMP
AC_TYPE_SIGNAL
AC_CHECK_FUNCS(mkdir strdup)

dnl Check for cppunit for unit testing

AC_CONFIG_HEADERS([src/utils/UtilsConfig.h:src/utils/UtilsConfig.h.in])

AC_OUTPUT(Makefile documentation/Makefile src/Makefile test/Makefile test/plugins/Makefile)

echo "Ready to build"

