#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([STM32F3DISCOVERY], [3.0], [])

# Checks for programs.
AM_INIT_AUTOMAKE([foreign -Wall])
AC_PROG_CC
AM_PROG_CC_C_O
AM_PROG_AR
AM_PROG_AS
LT_INIT
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.

# Checks for libraries.

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

CFLAGS=
CXXFLAGS=
LDFLAGS=

# Checks for library functions.
AC_MSG_CHECKING([release status])
AC_ARG_ENABLE(
  [release],
  [AS_HELP_STRING([--disable-release],       [Disable release [default=no]])],
  [
    case "${enableval}" in
    yes) release='true';;
    no)  release='false';;
    *)   AC_MSG_ERROR([bad value ${enableval} for --disable-release]);;
    esac
  ],
  [
    release='true'
  ]
  )

if test "${release}" = 'true' ; then
  CFLAGS+=" -DRELEASE "
else
  CFLAGS+=" -D_DEBUG "
fi
AC_MSG_RESULT([${release}])



AC_CONFIG_FILES([Makefile])

AC_OUTPUT
