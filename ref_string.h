#ifndef _REF_STRING_H_
#define _REF_STRING_H_

/*
	pass around freeable copies of memory blocks
	refcounted to prevent deletion if still being used
	NOT thread safe
	avoid struct copying
*/

#include <string.h> // memcpy
#include <stdio.h> // snprintf
#include <stdlib.h> // malloc
#include <stdbool.h>

#ifndef N
// "#define namespace" to have no namespace (i.e. string)
#  ifndef namespace
#  define namespace ref_
#  endif

#  include "concatsym.h"

#  define N(a) CONCATSYM(namespace,a)
#endif

#define REF_IS_STATIC 0
#define REF_IS_ONLY_REF 1
/* refcounter is not 1 high, because a non-static ref will be converted to
   static when its refs goes to 0, setting base to NULL
*/

typedef struct N(string) {
	char* s;
	size_t l;
	unsigned int refs; 		/* do we ever need more than 4 billion refs? */
} *N(string);

/* str MAY be mutated (if no other refs to it)
   will always return a string ending in \0
*/
static N(string) N(nullendify)(N(string) str) {
	if(str->refs == REF_IS_STATIC) {
		/* this is the only time str->base may be null */
		if(str->base == NULL) return str;
	}
	/* Convert a string to be null terminated, reallocing if necessary  */
	if(str->base[str->len-1] == '\0') {
		return str;
	}
	// we need to make a copy of it regardless, to make room for the \0
	char* copy;
	if(str->refs == REF_IS_ONLY_REF) {
		copy = realloc(str->base, str->len+1);
	} else {
		copy = malloc(str->len+1);
		memcpy(copy,str->base,str->len);
		if(str->refs != REF_IS_STATIC) {
			N(unref)(str);
		}
		N(string)* new = malloc(sizeof(struct N(string)));
		new->refs = 1;
		new->len = str->len;
		str = new;
		/* we can't mutate the argument anymore... */
	}
	copy[str->len] = '\0';
	str->base = copy;
	str->len += 1;
	return str;
}

static N(string) N(copy)(N(string) str) {
	if(str->refs != REF_IS_STATIC) {
		++str->refs;
	}
	return str;
}

static void N(unref)(N(string) str) {
	if(str->refs == REF_IS_STATIC) {
		record(WARNING, "Unref a static string? %.*s",
			   STRING_FOR_PRINTF(*str));
		return;
	}
	assert(str->base != NULL);
	--str->refs;
	if(str->refs == REF_IS_STATIC) {
		free(str->base);
	}
	str->len = 0;
}

#ifdef namespace
#undef namespace
#endif
#undef N
#endif /* _MYSTRING_H_ */
