/* pager.c - display very long strings (with scrolling)
 *
 * Copyright 1999  Jochen Voss  */

static const  char  rcsid[] = "$Id: pager.c,v 1.11 1999/06/06 13:18:29 voss Exp $";

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>

#include "mbuggy.h"
#include "copying.h"


static unsigned  lines_used, current_line;
static volatile  int  mb_lines = 25;


static void
print_page (unsigned current_line)
{
  int  i;

  for (i=0; i<mb_lines-3; ++i) {
    if (current_line + i < lines_used) {
      mvwaddstr (moon, i, 2, copying_lines[current_line+i]);
    } else {
      mvwaddstr (moon, i, 2, "~");
    }
    wclrtoeol (moon);
  }
  wnoutrefresh (moon);

  mvwprintw (status, 0, 0, "=== COPYING %3d%% ===  ",
	     lines_used==0 ? 100 :(int)(current_line*100.0/(lines_used-1)+.5));
  wnoutrefresh (status);
}

static void
setup_screen (void)
{
  mb_lines = LINES;
  werase (moon);
  print_page (current_line);

  werase (status);
  
  werase (message);
  waddstr (message, "`q' to return, ` ' page down, `b' page up");
  wnoutrefresh (message);
}

static void
key_handler (game_time t)
{
  int  meaning = read_key ();
  if (meaning & mbk_end) {
    quit_main_loop ();
  } else if (meaning & mbk_up) {
    if (current_line > 0)  --current_line;
    print_page (current_line);
  } else if (meaning & mbk_down) {
    if (current_line < lines_used-1)  ++current_line;
    print_page (current_line);
  } else if (meaning & mbk_pagedown) {
    current_line += mb_lines-3;
    if (current_line >= lines_used) {
      current_line = lines_used-1;
      if (current_line < 0)  current_line = 0;
    }
    print_page (current_line);
  } else if (meaning & mbk_pageup) {
    if (current_line > mb_lines-3) {
      current_line -= mb_lines-3;
    } else {
      current_line = 0;
    }
    print_page (current_line);
  } else if (meaning & mbk_first) {
    current_line = 0;
    print_page (current_line);
  } else if (meaning & mbk_last) {
    current_line = lines_used-1;
    if (current_line < 0)  current_line = 0;
    print_page (current_line);
  } else {
    beep ();
  }
}

void
pager_mode (int what)
{
  int  i;

  game_state = PAGER;

  lines_used = sizeof (copying_lines) / sizeof (const char *);
  switch (what) {
  case 0:
    current_line = 0;
    break;
  case 1:
    current_line = 0;
    for (i=0; i<lines_used; ++i) {
      if (strstr (copying_lines[i], "NO WARRANTY")) {
	current_line = i;
	break;
      }
    }
  default:
    break;
  }
  setup_screen ();

  add_event (0, quit_main_loop_h, NULL);
  main_loop (3600, key_handler);
}

void
resize_pager (void)
{
  setup_screen ();
  doupdate ();
}
