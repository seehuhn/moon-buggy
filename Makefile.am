## Process this file with automake to produce Makefile.in
# $Id: Makefile.am,v 1.4 1998/12/23 10:13:55 voss Exp $

EXTRA_DIST = mbpaths.c manpage.in
DISTCLEANFILES = moon-buggy.6

bin_PROGRAMS = moon-buggy
moon_buggy_SOURCES = main.c queue.c moon.c buggy.c highscore.c lag.c moon.h \
	realname.c vclock.c error.c xmalloc.c xstrdup.c @LIBOBJS@
moon_buggy_LDADD = mbpaths.o -lcurses -lm
info_TEXINFOS = mbuggy.texi
man_MANS = moon-buggy.6

scoredir = $(sharedstatedir)/moon-buggy
mbpaths.o: mbpaths.c moon.h Makefile
	$(COMPILE) -DSCORE_DIR=\"$(scoredir)\" -c $<

install-data-hook:
	$(mkinstalldirs) $(DESTDIR)$(scoredir)

uninstall-local:
	-rmdir $(scoredir)

if short_getopt
mbsep = \\\\\"
endif
moon-buggy.6: manpage.in Makefile
	echo ".\\\" $@ - automatically created from $<" >$@
	sed "s:@MBSCORE@:$(scoredir):g;\
		s:@C@:$(mbsep):g;\
		s:@MBNAME@:$(PACKAGE):g;\
		s:@MBVERS@:$(VERSION):g" $< >>$@
