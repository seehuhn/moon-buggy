/* mvwaddnstr.c - replacement for the curses function mvwaddnstr
 *
 * Copyright 1998  Jochen Voss.  */

static const  char  rcsid[] = "$Id: mvwaddnstr.c,v 1.4 1999/07/21 10:40:26 voss Rel $";


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>

#include "moon-buggy.h"

int
mvwaddnstr (WINDOW *win, int y, int x, const char *str, int n)
{
  char  buffer [n+1];
  strncpy (buffer, str, n);
  buffer[n] = '\0';
  return  mvwaddstr (win, y, x, buffer);
}
