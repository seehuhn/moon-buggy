dnl acinclude.m4 - aclocal include file for "aclocal.m4"
dnl Copyright 2000  Jochen Voss
dnl $Id: acinclude.m4,v 1.1 2000/03/14 20:34:31 voss Exp $

AC_DEFUN(AC_CHECK_CURSES,[
  AC_MSG_CHECKING("for curses linker flags")
  CURSES_LIBS="-lcurses"
  AC_ARG_WITH(curses-libs,
    [  --with-curses-libs[=ARG]
                          the -l and -L linker flags for the curses library
			  [-lcurses]],
    if test "x$withval" != xyes; then
      CURSES_LIBS="$withval"
    fi
  )
  AC_SUBST(CURSES_LIBS)
  AC_MSG_RESULT($CURSES_LIBS)

  AC_MSG_CHECKING("for curses preprocessor flags")
  CURSES_INCLUDEDIR=""
  AC_ARG_WITH(curses-includedir,
    [  --with-curses-includedir[=DIR]
                          the header file location for the curses library
			  [empty]],
    if test "x$withval" != xyes; then
      CURSES_INCLUDEDIR="-I$withval"
    fi
  )
  AC_SUBST(CURSES_INCLUDEDIR)
  AC_MSG_RESULT($CURSES_INCLUDEDIR)

  AC_MSG_CHECKING("how to include the curses header file")
  CURSES_HEADER="<curses.h>"
  AC_ARG_WITH(curses-header,
    [  --with-curses-header[=ARG]
                          the curses header file name (including brackets)
			  [<curses.h>]],
    if test "x$withval" != xyes; then
      CURSES_HEADER="$withval"
    fi
  )
  AC_DEFINE_UNQUOTED(CURSES_HEADER, $CURSES_HEADER, [define to the curses header file name (including brackets). ])
  AC_MSG_RESULT($CURSES_HEADER)
])
