/* lag.c - keep time lag statistics
 *
 * Copyright (C) 1998  Jochen Voss.  */

static const  char  rcsid[] = "$Id: lag.c,v 1.1 1998/12/18 23:16:49 voss Exp $";


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "moon.h"

#define TIME_BUFFER_SIZE 10

struct lagmeter {
  double  data[TIME_BUFFER_SIZE];
  int  used, pos;
  double  sum;
};

struct lagmeter *
new_lagmeter (void)
{
  struct lagmeter *res;

  res = xmalloc (sizeof (struct lagmeter));
  res->used = 0;
  res->pos = 0;
  res->sum = 0;

  return  res;
}

void
add_lag (struct lagmeter *lm, double x)
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
get_lag (const struct lagmeter *lm)
{
  if (lm->used == 0)  return 0;
  return  lm->sum / lm->used;
}
