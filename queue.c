/* queue.c - a queue of events to happen
 *
 * Copyright 1999, 2000  Jochen Voss  */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef _XOPEN_SOURCE
#define _XOPEN_SOURCE_EXTENDED 1
#endif

#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif
#include <unistd.h>
#include <math.h>
#ifdef HAVE_ERRNO_H
#include <errno.h>
#else
extern  int  errno;
#endif
#include <assert.h>

#if defined(__hp9000s800)
#include <stdarg.h>
#endif

#include "moon-buggy.h"


/* The queue of events */
struct event {
  struct event *next;
  game_time  t;			/* time, when the event should happen */
  callback_fn  callback;	/* function to call */
  void *client_data;		/* argument to pass */
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

game_time
current_time (void)
{
  return  to_game (vclock ());
}

/**********************************************************************
 * wait for timeouts or keyboard input
 */

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

  if (handle_signals ())  return -1;
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

static void
wait_for_key (void)
{
  int  res;

  do {
    res = my_select (NULL);
  } while (res < 0);
}

static int
wait_until (game_time t, real_time *t_return)
/* Wait for time *T or the next key press, whichever comes first.
 * Return a positive value, if a key was pressed, and 0 else.
 * Set *T_RETURN to the return time.  */
{
  double  start;
  int  res;

  do {
    double  dt, sec, usec;
    struct timeval  tv;

    start = vclock ();
    dt = to_real(t) - start;
    if (dt <= 0) {
      *t_return = start;
      return  key_ready ();
    }

    usec = 1e6 * modf (dt, &sec) + 0.5;
    tv.tv_sec = sec + 0.5;
    tv.tv_usec = usec + 0.5;
    res = my_select (&tv);
  } while (res < 0);
  *t_return = vclock ();

  return  res;
}

static void
drain_input (void)
/* Discard all data from the input queue.  */
{
  char  buffer [16];
  int  oldflags;

  oldflags = fcntl (0, F_GETFL, 0);
  if (oldflags < 0) {
    fatal ("Cannot get file status flags (%s)", strerror (errno));
  }
  fcntl (0, F_SETFL, oldflags|O_NONBLOCK);
  while (read (0, buffer, 16) == 16)
    ;
  fcntl (0, F_SETFL, oldflags);
}

void
clock_reset (void)
/* Adjust the clock to make 0 the current time.  */
{
  time_base = vclock ();
}

static void
dummy_h (game_time t, void *client_data)
/* This function is a possible callback argument to `add_event'.
 * It does nothing.  */
{
  return;
}

void
clock_freeze (void)
/* Prepare to freeze the game's clock.
 * This function should be called before the game is resumed.
 * Afterwards you should use `clock_thaw ()' to restart the
 * game in the current state.  If the next event is less then 0.1
 * seconds in the future, we add some gap to allow for 0.1 seconds
 * pause after the restart.  */
{
  game_time  t;

  remove_event (dummy_h);
  t = current_time ();
  if (queue && t >= queue->t - 0.1)  t = queue->t - 0.1;
  add_event (t, dummy_h, NULL);
}

void
clock_thaw (void)
/* Adjust the clock to make the next event occur immediately.
 * The queue must contain at least one element.  */
{
  assert (queue);
  time_base = vclock () - queue->t;
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
  drain_input ();
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
  exit_flag = 1;
}

void
main_loop (void)
{
  clock_reset ();
  exit_flag = 0;

  while (! exit_flag) {
    int  retval;
    double  t;

    mode_update ();

    if (queue) {
      retval = wait_until (queue->t, &t);
    } else {
      wait_for_key ();
      t = vclock ();
      retval = 1;
    }

    if (retval>0) {
      int  meaning = read_key ();
      if (meaning != -1) {
        if (! mode_keypress (to_game (t), meaning))  beep ();
      }
    }

    while (queue && queue->t <= current_time ()) {
      struct event *ev = queue;
      queue = queue->next;

      ev->callback (ev->t, ev->client_data);
      free (ev);
    }
  }
}

/**********************************************************************
 * some handlers for the main loop above
 */

void
print_hint_h (game_time t, void *client_data)
/* This function is a possible callback argument to `add_event'.
 * It causes the level hint (const char *)CLIENT_DATA to be
 * displayed for 4 seconds.  */
{
  print_hint (client_data);
  remove_event (clear_hint_h);
  add_event (t+4, clear_hint_h, NULL);
}

void
clear_hint_h (game_time t, void *client_data)
/* This function is a possible callback argument to `add_event'.
 * It causes the screen's hint area to be cleared.
 * The arguments T and CLIENT_DATA are ignored.  */
{
  wmove (moon, LINES-11, 0);
  wclrtoeol (moon);
  wnoutrefresh (moon);
}
