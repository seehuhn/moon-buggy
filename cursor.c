/* cursor.c - handle the cursor's visibility
 *
 * Copyright (C) 2000  Jochen Voss.  */

static const  char  rcsid[] = "$Id: cursor.c,v 1.1 2000/03/31 11:19:34 voss Rel $";

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "moon-buggy.h"


void
hide_cursor (void)
{
  leaveok (moon, TRUE);
  leaveok (status, TRUE);
  leaveok (message, TRUE);
  curs_set (0);
}

void
show_cursor (void)
{
  curs_set (1);
  leaveok (moon, FALSE);
  leaveok (status, FALSE);
  leaveok (message, FALSE);
}
