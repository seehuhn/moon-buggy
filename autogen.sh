#! /bin/sh
# autogen.sh - recreate automatically generated files
# $Id: autogen.sh,v 1.3 1999/05/30 19:54:39 voss Exp $
aclocal
autoconf
autoheader
automake -a
