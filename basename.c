/* basename.c - Strip the leading path of a file name.
 *
 * Copyright (C) 1997  Jochen Voss.  */

static const  char  rcsid[] = "$Id: basename.c,v 1.2 1999/01/01 18:06:04 voss Exp $";

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>

#include "moon.h"

const char *
basename (const char *name)
{
  const char *pos = strrchr (name, '/');
  return  pos ? pos+1 : name;
}
