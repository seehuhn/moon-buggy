Summary: drive a strange car across the moon
Name: moon-buggy
Copyright: GPL
Group: Games
Version: 0.3
Release: 1
Packager: Jochen Voﬂ <voss@mathematik.uni-kl.de>
Source: moon-buggy-0.3.tar.gz
Icon: moon-buggy.gif
%description
moon-buggy is a simple, curses-based game, where you drive some kind
of car across the surface of the moon.  Unfortunately there are
dangerous craters there.  Fortunately your car can jump!  So just go
on, and jump over the craters.

%prep
%setup

%build
CFLAGS="$RPM_OPT_FLAGS" LDFLAGS=-s configure --sharedstatedir=/var/games --with-suid=games
make

%install
make install
rm -f /var/games/moon-buggy/mbscore

%post
if /bin/sh -c 'install-info --version | sed 1q | fgrep -s -v -i debian' >/dev/null 2>&1; then \
  for file in mbuggy.info; do \
    echo " install-info --info-dir=/usr/local/info /usr/local/info/$file";\
    install-info --info-dir=/usr/local/info /usr/local/info/$file || :;\
  done; \
else : ; fi
if test "xgames" != "x"; then \
  chown "games" "//var/games/moon-buggy" \
    && { test ! -e "//var/games/moon-buggy/mbscore" \
         || chown "games" "//var/games/moon-buggy/mbscore" ; } \
    && { test ! -e "//var/games/moon-buggy/mbscore" \
         || chmod go-w "//var/games/moon-buggy/mbscore" ; } \
    && chmod go-w "//var/games/moon-buggy" ; \
fi

%preun
cd /usr/local/info; \
if /bin/sh -c 'install-info --version | sed 1q | fgrep -s -v -i debian' >/dev/null 2>&1; then \
  ii=yes; \
else ii=; fi; \
for file in mbuggy.info; do \
  test -z "i" \
    || install-info --info-dir=/usr/local/info --remove $file; \
done

%files
/usr/local/bin/moon-buggy
/usr/local/bin/maint-buggy
/usr/local/info/mbuggy.info
/usr/local/man/man6/moon-buggy.6
/usr/local/man/man6/maint-buggy.6
/var/games/moon-buggy
