/* basename.c - strip the leading path of a file name
 *
 * Copyright 1999  Jochen Voss  */

static const  char  rcsid[] = "$Id: basename.c,v 1.3 1999/01/02 12:36:24 voss Rel $";

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
