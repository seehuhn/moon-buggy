#! /bin/sh
# autogen.sh - recreate automatically generated files
# $Id: autogen.sh,v 1.2 1999/05/08 13:03:06 voss Rel $
aclocal
autoconf
autoheader
automake -a
ln -sf /home/voss/src/library/darray.h .
