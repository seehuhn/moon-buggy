/* realname.c - query the user's real name
 *
 * Copyright 1999  Jochen Voss  */

static const  char  rcsid[] = "$Id: realname.c,v 1.5 1999/01/02 12:08:08 voss Rel $";

#define _POSIX_SOURCE

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
/* Query the real user name.
 * Store the result into BUFFER, but do not write more then SIZE
 * characters.  */
{
  char *tmp;

  tmp = xmalloc (size);
  
  if (buffer[0] == '\0') {
    uid_t me = getuid ();
    struct passwd *my_passwd = getpwuid (me);
    if (my_passwd) {
      strncpy (buffer, my_passwd->pw_gecos, size);
    }
  }

  wclear (message);
  if (buffer[0] == '\0') {
    waddstr (message, "please enter your name: ");
  } else {
    char  tmpl [100];
    sprintf (tmpl, "please enter your name (default: \"%%.%ds\"): ", size);
    wprintw (message, tmpl, buffer);
  }

  echo ();
  leaveok (message, FALSE);
  wgetnstr (message, tmp, size);
  noecho ();
  leaveok (message, TRUE);
  if (tmp[0]) {
    strncpy (buffer, tmp, size);
  }

  free (tmp);
}
