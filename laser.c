/* laser.c - a mining laser device for the moon-buggy
 *
 * Copyright (C) 1999  Jochen Voss.  */

static const  char  rcsid[] = "$Id: laser.c,v 1.1 1999/04/24 17:48:30 voss Exp $";

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "moon.h"


enum beam_state { bs_START, bs_RUN, bs_CLOUD };

struct beam {
  enum beam_state  state;
  int  left, right;
  int  count;
};


static void
beam_handler (game_time t, void *client_data)
{
  struct beam *b = client_data;
  int  i;

  switch (b->state) {
  case bs_START:
    wmove (moon, LINES-5, b->left);
    for (i=0; i<=b->right-b->left; ++i)  waddch (moon, '-');
    b->state = bs_RUN;
    b->count = 40;
    add_event (t+TICK(0.5), beam_handler, client_data);
    break;
  case bs_RUN:
    if (b->count > 0) {
      mvwaddch (moon, LINES-5, b->right, ' ');
      b->left -= 1;
      b->right -= 1;
      mvwaddch (moon, LINES-5, b->left, '-');
      b->count -= 1;
      add_event (t+TICK(0.5), beam_handler, client_data);
    } else {
      wmove (moon, LINES-5, b->left);
      for (i=0; i<2; ++i)  waddch (moon, '*');
      for (i=2; i<=b->right-b->left; ++i)  waddch (moon, ' ');
      b->state = bs_CLOUD;
      add_event (t+TICK(8), beam_handler, client_data);
    }
    break;
  case bs_CLOUD:
    wmove (moon, LINES-5, b->left);
    for (i=0; i<2; ++i)  waddch (moon, ' ');
    free (b);
    break;
  }
}

void
fire_laser (double t)
{
  struct beam *b = xmalloc (sizeof (struct beam));
  b->state = bs_START;
  b->left = car_base-speed-8;
  b->right = car_base-speed-1;
  add_event (t+TICK(0.5), beam_handler, b);
}
