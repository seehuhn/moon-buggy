# merge.sed - used by "Makefile.am" to create "maint-buggy"
# Copyright 1999  Jochen Voss
# $Id: merge.sed,v 1.5 2000/04/26 20:31:24 voss Rel $
s/^#:notice$/# Generated automatically from maint.in by Makefile./g
/#:pre-uninstall/ r uninst.pre
