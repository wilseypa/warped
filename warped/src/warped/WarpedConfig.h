/* src/warped/WarpedConfig.h.  Generated from WarpedConfig.h.in by configure.  */
/* src/warped/WarpedConfig.h.in.  Generated from configure.ac by autoheader.  */

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the `nsl' library (-lnsl). */
#define HAVE_LIBNSL 1

/* Define to 1 if you have the `socket' library (-lsocket). */
/* #undef HAVE_LIBSOCKET */

/* Define to 1 if you have the `utils' library (-lutils). */
#define HAVE_LIBUTILS 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to the sub-directory in which libtool stores uninstalled libraries.
   */
#define LT_OBJDIR ".libs/"

/* Name of package */
#define PACKAGE "warped"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT ""

/* Define to the full name of this package. */
#define PACKAGE_NAME "warped"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "warped 3.0"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "warped"

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION "3.0"

/* The size of `long ', as computed by sizeof. */
#define SIZEOF_LONG_ 8

/* The size of `long long ', as computed by sizeof. */
#define SIZEOF_LONG_LONG_ 8

/* When spawning remote processes via ssh, use the command specified in
   SSH_COMMAND */
#define SSH_COMMAND $SSH_PATH 

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Solaris 7: int setsockopt(int,int,int,const void *, int ); Solaris 2.6: int
   setsockopt(int,int,int,const void *, size_t ); (3XN) Solaris 2.6: int
   setsockopt(int,int,int,const char *, int ); (3N) Linux: int
   setsockopt(int,int,int,const void *, socklen_t ); */
/* #undef USE_CHAR_FOR_VOID */

/* Compile cppunit tests */
/* #undef USE_CPPUNIT */

/* See comments for USE_CHAR_FOR_VOID */
#define USE_SIZE_T 1

/* LINUX platforms require "socklen_t" while SOLARIS platforms require it to
   be an "int". This macro tells use whether or not to use socklen_t. */
#define USE_SOCKLEN_T 1

/* Compile in TIMEWARP support */
#define USE_TIMEWARP 1

/* Version number of package */
#define VERSION "3.0"
