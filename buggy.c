/* buggy.c - implement the moon buggy
 *
 * Copyright 1999  Jochen Voss  */

static const  char  rcsid[] = "$Id: buggy.c,v 1.13 1999/05/22 13:43:58 voss Exp $";

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <assert.h>

#include "mbuggy.h"
#include "buggy.h"


int  car_x, car_y;


typedef  struct scene {
  enum car_state  n;		/* image number (index to array `image') */
  int  y;			/* vertical position */
  double  dt;			/* time to next state */
  int  has_ground;		/* flag, true iff we may crash or jump */
}  scenario [];

static  scenario  sz_empty = {
  { car_NORMAL, 5, -1, 1 }
};

static scenario sz_jump = {
  { car_START, 5, TICK(1.25), 0 },
  { car_UP1, 6, TICK(2.5), 0 },
  { car_UP2, 7, TICK(5), 0 },
  { car_UP1, 6, TICK(2.5), 0 },
  { car_LAND, 5, TICK(2.5), 1 },
  { car_NORMAL, 5, -1, 1 }
};

static  struct scene *state;


void
initialise_buggy (void)
{
  state = sz_empty;
}

void
print_buggy (void)
{
  enum car_state  n = state->n;
  int  y = state->y;

  if (y < car_y) {
    mvwaddstr (moon, LINES-car_y-1, car_x, "       ");
  } else if (y > car_y) {
    mvwaddstr (moon, LINES-car_y, car_x, "       ");
  }
  car_y = y;
  mvwaddstr (moon, LINES-y-1, car_x, image[n][0]);
  mvwaddstr (moon, LINES-y, car_x, image[n][1]);
  wnoutrefresh (moon);
}

static void
jump_handler (game_time t, void *client_data)
{
  state = client_data;
  print_buggy ();
  if (crash_check ())  quit_main_loop ();
  if (state->dt >= -0.5) {
    add_event (t+state->dt, jump_handler, state+1);
  }
}

void
jump (double t)
{
  assert (state->has_ground);
  remove_event (jump_handler);	/* only on jump at a time */
  add_event (t, jump_handler, sz_jump);
}

int
can_jump (void)
{
  return  state->has_ground;
}

int
crash_check (void)
/* Return true, if the car crashed.  */
{
  if (! state->has_ground)  return 0;
  if (ground2[car_x+1] == ' ') {
    mvwaddstr (moon, LINES-car_y-1, car_x, "       ");
    mvwaddstr (moon, LINES-car_y, car_x, "       ");
    mvwaddstr (moon, LINES-8, car_x, "       ");
    mvwaddstr (moon, LINES-7, car_x, " m     ");
    mvwaddstr (moon, LINES-6, car_x, " Om    ");
    mvwaddstr (moon, LINES-5, car_x, "(;-(*) ");
    mvwaddch (moon, LINES-4, car_x+1, 'o');
    wnoutrefresh (moon);
    return 1;
  }
  if (ground2[car_x+5] == ' ') {
    mvwaddstr (moon, LINES-car_y-1, car_x, "       ");
    mvwaddstr (moon, LINES-car_y, car_x, "       ");
    mvwaddstr (moon, LINES-8, car_x, "       ");
    mvwaddstr (moon, LINES-7, car_x, "       ");
    mvwaddstr (moon, LINES-6, car_x, "    0  ");
    mvwaddstr (moon, LINES-5, car_x, "(*;_(m.");
    mvwaddch  (moon, LINES-4, car_x+5,    'o');
    wnoutrefresh (moon);
    return 1;
  }
  if (bonus) {
    score_bonus (1<<bonus);
    bonus = 0;
  }

#if MB_DEBUG
  ground2[car_x+1] = 'O';
  ground2[car_x+5] = 'O';
#endif
  
  return  0;
}
