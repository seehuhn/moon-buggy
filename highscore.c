/* highscore.c - maintain the highscore list
 *
 * Copyright 1999, 2000, 2001, 2006  Jochen Voss  */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef _XOPEN_SOURCE
#define _XOPEN_SOURCE_EXTENDED 1
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <assert.h>
#if HAVE_ERRNO_H
#include <errno.h>
#else
extern  int  errno;
#endif

#if defined(__hp9000s800)
#include <stdarg.h>
#endif

#include "moon-buggy.h"


struct mode *highscore_mode;


#define SCORE_FILE "mbscore"
#define MAX_NAME_CHARS 40
#define HIGHSCORE_SLOTS 100

#define qx(str) #str
#define quote(str) qx(str)


/* The highscore list, as read by `read_data'.  */
struct score_entry {
  int  score, level;
  time_t  date;
  char  name [MAX_NAME_CHARS];
  int  new;
};
static  struct score_entry  highscore [HIGHSCORE_SLOTS];
static  int  highscore_changed;


static char *
compose_filename (const char *dir, const char *name)
/* Concatenate the path DIR and the name NAME in a newly allocated string.  */
{
  char *res;

  if (dir) {
    res = xmalloc (strlen(dir) + 1 + strlen(name) + 1);
    strcpy (res, dir);
    strcat (res, "/");
    strcat (res, name);
  } else {
    res = xstrdup (name);
  }
  return  res;
}

static char *
local_score_file_name (void)
/* Return the local score file's name.
 * The caller is reponsible for freeing the returned string via `free'.  */
{
  uid_t  me = getuid ();
  struct passwd *my_passwd = getpwuid (me);

  if (my_passwd && my_passwd->pw_dir) {
    return  compose_filename (my_passwd->pw_dir, "." SCORE_FILE);
  } else {
    return  compose_filename (NULL, "." SCORE_FILE);
  }
}

static char *
global_score_file_name (void)
/* Return the global score file's name.
 * The is reponsible for freeing the returned string via `free'.  */
{
  return  compose_filename (score_dir, SCORE_FILE);
}


static int
compare_entries (const void *a, const void *b)
/* Compare two score values.
 * This is a comparison function for the use with `qsort'.  */
{
  const  struct score_entry *aa = a;
  const  struct score_entry *bb = b;
  if (aa->score > bb->score)  return -1;
  if (aa->score < bb->score)  return +1;
  return  0;
}

static void
merge_entry (const struct score_entry *entry)
/* Merge ENTRY into the highscore list `highscore'.
 * Set `highscore_changed' to 1 if the list is changed.  */
{
  struct score_entry *last;

  last = highscore+(HIGHSCORE_SLOTS-1);

  if (entry->score > last->score) {
    memcpy (last, entry, sizeof (struct score_entry));
    qsort (highscore, HIGHSCORE_SLOTS, sizeof (struct score_entry),
           compare_entries);
    highscore_changed = 1;
  }
}

static void
randomize_entry (int n)
/* Fill slot N of `highscore' with a random entry.   */
{
  static const char *names [13] = {
    "Dwalin", "Balin", "Kili", "Fili", "Dori", "Nori", "Ori",
    "Oin", "Gloin", "Bifur", "Bofur", "Bombur", "Thorin"
  };

  highscore[n].score = 10*(HIGHSCORE_SLOTS-n);
  highscore[n].level = 1;
  highscore[n].date = time (NULL);
  strcpy (highscore[n].name, names[uniform_rnd(13)]);
  highscore[n].new = 0;

  highscore_changed = 1;
}

static time_t
expire_date (int rank, time_t date)
/* Calculate the expiration date for an entry, which is at position RANK
 * and which was entered at DATE.  */
{
  /* linear interpolation: rank 3 expires after 180 days, the last one
   *                       after 14 days.  */
  double  day = 24*60*60;
  double  rate = (180-14)*day/(HIGHSCORE_SLOTS-4);

  if (rank < 3)  return  time(NULL)+2000*day;
  return  date + 14*day + (HIGHSCORE_SLOTS-1-rank)*rate;
}

static void
refill_old_entries (void)
/* Replace old entries of `highscore' with new random ones.  */
{
  time_t  now;
  int  i;

  now = time (NULL);
  for (i=3; i<HIGHSCORE_SLOTS; ++i) {
    if (expire_date (i, highscore[i].date) < now)  randomize_entry (i);
  }
  qsort (highscore, HIGHSCORE_SLOTS, sizeof (struct score_entry),
         compare_entries);
}


static void
generate_data (void)
/* Initialise `highscore' with random entries.  */
{
  int  i;

  for (i=0; i<HIGHSCORE_SLOTS; ++i)  randomize_entry (i);
}


static int
read_version2_data (FILE *score_file)
{
  int  err = 0;
  int  i, res;

  generate_data ();
  for (i=0; i<10 && ! err; ++i) {
    int  score, level;
    int  day, month, year;
    char  name [MAX_NAME_CHARS+1];
    res = fscanf (score_file,
                  "|%d|%d|%d|%d|%d|%" quote(MAX_NAME_CHARS) "[^|]|\n",
                  &score, &level, &day, &month, &year, name);
    if (res != 6) {
      print_message ("Score file corrupted");
      err = 1;
    }
    highscore[i].score = score;
    highscore[i].level = level;
    highscore[i].date = convert_old_date (day, month, year);
    highscore[i].name[0] = '\0';
    highscore[i].new = 0;
    strncat (highscore[i].name, name, MAX_NAME_CHARS);
  }
  qsort (highscore, HIGHSCORE_SLOTS, sizeof (struct score_entry),
         compare_entries);
  refill_old_entries ();

  return  err;
}

static int
read_version3_data (FILE *score_file)
/* Read the highscore list from SCORE_FILE into `highscore'.
 * Return 1 on success and 0 on error.  */
{
  int  err = 0;
  int  i, res;

  for (i=0; i<HIGHSCORE_SLOTS && ! err; ++i) {
    int  score, level;
    char  date [MAX_DATE_CHARS];
    char  name [MAX_NAME_CHARS+1];
    res = fscanf (score_file,
                  "|%d|%d|%" quote(MAX_DATE_CHARS) "[^|]"
                  "|%" quote(MAX_NAME_CHARS) "[^|]|\n",
                  &score, &level, date, name);
    if (res != 4) {
      print_message ("Score file corrupted");
      err = 1;
    }
    highscore[i].score = score;
    highscore[i].level = level;
    highscore[i].date = parse_date (date);
    highscore[i].name[0] = '\0';
    highscore[i].new = 0;
    strncat (highscore[i].name, name, MAX_NAME_CHARS);
  }
  if (ferror (score_file)) {
    print_message ("Error while reading score file");
    err = 1;
  }

  return  err;
}

static int
read_data (FILE *score_file)
/* Read the highscore list from SCORE_FILE into `highscore'.
 * Return 1 on success and 0 on error.  */
{
  int  version;
  int  err = 0;
  int  res;

  res = fscanf (score_file, "moon-buggy hiscore file (version %d)\n",
                &version);
  if (res != 1) {
    print_message ("Score file corrupted");
    return  0;
  }
  highscore_changed = 0;

  switch (version) {
  case 2:
    err = read_version2_data (score_file);
    break;
  case 3:
    err = read_version3_data (score_file);
    break;
  default:
    {
      char buffer [80];
      sprintf (buffer, "Invalid score file version %d", version);
      print_message (buffer);
    }
    err = 1;
    break;
  }
  if (ferror (score_file)) {
    print_message ("Error while reading score file");
    err = 1;
  }

  return  ! err;
}

static void
write_data (FILE *score_file)
/* Write out the current highscore list to stream SCORE_FILE.  */
{
  int  i;
  int  res;

  res = fprintf (score_file, "moon-buggy hiscore file (version 3)\n");
  if (res < 0)  fatal ("Score file write error (%s)", strerror (errno));

  for (i=0; i<HIGHSCORE_SLOTS; ++i) {
    char  date [MAX_DATE_CHARS];

    format_date (date, highscore[i].date);
    res = fprintf (score_file,
                   "|%d|%d|%s|%." quote(MAX_NAME_CHARS) "s|\n",
                   highscore[i].score, highscore[i].level, date,
                   highscore[i].name);
    if (res < 0)  fatal ("Score file write error (%s)", strerror (errno));
  }

#if HAVE_FTRUNCATE
  {
    int  fd = fileno (score_file);
    long int  pos = ftell (score_file);
    if (pos != -1) {
#if HAVE_FCLEAN
      fclean (score_file);
#else
      fflush (score_file);
#endif
      ftruncate (fd, pos);
    }
  }
#endif
  highscore_changed = 0;
}

static int
do_open (const char *name, int flags, int lock, int must_succeed)
/* Try to open the file NAME using open flags FLAGS.
 * If LOCK ist 1, try to get a shared lock for the file.
 * If LOCK ist 2, try to get a exclusive lock for the file.
 * If MUST_SUCCEED is true, then abort on failure.
 * Otherwise return -1 if the file cannot be accessed.  */
{
  int  fd;
  int  lock_done;
  mode_t  mode, mask;

  lock_done = 1;
  if (lock == 1) {
#ifdef O_SHLOCK
    flags |= O_SHLOCK;
#else
    lock_done = 0;
#endif
  }
  if (lock == 2) {
#ifdef O_EXLOCK
    flags |= O_EXLOCK;
#else
    lock_done = 0;
#endif
  }

  mode = S_IWUSR|S_IRUSR|S_IRGRP|S_IROTH;
  if ( is_setgid () )  mode |= S_IWGRP;
  mask = umask (0);
  do {
    fd = open (name, flags, mode);
  } while (fd == -1 && errno == EINTR);
  if (fd == -1 && (must_succeed || (errno != EACCES && errno != ENOENT))) {
    fatal ("Cannot open score file \"%s\": %s", name, strerror (errno));
  }
  umask (mask);

  if (fd != -1 && ! lock_done) {
    struct flock  l;
    int  res;

    l.l_type = (lock == 1) ? F_RDLCK : F_WRLCK;
    l.l_whence = SEEK_SET;
    l.l_start = 0;
    l.l_len = 0;

    do {
      res = fcntl (fd, F_SETLKW, &l);
    } while (res == -1 && errno == EINTR);
    if (res == -1) {
      fatal ("Cannot lock score file \"%s\": %s", name, strerror (errno));
    }
  }

  return  fd;
}

static void
update_score_file (const struct score_entry *entry)
/* Modify the highscore file, to contain the data from *ENTRY.
 * This updates the array `highscores', too.
 * If ENTRY is NULL, only `highscores' is updated.  */
{
  char *local_name, *global_name;
  int  in_fd, out_fd;
  enum  persona  in_pers, out_pers;
  FILE *f;
  int  res, method;

  global_name = global_score_file_name ();
  local_name = local_score_file_name ();

  for (method=1; method<=4; ++method) {
    in_fd = out_fd = -1;

    switch (method) {
    case 1:
      /* method 1: try read/write access to global score file
       *           on success: in_fd == global, out_fd == global */
      in_pers = out_pers = pers_GAME;
      set_persona (pers_GAME);
      in_fd = out_fd = do_open (global_name, O_RDWR, 2, 0);
      break;
    case 2:
      /* method 2: try write access to global score file
       *           on success: in_fd == -1, out_fd == global */
      out_pers = pers_GAME;
      set_persona (pers_GAME);
      out_fd = do_open (global_name, O_WRONLY|O_CREAT, 2, 0);
      break;
    case 3:
      /* method 3: try read/write access to local score file
       *           on success: in_fd == local, out_fd == local */
      in_pers = out_pers = pers_USER;
      set_persona (pers_USER);
      in_fd = out_fd = do_open (local_name, O_RDWR, 2, 0);
      break;
    case 4:
      /* method 4: try write access to local score file and
       *               read access to global score file
       *           on success: in_fd == global or -1, out_fd == local */
      out_pers = pers_USER;
      set_persona (pers_USER);
      out_fd = do_open (local_name, O_WRONLY|O_CREAT, 2, 1);
      in_pers = pers_GAME;
      set_persona (pers_GAME);
      in_fd = do_open (global_name, O_RDONLY, 1, 0);
      break;
    }

    if (in_fd != -1) {
      int  read_success;

      set_persona (in_pers);
      f = fdopen (in_fd, in_fd==out_fd ? "r+" : "r");
      read_success = read_data (f);
      if (read_success) {
        if (in_fd != out_fd) {
          res = fclose (f);
          if (res == EOF)  fatal ("Score file read error (%s)", strerror (errno));
        } else {
          res = fseek (f, 0, SEEK_SET);
          if (res != 0)  fatal ("Score file seek error (%s)", strerror (errno));
        }
      } else {
        fclose (f);
        if (out_fd == in_fd)
          out_fd = -1;
        in_fd = -1;
      }
    }

    if (out_fd != -1)  break;
  }
  if (in_fd == -1)  generate_data ();

  refill_old_entries ();
  if (entry)  merge_entry (entry);
  refill_old_entries ();

  assert (out_fd != -1);
  if (in_fd != out_fd || highscore_changed) {
    set_persona (out_pers);
    if (in_fd != out_fd)  f = fdopen (out_fd, "w");
    write_data (f);
  }
  res = fclose (f);
  if (res == EOF)  fatal ("Score file write error (%s)", strerror (errno));

  set_persona (pers_USER);
  free (local_name);
  free (global_name);
}

void
create_highscores (void)
/* Make sure, that a highscore file exists.
 * This must be called on installation, to make setgid-usage work.  */
{
  block_all ();
  update_score_file (NULL);
  unblock ();
}

void
show_highscores (void)
/* Print the highscore list to stdout.
 * Do not use curses.  */
{
  time_t  now;
  int  i;

  block_all ();
  update_score_file (NULL);
  unblock ();

  now = time (NULL);
  puts ("rank   score lvl     date  expires  name");
  for (i=0; i<HIGHSCORE_SLOTS; ++i) {
    char  date [16];
    char  expire [16];
    double  dt;

    format_display_date (date, highscore[i].date);
    dt = difftime (expire_date (i, highscore[i].date), now);
    format_relative_time (expire, dt);
    printf ("%3d %8u %-3d  %s %s  %." quote(MAX_NAME_CHARS) "s\n",
            i+1, highscore[i].score, highscore[i].level, date, expire,
            highscore[i].name);
  }
}

static  int  highscore_valid, last_score, last_level;
static  int  gap;		/* number of omitted entries after pos 3 */
static  int  max_line;

static void
fix_gap (void)
/* Cast `gap' to valid value.  */
{
  int  limit = HIGHSCORE_SLOTS-(max_line-3);
  if (gap > limit)  gap = limit;
  if (gap < 2)  gap = 0;
}

static void
center_new (void)
/* Prepare `gap' in order to view values around `last_score'.  */
{
  int  i, pos;

  max_line = LINES-11;
  if (max_line > 25)  max_line = 25;

  for (i=1; i<HIGHSCORE_SLOTS; ++i) {
    if (highscore[i].score < last_score)  break;
  }
  --i;

  pos = 7+(max_line-6)/2;
  gap = 1 + (i+4) - pos;
  fix_gap ();
}

static void
print_scores (void)
/* Print the highscore table to the screen.  */
{
  time_t  now;
  int  i, line, my_rank;

  now = time (NULL);

  mvwaddstr (moon, 1, 5, "rank   score lvl     date  expires  name");
  line = 3;
  my_rank = -1;
  for (i=0; i<HIGHSCORE_SLOTS; ++i) {
    char  date [16];
    char  expire [16];
    double  dt;

    if (highscore[i].new) {
      if (highscore[i].score == last_score)  my_rank = i+1;
    }

    if ((i==3 && gap>0)
        || (i<HIGHSCORE_SLOTS-1 && line==max_line)) {
      mvwprintw (moon, line++, 5, "  ...");
      wclrtoeol (moon);
    }
    if ((i>=3 && i<3+gap) || line>max_line)  continue;

    format_display_date (date, highscore[i].date);
    dt = difftime (expire_date (i, highscore[i].date), now);
    format_relative_time (expire, dt);
    if (highscore[i].new)  wstandout (moon);
    mvwprintw (moon, line++, 5,
               "%3d %8u %-3d  %s %s  %." quote(MAX_NAME_CHARS) "s\n",
               i+1, highscore[i].score, highscore[i].level, date, expire,
               highscore[i].name);
    if (highscore[i].new)  wstandend (moon);
  }
  ++line;
  if (last_score > 0)
    mvwprintw (moon, line++, 17, "your score: %d", last_score);
  if (my_rank > 0) mvwprintw (moon, line++, 17, "your rank: %d", my_rank);
  wrefresh (moon);
}

void
score_set (int score, int level)
{
  last_score = score;
  last_level = level;
}

static void
enter_name_h (game_time t, void *client_data)
{
  struct score_entry  entry;
  int  res;

  entry.score = last_score;
  entry.level = last_level;
  entry.date = time (NULL);
 retry:
  entry.name[0] = '\0';
  res = get_real_user_name (entry.name, MAX_NAME_CHARS);
  if (res == ERR || ! entry.name[0]) {
    mode_redraw ();
    goto retry;
  }
  entry.new = 1;

  print_message ("writing score file ...");
  doupdate ();
  block_all ();
  update_score_file (&entry);
  unblock ();
  mode_redraw ();
}

static void
highscore_enter (int seed)
{
  print_ground ();
  print_buggy ();

  print_message ("loading score file ...");
  doupdate ();
  block_all ();
  update_score_file (NULL);
  highscore_valid = 1;
  unblock ();
  center_new ();

  if (last_score > highscore[HIGHSCORE_SLOTS-1].score) {
    add_event (0.0, enter_name_h, NULL);
  }
}

static void
highscore_leave (void)
{
  print_game_over (0);
  last_score = last_level = 0;
}

void
highscore_redraw (void)
{
  resize_meteors ();
  resize_ground (0);

  max_line = LINES-11;
  if (max_line > 25)  max_line = 25;
  fix_gap ();

  print_ground ();
  adjust_score (0);
  print_lives ();
  print_buggy ();
  if (last_level > 0)  print_game_over (1);
  if (highscore_valid)  print_scores ();
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
    if (highscore_valid) {
      if (gap > 2) --gap; else gap = 0;
      print_scores ();
    }
    break;
  case 4:
    if (highscore_valid) {
      if (gap < 2) gap = 2; else ++gap;
      fix_gap ();
      print_scores ();
    }
    break;
  case 5:
    if (highscore_valid) {
      gap -= max_line-7;
      fix_gap ();
      print_scores ();
    }
    break;
  case 6:
    if (highscore_valid) {
      gap += max_line-7;
      fix_gap ();
      print_scores ();
    }
    break;
  case 7:
    print_message ("reloading score file ...");
    doupdate ();
    block_all ();
    update_score_file (NULL);
    highscore_valid = 1;
    unblock ();
    center_new ();
    mode_redraw ();
    break;
  }
}

void
setup_highscore_mode (void)
{
  highscore_mode = new_mode ();
  highscore_mode->enter = highscore_enter;
  highscore_mode->leave = highscore_leave;
  highscore_mode->redraw = highscore_redraw;
  highscore_mode->keypress = key_handler;
  mode_add_key (highscore_mode, mbk_start, "new game", 1);
  mode_add_key (highscore_mode, mbk_end, "quit", 2);
  mode_add_key (highscore_mode, mbk_up, "up", 3);
  mode_add_key (highscore_mode, mbk_down, "down", 4);
  mode_add_key (highscore_mode, mbk_pageup, "pg up", 5);
  mode_add_key (highscore_mode, mbk_pagedown, "pg down", 6);
  mode_add_key (highscore_mode, mbk_scores, "reload", 7);
  mode_complete (highscore_mode);
}
