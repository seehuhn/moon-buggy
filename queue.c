/* queue.c - a queue of events to happen
 *
 * Copyright (C) 1998  Jochen Voss.  */

static const  char  rcsid[] = "$Id: queue.c,v 1.6 1999/01/01 17:58:49 voss Exp $";

#define _POSIX_SOURCE

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
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
  double  t;
  int  reached;
  enum event_type  type;
};
static  struct event *queue = NULL;

/* All event times are relative to `time_base'.  */
double  time_base = 0;

/* This measures the lag introduced by the select call.  */
struct circle_buffer *queuelag;

/* The sum of sleeping times */
double  sleep_meter = 0;


static int
key_ready (void)
/* Return a positive value iff keyboard input is ready.  */
{
  fd_set  rfds;
  int  retval;

  /* Watch stdin (fd 0) to see when it has input. */
  FD_ZERO (&rfds);
  FD_SET (0, &rfds);
  
  do {
    struct timeval  tv;
    
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    retval = select (FD_SETSIZE, &rfds, NULL, NULL, &tv);
  } while (retval<0 && errno == EINTR);
  if (retval < 0) {
    fatal ("Select failed: %s", strerror (errno));
  }
  
  return  retval;
}

static int
wait_for_key (void)
/* Wait for the next key press and return a positive value.  */
{
  fd_set  rfds;
  int  retval;

  /* Watch stdin (fd 0) to see when it has input. */
  FD_ZERO (&rfds);
  FD_SET (0, &rfds);
  
  do {
    retval = select (FD_SETSIZE, &rfds, NULL, NULL, NULL);
  } while (retval<0 && errno == EINTR);
  if (retval < 0) {
    fatal ("Select failed: %s", strerror (errno));
  }
  
  return  retval;
}

static int
wait_until (double t)
/* Wait for time T or the next key press, whichever comes first.
 * Return a positive value, if a key was pressed, and 0 else.  */
{
  fd_set  rfds;
  int  retval;

  /* Watch stdin (fd 0) to see when it has input. */
  FD_ZERO (&rfds);
  FD_SET (0, &rfds);
  
  do {
    struct timeval  tv;
    double  dt, sec, usec;
    
    dt = t - vclock ();
    if (dt < 0)  dt = 0;
    sleep_meter += dt;
    
    usec = 1e6 * modf (dt, &sec) + 0.5;
    tv.tv_sec = sec + 0.5;
    tv.tv_usec = usec + 0.5;
    retval = select (FD_SETSIZE, &rfds, NULL, NULL, &tv);
  } while (retval<0 && errno == EINTR);

  if (retval < 0) {
    fatal ("Select failed: %s", strerror (errno));
  }
  
  return  retval;
}

void
clock_adjust_delay (double dt)
/* Make the next event occur after DT time steps.  */
{
  double  t = vclock ();
  double  new_time_base = t + dt - queue->t;
  if (new_time_base > time_base)  time_base = new_time_base;
}

void
clear_queue (void)
/* Remove all events from the queue.  */
{
  struct event *ev = queue;
  while (ev) {
    struct event *old = ev;
    ev = old->next;
    free (old);
  }
  queue = NULL;

  while (key_ready ())  wgetch (moon);
}

void
add_event (double t, enum event_type type)
/* Add a new event to the queue.
 * The event happens at time T and is of type TYPE.  */
{
  struct event **evp;
  struct event *ev;

  assert (type != ev_KEY);

  t -= time_base;
  
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
get_event (double *t_return)
/* Wait until the next event happens and return it.
 * In *T_RETURN the specified event time is stored and the event's
 * type is returned.  */
{
  struct event *ev;
  enum event_type  res;
  int  retval;
  
  if (! (queue && queue->reached)) {
    double  t;
    
    if (queue) {
      double  s, correct;
      
      correct = get_mean (queuelag);
      s = time_base + queue->t - correct;
      retval = wait_until (s);
      t = vclock ();
      if (t-s > 0.4) {
	clock_adjust_delay (0.4);
      } else {
	add_value (queuelag, t - s);
      }
    
      ev = queue;
      while (ev && time_base+ev->t <= t) {
	ev->reached = 1;
	ev = ev->next;
      }
    } else {
      retval = wait_for_key ();
      t = vclock ();
    }

    if (retval>0) {
      if (t_return)  *t_return = t;
      return  ev_KEY;
    }
  }

  /* deliver the event */
  ev = queue;
  queue = queue->next;
  if (t_return)  *t_return = time_base + ev->t;
  res = ev->type;
  free (ev);
  return  res;
}

int
remove_event (enum event_type type, double *t)
{
  struct event **evp;
  struct event *ev;
  
  evp = &queue;
  while (*evp && (*evp)->type != type)  evp = &((*evp)->next);
  if (! *evp)  return 0;
  
  ev = *evp;
  *evp = (*evp)->next;
  *t = time_base + ev->t;
  free (ev);

  return  1;
}
