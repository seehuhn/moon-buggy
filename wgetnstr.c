/* wgetnstr.c - replacement for the curses function
 *
 * Copyright (C) 1998  Jochen Voss.  */

static const  char  rcsid[] = "$Id: wgetnstr.c,v 1.1 1999/01/01 13:55:44 voss Exp $";

#define _POSIX_SOURCE

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <ctype.h>
#if HAVE_ERRNO_H
#include <errno.h>
#else
extern  int  errno;
#endif

#include "moon.h"


int
wgetnstr (WINDOW *win, char *str, int n)
{
  char  erase = erasechar ();
  int  done = 0;
  int  pos = 0;

  echo ();
  keypad (win, FALSE);
  wrefresh (win);
  do {
    char  c;

    do {
      c = wgetch (win);
    } while (c == ERR && errno == EINTR);
    switch (c) {
    case ERR:
      fatal ("wgetnstr: wgetch failed");
    case '\n':
    case '\r':
      done = 1;
      break;
    default:
      if (c == erase && pos > 0) {
	  --pos;
      } else if (pos < n && isprint(c)) {
	str[pos++] = c;
      } else {
	beep ();
	doupdate ();
      }
    }
  } while (! done);
  noecho ();
  keypad (win, TRUE);

  if (pos < n)  str[pos] = '\0';

  return  OK;
}
