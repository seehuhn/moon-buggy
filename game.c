/* game.c - play the game
 *
 * Copyright 1999  Jochen Voss  */

static const  char  rcsid[] = "$Id: game.c,v 1.32 2000/04/01 07:54:53 voss Exp $";

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <signal.h>

#include "moon-buggy.h"


struct mode *game_mode;
struct mode *pause_mode;


int  crash_detected;		/* a crash is in progress */
static  int  level;		/* the current level (pause_mode only) */
static  int  lives;		/* cars left (including the current one) */
static  int  score;		/* points we already got */
int  stakes;			/* points to get, when we reach the ground */


void
adjust_score (int val)
/* Add VAL to the score and display the result.  */
{
  if (crash_detected)  return;
  score += val;
  mvwprintw (status, 0, car_base-7, "score: %-8d", score);
  wnoutrefresh (status);
}

void
print_lives (void)
{
  mvwprintw (status, 0, car_base-20, "lives: %d", lives);
  wnoutrefresh (status);
}

/**********************************************************************
 * game mode
 */

static void
setup_screen (void)
{
  resize_ground (1);
}

static void
game_enter (int seed)
{
  clock_reset ();

  if (seed == 0) {
    level = 0;
    score = 0;
    lives = 3;
    setup_screen ();
  }

  resize_ground (1);
  level_start (level);

  crash_detected = 0;
  stakes = 0;
  print_ground ();
  print_lives ();

  initialise_buggy ();
  print_buggy ();

  start_scrolling (1);
}

static void
game_leave (void)
{
  --lives;
  level = current_level ();
  extinguish_laser ();
  remove_meteors ();
}

static void
resize_game (void)
{
  resize_meteors ();
  resize_laser ();
  resize_ground (0);

  print_ground ();
  adjust_score (0);
  print_lives ();
  print_buggy ();
}

static void
key_handler (game_time t, int val)
{
  switch (val) {
  case 1:
    if (! crash_detected && can_jump())  jump (t);
    break;
  case 2:
    if (! crash_detected)  fire_laser (t);
    break;
  case 3:
    lives = 1;
    print_message ("aborted at user's request");
    mode_change (pause_mode, 0);
    break;
  }
}

static void
signal_handler (int signum)
{
  switch (signum) {
  case SIGTSTP:
    if (lives > 1)  lives = 1;
    if (! crash_detected)  crash_detected = 1;
    break;
  case SIGCONT:
    print_message ("GAME OVER (suspended)");
    break;
  }
}

void
leave_pause_mode (game_time t, void *client_data)
/* This function is a possible callback argument to `add_event'.
 * It causes the main loop to terminate.
 * The arguments T and CLIENT_DATA are ignored.  */
{
  if (lives > 0) {
    mode_change (game_mode, 1);
  } else {
    score_set (score);
    mode_change (highscore_mode, level+1);
  }
}

static void
print_lives_h (game_time t, void *client_data)
/* This function is a possible callback argument to `add_event'.
 * It updates the number of lives display.  */
{
  print_lives ();
}

static void
pause_enter (int seed)
{
  clock_reset ();
  add_event (1.5, print_lives_h, NULL);
  add_event (2.0, leave_pause_mode, NULL);
  print_ground ();
  adjust_score (0);
  print_lives ();
  print_buggy ();
  if (lives <= 10) {
#ifdef A_BLINK
    wattron (moon, A_BLINK);
#endif
    mvwaddstr (moon, LINES-11, car_base-1, "GAME OVER");
#ifdef A_BLINK
    wattroff (moon, A_BLINK);
#endif
  }
}

static void
pause_key_handler (game_time t, int val)
{
  if (t < 0.5)  return;
  
  switch (val) {
  case 1:
    leave_pause_mode (0, NULL);
    break;
  case 3:
    lives = 0;
    leave_pause_mode (0, NULL);
    break;
  }
}

void
setup_game_mode (void)
{
  game_mode = new_mode ();
  game_mode->enter = game_enter;
  game_mode->leave = game_leave;
  game_mode->redraw = resize_game;
  game_mode->keypress = key_handler;
  game_mode->signal = signal_handler;
  mode_add_key (game_mode, mbk_jump, "jump", 1);
  mode_add_key (game_mode, mbk_fire, "fire", 2);
  mode_add_key (game_mode, mbk_end, "abort game", 3);

  pause_mode = new_mode ();
  pause_mode->enter = pause_enter;
  pause_mode->keypress = pause_key_handler;
  mode_add_key (pause_mode, mbk_start, "continue", 1);
  mode_add_key (pause_mode, mbk_end, "abort game", 3);
}
