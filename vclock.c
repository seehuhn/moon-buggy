/* vclock.c - high-precision wall clock time
 *
 * Copyright (C) 1998  Jochen Voss.  */

static const  char  rcsid[] = "$Id: vclock.c,v 1.2 1999/05/22 13:43:59 voss Rel $";


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sys/time.h>

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
