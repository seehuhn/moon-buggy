/* buggy.c - implement the moon buggy
 *
 * Copyright (C) 1998  Jochen Voss.  */

static const  char  rcsid[] = "$Id: buggy.c,v 1.1 1998/12/18 23:14:29 voss Exp $";

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <assert.h>

#include "moon.h"


int  car_base;


enum car_state { car_NORMAL, car_START, car_UP1, car_UP2, car_LAND };
typedef  struct scene {
  double  t;
  enum car_state  state;
}  scenario [];

static  scenario  sz_empty = {
  { -1.0, -1 }
};

static  scenario  sz_jump = {
  { 0.0, car_START },
  { 0.1, car_UP1 },
  { 0.3, car_UP2 },
  { 0.7, car_UP1 },
  { 0.9, car_LAND },
  { 1.1, car_NORMAL },
  { -1.0, -1 }
};

static  struct scene *state = sz_empty;
static  enum car_state  cstate = car_NORMAL;

#define  speed  1

void
print_buggy (void)
{
  if (state->t >= -0.5) {
    cstate = state->state;
    ++state;
  }
  
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
    mvwaddstr (moon, LINES-8, car_base-speed, "         ");
    mvwaddstr (moon, LINES-7, car_base-speed, "         ");
    mvwaddstr (moon, LINES-6, car_base-speed, "    omm  ");
    mvwaddstr (moon, LINES-5, car_base-speed, " (o)_(o) ");
    break;
  }

  wnoutrefresh (moon);
}

void
jump (double t)
{
  struct scene *sptr = sz_jump;

  if (cstate != car_NORMAL) {
    assert (cstate == car_LAND);
    remove_event (car_NORMAL, &t);
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
  if (cstate == car_UP1 || cstate == car_UP2)  return 0;
  if (ground2[car_base-speed+2] == ' ') {
    mvwaddstr (moon, LINES-8, car_base-speed, "         ");
    mvwaddstr (moon, LINES-7, car_base-speed, "  m      ");
    mvwaddstr (moon, LINES-6, car_base-speed, "  Om     ");
    mvwaddstr (moon, LINES-5, car_base-speed, " (;-(*)  ");
    mvwaddch (moon, LINES-4, car_base-speed+2, 'o');
    wnoutrefresh (moon);
    return 1;
  }
  if (ground2[car_base-speed+6] == ' ') {
    mvwaddstr (moon, LINES-8, car_base-speed, "         ");
    mvwaddstr (moon, LINES-7, car_base-speed, "         ");
    mvwaddstr (moon, LINES-6, car_base-speed, "     0   ");
    mvwaddstr (moon, LINES-5, car_base-speed, " (*;_(m. ");
    mvwaddch (moon, LINES-4, car_base-speed+6, 'o');
    wnoutrefresh (moon);
    return 1;
  }
  return  0;
}
