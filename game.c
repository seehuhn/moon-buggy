/* game.c - play the game
 *
 * Copyright 1999  Jochen Voss  */

static const  char  rcsid[] = "$Id: game.c,v 1.15 1999/05/22 13:43:58 voss Exp $";

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "mbuggy.h"


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
score_handler (game_time t, void *client_data)
{
  ++score;
  print_score ();
  add_event (t+TICK(12.5), score_handler, NULL);
}

static void
life_key_handler (game_time t)
{
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
    quit_main_loop ();
    lives = 1;
    print_message ("aborted at user's request");
    break;
  case 'a':
    fire_laser (t);
    break;
  }
}

static void
spend_life (void)
{
  bonus = 0;
  print_ground ();
  print_score ();
  print_lives ();

  clear_queue ();

  initialise_buggy ();
  print_buggy ();

  start_scrolling (1);
  add_event (3, clear_message_h, NULL);
  add_event (TICK(75), score_handler, NULL);

  main_loop (1, life_key_handler);

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

  car_x = car_base;
  
  game_state = PLAYING;
  setup_screen ();
  
  score = 0;
  lives = 3;
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

  return  highscore_mode ();
}

void
resize_game (void)
{
  werase (moon);
  werase (status);
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
