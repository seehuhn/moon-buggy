/* control.c - challenge the user in a controlled way
 *
 * Copyright (C) 1999  Jochen Voss.  */

static const  char  rcsid[] = "$Id: control.c,v 1.1 1999/05/22 17:11:15 voss Exp $";

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "mbuggy.h"


static  int  hole, gap;
static  int  ticks, count;


void
control_init (int new_game)
{
  if (new_game) {
    hole = 2;
    ticks = car_base;
    count = 0;
  }
}

int
control_tick (void)
{
  int  res;

  if (hole > 0) {
    --hole;
    if (! hole) {
      if (count >= 3) {
	/* TODO: neuer Level */
	ticks = 30;
	count = 0;
      }
      if (ticks < car_base) {
	gap = car_base-ticks;
      } else if (ticks < 342) {
	gap = 14.5 - (ticks-car_base)*4.0/(342-car_base) + uniform_rnd (4);
      } else {
	if (count) {
	  gap = 10;
	} else {
	  gap = 13;
	}
	++count;
      }
    }
    res = 1;
  } else {
    --gap;
    if (gap <= 0)  hole = 2;
    res = 0;
  }
  // if (uniform_rnd(20) == 0)  place_meteor (t);

  ++ticks;
  
  return  res;
}
