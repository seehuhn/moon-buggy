/* mesg.c - restrict write access to the terminal
 *
 * Copyright (C) 2000  Jochen Voss.  */

static const  char  rcsid[] = "$Id: mesg.c,v 1.1 2000/11/01 13:10:54 voss Exp $";

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#ifdef _XOPEN_SOURCE
#define _XOPEN_SOURCE_EXTENDED 1
#endif

#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#if HAVE_ERRNO_H
#include <errno.h>
#else
extern  int  errno;
#endif
#include <assert.h>

#include "moon-buggy.h"


static  mode_t  term_mode;
static  int  term_mode_valid;


void
mesg_off (void)
/* Restrict write access to the terminal as "mesg n" would do.  */
{
  struct stat st;
  int  res;
  
  assert (! term_mode_valid);

  if ( ! isatty (0) )  return;
  res = fstat (0, &st);
  if (res < 0)  fatal ("Cannot get terminal attributes: %s", strerror (errno));

  term_mode = st.st_mode;
  term_mode_valid = 1;

  st.st_mode &= ~(S_IWGRP|S_IWOTH);
  res = fchmod (0, st.st_mode);
  if (res < 0)
    fatal ("Cannot set terminal permissions: %s", strerror (errno));
}

void
mesg_restore (void)
/* Restore the terminal's original access mode.  */
{
  int  res;
  
  if (! term_mode_valid)  return;

  res = fchmod (0, term_mode);
  if (res < 0)
    fatal ("Cannot restore terminal permissions: %s", strerror (errno));
  
  term_mode_valid = 0;
}
