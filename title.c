/* title.c - print the title page
 *
 * Copyright 1999  Jochen Voss  */

static const  char  rcsid[] = "$Id: title.c,v 1.18 2000/04/01 07:53:29 voss Exp $";


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "moon-buggy.h"


struct mode *title_mode;


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
	   VERSION ", Copyright 1998,99 Jochen Voss\n");
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

  if (5 + title_lines + 7 <= LINES)  print_buggy ();

  wnoutrefresh (moon);
}

static void
setup_screen (void)
{
  resize_ground (1);
  print_title ();
  print_ground ();
}

static void
title_enter (int x)
{
  setup_screen ();
}

static void
key_handler (game_time t, int val)
{
  switch (val) {
  case 1:
    mode_change (game_mode, 0);
    break;
  case 2:
    quit_main_loop ();
    break;
  case 3:
    mode_change (pager_mode, 0);
    break;
  case 4:
    mode_change (pager_mode, 1);
    break;
  case 5:
    mode_change (highscore_mode, 0);
    break;
  }
}

void
setup_title_mode (void)
{
  title_mode = new_mode ();
  title_mode->enter = title_enter;
  title_mode->redraw = setup_screen;
  title_mode->keypress = key_handler;
  mode_add_key (title_mode, mbk_start, "start game", 1);
  mode_add_key (title_mode, mbk_end, "quit", 2);
  mode_add_key (title_mode, mbk_copyright, "show copyright", 3);
  mode_add_key (title_mode, mbk_warranty, "show warranty", 4);
  mode_add_key (title_mode, mbk_scores, "show scores", 5);
}
