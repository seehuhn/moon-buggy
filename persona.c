/* persona.c - handle the persona of the process
 *
 * Copyright (C) 1998  Jochen Voss.  */

static const  char  rcsid[] = "$Id: persona.c,v 1.2 1998/12/31 00:53:14 voss Exp $";


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#include "moon.h"


/* which method of uid handling is used?  */
static  enum { m_NONE, m_SAVED, m_EXCH }  method = m_NONE;

/* the current persona */
static  enum { pers_GAME, pers_USER }  persona = pers_GAME;

/* The real and effective ids */
uid_t  user_user_id, game_user_id;

void
initialize_persona (void)
{
  long int  saved_ids_flag;

  persona = pers_GAME;
  method = m_NONE;
  
  user_user_id = getuid ();
  game_user_id = geteuid ();
  if (user_user_id == game_user_id)  return;

  /* check for the POSIX saved id feature.  */
#ifdef _POSIX_SAVED_IDS
  saved_ids_flag = _POSIX_SAVED_IDS;
#else  /* not defined(_POSIX_SAVED_IDS) */
  saved_ids_flag = sysconf (_SC_SAVED_IDS);
#endif /* not define(_POSIX_SAVED_IDS) */
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
  method = m_NONE;
#endif
}

void
set_game_persona (void)
{
  if (persona == pers_GAME)  return;

  switch (method) {
  case m_NONE:
    break;
  case m_SAVED:
    setuid (game_user_id);
    break;
  case m_EXCH:
#ifdef HAVE_SETREUID
    setreuid (game_user_id, user_user_id);
#else
    fatal ("cannot switch user id"); /* should not occur */
#endif
    break;
  }
  persona = pers_GAME;
}

void
set_user_persona (void)
{
  if (persona == pers_USER)  return;

  switch (method) {
  case m_NONE:
    break;
  case m_SAVED:
    setuid (user_user_id);
    break;
  case m_EXCH:
#ifdef HAVE_SETREUID
    setreuid (user_user_id, game_user_id);
#else
    fatal ("cannot switch user id"); /* should not occur */
#endif
    break;
  }
  persona = pers_USER;
}
