# text2c.sed - used by "Makefile.am" to create "copying.h"
# Copyright 1999  Jochen Voﬂ
# $Id: text2c.sed,v 1.3 1999/01/02 12:05:51 voss Rel $
s/\\/\\\\/g
s/"/\\"/g
s///g
s/^/  "/
s/$/",/
1 i\
/* copying.h - moon-buggy's copyright notice\
 * Automatically created from file "COPYING".\
 * DO NOT EDIT !!! */\
\
static const char *copying_lines [] = {
$ a\
};
