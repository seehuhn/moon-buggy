/* control.c - challenge the user in a controlled way
 *
 * Copyright (C) 1999  Jochen Voss.  */

static const  char  rcsid[] = "$Id: control.c,v 1.2 1999/05/22 20:45:42 voss Exp $";

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "mbuggy.h"


static  int  hole, first;
static  int  level, last_level, ticks;

static union {
  struct {
    int  gap;
    int  count;
  } l0;
  struct {
    int  gap;
    int  count;
  } l1;
  struct {
    int  gap;
  } l2;
} data;


static void
skip_level (double t)
{
  ++level;
}


static void
level0_init (void)
{
  hole = 2;
  data.l0.count = 0;
}

static void
level0 (double t)
{
  if (first) {
    if (ticks < 350) {
      data.l0.gap = 14.5 - ticks*4.0/350 + uniform_rnd (4);
    } else {
      if (data.l0.count) {
	data.l0.gap = 10;
      } else {
	data.l0.gap = 13;
      }
      if (data.l0.count > 2) {
	++level;
      } else {
	++data.l0.count;
      }
    }
  }

  --data.l0.gap;
  if (data.l0.gap <= 0)  hole = 2;
}


static void
level1_init (void)
{
  hole = 5;
  data.l1.count = 0;
}

static void
level1 (double t)
{
  if (first) {
    if (ticks < 300) {
      data.l1.gap = 14.5 - ticks*4.0/300 + uniform_rnd (4);
    } else {
      switch (data.l1.count) {
      default:
	data.l1.gap = 12;
	break;
      case 1:
	data.l1.gap = 7;
	break;
      case 2:
	data.l1.gap = 8;
	break;
      }
      if (data.l1.count > 2) {
	++level;
      } else {
	++data.l1.count;
      }
    }
  }

  --data.l1.gap;
  if (data.l1.gap <= 0) {
    hole = data.l1.count > 0 ? 4 : 2 + uniform_rnd (2);
  }
}


static void
level2_init (void)
{
  hole = 2;
}

static void
level2 (double t)
{
  if (first) {
    data.l2.gap = 11 + uniform_rnd (7);
  }
  --data.l2.gap;
  if (uniform_rnd (20) == 0)  place_meteor (t);
  if (data.l2.gap <= 0)  hole = 2 + uniform_rnd (2);
}

#define LEVEL_COUNT 3

static struct {
  void (*init_fn) (void);
  void (*fn) (double t);
} levels [LEVEL_COUNT] = {
  { level0_init, level0 },
  { level1_init, level1 },
  { level2_init, level2 }
};

void
control_init (int new_game)
{
  if (new_game) {
    level = 0;
    last_level = -1;
    ticks = 0;
  }
}

int
control_tick (double t)
{
  int  res;

  if (level != last_level) {
    level = level % LEVEL_COUNT;
    if (levels[level].init_fn)  levels[level].init_fn ();
    if (last_level >= 0)  ticks = -40;
    last_level = level;
    first = 0;
  }
  if (ticks < 0) {
    res = 0;
  } else if (hole > 0) {
    --hole;
    res = 1;
    first = 1;
  } else {
    levels[level].fn (t);
    res = 0;
    first = 0;
  }

  ++ticks;
  
  return  res;
}
