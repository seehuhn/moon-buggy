/* queue.c - a queue of events to happen
 *
 * Copyright 1999  Jochen Voss  */

static const  char  rcsid[] = "$Id: queue.c,v 1.25 1999/06/13 19:18:12 voss Rel $";

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#define _XOPEN_SOURCE_EXTENDED 1

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
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

#if defined(__hp9000s800)
#include <stdarg.h>
#endif

#include "mbuggy.h"


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
static  double  time_slept, sleep_base;
static  double  cpu_load = 0;
#define ALPHA 2.0

static int
my_select (struct timeval *timeout)
/* Wait until input is ready on stdin or a timeout is reached.
 * TIMEOUT has the same meaning, as is has for the `select' system
 * call.  The return value is 0 if we return because of a timeout,
 * positive if input is ready, and negative if a signal occured.  In
 * the latter case we must calculate a new TIMEOUT value and call
 * `my_select' again.  */
{
  fd_set  rfds;
  int  res;

  /* Watch stdin (fd 0) to see when it has input. */
  FD_ZERO (&rfds);
  FD_SET (0, &rfds);

  handle_signals ();
  res = select (FD_SETSIZE, &rfds, NULL, NULL, timeout);
  if (res < 0) {
    if (errno == EINTR) {
      handle_signals ();
    } else {
      fatal ("Select failed: %s", strerror (errno));
    }
  }
  return  res;
}

static int
key_ready (void)
/* Return a positive value iff keyboard input is ready.  */
{
  int  res;

  do {
    struct timeval  ancient_time;
    
    ancient_time.tv_sec = 0;
    ancient_time.tv_usec = 0;
    res = my_select (&ancient_time);
  } while (res < 0);
  return  res;
}

static int
wait_until (real_time *t)
/* Wait for time *T or the next key press, whichever comes first.
 * Return a positive value, if a key was pressed, and 0 else.
 * Set *T to the return time.  */
{
  double  start, stop;
  int  res;

  do {
    double  dt, sec, usec;
    struct timeval  tv;
    
    start = vclock ();
    dt = *t - start;
    if (dt <= 0) {
      *t = start;
      return  key_ready ();
    }
    
    usec = 1e6 * modf (dt, &sec) + 0.5;
    tv.tv_sec = sec + 0.5;
    tv.tv_usec = usec + 0.5;
    res = my_select (&tv);
  } while (res < 0);

  *t = stop = vclock ();
  time_slept += stop-start;
  if (! res && stop-sleep_base > 0.1) {
    double  q = exp (-ALPHA*(stop-sleep_base));
    cpu_load = q*cpu_load + (1-q)*(1-time_slept/(stop-sleep_base));
    time_slept = 0;
    sleep_base = stop;
  }
  
  return  res;
}

void
clock_adjust_delay (double dt)
/* Adjust the clock to make the next event occur after DT steps of time.
 * The queue must contain at least one element.
 * This function MUST be called before the first `get_event' call.  */
{
  double  now;
  
  if (! queue)  return;
  
  now = vclock ();
  time_base = now + dt - queue->t;
  
  time_slept = 0;
  sleep_base = now;
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
  while (key_ready ()) {	/* eat up input */
    char  c;
    read (0, &c, 1);
  }
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

static void
dummy_h (game_time t, void *client_data)
/* This function is a possible callback argument to `add_event'.
 * It does nothing.  */
{
  return;
}

void
fix_game_time (void)
/* Add a do-nothing event to the queue's head.
 * This function should be called before the game is resumed.
 * Afterwards you should use `clock_adjust_delay (0)' to restart the
 * game in the current state.  If the next event is less then 0.5
 * seconds in the future, we add some gap to allow for 0.5 seconds
 * pause after the restart.  */
{
  game_time  t;

  if (! queue)  return;
  t = to_game (vclock ());
  if (t >= queue->t - 0.5)  t = queue->t - 0.5;
  add_event (t, dummy_h, NULL);
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
#if 0				/* TODO */
    mvwprintw (status, 0, 3, "load:%5.1f%%", cpu_load*100);
    wnoutrefresh (status);
#endif
    doupdate ();
  }
  res = (queue != NULL);
  clear_queue ();

  --queue_recursion_depth;
  exit_flag = 0;

  return  res;
}

void
xsleep (double dt)
/* Sleep of DT seconds.  */
{
  add_event (0, quit_main_loop_h, NULL);
  main_loop (dt, NULL);
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
print_message_h (game_time t, void *client_data)
/* This function is a possible callback argument to `add_event'.
 * It causes the message (const char *)CLIENT_DATA to be displayed
 * for 6 seconds.  */
{
  print_message (client_data);
  remove_event (clear_message_h);
  add_event (t+6, clear_message_h, NULL);
}

void
clear_message_h (game_time t, void *client_data)
/* This function is a possible callback argument to `add_event'.
 * It causes the screen's message area to be cleared.
 * The arguments T and CLIENT_DATA is ignored.  */
{
  werase (message);
  wnoutrefresh (message);
}
