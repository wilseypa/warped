AC_INIT([warped],[4.0.0])
AC_CONFIG_HEADERS([src/WarpedConfig.h])

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([src/Event.cpp])
AM_INIT_AUTOMAKE([foreign])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES])
AC_PREREQ(2.61)

AC_LANG([C++])

dnl Checks for programs.
AC_PROG_LIBTOOL
AC_PROG_CXX
AC_PROG_CXXCPP
AC_PROG_INSTALL

LT_INIT

m4_include([m4/ax_cxx_compile_stdcxx_11.m4])
AX_CXX_COMPILE_STDCXX_11([noext], [mandatory])

AC_CHECK_LIB(nsl, gethostent)
AC_CHECK_LIB(socket,socket)

m4_include([m4/check_lib_mpich.m4])
CHECK_LIB_MPICH

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
AC_CHECK_HEADERS(unistd.h)

dnl Look for cppunit
m4_include([m4/check_lib_cppunit.m4])
CHECK_LIB_CPPUNIT

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

AC_CONFIG_FILES([Makefile src/Makefile thirdparty/Makefile thirdparty/json/Makefile thirdparty/metis/Makefile thirdparty/metis/GKlib/Makefile thirdparty/metis/libmetis/Makefile test/Makefile test/catch_tests/Makefile test/cppunit_tests/Makefile])

AC_DEFINE([USETSX_RTM], 1, [Define to 1 to use Intel RTM TSX on supported Haswell processors])
AC_DEFINE([TSXRTM_RETRIES], 10, [Define to number of times to try to enter transactional region for RTM])
AC_DEFINE([L1DSZ], 64, [L1 cache line size for data alignment for TM])

AC_OUTPUT
