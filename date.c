/* date.c - handle the highscore list's date entries
 *
 * Copyright (C) 2000  Jochen Voss.  */

static const  char  rcsid[] = "$Id: date.c,v 1.1 2000/01/03 14:42:06 voss Exp $";


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>

#include "moon-buggy.h"


time_t
parse_date (const char *str)
/* Read the date from STR and convert it to a `time_t'.
 * This is used to decode highscore entries.  */
{
  struct tm  broken;

  sscanf (str, "%d-%d-%d %d:%d:%d",
	  &broken.tm_year, &broken.tm_mon, &broken.tm_mday,
	  &broken.tm_hour, &broken.tm_min, &broken.tm_sec);
  broken.tm_year -= 1900;
  broken.tm_mon -= 1;
  broken.tm_isdst = -1;

  return  mktime (&broken);
}

time_t
convert_old_date (int day, int month, int year)
{
  struct tm  broken;

  broken.tm_year = year-1900;
  broken.tm_mon = month-1;
  broken.tm_mday = day;
  broken.tm_hour = 12;
  broken.tm_min = 0;
  broken.tm_sec = 0;

  return  mktime (&broken);
}

void
format_date (char *buffer, time_t date)
/* Into the BUFFER format a textual representation of DATE.
 * Buffer must contain at least MAX_DATE_CHARS characters.
 * The filled-in string is feasible for parsing with `parse_date'.  */
{
  struct tm *loctime;
  
  loctime = localtime (&date);
  sprintf (buffer, "%d-%d-%d %d:%d:%d",
	   loctime->tm_year+1900, loctime->tm_mon+1, loctime->tm_mday,
	   loctime->tm_hour, loctime->tm_min, loctime->tm_sec);
}

void
format_display_date (char *buffer, time_t date)
/* Into the BUFFER format a textual representation of DATE.
 * Buffer must contain at least 11 characters.  The filled-in string
 * is meant to be part of the displayed highscore list.  */
{
  struct tm *loctime;
  
  loctime = localtime (&date);
  sprintf (buffer, "%4d-%02d-%02d",
	   loctime->tm_year+1900, loctime->tm_mon+1, loctime->tm_mday);
}
