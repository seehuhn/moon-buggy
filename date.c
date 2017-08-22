/* date.c - handle the highscore list's date entries
 *
 * Copyright (C) 2000  Jochen Voss.  */

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
  if (loctime) {
    sprintf (buffer, "%d-%d-%d %d:%d:%d",
             loctime->tm_year+1900, loctime->tm_mon+1, loctime->tm_mday,
             loctime->tm_hour, loctime->tm_min, loctime->tm_sec);
  } else {
    sprintf (buffer, "sometimes");
  }
}

void
format_display_date (char *buffer, time_t date)
/* Into the BUFFER format a textual representation of DATE.
 * Buffer must contain at least 11 characters.  The filled-in string
 * is meant to be part of the displayed highscore list.  */
{
  struct tm *loctime;

  loctime = localtime (&date);
  if (loctime) {
    sprintf (buffer, "%4d-%02d-%02d",
             loctime->tm_year+1900, loctime->tm_mon+1, loctime->tm_mday);
  } else {
    sprintf (buffer, "sometimes");
  }
}

void
format_relative_time (char *buffer, double dt)
/* Into the BUFFER format a textual representation of time period DT.
 * Buffer must contain at least 5 characters.  The filled-in string
 * is meant to be part of the displayed highscore list.  */
{
  double  hour = 60*60;
  double  day = 24*hour;

  if (dt <= 0) {
    sprintf (buffer, "soon");
  } else if (dt > 999*day) {
    sprintf (buffer, " -- ");
  } else if (dt >= day) {
    sprintf (buffer, "%3dd", (int)(dt/day+0.5));
  } else if (dt >= hour) {
    sprintf (buffer, "%3dh", (int)(dt/hour+0.5));
  } else {
    sprintf (buffer, "%3dm", (int)(dt/60+0.5));
  }
}
