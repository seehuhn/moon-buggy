/* xgetch.c - read single characters and check for errors
 *
 * Copyright 1999  Jochen Voss.  */

static const  char  rcsid[] = "$Id: xgetch.c,v 1.5 1999/06/03 13:18:21 voss Exp $";

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
/* Like curses' `wgetch', but handle errors and interraupts.
 * The function returns `ERR', if an interrupt occured.  */
{
  int  c;

  handle_signals ();
  c = wgetch (win);
  if (c == ERR) {
    if (errno == EINTR) {
      handle_signals ();
      /* Take care: WIN may be invalid, now.  */
    } else {
      fatal ("Cannot read keyboard input");
    }
  }
  return  c;
}
