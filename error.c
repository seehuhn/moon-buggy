/* error.c - functions to signal error conditions
 *
 * Copyright (C) 1998  Jochen Voss.  */

static const  char  rcsid[] = "$Id: error.c,v 1.1 1998/12/17 19:59:31 voss Exp $";

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <unistd.h>

#include "moon.h"


void
fatal (const char *format, ...)
/* Signal a fatal error and quit immediately.  The arguments have the
 * same meaning, as in the function `printf'.  The function tries to
 * conform with the "GNU coding standards". */
{
  va_list  ap;

  va_start (ap, format);
  fflush (NULL);
  if (isatty (STDIN_FILENO)) {
    fputs ("Fatal error: ", stderr);
  } else {
    fprintf (stderr, "%s: ", my_name);
  }
  if (! isatty (STDIN_FILENO) && isupper (format[0])) {
    /* We all hope, that '%' is no upper-case letter.  */
    fputc (tolower (format[0]), stderr);
    vfprintf (stderr, format+1, ap);
  } else {
    vfprintf (stderr, format, ap);
  }
  fputc ('\n', stderr);
  va_end (ap);

  exit (EXIT_FAILURE);
}

void
warning (const char *format, ...)
/* Emit a warning message.  The arguments have the
 * same meaning, as in the function `printf'.  The function tries to
 * conform with the "GNU coding standards". */
{
  va_list  ap;

  va_start (ap, format);
  fflush (NULL);
  if (isatty (STDIN_FILENO)) {
    fputs ("Warning: ", stderr);
  } else {
    fprintf (stderr, "%s: ", my_name);
  }
  if (! isatty (STDIN_FILENO) && isupper (format[0])) {
    /* We all hope, that '%' is no upper-case letter.  */
    fputc (tolower (format[0]), stderr);
    vfprintf (stderr, format+1, ap);
  } else {
    vfprintf (stderr, format, ap);
  }
  fputc ('\n', stderr);
  va_end (ap);
}
