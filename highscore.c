/* highscore.c - maintain the highscore list
 *
 * Copyright 1999  Jochen Voss  */

static const  char  rcsid[] = "$Id: highscore.c,v 1.15 1999/05/25 15:35:36 voss Exp $";

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#define _XOPEN_SOURCE_EXTENDED 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <fcntl.h>
#include <time.h>
#if HAVE_ERRNO_H
#include <errno.h>
#else
extern  int  errno;
#endif

#if defined(__hp9000s800)
#include <stdarg.h>
#endif

#include "mbuggy.h"


#define SCORE_FILE "mbscore"
#define PLAYER_MAX_LEN 40
#define HIGHSCORE_SLOTS 10

#define qx(str) #str
#define quote(str) qx(str)


/* The top ten list, as read by `read_table'.  */
struct score_entry {
  long  score;
  int  day, month, year;
  char  player [PLAYER_MAX_LEN+1];
  int  new;
};
static  struct score_entry  hiscores [HIGHSCORE_SLOTS];
static  int  highscore_valid = 0;

/* The name of the score file to write.  */
static  char *score_file_name;

static long  my_score;


static void
get_current_date (int *day_p, int *month_p, int *year_p)
{
  time_t curtime;
  struct tm *loctime;
  
  /* Get the current time. */
  curtime = time (NULL);
  
  /* Convert it to local time representation. */
  loctime = localtime (&curtime);

  *day_p = loctime->tm_mday;
  *month_p = loctime->tm_mon + 1;
  *year_p = loctime->tm_year + 1900;
}

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

static int
do_open (const char *name, int flags, int must_succeed)
/* Try to open the file NAME using open flags FLAGS.
 * If MUST_SUCCEED is true, then abort on failure.
 * Otherwise return -1 if the file cannot be accessed.  */
{
  int  res;
  
  do {
    res = open (name, flags, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
  } while (res == -1 && errno == EINTR);
  if (res == -1 && (must_succeed || (errno != EACCES && errno != ENOENT))) {
    fatal ("Cannot open score file \"%s\": %s", name, strerror (errno));
  }
  return  res;
}

static void
do_lock (int fd, int type, const char *name)
/* Try to get a file lock on file descriptor FD with lock type TYPE.
 * The string NAME is used to construct error messages only.  It
 * should be the file name associated with FD.  */
{
  struct flock  l;
  int  res;

  l.l_type = type;
  l.l_whence = SEEK_SET;
  l.l_start = 0;
  l.l_len = 0;

  do {
    res = fcntl (fd, F_SETLKW, &l);
  } while (res == -1 && errno == EINTR);
#if 0
  if (res == -1) {
    fatal ("Cannot lock score file \"%s\": %s", name, strerror (errno));
  }
#endif
}

static void
find_tables (int *in_fd, int *out_fd)
/* Locate the highscore files for reading and writing (and try to lock them).
 * The file descriptor to read from is placed in *IN_FD.  A value of
 * -1 directs the caller to create an empty table.  The modified table
 * should be written to file descriptor *OUT_FD.  Note the *IN_FD and
 * *OUT_FD may be equal.  */
{
  char *global_name;
  uid_t me;
  struct passwd *my_passwd;
  int  res;

  global_name = compose_filename (score_dir, SCORE_FILE);

  /* step 1: try read/write access to global score file */
  set_game_persona ();
  res = do_open (global_name, O_RDWR, 0);
  set_user_persona ();
  if (res >= 0) {
    do_lock (res, F_WRLCK, global_name);
    *in_fd = *out_fd = res;
    return;
  }
  
  /* step 2: try write access to global score file */
  set_game_persona ();
  res = do_open (global_name, O_WRONLY|O_CREAT, 0);
  set_user_persona ();
  if (res >= 0) {
    do_lock (res, F_WRLCK, global_name);
    *in_fd = -1;
    *out_fd = res;
    return;
  }

  /* construct the local score file name */
  me = getuid ();
  my_passwd = getpwuid (me);
  if (my_passwd && my_passwd->pw_dir) {
    score_file_name = compose_filename (my_passwd->pw_dir, "." SCORE_FILE);
  } else {
    score_file_name = compose_filename (NULL, "." SCORE_FILE);
  }

  /* step 3: try read/write access to local score file */
  res = do_open (score_file_name, O_RDWR, 0);
  if (res >= 0) {
    do_lock (res, F_WRLCK, score_file_name);
    *in_fd = *out_fd = res;
    free (global_name);
    return;
  }
  
  /* step 4: try write access to local score file */
  res = do_open (score_file_name, O_WRONLY|O_CREAT, 1);
  do_lock (res, F_WRLCK, score_file_name);
  *out_fd = res;
  
  /* step 4a: try read access to global score file */
  set_game_persona ();
  res = do_open (global_name, O_RDONLY, 0);
  set_user_persona ();
  if (res >= 0) {
    do_lock (res, F_RDLCK, global_name);
    *in_fd = res;
  } else {
    *in_fd = -1;
  }
  free (global_name);
}

static void
read_table (FILE *score_file)
/* Read the top ten list from SCORE_FILE.  */
{
  int  version, i;
  int  res;
  
  res = fscanf (score_file, "moon-buggy hiscore file (version %d)\n",
		&version);
  if (res != 1)  fatal ("Score file corrupted");
  if (version != 1)  fatal ("Unknown save file version %d", version);

  for (i=0; i<HIGHSCORE_SLOTS; ++i) {
    long  score;
    int  day, month, year;
    char  name [PLAYER_MAX_LEN+1];
    res = fscanf (score_file,
		  "|%ld|%d|%d|%d|%" quote(PLAYER_MAX_LEN) "[^|]|\n",
		  &score, &day, &month, &year, name);
    if (res != 5)  fatal ("Score file corrupted: %d", res);
    hiscores[i].score = score;
    hiscores[i].day = day;
    hiscores[i].month = month;
    hiscores[i].year = year;
    hiscores[i].player[0] = '\0';
    hiscores[i].new = 0;
    strncat (hiscores[i].player, name, PLAYER_MAX_LEN);
  }
}

/* A list of names for use in newly created highscore lists.  */
static const char *names [13] = {
  "Dwalin",
  "Balin",
  "Kili",
  "Fili",
  "Dori",
  "Nori",
  "Ori",
  "Oin",
  "Gloin",
  "Bifur",
  "Bofur",
  "Bombur",
  "Thorin"
};

static void
generate_table (void)
/* Generate an empty highscore table.  */
{
  int  day, month, year;
  int  i;

  get_current_date (&day, &month, &year);
  for (i=0; i<HIGHSCORE_SLOTS; ++i) {
    hiscores[i].score = 200*(HIGHSCORE_SLOTS-i);
    hiscores[i].day = day;
    hiscores[i].month = month;
    hiscores[i].year = year;
    strcpy (hiscores[i].player, names[uniform_rnd(13)]);
    hiscores[i].new = 0;
  }
}

static void
write_table (FILE *score_file)
/* Write out the current top ten list to file SCORE_FILE.  */
{
  int  i;
  int  res;

  res = fprintf (score_file, "moon-buggy hiscore file (version 1)\n");
  if (res < 0) {
    fatal ("Cannot write score file \"%s\": %s",
	   score_file_name, strerror (errno));
  }

  for (i=0; i<HIGHSCORE_SLOTS; ++i) {
    fprintf (score_file, "|%ld|%d|%d|%d|%." quote(PLAYER_MAX_LEN) "s|\n",
	     hiscores[i].score,
	     hiscores[i].day, hiscores[i].month, hiscores[i].year,
	     hiscores[i].player);
  }
}

static void
print_scores (void)
/* Print the top ten table to the screen.  */
{
  int  i;

  mvwaddstr (moon, 1, PLAYER_MAX_LEN-4, "name     date     score");
  for (i=0; i<HIGHSCORE_SLOTS; ++i) {
    if (hiscores[i].new)  wstandout (moon);
    mvwprintw (moon, 3+i, 0,
	       "%" quote(PLAYER_MAX_LEN) "s%c %4d-%02d-%02d  %-12lu",
	       hiscores[i].player, hiscores[i].new ? '*' : ' ',
	       hiscores[i].year, hiscores[i].month, hiscores[i].day,
	       hiscores[i].score);
    if (hiscores[i].new)  wstandend (moon);
  }
  mvwprintw (moon, 3+HIGHSCORE_SLOTS+1, PLAYER_MAX_LEN+2, "your score: %d",
	     my_score);
  wrefresh (moon);
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

static char  real_name [PLAYER_MAX_LEN];

static void
write_scores (void)
/* Load the top ten table and add the current game if appropriate.  */
{
  struct score_entry *new_entry;
  int  in_fd, out_fd;
  FILE *in, *out;
  int  changed = 0;
  int  i, res;
  
  find_tables (&in_fd, &out_fd);
  if (in_fd >= 0) {
    if (in_fd == out_fd) {
      in = out = fdopen (in_fd, "r+");
      if (in == NULL)
	fatal ("fdopen failed on score file: %s", strerror (errno));
    } else {
      in = fdopen (in_fd, "r");
      if (in == NULL)
	fatal ("fdopen failed on global score file: %s", strerror (errno));
      out = fdopen (out_fd, "w");
      if (out == NULL)
	fatal ("fdopen failed on local score file: %s", strerror (errno));
    }
  } else {
    in = NULL;
    out = fdopen (out_fd, "w");
    if (out == NULL)
      fatal ("fdopen failed on score file: %s", strerror (errno));
  }
    
  if (in_fd >= 0) {
    read_table (in);
    if (in_fd != out_fd) {
      res = fclose (in);
    } else {
      fseek (out, 0, SEEK_SET);
    }
  } else {
    generate_table ();
    changed = 1;
  }
  
  new_entry = hiscores+(HIGHSCORE_SLOTS-1);
  if (my_score > new_entry->score) {
    int  day, month, year;

    get_real_user_name (real_name, PLAYER_MAX_LEN);
    strncpy (new_entry->player, real_name, PLAYER_MAX_LEN);
    for (i=0; i<PLAYER_MAX_LEN; ++i) {
      if (new_entry->player[i] == '|')  new_entry->player[i] = ';';
    }
    get_current_date (&day, &month, &year);
    new_entry->score = my_score;
    new_entry->day = day;
    new_entry->month = month;
    new_entry->year = year;
    new_entry->new = 1;
    qsort (hiscores, HIGHSCORE_SLOTS, sizeof (struct score_entry),
	   compare_entries);
    changed = 1;
  }

  if (changed) {
    write_table (out);

#if HAVE_FTRUNCATE
    if (in_fd == out_fd) {
      long int  pos = ftell (out);
      if (pos != -1) {
#if HAVE_FCLEAN
	fclean (out);
#else
	fflush (out);
#endif
	ftruncate (out_fd, pos);
      }
    }
#endif
  }
  
  res = fclose (out);
  if (res == EOF) {
    fatal ("cannot write score file \"%s\": %s",
	   score_file_name, strerror (errno));
  }
}

int
highscore_mode (long score)
{
  int  done = 0;
  int  again = 1;

  my_score = score;
  game_state = HIGHSCORE;
  
  block_all ();
  print_message ("loading score file ...");
  doupdate ();
  write_scores ();
  highscore_valid = 1;
  unblock ();
  
  print_scores ();

  do {
    print_message ("play again (y/n)?");
    doupdate ();
    switch (xgetch (message)) {
    case KEY_ENTER:
    case 'y':
    case ' ':
      done = 1;
      break;
    case KEY_BREAK:
    case KEY_CLOSE:
    case 27:			/* ESC */
    case 'n':
    case 'q':
      again = 0;
      done = 1;
      break;
    default:
      beep ();
      doupdate ();
    }
  } while (! done);

  return  again;
}

void
resize_highscore (void)
{
  resize_game ();
  if (highscore_valid) {
    print_message ("play again (y/n)?");
    print_scores ();
  }
}
