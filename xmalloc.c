/* xmalloc.c - memory allocation with error checking
 *
 * Copyright (C) 1998  Jochen Voss
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * Written by Jochen Voﬂ (voss@mathematik.uni-kl.de).  */

static const  char  rcsid[] = "$Id: xmalloc.c,v 1.1 1998/12/17 19:59:35 voss Exp $";

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>

#include "moon.h"


void *
xmalloc (size_t size)
/* Like `malloc', but check for shortage of memory.  `xmalloc' never
 * returns `NULL'.  */
{
  void *ptr = malloc (size);
  if (ptr == NULL)  fatal ("out of memory");
  return  ptr;
}

void *
xrealloc (void *ptr, size_t size)
/* Like `malloc', but check for shortage of memory.  `xrealloc' never
 * returns `NULL'.  */
{
  void *tmp = realloc (ptr, size);
  if (tmp == NULL)  fatal ("out of memory");
  return  tmp;
}
