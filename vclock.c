/* vclock.c - high-precision wall clock time
 *
 * Copyright (C) 1998  Jochen Voss.  */

static const  char  rcsid[] = "$Id: vclock.c,v 1.4 1999/06/13 18:36:59 voss Exp $";


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#define _XOPEN_SOURCE_EXTENDED 1

#include <sys/time.h>

#if defined(__hp9000s800)
#include <stdarg.h>
#endif

#include "mbuggy.h"


double
vclock (void)
/* Return the elapsed (wall clock) time (measured in seconds) since
 * some base time with greater precision than `clock()' does.  */
{
  struct timeval  x;

  gettimeofday (&x, NULL);
  return  (x.tv_sec + x.tv_usec*1.0e-6);
}
