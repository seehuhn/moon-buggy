/* mbuggy.h - global moon-buggy header file
 *
 * Copyright 1999  Jochen Voss
 *
 * $Id: mbuggy.h,v 1.16 1999/06/25 23:26:41 voss Rel $ */

#ifndef FILE_MBUGGY_H_SEEN
#define FILE_MBUGGY_H_SEEN

/* The game's overall speed.  High values indicate fast scrolling.
 * You may try to decrease this, if the animation flickers too much. */
#define  MB_SPEED  1.0

#include <curses.h>
#include <stdlib.h>		/* we use `size_t' */

#define TICK(x) ((x)*0.08/(MB_SPEED))
#define BASELINE (LINES-5)

/* from "main.c" */
extern  const char *my_name;
extern  WINDOW *moon, *status, *message;

enum game_state { INIT, TITLE, PAGER, PLAYING, HIGHSCORE };
extern  enum game_state  game_state;

extern int  car_base;


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
extern  int  crash_detected;
extern  int  stakes;

extern  void  adjust_score (int val);
extern  int  game_mode (void);
extern  void  resize_game (void);
extern  void  quit_game (void);

/* from "level.c" */
extern  void  level_start (int initial);
extern  void  level_tick (double t);
extern  int  current_level (void);

/* from "moon.c" */
extern  int *bonus;
extern  char *ground1, *ground2;
extern  void  resize_ground (int clear_it);
extern  void  print_ground (void);
extern  void  start_scrolling (double t);

/* from "buggy.c" */
extern  int  car_x, car_y;
extern  void  initialise_buggy (void);
extern  void  print_buggy (void);
extern  void  jump (double t);
extern  int  can_jump (void);
extern  int  crash_check (void);
extern  void  shift_buggy (int dx);
extern  int  car_meteor_hit (double t, int x);

/* from "laser.c" */
extern  void  fire_laser (double t);
extern  void  extinguish_laser (void);
extern  int  laser_hit (int x);
extern  void  resize_laser (void);

/* from "meteor.c" */
extern  void  place_meteor (double t);
extern  void  remove_meteors (void);
extern  int  meteor_laser_hit (int x0, int x1);
extern  int  meteor_car_hit (int x0, int x1);
extern  void  resize_meteors (void);

/* from "highscore.c" */
extern  int  highscore_mode (int score, int level);
extern  void  resize_highscore (void);

/* from "realname.c" */
extern  void  get_real_user_name (char *buffer, size_t size);

/* from "queue.c" */
typedef  double  game_time;
typedef  void (*callback_fn) (game_time, void *);

extern  double  sleep_meter;

extern  void  save_queue (void);
extern  void  restore_queue (void);

extern  void  clock_adjust_delay (double dt);
extern  void  clear_queue (void);
extern  void  add_event (game_time t, callback_fn callback, void *client_data);
extern  void  remove_event (callback_fn callback);
extern  void  remove_client_data (void *client_data);
extern  void  fix_game_time (void);
extern  void  quit_main_loop (void);
extern  int  main_loop (double dt, void (*key_handler)(game_time));
extern  void  xsleep (double dt);

extern  void  quit_main_loop_h (game_time, void *);
extern  void  print_message_h (game_time t, void *client_data);
extern  void  clear_message_h (game_time, void *);

/* from "vclock.c" */
extern  double  vclock (void);

/* from "getdate.c" */
extern  void  get_date (int *day_p, int *month_p, int *year_p);

/* from "persona.c" */
enum persona { pers_GAME, pers_USER };
extern  void  initialise_persona (void);
extern  void  set_persona (enum persona  pers);

/* from "signal.c" */
extern  void  block_all (void);
extern  void  unblock (void);
extern  void  initialise_signals (void);
extern  void  handle_signals (void);

/* from "keyboard.c" */
enum mb_key {
  mbk_copyright = 1, mbk_down = 2, mbk_end = 4, mbk_fire = 8, mbk_first = 16,
  mbk_jump = 32, mbk_last = 64, mbk_pagedown = 128, mbk_pageup = 256,
  mbk_start = 512, mbk_up = 1024, mbk_warranty = 2048
};
extern  void  install_keys (void);
extern  int  read_key (void);

/* from "random.c" */
extern  void  init_rnd (void);
extern  int  uniform_rnd (unsigned limit);

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

#endif /* FILE_MBUGGY_H_SEEN */
