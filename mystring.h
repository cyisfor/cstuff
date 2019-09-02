#ifndef _MYSTRING_H_
#define _MYSTRING_H_

#include <string.h> // memcpy
#include <stdio.h> // snprintf
#include <stdlib.h> // malloc
#include <stdbool.h>

#define LITSIZ(...) (sizeof(__VA_ARGS__)-1)
#define LITLEN(...) (__VA_ARGS__), LITSIZ(__VA_ARGS__)

/* C makes this literally unusable. You cannot create a mutable structure that points
	 to const data. 
typedef struct cstring {
	const char* const s;
	const size_t l;
} cstring;
*/

#include "myint.h"

typedef struct string {
	const byte* base;
	size_t len;
} string;

/* C continues to suck, so we have to have a special 'non-const-string' class
   for when the base needs to be modified but not extended (bstring). We can't
   just define string as 'byte*' without const, because then we could only use
   const strings when we wanted const byte*, and C forbids assignment to
   a const value.

   i.e.

   const ncstring s = LITSTR("something");
   s.base = "somethingelse"; // error
   s = LITSTR("somethingelse"); // error: C sucks

   string s = { strdup("foo"), 3 };
   s.base[0] = 'b'; // error: C sucks
 */
typedef struct ncstring {
	byte* base;
	size_t len;
} ncstring;

typedef struct bstring {
	byte* base;
	size_t len;
	size_t space;
} bstring;


#define CSTRING(str) (*((const string*)&str)) // any kind of string
#define DEREFSTRING(str) (*((string*)&str)) // any kind of string, but may segfault
#define STRING(str) ((string){str.base, str.len}) // any kind of string
#define LITSTR(...) (const string){.base = (__VA_ARGS__), .len = LITSIZ(__VA_ARGS__)}
static
bstring bstringstr(const byte* str, size_t len) {
	byte* buf = malloc(len);
	memcpy(buf,str,len);
	return ((bstring) { .base = buf, .len = len, .space = len });
}

static string strlenstr(const byte* str) {
	size_t len = strlen(str);
	return (string) { .base = str, .len = len};
}

#define bstringlit(lit) bstringstr(LITLEN(lit))

#define BLOCK_SIZE 0x200

static
void strgrow(bstring* st, size_t newmin) {
	st->space = (st->space * 3)>>1;
	if(st->space < newmin)
		st->space = newmin;
	st->base = realloc(st->base, st->space);
}

static
void strreserve(bstring* st, size_t n) {
	// could say st.space = pow(1.5,log(st.l+n)/(log(3)-log(2))+1)
	// but that seems like a lot of expensive math calls, when the loop'll only happen like 4 times max
	while(st->len + n > st->space) strgrow(st,st->len+n);
}

static
void straddn(bstring* st, const byte* c, size_t n) {
	strreserve(st, n);
	memcpy(st->base + st->len, c, n);
	st->len += n;
}

#define stradd(st,lit) straddn(st,lit,sizeof(lit)-1)

static
void straddint(bstring* st, int i) {
	strreserve(st,0x10);
	st->len += snprintf(st->base + st->len, 0x10, "%x",i);
}

static
void straddstr(bstring* st, string s) {
	straddn(st, s.base, s.len);
}

static
void strrewind(bstring* st) {
	st->len = 0;
}

static
void strclear(bstring* st) {
	// can cheat, by setting ->base to static and ->space to 0
	if(st->space == 0) return;
	free(st->base);
	st->base = NULL;
	st->len = 0;
	st->space = 0;
}

static
const byte* ZSTR(const string st) {
	static byte* buf = NULL;
	if(st.base[st.len-1] == 0) return st.base;
	buf = realloc(buf, st.len+1);
	memcpy(buf, st.base, st.len);
	buf[st.len] = 0;
	return buf;
}

#define STRANDLEN(st) st.base, st.len
#include <assert.h>

#define STRING_FOR_PRINTF(st) ({ assert(st.len <= 0xFFFFFFFF); (unsigned int)st.len; }), st.base

#define NULL_STRING ((string){NULL,0})

#include "defer.h"

#define AUTO_BSTRING(name,value) bstring name = value; DEFER { strclear(&name); }

static
ncstring string_copy(const string str) {
	ncstring ret = {
		.base = malloc(str.len),
		.len = str.len
	};
	memcpy(ret.base, str.base, str.len);
	return ret;
}

#endif /* _MYSTRING_H_ */
