#! /bin/sh
# autogen.sh - recreate automatically generated files
# $Id: autogen.sh,v 1.4 1999/06/02 07:07:16 voss Exp $
aclocal
autoconf
autoheader
automake -a
touch uninst.pre.in
