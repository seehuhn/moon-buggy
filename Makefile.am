## Process this file with automake to produce Makefile.in
# Copyright 1999  Jochen Voß
# $Id: Makefile.am,v 1.22 1999/05/19 21:14:48 voss Exp $

EXTRA_DIST = moonman.in maintman.in instcmds car.img \
	text2c.sed merge.sed img.sed \
	mbuggy.spec moon-buggy.gif moon-buggy.png title.eps
DISTCLEANFILES = moon-buggy.6 maint-buggy maint-buggy.6 uninst.pre uninst.post

bin_PROGRAMS = moon-buggy
moon_buggy_SOURCES = moon.h main.c title.c pager.c copying.h \
	game.c moon.c buggy.c buggy.h laser.c meteor.c highscore.c \
	realname.c queue.c vclock.c persona.c signal.c xgetch.c \
	random.c error.c xmalloc.c xstrdup.c darray.h hpath.c
moon_buggy_LDADD = @LIBOBJS@

bin_SCRIPTS = maint-buggy
uninst.pre: Makefile instcmds
	$(srcdir)/instcmds --uninstall pre >uninst.pre
uninst.post: Makefile instcmds
	$(srcdir)/instcmds --uninstall post >uninst.post
maint-buggy: maint.in merge.sed uninst.pre uninst.post
	sed -f $(srcdir)/merge.sed $< >$@

info_TEXINFOS = mbuggy.texi
man_MANS = moon-buggy.6 maint-buggy.6

scoredir = $(sharedstatedir)/moon-buggy
hpath.o: hpath.c moon.h Makefile
	$(COMPILE) -DSCORE_DIR=\"$(scoredir)\" -c $<

BUILT_SOURCES = copying.h buggy.h
copying.h: COPYING text2c.sed
	cd $(srcdir) && sed -f text2c.sed $< >$@
buggy.h: car.img img.sed
	cd $(srcdir) && sed -n -f img.sed $< >$@

install-exec-hook:
	if test "x@GAMEUSER@" != "x"; then \
	  chown "@GAMEUSER@" "$(DESTDIR)/$(bindir)/moon-buggy" \
	    && chmod u+s "$(DESTDIR)/$(bindir)/moon-buggy" ; \
	fi
	chmod go-x "$(DESTDIR)/$(bindir)/maint-buggy"

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(scoredir)

install-data-hook:
	@$(POST_INSTALL)
	if test "x@GAMEUSER@" != "x"; then \
	  chown "@GAMEUSER@" "$(DESTDIR)/$(scoredir)" \
	    && { test ! -e "$(DESTDIR)/$(scoredir)/mbscore" \
		 || chown "@GAMEUSER@" "$(DESTDIR)/$(scoredir)/mbscore" ; } \
	    && { test ! -e "$(DESTDIR)/$(scoredir)/mbscore" \
		 || chmod go-w "$(DESTDIR)/$(scoredir)/mbscore" ; } \
	    && chmod go-w "$(DESTDIR)/$(scoredir)" ; \
	fi

uninstall-local:
	rm -f $(DESTDIR)$(scoredir)/mbscore
	-rmdir $(DESTDIR)$(scoredir)

if short_getopt
mbsep = \\\\\"
endif
moon-buggy.6: moonman.in Makefile
	echo ".\\\" $@ - automatically created from $<" >$@
	sed "s:@MBSCORE@:$(scoredir):g;\
		s:@C@:$(mbsep):g;\
		s:@MBNAME@:$(PACKAGE):g;\
		s:@MBVERS@:$(VERSION):g" $< >>$@
maint-buggy.6: maintman.in Makefile
	echo ".\\\" $@ - automatically created from $<" >$@
	sed "s:@MBNAME@:$(PACKAGE):g;\
		s:@MBVERS@:$(VERSION):g" $< >>$@
