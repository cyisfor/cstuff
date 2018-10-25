#ifndef _MYSTRING_H_
#define _MYSTRING_H_

#include <string.h> // memcpy
#include <stdio.h> // snprintf
#include <stdlib.h> // malloc
#include <stdbool.h>

#define LITSIZ(a) (sizeof(a)-1)
#define LITLEN(a) a, LITSIZ(a)

/* C makes this almost unusable. You cannot create a mutable structure that points
	 to const data. */
typedef struct cstring {
	const char* const s;
	const size_t l;
} cstring;

typedef struct string {
	const char* s;
	size_t l;
} string;

typedef struct bstring {
	char* s;
	size_t l;
	size_t space;
} bstring;

typedef struct ownablestring {
	const char* s;
	size_t l;
	bool owned; // s is owned by us.
	// note, can't be cast to/from a bstring!
} ownablestring;

static ownablestring ownable_zstring(const string str) {
	ownablestring ret = {};
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

static ownablestring own_string(ownablestring str) {
	if(str.owned) {
		return str;
	}
	ownablestring ret = {
		.s = malloc(str.l),
		.l = str.l,
		.owned = true
	};
	memcpy(ret.s,str.s,str.l);
	return ret;
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
		str.owned = false;
*/
		

#define CSTRING(str) (*((const string*)&str)) // any kind of string
#define STRING(str) (*((string*)&str)) // any kind of string, but may segfault

static
bstring bstringstr(const char* s, size_t n) {
	bstring ret = {
		.s = malloc(n),
		.l = n,
		.space = n
	};
	memcpy(ret.s,s,n);
	return ret;
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


#endif /* _MYSTRING_H_ */
