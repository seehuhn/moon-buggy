/* moon.c - implement the moon
 *
 * Copyright 1999  Jochen Voss  */

static const  char  rcsid[] = "$Id: moon.c,v 1.9 1999/04/24 17:56:10 voss Exp $";


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


char *ground1, *ground2, *ground3;
static int  ground_width;

static  int  hole = 2;


void
resize_ground (int clear_it)
{
  int  cols, i;

  block_winch ();
  cols = COLS;
  if (ground_width != cols) {
    ground1 = xrealloc (ground1, cols);
    ground2 = xrealloc (ground2, cols);
    ground3 = xrealloc (ground3, cols);
  }
  for (i=(clear_it ? 0 : ground_width); i<cols; ++i) {
    ground1[i] = '#';
    ground2[i] = '#';
    ground3[i] = ' ';
  }
  ground_width = cols;
  car_base = (cols > 80 ? 80 : cols) - 12;
  score_base = car_base + 7;
  unblock ();
}

int
d_rnd (int limit)
/* Returns a random integer `x' with `0 <= x < limit'.  */
{
  return  (int)((double)limit*rand()/(RAND_MAX+1.0));
}

void
print_ground (void)
{
  mvwaddnstr (moon, LINES-4, 0, ground2, ground_width);
  mvwaddnstr (moon, LINES-3, 0, ground1, ground_width);
  wnoutrefresh (moon);
}

void
scroll_ground (void)
{
  char  nextchar;

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
  
  memmove (ground2+1, ground2, ground_width-1);
  ground2[0] = nextchar;

  memmove (ground3+1, ground3, ground_width-1);
  ground3[0] = d_rnd(30) ? ' ' : 'o';
}
