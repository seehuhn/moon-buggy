#! /bin/sh
# autogen.sh - recreate automatically generated files
# $Id: autogen.sh,v 1.6 1999/06/25 23:26:06 voss Rel $
aclocal
autoconf
autoheader
touch uninst.pre.in
automake -a
echo "warning: after configure run, remove empty \"uninst.pre.in\""
