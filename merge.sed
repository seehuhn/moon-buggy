# merge.sed - used by "Makefile.am" to create "maint-buggy"
# Copyright 1999  Jochen Voﬂ
# $Id: merge.sed,v 1.2 1999/01/02 12:17:54 voss Rel $
s/^#:notice$/# Generated automatically from maint.in by Makefile./g
/#:pre-uninstall/ r uninst.pre
/#:post-uninstall/ r uninst.post
