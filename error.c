/* error.c - functions to signal error conditions
 *
 * Copyright (C) 1998  Jochen Voss.  */

static const  char  rcsid[] = "$Id: error.c,v 1.4 1999/01/01 18:05:37 voss Exp $";

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <unistd.h>

#include "moon.h"


void
fatal (const char *format, ...)
/* Signal a fatal error and quit immediately.  The arguments have the
 * same meaning, as in the function `printf'.  */
{
  va_list  ap;

  prepare_for_exit ();
  
  va_start (ap, format);
  fflush (NULL);
  fputs ("Fatal error: ", stderr);
  vfprintf (stderr, format, ap);
  fputc ('\n', stderr);
  va_end (ap);

  exit (EXIT_FAILURE);
}
