/* darray.h - implement dynamically growing arrays
 *
 * Copyright (C) 1998  Jochen Voss.
 */

#ifndef FILE_DARRAY_H_SEEN
#define FILE_DARRAY_H_SEEN

#include <string.h>

#ifndef DA_MALLOC
#define DA_MALLOC xmalloc
#endif /* not DA_MALLOC */
#ifndef DA_REALLOC
#define DA_REALLOC xrealloc
#endif /* not DA_REALLOC */
#ifndef DA_INC
#define DA_INC 4
#endif /* not DA_INC */

#define DA_INIT(x,type) \
        do { \
          (x).slots = DA_INC; \
          (x).data = (type*)DA_MALLOC((x).slots*sizeof(type)); \
          (x).used = 0; \
        } while(0)

#define DA_CLEAR(x) \
        (x).used = 0

#define DA_ADD(x,type,val) \
        do { \
          if ((x).used >= (x).slots) { \
            (x).slots += DA_INC; \
            (x).data = (type*)DA_REALLOC ((x).data, \
                                          (x).slots*sizeof(type)); \
          } \
          (x).data[(x).used] = val; \
          (x).used += 1; \
        } while(0)

#define DA_ADD_EMPTY(x,type,ptr) \
        do { \
          if ((x).used >= (x).slots) { \
            (x).slots += DA_INC; \
            (x).data = (type*)DA_REALLOC ((x).data, \
                                          (x).slots*sizeof(type)); \
          } \
          ptr = (x).data+(x).used; \
          (x).used += 1; \
        } while(0)

#define DA_REMOVE(x,type,idx) \
        do { \
          int _idx = (idx); \
          memmove((x).data+_idx, (x).data+_idx+1, \
                  ((x).used-_idx-1)*sizeof(type)); \
          (x).used -= 1; \
        } while(0)

#define DA_REMOVE_VALUE(x,type,val) \
        do { \
          int _i = 0; \
          while (_i < (x).used) { \
            if ((x).data[_i] == (val)) { \
              memmove((x).data+_i, (x).data+_i+1, \
                      ((x).used-_i-1)*sizeof(type)); \
              (x).used -= 1; \
            } else { \
              ++_i; \
            } \
          } \
        } while(0)

#endif /* FILE_DARRAY_H_SEEN */
