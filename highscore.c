/* highscore.c - maintain the high score list
 *
 * Copyright (C) 1998  Jochen Voss.  */

static const  char  rcsid[] = "$Id: highscore.c,v 1.1 1998/12/19 20:33:54 voss Exp $";


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "moon.h"


#define SCORE_FILE "mbscore"
#define PLAYER_MAX_LEN 40
#define HIGHSCORE_SLOTS 10

#define qx(str) #str
#define quote(str) qx(str)

struct score_entry {
  unsigned long  score;
  int  day, month, year;
  char  player [PLAYER_MAX_LEN+1];
  int  new;
};
static  struct score_entry  hiscores [HIGHSCORE_SLOTS];
static  char *score_file_name;

static void
find_table (void)
{
  score_file_name = SCORE_FILE;
}

static void
read_table (void)
{
  FILE *score_file;
  int  version, i;
  int  res;
  
  score_file = fopen (score_file_name, "r");
  if (! score_file) {
    fatal ("cannot read score file \"%s\": %s",
	   score_file_name, strerror (errno));
  }

  res = fscanf (score_file, "moon-buggy hiscore file (version %d)\n", &version);
  if (res != 1)  fatal ("score file corrupted");
  if (version != 1)  fatal ("unknown save file version %d", version);

  for (i=0; i<HIGHSCORE_SLOTS; ++i) {
    unsigned long  score;
    int  day, month, year;
    char  name [PLAYER_MAX_LEN+1];
    res = fscanf (score_file, "|%lu|%d|%d|%d|%" quote(PLAYER_MAX_LEN) "[^|]|\n",
		  &score, &day, &month, &year, name);
    if (res != 5)  fatal ("score file corrupted: %d", res);
    hiscores[i].score = score;
    hiscores[i].day = day;
    hiscores[i].month = month;
    hiscores[i].year = year;
    hiscores[i].player[0] = '\0';
    hiscores[i].new = 0;
    strncat (hiscores[i].player, name, PLAYER_MAX_LEN);
  }
  
  res = fclose (score_file);
  if (res == EOF) {
    fatal ("cannot read score file \"%s\": %s",
	   score_file_name, strerror (errno));
  }
}

static void
write_table (void)
{
  FILE *score_file;
  int  i;
  int  res;
  
  score_file = fopen (score_file_name, "w");
  if (! score_file) {
    fatal ("cannot write score file \"%s\": %s",
	   score_file_name, strerror (errno));
  }

  res = fprintf (score_file, "moon-buggy hiscore file (version 1)\n");
  if (res < 0) {
    fatal ("cannot write score file \"%s\": %s",
	   score_file_name, strerror (errno));
  }

  for (i=0; i<HIGHSCORE_SLOTS; ++i) {
    fprintf (score_file, "|%lu|%d|%d|%d|%." quote(PLAYER_MAX_LEN) "s|\n",
	     hiscores[i].score,
	     hiscores[i].day, hiscores[i].month, hiscores[i].year,
	     hiscores[i].player);
  }
  
  res = fclose (score_file);
  if (res == EOF) {
    fatal ("cannot write score file \"%s\": %s",
	   score_file_name, strerror (errno));
  }
}

static void
print_scores ()
{
  int  i;

  mvwaddstr (moon, 1, PLAYER_MAX_LEN-4, "name     date     score");
  for (i=0; i<HIGHSCORE_SLOTS; ++i) {
    if (hiscores[i].new)  wstandout (moon);
    mvwprintw (moon, 3+i, 0, "%" quote(PLAYER_MAX_LEN) "s%c %4d-%02d-%02d  %lu",
	       hiscores[i].player, hiscores[i].new ? '*' : ' ',
	       hiscores[i].year, hiscores[i].month, hiscores[i].day,
	       hiscores[i].score);
    if (hiscores[i].new)  wstandend (moon);
  }
  mvwprintw (moon, 3+HIGHSCORE_SLOTS+1, PLAYER_MAX_LEN+2, "your score: %d",
	     score);
  wrefresh (moon);
}

static int
compare_entries (const void *a, const void *b)
{
  const  struct score_entry *aa = a;
  const  struct score_entry *bb = b;
  if (aa->score > bb->score)  return -1;
  if (aa->score < bb->score)  return +1;
  return  0;
}

static char  real_name [PLAYER_MAX_LEN];

void
write_hiscore (void)
{
  struct score_entry *new_entry;
  int  i;
  
  find_table ();
  read_table ();
  
  new_entry = hiscores+(HIGHSCORE_SLOTS-1);
  if (score > new_entry->score) {
    get_real_user_name (real_name, PLAYER_MAX_LEN);
    strncpy (new_entry->player, real_name, PLAYER_MAX_LEN);
    for (i=0; i<PLAYER_MAX_LEN; ++i) {
      if (new_entry->player[i] == '|')  new_entry->player[i] = ';';
    }
    new_entry->score = score;
    new_entry->new = 1;
    qsort (hiscores, HIGHSCORE_SLOTS, sizeof (struct score_entry),
	   compare_entries);
  }

  write_table ();
  
  print_scores ();
}
