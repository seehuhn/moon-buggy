/* game.c - play the game
 *
 * Copyright 1999  Jochen Voss  */

static const  char  rcsid[] = "$Id: game.c,v 1.18 1999/05/23 14:20:25 voss Exp $";

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "mbuggy.h"


long  bonus;
int  crash_detected;
static  int  lives;
static  long  score;


void
adjust_score (int adjust)
{
  if (crash_detected)  return;
  score += adjust;
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
score_handler (game_time t, void *client_data)
{
  if (crash_detected)  return;
  adjust_score (1);
  add_event (t+TICK(12.5), score_handler, NULL);
}

static void
life_key_handler (game_time t)
{
  switch (xgetch (moon)) {
  case ' ':
    if (! crash_detected && can_jump()) {
      jump (t);
      adjust_score (+1);
    }
    break;
  case KEY_BREAK:
  case KEY_CLOSE:
  case 27:			/* ESC */
  case 'q':
    quit_main_loop ();
    lives = 1;
    print_message ("aborted at user's request");
    break;
  case 'a':
    if (! crash_detected)  fire_laser (t);
    break;
  }
}

static void
spend_life ()
{
  crash_detected = 0;
  bonus = 0;
  print_ground ();
  print_lives ();

  clear_queue ();

  initialise_buggy ();
  print_buggy ();

  start_scrolling (1);
  add_event (TICK(75), score_handler, NULL);

  main_loop (0.5, life_key_handler);

  extinguish_laser ();
}

static void
setup_screen (void)
{
  werase (moon);
  werase (status);
  resize_ground (1);
}

static void
game_key_handler (game_time t)
{
  switch (xgetch (moon)) {
  case KEY_BREAK:
  case KEY_CANCEL:
  case KEY_EXIT:
  case 'q':
    quit_main_loop ();
    lives = 0;
    break;
  case KEY_BEG:
  case KEY_ENTER:
    quit_main_loop ();
    break;
  default:
    beep ();
    break;
  }
}

int
game_mode (void)
{
  int  i;
  
  game_state = PLAYING;
  setup_screen ();
  
  score = 0;
  lives = 3;
  level_start ();
  do {
    for (i=car_base-4; i<car_base+8; ++i) {
      ground2[i] = '#';
    }
    spend_life ();
    --lives;

    print_lives ();

    add_event (2, quit_main_loop_h, NULL);
    main_loop (2, game_key_handler);
    remove_meteors ();
  } while (lives > 0);

  wattron (moon, A_BLINK);
  mvwaddstr (moon, LINES-11, car_base-1, "GAME OVER");
  wattroff (moon, A_BLINK);

  return  highscore_mode (score);
}

void
resize_game (void)
{
  werase (moon);
  werase (status);
  resize_ground (0);
  print_ground ();
  adjust_score (0);
  print_lives ();
  print_buggy ();
}
