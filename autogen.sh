#! /bin/sh
# autogen.sh - recreate automatically generated files
aclocal
autoconf
autoheader
automake -a
