/* moon.c - implement the moon to drive on
 *
 * Copyright 1999  Jochen Voss  */

static const  char  rcsid[] = "$Id: moon.c,v 1.16 1999/05/22 17:12:12 voss Exp $";


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#if STDC_HEADERS
# include <string.h>
# include <stdlib.h>
#else
# ifndef HAVE_MEMMOVE
#  define memmove(d, s, n) bcopy ((s), (d), (n))
# endif
#endif
#include <assert.h>

#include "mbuggy.h"


char *ground1, *ground2;
static int  ground_width;


void
resize_ground (int clear_it)
{
  int  cols, i;

  block_winch ();
  cols = COLS;
  if (ground_width != cols) {
    ground1 = xrealloc (ground1, cols);
    ground2 = xrealloc (ground2, cols);
  }
  for (i=(clear_it ? 0 : ground_width); i<cols; ++i) {
    ground1[i] = '#';
    ground2[i] = '#';
  }
  ground_width = cols;
  car_base = (cols > 80 ? 80 : cols) - 12;
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
scroll_handler (game_time t, void *client_data)
{
  char  nextchar = control_tick () ? ' ' : '#';
  
  memmove (ground2+1, ground2, ground_width-1);
  ground2[0] = nextchar;
  print_ground ();

  if (ground2[car_x + 7] == ' ')  ++bonus;
  if (crash_detected)  shift_buggy (1);
  if (crash_detected || crash_check ()) {
    ++crash_detected;
    if (crash_detected >= 2)  quit_main_loop ();
  }
  
  add_event (t+TICK(1), scroll_handler, NULL);
}

void
start_scrolling (double t)
{
  add_event (t, scroll_handler, NULL);
}
