/* title.c - print the title page
 *
 * Copyright 1999  Jochen Voss  */

static const  char  rcsid[] = "$Id: title.c,v 1.6 1999/01/09 14:06:13 voss Rel $";


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "moon.h"


const char *title [] = {
  "MM     MM   OOOOO    OOOOO   NN     N",
  "M M   M M  O     O  O     O  N N    N",
  "M  M M  M  O     O  O     O  N  N   N",
  "M   M   M  O     O  O     O  N   N  N",
  "M       M  O     O  O     O  N    N N",
  "M       M   OOOOO    OOOOO   N     NN",
  "",
  "      BBBBBB   U     U   GGGGG    GGGGG   Y     Y",
  "      B     B  U     U  G     G  G     G   Y   Y",
  "      BBBBBB   U     U  G        G          Y Y",
  "      B     B  U     U  G   GGG  G   GGG     Y",
  "      B     B  U     U  G     G  G     G    Y",
  "      BBBBBB    UUUUU    GGGGG    GGGGG   YY"
};

static void
print_title (void)
/* Print the title screen.  */
{
  int  title_lines = sizeof (title) / sizeof (const char *);

  waddstr (moon, "  Moon-Buggy version "
	   VERSION ", Copyright (C) 1998 Jochen Voﬂ\n");
  waddstr (moon, "  Moon-Buggy comes with ABSOLUTELY NO WARRANTY;"
	   " for details type `w'.\n");
  waddstr (moon,
	   "  This is free software, and you are welcome to redistribute it\n\
  under certain conditions; type `c' for details.\n");

  if (5 + title_lines + 5 <= LINES) {
    int  top = (LINES-title_lines)/3.0 + 0.5;
    int  i;
    
    if (top < 5)  top = 5;
    for (i=0; i<title_lines; ++i) {
      mvwaddstr (moon, top+i, (COLS-49)/2, title[i]);
    }
  }

  if (5 + title_lines + 7 <= LINES) {
    initialize_buggy ();
    print_buggy ();
  }

  wnoutrefresh (moon);
}

static void
setup_screen (void)
{
  wclear (moon);
  resize_ground (1);
  print_title ();
  print_ground ();

  wclear (status);
  wnoutrefresh (status);
  
  wclear (message);
  waddstr (message, "press `space' to start");
  wnoutrefresh (message);
}

int
title_mode (void)
/* Show the title, until the user does not want to see it any more.  */
{
  int  done = 0;
  int  abort = 0;
  
  game_state = TITLE;
  setup_screen ();
  doupdate ();

  do {
    switch (get_event (NULL)) {
    case ev_KEY:
      switch (xgetch (moon)) {
      case KEY_BREAK:
      case KEY_CANCEL:
      case KEY_UNDO:
      case 'q':
	done = abort = 1;
	break;
      case KEY_BEG:
      case KEY_CLEAR:
      case KEY_ENTER:
      case KEY_EXIT:
      case 27:			/* ESC */
      case ' ':
	done = 1;
	break;
      case 'c':
	pager_mode (0);
	game_state = TITLE;
	setup_screen ();
	doupdate ();
	break;
      case 'w':
	pager_mode (1);
	game_state = TITLE;
	setup_screen ();
	doupdate ();
	break;
      default:
	break;
      }
      break;
    default:
      break;
    }
  } while (! done);

  return  abort;
}

void
resize_title (void)
{
  setup_screen ();
  doupdate ();
}
