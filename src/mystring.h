#ifndef _MYSTRING_H_
#define _MYSTRING_H_

#include <string.h> // memcpy
#include <stdio.h> // snprintf
#include <stdlib.h> // malloc
#include <stdbool.h>

#define LITSIZ(a) (sizeof(a)-1)
#define LITLEN(a) a, LITSIZ(a)

/* C makes this literally unusable. You cannot create a mutable structure that points
	 to const data. 
typedef struct cstring {
	const char* const s;
	const size_t l;
} cstring;
*/

typedef struct string {
	const char* base;
	size_t len;
} string;

typedef struct bstring {
	char* base;
	size_t len;
	size_t space;
} bstring;


#define CSTRING(str) (*((const string*)&str)) // any kind of string
#define STRING(str) (*((string*)&str)) // any kind of string, but may segfault
#define LITSTR(lit) (string){.base = lit, .len = LITSIZ(lit)}
static
bstring bstringstr(const char* str, size_t len) {
	char* buf = malloc(len);
	memcpy(buf,str,len);
	return ((bstring) { .base = buf, .len = len, .space = len });
}

static string strlenstr(const char* str) {
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
void straddn(bstring* st, const char* c, size_t n) {
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
const char* ZSTR(const string st) {
	static char* buf = NULL;
	if(st.base[st.len-1] == 0) return st.base;
	buf = realloc(buf, st.len+1);
	memcpy(buf, st.base, st.len);
	buf[st.len] = 0;
	return buf;
}

#define STRANDLEN(st) st.base, st.len

#endif /* _MYSTRING_H_ */
