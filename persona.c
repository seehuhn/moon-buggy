/* persona.c - handle the persona of the process
 *
 * Copyright 1999  Jochen Voss  */

static const  char  rcsid[] = "$Id: persona.c,v 1.12 1999/05/26 21:04:13 voss Exp $";

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#define _XOPEN_SOURCE_EXTENDED 1

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#if defined(__hp9000s800)
#include <stdarg.h>
#endif

#include "mbuggy.h"


/* which method of uid handling is used?  */
static  enum { m_NONE, m_SAVED, m_EXCH }  method = m_NONE;

/* the current effective user id */
static  enum { pers_GAME, pers_USER }  persona = pers_GAME;

/* The real and effective ids */
static  uid_t  user_user_id, game_user_id;
static  gid_t  user_group_id, game_group_id;


void
initialise_persona (void)
{
  long int  saved_ids_flag;

  persona = pers_GAME;
  method = m_NONE;
  
  user_user_id = getuid ();
  game_user_id = geteuid ();
  user_group_id = getgid ();
  game_group_id = getegid ();
  if (user_user_id == game_user_id
      && user_group_id == game_group_id)  return;

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
  fputs ("WARNING: suid usage not supported on this system!\n", stderr);
  sleep (3);
  setuid (user_user_id);
  setgid (user_group_id);
  method = m_NONE;
#endif
}

void
set_game_persona (void)
{
  int  res;
  
  if (persona == pers_GAME)  return;

  switch (method) {
  case m_NONE:
    break;
  case m_SAVED:
    res = setuid (game_user_id);
    if (res < 0)  fatal ("cannot set uid to games");
    res = setgid (game_group_id);
    if (res < 0)  fatal ("cannot set gid to games");
    break;
  case m_EXCH:
#ifdef HAVE_SETREUID
    res = setreuid (user_user_id, game_user_id);
    if (res < 0)  fatal ("cannot switch real/effective uid to games");
    res = setregid (user_group_id, game_group_id);
    if (res < 0)  fatal ("cannot switch real/effective gid to games");
#else
    fatal ("cannot switch ids"); /* should not occur */
#endif
    break;
  }
  persona = pers_GAME;
}

void
set_user_persona (void)
{
  int  res;
  
  if (persona == pers_USER)  return;

  switch (method) {
  case m_NONE:
    break;
  case m_SAVED:
    res = setuid (user_user_id);
    if (res < 0)  fatal ("cannot set uid to user");
    res = setgid (user_group_id);
    if (res < 0)  fatal ("cannot set gid to user");
    break;
  case m_EXCH:
#ifdef HAVE_SETREUID
    res = setreuid (game_user_id, user_user_id);
    if (res < 0)  fatal ("cannot switch real/effective uid to user");
    res = setregid (game_group_id, user_group_id);
    if (res < 0)  fatal ("cannot switch real/effective gid to user");
#else
    fatal ("cannot switch ids"); /* should not occur */
#endif
    break;
  }
  persona = pers_USER;
}
