/* pager.c - display very long strings (with scrolling)
 *
 * Copyright 1999, 2000  Jochen Voss  */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>

#include "moon-buggy.h"
#include "copying.h"



struct mode *pager_mode;


static  int  lines_used, current_line;
static  int  mb_lines = 25;


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
pager_redraw (void)
{
  mb_lines = LINES;
  print_page (current_line);
}

static void
key_handler (game_time t, int val)
{
  switch (val) {
  case 1:
    mode_change (title_mode, 0);
    break;
  case 2:
    if (current_line > 0)  --current_line;
    print_page (current_line);
    break;
  case 3:
    if (current_line < lines_used-1)  ++current_line;
    print_page (current_line);
    break;
  case 4:
    if (current_line > mb_lines-3) {
      current_line -= mb_lines-3;
    } else {
      current_line = 0;
    }
    print_page (current_line);
    break;
  case 5:
    current_line += mb_lines-3;
    if (current_line >= lines_used) {
      current_line = lines_used-1;
      if (current_line < 0)  current_line = 0;
    }
    print_page (current_line);
    break;
  case 6:
    current_line = 0;
    print_page (current_line);
    break;
  case 7:
    current_line = lines_used-1;
    if (current_line < 0)  current_line = 0;
    print_page (current_line);
    break;
  }
}

static void
pager_enter (int what)
{
  int  i;

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
}

static void
pager_leave (void)
{
  werase (status);
  wnoutrefresh (status);
}

void
setup_pager_mode (void)
{
  pager_mode = new_mode ();
  pager_mode->enter = pager_enter;
  pager_mode->leave = pager_leave;
  pager_mode->redraw = pager_redraw;
  pager_mode->keypress = key_handler;
  mode_add_key (pager_mode, mbk_end, "quit", 1);
  mode_add_key (pager_mode, mbk_up, "up", 2);
  mode_add_key (pager_mode, mbk_down, "down", 3);
  mode_add_key (pager_mode, mbk_pageup, "pg up", 4);
  mode_add_key (pager_mode, mbk_pagedown, "pg down", 5);
  mode_add_key (pager_mode, mbk_first, "home", 6);
  mode_add_key (pager_mode, mbk_last, "end", 7);
  mode_complete (pager_mode);
}
