/* queue.c - a queue of events to happen
 *
 * Copyright 1999  Jochen Voss  */

static const  char  rcsid[] = "$Id: queue.c,v 1.11 1999/04/07 15:26:36 voss Exp $";

#define _POSIX_SOURCE 1

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#if HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif
#include <sys/time.h>
#include <math.h>
#if HAVE_ERRNO_H
#include <errno.h>
#else
extern  int  errno;
#endif
#include <assert.h>

#include "moon.h"


/* The queue of events */
struct event {
  struct event *next;
  game_time  t;			/* the time, when the event will happen */
  int  reached;			/* true, iff we know this time to be past */
  enum event_type  type;	/* the event's type */
};
static  struct event *queue;

/**********************************************************************
 * convert between game time and real time
 */

typedef  double  real_time;

/* The type `game_time' is relative to `time_base'.  */
static  double  time_base;

static real_time
to_real (game_time t)
{
  return  t + time_base;
}

static game_time
to_game (real_time t)
{
  return  t - time_base;
}

/**********************************************************************
 * wait for timeouts or keyboard input
 */

/* These are used to measure the system's load.  */
static  double  time_slept;
static  game_time  sleep_base;

static int
my_select (struct timeval *timeout)
/* Wait until input is ready on from stdin or a timeout is reached.
 * TIMEOUT has the same meaning, as is has for the `select' system
 * call.  The return value is 0 if we return because of a timeout, and
 * positive when input is ready.  */
{
  fd_set  rfds;
  int  retval;

  /* Watch stdin (fd 0) to see when it has input. */
  FD_ZERO (&rfds);
  FD_SET (0, &rfds);

  handle_signals ();
  do {
    retval = select (FD_SETSIZE, &rfds, NULL, NULL, timeout);
    if (retval < 0 && errno == EINTR)  handle_signals ();
  } while (retval < 0 && errno == EINTR);
  if (retval < 0)  fatal ("Select failed: %s", strerror (errno));

  return  retval;
}

static int
key_ready (void)
/* Return a positive value iff keyboard input is ready.  */
{
  struct timeval  ancient_time;

  ancient_time.tv_sec = 0;
  ancient_time.tv_usec = 0;
  return  my_select (&ancient_time);
}

static void
wait_for_key (void)
/* Wait for the next key press.
 * The key is not removed from the input buffer and must be read by a
 * function like `xgetch'.  */
{
  my_select (NULL);
}

static int
wait_until (real_time *t)
/* Wait for time *T or the next key press, whichever comes first.
 * Return a positive value, if a key was pressed, and 0 else.
 * Set *T to the return time.  */
{
  double  start, stop, dt;
  double  sec, usec;
  struct timeval  tv;
  int  retval;

  start = vclock ();
  dt = *t - start;
  if (dt <= 0) {
    *t = start;
    return  key_ready ();
  }
    
  usec = 1e6 * modf (dt, &sec) + 0.5;
  tv.tv_sec = sec + 0.5;
  tv.tv_usec = usec + 0.5;
  retval = my_select (&tv);

  *t = stop = vclock ();
  time_slept += stop-start;
  
  return  retval;
}

void
clock_adjust_delay (double dt)
/* Adjust the clock to make the next event occur after DT steps of time.
 * The queue must contain at least one element.
 * This function MUST be called before the first `get_event' call.  */
{
  double  now;
  
  assert (queue);
  
  now = vclock ();
  time_base = now + dt - queue->t;
  
  time_slept = 0;
  sleep_base = to_game (now);
}

void
clear_queue (void)
/* Remove all events from the queue.  */
{
  struct event *ev;

  ev = queue, queue = NULL;
  while (ev) {
    struct event *old = ev;
    ev = old->next;
    free (old);
  }
  while (key_ready ())  xgetch (moon);
}

void
add_event (game_time t, enum event_type type)
/* Add a new event to the queue.
 * The event happens at time T and is of type TYPE.  */
{
  struct event **evp;
  struct event *ev;

  assert (type != ev_KEY);
  
  evp = &queue;
  while (*evp && (*evp)->t <= t)  evp = &((*evp)->next);

  ev = xmalloc (sizeof (struct event));
  ev->next = *evp;
  ev->t = t;
  ev->reached = 0;
  ev->type = type;

  *evp = ev;
}

enum event_type
get_event (game_time *t_return)
/* Wait until the next event happens and return it.
 * The specified event time is stored in *T_RETURN and the return
 * value is the event's type.  */
{
  struct event *ev;
  enum event_type  res;
  int  retval;
  
  if (! (queue && queue->reached)) {
    double  t;
    
    if (queue) {
      t = to_real (queue->t);
      retval = wait_until (&t);

      ev = queue;
      while (ev && ev->t <= to_game (t)) {
	ev->reached = 1;
	ev = ev->next;
      }
    } else {
      wait_for_key ();
      t = vclock ();
      retval = 1;
    }

    if (retval>0) {		/* key pressed */
      if (t_return)  *t_return = to_game (t);
      return  ev_KEY;
    }
  }

  /* deliver the event */
  ev = queue;
  queue = queue->next;
  if (t_return)  *t_return = ev->t;
  res = ev->type;
  free (ev);
  
  return  res;
}

int
remove_event (enum event_type type, game_time *t_return)
{
  struct event **evp;
  struct event *ev;
  
  evp = &queue;
  while (*evp && (*evp)->type != type)  evp = &((*evp)->next);
  if (! *evp)  return 0;
  
  ev = *evp;
  *evp = (*evp)->next;
  *t_return = ev->t;
  free (ev);
  return  1;
}
