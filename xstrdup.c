/* xstrdup.c - fail save strdup
 *
 * Copyright (C) 1998  Jochen Voss.  */

static const  char  rcsid[] = "$Id: xstrdup.c,v 1.1 1998/12/17 19:59:40 voss Exp $";

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>

#include "moon.h"


char *
xstrdup (const char *str)
{
  char *tmp = strdup (str);
  if (tmp == NULL)  fatal ("Memory exhausted");
  return  tmp;
}
