/* xstrdup.c - fail save strdup
 *
 * Copyright (C) 1998  Jochen Voss.  */

static const  char  rcsid[] = "$Id: xstrdup.c,v 1.2 1998/12/22 22:29:55 voss Exp $";

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>

#include "moon.h"


char *
xstrdup (const char *str)
{
  char *tmp = xmalloc (strlen(str) + 1);
  if (tmp == NULL)  fatal ("Memory exhausted");
  strcpy (tmp, str);
  return  tmp;
}
