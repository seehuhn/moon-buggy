dnl acinclude.m4 - aclocal include file for "aclocal.m4"
dnl Copyright 2000  Jochen Voss

dnl The idea of this tests, is to be as simple, as possible.
dnl The only special case we check for, is old versions of
dnl ncurses.  Any other problems must be handled by the user
dnl via configure's --with-curses-includedir,
dnl --with-curses-header, and --with-curses-libs options.

AC_DEFUN([JV_CHECK_CURSES],[
  dnl step 1: find the correct preprocessor flags
  dnl         This is completely left to the user.
  AC_MSG_CHECKING([for curses preprocessor flags])
  AC_ARG_WITH(curses-includedir,
    [  --with-curses-includedir[=DIR]
                          special header file location for the curses library
			  [none]],[
    case "$withval" in
      yes|no) ;;
      *) CURSES_INCLUDEDIR="-I$withval" ;;
    esac
  ])
  AC_SUBST(CURSES_INCLUDEDIR)
  if test -z "$CURSES_INCLUDEDIR"; then
    mb_result="none"
  else
    mb_result="$CURSES_INCLUDEDIR"
  fi
  AC_MSG_RESULT($mb_result)


  dnl step 2: find the correct header file name
  dnl         When nothing is specified on the command line, we try to guess.
  AC_ARG_WITH(curses-header,
    [  --with-curses-header[=ARG]
                          the curses header file name (including brackets)
			  [<curses.h>]],[
    case "$withval" in
      yes|no) ;;
      *) CURSES_HEADER="$withval" ;;
    esac
  ])
  if test -z "$CURSES_HEADER"; then
    mb_save_CPPFLAGS="$CPPFLAGS"
    CPPFLAGS="$CPPFLAGS $CURSES_INCLUDEDIR"
    AC_CHECK_HEADERS(curses.h ncurses.h ncurses/ncurses.h ncurses/curses.h, break)
    CPPFLAGS="$mb_save_CPPFLAGS"

    if test "$ac_cv_header_curses_h" = yes; then
      CURSES_HEADER="<curses.h>"
    elif test "$ac_cv_header_ncurses_h" = yes; then
      CURSES_HEADER="<ncurses.h>"
    elif test "$ac_cv_header_ncurses_ncurses_h" = yes; then
      CURSES_HEADER="<ncurses/ncurses.h>"
    elif test "$ac_cv_header_ncurses_curses_h" = yes; then
      CURSES_HEADER="<ncurses/curses.h>"
    fi
  fi
  AC_MSG_CHECKING([how to include the curses header file])
  if test -n "$CURSES_HEADER"; then
    AC_DEFINE_UNQUOTED(CURSES_HEADER, $CURSES_HEADER, [Define to the curses header file name (including brackets). ])
    AC_MSG_RESULT($CURSES_HEADER)
  else
    AC_MSG_RESULT([unknown])
    AC_MSG_WARN([no curses header found, try --with-curses-header])
  fi


  dnl step 3: try to find the correct linker flags
  dnl         When nothing is specified on the command line, we try to guess.
  AC_ARG_WITH(curses-libs,
    [  --with-curses-libs[=ARG]
                          the -l and -L linker flags for the curses library
			  [-lcurses]],[
    case "$withval" in
      yes|no) ;;
      *) CURSES_LIBS="$withval" ;;
    esac
  ])
  if test -z "$CURSES_LIBS"; then
    AC_CHECK_LIB(curses,initscr,CURSES_LIBS=-lcurses)
    if test -z "$CURSES_LIBS"; then
      AC_CHECK_LIB(ncurses,initscr,CURSES_LIBS=-lncurses)
    fi
  fi
  AC_MSG_CHECKING([for curses linker flags])
  if test -n "$CURSES_LIBS"; then
    AC_SUBST(CURSES_LIBS)
    AC_MSG_RESULT($CURSES_LIBS)
  else
    AC_MSG_RESULT([unknown])
    AC_MSG_WARN([curses library not found, try --with-curses-libs])
  fi
])
