/* lag.c - circular buffer to keep time lag statistics
 *
 * Copyright (C) 1998  Jochen Voss.  */

static const  char  rcsid[] = "$Id: lag.c,v 1.3 1999/03/08 20:24:57 voss Exp $";


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "moon.h"


#define TIME_BUFFER_SIZE 10

struct circle_buffer {
  double  data[TIME_BUFFER_SIZE];
  int  used, pos;
  double  sum;
};


struct circle_buffer *
new_circle_buffer (void)
{
  struct circle_buffer *res;

  res = xmalloc (sizeof (struct circle_buffer));
  res->used = 0;
  res->pos = 0;
  res->sum = 0;

  return  res;
}

void
add_value (struct circle_buffer *lm, double x)
{
  if (x>0.5) {
    x = 0.5;
  } else if (x < -0.5) {
    x = -0.5;
  }
  
  if (lm->used < TIME_BUFFER_SIZE) {
    lm->data[lm->used] = x;
    ++lm->used;
  } else {
    lm->sum -= lm->data [lm->pos];
    lm->data [lm->pos] = x;
    lm->pos = (lm->pos + 1) % TIME_BUFFER_SIZE;
  }
  lm->sum += x;
}

double
get_mean (const struct circle_buffer *lm)
{
  if (lm->used == 0)  return 0;
  return  lm->sum / lm->used;
}
