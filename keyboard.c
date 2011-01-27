/* keyboard.c - translate key codes to key meanings
 *
 * Copyright 1999, 2000  Jochen Voss.  */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#if HAVE_ERRNO_H
#include <errno.h>
#else
extern  int  errno;
#endif

#include "moon-buggy.h"


struct hash_entry {
  struct hash_entry *next;
  int  key_code;
  int  meaning;
  int  priority;
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
add_key (int key_code, enum mb_key meaning, int priority)
{
  struct hash_entry **entry_p;

#if NCURSES_VERSION_MAJOR >= 4
  if ((key_code < 0 || key_code > 255) && ! has_key (key_code))  return;
#endif
  entry_p = locate (key_code);
  if (! *entry_p) {
    *entry_p = xmalloc (sizeof (struct hash_entry));
    (*entry_p)->next = NULL;
    (*entry_p)->key_code = key_code;
    (*entry_p)->meaning = 0;
    (*entry_p)->priority = priority;
  }
  (*entry_p)->meaning |= meaning;
}

void
install_keys (void)
{
  int  i;

  for (i=0; i<HASH_SIZE; ++i)  hash_table [i] = NULL;

  add_key ('c', mbk_copyright, 100);

  add_key (14, mbk_down, 80);	/* \C-n */
#ifdef KEY_DOWN
  add_key (KEY_DOWN, mbk_down, 100);
#endif

  add_key ('q', mbk_end, 100);
  add_key ('n', mbk_end, 90);
  add_key (27, mbk_end, 80);

  add_key ('a', mbk_fire, 100);
  add_key ('l', mbk_fire, 60);

  add_key ('<', mbk_first, 90);
#ifdef KEY_HOME
  add_key (KEY_HOME, mbk_first, 100);
#endif

  add_key (' ', mbk_jump, 100);
  add_key ('j', mbk_jump, 50);

  add_key ('>', mbk_last, 90);
#ifdef KEY_END
  add_key (KEY_END, mbk_last, 100);
#endif

  add_key (' ', mbk_pagedown, 90);
#ifdef KEY_NPAGE
  add_key (KEY_NPAGE, mbk_pagedown, 100);
#endif

  add_key ('b', mbk_pageup, 90);
#ifdef KEY_PPAGE
  add_key (KEY_PPAGE, mbk_pageup, 100);
#endif

  add_key ('y', mbk_start, 100);
  add_key (' ', mbk_start, 90);
  add_key (10, mbk_start, 80);	/* RET */

  add_key (16, mbk_up, 80);	/* \C-p */
#ifdef KEY_UP
  add_key (KEY_UP, mbk_up, 100);
#endif

  add_key ('w', mbk_warranty, 100);

  add_key ('s', mbk_scores, 100);

  add_key ('r', mbk_redraw, 20);
  add_key (12, mbk_redraw, 10);	/* \C-l */
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
#ifdef KEY_RESIZE
  if (key_code == KEY_RESIZE)  return -1;
#endif
  if (key_code < 256 && isalpha (key_code))  key_code = tolower (key_code);
  entry_p = locate (key_code);
  return  *entry_p ? (*entry_p)->meaning : 0;
}

static int
function_key (int key)
{
  return  key>255 || key == ' ' || key == 10;
}

static int
control_key (int key)
{
  return  key >= 1 && key <= 26;
}

char *
key_name (int key)
/* Convert a key code into a string.
 * The retured string must not be modified by the caller.  Subsequent
 * calls to `key_name' may overwrite the returned string.
 * If the key's name is unknown, NULL is returned.  */
{
  static  char  buffer [8];
  int  i;

#ifdef KEY_BACKSPACE
  if (key == KEY_BACKSPACE)  return "BS";
#endif
#ifdef KEY_BEG
  if (key == KEY_BEG)  return "BEG";
#endif
#ifdef KEY_BREAK
  if (key == KEY_BREAK)  return "BREAK";
#endif
#ifdef KEY_CANCEL
  if (key == KEY_CANCEL)  return "CANCEL";
#endif
#ifdef KEY_CLOSE
  if (key == KEY_CLOSE)  return "CLOSE";
#endif
#ifdef KEY_DC
  if (key == KEY_DC)  return "DEL";
#endif
#ifdef KEY_DOWN
  if (key == KEY_DOWN)  return "DOWN";
#endif
#ifdef KEY_END
  if (key == KEY_END)  return "END";
#endif
#ifdef KEY_ENTER
  if (key == KEY_ENTER)  return "ENTER";
#endif
#ifdef KEY_EXIT
  if (key == KEY_EXIT)  return "EXIT";
#endif
#ifdef KEY_HOME
  if (key == KEY_HOME)  return "HOME";
#endif
#ifdef KEY_IC
  if (key == KEY_IC)  return "INS";
#endif
#ifdef KEY_LEFT
  if (key == KEY_LEFT)  return "LEFT";
#endif
#ifdef KEY_NPAGE
  if (key == KEY_NPAGE)  return "NEXT";
#endif
#ifdef KEY_PPAGE
  if (key == KEY_PPAGE)  return "PREV";
#endif
#ifdef KEY_RIGHT
  if (key == KEY_RIGHT)  return "RIGHT";
#endif
#ifdef KEY_UNDO
  if (key == KEY_UNDO)  return "UNDO";
#endif
#ifdef KEY_UP
  if (key == KEY_UP)  return "UP";
#endif
#ifdef KEY_F0
  for (i=0; i<64; ++i) {
    if (key == KEY_F(i)) {
      sprintf (buffer, "F%d", i);
      return  buffer;
    }
  }
#endif

  if (key > 255)  return NULL;
  if (key == ' ')  return "SPC";
  if (key == 10)  return "RET";
  if (isgraph (key)) {
    sprintf (buffer, "%c", key);
    return  buffer;
  }
  if (control_key (key)) {
    sprintf (buffer, "C-%c", key+'a'-1);
    return  buffer;
  }
  return  NULL;
}

#define MAX_KEYS 16
struct key_name {
  char name [8];
  int  priority, base_priority;
} data [MAX_KEYS];
struct key_info {
  int  fn, ctrl, norm;
  int  k;
  struct key_name  data [MAX_KEYS];
};

static void
set_display_priorities (struct key_name *data, int k)
{
  int i;
  for (i=0; i<k; ++i)  data[i].priority = data[i].base_priority;
}

static int
compare_keys (const void *a, const void *b)
{
  const struct key_name *aa = a;
  const struct key_name *bb = b;

  if (aa->priority < bb->priority)  return +1;
  if (aa->priority > bb->priority)  return -1;
  if (strlen (aa->name) < strlen (bb->name))  return -1;
  if (strlen (aa->name) > strlen (bb->name))  return +1;
  return  strcmp (aa->name, bb->name);
}

static int
choose_keys (int *n, const struct binding *b, struct key_info *keys,
             int max_len)
/* Choose the keys to explain, based on available display space.
 * Input are *N key bindings in the array B and the corresponding key
 * names in KEYS.  The resulting explanation must fit within MAX_LEN
 * characters.
 *
 * This function may decrease *N and clear the priority values of
 * some keys in order to exclude keys from the explanation.  */
{
  int  finished;
  int  *first;
  int  i, k, len;

  /* Is there enough space to explain all functions?  */
  len = max_len;
  for (i=0; i<*n; ++i) {
    int  x;
    x = (i>0) ? 1 : 0;		/* " " */
    x += 2 + strlen(b[i].desc);	/* "x:desc" */
    if (len >= x) {
      len -= x;
    } else {
      *n=i;
      break;
    }
  }

  /* How much space do we have left? */
  len = max_len;
  first = xmalloc (*n * sizeof(int));
  for (i=0; i<*n; ++i) {
    first[i] = 1;
    if (i>0)  --len;		/* " " */
    len -= 1 + strlen(b[i].desc); /* ":desc" */
  }
  finished = 0;
  for (k=0; ! finished; ++k) {
    finished = 1;
    for (i=0; i<*n; ++i) {
      if (k < keys[i].k) {
        int  x;

        x = first[i] ? 0 : 1;	/* "," */
        x += strlen (keys[i].data[k].name);
        if (x <= len) {
          len -= x;
          first[i] = 0;
        } else {
          keys[i].data[k].base_priority = 0;
        }
        finished = 0;
      }
    }
  }
  free (first);

  for (i=0; i<*n; ++i) {
    set_display_priorities (keys[i].data, keys[i].k);
    qsort (keys[i].data, keys[i].k, sizeof (struct key_name), compare_keys);
  }

  return  len;
}

void
describe_keys (int n, const struct binding *b)
{
  struct key_info *keys;
  char* buffer;
  int  len, max_len;
  int  i, j, k;

  keys = xmalloc (n*sizeof(struct key_info));
  for (i=0; i<n; ++i) {
    keys[i].fn = 0;
    keys[i].ctrl = 0;
    keys[i].norm = 0;
    keys[i].k = 0;
  }
  for (j=0; j<HASH_SIZE; ++j) {
    struct hash_entry *ent = hash_table[j];

    while (ent) {
      for (i=0; i<n; ++i) {
        if (ent->meaning & b[i].meanings) {
          char *name = key_name (ent->key_code);

          if (name) {
            k = keys[i].k++;
            strncpy (keys[i].data[k].name, name, 7);
            keys[i].data[k].name[7] = 0;
            keys[i].data[k].base_priority = ent->priority;
            keys[i].data[k].priority = ent->priority;
            if (function_key (ent->key_code)) {
              if (keys[i].fn == 0)  keys[i].data[k].priority += 1050;
              ++keys[i].fn;
            } else if (control_key (ent->key_code)) {
              if (keys[i].ctrl == 0)  keys[i].data[k].priority += 1000;
              ++keys[i].ctrl;
            } else {
              keys[i].data[k].priority += 1100;
              ++keys[i].norm;
            }
          }
          break;
        }
      }
      ent = ent->next;
    }
  }
  for (i=0; i<n; ++i) {
    qsort (keys[i].data, keys[i].k, sizeof (struct key_name), compare_keys);
  }

  max_len = COLS;
  len = choose_keys (&n, b, keys, max_len);
  buffer = xmalloc (max_len+1);

  buffer[0] = '\0';
  for (i=0; i<n; ++i) {
    int  first = 1;
    for (k=0; k<keys[i].k; ++k) {
      if (keys[i].data[k].priority) {
        if (! first) {
          strcat (buffer, ",");
        } else {
          if (i>0) {
            if (len > 0) {
              --len;
              strcat (buffer, "  ");
            } else {
              strcat (buffer, " ");
            }
          }
          first = 0;
        }
        strcat (buffer, keys[i].data[k].name);
      }
    }
    if (keys[i].data[0].priority) {
      strcat (buffer, ":");
      strcat (buffer, b[i].desc);
    }
  }
  print_message (buffer);

  free (buffer);
  free (keys);
}
