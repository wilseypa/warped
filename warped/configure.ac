dnl Process this file with autoconf to produce a configure script.
AC_INIT([warped],[3.0])
AC_CONFIG_HEADERS([src/warped/WarpedConfig.h])

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([src/warped/Event.cpp])
AM_INIT_AUTOMAKE
AC_PREREQ(2.61)

AC_SUBST(WARPED_VERSION, [2.0-pre031216])

AC_CANONICAL_HOST
AC_LANG(C++)

dnl Checks for programs.
AC_PROG_LIBTOOL
AC_PROG_CXX
AC_PROG_CXXCPP
AC_PROG_INSTALL
LT_INIT

AC_CHECK_LIB(nsl, gethostent)
AC_CHECK_LIB(socket,socket)

AH_TEMPLATE([USE_TIMEWARP], [Compile in TIMEWARP support])
AH_TEMPLATE([USE_CPPUNIT], [Compile cppunit tests])

dnl Check to see if TIMEWARP was enabled.
AC_ARG_ENABLE(timewarp,
 AS_HELP_STRING([--enable-timewarp],[build support for timewarp]),
 [AC_DEFINE(USE_TIMEWARP)]
 [case "${enableval}" in
   yes) timewarp=true ;;
   no)  timewarp=false ;;
   *) AC_MSG_ERROR(bad value ${enableval} for --enable-timewarp) ;;
 esac],[timewarp=false])
 AM_CONDITIONAL(USE_TIMEWARP, test $timewarp = true)

dnl Find the tr1/unordered_map header.
AC_CHECK_HEADERS([tr1/unordered_map], [break])

havempi=no
havempich=no
if test x$timewarp = xtrue; then
AC_CHECK_HEADER( mpi.h,,
	AC_MSG_ERROR(Couldn't find mpi.h and you asked to enable mpi.  You probably need to define an include path.) )
AC_CHECK_LIB( mpi, MPI_Init, havempi=yes,
	AC_CHECK_LIB( mpich, MPI_Init, havempich=yes,
	  AC_MSG_ERROR([Could neither find libmpi.a nor libmpich.a and you asked to enable mpi.  You probably need to define a lib path via LDFLAGS and -L]), -lmpl  -lpthread))
fi
AM_CONDITIONAL(HAVE_MPICH, test $havempich = yes)
AM_CONDITIONAL(HAVE_MPI, test $havempi = yes)


AH_TEMPLATE([SSH_COMMAND],[When spawning remote processes via ssh, use the command specified in SSH_COMMAND])
AC_ARG_VAR([SSH_COMMAND],[When spawning remote processes via ssh, use the command specified in SSH_COMMAND])
dnl Check if we're configuring with --with-ssh, e.g. --with-ssh=/usr/bin/ssh
AC_ARG_WITH(ssh, AS_HELP_STRING([--with-ssh],[location of ssh]), 
 [AC_MSG_CHECKING(for $withval)
    if test -f $withval; then
        AC_DEFINE_UNQUOTED( SSH_COMMAND, $withval )
        AC_MSG_RESULT(yes)
    else
        AC_MSG_RESULT(no)
        AC_MSG_ERROR(Couldn't find ${withval} and you specified --with-ssh=${withval}. You need to specify a correct path to ssh.)
    fi]
 , 
 [AC_PATH_PROG(ssh_path, ssh)
    if test x$ssh_path = x; then
        AC_MSG_ERROR(You need to specify a correct path to ssh with the configure option --with-ssh=.)
    else
		AC_DEFINE_UNQUOTED( SSH_COMMAND, $ssh_path )
   fi]
)

AC_CHECK_SIZEOF( long )
AC_CHECK_SIZEOF( long long )

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(unistd.h)

dnl Look for libutils
LIB_UTILS

dnl Look for cppunit
dnl Check to see if cppunit was enabled.
AC_ARG_ENABLE(cppunit,
AS_HELP_STRING([--enable-cppunit],[build support for cppunit]),
AC_DEFINE(USE_CPPUNIT)
[case "${enableval}" in
  yes) cppunit=true ;;
  no)  cppunit=false ;;
  *) AC_MSG_ERROR(bad value ${enableval} for --enable-cppunit) ;;
esac],[cppunit=false])
AM_CONDITIONAL(USE_CPPUNIT, test x$cppunit = xtrue)

if test x$cppunit = xtrue; then
LIB_CPPUNIT
fi

dnl Look for PCCTS headers.  That's all we need for warped - we don't need
dnl the binaries.
CL_PCCTS_HEADERS

AH_TEMPLATE(USE_SOCKLEN_T, [LINUX platforms require "socklen_t" while
SOLARIS platforms require it to be an "int".  This macro tells use whether
or not to use socklen_t.])

found_sockopt_type=0
AC_MSG_CHECKING([does getsockopt want socklen_t])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <sys/types.h>
#include <sys/socket.h>]], [[socklen_t dummy; int rc, skt; void *ssz;
getsockopt(skt,SOL_SOCKET,SO_SNDBUF,ssz,&dummy);]])],[found_sockopt_type=1],[])
case $found_sockopt_type in 
     0) AC_MSG_RESULT([no])
     ;;
     1) AC_MSG_RESULT([yes])
	AC_DEFINE([USE_SOCKLEN_T], 1)
     ;;
esac

have_getcpu=false
AH_TEMPLATE(HAVE_SCHED_GETCPU, [Defines whether sched_getcpu function exists])
AC_CHECK_FUNC([sched_getcpu], [AC_DEFINE([HAVE_SCHED_GETCPU], [1])]
  have_getcpu=true)
AM_CONDITIONAL(USE_DVFS, test x$have_getcpu = xtrue)

AH_TEMPLATE(USE_CHAR_FOR_VOID,
	[Solaris 7:   int setsockopt(int,int,int,const void *, int );
	Solaris 2.6: int setsockopt(int,int,int,const void *, size_t ); (3XN)
	Solaris 2.6: int setsockopt(int,int,int,const char *, int ); (3N)
	Linux:       int setsockopt(int,int,int,const void *, socklen_t );] )

AH_TEMPLATE(USE_SIZE_T,
	[See comments for USE_CHAR_FOR_VOID])

dnl While LINUX requires socklen_t for some functions, SOLARIS requires
dnl int sometimes and size_t sometimes. In addition, depending on certain 
dnl flags, Solaris 2.6 might have a different function signature when compared
dnl to Solaris .27. This check will take care of that.
dnl These are currently known signatures:
dnl Solaris 7:   int setsockopt(int,int,int,const void *, int );
dnl Solaris 2.6: int setsockopt(int,int,int,const void *, size_t ); (3XN)
dnl Solaris 2.6: int setsockopt(int,int,int,const char *, int ); (3N)
dnl Linux:       int setsockopt(int,int,int,const void *, socklen_t );
found_sockopt_type2=0
found_sockopt_type3=0
AC_MSG_CHECKING([does setsockopt want (void *) and size_t])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <sys/socket.h>]], [[size_t dummy; int l, m, rc, skt; const void *ssz;
setsockopt(skt,l,m,ssz,dummy);]])],[found_sockopt_type2=1],[])
case $found_sockopt_type2 in 
     0) AC_MSG_RESULT(no)
        AC_MSG_CHECKING([if setsockopt wants (char *) and int])
        AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <sys/socket.h>]], [[int dummy; int l, m, rc, skt; const char *ssz;
                       setsockopt(skt,l,m,ssz,dummy);]])],[found_sockopt_type3=1],[])
        case $found_sockopt_type3 in
        0) AC_MSG_RESULT([no])
        ;;
        1) AC_MSG_RESULT([yes])
           AC_DEFINE([USE_CHAR_FOR_VOID], 1)
        ;;
        esac
     ;;
     1) AC_MSG_RESULT([yes])
           AC_DEFINE([USE_SIZE_T], 1)
     ;;
esac

AC_SUBST(CPPFLAGS)
AC_SUBST(CFLAGS)
AC_SUBST(CXXFLAGS)
AC_SUBST(LDFLAGS)
AC_SUBST(LIBS)
AC_SUBST(CXX)
AC_SUBST(MAKEDEPEND)
AC_CONFIG_FILES([Makefile src/Makefile simulationmodels/Makefile])
AC_OUTPUT

echo "You have now configured the warped system for building with dlg, antlr,\
and a C++ compiler."

dnl rm -f ./config.cache
