/* mode.c - handle the program's modes in a consistent way
 *
 * Copyright (C) 2000  Jochen Voss.  */

static const  char  rcsid[] = "$Id: mode.c,v 1.6 2000/06/01 18:49:07 voss Exp $";


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>

#include "moon-buggy.h"
#include "darray.h"


static const  struct mode *current;
static  int  mode_entered, mode_seed;


static void
mode_enter (void)
{
  werase (moon);
  wnoutrefresh (moon);
  describe_keys (current->keys.used, current->keys.data);
  current->enter (mode_seed);
  mode_entered = 1;
}

void
mode_update (void)
{
  if (! mode_entered) {
    clear_queue ();
    mode_enter ();
  }
  doupdate ();
}

struct mode *
new_mode (void)
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
{
  struct binding *keys;

  DA_ADD_EMPTY (m->keys, struct binding, keys);
  keys->meanings = meanings;
  keys->desc = desc;
  keys->res = res;
}

void
mode_start (const struct mode *m, int seed)
{
  wclear (status);
  wnoutrefresh (status);
  wclear (message);
  wnoutrefresh (message);

  current = m;
  mode_entered = 0;
  mode_seed = seed;
}

void
mode_change (const struct mode *m, int seed)
{
  if (current->leave && mode_entered)  current->leave ();

  current = m;
  mode_entered = 0;
  mode_seed = seed;
}

void
mode_leave (void)
{
  if (current->leave && mode_entered)  current->leave ();
  current = NULL;
}

void
mode_redraw (void)
{
  if (! mode_entered)  return;
  wclear (moon);
  wnoutrefresh (moon);
  wclear (status);
  wnoutrefresh (status);
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
{
  if (current->signal)  current->signal (signum);
}

void
mode_keys (void)
/* Explain the mode's key bindings.  */
{
  describe_keys (current->keys.used, current->keys.data);
}
