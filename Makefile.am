## Process this file with automake to produce Makefile.in
# $Id: Makefile.am,v 1.9 1998/12/30 22:38:07 voss Exp $

EXTRA_DIST = mbpaths.c manpage.in moon-buggy-0.2-1.spec mbicon.gif mbicon.png \
	text2c.sed
DISTCLEANFILES = moon-buggy.6

bin_PROGRAMS = moon-buggy
moon_buggy_SOURCES = moon.h main.c title.c pager.c copying.h game.c \
	moon.c buggy.c highscore.c realname.c queue.c lag.c vclock.c \
	persona.c error.c xmalloc.c xstrdup.c mbpaths.c
moon_buggy_LDADD = @LIBOBJS@

info_TEXINFOS = mbuggy.texi
man_MANS = moon-buggy.6

scoredir = $(sharedstatedir)/moon-buggy
mbpaths.o: mbpaths.c moon.h Makefile
	$(COMPILE) -DSCORE_DIR=\"$(scoredir)\" -c $<

BUILT_SOURCES = copying.h
copying.h: COPYING text2c.sed
	cd $(srcdir) && sed -f text2c.sed $< >$@

install-data-hook:
	$(mkinstalldirs) $(DESTDIR)$(scoredir)

uninstall-local:
	-rmdir $(DESTDIR)$(scoredir)

if short_getopt
mbsep = \\\\\"
endif
moon-buggy.6: manpage.in Makefile
	echo ".\\\" $@ - automatically created from $<" >$@
	sed "s:@MBSCORE@:$(scoredir):g;\
		s:@C@:$(mbsep):g;\
		s:@MBNAME@:$(PACKAGE):g;\
		s:@MBVERS@:$(VERSION):g" $< >>$@
