/* keyboard.c - translate key codes to key meanings
 *
 * Copyright 1999  Jochen Voss.  */

static const  char  rcsid[] = "$Id: keyboard.c,v 1.4 1999/06/17 14:45:23 voss Rel $";

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#if HAVE_ERRNO_H
#include <errno.h>
#else
extern  int  errno;
#endif

#include "mbuggy.h"


struct hash_entry {
  struct hash_entry *next;
  int  key_code;
  int  meaning;
};

#define  HASH_SIZE  37
struct hash_entry *hash_table [HASH_SIZE];


static unsigned
m_mod (int x, int y)
{
  int  r = x%y;
  return  r<0 ? (y<0 ? r-y : r+y) : r;
}

static struct hash_entry **
locate (int key_code)
{
  int  slot = m_mod (key_code, HASH_SIZE);
  struct hash_entry **res = &hash_table[slot];
  while (*res && (*res)->key_code != key_code)  res = &((*res)->next);
  return  res;
}

static void
add_key (int key_code, enum mb_key meaning)
{
  struct hash_entry **entry_p = locate (key_code);
  if (! *entry_p) {
    *entry_p = xmalloc (sizeof (struct hash_entry));
    (*entry_p)->next = NULL;
    (*entry_p)->key_code = key_code;
    (*entry_p)->meaning = 0;
  }
  (*entry_p)->meaning |= meaning;
}

static void
remove_key (int key_code, enum mb_key meaning)
{
  struct hash_entry **entry_p = locate (key_code);
  (*entry_p)->meaning &= ~(int)meaning;
  if ((*entry_p)->meaning == 0) {
    struct hash_entry *old = *entry_p;
    *entry_p = (*entry_p)->next;
    free (old);
  }
}

void
install_keys (void)
{
  int  i;

  for (i=0; i<HASH_SIZE; ++i)  hash_table [i] = NULL;
  add_key ('c', mbk_copyright);

  add_key (14, mbk_down);	/* \C-n */
#ifdef KEY_DOWN
  add_key (KEY_DOWN, mbk_down);
#endif

  add_key ('q', mbk_end);
  add_key ('n', mbk_end);
  add_key (27, mbk_end);
#ifdef KEY_BREAK
  add_key (KEY_BREAK, mbk_end);
#endif
#ifdef KEY_CANCEL
  add_key (KEY_CANCEL, mbk_end);
#endif
#ifdef KEY_EXIT
  add_key (KEY_EXIT, mbk_end);
#endif
#ifdef KEY_CLOSE
  add_key (KEY_CLOSE, mbk_end);
#endif
#ifdef KEY_LEFT
  add_key (KEY_LEFT, mbk_end);
#endif
#ifdef KEY_UNDO
  add_key (KEY_UNDO, mbk_end);
#endif

  add_key ('a', mbk_fire);

  add_key ('<', mbk_first);
#ifdef KEY_A1
  add_key (KEY_A1, mbk_first);
#endif
#ifdef KEY_HOME
  add_key (KEY_HOME, mbk_first);
#endif

  add_key (' ', mbk_jump);
  
  add_key ('>', mbk_last);
#ifdef KEY_C1
  add_key (KEY_C1, mbk_last);
#endif
#ifdef KEY_END
  add_key (KEY_END, mbk_last);
#endif

  add_key (' ', mbk_pagedown);
#ifdef KEY_NPAGE
  add_key (KEY_NPAGE, mbk_pagedown);
#endif

  add_key ('b', mbk_pageup);
#ifdef KEY_PPAGE
  add_key (KEY_PPAGE, mbk_pageup);
#endif

  add_key ('y', mbk_start);
  add_key (' ', mbk_start);
#ifdef KEY_BEG
  add_key (KEY_BEG, mbk_start);
#endif
#ifdef KEY_ENTER
  add_key (KEY_ENTER, mbk_start);
#endif

  add_key (16, mbk_up);		/* \C-p */
#ifdef KEY_UP
  add_key (KEY_UP, mbk_up);
#endif

  add_key ('w', mbk_warranty);
}

int
read_key (void)
{
  int key_code;
  struct hash_entry **entry_p;
  
  do {
    key_code = wgetch (moon);
  } while (key_code == ERR && errno == EINTR);
  if (key_code == ERR)  fatal ("Cannot read keyboard input");
  entry_p = locate (key_code);
  return  *entry_p ? (*entry_p)->meaning : 0;
}
