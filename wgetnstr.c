/* wgetnstr.c - replacement for the curses function
 *
 * Copyright 1999  Jochen Voss  */

static const  char  rcsid[] = "$Id: wgetnstr.c,v 1.6 1999/07/21 10:38:02 voss Rel $";

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <ctype.h>
#if HAVE_ERRNO_H
#include <errno.h>
#else
extern  int  errno;
#endif

#include "moon-buggy.h"


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
    int  c;

    do {
      c = wgetch (win);
    } while (c == ERR && errno == EINTR);
    switch (c) {
    case '\n':
    case '\r':
      done = 1;
      break;
    case ERR:
      fatal ("Cannot read keyboard input");
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
