/* signal.c - signal handling
 *
 * Copyright 1999  Jochen Voss.  */

static const  char  rcsid[] = "$Id: signal.c,v 1.11 1999/07/21 10:38:32 voss Exp $";

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <signal.h>
#include <assert.h>

#include "moon-buggy.h"
#include "darray.h"


struct sig_info {
  int  signum;
  volatile  sig_atomic_t  pending;
  void (*handler) (int);
};
static struct {
  struct sig_info *data;
  int  slots, used;
} sig_info_table;

static volatile  sig_atomic_t  signal_arrived;

static  sigset_t  full_set, old_sigset;


void
block_all (void)
/* Block all signals until `unblock' is called.  */
{
  sigprocmask (SIG_BLOCK, &full_set, &old_sigset);
}

void
unblock (void)
/* Undo the effect of `block_all'.  */
{
  sigprocmask (SIG_SETMASK, &old_sigset, NULL);
}

static RETSIGTYPE
generic_handler (int signum)
/* Interrupt handlers shouldn't do much.  So we just note that the
 * signal arrived.  */
{
  int  i;
  
  signal_arrived = 1;
  for (i=0; i<sig_info_table.used; ++i) {
    if (sig_info_table.data[i].signum == signum)  break;
  }
  assert (i<sig_info_table.used);
  sig_info_table.data[i].pending = 1;

  if (signum == SIGINT || signum == SIGHUP || signum == SIGTERM) {
    /* Pressing `C-c' twice exits, even if the program hangs.  */
    signal (signum, SIG_DFL);
  }
}

static void
my_signal (int signum, void (*handler)(int), int ignore_test)
/* Install HANDLER as a new signal handler for signal SIGNUM.  But if
 * IGNORE_TEST is true and SIGNUM was ignored before, do nothing.  */
{
  struct sig_info *info;
  struct sigaction  action;

  if (ignore_test) {
    sigaction (signum, NULL, &action);
    if (action.sa_handler == SIG_IGN)  return;
  }

  DA_ADD_EMPTY (sig_info_table, struct sig_info, info);
  info->signum = signum;
  info->pending = 0;
  info->handler = handler;

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
  fix_game_time ();
  if (game_state == PLAYING)  quit_game ();
  prepare_for_exit ();
  signal (SIGTSTP, SIG_DFL);
  raise (SIGTSTP);
}

static void
do_resize ()
{
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

static void
cont_handler (int signum)
{
  my_signal (SIGTSTP, tstp_handler, 0);
  if (game_state == PLAYING) {
    print_message ("GAME OVER (suspended)");
  }

  refresh ();
  cbreak ();
  noecho ();
  leaveok (moon, TRUE);
  leaveok (status, TRUE);
  leaveok (message, TRUE);
  do_resize ();
  clock_adjust_delay (0);
}

static void
winch_handler (int signum)
{
  fix_game_time ();
  delwin (moon);
  delwin (status);
  delwin (message);
  endwin ();
  refresh ();
  clearok (curscr, TRUE);
  allocate_windows ();
  do_resize ();
  clock_adjust_delay (1);	/* wait some extra time */
}

/************************************************************
 * The outside-visible entry point
 */

void
initialise_signals (void)
{
  DA_INIT (sig_info_table, struct sig_info);
  my_signal (SIGINT, termination_handler, 1);
  my_signal (SIGHUP, termination_handler, 1);
  my_signal (SIGTERM, termination_handler, 1);
  my_signal (SIGCONT, cont_handler, 0);
  my_signal (SIGTSTP, tstp_handler, 0);
#ifdef SIGWINCH
  my_signal (SIGWINCH, winch_handler, 0);
#endif
  
  sigfillset (&full_set);
}

void
handle_signals (void)
{
  while (signal_arrived) {
    int  i;

    signal_arrived = 0;
    for (i=0; i<sig_info_table.used; ++i) {
      if (sig_info_table.data[i].pending) {
	sig_info_table.data[i].pending = 0;	
	sig_info_table.data[i].handler (sig_info_table.data[i].signum);
      }
    }
  }
}
