/* title.c - print the title page
 *
 * Copyright 1999  Jochen Voss  */

static const  char  rcsid[] = "$Id: title.c,v 1.13 1999/06/06 13:17:43 voss Exp $";


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "mbuggy.h"


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
    initialise_buggy ();
    print_buggy ();
  }

  wnoutrefresh (moon);
}

static void
setup_screen (void)
{
  werase (moon);
  resize_ground (1);
  print_title ();
  print_ground ();

  werase (status);
  wnoutrefresh (status);
  
  werase (message);
  waddstr (message, "press `space' to start");
  wnoutrefresh (message);
}


static  int  abort_flag;

static void
key_handler (game_time t)
{
  int  meaning = read_key ();
  if (meaning & mbk_end) {
    abort_flag = 1;
    quit_main_loop ();
  } else if (meaning & mbk_start) {
    quit_main_loop ();
  } else if (meaning & mbk_copyright) {
    save_queue ();
    pager_mode (0);
    restore_queue ();
    game_state = TITLE;
    setup_screen ();
  } else if (meaning & mbk_warranty) {
    save_queue ();
    pager_mode (1);
    restore_queue ();
    game_state = TITLE;
    setup_screen ();
  } else {
    beep ();
  }
}

int
title_mode (void)
/* Show the title, until the user does not want to see it any more.  */
{
  game_state = TITLE;
  setup_screen ();

  abort_flag = 0;
  add_event (0, quit_main_loop_h, NULL);
  main_loop (3600, key_handler);

  return  abort_flag;
}

void
resize_title (void)
{
  setup_screen ();
  doupdate ();
}
