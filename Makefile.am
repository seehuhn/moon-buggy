## Process this file with automake to produce Makefile.in
# $Id: Makefile.am,v 1.2 1998/12/18 23:18:11 voss Exp $

bin_PROGRAMS = moon-buggy
moon_buggy_SOURCES = main.c queue.c moon.c buggy.c lag.c moon.h \
	vclock.c error.c xmalloc.c xstrdup.c @LIBOBJS@
moon_buggy_LDADD = -lcurses -lm
