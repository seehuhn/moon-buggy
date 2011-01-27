/* level.c - challenge the user in a controlled way
 *
 * Copyright 1999  Jochen Voss.  */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <assert.h>

#include "moon-buggy.h"


/* The length of the inter-level gap (measured in ticks)  */
#define  PAUSE  40


static  int  hole, plateau;
static  int  is_edge, crater_seen;
static  int  level, initial_level, last_level, ticks;

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
    int  state, gap, next_gap, spare_time;
  } l6;
  struct {
    int  state, gap, next_gap, spare_time;
  } l_fin;
} data;


static void
level0_init (void)
{
  hole = 2;
  data.l0.state = 0;
}

static void
level0 (double t)
{
  if (is_edge) {
    if (ticks < 345) {
      data.l0.gap = 14.5 - ticks*4.0/345 + uniform_rnd (4.5 + ticks*4.0/345);
    } else {
      switch (data.l0.state) {
      case 0:
        data.l0.gap = 13;
        break;
      case 1:
        data.l0.gap = 10;
        break;
      case 2:
        data.l0.gap = 11;
        break;
      default:
        ++level;
        break;
      }
      ++data.l0.state;
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
  static const  int  table [11] = { 2, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4 };
  if (is_edge) {
    if (ticks < 300) {
      data.l1.gap = 14.5 - ticks*4.0/300 + uniform_rnd (3);
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
      hole = table [uniform_rnd (11)];
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
  if (is_edge) {
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
}

static void
level3 (double t)
{
  if (data.l3.state < 0) {
    place_meteor ();
    data.l3.state = 0;
  }

  if (is_edge) {
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
      bonus[0] += 20;
      ++level;
      break;
    }
  }

  --data.l3.gap;
  if (data.l3.gap == data.l3.pos)  place_meteor ();
  if (data.l3.gap <= 0)  hole = 2;
}


static void
level4_init (void)
{
  hole = 6;
  data.l4.next_gap = 8 + uniform_rnd (10);
  data.l4.state = 3;
}

static void
level4 (double t)
{
  if (is_edge) {
    data.l4.gap = data.l4.next_gap;
    if (data.l4.state == 0 && ticks < 700) {
      data.l4.state = 3 + uniform_rnd (3 + 2*(ticks < 350));
    } else {
      --data.l4.state;
    }
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
  if (uniform_rnd (ticks % 20 < 8 ? 4 : 8) == 0)  place_meteor ();
  if (ticks >= 125)  ++level;
}


static void
level6_init (void)
{
  hole = 5;
  data.l6.next_gap = 8 + uniform_rnd (10);
  data.l6.state = 3;
  data.l6.spare_time = 6;
}

static void
level6 (double t)
{
  int  slip;			/* this many ticks/meteor may the user vaste */

  if (is_edge) {
    data.l6.gap = data.l6.next_gap;
    if (data.l6.state == 0) {
      data.l6.state = 3 + uniform_rnd (5);
      if (ticks >= 375)  ++level;
    } else {
      --data.l6.state;
    }
  }
  --data.l6.gap;
  ++data.l6.spare_time;
  if (data.l6.gap <= 0) {
    if (data.l6.state) {
      hole = 2 + uniform_rnd (3);
      data.l6.next_gap = 15 + uniform_rnd (6) - hole;
    } else {
      if (uniform_rnd (3)) {
        hole = 5;
        data.l6.next_gap = 9 + uniform_rnd (10);
      } else {
        hole = 2 + uniform_rnd (2);
        data.l6.next_gap = 9;
      }
    }
    data.l6.spare_time -= 11;
  }

  slip = (380-ticks)*3.0/374 + 1.5;
  if (data.l6.spare_time >= 3+slip) {
    if (uniform_rnd (data.l6.spare_time) > 2) {
      place_meteor ();
      data.l6.spare_time -= 3+slip;
    }
  }
}


static void
level_fin_init (void)
{
  data.l_fin.state = uniform_rnd (10);
  hole = 2;
  data.l_fin.next_gap = 7;
  data.l_fin.spare_time = 0;
}

static void
level_fin (double t)
{
  if (is_edge) {
    data.l_fin.gap = data.l_fin.next_gap;
    if (data.l_fin.state == 0) {
      data.l_fin.state = 3 + uniform_rnd (6);
    } else {
      --data.l_fin.state;
    }
  }
  --data.l_fin.gap;
  ++data.l_fin.spare_time;
  if (data.l_fin.gap <= 0) {
    if (data.l_fin.state) {
      hole = 2 + uniform_rnd (3);
      data.l_fin.next_gap = 15 + uniform_rnd (6) - hole;
    } else {
      if (uniform_rnd (3)) {
        hole = 5 + uniform_rnd (2);
        data.l_fin.next_gap = 9 + uniform_rnd (10);
      } else {
        hole = 2 + uniform_rnd (3);
        data.l_fin.next_gap = 9;
      }
    }
    data.l_fin.spare_time -= 11;
  }

  if (data.l_fin.spare_time >= 5) {
    if (uniform_rnd (data.l_fin.spare_time) > 2) {
      place_meteor ();
      data.l_fin.spare_time -= 5;
    }
  }
}

#define LEVEL_COUNT 8

static struct {
  void (*init_fn) (void);
  void (*fn) (double t);
  const char *msg;
} levels [LEVEL_COUNT] = {
  { level0_init, level0, "good luck (or use <SPC> to jump)" },
  { level1_init, level1, "some craters are wider than others" },
  { level2_init, level2 },
  { level3_init, level3, "use <a> to fire the laser" },
  { level4_init, level4 },
  { NULL, level5, "... but what is that?" },
  { level6_init, level6 },
  { level_fin_init, level_fin, "You may start your journey, now." }
};

void
level_start (int initial)
/* Start the game anew with level INITIAL.  */
{
  assert (initial >= 0 && initial < LEVEL_COUNT);
  level = initial_level = initial;
  last_level = -1;
  ticks = 0;
}

static void
score_crater (int width)
{
  static const  int  score_table [] = { 0, 0, 4, 8, 9, 16, 32 };
  assert (width < 7);
  bonus[0] += score_table [width];
}

static void
score_plateau (int width)
{
  static const  int  score_table [] = { 0, 0, 0, 0, 0, 0, 32, 18, 9 };
  if (width > 8)  return;
  bonus[0] += score_table [width];
}

void
level_tick (double t)
/* Advance the current level's state by one.
 * The function must be called every time the ground moved.  It fills
 * in the new values of `ground2[0]' and `bonus[0]'.  The parameter T
 * must be the current game time.  */
{
  int  ground;

  bonus[0] = 0;
  if (level != last_level) {
    double  msg_t;
    level = level % LEVEL_COUNT;
    hole = 0;
    if (levels[level].init_fn)  levels[level].init_fn ();
    if (last_level >= 0) {
      ticks = -PAUSE;
      msg_t = t + TICK(car_x + PAUSE/5);
    } else {
      msg_t = t;
    }
    if (levels[level].msg) {
      add_event (msg_t, print_hint_h, (void *)levels[level].msg);
    }
    last_level = level;
    is_edge = 1;
    crater_seen = 0;
  }
  if (ticks < 0) {
    ground = '#';
  } else if (hole > 0) {
    if (! crater_seen) {
      score_crater (hole);
      is_edge = 1;
      crater_seen = 1;
      plateau = 0;
    }

    ground = ' ';
    --hole;
  } else {
    levels[level].fn (t);
    is_edge = 0;
    crater_seen = 0;

    ground = '#';
    ++plateau;
    if (hole > 0)  score_plateau (plateau);
  }

  ground2[0] = ground;
  print_buggy(); /* ++pg now the refresh of the car is needed here. */

  ++ticks;
}

int
current_level (void)
/* Return the current level's number as the car sees it.  */
{
  int  res = (ticks+PAUSE/2 >= car_x) ? level : level - 1;
  if (res < initial_level)  res = initial_level;
  return  res;
}
