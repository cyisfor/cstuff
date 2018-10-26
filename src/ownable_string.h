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

#define LITSIZ(a) (sizeof(a)-1)
#define LITLEN(a) a, LITSIZ(a)

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

static N(string) N(take)(N(string)* str) {
	N(string) ret = *src;
	str->s = NULL;
	str->l = 0;
	str->state = N(CONSTANT);
	return ret;
}

static N(string) N(copy)(N(string) str) {
	N(str) ret = str;
	if(ret.state == N(FREEABLE)) {
		ret.state = N(TRANSIENT);
	}
	return ret;
}

static N(string) N(ensure)(N(string) str) {
	if(str.state != N(TRANSIENT)) return str;
	
	char* buf = malloc(str.l);
	memcpy(buf,str.s,str.l);
	return ((N(string)) {
		.s = buf,
		.l = str.l,
		.state = N(FREEABLE)
	});
}

static void N(clear)(N(string)* str) {
	if(str->state == N(FREEABLE)) {
		free(str->s);
	}
	str->s = NULL;
	str->l = 0;
	str->state = N(CONSTANT);
}




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
	ownable_string bar3 = 
	foobar(bar);
	ownable_string_clear(&bar);
}

void foobar(ownable_string bar) {
	ownable_string foobar = ownable_ensure(bar);
	g.foobar = foobar;
	...
}
/* so like
 */

#define CSTRING(str) (*((const string*)&str)) // any kind of string
#define STRING(str) (*((string*)&str)) // any kind of string, but may segfault
#define OSTRING(str) ((ownablestring){ .s = str.s, .l = str.l, .owned = false })
static
bstring bstringstr(const char* s, size_t n) {
	char* buf = malloc(n);
	memcpy(buf,s,n);
	return ((bstring) { .s = buf, .l = n, .space = n });
}

#define bstringlit(lit) bstringstr(LITLEN(lit))

#define BLOCK_SIZE 0x200

static
void strgrow(bstring* st, size_t newmin) {
	st->space = (st->space * 3)>>1;
	if(st->space < newmin)
		st->space = newmin;
	st->s = realloc(st->s, st->space);
}

static
void strreserve(bstring* st, size_t n) {
	// could say st.space = pow(1.5,log(st.l+n)/(log(3)-log(2))+1)
	// but that seems like a lot of expensive math calls, when the loop'll only happen like 4 times max
	while(st->l + n > st->space) strgrow(st,st->l+n);
}

static
void straddn(bstring* st, const char* c, size_t n) {
	strreserve(st, n);
	memcpy(st->s + st->l, c, n);
	st->l += n;
}

#define stradd(st,lit) straddn(st,lit,sizeof(lit)-1)

static
void straddint(bstring* st, int i) {
	strreserve(st,0x10);
	st->l += snprintf(st->s + st->l, 0x10, "%x",i);
}

static
void strrewind(bstring* st) {
	st->l = 0;
}

static
void strclear(bstring* st) {
	free(st->s);
	st->s = NULL;
	st->l = 0;
	st->space = 0;
}

#define STRANDLEN(st) st.s, st.l

#ifdef namespace
#undef namespace
#endif
#endif /* _MYSTRING_H_ */
