/* vclock.c - high-precision wall clock time
 *
 * Copyright 1998  Jochen Voss.  */

static const  char  rcsid[] = "$Id: vclock.c,v 1.7 2000/03/17 23:01:51 voss Rel $";


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef _XOPEN_SOURCE
#define _XOPEN_SOURCE_EXTENDED 1
#endif

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
