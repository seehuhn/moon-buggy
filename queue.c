/* queue.c - a queue of events to happen
 *
 * Copyright (C) 1998  Jochen Voss.  */

static const  char  rcsid[] = "$Id: queue.c,v 1.1 1998/12/18 23:14:38 voss Exp $";

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <sys/types.h>
#include <sys/time.h>
#include <math.h>
#include <errno.h>
#include <assert.h>

#include "moon.h"


struct event {
  struct event *next;
  double  t;
  int  reached;
  enum event_type  type;
};

static  struct event *queue = NULL;
struct lagmeter *queuelag;


void
add_event (double t, enum event_type type)
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

static int
wait_until (double t)
/* Wait until the first one of time T and the next key press.
 * Return a positive value, if a key press occured, and 0 else.  */
{
  fd_set  rfds;
  int  retval;

  /* Watch stdin (fd 0) to see when it has input. */
  FD_ZERO(&rfds);
  FD_SET(0, &rfds);
  
  do {
    struct timeval  tv;
    double  dt, sec, usec;
    
    dt = t - vclock ();
    if (dt < 0)  dt = 0;
    
    usec = 1e6 * modf (dt, &sec) + 0.5;
    tv.tv_sec = sec + 0.5;
    tv.tv_usec = usec + 0.5;
    retval = select(FD_SETSIZE, &rfds, NULL, NULL, &tv);
  } while (retval<0 && errno == EINTR);

  if (retval < 0) {
    perror ("select failed");
  }
  
  return  retval;
}

enum event_type
get_event (double *t_return)
{
  struct event *ev;
  enum event_type  res;
  int  retval;

  assert (queue);
  
  if (! queue->reached) {
    double  t, correct;

    correct = get_lag (queuelag);
    retval = wait_until (queue->t - correct);
    t = vclock ();
    add_lag (queuelag, t - (queue->t - correct));

    ev = queue;
    while (ev && ev->t <= t) {
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
  *t_return = ev->t;
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
  *t = ev->t;
  free (ev);

  return  1;
}
