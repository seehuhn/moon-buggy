dnl Process this file with autoconf to produce a configure script.
AC_REVISION($Revision: 1.4 $)

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

# Check for library functions.
AC_CHECK_LIB(m, modf)
AC_CHECK_FUNCS(
  [getcwd gettimeofday fnmatch chdir rmdir unlink lstat system getenv],[],
  AC_MSG_ERROR([required function missing]))

# Look for ncurses library to link to
ncurses=auto
AC_ARG_WITH([ncurses],
            AC_HELP_STRING([--with-ncurses], [compile/link with ncurses library] ),
            [ncurses=ncurses])
AC_ARG_WITH([ncursesw],
            AC_HELP_STRING([--with-ncursesw], [compile/link with wide-char ncurses library @<:@default@:>@]),
            [ncurses=ncursesw])
if test "$ncurses" = "auto" -o "$ncurses" = "ncursesw"; then
  PKG_CHECK_MODULES([NCURSES], [ncursesw], [LIBS="$LIBS $NCURSES_LIBS"; ncurses=ncursesw],
    [AC_CHECK_LIB([ncursesw],
                 [initscr],
                 [LIBS="$LIBS -lncursesw"; ncurses=ncursesw],
                 [ncurses=ncurses])
    ])
fi
if test "$ncurses" = "ncurses"; then
  PKG_CHECK_MODULES([NCURSES], [ncurses], [LIBS="$LIBS $NCURSES_LIBS"],
    [AC_CHECK_LIB([ncurses],
                 [initscr],
                 [LIBS="$LIBS -lncurses"],
                 [AC_MSG_ERROR(ncurses library is required)])
    ])
fi

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
