/* realname.c - query the user's real name
 *
 * Copyright 1999  Jochen Voss  */

static const  char  rcsid[] = "$Id: realname.c,v 1.7 1999/04/23 17:54:54 voss Exp $";

#define _POSIX_SOURCE 1

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <assert.h>

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

  werase (message);
  if (buffer[0] == '\0') {
    waddstr (message, "please enter your name: ");
  } else {
    char  tmpl [100];
    int  def_size;

    def_size = COLS - size - strlen("please enter your name (default: \""
				  "\"): ");
    if (def_size >= (int)xstrnlen(buffer, size)) {
      sprintf (tmpl, "please enter your name (default: \"%%.%ds\"): ", size);
    } else {
      def_size -= 2;
      if (def_size < 6)  def_size = 6;
      assert (size >= 8);
      sprintf (tmpl, "please enter your name (default: \"%%.%ds..\"): ",
	       def_size);
    }
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
