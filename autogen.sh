#! /bin/sh
# autogen.sh - recreate automatically generated files
# $Id: autogen.sh,v 1.5 1999/06/05 13:54:19 voss Exp $
aclocal
autoconf
autoheader
touch uninst.pre.in
automake -a
