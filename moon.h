/* moon.h - global moon-buggy header file
 *
 * Copyright (C) 1998  Jochen Voss.
 *
 * $Id: moon.h,v 1.1 1998/12/17 20:00:29 voss Exp $ */

#ifndef FILE_MOON_H_SEEN
#define FILE_MOON_H_SEEN

extern  const char *my_name;

extern  double  vclock (void);
extern  void *xmalloc (size_t size);
extern  void *xrealloc (void *ptr, size_t size);
extern  char *xstrdup (const char *str);

/* from "error.c" */
#ifdef __GNUC__
extern  void  fatal (const char *format, ...)
	__attribute__ ((noreturn)) __attribute__ ((format (printf, 1, 2)));
extern  void  warning (const char *format, ...)
	__attribute__ ((format (printf, 1, 2)));
#else
extern  void  fatal (const char *format, ...);
extern  void  warning (const char *format, ...);
#endif

#endif /* FILE_MOON_H_SEEN */
