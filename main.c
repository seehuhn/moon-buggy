/* main.c - moon-buggy main file
 *
 * Copyright (C) 1998  Jochen Voss.  */

static const  char  rcsid[] = "$Id: main.c,v 1.8 1998/12/27 14:09:53 voss Exp $";

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
extern int optind, opterr, optopt;
#endif
#include <signal.h>

#include "moon.h"


const char *my_name;

long  score, bonus;
static int  lives;

static void
print_score ()
{
  mvwprintw (status, 0, car_base-7, "score: %-8ld", score);
  wnoutrefresh (status);
}

static void
print_lives ()
{
  mvwprintw (status, 0, car_base-20, "lives: %d", lives);
  wnoutrefresh (status);
}

static void
print_ground ()
{
  mvwaddchnstr (moon, LINES-4, 0, ground2, COLS);
  wnoutrefresh (moon);
}

static void
print_message (const char *str)
{
  mvwaddstr (status, 1, 0, str);
  wclrtoeol (status);
  wnoutrefresh (status);
}

void
prepare_for_exit (void)
{
  wmove (status, 1, 0);
  wclrtoeol (status);
  wnoutrefresh (moon);
  wnoutrefresh (status);
  doupdate ();
  endwin ();
}

/**********************************************************************
 * game control
 */

static void
spend_life (void)
{
  double  base, t;
  int  done = 0;

  bonus = 0;
  print_ground ();
  print_score ();
  print_lives ();

  clear_queue ();

  initialize_buggy ();
  print_buggy ();
  
  doupdate ();
  
  base = vclock ();
  add_event (base+1, ev_SCROLL);
  add_event (base+3, ev_STATUS);
  add_event (base+6, ev_SCORE);

  do {
    switch (get_event (&t)) {
    case ev_SCROLL:
      scroll_ground ();
      print_ground ();
      if (ground2[score_base] == ' ')  ++bonus;
      if (crash_check ())  done = 1;
      add_event (t+0.08, ev_SCROLL);
      break;
    case ev_KEY:
      switch (wgetch (moon)) {
      case ' ':
	if (can_jump()) {
	  jump (t);
	  ++score;
	  print_score ();
	}
	break;
      case KEY_BREAK:
      case KEY_CLOSE:
      case '\e':
      case 'q':
	done = 1;
	lives = 1;
	break;
      }
      break;
    case ev_STATUS:
      wmove (status, 1, 0);
      wclrtoeol (status);
      wnoutrefresh (status);
      break;
    case ev_BUGGY:
      print_buggy ();
      break;
    case ev_SCORE:
      ++score;
      print_score ();
      add_event (t+1, ev_SCORE);
      break;
    }
    doupdate ();
  } while (! done);
}

static void
play_game ()
{
  int  i;
  
  wclear (moon);
  wmove (status, 0, 0);
  wclrtoeol (status);
  
  for (i=0; i<COLS; ++i) {
    ground1[i] = '#';
    ground2[i] = '#';
  }
  mvwaddchnstr (moon, LINES-3, 0, ground1, COLS);
  car_base = (COLS > 80 ? 80 : COLS) - 12;
  score_base = car_base + 7;

  score = 0;
  lives = 3;
  do {
    spend_life ();
    --lives;
    if (lives > 0) {
      sleep (1);
      for (i=car_base-4; i<car_base+8; ++i) {
	ground2[i] = 'X';
      }
    }
  } while (lives > 0);

  wattron (moon, A_BLINK);
  mvwaddstr (moon, LINES-11, car_base-1, "GAME OVER");
  wattroff (moon, A_BLINK);

  print_lives ();
  write_hiscore ();
}

static void
main_loop (void)
{
  int  done = 0;
  
  do {
    int  c;
    play_game ();
  ask:
    print_message ("play again (y/n)?");
    doupdate ();
    c = wgetch (status);
    switch (c) {
    case 'y':
    case 'Y':
      break;
    case 'n':
    case 'N':
    case 'q':
    case 'Q':
      done = 1;
      break;
    default:
      beep ();
      goto ask;
    }
  } while (! done);
}

static volatile  sig_atomic_t  termination_in_progress = 0;
     
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
  score -= 10;
  print_score ();
  prepare_for_exit ();
  fputs ("suspended (penalty: 10 points)\n", stderr);
  raise (SIGTSTP);
}
    
static RETSIGTYPE
cont_handler (int signum)
{
  signal (SIGCONT, cont_handler);
  signal (SIGTSTP, tstp_handler);
  print_message ("suspended (penalty: 10 points)");
  wnoutrefresh (moon);
  wnoutrefresh (status);
  doupdate ();
  clock_adjust_delay (0.5);
}
 
int
main (int argc, char **argv)
{
#ifdef HAVE_GETOPT_LONG
  struct option  long_options [] = {
    { "help", no_argument, 0, 'h'},
    { "version", no_argument, 0, 'V'},
    { NULL, 0, NULL, 0}
  };
#endif
#define RND_SHORT_OPTIONS "hV"
  int  help_flag = 0;
  int  version_flag = 0;
  int  error_flag = 0;
  
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
 
  initscr ();
  cbreak ();
  noecho ();
  
  status = newwin (2, 0, LINES-2, 0);
  keypad (status, TRUE);
  leaveok (status, TRUE);
  moon = newwin (LINES-2, 0, 0, 0);
  keypad (moon, TRUE);
  leaveok (moon, TRUE);

  queuelag = new_lagmeter ();
  ground1 = xmalloc (COLS*sizeof(chtype));
  ground2 = xmalloc (COLS*sizeof(chtype));

  print_message ("good luck");
  main_loop ();
  mvwaddstr (moon, LINES-11, car_base-1, "GAME OVER");
  
  prepare_for_exit ();
  return  0;
}
