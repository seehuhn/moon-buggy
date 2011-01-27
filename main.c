/* main.c - moon-buggy main file
 *
 * Copyright 1999, 2000, 2004, 2006  Jochen Voss  */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <string.h>
#ifdef HAVE_GETOPT_H
#include <getopt.h>
#else
#include <unistd.h>
extern char *optarg;
extern int  optind;
#endif
#ifdef HAVE_LOCALE_H
#include <locale.h>
#endif

#include "moon-buggy.h"


const char *my_name;
static  int  curses_initialised;
static  int  mesg_flag;
WINDOW *moon, *status, *message;

int  car_base;


void
print_message (const char *str)
/* Display STR in the message line.  */
{
  if (curses_initialised) {
    werase (message);
    waddstr (message, str);
    wnoutrefresh (message);
  } else {
    fprintf (stderr, "%s\n", str);
  }
}

void
print_hint (const char *str)
/* Display STR in the message line.  */
{
  if (curses_initialised) {
    int  len = strlen (str);
    int  cols = COLS;
    int  pos;

    if (car_base+3+len/2 >= cols) {
      pos = cols - len - 1;
    } else {
      pos = car_base+3-len/2;
    }
    wmove (moon, LINES-11, 0);
    wclrtoeol (moon);
    mvwaddstr (moon, LINES-11, pos, str);
    wnoutrefresh (moon);
  }
}

void
prepare_screen (void)
/* Prepare the screen for the program.
 * Calls to `prepare_screen' and `prepare_for_exit' must be paired.  */
{
  cbreak ();
  noecho ();
  hide_cursor ();
  term_prepare (mesg_flag);
}

void
prepare_for_exit (void)
/* Prepare the screen to exit from the program.
 * Calls to `prepare_screen' and `prepare_for_exit' must be paired.  */
{
  if (! curses_initialised)  return;
  term_restore ();
  show_cursor ();
  wrefresh (moon);
  wrefresh (message);
  werase (status);
  wmove (status, 0, 0);
  wrefresh (status);
  endwin ();
  fflush (NULL);
}

void
allocate_windows (void)
/* Create the curses windows.  */
{
  initscr ();

  moon = newwin (LINES-2, 0, 0, 0);
  keypad (moon, TRUE);
  intrflush (moon, FALSE);

  status = newwin (1, 0, LINES-1, 0);
  keypad (status, TRUE);
  intrflush (status, FALSE);

  message = newwin (1, 0, LINES-2, 0);
  keypad (message, TRUE);
  intrflush (message, FALSE);
}

void
clear_windows (void)
{
  wclear (moon);  wnoutrefresh (moon);
  wclear (status);  wnoutrefresh (status);
  wclear (message);  wnoutrefresh (message);
}

/************************************************************
 * main procedure
 */

int
main (int argc, char **argv)
{
#ifdef HAVE_GETOPT_LONG
  struct option  long_options [] = {
    { "create-scores", no_argument, 0, 'c' },
    { "help", no_argument, 0, 'h' },
    { "mesg", no_argument, 0, 'm' },
    { "no-title", no_argument, 0, 'n' },
    { "show-scores", no_argument, 0, 's' },
    { "version", no_argument, 0, 'V' },
    { NULL, 0, NULL, 0}
  };
#endif
#define MB_SHORT_OPTIONS "chmnsV"
  int  help_flag = 0;
  int  highscore_flag = 0;
  int  title_flag = 1;
  int  version_flag = 0;
  int  error_flag = 0;

  initialise_persona ();
  set_persona (pers_USER);

#ifdef HAVE_SETLOCALE
  setlocale (LC_CTYPE, "");
#endif

  /* `basename' seems to be non-standard.  So we avoid it.  */
  my_name = strrchr (argv[0], '/');
  my_name = xstrdup (my_name ? my_name+1 : argv[0]);

  do {
    int  c;
#ifdef HAVE_GETOPT_LONG
    int  ind;
    c = getopt_long (argc, argv, MB_SHORT_OPTIONS, long_options, &ind);
#else
    c = getopt (argc, argv, MB_SHORT_OPTIONS);
#endif
    if (c == -1)  break;
    switch (c) {
    case 'c':
      highscore_flag = 2;
      break;
    case 'h':
      help_flag = 1;
      break;
    case 'm':
      mesg_flag = 1;
      break;
    case 'n':
      title_flag = 0;
      break;
    case 's':
      highscore_flag = 1;
      break;
    case 'V':
      version_flag = 1;
      break;
    default:
      error_flag = 1;
    }
  } while (! error_flag);

  if (argc != optind) {
    fputs ("too many arguments\n", stderr);
    error_flag = 1;
  }

  if (version_flag) {
    puts ("Moon-Buggy " VERSION);
    puts ("Copyright 1998-2006  Jochen Voss");
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
    /* --create-scores: create the highscore file (internal use only) */
    fputs (OPT("-h","--help         ") "show this message and exit\n", out);
    fputs (OPT("-m","--mesg         ") "imply the effect of \"mesg n\"\n",
           out);
    fputs (OPT("-n","--no-title     ") "omit the title screen\n", out);
    fputs (OPT("-s","--show-scores  ") "only show the highscore list\n", out);
    fputs (OPT("-V","--version      ") "show the version number and exit\n\n",
           out);
    fputs ("Please report bugs to <voss@seehuhn.de>.\n", out);
    exit (error_flag);
  }

  init_rnd ();

  if (highscore_flag) {
    if (highscore_flag == 1) {
      show_highscores ();
    } else {
      create_highscores ();
    }
    exit (0);
  }

  initialise_signals ();

  allocate_windows ();
  curses_initialised = 1;
  prepare_screen ();

  install_keys ();
  setup_title_mode ();
  setup_pager_mode ();
  setup_game_mode ();
  setup_highscore_mode ();

  clear_windows ();

  resize_ground (1);
  initialise_buggy ();

  if (title_flag) {
    mode_change (title_mode, 0);
  } else {
    mode_change (game_mode, 0);
  }
  main_loop ();
  mode_change (NULL, 0);

  prepare_for_exit ();
  return  0;
}
