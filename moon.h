/* moon.h - global moon-buggy header file
 *
 * Copyright 1999  Jochen Voss
 *
 * $Id: moon.h,v 1.15 1999/04/21 19:19:26 voss Exp $ */

#ifndef FILE_MOON_H_SEEN
#define FILE_MOON_H_SEEN

/* The game's overall speed.
 * You may try to decrease this, if the moon's ground don't move
 * properly. */
#define  MB_SPEED  1.0

/* Set to 1 to see additional debugging information.
 * Only used for the game's development.  */
#define MB_DEBUG 0

#include <curses.h>
#include <stdlib.h>		/* we use `size_t' */

/* If key symbols are not defined move them out of the way.
 * Avoid duplicate case labels.  */
#ifndef KEY_BEG
#define KEY_BEG -1
#endif
#ifndef KEY_CANCEL
#define KEY_CANCEL -2
#endif
#ifndef KEY_CLOSE
#define KEY_CLOSE -3
#endif
#ifndef KEY_END
#define KEY_END -4
#endif
#ifndef KEY_EXIT
#define KEY_EXIT -5
#endif
#ifndef KEY_UNDO
#define KEY_UNDO -6
#endif

#define TICK(x) ((x)*0.08/(MB_SPEED))

/* from "main.c" */
extern  const char *my_name;
extern  WINDOW *moon, *status, *message;

enum game_state { INIT, TITLE, PAGER, PLAYING, HIGHSCORE };
extern  enum game_state  game_state;

extern  void  print_message (const char *str);
extern  void  allocate_windows (void);
extern  void  prepare_for_exit (void);

/* from "title.c" */
extern  int  title_mode (void);
extern  void  resize_title (void);

/* from "pager.c" */
extern  void  pager_mode (int);
extern  void  resize_pager (void);

/* from "game.c" */
extern  long  score, bonus;

extern  int  game_mode (void);
extern  void  resize_game (void);
extern  void  score_bonus (int x);

/* from "moon.c" */
extern  char *ground1, *ground2, *ground3;
extern  int  d_rnd (int limit);
extern  void  resize_ground (int clear_it);
extern  void  print_ground (void);
extern  void  scroll_ground (void);

/* from "buggy.c" */
extern  int  car_base, score_base;
extern  void  initialise_buggy (void);
extern  void  animate_buggy (void);
extern  int  print_buggy (void);
extern  void  jump (double t);
extern  int  can_jump (void);
extern  int  crash_check (void);

/* from "highscore.c" */
extern  int  highscore_mode (void);
extern  void  resize_highscore (void);

/* from "realname.c" */
extern  void  get_real_user_name (char *buffer, size_t size);

/* from "queue.c" */
enum event_type { ev_KEY, ev_TIMEOUT, ev_MESSAGE, ev_SCROLL, ev_BUGGY, \
		  ev_SCORE };
typedef  double  game_time;
extern  double  sleep_meter;
extern  struct circle_buffer *queuelag;

extern  void  clock_adjust_delay (double dt);
extern  void  clear_queue (void);
extern  void  add_event (game_time t, enum event_type type);
extern  enum event_type  get_event (game_time *t_return);
extern  int  remove_event (enum event_type type, game_time *t_return);

/* from "vclock.c" */
extern  double  vclock (void);

/* from "persona.c" */
extern  void  initialise_persona (void);
extern  void  set_game_persona (void);
extern  void  set_user_persona (void);

/* from "signal.c" */
extern  void  block_winch (void);
extern  void  block_all (void);
extern  void  unblock (void);
extern  void  initialise_signals (void);
extern  void  handle_signals (void);

/* from "xgetch.c" */
extern  int  xgetch (WINDOW *win);

/* from "error.c" */
#ifdef __GNUC__
extern  void  fatal (const char *format, ...)
	__attribute__ ((noreturn)) __attribute__ ((format (printf, 1, 2)));
#else
extern  void  fatal (const char *format, ...);
#endif

/* from "xmalloc.c" */
extern  void *xmalloc (size_t size);
extern  void *xrealloc (void *ptr, size_t size);

/* from "xstrdup.c" */
extern  size_t  xstrnlen (const char *str, size_t size);
extern  char *xstrdup (const char *str);
extern  char *xstrndup (const char *str, size_t size);

/* from "hpath.c" */
extern  const char *score_dir;

/* for configure's AC_REPLACE_FUNCS calls */
#ifndef HAVE_MVWADDNSTR
extern  int  mvwaddnstr (WINDOW *win, int y, int x, const char *str, int n);
#endif
#ifndef HAVE_WGETNSTR
extern  int  wgetnstr (WINDOW *win, char *str, int n);
#endif

#endif /* FILE_MOON_H_SEEN */
