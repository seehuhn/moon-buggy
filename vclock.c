/* vclock.c - high-precision wall clock time
 *
 * Copyright 1998  Jochen Voss.  */

static const  char  rcsid[] = "$Id: vclock.c,v 1.6 1999/07/21 10:38:11 voss Rel $";


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#define _XOPEN_SOURCE_EXTENDED 1

#include <sys/time.h>

#if defined(__hp9000s800)
#include <stdarg.h>
#endif

#include "moon-buggy.h"


double
vclock (void)
/* Return the elapsed (wall clock) time (measured in seconds) since
 * some base time with greater precision than `clock()' does.  */
{
  struct timeval  x;

  gettimeofday (&x, NULL);
  return  (x.tv_sec + x.tv_usec*1.0e-6);
}
