/* mode.c - handle the program's modes in a consistent way
 *
 * Copyright (C) 2000  Jochen Voss.  */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>

#include "moon-buggy.h"
#include "darray.h"


static const  struct mode *current;
static  int  mode_entered, mode_seed;


struct mode *
new_mode (void)
/* Allocate a new mode struct.  */
{
  struct mode *res = xmalloc (sizeof (struct mode));
  res->enter = NULL;
  res->leave = NULL;
  res->redraw = NULL;

  DA_INIT (res->keys, struct binding);
  res->keypress = NULL;

  return  res;
}

void
mode_add_key (struct mode *m, int meanings, const char *desc, int res)
/* Add a key binding to mode M.
 * MEANINGS should a combination (via |) of `mbk_key' values, DESC is
 * the label to display near the bottom of the screen.  RES is passed
 * through to `keypress' handler.  */
{
  struct binding *keys;

  DA_ADD_EMPTY (m->keys, struct binding, keys);
  keys->meanings = meanings;
  keys->desc = desc;
  keys->res = res;
}

void
mode_complete (struct mode *m)
/* This must be called at the very end of each mode's initialisation.  */
{
  mode_add_key (m, mbk_redraw, "redraw", 0);
}

void
mode_change (const struct mode *m, int seed)
/* Change into new mode M.  Pass SEED to the `enter' handler.  */
{
  if (mode_entered && current->leave)  current->leave ();

  current = m;
  mode_entered = 0;
  mode_seed = seed;
}

static void
mode_enter (void)
{
  werase (moon);
  wnoutrefresh (moon);
  if (! current)  return;

  if (current->enter)  current->enter (mode_seed);
  mode_entered = 1;
}

void
mode_update (void)
/* Flush queued mode updates.  */
{
  if (! mode_entered) {
    clear_queue ();
    mode_enter ();
    mode_redraw ();
  }
  doupdate ();
}

void
mode_redraw (void)
/* Make the current mode redraw the screen but leave the status message intact.
 * This is also called after the screen is resized.  */
{
  if (! mode_entered)  return;
  describe_keys (current->keys.used, current->keys.data);
  if (current->redraw)  current->redraw ();
  doupdate ();
}

int
mode_keypress (game_time t, int meaning)
/* Feed a keypress with meaning MEANING to the current mode.
 * Return 1 if the keypress could be processed, and 0 else.  */
{
  int  i;

  if (meaning == mbk_redraw) {
    clear_windows ();
    mode_redraw ();
    return  1;
  }
  for (i=0; i<current->keys.used; ++i) {
    if (current->keys.data[i].meanings & meaning) {
      current->keypress (t, current->keys.data[i].res);
      return  1;
    }
  }
  return  0;
}

void
mode_signal (int signum)
/* Feed signal SIGNUM to the current mode.  */
{
  if (current->signal)  current->signal (signum);
}
