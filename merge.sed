# merge.sed - used by "Makefile.am" to create "maint-buggy"
# Copyright 1999  Jochen Voss
# $Id: merge.sed,v 1.4 1999/06/25 23:14:05 voss Rel $
s/^#:notice$/# Generated automatically from maint.in by Makefile./g
/#:pre-uninstall/ r uninst.pre
