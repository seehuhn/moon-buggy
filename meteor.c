/* meteor.c - stones on the moon to shoot on
 *
 * Copyright (C) 1999  Jochen Voss.  */

static const  char  rcsid[] = "$Id: meteor.c,v 1.2 1999/05/22 13:43:58 voss Exp $";

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "mbuggy.h"
#include "darray.h"


enum meteor_state { ms_START, ms_BIG, ms_MEDIUM, ms_SMALL };
static char  m_image [] = { 'O', 'O', 'o', ',' };

struct meteor {
  enum meteor_state  state;
  int  x;
};

static  struct {
  struct meteor **data;
  int  slots, used;
} meteor_table;


static void
meteor_handler (game_time t, void *client_data)
/* Move a meteor along with the ground.  Check for laser hits. */
{
  struct meteor *m = client_data;

  switch (m->state) {
  case ms_START:
    mvwaddch (moon, BASELINE, m->x, m_image[m->state]);
    m->state = ms_BIG;
    add_event (t+TICK(1), meteor_handler, m);
    break;
  default:
    mvwaddch (moon, BASELINE, m->x, ' ');
    if (m->x < COLS-1 ) {
      m->x += 1;
      if (laser_hit (m->x)) {
	m->state += 1;
	if (m->state > ms_SMALL) {
	  DA_REMOVE_VALUE (meteor_table, struct meteor *, m);
	  free (m);
	} else {
	  mvwaddch (moon, BASELINE, m->x, m_image[m->state]);
	  add_event (t+TICK(1), meteor_handler, m);
	}
      } else {
	mvwaddch (moon, BASELINE, m->x, m_image[m->state]);
	add_event (t+TICK(1), meteor_handler, m);
      }
    } else {
      DA_REMOVE_VALUE (meteor_table, struct meteor *, m);
      free (m);
    }
    break;
  }
  wnoutrefresh (moon);
}

void
place_meteor (double t)
/* Place a new meteor on the ground.
 * This is the only function, that must be called from the outside.
 * The complete handling of the meteor results automatically.  */
{
  struct meteor *m;

  if (! meteor_table.data)  DA_INIT (meteor_table, struct meteor *);
  m = xmalloc (sizeof (struct meteor));
  m->state = ms_START;
  m->x = 0;
  DA_ADD (meteor_table, struct meteor *, m);
  add_event (t+TICK(1), meteor_handler, m);
}

void
remove_meteors (void)
/* Remove all meteors from the ground.
 * Free any resources used by the internal representation.  */
{
  int  j;

  for (j=0; j<meteor_table.used; ++j) {
    struct meteor *m = meteor_table.data[j];
    mvwaddch (moon, BASELINE, m->x, ' ');
    free (m);
  }
  DA_CLEAR (meteor_table);
  wnoutrefresh (moon);
}

int
meteor_hit (int x0, int x1)
/* Check for meteors at positions >=x0 and <x1.
 * All these are hit by the laser.
 * Return true, if there are any hits.  */
{
  int  j;
  int  res = 0;

  for (j=0; j<meteor_table.used; ++j) {
    struct meteor *m = meteor_table.data[j];
    if (m->x >= x0 && m->x < x1) {
      m->state += 1;
      wnoutrefresh (moon);
      if (m->state > ms_SMALL) {
	mvwaddch (moon, BASELINE, m->x, ' ');
	remove_client_data (m);
	DA_REMOVE_VALUE (meteor_table, struct meteor *, m);
	free (m);
      } else {
	mvwaddch (moon, BASELINE, m->x, m_image[m->state]);
      }
      res = 1;
    }
  }
  return  res;
}
