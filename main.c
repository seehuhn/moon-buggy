/* main.c - moon-buggy main file
 *
 * Copyright (C) 1998  Jochen Voss.  */

static const  char  rcsid[] = "$Id: main.c,v 1.4 1998/12/20 00:45:35 voss Exp $";

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if STDC_HEADERS
# include <string.h>
#endif
#include <unistd.h>		/* TODO: remove */

#include "moon.h"


const char *my_name;
unsigned long  score, bonus;
static int  lives;


static void
print_time (double t)
{
  int  tt = t*100+0.5;
  int  min, sec, frac;

  frac = tt%100;
  tt /= 100;
  sec = tt%60;
  min = tt/60;
  mvwprintw (status, 0, COLS-17, "time: %2d:%02d.%02d", min, sec, frac);
  wnoutrefresh (status);
}

static void
print_score ()
{
  mvwprintw (status, 0, 0, "score: %lu", score);
  wnoutrefresh (status);
}

static void
print_lives ()
{
  mvwprintw (status, 0, 20, "lives: %d", lives);
  wnoutrefresh (status);
}

static void
print_ground ()
{
  mvwaddchnstr (moon, LINES-4, 0, ground2, COLS);
  wnoutrefresh (moon);
}

static void
main_loop (double wait)
{
  double  base, t;
  int  done = 0;

  doupdate ();
  
  base = vclock ();
  add_event (base+3, ev_STATUS);
  add_event (base+wait, ev_SCROLL);
  add_event (base+wait+5, ev_SCORE);

  do {
    switch (get_event (&t)) {
    case ev_SCROLL:
      scroll_ground ();
      print_ground ();
      if (ground2[score_base] == ' ')  ++bonus;
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
	lives = 1;
	break;
      }
      break;
    case ev_STATUS:
      wmove (status, 1, 0);
      wclrtoeol (status);
      wnoutrefresh (status);
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

static void
do_one_game ()
{
  int  i;
  
  wclear (moon);
  wmove (status, 0, 0);
  wclrtoeol (status);
  
  for (i=0; i<COLS; ++i) {
    ground1[i] = '#';
    ground2[i] = '#';
  }
  mvwaddchnstr (moon, LINES-3, 0, ground1, COLS);
  car_base = (COLS > 80 ? 80 : COLS) - 12;
  score_base = car_base + 7;

  score = 0;
  lives = 3;
  do {
    bonus = 0;
    print_ground ();
    print_score ();
    print_lives ();
    print_buggy ();

    clear_queue ();
    main_loop (1);
    --lives;
    if (lives > 0) {
      sleep (1);
      for (i=car_base-4; i<car_base+8; ++i) {
	ground2[i] = 'X';
      }
    }
  } while (lives > 0);

  wattron (moon, A_BLINK);
  mvwaddstr (moon, LINES-11, car_base-1, "GAME OVER");
  wattroff (moon, A_BLINK);

  print_lives ();
  write_hiscore ();
}

void
prepare_for_exit (void)
{
  wmove (status, 1, 0);
  wclrtoeol (status);
  wnoutrefresh (moon);
  wnoutrefresh (status);
  doupdate ();
  
  delwin (moon);
  delwin (status);
  endwin ();
}

int
main (int argc, char **argv)
{
  int  done = 0;
  
  my_name = xstrdup (basename (argv[0]));
  
  initscr ();
  cbreak ();
  noecho ();
  
  status = newwin (2, 0, LINES-2, 0);
  keypad (status, TRUE);
  moon = newwin (LINES-2, 0, 0, 0);
  keypad (moon, TRUE);

  queuelag = new_lagmeter ();
  ground1 = xmalloc (COLS*sizeof(chtype));
  ground2 = xmalloc (COLS*sizeof(chtype));

  mvwaddstr (status, 1, 0, "good luck");

  do {
    int  c;
    do_one_game ();
  ask:
    mvwaddstr (status, 1, 0, "play again (y/n)?");
    c = wgetch (status);
    switch (c) {
    case 'y':
    case 'Y':
      break;
    case 'n':
    case 'N':
    case 'q':
    case 'Q':
      done = 1;
      break;
    default:
      beep ();
      goto ask;
    }
  } while (! done);

  mvwaddstr (moon, LINES-11, car_base-1, "GAME OVER");
  
  prepare_for_exit ();
  return  0;
}
