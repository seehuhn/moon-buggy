/* game.c - play the game
 *
 * Copyright 1999  Jochen Voss  */

static const  char  rcsid[] = "$Id: game.c,v 1.6 1999/03/08 20:32:54 voss Exp $";

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "moon.h"


long  score, bonus;
static  int  lives;


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

static void
spend_life (void)
{
  double  t;
  int  done = 0;

  bonus = 0;
  print_ground ();
  print_score ();
  print_lives ();

  clear_queue ();

  initialise_buggy ();
  print_buggy ();
  
  doupdate ();
  
  add_event (1, ev_SCROLL);
  add_event (3, ev_MESSAGE);
  add_event (TICK(75), ev_SCORE);
  clock_adjust_delay (1);

  do {
    switch (get_event (&t)) {
    case ev_SCROLL:
      scroll_ground ();
      print_ground ();
      if (ground2[score_base] == ' ')  ++bonus;
      if (crash_check ())  done = 1;
      add_event (t+TICK(1), ev_SCROLL);
      break;
    case ev_KEY:
      switch (xgetch (moon)) {
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
      if (print_buggy ())  done = 1;
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
  
  score = 0;
  lives = 3;
  do {
    int  done = 0;

    for (i=car_base-4; i<car_base+8; ++i) {
      ground2[i] = '#';
    }
    spend_life ();
    --lives;

    print_lives ();
    doupdate ();
    
    clear_queue ();
    add_event (0, ev_TIMEOUT);
    clock_adjust_delay (2);
    
    do {
      switch (get_event (NULL)) {
      case ev_TIMEOUT:
	done = 1;
	break;
      case ev_KEY:
	switch (xgetch (moon)) {
	case KEY_BREAK:
	case KEY_CANCEL:
	case KEY_EXIT:
	case 'q':
	  done = 1;
	  lives = 0;
	  break;
	case KEY_BEG:
	case KEY_ENTER:
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
  } while (lives > 0);

  wattron (moon, A_BLINK);
  mvwaddstr (moon, LINES-11, car_base-1, "GAME OVER");
  wattroff (moon, A_BLINK);

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
