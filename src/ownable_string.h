#ifndef _OWNABLE_STRING_H_
#define _OWNABLE_STRING_H_

#include <string.h> // memcpy
#include <stdio.h> // snprintf
#include <stdlib.h> // malloc
#include <stdbool.h>

#define LITSIZ(a) (sizeof(a)-1)
#define LITLEN(a) a, LITSIZ(a)

#ifndef N
#  ifndef namespace
#  define namespace ownable_
#  endif

#  include "concatsym.h"

#  define N(a) CONCATSYM(namespace,a)
#endif

typedef struct N(string) {
	const char* s;
	size_t l;
	bool owned; // s is owned by us.
} N(string);

static N(string) N(zstring)(const N(string) str) {
	N(string) ret = {};
	if(str.s[str.l-1] == '\0') {
		ret.s = str.s;
		ret.l = str.l;
		ret.owned = false;
	} else {
		char* copy = malloc(str.l+1);
		memcpy(copy,str.s,str.l);
		copy[str.l] = '\0';
		ret.s = copy;
		ret.l = str.l+1;
		ret.owned = true;
	}
	return ret;
}

static N(string) N(use)(N(string)* str) {
	
	if(str->owned) {
		return str;
	}
	char* buf = malloc(str.l);
	memcpy(buf,str.s,str.l);
	return ((ownablestring) {
		.s = buf,
		.l = str.l,
		.owned = true
	});
}

static void ownable_string_free(ownablestring* str) {
	if(str->owned) return;
	const char* s = str->s;
	str->s = NULL;
	str->l = 0;
	str->owned = false;
	free((char*)s);
}

#define ownable_string_disown(str) str.owned = false

/*
	Because llvm sucks, and rust sucks.

	pass around ownable copies of memory blocks
	copy memory when ownership is needed
	if you pass an ownable string to a function which saves the string globally,
	  unset ownership in the parent.

	void foo(ownablestring str) {
	  g.thing = own_string(str);
	}
	void bar(ownablestring str) {
	  foo(str);
		ownable_string_disown(str); or str.owned = false;
		ownable_string_free(str); // won't screw up g.thing
	}

	doing disown isn't needed for a temporary ownablestring that will
	never be freed i.e. OSTRING(str)
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

//#undef string ehh... but in code that uses this we don't wanna specify "ownablestring"
#endif /* _MYSTRING_H_ */
