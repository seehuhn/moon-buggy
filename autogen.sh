#! /bin/sh
# autogen.sh - recreate automatically generated files
# $Id: autogen.sh,v 1.1 1999/05/08 12:45:57 voss Exp $
aclocal
autoconf
autoheader
automake -a
ln -s /home/voss/src/library/darray.h .
