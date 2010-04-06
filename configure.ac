#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

dnl --------------------------------
dnl Initialization macros.
dnl --------------------------------

AC_INIT(stomp/stomp.h)
AM_CONFIG_HEADER(config.h)

dnl -----------------------------------------------
dnl Package name and version number (user defined)
dnl -----------------------------------------------

GENERIC_LIBRARY_NAME=stomp

#release versioning
GENERIC_MAJOR_VERSION=1
GENERIC_MINOR_VERSION=0
GENERIC_MICRO_VERSION=0

#API version (often = GENERIC_MAJOR_VERSION.GENERIC_MINOR_VERSION)
GENERIC_API_VERSION=1.0
AC_SUBST(GENERIC_API_VERSION)

#shared library versioning
GENERIC_LIBRARY_VERSION=1:0:0

dnl --------------------------------
dnl Package name and version number
dnl --------------------------------

AC_SUBST(GENERIC_LIBRARY_VERSION)

PACKAGE=$GENERIC_LIBRARY_NAME
AC_SUBST(GENERIC_LIBRARY_NAME)

GENERIC_VERSION=$GENERIC_MAJOR_VERSION.$GENERIC_MINOR_VERSION.$GENERIC_MICRO_VERSION
GENERIC_RELEASE=$GENERIC_MAJOR_VERSION.$GENERIC_MINOR_VERSION
AC_SUBST(GENERIC_RELEASE)
AC_SUBST(GENERIC_VERSION)

VERSION=$GENERIC_VERSION

AM_INIT_AUTOMAKE($PACKAGE, $VERSION, ryan.scranton@gmail.com)

# Checks for programs.
AC_PROG_CC
AC_PROG_CPP
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AM_SANITY_CHECK
AC_LANG_CPLUSPLUS

# Turn on library exporting.
AC_PROG_LIBTOOL
AC_SUBST(LIBTOOL_DEPS)

# Location of gflags directories as input by user.
gflags_dir_given="no"

AC_ARG_WITH(gflags_dir, AC_HELP_STRING([--with-gflags-dir=PREFIX],
                                       [prefix directory containing gflags]),
            [gflags_dir_given="yes"])

if test "$gflags_dir_given" = "yes"; then
  GFLAGS_INCLUDE="-I$with_gflags_dir/include"
  GFLAGS_LIB="-L$with_gflags_dir/lib"
  AC_SUBST(GFLAGS_INCLUDE)
  AC_SUBST(GFLAGS_LIB)
  CFLAGS="$CFLAGS -I$with_gflags_dir/include"
  CPPFLAGS="$CPPFLAGS -I$with_gflags_dir/include"
  CXXFLAGS="$CXXFLAGS -I$with_gflags_dir/include"
  LDFLAGS="$LDFLAGS -L$with_gflags_dir/lib"
fi

# Check for C++ stl.
AC_LANG_CPLUSPLUS
AC_CHECK_HEADER([string], [], [AC_MSG_ERROR([cannot find STL string])])
AC_CHECK_HEADER([vector], [], [AC_MSG_ERROR([cannot find STL vector])])
AC_CHECK_HEADER([map], [], [AC_MSG_ERROR([cannot find STL map])])
AC_CHECK_HEADER([queue], [], [AC_MSG_ERROR([cannot find STL queue])])
AC_CHECK_HEADER([algorithm], [], [AC_MSG_ERROR([cannot find STL algorithms])])

# Check for gflags
AC_LANG_CPLUSPLUS
AC_CHECK_HEADER(google/gflags.h, [], [AC_MSG_ERROR([cannot find gflags.h])])
AC_CHECK_LIB([gflags], [main], [],
             [AC_MSG_ERROR([cannot find libgflags])])

AC_OUTPUT(Makefile \
          stomp-1.0.pc \
          stomp/Makefile
)