/* main.c - moon-buggy main file
 *
 * Copyright (C) 1998  Jochen Voss.  */

static const  char  rcsid[] = "$Id: main.c,v 1.10 1998/12/30 19:39:08 voss Exp $";

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#if STDC_HEADERS
# include <string.h>
#endif
#ifdef HAVE_GETOPT_H
#include <getopt.h>
#else
#include <unistd.h>
extern char *optarg;
extern int  optind;
#endif
#include <signal.h>

#include "moon.h"


const char *my_name;
WINDOW *moon, *status, *message;
enum game_state  game_state = INIT;


void
print_message (const char *str)
{
  wclear (message);
  waddstr (message, str);
  wnoutrefresh (message);
}

static void
allocate_windows (void)
{
  moon = newwin (LINES-2, 0, 0, 0);
  keypad (moon, TRUE);
  leaveok (moon, TRUE);

  status = newwin (1, 0, LINES-2, 0);
  keypad (status, TRUE);
  leaveok (status, TRUE);

  message = newwin (1, 0, LINES-1, 0);
  keypad (message, TRUE);
  leaveok (message, TRUE);
}

void
prepare_for_exit (void)
{
  wclear (message);
  wnoutrefresh (moon);
  wnoutrefresh (status);
  wnoutrefresh (message);
  doupdate ();
  endwin ();
}

/************************************************************
 * signal handling
 */

static volatile  sig_atomic_t  termination_in_progress = 0;
static  sigset_t  winch_set, full_set, old_sigset;

void
block_winch (void)
{
  sigprocmask (SIG_BLOCK, &winch_set, &old_sigset);
}

void
block_all (void)
{
  sigprocmask (SIG_BLOCK, &full_set, &old_sigset);
}

void
unblock (void)
{
  sigprocmask (SIG_SETMASK, &old_sigset, NULL);
}
   
static RETSIGTYPE
termination_handler (int signum)
{
  signal (signum, SIG_DFL);
  if (termination_in_progress)  raise (signum);
  termination_in_progress = 1;
  prepare_for_exit ();
  fprintf (stderr, "GAME ABORTED (signal %d)\n", signum);
  raise (signum);
}

static RETSIGTYPE
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
    
static RETSIGTYPE
cont_handler (int signum)
{
  signal (SIGCONT, cont_handler);
  signal (SIGTSTP, tstp_handler);
  if (game_state == PLAYING)  print_message ("suspended (penalty: 10 points)");
  leaveok (moon, TRUE);
  leaveok (status, TRUE);
  leaveok (message, TRUE);

  wnoutrefresh (moon);
  wnoutrefresh (status);
  wnoutrefresh (message);
  doupdate ();
  if (game_state == PLAYING)  clock_adjust_delay (0.5);
}
    
static RETSIGTYPE
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
 * main procedure
 */

int
main (int argc, char **argv)
{
#ifdef HAVE_GETOPT_LONG
  struct option  long_options [] = {
    { "help", no_argument, 0, 'h'},
    { "no-title", no_argument, 0, 'n' },
    { "version", no_argument, 0, 'V'},
    { NULL, 0, NULL, 0}
  };
#endif
#define RND_SHORT_OPTIONS "hnV"
  int  help_flag = 0;
  int  title_flag = 1;
  int  version_flag = 0;
  int  error_flag = 0;
  int  res;
  
  initialize_persona ();
  set_user_persona ();
  my_name = xstrdup (basename (argv[0]));

  while (! error_flag) {
    int  c;
#ifdef HAVE_GETOPT_LONG
    int  ind;
    c = getopt_long (argc, argv, RND_SHORT_OPTIONS, long_options, &ind);
#else
    c = getopt (argc, argv, RND_SHORT_OPTIONS);
#endif
    if (c == -1)  break;
    switch (c) {
    case 'h':
      help_flag = 1;
      break;
    case 'n':
      title_flag = 0;
      break;
    case 'V':
      version_flag = 1;
      break;
    default:
      error_flag = 1;
    }
 }

  if (argc != optind) {
    fputs ("too many arguments\n", stderr);
    error_flag = 1;
  }
  
  if (version_flag) {
    puts (PACKAGE ", version " VERSION);
    if (! error_flag)  exit (0);
  }
  if (error_flag || help_flag) {
#ifdef HAVE_GETOPT_LONG
#define OPT(short,long) "  " short ", " long "    "
#else
#define OPT(short,long) "  " short "  "
#endif
    FILE *out = error_flag ? stderr : stdout;
    fprintf (out, "usage:  %s [options]\n\n", my_name);
    fputs ("The options are\n", out);
    fputs (OPT("-h","--help    ") "show this message and exit\n", out);
    fputs (OPT("-n","--no-title") "omit the title screen\n", out);
    fputs (OPT("-V","--version ") "show the version number and exit\n", out);
    exit (error_flag);
  }

  if (signal (SIGINT, termination_handler) == SIG_IGN)
    signal (SIGINT, SIG_IGN);
  if (signal (SIGHUP, termination_handler) == SIG_IGN)
    signal (SIGHUP, SIG_IGN);
  if (signal (SIGTERM, termination_handler) == SIG_IGN)
    signal (SIGTERM, SIG_IGN);
  signal (SIGCONT, cont_handler);
  signal (SIGTSTP, tstp_handler);
  signal (SIGWINCH, winch_handler);

  sigemptyset (&winch_set);
  sigfillset (&full_set);
  sigaddset (&winch_set, SIGWINCH);
  
  initscr ();
  cbreak ();
  noecho ();

  allocate_windows ();

  queuelag = new_circle_buffer ();

  if (title_flag) {
    res = title_mode ();
  } else {
    res = 0;
  }

  if (! res) {
    print_message ("good luck");
    while (game_mode ())
      ;
    mvwaddstr (moon, LINES-11, car_base-1, "GAME OVER");
  }
  
  prepare_for_exit ();
  return  0;
}
