/* xgetch.c - read single characters and check for errors
 *
 * Copyright 1999  Jochen Voss.  */

static const  char  rcsid[] = "$Id: xgetch.c,v 1.4 1999/05/25 15:24:25 voss Exp $";

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if HAVE_ERRNO_H
#include <errno.h>
#else
extern  int  errno;
#endif

#include "mbuggy.h"


int
xgetch (WINDOW *win)
/* Like curses' `wgetch', but never returns `ERR'.  */
{
  int  c;
  
  do {
    c = wgetch (win);
  } while (c == ERR && errno == EINTR);
  if (c == ERR)  fatal ("cannot read keyboard input");

  return  c;
}
