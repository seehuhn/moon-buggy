/* basename.c - Strip the leading path of a file name.
 *
 * Copyright (C) 1997  Jochen Voss.  */

static const  char  rcsid[] = "$Id: basename.c,v 1.1 1998/12/17 19:59:24 voss Exp $";

#include <string.h>

const char *
basename (const char *name)
{
  const char *pos = strrchr (name, '/');
  return  pos ? pos+1 : name;
}
