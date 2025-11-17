dnl Process this file with autoconf to produce a configure script.

AC_INIT([moon-buggy],[1.1.0],[voss@seehuhn.de])
AC_CONFIG_SRCDIR([moon-buggy.h])
AM_INIT_AUTOMAKE
AC_CONFIG_HEADERS([config.h])

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
m4_warn([obsolete],
[The preprocessor macro 'STDC_HEADERS' is obsolete.
  Except in unusual embedded environments, you can safely include all
  C89 headers unconditionally.])dnl
# Autoupdate added the next two lines to ensure that your configure
# script's behavior did not change.  They are probably safe to remove.
AC_CHECK_INCLUDES_DEFAULT
AC_PROG_EGREP

AC_CHECK_HEADERS(getopt.h errno.h locale.h termios.h)

# Check if <sys/select.h> needs to be included for fd_set
AC_MSG_CHECKING([for fd_set])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <sys/types.h>]],[[fd_set readMask, writeMask;]])],[mb_ok=yes],[mb_ok=no])
if test $mb_ok = yes; then
    AC_MSG_RESULT([yes, found in sys/types.h])
else
    AC_EGREP_HEADER(fd_mask, sys/select.h, mb_ok=yes)
    if test $mb_ok = yes; then
        AC_DEFINE(HAVE_SYS_SELECT_H,1,
		  [Define if you have the <sys/select.h> header file.])
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
m4_warn([obsolete],
[your code may safely assume C89 semantics that RETSIGTYPE is void.
Remove this warning and the 'AC_CACHE_CHECK' when you adjust the code.])dnl
AC_CACHE_CHECK([return type of signal handlers],[ac_cv_type_signal],[AC_COMPILE_IFELSE(
[AC_LANG_PROGRAM([#include <sys/types.h>
#include <signal.h>
],
		 [return *(signal (0, 0)) (0) == 1;])],
		   [ac_cv_type_signal=int],
		   [ac_cv_type_signal=void])])
AC_DEFINE_UNQUOTED([RETSIGTYPE],[$ac_cv_type_signal],[Define as the return type of signal handlers
		    ('int' or 'void').])

AC_CHECK_FUNCS(fclean ftruncate getopt_long setreuid setlocale)

AM_CONDITIONAL(short_getopt, test "x$ac_cv_func_getopt_long" != xyes)

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
