/* main.c - moon-buggy main file
 *
 * Copyright 1999  Jochen Voss  */

static const  char  rcsid[] = "$Id: main.c,v 1.15 1999/03/08 20:26:29 voss Exp $";

#define _POSIX_SOURCE 1

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

#include "moon.h"


const char *my_name;
WINDOW *moon, *status, *message;
enum game_state  game_state = INIT;


void
print_message (const char *str)
/* Display STR in the message line.  */
{
  wclear (message);
  waddstr (message, str);
  wnoutrefresh (message);
}

void
allocate_windows (void)
/* Create the curses windows.  */
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
/* Prepare the screen to exit from the program.  */
{
  wclear (message);
  wnoutrefresh (moon);
  wnoutrefresh (status);
  wnoutrefresh (message);
  doupdate ();
  endwin ();
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
  
  initialise_persona ();
  set_user_persona ();

  /* `basename' seems to be non-standard.  So we avoid it.  */
  my_name = strrchr (argv[0], '/');
  my_name = xstrdup (my_name ? my_name+1 : argv[0]);

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
    puts ("Moon-Buggy " VERSION);
    puts ("Copyright (C) 1998 Jochen Voﬂ");
    puts ("\
Moon-Buggy comes with NO WARRANTY, to the extent permitted by law.");
    puts ("\
You may redistribute copies of Moon-Buggy under the terms of the GNU\n\
General Public License.  For more information about these matters, see\n\
the file named COPYING or press `c' at Moon-Buggy's title screen.");
    if (! error_flag)  exit (0);
  }
  if (error_flag || help_flag) {
#ifdef HAVE_GETOPT_LONG
#define OPT(short,long) "  " short ", " long "    "
#else
#define OPT(short,long) "  " short "  "
#endif
    FILE *out = error_flag ? stderr : stdout;
    fprintf (out, "usage: %s [options]\n\n", my_name);
    fputs ("The options are\n", out);
    fputs (OPT("-h","--help    ") "show this message and exit\n", out);
    fputs (OPT("-n","--no-title") "omit the title screen\n", out);
    fputs (OPT("-V","--version ") "show the version number and exit\n\n", out);
    fputs ("Please report bugs to <voss@mathematik.uni-kl.de>.\n", out);
    exit (error_flag);
  }

  initialise_signals ();
  
  initscr ();
  cbreak ();
  noecho ();

  allocate_windows ();

  if (title_flag) {
    res = title_mode ();
  } else {
    res = 0;
  }

  if (! res) {
    print_message ("good luck (or use SPACE to jump)");
    while (game_mode ())
      ;
    mvwaddstr (moon, LINES-11, car_base-1, "GAME OVER");
  }
  
  prepare_for_exit ();
  return  0;
}
