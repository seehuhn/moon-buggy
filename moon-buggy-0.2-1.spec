Summary: drive a strange car across the moon
Name: moon-buggy
Copyright: GPL
Group: Games
Version: 0.2
Release: 1
Packager: Jochen Voﬂ <voss@mathematik.uni-kl.de>
Source: moon-buggy-0.2.tar.gz
Icon: moon-buggy.gif
%description
moon-buggy is a simple, curses-based game, where you drive some kind
of car across the surface of the moon.  Unfortunately there are
dangerous craters there.  Fortunately your car can jump!  So just go
on, and jump over the craters (using the space key).

%prep
%setup

%build
CFLAGS="$RPM_OPT_FLAGS" LDFLAGS=-s configure --prefix=/usr/games --sharedstatedir=/var/games
make

%install
make install
{ chown games /usr/games/bin/moon-buggy \
    && chmod u+s /usr/games/bin/moon-buggy \
    && chown games /var/games/moon-buggy; } || true
rm -f /var/games/moon-buggy/mbscore

%post
{ cd /usr/games/info && install-info mbuggy.info dir; } || true

%preun
rm -f /var/games/moon-buggy/mbscore
{ cd /usr/games/info && install-info --delete mbuggy.info dir; } || true

%files
/usr/games/bin/moon-buggy
/usr/games/info/mbuggy.info
/usr/games/man/man6/moon-buggy.6
/var/games/moon-buggy
