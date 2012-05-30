dnl Process this file with autoconf to produce a configure script.

AC_INIT(moon-buggy, 1.0.51, voss@seehuhn.de)
AC_CONFIG_SRCDIR([moon-buggy.h])
AM_INIT_AUTOMAKE
AM_CONFIG_HEADER(config.h)

AC_CANONICAL_HOST

dnl Some systems want _XOPEN_SOURCE to be defined as we use curses.
case "$host" in
  *bsd*|sparc-sun-solaris2.4|*-*-darwin*) ;;
  *) AC_DEFINE(_XOPEN_SOURCE,1,[Define this if your system supports it.]) ;;
esac

AC_ARG_WITH(setgid,
[  --with-setgid=NAME      setgid usage: install moon-buggy as group NAME],
[case "${withval}" in
  no)  GAMEGROUP="" ;;
  yes) AC_MSG_ERROR([missing argument for --with-setgid]) ;;
  *)   GAMEGROUP="$withval" ;;
esac])
AC_SUBST(GAMEGROUP)

dnl Checks for programs.
AC_PROG_CC

dnl Checks for libraries.
AC_CHECK_LIB(m, modf)
JV_CHECK_CURSES

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(getopt.h errno.h locale.h termios.h)

# Check if <sys/select.h> needs to be included for fd_set
AC_MSG_CHECKING([for fd_set])
AC_TRY_COMPILE([#include <sys/types.h>],
        [fd_set readMask, writeMask;], mb_ok=yes, mb_ok=no)
if test $mb_ok = yes; then
    AC_MSG_RESULT([yes, found in sys/types.h])
else
    AC_EGREP_HEADER(fd_mask, sys/select.h, mb_ok=yes)
    if test $mb_ok = yes; then
        AC_DEFINE(HAVE_SYS_SELECT_H,1,
		  [Define if you have the <errno.h> header file.])
        AC_MSG_RESULT([yes, found in sys/select.h])
    else
        AC_DEFINE(NO_FD_SET,1,[Define if your system lacks `fd_set'.])
        AC_MSG_RESULT(no)
    fi
fi

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_UID_T
AC_TYPE_SIZE_T

dnl Checks for library functions.
AC_TYPE_SIGNAL
AC_CHECK_FUNCS(fclean ftruncate getopt_long setreuid setlocale)

AM_CONDITIONAL(short_getopt, test "x$ac_cv_func_getopt_long" != xyes)

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
