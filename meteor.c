/* meteor.c - stones on the moon to shoot at
 *
 * Copyright 1999, 2000  Jochen Voss.  */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "moon-buggy.h"
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
score_meteor (struct meteor *m)
{
  adjust_score (13);
  bonus[m->x] -= 20;
}

static int
scroll_one_meteor (struct meteor *m)
/* Move the meteor *M along with the ground.
 * Check for collisions with the car or with laser beams.
 * Return 1 iff the meteor should be removed from the list.  */
{
  if (m->state == ms_START) {
    m->state = ms_BIG;
  } else {
    mvwaddch (moon, BASELINE, m->x, ' ');
  }

  m->x += 1;
  if (m->x >= COLS )  return 1;

  if (laser_hit (m->x)) {
    m->state += 1;
    if (m->state > ms_SMALL) {
      score_meteor (m);
      return  1;
    }
  }
  if (car_meteor_hit (m->x))  return  1;

  mvwaddch (moon, BASELINE, m->x, m_image[m->state]);
  return  0;
}

void
scroll_meteors (void)
/* Move the meteors along with the ground.
 * Handle collisions with the car or with laser beams. */
{
  int  j;

  if (meteor_table.used > 0)  wnoutrefresh (moon);
  for (j=meteor_table.used-1; j>=0; --j) {
    struct meteor *m = meteor_table.data[j];
    int  res;

    res = scroll_one_meteor (m);
    if (res) {
      DA_REMOVE (meteor_table, struct meteor *, j);
      free (m);
    }
  }
}

void
place_meteor (void)
/* Place a new meteor on the ground.  */
{
  struct meteor *m;

  if (! meteor_table.data)  DA_INIT (meteor_table, struct meteor *);
  m = xmalloc (sizeof (struct meteor));
  m->state = ms_START;
  m->x = 0;
  bonus[0] += 20;
  DA_ADD (meteor_table, struct meteor *, m);
}

void
remove_meteors (void)
/* Remove all meteors from the ground.
 * Free any resources used by the internal representation.  */
{
  int  j;

  if (meteor_table.used > 0)  wnoutrefresh (moon);
  for (j=0; j<meteor_table.used; ++j) {
    struct meteor *m = meteor_table.data[j];
    mvwaddch (moon, BASELINE, m->x, ' ');
    free (m);
  }
  DA_CLEAR (meteor_table);
}

int
meteor_laser_hit (int x0, int x1)
/* Check for meteors at positions >=x0 and <x1.
 * All these are hit by the laser.
 * Return true, if there are any hits.  */
{
  int  j;

  for (j=0; j<meteor_table.used; ++j) {
    struct meteor *m = meteor_table.data[j];
    if (m->x >= x0 && m->x < x1) {
      int  x = m->x;

      m->state += 1;
      wnoutrefresh (moon);
      if (m->state > ms_SMALL) {
        mvwaddch (moon, BASELINE, m->x, ' ');
        score_meteor (m);
        remove_client_data (m);
        DA_REMOVE_VALUE (meteor_table, struct meteor *, m);
        free (m);
      } else {
        mvwaddch (moon, BASELINE, m->x, m_image[m->state]);
      }
      return  x;
    }
  }
  return  0;
}

int
meteor_car_hit (int x0, int x1)
/* Check for meteors at positions >=x0 and <x1.
 * All these are destroyed by the landing car.
 * Return true, if there are any hits.  */
{
  int  j;
  int  res = 0;

  for (j=meteor_table.used-1; j>=0; --j) {
    struct meteor *m = meteor_table.data[j];
    if (m->x >= x0 && m->x < x1) {
      mvwaddch (moon, BASELINE, m->x, ' ');
      remove_client_data (m);
      DA_REMOVE_VALUE (meteor_table, struct meteor *, m);
      free (m);
      res = 1;
    }
  }
  if (res)  wnoutrefresh (moon);
  return  res;
}

void
resize_meteors (void)
/* Silently remove all meteors, which are no longer visible.  */
{
  int  j;

  j = 0;
  while (j<meteor_table.used) {
    struct meteor *m = meteor_table.data[j];
    if (m->x >= COLS) {
      remove_client_data (m);
      DA_REMOVE_VALUE (meteor_table, struct meteor *, m);
      free (m);
    } else {
      ++j;
    }
  }
}
