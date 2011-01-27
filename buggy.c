/* buggy.c - implement the moon buggy
 *
 * Copyright 1999, 2004  Jochen Voss  */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <assert.h>

#include "moon-buggy.h"
#include "buggy.h"


int  car_x, car_y;

/**********************************************************************
 * display the car
 */

typedef  struct scene {
  enum car_state  n;		/* image number (index to array `image') */
  int  y;			/* vertical position */
  double  dt;			/* time to next state */
  int  has_ground;		/* flag, true iff we may crash or jump */
}  scenario [];

static  scenario  sz_empty = {
  { car_NORMAL, 5, TICK(1), 1 },
  { car_NORMAL1, 5, TICK(2.5), 1 },
  { car_NORMAL2, 5, TICK(4), 1 },
  { car_NORMAL3, 5, TICK(5.5), 1 }
};

static scenario sz_jump = {
  { car_START, 5, TICK(1), 0 },
  { car_UP1, 6, TICK(2.5), 0 },
  { car_UP2, 7, TICK(5), 0 },
  { car_UP1, 6, TICK(2.5), 0 },
  { car_LAND, 5, TICK(2.5), 1 },
  { car_NORMAL, 5, -1, 1 }
};

static  scenario  sz_crash = {
  { car_BROKEN, 5, -1, 0 }
};

static  scenario  sz_ram = {
  { car_RAM1, 5, TICK(1), 0 },
  { car_RAM2, 5, TICK(0.5), 0 },
  { car_RAM3, 5, -1, 0 }
};

static  scenario  sz_sit = {
  { car_SIT, 5, -1, 0 }
};

static  struct scene *state;

static  int  nextG;


void
initialise_buggy (void)
/* Reset the buggy to its initial state.  */
{
  int  y;

  state = sz_empty;
  for (y=5; y<9; ++y)  mvwaddstr (moon, LINES-y, car_x, "       ");
  car_x = car_base;
  car_y = state->y;
  wnoutrefresh (moon);
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

/* ++pg the changing of the 4 normal states is realized
        in the following lines.
*/
  if (n==car_NORMAL)  {
    mvwaddstr (moon, LINES-y-1, car_x, image[n+nextG][0]);
    mvwaddstr (moon, LINES-y, car_x, image[n+nextG][1]);
  } else {
    mvwaddstr (moon, LINES-y-1, car_x, image[n][0]);
    mvwaddstr (moon, LINES-y, car_x, image[n][1]);
  }
  nextG++; if (nextG>3) nextG=0;


  if (n == car_BROKEN) {
    if (ground2[car_x+1] == ' ')  mvwaddch (moon, LINES-4, car_x+1, 'o');
    if (ground2[car_x+5] == ' ')  mvwaddch (moon, LINES-4, car_x+5, 'o');
  }
  wnoutrefresh (moon);
}

void
shift_buggy (int dx)
/* Horizontally shift the buggy by the amount DX.
 * Positive values of dx indicate a shift to the right.  */
{
  mvwaddstr (moon, LINES-car_y-1, car_x, "       ");
  mvwaddstr (moon, LINES-car_y, car_x, "       ");
  car_x += dx;
  print_buggy ();
}

/**********************************************************************
 * display a rolling wheel after a crash
 */

static int wheel_x, wheel_y;

static void
wheel_handler (game_time t, void *client_data)
{
  int  wheel_crash;

  wheel_crash = (wheel_x<car_x && wheel_y==LINES-5 && ground2[wheel_x]==' ');
  if (wheel_x < car_x)  mvwaddch (moon, wheel_y, wheel_x, ' ');
  wheel_x -= 1;
  switch (car_x - wheel_x) {
  case 1:
  case 5:
  case 7:
  case 8:
  case 9:
    wheel_y = LINES - 6;
    break;
  case 2:
  case 3:
  case 4:
    wheel_y = LINES - 7;
    break;
  default:
    wheel_y = LINES - 5;
    break;
  }
  if (wheel_x >= 0 && ! wheel_crash) {
    mvwaddch (moon, wheel_y, wheel_x, 'o');
    add_event (t+TICK(2.3), wheel_handler, NULL);
  } else {
    crash_detected = 1000;
  }
  wnoutrefresh (moon);
}

static void
start_wheel (void)
{
  wheel_x = car_x;
  add_event (current_time()+TICK(0.5), wheel_handler, NULL);
}

/**********************************************************************
 * handle the jumps
 */

static void
jump_handler (game_time t, void *client_data)
{
  state = client_data;
  if (car_y > 5 && state->y == 5) {
    if (meteor_car_hit (car_x, car_x+7)) {
      state = sz_sit;
      start_wheel ();
      crash_detected = 1;
    }
  }
  print_buggy ();
  if (state->dt >= -0.5) {
    add_event (t+state->dt, jump_handler, state+1);
  }
}

void
jump (game_time t)
{
  assert (state->has_ground);
  remove_event (jump_handler);	/* only one jump at a time */
  add_event (t, jump_handler, sz_jump);
}

int
can_jump (void)
{
  return  state->has_ground;
}

/**********************************************************************
 * check for crashes
 */

int
crash_check (void)
/* Return true, if the car crashed.  */
{
  if (! state->has_ground)  return 0;
  if (ground2[car_x+1] == ' ' || ground2[car_x+5] == ' ') {
    remove_event (jump_handler);
    state = sz_crash;
    print_buggy ();
    start_wheel ();
    return 1;
  }

  return  0;
}

int
car_meteor_hit (int x)
/* Return true, if the car is down and occupies position X.
 * Then the car crashes immediately.  */
{
  if (car_y == 5 && x >= car_x && x < car_x+7) {
    remove_event (jump_handler);
    add_event (current_time (), jump_handler, sz_ram);
    print_buggy ();
    start_wheel ();
    crash_detected = 1;
    return 1;
  }

  return  0;
}
