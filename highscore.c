/* highscore.c - maintain the highscore list
 *
 * Copyright 1999  Jochen Voss  */

static const  char  rcsid[] = "$Id: highscore.c,v 1.17 1999/06/02 07:06:28 voss Exp $";

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#define _XOPEN_SOURCE_EXTENDED 1

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

#include "mbuggy.h"


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

  lock_done = 1;
  if (lock == 1) {
#ifdef O_SHLOCK
    flags |= O_SHLOCK;
#else
    lock_done = 0;
#endif
  }
  if (lock == 2) {
#ifdef O_SHLOCK
    flags |= O_EXLOCK;
#else
    lock_done = 0;
#endif
  }
  
  do {
    fd = open (name, flags, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
  } while (fd == -1 && errno == EINTR);
  if (fd == -1 && (must_succeed || (errno != EACCES && errno != ENOENT))) {
    fatal ("Cannot open score file \"%s\": %s", name, strerror (errno));
  }

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

#define SCORE_FILE "mbscore"
#define NAME_MAX_LEN 40
#define TOPTEN_SLOTS 10

#define qx(str) #str
#define quote(str) qx(str)


/* The top ten list, as read by `read_data'.  */
struct score_entry {
  int  score, level;
  int  day, month, year;
  char  name [NAME_MAX_LEN+1];
  int  new;
};
static  struct score_entry  topten [TOPTEN_SLOTS];


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
 * The is reponsible for freeing the returned string via `free'.  */
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


static void
generate_data (void)
/* Generate an empty top-ten table and write it into `topten'.  */
{
  static const char *names [13] = {
    "Dwalin", "Balin", "Kili", "Fili", "Dori", "Nori", "Ori",
    "Oin", "Gloin", "Bifur", "Bofur", "Bombur", "Thorin"
  };
  int  day, month, year;
  int  i;

  get_date (&day, &month, &year);
  for (i=0; i<TOPTEN_SLOTS; ++i) {
    topten[i].score = 100*(TOPTEN_SLOTS-i);
    topten[i].level = 1;
    topten[i].day = day;
    topten[i].month = month;
    topten[i].year = year;
    strcpy (topten[i].name, names[uniform_rnd(13)]);
    topten[i].new = 0;
  }
}


static void
read_data (FILE *score_file)
/* Read the top ten list from SCORE_FILE into `topten'.  */
{
  int  version, i;
  int  res;
  
  res = fscanf (score_file, "moon-buggy hiscore file (version %d)\n",
		&version);
  if (res != 1)  fatal ("Score file corrupted");
  if (version != 2)  fatal ("Wrong save file version %d", version);

  for (i=0; i<TOPTEN_SLOTS; ++i) {
    int  score, level;
    int  day, month, year;
    char  name [NAME_MAX_LEN+1];
    res = fscanf (score_file,
		  "|%d|%d|%d|%d|%d|%" quote(NAME_MAX_LEN) "[^|]|\n",
		  &score, &level, &day, &month, &year, name);
    if (res != 6)  fatal ("Score file corrupted");
    topten[i].score = score;
    topten[i].level = level;
    topten[i].day = day;
    topten[i].month = month;
    topten[i].year = year;
    topten[i].name[0] = '\0';
    topten[i].new = 0;
    strncat (topten[i].name, name, NAME_MAX_LEN);
  }
}

static void
write_data (FILE *score_file, int trunc)
/* Write out the current top ten list to stream SCORE_FILE.
 * If TRUNC is true, try to truncate the file.  */
{
  int  i;
  int  res;

  res = fprintf (score_file, "moon-buggy hiscore file (version 2)\n");
  if (res < 0)  fatal ("Score file write error (%s)", strerror (errno));

  for (i=0; i<TOPTEN_SLOTS; ++i) {
    res = fprintf (score_file,
		   "|%d|%d|%d|%d|%d|%." quote(NAME_MAX_LEN) "s|\n",
		   topten[i].score, topten[i].level,
		   topten[i].day, topten[i].month, topten[i].year,
		   topten[i].name);
    if (res < 0)  fatal ("Score file write error (%s)", strerror (errno));
  }

#if HAVE_FTRUNCATE
    if (trunc) {
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

static int
merge_entry (const struct score_entry *entry)
{
  struct score_entry *last;

  last = topten+(TOPTEN_SLOTS-1);
  
  if (entry->score > last->score) {
    memcpy (last, entry, sizeof (struct score_entry));
    qsort (topten, TOPTEN_SLOTS, sizeof (struct score_entry),
	   compare_entries);
    return  1;
  } else {
    return  0;
  }
}

static void
load_table (void)
/* Load the top-ten list into the variable `topten'.  */
{
  char *local_name, *global_name;
  int  in_fd, out_fd;
  enum  persona  in_pers, out_pers;
  FILE *f;
  int  res;

  global_name = global_score_file_name ();
  local_name = local_score_file_name ();
  out_pers = pers_USER;
  out_fd = -1;

  do {
    /* step 1: try read/write access to global score file */
    set_persona (pers_GAME);
    in_pers = pers_GAME;
    in_fd = do_open (global_name, O_RDWR, 2, 0);
    if (in_fd != -1)  break;
  
    /* step 2: try write access to global score file */
    out_pers = pers_GAME;
    out_fd = do_open (global_name, O_WRONLY|O_CREAT, 2, 0);
    if (out_fd != -1)  break;

    /* step 3: try read/write access to local score file */
    set_persona (pers_USER);
    in_pers = pers_USER;
    in_fd = do_open (local_name, O_RDWR, 2, 0);
    if (in_fd != -1)  break;
  
    /* step 4: try write access to local score file and
     *	     read access to global score file */
    out_pers = pers_USER;
    out_fd = do_open (local_name, O_WRONLY|O_CREAT, 2, 1);
    set_persona (pers_GAME);
    in_pers = pers_GAME;
    in_fd = do_open (global_name, O_RDONLY, 1, 0);
  } while (0);

  if (in_fd != -1) {
    assert (in_fd != out_fd);
    set_persona (in_pers);
    f = fdopen (in_fd, "r");
    read_data (f);
    res = fclose (f);
    if (res == EOF)  fatal ("Score file read error (%s)", strerror (errno));
  } else {
    generate_data ();
  }

  if (out_fd != -1) {
    set_persona (out_pers);
    f = fdopen (out_fd, "w");
    write_data (f, 0);
    res = fclose (f);
    if (res == EOF)  fatal ("Score file write error (%s)", strerror (errno));
  }
  
  set_persona (pers_USER);
  free (local_name);
  free (global_name);
}

static void
modify_table (const struct score_entry *entry)
/* Modify the top-ten list, to contain the data from *ENTRY.  */
{
  char *local_name, *global_name;
  int  in_fd, out_fd;
  enum  persona  in_pers, out_pers;
  FILE *f;
  int  res, changed;

  global_name = global_score_file_name ();
  local_name = local_score_file_name ();

  do {
    /* step 1: try read/write access to global score file */
    set_persona (pers_GAME);
    in_pers = out_pers = pers_GAME;
    in_fd = out_fd = do_open (global_name, O_RDWR, 2, 0);
    if (in_fd != -1)  break;
  
    /* step 2: try write access to global score file */
    out_pers = pers_GAME;
    out_fd = do_open (global_name, O_WRONLY|O_CREAT, 2, 0);
    if (out_fd != -1)  break;

    /* step 3: try read/write access to local score file */
    set_persona (pers_USER);
    in_pers = out_pers = pers_USER;
    in_fd = out_fd = do_open (local_name, O_RDWR, 2, 0);
    if (in_fd != -1)  break;
  
    /* step 4: try write access to local score file and
     *	     read access to global score file */
    out_pers = pers_USER;
    out_fd = do_open (local_name, O_WRONLY|O_CREAT, 2, 1);
    set_persona (pers_GAME);
    in_pers = pers_GAME;
    in_fd = do_open (global_name, O_RDONLY, 1, 0);
  } while (0);

  if (in_fd != -1) {
    set_persona (in_pers);
    f = fdopen (in_fd, in_fd==out_fd ? "r+" : "r");
    read_data (f);
    if (in_fd != out_fd) {
      res = fclose (f);
      if (res == EOF)  fatal ("Score file read error (%s)", strerror (errno));
    } else {
      res = fseek (f, 0, SEEK_SET);
      if (res != 0)  fatal ("Score file seek error (%s)", strerror (errno));
    }
  } else {
    generate_data ();
  }

  changed = merge_entry (entry);

  assert (out_fd != -1);
  set_persona (out_pers);
  if (changed) {
    if (in_fd != out_fd)  f = fdopen (out_fd, "w");
    write_data (f, 0);
  }
  res = fclose (f);
  if (res == EOF)  fatal ("Score file write error (%s)", strerror (errno));
  
  set_persona (pers_USER);
  free (local_name);
  free (global_name);
}

static int  topten_valid, last_score;

static void
print_scores (int my_score)
/* Print the top ten table to the screen.  */
{
  int  i;

  mvwaddstr (moon, 1, NAME_MAX_LEN-4, "name     date     level score");
  for (i=0; i<TOPTEN_SLOTS; ++i) {
    if (topten[i].new)  wstandout (moon);
    mvwprintw (moon, 3+i, 0,
	       "%" quote(NAME_MAX_LEN) "s%c %4d-%02d-%02d  %3d   %-12lu",
	       topten[i].name, topten[i].new ? '*' : ' ',
	       topten[i].year, topten[i].month, topten[i].day,
	       topten[i].level, topten[i].score);
    if (topten[i].new)  wstandend (moon);
  }
  mvwprintw (moon, 3+TOPTEN_SLOTS+1, NAME_MAX_LEN+2, "your score: %d",
	     my_score);
  wrefresh (moon);
}

int
highscore_mode (int score, int level)
{
  int  done = 0;
  int  again = 1;

  game_state = HIGHSCORE;
  
  print_message ("loading score file ...");
  doupdate ();
  block_all ();
  load_table ();
  topten_valid = 1;
  last_score = score;
  unblock ();
  print_scores (score);

  if (score > topten[TOPTEN_SLOTS-1].score) {
    struct score_entry  entry;
    entry.score = score;
    entry.level = level;
    get_date (&entry.day, &entry.month, &entry.year);
    entry.name[0] = '\0';
    get_real_user_name (entry.name, NAME_MAX_LEN);
    entry.new = 1;
  
    print_message ("writing score file ...");
    doupdate ();
    block_all ();
    modify_table (&entry);
    unblock ();
    print_scores (score);
  }

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
  if (topten_valid) {
    print_message ("play again (y/n)?");
    print_scores (last_score);
  }
}
