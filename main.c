/* main.c - moon-buggy main file
 *
 * Copyright (C) 1998  Jochen Voss.  */

static const  char  rcsid[] = "$Id: main.c,v 1.1 1998/12/17 20:00:25 voss Exp $";

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if STDC_HEADERS
# include <string.h>
# include <stdlib.h>
#else
# ifndef HAVE_MEMCPY
#  define memmove(d, s, n) bcopy ((s), (d), (n))
# endif
#endif
#include <curses.h>

#include "moon.h"


const char *my_name;

WINDOW *score, *moon;
static  chtype *ground1, *ground2, *ground3;


static int
d_rnd (int anz)
/* Returns a random integer `x' with `0 <= x < anz'.  */
{
  return  (int)((double)anz*rand()/(RAND_MAX+1.0));
}

static void
scroll_ground (void)
{
  memmove (ground1+1, ground1, (COLS-1)*sizeof(chtype));
  memmove (ground2+1, ground2, (COLS-1)*sizeof(chtype));
  memmove (ground3+1, ground3, (COLS-1)*sizeof(chtype));
  ground1[0] = d_rnd(30) ? ' ' : '*';
  ground2[0] = d_rnd(10) ? '#' : ' ';
  ground3[0] = '#';
}

static void
print_car (int speed, int jump)
{
  if (jump) {
    mvwaddstr (moon, LINES-7, COLS-12-speed, "    Omm  ");
    mvwaddstr (moon, LINES-6, COLS-12-speed, " (0)-(0) ");
    mvwaddstr (moon, LINES-5, COLS-12-speed, "         ");
  } else {
    mvwaddstr (moon, LINES-7, COLS-12-speed, "         ");
    mvwaddstr (moon, LINES-6, COLS-12-speed, "    Omm  ");
    mvwaddstr (moon, LINES-5, COLS-12-speed, " (0)-(0) ");
  }
}

static void
print_time (double t)
{
  int  tt = t*100+0.5;
  int  min, sec, frac;

  frac = tt%100;
  tt /= 100;
  sec = tt%60;
  min = tt/60;
  mvwprintw (score, 1, 0, "%5d:%02d:%02d", min, sec, frac);
  wnoutrefresh (score);
}

static void
test_output (void)
{
  double  base1, base2, t;

  base1 = base2 = vclock ();
  do {
    t = vclock ();
    if (t > base2+0.05) {
      scroll_ground ();
      base2 += 0.05;
      mvwaddchnstr (moon, LINES-5, 0, ground1, COLS);
      mvwaddchnstr (moon, LINES-4, 0, ground2, COLS);
      wnoutrefresh (moon);
    }
    if (ground1[COLS-13] != ' ')  print_car (0, 1);
    print_time (t-base1);
    doupdate ();
  } while (t < base1+50);
}

int
main (int argc, char **argv)
{
  int  i;
  
  my_name = xstrdup (basename (argv[0]));
  
  initscr ();
  cbreak ();
  noecho ();

  score = newwin (2, 0, LINES-2, 0);
  moon = newwin (LINES-2, 0, 0, 0);
  ground1 = xmalloc (COLS*sizeof(chtype));
  ground2 = xmalloc (COLS*sizeof(chtype));
  ground3 = xmalloc (COLS*sizeof(chtype));
  for (i=0; i<COLS; ++i) {
    ground1[i] = ' ';
    ground2[i] = '#';
    ground3[i] = '#';
  }
  mvwaddchnstr (moon, LINES-3, 0, ground3, COLS);
  print_car (0, 0);

  test_output ();
  
  delwin (moon);
  delwin (score);
  endwin ();

  return  0;
}
