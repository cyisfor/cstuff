#ifndef _OWNABLE_STRING_H_
#define _OWNABLE_STRING_H_

/*
	Because llvm sucks, and rust sucks.

	pass around freeable copies of memory blocks
	memdup when permanence is needed
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

enum N(state) { N(CONSTANT), N(TRANSIENT), N(FREEABLE) };

typedef struct N(string) {
	const char* s;
	size_t l;
	enum N(state) state;
} N(string);

static N(string) N(zstring)(const N(string) str) {
	if(str.s[str.l-1] == '\0') {
		N(string) ret = str;
		if(ret.state = N(FREEABLE))
			ret.state = N(TRANSIENT);
		return ret;
			
	} 
	N(string) ret;
	char* copy = malloc(str.l+1);
	memcpy(copy,str.s,str.l);
	copy[str.l] = '\0';
	ret.s = copy;
	ret.l = str.l+1;
	ret.state = N(FREEABLE);
	return ret;
}


static N(string) N(ensure)(const N(string) str) {
	if(str.state != N(TRANSIENT)) return str;
	
	char* buf = malloc(str.l);
	memcpy(buf,str.s,str.l);
	return ((N(string)) {
		.s = buf,
		.l = str.l,
		.state = N(FREEABLE)
	});
}

static N(string) N(take)(N(string)* str) {
	N(string) ret = *str;
	if(ret.state == N(TRANSIENT)) {
		ret = N(ensure)(ret);
	}
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
	ownable_string_clear(&bar1);
	ownable_string_clear(&bar3);
	// we must not free bar2.s until all transient copies are unused
	// since foobar saves an ownable_copy without ownable_ensure,
	// we can "never" ownable_string_clear(&bar2); until..

	ownable_string_clear(&g.bar3);
	ownable_string_clear(&bar2);
}

void foobar(ownable_string bar1, ownable_string* bar2, ownable_string bar3) {
	g.bar1 = ownable_ensure(bar1);
	g.bar2 = ownable_take(bar2);
	g.bar3 = ownable_copy(bar3);
	
	...
}

*/

#ifdef namespace
#undef namespace
#endif
#endif /* _MYSTRING_H_ */
