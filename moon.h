/* moon.h - global moon-buggy header file
 *
 * Copyright (C) 1998  Jochen Voss.
 *
 * $Id: moon.h,v 1.8 1998/12/28 20:12:53 voss Exp $ */

#ifndef FILE_MOON_H_SEEN
#define FILE_MOON_H_SEEN

#include <curses.h>
WINDOW *status, *moon;

extern  const char *my_name;
extern  long  score, bonus;

extern  void  prepare_for_exit (void);

extern  int  d_rnd (int limit);
extern  double  vclock (void);
extern  void *xmalloc (size_t size);
extern  void *xrealloc (void *ptr, size_t size);
extern  char *xstrdup (const char *str);

/* from "persona.c" */
extern  void  initialize_persona (void);
extern  void  set_game_persona (void);
extern  void  set_user_persona (void);

/* from "error.c" */
#ifdef __GNUC__
extern  void  fatal (const char *format, ...)
	__attribute__ ((noreturn)) __attribute__ ((format (printf, 1, 2)));
#else
extern  void  fatal (const char *format, ...);
#endif

/* from "queue.c" */
enum event_type { ev_KEY, ev_STATUS, ev_SCROLL, ev_BUGGY, ev_SCORE };
extern  double  sleep_meter;
extern  void  clock_adjust_delay (double dt);
extern  void  clear_queue (void);
extern  void  add_event (double t, enum event_type type);
extern  enum event_type  get_event (double *t);
extern  int  remove_event (enum event_type type, double *t);

/* from "moon.c" */
extern  char *ground1, *ground2;
extern  void  scroll_ground (void);

/* from "buggy.c" */
extern  int  car_base, score_base;
extern  void  initialize_buggy (void);
extern  void  print_buggy (void);
extern  void  jump (double t);
extern  int  can_jump (void);
extern  int  crash_check (void);

/* from "highscore.c" */
extern  void  write_hiscore (void);

/* from "lag.c" */
struct circle_buffer;
extern  struct circle_buffer *new_circle_buffer (void);
extern  void  add_value (struct circle_buffer *lm, double x);
extern  double  get_mean (const struct circle_buffer *lm);

extern  struct circle_buffer *queuelag;

/* from "realname.c" */
extern  void  get_real_user_name (char *buffer, size_t size);

/* from "mbpaths.c" */
extern  const char *score_dir;

#endif /* FILE_MOON_H_SEEN */
