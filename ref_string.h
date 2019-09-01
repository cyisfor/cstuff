#ifndef _REF_STRING_H_
#define _REF_STRING_H_

/*
	pass around freeable copies of memory blocks
	refcounted to prevent deletion if still being used
	NOT thread safe
	avoid struct copying
*/

#include "mystring.h"

#include <string.h> // memcpy
#include <stdio.h> // snprintf
#include <stdlib.h> // malloc
#include <stdbool.h>

#ifndef N
#  define namespace rstring

#  include "concatsym.h"

#  define N(a) CONCATSYM(CONCATSYM(namespace,_),a)
#  define T namespace
#endif

#define REF_IS_STATIC 0
#define REF_IS_ONLY_REF 1
/* refcounter is not 1 high, because a non-static ref will be converted to
   static when its refs goes to 0, setting base to NULL
   base should never be NULL if refs is not 0
*/

typedef struct T {
	byte* s;
	size_t l;
	unsigned int refs; 		/* do we ever need more than 4 billion refs? */
} *T;

static
T N(from_C)(const byte* base, size_t len) {
	T ret = malloc(str.l);
	memcpy(ret->base, str.base, str.l);
	ret->refs = 1;
	return ret;
}

static
T N(from)(const string str) {
	return N(from_C)(str.base, str.len);
}

static
struct T N(from_static_f)(byte* base, size_t len) {
	struct T str = {
		.base = base,
		.len = len
		.refs = REF_IS_STATIC
	};
	return str;
}

T N(unallocated)(size_t len) {
	T str = malloc(sizeof(struct T));
	str.base = malloc(len);
	str.len = len;
	str.refs = 1;
	return str;
}

/* ugh, cpp... */
#if namespace == rstring
#define rstring_static(lit) rstring_from_static_f(LITLEN(lit))
#define rstring_get_obj(str, type) ({ assert(str->len == sizeof(type)); (type*)str->base; })

#define rstring_from_obj_2(tempvar, obj) ({ \
	rstring tempvar = rstring_unallocated(sizeof(obj));	\
	memcpy(&tempvar.base, &obj, sizeof(obj)); \
	tempvar;								  \
	})
#define rstring_from_obj(obj) rstring_from_obj_2(tempvar ## __COUNTER, obj)
#else
#error we can't do this without macros, sorry.
static struct T N(static)(byte* lit) {
	return N(from_static_f)(lit, strlen(lit));
}
#endif



/* will always return a string ending in \0
   str MAY be mutated (if no other refs to it)
*/
static
T N(nullendify)(T str) {
	if(str->refs == REF_IS_STATIC) {
		/* this is the only time str->base may be null */
		if(str->base == NULL) return str;
	}
	/* Convert a string to be null terminated, reallocing if necessary  */
	if(str->base[str->len-1] == '\0') {
		return str;
	}
	// we need to make a copy of it regardless, to make room for the \0
	byte* copy;
	if(str->refs == REF_IS_ONLY_REF) {
		str->base = realloc(str->base, ++str->len);
	} else {
		T* new = malloc(sizeof(struct T));
		new->base = malloc(str->len+1);
		memcpy(new->base,str->base,str->len);
		if(str->refs != REF_IS_STATIC) {
			N(unref)(str);
		}
		new->refs = 1;
		new->len = str->len+1;
		str = new;
		/* we can't mutate the argument anymore... */
	}
	str->base[str->len-1] = '\0';
	return str;
}

static
T N(copy)(T str) {
	if(str->refs != REF_IS_STATIC) {
		++str->refs;
	}
	return str;
}

static
void N(unref)(T str) {
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
