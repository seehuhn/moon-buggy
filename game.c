/* game.c - play the game
 *
 * Copyright (C) 1998  Jochen Voss.  */

static const  char  rcsid[] = "$Id: game.c,v 1.2 1999/01/01 18:05:20 voss Exp $";


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "moon.h"


long  score, bonus;
static  int  lives;
static  struct circle_buffer *load_meter = NULL;


static void
print_score (void)
{
  mvwprintw (status, 0, car_base-7, "score: %-8ld", score);
  wnoutrefresh (status);
}

static void
print_lives (void)
{
  mvwprintw (status, 0, car_base-20, "lives: %d", lives);
  wnoutrefresh (status);
}

static double
limited (double min, double x, double max)
/* Return the point of [MIN; MAX], which is nearest to X.  */
{
  if (x < min)  return min;
  if (x > max)  return max;
  return  x;
}

static void
spend_life (void)
{
  double  base, t, sleep_delta;
  int  done = 0;

  bonus = 0;
  print_ground ();
  print_score ();
  print_lives ();

  clear_queue ();

  initialize_buggy ();
  print_buggy ();
  
  doupdate ();
  
  base = vclock ();
  add_event (base+1, ev_SCROLL);
  sleep_meter = 0;
  sleep_delta = 1;
  add_event (base+3, ev_MESSAGE);
  add_event (base+TICK(75), ev_SCORE);

  do {
    switch (get_event (&t)) {
    case ev_SCROLL:
      scroll_ground ();
      print_ground ();
      if (ground2[score_base] == ' ')  ++bonus;
      if (crash_check ())  done = 1;
      add_value (load_meter,
		 limited (0, (sleep_delta-sleep_meter)/sleep_delta, 1));
#if 0
      mvwprintw (status, 0, 0, "load: %d%%  ",
		 (int)(100.0*get_mean (load_meter)+.5));
      wnoutrefresh (status);
#endif
      add_event (t+TICK(1), ev_SCROLL);
      sleep_meter = 0;
      sleep_delta = TICK(1);
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
      case 27:			/* ESC */
      case 'q':
	done = 1;
	lives = 1;
	break;
      }
      break;
    case ev_MESSAGE:
      wclear (message);
      wnoutrefresh (message);
      break;
    case ev_BUGGY:
      print_buggy ();
      break;
    case ev_SCORE:
      ++score;
      print_score ();
      add_event (t+TICK(12.5), ev_SCORE);
      break;
    case ev_TIMEOUT:		/* should not happen */
      break;
    }
    doupdate ();
  } while (! done);
}

static void
setup_screen (void)
{
  wclear (moon);
  wclear (status);
  resize_ground (1);
}

int
game_mode (void)
{
  int  i;

  game_state = PLAYING;
  setup_screen ();

  if (load_meter == NULL)  load_meter = new_circle_buffer ();
  
  score = 0;
  lives = 3;
  do {
    for (i=car_base-4; i<car_base+8; ++i) {
      ground2[i] = '#';
    }
    spend_life ();
    --lives;
    if (lives > 0) {
      int  done = 0;
      
      clear_queue ();
      add_event (vclock()+1, ev_TIMEOUT);
      do {
	switch (get_event (NULL)) {
	case ev_TIMEOUT:
	  done = 1;
	  break;
	case ev_KEY:
	  switch (wgetch (moon)) {
	  case KEY_BREAK:
	  case KEY_CANCEL:
	  case KEY_EXIT:
	  case 'q':
	    done = 1;
	    lives = 0;
	    break;
	  case KEY_BEG:
	  case KEY_ENTER:
	  case ' ':
	    done = 1;
	    break;
	  default:
	    beep ();
	    doupdate ();
	    break;
	  }
	  break;
	default:
	  break;
	}
      } while (! done);
    }
  } while (lives > 0);

  wattron (moon, A_BLINK);
  mvwaddstr (moon, LINES-11, car_base-1, "GAME OVER");
  wattroff (moon, A_BLINK);

  print_lives ();

  return  highscore_mode ();
}

void
resize_game (void)
{
  wclear (moon);
  wclear (status);
  resize_ground (0);
  print_ground ();
  print_score ();
  print_lives ();
  print_buggy ();
}

void
score_bonus (int x)
{
  score += x;
  print_score ();
}
