dnl Check for cppunit

dnl Usage: CHECK_LIB_CPPUNIT

AC_DEFUN([CHECK_LIB_CPPUNIT],
[
AC_CACHE_CHECK([for location of libcppunit], [cl_cv_lib_cppunit],
  [AC_ARG_WITH(cppunit,
    AS_HELP_STRING([--with-cppunit],
      [location of libcppunit (default is /usr)]),
      [
        case "${withval}" in
          yes)  cl_cv_lib_cppunit=/usr; ;;
          no)   cl_cv_lib_cppunit=no ;;
          "")   cl_cv_lib_cppunit=no ;;
          *)    cl_cv_lib_cppunit=$withval ;;
        esac
      ],
      [cl_cv_lib_cppunit=/usr]
  )]
)

if test $cl_cv_lib_cppunit = no ; then
   Disable the build of the unit tests
  AC_MSG_ERROR("cpp_unit_disabled via --with-cppunit")
  AC_SUBST(CPP_UNIT_TEST_DIR, "")
  return;
fi

if test $cl_cv_lib_cppunit != /usr ; then
LDFLAGS="$LDFLAGS -L$cl_cv_lib_cppunit/lib"
CPPFLAGS="$CPPFLAGS -I$cl_cv_lib_cppunit/include"
fi

dnl We might want to add addition checks for CPPUNIT headers later
AC_CHECK_HEADER(cppunit/Test.h, ,
  AC_MSG_WARN(Couldn't find cppunit/TestCase.h which is optional but
    required to run the unit tests in the 'test' directory. This file is
    part of the cppunit distribution.  You probably need to install it
    and/or specify the location of it with the --with-cppunit option.))

dnl Check for the CPPUNIT library
AC_CHECK_LIB(cppunit, main, 
  [AC_SUBST(CPP_UNIT_TEST_DIR,[test]) \
        LIBS="$LIBS -lcppunit"], 
  [AC_SUBST(CPP_UNIT_TEST_DIR, "") \
  AC_MSG_WARN(Couldn't find cppunit library which is optional but
    required to run the unit tests in the 'test' directory. This file is
    part of the cppunit distribution.  You probably need to install it
    and/or specify the location of it with the --with-cppunit option.)
        ])
]) dnl end LIB_WARPED
