/* buggy.c - implement the moon buggy
 *
 * Copyright 1999  Jochen Voss  */

static const  char  rcsid[] = "$Id: buggy.c,v 1.8 1999/04/21 19:19:38 voss Exp $";

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <assert.h>

#include "moon.h"


int  car_base, score_base;


enum car_state { car_NORMAL, car_START, car_UP1, car_UP2, car_LAND };
typedef  struct scene {
  double  t;
  enum car_state  state;
}  scenario [];

static  scenario  sz_empty = {
  { -1, -1 }
};

static  scenario  sz_jump = {
  { TICK(0), car_START },
  { TICK(1.25), car_UP1 },
  { TICK(3.75), car_UP2 },
  { TICK(8.75), car_UP1 },
  { TICK(11.25), car_LAND },
  { TICK(13.75), car_NORMAL },
  { -1, -1 }
};

static  struct scene *state;
static  enum car_state  cstate;

#define  speed  1

void
initialise_buggy (void)
{
  state = sz_empty;
  cstate = car_NORMAL;
}

void
animate_buggy (void)
{
  if (state->t >= -0.5) {
    cstate = state->state;
    ++state;
  }
}

int
print_buggy (void)
{
  int  crash = 0;
  
  switch (cstate) {
  case car_NORMAL:
    mvwaddstr (moon, LINES-8, car_base-speed, "         ");
    mvwaddstr (moon, LINES-7, car_base-speed, "         ");
    mvwaddstr (moon, LINES-6, car_base-speed, "    Omm  ");
    mvwaddstr (moon, LINES-5, car_base-speed, " (0)-(0) ");
    break;
  case car_START:
    mvwaddstr (moon, LINES-8, car_base-speed, "         ");
    mvwaddstr (moon, LINES-7, car_base-speed, "         ");
    mvwaddstr (moon, LINES-6, car_base-speed, "    OMM  ");
    mvwaddstr (moon, LINES-5, car_base-speed, " (0)-(0) ");
    break;
  case car_UP1:
    mvwaddstr (moon, LINES-8, car_base-speed, "         ");
    mvwaddstr (moon, LINES-7, car_base-speed, "    OMm  ");
    mvwaddstr (moon, LINES-6, car_base-speed, " (0)-(0) ");
    mvwaddstr (moon, LINES-5, car_base-speed, "         ");
    break;
  case car_UP2:
    mvwaddstr (moon, LINES-8, car_base-speed, "    oMm  ");
    mvwaddstr (moon, LINES-7, car_base-speed, " (0)-(0) ");
    mvwaddstr (moon, LINES-6, car_base-speed, "         ");
    mvwaddstr (moon, LINES-5, car_base-speed, "         ");
    break;
  case car_LAND:
    crash = crash_check ();
    if (! crash) {
      mvwaddstr (moon, LINES-8, car_base-speed, "         ");
      mvwaddstr (moon, LINES-7, car_base-speed, "         ");
      mvwaddstr (moon, LINES-6, car_base-speed, "    omm  ");
      mvwaddstr (moon, LINES-5, car_base-speed, " (o)_(o) ");
    }
    break;
  }

  wnoutrefresh (moon);
  return  crash;
}

void
jump (double t)
{
  struct scene *sptr = sz_jump;

  if (cstate != car_NORMAL) {
    assert (cstate == car_LAND);
    remove_event (ev_BUGGY, &t);
  }
  cstate = car_START;
  state = sptr;
  while (sptr->t >= -0.5) {
    add_event (t+sptr->t, ev_BUGGY);
    ++sptr;
  }
}

int
can_jump (void)
{
  return  (cstate == car_NORMAL || cstate == car_LAND);
}

int
crash_check (void)
{
  int  base = car_base - speed;
  
  if (cstate == car_START || cstate == car_UP1 || cstate == car_UP2)  return 0;
  if (ground2[base+2] == ' ') {
    mvwaddstr (moon, LINES-8, base, "         ");
    mvwaddstr (moon, LINES-7, base, "  m      ");
    mvwaddstr (moon, LINES-6, base, "  Om     ");
    mvwaddstr (moon, LINES-5, base, " (;-(*)  ");
    mvwaddch (moon, LINES-4, base+2,  'o');
    wnoutrefresh (moon);
    return 1;
  }
  if (ground2[base+6] == ' ') {
    mvwaddstr (moon, LINES-8, base, "         ");
    mvwaddstr (moon, LINES-7, base, "         ");
    mvwaddstr (moon, LINES-6, base, "     0   ");
    mvwaddstr (moon, LINES-5, base, " (*;_(m. ");
    mvwaddch (moon, LINES-4, base+6,      'o');
    wnoutrefresh (moon);
    return 1;
  }
  if (bonus) {
    score_bonus (1<<bonus);
    bonus = 0;
  }

#if MB_DEBUG
  ground2[base+2] = 'O';
  ground2[base+6] = 'O';
#endif
  
  return  0;
}
