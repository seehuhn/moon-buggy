/* mvwaddnstr.c - replacement for the curses function mvwaddnstr
 *
 * Copyright (C) 1998  Jochen Voss.  */

static const  char  rcsid[] = "$Id: mvwaddnstr.c,v 1.1 1998/12/30 23:28:07 voss Rel $";


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>

#include "moon.h"

int
mvwaddnstr (WINDOW *win, int y, int x, const char *str, int n)
{
  char  buffer [n+1];
  strncpy (buffer, str, n);
  buffer[n] = '\0';
  return  mvwaddstr (win, y, x, buffer);
}
