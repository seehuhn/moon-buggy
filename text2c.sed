# text2c.sed - used by "Makefile.am" to create "copying.h"
# Copyright 1999  Jochen Voss
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
