# merge.sed - used by "Makefile.am" to create "maint-buggy"
# $Id: merge.sed,v 1.1 1999/01/01 19:13:43 voss Exp $
s/^#:notice$/# Generated automatically from maint.in by Makefile./g
/#:pre-uninstall/ r uninst.pre
/#:post-uninstall/ r uninst.post
