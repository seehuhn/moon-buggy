# merge.sed - used by "Makefile.am" to create "maint-buggy"
# Copyright 1999  Jochen Voﬂ
# $Id: merge.sed,v 1.3 1999/06/02 07:03:01 voss Exp $
s/^#:notice$/# Generated automatically from maint.in by Makefile./g
/#:pre-uninstall/ r uninst.pre
