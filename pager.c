/* pager.c - display very long strings (with scrolling)
 *
 * Copyright (C) 1998  Jochen Voss.  */

static const  char  rcsid[] = "$Id: pager.c,v 1.1 1998/12/29 00:37:21 voss Exp $";

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>

#include "moon.h"
#include "copying.h"


static unsigned  lines_used, current_line = 0;


static void
print_page (unsigned current_line)
{
  int  i;

  for (i=0; i<LINES-3; ++i) {
    if (current_line + i < lines_used) {
      mvwaddstr (moon, i, 2, copying_lines[current_line+i]);
    } else {
      mvwaddstr (moon, i, 2, "~");
    }
    wclrtoeol (moon);
  }
  wnoutrefresh (moon);

  mvwprintw (status, 0, 0, "=== COPYING %3d%% ===",
	     lines_used==0 ? 100 :(int)(current_line*100.0/(lines_used-1)+.5));
  wnoutrefresh (status);
}

static void
setup_screen (void)
{
  wclear (moon);
  wclear (status);
  print_page (current_line);
  mvwaddstr (status, 1, 0,
	     "`q' to return, page up/down and arrow keys to navigate");
  wnoutrefresh (status);
}

void
pager_mode (int what)
{
  int  done = 0;
  int  changed = 0;
  int  i;

  screen_done = 0;
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
  
  screen_done = 1;
  doupdate ();

  do {
    switch (get_event (NULL)) {
    case ev_KEY:
      switch (wgetch (moon)) {
      case KEY_BREAK:
      case KEY_CANCEL:
      case KEY_EXIT:
      case '\e':
      case 'q':
	done = 1;
	break;
      case KEY_UP:
	if (current_line > 0)  --current_line;
	changed = 1;
	break;
      case KEY_DOWN:
	if (current_line < lines_used-1)  ++current_line;
	changed = 1;
	break;
      case KEY_NPAGE:
      case ' ':
	current_line += LINES-3;
	if (current_line >= lines_used) {
	  current_line = lines_used-1;
	  if (current_line < 0)  current_line = 0;
	}
	changed = 1;
	break;
      case KEY_PPAGE:
      case 'b':
	if (current_line > LINES-3) {
	  current_line -= LINES-3;
	} else {
	  current_line = 0;
	}
	changed = 1;
	break;
      case KEY_A1:
      case KEY_HOME:
      case '<':
	current_line = 0;
	changed = 1;
	break;
      case KEY_C1:
      case KEY_END:
      case '>':
	current_line = lines_used-1;
	if (current_line < 0)  current_line = 0;
	changed = 1;
	break;
      default:
	beep ();
	doupdate ();
	break;
      }
      break;
    default:
      break;
    }
    if (changed) {
      print_page (current_line);
      doupdate ();
      changed = 0;
    }
  } while (! done);
}
