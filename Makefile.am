## Process this file with automake to produce Makefile.in
# $Id: Makefile.am,v 1.3 1998/12/20 01:01:19 voss Exp $

bin_PROGRAMS = moon-buggy
moon_buggy_SOURCES = main.c queue.c moon.c buggy.c highscore.c lag.c moon.h \
	realname.c vclock.c error.c xmalloc.c xstrdup.c @LIBOBJS@
moon_buggy_LDADD = mbpaths.o -lcurses -lm
info_TEXINFOS = mbuggy.texi

scoredir = $(sharedstatedir)/moon-buggy
mbpaths.o: mbpaths.c moon.h Makefile
	$(COMPILE) -DSCORE_DIR=\"$(scoredir)\" -c $<
score_DATA = mbscore.tmpl
EXTRA_DIST = mbscore.tmpl mbpaths.c

install-data-hook:
	[ -e $(scoredir)/mbscore ] \
	  || cp $(scoredir)/mbscore.tmpl $(scoredir)/mbscore
uninstall-local:
	rm -f $(scoredir)/mbscore
	-rmdir $(scoredir)
