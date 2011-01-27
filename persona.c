/* persona.c - handle the persona of the process
 *
 * Copyright 1999  Jochen Voss  */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef _XOPEN_SOURCE
#define _XOPEN_SOURCE_EXTENDED 1
#endif

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#if defined(__hp9000s800)
#include <stdarg.h>
#endif

#include "moon-buggy.h"


/* which method of uid handling is used?  */
static  enum { m_NONE, m_SAVED, m_EXCH }  method = m_NONE;

/* the current effective user id */
static  enum persona  current = pers_GAME;

/* The real and effective ids */
static  uid_t  user_uid, game_uid;
static  gid_t  user_gid, game_gid;


void
initialise_persona (void)
{
  long int  saved_ids_flag;

  current = pers_GAME;
  method = m_NONE;

  user_uid = getuid ();
  game_uid = geteuid ();
  user_gid = getgid ();
  game_gid = getegid ();
  if (user_uid == game_uid && user_gid == game_gid)  return;

  /* check for the POSIX saved id feature.  */
#ifdef _POSIX_SAVED_IDS
  saved_ids_flag = 1;
#else  /* not defined(_POSIX_SAVED_IDS) */
  saved_ids_flag = sysconf (_SC_SAVED_IDS);
#endif /* not defined(_POSIX_SAVED_IDS) */
  if (saved_ids_flag != -1) {
    method = m_SAVED;
    return;
  }

#ifdef HAVE_SETREUID
  method = m_EXCH;
#else
  fputs ("WARNING: setuid/setgid usage not supported on this system!\n",
         stderr);
  sleep (3);
  setuid (user_uid);
  setgid (user_gid);
  method = m_NONE;
#endif
}

int
is_setgid (void)
/* Return true, iff the game is run setgid and we currently use
 * privileged access.  */
{
  return  (user_gid != game_gid && current == pers_GAME);
}


static char *name[] = { "game", "user" };

void
set_persona (enum persona  pers)
/* Switch to process persona (effective user id) PERS.  */
{
  int  res;
  uid_t  old_uid, new_uid;
  gid_t  old_gid, new_gid;

  if (current == pers)  return;
  switch (pers) {
  case pers_GAME:
    old_uid = user_uid;
    new_uid = game_uid;
    old_gid = user_gid;
    new_gid = game_gid;
    break;
  case pers_USER:
    old_uid = game_uid;
    new_uid = user_uid;
    old_gid = game_gid;
    new_gid = user_gid;
    break;
  default:
    abort ();			/* should not happen */
  }

  switch (method) {
  case m_NONE:
    break;
  case m_SAVED:
    res = setuid (new_uid);
    if (res < 0)  fatal ("Cannot set uid to %s", name[pers]);
    res = setgid (new_gid);
    if (res < 0)  fatal ("Cannot set gid to %s", name[pers]);
    break;
  case m_EXCH:
#ifdef HAVE_SETREUID
    res = setreuid (old_uid, new_uid);
    if (res < 0)  fatal ("Cannot switch real/effective uid to %s", name[pers]);
    res = setregid (old_gid, new_gid);
    if (res < 0)  fatal ("Cannot switch real/effective gid to %s", name[pers]);
#else
    abort ();			/* should not happen */
#endif
    break;
  }
  current = pers;
}
