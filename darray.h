/* darray.h - implement dynamically growing arrays
 *
 * Copyright (C) 1998  Jochen Voss.
 *
 * $Id: darray.h,v 1.1 1999/05/07 18:39:55 voss Exp $ */

#ifndef FILE_DARRAY_H_SEEN
#define FILE_DARRAY_H_SEEN

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
	(x).slots = DA_INC; \
	(x).data = (type*)DA_MALLOC((x).slots*sizeof(type)); \
	(x).used = 0;

#define DA_ADD(x,type,val) \
	if ((x).used >= (x).slots) { \
	  (x).slots += DA_INC; \
	  (x).data = (type*)DA_REALLOC ((x).data, \
					(x).slots*sizeof(type)); \
	} \
	(x).data[(x).used] = val; \
	(x).used += 1;

#define DA_ADD_EMPTY(x,type,ptr) \
	if ((x).used >= (x).slots) { \
	  (x).slots += DA_INC; \
	  (x).data = (type*)DA_REALLOC ((x).data, \
					(x).slots*sizeof(type)); \
	} \
	ptr = (x).data+(x).used; \
	(x).used += 1;

#define DA_REMOVE(x,type,idx) \
	memmove((x).data+idx+1, (x).data+idx, ((x).used-idx-1)*sizeof(type)); \
	(x).used -= 1;

#endif /* FILE_DARRAY_H_SEEN */
