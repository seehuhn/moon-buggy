/* getdate.c - get the current date
 *
 * Copyright 1999  Jochen Voss.  */

static const  char  rcsid[] = "$Id: getdate.c,v 1.2 1999/06/17 14:45:31 voss Rel $";

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <time.h>


void
get_date (int *day_p, int *month_p, int *year_p)
/* Place the current date in *DAY_P, *MONTH_P, and *YEAR_P.  */
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
