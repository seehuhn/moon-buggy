/* realname.c - query the user's real name
 *
 * Copyright (C) 1998  Jochen Voss.  */

static const  char  rcsid[] = "$Id: realname.c,v 1.2 1998/12/26 11:40:51 voss Exp $";


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

#include "moon.h"


void
get_real_user_name (char *buffer, size_t size)
{
  char  tmp [size];

  if (buffer[0] == '\0') {
    uid_t me = getuid ();
    struct passwd *my_passwd = getpwuid (me);
    if (my_passwd) {
      strncpy (buffer, my_passwd->pw_gecos, size);
    }
  }

  if (buffer[0] == '\0') {
    mvwaddstr (moon, 4, 8, "please enter your name:");
  } else {
    char  tmpl [100];
    sprintf (tmpl, "please enter your name (default: \"%%.%ds\"):", size);
    mvwprintw (moon, 4, 8, tmpl, buffer);
  }
  echo ();
  leaveok (moon, FALSE);
  mvwgetnstr (moon, 5, 10, tmp, size);
  noecho ();
  leaveok (moon, TRUE);
  if (tmp[0]) {
    strncpy (buffer, tmp, size);
  }
}
