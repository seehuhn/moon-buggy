#! /bin/sh
# autogen.sh - recreate automatically generated files
# $Id: autogen.sh,v 1.7 2000/04/26 20:43:35 voss Rel $
aclocal
autoconf
autoheader
automake -a
