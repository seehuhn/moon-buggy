/* level.c - challenge the user in a controlled way
 *
 * Copyright (C) 1999  Jochen Voss.  */

static const  char  rcsid[] = "$Id: level.c,v 1.2 1999/05/23 21:06:08 voss Exp $";

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "mbuggy.h"


static  int  hole, first;
static  int  level, last_level, ticks;

static union {
  struct {
    int  state, gap;
  } l0;
  struct {
    int  state, gap;
  } l1;
  struct {
    int  state, gap;
  } l2;
  struct {
    int  state, gap, pos;
  } l3;
  struct {
    int  state, gap, next_gap;
  } l4;
  /* l5 */
  struct {
    int  state, gap, next_gap;
  } l_fin;
} data;


static void
level0_init (void)
{
  print_message ("good luck (or use SPACE to jump)");
  hole = 2;
  data.l0.state = 0;
}

static void
level0 (double t)
{
  if (first) {
    if (ticks < 350) {
      data.l0.gap = 14.5 - ticks*4.0/350 + uniform_rnd (4);
    } else {
      if (data.l0.state) {
	data.l0.gap = 10;
      } else {
	data.l0.gap = 13;
      }
      if (data.l0.state > 2) {
	++level;
      } else {
	++data.l0.state;
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
  data.l1.state = 0;
}

static void
level1 (double t)
{
  if (first) {
    if (ticks < 300) {
      data.l1.gap = 13.5 - ticks*4.0/300 + uniform_rnd (4);
    } else {
      switch (data.l1.state) {
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
      if (data.l1.state > 2) {
	++level;
      } else {
	++data.l1.state;
      }
    }
  }

  --data.l1.gap;
  if (data.l1.gap <= 0) {
    switch (data.l1.state) {
    case 1:
    case 2:
      hole = 4;
      break;
    case 3:
      hole = 5;
      break;
    default:
      hole = 2 + uniform_rnd (2);
      break;
    }
  }
}


static void
level2_init (void)
{
  hole = 2;
  data.l2.state = 0;
}

static void
level2 (double t)
{
  if (first) {
    if (data.l2.state == 0) {
      data.l2.gap = 8;
      if (ticks >= 310) {
	data.l2.state = -1;
      } else {
	data.l2.state = 2 + uniform_rnd (3 + (ticks < 190));
      }
    } else if (data.l2.state > 0) {
      data.l2.gap = 12 + uniform_rnd (6);
      --data.l2.state;
    } else {
      switch (data.l2.state) {
      case -1:
	data.l2.gap = 16;
	break;
      case -2:
	data.l2.gap = 13;
	break;
      case -4:
	data.l2.gap = 14;
	break;
      default:
	data.l2.gap = 7;
	break;
      }
      --data.l2.state;
    }
  }
  --data.l2.gap;
  if (data.l2.gap <= 0)  hole = 2;
  if (data.l2.state < -6)  ++level;
}


static void
level3_init (void)
{
  data.l3.state = -1;
  print_message ("use key <a> to fire the laser");
}

static void
level3 (double t)
{
  if (data.l3.state < 0) {
    place_meteor (t);
    data.l3.state = 0;
  }
  
  if (first) {
    data.l3.gap = 20 + uniform_rnd (10);
    if (ticks > 160)  ++data.l3.state;
    switch (data.l3.state) {
    case 0:
      data.l3.pos = uniform_rnd (5)+1;
      break;
    case 1:
      data.l3.pos = data.l3.gap - uniform_rnd (7) - 1;
      break;
    case 2:
      data.l3.pos = data.l3.gap - uniform_rnd (2) - 1;
      break;
    default:
      ++level;
      break;
    }
  }

  --data.l3.gap;
  if (data.l3.gap == data.l3.pos)  place_meteor (t);
  if (data.l3.gap <= 0)  hole = 2;
}


static void
level4_init (void)
{
  if (uniform_rnd (2)) {
    hole = 6;
    data.l4.next_gap = 8 + uniform_rnd (10);
  } else {
    hole = 2;
    data.l4.next_gap = 7;
  }
  data.l4.state = 3;
}

static void
level4 (double t)
{
  if (first) {
    data.l4.gap = data.l4.next_gap;
    if (data.l4.state == 0 && ticks < 700) {
      data.l4.state = 3 + uniform_rnd (3 + 2*(ticks < 350));
    }
    --data.l4.state;
    if (data.l4.state < -5)  ++level;
  }
  --data.l4.gap;
  if (data.l4.gap <= 0) {
    switch (data.l4.state) {
    default:
      hole = 2 + uniform_rnd (3);
      data.l4.next_gap = 14 + uniform_rnd (6) - hole;
      break;
    case 0:
      if (uniform_rnd (2)) {
	hole = 5;
	data.l4.next_gap = 9 + uniform_rnd (10);
      } else {
	hole = 2;
	data.l4.next_gap = 8;
      }
      break;
    case -1:
      hole = 3;
      data.l4.next_gap = 14;
      break;
    case -2:
      hole = 6;
      data.l4.next_gap = 6;
      break;
    case -3:
      hole = 4;
      data.l4.next_gap = 16 + uniform_rnd (6);
      break;
    case -4:
      hole = 2;
      data.l4.next_gap = 7;
      break;
    case -5:
      hole = 5;
      data.l4.next_gap = 12;
      break;
    }
  }
}


static void
level5 (double t)
{
  if (uniform_rnd (ticks % 20 < 8 ? 4 : 8) == 0)  place_meteor (t);
  if (ticks >= 125)  ++level;
}


static void
level_fin_init (void)
{
  data.l_fin.state = uniform_rnd (10);
  hole = 2;
  data.l_fin.next_gap = 7;
}

static void
level_fin (double t)
{
  if (first) {
    data.l_fin.gap = data.l_fin.next_gap;
    if (data.l_fin.state == 0) {
      data.l_fin.state = 2 + uniform_rnd (5);
    } else {
      --data.l_fin.state;
    }
  }
  --data.l_fin.gap;
  if (uniform_rnd (20) == 0)  place_meteor (t);
  if (data.l_fin.gap <= 0) {
    switch (data.l_fin.state) {
    default:
      hole = 2 + uniform_rnd (3);
      data.l_fin.next_gap = 14 + uniform_rnd (6) - hole;
      break;
    case 0:
      if (uniform_rnd (2)) {
	hole = 5;
	data.l_fin.next_gap = 9 + uniform_rnd (10);
      } else {
	hole = 2;
	data.l_fin.next_gap = 8;
      }
      break;
    }
  }
}

#define LEVEL_COUNT 7

static struct {
  void (*init_fn) (void);
  void (*fn) (double t);
} levels [LEVEL_COUNT] = {
  { level0_init, level0 },
  { level1_init, level1 },
  { level2_init, level2 },
  { level3_init, level3 },
  { level4_init, level4 },
  { NULL, level5 },
  { level_fin_init, level_fin }
};

void
level_start ()
/* Start the game anew with level 0.  */
{
  level = 0;
  last_level = -1;
  ticks = 0;
}

int
level_tick (double t)
/* Advance the current level's state by one.
 * This must be called every time the ground moves.
 * The parameter T must be the current game time.  */
{
  int  res;

  if (level != last_level) {
    level = level % LEVEL_COUNT;
    hole = 0;
    if (levels[level].init_fn)  levels[level].init_fn ();
    if (last_level >= 0)  ticks = -40;
    last_level = level;
    first = 1;
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
