/* laser.c - a mining laser device for the moon-buggy
 *
 * Copyright (C) 1999  Jochen Voss.  */

static const  char  rcsid[] = "$Id: laser.c,v 1.3 1999/05/08 12:40:38 voss Exp $";

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "moon.h"
#include "darray.h"


enum beam_state { bs_START, bs_RUN, bs_CLOUD };

struct beam {
  enum beam_state  state;
  int  left, right, y;
  int  count;
};

static  struct {
  struct beam **data;
  int  slots, used;
} beam_table;


static void
beam_handler (game_time t, void *client_data)
{
  struct beam *b = client_data;
  int  i;

  switch (b->state) {
  case bs_START:
    wmove (moon, LINES-b->y, b->left);
    for (i=0; i<b->right-b->left; ++i)  waddch (moon, '-');
    b->state = bs_RUN;
    b->count = 40;
    add_event (t+TICK(0.25), beam_handler, client_data);
    break;
  case bs_RUN:
    if (b->count > 0) {
      b->left -= 1;
      b->right -= 1;
      mvwaddch (moon, LINES-b->y, b->left, '-');
      mvwaddch (moon, LINES-b->y, b->right, ' ');
      b->count -= 1;
      add_event (t+TICK(0.25), beam_handler, client_data);
    } else {
      wmove (moon, LINES-b->y, b->left);
      for (i=0; i<2; ++i)  waddch (moon, '*');
      for (i=2; i<b->right-b->left; ++i)  waddch (moon, ' ');
      b->right = b->left+2;
      b->state = bs_CLOUD;
      b->count = 3;
      add_event (t+TICK(1), beam_handler, client_data);
    }
    break;
  case bs_CLOUD:
    if (b->count > 1) {
      mvwaddch (moon, LINES-b->y, b->left, ' ');
      mvwaddch (moon, LINES-b->y, b->right, '*');
      b->left += 1;
      b->right += 1;
      b->count -= 1;
      add_event (t+TICK(1), beam_handler, client_data);
    } else if (b->count > 0) {
      mvwaddch (moon, LINES-b->y, b->left, ' ');
      b->left += 1;
      b->right += 1;
      for (i=b->left; i<b->right; ++i)  mvwaddch (moon, LINES-b->y, i, '.');
      b->count -= 1;
      add_event (t+TICK(1), beam_handler, client_data);
    } else {
      wmove (moon, LINES-b->y, b->left);
      for (i=0; i<2; ++i)  waddch (moon, ' ');
      DA_REMOVE_VALUE (beam_table, struct beam *, b);
      free (b);
    }
    break;
  }
  wnoutrefresh (moon);
}

void
fire_laser (double t)
{
  struct beam *b;

  if (! beam_table.data)  DA_INIT (beam_table, struct beam *);
  b = xmalloc (sizeof (struct beam));
  b->state = bs_START;
  b->left = car_x-8;
  b->right = car_x;
  b->y = car_y;
  DA_ADD (beam_table, struct beam *, b);
  add_event (t+TICK(0.25), beam_handler, b);
}

void
extinguish_laser (void)
/* Clear all laser beams from the screen.  */
{
  int  j;

  for (j=0; j<beam_table.used; ++j) {
    struct beam *b = beam_table.data[j];
    int  i;

    for (i=b->left; i<b->right; ++i)  mvwaddch (moon, LINES-b->y, i, ' ');
    free (b);
  }
  DA_CLEAR (beam_table);
  wnoutrefresh (moon);
}
