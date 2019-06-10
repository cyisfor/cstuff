#ifndef _OWNABLE_STRING_H_
#define _OWNABLE_STRING_H_

/*
	Because llvm sucks, and rust sucks.

	pass around freeable copies of memory blocks
	memdup when permanence is needed

	Try to avoid struct copying this i.e.
	void foo(ownable_string bar) { ... } foo(baz);
	unless you're sure it's either transient or constant. The copy passed to foo
	should be implicitly made transient, if the parent one is freeable.
	how to do this: foo(ownable_copy(baz));

	If you avoid struct copying though, you can transfer ownership, and
	avoid transient strings entirely.
	i.e.
	void foo(ownable_string* bar);

	To save a transient string, you have to strdup a copy of it, that's now
	freeable. So if you insist on struct copying, you'll have an freeable string,
	then downgrade it to transient to pass it by value, then save that transient
	string creating a second freeable string, then deallocating the first.
	
	Without struct copying, you have an freeable string, pass its address, and
	now the string is NULL in the parent, and the one saved didn't have to malloc
	anything.
*/

#include <string.h> // memcpy
#include <stdio.h> // snprintf
#include <stdlib.h> // malloc
#include <stdbool.h>

#ifndef N
// "#define namespace" to have no namespace (i.e. string)
#  ifndef namespace
#  define namespace ownable_
#  endif

#  include "concatsym.h"

#  define N(a) CONCATSYM(namespace,a)
#endif

enum N(state) {
	N(CONSTANT), // we know the string will never be freed
		N(TRANSIENT), // we have to make sure it isn't freed elsewhere
		N(FREEABLE) // we can free it here, and it's not used elsewhere.
		};

typedef struct N(string) {
	const char* s;
	size_t l;
	enum N(state) state;
} N(string);

static void N(nullendify)(const N(string)* str) {
	/* Convert a string to be null terminated, reallocing if necessary  */
	if(str->s[str->l-1] == '\0') {
		return;			
	}
	// we need to make a copy of it regardless, to make room for the \0
	char* copy;
	if(str->state == N(FREEABLE)) {
		// WARNING: this will make any transient strings of this one point to
		// freed memory!
		copy = realloc(str->s, str->l+1);
	} else {
		copy = malloc(str->l+1);
		memcpy(copy,str.s,str.l);
	}
	copy[str.l] = '\0';
	if(str->state == N(FREEABLE)) {
		free(str->s);
	}
	str->s = copy;
	str->l += 1;
}

static void N(ensure)(const N(string)* str) {
	/* ensure we do not have a transient string, so return one that's either
		 freeable, constant, or a freeably copy of the transient one.
		 */
	if(str->state != N(TRANSIENT)) return;

	char* buf = malloc(str->l);
	memcpy(buf,str->s,str->l);
	str->s = buf;
	str->state = N(FREEABLE);
}

static N(string) N(take)(N(string)* str) {
	N(ensure)(str);
	N(string) ret = *str;
	str->s = NULL;
	str->l = 0;
	str->state = N(CONSTANT);
	return ret;
}

static N(string) N(copy)(const N(string) str) {
	N(string) ret = str;
	if(ret.state == N(FREEABLE)) {
		ret.state = N(TRANSIENT);
	}
	return ret;
}

static void N(clear)(N(string)* str) {
	if(str->state == N(FREEABLE)) {
		free((char*)str->s);
	}
	str->s = NULL;
	str->l = 0;
	str->state = N(CONSTANT);
}

static void N(replace)(N(string)* dest, const N(string) src) {
	if(dest->s != src.s) { // XXX: will this screw things up sometimes?
		char* destbuf;
		if(dest->state == N(FREEABLE)) {
			destbuf = realloc((char*)dest->s, src.l);
		} else {
			dest->state = N(FREEABLE);
			destbuf = malloc(src.l);
		}
		memcpy(destbuf, src.s, src.l);
		dest->s = destbuf;
	}
	dest->l = src.l;
}

/* so like

void foo(void) {
	ownable_string bar1 = {
		.s = "unownable string",
		.l = 23,
		.state = N(CONSTANT)
	}, bar2 = {
		.s = strdup("derp"),
		.l = 5,
		.state = N(FREEABLE)
	};
	ownable_string bar3 = ownable_copy(bar2);
	foobar(bar1,&bar2,bar3);
	assert(bar2.s == NULL);
	ownable_clear(&bar1);
	ownable_clear(&bar3);

	ownable_clear(&g.bar2);
}

void foobar(ownable_string bar1, ownable_string* bar2, ownable_string bar3) {
	g.bar1 = ownable_copy(bar1);
	ownable_ensure(&g.bar1);
	g.bar2 = ownable_take(bar2);
	g.bar3 = ownable_copy(bar3);
	ownable_ensure(&g.bar3);
	
	...
}

*/

#ifdef namespace
#undef namespace
#endif
#endif /* _MYSTRING_H_ */
