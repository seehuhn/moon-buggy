/* moon.c - implement the moon
 *
 * Copyright (C) 1998  Jochen Voss.  */

static const  char  rcsid[] = "$Id: moon.c,v 1.1 1998/12/18 23:16:15 voss Exp $";


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#if STDC_HEADERS
# include <string.h>
# include <stdlib.h>
#else
# ifndef HAVE_MEMMOVE
#  define memmove(d, s, n) bcopy ((s), (d), (n))
# endif
#endif
#include <assert.h>

#include "moon.h"


chtype *ground1, *ground2, *ground1;

static  int  hole = 2;


static int
d_rnd (int anz)
/* Returns a random integer `x' with `0 <= x < anz'.  */
{
  return  (int)((double)anz*rand()/(RAND_MAX+1.0));
}

void
scroll_ground (void)
{
  chtype  nextchar;

  assert (hole != 0);
  if (hole > 0) {
    nextchar = ' ';
    --hole;
    if (hole == 0)  hole = -11-d_rnd (7);
  } else {
    nextchar = '#';
    ++hole;
    if (hole == 0)  hole = 2+d_rnd (2);
  }
  
  memmove (ground2+1, ground2, (COLS-1)*sizeof(chtype));
  ground2[0] = nextchar;
}
