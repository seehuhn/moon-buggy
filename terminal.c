/* terminal.c - prepare the terminal
 *
 * Copyright (C) 2000  Jochen Voss.  */

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
#if HAVE_TERMIOS_H
#include <termios.h>
#endif
#include <assert.h>

#include "moon-buggy.h"


static  mode_t  term_mode;	/* The terminal mode before we changed it */
static  int  term_mode_valid;	/* true iff the above is valid */
static  int  attributes_changed; /* true iff we changed the IXON attribute */


void
term_prepare (int mesg_n_flag)
/* Prepare the terminal for game play.
 * This tries to disable the C-s key and, if MESG_N_FLAG is set,
 * restricts write access to the terminal as "mesg n" would do.  */
{
  struct stat  st;
#if HAVE_TERMIOS_H
  struct termios  settings;
#endif
  int  res;

  assert (! term_mode_valid);
  if ( ! isatty (0) )  return;

  if (mesg_n_flag) {
    res = fstat (0, &st);
    if (res < 0)
      fatal ("Cannot get terminal attributes: %s", strerror (errno));

    term_mode = st.st_mode;
    term_mode_valid = 1;

    st.st_mode &= ~(S_IWGRP|S_IWOTH);
    res = fchmod (0, st.st_mode);
    if (res < 0)
      fatal ("Cannot set terminal permissions: %s", strerror (errno));
  }

#if HAVE_TERMIOS_H
  res = tcgetattr (0, &settings);
  if (res < 0)
    fatal ("Cannot get terminal attributes: %s", strerror (errno));

  if (settings.c_iflag & IXON) {
    settings.c_iflag &= ~IXON;
    res = tcsetattr (0, TCSANOW, &settings);
    if (res < 0)
      fatal ("Cannot set terminal attributes: %s", strerror (errno));
    attributes_changed = 1;
  }
#endif
}

void
term_restore (void)
/* Restore the terminal's original access mode.  */
{
#if HAVE_TERMIOS_H
  struct termios  settings;
#endif
  int  res;

#if HAVE_TERMIOS_H
  if (attributes_changed) {
    res = tcgetattr (0, &settings);
    if (res < 0)
      fatal ("Cannot get terminal attributes: %s", strerror (errno));

    settings.c_iflag |= IXON;
    res = tcsetattr (0, TCSANOW, &settings);
    if (res < 0)
      fatal ("Cannot set terminal attributes: %s", strerror (errno));
  }
#endif

  if (term_mode_valid) {
    res = fchmod (0, term_mode);
    if (res < 0)
      fatal ("Cannot restore terminal permissions: %s", strerror (errno));
    term_mode_valid = 0;
  }
}
