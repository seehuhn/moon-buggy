/* queue.c - a queue of events to happen
 *
 * Copyright (C) 1998  Jochen Voss.  */

static const  char  rcsid[] = "$Id: queue.c,v 1.3 1998/12/26 12:13:00 voss Exp $";

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <math.h>
#include <errno.h>
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
struct lagmeter *queuelag;


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

enum event_type
get_event (double *t_return)
/* Wait until the next event happens and return it.
 * In *T_RETURN the specified event time is stored and the event's
 * type is returned.  */
{
  struct event *ev;
  enum event_type  res;
  int  retval;

  assert (queue);
  
  if (! queue->reached) {
    double  s, t, correct;

    correct = get_lag (queuelag);
    s = time_base + queue->t - correct;
    retval = wait_until (s);
    t = vclock ();
    if (t-s > 0.4) {
      clock_adjust_delay (0.4);
    } else {
      add_lag (queuelag, t - s);
    }

    ev = queue;
    while (ev && time_base+ev->t <= t) {
      ev->reached = 1;
      ev = ev->next;
    }

    if (retval>0) {
      *t_return = t;
      return  ev_KEY;
    }
  }

  /* deliver the event */
  ev = queue;
  queue = queue->next;
  *t_return = time_base + ev->t;
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
