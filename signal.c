/* signal.c - signal handling
 *
 * Copyright (C) 1999  Jochen Voss.  */

static const  char  rcsid[] = "$Id: signal.c,v 1.2 1999/04/23 22:18:07 voss Exp $";

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <signal.h>

#include "moon.h"


static  void (*handlers [NSIG]) (int);
static volatile  sig_atomic_t  pending [NSIG];
static volatile  sig_atomic_t  signal_arrived;

static  sigset_t  winch_set, full_set, old_sigset;


void
block_winch (void)
/* Block the WINCH signal until `unblock' is called.  */
{
  sigprocmask (SIG_BLOCK, &winch_set, &old_sigset);
}

void
block_all (void)
/* Block all signals until `unblock' is called.  */
{
  sigprocmask (SIG_BLOCK, &full_set, &old_sigset);
}

void
unblock (void)
/* Undo the effect of `block_winch' or `block_all'.  */
{
  sigprocmask (SIG_SETMASK, &old_sigset, NULL);
}

static RETSIGTYPE
generic_handler (int signum)
/* Interrupt handlers shouldn't do much.  So we just note that the
 * signal arrived.  */
{
  signal_arrived = 1;
  pending [signum] = 1;

  if (signum == SIGINT || signum == SIGHUP || signum == SIGTERM) {
    /* Pressing `C-c' twice exits, even if the program hangs.  */
    signal (signum, SIG_DFL);
  }
}

static void
my_signal (int signum, void (*handler)(int), int ignore_test)
{
  struct sigaction  action;

  handlers [signum] = handler;
  if (ignore_test) {
    sigaction (signum, NULL, &action);
    if (action.sa_handler == SIG_IGN)  return;
  }
  action.sa_handler = generic_handler;
  sigemptyset (&action.sa_mask);
  action.sa_flags = 0;
  sigaction (signum, &action, NULL);
}

/************************************************************
 * signal handlers
 */

static void
termination_handler (int signum)
{
  prepare_for_exit ();
  fprintf (stderr, "GAME ABORTED (signal %d)\n", signum);
  /* We did `signal (signum, SIG_DFL)' in `generic_handler'.  */
  raise (signum);
}

static void
tstp_handler (int signum)
{
  signal (SIGTSTP, SIG_DFL);
  if (game_state == PLAYING) {
    score_bonus (-10);
  }
  prepare_for_exit ();
  if (game_state == PLAYING)
    fputs ("suspended (penalty: 10 points)\n", stderr);
  raise (SIGTSTP);
}

static void
cont_handler (int signum)
{
  my_signal (SIGTSTP, tstp_handler, 0);
  if (game_state == PLAYING)  print_message ("suspended (penalty: 10 points)");

  cbreak ();
  noecho ();
  leaveok (moon, TRUE);
  leaveok (status, TRUE);
  leaveok (message, TRUE);

  wnoutrefresh (moon);
  wnoutrefresh (status);
  wnoutrefresh (message);
  doupdate ();
  if (game_state == PLAYING)  clock_adjust_delay (0.5);
}

static void
winch_handler (int signum)
{
  delwin (moon);
  delwin (status);
  delwin (message);
  endwin ();
  refresh ();
  allocate_windows ();
  
  switch (game_state) {
  case TITLE:
    resize_title ();
    break;
  case PAGER:
    resize_pager ();
    break;
  case PLAYING:
    resize_game ();
    break;
  case HIGHSCORE:
    resize_highscore ();
    break;
  default:
    break;
  }
}

/************************************************************
 * The outside-visible entry point
 */

void
initialise_signals (void)
{
  my_signal (SIGINT, termination_handler, 1);
  my_signal (SIGHUP, termination_handler, 1);
  my_signal (SIGTERM, termination_handler, 1);
  my_signal (SIGCONT, cont_handler, 0);
  my_signal (SIGTSTP, tstp_handler, 0);
#ifdef SIGWINCH
  my_signal (SIGWINCH, winch_handler, 0);
#endif
  
  sigemptyset (&winch_set);
#ifdef SIGWINCH
  sigaddset (&winch_set, SIGWINCH);
#endif
  sigfillset (&full_set);
}

void
handle_signals (void)
{
  int  i;
  
  if (! signal_arrived)  return;
  for (i=0; i<NSIG; ++i) {
    if (pending[i])  handlers[i] (i);
    pending[i] = 0;
  }
  signal_arrived = 0;
}
