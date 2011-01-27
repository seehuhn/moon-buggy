/* random.c - generate pseudo random numbers
 *
 * Copyright 1999  Jochen Voss.  */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <time.h>
#include <assert.h>

#include "moon-buggy.h"


void
init_rnd (void)
/* Initialise the random number generator with a random seed.
 * The seed is based on the current time.  */
{
  srand (time (0));
}

int
uniform_rnd (unsigned limit)
/* Returns a pseudo random integer `x' with `0 <= x < limit'.
 * The numbers a uniformly distributed.  */
{
  assert (limit > 1);
  return  (int)((double)limit*rand()/(RAND_MAX+1.0));
}
