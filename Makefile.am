## Process this file with automake to produce Makefile.in
# Copyright 1999, 2000, 2001  Jochen Voss

EXTRA_DIST = manpage.in car.img \
	text2c.sed img.sed ANNOUNCE moon-buggy.lsm \
	moon-buggy.png moon-buggy.xpm title.eps
DISTCLEANFILES = moon-buggy.6

AM_CPPFLAGS = @CURSES_INCLUDEDIR@

bin_PROGRAMS = moon-buggy
moon_buggy_SOURCES = moon-buggy.h main.c mode.c title.c pager.c copying.h \
	game.c level.c ground.c buggy.c buggy.h laser.c meteor.c highscore.c \
	realname.c queue.c vclock.c date.c persona.c signal.c keyboard.c \
	terminal.c cursor.c random.c error.c xmalloc.c xstrdup.c darray.h \
	hpath.c
moon_buggy_LDADD = @CURSES_LIBS@

info_TEXINFOS = moon-buggy.texi
man_MANS = moon-buggy.6

scoredir = $(sharedstatedir)/moon-buggy
hpath.o: hpath.c moon-buggy.h Makefile
	$(COMPILE) -DSCORE_DIR=\"$(scoredir)\" -c $<

BUILT_SOURCES = copying.h buggy.h
copying.h: COPYING text2c.sed
	cd $(srcdir) && sed -f text2c.sed $(srcdir)/COPYING >$@
buggy.h: car.img img.sed
	cd $(srcdir) && sed -n -f img.sed $(srcdir)/car.img >$@

install-exec-hook:
	@$(POST_INSTALL)
	if test "x@GAMEGROUP@" != "x"; then \
	  chgrp "@GAMEGROUP@" "$(DESTDIR)$(bindir)/moon-buggy" \
	    && chmod 2111 "$(DESTDIR)$(bindir)/moon-buggy" ; \
	fi
	@$(NORMAL_INSTALL)

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(scoredir)

install-data-hook:
	@$(POST_INSTALL)
	if test "x@GAMEGROUP@" != "x"; then \
	  chgrp "@GAMEGROUP@" "$(DESTDIR)$(scoredir)" \
	    && { test ! -f "$(DESTDIR)$(scoredir)/mbscore" \
		 || chgrp "@GAMEGROUP@" "$(DESTDIR)$(scoredir)/mbscore" ; } \
	    && { test ! -f "$(DESTDIR)$(scoredir)/mbscore" \
		 || chmod 664 "$(DESTDIR)$(scoredir)/mbscore" ; } \
	    && chmod 575 "$(DESTDIR)$(scoredir)" ; \
	fi
	$(DESTDIR)$(bindir)/moon-buggy -c

uninstall-local:
	rm -f $(DESTDIR)$(scoredir)/mbscore
	-rmdir $(DESTDIR)$(scoredir)

dist-hook:
	if [ -d $(srcdir)/debian ]; then \
	  mkdir $(distdir)/debian; \
	  cp -p $(srcdir)/debian/* $(distdir)/debian/; \
	fi

if short_getopt
mbsep = \\\\\"
endif
moon-buggy.6: manpage.in Makefile.am
	echo ".\\\" $@ - automatically created from" $< >$@
	sed "s:@MBBINDIR@:$(bindir):g;\
		s:@MBSCORE@:$(scoredir):g;\
		s:@C@:$(mbsep):g;\
		s:@MBVERS@:$(VERSION):g" $(srcdir)/manpage.in >>$@
