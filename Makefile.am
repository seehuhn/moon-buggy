## Process this file with automake to produce Makefile.in
# Copyright 1999  Jochen Voß
# $Id: Makefile.am,v 1.11 1999/01/02 12:39:55 voss Exp $

EXTRA_DIST = hpath.c moonman.in maintman.in instcmds text2c.sed merge.sed \
	moon-buggy-0.2-1.spec moon-buggy.gif moon-buggy.png
DISTCLEANFILES = moon-buggy.6 maint-buggy.6 uninst.pre uninst.post

bin_PROGRAMS = moon-buggy
moon_buggy_SOURCES = moon.h main.c title.c pager.c copying.h game.c \
	moon.c buggy.c highscore.c realname.c queue.c lag.c vclock.c \
	persona.c xgetch.c error.c xmalloc.c xstrdup.c hpath.c
moon_buggy_LDADD = @LIBOBJS@

bin_SCRIPTS = maint-buggy
uninst.pre: Makefile instcmds
	AWK=$(AWK) $(srcdir)/instcmds --uninstall pre >uninst.pre
uninst.post: Makefile instcmds
	AWK=$(AWK) $(srcdir)/instcmds --uninstall post >uninst.post
maint-buggy: maint.in merge.sed uninst.pre uninst.post
	sed -f $(srcdir)/merge.sed $< >$@

info_TEXINFOS = mbuggy.texi
man_MANS = moon-buggy.6 maint-buggy.6

scoredir = $(sharedstatedir)/moon-buggy
hpath.o: hpath.c moon.h Makefile
	$(COMPILE) -DSCORE_DIR=\"$(scoredir)\" -c $<

BUILT_SOURCES = copying.h
copying.h: COPYING text2c.sed
	cd $(srcdir) && sed -f text2c.sed $< >$@

install-exec-hook:
	if test "x@GAMEUSER@" != "x"; then \
	  chown "@GAMEUSER@" "$(DESTDIR)/$(bindir)/moon-buggy" \
	    && chmod u+s "$(DESTDIR)/$(bindir)/moon-buggy" ; \
	fi
	chmod go-x "$(DESTDIR)/$(bindir)/maint-buggy"

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(scoredir)

install-data-hook:
	if test "x@GAMEUSER@" != "x"; then \
	  chown "@GAMEUSER@" "$(DESTDIR)/$(scoredir)" \
	    && { test ! -e "$(DESTDIR)/$(scoredir)/mbscore" \
		 || chown "@GAMEUSER@" "$(DESTDIR)/$(scoredir)/mbscore" ; } \
	    && { test ! -e "$(DESTDIR)/$(scoredir)/mbscore" \
		 || chmod go-w "$(DESTDIR)/$(scoredir)/mbscore" ; } \
	    && chmod go-w "$(DESTDIR)/$(scoredir)" ; \
	fi

uninstall-local:
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
