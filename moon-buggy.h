/* moon-buggy.h - global moon-buggy header file
 *
 * Copyright 1999, 2000  Jochen Voss
 */

#ifndef FILE_MOON_BUGGY_H_SEEN
#define FILE_MOON_BUGGY_H_SEEN

/* The game's overall speed.  High values indicate fast scrolling.
 * You may try to decrease this, if the animation flickers too much. */
#define  MB_SPEED  1.0

#include <stdlib.h>		/* we use `size_t' */
#include <time.h>		/* we use `time_t' */

#ifndef CURSES_HEADER
#define CURSES_HEADER <curses.h>
#endif
#include CURSES_HEADER

#define TICK(x) ((x)*0.08/(MB_SPEED))
#define BASELINE (LINES-5)


/* from "main.c" */
extern  const char *my_name;
extern  WINDOW *moon, *status, *message;

extern int  car_base;

extern  void  print_message (const char *str);
extern  void  print_hint (const char *str);
extern  void  prepare_screen (void);
extern  void  prepare_for_exit (void);
extern  void  allocate_windows (void);
extern  void  clear_windows (void);

/* from "queue.c" */
typedef  double  game_time;
typedef  void (*callback_fn) (game_time, void *);

extern  game_time  current_time (void);

extern  void  clock_reset (void);
extern  void  clock_thaw (void);
extern  void  clear_queue (void);
extern  void  add_event (game_time t, callback_fn callback, void *client_data);
extern  void  remove_event (callback_fn callback);
extern  void  remove_client_data (void *client_data);
extern  void  clock_freeze (void);
extern  void  quit_main_loop (void);
extern  void  main_loop (void);

extern  void  print_hint_h (game_time t, void *client_data);
extern  void  clear_hint_h (game_time, void *);

/* from "title.c" */
extern  struct mode *title_mode;
extern  void  setup_title_mode (void);

/* from "pager.c" */
extern  struct mode *pager_mode;
extern  void  setup_pager_mode (void);

/* from "game.c" */
extern  struct mode *game_mode;
extern  struct mode *crash_mode;
extern  int  crash_detected;
extern  int  stakes;

extern  void  adjust_score (int val);
extern  void  print_lives (void);
extern  void  print_game_over (int blink);
extern  void  setup_game_mode (void);

/* from "level.c" */
extern  void  level_start (int initial);
extern  void  level_tick (double t);
extern  int  current_level (void);

/* from "ground.c" */
extern  int *bonus;
extern  char *ground1, *ground2;
extern  void  resize_ground (int clear_it);
extern  void  print_ground (void);
extern  void  start_scrolling (double t);

/* from "buggy.c" */
extern  int  car_x, car_y;
extern  void  initialise_buggy (void);
extern  void  print_buggy (void);
extern  void  shift_buggy (int dx);
extern  void  jump (game_time t);
extern  int  can_jump (void);
extern  int  crash_check (void);
extern  int  car_meteor_hit (int x);

/* from "laser.c" */
extern  void  fire_laser (double t);
extern  void  extinguish_laser (void);
extern  int  laser_hit (int x);
extern  void  resize_laser (void);

/* from "meteor.c" */
extern  void  scroll_meteors (void);
extern  void  place_meteor (void);
extern  void  remove_meteors (void);
extern  int  meteor_laser_hit (int x0, int x1);
extern  int  meteor_car_hit (int x0, int x1);
extern  void  resize_meteors (void);

/* from "highscore.c" */
extern  struct mode *highscore_mode;
extern  void  create_highscores (void);
extern  void  show_highscores (void);

extern  void  score_set (int score, int level);
extern  void  setup_highscore_mode (void);

/* from "realname.c" */
extern  int  get_real_user_name (char *buffer, size_t size);

/* from "vclock.c" */
extern  double  vclock (void);

/* from "date.c" */
#define MAX_DATE_CHARS 32
extern  time_t  parse_date (const char *str);
extern  time_t  convert_old_date (int day, int month, int year);
extern  void  format_date (char *buffer, time_t date);
extern  void  format_display_date (char *buffer, time_t date);
extern  void  format_relative_time (char *buffer, double dt);

/* from "persona.c" */
enum persona { pers_GAME, pers_USER };
extern  void  initialise_persona (void);
extern  int  is_setgid (void);
extern  void  set_persona (enum persona  pers);

/* from "signal.c" */
extern  void  block_all (void);
extern  void  unblock (void);
extern  void  initialise_signals (void);
extern  int  handle_signals (void);

/* from "keyboard.c" */
enum mb_key {
  mbk_copyright = 1, mbk_down = 2, mbk_end = 4, mbk_fire = 8, mbk_first = 16,
  mbk_jump = 32, mbk_last = 64, mbk_pagedown = 128, mbk_pageup = 256,
  mbk_start = 512, mbk_up = 1024, mbk_warranty = 2048, mbk_scores = 4096,

  mbk_redraw = 8192		/* specially handled in `mode_keypress' */
};
struct binding {
  int  meanings;
  const char *desc;
  int  res;
};
extern  void  install_keys (void);
extern  int  read_key (void);
extern  void  describe_keys (int n, const struct binding *b);

/* from "mode.c" */
struct mode {
  void (*enter) (int seed);
  void (*leave) (void);
  void (*redraw) (void);
  void (*signal) (int signum);

  struct {
    struct binding *data;
    int used, slots;
  } keys;
  void (*keypress) (game_time, int);
};

extern  struct mode *new_mode (void);
extern  void  mode_add_key (struct mode *m,
                            int meanings, const char *desc, int res);
extern  void  mode_complete (struct mode *m);
extern  void  mode_change (const struct mode *m, int seed);
extern  void  mode_update (void);
extern  void  mode_redraw (void);
extern  int  mode_keypress (game_time t, int meaning);
extern  void  mode_signal (int signum);

/* from "terminal.c" */
extern  void  term_prepare (int mesg_n_flag);
extern  void  term_restore (void);

/* from "cursor.c" */
extern  void  hide_cursor (void);
extern  void  show_cursor (void);

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

#endif /* FILE_MOON_BUGGY_H_SEEN */
