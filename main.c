/* main.c - moon-buggy main file
 *
 * Copyright (C) 1998  Jochen Voss.  */

static const  char  rcsid[] = "$Id: main.c,v 1.3 1998/12/18 23:21:28 voss Exp $";

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if STDC_HEADERS
# include <string.h>
#endif

#include "moon.h"


const char *my_name;
unsigned long  score;


static void
print_time (double t)
{
  int  tt = t*100+0.5;
  int  min, sec, frac;

  frac = tt%100;
  tt /= 100;
  sec = tt%60;
  min = tt/60;
  mvwprintw (status, 1, COLS-17, "time: %2d:%02d:%02d", min, sec, frac);
  wnoutrefresh (status);
}

static void
print_score ()
{
  mvwprintw (status, 0, COLS-17, "score: %lu", score);
  wnoutrefresh (status);
}

static void
main_loop (void)
{
  double  base, t;
  int  done = 0;

  base = vclock ();
  add_event (base, ev_SCROLL);
  add_event (base+5, ev_SCORE);

  do {
    switch (get_event (&t)) {
    case ev_SCROLL:
      scroll_ground ();
      mvwaddchnstr (moon, LINES-4, 0, ground2, COLS);
      wnoutrefresh (moon);
      if (crash_check ())  done = 1;
      add_event (t+0.08, ev_SCROLL);
      break;
    case ev_KEY:
      switch (wgetch (moon)) {
      case ' ':
	if (can_jump()) {
	  jump (t);
	  ++score;
	  print_score ();
	}
	break;
      case KEY_BREAK:
      case KEY_CLOSE:
      case '\e':
      case 'q':
	done = 1;
	break;
      }
      break;
    case ev_BUGGY:
      print_buggy ();
      break;
    case ev_SCORE:
      ++score;
      print_score ();
      add_event (t+1, ev_SCORE);
      break;
    }
    print_time (t-base);
    doupdate ();
  } while (! done);
}

int
main (int argc, char **argv)
{
  int  i;
  
  my_name = xstrdup (basename (argv[0]));
  
  initscr ();
  cbreak ();
  noecho ();

  queuelag = new_lagmeter ();
  
  status = newwin (2, 0, LINES-2, 0);
  moon = newwin (LINES-2, 0, 0, 0);
  keypad (moon, TRUE);
  
  ground1 = xmalloc (COLS*sizeof(chtype));
  ground2 = xmalloc (COLS*sizeof(chtype));
  for (i=0; i<COLS; ++i) {
    ground1[i] = '#';
    ground2[i] = '#';
  }
  car_base = (COLS > 80 ? 80 : COLS) - 12;

  mvwaddchnstr (moon, LINES-3, 0, ground1, COLS);
  print_buggy ();

  score = 0;
  print_score ();
  
  main_loop ();
  
  delwin (moon);
  delwin (status);
  endwin ();

  return  0;
}
