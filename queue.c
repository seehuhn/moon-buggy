/* queue.c - a queue of events to happen
 *
 * Copyright 1999  Jochen Voss  */

static const  char  rcsid[] = "$Id: queue.c,v 1.13 1999/05/11 21:58:13 voss Exp $";

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
  game_time  t;			/* time, when the event should happen */
  callback_fn  callback;	/* function to call */
  void *client_data;		/* argument to pass */
};
static  struct event *queue;

/**********************************************************************
 * recursive queues
 */

#define MAX_QUEUE_RECURSION 1

static  struct event *queue_buffer [MAX_QUEUE_RECURSION];
static  int  queue_recursion_depth = -1;

void
save_queue (void)
{
  assert (queue_recursion_depth >= 0);
  assert (queue_recursion_depth < MAX_QUEUE_RECURSION);
  queue_buffer [queue_recursion_depth] = queue;
  queue = NULL;
}

void
restore_queue (void)
{
  assert (queue_recursion_depth >= 0);
  queue = queue_buffer[queue_recursion_depth];
}

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
add_event (game_time t, callback_fn callback, void *client_data)
/* Add a new event for time T to the queue.
 * The event calls function CALLBACK with argument CLIENT_DATA.  */
{
  struct event **evp;
  struct event *ev;
  
  evp = &queue;
  while (*evp && (*evp)->t <= t)  evp = &((*evp)->next);

  ev = xmalloc (sizeof (struct event));
  ev->next = *evp;
  ev->t = t;
  ev->callback = callback;
  ev->client_data = client_data;

  *evp = ev;
}

void
remove_event (callback_fn callback)
/* Remove all events from the queue, which would call CALLBACK.  */
{
  struct event **evp;

  evp = &queue;
  while (*evp) {
    struct event *ev = *evp;
    if (ev->callback == callback) {
      *evp = (*evp)->next;
      free (ev);
    } else {
      evp = &((*evp)->next);
    }
  }
}

void
remove_client_data (void *client_data)
/* Remove all events from the queue, which refer to CLIENT_DATA.  */
{
  struct event **evp;

  evp = &queue;
  while (*evp) {
    struct event *ev = *evp;
    if (ev->client_data == client_data) {
      *evp = (*evp)->next;
      free (ev);
    } else {
      evp = &((*evp)->next);
    }
  }
}

/**********************************************************************
 * the main loop
 */

static  int  exit_flag;

void
quit_main_loop (void)
{
  assert (queue_recursion_depth >= 0);
  exit_flag = 1;
}

int
main_loop (double dt, void (*key_handler)(game_time))
{
  int  res;

  ++queue_recursion_depth;
  
  clock_adjust_delay (dt);
  doupdate ();

  exit_flag = 0;
  while (queue && ! exit_flag) {
    int  retval;
    double  t;
    
    t = to_real (queue->t);
    retval = wait_until (&t);

    if (retval>0 && key_handler)  key_handler (to_game (t));
    
    while (queue && queue->t <= to_game (vclock ())) {
      struct event *ev = queue;
      queue = queue->next;

      ev->callback (ev->t, ev->client_data);
      free (ev);
    }
    doupdate ();
  }
  res = (queue != NULL);
  clear_queue ();

  --queue_recursion_depth;
  exit_flag = 0;

  return  res;
}

/**********************************************************************
 * some handlers for the main loop above
 */

void
quit_main_loop_h (game_time t, void *client_data)
/* This function is a possible callback argument to `add_event'.
 * It causes the main loop to terminate.
 * The arguments T and CLIENT_DATA is ignored.  */
{
  quit_main_loop ();
}

void
clear_message_h (game_time t, void *client_data)
/* This function is a possible callback argument to `add_event'.
 * It causes the screen message area to be cleared.
 * The arguments T and CLIENT_DATA is ignored.  */
{
  werase (message);
  wnoutrefresh (message);
}
