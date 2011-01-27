/* game.c - play the game
 *
 * Copyright 1999, 2000  Jochen Voss  */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <signal.h>

#include "moon-buggy.h"


struct mode *game_mode;
struct mode *crash_mode;


int  crash_detected;		/* a crash is in progress */
static  int  level;		/* the current level (crash_mode only) */
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

void
print_game_over (int blink)
{
#ifdef A_BLINK
  if (blink)  wattron (moon, A_BLINK);
#endif
  mvwaddstr (moon, LINES-11, car_base-1, "GAME OVER");
#ifdef A_BLINK
  if (blink)  wattroff (moon, A_BLINK);
#endif
  wnoutrefresh (moon);
}

static void
game_enter (int seed)
{
  clock_reset ();

  if (seed == 0) {
    level = 0;
    score = 0;
    lives = 3;
    werase (status);
    wnoutrefresh (status);
  }

  resize_ground (1);
  level_start (level);

  crash_detected = 0;
  stakes = 0;
  initialise_buggy ();
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
game_redraw (void)
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
    mode_change (crash_mode, 0);
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
    print_message ("GAME OVER (suspended)");
    break;
  case SIGCONT:
    print_message ("GAME OVER (suspended)");
    break;
  }
}

/**********************************************************************
 * crash mode
 */

static  int  lives_flag;

static void
leave_crash_mode (game_time t, void *client_data)
/* This function is a possible callback argument to `add_event'.
 * It switch control back to either `game_mode' or `highscore_mode'.
 * The arguments T and CLIENT_DATA are ignored.  */
{
  if (lives > 0) {
    mode_change (game_mode, 1);
  } else {
    score_set (score, level+1);
    mode_change (highscore_mode, 0);
  }
}

static void
print_lives_h (game_time t, void *client_data)
/* This function is a callback argument to `add_event'.
 * It updates the number of lives display.  */
{
  print_lives ();
  lives_flag = 1;
}

static void
crash_enter (int seed)
{
  clock_reset ();
  lives_flag = 0;
  add_event (1.2, print_lives_h, NULL);
  add_event (2.0, leave_crash_mode, NULL);
  print_buggy ();
  if (lives <= 0)  print_game_over (1);
}

static void
crash_redraw (void)
{
  resize_ground (0);

  print_ground ();
  print_buggy ();
  adjust_score (0);
  if (lives_flag)  print_lives ();
}

static void
crash_key_handler (game_time t, int val)
{
  if (t < 0.5)  return;

  switch (val) {
  case 1:
    leave_crash_mode (0, NULL);
    break;
  case 3:
    lives = 0;
    leave_crash_mode (0, NULL);
    break;
  }
}

void
setup_game_mode (void)
{
  game_mode = new_mode ();
  game_mode->enter = game_enter;
  game_mode->leave = game_leave;
  game_mode->redraw = game_redraw;
  game_mode->keypress = key_handler;
  game_mode->signal = signal_handler;
  mode_add_key (game_mode, mbk_jump, "jump", 1);
  mode_add_key (game_mode, mbk_fire, "fire", 2);
  mode_add_key (game_mode, mbk_end, "abort game", 3);
  mode_complete (game_mode);

  crash_mode = new_mode ();
  crash_mode->enter = crash_enter;
  crash_mode->redraw = crash_redraw;
  crash_mode->keypress = crash_key_handler;
  mode_add_key (crash_mode, mbk_start, "continue", 1);
  mode_add_key (crash_mode, mbk_end, "abort game", 3);
  mode_complete (crash_mode);
}
