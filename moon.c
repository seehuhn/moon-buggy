/* moon.c - implement the moon to drive on
 *
 * Copyright 1999  Jochen Voss  */

static const  char  rcsid[] = "$Id: moon.c,v 1.22 1999/06/03 13:20:11 voss Exp $";


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>
#include <stdlib.h>

#include "mbuggy.h"


int *bonus;			/* points to get, if we drive over them */
char *ground1, *ground2;
static int  ground_width;


void
resize_ground (int clear_it)
{
  int  cols, i, old;

  block_winch ();
  cols = COLS;
  if (ground_width != cols) {
    bonus = xrealloc (bonus, cols*sizeof(int));
    ground1 = xrealloc (ground1, cols);
    ground2 = xrealloc (ground2, cols);
  }
  for (i=(clear_it ? 0 : ground_width); i<cols; ++i) {
    bonus[i] = 0;
    ground1[i] = '#';
    ground2[i] = '#';
  }
  ground_width = cols;
  old = car_base;
  car_base = (cols > 80 ? 80 : cols) - 12;
  car_x += (car_base-old);
  unblock ();
}

void
print_ground (void)
{
  mvwaddnstr (moon, LINES-4, 0, ground2, ground_width);
  mvwaddnstr (moon, LINES-3, 0, ground1, ground_width);
  wnoutrefresh (moon);
}

static void
print_level (void)
{
  mvwprintw (status, 0, car_base-32, "level: %d", current_level () + 1);
  wnoutrefresh (status);
}

static void
scroll_handler (game_time t, void *client_data)
{
  memmove (bonus+1, bonus, (ground_width-1)*sizeof(int));
  memmove (ground2+1, ground2, ground_width-1);
  level_tick (t);
  print_ground ();
  print_level ();

  stakes += bonus[car_x + 7];
  
  if (crash_detected)  shift_buggy (1);
  if (crash_detected || crash_check ()) {
    ++crash_detected;
    if (crash_detected > 2)  quit_main_loop ();
  }

  if (can_jump () && stakes) {
    adjust_score (stakes);
    stakes = 0;
  }
  
  add_event (t+TICK(1), scroll_handler, NULL);
}

void
start_scrolling (double t)
{
  add_event (t, scroll_handler, NULL);
}
