/* xstrdup.c - fail save strdup
 *
 * Copyright 1998  Jochen Voss.  */

static const  char  rcsid[] = "$Id: xstrdup.c,v 1.7 1999/06/13 19:16:46 voss Rel $";

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>

#include "mbuggy.h"


char *
xstrdup (const char *str)
/* Duplicate STR as `strdup' does, but never return NULL.  */
{
  char *tmp = xmalloc (strlen(str) + 1);
  strcpy (tmp, str);
  return  tmp;
}

size_t
xstrnlen (const char *str, size_t size)
{
  size_t  n = 0;
  while (n<size && str[n])  ++n;
  return  n;
}

char *
xstrndup (const char *str, size_t size)
/* Like `strndup' from glibc, but never returns NULL.  */
{
  size_t  n = xstrnlen (str, size);
  char *tmp = xmalloc (n + 1);
  
  if (tmp == NULL)  fatal ("Memory exhausted");
  strncpy (tmp, str, n);
  tmp[n] = '\0';
  
  return  tmp;
}
